# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import math
import logging
import operator
from enum import Enum
from pathlib import Path
from google.protobuf import json_format
from typing import List, Dict, Optional, Tuple, Union

from pulse.cdm.patient import eSex, SEPatient
from pulse.cdm.validation import SEPatientTimeSeriesValidation
from pulse.cdm.io.validation import serialize_patient_time_series_validation_list_from_file
from pulse.cdm.scalars import AreaUnit, FrequencyUnit, LengthUnit, MassUnit, MassPerVolumeUnit, \
                              PowerUnit, PressureUnit, TimeUnit, VolumeUnit


_pulse_logger = logging.getLogger('pulse')


class Field(Enum):
    Age_yr = 1
    Weight_kg = 2
    Height_cm = 3
    BodyDensity_g_Per_cm3 = 4
    BodyFatFraction = 5
    BodyMassIndex = 6
    LeanBodyMass_kg = 7
    IdealBodyWeight_kg = 8
    AlveoliSurfaceArea_m2 = 9
    RightLungRatio = 10
    SkinSurfaceArea_m2 = 11
    BasalMetabolicRate_kcal_Per_day = 12
    BloodVolumeBaseline_mL = 13
    DiastolicArterialPressureBaseline_mmHg = 14
    HeartRateBaseline_bpm = 15
    MeanArterialPressureBaseline_mmHg = 16
    PulsePressureBaseline_mmHg = 17
    RespirationRateBaseline_bpm = 18
    SystolicArterialPressureBaseline_mmHg = 19
    TidalVolumeBaseline_L = 20
    HeartRateMaximum_bpm = 21
    HeartRateMinimum_bpm = 22
    ExpiratoryReserveVolume_L = 23
    FunctionalResidualCapacity_L = 24
    InspiratoryCapacity_L = 25
    InspiratoryReserveVolume_L = 26
    ResidualVolume_L = 27
    TotalLungCapacity_L = 28
    VitalCapacity_L = 29


def _get_patient_values(patient: SEPatient):
    patient_values = {
        Field.Age_yr: patient.get_age().get_value(TimeUnit.yr),
        Field.Weight_kg: patient.get_weight().get_value(MassUnit.kg),
        Field.Height_cm: patient.get_height().get_value(LengthUnit.cm),
        Field.BodyDensity_g_Per_cm3: patient.get_body_density().get_value(MassPerVolumeUnit.g_Per_cm3),
        Field.BodyFatFraction: patient.get_body_fat_fraction().get_value(),
        Field.BodyMassIndex: patient.get_body_mass_index().get_value(),
        Field.LeanBodyMass_kg: patient.get_lean_body_mass().get_value(MassUnit.kg),
        Field.IdealBodyWeight_kg: patient.get_ideal_body_weight().get_value(MassUnit.kg),
        Field.AlveoliSurfaceArea_m2: patient.get_alveoli_surface_area().get_value(AreaUnit.m2),
        Field.RightLungRatio: patient.get_right_lung_ratio().get_value(),
        Field.SkinSurfaceArea_m2: patient.get_skin_surface_area().get_value(AreaUnit.m2),
        Field.BasalMetabolicRate_kcal_Per_day: patient.get_basal_metabolic_rate().get_value(PowerUnit.kcal_Per_day),
        Field.BloodVolumeBaseline_mL: patient.get_blood_volume_baseline().get_value(VolumeUnit.mL),
        Field.DiastolicArterialPressureBaseline_mmHg: patient.get_diastolic_arterial_pressure_baseline().get_value(PressureUnit.mmHg),
        Field.HeartRateBaseline_bpm: patient.get_heart_rate_baseline().get_value(FrequencyUnit.Per_min),
        Field.MeanArterialPressureBaseline_mmHg: patient.get_mean_arterial_pressure_baseline().get_value(PressureUnit.mmHg),
        Field.PulsePressureBaseline_mmHg: patient.get_pulse_pressure_baseline().get_value(PressureUnit.mmHg),
        Field.RespirationRateBaseline_bpm: patient.get_respiration_rate_baseline().get_value(FrequencyUnit.Per_min),
        Field.SystolicArterialPressureBaseline_mmHg: patient.get_systolic_arterial_pressure_baseline().get_value(PressureUnit.mmHg),
        Field.TidalVolumeBaseline_L: patient.get_tidal_volume_baseline().get_value(VolumeUnit.L),
        Field.HeartRateMaximum_bpm: patient.get_heart_rate_maximum().get_value(FrequencyUnit.Per_min),
        Field.HeartRateMinimum_bpm: patient.get_heart_rate_minimum().get_value(FrequencyUnit.Per_min),
        Field.ExpiratoryReserveVolume_L: patient.get_expiratory_reserve_volume().get_value(VolumeUnit.L),
        Field.FunctionalResidualCapacity_L: patient.get_functional_residual_capacity().get_value(VolumeUnit.L),
        Field.InspiratoryCapacity_L: patient.get_inspiratory_capacity().get_value(VolumeUnit.L),
        Field.InspiratoryReserveVolume_L: patient.get_inspiratory_reserve_volume().get_value(VolumeUnit.L),
        Field.ResidualVolume_L: patient.get_residual_volume().get_value(VolumeUnit.L),
        Field.TotalLungCapacity_L: patient.get_total_lung_capacity().get_value(VolumeUnit.L),
        Field.VitalCapacity_L: patient.get_vital_capacity().get_value(VolumeUnit.L)
    }
    return patient_values


def _key_patient_values(patient_values: dict):
    return (f"{patient_values[Field.Age_yr]}yr_"
            f"{patient_values[Field.Height_cm]}cm_"
            f"{patient_values[Field.BodyMassIndex]}_"
            f"{patient_values[Field.BodyFatFraction]}_"
            f"{patient_values[Field.HeartRateBaseline_bpm]}bpm_"
            f"{patient_values[Field.RespirationRateBaseline_bpm]}bpm_"
            f"{patient_values[Field.MeanArterialPressureBaseline_mmHg]}_mmHg")

# Class attributes get added dynamically during analysis
class PropertyError(object):
    def __init__(self):
        self.errors = list()


class Condition:
    _slots = ["field", "_operator", "_value"]

    def __init__(self, field: Field, operator: str, value: float):
        self._field = field
        self._operator = operator
        self._value = value

    def _nequal(self, frm, to) -> bool:
        return not self._equal(frm, to)

    def _equal(self, frm: float, to: float) -> bool:
        if frm is None or to is None:
            return False
        if not math.isclose(frm, to, abs_tol=0.0001):
            return False
        return True

    def _compare(self, left: float, right: float, op: str) -> bool:
        ops = {'>': operator.gt,
               '<': operator.lt,
               '>=': operator.ge,
               '<=': operator.le,
               '==': self._equal,
               '!=': self._nequal}

        return ops[op](left, right)

    def eval(self, patient_sex: eSex, patient_values: dict) -> bool:
        if self._field not in patient_values:
            _pulse_logger.error(f"Unknown field: {self._field}")
            return False
        return self._compare(patient_values[self._field], self._value, self._operator)


class Conditional:
    _slots = ["_conditions", "_conditional_type", "_sex"]

    class ConditionalType(Enum):
        AND = 1
        OR = 2

    def __init__(self, cType: str="AND"):
        self._conditions = list()
        self._conditional_type = self.ConditionalType[cType]
        self._sex = None

    def sex(self, sex: eSex) -> None:
        self._sex = sex

    def add_condition(self, field: Field, operator: str, value: float) -> None:
        self._conditions.append(Condition(field, operator, value))

    def add_conditional(self, conditional: "Conditional") -> None:
        self._conditions.append(conditional)

    def eval(self, patient_sex: eSex, patient_values: dict) -> bool:
        if self._sex is not None and patient_sex != self._sex:
            return False
        if self._conditional_type is self.ConditionalType.AND:
            for condition in self._conditions:
                if not condition.eval(patient_sex, patient_values):
                    return False
            return True
        elif self._conditional_type is self.ConditionalType.OR:
            for condition in self._conditions:
                if condition.eval(patient_sex, patient_values):
                    return True
            return False

        _pulse_logger.error(f"Unknown conditional type: {self._conditional_type}")
        return False

class PatientVariabilityResults:
    __slots__ = ["_validation_results"]

    def __init__(self):
        self._validation_results = list()

    def open_validation_results(self, validation_results_file: Path):
        if not validation_results_file.is_file():
            raise ValueError(f"Can't locate validation results: {validation_results_file}")
        serialize_patient_time_series_validation_list_from_file(validation_results_file, self._validation_results)

    # Get validation result by patient name
    def get_patient_state(self, name: str) -> Optional[SEPatientTimeSeriesValidation]:
        for validation in self._validation_results:
            if validation.get_patient().get_name() == name:
                return validation
        return None

    # Identify parameter bounds of the given parameter in the given list of patients
    def get_field_values(self, field: Field, sex: eSex) -> List[float]:
        v = list()
        for validation in self._validation_results:
            patient = validation.get_patient()
            if patient.get_sex() != sex:
                continue
            if field is Field.Age_yr:
                param = patient.get_age().get_value(TimeUnit.yr)
            elif field is Field.Height_cm:
                param = patient.get_height().get_value(LengthUnit.cm)
            elif field is Field.BodyMassIndex:
                param = patient.get_basal_metabolic_rate().get_value()
            elif field is Field.HeartRateBaseline_bpm:
                param = patient.get_heart_rate_baseline().get_value(FrequencyUnit.Per_min)
            elif field is Field.MeanArterialPressureBaseline_mmHg:
                param = patient.get_mean_arterial_pressure_baseline().get_value(PressureUnit.mmHg)
            elif field is Field.PulsePressureBaseline_mmHg:
                param = patient.get_pulse_pressure_baseline().get_value(PressureUnit.mmHg)
            else:
                raise ValueError(f"Unknown field {field}")

            if param not in v:
                v.append(param)
        v.sort()
        return v

    # Generates list of patients that match any of the conditionals/conditions
    def conditional_filter(self, conditionals: List[Union[Conditional, Condition]]) -> Tuple[List[str], Dict[str, PropertyError]]:
        collected_errors = dict()
        patient_matches = list()

        for validation_result in self._validation_results:
            patient = validation_result.get_patient()
            patient_values = _get_patient_values(patient)

            for conditional in conditionals:
                if conditional.eval(patient.get_sex(), patient_values):
                    patient_matches.append(patient.get_name())

                    # Pull out the system properties that are patient specific and their errors
                    for system, targets in validation_result.get_targets().items():
                        if system not in collected_errors:
                            collected_errors[system] = dict()

                        sys_results = collected_errors[system]
                        for tgt in targets:
                            if tgt.is_patient_specific():
                                if tgt.get_header() not in sys_results:
                                    sys_results[tgt.get_header()] = PropertyError()
                                sys_results[tgt.get_header()].errors.append(tgt.get_error_value())
                #else:
                #    _pulse_logger.info("Not in query: " + _key_patient_values(patient_values))

        return patient_matches, collected_errors


if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

    lineSep = "-------------------------------------------------------------------"
    results = PatientVariabilityResults(dir=Path("./test_results/patient_variability/validation/"))

    # List all the ranges of our result set for males
    ages_yr = results.get_field_values(Field.Age_yr, eSex.Male)
    print(f"Male Age_yr Range:\n\tMin: {ages_yr[0]}\n\tMax: {ages_yr[-1]}")
    heights_cm = results.get_field_values(Field.Height_cm, eSex.Male)
    print(f"Male Height_cm Range:\n\tMin: {heights_cm[0]}\n\tMax: {heights_cm[-1]}")
    bmis = results.get_field_values(Field.BodyMassIndex, eSex.Male)
    print(f"Male BMI Range:\n\tMin: {bmis[0]}\n\tMax: {bmis[-1]}")
    heart_rates_bpm = results.get_field_values(Field.HeartRateBaseline_bpm, eSex.Male)
    print(f"Male HeartRate_bpm Range:\n\tMin: {heart_rates_bpm[0]}\n\tMax: {heart_rates_bpm[-1]}")
    mean_arterial_pressures_mmHg = results.get_field_values(Field.MeanArterialPressureBaseline_mmHg, eSex.Male)
    print(f"Male MeanArterialPressure_mmHg Range:\n\tMin: {mean_arterial_pressures_mmHg[0]}\n\tMax: {mean_arterial_pressures_mmHg[-1]}")
    pulse_pressures_mmHg = results.get_field_values(Field.PulsePressureBaseline_mmHg, eSex.Male)
    print(f"Male PulsePressure_mmHg Range:\n\tMin: {pulse_pressures_mmHg[0]}\n\tMax: {pulse_pressures_mmHg[-1]}")

    # Example: Use conditional filtering (AND only)
    # male AND age < 45 yr AND height >= 165 cm AND height <= 185 cm AND pulse pressure = 40.5 mmHg
    conditional = Conditional() # Note: Default conditional type is AND
    conditional.sex(eSex.Male)
    conditional.add_condition(Field.Age_yr, '<', 45)
    conditional.add_condition(Field.Height_cm, '>=', 165)
    conditional.add_condition(Field.Height_cm, '<=', 185)
    conditional.add_condition(Field.PulsePressureBaseline_mmHg, '==', 40.5)
    matches_conditionalA, errors_conditionalA = results.conditional_filter([conditional])
    print("\n\n\nMale AND age < 45 yr AND height >= 165 cm AND height <= 185 cm AND pulse pressure = 40.5 mmHg:\n" + lineSep)
    for p in matches_conditionalA:
        print(p)

    # Example: Use conditional filtering (includes OR)
    # female AND BMI == 16 AND (HR < 65 bpm OR HR > 85 bpm)
    conditional = Conditional()
    conditional.sex(eSex.Female)
    conditional.add_condition(Field.BodyMassIndex, '==', 16)
    conditional1 = Conditional('OR')
    conditional1.add_condition(Field.HeartRateBaseline_bpm, '<', 65)
    conditional1.add_condition(Field.HeartRateBaseline_bpm, '>', 85)
    conditional.add_conditional(conditional1)
    matches_conditionalB, errors_conditionalB = results.conditional_filter([conditional])
    print("\n\n\nFemale AND BMI == 16 AND (HR < 65 bpm OR HR > 85 bpm):\n" + lineSep)
    for p in matches_conditionalB:
        print(p)
