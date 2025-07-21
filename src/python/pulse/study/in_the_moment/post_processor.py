# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import argparse
import logging
import io
import json

from pathlib import Path
from json2html import *

from casualty_generation import (calculate_population_error, calculate_injury_error,
                                 plot_population_error, plot_injury_error, to_specification_lists)

from army_dataset import population_distributions as army_population_distributions
from army_dataset import injury_distributions as army_injury_distributions

from pulse.cdm.utils.file_utils import get_root_dir
from pulse.cdm.utils.markdown import process_file as process_markdown
from pulse.cdm.utils.markdown import table as markdown_table
from pulse.study.in_the_moment.triage_dataset import Intervention
from pulse.study.in_the_moment.triage_study_pipeline import Dataset

_logger = logging.getLogger('log')


def _create_table(triage: dict) -> str:
    fields = [0, 1, 2, 3, 4]
    headings = ["Vitals", "Description", "START", "SALT", "BCD"]
    align = []
    for i in range(len(fields)):
        align.append(('^', '^'))

    vitals = triage["vitals"]
    # Clean up some float formatting
    vitals["heart_rate"] = int(vitals["heart_rate"])
    vitals["respiratory_rate"] = int(vitals["respiratory_rate"])
    vitals["spO2"] = int(vitals['spO2']*100)
    vitals["systolic_pressure"] = int(vitals["systolic_pressure"])
    vitals["diastolic_pressure"] = int(vitals["diastolic_pressure"])
    if "brain_o2_pp" in vitals:
        vitals["brain_o2_pp"] = int(vitals["brain_o2_pp"])
    vitals = json2html.convert(json=json.dumps(vitals),
                               table_attributes='border="1" cellpadding="15"')
    vitals = vitals.replace(". ", "<br> ")
    desc = ""
    for sentence in triage["injury_description"]:
        sentence = sentence.replace('\n', '<br>')
        desc += f"{sentence}<br>"
    desc += f"<br>"
    for sentence in triage["vitals_description"]:
        sentence = sentence.replace('\n', '<br>')
        desc += f"{sentence}<br>"
    desc += f"<br>&nbsp;<br>TRISS: {triage['triss']:.2f}%<br>NEWS: {triage['news']}"

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


def create_markdown(set_name: str, set_type: str, study_run: dict, output_dir):
    output_md_dir = output_dir / f"{set_name}/{set_type}"
    output_md_dir.mkdir(exist_ok=True, parents=True)

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
        tgt_injuries = run["specification"]["injuries"]
        if len(tgt_injuries) == 1:
            loc = tgt_injuries[0]["location"]
            typ = tgt_injuries[0]["type"]
            styp = tgt_injuries[0]["sub_type"]
            cmpt = tgt_injuries[0]["cmpt"]
            sev = tgt_injuries[0]["severity"]
            if tgt_injuries[0]["can_intervene"]:
                intervention = "i"
            else:
                intervention = ""
        else:  # Get the injury with the greatest severity
            loc = None
            typ = None
            styp = None
            cmpt = None
            sev = 0
            intervention = ""
            for i in tgt_injuries:
                if i["severity"] > sev:
                    loc = i["location"]
                    typ = i["type"]
                    sev = i["severity"]
                    styp = i["sub_type"]
                    cmpt = i["cmpt"]
                    if i["can_intervene"]:
                        intervention = "i"
                    else:
                        intervention = ""

        anchor = f"itm_{set_name}_{set_type}_casualty_{pid}"
        filename = output_md_dir / f"{anchor}.md"
        _logger.info(f"Writing documentation to {filename}")
        extra = ""
        if styp:
            extra += f"_[{styp}]"
        if cmpt:
            extra += f"_[{cmpt}]"
        if "death" in run:
            extra += f"_[{int(run['death']['time'])}min]"
        tgts[loc][typ].append((filename, f"casualty_{pid}_({sev}{intervention}){extra}"))

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
                    for intervention in triage["vitals"]["interventions"]:
                        if intervention == Intervention.RepositionAirway:
                            file.write(f"The casualty's head was repositioned to open the airway.\n")
                            continue
                        if intervention == Intervention.Tourniquet:
                            file.write(f"A tourniquet was applied to the casualty's wounded extremity.\n")
                        if intervention == Intervention.WoundPack:
                            file.write(f"Gauze was use to pack the casualty's wounded.\n")
                    final_triage = visit["intervention"]
                    if "vitals" in final_triage:
                        file.write(f"Casualty state an hour after the intervention is performed.\n\n")
                        table = _create_table(final_triage)
                    elif "death" in final_triage:
                        table = (f"Casualty does not survive with intervention. "
                                 f"{final_triage['death']['cause']} at time {final_triage['death']['time_s']}")
                    else:
                        _logger.fatal("Casualty has not vitals and did not die???")
                        exit(1)

                    file.write(table)
                    file.write("\n\n")

            if "death" in run:
                triage = run["death"]["triage"]
                table = _create_table(triage)
                file.write(f"### Time of Death: {run['death']['time']:.1f} min from point of injury\n\n")
                file.write(table)
                file.write("\n\n")

    for loc, types in tgts.items():
        for typ, files in types.items():
            filename = output_md_dir / f"itm_{set_name}_{set_type}_{loc}_{typ}_table.md"
            _logger.info(f"Writing documentation to {filename}")
            with open(filename, 'w') as file:
                file.write("@secreflist\n")
                for f in files:
                    fp = Path(f[0])
                    name = f[1].capitalize().replace('_', ' ')
                    file.write(f"\t@refitem {fp.stem} \"{name}\"\n")
                file.write("@endsecreflist\n")

    # Update our landing page with all these runs
    src = Path(get_root_dir()) / f"src/python/pulse/study/in_the_moment/docs/itm_{set_name}_{set_type}_dataset.md"
    if src.exists():
        process_markdown(src, output_md_dir, output_md_dir)
    else:
        _logger.error(f"Unable to find markdown file: {src}")


def create_align_file(study_run: dict, filename: Path, include_reasonings: bool):
    preamble = ("You are an army combat medic. "
                "There has been an targeted strike on your base resulting in many wounded soldiers."
                "You need to triage and tag each casualty as quickly as possible."
                "You come to your first casualty.")
    responses = ["Green (Minor)",
                 "Yellow (Delayed)",
                 "Red (Immediate)",
                 "Black (Expectant)"]

    cases = []

    def _tag_index(color: str) -> int:
        if color == "Green":
            return 0
        if color == "Yellow":
            return 1
        if color == "Red":
            return 2
        return 3

    for pid, run in study_run.items():
        for time, visit in run["visits"].items():
            triage = visit["triage"]
            prompt = (preamble +
                      "\n\n".join(triage["injury_description"]) +
                      "\n\n".join(triage["vitals_description"]) +
                      "\n\nHow do you tag this casualty?")
            tags = triage["tags"]
            p_labels = {"START": 0.0, "SALT": 0.0, "BCD_SIEVE": 0.0}
            p_reasonings = {"START": "", "SALT": "", "BCD_SIEVE": ""}
            labels = []
            reasonings = []
            for i in range(4):
                labels.append(p_labels.copy())
                reasonings.append(p_reasonings.copy())
            i = _tag_index(tags["start"])
            labels[i]["START"] = 1.0
            if include_reasonings:
                reasonings[i]["START"] = tags["start_reason"]
            i = _tag_index(tags["salt"])
            labels[i]["SALT"] = 1.0
            if include_reasonings:
                reasonings[i]["SALT"] = tags["salt_reason"]
            i = _tag_index(tags["bcd_sieve"])
            labels[i]["BCD_SIEVE"] = 1.0
            if include_reasonings:
                reasonings[i]["BCD_SIEVE"] = tags["bcd_sieve_reason"]
            case = {"prompt": prompt, "responses": responses, "labels": labels}
            if include_reasonings:
                case["reasonings"] = reasonings
            cases.append(case)

    with open(filename, 'w') as file:
        json.dump(cases, file, indent=2)


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
        "-tf", "--train_file",
        type=Path,
        default=Path("./test_results/itm/triage_study/training_casualties.json"),
        help="Triage study training file"
    )
    parser.add_argument(
        "-ef", "--eval_file",
        type=Path,
        default=Path("./test_results/itm/triage_study/training_casualties.json"),
        help="Triage study evaluation file"
    )
    parser.add_argument(
        "-md", "--markdown",
        action='store_true',
        help="Generate markdown files from both provided study files"
    )
    parser.add_argument(
        "-ta2", "--to_align_input",
        action='store_true',
        help="Generate align input files from both provided study files"
    )
    parser.add_argument(
        "-dt", "--distribution_tables",
        action='store_true',
        help="Create distribution tables for provided eval file only"
    )
    opts = parser.parse_args()

    # TODO Add dataset arg when we add more datasets
    dataset = Dataset.Army

    output_dir = opts.output_dir / dataset.value
    output_dir.mkdir(parents=True, exist_ok=True)

    output_md_dir = None
    if opts.markdown:
        output_md_dir = Path("./docs/markdown/itm")
        output_md_dir.mkdir(parents=True, exist_ok=True)
        # Update our landing page with all these runs
        src = Path(get_root_dir()) / "src/python/pulse/study/in_the_moment/docs/itm_triage_datasets.md"
        if src.exists():
            process_markdown(src, output_md_dir, output_md_dir)
        else:
            _logger.error(f"Unable to find markdown file: {src}")

    if opts.train_file.exists():
        with open(opts.train_file, 'r') as file:
            study = json.load(file)

        if opts.markdown:
            create_markdown(dataset.value, "train", study, output_md_dir)

        if opts.to_align_input:
            create_align_file(study, output_dir/f"align_{opts.train_file.stem}.json", include_reasonings=True)

    if opts.eval_file.exists():
        with open(opts.eval_file, 'r') as file:
            study = json.load(file)

        if opts.markdown:
            create_markdown(dataset.value, "eval", study, output_md_dir)

        if opts.to_align_input:
            create_align_file(study, output_dir/f"align_{opts.eval_file.stem}.json", include_reasonings=False)

        if opts.distribution_tables:
            # Write out the error images for this generated dataset
            spec = to_specification_lists(study)
            results_dir = output_dir / opts.distribution_tables.stem
            results_dir.mkdir(parents=True, exist_ok=True)
            results_stem = str(results_dir/opts.distribution_tables.stem)
            population_error = calculate_population_error(spec, army_population_distributions)
            plot_population_error(population_error, results_stem)

            injury_error = calculate_injury_error(spec["injuries"], army_injury_distributions)
            plot_injury_error(injury_error, results_stem)


if __name__ == "__main__":
    main()
