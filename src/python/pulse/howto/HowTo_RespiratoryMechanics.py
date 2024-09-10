# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.engine.PulseEngine import PulseEngine
from pulse.cdm.engine import SEDataRequest, SEDataRequestManager, eSwitch
from pulse.cdm.patient_actions import SERespiratoryMechanicsConfiguration
from pulse.cdm.scalars import FrequencyUnit, PressureUnit, PressureTimePerVolumeUnit,\
                              TimeUnit, VolumeUnit, VolumePerPressureUnit, VolumePerTimeUnit

def HowTo_RespiratoryMechanics():
    pulse = PulseEngine()
    pulse.set_log_filename("./test_results/howto/HowTo_RespiratoryMechanics.py.log")
    pulse.log_to_console(True)

    data_requests = [
        SEDataRequest.create_physiology_request("RespirationRate", unit=FrequencyUnit.Per_min),
        SEDataRequest.create_physiology_request("TidalVolume", unit=VolumeUnit.mL),
        SEDataRequest.create_physiology_request("TotalLungVolume", unit=VolumeUnit.mL),
        SEDataRequest.create_physiology_request("ExpiratoryRespiratoryResistance", PressureTimePerVolumeUnit.cmH2O_s_Per_L),
        SEDataRequest.create_physiology_request("InspiratoryRespiratoryResistance", PressureTimePerVolumeUnit.cmH2O_s_Per_L),
        SEDataRequest.create_physiology_request("RespiratoryCompliance", VolumePerPressureUnit.L_Per_cmH2O),
        SEDataRequest.create_physiology_request("TotalPulmonaryVentilation", VolumePerTimeUnit.L_Per_min)]

    data_mgr = SEDataRequestManager(data_requests)
    data_mgr.set_results_filename("./test_results/howto/HowTo_RespiratoryMechanics.py.csv")

    # NOTE: No data requests are being provided, so Pulse will return the default vitals data
    if not pulse.serialize_from_file("./states/StandardMale@0s.json", data_mgr):
        print("Unable to load initial state file")
        return

    # Get default data at time 0s from the engine
    results = pulse.pull_data()
    data_mgr.to_console(results)

    for i in range(6):
        pulse.advance_time_s(10)
        # Get the values of the data you requested at this time
        results = pulse.pull_data()
        # And write it out to the console
        data_mgr.to_console(results)

    config = SERespiratoryMechanicsConfiguration()
    mechanics = config.get_settings()
    mechanics.set_active(eSwitch.On)
    # Create a mechanics profile
    resistance_cmH2O_s_Per_L = 13.0
    compliance_mL_Per_cmH2O = 50.0
    respirationRate_bpm = 15.0
    inspiratoryTime_s = 0.9

    # These need to add to 1.0 (100%)
    # const double UpperResistanceFraction = 0.5;
    # UpperResistanceFraction is implied
    BronchiResistanceFraction = 0.3
    AlveoliDuctResistanceFraction = 0.2

    upperResistance = resistance_cmH2O_s_Per_L - (BronchiResistanceFraction * resistance_cmH2O_s_Per_L + AlveoliDuctResistanceFraction * resistance_cmH2O_s_Per_L) / 2
    bronchiResistance = 2 * (resistance_cmH2O_s_Per_L - upperResistance) - AlveoliDuctResistanceFraction * resistance_cmH2O_s_Per_L
    alveoliDuctResistance = 2 * (resistance_cmH2O_s_Per_L - upperResistance) - bronchiResistance
    sideResistance = bronchiResistance + alveoliDuctResistance
    RespiratorySideCompliance_L_Per_cmH2O = compliance_mL_Per_cmH2O / 2.0
    LungCompliance_L_Per_cmH2O = 2.0 * RespiratorySideCompliance_L_Per_cmH2O
    ChestWallCompliance_L_Per_cmH2O = LungCompliance_L_Per_cmH2O
    totalBreathTime_s = 1.0 / (respirationRate_bpm / 60.0)
    inspiratoryFraction = inspiratoryTime_s / totalBreathTime_s
    expiratoryFraction = 1.0 - inspiratoryFraction
    InspiratoryRiseFraction = inspiratoryFraction
    InspiratoryHoldFraction = 0.0
    InspiratoryReleaseFraction = min(inspiratoryFraction, expiratoryFraction * 0.5)
    InspiratoryToExpiratoryPauseFraction = 1.0 - InspiratoryRiseFraction - InspiratoryReleaseFraction
    ExpiratoryRiseFraction = 0.0
    ExpiratoryHoldFraction = 0.0
    ExpiratoryReleaseFraction = 0.0

    mechanics.get_upper_inspiratory_resistance().set_value(upperResistance, PressureTimePerVolumeUnit.cmH2O_s_Per_L)
    mechanics.get_upper_expiratory_resistance().set_value(upperResistance, PressureTimePerVolumeUnit.cmH2O_s_Per_L)
    mechanics.get_left_inspiratory_resistance().set_value(sideResistance, PressureTimePerVolumeUnit.cmH2O_s_Per_L)
    mechanics.get_left_expiratory_resistance().set_value(sideResistance, PressureTimePerVolumeUnit.cmH2O_s_Per_L)
    mechanics.get_right_inspiratory_resistance().set_value(sideResistance, PressureTimePerVolumeUnit.cmH2O_s_Per_L)
    lcc = mechanics.get_left_compliance_curve()
    lc = lcc.add_constant_segment()
    lc.get_begin_volume().set_value(float("-inf"), VolumeUnit.mL)
    mechanics.get_right_expiratory_resistance().set_value(sideResistance, PressureTimePerVolumeUnit.cmH2O_s_Per_L)

    lc.get_end_volume().set_value(float("inf"), VolumeUnit.mL)
    lc.get_compliance().set_value(RespiratorySideCompliance_L_Per_cmH2O, VolumePerPressureUnit.mL_Per_cmH2O)
    rcc = mechanics.get_right_compliance_curve()
    rc = rcc.add_constant_segment()
    rc.get_begin_volume().set_value(float("-inf"), VolumeUnit.mL)
    rc.get_end_volume().set_value(float("inf"), VolumeUnit.mL)
    rc.get_compliance().set_value(RespiratorySideCompliance_L_Per_cmH2O, VolumePerPressureUnit.mL_Per_cmH2O)

    mechanics.get_inspiratory_rise_time().set_value(InspiratoryRiseFraction * totalBreathTime_s, TimeUnit.s)
    mechanics.get_inspiratory_hold_time().set_value(InspiratoryHoldFraction * totalBreathTime_s, TimeUnit.s)
    mechanics.get_inspiratory_release_time().set_value(InspiratoryReleaseFraction * totalBreathTime_s, TimeUnit.s)
    mechanics.get_inspiratory_to_expiratory_pause_time().set_value(InspiratoryToExpiratoryPauseFraction * totalBreathTime_s, TimeUnit.s)
    mechanics.get_expiratory_rise_time().set_value(ExpiratoryRiseFraction * totalBreathTime_s, TimeUnit.s)
    mechanics.get_expiratory_hold_time().set_value(ExpiratoryHoldFraction * totalBreathTime_s, TimeUnit.s)
    mechanics.get_expiratory_release_time().set_value(ExpiratoryReleaseFraction * totalBreathTime_s, TimeUnit.s)

    mechanics.get_inspiratory_peak_pressure().set_value(-13.0, PressureUnit.cmH2O)
    mechanics.get_expiratory_peak_pressure().set_value(0.0, PressureUnit.cmH2O)

    pulse.process_action(config)

    for i in range(12):
        pulse.advance_time_s(10)
        # Get the values of the data you requested at this time
        results = pulse.pull_data()
        # And write it out to the console
        data_mgr.to_console(results)


HowTo_RespiratoryMechanics()
