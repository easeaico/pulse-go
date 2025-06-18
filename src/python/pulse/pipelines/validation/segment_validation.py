# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import json
import re
import sys
import logging
import numpy as np
from pathlib import Path
from typing import List

import PyPulse

from pulse.cdm.engine import SEDataRequested, eEvent
from pulse.cdm.validation import SESegmentValidationTarget, generate_percentage_span, format_float
from pulse.cdm.utils.logger import PulseLog
from pulse.cdm.utils.markdown import table
from pulse.cdm.utils.math_utils import percent_change, percent_difference
from pulse.cdm.io.engine import serialize_data_requested_result_from_file
from pulse.cdm.io.validation import serialize_segment_validation_segment_list_from_file


_pulse_logger = logging.getLogger('pulse')


def validate(name: str, scenario_dir: Path, results_dir: Path, sheet_name: str = "") -> None:
    target_files = [item.name for item in scenario_dir.glob("*")
                    if not item.is_dir() and "-ValidationTargets.json" in item.name]

    def find_results_files() -> (Path, Path, list[Path]):
        segments_filename = Path(results_dir / f"{target_name}Results-Segments.json")
        if not segments_filename.exists():
            _pulse_logger.error(f"Unable to locate segments for {segments_filename}")
            return None, None, []

        log_filename = Path(results_dir / f"{target_name}Results.log")
        if not log_filename.exists():
            _pulse_logger.warning(f"Unable to locate log file {log_filename}")
            log_filename = None

        # Are there any assessment files?
        assessment_files = [item for item in results_dir.glob(f"{target_name}*@*.json")]

        return segments_filename, log_filename, assessment_files

    for target_file in target_files:
        if sheet_name and not target_file.startswith(sheet_name):
            continue
        target_name = target_file[:target_file.find('-')]
        # Create a directory to put our tables
        table_dir = Path(f"./validation/tables/{name}/{target_name}")
        table_dir.mkdir(parents=True, exist_ok=True)

        targets_filename = Path(scenario_dir / target_file)
        results_files = find_results_files()
        # idx 0 = segments file
        # idx 1 = log file
        # idx 2 = list of assessment files
        if results_files[0] is None:
            _pulse_logger.error(f"NOT Validating {targets_filename}")
            continue  # Must, at least, have a segments file

        # Get all validation targets and segment results from files
        _pulse_logger.info(f"Validating {results_files[0]} against {targets_filename}")
        targets = serialize_segment_validation_segment_list_from_file(str(targets_filename))
        # Load the results for this test case
        results = serialize_data_requested_result_from_file(str(results_files[0]))

        # Look through the targets and find all the sheets this test case references
        referenced_results = {}
        for target in targets:
            for property_target in target.get_validation_targets():
                if property_target.has_comparison_formula():
                    #  Does this formula reference another workbook/sheet?
                    formula = property_target.get_comparison_formula()
                    #  Note: If we want to also allow referencing other workbook books, maybe add another [a-zA-Z]+:
                    sheet_references = re.findall(r"\{[a-zA-Z]+:[0-9]+\}", formula, re.DOTALL)
                    for sheet_reference in sheet_references:
                        sheet_name = re.findall(r"[a-zA-Z]+", sheet_reference, re.DOTALL)[0]
                        if sheet_name not in referenced_results:
                            referenced_segments_filename = Path(str(results_files[0]).replace(target_name, sheet_name))
                            if not referenced_segments_filename.exists():
                                _pulse_logger.error(f"Cannot find referenced sheet: {referenced_segments_filename}")
                            else:
                                referenced_results[sheet_name] = (
                                    serialize_data_requested_result_from_file(str(referenced_segments_filename)))
                                #  TODO Not supporting referencing event/assessment values from another workbook/sheet
                                #  Would need to call find_results_files for the referenced sheet, and pass them along

        headers = ["Property Name", "Validation", "Engine Value", "Percent Error", "Percent Change", "Notes"]
        fields = list(range(len(headers)))
        align = [('<', '<')] * len(headers)

        log = PulseLog()
        for target in targets:
            if not target.has_validation_targets():
                continue
            # Get the result associated with this target
            seg_id = target.get_segment_id()
            _pulse_logger.info(f"Processing segment {seg_id}")

            # Evaluate targets and create Markdown tables for each segment
            table_data = []
            segment_durations = None
            for tgt in target.get_validation_targets():
                supplemental_results = None
                if "Event" in tgt.get_header():
                    header = tgt.get_header().split('-')
                    if results_files[1] is None:
                        _pulse_logger.error(f"No log file, cannot Validate {tgt.get_header()}")
                        continue
                    seg_start_time = results.get_segment(seg_id - 1).time_s
                    seg_end_time = results.get_segment(seg_id).time_s
                    if not log.is_valid:
                        log.parse(results_files[1])
                    if not segment_durations:
                        segment_durations = log.get_active_events_in_window(seg_start_time, seg_end_time)
                    event = eEvent[header[1]]
                    if event not in segment_durations:
                        supplemental_results = segment_durations[None]
                    else:
                        supplemental_results = segment_durations[event]
                elif "Assessment" in tgt.get_header():
                    if len(results_files[2]) == 0:
                        _pulse_logger.error(f"No assessment files found, cannot Validate {tgt.get_header()}")
                        continue
                    header = tgt.get_header().split('-')
                    seg_end_time = results.get_segment(seg_id).time_s
                    if seg_end_time - int(seg_end_time) == 0:
                        seg_end_time = int(seg_end_time)
                    for assessment_file in results_files[2]:
                        if header[1] in assessment_file.name and f"@{seg_end_time}s.json" in assessment_file.name:
                            with open(assessment_file, 'r') as file:
                                supplemental_results = json.load(file)
                            break
                    if supplemental_results is None:
                        _pulse_logger.error(f"Could not get assessment value, cannot Validate {tgt.get_header()}")
                        continue

                # If the target result is event or assessment, we need to pull it and insert it into the results
                table_data.append(evaluate(seg_id, tgt, results, supplemental_results, referenced_results))

            # Write our table
            md_filename = table_dir / f"Segment{seg_id}ValidationTable.md"
            with open(md_filename, "w") as md_file:
                _pulse_logger.info(f"Writing {md_filename}")
                lines = list()
                if target.has_notes():
                    lines.append(target.get_notes().rstrip())
                    lines.append("\n\n")
                table_name = table_dir.as_posix()
                table_name = table_name[table_name.rindex('/') + 1:]
                lines.append(f"<center>\n<i>@tabledef {{{table_name}Segment{seg_id}}}. "
                             f"Data request validation results for Segment {seg_id}.</i>\n</center>\n\n")
                md_file.writelines(lines)
                table(md_file, table_data, fields, headers, align)


def evaluate(seg_id: int,
             tgt: SESegmentValidationTarget,
             results: SEDataRequested,
             supplemental_results: dict,
             referenced_results: dict) -> List[str]:
    header = tgt.get_header()
    _pulse_logger.info(f"Evaluating {header}")

    epsilon = 1E-9

    result = results.get_segment(seg_id)
    if result is None:
        _pulse_logger.error(f"Could not find result for segment {seg_id}")
        return []
    if "Event" in header:
        header = header.replace("Event-", "")
        engine_val = supplemental_results[header.split('-')[1]]
    elif "Assessment" in header:
        header = header.replace("Assessment-", "")
        engine_val = supplemental_results[header.split('-')[1]]
    else:
        header_idx = results.get_header_index(header)
        if header_idx is None:
            _pulse_logger.error(f"Could not find results for {header} in segment {seg_id}")
            return []
        engine_val = result.values[header_idx]

    def _convert_unit(_header: str, _val: float):
        paren_idx = _header.find("(")
        if paren_idx != -1:
            requested_unit = _header[paren_idx+1:-1].replace("_", " ")
            engine_full_header = results.get_headers()[results.get_header_index(_header)]
            engine_paren_idx = engine_full_header.find("(")
            if engine_paren_idx == -1:
                raise ValueError(f"Cannot convert between {requested_unit} and unitless for {_header}")
            curr_unit = engine_full_header[engine_paren_idx+1:-1].replace("_", " ")
            if curr_unit != requested_unit:
                _val = PyPulse.convert(_val, curr_unit, requested_unit)

        return _val

    # Convert to validation unit if needed
    engine_val = _convert_unit(header, engine_val)
    # Check for multiple expressions
    # Not currently supporting a mix of 'and' and 'or', or parens at this point
    # The problem is splitting out the %diff and %change for each expression and getting them into the table
    # It's probably not that bad, we can revisit if needed
    logical_join = None
    formula = tgt.get_comparison_formula()
    replace = tgt.get_comparison_formula().lower()
    s = replace.find("healthy")
    if s >= 0:
        formula = f"{formula[:s]}-1{formula[s + 7:]}"
    s = replace.find("baseline")
    if s >= 0:
        formula = f"{formula[:s]}-1{formula[s + 8:]}"
    if ("and" in formula and "or" in formula) or '(' in formula:
        _pulse_logger.error(f"We currently don't support this complex of a formula: {formula}")
        return []
    # Split out multiple expressions
    if "and" in formula:
        logical_join = " and "
        expressions = formula.split("and")
    elif "or" in formula:
        logical_join = " or "
        expressions = formula.split("or")
    else:
        expressions = [formula]

    formula_expected_str = ""
    formula_error_str = ""
    formula_change_str = ""
    for expression in expressions:
        expression_expected_str = ""
        expression_error_str = ""
        expression_change_str = ""
        # Grab the formula, so we can use it in the table, so we know how we got the expected value
        compare_type = expression.replace("{v}", "").strip()
        #   Find all local segment references
        local_references = set(re.findall(r"\{-?[0-9]+\}", expression, re.DOTALL))
        for local_reference in local_references:
            segment = int(local_reference.replace('{', '').replace('}', ''))
            tgt_result = results.get_segment(segment)
            if tgt_result is None:
                raise Exception("Could not find result for segment " + local_reference)
            segment_val = tgt_result.values[results.get_header_index(header)]
            segment_val = _convert_unit(header, segment_val)
            # Replace the reference with the value
            expression = expression.replace(local_reference, str(segment_val))
        #   Find all external segment references
        sheet_references = set(re.findall(r"\{[a-zA-Z]+:-?[0-9]+\}", expression, re.DOTALL))
        for sheet_reference in sheet_references:
            sheet_info = sheet_reference.replace('{', '').replace('}', '').split(':')
            tgt_result = referenced_results[sheet_info[0]].get_segment(int(sheet_info[1]))
            if tgt_result is None:
                raise Exception("Could not find result for segment " + sheet_reference)
            segment_val = tgt_result.values[referenced_results[sheet_info[0]].get_header_index(header)]
            segment_val = _convert_unit(header, segment_val)
            # Replace the reference with the value
            expression = expression.replace(sheet_reference, str(segment_val))

        referenced_segments = list(local_references) + list(sheet_references)
        if len(referenced_segments) > 1:
            _pulse_logger.warning(f"Multiple segments referenced in an expression...not sure table will make sense")

        expected_val_expression = (expression.replace('=', '')
                                   .replace('>', '')
                                   .replace('<', '')
                                   .replace("{v}", '')).strip()
        try:
            if "nan" in expected_val_expression:
                expected_val = np.nan
                _pulse_logger.error(f"Expected value is NaN, is this intentional?")
            else:
                expected_val = eval(expected_val_expression)
        except NameError:
            _pulse_logger.error(f"Unable to evaluate expression {expected_val_expression}")
            return []

        if '>' in expression or '<' in expression:
            # TODO: Implement gradient?
            change = percent_change(expected_val, engine_val, epsilon)
            c = '"danger"'
            if '>' in expression:
                if '=' in expression:
                    compare_type = compare_type.replace(">=", "GreaterThanEqualTo")
                    if not np.isnan(change) and change >= 0.0:
                        c = '"success"'
                else:
                    compare_type = compare_type.replace(">", "GreaterThan")
                    if not np.isnan(change) and change > 0.0:
                        c = '"success"'
            else:
                if '=' in expression:
                    compare_type = compare_type.replace("<=", "LessThanEqualTo")
                    if not np.isnan(change) and change <= 0.0:
                        c = '"success"'
                else:
                    compare_type = compare_type.replace("<", "LessThan")
                    if not np.isnan(change) and change < 0.0:
                        c = '"success"'
            expression_change_str = f'<span class={c}>{format_float(change)}%</span>'
            expression_expected_str = f"{format_float(expected_val)}"

        elif '=' in expression:
            compare_type = compare_type.replace("=", "EqualTo")
            if isinstance(engine_val, float):
                err = percent_difference(expected_val, engine_val, epsilon)
                # Close enough
                if abs(err) < epsilon:
                    err = 0.
                success = 10 if not tgt.has_good_percent_error() else tgt.get_good_percent_error()
                warning = 30 if not tgt.has_fair_percent_error() else tgt.get_fair_percent_error()
                expression_error_str = generate_percentage_span(err, success, warning)
                expression_expected_str = f"{format_float(expected_val)}"
            elif isinstance(engine_val, bool):
                if engine_val != expected_val:
                    expression_error_str = f"<span class=\"danger\">Fail</span>"
                else:
                    expression_error_str = f"<span class=\"success\">Pass</span>"
                expression_expected_str = f"{expected_val}"
            elif isinstance(engine_val, str):
                expected_val = expected_val.replace('"', '')
                expected_val = expected_val.replace("'", '')
                if engine_val != expected_val:
                    expression_error_str = f"<span class=\"danger\">Fail</span>"
                else:
                    expression_error_str = f"<span class=\"success\">Pass</span>"
                expression_expected_str = f"{expected_val}"
            else:
                _pulse_logger.error(f"Unsupported data type for property {header}")

        elif '[' in expression and ']' in expression:
            compare_type = None
            values = expression.replace('[', '').replace(']', '').split(',')
            tgt_min = float(values[0].strip())
            tgt_max = float(values[1].strip())
            min_err = percent_difference(tgt_min, engine_val, epsilon)
            max_err = percent_difference(tgt_max, engine_val, epsilon)

            err = np.inf
            # No error if we are in range
            if tgt_min <= engine_val <= tgt_max:
                err = 0.
            elif engine_val > tgt_max:
                err = max_err
            elif engine_val < tgt_min:
                err = min_err

            # Close enough
            if abs(err) < epsilon:
                err = 0.

            success = 0 if not tgt.has_good_percent_error() else tgt.get_good_percent_error()
            warning = 10 if not tgt.has_fair_percent_error() else tgt.get_fair_percent_error()
            expression_error_str = generate_percentage_span(err, success, warning)
            expression_expected_str = f"[{format_float(tgt_min)},{format_float(tgt_max)}]"

        else:
            # TODO empty formula means we are not validating this row
            # TODO Trends to/from a value
            _pulse_logger.error(f"Not sure how to handle expression: {expression}")
            continue

        # Add comparison type to beginning of expected string
        if len(referenced_segments) > 0:
            # TODO Not supporting multiple segment references in 1 expression
            tgt_seg = referenced_segments[0].replace('{', '').replace('}', '')
            if ':' in tgt_seg:
                ref = tgt_seg.split(':')
                if '-1' in tgt_seg:
                    seg = f"{ref[0]} Healthy"
                elif '0' in tgt_seg:
                    seg = f"{ref[0]} Baseline"
                else:
                    seg = f"Segment{ref[1]}"
            else:
                if '-1' in tgt_seg:
                    seg = "Healthy"
                elif '0' in tgt_seg:
                    seg = "Baseline"
                else:
                    seg = f"Segment{tgt_seg}"
            compare_type = compare_type.replace(referenced_segments[0], seg)
            expression_expected_str = f"{compare_type} ({expression_expected_str})"
        elif compare_type:
            expression_expected_str = compare_type

        if logical_join:
            if len(formula_expected_str) > 0:
                formula_expected_str += logical_join + expression_expected_str
            else:
                formula_expected_str = expression_expected_str

            if len(formula_error_str) > 0:
                formula_error_str += logical_join + expression_error_str
            else:
                formula_error_str = expression_error_str

            if len(formula_change_str) > 0:
                formula_change_str += logical_join + expression_change_str
            else:
                formula_change_str = expression_change_str
        else:
            formula_expected_str = expression_expected_str
            formula_error_str = expression_error_str
            formula_change_str = expression_change_str

    if tgt.get_reference():
        references = [ref.strip() for ref in tgt.get_reference().replace("\n", "").split(",")]
        for ref in references:
            if not ref.startswith('['):
                formula_expected_str += f" @cite {ref}"

    if logical_join:
        if "or" in logical_join:
            def _get_best_or_class(logical_expression: str):
                _success = ""
                _warning = ""
                _danger = ""
                for e in logical_expression.split("or"):
                    # TODO might want to keep the the lowest (or highest) percent for each category?
                    if "success" in e:
                        _success = e.strip()
                    elif "warning" in e:
                        _warning = e.strip()
                    elif "danger" in e:
                        _danger = e.strip()
                if _success:
                    return _success
                elif _warning:
                    return _warning
                elif _danger:
                    return _danger
            if formula_error_str:
                formula_error_str = _get_best_or_class(formula_error_str)
            if formula_change_str:
                formula_change_str = _get_best_or_class(formula_change_str)
        elif "and" in logical_join:
            def _replace_and_class(logical_expression: str):
                _success = 0
                _warning = 0
                _danger = 0
                for e in logical_expression.split("and"):
                    # TODO might want to keep the the lowest (or highest) percent for each category?
                    if "success" in e:
                        _success += 1
                    elif "warning" in e:
                        _warning += 1
                    elif "danger" in e:
                        _danger += 1
                # Change class tags with the worst class, so the cell is colored appropriately
                if _danger > 0:
                    logical_expression.replace("warning", "danger")
                    logical_expression.replace("success", "danger")
                elif _warning > 0:
                    logical_expression.replace("success", "warning")
                return logical_expression

            if formula_error_str:
                formula_error_str = _replace_and_class(formula_error_str)
            if formula_change_str:
                formula_change_str = _replace_and_class(formula_change_str)

    return [
        header,
        formula_expected_str if formula_expected_str else "&nbsp;",
        f"{format_float(engine_val) if isinstance(engine_val,float) else engine_val}",
        formula_error_str if formula_error_str else "&nbsp;",
        formula_change_str if formula_change_str else "&nbsp;",
        tgt.get_notes() if tgt.get_notes() else "&nbsp;"
        ]


def main():
    logging.basicConfig(level=logging.DEBUG, format='%(levelname)s: %(message)s')

    if len(sys.argv) < 3:
        _pulse_logger.error("Expected inputs : <validation targets directory> <results directory> [markdown directory]")
        sys.exit(1)

    targets_dir = Path(sys.argv[1])
    results_dir = Path(sys.argv[2])
    markdown_dir = None

    if not targets_dir.is_dir():
        _pulse_logger.error("Please provide a valid validation targets directory")
        sys.exit(1)

    if not results_dir.is_dir():
        _pulse_logger.error("Please provide a valid results directory")
        sys.exit(1)

    if len(sys.argv) > 3:
        markdown_dir = Path(sys.argv[3])
        if not markdown_dir.is_dir():
            _pulse_logger.error("Please provide a valid markdown directory")
            sys.exit(1)

    validate(targets_dir, results_dir, markdown_dir)


if __name__ == "__main__":
    main()
