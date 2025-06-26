# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import json
import logging
import numpy as np
import random
import re

from pathlib import Path
from typing import List

from pulse.cdm.scalars import FrequencyUnit, PressureUnit
from pulse.study.in_the_moment.triage_dataset import Breathing, Hemorrhage, AVPU, TriageDataset, PulseData
from triage_dataset_generation import (synthetic_population_generation, synthetic_injury_generation,
                                       calculate_population_error, calculate_injury_error,
                                       plot_population_error, plot_injury_error)

from pulse.cdm.engine import SEAction, eGate, eSide
from pulse.cdm.patient_actions import (SEAcuteRespiratoryDistressSyndromeExacerbation,
                                       SEAcuteStress, SEAirwayObstruction,
                                       SEBrainInjury, eBrainInjuryType,
                                       SEHemorrhage, eHemorrhage_Compartment,
                                       SEHemothorax, eLungCompartment, SETensionPneumothorax, eHemorrhage_Type)

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


def _camel_case_split(s: str):
    matches = re.finditer('.+?(?:(?<=[a-z])(?=[A-Z])|(?<=[A-Z])(?=[A-Z][a-z])|$)', s)
    return [m.group(0) for m in matches]


def _injury_list_to_dict(injuries: list):
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
    return injury_dict


class ArmyDataset(TriageDataset):

    def generate_dataset(self, population_size: int, output_dir: Path):
        # TODO Could get this generic enough to put in base class
        output_file = Path(f"{output_dir}/{population_size}_patients_with_injuries.json")
        if output_file.exists():
            with open(output_file, 'r') as file:
                synthetic_patients = json.load(file)
            return synthetic_patients

        # Generate it
        if population_size <= 0:
            patient_injuries = []
            max_steps = [0.9, 1.9, 2.9, 3.9, 5.0]

            def _patient_set(loc: str, typ: str, sev: List[float]):
                for s in sev:
                    patient_injuries.append([{"location": loc, "type": typ, "severity": s}])

            # This is our test dataset: Standard Male, with a spectrum of each injury (where applicable)
            # Head and Neck
            _patient_set("head_and_neck", "tbi", max_steps)
            _patient_set("head_and_neck", "airway_obstruction", max_steps)
            _patient_set("head_and_neck", "superficial", [1.0])
            # Thorax
            _patient_set("thorax", "fracture", max_steps)
            _patient_set("thorax", "hemothorax", max_steps)
            _patient_set("thorax", "hemorrhage", max_steps)
            _patient_set("thorax", "pneumothorax", max_steps)
            _patient_set("thorax", "pulmonary_contusion", max_steps)
            _patient_set("thorax", "spinal", max_steps)
            # Abdomen
            _patient_set("abdomen", "hemorrhage", max_steps)
            _patient_set("abdomen", "laceration_contusion", max_steps)
            # Extremities
            _patient_set("extremity", "burn_nerve", [1.0, 2.5, 3.5, 4.5])
            _patient_set("extremity", "contusion_sprain_strain", [1.0, 2.5, 3.5, 4.5])
            _patient_set("extremity", "fracture_dislocation", [1.0, 2.5, 3.5, 4.5])
            _patient_set("extremity", "hemorrhage", [1.0, 2.5, 3.5, 4.5])
            #  Multiple Injuries
            patient_injuries.append([{"location": "thorax", "type": "hemothorax", "severity": 2.85},
                                     {"location": "thorax", "type": "hemorrhage", "severity": 2.85}])
            patients = {"age": [], "state": []}
            for i in range(len(patient_injuries)):
                patients["age"].append(44.0)
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

    def injury_description(self,
                           duration_min: float,
                           injuries: List[dict],
                           actions: List[dict],
                           vitals: dict) -> List[str]:
        description = []

        # Check to see if the casualty can walk
        if vitals["ambulatory"]:
            description.append("The casualty is able to walk.")
        else:
            description.append("The casualty is unable to walk.")

        breathing_type = vitals["breathing"]["type"]
        if breathing_type == Breathing.Obstructed:
            if vitals["breathing"]["able_to_clear"]:
                description.append("The casualty's airway was obstructed by something,"
                                   "but you were able to clear it out.")
            else:
                description.append("The casualty's airway is obstructed by something and you are unable to correct it.")
        elif breathing_type == Breathing.Distressed:
            description.append("The patient breathing is distressed.")
        else:
            rr = vitals["respiratory_rate"]
            if rr < 12:
                description.append("The casualty is breathing slowly.")
            elif rr < 20:
                description.append("The casualty is breathing normally.")
            elif rr < 30:
                description.append("The casualty is breathing fast.")
            else:
                description.append("The casualty is breathing in very short and rapid breaths.")

        if vitals["avpu"] == AVPU.Alert:
            description.append("The casualty is alert.")
        elif vitals["avpu"] == AVPU.Voice:
            description.append("The casualty's eyes are closed, but they are responding to your voice.")
        elif vitals["avpu"] == AVPU.Pain:
            description.append("The casualty's eyes are closed and only respond when you pinch them.")
        else:  # AVPU.Unresponsive
            description.append("The casualty is unconscious and unresponsive to any stimuli.")

        # TODO Mental State, based on duration since injury?

        injury_dict = _injury_list_to_dict(injuries)
        for loc, types in injury_dict.items():
            for typ, severities in types.items():
                num = len(severities)
                if num == 1:
                    sev = severities[0]
                    min_sev = sev
                    max_sev = sev
                else:
                    min_sev = min(severities)
                    max_sev = max(severities)
                    sev = min_sev

                if loc == "head_and_neck":
                    if num > 1:
                        _log.error(f"Multiple injuries on the {loc}, is currently unsupported")
                        exit(1)
                    if typ == "tbi":
                        if vitals["avpu"] == AVPU.Alert:
                            if sev < 1:
                                description.append(f"The casualty is complaining of a headache.")
                            elif sev < 2:
                                description.append(f"The casualty says they have a headache and feel nauseous.")
                            elif sev < 3:
                                description.append(f"The casualty is complaining of a headache and is slurring their words.")
                            elif sev < 4:
                                description.append(f"The casualty has dilated pupils and has been vomiting.")
                            else:
                                _log.error("We shouldn't be here and alert....")
                                exit(1)
                        else:
                            if sev < 2:
                                _log.error("We shouldn't be here and NOT alert....")
                                exit(1)
                        continue

                    if typ == "airway_obstruction":
                        # Should be covered above in the breathing section
                        continue

                    if typ == "superficial":
                        description.append(f"The casualty has some {typ} wounds to their head and neck area.")
                        continue

                if loc == "thorax":
                    if num > 2:
                        _log.error(f"More than 2 {typ} injuries on the {loc}, is currently unsupported")
                        exit(1)

                    if typ == "fracture":
                        if max_sev < 1:
                            pass
                        elif sev < 2:
                            description.append(f"The casualty is complaining of chest pain.")
                        elif sev < 3:
                            description.append(f"The casualty is complaining of chest pain and is wheezing.")
                        elif sev < 4:
                            description.append(f"The casualty is in obvious chest pain and wheezing when breathing.")
                        else:
                            description.append(f"The casualty is in obvious chest pain and wheezing when breathing.")
                        continue

                    if typ == "hemorrhage":
                        # TODO Improve
                        if max_sev < 1:
                            pass
                        elif sev < 2:
                            description.append(f"The casualty is complaining of chest pain.")
                        elif sev < 3:
                            description.append(f"The casualty is complaining of chest pain and is wheezing.")
                        elif sev < 4:
                            description.append(f"The casualty is in obvious chest pain and wheezing when breathing.")
                        else:
                            description.append(f"The casualty is in obvious chest pain and wheezing when breathing.")
                        continue

                    if typ == "hemothorax":
                        # TODO Improve
                        if max_sev < 1:
                            pass
                        elif sev < 2:
                            description.append(f"The casualty is complaining of chest pain.")
                        elif sev < 3:
                            description.append(f"The casualty is complaining of chest pain and is wheezing.")
                        elif sev < 4:
                            description.append(f"The casualty is in obvious chest pain and wheezing when breathing.")
                        else:
                            description.append(f"The casualty is in obvious chest pain and wheezing when breathing.")
                        continue

                    if typ == "pneumothorax":
                        if num == 1:
                            # Defer to normal breathing description for <2
                            side = actions[0]['PatientAction']['TensionPneumothorax']['Side'].lower()
                            if 2 < sev < 3:
                                description.append(f"The {side} side of their chest seems to be moving less during breathing.")
                            elif sev < 4:
                                description.append(f"The casualty's {side} chest is showing clear signs of reduced expansion")
                            elif sev < 5:
                                description.append(f"The casualty's {side} chest is not moving when breathing.")
                            continue

                        elif num == 2:
                            if 2 < min_sev < 3:
                                description.append(
                                    f"Both side of their chest seems to be moving less during breathing.")
                            elif min_sev < 4:
                                description.append(
                                    f"Both sides of the casualty's chest are showing clear signs of reduced expansion")
                            elif min_sev < 5:
                                description.append(f"The casualty's chest is not moving when breathing.")
                            continue

                    if typ == "pulmonary_contusion":
                        if max_sev < 1:
                            pass
                        elif sev < 2:
                            description.append(f"The casualty is complaining of chest pain.")
                        elif sev < 3:
                            description.append(f"The casualty is complaining of chest pain and is wheezing.")
                        elif sev < 4:
                            description.append(f"The casualty is in obvious chest pain and wheezing when breathing.")
                        else:
                            description.append(f"The casualty is in obvious chest pain and wheezing when breathing.")
                        continue

                    if typ == "spinal":
                        # TODO Not sure how to describe this
                        continue

                if loc == "abdomen":
                    if num > 1:
                        _log.error(f"Multiple injuries on the {loc}, is currently unsupported")
                        exit(1)

                    if typ == "hemorrhage":
                        if sev < 1:
                            sev_mod = "minor"
                        elif sev < 2:
                            sev_mod = "mild"
                        elif sev < 3:
                            sev_mod = "substantial"
                        elif sev < 4:
                            sev_mod = "massive"
                        else:
                            sev_mod = "catastrophic"
                        description.append(f"The casualty has a {sev_mod} {typ} to their {loc}.")
                        continue

                    if typ == "laceration_contusion":
                        if sev < 1:
                            sev_mod = "minor"
                        elif sev < 2:
                            sev_mod = "mild"
                        elif sev < 3:
                            sev_mod = "substantial"
                        elif sev < 4:
                            sev_mod = "severe"
                        else:
                            sev_mod = "critical"
                        t = random.choice(typ.split('_'))
                        description.append(f"The casualty has a {sev_mod} {t} to their {loc}.")
                        continue

                if loc == "extremity":
                    if num > 1:
                        _log.error(f"Multiple injuries on the {loc}, is currently unsupported")
                        exit(1)

                    if typ == "hemorrhage":
                        cmpt = actions[0]["PatientAction"]["Hemorrhage"]["Compartment"]
                        cmpt = " ".join(_camel_case_split(cmpt)).lower()
                        if sev < 1:
                            description.append(f"The casualty has some minor bleeding on their {cmpt}.")
                        elif sev < 2:
                            sev_mod = "mild"
                            description.append(f"The casualty has {sev_mod} hemorrhage on their {cmpt}.")
                        elif sev < 3:
                            sev_mod = "substantial"
                            description.append(f"The casualty has {sev_mod} hemorrhage on their {cmpt}.")
                        elif sev < 4:
                            sev_mod = "severe"
                            description.append(f"The casualty has {sev_mod} hemorrhage on their {cmpt}.")
                        else:
                            sev_mod = "critical"
                            description.append(f"The casualty lost their {cmpt} and is hemorrhaging blood.")
                        continue

                    if typ == "fracture_dislocation":
                        t = random.choice(typ.split('_'))
                        if t == "fracture":
                            if sev < 1:
                                b = random.choice(["forearm", "lower leg", "hands"])
                            elif sev < 2:
                                b = random.choice(["forearm", "lower leg", "hands"])
                            elif sev < 3:
                                b = random.choice(["forearm", "lower leg", "hands"])
                            elif sev < 4:
                                b = random.choice(["forearm", "lower leg", "hands"])
                            else:
                                b = random.choice(["forearm", "lower leg", "hands"])
                            description.append(f"The casualty has a burns on their {b}")
                        else:  # dislocation
                            if sev < 1:
                                b = random.choice(["finger", "toe"])
                            elif sev < 2:
                                b = random.choice(["wrist", "jaw", "elbow"])
                            elif sev < 3:
                                b = "shoulder"
                            elif sev < 4:
                                b = random.choice(["knee", "ankle"])
                            else:
                                b = "hip"
                            description.append(f"The casualty has a dislocated {b}.")
                        continue

                    if typ == "contusion_sprain_strain":
                        if sev < 1:
                            sev_mod = "minor"
                        elif sev < 2:
                            sev_mod = "mild"
                        elif sev < 3:
                            sev_mod = "substantial"
                        elif sev < 4:
                            sev_mod = "severe"
                        else:
                            sev_mod = "critical"
                        t = random.choice(typ.split('_'))
                        description.append(f"The casualty has a {sev_mod} {t} to their {loc}.")
                        continue

                    if typ == "burn_nerve":
                        # TODO For now, just assume burn
                        t = "burn"  # random.choice(typ.split('_'))
                        if t == "burn":
                            if sev < 1:
                                b = random.choice(["forearm", "lower leg", "hands"])
                            elif sev < 2:
                                b = random.choice(["forearm", "lower leg", "hands"])
                            elif sev < 3:
                                b = random.choice(["forearm", "lower leg", "hands"])
                            elif sev < 4:
                                b = random.choice(["forearm", "lower leg", "hands"])
                            else:
                                b = random.choice(["forearm", "lower leg", "hands"])
                            description.append(f"The casualty has a burns on their {b}.")
                        else:  # nerve
                            # TODO Not sure what description we want for nerve...
                            pass
                        continue

                _log.error(f"Unsupported injury: {loc} {typ}")
                exit(1)

        return description

    def calculate_triage_vitals(self, synthetic_patient: dict, active_events: dict, pulse_data: PulseData):
        synthetic_injuries = synthetic_patient["injuries"]

        # Find the highest severity injury
        max_severity = 0
        for injury in synthetic_injuries:
            if injury['severity'] > max_severity:
                max_severity = injury['severity']

        # Breathing
        obstruction = False
        clearable_airway = True
        breathing = None
        for injury in synthetic_injuries:
            if injury["location"] == "head_and_neck" and injury["type"] == "airway_obstruction":
                obstruction = True
                clearable_airway = True if injury["severity"] >= 3.5 else False
                breathing = Breathing.Obstructed
        if not obstruction:
            if "" in active_events:
                breathing = Breathing.Distressed
            else:
                rr = pulse_data.get_rr(FrequencyUnit.Per_min)
                if rr < 1.0:
                    breathing = None
                elif rr < 12:
                    breathing = Breathing.Slow
                elif rr < 18:
                    breathing = Breathing.Normal
                else:
                    breathing = Breathing.Fast

        # Hemorrhage
        hemorrhage = None
        controllable_hemorrhage = False
        for injury in synthetic_injuries:
            if injury["type"] == "hemorrhage":
                if injury["location"] == "extremity":
                    controllable_hemorrhage = True
                if injury["severity"] >= 3:
                    hemorrhage = Hemorrhage.Major
                else:
                    hemorrhage = Hemorrhage.Minor

        # AVPU
        avpu = AVPU.Alert
        # Check max severity and oxygen partial pressure in the brain
        brain_o2_pp = pulse_data.get_brain_o2_pp(PressureUnit.mmHg)
        if max_severity == 5.0 or brain_o2_pp < 15:
            avpu = AVPU.Unresponsive
        elif max_severity == 4.0:
            if 15 <= brain_o2_pp <= 25:
                avpu = AVPU.Pain
            else:
                avpu = AVPU.Voice
        elif 25 < brain_o2_pp < 35:
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

        return {"age": synthetic_patient["age"],
                "avpu": avpu,
                "ambulatory": ambulatory,
                "breathing": {"type": breathing, "able_to_clear": clearable_airway},
                "healthy_capillary_refill_time": healthy_capillary_refill_time,
                "heart_rate": pulse_data.get_hr(FrequencyUnit.Per_min),
                "major_injuries": True if max_severity > 2 else False,
                "hemorrhage": {"type": hemorrhage, "controllable": controllable_hemorrhage},
                "respiratory_rate": pulse_data.get_rr(FrequencyUnit.Per_min),
                "spO2": pulse_data.get_spo2(),
                "systolic_pressure": pulse_data.get_systolic_pressure(PressureUnit.mmHg),
                "diastolic_pressure": pulse_data.get_diastolic_pressure(PressureUnit.mmHg)
                }

    def injury_actions(self, injuries: list) -> List[SEAction]:
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
        injury_dict = _injury_list_to_dict(injuries)

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
                        actions.append(hemorrhage)
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

    def can_perform_interventions(self, synthetic_injuries: list) -> bool:
        for injury in synthetic_injuries:
            if injury["location"] == "head_and_neck" and injury["type"] == "airway_obstruction":
                return True
            if injury["location"] == "extremity" and injury["type"] == "hemorrhage":
                return True
        return False

    def injury_interventions(self, synthetic_injuries: list, pulse_injuries: list, vitals: dict):
        interventions = []
        for injury in pulse_injuries:
            if "Hemorrhage" in injury["PatientAction"]:
                t = injury["PatientAction"]["Hemorrhage"]["Type"]
                if t == "External":
                    h = SEHemorrhage()
                    h.set_compartment(injury["PatientAction"]["Hemorrhage"]["Compartment"])
                    h.set_type(eHemorrhage_Type.External)
                    h.get_severity().set_value(0.05)
                    interventions.append(h)
                    _log.info("Applying tourniquet to external hemorrhage")
            elif "AirwayObstruction" in injury["PatientAction"]:
                ao = SEAirwayObstruction()
                ao.get_severity().set_value(injury["severity"]/1.5)
                interventions.append(ao)
                _log.info("Clearing airway obstruction")

        return interventions


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
