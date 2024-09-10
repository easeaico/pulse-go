# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.scalars import SEScalar, SEScalar0To1, SEScalarNegative1To1, \
                              SEScalarArea, AreaUnit, \
                              SEScalarEquivalentWeightPerVolume, EquivalentWeightPerVolumeUnit, \
                              SEScalarForce, ForceUnit, \
                              SEScalarFrequency, FrequencyUnit, \
                              SEScalarHeatCapacitancePerMass, HeatCapacitancePerMassUnit, \
                              SEScalarHeatConductance, HeatConductanceUnit, \
                              SEScalarHeatConductancePerArea, HeatConductancePerAreaUnit, \
                              SEScalarHeatResistanceArea, HeatResistanceAreaUnit, \
                              SEScalarInversePressure, InversePressureUnit, \
                              SEScalarLength, LengthUnit, \
                              SEScalarLengthPerTime, LengthPerTimeUnit, \
                              SEScalarMass, MassUnit, \
                              SEScalarMassPerAmount, MassPerAmountUnit, \
                              SEScalarMassPerAreaTime, MassPerAreaTimeUnit, \
                              SEScalarMassPerTime, MassPerTimeUnit, \
                              SEScalarMassPerVolume, MassPerVolumeUnit, \
                              SEScalarPower, PowerUnit, \
                              SEScalarPressure, PressureUnit, \
                              SEScalarPressurePerVolume, PressurePerVolumeUnit, \
                              SEScalarPressureTimePerVolume, PressureTimePerVolumeUnit, \
                              SEScalarTemperature, TemperatureUnit, \
                              SEScalarUnsigned, \
                              SEScalarTime, TimeUnit, \
                              SEScalarVolume, VolumeUnit, \
                              SEScalarVolumePerPressure, VolumePerPressureUnit, \
                              SEScalarVolumePerTime, VolumePerTimeUnit, \
                              SEScalarVolumePerTimeMass, VolumePerTimeMassUnit, \
                              SEScalarVolumePerTimePressure, VolumePerTimePressureUnit
from pulse.cdm.bind.Properties_pb2 import ScalarData, Scalar0To1Data, ScalarNegative1To1Data, ScalarAreaData, \
                                          ScalarEquivalentWeightPerVolumeData, \
                                          ScalarForceData, ScalarFrequencyData,\
                                          ScalarHeatCapacitancePerMassData, ScalarHeatConductanceData, \
                                          ScalarHeatConductancePerAreaData, ScalarHeatResistanceAreaData, \
                                          ScalarInversePressureData, ScalarLengthData, ScalarLengthPerTimeData, \
                                          ScalarMassData, ScalarMassPerAmountData, ScalarMassPerAreaTimeData, \
                                          ScalarMassPerTimeData, ScalarMassPerVolumeData, ScalarPowerData, \
                                          ScalarPressureTimePerVolumeData, ScalarPressureData,\
                                          ScalarPressurePerVolumeData, ScalarTemperatureData, ScalarTimeData, \
                                          ScalarUnsignedData, ScalarVolumeData, ScalarVolumePerPressureData, \
                                          ScalarVolumePerTimeData, ScalarVolumePerTimeMassData, \
                                          ScalarVolumePerTimePressureData

def serialize_scalar_to_bind(src: SEScalar, dst: ScalarData):
    dst.Value = src.get_value()
def serialize_scalar_from_bind(src: ScalarData, dst: SEScalar):
    dst.set_value(src.Value)

def serialize_scalar_0to1_to_bind(src: SEScalar0To1, dst: Scalar0To1Data):
    dst.Scalar0To1.Value = src.get_value()
def serialize_scalar_0to1_from_bind(src: Scalar0To1Data, dst: SEScalar0To1):
    dst.set_value(src.Scalar0To1.Value)

def serialize_scalar_negative1to1_to_bind(src: SEScalarNegative1To1, dst: ScalarNegative1To1Data):
    dst.ScalarNegative1To1.Value = src.get_value()
def serialize_scalar_negative1to1_from_bind(src: ScalarNegative1To1Data, dst: SEScalarNegative1To1):
    dst.set_value(src.ScalarNegative1To1.Value)

def serialize_scalar_area_to_bind(src: SEScalarArea, dst: ScalarAreaData):
    dst.ScalarArea.Value = src.get_value()
    dst.ScalarArea.Unit = src.get_unit().get_string()
def serialize_scalar_area_from_bind(src: ScalarAreaData, dst: SEScalarArea):
    dst.set_value(src.ScalarArea.Value, AreaUnit.from_string(src.ScalarArea.Unit))

def serialize_scalar_equivalent_weight_per_volume_to_bind(src: SEScalarEquivalentWeightPerVolume, dst: ScalarEquivalentWeightPerVolumeData):
    dst.ScalarEquivalentWeightPerVolume.Value = src.get_value()
    dst.ScalarEquivalentWeightPerVolume.Unit = src.get_unit().get_string()
def serialize_scalar_equivalent_weight_per_volume_from_bind(src: ScalarEquivalentWeightPerVolumeData, dst: SEScalarEquivalentWeightPerVolume):
    dst.set_value(src.ScalarEquivalentWeightPerVolume.Value, EquivalentWeightPerVolumeUnit.from_string(src.ScalarEquivalentWeightPerVolume.Unit))

def serialize_scalar_force_to_bind(src: SEScalarForce, dst: ScalarForceData):
    dst.ScalarForce.Value = src.get_value()
    dst.ScalarForce.Unit = src.get_unit().get_string()
def serialize_scalar_force_from_bind(src: ScalarForceData, dst: SEScalarForce):
    dst.set_value(src.ScalarForce.Value, ForceUnit.from_string(src.ScalarForce.Unit))

def serialize_scalar_frequency_to_bind(src: SEScalarFrequency, dst: ScalarFrequencyData):
    dst.ScalarFrequency.Value = src.get_value()
    dst.ScalarFrequency.Unit = src.get_unit().get_string()
def serialize_scalar_frequency_from_bind(src: ScalarFrequencyData, dst: SEScalarFrequency):
    dst.set_value(src.ScalarFrequency.Value, FrequencyUnit.from_string(src.ScalarFrequency.Unit))

def serialize_scalar_heat_capacitance_per_mass_to_bind(src: SEScalarHeatCapacitancePerMass, dst: ScalarHeatCapacitancePerMassData):
    dst.ScalarHeatCapacitancePerMass.Value = src.get_value()
    dst.ScalarHeatCapacitancePerMass.Unit = src.get_unit().get_string()
def serialize_scalar_heat_capacitance_per_mass_from_bind(src: ScalarHeatCapacitancePerMassData, dst: SEScalarHeatCapacitancePerMass):
    dst.set_value(src.ScalarHeatCapacitancePerMass.Value, HeatCapacitancePerMassUnit.from_string(src.ScalarHeatCapacitancePerMass.Unit))

def serialize_scalar_heat_conductance_to_bind(src: SEScalarHeatConductance, dst: ScalarHeatConductanceData):
    dst.ScalarHeatConductance.Value = src.get_value()
    dst.ScalarHeatConductance.Unit = src.get_unit().get_string()
def serialize_scalar_heat_conductance_from_bind(src: ScalarHeatConductanceData, dst: SEScalarHeatConductance):
    dst.set_value(src.ScalarHeatConductance.Value, HeatConductanceUnit.from_string(src.ScalarHeatConductance.Unit))

def serialize_scalar_heat_conductance_per_area_to_bind(src: SEScalarHeatConductancePerArea, dst: ScalarHeatConductancePerAreaData):
    dst.ScalarHeatConductancePerArea.Value = src.get_value()
    dst.ScalarHeatConductancePerArea.Unit = src.get_unit().get_string()
def serialize_scalar_heat_conductance_per_area__from_bind(src: ScalarHeatConductancePerAreaData, dst: SEScalarHeatConductancePerArea):
    dst.set_value(src.ScalarHeatConductancePerArea.Value, HeatConductancePerAreaUnit.from_string(src.ScalarHeatConductancePerArea.Unit))

def serialize_scalar_heat_resistance_area_to_bind(src: SEScalarHeatResistanceArea, dst: ScalarHeatResistanceAreaData):
    dst.ScalarHeatResistanceArea.Value = src.get_value()
    dst.ScalarHeatResistanceArea.Unit = src.get_unit().get_string()
def serialize_scalar_heat_resistance_area_from_bind(src: ScalarHeatResistanceAreaData, dst: SEScalarHeatResistanceArea):
    dst.set_value(src.ScalarHeatResistanceArea.Value, HeatResistanceAreaUnit.from_string(src.ScalarHeatResistanceArea.Unit))

def serialize_scalar_inverse_pressure_to_bind(src: SEScalarInversePressure, dst: ScalarInversePressureData):
    dst.ScalarInversePressure.Value = src.get_value()
    dst.ScalarInversePressure.Unit = src.get_unit().get_string()
def serialize_scalar_inverse_pressure_from_bind(src: ScalarInversePressureData, dst: SEScalarInversePressure):
    dst.set_value(src.ScalarInversePressure.Value, InversePressureUnit.from_string(src.ScalarInversePressure.Unit))

def serialize_scalar_length_to_bind(src: SEScalarLength, dst: ScalarLengthData):
    dst.ScalarLength.Value = src.get_value()
    dst.ScalarLength.Unit = src.get_unit().get_string()
def serialize_scalar_length_from_bind(src: ScalarLengthData, dst: SEScalarLength):
    dst.set_value(src.ScalarLength.Value, LengthUnit.from_string(src.ScalarLength.Unit))

def serialize_scalar_length_per_time_to_bind(src: SEScalarLengthPerTime, dst: ScalarLengthPerTimeData):
    dst.ScalarLengthPerTime.Value = src.get_value()
    dst.ScalarLengthPerTime.Unit = src.get_unit().get_string()
def serialize_scalar_length_per_time_from_bind(src: ScalarLengthPerTimeData, dst: SEScalarLengthPerTime):
    dst.set_value(src.ScalarLengthPerTime.Value, LengthPerTimeUnit.from_string(src.ScalarLengthPerTime.Unit))

def serialize_scalar_mass_to_bind(src: SEScalarMass, dst: ScalarMassData):
    dst.ScalarMass.Value = src.get_value()
    dst.ScalarMass.Unit = src.get_unit().get_string()
def serialize_scalar_mass_from_bind(src: ScalarMassData, dst: SEScalarMass):
    dst.set_value(src.ScalarMass.Value, MassUnit.from_string(src.ScalarMass.Unit))

def serialize_scalar_mass_per_amount_to_bind(src: SEScalarMassPerAmount, dst: ScalarMassPerAmountData):
    dst.ScalarMassPerAmount.Value = src.get_value()
    dst.ScalarMassPerAmount.Unit = src.get_unit().get_string()
def serialize_scalar_mass_per_amount_from_bind(src: ScalarMassPerAmountData, dst: SEScalarMassPerAmount):
    dst.set_value(src.ScalarMassPerAmount.Value, MassPerAmountUnit.from_string(src.ScalarMassPerAmount.Unit))

def serialize_scalar_mass_per_area_time_to_bind(src: SEScalarMassPerAreaTime, dst: ScalarMassPerAreaTimeData):
    dst.ScalarMassPerAreaTime.Value = src.get_value()
    dst.ScalarMassPerAreaTime.Unit = src.get_unit().get_string()
def serialize_scalar_mass_per_area_time_from_bind(src: ScalarMassPerAreaTimeData, dst: SEScalarMassPerAreaTime):
    dst.set_value(src.ScalarMassPerAreaTime.Value, MassPerAreaTimeUnit.from_string(src.ScalarMassPerAreaTime.Unit))

def serialize_scalar_mass_per_time_to_bind(src: SEScalarMassPerTime, dst: ScalarMassPerTimeData):
    dst.ScalarMassPerTime.Value = src.get_value()
    dst.ScalarMassPerTime.Unit = src.get_unit().get_string()
def serialize_scalar_mass_per_time_from_bind(src: ScalarMassPerTimeData, dst: SEScalarMassPerTime):
    dst.set_value(src.ScalarMassPerTime.Value, MassPerTimeUnit.from_string(src.ScalarMassPerTime.Unit))

def serialize_scalar_mass_per_volume_to_bind(src: SEScalarMassPerVolume, dst: ScalarMassPerVolumeData):
    dst.ScalarMassPerVolume.Value = src.get_value()
    dst.ScalarMassPerVolume.Unit = src.get_unit().get_string()
def serialize_scalar_mass_per_volume_from_bind(src: ScalarMassPerVolumeData, dst: SEScalarMassPerVolume):
    dst.set_value(src.ScalarMassPerVolume.Value, MassPerVolumeUnit.from_string(src.ScalarMassPerVolume.Unit))

def serialize_scalar_power_to_bind(src: SEScalarPower, dst: ScalarPowerData):
    dst.ScalarPower.Value = src.get_value()
    dst.ScalarPower.Unit = src.get_unit().get_string()
def serialize_scalar_power_from_bind(src: ScalarPowerData, dst: SEScalarPower):
    dst.set_value(src.ScalarPower.Value, PowerUnit.from_string(src.ScalarPower.Unit))

def serialize_scalar_pressure_to_bind(src: SEScalarPressure, dst: ScalarPressureData):
    dst.ScalarPressure.Value = src.get_value()
    dst.ScalarPressure.Unit = src.get_unit().get_string()
def serialize_scalar_pressure_from_bind(src: ScalarPressureData, dst: SEScalarPressure):
    dst.set_value(src.ScalarPressure.Value, PressureUnit.from_string(src.ScalarPressure.Unit))

def serialize_scalar_pressure_per_volume_to_bind(src: SEScalarPressurePerVolume, dst: ScalarPressurePerVolumeData):
    dst.ScalarPressurePerVolume.Value = src.get_value()
    dst.ScalarPressurePerVolume.Unit = src.get_unit().get_string()
def serialize_scalar_pressure_per_volume_from_bind(src: ScalarPressurePerVolumeData, dst: SEScalarPressurePerVolume):
    dst.set_value(src.ScalarPressurePerVolume.Value, PressurePerVolumeUnit.from_string(src.ScalarPressurePerVolume.Unit))

def serialize_scalar_pressure_time_per_volume_to_bind(src: SEScalarPressureTimePerVolume, dst: ScalarPressureTimePerVolumeData):
    dst.ScalarPressureTimePerVolume.Value = src.get_value()
    dst.ScalarPressureTimePerVolume.Unit = src.get_unit().get_string()
def serialize_scalar_pressure_time_per_volume_from_bind(src: ScalarPressureTimePerVolumeData, dst: SEScalarPressureTimePerVolume):
    dst.set_value(src.ScalarPressureTimePerVolume.Value, PressureTimePerVolumeUnit.from_string(src.ScalarPressureTimePerVolume.Unit))

def serialize_scalar_temperature_to_bind(src: SEScalarTemperature, dst: ScalarTemperatureData):
    dst.ScalarTemperature.Value = src.get_value()
    dst.ScalarTemperature.Unit = src.get_unit().get_string()
def serialize_scalar_temperature_from_bind(src: ScalarTemperatureData, dst: SEScalarTemperature):
    dst.set_value(src.ScalarTemperature.Value, TemperatureUnit.from_string(src.ScalarTemperature.Unit))

def serialize_scalar_unsigned_to_bind(src: SEScalarUnsigned, dst: ScalarUnsignedData):
    dst.ScalarUnsigned.Value = src.get_value()
def serialize_scalar_unsigned_from_bind(src: ScalarUnsignedData, dst: SEScalarUnsigned):
    dst.set_value(src.ScalarUnsigned.Value)

def serialize_scalar_time_to_bind(src: SEScalarTime, dst: ScalarTimeData):
    dst.ScalarTime.Value = src.get_value()
    dst.ScalarTime.Unit = src.get_unit().get_string()
def serialize_scalar_time_from_bind(src: ScalarTimeData, dst: SEScalarTime):
    dst.set_value(src.ScalarTime.Value, TimeUnit.from_string(src.ScalarTime.Unit))

def serialize_scalar_volume_to_bind(src: SEScalarVolume, dst: ScalarVolumeData):
    dst.ScalarVolume.Value = src.get_value()
    dst.ScalarVolume.Unit = src.get_unit().get_string()
def serialize_scalar_volume_from_bind(src: ScalarVolumeData, dst: SEScalarVolume):
    dst.set_value(src.ScalarVolume.Value, VolumeUnit.from_string(src.ScalarVolume.Unit))

def serialize_scalar_volume_per_pressure_to_bind(src: SEScalarVolumePerPressure, dst: ScalarVolumePerPressureData):
    dst.ScalarVolumePerPressure.Value = src.get_value()
    dst.ScalarVolumePerPressure.Unit = src.get_unit().get_string()
def serialize_scalar_volume_per_pressure_from_bind(src: ScalarVolumePerPressureData, dst: SEScalarVolumePerPressure):
    dst.set_value(src.ScalarVolumePerPressure.Value, VolumePerPressureUnit.from_string(src.ScalarVolumePerPressure.Unit))

def serialize_scalar_volume_per_time_to_bind(src: SEScalarVolumePerTime, dst: ScalarVolumePerTimeData):
    dst.ScalarVolumePerTime.Value = src.get_value()
    dst.ScalarVolumePerTime.Unit = src.get_unit().get_string()
def serialize_scalar_volume_per_time_from_bind(src: ScalarVolumePerTimeData, dst: SEScalarVolumePerTime):
    dst.set_value(src.ScalarVolumePerTime.Value, VolumePerTimeUnit.from_string(src.ScalarVolumePerTime.Unit))

def serialize_scalar_volume_per_time_mass_to_bind(src: SEScalarVolumePerTimeMass, dst: ScalarVolumePerTimeMassData):
    dst.ScalarVolumePerTimeMass.Value = src.get_value()
    dst.ScalarVolumePerTimeMass.Unit = src.get_unit().get_string()
def serialize_scalar_volume_per_time_mass_from_bind(src: ScalarVolumePerTimeMassData, dst: SEScalarVolumePerTimeMass):
    dst.set_value(src.ScalarVolumePerTimeMass.Value, VolumePerTimeMassUnit.from_string(src.ScalarVolumePerTimeMass.Unit))

def serialize_scalar_volume_per_time_pressure_to_bind(src: SEScalarVolumePerTimePressure, dst: ScalarVolumePerTimePressureData):
    dst.ScalarVolumePerTimePressure.Value = src.get_value()
    dst.ScalarVolumePerTimePressure.Unit = src.get_unit().get_string()
def serialize_scalar_volume_per_time_pressure_from_bind(src: ScalarVolumePerTimePressureData, dst: SEScalarVolumePerTimePressure):
    dst.set_value(src.ScalarVolumePerTimePressure.Value, VolumePerTimePressureUnit.from_string(src.ScalarVolumePerTimePressure.Unit))
