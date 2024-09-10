/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

#include "PulseScenario.h"
#include "cdm/engine/SESerializeState.h"
#include "cdm/scenario/SEScenarioExec.h"
#include "cdm/utils/ParameterIteration.h"

namespace pulse::study::patient_variability
{
  enum class eGenStyle
  {
    Combo = 0,
    Slice
  };

  class ScenarioIteration : protected PulseScenario
  {
  public:
    ScenarioIteration(Logger& logger);
    virtual ~ScenarioIteration();

    void Clear() override;

    std::string GetName() const override;
    void SetName(const std::string& name) override;
    bool HasName() const override;

    virtual eGenStyle GetGenStyle() const { return m_GenStyle; }
    virtual void SetGenStyle(eGenStyle s) { m_GenStyle = s; }

    virtual void SetIterationName(const std::string& n) { m_IterationName = n; }
    virtual std::string GetIterationName() const { return m_IterationName; }

    virtual void SetResultsDirectory(const std::string& d);
    virtual std::string GetResultsDirectory() const { return m_ResultsDirectory; }

    virtual void SetScenarioExecListFilename(const std::string& d);
    virtual std::string GetScenarioExecListFilename() const { return m_ScenarioExecListFilename; }

    virtual void SetStateDirectory(const std::string& d);
    virtual std::string GetStateDirectory() const { return m_StateDirectory; }

    const std::vector<SEScenarioExecStatus>& GetStatus() const { return m_ScenarioList; }
  protected:
    virtual void FixUp() {};
    virtual bool WriteScenario();
    virtual bool WriteScenarioList();

    // Statefull
    eGenStyle                                     m_GenStyle;
    std::string                                   m_IterationName;
    std::string                                   m_ResultsDirectory;
    std::string                                   m_ScenarioExecListFilename;
    std::string                                   m_StateDirectory;

    // Stateless
    std::string                                   m_ScenarioDirectory;
    size_t                                        m_Duplicates;
    SESerializeState                              m_Serialize;
    SEScenarioExecStatus                          m_ScenarioStatus;
    std::vector<SEScenarioExecStatus>             m_ScenarioList;
  };
}
