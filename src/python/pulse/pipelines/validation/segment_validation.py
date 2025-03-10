# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import re
import sys
import logging
import numpy as np
from pathlib import Path
from typing import Dict, List, Optional

import PyPulse

from pulse.cdm.engine import SEDataRequested
from pulse.cdm.validation import SESegmentValidationTarget
from pulse.cdm.utils.markdown import table
from pulse.cdm.utils.math_utils import generate_percentage_span, percent_change, percent_difference
from pulse.cdm.io.engine import serialize_data_requested_result_from_file
from pulse.cdm.io.validation import serialize_segment_validation_segment_list_from_file
_pulse_logger = logging.getLogger('pulse')


def validate(targets_filename: Path, segments_filename: Path, table_dir: Path) -> None:
    # Get all validation targets and segment results from files
    _pulse_logger.info(f"Validating {segments_filename} against {targets_filename}")
    # Get the sheet name from the filename
    this_sheet_name = segments_filename.stem[:segments_filename.stem.find("Results")]
    targets = serialize_segment_validation_segment_list_from_file(str(targets_filename))
    referenced_results = {}
    # Look through the targets and find all the sheets this test case references
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
                        referenced_segments_filename = Path(str(segments_filename).replace(this_sheet_name, sheet_name))
                        if not referenced_segments_filename.exists():
                            _pulse_logger.error(f"Cannot find referenced sheet: {referenced_segments_filename}")
                        else:
                            referenced_results[sheet_name] = (
                                serialize_data_requested_result_from_file(str(referenced_segments_filename)))
    # Load the results for this test case
    results = serialize_data_requested_result_from_file(str(segments_filename))

    headers = ["Property Name", "Validation", "Engine Value", "Percent Error", "Percent Change", "Notes"]
    fields = list(range(len(headers)))
    align = [('<', '<')] * len(headers)

    for target in targets:
        if not target.has_validation_targets():
            continue
        # Get the result associated with this target
        seg_id = target.get_segment_id()

        # Evaluate targets and create Markdown tables for each segment
        table_data = []
        for tgt in target.get_validation_targets():
            table_data.append(evaluate(seg_id, tgt, results, referenced_results))

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
             referenced_results: List[SEDataRequested]) -> List[str]:
    header = tgt.get_header()

    epsilon = 1E-9
    percent_precision = 1
    value_precision = 4
    expected_str = ""
    err_str = ""
    change_str = ""

    result = results.get_segment(seg_id)
    if result is None:
        raise ValueError(f"Could not find result for segment {seg_id}")
    header_idx = results.get_header_index(header)
    if header_idx is None:
        raise ValueError(f"Could not find results for {header} in segment {seg_id}")
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
    formula = tgt.get_comparison_formula().lower().replace("healthy", "-1").replace("baseline", "0")
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

    for expression in expressions:
        # First sub in all referenced segment values into the expression

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
        expected_val = eval(expected_val_expression)

        compare_type = None
        if '>' in expression or '<' in expression:
            if '>' in expression:
                compare_type = "GreaterThan"
            else:
                compare_type = "LessThan"
            # TODO: Implement gradient?
            change = percent_change(expected_val, engine_val, epsilon)
            c = '"danger"'
            if not np.isnan(change) and change < 0.0:
                c = '"success"'
            change_str = f'<span class={c}>{change:.{percent_precision}f}%</span>'

        elif '[' in expression and ']' in expression:
            tgt_min = tgt.get_target_minimum()
            tgt_max = tgt.get_target_maximum()
            expected_str = f"[{tgt_min:.{value_precision}G},{tgt_max:.{value_precision}G}]"
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

            err_str = generate_percentage_span(err, percent_precision)
        elif '=' in expression:
            compare_type = "EqualTo"
            err = percent_difference(expected_val, engine_val, epsilon)
            # Close enough
            if abs(err) < epsilon:
                err = 0.
            err_str = generate_percentage_span(err, percent_precision)
        else:
            # TODO empty formula means we are not validating this row
            # TODO Trends to/from a value
            _pulse_logger.error(f"Not sure how to handle expression: {expression}")
            continue
        expected_str = f"({expected_val:.{value_precision}G})"

        # Add comparison type to beginning of expected string
        if len(referenced_segments) > 0:
            # TODO Not supporting multiple segment references in 1 expression
            tgt_seg = referenced_segments[0].replace('{', '').replace('}', '')
            if ':' in tgt_seg:
                ref = tgt_seg.split(':')
                if '0' in tgt_seg:
                    expected_str = f"{compare_type} {ref[0]} Baseline {expected_str}"
                else:
                    expected_str = f"{compare_type} {ref[0]} Segment {ref[1]} {expected_str}"
            else:
                if '0' in tgt_seg:
                    expected_str = f"{compare_type} Baseline {expected_str}"
                else:
                    expected_str = f"{compare_type} Segment {tgt_seg} {expected_str}"
        else:
            expected_str = f'{compare_type} {expected_str}'

    if tgt.get_reference():
        references = [ref.strip() for ref in tgt.get_reference().replace("\n", "").split(",")]
        for ref in references:
            if not ref.startswith('['):
                expected_str += f" @cite {ref}"

    return [
        header,
        expected_str if expected_str else "&nbsp;",
        f"{engine_val:.{value_precision}G}",
        err_str if err_str else "&nbsp;",
        change_str if change_str else "&nbsp;",
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
