# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import logging
from enum import Enum
from typing import List

from pulse.cdm.engine import SEEventChange, eEvent
from pulse.cdm.patient_actions import SEHemorrhage
from pulse.cdm.scalars import TimeUnit
from pulse.engine.PulseEngineResults import PulseResultsProcessor

_log = logging.getLogger("pulse")


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


class DeathCheck:
    """
    Check if the patient is dead
    """

    __slots__ = ("_brain_O2_deficit", "_start_brain_O2_deficit_s",
                 "_myocardium_O2_deficit", "_start_myocardium_O2_deficit_s",
                 "_spO2_deficit", "_start_spO2_deficit_s",
                 "_max_hr_bpm", "_cause_of_death")

    TIME_s = "Time(s)"
    HR_bpm = "HeartRate(1/min)"
    SPO2 = "OxygenSaturation"
    SAP_mmHg = "SystolicArterialPressure(mmHg)"

    def __init__(self, max_hr_bpm):
        super().__init__()

        self._brain_O2_deficit = False
        self._start_brain_O2_deficit_s = 0.
        self._myocardium_O2_deficit = False
        self._start_myocardium_O2_deficit_s = 0.
        self._spO2_deficit = False
        self._start_spO2_deficit_s = 0.
        self._max_hr_bpm = max_hr_bpm
        self._cause_of_death = ""

    @property
    def cause_of_death(self): return self._cause_of_death

    def is_dead(self, curr_time_s: float, hr_bpm: float, spO2: float, events: List[SEEventChange]) -> bool:
        """
        Determine if death is indicated, if so raise StopIteration.
        """

        for change in events:
            if change.event == eEvent.IrreversibleState and change.active:
                self._cause_of_death = f"Patient died from irreversible state @{curr_time_s}s"
                return True

            if change.event == eEvent.CardiovascularCollapse and change.active:
                self._cause_of_death = f"Patient died from cardiovascular collapse @{curr_time_s}s"
                return True

            if change.event == eEvent.BrainOxygenDeficit:
                if change.active:
                    if not self._brain_O2_deficit:
                        self._brain_O2_deficit = True
                        self._start_brain_O2_deficit_s = change.sim_time.get_value(TimeUnit.s)
                    elif (curr_time_s - self._start_brain_O2_deficit_s) > 180:
                        self._cause_of_death = f"Patient died from brain O2 deficit of 180s @{curr_time_s}s"
                        return True
                else:
                    self._brain_O2_deficit = False
                    self._start_brain_O2_deficit_s = 0

            if change.event == eEvent.MyocardiumOxygenDeficit:
                if change.active:
                    if not self._myocardium_O2_deficit:
                        self._myocardium_O2_deficit = True
                        self._start_myocardium_O2_deficit_s = change.sim_time.get_value(TimeUnit.s)
                    elif (curr_time_s - self._start_myocardium_O2_deficit_s) > 180:
                        self._cause_of_death = f"Patient died from myocardium O2 deficit of 180s @{curr_time_s}s"
                        return True
                else:
                    self._myocardium_O2_deficit = False
                    self._start_myocardium_O2_deficit_s = 0

        if hr_bpm >= self._max_hr_bpm:
            self._cause_of_death = f"Patient died from reaching max hr of {self._max_hr_bpm} @{curr_time_s}s"
            return True

        if spO2 < 0.85:
            if not self._spO2_deficit:
                self._spO2_deficit = True
                self._start_spO2_deficit_s = curr_time_s
            elif (curr_time_s - self._start_spO2_deficit_s) > 140:
                self._cause_of_death = f"Patient died from SpO2 < 85 for 140s @{curr_time_s}s"
                return True
        else:
            self._spO2_deficit = False

        return False


class START_Protocol:
    pass


class SALT_Protocol:
    pass


class BCD_Sieve_Protocol:

    @staticmethod
    def tag(hemorrhage_actions: List[SEHemorrhage],
            ambulatory: bool,
            avpu: AVPU,
            respiration_rate_bpm: float,
            heart_rate_bpm: float) -> TriageTag:  # TODO return intervention actions to take

        if len(hemorrhage_actions) > 0:
            for hemorrhage in hemorrhage_actions:
                if hemorrhage.get_severity() > 0.5:  # TODO define catastrophic hemorrhage
                    return TriageTag.Red

        if ambulatory:
            return TriageTag.Green

        if respiration_rate_bpm <= 2:  # TODO and no SEAirwayObstruction
            return TriageTag.Black

        if avpu == AVPU.Pain or avpu == AVPU.Unresponsive:
            return TriageTag.Red

        if respiration_rate_bpm < 12 or respiration_rate_bpm > 23:
            return TriageTag.Red

        if heart_rate_bpm > 100:
            return TriageTag.Red

        return TriageTag.Yellow


class ITM_Triage_Module(PulseResultsProcessor):
    pass
