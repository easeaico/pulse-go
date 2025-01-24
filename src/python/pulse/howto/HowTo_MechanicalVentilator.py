# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.engine.PulseEngine import PulseEngine
from pulse.cdm.mechanical_ventilator_actions import SEMechanicalVentilatorConfiguration, \
                                                    SEMechanicalVentilatorContinuousPositiveAirwayPressure, \
                                                    SEMechanicalVentilatorPressureControl, \
                                                    SEMechanicalVentilatorVolumeControl, \
                                                    SEMechanicalVentilatorHold, \
                                                    SEMechanicalVentilatorLeak, \
                                                    eMechanicalVentilator_PressureControlMode, \
                                                    eMechanicalVentilator_VolumeControlMode
from pulse.cdm.scalars import FrequencyUnit, PressureUnit, PressureTimePerVolumeUnit, \
                              TimeUnit, VolumeUnit, VolumePerPressureUnit, VolumePerTimeUnit
from pulse.cdm.mechanical_ventilator import eSwitch, eDriverWaveform
from pulse.cdm.engine import SEDataRequest, SEDataRequestManager
from pulse.cdm.patient_actions import SEDyspnea


def HowTo_MechanicalVentilator():
    pulse = PulseEngine()
    pulse.set_log_filename("./test_results/howto/HowTo_MechanicalVentilator.py.log")
    pulse.log_to_console(True)

    data_requests = [
        SEDataRequest.create_physiology_request("RespirationRate", unit=FrequencyUnit.Per_min),
        # Ventilator Monitor Data
        SEDataRequest.create_mechanical_ventilator_request("RespirationRate", unit=FrequencyUnit.Per_min)]

    data_mgr = SEDataRequestManager(data_requests)
    data_mgr.set_results_filename("./test_results/howto/HowTo_MechanicalVentilator.py.csv")

    # NOTE: No data requests are being provided, so Pulse will return the default vitals data
    if not pulse.serialize_from_file("./states/StandardMale@0s.json", data_mgr):
        print("Unable to load initial state file")
        return

    # Get default data at time 0s from the engine
    results = pulse.pull_data()
    data_mgr.to_console(results)

    # Give the patient Dyspnea
    dsypnea = SEDyspnea()
    dsypnea.set_comment("Patient's dsypnea occurs")
    dsypnea.get_respiration_rate_severity().set_value(1.0)
    pulse.process_action(dsypnea)
    pulse.advance_time_s(10)
    # Get the values of the data you requested at this time
    results = pulse.pull_data()
    # And write it out to the console
    data_mgr.to_console(results)

    # We have action support for several commonly used ventilator modes
    # Pulse is not limited to these modes, These modes are designed for simple understanding
    # Our implementation supports any ventilator mode, you will just need to translate the user facing inputs
    # to a timing/control profile using our configuration settings
    # These modes are internally converted into a configuration setting,
    # You can retrieve the resulting settings action and use it as a basis for more configurations if you want.
    # For example, if you wanted to lengthen the InspirationPatientTriggerFlow of the mode

    cpap = SEMechanicalVentilatorContinuousPositiveAirwayPressure()
    cpap.set_connection(eSwitch.On)
    cpap.get_fraction_inspired_oxygen().set_value(0.21)
    cpap.get_delta_pressure_support().set_value(8.0, PressureUnit.cmH2O)
    cpap.get_positive_end_expired_pressure().set_value(5.0, PressureUnit.cmH2O)
    cpap.get_slope().set_value(0.2, TimeUnit.s)
    #  If you understand our ventilator methodology, You can modify the underlying settings on any mode
    cpap.get_supplemental_settings().get_fraction_inspired_gas("Desflurane").get_fraction_amount().set_value(0.01);
    pulse.process_action(cpap)
    pulse.advance_time_s(10)
    # Get the values of the data you requested at this time
    results = pulse.pull_data()
    # And write it out to the console
    data_mgr.to_console(results)

    pc_ac = SEMechanicalVentilatorPressureControl()
    pc_ac.set_connection(eSwitch.On)
    pc_ac.set_mode(eMechanicalVentilator_PressureControlMode.AssistedControl)
    pc_ac.get_fraction_inspired_oxygen().set_value(0.21)
    pc_ac.get_inspiratory_period().set_value(1.0, TimeUnit.s)
    pc_ac.get_inspiratory_pressure().set_value(13.0, PressureUnit.cmH2O)
    pc_ac.get_positive_end_expired_pressure().set_value(5.0, PressureUnit.cmH2O)
    pc_ac.get_respiration_rate().set_value(12.0, FrequencyUnit.Per_min)
    pc_ac.get_slope().set_value(0.1, TimeUnit.s)
    pulse.process_action(pc_ac)
    pulse.advance_time_s(10)
    # Get the values of the data you requested at this time
    results = pulse.pull_data()
    # And write it out to the console
    data_mgr.to_console(results)

    vc_ac = SEMechanicalVentilatorVolumeControl()
    vc_ac.set_connection(eSwitch.On)
    vc_ac.set_mode(eMechanicalVentilator_VolumeControlMode.AssistedControl)
    vc_ac.get_flow().set_value(50.0, VolumePerTimeUnit.L_Per_min)
    vc_ac.get_fraction_inspired_oxygen().set_value(0.21)
    vc_ac.get_inspiratory_period().set_value(1.0, TimeUnit.s)
    vc_ac.get_positive_end_expired_pressure().set_value(5.0, PressureUnit.cmH2O)
    vc_ac.get_respiration_rate().set_value(12.0, FrequencyUnit.Per_min)
    vc_ac.get_tidal_volume().set_value(600.0, VolumeUnit.mL)
    pulse.process_action(vc_ac)
    pulse.advance_time_s(10)
    # Get the values of the data you requested at this time
    results = pulse.pull_data()
    # And write it out to the console
    data_mgr.to_console(results)

    # Here is an example of programming a custom ventilator mode
    mechanical_ventilator_cfg = SEMechanicalVentilatorConfiguration()
    mechanical_ventilator_cfg.set_comment("Attach the mechanical ventilator")
    # You could just point to a mechanical ventilator configuration file if you want (no examples)
    # mechanical_ventilator.set_settings_file("/path/to/file")
    mechanical_ventilator = mechanical_ventilator_cfg.get_settings()
    mechanical_ventilator.set_connection(eSwitch.On)
    mechanical_ventilator.set_inspiration_waveform(eDriverWaveform.Square)
    mechanical_ventilator.set_expiration_waveform(eDriverWaveform.Square)
    mechanical_ventilator.get_peak_inspiratory_pressure().set_value(21.0, PressureUnit.cmH2O)
    mechanical_ventilator.get_positive_end_expired_pressure().set_value(10.0, PressureUnit.cmH2O)
    mechanical_ventilator.get_fraction_inspired_gas("Oxygen").get_fraction_amount().set_value(0.5)
    respiration_rate_per_min = 20.0
    ie_ratio = 0.5
    # Translate ventilator settings
    total_period_s = 60.0 / respiration_rate_per_min
    inspiratory_period_s = ie_ratio * total_period_s / (1 + ie_ratio)
    expiratory_period_s = total_period_s - inspiratory_period_s
    mechanical_ventilator.get_inspiration_machine_trigger_time().set_value(expiratory_period_s, TimeUnit.s)
    mechanical_ventilator.get_expiration_cycle_time().set_value(inspiratory_period_s, TimeUnit.s)
    pulse.process_action(mechanical_ventilator_cfg)
    # Advance some time and print out the vitals
    pulse.advance_time_s(10)
    # Get the values of the data you requested at this time
    results = pulse.pull_data()
    # And write it out to the console
    data_mgr.to_console(results)

    # You can also perform a hold
    hold = SEMechanicalVentilatorHold()
    hold.set_state(eSwitch.On)
    pulse.process_action(hold)
    # Advance some time and print out the vitals
    pulse.advance_time_s(5)
    # Get the values of the data you requested at this time
    results = pulse.pull_data()
    # And write it out to the console
    data_mgr.to_console(results)
    hold.set_state(eSwitch.Off)
    pulse.process_action(hold)
    # Advance some time and print out the vitals
    pulse.advance_time_s(5)
    # Get the values of the data you requested at this time
    results = pulse.pull_data()
    # And write it out to the console
    data_mgr.to_console(results)

    # A leak can be specified
    leak = SEMechanicalVentilatorLeak()
    leak.get_severity().set_value(0.5)
    pulse.process_action(leak)
    # Advance some time and print out the vitals
    pulse.advance_time_s(5)
    # Get the values of the data you requested at this time
    results = pulse.pull_data()
    # And write it out to the console
    data_mgr.to_console(results)
    leak.get_severity().set_value(0.0)  # Turn off the leak
    pulse.process_action(leak)
    # Advance some time and print out the vitals
    pulse.advance_time_s(5)
    # Get the values of the data you requested at this time
    results = pulse.pull_data()
    # And write it out to the console
    data_mgr.to_console(results)


if __name__ == "__main__":
    HowTo_MechanicalVentilator()
