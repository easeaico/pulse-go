/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/utils/DataTrack.h"
#include "cdm/utils/FileUtils.h"
#include "cdm/engine/SEDecimalFormat.h"
#include "cdm/circuit/electrical/SEElectricalCircuit.h"
#include "cdm/circuit/fluid/SEFluidCircuit.h"
#include "cdm/circuit/thermal/SEThermalCircuit.h"
#include "cdm/compartment/fluid/SEGasCompartment.h"
#include "cdm/compartment/fluid/SEGasCompartmentLink.h"
#include "cdm/compartment/fluid/SEGasCompartmentGraph.h"
#include "cdm/compartment/substances/SEGasSubstanceQuantity.h"
#include "cdm/compartment/fluid/SELiquidCompartment.h"
#include "cdm/compartment/fluid/SELiquidCompartmentLink.h"
#include "cdm/compartment/fluid/SELiquidCompartmentGraph.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/compartment/substances/SELiquidSubstanceQuantity.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarAmountPerVolume.h"

#include <iterator>

DataTrack::DataTrack() : DataTrack(nullptr)
{
}

DataTrack::DataTrack(Logger* logger) : Loggable(logger)
{
  m_Delimiter = ',';
}

DataTrack::~DataTrack()
{
  Clear();
}

void DataTrack::Clear()
{
  Reset();
  if(m_FileStream.is_open())
    m_FileStream.close();
}

void DataTrack::Reset()
{
  // Clear out our data
  m_Times.clear();
  m_Elements.clear();
}


std::vector<double> const& DataTrack::GetTimes() const
{
  return m_Times;
}

DataTrack::Element& DataTrack::GetElement(size_t idx)
{
  if (idx < m_Elements.size())
    return m_Elements[idx];
  Element e;
  e.name = "Unknown-"+std::to_string(m_Elements.size());
  e.idx = m_Elements.size();
  m_Elements.push_back(e);
  return m_Elements.back();
}
DataTrack::Element& DataTrack::GetElement(std::string const& name)
{
  for (Element& e : m_Elements)
  {
    if (e.name == name)
      return e;
  }
  Element e;
  e.name = name;
  e.idx = m_Elements.size();
  m_Elements.push_back(e);
  return m_Elements.back();
}

void DataTrack::SetFormatting(const std::string& name, const SEDecimalFormat& f)
{
  Element& e = GetElement(name);
  e.format = f;
}
void DataTrack::SetFormatting(const std::string& name, std::streamsize precision)
{
  Element& e = GetElement(name);
  e.format.SetPrecision(precision);
}
void DataTrack::SetDefaultFormatting(std::streamsize precision)
{
  m_DefaultPrecision = precision;
}

void DataTrack::Probe(size_t idx, double value)
{
  Element& e = GetElement(idx);
  e.probe = value;
}
size_t DataTrack::Probe(const std::string& name, double value)
{
  Element& e = GetElement(name);
  e.probe = value;
  return e.idx;
}
size_t DataTrack::Probe(const std::string& name, double value, int i)
{
  std::string count = name + std::to_string(i);
  Element& e = GetElement(count);
  e.probe = value;
  return e.idx;
}

void DataTrack::Probe(const SEFluidCircuit& c)
{
  for (SEFluidCircuitNode* n : c.GetNodes())
  {
    if (n->HasPressure())
    {
      auto unit = n->GetPressure().GetUnit();
      Probe(n->GetName() + "_Pressure_" + unit->GetString(), n->GetPressure().GetValue(*unit));
    }
    if (n->HasVolume())
    {
      auto unit = n->GetVolume().GetUnit();
      Probe(n->GetName() + "_Volume_" + unit->GetString(), n->GetVolume().GetValue(*unit));
    }
  }
  for (SEFluidCircuitPath* p : c.GetPaths())
  {
    if (p->HasSwitch())
      Probe(p->GetName() + "_Switch", p->GetSwitch() == eGate::Open ? 1 : 0);
    if (p->HasValve())
      Probe(p->GetName() + "_Valve", p->GetValve() == eGate::Closed ? 1 : 0);

    if (p->HasResistance())
    {
      auto unit = p->GetResistance().GetUnit();
      Probe(p->GetName() + "_Resistance_" + unit->GetString(), p->GetResistance().GetValue(*unit));
    }
    if (p->HasCompliance())
    {
      auto unit = p->GetCompliance().GetUnit();
      Probe(p->GetName() + "_Compliance_" + unit->GetString(), p->GetCompliance().GetValue(*unit));
    }
    if (p->HasInertance())
    {
      auto unit = p->GetInertance().GetUnit();
      Probe(p->GetName() + "_Inertance_" + unit->GetString(), p->GetInertance().GetValue(*unit));
    }
    if (p->HasFlow())
    {
      auto unit = p->GetFlow().GetUnit();
      Probe(p->GetName() + "_Flow_" + unit->GetString(), p->GetFlow().GetValue(*unit));
    }
    if (p->HasPressureSource())
    {
      auto unit = p->GetPressureSource().GetUnit();
      Probe(p->GetName() + "_PressureSource_" + unit->GetString(), p->GetPressureSource().GetValue(*unit));
    }
    if (p->HasFlowSource())
    {
      auto unit = p->GetFlowSource().GetUnit();
      Probe(p->GetName() + "_FlowSource_" + unit->GetString(), p->GetFlowSource().GetValue(*unit));
    }    
  }
}

void DataTrack::Probe(const SEThermalCircuit& c)
{
  for (SEThermalCircuitNode* n : c.GetNodes())
  {
    if (n->HasTemperature())
    {
      auto unit = n->GetTemperature().GetUnit();
      Probe(n->GetName() + "_Temperature_" + unit->GetString(), n->GetTemperature().GetValue(*unit));
    }
    if (n->HasHeat())
    {
      auto unit = n->GetHeat().GetUnit();
      Probe(n->GetName() + "_Heat_" + unit->GetString(), n->GetHeat().GetValue(*unit));
    }
  }
  for (SEThermalCircuitPath* p : c.GetPaths())
  {
    if (p->HasSwitch())
      Probe(p->GetName() + "_Switch", p->GetSwitch() ==  eGate::Open ? 1 : 0);
    if (p->HasValve())
      Probe(p->GetName() + "_Valve", p->GetValve() == eGate::Closed ? 1 : 0);

    if (p->HasResistance())
    {
      auto unit = p->GetResistance().GetUnit();
      Probe(p->GetName() + "_Resistance_" + unit->GetString(), p->GetResistance().GetValue(*unit));
    }
    if (p->HasCapacitance())
    {
      auto unit = p->GetCapacitance().GetUnit();
      Probe(p->GetName() + "_Capacitance_" + unit->GetString(), p->GetCapacitance().GetValue(*unit));
    }
    if (p->HasInductance())
    {
      auto unit = p->GetInductance().GetUnit();
      Probe(p->GetName() + "_Inductance_" + unit->GetString(), p->GetInductance().GetValue(*unit));
    }
    if (p->HasHeatTransferRate())
    {
      auto unit = p->GetHeatTransferRate().GetUnit();
      Probe(p->GetName() + "_HeatTransferRate_" + unit->GetString(), p->GetHeatTransferRate().GetValue(*unit));
    }
    if (p->HasTemperatureSource())
    {
      auto unit = p->GetTemperatureSource().GetUnit();
      Probe(p->GetName() + "_TemperatureSource_" + unit->GetString(), p->GetTemperatureSource().GetValue(*unit));
    }
    if (p->HasHeatSource())
    {
      auto unit = p->GetHeatSource().GetUnit();
      Probe(p->GetName() + "_HeatSource_" + unit->GetString(), p->GetHeatSource().GetValue(*unit));
    }    
  }
}

void DataTrack::Probe(const SEElectricalCircuit& c)
{
  for (SEElectricalCircuitNode* n : c.GetNodes())
  {
    if (n->HasVoltage())
    {
      auto unit = n->GetVoltage().GetUnit();
      Probe(n->GetName() + "_Voltage_" + unit->GetString(), n->GetVoltage().GetValue(*unit));
    }
    if (n->HasCharge())
    {
      auto unit = n->GetCharge().GetUnit();
      Probe(n->GetName() + "_Charge_" + unit->GetString(), n->GetCharge().GetValue(*unit));
    }
  }
  for (SEElectricalCircuitPath* p : c.GetPaths())
  {
    if (p->HasSwitch())
      Probe(p->GetName() + "_Switch", p->GetSwitch() == eGate::Open ? 1 : 0);
    if (p->HasValve())
      Probe(p->GetName() + "_Valve", p->GetValve() == eGate::Closed ? 1 : 0);

    if (p->HasResistance())
    {
      auto unit = p->GetResistance().GetUnit();
      Probe(p->GetName() + "_Resistance_" + unit->GetString(), p->GetResistance().GetValue(*unit));
    }
    if (p->HasCapacitance())
    {
      auto unit = p->GetCapacitance().GetUnit();
      Probe(p->GetName() + "_Capacitance_" + unit->GetString(), p->GetCapacitance().GetValue(*unit));
    }
    if (p->HasInductance())
    {
      auto unit = p->GetInductance().GetUnit();
      Probe(p->GetName() + "_Inductance_" + unit->GetString(), p->GetInductance().GetValue(*unit));
    }
    if (p->HasCurrent())
    {
      auto unit = p->GetCurrent().GetUnit();
      Probe(p->GetName() + "_Current_" + unit->GetString(), p->GetCurrent().GetValue(*unit));
    }
    if (p->HasVoltageSource())
    {
      auto unit = p->GetVoltageSource().GetUnit();
      Probe(p->GetName() + "_VoltageSource_" + unit->GetString(), p->GetVoltageSource().GetValue(*unit));
    }
    if (p->HasCurrentSource())
    {
      auto unit = p->GetCurrentSource().GetUnit();
      Probe(p->GetName() + "_CurrentSource_" + unit->GetString(), p->GetCurrentSource().GetValue(*unit));
    }    
  }
}

void DataTrack::Probe(const SELiquidCompartmentGraph& graph)
{
  for (SELiquidCompartment* cmpt : graph.GetCompartments())
  {
    if (cmpt->HasPressure())
    {
      auto unit = cmpt->GetPressure().GetUnit();
      Probe(cmpt->GetName() + "_Pressure_" + unit->GetString(), cmpt->GetPressure().GetValue(*unit));
    }
    if (cmpt->HasVolume())
    {
      auto unit = cmpt->GetVolume().GetUnit();
      Probe(cmpt->GetName() + "_Volume_" + unit->GetString(), cmpt->GetVolume().GetValue(*unit));
    }
    if (cmpt->HasPH())
    {
      Probe(cmpt->GetName() + "_pH_", cmpt->GetPH().GetValue());
    }

    for (SELiquidSubstanceQuantity* subQ : cmpt->GetSubstanceQuantities())
    {
      if (subQ->HasSaturation())
      {
        Probe(cmpt->GetName() + "_" + subQ->GetSubstance().GetName() + "_Saturation", subQ->GetSaturation().GetValue());
      }
      if (subQ->HasPartialPressure())
      {
        auto unit = subQ->GetPartialPressure().GetUnit();
        Probe(cmpt->GetName() + "_" + subQ->GetSubstance().GetName() + "_PartialPressure_" + unit->GetString(), subQ->GetPartialPressure().GetValue(*unit));
      }
      if (subQ->HasMass())
      {
        auto unit = subQ->GetMass().GetUnit();
        Probe(cmpt->GetName() + "_" + subQ->GetSubstance().GetName() + "_Mass_" + unit->GetString(), subQ->GetMass().GetValue(*unit));
      }
      if (subQ->HasConcentration())
      {
        auto unit = subQ->GetConcentration().GetUnit();
        Probe(cmpt->GetName() + "_" + subQ->GetSubstance().GetName() + "_Concentration_" + unit->GetString(), subQ->GetConcentration().GetValue(*unit));
      }
      if (subQ->HasMolarity())
      {
        auto unit = subQ->GetMolarity().GetUnit();
        Probe(cmpt->GetName() + "_" + subQ->GetSubstance().GetName() + "_Molarity_" + unit->GetString(), subQ->GetMolarity().GetValue(*unit));
      }
    }
  }
  for (SELiquidCompartmentLink* link : graph.GetLinks())
  {
    if (link->HasFlow())
    {
      auto unit = link->GetFlow().GetUnit();
      Probe(link->GetName() + "_Flow_" + unit->GetString(), link->GetFlow(*unit));
    }
  }
}

double DataTrack::GetProbe(size_t idx) const
{
  if (idx >= m_Elements.size())
    return std::numeric_limits<double>::quiet_NaN();
  return m_Elements[idx].probe;
}
double DataTrack::GetProbe(const std::string& name) const
{
  for (const Element& e : m_Elements)
  {
    if (e.name == name)
      return e.probe;
  }
  return std::numeric_limits<double>::quiet_NaN();
}
std::string DataTrack::GetProbeName(size_t idx) const
{
  if (idx >= m_Elements.size())
    return "IndexOutOfBounds";
  return m_Elements[idx].name;
}

size_t DataTrack::Track(const std::string& name, double time, double value)
{
  if (m_LastTime != time)
  {
    m_LastTime = time;
    m_Times.push_back(time);
  }

  // Create element we are to update if we don't have it
  Element& ue = GetElement(name);
  // The Track vector should be the same length as m_Time
  // Add NaN's to all Element tracking vectors to equal the size to m_Time
  for (Element& e : m_Elements)
  {
    while (e.track.size() < m_Times.size())
    {
      e.track.push_back(std::numeric_limits<double>::quiet_NaN());
    }
  }
  ue.track.back() = value;
  return ue.idx;
}

void DataTrack::Track(double time_s, const SEElectricalCircuit& c)
{
  for (SEElectricalCircuitNode* n : c.GetNodes())
  {
    if (n->HasVoltage())
    {
      auto unit = n->GetVoltage().GetUnit();
      Track(n->GetName() + "_Voltage_" + unit->GetString(), time_s, n->GetVoltage().GetValue(*unit));
    }
    if (n->HasCharge())
    {
      auto unit = n->GetCharge().GetUnit();
      Track(n->GetName() + "_Charge_" + unit->GetString(), time_s, n->GetCharge().GetValue(*unit));
    }
  }
  for (SEElectricalCircuitPath* p : c.GetPaths())
  {
    if (p->HasSwitch())
      Track(p->GetName() + "_Switch", time_s, p->GetSwitch() == eGate::Open ? 1 : 0);
    if (p->HasValve())
      Track(p->GetName() + "_Valve", time_s, p->GetValve() == eGate::Closed ? 1 : 0);

    if (p->HasResistance())
    {
      auto unit = p->GetResistance().GetUnit();
      Track(p->GetName() + "_Resistance_" + unit->GetString(), time_s, p->GetResistance().GetValue(*unit));
    }
    if (p->HasCapacitance())
    {
      auto unit = p->GetCapacitance().GetUnit();
      Track(p->GetName() + "_Capacitance_" + unit->GetString(), time_s, p->GetCapacitance().GetValue(*unit));
    }
    if (p->HasInductance())
    {
      auto unit = p->GetInductance().GetUnit();
      Track(p->GetName() + "_Inductance_" + unit->GetString(), time_s, p->GetInductance().GetValue(*unit));
    }
    if (p->HasCurrent())
    {
      auto unit = p->GetCurrent().GetUnit();
      Track(p->GetName() + "_Current_" + unit->GetString(), time_s, p->GetCurrent().GetValue(*unit));
    }
    if (p->HasVoltageSource())
    {
      auto unit = p->GetVoltageSource().GetUnit();
      Track(p->GetName() + "_VoltageSource_" + unit->GetString(), time_s, p->GetVoltageSource().GetValue(*unit));
    }
    if (p->HasCurrentSource())
    {
      auto unit = p->GetCurrentSource().GetUnit();
      Track(p->GetName() + "_CurrentSource_" + unit->GetString(), time_s, p->GetCurrentSource().GetValue(*unit));
    }    
  }
}
void DataTrack::Track(double time_s, const SEFluidCircuit& c)
{
  for (SEFluidCircuitNode* n : c.GetNodes())
  {
    if (n->HasPressure())
    {
      auto unit = n->GetPressure().GetUnit();
      Track(n->GetName() + "_Pressure_" + unit->GetString(), time_s, n->GetPressure().GetValue(*unit));
    }
    if (n->HasVolume())
    {
      auto unit = n->GetVolume().GetUnit();
      Track(n->GetName() + "_Volume_" + unit->GetString(), time_s, n->GetVolume().GetValue(*unit));
    }
  }
  for (SEFluidCircuitPath* p : c.GetPaths())
  {
    if (p->HasSwitch())
      Track(p->GetName() + "_Switch", time_s, p->GetSwitch() == eGate::Open ? 1 : 0);
    if (p->HasValve())
      Track(p->GetName() + "_Valve", time_s, p->GetValve() == eGate::Closed ? 1 : 0);

    if (p->HasResistance())
    {
      auto unit = p->GetResistance().GetUnit();
      Track(p->GetName() + "_Resistance_" + unit->GetString(), time_s, p->GetResistance().GetValue(*unit));
    }
    if (p->HasCompliance())
    {
      auto unit = p->GetCompliance().GetUnit();
      Track(p->GetName() + "_Compliance_" + unit->GetString(), time_s, p->GetCompliance().GetValue(*unit));
    }
    if (p->HasInertance())
    {
      auto unit = p->GetInertance().GetUnit();
      Track(p->GetName() + "_Inertance_" + unit->GetString(), time_s, p->GetInertance().GetValue(*unit));
    }
    if (p->HasFlow())
    {
      auto unit = p->GetFlow().GetUnit();
      Track(p->GetName() + "_Flow_" + unit->GetString(), time_s, p->GetFlow().GetValue(*unit));
    }
    if (p->HasPressureSource())
    {
      auto unit = p->GetPressureSource().GetUnit();
      Track(p->GetName() + "_PressureSource_" + unit->GetString(), time_s, p->GetPressureSource().GetValue(*unit));
    }
    if (p->HasFlowSource())
    {
      auto unit = p->GetFlowSource().GetUnit();
      Track(p->GetName() + "_FlowSource_" + unit->GetString(), time_s, p->GetFlowSource().GetValue(*unit));
    }   
  }
}
void DataTrack::Track(double time_s, const SEThermalCircuit& c)
{
  for (SEThermalCircuitNode* n : c.GetNodes())
  {
    if (n->HasTemperature())
    {
      auto unit = n->GetTemperature().GetUnit();
      Track(n->GetName() + "_Temperature_" + unit->GetString(), time_s, n->GetTemperature().GetValue(*unit));
    }
    if (n->HasHeat())
    {
      auto unit = n->GetHeat().GetUnit();
      Track(n->GetName() + "_Heat_" + unit->GetString(), time_s, n->GetHeat().GetValue(*unit));
    }
  }
  for (SEThermalCircuitPath* p : c.GetPaths())
  {
    if (p->HasSwitch())
      Track(p->GetName() + "_Switch", time_s, p->GetSwitch() == eGate::Open ? 1 : 0);
    if (p->HasValve())
      Track(p->GetName() + "_Valve", time_s, p->GetValve() == eGate::Closed ? 1 : 0);

    if (p->HasResistance())
    {
      auto unit = p->GetResistance().GetUnit();
      Track(p->GetName() + "_Resistance_" + unit->GetString(), time_s, p->GetResistance().GetValue(*unit));
    }
    if (p->HasCapacitance())
    {
      auto unit = p->GetCapacitance().GetUnit();
      Track(p->GetName() + "_Capacitance_" + unit->GetString(), time_s, p->GetCapacitance().GetValue(*unit));
    }
    if (p->HasInductance())
    {
      auto unit = p->GetInductance().GetUnit();
      Track(p->GetName() + "_Inductance_" + unit->GetString(), time_s, p->GetInductance().GetValue(*unit));
    }
    if (p->HasHeatTransferRate())
    {
      auto unit = p->GetHeatTransferRate().GetUnit();
      Track(p->GetName() + "_HeatTransferRate_" + unit->GetString(), time_s, p->GetHeatTransferRate().GetValue(*unit));
    }
    if (p->HasTemperatureSource())
    {
      auto unit = p->GetTemperatureSource().GetUnit();
      Track(p->GetName() + "_TemperatureSource_" + unit->GetString(), time_s, p->GetTemperatureSource().GetValue(*unit));
    }
    if (p->HasHeatSource())
    {
      auto unit = p->GetHeatSource().GetUnit();
      Track(p->GetName() + "_HeatSource_" + unit->GetString(), time_s, p->GetHeatSource().GetValue(*unit));
    }
  }
}

void DataTrack::Track(double time_s, const SEGasCompartmentGraph& graph, std::vector<SESubstance*>* substances)
{
  for (SEGasCompartment* cmpt : graph.GetCompartments())
  {
    if (cmpt->HasPressure())
    {
      auto unit = cmpt->GetPressure().GetUnit();
      Track(cmpt->GetName() + "_Pressure_" + unit->GetString(), time_s, cmpt->GetPressure().GetValue(*unit));
    }
    if (cmpt->HasVolume())
    {
      auto unit = cmpt->GetVolume().GetUnit();
      Track(cmpt->GetName() + "_Volume_" + unit->GetString(), time_s, cmpt->GetVolume().GetValue(*unit));
    }

    for (SEGasSubstanceQuantity* subQ : cmpt->GetSubstanceQuantities())
    {
      if (substances != nullptr && !substances->empty() && !Contains((*substances), subQ->GetSubstance()))
        continue;

      if (subQ->HasPartialPressure())
      {
        auto unit = subQ->GetPartialPressure().GetUnit();
        Track(cmpt->GetName() + "_" + subQ->GetSubstance().GetName() + "_PartialPressure_" + unit->GetString(), time_s, subQ->GetPartialPressure().GetValue(*unit));
      }
      if (subQ->HasVolume())
      {
        auto unit = subQ->GetVolume().GetUnit();
        Track(cmpt->GetName() + "_" + subQ->GetSubstance().GetName() + "_Volume_" + unit->GetString(), time_s, subQ->GetVolume().GetValue(*unit));
      }
      if (subQ->HasVolumeFraction())
      {
        Track(cmpt->GetName() + "_" + subQ->GetSubstance().GetName() + "_VolumeFraction", time_s, subQ->GetVolumeFraction().GetValue());
      }
    }
  }
  for (SEGasCompartmentLink* link : graph.GetLinks())
  {
    
    if (link->HasFlow())
    {
      auto unit = link->GetFlow().GetUnit();
      Track(link->GetName() + "_Flow_"+unit->GetString(), time_s, link->GetFlow(*unit));
    }
  }
}
void DataTrack::Track(double time_s, const SELiquidCompartmentGraph& graph, std::vector<SESubstance*>* substances)
{
  for (SELiquidCompartment* cmpt : graph.GetCompartments())
  {
    if (cmpt->HasPressure())
    {
      auto unit = cmpt->GetPressure().GetUnit();
      Track(cmpt->GetName() + "_Pressure_" + unit->GetString(), time_s, cmpt->GetPressure().GetValue(*unit));
    }
    if (cmpt->HasVolume())
    {
      auto unit = cmpt->GetVolume().GetUnit();
      Track(cmpt->GetName() + "_Volume_" + unit->GetString(), time_s, cmpt->GetVolume().GetValue(*unit));
    }
  if (cmpt->HasPH())
  {
    Track(cmpt->GetName() + "_pH_", time_s, cmpt->GetPH().GetValue());
  }

    for (SELiquidSubstanceQuantity* subQ : cmpt->GetSubstanceQuantities())
    {
      if (substances!=nullptr && !substances->empty() && !Contains((*substances), subQ->GetSubstance()))
        continue;

      if (subQ->HasSaturation())
      {
        Track(cmpt->GetName() + "_" + subQ->GetSubstance().GetName() + "_Saturation", time_s, subQ->GetSaturation().GetValue());
      }
      if (subQ->HasPartialPressure())
      {
        auto unit = subQ->GetPartialPressure().GetUnit();
        Track(cmpt->GetName() + "_" + subQ->GetSubstance().GetName() + "_PartialPressure_" + unit->GetString(), time_s, subQ->GetPartialPressure().GetValue(*unit));
      }
      if (subQ->HasMass())
      {
        auto unit = subQ->GetMass().GetUnit();
        Track(cmpt->GetName() + "_" + subQ->GetSubstance().GetName() + "_Mass_" + unit->GetString(), time_s, subQ->GetMass().GetValue(*unit));
      }
      if (subQ->HasConcentration())
      {
        auto unit = subQ->GetConcentration().GetUnit();
        Track(cmpt->GetName() + "_" + subQ->GetSubstance().GetName() + "_Concentration_" + unit->GetString(), time_s, subQ->GetConcentration().GetValue(*unit));
      }
      if (subQ->HasMolarity())
      {
        auto unit = subQ->GetMolarity().GetUnit();
        Track(cmpt->GetName() + "_" + subQ->GetSubstance().GetName() + "_Molarity_" + unit->GetString(), time_s, subQ->GetMolarity().GetValue(*unit));
      }
    }
  }
  for (SELiquidCompartmentLink* link : graph.GetLinks())
  {
    if (link->HasFlow())
    {
      auto unit = link->GetFlow().GetUnit();
      Track(link->GetName() + "_Flow_" + unit->GetString(), time_s, link->GetFlow(*unit));
    }
  }
}

double DataTrack::GetTrack(size_t idx, double time)
{
  Element& e = GetElement(idx);
  for (size_t i = 0; i < e.track.size(); i++)
  {
    if (m_Times[i] == time)
      return e.track[i];// All lengths should be the same
  }
  return std::numeric_limits<double>::quiet_NaN();
}

double DataTrack::GetTrack(const std::string& name, double time)
{
  Element& e = GetElement(name);
  for(size_t i=0; i<e.track.size(); i++)
  {
    if(m_Times[i]==time)
      return e.track[i];// All lengths should be the same
  }
  return std::numeric_limits<double>::quiet_NaN();
}

std::vector<std::string> DataTrack::ReadTrackFromFile(const char* fileName)
{
  m_Times.clear();
  m_Elements.clear();
  std::string line;
  std::ifstream infile(fileName);
  // Grab the headings from the first line
  std::getline(infile, line);
  std::istringstream iss(line);
  std::vector<std::string> headings;
  std::size_t pos = 0;
  while (pos < line.size())
    if ((pos = line.find_first_of(',', pos)) != std::string::npos)
      line[pos] = ' ';
  copy(std::istream_iterator<std::string>(iss),
         std::istream_iterator<std::string>(),
         std::back_inserter<std::vector<std::string>>(headings));
  // Pull out each value
  std::vector<double> values;
  while(std::getline(infile, line))
  {
    pos = 0;
    while (pos < line.size())
      if ((pos = line.find_first_of(',', pos)) != std::string::npos)
        line[pos] = ' ';
    std::istringstream issL(line);
    copy(std::istream_iterator<double>(issL),
             std::istream_iterator<double>(),
             std::back_inserter<std::vector<double>>(values));
    double time = values[0];
    for(unsigned int i=1; i<values.size(); i++)
    {
      Track(headings[i],time,values[i]);
    }
    values.clear();
  }
  // Get each value for a time
  infile.close();
  return headings;
}

std::vector<std::string> DataTrack::StreamDataFromFile(const char* fileName)
{
  Reset();
  std::string line;
  m_FileStream.open(fileName);
  // Grab the headings from the first line
  std::getline(m_FileStream, line);
  std::size_t pos = 0;
  while (pos < line.size())
    if ((pos = line.find_first_of(',', pos)) != std::string::npos)
      line[pos] = ' ';
  std::istringstream iss(line);
  std::vector<std::string> headings;
  copy(std::istream_iterator<std::string>(iss),
         std::istream_iterator<std::string>(),
         std::back_inserter<std::vector<std::string>>(headings));

  return headings;
}

double DataTrack::StreamDataFromFile(std::vector<std::string>* headings)
{
  double time=std::numeric_limits<double>::quiet_NaN();
  std::string line;
  // Pull out each value
  std::vector<double> values;
  if(std::getline(m_FileStream, line))
  {
    std::size_t pos = 0;
    while (pos < line.size())
      if ((pos = line.find_first_of(',', pos)) != std::string::npos)
        line[pos] = ' ';
    std::istringstream iss(line);
    copy(std::istream_iterator<double>(iss),
             std::istream_iterator<double>(),
             std::back_inserter<std::vector<double>>(values));
    time = values[0];
    for(unsigned int i=1; i<values.size(); i++)
      Probe(headings->at(i),values[i]);
    values.clear();
  }
  return time;
}

void DataTrack::CreateFile(const char* fileName, std::ofstream& file)
{
  CreateFilePath(fileName);
  size_t idx = 0;
  file.open(fileName, std::ofstream::out | std::ofstream::trunc);
  // Write our headers
  file<<"Time(s)"<<m_Delimiter;
  for (Element& e : m_Elements)
  {
    file << e.name;
    if ((++idx)<(m_Elements.size()))
      file << m_Delimiter;
  }
  file << std::endl;
  file.flush();
}

void DataTrack::WriteTrackToFile(const char* fileName)
{
  size_t idx = 0;
  std::ofstream file;
  file.open(fileName, std::ofstream::out | std::ofstream::trunc);
  // Write our headers  
  file << "Time(s)" << m_Delimiter;
  for (Element& e : m_Elements)
  {
    file << e.name;
    if ((++idx) < (m_Elements.size()))
      file << m_Delimiter;
  }
  file<<std::endl;
  file.flush();
  StreamTrackToFile(file);
}


void DataTrack::StreamTrackToFile(std::ofstream& file)
{  
  double d;
 // Write out map values
  for(size_t i=0; i<m_Times.size(); i++)
  {
    size_t idx = 0;
    file << std::fixed << std::setprecision(m_DefaultPrecision) << m_Times[i] << m_Delimiter;
    for (Element& e : m_Elements)
    {
      d = e.track[i];
      if (d == 0)
      {
        file << std::fixed << std::setprecision(0);
        file << 0;
      }
      else if (d - ((int)d) == 0)
      {
        file << std::fixed << std::setprecision(0);
        file << d;
      }
      else if (std::isnan(d))
      {
        file << "-1.$";
      }
      else
      {
        e.format.SetStream(file);
        file << d;
      }
      if ((++idx) < (m_Elements.size()))
        file << m_Delimiter;
    }
    file<<std::endl;
  }
  file.flush();

  // Clear out the data we had now that it is written
  m_Times.clear();
  for (Element& e : m_Elements)
  {
    e.track.clear();
  }
}

void DataTrack::StreamProbesToFile(double time, std::ofstream& file)
{
  double d;
  size_t idx = 0;
  // Write out probe values in heading order
  file << std::fixed << std::setprecision(2) << time << m_Delimiter;
  for (Element& e : m_Elements)
  {
    d = e.probe;
    if (d == 0)
    {
      file << std::fixed << std::setprecision(0);
      file << 0;
    }
    else if (d-((int)d) == 0)
    {
      file << std::fixed << std::setprecision(0);
      file << d;
    }
    else if (std::isnan(d))
    {
      file << "-1.$";
    }
    else
    {
      e.format.SetStream(file);
      file << d;
    }
    if ((++idx) < (m_Elements.size()))
      file << m_Delimiter;
  }
  file<<std::endl;
  file.flush();
}
