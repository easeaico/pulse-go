# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.
import copy
import logging
import json
import shutil

from pathlib import Path

from pulse.cdm.engine import SEDataRequestManager
from pulse.cdm.patient_actions import SEAirwayObstruction
from pulse.cdm.scalars import FrequencyUnit, TimeUnit
from pulse.engine.PulseEngine import PulseEngine
from pulse.study.in_the_moment.army_dataset import ArmyDataset
from pulse.study.in_the_moment.triage_post_processor import create_align_dataset, create_align_markdown
from pulse.study.in_the_moment.triage_dataset import convert_keys_to_int, PulseData, TriageColor
from triage_study_pipeline import TriageStudy, Dataset

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


def generate_icl_examples():

    # For our training set, we want to gather a set of casualties that contain all tag colors and all tag reasons
    # First we start by programmatically pulling out cases from our example casualty set with unique tag color/reasons
    # Manually examining these sets, we created the following cases for tag color/reasons not in this dataset

    with open(Path("./test_results/itm/triage_study/example_casualties.json"), 'r') as f:
        study_run = json.load(f, object_hook=convert_keys_to_int)

    # Map these reasons to the same casualty no matter what
    salt_survivability = ["Casualty is likely to survive these injuries.",
                          "Casualty is NOT likely to survive these injuries."]

    # TODO Add dataset arg when we add more datasets, we need to know what dataset the casualty file is from
    dataset = ArmyDataset()
    dataset_name = Dataset.Army

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
                desc = triage["injury_description"]
                icl_vitals.append((pid, time, vitals, desc))
                if salt_survivability[0] in salt_reason or salt_survivability[1] in salt_reason:
                    other_vitals = vitals.copy()
                    other_vitals["survivable_injuries"] = not other_vitals["survivable_injuries"]
                    icl_vitals.append((pid, f"!{time}", other_vitals, desc))
                break  # We take, at most, 1 visit from a casualty

    # The example set does not have a triage state where an airway obstruction is recoverable
    # So let's do this manually and get the vitals for it
    ao_vitals = _find_recoverable_airway_obstruction_vitals(dataset, True)
    icl_vitals.append((len(study_run) + 1, "ao-i", ao_vitals, ""))
    not_ao_vitals = ao_vitals.copy()
    not_ao_vitals["survivable_injuries"] = not not_ao_vitals["survivable_injuries"]
    icl_vitals.append((len(study_run) + 1, "!ao-i", not_ao_vitals, ""))
    ao_vitals = _find_recoverable_airway_obstruction_vitals(dataset, False)
    icl_vitals.append((len(study_run) + 1, "ao", ao_vitals, ""))

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
        desc = items[3]

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
            "injury_description": desc,
            "vitals_description": dataset.vitals_description(vitals)
        }
        if pid not in icl_casualties:
            icl_casualties[pid] = {"visits": {}}
        visits = icl_casualties[pid]["visits"]
        if time not in visits:
            visits[time] = {"triage": triage}

    align = create_align_dataset(icl_casualties, "example_icl")
    out_file = Path("./test_results/itm/triage_study/post_processing/align_example_icl.json")
    _log.info(f"Writing {out_file}")
    with open(out_file, 'w') as file:
        json.dump(align, file, indent=2)
    dl_file = Path("./docs/html/files/itm/army/itm_align_example_icl.json")
    _log.info(f"Copying to {dl_file}")
    shutil.copyfile(out_file, dl_file)
    output_md_dir = Path("./docs/markdown/itm")
    output_md_dir.mkdir(parents=True, exist_ok=True)
    create_align_markdown(dataset_name.value, "ex_icl", align, output_md_dir)


def compare_results():

    triage_study = Path("./test_results/itm/triage_study/1000_casualties.json")
    with open(triage_study, 'r') as f:
        study_run = json.load(f, object_hook=convert_keys_to_int)

    align_output_dir = Path("./test_results/itm/triage_study/align/outputs_20250731")
    eval_type = "fewshot"
    protocol = "bcd"
    align_results = align_output_dir / f"eval_{eval_type}/{protocol}/input_output.json"
    with open(align_results, 'r') as f:
        align_run = json.load(f)

    num_study_runs = len(study_run)
    num_align_runs = len(align_run)
    if num_study_runs != num_align_runs:
        _log.warning(f"Number of casualties does not match. Triage: {num_study_runs} vs Align: {num_align_runs}")

        if num_study_runs > num_align_runs:
            align_casualties = []
            # Get all the casualty id's in align
            for align in align_run:
                probe_id = align["input"]["full_state"]["meta_info"][0].split('_')
                casualty_id = int(probe_id[1])
                time = probe_id[3][:4]
                align_casualties.append(casualty_id)
            missing = []
            for tid, run in study_run.items():
                if tid not in align_casualties:
                    missing.append(tid)
                    if len(run["visits"]) == 0:
                        if "death" in run:
                            if run["death"]["time"] < 15:
                                missing.pop(-1)
            if len(missing):
                for missed in missing:
                    _log.error(f"Align is missing casualty {missed}")
            else:  # TODO Could work on getting align to recognize deceased casualty descriptions
                _log.info(f"\tIt's ok, missing patients died before our triage time and were not included for eval.")
        else:
            _log.fatal("There are more probes than casualties, (probably multiple times per casualty). "
                       "This code id not set up for that yet.")
            exit(1)

    if protocol == "bcd":
        protocol = protocol + "_sieve"
    triage_colors = list(TriageColor)

    align_tag_counts = {TriageColor.Green: 0, TriageColor.Yellow: 0, TriageColor.Red: 0, TriageColor.Black: 0}
    triage_tag_counts = copy.deepcopy(align_tag_counts)

    num_missed = 0
    missed_injuries = {}
    for align in align_run:
        probe_id = align["input"]["full_state"]["meta_info"][0].split('_')
        casualty_id = int(probe_id[1])
        time = probe_id[3][:4]
        expected_align_tag = None
        for i, label in enumerate(align["label"]):
            if protocol.upper() in label:
                expected_align_tag = triage_colors[i]
        align_tag = triage_colors[align["output"]["action"]["action_id"]]
        align_reason = align["output"]["action"]["justification"]

        triage = study_run[casualty_id]["visits"][time]["triage"]
        triage_tag = triage["tags"][protocol]

        if expected_align_tag != triage_tag:
            _log.error(f"Uh-oh")
        align_tag_counts[align_tag] += 1
        triage_tag_counts[triage_tag] += 1

        if triage_tag != align_tag:
            num_missed += 1
            _log.warning(f"Casualty {casualty_id} tag does not match")
            _log.warning(f"\t Triage: {triage_tag}: {triage['tags'][f'{protocol}_reason']}")
            _log.warning(f"\t Align: {align_tag}: {align_reason}")
            for injury in study_run[casualty_id]["specification"]["injuries"]:
                inj = f"{injury['location']}-{injury['type']}"
                _log.warning(f"\t\t{inj} {injury['severity']}")
                if inj not in missed_injuries:
                    missed_injuries[inj] = 0
                missed_injuries[inj] += 1
            _log.warning(f"Description:\n{align['input']['state']}")

    _log.info(f"Align missed {num_missed} tags out of {num_align_runs} ({num_missed/num_align_runs*100}%)")
    for inj, cnt in missed_injuries.items():
        _log.info(f"{inj}: {cnt}")


def main():
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

    generate_icl_examples()

    # compare_results()


if __name__ == "__main__":
    main()
