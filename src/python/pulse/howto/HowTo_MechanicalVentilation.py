# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from enum import Enum
from pulse.cdm.enums import eSerializationFormat
from pulse.cdm.patient_actions import SEMechanicalVentilation, eSwitch
from pulse.cdm.scalars import VolumePerTimeUnit, PressureUnit, VolumeUnit
from pulse.engine.PulseEngine import PulseEngine

def HowTo_MechanicalVentilation():
    pulse = PulseEngine()
    pulse.set_log_filename("./test_results/howto/HowTo_MechanicalVentilation.py.log")
    pulse.log_to_console(True)

    # NOTE: No data requests are being provided, so Pulse will return the default vitals data
    if not pulse.serialize_from_file("./states/Soldier@0s.json", None):
        print("Unable to load initial state file")
        return

    # Get some data from the engine
    results = pulse.pull_data()
    print(results)

    ventilation = SEMechanicalVentilation()
    ventilation.set_comment("Patient is placed on a mechanical ventilator")
    ventilation.get_flow().set_value(50, VolumePerTimeUnit.mL_Per_s)
    ventilation.get_pressure().set_value(.2, PressureUnit.psi)
    ventilation.get_mechanical_dead_space().set_value(100, VolumeUnit.mL)
    ventilation.set_state(eSwitch.On)
    pulse.process_action(ventilation)

    # Advance some time and print out the vitals
    pulse.advance_time_s(30)
    results = pulse.pull_data()
    pulse.print_results()

HowTo_MechanicalVentilation()

