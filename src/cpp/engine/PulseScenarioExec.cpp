/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "PulseConfiguration.h"
#include "PulseScenario.h"
#include "PulseScenarioExec.h"
#include "engine/common/Engine.h"
#include "engine/io/protobuf/PBScenario.h"

#include "cdm/engine/SEAction.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/utils/FileUtils.h"
#include "cdm/utils/ConfigParser.h"
#include "cdm/utils/ThreadPool.h"
#include "cdm/utils/TimingProfile.h"


void PulseScenarioExec::Clear()
{
  SEScenarioExec::Clear();
  m_ModelType = eModelType::HumanAdultWholeBody;
  m_Statuses.clear();
  m_Threads.clear();
}

void PulseScenarioExec::Copy(const PulseScenarioExec& src)
{
  pulse::PBScenario::Copy(src, *this);
}

bool PulseScenarioExec::SerializeToString(std::string& output, eSerializationFormat m, Logger* logger) const
{
  return pulse::PBScenario::SerializeToString(*this, output, m, logger);
}
bool PulseScenarioExec::SerializeFromString(const std::string& src, eSerializationFormat m, Logger* logger)
{
  return pulse::PBScenario::SerializeFromString(src, *this, m, logger);
}

bool PulseScenarioExec::Execute()
{
  std::string scenarioPath, scenarioFilename;
  if (!GetScenarioContent().empty())
  {
    if (m_LogToConsole == eSwitch::NullSwitch)
      m_LogToConsole = eSwitch::On;

    PulseScenario sce(GetLogger(), GetDataRootDirectory());
    if (!sce.SerializeFromString(GetScenarioContent(), GetContentFormat()))
      return false;
    return Execute(sce);
  }
  else if (!GetScenarioFilename().empty())
  {
    if (m_LogToConsole == eSwitch::NullSwitch)
      m_LogToConsole = eSwitch::On;

    SEScenarioExecStatus status;
    PulseScenario sce(GetLogger(), GetDataRootDirectory());
    SplitPathFilename(GetScenarioFilename(), scenarioPath, scenarioFilename);
    GetDataRequestFilesSearch().insert(scenarioPath);
    if (!sce.SerializeFromFile(GetScenarioFilename()))
      return false;
    return Execute(sce, &status);
  }
  else if (!GetScenarioDirectory().empty() || !GetScenarioExecListFilename().empty())
  {
    TimingProfile profiler;
    profiler.Start("Total");
    if (m_LogToConsole == eSwitch::NullSwitch)
      m_LogToConsole = eSwitch::Off;

    m_Statuses.clear();
    if (!GetScenarioExecListFilename().empty())
    {
      if (!SEScenarioExecStatus::SerializeFromFile(GetScenarioExecListFilename(), m_Statuses, GetLogger()))
      {
        Fatal("Unable to serialize ScenarioExecListFilename " + GetScenarioExecListFilename());
        return false;
      }
    }
    else if (!GetScenarioDirectory().empty())
    {
      SEScenarioExecStatus status;
      std::vector<std::string> scenarios;
      ListFiles(GetScenarioDirectory(), scenarios, true, ".json");
      for (auto filename : scenarios)
      {
        status.SetScenarioFilename(filename);
        m_Statuses.push_back(status);
      }
      SetScenarioExecListFilename("./test_results/scenarios/ScenarioDirectoryStatuses.json");
      SEScenarioExecStatus::SerializeToFile(m_Statuses, m_ScenarioExecListFilename, GetLogger());
      if (m_Statuses.empty())
      {
        Info("No scenarios listed in provided scenario exec list file");
        return true;
      }
    }
    size_t numThreadsToUse = ComputeNumThreads();
    if (numThreadsToUse <= 0)
    {
      Fatal("Unable to compute the number of threads to use");
      return false;
    }
    if (numThreadsToUse > m_Statuses.size())
      numThreadsToUse = m_Statuses.size();

    // How many scenarios do we need to run
    m_Completed = 0;
    for (auto& status : m_Statuses)
    {
      if (status.GetScenarioExecutionState() == eScenarioExecutionState::Complete)
        m_Completed++;
    }
    size_t ToExecute = m_Statuses.size() - m_Completed;
    Info("We need to run " + std::to_string(ToExecute) + "/" + std::to_string(m_Statuses.size()) + " scenarios");
    if (ToExecute == 0)
    {
      Info("Nothing to do");
      return true;
    }

    std::string copy;
    SerializeToString(copy, eSerializationFormat::JSON, GetLogger());
    for (size_t p = 0; p < numThreadsToUse; p++)
    {
      m_Threads.push_back(std::thread(&PulseScenarioExec::ControllerLoop,
        copy, &m_Mutex, &m_Statuses, &m_Completed, m_ScenarioExecListFilename, GetLogger()));
    }
    for (size_t p = 0; p < numThreadsToUse; p++)
      m_Threads[p].join();


    Info("It took " + pulse::cdm::to_string(profiler.GetElapsedTime_s("Total")) + "s to run these scenarios");
    profiler.Clear();
    return true;
  }
  else if (!GetScenarioLogFilename().empty())
  {
    if (m_LogToConsole == eSwitch::NullSwitch)
      m_LogToConsole = eSwitch::On;
    return ConvertLog();
  }
  else if (!GetScenarioLogDirectory().empty())
  {
    if (m_LogToConsole == eSwitch::NullSwitch)
      m_LogToConsole = eSwitch::Off;

    size_t numThreadsToUse = ComputeNumThreads();
    if (numThreadsToUse <= 0)
    {
      Fatal("Unable to compute the number of threads to use");
      return false;
    }
    // Let's get all the scenarios and create a thread pool
    std::vector<std::string> logs;
    ListFiles(GetScenarioLogDirectory(), logs, true, ".log", ".cnv.log");
    // Let's not kick off more threads than we need
    if (numThreadsToUse > logs.size())
      numThreadsToUse = logs.size();

    ThreadPool pool(numThreadsToUse);
    std::vector<std::future<bool>> futures;
    std::vector<PulseScenarioExec*> opts;
    for (auto filename : logs)
    {
      PulseScenarioExec* pse = new PulseScenarioExec(GetLogger());
      opts.emplace_back(pse);
      pse->Copy(*this);
      pse->SetScenarioLogFilename(filename);
      futures.emplace_back(pool.enqueue([&pse]() -> bool { return pse->ConvertLog(); }));
    }

    bool success = true;
    for (size_t i = 0; i < futures.size(); ++i)
    {
      if (!futures[i].get())
      {
        Error("Failed to convert " + opts[i]->m_ScenarioLogFilename);
        success = false;
      }
    }

    for ( PulseScenarioExec* o: opts)
      delete o;

    return success;
  }

  Error("No scenario content/log provided");
  return false;
}

bool PulseScenarioExec::Execute(PulseScenario& sce, SEScenarioExecStatus* status)
{
  auto pe = CreatePulseEngine(m_ModelType, sce.GetLogger());

  if (!GetEngineConfigurationFilename().empty())
  {
    PulseConfiguration pc(sce.GetLogger());
    if (!pc.SerializeFromFile(GetEngineConfigurationFilename(), sce.GetSubstanceManager()))
      return false;
    sce.GetConfiguration().Merge(pc, sce.GetSubstanceManager());
  }
  else if (!GetEngineConfigurationContent().empty())
  {
    PulseConfiguration pc(sce.GetLogger());
    if (!pc.SerializeFromString(GetEngineConfigurationFilename(), GetContentFormat(), sce.GetSubstanceManager()))
      return false;
    sce.GetConfiguration().Merge(pc, sce.GetSubstanceManager());
  }
  if (sce.HasConfiguration())
    pe->SetConfigurationOverride(&sce.GetConfiguration());

  return SEScenarioExec::Execute(*pe, sce, status);
}

bool PulseScenarioExec::ConvertLog()
{
  return SEScenarioExec::ConvertLog();
}

size_t PulseScenarioExec::ComputeNumThreads()
{
  size_t numThreadsSupported = std::thread::hardware_concurrency();
  if (numThreadsSupported == 0)
  {
    Fatal("Unable to detect number of processors");
    return 0;
  }

  size_t numThreadsToUse;
  if (m_ThreadCount > 0)
    numThreadsToUse = m_ThreadCount > (int)numThreadsSupported ? numThreadsSupported : m_ThreadCount;
  else if (m_ThreadCount == 0)
    numThreadsToUse = numThreadsSupported;
  else
  {
    numThreadsToUse = numThreadsSupported + m_ThreadCount;
    if (numThreadsToUse <= 0)
    {
      Warning("The abs of the negeative requested thread count is more than the number of cores available on the system, only using 1 thread");
      numThreadsToUse = 1;
    }
  }

  return numThreadsToUse;
}

void PulseScenarioExec::ControllerLoop(const std::string copy,
                                       std::mutex* mutex,
                                       std::vector<SEScenarioExecStatus>* statuses,
                                       size_t* completed,
                                       const std::string scenarioExecListFilename,
                                       Logger* logger)
{
  PulseScenarioExec exec(logger);
  // Create a tmp status to be fill out in parrallel with other threads
  // Once complete, we will copy the data back into the status array before saving it out
  SEScenarioExecStatus  working;
  SEScenarioExecStatus* found;
  while (true)
  {
    found = nullptr;
    // Lock and look in the shared status vector for an available scenario
    mutex->lock();
    for (SEScenarioExecStatus& status : *statuses)
    {
      if (status.GetScenarioExecutionState() == eScenarioExecutionState::Waiting)
      {
        found = &status;
        found->SetScenarioExecutionState(eScenarioExecutionState::Executing);
        break;
      }
    }
    mutex->unlock();
    if (!found)
      return;
    working.Copy(*found);

    // Execute this scenario (Reset the exec every run)
    exec.SerializeFromString(copy, eSerializationFormat::JSON, logger);
    PulseScenario sce(exec.GetDataRootDirectory());
    if (sce.SerializeFromFile(working.GetScenarioFilename()))
    {
      sce.GetLogger()->AddConsolePrefix("["+sce.GetName()+"] ");
      //exec.Info("Executing " + working.GetScenarioFilename());
      // Check to see if we want scenario's to log to the console or not
      exec.Execute(sce, &working);
    }
    else
    {
      working.SetFatalRuntimeError(true);
      exec.Error("Unable to serialize scenario file: " + working.GetScenarioFilename());
    }

    // Lock to update our shared status
    mutex->lock();
    working.SetScenarioExecutionState(eScenarioExecutionState::Complete);
    found->Copy(working);
    (*completed)++;
    exec.Info("Completed " + std::to_string(*completed)+"/"+ std::to_string(statuses->size()) + " " + working.GetScenarioFilename());
    SEScenarioExecStatus::SerializeToFile(*statuses, scenarioExecListFilename, exec.GetLogger());
    mutex->unlock();
  }
}
