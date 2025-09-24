/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "EngineHowTo.h"
#include "PulseEngine.h"
#include "PulseScenarioExec.h"

// Include the various types you will be using in your code
#include "cdm/scenario/SEScenario.h"
#include "cdm/scenario/SEScenarioExec.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEAdvanceTime.h"
#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/engine/SEAdvanceHandler.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"


//--------------------------------------------------------------------------------------------------
/// \brief
/// A class used to handle any specific logic you may want to do each time step
///
/// \details
/// This method will be called at the end of EACH time step of the engine
/// The SEScenarioExecutor will process the advance time actions in a scenario and 
/// step the engine, calling this method each time step
//--------------------------------------------------------------------------------------------------
class MyCustomExec : public SEAdvanceHandler
{
public:
  MyCustomExec() : SEAdvanceHandler(false) { } // Nix callbacks during stabilization
  void OnAdvance(double time_s) override
  {
    // you are given the current scenairo time and the engine, so you can do what ever you want
  }
};

//--------------------------------------------------------------------------------------------------
/// \brief
/// Usage of creating and running a scenario
///
/// \details
//--------------------------------------------------------------------------------------------------
void HowToRunScenarios()
{
  Logger logger("./test_results/howto/HowTo-RunScenarios.cpp/HowToRunScenarios.log");
  logger.LogToConsole(true);

  // Let's make a scenario (you could just point the executor to a scenario json file on disk as well)
  SEScenario sce1(&logger);
  sce1.SetName("HowToRunScenarios1");
  sce1.SetDescription("Simple Scenario to demonstrate building a scenario by the CDM API");
  sce1.SetEngineStateFile("./states/StandardMale@0s.json");
  // Note you can set an Engine state, or create your own SEPatient object (see HowTo-CreateAPatient)
  // When filling out a data request, units are optional
  // The units will be set to whatever units the engine uses.
  // NOTE: The scenario makes it's own copy of these requests
  // Once you set it, any changes will not be reflected in the scenario
  // You can reuse this object for future actions
  sce1.GetDataRequestManager().CreatePhysiologyDataRequest("HeartRate",FrequencyUnit::Per_min);
  sce1.GetDataRequestManager().CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
  sce1.GetDataRequestManager().CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit::mL);
  sce1.GetDataRequestManager().SetResultsFilename("./test_results/howto/HowTo-RunScenarios.cpp/Scenario1.csv");

  // NOTE: the scenario will make it's own copy of this action
  // Once you set it, any changes will not be reflected in the scenario
  // You can reuse this object for future actions
  SEAdvanceTime adv;
  adv.GetTime().SetValue(2, TimeUnit::min);
  sce1.AddAction(adv);

  std::string sceOrigFilename = "./test_results/howto/HowTo-RunScenarios.cpp/Scenario1.json";
  if (!sce1.SerializeToFile(sceOrigFilename))
  {
    logger.Fatal("Failed to serialize scenario 1");
    return;
  }

  // Save data requests to a file
  std::string drFile = "./test_results/howto/HowTo-RunScenarios.cpp/DataRequests.json";
  if (!sce1.GetDataRequestManager().SerializeDataRequestsToFile(drFile))
  {
    logger.Fatal("Failed to serialize data requests");
    return;
  }

  // Create new scenario that uses data requests saved to file
  SEScenario sce2(&logger);
  sce2.SetName("HowToRunScenario2");
  sce2.SetDescription("Simple Scenario to demonstrate using data requests in a file");
  sce2.SetEngineStateFile("./states/StandardFemale@0s.json");
  sce2.GetDataRequestFiles().push_back(drFile);
  sce2.GetDataRequestManager().SetResultsFilename("./test_results/howto/HowTo-RunScenarios.cpp/Scenario2.csv");
  sce2.AddAction(adv);

  std::string sceRefFilename = "./test_results/howto/HowTo-RunScenarios.cpp/Scenario2.json";
  if (!sce2.SerializeToFile(sceRefFilename))
  {
    logger.Fatal("Failed to serialize scenario 2");
    return;
  }

  // You can run a single scenario, or a set of scenarios in series via the PulseScenarioExec object
  PulseScenarioExec execOpts(&logger);
  execOpts.SetScenarioFilename(sceRefFilename);
  if (!execOpts.Execute())
  {
    logger.Fatal("Failed to execute scenario with the file reference");
    return;
  }

  logger.Info("Executing scenarios in parallel.");
  // If you have several scenarios and want to run them in parallel, you can use this object for that as well
  // First write an exec status file
  std::string sceExecStatusFilename = "./test_results/howto/HowTo-RunScenarios.cpp/ScenarioExecStatus.json";
  std::vector<SEScenarioExecStatus> sces;
  sces.push_back(SEScenarioExecStatus());
  sces.push_back(SEScenarioExecStatus());
  sces[0].SetScenarioFilename(sceOrigFilename);
  sces[1].SetScenarioFilename(sceRefFilename);
  SEScenarioExecStatus::SerializeToFile(sces, sceExecStatusFilename, &logger);

  // Set up our executor
  execOpts.Clear(); // Need to clear out the previous run
  // Turn off console logging since we will be running many scenarios at the same time
  execOpts.LogToConsole(eSwitch::Off);
  // Tell the execOpts that we want to run that set of scenarios
  execOpts.SetScenarioExecListFilename(sceExecStatusFilename);
  // You can set the number of threads to process your set of scenarios.
  // -1 will figure out how many cores your machine has and create 1 less that that number of threads.
  // If you have less that that scenarios, we just create a thread per scenario.
  // In this case, 2 threads will be created to run 2 scenarios
  //execOpts.SetThreadCount(-1);
  // Run them
  if (!execOpts.Execute())
  {
    logger.Fatal("Failed to execute scenario with the file reference");
    return;
  }
  // Now read the stats back in and make sure everything ran ok
  std::vector<SEScenarioExecStatus> sts;
  SEScenarioExecStatus::SerializeFromFile(sceExecStatusFilename, sts, &logger);
  for (SEScenarioExecStatus s : sts)
  {
    if (s.GetEngineInitializationState() != eEngineInitializationState::Initialized)
    {
      logger.Error("Unable to initialize scenario: " + s.GetScenarioFilename());
      continue;
    }
    if (s.GetScenarioExecutionState() != eScenarioExecutionState::Complete)
    {
      logger.Error("Error running scenario: " + s.GetScenarioFilename());
      continue;
    }
    logger.Info("Successfully ran scenario " + s.GetScenarioFilename());
    logger.Info("\t CSV: " + s.GetCSVFilename());
    logger.Info("\t Log: " + s.GetLogFilename());
  }
}
