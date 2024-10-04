# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import logging
from pathlib import Path

from pulse.cdm.engine import SEDataRequestManager, SEDataRequest

from pulse.engine.PulseEngine import PulseEngine
from pulse.cdm.engine import SESerializeState
from pulse.cdm.scalars import FrequencyUnit, PressureUnit, TemperatureUnit, VolumeUnit
from pulse.cdm.utils.logger import parse_actions, parse_events, parse_patient
from pulse.cdm.patient_actions import SEHemorrhage, eHemorrhage_Compartment, eHemorrhage_Type


def main():
    out_dir = Path("./test_results/howto/HowTo_ProcessResults.py/")
    run_engine(out_dir, total_duration_min=2, sample_step_s=20)
    process_results(out_dir)


def run_engine(out_dir: Path, total_duration_min: float, sample_step_s: float):
    pulse = PulseEngine()
    pulse.set_log_filename(str(out_dir) + "pulse.log")
    pulse.log_to_console(True)
    # Grab the core vital scalars (not interested in waveforms in this example)
    data_requests = [
        SEDataRequest.create_physiology_request("HeartRate", unit=FrequencyUnit.Per_min),
        SEDataRequest.create_physiology_request("MeanArterialPressure", unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("SystolicArterialPressure", unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("DiastolicArterialPressure", unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("OxygenSaturation"),
        SEDataRequest.create_physiology_request("EndTidalCarbonDioxidePressure", unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("RespirationRate", unit=FrequencyUnit.Per_min),
        SEDataRequest.create_physiology_request("SkinTemperature", unit=TemperatureUnit.C),
        SEDataRequest.create_physiology_request("BloodVolume", unit=VolumeUnit.mL)
    ]
    data_req_mgr = SEDataRequestManager(data_requests)
    data_req_mgr.set_results_filename(str(out_dir) + "data.csv")
    if not pulse.serialize_from_file("./states/StandardMale@0s.json", data_req_mgr):
        print("Unable to load initial state file")
        return
    # Apply some injuries to get some degrading vitals
    leg = SEHemorrhage()
    leg.set_compartment(eHemorrhage_Compartment.RightLeg)
    leg.get_severity().set_value(0.9)

    vena_cava = SEHemorrhage()
    vena_cava.set_compartment(eHemorrhage_Compartment.VenaCava)
    vena_cava.get_severity().set_value(0.1)

    num_samples = int(total_duration_min*60 / sample_step_s)
    current_time_s = 0
    save = SESerializeState()
    for step in range(num_samples):
        current_time_s += sample_step_s
        pulse.advance_time_s(sample_step_s)
        # Save the state every time step in a binary form
        save.set_filename(f"{str(out_dir)}/state@{'%.1f'%current_time_s}.pbb")
        pulse.process_action(save)


def process_results(out_dir: Path):
    # Find the log file in this directory
    log = str(list(out_dir.glob("*.log"))[0])
    # Pull out various items from the log
    actions = parse_actions(log)
    events = parse_events(log)
    patient = parse_patient(log)


def find_vitals(out_dir: Path):
    pass


if __name__ == "__main__":
    main()
