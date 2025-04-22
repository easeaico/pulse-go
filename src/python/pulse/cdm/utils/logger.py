# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import re
import json
import logging
from enum import Enum
from json import JSONDecodeError
from typing import List

from pulse.cdm.engine import eSerializationFormat, eEvent
from pulse.cdm.patient import SEPatient
from pulse.cdm.io.patient import serialize_patient_from_string


_pulse_logger = logging.getLogger('pulse')


def get_severity_str(severity: float) -> str:
    """
    :param severity: The severity value.

    :return: Severity as a qualitative string.
    """
    if severity == 0:
        raise ValueError("Severity string undefined for 0 severity")
    elif severity <= 0.3:
        severity_str = "Mild"
    elif severity <= 0.6:
        severity_str = "Moderate"
    else:
        severity_str = "Severe"

    return severity_str


def break_camel_case(string: str):
    # https://stackoverflow.com/a/9283563
    camel_case_regex = r"""
        (            # start the group
            # alternative 1
        (?<=[a-z])  # current position is preceded by a lower char
                    # (positive lookbehind: does not consume any char)
        [A-Z]       # an upper char
                    #
        |   # or
            # alternative 2
        (?<!\A)     # current position is not at the beginning of the string
                    # (negative lookbehind: does not consume any char)
        [A-Z]       # an upper char
        (?=[a-z])   # matches if next char is a lower char
                    # lookahead assertion: does not consume any char
        )           # end the group"""

    return re.sub(camel_case_regex, r' \1', string, flags=re.VERBOSE)


class ePrettyPrintType(Enum):
    Action = 0
    Condition = 1


def pretty_print(string: str, print_type: ePrettyPrintType, preserve_camel_case: bool = False):
    ret = ""
    string = string.replace('"', '')
    string = string.replace('{', '')
    string = string.replace('}', '')
    string = string.replace(',', '')
    string = string.replace('[', '')
    string = string.replace(']', '')

    lines = [s.rstrip() for s in string.splitlines() if s]

    idx = 0
    while idx < len(lines):
        line = lines[idx].rstrip()

        if len(line) == 0:
            idx += 1
            continue
        if print_type.name in line:
            idx += 1
            continue
        if "ReadOnly" in line:
            idx += 1
            continue
        if line.endswith("Comment:"):
            idx += 1
            continue

        found_unit = False
        if idx+1 < len(lines):
            peek = lines[idx+1]
            if "Scalar" in peek:
                peek = lines[idx+2]
                line = ''.join([line, peek[(peek.find("Value")+5):].rstrip()])
                if (idx+3) < len(lines) and "Unit:" in lines[idx+3]:
                    idx += 3
                    peek = lines[idx]
                    line = ''.join([line, peek[(peek.find("Unit:")+5):].rstrip()])
                    found_unit = True
                else:
                    idx += 2

        if not preserve_camel_case and "Comment:" not in line:
            # Don't change files or units
            if found_unit or "Unit:" in line or "File" in line:
                colon = line.rfind(":")
                if colon != -1:
                    line = break_camel_case(line[:colon]) + line[colon:]
            else:
                line = break_camel_case(line)

        ret = ''.join([ret, line, "\n"])
        idx += 1

    ret = ret.replace('::', ':')

    return ret


class LogItem:
    __slots__ = ["time", "text"]

    def __init__(self, time: float, text: str):
        self.time = time
        self.text = text


class LogAction(LogItem):
    __slots__ = ["name"]

    def __init__(self, time: float, name: str, text: str):
        super().__init__(time, text)
        self.name = name

    def __repr__(self):
        return f"{self.time}: {self.name}"


class LogEvent(LogItem):
    __slots__ = ["event", "active"]

    def __init__(self, time: float, event: eEvent, text: str, active: bool):
        super().__init__(time, text)
        self.event = event
        self.active = active

    def __repr__(self):
        return f"{self.time}: {self.text}"


# TODO We should gather up all these methods into a Log class, so we only need to process a file once

def parse_timing(log_file: str):
    start = 0
    end = 99999

    def pull_time():
        s = line.rfind(']') + 1
        e = line.find("(s)", s)
        return float(line[s:e].strip())

    with open(log_file) as f:
        lines = f.readlines()
        idx = 0
        while idx < len(lines):
            line = lines[idx]
            if len(line) == 0:
                idx += 1
                continue
            if "[Initial SimTime(s)]" in line:
                start = pull_time()
            elif "[Final SimTime(s)]" in line:
                end = pull_time()
            idx += 1
    return start, end


def parse_actions(log_file: str, omit: List[str] = []):
    patient_action = "PatientAction"
    enviro_action = "EnvironmentAction"
    equip_action = "EquipmentAction"
    adv_stable = "AdvanceUntilStable"
    adv_time = "AdvanceTime"
    serialize_requested = "SerializeRequested"
    serialize_state = "SerializeState"

    omit.append(adv_time)
    action_tag = "[Action]"
    actions = []
    with open(log_file) as f:
        lines = f.readlines()
        idx = 0
        while idx < len(lines):
            line = lines[idx]
            if len(line) == 0:
                idx += 1
                continue
            action_idx = line.find(action_tag)
            if action_idx == -1:
                idx += 1
                continue
            elif adv_time in line:
                idx += 1
                continue
            else:
                action_text = line
                # Group 0: Entire match
                # Group 1: Time
                match = re.search(r'\[(\d*\.?\d*)\(.*\)\]', action_text)
                if match is None:
                    _pulse_logger.error("Could not parse actions from " + str(log_file))
                    return actions
                action_time = float(match.group(1))
                action_text = action_text[(action_idx+len(action_tag)):].lstrip()

                # Find blank line at end of action
                while (idx + 1) < len(lines) and len(lines[idx+1].strip()) != 0:
                    idx += 1
                    line = lines[idx]
                    action_text = ''.join([action_text, line])

                # Attempt to determine action name
                try:
                    action_data = json.loads(action_text)
                except JSONDecodeError:
                    _pulse_logger.error("Could not parse actions from " + str(log_file))
                    return actions

                if adv_time in action_data:
                    action_name = adv_time
                elif adv_stable in action_data:
                    action_name = adv_stable
                elif serialize_requested in action_data:
                    action_name = serialize_requested
                elif serialize_state in action_data:
                    action_name = serialize_state
                elif patient_action in action_data:
                    action_name = list(action_data[patient_action].keys())[0]
                elif enviro_action in action_data:
                    action_name = list(action_data[enviro_action].keys())[0]
                elif equip_action in action_data:
                    action_name = list(action_data[equip_action].keys())[0]
                else:
                    _pulse_logger.warning(f"Unable to determine action name: {action_text}")
                    action_name = action_data.keys()[0]

                # Check to see if it should be omitted
                keep_action = True
                for o in omit:
                    if o in action_text:
                        keep_action = False
                        break
                if not keep_action:
                    idx += 1
                    continue

                action_text = pretty_print(action_text, ePrettyPrintType.Action)

                # Remove leading spaces on each line
                action_text = '\n'.join([s.strip() for s in action_text.splitlines()])

                actions.append(LogAction(time=action_time, name=action_name, text=action_text))

            idx += 1

    return actions


def parse_events(log_file: str, omit: List[str] = []):
    # event_tag = "[Event"
    events = []
    with open(log_file) as f:
        lines = f.readlines()
        for line in lines:
            if len(line) == 0:
                continue
            if "[Initial SimTime(s)]" in line:
                stabilization_events = events
                events = []
                # Clean up events triggered during stabilization
                # Remove any inactive events we found
                # Reset all event times we have found to 0
                for se in stabilization_events:
                    if se.event == eEvent.Stabilizing:
                        continue
                    if se.active:
                        se.time = 0
                        events.append(se)
                    else:
                        for i, e in enumerate(events):
                            if e.event == se.event:
                                del events[i]
                                break
                continue
            match = re.search(
                r"\[(?P<time_val>\d+.?\d*)\(.*\)\]\s*\[Event(?P<event_name>.*)(?P<active>[01])\](?P<event_text>.*)",
                line
            )
            if match is None:
                continue
            text = match.group("event_text").strip()
            name = match.group("event_name").strip()
            time = float(match.group("time_val"))
            active = True if match.group("active").strip() == '1' else False

            # Check to see if it should be omitted
            keep_event = True
            for o in omit:
                if o in text:
                    keep_event = False
                    break
            if not keep_event:
                continue

            events.append(LogEvent(time=time, event=eEvent.from_str(name), text=text, active=active))

    return events


def active_event_windows(events: [eEvent],
                         engine_start: float,
                         engine_end: float):

    windows = {}
    for e in events:
        if e.active:
            if e.event not in windows:
                windows[e.event] = []
            # Assume the event will be active through the duration of the simulation
            windows[e.event].append((e.time, engine_end))
        else:  # Not active
            if e.event not in windows:
                # So the event was on the whole time?
                # It would be better if you concat logs from time 0, so you can catch all event activations
                windows[e.event] = [(engine_start, e.time)]
            else:
                # Set a more specific time for this event
                w = windows[e.event][-1]
                windows[e.event][-1] = (w[0], e.time)

    return windows


def active_events(active_windows: {},
                  window_start: float,
                  window_end: float):

    events = {}
    for e, actives in active_windows.items():
        if e not in events:
            events[e] = {"Duration_s": 0.0, "ActiveFraction": 0.0, "FinalState": False}
        for active in actives:
            if active[0] < window_start:
                start = window_start
            elif window_start <= active[0] <= window_end:
                start = active[0]
            else:
                continue

            end = window_end
            if active[1] > window_end:
                end = window_end
            elif window_end <= active[1]:
                end = active[1]

            events[e]["Duration_s"] += (end - start)
            if end == window_end:
                events[e]["FinalState"] = True
        events[e]["ActiveFraction"] = events[e]["Duration_s"] / (window_end - window_start)

    return events


def parse_active_event_windows(log_file: str):
    start, end = parse_timing(log_file)
    events = parse_events(log_file)
    return active_event_windows(events, engine_start=start, engine_end=end)


def parse_patient(log_file: str):
    patient_text = ""
    patient_tag = "[Patient]"
    with open(log_file) as f:
        lines = f.readlines()
        idx = 0
        while idx < len(lines):
            line = lines[idx]
            if len(line) == 0:
                idx += 1
                continue
            patient_idx = line.find(patient_tag)
            if patient_idx == -1:
                idx += 1
                continue
            else:
                # Find blank line at end of action
                while (idx + 1) < len(lines) and len(lines[idx+1].strip()) != 0:
                    idx += 1
                    line = lines[idx]
                    patient_text = ''.join([patient_text, line])
                patient = SEPatient()
                serialize_patient_from_string(patient_text, patient, eSerializationFormat.JSON)
                return patient
    # No patient found in log...
    return None
