# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import logging
import argparse
import numpy as np
import pandas as pd
from pathlib import Path
import matplotlib.pyplot as plt
import plotly.offline as pyo
import plotly.graph_objects as go
from typing import Dict, List, Tuple

from pulse.cdm.patient import eSex
from pulse.cdm.scenario import SEScenarioExecStatus
from pulse.study.patient_variability.analysis_utils import PatientVariabilityResults, Conditional, Field, PropertyError
from pulse.cdm.io.scenario import serialize_scenario_exec_status_list_from_file


_pulse_logger = logging.getLogger('pulse')


fields = [
    Field.Age_yr,
    Field.Height_cm,
    Field.BodyMassIndex,
    Field.BodyFatFraction,
    Field.HeartRateBaseline_bpm,
    Field.MeanArterialPressureBaseline_mmHg,
    Field.PulsePressureBaseline_mmHg,
    Field.RespirationRateBaseline_bpm
]


minAge_yr = 18
stdAge_yr = 44
maxAge_yr = 65
minMaleHeight_cm = 163
stdMaleHeight_cm = 180.34
maxMaleHeight_cm = 190
minFemaleHeight_cm = 151
stdFemaleHeight_cm = 162.56
maxFemaleHeight_cm = 175.5
minBMI = 16
stdMaleBMI = 23.71
stdFemaleBMI = 22.31
maxBMI = 29.9
minMaleBFF = 0.02
stdMaleBFF = 0.21
maxMaleBFF = 0.25
minFemaleBFF = 0.1
stdFemaleBFF = 0.28
maxFemaleBFF = 0.32
minHR_bpm = 60
stdHR_bpm = 72
maxHR_bpm = 100
minMAP_mmHg = 70
stdMAP_mmHg = 87
maxMAP_mmHg = 100
minPulsePressure_mmHg = 30
stdPulsePressure_mmHg = 40.5
maxPulsePressure_mmHg = 60
minRR_bpm = 8
stdRR_bpm = 12
maxRR_bpm = 20


mapAllowableErrorFraction = 0.1
stdMapMin_mmHg = stdMAP_mmHg - stdMAP_mmHg * mapAllowableErrorFraction
stdMapMax_mmHg = stdMAP_mmHg + stdMAP_mmHg * mapAllowableErrorFraction
pulsePressureAllowableErrorFraction = 0.1
stdPulsePressureMin_mmHg = stdPulsePressure_mmHg - stdPulsePressure_mmHg * pulsePressureAllowableErrorFraction
stdPulsePressureMax_mmHg = stdPulsePressure_mmHg + stdPulsePressure_mmHg * pulsePressureAllowableErrorFraction


standard_values = {
    (eSex.Male, Field.Age_yr): stdAge_yr,
    (eSex.Female, Field.Age_yr): stdAge_yr,
    (eSex.Male, Field.Height_cm): stdMaleHeight_cm,
    (eSex.Female, Field.Height_cm): stdFemaleHeight_cm,
    (eSex.Male, Field.BodyMassIndex): stdMaleBMI,
    (eSex.Female, Field.BodyMassIndex): stdFemaleBMI,
    (eSex.Male, Field.BodyFatFraction): stdMaleBFF,
    (eSex.Female, Field.BodyFatFraction): stdFemaleBFF,
    (eSex.Male, Field.HeartRateBaseline_bpm): stdHR_bpm,
    (eSex.Female, Field.HeartRateBaseline_bpm): stdHR_bpm,
    (eSex.Male, Field.MeanArterialPressureBaseline_mmHg): stdMAP_mmHg,
    (eSex.Female, Field.MeanArterialPressureBaseline_mmHg): stdMAP_mmHg,
    (eSex.Male, Field.PulsePressureBaseline_mmHg): stdPulsePressure_mmHg,
    (eSex.Female, Field.PulsePressureBaseline_mmHg): stdPulsePressure_mmHg,
    (eSex.Male, Field.RespirationRateBaseline_bpm): stdRR_bpm,
    (eSex.Female, Field.RespirationRateBaseline_bpm): stdRR_bpm
}


class PatientVariabilityAnalysis(PatientVariabilityResults):
    __slots__ = ["_systems", "_output_dir"]
    pass_errors = [10.0, 30.0]  # Errors are in %, not fraction!!!

    def __init__(self):
        super().__init__()
        self._systems = list()
        self._output_dir = None

    def process(self, scenario_exec_status_file: Path, validation_results_file: Path, output_directory: Path) -> None:
        _exec_status = list()
        if not scenario_exec_status_file.is_file():
            _pulse_logger.warning(f"Could not locate scenario exec status file: {scenario_exec_status_file}")
        else:
            serialize_scenario_exec_status_list_from_file(scenario_exec_status_file, _exec_status)
        # Output scenario exec summary
        if _exec_status:
            for category, value in SEScenarioExecStatus.summarize_exec_status_list(_exec_status).items():
                _pulse_logger.info(f"{category} Runs: {value}")

        _pulse_logger.info(f"Loading {validation_results_file}...")
        self.open_validation_results(validation_results_file)
        _pulse_logger.info(f"Done!")
        self._output_dir = output_directory
        self._output_dir.mkdir(exist_ok=True, parents=True)

        # Identify number of patients and systems in this dataset
        matches, results = analysis.everything_query()
        _pulse_logger.info(f"All patients: {len(matches)}")
        self._systems = self.combine_categories(results)

        self.create_radar_charts()
        self.create_box_plots()

    def create_box_plots(self) -> None:
        _pulse_logger.info(" --------------------- Create box plots ---------------------")
        for pass_error in self.pass_errors:
            # Standard male
            std_male_matches, results = self.standard_query(eSex.Male)
            _pulse_logger.info(f"Standard male patients: {len(std_male_matches)}")
            standard_male_values = self.calculate_system_pass_rate(results, pass_error, self._systems)

            # Standard female
            std_female_matches, results = self.standard_query(eSex.Female)
            _pulse_logger.info(f"Standard female patients: {len(std_female_matches)}")
            standard_female_values = self.calculate_system_pass_rate(results, pass_error, self._systems)

            for idx, system in enumerate(self._systems):
                plot_data = {}
                for field in fields:
                    _pulse_logger.info(f"Evaluating {system} system with {field}")
                    _, results = self.single_parameter_query(eSex.Male, field)
                    male_values = self.calculate_parameter_pass_rate(results, pass_error, system)
                    _, results = self.single_parameter_query(eSex.Female, field)
                    female_values = self.calculate_parameter_pass_rate(results, pass_error, system)
                    # Clean up the category names for the plot
                    str_field = str(field).replace("Field.", "")
                    str_field = str_field.split("_", 1)[0]
                    str_field = str_field.replace("BodyMassIndex", "BMI")
                    str_field = str_field.replace("BodyFatFraction", "FF")
                    str_field = str_field.replace("HeartRateBaseline", "HR")
                    str_field = str_field.replace("MeanArterialPressureBaseline", "MAP")
                    str_field = str_field.replace("PulsePressureBaseline", "PP")
                    str_field = str_field.replace("RespirationRateBaseline", "RR")
                    # Blood pressures are special
                    if str_field != "MAP" and str_field != "PP":
                        combined_values = male_values + female_values
                        str_field += f" ({len(combined_values)})"
                        plot_data.update({str_field: combined_values})
                        _pulse_logger.info(f"Patients evaluated: {len(combined_values)}")

                # Blood pressures are special
                _, results = self.blood_pressure_query(eSex.Male)
                male_values = self.calculate_parameter_pass_rate(results, pass_error, system)
                _, results = self.blood_pressure_query(eSex.Female)
                female_values = self.calculate_parameter_pass_rate(results, pass_error, system)
                combined_values = male_values + female_values
                str_field = "BP"
                str_field += f" ({len(combined_values)})"
                plot_data.update({str_field: combined_values})
                _pulse_logger.info(f"Patients evaluated: {len(combined_values)}")

                # Add NaNs to make data size the same, so pandas doesn't complain
                data_length = 0
                for key, value in plot_data.items():
                    length = len(value)
                    if length > data_length:
                        data_length = length
                for key, value in plot_data.items():
                    length_diff = data_length - len(value)
                    if length_diff > 0:
                        value = np.append(value, np.repeat(np.nan, length_diff))
                        plot_data.update({key: value})

                df = pd.DataFrame(plot_data)
                df.boxplot(grid=True, rot=0)
                left, right = plt.xlim()
                plt.hlines(standard_male_values[idx], xmin=left, xmax=right, color='blue', linestyles='--')
                plt.hlines(standard_female_values[idx], xmin=left, xmax=right, color='r', linestyles='--')
                plt.ylim([0.0, 1.01])
                plt.title(f"{system} Pass Rate Per Parameter within {pass_error}%")
                plt.tight_layout()
                #plt.show()
                plot_file = str(self._output_dir / f"Box_Plot_{pass_error}%_{system}.png")
                _pulse_logger.info("Creating box plot: " + plot_file)
                plt.savefig(plot_file)
                plt.clf()

    def create_radar_charts(self) -> None:
        _pulse_logger.info(" --------------------- Create radar chart ---------------------")
        for pass_error in self.pass_errors:
            # Standard male
            std_male_matches, results = self.standard_query(eSex.Male)
            _pulse_logger.info(f"Standard male patients: {len(std_male_matches)}")
            radar_standard_male_values = self.calculate_system_pass_rate(results, pass_error, self._systems)

            #Standard female
            std_female_matches, results = self.standard_query(eSex.Female)
            _pulse_logger.info(f"Standard female patients: {len(std_female_matches)}")
            radar_standard_female_values = self.calculate_system_pass_rate(results, pass_error, self._systems)

            # Nonstandard male query
            nonstd_male_matches, results = self.non_standard_query(eSex.Male)
            _pulse_logger.info(f"Nonstandard male patients: {len(nonstd_male_matches)}")
            radar_nonstandard_male_values = self.calculate_system_pass_rate(results, pass_error, self._systems)

            # Nonstandard female query
            nonstd_female_matches, results = self.non_standard_query(eSex.Female)
            _pulse_logger.info(f"Nonstandard female patients: {len(nonstd_female_matches)}")
            radar_nonstandard_female_values = self.calculate_system_pass_rate(results, pass_error, self._systems)

            # Generate radar chart
            radar_categories = [*self._systems, self._systems[0]]

            radar_standard_male_values = [*radar_standard_male_values, radar_standard_male_values[0]]
            radar_standard_female_values = [*radar_standard_female_values, radar_standard_female_values[0]]
            radar_nonstandard_male_values = [*radar_nonstandard_male_values, radar_nonstandard_male_values[0]]
            radar_nonstandard_female_values = [*radar_nonstandard_female_values, radar_nonstandard_female_values[0]]

            fig = go.Figure(
                data=[
                    go.Scatterpolar(r=radar_standard_male_values, theta=radar_categories, name=f'Standard Male ({len(std_male_matches)} total)'),
                    go.Scatterpolar(r=radar_standard_female_values, theta=radar_categories, name=f'Standard Female ({len(std_female_matches)} total)'),
                    go.Scatterpolar(r=radar_nonstandard_male_values, theta=radar_categories, name=f'Cumulative Nonstandard Male ({len(nonstd_male_matches)} total)'),
                    go.Scatterpolar(r=radar_nonstandard_female_values, theta=radar_categories, name=f'Cumulative Nonstandard Female ({len(nonstd_female_matches)} total)')
                ],
                layout=go.Layout(
                    title=go.layout.Title(text=f"Physiology System Pass Rate within {pass_error}%"),
                    polar={'radialaxis': {'visible': True}},
                    showlegend=True
                )
            )
            plot_file = str(self._output_dir / f"RadarChart_{pass_error}%_.html")
            _pulse_logger.info("Creating radar chart: " + plot_file)
            pyo.offline.plot(fig, filename=plot_file)

    def analyze_property_error(self, property_error: PropertyError) -> None:
        # We can dynamically add members to our property_error object
        property_error.average_error = 0
        for error in property_error.errors:
            property_error.average_error = property_error.average_error + error
        property_error.average_error = property_error.average_error / len(property_error.errors)

    def standard_query(self, sex: eSex) -> Tuple[List[str], Dict[str, PropertyError]]:
        _pulse_logger.debug(f"Standard {sex.name} query")
        query = Conditional()
        query.sex(sex)
        for field in fields:
            query.add_condition(field, '==', standard_values[sex, field])
        return self.conditional_filter([query])

    def non_standard_query(self, sex: eSex) -> Tuple[List[str], Dict[str, PropertyError]]:
        _pulse_logger.debug(f"Nonstandard {sex.name} query")
        query = Conditional()
        query.sex(sex)
        queryOr = Conditional('OR')
        for field in fields:
            queryOr.add_condition(field, '!=', standard_values[sex, field])
        query.add_conditional(queryOr)
        return self.conditional_filter([query])

    def everything_query(self) -> Tuple[List[str], Dict[str, PropertyError]]:
        _pulse_logger.info("Everything query")
        query = Conditional()
        return self.conditional_filter([query])

    def single_parameter_query(self, sex: eSex, field:Field) -> Tuple[List[str], Dict[str, PropertyError]]:
        _pulse_logger.debug(f"{sex.name} {field.name} query")
        query = Conditional()
        query.sex(sex)
        for current_field in fields:
            if field != current_field:
                query.add_condition(current_field, '==', standard_values[sex, current_field])
        return self.conditional_filter([query])

    def blood_pressure_query(self, sex: eSex) -> Tuple[List[str], Dict[str, PropertyError]]:
        _pulse_logger.debug(f"{sex.name} Blood Pressure query")
        query = Conditional()
        query.sex(sex)
        for current_field in fields:
            if Field.MeanArterialPressureBaseline_mmHg != current_field and Field.PulsePressureBaseline_mmHg != current_field:
                query.add_condition(current_field, '==', standard_values[sex, current_field])
        return self.conditional_filter([query])

    def calculate_system_pass_rate(self, results: Dict[str, PropertyError], pass_error: float, categories: List[str]) -> List[float]:
        pass_rates = [0] * len(categories)
        total_num_pass = [0] * len(categories)
        total_num_fail = [0] * len(categories)
        for system, properties in results.items():
            _pulse_logger.info(f"Examining {system} system")
            num_pass = 0
            num_fail = 0
            for property, property_error in properties.items():
                for error in property_error.errors:
                    if abs(error) > pass_error:
                        num_fail = num_fail + 1
                    else:
                        num_pass = num_pass + 1
            for idx, category in enumerate(categories):
                if category in system:
                    total_num_pass[idx] = total_num_pass[idx] + num_pass
                    total_num_fail[idx] = total_num_fail[idx] + num_fail

        for idx, category in enumerate(categories):
            pass_rate = 0
            num_pass = total_num_pass[idx]
            num_fail = total_num_fail[idx]
            if num_pass + num_fail != 0:
                pass_rate = num_pass / (num_pass + num_fail)
            pass_rates[idx] = pass_rate
        return pass_rates

    def calculate_parameter_pass_rate(self, results: Dict[str, PropertyError], pass_error: float, category: str) -> List[float]:
        per_patient_num_pass = list()
        per_patient_num_fail = list()
        for system, properties in results.items():
            if not category in system:
                continue
            _pulse_logger.info(f"Examining {system} system ")
            for property, property_error in properties.items():
                for idx, error in enumerate(property_error.errors):
                    num_pass = 0
                    num_fail = 0
                    if abs(error) > pass_error:
                        num_fail = 1
                    else:
                        num_pass = 1
                    if len(per_patient_num_pass) < idx + 1:
                        per_patient_num_pass.append(num_pass)
                        per_patient_num_fail.append(num_fail)
                    else:
                        per_patient_num_pass[idx] = per_patient_num_pass[idx] + num_pass
                        per_patient_num_fail[idx] = per_patient_num_fail[idx] + num_fail
        per_patient_pass_rates = list()
        for idx, patient in enumerate(per_patient_num_pass):
            pass_rate = per_patient_num_pass[idx] / (per_patient_num_pass[idx] + per_patient_num_fail[idx])
            per_patient_pass_rates.append(pass_rate)
        return per_patient_pass_rates

    def combine_categories(self, results: Dict[str, PropertyError]) -> List[str]:
        start_categories = list(results.keys())
        end_categories = list()
        for start_category in start_categories:
            new_category = True
            for idx, end_category in enumerate(end_categories):
                if start_category in end_category:
                    # Replace end_category with start_category
                    end_categories[idx] = start_category
                    new_category = False
                if end_category in start_category:
                    # Don't do anything
                    new_category = False
                    break
            if new_category:
                # New, so add
                end_categories.append(start_category)
        final_categories = list()
        for end_category in end_categories:
            duplicate = False
            for finalCategory in final_categories:
                if end_category == finalCategory:
                    duplicate = True
                    break
            if not duplicate:
                final_categories.append(end_category)

        return final_categories


if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

    mode = "validation"
    # Option to validate/generate tables for either the baseline/verification csv or the test_results
    parser = argparse.ArgumentParser(description="Process a validation targets file to generate plots")
    parser.add_argument(
        "-s", "--scenario_exec",
        type=Path,
        default=f"./test_results/patient_variability/{mode}/scenarios/Validation.json",
        help="A scenario exec status list json file to analyze and create plots for."
    )
    parser.add_argument(
        "-v", "--validation_results",
        type=Path,
        default=f"./test_results/patient_variability/{mode}/scenarios/ValidationTargets.json",
        help="A validation targets list json file to analyze and create plots for."
    )
    parser.add_argument(
        "-o", "--output-directory",
        type=Path,
        default=f"./test_results/patient_variability/{mode}/analysis",
        help="Where to store validation analysis artifacts."
    )
    opts = parser.parse_args()

    analysis = PatientVariabilityAnalysis()
    analysis.process(scenario_exec_status_file=opts.scenario_exec,
                     validation_results_file=opts.validation_results,
                     output_directory=opts.output_directory)
