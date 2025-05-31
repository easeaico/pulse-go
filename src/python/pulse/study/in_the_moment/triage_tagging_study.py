# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import argparse
import json
import logging
import numpy as np

from pathlib import Path
from typing import List, TypeVar

from pulse.cdm.engine import SEAction, SEAdvanceTime, SEDataRequest, SESerializeState, eGate, eSide, eSwitch
from pulse.cdm.patient import eSex
from pulse.cdm.patient_actions import (SEAcuteRespiratoryDistressSyndromeExacerbation,
                                       SEAcuteStress, SEAirwayObstruction,
                                       SEBrainInjury, eBrainInjuryType,
                                       SEHemorrhage, eHemorrhage_Compartment,
                                       SEHemothorax, eLungCompartment, SETensionPneumothorax)
from pulse.cdm.scenario import SEScenario, SEScenarioExecStatus
from pulse.cdm.scalars import FrequencyUnit, LengthUnit, MassUnit, PressureUnit, TimeUnit, VolumeUnit, VolumePerTimeUnit
from pulse.cdm.io.scenario import serialize_scenario_to_file, \
                                  serialize_scenario_exec_status_list_to_file, \
                                  serialize_scenario_exec_status_list_from_file

from pulse.engine.PulseScenarioExec import PulseScenarioExec

_log = logging.getLogger("pulse")

_data_requests = [
    SEDataRequest.create_physiology_request("HeartRate", unit=FrequencyUnit.Per_min),
    SEDataRequest.create_physiology_request("CardiacOutput", unit=VolumePerTimeUnit.L_Per_min),
    SEDataRequest.create_physiology_request("ArterialPressure", unit=PressureUnit.mmHg),
    SEDataRequest.create_physiology_request("MeanArterialPressure", unit=PressureUnit.mmHg),
    SEDataRequest.create_physiology_request("SystolicArterialPressure", unit=PressureUnit.mmHg),
    SEDataRequest.create_physiology_request("DiastolicArterialPressure", unit=PressureUnit.mmHg),
    SEDataRequest.create_physiology_request("BloodVolume", unit=VolumeUnit.mL),
    SEDataRequest.create_physiology_request("OxygenSaturation"),
    SEDataRequest.create_physiology_request("EndTidalCarbonDioxidePressure", unit=PressureUnit.mmHg),
    SEDataRequest.create_physiology_request("RespirationRate", unit=FrequencyUnit.Per_min)
]


def injuries_to_actions(injuries: list) -> List[SEAction]:
    actions: List[SEAction] = []

    def to_pulse_severity(value: float,
                          min_input: float = 1.0, max_input: float = 5.0,
                          min_output: float = 0.0, max_output: float = 1.0) -> float:
        return (value - min_input) / (max_input - min_input) * (max_output - min_output) + min_output

    def get_action(action_class) -> any:
        for a in actions:
            if isinstance(a, action_class):
                return a
        actions.append(action_class())
        return actions[-1]

    # Collapse injuries to a dict:
    #  location -> type -> [severities]
    # This will make supporting polytraumas easier
    injury_dict = {}
    for i in injuries:
        if i["location"] not in injury_dict:
            injury_dict[i["location"]] = {}
        locations = injury_dict[i["location"]]
        if i["type"] not in locations:
            locations[i["type"]] = []
        locations[i["type"]].append(i["severity"])

    # Note, this is written with the assumption all injuries in list are at the same location

    for location, types in injury_dict.items():
        for t, severities in types.items():
            num = len(severities)

            if location == "head_and_neck":
                if num > 1:
                    _log.error(f"Multiple {t} injuries on the {location}, is currently unsupported")
                    exit(1)

                if t == "tbi":
                    tbi = SEBrainInjury()
                    tbi.get_severity().set_value(to_pulse_severity(severities[0]))
                    tbi_type = np.random.randint(0, 2)
                    if tbi_type == 0:
                        tbi.set_injury_type(eBrainInjuryType.Diffuse)
                    elif tbi_type == 1:
                        tbi.set_injury_type(eBrainInjuryType.LeftFocal)
                    elif tbi_type == 2:
                        tbi.set_injury_type(eBrainInjuryType.RightFocal)
                    actions.append(tbi)
                    continue

                elif t == "airway_obstruction":
                    obs = SEAirwayObstruction()
                    obs.get_severity().set_value(to_pulse_severity(severities[0]))
                    actions.append(obs)
                    continue

                elif t == "superficial":
                    stress = SEAcuteStress()
                    stress.get_severity().set_value(to_pulse_severity(severities[0],
                                                                      min_output=0.2,
                                                                      max_output=0.4))
                    actions.append(stress)

                    skin = SEHemorrhage()
                    skin.set_compartment(eHemorrhage_Compartment.Skin.value)
                    skin.get_severity().set_value(to_pulse_severity(severities[0]))
                    actions.append(skin)
                    continue

            if location == "thorax":
                if num > 2:
                    _log.error(f"More than 2 {t} injuries on the {location}, is currently unsupported")
                    exit(1)

                if t == "pneumothorax":
                    if num == 1:
                        pneumo = SETensionPneumothorax()
                        side = np.random.randint(0, 1)
                        if side == 0:
                            pneumo.set_side(eSide.Left)
                        elif side == 1:
                            pneumo.set_side(eSide.Right)
                        gate = np.random.randint(0, 1)
                        if gate == 0:
                            pneumo.set_type(eGate.Open)
                        elif gate == 1:
                            pneumo.set_type(eGate.Closed)
                        pneumo.get_severity().set_value(to_pulse_severity(severities[0]))
                        actions.append(pneumo)
                        continue
                    elif num == 2:
                        left = SETensionPneumothorax()
                        left.set_side(eSide.Left)
                        gate = np.random.randint(0, 1)
                        if gate == 0:
                            left.set_type(eGate.Open)
                        elif gate == 1:
                            left.set_type(eGate.Closed)
                        left.get_severity().set_value(to_pulse_severity(severities[0]))
                        actions.append(left)

                        right = SETensionPneumothorax()
                        right.set_side(eSide.Right)
                        gate = np.random.randint(0, 1)
                        if gate == 0:
                            right.set_type(eGate.Open)
                        elif gate == 1:
                            right.set_type(eGate.Closed)
                        right.get_severity().set_value(to_pulse_severity(severities[1]))
                        actions.append(right)
                        continue

                if t == "pulmonary_contusion":
                    if num == 1:
                        ards = SEAcuteRespiratoryDistressSyndromeExacerbation()
                        cmpt = np.random.randint(0, 1)
                        if cmpt == 0:
                            ards.get_severity(eLungCompartment.LeftLung).set_value(to_pulse_severity(severities[0]))
                        elif cmpt == 1:
                            ards.get_severity(eLungCompartment.RightLung).set_value(to_pulse_severity(severities[0]))
                        actions.append(ards)
                        continue
                    elif num == 2:
                        left = SEAcuteRespiratoryDistressSyndromeExacerbation()
                        left.get_severity(eLungCompartment.LeftLung).set_value(to_pulse_severity(severities[0]))
                        actions.append(left)

                        right = SEAcuteRespiratoryDistressSyndromeExacerbation()
                        right.get_severity(eLungCompartment.RightLung).set_value(to_pulse_severity(severities[1]))
                        actions.append(right)
                        continue

                if t == "hemothorax":
                    if num == 1:
                        hemo = SEHemothorax()
                        side = np.random.randint(0, 1)
                        if side == 0:
                            hemo.set_side(eSide.Left)
                        elif side == 1:
                            hemo.set_side(eSide.Right)
                        hemo.get_severity().set_value(to_pulse_severity(severities[0]))
                        actions.append(hemo)
                        continue
                    elif num == 2:
                        left = SEHemothorax()
                        left.set_side(eSide.Left)
                        left.get_severity().set_value(to_pulse_severity(severities[0]))
                        actions.append(left)

                        right = SEHemothorax()
                        right.set_side(eSide.Right)
                        right.get_severity().set_value(to_pulse_severity(severities[1]))
                        actions.append(right)
                        continue

                if t == "hemorrhage":
                    severity = severities[0]
                    if num == 2:
                        # Average the severities
                        severity += severities[1]
                        severity /= 2
                    skin = SEHemorrhage()
                    skin.set_compartment(eHemorrhage_Compartment.Skin.value)
                    skin.get_severity().set_value(to_pulse_severity(severities[0]))
                    actions.append(skin)

                    muscle = SEHemorrhage()
                    muscle.set_compartment(eHemorrhage_Compartment.Muscle.value)
                    muscle.get_severity().set_value(to_pulse_severity(severities[0]))
                    actions.append(muscle)
                    continue

                if t == "fracture" or t == "spinal":
                    # Going to keep adding stress severities and cap at 1
                    # Create/Grab a stress action
                    stress = get_action(SEAcuteStress)
                    severity = 0
                    if stress.has_severity():
                        severity = stress.get_severity().get_value()
                    for s in severities:
                        severity += to_pulse_severity(s,
                                                      min_output=0.2,
                                                      max_output=0.7)
                    if severity > 1.0:
                        severity = 1.0
                    stress.get_severity().set_value(severity)
                    continue

            if location == "abdomen":
                if num > 1:
                    _log.error(f"Multiple {t} injuries on the {location}, is currently unsupported")
                    exit(1)

                if t == "hemorrhage":
                    skin = SEHemorrhage()
                    skin.set_compartment(eHemorrhage_Compartment.Skin.value)
                    skin.get_severity().set_value(to_pulse_severity(severities[0]))
                    actions.append(skin)

                    muscle = SEHemorrhage()
                    muscle.set_compartment(eHemorrhage_Compartment.Muscle.value)
                    muscle.get_severity().set_value(to_pulse_severity(severities[0]))
                    actions.append(muscle)
                    continue

                if t == "laceration_contusion":
                    stress = SEAcuteStress()
                    stress.get_severity().set_value(to_pulse_severity(severities[0],
                                                                      min_output=0.2,
                                                                      max_output=0.4))
                    actions.append(stress)

                    skin = SEHemorrhage()
                    skin.set_compartment(eHemorrhage_Compartment.Skin.value)
                    skin.get_severity().set_value(to_pulse_severity(severities[0]))
                    actions.append(skin)

                    muscle = SEHemorrhage()
                    muscle.set_compartment(eHemorrhage_Compartment.Muscle.value)
                    muscle.get_severity().set_value(to_pulse_severity(severities[0]))
                    actions.append(muscle)
                    continue

            if location == "extremity":
                if num > 1:
                    _log.error(f"Multiple {t} injuries on the {location}, is currently unsupported")
                    exit(1)

                if t == "hemorrhage":
                    hemorrhage = SEHemorrhage()
                    cmpt = np.random.randint(0, 3)
                    if cmpt == 0:
                        hemorrhage.set_compartment(eHemorrhage_Compartment.LeftArm.value)
                    elif cmpt == 1:
                        hemorrhage.set_compartment(eHemorrhage_Compartment.LeftLeg.value)
                    elif cmpt == 2:
                        hemorrhage.set_compartment(eHemorrhage_Compartment.RightArm.value)
                    elif cmpt == 3:
                        hemorrhage.set_compartment(eHemorrhage_Compartment.RightLeg.value)
                    hemorrhage.get_severity().set_value(to_pulse_severity(severities[0]))
                    continue

                if t == "fracture_dislocation":
                    stress = SEAcuteStress()
                    stress.get_severity().set_value(to_pulse_severity(severities[0],
                                                                      min_output=0.2,
                                                                      max_output=0.7))
                    actions.append(stress)
                    continue

                if t == "contusion_sprain_strain":
                    stress = SEAcuteStress()
                    stress.get_severity().set_value(to_pulse_severity(severities[0],
                                                                      min_output=0.2,
                                                                      max_output=0.7))
                    actions.append(stress)
                    continue

                if t == "burn_nerve":
                    stress = SEAcuteStress()
                    stress.get_severity().set_value(to_pulse_severity(severities[0],
                                                                      min_output=0.2,
                                                                      max_output=0.7))
                    actions.append(stress)
                    continue

            _log.error(f"Unsupported injury: {location} {t}")

    return actions


def generate_initial_injury_states(synthetic_patients: list, output_dir: Path):
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
    # You will need to delete this file if you want to rerun this file (set of scenarios)
    if not injury_exec_status_filename.exists():
        for i, sp in enumerate(synthetic_patients):  # Just create a single patient, but loop more if you want more
            s = SEScenario()
            s.set_name(f"Patient_{i}")  # Result csv/log file will use this as its filename
            s.set_description("")
            p = s.get_patient_configuration().get_patient()
            p.set_sex(eSex.Male if sp["sex"] == "male" else eSex.Female)
            p.set_name(f"Patient_{i}")
            p.get_age().set_value(sp["age"], TimeUnit.yr)
            p.get_height().set_value(sp["height"], LengthUnit.cm)
            p.get_body_mass_index().set_value(sp["bmi"])
            p.get_heart_rate_baseline().set_value(sp["heart_rate"], FrequencyUnit.Per_min)

            # Add a bit of buffer to show patient baseline
            adv = SEAdvanceTime()
            adv.get_time().set_value(30, TimeUnit.s)
            s.get_actions().append(adv)

            # Add the injuries
            for action in injuries_to_actions(sp["injuries"]):
                s.get_actions().append(action)

            # Add a minimum amount of time until treatment can start
            adv = SEAdvanceTime()
            adv.get_time().set_value(30, TimeUnit.s)
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


def main():
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
    parser = argparse.ArgumentParser(description="Process the full pipeline for segment validation")
    parser.add_argument(
        "-f", "--filename",
        type=Path,
        default="./test_results/itm/data/army/1000_patients_with_injuries.json",
        help="Synthetic data generated json file"
    )
    parser.add_argument(
        "-o", "--output_dir",
        type=Path,
        default="./test_results/itm/triage_study",
        help="Location to put all files related to this study"
    )
    opts = parser.parse_args()
    if not opts.filename.exists():
        _log.error(f"{opts.filename} cannot be found. Please provide a valid synthetic data json file")

    with open(opts.filename, 'r') as file:
        synthetic_patients = json.load(file)
    output_dir = opts.output_dir
    output_dir.mkdir(parents=True, exist_ok=True)

    generate_initial_injury_states(synthetic_patients, output_dir)


if __name__ == "__main__":
    main()
