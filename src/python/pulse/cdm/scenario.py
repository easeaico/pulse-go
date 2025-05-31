# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import logging
from enum import Enum
from typing import Dict, List

from pulse.cdm.patient import SEPatientConfiguration
from pulse.cdm.engine import eEngineInitializationState, eSerializationFormat, eSwitch, SEAction, \
                             SEDataRequestManager, SEEngineInitializationStatus


_pulse_logger = logging.getLogger('pulse')


class SEScenario:
    __slots__ = ["_name", "_description", "_params", "_engine_state_file", "_dr_mgr", "_dr_files", "_actions"]

    def __init__(self):
        self._dr_mgr = SEDataRequestManager()
        self.clear()

    def __repr__(self):
        return f'SEScenario({self._name}, {self._description}, {self._params}, {self._engine_state_file}, ' \
            f'{self._dr_mgr}, {self._dr_files}, {self._actions})'

    def __str__(self):
        return f'SEScenario:\n\tName: {self._name}\n\tDescription: {self._description}\n\tPatient Configuration: ' \
            f'{self._params}\n\tEngine State File: {self._engine_state_file}\n\tData Request Manager: {self._dr_mgr}\n\t' \
            f'Data Request Files: {self._dr_files}\n\tActions: {self._actions}'

    def clear(self) -> None:
        self._name = ""
        self._description = ""
        self._params = None
        self._engine_state_file = None
        self._dr_mgr.clear()
        self._dr_files = []
        self._actions = []

    def is_valid(self) -> bool:
        if len(self._actions) == 0:
            return False
        if not self.has_patient_configuration() and not self.has_engine_state():
            return False
        return True

    def get_name(self) -> str:
        return self._name
    def set_name(self, name: str) -> None:
        self._name = name
    def has_name(self) -> bool:
        return len(self._name) > 0
    def invalidate_name(self) -> None:
        self._name = ""

    def get_description(self) -> str:
        return self._description
    def set_description(self, description: str) -> None:
        self._description = description
    def has_description(self) -> bool:
        return len(self._description) > 0
    def invalidate_description(self) -> None:
        self._description = ""

    def get_engine_state(self) -> str:
        return self._engine_state_file
    def set_engine_state(self, state_file: str) -> None:
        self._engine_state_file = state_file
    def has_engine_state(self) -> bool:
        if self.has_patient_configuration():
            return False
        return self._engine_state_file is not None and len(self._engine_state_file) > 0
    def invalidate_engine_state(self) -> None:
        self._engine_state_file = None

    def get_patient_configuration(self) -> SEPatientConfiguration:
        if self._params is None:
            self._params = SEPatientConfiguration()
        return self._params
    def has_patient_configuration(self) -> bool:
        return self._params is not None and self._params.is_valid()
    def invalidate_patient_configuration(self) -> None:
        self._params = None

    def get_actions(self) -> List[SEAction]:
        return self._actions

    def get_data_request_manager(self) -> SEDataRequestManager:
        return self._dr_mgr

    def get_data_request_files(self) -> List[str]:
        return self._dr_files


class SEScenarioExec:
    __slots__ = ["_log_to_console", "_data_root_directory", "_output_root_directory", "_organize_output_directory",
                 "_auto_serialize_after_actions", "_auto_serialize_period_s", "_time_stamp_serialized_states",
                 "_engine_configuration_content", "_engine_configuration_filename", "_scenario_content", "_scenario_filename",
                 "_scenario_directory", "_scenario_exec_list_filename", "_scenario_log_filename", "_scenario_log_directory",
                 "_data_request_files_search", "_content_format", "_thread_count"]

    def __init__(self):
        self.clear()

    def clear(self) -> None:
        self._log_to_console = eSwitch.On
        self._data_root_directory = "./"
        self._output_root_directory = "./"
        self._organize_output_directory = eSwitch.Off

        self._auto_serialize_after_actions = eSwitch.Off
        self._auto_serialize_period_s = 0
        self._time_stamp_serialized_states = eSwitch.On

        self._engine_configuration_content = ""
        self._engine_configuration_filename = ""

        self._scenario_content = ""
        self._scenario_filename = ""
        self._scenario_directory = ""
        self._scenario_exec_list_filename = ""
        self._scenario_log_filename = ""
        self._scenario_log_directory = ""

        self._data_request_files_search = []

        self._content_format = eSerializationFormat.JSON
        self._thread_count = -1

    def get_log_to_console(self) -> eSwitch:
        return self._log_to_console
    def set_log_to_console(self, s: eSwitch) -> None:
        self._log_to_console = s

    def get_data_root_directory(self) -> str:
        return self._data_root_directory
    def set_data_root_directory(self, s: str) -> None:
        self._data_root_directory = s

    def get_output_root_directory(self) -> str:
        return self._output_root_directory
    def set_output_root_directory(self, s: str) -> None:
        self._output_root_directory = s

    def get_organize_output_directory(self) -> eSwitch:
        return self._organize_output_directory
    def set_organize_output_directory(self, s: eSwitch) -> None:
        self._organize_output_directory = s

    def get_auto_serialize_after_actions(self) -> eSwitch:
        return self._auto_serialize_after_actions
    def set_auto_serialize_after_actions(self, s: eSwitch) -> None:
        self._auto_serialize_after_actions = s

    def get_auto_serialize_period_s(self) -> float:
        return self._auto_serialize_period_s
    def set_auto_serialize_period_s(self, p: float) -> None:
        self._auto_serialize_period_s = p

    def get_time_stamp_serialized_states(self) -> eSwitch:
        return self._time_stamp_serialized_states
    def set_time_stamp_serialized_states(self, s: eSwitch) -> None:
        self._time_stamp_serialized_states = s

    def get_engine_configuration_content(self) -> str:
        return self._engine_configuration_content
    def set_engine_configuration_content(self, s: str) -> None:
        self._engine_configuration_content = s
        self._engine_configuration_filename = ""

    def get_engine_configuration_filename(self) -> str:
        return self._engine_configuration_filename
    def set_engine_configuration_filename(self, s: str) -> None:
        self._engine_configuration_content = ""
        self._engine_configuration_filename = s

    def get_scenario_content(self) -> str:
        return self._scenario_content
    def set_scenario_content(self, s: str) -> None:
        self._scenario_content = s
        self._scenario_filename = ""
        self._scenario_directory = ""
        self._scenario_exec_list_filename = ""
        self._scenario_log_filename = ""
        self._scenario_log_directory = ""

    def get_scenario_filename(self) -> str:
        return self._scenario_filename
    def set_scenario_filename(self, s: str) -> None:
        self._scenario_content = ""
        self._scenario_filename = s
        self._scenario_directory = ""
        self._scenario_exec_list_filename = ""
        self._scenario_log_filename = ""
        self._scenario_log_directory = ""

    def get_scenario_directory(self) -> str:
        return self._scenario_directory
    def set_scenario_directory(self, s: str) -> None:
        self._scenario_content = ""
        self._scenario_filename = ""
        self._scenario_directory = s
        self._scenario_exec_list_filename = ""
        self._scenario_log_filename = ""
        self._scenario_log_directory = ""

    def get_scenario_exec_list_filename(self) -> str:
        return self._scenario_exec_list_filename
    def set_scenario_exec_list_filename(self, s: str) -> None:
        self._scenario_content = ""
        self._scenario_filename = ""
        self._scenario_directory = ""
        self._scenario_exec_list_filename = s
        self._scenario_log_filename = ""
        self._scenario_log_directory = ""

    def get_scenario_log_filename(self) -> str:
        return self._scenario_log_filename
    def set_scenario_log_filename(self, s: str) -> None:
        self._scenario_content = ""
        self._scenario_filename = ""
        self._scenario_directory = ""
        self._scenario_exec_list_filename = ""
        self._scenario_log_filename = s
        self._scenario_log_directory = ""

    def get_scenario_log_directory(self) -> str:
        return self._scenario_log_directory
    def set_scenario_log_directory(self, s: str) -> None:
        self._scenario_content = ""
        self._scenario_filename = ""
        self._scenario_directory = ""
        self._scenario_exec_list_filename = ""
        self._scenario_log_filename = ""
        self._scenario_log_directory = s

    def get_data_request_files_search(self) -> List[str]:
        return self._data_request_files_search

    def get_content_format(self) -> eSerializationFormat:
        return self._content_format
    def set_content_format(self, s: eSerializationFormat) -> None:
        self._content_format = s

    def get_thread_count(self) -> int:
        return self._thread_count
    def set_thread_count(self, c: int) -> None:
        self._thread_count = c


class eScenarioExecutionState(Enum):
    Waiting = 0
    Running = 1
    Complete = 2

class SEScenarioExecStatus(SEEngineInitializationStatus):
    __slots__ = ("_scenario_filename", "_scenario_execution_state",
                 "_runtime_error", "_fatal_runtime_error",
                 "_final_simulation_time_s")

    def __init__(self):
        self.clear()

    def __str__(self) -> str:
        return f'SEScenarioExecStatus:\n\tScenario Filename: {self._scenario_filename}\n\tScenario Execution State: ' \
            f'{self._scenario_execution_state}\n\tRuntime Error: {self._runtime_error}\n\tFatal Runtime Error: ' \
            f'{self._fatal_runtime_error}\n\tFinal Simulation Time (s): {self._final_simulation_time_s}'

    def clear(self) -> None:
        super().clear()
        self._scenario_filename = ""
        self._scenario_execution_state = eScenarioExecutionState.Waiting
        self._runtime_error = False
        self._fatal_runtime_error = False
        self._final_simulation_time_s = 0.

    def copy(self, other: "SEScenarioExecStatus") -> None:
        super().copy()
        self._scenario_filename = other._scenario_filename
        self._scenario_execution_state = other._scenario_execution_state
        self._final_simulation_time_s = other._final_simulation_time_s
        self._runtime_error = other._runtime_error
        self._fatal_runtime_error = other._fatal_runtime_error

    def has_scenario_filename(self) -> bool:
        return bool(self._scenario_filename)
    def get_scenario_filename(self) -> str:
        return self._scenario_filename
    def set_scenario_filename(self, fn: str) -> None:
        self._scenario_filename = fn

    def get_scenario_execution_state(self) -> eScenarioExecutionState:
        return self._scenario_execution_state
    def set_scenario_execution_state(self, s: eScenarioExecutionState) -> None:
        self._scenario_execution_state = s

    def has_runtime_error(self) -> bool:
        return self._runtime_error
    def set_runtime_error(self, e: bool) -> None:
        self._runtime_error = e

    def has_fatal_runtime_error(self) -> bool:
        return self._fatal_runtime_error
    def set_fatal_runtime_error(self, e: bool) -> None:
        self._fatal_runtime_error = e

    def get_final_simulation_time_s(self) -> float:
        return self._final_simulation_time_s
    def set_final_simulation_time_s(self, t: float) -> None:
        self._final_simulation_time_s = t

    @staticmethod
    def summarize_exec_status_list(lst: List["SEScenarioExecStatus"]) -> Dict[str, int]:
        # Count everything
        summary = {
            "Total": 0,
            "Valid": 0,
            "FailedState": 0,
            "FailedSetup": 0,
            "FailedStabilization": 0,
            "RuntimeError": 0,
            "FatalRuntimeError": 0
        }
        for status in lst:
            if status.get_initialization_state() == eEngineInitializationState.FailedState:
                summary["FailedState"] += 1
            elif status.get_initialization_state() == eEngineInitializationState.FailedPatientSetup:
                summary["FailedSetup"] += 1
            elif status.get_initialization_state() == eEngineInitializationState.FailedStabilization:
                summary["FailedStabilization"] += 1
            elif status.get_scenario_execution_state() == eScenarioExecutionState.Complete and status.has_fatal_runtime_error():
                summary["FatalRuntimeError"] += 1
            elif status.get_scenario_execution_state() == eScenarioExecutionState.Complete and status.has_runtime_error():
                summary["RuntimeError"] += 1
            elif status.get_scenario_execution_state() == eScenarioExecutionState.Complete:
                summary["Valid"] += 1
            else:
                continue

            summary["Total"] += 1

        return summary
