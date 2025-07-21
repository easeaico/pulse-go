# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.enums import eSerializationFormat
from google.protobuf import json_format

from pulse.cdm.environment import eSurroundingType, SEEnvironmentalConditions, \
                                  SEAppliedTemperature, SEActiveConditioning
from pulse.cdm.bind.Environment_pb2 import EnvironmentalConditionsData, \
                                           AppliedTemperatureData, \
                                           ActiveConditioningData

from pulse.cdm.bind.Substance_pb2 import SubstanceFractionData, SubstanceConcentrationData

from pulse.cdm.io.scalars import *

def serialize_environmental_conditions_to_string(src: SEEnvironmentalConditions, fmt: eSerializationFormat):
    dst = EnvironmentalConditionsData()
    serialize_environmental_conditions_to_bind(src, dst)
    return json_format.MessageToJson(dst, True, True)

def serialize_environmental_conditions_to_file(src: SEEnvironmentalConditions, filename: str):
    string = serialize_environmental_conditions_to_string(src, eSerializationFormat.JSON)
    file = open(filename, "w")
    n = file.write(string)
    file.close()

def serialize_environmental_conditions_from_string(string: str, dst: SEEnvironmentalConditions, fmt: eSerializationFormat):
    src = EnvironmentalConditionsData()
    json_format.Parse(string, src)
    serialize_environmental_conditions_from_bind(src,dst)

def serialize_environmental_conditions_from_file(filename: str, dst: SEEnvironmentalConditions):
    with open(filename) as f:
        string = f.read()
    serialize_environmental_conditions_from_string(string, dst, eSerializationFormat.JSON)

def serialize_environmental_conditions_to_bind(src: SEEnvironmentalConditions, dst: EnvironmentalConditionsData):
    dst.SurroundingType = src.get_surrounding_type().value
    if src.has_air_density():
        serialize_scalar_mass_per_volume_to_bind(src.get_air_density(), dst.AirDensity)
    if src.has_air_velocity():
        serialize_scalar_length_per_time_to_bind(src.get_air_velocity(), dst.AirVelocity)
    if src.has_ambient_temperature():
        serialize_scalar_temperature_to_bind(src.get_ambient_temperature(), dst.AmbientTemperature)
    if src.has_atmospheric_pressure():
        serialize_scalar_pressure_to_bind(src.get_atmospheric_pressure(), dst.AtmosphericPressure)
    if src.has_clothing_resistance():
        serialize_scalar_heat_resistance_area_to_bind(src.get_clothing_resistance(), dst.ClothingResistance)
    if src.has_emissivity():
        serialize_scalar_0to1_to_bind(src.get_emissivity(), dst.Emissivity)
    if src.has_mean_radiant_temperature():
        serialize_scalar_temperature_to_bind(src.get_mean_radiant_temperature(), dst.MeanRadiantTemperature)
    if src.has_mechanical_dead_space():
        serialize_scalar_volume_to_bind(src.get_mechanical_dead_space(), dst.MechanicalDeadSpace)
    if src.has_relative_humidity():
        serialize_scalar_0to1_to_bind(src.get_relative_humidity(), dst.RelativeHumidity)
    if src.has_respiration_ambient_temperature():
        serialize_scalar_temperature_to_bind(src.get_respiration_ambient_temperature(), dst.RespirationAmbientTemperature)
    for aGas in src.get_ambient_gasses():
        sf = SubstanceFractionData()
        sf.Name = aGas.get_substance()
        serialize_scalar_0to1_to_bind(aGas.get_fraction_amount(), sf.Amount)
        dst.AmbientGas.append(sf)
    for aAerosol in src.get_ambient_aerosols():
        sc = SubstanceConcentrationData()
        sc.Name = aAerosol.get_substance()
        serialize_scalar_mass_per_volume_to_bind(aAerosol.get_concentration(), sc.Concentration)
        dst.AmbientAerosol.append(sc)

def serialize_environmental_conditions_from_bind(src: EnvironmentalConditionsData, dst: SEEnvironmentalConditions):
    dst.set_surrounding_type(eSurroundingType(src.SurroundingType))
    if src.HasField("AirDensity"):
        serialize_scalar_mass_per_volume_from_bind(src.AirDensity, dst.get_air_density())
    if src.HasField("AirVelocity"):
        serialize_scalar_length_per_time_from_bind(src.AirVelocity, dst.get_air_velocity())
    if src.HasField("AmbientTemperature"):
        serialize_scalar_temperature_from_bind(src.AmbientTemperature, dst.get_ambient_temperature())
    if src.HasField("AtmosphericPressure"):
        serialize_scalar_pressure_from_bind(src.AtmosphericPressure, dst.get_atmospheric_pressure())
    if src.HasField("ClothingResistance"):
        serialize_scalar_heat_resistance_area_from_bind(src.ClothingResistance, dst.get_clothing_resistance())
    if src.HasField("Emissivity"):
        serialize_scalar_0to1_from_bind(src.Emissivity, dst.get_emissivity())
    if src.HasField("MeanRadiantTemperature"):
        serialize_scalar_temperature_from_bind(src.MeanRadiantTemperature, dst.get_mean_radiant_temperature())
    if src.HasField("MechanicalDeadSpace"):
        serialize_scalar_volume_from_bind(src.MechanicalDeadSpace, dst.get_mechanical_dead_space())
    if src.HasField("RelativeHumidity"):
        serialize_scalar_0to1_from_bind(src.RelativeHumidity, dst.get_relative_humidity())
    if src.HasField("RespirationAmbientTemperature"):
        serialize_scalar_temperature_from_bind(src.RespirationAmbientTemperature, dst.get_respiration_ambient_temperature())
    for aGas in src.AmbientGas:
        serialize_scalar_0to1_from_bind(aGas.Amount, dst.get_ambient_gas(aGas.Name).get_fraction_amount())
    for aAerosol in src.AmbientAerosol:
        serialize_scalar_mass_per_volume_from_bind(aAerosol.Concentration, dst.get_ambient_aerosol(aAerosol.Name).get_concentration())

def serialize_active_conditioning_to_bind(src: SEActiveConditioning, dst: ActiveConditioningData):
    if src.has_power():
        serialize_scalar_power_to_bind(src.get_power(), dst.Power)
    if src.has_surface_area():
        serialize_scalar_area_to_bind(src.get_surface_area(), dst.SurfaceArea)
    if src.has_surface_area_fraction():
        serialize_scalar_0to1_to_bind(src.get_surface_area_fraction(), dst.SurfaceAreaFraction)

def serialize_active_conditioning_from_bind(src: ActiveConditioningData, dst: SEActiveConditioning):
    raise Exception("serialize_active_conditioning_from_bind not implemented")

def serialize_applied_temperature_to_bind(src: SEAppliedTemperature, dst: AppliedTemperatureData):
    dst.State = src.get_state().value
    if src.has_temperature():
        serialize_scalar_temperature_to_bind(src.get_temperature(), dst.Temperature)
    if src.has_surface_area():
        serialize_scalar_area_to_bind(src.get_surface_area(), dst.SurfaceArea)
    if src.has_surface_area_fraction():
        serialize_scalar_0to1_to_bind(src.get_surface_area_fraction(), dst.SurfaceAreaFraction)

def serialize_applied_temperature_from_bind(src: AppliedTemperatureData, dst: SEAppliedTemperature):
    raise Exception("serialize_applied_temperature_from_bind not implemented")