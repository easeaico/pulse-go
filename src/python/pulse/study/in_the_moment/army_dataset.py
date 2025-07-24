# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import logging
import math

import numpy as np
import re
import statistics

from pathlib import Path
from typing import List

from pulse.cdm.physiology import eHeartRhythm
from pulse.cdm.scalars import FrequencyUnit, PressureUnit
from pulse.cdm.utils.math_utils import percent_difference
from pulse.study.in_the_moment.triage_dataset import Hemorrhage, AVPU, TriageDataset, PulseData, Intervention
from casualty_generation import (casualty_population_generation, population_injury_generation,
                                 test_injury, measure_error, _bounded_random_normal, to_specification_lists,
                                 to_severity_lists, InjurySeverityOpts)

from pulse.cdm.enums import eGate, eSide, eSwitch
from pulse.cdm.engine import SEAction, eEvent
from pulse.cdm.patient_actions import (SEAcuteRespiratoryDistressSyndromeExacerbation,
                                       SEAcuteStress, SEAirwayObstruction,
                                       SEBrainInjury, eBrainInjuryType,
                                       SEHemorrhage, eHemorrhage_Compartment,
                                       SEHemothorax, eLungCompartment, SETensionPneumothorax, eHemorrhage_Type,
                                       SENeedleDecompression, SEChestOcclusiveDressing)

_log = logging.getLogger("pulse")


population_distributions = {
    "heart_rate": {"mean": 72, "std": 11},
    "sex": {
              "male": {"percent": 84.6, "height": {"mean": 177, "std": 7.1}, "bmi": {"mean": 26.4, "std": 3.4}},
              "female": {"percent": 15.4, "height": {"mean": 163.5, "std": 7.7}, "bmi": {"mean": 24.7, "std": 2.8}}
            },
    "age": {"bins": [18, 25, 30, 35, 40, 55],
            "percents": [41.6, 22.4, 15.3, 11.3, 9.4],
            "counts": [191975, 103628, 70783, 52055, 43215]}
}

injury_distributions = {  # Location -> Type -> Severity mean/std or explicit value/percent
    "head_and_neck": {"percent": 36.2, "severity_mean": 2.69, "types": {
        "airway_obstruction": {"percent": 18, "severity": {"mean": 4.0, "std": 0.70}},
        "superficial": {"percent": 60, "severity": {"values": [1.0], "percents": [100]}},
        "tbi": {"percent": 22, "severity": {"mean": 3.5, "std": 0.65}}
    }},
    "thorax": {"percent": 8.6, "severity_mean": 2.85, "polytrauma": {"max": 4, "mean": 2.3}, "types": {
        "fracture": {"percent": 51.2, "severity": {"mean": 2.85, "std": 0.85}, "max": 2},
        "hemothorax": {"percent": 30, "severity": {"mean": 2.85, "std": 0.85}, "max": 2},
        "hemorrhage": {"percent": 34.6, "severity": {"mean": 2.85, "std": 0.85}, "max": 2},
        "pneumothorax": {"percent": 51.8, "severity": {"mean": 2.85, "std": 0.85}, "max": 2},
        "pulmonary_contusion": {"percent": 50.2, "severity": {"mean": 2.85, "std": 0.85}, "max": 2},
        "spinal": {"percent": 14.6, "severity": {"mean": 2.85, "std": 0.25}, "max": 1}
    }},
    "abdomen": {"percent": 6.9, "severity_mean": 2.85, "types": {
        "hemorrhage": {"percent": 34.6, "severity": {"mean": 2.85, "std": 0.85}},
        "laceration_contusion": {"percent": 65.4, "severity": {"mean": 2.85, "std": 0.85}}
    }},
    "extremity": {"percent": 49.4, "severity_mean": 2.05, "types": {
        "burn_nerve": {"percent": 5, "severity": {"values": [2.0, 3.0, 4.0, 5.0], "percents": [56, 23, 17, 7]}},
        "contusion_sprain_strain": {"percent": 20, "severity": {"values": [2.0, 3.0, 4.0, 5.0], "percents": [56, 23, 17, 7]}},
        "fracture_dislocation": {"percent": 22, "severity": {"values": [2.0, 3.0, 4.0, 5.0], "percents": [56, 23, 17, 7]}},
        "hemorrhage": {"percent": 52, "severity": {"values": [2.0, 3.0, 4.0, 5.0], "percents": [56, 23, 17, 7]}}
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
            locations[i["type"]] = {"severities": [], "sub_types": [], "cmpts": []}
        locations[i["type"]]["severities"].append(i["severity"])
        locations[i["type"]]["sub_types"].append(i["sub_type"])
        locations[i["type"]]["cmpts"].append(i["cmpt"])
    return injury_dict


class ArmyDataset(TriageDataset):

    def generate_dataset(self, population_size: int, injury_opts: InjurySeverityOpts = None):

        if population_size <= 0:
            # This is our example dataset: Standard Male, with all possible severities for each injury
            _log.info("Creating example dataset")
            casualty_injuries = []

            def _add_injury(loc: str, typ: str, sev: float, intervene: bool, styp: str = None, cmpt: str = None):
                casualty_injuries.append([{"location": loc, "type": typ, "sub_type": styp, "cmpt": cmpt,
                                           "severity": sev, "can_intervene": intervene}])

            # Head and Neck

            _add_injury(loc="head_and_neck", typ="airway_obstruction", sev=1.0, intervene=True)
            _add_injury(loc="head_and_neck", typ="airway_obstruction", sev=2.0, intervene=True)
            _add_injury(loc="head_and_neck", typ="airway_obstruction", sev=3.0, intervene=True)
            _add_injury(loc="head_and_neck", typ="airway_obstruction", sev=4.0, intervene=True)
            _add_injury(loc="head_and_neck", typ="airway_obstruction", sev=5.0, intervene=True)
            _add_injury(loc="head_and_neck", typ="airway_obstruction", sev=5.0, intervene=False)
            _add_injury(loc="head_and_neck", typ="airway_obstruction", sev=6.0, intervene=True)
            _add_injury(loc="head_and_neck", typ="airway_obstruction", sev=6.0, intervene=False)

            _add_injury(loc="head_and_neck", typ="superficial", sev=1.0, intervene=False)

            _add_injury(loc="head_and_neck", typ="tbi", sev=1.0, intervene=False)
            _add_injury(loc="head_and_neck", typ="tbi", sev=2.0, intervene=False)
            _add_injury(loc="head_and_neck", typ="tbi", sev=3.0, intervene=False)
            _add_injury(loc="head_and_neck", typ="tbi", sev=4.0, intervene=False)
            _add_injury(loc="head_and_neck", typ="tbi", sev=5.0, intervene=False)
            _add_injury(loc="head_and_neck", typ="tbi", sev=6.0, intervene=False)

            # Thorax

            _add_injury(loc="thorax", typ="fracture", sev=1.0, intervene=False)
            _add_injury(loc="thorax", typ="fracture", sev=2.0, intervene=False)
            _add_injury(loc="thorax", typ="fracture", sev=3.0, intervene=False)
            _add_injury(loc="thorax", typ="fracture", sev=4.0, intervene=False)
            _add_injury(loc="thorax", typ="fracture", sev=5.0, intervene=False)
            _add_injury(loc="thorax", typ="fracture", sev=6.0, intervene=False)

            _add_injury(loc="thorax", typ="hemothorax", sev=1.0, intervene=False)
            _add_injury(loc="thorax", typ="hemothorax", sev=2.0, intervene=False)
            _add_injury(loc="thorax", typ="hemothorax", sev=3.0, intervene=False)
            _add_injury(loc="thorax", typ="hemothorax", sev=4.0, intervene=False)
            _add_injury(loc="thorax", typ="hemothorax", sev=5.0, intervene=False)
            _add_injury(loc="thorax", typ="hemothorax", sev=6.0, intervene=False)

            _add_injury(loc="thorax", typ="hemorrhage", sev=1.0, intervene=False)
            _add_injury(loc="thorax", typ="hemorrhage", sev=2.0, intervene=False)
            _add_injury(loc="thorax", typ="hemorrhage", sev=3.0, intervene=False)
            _add_injury(loc="thorax", typ="hemorrhage", sev=4.0, intervene=False)
            _add_injury(loc="thorax", typ="hemorrhage", sev=5.0, intervene=False)
            _add_injury(loc="thorax", typ="hemorrhage", sev=6.0, intervene=False)

            _add_injury(loc="thorax", typ="pneumothorax", sev=1.0, intervene=True)
            _add_injury(loc="thorax", typ="pneumothorax", sev=2.0, intervene=True)
            _add_injury(loc="thorax", typ="pneumothorax", sev=3.0, intervene=True)
            _add_injury(loc="thorax", typ="pneumothorax", sev=4.0, intervene=True)
            _add_injury(loc="thorax", typ="pneumothorax", sev=5.0, intervene=True)
            _add_injury(loc="thorax", typ="pneumothorax", sev=6.0, intervene=True)

            _add_injury(loc="thorax", typ="pulmonary_contusion", sev=1.0, intervene=False)
            _add_injury(loc="thorax", typ="pulmonary_contusion", sev=2.0, intervene=False)
            _add_injury(loc="thorax", typ="pulmonary_contusion", sev=3.0, intervene=False)
            _add_injury(loc="thorax", typ="pulmonary_contusion", sev=4.0, intervene=False)
            _add_injury(loc="thorax", typ="pulmonary_contusion", sev=5.0, intervene=False)
            _add_injury(loc="thorax", typ="pulmonary_contusion", sev=6.0, intervene=False)

            _add_injury(loc="thorax", typ="spinal", sev=1.0, intervene=False)
            _add_injury(loc="thorax", typ="spinal", sev=2.0, intervene=False)
            _add_injury(loc="thorax", typ="spinal", sev=3.0, intervene=False)
            _add_injury(loc="thorax", typ="spinal", sev=4.0, intervene=False)
            _add_injury(loc="thorax", typ="spinal", sev=5.0, intervene=False)
            _add_injury(loc="thorax", typ="spinal", sev=6.0, intervene=False)

            # Abdomen

            _add_injury(loc="abdomen", typ="hemorrhage", sev=1.0, cmpt="liver", intervene=True)
            _add_injury(loc="abdomen", typ="hemorrhage", sev=1.0, cmpt="spleen", intervene=True)
            _add_injury(loc="abdomen", typ="hemorrhage", sev=2.0, cmpt="liver", intervene=True)
            _add_injury(loc="abdomen", typ="hemorrhage", sev=2.0, cmpt="spleen", intervene=True)
            _add_injury(loc="abdomen", typ="hemorrhage", sev=3.0, cmpt="liver", intervene=True)
            _add_injury(loc="abdomen", typ="hemorrhage", sev=3.0, cmpt="spleen", intervene=True)
            _add_injury(loc="abdomen", typ="hemorrhage", sev=4.0, cmpt="liver", intervene=True)
            _add_injury(loc="abdomen", typ="hemorrhage", sev=4.0, cmpt="spleen", intervene=True)
            _add_injury(loc="abdomen", typ="hemorrhage", sev=5.0, cmpt="liver", intervene=True)
            _add_injury(loc="abdomen", typ="hemorrhage", sev=5.0, cmpt="spleen", intervene=True)
            _add_injury(loc="abdomen", typ="hemorrhage", sev=6.0, cmpt="liver", intervene=True)
            _add_injury(loc="abdomen", typ="hemorrhage", sev=6.0, cmpt="spleen", intervene=True)

            _add_injury(loc="abdomen", typ="laceration_contusion", sev=1.0, styp="contusion", intervene=False)
            _add_injury(loc="abdomen", typ="laceration_contusion", sev=1.0, styp="laceration", intervene=True)
            _add_injury(loc="abdomen", typ="laceration_contusion", sev=2.0, styp="contusion", intervene=False)
            _add_injury(loc="abdomen", typ="laceration_contusion", sev=2.0, styp="laceration", intervene=True)
            _add_injury(loc="abdomen", typ="laceration_contusion", sev=3.0, styp="contusion", intervene=False)
            _add_injury(loc="abdomen", typ="laceration_contusion", sev=3.0, styp="laceration", intervene=True)
            _add_injury(loc="abdomen", typ="laceration_contusion", sev=4.0, styp="contusion", intervene=False)
            _add_injury(loc="abdomen", typ="laceration_contusion", sev=4.0, styp="laceration", intervene=True)
            _add_injury(loc="abdomen", typ="laceration_contusion", sev=5.0, styp="contusion", intervene=False)
            _add_injury(loc="abdomen", typ="laceration_contusion", sev=5.0, styp="laceration", intervene=True)
            _add_injury(loc="abdomen", typ="laceration_contusion", sev=6.0, styp="contusion", intervene=False)
            _add_injury(loc="abdomen", typ="laceration_contusion", sev=6.0, styp="laceration", intervene=True)

            # Extremities

            _add_injury(loc="extremity", typ="burn_nerve", sev=2.0, cmpt="right_leg", intervene=False)
            _add_injury(loc="extremity", typ="burn_nerve", sev=3.0, cmpt="left_leg", intervene=False)
            _add_injury(loc="extremity", typ="burn_nerve", sev=4.0, cmpt="right_arm", intervene=False)
            _add_injury(loc="extremity", typ="burn_nerve", sev=5.0, cmpt="left_arm", intervene=False)

            _add_injury(loc="extremity", typ="contusion_sprain_strain", sev=2.0, cmpt="right_leg", styp="contusion", intervene=False)
            _add_injury(loc="extremity", typ="contusion_sprain_strain", sev=3.0, cmpt="left_leg", styp="sprain", intervene=False)
            _add_injury(loc="extremity", typ="contusion_sprain_strain", sev=4.0, cmpt="right_arm", styp="strain", intervene=False)
            _add_injury(loc="extremity", typ="contusion_sprain_strain", sev=5.0, cmpt="left_arm", styp="contusion", intervene=False)

            _add_injury(loc="extremity", typ="fracture_dislocation", sev=2.0, cmpt="right_leg", styp="fracture", intervene=False)
            _add_injury(loc="extremity", typ="fracture_dislocation", sev=3.0, cmpt="left_leg", styp="dislocation", intervene=False)
            _add_injury(loc="extremity", typ="fracture_dislocation", sev=4.0, cmpt="right_arm", styp="fracture", intervene=False)
            _add_injury(loc="extremity", typ="fracture_dislocation", sev=5.0, cmpt="left_arm", styp="dislocation", intervene=False)

            _add_injury(loc="extremity", typ="hemorrhage", sev=2.0, cmpt="right_arm", intervene=True)
            _add_injury(loc="extremity", typ="hemorrhage", sev=2.0, cmpt="right_leg", intervene=True)
            _add_injury(loc="extremity", typ="hemorrhage", sev=3.0, cmpt="left_arm", intervene=True)
            _add_injury(loc="extremity", typ="hemorrhage", sev=3.0, cmpt="left_leg", intervene=True)
            _add_injury(loc="extremity", typ="hemorrhage", sev=4.0, cmpt="right_arm", intervene=True)
            _add_injury(loc="extremity", typ="hemorrhage", sev=4.0, cmpt="left_leg", intervene=True)
            _add_injury(loc="extremity", typ="hemorrhage", sev=5.0, cmpt="right_leg", intervene=True)

            #  Polytraumas

            casualty_injuries.append([{"location": "thorax", "type": "hemothorax", "sub_type": None, "cmpt": None,
                                       "severity": 2.0, "can_intervene": False},
                                      {"location": "thorax", "type": "hemorrhage", "sub_type": None, "cmpt": None,
                                       "severity": 3.0, "can_intervene": False}])

            casualties = {"age": [], "state": []}
            for _ in range(len(casualty_injuries)):
                casualties["age"].append(44.0)
                casualties["state"].append("./states/StandardMale@0s.json")
        else:
            _log.info(f"Creating dataset of {population_size} casualties")
            casualties = casualty_population_generation(population_size, population_distributions)
            casualty_injuries = population_injury_generation(population_size, injury_distributions, injury_opts)

            # Check\count for injury combinations not currently supported in Pulse
            num_hemopneumothorax = 0
            for injuries in casualty_injuries:
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
        combined_casualties = {}
        for i in range(len(casualty_injuries)):
            casualty = {}
            for field, values in casualties.items():
                casualty[field] = values[i]
            casualty["injuries"] = casualty_injuries[i]
            combined_casualties[i] = {"specification": casualty}
        return combined_casualties

    @staticmethod
    def vitals_description(vitals: dict) -> List[str]:
        description = []

        # Check to see if the casualty can walk
        if vitals["ambulatory"]:
            description.append("The casualty is able to walk.")
            return description
        else:
            description.append("The casualty is unable to walk.")

        if vitals["avpu"] == AVPU.Alert:
            description.append("The casualty is alert.")
        elif vitals["avpu"] == AVPU.Voice:
            description.append("The casualty's eyes are closed.\n"
                               "They are responding to your voice.")
        elif vitals["avpu"] == AVPU.Pain:
            description.append("The casualty's eyes are closed.\n"
                               "They are not responding to your voice, but they are responding to pain stimuli.")
        else:  # AVPU.Unresponsive
            description.append("The casualty is unconscious and unresponsive to any stimuli.")

        rr = vitals["respiratory_rate"]
        if not vitals["breathing"]:
            if "reposition_airway" in vitals["interventions"]:
                description.append("Casualty was not breathing.")
                description.append("Repositioning their airway resulted in spontaneous breathing.")
            else:
                description.append("Casualty is not breathing.")
                description.append("Repositioning their airway did not help breathing.")
        elif rr < 12:
            description.append("The casualty is breathing slowly.")
        elif rr < 18:
            description.append("The casualty is breathing normally.")
        elif rr < 30:
            description.append("The casualty is breathing rapidly.")
        else:
            description.append("The casualty's breathing is very rapid and shallow.")
        if vitals["breathing_distressed"]:
            description.append("The casualty's breathing is distressed.")

        hr = vitals["heart_rate"]
        if not vitals["peripheral_pulse"]:
            description.append("Casualty does not have a peripheral pulse.")
        else:
            rhythm = vitals["heart_rhythm"]
            # TODO Support all other abnormal heart rhythms
            if rhythm == eHeartRhythm.SinusBradycardia.name:
                description.append("The casualty is experiencing bradycardia.")
            elif rhythm == eHeartRhythm.SinusTachycardia.name:
                description.append("The casualty is experiencing tachycardia.")
            else:
                description.append("The casualty has a normal heart rate.")
        if vitals["healthy_capillary_refill_time"]:
            description.append("The casualty has a healthy capillary refill time.")
        else:
            description.append("The casualty has a poor capillary refill time.")

        return description

    def injury_description(self,
                           duration_min: float,
                           injuries: List[dict],
                           actions: List[dict],
                           vitals: dict) -> List[str]:

        description = []
        # TODO Mental State, based on duration since injury?

        injury_dict = _injury_list_to_dict(injuries)
        for loc, types in injury_dict.items():
            for typ, items in types.items():
                severities = items["severities"]
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
                            if sev == 1.0:
                                description.append(f"The casualty is complaining of a headache.")
                            elif sev == 2.0:
                                description.append(f"The casualty says they have a headache and feel nauseous.")
                            elif sev == 3.0:
                                description.append(f"The casualty is complaining of a headache and slurring words.")
                            else:
                                _log.error("We shouldn't be here and alert....")
                                exit(1)
                        else:
                            if sev < 2:
                                _log.error("We shouldn't be here and NOT alert....")
                                exit(1)
                        continue

                    if typ == "airway_obstruction":
                        # Should be covered above in vitals description
                        continue

                    if typ == "superficial":
                        description.append(f"The casualty has some {typ} wounds to their head and neck area.")
                        continue

                if loc == "thorax":
                    if num > 2:
                        _log.error(f"More than 2 {typ} injuries on the {loc}, is currently unsupported")
                        exit(1)

                    if typ == "fracture":
                        if vitals["avpu"] == AVPU.Alert:
                            description.append("Casualty is complaining about chest pain.")
                        else:
                            description.append("There is no visible injury to the casualty.")
                        continue

                    if typ == "hemorrhage":
                        if max_sev == 1.0:
                            description.append(f"There is minor hemorrhaging on the torso.")
                        elif max_sev == 2.0:
                            description.append(f"There is moderate hemorrhaging on the torso.")
                        elif max_sev == 3.0:
                            description.append(f"There is serious hemorrhaging on the torso.")
                        elif max_sev == 4.0:
                            description.append(f"There is severe hemorrhaging on the torso.")
                        else:
                            description.append(f"There is critical hemorrhaging on the torso.")
                        continue

                    if typ == "hemothorax":
                        # Vitals info is good
                        continue

                    if typ == "pneumothorax":
                        if num == 1:
                            if vitals["avpu"] == AVPU.Alert:
                                description.append("Casualty is complaining about chest pain.")
                            else:
                                description.append("There is no visible injury to the casualty.")
                            continue

                        elif num == 2:
                            if vitals["avpu"] == AVPU.Alert:
                                description.append("Casualty is complaining about chest pain.")
                            else:
                                description.append("There is no visible injury to the casualty.")
                            continue

                    if typ == "pulmonary_contusion":
                        if vitals["avpu"] == AVPU.Alert:
                            description.append("Casualty is complaining about chest pain.")
                        else:
                            description.append("There is no visible injury to the casualty.")
                        continue

                    if typ == "spinal":
                        description.append("There is no visible injury to the casualty.")
                        continue

                if loc == "abdomen":
                    if num > 1:
                        _log.error(f"Multiple injuries on the {loc}, is currently unsupported")
                        exit(1)

                    if typ == "hemorrhage":
                        if max_sev == 1.0:
                            sev_mod = "minor"
                        elif max_sev == 2.0:
                            sev_mod = "moderate"
                        elif max_sev == 3.0:
                            sev_mod = "serious"
                        elif max_sev == 4.0:
                            sev_mod = "severe"
                        else:
                            sev_mod = "critical"
                        description.append(f"The casualty has a {sev_mod} {typ} from their {loc}.")
                        continue

                    if typ == "laceration_contusion":
                        if max_sev == 1.0:
                            sev_mod = "minor"
                        elif max_sev == 2.0:
                            sev_mod = "moderate"
                        elif max_sev == 3.0:
                            sev_mod = "serious"
                        elif max_sev == 4.0:
                            sev_mod = "severe"
                        else:
                            sev_mod = "critical"
                        styp = items["sub_types"][0]
                        if styp == "laceration":
                            description.append(f"The casualty has a {sev_mod} skin {styp} to their {loc}.")
                        else:
                            description.append(f"The casualty has a {sev_mod} abdominal bruising.")
                        continue

                if loc == "extremity":
                    if num > 1:
                        _log.error(f"Multiple injuries on the {loc}, is currently unsupported")
                        exit(1)

                    if typ == "hemorrhage":
                        cmpt = items["cmpts"][0].replace('_', ' ')
                        if max_sev == 1.0:
                            sev_mod = "minor"
                        elif max_sev == 2.0:
                            sev_mod = "moderate"
                        elif max_sev == 3.0:
                            sev_mod = "serious"
                        elif max_sev == 4.0:
                            sev_mod = "severe"
                        else:
                            sev_mod = "critical"
                        description.append(f"The casualty has a {sev_mod} {typ} on their {cmpt}.")
                        continue

                    if typ == "fracture_dislocation":
                        if max_sev == 1.0:
                            sev_mod = "minor"
                        elif max_sev == 2.0:
                            sev_mod = "moderate"
                        elif max_sev == 3.0:
                            sev_mod = "serious"
                        elif max_sev == 4.0:
                            sev_mod = "severe"
                        else:
                            sev_mod = "critical"
                        cmpt = items["cmpts"][0].replace('_', ' ')
                        description.append(f"The casualty has a {sev_mod} injury to their {cmpt}.")
                        continue

                    if typ == "contusion_sprain_strain":
                        if max_sev == 1.0:
                            sev_mod = "minor"
                        elif max_sev == 2.0:
                            sev_mod = "moderate"
                        elif max_sev == 3.0:
                            sev_mod = "serious"
                        elif max_sev == 4.0:
                            sev_mod = "severe"
                        else:
                            sev_mod = "critical"
                        cmpt = items["cmpts"][0].replace('_', ' ')
                        description.append(f"The casualty has a {sev_mod} injury to their {cmpt}.")
                        continue

                    if typ == "burn_nerve":
                        styp = "burn"  # TODO items["sub_types"][0]
                        if styp == "burn":
                            if max_sev == 1.0:
                                sev_mod = "minor"
                            elif max_sev == 2.0:
                                sev_mod = "moderate"
                            elif max_sev == 3.0:
                                sev_mod = "serious"
                            elif max_sev == 4.0:
                                sev_mod = "severe"
                            else:
                                sev_mod = "critical"
                            cmpt = items["cmpts"][0].replace('_', ' ')
                            description.append(f"The casualty has {sev_mod} burns to their {cmpt}.")
                        else:  # nerve
                            # TODO Not sure what description we want for nerve...
                            pass
                        continue

                _log.error(f"Unsupported injury: {loc} {typ}")
                exit(1)

        return description

    def calculate_triage_vitals(self, synthetic_patient: dict, active_events: dict, pulse_data: PulseData):
        synthetic_injuries = synthetic_patient["injuries"]
        interventions = []

        def _can_intervene(loc: str, inj: str):
            for i in synthetic_injuries:
                if i["can_intervene"] and i["location"] == loc and i["type"] == inj:
                    return True
            return False

        # Find the highest severity injury
        # TODO Current implementation is limited when used to calculate vitals of a intervened casualty
        # TODO We really need to get the max_severity of an untreated injury
        max_severity = 0
        for injury in synthetic_injuries:
            if injury['severity'] > max_severity:
                max_severity = injury['severity']

        # ref: Schluter et al.
        # ISS has values from 0 to 75
        # Only the highest AIS score in each body region is used. The
        # three most severely injured body regions have their score
        # squared and added together to produce the ISS. An AIS of 6 in
        # any anatomic region represents a fatal injury and automatically
        # scores an ISS of 75, regardless of other injuries
        # This data set does not separate head and neck and facial injuries
        # This data set also only supports poly trauma on the thorax
        # Meaning we can just square the max severity for our ISS score
        # since our severity is AIS
        iss = max_severity * max_severity
        # Override iss if severity is high enough
        if max_severity == 6.0:
            iss = 75

        # TODO What is blunt trauma?
        blunt_trauma = True
        for injury in synthetic_injuries:
            if injury["type"] == "hemorrhage":
                blunt_trauma = False

        # Breathing
        breathing_distressed = False
        rr = pulse_data.get_rr(FrequencyUnit.Per_min)
        if eEvent.Tachypnea in active_events or rr < 6:
            breathing_distressed = True
            if _can_intervene("thorax", "pneumothorax"):
                interventions.append(Intervention.NeedleDecompress)
        if rr < 1.0:
            breathing = False
            breathing_distressed = None
            if _can_intervene("head_and_neck", "airway_obstruction"):
                interventions.append(Intervention.RepositionAirway)
        else:
            breathing = True

        # AVPU
        avpu = AVPU.Alert
        # Check max severity and oxygen partial pressure in the brain
        brain_o2_pp = pulse_data.get_brain_o2_pp(PressureUnit.mmHg)
        if max_severity == 6.0 or brain_o2_pp < 15 or not breathing:
            avpu = AVPU.Unresponsive
        elif 15 <= brain_o2_pp <= 25:
            avpu = AVPU.Pain
        elif 25 < brain_o2_pp < 35 or max_severity >= 4.0:
            avpu = AVPU.Voice

        # Ambulatory
        ambulatory = True
        if max_severity > 3.0:
            ambulatory = False
        elif avpu != AVPU.Alert:
            ambulatory = False
        if rr <= 1.0:
            ambulatory = False

        # Check for abdominal or leg wounds, hard to walk with these
        for injury in synthetic_injuries:
            if injury["location"] == "extremity":
                if injury["severity"] > 2 and injury["cmpt"] and "leg" in injury["cmpt"]:
                    ambulatory = False
            if injury["location"] == "abdomen":
                if injury["severity"] >= 2:
                    ambulatory = False

        # Hemorrhage / Laceration
        hemorrhage = None
        for injury in synthetic_injuries:
            if injury["type"] == "hemorrhage" or injury["type"] == "laceration_contusion":
                # TODO support gauze or pressure bandage on thorax hemorrhages?
                if injury["severity"] > 3:
                    hemorrhage = Hemorrhage.Major
                elif not hemorrhage:
                    hemorrhage = Hemorrhage.Minor

                if injury["location"] == "abdomen":
                    if _can_intervene("abdomen", "hemorrhage"):
                        interventions.append(Intervention.WoundPack)
                    elif injury["sub_type"] and injury["sub_type"] == "laceration":
                        if _can_intervene("abdomen", "laceration_contusion"):
                            interventions.append(Intervention.WoundPack)
                elif injury["location"] == "extremity":
                    if _can_intervene("extremity", "hemorrhage"):
                        interventions.append(Intervention.Tourniquet)

        # NOTE: SALT Protocol
        survivable_injuries = True
        if max_severity == 6.0:
            survivable_injuries = False

        healthy_capillary_refill_time = True
        if pulse_data.get_ppi() < 0.003:
            healthy_capillary_refill_time = False
        peripheral_pulse = healthy_capillary_refill_time

        return {"age": synthetic_patient["age"],
                "avpu": avpu,
                "ambulatory": ambulatory,
                "blunt_trauma": blunt_trauma,
                "brain_o2_pp": brain_o2_pp,
                "breathing": breathing,
                "breathing_distressed": breathing_distressed,
                "healthy_capillary_refill_time": healthy_capillary_refill_time,
                "heart_rate": pulse_data.get_hr(FrequencyUnit.Per_min),
                "heart_rhythm": pulse_data.get_heart_rhythm().name,
                "hemorrhage": hemorrhage,
                "interventions": interventions,
                "iss": iss,
                "major_injuries": True if max_severity > 3 else False,
                "peripheral_pulse": peripheral_pulse,
                "respiratory_rate": pulse_data.get_rr(FrequencyUnit.Per_min),
                "spO2": pulse_data.get_spo2(),
                "systolic_pressure": pulse_data.get_systolic_pressure(PressureUnit.mmHg),
                "diastolic_pressure": pulse_data.get_diastolic_pressure(PressureUnit.mmHg),
                "survivable_injuries": survivable_injuries
                }

    def injury_actions(self, injuries: list) -> List[SEAction]:
        actions: List[SEAction] = []

        def _ais_2_pulse(value: float,
                         min_input: float = 1.0, max_input: float = 6.0,
                         min_output: float = 0.15, max_output: float = 1.0) -> float:
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

        # Create a ledger to for AIS polytraumas to pulse
        # We will sum AIS score and join pulse severity ranges
        # We will use the highest min, and the highest max
        # ex. (0, 1) + (0.2, 0.7) = (0.2, 1.0)
        ledger = {"airway_obstruction": {"ais": 0, "low": 0, "high": 0},
                  "ards": {"left_lung": {"ais": 0, "low": 0, "high": 0},
                           "right_lung": {"ais": 0, "low": 0, "high": 0}},
                  "hemorrhage": {"left_arm": {"ais": 0, "low": 0, "high": 0},
                                 "left_leg": {"ais": 0, "low": 0, "high": 0},
                                 "liver": {"ais": 0, "low": 0, "high": 0},
                                 "muscle": {"ais": 0, "low": 0, "high": 0},
                                 "right_arm": {"ais": 0, "low": 0, "high": 0},
                                 "right_leg": {"ais": 0, "low": 0, "high": 0},
                                 "skin": {"ais": 0, "low": 0, "high": 0},
                                 "spleen": {"ais": 0, "low": 0, "high": 0}},
                  "pneumothorax": {"left_lung": {"ais": 0, "low": 0, "high": 0},
                                   "right_lung": {"ais": 0, "low": 0, "high": 0}},
                  "stress": {"ais": 0, "low": 0, "high": 0},
                  "tbi": {"ais": 0, "low": 0, "high": 0},
                  }

        def _post(injury: str, cmpt: str = None, ais: int = 1, low: float = 0.0, high: float = 1.0, sev: float = None):
            if cmpt:
                ledger[injury][cmpt]["ais"] = min(ledger[injury][cmpt]["ais"]+ais, 6)
                ledger[injury][cmpt]["low"] = max(low, ledger[injury][cmpt]["low"])
                ledger[injury][cmpt]["high"] = max(high, ledger[injury][cmpt]["high"])
                ledger[injury][cmpt]["sev"] = sev
            else:
                ledger[injury]["ais"] = min(ledger[injury]["ais"]+ais, 6)
                ledger[injury]["low"] = max(low, ledger[injury]["low"])
                ledger[injury]["high"] = max(high, ledger[injury]["high"])
                ledger[injury]["sev"] = sev

        for location, types in injury_dict.items():
            for t, items in types.items():
                severities = items["severities"]
                ais = sum(severities)
                num = len(severities)

                if location == "head_and_neck":

                    if num > 1 or len(types) > 1:
                        _log.fatal(f"More than one {location} injury is unsupported")
                        exit(1)

                    if t == "airway_obstruction":
                        _post(injury="airway_obstruction", ais=ais, low=0.15, high=0.83)
                        _post(injury="stress", ais=ais, low=0.15, high=0.35)
                        continue

                    if t == "superficial":
                        # 5-15 mL/min
                        _post(injury="hemorrhage", cmpt="skin", ais=ais, low=0.03, high=0.08)
                        _post(injury="stress", ais=ais, low=0.15, high=0.35)
                        continue

                    if t == "tbi":
                        _post(injury="tbi", ais=ais, low=0.15, high=1.0)
                        _post(injury="stress", ais=ais, low=0.15, high=0.35)
                        continue

                if location == "thorax":

                    if t == "pneumothorax":

                        _post(injury="stress", ais=ais, low=0.15, high=0.35)
                        if num == 1:
                            side = np.random.randint(0, 1)
                            if side == 0:
                                _post(injury="pneumothorax", cmpt="left_lung", ais=ais, low=0.05, high=0.6)
                            elif side == 1:
                                _post(injury="pneumothorax", cmpt="right_lung", ais=ais, low=0.05, high=0.6)
                            continue

                        elif num == 2:
                            _post(injury="pneumothorax", cmpt="left_lung", ais=severities[0], low=0.05, high=0.6)
                            _post(injury="pneumothorax", cmpt="right_lung", ais=severities[1], low=0.05, high=0.6)
                            continue

                        else:
                            _log.fatal(f"More than two {location} pneumothorax is unsupported")
                            exit(1)

                    if t == "pulmonary_contusion":

                        _post(injury="stress", ais=ais, low=0.15, high=0.35)
                        if num == 1:
                            side = np.random.randint(0, 1)
                            if side == 0:
                                _post(injury="ards", cmpt="left_lung", ais=ais, low=0.15, high=1.0)
                            elif side == 1:
                                _post(injury="ards", cmpt="right_lung", ais=ais, low=0.15, high=1.0)
                            continue

                        elif num == 2:
                            _post(injury="ards", cmpt="left_lung", ais=severities[0], low=0.15, high=1.0)
                            _post(injury="ards", cmpt="right_lung", ais=severities[1], low=0.15, high=1.0)
                            continue

                        else:
                            _log.fatal(f"More than two {location} pulmonary_contusion is unsupported")
                            exit(1)

                    if t == "hemothorax":

                        _post(injury="stress", ais=ais, low=0.15, high=0.35)
                        _post(injury="hemorrhage", cmpt="muscle", ais=ais, low=0.02, high=0.10)  # 11-60 mL/min
                        if num == 1:
                            side = np.random.randint(0, 1)
                            if side == 0:
                                _post(injury="pneumothorax", cmpt="left_lung", ais=ais, low=0.05, high=0.6)
                            elif side == 1:
                                _post(injury="pneumothorax", cmpt="right_lung", ais=ais, low=0.05, high=0.6)
                            continue

                        elif num == 2:
                            _post(injury="pneumothorax", cmpt="left_lung", ais=severities[0], low=0.05, high=0.6)
                            _post(injury="pneumothorax", cmpt="right_lung", ais=severities[1], low=0.05, high=0.6)
                            continue

                        else:
                            _log.fatal(f"More than two {location} hemothorax is unsupported")
                            exit(1)

                    if t == "hemorrhage":

                        if num > 2:
                            _log.fatal(f"More than two {location} hemorrhage is unsupported")
                            exit(1)

                        _post(injury="stress", ais=ais, low=0.15, high=0.35)
                        # 15-75 mL/min combined
                        _post(injury="hemorrhage", cmpt="muscle", ais=ais, low=0.02, high=0.10)
                        _post(injury="hemorrhage", cmpt="skin", ais=ais, low=0.02, high=0.10)
                        continue

                    if t == "fracture":

                        if num > 2:
                            _log.fatal(f"More than two {location} fracture is unsupported")
                            exit(1)

                        _post(injury="stress", ais=ais, low=0.20, high=0.70)
                        continue

                    if t == "spinal":

                        if num > 1:
                            _log.fatal(f"More than one {location} spinal injury is unsupported")
                            exit(1)

                        _post(injury="stress", ais=ais, low=0.20, high=0.70)
                        continue

                if location == "abdomen":
                    if num > 1 or len(types) > 1:
                        _log.fatal(f"More than one {location} injury is unsupported")
                        exit(1)

                    if t == "hemorrhage":
                        cmpt = items["cmpts"][0]
                        if ais == 1:  # ~13 mL/min
                            if cmpt == "spleen":
                                _post(injury="hemorrhage", cmpt=cmpt, sev=0.11)
                            elif cmpt == "liver":
                                _post(injury="hemorrhage", cmpt=cmpt, sev=0.02)
                        elif ais == 2:  # ~27 mL/min
                            if cmpt == "spleen":
                                _post(injury="hemorrhage", cmpt=cmpt, sev=0.21)
                            elif cmpt == "liver":
                                _post(injury="hemorrhage", cmpt=cmpt, sev=0.04)
                        elif ais == 3:  # ~38 mL/min
                            if cmpt == "spleen":
                                _post(injury="hemorrhage", cmpt=cmpt, sev=0.28)
                            elif cmpt == "liver":
                                _post(injury="hemorrhage", cmpt=cmpt, sev=0.05)
                        elif ais == 4:  # ~50 mL/min
                            if cmpt == "spleen":
                                _post(injury="hemorrhage", cmpt=cmpt, sev=0.35)
                            elif cmpt == "liver":
                                _post(injury="hemorrhage", cmpt=cmpt, sev=0.06)
                        elif ais == 5:  # ~63 mL/min
                            if cmpt == "spleen":
                                _post(injury="hemorrhage", cmpt=cmpt, sev=0.42)
                            elif cmpt == "liver":
                                _post(injury="hemorrhage", cmpt=cmpt, sev=0.07)
                        elif ais == 6:  # ~75 mL/min
                            if cmpt == "spleen":
                                _post(injury="hemorrhage", cmpt=cmpt, sev=0.48)
                            elif cmpt == "liver":
                                _post(injury="hemorrhage", cmpt=cmpt, sev=0.09)
                        _post(injury="stress", ais=ais, low=0.15, high=0.35)
                        continue

                    if t == "laceration_contusion":
                        # Laceration 9-40 mL/min
                        _post(injury="hemorrhage", cmpt="skin", ais=ais, low=0.05, high=0.20)
                        _post(injury="stress", ais=ais, low=0.15, high=0.35)
                        continue

                if location == "extremity":
                    if num > 1 or len(types) > 1:
                        _log.fatal(f"More than one {location} injury is unsupported")
                        exit(1)

                    if t == "hemorrhage":
                        cmpt = items["cmpts"][0]
                        # Use explicit severities for a specific flow rate
                        if ais == 1:  # ~13 mL/min
                            if "arm" in cmpt:
                                _post(injury="hemorrhage", cmpt=cmpt, sev=0.24)
                            elif "leg" in cmpt:
                                _post(injury="hemorrhage", cmpt=cmpt, sev=0.07)
                        elif ais == 2:  # ~27 mL/min
                            if "arm" in cmpt:
                                _post(injury="hemorrhage", cmpt=cmpt, sev=0.45)
                            elif "leg" in cmpt:
                                _post(injury="hemorrhage", cmpt=cmpt, sev=0.14)
                        elif ais == 3:  # ~38 mL/min
                            if "arm" in cmpt:
                                _post(injury="hemorrhage", cmpt=cmpt, sev=0.63)
                            elif "leg" in cmpt:
                                _post(injury="hemorrhage", cmpt=cmpt, sev=0.19)
                        elif ais == 4:  # ~50 mL/min
                            if "arm" in cmpt:
                                _post(injury="hemorrhage", cmpt=cmpt, sev=0.84)
                            elif "leg" in cmpt:
                                _post(injury="hemorrhage", cmpt=cmpt, sev=0.25)
                        elif ais == 5:  # ~63 mL/min
                            if "arm" in cmpt:
                                _log.fatal(f"AIS of {ais} does not support arm hemorrhages")
                                exit(1)
                            elif "leg" in cmpt:
                                _post(injury="hemorrhage", cmpt=cmpt, sev=0.30)
                        elif ais == 6:  # ~75 mL/min
                            if "arm" in cmpt:
                                _log.fatal(f"AIS of {ais} does not support arm hemorrhages")
                                exit(1)
                            elif "leg" in cmpt:
                                _post(injury="hemorrhage", cmpt=cmpt, sev=0.35)
                        _post(injury="stress", ais=ais, low=0.15, high=0.35)
                        continue

                    if t == "fracture_dislocation":
                        _post(injury="stress", ais=ais, low=0.20, high=0.70)
                        continue

                    if t == "contusion_sprain_strain":
                        _post(injury="stress", ais=ais, low=0.20, high=0.70)
                        continue

                    if t == "burn_nerve":
                        _post(injury="stress", ais=ais, low=0.20, high=0.70)
                        continue

                _log.error(f"Unsupported injury: {location} {t}")

        # Create Pulse actions

        injury = ledger["airway_obstruction"]
        if injury["ais"] > 0:
            obs = SEAirwayObstruction()
            obs.get_severity().set_value(_ais_2_pulse(value=injury["ais"],
                                                      min_output=injury["low"],
                                                      max_output=injury["high"]))
            actions.append(obs)

        injury = ledger["ards"]["left_lung"]
        if injury["ais"] > 0:
            ards = SEAcuteRespiratoryDistressSyndromeExacerbation()
            ards.get_severity(eLungCompartment.LeftLung).set_value(_ais_2_pulse(value=injury["ais"],
                                                                                min_output=injury["low"],
                                                                                max_output=injury["high"]))
            actions.append(ards)

        injury = ledger["ards"]["right_lung"]
        if injury["ais"] > 0:
            ards = SEAcuteRespiratoryDistressSyndromeExacerbation()
            ards.get_severity(eLungCompartment.RightLung).set_value(_ais_2_pulse(value=injury["ais"],
                                                                                 min_output=injury["low"],
                                                                                 max_output=injury["high"]))
            actions.append(ards)

        def _create_hemorrhage(_injury: dict, _cmpt: str, min_input: float = 1.0, max_input: float = 6.0):
            h = SEHemorrhage()
            h.set_compartment(_cmpt)
            h.set_type(eHemorrhage_Type.External)
            if _injury["sev"]:
                h.get_severity().set_value(_injury["sev"])
            else:
                h.get_severity().set_value(_ais_2_pulse(value=_injury["ais"],
                                                        min_input=min_input,
                                                        max_input=max_input,
                                                        min_output=_injury["low"],
                                                        max_output=_injury["high"]))
            return h

        injury = ledger["hemorrhage"]["left_arm"]
        if injury["ais"] > 0:
            actions.append(_create_hemorrhage(injury, eHemorrhage_Compartment.LeftArm.value))
        injury = ledger["hemorrhage"]["left_leg"]
        if injury["ais"] > 0:
            actions.append(_create_hemorrhage(injury, eHemorrhage_Compartment.LeftLeg.value))
        injury = ledger["hemorrhage"]["liver"]
        if injury["ais"] > 0:
            actions.append(_create_hemorrhage(injury, eHemorrhage_Compartment.Liver.value))
        injury = ledger["hemorrhage"]["muscle"]
        if injury["ais"] > 0:
            actions.append(_create_hemorrhage(injury, eHemorrhage_Compartment.Muscle.value))
        injury = ledger["hemorrhage"]["right_arm"]
        if injury["ais"] > 0:
            actions.append(_create_hemorrhage(injury, eHemorrhage_Compartment.RightArm.value))
        injury = ledger["hemorrhage"]["right_leg"]
        if injury["ais"] > 0:
            actions.append(_create_hemorrhage(injury, eHemorrhage_Compartment.RightLeg.value))
        injury = ledger["hemorrhage"]["skin"]
        if injury["ais"] > 0:
            actions.append(_create_hemorrhage(injury, eHemorrhage_Compartment.Skin.value))
        injury = ledger["hemorrhage"]["spleen"]
        if injury["ais"] > 0:
            actions.append(_create_hemorrhage(injury, eHemorrhage_Compartment.Spleen.value))

        injury = ledger["pneumothorax"]["left_lung"]
        if injury["ais"] > 0:
            pneumo = SETensionPneumothorax()
            pneumo.set_side(eSide.Left)
            pneumo.set_type(eGate.Closed)
            pneumo.get_severity().set_value(_ais_2_pulse(value=injury["ais"],
                                                         min_output=injury["low"],
                                                         max_output=injury["high"]))
            actions.append(pneumo)

        injury = ledger["pneumothorax"]["right_lung"]
        if injury["ais"] > 0:
            pneumo = SETensionPneumothorax()
            pneumo.set_side(eSide.Right)
            pneumo.set_type(eGate.Closed)
            pneumo.get_severity().set_value(_ais_2_pulse(value=injury["ais"],
                                                         min_output=injury["low"],
                                                         max_output=injury["high"]))
            actions.append(pneumo)

        injury = ledger["stress"]
        if injury["ais"] > 0:
            stress = SEAcuteStress()
            stress.get_severity().set_value(_ais_2_pulse(value=injury["ais"],
                                                         min_output=injury["low"],
                                                         max_output=injury["high"]))
            actions.append(stress)

        injury = ledger["tbi"]
        if injury["ais"] > 0:
            tbi = SEBrainInjury()
            tbi_type = np.random.randint(0, 2)
            if tbi_type == 0:
                tbi.set_injury_type(eBrainInjuryType.Diffuse)
            elif tbi_type == 1:
                tbi.set_injury_type(eBrainInjuryType.LeftFocal)
            elif tbi_type == 2:
                tbi.set_injury_type(eBrainInjuryType.RightFocal)
            tbi.get_severity().set_value(_ais_2_pulse(value=injury["ais"],
                                                      min_output=injury["low"],
                                                      max_output=injury["high"]))
            actions.append(tbi)

        return actions

    def injury_interventions(self, synthetic_injuries: list, pulse_injuries: list, vitals: dict):
        # TODO Figure out a way map interventions to specific actions, like treat 1 of 2 hemorrhages
        actions = []
        interventions = vitals["interventions"]
        for injury in pulse_injuries:
            if Intervention.RepositionAirway in interventions:
                if "AirwayObstruction" in injury["PatientAction"]:
                    ao = SEAirwayObstruction()
                    s = injury["PatientAction"]["AirwayObstruction"]["Severity"]["Scalar0To1"]["Value"]
                    ao.get_severity().set_value(s*0.6666)  # Remove 1/3 of the obstruction
                    ao.set_comment(f"Reposition Airway. Reduce severity from {s} to {s*0.6666}")
                    actions.append(ao)
                    _log.info("Clearing airway obstruction")
            if Intervention.WoundPack in interventions:
                if "Hemorrhage" in injury["PatientAction"] and synthetic_injuries[0]["location"] == "abdomen":
                    h = SEHemorrhage()
                    cmpt = injury["PatientAction"]["Hemorrhage"]["Compartment"]
                    h.set_compartment(cmpt)
                    h.set_type(eHemorrhage_Type.External)
                    s = injury["PatientAction"]["Hemorrhage"]["Severity"]["Scalar0To1"]["Value"]
                    h.get_severity().set_value(s * 0.5)
                    h.set_comment(f"Pack wound. Reduce severity from {s} to {s * 0.5}")
                    actions.append(h)
                    _log.info(f"Packing abdominal {cmpt} hemorrhage")
            if Intervention.Tourniquet in interventions and synthetic_injuries[0]["location"] == "extremity":
                if "Hemorrhage" in injury["PatientAction"]:
                    h = SEHemorrhage()
                    cmpt = injury["PatientAction"]["Hemorrhage"]["Compartment"]
                    h.set_compartment(cmpt)
                    h.set_type(eHemorrhage_Type.External)
                    h.get_severity().set_value(0.05)
                    h.set_comment(f"Apply Tourniquet. Reduce severity to 0.05")
                    actions.append(h)
                    _log.info(f"Applying tourniquet to external {cmpt} hemorrhage")
            if Intervention.NeedleDecompress in interventions:
                if "TensionPneumothorax" in injury["PatientAction"]:
                    s = injury["PatientAction"]["TensionPneumothorax"]["Side"]
                    t = injury["PatientAction"]["TensionPneumothorax"]["Type"]
                    if t == "Closed":
                        nde = SENeedleDecompression()
                        nde.set_state(eSwitch.On)
                        if s == "Left":
                            nde.set_side(eSide.Left)
                        else:
                            nde.set_side(eSide.Right)
                        nde.set_comment(f"Decompress the plural space")
                        actions.append(nde)
                        _log.info(f"Applying Needle Decompression to {s} side")
                    else:
                        cod = SEChestOcclusiveDressing()
                        cod.set_state(eSwitch.On)
                        if s == "Left":
                            cod.set_side(eSide.Left)
                        else:
                            cod.set_side(eSide.Right)
                        cod.set_comment(f"Wrapping chest with occlusive dressing")
                        actions.append(cod)
                        _log.info(f"Applying Chest Occlusive Dressing to the {s} side")
        if len(actions) == 0:
            _log.error(f"Was told there were to be interventions done to this casualty, but not sure what to do...")
            exit(1)
        return actions


def main():
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
    output_dir = Path("./test_results/itm/army")
    output_dir.mkdir(parents=True, exist_ok=True)

    """
    Python stats are wierd....
    Order of how/what random distribution seems to impact its results....
    I am doing the same amount of calls, just in a different order...
    And get more failures one way than with another...

    mode = 5
    for population 5000 -> 10000
      test 1000 airway obst arrays
        if > 5% fail, move to next population
      test 1000 tbi array
        if > 5% fail, move to next population
      test 1000 hemorrhage arrays
        if > 5% fail move to next population
      If you get here, we found a good population
    This stops with a population of 7000

    Is different than

    mode = 6
    for population 5000 -> 10000
      for 1 to 1000
        test 1 airway obst arrays
            if fail, count and continue
        test 1 tbi array
            if fail, count and continue
        test 1 hemorrhage arrays
            if fail, count and continue
        if > 5% fail move to next population else stop
        
    This stops with a population of 9000
    
    The successful 7000 mode 5 run should have the same # of calls to random that 7000 mode 6 has....
    """
    mode = 4

    # Test specific injury
    if mode == 1:
        test_injury(injury_distributions=injury_distributions["thorax"],
                    num_patients_injured=1000,
                    log=True)

    elif mode == 2:
        for p in range(100, 10001, 100):
            max_error = 0
            for i in range(50):
                err = test_injury(injury_distributions=injury_distributions["thorax"],
                                  num_patients_injured=p,
                                  log=False)
                if err > max_error:
                    max_error = err
            _log.info(f"Max Error of {max_error} for {p} patients")

    # Run a measurement study
    elif mode == 3:
        # Measure error for various population sizes
        for p in [100, 500, 1000, 2000, 3000]:
            i = 25
            _log.info(f"Measuring error for a population size of {p} using {i} iterations")
            measure_error(iterations=i, population_size=p,
                          population_distributions=population_distributions,
                          injury_distributions=injury_distributions,
                          results_stem=f"{output_dir}/measurements/{p}")

    # Generate a data set
    elif mode == 4:
        # This is pretty much the closest to a full run
        num_casualties = 9000
        injury_opts = InjurySeverityOpts()
        injury_opts.halt_on_error = False

        failures = 0
        max_diff = 5  # percent
        num_runs = 1000
        for i in range(1, num_runs+1):
            fail = False
            spec = to_specification_lists(ArmyDataset().generate_dataset(population_size=num_casualties, injury_opts=injury_opts))
            severities = to_severity_lists(spec)
            for location, injuries in severities.items():
                for injury, severities in injuries.items():
                    severity_dist = injury_distributions[location]["types"][injury]["severity"]
                    if "mean" in severity_dist:
                        mean = statistics.mean(severities)
                        mean_pd = percent_difference(severity_dist["mean"], mean)
                        if mean_pd > max_diff:
                            failures += 1
                            _log.info(f"\tRun {i} FAILED from {location}-{injury} cnt={len(severities)}")
                            fail = True
                            break
                if fail:
                    break
            if not fail:
                _log.info(f"\tRun {i} passed")

        _log.info(f"Generating {num_runs} datasets with {num_casualties} casualties, "
                  f"we had {failures} sets ({failures/num_runs*100:.2f}%) "
                  f"produce a severity array with > than {max_diff}% error")

    elif mode == 5:

        samples = 100
        diff_maximum = 5  # percent
        specific = None  # [(2.85, 0.85, 189, "test")]

        for num_casualties in range(5000, 50001, 1000):
            _log.info(f"Running {num_casualties} casualties")
            distributions = []
            if specific:
                distributions = specific
            else:
                for location, location_dist in injury_distributions.items():
                    for injury, injury_dist in location_dist["types"].items():
                        severity_dist = injury_dist["severity"]
                        if "mean" in severity_dist:
                            size = math.floor(num_casualties*location_dist["percent"]*injury_dist["percent"]*0.0001)
                            distributions.append((severity_dist["mean"],
                                                  severity_dist["std"],
                                                  size,
                                                  f"{location}-{injury}"))

            keep_going = False
            for dist in distributions:
                diffs = []
                maxs = []
                mins = []
                sixes = 0
                ones = 0
                for i in range(1, samples):
                    bound = _bounded_random_normal(mean=dist[0], stdev=dist[1], size=int(dist[2]))
                    bounded_mean = statistics.mean(bound)
                    bounded_mean_pd = percent_difference(dist[0], bounded_mean)
                    #_log.info(f"\t"
                    #          f"1: {bound.count(1.0)} "
                    #          f"2: {bound.count(2.0)} "
                    #          f"3: {bound.count(3.0)} "
                    #          f"4: {bound.count(4.0)} "
                    #          f"5: {bound.count(5.0)} "
                    #          f"6: {bound.count(6.0)} "
                    #          f"- bounded {bounded} ({bounded/len(bound)}%)values "
                    #          f"- %diff {bounded_mean_pd}")
                    diffs.append(bounded_mean_pd)
                    if min(bound) == 1.0:
                        ones += 1
                    if max(bound) == 6.0:
                        sixes += 1
                    maxs.append(max(bound))
                    mins.append(min(bound))
                failed_diffs = 0
                for diff in diffs:
                    if diff > diff_maximum:
                        failed_diffs += 1
                percent_failed = failed_diffs/len(diffs)*100
                percent_ones = ones/len(diffs)*100
                percent_sixes = sixes/len(diffs)*100
                _log.info(f"\t{samples} arrays were generated with a mean:{dist[0]}, std:{dist[1]}, length {dist[2]}: "
                          f"- {failed_diffs} had a mean greater than {diff_maximum}% ({percent_failed:.2f}%) "
                          f"- {percent_ones:.2f}% had 1's "
                          f"- {percent_sixes:.2f}% had 6's "
                          f"- {dist[3]}")
                if percent_failed > diff_maximum:
                    keep_going = True
                    break

            if keep_going:
                _log.info(f"{num_casualties} is NOT enough")
            else:
                _log.info(f"{num_casualties} is enough")
                break

    elif mode == 6:

        samples = 1000
        diff_maximum = 5  # percent

        for num_casualties in range(9000, 50001, 1000):
            _log.info(f"Running {num_casualties} casualties")

            failed_diffs = 0
            for i in range(1, samples):
                failed = False
                for location, location_dist in injury_distributions.items():
                    for injury, injury_dist in location_dist["types"].items():
                        severity_dist = injury_dist["severity"]
                        if "mean" in severity_dist:
                            mean = severity_dist["mean"]
                            stdev = severity_dist["std"]
                            size = math.floor(num_casualties*location_dist["percent"]*injury_dist["percent"]*0.0001)
                            size += np.random.randint(-3, 3)

                            bound = _bounded_random_normal(mean=mean, stdev=stdev, size=int(size))
                            bounded_mean = statistics.mean(bound)
                            bounded_mean_pd = percent_difference(mean, bounded_mean)
                            if bounded_mean_pd > diff_maximum:
                                failed_diffs += 1
                                failed = True
                                _log.info(f"\t{i}-{location}-{injury} failed with diff {bounded_mean_pd}%")
                                break
                    if failed:
                        break
                if failed_diffs/samples*100 > diff_maximum:
                    _log.info(f"Aborting {num_casualties}")
                    break

            if failed_diffs/samples*100 < diff_maximum:
                _log.info(f"{num_casualties} was enough, "
                          f"failed {failed_diffs} out of {samples} ({failed_diffs/samples*100})%")
                break


if __name__ == "__main__":
    main()
