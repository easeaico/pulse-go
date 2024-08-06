# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.bind.MechanicalVentilatorActions_pb2 import MechanicalVentilatorActionData, \
                                                           MechanicalVentilatorConfigurationData, \
                                                           MechanicalVentilatorModeData, \
                                                           MechanicalVentilatorContinuousPositiveAirwayPressureData, \
                                                           MechanicalVentilatorPressureControlData, \
                                                           MechanicalVentilatorVolumeControlData, \
                                                           MechanicalVentilatorHoldData, \
                                                           MechanicalVentilatorLeakData
from pulse.cdm.mechanical_ventilator_actions import SEMechanicalVentilatorAction, \
                                                    SEMechanicalVentilatorConfiguration, \
                                                    SEMechanicalVentilatorMode, \
                                                    SEMechanicalVentilatorContinuousPositiveAirwayPressure, \
                                                    SEMechanicalVentilatorPressureControl, \
                                                    SEMechanicalVentilatorVolumeControl, \
                                                    SEMechanicalVentilatorHold, \
                                                    SEMechanicalVentilatorLeak
from pulse.cdm.io.mechanical_ventilator import serialize_mechanical_ventilator_to_bind,\
                                               serialize_mechanical_ventilator_from_bind
from pulse.cdm.io.equipment_actions import serialize_equipment_action_from_bind, \
                                           serialize_equipment_action_to_bind
from pulse.cdm.io.scalars import serialize_scalar_pressure_to_bind, \
                                 serialize_scalar_0to1_to_bind, \
                                 serialize_scalar_time_to_bind, \
                                 serialize_scalar_frequency_to_bind, \
                                 serialize_scalar_volume_per_time_to_bind, \
                                 serialize_scalar_volume_to_bind

def serialize_mechanical_ventilator_action_to_bind(src: SEMechanicalVentilatorAction, dst:MechanicalVentilatorActionData):
    serialize_equipment_action_to_bind(src,dst.EquipmentAction)

def serialize_mechanical_ventilator_action_from_bind(src: MechanicalVentilatorActionData, dst: SEMechanicalVentilatorAction):
    serialize_equipment_action_from_bind(src.EquipmentAction, dst)

def serialize_mechanical_ventilator_configuration_to_bind(src: SEMechanicalVentilatorConfiguration, dst: MechanicalVentilatorConfigurationData):
    serialize_mechanical_ventilator_action_to_bind(src, dst.MechanicalVentilatorAction)
    if src.has_settings_file():
        dst.SettingsFile = src.get_settings_file()
    elif src.has_settings():
        serialize_mechanical_ventilator_to_bind(src.get_settings(), dst.Settings)

def serialize_mechanical_ventilator_configuration_from_bind(src: MechanicalVentilatorConfigurationData, dst: SEMechanicalVentilatorConfiguration):
    serialize_mechanical_ventilator_action_from_bind(src.MechanicalVentilatorAction, dst)
    raise Exception("serialize_mechanical_ventilator_configuration_from_bind not implemented")

def serialize_mechanical_ventilator_mode_to_bind(src: SEMechanicalVentilatorMode, dst: MechanicalVentilatorModeData):
    serialize_mechanical_ventilator_action_to_bind(src, dst.MechanicalVentilatorAction)
    dst.Connection = src.get_connection().value

def serialize_mechanical_ventilator_mode_from_bind(src: MechanicalVentilatorModeData, dst: SEMechanicalVentilatorMode):
    serialize_mechanical_ventilator_action_from_bind(src.MechanicalVentilatorAction, dst)
    raise Exception("serialize_mechanical_ventilator_mode_from_bind not implemented")

def serialize_mechanical_ventilator_continuous_positive_airway_pressure_to_bind(src: SEMechanicalVentilatorContinuousPositiveAirwayPressure, dst: MechanicalVentilatorContinuousPositiveAirwayPressureData):
    serialize_mechanical_ventilator_mode_to_bind(src, dst.MechanicalVentilatorMode)
    if src.has_delta_pressure_support():
        serialize_scalar_pressure_to_bind(src.get_delta_pressure_support(), dst.DeltaPressureSupport)
    if src.has_expiration_cycle_flow():
        serialize_scalar_volume_per_time_to_bind(src.get_expiration_cycle_flow(), dst.ExpirationCycleFlow)
    if src.has_expiration_cycle_pressure():
        serialize_scalar_pressure_to_bind(src.get_expiration_cycle_pressure(), dst.ExpirationCyclePressure)
    dst.ExpirationWaveform = src.get_expiration_waveform().value
    if src.has_fraction_inspired_oxygen():
        serialize_scalar_0to1_to_bind(src.get_fraction_inspired_oxygen(), dst.FractionInspiredOxygen)
    if src.has_inspiration_patient_trigger_flow():
        serialize_scalar_volume_per_time_to_bind(src.get_inspiration_patient_trigger_flow(), dst.InspirationCycleFlow)
    if src.has_inspiration_patient_trigger_pressure():
        serialize_scalar_pressure_to_bind(src.get_inspiration_patient_trigger_pressure(), dst.InspirationCyclePressure)
    dst.InspirationWaveform = src.get_inspiration_waveform().value
    if src.has_positive_end_expired_pressure():
        serialize_scalar_pressure_to_bind(src.get_positive_end_expired_pressure(), dst.PositiveEndExpiratoryPressure)
    if src.has_slope():
        serialize_scalar_time_to_bind(src.get_slope(), dst.Slope)

def serialize_mechanical_ventilator_continuous_positive_airway_pressure_from_bind(src: MechanicalVentilatorContinuousPositiveAirwayPressureData, dst: SEMechanicalVentilatorContinuousPositiveAirwayPressure):
    serialize_mechanical_ventilator_mode_from_bind(src.MechanicalVentilatorMode, dst)

def serialize_mechanical_ventilator_pressure_control_to_bind(src: SEMechanicalVentilatorPressureControl, dst: MechanicalVentilatorPressureControlData):
    serialize_mechanical_ventilator_mode_to_bind(src, dst.MechanicalVentilatorMode)
    dst.Mode = src.get_mode().value
    if src.has_fraction_inspired_oxygen():
        serialize_scalar_0to1_to_bind(src.get_fraction_inspired_oxygen(), dst.FractionInspiredOxygen)
    if src.has_inspiration_patient_trigger_flow():
        serialize_scalar_volume_per_time_to_bind(src.get_inspiration_patient_trigger_flow(), dst.InspirationCycleFlow)
    if src.has_inspiration_patient_trigger_pressure():
        serialize_scalar_pressure_to_bind(src.get_inspiration_patient_trigger_pressure(), dst.InspirationCyclePressure)
    dst.InspirationWaveform = src.get_inspiration_waveform().value
    if src.has_inspiratory_period():
        serialize_scalar_time_to_bind(src.get_inspiratory_period(), dst.InspiratoryPeriod)
    if src.has_inspiratory_pressure():
        serialize_scalar_pressure_to_bind(src.get_inspiratory_pressure(), dst.InspiratoryPressure)
    if src.has_positive_end_expired_pressure():
        serialize_scalar_pressure_to_bind(src.get_positive_end_expired_pressure(), dst.PositiveEndExpiratoryPressure)
    if src.has_respiration_rate():
        serialize_scalar_frequency_to_bind(src.get_respiration_rate(), dst.RespirationRate)
    if src.has_slope():
        serialize_scalar_time_to_bind(src.get_slope(), dst.Slope)

def serialize_mechanical_ventilator_pressure_control_from_bind(src: MechanicalVentilatorPressureControlData, dst: SEMechanicalVentilatorPressureControl):
    serialize_mechanical_ventilator_mode_from_bind(src.MechanicalVentilatorMode, dst)

def serialize_mechanical_ventilator_volume_control_to_bind(src: SEMechanicalVentilatorVolumeControl, dst: MechanicalVentilatorVolumeControlData):
    serialize_mechanical_ventilator_mode_to_bind(src, dst.MechanicalVentilatorMode)
    dst.Mode = src.get_mode().value
    if src.has_flow():
        serialize_scalar_volume_per_time_to_bind(src.get_flow(), dst.Flow)
    if src.has_fraction_inspired_oxygen():
        serialize_scalar_0to1_to_bind(src.get_fraction_inspired_oxygen(), dst.FractionInspiredOxygen)
    if src.has_inspiration_patient_trigger_flow():
        serialize_scalar_volume_per_time_to_bind(src.get_inspiration_patient_trigger_flow(), dst.InspirationCycleFlow)
    if src.has_inspiration_patient_trigger_pressure():
        serialize_scalar_pressure_to_bind(src.get_inspiration_patient_trigger_pressure(), dst.InspirationCyclePressure)
    dst.InspirationWaveform = src.get_inspiration_waveform().value
    if src.has_inspiratory_period():
        serialize_scalar_time_to_bind(src.get_inspiratory_period(), dst.InspiratoryPeriod)
    if src.has_positive_end_expired_pressure():
        serialize_scalar_pressure_to_bind(src.get_positive_end_expired_pressure(), dst.PositiveEndExpiratoryPressure)
    if src.has_respiration_rate():
        serialize_scalar_frequency_to_bind(src.get_respiration_rate(), dst.RespirationRate)
    if src.has_slope():
        serialize_scalar_time_to_bind(src.get_slope(), dst.Slope)
    if src.has_tidal_volume():
        serialize_scalar_volume_to_bind(src.get_tidal_volume(), dst.TidalVolume)

def serialize_mechanical_ventilator_volume_control_from_bind(src: MechanicalVentilatorVolumeControlData, dst: SEMechanicalVentilatorVolumeControl):
    serialize_mechanical_ventilator_mode_from_bind(src.MechanicalVentilatorMode, dst)

def serialize_mechanical_ventilator_hold_to_bind(src: SEMechanicalVentilatorHold, dst: MechanicalVentilatorHoldData):
    serialize_mechanical_ventilator_action_to_bind(src, dst.MechanicalVentilatorAction)
    dst.State = src.get_state().value
    dst.AppliedRespiratoryCycle = src.get_applied_respiratory_cycle()

def serialize_mechanical_ventilator_hold_from_bind(src: MechanicalVentilatorHoldData, dst: SEMechanicalVentilatorHold):
    serialize_mechanical_ventilator_action_from_bind(src.MechanicalVentilatorAction, dst)
    raise Exception("serialize_mechanical_ventilator_hold_from_bind not implemented")

def serialize_mechanical_ventilator_leak_to_bind(src: SEMechanicalVentilatorLeak, dst: MechanicalVentilatorLeakData):
    serialize_mechanical_ventilator_action_to_bind(src, dst.MechanicalVentilatorAction)
    if src.has_severity():
        serialize_scalar_0to1_to_bind(src.get_severity(), dst.Severity)

def serialize_mechanical_ventilator_leak_from_bind(src: MechanicalVentilatorLeakData, dst: SEMechanicalVentilatorLeak):
    serialize_mechanical_ventilator_action_from_bind(src.MechanicalVentilatorAction, dst)
    raise Exception("serialize_mechanical_ventilator_leak_from_bind not implemented")
