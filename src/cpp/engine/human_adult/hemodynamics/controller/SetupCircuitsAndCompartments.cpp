/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/CommonDefs.h"
#include "engine/human_adult/hemodynamics/controller/Controller.h"
#include "engine/human_adult/hemodynamics/controller/CompartmentManager.h"
#include "engine/human_adult/whole_body/controller/CircuitManager.h"
#include "engine/human_adult/whole_body/controller/SubstanceManager.h"
#include "engine/human_adult/whole_body/system/environment/EnvironmentModel.h"

#include "cdm/system/environment/SEEnvironmentalConditions.h"
#include "cdm/circuit/SECircuitManager.h"
#include "cdm/circuit/fluid/SEFluidCircuit.h"
#include "cdm/circuit/fluid/SEFluidCircuitNode.h"
#include "cdm/circuit/fluid/SEFluidCircuitPath.h"
#include "cdm/compartment/fluid/SEGasCompartment.h"
#include "cdm/compartment/fluid/SEGasCompartmentLink.h"
#include "cdm/compartment/fluid/SEGasCompartmentGraph.h"
#include "cdm/compartment/fluid/SELiquidCompartment.h"
#include "cdm/compartment/fluid/SELiquidCompartmentLink.h"
#include "cdm/compartment/fluid/SELiquidCompartmentGraph.h"

namespace pulse { namespace human_adult_hemodynamics
{
  bool Controller::CreateCircuitsAndCompartments()
  {
    m_Circuits->Clear();
    m_Compartments->Clear();
    m_Compartments->Setup();

    SetupCardiovascular();
    if (m_Config->UseExpandedKidneys() == eSwitch::On)
      SetupRenal();
    if (m_Config->IsTissueEnabled())
      SetupTissue();
    if (m_Config->IsCerebrospinalFluidEnabled())
      SetupCerebrospinalFluid();

    // We only want modifiers associated with cv circuit
    auto itr = m_Config->GetModifiers().begin();
    while (itr != m_Config->GetModifiers().end())
    {
      if (m_Circuits->GetActiveCardiovascularCircuit().HasPath(itr->first))
        ++itr;
      else
        m_Config->GetModifiers().erase(itr++);
    }

    m_Compartments->StateChange();
    return true;
  }
END_NAMESPACE_EX
