# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import argparse
import json
import logging
import math
import timeit

from enum import Enum
from pathlib import Path
from timeit import default_timer as timer
from typing import List, NamedTuple, Dict

from army_dataset import ArmyDataset

from pulse.cdm.engine import SEAdvanceTime, SESerializeState, SEEventChange, eEvent
from pulse.cdm.enums import eSwitch, eSerializationFormat
from pulse.cdm.patient import eSex
from pulse.cdm.physiology import eHeartRhythm
from pulse.cdm.scenario import SEScenario, SEScenarioExecStatus
from pulse.cdm.scalars import FrequencyUnit, LengthUnit, TimeUnit
from pulse.cdm.io.scenario import serialize_scenario_to_file, \
                                  serialize_scenario_exec_status_list_to_file, \
                                  serialize_scenario_exec_status_list_from_file, \
                                  serialize_scenario_exec_status_to_string
from pulse.engine.PulseEngineResults import PulseEngineReprocessor, PulseResultsProcessor, PulseLogAction
from pulse.engine.PulseScenarioExec import PulseScenarioExec
from pulse.study.in_the_moment.casualty_generation import InjurySeverityOpts
from pulse.study.in_the_moment.triage_dataset import AVPU, TriageTag, TriageColor, Hemorrhage, PulseData, \
    Intervention

_log = logging.getLogger("pulse")


class Dataset(str, Enum):
    Army = "army"
    Navy = "navy"


def _exec_status_to_dict(status: SEScenarioExecStatus):
    return json.loads(serialize_scenario_exec_status_to_string(status, eSerializationFormat.JSON))


def _convert_keys_to_int(obj):
    new_obj = {}
    for k, v in obj.items():
        try:
            new_obj[int(k)] = v
        except ValueError:
            new_obj[k] = v
    return new_obj


class DeathCheckModule(PulseResultsProcessor):
    __slots__ = ["_cause_of_death", "_time_of_death",
                 "_brain_O2_deficit", "_start_brain_O2_deficit_s",
                 "_myocardium_O2_deficit", "_start_myocardium_O2_deficit_s",
                 "_spO2_deficit", "_start_spO2_deficit_s",
                 "_max_hr_bpm"
                 ]

    def __init__(self, max_hr_bpm):
        super().__init__()
        self._time_of_death = None
        self._cause_of_death = ""
        self._brain_O2_deficit = False
        self._start_brain_O2_deficit_s = 0.
        self._myocardium_O2_deficit = False
        self._start_myocardium_O2_deficit_s = 0.
        self._spO2_deficit = False
        self._start_spO2_deficit_s = 0.
        self._max_hr_bpm = max_hr_bpm

    @property
    def cause_of_death(self): return self._cause_of_death

    @property
    def time_of_death(self): return self._time_of_death

    def process_time_step(self,
                          data_slice: NamedTuple,
                          header_idx: Dict[str, int],
                          event_changes: List[SEEventChange],
                          action_changes: List[PulseLogAction]) -> None:
        # Time is always index 0 of the data_slice
        curr_time_s = data_slice[0]
        hr_bpm = data_slice[1]
        sp_o2 = data_slice[13]

        # Generally, you should process event/action changes every time step
        for event_change in event_changes:
            if event_change.event == eEvent.IrreversibleState and event_change.active:
                self._time_of_death = curr_time_s
                self._cause_of_death = f"Death from irreversible state."
                raise StopIteration(self._cause_of_death)

            if event_change.event == eEvent.CardiovascularCollapse and event_change.active:
                self._time_of_death = curr_time_s
                self._cause_of_death = f"Death from cardiovascular collapse."
                raise StopIteration(self._cause_of_death)

            if event_change.event == eEvent.BrainOxygenDeficit:
                if event_change.active:
                    if not self._brain_O2_deficit:
                        self._brain_O2_deficit = True
                        self._start_brain_O2_deficit_s = curr_time_s
                else:
                    self._brain_O2_deficit = False
                    self._start_brain_O2_deficit_s = 0
            if self._brain_O2_deficit and (curr_time_s - self._start_brain_O2_deficit_s) > 180:
                self._time_of_death = curr_time_s
                self._cause_of_death = f"Death from a brain O2 deficit lasting 180s."
                raise StopIteration(self._cause_of_death)

            if event_change.event == eEvent.MyocardiumOxygenDeficit:
                if event_change.active:
                    if not self._myocardium_O2_deficit:
                        self._myocardium_O2_deficit = True
                        self._start_myocardium_O2_deficit_s = curr_time_s
                else:
                    self._myocardium_O2_deficit = False
                    self._start_myocardium_O2_deficit_s = 0
            if self._myocardium_O2_deficit and (curr_time_s - self._start_myocardium_O2_deficit_s) > 180:
                self._time_of_death = curr_time_s
                self._cause_of_death = f"Death from a myocardium O2 deficit lasting 180s."
                raise StopIteration(self._cause_of_death)

        if hr_bpm >= self._max_hr_bpm:
            self._time_of_death = curr_time_s
            self._cause_of_death = f"Death from reaching max hr of {self._max_hr_bpm}."
            raise StopIteration(self._cause_of_death)

        if sp_o2 < 0.85:
            if not self._spO2_deficit:
                self._spO2_deficit = True
                self._start_spO2_deficit_s = curr_time_s
            elif (curr_time_s - self._start_spO2_deficit_s) > 140:
                self._time_of_death = curr_time_s
                self._cause_of_death = f"Death from SpO2 < 85 for 140s."
                raise StopIteration(self._cause_of_death)
        else:
            self._spO2_deficit = False


class TriageStudy:
    __slots__ = ["_output_dir", "_triage_study", "_dataset", "_pulse_data", "_tgt_id",
                 "_injury_scenarios_dir", "_injury_states_dir", "_injury_outputs_dir", "_injury_exec_status_filename",
                 "_intervention_scenarios_dir", "_intervention_outputs_dir", "_intervention_exec_status_filename",
                 "_total_interventions",
                 "injury_opts"
                 ]

    def __init__(self, dataset: Dataset, output_dir: Path):
        self._triage_study = {}
        self._output_dir = output_dir
        self._pulse_data = PulseData()
        self._tgt_id = None
        self._total_interventions = 0
        if dataset == Dataset.Army:
            self._dataset = ArmyDataset()
        else:
            raise NotImplementedError()

        self._injury_scenarios_dir = None
        self._injury_states_dir = None
        self._injury_outputs_dir = None
        self._injury_exec_status_filename = None
        self._intervention_scenarios_dir = None
        self._intervention_outputs_dir = None
        self._intervention_exec_status_filename = None
        self.injury_opts = InjurySeverityOpts()

    def _set_artifact_folder_name(self, folder: str):
        # Directories and files associated with simulating injuries using Pulse
        self._injury_scenarios_dir = self._output_dir / f"{folder}/injuries/scenarios"
        self._injury_states_dir = self._output_dir / f"{folder}/injuries/states"
        self._injury_outputs_dir = self._output_dir / f"{folder}/injuries/outputs"
        self._injury_scenarios_dir.mkdir(parents=True, exist_ok=True)
        self._injury_states_dir.mkdir(parents=True, exist_ok=True)
        self._injury_outputs_dir.mkdir(parents=True, exist_ok=True)
        # This tracks the status of the execution of these scenarios
        self._injury_exec_status_filename = self._output_dir / f"{folder}/injuries/exec_status.json"
        # Directories and files associated with simulating interventions using Pulse
        self._intervention_scenarios_dir = self._output_dir / f"{folder}/interventions/scenarios"
        self._intervention_outputs_dir = self._output_dir / f"{folder}/interventions/outputs"
        self._intervention_scenarios_dir.mkdir(parents=True, exist_ok=True)
        self._intervention_outputs_dir.mkdir(parents=True, exist_ok=True)
        # This tracks the status of the execution of these scenarios
        self._intervention_exec_status_filename = self._output_dir / f"{folder}/interventions/exec_status.json"

    @property
    def total_interventions(self): return self._total_interventions

    def triage(self, num_casualties: int, tgt_id: int = None, skip_visited: bool = False):
        start_time = timer()
        if num_casualties == 0:
            file = self._output_dir / f"training_casualties.json"
        else:
            file = self._output_dir / f"{num_casualties}_casualties.json"
        if file.exists():
            _log.info(f"Loading an existing file ({file}) for this number of casualties.")
            try:
                with open(file, 'r') as f:
                    self._triage_study = json.load(f, object_hook=_convert_keys_to_int)
                # Only keep patient specifications, everything else will be regenerated
                for i, casualty in self._triage_study.items():
                    if self._tgt_id:
                        if i != self._tgt_id:
                            continue
                    keys_to_remove = []
                    for key, item in casualty.items():
                        if key == "specification":
                            continue
                        keys_to_remove.append(key)
                    for key in keys_to_remove:
                        casualty.pop(key)
            except Exception as e:
                _log.error(f"Unable to load file {file}: {e}")
        else:
            self._triage_study = self._dataset.generate_dataset(num_casualties, injury_opts=self.injury_opts)
            with open(file, 'w') as f:
                json.dump(self._triage_study, f, indent=2)
        self._triage(file, tgt_id, skip_visited)
        elapsed_time = timer() - start_time
        _log.info(f"Execution took {elapsed_time/60:.1f} min")

    def triage_file(self, file: Path, tgt_id: int = None, skip_visited: bool = False):
        start_time = timer()
        if file.exists():
            with open(file, 'r') as f:
                self._triage_study = json.load(f, object_hook=_convert_keys_to_int)
                # Only keep patient specifications, everything else will be regenerated
                for i, casualty in self._triage_study.items():
                    if self._tgt_id:
                        if i != self._tgt_id:
                            continue
                    keys_to_remove = []
                    for key, item in casualty.items():
                        if key == "specification":
                            continue
                        keys_to_remove.append(key)
                    for key in keys_to_remove:
                        casualty.pop(key)
            self._triage(file, tgt_id, skip_visited)
        else:
            _log.fatal(f"Specified triage file does not exist: {file}")
            exit(1)
        elapsed_time = timer() - start_time
        _log.info(f"Execution took {elapsed_time/60:.1f} min")

    def _triage(self, out_file: Path, tgt_id: int = None, skip_visited: bool = False):
        self._tgt_id = tgt_id

        # Add a subdir to the output dir for this population size
        self._set_artifact_folder_name(out_file.stem)

        # Simulate the injuries and create states
        self._generate_initial_injury_states(untreated_injury_time_min=5,
                                             state_interval_min=5,
                                             total_injury_duration_min=60)
        # Triage all the injury states
        self._triage_injured_states()

        with open(out_file, 'w') as f:
            json.dump(self._triage_study, f, indent=2)

        # Simulate intervened casualties
        self._simulate_interventions(total_simulation_duration_min=60)
        # Assess final casualty state after each visit
        self._assess_interventions(duration_min=60)

        # Write out all the data we collected
        with open(out_file, 'w') as f:
            json.dump(self._triage_study, f, indent=2)

    def _death_triage(self,
                      time_min: float,
                      injuries: List[dict],
                      pulse_injuries: List[dict],
                      vitals: dict):
        vitals["avpu"] = AVPU.Unresponsive
        vitals["ambulatory"] = False
        vitals["brain_o2_pp"] = 0.0
        vitals["breathing"] = False
        vitals["breathing_distressed"] = None
        vitals["healthy_capillary_refill_time"] = False
        vitals["heart_rate"] = 0.0
        vitals["heart_rhythm"] = eHeartRhythm.Asystole.name
        vitals["peripheral_pulse"] = False
        vitals["respiratory_rate"] = 0.0
        vitals["spO2"] = 0.0
        vitals["systolic_pressure"] = 0
        vitals["diastolic_pressure"] = 0
        vitals["survivable_injuries"] = False

        # interventions will not work
        for injury in injuries:
            injury["can_intervene"] = False
        vitals["interventions"].clear()

        start_color, start_reason = self._start_tag(vitals)
        salt_color, salt_reason = self._salt_tag(vitals)
        bcd_color, bcd_reason = self._bcd_sieve_tag(vitals)
        triage = {
            "state": None,
            "vitals": vitals,
            "tags": {"start": start_color,
                     "start_reason": start_reason,
                     "salt": salt_color,
                     "salt_reason": salt_reason,
                     "bcd_sieve": bcd_color,
                     "bcd_sieve_reason": bcd_reason},
            "triss": 0.0,
            "news": 0.0,
            "injury_description": self._dataset.injury_description(time_min, injuries, pulse_injuries, vitals),
            "vitals_description": self._dataset.vitals_description(time_min, injuries, pulse_injuries, vitals)
        }
        return triage

    def _generate_initial_injury_states(self,
                                        untreated_injury_time_min: float,
                                        state_interval_min: float,
                                        total_injury_duration_min: float):
        executor = PulseScenarioExec()
        injury_scenarios: List[SEScenarioExecStatus] = []

        # Let's create a set of scenarios that create initial casualty states
        # Scenarios will not be rerun if they are marked as complete in this json file
        # You will need to delete the exec_status.json file if you want to rerun scenarios already run
        # You could also edit exec_status to rerun particular scenarios
        if not self._injury_exec_status_filename.exists():
            _log.info("Creating Pulse scenarios\n")
            for i, data in self._triage_study.items():
                sp = data["specification"]
                s = SEScenario()
                s.set_name(f"Casualty_{i}")
                s.set_description("")
                if "state" in sp:
                    s.set_engine_state(sp["state"])
                    _log.info(f"Creating casualty {i}: {sp['state']}")
                else:
                    p = s.get_patient_configuration().get_patient()
                    p.set_sex(eSex.Male if sp["sex"] == "male" else eSex.Female)
                    p.set_name(f"Casualty_{i}")
                    p.get_age().set_value(sp["age"], TimeUnit.yr)
                    p.get_height().set_value(sp["height"], LengthUnit.cm)
                    p.get_body_mass_index().set_value(sp["bmi"])
                    p.get_heart_rate_baseline().set_value(sp["heart_rate"], FrequencyUnit.Per_min)
                    _log.info(f"Creating casualty {i}: "
                              f"{sp['sex']}-{sp['age']}yr-{sp['height']}cm-{sp['bmi']}bmi-{sp['heart_rate']}bpm")

                s.get_data_request_manager().set_data_requests(self._pulse_data.data_requests)
                s.get_data_request_manager().set_results_filename(f"{self._injury_outputs_dir}/"
                                                                  f"casualty_{i}/initial_injury.csv")

                # Add a bit of buffer to show casualty baseline
                adv = SEAdvanceTime()
                adv.get_time().set_value(0.5, TimeUnit.min)
                s.get_actions().append(adv)

                injury_duration_min = 0.0
                # Add the injuries
                _log.info(f"Translating injuries to Pulse: {sp['injuries']}")
                for action in self._dataset.injury_actions(sp["injuries"]):
                    s.get_actions().append(action)
                    _log.info(f"\t{action}")

                # Advance the minimum injury time
                adv = SEAdvanceTime()
                adv.get_time().set_value(untreated_injury_time_min, TimeUnit.min)
                s.get_actions().append(adv)
                injury_duration_min += adv.get_time().get_value(TimeUnit.min)

                state = SESerializeState()
                state.set_comment(f"Injury Duration: {injury_duration_min} min")
                state.set_filename(str(self._injury_states_dir/f"casualty_{i}/injury@{injury_duration_min}min.json"))
                s.get_actions().append(state)

                # Simulate casualty saving new states at specified intervals
                while injury_duration_min <= total_injury_duration_min:
                    adv = SEAdvanceTime()
                    adv.get_time().set_value(state_interval_min, TimeUnit.min)
                    s.get_actions().append(adv)
                    injury_duration_min += adv.get_time().get_value(TimeUnit.min)

                    state = SESerializeState()
                    state.set_comment(f"Injury Duration: {injury_duration_min} min")
                    state.set_filename(str(self._injury_states_dir/f"casualty_{i}/injury@{injury_duration_min}min.json"))
                    s.get_actions().append(state)

                # Write the scenario to disk
                sce_path = Path(f"{self._injury_scenarios_dir}/casualty_{i}")
                sce_path.mkdir(parents=True, exist_ok=True)
                f = f"{sce_path}/initial_injury.json"
                serialize_scenario_to_file(s, f)
                # Add this scenario to our exec status
                e = SEScenarioExecStatus()
                e.set_scenario_filename(f)
                injury_scenarios.append(e)
                _log.info("")

            # Write out the exec status so we can run it
            serialize_scenario_exec_status_list_to_file(injury_scenarios,
                                                        str(self._injury_exec_status_filename))
        # Now run those scenarios (this will be quick if its already been run)
        # executor.set_thread_count(1)
        executor.set_log_to_console(eSwitch.Off)  # Output can get pretty busy...
        executor.set_output_root_directory(str(self._injury_outputs_dir))
        executor.set_scenario_exec_list_filename(str(self._injury_exec_status_filename))
        _log.info("Executing injury scenarios")
        if not executor.execute_scenario():
            # You can view the casualty_states_exec to see what happened
            _log.fatal(f"Problem running {self._injury_exec_status_filename}")
            exit(1)
        # Read in the exec status and return it
        casualty_states_exec_status: List[SEScenarioExecStatus] = []
        serialize_scenario_exec_status_list_from_file(str(self._injury_exec_status_filename),
                                                      casualty_states_exec_status)

        if len(casualty_states_exec_status) != len(self._triage_study):
            _log.fatal(f"Number of scenarios executed ({len(casualty_states_exec_status)}) "
                       f"does not equal the number of triage study casualties ({len(self._triage_study)})")
            exit(1)
        for i, status in enumerate(casualty_states_exec_status):
            self._triage_study[i]["injury_exec_status"] = _exec_status_to_dict(status)

    def _triage_injured_states(self):
        for i, data in self._triage_study.items():
            if self._tgt_id:
                if i != self._tgt_id:
                    continue
            _log.info(f"Triaging casualty {i} - {data['specification']['injuries']}")

            exec_status = data["injury_exec_status"]
            spec = data["specification"]
            injuries = spec["injuries"]

            # Pull the results from our exec status
            r = PulseEngineReprocessor(csv_files=[Path(exec_status["InitializationStatus"]["CSVFilename"])],
                                       log_files=[Path(exec_status["InitializationStatus"]["LogFilename"])])
            # Get which casualty this is
            casualty = Path(exec_status["ScenarioFilename"]).parts[-2]

            states = {}
            pulse_injuries = []
            # Get actions provided to the scenario, they are the Pulse injury actions
            # These are provided as a dict, and the key is the time provided
            # We may want to get the active actions in API format
            # It really depends on how much we need the action info here
            for time_s, actions in r.actions.items():
                for action in actions:
                    if action.name == "SerializeState":
                        # Our scenarios will have serialization actions with a comment containing injury duration
                        comment = action.data["SerializeState"]["Action"]["Comment"]
                        duration_min = float(comment[comment.find(':')+1:comment.find("min")].strip())
                        states[duration_min] = action.data["SerializeState"]["Filename"]
                    else:
                        pulse_injuries.append(action.data)
            data["pulse_injuries"] = pulse_injuries

            # Check to see when/if the casualty died
            death_module = DeathCheckModule(r.patient.get_heart_rate_maximum().get_value(FrequencyUnit.Per_min))
            r.replay([death_module])
            if death_module.cause_of_death:
                _log.info(f"{casualty} cause of death: {death_module.cause_of_death}")
                # Grab some vitals from the time of death
                self._pulse_data.set_values(r.get_values_at_time(r.end_time_s-1))
                active_events = r.get_active_events_in_window(r.start_time_s, r.end_time_s)
                vitals = self._dataset.calculate_triage_vitals(spec, active_events, self._pulse_data)
                triage = self._death_triage(death_module.time_of_death/60, injuries, pulse_injuries, vitals)
                data["death"] = {"time": death_module.time_of_death/60,
                                 "cause": death_module.cause_of_death,
                                 "triage": triage}

            # dict of triage times of interest for this casualty to triage vitals
            data["visits"] = {}
            # Data needed for tagging protocols for every triage time for this casualty
            for time_min, injury_state in states.items():
                time_s = time_min * 60
                if death_module.time_of_death and time_s >= death_module.time_of_death:
                    continue
                self._pulse_data.set_values(r.get_values_at_time(time_s))
                # Get active events from the last minute of this triage time
                active_events = r.get_active_events_in_window(time_s - 60, time_s)

                vitals = self._dataset.calculate_triage_vitals(spec, active_events, self._pulse_data)
                start_color, start_reason = self._start_tag(vitals)
                salt_color, salt_reason = self._salt_tag(vitals)
                bcd_color, bcd_reason = self._bcd_sieve_tag(vitals)
                triage = {
                    "state": injury_state,
                    "vitals": vitals,
                    "tags": {"start": start_color,
                             "start_reason": start_reason,
                             "salt": salt_color,
                             "salt_reason": salt_reason,
                             "bcd_sieve": bcd_color,
                             "bcd_sieve_reason": bcd_reason},
                    "triss": self._calculate_triss_score(vitals),
                    "news": self._calculate_news_score(vitals),
                    "injury_description": self._dataset.injury_description(time_min, injuries, pulse_injuries, vitals),
                    "vitals_description": self._dataset.vitals_description(time_min, injuries, pulse_injuries, vitals)
                }
                data["visits"][time_min] = {"triage": triage}

    @staticmethod
    def _calculate_triss_score(vitals: dict):
        # https://www.mdapp.co/trauma-injury-severity-score-triss-calculator-277/

        # Age
        age = vitals["age"]
        age_index = 0
        if age > 55:
            age_index = 1

        # Glasgow Coma Scale
        gcs = 0
        gcs_code = 0
        # https://www.mdcalc.com/calc/64/glasgow-coma-scale-score-gcs#evidence (keep clicking evidence button)
        avpu = vitals["avpu"]
        if avpu == AVPU.Unresponsive:
            # Assuming no eye-opening, no verbal response, flexor and extensor reactions
            gcs = 3
            gcs_code = 0
        elif avpu == AVPU.Pain:
            # Assuming eye-opening to pain stimulus,
            # Inappropriate words with no sentences
            # Movement toward pressure/pain
            gcs = 8
            gcs_code = 2
        elif avpu == AVPU.Voice:
            gcs = 12  # Assuming eye open to auditory stimulus, oriented responses, conscious obeying of motor commands
            gcs_code = 3
        else:  # ALERT
            gcs = 15  # Assuming spontaneous opening, oriented responses, conscious obeying of motor commands
            gcs_code = 4

        # Systolic Blood Pressure
        sbp = vitals["systolic_pressure"]
        if sbp >= 89:
            sbp_code = 4
        elif 76 <= sbp < 89:
            sbp_code = 3
        elif 50 <= sbp < 75:
            sbp_code = 2
        elif 1 <= sbp < 50:
            sbp_code = 1
        else:
            sbp_code = 0

        rr = vitals["respiratory_rate"]
        if 10 <= rr <= 29:
            rr_code = 4
        elif rr > 29:
            rr_code = 3
        elif 6 <= rr < 10:
            rr_code = 2
        elif 1 <= rr < 6:
            rr_code = 1
        else:
            rr_code = 0

        revised_trauma_score = gcs_code*0.9368 + sbp_code*0.7326 + rr_code*0.2908

        iss = vitals["iss"]
        # https://www.mdapp.co/trauma-injury-severity-score-triss-calculator-277/
        if vitals["blunt_trauma"]:
            triss = -0.4499 + 0.8085*revised_trauma_score - 0.0835*iss - 1.7430*age_index
        else:
            triss = -2.5355 + 0.9934*revised_trauma_score - 0.0651*iss - 1.1360*age_index

        pd_survival = 1 / (1 + math.exp(-triss)) * 100

        return pd_survival

    @staticmethod
    def _calculate_news_score(vitals: dict):
        # https://www.mdcalc.com/calc/1873/national-early-warning-score-news#next-steps
        news = 0

        # Respiration Rate
        if 8 < vitals["respiratory_rate"] < 12:
            news = news+1
        elif 21 <= vitals["respiratory_rate"] < 25:
            news = news+2
        else:
            news = news+3

        # O2 Saturation
        sp_o2 = vitals["spO2"]
        if 0.94 <= sp_o2 < 0.96:
            news = news+1
        elif 0.92 <= sp_o2 < 0.94:
            news = news+2
        elif sp_o2 < 0.92:
            news = news+3

        # TODO temperature

        # Systolic Blood Pressure
        sbp = vitals["systolic_pressure"]
        if sbp >= 220 or sbp <= 90:
            news = news+3
        elif 91 < sbp < 100:
            news = news+2
        elif 100 <= sbp < 110:
            news = news+1

        # Heart Rate
        hr = vitals["heart_rate"]
        if hr <= 40 or hr >= 131:
            news = news+3
        elif 131 > hr >= 110:
            news = news+2
        elif 110 > hr >= 90:
            news = news+1
        elif 90 > hr > 40:
            news = news+1

        # AVPU
        if not vitals["avpu"] == AVPU.Alert:
            news = news+3

        return news

    @staticmethod
    def _start_tag(vitals: dict) -> (str, str):
        tag = TriageTag()

        if vitals["ambulatory"]:
            tag.apply(TriageColor.Green, "Casualty is ambulatory.")
            return tag.color, tag.reason

        if not vitals["breathing"]:
            if "reposition_airway" in vitals["interventions"]:
                tag.apply(TriageColor.Red, "Casualty was not breathing.\n"
                                           "Repositioning their airway resulted in spontaneous breathing.")
            else:
                tag.apply(TriageColor.Black, "Casualty is not breathing.\n"
                                             "Repositioning their airway did not help breathing.")

        if vitals["respiratory_rate"] > 30.0:
            tag.apply(TriageColor.Red, "Casualty respiratory rate > 30 bpm.")

        if not vitals["healthy_capillary_refill_time"]:
            tag.apply(TriageColor.Red, "Casualty does not have a healthy capillary refill time.")

        if vitals["avpu"] == AVPU.Alert:
            tag.apply(TriageColor.Yellow, "Casualty is unable to walk, but can follow commands.")
        else:
            tag.apply(TriageColor.Red, "Casualty is unable to follow commands.")

        return tag.color, tag.reason

    @staticmethod
    def _salt_tag(vitals: dict) -> (str, str):
        tag = TriageTag()

        if vitals["ambulatory"]:
            tag.apply(TriageColor.Green, "Casualty is ambulatory.")
            return tag.color, tag.reason

        hemorrhage = vitals["hemorrhage"]
        survivable = vitals["survivable_injuries"]

        # Is the casualty not breathing?
        if not vitals["breathing"]:
            if "reposition_airway" in vitals["interventions"]:
                tag.apply(TriageColor.Red, "Casualty was not breathing.\n"
                                           "Repositioning their airway resulted in spontaneous breathing.")
            else:
                tag.apply(TriageColor.Black, "Casualty is not breathing.\n"
                                             "Repositioning their airway did not result in spontaneous breathing.\n"
                                             "Casualty is NOT likely to survive these injuries.")

        # Does the casualty obey commands or make purposeful movements?
        if vitals["avpu"] == AVPU.Pain or vitals["avpu"] == AVPU.Unresponsive:
            if survivable:
                tag.apply(TriageColor.Red, "Casualty does not obey commands or make purposeful movements.\n"
                                           "Casualty is likely to survive these injuries.")
            else:
                tag.apply(TriageColor.Black, "Casualty does not obey commands or make purposeful movements.\n"
                                             "Casualty is NOT likely to survive these injuries.")

        # Does the casualty have a peripheral pulse?
        elif not vitals["peripheral_pulse"]:
            if survivable:
                tag.apply(TriageColor.Red, "Casualty does not have a peripheral pulse.\n"
                                           "Casualty is likely to survive these injuries.")
            else:
                tag.apply(TriageColor.Black, "Casualty does not have a peripheral pulse.\n"
                                             "Casualty is NOT likely to survive these injuries.")

        # Is the casualty in respiratory distress?
        elif vitals["breathing_distressed"]:
            if survivable:
                tag.apply(TriageColor.Red, "Casualty is in respiratory distress.\n"
                                           "Casualty is likely to survive these injuries.")
            else:
                tag.apply(TriageColor.Black, "Casualty is in respiratory distress.\n"
                                             "Casualty is NOT likely to survive these injuries.")

        # Does the casually have a major, uncontrollable hemorrhage?
        elif hemorrhage == Hemorrhage.Major:
            if "wound_pack" in vitals["interventions"]:
                tag.apply(TriageColor.Red, "Casualty has a major hemorrhage.\n"
                                           "You were able to pack the wound with gauze to reduce the bleeding.")
            elif "tourniquet" in vitals["interventions"]:
                tag.apply(TriageColor.Red, "Casualty has a major hemorrhage.\n"
                                           "You were able to apply a tourniquet to reduce the bleeding.")
            else:
                if survivable:
                    tag.apply(TriageColor.Red, "Casualty has a major hemorrhage.\n"
                                               "Casualty is likely to survive these injuries.")
                else:
                    tag.apply(TriageColor.Black, "Casualty has a major hemorrhage.\n"
                                                 "Casualty is NOT likely to survive these injuries.")

        # Nothing too crazy...
        else:
            if vitals["major_injuries"]:
                tag.apply(TriageColor.Yellow, "Casualty injuries major.")
            else:
                tag.apply(TriageColor.Green, "Casualty injuries are minor.")

        return tag.color, tag.reason

    @staticmethod
    def _bcd_sieve_tag(vitals: dict) -> (str, str):
        tag = TriageTag()

        if vitals["ambulatory"]:
            tag.apply(TriageColor.Green, "Casualty is ambulatory.")
            return tag.color, tag.reason

        hemorrhage = vitals["hemorrhage"]
        if hemorrhage and hemorrhage == Hemorrhage.Major:
            tag.apply(TriageColor.Red, "Casualty has catastrophic hemorrhage.")

        if not vitals["breathing"]:
            if Intervention.RepositionAirway in vitals["interventions"]:
                tag.apply(TriageColor.Red, "Casualty was not breathing.\n"
                                           "Repositioning their airway resulted in spontaneous breathing.")
            else:
                tag.apply(TriageColor.Black, "Casualty is not breathing.\n"
                                             "Repositioning their airway did not help breathing.")

        if vitals["avpu"] == AVPU.Pain or vitals["avpu"] == AVPU.Unresponsive:
            tag.apply(TriageColor.Red, "Casualty is not responding to voice.")

        # TODO Should we put in specific values instead of general descriptions?

        if vitals["respiratory_rate"] > 23.0 or vitals["respiratory_rate"] < 12.0:
            tag.apply(TriageColor.Red, "Casualty has abnormal breathing rate.")

        if vitals["heart_rate"] > 100:
            tag.apply(TriageColor.Red, "Casualty has elevated heart rate.")
        else:
            tag.apply(TriageColor.Yellow, "Casualty has normal heart rate.")

        return tag.color, tag.reason

    def _simulate_interventions(self, total_simulation_duration_min: float):

        executor = PulseScenarioExec()
        intervention_scenarios: List[SEScenarioExecStatus] = []

        # Add an intervention dict to casualties we can treat
        for i, casualty in self._triage_study.items():
            if self._tgt_id:
                if i != self._tgt_id:
                    continue
            for time_min, visit in casualty["visits"].items():
                if self._dataset.can_perform_interventions(casualty["specification"]["injuries"],
                                                           visit["triage"]["vitals"]):
                    visit["intervention"] = {}

        # Let's create a set of scenarios that apply protocol interventions to injured casualties
        # Scenarios will not be rerun if they are marked as complete in this json file
        # You will need to delete the exec_status.json file if you want to rerun scenarios already run
        # You could also edit exec_status to rerun particular scenarios
        if not self._intervention_exec_status_filename.exists():
            for i, casualty in self._triage_study.items():
                for time_s, visit in casualty["visits"].items():

                    # Only simulate the injuries we can perform interventions on
                    if "intervention" not in visit:
                        continue
                    _log.info(f"Performing interventions on casualty {i}")

                    triage = visit["triage"]
                    s_fn = ("intervention" + triage["state"][triage["state"].rfind('@'):])
                    o_fn = s_fn.replace(".json", ".csv")

                    s = SEScenario()
                    s.set_name(f"Casualty {i}")
                    s.set_description(f"Interventions for casualty {i}")
                    s.set_engine_state(triage["state"])
                    s.get_data_request_manager().set_data_requests(self._pulse_data.data_requests)
                    s.get_data_request_manager().set_results_filename(f"{self._intervention_outputs_dir}"
                                                                      f"/casualty_{i}/{o_fn}")
                    # Add interventions
                    for action in self._dataset.injury_interventions(casualty["specification"]["injuries"],
                                                                     casualty["pulse_injuries"],
                                                                     triage["vitals"]):
                        s.get_actions().append(action)
                        # TODO add action action to our data structure

                    # Simulate the treated casualty for an amount of time
                    adv = SEAdvanceTime()
                    adv.get_time().set_value(total_simulation_duration_min, TimeUnit.min)
                    s.get_actions().append(adv)
                    # Write out the scenario
                    sce_path = Path(f"{self._intervention_scenarios_dir}/casualty_{i}/")
                    sce_path.mkdir(parents=True, exist_ok=True)
                    f = f"{sce_path}/{s_fn}"
                    serialize_scenario_to_file(s, f)
                    # Add this scenario to our exec status
                    e = SEScenarioExecStatus()
                    e.set_scenario_filename(f)
                    intervention_scenarios.append(e)

            # Write out the exec status so we can run it
            serialize_scenario_exec_status_list_to_file(intervention_scenarios,
                                                        str(self._intervention_exec_status_filename))
        # Now run those scenarios (this will be quick if its already been run)
        # executor.set_thread_count(1)
        executor.set_log_to_console(eSwitch.Off)  # Output can get pretty busy...
        executor.set_output_root_directory(str(self._intervention_outputs_dir))
        executor.set_scenario_exec_list_filename(str(self._intervention_exec_status_filename))
        _log.info("Executing intervention scenarios")
        if not executor.execute_scenario():
            # You can view the casualty_states_exec to see what happened
            _log.fatal(f"Problem running {self._intervention_exec_status_filename}")
            exit(1)
        # Read in the exec status and return it
        intervention_exec_status: List[SEScenarioExecStatus] = []
        serialize_scenario_exec_status_list_from_file(str(self._intervention_exec_status_filename),
                                                      intervention_exec_status)

        v = 0
        for i, casualty in self._triage_study.items():
            if self._tgt_id:
                if i != self._tgt_id:
                    v += 1
                    continue
            for time_s, visit in casualty["visits"].items():
                if "intervention" not in visit:
                    continue
                visit["intervention"]["intervention_exec_status"] = _exec_status_to_dict(intervention_exec_status[v])
                v += 1
        self._total_interventions = v

    def _assess_interventions(self, duration_min: float):
        p = 0
        for i, casualty in self._triage_study.items():
            if self._tgt_id:
                if i != self._tgt_id:
                    continue

            spec = casualty["specification"]
            for time_s, visit in casualty["visits"].items():
                if "intervention" not in visit:
                    continue

                intervention = visit["intervention"]
                p += 1
                _log.info(f"[{p}/{self._total_interventions}]"
                          f"Assessing casualty {i} treated at time {time_s}")

                exec_status = intervention["intervention_exec_status"]

                # Pull the results from our exec status
                r = PulseEngineReprocessor(csv_files=[Path(exec_status["InitializationStatus"]["CSVFilename"])],
                                           log_files=[Path(exec_status["InitializationStatus"]["LogFilename"])])

                # Check to see when/if the casualty died
                death_module = DeathCheckModule(
                    r.patient.get_heart_rate_maximum().get_value(FrequencyUnit.Per_min))
                r.replay([death_module])
                if death_module.cause_of_death:
                    _log.info(f"Intervened casualty {i} died.")
                    _log.info(f"Cause of death: {death_module.cause_of_death}")
                    intervention["death"] = {"time_s": death_module.time_of_death,
                                             "cause": death_module.cause_of_death}
                else:
                    self._pulse_data.set_values(r.get_values_at_time(r.end_time_s))
                    # Get active events from the last minute of this simulation
                    active_events = r.get_active_events_in_window(r.end_time_s - 60, r.end_time_s)
                    vitals = self._dataset.calculate_triage_vitals(spec, active_events, self._pulse_data)
                    intervention["vitals"] = vitals
                    start_color, start_reason = self._start_tag(vitals)
                    salt_color, salt_reason = self._salt_tag(vitals)
                    bcd_color, bcd_reason = self._bcd_sieve_tag(vitals)
                    tags = {"start": start_color,
                            "start_reason": start_reason,
                            "salt": salt_color,
                            "salt_reason": salt_reason,
                            "bcd_sieve": bcd_color,
                            "bcd_sieve_reason": bcd_reason}
                    intervention["tags"] = tags
                    intervention["triss"] = self._calculate_triss_score(vitals)
                    intervention["news"] = self._calculate_news_score(vitals)
                    intervention["injury_description"] = [f"Casualty has been waiting {duration_min} min for further care."]
                    intervention["vitals_description"] = [""]  # TODO Need to improve vitals to handle interventions


def main():
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
    parser = argparse.ArgumentParser(description="Execute ITM triage study")
    parser.add_argument(
        "-o", "--output_dir",
        type=Path,
        default="./test_results/itm/triage_study",
        help="Location to put all files related to this study"
    )
    # Training related arguments
    dataset_group = parser.add_mutually_exclusive_group()
    dataset_group.add_argument(
        "-t", "--train",
        action='store_true',
        help="Generate the training study file\n"
             "The population file will be written here: <output_dir>/populations/training_casualties.json\n"
             "The study file will be written here: <output_dir>/triage_study_training.json"
    )
    dataset_group.add_argument(
        "-num", "--num_casualties",
        type=int,
        default=None,
        help="Specify the number of casualties\n"
             "A new casualty file will be created using the convention, <output_dir>/casualties/<size>_casualties.\n"
             "If a file of this name already exists, it will be loaded and used.\n"
             "Delete or rename the casualty file if you would like to generate a new file"
    )
    dataset_group.add_argument(
        "-sf", "--triage_file",
        type=Path,
        default=None,
        help="Process an existing file."
    )
    parser.add_argument(
        "-s", "--skip_visited",
        type=bool,
        default=False,
        help="If True, casualties with triage data will be skipped.\n"
             "If False, all casualties will be triaged. Useful during development\n"
             "Useful for large populations, and this program stops in the middle of generating this triage file."
    )
    parser.add_argument(
        "-id", "--id",
        type=int,
        default=None,
        help="Specific casualty id to triage"
    )

    # Special arguments
    parser.add_argument(
        "-min_pd", "--max_injury_severity_percent_difference",
        type=float,
        default=5,
        help="Accurate representative casualty sizes are dependent on well np.random.normal \n"
             "can generate injury severities that match the specified injury severity distribution mean.\n"
             "This program will halt if the mean of any generated injury severity list is more than provided value.\n"
             "This option is only used when -num is provided and a new file/dataset is created."
    )
    parser.add_argument(
        "-fp", "--force_injury_severity_distributions",
        type=float,
        default=-1,
        help="Iterate np.random.normal to attempt to find passing injury severity distributions.\n"
             "This option is only used when -num is provided and a new file/dataset is created.\n"
             "Using this option can invalidate the representative accuracy of your dataset."
    )
    opts = parser.parse_args()
    output_dir = opts.output_dir
    output_dir.mkdir(parents=True, exist_ok=True)

    file_handler = logging.FileHandler(output_dir/"triage_study_pipeline.log")
    file_handler.setLevel(logging.INFO)
    _log.addHandler(file_handler)

    triage_study = TriageStudy(Dataset.Army, output_dir)
    if opts.train:
        triage_study.injury_opts.force_valid_distributions = opts.force_injury_severity_distributions
        triage_study.injury_opts.max_percent_difference = opts.max_injury_severity_percent_difference
        triage_study.triage(num_casualties=0, tgt_id=opts.id, skip_visited=opts.skip_visited)
    elif opts.num_casualties:
        triage_study.triage(num_casualties=opts.num_casualties, tgt_id=opts.id, skip_visited=opts.skip_visited)
    elif opts.triage_file:
        triage_study.triage_file(file=opts.triage_file, tgt_id=opts.id, skip_visited=opts.skip_visited)


if __name__ == "__main__":
    main()
