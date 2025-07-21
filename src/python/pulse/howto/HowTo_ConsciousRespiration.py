# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from enum import Enum
from pulse.cdm.enums import eSerializationFormat
from pulse.cdm.scalars import SEScalarTime, TimeUnit, SEScalar0To1
from pulse.cdm.patient_actions import SEConsciousRespiration
from pulse.engine.PulseEngine import PulseEngine

def HowTo_ConsciousRespiration():
    pulse = PulseEngine()
    pulse.set_log_filename("./test_results/howto/HowTo_ConsciousRespiration.py.log")
    pulse.log_to_console(True)

    # NOTE: No data requests are being provided, so Pulse will return the default vitals data
    if not pulse.serialize_from_file("./states/Soldier@0s.json", None):
        print("Unable to load initial state file")
        return

    # Get some data from the engine
    results = pulse.pull_data()
    pulse.print_results()

    respiration = SEConsciousRespiration()
    respiration.set_comment("Patient inhales for 3 seconds, holds for 15, then exhales for 3 seconds")
    respiration.add_forced_inhale(inhale_period=SEScalarTime(value=3, units=TimeUnit.s),
                                  volume_fraction=SEScalar0To1(value=.5), hold_period=SEScalarTime(value=1, units=TimeUnit.s))
    respiration.add_forced_pause(hold_period=SEScalarTime(value=15, units=TimeUnit.s))
    respiration.add_forced_exhale(exhale_period=SEScalarTime(value=3, units=TimeUnit.s),
                                  volume_fraction=SEScalar0To1(value=.5))
    respiration.set_start_immediately(True)
    pulse.process_action(respiration)

    # Advance some time and print out the vitals
    pulse.advance_time_s(30)
    results = pulse.pull_data()
    pulse.print_results()

HowTo_ConsciousRespiration()

