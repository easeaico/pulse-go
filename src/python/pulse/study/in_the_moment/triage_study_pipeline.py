# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import argparse
import json
import logging
import math

from datetime import datetime
from enum import Enum
from pathlib import Path
from timeit import default_timer as timer
from typing import List, NamedTuple, Dict

from army_dataset import ArmyDataset

from pulse.cdm.engine import SEAdvanceTime, SESerializeState, SEEventChange, eEvent
from pulse.cdm.enums import eSwitch, eSerializationFormat
from pulse.cdm.patient import eSex, SEPatient
from pulse.cdm.physiology import eHeartRhythm
from pulse.cdm.scenario import SEScenario, SEScenarioExecStatus
from pulse.cdm.scalars import FrequencyUnit, LengthUnit, TimeUnit
from pulse.cdm.io.scenario import serialize_scenario_to_file, \
    serialize_scenario_exec_status_list_to_file, \
    serialize_scenario_exec_status_list_from_file, \
    serialize_scenario_exec_status_to_string, serialize_scenario_from_file
from pulse.engine.PulseEngine import PulseEngine
from pulse.engine.PulseEngineResults import PulseEngineReprocessor, PulseResultsProcessor, PulseLogAction
from pulse.engine.PulseScenarioExec import PulseScenarioExec
from pulse.study.in_the_moment.casualty_generation import InjurySeverityOpts
from pulse.study.in_the_moment.triage_dataset import AVPU, TriageTag, TriageColor, PulseData, \
    Intervention, convert_keys_to_int

_log = logging.getLogger("pulse")


class Dataset(str, Enum):
    Army = "army"
    Navy = "navy"


def _exec_status_to_dict(status: SEScenarioExecStatus):
    return json.loads(serialize_scenario_exec_status_to_string(status, eSerializationFormat.JSON))


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
                 "_total_interventions", "_num_pulse_casualties",
                 "injury_opts", "keep_triage"
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
        self.keep_triage = False
        self._num_pulse_casualties = 0

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

    @staticmethod
    def _clear_casualty(casualty: dict):
        keys_to_remove = []
        for key, item in casualty.items():
            if key == "specification":
                continue
            keys_to_remove.append(key)
        for key in keys_to_remove:
            casualty.pop(key)

    def triage(self, num_casualties: int, tgt_id: int = None, skip_visited: bool = False,
               untreated_injury_time_min: int = 5, state_interval_min: int = 5, total_injury_duration_min: int = 60):
        start_time = timer()
        if num_casualties == 0:
            file = self._output_dir / f"example_casualties.json"
        else:
            file = self._output_dir / f"{num_casualties}_casualties.json"
        if file.exists():
            _log.info(f"Loading an existing file ({file}) for this number of casualties.")
            try:
                with open(file, 'r') as f:
                    self._triage_study = json.load(f, object_hook=convert_keys_to_int)
                if not self.keep_triage:
                    # Only keep patient specifications, everything else will be regenerated
                    for i, casualty in self._triage_study.items():
                        if self._tgt_id:
                            if i != self._tgt_id:
                                continue
                        self._clear_casualty(casualty)
                self._num_pulse_casualties = 0
                for i, casualty in self._triage_study.items():
                    spec = casualty["specification"]
                    if spec["pulse"]:
                        self._num_pulse_casualties += 1
            except Exception as e:
                _log.error(f"Unable to load file {file}: {e}")
        else:
            self._triage_study = self._dataset.generate_dataset(num_casualties, injury_opts=self.injury_opts)

            # Check Pulse can simulate patients
            p = SEPatient()
            pulse = PulseEngine()
            pulse.log_to_console(False)
            self._num_pulse_casualties = 0
            for i, casualty in self._triage_study.items():
                spec = casualty["specification"]
                if "state" in spec:
                    spec["pulse"] = True
                    self._num_pulse_casualties += 1
                    continue

                p.clear()
                if spec["sex"] == "female":
                    p.set_sex(eSex.Female)
                p.get_age().set_value(spec["age"], TimeUnit.yr)
                p.get_height().set_value(spec["height"], LengthUnit.cm)
                p.get_body_mass_index().set_value(spec["bmi"])
                # HR range is too wide, not using it for now
                # p.get_heart_rate_baseline().set_value(spec["heart_rate"], FrequencyUnit.Per_min)
                if not pulse.is_valid_patient(p):
                    spec["pulse"] = False
                    _log.warning(f"Pulse cannot simulate casualty {i}\n{spec}")
                else:
                    spec["pulse"] = True
                    self._num_pulse_casualties += 1

            with open(file, 'w') as f:
                json.dump(self._triage_study, f, indent=2)
        _log.info(f"Study has {self._num_pulse_casualties}/{len(self._triage_study)} Pulse compatible casualties")
        self._triage(file, tgt_id, skip_visited,
                     untreated_injury_time_min=untreated_injury_time_min,
                     state_interval_min=state_interval_min,
                     total_injury_duration_min=total_injury_duration_min)
        elapsed_time = timer() - start_time
        _log.info(f"Execution took {elapsed_time/60:.1f} min")

    def _triage(self, out_file: Path, tgt_id: int = None, skip_visited: bool = False,
                untreated_injury_time_min: int = 15, state_interval_min: int = 45, total_injury_duration_min: int = 60):
        self._tgt_id = tgt_id

        # Add a subdir to the output dir for this population size
        self._set_artifact_folder_name(out_file.stem)

        # Simulate the injuries and create states
        start_time = timer()
        self._generate_initial_injury_states(untreated_injury_time_min=untreated_injury_time_min,
                                             state_interval_min=state_interval_min,
                                             total_injury_duration_min=total_injury_duration_min)
        elapsed_time = timer() - start_time
        _log.info(f"It took {elapsed_time / 60:.1f} min to simulate injuries")

        # Triage all the injury states
        start_time = timer()
        self._triage_injured_states(out_file)
        elapsed_time = timer() - start_time
        _log.info(f"It took {elapsed_time / 60:.1f} min to triage injuries")

        with open(out_file, 'w') as f:
            json.dump(self._triage_study, f, indent=2)

        # Simulate intervened casualties
        start_time = timer()
        self._simulate_interventions(total_simulation_duration_min=total_injury_duration_min)
        elapsed_time = timer() - start_time
        _log.info(f"It took {elapsed_time / 60:.1f} min to simulate interventions")

        # Assess final casualty state after each visit
        start_time = timer()
        self._assess_interventions(duration_min=total_injury_duration_min)
        elapsed_time = timer() - start_time
        _log.info(f"It took {elapsed_time / 60:.1f} min to assess interventions")

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
        vitals["interventions"].clear()

        start_color, start_reason = self.start_tag(vitals)
        salt_color, salt_reason = self.salt_tag(vitals)
        bcd_color, bcd_reason = self.bcd_sieve_tag(vitals)
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
            "vitals_description": self._dataset.vitals_description(vitals)
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
                spec = data["specification"]
                if not spec["pulse"]:
                    continue

                s = SEScenario()
                s.set_name(f"Casualty_{i}")
                s.set_description("")
                if "state" in spec:
                    s.set_engine_state(spec["state"])
                    _log.info(f"Creating casualty {i}: {spec['state']}")
                else:
                    p = s.get_patient_configuration().get_patient()
                    p.set_sex(eSex.Male if spec["sex"] == "male" else eSex.Female)
                    p.set_name(f"Casualty_{i}")
                    p.get_age().set_value(spec["age"], TimeUnit.yr)
                    p.get_height().set_value(spec["height"], LengthUnit.cm)
                    p.get_body_mass_index().set_value(spec["bmi"])
                    # HR demographic range causing invalid patients
                    # p.get_heart_rate_baseline().set_value(sp["heart_rate"], FrequencyUnit.Per_min)
                    _log.info(f"Creating casualty {i}: "
                              f"{spec['sex']}-{spec['age']}yr-{spec['height']}cm-{spec['bmi']}bmi-{spec['heart_rate']}bpm")

                s.get_data_request_manager().set_data_requests(self._pulse_data.data_requests)
                s.get_data_request_manager().set_results_filename(f"{self._injury_outputs_dir}/"
                                                                  f"casualty_{i}/initial_injury.csv")

                injury_duration_min = 0.0
                # Add the injuries
                _log.info(f"Translating injuries to Pulse: {spec['injuries']}")
                for action in self._dataset.injury_actions(spec["injuries"]):
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
                while injury_duration_min < total_injury_duration_min:
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

        if len(casualty_states_exec_status) != self._num_pulse_casualties:
            _log.fatal(f"Number of scenarios executed ({len(casualty_states_exec_status)}) "
                       f"does not equal the number of triage study casualties ({self._num_pulse_casualties})")
            exit(1)
        for status in casualty_states_exec_status:
            sce = Path(status.get_scenario_filename()).parts[-2]
            i = int(sce[sce.find('_')+1:])
            self._triage_study[i]["injury_exec_status"] = _exec_status_to_dict(status)
            # TODO Check if runs were successful or not

    def _triage_injured_states(self, out_file: Path):
        _log.info("Triaging injured casualties")
        for i, casualty in self._triage_study.items():
            spec = casualty["specification"]
            if not spec["pulse"]:
                continue

            if self._tgt_id:
                if i != self._tgt_id:
                    continue

            if self.keep_triage:
                if ("injury_exec_status" in casualty and
                        "pulse_injuries" in casualty and
                        "visits" in casualty and ("final" in casualty or "death" in casualty)):
                    # Check to see if there is triage data already for this run
                    continue

            _log.info(f"Triaging casualty {i} - {casualty['specification']['injuries']}")
            injuries = spec["injuries"]
            exec_status = casualty["injury_exec_status"]
            # Pull the results from our exec status
            r = PulseEngineReprocessor(csv_files=[Path(exec_status["InitializationStatus"]["CSVFilename"])],
                                       log_files=[Path(exec_status["InitializationStatus"]["LogFilename"])])

            states = {}
            pulse_injuries = []
            # Get which casualty this is
            casualty_name = Path(exec_status["ScenarioFilename"]).parts[-2]
            with open(exec_status["ScenarioFilename"], 'r') as f:
                sce = json.load(f)

            # Get actions provided to the scenario, they are the Pulse injury actions
            for action in sce["AnyAction"]:
                if "AdvanceTime" in action:
                    continue
                if "SerializeState" in action:
                    # Our scenarios will have serialization actions with a comment containing injury duration
                    comment = action["SerializeState"]["Action"]["Comment"]
                    duration_min = float(comment[comment.find(':')+1:comment.find("min")].strip())
                    states[duration_min] = action["SerializeState"]["Filename"]
                else:
                    pulse_injuries.append(action)
            casualty["pulse_injuries"] = pulse_injuries
            final_time = next(reversed(states.keys()))

            # Check to see when/if the casualty died
            death_module = DeathCheckModule(r.patient.get_heart_rate_maximum().get_value(FrequencyUnit.Per_min))
            r.replay([death_module])
            if death_module.cause_of_death:
                _log.info(f"{casualty_name} cause of death: {death_module.cause_of_death} at {death_module.time_of_death}")
                # Grab some vitals from the time of death
                self._pulse_data.set_values(r.get_values_at_time(r.end_time_s-1))
                active_events = r.get_active_events_in_window(r.start_time_s, r.end_time_s)
                vitals = self._dataset.calculate_triage_vitals(spec, active_events, self._pulse_data)
                triage = self._death_triage(death_module.time_of_death/60, injuries, pulse_injuries, vitals)
                casualty["death"] = {"time": death_module.time_of_death/60,
                                 "cause": death_module.cause_of_death,
                                 "triage": triage}

            # dict of triage times of interest for this casualty to triage vitals
            casualty["visits"] = {}
            # Data needed for tagging protocols for every triage time for this casualty
            for time_min, injury_state in states.items():
                time_s = time_min * 60
                if death_module.time_of_death and time_s >= death_module.time_of_death:
                    continue
                self._pulse_data.set_values(r.get_values_at_time(time_s))
                # Get active events from the last minute of this triage time
                active_events = r.get_active_events_in_window(time_s - 60, time_s)

                vitals = self._dataset.calculate_triage_vitals(spec, active_events, self._pulse_data)
                start_color, start_reason = self.start_tag(vitals)
                salt_color, salt_reason = self.salt_tag(vitals)
                bcd_color, bcd_reason = self.bcd_sieve_tag(vitals)
                triage = {
                    "state": injury_state,
                    "vitals": vitals,
                    "tags": {"start": start_color,
                             "start_reason": start_reason,
                             "salt": salt_color,
                             "salt_reason": salt_reason,
                             "bcd_sieve": bcd_color,
                             "bcd_sieve_reason": bcd_reason},
                    "triss": self.calculate_triss_score(vitals),
                    "news": self.calculate_news_score(vitals),
                    "injury_description": self._dataset.injury_description(time_min, injuries, pulse_injuries, vitals),
                    "vitals_description": self._dataset.vitals_description(vitals)
                }
                casualty["visits"][time_min] = {"triage": triage}
            # Take the last visit out, and it will be our final state (no intervention)
            if "death" not in casualty and final_time in casualty["visits"]:
                final_visit = casualty["visits"].pop(final_time)
                final_visit["time"] = final_time
                casualty["final"] = final_visit

            if self.keep_triage:
                # Save out the study file to preserve our triage data
                with open(out_file, 'w') as f:
                    json.dump(self._triage_study, f, indent=2)

    @staticmethod
    def calculate_triss_score(vitals: dict):
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
    def calculate_news_score(vitals: dict):
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
    def start_tag(vitals: dict) -> (str, str):
        tag = TriageTag()

        if vitals["ambulatory"]:
            tag.apply(TriageColor.Green, "Casualty is ambulatory.")
            return tag.color, tag.reason

        if not vitals["breathing"]:
            if Intervention.RepositionAirway in vitals["interventions"]:
                tag.apply(TriageColor.Red, "Casualty was not breathing.\n"
                                           "Repositioning their airway resulted in spontaneous breathing.")
            else:
                tag.apply(TriageColor.Black, "Casualty is not breathing.\n"
                                             "Repositioning their airway did not help breathing.")

        if vitals["respiratory_rate"] > 30.0:
            tag.apply(TriageColor.Red, "Casualty respiratory rate greater than 30 breaths per minute.")

        if not vitals["healthy_capillary_refill_time"]:
            tag.apply(TriageColor.Red, "Casualty does not have a healthy capillary refill time.")

        if vitals["avpu"] == AVPU.Alert:
            tag.apply(TriageColor.Yellow, "Casualty is unable to walk, but can follow commands.")
        else:
            tag.apply(TriageColor.Red, "Casualty is unable to follow commands.")

        return tag.color, tag.reason

    @staticmethod
    def salt_tag(vitals: dict) -> (str, str):
        tag = TriageTag()

        if vitals["ambulatory"]:
            tag.apply(TriageColor.Green, "Casualty is ambulatory.")
            return tag.color, tag.reason

        survivable = vitals["survivable_injuries"]

        # For gathering ICL examples, keep the red/black survivable/not survivable descriptions the same
        # The only difference should be the NOT. That is looked for in the tagging icl logic

        # Is the casualty not breathing?
        if not vitals["breathing"]:
            if Intervention.RepositionAirway in vitals["interventions"]:
                tag.apply(TriageColor.Red, "Casualty was not breathing.\n"
                                           "Repositioning their airway resulted in spontaneous breathing.")
            else:
                tag.apply(TriageColor.Black, "Casualty is not breathing.\n"
                                             "Repositioning their airway did not result in spontaneous breathing.")

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

        # Does the casualty have a major hemorrhage?
        elif (vitals["visible_hemorrhage_severity"] >= 3 and
                Intervention.WoundPack not in vitals["interventions"] and
                Intervention.Tourniquet not in vitals["interventions"]):
            if survivable:
                tag.apply(TriageColor.Red, "Casualty has an uncontrolled major hemorrhage.\n"
                                           "Casualty is likely to survive these injuries.")
            else:
                tag.apply(TriageColor.Black, "Casualty has a uncontrolled major hemorrhage.\n"
                                             "Casualty is NOT likely to survive these injuries.")

        # Nothing too crazy...
        else:
            if vitals["major_injuries"]:
                tag.apply(TriageColor.Yellow, "Casualty injuries are major.")
            else:
                tag.apply(TriageColor.Green, "Casualty injuries are minor.")

        return tag.color, tag.reason

    @staticmethod
    def bcd_sieve_tag(vitals: dict) -> (str, str):
        tag = TriageTag()

        if vitals["ambulatory"]:
            tag.apply(TriageColor.Green, "Casualty is ambulatory.")
            return tag.color, tag.reason

        if vitals["visible_hemorrhage_severity"] >= 4:
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
            spec = casualty["specification"]
            if not spec["pulse"]:
                continue

            if self._tgt_id:
                if i != self._tgt_id:
                    continue
            for time_min, visit in casualty["visits"].items():
                if len(visit["triage"]["vitals"]["interventions"]) > 0:
                    visit["intervention"] = {}

        # Let's create a set of scenarios that apply protocol interventions to injured casualties
        # Scenarios will not be rerun if they are marked as complete in this json file
        # You will need to delete the exec_status.json file if you want to rerun scenarios already run
        # You could also edit exec_status to rerun particular scenarios
        if not self._intervention_exec_status_filename.exists():
            for i, casualty in self._triage_study.items():
                if self._tgt_id:
                    if i != self._tgt_id:
                        continue
                spec = casualty["specification"]
                if not spec["pulse"]:
                    continue
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
                    for action in self._dataset.injury_interventions(spec["injuries"],
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
            spec = casualty["specification"]
            if not spec["pulse"]:
                continue

            for time, visit in casualty["visits"].items():
                if "intervention" not in visit:
                    continue
                sce = Path(intervention_exec_status[v].get_scenario_filename()).parts[-2]
                sce_id = int(sce[sce.find('_') + 1:])
                if sce_id != i:
                    _log.error(f"Mismatch of intervention status for casualty {i}@{time}, found status for {sce_id}")
                    exit(1)
                visit["intervention"]["intervention_exec_status"] = _exec_status_to_dict(intervention_exec_status[v])
                v += 1
                # TODO Check if runs were successful or not
        self._total_interventions = v

    def _assess_interventions(self, duration_min: float):
        p = 0
        for i, casualty in self._triage_study.items():
            spec = casualty["specification"]
            if not spec["pulse"]:
                continue

            if self._tgt_id:
                if i != self._tgt_id:
                    continue

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
                    # Grab some vitals from the time of death
                    self._pulse_data.set_values(r.get_values_at_time(r.end_time_s - 1))
                    active_events = r.get_active_events_in_window(r.start_time_s, r.end_time_s)
                    vitals = self._dataset.calculate_triage_vitals(spec, active_events, self._pulse_data)
                    triage = self._death_triage(death_module.time_of_death / 60,
                                                casualty["specification"]["injuries"],
                                                casualty["pulse_injuries"], vitals)
                    intervention["death"] = {"time": death_module.time_of_death / 60,
                                             "cause": death_module.cause_of_death,
                                             "triage": triage}
                else:
                    self._pulse_data.set_values(r.get_values_at_time(r.end_time_s))
                    # Get active events from the last minute of this simulation
                    active_events = r.get_active_events_in_window(r.end_time_s - 60, r.end_time_s)
                    vitals = self._dataset.calculate_triage_vitals(spec, active_events, self._pulse_data)
                    intervention["vitals"] = vitals
                    start_color, start_reason = self.start_tag(vitals)
                    salt_color, salt_reason = self.salt_tag(vitals)
                    bcd_color, bcd_reason = self.bcd_sieve_tag(vitals)
                    tags = {"start": start_color,
                            "start_reason": start_reason,
                            "salt": salt_color,
                            "salt_reason": salt_reason,
                            "bcd_sieve": bcd_color,
                            "bcd_sieve_reason": bcd_reason}
                    intervention["tags"] = tags
                    intervention["triss"] = self.calculate_triss_score(vitals)
                    intervention["news"] = self.calculate_news_score(vitals)
                    intervention["injury_description"] = [f"Casualty has been waiting {duration_min} min for further care."]
                    intervention["vitals_description"] = [""]  # TODO Need to improve vitals to handle interventions


def main():
    logging.basicConfig(level=logging.INFO, format='%(asctime)s -  %(message)s', datefmt='%Y-%m-%d %H:%M:%S')
    parser = argparse.ArgumentParser(description="Execute ITM triage study")
    parser.add_argument(
        "-o", "--output_dir",
        type=Path,
        default="./test_results/itm/triage_study",
        help="Location to put all files related to this study"
    )
    # Example dataset related arguments
    dataset_group = parser.add_mutually_exclusive_group()
    dataset_group.add_argument(
        "-ex", "--example",
        action='store_true',
        help="Generate the example study file\n"
             "The population file will be written here: <output_dir>/populations/example_casualties.json\n"
             "The study file will be written here: <output_dir>/triage_study_example.json"
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
    parser.add_argument(
        "-kt", "--keep_triage",
        action='store_true',
        help="Keep the triage data preserved and only triage casualties who have not been visited.\n"
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
        action='store_true',
        help="Iterate np.random.normal to attempt to find passing injury severity distributions.\n"
             "This option is only used when -num is provided and a new file/dataset is created.\n"
             "Using this option can invalidate the representative accuracy of your dataset."
    )
    opts = parser.parse_args()
    output_dir = opts.output_dir
    output_dir.mkdir(parents=True, exist_ok=True)

    now = datetime.now().strftime("%Y_%m_%d-%H_%M")

    triage_study = TriageStudy(Dataset.Army, output_dir)
    if opts.example:
        file_handler = logging.FileHandler(output_dir / f"triage_study_pipeline_example-{now}.log", mode='w')
        file_handler.setFormatter(logging.Formatter('%(asctime)s - %(message)s'))
        file_handler.setLevel(logging.INFO)
        _log.addHandler(file_handler)

        triage_study.keep_triage = opts.keep_triage
        triage_study.injury_opts.force_valid_distributions = opts.force_injury_severity_distributions
        triage_study.injury_opts.max_percent_difference = opts.max_injury_severity_percent_difference
        triage_study.triage(num_casualties=0, tgt_id=opts.id, skip_visited=opts.skip_visited,
                            untreated_injury_time_min=5, state_interval_min=5, total_injury_duration_min=60)
    elif opts.num_casualties:
        file_handler = logging.FileHandler(output_dir/f"triage_study_pipeline_{opts.num_casualties}-{now}.log", mode='w')
        file_handler.setFormatter(logging.Formatter('%(asctime)s - %(message)s'))
        file_handler.setLevel(logging.INFO)
        _log.addHandler(file_handler)

        triage_study.keep_triage = opts.keep_triage
        triage_study.injury_opts.force_valid_distributions = opts.force_injury_severity_distributions
        triage_study.injury_opts.max_percent_difference = opts.max_injury_severity_percent_difference
        triage_study.triage(num_casualties=opts.num_casualties, tgt_id=opts.id, skip_visited=opts.skip_visited,
                            untreated_injury_time_min=15, state_interval_min=45, total_injury_duration_min=60)


if __name__ == "__main__":
    main()
