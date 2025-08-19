# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.enums import eSerializationFormat
from pulse.cdm.patient_actions import SESubstanceCompoundInfusion
from pulse.cdm.scalars import VolumePerTimeUnit, VolumeUnit
from pulse.engine.PulseEngine import PulseEngine
from pulse.cdm.engine import SEDataRequest, SEDataRequestManager

def HowTo_CompoundSubstanceInfusion():
    pulse = PulseEngine()
    pulse.set_log_filename("./test_results/howto/HowTo_SubstanceCompoundInfusion.py.log")
    pulse.log_to_console(True)

    data_requests = [
        SEDataRequest.create_physiology_request("BloodVolume", unit=VolumeUnit.mL),
        SEDataRequest.create_physiology_request("CardiacOutput", unit=VolumePerTimeUnit.L_Per_min),
        SEDataRequest.create_action_request("SubstanceCompoundInfusion", "BagVolume", unit=VolumeUnit.mL)
    ]
    data_mgr = SEDataRequestManager(data_requests)

    if not pulse.serialize_from_file("./states/Soldier@0s.json", data_mgr):
        print("Unable to load initial state file")
        return

    # Get some data from the engine
    results = pulse.pull_data()
    pulse.print_results()

    # Perform an action
    substance_compound = SESubstanceCompoundInfusion()
    substance_compound.set_comment("Patient receives infusion of blood")
    substance_compound.set_compound("Blood")
    substance_compound.get_rate().set_value(100, VolumePerTimeUnit.mL_Per_min)
    substance_compound.get_bag_volume().set_value(.5, VolumeUnit.L)
    pulse.process_action(substance_compound)

    # Advance some time and print out the vitals
    for i in range(6):
        pulse.advance_time_s(5)
        results = pulse.pull_data()
        pulse.print_results()

    pulse.clear()

HowTo_CompoundSubstanceInfusion()

