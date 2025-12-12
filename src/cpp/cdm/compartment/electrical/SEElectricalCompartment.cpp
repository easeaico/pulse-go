/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/compartment/electrical/SEElectricalCompartment.h"
#include "cdm/compartment/electrical/SEElectricalCompartmentLink.h"
#include "cdm/circuit/electrical/SEElectricalCircuitNode.h"
#include "cdm/properties/SEScalarElectricCurrent.h"
#include "cdm/properties/SEScalarElectricCharge.h"
#include "cdm/properties/SEScalarElectricPotential.h"

SEElectricalCompartment::SEElectricalCompartment(const std::string& name, Logger* logger) : SECompartment(name, logger), m_Nodes(logger)
{
  m_CurrentIn = nullptr;
  m_CurrentOut = nullptr;
  m_Charge = nullptr;
  m_Voltage = nullptr;
}

SEElectricalCompartment::~SEElectricalCompartment()
{
  Clear();
}

void SEElectricalCompartment::Clear()
{
  SECompartment::Clear();
  SAFE_DELETE(m_CurrentIn);
  SAFE_DELETE(m_CurrentOut);
  SAFE_DELETE(m_Charge);
  SAFE_DELETE(m_Voltage); 
  m_Links.clear();
  m_Children.clear();
  m_Nodes.Clear();
}

const SEScalar* SEElectricalCompartment::GetScalar(const std::string& name)
{
  if (name.compare("CurrentIn") == 0)
    return &GetCurrentIn();
  if (name.compare("CurrentOut") == 0)
    return &GetCurrentOut();
  if (name.compare("Charge") == 0)
    return &GetCharge();
  if (name.compare("Voltage") == 0)
    return &GetVoltage();
  return nullptr;
}

void SEElectricalCompartment::StateChange()
{
  m_Leaves.clear();
  FindLeaves<SEElectricalCompartment>(*this, m_Leaves);
  m_Nodes.StateChange();
}

void SEElectricalCompartment::MapNode(SEElectricalCircuitNode& node)
{
  if (HasChildren())
    Fatal("You cannot map nodes to compartments with children.");
  else
    m_Nodes.MapNode(node);
}

bool SEElectricalCompartment::HasCharge() const
{
  if (m_Nodes.HasMapping())
    return m_Nodes.HasQuantity();
  if (!m_Children.empty())
  {
    for (auto* child : m_Children)
      if (child->HasCharge())
        return true;
    return false;
  }
  return m_Charge == nullptr ? false : m_Charge->IsValid();
}
SEScalarElectricCharge& SEElectricalCompartment::GetCharge()
{
  if (m_Nodes.HasMapping())
    return m_Nodes.GetQuantity();
  if (m_Charge == nullptr)
    m_Charge = new SEScalarElectricCharge();
  if (!m_Children.empty())
  {
    m_Charge->SetReadOnly(false);
    m_Charge->Invalidate();
    for (SEElectricalCompartment* child : m_Children)
      if(child->HasCharge())
        m_Charge->Increment(child->GetCharge());
    m_Charge->SetReadOnly(true);
  }
  return *m_Charge;
}
double SEElectricalCompartment::GetCharge(const ElectricChargeUnit& unit) const
{
  if (m_Nodes.HasMapping())
    return m_Nodes.GetQuantity(unit);
  if (!m_Children.empty())
  {
    double Charge = 0;
    for (SEElectricalCompartment* child : m_Children)
      if(child->HasCharge())
        Charge += child->GetCharge(unit);
    return Charge;
  }
  if (m_Charge == nullptr)
    return SEScalar::dNaN();
  return m_Charge->GetValue(unit);
}

bool SEElectricalCompartment::HasVoltage() const
{
  if (m_Nodes.HasMapping())
    return m_Nodes.HasPotential();
  if (!m_Children.empty())
  {
    for (auto* child : m_Children)
      if (child->HasVoltage())
        return true;
    return false;
  }
  return m_Voltage == nullptr ? false : m_Voltage->IsValid();
}
SEScalarElectricPotential& SEElectricalCompartment::GetVoltage()
{
  if (m_Nodes.HasMapping())
    return m_Nodes.GetPotential();
  if (m_Voltage == nullptr)
    m_Voltage = new SEScalarElectricPotential();
  if (!m_Children.empty())
  {
    m_Voltage->SetReadOnly(false);
    const ElectricPotentialUnit* tUnit = nullptr;
    for (SEElectricalCompartment* child : m_Children)
    {
      if (child->HasVoltage())
      {
        tUnit = child->GetVoltage().GetUnit();
        break;
      }
    }
    if (tUnit != nullptr)
      m_Voltage->SetValue(GetVoltage(*tUnit), *tUnit);
    m_Voltage->SetReadOnly(true);
  }
  return *m_Voltage;
}
double SEElectricalCompartment::GetVoltage(const ElectricPotentialUnit& unit) const
{
  if (!HasVoltage())
    return SEScalar::dNaN();
  if (m_Nodes.HasMapping())
    return m_Nodes.GetPotential(unit);
  if (!m_Children.empty())
  {
    double Voltage = 0;
    if (HasCharge())
    {
      double totalCharge_C = GetCharge(ElectricChargeUnit::C);
      for (SEElectricalCompartment* child : m_Children)
      {
        if (child->HasVoltage() && child->HasCharge())
        {
          Voltage += child->GetVoltage(unit) * (child->GetCharge(ElectricChargeUnit::C) / totalCharge_C);
        }
      }
      return Voltage;
    }
    else
    {
      int numHas = 0;
      for (SEElectricalCompartment* child : m_Children)
      {
        if (child->HasVoltage())
        {
          numHas++;
          Voltage += child->GetVoltage(unit);
        }
        Voltage /= numHas;
      }
      return Voltage;
    }
  }
  if (m_Voltage == nullptr)
    return SEScalar::dNaN();
  return m_Voltage->GetValue(unit);
}

bool SEElectricalCompartment::HasCurrentIn() const
{
  if (m_Links.empty())
    return false;
  for (auto* link : m_Links)
    if (link->HasCurrent())
      return true;
  return false;
}
const SEScalarElectricCurrent& SEElectricalCompartment::GetCurrentIn() const
{
  
  if (m_CurrentIn == nullptr)
    m_CurrentIn = new SEScalarElectricCurrent();
  m_CurrentIn->SetReadOnly(false);
  if (m_Links.empty())
    m_CurrentIn->Invalidate();
  else
    m_CurrentIn->SetValue(CalculateInflow_A(), ElectricCurrentUnit::A);
  m_CurrentIn->SetReadOnly(true);
  return *m_CurrentIn;
}
double SEElectricalCompartment::GetCurrentIn(const ElectricCurrentUnit& unit) const
{
  if (m_Links.empty())
    return SEScalar::dNaN();
  return Convert(CalculateInflow_A(), ElectricCurrentUnit::A, unit);
}

bool SEElectricalCompartment::HasCurrentOut() const
{
  if (m_Links.empty())
    return false;
  for (auto* link : m_Links)
    if (link->HasCurrent())
      return true;
  return false;
}
const SEScalarElectricCurrent& SEElectricalCompartment::GetCurrentOut() const
{
  if (m_CurrentOut == nullptr)
    m_CurrentOut = new SEScalarElectricCurrent();
  m_CurrentOut->SetReadOnly(false);
  if (m_Links.empty())
    m_CurrentOut->Invalidate();
  else
    m_CurrentOut->SetValue(CalculateOutflow_A(), ElectricCurrentUnit::A);
  m_CurrentOut->SetReadOnly(true);
  return *m_CurrentOut;
}
double SEElectricalCompartment::GetCurrentOut(const ElectricCurrentUnit& unit) const
{
  if (m_Links.empty())
    return SEScalar::dNaN();
  return Convert(CalculateOutflow_A(), ElectricCurrentUnit::A, unit);
}

double SEElectricalCompartment::CalculateInflow_A() const
{
  double flow_W = 0;

  SEScalarElectricCurrent* f;
  for (SEElectricalCompartmentLink* link : m_IncomingLinks)
  {
    // Positive flow on an incoming path, is flow into the compartment
    f = &link->GetCurrent();
    if (f->IsPositive() || f->IsZero())
      flow_W += f->GetValue(ElectricCurrentUnit::A);
  }
  for (SEElectricalCompartmentLink* link : m_OutgoingLinks)
  {
    // Negative flow on an outgoing path, is flow into the compartment
    f = &link->GetCurrent();
    if (f->IsNegative() || f->IsZero())
      flow_W += -f->GetValue(ElectricCurrentUnit::A);
  }
  if (flow_W < 0)
    flow_W = 0;// This number is something like x.e-12, which we treat as 0
  return flow_W;
}

double SEElectricalCompartment::CalculateOutflow_A() const
{
  double flow_W = 0;

  SEScalarElectricCurrent* f;
  for (SEElectricalCompartmentLink* link : m_IncomingLinks)
  {
    // Negative flow on an incoming path, is flow out of the compartment
    f = &link->GetCurrent();
    if (f->IsNegative() || f->IsZero())
      flow_W += -f->GetValue(ElectricCurrentUnit::A);
  }
  for (SEElectricalCompartmentLink* link : m_OutgoingLinks)
  {
    // Positive flow on an outgoing path, is flow out of the compartment
    f = &link->GetCurrent();
    if (f->IsPositive() || f->IsZero())
      flow_W += f->GetValue(ElectricCurrentUnit::A);
  }
  if (flow_W < 0)
    flow_W = 0;// This number is something like x.e-12, which we treat as 0
  return flow_W;
}

void SEElectricalCompartment::AddLink(SEElectricalCompartmentLink& link)
{
  if (!Contains(m_Links, link))
  {
    m_Links.push_back(&link);
    if (HasChildren())
    {
      SEElectricalCompartment& src = link.GetSourceCompartment();
      SEElectricalCompartment& tgt = link.GetTargetCompartment();

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
void SEElectricalCompartment::RemoveLink(SEElectricalCompartmentLink& link)
{
  Remove(m_Links, &link);
  Remove(m_IncomingLinks, &link);
  Remove(m_OutgoingLinks, &link);
}
void SEElectricalCompartment::RemoveLinks()
{
  m_Links.clear();
}
const std::vector<SEElectricalCompartmentLink*>& SEElectricalCompartment::GetLinks()
{
  return m_Links;
}

bool SEElectricalCompartment::HasChild(const SEElectricalCompartment& cmpt)
{
  for (SEElectricalCompartment* child : m_Children)
  {
    if (&cmpt == child)
      return true;
    if (child->HasChild(cmpt))
      return true;
  }
  return false;
}

void SEElectricalCompartment::AddChild(SEElectricalCompartment& child)
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
