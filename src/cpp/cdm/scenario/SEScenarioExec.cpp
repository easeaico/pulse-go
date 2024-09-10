/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/scenario/SEScenarioExec.h"
#include "cdm/scenario/SEScenario.h"
#include "cdm/scenario/SEScenarioLog.h"
#include "cdm/io/protobuf/PBScenario.h"
#include "cdm/engine/SEAction.h"
#include "cdm/engine/SECondition.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/engine/SEAdvanceTime.h"
#include "cdm/engine/SESerializeState.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEEventManager.h"
#include "cdm/PhysiologyEngine.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/engine/SEEngineConfiguration.h"
#include "cdm/patient/SEPatient.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/utils/TimingProfile.h"
#include "cdm/utils/ConfigParser.h"
#include "cdm/utils/FileUtils.h"
#include "cdm/utils/GeneralMath.h"

SEScenarioExec::SEScenarioExec(Logger* logger) : Loggable(logger)
{
  Clear();
}

SEScenarioExec::~SEScenarioExec()
{
  Clear();
}

void SEScenarioExec::Clear()
{
  m_LogToConsole = eSwitch::NullSwitch;

  m_DataRootDirectory = ".";
  m_OutputRootDirectory = "";
  m_OrganizeOutputDirectory = eSwitch::Off;
  m_RelativeSerialization = eRelativeSerialization::ToWorkingDir;

  m_BaseFilename = "";
  m_LogFilename = "";
  m_DataRequestCSVFilename = "";

  m_EngineConfigurationContent = "";
  m_EngineConfigurationFilename = "";

  m_ScenarioContent = "";
  m_ScenarioFilename = "";
  m_ScenarioDirectory = "";
  m_ScenarioExecListFilename = "";
  m_ContentFormat = eSerializationFormat::JSON;

  m_ScenarioLogFilename = "";
  m_ScenarioLogDirectory = "";

  m_DataRequestFilesSearch.clear();

  m_ThreadCount = -1;// One less that number of threads the system supports

  m_SaveNextStep = false;
  m_AutoSerializeFilename = "";
  m_AutoSerializeBaseFilename = "";
  m_AutoSerializeFilenameExt = ".json";
  m_AutoSerializeAfterActions = eSwitch::Off;
  m_AutoSerializePeriod_s = 0;
  m_AutoSerializeTime_s = 0;
  m_TimeStampSerializedStates = eSwitch::Off;
  m_ReloadSerializedState = eSwitch::On;
  m_SerializationOutput.str("");
  m_SerializationActions.str("");
}

void SEScenarioExec::Copy(const SEScenarioExec& src)
{
  PBScenario::Copy(src, *this);
}

bool SEScenarioExec::SerializeToString(std::string& output, eSerializationFormat m) const
{
  return PBScenario::SerializeToString(*this, output, m, GetLogger());
}
bool SEScenarioExec::SerializeFromString(const std::string& src, eSerializationFormat m)
{
  return PBScenario::SerializeFromString(src, *this, m, GetLogger());
}

void SEScenarioExec::SetOutputRootDirectory(const std::string& d)
{
  m_OutputRootDirectory = d;
  if (!m_OutputRootDirectory.empty())
  {
    std::replace(m_OutputRootDirectory.begin(), m_OutputRootDirectory.end(), '\\', '/');
    if (m_OutputRootDirectory.back() != '/')
      m_OutputRootDirectory += "/";
  }
}

bool SEScenarioExec::Execute(PhysiologyEngine& pe, SEScenario& sce, SEScenarioExecStatus* status)
{
  return Process(pe, sce, status);
}
bool SEScenarioExec::Process(PhysiologyEngine& pe, SEScenario& sce, SEScenarioExecStatus* status)
{
  std::string sceRelPath = "";
  std::string scenarioDir = "";

  // Try to read our config file to identify scenario directory path
  ConfigSet* config = ConfigParser::FileToConfigSet("run.config");
  if (config->HasKey("scenario_dir"))
  {
    scenarioDir = config->GetValue("scenario_dir");
    m_DataRequestFilesSearch.insert(scenarioDir);
  }
  delete config;
  std::string csvFilenamePostfix = "";
  if (sce.GetDataRequestManager().HasResultsFilename())
  {
    // This is where we put results/logs if specified
    std::string ext;
    std::string resultsFilename = sce.GetDataRequestManager().GetResultFilename();
    // If we are given a directory, make the csv file the scenario name + 'Results' in that directory
    if (resultsFilename.back() == '/' || resultsFilename.back() == '\\')
    {
      csvFilenamePostfix = "Results";
      m_BaseFilename = sce.GetName();
      m_OutputRootDirectory = resultsFilename;
    }
    else
    {
      if (!m_OutputRootDirectory.empty() &&
         (resultsFilename.rfind("./") == 0 || resultsFilename.rfind(".\\") == 0))
        resultsFilename = m_OutputRootDirectory + "/" + resultsFilename.substr(2);
      SplitPathFilenameExt(resultsFilename, m_OutputRootDirectory, m_BaseFilename, ext);
    }
  }
  else if (!m_ScenarioFilename.empty())
  {
    std::string ext;
    // Set the output to the same location as the scenario file
    if (m_OutputRootDirectory.empty())
      SplitPathFilenameExt(m_ScenarioFilename, m_OutputRootDirectory, m_BaseFilename, ext);
    else
      SplitFilenameExt(m_ScenarioFilename, m_BaseFilename, ext);

    // If this scenario file is in our source scenario files,
    // let's write the output in the same place our testing framework does
    if (!scenarioDir.empty())
    {
      sceRelPath = RelativePathFrom(scenarioDir, m_ScenarioFilename);
      if (!sceRelPath.empty())
        m_OutputRootDirectory = "./test_results/scenarios/" + sceRelPath;
    }
    csvFilenamePostfix = "Results";
  }
  else if (sce.HasName())
  {
    m_BaseFilename = sce.GetName();
    if (sce.GetName().find("./") == 0)
    {
      m_BaseFilename = sce.GetName().substr(sce.GetName().find_last_of("/")+1);
      m_OutputRootDirectory = sce.GetName().substr(0, sce.GetName().find_last_of("/"));
    }
    else if (m_OutputRootDirectory.empty())
    {
      m_OutputRootDirectory = "./test_results/scenarios/";
    }
    csvFilenamePostfix = "Results";
  }
  else
  {
    m_BaseFilename = "Pulse";
    m_OutputRootDirectory = "./test_results/scenarios";
    sce.Warning("Unable to name scenario output, writing log to './Pulse.log'");
    csvFilenamePostfix = "Results";
  }

  if (m_OrganizeOutputDirectory==eSwitch::On)
  {
    std::string relativePath = "";
    if (!m_ScenarioDirectory.empty())
    {
      // Get the relative directory path from to the scenario directory to the scenario file
      relativePath = RelativePathFrom(m_ScenarioDirectory, m_ScenarioFilename);
    }
    // Append it to our m_OutputRootDirectory
    m_OutputRootDirectory += "/" + relativePath + "/" + m_BaseFilename;
  }

  m_LogFilename = m_OutputRootDirectory + "/" + m_BaseFilename + ".log";
  m_DataRequestCSVFilename = m_OutputRootDirectory + "/" + m_BaseFilename + csvFilenamePostfix + ".csv";
  if (m_AutoSerializeAfterActions == eSwitch::On)
  {
    if (m_ReloadSerializedState == eSwitch::Off)
    {
      m_LogFilename = m_OutputRootDirectory + "/" + m_BaseFilename + ".orig.log";
      m_DataRequestCSVFilename = m_OutputRootDirectory + "/" + m_BaseFilename + "Results.orig.csv";
      m_AutoSerializeBaseFilename = "./test_results/autoserialization/" + sceRelPath + "/" + m_BaseFilename + "/ReloadOff/";
    }
    else
      m_AutoSerializeBaseFilename = "./test_results/autoserialization/" + sceRelPath + "/" + m_BaseFilename + "/ReloadOn/";
  }

  sce.Info("Creating Log File : " + m_LogFilename);
  pe.GetLogger()->SetLogFile(m_LogFilename);
  pe.GetLogger()->LogToConsole(m_LogToConsole == eSwitch::On);

  if (status)
  {
    status->SetLogFilename(m_LogFilename);
    status->SetCSVFilename(m_DataRequestCSVFilename);
  }

  if(!sce.IsValid())
  {
    pe.GetLogger()->Error("Invalid Scenario");
    return false;
  }

  sce.ProcessDataRequestFiles(m_DataRequestFilesSearch);

  try
  {
    // Initialize the engine with a state or initial parameters
    if (sce.HasEngineStateFile())
    {
      // Check and see if the state file is relative
      std::string state = sce.GetEngineStateFile();
      if (IsRelativePath(state))
      {
        // If it is not relative from the working directory
        // Is it relative from the m_OutputRootDirectory
        if (!FileExists(state))
        {
          if (state.rfind("./", 0) == 0)
            state = state.substr(2);
          state = m_OutputRootDirectory + state;
        }
      }
      if (!pe.SerializeFromFile(state))
      {
        if (status)
          status->SetEngineInitializationState(eEngineInitializationState::FailedState);
        pe.GetLogger()->Error("Unable to load state file: "+ state);
        return false;
      }
      // WE ARE OVERWRITING ANY DATA REQUESTS IN THE STATE WITH WHATS IN THE SCENARIO!!!
      pe.GetEngineTracker()->GetDataRequestManager().Copy(sce.GetDataRequestManager());
      if (sce.GetDataRequestManager().HasDataRequests())
      {
        remove(m_DataRequestCSVFilename.c_str());
        sce.Info("Creating CSV File : " + m_DataRequestCSVFilename);
        pe.GetEngineTracker()->GetDataRequestManager().SetResultsFilename(m_DataRequestCSVFilename);
      }
      pe.GetEngineTracker()->TrackData(pe.GetSimulationTime(TimeUnit::s));
    }
    else if (sce.HasPatientConfiguration())
    {
      sce.GetPatientConfiguration().SetDataRoot(m_DataRootDirectory);
      // Make a copy of the data requests, note this clears out data requests from the engine
      pe.GetEngineTracker()->GetDataRequestManager().Copy(sce.GetDataRequestManager());
      if (sce.GetDataRequestManager().HasDataRequests())
      {
        remove(m_DataRequestCSVFilename.c_str());
        sce.Info("Creating CSV File : " + m_DataRequestCSVFilename);
        pe.GetEngineTracker()->GetDataRequestManager().SetResultsFilename(m_DataRequestCSVFilename);
      }
      if (!pe.InitializeEngine(sce.GetPatientConfiguration()))
      {
        if (status)
          status->SetEngineInitializationState(pe.GetInitializationState());
        pe.GetLogger()->Error("Unable to initialize engine");
        return false;
      }
      if (status)
        status->SetStabilizationTime_s(pe.GetStabilizationTime(TimeUnit::s));
    }
    else
    {
      if (status)
        status->SetEngineInitializationState(eEngineInitializationState::FailedState);
      pe.GetLogger()->Error("No initial engine parameters set");
      return false;
    }
    if (status)
      status->SetEngineInitializationState(eEngineInitializationState::Initialized);

    return ProcessActions(pe, sce, status);
  }
  catch (CommonDataModelException& ex)
  {
    if (status)
      status->SetFatalRuntimeError(true);
    pe.GetLogger()->Fatal(ex.what());
    return false;
  }
  catch (std::exception& ex)
  {
    if (status)
      status->SetFatalRuntimeError(true);
    pe.GetLogger()->Fatal(ex.what());
    return false;
  }
  catch (...)
  {
    if (status)
      status->SetFatalRuntimeError(true);
    pe.GetLogger()->Fatal("Caught unknown exception, ending simulation");
    return false;
  }
  return true;
}

bool SEScenarioExec::ProcessActions(PhysiologyEngine& pe, SEScenario& sce, SEScenarioExecStatus* status)
{
  std::stringstream ss;
  pe.GetLogger()->Info("Executing Scenario");
  pe.GetLogger()->AddForward(status);

  double dT_s=pe.GetTimeStep(TimeUnit::s);
  double scenarioTime_s = 0;
  double statusTime_s = 0;// Current time of this status cycle
  double statusStep_s = 60;//How long did it take to simulate this much time

  double sampleTime_s = sce.GetDataRequestManager().GetSamplesPerSecond();
  if (sampleTime_s != 0)
    sampleTime_s = 1 / sampleTime_s;
  double currentSampleTime_s = 0;

  TimingProfile profiler;
  profiler.Start("Total");
  profiler.Start("Status");

  bool err=false;
  const SEAdvanceTime* adv;
  double expectedFinalSimTime_s = 0;
  double spareAdvanceTime_s = 0;
  for (SEAction* a : sce.GetActions())
  {
    if (!a) continue;
    // We override advance time actions in order to advance and
    // pull requested data at each time step, all other actions
    // will be processed by the engine
    adv=dynamic_cast<const SEAdvanceTime*>(a);
    if (adv!=nullptr)
    {
      pe.GetLogger()->Info("[Action]\n" + a->ToJSON());
      expectedFinalSimTime_s += adv->GetTime(TimeUnit::s);

      double time_s = adv->GetTime(TimeUnit::s) + spareAdvanceTime_s;
      int count = (int)(time_s/dT_s);
      spareAdvanceTime_s = time_s - (count * dT_s);
      for (int i=0;i<count;i++)
      {
        AdvanceEngine(pe);

        // Pull data from the engine
        scenarioTime_s = pe.GetSimulationTime(TimeUnit::s);
        currentSampleTime_s += dT_s;
        if (currentSampleTime_s >= sampleTime_s)
        {
          currentSampleTime_s = 0;
          pe.GetEngineTracker()->TrackData(scenarioTime_s);
        }
        statusTime_s += dT_s;
        // How are we running?
        if (statusTime_s>statusStep_s)
        {
          statusTime_s = 0;
          ss << "Current Time is " << scenarioTime_s << "s, it took " << profiler.GetElapsedTime_s("Status") << "s to simulate the past " << statusStep_s << "s" << std::flush;
          profiler.Reset("Status");
          pe.GetLogger()->Info(ss);
        }
        if(pe.GetEventManager().IsEventActive(eEvent::IrreversibleState))
          return false;// Patient is for all intents and purposes dead, or out at least out of its methodology bounds, quit running
      }
      continue;
    }

    if(!ProcessAction(pe, *a))
      err=true;

    if(pe.GetEventManager().IsEventActive(eEvent::IrreversibleState))
      return false;// Patient is for all intents and purposes dead, or out at least out of its methodology bounds, quit running
  }

  ss << "It took " << profiler.GetElapsedTime_s("Total") << "s to run this simulation";
  profiler.Clear();
  pe.GetLogger()->Info(ss);

  double simTime_s = pe.GetSimulationTime(TimeUnit::s);
  pe.GetLogger()->Info("[Final SimTime] " + pulse::cdm::to_string(simTime_s)+"(s)");
  pe.GetLogger()->Info("[Expected Final SimTime] " + pulse::cdm::to_string(expectedFinalSimTime_s)+"(s)");
  if (GeneralMath::PercentDifference(expectedFinalSimTime_s, simTime_s) > 0.01)
  {
    err = true;
    pe.GetLogger()->Error("!!!! Simulation time does not equal expected end time !!!!");
  }
  if (status)
    status->SetFinalSimulationTime_s(simTime_s);

  return !err;
}

bool SEScenarioExec::ProcessAction(PhysiologyEngine& pe, SEAction& action)
{
  if (m_AutoSerializeAfterActions == eSwitch::On && m_SerializationActions.str().empty())
  {
    m_SerializationActions << "AfterActions";
  }
  SESerializeState* ss = dynamic_cast<SESerializeState*>(&action);
  if (ss != nullptr && ss->GetMode() == eSerialization_Mode::Save)
  {
    if (!ss->HasFilename())
    {
      ss->SetFilename("./states/"+pe.GetPatient().GetName()+
        "@"+pulse::cdm::to_string(pe.GetSimulationTime(TimeUnit::s))+"s.json");
    }
    else
    {
      // If its relative, we add the serialization directory
      if (IsRelativePath(ss->GetFilename()))
      {
        switch (m_RelativeSerialization)
        {
        case eRelativeSerialization::ToWorkingDir:
        {
          break;
        }
        case eRelativeSerialization::ToOutputDir:
        {
          ss->SetFilename(m_OutputRootDirectory + ss->GetFilename().substr(2));
          break;
        }
        case eRelativeSerialization::ToScenarioDir:
        {
          Error("TODO: Implement relative to scenario dir");
          break;
        }
        }
      }
    }
  }
  return pe.ProcessAction(action);
}

void SEScenarioExec::AdvanceEngine(PhysiologyEngine& pe)
{
  if (m_AutoSerializePeriod_s > 0)
  {
    m_AutoSerializeTime_s += pe.GetTimeStep(TimeUnit::s);
    if (m_AutoSerializeTime_s >= m_AutoSerializePeriod_s)
    {
      m_AutoSerializeTime_s = 0;
      m_SerializationOutput.str("");
      m_SerializationOutput << m_AutoSerializeBaseFilename;
      if (m_TimeStampSerializedStates == eSwitch::On)
        m_SerializationOutput << "@" << pe.GetSimulationTime(TimeUnit::s);
      pe.GetLogger()->Info("Serializing state after requested period");
      pe.SerializeToFile(m_SerializationOutput.str() + m_AutoSerializeFilenameExt);
      if (m_ReloadSerializedState == eSwitch::On)
      {
        pe.SerializeFromFile(m_SerializationOutput.str() + m_AutoSerializeFilenameExt);
        pe.SerializeToFile(m_SerializationOutput.str() + ".Reloaded" + m_AutoSerializeFilenameExt);
      }
    }
  }
  pe.AdvanceModelTime();
  if (m_SaveNextStep)
  {
    m_SaveNextStep = false;
    m_SerializationOutput.str("");
    m_SerializationOutput << m_AutoSerializeBaseFilename;
    if (m_TimeStampSerializedStates == eSwitch::On)
      m_SerializationOutput << "@" << pe.GetSimulationTime(TimeUnit::s);
    pe.GetLogger()->Info("Serializing state again (after the next timestep)");
    pe.SerializeToFile(m_SerializationOutput.str() + m_AutoSerializeFilenameExt);
  }
  if (m_SerializationActions.str().length() > 0)
  {
    m_SaveNextStep = true;
    m_SerializationOutput.str("");
    m_SerializationOutput << m_AutoSerializeBaseFilename << m_SerializationActions.str();
    if (m_TimeStampSerializedStates == eSwitch::On)
      m_SerializationOutput << "@" << pe.GetSimulationTime(TimeUnit::s);
    pe.GetLogger()->Info("Serializing state after action");
    pe.SerializeToFile(m_SerializationOutput.str() + m_AutoSerializeFilenameExt);
    if (m_ReloadSerializedState == eSwitch::On)
    {
      pe.GetLogger()->Info("Reloading and saving reloaded state");
      pe.SerializeFromFile(m_SerializationOutput.str() + m_AutoSerializeFilenameExt);
      pe.SerializeToFile(m_SerializationOutput.str() + ".Reloaded" + m_AutoSerializeFilenameExt);
    }
    m_SerializationActions.str("");
  }
}

bool SEScenarioExec::ConvertLog()
{
  std::string ext;
  // Set the output to the same location as the log file
  if (m_OutputRootDirectory.empty())
    SplitPathFilenameExt(m_ScenarioLogFilename, m_OutputRootDirectory, m_BaseFilename, ext);
  else
    SplitFilenameExt(m_ScenarioLogFilename, m_BaseFilename, ext);

  if (m_OrganizeOutputDirectory==eSwitch::On)
  {
    std::string relativePath = "";
    if (!m_ScenarioLogDirectory.empty())
    {
      // Get the relative directory path from to the scenario directory to the scenario file
      relativePath = RelativePathFrom(m_ScenarioLogDirectory, m_ScenarioLogFilename);
    }
    // Append it to our m_OutputRootDirectory
    m_OutputRootDirectory += relativePath;
    if (m_OutputRootDirectory.back() != '/')
      m_OutputRootDirectory += "/";
  }

  Info("Converting : " + m_ScenarioLogFilename);
  std::string outScenarioFilename = m_OutputRootDirectory + m_BaseFilename + ".json";

  SEScenario sce(GetLogger());
  SEScenarioLog sceL(GetLogger());

  if (!sceL.Convert(m_ScenarioLogFilename, sce))
  {
    Error("Unable to convert scenario from log file: " + m_ScenarioLogFilename);
    return false;
  }

  if (!sce.SerializeToFile(outScenarioFilename))
  {
    Error("Unable to serialize scenario from log file : " + m_ScenarioLogFilename);
    return false;
  }
  Info("Successfully wrote scenario file: " + outScenarioFilename);

  return true;
}

SEScenarioExecStatus::SEScenarioExecStatus()
{
  Clear();
}
SEScenarioExecStatus::~SEScenarioExecStatus()
{

}

void SEScenarioExecStatus::Clear()
{
  ClearStatus();
  m_ScenarioFilename = "";
}

void SEScenarioExecStatus::ClearStatus()
{
  SEEngineInitializationStatus::Clear();
  m_ScenarioExecutionState = eScenarioExecutionState::Waiting;
  m_RuntimeError = false;
  m_FatalRuntimeError = false;
  m_FinalSimulationTime_s = 0;
}

void SEScenarioExecStatus::Copy(const SEScenarioExecStatus& from)
{
  PBScenario::Copy(from, *this);
}
bool SEScenarioExecStatus::SerializeToString(std::string& output, eSerializationFormat m, Logger* logger) const
{
  return PBScenario::SerializeToString(*this, output, m, logger);
}
bool SEScenarioExecStatus::SerializeFromString(const std::string& src, eSerializationFormat m, Logger* logger)
{
  return PBScenario::SerializeFromString(src, *this, m, logger);
}

bool SEScenarioExecStatus::SerializeToFile(const std::vector<SEScenarioExecStatus>& src, const std::string& filename, Logger* logger)
{
  return PBScenario::SerializeToFile(src, filename, logger);
}
bool SEScenarioExecStatus::SerializeFromFile(const std::string& filename, std::vector<SEScenarioExecStatus>& dst, Logger* logger)
{
  return PBScenario::SerializeFromFile(filename, dst, logger);
}

