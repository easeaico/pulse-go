# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pathlib import Path

from pulse.cdm.engine import eSwitch, eSerializationFormat
from pulse.cdm.engine import SEDataRequestManager, SEDataRequest
from pulse.cdm.patient import SEPatientConfiguration
from pulse.cdm.patient_actions import SEAcuteRespiratoryDistressSyndromeExacerbation
from pulse.cdm.physiology import eLungCompartment
from pulse.cdm.scalars import FrequencyUnit, LengthUnit, MassUnit, PressureUnit, \
                              TimeUnit, VolumeUnit

from pulse.engine.PulseEngine import PulseEngine
from pulse.engine.PulseConfiguration import PulseConfiguration


def HowTo_ExpandedRespiratory():
    pulse = PulseEngine()
    pulse.set_log_filename("./test_results/howto/HowTo_ExpandedRespiratory.py.log")
    pulse.log_to_console(True)

    cfg = PulseConfiguration()
    cfg.set_expanded_respiratory(eSwitch.On)
    pulse.set_configuration_override(cfg)

    data_requests = [
        SEDataRequest.create_physiology_request("HeartRate", unit=FrequencyUnit.Per_min),
        SEDataRequest.create_physiology_request("ArterialPressure", unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("MeanArterialPressure", unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("SystolicArterialPressure", unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("DiastolicArterialPressure", unit=PressureUnit.mmHg),

        SEDataRequest.create_physiology_request("EndTidalCarbonDioxidePressure", unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("HorowitzIndex", unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("OxygenSaturation"),
        SEDataRequest.create_physiology_request("RespirationRate", unit=FrequencyUnit.Per_min),
        SEDataRequest.create_physiology_request("TidalVolume", unit=VolumeUnit.mL),
        SEDataRequest.create_physiology_request("TotalLungVolume", unit=VolumeUnit.mL),
        SEDataRequest.create_gas_compartment_request("LeftLungPulmonary", "Volume", unit=VolumeUnit.mL),
        SEDataRequest.create_gas_compartment_request("RightLungPulmonary", "Volume", unit=VolumeUnit.L),
    ]
    data_req_mgr = SEDataRequestManager(data_requests)
    # NOTE: If No data requests are being provided, Pulse will return the default vitals data
    # In addition to getting this data back via this API
    # You can have Pulse write the data you have requested to a CSV file
    data_req_mgr.set_results_filename("./test_results/howto/HowTo_ExpandedRespiratory.py.csv")

    state_filename = Path("./test_results/howto/Satish.json")
    if state_filename.exists():
        # If you have run the patient configuration code below and have a state file, you can load it like this
        # If you want to stabilize again, just delete that file
        pulse.serialize_from_file(str(state_filename), data_req_mgr)
    else:
        pc = SEPatientConfiguration()
        p = pc.get_patient()
        p.set_name("Satish")
        p.get_age().set_value(22, TimeUnit.yr)
        p.get_height().set_value(72, LengthUnit.inch)
        p.get_weight().set_value(180, MassUnit.lb)
        if not pulse.initialize_engine(pc, data_req_mgr):
            print("Unable to load stabilize engine")
            return
        # Save out the initial stabilized state, we can use it later and start up quicker
        pulse.serialize_to_file(str(state_filename))

    # Get default data at time 0s from the engine
    results = pulse.pull_data()
    pulse.print_results()


    # Advance some time and print out the vitals
    pulse.advance_time_s(30)
    results = pulse.pull_data()
    pulse.print_results()

    ards = SEAcuteRespiratoryDistressSyndromeExacerbation()
    ards.get_severity(eLungCompartment.RightSuperiorLobe).set_value(0.3)
    ards.get_severity(eLungCompartment.RightSuperiorLobePosterior).set_value(0.3)
    ards.get_severity(eLungCompartment.RightSuperiorLobeAnterior).set_value(0.3)
    ards.get_severity(eLungCompartment.RightMiddleLobeLateral).set_value(0.3)
    ards.get_severity(eLungCompartment.RightMiddleLobeMedial).set_value(0.3)
    ards.get_severity(eLungCompartment.RightInferiorLobeSuperior).set_value(0.3)
    ards.get_severity(eLungCompartment.RightInferiorLobeMedialBasal).set_value(0.3)
    ards.get_severity(eLungCompartment.RightInferiorLobeAnteriorBasal).set_value(0.3)
    ards.get_severity(eLungCompartment.RightInferiorLobeLateralBasal).set_value(0.3)
    ards.get_severity(eLungCompartment.RightInferiorLobePosteriorBasal).set_value(0.3)
    ards.get_severity(eLungCompartment.LeftInferiorLobePosteriorBasal).set_value(0.3)
    ards.get_severity(eLungCompartment.LeftInferiorLobeLateralBasal).set_value(0.3)
    ards.get_severity(eLungCompartment.LeftInferiorLobeAnteromedialBasal).set_value(0.3)
    ards.get_severity(eLungCompartment.LeftInferiorLobeSuperior).set_value(0.3)
    ards.get_severity(eLungCompartment.LeftSuperiorLobeInferiorLingula).set_value(0.3)
    ards.get_severity(eLungCompartment.LeftSuperiorLobeSuperiorLingula).set_value(0.3)
    ards.get_severity(eLungCompartment.LeftSuperiorLobeAnterior).set_value(0.3)
    ards.get_severity(eLungCompartment.LeftSuperiorLobeApicoposterior).set_value(0.3)
    pulse.process_action(ards)

    # Advance some time and print out the vitals
    pulse.advance_time_s(300)
    results = pulse.pull_data()
    pulse.print_results()

HowTo_ExpandedRespiratory()

