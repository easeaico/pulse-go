from pulse.cdm.physiology import SECardiovascularMechanicsModifiers, \
                                 SERespiratoryMechanics, SERespiratoryMechanicsModifiers
from pulse.cdm.bind.Physiology_pb2 import CardiovascularMechanicsModifiersData, \
                                          RespiratoryMechanicsData, RespiratoryMechanicsModifiersData

from pulse.cdm.io.curve import serialize_curve_to_bind
from pulse.cdm.io.scalars import serialize_scalar_pressure_time_per_volume_to_bind, \
                                 serialize_scalar_pressure_to_bind, \
                                 serialize_scalar_unsigned_to_bind, \
                                 serialize_scalar_time_to_bind


def serialize_cardiovascular_mechanics_modifiers_to_bind(src: SECardiovascularMechanicsModifiers, dst: CardiovascularMechanicsModifiersData):
    if src.has_arterial_compliance_multiplier():
        serialize_scalar_unsigned_to_bind(src.get_arterial_compliance_multiplier(), dst.ArterialComplianceMultiplier)
    if src.has_arterial_resistance_multiplier():
        serialize_scalar_unsigned_to_bind(src.get_arterial_resistance_multiplier(), dst.ArterialResistanceMultiplier)
    if src.has_pulmonary_compliance_multiplier():
        serialize_scalar_unsigned_to_bind(src.get_pulmonary_compliance_multiplier(), dst.PulmonaryComplianceMultiplier)
    if src.has_pulmonary_resistance_multiplier():
        serialize_scalar_unsigned_to_bind(src.get_pulmonary_resistance_multiplier(), dst.PulmonaryResistanceMultiplier)
    if src.has_systemic_resistance_multiplier():
        serialize_scalar_unsigned_to_bind(src.get_systemic_resistance_multiplier(), dst.SystemicResistanceMultiplier)
    if src.has_systemic_compliance_multiplier():
        serialize_scalar_unsigned_to_bind(src.get_systemic_compliance_multiplier(), dst.SystemicComplianceMultiplier)
    if src.has_venous_compliance_multiplier():
        serialize_scalar_unsigned_to_bind(src.get_venous_compliance_multiplier(), dst.VenousComplianceMultiplier)
    if src.has_venous_resistance_multiplier():
        serialize_scalar_unsigned_to_bind(src.get_venous_resistance_multiplier(), dst.VenousResistanceMultiplier)

    if src.has_heart_rate_multiplier():
        serialize_scalar_unsigned_to_bind(src.get_heart_rate_multiplier(), dst.HeartRateMultiplier)
    if src.has_stroke_volume_multiplier():
        serialize_scalar_unsigned_to_bind(src.get_stroke_volume_multiplier(), dst.StrokeVolumeMultiplier)


def serialize_cardiovascular_mechanics_modifiers_from_bind(src: CardiovascularMechanicsModifiersData, dst: SECardiovascularMechanicsModifiers):
    raise Exception("serialize_cardiovascular_mechanics_modifiers_from_bind not implemented")


def serialize_respiratory_mechanics_to_bind(src: SERespiratoryMechanics, dst: RespiratoryMechanicsData):
    dst.Active = src.get_active().value

    dst.DefaultType = src.get_default_type().value

    if src.has_left_compliance_curve():
        serialize_curve_to_bind(src.get_left_compliance_curve(), dst.LeftComplianceCurve)
    if src.has_right_compliance_curve():
        serialize_curve_to_bind(src.get_right_compliance_curve(), dst.RightComplianceCurve)

    if src.has_left_expiratory_resistance():
        serialize_scalar_pressure_time_per_volume_to_bind(src.get_left_expiratory_resistance(), dst.LeftExpiratoryResistance)
    if src.has_left_inspiratory_resistance():
        serialize_scalar_pressure_time_per_volume_to_bind(src.get_left_inspiratory_resistance(), dst.LeftInspiratoryResistance)
    if src.has_right_expiratory_resistance():
        serialize_scalar_pressure_time_per_volume_to_bind(src.get_right_expiratory_resistance(), dst.RightExpiratoryResistance)
    if src.has_right_inspiratory_resistance():
        serialize_scalar_pressure_time_per_volume_to_bind(src.get_right_inspiratory_resistance(), dst.RightInspiratoryResistance)
    if src.has_upper_expiratory_resistance():
        serialize_scalar_pressure_time_per_volume_to_bind(src.get_upper_expiratory_resistance(), dst.UpperExpiratoryResistance)
    if src.has_upper_inspiratory_resistance():
        serialize_scalar_pressure_time_per_volume_to_bind(src.get_upper_inspiratory_resistance(), dst.UpperInspiratoryResistance)

    if src.has_inspiratory_peak_pressure():
        serialize_scalar_pressure_to_bind(src.get_inspiratory_peak_pressure(), dst.InspiratoryPeakPressure)
    if src.has_expiratory_peak_pressure():
        serialize_scalar_pressure_to_bind(src.get_expiratory_peak_pressure(), dst.ExpiratoryPeakPressure)

    if src.has_inspiratory_rise_time():
        serialize_scalar_time_to_bind(src.get_inspiratory_rise_time(), dst.InspiratoryRiseTime)
    if src.has_expiratory_rise_time():
        serialize_scalar_time_to_bind(src.get_expiratory_rise_time(), dst.ExpiratoryRiseTime)
    if src.has_inspiratory_release_time():
        serialize_scalar_time_to_bind(src.get_inspiratory_release_time(), dst.InspiratoryReleaseTime)
    if src.has_inspiratory_to_expiratory_pause_time():
        serialize_scalar_time_to_bind(src.get_inspiratory_to_expiratory_pause_time(), dst.InspiratoryToExpiratoryPauseTime)
    if src.has_expiratory_rise_time():
        serialize_scalar_time_to_bind(src.get_expiratory_rise_time(), dst.ExpiratoryRiseTime)
    if src.has_expiratory_hold_time():
        serialize_scalar_time_to_bind(src.get_expiratory_hold_time(), dst.ExpiratoryHoldTime)
    if src.has_expiratory_release_time():
        serialize_scalar_time_to_bind(src.get_expiratory_release_time(), dst.ExpiratoryReleaseTime)
    if src.has_residue_time():
        serialize_scalar_time_to_bind(src.get_residue_time(), dst.ResidueTime)


def serialize_respiratory_mechanics_from_bind(src: RespiratoryMechanicsData, dst: SERespiratoryMechanics):
    raise Exception("serialize_respiratory_mechanics_from_bind not implemented")


def serialize_respiratory_mechanics_modifiers_to_bind(src: SERespiratoryMechanicsModifiers, dst: RespiratoryMechanicsModifiersData):
    if src.has_left_compliance_multiplier():
        serialize_scalar_unsigned_to_bind(src.get_left_compliance_multiplier(), dst.LeftComplianceMultiplier)
    if src.has_right_compliance_multiplier():
        serialize_scalar_unsigned_to_bind(src.get_right_compliance_multiplier(), dst.RightComplianceMultiplier)

    if src.has_left_expiratory_resistance_multiplier():
        serialize_scalar_unsigned_to_bind(src.get_left_expiratory_resistance_multiplier(), dst.LeftExpiratoryResistanceMultiplier)
    if src.has_left_inspiratory_resistance_multiplier():
        serialize_scalar_unsigned_to_bind(src.get_left_inspiratory_resistance_multiplier(), dst.LeftInspiratoryResistanceMultiplier)
    if src.has_right_expiratory_resistance_multiplier():
        serialize_scalar_unsigned_to_bind(src.get_right_expiratory_resistance_multiplier(), dst.RightExpiratoryResistanceMultiplier)
    if src.has_right_inspiratory_resistance_multiplier():
        serialize_scalar_unsigned_to_bind(src.get_right_inspiratory_resistance_multiplier(), dst.RightInspiratoryResistanceMultiplier)
    if src.has_upper_expiratory_resistance_multiplier():
        serialize_scalar_unsigned_to_bind(src.get_upper_expiratory_resistance_multiplier(), dst.UpperExpiratoryResistanceMultiplier)
    if src.has_upper_inspiratory_resistance_multiplier():
        serialize_scalar_unsigned_to_bind(src.get_upper_inspiratory_resistance_multiplier(), dst.UpperInspiratoryResistanceMultiplier)

    if src.has_respiration_rate_multiplier():
        serialize_scalar_unsigned_to_bind(src.get_respiration_rate_multiplier(), dst.RespirationRateMultiplier)
    if src.has_tidal_volume_multiplier():
        serialize_scalar_unsigned_to_bind(src.get_tidal_volume_multiplier(), dst.TidalVolumeMultiplier)

    for c, s in src._volume_increments.items():
        i = dst.VolumeIncrement.add()
        i.Compartment = c.value
        serialize_scalar_unsigned_to_bind(s, i.Increment)

def serialize_respiratory_mechanics_modifiers_from_bind(src: RespiratoryMechanicsModifiersData, dst: SERespiratoryMechanicsModifiers):
    raise Exception("serialize_respiratory_mechanics_modifiers_from_bind not implemented")
