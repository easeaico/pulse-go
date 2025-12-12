/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/compartment/thermal/SEThermalCompartment.h"
#include "cdm/compartment/thermal/SEThermalCompartmentLink.h"
#include "cdm/circuit/thermal/SEThermalCircuitNode.h"
#include "cdm/properties/SEScalarPower.h"
#include "cdm/properties/SEScalarEnergy.h"
#include "cdm/properties/SEScalarTemperature.h"

SEThermalCompartment::SEThermalCompartment(const std::string& name, Logger* logger) : SECompartment(name, logger), m_Nodes(logger)
{
  m_HeatTransferRateIn = nullptr;
  m_HeatTransferRateOut = nullptr;
  m_Heat = nullptr;
  m_Temperature = nullptr;
}

SEThermalCompartment::~SEThermalCompartment()
{
  Clear();
}

void SEThermalCompartment::Clear()
{
  SECompartment::Clear();
  SAFE_DELETE(m_HeatTransferRateIn);
  SAFE_DELETE(m_HeatTransferRateOut);
  SAFE_DELETE(m_Heat);
  SAFE_DELETE(m_Temperature); 
  m_Links.clear();
  m_Children.clear();
  m_Nodes.Clear();
}

const SEScalar* SEThermalCompartment::GetScalar(const std::string& name)
{
  if (name.compare("HeatTransferRateIn") == 0)
    return &GetHeatTransferRateIn();
  if (name.compare("HeatTransferRateOut") == 0)
    return &GetHeatTransferRateOut();
  if (name.compare("Heat") == 0)
    return &GetHeat();
  if (name.compare("Temperature") == 0)
    return &GetTemperature();
  return nullptr;
}

void SEThermalCompartment::StateChange()
{
  m_Leaves.clear();
  FindLeaves<SEThermalCompartment>(*this, m_Leaves);
  m_Nodes.StateChange();
}

void SEThermalCompartment::MapNode(SEThermalCircuitNode& node)
{
  if (HasChildren())
    Fatal("You cannot map nodes to compartments with children.");
  else
    m_Nodes.MapNode(node);
}

bool SEThermalCompartment::HasHeat() const
{
  if (m_Nodes.HasMapping())
    return m_Nodes.HasQuantity();
  if (!m_Children.empty())
  {
    for (auto* child : m_Children)
      if (child->HasHeat())
        return true;
    return false;
  }
  return m_Heat == nullptr ? false : m_Heat->IsValid();
}
SEScalarEnergy& SEThermalCompartment::GetHeat()
{
  if (m_Nodes.HasMapping())
    return m_Nodes.GetQuantity();
  if (m_Heat == nullptr)
    m_Heat = new SEScalarEnergy();
  if (!m_Children.empty())
  {
    m_Heat->SetReadOnly(false);
    m_Heat->Invalidate();
    for (SEThermalCompartment* child : m_Children)
      if(child->HasHeat())
        m_Heat->Increment(child->GetHeat());
    m_Heat->SetReadOnly(true);
  }
  return *m_Heat;
}
double SEThermalCompartment::GetHeat(const EnergyUnit& unit) const
{
  if (m_Nodes.HasMapping())
    return m_Nodes.GetQuantity(unit);
  if (!m_Children.empty())
  {
    double heat = 0;
    for (SEThermalCompartment* child : m_Children)
      if(child->HasHeat())
        heat += child->GetHeat(unit);
    return heat;
  }
  if (m_Heat == nullptr)
    return SEScalar::dNaN();
  return m_Heat->GetValue(unit);
}

bool SEThermalCompartment::HasTemperature() const
{
  if (m_Nodes.HasMapping())
    return m_Nodes.HasPotential();
  if (!m_Children.empty())
  {
    for (auto* child : m_Children)
      if (child->HasTemperature())
        return true;
    return false;
  }
  return m_Temperature == nullptr ? false : m_Temperature->IsValid();
}
SEScalarTemperature& SEThermalCompartment::GetTemperature()
{
  if (m_Nodes.HasMapping())
    return m_Nodes.GetPotential();
  if (m_Temperature == nullptr)
    m_Temperature = new SEScalarTemperature();
  if (!m_Children.empty())
  {
    m_Temperature->SetReadOnly(false);
    const TemperatureUnit* tUnit = nullptr;
    for (SEThermalCompartment* child : m_Children)
    {
      if (child->HasTemperature())
      {
        tUnit = child->GetTemperature().GetUnit();
        break;
      }
    }
    if (tUnit != nullptr)
      m_Temperature->SetValue(GetTemperature(*tUnit), *tUnit);
    m_Temperature->SetReadOnly(true);
  }
  return *m_Temperature;
}
double SEThermalCompartment::GetTemperature(const TemperatureUnit& unit) const
{
  if (!HasTemperature())
    return SEScalar::dNaN();
  if (m_Nodes.HasMapping())
    return m_Nodes.GetPotential(unit);
  if (!m_Children.empty())
  {
    double temperature = 0;
    if (HasHeat())
    {
      double totalHeat_J = GetHeat(EnergyUnit::J);
      for (SEThermalCompartment* child : m_Children)
      {
        if (child->HasTemperature() && child->HasHeat())
        {
          temperature += child->GetTemperature(unit) * (child->GetHeat(EnergyUnit::J) / totalHeat_J);
        }
      }
      return temperature;
    }
    else
    {
      int numHas = 0;
      for (SEThermalCompartment* child : m_Children)
      {
        if (child->HasTemperature())
        {
          numHas++;
          temperature += child->GetTemperature(unit);
        }
        temperature /= numHas;
      }
      return temperature;
    }
  }
  if (m_Temperature == nullptr)
    return SEScalar::dNaN();
  return m_Temperature->GetValue(unit);
}

bool SEThermalCompartment::HasHeatTransferRateIn() const
{
  if (m_Links.empty())
    return false;
  for (auto* link : m_Links)
    if (link->HasHeatTransferRate())
      return true;
  return false;
}
const SEScalarPower& SEThermalCompartment::GetHeatTransferRateIn() const
{
  
  if (m_HeatTransferRateIn == nullptr)
    m_HeatTransferRateIn = new SEScalarPower();
  m_HeatTransferRateIn->SetReadOnly(false);
  if (m_Links.empty())
    m_HeatTransferRateIn->Invalidate();
  else
    m_HeatTransferRateIn->SetValue(CalculateInflow_W(), PowerUnit::W);
  m_HeatTransferRateIn->SetReadOnly(true);
  return *m_HeatTransferRateIn;
}
double SEThermalCompartment::GetHeatTransferRateIn(const PowerUnit& unit) const
{
  if (m_Links.empty())
    return SEScalar::dNaN();
  return Convert(CalculateInflow_W(), PowerUnit::W, unit);
}

bool SEThermalCompartment::HasHeatTransferRateOut() const
{
  if (m_Links.empty())
    return false;
  for (auto* link : m_Links)
    if (link->HasHeatTransferRate())
      return true;
  return false;
}
const SEScalarPower& SEThermalCompartment::GetHeatTransferRateOut() const
{
  if (m_HeatTransferRateOut == nullptr)
    m_HeatTransferRateOut = new SEScalarPower();
  m_HeatTransferRateOut->SetReadOnly(false);
  if (m_Links.empty())
    m_HeatTransferRateOut->Invalidate();
  else
    m_HeatTransferRateOut->SetValue(CalculateOutflow_W(), PowerUnit::W);
  m_HeatTransferRateOut->SetReadOnly(true);
  return *m_HeatTransferRateOut;
}
double SEThermalCompartment::GetHeatTransferRateOut(const PowerUnit& unit) const
{
  if (m_Links.empty())
    return SEScalar::dNaN();
  return Convert(CalculateOutflow_W(), PowerUnit::W, unit);
}

double SEThermalCompartment::CalculateInflow_W() const
{
  double flow_W = 0;

  SEScalarPower* f;
  for (SEThermalCompartmentLink* link : m_IncomingLinks)
  {
    // Positive flow on an incoming path, is flow into the compartment
    f = &link->GetHeatTransferRate();
    if (f->IsPositive() || f->IsZero())
      flow_W += f->GetValue(PowerUnit::W);
  }
  for (SEThermalCompartmentLink* link : m_OutgoingLinks)
  {
    // Negative flow on an outgoing path, is flow into the compartment
    f = &link->GetHeatTransferRate();
    if (f->IsNegative() || f->IsZero())
      flow_W += -f->GetValue(PowerUnit::W);
  }
  if (flow_W < 0)
    flow_W = 0;// This number is something like x.e-12, which we treat as 0
  return flow_W;
}

double SEThermalCompartment::CalculateOutflow_W() const
{
  double flow_W = 0;

  SEScalarPower* f;
  for (SEThermalCompartmentLink* link : m_IncomingLinks)
  {
    // Negative flow on an incoming path, is flow out of the compartment
    f = &link->GetHeatTransferRate();
    if (f->IsNegative() || f->IsZero())
      flow_W += -f->GetValue(PowerUnit::W);
  }
  for (SEThermalCompartmentLink* link : m_OutgoingLinks)
  {
    // Positive flow on an outgoing path, is flow out of the compartment
    f = &link->GetHeatTransferRate();
    if (f->IsPositive() || f->IsZero())
      flow_W += f->GetValue(PowerUnit::W);
  }
  if (flow_W < 0)
    flow_W = 0;// This number is something like x.e-12, which we treat as 0
  return flow_W;
}

void SEThermalCompartment::AddLink(SEThermalCompartmentLink& link)
{
  if (!Contains(m_Links, link))
  {
    m_Links.push_back(&link);
    if (HasChildren())
    {
      SEThermalCompartment& src = link.GetSourceCompartment();
      SEThermalCompartment& tgt = link.GetTargetCompartment();

      if (this != &src && !HasChild(src))
        m_IncomingLinks.push_back(&link);
      else if (this != &tgt && !HasChild(tgt))
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
void SEThermalCompartment::RemoveLink(SEThermalCompartmentLink& link)
{
  Remove(m_Links, &link);
  Remove(m_IncomingLinks, &link);
  Remove(m_OutgoingLinks, &link);
}
void SEThermalCompartment::RemoveLinks()
{
  m_Links.clear();
}
const std::vector<SEThermalCompartmentLink*>& SEThermalCompartment::GetLinks()
{
  return m_Links;
}

bool SEThermalCompartment::HasChild(const SEThermalCompartment& cmpt)
{
  for (SEThermalCompartment* child : m_Children)
  {
    if (&cmpt == child)
      return true;
    if (child->HasChild(cmpt))
      return true;
  }
  return false;
}

void SEThermalCompartment::AddChild(SEThermalCompartment& child)
{
  if (HasNodeMapping())
  {
    Fatal("You cannont add a child compartment to a compartment mapped to nodes");
    return;
  }
  if (HasChild(child))
    return;
  m_Children.push_back(&child);
}
