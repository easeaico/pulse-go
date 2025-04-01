# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import logging
import math
import json

import numpy as np
import pandas as pd
from pathlib import Path
from typing import Dict, List, Optional

import PyPulse
from pulse.cdm.validation import SEPatientTimeSeriesValidation, generate_percentage_span
from pulse.cdm.utils.csv_utils import read_csv_into_df
from pulse.cdm.utils.math_utils import format_float
from pulse.cdm.utils.markdown import table
from pulse.cdm.utils.math_utils import percent_tolerance
from pulse.cdm.validation import SETimeSeriesValidationTarget
from pulse.cdm.io.validation import serialize_patient_time_series_validation_to_file


_pulse_logger = logging.getLogger('pulse')


def validate(
    patient_validation: SEPatientTimeSeriesValidation,
    csv_filename: Path,
    assessment_files: Optional[list],
    output_file: Optional[Path]
) -> None:
    """
    Validates given targets against given results csv.

    :param patient_validation: Validation targets to validate. Will
        be modified with results.
    :param csv_filename: Path to csv results file.
    :param assessment_files: List of Paths to assessment json files.
    :param output_file: (Optional) If provided, serialize
        results to this location.
    """
    df = read_csv_into_df(csv_filename)
    assessments = {}
    if assessment_files:
        base = csv_filename.stem.replace("Results", '')
        for assessment_file in assessment_files:
            with open(assessment_file, 'r') as file:
                key = assessment_file.stem.replace(base, '')
                assessments[key] = json.load(file)

    targets = patient_validation.get_targets()
    for tgt_table, tgts in targets.items():
        # No validation targets for this table
        if not tgts:
            continue

        # Evaluate results against targets
        for tgt in tgts:
            evaluate(tgt=tgt, results=df, assessments=assessments)

    # Serialize results, if requested
    if output_file is not None:
        _pulse_logger.info(f"Writing {output_file}")
        output_file.parent.mkdir(parents=True, exist_ok=True)
        serialize_patient_time_series_validation_to_file(patient_validation, output_file)


def evaluate(tgt: SETimeSeriesValidationTarget, results: pd.DataFrame, assessments: dict, epsilon: float=1E-9) -> None:
    """
    Evaluates given target.

    :param tgt: Validation target of interest. Will be modified with results.
    :param results: Data frame containing results for comparison.
    :param assessments: dict containing dicts for any generated assessment file.
    :param epsilon: Absolute tolerance in error.

    :raises ValueError: Required header (of any compatible unit) is not present in
        results data frame.
    """
    tgt_header = tgt.get_header()
    compare_type = tgt.get_comparison_type()
    target_type = tgt.get_target_type()

    if compare_type == SETimeSeriesValidationTarget.eComparisonType.NotValidating:
        return

    def _get_header(header: str) -> Optional[pd.Series]:
        """
        Attempts to retrieve series corresponding to header from data
        frame. If exact match is not found, will attempt to perform
        unit conversion.

        :param header: Header of interest.

        :raises ValueError: Header (of any compatible unit) is not
            present in data frame.

        :returns: Series corresponding to header.
        """
        series = None
        if header not in results.columns:
            # Attempt to locate header with different unit and convert
            paren_idx = header.find("(")
            if paren_idx != -1:
                unitless_header = header[:(paren_idx+1)]
                desired_unit = header[(paren_idx+1):-1].replace("_", " ")
                for h in results.columns:
                    if h.startswith(unitless_header):
                        results_paren_idx = h.find("(")
                        if results_paren_idx != -1:
                            results_unit = h[(results_paren_idx+1):-1].replace("_", " ")
                            series = results[h].map(lambda x: PyPulse.convert(x, results_unit, desired_unit)).squeeze()
                            break
            if series is None:
                _pulse_logger.error(f"Expected results missing header: {header}")
        else:
            series = results[header].squeeze()

        return series

    if tgt.get_assessment():
        comparison_value = math.nan
        if tgt.get_assessment() not in assessments:
            _pulse_logger.error(f"Assessment {tgt.get_assessment()} not found")
        else:
            assessment = assessments[tgt.get_assessment()]
            assessment_header = tgt_header
            desired_unit = None
            paren_idx = assessment_header.find("(")
            if paren_idx != -1:
                desired_unit = assessment_header[(paren_idx+1):-1].replace("_", " ")
                assessment_header = assessment_header[:paren_idx]
            if assessment_header not in assessment:
                _pulse_logger.error(f"Header {assessment_header} not found in {tgt.get_assessment()}")
            else:
                assessment_unit = None
                content = assessment[assessment_header]
                if isinstance(content, dict):
                    if "Value" in content:
                        comparison_value = content["Value"]
                    else:
                        for key in content:
                            if "Scalar" in key:
                                comparison_value = content[key]["Value"]
                                if "Unit" in content[key]:
                                    assessment_unit = content[key]["Unit"]
                    if assessment_unit is not None and assessment_unit != desired_unit:
                        comparison_value = PyPulse.convert(comparison_value, assessment_unit, desired_unit)
                elif isinstance(content, str):
                    comparison_value = content
    else:
        header_series = _get_header(tgt_header)

        if header_series is None:
            comparison_value = math.nan
        else:
            # Normalize series based on target type
            if (
               target_type == SETimeSeriesValidationTarget.eTargetType.MeanPerIdealWeight_kg or
               target_type == SETimeSeriesValidationTarget.eTargetType.MinPerIdealWeight_kg or
               target_type == SETimeSeriesValidationTarget.eTargetType.MaxPerIdealWeight_kg
            ):
                ideal_weight_kg_series = _get_header("Patient-IdealBodyWeight(kg)")
                header_series = header_series.div(ideal_weight_kg_series)

            # Compute requested comparison value
            if (
                target_type == SETimeSeriesValidationTarget.eTargetType.Minimum or
                target_type == SETimeSeriesValidationTarget.eTargetType.MinPerIdealWeight_kg
            ):
                comparison_value = header_series.min()
            elif (
                target_type == SETimeSeriesValidationTarget.eTargetType.Maximum or
                target_type == SETimeSeriesValidationTarget.eTargetType.MaxPerIdealWeight_kg
            ):
                comparison_value = header_series.max()
            elif (
                target_type == SETimeSeriesValidationTarget.eTargetType.Mean or
                target_type == SETimeSeriesValidationTarget.eTargetType.MeanPerIdealWeight_kg
            ):
                comparison_value = header_series.mean() if not header_series.empty else np.nan
            else:
                raise ValueError(f"Unknown target type: {target_type}")

    error = None

    if isinstance(comparison_value, str):
        tgt.set_computed_enum(comparison_value)
        if tgt.get_target() != comparison_value:
            error = np.nan
        else:
            error = 0.
    else:
        tgt.set_computed_value(comparison_value)
        if np.isnan(comparison_value):
            _pulse_logger.error(f"Comparison value for {tgt_header} is nan")

        # Compute error
        tgt_min = tgt.get_target_minimum()
        tgt_max = tgt.get_target_maximum()
        min_error = percent_tolerance(tgt_min, comparison_value, epsilon)
        max_error = percent_tolerance(tgt_max, comparison_value, epsilon)

        # No error if we are in range
        if np.isnan(comparison_value):
            error = np.nan
        elif tgt_min <= comparison_value <= tgt_max:
            error = 0.
        elif comparison_value > tgt_max:
            error = max_error
        elif comparison_value < tgt_min:
            error = min_error
        else:
            raise ValueError("Unable to determine error")

    # Close enough
    if abs(error) < 1E-15:
        error = 0.
    tgt.set_error_value(error)


def gen_expected_str(tgt: SETimeSeriesValidationTarget) -> str:
    """
    Generates string representing expected value.

    :param tgt: The validation target.

    :raises ValueError: Unknown comparision type.

    :returns: Expected value string.
    """
    compare_type = tgt.get_comparison_type()
    formatting = tgt.get_table_formatting()
    if compare_type == SETimeSeriesValidationTarget.eComparisonType.EqualToValue:
        expected_str = f"{format_float(tgt.get_target(), formatting)}"
    elif compare_type == SETimeSeriesValidationTarget.eComparisonType.Range:
        expected_str = (f"[{format_float(tgt.get_target_minimum(), formatting)},"
                        f" {format_float(tgt.get_target_maximum(), formatting)}]")
    elif compare_type == SETimeSeriesValidationTarget.eComparisonType.EqualToEnum:
        expected_str = f"{tgt.get_target()}"
    else:
        raise ValueError(f"Unknown comparison type: {compare_type}")

    # Add any references
    if tgt.get_reference():
        references = [ref.strip() for ref in tgt.get_reference().replace("\n", "").split(",")]
        for ref in references:
            if not ref.startswith('['):
                expected_str += f" @cite {ref}"

    if not expected_str:
        return "&nbsp;"

    return expected_str


def gen_engine_val_str(tgt: SETimeSeriesValidationTarget) -> str:
    """
    Generates string representing engine value.

    :param tgt: The validation target.

    :returns: Engine value string.
    """
    target_type = tgt.get_target_type()
    if target_type == SETimeSeriesValidationTarget.eTargetType.Enumeration:
        engine_val_str = tgt.get_computed_enum()
    else:
        formatting = tgt.get_table_formatting()
        engine_val_str = f"{target_type.name.replace('_kg', '(kg)')} of " \
                         f"{format_float(tgt.get_computed_value(), formatting)}"

    if not engine_val_str:
        return "&nbsp;"

    return engine_val_str


def generate_validation_tables(
    target_map: SEPatientTimeSeriesValidation,
    table_dir: Path
) -> None:
    """
    Generates validation tables for given target map.

    :param target_map: Validation targets.
    :param table_dir: Tables will be saved to this directory.

    :raises ValueError: Unknown comparison type
    :raises ValueError: Unknown patient (patient has no name so table file cannot be named)
    """
    headers = ["Property Name", "Expected Value", "Engine Value", "Percent Error", "Notes"]
    fields = list(range(len(headers)))
    align = [('<', '<')] * len(headers)

    table_dir.mkdir(parents=True, exist_ok=True)
    patient_name = target_map.get_patient().get_name()
    if not patient_name:
        # Shouldn't really happen as dataset reader sets to patient filename if one doesn't exist
        raise ValueError("Unknown patient found, cannot write table as I don't know what to name it.")

    targets = target_map.get_targets()
    for tgt_table, tgts in targets.items():
        # No validation targets for this table
        if not tgts:
            continue

        table_data = []
        for tgt in tgts:
            # Not validated
            if not tgt.is_evaluated():
                continue

            success = 10 if not tgt.has_good_percent_error() else tgt.get_good_percent_error()
            warning = 30 if not tgt.has_fair_percent_error() else tgt.get_fair_percent_error()
            table_data.append([
                tgt.get_header(),
                gen_expected_str(tgt),
                gen_engine_val_str(tgt),
                generate_percentage_span(tgt.get_error_value(), success, warning),
                notes if (notes := tgt.get_notes()) else "&nbsp;"
            ])

        # Write out table
        md_filename = table_dir / f"{tgt_table}-{patient_name}ValidationTable.md"
        with open(md_filename, "w") as md_file:
            _pulse_logger.info(f"Writing {md_filename}")
            table(md_file, table_data, fields, headers, align)
