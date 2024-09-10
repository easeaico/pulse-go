/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "ScenarioIteration.h"
#include "cdm/utils/FileUtils.h"

namespace pulse::study::patient_variability
{
  ScenarioIteration::ScenarioIteration(Logger& logger) : PulseScenario(&logger)
  {
    m_GenStyle = eGenStyle::Combo;
    m_Duplicates = 0;
    m_IterationName = "";
    m_ScenarioDirectory = "";
    m_ScenarioExecListFilename = "";
    m_StateDirectory = "";
  }
  ScenarioIteration::~ScenarioIteration()
  {

  }

  void ScenarioIteration::Clear()
  {
    PulseScenario::Clear();
  }

  std::string ScenarioIteration::GetName() const
  {
    return PulseScenario::GetName();
  }
  void ScenarioIteration::SetName(const std::string& name)
  {
    PulseScenario::SetName(name);
  }
  bool ScenarioIteration::HasName() const
  {
    return PulseScenario::HasName();
  }

  void ScenarioIteration::SetResultsDirectory(const std::string& d)
  {
    if (d.empty())
      m_ResultsDirectory = "./test_results/patient_variability/results/" + GetIterationName();
    else
      m_ResultsDirectory = d;
    if (m_ResultsDirectory.back() != '/')
      m_ResultsDirectory = m_ResultsDirectory + "/";
  }

  void ScenarioIteration::SetScenarioExecListFilename(const std::string& d)
  {
    if (d.empty())
      m_ScenarioExecListFilename = "./test_results/patient_variability/scenarios/"+GetIterationName()+"_list.json";
    else
      m_ScenarioExecListFilename = d;

    std::string path, filename, ext;
    SplitPathFilenameExt(m_ScenarioExecListFilename, path, filename, ext);
    m_ScenarioDirectory = path + filename;
    if (m_ScenarioDirectory.back() != '/')
      m_ScenarioDirectory = m_ScenarioDirectory + "/";
  }

  void ScenarioIteration::SetStateDirectory(const std::string& d)
  {
    if (d.empty())
      m_StateDirectory = "./test_results/patient_variability/states/"+GetIterationName();
    else
      m_StateDirectory = d;
    if (m_StateDirectory.back() != '/')
      m_StateDirectory = m_StateDirectory + "/";
  }

  bool ScenarioIteration::WriteScenario()
  {
    m_ScenarioStatus.Clear();
    m_ScenarioStatus.SetScenarioFilename(m_ScenarioDirectory+m_Name+".json");
    m_ScenarioList.push_back(m_ScenarioStatus);

    Info("Writing scenario " + m_ScenarioStatus.GetScenarioFilename());
    return SerializeToFile(m_ScenarioStatus.GetScenarioFilename());
  }

  bool ScenarioIteration::WriteScenarioList()
  {
    return SEScenarioExecStatus::SerializeToFile(m_ScenarioList, m_ScenarioExecListFilename, GetLogger());
  }
}
