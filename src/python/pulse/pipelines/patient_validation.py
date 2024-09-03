# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import sys
import logging
import argparse
from pathlib import Path
from typing import List, Optional, Union

from pulse.cdm.engine import eEngineInitializationState
from pulse.cdm.scenario import eScenarioExecutionState, SEScenarioExecStatus
from pulse.cdm.validation import SEPatientTimeSeriesValidation
from pulse.pipelines.dataset.timeseries_dataset_reader import gen_patient_targets
from pulse.pipelines.validation.timeseries_validation import (
    validate, generate_validation_tables,
    gen_expected_str, gen_engine_val_str)
from pulse.cdm.io.scenario import serialize_scenario_exec_status_list_from_file
from pulse.cdm.io.validation import (
    serialize_patient_time_series_validation_to_file,
    serialize_patient_time_series_validation_list_to_file)


_pulse_logger = logging.getLogger('pulse')


def timeseries_validation_pipeline(
        log_file: Path,
        csv_file: Path,
        table_dir: Optional[Path] = None,
        out_file: Optional[Path] = None,
        assessment_files: Optional[list] = None
) -> SEPatientTimeSeriesValidation:
    """
    Processes given log and csv file through the timeseries validation
    pipeline.

    :param log_file: Path to log file.
    :param csv_file: Path to csv results file.
    :param table_dir: (Optional) Path to table output directory.
    :param out_file: (Optional) Generated targets will be serialized here.

    :return: Evaluated targets
    """

    patient_validation = gen_patient_targets(log_file=log_file)
    if patient_validation is None:
        _pulse_logger.error("Unable to generate patient targets")
        return False

    validate(patient_validation=patient_validation,
             csv_filename=csv_file,
             assessment_files=assessment_files,
             output_file=out_file)

    # Generate Tables (Optional)
    if table_dir is not None:
        generate_validation_tables(target_map=patient_validation, table_dir=table_dir)

    # Write the evaluated validation out (Optional)
    if out_file is not None:
        _pulse_logger.info(f"Writing {out_file}")
        out_file.parent.mkdir(parents=True, exist_ok=True)
        serialize_patient_time_series_validation_to_file(patient_validation, out_file)

    return patient_validation


def bulk_timeseries_validation_pipeline(
        filename_base_paths: Optional[List[Path]] = None,
        out_file: Path = None,
        table_dir: Optional[Union[Path, List[Optional[Path]]]] = None,
        serialize_per_file: bool = False
) -> list:
    """
    Processes bulk list of files through timeseries validation pipeline to
    one output file, optionally saving individual validation tables. If an
    exception is raised during execution, an attempt will be made to serialize
    the current list of targets.

    :param filename_base_paths: List of file base paths to process. ".log" and
        ".csv" will be appended to these paths to locate log and results files.
    :param out_file: Generated targets will be serialized to this file.
    :param table_dir: (Optional) Path or list of paths indicating where tables
        should be generated for the corresponding filename_base_path. If only
        one path is provided, all tables will be generated in that directory.
        If None is provided (single or as list item), tables will not be
        generated.
    :param serialize_per_file: If true, each individual file's validation targets
        will be serialized to its own json file, in addition to the bulk file.
    """
    all_tgts = list()

    if not filename_base_paths:
        _pulse_logger.warning("No files to process for timeseries validation.")
        return all_tgts

    cnt = 0
    num_files = len(filename_base_paths)
    # If only one table dir provided, use the same table dir for every input file
    if not isinstance(table_dir, List):
        table_dirs = [table_dir] * num_files
    else:
        table_dirs = table_dir

    try:
        # Run the pipeline on each input file
        for filename_base, t_dir in zip(filename_base_paths, table_dirs):
            cnt += 1

            # Find all data files to validate
            log_file = filename_base.parent / f"{filename_base.name}.log"
            csv_file = filename_base.parent / f"{filename_base.name}.csv"
            sce_out_file = filename_base.parent / f"{filename_base.name}.json" if serialize_per_file else None
            # Look for any assessment json files
            json_files = filename_base.parent.glob('*.json')
            assessment_files = [f for f in json_files if "@" in f.name and filename_base.name in f.name]
            # Check if the csv file should have *Results
            if not csv_file.exists():
                csv_file = filename_base.parent / f"{filename_base.name}Results.csv"
                if not csv_file.exists():
                    _pulse_logger.error(f'[{cnt}/{num_files}] CSV file does not exist for {filename_base}')
                    continue

            _pulse_logger.info(f"[{cnt}/{num_files}] Validating : {csv_file}")
            tgts = timeseries_validation_pipeline(
                log_file=log_file,
                csv_file=csv_file,
                table_dir=t_dir,
                out_file=sce_out_file,
                assessment_files=assessment_files
            )
            all_tgts.append(tgts)
    except:
        raise
    finally:
        # Save current target map, even if something goes wrong
        if out_file is not None:
            out_file.parent.mkdir(parents=True, exist_ok=True)
            _pulse_logger.info(f"Writing {out_file}")
            serialize_patient_time_series_validation_list_to_file(all_tgts, out_file)
    return all_tgts


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
    logging.getLogger("pycel").setLevel(logging.WARNING)

    # Option to validate/generate tables for either the baseline/verification csv or the test_results
    parser = argparse.ArgumentParser(description="Process the full pipeline for timeseries validation")
    parser.add_argument(
        "-i", "--input",
        type=str,
        required=True,
        help="Perform patient validation on a results directory created by our development tools.\n"
             "  To do this, run from your install/bin and set this to either `verification` or `test_result`\n"
             "Perform patient validation on a specific csv/log file result using a filename base path.\n"
             "  E.g. path/to/file1 indicates path/to/file1.csv and path/to/file1.log should be processed."
             "  To do this, set this arg to the file name base path\n"
             "Perform patient validation on csv/log files provided in a ScenarioExecList json file\n"
             "  To do this, set this arg to the ScenarioExecList json file to be processed"
    )
    parser.add_argument(
        "-o", "--output-file",
        type=Path,
        default=None,
        help="Where generated targets should be serialized."
    )
    parser.add_argument(
        "-t", "--table-dir",
        type=Path,
        default=None,
        help="If provided, generate tables and write them in this directory."
    )
    parser.add_argument(
        "-s", "--serialize-per-file",
        action='store_true',
        help="Serialize generated targets into individual files (in addition to bulk serialization)."
    )
    opts = parser.parse_args()

    filename_base_paths = []
    out_file = opts.output_file
    table_dir = opts.table_dir
    serialize_per_file = opts.serialize_per_file
    if 'verification' == opts.input or 'test_results' == opts.input:
        mode = opts.input
        search = Path(mode + "/scenarios/validation/")

        files = search.glob('*.log')
        for file in files:
            filename_base_paths.append(Path(str(file.parent)+'/'+file.stem))
        if table_dir is None:
            table_dir = Path("./test_results/tables")
    elif '.json' in opts.input:
        statuses = list()
        serialize_scenario_exec_status_list_from_file(opts.input, statuses)
        for status in statuses:
            if (
                    status.get_initialization_state() != eEngineInitializationState.Initialized or
                    status.get_scenario_execution_state() != eScenarioExecutionState.Complete
            ):
                _pulse_logger.warning(
                    f"{status.get_scenario_filename()} results are incomplete. Skipping validation.")

            filename_base_paths.append(Path(status.get_csv_filename()).with_suffix(""))

        for category, value in SEScenarioExecStatus.summarize_exec_status_list(statuses).items():
            _pulse_logger.info(f"{category} Runs: {value}")
        if out_file is None:
            out_file = Path(opts.input).parent / f"{Path(opts.input).stem}Targets.json"
    else:
        filename_base_paths.append(opts.input)
        if out_file is None:
            out_file = opts.input.parent / f"{opts.input.stem}Targets.json"

    all_validation = bulk_timeseries_validation_pipeline(
        filename_base_paths=filename_base_paths,
        out_file=out_file,
        table_dir=table_dir,
        serialize_per_file=serialize_per_file
    )

    # Only write a html file for test results
    if opts.input == "test_results":
        # Push Standard patients to the front
        all_validation.insert(0, all_validation.pop(
            [idx for idx, tgt in enumerate(all_validation) if tgt.get_patient().get_name() == "StandardFemale"][0]))
        all_validation.insert(0, all_validation.pop(
            [idx for idx, tgt in enumerate(all_validation) if tgt.get_patient().get_name() == "StandardMale"][0]))
        html_file = "./test_results/PatientValidation.html"
        _pulse_logger.info(f"Writing {html_file}")
        f = open(html_file, "w")
        f.write("<html>\n")
        f.writelines("<body>\n")
        f.write("<h1>Patient Validation</h1>\n")
        for validation in all_validation:
            f.write("<br>\n")
            for type, tgts in validation.get_targets().items():
                f.writelines("<table border=\"1\">\n")
                f.write("<tr>")
                f.write("<th> " + validation.get_patient().get_name() + " " + type + " Validation </th>")
                f.write("<th> Expected Value </th>")
                f.write("<th> Engine Value </th>")
                f.write("<th> Percent Error </th>")
                f.write("<th> Notes </th>")
                f.write("</tr>\n")
                for tgt in tgts:
                    if tgt.get_error_value() <= 10.0:
                        f.write("<tr bgcolor=\"#00FF00\">")
                    elif tgt.get_error_value() <= 30.0:
                        f.write("<tr bgcolor=\"#FFFF00\">")
                    elif tgt.get_error_value() > 30.0:
                        f.write("<tr bgcolor=\"#FF0000\">")
                    else:
                        f.write("<tr bgcolor=\"#FFFFFF\">")
                    f.write("<td>" + tgt.get_header() + "</td>")
                    f.write("<td>" + gen_expected_str(tgt) + "</td>")
                    f.write("<td>" + gen_engine_val_str(tgt) + "</td>")
                    f.write(f"<td>{tgt.get_error_value():{tgt.get_table_formatting()}}</td>")
                    f.write("<td>" + tgt.get_notes() + "</td></tr>\n")
                f.write("</table><br>\n")
        f.write("</body>\n")
        f.write("</html>\n")
        f.close()
