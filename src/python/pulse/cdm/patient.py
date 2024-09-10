# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from enum import Enum
from pulse.cdm.scalars import SEScalar, SEScalar0To1, SEScalarArea, SEScalarFrequency, SEScalarLength, \
                              SEScalarMass, SEScalarMassPerTime, SEScalarMassPerVolume, SEScalarPower, \
                              SEScalarPressure, SEScalarTime, SEScalarVolume
from pulse.cdm.engine import SEConditionManager

class eSex(Enum):
    Male = 0
    Female = 1

class SEPatientConfiguration():
    __slots__ = ["_patient", "_patient_file", "_condition_manager", "_data_root_dir"]

    def __init__(self):
        self.clear()

    def clear(self):
        self._patient = None
        self._patient_file = None
        self._condition_manager = None
        self._data_root_dir = "./"

    def is_valid(self):
        if not self.has_patient() and not self.has_patient_file():
            return False;
        return True

    def set_data_root_dir(self, dir: str):
        self._data_root_dir = dir
    def get_data_root_dir(self):
        return self._data_root_dir

    def has_patient(self):
        return self._patient is not None
    def get_patient(self):
        if self._patient is None:
            self._patient = SEPatient()
        return self._patient
    def set_patient(self, patient):
        if not isinstance(SEPatient, patient):
            raise Exception("Provided argument must be a SEPatient")
        self._patient = patient

    def has_patient_file(self):
        return self._patient_file is not None
    def get_patient_file(self):
        return self._patient_file
    def set_patient_file(self, file: str):
        self._patient_file = file

    def has_conditions(self):
        return False if self._condition_manager is None else not self._condition_manager.is_empty()
    def get_conditions(self):
        if self._condition_manager is None:
            self._condition_manager = SEConditionManager()
        return self._condition_manager
    def invalidate_conditions(self):
        self._condition_manager = None

class SEPatient():
    __slots__ = ["_name", "_sex", "_age", "_weight", "_height", "_body_density",
                 "_body_fat_fraction", "_body_mass_index", "_ideal_body_weight",
                 "_lean_body_mass", "_alveoli_surface_area", "_right_lung_ratio",
                 "_skin_surface_area", "_basal_metabolic_rate", "_blood_volume_baseline",
                 "_diastolic_arterial_pressure_baseline", "_heart_rate_baseline",
                 "_mean_arterial_pressure_baseline", "_pulse_pressure_baseline",
                 "_respiration_rate_baseline", "_systolic_arterial_pressure_baseline",
                 "_tidal_volume_baseline", "_heart_rate_maximum",  "_heart_rate_minimum",
                 "_expiratory_reserve_volume",  "_functional_residual_capacity",
                 "_inspiratory_capacity",  "_inspiratory_reserve_volume",
                 "_residual_volume",  "_total_lung_capacity", "_vital_capacity"]

    def __init__(self):
        self._name = None
        self._sex = eSex.Male
        self._age = None
        self._weight = None
        self._height = None
        self._body_density = None
        self._body_fat_fraction = None
        self._body_mass_index = None
        self._lean_body_mass = None
        self._ideal_body_weight = None

        self._alveoli_surface_area = None
        self._right_lung_ratio = None
        self._skin_surface_area = None

        self._basal_metabolic_rate = None
        self._blood_volume_baseline = None
        self._diastolic_arterial_pressure_baseline = None
        self._heart_rate_baseline = None
        self._mean_arterial_pressure_baseline = None
        self._pulse_pressure_baseline = None
        self._respiration_rate_baseline = None
        self._systolic_arterial_pressure_baseline = None
        self._tidal_volume_baseline = None

        self._heart_rate_maximum = None
        self._heart_rate_minimum = None
        self._expiratory_reserve_volume = None
        self._functional_residual_capacity = None
        self._inspiratory_capacity = None
        self._inspiratory_reserve_volume = None
        self._residual_volume = None
        self._total_lung_capacity = None
        self._vital_capacity = None

    def clear(self):
        self._name = None
        self._sex = eSex.Male
        if self._age is not None: self._age.invalidate()
        if self._weight is not None: self._weight.invalidate()
        if self._height is not None: self._height.invalidate()
        if self._body_density is not None: self._body_density.invalidate()
        if self._body_fat_fraction is not None: self._body_fat_fraction.invalidate()
        if self._body_mass_index is not None: self._body_mass_index.invalidate()
        if self._lean_body_mass is not None: self._lean_body_mass.invalidate()
        if self._ideal_body_weight is not None: self._ideal_body_weight.invalidate()

        if self._alveoli_surface_area is not None: self._alveoli_surface_area.invalidate()
        if self._right_lung_ratio is not None: self._right_lung_ratio.invalidate()
        if self._skin_surface_area is not None: self._skin_surface_area.invalidate()

        if self._basal_metabolic_rate is not None: self._basal_metabolic_rate.invalidate()
        if self._blood_volume_baseline is not None: self._blood_volume_baseline.invalidate()
        if self._diastolic_arterial_pressure_baseline is not None: self._diastolic_arterial_pressure_baseline.invalidate()
        if self._heart_rate_baseline is not None: self._heart_rate_baseline.invalidate()
        if self._mean_arterial_pressure_baseline is not None: self._mean_arterial_pressure_baseline.invalidate()
        if self._pulse_pressure_baseline is not None: self._pulse_pressure_baseline.invalidate()
        if self._respiration_rate_baseline is not None: self._respiration_rate_baseline.invalidate()
        if self._systolic_arterial_pressure_baseline is not None: self._systolic_arterial_pressure_baseline.invalidate()
        if self._tidal_volume_baseline is not None: self._tidal_volume_baseline.invalidate()

        if self._heart_rate_maximum is not None: self._heart_rate_maximum.invalidate()
        if self._heart_rate_minimum is not None: self._heart_rate_minimum.invalidate()
        if self._expiratory_reserve_volume is not None: self._expiratory_reserve_volume.invalidate()
        if self._functional_residual_capacity is not None: self._functional_residual_capacity.invalidate()
        if self._inspiratory_capacity is not None: self._inspiratory_capacity.invalidate()
        if self._inspiratory_reserve_volume is not None: self._inspiratory_reserve_volume.invalidate()
        if self._residual_volume is not None: self._residual_volume.invalidate()
        if self._total_lung_capacity is not None: self._total_lung_capacity.invalidate()
        if self._vital_capacity is not None: self._vital_capacity.invalidate()

    def copy(self, src):
        if not isinstance(src, SEPatient):
            raise Exception("Provided argument must be a SEPatient")
        self.clear()
        if src.has_name(): self._name = src._name
        self._sex = src._sex
        if src.has_age(): self.get_age().set(src._age)
        if src.has_weight(): self.get_weight().set(src._weight)
        if src.has_height(): self.get_height().set(src._height)
        if src.has_body_density(): self.get_body_density().set(src._body_density)
        if src.has_body_fat_fraction(): self.get_body_fat_fraction().set(src._body_fat_fraction)
        if src.has_body_mass_index(): self.get_body_mass_index().set(src._body_mass_index)
        if src.has_lean_body_mass(): self.get_lean_body_mass().set(src._lean_body_mass)
        if src.has_ideal_body_weight(): self.get_ideal_body_weight().set(src._ideal_body_weight)

        if src.has_alveoli_surface_area(): self.get_alveoli_surface_area().set(src._alveoli_surface_area)
        if src.has_right_lung_ratio(): self.get_right_lung_ratio().set(src._right_lung_ratio)
        if src.has_skin_surface_area(): self.get_skin_surface_area().set(src._skin_surface_area)

        if src.has_basal_metabolic_rate(): self.get_basal_metabolic_rate().set(src._basal_metabolic_rate)
        if src.has_blood_volume_baseline(): self.get_blood_volume_baseline().set(src._blood_volume_baseline)
        if src.has_diastolic_arterial_pressure_baseline(): self.get_diastolic_arterial_pressure_baseline().set(src._diastolic_arterial_pressure_baseline)
        if src.has_heart_rate_baseline(): self.get_heart_rate_baseline().set(src._heart_rate_baseline)
        if src.has_mean_arterial_pressure_baseline(): self.get_mean_arterial_pressure_baseline().set(src._mean_arterial_pressure_baseline)
        if src.has_pulse_pressure_baseline(): self.get_pulse_pressure_baseline().set(src._pulse_pressure_baseline)
        if src.has_respiration_rate_baseline(): self.get_respiration_rate_baseline().set(src._respiration_rate_baseline)
        if src.has_systolic_arterial_pressure_baseline(): self.get_systolic_arterial_pressure_baseline().set(src._systolic_arterial_pressure_baseline)
        if src.has_tidal_volume_baseline(): self.get_tidal_volume_baseline().set(src._tidal_volume_baseline)

        if src.has_heart_rate_maximum(): self.get_heart_rate_maximum().set(src._heart_rate_maximum)
        if src.has_heart_rate_minimum(): self.get_heart_rate_minimum().set(src._heart_rate_minimum)
        if src.has_expiratory_reserve_volume(): self.get_expiratory_reserve_volume().set(src._expiratory_reserve_volume)
        if src.has_functional_residual_capacity(): self.get_functional_residual_capacity().set(src._functional_residual_capacity)
        if src.has_inspiratory_capacity(): self.get_inspiratory_capacity().set(src._inspiratory_capacity)
        if src.has_inspiratory_reserve_volume(): self.get_inspiratory_reserve_volume().set(src._inspiratory_reserve_volume)
        if src.has_residual_volume(): self.get_residual_volume().set(src._residual_volume)
        if src.has_total_lung_capacity(): self.get_total_lung_capacity().set(src._total_lung_capacity)
        if src.has_vital_capacity(): self.get_vital_capacity().set(src._vital_capacity)

    def get_name(self):
        return self._name
    def set_name(self, name: str):
        self._name = name
    def has_name(self):
        return self._name is not None
    def invalidate_name(self):
        self._name = None

    def get_sex(self):
        return self._sex
    def set_sex(self, sex: eSex):
        self._sex = sex

    def has_age(self):
        return False if self._age is None else self._age.is_valid()
    def get_age(self):
        if self._age is None:
            self._age = SEScalarTime()
        return self._age

    def has_weight(self):
        return False if self._weight is None else self._weight.is_valid()
    def get_weight(self):
        if self._weight is None:
            self._weight = SEScalarMass()
        return self._weight

    def has_height(self):
        return False if self._height is None else self._height.is_valid()
    def get_height(self):
        if self._height is None:
            self._height = SEScalarLength()
        return self._height

    def has_body_density(self):
        return False if self._body_density is None else self._body_density.is_valid()
    def get_body_density(self):
        if self._body_density is None:
            self._body_density = SEScalarMassPerVolume()
        return self._body_density

    def has_body_fat_fraction(self):
        return False if self._body_fat_fraction is None else self._body_fat_fraction.is_valid()
    def get_body_fat_fraction(self):
        if self._body_fat_fraction is None:
            self._body_fat_fraction = SEScalar0To1()
        return self._body_fat_fraction

    def has_body_mass_index(self):
        return False if self._body_mass_index is None else self._body_mass_index.is_valid()
    def get_body_mass_index(self):
        if self._body_mass_index is None:
            self._body_mass_index = SEScalar()
        return self._body_mass_index

    def has_ideal_body_weight(self):
        return False if self._ideal_body_weight is None else self._ideal_body_weight.is_valid()
    def get_ideal_body_weight(self):
        if self._ideal_body_weight is None:
            self._ideal_body_weight = SEScalarMass()
        return self._ideal_body_weight

    def has_lean_body_mass(self):
        return False if self._lean_body_mass is None else self._lean_body_mass.is_valid()
    def get_lean_body_mass(self):
        if self._lean_body_mass is None:
            self._lean_body_mass = SEScalarMass()
        return self._lean_body_mass

    def has_alveoli_surface_area(self):
        return False if self._alveoli_surface_area is None else self._alveoli_surface_area.is_valid()
    def get_alveoli_surface_area(self):
        if self._alveoli_surface_area is None:
            self._alveoli_surface_area = SEScalarArea()
        return self._alveoli_surface_area

    def has_right_lung_ratio(self):
        return False if self._right_lung_ratio is None else self._right_lung_ratio.is_valid()
    def get_right_lung_ratio(self):
        if self._right_lung_ratio is None:
            self._right_lung_ratio = SEScalar0To1()
        return self._right_lung_ratio

    def has_skin_surface_area(self):
        return False if self._skin_surface_area is None else self._skin_surface_area.is_valid()
    def get_skin_surface_area(self):
        if self._skin_surface_area is None:
            self._skin_surface_area = SEScalarArea()
        return self._skin_surface_area

    def has_basal_metabolic_rate(self):
        return False if self._basal_metabolic_rate is None else self._basal_metabolic_rate.is_valid()
    def get_basal_metabolic_rate(self):
        if self._basal_metabolic_rate is None:
            self._basal_metabolic_rate = SEScalarPower()
        return self._basal_metabolic_rate

    def has_blood_volume_baseline(self):
        return False if self._blood_volume_baseline is None else self._blood_volume_baseline.is_valid()
    def get_blood_volume_baseline(self):
        if self._blood_volume_baseline is None:
            self._blood_volume_baseline = SEScalarVolume()
        return self._blood_volume_baseline

    def has_diastolic_arterial_pressure_baseline(self):
        return False if self._diastolic_arterial_pressure_baseline is None else self._diastolic_arterial_pressure_baseline.is_valid()
    def get_diastolic_arterial_pressure_baseline(self):
        if self._diastolic_arterial_pressure_baseline is None:
            self._diastolic_arterial_pressure_baseline = SEScalarPressure()
        return self._diastolic_arterial_pressure_baseline

    def has_heart_rate_baseline(self):
        return False if self._heart_rate_baseline is None else self._heart_rate_baseline.is_valid()
    def get_heart_rate_baseline(self):
        if self._heart_rate_baseline is None:
            self._heart_rate_baseline = SEScalarFrequency()
        return self._heart_rate_baseline

    def has_mean_arterial_pressure_baseline(self):
        return False if self._mean_arterial_pressure_baseline is None else self._mean_arterial_pressure_baseline.is_valid()
    def get_mean_arterial_pressure_baseline(self):
        if self._mean_arterial_pressure_baseline is None:
            self._mean_arterial_pressure_baseline = SEScalarPressure()
        return self._mean_arterial_pressure_baseline

    def has_pulse_pressure_baseline(self):
        return False if self._pulse_pressure_baseline is None else self._pulse_pressure_baseline.is_valid()
    def get_pulse_pressure_baseline(self):
        if self._pulse_pressure_baseline is None:
            self._pulse_pressure_baseline = SEScalarPressure()
        return self._pulse_pressure_baseline

    def has_respiration_rate_baseline(self):
        return False if self._respiration_rate_baseline is None else self._respiration_rate_baseline.is_valid()
    def get_respiration_rate_baseline(self):
        if self._respiration_rate_baseline is None:
            self._respiration_rate_baseline = SEScalarFrequency()
        return self._respiration_rate_baseline

    def has_systolic_arterial_pressure_baseline(self):
        return False if self._systolic_arterial_pressure_baseline is None else self._systolic_arterial_pressure_baseline.is_valid()
    def get_systolic_arterial_pressure_baseline(self):
        if self._systolic_arterial_pressure_baseline is None:
            self._systolic_arterial_pressure_baseline = SEScalarPressure()
        return self._systolic_arterial_pressure_baseline

    def has_tidal_volume_baseline(self):
        return False if self._tidal_volume_baseline is None else self._tidal_volume_baseline.is_valid()
    def get_tidal_volume_baseline(self):
        if self._tidal_volume_baseline is None:
            self._tidal_volume_baseline = SEScalarVolume()
        return self._tidal_volume_baseline

    def has_heart_rate_maximum(self):
        return False if self._heart_rate_maximum is None else self._heart_rate_maximum.is_valid()
    def get_heart_rate_maximum(self):
        if self._heart_rate_maximum is None:
            self._heart_rate_maximum = SEScalarFrequency()
        return self._heart_rate_maximum

    def has_heart_rate_minimum(self):
        return False if self._heart_rate_minimum is None else self._heart_rate_minimum.is_valid()
    def get_heart_rate_minimum(self):
        if self._heart_rate_minimum is None:
            self._heart_rate_minimum = SEScalarFrequency()
        return self._heart_rate_minimum

    def has_expiratory_reserve_volume(self):
        return False if self._expiratory_reserve_volume is None else self._expiratory_reserve_volume.is_valid()
    def get_expiratory_reserve_volume(self):
        if self._expiratory_reserve_volume is None:
            self._expiratory_reserve_volume = SEScalarVolume()
        return self._expiratory_reserve_volume

    def has_functional_residual_capacity(self):
        return False if self._functional_residual_capacity is None else self._functional_residual_capacity.is_valid()
    def get_functional_residual_capacity(self):
        if self._functional_residual_capacity is None:
            self._functional_residual_capacity = SEScalarVolume()
        return self._functional_residual_capacity

    def has_inspiratory_capacity(self):
        return False if self._inspiratory_capacity is None else self._inspiratory_capacity.is_valid()
    def get_inspiratory_capacity(self):
        if self._inspiratory_capacity is None:
            self._inspiratory_capacity = SEScalarVolume()
        return self._inspiratory_capacity

    def has_inspiratory_reserve_volume(self):
        return False if self._inspiratory_reserve_volume is None else self._inspiratory_reserve_volume.is_valid()
    def get_inspiratory_reserve_volume(self):
        if self._inspiratory_reserve_volume is None:
            self._inspiratory_reserve_volume = SEScalarVolume()
        return self._inspiratory_reserve_volume

    def has_residual_volume(self):
        return False if self._residual_volume is None else self._residual_volume.is_valid()
    def get_residual_volume(self):
        if self._residual_volume is None:
            self._residual_volume = SEScalarVolume()
        return self._residual_volume

    def has_total_lung_capacity(self):
        return False if self._total_lung_capacity is None else self._total_lung_capacity.is_valid()
    def get_total_lung_capacity(self):
        if self._total_lung_capacity is None:
            self._total_lung_capacity = SEScalarVolume()
        return self._total_lung_capacity

    def has_vital_capacity(self):
        return False if self._vital_capacity is None else self._vital_capacity.is_valid()
    def get_vital_capacity(self):
        if self._vital_capacity is None:
            self._vital_capacity = SEScalarVolume()
        return self._vital_capacity

class SENutrition():
    __slots__ = ["_carbohydrate", "_carbohydrate_digestion_rate",
                 "_fat", "_fat_digestion_rate",
                 "_protein", "_protein_digestion_rate",
                 "_calcium", "_sodium", "_water",]

    def __init__(self):
        self._carbohydrate = None
        self._carbohydrate_digestion_rate = None
        self._fat = None
        self._fat_digestion_rate = None
        self._protein = None
        self._protein_digestion_rate = None
        self._calcium = None
        self._sodium = None
        self._water = None

    def clear(self):
        if self._carbohydrate is not None: self._carbohydrate.invalidate()
        if self._carbohydrate_digestion_rate is not None: self._carbohydrate_digestion_rate.invalidate()
        if self._fat is not None: self._fat.invalidate()
        if self._fat_digestion_rate is not None: self._fat_digestion_rate.invalidate()
        if self._protein is not None: self._protein.invalidate()
        if self._protein_digestion_rate is not None: self._protein_digestion_rate.invalidate()
        if self._calcium is not None: self._calcium.invalidate()
        if self._sodium is not None: self._sodium.invalidate()
        if self._water is not None: self._water.invalidate()

    def copy(self, src):
        if not isinstance(SENutrition, src):
            raise Exception("Provided argument must be a SENutrition")
        self.clear()
        if src.has_carbohydrate(): self.get_carbohydrate().set(src._carbohydrate)
        if src.has_carbohydrate_digestion_rate(): self.get_carbohydrate_digestion_rate().set(src._carbohydrate_digestion_rate)
        if src.has_fat(): self.get_fat().set(src._fat)
        if src.has_fat_digestion_rate(): self.get_fat_digestion_rate().set(src._fat_digestion_rate)
        if src.has_protein(): self.get_protein().set(src._protein)
        if src.has_protein_digestion_rate(): self.get_protein_digestion_rate().set(src._protein_digestion_rate)
        if src.has_calcium(): self.get_calcium().set(src._calcium)
        if src.has_sodium(): self.get_sodium().set(src._sodium)
        if src.has_water(): self.get_water().set(src._water)

    def has_carbohydrate(self):
        return False if self._carbohydrate is None else self._carbohydrate.is_valid()
    def get_carbohydrate(self):
        if self._carbohydrate is None:
            self._carbohydrate = SEScalarMass()
        return self._carbohydrate

    def has_carbohydrate_digestion_rate(self):
        return False if self._carbohydrate_digestion_rate is None else self._carbohydrate_digestion_rate.is_valid()
    def get_carbohydrate_digestion_rate(self):
        if self._carbohydrate_digestion_rate is None:
            self._carbohydrate_digestion_rate = SEScalarMassPerTime()
        return self._carbohydrate_digestion_rate

    def has_fat(self):
        return False if self._fat is None else self._fat.is_valid()
    def get_fat(self):
        if self._fat is None:
            self._fat = SEScalarMass()
        return self._fat

    def has_fat_digestion_rate(self):
        return False if self._fat_digestion_rate is None else self._fat_digestion_rate.is_valid()
    def get_fat_digestion_rate(self):
        if self._fat_digestion_rate is None:
            self._fat_digestion_rate = SEScalarMassPerTime()
        return self._fat_digestion_rate

    def has_protein(self):
        return False if self._protein is None else self._protein.is_valid()
    def get_protein(self):
        if self._protein is None:
            self._protein = SEScalarMass()
        return self._protein

    def has_protein_digestion_rate(self):
        return False if self._protein_digestion_rate is None else self._protein_digestion_rate.is_valid()
    def get_protein_digestion_rate(self):
        if self._protein_digestion_rate is None:
            self._protein_digestion_rate = SEScalarMassPerTime()
        return self._protein_digestion_rate

    def has_calcium(self):
        return False if self._calcium is None else self._calcium.is_valid()
    def get_calcium(self):
        if self._calcium is None:
            self._calcium = SEScalarMass()
        return self._calcium

    def has_sodium(self):
        return False if self._sodium is None else self._sodium.is_valid()
    def get_sodium(self):
        if self._sodium is None:
            self._sodium = SEScalarMass()
        return self._sodium

    def has_water(self):
        return False if self._water is None else self._water.is_valid()
    def get_water(self):
        if self._water is None:
            self._water = SEScalarVolume()
        return self._water
