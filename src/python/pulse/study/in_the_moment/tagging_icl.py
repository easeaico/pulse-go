# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import logging
import json

from pathlib import Path

from pulse.cdm.engine import SEDataRequestManager
from pulse.cdm.patient_actions import SEAirwayObstruction
from pulse.cdm.scalars import FrequencyUnit, TimeUnit
from pulse.engine.PulseEngine import PulseEngine
from pulse.study.in_the_moment.army_dataset import ArmyDataset
from pulse.study.in_the_moment.post_processor import create_align_file
from pulse.study.in_the_moment.triage_dataset import convert_keys_to_int, PulseData
from triage_study_pipeline import TriageStudy

_log = logging.getLogger('log')

tag_labels = ["Green (Minor)",
              "Yellow (Delayed)",
              "Red (Immediate)",
              "Black (Expectant)"]


def _find_recoverable_airway_obstruction_vitals(dataset, can_intervene: bool) -> dict:

    pulse_data = PulseData()
    data_req_mgr = SEDataRequestManager(pulse_data.data_requests)
    pulse = PulseEngine()
    pulse.log_to_console(False)
    if not pulse.serialize_from_file("./states/StandardMale@0s.json", data_req_mgr):
        print("Unable to load initial state file")
        exit(1)
    airway_obstruction = SEAirwayObstruction()
    airway_obstruction.set_comment("Patient's airways are obstructed")
    airway_obstruction.get_severity().set_value(0.83)
    pulse.process_action(airway_obstruction)

    pulse_data.set_values(pulse.pull_data())
    while pulse_data.get_rr(FrequencyUnit.Per_min) > 0:
        pulse.advance_time_s(1)
        pulse_data.set_values(pulse.pull_data())

    _log.info(f"Casualty has stopped breathing due to airway obstruction at {pulse_data.get_time(TimeUnit.s):.2f}s")
    vitals = dataset.calculate_triage_vitals(synthetic_patient={"age": 27,
                                                                "injuries": [{"location": "head_and_neck",
                                                                              "type": "airway_obstruction",
                                                                              "sub_type": None,
                                                                              "cmpt": None,
                                                                              "severity": 6.0,
                                                                              "can_intervene": can_intervene}]},
                                             active_events={},
                                             pulse_data=pulse_data)
    return vitals


def main():
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

    # For our training set, we want to gather a set of casualties that contain all tag colors and all tag reasons
    # First we start by programmatically pulling out cases from our example casualty set with unique tag color/reasons
    # Manually examining these sets, we created the following cases for tag color/reasons not in this dataset

    with open(Path("./test_results/itm/triage_study/example_casualties.json"), 'r') as f:
        study_run = json.load(f, object_hook=convert_keys_to_int)

    # Map these reasons to the same casualty no matter what
    salt_survivability = ["Casualty is likely to survive these injuries.",
                          "Casualty is NOT likely to survive these injuries."]

    dataset = ArmyDataset()

    icl_vitals = []
    start_reasons = {}
    salt_reasons = {}
    bcd_reasons = {}
    for pid, run in study_run.items():
        for time, visit in run["visits"].items():
            triage = visit["triage"]
            tags = triage["tags"]

            unique_start_reason = False
            start_reason = tags["start_reason"].replace('\n', ' ')
            if start_reason not in start_reasons:
                start_reasons[start_reason] = (pid, time)
                unique_start_reason = True

            unique_salt_reason = False
            salt_reason = tags["salt_reason"].replace('\n', ' ')

            if salt_reason not in salt_reasons:
                salt_reasons[salt_reason] = (pid, time)
                unique_salt_reason = True
                other_salt_reason = None
                if salt_survivability[0] in salt_reason:
                    other_salt_reason = salt_reason.replace(salt_survivability[0], salt_survivability[1])
                elif salt_survivability[1] in salt_reason:
                    other_salt_reason = salt_reason.replace(salt_survivability[1], salt_survivability[0])
                if other_salt_reason and other_salt_reason not in salt_reasons:
                    salt_reasons[other_salt_reason] = (pid, time)
                    unique_salt_reason = True

            unique_bcd_reason = False
            bcd_reason = tags["bcd_sieve_reason"].replace('\n', ' ')
            if bcd_reason not in bcd_reasons:
                bcd_reasons[bcd_reason] = (pid, time)
                unique_bcd_reason = True
            if unique_start_reason or unique_salt_reason or unique_bcd_reason:
                vitals = triage["vitals"]
                icl_vitals.append((pid, time, vitals))
                if salt_survivability[0] in salt_reason or salt_survivability[1] in salt_reason:
                    other_vitals = vitals.copy()
                    other_vitals["survivable_injuries"] = not other_vitals["survivable_injuries"]
                    icl_vitals.append((pid, f"!{time}", other_vitals))
                break  # We take, at most, 1 visit from a casualty

    # The example set does not have a triage state where an airway obstruction is recoverable
    # So let's do this manually and get the vitals for it
    ao_vitals = _find_recoverable_airway_obstruction_vitals(dataset, True)
    icl_vitals.append((len(study_run)+1, "ao-i", ao_vitals))
    not_ao_vitals = ao_vitals.copy()
    not_ao_vitals["survivable_injuries"] = not not_ao_vitals["survivable_injuries"]
    icl_vitals.append((len(study_run)+1, "!ao-i", not_ao_vitals))
    ao_vitals = _find_recoverable_airway_obstruction_vitals(dataset, False)
    icl_vitals.append((len(study_run) + 1, "ao", ao_vitals))

    _log.info(f"Found {len(start_reasons)} START reasons")
    for reason, pid in start_reasons.items():
        _log.info(f"\t{reason} from {pid}")

    _log.info(f"Found {len(salt_reasons)} SALT reasons")
    for reason, pid in salt_reasons.items():
        _log.info(f"\t{reason} from {pid}")

    _log.info(f"Found {len(bcd_reasons)} BCD Sieve reasons")
    for reason, pid in bcd_reasons.items():
        _log.info(f"\t{reason} from {pid}")

    icl_casualties = {}
    for items in icl_vitals:
        pid = items[0]
        time = items[1]
        vitals: dict = items[2]

        start_color, start_reason = TriageStudy.start_tag(vitals)
        salt_color, salt_reason = TriageStudy.salt_tag(vitals)
        bcd_color, bcd_reason = TriageStudy.bcd_sieve_tag(vitals)
        triage = {
            "vitals": vitals,
            "tags": {"start": start_color,
                     "start_reason": start_reason,
                     "salt": salt_color,
                     "salt_reason": salt_reason,
                     "bcd_sieve": bcd_color,
                     "bcd_sieve_reason": bcd_reason},
            "injury_description": "",
            "vitals_description": dataset.vitals_description(vitals)
        }
        if pid not in icl_casualties:
            icl_casualties[pid] = {"visits": {}}
        visits = icl_casualties[pid]["visits"]
        if time not in visits:
            visits[time] = {"triage": triage}
    create_align_file(icl_casualties, Path("./test_results/itm/triage_study/post_processing/align_icl.json"))


if __name__ == "__main__":
    main()
