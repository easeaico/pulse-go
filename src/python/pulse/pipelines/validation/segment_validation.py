# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

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
    targets = serialize_segment_validation_segment_list_from_file(targets_filename)
    results = serialize_data_requested_result_from_file(segments_filename)

    headers = ["Property Name", "Validation", "Engine Value", "Percent Error", "Percent Change", "Notes"]
    fields = list(range(len(headers)))
    align = [('<', '<')] * len(headers)

    for target in targets:
        if not target.has_validation_targets():
            continue
        # Get the result associated with this target
        seg_id = target.get_segment_id()


        # Evaluate targets and create markdown tables for each segment
        table_data = []
        for tgt in target.get_validation_targets():
            table_data.append(evaluate(seg_id, tgt, results))

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
            lines.append(f"<center>\n<i>@tabledef {{{table_name}Segment{seg_id}}}. Data request validation results for Segment {seg_id}.</i>\n</center>\n\n")
            md_file.writelines(lines)
            table(md_file, table_data, fields, headers, align)


def evaluate(seg_id: int, tgt: SESegmentValidationTarget, results: SEDataRequested) -> List[str]:
    header = tgt.get_header()
    compare_type = tgt.get_comparison_type()

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

    def _convert_unit(header: str, val: float):
        paren_idx = header.find("(")
        if paren_idx != -1:
            requested_unit = header[paren_idx+1:-1].replace("_", " ")
            engine_full_header = results.get_headers()[results.get_header_index(header)]
            engine_paren_idx = engine_full_header.find("(")
            if engine_paren_idx == -1:
                raise ValueError(f"Cannot convert between {requested_unit} and unitless for {header}")
            curr_unit = engine_full_header[engine_paren_idx+1:-1].replace("_", " ")
            if curr_unit != requested_unit:
                val = PyPulse.convert(val, curr_unit, requested_unit)

        return val

    # Convert to validation unit if needed
    engine_val = _convert_unit(header, engine_val)

    if compare_type == SESegmentValidationTarget.eComparisonType.EqualToSegment or \
       compare_type == SESegmentValidationTarget.eComparisonType.EqualToValue:

        if compare_type == SESegmentValidationTarget.eComparisonType.EqualToSegment:
            tgt_result = results.get_segment(tgt.get_target_segment())
            if tgt_result is None:
                raise Exception("Could not find result for segment " + str(seg_id))
            expected_val = tgt_result.values[results.get_header_index(header)]
            expected_val = _convert_unit(header, expected_val)
            expected_str = f"({expected_val:.{value_precision}G})"
        else:
            expected_val = tgt.get_target()
            expected_str = f"{expected_val:.{value_precision}G}"
        err = percent_difference(expected_val, engine_val, epsilon)

        # Close enough
        if abs(err) < epsilon:
            err = 0.

        err_str = generate_percentage_span(err, percent_precision)
    elif compare_type == SESegmentValidationTarget.eComparisonType.GreaterThanSegment or \
         compare_type == SESegmentValidationTarget.eComparisonType.GreaterThanValue:

        if compare_type == SESegmentValidationTarget.eComparisonType.GreaterThanSegment:
            tgt_result = results.get_segment(tgt.get_target_segment())
            if tgt_result is None:
                raise Exception("Could not find result for segment " + str(seg_id))
            expected_val = tgt_result.values[results.get_header_index(header)]
            expected_val = _convert_unit(header, expected_val)
            expected_str = f"({expected_val:.{value_precision}G})"
        else:
            expected_val = tgt.get_target()
            expected_str = f"{expected_val:.{value_precision}G}"

        # TODO: Implement gradient for greater than?
        change = percent_change(expected_val, engine_val, epsilon)
        c = '"danger"'
        if not np.isnan(change) and change > 0.0:
            c='"success"'
        change_str = f'<span class={c}>{change:.{percent_precision}f}%</span>'
    elif compare_type == SESegmentValidationTarget.eComparisonType.LessThanSegment or \
         compare_type == SESegmentValidationTarget.eComparisonType.LessThanValue:

        if compare_type == SESegmentValidationTarget.eComparisonType.LessThanSegment:
            tgt_result = results.get_segment(tgt.get_target_segment())
            if tgt_result is None:
                raise Exception("Could not find result for segment " + str(seg_id))
            expected_val = tgt_result.values[results.get_header_index(header)]
            expected_val = _convert_unit(header, expected_val)
            expected_str = f"({expected_val:.{value_precision}G})"
        else:
            expected_val = tgt.get_target()
            expected_str = f"{expected_val:.{value_precision}G}"

        # TODO: Implement gradient for less than?
        change = percent_change(expected_val, engine_val, epsilon)
        c = '"danger"'
        if not np.isnan(change) and change < 0.0:
            c='"success"'
        change_str = f'<span class={c}>{change:.{percent_precision}f}%</span>'
    elif compare_type == SESegmentValidationTarget.eComparisonType.TrendsToSegment or \
         compare_type == SESegmentValidationTarget.eComparisonType.TrendsToValue:

        if compare_type == SESegmentValidationTarget.eComparisonType.TrendsToSegment:
            tgt_result = results.get_segment(tgt.get_target_segment())
            if tgt_result is None:
                raise Exception("Could not find result for segment " + str(seg_id))
            expected_val = tgt_result.values[results.get_header_index(header)]
            expected_val = _convert_unit(header, expected_val)
            expected_str = f"({expected_val:.{value_precision}G})"
        else:
            expected_val = tgt.get_target()
            expected_str = f"{expected_val:.{value_precision}G}"

        # Get engine values at beginning (end of last) and end of current segment
        prev_seg_id = seg_id - 1
        if prev_seg_id not in results or header not in results[prev_seg_id]:
            raise Exception(f"Unable to find segment {seg_id} initial value for trends towards comparison for {header}.")
        init_val = results[prev_seg_id][header]

        init_change = percent_change(expected_val, init_val, epsilon)
        end_change = percent_change(expected_val, engine_val, epsilon)

        # TODO: gradient for trends to?
        c = '"danger"'
        # Check that difference between expected and engine decreased and both
        # percentages are the same sign
        if not np.isnan(init_change) and not np.isnan(end_change) and \
             abs(end_change) < abs(init_change) and init_change * end_change > 0.0:
            c='"success"'
        # TODO: what percent change value is valuable to display?
        change = percent_change(init_val, engine_val, epsilon)
        change_str = f'<span class={c}>{change:.{percent_precision}f}%</span>'
    elif compare_type == SESegmentValidationTarget.eComparisonType.Range:
        tgt_min = tgt.get_target_minimum()
        tgt_max = tgt.get_target_maximum()
        expected_str = f"[{tgt_min:.{value_precision}G},{tgt_max:.{value_precision}G}]"
        min_err = percent_difference(tgt_min, engine_val, epsilon)
        max_err = percent_difference(tgt_max, engine_val, epsilon)

        # No error if we are in range
        if engine_val >= tgt_min and engine_val <= tgt_max:
            err = 0.
        elif engine_val > tgt_max:
            err = max_err
        elif engine_val < tgt_min:
            err = min_err

        # Close enough
        if abs(err) < epsilon:
            err = 0.

        err_str = generate_percentage_span(err, percent_precision)
    elif compare_type == SESegmentValidationTarget.eComparisonType.NotValidating:
        pass
    else:
        raise ValueError(f"Unknown compare type: {compare_type}")

    if tgt.get_reference():
        references = [ref.strip() for ref in tgt.get_reference().replace("\n", "").split(",")]
        for ref in references:
            if not ref.startswith('['):
                expected_str += f" @cite {ref}"

    # Add comparison type to beginning of expected string
    if "Segment" in compare_type.name:
        compare_str = compare_type.name.replace("Segment", "")
        tgt_seg = tgt.get_target_segment()
        if tgt_seg == 0:
            expected_str = f'{compare_str} Baseline {expected_str}'
        else:
            expected_str = f'{compare_str} Segment {tgt_seg} {expected_str}'
    elif "Value" in compare_type.name:
        expected_str = f'{compare_type.name.replace("Value", "")} {expected_str}'

    return [
        header,
        expected_str if expected_str else "&nbsp;",
        f"{engine_val:.{value_precision}G}",
        err_str if err_str else "&nbsp;",
        change_str if change_str else "&nbsp;",
        tgt.get_notes() if tgt.get_notes() else "&nbsp;"
        ]


if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG, format='%(levelname)s: %(message)s')

    targets_dir = None
    results_dir = None

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
