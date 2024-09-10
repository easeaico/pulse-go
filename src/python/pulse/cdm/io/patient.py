# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.engine import eSerializationFormat
from google.protobuf import json_format, text_format

from pulse.cdm.patient import SEPatient, eSex, SENutrition
from pulse.cdm.bind.Patient_pb2 import PatientData
from pulse.cdm.bind.PatientNutrition_pb2 import NutritionData

from pulse.cdm.io.scalars import *

def serialize_patient_to_string(src: SEPatient, fmt: eSerializationFormat):
    dst = PatientData()
    serialize_patient_to_bind(src, dst)
    return json_format.MessageToJson(dst, True, True)

def serialize_patient_to_file(src: SEPatient, filename: str):
    string = serialize_patient_to_string(src, eSerializationFormat.JSON)
    file = open(filename, "w")
    n = file.write(string)
    file.close()

def serialize_patient_from_string(string: str, dst: SEPatient, fmt: eSerializationFormat):
    src = PatientData()
    if fmt == eSerializationFormat.JSON:
        json_format.Parse(string, src)
    elif fmt == eSerializationFormat.TEXT:
        text_format.Parse(string, src)
    serialize_patient_from_bind(src,dst)

def serialize_patient_from_file(filename: str, dst: SEPatient):
    with open(filename) as f:
        string = f.read()
    serialize_patient_from_string(string, dst, eSerializationFormat.JSON)

def serialize_patient_to_bind(src: SEPatient, dst: PatientData):
    if src.has_name():
        dst.Name = src.get_name()
    dst.Sex = src.get_sex().value
    if src.has_age():
        serialize_scalar_time_to_bind(src.get_age(), dst.Age)
    if src.has_weight():
        serialize_scalar_mass_to_bind(src.get_weight(), dst.Weight)
    if src.has_height():
        serialize_scalar_length_to_bind(src.get_height(), dst.Height)
    if src.has_body_density():
        serialize_scalar_mass_per_volume_to_bind(src.get_body_density(), dst.BodyDensity)
    if src.has_body_fat_fraction():
        serialize_scalar_0to1_to_bind(src.get_body_fat_fraction(), dst.BodyFatFraction)
    if src.has_body_mass_index():
        serialize_scalar_to_bind(src.get_body_mass_index(), dst.BodyMassIndex)
    if src.has_ideal_body_weight():
        serialize_scalar_mass_to_bind(src.get_ideal_body_weight(), dst.IdealBodyWeight)
    if src.has_lean_body_mass():
        serialize_scalar_mass_to_bind(src.get_lean_body_mass(), dst.LeanBodyMass)
    if src.has_alveoli_surface_area():
        serialize_scalar_area_to_bind(src.get_alveoli_surface_area(), dst.AlveoliSurfaceArea)
    if src.has_right_lung_ratio():
        serialize_scalar_0to1_to_bind(src.get_right_lung_ratio(), dst.RightLungRatio)
    if src.has_skin_surface_area():
        serialize_scalar_area_to_bind(src.get_skin_surface_area(), dst.SkinSurfaceArea)
    if src.has_basal_metabolic_rate():
        serialize_scalar_power_to_bind(src.get_basal_metabolic_rate(), dst.BasalMetabolicRate)
    if src.has_blood_volume_baseline():
        serialize_scalar_volume_to_bind(src.get_blood_volume_baseline(), dst.BloodVolumeBaseline)
    if src.has_diastolic_arterial_pressure_baseline():
        serialize_scalar_pressure_to_bind(src.get_diastolic_arterial_pressure_baseline(), dst.DiastolicArterialPressureBaseline)
    if src.has_heart_rate_baseline():
        serialize_scalar_frequency_to_bind(src.get_heart_rate_baseline(), dst.HeartRateBaseline)
    if src.has_mean_arterial_pressure_baseline():
        serialize_scalar_pressure_to_bind(src.get_mean_arterial_pressure_baseline(), dst.MeanArterialPressureBaseline)
    if src.has_pulse_pressure_baseline():
        serialize_scalar_pressure_to_bind(src.get_pulse_pressure_baseline(), dst.PulsePressureBaseline)
    if src.has_respiration_rate_baseline():
        serialize_scalar_frequency_to_bind(src.get_respiration_rate_baseline(), dst.RespirationRateBaseline)
    if src.has_systolic_arterial_pressure_baseline():
        serialize_scalar_pressure_to_bind(src.get_systolic_arterial_pressure_baseline(), dst.SystolicArterialPressureBaseline)
    if src.has_tidal_volume_baseline():
        serialize_scalar_volume_to_bind(src.get_tidal_volume_baseline(), dst.TidalVolumeBaseline)
    if src.has_heart_rate_maximum():
        serialize_scalar_frequency_to_bind(src.get_heart_rate_maximum(), dst.HeartRateMaximum)
    if src.has_heart_rate_minimum():
        serialize_scalar_frequency_to_bind(src.get_heart_rate_minimum(), dst.HeartRateMinimum)
    if src.has_expiratory_reserve_volume():
        serialize_scalar_volume_to_bind(src.get_expiratory_reserve_volume(), dst.ExpiratoryReserveVolume)
    if src.has_functional_residual_capacity():
        serialize_scalar_volume_to_bind(src.get_functional_residual_capacity(), dst.FunctionalResidualCapacity)
    if src.has_inspiratory_capacity():
        serialize_scalar_volume_to_bind(src.get_inspiratory_capacity(), dst.InspiratoryCapacity)
    if src.has_inspiratory_reserve_volume():
        serialize_scalar_volume_to_bind(src.get_inspiratory_reserve_volume(), dst.InspiratoryReserveVolume)
    if src.has_residual_volume():
        serialize_scalar_volume_to_bind(src.get_residual_volume(), dst.ResidualVolume)
    if src.has_total_lung_capacity():
        serialize_scalar_volume_to_bind(src.get_total_lung_capacity(), dst.TotalLungCapacity)
    if src.has_vital_capacity():
        serialize_scalar_volume_to_bind(src.get_vital_capacity(), dst.VitalCapacity)

def serialize_patient_from_bind(src: PatientData, dst: SEPatient):
    dst.clear()
    dst.set_name(src.Name)
    dst.set_sex(eSex(src.Sex))
    if src.HasField("Age"):
        serialize_scalar_time_from_bind(src.Age, dst.get_age())
    if src.HasField("Weight"):
        serialize_scalar_mass_from_bind(src.Weight, dst.get_weight())
    if src.HasField("Height"):
        serialize_scalar_length_from_bind(src.Height, dst.get_height())
    if src.HasField("BodyDensity"):
        serialize_scalar_mass_per_volume_from_bind(src.BodyDensity, dst.get_body_density())
    if src.HasField("BodyFatFraction"):
        serialize_scalar_0to1_from_bind(src.BodyFatFraction, dst.get_body_fat_fraction())
    if src.HasField("BodyMassIndex"):
        serialize_scalar_from_bind(src.BodyMassIndex, dst.get_body_mass_index())
    if src.HasField("IdealBodyWeight"):
        serialize_scalar_mass_from_bind(src.IdealBodyWeight, dst.get_ideal_body_weight())
    if src.HasField("LeanBodyMass"):
        serialize_scalar_mass_from_bind(src.LeanBodyMass, dst.get_lean_body_mass())
    if src.HasField("AlveoliSurfaceArea"):
        serialize_scalar_area_from_bind(src.AlveoliSurfaceArea, dst.get_alveoli_surface_area())
    if src.HasField("RightLungRatio"):
        serialize_scalar_0to1_from_bind(src.RightLungRatio, dst.get_right_lung_ratio())
    if src.HasField("SkinSurfaceArea"):
        serialize_scalar_area_from_bind(src.SkinSurfaceArea, dst.get_skin_surface_area())
    if src.HasField("BasalMetabolicRate"):
        serialize_scalar_power_from_bind(src.BasalMetabolicRate, dst.get_basal_metabolic_rate())
    if src.HasField("BloodVolumeBaseline"):
        serialize_scalar_volume_from_bind(src.BloodVolumeBaseline, dst.get_blood_volume_baseline())
    if src.HasField("DiastolicArterialPressureBaseline"):
        serialize_scalar_pressure_from_bind(src.DiastolicArterialPressureBaseline, dst.get_diastolic_arterial_pressure_baseline())
    if src.HasField("HeartRateBaseline"):
        serialize_scalar_frequency_from_bind(src.HeartRateBaseline, dst.get_heart_rate_baseline())
    if src.HasField("MeanArterialPressureBaseline"):
        serialize_scalar_pressure_from_bind(src.MeanArterialPressureBaseline, dst.get_mean_arterial_pressure_baseline())
    if src.HasField("PulsePressureBaseline"):
        serialize_scalar_pressure_from_bind(src.PulsePressureBaseline, dst.get_pulse_pressure_baseline())
    if src.HasField("RespirationRateBaseline"):
        serialize_scalar_frequency_from_bind(src.RespirationRateBaseline, dst.get_respiration_rate_baseline())
    if src.HasField("SystolicArterialPressureBaseline"):
        serialize_scalar_pressure_from_bind(src.SystolicArterialPressureBaseline, dst.get_systolic_arterial_pressure_baseline())
    if src.HasField("TidalVolumeBaseline"):
        serialize_scalar_volume_from_bind(src.TidalVolumeBaseline, dst.get_tidal_volume_baseline())
    if src.HasField("HeartRateMaximum"):
        serialize_scalar_frequency_from_bind(src.HeartRateMaximum, dst.get_heart_rate_maximum())
    if src.HasField("HeartRateMinimum"):
        serialize_scalar_frequency_from_bind(src.HeartRateMinimum, dst.get_heart_rate_minimum())
    if src.HasField("ExpiratoryReserveVolume"):
        serialize_scalar_volume_from_bind(src.ExpiratoryReserveVolume, dst.get_expiratory_reserve_volume())
    if src.HasField("FunctionalResidualCapacity"):
        serialize_scalar_volume_from_bind(src.FunctionalResidualCapacity, dst.get_functional_residual_capacity())
    if src.HasField("InspiratoryCapacity"):
        serialize_scalar_volume_from_bind(src.InspiratoryCapacity, dst.get_inspiratory_capacity())
    if src.HasField("InspiratoryReserveVolume"):
        serialize_scalar_volume_from_bind(src.InspiratoryReserveVolume, dst.get_inspiratory_reserve_volume())
    if src.HasField("ResidualVolume"):
        serialize_scalar_volume_from_bind(src.ResidualVolume, dst.get_residual_volume())
    if src.HasField("TotalLungCapacity"):
        serialize_scalar_volume_from_bind(src.TotalLungCapacity, dst.get_total_lung_capacity())
    if src.HasField("VitalCapacity"):
        serialize_scalar_volume_from_bind(src.VitalCapacity, dst.get_vital_capacity())

def serialize_nutrition_to_string(src: SENutrition, fmt: eSerializationFormat):
    dst = NutritionData()
    serialize_nutrition_to_bind(src, dst)
    return json_format.MessageToJson(dst, True, True)

def serialize_nutrition_to_file(src: SENutrition, filename: str):
    string = serialize_nutrition_to_string(src, eSerializationFormat.JSON)
    file = open(filename, "w")
    n = file.write(string)
    file.close()

def serialize_nutrition_from_string(string: str, dst: SENutrition, fmt: eSerializationFormat):
    src = NutritionData()
    json_format.Parse(string, src)
    serialize_nutrition_from_bind(src,dst)

def serialize_nutrition_from_file(filename: str, dst: SENutrition):
    with open(filename) as f:
        string = f.read()
    serialize_nutrition_from_string(string, dst, eSerializationFormat.JSON)

def serialize_nutrition_to_bind(src: SENutrition, dst: NutritionData):
    if src.has_carbohydrate():
        serialize_scalar_mass_to_bind(src.get_carbohydrate(), dst.Carbohydrate)
    if src.has_carbohydrate_digestion_rate():
        serialize_scalar_mass_per_time_to_bind(src.get_carbohydrate_digestion_rate(), dst.CarbohydrateDigestionRate)
    if src.has_fat():
        serialize_scalar_mass_to_bind(src.get_fat(), dst.Fat)
    if src.has_fat_digestion_rate():
        serialize_scalar_mass_per_time_to_bind(src.get_fat_digestion_rate(), dst.FatDigestionRate)
    if src.has_protein():
        serialize_scalar_mass_to_bind(src.get_protein(), dst.Protein)
    if src.has_protein_digestion_rate():
        serialize_scalar_mass_per_time_to_bind(src.get_protein_digestion_rate(), dst.ProteinDigestionRate)
    if src.has_calcium():
        serialize_scalar_mass_to_bind(src.get_calcium(), dst.Calcium)
    if src.has_sodium():
        serialize_scalar_mass_to_bind(src.get_sodium(), dst.Sodium)
    if src.has_water():
        serialize_scalar_volume_to_bind(src.get_water(), dst.Water)

def serialize_nutrition_from_bind(src: NutritionData, dst: SENutrition):
    dst.clear()
    if src.HasField("Carbohydrate"):
        serialize_scalar_mass_from_bind(src.Carbohydrate, dst.get_carbohydrate())
    if src.HasField("CarbohydrateDigestionRate"):
        serialize_scalar_mass_per_time_from_bind(src.CarbohydrateDigestionRate, dst.get_carbohydrate_digestion_rate())
    if src.HasField("Fat"):
        serialize_scalar_mass_from_bind(src.Fat, dst.get_fat())
    if src.HasField("FatDigestionRate"):
        serialize_scalar_mass_per_time_from_bind(src.FatDigestionRate, dst.get_fat_digestion_rate())
    if src.HasField("Protein"):
        serialize_scalar_mass_from_bind(src.Protein, dst.get_protein())
    if src.HasField("ProteinDigestionRate"):
        serialize_scalar_mass_per_time_from_bind(src.ProteinDigestionRate, dst.get_protein_digestion_rate())
    if src.HasField("Calcium"):
        serialize_scalar_mass_from_bind(src.Calcium, dst.get_calcium())
    if src.HasField("Sodium"):
        serialize_scalar_mass_from_bind(src.Sodium, dst.get_sodium())
    if src.HasField("Water"):
        serialize_scalar_volume_from_bind(src.Water, dst.get_water())
