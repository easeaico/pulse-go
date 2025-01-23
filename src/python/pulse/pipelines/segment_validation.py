# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import sys
import glob
import shutil
import logging
import argparse
import matplotlib.pyplot as plt
from enum import Enum
from pathlib import Path

from pulse.cdm.engine import eSwitch
from pulse.cdm.scenario import SEScenarioExecStatus
from pulse.cdm.io.scenario import serialize_scenario_exec_status_list_to_file
from pulse.cdm.validation import SESegmentValidationPipelineConfig
from pulse.cdm.io.validation import serialize_segment_validation_pipeline_config_from_file
from pulse.cdm.utils.markdown import process_file
from pulse.cdm.utils.file_utils import get_root_dir, get_validation_dir
from pulse.cdm.utils.plotter import create_plots, plot_with_test_results
from pulse.pipelines.dataset.segment_dataset_reader import gen_scenarios_and_targets
from pulse.pipelines.validation.segment_validation import validate
from pulse.engine.PulseScenarioExec import PulseScenarioExec

_pulse_logger = logging.getLogger('pulse')

class eExecOpt(Enum):
    GenerateOnly = 0
    SkipScenarioExecution = 1
    MarkdownOnly = 2
    Full = 3

# Pipeline Flow
#   1. Read the xlsx and generate scenario and validation target files
#    - ./validation/scenarios/xlsx filename/scenario tab name.json
#   2. Run the generated scenario(s)
#    - ./test_results/scenarios/xlsx filename/scenario name.(log||csv)
#   3. Generate validation tables
#   4. Generate any plots
#   5. Preprocess markdown file (inserts tables)
# Exec Options:
#   GenerateOnly = Step 1
#   SkipScenarioExecution = Steps 1,3,4,5
#   MarkdownOnly = Steps 3,4,5
#   Full = Steps 1,2,3,4,5


def segment_validation_pipeline(folder: Path, exec_opt: eExecOpt, use_test_results: bool = False) -> bool:
    if folder.is_dir():
        xls_dir = folder
    else:  # Must be inside the code base
        xls_dir = Path(get_root_dir()) / "data/human/adult/validation/Scenarios" / folder
    _pulse_logger.info(f"Processing {xls_dir}")
    if not xls_dir.is_dir():
        _pulse_logger.error("Could not find " + str(xls_dir))
        _pulse_logger.error("Please provide a valid folder")
        return False
    xls_file = xls_dir / (xls_dir.name + ".xlsx")
    if not xls_file.is_file():
        _pulse_logger.error("Could not find " + str(xls_file))
        _pulse_logger.error("Please provide a valid xls file")
        return False

    # This is where we will generate scenarios
    scenario_dir = Path("./validation/scenarios/"+xls_dir.name)
    # This is where we read the csv to validate
    validate_dir = Path("./verification/scenarios/"+xls_dir.name)
    # This is where the scenario should generate results
    test_results_dir = Path("./test_results/scenarios/" + xls_dir.name)

    scenario_dir.mkdir(parents=True, exist_ok=True)
    validate_dir.mkdir(parents=True, exist_ok=True)

    if exec_opt is eExecOpt.Full and not use_test_results:
        use_test_results = True
        _pulse_logger.info("Running full pipeline, switching to validate test results")
    if use_test_results:  # Let's validate what we generate
        validate_dir = test_results_dir
    elif not validate_dir.is_dir():
        _pulse_logger.error(f"Results directory ({validate_dir}) does not exist. Aborting")
        return False

    sce_ids = gen_scenarios_and_targets(xls_file, scenario_dir, test_results_dir)
    if exec_opt is eExecOpt.GenerateOnly:
        return True

    # plots and md files are expected to be:
    # 1. In the same directory as the xlsx file
    # 2. In the run.config root/docs/Validation directory
    # It is assumed the json and md file names are the same, just different extension

    md_files = []
    base_md = Path(xls_dir / f"{xls_dir.name}.md")
    if not base_md.is_file():
        base_md = Path(get_root_dir()) / "docs" / "Validation" / f"{xls_dir.name}.md"
    if base_md.is_file():
        md_files.append(base_md)
    # Look for any section md files
    extra_mds = xls_dir.glob(f"{xls_dir.name}_*.md")
    for extra_md in extra_mds:
        md_files.append(extra_md)
    # Find any other files that start with this name
    for sce_id in sce_ids:
        md_file = Path(get_root_dir()) / "docs" / "Validation" / f"{xls_dir.name}-{sce_id}.md"
        if not md_file.is_file():
            md_file = Path(get_root_dir()) / "docs" / "Validation" / f"{sce_id}.md"
            if not md_file.is_file():
                md_file = Path(xls_dir / f"{xls_dir.name}-{sce_id}.md")
                if not md_file.is_file():
                    md_file = None
        if md_file is not None:
            md_files.append(md_file)

    if len(md_files) == 0:
        _pulse_logger.error(f"Could not find md files, at least one should be in:")
        _pulse_logger.error(f"The same dir as the xlsx, or in your source/docs/Validation directory")
        sys.exit(1)
    config_file = Path(xls_dir / f"{xls_dir.name}.json")
    if not config_file.is_file():
        config_file = None
        _pulse_logger.info("No config file found")
    else:
        _pulse_logger.info(f"Using config file {config_file}")

    # Is there a custom bib file
    bib_file = Path(xls_dir / "Sources.bib")
    if bib_file.is_file():
        # Copy this file up as "Custom.bib"
        shutil.copyfile(xls_dir/"Sources.bib", "./docs/Custom.bib")
        # Are there any images in this directory
        images = glob.glob(str(xls_dir/'*'))
        for image in images:
            ext = [".jpg", ".png"]
            if image.endswith(tuple(ext)):
                image_dir = Path(f"./docs/html/Images/{xls_dir.name}")
                image_dir.mkdir(parents=True, exist_ok=True)
                shutil.copy(image, str(image_dir))

    # Run scenarios if we are running full pipeline
    if exec_opt is eExecOpt.Full:
        sce_exec = PulseScenarioExec()
        sce_exec.set_log_to_console(eSwitch.On)

        # Get list of all scenarios
        scenarios = [
            item.name for item in scenario_dir.glob("*.json")
            if not item.is_dir()
            and "-ValidationTargets.json" not in item.name
            and "-ExecStatus.json" not in item.name
            and "DataRequests.json" not in item.name
        ]

        # Create exec statuses for each scenario
        sce_list = []
        for scenario in scenarios:
            scenario_file = scenario_dir / scenario

            sce_status = SEScenarioExecStatus()
            sce_status.set_scenario_filename(scenario_file.as_posix())
            sce_list.append(sce_status)

        # Save statuses to file to send over to C++ for parallel execution
        sce_exec_list_file = scenario_dir / f"{xls_dir.name}-ExecStatus.json"
        serialize_scenario_exec_status_list_to_file(sce_list, sce_exec_list_file)
        sce_exec.set_scenario_exec_list_filename(sce_exec_list_file.as_posix())
        sce_exec.set_log_to_console(eSwitch.On)
        _pulse_logger.info("Executing scenarios")
        if not sce_exec.execute_scenario():
            _pulse_logger.warning(f"Scenarios not successfully run. Check {sce_exec_list_file} for details")
        else:
            _pulse_logger.info("Completed executing scenarios")

    config = None
    if config_file is not None:
        config = SESegmentValidationPipelineConfig()
        serialize_segment_validation_pipeline_config_from_file(config_file, config)

    # Carry out validation on each scenario
    targets = [item.name for item in scenario_dir.glob("*")
               if not item.is_dir() and "-ValidationTargets.json" in item.name]
    for target_file in targets:
        abs_targets_filename = Path(scenario_dir / target_file)
        abs_segments_filename = Path(validate_dir / target_file.replace("-ValidationTargets", "Results-Segments"))
        if not abs_segments_filename.exists():
            _pulse_logger.error(f"Unable to locate segments for {abs_segments_filename}. Continuing without validating.")
            continue

        table_dir = Path("./validation/tables/" + xls_dir.name + '/' + target_file.split('-')[0])
        table_dir.mkdir(parents=True, exist_ok=True)
        validate(abs_targets_filename, abs_segments_filename, table_dir=table_dir)

    if config is not None:
        for table in config.get_tables():
            if not table.write_table(
                validate_dir=validate_dir,
                in_dir=xls_dir,
                out_dir=Path("./validation/tables") / xls_dir.name / table.get_scenario_name()
            ):
                _pulse_logger.error(f"Could not write {table.get_scenario_name()}/{table.get_table_name()}")
        if use_test_results:
            plot_with_test_results(config.get_plotters())
        create_plots(config.get_plotters())

    # Run doxygen preprocessor
    for md_file in md_files:
        process_file(
            fpath=md_file,
            ref_dir=Path("./validation/tables"),
            dest_dir=Path("./validation/markdown"),
            replace_refs=False
        )


def main():
    logging.basicConfig(level=logging.DEBUG, format='%(levelname)s: %(message)s')
    logging.getLogger('PIL').setLevel(logging.INFO)
    plt.set_loglevel("info")

    parser = argparse.ArgumentParser(description="Process the full pipeline for segment validation")
    parser.add_argument(
        "folder",
        nargs='?',
        type=Path,
        default=None,
        help="validation folder to process during this pipeline run"
    )
    parser.add_argument(
        "-t", "--use-test-results",
        action='store_true',
        help="verify the latest test results."
    )
    run_group = parser.add_mutually_exclusive_group()
    run_group.add_argument(
        "-g", "--generate-only",
        action='store_true',
        help="only generate scenarios and validation target files"
    )
    run_group.add_argument(
        "-s", "--skip_exec",
        action='store_true',
        help="do not run the scenarios, results are expected to be found"
    )
    run_group.add_argument(
        "-m", "--markdown",
        action='store_true',
        help="only validate and build markdown"
    )
    run_group.add_argument(
        "-f", "--full",
        action='store_true',
        help="Run the full pipeline"
    )

    opts = parser.parse_args()

    folders = []
    if opts.folder is not None:
        folders.append(opts.folder)
    else:
        # In the future, we will do all xlsx in the data/validation dir
        # But for now, we are just hard coding the automated xlsx files
        folders.append("Hemorrhage")
        folders.append("AirwayObstruction")
        folders.append("Dehydration")
        folders.append("MechanicalVentilator")
        folders.append("CSTARS")

    exec_opt = eExecOpt.Full
    if opts.generate_only:
        exec_opt = eExecOpt.GenerateOnly
    elif opts.skip_exec:
        exec_opt = eExecOpt.SkipScenarioExecution
    elif opts.markdown:
        exec_opt = eExecOpt.MarkdownOnly

    # Clean out our results directory
    mk_dir = "./validation/markdown"
    if Path(mk_dir).exists():
        shutil.rmtree(mk_dir)
    for folder in folders:
        segment_validation_pipeline(
            folder=Path(folder),
            exec_opt=exec_opt,
            use_test_results=opts.use_test_results
        )


if __name__ == "__main__":
    main()
