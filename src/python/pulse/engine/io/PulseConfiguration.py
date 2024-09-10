# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.engine import eSwitch, eSerializationFormat
from pulse.cdm.io.scalars import *
from pulse.engine.PulseConfiguration import PulseConfiguration
from pulse.engine.bind.Configuration_pb2 import ConfigurationData
from google.protobuf import json_format


def serialize_pulse_configuration_to_string(src: PulseConfiguration, fmt: eSerializationFormat) -> str:
    dst = ConfigurationData()
    serialize_pulse_configuration_to_bind(src, dst)
    return json_format.MessageToJson(dst, True, True)


def serialize_pulse_configuration_to_file(src: PulseConfiguration, filename: str) -> None:
    string = serialize_pulse_configuration_to_string(src, eSerializationFormat.JSON)
    file = open(filename, "w")
    n = file.write(string)
    file.close()


def serialize_pulse_configuration_from_string(string: str, dst: PulseConfiguration, fmt: eSerializationFormat) -> None:
    src = ConfigurationData()
    json_format.parse(string, src)
    serialize_pulse_configuration_from_bind(src, dst)


def serialize_pulse_configuration_from_file(filename: str, dst: PulseConfiguration) -> None:
    with open(filename) as f:
        string = f.read()
    serialize_pulse_configuration_from_string(string, dst, eSerializationFormat.JSON)


def serialize_pulse_configuration_to_bind(src: PulseConfiguration, dst: ConfigurationData) -> None:
    if src.allow_dynamic_timestep() is not eSwitch.NullSwitch:
        dst.AllowDynamicTimeStep = src.allow_dynamic_timestep().value
    if src.has_time_step():
        serialize_scalar_time_to_bind(src.get_time_step(), dst.TimeStep)


def serialize_pulse_configuration_from_bind(src: ConfigurationData, dst: PulseConfiguration):
    raise Exception("serialize_pulse_configuration_from_bind not implemented")