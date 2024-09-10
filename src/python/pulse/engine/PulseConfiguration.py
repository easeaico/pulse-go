# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.engine import eSwitch
from pulse.cdm.scalars import SEScalarTime

class PulseConfiguration():
    __slots__ = ["_time_step", "_allow_dynamic_timestep"]

    def __init__(self):
        self._allow_dynamic_timestep = eSwitch.NullSwitch
        self._time_step = None

    def clear(self):
        self._allow_dynamic_timestep = eSwitch.NullSwitch
        if self._time_step is not None: self._time_step.invalidate()

    def set_allow_dynamic_timestep(self, s: eSwitch):
        self._allow_dynamic_timestep = s
    def allow_dynamic_timestep(self):
        return self._allow_dynamic_timestep

    def has_time_step(self):
        return False if self._time_step is None else self._time_step.is_valid()
    def get_time_step(self):
        if self._time_step is None:
            self._time_step = SEScalarTime()
        return self._time_step

