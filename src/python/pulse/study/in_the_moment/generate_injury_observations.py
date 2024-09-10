# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import re
import sys
import json
import logging
import faulthandler
from enum import Enum
from pathlib import Path
from multiprocessing import Pool
from typing import Any, Dict, Hashable, Iterable, List, NamedTuple, Optional, Tuple

from pulse.cdm.engine import eEvent, SEEventChange
from pulse.cdm.patient import eSex
from pulse.cdm.scalars import TimeUnit, FrequencyUnit, SEScalarFrequency, SEScalarTime
from pulse.cdm.scenario import SEScenarioExecStatus, SEScenarioReport, \
                               SEObservationReportModule, SETimestepReportModule
from pulse.cdm.utils.logger import break_camel_case, get_severity_str
from pulse.cdm.io.scenario import serialize_scenario_exec_status_list_from_file


_pulse_logger = logging.getLogger('pulse')


class AVPU(str, Enum):
    Alert = "Alert"
    Voice = "Voice"
    Pain = "Pain"
    Unresponsive = "Unresponsive"


class TriageTag(str, Enum):
    Black = "Black"
    Red = "Red"
    Yellow = "Yellow"
    Green = "Green"



class ITMObservationModule(SEObservationReportModule):
    """
    Reports requested vitals at the observation timestep.
    """

    __slots__ = ("_active_events", "_active_injuries",
                 # Pulse physiology data
                 "_hr_bpm", "_rr_bpm", "_map_mmHg", "_sbp_mmHg", "_dbp_mmHg", "_spO2", "_ppi",
                 "_core_temp_C", "_skin_temp_C", "_brainO2pp",
                 # Function of pulse physiology data and/or action(s)
                 "_airway_maneuverable", "_ambulatory", "_avpu", "_conscious",
                 "_crt_s", "_gcs", "_healthy_capillary_refill",
                 "_radial_pulse_present", "_skin_touch", "_skin_color"
                 )

    def __init__(self):
        super().__init__()

        self._headers = \
            [
                "HeartRate(1/min)", "RespirationRate(1/min)", "MeanArterialPressure(mmHg)",
                "SystolicArterialPressure(mmHg)", "DiastolicArterialPressure(mmHg)", "OxygenSaturation",
                "PeripheralPerfusionIndex", "CoreTemperature(degC)", "SkinTemperature(degC)",
                "BrainVasculature-Oxygen-PartialPressure(mmHg)"
            ]
        self._hr_bpm = None
        self._rr_bpm = None
        self._map_mmHg = None
        self._sbp_mmHg = None
        self._dbp_mmHg = None
        self._spO2 = None
        self._ppi = None
        self._core_temp_C = None
        self._skin_temp_C = None
        self._brainO2pp = None

        self._airway_maneuverable = True
        self._ambulatory = True
        self._avpu = AVPU.Alert
        self._conscious = True
        self._crt_s = 1
        self._gcs = 15
        self._healthy_capillary_refill = True
        self._radial_pulse_present = True
        self._skin_touch = None
        self._skin_color = None

        self._active_events = list()
        self._active_injuries = dict()

    def handle_event(self, change: SEEventChange) -> None:
        """
        Add/remove event from active events.
        """
        if change.active:
            self._active_events.append(change.event.name)
        else:
            if change.event.name in self._active_events:
                self._active_events.remove(change.event.name)
            else:
                _pulse_logger.warning(f"Could not find corresponding active abnormality: {change.event.name}")

    def handle_action(self, action: str, action_time: SEScalarTime) -> None:
        super().handle_action(action=action, action_time=action_time)

        # Load into dict direct from JSON because we can't serialize actions from bind
        action_data = json.loads(action)

        action_type = "PatientAction"
        if action_type in action_data:
            action_name = list(action_data[action_type].keys())[0]
        else:
            return  # We are only working with patient actions

        severity = 0
        if "Severity" in action_data[action_type][action_name]:
            insult = True
            severity = action_data[action_type][action_name]["Severity"]["Scalar0To1"]["Value"]

        # Action specific logic

        if action_name == "BrainInjury":
            if severity == 0:
                self._gcs = 15
                self._conscious = True
                self._avpu = AVPU.Alert
            elif severity <= 0.1:
                self._gcs = 14
                self._conscious = True
                self._avpu = AVPU.Alert
            elif severity <= 0.2:
                self._gcs = 14
                self._conscious = True
                self._avpu = AVPU.Alert
            elif severity <= 0.3:
                self._gcs = 13
                self._conscious = True
                self._avpu = AVPU.Alert
            elif severity <= 0.4:
                self._gcs = 12
                self._conscious = True
                self._avpu = AVPU.Voice
            elif severity <= 0.5:
                self._gcs = 10
                self._conscious = True
                self._avpu = AVPU.Voice
            elif severity <= 0.6:
                self._gcs = 9
                self._conscious = True
                self._avpu = AVPU.Voice
            elif severity <= 0.7:
                self._gcs = 8
                self._conscious = False
                self._avpu = AVPU.Pain
            elif severity <= 0.8:
                self._gcs = 6
                self._conscious = False
                self._avpu = AVPU.Pain
            elif severity <= 0.9:
                self._gcs = 4
                self._conscious = False
                self._avpu = AVPU.Unresponsive
            else:
                self._gcs = 3
                self._conscious = False
                self._avpu = AVPU.Unresponsive
            self._active_injuries[break_camel_case(action_name).strip()] = severity

        elif action_name == "Hemorrhage":
            action_description = f'{action_data[action_type][action_name]["Compartment"]}' \
                                 f'{action_data[action_type][action_name]["Type"]}' \
                                 f'{action_name}'
            self._active_injuries[break_camel_case(action_description).strip()] = severity

        elif action_name == "AirwayObstruction":
            if severity >= 0.89:
                self._airway_maneuverable = False
            self._active_injuries[break_camel_case(action_name).strip()] = severity

        if action_name == "TensionPneumothorax":
            action_description = f'{action_data[action_type][action_name]["Side"]}' \
                                 f'{action_data[action_type][action_name]["Type"]}' \
                                 f'{action_name}'
            self._active_injuries[break_camel_case(action_description).strip()] = severity

        # Test for ambulatory conditions
        if self._gcs <= 10:
            self._ambulatory = False
        if severity > 0.59:
            self._ambulatory = False

    def update(self, data_slice: NamedTuple, slice_idx: Dict[str, int]) -> Iterable[Tuple[Hashable, Any]]:
        observations = list()

        pulse_data = dict()
        # Pulse data we are interested in
        self._hr_bpm = data_slice[slice_idx[self._headers[0]]]
        self._rr_bpm = data_slice[slice_idx[self._headers[1]]]
        self._map_mmHg = data_slice[slice_idx[self._headers[2]]]
        self._sbp_mmHg = data_slice[slice_idx[self._headers[3]]]
        self._dbp_mmHg = data_slice[slice_idx[self._headers[4]]]
        self._spO2 = data_slice[slice_idx[self._headers[5]]]
        self._ppi = data_slice[slice_idx[self._headers[6]]]
        self._core_temp_C = data_slice[slice_idx[self._headers[7]]]
        self._skin_temp_C = data_slice[slice_idx[self._headers[8]]]
        self._brainO2pp = data_slice[slice_idx[self._headers[9]]]
        pulse_data["HR_bpm"] = self._hr_bpm
        pulse_data["RR_bpm"] = self._rr_bpm
        pulse_data["MAP_mmmHg"] = self._map_mmHg
        pulse_data["SBP_mmHg"] = self._sbp_mmHg
        pulse_data["DBP_mmHg"] = self._dbp_mmHg
        pulse_data["SpO2"] = self._spO2
        pulse_data["PPI"] = self._ppi
        pulse_data["CoreTemp_C"] = self._core_temp_C
        pulse_data["SkinTemp_C"] = self._skin_temp_C
        pulse_data["BO2PP_mmHg"] = self._brainO2pp
        pulse_data["Events"] = self._active_events[:]  # Return a copy of list
        actions = list()
        for key, value in self._active_injuries.items():
            actions.append(f"{key} {value}")
        pulse_data["Actions"] = actions
        observations.append(("Pulse", pulse_data))

        # Via Kyle
        if self._brainO2pp < 29:
            self._ambulatory = False
            self._avpu = AVPU.Unresponsive
            self._conscious = False

        triage_data = dict()
        # Calculate Triage Data Using Sigle (https://pubmed.ncbi.nlm.nih.gov/37318826/)
        self._skin_color = "normal"
        if 0 < self._rr_bpm <= 9:
            self._skin_color = "cyanotic"
        elif self._rr_bpm == 0:
            self._skin_color = "grey"

        # TODO: Use skin temp directly?
        self._skin_touch = "hot"
        if 35 <= self._core_temp_C < 39:
            self._skin_touch = "normal"
        elif 32 <= self._core_temp_C < 35:
            self._skin_touch = "cold/pale"
        elif self._core_temp_C < 32:
            self._skin_touch = "cold"

        self._crt_s = 1
        if 80 < self._sbp_mmHg <= 100:
            self._crt_s = 2
            self._radial_pulse_present = True
            self._healthy_capillary_refill = True
        elif 60 <= self._sbp_mmHg <= 80:
            self._crt_s = 3
            self._radial_pulse_present = False
            self._healthy_capillary_refill = False
        elif self._sbp_mmHg < 60:
            # This was a random value between 4-10 in the original paper
            # Set to 7 for deterministic behavior
            # Global seeding prevents randomness of unstructured text
            self._crt_s = 7
            self._radial_pulse_present = False
            self._healthy_capillary_refill = False
        # End Sigle

        """
        We could also use Naylor (https://academic.oup.com/milmed/article/185/11-12/e1903/5880630)
        if self._sbp_mmHg < 85.1:
            self._radial_pulse_present = False
        if self._ppi < 0.003:
            self._healthy_capillary_refill = False
        """

        triage_data["Ambulatory"] = self._ambulatory
        triage_data["AVPU"] = self._avpu
        triage_data["Conscious"] = self._conscious
        triage_data["GCS"] = self._gcs
        triage_data["CRT_s"] = self._crt_s
        triage_data["SkinColor"] = self._skin_color
        triage_data["SkinTouch"] = self._skin_touch
        triage_data["RadialPulsePresent"] = self._radial_pulse_present
        triage_data["HealthyCapillaryRefillTime"] = self._healthy_capillary_refill
        triage_data["START"] = self._start_triage()
        triage_data["BCD"] = self._bcd_triage()
        triage_data["WEBB"] = self._webb_triage()
        observations.append(("Triage", triage_data))

        observations.append(("Vitals", self._itm_vital_level_elements()))
        observations.append(("Injuries", self._itm_injury_level_elements()))

        return observations

    def _max_severity(self):
        max_severity = 0
        for action, severity in self._active_injuries.items():
            if severity > max_severity:
                max_severity = severity
        return max_severity

    def _max_hemorrhage(self):
        max_severity = 0
        for action, severity in self._active_injuries.items():
            if "Hemorrhage" in action and severity > max_severity:
                max_severity = severity
        return max_severity

    def _bcd_triage(self) -> dict:
        """
        Computes BCD Sieve Triage category.
        """

        triage_status = None
        if self._max_hemorrhage() >= 0.6:  # TODO: What determines a catastrophic hemorrhage?
            triage_status = TriageTag.Red
        elif self._ambulatory:
            triage_status = TriageTag.Green
        elif self._rr_bpm == 0:  # Breathing?
            triage_status = TriageTag.Black
        elif self._gcs <= 8:  # Responds to voice?
            triage_status = TriageTag.Red
        elif (  # Abnormal breathing rate
            self._rr_bpm < 12 or
            self._rr_bpm > 23
        ):
            triage_status = TriageTag.Red
        elif self._hr_bpm > 100:
            triage_status = TriageTag.Red
        else:
            triage_status = TriageTag.Yellow

        return {"Tag": triage_status}

    def _start_triage(self) -> Iterable[Tuple[Hashable, Any]]:
        """
        Computes START Triage category.
        """
        observations = dict()

        if self._ambulatory:
            observations["Tag"] = TriageTag.Green
            observations["Description"] = "Patient is able to walk to the designated safety area."
            return observations

        description = ""
        triage_status = None

        if self._conscious:
            description = "The patient is conscious. "
        else:
            description = "The patient is unconscious. "

        if self._rr_bpm < 12:
            if self._rr_bpm <= 3:
                if self._airway_maneuverable:
                    description += "Patient was not breathing. "
                    description += "By repositioning the airway, the patient is now breathing again. "
                    triage_status = TriageTag.Red if triage_status is None else triage_status
                else:
                    observations["Tag"] = TriageTag.Black
                    observations["Description"] = \
                        "Patient is not breathing. Repositioning the airway did not help."
                    return observations
            else:
                description += "Patient in breathing slowly. "
        elif self._rr_bpm >= 30:
            description += "Patient is breathing too fast, respiration rate looks greater than 30. "
            triage_status = TriageTag.Red if triage_status is None else triage_status
        elif 18 >= self._rr_bpm >= 12:
            description += "Patient is breathing normally. "
        else:
            description += "Patient is breathing fast, respiration rate looks be less than 30. "

        # TODO how to handle this...
        # if self._hemorrhage_type is not None:
        #    description += f"Patient has a {self._hemorrhage_type} hemorrhage. "
        #    description += "You have used a tourniquet to stop the bleeding. "

        if self._sbp_mmHg < 85.1:
            triage_status = TriageTag.Red if triage_status is None else triage_status
            description += "No radial pulse can be detected on the patient. "
        else:
            if self._hr_bpm > 100:
                description += "Patient pulse is abnormally high. "
            elif self._hr_bpm < 60:
                description += "Patient pulse is abnormally slow. "
            else:
                description += "Patient pulse is within the normal range. "

        # PPI is serving as a replacement for capillary refill time (CRT)
        if self._ppi < 0.003:
            triage_status = TriageTag.Red if triage_status is None else triage_status
            description += "Capillary refill time is more than 3 seconds. "

        if self._conscious and (self._avpu == AVPU.Alert or self._avpu == AVPU.Voice):
            triage_status = TriageTag.Yellow if triage_status is None else triage_status
            description += "The patient is able to follow basic commands."
        else:
            triage_status = TriageTag.Red if triage_status is None else triage_status
            description += "The patient is not able to follow basic commands."

        observations["Tag"] = triage_status
        observations["Description"] = description
        return observations

    def _salt_triage(self) -> dict:
        pass

    def _webb_triage(self) -> dict:
        triage_status = None
        observations = dict()
        if self._rr_bpm == 0:
            triage_status = TriageTag.Black
        elif self._rr_bpm > 30:
            triage_status = TriageTag.Red
        elif self._sbp_mmHg < 85.1:
            triage_status = TriageTag.Red
        elif self._ppi < 0.003:
            triage_status = TriageTag.Red
        elif self._brainO2pp < 19:
            triage_status = TriageTag.Red
        elif self._spO2 < 0.92:
            triage_status = TriageTag.Yellow
        else:
            triage_status = TriageTag.Green

        observations["Tag"] = triage_status
        return observations

    def _itm_vital_level_elements(self) -> dict:
        elements = dict()
        elements["conscious"] = self._conscious
        elements["avpu"] = self._avpu
        elements["ambulatory"] = self._ambulatory

        max_injury_severity = 0
        if (not self._conscious) or (self._avpu == AVPU.Pain) or (self._avpu == AVPU.Unresponsive):
            elements["mental_status"] = "UNRESPONSIVE"
        elif max_injury_severity < 0.2:
            elements["mental_status"] = "CALM"
        elif 0.2 >= max_injury_severity < 0.4:
            elements["mental_status"] = "UPSET"
        elif 0.4 >= max_injury_severity < 0.6:
            elements["mental_status"] = "CONFUSED"
        elif 0.6 >= max_injury_severity < 0.8:
            elements["mental_status"] = "AGONY"
        else:
            elements["mental_status"] = "SHOCK"

        if self._rr_bpm == 0:
            elements["breathing"] = "NONE"
        elif self._rr_bpm < 12:
            elements["breathing"] = "RESTRICTED"
        elif 12 >= self._hr_bpm <= 20:
            elements["breathing"] = "NORMAL"
        elif self._hr_bpm > 20:
            elements["breathing"] = "FAST"

        if not self._radial_pulse_present:
            elements["heart_rate"] = "NONE"
        elif self._hr_bpm < 60:
            elements["heart_rate"] = "RESTRICTED"
        elif 60 >= self._hr_bpm <= 100:
            elements["heart_rate"] = "NORMAL"
        elif self._hr_bpm > 100:
            elements["heart_rate"] = "FAST"

        elements["Spo2"] = int(self._spO2*100)
        return elements

    def _itm_injury_level_elements(self) -> list:
        injuries = list()
        for action, severity in self._active_injuries.items():
            injury = dict()
            if "Airway" in action:
                # TODO maybe Shrapnel or Laceration
                # TODO Suggest support for Penetrating

                if severity <= 0.2:
                    injury["name"] = "Shrapnel"
                    injury["location"] = "face"
                    injury["severity"] = "minor"
                    injury["status"] = "visible"
                elif severity <= 0.4:
                    injury["name"] = "Shrapnel"
                    injury["location"] = "face"
                    injury["severity"] = "moderate"
                    injury["status"] = "visible"
                elif severity <= 0.6:
                    injury["name"] = "Shrapnel"
                    injury["location"] = "neck"
                    injury["severity"] = "substantial"
                    injury["status"] = "visible"
                elif severity <= 0.8:
                    injury["name"] = "Shrapnel"
                    injury["location"] = "neck"
                    injury["severity"] = "major"
                    injury["status"] = "visible"
                else:
                    injury["name"] = "Shrapnel"
                    injury["location"] = "neck"
                    injury["severity"] = "extreme"
                    injury["status"] = "visible"

            elif "Hemorrhage" in action:
                # TODO Add more location information
                if "Left Leg" in action:
                    injury["location"] = "left thigh"
                elif "Right Leg" in action:
                    injury["location"] = "right thigh"
                elif "Right Arm" in action:
                    injury["location"] = "right bicep"
                elif "Right Arm" in action:
                    injury["location"] = "right bicep"

                if severity <= 0.2:
                    injury["name"] = "Laceration"
                    injury["severity"] = "minor"
                    injury["status"] = "discoverable"
                elif severity <= 0.4:
                    injury["name"] = "Laceration"
                    injury["severity"] = "moderate"
                    injury["status"] = "visible"
                elif severity <= 0.6:
                    injury["name"] = "Laceration"
                    injury["severity"] = "substantial"
                    injury["status"] = "visible"
                elif severity <= 0.8:
                    injury["name"] = "Laceration"
                    injury["severity"] = "major"
                    injury["status"] = "visible"
                else:
                    injury["name"] = "Laceration"
                    injury["severity"] = "extreme"
                    injury["status"] = "visible"

            elif "Pneumothorax" in action:
                if "Left" in action:
                    injury["location"] = "left chest"
                else:
                    injury["location"] = "right chest"

                if severity <= 0.2:
                    injury["name"] = "Chest Collapse"
                    injury["severity"] = "minor"
                    injury["status"] = "hidden"
                elif severity <= 0.4:
                    injury["name"] = "Chest Collapse"
                    injury["severity"] = "moderate"
                    injury["status"] = "discoverable"
                elif severity <= 0.6:
                    injury["name"] = "Chest Collapse"
                    injury["severity"] = "substantial"
                    injury["status"] = "visible"
                elif severity <= 0.8:
                    injury["name"] = "Chest Collapse"
                    injury["severity"] = "major"
                    injury["status"] = "visible"
                else:
                    injury["name"] = "Chest Collapse"
                    injury["severity"] = "extreme"
                    injury["status"] = "visible"

            injuries.append(injury)
        return injuries



class TCCCDeathCheckModule(SETimestepReportModule):
    """
    Reports if patient survives. Raises StopIteration on update if patient death detected.
    """

    __slots__ = ("_irreversible_state", "_cardiovascular_collapse", "_brain_O2_deficit",
                 "_start_brain_O2_deficit_s", "_myocardium_O2_deficit", "_start_myocardium_O2_deficit_s",
                 "_spO2_deficit", "_start_spO2_deficit_s", "_max_hr_bpm", "_survived")

    TIME_s = "Time(s)"
    HR_bpm = "HeartRate(1/min)"
    SPO2 = "OxygenSaturation"
    SAP_mmHg = "SystolicArterialPressure(mmHg)"

    def __init__(self):
        super().__init__()
        self._headers = [
            self.TIME_s,
            self.HR_bpm,
            self.SPO2,
            self.SAP_mmHg
        ]
        self._irreversible_state = False
        self._cardiovascular_collapse = False
        self._brain_O2_deficit = False
        self._start_brain_O2_deficit_s = 0.
        self._myocardium_O2_deficit = False
        self._start_myocardium_O2_deficit_s = 0.
        self._spO2_deficit = False
        self._start_spO2_deficit_s = 0.
        self._max_hr_bpm = None
        self._survived = True

    def set_max_hr(self, max_hr: SEScalarFrequency) -> None:
        self._max_hr_bpm = max_hr.get_value(FrequencyUnit.Per_min)

    def handle_event(self, change: SEEventChange) -> None:
        """
        Check for death indicators.
        """
        if change.event == eEvent.IrreversibleState:
            self._irreversible_state = change.active

        if change.event == eEvent.CardiovascularCollapse:
            self._cardiovascular_collapse = change.active

        if change.event == eEvent.BrainOxygenDeficit:
            if change.active:
                if not self._brain_O2_deficit:
                    self._brain_O2_deficit = True
                    self._start_brain_O2_deficit_s = change.sim_time.get_value(TimeUnit.s)
            else:
                self._brain_O2_deficit = False
                self._start_brain_O2_deficit_s = 0

        if change.event == eEvent.MyocardiumOxygenDeficit:
            if change.active:
                if not self._myocardium_O2_deficit:
                    self._myocardium_O2_deficit = True
                    self._start_myocardium_O2_deficit_s = change.sim_time.get_value(TimeUnit.s)
            else:
                self._myocardium_O2_deficit = False
                self._start_myocardium_O2_deficit_s = 0

    def update(self, data_slice: NamedTuple, slice_idx: Dict[str, int]) -> None:
        """
        Determine if death is indicated, if so raise StopIteration.
        """
        curr_time_s = data_slice[slice_idx[self.TIME_s]]

        if self._irreversible_state:
            self._survived = False
            raise StopIteration(f"Patient died from irreversible state @{curr_time_s}s")

        if self._cardiovascular_collapse:
            self._survived = False
            raise StopIteration(f"Patient died from cardiovascular collapse @{curr_time_s}s")

        if self._max_hr_bpm is None:
            raise ValueError("Max HR BPM is not set in death check")
        elif data_slice[slice_idx[self.HR_bpm]] >= self._max_hr_bpm:
            self._survived = False
            raise StopIteration(f"Patient died from reaching max hr of {self._max_hr_bpm} @{curr_time_s}s")

        if self._brain_O2_deficit and (curr_time_s - self._start_brain_O2_deficit_s) > 180:
            self._survived = False
            raise StopIteration(f"Patient died from brain O2 deficit of 180s @{curr_time_s}s")

        if self._myocardium_O2_deficit and (curr_time_s - self._start_myocardium_O2_deficit_s) > 180:
            self._survived = False
            raise StopIteration(f"Patient died from myocardium O2 deficit of 180s @{curr_time_s}s")

        if data_slice[slice_idx[self.SPO2]] < 0.85:
            if not self._spO2_deficit:
                self._spO2_deficit = True
                self._start_spO2_deficit_s = curr_time_s
            elif (curr_time_s - self._start_spO2_deficit_s) > 140:
                self._survived = False
                raise StopIteration(f"Patient died from SpO2 < 85 for 140s @{curr_time_s}s")
        else:
            self._spO2_deficit = False

        #if data_slice[slice_idx[self.SAP_mmHg]] < 60:
        #    self._survived = False
        #    raise StopIteration(f"Patient died from SBP < 60 @{curr_time_s}s")

    def report(self) -> Iterable[Tuple[Hashable, Any]]:
        """
        Report survival status.
        """
        return [
            ("Survives", self._survived),
        ]


class ITMScenarioReport(SEScenarioReport):
    def __init__(
        self,
        log_file: Path,
        csv_file: Path,
        observation_frequency_min: float,
        actions_filter: Optional[List[str]],
        events_filter: Optional[List[str]],
        *unstructured_text_args: Any,
        **unstructured_text_kwargs: Any
    ):
        """
        Generates ITM report from given results and log files.

        :param log_file: Path to log file
        :param csv_file: Path to csv file containing scenario results
        :param observation_frequency_s: Frequency to include observations in report
        :param headers: Headers to include in each observation, if not using defaults
        :param actions_filter: Exclude actions containing any of these strings
        :param events_filter: Exclude events containing any of these strings
        """

        # Always filter advance time and serialize actions
        adv_time = "AdvanceTime"
        if actions_filter is None or adv_time not in actions_filter:
            if not actions_filter:
                actions_filter = list()
            actions_filter.append(adv_time)
        serialize = "Serialize"
        if serialize not in actions_filter:
            actions_filter.append(serialize)

        # Desired vitals and display names


        # Report modules
        observation_modules = [ITMObservationModule()]
        timestep_modules = None
        death_check_module = TCCCDeathCheckModule()

        super().__init__(
            log_file=log_file,
            csv_file=csv_file,
            observation_frequency_min=observation_frequency_min,
            observation_modules=observation_modules,
            timestep_modules=timestep_modules,
            death_check_module=death_check_module,
            actions_filter=actions_filter,
            events_filter=events_filter
        )

        # After super init so that we can use the parsed patient
        if self._patient.has_heart_rate_maximum():
            death_check_module.set_max_hr(self._patient.get_heart_rate_maximum())
        else:
            raise ValueError("Could not determine max heart rate")

    def _initialize_report(self) -> Dict:
        """
        Initialize the report dict with any initial datapoints.

        :return: Initialized report dict
        """
        init = super()._initialize_report()
        init["PatientAge_yr"] = self._patient.get_age().get_value(TimeUnit.yr)
        init["PatientSex"] = "Male" if self._patient.get_sex() == eSex.Male else "Female"

        return init


def generate_observations(injury_scenario: SEScenarioExecStatus) -> None:
    _pulse_logger.info(f"Generating observations for {injury_scenario.get_scenario_filename()}")

    # Replace "scenarios" in filepath with "observations"
    parts = list(Path(injury_scenario.get_scenario_filename()).parts)
    parts[parts.index("scenarios")] = "observations"
    out_file = Path(*parts)

    # TODO: Ask for observation frequency
    observation_frequency_min = 3

    # Call our post processor to generate observation files for each scenario
    report = ITMScenarioReport(
        Path(injury_scenario.get_log_filename()),            # log file
        Path(injury_scenario.get_csv_filename()),            # csv file
        3,                                                   # observation frequency (min)
        None,                                                # actions filter
        None,                                                # events filter
    )
    report.write(out_file)


if __name__ == "__main__":
    #faulthandler.enable()
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

    mode = "itm"
    if len(sys.argv) > 1:
        mode = sys.argv[1]
    injury_scenarios_list_filename = f"./test_results/patient_variability/{mode}/scenarios/tccc.json"

    # Read in the executed injury scenario exec status
    injury_scenarios = []
    serialize_scenario_exec_status_list_from_file(injury_scenarios_list_filename, injury_scenarios)

    print("Running "+str(len(injury_scenarios))+" scenarios")
    i = 0
    for injury_scenario in injury_scenarios:
        #if "AO0.9_H0_TBI0_TP0_D0s.json" in injury_scenario.get_scenario_filename():
        #    pass
        generate_observations(injury_scenario)
        print(str(i))
        i += 1

    """
    # TODO: Ask number of processes
    num_processes = 5
    # TODO: Chunksize? Setting chunksize can reduce overhead for very long iterables
    chunksize = 1

    # Generate observations for each injury scenario (in parallel)
    with Pool(num_processes) as p:
        # Issue tasks to the process pool
        # imap_unordered is a lazy version of map (reduces memory) and doesn't care about
        # return order so slower processes don't block the completion of faster processes
        p.imap_unordered(generate_observations, injury_scenarios, chunksize=chunksize)
        # Shutdown the process pool (don't allow more tasks to be issued)
        p.close()
        # Wait for all issued tasks to complete
        p.join()
    """
