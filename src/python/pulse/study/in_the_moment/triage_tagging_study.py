# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import argparse
import logging
import PyPulse

from enum import Enum
from pathlib import Path
from typing import List

from army_dataset import injuries_to_actions as army_injuries_to_actions
from army_dataset import generate_dataset as generate_army_dataset

from pulse.cdm.engine import SEAdvanceTime, SEDataRequest, SESerializeState, eSwitch
from pulse.cdm.patient import eSex
from pulse.cdm.scenario import SEScenario, SEScenarioExecStatus
from pulse.cdm.scalars import FrequencyUnit, LengthUnit, PressureUnit, TimeUnit, VolumeUnit, VolumePerTimeUnit
from pulse.cdm.io.scenario import serialize_scenario_to_file, \
                                  serialize_scenario_exec_status_list_to_file, \
                                  serialize_scenario_exec_status_list_from_file
from pulse.engine.PulseEngineResults import PulseEngineReprocessor
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


_data_requests = [
    SEDataRequest.create_physiology_request("HeartRate", unit=FrequencyUnit.Per_min),
    SEDataRequest.create_physiology_request("CardiacOutput", unit=VolumePerTimeUnit.L_Per_min),
    SEDataRequest.create_physiology_request("ArterialPressure", unit=PressureUnit.mmHg),
    SEDataRequest.create_physiology_request("MeanArterialPressure", unit=PressureUnit.mmHg),
    SEDataRequest.create_physiology_request("SystolicArterialPressure", unit=PressureUnit.mmHg),
    SEDataRequest.create_physiology_request("DiastolicArterialPressure", unit=PressureUnit.mmHg),
    SEDataRequest.create_physiology_request("BloodVolume", unit=VolumeUnit.mL),
    SEDataRequest.create_physiology_request("RespirationRate", unit=FrequencyUnit.Per_min),
    SEDataRequest.create_physiology_request("EndTidalCarbonDioxidePressure", unit=PressureUnit.mmHg),
    SEDataRequest.create_physiology_request("OxygenSaturation"),
    SEDataRequest.create_physiology_request("PeripheralPerfusionIndex")
]


def generate_initial_injury_states(synthetic_patients: list, minimum_injury_time_min: float, output_dir: Path):
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

            # Add a bit of buffer to show patient baseline
            adv = SEAdvanceTime()
            adv.get_time().set_value(5, TimeUnit.s)
            s.get_actions().append(adv)

            # Add the injuries
            for action in army_injuries_to_actions(sp["injuries"]):
                s.get_actions().append(action)

            # Add a minimum amount of time until treatment can start
            adv = SEAdvanceTime()
            adv.get_time().set_value(minimum_injury_time_min, TimeUnit.min)
            s.get_actions().append(adv)

            # Finally, save out the state of this patient
            state = SESerializeState()
            state.set_filename(str(injury_states_dir / f"patient_{i}.json"))
            s.get_actions().append(state)
            f = f"{injury_scenarios_dir}/patient_{i}.json"

            # Write the scenario to disk
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
    injuries = synthetic_patient["injuries"]

    results = PulseEngineReprocessor(csv_files=[Path(exec_status.get_csv_filename())],
                                     log_files=[Path(exec_status.get_log_filename())])
    values = results.get_values_at_time(results.end_time_s)

    # Get active events from the last minute of the simulation
    active_events = results.get_active_events_in_window(results.end_time_s-60, results.end_time_s)

    # Get actions provided to the scenario
    # These are provided as a dict, and the key is the time provided
    # We may want to get the active actions in API format
    # It really depends on how much we need the action info here
    for time_s, actions in results.actions.items():
        for action in actions:
            _log.info(f"[{time_s}] {action.text}")

    cardiac_output_mL_Per_s = PyPulse.convert(values[2],
                                              VolumePerTimeUnit.L_Per_min.get_string(),
                                              VolumePerTimeUnit.mL_Per_s.get_string())

    # TODO figure out the data we need for all our tagging protocols

    # Unhealthy CRT > 2s = PPI < 0.3%
    healthy_capillary_refill_time = True
    if values[11] < 0.003:
        healthy_capillary_refill_time = False

    triage_vitals = {
        "ambulatory": True,
        "breathing": True,
        "visible_respiratory_distress": True,
        "visible_hemorrhage": True,
        "avpu": AVPU.Alert,
        "respiratory_rate": 11,
        "heart_rate": 72,
        "radial_pulse_present": True,
        "healthy_capillary_refill_time": healthy_capillary_refill_time,
        "spO2": values[10]
    }
    return triage_vitals


def start_protocol(triage_vitals):
    #  TODO Implement tagging algorithm
    #  TODO Create a list of intervention actions
    #  TODO   - ex. set airway obstruction or hemorrhage to 0

    return TriageTag.Green


def salt_protocol(triage_vitals):
    #  TODO Implement tagging algorithm
    #  TODO Create a list of intervention actions
    #  TODO   - ex. set airway obstruction or hemorrhage to 0

    return TriageTag.Green


def bcd_sieve_protocol(triage_vitals):
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
    exec_status = generate_initial_injury_states(synthetic_patients, minimum_injury_time_min=5, output_dir=output_dir)
    # Tag each patient
    for i in range(len(synthetic_patients)):
        triage_vitals = generate_triage_data(synthetic_patient=synthetic_patients[i], exec_status=exec_status[i])
        _log.info(f"START Tag: {start_protocol(triage_vitals)}")
        _log.info(f"SALT Tag: {salt_protocol(triage_vitals)}")
        _log.info(f"BCD Sieve Tag: {bcd_sieve_protocol(triage_vitals)}")


if __name__ == "__main__":
    main()
