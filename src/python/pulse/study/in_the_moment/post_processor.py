# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import argparse
import logging
import io
import json
import shutil

from pathlib import Path
from json2html import *

from pulse.cdm.utils.file_utils import get_root_dir
from pulse.cdm.utils.markdown import process_file as process_markdown
from pulse.cdm.utils.markdown import table as markdown_table

_logger = logging.getLogger('log')


def _create_table(triage: dict) -> str:
    fields = [0, 1, 2, 3, 4]
    headings = ["Vitals", "Description", "START", "SALT", "BCD"]
    align = []
    for i in range(len(fields)):
        align.append(('^', '^'))

    vitals = triage["vitals"]
    vitals = json2html.convert(json=json.dumps(vitals),
                               table_attributes='border="1" cellpadding="15"')
    vitals = vitals.replace(". ", "<br> ")
    desc = ""
    for sentence in triage["description"]:
        desc += f"{sentence}<br>"
    tags = triage["tags"]
    start_reason = tags['start_reason'].replace('\n', '<br>')
    salt_reason = tags['salt_reason'].replace('\n', '<br>')
    bcd_sieve_reason = tags['bcd_sieve_reason'].replace('\n', '<br>')
    data = [(vitals, desc,
             f"{tags['start']}<br>&nbsp;<br>{start_reason}",
             f"{tags['salt']}<br>&nbsp;<br>{salt_reason}",
             f"{tags['bcd_sieve']}<br>&nbsp;<br>{bcd_sieve_reason}")]

    table_io = io.StringIO()
    markdown_table(table_io, data, fields, headings, align)
    table_str = table_io.getvalue()
    table_io.close()
    return table_str


def create_army_test_dataset_documentation(study_run: dict, output_dir):
    itm_dir = output_dir / f"itm/army"
    itm_dir.mkdir(exist_ok=True, parents=True)

    # Let's sort the injuries into their categories
    tgts = {
            "head_and_neck": {
                "airway_obstruction": [],
                "superficial": [],
                "tbi": []
            },
            "thorax": {
                "fracture": [],
                "hemothorax": [],
                "hemorrhage": [],
                "pneumothorax": [],
                "pulmonary_contusion": [],
                "spinal": []
            },
            "abdomen": {
                "hemorrhage": [],
                "laceration_contusion": []
                },
            "extremity":  {
                "burn_nerve": [],
                "contusion_sprain_strain": [],
                "fracture_dislocation": [],
                "hemorrhage": []
                }
            }

    for pid, run in study_run.items():
        tgt_injuries = run["synthetic_patient"]["injuries"]
        if len(tgt_injuries) == 1:
            loc = tgt_injuries[0]["location"]
            typ = tgt_injuries[0]["type"]
            sev = tgt_injuries[0]["severity"]
        else:  # Get the injury with the greatest severity
            loc = None
            typ = None
            sev = 0
            for i in tgt_injuries:
                if i["severity"] > sev:
                    loc = i["location"]
                    typ = i["type"]
                    sev = i["severity"]

        anchor = f"casualty_{pid}"
        filename = itm_dir / f"{anchor}.md"
        _logger.info(f"Writing documentation to {filename}")
        tgts[loc][typ].append(filename)

        with open(filename, 'w') as file:
            file.write(f"\\anchor {anchor}\n\n")
            file.write(f"Each section below is independent and represents the duration time since the injury.\n\n")
            file.write(f"These sections represent the status of the patient as if a medical professional "
                       f"just arrived to triage the patient at the provided time.\n\n")
            file.write(f"### Injury\n\n")
            if len(tgt_injuries) == 1:
                file.write(f"Casualty has a {typ} on the {loc} with severity {sev}\n\n")
            else:
                file.write(f"Casualty found with multiple injuries")
                for i in tgt_injuries:
                    file.write(f"Casualty has a {i['type']} on the {i['location']} with severity {i['severity']}\n")
                file.write(f"\n")
            if "death" in run:
                file.write(f"### Death\n\n")
                file.write(f"If left untreated, "
                           f"casualty will die <b>~{int(run['death']['time'])} min</b> from the initial injury.\n\n")
                file.write(f"<b>Cause of Death:</b>  {run['death']['cause']}\n\n")

            for time, visit in run["visits"].items():
                triage = visit["triage"]
                table = _create_table(triage)
                file.write(f"### Triage Tag Time: {time} min from point of injury\n\n")
                file.write(table)
                file.write("\n\n")
                if "intervention" in visit:
                    file.write(f"#### Intervention\n\n")
                    file.write(f"Casualty state a hour after the following intervention is performed.\n")
                    file.write(f"<Intervention Description>\n")  # TODO Use intervention string
                    triage = visit["intervention"]
                    # TODO check to see if the patient dies even with intervention
                    table = _create_table(triage)
                    file.write(table)
                    file.write("\n\n")

    for loc, types in tgts.items():
        for typ, files in types.items():
            filename = itm_dir / f"{loc}_{typ}_table.md"
            _logger.info(f"Writing documentation to {filename}")
            with open(filename, 'w') as file:
                file.write("@secreflist\n")
                for f in files:
                    fp = Path(f)
                    anchor = fp.stem
                    name = anchor.capitalize().replace('_', ' ')
                    file.write(f"\t@refitem {fp.stem} \"{name}\"\n")
                file.write("@endsecreflist\n")

    # Update our landing page with all these runs
    src = Path(get_root_dir()) / "src/python/pulse/study/in_the_moment/army_test_dataset.md"
    process_markdown(src, itm_dir, itm_dir)


def main():
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
    parser = argparse.ArgumentParser(description="Process the full pipeline for segment validation")
    parser.add_argument(
        "-o", "--output_dir",
        type=Path,
        default="./test_results/itm/triage_study/post_processing/",
        help="Location to put all files related to this study"
    )
    parser.add_argument(
        "-p", "--population_file",
        type=Path,
        default=Path("./test_results/itm/triage_study/triage_study.json"),
        help="Location to put all files related to this study"
    )
    opts = parser.parse_args()
    output_dir = opts.output_dir
    output_dir.mkdir(parents=True, exist_ok=True)

    with open(opts.population_file, 'r') as file:
        study_run = json.load(file)

    create_army_test_dataset_documentation(study_run, Path("./docs/markdown"))


if __name__ == "__main__":
    main()
