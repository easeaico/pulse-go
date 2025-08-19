# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import abc
import dataframe_image as dfi
import logging
import pandas as pd

from enum import Enum
from typing import List

import PyPulse
from pulse.cdm.engine import SEAction, SEDataRequest
from pulse.cdm.physiology import eHeartRhythm
from pulse.cdm.scalars import PressureUnit, FrequencyUnit, VolumePerTimeUnit, VolumeUnit, TimeUnit
from pulse.cdm.utils.markdown import table


_log = logging.getLogger("pulse")


class AVPU(str, Enum):
    Alert = "Alert"
    Voice = "Voice"
    Pain = "Pain"
    Unresponsive = "Unresponsive"


class Intervention(str, Enum):
    NeedleDecompress = "needle_decompression"
    RepositionAirway = "reposition_airway"
    Tourniquet = "tourniquet"
    WoundPack = "wound_pack"


class TriageColor(str, Enum):
    Green = "Green"
    Yellow = "Yellow"
    Red = "Red"
    Black = "Black"


class TriageTag:
    __slots__ = ["_color", "_reason"]

    def __init__(self):
        self._color = None
        self._reason = None

    def apply(self, t: TriageColor, reason: str):
        if self._color is None:
            self._color = t
            self._reason = reason
            return

        elif self._color == TriageColor.Yellow:
            if t == TriageColor.Red or TriageColor.Black:
                self._color = t
                self._reason = reason
            return

        elif self._color == TriageColor.Red:
            if t == TriageColor.Black:
                self._color = t
                self._reason = reason

    @property
    def color(self): return self._color

    @property
    def reason(self): return self._reason


class PulseData:
    __slots__ = ["_values", "_data_requests"]

    def __init__(self):
        self._values = None
        # Make sure the accessor methods below are in sync with this order and units
        # !!! ALSO UPDATE THE DEATH CHECKER !!!
        self._data_requests = [
            SEDataRequest.create_physiology_request("HeartRate", unit=FrequencyUnit.Per_min),
            SEDataRequest.create_physiology_request("HeartRhythm"),
            SEDataRequest.create_physiology_request("CardiacOutput", unit=VolumePerTimeUnit.L_Per_min),
            SEDataRequest.create_physiology_request("ArterialPressure", unit=PressureUnit.mmHg),
            SEDataRequest.create_physiology_request("MeanArterialPressure", unit=PressureUnit.mmHg),
            SEDataRequest.create_physiology_request("SystolicArterialPressure", unit=PressureUnit.mmHg),
            SEDataRequest.create_physiology_request("DiastolicArterialPressure", unit=PressureUnit.mmHg),
            SEDataRequest.create_physiology_request("BloodVolume", unit=VolumeUnit.mL),
            SEDataRequest.create_physiology_request("TotalHemorrhageRate", unit=VolumePerTimeUnit.mL_Per_min),
            SEDataRequest.create_physiology_request("TotalHemorrhagedVolume", unit=VolumeUnit.mL),
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

    def get_time(self, unit: TimeUnit):
        return self._get_value(0, TimeUnit.s, unit)

    def get_hr(self, unit: FrequencyUnit):
        return self._get_value(1, FrequencyUnit.Per_min, unit)

    def get_heart_rhythm(self):
        return eHeartRhythm(self._values[2])

    def get_cardiac_output(self, unit: VolumePerTimeUnit):
        return self._get_value(3, VolumePerTimeUnit.L_Per_min, unit)

    def get_arterial_pressure(self, unit: PressureUnit):
        return self._get_value(4, PressureUnit.mmHg, unit)

    def get_map(self, unit: PressureUnit):
        return self._get_value(5, PressureUnit.mmHg, unit)

    def get_systolic_pressure(self, unit: PressureUnit):
        return self._get_value(6, PressureUnit.mmHg, unit)

    def get_diastolic_pressure(self, unit: PressureUnit):
        return self._get_value(7, PressureUnit.mmHg, unit)

    def get_blood_volume(self, unit: VolumeUnit):
        return self._get_value(8, VolumeUnit.mL, unit)

    def get_hemorrhage_rate(self, unit: VolumePerTimeUnit):
        return self._get_value(9, VolumePerTimeUnit.mL_Per_min, unit)

    def get_hemorrhaged_volume(self, unit: VolumeUnit):
        return self._get_value(10, VolumeUnit.mL, unit)

    def get_rr(self, unit: FrequencyUnit):
        return self._get_value(11, FrequencyUnit.Per_min, unit)

    def get_etco2(self, unit: PressureUnit):
        return self._get_value(12, PressureUnit.mmHg, unit)

    def get_spo2(self):
        return self._values[13]

    def get_ppi(self):
        return self._values[14]

    def get_brain_o2_pp(self, unit: PressureUnit):
        return self._get_value(15, PressureUnit.mmHg, unit)


class TriageDataset(metaclass=abc.ABCMeta):

    @abc.abstractmethod
    def generate_dataset(self, population_size: int):
        pass

    @abc.abstractmethod
    def injury_description(self,
                           duration_min: float,
                           injuries: List[dict],
                           actions: List[dict],
                           vitals: dict) -> List[str]:
        pass

    @staticmethod
    def vitals_description(vitals: dict) -> List[str]:
        pass

    @abc.abstractmethod
    def injury_actions(self, injuries: list) -> List[SEAction]:
        pass

    @abc.abstractmethod
    def calculate_triage_vitals(self, synthetic_patient: dict, active_events: dict, pulse_data: PulseData):
        pass

    @abc.abstractmethod
    def injury_interventions(self, synthetic_injuries: list, pulse_injuries: list, vitals: dict):
        pass


def convert_keys_to_int(obj):
    new_obj = {}
    for k, v in obj.items():
        try:
            new_obj[int(k)] = v
        except ValueError:
            new_obj[k] = v
    return new_obj


def create_report(basename: str, data, fields, headings, widths=None):
    align = []
    for i in range(len(fields)):
        align.append(('^', '^'))
    f = open(str(basename) + ".md", "w")
    table(f, data, fields, headings, align)
    f.close()

    # Write out table as png
    wrapped_headers = headings  # ["<br>".join(textwrap.wrap(h, width=20)) for h in headings]
    df = pd.DataFrame(data, columns=wrapped_headers)
    df.style.format(escape="html")  # Actually wrap column names
    df_styler = df.style.hide(axis="index") \
        .set_properties(subset=wrapped_headers[1:], **{'text-align': 'center'}) \
        .set_properties(subset=[wrapped_headers[0]], **{'text-align': 'left'}) \
        .set_properties(**{'border': '1px black solid'})
    if widths:
        for i, width in enumerate(widths):
            df_styler = df_styler.set_properties(subset=wrapped_headers[i], **{'width': width})
    df_styler.set_table_styles(table_styles=[
        {'selector': 'th.col_heading', 'props': 'text-align: center; border: 1px black solid;'},
    ], overwrite=False)
    img_filename = str(basename) + ".png"
    _log.info(f"Writing {img_filename}")
    dfi.export(df_styler, img_filename, table_conversion='chrome', chrome_path=None, fontsize=4, max_cols=200, max_rows=200, dpi=600)
