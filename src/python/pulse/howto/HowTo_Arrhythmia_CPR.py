# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.patient_actions import SEArrhythmia, SEChestCompression, SEChestCompressionInstantaneous
from pulse.cdm.physiology import eHeartRhythm
from pulse.cdm.scalars import ForceUnit, TimeUnit, LengthUnit
from pulse.engine.PulseEngine import PulseEngine

def HowTo_Arrhythmia():
    pulse = PulseEngine()
    pulse.set_log_filename("./test_results/howto/HowTo_Arrhythmia.py.log")
    pulse.log_to_console(True)

    # NOTE: No data requests are being provided, so Pulse will return the default vitals data
    if not pulse.serialize_from_file("./states/Soldier@0s.json", None):
        print("Unable to load initial state file")
        return

    # Get some data from the engine
    results = pulse.pull_data()
    pulse.print_results()

    arryhthmia = SEArrhythmia()
    arryhthmia.set_comment("Patient experiences cardiac arrest")
    arryhthmia.set_rhythm(eHeartRhythm.StableVentricularTachycardia)
    pulse.process_action(arryhthmia)

    # Advance some time and print out the vitals
    pulse.advance_time_s(10)
    results = pulse.pull_data()
    pulse.print_results()

    # If you are using a sensor, you can directly pass over a force
    # You should pass the sensor readings over at a decent rate continuously
    cpr_force = SEChestCompressionInstantaneous()
    cpr_force.set_comment("Press and hold the chest")
    cpr_force.get_force().set_value(450, ForceUnit.N)
    pulse.process_action(cpr_force)

    pulse.advance_time_s(1)
    results = pulse.pull_data()
    pulse.print_results()

    cpr_force.set_comment("Stop pressing")
    cpr_force.get_force().set_value(0, ForceUnit.N)
    pulse.process_action(cpr_force)

    # If you are in software, you can pass in a depth value for a time period
    cpr_depth = SEChestCompression()
    cpr_depth.set_comment("Press and hold the chest")
    cpr_depth.get_depth().set_value(5, LengthUnit.cm)
    cpr_depth.get_compression_period().set_value(1, TimeUnit.s)
    pulse.process_action(cpr_depth)

    pulse.advance_time_s(10)
    results = pulse.pull_data()
    pulse.print_results()

HowTo_Arrhythmia()

