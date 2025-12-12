/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "EngineHowTo.h"
#include "PulseEngine.h"

// Include the various types you will be using in your code
#include "engine/PulseConfiguration.h"
#include "cdm/engine/SEDynamicStabilization.h"
#include "cdm/engine/SEDynamicStabilizationEngineConvergence.h"
#include "cdm/engine/SEEngineConfiguration.h"
#include "cdm/engine/SEAdvanceUntilStable.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEDataRequestTracker.h"
#include "cdm/patient/actions/SEChronicObstructivePulmonaryDiseaseExacerbation.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolumePerTime.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// Usage of creating and running a scenario
///
/// \details
//--------------------------------------------------------------------------------------------------
void HowToRunToStable()
{
  // Create the engine and load the patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->LogToConsole(true);
  pe->GetLogger()->SetLogFile("./test_results/howto/HowToRunToStable.cpp/HowToRunToStable.log");
  pe->GetLogger()->Info("HowToRunToStable");

  // Create data requests for each value that should be written to the output log as the engine is executing
  SEDataRequestManager drMgr(pe->GetLogger());
  drMgr.CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("CardiacOutput", VolumePerTimeUnit::mL_Per_min);
  drMgr.CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("HemoglobinContent", MassUnit::g);
  drMgr.CreatePhysiologyDataRequest("InspiratoryExpiratoryRatio");
  drMgr.CreateGasCompartmentDataRequest(pulse::PulmonaryCompartment::Carina, "Inflow");
  drMgr.SetResultsFilename("./test_results/howto/HowToRunToStable.cpp/HowToRunToStable.csv");

  // Setup any custom convergence criteria BEFORE you call SerializeFromFile or InitializeEngine
  PulseConfiguration cfg;
  SEDynamicStabilizationEngineConvergence& cvg = cfg.GetDynamicStabilization().GetConvergenceCriteria("CustomCriteria");
  cvg.GetConvergenceTime().SetValue(15, TimeUnit::s);
  cvg.GetMinimumReactionTime().SetValue(25, TimeUnit::s);
  cvg.GetMaximumAllowedStabilizationTime().SetValue(1000, TimeUnit::s);
  cvg.CreatePropertyConvergence(drMgr.CreatePhysiologyDataRequest("HeartRate"), 4.0);
  cvg.CreatePropertyConvergence(drMgr.CreatePhysiologyDataRequest("CardiacOutput"), 4.0);
  cvg.CreatePropertyConvergence(drMgr.CreatePhysiologyDataRequest("MeanArterialPressure"), 4.0);
  cvg.CreatePropertyConvergence(drMgr.CreatePhysiologyDataRequest("SystolicArterialPressure"), 4.0);
  cvg.CreatePropertyConvergence(drMgr.CreatePhysiologyDataRequest("DiastolicArterialPressure"), 4.0);
  cvg.CreatePropertyConvergence(drMgr.CreatePhysiologyDataRequest("TidalVolume"), 4.0);
  cvg.CreatePropertyConvergence(drMgr.CreatePhysiologyDataRequest("OxygenSaturation"), 4.0);
  //pe->SetConfigurationOverride(&cfg);
  // TODO Custom criteria is overwriting the default criteria, need to update confing merge method

  if (!pe->SerializeFromFile("./states/StandardMale@0s.json", &drMgr))
  {
    pe->GetLogger()->Error("Could not load state, check the error");
    return;
  }

  SEChronicObstructivePulmonaryDiseaseExacerbation COPD;
  COPD.GetBronchitisSeverity().SetValue(0.5);
  COPD.GetEmphysemaSeverity(eLungCompartment::LeftLung).SetValue(0.7);
  COPD.GetEmphysemaSeverity(eLungCompartment::RightLung).SetValue(0.7);
  pe->ProcessAction(COPD);

  SEAdvanceUntilStable aus;
  //aus.SetCriteria("CustomCriteria");
  // If you don't want to create your own criteria...
  // There are several different stabilization criteria in the src/data/Data.xlsx and bin/config/DynamicStabilization.json
  // Names are the keys of the ConvergenceCriteria dict of the json file
  aus.SetCriteria("Chronic Obstructive Pulmonary Disease");
  // If you do not provide a criteria, the default AdvanceUntilStable criteria will be used
  pe->ProcessAction(aus);

  if (!pe->AdvanceModelTime(30, TimeUnit::s))// Note this tracker class takes in seconds
  {
    pe->GetLogger()->Fatal("Unable to advance engine time");
    return;
  }
}
