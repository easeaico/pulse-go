# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.
import copy

import dataframe_image as dfi
import logging
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import random
import statistics


from itertools import combinations
from pathlib import Path
from pulse.cdm.utils.markdown import table
from scipy.stats import truncnorm

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
    "head_and_neck": {"percent": 36.2, "severity_mean": 2.69, "types": {
        "tbi": {"percent": 22, "severity": {"mean": 3.5, "std": 0.25}},
        "airway_obstruction": {"percent": 18, "severity": {"mean": 4.0, "std": 0.25}},
        "superficial": {"percent": 60, "severity": {"values": [1.0], "percents": [100]}}
    }},
    "thorax": {"percent": 8.6, "severity_mean": 2.85, "polytrauma": {"max": 4, "mean": 2.3}, "types": {
        "pneumothorax": {"percent": 51.8, "severity": {"mean": 2.85, "std": 0.25}},
        "pulmonary_contusion": {"percent": 50.2, "severity": {"mean": 2.85, "std": 0.25}},
        "fracture": {"percent": 51.2, "severity": {"mean": 2.85, "std": 0.25}},
        "hemothorax": {"percent": 30, "severity": {"mean": 2.85, "std": 0.25}},
        "hemorrhage": {"percent": 34.6, "severity": {"mean": 2.85, "std": 0.25}},
        "spinal": {"percent": 14.6, "severity": {"mean": 2.85, "std": 0.25}}
    }},
    "abdomen": {"percent": 6.9, "severity_mean": 2.85, "types": {
        "hemorrhage": {"percent": 34.6, "severity": {"mean": 2.85, "std": 0.25}},
        "laceration_contusion": {"percent": 65.4, "severity": {"mean": 2.85, "std": 0.25}}
    }},
    "extremity": {"percent": 49.4, "severity_mean": 2.05, "types": {
        "hemorrhage": {"percent": 52, "severity": {"values": [1.0, 2.5, 3.5, 4.5], "percents": [56, 23, 17, 7]}},
        "fracture_dislocation": {"percent": 22, "severity": {"values": [1.0, 2.5, 3.5, 4.5], "percents": [56, 23, 17, 7]}},
        "contusion_sprain_strain": {"percent": 20, "severity": {"values": [1.0, 2.5, 3.5, 4.5], "percents": [56, 23, 17, 7]}},
        "burn_nerve": {"percent": 5, "severity": {"values": [1.0, 2.5, 3.5, 4.5], "percents": [56, 23, 17, 7]}},
    }}
}


def main():
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

    # Test specific injury
    if False:
        test_injury(injury_distributions=army_injury_distributions["thorax"],
                    num_patients_injured=1000,
                    log=True)
    if False:
        for p in range(100, 10001, 100):
            max_error = 0
            for i in range(50):
                err = test_injury(injury_distributions=army_injury_distributions["abdomen"],
                                  num_patients_injured=p,
                                  log=False)
                if err > max_error:
                    max_error = err
            _log.info(f"Max Error of {max_error} for {p} patients")

    # Run a measurement study
    if False:
        # Measure error for various population sizes
        for p in range(500, 5001, 500):
            i = 25
            _log.info(f"Measuring error for a population size of {p} using {i} iterations")
            measure_error(iterations=i, population_size=p,
                          population_distributions=army_population_distributions,
                          injury_distributions=army_injury_distributions,
                          results_stem=f"./results/measurements/{p}")

    # Generate a data set
    if True:
        population_size = 1000
        army_patients = synthetic_population_generation(population_size, army_population_distributions)
        army_population_error = calculate_population_error(army_patients, army_population_distributions)
        plot_population_error(army_population_error, f"./results/army_population_of_{population_size}")

        army_patient_injuries = synthetic_injury_generation(population_size, army_injury_distributions)
        army_injury_error = calculate_injury_error(army_patient_injuries, army_injury_distributions)
        plot_injury_error(army_injury_error, f"./results/army_injuries_of_population_of_{population_size}")


def synthetic_population_generation(size: int, distributions: dict) -> dict:
    _log.info(f"Generate data with {size} samples")

    population_data = {}
    
    # Sex
    male_distributions = distributions["sex"]["male"]
    female_distributions = distributions["sex"]["female"]
    if "percent" in female_distributions:
        r = np.random.binomial(n=1, p=female_distributions["percent"] * 0.01, size=size)
        sexes = ["female" if s == 1 else "male" for s in r]
    elif "percent" in male_distributions:
        r = np.random.binomial(n=1, p=male_distributions["percent"] * 0.01, size=size)
        sexes = ["male" if s == 1 else "female" for s in r]
    else:
        _log.error("Must provide the percent percentage of either male's or female's")
        return population_data
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
    age_groups = _weighted_choices(choices=age_bins, percents=distributions["age"]["percents"], size=size)
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


def calculate_population_error(population: dict, distributions: dict) -> dict:
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


def synthetic_injury_generation(population_size: int, distributions: dict) -> list:
    # Array or arrays
    # An array of injuries for each patient
    patient_injuries = []

    # TODO Assuming only 1 injury for each patient
    injury_locations = _weighted_choices(
        choices=list(distributions.keys()),
        percents=[value["percent"] for value in distributions.values()],
        size=population_size)

    # Generate severities for each injury type
    ledger = {}
    for location, injury_distributions in distributions.items():
        injury_types = injury_distributions["types"]
        # Generate a single injury type based on each supported location
        ledger[location] = {"index": 0,
                            "injury_severities": {}}
        num_injured = injury_locations.count(location)
        if "polytrauma" in injury_distributions:
            # Generate a list of injury counts for each patient
            polytrauma = injury_distributions["polytrauma"]
            num_polytrauma_injuries = _bounded_random_choices(mean=polytrauma["mean"], sd=0.5,
                                                           low=1, upp=polytrauma["max"],
                                                           size=num_injured)
            num_polytrauma_injuries = [round(x) for x in num_polytrauma_injuries]
            polytraumas = _weighted_choices(choices=list(injury_types.keys()),
                                            percents=[injury_types[t]["percent"] for t in injury_types],
                                            size=sum(num_polytrauma_injuries))
            ledger[location]["injuries"] = _random_grouping(polytraumas,
                                                            num_polytrauma_injuries,
                                                            list(injury_types.keys()))

            # Check that our tuples don't have more than 2 of any 1 injury
            for injury in injuries:
                if isinstance(injury, tuple) and len(injury) > 2:
                    unique = set(injury)
                    for u in unique:
                        if list(injury).count(u) >= 3:
                            _log.fatal(f"Is this a good injury mix {injury}")
        else:
            ledger[location]["injuries"] = _weighted_choices(
                                                            choices=list(injury_types.keys()),
                                                            percents=[injury_types[t]["percent"] for t in injury_types],
                                                            size=num_injured)
        injuries = ledger[location]["injuries"]
        injury_severities = ledger[location]["injury_severities"]
        for injury_type, dist in injury_types.items():
            severity_dist = dist["severity"]
            if "mean" in severity_dist:
                injury_severities[injury_type] = {"index": 0,
                                                  "severities": np.random.normal(loc=severity_dist["mean"],
                                                                                 scale=severity_dist["std"],
                                                                                 size=_count(injuries, injury_type))}
            elif "values" in severity_dist:
                injury_severities[injury_type] = {"index": 0,
                                                  "severities": _weighted_choices(
                                                      choices=severity_dist["values"],
                                                      percents=severity_dist["percents"],
                                                      size=_count(injuries, injury_type))}

    # Map the types and severities back to the injury locations
    for location in injury_locations:
        type_ledger = ledger[location]
        injury_type = type_ledger["injuries"][type_ledger["index"]]
        type_ledger["index"] += 1
        if isinstance(injury_type, str):
            severity_ledger = type_ledger["injury_severities"][injury_type]
            injury_severity = severity_ledger["severities"][severity_ledger["index"]]
            severity_ledger["index"] += 1

            patient_injuries.append([Injury(
                location,
                injury_type,
                injury_severity)])
        else:
            patient_injuries.append([])
            for injury_type_str in injury_type:
                severity_ledger = type_ledger["injury_severities"][injury_type_str]
                injury_severity = severity_ledger["severities"][severity_ledger["index"]]
                severity_ledger["index"] += 1

                patient_injuries[-1].append(Injury(
                    location,
                    injury_type_str,
                    injury_severity))

    return patient_injuries


def calculate_injury_error(patients_injuries: list, injury_distributions: dict) -> dict:
    error = {}

    # Count everything up, and gather all our severities
    num_injuries = 0
    for patient_injuries in patients_injuries:
        num_injuries += len(patient_injuries)
        locations = set()
        for patient_injury in patient_injuries:
            locations.add(patient_injury.location)
            if patient_injury.location not in error:
                error[patient_injury.location] = {"count": 0, "injuries": {}}
            injury_location = error[patient_injury.location]
            location_injuries = injury_location["injuries"]
            if patient_injury.type not in location_injuries:
                location_injuries[patient_injury.type] = {"count": 0, "severities": []}
            injury = location_injuries[patient_injury.type]
            injury["count"] += 1
            injury["severities"].append(patient_injury.severity)
        for location in locations:
            injury_location = error[location]
            injury_location["count"] += 1

    # Calculate the stats
    for location, location_distributions in injury_distributions.items():
        # Injury Location Distributions
        location_error = error[location]
        location_error["synthetic_distribution"] = 100 * location_error["count"] / len(patients_injuries)
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
                _dict_field_value(error, "count", "d"),
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
    headings = ["Injury Location", "Injury Type", "Count",
                "Synthetic Distribution %", "Actual Distribution %", "Distribution % Error",
                "Synthetic Severity Mean", "Actual Severity Mean", "Severity Mean Error",
                "Synthetic Severity SD", "Actual Severity SD", "Severity SD Error"]
    fields = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]  # All headings
    for location in sorted(injury_error.keys()):
        data.append(_error_row(location, injury_error[location], True))
        injuries = injury_error[location]["injuries"]
        for injury in sorted(injuries.keys()):
            data.append(_error_row(injury, injuries[injury], False))
    _create_report(f"{results_stem}_statistics", data, fields, headings)


def measure_error(iterations: int, population_size: int,
                  population_distributions: dict,  injury_distributions: dict,
                  results_stem: str) -> dict:
    out_dir = Path(results_stem).parent
    out_dir.mkdir(exist_ok=True)

    error = {
        "demographics": {"female_distribution": {"min": None, "max": None, "mean": None, "errors": []},
                         "female_height_mean": {"min": None, "max": None, "mean": None, "errors": []},
                         "female_height_std": {"min": None, "max": None, "mean": None, "errors": []},
                         "female_bmi_mean": {"min": None, "max": None, "mean": None, "errors": []},
                         "female_bmi_std": {"min": None, "max": None, "mean": None, "errors": []},
                         "male_distribution": {"min": None, "max": None, "mean": None, "errors": []},
                         "male_height_mean": {"min": None, "max": None, "mean": None, "errors": []},
                         "male_height_std": {"min": None, "max": None, "mean": None, "errors": []},
                         "male_bmi_mean": {"min": None, "max": None, "mean": None, "errors": []},
                         "male_bmi_std": {"min": None, "max": None, "mean": None, "errors": []},
                         "heart_rate_mean": {"min": None, "max": None, "mean": None, "errors": []},
                         "heart_rate_std": {"min": None, "max": None, "mean": None, "errors": []}},
        "injuries": {}
    }

    demographics = error["demographics"]
    injuries = error["injuries"]
    for i in range(iterations):
        patients = synthetic_population_generation(population_size, population_distributions)
        population_error = calculate_population_error(patients, population_distributions)

        demographics["female_distribution"]["errors"].append(population_error["sex"]["female"]["count"]["error"])
        demographics["female_height_mean"]["errors"].append(population_error["sex"]["female"]["height"]["mean_error"])
        demographics["female_height_std"]["errors"].append(population_error["sex"]["female"]["height"]["std_error"])
        demographics["female_bmi_mean"]["errors"].append(population_error["sex"]["female"]["bmi"]["mean_error"])
        demographics["female_bmi_std"]["errors"].append(population_error["sex"]["female"]["bmi"]["std_error"])
        demographics["male_distribution"]["errors"].append(population_error["sex"]["male"]["count"]["error"])
        demographics["male_height_mean"]["errors"].append(population_error["sex"]["male"]["height"]["mean_error"])
        demographics["male_height_std"]["errors"].append(population_error["sex"]["male"]["height"]["std_error"])
        demographics["male_bmi_mean"]["errors"].append(population_error["sex"]["male"]["bmi"]["mean_error"])
        demographics["male_bmi_std"]["errors"].append(population_error["sex"]["male"]["bmi"]["std_error"])
        demographics["heart_rate_mean"]["errors"].append(population_error["heart_rate"]["mean_error"])
        demographics["heart_rate_std"]["errors"].append(population_error["heart_rate"]["std_error"])

        patient_injuries = synthetic_injury_generation(population_size, injury_distributions)
        injury_error = calculate_injury_error(patient_injuries, injury_distributions)

        for location, location_stats in injury_error.items():
            if location not in injuries:
                injuries[location] = {"types": {},
                                      "distribution": {"min": None, "max": None, "mean": None, "errors": []},
                                      "severity_mean": {"min": None, "max": None, "mean": None, "errors": []},
                                      "severity_std": {"min": None, "max": None, "mean": None, "errors": []}}
            location_measurements = injuries[location]
            if "distribution_error" in location_stats:
                location_measurements["distribution"]["errors"].append(location_stats["distribution_error"])
            if "severity_mean_error" in location_stats:
                location_measurements["severity_mean"]["errors"].append(location_stats["severity_mean_error"])
            if "severity_std_error" in location_stats:
                location_measurements["severity_std"]["errors"].append(location_stats["severity_std_error"])

            injury_types = location_measurements["types"]
            for injury, injury_stats in location_stats["injuries"].items():
                if injury not in injury_types:
                    injury_types[injury] = {"distribution": {"min": None, "max": None, "mean": None, "errors": []},
                                            "severity_mean": {"min": None, "max": None, "mean": None, "errors": []},
                                            "severity_std": {"min": None, "max": None, "mean": None, "errors": []}}
                injury_measurements = injury_types[injury]
                if "distribution_error" in injury_stats:
                    injury_measurements["distribution"]["errors"].append(injury_stats["distribution_error"])
                if "severity_mean_error" in injury_stats:
                    injury_measurements["severity_mean"]["errors"].append(injury_stats["severity_mean_error"])
                if "severity_std_error" in injury_stats:
                    injury_measurements["severity_std"]["errors"].append(injury_stats["severity_std_error"])

    def accumulate(measurements: dict):
        errors = measurements["errors"]
        if len(errors) > 0:
            measurements["min"] = min(errors, key=abs)
            measurements["max"] = max(errors, key=abs)
            measurements["mean"] = statistics.mean(errors)

    accumulate(demographics["female_distribution"])
    accumulate(demographics["female_height_mean"])
    accumulate(demographics["female_height_std"])
    accumulate(demographics["female_bmi_mean"])
    accumulate(demographics["female_bmi_std"])
    accumulate(demographics["male_distribution"])
    accumulate(demographics["male_height_mean"])
    accumulate(demographics["male_height_std"])
    accumulate(demographics["male_bmi_mean"])
    accumulate(demographics["male_bmi_std"])
    accumulate(demographics["heart_rate_mean"])
    accumulate(demographics["heart_rate_std"])

    for location_measurements in injuries.values():
        accumulate(location_measurements["distribution"])
        accumulate(location_measurements["severity_mean"])
        accumulate(location_measurements["severity_std"])

        for injury_measurements in location_measurements["types"].values():
            accumulate(injury_measurements["distribution"])
            accumulate(injury_measurements["severity_mean"])
            accumulate(injury_measurements["severity_std"])

    demographic_rows = []
    demographic_headings = ["Descriptor",
                            "Min", "Max", "Mean",]
    demographic_fields = [0, 1, 2, 3]  # All headings
    for descriptor, stats in demographics.items():
        fmt = ".1f" if "distribution" in descriptor else ".3f"
        demographic_rows.append((descriptor,
                                 f"{stats['min']:{fmt}}",
                                 f"{stats['max']:{fmt}}",
                                 f"{stats['mean']:{fmt}}"))
    _log.info(f"\tGenerating demographic reports...")
    _create_report(f"{results_stem}_demographics", demographic_rows, demographic_fields, demographic_headings)

    def _injury_dict_field_value(d: dict, f1: str, f2: str, fmt: str):
        if f1 in d and d[f1][f2]:
            return f"{d[f1][f2]:{fmt}}"
        return ""

    # Error Table
    def _injury_row(name: str, m: dict, b: bool):
        if b:
            injury_location = name
            injury_type = ""
        else:
            injury_location = ""
            injury_type = name
        return (injury_location, injury_type,
                _injury_dict_field_value(m, "distribution", "min", ".1f"),
                _injury_dict_field_value(m, "distribution", "max", ".1f"),
                _injury_dict_field_value(m, "distribution", "mean", ".1f"),
                _injury_dict_field_value(m, "severity_mean", "min", ".3f"),
                _injury_dict_field_value(m, "severity_mean", "max", ".3f"),
                _injury_dict_field_value(m, "severity_mean", "mean", ".3f"),
                _injury_dict_field_value(m, "severity_std", "min", ".3f"),
                _injury_dict_field_value(m, "severity_std", "max", ".3f"),
                _injury_dict_field_value(m, "severity_std", "mean", ".3f"))

    injury_rows = []
    injury_headings = ["Injury Location", "Injury Type",
                       "Min Distribution %", "Max Distribution %", "Mean Distribution %",
                       "Min Severity Mean", "Max Severity Mean", "Mean Severity Mean",
                       "Min Severity SD", "Max Severity SD", "Mean Severity SD"]
    injury_fields = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]  # All headings
    for location in sorted(injuries.keys()):
        injury_rows.append(_injury_row(location, injuries[location], True))
        types = injuries[location]["types"]
        for type_ in sorted(types.keys()):
            injury_rows.append(_injury_row(type_, types[type_], False))
    _log.info(f"\tGenerating injury reports...")
    _create_report(f"{results_stem}_injuries", injury_rows, injury_fields, injury_headings)

    return error


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


def _random_grouping(pool: list, groups: list, choices: list) -> list:
    seed = random.Random()
    grouped_list = []
    for group in groups:
        if group == 1:
            # Just pick something random from the pool
            pick = seed.choice(pool)
            # And remove it from the pool
            pool.remove(pick)
            # That's all in this group
            grouped_list.append(pick)
        else:
            # Count up how many of each choice we have
            counts = [(c, pool.count(c)) for c in choices]
            # Remove any counts of choices no longer in the pool
            i = 0
            while i < len(counts):
                if counts[i][1] == 0:
                    counts.remove(counts[i])
                else:
                    i += 1

            # Which choice do we have the most of?
            sorted_counts = sorted(counts, key=lambda x: x[1], reverse=True)
            # Add that choice to this group
            g = [sorted_counts[0][0]]
            # Take one of those choices out of the pool
            pool.remove(g[0])
            # If this was the last of this choice in the pool, remove it from our counts
            if sorted_counts[0][1] == 1:
                sorted_counts.remove(sorted_counts[0])
            for _ in range(group-1):
                if len(sorted_counts) == 0:
                    _log.error("We are all out of choices...")  # We shouldn't ever get here....
                # Pick a random choice from our counts
                i = random.randint(0, len(sorted_counts)-1)
                # Add it to this group
                g.append(sorted_counts[i][0])
                try:
                    # Remove an instance of this choice from the pool
                    pool.remove(g[-1])
                except ValueError as e:
                    _log.error(f"Nuts {e}")
                # Now remove this choice from our counts, so we don't pick it again
                del sorted_counts[i]
            # Add this group to our list
            grouped_list.append(tuple(g))

    return grouped_list


def _weighted_choices(choices: list, size: int, percents: list, algo: int = 2) -> list:

    # Check to see if this list has any tuples
    # Remove the tuples. np.random does not like tuples in the choice list
    has_tuple = False
    for idx, item in enumerate(choices):
        if isinstance(item, tuple):
            has_tuple = True
            if isinstance(item, tuple):
                choices[idx] = ";".join(item)

    result = None
    if algo == 0:
        try:
            result = list(np.random.choice(choices, size=size, p=_normalize_list(percents)))
        except ValueError as e:
            _log.warning(f"{e}")
    elif algo == 1:
        result = random.choices(choices, weights=percents, k=size)
    elif algo == 2:
        result = []
        p = _normalize_list(percents)
        for i, choice in enumerate(choices):
            result.extend([choice] * round(size*p[i]))

        if len(result) < size:
            to_add = size - len(result)
            if to_add > 1:
                _log.info(f"Generated too few choices, adding {to_add}")
            result.extend(list(np.random.choice(choices, size=to_add, p=p)))
        elif len(result) > size:
            to_take_away = len(result)-size
            if to_take_away > 1:
                _log.info(f"Generated too many choices, removing {to_take_away}")
            result = result[:-to_take_away]

    if result:
        if has_tuple:  # Turn choices and results back into tuples
            for idx, item in enumerate(choices):
                if ';' in item:
                    choices[idx] = tuple(item.split(";"))
            for idx, item in enumerate(result):
                if ';' in item:
                    result[idx] = tuple(item.split(";"))
        return result

    _log.error("Unknown algo for _weighted_randomness")
    return [0.0] * size


def _bounded_random_choices(mean: float, sd: float, low: int, upp: int, size: int):
    return truncnorm(
        (low - mean) / sd, (upp - mean) / sd, loc=mean, scale=sd).rvs(size)


def generate_combinations(choices: list, max_in_a_choice: int) -> list:

    selections = copy.deepcopy(choices)
    for i in range(2, max_in_a_choice+1, 1):
        _log.info(f"Creating sets of {i} injuries")
        selections.extend(list(combinations(choices, i)))

    return selections


def _normalize_list(data: list):
    if len(data) == 1:
        return [1.0]

    min_val = min(data)
    max_val = max(data)

    if min_val == max_val:
        return [0.0] * len(data)

    total = sum(data)
    normalized_data = [x / total for x in data]
    return normalized_data


def _count(list_: list, find: str = None):
    # if find is None, we count all items
    count = 0
    for i in list_:
        if isinstance(i, tuple):
            for t in i:
                if not find or t == find:
                    count += 1
        else:
            if not find or i == find:
                count += 1
    return count


def test_injury(injury_distributions: dict, num_patients_injured: int, log: bool = True):
    injury_types = injury_distributions["types"]
    if log:
        _log.info(f"Total number of patients: {num_patients_injured}")

    if "polytrauma" in injury_distributions:
        polytrauma = injury_distributions["polytrauma"]
        num_patient_injuries = _bounded_random_choices(mean=polytrauma["mean"], sd=0.5,
                                                       low=1, upp=polytrauma["max"],
                                                       size=num_patients_injured)
        num_patient_injuries = [round(x) for x in num_patient_injuries]
        num_location_injuries = sum(num_patient_injuries)
        injuries = _weighted_choices(choices=list(injury_types.keys()),
                                     percents=[t["percent"] for t in injury_types.values()],
                                     size=num_location_injuries)
        injuries = _random_grouping(injuries, num_patient_injuries, list(injury_types.keys()))

        polytrauma_patients = [0] * polytrauma["max"]
        if log:
            for i in num_patient_injuries:
                polytrauma_patients[i - 1] += 1
            for i, c in enumerate(polytrauma_patients):
                _log.info(f"  There are {c} patients with {i + 1} injuries")
            _log.info(f"Mean number of injuries per patient: {num_location_injuries / num_patients_injured}")
            _log.info(f"Total number of injuries for all patient: {num_location_injuries}")

        # Check that our tuples don't have more than 2 of any 1 injury
        for injury in injuries:
            if isinstance(injury, tuple) and len(injury) > 2:
                unique = set(injury)
                for u in unique:
                    if list(injury).count(u) >= 3:
                        _log.fatal(f"Is this a good injury mix {injury}")

    else:
        injuries = _weighted_choices(choices=list(injury_types.keys()),
                                     percents=[injury_types[t]["percent"] for t in injury_types],
                                     size=num_patients_injured)

    max_error = 0
    for injury_type, distribution in injury_types.items():
        num_injuries = _count(injuries, injury_type)
        synthetic_distribution = 100 * num_injuries/num_patients_injured
        actual_distribution = injury_types[injury_type]['percent']
        if abs(synthetic_distribution - actual_distribution) > max_error:
            max_error = abs(synthetic_distribution - actual_distribution)
        if log:
            _log.info(f"There are {num_injuries} {injury_type} injuries")
            _log.info(f" Synthetic distribution: {synthetic_distribution}%")
            _log.info(f" Actual distribution: {actual_distribution}%")
            _log.info(f" Distribution Error: {synthetic_distribution - actual_distribution}%")

    return max_error


if __name__ == "__main__":
    main()

