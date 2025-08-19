# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.enums import eSerializationFormat
from pulse.cdm.patient import SEPatientConfiguration
from pulse.engine.PulseEngine import PulseEngine
from pulse.cdm.environment_actions import SEChangeEnvironmentalConditions
from pulse.cdm.scalars import LengthPerTimeUnit, TemperatureUnit, \
                              PressureUnit, HeatResistanceAreaUnit, \
                              MassPerVolumeUnit, VolumeUnit

from pulse.cdm.io.environment import serialize_environmental_conditions_from_file

def HowTo_EnvironmentalConditions():
    pulse = PulseEngine()
    pulse.set_log_filename("./test_results/howto/HowTo_Environment.py.log")
    pulse.log_to_console(True)

    # Initialize the environment to a specific envirionment
    pc = SEPatientConfiguration()
    pc.set_patient_file("./patients/Soldier.json")
    env = pc.get_conditions().get_initial_environmental_conditions()
    # Let's  load up a file from disk (You don't have to start with a file)
    serialize_environmental_conditions_from_file("./environments/ExerciseEnvironment.json",
                                                    env.get_environmental_conditions())
    # Now let's modify a few properties
    env.get_environmental_conditions().get_air_density().set_value(1.225, MassPerVolumeUnit.kg_Per_m3)
    env.get_environmental_conditions().get_ambient_temperature().set_value(33, TemperatureUnit.C)
    env.get_environmental_conditions().get_respiration_ambient_temperature().set_value(33, TemperatureUnit.C)

    # Initialize the engine with our configuration
    # NOTE: No data requests are being provided, so Pulse will return the default vitals data
    if not pulse.initialize_engine(pc, None):
        print("Unable to load stabilize engine")
        return

    # Get some data from the engine
    results = pulse.pull_data()
    pulse.print_results()

    # Change the environment
    change = SEChangeEnvironmentalConditions()
    change.set_comment("Modify Environmental Conditions")
    # You could just point to an environment file if you want
    # We provide a few in the bin/environments folder
    # change.set_environmental_conditions_file("/path/to/file")
    environmental_conditions = change.get_environmental_conditions()
    # Not all properties have to be set on the environmental conditions
    # Properties not included will retain their current value
    environmental_conditions.get_air_velocity().set_value(0.2, LengthPerTimeUnit.m_Per_s)
    environmental_conditions.get_ambient_temperature().set_value(22, TemperatureUnit.C)
    environmental_conditions.get_atmospheric_pressure().set_value(525, PressureUnit.mmHg)
    environmental_conditions.get_clothing_resistance().set_value(0.5, HeatResistanceAreaUnit.clo)
    environmental_conditions.get_mechanical_dead_space().set_value(0.5, VolumeUnit.L)
    environmental_conditions.get_relative_humidity().set_value(0.5)
    # When changing ambient gas mixture, fractions must sum to 1
    environmental_conditions.remove_ambient_gasses()
    environmental_conditions.get_ambient_gas("Nitrogen").get_fraction_amount().set_value(0.7896)
    environmental_conditions.get_ambient_gas("Oxygen").get_fraction_amount().set_value(0.21)
    environmental_conditions.get_ambient_gas("CarbonDioxide").get_fraction_amount().set_value(4.0E-4)
    # Note CarbonMonoxide can be added to the ambient gas mixture this way
    pulse.process_action(change)

    # Advance some time and print out the vitals
    pulse.advance_time_s(300)
    results = pulse.pull_data()
    pulse.print_results()


HowTo_EnvironmentalConditions()