# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import json
import logging
import numpy as np

from pathlib import Path
from typing import List

from synthetic_data_generation import (synthetic_population_generation, synthetic_injury_generation,
                                       calculate_population_error, calculate_injury_error,
                                       plot_population_error, plot_injury_error)

from pulse.cdm.engine import SEAction, eGate, eSide
from pulse.cdm.patient_actions import (SEAcuteRespiratoryDistressSyndromeExacerbation,
                                       SEAcuteStress, SEAirwayObstruction,
                                       SEBrainInjury, eBrainInjuryType,
                                       SEHemorrhage, eHemorrhage_Compartment,
                                       SEHemothorax, eLungCompartment, SETensionPneumothorax)

_log = logging.getLogger("pulse")


population_distributions = {
    "heart_rate": {"mean": 72, "std": 11},
    "sex": {
              "male": {"height": {"mean": 177, "std": 7.1}, "bmi": {"mean": 26.4, "std": 3.4}},
              "female": {"percent": 15.4, "height": {"mean": 163.5, "std": 7.7}, "bmi": {"mean": 24.7, "std": 2.8}}
            },
    "age": {"bins": [18, 25, 30, 35, 40, 55],
            "percents": [41.6, 22.4, 15.3, 11.3, 9.4],
            "counts": [191975, 103628, 70783, 52055, 43215]}
}

injury_distributions = {  # Location -> Type -> Severity mean/std or explicit value/percent
    "head_and_neck": {"percent": 36.2, "severity_mean": 2.69, "types": {
        "tbi": {"percent": 22, "severity": {"mean": 3.5, "std": 0.25}},
        "airway_obstruction": {"percent": 18, "severity": {"mean": 4.0, "std": 0.25}},
        "superficial": {"percent": 60, "severity": {"values": [1.0], "percents": [100]}}
    }},
    "thorax": {"percent": 8.6, "severity_mean": 2.85, "polytrauma": {"max": 4, "mean": 2.3}, "types": {
        "fracture": {"percent": 51.2, "severity": {"mean": 2.85, "std": 0.25}},
        "hemothorax": {"percent": 30, "severity": {"mean": 2.85, "std": 0.25}},
        "hemorrhage": {"percent": 34.6, "severity": {"mean": 2.85, "std": 0.25}},
        "pneumothorax": {"percent": 51.8, "severity": {"mean": 2.85, "std": 0.25}},
        "pulmonary_contusion": {"percent": 50.2, "severity": {"mean": 2.85, "std": 0.25}},
        "spinal": {"percent": 14.6, "severity": {"mean": 2.85, "std": 0.25}}
    }},
    "abdomen": {"percent": 6.9, "severity_mean": 2.85, "types": {
        "hemorrhage": {"percent": 34.6, "severity": {"mean": 2.85, "std": 0.25}},
        "laceration_contusion": {"percent": 65.4, "severity": {"mean": 2.85, "std": 0.25}}
    }},
    "extremity": {"percent": 49.4, "severity_mean": 2.05, "types": {
        "burn_nerve": {"percent": 5, "severity": {"values": [1.0, 2.5, 3.5, 4.5], "percents": [56, 23, 17, 7]}},
        "contusion_sprain_strain": {"percent": 20, "severity": {"values": [1.0, 2.5, 3.5, 4.5], "percents": [56, 23, 17, 7]}},
        "fracture_dislocation": {"percent": 22, "severity": {"values": [1.0, 2.5, 3.5, 4.5], "percents": [56, 23, 17, 7]}},
        "hemorrhage": {"percent": 52, "severity": {"values": [1.0, 2.5, 3.5, 4.5], "percents": [56, 23, 17, 7]}}
    }}
}


def generate_dataset(population_size: int, output_dir: Path):
    # TODO Could get this generic enough to put in synthetic data generation
    output_file = Path(f"{output_dir}/{population_size}_patients_with_injuries.json")
    if output_file.exists():
        with open(output_file, 'r') as file:
            synthetic_patients = json.load(file)
        return synthetic_patients

    # Generate it
    if population_size <= 0:
        # This is our test dataset: Standard Male, with each of these injuries
        patient_injuries = [  # Single Injuries
                            [{"location": "head_and_neck", "type": "tbi", "severity": 3.5}],
                            [{"location": "head_and_neck", "type": "airway_obstruction", "severity": 4.0}],
                            [{"location": "head_and_neck", "type": "superficial", "severity": 1.0}],
                            [{"location": "thorax", "type": "fracture", "severity": 2.85}],
                            [{"location": "thorax", "type": "hemothorax", "severity": 2.85}],
                            [{"location": "thorax", "type": "hemorrhage", "severity": 2.85}],
                            [{"location": "thorax", "type": "pneumothorax", "severity": 2.85}],
                            [{"location": "thorax", "type": "pulmonary_contusion", "severity": 2.85}],
                            [{"location": "thorax", "type": "spinal", "severity": 2.85}],
                            [{"location": "abdomen", "type": "hemorrhage", "severity": 2.85}],
                            [{"location": "abdomen", "type": "laceration_contusion", "severity": 2.85}],
                            [{"location": "extremity", "type": "burn_nerve", "severity": 4.5}],
                            [{"location": "extremity", "type": "contusion_sprain_strain", "severity": 4.5}],
                            [{"location": "extremity", "type": "fracture_dislocation", "severity": 4.5}],
                            [{"location": "extremity", "type": "hemorrhage", "severity": 4.5}],
                            #  Multiple Injuries
                            [{"location": "thorax", "type": "hemothorax", "severity": 2.85},
                             {"location": "thorax", "type": "hemorrhage", "severity": 2.85}]
                            ]
        patients = {"state": []}
        for i in range(len(patient_injuries)):
            patients["state"].append("./states/StandardMale@0s.json")
    else:
        patients = synthetic_population_generation(population_size, population_distributions)
        patient_injuries = synthetic_injury_generation(population_size, injury_distributions)

        # Write out the error images for this generated dataset
        population_error = calculate_population_error(patients, population_distributions)
        plot_population_error(population_error, f"{output_dir}/population_of_{population_size}")

        injury_error = calculate_injury_error(patient_injuries, injury_distributions)
        plot_injury_error(injury_error, f"{output_dir}/injuries_of_population_of_{population_size}")

    # Check\count for injury combinations not currently supported in Pulse
    num_hemopneumothorax = 0
    for injuries in patient_injuries:
        if len(injuries) > 1:
            hemopneumothorax = 0
            for injury in injuries:
                if injury["type"] == "pneumothorax" or injury["type"] == "hemothorax":
                    hemopneumothorax += 1
            if hemopneumothorax >= 3:
                num_hemopneumothorax += 1
    if num_hemopneumothorax > 0:
        _log.warning(f"Found {num_hemopneumothorax} hemopneumothorax(s), "
                     f"Pulse currently does not support this type of injury")

    # Combine the patients and their injuries to a dict
    data = []
    for i in range(len(patient_injuries)):
        patient = {}
        for field, values in patients.items():
            patient[field] = values[i]
        patient["injuries"] = patient_injuries[i]
        data.append(patient)

    output_dir.mkdir(parents=True, exist_ok=True)
    with open(f"{output_dir}/{population_size}_patients_with_injuries.json", 'w') as f:
        json.dump(data, f, indent=2)

    return data


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

    # TODO this is written with the assumption all injuries in list are at the same location
    # TODO this is going to need a lot of work...

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


def main():
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

    # TODO argparse
    output_dir = Path("./test_results/itm/army")
    army_dir = Path(output_dir / "army")
    army_dir.mkdir(parents=True, exist_ok=True)

    # Test specific injury
    if False:
        test_injury(injury_distributions=army_injury_distributions["thorax"],
                    num_patients_injured=1000,
                    log=True)
    if False:
        for p in range(100, 10001, 100):
            max_error = 0
            for i in range(50):
                err = test_injury(injury_distributions=army_injury_distributions["abdomen"],
                                  num_patients_injured=p,
                                  log=False)
                if err > max_error:
                    max_error = err
            _log.info(f"Max Error of {max_error} for {p} patients")

    # Run a measurement study
    if False:
        # Measure error for various population sizes
        for p in [100, 500, 1000, 2000, 3000]:
            i = 25
            _log.info(f"Measuring error for a population size of {p} using {i} iterations")
            measure_error(iterations=i, population_size=p,
                          population_distributions=army_population_distributions,
                          injury_distributions=army_injury_distributions,
                          results_stem=f"{army_dir}/measurements/{p}")

    # Generate a data set
    if True:
        generate_dataset(population_size=2000, output_dir=output_dir)


if __name__ == "__main__":
    main()
