# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.enums import eSerializationFormat
from pulse.cdm.patient_actions import SETensionPneumothorax, \
                                      SENeedleDecompression, \
                                      SEChestOcclusiveDressing, \
                                      eSwitch, eSide, eGate
from pulse.engine.PulseEngine import PulseEngine

def HowTo_TensionPneumothorax():
    pulse = PulseEngine()
    pulse.set_log_filename("./test_results/howto/HowTo_TensionPneumothorax.py.log")
    pulse.log_to_console(True)

    # NOTE: No data requests are being provided, so Pulse will return the default vitals data
    if not pulse.serialize_from_file("./states/Soldier@0s.json", None):
        print("Unable to load initial state file")
        return

    # Get some data from the engine
    results = pulse.pull_data()
    pulse.print_results()

    # Perform an action
    tension = SETensionPneumothorax()
    tension.set_comment("Patient ")
    tension.get_severity().set_value(.7)
    tension.set_side(eSide.Right)
    tension.set_type(eGate.Open)
    pulse.process_action(tension)

    # Advance some time and print out the vitals
    pulse.advance_time_s(30)
    results = pulse.pull_data()
    pulse.print_results()

    # There are two ways you can help a pneumothorax
    # Apply a chest occlusive dressing to an open pneumothorax
    # It's not required for closed.
    dressing = SEChestOcclusiveDressing()
    dressing.set_comment("Applying Chest occlusive dressing to the patient's right side")
    dressing.set_side(eSide.Right)
    dressing.set_state(eSwitch.On)
    pulse.process_action(dressing)

    pulse.advance_time_s(30)
    results = pulse.pull_data()
    pulse.print_results()

    # Both can use the needle
    needle_decomp = SENeedleDecompression()
    needle_decomp.set_comment("Patient undergoes needle decompression on right site")
    needle_decomp.set_side(eSide.Right)
    needle_decomp.set_state(eSwitch.On)
    pulse.process_action(needle_decomp)

    # Advance some time and print out the vitals
    pulse.advance_time_s(30)
    results = pulse.pull_data()
    pulse.print_results()

HowTo_TensionPneumothorax()

