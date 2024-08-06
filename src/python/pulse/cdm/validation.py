# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import logging
import numpy as np
from enum import Enum
from pathlib import Path
from typing import Any, Dict, List, Optional

import PyPulse
from pulse.cdm.patient import SEPatient
from pulse.cdm.plots import SEPlotter
from pulse.cdm.utils.file_utils import get_scenario_dir
from pulse.cdm.utils.markdown import table
from pulse.cdm.io.engine import serialize_data_request_list_from_file, serialize_data_requested_result_from_file


_pulse_logger = logging.getLogger('pulse')


class SEValidationTarget:
    __slots__ = ["_header", "_reference", "_notes", "_table_formatting",
                 "_target", "_target_min", "_target_max"]

    def __init__(self):
        self.clear()

    def __repr__(self) -> str:
        return f'SEValidationTarget({self._header}, {self._reference}, {self._notes}, ' \
               f'{self._table_formatting}, {self._target}, {self._target_min}, {self._target_max})'

    def __str__(self) -> str:
        return f'SEValidationTarget:' \
                f'\n\tHeader: {self._header}' \
                f'\n\tReference: {self._reference}' \
                f'\n\tNotes: {self._notes}' \
                f'\n\tTable Formatting: {self._table_formatting}' \
                f'\n\tTarget: {self._target}' \
                f'\n\tTarget Range: [{self._target_min}, {self._target_max}]'

    def is_valid(self) -> bool:
        if np.isnan(self._target) and np.isnan(self._target_max):
            return False
        return True

    def clear(self) -> None:
        self._header = ""
        self._reference = ""
        self._notes = ""
        self._table_formatting = None
        self._target = np.nan
        self._target_min = np.nan
        self._target_max = np.nan

    def get_header(self) -> str:
        return self._header
    def set_header(self, h: str):
        self._header = h

    def has_reference(self) -> bool:
        return self._reference != ""
    def get_reference(self) -> str:
        return self._reference
    def set_reference(self, r: str):
        self._reference = r

    def get_notes(self) -> str:
        return self._notes
    def set_notes(self, n: str):
        self._notes = n

    def has_table_formatting(self) -> bool:
        return self._table_formatting is not None
    def get_table_formatting(self) -> Optional[str]:
        return self._table_formatting
    def set_table_formatting(self, specification: str) -> None:
        self._table_formatting = specification
    def invalidate_table_formatting(self) -> None:
        self._table_formatting = None

    def get_target_maximum(self) -> float:
        return self._target_max
    def get_target_minimum(self) -> float:
        return self._target_min
    def get_target(self) -> float:
        return self._target


class SESegmentValidationTarget(SEValidationTarget):
    __slots__ = ["_comparison_type", "_target_segment", "_computed_value", "_error_value"]

    class eComparisonType(Enum):
        NotValidating = 0
        EqualToValue = 1
        EqualToSegment = 2
        GreaterThanValue = 3
        GreaterThanSegment = 4
        LessThanValue = 5
        LessThanSegment = 6
        TrendsToValue = 7
        TrendsToSegment = 8
        Range = 9

    def __init__(self):
        super().__init__()
        self.clear()

    def __repr__(self):
        return f'SESegmentValidationTarget({super().__repr__()}, {self._segment}, {self._comparison_type}' \
               f'{self._computed_value}, {self._error_value})'

    def clear(self):
        super().clear()
        self._comparison_type = self.eComparisonType.NotValidating
        self._target_segment = np.nan
        self._computed_value = None
        self._error_value = None

    def get_comparison_type(self) -> eComparisonType:
        return self._comparison_type
    def get_target_segment(self) -> int:
        return self._target_segment

    def set_equal_to_segment(self, segment: int):
        self._comparison_type = self.eComparisonType.EqualToSegment
        self._target = np.nan
        self._target_max = np.nan
        self._target_min = np.nan
        self._target_segment = segment
    def set_equal_to_value(self, d: float):
        self._comparison_type = self.eComparisonType.EqualToValue
        self._target = d
        self._target_max = d
        self._target_min = d
        self._target_segment = 0
    def set_greater_than_segment(self, segment: int):
        self._comparison_type = self.eComparisonType.GreaterThanSegment
        self._target = np.nan
        self._target_max = np.nan
        self._target_min = np.nan
        self._target_segment = segment
    def set_greater_than_value(self, d: float):
        self._comparison_type = self.eComparisonType.GreaterThanValue
        self._target = d
        self._target_max = d
        self._target_min = d
        self._target_segment = 0
    def set_less_than_segment(self, segment: int):
        self._comparison_type = self.eComparisonType.LessThanSegment
        self._target = np.nan
        self._target_max = np.nan
        self._target_min = np.nan
        self._target_segment = segment
    def set_less_than_value(self, d: float):
        self._comparison_type = self.eComparisonType.LessThanValue
        self._target = d
        self._target_max = d
        self._target_min = d
        self._target_segment = 0
    def set_trends_to_segment(self, segment: int):
        self._comparison_type = self.eComparisonType.TrendsToSegment
        self._target = np.nan
        self._target_max = np.nan
        self._target_min = np.nan
        self._target_segment = segment
    def set_trends_to_value(self, d: float):
        self._comparison_type = self.eComparisonType.TrendsToValue
        self._target = d
        self._target_max = d
        self._target_min = d
        self._target_segment = 0
    def set_range(self, min: float, max: float):
        self._comparison_type = self.eComparisonType.Range
        self._target = np.nan
        self._target_max = max
        self._target_min = min
        self._target_segment = 0

    def has_computed_value(self) -> bool:
        return self._computed_value is not None
    def get_computed_value(self) -> Optional[float]:
        return self._computed_value
    def set_computed_value(self, val: float) -> None:
        self._computed_value = val
    def invalidate_computed_value(self) -> None:
        self._computed_value = None

    def has_error_value(self) -> bool:
        return self._error_value is not None
    def get_error_value(self) -> Optional[float]:
        return self._error_value
    def set_error_value(self, err: float) -> None:
        self._error_value = err
    def invalidate_error_value(self) -> None:
        self._error_value = None


class SESegmentValidationSegment:
    __slots__ = ["_segment_id", "_notes", "_validation_targets", "_actions"]

    def __init__(self):
        self._segment_id = 0
        self._notes = ""
        self._validation_targets = []

        # Not serializing
        self._actions = []

    def clear(self) -> None:
        self._notes = ""
        self._validation_targets = []

        self._actions = []

    def get_segment_id(self) -> int:
        return self._segment_id
    def set_segment_id(self, id: int) -> None:
        self._segment_id = id

    def has_notes(self) -> bool:
        return len(self._notes) > 0
    def get_notes(self) -> str:
        return self._notes
    def set_notes(self, notes: str) -> None:
        self._notes = notes
    def invalidate_notes(self) -> None:
        self._notes = ""

    def has_validation_targets(self) -> bool:
        return len(self._validation_targets) > 0
    def get_validation_targets(self) -> List[SESegmentValidationTarget]:
        return self._validation_targets
    def add_validation_target(self, tgt: SESegmentValidationTarget) -> None:
        self._validation_targets.append(tgt)
    def set_validation_targets(self, tgts: List[SESegmentValidationTarget]) -> None:
        self._validation_targets = tgts
    def invalidate_validation_targets(self) -> None:
        self._validation_targets = []

    def has_actions(self) -> bool:
        return len(self._actions) > 0
    def get_actions(self) -> List[str]:
        return self._actions
    def set_actions(self, actions: List[str]) -> None:
        self._actions = actions
    def invalidate_actions(self) -> None:
        self._actions = []

class SESegmentValidationSegmentTable:
    __slots__ = ["_table_name", "_scenario_name", "_segment", "_headers", "_dr_files"]

    def __init__(self):
        self.clear()

    def clear(self) -> None:
        self._table_name = None
        self._scenario_name = None
        self._segment = None
        self._headers = list()
        self._dr_files = list()

    def has_table_name(self) -> bool:
        return self._table_name is not None and len(self._table_name) > 0
    def get_table_name(self) -> Optional[str]:
        return self._table_name
    def set_table_name(self, name: str) -> None:
        self._table_name = name
    def invalidate_table_name(self) -> None:
        self._table_name = None

    def has_scenario_name(self) -> bool:
        return self._scenario_name is not None and len(self._scenario_name) > 0
    def get_scenario_name(self) -> Optional[str]:
        return self._scenario_name
    def set_scenario_name(self, name: str) -> None:
        self._scenario_name = name
    def invalidate_scenario_name(self) -> None:
        self._scenario_name = None

    def has_segment(self) -> bool:
        return self._segment is not None
    def get_segment(self) -> Optional[int]:
        return self._segment
    def set_segment(self, segment: int) -> None:
        self._segment = segment
    def invalidate_segment(self) -> None:
        self._segment = None

    def get_headers(self) -> List[str]:
        return self._headers
    def invalidate_headers(self) -> None:
        self._headers = None

    def get_data_request_files(self) -> List[str]:
        return self._dr_files
    def invalidate_data_request_files(self) -> None:
        self._dr_files = None

    def _process_results(self, segments_filename: Path, in_dir: Path) -> Optional[List[List[Any]]]:
        results = serialize_data_requested_result_from_file(segments_filename)
        result = results.get_segment(self.get_segment())
        if result is None:
            _pulse_logger.error(f"Could not find result for segment {self.get_segment()}")
            return None

        def _get_engine_value(dr_header: str, precision: int=3) -> List[str]:
            header_idx = results.get_header_index(dr_header)

            if header_idx is None:
                _pulse_logger.error(f"Could not find results for {dr_header} in segment {self.get_segment()}")
                return None

            engine_val = result.values[header_idx]

            # Convert unit if needed
            paren_idx = dr_header.find("(")
            if paren_idx != -1 and engine_val != "NaN":
                table_unit = dr_header[paren_idx+1:-1].replace("_", " ")
                engine_full_header = results.get_headers()[header_idx]
                engine_paren_idx = engine_full_header.find("(")
                if engine_paren_idx == -1:
                    raise ValueError(f"Cannot convert between {table_unit} and unitless for {dr_header}")
                engine_unit = engine_full_header[engine_paren_idx+1:-1].replace("_", " ")
                if engine_unit != table_unit:
                    engine_val = PyPulse.convert(engine_val, engine_unit, table_unit)

            return [
                dr_header,
                f"{engine_val:.{precision}G}" if engine_val != "NaN" else "NaN"
            ]

        table_data = list()
        for header in self.get_headers():
            table_data.append(_get_engine_value(header))

        alt_locations = [in_dir, Path(get_scenario_dir())]
        for dr_file in self.get_data_request_files():
            dr_path = Path(dr_file)
            if not dr_path.is_file():
                for dir in alt_locations:
                    dr_path = dir / dr_file
                    if dr_path.is_file():
                        break
                else:
                    _pulse_logger.error(f"Could not find data request file: {dr_file}")
                    return None
            drs = list()
            serialize_data_request_list_from_file(dr_path, drs)
            for dr in drs:
                table_data.append(_get_engine_value(str(dr), dr.get_precision()))

        return table_data

    def write_table(self, validate_dir: Path, in_dir: Path, out_dir: Path) -> bool:
        if not self.has_scenario_name():
            _pulse_logger.error("Can't write segment table without scenario specified")
            return False
        if not self.has_segment():
            _pulse_logger.error("Can't write segment table without segment specified")
            return False
        segments_file = validate_dir / f"{self.get_scenario_name()}Results-Segments.json"
        if not segments_file.is_file():
            _pulse_logger.error(f"Could not find segments file: {segments_file}")
            return False

        if not self.has_table_name():
            t_name = f"SegmentTable{self.get_segment()}"
            _pulse_logger.warning(f"Found table with no name, saving as: {t_name}")
            self.set_table_name(t_name)

        table_headers = ["Property Name", "Engine Value"]
        fields = list(range(len(table_headers)))
        align = [('<', '<')] * len(table_headers)
        table_data = self._process_results(segments_file, in_dir)
        if table_data is None:
            return False

        md_filename = out_dir / f"{self.get_table_name()}.md"
        with open(md_filename, "w") as md_file:
            _pulse_logger.info(f"Writing {md_filename}")
            lines = [
                f"<center>\n<i>@tabledef {{{self.get_table_name()}}}. Data requests for Segment {self.get_segment()}.</i>\n</center>\n\n"
            ]
            md_file.writelines(lines)
            table(md_file, table_data, fields, table_headers, align)

        return True


class SESegmentValidationPipelineConfig:
    __slots__ = ["_plotters", "_tables"]
    def __init__(self):
        self.clear()

    def clear(self) -> None:
        self._plotters = list()
        self._tables = list()

    def get_plotters(self) -> List[SEPlotter]:
        return self._plotters

    def get_tables(self) -> List[SESegmentValidationSegmentTable]:
        return self._tables


class SETimeSeriesValidationTarget(SEValidationTarget):
    __slots__ = ["_target_type", "_comparison_type", "_patient_specific",
                 "_computed_value", "_error_value"]

    class eComparisonType(Enum):
        NotValidating = 0
        EqualToValue = 1
        Range = 5

    class eTargetType(Enum):
        Mean = 0
        Minimum = 1
        Maximum = 2
        MeanPerIdealWeight_kg = 3
        MinPerIdealWeight_kg = 4
        MaxPerIdealWeight_kg = 5

    def __init__(self):
        super().__init__()
        self.clear()

    def clear(self):
        super().clear()
        self._comparison_type = self.eComparisonType.NotValidating
        self._target_type = self.eTargetType.Mean
        self._computed_value = None
        self._error_value = None
        self._patient_specific = None

    def is_evaluated(self) -> bool:
        return self.has_computed_value() and self.has_error_value()

    def get_comparison_type(self) -> eComparisonType:
        return self._comparison_type
    def get_target_type(self) -> eTargetType:
        return self._target_type

    def set_equal_to(self, d: float, t: eTargetType):
        self._comparison_type = self.eComparisonType.EqualToValue
        self._target_type = t
        self._target = d
        self._target_max = d
        self._target_min = d
    def set_range(self, min: float, max: float, t: eTargetType):
        self._comparison_type = self.eComparisonType.Range
        self._target_type = t
        self._target = np.nan
        self._target_max = max
        self._target_min = min

    def has_computed_value(self) -> bool:
        return self._computed_value is not None
    def get_computed_value(self) -> Optional[float]:
        return self._computed_value
    def set_computed_value(self, val: float) -> None:
        self._computed_value = val
    def invalidate_computed_value(self) -> None:
        self._computed_value = None

    def has_error_value(self) -> bool:
        return self._error_value is not None
    def get_error_value(self) -> Optional[float]:
        return self._error_value
    def set_error_value(self, err: float) -> None:
        self._error_value = err
    def invalidate_error_value(self) -> None:
        self._error_value = None

    def has_patient_specific_setting(self) -> bool:
        return self._patient_specific is not None
    def is_patient_specific(self) -> Optional[bool]:
        return self._patient_specific
    def set_patient_specific_setting(self, patient_specific: bool) -> None:
        self._patient_specific = patient_specific
    def invalidate_patient_specific_setting(self) -> None:
        self._patient_specific = None


class SEPatientTimeSeriesValidation:
    __slots__ = ["_patient", "_targets"]

    def __init__(self):
        self.clear()

    def clear(self) -> None:
        self._patient = None
        self._targets = dict()

    def get_targets(self) -> Dict[str, List[SETimeSeriesValidationTarget]]:
        return self._targets
    def invalidate_targets(self) -> None:
        self._targets = dict()

    def has_patient(self) -> bool:
        return self._patient is not None
    def get_patient(self) -> SEPatient:
        from pulse.cdm.patient import SEPatient
        if self._patient is None:
            self._patient = SEPatient()
        return self._patient
    def invalidate_patient(self) -> None:
        self._patient = None
