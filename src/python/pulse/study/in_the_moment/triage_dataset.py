# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import abc

from enum import Enum
from pathlib import Path
from typing import List


import PyPulse
from pulse.cdm.engine import SEAction, SEDataRequest
from pulse.cdm.scalars import PressureUnit, FrequencyUnit, VolumePerTimeUnit, VolumeUnit


class AVPU(str, Enum):
    Alert = "Alert"
    Voice = "Voice"
    Pain = "Pain"
    Unresponsive = "Unresponsive"


class Breathing(str, Enum):
    Obstructed = "Obstructed"
    Distressed = "Distressed"
    Slow = "Slow"
    Normal = "Normal"
    Fast = "Fast"


class Hemorrhage(str, Enum):
    Minor = "Minor"
    Major = "Major"


class TriageColor(str, Enum):
    Black = "Black"
    Red = "Red"
    Yellow = "Yellow"
    Green = "Green"


class TriageTag:
    __slots__ = ["_color"]

    def __init__(self):
        self._color = TriageColor.Green

    def apply(self, t: TriageColor):
        if self._color == TriageColor.Green:
            self._color = t
        elif self._color == TriageColor.Yellow:
            if t == TriageColor.Red or TriageColor.Black:
                self._color = t
        elif self._color == TriageColor.Red:
            if t == TriageColor.Black:
                self._color = t

    @property
    def color(self): return self._color


class PulseData:
    __slots__ = ["_values", "_data_requests"]

    def __init__(self):
        self._values = None
        # Make sure the accessor methods below are in sync with this order and units
        self._data_requests = [
            SEDataRequest.create_physiology_request("HeartRate", unit=FrequencyUnit.Per_min),
            SEDataRequest.create_physiology_request("CardiacOutput", unit=VolumePerTimeUnit.L_Per_min),
            SEDataRequest.create_physiology_request("ArterialPressure", unit=PressureUnit.mmHg),
            SEDataRequest.create_physiology_request("MeanArterialPressure", unit=PressureUnit.mmHg),
            SEDataRequest.create_physiology_request("SystolicArterialPressure", unit=PressureUnit.mmHg),
            SEDataRequest.create_physiology_request("DiastolicArterialPressure", unit=PressureUnit.mmHg),
            SEDataRequest.create_physiology_request("BloodVolume", unit=VolumeUnit.mL),
            SEDataRequest.create_physiology_request("TotalHemorrhageRate", unit=VolumePerTimeUnit.L_Per_min),
            SEDataRequest.create_physiology_request("RespirationRate", unit=FrequencyUnit.Per_min),
            SEDataRequest.create_physiology_request("EndTidalCarbonDioxidePressure", unit=PressureUnit.mmHg),
            SEDataRequest.create_physiology_request("OxygenSaturation"),
            SEDataRequest.create_physiology_request("PeripheralPerfusionIndex"),
            SEDataRequest.create_liquid_compartment_substance_request("BrainVasculature",
                                                                      "Oxygen", "PartialPressure",
                                                                      unit=PressureUnit.mmHg),
        ]

    @property
    def data_requests(self): return self._data_requests

    def set_values(self, values: list):
        self._values = values

    def _get_value(self, idx, dr_unit, to_unit):
        if to_unit == dr_unit:
            return self._values[idx]
        return PyPulse.convert(self._values[idx],
                               dr_unit.get_string(),
                               to_unit.get_string())

    def get_hr(self, unit: FrequencyUnit):
        return self._get_value(1, FrequencyUnit.Per_min, unit)

    def get_cardiac_output(self, unit: VolumePerTimeUnit):
        return self._get_value(2, VolumePerTimeUnit.L_Per_min, unit)

    def get_arterial_pressure(self, unit: PressureUnit):
        return self._get_value(3, PressureUnit.mmHg, unit)

    def get_map(self, unit: PressureUnit):
        return self._get_value(4, PressureUnit.mmHg, unit)

    def get_systolic_pressure(self, unit: PressureUnit):
        return self._get_value(5, PressureUnit.mmHg, unit)

    def get_diastolic_pressure(self, unit: PressureUnit):
        return self._get_value(6, PressureUnit.mmHg, unit)

    def get_blood_volume(self, unit: VolumeUnit):
        return self._get_value(7, VolumeUnit.mL, unit)

    def get_hemorrhage_rate(self, unit: VolumePerTimeUnit):
        return self._get_value(8, VolumePerTimeUnit.L_Per_min, unit)

    def get_rr(self, unit: FrequencyUnit):
        return self._get_value(9, FrequencyUnit.Per_min, unit)

    def get_etco2(self, unit: PressureUnit):
        return self._get_value(10, PressureUnit.mmHg, unit)

    def get_spo2(self):
        return self._values[11]

    def get_ppi(self):
        return self._values[12]

    def get_brain_o2_pp(self, unit: PressureUnit):
        return self._get_value(13, PressureUnit.mmHg, unit)


class TriageDataset(metaclass=abc.ABCMeta):

    @abc.abstractmethod
    def generate_dataset(self, population_size: int, output_dir: Path):
        pass

    @abc.abstractmethod
    def injury_description(self,
                           duration_min: float,
                           injuries: List[dict],
                           actions: List[dict],
                           vitals: dict) -> List[str]:
        pass

    @abc.abstractmethod
    def injury_actions(self, injuries: list) -> List[SEAction]:
        pass

    @abc.abstractmethod
    def calculate_triage_vitals(self, synthetic_patient: dict, active_events: dict, pulse_data: PulseData):
        pass

    @abc.abstractmethod
    def can_perform_interventions(self, synthetic_injuries: list) -> bool:
        pass

    @abc.abstractmethod
    def injury_interventions(self, synthetic_injuries: list, pulse_injuries: list, vitals: dict):
        pass
