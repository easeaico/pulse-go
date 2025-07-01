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
from pulse.cdm.utils.markdown import table as markdown_table

_logger = logging.getLogger('log')


def _create_table(data, fields, headings) -> str:
    align = []
    for i in range(len(fields)):
        align.append(('^', '^'))
    table_io = io.StringIO()
    markdown_table(table_io, data, fields, headings, align)
    table_str = table_io.getvalue()
    table_io.close()
    return table_str


def create_army_test_dataset_documentation(study_run: dict, output_dir):
    itm_dir = output_dir / f"itm/army"
    itm_dir.mkdir(exist_ok=True, parents=True)

    src_dir = Path(get_root_dir()) / "src/python/pulse/study/in_the_moment"
    src_homepage = src_dir/"army_test_dataset.md"
    dst_homepage = itm_dir/"army_test_dataset.md"
    if not src_homepage.exists():
        _logger.error(f"Unable to find homepage: {src_homepage}")
        exit(1)
    shutil.copy(src_homepage, dst_homepage)

    tgts = [("head_and_neck", "airway_obstruction"),
            ("head_and_neck", "superficial"),
            ("head_and_neck", "tbi")
            ]

    for tgt in tgts:
        for pid, run in study_run.items():
            tgt_injuries = run["synthetic_patient"]["injuries"]
            sev = []
            for tgt_injury in tgt_injuries:
                if tgt_injury["location"] == tgt[0] and tgt_injury["type"] == tgt[1]:
                    sev.append(tgt_injury["severity"])

            if len(sev) == 0:
                continue

            _logger.info(f"Patient {pid} meets criteria")

            data = []
            headings = ["Vitals", "Description", "START", "SALT", "BCD"]
            fields = [0, 1, 2, 3, 4]
            for time, visit in run["visits"].items():
                vitals = {"duration": time} | visit["triage"]["vitals"]
                vitals = json2html.convert(json=json.dumps(vitals),
                                           table_attributes='border="1" cellpadding="15"')
                vitals = vitals.replace(". ", "<br> ")
                desc = ""
                for sentence in visit["triage"]["description"]:
                    desc += f"{sentence}<br>"
                tags = visit["triage"]["tags"]
                start_reason = tags['start_reason'].replace('\n', '<br>')
                salt_reason = tags['salt_reason'].replace('\n', '<br>')
                bcd_sieve_reason = tags['bcd_sieve_reason'].replace('\n', '<br>')
                data.append((vitals, desc,
                             f"{tags['start']}<br>&nbsp;<br>{start_reason}",
                             f"{tags['salt']}<br>&nbsp;<br>{salt_reason}",
                             f"{tags['bcd_sieve']}<br>&nbsp;<br>{bcd_sieve_reason}"))
            # Sort our results
            data = sorted(data, key=lambda element: (element[0], element[1]))
            table = _create_table(data=data, fields=fields, headings=headings)
            anchor = f"{tgt[0]}_{tgt[1]}_{sev[0]}".replace('.', '_')
            filename = itm_dir/f"patient_{pid}_{anchor}.md"
            _logger.info(f"Writing table {filename}")
            with open(filename, 'w') as file:
                file.write(f"\\anchor {anchor}\n")
                file.write(table)
                file.write(f"<center><i>Casualty with a {tgt[1]} on the {tgt[0]} with severity {sev[0]}</i></center>")


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
