# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.engine.PulseEngine import PulseEngine
from pulse.cdm.ecmo import eECMO_CannulationLocation
from pulse.cdm.ecmo_actions import SEECMOConfiguration
from pulse.cdm.scalars import FrequencyUnit, MassUnit, MassPerVolumeUnit, \
                              PressureUnit, VolumeUnit, VolumePerTimeMassUnit, VolumePerTimeUnit
from pulse.cdm.engine import SEDataRequest, SEDataRequestManager

def HowTo_ECMO():
    pulse = PulseEngine()
    pulse.set_log_filename("./test_results/howto/HowTo_ECMO.py.log")
    pulse.log_to_console(True)

    data_requests = [
        SEDataRequest.create_physiology_request("BloodPH"),
        SEDataRequest.create_physiology_request("BloodVolume", unit=VolumeUnit.mL),
        SEDataRequest.create_physiology_request("CardiacOutput", unit=VolumePerTimeUnit.L_Per_min),
        SEDataRequest.create_physiology_request("EndTidalCarbonDioxidePressure", unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("HeartRate", unit=FrequencyUnit.Per_min),
        SEDataRequest.create_physiology_request("Hematocrit"),
        SEDataRequest.create_physiology_request("OxygenSaturation"),
        SEDataRequest.create_physiology_request("RespirationRate", unit=FrequencyUnit.Per_min),
        SEDataRequest.create_physiology_request("SystolicArterialPressure", unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("DiastolicArterialPressure", unit=PressureUnit.mmHg),
        SEDataRequest.create_physiology_request("TidalVolume", unit=VolumeUnit.mL),
        SEDataRequest.create_physiology_request("TotalPulmonaryVentilation", unit=VolumePerTimeUnit.L_Per_min),
        SEDataRequest.create_substance_request("Bicarbonate", "BloodConcentration", unit=MassPerVolumeUnit.g_Per_L),
        SEDataRequest.create_substance_request("CarbonDioxide", "AlveolarTransfer", unit=VolumePerTimeUnit.mL_Per_s),
        SEDataRequest.create_substance_request("Oxygen", "AlveolarTransfer", unit=VolumePerTimeUnit.mL_Per_s),
        SEDataRequest.create_substance_request("Sodium", "BloodConcentration", unit=MassPerVolumeUnit.g_Per_L),
        SEDataRequest.create_substance_request("Sodium", "Clearance-RenalClearance", unit=VolumePerTimeMassUnit.mL_Per_min_kg),
        SEDataRequest.create_substance_request("Sodium", "MassInBody", unit=MassUnit.g),
        SEDataRequest.create_liquid_compartment_substance_request("Aorta", "CarbonDioxide", "PartialPressure", unit=PressureUnit.mmHg),
        SEDataRequest.create_liquid_compartment_substance_request("Aorta", "Oxygen", "PartialPressure", unit=PressureUnit.mmHg),
        # ECMO Compartments
        SEDataRequest.create_liquid_compartment_request("ECMOBloodSamplingPort", "Inflow", unit=VolumePerTimeUnit.mL_Per_s),
        SEDataRequest.create_liquid_compartment_request("ECMOBloodSamplingPort", "Outflow", unit=VolumePerTimeUnit.mL_Per_s),
        SEDataRequest.create_liquid_compartment_request("ECMOOxygenator", "Inflow", unit=VolumePerTimeUnit.mL_Per_s),
        SEDataRequest.create_liquid_compartment_request("ECMOOxygenator", "Outflow", unit=VolumePerTimeUnit.mL_Per_s),
        SEDataRequest.create_liquid_compartment_substance_request("ECMOOxygenator", "Sodium", "Concentration", unit=MassPerVolumeUnit.g_Per_dL),
        SEDataRequest.create_liquid_compartment_substance_request("VenaCava", "Sodium", "Concentration", unit=MassPerVolumeUnit.g_Per_dL),
        SEDataRequest.create_liquid_compartment_substance_request("Aorta", "Sodium", "Concentration", unit=MassPerVolumeUnit.g_Per_dL),
    ]

    data_mgr = SEDataRequestManager(data_requests)
    data_mgr.set_results_filename("./test_results/howto/HowTo_ECMO.py.csv")

    if not pulse.serialize_from_file("./states/StandardMale@0s.json", data_mgr):
        print("Unable to load initial state file")
        return

    pulse.advance_time_s(5)
    # Get the values of the data you requested at this time
    results = pulse.pull_data()
    # And write it out to the console
    data_mgr.to_console(results)

    # Attach ECMO from jugular to left leg
    cfg = SEECMOConfiguration()
    settings = cfg.get_settings()
    settings.set_inflow_location(eECMO_CannulationLocation.InternalJugular)
    settings.set_outflow_location(eECMO_CannulationLocation.InternalJugular)
    settings.get_oxygenator_volume().set_value(500, VolumeUnit.mL)
    settings.get_transfusion_flow().set_value(5, VolumePerTimeUnit.mL_Per_s)
    settings.set_substance_compound("Saline")
    # If you provide both, the compound will be added first, then any substance concentrations will be overwritten
    pulse.process_action(cfg)
    settings.clear() # Clear the settings so we only change the flow

    pulse.advance_time_s(30)
    results = pulse.pull_data()
    data_mgr.to_console(results)

    # What is the current (last value in array) sodium concentration at the vena cava?
    vcNa = results[26]
    newNa = vcNa * 0.95
    print("Vena Cava Na concentration is " + str(vcNa) + ", reducing to " + str(newNa))


    # Increase flow
    settings.get_transfusion_flow().set_value(4, VolumePerTimeUnit.mL_Per_s)
    settings.get_substance_concentration("Sodium").get_concentration().set_value(newNa, MassPerVolumeUnit.g_Per_dL)
    pulse.process_action(cfg)

    pulse.advance_time_s(30)
    results = pulse.pull_data()
    data_mgr.to_console(results)

    # Further increase flow
    settings.get_transfusion_flow().set_value(0.8, VolumePerTimeUnit.mL_Per_s)
    pulse.process_action(cfg)

    pulse.advance_time_s(30)
    results = pulse.pull_data()
    data_mgr.to_console(results)

    # Detach ECMO
    settings.get_transfusion_flow().set_value(0, VolumePerTimeUnit.mL_Per_s)
    pulse.process_action(cfg)

    pulse.advance_time_s(30)
    results = pulse.pull_data()
    data_mgr.to_console(results)

HowTo_ECMO()
