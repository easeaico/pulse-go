# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import logging
import operator
import numpy as np
from enum import Enum
from pathlib import Path
from string import Template
from typing import Any, Dict, List, NamedTuple, Optional, Tuple

from pulse.cdm.engine import SEEventChange


_pulse_logger = logging.getLogger('pulse')


class SEUnstructuredTextProperty:
    __slots__ = ("_property", "_property_type", "_comparison_type", "_target_min", "_target_max",
                "_target_min_inclusive", "_target_max_inclusive")

    class eComparisonType(Enum):
        AnyValue = 0
        GreaterThan = 1
        GreaterThanEqualTo = 2
        LessThan = 3
        LessThanEqualTo = 4
        EqualTo = 5
        Range = 6
        Active = 7
        Inactive = 8

    class ePropertyType(Enum):
        DataRequest = 0
        Event = 1

    def __init__(self):
        self.clear()

    def __repr__(self):
        return f'SEUnstructuredTextProperty({self._property}, {self._property_type}, {self._comparison_type}, ' \
               f'{self._target_min}, {self._target_max})'

    def clear(self) -> None:
        self._property = None
        self._property_type = self.ePropertyType.DataRequest
        self._comparison_type = self.eComparisonType.AnyValue
        self._target_min = np.nan
        self._target_max = np.nan
        self._target_min_inclusive = False
        self._target_max_inclusive = False

    def has_property(self) -> bool:
        return self._property is not None
    def get_property(self) -> Optional[str]:
        return self._property
    def set_property(self, p: str) -> None:
        self._property = p
    def invalidate_property(self) -> None:
        self._property = None

    def get_property_type(self) -> ePropertyType:
        return self._property_type
    def set_property_type(self, prop_type: ePropertyType) -> None:
        self._property_type = prop_type

    def get_comparison_type(self) -> eComparisonType:
        return self._comparison_type

    def set_any_value(self) -> None:
        self._comparison_type = self.eComparisonType.AnyValue
        self._target_min = np.nan
        self._target_max = np.nan
    def set_gt(self, v: float) -> None:
        self._comparison_type = self.eComparisonType.GreaterThan
        self._target_min = v
        self._target_max = np.nan
    def set_ge(self, v: float) -> None:
        self._comparison_type = self.eComparisonType.GreaterThanEqualTo
        self._target_min = v
        self._target_max = np.nan
    def set_lt(self, v: float) -> None:
        self._comparison_type = self.eComparisonType.LessThan
        self._target_min = np.nan
        self._target_max = v
    def set_le(self, v: float) -> None:
        self._comparison_type = self.eComparisonType.LessThanEqualTo
        self._target_min = np.nan
        self._target_max = v
    def set_eq(self, v: float) -> None:
        self._comparison_type = self.eComparisonType.EqualTo
        self._target_min = v
        self._target_max = v
    def set_range(self, min: float, max: float, min_inclusive: bool, max_inclusive: bool) -> None:
        self._comparison_type = self.eComparisonType.Range
        self._target_min = min
        self._target_max = max
        self._target_min_inclusive = min_inclusive
        self._target_max_inclusive = max_inclusive
    def set_active(self, active: bool) -> None:
        if active:
            self._comparison_type = self.eComparisonType.Active
        else:
            self._comparison_type = self.eComparisonType.Inactive
        self._target_min = np.nan
        self._target_max = np.nan

    def evaluate(
        self,
        data_slice: NamedTuple,
        slice_idx: Dict[str, int],
        events: List[SEEventChange],
    ) -> bool:
        if self._property_type == self.ePropertyType.DataRequest:
            # TODO: If DR not in slice do we error or does this set of phrases just not apply?
            value = data_slice[slice_idx[self._property]]

            if self._comparison_type == self.eComparisonType.AnyValue:
                return True
            elif self._comparison_type == self.eComparisonType.GreaterThan:
                return value > self._target_min
            elif self._comparison_type == self.eComparisonType.GreaterThanEqualTo:
                return value >= self._target_min
            elif self._comparison_type == self.eComparisonType.LessThan:
                return value < self._target_max
            elif self._comparison_type == self.eComparisonType.LessThanEqualTo:
                return value <= self._target_max
            elif self._comparison_type == self.eComparisonType.EqualTo:
                return value == self._target_min  # TODO: Floating point errors?
            elif self._comparison_type == self.eComparisonType.Range:
                min_comparator = operator.ge if self._target_min_inclusive else operator.gt
                max_comparator = operator.le if self._target_max_inclusive else operator.lt
                return min_comparator(value, self._target_min) and max_comparator(value, self._target_max)
            else:
                raise ValueError(f"Invalid comparison type: {self._comparison_type}")
        elif self._property_type == self.ePropertyType.Event:
            # TODO: What if there are multiple events that match?
            for event in events:
                if self._property == event.event.name:
                    if self._comparison_type == self.eComparisonType.AnyValue:
                        return True
                    elif self._comparison_type == self.eComparisonType.Active and event.active:
                        return event.active
                    elif self._comparison_type == self.eComparisonType.Inactive:
                        return not event.active
                    else:
                        raise ValueError(f"Invalid comparison type: {self._comparison_type}")
        else:
            raise ValueError(f"Unknown property type: {self._property_type}")


class SEUnstructuredTextGroup:
    __slots__ = ("_properties", "_phrases")

    def __init__(self):
        self.clear()

    def __repr__(self):
        return f'SEUnstructuredTextGroup({self._properties}, {self._phrases})'

    def clear(self) -> None:
        self._properties = list()
        self._phrases = list()

    def get_properties(self) -> List[SEUnstructuredTextProperty]:
        return self._properties
    def set_properties(self, props: List[SEUnstructuredTextProperty]) -> None:
        self._properties = props
    def invalidate_properties(self) -> None:
        self._properties = list()

    def get_phrases(self) -> List[List[Template]]:
        return self._phrases
    def set_phrases(self, phrases: List[List[Template]]) -> None:
        self._phrases = phrases
    def invalidate_phrases(self) -> None:
        self._phrases = list()

    def evaluate(
        self,
        data_slice: NamedTuple,
        slice_idx: Dict[str, int],
        events: List[SEEventChange],
    ) -> bool:
        for prop in self._properties:
            if not prop.evaluate(data_slice=data_slice, slice_idx=slice_idx, events=events):
                return False

        return True
