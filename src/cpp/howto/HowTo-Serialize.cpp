/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#include "EngineHowTo.h"
#include "PulseEngine.h"

#include "cdm/engine/SEActionManager.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEDataRequestTracker.h"
#include "cdm/engine/SESerializeRequested.h"
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/system/physiology/SEEnergySystem.h"
#include "cdm/system/physiology/SERespiratorySystem.h"
#include "cdm/patient/actions/SEAirwayObstruction.h"
#include "cdm/patient/actions/SEHemorrhage.h"
#include "cdm/patient/actions/SETensionPneumothorax.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTemperature.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// Serializing a patient state to use later in your Pulse based application
///
/// \details
/// This will show you how to setup a patient and save it out for use in your application
//--------------------------------------------------------------------------------------------------
void HowToSerialize()
{
  std::stringstream ss;
  // Create a Pulse Engine and load the standard patient
  // This is a healty patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/howto/HowTo_Serialize.cpp/HowTo_Serialize.log");

  SEDataRequestManager drMgr(pe->GetLogger());
  drMgr.CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("BloodVolume", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("TidalVolume", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("OxygenSaturation");
  drMgr.CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::Aorta, "Oxygen", "PartialPressure");
  drMgr.CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::Aorta, "CarbonDioxide", "PartialPressure");
  drMgr.CreateGasCompartmentDataRequest(pulse::PulmonaryCompartment::Lungs, "Volume");
  drMgr.CreateGasCompartmentDataRequest(pulse::PulmonaryCompartment::Carina, "InFlow");
  drMgr.SetResultsFilename("./test_results/howto/HowTo_Serialize.cpp/HowTo_Serialize.csv");

  if (!pe->SerializeFromFile("./states/StandardMale@0s.json", &drMgr))
  {
    pe->GetLogger()->Error("Could not load state, check the error");
    return;
  }

  SESerializeRequested serializeRequested;
  serializeRequested.SetFilename("./test_results/howto/HowTo_Serialize.cpp/HowTo_Serialize.requested.json");
  pe->ProcessAction(serializeRequested);

  pe->GetLogger()->Info("Healthy patient vitals");
  pe->GetDataRequestTracker().LogRequestedValues();

  // Next we will want to make the patient unhealthy in some way.
  // This will allow you to start your application with unhealty state

  // Let's mimic a battle field injury

  // Add a Hemorrhage
  SEHemorrhage hemorrhageLeg;
  hemorrhageLeg.SetCompartment(eHemorrhage_Compartment::RightLeg);//the location of the hemorrhage
  hemorrhageLeg.GetSeverity().SetValue(0.5);//the severity of hemorrhage
  pe->ProcessAction(hemorrhageLeg);

  // Add a Tension Pnuemothorax
  SETensionPneumothorax pneumo;
  pneumo.SetSide(eSide::Right);
  pneumo.SetType(eGate::Closed);
  pneumo.GetSeverity().SetValue(0.5);
  pe->ProcessAction(pneumo);

  // Add an Airway Obstruction
  SEAirwayObstruction obstruction;
  obstruction.GetSeverity().SetValue(0.25);
  pe->ProcessAction(obstruction);

  // Now let's advance 5 minutes with these actions to get the patient into a injured state
  pe->AdvanceModelTime(5, TimeUnit::min);

  pe->GetLogger()->Info("Wounded patient vitals");
  pe->GetDataRequestTracker().LogRequestedValues();

  // Save this state out.
  std::string state = "./test_results/howto/HowTo_Serialize.cpp/WoundedSoldier.json";
  // You an then load this state in your application
  if (!pe->SerializeToFile(state))
  {
    pe->GetLogger()->Error("Could not save state, check the error");
    return;
  }

  // Now let's load that state back in and continue running, and writing to our same csv file

  if (!pe->SerializeFromFile(state))
  {
    pe->GetLogger()->Error("Could not load state, check the error");
    return;
  }

  // Let's apply some interventions
  hemorrhageLeg.SetCompartment(eHemorrhage_Compartment::RightLeg);//the location of the hemorrhage
  hemorrhageLeg.GetSeverity().SetValue(0.0);// stop the bleeding
  pe->ProcessAction(hemorrhageLeg);

  // Now let's advance 5 minutes with these actions to get the patient's intervened state
  pe->AdvanceModelTime(5, TimeUnit::min);

  pe->GetLogger()->Info("Treated patient vitals");
  pe->GetDataRequestTracker().LogRequestedValues();
}
