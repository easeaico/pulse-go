# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import logging

from pulse.cdm.scenario import SEScenarioExec
from pulse.engine.PulseEngine import eModelType, eSerializationFormat


_pulse_logger = logging.getLogger('pulse')


class PulseScenarioExec(SEScenarioExec):
    __slots__ = ["_model_type"]

    def __init__(self):
        super().__init__()
        self._model_type = eModelType.HumanAdultWholeBody

    def clear(self) -> None:
        super().clear()
        self._model_type = eModelType.HumanAdultWholeBody

    def get_model_type(self) -> eModelType:
        return self._model_type

    def set_model_type(self, t: eModelType) -> None:
        self._model_type = t

    def execute_scenario(self) -> bool:
        from pulse.engine.io.PulseScenarioExec import serialize_pulse_scenario_exec_to_string
        import PyPulse

        json = serialize_pulse_scenario_exec_to_string(self, eSerializationFormat.JSON)
        return PyPulse.execute_scenario(json, PyPulse.serialization_format.json)
