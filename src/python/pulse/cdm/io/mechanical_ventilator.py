# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.mechanical_ventilator import SEMechanicalVentilatorAlarms, SEMechanicalVentilatorSettings
from pulse.cdm.bind.MechanicalVentilator_pb2 import MechanicalVentilatorAlarmsData, MechanicalVentilatorSettingsData
from pulse.cdm.bind.Substance_pb2 import SubstanceFractionData, SubstanceConcentrationData

from pulse.cdm.io.scalars import *


def serialize_mechanical_ventilator_alarms_to_bind(src: SEMechanicalVentilatorAlarms, dst: MechanicalVentilatorAlarmsData):

    if src.has_apnea_time_threshold():
        serialize_scalar_time_to_bind(src.get_apnea_time_threshold(), dst.ApneaTimeThreshold)
    if src.has_auto_positive_end_expiratory_pressure_threshold():
        serialize_scalar_pressure_to_bind(src.get_auto_positive_end_expiratory_pressure_threshold(), dst.AutoPositiveEndExpiratoryPressureThreshold)
    if src.has_circuit_leak_threshold():
        serialize_scalar_0to1_to_bind(src.get_circuit_leak_threshold(), dst.CircuitLeakThreshold)
    if src.has_high_end_tidal_carbon_dioxide_threshold():
        serialize_scalar_pressure_to_bind(src.get_high_end_tidal_carbon_dioxide_threshold(), dst.HighEndTidalCarbonDioxideThreshold)
    if src.has_high_minute_ventilation_threshold():
        serialize_scalar_volume_per_time_to_bind(src.get_high_minute_ventilation_threshold(), dst.HighMinuteVentilationThreshold)
    if src.has_high_oxygen_saturation_threshold():
        serialize_scalar_0to1_to_bind(src.get_high_oxygen_saturation_threshold(), dst.HighOxygenSaturationThreshold)
    if src.has_high_positive_end_expiratory_pressure_threshold():
        serialize_scalar_pressure_to_bind(src.get_high_positive_end_expiratory_pressure_threshold(), dst.HighPositiveEndExpiratoryPressureThreshold)
    dst.HighPressureCycleOption = src.get_high_pressure_cycle_option().value
    if src.has_high_pressure_threshold():
        serialize_scalar_pressure_to_bind(src.get_high_pressure_threshold(), dst.HighPressureThreshold)
    if src.has_high_respiratory_rate_threshold():
        serialize_scalar_frequency_to_bind(src.get_high_respiratory_rate_threshold(), dst.HighRespiratoryRateThreshold)
    if src.has_high_tidal_volume_threshold():
        serialize_scalar_volume_to_bind(src.get_high_tidal_volume_threshold(), dst.HighTidalVolumeThreshold)
    if src.has_low_end_tidal_carbon_dioxide_threshold():
        serialize_scalar_pressure_to_bind(src.get_low_end_tidal_carbon_dioxide_threshold(), dst.LowEndTidalCarbonDioxideThreshold)
    if src.has_low_minute_ventilation_threshold():
        serialize_scalar_volume_per_time_to_bind(src.get_low_minute_ventilation_threshold(), dst.LowMinuteVentilationThreshold)
    if src.has_low_oxygen_saturation_threshold():
        serialize_scalar_0to1_to_bind(src.get_low_oxygen_saturation_threshold(), dst.LowOxygenSaturationThreshold)
    if src.has_low_positive_end_expiratory_pressure_threshold():
        serialize_scalar_pressure_to_bind(src.get_low_positive_end_expiratory_pressure_threshold(), dst.LowPositiveEndExpiratoryPressureThreshold)
    if src.has_low_pressure_threshold():
        serialize_scalar_pressure_to_bind(src.get_low_pressure_threshold(), dst.LowPressureThreshold)
    if src.has_low_tidal_volume_threshold():
        serialize_scalar_volume_to_bind(src.get_low_tidal_volume_threshold(), dst.LowTidalVolumeThreshold)
    if src.has_oxygen_supply_failure_threshold():
        serialize_scalar_0to1_to_bind(src.get_oxygen_supply_failure_threshold(), dst.OxygenSupplyFailureThreshold)

def serialize_mechanical_ventilator_alarms_from_bind(src: MechanicalVentilatorAlarmsData, dst: SEMechanicalVentilatorAlarms):
    raise Exception("serialize_mechanical_ventilator_alarms_from_bind not implemented")


def serialize_mechanical_ventilator_settings_to_bind(src: SEMechanicalVentilatorSettings, dst: MechanicalVentilatorSettingsData):
    dst.Connection = src.get_connection().value
    if src.has_connection_volume():
        serialize_scalar_volume_to_bind(src.get_connection_volume(), dst.ConnectionVolume)
    if src.has_compliance():
        serialize_scalar_volume_per_pressure_to_bind(src.get_compliance(), dst.Compliance)
    if src.has_driver_damping_parameter():
        serialize_scalar_frequency_to_bind(src.get_driver_damping_parameter(), dst.DriverDampingParameter)

    if src.has_positive_end_expired_pressure():
        serialize_scalar_pressure_to_bind(src.get_positive_end_expired_pressure(), dst.PositiveEndExpiratoryPressure)
    elif src.has_functional_residual_capacity():
        serialize_scalar_pressure_to_bind(src.get_functional_residual_capacity(), dst.FunctionalResidualCapacity)

    if src.has_expiration_cycle_flow():
        serialize_scalar_volume_per_time_to_bind(src.get_expiration_cycle_flow(), dst.ExpirationCycleFlow)
    elif src.has_expiration_cycle_pressure():
        serialize_scalar_pressure_to_bind(src.get_expiration_cycle_pressure(), dst.ExpirationCyclePressure)
    elif src.has_expiration_cycle_time():
        serialize_scalar_time_to_bind(src.get_expiration_cycle_time(), dst.ExpirationCycleTime)
    elif src.has_expiration_cycle_volume():
        serialize_scalar_volume_to_bind(src.get_expiration_cycle_volume(), dst.ExpirationCycleVolume)
    else:
        dst.ExpirationCycleRespiratoryModel = src.get_expiration_cycle_respiratory_model().value

    if src.has_expiration_limb_volume():
        serialize_scalar_volume_to_bind(src.get_expiration_limb_volume(), dst.ExpirationLimbVolume)
    if src.has_expiration_tube_resistance():
        serialize_scalar_pressure_time_per_volume_to_bind(src.get_expiration_tube_resistance(), dst.ExpirationTubeResistance)
    if src.has_expiration_valve_resistance():
        serialize_scalar_pressure_time_per_volume_to_bind(src.get_expiration_valve_resistance(), dst.ExpirationValveResistance)
    if src.has_expiration_valve_volume():
        serialize_scalar_volume_to_bind(src.get_expiration_valve_volume(), dst.ExpirationValveVolume)
    dst.ExpirationWaveform = src.get_expiration_waveform().value
    if src.has_expiration_waveform_period():
        serialize_scalar_time_to_bind(src.get_expiration_waveform_period(), dst.ExpirationWaveformPeriod)

    if src.has_inspiration_limit_flow():
        serialize_scalar_volume_per_time_to_bind(src.get_inspiration_limit_flow(), dst.InspirationLimitFlow)
    elif src.has_inspiration_limit_pressure():
        serialize_scalar_pressure_to_bind(src.get_inspiration_limit_pressure(), dst.InspirationLimitPressure)
    elif src.has_inspiration_limit_volume():
        serialize_scalar_volume_to_bind(src.get_inspiration_limit_volume(), dst.InspirationLimitVolume)

    if src.has_inspiration_pause_time():
        serialize_scalar_time_to_bind(src.get_inspiration_pause_time(), dst.InspirationPauseTime)

    if src.has_peak_inspiratory_pressure():
        serialize_scalar_pressure_to_bind(src.get_peak_inspiratory_pressure(), dst.PeakInspiratoryPressure)
    elif src.has_inspiration_target_flow():
        serialize_scalar_pressure_to_bind(src.get_inspiration_target_flow(), dst.InspirationTargetFlow)

    if src.has_inspiration_machine_trigger_time():
        serialize_scalar_time_to_bind(src.get_inspiration_machine_trigger_time(), dst.InspirationMachineTriggerTime)

    if src.has_inspiration_patient_trigger_flow():
        serialize_scalar_volume_per_time_to_bind(src.get_inspiration_patient_trigger_flow(), dst.InspirationPatientTriggerFlow)
    elif src.has_inspiration_patient_trigger_pressure():
        serialize_scalar_pressure_to_bind(src.get_inspiration_patient_trigger_pressure(), dst.InspirationPatientTriggerPressure)
    else:
        dst.InspirationPatientTriggerRespiratoryModel = src.get_inspiration_patient_trigger_respiratory_model().value

    if src.has_inspiration_limb_volume():
        serialize_scalar_volume_to_bind(src.get_inspiration_limb_volume(), dst.InspirationLimbVolume)
    if src.has_inspiration_tube_resistance():
        serialize_scalar_pressure_time_per_volume_to_bind(src.get_inspiration_tube_resistance(), dst.InspirationTubeResistance)
    if src.has_inspiration_valve_resistance():
        serialize_scalar_pressure_time_per_volume_to_bind(src.get_inspiration_valve_resistance(), dst.InspirationValveResistance)
    if src.has_inspiration_valve_volume():
        serialize_scalar_volume_to_bind(src.get_inspiration_valve_volume(), dst.InspirationValveVolume)
    dst.InspirationWaveform = src.get_inspiration_waveform().value
    if src.has_inspiration_waveform_period():
        serialize_scalar_time_to_bind(src.get_inspiration_waveform_period(), dst.InspirationWaveformPeriod)

    if src.has_relief_valve_threshold():
        serialize_scalar_pressure_to_bind(src.get_relief_valve_threshold(), dst.ReliefValveThreshold)
    if src.has_y_piece_volume():
        serialize_scalar_volume_to_bind(src.get_y_piece_volume(), dst.YPieceVolume)

    if src.has_alarms():
        serialize_mechanical_ventilator_alarms_to_bind(src.get_alarms(), dst.Alarms)

    for aGas in src.get_fraction_inspired_gasses():
        sf = SubstanceFractionData()
        sf.Name = aGas.get_substance()
        serialize_scalar_0to1_to_bind(aGas.get_fraction_amount(), sf.Amount)
        dst.FractionInspiredGas.append(sf)

    for aAerosol in src.get_concentration_inspired_aerosols():
        sc = SubstanceConcentrationData()
        sc.Name = aAerosol.get_substance()
        serialize_scalar_mass_per_volume_to_bind(aAerosol.get_concentration(), sc.Concentration)
        dst.ConcentrationInspiredAerosol.append(sc)


def serialize_mechanical_ventilator_settings_from_bind(src: MechanicalVentilatorSettingsData, dst: SEMechanicalVentilatorSettings):
    raise Exception("serialize_mechanical_ventilator_settings_from_bind not implemented")
