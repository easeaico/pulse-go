# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import argparse
import json
import logging

from enum import Enum
from pathlib import Path
from typing import List, NamedTuple, Dict

from army_dataset import ArmyDataset

from pulse.cdm.engine import SEAdvanceTime, SESerializeState, SEEventChange, \
                             eSwitch, eEvent, eSerializationFormat
from pulse.cdm.patient import eSex
from pulse.cdm.scenario import SEScenario, SEScenarioExecStatus
from pulse.cdm.scalars import FrequencyUnit, LengthUnit, TimeUnit
from pulse.cdm.io.scenario import serialize_scenario_to_file, \
                                  serialize_scenario_exec_status_list_to_file, \
                                  serialize_scenario_exec_status_list_from_file, \
                                  serialize_scenario_exec_status_to_string
from pulse.engine.PulseEngineResults import PulseEngineReprocessor, PulseResultsProcessor, PulseLogAction
from pulse.engine.PulseScenarioExec import PulseScenarioExec
from pulse.study.in_the_moment.triage_dataset import AVPU, TriageTag, Breathing, TriageColor, Hemorrhage, PulseData

_log = logging.getLogger("pulse")


class Dataset(int, Enum):
    Army = 0
    Navy = 1


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
        spO2 = data_slice[10]

        # Generally, you should process event/action changes every time step
        for event_change in event_changes:
            if event_change.event == eEvent.IrreversibleState and event_change.active:
                self._time_of_death = curr_time_s
                self._cause_of_death = f"Patient died from irreversible state at {curr_time_s}s"
                raise StopIteration(self._cause_of_death)

            if event_change.event == eEvent.CardiovascularCollapse and event_change.active:
                self._time_of_death = curr_time_s
                self._cause_of_death = f"Patient died from cardiovascular collapse at {curr_time_s}s"
                raise StopIteration(self._cause_of_death)

            if event_change.event == eEvent.BrainOxygenDeficit:
                if event_change.active:
                    if not self._brain_O2_deficit:
                        self._brain_O2_deficit = True
                        self._start_brain_O2_deficit_s = curr_time_s
                    elif (curr_time_s - self._start_brain_O2_deficit_s) > 180:
                        self._time_of_death = curr_time_s
                        self._cause_of_death = f"Patient died from brain O2 deficit of 180s at {curr_time_s}s"
                        raise StopIteration(self._cause_of_death)
                else:
                    self._brain_O2_deficit = False
                    self._start_brain_O2_deficit_s = 0

            if event_change.event == eEvent.MyocardiumOxygenDeficit:
                if event_change.active:
                    if not self._myocardium_O2_deficit:
                        self._myocardium_O2_deficit = True
                        self._start_myocardium_O2_deficit_s = curr_time_s
                    elif (curr_time_s - self._start_myocardium_O2_deficit_s) > 180:
                        self._time_of_death = curr_time_s
                        self._cause_of_death = f"Patient died from myocardium O2 deficit of 180s at {curr_time_s}s"
                        raise StopIteration(self._cause_of_death)
                else:
                    self._myocardium_O2_deficit = False
                    self._start_myocardium_O2_deficit_s = 0

        if hr_bpm >= self._max_hr_bpm:
            self._time_of_death = curr_time_s
            self._cause_of_death = f"Patient died from reaching max hr of {self._max_hr_bpm} at {curr_time_s}s"
            raise StopIteration(self._cause_of_death)

        if spO2 < 0.85:
            if not self._spO2_deficit:
                self._spO2_deficit = True
                self._start_spO2_deficit_s = curr_time_s
            elif (curr_time_s - self._start_spO2_deficit_s) > 140:
                self._time_of_death = curr_time_s
                self._cause_of_death = f"Patient died from SpO2 < 85 for 140s at {curr_time_s}s"
                raise StopIteration(self._cause_of_death)
        else:
            self._spO2_deficit = False


class TriageStudy:
    __slots__ = ["_output_dir", "_triage_study", "_dataset", "_pulse_data",
                 "_injury_scenarios_dir", "_injury_states_dir", "_injury_outputs_dir", "_injury_exec_status_filename",
                 "_intervention_scenarios_dir", "_intervention_outputs_dir", "_intervention_exec_status_filename",
                 "_total_interventions"]

    def __init__(self, dataset: Dataset, output_dir: Path):
        self._triage_study = {}
        self._output_dir = output_dir
        self._pulse_data = PulseData()
        # Directories and files associated with simulating injuries using Pulse
        self._injury_scenarios_dir = output_dir / "injuries/scenarios"
        self._injury_states_dir = output_dir / "injuries/states"
        self._injury_outputs_dir = output_dir / "injuries/outputs"
        self._injury_scenarios_dir.mkdir(parents=True, exist_ok=True)
        self._injury_states_dir.mkdir(parents=True, exist_ok=True)
        self._injury_outputs_dir.mkdir(parents=True, exist_ok=True)
        # This tracks the status of the execution of these scenarios
        self._injury_exec_status_filename = output_dir / "injuries/exec_status.json"
        # Directories and files associated with simulating interventions using Pulse
        self._intervention_scenarios_dir = output_dir / "interventions/scenarios"
        self._intervention_outputs_dir = output_dir / "interventions/outputs"
        self._intervention_scenarios_dir.mkdir(parents=True, exist_ok=True)
        self._intervention_outputs_dir.mkdir(parents=True, exist_ok=True)
        # This tracks the status of the execution of these scenarios
        self._intervention_exec_status_filename = output_dir / "interventions/exec_status.json"
        self._total_interventions = 0
        if dataset == Dataset.Army:
            self._dataset = ArmyDataset()
        else:
            raise NotImplementedError()

    @property
    def total_interventions(self): return self._total_interventions

    def analyze_population_size(self, population_size: int):
        self._triage_study = {}
        # Create synthetic patient file
        synthetic_patients = self._dataset.generate_dataset(population_size, self._output_dir/"synthetic")
        self._analyze_synthetic_patients(synthetic_patients)

    def analyze_population_file(self, population_file: Path):
        self._triage_study = {}
        if population_file.exists():
            with open(population_file, 'r') as file:
                synthetic_patients = json.load(file)
            self._analyze_synthetic_patients(synthetic_patients)
        else:
            _log.fatal(f"Specified population file does not exist: {population_file}")
            exit(1)

    def _analyze_synthetic_patients(self, synthetic_patients: dict):
        for i, sp in enumerate(synthetic_patients):
            self._triage_study[i] = {"synthetic_patient": sp}

        # Simulate the injuries and create states
        self._generate_initial_injury_states(untreated_injury_time_min=5,
                                             state_interval_min=5,
                                             total_injury_duration_min=60)
        # Triage all the injury states
        self._triage_injured_states()
        # Simulate triaged patients
        self._simulate_interventions(total_simulation_duration_min=60)
        # Assess final patient state after each visit
        self._assess_interventions()
        # Write out all the data we collected
        triage_study_file = self._output_dir/"triage_study.json"
        with open(triage_study_file, 'w') as f:
            json.dump(self._triage_study, f, indent=2)

    def _generate_initial_injury_states(self,
                                        untreated_injury_time_min: float,
                                        state_interval_min: float,
                                        total_injury_duration_min: float):
        executor = PulseScenarioExec()
        injury_scenarios: List[SEScenarioExecStatus] = []

        # Let's create a set of scenarios that create initial patient states
        # Scenarios will not be rerun if they are marked as complete in this json file
        # You will need to delete the exec_status.json file if you want to rerun scenarios already run
        # You could also edit exec_status to rerun particular scenarios
        if not self._injury_exec_status_filename.exists():
            for i, data in self._triage_study.items():
                sp = data["synthetic_patient"]
                s = SEScenario()
                s.set_name(f"Patient_{i}")
                s.set_description("")
                if "state" in sp:
                    s.set_engine_state(sp["state"])
                else:
                    p = s.get_patient_configuration().get_patient()
                    p.set_sex(eSex.Male if sp["sex"] == "male" else eSex.Female)
                    p.set_name(f"Patient_{i}")
                    p.get_age().set_value(sp["age"], TimeUnit.yr)
                    p.get_height().set_value(sp["height"], LengthUnit.cm)
                    p.get_body_mass_index().set_value(sp["bmi"])
                    p.get_heart_rate_baseline().set_value(sp["heart_rate"], FrequencyUnit.Per_min)

                s.get_data_request_manager().set_data_requests(self._pulse_data.data_requests)
                s.get_data_request_manager().set_results_filename(f"{self._injury_outputs_dir}/"
                                                                  f"patient_{i}/initial_injury.csv")

                # Add a bit of buffer to show patient baseline
                adv = SEAdvanceTime()
                adv.get_time().set_value(0.5, TimeUnit.min)
                s.get_actions().append(adv)

                injury_duration_min = 0.0
                # Add the injuries
                for action in self._dataset.injury_actions(sp["injuries"]):
                    s.get_actions().append(action)

                # Advance the minimum injury time
                adv = SEAdvanceTime()
                adv.get_time().set_value(untreated_injury_time_min, TimeUnit.min)
                s.get_actions().append(adv)
                injury_duration_min += adv.get_time().get_value(TimeUnit.min)

                state = SESerializeState()
                state.set_comment(f"Injury Duration: {injury_duration_min} min")
                state.set_filename(str(self._injury_states_dir/f"patient_{i}/injury@{injury_duration_min}min.json"))
                s.get_actions().append(state)

                # Simulate patient saving new states at specified intervals
                while injury_duration_min <= total_injury_duration_min:
                    adv = SEAdvanceTime()
                    adv.get_time().set_value(state_interval_min, TimeUnit.min)
                    s.get_actions().append(adv)
                    injury_duration_min += adv.get_time().get_value(TimeUnit.min)

                    state = SESerializeState()
                    state.set_comment(f"Injury Duration: {injury_duration_min} min")
                    state.set_filename(str(self._injury_states_dir/f"patient_{i}/injury@{injury_duration_min}min.json"))
                    s.get_actions().append(state)

                # Write the scenario to disk
                sce_path = Path(f"{self._injury_scenarios_dir}/patient_{i}")
                sce_path.mkdir(parents=True, exist_ok=True)
                f = f"{sce_path}/initial_injury.json"
                serialize_scenario_to_file(s, f)
                # Add this scenario to our exec status
                e = SEScenarioExecStatus()
                e.set_scenario_filename(f)
                injury_scenarios.append(e)

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
            # You can view the patient_states_exec to see what happened
            _log.fatal(f"Problem running {self._injury_exec_status_filename}")
            exit(1)
        # Read in the exec status and return it
        patient_states_exec_status: List[SEScenarioExecStatus] = []
        serialize_scenario_exec_status_list_from_file(str(self._injury_exec_status_filename),
                                                      patient_states_exec_status)

        if len(patient_states_exec_status) != len(self._triage_study):
            _log.fatal(f"Number of scenarios executed ({len(patient_states_exec_status)}) "
                       f"does not equal the number of triage study patients ({len(self._triage_study)})")
            exit(1)
        for i, status in enumerate(patient_states_exec_status):
            self._triage_study[i]["injury_exec_status"] = _exec_status_to_dict(status)

    def _triage_injured_states(self):
        for i, data in self._triage_study.items():
            _log.info(f"Triaging patient {i}")

            exec_status = data["injury_exec_status"]
            synthetic_patient = data["synthetic_patient"]
            synthetic_injuries = synthetic_patient["injuries"]

            # Pull the results from our exec status
            r = PulseEngineReprocessor(csv_files=[Path(exec_status["InitializationStatus"]["CSVFilename"])],
                                       log_files=[Path(exec_status["InitializationStatus"]["LogFilename"])])
            # Get which patient this is
            patient = Path(exec_status["ScenarioFilename"]).parts[-2]

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

            # Check to see when/if the patient died
            death_module = DeathCheckModule(r.patient.get_heart_rate_maximum().get_value(FrequencyUnit.Per_min))
            r.replay([death_module])
            if death_module.cause_of_death:
                _log.info(f"{patient} cause of death: {death_module.cause_of_death}")

            # dict of triage times of interest for this patient to triage vitals
            data["visits"] = {}
            # Data needed for tagging protocols for every triage time for this patient
            for time_s, injury_state in states.items():
                if death_module.time_of_death and death_module.time_of_death <= time_s:
                    triage = {
                        "state": injury_state,
                        "death": {"time_s": death_module.time_of_death,
                                  "cause": death_module.cause_of_death}
                    }
                else:
                    self._pulse_data.set_values(r.get_values_at_time(time_s))
                    # Get active events from the last minute of this triage time
                    active_events = r.get_active_events_in_window(time_s - 60, time_s)

                    vitals = self._dataset.calculate_triage_vitals(synthetic_patient, active_events, self._pulse_data)
                    triage = {
                        "state": injury_state,
                        "vitals": vitals,
                        "tags": {"start": self._start_tag(vitals),
                                 "salt": self._salt_tag(vitals),
                                 "bcd_sieve": self._bcd_sieve_tag(vitals)},
                        "triss": self._calculate_triss_score(synthetic_injuries, vitals),
                        "news": self._calculate_news_score(vitals),
                        "description": self._dataset.injury_description(time_s, synthetic_injuries, pulse_injuries, vitals)
                    }
                data["visits"][time_s] = {"triage": triage}

    @staticmethod
    def _calculate_triss_score(synthetic_injuries: list, vitals: dict):
        # https://www.mdapp.co/trauma-injury-severity-score-triss-calculator-277/

        # Age
        age = vitals["age"]
        age_index = 0
        if age > 55:
            age_index = 1

        # Glasgow Coma Scale
        gcs = 0
        gcs_code = 0
        avpu = vitals["avpu"]
        if avpu == AVPU.Unresponsive:
            # Assuming no eye-opening, no verbal response, flexor and extensor reactions
            gcs = 5
            gcs_code = 1
        elif avpu == AVPU.Pain:
            # Assuming eye-opening to pain stimulus,
            # Inappropriate words with no sentences
            # Movement toward pressure/pain
            gcs = 9
            gcs_code = 3
        elif avpu == AVPU.Voice:
            gcs = 14  # Assuming eye open to auditory stimulus, oriented responses, conscious obeying of motor commands
            gcs_code = 4
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
        if 29 < rr < 10:
            rr_code = 4
        elif rr >= 29:
            rr_code = 3
        elif 6 <= rr < 10:
            rr_code = 2
        elif 1 <= rr < 6:
            rr_code = 1
        else:
            rr_code = 0

        revised_trauma_score = gcs_code*0.9368 + sbp_code*0.7326 + rr_code*0.2908

        # TODO blunt and iss should be in the vitals calc
        iss = 0
        blunt = True  # blunt force injury
        for injury in synthetic_injuries:
            iss = iss + injury["severity"]
            if "Hemorrhage" in injury["type"]:
                blunt = False  # penetrating injury
        # TODO 3 locations for hemorrhage are they all blunt?

        if blunt:
            triss = -0.4499 + 0.8505*revised_trauma_score - 0.0835*iss - 1.7430*age_index
        else:
            triss = -2.5355 + 0.9934*revised_trauma_score - 0.0651*iss - 1.1360*age_index

        return triss

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
    def _start_tag(vitals: dict):
        tag = TriageTag()  # Starts off Green

        if vitals["ambulatory"]:
            return TriageColor.Green

        if vitals["breathing"]["type"] == Breathing.Obstructed:
            if vitals["breathing"]["able_to_clear"]:
                tag.apply(TriageColor.Red)
            else:
                tag.apply(TriageColor.Black)

        if vitals["respiratory_rate"] > 30.0:
            tag.apply(TriageColor.Red)

        if not vitals["healthy_capillary_refill_time"]:
            tag.apply(TriageColor.Red)

        if vitals["avpu"] == AVPU.Voice:
            tag.apply(TriageColor.Yellow)
        elif vitals["avpu"] == AVPU.Pain or vitals["avpu"] == AVPU.Unresponsive:
            tag.apply(TriageColor.Red)

        return tag.color

    @staticmethod
    def _salt_tag(vitals: dict):
        tag = TriageTag()  # Starts off Green

        if vitals["ambulatory"]:
            return TriageColor.Green

        if vitals["breathing"]["type"] == Breathing.Obstructed:
            if vitals["breathing"]["able_to_clear"]:
                tag.apply(TriageColor.Red)
            else:
                tag.apply(TriageColor.Black)
        elif vitals["breathing"]["type"] == Breathing.Distressed:
            tag.apply(TriageColor.Red)

        hemorrhage = vitals["hemorrhage"]["type"]
        if hemorrhage == Hemorrhage.Major and not vitals["hemorrhage"]["controllable"]:
            tag.apply(TriageColor.Red)

        # TODO How would we black tag via limited resources?

        # TODO Is this peripheral pulse?
        if not vitals["healthy_capillary_refill_time"]:
            tag.apply(TriageColor.Red)

        if vitals["avpu"] == AVPU.Pain or vitals["avpu"] == AVPU.Unresponsive:
            tag.apply(TriageColor.Red)

        if vitals["major_injuries"]:
            tag.apply(TriageColor.Yellow)

        return tag.color

    @staticmethod
    def _bcd_sieve_tag(vitals: dict):
        tag = TriageTag()  # Starts off Green

        hemorrhage = vitals["hemorrhage"]["type"]
        if hemorrhage == Hemorrhage.Major:
            tag.apply(TriageColor.Red)

        if vitals["ambulatory"]:
            return TriageColor.Green

        if vitals["breathing"]["type"] == Breathing.Obstructed:
            if not vitals["breathing"]["able_to_clear"]:
                tag.apply(TriageColor.Black)

        if vitals["avpu"] == AVPU.Pain or AVPU.Unresponsive:
            tag.apply(TriageColor.Red)

        if vitals["respiratory_rate"] > 23.0 or vitals["respiratory_rate"] < 12.0:
            tag.apply(TriageColor.Red)

        if vitals["heart_rate"] > 100:
            tag.apply(TriageColor.Red)

        tag.apply(TriageColor.Yellow)

        return tag.color

    def _simulate_interventions(self, total_simulation_duration_min: float):

        executor = PulseScenarioExec()
        intervention_scenarios: List[SEScenarioExecStatus] = []

        # Add an intervention dict to patients we can treat
        for i, patient in self._triage_study.items():
            if not self._dataset.can_perform_interventions(patient["synthetic_patient"]["injuries"]):
                continue
            for time_s, visit in patient["visits"].items():
                visit["intervention"] = {}

        # Let's create a set of scenarios that apply protocol interventions to injured patients
        # Scenarios will not be rerun if they are marked as complete in this json file
        # You will need to delete the exec_status.json file if you want to rerun scenarios already run
        # You could also edit exec_status to rerun particular scenarios
        if not self._intervention_exec_status_filename.exists():
            for i, patient in self._triage_study.items():
                for time_s, visit in patient["visits"].items():

                    # Only simulate the injuries we can perform interventions on
                    if "intervention" not in visit:
                        continue
                    _log.info(f"Performing interventions on patient {i}")

                    triage = visit["triage"]
                    s_fn = ("intervention" + triage["state"][triage["state"].rfind('@'):])
                    o_fn = s_fn.replace(".json", ".csv")

                    s = SEScenario()
                    s.set_name(f"Patient {i}")
                    s.set_description(f"Interventions for ")
                    s.set_engine_state(triage["state"])
                    s.get_data_request_manager().set_data_requests(self._pulse_data.data_requests)
                    s.get_data_request_manager().set_results_filename(f"{self._intervention_outputs_dir}"
                                                                      f"/patient_{i}/{o_fn}")
                    # Add interventions
                    for action in self._dataset.injury_interventions(patient["synthetic_patient"]["injuries"],
                                                                     patient["pulse_injuries"],
                                                                     triage["vitals"]):
                        s.get_actions().append(action)
                        # TODO add action action to our data structure

                    # Simulate the treated patient for an amount of time
                    adv = SEAdvanceTime()
                    adv.get_time().set_value(total_simulation_duration_min, TimeUnit.min)
                    s.get_actions().append(adv)
                    # Write out the scenario
                    sce_path = Path(f"{self._intervention_scenarios_dir}/patient_{i}/")
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
            # You can view the patient_states_exec to see what happened
            _log.fatal(f"Problem running {self._intervention_exec_status_filename}")
            exit(1)
        # Read in the exec status and return it
        intervention_exec_status: List[SEScenarioExecStatus] = []
        serialize_scenario_exec_status_list_from_file(str(self._intervention_exec_status_filename),
                                                      intervention_exec_status)

        v = 0
        for i, patient in self._triage_study.items():
            for time_s, visit in patient["visits"].items():
                if "intervention" not in visit:
                    continue
                visit["intervention"]["intervention_exec_status"] = _exec_status_to_dict(intervention_exec_status[v])
                v += 1
        self._total_interventions = v

    def _assess_interventions(self):
        p = 0
        for i, patient in self._triage_study.items():
            pulse_injuries = patient["pulse_injuries"]
            synthetic_patient = patient["synthetic_patient"]
            synthetic_injuries = synthetic_patient["injuries"]
            for time_s, visit in patient["visits"].items():
                if "intervention" not in visit:
                    continue

                intervention = visit["intervention"]
                p += 1
                _log.info(f"[{p}/{self._total_interventions}]"
                          f"Assessing patient {i} treated at time {time_s}")

                exec_status = intervention["intervention_exec_status"]

                # Pull the results from our exec status
                r = PulseEngineReprocessor(csv_files=[Path(exec_status["InitializationStatus"]["CSVFilename"])],
                                           log_files=[Path(exec_status["InitializationStatus"]["LogFilename"])])

                # Check to see when/if the patient died
                death_module = DeathCheckModule(
                    r.patient.get_heart_rate_maximum().get_value(FrequencyUnit.Per_min))
                r.replay([death_module])
                if death_module.cause_of_death:
                    _log.info(f"{i} cause of death: {death_module.cause_of_death}")
                    intervention["death"] = {"time_s": death_module.time_of_death,
                                             "cause": death_module.cause_of_death}
                else:
                    self._pulse_data.set_values(r.get_values_at_time(r.end_time_s))
                    # Get active events from the last minute of this simulation
                    active_events = r.get_active_events_in_window(r.end_time_s - 60, r.end_time_s)
                    vitals = self._dataset.calculate_triage_vitals(synthetic_patient, active_events, self._pulse_data)
                    intervention["vitals"] = vitals
                    tags = {"start": self._start_tag(vitals),
                            "salt": self._salt_tag(vitals),
                            "bcd_sieve": self._bcd_sieve_tag(vitals)}
                    intervention["tags"] = tags
                    intervention["triss"] = self._calculate_triss_score(synthetic_injuries, vitals)
                    intervention["news"] = self._calculate_news_score(vitals)


def main():
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
    parser = argparse.ArgumentParser(description="Process the full pipeline for segment validation")
    parser.add_argument(
        "-o", "--output_dir",
        type=Path,
        default="./test_results/itm/triage_study",
        help="Location to put all files related to this study"
    )
    parser.add_argument(
        "-p", "--population_file",
        type=Path,
        default=None,
        help="Location to put all files related to this study"
    )
    opts = parser.parse_args()
    output_dir = opts.output_dir
    output_dir.mkdir(parents=True, exist_ok=True)

    triage_study = TriageStudy(Dataset.Army, output_dir)
    if opts.population_file:
        triage_study.analyze_population_file(opts.population_file)
    else:
        triage_study.analyze_population_size(0)


if __name__ == "__main__":
    main()
