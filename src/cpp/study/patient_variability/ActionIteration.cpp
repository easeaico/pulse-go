/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "ActionIteration.h"

namespace pulse::study::patient_variability
{
  ActionIteration::ActionIteration(Logger& logger) : ScenarioIteration(logger)
  {

  }
  ActionIteration::~ActionIteration()
  {

  }

  void ActionIteration::Clear()
  {
    ScenarioIteration::Clear();
  }

  bool ActionIteration::GenerateScenarios(const PatientIteration& patients)
  {
    FixUp();
    m_ScenarioStates.clear();

    Info("Generating " + GetIterationName() + " scenarios to: " + m_ScenarioDirectory);
    Info("Generating " + GetIterationName() + " states to: " + m_StateDirectory);

    for (auto& itr : patients.GetPatientStates())
    {
      switch (m_GenStyle)
      {
      case eGenStyle::Combo:
        GenerateCombinationActionSets(itr);
        break;
      case eGenStyle::Slice:
        GenerateSlicedActionSets(itr);
        break;
      }
    }

    Info("Removed " + std::to_string(m_Duplicates) + " duplicate scenarios");
    Info("Defined " + std::to_string(m_ScenarioList.size()) + " scenarios");
    WriteScenarioList();

    return true;
  }
}
