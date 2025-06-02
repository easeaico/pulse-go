# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from enum import Enum
from pulse.cdm.engine import eSwitch
from pulse.cdm.equipment import SEEquipment
from pulse.cdm.substance import SESubstanceFraction, \
                                SESubstanceConcentration
from pulse.cdm.scalars import SEScalar0To1, SEScalarFrequency, SEScalarPressureTimePerVolume, SEScalarVolumePerTime, \
                              SEScalarPressure, SEScalarTime, SEScalarVolume, SEScalarVolumePerPressure, \
                              MassPerVolumeUnit


class eDriverWaveform(Enum):
    NullDriverWaveform = 0
    Square = 1
    AscendingRamp = 2
    DescendingRamp = 3
    ExponentialGrowth = 4
    ExponentialDecay = 5
    SinusoidalRise = 6
    SinusoidalFall = 7
    SigmoidalRise = 8
    SigmoidalFall = 9


class SEMechanicalVentilatorAlarms:
    __slots__ = ["_apnea_time_threshold",
                 "_auto_positive_end_expiratory_pressure_threshold",
                 "_circuit_leak_threshold",
                 "_high_end_tidal_carbon_dioxide_threshold",
                 "_high_minute_ventilation_threshold",
                 "_high_oxygen_saturation_threshold",
                 "_high_positive_end_expiratory_pressure_threshold",
                 "_high_pressure_cycle_option",
                 "_high_pressure_threshold",
                 "_high_respiratory_rate_threshold",
                 "_high_tidal_volume_threshold",
                 "_low_end_tidal_carbon_dioxide_threshold",
                 "_low_minute_ventilation_threshold",
                 "_low_oxygen_saturation_threshold",
                 "_low_positive_end_expiratory_pressure_threshold",
                 "_low_pressure_threshold",
                 "_low_tidal_volume_threshold",
                 "_oxygen_supply_failure_threshold"
                ]

    def __init__(self):
        super().__init__()
        self._apnea_time_threshold = None
        self._auto_positive_end_expiratory_pressure_threshold = None
        self._circuit_leak_threshold = None
        self._high_end_tidal_carbon_dioxide_threshold = None
        self._high_minute_ventilation_threshold = None
        self._high_oxygen_saturation_threshold = None
        self._high_positive_end_expiratory_pressure_threshold = None
        self._high_pressure_cycle_option = eSwitch.NullSwitch
        self._high_pressure_threshold = None
        self._high_respiratory_rate_threshold = None
        self._high_tidal_volume_threshold = None
        self._low_end_tidal_carbon_dioxide_threshold = None
        self._low_minute_ventilation_threshold = None
        self._low_oxygen_saturation_threshold = None
        self._low_positive_end_expiratory_pressure_threshold = None
        self._low_pressure_threshold = None
        self._low_tidal_volume_threshold = None
        self._oxygen_supply_failure_threshold = None

    def clear(self):
        if self._apnea_time_threshold is not None:
            self._apnea_time_threshold.invalidate()
        if self._auto_positive_end_expiratory_pressure_threshold is not None:
            self._auto_positive_end_expiratory_pressure_threshold.invalidate()
        if self._circuit_leak_threshold is not None:
            self._circuit_leak_threshold.invalidate()
        if self._high_end_tidal_carbon_dioxide_threshold is not None:
            self._high_end_tidal_carbon_dioxide_threshold.invalidate()
        if self._high_minute_ventilation_threshold is not None:
            self._high_minute_ventilation_threshold.invalidate()
        if self._high_oxygen_saturation_threshold is not None:
            self._high_oxygen_saturation_threshold.invalidate()
        if self._high_positive_end_expiratory_pressure_threshold is not None:
            self._high_positive_end_expiratory_pressure_threshold.invalidate()
        self._high_pressure_cycle_option = eSwitch.NullSwitch
        if self._high_pressure_threshold is not None:
            self._high_pressure_threshold.invalidate()
        if self._high_respiratory_rate_threshold is not None:
            self._high_respiratory_rate_threshold.invalidate()
        if self._high_tidal_volume_threshold is not None:
            self._high_tidal_volume_threshold.invalidate()
        if self._low_end_tidal_carbon_dioxide_threshold is not None:
            self._low_end_tidal_carbon_dioxide_threshold.invalidate()
        if self._low_minute_ventilation_threshold is not None:
            self._low_minute_ventilation_threshold.invalidate()
        if self._low_oxygen_saturation_threshold is not None:
            self._low_oxygen_saturation_threshold.invalidate()
        if self._low_positive_end_expiratory_pressure_threshold is not None:
            self._low_positive_end_expiratory_pressure_threshold.invalidate()
        if self._low_pressure_threshold is not None:
            self._low_pressure_threshold.invalidate()
        if self._low_tidal_volume_threshold is not None:
            self._low_tidal_volume_threshold.invalidate()
        if self._oxygen_supply_failure_threshold is not None:
            self._oxygen_supply_failure_threshold.invalidate()

    def copy(self, src):
        if not isinstance(SEMechanicalVentilatorAlarms, src):
            raise Exception("Provided argument must be a SEMechanicalVentilatorAlarms")
        self.clear()
        if src.has_apnea_time_threshold():
            self.get_apnea_time_threshold().set(src._apnea_time_threshold)
        if src.has_auto_positive_end_expiratory_pressure_threshold():
            self.get_auto_positive_end_expiratory_pressure_threshold().set(src._auto_positive_end_expiratory_pressure_threshold)
        if src.has_circuit_leak_threshold():
            self.get_circuit_leak_threshold().set(src._circuit_leak_threshold)
        if src.has_high_end_tidal_carbon_dioxide_threshold():
            self.get_high_end_tidal_carbon_dioxide_threshold().set(src._high_end_tidal_carbon_dioxide_threshold)
        if src.has_high_minute_ventilation_threshold():
            self.get_high_minute_ventilation_threshold().set(src._high_minute_ventilation_threshold)
        if src.has_high_oxygen_saturation_threshold():
            self.get_high_oxygen_saturation_threshold().set(src._high_oxygen_saturation_threshold)
        if src.has_high_positive_end_expiratory_pressure_threshold():
            self.get_high_positive_end_expiratory_pressure_threshold().set(src._high_positive_end_expiratory_pressure_threshold)
        if src.has_high_pressure_cycle_option():
            self.get_high_pressure_cycle_option().set(src._high_pressure_cycle_option)
        self._high_pressure_threshold = src._high_pressure_threshold
        if src.has_high_respiratory_rate_threshold():
            self.get_high_respiratory_rate_threshold().set(src._high_respiratory_rate_threshold)
        if src.has_high_tidal_volume_threshold():
            self.get_high_tidal_volume_threshold().set(src._high_tidal_volume_threshold)
        if src.has_low_end_tidal_carbon_dioxide_threshold():
            self.get_low_end_tidal_carbon_dioxide_threshold().set(src._low_end_tidal_carbon_dioxide_threshold)
        if src.has_low_minute_ventilation_threshold():
            self.get_low_minute_ventilation_threshold().set(src._low_minute_ventilation_threshold)
        if src.has_low_oxygen_saturation_threshold():
            self.get_low_oxygen_saturation_threshold().set(src._low_oxygen_saturation_threshold)
        if src.has_low_positive_end_expiratory_pressure_threshold():
            self.get_low_positive_end_expiratory_pressure_threshold().set(src._low_positive_end_expiratory_pressure_threshold)
        if src.has_low_pressure_threshold():
            self.get_low_pressure_threshold().set(src._low_pressure_threshold)
        if src.has_low_tidal_volume_threshold():
            self.get_low_tidal_volume_threshold().set(src._low_tidal_volume_threshold)
        if src.has_oxygen_supply_failure_threshold():
            self.get_oxygen_supply_failure_threshold().set(src._oxygen_supply_failure_threshold)

    def has_apnea_time_threshold(self):
        return False if self._apnea_time_threshold is None else self._apnea_time_threshold.is_valid()
    def get_apnea_time_threshold(self):
        if self._apnea_time_threshold is None:
            self._apnea_time_threshold = SEScalarTime()
        return self._apnea_time_threshold

    def has_auto_positive_end_expiratory_pressure_threshold(self):
        return False if self._auto_positive_end_expiratory_pressure_threshold is None else self._auto_positive_end_expiratory_pressure_threshold.is_valid()
    def get_auto_positive_end_expiratory_pressure_threshold(self):
        if self._auto_positive_end_expiratory_pressure_threshold is None:
            self._auto_positive_end_expiratory_pressure_threshold = SEScalarPressure()
        return self._auto_positive_end_expiratory_pressure_threshold

    def has_circuit_leak_threshold(self):
        return False if self._circuit_leak_threshold is None else self._circuit_leak_threshold.is_valid()
    def get_circuit_leak_threshold(self):
        if self._circuit_leak_threshold is None:
            self._circuit_leak_threshold = SEScalar0To1()
        return self._circuit_leak_threshold

    def has_high_end_tidal_carbon_dioxide_threshold(self):
        return False if self._high_end_tidal_carbon_dioxide_threshold is None else self._high_end_tidal_carbon_dioxide_threshold.is_valid()
    def get_high_end_tidal_carbon_dioxide_threshold(self):
        if self._high_end_tidal_carbon_dioxide_threshold is None:
            self._high_end_tidal_carbon_dioxide_threshold = SEScalarPressure()
        return self._high_end_tidal_carbon_dioxide_threshold

    def has_high_minute_ventilation_threshold(self):
        return False if self._high_minute_ventilation_threshold is None else self._high_minute_ventilation_threshold.is_valid()
    def get_high_minute_ventilation_threshold(self):
        if self._high_minute_ventilation_threshold is None:
            self._high_minute_ventilation_threshold = SEScalarVolumePerTime()
        return self._high_minute_ventilation_threshold

    def has_high_oxygen_saturation_threshold(self):
        return False if self._high_oxygen_saturation_threshold is None else self._high_oxygen_saturation_threshold.is_valid()
    def get_high_oxygen_saturation_threshold(self):
        if self._high_oxygen_saturation_threshold is None:
            self._high_oxygen_saturation_threshold = SEScalar0To1()
        return self._high_oxygen_saturation_threshold

    def has_high_positive_end_expiratory_pressure_threshold(self):
        return False if self._high_positive_end_expiratory_pressure_threshold is None else self._high_positive_end_expiratory_pressure_threshold.is_valid()
    def get_high_positive_end_expiratory_pressure_threshold(self):
        if self._high_positive_end_expiratory_pressure_threshold is None:
            self._high_positive_end_expiratory_pressure_threshold = SEScalarPressure()
        return self._high_positive_end_expiratory_pressure_threshold

    def get_high_pressure_cycle_option(self):
        return self._high_pressure_cycle_option
    def set_high_pressure_cycle_option(self, t: eSwitch):
        self._high_pressure_cycle_option = t
    def has_high_pressure_threshold(self):
        return self._high_pressure_cycle_option != eSwitch.NullSwitch

    def has_high_pressure_threshold(self):
        return False if self._high_pressure_threshold is None else self._high_pressure_threshold.is_valid()
    def get_high_pressure_threshold(self):
        if self._high_pressure_threshold is None:
            self._high_pressure_threshold = SEScalarPressure()
        return self._high_pressure_threshold

    def has_high_respiratory_rate_threshold(self):
        return False if self._high_respiratory_rate_threshold is None else self._high_respiratory_rate_threshold.is_valid()
    def get_high_respiratory_rate_threshold(self):
        if self._high_respiratory_rate_threshold is None:
            self._high_respiratory_rate_threshold = SEScalarFrequency()
        return self._high_respiratory_rate_threshold

    def has_high_tidal_volume_threshold(self):
        return False if self._high_tidal_volume_threshold is None else self._high_tidal_volume_threshold.is_valid()
    def get_high_tidal_volume_threshold(self):
        if self._high_tidal_volume_threshold is None:
            self._high_tidal_volume_threshold = SEScalarVolume()
        return self._high_tidal_volume_threshold

    def has_low_end_tidal_carbon_dioxide_threshold(self):
        return False if self._low_end_tidal_carbon_dioxide_threshold is None else self._low_end_tidal_carbon_dioxide_threshold.is_valid()
    def get_low_end_tidal_carbon_dioxide_threshold(self):
        if self._low_end_tidal_carbon_dioxide_threshold is None:
            self._low_end_tidal_carbon_dioxide_threshold = SEScalarPressure()
        return self._low_end_tidal_carbon_dioxide_threshold

    def has_low_minute_ventilation_threshold(self):
        return False if self._low_minute_ventilation_threshold is None else self._low_minute_ventilation_threshold.is_valid()
    def get_low_minute_ventilation_threshold(self):
        if self._low_minute_ventilation_threshold is None:
            self._low_minute_ventilation_threshold = SEScalarVolumePerTime()
        return self._low_minute_ventilation_threshold

    def has_low_oxygen_saturation_threshold(self):
        return False if self._low_oxygen_saturation_threshold is None else self._low_oxygen_saturation_threshold.is_valid()
    def get_low_oxygen_saturation_threshold(self):
        if self._low_oxygen_saturation_threshold is None:
            self._low_oxygen_saturation_threshold = SEScalar0To1()
        return self._low_oxygen_saturation_threshold

    def has_low_positive_end_expiratory_pressure_threshold(self):
        return False if self._low_positive_end_expiratory_pressure_threshold is None else self._low_positive_end_expiratory_pressure_threshold.is_valid()
    def get_low_positive_end_expiratory_pressure_threshold(self):
        if self._low_positive_end_expiratory_pressure_threshold is None:
            self._low_positive_end_expiratory_pressure_threshold = SEScalarPressure()
        return self._low_positive_end_expiratory_pressure_threshold

    def has_low_pressure_threshold(self):
        return False if self._low_pressure_threshold is None else self._low_pressure_threshold.is_valid()
    def get_low_pressure_threshold(self):
        if self._low_pressure_threshold is None:
            self._low_pressure_threshold = SEScalarPressure()
        return self._low_pressure_threshold

    def has_low_tidal_volume_threshold(self):
        return False if self._low_tidal_volume_threshold is None else self._low_tidal_volume_threshold.is_valid()
    def get_low_tidal_volume_threshold(self):
        if self._low_tidal_volume_threshold is None:
            self._low_tidal_volume_threshold = SEScalarVolume()
        return self._low_tidal_volume_threshold

    def has_oxygen_supply_failure_threshold(self):
        return False if self._oxygen_supply_failure_threshold is None else self._oxygen_supply_failure_threshold.is_valid()
    def get_oxygen_supply_failure_threshold(self):
        if self._oxygen_supply_failure_threshold is None:
            self._oxygen_supply_failure_threshold = SEScalar0To1()
        return self._oxygen_supply_failure_threshold


class SEMechanicalVentilatorSettings:
    __slots__ = ["_connection",
                 "_connection_volume",
                 "_compliance",
                 "_driver_damping_parameter",
                 # One of
                 "_positive_end_expired_pressure",
                 "_functional_residual_capacity",
                 # One of
                 "_expiration_cycle_flow",
                 "_expiration_cycle_pressure",
                 "_expiration_cycle_time",
                 "_expiration_cycle_volume",
                 "_expiration_cycle_respiratory_model",

                 "_expiration_limb_volume",
                 "_expiration_tube_resistance",
                 "_expiration_valve_resistance",
                 "_expiration_valve_volume",
                 "_expiration_waveform",
                 "_expiration_waveform_period",
                 # One of
                 "_inspiration_limit_flow",
                 "_inspiration_limit_pressure",
                 "_inspiration_limit_volume",

                 "_inspiration_pause_time",
                 # One of
                 "_peak_inspiratory_pressure",
                 "_inspiration_target_flow",
                 # One of
                 "_inspiration_machine_trigger_time",
                 # One of
                 "_inspiration_patient_trigger_flow",
                 "_inspiration_patient_trigger_pressure",
                 "_inspiration_patient_trigger_respiratory_model",

                 "_inspiration_limb_volume",
                 "_inspiration_tube_resistance",
                 "_inspiration_valve_resistance",
                 "_inspiration_valve_volume",
                 "_inspiration_waveform",
                 "_inspiration_waveform_period",

                 "_relief_valve_threshold",
                 "_y_piece_volume",

                 "_alarms",

                 "_fraction_inspired_gasses",
                 "_concentration_inspired_aerosol"]

    def __init__(self):
        super().__init__()
        self._connection = eSwitch.NullSwitch
        self._connection_volume = None
        self._compliance = None
        self._driver_damping_parameter = None

        self._positive_end_expired_pressure = None
        self._functional_residual_capacity = None

        self._expiration_cycle_flow = None
        self._expiration_cycle_pressure = None
        self._expiration_cycle_time = None
        self._expiration_cycle_volume = None
        self._expiration_cycle_respiratory_model = eSwitch.NullSwitch

        self._expiration_limb_volume = None
        self._expiration_tube_resistance = None
        self._expiration_valve_resistance = None
        self._expiration_valve_volume = None
        self._expiration_waveform = eDriverWaveform.NullDriverWaveform
        self._expiration_waveform_period = None

        self._inspiration_limit_flow = None
        self._inspiration_limit_pressure = None
        self._inspiration_limit_volume = None

        self._inspiration_pause_time = None

        self._peak_inspiratory_pressure = None
        self._inspiration_target_flow = None

        self._inspiration_machine_trigger_time = None

        self._inspiration_patient_trigger_flow = None
        self._inspiration_patient_trigger_pressure = None
        self._inspiration_patient_trigger_respiratory_model = eSwitch.NullSwitch

        self._inspiration_limb_volume = None
        self._inspiration_tube_resistance = None
        self._inspiration_valve_resistance = None
        self._inspiration_valve_volume = None
        self._inspiration_waveform = eDriverWaveform.NullDriverWaveform
        self._inspiration_waveform_period = None

        self._relief_valve_threshold = None
        self._y_piece_volume = None

        self._alarms = None

        self._fraction_inspired_gasses = []
        self._concentration_inspired_aerosol = []

    def clear(self):
        self._connection = eSwitch.NullSwitch
        if self._connection_volume is not None: self._connection_volume.invalidate()
        if self._compliance is not None: self._compliance.invalidate()
        if self._driver_damping_parameter is not None: self._driver_damping_parameter.invalidate()

        if self._positive_end_expired_pressure is not None: self._positive_end_expired_pressure.invalidate()
        if self._functional_residual_capacity is not None: self._functional_residual_capacity.invalidate()

        if self._expiration_cycle_flow is not None: self._expiration_cycle_flow.invalidate()
        if self._expiration_cycle_pressure is not None: self._expiration_cycle_pressure.invalidate()
        if self._expiration_cycle_time is not None: self._expiration_cycle_time.invalidate()
        if self._expiration_cycle_volume is not None: self._expiration_cycle_volume.invalidate()
        self._expiration_cycle_respiratory_model = eSwitch.NullSwitch

        if self._expiration_limb_volume is not None: self._expiration_limb_volume.invalidate()
        if self._expiration_tube_resistance is not None: self._expiration_tube_resistance.invalidate()
        if self._expiration_valve_resistance is not None: self._expiration_valve_resistance.invalidate()
        if self._expiration_valve_volume is not None: self._expiration_valve_volume.invalidate()
        self._expiration_waveform = eDriverWaveform.NullDriverWaveform
        if self._expiration_waveform_period is not None: self._expiration_waveform_period.invalidate()

        if self._inspiration_limit_flow is not None: self._inspiration_limit_flow.invalidate()
        if self._inspiration_limit_pressure is not None: self._inspiration_limit_pressure.invalidate()
        if self._inspiration_limit_volume is not None: self._inspiration_limit_volume.invalidate()

        if self._inspiration_pause_time is not None: self._inspiration_pause_time.invalidate()

        if self._peak_inspiratory_pressure is not None: self._peak_inspiratory_pressure.invalidate()
        if self._inspiration_target_flow is not None: self._inspiration_target_flow.invalidate()
        self._inspiration_patient_trigger_respiratory_model = eSwitch.NullSwitch

        if self._inspiration_machine_trigger_time is not None: self._inspiration_machine_trigger_time.invalidate()

        if self._inspiration_patient_trigger_flow is not None: self._inspiration_patient_trigger_flow.invalidate()
        if self._inspiration_patient_trigger_pressure is not None: self._inspiration_patient_trigger_pressure.invalidate()

        if self._inspiration_limb_volume is not None: self._inspiration_limb_volume.invalidate()
        if self._inspiration_tube_resistance is not None: self._inspiration_tube_resistance.invalidate()
        if self._inspiration_valve_resistance is not None: self._inspiration_valve_resistance.invalidate()
        if self._inspiration_valve_volume is not None: self._inspiration_valve_volume.invalidate()
        self._inspiration_waveform = eDriverWaveform.NullDriverWaveform
        if self._inspiration_waveform_period is not None: self._inspiration_waveform_period.invalidate()

        if self._relief_valve_threshold is not None: self._relief_valve_threshold.invalidate()
        if self._y_piece_volume is not None: self._y_piece_volume.invalidate()

        if self._alarms is not None: self._alarms.clear()

        self._fraction_inspired_gasses = []
        self._concentration_inspired_aerosol = []

    def copy(self, src):
        if not isinstance(SEMechanicalVentilatorSettings, src):
            raise Exception("Provided argument must be a SEMechanicalVentilatorSettings")
        self.clear()
        self._connection = src._connection
        if src.has_connection_volume(): self.get_connection_volume().set(src._connection_volume)
        if src.has_compliance(): self.get_compliance().set(src._compliance)
        if src.has_driver_damping_parameter(): self.get_driver_damping_parameter().set(src._driver_damping_parameter)

        if src.has_positive_end_expired_pressure(): self.get_positive_end_expired_pressure().set(src._positive_end_expired_pressure)
        if src.has_functional_residual_capacity(): self.get_functional_residual_capacity().set(src._functional_residual_capacity)

        if src.has_expiration_cycle_flow(): self.get_expiration_cycle_flow().set(src._expiration_cycle_flow)
        if src.has_expiration_cycle_pressure(): self.get_expiration_cycle_pressure().set(src._expiration_cycle_pressure)
        if src.has_expiration_cycle_time(): self.get_expiration_cycle_time().set(src._expiration_cycle_time)
        if src.has_expiration_cycle_volume(): self.get_expiration_cycle_volume().set(src._expiration_cycle_volume)
        self._expiration_cycle_respiratory_model = src._expiration_cycle_respiratory_model

        if src.has_expiration_limb_volume(): self.get_expiration_limb_volume().set(src._expiration_limb_volume)
        if src.has_expiration_tube_resistance(): self.get_expiration_tube_resistance().set(src._expiration_tube_resistance)
        if src.has_expiration_valve_resistance(): self.get_expiration_valve_resistance().set(src._expiration_valve_resistance)
        if src.has_expiration_valve_volume(): self.get_expiration_valve_volume().set(src._expiration_valve_volume)
        self._expiration_waveform = src._expiration_waveform
        if src.has_expiration_waveform_period(): self.get_expiration_waveform_period().set(src._expiration_waveform_period)

        if src.has_inspiration_limit_flow(): self.get_inspiration_limit_flow().set(src._inspiration_limit_flow)
        if src.has_inspiration_limit_pressure(): self.get_inspiration_limit_pressure().set(src._inspiration_limit_pressure)
        if src.has_inspiration_limit_volume(): self.get_inspiration_limit_volume().set(src._inspiration_limit_volume)

        if src.has_inspiration_pause_time(): self.get_inspiration_pause_time().set(src._inspiration_pause_time)

        if src.has_peak_inspiratory_pressure(): self.get_peak_inspiratory_pressure().set(src._peak_inspiratory_pressure)
        if src.has_inspiration_target_flow(): self.get_inspiration_target_flow().set(src._inspiration_target_flow)

        if src.has_inspiration_machine_trigger_time(): self.get_inspiration_machine_trigger_time().set(src._inspiration_machine_trigger_time)

        if src.has_inspiration_patient_trigger_flow(): self.get_inspiration_patient_trigger_flow().set(src._inspiration_patient_trigger_flow)
        if src.has_inspiration_patient_trigger_pressure(): self.get_inspiration_patient_trigger_pressure().set(src._inspiration_patient_trigger_pressure)
        self._inspiration_patient_trigger_respiratory_model = src._inspiration_patient_trigger_respiratory_model

        if src.has_inspiration_limb_volume(): self.get_inspiration_limb_volume().set(src._inspiration_limb_volume)
        if src.has_inspiration_tube_resistance(): self.get_inspiration_tube_resistance().set(src._inspiration_tube_resistance)
        if src.has_inspiration_valve_resistance(): self.get_inspiration_valve_resistance().set(src._inspiration_valve_resistance)
        if src.has_inspiration_valve_volume(): self.get_inspiration_valve_volume().set(src._inspiration_valve_volume)
        self._inspiration_waveform = src._inspiration_waveform
        if src.has_inspiration_waveform_period(): self.get_inspiration_waveform_period().set(src._inspiration_waveform_period)

        if src.has_relief_valve_threshold(): self.get_relief_valve_threshold().set(src._relief_valve_threshold)
        if src.has_y_piece_volume(): self.get_y_piece_volume().set(src._y_piece_volume)

        if src.has_alarms(): self.get_alarms().copy(src._alarms)

        if src.has_fraction_inspired_gasses:
            self._fraction_inspired_gasses.append(src._fraction_inspired_gasses[:])
        if src.has_concentration_inspired_aerosol:
            self._concentration_inspired_aerosol.append(src._concentration_inspired_aerosol[:])

    def get_connection(self):
        return self._connection
    def set_connection(self, t: eSwitch):
        self._connection = t
    def has_connection(self):
        return self._connection != eSwitch.NullSwitch

    def has_connection_volume(self):
        return False if self._connection_volume is None else self._connection_volume.is_valid()
    def get_connection_volume(self):
        if self._connection_volume is None:
            self._connection_volume = SEScalarVolume()
        return self._connection_volume

    def has_compliance(self):
        return False if self._compliance is None else self._compliance.is_valid()
    def get_compliance(self):
        if self._compliance is None:
            self._compliance = SEScalarVolumePerPressure()
        return self._compliance

    def has_driver_damping_parameter(self):
        return False if self._driver_damping_parameter is None else self._driver_damping_parameter.is_valid()
    def get_driver_damping_parameter(self):
        if self._driver_damping_parameter is None:
            self._driver_damping_parameter = SEScalarFrequency()
        return self._driver_damping_parameter

    def has_positive_end_expired_pressure(self):
        return False if self._positive_end_expired_pressure is None else self._positive_end_expired_pressure.is_valid()
    def get_positive_end_expired_pressure(self):
        if self._positive_end_expired_pressure is None:
            self._positive_end_expired_pressure = SEScalarPressure()
        return self._positive_end_expired_pressure
    
    def has_functional_residual_capacity(self):
        return False if self._functional_residual_capacity is None else self._functional_residual_capacity.is_valid()
    def get_functional_residual_capacity(self):
        if self._functional_residual_capacity is None:
            self._functional_residual_capacity = SEScalarPressure()
        return self._functional_residual_capacity
    
    def has_expiration_cycle_flow(self):
        return False if self._expiration_cycle_flow is None else self._expiration_cycle_flow.is_valid()
    def get_expiration_cycle_flow(self):
        if self._expiration_cycle_flow is None:
            self._expiration_cycle_flow = SEScalarVolumePerTime()
        return self._expiration_cycle_flow
    
    def has_expiration_cycle_pressure(self):
        return False if self._expiration_cycle_pressure is None else self._expiration_cycle_pressure.is_valid()
    def get_expiration_cycle_pressure(self):
        if self._expiration_cycle_pressure is None:
            self._expiration_cycle_pressure = SEScalarPressure()
        return self._expiration_cycle_pressure
    
    def has_expiration_cycle_time(self):
        return False if self._expiration_cycle_time is None else self._expiration_cycle_time.is_valid()
    def get_expiration_cycle_time(self):
        if self._expiration_cycle_time is None:
            self._expiration_cycle_time = SEScalarTime()
        return self._expiration_cycle_time
    
    def has_expiration_cycle_volume(self):
        return False if self._expiration_cycle_volume is None else self._expiration_cycle_volume.is_valid()
    def get_expiration_cycle_volume(self):
        if self._expiration_cycle_volume is None:
            self._expiration_cycle_volume = SEScalarVolume()
        return self._expiration_cycle_volume

    def get_expiration_cycle_respiratory_model(self):
        return self._expiration_cycle_respiratory_model
    def set_expiration_cycle_respiratory_model(self, s: eSwitch):
        self._expiration_cycle_respiratory_model = s
    def has_expiration_cycle_respiratory_model(self):
        return self._expiration_cycle_respiratory_model != eSwitch.NullSwitch

    def has_expiration_limb_volume(self):
        return False if self._expiration_limb_volume is None else self._expiration_limb_volume.is_valid()
    def get_expiration_limb_volume(self):
        if self._expiration_limb_volume is None:
            self._expiration_limb_volume = SEScalarVolume()
        return self._expiration_limb_volume
    
    def has_expiration_tube_resistance(self):
        return False if self._expiration_tube_resistance is None else self._expiration_tube_resistance.is_valid()
    def get_expiration_tube_resistance(self):
        if self._expiration_tube_resistance is None:
            self._expiration_tube_resistance = SEScalarPressureTimePerVolume()
        return self._expiration_tube_resistance
    
    def has_expiration_valve_resistance(self):
        return False if self._expiration_valve_resistance is None else self._expiration_valve_resistance.is_valid()
    def get_expiration_valve_resistance(self):
        if self._expiration_valve_resistance is None:
            self._expiration_valve_resistance = SEScalarPressureTimePerVolume()
        return self._expiration_valve_resistance
    
    def has_expiration_valve_volume(self):
        return False if self._expiration_valve_volume is None else self._expiration_valve_volume.is_valid()
    def get_expiration_valve_volume(self):
        if self._expiration_valve_volume is None:
            self._expiration_valve_volume = SEScalarVolume()
        return self._expiration_valve_volume

    def get_expiration_waveform(self):
        return self._expiration_waveform
    def set_expiration_waveform(self, t: eDriverWaveform):
        self._expiration_waveform = t

    def has_expiration_waveform_period(self):
        return False if self._expiration_waveform_period is None else self._expiration_waveform_period.is_valid()
    def get_expiration_waveform_period(self):
        if self._expiration_waveform_period is None:
            self._expiration_waveform_period = SEScalarTime()
        return self._expiration_waveform_period
        
    def has_inspiration_limit_flow(self):
        return False if self._inspiration_limit_flow is None else self._inspiration_limit_flow.is_valid()
    def get_inspiration_limit_flow(self):
        if self._inspiration_limit_flow is None:
            self._inspiration_limit_flow = SEScalarVolumePerTime()
        return self._inspiration_limit_flow

    def has_inspiration_limit_pressure(self):
        return False if self._inspiration_limit_pressure is None else self._inspiration_limit_pressure.is_valid()
    def get_inspiration_limit_pressure(self):
        if self._inspiration_limit_pressure is None:
            self._inspiration_limit_pressure = SEScalarPressure()
        return self._inspiration_limit_pressure

    def has_inspiration_limit_volume(self):
        return False if self._inspiration_limit_volume is None else self._inspiration_limit_volume.is_valid()
    def get_inspiration_limit_volume(self):
        if self._inspiration_limit_volume is None:
            self._inspiration_limit_volume = SEScalarVolume()
        return self._inspiration_limit_volume
        
    def has_inspiration_pause_time(self):
        return False if self._inspiration_pause_time is None else self._inspiration_pause_time.is_valid()
    def get_inspiration_pause_time(self):
        if self._inspiration_pause_time is None:
            self._inspiration_pause_time = SEScalarTime()
        return self._inspiration_pause_time

    def has_peak_inspiratory_pressure(self):
        return False if self._peak_inspiratory_pressure is None else self._peak_inspiratory_pressure.is_valid()
    def get_peak_inspiratory_pressure(self):
        if self._peak_inspiratory_pressure is None:
            self._peak_inspiratory_pressure = SEScalarPressure()
        return self._peak_inspiratory_pressure

    def has_inspiration_target_flow(self):
        return False if self._inspiration_target_flow is None else self._inspiration_target_flow.is_valid()
    def get_inspiration_target_flow(self):
        if self._inspiration_target_flow is None:
            self._inspiration_target_flow = SEScalarPressure()
        return self._inspiration_target_flow

    def has_inspiration_machine_trigger_time(self):
        return False if self._inspiration_machine_trigger_time is None else self._inspiration_machine_trigger_time.is_valid()
    def get_inspiration_machine_trigger_time(self):
        if self._inspiration_machine_trigger_time is None:
            self._inspiration_machine_trigger_time = SEScalarTime()
        return self._inspiration_machine_trigger_time

    def has_inspiration_patient_trigger_flow(self):
        return False if self._inspiration_patient_trigger_flow is None else self._inspiration_patient_trigger_flow.is_valid()
    def get_inspiration_patient_trigger_flow(self):
        if self._inspiration_patient_trigger_flow is None:
            self._inspiration_patient_trigger_flow = SEScalarVolumePerTime()
        return self._inspiration_patient_trigger_flow

    def has_inspiration_patient_trigger_pressure(self):
        return False if self._inspiration_patient_trigger_pressure is None else self._inspiration_patient_trigger_pressure.is_valid()
    def get_inspiration_patient_trigger_pressure(self):
        if self._inspiration_patient_trigger_pressure is None:
            self._inspiration_patient_trigger_pressure = SEScalarPressure()
        return self._inspiration_patient_trigger_pressure

    def get_inspiration_patient_trigger_respiratory_model(self):
        return self._inspiration_patient_trigger_respiratory_model
    def set_inspiration_patient_trigger_respiratory_model(self, s: eSwitch):
        self._inspiration_patient_trigger_respiratory_model = s
    def has_inspiration_patient_trigger_respiratory_model(self):
        return self._inspiration_patient_trigger_respiratory_model != eSwitch.NullSwitch

    def has_inspiration_limb_volume(self):
        return False if self._inspiration_limb_volume is None else self._inspiration_limb_volume.is_valid()
    def get_inspiration_limb_volume(self):
        if self._inspiration_limb_volume is None:
            self._inspiration_limb_volume = SEScalarVolume()
        return self._inspiration_limb_volume

    def has_inspiration_tube_resistance(self):
        return False if self._inspiration_tube_resistance is None else self._inspiration_tube_resistance.is_valid()
    def get_inspiration_tube_resistance(self):
        if self._inspiration_tube_resistance is None:
            self._inspiration_tube_resistance = SEScalarPressureTimePerVolume()
        return self._inspiration_tube_resistance

    def has_inspiration_valve_resistance(self):
        return False if self._inspiration_valve_resistance is None else self._inspiration_valve_resistance.is_valid()
    def get_inspiration_valve_resistance(self):
        if self._inspiration_valve_resistance is None:
            self._inspiration_valve_resistance = SEScalarPressureTimePerVolume()
        return self._inspiration_valve_resistance

    def has_inspiration_valve_volume(self):
        return False if self._inspiration_valve_volume is None else self._inspiration_valve_volume.is_valid()
    def get_inspiration_valve_volume(self):
        if self._inspiration_valve_volume is None:
            self._inspiration_valve_volume = SEScalarVolume()
        return self._inspiration_valve_volume

    def has_relief_valve_threshold(self):
        return False if self._relief_valve_threshold is None else self._relief_valve_threshold.is_valid()
    def get_relief_valve_threshold(self):
        if self._relief_valve_threshold is None:
            self._relief_valve_threshold = SEScalarPressure()
        return self._relief_valve_threshold

    def has_y_piece_volume(self):
        return False if self._y_piece_volume is None else self._y_piece_volume.is_valid()
    def get_y_piece_volume(self):
        if self._y_piece_volume is None:
            self._y_piece_volume = SEScalarVolume()
        return self._y_piece_volume

    def get_inspiration_waveform(self):
        return self._inspiration_waveform
    def set_inspiration_waveform(self, t: eDriverWaveform):
        self._inspiration_waveform = t

    def has_inspiration_waveform_period(self):
        return False if self._inspiration_waveform_period is None else self._inspiration_waveform_period.is_valid()
    def get_inspiration_waveform_period(self):
        if self._inspiration_waveform_period is None:
            self._inspiration_waveform_period = SEScalarTime()
        return self._inspiration_waveform_period

    def has_alarms(self):
        return False if self._alarms is None else True
    def get_alarms(self):
        if self._alarms is None:
            self._alarms = SEMechanicalVentilatorAlarms()
        return self._alarms
        
    def has_fraction_inspired_gas(self, substance_name: str = None):
        if substance_name is None:
            return len(self._fraction_inspired_gasses) > 0
        else:
            for sf in self._fraction_inspired_gasses:
                if sf.get_substance() is substance_name:
                    return True
        return False
    def get_fraction_inspired_gas(self, substance_name: str):
        for sf in self._fraction_inspired_gasses:
            if sf.get_substance() is substance_name:
                return sf
        sf = SESubstanceFraction()
        sf.set_substance(substance_name)
        sf.get_fraction_amount().set_value(0)
        self._fraction_inspired_gasses.append(sf)
        return sf
    def get_fraction_inspired_gasses(self):
        return self._fraction_inspired_gasses
    def remove_fraction_inspired_gas(self, substance_name: str):
        for sf in self._fraction_inspired_gasses:
            if sf.get_substance() is substance_name:
                self._fraction_inspired_gasses.remove(sf)
    def remove_fraction_inspired_gasses(self):
        self._fraction_inspired_gasses = []

    def has_concentration_inspired_aerosol(self, substance_name: str=None):
        if substance_name is None:
            return len(self._concentration_inspired_aerosol) > 0
        else:
            for sf in self._concentration_inspired_aerosol:
                if sf.get_substance() is substance_name:
                    return True
        return False
    def get_concentration_inspired_aerosol(self, substance_name: str):
        for sf in self._concentration_inspired_aerosol:
            if sf.get_substance() is substance_name:
                return sf
        sf = SESubstanceConcentration()
        sf.set_substance(substance_name)
        sf.get_concentration().set_value(0, MassPerVolumeUnit.kg_Per_m3)
        self._concentration_inspired_aerosol.append(sf)
        return sf
    def get_concentration_inspired_aerosols(self):
        return self._concentration_inspired_aerosol
    def remove_concentration_inspired_aerosol(self, substance_name: str):
        for sf in self._concentration_inspired_aerosol:
            if sf.get_substance() is substance_name:
                self._concentration_inspired_aerosol.remove(sf)
    def remove_concentration_inspired_aerosol(self):
        self._concentration_inspired_aerosol = []