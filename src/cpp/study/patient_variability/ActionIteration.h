/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

#include "ScenarioIteration.h"
#include "PatientIteration.h"

namespace pulse::study::patient_variability
{
  class ActionIteration : public ScenarioIteration
  {
  public:
    ActionIteration(Logger& logger);
    virtual ~ActionIteration();

    void Clear() override;

    bool CreateStates() const { return m_CreateStates; }
    void CreateStates(bool b) { m_CreateStates = b; }

    double GetBaselineDuration_s() const { return m_BaselineDuration_s; }
    void SetBaselineDuration_s(double d) { m_BaselineDuration_s = d; }

    double GetMaxSimTime_min() const { return m_MaxSimTime_min; }
    void SetMaxSimTime_min(double d) { m_MaxSimTime_min = d; }

    virtual bool GenerateScenarios(const PatientIteration& patients);

  protected:
    virtual void GenerateSlicedActionSets(std::pair<std::string, std::string>) = 0;
    virtual void GenerateCombinationActionSets(std::pair<std::string, std::string>) = 0;


    bool                                          m_CreateStates;
    double                                        m_BaselineDuration_s;
    double                                        m_MaxSimTime_min;
    std::map<std::string, std::string>            m_ScenarioStates;
  };
}
