# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pathlib import Path

from pulse.cdm.engine import SEDataRequestManager, SEDataRequest

from pulse.engine.PulseEngine import PulseEngine
from pulse.cdm.engine import IEventHandler, SEEventChange, eEvent
from pulse.cdm.scalars import FrequencyUnit, PressureUnit, TemperatureUnit, VolumeUnit, VolumePerTimeUnit
from pulse.cdm.utils.logger import parse_actions, parse_patient, parse_active_event_windows
from pulse.cdm.patient_actions import SEHemorrhage, eHemorrhage_Compartment, SESubstanceCompoundInfusion


def main():
    out_dir = Path("./test_results/howto/HowTo_ProcessResults.py/")
    out_dir.mkdir(parents=True, exist_ok=True)
    csv_file, log_file = run_engine(out_dir, total_duration_min=20, sample_step_s=10)
    process_results(csv_file, log_file)


class MyEventHandler(IEventHandler):
    __slots__ = ["start_resuscitation"]

    def __init__(self):
        super().__init__()
        self.start_resuscitation = False

    def handle_event(self, change: SEEventChange):
        if (change.event == eEvent.StartOfInhale or
                change.event == eEvent.StartOfExhale or
                change.event == eEvent.StartOfCardiacCycle):
            return
        # Listen for specific event states you are interested in
        if change.event == eEvent.HypovolemicShock and change.active:
            self.start_resuscitation = True


def run_engine(out_dir: Path, total_duration_min: float, sample_step_s: float):
    pulse = PulseEngine()
    log_file = out_dir / "pulse.log"
    csv_file = out_dir / "data.csv"
    if csv_file.exists():
        return csv_file, log_file  # Already ran, just return those results

    events = MyEventHandler()
    pulse.set_log_filename(str(log_file))
    pulse.log_to_console(True)
    pulse.set_event_handler(events)
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
    data_req_mgr.set_results_filename(str(csv_file))
    if not pulse.serialize_from_file("./states/StandardMale@0s.json", data_req_mgr):
        print("Unable to load initial state file")
        return

    # Apply some injuries to get some degrading vitals, we want to run until the patient dies
    leg = SEHemorrhage()
    leg.set_compartment(eHemorrhage_Compartment.RightLeg.value)
    leg.get_severity().set_value(0.9)
    pulse.process_action(leg)

    vena_cava = SEHemorrhage()
    vena_cava.set_compartment(eHemorrhage_Compartment.VenaCava.value)
    vena_cava.get_severity().set_value(0.1)
    pulse.process_action(vena_cava)

    num_samples = int(total_duration_min * 60 / sample_step_s)
    for step in range(num_samples):
        pulse.advance_time_s(sample_step_s)
        if events.start_resuscitation:
            break

    leg.get_severity().set_value(0.0)
    pulse.process_action(leg)

    vena_cava.get_severity().set_value(0.0)
    pulse.process_action(vena_cava)

    iv = SESubstanceCompoundInfusion()
    iv.set_compound("Blood")
    iv.get_rate().set_value(100, VolumePerTimeUnit.mL_Per_min)
    iv.get_bag_volume().set_value(.5, VolumeUnit.L)
    pulse.process_action(iv)

    pulse.advance_time_s(5 * 60)

    return csv_file, log_file


def process_results(csv_file: Path, log_file: Path):
    # Pull out various items from the log
    actions = parse_actions(str(log_file))
    patient = parse_patient(str(log_file))

    active_event_windows = parse_active_event_windows(str(log_file))


if __name__ == "__main__":
    main()
