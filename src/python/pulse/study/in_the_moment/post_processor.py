# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import argparse
import logging
import io
import json
import shutil

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
                file.write(f"Casualty found with multiple injuries.\n")
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
                        file.write(f"Casualty dies at time {final_triage['death']['time']:.1f} min. "
                                   f"{final_triage['death']['cause']}")
                                   #f"Intervention extended life {final_triage['death']['time']-run['death']['time']:.1f} min.")
                        file.write("\n\n")
                        triage = final_triage["death"]["triage"]
                        table = _create_table(triage)
                    else:
                        _logger.fatal("Casualty has not vitals and did not die???")
                        exit(1)

                    file.write(table)
                    file.write("\n\n")

            if "death" in run:
                triage = run["death"]["triage"]
                table = _create_table(triage)
                file.write(f"### Final State if no interventions are applied: "
                           f"Death occurs {run['death']['time']:.1f} min from point of injury\n\n")
                file.write(table)
                file.write("\n\n")
            elif "final" in run:
                triage = run["final"]["triage"]
                table = _create_table(triage)
                file.write(f"### Final State if no interventions are applied: "
                           f"{run['final']['time']:.1f} min from point of injury\n\n")
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


def create_align_file(study_run: dict, filename: Path):
    responses = ["Green (Minor)",
                 "Yellow (Delayed)",
                 "Red (Immediate)",
                 "Black (Expectant)"]

    def _tag_index(color: str) -> int:
        if color == "Green":
            return 0
        if color == "Yellow":
            return 1
        if color == "Red":
            return 2
        return 3

    cases = []
    for pid, run in study_run.items():
        for time, visit in run["visits"].items():
            triage = visit["triage"]
            prompt = ("".join(triage["injury_description"]) +
                      "\n\n".join(triage["vitals_description"]))
            tags = triage["tags"]
            choices = [{}, {}, {}, {}]
            for i, choice in enumerate(choices):
                choice["action_id"] = i
                choice["unstructured"] = responses[i]
                choice["kdma_association"] = {}
            labels = [{}, {}, {}, {}]
            reasonings = [{}, {}, {}, {}]
            # START
            i = _tag_index(tags["start"])
            choices[i]["kdma_association"]["START"] = 1.0
            labels[i]["START"] = 1.0
            reasonings[i]["START"] = tags["start_reason"]
            # SALT
            i = _tag_index(tags["salt"])
            choices[i]["kdma_association"]["SALT"] = 1.0
            labels[i]["SALT"] = 1.0
            reasonings[i]["SALT"] = tags["salt_reason"]
            # BCD Sieve
            i = _tag_index(tags["bcd_sieve"])
            choices[i]["kdma_association"]["BCD_SIEVE"] = 1.0
            labels[i]["BCD_SIEVE"] = 1.0
            reasonings[i]["BCD_SIEVE"] = tags["bcd_sieve_reason"]

            case = {"input": {
                       "scenario_id": filename.stem,
                       "full_state": {
                           "unstructured": prompt,
                           "meta_info": { "scene_id": f"Casualty_{pid}_at_{time}min"},
                           "scenario_complete": False},
                       "state": prompt,
                       "choices": choices,
                       "label": labels,
                       "reasoning": reasonings}}
            cases.append(case)

    with open(filename, 'w') as file:
        json.dump(cases, file, indent=2)


def _create_align_table(scenario: dict) -> str:
    fields = [0, 1, 2, 3]
    headings = ["Description", "START", "SALT", "BCD"]
    alignment = []
    for i in range(len(fields)):
        alignment.append(('^', '^'))

    colors = ["Green",
              "Yellow",
              "Red",
              "Black"]

    start_tag = "Green"
    start_reason = "Missing"
    salt_tag = "Green"
    salt_reason = "Missing"
    bcd_sieve_tag = "Green"
    bcd_sieve_reason = "Missing"
    for i, reasons in enumerate(scenario["input"]["reasoning"]):
        if "START" in reasons:
            start_tag = colors[i]
            start_reason = reasons["START"].replace('\n', '<br>')
        if "SALT" in reasons:
            salt_tag = colors[i]
            salt_reason = reasons["SALT"].replace('\n', '<br>')
        if "BCD_SIEVE" in reasons:
            bcd_sieve_tag = colors[i]
            bcd_sieve_reason = reasons["BCD_SIEVE"].replace('\n', '<br>')

    data = [(scenario["input"]["state"].replace('\n', '<br>'),
             f"{start_tag}<br>&nbsp;<br>{start_reason}",
             f"{salt_tag}<br>&nbsp;<br>{salt_reason}",
             f"{bcd_sieve_tag}<br>&nbsp;<br>{bcd_sieve_reason}")]

    table_io = io.StringIO()
    markdown_table(table_io, data, fields, headings, alignment)
    table_str = table_io.getvalue()
    table_io.close()
    return table_str


def create_align_markdown(set_name: str, set_type: str, align: dict, output_dir):
    anchor = f"itm_align_{set_name}_{set_type}"
    filename = output_dir / f"{anchor}.md"
    _logger.info(f"Writing documentation to {filename}")

    with open(filename, 'w') as file:
        file.write(f"\\anchor {anchor}\n\n")
        file.write(f"Each section below is independent and represents a single injured casualty and how we tag it.\n\n")
        file.write("<a href=\"./files/itm/army/itm_align_icl.json\">Download this dataset</a>\n\n")

        for scenario in align:
            file.write(f"#####{scenario['input']['full_state']['meta_info']['scene_id']}\n\n")
            file.write(_create_align_table(scenario))
            file.write(f"\n\n")


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
        "-ex", "--example_file",
        type=Path,
        default=Path("./test_results/itm/triage_study/example_casualties.json"),
        help="Triage study example file"
    )
    parser.add_argument(
        "-icl", "--icl_file",
        type=Path,
        default=Path("./test_results/itm/triage_study/post_processing/align_icl.json"),
        help="Triage study example file"
    )
    parser.add_argument(
        "-ev", "--eval_file",
        type=Path,
        default=Path("./test_results/itm/triage_study/1000_casualties.json"),
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

    downloads_dir = Path(f"./docs/html/files/itm/{dataset.value}")
    downloads_dir.mkdir(parents=True, exist_ok=True)
    if opts.example_file.exists():
        shutil.copyfile(opts.example_file, downloads_dir / "itm_example_dataset.json")
    if opts.eval_file.exists():
        shutil.copyfile(opts.eval_file, downloads_dir / "itm_eval_dataset.json")
    if opts.icl_file.exists():
        shutil.copyfile(opts.icl_file, downloads_dir / "itm_align_icl.json")

    output_md_dir = None
    if opts.markdown:
        output_md_dir = Path("./docs/markdown/itm")
        output_md_dir.mkdir(parents=True, exist_ok=True)

        def _severity(d: dict):
            return f"mean: {d['mean']:.1f}<br> stdev: {d['std']:.1f}"
        # Generate demographic table
        pop = army_population_distributions
        hr = _severity(pop['heart_rate'])
        male_p = f"{pop['sex']['male']['percent']:.1f}"
        male_ht = _severity(pop['sex']['male']['height'])
        male_bmi = _severity(pop['sex']['male']['bmi'])
        female_p = f"{pop['sex']['female']['percent']:.1f}"
        female_ht = _severity(pop['sex']['female']['height'])
        female_bmi = _severity(pop['sex']['female']['bmi'])
        with open(output_md_dir / "army_population_table.md", 'w') as file:
            file.write(f"|             |  Male     | Female     |\n")
            file.write(f"|-------------|:---------:|:----------:|\n")
            file.write(f"| Percent     | {male_p}% |{female_p}% |\n")
            file.write(f"| Height (cm) | {male_ht} |{female_ht} |\n")
            file.write(f"| BMI         | {male_bmi}|{female_bmi}|\n")
            file.write(f"| Heart Rate  | {hr}      |{hr}        |\n")

        # Generate injury table
        def _percent_severity(d: dict):
            percent = d["percent"]
            severity = d["severity"]
            if "mean" in severity:
                return percent, f"mean: {severity['mean']:.1f}<br> stdev: {severity['std']:.1f}"
            elif "values" in severity:
                dist = ""
                for i, sp in enumerate(severity["percents"]):
                    v = severity["values"][i]
                    dist += f"{sp}% are {v}<br>"
                return percent, dist
        inj = army_injury_distributions
        hn_p = inj["head_and_neck"]["percent"]
        hn_ao_p, hn_ao_s = _percent_severity(inj["head_and_neck"]["types"]["airway_obstruction"])
        hn_s_p, hn_s_s = _percent_severity(inj["head_and_neck"]["types"]["superficial"])
        hn_tbi_p, hn_tbi_s = _percent_severity(inj["head_and_neck"]["types"]["tbi"])
        t_p = inj["thorax"]["percent"]
        t_f_p, t_f_s = _percent_severity(inj["thorax"]["types"]["fracture"])
        t_hg_p, t_hg_s = _percent_severity(inj["thorax"]["types"]["hemorrhage"])
        t_hx_p, t_hx_s = _percent_severity(inj["thorax"]["types"]["hemothorax"])
        t_px_p, t_px_s = _percent_severity(inj["thorax"]["types"]["pneumothorax"])
        t_pc_p, t_pc_s = _percent_severity(inj["thorax"]["types"]["pulmonary_contusion"])
        t_s_p, t_s_s = _percent_severity(inj["thorax"]["types"]["spinal"])
        a_p = inj["abdomen"]["percent"]
        a_hg_p, a_hg_s = _percent_severity(inj["abdomen"]["types"]["hemorrhage"])
        a_lc_p, a_lc_s = _percent_severity(inj["abdomen"]["types"]["laceration_contusion"])
        e_p = inj["extremity"]["percent"]
        e_bn_p, e_bn_s = _percent_severity(inj["extremity"]["types"]["burn_nerve"])
        e_css_p, e_css_s = _percent_severity(inj["extremity"]["types"]["contusion_sprain_strain"])
        e_fd_p, e_fd_s = _percent_severity(inj["extremity"]["types"]["fracture_dislocation"])
        e_hg_p, e_hg_s = _percent_severity(inj["extremity"]["types"]["hemorrhage"])
        with open(output_md_dir / "army_injury_table.md", 'w') as file:
            file.write(f"| Location    | Type                        | Proportion |   AIS    |\n")
            file.write(f"| ----------- |-----------------------------|:----------:|:--------:|\n")
            file.write(f"| Head / Neck |                             |{hn_p}%     |          |\n")
            file.write(f"|             | Airway Obstruction          |{hn_ao_p}%  |{hn_ao_s} |\n")
            file.write(f"|             | Superficial                 |{hn_s_p}%   |{hn_s_s}  |\n")
            file.write(f"|             | Traumatic Brain Injury      |{hn_tbi_p}% |{hn_tbi_s}|\n")
            file.write(f"| Thorax      |                             |{t_p}%      |          |\n")
            file.write(f"|             | Fracture                    |{t_f_p}%    |{t_f_s}   |\n")
            file.write(f"|             | Hemorrhage                  |{t_hg_p}%   |{t_hg_s}  |\n")
            file.write(f"|             | Hemothorax                  |{t_hx_p}%   |{t_hx_s}  |\n")
            file.write(f"|             | Pneumothorax                |{t_px_p}%   |{t_px_s}  |\n")
            file.write(f"|             | Pulmonary Contusion         |{t_pc_p}%   |{t_pc_s}  |\n")
            file.write(f"|             | Spinal                      |{t_s_p}%    |{t_s_s}   |\n")
            file.write(f"| Abdomen     |                             |{a_p}%      |          |\n")
            file.write(f"|             | Hemorrhage                  |{a_hg_p}%   |{a_hg_s}  |\n")
            file.write(f"|             | Laceration / Contusion      |{a_lc_p}%   |{a_lc_s}  |\n")
            file.write(f"| Extremity   |                             |{e_p}%      |          |\n")
            file.write(f"|             | Burn / Nerve                |{e_bn_p}%   |{e_bn_s}  |\n")
            file.write(f"|             | Contusion / Sprain / Strain |{e_css_p}%  |{e_css_s} |\n")
            file.write(f"|             | Fracture / Dislocation      |{e_fd_p}%   |{e_fd_s}  |\n")
            file.write(f"|             | Hemorrhage                  |{e_hg_p}%   |{e_hg_s}  |\n")

        # Update our landing page with all these runs
        src = Path(get_root_dir()) / "src/python/pulse/study/in_the_moment/docs/itm_triage_datasets.md"
        if src.exists():
            process_markdown(src, output_md_dir, output_md_dir)
        else:
            _logger.error(f"Unable to find markdown file: {src}")

    if opts.example_file.exists():
        with open(opts.example_file, 'r') as file:
            study = json.load(file)

        if opts.markdown:
            create_markdown(dataset.value, "example", study, output_md_dir)

        if opts.to_align_input:
            create_align_file(study, output_dir/f"align_{opts.example_file.stem}.json")

    if opts.icl_file.exists():
        with open(opts.icl_file, 'r') as file:
            study = json.load(file)

        if opts.markdown:
            create_align_markdown(dataset.value, "icl", study, output_md_dir)

    if opts.eval_file.exists():
        with open(opts.eval_file, 'r') as file:
            study = json.load(file)

        if opts.distribution_tables:
            output_tb_dir = Path("./docs/html/Images/itm")
            output_tb_dir.mkdir(parents=True, exist_ok=True)
            # Write out the error images for this generated dataset
            spec = to_specification_lists(study)
            results_stem = str(output_tb_dir / "eval_casualties")
            population_error = calculate_population_error(spec, army_population_distributions)
            plot_population_error(population_error, results_stem)

            injury_error = calculate_injury_error(spec["injuries"], army_injury_distributions)
            plot_injury_error(injury_error, results_stem)

        if opts.markdown:
            create_markdown(dataset.value, "eval", study, output_md_dir)

        if opts.to_align_input:
            create_align_file(study, output_dir/f"align_{opts.eval_file.stem}.json")


if __name__ == "__main__":
    main()
