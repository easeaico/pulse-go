# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.patient import SEPatientConfiguration
from pulse.engine.PulseEngine import PulseEngine

def HowTo_Dehydration():
    pulse = PulseEngine()
    pulse.set_log_filename("./test_results/howto/HowTo_Dehydration.py.log")
    pulse.log_to_console(True)

    pc = SEPatientConfiguration()
    pc.set_patient_file("./patients/StandardMale.json")
    pulmonary_shunt = pc.get_conditions().get_pulmonary_shunt()
    pulmonary_shunt.get_severity().set_value(0.3)

    # Initialize the engine with our configuration
    # NOTE: No data requests are being provided, so Pulse will return the default vitals data
    if not pulse.initialize_engine(pc, None):
        print("Unable to load stabilize engine")
        return

    # Get some data from the engine
    results = pulse.pull_data()
    pulse.print_results()

    # Advance some time and print out the vitals
    # look for the Oxygen saturation to drastically drop
    pulse.advance_time_s(30)
    results = pulse.pull_data()
    pulse.print_results()

HowTo_Dehydration()

