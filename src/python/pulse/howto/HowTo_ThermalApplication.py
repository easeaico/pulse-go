# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.enums import eSerializationFormat, eSwitch
from pulse.cdm.environment_actions import SEThermalApplication
from pulse.engine.PulseEngine import PulseEngine
from pulse.cdm.scalars import AreaUnit, PowerUnit, TemperatureUnit

def HowTo_ThermalApplication():
    pulse = PulseEngine()
    pulse.set_log_filename("./test_results/howto/HowTo_ThermalApplication.py.log")
    pulse.log_to_console(True)

    # NOTE: No data requests are being provided, so Pulse will return the default vitals data
    if not pulse.serialize_from_file("./states/Soldier@0s.json", None):
        print("Unable to load initial state file")
        return

    # Get default data at time 0s from the engine
    results = pulse.pull_data()
    pulse.print_results()

    # Perform an action
    change = SEThermalApplication()
    change.set_comment("Apply a blanket, fan, etc.")
    # You do not have to set all 3 of the application types
    # Types not included will retain their current values
    active_heating = change.get_active_heating()
    active_heating.get_power().set_value(500, PowerUnit.BTU_Per_hr)
    # You can set surface area fraction, or with surface area
    # Note the log message if you set both!
    active_heating.get_surface_area().set_value(0.1, AreaUnit.m2)
    active_heating.get_surface_area_fraction().set_value(0.2)

    active_cooling = change.get_active_cooling()
    active_cooling.get_power().set_value(500, PowerUnit.BTU_Per_hr)
    # You can set surface area fraction, or with surface area
    active_cooling.get_surface_area().set_value(0.1, AreaUnit.m2)

    applied_temperature = change.get_applied_temperature();
    applied_temperature.set_state(eSwitch.On)
    applied_temperature.get_temperature().set_value(30, TemperatureUnit.F)
    applied_temperature.get_surface_area_fraction().set_value(1.0)

    pulse.process_action(change)

    # Advance some time and print out the vitals
    pulse.advance_time_s(30)
    results = pulse.pull_data()
    pulse.print_results()

HowTo_ThermalApplication()