# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import logging
from pathlib import Path
from typing import List

from pulse.cdm.engine import SEDataRequest, SEAdvanceTime
from pulse.cdm.enums import eSwitch, eEngineInitializationState
from pulse.cdm.io.scenario import serialize_scenario_to_file, serialize_scenario_exec_status_list_from_file, \
    serialize_scenario_exec_status_list_to_file
from pulse.cdm.scalars import FrequencyUnit, TimeUnit, VolumeUnit
from pulse.cdm.scenario import SEScenario, SEScenarioExecStatus, eScenarioExecutionState
from pulse.engine.PulseScenarioExec import PulseScenarioExec

_log = logging.getLogger('pulse')


def how_to_run_scenarios():
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
    file_handler = logging.FileHandler("./how_to_run_scenarios.log", mode='w')
    file_handler.setFormatter(logging.Formatter('%(asctime)s - %(message)s'))
    file_handler.setLevel(logging.INFO)
    _log.addHandler(file_handler)

    executor = PulseScenarioExec()

    out_dir = "./test_results/howto/HowTo-RunScenarios.py"
    Path(out_dir).mkdir(exist_ok=True, parents=True)

    adv = SEAdvanceTime()
    adv.get_time().set_value(2, TimeUnit.min)

    data_requests = [
        SEDataRequest.create_physiology_request("HeartRate", unit=FrequencyUnit.Per_min),
        SEDataRequest.create_physiology_request("RespirationRate", unit=FrequencyUnit.Per_min),
        SEDataRequest.create_physiology_request("TotalLungVolume", unit=VolumeUnit.mL),
    ]

    sce1 = SEScenario()
    sce1.set_name("HowToRunScenarios1")
    sce1.set_description("Simple Scenario to demonstrate building a scenario by the CDM API")
    sce1.set_engine_state("./states/StandardMale@0s.json")
    sce1.get_data_request_manager().set_data_requests(data_requests)
    sce1.get_data_request_manager().set_results_filename(f"{out_dir}/Scenario1.csv")
    sce1.get_actions().append(adv)
    sce1_filename = f"{out_dir}/Scenario1.json"
    serialize_scenario_to_file(sce1, sce1_filename)

    sce2 = SEScenario()
    sce2.set_name("HowToRunScenarios2")
    sce2.set_description("Simple Scenario to demonstrate building a scenario by the CDM API")
    sce2.set_engine_state("./states/StandardFemale@0s.json")
    sce2.get_data_request_manager().set_data_requests(data_requests)
    sce2.get_data_request_manager().set_results_filename(f"{out_dir}/Scenario2.csv")
    sce2.get_actions().append(adv)
    sce2_filename = f"{out_dir}/Scenario2.json"
    serialize_scenario_to_file(sce2, sce2_filename)

    # Run a single scenario
    executor.set_scenario_filename(sce1_filename)
    if not executor.execute_scenario():
        _log.error("Unable to run scenario1")
        return

    # Set up the scenarios we want to run
    exec_status_filename = f"{out_dir}/ScenarioExecStatus.json"
    scenarios: List[SEScenarioExecStatus] = [SEScenarioExecStatus(), SEScenarioExecStatus()]
    scenarios[0].set_scenario_filename(sce1_filename)
    scenarios[1].set_scenario_filename(sce2_filename)
    # Write out the exec status so we can run it
    serialize_scenario_exec_status_list_to_file(scenarios, exec_status_filename)

    # Run Scenarios in parallel
    executor.clear()
    # You can set the number of threads to process your set of scenarios.
    # -1 will figure out how many cores your machine has and create 1 less that that number of threads.
    # If you have less that that scenarios, we just create a thread per scenario.
    # In this case, 2 threads will be created to run 2 scenarios
    # executor.set_thread_count(1)
    executor.set_log_to_console(eSwitch.Off)  # Output can get pretty busy...
    executor.set_scenario_exec_list_filename(exec_status_filename)
    _log.info("Executing scenarios with thread pool")
    if not executor.execute_scenario():
        # You can view the casualty_states_exec to see what happened
        _log.fatal(f"Problem running {exec_status_filename}")
        return
    # Read in the exec status and return it
    sce_exec_status: List[SEScenarioExecStatus] = []
    serialize_scenario_exec_status_list_from_file(exec_status_filename, sce_exec_status)

    for sts in sce_exec_status:
        if sts.get_initialization_state() != eEngineInitializationState.Initialized:
            _log.error(f"Unable to initialize scenario: {sts.get_scenario_filename()}")
            continue
        if sts.get_scenario_execution_state() != eScenarioExecutionState.Complete:
            _log.error(f"Error running scenario: {sts.get_scenario_filename()}")
            continue
        _log.info(f"Successfully ran scenario {sts.get_scenario_filename()}")
        _log.info(f"\t CSV: {sts.get_csv_filename()}")
        _log.info(f"\t Log: {sts.get_log_filename()}")


if __name__ == "__main__":
    how_to_run_scenarios()
