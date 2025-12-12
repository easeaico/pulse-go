/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "EngineHowTo.h"
#include "PulseEngine.h"

// Include the various types you will be using in your code
#include "cdm/engine/SEDataRequestTracker.h"
#include "cdm/scenario/SEScenario.h"
#include "cdm/scenario/SEScenarioLog.h"
#include "cdm/utils/FileUtils.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// Creating a scenario based on log file
///
/// \details
/// Read a log file to re-create the executed scenario
//--------------------------------------------------------------------------------------------------
void HowToScenarioFromLog()
{
  bool legacyLog = false;

  if (!legacyLog)
  {
    Logger log;
    log.LogToConsole(true);
    log.SetLogFile("./test_results/howto/HowTo_ScenarioFromLog.cpp.log");
    log.Info("HowTo_ScenarioFromLog");

    SEScenario sce(&log);
    SEScenarioLog sceL(&log);
    std::string logFilename = "./scenarios/ingmar.log";
    if (!sceL.Convert(logFilename, sce))
    {
      log.Error("Unable to convert scenario from log file : " + logFilename);
    }
    // Write out our scenario file
    std::string sceFilename = Replace(logFilename, ".log", ".sce.json");
    if (!sce.SerializeToFile(sceFilename))
    {
      log.Error("Unable to write scenario file : " + sceFilename);
    }
  }
  else
  {
    // Create scenario from a legacy log file
    // This functionality is not fully functional
    // If you are using this, you will probably need to extend the SEScenarioLogLegacy::NewAction method
    // The log format has been updated to to leverage more well formed messages we can easily read back in
    std::string filename = "./scenarios/pulse-cpr-bvm-2mins";
    std::string stateFilename = "./states/ECL_2_DeterioratingPatientCPR/CoarseVentricularFibrillation.json";
    SEScenarioLog::GenerateScenarioFromLegacyLog(filename + ".log", stateFilename, 300);
  }
}
