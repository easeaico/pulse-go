/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "EngineHowTo.h"
#include "PulseEngine.h"

// Include the various types you will be using in your code
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/patient/actions/SERespiratoryMechanicsModification.h"
#include "cdm/system/physiology/SERespiratoryMechanicsModifiers.h"
#include "cdm/system/physiology/SERespiratorySystem.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarPressurePerVolume.h"
#include "cdm/properties/SEScalarPressureTimePerVolume.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarUnsigned.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerPressure.h"
#include "cdm/properties/SEScalarVolumePerTime.h"


//--------------------------------------------------------------------------------------------------
/// \brief
/// Custom driver settings for our Respiratory system
/// Drive respiration with your own driver
///
/// \details
/// Refer to the SERespiratoryMechanicsModifiers class
//--------------------------------------------------------------------------------------------------
void HowToRespiratoryMechanicsModification()
{
  // Create a Pulse Engine and load the standard patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/howto/HowTo_RespiratoryMechanicsModification.cpp.log");
  pe->GetLogger()->Info("HowTo_RespiratoryMechanicsModification");

  // With this engine, you do not initialize it, its already ready to go at construction time

  // You can load a previously saved state, but this is optional!
  if (!pe->SerializeFromFile("./states/StandardMale@0s.json"))// Select patient
  {
    pe->GetLogger()->Error("Could not load state, check the error");
    return;
  }

  // Setup data requests to write to a csv file so we can plot data
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("TidalVolume", VolumeUnit::mL);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit::mL);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("ExpiratoryRespiratoryResistance", PressureTimePerVolumeUnit::cmH2O_s_Per_L);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("InspiratoryRespiratoryResistance", PressureTimePerVolumeUnit::cmH2O_s_Per_L);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("RespiratoryCompliance", VolumePerPressureUnit::L_Per_cmH2O);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("TotalPulmonaryVentilation", VolumePerTimeUnit::L_Per_min);
  pe->GetEngineTracker()->GetDataRequestManager().SetResultsFilename("./test_results/howto/HowTo_RespiratoryMechanicsModification.cpp.csv");

  for (size_t i = 0; i < 6; i++)
  {
    AdvanceAndTrackTime_s(10, *pe);
    pe->GetEngineTracker()->LogRequestedValues();
  }

  SERespiratoryMechanicsModification config;
  SERespiratoryMechanicsModifiers& mechanics = config.GetModifiers();
  // By default, the engine will run a stabilization stage to get to a new homeostatis based on the provided modifiers
  // You can listen to the Stabilization event to see when the stabilization stage ends(and starts)
  // If you are slowly modifying the system with your own logic, and don't want the stabilization stage to run
  // set the incremental flag to true, and the engine apply this action and not run a stabilization stage
  mechanics.GetRespirationRateMultiplier().SetValue(1.2);
  mechanics.GetLungVolumeIncrement(eLungCompartment::LeftLung).SetValue(100.0, VolumeUnit::mL);
  pe->ProcessAction(config);

  for (size_t i = 0; i < 12; i++)
  {
    AdvanceAndTrackTime_s(10, *pe);
    pe->GetEngineTracker()->LogRequestedValues();
  }
}