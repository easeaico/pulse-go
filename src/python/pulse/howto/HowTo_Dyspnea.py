# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.engine import eSerializationFormat
from pulse.cdm.patient_actions import SEDyspnea
from pulse.engine.PulseEngine import PulseEngine

def HowTo_Dyspnea():
    pulse = PulseEngine()
    pulse.set_log_filename("./test_results/howto/HowTo_Dyspnea.py.log")
    pulse.log_to_console(True)

    # NOTE: No data requests are being provided, so Pulse will return the default vitals data
    if not pulse.serialize_from_file("./states/Soldier@0s.json", None):
        print("Unable to load initial state file")
        return

    # Get some data from the engine
    results = pulse.pull_data()
    pulse.print_results()

    # Perform an action
    dsypnea = SEDyspnea()
    dsypnea.set_comment("Patient's dsypnea occurs")
    dsypnea.get_respiration_rate_severity().set_value(0.5)
    dsypnea.get_tidal_volume_severity().set_value(0.5)
    pulse.process_action(dsypnea)

    # Advance some time and print out the vitals
    pulse.advance_time_s(30)
    results = pulse.pull_data()
    pulse.print_results()

HowTo_Dyspnea()

