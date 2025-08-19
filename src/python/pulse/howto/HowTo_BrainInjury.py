# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.enums import eSerializationFormat
from pulse.cdm.patient_actions import SEBrainInjury, eBrainInjuryType
from pulse.engine.PulseEngine import PulseEngine

def HowTo_BrainInjury():
    pulse = PulseEngine()
    pulse.set_log_filename("./test_results/howto/HowTo_BrainInjury.py.log")
    pulse.log_to_console(True)

    # NOTE: No data requests are being provided, so Pulse will return the default vitals data
    if not pulse.serialize_from_file("./states/Soldier@0s.json", None):
        print("Unable to load initial state file")
        return

    # Get some data from the engine
    results = pulse.pull_data()
    pulse.print_results()

    brain_injury = SEBrainInjury()
    brain_injury.set_comment("Patient receives a brain injury")
    brain_injury.set_injury_type(eBrainInjuryType.LeftFocal)
    brain_injury.get_severity().set_value(.5)
    pulse.process_action(brain_injury)

    # Advance some time and print out the vitals
    pulse.advance_time_s(30)
    results = pulse.pull_data()
    pulse.print_results()

HowTo_BrainInjury()

