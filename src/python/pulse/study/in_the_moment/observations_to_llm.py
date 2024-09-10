# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import sys
import json
import logging
from typing import Dict
from pathlib import Path


_pulse_logger = logging.getLogger('pulse')


def observations_to_llm(filename: Path, tag_counts: Dict[str, Dict[str, int]]):
    """
    Post process the observation json for LLM input
    """
    if not filename.exists():
        _pulse_logger.error(f"Could not find file {filename}")
    # Replace "observations" in filepath with "llm"
    parts = list(filename.parts)
    parts[parts.index("observations")] = "llm"
    out_file = Path(*parts)
    out_file.parent.mkdir(parents=True, exist_ok=True)

    # Read observation file into dictionary
    try:
        with open(filename) as f:
            observations = json.load(f)
    except:
        _pulse_logger.error(f"Could not load file {filename}")
        return

    # Iterate through each observation to pull out desired info
    out_obs = list()
    for obs in observations["Observations"]:
        out_obs.append(dict())
        out_obs[-1]["scenario_id"] = f"{filename.parts[-2]}/{filename.stem}"
        time = str(obs["SimTime_min"])+"m"
        out_obs[-1]["probe_id"] = f"{filename.parts[-2]}/{filename.stem}@{time}"
        out_obs[-1]["state"] = None

        # Fill in the scenario text
        # We can just copy over the generated unstructured text
        if "Description" in obs:
            out_obs[-1]["scenario"] = obs["Description"]
        # We can just build a dict of selected values and dump that
        # sdict = dict()
        # sdict["HR_bpm"] = obs["HR_bpm"]
        # sdict["RR_bpm"] = obs["RR_bpm"]
        # sdict["SBP_mmHg"] = obs["SBP_mmHg"]
        # sdict["CoreTemp_C"] = obs["CoreTemp_C"]
        # sdict["PPI"] = obs["PPI"]
        # sdict["BO2PP_mmHg"] = obs["BO2PP_mmHg"]
        # sdict["GCS"] = obs["GCS"]
        # sdict["walk_ability"] = obs["walk_ability"]
        # optional_keys = ["SkinColor", "SkinTouch", "CRT_s", "RadialPulsePalpability"]
        # for key in optional_keys:
        #     if key in obs:
        #         sdict[key] = obs[key]
        # out_obs[-1]["scenario"] = sdict

        # Ask our tagging question, and provide the choices array
        out_obs[-1]["probe"] = "After a nearby explosion, you come across a wounded person lying on the ground. What triage category would you assign?"

        tag_choices = {"GREEN": "Minimal", "YELLOW": "Delayed", "RED": "Immediate", "BLACK": "Expectant"}
        choices = [
            f"Tag civilian as {tag_color} for {tag_status}." for tag_color, tag_status in tag_choices.items()
        ]
        out_obs[-1]["choices"] = choices  # Note this list will continue to change if you modify it

        # Compute accuracy scores for probe choices
        out_obs.append(list())
        for tag_color, tag_status in tag_choices.items():
            acc = 0.
            if tag_color.lower() == obs["START"].lower():
                acc = 10.0
            out_obs[-1].append({"accuracy": acc})

        # Accumulate tag counts
        tag_algos = ["START"] #, "BCD"]
        for tag_algo in tag_algos:
            if tag_algo not in tag_counts:
                tag_counts[tag_algo] = {}
            if obs[tag_algo] not in tag_counts[tag_algo]:
                tag_counts[tag_algo][obs[tag_algo]] = 0
            tag_counts[tag_algo][obs[tag_algo]] += 1

    # Write out new structure
    with open(out_file, "w") as f:
        _pulse_logger.info(f"Writing {out_file}")
        f.write(json.dumps(out_obs, indent=2))


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

    """
    parser = argparse.ArgumentParser(description="Postprocess a directory of observation json(s)")
    parser.add_argument(
        "filenames",
        type=Path,
        nargs="+",
        help="File(s) to process"
    )
    args = parser.parse_args()
    """
    mode = "itm"
    if len(sys.argv) > 1:
        mode = sys.argv[1]
    injury_scenarios_list_filename = f"./test_results/patient_variability/{mode}/scenarios/tccc.json"
    obs_dir = Path(f"test_results/patient_variability/{mode}/observations")
    obs_files = list(obs_dir.rglob('*.json'))

    tag_counts = dict()
    for in_file in obs_files:
        observations_to_llm(in_file, tag_counts)

    # Print tag counts
    for tag_algo, counts in tag_counts.items():
        _pulse_logger.info(f"Total {tag_algo} Tag Counts ({len(obs_files)} patients):")
        for tag, count in counts.items():
            _pulse_logger.info(f"\t{tag}: {count}")
