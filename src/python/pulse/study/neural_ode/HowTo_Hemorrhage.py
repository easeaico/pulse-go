# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.enums import eSerializationFormat
from pulse.cdm.patient_actions import SEHemorrhage, eHemorrhageType
from pulse.engine.PulseEngine import PulseEngine
from pulse.cdm.scalars import VolumePerTimeUnit
from pulse.cdm.engine import SEAction, eSerializationFormat, SEDataRequestManager, SEDataRequest

import PyPulse

from pathlib import Path
import numpy as np

import torch
from torch.utils.data import TensorDataset

from torchvision.transforms import ToTensor

def serialize_from_file(pulse_engine, state_file: str,
                              data_request_mgr: SEDataRequestManager):
    '''
    Wrapper to automatically handle json vs pbb
    '''
    # Process requests and setup our results structure
    state_file = Path(state_file)
    if state_file.suffix == '.json':
        drm = pulse_engine._process_requests(data_request_mgr, eSerializationFormat.JSON)
        pulse_engine._is_ready = pulse_engine.__pulse.serialize_from_file(state_file, drm, PyPulse.serialization_format.json)
    elif state_file.suffix == '.pbb':
        drm = pulse_engine._process_requests(data_request_mgr, eSerializationFormat.BINARY)
        pulse_engine._is_ready = pulse_engine.__pulse.serialize_from_file(state_file, drm, PyPulse.serialization_format.binary)
    else:
        raise ValueError(state_file)
    pulse_engine._dt_s = pulse_engine.__pulse.get_timestep("s")
    if pulse_engine._is_ready:
        pulse_engine._pull(True)
    return pulse_engine._is_ready


def run_hemorrhage(initial_state='states/Soldier@0s.json', severity=0.8, freq_hz=1, time_s=30):
    pulse = PulseEngine()
    # TODO use Sacred or DVC for this
    pulse.set_log_filename("./test_results/neural_ode/HowTo_Hemorrhage.py.log")
    pulse.log_to_console(True)

    # NOTE: No data requests are being provided, so Pulse will return the default vitals data
    if not pulse.serialize_from_file(initial_state, None):
        print("Unable to load initial state file")
        return

    # Get default data at time 0s from the engine
    results = pulse.pull_data()
    print(results)

    # Perform an action
    hemorrhage = SEHemorrhage()
    hemorrhage.set_comment("Laceration to the leg")
    hemorrhage.set_external(SEHemorrhage.ExternalCompartment.RightLeg)
    hemorrhage.get_severity().set_value(severity)
    # Optionally, You can set the flow rate of the hemorrhage,
    # but this needs to be provided the proper flow rate associated with the anatomy
    # This is implemented as a flow source, this rate will be constant, and will not be affected by dropping blood pressures
    # It is intended to interact with sensors or with something continuously monitoring physiology and updating the flow
    #hemorrhage.get_flow_rate().set_value(75,VolumePerTimeUnit.mL_Per_min)
    pulse.process_action(hemorrhage)

    # Advance some time and print out the vitals
    # For this example, you will see a decrease in blood volume
    pulse.advance_time_sample_per_s(time_s, 1./freq_hz)
    results = pulse.pull_data()
    keys = ['SimulationTime(s)', 'HeartRate (1/min)', 'ArterialPressure (mmHg)', 'MeanArterialPressure (mmHg)', 'SystolicArterialPressure (mmHg)', 'DiastolicArterialPressure (mmHg)', 'OxygenSaturation (None)', 'EndTidalCarbonDioxidePressure (mmHg)', 'RespirationRate (1/min)', 'SkinTemperature (degC)', 'CardiacOutput (L/min)', 'BloodVolume (mL)', 'Lead3ElectricPotential (mV)', 'Carina - CarbonDioxide - PartialPressure (mmHg)']
    tfm = ToTensor()
    tensors = tfm(np.array(list(results.values())).T)
    dset = TensorDataset(*tensors)
    torch.save(dset, 'test_dset.pt')


if __name__ == '__main__':
    run_hemorrhage()
