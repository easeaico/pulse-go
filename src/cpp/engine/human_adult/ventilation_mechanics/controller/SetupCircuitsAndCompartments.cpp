/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/CommonDefs.h"
#include "engine/human_adult/ventilation_mechanics/controller/Controller.h"
#include "engine/human_adult/ventilation_mechanics/controller/CompartmentManager.h"
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

namespace pulse { namespace human_adult_ventilation_mechanics
{
  bool Controller::CreateCircuitsAndCompartments()
  {
    m_Circuits->Clear();
    m_Compartments->Clear();
    m_Compartments->Setup();

    // This node is shared between air based circuits
    SEFluidCircuitNode& Ambient = m_Circuits->CreateFluidNode(pulse::EnvironmentNode::Ambient);
    Ambient.GetNextVolume().SetValue(std::numeric_limits<double>::infinity(), VolumeUnit::L);
    Ambient.GetVolumeBaseline().SetValue(std::numeric_limits<double>::infinity(), VolumeUnit::L);
    SEGasCompartment& gEnvironment = m_Compartments->CreateGasCompartment(pulse::EnvironmentCompartment::Ambient);
    gEnvironment.MapNode(Ambient);
    SELiquidCompartment& lEnvironment = m_Compartments->CreateLiquidCompartment(pulse::EnvironmentCompartment::Ambient);
    lEnvironment.MapNode(Ambient);
    Ambient.SetAsReferenceNode();

    m_EnvironmentModel->Clear();
    m_EnvironmentModel->Initialize();

    // Below is what we need to initialize the system if we don't have substances
    // But Respiratory::TuneCircuit still assumes substances are in the system
    //double atmPressure_cmH2O = 1033.23;// 1 atm
    //if (GetConfiguration().HasInitialEnvironmentalConditions() && GetConfiguration().GetInitialEnvironmentalConditions()->HasAtmosphericPressure())
    //  atmPressure_cmH20 = GetConfiguration().GetInitialEnvironmentalConditions()->GetAtmosphericPressure(PressureUnit::cmH2O);
    //gEnvironment.GetPressure().SetValue(atmPressure_cmH2O, PressureUnit::cmH2O);

    SetupRespiratory();
    SetupMechanicalVentilator();

    // We only want modifiers associated with respiratory and the ventilator
    auto itr = m_Config->GetModifiers().begin();
    while (itr != m_Config->GetModifiers().end())
    {
      if (!m_Circuits->GetRespiratoryAndMechanicalVentilatorCircuit().HasPath(itr->first))
        m_Config->GetModifiers().erase(itr++);
      else
        ++itr;
    }

    m_Compartments->StateChange();
    return true;
  }
END_NAMESPACE_EX
