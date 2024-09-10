# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.engine.PulseEngine import PulseEngine
from pulse.cdm.engine import SEDataRequest, SEDataRequestManager, eSwitch
from pulse.cdm.patient_actions import SECardiovascularMechanicsModification, SERespiratoryMechanicsModification
from pulse.cdm.scalars import FrequencyUnit, PressureUnit, PressureTimePerVolumeUnit,\
                              TimeUnit, VolumeUnit, VolumePerPressureUnit, VolumePerTimeUnit

def HowTo_SystemModifiers():
    pulse = PulseEngine()
    pulse.set_log_filename("./test_results/howto/HowTo_SystemModifiers.py.log")
    pulse.log_to_console(True)

    data_requests = [
        SEDataRequest.create_physiology_request("HeartRate", unit=FrequencyUnit.Per_min),
        SEDataRequest.create_physiology_request("ArterialPressure",  unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("MeanArterialPressure",  unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("SystolicArterialPressure",  unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("DiastolicArterialPressure", unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("RespirationRate", unit=FrequencyUnit.Per_min),
        SEDataRequest.create_physiology_request("TidalVolume", unit=VolumeUnit.mL),
        SEDataRequest.create_physiology_request("TotalLungVolume", unit=VolumeUnit.mL),
        SEDataRequest.create_physiology_request("ExpiratoryRespiratoryResistance", PressureTimePerVolumeUnit.cmH2O_s_Per_L),
        SEDataRequest.create_physiology_request("InspiratoryRespiratoryResistance", PressureTimePerVolumeUnit.cmH2O_s_Per_L),
        SEDataRequest.create_physiology_request("RespiratoryCompliance", VolumePerPressureUnit.L_Per_cmH2O),
        SEDataRequest.create_physiology_request("TotalPulmonaryVentilation", VolumePerTimeUnit.L_Per_min)]

    data_mgr = SEDataRequestManager(data_requests)
    data_mgr.set_results_filename("./test_results/howto/HowTo_SystemModifiers.py.csv")

    # NOTE: No data requests are being provided, so Pulse will return the default vitals data
    if not pulse.serialize_from_file("./states/StandardMale@0s.json", data_mgr):
        print("Unable to load initial state file")
        return

    # Get default data at time 0s from the engine
    results = pulse.pull_data()
    data_mgr.to_console(results)

    for i in range(3):
        pulse.advance_time_s(10)
        # Get the values of the data you requested at this time
        results = pulse.pull_data()
        # And write it out to the console
        data_mgr.to_console(results)

    rMod = SERespiratoryMechanicsModification()
    rMods = rMod.get_modifiers()
    rMods.get_respiration_rate_multiplier().set_value(1.05)
    pulse.process_action(rMod)

    cMod = SECardiovascularMechanicsModification()
    cMods = cMod.get_modifiers()
    cMods.get_heart_rate_multiplier().set_value(1.05)
    pulse.process_action(cMod)

    for i in range(12):
        pulse.advance_time_s(10)
        # Get the values of the data you requested at this time
        results = pulse.pull_data()
        # And write it out to the console
        data_mgr.to_console(results)


HowTo_SystemModifiers()
