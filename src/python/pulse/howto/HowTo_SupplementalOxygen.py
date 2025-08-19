# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.enums import eSerializationFormat
from pulse.cdm.scalars import VolumeUnit, VolumePerTimeUnit
from pulse.cdm.patient_actions import SESupplementalOxygen, eDevice
from pulse.engine.PulseEngine import PulseEngine

def HowTo_SupplementalOxygen():
    pulse = PulseEngine()
    pulse.set_log_filename("./test_results/howto/HowTo_SupplementalOxygen.py.log")
    pulse.log_to_console(True)

    # NOTE: No data requests are being provided, so Pulse will return the default vitals data
    if not pulse.serialize_from_file("./states/Soldier@0s.json", None):
        print("Unable to load initial state file")
        return

    # Get some data from the engine
    results = pulse.pull_data()
    pulse.print_results()

    # Perform an action
    oxygen = SESupplementalOxygen()
    oxygen.set_comment("Patient receives supplemental oxygen ")
    oxygen.set_device(eDevice.NasalCannula)
    oxygen.get_volume().set_value(20,VolumeUnit.L)
    oxygen.get_flow().set_value(2, VolumePerTimeUnit.L_Per_min)
    pulse.process_action(oxygen)

    # Advance some time and print out the vitals
    pulse.advance_time_s(30)
    results = pulse.pull_data()
    pulse.print_results()

HowTo_SupplementalOxygen()

