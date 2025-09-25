# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.engine import SEDataRequestManager, SEDataRequest, SEAdvanceUntilStable
from pulse.cdm.patient import SEPatientConfiguration
from pulse.cdm.patient_actions import SEChronicObstructivePulmonaryDiseaseExacerbation
from pulse.cdm.physiology import eLungCompartment
from pulse.cdm.scalars import FrequencyUnit, PressureUnit, VolumeUnit, VolumePerTimeUnit
from pulse.engine.PulseEngine import PulseEngine


_data_requests = [
    SEDataRequest.create_physiology_request("HeartRate", unit=FrequencyUnit.Per_min),
    SEDataRequest.create_physiology_request("MeanArterialPressure",  unit=PressureUnit.mmHg),
    SEDataRequest.create_physiology_request("OxygenSaturation"),
    SEDataRequest.create_physiology_request("EndTidalCarbonDioxidePressure",  unit=PressureUnit.mmHg),
    SEDataRequest.create_physiology_request("RespirationRate",  unit=FrequencyUnit.Per_min),
    SEDataRequest.create_gas_compartment_request("LeftLungPulmonary", "Volume",  unit=VolumeUnit.mL),
    SEDataRequest.create_gas_compartment_request("RightLungPulmonary", "Volume",  unit=VolumeUnit.L),  # Unit difference is OK!
    SEDataRequest.create_substance_request("Oxygen", "AlveolarTransfer", VolumePerTimeUnit.mL_Per_s),
    SEDataRequest.create_substance_request("CarbonDioxide", "AlveolarTransfer", VolumePerTimeUnit.mL_Per_s),
]


def how_to_run_copd_condition():
    pulse = PulseEngine()
    pulse.set_log_filename("./test_results/howto/HowTo_COPD.py/how_to_run_copd_condition.log")
    pulse.log_to_console(True)

    pc = SEPatientConfiguration()
    pc.set_patient_file("./patients/StandardMale.json")
    copd = pc.get_conditions().get_chronic_obstructive_pulmonary_disease()
    copd.get_bronchitis_severity().set_value(0.2)
    copd.get_emphysema_severity(eLungCompartment.LeftLung).set_value(0.3)
    copd.get_emphysema_severity(eLungCompartment.RightLung).set_value(0.3)

    data_req_mgr = SEDataRequestManager(_data_requests)
    data_req_mgr.set_results_filename("./test_results/howto/HowTo_COPD.py/how_to_run_copd_condition.csv")

    if not pulse.initialize_engine(pc, data_req_mgr):
        print("Unable to load stabilize engine")
        return

    # Get some data from the engine
    pulse.pull_data()
    pulse.print_results()

    # Perform an action to exacerbate the initial condition state
    exacerbation = SEChronicObstructivePulmonaryDiseaseExacerbation()
    exacerbation.set_comment("Patient's COPD is exacerbated")
    exacerbation.get_bronchitis_severity().set_value(0.5)
    exacerbation.get_emphysema_severity(eLungCompartment.LeftLung).set_value(0.4)
    exacerbation.get_emphysema_severity(eLungCompartment.RightLung).set_value(0.2)
    pulse.process_action(exacerbation)

    # Advance some time and print out the vitals
    pulse.advance_time_s(30)
    pulse.pull_data()
    pulse.print_results()


def how_to_run_copd_exacerbation():
    pulse = PulseEngine()
    pulse.set_log_filename("./test_results/howto/HowTo_COPD.py/how_to_run_copd_exacerbation.log")
    pulse.log_to_console(True)

    data_req_mgr = SEDataRequestManager(_data_requests)
    data_req_mgr.set_results_filename("./test_results/howto/HowTo_COPD.py/how_to_run_copd_exacerbation.csv")

    if not pulse.serialize_from_file("./states/StandardMale@0s.json", data_req_mgr):
        print("Unable to load stabilize engine")
        return

    # Get some data from the engine
    pulse.pull_data()
    pulse.print_results()

    # Perform an action to exacerbate the initial condition state
    exacerbation = SEChronicObstructivePulmonaryDiseaseExacerbation()
    exacerbation.set_comment("Patient's COPD is exacerbated")
    exacerbation.get_bronchitis_severity().set_value(0.5)
    exacerbation.get_emphysema_severity(eLungCompartment.LeftLung).set_value(0.4)
    exacerbation.get_emphysema_severity(eLungCompartment.RightLung).set_value(0.2)
    pulse.process_action(exacerbation)

    # Advance until our system is stable
    adv = SEAdvanceUntilStable()
    pulse.process_action(adv)
    pulse.pull_data()
    pulse.print_results()
    print("Add a lil more time.")
    # Simulate a bit of 'stable' time
    pulse.advance_time_s(30)
    pulse.pull_data()
    pulse.print_results()


if __name__ == "__main__":
    # how_to_run_copd_condition()
    how_to_run_copd_exacerbation()

