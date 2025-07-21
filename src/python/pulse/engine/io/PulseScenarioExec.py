# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.engine.PulseScenarioExec import PulseScenarioExec

from pulse.cdm.enums import eSerializationFormat
from pulse.cdm.io.scenario import serialize_scenario_exec_to_bind
from pulse.engine.bind.Scenario_pb2 import ScenarioExecData
from google.protobuf import json_format

def serialize_pulse_scenario_exec_to_string(src: PulseScenarioExec, fmt: eSerializationFormat) -> str:
    dst = ScenarioExecData()
    serialize_pulse_scenario_exec_to_bind(src, dst)
    return json_format.MessageToJson(dst, True, True)

def serialize_pulse_scenario_exec_to_file(src: PulseScenarioExec, filename: str) -> None:
    string = serialize_pulse_scenario_exec_to_string(src, eSerializationFormat.JSON)
    file = open(filename, "w")
    n = file.write(string)
    file.close()

def serialize_pulse_scenario_exec_from_string(string: str, dst: PulseScenarioExec, fmt: eSerializationFormat) -> None:
    src = ScenarioExecData()
    json_format.parse(string, src)
    serialize_pulse_scenario_exec_from_bind(src, dst)

def serialize_pulse_scenario_exec_from_file(filename: str, dst: PulseScenarioExec) -> None:
    with open(filename) as f:
        string = f.read()
    serialize_pulse_scenario_exec_from_string(string, dst, eSerializationFormat.JSON)

def serialize_pulse_scenario_exec_to_bind(src: PulseScenarioExec, dst: ScenarioExecData) -> None:
    serialize_scenario_exec_to_bind(src, dst.ScenarioExec)
    dst.ModelType = src.get_model_type().value

def serialize_pulse_scenario_exec_from_bind(src: ScenarioExecData, dst: PulseScenarioExec):
    raise Exception("serialize_pulse_scenario_exec_from_bind not implemented")
