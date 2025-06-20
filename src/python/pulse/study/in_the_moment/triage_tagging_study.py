# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import argparse
import json
import logging

from enum import Enum
from pathlib import Path
from typing import List, NamedTuple, Dict

import PyPulse

from army_dataset import injuries_to_actions as army_injuries_to_actions
from army_dataset import generate_dataset as generate_army_dataset

from pulse.cdm.engine import SEAdvanceTime, SEDataRequest, SESerializeState, SEEventChange, \
                             eSwitch, eEvent, eSerializationFormat
from pulse.cdm.patient import eSex
from pulse.cdm.patient_actions import SEAirwayObstruction, SEHemorrhage, eHemorrhage_Type
from pulse.cdm.scenario import SEScenario, SEScenarioExecStatus
from pulse.cdm.scalars import FrequencyUnit, LengthUnit, PressureUnit, TimeUnit, VolumeUnit, VolumePerTimeUnit
from pulse.cdm.io.scenario import serialize_scenario_to_file, \
                                  serialize_scenario_exec_status_list_to_file, \
                                  serialize_scenario_exec_status_list_from_file, \
                                  serialize_scenario_exec_status_to_string
from pulse.engine.PulseEngineResults import PulseEngineReprocessor, PulseResultsProcessor, PulseLogAction
from pulse.engine.PulseScenarioExec import PulseScenarioExec

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


class PulseData:
    __slots__ = ["_values", "_data_requests"]

    def __init__(self):
        self._values = None
        # Make sure the accessor methods below are in sync with this order and units
        self._data_requests = [
            SEDataRequest.create_physiology_request("HeartRate", unit=FrequencyUnit.Per_min),
            SEDataRequest.create_physiology_request("CardiacOutput", unit=VolumePerTimeUnit.L_Per_min),
            SEDataRequest.create_physiology_request("ArterialPressure", unit=PressureUnit.mmHg),
            SEDataRequest.create_physiology_request("MeanArterialPressure", unit=PressureUnit.mmHg),
            SEDataRequest.create_physiology_request("SystolicArterialPressure", unit=PressureUnit.mmHg),
            SEDataRequest.create_physiology_request("DiastolicArterialPressure", unit=PressureUnit.mmHg),
            SEDataRequest.create_physiology_request("BloodVolume", unit=VolumeUnit.mL),
            SEDataRequest.create_physiology_request("TotalHemorrhageRate", unit=VolumePerTimeUnit.L_Per_min),
            SEDataRequest.create_physiology_request("RespirationRate", unit=FrequencyUnit.Per_min),
            SEDataRequest.create_physiology_request("EndTidalCarbonDioxidePressure", unit=PressureUnit.mmHg),
            SEDataRequest.create_physiology_request("OxygenSaturation"),
            SEDataRequest.create_physiology_request("PeripheralPerfusionIndex"),
            SEDataRequest.create_liquid_compartment_substance_request("BrainVasculature",
                                                                      "Oxygen", "PartialPressure",
                                                                      unit=PressureUnit.mmHg),
        ]

    @property
    def data_requests(self): return self._data_requests

    def set_values(self, values: list):
        self._values = values

    def _get_value(self, idx, dr_unit, to_unit):
        if to_unit == dr_unit:
            return self._values[idx]
        return PyPulse.convert(self._values[idx],
                               dr_unit.get_string(),
                               to_unit.get_string())

    def get_hr(self, unit: FrequencyUnit):
        return self._get_value(1, FrequencyUnit.Per_min, unit)

    def get_cardiac_output(self, unit: VolumePerTimeUnit):
        return self._get_value(2, VolumePerTimeUnit.L_Per_min, unit)

    def get_arterial_pressure(self, unit: PressureUnit):
        return self._get_value(3, PressureUnit.mmHg, unit)

    def get_map(self, unit: PressureUnit):
        return self._get_value(4, PressureUnit.mmHg, unit)

    def get_systolic_pressure(self, unit: PressureUnit):
        return self._get_value(5, PressureUnit.mmHg, unit)

    def get_diastolic_pressure(self, unit: PressureUnit):
        return self._get_value(6, PressureUnit.mmHg, unit)

    def get_blood_volume(self, unit: VolumeUnit):
        return self._get_value(7, VolumeUnit.mL, unit)

    def get_hemorrhage_rate(self, unit: VolumePerTimeUnit):
        return self._get_value(8, VolumePerTimeUnit.L_Per_min, unit)

    def get_rr(self, unit: FrequencyUnit):
        return self._get_value(9, FrequencyUnit.Per_min, unit)

    def get_etco2(self, unit: PressureUnit):
        return self._get_value(10, PressureUnit.mmHg, unit)

    def get_spo2(self):
        return self._values[11]

    def get_ppi(self):
        return self._values[12]

    def get_brain_o2_pp(self, unit: PressureUnit):
        return self._get_value(13, PressureUnit.mmHg, unit)


class TriageStudy:
    __slots__ = ["_output_dir", "_triage_study", "_pulse_data",
                 "_injury_scenarios_dir", "_injury_states_dir", "_injury_outputs_dir", "_injury_exec_status_filename",
                 "_intervention_scenarios_dir", "_intervention_outputs_dir", "_intervention_exec_status_filename",
                 "_total_visits"]

    def __init__(self, output_dir: Path):
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
        self._total_visits = 0

    @property
    def total_interventions(self): return self._total_visits

    def analyze_population_size(self, population_size: int):
        self._triage_study = {}
        # Create synthetic patient file
        synthetic_patients = generate_army_dataset(population_size, self._output_dir/"synthetic")
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
                                             total_simulation_duration_min=60)
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
                                        total_simulation_duration_min: float):
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

                current_time_min = 0.0
                # Add a bit of buffer to show patient baseline
                adv = SEAdvanceTime()
                adv.get_time().set_value(0.5, TimeUnit.min)
                s.get_actions().append(adv)
                current_time_min += adv.get_time().get_value(TimeUnit.min)

                # Add the injuries
                for action in army_injuries_to_actions(sp["injuries"]):
                    s.get_actions().append(action)

                # Advance the minimum injury time
                adv = SEAdvanceTime()
                adv.get_time().set_value(untreated_injury_time_min, TimeUnit.min)
                s.get_actions().append(adv)
                current_time_min += adv.get_time().get_value(TimeUnit.min)

                state = SESerializeState()
                state.set_filename(str(self._injury_states_dir/f"patient_{i}/injury@{current_time_min}min.json"))
                s.get_actions().append(state)

                # Simulate patient saving new states at specified intervals
                while current_time_min <= total_simulation_duration_min:
                    adv = SEAdvanceTime()
                    adv.get_time().set_value(state_interval_min, TimeUnit.min)
                    s.get_actions().append(adv)
                    current_time_min += adv.get_time().get_value(TimeUnit.min)

                    state = SESerializeState()
                    state.set_filename(str(self._injury_states_dir/f"patient_{i}/injury@{current_time_min}min.json"))
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
            synthetic_injuries = data["synthetic_patient"]["injuries"]

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
                        states[time_s] = action.data["SerializeState"]["Filename"]
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

                    vitals = self._calculate_triage_vitals(synthetic_injuries, active_events, self._pulse_data)
                    triage = {
                        "state": injury_state,
                        "vitals": vitals,
                        "tags": {"start": self._start_tag(synthetic_injuries, pulse_injuries, vitals),
                                 "salt": self._salt_tag(synthetic_injuries, pulse_injuries, vitals),
                                 "bcd_sieve": self._bcd_sieve_tag(synthetic_injuries, pulse_injuries, vitals)},
                        "triss": self._calculate_triss_score(synthetic_injuries, pulse_injuries, vitals),
                        "news": self._calculate_news_score(synthetic_injuries, pulse_injuries, vitals),
                        "description": self._generate_injury_description(synthetic_injuries, pulse_injuries, vitals)
                    }
                data["visits"][time_s] = {"triage": triage}

    @staticmethod
    def _calculate_triage_vitals(synthetic_injuries: list, active_events: dict, pulse_data: PulseData):
        # Breathing
        breathing = True
        if pulse_data.get_rr(FrequencyUnit.Per_min) < 1.0:
            breathing = False

        # Respiratory Distress
        respiratory_distress = False
        if eEvent.Tachypnea in active_events:
            respiratory_distress = True

        # Controlled Hemorrhage
        controlled_hemorrhage = False
        if pulse_data.get_hemorrhage_rate(VolumePerTimeUnit.L_Per_min) < 0.015:
            controlled_hemorrhage = True

        # AVPU
        avpu = AVPU.Alert
        # Find the highest severity injury
        max_severity = 0
        for injury in synthetic_injuries:
            if injury['severity'] > max_severity:
                max_severity = injury['severity']
        # Now check severity and oxygen partial pressure in the brain
        if max_severity == 5.0 or pulse_data.get_brain_o2_pp(PressureUnit.mmHg) < 15:
            avpu = AVPU.Unresponsive
        elif max_severity == 4.0:
            if 15 <= pulse_data.get_brain_o2_pp(PressureUnit.mmHg) <= 25:
                avpu = AVPU.Pain
            else:
                avpu = AVPU.Voice
        elif 25 < pulse_data.get_brain_o2_pp(PressureUnit.mmHg) < 35:
            avpu = AVPU.Voice

        # Ambulatory
        ambulatory = True
        if max_severity >= 2.5:
            ambulatory = False
        elif avpu != AVPU.Alert:
            ambulatory = False

        # Unhealthy CRT > 2s - we are associating with hypotension
        healthy_capillary_refill_time = True
        if pulse_data.get_map(PressureUnit.mmHg) < 60:
            healthy_capillary_refill_time = False

        return {"ambulatory": ambulatory,
                "breathing": breathing,
                "respiratory_distress": respiratory_distress,
                "controlled_hemorrhage": controlled_hemorrhage,
                "avpu": avpu,
                "respiratory_rate": pulse_data.get_rr(FrequencyUnit.Per_min),
                "heart_rate": pulse_data.get_hr(FrequencyUnit.Per_min),
                "healthy_capillary_refill_time": healthy_capillary_refill_time,
                }

    @staticmethod
    def _calculate_triss_score(synthetic_injuries: list, pulse_injuries: list, vitals: dict):
        # https://www.mdapp.co/trauma-injury-severity-score-triss-calculator-277/
        #TODO: Need the patient age
        #age
        age = 23
        age_index = 0
        if age > 55:
            age_index = 1

        #Glascow Coma Scale
        GCS = 0
        GCS_code = 0
        if vitals["avpu"] == AVPU.Unresponsive:
            GCS = 5 #Assuming no eye opening, no verbal response, flexor and extensor reactions
            GCS_code = 1
        elif vitals["avpu"] == AVPU.Pain:
            GCS = 9 #Assuming eye opening to pain stimulus, inappropriate words with no sentences, movement toward pressure/pain
            GCS_code = 3
        elif vitals["avpu"] == AVPU.Voice:
            GCS = 14 #Assuming eye open to auditory stimulus, oriented responses, conscious obeying of motor commands
            GCS_code = 4
        else: #ALERT
            GCS = 15 #Assuming spontaneous opening, oriented responses, conscious obeying of motor commands
            GCS_code = 4

        #Systolic Blood Pressure
        #TODO: Need the systolic blood pressure
        SBP = 110
        SBP_code = 0
        if SBP >= 89:
            SBP_code = 4
        elif SBP >= 76 and SBP < 89:
            SBP_code = 3
        elif SBP >=50 and SBP < 75:
            SBP_code = 2
        elif SBP >= 1 and SBP < 50:
            SBP_code = 1
        else:
            SBP_code = 0

        RR_code = 0
        if vitals["respiratory_rate"] > 29 and vitals["respiratory_rate"] < 10:
            RR_code = 4
        elif vitals["respiratory_rate"] >= 29:
            RR_code = 3
        elif vitals["respiratory_rate"] >= 6 and vitals["respiratory_rate"] < 10:
            RR_code = 2
        elif vitals["respiratory_rate"] >= 1 and vitals["respiratory_rate"] < 6:
            RR_code = 1
        else:
            RR_code = 0

        Revised_Trauma_Score = GCS_code*0.9368 + SBP_code*0.7326 + RR_code*0.2908

        ISS = 0
        blunt = True #blunt force injury
        for injury in synthetic_injuries:
            ISS = ISS + injury["severity"]
            if "Hemorrhage" in injury["type"]:
                blunt = False #penetrating injury

        if blunt:
            TRISS = -0.4499 + 0.8505*Revised_Trauma_Score - 0.0835*ISS - 1.7430*age_index
        else:
            TRISS = -2.5355 + 0.9934*Revised_Trauma_Score - 0.0651*ISS - 1.1360*age_index

        return TRISS

    @staticmethod
    def _calculate_news_score(synthetic_injuries: list, pulse_injuries: list, vitals: dict):
        # https://www.mdcalc.com/calc/1873/national-early-warning-score-news#next-steps
        return 0

    @staticmethod
    def _generate_injury_description(synthetic_injuries: list, pulse_injuries: list, vitals: dict):
        return ""

    @staticmethod
    def _generate_pulse_interventions(synthetic_injuries: list, pulse_injuries: list, vitals: dict):
        interventions = []
        # TODO Use some better logic here, Just turning off the major stuff
        for injury in pulse_injuries:
            if "Hemorrhage" in injury["PatientAction"]:
                t = injury["PatientAction"]["Hemorrhage"]["Type"]
                if t == "External":
                    h = SEHemorrhage()
                    h.set_compartment(injury["PatientAction"]["Hemorrhage"]["Compartment"])
                    h.set_type(eHemorrhage_Type.External)
                    h.get_severity().set_value(0)
            elif "AirwayObstruction" in injury["PatientAction"]:
                ao = SEAirwayObstruction()
                ao.get_severity().set_value(0)
                interventions.append(ao)

        return interventions

    @staticmethod
    def _start_tag(synthetic_injuries: list, pulse_injuries: list, vitals: dict):
        tag = TriageTag.Green
        if vitals["ambulatory"]:
            return tag

        tag = TriageTag.Yellow

        if not vitals["breathing"]:
            for injury in synthetic_injuries:
                if "AirwayObstruction" in injury["type"] and injury["severity"]<5:
                    tag = TriageTag.Red
                else:
                    tag = TriageTag.Black
                    return tag

        if vitals["respiratory_rate"] > 30.0:
            tag = TriageTag.Red

        if not vitals["healthy_capillary_refill_time"]:
            tag = TriageTag.Red

        if vitals["avpu"] != AVPU.Alert or vitals["avpu"] != AVPU.Voice:
            tag = TriageTag.Red

        return tag

    @staticmethod
    def _salt_tag(synthetic_injuries: list, pulse_injuries: list, vitals: dict):
        tag = TriageTag.Green
        if vitals["ambulatory"]:
            return tag

        if not vitals["breathing"]:
            for injury in synthetic_injuries:
                if "AirwayObstruction" in injury["type"] and injury["severity"] < 5:
                    tag = TriageTag.Red
                else:
                    tag = TriageTag.Black
                    return tag

        if not vitals["controlled_hemorrhage"]:
            for injury in synthetic_injuries:
                if "Extremity" in injury["location"] or injury["severity"] < 2:
                    if tag != TriageTag.Red:
                        tag = TriageTag.Yellow
                elif injury["severity"] > 4:
                    tag = TriageTag.Black
                    return tag
                else:
                    tag = TriageTag.Red

        if not vitals["healthy_capillary_refill_time"]:
            tag = TriageTag.Red

        if vitals["respiratory_distress"]:
            tag = TriageTag.Red

        if vitals["avpu"] == AVPU.Pain or AVPU.Unresponsive:
                tag = TriageTag.Red

        if tag == TriageTag.Green:
            for injury in synthetic_injuries:
                if injury["severity"] >= 2:
                    tag = TriageTag.Yellow

        return tag

    @staticmethod
    def _bcd_sieve_tag(synthetic_injuries: list, pulse_injuries: list, vitals: dict):
        tag = TriageTag.Green

        if vitals["ambulatory"]:
            return tag

        if not vitals["controlled_hemorrhage"]:
            tag = TriageTag.Red

        if not vitals["breathing"]:
            for injury in synthetic_injuries:
                if "AirwayObstruction" in injury["type"] and injury["severity"] < 5:
                    tag = TriageTag.Red
                else:
                    tag = TriageTag.Black
                    return tag

        if vitals["avpu"] == AVPU.Pain or AVPU.Unresponsive:
            tag = TriageTag.Red

        if vitals["respiratory_respiratory"] > 23.0 or vitals["respiratory_respirator"] < 12.0:
            tag = TriageTag.Red

        if vitals["heart_rate"] > 100:
            tag = TriageTag.Red

        tag = TriageTag.Yellow
        return tag

    def _simulate_interventions(self, total_simulation_duration_min: float):

        executor = PulseScenarioExec()
        intervention_scenarios: List[SEScenarioExecStatus] = []

        # Let's create a set of scenarios that apply protocol interventions to injured patients
        # Scenarios will not be rerun if they are marked as complete in this json file
        # You will need to delete the exec_status.json file if you want to rerun scenarios already run
        # You could also edit exec_status to rerun particular scenarios
        if not self._intervention_exec_status_filename.exists():
            for i, patient in self._triage_study.items():
                for time_s, visit in patient["visits"].items():
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
                    for action in self._generate_pulse_interventions(patient["synthetic_patient"]["injuries"],
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
                if "death" in visit["triage"]:
                    continue
                visit["intervention"] = {"intervention_exec_status": _exec_status_to_dict(intervention_exec_status[v])}
                v += 1
        self._total_visits = v

    def _assess_interventions(self):
        p = 0
        for i, patient in self._triage_study.items():
            pulse_injuries = patient["pulse_injuries"]
            synthetic_injuries = patient["synthetic_patient"]["injuries"]
            for time_s, visit in patient["visits"].items():
                intervention = visit["intervention"]
                p += 1
                _log.info(f"[{p}/{self._total_visits}]"
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
                    vitals = self._calculate_triage_vitals(synthetic_injuries, active_events, self._pulse_data)
                    intervention["vitals"] = vitals
                    tags = {"start": self._start_tag(synthetic_injuries, pulse_injuries, vitals),
                            "salt": self._salt_tag(synthetic_injuries, pulse_injuries, vitals),
                            "bcd_sieve": self._bcd_sieve_tag(synthetic_injuries, pulse_injuries, vitals)}
                    intervention["tags"] = tags
                    intervention["triss"] = self._calculate_triss_score(synthetic_injuries, pulse_injuries, vitals)
                    intervention["news"] = self._calculate_news_score(synthetic_injuries, pulse_injuries, vitals)


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

    triage_study = TriageStudy(output_dir)
    if opts.population_file:
        triage_study.analyze_population_file(opts.population_file)
    else:
        triage_study.analyze_population_size(0)


if __name__ == "__main__":
    main()
