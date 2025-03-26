# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import logging
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import random as rand

from pathlib import Path

_log = logging.getLogger("pulse")


def main():
    # Total population size
    population_size = 3000
    stats_file_name = Path("./ArmyDemographicStats.csv")
    synthetic_data_file = Path(f"./SyntheticArmyInjuryPopulation_{population_size}samples.csv")
    synthetic_data_generation(population_size, stats_file_name, synthetic_data_file)
    validate_synthetic_population_data(synthetic_data_file, stats_file_name)
    validate_synthetic_injury_data(synthetic_data_file)


def synthetic_data_generation(population_size: int, statistics_file: Path, synthetic_data_file: Path):
    _log.info(f"Generate data with {population_size} samples")

    statistics_data = pd.read_csv(statistics_file, index_col="Parameters")
    
    # Gender
    gender = ["female", "male"]
    female_frequency = statistics_data.loc["GenderSplit", "Gender"]
    gender_split = [female_frequency, 100-female_frequency]
    gender_list = rand.choices(gender, weights=gender_split, k=population_size)
    population_data = pd.DataFrame({"Gender": gender_list})

    # Height and BMI
    number_gender = population_data.value_counts("Gender")
    number_female = number_gender["female"]

    height_female_list = np.random.normal(loc=statistics_data.loc["mean", "FemaleHeight"],
                                          scale=statistics_data.loc["std", "FemaleHeight"],
                                          size=number_female)
    height_male_list = np.random.normal(loc=statistics_data.loc["mean", "MaleHeight"],
                                        scale=statistics_data.loc["std", "MaleHeight"],
                                        size=population_size - number_female)

    bmi_female_list = np.random.normal(loc=statistics_data.loc["mean", "FemaleBMI"],
                                       scale=statistics_data.loc["std", "FemaleBMI"],
                                       size=number_female)
    bmi_male_list = np.random.normal(loc=statistics_data.loc["mean", "MaleBMI"],
                                     scale=statistics_data.loc["std", "MaleBMI"],
                                     size=population_size - number_female)

    female_count = 0
    male_count = 0
    height_list = []
    bmi_list = []
    for index, value in population_data["Gender"].items():
        if value == "female":
            height_list.append(height_female_list[female_count])
            bmi_list.append(bmi_female_list[female_count])
            female_count = female_count+1
        else:
            height_list.append(height_male_list[male_count])
            bmi_list.append(bmi_male_list[male_count])
            male_count = male_count+1

    # Heart Rate
    hr_list = np.random.normal(loc=statistics_data.loc["mean", "HeartRate"],
                               scale=statistics_data.loc["std", "HeartRate"],
                               size=population_size)

    # Alternative Age Distribution
    age_groups = ["Under25", "26To30", "31To35", "36To40", "41Plus"]
    age_weights = [41.6, 22.4, 15.3, 11.3, 9.4]

    # Create bins of ages
    age_bin_list = rand.choices(age_groups, weights=age_weights, k=population_size)
    age_bin_data = pd.DataFrame({"AgeBins": age_bin_list})

    # Assume normal distribution for each bin
    age_bin_number = age_bin_data.value_counts("AgeBins")

    under25_list = []
    for i in range(age_bin_number["Under25"]):
        under25_list.append(np.random.randint(18, 25))

    bin26_to30_list = []
    for i in range(age_bin_number["26To30"]):
        bin26_to30_list.append(np.random.randint(26, 30))

    bin31_to35_list = []
    for i in range(age_bin_number["31To35"]):
        bin31_to35_list.append(np.random.randint(31, 35))

    bin36_to40_list = []
    for i in range(age_bin_number["36To40"]):
        bin36_to40_list.append(np.random.randint(36, 40))

    over41_list = []
    for i in range(age_bin_number["41Plus"]):
        over41_list.append(np.random.randint(41, 55))
    
    age_list = []
    under25_count = 0
    bin26_to30_count = 0
    bin31_to35_count = 0
    bin36_to40_count = 0
    over41_count = 0
    for index, value in age_bin_data["AgeBins"].items():
        if value == "Under25":
            age_list.append(under25_list[under25_count])
            under25_count = under25_count + 1
        elif value == "26To30":
            age_list.append(bin26_to30_list[bin26_to30_count])
            bin26_to30_count = bin26_to30_count + 1
        elif value == "31To35":
            age_list.append(bin31_to35_list[bin31_to35_count])
            bin31_to35_count = bin31_to35_count + 1
        elif value == "36To40":
            age_list.append(bin36_to40_list[bin36_to40_count])
            bin36_to40_count = bin36_to40_count + 1
        elif value == "41Plus":
            age_list.append(over41_list[over41_count])
            over41_count = over41_count + 1
        else:
            print("Unknown Age Range: " + value)

    population_data["Age"] = age_list
    population_data["Height"] = height_list
    population_data["BMI"] = bmi_list
    population_data["Heart Rate"] = hr_list

    population_data.head()
    population_data.describe()

    # Apply Injuries
    injury_data = generate_synthetic_injuries(population_size)

    population_injury_data = pd.concat([population_data, injury_data], axis=1)
   
    population_injury_data.to_csv(synthetic_data_file, index=False)


def validate_synthetic_population_data(synthetic_data_file: Path, stats_file_name: Path):
    _log.info(f"Validating data in {synthetic_data_file}")
    
    # Load synthetic data
    population_data = pd.read_csv(synthetic_data_file)

    # Load statistics file
    statistics_data = pd.read_csv(stats_file_name, index_col="Parameters")

    # Gender Split
    number_gender = population_data.value_counts("Gender")
    number_female = number_gender["female"]
    number_male = number_gender["male"]
    percent_female = 100 * number_female / (number_female+number_male)
    actual_percent_female = statistics_data.loc["GenderSplit", "Gender"]
    gender_split_error = percent_female - actual_percent_female

    # Need a list of means, stds, errors
    means = []
    actual_means = []
    stds = []
    actual_stds = []
    errors = []

    # Height and BMI
    female_height_list = []
    male_height_list = []
    female_bmi_list = []
    male_bmi_list = []
    for index, value in population_data["Gender"].items():
        if value == "female":
            female_height_list.append(population_data.loc[index, "Height"])
            female_bmi_list.append(population_data.loc[index, "BMI"])
        else:
            male_height_list.append(population_data.loc[index, "Height"])
            male_bmi_list.append(population_data.loc[index, "BMI"])

    female_height = np.array(female_height_list)
    male_height = np.array(male_height_list)
    female_bmi = np.array(female_bmi_list)
    male_bmi = np.array(male_bmi_list)
    
    means.append(np.mean(female_height))
    stds.append(np.std(female_height))
    means.append(np.mean(female_bmi))
    stds.append(np.std(female_bmi))

    means.append(np.mean(male_height))
    stds.append(np.std(male_height))
    means.append(np.mean(male_bmi))
    stds.append(np.std(male_bmi))

    actual_means.append(statistics_data.loc["mean", "FemaleHeight"])
    actual_stds.append(statistics_data.loc["std", "FemaleHeight"])
    actual_means.append(statistics_data.loc["mean", "FemaleBMI"])
    actual_stds.append(statistics_data.loc["std", "FemaleBMI"])
    
    actual_means.append(statistics_data.loc["mean", "MaleHeight"])
    actual_stds.append(statistics_data.loc["std", "MaleHeight"])
    actual_means.append(statistics_data.loc["mean", "MaleBMI"])
    actual_stds.append(statistics_data.loc["std", "MaleBMI"])

    # Heart Rate
    means.append(population_data["Heart Rate"].mean())
    stds.append(population_data["Heart Rate"].std())

    actual_means.append(statistics_data.loc["mean", "HeartRate"])
    actual_stds.append(statistics_data.loc["std", "HeartRate"])

    # Calculate Error
    mean_error = []
    std_error = []
    for i in range(len(means)):
        mean_error.append(means[i] - actual_means[i])
        std_error.append(stds[i] - actual_stds[i])

    # Age
    age_list = population_data["Age"]
    age_bins = [18, 25, 31, 35, 41, 55]
    counts, bins = np.histogram(age_list, bins=age_bins)
    normalized_counts = counts / counts.sum()

    fig, axes = plt.subplots(1, 2, figsize=(10, 5)) 
    #axes[0].hist(age_list, bins=age_bins, density=True, color="skyblue", edgecolor="black")
    axes[0].hist(age_bins[:-1], age_bins, weights=normalized_counts, color="skyblue", edgecolor="black")
    axes[0].set_ylim(0, 0.45)
    axes[0].set_title("Normalized Synthetic Age Data")
    axes[0].set_xlabel("Age")
    axes[0].set_ylabel("Normalized Frequency")

    actual_age_counts = [191975, 103628, 70783, 52055, 43215]
    total_population = sum(actual_age_counts)
    normalized_age_counts = [x / total_population for x in actual_age_counts]
    
    axes[1].hist(age_bins[:-1], age_bins, weights=normalized_age_counts, color="green", edgecolor="black")
    axes[1].set_ylim(0, 0.45)
    axes[1].set_title("Normalized Actual Age Data")
    axes[1].set_xlabel("Age")
    axes[1].set_ylabel("Normalized Frequency")
    
    plt.savefig(synthetic_data_file.stem + "_AgeHistogram.jpg", format="jpeg")  # Saves as JPEG

    # Gender Comparison Table
    gender_split_table = pd.DataFrame({"Synthetic Gender Split": [percent_female],
                                       "Actual Gender Split": [actual_percent_female],
                                       "Error": gender_split_error})
    print(gender_split_table)
    gender_split_table.to_csv(synthetic_data_file.stem + "_GenderSplitTable.csv", index=False)
    gender_split_table.to_html(synthetic_data_file.stem + "_GenderSplitTable.html", index=False)

    # Other parameter comparison
    descriptors = ["Female Height", "Female BMI", "Male Height", "Male BMI", "Heart Rate"]
    statistics_validation_table = pd.DataFrame({"Demographic Descriptor": descriptors,
                                                "Synthetic Mean": means, "Actual Mean": actual_means,
                                                "Mean Error": mean_error,
                                                "Synthetic Standard Deviation": stds,
                                                "Actual Standard Deviation": actual_stds,
                                                "Std Error": std_error})
    print(statistics_validation_table)
    statistics_validation_table.to_csv(synthetic_data_file.stem + "_SyntheticPopulationValidationTable.csv", index=False)
    statistics_validation_table.to_html(synthetic_data_file.stem + "_SyntheticPopulationValidationTable.html", index=False)


def generate_synthetic_injuries(population_size: int):

    # Number and Location of Injuries
    injury_location = ["HeadAndNeck", "Thorax", "Abdomen", "Extremity"]
    injury_location_weights = [36.2, 8.6, 6.9, 49.4]
    injury_location_count_mean = 1.011  # going to assume 1 location

    injury_location_list = rand.choices(injury_location, weights=injury_location_weights, k=population_size)

    injury_data = pd.DataFrame({"Injury Location": injury_location_list})

    # Type and Number of Injuries in a location
    injury_head_neck_type = ["TBI", "Airway Obstruction", "Superficial Injuries"]
    injury_head_neck_weights = [22, 18, 60]
    injury_head_neck_number_mean = 1.0
    injury_thorax_type = ["Pneumothorax", "Pulmonary Contusion", "Fractures", "Hemothorax", "Hemorrhage", "Spinal Injury"]
    injury_thorax_weights = [51.8, 50.2, 51.2, 30, 34.6, 14.6]
    injury_thorax_number_mean = 2.3
    injury_abdomen_type = ["Hemorrhage", "Lacerations/Contusions"]
    injury_abdomen_weights = [34.6, 65.4]
    injury_abdomen_number_mean = 1.0
    injury_extremity_type = ["Hemorrhage", "Fractures/Dislocations", "Contusions/Sprains/Strains", "Burns/Nerves"]
    injury_extremity_weight = [52, 22, 20, 5]
    injury_extremity_number_mean = 1.0

    # count types of injuries for the loop below
    number_injury_locations = injury_data.value_counts("Injury Location")

    head_neck_injury_list = rand.choices(injury_head_neck_type,
                                         weights=injury_head_neck_weights,
                                         k=number_injury_locations["HeadAndNeck"])
    thorax_injury_list = rand.choices(injury_thorax_type,
                                      weights=injury_thorax_weights,
                                      k=number_injury_locations["Thorax"])
    abdomen_injury_list = rand.choices(injury_abdomen_type,
                                       weights=injury_abdomen_weights,
                                       k=number_injury_locations["Abdomen"])
    extremity_injury_list = rand.choices(injury_extremity_type,
                                         weights=injury_extremity_weight,
                                         k=number_injury_locations["Extremity"])

    injury_type_list = []
    head_neck_count = 0
    thorax_count = 0
    abdomen_count = 0
    extremity_count = 0
    for i in range(len(injury_location_list)):
        loc = injury_location_list[i]
        if loc == "HeadAndNeck":
            injury_type_list.append(head_neck_injury_list[head_neck_count])
            head_neck_count = head_neck_count + 1
        elif loc == "Thorax":
            injury_type_list.append(thorax_injury_list[thorax_count])
            thorax_count = thorax_count + 1
        elif loc == "Abdomen":
            number_injuries = 1
            injury_type_list.append(abdomen_injury_list[abdomen_count])
            abdomen_count = abdomen_count + 1
        elif loc == "Extremity":
            number_injuries = 1
            injury_type_list.append(extremity_injury_list[extremity_count])
            extremity_count = extremity_count + 1
        else:
            print("Unknown Injury Location: " + loc)

    injury_data["Injury Type"] = injury_type_list
    
    # Apply Severities
    number_injury_types = injury_data.value_counts("Injury Type")
    
    mean_tbi_severity = 3.5
    std_severity = 0.25
    mean_airway_severity = 4.0
    mean_thorax_abdomen_severity = 2.85
    severity_extremity_value = [1, 2.5, 3.5, 4.5]
    severity_extremity_weight = [56, 23, 17, 7]
    tbi_severity_list = np.random.normal(loc=mean_tbi_severity,
                                         scale=std_severity,
                                         size=number_injury_types["TBI"])
    airway_severity_list = np.random.normal(loc=mean_airway_severity,
                                            scale=std_severity,
                                            size=number_injury_types["Airway Obstruction"])
    thorax_and_abdomen_severity_list = np.random.normal(loc=mean_thorax_abdomen_severity,
                                                        scale=std_severity,
                                                        size=number_injury_locations["Thorax"]+number_injury_locations["Abdomen"])
    extremity_severity_list = rand.choices(severity_extremity_value,
                                           severity_extremity_weight,
                                           k=number_injury_locations["Extremity"])

    injury_severity_list = []
    tbi_count = 0
    airway_count = 0
    thorax_abdomen_count = 0
    extremity_count = 0
    for i in range(len(injury_location_list)):
        loc = injury_location_list[i]
        if loc == "HeadAndNeck":
            loc2 = injury_type_list[i]
            if loc2 == "TBI":
                injury_severity_list.append(tbi_severity_list[tbi_count])
                tbi_count = tbi_count + 1
            elif loc2 == "Airway Obstruction":
                injury_severity_list.append(airway_severity_list[airway_count])
                airway_count = airway_count + 1
            elif loc2 == "Superficial Injuries":
                injury_severity_list.append(1.0)
            else:
                print("Unknown Injury Type: " + loc2)
        elif loc == "Abdomen" or loc == "Thorax":
            injury_severity_list.append(thorax_and_abdomen_severity_list[thorax_abdomen_count])
            thorax_abdomen_count = thorax_abdomen_count + 1
        elif loc == "Extremity":
            injury_severity_list.append(extremity_severity_list[extremity_count])
            extremity_count = extremity_count + 1
        else :
            print("Unknown Injury Location: " + loc)

    injury_data["Injury Severity"] =  injury_severity_list

    return injury_data


def validate_synthetic_injury_data(synthetic_data_file):
    
    # Load synthetic data
    population_data = pd.read_csv(synthetic_data_file)

    actual_injury_location_percents = [36.2, 8.6, 6.9, 49.4]

    injury_head_neck_type = ["TBI", "Airway Obstruction", "Superficial Injuries"]
    injury_thorax_type = ["Pneumothorax", "Pulmonary Contusion", "Fractures", "Hemothorax", "Hemorrhage", "Spinal Injury"]
    injury_abdomen_type = ["Hemorrhage", "Lacerations/Contusions"]
    injury_extremity_type = ["Hemorrhage", "Fractures/Dislocations", "Contusions/Sprains/Strains", "Burns/Nerves"]
    actual_injury_head_neck_percents = [22, 18, 60]
    actual_injury_thorax_percents = [51.8, 50.2, 51.2, 30, 34.6, 14.6]
    actual_injury_abdomen_percents = [34.6, 65.4]
    actual_injury_extremity_percents = [52, 22, 20, 5]
    actual_severity_mean = [2.69, 2.85, 2.85, 2.05]

    # Location of Injury
    count_injury_locations = population_data.value_counts("Injury Location")
    injury_location_counts = [count_injury_locations["HeadAndNeck"],
                              count_injury_locations["Abdomen"],
                              count_injury_locations["Thorax"],
                              count_injury_locations["Extremity"]]
    
    number_injuries = len(population_data)
    severity = population_data["Injury Severity"]

    percent_injury_locations = []
    error_percent_injury_locations = []
    for i in range(len(injury_location_counts)):
        percent_injury_locations.append(100*injury_location_counts[i]/number_injuries)
        error_percent_injury_locations.append(percent_injury_locations[i] - actual_injury_location_percents[i])

    count_injury_types = population_data.value_counts("Injury Type")
    
    abdomen_hemorrhage = 0
    thorax_hemorrhage = 0
    extremity_hemorrhage = 0
    head_neck_severity = 0
    thorax_severity = 0
    abdomen_severity = 0
    extremity_severity = 0
    injury_type = population_data["Injury Type"]
    injury_location = population_data["Injury Location"]
    for i in range(len(injury_type)):
        i_type = injury_type[i]
        i_loc = injury_location[i]
        if i_loc == "Abdomen":
            abdomen_severity = abdomen_severity + severity[i]
            if i_type == "Hemorrhage":
                abdomen_hemorrhage = abdomen_hemorrhage + 1
        elif i_loc == "Thorax":
            thorax_severity = thorax_severity + severity[i]
            if i_type == "Hemorrhage":
                thorax_hemorrhage = thorax_hemorrhage + 1
        elif i_loc == "Extremity":
            extremity_severity = extremity_severity + severity[i]
            if i_type == "Hemorrhage":
                extremity_hemorrhage = extremity_hemorrhage + 1
        elif i_loc == "HeadAndNeck":
            head_neck_severity = head_neck_severity + severity[i]
        else:
            print("Unknown injury location " + i_loc)

    injury_head_neck_type_counts = [count_injury_types["TBI"],
                                    count_injury_types["Airway Obstruction"],
                                    count_injury_types["Superficial Injuries"]]
    head_neck_injury_total = sum(injury_head_neck_type_counts)
    injury_abdomen_type_counts = [abdomen_hemorrhage,
                                  count_injury_types["Lacerations/Contusions"]]
    abdomen_injury_total = sum(injury_abdomen_type_counts)
    injury_thorax_type_counts = [count_injury_types["Pneumothorax"],
                                 count_injury_types["Pulmonary Contusion"],
                                 count_injury_types["Fractures"],
                                 count_injury_types["Hemothorax"],
                                 thorax_hemorrhage,
                                 count_injury_types["Spinal Injury"]]
    thorax_injury_total = sum(injury_thorax_type_counts)
    injury_extremity_type_counts = [extremity_hemorrhage,
                                    count_injury_types["Fractures/Dislocations"],
                                    count_injury_types["Contusions/Sprains/Strains"],
                                    count_injury_types["Burns/Nerves"]]
    extremity_injury_total = sum(injury_extremity_type_counts)

    severity_mean = [head_neck_severity/head_neck_injury_total,
                     thorax_severity/thorax_injury_total,
                     abdomen_severity/abdomen_injury_total,
                     extremity_severity/extremity_injury_total]
    severity_mean_error = []
    for i in range(len(severity_mean)):
        severity_mean_error.append(severity_mean[i] - actual_severity_mean[i])

    percent_injuries = []
    error_percent_injuries = []
    actual_percent_injuries = []
    # start with head neck injuries
    percent_injuries.append(percent_injury_locations[0])
    error_percent_injuries.append(error_percent_injury_locations[0])
    actual_percent_injuries.append(actual_injury_location_percents[0])
    for i in range(len(injury_head_neck_type_counts)):
        val = 100*injury_head_neck_type_counts[i]/head_neck_injury_total
        percent_injuries.append(val)
        error_percent_injuries.append(val - actual_injury_head_neck_percents[i])
        actual_percent_injuries.append(actual_injury_head_neck_percents[i])

    # Add Thorax Injuries
    percent_injuries.append(percent_injury_locations[1])
    error_percent_injuries.append(error_percent_injury_locations[1])
    actual_percent_injuries.append(actual_injury_location_percents[1])
    for i in range(len(injury_thorax_type_counts)):
        val = 100*injury_thorax_type_counts[i]/thorax_injury_total
        percent_injuries.append(val)
        error_percent_injuries.append(val - actual_injury_thorax_percents[i])
        actual_percent_injuries.append(actual_injury_thorax_percents[i])

    # Add Abdomen Injuries
    percent_injuries.append(percent_injury_locations[2])
    error_percent_injuries.append(error_percent_injury_locations[2])
    actual_percent_injuries.append(actual_injury_location_percents[2])
    for i in range(len(injury_abdomen_type_counts)):
        val = 100*injury_abdomen_type_counts[i]/abdomen_injury_total
        percent_injuries.append(val)
        error_percent_injuries.append(val - actual_injury_abdomen_percents[i])
        actual_percent_injuries.append(actual_injury_abdomen_percents[i])

    # Add Extremity Injuries
    percent_injuries.append(percent_injury_locations[3])
    error_percent_injuries.append(error_percent_injury_locations[3])
    actual_percent_injuries.append(actual_injury_location_percents[3])
    for i in range(len(injury_extremity_type_counts)):
        val = 100*injury_extremity_type_counts[i]/extremity_injury_total
        percent_injuries.append(val)
        error_percent_injuries.append(val - actual_injury_extremity_percents[i])
        actual_percent_injuries.append(actual_injury_extremity_percents[i])

    # Create table for validation
    location_descriptors = ["Head and Neck", "", "", "", "Thorax", "", "", "", "", "", "", "Abdomen", "", "", "Extremity", "", "", "", ""]
    severity_mean_list = [severity_mean[0], "", "", "", severity_mean[1], "", "", "", "", "", "", severity_mean[2], "", "", severity_mean[3], "", "", "", ""]
    actual_severity_mean_list = [actual_severity_mean[0], "", "", "", actual_severity_mean[1], "", "", "", "", "", "", actual_severity_mean[2], "", "", actual_severity_mean[3], "", "", "", ""]
    severity_error_list = [severity_mean_error[0], "", "", "", severity_mean_error[1], "", "", "", "", "", "", severity_mean_error[2], "", "", severity_mean_error[3], "", "", "", ""]
    type_descriptors = ["", "TBI", "Airway Obstruction", "Superficial Injuries", "", "Pneumothorax", "Pulmonary Contusion", "Fractures", "Hemothorax", "Hemorrhage", "Spinal Injury","", "Hemorrhage", "Lacerations/Contusions", "", "Hemorrhage", "Fractures/Dislocations", "Contusions/Sprains/Strains", "Burns/Nerves"]
    injury_location_validation_table = pd.DataFrame({"Injury Locations": location_descriptors, "Mean Injury Severity": severity_mean_list, "Actual Severity Mean": actual_severity_mean_list, "Severity Error": severity_error_list,"Injury Types": type_descriptors, "Synthetic Injury Distribution (%)": percent_injuries, "Actual Injury Distribution (%)": actual_percent_injuries, "Distribution Error": error_percent_injuries})
    print(injury_location_validation_table)
    injury_location_validation_table.to_csv(synthetic_data_file.stem + "_InjuryValidationTable.csv", index=False)
    injury_location_validation_table.to_html(synthetic_data_file.stem + "_InjuryValidationTable.html", index=False)


if __name__ == "__main__":
    main()

