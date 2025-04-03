# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import dataframe_image as dfi
import logging
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import random as rand
import sys

from pathlib import Path
from pulse.cdm.utils.markdown import table

_log = logging.getLogger("pulse")

army_population_distributions = {
    "heart_rate": {"mean": 72, "std": 11},
    "sex": {
              "male": {"height": {"mean": 177, "std": 7.1}, "bmi": {"mean": 26.4, "std": 3.4}},
              "female": {"percent": 15.4, "height": {"mean": 163.5, "std": 7.7}, "bmi": {"mean": 24.7, "std": 2.8}}
            },
    "age": {"bins": [18, 25, 30, 35, 40, 55],
            "percents": [41.6, 22.4, 15.3, 11.3, 9.4],
            "counts": [191975, 103628, 70783, 52055, 43215]}
}

army_injury_distributions = {  # Location -> Type -> Severity mean/std or explicit value/percent
    "head_and_neck": {"percent": 36.2, "mean": 2.69, "types": {
        "tbi": {"percent": 22, "severity": {"mean": 3.5, "std": 0.25}},
        "airway_obstruction": {"percent": 18, "severity": {"mean": 4.0, "std": 0.25}},
        "superficial": {"percent": 60, "severity": {"values": [1.0], "percents": [100]}}
    }},
    "thorax": {"percent": 8.6, "mean": 2.85, "types": {
        "pneumothorax": {"percent": 51.8, "severity": {"mean": 2.85, "std": 0.25}},
        "pulmonary_contusion": {"percent": 50.2, "severity": {"mean": 2.85, "std": 0.25}},
        "fracture": {"percent": 51.2, "severity": {"mean": 2.85, "std": 0.25}},
        "hemothorax": {"percent": 30, "severity": {"mean": 2.85, "std": 0.25}},
        "hemorrhage": {"percent": 34.6, "severity": {"mean": 2.85, "std": 0.25}},
        "spinal": {"percent": 14.6, "severity": {"mean": 2.85, "std": 0.25}}
    }},
    "abdomen": {"percent": 6.9, "mean": 2.85, "types": {
        "hemorrhage": {"percent": 34.6, "severity": {"mean": 2.85, "std": 0.25}},
        "laceration_contusion": {"percent": 65.4, "severity": {"mean": 2.85, "std": 0.25}}
    }},
    "extremity": {"percent": 49.4, "mean": 2.05, "types": {
        "hemorrhage": {"percent": 52, "severity": {"values": [1.0, 2.5, 3.5, 4.5], "percents": [56, 23, 17, 7]}},
        "fracture_dislocation": {"percent": 22, "severity": {"values": [1.0, 2.5, 3.5, 4.5], "percents": [56, 23, 17, 7]}},
        "contusion_sprain_strain": {"percent": 20, "severity": {"values": [1.0, 2.5, 3.5, 4.5], "percents": [56, 23, 17, 7]}},
        "burn_nerve": {"percent": 5, "severity": {"values": [1.0, 2.5, 3.5, 4.5], "percents": [56, 23, 17, 7]}},
    }}
}


def main():

    population_size = 4000
    #army_patients = synthetic_data_generation(population_size, army_population_distributions)
    #army_population_error = calculate_synthetic_population_error(army_patients, army_population_distributions)
    #plot_population_error(army_population_error, f"./results/army_population_of_{population_size}")

    army_patient_injuries = generate_synthetic_injuries(population_size, army_injury_distributions)
    army_injury_error = calculate_synthetic_injury_error(army_patient_injuries, army_injury_distributions)
    plot_injury_error(army_injury_error, f"./results/army_injuries_of_population_of_{population_size}")


def synthetic_data_generation(size: int, distributions: dict) -> dict:
    _log.info(f"Generate data with {size} samples")

    population_data = {}
    
    # Sex
    male_distributions = distributions["sex"]["male"]
    female_distributions = distributions["sex"]["female"]
    if "percent" in female_distributions:
        sex_percent = [100 - female_distributions["percent"], female_distributions["percent"]]
    elif "percent" in male_distributions:
        sex_percent = [male_distributions["percent"], 100-male_distributions["percent"]]
    else:
        _log.error("Must provide the percent percentage of either male's or female's")
        return population_data
    sexes = rand.choices(["male", "female"], weights=sex_percent, k=size)
    num_females = sexes.count("female")
    num_males = sexes.count("male")

    # Height
    female_heights = np.random.normal(loc=female_distributions["height"]["mean"],
                                      scale=female_distributions["height"]["std"],
                                      size=num_females)
    male_heights = np.random.normal(loc=male_distributions["height"]["mean"],
                                    scale=male_distributions["height"]["std"],
                                    size=num_males)

    # BMI
    female_bmi = np.random.normal(loc=female_distributions["bmi"]["mean"],
                                  scale=female_distributions["bmi"]["std"],
                                  size=num_females)
    male_bmi = np.random.normal(loc=male_distributions["bmi"]["mean"],
                                scale=male_distributions["bmi"]["std"],
                                size=num_males)

    bmi = []
    heights = []
    female_idx = 0
    male_idx = 0
    for sex in sexes:
        if sex == "female":
            heights.append(female_heights[female_idx])
            bmi.append(female_bmi[female_idx])
            female_idx += 1
        else:
            heights.append(male_heights[male_idx])
            bmi.append(male_bmi[male_idx])
            male_idx += 1

    # Heart Rate
    heart_rates = np.random.normal(loc=distributions["heart_rate"]["mean"],
                                   scale=distributions["heart_rate"]["std"],
                                   size=size)

    # Age
    num_bins = len(distributions["age"]["bins"])
    num_percents = len(distributions["age"]["percents"])
    if num_bins != num_percents+1:
        _log.error("Age bins must be 1 more that the percents length")
        _log.error(f"Provided {len(distributions['age']['bins'])} bins")
        _log.error(f"Provided {len(distributions['age']['percents'])} percents")
        return population_data

    age_bins = []
    for i in range(num_percents):
        age_min = distributions["age"]["bins"][i]
        age_max = distributions["age"]["bins"][i+1]
        if i > 0:
            age_min += 1
        age_bins.append(f"{age_min}-{age_max}")

    ages = []
    age_groups = rand.choices(age_bins, weights=distributions["age"]["percents"], k=size)
    for age_group in age_groups:
        idx = age_bins.index(age_group)
        low = distributions["age"]["bins"][idx]
        high = distributions["age"]["bins"][idx+1]
        if idx > 0:
            low += 1
        ages.append(np.random.randint(low, high))

    population_data["sex"] = sexes
    population_data["age"] = ages
    population_data["height"] = heights
    population_data["bmi"] = bmi
    population_data["heart_rate"] = heart_rates

    return population_data


def calculate_synthetic_population_error(population: dict, distributions: dict) -> dict:
    error = {}

    # Sex
    female_count = {}
    male_count = {}
    if "percent" in distributions["sex"]["female"]:
        female_count["actual"] = distributions["sex"]["female"]["percent"]
        male_count["actual"] = 100 - female_count["actual"]
    elif "percent" in distributions["sex"]["male"]:
        male_count["actual"] = distributions["sex"]["male"]["percent"]
        female_count["actual"] = 100 - male_count["actual"]
    else:
        _log.error("Must provide the percent percentage of either male's or female's")
        return {}
    female_count["synthetic"] = 100 * population["sex"].count("female") / len(population["sex"])
    female_count["error"] = female_count["synthetic"] - female_count["actual"]
    male_count["synthetic"] = 100 * population["sex"].count("male") / len(population["sex"])
    male_count["error"] = male_count["synthetic"] - male_count["actual"]

    # Height and BMI
    female_heights = []
    male_heights = []
    female_bmis = []
    male_bmis = []
    for i, sex in enumerate(population["sex"]):
        if sex == "female":
            female_heights.append(population["height"][i])
            female_bmis.append(population["bmi"][i])
        else:
            male_heights.append(population["height"][i])
            male_bmis.append(population["bmi"][i])

    female_height = {"synthetic_mean": np.mean(female_heights),
                     "actual_mean": distributions["sex"]["female"]["height"]["mean"],
                     "synthetic_std": np.std(female_heights),
                     "actual_std": distributions["sex"]["female"]["height"]["std"]}
    female_height["mean_error"] = female_height["synthetic_mean"] - female_height["actual_mean"]
    female_height["std_error"] = female_height["synthetic_std"] - female_height["actual_std"]

    male_height = {"synthetic_mean": np.mean(male_heights),
                   "actual_mean": distributions["sex"]["male"]["height"]["mean"],
                   "synthetic_std": np.std(male_heights),
                   "actual_std": distributions["sex"]["male"]["height"]["std"]}
    male_height["mean_error"] = male_height["synthetic_mean"] - male_height["actual_mean"]
    male_height["std_error"] = male_height["synthetic_std"] - male_height["actual_std"]

    female_bmi = {"synthetic_mean": np.mean(female_bmis),
                  "actual_mean": distributions["sex"]["female"]["bmi"]["mean"],
                  "synthetic_std": np.std(female_bmis),
                  "actual_std": distributions["sex"]["female"]["bmi"]["std"]}
    female_bmi["mean_error"] = female_bmi["synthetic_mean"] - female_bmi["actual_mean"]
    female_bmi["std_error"] = female_bmi["synthetic_std"] - female_bmi["actual_std"]

    male_bmi = {"synthetic_mean": np.mean(male_bmis),
                "actual_mean": distributions["sex"]["male"]["bmi"]["mean"],
                "synthetic_std": np.std(male_bmis),
                "actual_std": distributions["sex"]["male"]["bmi"]["std"]}
    male_bmi["mean_error"] = male_bmi["synthetic_mean"] - male_bmi["actual_mean"]
    male_bmi["std_error"] = male_bmi["synthetic_std"] - male_bmi["actual_std"]

    error["sex"] = {"female": {"count": female_count, "height": female_height, "bmi": female_bmi},
                    "male": {"count": male_count, "height": male_height, "bmi": male_bmi}}

    # Heart Rate
    error["heart_rate"] = {"synthetic_mean": np.mean(population["heart_rate"]),
                           "actual_mean": distributions["heart_rate"]["mean"],
                           "synthetic_std": np.std(population["heart_rate"]),
                           "actual_std": distributions["heart_rate"]["std"]}
    error["heart_rate"]["mean_error"] = error["heart_rate"]["synthetic_mean"] - error["heart_rate"]["actual_mean"]
    error["heart_rate"]["std_error"] = error["heart_rate"]["synthetic_std"] - error["heart_rate"]["actual_std"]

    # Age
    age_bins = distributions["age"]["bins"]
    actual_age_counts = distributions["age"]["counts"]
    synthetic_age_counts, bins = np.histogram(population["age"], bins=age_bins)
    error["age"] = {"bins": age_bins,
                    "synthetic_counts": synthetic_age_counts / synthetic_age_counts.sum(),
                    "actual_counts": [x / sum(actual_age_counts) for x in actual_age_counts]}

    return error


def plot_population_error(population_error: dict, results_stem: str):
    out_dir = Path(results_stem).parent
    out_dir.mkdir(exist_ok=True)

    # Age
    age_bins = population_error["age"]["bins"]
    actual_age_counts = population_error["age"]["actual_counts"]
    synthetic_age_counts = population_error["age"]["synthetic_counts"]
    fig, axes = plt.subplots(1, 2, figsize=(10, 5))
    axes[0].hist(age_bins[:-1], age_bins, weights=synthetic_age_counts, color="skyblue", edgecolor="black")
    axes[0].set_ylim(0, 0.45)
    axes[0].set_title("Normalized Synthetic Age Data")
    axes[0].set_xlabel("Age")
    axes[0].set_ylabel("Normalized Frequency")
    axes[1].hist(age_bins[:-1], age_bins, weights=actual_age_counts, color="green", edgecolor="black")
    axes[1].set_ylim(0, 0.45)
    axes[1].set_title("Normalized Actual Age Data")
    axes[1].set_xlabel("Age")
    axes[1].set_ylabel("Normalized Frequency")
    plt.savefig(f"{results_stem}_age_histogram.jpg", format="jpeg")

    # Error Table
    def _error_row(name: str, error: dict):
        return (name,
                f"{error['synthetic_mean']:.1f}",
                f"{error['actual_mean']:.1f}",
                f"{error['mean_error']:.1f}",
                f"{error['synthetic_std']:.3f}",
                f"{error['actual_std']:.3f}",
                f"{error['std_error']:.3f}")
    data = []
    headings = ["Descriptor",
                "Synthetic Mean", "Actual Mean", "Mean Error",
                "Synthetic SD", "Actual SD", "SD Error"]
    fields = [0, 1, 2, 3, 4, 5, 6]  # All headings
    data.append(_error_row("Female Height", population_error["sex"]["female"]["height"]))
    data.append(_error_row("Female BMI", population_error["sex"]["female"]["bmi"]))
    data.append(_error_row("Male Height", population_error["sex"]["male"]["height"]))
    data.append(_error_row("Male BMI", population_error["sex"]["male"]["bmi"]))
    data.append(_error_row("Heart Rate", population_error["heart_rate"]))
    _create_report(f"{results_stem}_statistics", data, fields, headings)

    # Sex Count Table
    def _count_row(name: str, error: dict):
        return (name,
                f"{error['synthetic']:.1f}%",
                f"{error['actual']:.1f}%",
                f"{error['error']:.1f}")
    data = []
    headings = ["Sex",
                "Synthetic", "Actual", "Error"]
    fields = [0, 1, 2, 3]  # All headings
    data.append(_count_row("Female", population_error["sex"]["female"]["count"]))
    data.append(_count_row("Male", population_error["sex"]["male"]["count"]))
    _create_report(f"{results_stem}_sex", data, fields, headings)


class Injury:
    __slots__ = ["location", "type", "severity"]

    def __init__(self, location: str, type: str, severity: float):
        self.location = location
        self.type = type
        self.severity = severity


def generate_synthetic_injuries(population_size: int, distributions: dict) -> list:
    # Array or arrays
    # An array of injuries for each patient
    patient_injuries = []

    # TODO Assuming only 1 injury for each patient
    injury_locations = rand.choices(list(distributions.keys()),
                                    weights=[value["percent"] for value in distributions.values()],
                                    k=population_size)

    # Generate severities for each injury type
    ledger = {}
    for location, injury_distributions in distributions.items():
        injury_types = injury_distributions["types"]
        # Generate a single injury type based on each supported location
        ledger[location] = {"index": 0,
                            "injuries": rand.choices(list(injury_types.keys()),
                                                     weights=[injury_types[t]["percent"] for t in injury_types],
                                                     k=injury_locations.count(location)),
                            "injury_severities": {}}
        injuries = ledger[location]["injuries"]
        injury_severities = ledger[location]["injury_severities"]
        for injury, dist in injury_types.items():
            severity_dist = dist["severity"]
            if "mean" in severity_dist:
                injury_severities[injury] = {"index": 0,
                                             "severities": np.random.normal(loc=severity_dist["mean"],
                                                                            scale=severity_dist["std"],
                                                                            size=injuries.count(injury))}
            elif "values" in severity_dist:
                injury_severities[injury] = {"index": 0,
                                             "severities": rand.choices(severity_dist["values"],
                                                                        weights=severity_dist["percents"],
                                                                        k=injuries.count(injury))}

    # Map the types and severities back to the injury locations
    injury_types = []
    injury_severities = []
    for location in injury_locations:
        injury_ledger = ledger[location]
        injury = injury_ledger["injuries"][injury_ledger["index"]]
        injury_ledger["index"] += 1
        injury_types.append(injury)
        severity_ledger = injury_ledger["injury_severities"][injury]
        severity = severity_ledger["severities"][severity_ledger["index"]]
        severity_ledger["index"] += 1
        injury_severities.append(severity)

    # TODO still assuming 1 injury per patient
    for i in range(len(injury_locations)):
        patient_injuries.append([Injury(
            injury_locations[i],
            injury_types[i],
            injury_severities[i])])

    return patient_injuries


def calculate_synthetic_injury_error(patients_injuries: list, injury_distributions: dict) -> dict:
    error = {}

    # Count everything up, and gather all our severities
    num_injuries = 0
    for patient_injuries in patients_injuries:
        num_injuries += len(patient_injuries)
        for patient_injury in patient_injuries:
            if patient_injury.location not in error:
                error[patient_injury.location] = {"count": 0, "injuries": {}}
            injury_location = error[patient_injury.location]
            injury_location["count"] += 1
            location_injuries = injury_location["injuries"]
            if patient_injury.type not in location_injuries:
                location_injuries[patient_injury.type] = {"count": 0, "severities": []}
            injury = location_injuries[patient_injury.type]
            injury["count"] += 1
            injury["severities"].append(patient_injury.severity)

    # Calculate the stats
    for location, location_distributions in injury_distributions.items():
        # Injury Location Distributions
        location_error = error[location]
        location_error["synthetic_distribution"] = 100 * location_error["count"] / num_injuries
        location_error["actual_distribution"] = location_distributions["percent"]
        location_error["distribution_error"] = (location_error["synthetic_distribution"] -
                                                location_error["actual_distribution"])
        # Injury Type Distributions
        location_severities = []
        injury_distributions = location_distributions["types"]
        for injury, injury_error in location_error["injuries"].items():
            injury_error["synthetic_distribution"] = 100 * injury_error["count"] / location_error["count"]
            injury_error["actual_distribution"] = injury_distributions[injury]["percent"]
            injury_error["distribution_error"] = (injury_error["synthetic_distribution"] -
                                                  injury_error["actual_distribution"])

            location_severities.extend(injury_error["severities"])
            injury_severity = injury_distributions[injury]["severity"]

            if "mean" in injury_severity:
                injury_error["synthetic_severity_mean"] = np.mean(injury_error["severities"])
                injury_error["actual_severity_mean"] = injury_severity["mean"]
                injury_error["severity_mean_error"] = (injury_error["synthetic_severity_mean"] -
                                                       injury_error["actual_severity_mean"])

            if "std" in injury_severity:
                injury_error["synthetic_severity_std"] = np.std(injury_error["severities"])
                injury_error["actual_severity_std"] = injury_severity["std"]
                injury_error["severity_std_error"] = (injury_error["synthetic_severity_std"] -
                                                      injury_error["actual_severity_std"])

        if "mean" in location_distributions:
            location_error["synthetic_severity_mean"] = np.mean(location_severities)
            location_error["actual_severity_mean"] = location_distributions["mean"]
            location_error["severity_mean_error"] = (location_error["synthetic_severity_mean"] -
                                                     location_error["actual_severity_mean"])

        if "std" in location_distributions:
            location_error["synthetic_severity_std"] = np.std(location_severities)
            location_error["actual_severity_std"] = location_distributions["std"]
            location_error["severity_std_error"] = (location_error["synthetic_severity_std"] -
                                                    location_error["actual_severity_std"])

    return error


def plot_injury_error(injury_error: dict, results_stem: str):
    out_dir = Path(results_stem).parent
    out_dir.mkdir(exist_ok=True)

    def _dict_field_value(d: dict, f: str, fmt: str):
        if f in d:
            return f"{d[f]:{fmt}}"
        return ""

    # Error Table
    def _error_row(name: str, error: dict, b: bool):
        if b:
            injury_location = name
            injury_type = ""
        else:
            injury_location = ""
            injury_type = name
        return (injury_location, injury_type,
                _dict_field_value(error, "synthetic_distribution", ".1f"),
                _dict_field_value(error, "actual_distribution", ".1f"),
                _dict_field_value(error, "distribution_error", ".1f"),
                _dict_field_value(error, "synthetic_severity_mean", ".3f"),
                _dict_field_value(error, "actual_severity_mean", ".3f"),
                _dict_field_value(error, "severity_mean_error", ".3f"),
                _dict_field_value(error, "synthetic_severity_std", ".3f"),
                _dict_field_value(error, "actual_severity_std", ".3f"),
                _dict_field_value(error, "severity_std_error", ".3f"))
    data = []
    headings = ["Injury Location", "Injury Type",
                "Synthetic Distribution %", "Actual Distribution %", "Distribution % Error",
                "Synthetic Severity Mean", "Actual Severity Mean", "Severity Mean Error",
                "Synthetic Severity SD", "Actual Severity SD", "Severity SD Error"]
    fields = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]  # All headings
    for location in sorted(injury_error.keys()):
        data.append(_error_row(location, injury_error[location], True))
        injuries = injury_error[location]["injuries"]
        for injury in sorted(injuries.keys()):
            data.append(_error_row(injury, injuries[injury], False))
    _create_report(f"{results_stem}_statistics", data, fields, headings)


def _create_report(basename: str, data, fields, headings, widths=None):
    align = []
    for i in range(len(fields)):
        align.append(('^', '^'))
    f = open(str(basename) + ".md", "w")
    table(f, data, fields, headings, align)
    f.close()

    # Write out table as png
    wrapped_headers = headings  # ["<br>".join(textwrap.wrap(h, width=20)) for h in headings]
    df = pd.DataFrame(data, columns=wrapped_headers)
    df.style.format(escape="html")  # Actually wrap column names
    df_styler = df.style.hide(axis="index") \
        .set_properties(subset=wrapped_headers[1:], **{'text-align': 'center'}) \
        .set_properties(subset=[wrapped_headers[0]], **{'text-align': 'left'}) \
        .set_properties(**{'border': '1px black solid'})
    if widths:
        for i, width in enumerate(widths):
            df_styler = df_styler.set_properties(subset=wrapped_headers[i], **{'width': width})
    df_styler.set_table_styles(table_styles=[
        {'selector': 'th.col_heading', 'props': 'text-align: center; border: 1px black solid;'},
    ], overwrite=False)
    img_filename = str(basename) + ".png"
    _log.info(f"Writing {img_filename}")
    dfi.export(df_styler, img_filename, table_conversion='playwright', dpi=600)


if __name__ == "__main__":
    main()

