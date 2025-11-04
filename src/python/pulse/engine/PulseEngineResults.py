# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import re
import abc
import json
import logging

from json import JSONDecodeError
from pathlib import Path
from typing import Dict, List, NamedTuple, Optional, Set

from pulse.cdm.enums import eSerializationFormat, eSwitch
from pulse.cdm.engine import SEEventChange, eEvent
from pulse.cdm.patient import SEPatient
from pulse.cdm.scalars import SEScalarTime, TimeUnit
from pulse.cdm.utils.csv_utils import read_csv_into_df, concat_dataframes
from pulse.cdm.utils.logger import pretty_print, ePrettyPrintType
from pulse.cdm.io.patient import serialize_patient_from_string


_pulse_logger = logging.getLogger('pulse')


class PulseLogAction:
    __slots__ = ["time", "text", "name", "data"]

    def __init__(self, time: float, name: str, text: str, data: dict):
        self.time = time
        self.text = text
        self.name = name
        self.data = data

    def __repr__(self):
        return f"{self.time}: {self.name}"


class PulseLog:
    __slots__ = ["_start_time_s", "_end_time_s",
                 "_patient", "_actions",
                 "_events", "_event_windows", "_event_changes"]

    def __init__(self,
                 log_files: List[Path],
                 actions_filter: Optional[List[str]] = None,
                 events_filter: Optional[List[str]] = None):

        self._start_time_s = 0.0
        self._end_time_s = 999999.0
        self._patient = None
        self._actions = {}
        self._events = {}
        self._event_windows = None

        self._parse(log_files, actions_filter, events_filter)

    @property
    def start_time_s(self): return self._start_time_s

    @property
    def end_time_s(self): return self._end_time_s

    @property
    def patient(self): return self._patient

    @property
    def actions(self): return self._actions

    @property
    def events(self): return self._events

    @property
    def event_windows(self): return self._event_windows

    @property
    def event_changes(self): return self._event_changes

    def _parse(self,
               log_files: List[Path],
               omit_actions: Optional[List[str]] = None,
               omit_events: Optional[List[str]] = None):

        def pull_time():
            _s = line.rfind(']') + 1
            _e = line.find("(s)", _s)
            return float(line[_s:_e].strip())

        for log_file in log_files:

            if not log_file.exists():
                _pulse_logger.error(f"Unable to parse log file: {log_file}")
                return False

            patient_action = "PatientAction"
            enviro_action = "EnvironmentAction"
            equip_action = "EquipmentAction"
            adv_stable = "AdvanceUntilStable"
            adv_time = "AdvanceTime"
            serialize_requested = "SerializeRequested"
            serialize_state = "SerializeState"
            action_tag = "[Action]"
            if not omit_actions:
                omit_actions = []
            omit_actions.append(adv_time)

            if not omit_events:
                omit_events = []

            with open(log_file) as f:
                lines = f.readlines()
                idx = 0
                while idx < len(lines):
                    line = lines[idx]
                    if len(line) == 0 or line == "\n":
                        idx += 1
                        continue

                    # Look for patient
                    patient_idx = line.find("[Patient]")
                    if patient_idx > -1:
                        # Find blank line at end of action
                        patient_text = ""
                        while (idx + 1) < len(lines) and len(lines[idx + 1].strip()) != 0:
                            idx += 1
                            line = lines[idx]
                            patient_text = ''.join([patient_text, line])
                        self._patient = SEPatient()
                        serialize_patient_from_string(patient_text, self._patient, eSerializationFormat.JSON)
                        idx += 1
                        continue

                    # Look for start/end times
                    if "[Initial SimTime(s)]" in line:
                        self._start_time_s = pull_time()
                        # Remove stabilization events from our event list
                        stabilization_events = self._events
                        self._events = {}
                        # Clean up events triggered during stabilization
                        # Remove any inactive events we found
                        # Reset all event times we have found to 0
                        zero = 0.0
                        for time, events in stabilization_events.items():
                            for se in events:
                                if se.event == eEvent.Stabilizing:
                                    continue
                                if se.active:
                                    se.sim_time.set_value(zero, TimeUnit.s)
                                    if zero not in self._events:
                                        self._events[zero] = []
                                    self._events[zero].append(se)
                                else:  # Event went from active to inactive in stabilization, so take it out
                                    for i, e in enumerate(self._events[zero]):
                                        if e.event == se.event:
                                            del self._events[zero][i]
                                            break

                        idx += 1
                        continue
                    elif "[Final SimTime(s)]" in line:
                        self._end_time_s = pull_time()
                        idx += 1
                        continue

                    # Look for actions
                    action_idx = line.find(action_tag)
                    if action_idx > -1 and adv_time not in line:
                        action_text = line
                        # Group 0: Entire match
                        # Group 1: Time
                        match = re.search(r'\[(\d*\.?\d*)\(.*\)\]', action_text)
                        if match is None:
                            _pulse_logger.error(f"Could not match action text from {log_file} : {action_text}")
                            idx += 1
                            continue
                        action_time = float(match.group(1))
                        action_text = action_text[(action_idx + len(action_tag)):].lstrip()

                        # Find blank line at end of action
                        while (idx + 1) < len(lines) and len(lines[idx + 1].strip()) != 0:
                            idx += 1
                            line = lines[idx]
                            if '[I' in line:
                                # This should not be here, something corrupted the log
                                # If you want to use [ ] in a comment, we'd need to strengthen this logic
                                _pulse_logger.error("Action json is corrupted")
                                break
                            action_text = ''.join([action_text, line])

                        # Attempt to determine action name
                        try:
                            action_dict = json.loads(action_text)
                        except JSONDecodeError as ex:
                            _pulse_logger.error(f"Could not parse action from {log_file} : {action_text}")
                            _pulse_logger.error(f"{ex}")
                            idx += 1
                            continue

                        if adv_time in action_dict:
                            action_name = adv_time
                        elif adv_stable in action_dict:
                            action_name = adv_stable
                        elif serialize_requested in action_dict:
                            action_name = serialize_requested
                        elif serialize_state in action_dict:
                            action_name = serialize_state
                        elif patient_action in action_dict:
                            action_name = list(action_dict[patient_action].keys())[0]
                        elif enviro_action in action_dict:
                            action_name = list(action_dict[enviro_action].keys())[0]
                        elif equip_action in action_dict:
                            action_name = list(action_dict[equip_action].keys())[0]
                        else:
                            _pulse_logger.warning(f"Unable to determine action name: {action_text}")
                            action_name = action_dict.keys()[0]

                        # Check to see if it should be omitted
                        keep_action = True
                        for o in omit_actions:
                            if o in action_text:
                                keep_action = False
                                break
                        if keep_action:
                            action_text = pretty_print(action_text, ePrettyPrintType.Action)
                            # Remove leading spaces on each line
                            action_text = '\n'.join([s.strip() for s in action_text.splitlines()])
                            if action_time not in self._actions:
                                self._actions[action_time] = []
                            self._actions[action_time].append(PulseLogAction(time=action_time, name=action_name,
                                                                             text=action_text, data=action_dict))
                        idx += 1
                        continue

                    # Look for events
                    match = re.search(
                        r"\[(?P<time_val>\d+.?\d*)\(.*\)\]\s*\[Event(?P<event_name>.*)(?P<active>[01])\](?P<event_text>.*)",
                        line)
                    if match:
                        text = match.group("event_text").strip()
                        name = match.group("event_name").strip()
                        time = float(match.group("time_val"))
                        active = True if match.group("active").strip() == '1' else False

                        # Check to see if it should be omitted
                        keep_event = True
                        for o in omit_events:
                            if o in text:
                                keep_event = False
                                break
                        if keep_event:
                            if time not in self._events:
                                self._events[time] = []
                            self._events[time].append(SEEventChange(event=eEvent.from_str(name),
                                                                    active=active,
                                                                    sim_time_s=time))
                        idx += 1
                        continue

                    idx += 1

        # Sort the events into windows
        self._event_windows = {}
        for time, events in self._events.items():
            for e in events:
                if e.active:
                    if e.event not in self._event_windows:
                        self._event_windows[e.event] = []
                    # Assume the event will be active through the duration of the simulation
                    self._event_windows[e.event].append((time, self._end_time_s))
                else:  # Not active
                    if e.event not in self._event_windows:
                        # So the event was on the whole time?
                        # It would be better if you concat logs from time 0, so you can catch all event activations
                        self._event_windows[e.event] = [(self.start_time_s, time)]
                    else:
                        # Set a more specific time for this event
                        w = self._event_windows[e.event][-1]
                        self._event_windows[e.event][-1] = (w[0], time)

        return True

    def get_active_events_in_window(self, window_start_s: float, window_end_s: float):

        events = {None: {"Duration_s": 0.0, "ActiveFraction": 0.0, "FinalState": False}}
        for e, actives in self._event_windows.items():
            if e not in events:
                events[e] = {"Duration_s": 0.0, "ActiveFraction": 0.0, "FinalState": False}
            for active in actives:
                if active[0] < window_start_s:
                    start = window_start_s
                elif window_start_s <= active[0] <= window_end_s:
                    start = active[0]
                else:
                    continue

                end = window_end_s
                if active[1] > window_end_s:
                    end = window_end_s
                elif active[1] < window_end_s:
                    end = active[1]

                events[e]["Duration_s"] += (end - start)
                if end == window_end_s:
                    events[e]["FinalState"] = True
            events[e]["ActiveFraction"] = events[e]["Duration_s"] / (window_end_s - window_start_s)

        return events

    def get_event_status(self, event: eEvent, time_s: float) -> eSwitch:
        for e, actives in self._event_windows.items():
            if e == event:
                for window in actives:
                    if window[0] <= time_s <= window[1]:
                        return eSwitch.On
        return eSwitch.Off


class PulseResultsProcessor(metaclass=abc.ABCMeta):
    __slots__ = ["_headers"]

    def __init__(self):
        self._headers = []

    def required_headers(self) -> List[str]:
        return self._headers

    @abc.abstractmethod
    def process_time_step(self,
                          data_slice: NamedTuple,
                          header_idx: Dict[str, int],
                          event_changes: List[SEEventChange],
                          action_changes: List[PulseLogAction]) -> None:
        """
        Process given data.

        :param data_slice: Single timestep of data
        :param header_idx: Maps headers to tuple index for faster look-up
        :param event_changes: Event changes associated with this time step
        :param action_changes: Action changes associated with this time step
        """


class PulseEngineReprocessor(PulseLog):
    """
    Replay a Pulse simulation
    """
    __slots__ = ("_df", "_modules", "_time_header")

    def __init__(
        self,
        csv_files: List[Path],
        log_files: List[Path],
        actions_filter: Optional[List[str]] = None,
        events_filter: Optional[List[str]] = None
    ):
        """
        Replays a simulation for custom modules to postprocess the results.

        :param log_files: Path to log files to concatenate together
        :param csv_files: Path to csv files containing scenario results to concatenate
        :param actions_filter: Exclude actions containing any of these strings
        :param events_filter: Exclude events containing any of these strings
        """
        super().__init__(
            log_files=log_files,
            actions_filter=actions_filter,
            events_filter=events_filter
        )
        self._time_header = "Time(s)"

        self._df = None
        self._prepare_df(csv_files)

    def _required_headers(self) -> Set[str]:
        """
        Generate complete set of headers required for this report.

        :return: Set of required headers.
        """
        out = {self._time_header}
        for module in self._modules:
            out.update(module.required_headers())
        return out

    def _prepare_df(self, csv_files: List[Path]) -> None:
        """
        Loads dataframe from CSV files and filters down to only needed columns,
        converting units where needed.
        """

        df = None
        for csv_file in csv_files:
            if not csv_file.is_file():
                raise ValueError(f"CSV file ({csv_file}) does not exist/is not a file")
            if not df:
                df = read_csv_into_df(csv_file)
            else:
                df = concat_dataframes(df, read_csv_into_df(csv_file))

        self._df = df

    def get_values_at_time(self, time_s: float):
        # time_s does not need to be in the dataframe
        # Find the nearest time in the data frame to time_s
        for index, row in self._df.iterrows():
            if row[0] >= time_s:
                return row.values.tolist()
        _pulse_logger.error(f"Could not find time {time_s}")
        return []

    def replay(self, modules: List[PulseResultsProcessor]):
        stop = False

        """
        TODO 
        # Filter dataframe to only required headers
        all_headers = self._required_headers()
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
        """

        event_times = []
        event_changes = []
        events = self._events.copy()

        action_times = []
        action_changes = []
        actions = self._actions.copy()

        idx = {name: i for i, name in enumerate(list(self._df), start=0)}  # for faster named tuple look-up
        for data_slice in self._df.itertuples(index=False, name="DataSlice"):  # itertuples is faster than iterrows
            time_s = data_slice[idx[self._time_header]]

            # Send data to each module for processing

            # The data frame times can be at a different sampling rate than the events and actions
            # So grab any events or action sets on this time or that we might have passed
            for et, e in events.items():
                if time_s >= et:
                    event_times.append(et)
                    event_changes.extend(e)
            for et in event_times:
                del events[et]

            for at, a in actions.items():
                if time_s >= at:
                    action_times.append(at)
                    action_changes.extend(a)
            for at in action_times:
                del actions[at]

            for module in modules:
                try:
                    module.process_time_step(data_slice=data_slice, header_idx=idx,
                                             event_changes=event_changes, action_changes=action_changes)
                except StopIteration:
                    stop = True
            if stop:
                _pulse_logger.info("StopIteration received, stopping early")
                break

            event_times.clear()
            event_changes.clear()

            action_times.clear()
            action_changes.clear()
