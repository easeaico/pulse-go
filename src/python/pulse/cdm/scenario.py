# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import re
import abc
import json
import logging
import numpy as np
import pandas as pd
from enum import Enum
from pathlib import Path
from typing import Any, Dict, Hashable, Iterable, List, NamedTuple, Optional, Set, Tuple

import PyPulse
from pulse.cdm.engine import eEngineInitializationState, eEvent, eSerializationFormat, eSwitch, SEAction, \
                             SEDataRequestManager, SEEngineInitializationStatus, SEEventChange
from pulse.cdm.patient import SEPatient, SEPatientConfiguration
from pulse.cdm.scalars import SEScalarTime, TimeUnit, get_unit
from pulse.cdm.io.patient import serialize_patient_from_string
from pulse.cdm.utils.csv_utils import read_csv_into_df


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



class SEScenarioLog:
    __slots__ = ("_log_file", "_patient", "_actions", "_actions_filter", "_events", "_events_filter",
                 "_extract_patient", "_extract_actions", "_extract_events")

    def __init__(
        self,
        log_file: Path,
        actions_filter: Optional[List[str]] = None,
        events_filter: Optional[List[str]] = None,
        extract_patient: bool = True,
        extract_actions: bool = True,
        extract_events: bool = True
    ):
        """
        Base class to parse log content. Inheriting classes should implement method(s) to
        use this extracted info.

        :param log_file: Path to log file
        :param actions_filter: Exclude actions containing any of these strings
        :param events_filter: Exclude events containing any of these strings
        """
        if not log_file.is_file():
            raise ValueError(f"Log file ({log_file}) does not exist/is not a file")
        self._log_file = log_file
        self._actions_filter = actions_filter
        self._events_filter = events_filter
        self._patient = SEPatient()
        self._actions = list()
        self._events = list()
        self._extract_patient = extract_patient
        self._extract_actions = extract_actions
        self._extract_events = extract_events

    def _process_log(self) -> None:
        """
        Extracts patient, actions, and events from log
        """

        with open(self._log_file) as f:
            lines = f.readlines()

            if self._extract_patient:
                self._parse_patient(lines)
            if self._extract_events:
                self._parse_events(lines)
            if self._extract_actions:
                self._parse_actions(lines)

    def _parse_patient(self, lines: List[str]) -> None:
        """
        Parse patient info from log.

        :param lines: Lines of text from log
        """
        patient_text = list()
        if not SEScenarioLog._parse_dict(tag="[Patient]", lines=lines, max_matches=1, matches_out=patient_text) or not patient_text:
            _pulse_logger.warning(f"Could not extract patient info: {self._log_file}")
        else:
            serialize_patient_from_string(patient_text[0][1], self._patient, eSerializationFormat.JSON)

    def _parse_events(self, lines: List[str]) -> None:
        """
        Parse events from log.

        :param lines: Lines of text from log
        """
        if not SEScenarioLog._parse_line(tag=r"\[Event\s*[^\]]*\]", lines=lines, matches_out=self._events):
            _pulse_logger.warning(f"Could not extract events: {self._log_file}")
        elif self._events_filter:
            self._events = [
                e
                for e in self._events if SEScenarioLog._filter(item=e[1], filter_out=self._events_filter)
            ]

    def _parse_actions(self, lines: List[str]) -> None:
        """
        Parse actions from log.

        :param lines: Lines of text from log
        """
        if not SEScenarioLog._parse_dict(tag="[Action]", lines=lines, matches_out=self._actions):
            _pulse_logger.warning(f"Could not extract actions: {self._log_file}")
        elif self._actions_filter:
            self._actions = [
                a
                for a in self._actions if SEScenarioLog._filter(item=a[1], filter_out=self._actions_filter)
            ]

    @staticmethod
    def _filter(item: str, filter_out: List[str]) -> bool:
        """
        :param item: Value to check
        :param filter_out: Values that given item shoud not contain

        :return: Whether given item passes given filters
        """
        for filter in filter_out:
            if filter in item:
                return False
        return True

    @staticmethod
    def _parse_dict(
        tag: str,
        lines: List[str],
        matches_out: List[Tuple[SEScalarTime, str]],
        max_matches: Optional[int]=None
    ) -> bool:
        """
        Parses dict-like structures following given tag

        :param tag: Tag identifying content to extract
        :param lines: Log text
        :param matches_out: List where matches will be added
        :param max_matches: If provided, limit the number of matches to this value

        :return: Whether parsing was successful
        """
        idx = 0
        while idx < len(lines):
            line = lines[idx]
            tag_idx = line.find(tag)
            if tag_idx != -1:
                tag_text = line
                # Extract time
                match = re.search(r'\[(?P<time>\d*\.?\d*)\((?P<time_unit>.*)\)\]', tag_text)
                if match is None:
                    return False
                tag_time = float(match['time'])
                time_unit = get_unit(match['time_unit'])
                tag_text = tag_text[(tag_idx+len(tag)):].lstrip()

                # Find blank line indicating end of tag match
                while (idx + 1) < len(lines) and len(lines[idx+1].strip()) != 0:
                    idx += 1
                    line = lines[idx]
                    tag_text += line

                matches_out.append((SEScalarTime(tag_time, time_unit), tag_text))
                if max_matches is not None and len(matches_out) >= max_matches:
                    break
            idx += 1
        return True

    @staticmethod
    def _parse_line(
        tag: str,
        lines: List[str],
        matches_out: List[Tuple[SEScalarTime, str]],
        max_matches: Optional[int]=None
    ):
        """
        Parses single line containing given tag

        :param tag: Tag identifying content to extract. If regex, should already be escaped.
        :param lines: Log text
        :param matches_out: List where matches will be added
        :param max_matches: If provided, limit the number of matches to this value

        :return: Whether parsing was successful
        """
        for line in lines:
            tag_search = re.search(tag, line)
            if tag_search is not None:
                tag_idx = tag_search.start()
                tag_text = line
                # Get time and find beginning of actual tag text content
                match = re.search(
                    r'\[(?P<time>\d*\.?\d*)\((?P<time_unit>.*)\)\]\s*(?P<tag>' +
                        tag +
                        r'\s*)',
                    tag_text
                )
                if match is None:
                    return False
                tag_time = float(match['time'])
                time_unit = get_unit(match['time_unit'])
                tag_text = tag_text[tag_idx:].strip()

                matches_out.append((SEScalarTime(tag_time, time_unit), tag_text))
                if max_matches is not None and len(matches_out) >= max_matches:
                    break

        return True


class SEReportModule(metaclass=abc.ABCMeta):
    __slots__ = ("_headers")

    def __init__(self):
        self._headers = []

    def required_headers(self) -> List[str]:
        return self._headers

    def handle_event(self, change: SEEventChange) -> None:
        """ Process given event change """

    def handle_action(self, action: str, action_time: SEScalarTime) -> None:
        """ Process action """

    @abc.abstractmethod
    def update(self, data_slice: NamedTuple, slice_idx: Dict[str, int]) -> Any:
        """
        Process given data.

        :param data_slice: Single timestep of data
        :param slice_idx: Maps headers to tuple index for faster look-up
        """


class SEObservationReportModule(SEReportModule):
    """
    Report module that will only be updated when an observation is due.
    """
    @abc.abstractmethod
    def update(self, data_slice: NamedTuple, slice_idx: Dict[str, int]) -> Iterable[Tuple[Hashable, Any]]:
        """ Process given data and report results """

    def __call__(self, data_slice: NamedTuple, slice_idx: Dict[str, int]) -> Iterable[Tuple[Hashable, Any]]:
        return self.update(data_slice=data_slice, slice_idx=slice_idx)


class SETimestepReportModule(SEReportModule):
    """
    Report module that will be updated every timestep, but only provides results when report is called.
    """
    def update(self, data_slice: NamedTuple, slice_idx: Dict[str, int]) -> None:
        """ Process given data """

    def __call__(self, data_slice: NamedTuple, slice_idx: Dict[str, int]) -> None:
        return self.update(data_slice=data_slice, slice_idx=slice_idx)

    @abc.abstractmethod
    def report(self) -> Iterable[Tuple[Hashable, Any]]:
        """ Report results """


class SEScenarioReport(SEScenarioLog):
    __slots__ = ("_csv_file", "_df", "_death_check_module", "_observation_frequency_s", "_observation_modules",
                 "_timestep_modules", "_time_header")

    def __init__(
        self,
        log_file: Path,
        csv_file: Path,
        observation_frequency_min: float,
        observation_modules: Optional[List[SEObservationReportModule]] = None,
        timestep_modules: Optional[List[SETimestepReportModule]] = None,
        death_check_module: Optional[SETimestepReportModule] = None,
        actions_filter: Optional[List[str]] = None,
        events_filter: Optional[List[str]] = None
    ):
        """
        Generates summary results report from given results CSV and log file.

        :param log_file: Path to log file
        :param csv_file: Path to csv file containing scenario results
        :param observation_frequency_s: Frequency to include observations in report
        :param actions_filter: Exclude actions containing any of these strings
        :param events_filter: Exclude events containing any of these strings
        """
        super().__init__(
            log_file=log_file,
            actions_filter=actions_filter,
            events_filter=events_filter
        )

        self._time_header = "Time(s)"

        if observation_frequency_min <= 0:
            raise ValueError(f"Observation frequency must be positive, got {observation_frequency_min}")
        self._observation_frequency_s = SEScalarTime(observation_frequency_min, TimeUnit.min).get_value(TimeUnit.s)

        if not csv_file.is_file():
            raise ValueError(f"CSV file ({csv_file}) does not exist/is not a file")
        self._csv_file = csv_file

        self._observation_modules = observation_modules if observation_modules is not None else list()
        self._timestep_modules = timestep_modules if timestep_modules is not None else list()
        self._death_check_module = death_check_module

        self._df = None
        self._prepare_df()

        self._process_log()

    def _parse_events(self, lines: List[str]) -> None:
        """
        Generate SEEventChanges from events parsed in super method.
        """
        super()._parse_events(lines)

        events_out = list()
        for time, event in self._events:
            # Determine event name and active status
            # Note: using re.match here instead of re.search because we expect the match at index 0
            match = re.match(r'\[Event\s*(?P<event>\D*)(?P<active>\d)\]', event, re.IGNORECASE)
            if match is None:
                _pulse_logger.warning(f"Could not identify if event is active, ignoring: {event}")
                continue
            events_out.append((
                time,
                SEEventChange(
                    event=eEvent[match["event"].strip()],
                    active=bool(int(match["active"])),
                    sim_time_s=time.get_value(TimeUnit.s)
                )
            ))

        self._events = events_out

    def _required_headers(self) -> Set[str]:
        """
        Generate complete set of headers required for this report.

        :return: Set of required headers.
        """
        out = set([self._time_header])
        for module in self._timestep_modules:
            out.update(module.required_headers())
        for module in self._observation_modules:
            out.update(module.required_headers())
        if self._death_check_module is not None:
            out.update(self._death_check_module.required_headers())

        return out

    def _prepare_df(self) -> None:
        """
        Loads dataframe from CSV and filters down to only needed columns,
        converting units where needed.
        """
        all_headers = self._required_headers()

        # Filter dataframe to only required headers
        df = read_csv_into_df(self._csv_file)
        self._df = pd.DataFrame()
        for header in all_headers:
            if header not in df.columns:
                # Attempt to locate header with different unit and convert
                resolved = False
                paren_idx = header.find("(")
                if paren_idx != -1:
                    unitless_header = header[:(paren_idx+1)]
                    desired_unit = header[(paren_idx+1):-1].replace("_", " ")
                    for h in df.columns:
                        if h.startswith(unitless_header):
                            results_paren_idx = h.find("(")
                            if results_paren_idx != -1:
                                results_unit = h[(results_paren_idx+1):-1].replace("_", " ")
                                self._df[header] = df[h].map(lambda x: PyPulse.convert(x, results_unit, desired_unit))
                                resolved = True
                                break
                if not resolved:
                    raise ValueError(f"Missing required and/or requested header: {header}")
            else:
                self._df[header] = df[header]

    def _initialize_report(self) -> Dict:
        """
        Initialize the report dict with any initial datapoints.

        :return: Initialized report dict
        """
        return dict()

    def generate(self) -> Dict:
        """
        Generates report content.

        :return: Dict representing report.
        """
        out = self._initialize_report()

        # Determine time step
        # Note: Assumes time step is not variable
        times_s = self._df[self._time_header].tolist()
        timestep_s = 0.02
        if len(times_s) > 1:
            timestep_s = times_s[1] - times_s[0]

        # Compute observation times
        observation_times_s = np.arange(
            start=times_s[0],
            stop=times_s[-1] + self._observation_frequency_s,
            step=self._observation_frequency_s
        ).tolist()

        # Snap observation times to times that exist in data
        if timestep_s !=0:
            observation_times_s = [
                np.floor( (requested_time_s / timestep_s) + 0.5 ) * timestep_s
                for requested_time_s in observation_times_s
            ]

        def _generate_observation() -> Dict:
            observation = dict()
            observation["SimTime_min"] = SEScalarTime(time_s, TimeUnit.s).get_value(TimeUnit.min)
            for module in self._observation_modules:
                observation.update(module(data_slice=data_slice, slice_idx=idx))
            return observation

        observations = list()
        idx = {name: i for i, name in enumerate(list(self._df), start=0)}  # for faster named tuple look-up
        event_idx = 0
        action_idx = 0
        for data_slice in self._df.itertuples(index=False, name="DataSlice"):  # itertuples is faster than iterrows
            time_s = data_slice[idx[self._time_header]]

            # Send event changes to each module
            for next_event_idx, event_info in enumerate(self._events[event_idx:]):
                event_time, change = event_info
                if event_time.get_value(TimeUnit.s) == time_s:  # TODO: Floating point errors?
                    for module in self._timestep_modules:
                        module.handle_event(change)
                    for module in self._observation_modules:
                        module.handle_event(change)
                    if self._death_check_module is not None:
                        self._death_check_module.handle_event(change)
                    event_idx = next_event_idx + 1

            # Send action changes to each module
            for next_action_idx, action_info in enumerate(self._actions[action_idx:]):
                action_time, action = action_info
                if action_time.get_value(TimeUnit.s) == time_s:  # TODO: Floating point errors?
                    for module in self._timestep_modules:
                        module.handle_action(action=action, action_time=action_time)
                    for module in self._observation_modules:
                        module.handle_action(action=action, action_time=action_time)
                    if self._death_check_module is not None:
                        self._death_check_module.handle_action(action=action, action_time=action_time)
                    action_idx = action_idx + 1

            # Send data to each module for processing
            # TODO: Update data for timestep in which death is indicated?
            for module in self._timestep_modules:
                module(data_slice=data_slice, slice_idx=idx)
            try:
                if self._death_check_module is not None:
                    self._death_check_module(data_slice=data_slice, slice_idx=idx)
            except StopIteration:  # Death indication: stop iterating, do one last observation
                # observations.append(_generate_observation())
                break

            # If this is an observation time, update those modules
            if time_s in observation_times_s:  # TODO: Floating point errors?
               observations.append(_generate_observation())

        # Get final results from timestep modules
        for module in self._timestep_modules:
            out.update(module.report())
        if self._death_check_module is not None:
            out.update(self._death_check_module.report())

        out["Observations"] = observations
        return out

    def write(self, out_file: Path) -> None:
        """
        Generates and writes report.

        :param out_file: JSON filepath
        """
        out_file.parent.mkdir(parents=True, exist_ok=True)

        with open(out_file, "w") as f:
            f.write(json.dumps(self.generate(), indent=2))
