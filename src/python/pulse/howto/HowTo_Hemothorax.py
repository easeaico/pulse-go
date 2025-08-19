# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.enums import eSerializationFormat
from pulse.cdm.engine import SEDataRequest, SEDataRequestManager
from pulse.cdm.patient_actions import SEHemothorax, \
                                      SETubeThoracostomy, \
                                      eSide
from pulse.cdm.scalars import FrequencyUnit, PressureUnit, VolumeUnit, \
                              VolumePerTimeMassUnit, VolumePerTimeUnit
from pulse.engine.PulseEngine import PulseEngine

def HowTo_Hemothorax():
    pulse = PulseEngine()
    pulse.set_log_filename("./test_results/howto/HowTo_Hemothorax.py.log")
    pulse.log_to_console(True)

    data_requests = [
        SEDataRequest.create_physiology_request("HeartRate", unit=FrequencyUnit.Per_min),
        SEDataRequest.create_physiology_request("MeanArterialPressure",  unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("SystolicArterialPressure",  unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("DiastolicArterialPressure", unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("RespirationRate",  unit=FrequencyUnit.Per_min),
        SEDataRequest.create_physiology_request("TidalVolume", unit=VolumeUnit.mL),
        SEDataRequest.create_physiology_request("TotalLungVolume", unit=VolumeUnit.mL),
        SEDataRequest.create_physiology_request("OxygenSaturation"),
        SEDataRequest.create_physiology_request("CardiacOutput", unit=VolumePerTimeUnit.L_Per_min),
        SEDataRequest.create_physiology_request("BloodVolume",  unit=VolumeUnit.mL),

        SEDataRequest.create_action_data_request("LeftHemothorax", "FlowRate", unit=VolumePerTimeUnit.mL_Per_min),
        SEDataRequest.create_action_data_request("LeftHemothorax", "TotalBloodVolume", unit=VolumeUnit.mL),
    ]
    data_req_mgr = SEDataRequestManager(data_requests)
    data_req_mgr.set_results_filename("./test_results/howto/HowTo_Hemothorax.py.csv")

    if not pulse.serialize_from_file("./states/StandardMale@0s.json", data_req_mgr):
        print("Unable to load initial state file")
        return

    # Get some data from the engine
    pulse.advance_time_s(50)
    results = pulse.pull_data()
    data_req_mgr.to_console(results)

    # Give the patient a hemothorax
    hemothorax = SEHemothorax()
    hemothorax.set_comment("Patient ")
    hemothorax.get_severity().set_value(0.75)
    hemothorax.set_side(eSide.Left)
    pulse.process_action(hemothorax)

    # Advance some time and print out the vitals
    pulse.advance_time_s(120)
    results = pulse.pull_data()
    data_req_mgr.to_console(results)

    # Apply intervention
    tube_thoracostomy = SETubeThoracostomy()
    tube_thoracostomy.set_comment("Patient undergoes tube thoracostomy on left site")
    tube_thoracostomy.set_side(eSide.Left)
    pulse.process_action(tube_thoracostomy)

    # Advance some time and print out the vitals
    pulse.advance_time_s(400)
    results = pulse.pull_data()
    data_req_mgr.to_console(results)

HowTo_Hemothorax()
