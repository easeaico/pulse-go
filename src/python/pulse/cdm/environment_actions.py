# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.enums import eSwitch
from pulse.cdm.engine import SEAction
from pulse.cdm.environment import SEActiveConditioning, \
                                  SEAppliedTemperature, \
                                  SEEnvironmentalConditions

class SEEnvironmentAction(SEAction):
    def __init__(self):
        super().__init__()

class SEThermalApplication(SEEnvironmentAction):
    __slots__ = ["_clear_contents",
                 "_active_heating",
                 "_active_cooling",
                 "_applied_temperature"]

    def __init__(self):
        super().__init__()
        self._clear_contents = True
        self._active_heating = None
        self._active_cooling = None
        self._applied_temperature = None

    def clear(self):
        if self._clear_contents:
            self._active_heating = None
            self._active_cooling = None
            self._applied_temperature = None

    def copy(self, src):
        if not isinstance(SEThermalApplication, src):
            raise Exception("Provided argument must be a SEThermalApplication")
        self.clear()
        self._clear_contents = src._clear_contents
        if src.has_active_heating(): self.get_active_heating().copy(src._active_heating)
        if src.has_active_cooling(): self.get_active_cooling().copy(src._active_cooling)
        if src.has_applied_temperature(): self.get_applied_temperature().copy(src._applied_temperature)

    def is_valid(self):
        return True

    def is_active(self):
        if self.has_active_heating() and self._active_heating.get_power().is_positive():
            return True;
        if self.has_active_cooling() and self._active_cooling.get_power().is_positive():
            return True;
        if self.has_applied_temperature() and self._applied_temperature.get_state() is eSwitch.On:
            return True;
        return False;

    def get_clear_contents(self):
        return self._clear_contents
    def set_state(self, t: bool):
        self._clear_contents = t

    def has_active_heating(self):
        return self._active_heating is not None
    def get_active_heating(self):
        if self._active_heating is None:
            self._active_heating = SEActiveConditioning()
        return self._active_heating

    def has_active_cooling(self):
        return self._active_cooling is not None
    def get_active_cooling(self):
        if self._active_cooling is None:
            self._active_cooling = SEActiveConditioning()
        return self._active_cooling

    def has_applied_temperature(self):
        return self._applied_temperature is not None
    def get_applied_temperature(self):
        if self._applied_temperature is None:
            self._applied_temperature = SEAppliedTemperature()
        return self._applied_temperature

class SEChangeEnvironmentalConditions(SEEnvironmentAction):
    __slots__ = ["_environmental_conditions_file",
                 "_environmental_conditions"]

    def __init__(self):
        super().__init__()
        self._environmental_conditions_file = None
        self._environmental_conditions = None

    def clear(self):
        self._environmental_conditions_file = None
        self._environmental_conditions = None

    def copy(self, src):
        if not isinstance(SEChangeEnvironmentalConditions, src):
            raise Exception("Provided argument must be a SEChangeEnvironmentalConditions")
        self.clear()
        self._environmental_conditions_file = src._environmental_conditions_file
        self._environmental_conditions.copy(src._environmental_conditions)

    def is_valid(self):
        return self.has_environmental_conditions() or self.has_environmental_conditions_file()

    def is_active(self):
        return True;

    def has_environmental_conditions_file(self):
        return self._environmental_conditions_file is not None
    def get_environmental_conditions_file(self):
        return self._environmental_conditions_file
    def set_environmental_conditions_file(self, filename: str):
        self._environmental_conditions_file = filename

    def has_environmental_conditions(self):
        return self._environmental_conditions is not None
    def get_environmental_conditions(self):
        if self._environmental_conditions is None:
            self._environmental_conditions = SEEnvironmentalConditions()
        return self._environmental_conditions
