/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "EngineHowTo.h"
#include "PulseEngine.h"

// Include the various types you will be using in your code
#include "cdm/engine/SEDataRequestTracker.h"
#include "cdm/scenario/SEScenario.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/engine/SEAdvanceTime.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolumePerTime.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// Creating an engine based on a scenario file
///
/// \details
/// Read a scenario file and pull things out for your use case
//--------------------------------------------------------------------------------------------------
void HowToScenarioBase()
{
  // Create our engine
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/howto/HowTo_ScenarioBase.cpp/HowTo_ScenarioBase.log");
  pe->GetLogger()->Info("HowTo_ScenarioBase");

  // Create data requests for each value that should be written to the output log as the engine is executing
  SEDataRequestManager drMgr(pe->GetLogger());
  drMgr.CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("CardiacOutput", VolumePerTimeUnit::mL_Per_min);
  drMgr.CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);


  // Make a copy of the data requests, note this clears out data requests from the engine
  // This will clear out the data requests if any exist in the DataRequestManager
  drMgr.SerializeToFile("./test_results/howto/HowTo_ScenarioBase.cpp/DataRequestsFile.json");

  // NOTE : You can just make a DataRequests file that holds only data requests
  // And serialize that in instead of a scenario file, if all you want is a consistent set of requests
  // This will clear out any requests already in the object
  drMgr.SerializeFromFile("./test_results/howto/HowTo_ScenarioBase.cpp/DataRequestsFile.json");
  // Don't need to delete drData as obj is wrapped in a unique_ptr

  if (!drMgr.HasResultsFilename())
    drMgr.SetResultsFilename("./test_results/howto/HowTo_ScenarioBase.cpp/HowTo_ScenarioBase.csv");

  //Let's read the scenario we want to base this engine on
  SEScenario sce(pe->GetLogger());
  sce.SerializeFromFile("./test_results/howto/HowTo_ScenarioBase.cpp/HowTo_ScenarioBase.json");

  if (sce.HasEngineStateFile())
  {
    if (!pe->SerializeFromFile("./states/StandardMale@0s.json", &drMgr))
    {
      pe->GetLogger()->Error("Could not load state, check the error");
      return;
    }
  }
  else if (sce.HasPatientConfiguration())
  {
    if (!pe->InitializeEngine(sce.GetPatientConfiguration(), &drMgr))
    {
      pe->GetLogger()->Error("Could not load patient configuration, check the error");
      return;
    }
  }

  const SEAdvanceTime* adv;
  // Now run the scenario actions
  for (const SEAction* a : sce.GetActions())
  {
    // We want the tracker to process an advance time action so it will write each time step of data to our track file
    adv = dynamic_cast<const SEAdvanceTime*>(a);
    if (adv != nullptr)
      pe->AdvanceModelTime(adv->GetTime(TimeUnit::s), TimeUnit::s);// you could just do pe->AdvanceModelTime without tracking timesteps
    else
      pe->ProcessAction(*a);
  }

  // At this point your engine is where you want it to be
  // You could read in a new scenario and run it's actions 
  // or programatically add actions as your applications sees fit

}
