# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import argparse
import logging

from enum import Enum
from pathlib import Path
from typing import List, NamedTuple, Dict

from army_dataset import injuries_to_actions as army_injuries_to_actions
from army_dataset import generate_dataset as generate_army_dataset

from pulse.cdm.engine import SEAdvanceTime, SEDataRequest, SESerializeState, eSwitch, SEEventChange, eEvent
from pulse.cdm.patient import eSex
from pulse.cdm.scenario import SEScenario, SEScenarioExecStatus
from pulse.cdm.scalars import FrequencyUnit, LengthUnit, PressureUnit, TimeUnit, VolumeUnit, VolumePerTimeUnit
from pulse.cdm.io.scenario import serialize_scenario_to_file, \
                                  serialize_scenario_exec_status_list_to_file, \
                                  serialize_scenario_exec_status_list_from_file
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
                self._cause_of_death = f"Patient died from irreversible state @{curr_time_s}s"
                raise StopIteration(self._cause_of_death)

            if event_change.event == eEvent.CardiovascularCollapse and event_change.active:
                self._time_of_death = curr_time_s
                self._cause_of_death = f"Patient died from cardiovascular collapse @{curr_time_s}s"
                raise StopIteration(self._cause_of_death)

            if event_change.event == eEvent.BrainOxygenDeficit:
                if event_change.active:
                    if not self._brain_O2_deficit:
                        self._brain_O2_deficit = True
                        self._start_brain_O2_deficit_s = curr_time_s
                    elif (curr_time_s - self._start_brain_O2_deficit_s) > 180:
                        self._time_of_death = curr_time_s
                        self._cause_of_death = f"Patient died from brain O2 deficit of 180s @{curr_time_s}s"
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
                        self._cause_of_death = f"Patient died from myocardium O2 deficit of 180s @{curr_time_s}s"
                        raise StopIteration(self._cause_of_death)
                else:
                    self._myocardium_O2_deficit = False
                    self._start_myocardium_O2_deficit_s = 0

        if hr_bpm >= self._max_hr_bpm:
            self._time_of_death = curr_time_s
            self._cause_of_death = f"Patient died from reaching max hr of {self._max_hr_bpm} @{curr_time_s}s"
            raise StopIteration(self._cause_of_death)

        if spO2 < 0.85:
            if not self._spO2_deficit:
                self._spO2_deficit = True
                self._start_spO2_deficit_s = curr_time_s
            elif (curr_time_s - self._start_spO2_deficit_s) > 140:
                self._time_of_death = curr_time_s
                self._cause_of_death = f"Patient died from SpO2 < 85 for 140s @{curr_time_s}s"
                raise StopIteration(self._cause_of_death)
        else:
            self._spO2_deficit = False


class Data(Enum):
    HeartRate_Per_min = 1,
    CardiacOutput_L_Per_min = 2,
    ArterialPressure_mmHg = 3,
    MeanArterialPressure_mmHg = 4,
    SystolicArterialPressure_mmHg = 5,
    DiastolicArterialPressure_mmHg = 6,
    BloodVolume_mL = 7,
    TotalHemorrhageRate_L_Per_min = 8,
    RespirationRate_Per_min = 9,
    EndTidalCarbonDioxidePressure_mmHg = 10,
    OxygenSaturation = 11,
    PeripheralPerfusionIndex = 12,
    BrainVasculatureO2_mmHg = 13,


_data_requests = [
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
                                                              "Oxygen", "PartialPressure", unit=PressureUnit.mmHg),
]


def generate_initial_injury_states(synthetic_patients: list, output_dir: Path,
                                   untreated_injury_time_min: float,
                                   state_interval_min: float,
                                   total_simulation_duration_min: float):
    executor = PulseScenarioExec()
    injury_scenarios: List[SEScenarioExecStatus] = []

    # Let's create a set of scenarios that create initial patient states
    injury_scenarios_dir = output_dir / "injured/scenarios"
    injury_states_dir = output_dir / "injured/states"
    injury_outputs_dir = output_dir / "injured/outputs"
    injury_scenarios_dir.mkdir(parents=True, exist_ok=True)
    injury_states_dir.mkdir(parents=True, exist_ok=True)
    injury_outputs_dir.mkdir(parents=True, exist_ok=True)
    # This tracks the status of the execution of these scenarios
    injury_exec_status_filename = output_dir / "injured/exec_status.json"

    # Scenarios will not be rerun if they are marked as complete in this json file
    # You will need to delete the exec_status.json file if you want to rerun scenarios already run
    # You could also edit exec_status to rerun particular scenarios
    if not injury_exec_status_filename.exists():
        for i, sp in enumerate(synthetic_patients):
            s = SEScenario()
            s.set_name(f"Patient_{i}")  # Result csv/log file will use this as its filename
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

            s.get_data_request_manager().set_data_requests(_data_requests)
            s.get_data_request_manager().set_results_filename(f"{injury_outputs_dir}/patient_{i}/initial_injury.csv")

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
            state.set_filename(str(injury_states_dir/f"patient_{i}/injury@{current_time_min}min.json"))
            s.get_actions().append(state)

            # Simulate patient saving new states at specified intervals
            while current_time_min <= total_simulation_duration_min:
                adv = SEAdvanceTime()
                adv.get_time().set_value(state_interval_min, TimeUnit.min)
                s.get_actions().append(adv)
                current_time_min += adv.get_time().get_value(TimeUnit.min)

                state = SESerializeState()
                state.set_filename(str(injury_states_dir/f"patient_{i}/injury@{current_time_min}min.json"))
                s.get_actions().append(state)

            # Write the scenario to disk
            sce_path = Path(f"{injury_scenarios_dir}/patient_{i}")
            sce_path.mkdir(parents=True, exist_ok=True)
            f = f"{sce_path}/initial_injury.json"
            serialize_scenario_to_file(s, f)
            # Add this scenario to our exec status
            e = SEScenarioExecStatus()
            e.set_scenario_filename(f)
            injury_scenarios.append(e)

        # Write out the exec status so we can run it
        serialize_scenario_exec_status_list_to_file(injury_scenarios,
                                                    str(injury_exec_status_filename))
    # Now run those scenarios (this will be quick if its already been run)
    # executor.set_thread_count(1)
    executor.set_log_to_console(eSwitch.Off)  # Output can get pretty busy...
    executor.set_output_root_directory(str(injury_outputs_dir))
    executor.set_scenario_exec_list_filename(str(injury_exec_status_filename))
    if not executor.execute_scenario():
        # You can view the patient_states_exec to see what happened
        _log.error(f"Problem running {injury_exec_status_filename}")
        exit(1)
    # Read in the exec status and return it
    patient_states_exec_status: List[SEScenarioExecStatus] = []
    serialize_scenario_exec_status_list_from_file(str(injury_exec_status_filename), patient_states_exec_status)

    return patient_states_exec_status


def generate_triage_data(synthetic_patient: dict, exec_status: SEScenarioExecStatus):
    triage = {"triage_time": {}}  # dict of triage vitals for all times of interest for this patient

    # Pull the results from our exec status
    results = PulseEngineReprocessor(csv_files=[Path(exec_status.get_csv_filename())],
                                     log_files=[Path(exec_status.get_log_filename())])
    # Get which patient this is
    patient = Path(exec_status.get_scenario_filename()).parts[-2]

    states = {}
    injury_actions = []
    # Get actions provided to the scenario, they are the Pulse injury actions
    # These are provided as a dict, and the key is the time provided
    # We may want to get the active actions in API format
    # It really depends on how much we need the action info here
    for time_s, actions in results.actions.items():
        for action in actions:
            if action.name == "SerializeState":
                states[time_s] = action.data["SerializeState"]["Filename"]
            else:
                injury_actions.append(action)
    # The demographic injury specification used for this patient
    demographic_injuries = synthetic_patient["injuries"]
    triage["injury_actions"] = injury_actions
    triage["demographic_injuries"] = demographic_injuries

    # Check to see when/if the patient died
    triage_module = DeathCheckModule(results.patient.get_heart_rate_maximum().get_value(FrequencyUnit.Per_min))
    results.replay([triage_module])
    if triage_module.cause_of_death:
        _log.info(f"{patient} cause of death: {triage_module.cause_of_death}")

    # Data needed for Tagging protocols
    for time_s, injury_state in states.items():
        if triage_module.time_of_death and triage_module.time_of_death <= time_s:
            continue  # Patient has died

        values = results.get_values_at_time(time_s)
        # Get active events from the last minute of this triage time
        active_events = results.get_active_events_in_window(time_s - 60, time_s)

        # Breathing
        breathing = True
        if values[Data.RespirationRate_Per_min] < 1.0:
            breathing = False

        # Respiratory Distress
        respiratory_distress = False
        if eEvent.Tachypnea in active_events:
            respiratory_distress = True

        # Controlled Hemorrhage
        controlled_hemorrhage = False
        if values[Data.TotalHemorrhageRate_L_Per_min] < 0.015:
            controlled_hemorrhage = True

        # AVPU
        avpu = AVPU.Alert
        # Find the highest severity injury
        max_severity = 0
        for injury in demographic_injuries:
            if injury['severity'] > max_severity:
                max_severity = injury['severity']
        # Now check severity and oxygen partial pressure in the brain
        if max_severity == 5.0 or values[Data.BrainVasculatureO2_mmHg] < 15:
            avpu = AVPU.Unresponsive
        elif max_severity == 4.0:
            if 15 <= values[Data.BrainVasculatureO2_mmHg] <= 25:
                avpu = AVPU.Pain
            else:
                avpu = AVPU.Voice
        elif 25 < values[Data.BrainVasculatureO2_mmHg] < 35:
            avpu = AVPU.Voice

        # Ambulatory
        ambulatory = True
        if max_severity >= 2.5:
            ambulatory = False
        elif avpu != AVPU.Alert:
            ambulatory = False

        # Unhealthy CRT > 2s - we are associating with hypotension
        healthy_capillary_refill_time = True
        if values[Data.MeanArterialPressure_mmHg] < 60:
            healthy_capillary_refill_time = False

        triage_time = {
            "vitals": {
                "ambulatory": ambulatory,
                "breathing": breathing,
                "respiratory_distress": respiratory_distress,
                "controlled_hemorrhage": controlled_hemorrhage,
                "avpu": avpu,
                "respiratory_rate": values[Data.RespirationRate_Per_min],
                "heart_rate": values[Data.HeartRate_Per_min],
                "healthy_capillary_refill_time": healthy_capillary_refill_time,
            },
            "state": injury_state
        }
        triage["triage_time"][time_s] = triage_time

    return patient, triage


def start_protocol(patient_triage: dict):
    #  TODO Implement tagging algorithm
    #  TODO Create a list of intervention actions
    #  TODO   - ex. set airway obstruction or hemorrhage to 0

    return TriageTag.Green


def salt_protocol(patient_triage: dict):
    #  TODO Implement tagging algorithm
    #  TODO Create a list of intervention actions
    #  TODO   - ex. set airway obstruction or hemorrhage to 0

    return TriageTag.Green


def bcd_sieve_protocol(patient_triage: dict):
    #  TODO Implement tagging algorithm
    #  TODO Create a list of intervention actions
    #  TODO   - ex. set airway obstruction or hemorrhage to 0

    return TriageTag.Green


def main():
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
    parser = argparse.ArgumentParser(description="Process the full pipeline for segment validation")
    parser.add_argument(
        "-ps", "--population_size",
        type=int,
        default=0,
        help="Population size"
    )
    parser.add_argument(
        "-o", "--output_dir",
        type=Path,
        default="./test_results/itm/triage_study",
        help="Location to put all files related to this study"
    )
    opts = parser.parse_args()
    output_dir = opts.output_dir
    output_dir.mkdir(parents=True, exist_ok=True)

    # Load/generate the synthetic data set
    synthetic_patients = generate_army_dataset(opts.population_size, output_dir/"synthetic")
    # Simulate the injuries and create states
    exec_status = generate_initial_injury_states(synthetic_patients, output_dir=output_dir,
                                                 untreated_injury_time_min=5,
                                                 state_interval_min=5,
                                                 total_simulation_duration_min=60)
    # Tag each patient
    for i in range(len(synthetic_patients)):
        _log.info(f"Triaging patient {i}")
        patient, triage = generate_triage_data(synthetic_patient=synthetic_patients[i], exec_status=exec_status[i])
        _log.info(f"START Tag: {start_protocol(triage)}")
        _log.info(f"SALT Tag: {salt_protocol(triage)}")
        _log.info(f"BCD Sieve Tag: {bcd_sieve_protocol(triage)}")


if __name__ == "__main__":
    main()
