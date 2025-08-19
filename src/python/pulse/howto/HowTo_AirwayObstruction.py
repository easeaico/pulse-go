# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import logging
from pathlib import Path

from pulse.cdm.engine import SEDataRequest, SEDataRequestManager
from pulse.cdm.patient_actions import SEAirwayObstruction, eAirwayObstructionResistanceType
from pulse.cdm.scalars import FrequencyUnit, PressureUnit, VolumePerTimeUnit, VolumeUnit
from pulse.engine.PulseEngine import PulseEngine


_log = logging.getLogger("pulse")


def how_to_airway_obstruction():
    pulse = PulseEngine()
    pulse.set_log_filename("./test_results/howto/HowTo_AirwayObstruction.py.log")
    pulse.log_to_console(True)

    # NOTE: No data requests are being provided, so Pulse will return the default vitals data
    if not pulse.serialize_from_file("./states/StandardMale@0s.json", None):
        print("Unable to load initial state file")
        return

    # Get some data from the engine
    results = pulse.pull_data()
    pulse.print_results()

    airway_obstruction = SEAirwayObstruction()
    airway_obstruction.set_comment("Patient's airways are obstructed")
    airway_obstruction.get_severity().set_value(0.7)
    airway_obstruction.set_resistance_Type(eAirwayObstructionResistanceType.Oscillating)
    pulse.process_action(airway_obstruction)

    # Advance some time and print out the vitals
    pulse.advance_time_s(30)
    results = pulse.pull_data()
    pulse.print_results()


def main():
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
    how_to_airway_obstruction()


if __name__ == "__main__":
    main()

