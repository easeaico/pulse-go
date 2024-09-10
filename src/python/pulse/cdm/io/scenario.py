# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from google.protobuf import json_format
from typing import List

from pulse.cdm.engine import eSerializationFormat
from pulse.cdm.scenario import SEScenario, SEScenarioExec, SEScenarioExecStatus, eScenarioExecutionState
from pulse.cdm.bind.Scenario_pb2 import ScenarioData, ScenarioExecData, ScenarioExecStatusData, \
                                        ScenarioExecStatusListData
from pulse.cdm.bind.Engine_pb2 import ActionListData

from pulse.cdm.io.engine import (
    serialize_actions_to_bind,
    serialize_data_request_manager_to_bind,
    serialize_patient_configuration_to_bind,
    serialize_engine_initialization_status_from_bind,
    serialize_engine_initialization_status_to_bind
)


def serialize_scenario_to_string(src: SEScenario, fmt: eSerializationFormat):
    dst = ScenarioData()
    serialize_scenario_to_bind(src, dst)
    return json_format.MessageToJson(dst, True, True)

def serialize_scenario_to_file(src: SEScenario, filename: str):
    string = serialize_scenario_to_string(src, eSerializationFormat.JSON)
    file = open(filename, "w")
    n = file.write(string)
    file.close()

def serialize_scenario_from_string(string: str, dst: SEScenario, fmt: eSerializationFormat):
    src = ScenarioData()
    json_format.Parse(string, src)
    serialize_scenario_from_bind(src,dst)

def serialize_scenario_from_file(filename: str, dst: SEScenario):
    with open(filename) as f:
        string = f.read()
    serialize_scenario_from_string(string, dst, eSerializationFormat.JSON)

def serialize_scenario_to_bind(src: SEScenario, dst: ScenarioData):
    if src.has_name():
        dst.Name = src.get_name()

    if src.has_description():
        dst.Description = src.get_description()

    if src.has_patient_configuration():
        serialize_patient_configuration_to_bind(src.get_patient_configuration(), dst.PatientConfiguration)
    elif src.has_engine_state():
        dst.EngineStateFile = src.get_engine_state()

    if src.get_data_request_manager().has_data_requests():
        serialize_data_request_manager_to_bind(src.get_data_request_manager(), dst.DataRequestManager)

    dst.DataRequestFile.extend(src.get_data_request_files())

    actionListData = ActionListData()
    serialize_actions_to_bind(src.get_actions(), actionListData)
    dst.AnyAction = actionListData.AnyAction

def serialize_scenario_from_bind(src: ScenarioData, dst: SEScenario):
    raise Exception("serialize_scenario_from_bind not implemented")


def serialize_scenario_exec_to_string(src: SEScenarioExec, fmt: eSerializationFormat) -> str:
    dst = ScenarioExecData()
    serialize_scenario_exec_to_bind(src, dst)
    return json_format.MessageToJson(dst, True, True)

def serialize_scenario_exec_to_file(src: SEScenarioExec, filename: str) -> None:
    string = serialize_scenario_exec_to_string(src, eSerializationFormat.JSON)
    file = open(filename, "w")
    n = file.write(string)
    file.close()

def serialize_scenario_exec_from_string(string: str, dst: SEScenarioExec, fmt: eSerializationFormat) -> None:
    src = ScenarioExecData()
    json_format.parse(string, src)
    serialize_scenario_exec_from_bind(src, dst)

def serialize_scenario_exec_from_file(filename: str, dst: SEScenarioExec) -> None:
    with open(filename) as f:
        string = f.read()
    serialize_scenario_exec_from_string(string, dst, eSerializationFormat.JSON)

def serialize_scenario_exec_to_bind(src: SEScenarioExec, dst: ScenarioExecData) -> None:
    dst.LogToConsole = src.get_log_to_console().value
    dst.DataRootDirectory = src.get_data_root_directory()
    dst.OutputRootDirectory = src.get_output_root_directory()
    dst.OrganizeOutputDirectory = src.get_organize_output_directory().value

    dst.AutoSerializeAfterActions = src.get_auto_serialize_after_actions().value
    dst.AutoSerializePeriod_s = src.get_auto_serialize_period_s()
    dst.TimeStampSerializedStates = src.get_time_stamp_serialized_states().value

    if src.get_engine_configuration_content():
        dst.EngineConfigurationContent = src.get_engine_configuration_content()
    elif src.get_engine_configuration_filename():
        dst.EngineConfigurationFilename = src.get_engine_configuration_filename()

    if src.get_scenario_content():
        dst.ScenarioContent = src.get_scenario_content()
    elif src.get_scenario_filename():
        dst.ScenarioFilename = src.get_scenario_filename()
    elif src.get_scenario_directory():
        dst.ScenarioDirectory = src.get_scenario_directory()
    elif src.get_scenario_exec_list_filename():
        dst.ScenarioExecListFilename = src.get_scenario_exec_list_filename()
    elif src.get_scenario_log_filename():
        dst.ScenarioLogFilename = src.get_scenario_log_filename()
    elif src.get_scenario_log_directory():
        dst.ScenarioLogDirectory = src.get_scenario_log_directory()

    dst.ContentFormat = src.get_content_format().value
    dst.ThreadCount = src.get_thread_count()

    dst.DataRequestFilesSearch[:] = src.get_data_request_files_search()

def serialize_scenario_exec_from_bind(src: ScenarioExecData, dst: SEScenarioExec):
    raise Exception("serialize_scenario_exec_from_bind not implemented")


def serialize_scenario_exec_status_to_bind(src: SEScenarioExecStatus, dst: ScenarioExecStatusData) -> None:
    serialize_engine_initialization_status_to_bind(src, dst.InitializationStatus)
    if src.has_scenario_filename():
        dst.ScenarioFilename = src.get_scenario_filename()
    dst.RuntimeError = src.has_runtime_error()
    dst.FatalRuntimeError = src.has_fatal_runtime_error()
    dst.FinalSimulationTime_s = src.get_final_simulation_time_s()

def serialize_scenario_exec_status_to_string(src: SEScenarioExecStatus, fmt: eSerializationFormat) -> str:
    dst = ScenarioExecStatusData()
    serialize_scenario_exec_status_to_bind(src, dst)
    return json_format.MessageToJson(dst, True, True)

def serialize_scenario_exec_status_to_file(src: SEScenarioExecStatus, filename: str) -> None:
    string = serialize_scenario_exec_status_to_string(src, eSerializationFormat.JSON)
    with open(filename, "w") as file:
        file.write(string)

def serialize_scenario_exec_status_from_bind(src: ScenarioExecStatusData, dst: SEScenarioExecStatus) -> None:
    serialize_engine_initialization_status_from_bind(src.InitializationStatus, dst)
    dst.set_scenario_filename(src.ScenarioFilename)
    dst.set_scenario_execution_state(eScenarioExecutionState(src.ScenarioExecutionState))
    dst.set_runtime_error(src.RuntimeError)
    dst.set_fatal_runtime_error(src.FatalRuntimeError)
    dst.set_final_simulation_time_s(src.FinalSimulationTime_s)

def serialize_scenario_exec_status_from_string(
    string: str,
    dst: SEScenarioExecStatus,
    fmt: eSerializationFormat
) -> None:
    src = ScenarioExecStatusData()
    json_format.parse(string, src)
    serialize_scenario_exec_status_from_bind(src, dst)

def serialize_scenario_exec_status_from_file(filename: str, dst: SEScenarioExecStatus) -> None:
    with open(filename) as f:
        string = f.read()
    serialize_scenario_exec_status_from_string(string, dst, eSerializationFormat.JSON)


def serialize_scenario_exec_status_list_to_bind(
    src: List[SEScenarioExecStatus],
    dst: ScenarioExecStatusListData
) -> None:
    for status in src:
        serialize_scenario_exec_status_to_bind(status, dst.ScenarioExecStatus.add())

def serialize_scenario_exec_status_list_to_string(
    src: List[SEScenarioExecStatus],
    fmt: eSerializationFormat
) -> str:
    dst = ScenarioExecStatusListData()
    serialize_scenario_exec_status_list_to_bind(src, dst)
    return json_format.MessageToJson(dst, True, True)

def serialize_scenario_exec_status_list_to_file(src: List[SEScenarioExecStatus], filename: str):
    string = serialize_scenario_exec_status_list_to_string(src, eSerializationFormat.JSON)
    with open(filename, "w") as file:
        file.write(string)

def serialize_scenario_exec_status_list_from_bind(
    src: ScenarioExecStatusListData,
    dst: List[SEScenarioExecStatus]
) -> None:
    for statusData in src.ScenarioExecStatus:
        dst.append(SEScenarioExecStatus())
        serialize_scenario_exec_status_from_bind(statusData, dst[-1])

def serialize_scenario_exec_status_list_from_string(
    string: str,
    dst: List[SEScenarioExecStatus],
    fmt: eSerializationFormat
) -> None:
    src = ScenarioExecStatusListData()
    json_format.Parse(string, src)
    serialize_scenario_exec_status_list_from_bind(src, dst)

def serialize_scenario_exec_status_list_from_file(filename: str, dst: List[SEScenarioExecStatus]):
    with open(filename) as f:
        string = f.read()
    serialize_scenario_exec_status_list_from_string(string, dst, eSerializationFormat.JSON)
