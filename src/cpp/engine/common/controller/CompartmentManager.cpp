/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/PulseConfiguration.h"
#include "engine/common/controller/Controller.h"
#include "engine/common/controller/CompartmentManager.h"
#include "engine/common/controller/SubstanceManager.h"

#include "cdm/compartment/fluid/SEGasCompartment.h"
#include "cdm/compartment/fluid/SEGasCompartmentLink.h"
#include "cdm/compartment/fluid/SEGasCompartmentGraph.h"
#include "cdm/compartment/fluid/SELiquidCompartment.h"
#include "cdm/compartment/fluid/SELiquidCompartmentLink.h"
#include "cdm/compartment/fluid/SELiquidCompartmentGraph.h"
#include "cdm/compartment/thermal/SEThermalCompartment.h"
#include "cdm/compartment/thermal/SEThermalCompartmentLink.h"
#include "cdm/compartment/tissue/SETissueCompartment.h"
#include "cdm/substance/SESubstance.h"

#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarAmountPerVolume.h"

std::vector<std::string> pulse::Graph::_values;
std::vector<std::string> pulse::ChymeCompartment::_values;
//std::vector<std::string> pulse::ChymeLink::_values;
std::vector<std::string> pulse::PulmonaryCompartment::_values;
std::vector<std::string> pulse::PulmonaryLink::_values;
std::vector<std::string> pulse::ExpandedLungsPulmonaryCompartment::_values;
std::vector<std::string> pulse::ExpandedLungsPulmonaryLink::_values;
std::vector<std::string> pulse::TissueCompartment::_values;
std::vector<std::string> pulse::ExtravascularCompartment::_values;
std::vector<std::string> pulse::TemperatureCompartment::_values;
//std::vector<std::string> pulse::TissueLink::_values;
std::vector<std::string> pulse::VascularCompartment::_values;
std::vector<std::string> pulse::VascularLink::_values;
std::vector<std::string> pulse::ExpandedLungsVascularCompartment::_values;
std::vector<std::string> pulse::ExpandedLungsVascularLink::_values;
std::vector<std::string> pulse::ComputationalLifeVascularCompartment::_values;
std::vector<std::string> pulse::ComputationalLifeVascularLink::_values;
std::vector<std::string> pulse::UrineCompartment::_values;
std::vector<std::string> pulse::UrineLink::_values;
std::vector<std::string> pulse::EnvironmentCompartment::_values;
std::vector<std::string> pulse::AnesthesiaMachineCompartment::_values;
std::vector<std::string> pulse::AnesthesiaMachineLink::_values;
std::vector<std::string> pulse::BagValveMaskCompartment::_values;
std::vector<std::string> pulse::BagValveMaskLink::_values;
std::vector<std::string> pulse::InhalerCompartment::_values;
std::vector<std::string> pulse::InhalerLink::_values;
std::vector<std::string> pulse::MechanicalVentilationCompartment::_values;
std::vector<std::string> pulse::MechanicalVentilationLink::_values;
std::vector<std::string> pulse::MechanicalVentilatorCompartment::_values;
std::vector<std::string> pulse::MechanicalVentilatorLink::_values;
std::vector<std::string> pulse::NasalCannulaCompartment::_values;
std::vector<std::string> pulse::NasalCannulaLink::_values;
std::vector<std::string> pulse::NonRebreatherMaskCompartment::_values;
std::vector<std::string> pulse::NonRebreatherMaskLink::_values;
std::vector<std::string> pulse::SimpleMaskCompartment::_values;
std::vector<std::string> pulse::SimpleMaskLink::_values;

namespace pulse
{
  CompartmentManager::CompartmentManager(Data& data) : SECompartmentManager(data.GetSubstances()), m_data(data)
  {
    Clear();
  }

  CompartmentManager::~CompartmentManager()
  {
    Clear();
  }

  void CompartmentManager::Clear()
  {
    SECompartmentManager::Clear();
    m_CombinedCardiovascularGraph = nullptr;
    m_CardiovascularGraph = nullptr;
    m_RenalGraph = nullptr;
    m_RespiratoryGraph = nullptr;
    m_AnesthesiaMachineGraph = nullptr;
    m_BagValveMaskGraph = nullptr;
    m_MechanicalVentilatorGraph = nullptr;
    m_CombinedRespiratoryAnesthesiaGraph = nullptr;
    m_CombinedAerosolBagValveMaskGraph = nullptr;
    m_CombinedRespiratoryBagValveMaskGraph = nullptr;
    m_CombinedRespiratoryInhalerGraph = nullptr;
    m_CombinedAerosolMechanicalVentilationGraph = nullptr;
    m_CombinedRespiratoryMechanicalVentilationGraph = nullptr;
    m_CombinedAerosolMechanicalVentilatorGraph = nullptr;
    m_CombinedRespiratoryMechanicalVentilatorGraph = nullptr;
    m_CombinedRespiratoryNasalCannulaGraph = nullptr;
    m_CombinedRespiratoryNonRebreatherMaskGraph = nullptr;
    m_CombinedRespiratorySimpleMaskGraph = nullptr;
    m_AerosolGraph = nullptr;
    m_CombinedAerosolInhalerGraph = nullptr;

    m_TemperatureCompartments.clear();
    m_TemperatureLeafCompartments.clear();
    m_ChymeCompartments.clear();
    m_ChymeLeafCompartments.clear();
    m_PulmonaryCompartments.clear();
    m_PulmonaryLeafCompartments.clear();
    m_ExpandedLungsPulmonaryCompartments.clear();
    m_ExpandedLungsPulmonaryLeafCompartments.clear();
    m_TissueCompartments.clear();
    m_TissueLeafCompartments.clear();
    m_UrineCompartments.clear();
    m_UrineLeafCompartments.clear();
    m_VascularCompartments.clear();
    m_VascularLeafCompartments.clear();
    m_ExpandedLungsVascularCompartments.clear();
    m_ExpandedLungsVascularLeafCompartments.clear();
    m_ComputationalLifeVascularCompartments.clear();
    m_ComputationalLifeVascularLeafCompartments.clear();
    m_AnesthesiaMachineCompartments.clear();
    m_AnesthesiaMachineLeafCompartments.clear();
    m_AerosolCompartments.clear();
    m_AerosolLeafCompartments.clear();
    m_BagValveMaskCompartments.clear();
    m_BagValveMaskLeafCompartments.clear();
    m_BagValveMaskAerosolCompartments.clear();
    m_BagValveMaskAerosolLeafCompartments.clear();
    m_InhalerCompartments.clear();
    m_InhalerLeafCompartments.clear();
    m_InhalerAerosolCompartments.clear();
    m_InhalerAerosolLeafCompartments.clear();
    m_MechanicalVentilationCompartments.clear();
    m_MechanicalVentilationLeafCompartments.clear();
    m_MechanicalVentilationAerosolCompartments.clear();
    m_MechanicalVentilationAerosolLeafCompartments.clear();
    m_MechanicalVentilatorCompartments.clear();
    m_MechanicalVentilatorLeafCompartments.clear();
    m_MechanicalVentilatorAerosolCompartments.clear();
    m_MechanicalVentilatorAerosolLeafCompartments.clear();
    m_NasalCannulaCompartments.clear();
    m_NasalCannulaLeafCompartments.clear();
    m_NonRebreatherMaskCompartments.clear();
    m_NonRebreatherMaskLeafCompartments.clear();
    m_SimpleMaskCompartments.clear();
    m_SimpleMaskLeafCompartments.clear();
  }

#define SORT_CMPTS(bin, type) \
m_##bin##Compartments.clear(); \
m_##bin##LeafCompartments.clear(); \
for (const std::string& name : pulse::bin##Compartment::GetValues()) \
{ \
  SE##type##Compartment* cmpt = Get##type##Compartment(name); \
  if (cmpt == nullptr) \
  { \
    Warning("Could not find expected " + std::string(#bin) + " compartment, " + name + " in compartment manager"); \
    continue; \
  } \
  m_##bin##Compartments.push_back(cmpt); \
  if (!cmpt->HasChildren()) \
    m_##bin##LeafCompartments.push_back(cmpt); \
} 

  void CompartmentManager::StateChange()
  {
    SECompartmentManager::StateChange();

    // Hook up our compartment categories
    // Anatomy
    SORT_CMPTS(Chyme, Liquid);
    SORT_CMPTS(Pulmonary, Gas);
    if (m_data.GetConfiguration().UseExpandedLungs() == eSwitch::On)
    {
      SORT_CMPTS(ExpandedLungsPulmonary, Gas);
      // Add Expanded Leafs to Pulmonary Leafs
      for (SEGasCompartment* leaf : m_ExpandedLungsPulmonaryLeafCompartments)
        m_PulmonaryLeafCompartments.push_back(leaf);
    }
    SORT_CMPTS(Temperature, Thermal);
    if (m_data.GetConfiguration().IsTissueEnabled())
    {
      SORT_CMPTS(Tissue, Tissue);
      for (const std::string& name : pulse::ExtravascularCompartment::GetValues())
      {
        if (GetLiquidCompartment(name) == nullptr)
          Warning("Could not find expected Extravascular compartment, " + name + " in compartment manager");
      }
    }
    if (m_data.GetConfiguration().UseExpandedKidneys() == eSwitch::On)
    {
      SORT_CMPTS(Urine, Liquid);
    }

    SORT_CMPTS(Vascular, Liquid);
    if (m_data.GetConfiguration().UseExpandedLungs() == eSwitch::On)
    {
      SORT_CMPTS(ExpandedLungsVascular, Liquid);
      // Add Expanded Leafs to Vascular Leafs
      for (SELiquidCompartment* leaf : m_ExpandedLungsVascularLeafCompartments)
        m_VascularLeafCompartments.push_back(leaf);
    }
    else if (m_data.GetConfiguration().UseComputationalLifeExpansion() == eSwitch::On)
    {
      SORT_CMPTS(ComputationalLifeVascular, Liquid);
      // Add Expanded Leafs to Vascular Leafs
      for (SELiquidCompartment* leaf : m_ComputationalLifeVascularLeafCompartments)
        m_VascularLeafCompartments.push_back(leaf);
    }

    // Equipment
    SORT_CMPTS(AnesthesiaMachine, Gas);
    SORT_CMPTS(BagValveMask, Gas);
    SORT_CMPTS(Inhaler, Gas);
    SORT_CMPTS(MechanicalVentilation, Gas);
    SORT_CMPTS(MechanicalVentilator, Gas);

    m_AerosolCompartments.clear();
    m_AerosolLeafCompartments.clear();
    for (const std::string& name : pulse::PulmonaryCompartment::GetValues())
    {
      SELiquidCompartment* cmpt = GetLiquidCompartment(name);
      if (cmpt == nullptr)
      {
        Warning("Could not find expected Aerosol compartment, " + name + " in compartment manager");
        continue;
      }
      m_AerosolCompartments.push_back(cmpt);
      if (!cmpt->HasChildren())
        m_AerosolLeafCompartments.push_back(cmpt);
    }

    m_BagValveMaskAerosolCompartments.clear();
    m_BagValveMaskAerosolLeafCompartments.clear();
    for (const std::string& name : pulse::BagValveMaskCompartment::GetValues())
    {
      SELiquidCompartment* cmpt = GetLiquidCompartment(name);
      if (cmpt == nullptr)
      {
        Warning("Could not find expected Aerosol compartment, " + name + " in compartment manager");
        continue;
      }
      m_BagValveMaskAerosolCompartments.push_back(cmpt);
      if (!cmpt->HasChildren())
        m_BagValveMaskAerosolLeafCompartments.push_back(cmpt);
    }

    m_InhalerAerosolCompartments.clear();
    m_InhalerAerosolLeafCompartments.clear();
    for (const std::string& name : pulse::InhalerCompartment::GetValues())
    {
      SELiquidCompartment* cmpt = GetLiquidCompartment(name);
      if (cmpt == nullptr)
      {
        Warning("Could not find expected Aerosol compartment, " + name + " in compartment manager");
        continue;
      }
      m_InhalerAerosolCompartments.push_back(cmpt);
      if (!cmpt->HasChildren())
        m_InhalerAerosolLeafCompartments.push_back(cmpt);
    }

    m_MechanicalVentilationAerosolCompartments.clear();
    m_MechanicalVentilationAerosolLeafCompartments.clear();
    for (const std::string& name : pulse::MechanicalVentilationCompartment::GetValues())
    {
      SELiquidCompartment* cmpt = GetLiquidCompartment(name);
      if (cmpt == nullptr)
      {
        Warning("Could not find expected Aerosol compartment, " + name + " in compartment manager");
        continue;
      }
      m_MechanicalVentilationAerosolCompartments.push_back(cmpt);
      if (!cmpt->HasChildren())
        m_MechanicalVentilationAerosolLeafCompartments.push_back(cmpt);
    }

    m_MechanicalVentilatorAerosolCompartments.clear();
    m_MechanicalVentilatorAerosolLeafCompartments.clear();
    for (const std::string& name : pulse::MechanicalVentilatorCompartment::GetValues())
    {
      SELiquidCompartment* cmpt = GetLiquidCompartment(name);
      if (cmpt == nullptr)
      {
        Warning("Could not find expected Aerosol compartment, " + name + " in compartment manager");
        continue;
      }
      m_MechanicalVentilatorAerosolCompartments.push_back(cmpt);
      if (!cmpt->HasChildren())
        m_MechanicalVentilatorAerosolLeafCompartments.push_back(cmpt);
    }

    m_NasalCannulaCompartments.clear();
    m_NasalCannulaLeafCompartments.clear();
    for (const std::string& name : pulse::NasalCannulaCompartment::GetValues())
    {
      SEGasCompartment* cmpt = GetGasCompartment(name);
      if (cmpt == nullptr)
      {
        Warning("Could not find expected Nasal Cannula compartment, " + name + " in compartment manager");
        continue;
      }
      m_NasalCannulaCompartments.push_back(cmpt);
      if (!cmpt->HasChildren())
        m_NasalCannulaLeafCompartments.push_back(cmpt);
    }

    m_NonRebreatherMaskCompartments.clear();
    m_NonRebreatherMaskLeafCompartments.clear();
    for (const std::string& name : pulse::NonRebreatherMaskCompartment::GetValues())
    {
      SEGasCompartment* cmpt = GetGasCompartment(name);
      if (cmpt == nullptr)
      {
        Warning("Could not find expected Non Rebreather Mask compartment, " + name + " in compartment manager");
        continue;
      }
      m_NonRebreatherMaskCompartments.push_back(cmpt);
      if (!cmpt->HasChildren())
        m_NonRebreatherMaskLeafCompartments.push_back(cmpt);
    }

    m_SimpleMaskCompartments.clear();
    m_SimpleMaskLeafCompartments.clear();
    for (const std::string& name : pulse::SimpleMaskCompartment::GetValues())
    {
      SEGasCompartment* cmpt = GetGasCompartment(name);
      if (cmpt == nullptr)
      {
        Warning("Could not find expected Simple Mask compartment, " + name + " in compartment manager");
        continue;
      }
      m_SimpleMaskCompartments.push_back(cmpt);
      if (!cmpt->HasChildren())
        m_SimpleMaskLeafCompartments.push_back(cmpt);
    }

    // \todo Write some code to cross check compartments between what we have and what we should have
    // Here is some code to make sure all created compartments are in an enum
    // (We could have removed it from the enum, but not deleted the code that makes it)
    // It would also be nice to have code that makes sure there are no compartments that are made, but NOT in the enums
    // Note, Tissue Enum list does not contain the Tissue Extracellular or Intracellular names, NO list does

    m_CombinedCardiovascularGraph = GetLiquidGraph(pulse::Graph::ActiveCardiovascular);
    if (m_CombinedCardiovascularGraph == nullptr)
    {
      Error("Could not find required Graph " + std::string(pulse::Graph::ActiveCardiovascular));
    }
    m_CardiovascularGraph = GetLiquidGraph(pulse::Graph::Cardiovascular);
    if (m_CardiovascularGraph == nullptr)
    {
      Error("Could not find required Graph " + std::string(pulse::Graph::Cardiovascular));
    }
    m_RenalGraph = GetLiquidGraph(pulse::Graph::Renal);
    if (m_RenalGraph == nullptr)
    {
      Error("Could not find required Graph " + std::string(pulse::Graph::Renal));
    }
    m_RespiratoryGraph = GetGasGraph(pulse::Graph::Respiratory);
    if (m_RespiratoryGraph == nullptr)
    {
      Error("Could not find required Graph " + std::string(pulse::Graph::Respiratory));
    }
    m_AerosolGraph = GetLiquidGraph(pulse::Graph::Aerosol);
    if (m_AerosolGraph == nullptr)
    {
      Error("Could not find required Graph " + std::string(pulse::Graph::Aerosol));
    }
    m_AnesthesiaMachineGraph = GetGasGraph(pulse::Graph::AnesthesiaMachine);
    if (m_AnesthesiaMachineGraph == nullptr)
    {
      Error("Could not find required Graph " + std::string(pulse::Graph::AnesthesiaMachine));
    }
    m_CombinedRespiratoryAnesthesiaGraph = GetGasGraph(pulse::Graph::RespiratoryAndAnesthesiaMachine);
    if (m_CombinedRespiratoryAnesthesiaGraph == nullptr)
    {
      Error("Could not find required Graph " + std::string(pulse::Graph::RespiratoryAndAnesthesiaMachine));
    }
    m_CombinedRespiratoryBagValveMaskGraph = GetGasGraph(pulse::Graph::RespiratoryAndBagValveMask);
    if (m_CombinedRespiratoryBagValveMaskGraph == nullptr)
    {
      Error("Could not find required Graph " + std::string(pulse::Graph::RespiratoryAndBagValveMask));
    }
    m_CombinedAerosolBagValveMaskGraph = GetLiquidGraph(pulse::Graph::AerosolAndBagValveMask);
    if (m_CombinedAerosolBagValveMaskGraph == nullptr)
    {
      Error("Could not find required Graph " + std::string(pulse::Graph::AerosolAndBagValveMask));
    }
    m_CombinedRespiratoryInhalerGraph = GetGasGraph(pulse::Graph::RespiratoryAndInhaler);
    if (m_CombinedRespiratoryInhalerGraph == nullptr)
    {
      Error("Could not find required Graph " + std::string(pulse::Graph::RespiratoryAndInhaler));
    }
    m_CombinedAerosolInhalerGraph = GetLiquidGraph(pulse::Graph::AerosolAndInhaler);
    if (m_CombinedAerosolInhalerGraph == nullptr)
    {
      Error("Could not find required Graph " + std::string(pulse::Graph::AerosolAndInhaler));
    }
    m_CombinedRespiratoryMechanicalVentilationGraph = GetGasGraph(pulse::Graph::RespiratoryAndMechanicalVentilation);
    if (m_CombinedRespiratoryMechanicalVentilationGraph == nullptr)
    {
      Error("Could not find required Graph " + std::string(pulse::Graph::RespiratoryAndMechanicalVentilation));
    }
    m_CombinedAerosolMechanicalVentilationGraph = GetLiquidGraph(pulse::Graph::AerosolAndMechanicalVentilation);
    if (m_CombinedAerosolMechanicalVentilationGraph == nullptr)
    {
      Error("Could not find required Graph " + std::string(pulse::Graph::AerosolAndMechanicalVentilation));
    }
    m_CombinedRespiratoryMechanicalVentilatorGraph = GetGasGraph(pulse::Graph::RespiratoryAndMechanicalVentilator);
    if (m_CombinedRespiratoryMechanicalVentilatorGraph == nullptr)
    {
      Error("Could not find required Graph " + std::string(pulse::Graph::RespiratoryAndMechanicalVentilator));
    }
    m_CombinedAerosolMechanicalVentilatorGraph = GetLiquidGraph(pulse::Graph::AerosolAndMechanicalVentilator);
    if (m_CombinedAerosolMechanicalVentilatorGraph == nullptr)
    {
      Error("Could not find required Graph " + std::string(pulse::Graph::AerosolAndMechanicalVentilator));
    }
    m_CombinedRespiratoryNasalCannulaGraph = GetGasGraph(pulse::Graph::RespiratoryAndNasalCannula);
    if (m_CombinedRespiratoryNasalCannulaGraph == nullptr)
    {
      Error("Could not find required Graph " + std::string(pulse::Graph::RespiratoryAndNasalCannula));
    }
    m_CombinedRespiratoryNonRebreatherMaskGraph = GetGasGraph(pulse::Graph::RespiratoryAndNonRebreatherMask);
    if (m_CombinedRespiratoryNonRebreatherMaskGraph == nullptr)
    {
      Error("Could not find required Graph " + std::string(pulse::Graph::RespiratoryAndNonRebreatherMask));
    }
    m_CombinedRespiratorySimpleMaskGraph = GetGasGraph(pulse::Graph::RespiratoryAndSimpleMask);
    if (m_CombinedRespiratorySimpleMaskGraph == nullptr)
    {
      Error("Could not find required Graph " + std::string(pulse::Graph::RespiratoryAndSimpleMask));
    }
  }

  void CompartmentManager::AddGasCompartmentSubstance(SESubstance& sub)
  {
    SECompartmentManager::AddGasCompartmentSubstance(sub);
  }
  void CompartmentManager::AddLiquidCompartmentSubstance(SESubstance& sub)
  {
    SECompartmentManager::AddLiquidCompartmentSubstance(sub);
  }

  bool CompartmentManager::AllowGasSubstance(SESubstance& /*s*/, SEGasCompartment& /*cmpt*/) const
  {
    return true;
  }
  bool CompartmentManager::AllowLiquidSubstance(SESubstance& s, SELiquidCompartment& cmpt) const
  {
    if (s.HasAerosolization())// It's ok to add it everywhere
      return true;
    else
    {
      // Don't add it to the environment aerosol
      const std::vector<std::string>& e = pulse::EnvironmentCompartment::GetValues();
      if (std::find(e.begin(), e.end(), cmpt.GetName()) != e.end())
        return false;
      // Don't add it to the aerosol compartments (Liquid version of Respiratory cmpts)
      const std::vector<std::string>& p = pulse::PulmonaryCompartment::GetValues();
      if (std::find(p.begin(), p.end(), cmpt.GetName()) != p.end())
        return false;
      // Don't add it to aerosol cmpts either
      const std::vector<std::string>& bvm = pulse::BagValveMaskCompartment::GetValues();
      if (std::find(bvm.begin(), bvm.end(), cmpt.GetName()) != bvm.end())
        return false;
      const std::vector<std::string>& i = pulse::InhalerCompartment::GetValues();
      if (std::find(i.begin(), i.end(), cmpt.GetName()) != i.end())
        return false;
      const std::vector<std::string>& mvn = pulse::MechanicalVentilationCompartment::GetValues();
      if (std::find(mvn.begin(), mvn.end(), cmpt.GetName()) != mvn.end())
        return false;
      const std::vector<std::string>& mvr = pulse::MechanicalVentilatorCompartment::GetValues();
      if (std::find(mvr.begin(), mvr.end(), cmpt.GetName()) != mvr.end())
        return false;
    }
    return true;
  }

  SELiquidCompartmentGraph& CompartmentManager::GetActiveCardiovascularGraph()
  {
    if (m_CombinedCardiovascularGraph == nullptr)
      m_CombinedCardiovascularGraph = &CreateLiquidGraph(pulse::Graph::ActiveCardiovascular);
    return *m_CombinedCardiovascularGraph;
  }
  SELiquidCompartmentGraph& CompartmentManager::GetCardiovascularGraph()
  {
    if (m_CardiovascularGraph == nullptr)
      m_CardiovascularGraph = &CreateLiquidGraph(pulse::Graph::Cardiovascular);
    return *m_CardiovascularGraph;
  }
  SELiquidCompartmentGraph& CompartmentManager::GetRenalGraph()
  {
    if (m_RenalGraph == nullptr)
      m_RenalGraph = &CreateLiquidGraph(pulse::Graph::Renal);
    return *m_RenalGraph;
  }
  SEGasCompartmentGraph& CompartmentManager::GetActiveRespiratoryGraph()
  {
    switch (m_data.GetAirwayMode())
    {
    case eAirwayMode::Free:
      if (m_UpdateActiveAirwayGraph)
        m_data.GetCompartments().UpdateLinks(*m_RespiratoryGraph);
      m_UpdateActiveAirwayGraph = false;
      return *m_RespiratoryGraph;
    case eAirwayMode::AnesthesiaMachine:
      if (m_UpdateActiveAirwayGraph)
        m_data.GetCompartments().UpdateLinks(*m_CombinedRespiratoryAnesthesiaGraph);
      m_UpdateActiveAirwayGraph = false;
      return *m_CombinedRespiratoryAnesthesiaGraph;
    case eAirwayMode::BagValveMask:
      if (m_UpdateActiveAirwayGraph)
        m_data.GetCompartments().UpdateLinks(*m_CombinedRespiratoryBagValveMaskGraph);
      m_UpdateActiveAirwayGraph = false;
      return *m_CombinedRespiratoryBagValveMaskGraph;
    case eAirwayMode::Inhaler:
      if (m_UpdateActiveAirwayGraph)
        m_data.GetCompartments().UpdateLinks(*m_CombinedRespiratoryInhalerGraph);
      m_UpdateActiveAirwayGraph = false;
      return *m_CombinedRespiratoryInhalerGraph;
    case eAirwayMode::MechanicalVentilation:
      if (m_UpdateActiveAirwayGraph)
        m_data.GetCompartments().UpdateLinks(*m_CombinedRespiratoryMechanicalVentilationGraph);
      m_UpdateActiveAirwayGraph = false;
      return *m_CombinedRespiratoryMechanicalVentilationGraph;
    case eAirwayMode::MechanicalVentilator:
      if (m_UpdateActiveAirwayGraph)
        m_data.GetCompartments().UpdateLinks(*m_CombinedRespiratoryMechanicalVentilatorGraph);
      m_UpdateActiveAirwayGraph = false;
      return *m_CombinedRespiratoryMechanicalVentilatorGraph;
    case eAirwayMode::NasalCannula:
      if (m_UpdateActiveAirwayGraph)
        m_data.GetCompartments().UpdateLinks(*m_CombinedRespiratoryNasalCannulaGraph);
      m_UpdateActiveAirwayGraph = false;
      return *m_CombinedRespiratoryNasalCannulaGraph;
    case eAirwayMode::NonRebreatherMask:
      if (m_UpdateActiveAirwayGraph)
        m_data.GetCompartments().UpdateLinks(*m_CombinedRespiratoryNonRebreatherMaskGraph);
      m_UpdateActiveAirwayGraph = false;
      return *m_CombinedRespiratoryNonRebreatherMaskGraph;
    case eAirwayMode::SimpleMask:
      if (m_UpdateActiveAirwayGraph)
        m_data.GetCompartments().UpdateLinks(*m_CombinedRespiratorySimpleMaskGraph);
      m_UpdateActiveAirwayGraph = false;
      return *m_CombinedRespiratorySimpleMaskGraph;
    default:
      throw CommonDataModelException("Unknown airway mode");
    }
  }
  SEGasCompartmentGraph& CompartmentManager::GetRespiratoryGraph()
  {
    if (m_RespiratoryGraph == nullptr)
      m_RespiratoryGraph = &CreateGasGraph(pulse::Graph::Respiratory);
    return *m_RespiratoryGraph;
  }
  SEGasCompartmentGraph& CompartmentManager::GetAnesthesiaMachineGraph()
  {
    if (m_AnesthesiaMachineGraph == nullptr)
      m_AnesthesiaMachineGraph = &CreateGasGraph(pulse::Graph::AnesthesiaMachine);
    return *m_AnesthesiaMachineGraph;
  }
  SEGasCompartmentGraph& CompartmentManager::GetRespiratoryAndAnesthesiaMachineGraph()
  {
    if (m_CombinedRespiratoryAnesthesiaGraph == nullptr)
      m_CombinedRespiratoryAnesthesiaGraph = &CreateGasGraph(pulse::Graph::RespiratoryAndAnesthesiaMachine);
    return *m_CombinedRespiratoryAnesthesiaGraph;
  }

  SEGasCompartmentGraph& CompartmentManager::GetBagValveMaskGraph()
  {
    if (m_BagValveMaskGraph == nullptr)
      m_BagValveMaskGraph = &CreateGasGraph(pulse::Graph::BagValveMask);
    return *m_BagValveMaskGraph;
  }
  SEGasCompartmentGraph& CompartmentManager::GetRespiratoryAndBagValveMaskGraph()
  {
    if (m_CombinedRespiratoryBagValveMaskGraph == nullptr)
      m_CombinedRespiratoryBagValveMaskGraph = &CreateGasGraph(pulse::Graph::RespiratoryAndBagValveMask);
    return *m_CombinedRespiratoryBagValveMaskGraph;
  }
  SELiquidCompartmentGraph& CompartmentManager::GetAerosolAndBagValveMaskGraph()
  {
    if (m_CombinedAerosolBagValveMaskGraph == nullptr)
      m_CombinedAerosolBagValveMaskGraph = &CreateLiquidGraph(pulse::Graph::AerosolAndBagValveMask);
    return *m_CombinedAerosolBagValveMaskGraph;
  }

  SEGasCompartmentGraph& CompartmentManager::GetRespiratoryAndInhalerGraph()
  {
    if (m_CombinedRespiratoryInhalerGraph == nullptr)
      m_CombinedRespiratoryInhalerGraph = &CreateGasGraph(pulse::Graph::RespiratoryAndInhaler);
    return *m_CombinedRespiratoryInhalerGraph;
  }
  SELiquidCompartmentGraph& CompartmentManager::GetActiveAerosolGraph()
  {
    switch (m_data.GetAirwayMode())
    {
    case eAirwayMode::Free:
      if (m_UpdateActiveAerosolGraph)
        m_data.GetCompartments().UpdateLinks(*m_AerosolGraph);
      m_UpdateActiveAerosolGraph = false;
      return *m_AerosolGraph;
    case eAirwayMode::AnesthesiaMachine:// Just use the regular graph
      if (m_UpdateActiveAerosolGraph)
        m_data.GetCompartments().UpdateLinks(*m_AerosolGraph);
      m_UpdateActiveAerosolGraph = false;
      return *m_AerosolGraph;
    case eAirwayMode::BagValveMask:
      if (m_UpdateActiveAerosolGraph)
        m_data.GetCompartments().UpdateLinks(*m_CombinedAerosolBagValveMaskGraph);
      m_UpdateActiveAerosolGraph = false;
      return *m_CombinedAerosolBagValveMaskGraph;
    case eAirwayMode::Inhaler:
      if (m_UpdateActiveAerosolGraph)
        m_data.GetCompartments().UpdateLinks(*m_CombinedAerosolInhalerGraph);
      m_UpdateActiveAerosolGraph = false;
      return *m_CombinedAerosolInhalerGraph;
    case eAirwayMode::MechanicalVentilation:
      if (m_UpdateActiveAerosolGraph)
        m_data.GetCompartments().UpdateLinks(*m_CombinedAerosolMechanicalVentilationGraph);
      m_UpdateActiveAerosolGraph = false;
      return *m_CombinedAerosolMechanicalVentilationGraph;
    case eAirwayMode::MechanicalVentilator:
      if (m_UpdateActiveAerosolGraph)
        m_data.GetCompartments().UpdateLinks(*m_CombinedAerosolMechanicalVentilatorGraph);
      m_UpdateActiveAerosolGraph = false;
      return *m_CombinedAerosolMechanicalVentilatorGraph;
    case eAirwayMode::NasalCannula:// Just use the regular graph
      if (m_UpdateActiveAerosolGraph)
        m_data.GetCompartments().UpdateLinks(*m_AerosolGraph);
      m_UpdateActiveAerosolGraph = false;
      return *m_AerosolGraph;
    case eAirwayMode::NonRebreatherMask:// Just use the regular graph
      if (m_UpdateActiveAerosolGraph)
        m_data.GetCompartments().UpdateLinks(*m_AerosolGraph);
      m_UpdateActiveAerosolGraph = false;
      return *m_AerosolGraph;
    case eAirwayMode::SimpleMask:// Just use the regular graph
      if (m_UpdateActiveAerosolGraph)
        m_data.GetCompartments().UpdateLinks(*m_AerosolGraph);
      m_UpdateActiveAerosolGraph = false;
      return *m_AerosolGraph;
    default:
      throw CommonDataModelException("Unknown airway mode");
    }
  }
  SELiquidCompartmentGraph& CompartmentManager::GetAerosolGraph()
  {
    if (m_AerosolGraph == nullptr)
      m_AerosolGraph = &CreateLiquidGraph(pulse::Graph::Aerosol);
    return *m_AerosolGraph;
  }
  SELiquidCompartmentGraph& CompartmentManager::GetAerosolAndInhalerGraph()
  {
    if (m_CombinedAerosolInhalerGraph == nullptr)
      m_CombinedAerosolInhalerGraph = &CreateLiquidGraph(pulse::Graph::AerosolAndInhaler);
    return *m_CombinedAerosolInhalerGraph;
  }

  SEGasCompartmentGraph& CompartmentManager::GetRespiratoryAndMechanicalVentilationGraph()
  {
    if (m_CombinedRespiratoryMechanicalVentilationGraph == nullptr)
      m_CombinedRespiratoryMechanicalVentilationGraph = &CreateGasGraph(pulse::Graph::RespiratoryAndMechanicalVentilation);
    return *m_CombinedRespiratoryMechanicalVentilationGraph;
  }
  SELiquidCompartmentGraph& CompartmentManager::GetAerosolAndMechanicalVentilationGraph()
  {
    if (m_CombinedAerosolMechanicalVentilationGraph == nullptr)
      m_CombinedAerosolMechanicalVentilationGraph = &CreateLiquidGraph(pulse::Graph::AerosolAndMechanicalVentilation);
    return *m_CombinedAerosolMechanicalVentilationGraph;
  }

  SEGasCompartmentGraph& CompartmentManager::GetMechanicalVentilatorGraph()
  {
    if (m_MechanicalVentilatorGraph == nullptr)
      m_MechanicalVentilatorGraph = &CreateGasGraph(pulse::Graph::MechanicalVentilator);
    return *m_MechanicalVentilatorGraph;
  }
  SEGasCompartmentGraph& CompartmentManager::GetRespiratoryAndMechanicalVentilatorGraph()
  {
    if (m_CombinedRespiratoryMechanicalVentilatorGraph == nullptr)
      m_CombinedRespiratoryMechanicalVentilatorGraph = &CreateGasGraph(pulse::Graph::RespiratoryAndMechanicalVentilator);
    return *m_CombinedRespiratoryMechanicalVentilatorGraph;
  }
  SELiquidCompartmentGraph& CompartmentManager::GetAerosolAndMechanicalVentilatorGraph()
  {
    if (m_CombinedAerosolMechanicalVentilatorGraph == nullptr)
      m_CombinedAerosolMechanicalVentilatorGraph = &CreateLiquidGraph(pulse::Graph::AerosolAndMechanicalVentilator);
    return *m_CombinedAerosolMechanicalVentilatorGraph;
  }

  SEGasCompartmentGraph& CompartmentManager::GetRespiratoryAndNasalCannulaGraph()
  {
    if (m_CombinedRespiratoryNasalCannulaGraph == nullptr)
      m_CombinedRespiratoryNasalCannulaGraph = &CreateGasGraph(pulse::Graph::RespiratoryAndNasalCannula);
    return *m_CombinedRespiratoryNasalCannulaGraph;
  }

  SEGasCompartmentGraph& CompartmentManager::GetRespiratoryAndNonRebreatherMaskGraph()
  {
    if (m_CombinedRespiratoryNonRebreatherMaskGraph == nullptr)
      m_CombinedRespiratoryNonRebreatherMaskGraph = &CreateGasGraph(pulse::Graph::RespiratoryAndNonRebreatherMask);
    return *m_CombinedRespiratoryNonRebreatherMaskGraph;
  }

  SEGasCompartmentGraph& CompartmentManager::GetRespiratoryAndSimpleMaskGraph()
  {
    if (m_CombinedRespiratorySimpleMaskGraph == nullptr)
      m_CombinedRespiratorySimpleMaskGraph = &CreateGasGraph(pulse::Graph::RespiratoryAndSimpleMask);
    return *m_CombinedRespiratorySimpleMaskGraph;
  }
}
