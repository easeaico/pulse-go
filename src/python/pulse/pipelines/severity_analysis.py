# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import abc
from enum import Enum

import dataframe_image as dfi
import logging
import numpy as np
import pandas as pd

from typing import List
from pathlib import Path

from pulse.cdm.engine import SEDataRequest, SEAdvanceTime, eEvent
from pulse.cdm.enums import eSwitch, eGate, eSide
from pulse.cdm.io.scenario import serialize_scenario_to_file, serialize_scenario_exec_status_list_to_file, \
    serialize_scenario_exec_status_list_from_file
from pulse.cdm.patient_actions import (SEAcuteRespiratoryDistressSyndromeExacerbation,
                                       SEHemorrhage, eHemorrhage_Compartment, SETensionPneumothorax,
                                       SEAirwayObstruction)
from pulse.cdm.physiology import eLungCompartment
from pulse.cdm.scenario import SEScenario, SEScenarioExecStatus
from pulse.cdm.scalars import VolumePerTimeUnit, FrequencyUnit, PressureUnit, VolumeUnit, TimeUnit
from pulse.engine.PulseEngineResults import PulseEngineReprocessor
from pulse.engine.PulseScenarioExec import PulseScenarioExec

_log = logging.getLogger("pulse")


def write_table_image(img_filename: Path, headings: list, data: list, widths=[]):
    # Write out table as png
    wrapped_headers = headings  # ["<br>".join(textwrap.wrap(h, width=20)) for h in headings]
    df = pd.DataFrame(data, columns=wrapped_headers)
    df.style.format(escape="html")  # Actually wrap column names
    df_styler = df.style.hide(axis="index") \
        .set_properties(subset=wrapped_headers[1:], **{'text-align': 'center'}) \
        .set_properties(subset=[wrapped_headers[0]], **{'text-align': 'left'}) \
        .set_properties(**{'border': '1px black solid'})
    for i, width in enumerate(widths):
        df_styler = df_styler.set_properties(subset=wrapped_headers[i], **{'width': width})
    df_styler.set_table_styles(table_styles=[
        {'selector': 'th.col_heading', 'props': 'text-align: center; border: 1px black solid;'},
    ], overwrite=False)
    _log.info(f"Writing {img_filename}")
    dfi.export(df_styler, img_filename, table_conversion='playwright', dpi=800, fontsize=8, max_rows=-1, max_cols=-1)


class SeverityAnalysis(metaclass=abc.ABCMeta):
    __slots__ = ["_cmpt_groups",
                 "_cmpt_groups_idx",
                 "_data_requests",
                 "_name",
                 "_severity_range"]

    def __init__(self, name, severity_ranges: list):
        self._cmpt_groups = []
        self._cmpt_groups_idx = None
        self._data_requests = None
        self._name = name
        self._severity_range = [int(f * 100) / 100 for f in severity_ranges]

    def add_compartment_group(self, cmpts):
        name = ""
        for cmpt in cmpts:
            name += f"{cmpt.name}_"
        # Remove the last '_'
        name = name[:-1].lower()
        self._cmpt_groups.append((name, cmpts))

    @abc.abstractmethod
    def create_actions(self, severity: float):
        pass

    @abc.abstractmethod
    def pull_dependent_variables(self, exec_status):
        pass

    @abc.abstractmethod
    def write_table(self, out_dir: Path):
        pass

    def _execute_scenarios(self, duration_min: float, out_dir: Path):
        sce_dir = out_dir / f"{self._name}/scenarios"
        csv_dir = out_dir / f"{self._name}/outputs"
        exec_file = out_dir / "exec_status.json"

        name = ""
        if self._cmpt_groups_idx is not None:
            name = f"{self._cmpt_groups[self._cmpt_groups_idx][0]}"
            sce_dir = out_dir / f"{self._name}/{name}/scenarios"
            csv_dir = out_dir / f"{self._name}/{name}/outputs"
            exec_file = out_dir / f"{self._name}/{name}/exec_status.json"

        sce_dir.mkdir(parents=True, exist_ok=True)
        csv_dir.mkdir(parents=True, exist_ok=True)

        executor = PulseScenarioExec()
        scenarios: List[SEScenarioExecStatus] = []

        exec_filename = str(exec_file)
        if not exec_file.exists():
            _log.info("Creating Pulse scenarios\n")

            for severity in self._severity_range:
                if name:
                    sim_name = f"{name}_sev_{severity:.2f}"
                else:
                    sim_name = f"sev_{severity:.2f}"
                s = SEScenario()
                s.set_name(f"{sim_name}")
                s.set_description("")
                s.set_engine_state("./states/StandardMale@0s.json")
                _log.info(f"Creating scenario for {sim_name}")
                s.get_data_request_manager().set_data_requests(self._data_requests)
                s.get_data_request_manager().set_results_filename(f"{csv_dir}/{sim_name}.csv")
                for action in self.create_actions(severity):
                    s.get_actions().append(action)
                adv = SEAdvanceTime()
                adv.get_time().set_value(duration_min, TimeUnit.min)
                s.get_actions().append(adv)
                # Write the scenario to disk
                f = f"{sce_dir}/{sim_name}.json"
                serialize_scenario_to_file(s, f)
                # Add this scenario to our exec status
                e = SEScenarioExecStatus()
                e.set_scenario_filename(f)
                scenarios.append(e)

            # Write out the exec status so we can run it
            serialize_scenario_exec_status_list_to_file(scenarios, exec_filename)

        # Now run those scenarios (this will be quick if its already been run)
        # executor.set_thread_count(1)
        executor.set_log_to_console(eSwitch.Off)  # Output can get pretty busy...
        executor.set_output_root_directory(str(csv_dir))
        executor.set_scenario_exec_list_filename(exec_filename)
        _log.info(f"Executing {self._name} scenarios")
        if not executor.execute_scenario():
            # You can view the casualty_states_exec to see what happened
            _log.fatal(f"Problem running {exec_filename}")
            exit(1)
        # Read in the exec status and return it
        exec_statuses: List[SEScenarioExecStatus] = []
        serialize_scenario_exec_status_list_from_file(exec_filename, exec_statuses)
        for exec_status in exec_statuses:
            self.pull_dependent_variables(exec_status)

    def execute(self, duration_min: float, out_dir: Path):
        out_dir.mkdir(parents=True, exist_ok=True)

        self._cmpt_groups_idx = None
        if len(self._cmpt_groups) > 0:
            for i in range(len(self._cmpt_groups)):
                self._cmpt_groups_idx = i
                self._execute_scenarios(duration_min, out_dir)
        else:
            self._execute_scenarios(duration_min, out_dir)

        self.write_table(out_dir)


class SimpleAnalysis(SeverityAnalysis):
    __slots__ = ["_abbrev", "_action", "_ledger", "sample_min"]

    def __init__(self, name, action, severity_ranges: list, data_requests: list):
        super().__init__(name, severity_ranges)
        self._abbrev = []
        self._action = action
        self._ledger = {}
        self.sample_min = 5
        self._data_requests = data_requests

        for dr in self._data_requests:
            prop = dr.get_property_name()
            caps = [char for char in prop if char.isupper()]
            self._abbrev.append("".join(caps).lower())

        for name in self._abbrev:
            self._ledger[name] = {}
            for severity in self._severity_range:
                self._ledger[name][f"{severity:.2f}"] = {}

    def create_actions(self, severity: float):
        a = self._action()
        a.get_severity().set_value(severity)
        return [a]

    def pull_dependent_variables(self, exec_status):
        # Get which casualty this is
        parts = Path(exec_status.get_scenario_filename()).stem.split('sev')
        severity = parts[1].replace('_', '')

        r = PulseEngineReprocessor(csv_files=[Path(exec_status.get_csv_filename())],
                                   log_files=[Path(exec_status.get_log_filename())])
        actions = r.actions
        if len(actions) != 1:
            _log.fatal("We should have 1 and only 1 time of actions")
            exit(1)
        start_time_s = next(iter(actions))

        end_time_s = r.end_time_s
        if r.events:
            for time_s, events in r.events.items():
                for event in events:
                    if event.event == eEvent.IrreversibleState:
                        end_time_s = time_s
        duration_min = (r.end_time_s - start_time_s) / 60

        for i, name in enumerate(self._abbrev):
            time_min = self.sample_min
            while time_min < duration_min:
                results = r.get_values_at_time(time_min*60)
                if time_min not in self._ledger[name][severity]:
                    self._ledger[name][severity][time_min] = {}
                self._ledger[name][severity][time_min] = results[i+1]
                time_min += self.sample_min
                if time_min > end_time_s:  # Patient Died
                    # TODO pad the ledger
                    _log.fatal("Unhandled death in analysis")
                    exit(1)

    def write_table(self, out_dir: Path):

        for prop, severities in self._ledger.items():
            headings = ["Time (min)"]
            _rows = {}
            for severity, times in severities.items():
                headings.append(severity)
                for time, value in times.items():
                    if time not in _rows:
                        _rows[time] = []
                    _rows[time].append(value)
            _data = []
            for time, dd in _rows.items():
                _row = [time]
                for d in dd:
                    _row.append(f"{d:.1f}")
                _data.append(_row)
            write_table_image(out_dir/f"{self._name}_{prop}.png", headings, _data)


class LungAnalysis(SeverityAnalysis):
    __slots__ = ["_ledger", "_sample_min"]

    def __init__(self, name, severity_ranges: list):
        super().__init__(name, severity_ranges)
        self._data_requests = [
            SEDataRequest.create_physiology_request("OxygenSaturation"),
            SEDataRequest.create_physiology_request("RespirationRate", unit=FrequencyUnit.Per_min),
            SEDataRequest.create_physiology_request("TidalVolume", unit=VolumeUnit.mL),
            SEDataRequest.create_physiology_request("TotalLungVolume", unit=VolumeUnit.mL),
            SEDataRequest.create_physiology_request("EndTidalCarbonDioxidePressure", unit=PressureUnit.mmHg),
            SEDataRequest.create_gas_compartment_request("LeftLungPulmonary", "Volume", unit=VolumeUnit.mL),
            SEDataRequest.create_gas_compartment_request("RightLungPulmonary", "Volume", unit=VolumeUnit.mL),
            SEDataRequest.create_gas_compartment_substance_request("Carina", "CarbonDioxide", "PartialPressure",  unit=PressureUnit.mmHg),
            SEDataRequest.create_substance_request("Oxygen", "AlveolarTransfer", VolumePerTimeUnit.mL_Per_s),
            SEDataRequest.create_substance_request("CarbonDioxide", "AlveolarTransfer", VolumePerTimeUnit.mL_Per_s)
        ]
        self._ledger = {}
        self._sample_min = 5

    @abc.abstractmethod
    def create_actions(self, severity: float):
        pass

    def pull_dependent_variables(self, exec_status):
        # Get which casualty this is
        parts = Path(exec_status.get_scenario_filename()).stem.split('sev')
        name = parts[0][:-1]
        severity = parts[1].replace('_', '')

        r = PulseEngineReprocessor(csv_files=[Path(exec_status.get_csv_filename())],
                                   log_files=[Path(exec_status.get_log_filename())])
        actions = r.actions
        if len(actions) != 1:
            _log.fatal("We should have 1 and only 1 time of actions")
            exit(1)
        start_time_s = next(iter(actions))

        end_time_s = r.end_time_s
        if r.events:
            for time_s, events in r.events.items():
                for event in events:
                    # TODO figure out how death/irreversible state would occur (if at all)
                    if event.event == eEvent.CardiovascularCollapse:
                        end_time_s = time_s
        duration_min = (r.end_time_s - start_time_s) / 60

        if name not in self._ledger:
            self._ledger[name] = {}
        if severity not in self._ledger[name]:
            self._ledger[name][severity] = {}

        time_min = self._sample_min
        while time_min < duration_min:
            results = r.get_values_at_time(time_min*60)
            self._ledger[name][severity][time_min] = (results[1], results[2])  # SpO2, RR
            time_min += self._sample_min
            if time_min > end_time_s:  # Patient Died
                # TODO pad the ledger
                _log.fatal("Unhandled death in analysis")
                exit(1)

    def write_table(self, out_dir: Path):

        for cmpts, severities in self._ledger.items():
            headings = ["Time (min)"]
            rr_rows = {}
            spo2_rows = {}
            for severity, times in severities.items():
                headings.append(severity)
                for time, values in times.items():
                    if time not in rr_rows:
                        rr_rows[time] = []
                    rr_rows[time].append(values[1])
                    if time not in spo2_rows:
                        spo2_rows[time] = []
                    spo2_rows[time].append(values[0])
            rr_data = []
            for time, rrs in rr_rows.items():
                rr_row = [time]
                for rr in rrs:
                    rr_row.append(f"{rr:.1f}")
                rr_data.append(rr_row)
            write_table_image(out_dir/f"{self._name}_{cmpts}_rr.png", headings, rr_data)
            spo2_data = []
            for time, spo2s in spo2_rows.items():
                spo2_row = [time]
                for spo2 in spo2s:
                    spo2_row.append(f"{(spo2*100):.1f}")
                spo2_data.append(spo2_row)
            write_table_image(out_dir / f"{self._name}_{cmpts}_spo2.png", headings, spo2_data)


class PneumothoraxAnalysis(LungAnalysis):

    def create_actions(self, severity: float):
        actions = []
        for cmpt in self._cmpt_groups[self._cmpt_groups_idx][1]:
            pneumo = SETensionPneumothorax()
            pneumo.set_type(eGate.Closed)
            if cmpt == eLungCompartment.LeftLung:
                pneumo.set_side(eSide.Left)
            else:
                pneumo.set_side(eSide.Right)
            pneumo.get_severity().set_value(severity)
            actions.append(pneumo)
        return actions


class ARDSAnalysis(LungAnalysis):

    def create_actions(self, severity: float):
        ards = SEAcuteRespiratoryDistressSyndromeExacerbation()
        for cmpt in self._cmpt_groups[self._cmpt_groups_idx][1]:
            ards.get_severity(cmpt).set_value(severity)
        return [ards]


class HemorrhageAnalysis(SeverityAnalysis):
    __slots__ = ["_ledger"]

    def __init__(self, name, severity_ranges: list):
        super().__init__(name, severity_ranges)
        self._data_requests = [
            SEDataRequest.create_physiology_request("HeartRate", unit=FrequencyUnit.Per_min),
            SEDataRequest.create_physiology_request("MeanArterialPressure", unit=PressureUnit.mmHg),
            SEDataRequest.create_physiology_request("SystolicArterialPressure", unit=PressureUnit.mmHg),
            SEDataRequest.create_physiology_request("DiastolicArterialPressure", unit=PressureUnit.mmHg),
            SEDataRequest.create_physiology_request("CardiacOutput", unit=VolumePerTimeUnit.L_Per_min),
            SEDataRequest.create_physiology_request("BloodVolume", unit=VolumeUnit.mL),
            SEDataRequest.create_physiology_request("TotalHemorrhageRate", unit=VolumePerTimeUnit.mL_Per_min),
            SEDataRequest.create_physiology_request("TotalHemorrhagedVolume", unit=VolumeUnit.mL)
        ]
        self._ledger = {}

    def create_actions(self, severity: float):
        actions = []
        for cmpt in self._cmpt_groups[self._cmpt_groups_idx][1]:
            hemorrhage = SEHemorrhage()
            hemorrhage.set_compartment(cmpt.name)
            hemorrhage.get_severity().set_value(severity)
            actions.append(hemorrhage)
        return actions

    def pull_dependent_variables(self, exec_status):
        # Get which casualty this is
        parts = Path(exec_status.get_scenario_filename()).stem.split('sev')
        name = parts[0][:-1]
        severity = parts[1].replace('_', '')

        r = PulseEngineReprocessor(csv_files=[Path(exec_status.get_csv_filename())],
                                   log_files=[Path(exec_status.get_log_filename())])
        actions = r.actions
        if len(actions) != 1:
            _log.fatal("We should have 1 and only 1 time of actions")
            exit(1)
        start_time_s = next(iter(actions))

        end_time_s = r.end_time_s
        if r.events:
            for time_s, events in r.events.items():
                for event in events:
                    if event.event == eEvent.CardiovascularCollapse:
                        end_time_s = time_s
        results = r.get_values_at_time(end_time_s)
        duration_min = (r.end_time_s - start_time_s) / 60

        if severity not in self._ledger:
            self._ledger[severity] = {}
        if name not in self._ledger[severity]:
            self._ledger[severity][name] = results[8]/duration_min

    def write_table(self, out_dir: Path):
        headings = ["severity"]
        for cmpt in self._cmpt_groups:
            headings.append(f"{cmpt[0]} mL/min")

        data = []
        for sev, flows in self._ledger.items():
            row = [sev]
            for cmpt in self._cmpt_groups:
                row.append(f"{flows[cmpt[0]]:.1f}")
            data.append(tuple(row))
        write_table_image(out_dir/f"{self._name}.png", headings, data)


def main():
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

    out_dir = Path("./test_results/action_analysis/")

    class Mode(Enum):
        ALL = 0
        AIRWAY_OBSTRUCTION = 1
        ARDS = 2
        HEMORRHAGE_THORAX = 3
        HEMORRHAGE_ABDOMINAL = 4
        HEMORRHAGE_EXTREMITY = 5
        PNEUMOTHORAX = 6
    mode = Mode.HEMORRHAGE_ABDOMINAL

    tenth_ranges = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]
    hundredth_ranges = list(np.arange(0.01, 1.01, 0.01))
    three_hundredth_ranges = list(np.arange(0.03, 1.01, 0.03))
    high_detail = list(np.arange(0.8, 1.01, 0.01))

    if mode == Mode.ALL or mode == Mode.AIRWAY_OBSTRUCTION:
        ao_analysis = SimpleAnalysis("airway_obstruction", SEAirwayObstruction, high_detail,
                                     data_requests=[SEDataRequest.create_physiology_request("RespirationRate",
                                                                                            unit=FrequencyUnit.Per_min)])
        ao_analysis.execute(60, out_dir)

    if mode == Mode.ALL or mode == Mode.ARDS:
        ards_analysis = PneumothoraxAnalysis("ards", three_hundredth_ranges)
        ards_analysis.add_compartment_group([eLungCompartment.LeftLung])
        ards_analysis.add_compartment_group([eLungCompartment.RightLung])
        ards_analysis.execute(60, out_dir)

    if mode == Mode.ALL or mode == Mode.HEMORRHAGE_ABDOMINAL:
        hemorrhage_abdominal_analysis = HemorrhageAnalysis("hemorrhage_abdominal", three_hundredth_ranges)
        hemorrhage_abdominal_analysis.add_compartment_group([eHemorrhage_Compartment.Liver])
        hemorrhage_abdominal_analysis.add_compartment_group([eHemorrhage_Compartment.Spleen])
        hemorrhage_abdominal_analysis.execute(5, out_dir)

    if mode == Mode.ALL or mode == Mode.HEMORRHAGE_EXTREMITY:
        hemorrhage_extremities_analysis = HemorrhageAnalysis("hemorrhage_extremities", three_hundredth_ranges)
        hemorrhage_extremities_analysis.add_compartment_group([eHemorrhage_Compartment.LeftArm])
        hemorrhage_extremities_analysis.add_compartment_group([eHemorrhage_Compartment.RightArm])
        hemorrhage_extremities_analysis.add_compartment_group([eHemorrhage_Compartment.LeftLeg])
        hemorrhage_extremities_analysis.add_compartment_group([eHemorrhage_Compartment.RightLeg])
        hemorrhage_extremities_analysis.execute(5, out_dir)

    if mode == Mode.ALL or mode == Mode.HEMORRHAGE_THORAX:
        # Let's take a look at our hemorrhage model.
        # Say we want to simulate some thorax laceration type wounds on a casualty.
        # We need to bleed from the skin and muscle.
        # These are very large compartments so the 0-1 is not very intuitive,
        # So let's step some severities to see what kind bleeds we get these using low severities
        skin_muscle_ranges = list(np.arange(0.01, 0.31, 0.01))
        hemorrhage_muscle_skin_analysis = HemorrhageAnalysis("hemorrhage_skin_and_muscle", skin_muscle_ranges)
        hemorrhage_muscle_skin_analysis.add_compartment_group([eHemorrhage_Compartment.Muscle])
        hemorrhage_muscle_skin_analysis.add_compartment_group([eHemorrhage_Compartment.Skin])
        hemorrhage_muscle_skin_analysis.add_compartment_group([eHemorrhage_Compartment.Muscle,
                                                               eHemorrhage_Compartment.Skin])
        hemorrhage_muscle_skin_analysis.execute(5, out_dir)

    if mode == Mode.ALL or mode == Mode.PNEUMOTHORAX:
        pneumothorax_analysis = PneumothoraxAnalysis("pneumothorax", three_hundredth_ranges)
        pneumothorax_analysis.add_compartment_group([eLungCompartment.LeftLung])
        pneumothorax_analysis.add_compartment_group([eLungCompartment.RightLung])
        pneumothorax_analysis.execute(60, out_dir)


if __name__ == "__main__":
    main()
