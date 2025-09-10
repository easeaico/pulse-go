/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/human_adult/hemodynamics/controller/CompartmentManager.h"

#include "cdm/compartment/fluid/SELiquidCompartment.h"
#include "cdm/compartment/tissue/SETissueCompartment.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarAmountPerVolume.h"

namespace pulse { namespace human_adult_hemodynamics
{
  CompartmentManager::CompartmentManager(pulse::Data& data) : pulse::CompartmentManager(data)
  {
    Clear();
  }

  CompartmentManager::~CompartmentManager()
  {
    Clear();
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
  }
END_NAMESPACE_EX
