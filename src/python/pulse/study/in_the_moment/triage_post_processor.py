# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import argparse
import copy
import io
import json
import logging
import matplotlib.pyplot as plt
import shutil

from json2html import *
from pathlib import Path
from sankeyflow import Sankey

from pulse.cdm.utils.file_utils import get_root_dir
from pulse.cdm.utils.markdown import process_file as process_markdown
from pulse.cdm.utils.markdown import table as markdown_table
from pulse.study.in_the_moment.triage_dataset import Intervention, TriageColor, create_report
from pulse.study.in_the_moment.triage_study_pipeline import Dataset

from casualty_generation import (calculate_population_error, calculate_injury_error,
                                 plot_population_error, plot_injury_error, to_specification_lists, plot_population)

from army_dataset import population_distributions as army_population_distributions, injury_list_to_dict
from army_dataset import injury_distributions as army_injury_distributions


_log = logging.getLogger('log')


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
        _log.info(f"Writing documentation to {filename}")
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
                        _log.fatal("Casualty has not vitals and did not die???")
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
            _log.info(f"Writing documentation to {filename}")
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
        _log.error(f"Unable to find markdown file: {src}")


def create_align_dataset(study_run: dict, scenario_id: str) -> list:
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

    def triage_case(_triage: dict, _time: float):
        prompt = ("".join(_triage["injury_description"]) + "\n" +
                  "\n".join(_triage["vitals_description"]))
        tags = _triage["tags"]
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
                    "scenario_id": scenario_id,
                    "full_state": {
                        "unstructured": prompt,
                        "meta_info": {"scene_id": f"Casualty_{pid}_at_{_time}min"},
                        "scenario_complete": False,
                        "characters": [{
                            "id": "Patient A",
                            "name": "Patient A",
                            "unstructured": prompt,
                            "demographics": {
                              "sex": "Unknown",
                              "race": "Unknown"
                            },
                            "rapport": "neutral",
                            "unseen": False,
                            "vitals": _triage["vitals"]
                        }]
                    },
                    "state": prompt,
                    "choices": choices
                    },
                "label": labels,
                "reasoning": reasonings}
        return case

    cases = []
    for pid, run in study_run.items():
        if "visits" in run:
            for time, visit in run["visits"].items():
                cases.append(triage_case(visit["triage"], time))
        if "final" in run:
            cases.append(triage_case(run["final"]["triage"], run["final"]["time"]))
        if "death" in run:
            cases.append(triage_case(run["death"]["triage"], run["death"]["time"]))

    return cases


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
    for i, reasons in enumerate(scenario["reasoning"]):
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


def create_align_markdown(set_name: str, set_type: str, align: list, output_dir):
    anchor = f"itm_align_{set_name}_{set_type}"
    filename = output_dir / f"{anchor}.md"
    _log.info(f"Writing documentation to {filename}")

    with open(filename, 'w') as file:
        file.write(f"\\anchor {anchor}\n\n")
        file.write(f"This dataset is derived from the Example Pulse dataset.\n")
        file.write(f"Each section below is independent and represents a single injured casualty and how we tag it.\n\n")
        file.write("<a href=\"./files/itm/army/itm_align_icl.json\">Download the Align dataset</a>\n\n")

        for scenario in align:
            file.write(f"#####{scenario['input']['full_state']['meta_info']['scene_id']}\n\n")
            file.write(_create_align_table(scenario))
            file.write(f"\n\n")


def plot_kaplan_meier(study_run: dict, output_dir: str):
    ledger = {"total": 0, "time_counts": {10: 0, 20: 0, 30: 0, 40: 0, 50: 0, 60: 0, 70: 0, 75: 0}}
    max_time = 100  # set death to this to indicate casualty survives throughout our analysis time

    protocols = ["none", "start", "salt", "bcd_sieve"]
    survivability_counts = {}

    def _kaplan_meier_plot(key: str):
        styles = [("black", '-'),
                  ("red", '--'),
                  ("blue", '-.'),
                  ("green", ':')]
        times = list(ledger["time_counts"].keys())
        total_casualties = survivability_counts["none"][key]["total"]
        for s, p in enumerate(protocols):
            values = []
            for count in survivability_counts[p][key]["time_counts"].values():
                values.append(count / total_casualties * 100)
            plt.plot(times, values, label=p, color=styles[s][0], linestyle=styles[s][1], linewidth=4)

        # Add plot enhancements
        plt_file = f"{output_dir}_{key}_survivability.png"
        _log.info(f"Writing plot {plt_file}")
        plt.title(f"{key} survivability", fontsize=24)
        plt.xlabel("Time (min)", fontsize=14)
        plt.ylabel("Survival Rate (%)", fontsize=14)
        plt.ylim(top=101.0, bottom=0.0)
        plt.xticks(fontsize=12)  # For x-axis tick labels
        plt.yticks(fontsize=12)  # For y-axis tick labels
        plt.legend(fontsize=12)
        # dpi of 72 is good poster size
        plt.savefig(plt_file, dpi=72)
        plt.clf()  # Clears the entire figure
        plt.close()

    # Counting structure
    plots = set()
    plots.add("overall")
    for protocol in protocols:
        survivability_counts[protocol] = {"overall": copy.deepcopy(ledger)}
        for loc in army_injury_distributions.keys():
            plots.add(loc)
            survivability_counts[protocol][loc] = copy.deepcopy(ledger)
            for typ in army_injury_distributions[loc]["types"].keys():
                plots.add(f"{loc}-{typ}")
                survivability_counts[protocol][f"{loc}-{typ}"] = copy.deepcopy(ledger)
        for i in range(0, 6):
            plots.add(f"ais-{i+1}.0")
            survivability_counts[protocol][f"ais-{i+1}.0"] = copy.deepcopy(ledger)

    for pid, run in study_run.items():
        # Find the chart axes this casualty applies to:
        axes = set()
        axes.add("overall")  # Always adding to overall count
        for i in run["specification"]["injuries"]:
            axes.add(i["location"])
            axes.add(f"{i['location']}-{i['type']}")
            axes.add(f"ais-{i['severity']}")
        for axis in axes:
            for protocol in protocols:
                survivability_counts[protocol][axis]["total"] += 1

        if "final" in run:
            no_intervention_death = max_time
            if "ais-6.0" in axes:
                _log.info("here")
        elif "death" in run:
            no_intervention_death = run["death"]["time"]
        else:
            _log.info(f"{pid} has no final or death?")
            exit(1)

        intervention = None
        intervention_death = no_intervention_death
        if "15.0" in run["visits"]:
            visit = run["visits"]["15.0"]
            if "intervention" in visit:
                intervention_death = max_time
                if "death" in visit["intervention"]:
                    _log.info("intervention death")
                    intervention_death = visit["intervention"]["death"]["time"]
                interventions = visit["triage"]["vitals"]["interventions"]
                if len(interventions) > 1:
                    _log.info(f"{pid} has more than 1 intervention")
                    exit(1)
                elif len(interventions) == 1:
                    intervention = interventions[0]

        for time in ledger["time_counts"].keys():
            if no_intervention_death >= time:
                for axis in axes:
                    survivability_counts["none"][axis]["time_counts"][time] += 1
            if not intervention:
                if no_intervention_death >= time:
                    for axis in axes:
                        survivability_counts["start"][axis]["time_counts"][time] += 1
                        survivability_counts["salt"][axis]["time_counts"][time] += 1
                        survivability_counts["bcd_sieve"][axis]["time_counts"][time] += 1
            else:
                if intervention == Intervention.NeedleDecompress:
                    if intervention_death >= time:
                        for axis in axes:
                            survivability_counts["salt"][axis]["time_counts"][time] += 1
                    if no_intervention_death >= time:
                        for axis in axes:
                            survivability_counts["start"][axis]["time_counts"][time] += 1
                            survivability_counts["bcd_sieve"][axis]["time_counts"][time] += 1

                elif intervention == Intervention.RepositionAirway:
                    if intervention_death >= time:
                        for axis in axes:
                            survivability_counts["start"][axis]["time_counts"][time] += 1
                            survivability_counts["salt"][axis]["time_counts"][time] += 1
                            survivability_counts["bcd_sieve"][axis]["time_counts"][time] += 1

                elif intervention == Intervention.Tourniquet:
                    if intervention_death >= time:
                        for axis in axes:
                            survivability_counts["salt"][axis]["time_counts"][time] += 1
                            survivability_counts["bcd_sieve"][axis]["time_counts"][time] += 1
                    if no_intervention_death >= time:
                        for axis in axes:
                            survivability_counts["start"][axis]["time_counts"][time] += 1

                elif intervention == Intervention.WoundPack:
                    if intervention_death >= time:
                        for axis in axes:
                            survivability_counts["salt"][axis]["time_counts"][time] += 1
                            survivability_counts["bcd_sieve"][axis]["time_counts"][time] += 1
                    if no_intervention_death >= time:
                        for axis in axes:
                            survivability_counts["start"][axis]["time_counts"][time] += 1

    for plot in plots:
        _kaplan_meier_plot(plot)


def count_tags(study_run: dict):
    tag_counts = {TriageColor.Green: 0, TriageColor.Yellow: 0, TriageColor.Red: 0, TriageColor.Black: 0}
    protocol_counts = {"start": copy.deepcopy(tag_counts),
                       "salt": copy.deepcopy(tag_counts),
                       "bcd_sieve": copy.deepcopy(tag_counts)}
    ais_counts = {1.0: copy.deepcopy(protocol_counts),
                  2.0: copy.deepcopy(protocol_counts),
                  3.0: copy.deepcopy(protocol_counts),
                  4.0: copy.deepcopy(protocol_counts),
                  5.0: copy.deepcopy(protocol_counts),
                  6.0: copy.deepcopy(protocol_counts)}
    counts = {"locations": {}, "tags": copy.deepcopy(protocol_counts), "ais": copy.deepcopy(ais_counts)}
    for loc, injury in army_injury_distributions.items():
        counts["locations"][loc] = {"injuries": {}, "tags": copy.deepcopy(protocol_counts), "ais": copy.deepcopy(ais_counts)}
        for typ in injury["types"]:
            counts["locations"][loc]["injuries"][typ] = {"tags": copy.deepcopy(protocol_counts), "ais": copy.deepcopy(ais_counts)}

    time_of_interest = "15.0"
    for pid, run in study_run.items():
        injuries = injury_list_to_dict(run["specification"]["injuries"])
        visits = run["visits"]
        if time_of_interest not in visits:
            # They died before we got to visit, black tag
            for protocol in protocol_counts.keys():
                color = TriageColor.Black
                counts["tags"][protocol][color] += 1
                ais_set = set()
                for loc, types in injuries.items():
                    counts["locations"][loc]["tags"][protocol][color] += 1
                    for typ, items in types.items():
                        counts["locations"][loc]["injuries"][typ]["tags"][protocol][color] += 1
                        for ais in items["severities"]:
                            counts["locations"][loc]["ais"][ais][protocol][color] += 1
                            counts["locations"][loc]["injuries"][typ]["ais"][ais][protocol][color] += 1
                            ais_set.add(ais)
                for ais in ais_set:
                    counts["ais"][ais][protocol][color] += 1
        else:
            tags = visits[time_of_interest]["triage"]["tags"]
            for protocol in protocol_counts.keys():
                color = tags[protocol]
                counts["tags"][protocol][color] += 1
                ais_set = set()
                for loc, types in injuries.items():
                    counts["locations"][loc]["tags"][protocol][color] += 1
                    for typ, items in types.items():
                        counts["locations"][loc]["injuries"][typ]["tags"][protocol][color] += 1
                        for ais in items["severities"]:
                            counts["locations"][loc]["ais"][ais][protocol][color] += 1
                            counts["locations"][loc]["injuries"][typ]["ais"][ais][protocol][color] += 1
                            ais_set.add(ais)
                for ais in ais_set:
                    counts["ais"][ais][protocol][color] += 1
    return counts


def plot_tag_count_tables(tag_counts: dict, results_stem: str):

    def _tag_table(counts: dict):
        table = (f"<table border=\"1\" align=\"center\">"
                 f"<tr>"
                 f"<td align=\"center\">Start</td>"
                 f"<td align=\"center\" style=\"background-color: #bfedb9;\">{counts['start']['Green']}</td>"
                 f"<td align=\"center\" style=\"background-color: #ebedbb;\">{counts['start']['Yellow']}</td>"
                 f"<td align=\"center\" style=\"background-color: #edb9b9;\">{counts['start']['Red']}</td>"
                 f"<td align=\"center\" style=\"background-color: #070808;color: white\">{counts['start']['Black']}</td>"
                 f"</tr>"
                 f"<tr>"
                 f"<td align=\"center\">Salt</td>"
                 f"<td align=\"center\" style=\"background-color: #bfedb9;\">{counts['salt']['Green']}</td>"
                 f"<td align=\"center\" style=\"background-color: #ebedbb;\">{counts['salt']['Yellow']}</td>"
                 f"<td align=\"center\" style=\"background-color: #edb9b9;\">{counts['salt']['Red']}</td>"
                 f"<td align=\"center\" style=\"background-color: #070808;color: white\">{counts['salt']['Black']}</td>"
                 f"</tr>"
                 f"<tr>"
                 f"<td align=\"center\">BCD</td>"
                 f"<td align=\"center\" style=\"background-color: #bfedb9;\">{counts['bcd_sieve']['Green']}</td>"
                 f"<td align=\"center\" style=\"background-color: #ebedbb;\">{counts['bcd_sieve']['Yellow']}</td>"
                 f"<td align=\"center\" style=\"background-color: #edb9b9;\">{counts['bcd_sieve']['Red']}</td>"
                 f"<td align=\"center\" style=\"background-color: #070808;color: white\">{counts['bcd_sieve']['Black']}</td>"
                 f"</tr>"
                 f"</table>")
        return table

    # Error Table
    def _count_row(loc: str = None, typ: str = None, tags: dict = None, ais_tags: dict = None):
        if loc is None and typ is None:

            injury_location = "ALL"
            injury_type = "ALL"
        else:
            injury_location = "" if loc is None else loc
            injury_type = "" if typ is None else typ
        return (injury_location, injury_type,
                _tag_table(tags),
                _tag_table(ais_tags[1.0]),
                _tag_table(ais_tags[2.0]),
                _tag_table(ais_tags[3.0]),
                _tag_table(ais_tags[4.0]),
                _tag_table(ais_tags[5.0]),
                _tag_table(ais_tags[6.0]))
    full_data = []
    full_headings = ["Injury Location", "Injury Type", "Count",
                     "AIS 1", "AIS 2", "AIS 3", "AIS 4", "AIS 5", "AIS 6"]
    full_fields = [0, 1, 2, 3, 4, 5, 6, 7, 8]  # All headings
    full_data.append(_count_row(None, None, tag_counts["tags"], tag_counts["ais"]))
    for location, loc_items in tag_counts["locations"].items():
        full_data.append(_count_row(location, None, loc_items["tags"], loc_items["ais"]))
        for injury, inj_items in loc_items["injuries"].items():
            full_data.append(_count_row(None, injury, inj_items["tags"], inj_items["ais"]))
    create_report(f"{results_stem}_tag_counts", full_data, full_fields, full_headings)

    for ais in [1.0, 2.0, 3.0, 4.0, 5.0, 6.0]:
        ais_data = [_tag_table(tag_counts["ais"][ais])]
        create_report(f"{results_stem}_ais_{ais}_tag_counts", ais_data, [0], [f"AIS {ais} Tags"])
    create_report(f"{results_stem}_overall_tag_counts", [_tag_table(tag_counts["tags"])], [0], [f"Overall Tags"])


def plot_sankey_location_tag_count(tag_counts: dict, results_stem: str):

    for protocol in tag_counts["tags"].keys():
        flow_values = []
        for color, count in tag_counts["tags"][protocol].items():
            flow_values.append(count)
        for color in ["Green", "Yellow", "Red", "Black"]:
            for loc, typ_items in tag_counts["locations"].items():
                flow_values.append(typ_items["tags"][protocol][color])

        curvature = 0.3
        flows = [
            ("Protocol", "Green", flow_values[0], {'color': 'green', 'curvature': curvature}),
            ("Protocol", "Yellow", flow_values[1], {'color': 'yellow', 'curvature': curvature}),
            ("Protocol", "Red", flow_values[2], {'color': 'red', 'curvature': curvature}),
            ("Protocol", "Black", flow_values[3], {'color': 'grey', 'curvature': curvature}),
            ("Green", "gHead/Neck", flow_values[4], {'color': 'green', 'curvature': curvature}),
            ("Green", "gThorax", flow_values[5], {'color': 'green', 'curvature': curvature}),
            ("Green", "gAbdomen", flow_values[6], {'color': 'green', 'curvature': curvature}),
            ("Green", "gExtremity", flow_values[7], {'color': 'green', 'curvature': curvature}),
            ("Yellow", "yHead/Neck", flow_values[8], {'color': 'yellow', 'curvature': curvature}),
            ("Yellow", "yThorax", flow_values[9], {'color': 'yellow', 'curvature': curvature}),
            ("Yellow", "yAbdomen", flow_values[10], {'color': 'yellow', 'curvature': curvature}),
            ("Yellow", "yExtremity", flow_values[11], {'color': 'yellow', 'curvature': curvature}),
            ("Red", "rHead/Neck", flow_values[12], {'color': 'red', 'curvature': curvature}),
            ("Red", "rThorax", flow_values[13], {'color': 'red', 'curvature': curvature}),
            ("Red", "rAbdomen", flow_values[14], {'color': 'red', 'curvature': curvature}),
            ("Red", "rExtremity", flow_values[15], {'color': 'red', 'curvature': curvature}),
            ("Black", "bHead/Neck", flow_values[16], {'color': 'grey', 'curvature': curvature}),
            ("Black", "bThorax", flow_values[17], {'color': 'grey', 'curvature': curvature}),
            ("Black", "bAbdomen", flow_values[18], {'color': 'grey', 'curvature': curvature}),
            ("Black", "bExtremity", flow_values[19], {'color': 'grey', 'curvature': curvature}),
        ]
        _plot_sankey_tag_flows(flows, protocol, f"{results_stem}_{protocol}_location_sankey.png")


def _ais_flows(flow_values):
    curvature = 0.3
    return [("Protocol", "Green", flow_values[0], {'color': 'green', 'curvature': curvature}),
            ("Protocol", "Yellow", flow_values[1], {'color': 'yellow', 'curvature': curvature}),
            ("Protocol", "Red", flow_values[2], {'color': 'red', 'curvature': curvature}),
            ("Protocol", "Black", flow_values[3], {'color': 'grey', 'curvature': curvature}),
            ("Green", "gAIS-1", flow_values[4], {'color': 'green', 'curvature': curvature}),
            ("Green", "gAIS-2", flow_values[5], {'color': 'green', 'curvature': curvature}),
            ("Green", "gAIS-3", flow_values[6], {'color': 'green', 'curvature': curvature}),
            ("Green", "gAIS-4", flow_values[7], {'color': 'green', 'curvature': curvature}),
            ("Green", "gAIS-5", flow_values[8], {'color': 'green', 'curvature': curvature}),
            ("Green", "gAIS-6", flow_values[9], {'color': 'green', 'curvature': curvature}),
            ("Yellow", "yAIS-1", flow_values[10], {'color': 'yellow', 'curvature': curvature}),
            ("Yellow", "yAIS-2", flow_values[11], {'color': 'yellow', 'curvature': curvature}),
            ("Yellow", "yAIS-3", flow_values[12], {'color': 'yellow', 'curvature': curvature}),
            ("Yellow", "yAIS-4", flow_values[13], {'color': 'yellow', 'curvature': curvature}),
            ("Yellow", "yAIS-5", flow_values[14], {'color': 'yellow', 'curvature': curvature}),
            ("Yellow", "yAIS-6", flow_values[15], {'color': 'yellow', 'curvature': curvature}),
            ("Red", "rAIS-1", flow_values[16], {'color': 'red', 'curvature': curvature}),
            ("Red", "rAIS-2", flow_values[17], {'color': 'red', 'curvature': curvature}),
            ("Red", "rAIS-3", flow_values[18], {'color': 'red', 'curvature': curvature}),
            ("Red", "rAIS-4", flow_values[19], {'color': 'red', 'curvature': curvature}),
            ("Red", "rAIS-5", flow_values[20], {'color': 'red', 'curvature': curvature}),
            ("Red", "rAIS-6", flow_values[21], {'color': 'red', 'curvature': curvature}),
            ("Black", "bAIS-1", flow_values[22], {'color': 'grey', 'curvature': curvature}),
            ("Black", "bAIS-2", flow_values[23], {'color': 'grey', 'curvature': curvature}),
            ("Black", "bAIS-3", flow_values[24], {'color': 'grey', 'curvature': curvature}),
            ("Black", "bAIS-4", flow_values[25], {'color': 'grey', 'curvature': curvature}),
            ("Black", "bAIS-5", flow_values[26], {'color': 'grey', 'curvature': curvature}),
            ("Black", "bAIS-6", flow_values[27], {'color': 'grey', 'curvature': curvature}),
            ]


def plot_sankey_overall_ais_tag_count(tag_counts: dict, results_stem: str):

    for protocol in tag_counts["tags"].keys():
        flow_values = []
        for color, count in tag_counts["tags"][protocol].items():
            flow_values.append(count)
        for color in ["Green", "Yellow", "Red", "Black"]:
            for ais, ais_items in tag_counts["ais"].items():
                flow_values.append(ais_items[protocol][color])
        flows = _ais_flows(flow_values)
        _plot_sankey_tag_flows(flows, protocol, f"{results_stem}_{protocol}_overall_ais_sankey.png")


def plot_sankey_hemorrhage_ais_tag_count(tag_counts: dict, results_stem: str):

    locations = tag_counts["locations"]
    for protocol in tag_counts["tags"].keys():
        flow_values = []
        for color in tag_counts["tags"][protocol].keys():
            count = (locations["thorax"]["injuries"]["hemorrhage"]["tags"][protocol][color] +
                     locations["abdomen"]["injuries"]["hemorrhage"]["tags"][protocol][color] +
                     locations["abdomen"]["injuries"]["laceration_contusion"]["tags"][protocol][color] +
                     locations["extremity"]["injuries"]["hemorrhage"]["tags"][protocol][color])
            flow_values.append(count)
        for color in ["Green", "Yellow", "Red", "Black"]:
            for ais, ais_items in tag_counts["ais"].items():
                count = (locations["thorax"]["injuries"]["hemorrhage"]["ais"][ais][protocol][color] +
                         locations["abdomen"]["injuries"]["hemorrhage"]["ais"][ais][protocol][color] +
                         locations["abdomen"]["injuries"]["laceration_contusion"]["ais"][ais][protocol][color] +
                         locations["extremity"]["injuries"]["hemorrhage"]["ais"][ais][protocol][color])
                flow_values.append(count)
        flows = _ais_flows(flow_values)
        _plot_sankey_tag_flows(flows, protocol, f"{results_stem}_{protocol}_hemorrhage_ais_sankey.png")


def _plot_sankey_tag_flows(flows: list, protocol: str, filename: str):
    # Remove flows with 0 tags
    flows = [flow for flow in flows if flow[2] > 0]

    plt.figure(figsize=(10, 8), dpi=96)
    s = Sankey(flows=flows)
    # Adjust tag node colors
    for node in s.nodes[1]:
        color = node.label[0]
        if color == 'G':
            node.color = "darkgreen"
        elif color == 'Y':
            node.color = "gold"
        elif color == 'R':
            node.color = "darkred"
        else:
            node.color = 'dimgrey'
    # Adjust location nodes
    s.nodes[0][0].label = protocol.upper()
    for node in s.nodes[2]:
        color = node.label[0]
        if color == 'g':
            node.color = "darkgreen"
        elif color == 'y':
            node.color = "gold"
        elif color == 'r':
            node.color = "darkred"
        else:
            node.color = 'dimgrey'
        node.label = node.name[1:]
        node.label_pos = "right"
        node.label_format = "{label} {value:,.0f}"
        node.label_opts = {"fontsize": 7}

    s.draw()
    _log.info(f"Writing {filename}")
    plt.savefig(filename)
    plt.clf()  # Clears the entire figure
    plt.close()


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
        "-ev1k", "--eval_1k_file",
        type=Path,
        default=Path("./test_results/itm/triage_study/1000_casualties.json"),
        help="Triage study evaluation file"
    )
    parser.add_argument(
        "-ev10k", "--eval_10k_file",
        type=Path,
        default=Path("./test_results/itm/triage_study/10000_casualties.json"),
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
        "-plt", "--create_plots",
        action='store_true',
        help="Create plots and tables for provided eval file only"
    )

    opts = parser.parse_args()
    docs = Path(get_root_dir()) / "src/python/pulse/study/in_the_moment/docs"

    # TODO Add dataset arg when we add more datasets
    dataset = Dataset.Army

    output_dir = opts.output_dir / dataset.value
    output_dir.mkdir(parents=True, exist_ok=True)

    downloads_dir = Path(f"./docs/html/files/itm/{dataset.value}")
    downloads_dir.mkdir(parents=True, exist_ok=True)
    if opts.example_file.exists():
        shutil.copyfile(opts.example_file, downloads_dir / "itm_example_dataset.json")
    if opts.eval_1k_file.exists():
        shutil.copyfile(opts.eval_1k_file, downloads_dir / "itm_eval1k_dataset.json")
    if opts.eval_10k_file.exists():
        shutil.copyfile(opts.eval_10k_file, downloads_dir / "itm_eval10k_dataset.json")

    if docs.exists():
        shutil.copyfile(docs / "ALIGN_Triage.pdf", downloads_dir / "ALIGN_Triage.pdf")
        shutil.copyfile(docs / "ALIGN_Triage.png", downloads_dir / "ALIGN_Triage.png")
        shutil.copyfile(docs / "Triage_Evaluation.pdf", downloads_dir / "Triage_Evaluation.pdf")
        shutil.copyfile(docs / "Triage_Evaluation.png", downloads_dir / "Triage_Evaluation.png")

    output_img_dir = None
    if opts.create_plots:
        output_img_dir = Path(f"./docs/html/Images/itm/{dataset.value}")
        output_img_dir.mkdir(parents=True, exist_ok=True)
        plot_population(army_population_distributions["age"], output_img_dir)

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
            file.write(f"|                                   |  Male     | Female     |\n")
            file.write(f"|-----------------------------------|:---------:|:----------:|\n")
            file.write(f"| Percent @cite Demographics2022    | {male_p}% |{female_p}% |\n")
            file.write(f"| Height (cm) @cite Martin2016      | {male_ht} |{female_ht} |\n")
            file.write(f"| BMI @cite Martin2016              | {male_bmi}|{female_bmi}|\n")

        # Generate injury table
        def _percent_severity(d: dict):
            percent = d["percent"]
            severity = d["severity"]
            if "mean" in severity:
                return percent, f"mean: {severity['mean']:.2f}<br> stdev: {severity['std']:.2f}"
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
            file.write(f"| Location    | Type                        | Proportion                    | AIS                              |\n")
            file.write(f"| ----------- |-----------------------------|:-----------------------------:|:--------------------------------:|\n")
            file.write(f"| Head / Neck |                             |{hn_p}%    @cite Belmont2010   |                                  |\n")
            file.write(f"|             | Airway Obstruction          |{hn_ao_p}% @cite Blackburn2018 |{hn_ao_s}<br>**                   |\n")
            file.write(f"|             | Superficial                 |{hn_s_p}%                      |{hn_s_s}**                        |\n")
            file.write(f"|             | Traumatic Brain Injury      |{hn_tbi_p}% @cite 2025NCPTSD   |{hn_tbi_s}<br>@cite Blackburn2018 |\n")
            file.write(f"| Thorax      |                             |{t_p}% @cite Ivey2012          |                                  |\n")
            file.write(f"|             | Fracture                    |{t_f_p}%  @cite Ivey2012       |{t_f_s}<br>@cite Ivey2012         |\n")
            file.write(f"|             | Hemorrhage                  |{t_hg_p}% @cite Ivey2012       |{t_hg_s}<br>@cite Ivey2012        |\n")
            file.write(f"|             | Hemothorax                  |{t_hx_p}% @cite Ivey2012       |{t_hx_s}<br>@cite Ivey2012        |\n")
            file.write(f"|             | Pneumothorax                |{t_px_p}% @cite Ivey2012       |{t_px_s}<br>@cite Ivey2012        |\n")
            file.write(f"|             | Pulmonary Contusion         |{t_pc_p}% @cite Ivey2012       |{t_pc_s}<br>@cite Ivey2012        |\n")
            file.write(f"|             | Spinal                      |{t_s_p}%  @cite Ivey2012       |{t_s_s}<br>@cite Ivey2012         |\n")
            file.write(f"| Abdomen     |                             |{a_p}% @cite Belmont2010       |                                  |\n")
            file.write(f"|             | Hemorrhage                  |{a_hg_p}%*                     |{a_hg_s}<br>*                     |\n")
            file.write(f"|             | Laceration / Contusion      |{a_lc_p}%                      |{a_lc_s}<br>*                     |\n")
            file.write(f"| Extremity   |                             |{e_p}%   @cite Belmont2010     |                                  |\n")
            file.write(f"|             | Burn / Nerve                |{e_bn_p}%  @cite Perez2022     |{e_bn_s}@cite Perez2022           |\n")
            file.write(f"|             | Contusion / Sprain / Strain |{e_css_p}% @cite Perez2022     |{e_css_s}@cite Perez2022          |\n")
            file.write(f"|             | Fracture / Dislocation      |{e_fd_p}%  @cite Perez2022     |{e_fd_s}@cite Perez2022           |\n")
            file.write(f"|             | Hemorrhage                  |{e_hg_p}%  @cite Perez2022     |{e_hg_s}@cite Perez2022           |\n")
            file.write(f"<p style=\"font-size:8pt;\">"
                       f"*Assumed to match statistics for Thorax Hemorrhage<br>"
                       f"** Derived from @cite Blackburn2018</p><br>")

        # Update our landing page with all these runs
        src = Path(get_root_dir()) / "src/python/pulse/study/in_the_moment/docs/itm_triage_datasets.md"
        if src.exists():
            process_markdown(src, output_md_dir, output_md_dir)
        else:
            _log.error(f"Unable to find markdown file: {src}")

    if opts.example_file.exists():
        with open(opts.example_file, 'r') as file:
            study = json.load(file)

        if opts.markdown:
            create_markdown(dataset.value, "example", study, output_md_dir)

        if opts.to_align_input:
            align = create_align_dataset(study, "example")
            filename = output_dir/f"align_{opts.example_file.stem}.json"
            _log.info(f"Writing {filename}")
            with open(filename, 'w') as file:
                json.dump(align, file, indent=2)
            dl_file = downloads_dir / "itm_example_align.json"
            _log.info(f"Copying to {dl_file}")
            shutil.copyfile(filename, dl_file)

    def _process_eval_file(eval_file: Path, set_name: str):
        if eval_file.exists():
            with open(eval_file, 'r') as ef:
                eval_study = json.load(ef)
            _log.info(f"There are {len(eval_study)} casualties in {eval_file}")

            if opts.create_plots:
                dst_dir = output_img_dir / f"{set_name}"
                dst_dir.mkdir(exist_ok=True, parents=True)
                results_stem = str(dst_dir) + "/eval_casualties"

                # Write out the error images for this generated dataset
                spec = to_specification_lists(eval_study)
                population_error = calculate_population_error(spec, army_population_distributions)
                plot_population_error(population_error, results_stem)

                injury_error = calculate_injury_error(spec["injuries"], army_injury_distributions)
                plot_injury_error(injury_error, results_stem)

                # Result plots/tables
                tag_counts = count_tags(eval_study)
                plot_kaplan_meier(eval_study, results_stem)
                plot_tag_count_tables(tag_counts, results_stem)
                plot_sankey_location_tag_count(tag_counts, results_stem)
                plot_sankey_overall_ais_tag_count(tag_counts, results_stem)
                plot_sankey_hemorrhage_ais_tag_count(tag_counts, results_stem)

            if opts.markdown:
                create_markdown(dataset.value, f"eval{set_name}", eval_study, output_md_dir)

            if opts.to_align_input:
                eval_align = create_align_dataset(eval_study, "")
                eval_out = output_dir/f"align_eval{set_name}.json"
                _log.info(f"Writing {eval_out}")
                with open(eval_out, 'w') as eval_file:
                    json.dump(eval_align, eval_file, indent=2)
                eval_dl_file = downloads_dir / f"itm_eval{set_name}_align.json"
                _log.info(f"Copying to {eval_dl_file}")
                shutil.copyfile(eval_out, eval_dl_file)

    if opts.eval_1k_file.exists():
        _process_eval_file(opts.eval_1k_file, "1k")
    if opts.eval_10k_file.exists():
        _process_eval_file(opts.eval_10k_file, "10k")


if __name__ == "__main__":
    main()
