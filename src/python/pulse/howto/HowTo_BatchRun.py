# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import logging

from pathlib import Path
from typing import List

from pulse.cdm.engine import SEAdvanceTime, SEDataRequest, SESerializeState, eSwitch
from pulse.cdm.patient import eSex
from pulse.cdm.patient_actions import SEHemorrhage, eHemorrhage_Compartment
from pulse.cdm.plots import SEPlotSource, SESeries, SEMultiHeaderSeriesPlotter
from pulse.cdm.scenario import SEScenario, SEScenarioExecStatus
from pulse.cdm.scalars import FrequencyUnit, LengthUnit, MassUnit, PressureUnit, TimeUnit, VolumeUnit, VolumePerTimeUnit
from pulse.cdm.io.scenario import serialize_scenario_to_file, \
                                  serialize_scenario_exec_status_list_to_file, \
                                  serialize_scenario_exec_status_list_from_file
from pulse.cdm.utils.csv_utils import concat_csv_into_df
from pulse.cdm.utils.plotter import multi_header_series_plotter

from pulse.engine.PulseScenarioExec import PulseScenarioExec

_log = logging.getLogger("pulse")


def main():
    # This HowTo shows how to chain scenario executions together for a purpose
    # If you have a set of scenarios you can run from start to finish
    # You can just create those complete scenarios and execute them
    # You can see how to process results in the HowTo_ProcessResults.py
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

    executor = PulseScenarioExec()
    # Location of files associated with batch running
    results_dir = Path("./test_results/howto/HowToBatchRun.py")
    # A batch of runs consists of a list of scenarios
    # Each SEScenarioExecStatus tracks a scenario, where it is, if it ran, errors, etc.
    patient_creation_scenarios: List[SEScenarioExecStatus] = []

    # Let's create a set of scenarios that create initial patient states
    patient_scenarios_dir = results_dir / "patient/scenarios"
    patient_states_dir = results_dir / "patient/states"
    patient_scenarios_outputs_dir = results_dir / "patient/outputs"
    patient_scenarios_dir.mkdir(parents=True, exist_ok=True)
    patient_states_dir.mkdir(parents=True, exist_ok=True)
    patient_scenarios_outputs_dir.mkdir(parents=True, exist_ok=True)
    # This tracks the status of the execution of these scenarios
    patient_states_exec_status_filename = results_dir / "patient_states_exec_status.json"

    # Scenarios will not be rerun if they are marked as complete in this json file
    # You will need to delete this file if you want to rerun this file (set of scenarios)
    if not patient_states_exec_status_filename.exists():
        for i in range(1):  # Just create a single patient, but loop more if you want more
            s = SEScenario()
            s.set_name(f"CreatePatient{i}")  # Result csv/log file will use this as its filename
            s.set_description("")
            p = s.get_patient_configuration().get_patient()
            p.set_sex(eSex.Male)
            p.set_name(f"Patient{i}")
            p.get_age().set_value(22, TimeUnit.yr)
            p.get_height().set_value(72, LengthUnit.inch)
            p.get_weight().set_value(180, MassUnit.lb)
            p.get_heart_rate_baseline().set_value(72, FrequencyUnit.Per_min)
            state = SESerializeState()
            state.set_filename(str(patient_states_dir / f"patient_{i}.json"))
            s.get_actions().append(state)
            f = f"{patient_scenarios_dir}/patient_{i}.json"
            serialize_scenario_to_file(s, f)
            # Add this scenario to our exec status
            e = SEScenarioExecStatus()
            e.set_scenario_filename(f)
            patient_creation_scenarios.append(e)
        # Write out the exec status so we can run it
        serialize_scenario_exec_status_list_to_file(patient_creation_scenarios,
                                                    str(patient_states_exec_status_filename))
    # Now run those scenarios (this will be quick if its already been run)
    executor.set_log_to_console(eSwitch.Off)  # Output can get pretty busy...
    executor.set_output_root_directory(str(patient_scenarios_outputs_dir))
    executor.set_scenario_exec_list_filename(str(patient_states_exec_status_filename))
    if not executor.execute_scenario():
        # You can view the patient_states_exec to see what happened
        _log.error(f"Problem running {patient_states_exec_status_filename}")
        return
    # We can read in the status if we need to
    patient_states_exec_status: List[SEScenarioExecStatus] = []
    serialize_scenario_exec_status_list_from_file(str(patient_states_exec_status_filename), patient_states_exec_status)

    # Create a shared set of data requests we want each scenario to use
    data_requests = [
        SEDataRequest.create_physiology_request("HeartRate", unit=FrequencyUnit.Per_min),
        SEDataRequest.create_physiology_request("CardiacOutput", unit=VolumePerTimeUnit.L_Per_min),
        SEDataRequest.create_physiology_request("ArterialPressure", unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("MeanArterialPressure", unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("SystolicArterialPressure", unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("DiastolicArterialPressure", unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("BloodVolume", unit=VolumeUnit.mL),
        SEDataRequest.create_physiology_request("OxygenSaturation"),
        SEDataRequest.create_physiology_request("EndTidalCarbonDioxidePressure", unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("RespirationRate", unit=FrequencyUnit.Per_min)
    ]

    # Let's create a set of scenarios that:
    #   Loads a state from our patient creation scenarios
    #   Adds an injury
    #   Advances some time
    injury_scenarios: List[SEScenarioExecStatus] = []
    injury_scenarios_dir = results_dir / "injury/scenarios"
    injury_states_dir = results_dir / "injury/states"
    injury_scenarios_outputs_dir = results_dir / "injury/outputs"
    injury_scenarios_dir.mkdir(parents=True, exist_ok=True)
    injury_states_dir.mkdir(parents=True, exist_ok=True)
    injury_scenarios_outputs_dir.mkdir(parents=True, exist_ok=True)
    # This tracks the status of the execution of these scenarios
    injury_states_exec_status_filename = results_dir / "injury_states_exec_status.json"

    # Let's use all the state files we created, to create our injury scenarios
    if not injury_states_exec_status_filename.exists():
        for injury_state in patient_states_dir.glob("*.json"):
            for severity in [0.6, 0.8]:
                s = SEScenario()
                name = f"{injury_state.stem}__hemorrhage_{severity}"
                s.set_name(name)
                s.set_engine_state(f"{injury_state}")
                s.get_data_request_manager().set_data_requests(data_requests)
                # Note the result file being relative (starting with ./)
                # is going to make it relative to what is provided to executor.set_output_root_directory
                s.get_data_request_manager().set_results_filename(f"./{name}.csv")
                hemorrhage = SEHemorrhage()
                hemorrhage.set_compartment(eHemorrhage_Compartment.RightLeg.name)
                hemorrhage.get_severity().set_value(severity)
                s.get_actions().append(hemorrhage)
                adv = SEAdvanceTime()
                adv.get_time().set_value(5, TimeUnit.min)
                s.get_actions().append(adv)
                state = SESerializeState()
                state.set_filename(str(injury_states_dir / f"{name}.json"))
                s.get_actions().append(state)
                f = f"{injury_scenarios_dir}/{name}.json"
                serialize_scenario_to_file(s, f)
                # Add this scenario to our exec status
                e = SEScenarioExecStatus()
                e.set_scenario_filename(f)
                injury_scenarios.append(e)
        # Write out the exec status so we can run it
        serialize_scenario_exec_status_list_to_file(injury_scenarios, str(injury_states_exec_status_filename))
    # Now run those scenarios (this will be quick if its already been run)
    executor.set_log_to_console(eSwitch.Off)  # Output can get pretty busy...
    # executor.set_organize_output_directory(eSwitch.On)
    executor.set_output_root_directory(str(injury_scenarios_outputs_dir))
    executor.set_scenario_exec_list_filename(str(injury_states_exec_status_filename))
    # serialize_pulse_scenario_exec_to_file(executor, f"{results_dir}/injury_states_exec.json")
    if not executor.execute_scenario():
        # You can view the injury_states_exec to see what happened
        _log.error(f"Problem running {injury_states_exec_status_filename}")
        return
    # We can read in the status if we need to
    injury_states_status: List[SEScenarioExecStatus] = []
    serialize_scenario_exec_status_list_from_file(str(injury_states_exec_status_filename), injury_states_status)

    # Let's create a set of scenarios that:
    #   Loads a state from our injury scenarios
    #   Performs the appropriate intervention
    #   Advances some time
    intervention_scenarios: List[SEScenarioExecStatus] = []
    intervention_scenarios_dir = results_dir / "intervention/scenarios"
    intervention_states_dir = results_dir / "intervention/states"
    intervention_scenarios_outputs_dir = results_dir / "intervention/outputs"
    intervention_scenarios_dir.mkdir(parents=True, exist_ok=True)
    intervention_states_dir.mkdir(parents=True, exist_ok=True)
    intervention_scenarios_outputs_dir.mkdir(parents=True, exist_ok=True)
    # This tracks the status of the execution of these scenarios
    intervention_states_exec_status_filename = results_dir / "intervention_states_exec_status.json"

    # Let's use all the state files we created, to create our intervention scenarios
    if not intervention_states_exec_status_filename.exists():
        for injury_state in injury_states_dir.glob("*.json"):
            for severity in [0.0, 0.4]:
                s = SEScenario()
                name = f"{injury_state.stem}__tourniquet_{severity}"
                s.set_name(name)
                s.set_engine_state(f"{injury_state}")
                s.get_data_request_manager().set_data_requests(data_requests)
                # Note the result file being relative (starting with ./)
                # is going to make it relative to what is provided to executor.set_output_root_directory
                s.get_data_request_manager().set_results_filename(f"./{name}.csv")
                hemorrhage = SEHemorrhage()
                hemorrhage.set_compartment(eHemorrhage_Compartment.RightLeg.name)
                hemorrhage.get_severity().set_value(severity)
                s.get_actions().append(hemorrhage)
                adv = SEAdvanceTime()
                adv.get_time().set_value(5, TimeUnit.min)
                s.get_actions().append(adv)
                state = SESerializeState()
                state.set_filename(str(intervention_states_dir / f"{name}.json"))
                s.get_actions().append(state)
                f = f"{intervention_scenarios_dir}/{name}.json"
                serialize_scenario_to_file(s, f)
                # Add this scenario to our exec status
                e = SEScenarioExecStatus()
                e.set_scenario_filename(f)
                intervention_scenarios.append(e)
        # Write out the exec status so we can run it
        serialize_scenario_exec_status_list_to_file(intervention_scenarios,
                                                    str(intervention_states_exec_status_filename))
    # Now run those scenarios (this will be quick if its already been run)
    executor.set_log_to_console(eSwitch.Off)  # Output can get pretty busy...
    # executor.set_organize_output_directory(eSwitch.On)
    executor.set_output_root_directory(str(intervention_scenarios_outputs_dir))
    executor.set_scenario_exec_list_filename(str(intervention_states_exec_status_filename))
    # serialize_pulse_scenario_exec_to_file(executor, f"{results_dir}/intervention_states_exec.json")
    if not executor.execute_scenario():
        # You can view the intervention_states_exec to see what happened
        _log.error(f"Problem running {intervention_states_exec_status_filename}")
        return
    # We can read in the status if we need to
    intervention_states_status: List[SEScenarioExecStatus] = []
    serialize_scenario_exec_status_list_from_file(str(intervention_states_exec_status_filename),
                                                  intervention_states_status)

    # Let's join up the results from an injury and an intervention and save out those joined results
    final_outputs_dir = results_dir / "final/outputs"
    final_images_dir = results_dir / "final/images"
    final_outputs_dir.mkdir(parents=True, exist_ok=True)
    final_images_dir.mkdir(parents=True, exist_ok=True)

    final_csv_files = []
    # Check to see if we already created final csv files
    for final_csv_file in final_outputs_dir.glob("*.csv"):
        final_csv_files.append(final_csv_file)
    if len(final_csv_files) == 0:
        for intervention_csv in intervention_scenarios_outputs_dir.glob("*.csv"):
            #  Above, we used an output file naming convention where
            #  we used the injury file name as our base intervention filename, separated with '__'
            injury_csv = (injury_scenarios_outputs_dir /
                          (intervention_csv.name[:intervention_csv.name.rfind("__")] + ".csv"))
            if not intervention_csv.exists():
                _log.error(f"Could not find {intervention_csv}")
                continue
            if not injury_csv.exists():
                _log.error(f"Could not find {injury_csv}")
                continue
            # Now let's concatenate these results files
            concatenated_df = concat_csv_into_df(injury_csv, intervention_csv)
            if concatenated_df.empty:
                _log.error(f"Unable to concatenate {intervention_csv} to {injury_csv}")
                continue
            # Save the concatenated DataFrame to a new CSV file, using the intervention name
            finale_csv_file = final_outputs_dir / intervention_csv.name
            concatenated_df.to_csv(finale_csv_file, index=False)
            _log.info(f"Concatenated {injury_csv} with {intervention_csv} to create {finale_csv_file}")
            final_csv_files.append(finale_csv_file)

    # Let's create a few plots of values from all of our resulting data frames / csv files
    plot = SEMultiHeaderSeriesPlotter()
    plot.get_plot_config().set_values(
        output_path_override=final_images_dir,
        plot_actions=False,
        plot_events=False
    )
    for final_csv_file in final_csv_files:
        _log.info(f"Loading final csv: {final_csv_file}")
        names = final_csv_file.stem.split("__")
        label = ""
        for i in range(1, 3, 1):
            label += f"{names[i][0]}{names[i][names[i].find('_'):]}-"
        label = label[0:-1]
        plot.add_plot_source(SEPlotSource(csv_data=str(final_csv_file),
                                          label=label))
    series = SESeries()
    series.get_plot_config().set_defaults()
    series.get_plot_config().set_values(gridlines=True)
    series.set_title("Mean Arterial Pressure")
    series.set_output_filename("MeanArterialPressure.png")
    series.set_x_header("Time(s)")
    series.add_y_header("MeanArterialPressure(mmHg)")
    plot.add_series(series)

    series = SESeries()
    series.get_plot_config().set_defaults()
    series.get_plot_config().set_values(gridlines=True)
    series.set_title("Blood Volume")
    series.set_output_filename("BloodVolume.png")
    series.set_x_header("Time(s)")
    series.add_y_header("BloodVolume(mL)")
    plot.add_series(series)

    multi_header_series_plotter(plot)


if __name__ == "__main__":
    main()
