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
  drMgr.CreateGasCompartmentDataRequest(pulse::PulmonaryCompartment::Carina, "Inflow");
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

  // NOTE: Appending/Truncating data to csv files with multiple runs (the same or different engine) is not well defined
  // We have not had a use case where managing and reusing engines running multiple related states
  // Ex. Using the same engine with multiple states with different simulation times all needing to create a single continuous csv file
  // Ex. What if 2 different engines want to share (via serial execution) a csv file?
  // Maybe you have a different use case...that needs different csv control... let us know!
  // Might need to add a param on the data request manager to specify truc/append to the results file

  double lastSimTime_s = pe->GetSimulationTime(TimeUnit::s);

  // Now let's load that state back in and continue running, and writing to our same csv file
  // By not providing a data request manager, you are deferring to use the data request manager in the state
  // A provided data request manager to SerializeFromFile will override the state data request manager
  // 
  // The engine instance keeps all csv file handles and will append data to those file
  // Files contents are cleared when initially opened, and appended to, even after loading a new state with the same csv file

  if (!pe->SerializeFromFile(state))
  {
    pe->GetLogger()->Error("Could not load state, check the error");
    return;
  }
  // If you do not wish to track anything, reset the tracker
  // pe->GetDataRequestTracker().Reset();

  // You can close the tracked file and stop tracking
  //pe->GetDataRequestTracker().CloseResultsFile();

  // You can change what you track and to what file at any time
  //pe->GetDataRequestTracker().SetupDataRequests(drMgr);
  
  // TODO Should these 3 options reset the contents of the csv file if the same csv is used again?
  // Maybe add a bool param to 'forget' file handles so csv files are tructated on true, appended on false?

  // The engine simulation time is contained in the state, and the engine sim time will be set to this time
  // You can override the simulation time if you wish after loading the state
  //SEScalarTime newTime;
  //newTime.SetValue(lastSimTime_s + 300, TimeUnit::s);
  //pe->SetSimulationTime(newTime);

  // Let's apply some interventions
  hemorrhageLeg.SetCompartment(eHemorrhage_Compartment::RightLeg);//the location of the hemorrhage
  hemorrhageLeg.GetSeverity().SetValue(0.0);// stop the bleeding
  pe->ProcessAction(hemorrhageLeg);

  // Now let's advance 5 minutes with these actions to get the patient's intervened state
  pe->AdvanceModelTime(5, TimeUnit::min);

  pe->GetLogger()->Info("Treated patient vitals");
  pe->GetDataRequestTracker().LogRequestedValues();
}
