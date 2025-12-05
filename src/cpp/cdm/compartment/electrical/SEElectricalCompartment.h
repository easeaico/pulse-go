/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#pragma once
#include "cdm/blackbox/SEBlackBox.h"
#include "cdm/compartment/SECompartment.h"
#include "cdm/compartment/SECompartmentNodes.h"
#include "cdm/circuit/electrical/SEElectricalCircuitNode.h"
class SEElectricalCircuitNode;
class SEElectricalCompartmentLink;

class CDM_DECL SEElectricalCompartment : public SECompartment
{
  friend class PBCompartment;//friend the serialization class
  friend class SECompartmentManager;
  template<typename CompartmentType, typename CompartmentLinkType> friend class SECompartmentGraph;
protected:
  SEElectricalCompartment(const std::string& name, Logger* logger);
public:
  virtual ~SEElectricalCompartment();

  void Clear() override;

  const SEScalar* GetScalar(const std::string& name) override;

  virtual bool HasChildren() const override { return !m_Children.empty(); }
  virtual const std::vector<SEElectricalCompartment*>& GetChildren() { return m_Children; }
  virtual const std::vector<SEElectricalCompartment*>& GetLeaves() { return m_Leaves; }

  virtual bool HasNodeMapping() const { return m_Nodes.HasMapping(); }
  virtual  SECompartmentNodes<ELECTRICAL_COMPARTMENT_NODE>& GetNodeMapping() { return m_Nodes; }
  virtual void MapNode(SEElectricalCircuitNode& node);

  void StateChange() override;

  virtual bool HasCurrentIn() const;
  virtual const SEScalarElectricCurrent& GetCurrentIn() const;
  virtual double GetCurrentIn(const ElectricCurrentUnit& unit) const;

  virtual bool HasCurrentOut() const;
  virtual const SEScalarElectricCurrent& GetCurrentOut() const;
  virtual double GetCurrentOut(const ElectricCurrentUnit& unit) const;

  virtual bool HasCharge() const;
  virtual SEScalarElectricCharge& GetCharge();
  virtual double GetCharge(const ElectricChargeUnit& unit) const;

  virtual bool HasVoltage() const;
  virtual SEScalarElectricPotential& GetVoltage();
  virtual double GetVoltage(const ElectricPotentialUnit& unit) const;

  virtual void AddLink(SEElectricalCompartmentLink& link);
  virtual void RemoveLink(SEElectricalCompartmentLink& link);
  virtual void RemoveLinks();
  virtual const std::vector<SEElectricalCompartmentLink*>& GetLinks();

  virtual bool HasChild(const SEElectricalCompartment& cmpt);
  virtual void AddChild(SEElectricalCompartment& child);

protected:
  virtual double CalculateInflow_A() const;
  virtual double CalculateOutflow_A() const;

  mutable SEScalarElectricCurrent* m_CurrentIn;
  mutable SEScalarElectricCurrent* m_CurrentOut;
  SEScalarElectricCharge*          m_Charge;
  SEScalarElectricPotential*       m_Voltage;

  std::vector<SEElectricalCompartmentLink*> m_Links;
  std::vector<SEElectricalCompartmentLink*> m_IncomingLinks;
  std::vector<SEElectricalCompartmentLink*> m_OutgoingLinks;

  std::vector<SEElectricalCompartment*> m_Children;
  std::vector<SEElectricalCompartment*> m_Leaves;
  SECompartmentNodes<ELECTRICAL_COMPARTMENT_NODE>  m_Nodes;
};
