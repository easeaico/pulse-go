# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.curve import SECurve
from pulse.cdm.engine import eSwitch
from pulse.cdm.scalars import SEScalarPressureTimePerVolume, SEScalarPressure, SEScalarTime, SEScalarUnsigned

from enum import Enum

class eDefaultType(Enum):
    Model = 0
    Zero = 1

class eLungCompartment(Enum):
    LeftLung = 0
    RightLung = 1
    # Lobes
    LeftSuperiorLobe = 2
    LeftInferiorLobe = 3
    RightSuperiorLobe = 4
    RightMiddleLobe = 5
    RightInferiorLobe = 6


class SECardiovascularMechanicsModifiers:
    __slots__ = ["_arterial_compliance_multiplier",
                 "_arterial_resistance_multiplier",
                 "_pulmonary_compliance_multiplier",
                 "_pulmonary_resistance_multiplier",
                 "_systemic_resistance_multiplier",
                 "_systemic_compliance_multiplier",
                 "_venous_compliance_multiplier",
                 "_venous_resistance_multiplier",
                 "_heart_rate_multiplier",
                 "_stroke_volume_multiplier"]

    def __init__(self):
        self._arterial_compliance_multiplier = None
        self._arterial_resistance_multiplier = None
        self._pulmonary_compliance_multiplier = None
        self._pulmonary_resistance_multiplier = None
        self._systemic_resistance_multiplier = None
        self._systemic_compliance_multiplier = None
        self._venous_compliance_multiplier = None
        self._venous_resistance_multiplier = None
        self._heart_rate_multiplier = None
        self._stroke_volume_multiplier = None

    def clear(self):
        if self._arterial_compliance_multiplier is not None: self._arterial_compliance_multiplier.invalidate()
        if self._arterial_resistance_multiplier is not None: self._arterial_resistance_multiplier.invalidate()
        if self._pulmonary_compliance_multiplier is not None: self._pulmonary_compliance_multiplier.invalidate()
        if self._pulmonary_resistance_multiplier is not None: self._pulmonary_resistance_multiplier.invalidate()
        if self._systemic_resistance_multiplier is not None: self._systemic_resistance_multiplier.invalidate()
        if self._systemic_compliance_multiplier is not None: self._systemic_compliance_multiplier.invalidate()
        if self._venous_compliance_multiplier is not None: self._venous_compliance_multiplier.invalidate()
        if self._venous_resistance_multiplier is not None: self._venous_resistance_multiplier.invalidate()
        if self._heart_rate_multiplier is not None: self._heart_rate_multiplier.invalidate()
        if self._stroke_volume_multiplier is not None: self._stroke_volume_multiplier.invalidate()

    def copy(self, src):
        if not isinstance(SERespiratoryMechanics, src):
            raise Exception("Provided argument must be a SERespiratoryMechanics")
        self.clear()
        if src.has_arterial_compliance_multiplier() is not None: self.get_arterial_compliance_multiplier().set(src._arterial_compliance_multiplier)
        if src.has_arterial_resistance_multiplier() is not None: self.get_arterial_resistance_multiplier().set(src._arterial_resistance_multiplier)
        if src.has_pulmonary_compliance_multiplier() is not None: self.get_pulmonary_compliance_multiplier.set(src._pulmonary_compliance_multiplier)
        if src.has_pulmonary_resistance_multiplier() is not None: self.get_pulmonary_resistance_multiplier.set(src._pulmonary_resistance_multiplier)
        if src.has_systemic_resistance_multiplier() is not None: self.get_systemic_resistance_multiplier.set(src._systemic_resistance_multiplier)
        if src.has_systemic_compliance_multiplier() is not None: self.get_systemic_compliance_multiplier.set(src._systemic_compliance_multiplier)
        if src.has_venous_compliance_multiplier() is not None: self.get_venous_compliance_multiplier.set(src._venous_compliance_multiplier)
        if src.has_venous_resistance_multiplier() is not None: self.get_venous_resistance_multiplier.set(src._venous_resistance_multiplier)
        if src.has_heart_rate_multiplier() is not None: self.get_heart_rate_multiplier.set(src._heart_rate_multiplier)
        if src.has_stroke_volume_multiplier() is not None: self.get_stroke_volume_multiplier.set(src._stroke_volume_multiplier)

    def has_arterial_compliance_multiplier(self):
        return False if self._arterial_compliance_multiplier is None else self._arterial_compliance_multiplier.is_valid()
    def get_arterial_compliance_multiplier(self):
        if self._arterial_compliance_multiplier is None:
            self._arterial_compliance_multiplier = SEScalarUnsigned()
        return self._arterial_compliance_multiplier

    def has_arterial_resistance_multiplier(self):
        return False if self._arterial_resistance_multiplier is None else self._arterial_resistance_multiplier.is_valid()
    def get_arterial_resistance_multiplier(self):
        if self._arterial_resistance_multiplier is None:
            self._arterial_resistance_multiplier = SEScalarUnsigned()
        return self._arterial_resistance_multiplier

    def has_pulmonary_compliance_multiplier(self):
        return False if self._pulmonary_compliance_multiplier is None else self._pulmonary_compliance_multiplier.is_valid()
    def get_pulmonary_compliance_multiplier(self):
        if self._pulmonary_compliance_multiplier is None:
            self._pulmonary_compliance_multiplier = SEScalarUnsigned()
        return self._pulmonary_compliance_multiplier

    def has_pulmonary_resistance_multiplier(self):
        return False if self._pulmonary_resistance_multiplier is None else self._pulmonary_resistance_multiplier.is_valid()
    def get_pulmonary_resistance_multiplier(self):
        if self._pulmonary_resistance_multiplier is None:
            self._pulmonary_resistance_multiplier = SEScalarUnsigned()
        return self._pulmonary_resistance_multiplier

    def has_systemic_resistance_multiplier(self):
        return False if self._systemic_resistance_multiplier is None else self._systemic_resistance_multiplier.is_valid()
    def get_systemic_resistance_multiplier(self):
        if self._systemic_resistance_multiplier is None:
            self._systemic_resistance_multiplier = SEScalarUnsigned()
        return self._systemic_resistance_multiplier

    def has_systemic_compliance_multiplier(self):
        return False if self._systemic_compliance_multiplier is None else self._systemic_compliance_multiplier.is_valid()
    def get_systemic_compliance_multiplier(self):
        if self._systemic_compliance_multiplier is None:
            self._systemic_compliance_multiplier = SEScalarUnsigned()
        return self._systemic_compliance_multiplier

    def has_venous_compliance_multiplier(self):
        return False if self._venous_compliance_multiplier is None else self._venous_compliance_multiplier.is_valid()
    def get_venous_compliance_multiplier(self):
        if self._venous_compliance_multiplier is None:
            self._venous_compliance_multiplier = SEScalarUnsigned()
        return self._venous_compliance_multiplier

    def has_venous_resistance_multiplier(self):
        return False if self._venous_resistance_multiplier is None else self._venous_resistance_multiplier.is_valid()
    def get_venous_resistance_multiplier(self):
        if self._venous_resistance_multiplier is None:
            self._venous_resistance_multiplier = SEScalarUnsigned()
        return self._venous_resistance_multiplier

    def has_heart_rate_multiplier(self):
        return False if self._heart_rate_multiplier is None else self._heart_rate_multiplier.is_valid()
    def get_heart_rate_multiplier(self):
        if self._heart_rate_multiplier is None:
            self._heart_rate_multiplier = SEScalarUnsigned()
        return self._heart_rate_multiplier

    def has_stroke_volume_multiplier(self):
        return False if self._stroke_volume_multiplier is None else self._stroke_volume_multiplier.is_valid()
    def get_stroke_volume_multiplier(self):
        if self._stroke_volume_multiplier is None:
            self._stroke_volume_multiplier = SEScalarUnsigned()
        return self._stroke_volume_multiplier


class SERespiratoryMechanics:
    __slots__ = ["_active",
                 "_default_type",
                 "_left_compliance_curve",
                 "_right_compliance_curve",
                 "_left_expiratory_resistance",
                 "_left_inspiratory_resistance",
                 "_right_expiratory_resistance",
                 "_right_inspiratory_resistance",
                 "_upper_expiratory_resistance",
                 "_upper_inspiratory_resistance",
                 "_inspiratory_peak_pressure",
                 "_expiratory_peak_pressure",
                 "_inspiratory_rise_time",
                 "_inspiratory_hold_time",
                 "_inspiratory_release_time",
                 "_inspiratory_to_expiratory_pause_time",
                 "_expiratory_rise_time",
                 "_expiratory_hold_time",
                 "_expiratory_release_time",
                 "_residue_time"]

    def __init__(self):
        self._active = eSwitch.NullSwitch
        self._default_type = eDefaultType.Model
        self._left_compliance_curve = None
        self._right_compliance_curve = None
        self._left_expiratory_resistance = None
        self._left_inspiratory_resistance = None
        self._right_expiratory_resistance = None
        self._right_inspiratory_resistance = None
        self._upper_expiratory_resistance = None
        self._upper_inspiratory_resistance = None
        self._inspiratory_peak_pressure = None
        self._expiratory_peak_pressure = None
        self._inspiratory_rise_time = None
        self._inspiratory_hold_time = None
        self._inspiratory_release_time = None
        self._inspiratory_to_expiratory_pause_time = None
        self._expiratory_rise_time = None
        self._expiratory_hold_time = None
        self._expiratory_release_time = None
        self._residue_time = None

    def clear(self):
        self._active = eSwitch.NullSwitch
        self._default_type = eDefaultType.Model
        if self._left_compliance_curve is not None: self._left_compliance_curve.invalidate()
        if self._right_compliance_curve is not None: self._right_compliance_curve.invalidate()
        if self._left_expiratory_resistance is not None: self._left_expiratory_resistance.invalidate()
        if self._left_inspiratory_resistance is not None: self._left_inspiratory_resistance.invalidate()
        if self._right_expiratory_resistance is not None: self._right_expiratory_resistance.invalidate()
        if self._right_inspiratory_resistance is not None: self._right_inspiratory_resistance.invalidate()
        if self._upper_expiratory_resistance is not None: self._upper_expiratory_resistance.invalidate()
        if self._upper_inspiratory_resistance is not None: self._upper_inspiratory_resistance.invalidate()
        if self._inspiratory_peak_pressure is not None: self._inspiratory_peak_pressure.invalidate()
        if self._expiratory_peak_pressure is not None: self._expiratory_peak_pressure.invalidate()
        if self._inspiratory_rise_time is not None: self._inspiratory_rise_time.invalidate()
        if self._inspiratory_hold_time is not None: self._inspiratory_hold_time.invalidate()
        if self._inspiratory_release_time is not None: self._inspiratory_release_time.invalidate()
        if self._inspiratory_to_expiratory_pause_time is not None: self._inspiratory_to_expiratory_pause_time.invalidate()
        if self._expiratory_rise_time is not None: self._expiratory_rise_time.invalidate()
        if self._expiratory_hold_time is not None: self._expiratory_hold_time.invalidate()
        if self._expiratory_release_time is not None: self._expiratory_release_time.invalidate()
        if self._residue_time is not None: self._residue_time.invalidate()

    def copy(self, src):
        if not isinstance(SERespiratoryMechanics, src):
            raise Exception("Provided argument must be a SERespiratoryMechanics")
        self.clear()
        self._active = src._active
        self._default_type = src._default_type
        if src.has_left_compliance_curve() is not None: self.get_left_compliance_curve().set(src._left_compliance_curve)
        if src.has_right_compliance_curve() is not None: self.get_right_compliance_curve().set(src._right_compliance_curve)
        if src.has_left_expiratory_resistance() is not None: self.get_left_expiratory_resistance.set(src._left_expiratory_resistance)
        if src.has_left_inspiratory_resistance() is not None: self.get_left_inspiratory_resistance.set(src._left_inspiratory_resistance)
        if src.has_right_expiratory_resistance() is not None: self.get_right_expiratory_resistance.set(src._right_expiratory_resistance)
        if src.has_right_inspiratory_resistance() is not None: self.get_right_inspiratory_resistance.set(src._right_inspiratory_resistance)
        if src.has_upper_expiratory_resistance() is not None: self.get_upper_expiratory_resistance.set(src._upper_expiratory_resistance)
        if src.has_upper_inspiratory_resistance() is not None: self.get_upper_inspiratory_resistance.set(src._upper_inspiratory_resistance)
        if src.has_inspiratory_peak_pressure() is not None: self.get_inspiratory_peak_pressure.set(src._inspiratory_peak_pressure)
        if src.has_expiratory_peak_pressure() is not None: self.get_expiratory_peak_pressure.set(src._expiratory_peak_pressure)
        if src.has_inspiratory_rise_time() is not None: self.get_inspiratory_rise_time.set(src._inspiratory_rise_time)
        if src.has_inspiratory_hold_time() is not None: self.get_inspiratory_hold_time.set(src._inspiratory_hold_time)
        if src.has_inspiratory_release_time() is not None: self.get_inspiratory_release_time.set(src._inspiratory_release_time)
        if src.has_inspiratory_to_expiratory_pause_time() is not None: self.get_inspiratory_to_expiratory_pause_time.set(src._inspiratory_to_expiratory_pause_time)
        if src.has_expiratory_rise_time() is not None: self.get_expiratory_rise_time.set(src._expiratory_rise_time)
        if src.has_expiratory_hold_time() is not None: self.get_expiratory_hold_time.set(src._expiratory_hold_time)
        if src.has_expiratory_release_time() is not None: self.get_expiratory_release_time.set(src._expiratory_release_time)
        if src.has_residue_time() is not None: self.get_residue_time.set(src._residue_time)

    def get_active(self):
        return self._active
    def set_active(self, src : eSwitch):
        self._active = src

    def get_default_type(self):
        return self._active
    def set_default_type(self, src : eDefaultType):
        self._active = src

    def has_left_compliance_curve(self):
        return False if self._left_compliance_curve is None else self._left_compliance_curve.is_valid()
    def get_left_compliance_curve(self):
        if self._left_compliance_curve is None:
            self._left_compliance_curve = SECurve()
        return self._left_compliance_curve

    def has_right_compliance_curve(self):
        return False if self._right_compliance_curve is None else self._right_compliance_curve.is_valid()
    def get_right_compliance_curve(self):
        if self._right_compliance_curve is None:
            self._right_compliance_curve = SECurve()
        return self._right_compliance_curve

    def has_left_expiratory_resistance(self):
        return False if self._left_expiratory_resistance is None else self._left_expiratory_resistance.is_valid()
    def get_left_expiratory_resistance(self):
        if self._left_expiratory_resistance is None:
            self._left_expiratory_resistance = SEScalarPressureTimePerVolume()
        return self._left_expiratory_resistance

    def has_left_inspiratory_resistance(self):
        return False if self._left_inspiratory_resistance is None else self._left_inspiratory_resistance.is_valid()
    def get_left_inspiratory_resistance(self):
        if self._left_inspiratory_resistance is None:
            self._left_inspiratory_resistance = SEScalarPressureTimePerVolume()
        return self._left_inspiratory_resistance

    def has_right_expiratory_resistance(self):
        return False if self._right_expiratory_resistance is None else self._right_expiratory_resistance.is_valid()
    def get_right_expiratory_resistance(self):
        if self._right_expiratory_resistance is None:
            self._right_expiratory_resistance = SEScalarPressureTimePerVolume()
        return self._right_expiratory_resistance

    def has_right_inspiratory_resistance(self):
        return False if self._right_inspiratory_resistance is None else self._right_inspiratory_resistance.is_valid()
    def get_right_inspiratory_resistance(self):
        if self._right_inspiratory_resistance is None:
            self._right_inspiratory_resistance = SEScalarPressureTimePerVolume()
        return self._right_inspiratory_resistance

    def has_upper_expiratory_resistance(self):
        return False if self._upper_expiratory_resistance is None else self._upper_expiratory_resistance.is_valid()
    def get_upper_expiratory_resistance(self):
        if self._upper_expiratory_resistance is None:
            self._upper_expiratory_resistance = SEScalarPressureTimePerVolume()
        return self._upper_expiratory_resistance

    def has_upper_inspiratory_resistance(self):
        return False if self._upper_inspiratory_resistance is None else self._upper_inspiratory_resistance.is_valid()
    def get_upper_inspiratory_resistance(self):
        if self._upper_inspiratory_resistance is None:
            self._upper_inspiratory_resistance = SEScalarPressureTimePerVolume()
        return self._upper_inspiratory_resistance

    def has_inspiratory_peak_pressure(self):
        return False if self._inspiratory_peak_pressure is None else self._inspiratory_peak_pressure.is_valid()
    def get_inspiratory_peak_pressure(self):
        if self._inspiratory_peak_pressure is None:
            self._inspiratory_peak_pressure = SEScalarPressure()
        return self._inspiratory_peak_pressure

    def has_expiratory_peak_pressure(self):
        return False if self._expiratory_peak_pressure is None else self._expiratory_peak_pressure.is_valid()
    def get_expiratory_peak_pressure(self):
        if self._expiratory_peak_pressure is None:
            self._expiratory_peak_pressure = SEScalarPressure()
        return self._expiratory_peak_pressure

    def has_inspiratory_rise_time(self):
        return False if self._inspiratory_rise_time is None else self._inspiratory_rise_time.is_valid()
    def get_inspiratory_rise_time(self):
        if self._inspiratory_rise_time is None:
            self._inspiratory_rise_time = SEScalarTime()
        return self._inspiratory_rise_time

    def has_inspiratory_hold_time(self):
        return False if self._inspiratory_hold_time is None else self._inspiratory_hold_time.is_valid()
    def get_inspiratory_hold_time(self):
        if self._inspiratory_hold_time is None:
            self._inspiratory_hold_time = SEScalarTime()
        return self._inspiratory_hold_time

    def has_inspiratory_release_time(self):
        return False if self._inspiratory_release_time is None else self._inspiratory_release_time.is_valid()
    def get_inspiratory_release_time(self):
        if self._inspiratory_release_time is None:
            self._inspiratory_release_time = SEScalarTime()
        return self._inspiratory_release_time

    def has_inspiratory_to_expiratory_pause_time(self):
        return False if self._inspiratory_to_expiratory_pause_time is None else self._inspiratory_to_expiratory_pause_time.is_valid()
    def get_inspiratory_to_expiratory_pause_time(self):
        if self._inspiratory_to_expiratory_pause_time is None:
            self._inspiratory_to_expiratory_pause_time = SEScalarTime()
        return self._inspiratory_to_expiratory_pause_time

    def has_expiratory_rise_time(self):
        return False if self._expiratory_rise_time is None else self._expiratory_rise_time.is_valid()
    def get_expiratory_rise_time(self):
        if self._expiratory_rise_time is None:
            self._expiratory_rise_time = SEScalarTime()
        return self._expiratory_rise_time

    def has_expiratory_hold_time(self):
        return False if self._expiratory_hold_time is None else self._expiratory_hold_time.is_valid()
    def get_expiratory_hold_time(self):
        if self._expiratory_hold_time is None:
            self._expiratory_hold_time = SEScalarTime()
        return self._expiratory_hold_time

    def has_expiratory_release_time(self):
        return False if self._expiratory_release_time is None else self._expiratory_release_time.is_valid()
    def get_expiratory_release_time(self):
        if self._expiratory_release_time is None:
            self._expiratory_release_time = SEScalarTime()
        return self._expiratory_release_time

    def has_residue_time(self):
        return False if self._residue_time is None else self._residue_time.is_valid()
    def get_residue_time(self):
        if self._residue_time is None:
            self._residue_time = SEScalarTime()
        return self._residue_time


class SERespiratoryMechanicsModifiers:
    __slots__ = ["_left_compliance_multiplier",
                 "_right_compliance_multiplier",
                 "_left_expiratory_resistance_multiplier",
                 "_left_inspiratory_resistance_multiplier",
                 "_right_expiratory_resistance_multiplier",
                 "_right_inspiratory_resistance_multiplier",
                 "_upper_expiratory_resistance_multiplier",
                 "_upper_inspiratory_resistance_multiplier",
                 "_respiration_rate_multiplier",
                 "_tidal_volume_multiplier",
                 "_volume_increments"]

    def __init__(self):
        self._left_compliance_multiplier = None
        self._right_compliance_multiplier = None
        self._left_expiratory_resistance_multiplier = None
        self._left_inspiratory_resistance_multiplier = None
        self._right_expiratory_resistance_multiplier = None
        self._right_inspiratory_resistance_multiplier = None
        self._upper_expiratory_resistance_multiplier = None
        self._upper_inspiratory_resistance_multiplier = None
        self._respiration_rate_multiplier = None
        self._tidal_volume_multiplier = None
        self._volume_increments = {}

    def clear(self):
        if self._left_compliance_multiplier is not None: self._left_compliance_multiplier.invalidate()
        if self._right_compliance_multiplier is not None: self._right_compliance_multiplier.invalidate()
        if self._left_expiratory_resistance_multiplier is not None: self._left_expiratory_resistance_multiplier.invalidate()
        if self._left_inspiratory_resistance_multiplier is not None: self._left_inspiratory_resistance_multiplier.invalidate()
        if self._right_expiratory_resistance_multiplier is not None: self._right_expiratory_resistance_multiplier.invalidate()
        if self._right_inspiratory_resistance_multiplier is not None: self._right_inspiratory_resistance_multiplier.invalidate()
        if self._upper_expiratory_resistance_multiplier is not None: self._upper_expiratory_resistance_multiplier.invalidate()
        if self._upper_inspiratory_resistance_multiplier is not None: self._upper_inspiratory_resistance_multiplier.invalidate()
        if self._respiration_rate_multiplier is not None: self._respiration_rate_multiplier.invalidate()
        if self._tidal_volume_multiplier is not None: self._tidal_volume_multiplier.invalidate()
        for s in self._volume_increments.values():
            if s is not None:
                s.invalidate()
    def copy(self, src):
        if not isinstance(SERespiratoryMechanics, src):
            raise Exception("Provided argument must be a SERespiratoryMechanics")
        self.clear()
        if src.has_left_compliance_multiplier() is not None: self.get_left_compliance_multiplier().set(src._left_compliance_multiplier)
        if src.has_right_compliance_multiplier() is not None: self.get_right_compliance_multiplier().set(src._right_compliance_multiplier)
        if src.has_left_expiratory_resistance_multiplier() is not None: self.get_left_expiratory_resistance_multiplier.set(src._left_expiratory_resistance_multiplier)
        if src.has_left_inspiratory_resistance_multiplier() is not None: self.get_left_inspiratory_resistance_multiplier.set(src._left_inspiratory_resistance_multiplier)
        if src.has_right_expiratory_resistance_multiplier() is not None: self.get_right_expiratory_resistance_multiplier.set(src._right_expiratory_resistance_multiplier)
        if src.has_right_inspiratory_resistance_multiplier() is not None: self.get_right_inspiratory_resistance_multiplier.set(src._right_inspiratory_resistance_multiplier)
        if src.has_upper_expiratory_resistance_multiplier() is not None: self.get_upper_expiratory_resistance_multiplier.set(src._upper_expiratory_resistance_multiplier)
        if src.has_upper_inspiratory_resistance_multiplier() is not None: self.get_upper_inspiratory_resistance_multiplier.set(src._upper_inspiratory_resistance_multiplier)
        if src.has_respiration_rate_multiplier() is not None: self.get_respiration_rate_multiplier.set(src._respiration_rate_multiplier)
        if src.has_tidal_volume_multiplier() is not None: self.get_tidal_volume_multiplier.set(src._tidal_volume_multiplier)
        for c, s in src.get_volume_increments().items():
            self.get_volume_increment(c).set(s)

    def has_left_compliance_multiplier(self):
        return False if self._left_compliance_multiplier is None else self._left_compliance_multiplier.is_valid()
    def get_left_compliance_multiplier(self):
        if self._left_compliance_multiplier is None:
            self._left_compliance_multiplier = SEScalarUnsigned()
        return self._left_compliance_multiplier

    def has_right_compliance_multiplier(self):
        return False if self._right_compliance_multiplier is None else self._right_compliance_multiplier.is_valid()
    def get_right_compliance_multiplier(self):
        if self._right_compliance_multiplier is None:
            self._right_compliance_multiplier = SEScalarUnsigned()
        return self._right_compliance_multiplier

    def has_left_expiratory_resistance_multiplier(self):
        return False if self._left_expiratory_resistance_multiplier is None else self._left_expiratory_resistance_multiplier.is_valid()
    def get_left_expiratory_resistance_multiplier(self):
        if self._left_expiratory_resistance_multiplier is None:
            self._left_expiratory_resistance_multiplier = SEScalarUnsigned()
        return self._left_expiratory_resistance_multiplier

    def has_left_inspiratory_resistance_multiplier(self):
        return False if self._left_inspiratory_resistance_multiplier is None else self._left_inspiratory_resistance_multiplier.is_valid()
    def get_left_inspiratory_resistance_multiplier(self):
        if self._left_inspiratory_resistance_multiplier is None:
            self._left_inspiratory_resistance_multiplier = SEScalarUnsigned()
        return self._left_inspiratory_resistance_multiplier

    def has_right_expiratory_resistance_multiplier(self):
        return False if self._right_expiratory_resistance_multiplier is None else self._right_expiratory_resistance_multiplier.is_valid()
    def get_right_expiratory_resistance_multiplier(self):
        if self._right_expiratory_resistance_multiplier is None:
            self._right_expiratory_resistance_multiplier = SEScalarUnsigned()
        return self._right_expiratory_resistance_multiplier

    def has_right_inspiratory_resistance_multiplier(self):
        return False if self._right_inspiratory_resistance_multiplier is None else self._right_inspiratory_resistance_multiplier.is_valid()
    def get_right_inspiratory_resistance_multiplier(self):
        if self._right_inspiratory_resistance_multiplier is None:
            self._right_inspiratory_resistance_multiplier = SEScalarUnsigned()
        return self._right_inspiratory_resistance_multiplier

    def has_upper_expiratory_resistance_multiplier(self):
        return False if self._upper_expiratory_resistance_multiplier is None else self._upper_expiratory_resistance_multiplier.is_valid()
    def get_upper_expiratory_resistance_multiplier(self):
        if self._upper_expiratory_resistance_multiplier is None:
            self._upper_expiratory_resistance_multiplier = SEScalarUnsigned()
        return self._upper_expiratory_resistance_multiplier

    def has_upper_inspiratory_resistance_multiplier(self):
        return False if self._upper_inspiratory_resistance_multiplier is None else self._upper_inspiratory_resistance_multiplier.is_valid()
    def get_upper_inspiratory_resistance_multiplier(self):
        if self._upper_inspiratory_resistance_multiplier is None:
            self._upper_inspiratory_resistance_multiplier = SEScalarUnsigned()
        return self._upper_inspiratory_resistance_multiplier

    def has_respiration_rate_multiplier(self):
        return False if self._respiration_rate_multiplier is None else self._respiration_rate_multiplier.is_valid()
    def get_respiration_rate_multiplier(self):
        if self._respiration_rate_multiplier is None:
            self._respiration_rate_multiplier = SEScalarUnsigned()
        return self._respiration_rate_multiplier

    def has_tidal_volume_multiplier(self):
        return False if self._tidal_volume_multiplier is None else self._tidal_volume_multiplier.is_valid()
    def get_tidal_volume_multiplier(self):
        if self._tidal_volume_multiplier is None:
            self._tidal_volume_multiplier = SEScalarUnsigned()
        return self._tidal_volume_multiplier

    def has_volume_increment(self, cmpt: eLungCompartment=None):
        if cmpt is None:
            for s in self._volume_increments.values():
                if s is not None and s.is_valid():
                    return True
            return False
        if cmpt not in self._volume_increments:
            return False
        return self._volume_increments.get(cmpt).is_valid()
    def get_volume_increment(self, cmpt: eLungCompartment):
        s = self._volume_increments.get(cmpt)
        if s is None:
            s = SEScalarUnsigned()
            self._volume_increments[cmpt] = s
        return s
