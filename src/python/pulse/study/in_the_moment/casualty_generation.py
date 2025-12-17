# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import copy
import logging
import matplotlib.pyplot as plt
import numpy as np
import random
import statistics

from itertools import combinations
from pathlib import Path
from scipy.stats import truncnorm, norm

from pulse.cdm.utils.math_utils import percent_difference
from pulse.study.in_the_moment.triage_dataset import create_report

_log = logging.getLogger("pulse")


class InjurySeverityOpts:
    force_valid_distributions = False
    max_valid_percent_difference = 5.0
    halt_on_error = True


def to_specification_lists(study: dict) -> dict:
    # Turn the study map (id->casualty) to a map of specification parameter->[values of that parameter]
    spec = {}
    for casualty in study.values():
        s = casualty["specification"]
        for key, value in s.items():
            if key not in spec:
                spec[key] = []
            spec[key].append(value)
    return spec


def to_severity_lists(spec: dict) -> dict:
    severities = {}
    for injuries in spec["injuries"]:
        for injury in injuries:
            loc = injury["location"]
            typ = injury["type"]
            sev = injury["severity"]
            if loc not in severities:
                severities[loc] = {}
            if typ not in severities[loc]:
                severities[loc][typ] = []
            severities[loc][typ].append(sev)
    return severities


def casualty_population_generation(size: int, distributions: dict) -> dict:

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
    female_heights = np.random.normal(loc=female_distributions["height_cm"]["mean"],
                                      scale=female_distributions["height_cm"]["std"],
                                      size=num_females)

    male_heights = np.random.normal(loc=male_distributions["height_cm"]["mean"],
                                    scale=male_distributions["height_cm"]["std"],
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
    heart_rates = list(np.random.normal(loc=distributions["heart_rate_bpm"]["mean"],
                                        scale=distributions["heart_rate_bpm"]["std"],
                                        size=size))

    # Age
    num_bins = len(distributions["age_yr"]["bins"])
    num_percents = len(distributions["age_yr"]["percents"])
    if num_bins != num_percents+1:
        _log.error("Age bins must be 1 more that the percents length")
        _log.error(f"Provided {len(distributions['age_yr']['bins'])} bins")
        _log.error(f"Provided {len(distributions['age_yr']['percents'])} percents")
        return population_data

    age_bins = []
    for i in range(num_percents):
        age_min = distributions["age_yr"]["bins"][i]
        age_max = distributions["age_yr"]["bins"][i+1]
        if i > 0:
            age_min += 1
        age_bins.append(f"{age_min}-{age_max}")

    ages = []
    age_groups = _weighted_choices(choices=age_bins, percents=distributions["age_yr"]["percents"], size=size)
    for age_group in age_groups:
        idx = age_bins.index(age_group)
        low = distributions["age_yr"]["bins"][idx]
        high = distributions["age_yr"]["bins"][idx+1]
        if idx > 0:
            low += 1
        ages.append(np.random.randint(low, high+1))

    population_data["sex"] = sexes
    population_data["age_yr"] = ages
    population_data["height_cm"] = heights
    population_data["bmi"] = bmi
    population_data["heart_rate_bpm"] = heart_rates

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
    female_count["error"] = percent_difference(female_count["synthetic"], female_count["actual"])
    male_count["synthetic"] = 100 * population["sex"].count("male") / len(population["sex"])
    male_count["error"] = percent_difference(male_count["synthetic"], male_count["actual"])

    # Height and BMI
    female_heights = []
    male_heights = []
    female_bmis = []
    male_bmis = []
    for i, sex in enumerate(population["sex"]):
        if sex == "female":
            female_heights.append(population["height_cm"][i])
            female_bmis.append(population["bmi"][i])
        else:
            male_heights.append(population["height_cm"][i])
            male_bmis.append(population["bmi"][i])

    female_height = {"synthetic_mean": np.mean(female_heights),
                     "actual_mean": distributions["sex"]["female"]["height_cm"]["mean"],
                     "synthetic_std": np.std(female_heights),
                     "actual_std": distributions["sex"]["female"]["height_cm"]["std"]}
    female_height["mean_error"] = percent_difference(female_height["synthetic_mean"], female_height["actual_mean"])
    female_height["std_error"] = percent_difference(female_height["synthetic_std"], female_height["actual_std"])

    male_height = {"synthetic_mean": np.mean(male_heights),
                   "actual_mean": distributions["sex"]["male"]["height_cm"]["mean"],
                   "synthetic_std": np.std(male_heights),
                   "actual_std": distributions["sex"]["male"]["height_cm"]["std"]}
    male_height["mean_error"] = percent_difference(male_height["synthetic_mean"], male_height["actual_mean"])
    male_height["std_error"] = percent_difference(male_height["synthetic_std"], male_height["actual_std"])

    female_bmi = {"synthetic_mean": np.mean(female_bmis),
                  "actual_mean": distributions["sex"]["female"]["bmi"]["mean"],
                  "synthetic_std": np.std(female_bmis),
                  "actual_std": distributions["sex"]["female"]["bmi"]["std"]}
    female_bmi["mean_error"] = percent_difference(female_bmi["synthetic_mean"], female_bmi["actual_mean"])
    female_bmi["std_error"] = percent_difference(female_bmi["synthetic_std"], female_bmi["actual_std"])

    male_bmi = {"synthetic_mean": np.mean(male_bmis),
                "actual_mean": distributions["sex"]["male"]["bmi"]["mean"],
                "synthetic_std": np.std(male_bmis),
                "actual_std": distributions["sex"]["male"]["bmi"]["std"]}
    male_bmi["mean_error"] = percent_difference(male_bmi["synthetic_mean"], male_bmi["actual_mean"])
    male_bmi["std_error"] = percent_difference(male_bmi["synthetic_std"], male_bmi["actual_std"])

    error["sex"] = {"female": {"count": female_count, "height_cm": female_height, "bmi": female_bmi},
                    "male": {"count": male_count, "height_cm": male_height, "bmi": male_bmi}}

    # Heart Rate
    error["heart_rate_bpm"] = {"synthetic_mean": np.mean(population["heart_rate_bpm"]),
                           "actual_mean": distributions["heart_rate_bpm"]["mean"],
                           "synthetic_std": np.std(population["heart_rate_bpm"]),
                           "actual_std": distributions["heart_rate_bpm"]["std"]}
    error["heart_rate_bpm"]["mean_error"] = (
        percent_difference(error["heart_rate_bpm"]["synthetic_mean"], error["heart_rate_bpm"]["actual_mean"]))
    error["heart_rate_bpm"]["std_error"] = (
        percent_difference(error["heart_rate_bpm"]["synthetic_std"], error["heart_rate_bpm"]["actual_std"]))

    # Age
    age_bins = distributions["age_yr"]["bins"]
    actual_age_counts = distributions["age_yr"]["counts"]
    synthetic_age_counts, bins = np.histogram(population["age_yr"], bins=age_bins)
    error["age_yr"] = {"bins": age_bins,
                       "synthetic_counts": synthetic_age_counts / synthetic_age_counts.sum(),
                       "actual_counts": [x / sum(actual_age_counts) for x in actual_age_counts]}

    return error


def plot_population(ages: dict, img_dir: Path):
    groups = []
    age_bins = ages["bins"]
    for i in range(len(age_bins)-1):
        if i < len(age_bins)-2:
            groups.append(f"{age_bins[i]}-{age_bins[i+1]-1}")
        else:
            groups.append(f"{age_bins[i]}-{age_bins[i+1]}")
    synthetic_age_counts = ages["percents"]
    plt.xlabel("Age Groups")
    plt.ylabel('Percent %')
    plt.bar(groups, synthetic_age_counts)
    plt.savefig(img_dir/f"age_histogram.png", format="jpeg")
    plt.clf()  # Clears the entire figure
    plt.close()


def plot_population_error(population_error: dict, results_stem: str):

    # Age
    age_bins = population_error["age_yr"]["bins"]
    actual_age_counts = population_error["age_yr"]["actual_counts"]
    synthetic_age_counts = population_error["age_yr"]["synthetic_counts"]
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
    plt.clf()  # Clears the entire figure
    plt.close()

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
                "Synthetic Mean", "Actual Mean", "Mean % Difference",
                "Synthetic SD", "Actual SD", "SD % Difference"]
    fields = [0, 1, 2, 3, 4, 5, 6]  # All headings
    data.append(_error_row("Female Height", population_error["sex"]["female"]["height_cm"]))
    data.append(_error_row("Female BMI", population_error["sex"]["female"]["bmi"]))
    data.append(_error_row("Male Height", population_error["sex"]["male"]["height_cm"]))
    data.append(_error_row("Male BMI", population_error["sex"]["male"]["bmi"]))
    data.append(_error_row("Heart Rate", population_error["heart_rate_bpm"]))
    create_report(f"{results_stem}_statistics", data, fields, headings)

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
    create_report(f"{results_stem}_sex", data, fields, headings)


def _injury(location_: str, type_: str, severity_: float) -> dict:
    injury = {"location": location_,
              "severity": severity_,
              "type": type_,
              "sub_type": None,
              "cmpt": None,
              "can_intervene": False}

    # Randomize if injuries interventions can be applied to this injury
    if type_ == "airway_obstruction":
        # Flip a coin to see if the airway can be repositioned or not
        injury["can_intervene"] = np.random.randint(0, 2) == 1
        return injury

    if injury["location"] == "thorax":
        if type_ == "pneumothorax":
            injury["can_intervene"] = True

    if injury["location"] == "abdomen":
        if type_ == "hemorrhage":  # External Liver/Spleen Hemorrhage
            injury["can_intervene"] = True
            if np.random.randint(0, 2) == 1:
                injury["cmpt"] = "liver"
            else:
                injury["cmpt"] = "spleen"
        else:  # Flip a coin if an external laceration or internal bruise/bleed
            if np.random.randint(0, 2) == 1:
                injury["sub_type"] = "laceration"
                injury["can_intervene"] = True
            else:
                injury["sub_type"] = "contusion"
                injury["can_intervene"] = False

    if injury["location"] == "extremity":
        if type_ == "burn_nerve":
            if np.random.randint(0, 2) == 1:
                injury["sub_type"] = "burn"
            else:
                injury["sub_type"] = "nerve"
            cmpt = np.random.randint(0, 4)
            if cmpt == 0:
                injury["cmpt"] = "left_arm"
            elif cmpt == 1:
                injury["cmpt"] = "right_arm"
            elif cmpt == 2:
                injury["cmpt"] = "left_leg"
            elif cmpt == 3:
                injury["cmpt"] = "right_leg"
        elif type_ == "contusion_sprain_strain":
            t = np.random.randint(0, 3)
            if t == 0:
                injury["sub_type"] = "contusion"
            elif t == 1:
                injury["sub_type"] = "sprain"
            elif t == 2:
                injury["sub_type"] = "strain"
            cmpt = np.random.randint(0, 4)
            if cmpt == 0:
                injury["cmpt"] = "left_arm"
            elif cmpt == 1:
                injury["cmpt"] = "right_arm"
            elif cmpt == 2:
                injury["cmpt"] = "left_leg"
            elif cmpt == 3:
                injury["cmpt"] = "right_leg"
        elif type_ == "fracture_dislocation":
            if np.random.randint(0, 2) == 1:
                injury["sub_type"] = "fracture"
            else:
                injury["sub_type"] = "dislocation"
            cmpt = np.random.randint(0, 4)
            if cmpt == 0:
                injury["cmpt"] = "left_arm"
            elif cmpt == 1:
                injury["cmpt"] = "right_arm"
            elif cmpt == 2:
                injury["cmpt"] = "left_leg"
            elif cmpt == 3:
                injury["cmpt"] = "right_leg"
        elif type_ == "hemorrhage":
            injury["can_intervene"] = True
            if severity_ <= 4:
                cmpt = np.random.randint(0, 4)
                if cmpt == 0:
                    injury["cmpt"] = "left_arm"
                elif cmpt == 1:
                    injury["cmpt"] = "right_arm"
                elif cmpt == 2:
                    injury["cmpt"] = "left_leg"
                elif cmpt == 3:
                    injury["cmpt"] = "right_leg"
            else:
                if np.random.randint(0, 2) == 1:
                    injury["cmpt"] = "left_leg"
                else:
                    injury["cmpt"] = "right_leg"
    return injury


def population_injury_generation(population_size: int, distributions: dict, opts: InjurySeverityOpts = None):
    if not opts:
        opts = InjurySeverityOpts()
    # Array or arrays
    # An array of injuries for each patient
    patient_injuries = []

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
                                                            injury_types)

            # Check that our tuples don't have more than 2 of any 1 injury
            for injury in ledger[location]["injuries"]:
                if isinstance(injury, tuple) and len(injury) > 2:
                    unique = set(injury)
                    for u in unique:
                        if list(injury).count(u) >= 3:
                            _log.fatal(f"Is this a good injury mix {injury}")
        else:
            injury_set = _weighted_choices(choices=list(injury_types.keys()),
                                           percents=[injury_types[t]["percent"] for t in injury_types],
                                           size=num_injured)
            ledger[location]["injuries"] = injury_set
        injuries = ledger[location]["injuries"]
        injury_severities = ledger[location]["injury_severities"]
        for injury_type, dist in injury_types.items():
            randomized_severities = None
            severity_dist = dist["severity"]
            if "mean" in severity_dist:
                mean = None
                pdiff = None
                acceptable_distribution = False
                for i in range(10):
                    randomized_severities = _bounded_random_normal(mean=severity_dist["mean"],
                                                                   stdev=severity_dist["std"],
                                                                   size=_count(injuries, injury_type))
                    mean = statistics.mean(randomized_severities)
                    if not opts.force_valid_distributions:
                        pdiff = percent_difference(severity_dist["mean"], mean)
                        if pdiff <= opts.max_valid_percent_difference:
                            acceptable_distribution = True
                        break
                    else:
                        if i == 0:
                            _log.info(f"Iterating _bounded_random_normal for acceptable random severity distribution")
                        pdiff = percent_difference(severity_dist["mean"], mean)
                        if pdiff <= opts.max_valid_percent_difference:
                            _log.info(f"Acceptable mean generated on iteration {i+2}")
                            acceptable_distribution = True
                            break
                if not acceptable_distribution:
                    _log.error(f"Did not generate valid random severity distribution for {location}-{injury_type}:")
                    _log.error(f"\t% Diff of {pdiff:.2f}% for len={len(randomized_severities)}; "
                               f"Expected: {severity_dist['mean']}, Generated: {mean}")
                    if opts.halt_on_error:
                        exit(1)
                else:
                    _log.info(f"% diff for {location}-{injury_type} severities {pdiff:.2f}% "
                              f"(len={len(randomized_severities)})")

            elif "values" in severity_dist:
                randomized_severities = _weighted_choices(choices=severity_dist["values"],
                                                          percents=severity_dist["percents"],
                                                          size=_count(injuries, injury_type))
            else:
                _log.error("Unsupported severity randomization specification")
                exit(1)
            injury_severities[injury_type] = {"index": 0, "severities": randomized_severities}

    # Map the types and severities back to the injury locations
    for location in injury_locations:
        type_ledger = ledger[location]
        injury_type = type_ledger["injuries"][type_ledger["index"]]
        type_ledger["index"] += 1
        if isinstance(injury_type, str):
            severity_ledger = type_ledger["injury_severities"][injury_type]
            injury_severity = severity_ledger["severities"][severity_ledger["index"]]
            severity_ledger["index"] += 1

            patient_injuries.append([_injury(
                location_=location,
                type_=injury_type,
                severity_=injury_severity)])
        else:
            patient_injuries.append([])
            for injury_type_str in injury_type:
                severity_ledger = type_ledger["injury_severities"][injury_type_str]
                injury_severity = severity_ledger["severities"][severity_ledger["index"]]
                severity_ledger["index"] += 1

                patient_injuries[-1].append(_injury(
                    location_=location,
                    type_=injury_type_str,
                    severity_=injury_severity))

    # Randomize the injuries a few times
    for _ in range(5):
        random.shuffle(patient_injuries)
    return patient_injuries


def calculate_injury_error(patients_injuries: list, injury_distributions: dict) -> dict:
    error = {}

    # Count everything up, and gather all our severities
    num_injuries = 0
    for patient_injuries in patients_injuries:
        num_injuries += len(patient_injuries)
        locations = set()
        for patient_injury in patient_injuries:
            locations.add(patient_injury["location"])
            if patient_injury["location"] not in error:
                error[patient_injury["location"]] = {"count": 0, "injuries": {}}
            injury_location = error[patient_injury["location"]]
            location_injuries = injury_location["injuries"]
            if patient_injury["type"] not in location_injuries:
                location_injuries[patient_injury["type"]] = {"count": 0, "severities": []}
            injury = location_injuries[patient_injury["type"]]
            injury["count"] += 1
            injury["severities"].append(patient_injury["severity"])
        for location in locations:
            injury_location = error[location]
            injury_location["count"] += 1

    # Calculate the stats
    for location, location_distributions in injury_distributions.items():
        # Injury Location Distributions
        location_error = error[location]
        location_error["synthetic_distribution"] = 100 * location_error["count"] / len(patients_injuries)
        location_error["actual_distribution"] = location_distributions["percent"]
        location_error["distribution_error"] = percent_difference(location_error["synthetic_distribution"],
                                                                  location_error["actual_distribution"])
        # Injury Type Distributions
        location_severities = []
        injury_distributions = location_distributions["types"]
        for injury, injury_error in location_error["injuries"].items():
            injury_error["synthetic_distribution"] = 100 * injury_error["count"] / location_error["count"]
            injury_error["actual_distribution"] = injury_distributions[injury]["percent"]
            injury_error["distribution_error"] = percent_difference(injury_error["synthetic_distribution"],
                                                                    injury_error["actual_distribution"])

            location_severities.extend(injury_error["severities"])
            injury_severity = injury_distributions[injury]["severity"]

            if "mean" in injury_severity:
                injury_error["synthetic_severity_mean"] = np.mean(injury_error["severities"])
                injury_error["actual_severity_mean"] = injury_severity["mean"]
                injury_error["severity_mean_error"] = percent_difference(injury_error["synthetic_severity_mean"],
                                                                         injury_error["actual_severity_mean"])

            if "std" in injury_severity:
                injury_error["synthetic_severity_std"] = np.std(injury_error["severities"])
                injury_error["actual_severity_std"] = injury_severity["std"]
                injury_error["severity_std_error"] = percent_difference(injury_error["synthetic_severity_std"],
                                                                        injury_error["actual_severity_std"])

        if "mean" in location_distributions:
            location_error["synthetic_severity_mean"] = np.mean(location_severities)
            location_error["actual_severity_mean"] = location_distributions["mean"]
            location_error["severity_mean_error"] = percent_difference(location_error["synthetic_severity_mean"],
                                                                       location_error["actual_severity_mean"])

        if "std" in location_distributions:
            location_error["synthetic_severity_std"] = np.std(location_severities)
            location_error["actual_severity_std"] = location_distributions["std"]
            location_error["severity_std_error"] = percent_difference(location_error["synthetic_severity_std"],
                                                                      location_error["actual_severity_std"])

    return error


def plot_injury_error(injury_error: dict, results_stem: str):

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
                _dict_field_value(error, "severity_mean_error", ".3f"))
    data = []
    headings = ["Injury Location", "Injury Type", "Count",
                "Synthetic Distribution %", "Actual Distribution %", "Distribution % Difference",
                "Synthetic Severity Mean", "Actual Severity Mean", "Severity Mean % Difference"]
    fields = [0, 1, 2, 3, 4, 5, 6, 7, 8]  # All headings
    for location in sorted(injury_error.keys()):
        data.append(_error_row(location, injury_error[location], True))
        injuries = injury_error[location]["injuries"]
        for injury in sorted(injuries.keys()):
            data.append(_error_row(injury, injuries[injury], False))
    create_report(f"{results_stem}_injury_statistics", data, fields, headings)


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
        patients = casualty_population_generation(population_size, population_distributions)
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

        patient_injuries = population_injury_generation(population_size, injury_distributions)
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
    create_report(f"{results_stem}_demographics", demographic_rows, demographic_fields, demographic_headings)

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
    create_report(f"{results_stem}_injuries", injury_rows, injury_fields, injury_headings)

    return error


def _random_grouping(pool: list, groups: list, choices: dict) -> list:
    grouped_list = []
    for group in groups:
        if group == 1:
            # Just pick something random from the pool
            pick = random.choice(pool)
            # And remove it from the pool
            pool.remove(pick)
            # That's all in this group
            grouped_list.append(pick)
        else:
            # Count up how many of each choice we have
            counts = [(c, pool.count(c)) for c in choices.keys()]
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
            # If we can only have 1 of these injuries, remove it from our counts
            if sorted_counts[0][1] == 1 or choices[sorted_counts[0][0]]["max"] == 1:
                sorted_counts.remove(sorted_counts[0])
            for _ in range(group-1):
                if len(sorted_counts) == 0:
                    if len(pool) == 1:
                        _log.warning(f"We have 1 {pool[0]} left over.")
                        break
                    else:
                        _log.error("We are all out of choices...")  # We shouldn't ever get here....
                        exit(1)
                # Pick a random choice from our counts
                if len(sorted_counts) == 1:
                    i = 0
                else:
                    i = np.random.randint(0, len(sorted_counts))
                # Add it to this group
                c = sorted_counts[i][0]
                g.append(c)
                try:
                    pool.remove(c)
                except ValueError as e:
                    _log.error(f"Nuts {e}")
                    exit(1)
                sorted_counts[i] = (sorted_counts[i][0], sorted_counts[i][1]-1)
                if sorted_counts[i][1] == 0 or g.count(c) >= choices[c]["max"]:
                    # We have enough of these choices.
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


def _proper_round(num, dec=0):
    num = str(num)[:str(num).index('.') + dec + 2]
    if num[-1] >= '5':
        return float(num[:-2 - (not dec)] + str(int(num[-2 - (not dec)]) + 1))
    return float(num[:-1])


def _bounded_random_normal(mean: float, stdev: float, size: int,
                           lower: float = 1.0, upper: float = 6.0, gen: int = 0):
    if gen == 0:
        random_distribution = np.random.normal(loc=mean, scale=stdev, size=size)
    elif gen == 1:
        random_distribution = norm.rvs(loc=mean, scale=stdev, size=size)
    elif gen == 2:
        a = (lower - mean) / stdev
        b = (upper - mean) / stdev
        truncated_normal = truncnorm(a=a, b=b, loc=mean, scale=stdev)
        random_distribution = truncated_normal.rvs(size=size)
    else:
        _log.error("Unknown mode in _bounded_random_normal")
        exit(1)
    # mean = statistics.mean(random_distribution)

    # Convert real values to whole number floats
    random_rounded_distribution = []
    for v in random_distribution:
        random_rounded_distribution.append(_proper_round(v))
    # rounded_mean = statistics.mean(random_rounded_distribution)

    bound = []
    bounded = 0
    for s in random_rounded_distribution:
        if s < lower:
            s = lower
            bounded += 1
        elif s > upper:
            s = upper
            bounded += 1
        bound.append(s)
    return bound


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
        injuries = _random_grouping(injuries, num_patient_injuries, injury_types)

        polytrauma_patients = [0] * polytrauma["max"]
        if log:
            for i in num_patient_injuries:
                polytrauma_patients[i - 1] += 1
            for i, c in enumerate(polytrauma_patients):
                _log.info(f"  There are {c} patients with {i + 1} injuries")
            _log.info(f"Mean number of injuries per patient: {num_location_injuries / num_patients_injured}")
            _log.info(f"Total number of injuries for all patient: {num_location_injuries}")

        # Check that our tuples don't have more than 2 of any 1 injury
        max_polytrauma_counts = {}  # Curious to see which injuries are doubled in a polytrauma
        for injury in injuries:
            if isinstance(injury, tuple):
                # How many of each type do we have?
                for t in injury_types.keys():
                    num = list(injury).count(t)
                    if t not in max_polytrauma_counts:
                        max_polytrauma_counts[t] = 0
                    if num > max_polytrauma_counts[t]:
                        max_polytrauma_counts[t] = num
                    if num > injury_types[t]["max"]:
                        _log.fatal(f"Polytrauma has more than the maximum specified ({injury_types[t]['max']}) {t}s")
        _log.info(f"Max number in a polytrauma: {max_polytrauma_counts}")

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
