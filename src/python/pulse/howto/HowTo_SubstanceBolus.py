# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.enums import eSerializationFormat
from pulse.cdm.patient_actions import SESubstanceBolus, eSubstance_Administration
from pulse.cdm.scalars import TimeUnit, MassPerVolumeUnit, VolumeUnit
from pulse.engine.PulseEngine import PulseEngine

def HowTo_SubstanceBolus():
    pulse = PulseEngine()
    pulse.set_log_filename("./test_results/howto/HowTo_SubstanceBolus.py.log")
    pulse.log_to_console(True)

    # NOTE: No data requests are being provided, so Pulse will return the default vitals data
    if not pulse.serialize_from_file("./states/Soldier@0s.json", None):
        print("Unable to load initial state file")
        return

    # Get some data from the engine
    results = pulse.pull_data()
    pulse.print_results()

    # Perform an action
    bolus = SESubstanceBolus()
    bolus.set_comment("Patient receives injection of Epinephrine")
    bolus.set_admin_route(eSubstance_Administration.Intramuscular)
    # bolus.get_admin_duration().set_value(2, TimeUnit.s) (optional)
    bolus.set_substance("Epinephrine")
    bolus.get_dose().set_value(1.0, VolumeUnit.mL)
    bolus.get_concentration().set_value(1.1, MassPerVolumeUnit.ug_Per_L)
    pulse.process_action(bolus)

    # Advance some time and print out the vitals
    pulse.advance_time_s(30)
    results = pulse.pull_data()
    pulse.print_results()

HowTo_SubstanceBolus()

