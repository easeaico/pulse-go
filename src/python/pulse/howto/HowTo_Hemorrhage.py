# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import logging

from pulse.cdm.engine import SEDataRequestManager, SEDataRequest
from pulse.cdm.patient_actions import SEHemorrhage, eHemorrhage_Compartment
from pulse.engine.PulseEngine import PulseEngine
from pulse.cdm.scalars import VolumePerTimeUnit, FrequencyUnit, PressureUnit, VolumeUnit

_log = logging.getLogger("pulse")


def how_to_hemorrhage():
    pulse = PulseEngine()
    pulse.set_log_filename("./test_results/howto/HowTo_Hemorrhage.py.log")
    pulse.log_to_console(True)

    data_requests = [
        SEDataRequest.create_physiology_request("HeartRate", unit=FrequencyUnit.Per_min),
        SEDataRequest.create_physiology_request("HeartRhythm"),
        SEDataRequest.create_physiology_request("ArterialPressure", unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("MeanArterialPressure", unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("SystolicArterialPressure", unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("DiastolicArterialPressure", unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("CardiacOutput", unit=VolumePerTimeUnit.L_Per_min),
        SEDataRequest.create_physiology_request("BloodVolume", unit=VolumeUnit.mL),
        SEDataRequest.create_physiology_request("TotalHemorrhageRate", unit=VolumePerTimeUnit.mL_Per_min),
        SEDataRequest.create_physiology_request("TotalHemorrhagedVolume", unit=VolumeUnit.mL)
    ]
    data_req_mgr = SEDataRequestManager(data_requests)

    # NOTE: No data requests are being provided, so Pulse will return the default vitals data
    if not pulse.serialize_from_file("./states/Soldier@0s.json", data_req_mgr):
        print("Unable to load initial state file")
        return

    # Get default data at time 0s from the engine
    pulse.print_results()

    # Perform an action
    hemorrhage = SEHemorrhage()
    hemorrhage.set_comment("Laceration to the leg")
    hemorrhage.set_compartment(eHemorrhage_Compartment.RightLeg.name)
    hemorrhage.get_severity().set_value(0.8)
    # Optionally, You can set the flow rate of the hemorrhage,
    # but this needs to be provided the proper flow rate associated with the anatomy
    # This is implemented as a flow source, this rate will be constant.
    # It will not be affected by dropping blood pressures
    # It is intended to interact with sensors or with something continuously monitoring physiology and updating the flow
    # hemorrhage.get_flow_rate().set_value(75,VolumePerTimeUnit.mL_Per_min)
    pulse.process_action(hemorrhage)

    # Advance some time and print out the vitals
    pulse.advance_time_s(30)
    pulse.print_results()


def main():
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
    how_to_hemorrhage()


if __name__ == "__main__":
    main()

