/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SERunningAverage.h"

template<FLUID_COMPARTMENT_TEMPLATE>
SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::SEFluidCompartment(const std::string& name, Logger* logger) : SECompartment(name, logger), m_Nodes(logger)
{
  m_SampleFlow = false;
  m_InFlow = nullptr;
  m_OutFlow = nullptr;
  m_AverageInFlow = nullptr;
  m_AverageOutFlow = nullptr;
  m_AverageInFlow_mL_Per_s = nullptr;
  m_AverageOutFlow_mL_Per_s = nullptr;
  m_Pressure = nullptr;
  m_Volume = nullptr;
}
template<FLUID_COMPARTMENT_TEMPLATE>
SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::~SEFluidCompartment()
{
  Clear();
}

template<FLUID_COMPARTMENT_TEMPLATE>
void SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::Clear()
{
  SECompartment::Clear();
  m_SampleFlow = false;
  SAFE_DELETE(m_InFlow);
  SAFE_DELETE(m_OutFlow);
  SAFE_DELETE(m_AverageInFlow);
  SAFE_DELETE(m_AverageOutFlow);
  SAFE_DELETE(m_AverageInFlow_mL_Per_s);
  SAFE_DELETE(m_AverageOutFlow_mL_Per_s);
  SAFE_DELETE(m_Pressure);
  SAFE_DELETE(m_Volume);
  m_Links.clear();
  m_FluidChildren.clear();
  DELETE_VECTOR(m_SubstanceQuantities);
  m_Nodes.Clear();
}

template<FLUID_COMPARTMENT_TEMPLATE>
const SEScalar* SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::GetScalar(const std::string& name)
{
  if (name.compare("InFlow") == 0)
    return &GetInFlow();
  if (name.compare("OutFlow") == 0)
    return &GetOutFlow();
  if (name.compare("AverageInFlow") == 0)
    return &GetAverageInFlow();
  if (name.compare("AverageOutFlow") == 0)
    return &GetAverageOutFlow();
  if (name.compare("Pressure") == 0)
    return &GetPressure();
  if (name.compare("Volume") == 0)
    return &GetVolume();
  return nullptr;
}

template<FLUID_COMPARTMENT_TEMPLATE>
void SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::MapNode(SEFluidCircuitNode& node)
{
  if (HasChildren())
    Fatal("You cannot map nodes to compartments with children.");
  else
    m_Nodes.MapNode(node);
}

template<FLUID_COMPARTMENT_TEMPLATE>
void SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::Sample(bool StartCycle)
{
  if (m_SampleFlow)
  {
    m_AverageInFlow_mL_Per_s->Sample(GetInFlow(VolumePerTimeUnit::mL_Per_s));
    m_AverageOutFlow_mL_Per_s->Sample(GetOutFlow(VolumePerTimeUnit::mL_Per_s));
    if (StartCycle)
    {
      m_AverageInFlow->SetValue(m_AverageInFlow_mL_Per_s->Value(), VolumePerTimeUnit::mL_Per_s);
      m_AverageOutFlow->SetValue(m_AverageOutFlow_mL_Per_s->Value(), VolumePerTimeUnit::mL_Per_s);
      m_AverageInFlow_mL_Per_s->Invalidate();
      m_AverageOutFlow_mL_Per_s->Invalidate();
    }
  }
}

template<FLUID_COMPARTMENT_TEMPLATE>
void SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::SampleFlow()
{
  m_SampleFlow = true;
  GetAverageInFlow();
  GetAverageOutFlow();
}

template<FLUID_COMPARTMENT_TEMPLATE>
bool SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::HasInFlow() const
{
  if (m_Links.empty())
    return false;
  for (auto* link : m_Links)
    if (link->HasFlow())
      return true;
  return false;
}
template<FLUID_COMPARTMENT_TEMPLATE>
const SEScalarVolumePerTime& SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::GetInFlow() const
{
  if (m_InFlow == nullptr)
    m_InFlow = new SEScalarVolumePerTime();
  m_InFlow->SetReadOnly(false);
  if (m_Links.empty())
    m_InFlow->Invalidate();
  else
    m_InFlow->SetValue(CalculateInFlow_mL_Per_s(), VolumePerTimeUnit::mL_Per_s);
  m_InFlow->SetReadOnly(true);
  return *m_InFlow;
}
template<FLUID_COMPARTMENT_TEMPLATE>
double SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::GetInFlow(const VolumePerTimeUnit& unit) const
{
  if (m_Links.empty())
    return SEScalar::dNaN();
  return Convert(CalculateInFlow_mL_Per_s(), VolumePerTimeUnit::mL_Per_s, unit);
}

template<FLUID_COMPARTMENT_TEMPLATE>
bool SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::HasAverageInFlow() const
{
  return m_AverageInFlow == nullptr ? false : m_AverageInFlow->IsValid();
}
template<FLUID_COMPARTMENT_TEMPLATE>
const SEScalarVolumePerTime& SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::GetAverageInFlow() const
{
  if (m_AverageInFlow == nullptr)
  {
    m_AverageInFlow = new SEScalarVolumePerTime();
    m_AverageInFlow_mL_Per_s = new SERunningAverage();
  }
  return *m_AverageInFlow;
}
template<FLUID_COMPARTMENT_TEMPLATE>
double SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::GetAverageInFlow(const VolumePerTimeUnit& unit) const
{
  if (m_AverageInFlow == nullptr)
    return SEScalar::dNaN();
  return m_AverageInFlow->GetValue(unit);
}

template<FLUID_COMPARTMENT_TEMPLATE>
bool SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::HasOutFlow() const
{
  if (m_Links.empty())
    return false;
  for (auto* link : m_Links)
    if (link->HasFlow())
      return true;
  return false;
}
template<FLUID_COMPARTMENT_TEMPLATE>
const SEScalarVolumePerTime& SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::GetOutFlow() const
{
  if (m_OutFlow == nullptr)
    m_OutFlow = new SEScalarVolumePerTime();
  m_OutFlow->SetReadOnly(false);
  if (m_Links.empty())
    m_OutFlow->Invalidate();
  else
    m_OutFlow->SetValue(CalculateOutFlow_mL_Per_s(), VolumePerTimeUnit::mL_Per_s);
  m_OutFlow->SetReadOnly(true);
  return *m_OutFlow;
}
template<FLUID_COMPARTMENT_TEMPLATE>
double SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::GetOutFlow(const VolumePerTimeUnit& unit) const
{
  if (m_Links.empty())
    return SEScalar::dNaN();
  return Convert(CalculateOutFlow_mL_Per_s(), VolumePerTimeUnit::mL_Per_s, unit);
}

template<FLUID_COMPARTMENT_TEMPLATE>
bool SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::HasAverageOutFlow() const
{
  return m_AverageOutFlow == nullptr ? false : m_AverageOutFlow->IsValid();
}
template<FLUID_COMPARTMENT_TEMPLATE>
const SEScalarVolumePerTime& SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::GetAverageOutFlow() const
{
  if (m_AverageOutFlow == nullptr)
  {
    m_AverageOutFlow = new SEScalarVolumePerTime();
    m_AverageOutFlow_mL_Per_s = new SERunningAverage();
  }
  return *m_AverageOutFlow;
}
template<FLUID_COMPARTMENT_TEMPLATE>
double SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::GetAverageOutFlow(const VolumePerTimeUnit& unit) const
{
  if (m_AverageOutFlow == nullptr)
    return SEScalar::dNaN();
  return m_AverageOutFlow->GetValue(unit);
}

template<FLUID_COMPARTMENT_TEMPLATE>
double SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::CalculateInFlow_mL_Per_s() const
{
  double flow_mL_Per_s = 0;

  SEScalarVolumePerTime* f;
  for (auto* link : m_IncomingLinks)
  {
    // Positive flow on an incoming path, is flow into the compartment
    f = &link->GetFlow();
    if (f->IsPositive() || f->IsZero())
      flow_mL_Per_s += f->GetValue(VolumePerTimeUnit::mL_Per_s);
  }
  for (auto* link : m_OutgoingLinks)
  {// Negative flow on an outgoing path, is flow into the compartment
    f = &link->GetFlow();
    if (f->IsNegative() || f->IsZero())
      flow_mL_Per_s += -f->GetValue(VolumePerTimeUnit::mL_Per_s);

  }
  if (flow_mL_Per_s < 0)
    flow_mL_Per_s = 0;// This number is something like x.e-12, which we treat as 0
  return flow_mL_Per_s;
}

template<FLUID_COMPARTMENT_TEMPLATE>
double SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::CalculateOutFlow_mL_Per_s() const
{
  double flow_mL_Per_s = 0;

  SEScalarVolumePerTime* f;
  for (auto* link : m_IncomingLinks)
  {
    // Negative flow on an incoming path, is flow out of the compartment
    f = &link->GetFlow();
    if (f->IsNegative() || f->IsZero())
      flow_mL_Per_s += -f->GetValue(VolumePerTimeUnit::mL_Per_s);
  }
  for (auto* link : m_OutgoingLinks)
  {// Positive flow on an outgoing path, is flow out of the compartment
    f = &link->GetFlow();
    if (f->IsPositive() || f->IsZero())
      flow_mL_Per_s += f->GetValue(VolumePerTimeUnit::mL_Per_s);
  }
  if (flow_mL_Per_s < 0)
    flow_mL_Per_s = 0;// This number is something like x.e-12, which we treat as 0
  return flow_mL_Per_s;
}

template<FLUID_COMPARTMENT_TEMPLATE>
bool SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::HasPressure() const
{
  if (m_Nodes.HasMapping())
    return m_Nodes.HasPotential();
  if (!m_FluidChildren.empty())
  {
    for (auto* child : m_FluidChildren)
      if (child->HasPressure())
        return true;
    return false;
  }
  return m_Pressure == nullptr ? false : m_Pressure->IsValid();
}
template<FLUID_COMPARTMENT_TEMPLATE>
SEScalarPressure& SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::GetPressure()
{
  if (m_Nodes.HasMapping())
    return m_Nodes.GetPotential();
  if (m_Pressure == nullptr)
    m_Pressure = new SEScalarPressure(); 
  if (!m_FluidChildren.empty())
  {
    m_Pressure->SetReadOnly(false);
    m_Pressure->Invalidate();
    const PressureUnit* pUnit = nullptr;
    for (SEFluidCompartment* child : m_FluidChildren)
    {
      if (child->HasPressure())
      {
        pUnit = child->GetPressure().GetUnit();
        break;
      }
    }
    if(pUnit!=nullptr)
      m_Pressure->SetValue(GetPressure(*pUnit),*pUnit);
    m_Pressure->SetReadOnly(true);
  }
  return *m_Pressure;
}
template<FLUID_COMPARTMENT_TEMPLATE>
double SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::GetPressure(const PressureUnit& unit) const
{
  if (!HasPressure())
    return SEScalar::dNaN();
  if (m_Nodes.HasMapping())
    return m_Nodes.GetPotential(unit);
  if (!m_FluidChildren.empty())
  {
    double Pressure = 0;
    if (HasVolume())
    {
      double totalVolume_mL = GetVolume(VolumeUnit::mL);
      for (SEFluidCompartment* child : m_FluidChildren)
      {
        if (child->HasPressure() && child->HasVolume())
        {
          Pressure += child->GetPressure(unit) * (child->GetVolume(VolumeUnit::mL) / totalVolume_mL);
        }
      }
      return Pressure;
    }
    else
    {
      int numHas = 0;
      for (SEFluidCompartment* child : m_FluidChildren)
      {
        if (child->HasPressure())
        {
          numHas++;
          Pressure += child->GetPressure(unit);
        }
        Pressure /= numHas;
      }
      return Pressure;
    }
  }
  if (m_Pressure == nullptr)
    return SEScalar::dNaN();
  return m_Pressure->GetValue(unit);
}

template<FLUID_COMPARTMENT_TEMPLATE>
bool SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::HasVolume() const
{
  if (m_Nodes.HasMapping())
    return m_Nodes.HasQuantity();
  if (!m_FluidChildren.empty())
  {
    for (auto* child : m_FluidChildren)
      if (child->HasVolume())
        return true;
    return false;
  }
  return m_Volume == nullptr ? false : m_Volume->IsValid();
}
template<FLUID_COMPARTMENT_TEMPLATE>
SEScalarVolume& SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::GetVolume()
{
  if (m_Nodes.HasMapping())
    return m_Nodes.GetQuantity();
  if (m_Volume == nullptr)
    m_Volume = new SEScalarVolume();
  if (!m_FluidChildren.empty())
  {
    m_Volume->SetReadOnly(false);
    m_Volume->Invalidate();
    for (SEFluidCompartment* child : m_FluidChildren)
      if (child->HasVolume())
        m_Volume->Increment(child->GetVolume());
    m_Volume->SetReadOnly(true);
  }
  return *m_Volume;
}
template<FLUID_COMPARTMENT_TEMPLATE>
double SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::GetVolume(const VolumeUnit& unit) const
{
  if (m_Nodes.HasMapping())
    return m_Nodes.GetQuantity(unit);
  if (!m_FluidChildren.empty())
  {
    double volume = 0;
    for (SEFluidCompartment* child : m_FluidChildren)
      if (child->HasVolume())
        volume += child->GetVolume(unit);
    return volume;
  }
  if (m_Volume == nullptr)
    return SEScalar::dNaN();
  return m_Volume->GetValue(unit);
}

template<FLUID_COMPARTMENT_TEMPLATE>
bool SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::HasSubstanceQuantities() const
{
  return !m_SubstanceQuantities.empty();
}
template<FLUID_COMPARTMENT_TEMPLATE>
bool SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::HasSubstanceQuantity(const SESubstance& substance) const
{
  for (SubstanceQuantityType* sq : m_SubstanceQuantities)
  {
    if (&sq->GetSubstance() == &substance)
      return true;
  }
  return false;
}

template<FLUID_COMPARTMENT_TEMPLATE>
SubstanceQuantityType* SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::GetSubstanceQuantity(const SESubstance& substance) const
{
  for (SubstanceQuantityType* sq : m_SubstanceQuantities)
  {
    if (&sq->GetSubstance() == &substance)
      return sq;
  }
  return nullptr;
}
template<FLUID_COMPARTMENT_TEMPLATE>
const std::vector<SubstanceQuantityType*>& SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::GetSubstanceQuantities() const
{
  return m_SubstanceQuantities;
}
template<FLUID_COMPARTMENT_TEMPLATE>
void SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::RemoveSubstanceQuantity(const SESubstance& substance)
{
  for (size_t i = 0; i < m_SubstanceQuantities.size(); i++)
  {
    if (&m_SubstanceQuantities[i]->GetSubstance() == &substance)
    {
      SAFE_DELETE(m_SubstanceQuantities[i]);
      SAFE_DELETE(m_TransportSubstances[i]);// Assumes these are in sync
      m_SubstanceQuantities.erase(m_SubstanceQuantities.begin()+i);
      m_TransportSubstances.erase(m_TransportSubstances.begin()+i);
      
    }
  }
}
template<FLUID_COMPARTMENT_TEMPLATE>
void SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::ZeroSubstanceQuantities()
{
  for (SubstanceQuantityType* sq : m_SubstanceQuantities)
  {
    sq->SetToZero();
  }
}

template<FLUID_COMPARTMENT_TEMPLATE>
void SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::AddLink(LinkType& link)
{
  if (!Contains(m_Links, link))
  {
    m_Links.push_back(&link);
    if (HasChildren())
    {
      SEFluidCompartment& src = link.GetSourceCompartment();
      SEFluidCompartment& tgt = link.GetTargetCompartment();

      if(this!= &src && !HasChild(src))
        m_IncomingLinks.push_back(&link);
      else if (this!=&tgt && !HasChild(tgt))
        m_OutgoingLinks.push_back(&link);
    }
    else
    {
      // Is it incoming or out going?
      if (this == &link.GetSourceCompartment())
        m_OutgoingLinks.push_back(&link);
      else if (this == &link.GetTargetCompartment())
        m_IncomingLinks.push_back(&link);
    }
  }
}
template<FLUID_COMPARTMENT_TEMPLATE>
void SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::RemoveLink(LinkType& link)
{
  Remove(m_Links, &link);
  Remove(m_IncomingLinks, &link);
  Remove(m_OutgoingLinks, &link);
}
template<FLUID_COMPARTMENT_TEMPLATE>
void SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::RemoveLinks()
{
  m_Links.clear();
}
template<FLUID_COMPARTMENT_TEMPLATE>
const std::vector<LinkType*>& SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::GetLinks()
{
  return m_Links;
}

template<FLUID_COMPARTMENT_TEMPLATE>
bool SEFluidCompartment<FLUID_COMPARTMENT_TYPES>::HasChild(const SEFluidCompartment& cmpt)
{
  for (SEFluidCompartment* child : m_FluidChildren)
  {
    if (&cmpt == child)
      return true;
    if (child->HasChild(cmpt))
      return true;
  }
  return false;
}
