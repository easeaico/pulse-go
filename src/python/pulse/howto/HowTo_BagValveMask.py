# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.enums import eSerializationFormat, eSwitch
from pulse.cdm.scalars import FrequencyUnit, PressureUnit, TimeUnit, VolumeUnit, VolumePerTimeUnit
from pulse.engine.PulseEngine import PulseEngine


# BVM
from pulse.cdm.bag_valve_mask import SEBagValveMask
from pulse.cdm.bag_valve_mask_actions import (
    SEBagValveMaskConfiguration, SEBagValveMaskAutomated, SEBagValveMaskInstantaneous, SEBagValveMaskSqueeze
)

def HowTo_BagValveMask():
    pulse = PulseEngine()
    pulse.set_log_filename("./test_results/howto/HowTo_BagValveMask.py.log")
    pulse.log_to_console(True)

    # NOTE: No data requests are being provided, so Pulse will return the default vitals data
    if not pulse.serialize_from_file("./states/Soldier@0s.json", None):
            print("Unable to load initial state file")
            return

    # Get some data from the engine
    results = pulse.pull_data()
    pulse.print_results()

    # Put the BVM on the patiend
    cfg = SEBagValveMaskConfiguration()
    cfg.get_configuration().set_connection(eSwitch.On)
    pulse.process_action(cfg)

    # The automated BVM action can repeat the specified squeeze cycle
    auto_bvm = SEBagValveMaskAutomated()
    auto_bvm.get_breath_frequency().set_value(12, FrequencyUnit.Per_min)
    auto_bvm.get_inspiratory_expiratory_ratio().set_value(0.5)
    auto_bvm.get_squeeze_pressure().set_value(9.3, PressureUnit.cmH2O)
    pulse.process_action(auto_bvm)

    pulse.advance_time_s(5)
    results = pulse.pull_data()
    pulse.print_results()

    # Manual squeeze (one squeeze)
    squeeze = SEBagValveMaskSqueeze()
    squeeze.get_expiratory_period().set_value(2, TimeUnit.s)
    squeeze.get_inspiratory_period().set_value(3, TimeUnit.s)
    squeeze.get_squeeze_volume().set_value(500, VolumeUnit.mL)
    pulse.process_action(squeeze)

    # Advance some time and print out the vitals
    pulse.advance_time_s(5)
    results = pulse.pull_data()
    pulse.print_results()

    # Usually used with sensors, you can specify the instantaneous value
    # of the current time step of a squeeze. You should pass over sensor
    # readings at a decent rate continuously
    squeeze_flow = SEBagValveMaskInstantaneous()
    squeeze_flow.set_comment("Hold specified flow")
    squeeze_flow.get_flow().set_value(15, VolumePerTimeUnit.L_Per_min)
    pulse.process_action(squeeze_flow)

    pulse.advance_time_s(1)
    results = pulse.pull_data()
    pulse.print_results()

    squeeze_flow.set_comment("Stop flow")
    squeeze_flow.get_flow().set_value(0, VolumePerTimeUnit.L_Per_min)
    pulse.process_action(squeeze_flow)

    pulse.advance_time_s(3)
    results = pulse.pull_data()
    pulse.print_results()

HowTo_BagValveMask()
