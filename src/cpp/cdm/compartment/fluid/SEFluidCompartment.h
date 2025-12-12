/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include "cdm/compartment/SECompartment.h"
#include "cdm/circuit/fluid/SEFluidCircuitNode.h"
#include "cdm/compartment/SECompartmentNodes.h"
#include "cdm/substance/SESubstanceTransport.h"

class SESubstance;
class SERunningAverage;
template<typename EdgeType, typename VertexType, typename CompartmentType> class SEFluidCompartmentLink;

#define FLUID_COMPARTMENT_TEMPLATE typename LinkType, typename VertexType, typename TransportSubstanceType, typename SubstanceQuantityType
#define FLUID_COMPARTMENT_TYPES LinkType, VertexType, TransportSubstanceType, SubstanceQuantityType

template<FLUID_COMPARTMENT_TEMPLATE>
class SEFluidCompartment : public SECompartment, public VertexType
{
  friend class PBCompartment;//friend the serialization class
  template<typename CompartmentType, typename CompartmentLinkType> friend class SECompartmentGraph;
protected:
  SEFluidCompartment(const std::string& name, Logger* logger);
public:
  virtual ~SEFluidCompartment();

  void Clear() override;

  std::string GetName() const override { return m_Name; }

  const SEScalar* GetScalar(const std::string& name) override;

  bool HasChildren() const override { return !m_FluidChildren.empty(); }

  virtual bool HasNodeMapping() const { return m_Nodes.HasMapping(); }
  virtual  SECompartmentNodes<FLUID_COMPARTMENT_NODE>& GetNodeMapping() { return m_Nodes; }
  virtual void MapNode(SEFluidCircuitNode& node);

  virtual void Sample(bool CycleStart);
  virtual void SampleFlow(); // We can add other sampling support as needed, just follow the pattern

  virtual bool HasInflow() const;
  virtual const SEScalarVolumePerTime& GetInflow() const;
  virtual double GetInflow(const VolumePerTimeUnit& unit) const;

  virtual bool HasAverageInflow() const;
  virtual const SEScalarVolumePerTime& GetAverageInflow() const;
  virtual double GetAverageInflow(const VolumePerTimeUnit& unit) const;

  virtual bool HasOutflow() const;
  virtual const SEScalarVolumePerTime& GetOutflow() const;
  virtual double GetOutflow(const VolumePerTimeUnit& unit) const;

  virtual bool HasAverageOutflow() const;
  virtual const SEScalarVolumePerTime& GetAverageOutflow() const;
  virtual double GetAverageOutflow(const VolumePerTimeUnit& unit) const;

  virtual bool HasPressure() const;
  virtual SEScalarPressure& GetPressure();
  virtual double GetPressure(const PressureUnit& unit) const;

  virtual bool HasVolume() const;
  virtual SEScalarVolume& GetVolume();
  virtual double GetVolume(const VolumeUnit& unit) const;

  virtual bool HasSubstanceQuantities() const;
  virtual bool HasSubstanceQuantity(const SESubstance& substance) const;
  virtual SubstanceQuantityType* GetSubstanceQuantity(const SESubstance& substance) const;
  virtual const std::vector<SubstanceQuantityType*>& GetSubstanceQuantities() const;
  virtual void ZeroSubstanceQuantities();

  virtual void AddLink(LinkType& link);
  virtual void RemoveLink(LinkType& link);
  virtual void RemoveLinks();
  virtual const std::vector<LinkType*>& GetLinks();

  virtual bool HasChild(const SEFluidCompartment& cmpt);

protected:
  virtual void RemoveSubstanceQuantity(const SESubstance& substance);

  bool HasQuantity() const override { return HasVolume(); }
  SEScalarVolume& GetQuantity() override { return GetVolume(); }

  virtual double CalculateInflow_mL_Per_s() const;
  virtual double CalculateOutflow_mL_Per_s() const;

  std::vector<TransportSubstanceType*>& GetTransportSubstances() override { return m_TransportSubstances; }

  mutable SEScalarVolumePerTime* m_Inflow;
  mutable SEScalarVolumePerTime* m_Outflow;
  bool                           m_SampleFlow;
  mutable SEScalarVolumePerTime* m_AverageInflow;
  mutable SEScalarVolumePerTime* m_AverageOutflow;
  mutable SERunningAverage*      m_AverageInflow_mL_Per_s;
  mutable SERunningAverage*      m_AverageOutflow_mL_Per_s;

  SEScalarPressure*              m_Pressure;
  SEScalarVolume*                m_Volume;

  std::vector<SubstanceQuantityType*>  m_SubstanceQuantities;
  std::vector<TransportSubstanceType*> m_TransportSubstances;

  std::vector<LinkType*> m_Links;
  std::vector<LinkType*> m_IncomingLinks;
  std::vector<LinkType*> m_OutgoingLinks;
  std::vector<SEFluidCompartment*> m_FluidChildren;
  SECompartmentNodes<FLUID_COMPARTMENT_NODE>  m_Nodes;
};

#include "cdm/compartment/fluid/SEFluidCompartment.hxx"
