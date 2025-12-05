/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/compartment/fluid/SEGasCompartmentGraph.h"
#include "cdm/compartment/fluid/SELiquidCompartmentGraph.h"
#include "cdm/compartment/thermal/SEThermalCompartment.h"
#include "cdm/compartment/thermal/SEThermalCompartmentLink.h"
#include "cdm/compartment/tissue/SETissueCompartment.h"
// Circuits
#include "cdm/circuit/SECircuitManager.h"
#include "cdm/circuit/fluid/SEFluidCircuit.h"
#include "cdm/circuit/fluid/SEFluidCircuitNode.h"
#include "cdm/circuit/fluid/SEFluidCircuitPath.h"
#include "cdm/circuit/thermal/SEThermalCircuit.h"
#include "cdm/circuit/thermal/SEThermalCircuitNode.h"
#include "cdm/circuit/thermal/SEThermalCircuitPath.h"
#include "cdm/circuit/electrical/SEElectricalCircuit.h"
#include "cdm/circuit/electrical/SEElectricalCircuitNode.h"
#include "cdm/circuit/electrical/SEElectricalCircuitPath.h"
// Substances
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceManager.h"
// Properties
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarAmountPerVolume.h"

SECompartmentManager::SECompartmentManager(SESubstanceManager& subMgr) : Loggable(subMgr.GetLogger()), m_subMgr(subMgr)
{
  Clear();
}
SECompartmentManager::~SECompartmentManager()
{
  Clear();
}

void SECompartmentManager::Setup()
{
  m_O2      = m_subMgr.GetSubstance("Oxygen");
  m_CO2     = m_subMgr.GetSubstance("CarbonDioxide");
  m_CO      = m_subMgr.GetSubstance("CarbonMonoxide");
  m_Hb      = m_subMgr.GetSubstance("Hemoglobin");
  m_HbO2    = m_subMgr.GetSubstance("Oxyhemoglobin");
  m_HbCO2   = m_subMgr.GetSubstance("Carbaminohemoglobin");
  m_HbO2CO2 = m_subMgr.GetSubstance("OxyCarbaminohemoglobin");
  m_HbCO    = m_subMgr.GetSubstance("Carboxyhemoglobin");
}

void SECompartmentManager::Clear()
{
  m_O2      = nullptr;
  m_CO2     = nullptr;
  m_CO      = nullptr;
  m_Hb      = nullptr;
  m_HbO2    = nullptr;
  m_HbCO2   = nullptr;
  m_HbO2CO2 = nullptr;
  m_HbCO    = nullptr;
  DELETE_VECTOR(m_GasCompartments);
  m_GasName2Compartments.clear();
  m_GasLeafCompartments.clear();
  DELETE_VECTOR(m_GasLinks);
  m_GasName2Links.clear();
  m_GasSubstances.clear();
  DELETE_VECTOR(m_GasGraphs);
  m_GasName2Graphs.clear();

  DELETE_VECTOR(m_LiquidCompartments);
  m_LiquidName2Compartments.clear();
  m_LiquidLeafCompartments.clear();
  DELETE_VECTOR(m_LiquidLinks);
  m_LiquidName2Links.clear();
  m_LiquidSubstances.clear(); 
  DELETE_VECTOR(m_LiquidGraphs);
  m_LiquidName2Graphs.clear();
  m_CardiacCycleSampledCompartments.clear();

  DELETE_VECTOR(m_ThermalCompartments);
  m_ThermalName2Compartments.clear();
  m_ThermalLeafCompartments.clear();
  DELETE_VECTOR(m_ThermalLinks);
  m_ThermalLinks.clear();
  m_ThermalName2Links.clear();

  DELETE_VECTOR(m_TissueCompartments);
  m_TissueLeafCompartments.clear();
  m_TissueName2Compartments.clear();
}


bool SECompartmentManager::HasCompartment(eCompartment_Type type, const std::string& name) const
{
  switch (type)
  {
  case eCompartment_Type::Gas:
    return HasGasCompartment(name);
  case eCompartment_Type::Liquid:
    return HasLiquidCompartment(name);
  case eCompartment_Type::Thermal:
    return HasThermalCompartment(name);
  case eCompartment_Type::Tissue:
    return HasTissueCompartment(name);
  default:
    return false;
  }
}
SECompartment* SECompartmentManager::GetCompartment(eCompartment_Type type, const std::string& name)
{
  switch (type)
  {
  case eCompartment_Type::Gas:
    return GetGasCompartment(name);
  case eCompartment_Type::Liquid:
    return GetLiquidCompartment(name);
  case eCompartment_Type::Thermal:
    return GetThermalCompartment(name);
  case eCompartment_Type::Tissue:
    return GetTissueCompartment(name);
  default:
    return nullptr;
  }
}
const SECompartment* SECompartmentManager::GetCompartment(eCompartment_Type type, const std::string& name) const
{
  switch (type)
  {
  case eCompartment_Type::Gas:
    return GetGasCompartment(name);
  case eCompartment_Type::Liquid:
    return GetLiquidCompartment(name);
  case eCompartment_Type::Thermal:
    return GetThermalCompartment(name);
  case eCompartment_Type::Tissue:
    return GetTissueCompartment(name);
  default:
    return nullptr;
  }
}

//////////////////////
// GAS COMPARTMENTS //
//////////////////////

SEGasCompartment& SECompartmentManager::CreateGasCompartment(const std::string& name)
{
  return CreateCompartment<SEGasCompartment>(name, m_GasName2Compartments, &m_GasSubstances);
}
void SECompartmentManager::DeleteGasCompartment(const std::string& name)
{
  SEGasCompartment* cmpt = GetGasCompartment(name);
  if (cmpt != nullptr)
  {
    m_GasName2Compartments.erase(name);
    Remove(m_GasCompartments, cmpt);
    Remove(m_GasLeafCompartments, cmpt); 
    for (SEGasCompartmentGraph* g : m_GasGraphs)
      g->RemoveCompartment(*cmpt);
    SAFE_DELETE(cmpt);
  }
}
bool SECompartmentManager::HasGasCompartment(const std::string& name) const
{
  return HasCompartment<SEGasCompartment>(name, m_GasName2Compartments);
}
SEGasCompartment* SECompartmentManager::GetGasCompartment(const std::string& name)
{
  return GetCompartment<SEGasCompartment>(name, m_GasName2Compartments);
}
const SEGasCompartment* SECompartmentManager::GetGasCompartment(const std::string& name) const
{
  return GetCompartment<SEGasCompartment>(name, m_GasName2Compartments);
}
const std::vector<SEGasCompartment*>& SECompartmentManager::GetGasCompartments()
{
  return m_GasCompartments;
}
const std::vector<SEGasCompartment*>& SECompartmentManager::GetGasLeafCompartments()
{
  return m_GasLeafCompartments;
}
SEGasCompartmentLink& SECompartmentManager::CreateGasLink(SEGasCompartment& src, SEGasCompartment& tgt, const std::string& name)
{
  return CreateLink<SEGasCompartmentLink, SEGasCompartment>(src, tgt, name, m_GasName2Links);
}
void SECompartmentManager::DeleteGasLink(const std::string& name)
{
  SEGasCompartmentLink* link = GetGasLink(name);
  if (link != nullptr)
  {
    m_GasName2Links.erase(name);
    Remove(m_GasLinks, link);
    for (SEGasCompartmentGraph* g : m_GasGraphs)
      g->RemoveLink(*link);
    SAFE_DELETE(link);
  }
}
bool SECompartmentManager::HasGasLink(const std::string& name) const
{
  return HasLink<SEGasCompartmentLink>(name, m_GasName2Links);
}
SEGasCompartmentLink* SECompartmentManager::GetGasLink(const std::string& name)
{
  return GetLink<SEGasCompartmentLink>(name, m_GasName2Links);
}
const SEGasCompartmentLink* SECompartmentManager::GetGasLink(const std::string& name) const
{
  return GetLink<SEGasCompartmentLink>(name, m_GasName2Links);
}
const std::vector<SEGasCompartmentLink*>& SECompartmentManager::GetGasLinks()
{
  return m_GasLinks;
}
SEGasCompartmentGraph& SECompartmentManager::CreateGasGraph(const std::string& name)
{
  SEGasCompartmentGraph* graph = nullptr;

  auto find = m_GasName2Graphs.find(name);
  if (find == end(m_GasName2Graphs))
  {
    graph = new SEGasCompartmentGraph(name, GetLogger());
    m_GasName2Graphs[name] = graph;
    m_GasGraphs.push_back(graph);
  }
  else
  {
    graph = find->second;
    if (graph->GetName() != name)
      throw CommonDataModelException("Compartment Graph already exists for name(" + name + ")");
  }
  return *graph;
}
void SECompartmentManager::DeleteGasGraph(const std::string& name)
{
  SEGasCompartmentGraph* graph = GetGasGraph(name);
  if (graph != nullptr)
  {
    m_GasName2Graphs.erase(name);
    Remove(m_GasGraphs, graph);
    SAFE_DELETE(graph);
  }
}
bool SECompartmentManager::HasGasGraph(const std::string& name) const
{
  return m_GasName2Graphs.find(name) != end(m_GasName2Graphs);
}
SEGasCompartmentGraph* SECompartmentManager::GetGasGraph(const std::string& name)
{
  auto it = m_GasName2Graphs.find(name);
  if (it != m_GasName2Graphs.end())
    return it->second;
  return nullptr;
}
const SEGasCompartmentGraph* SECompartmentManager::GetGasGraph(const std::string& name) const
{
  auto it = m_GasName2Graphs.find(name);
  if (it != m_GasName2Graphs.end())
    return it->second;
  return nullptr;
}
const std::vector<SEGasCompartmentGraph*>& SECompartmentManager::GetGasGraphs()
{
  return m_GasGraphs;
}
void SECompartmentManager::AddGasCompartmentSubstance(SESubstance& s)
{
  if (!Contains(m_GasSubstances, s))
  {
    m_GasSubstances.push_back(&s);
    for (auto itr : m_GasName2Compartments)
    {
      if (AllowGasSubstance(s, *itr.second))
        AddSubstance<SEGasCompartment>(s, *itr.second);
    }
  }
}
const std::vector<SESubstance*>& SECompartmentManager::GetGasCompartmentSubstances() const
{
  return m_GasSubstances;
}

/////////////////////////
// LIQUID COMPARTMENTS //
/////////////////////////

SELiquidCompartment& SECompartmentManager::CreateLiquidCompartment(const std::string& name)
{
  return CreateCompartment<SELiquidCompartment>(name, m_LiquidName2Compartments, &m_LiquidSubstances);
}
void SECompartmentManager::DeleteLiquidCompartment(const std::string& name)
{
  SELiquidCompartment* cmpt = GetLiquidCompartment(name);
  if (cmpt != nullptr)
  {
    m_LiquidName2Compartments.erase(name);
    Remove(m_LiquidCompartments, cmpt);
    Remove(m_LiquidLeafCompartments, cmpt);
    for (SELiquidCompartmentGraph* g : m_LiquidGraphs)
      g->RemoveCompartment(*cmpt);
    SAFE_DELETE(cmpt);
  }
}
bool SECompartmentManager::HasLiquidCompartment(const std::string& name) const
{
  return HasCompartment<SELiquidCompartment>(name, m_LiquidName2Compartments);
}
SELiquidCompartment* SECompartmentManager::GetLiquidCompartment(const std::string& name)
{
  return GetCompartment<SELiquidCompartment>(name, m_LiquidName2Compartments);
}
const SELiquidCompartment* SECompartmentManager::GetLiquidCompartment(const std::string& name) const
{
  return GetCompartment<SELiquidCompartment>(name, m_LiquidName2Compartments);
}
const std::vector<SELiquidCompartment*>& SECompartmentManager::GetLiquidCompartments()
{
  return m_LiquidCompartments;
}
const std::vector<SELiquidCompartment*>& SECompartmentManager::GetLiquidLeafCompartments()
{
  return m_LiquidLeafCompartments;
}
SELiquidCompartmentLink& SECompartmentManager::CreateLiquidLink(SELiquidCompartment& src, SELiquidCompartment& tgt, const std::string& name)
{
  return CreateLink<SELiquidCompartmentLink, SELiquidCompartment>(src, tgt, name, m_LiquidName2Links);
}
void SECompartmentManager::DeleteLiquidLink(const std::string& name)
{
  SELiquidCompartmentLink* link = GetLiquidLink(name);
  if (link != nullptr)
  {
    m_LiquidName2Links.erase(name);
    Remove(m_LiquidLinks, link); 
    for (SELiquidCompartmentGraph* g : m_LiquidGraphs)
      g->RemoveLink(*link);
    SAFE_DELETE(link);
  }
}
bool SECompartmentManager::HasLiquidLink(const std::string& name) const
{
  return HasLink<SELiquidCompartmentLink>(name, m_LiquidName2Links);
}
SELiquidCompartmentLink* SECompartmentManager::GetLiquidLink(const std::string& name)
{
  return GetLink<SELiquidCompartmentLink>(name, m_LiquidName2Links);
}
const SELiquidCompartmentLink* SECompartmentManager::GetLiquidLink(const std::string& name) const
{
  return GetLink<SELiquidCompartmentLink>(name, m_LiquidName2Links);
}
const std::vector<SELiquidCompartmentLink*>& SECompartmentManager::GetLiquidLinks()
{
  return m_LiquidLinks;
}
SELiquidCompartmentGraph& SECompartmentManager::CreateLiquidGraph(const std::string& name)
{
  SELiquidCompartmentGraph* graph = nullptr;

  auto find = m_LiquidName2Graphs.find(name);
  if (find == end(m_LiquidName2Graphs))
  {
    graph = new SELiquidCompartmentGraph(name, GetLogger());
    m_LiquidName2Graphs[name] = graph;
    m_LiquidGraphs.push_back(graph);
  }
  else
  {
    graph = find->second;
    if (graph->GetName() != name)
      throw CommonDataModelException("Compartment Graph already exists for name(" + name + ")");
  }
  return *graph;
}
void SECompartmentManager::DeleteLiquidGraph(const std::string& name)
{
  SELiquidCompartmentGraph* graph = GetLiquidGraph(name);
  if (graph != nullptr)
  {
    m_LiquidName2Graphs.erase(name);
    Remove(m_LiquidGraphs, graph);
    SAFE_DELETE(graph);
  }
}
bool SECompartmentManager::HasLiquidGraph(const std::string& name) const
{
  return m_LiquidName2Graphs.find(name) != end(m_LiquidName2Graphs);
}
SELiquidCompartmentGraph* SECompartmentManager::GetLiquidGraph(const std::string& name)
{
  auto it = m_LiquidName2Graphs.find(name);
  if (it != m_LiquidName2Graphs.end())
    return it->second;
  return nullptr;
}
const SELiquidCompartmentGraph* SECompartmentManager::GetLiquidGraph(const std::string& name) const
{
  auto it = m_LiquidName2Graphs.find(name);
  if (it != m_LiquidName2Graphs.end())
    return it->second;
  return nullptr;
}
const std::vector<SELiquidCompartmentGraph*>& SECompartmentManager::GetLiquidGraphs()
{
  return m_LiquidGraphs;
}
void SECompartmentManager::AddLiquidCompartmentSubstance(SESubstance& s)
{  
  if (!Contains(m_LiquidSubstances, s))
  {
    m_LiquidSubstances.push_back(&s);
    for (auto itr : m_LiquidName2Compartments)
    {
      if(AllowLiquidSubstance(s, *itr.second))
        AddSubstance<SELiquidCompartment>(s, *itr.second);
    }
  }
}
const std::vector<SESubstance*>& SECompartmentManager::GetLiquidCompartmentSubstances() const
{
  return m_LiquidSubstances;
}

void SECompartmentManager::SampleByCardiacCyle(bool CycleStart)
{
  for (SELiquidCompartment* cmpt : m_CardiacCycleSampledCompartments)
    cmpt->Sample(CycleStart);
}
void SECompartmentManager::SampleByCardiacCyle(SELiquidCompartment& cmpt)
{
  if (!Contains(m_CardiacCycleSampledCompartments, cmpt))
    m_CardiacCycleSampledCompartments.push_back(&cmpt);
  cmpt.SampleFlow();
}

//////////////////////////
// THERMAL COMPARTMENTS //
//////////////////////////

SEThermalCompartment& SECompartmentManager::CreateThermalCompartment(const std::string& name)
{
  return CreateCompartment<SEThermalCompartment>(name, m_ThermalName2Compartments, nullptr);
}
void SECompartmentManager::DeleteThermalCompartment(const std::string& name)
{
  SEThermalCompartment* cmpt = GetThermalCompartment(name);
  if (cmpt != nullptr)
  {
    m_ThermalName2Compartments.erase(name);
    Remove(m_ThermalCompartments, cmpt);
    Remove(m_ThermalLeafCompartments, cmpt);
    SAFE_DELETE(cmpt);
  }
}
bool SECompartmentManager::HasThermalCompartment(const std::string& name) const
{
  return HasCompartment<SEThermalCompartment>(name, m_ThermalName2Compartments);
}
SEThermalCompartment* SECompartmentManager::GetThermalCompartment(const std::string& name)
{
  return GetCompartment<SEThermalCompartment>(name, m_ThermalName2Compartments);
}
const SEThermalCompartment* SECompartmentManager::GetThermalCompartment(const std::string& name) const
{
  return GetCompartment<SEThermalCompartment>(name, m_ThermalName2Compartments);
}
const std::vector<SEThermalCompartment*>& SECompartmentManager::GetThermalCompartments()
{
  return m_ThermalCompartments;
}
const std::vector<SEThermalCompartment*>& SECompartmentManager::GetThermalLeafCompartments()
{
  return m_ThermalLeafCompartments;
}
SEThermalCompartmentLink& SECompartmentManager::CreateThermalLink(SEThermalCompartment& src, SEThermalCompartment& tgt, const std::string& name)
{
  return CreateLink<SEThermalCompartmentLink, SEThermalCompartment>(src, tgt, name, m_ThermalName2Links);
}
void SECompartmentManager::DeleteThermalLink(const std::string& name)
{
  SEThermalCompartmentLink* link = GetThermalLink(name);
  if (link != nullptr)
  {
    m_ThermalName2Links.erase(name);
    Remove(m_ThermalLinks, link);
    SAFE_DELETE(link);
  }
}
bool SECompartmentManager::HasThermalLink(const std::string& name) const
{
  return HasLink<SEThermalCompartmentLink>(name, m_ThermalName2Links);
}
SEThermalCompartmentLink* SECompartmentManager::GetThermalLink(const std::string& name)
{
  return GetLink<SEThermalCompartmentLink>(name, m_ThermalName2Links);
}
const SEThermalCompartmentLink* SECompartmentManager::GetThermalLink(const std::string& name) const
{
  return GetLink<SEThermalCompartmentLink>(name, m_ThermalName2Links);
}
const std::vector<SEThermalCompartmentLink*>& SECompartmentManager::GetThermalLinks()
{
  return m_ThermalLinks;
}

/////////////////////////
// TISSUE COMPARTMENTS //
/////////////////////////

SETissueCompartment& SECompartmentManager::CreateTissueCompartment(const std::string& name)
{
  if (HasTissueCompartment(name))
      throw CommonDataModelException("Compartment already exists for name(" + name + ")");

  SELiquidCompartment& extracellular = CreateLiquidCompartment(name + "Extracellular");
  SELiquidCompartment& intracellular = CreateLiquidCompartment(name + "Intracellular");

  SETissueCompartment* tissue = new SETissueCompartment(name, extracellular, intracellular, GetLogger());
  m_TissueName2Compartments[name] = tissue;
  return *tissue;
}
void SECompartmentManager::DeleteTissueCompartment(const std::string& name)
{
  SETissueCompartment* cmpt = GetTissueCompartment(name);
  if (cmpt != nullptr)
  {
    m_TissueName2Compartments.erase(name);
    Remove(m_TissueCompartments, cmpt);
    Remove(m_TissueLeafCompartments, cmpt);
    SAFE_DELETE(cmpt);
  }
}
bool SECompartmentManager::HasTissueCompartment(const std::string& name) const
{
  return HasCompartment<SETissueCompartment>(name, m_TissueName2Compartments);
}
SETissueCompartment* SECompartmentManager::GetTissueCompartment(const std::string& name)
{
  return GetCompartment<SETissueCompartment>(name, m_TissueName2Compartments);
}
const SETissueCompartment* SECompartmentManager::GetTissueCompartment(const std::string& name) const
{
  return GetCompartment<SETissueCompartment>(name, m_TissueName2Compartments);
}
const std::vector<SETissueCompartment*>& SECompartmentManager::GetTissueCompartments()
{
  return m_TissueCompartments;
}
const std::vector<SETissueCompartment*>& SECompartmentManager::GetTissueLeafCompartments()
{
  return m_TissueLeafCompartments;
}



///////////////////////
// General Utilities //
///////////////////////

void SECompartmentManager::StateChange()
{
  // All of our Name 2 Compartment Maps are up to date
  // So track the compartments into our various vectors
  // for easier access and functional support 

  // Also, get a list of all of our leaves
  // The hierarchy should be build and good to go

  m_GasCompartments.clear();
  m_GasLeafCompartments.clear();
  for (auto itr : m_GasName2Compartments)
  {
    m_GasCompartments.push_back(itr.second);
    if (!itr.second->HasChildren())
    {
      m_GasLeafCompartments.push_back(itr.second);
    }
    itr.second->StateChange();
    SetSubstances<SEGasCompartment>(*itr.second, m_GasSubstances);
  }
  m_GasLinks.clear();
  for (auto itr : m_GasName2Links)
  {
    m_GasLinks.push_back(itr.second);
  }
  m_GasGraphs.clear();
  for (auto itr : m_GasName2Graphs)
  {
    m_GasGraphs.push_back(itr.second);
    itr.second->StateChange();
  }

  m_LiquidCompartments.clear();
  m_LiquidLeafCompartments.clear();
  for (auto itr : m_LiquidName2Compartments)
  {
    m_LiquidCompartments.push_back(itr.second);
    if (!itr.second->HasChildren())
    {
      m_LiquidLeafCompartments.push_back(itr.second);
    }
    itr.second->StateChange();
    SetSubstances<SELiquidCompartment>(*itr.second, m_LiquidSubstances);
  }
  m_LiquidLinks.clear();
  for (auto itr : m_LiquidName2Links)
  {
    m_LiquidLinks.push_back(itr.second);
  }
  m_LiquidGraphs.clear();
  for (auto itr : m_LiquidName2Graphs)
  {
    m_LiquidGraphs.push_back(itr.second);
    itr.second->StateChange();
  }

  m_ThermalCompartments.clear();
  m_ThermalLeafCompartments.clear();
  for (auto itr : m_ThermalName2Compartments)
  {
    m_ThermalCompartments.push_back(itr.second);
    if (!itr.second->HasChildren())
    {
      m_ThermalLeafCompartments.push_back(itr.second);
    }
    itr.second->StateChange();
  }
  m_ThermalLinks.clear();
  for (auto itr : m_ThermalName2Links)
  {
    m_ThermalLinks.push_back(itr.second);
  }

  m_TissueCompartments.clear();
  m_TissueLeafCompartments.clear();
  for (auto itr : m_TissueName2Compartments)
  {
    m_TissueCompartments.push_back(itr.second);
    if (!itr.second->HasChildren())
    {
      m_TissueLeafCompartments.push_back(itr.second);
    }
    itr.second->StateChange();
  }

  UpdateLinks();
}

// These methods UpdateLink methods are used to set what links you want a comparmtent to use to calculate its Inflow and Outflow
// By default, it will add all links to all compartments that are either a source or target to the link.
// If you set up multiple graphs for various configurations, you really only want the inflow/outflow for compartments associated with the active graph
// -- Such as a respiratory graph where the mouth is connected to the environment, and a respiratory equipment graph, were the mouth connects to the equipment)
// -- If the equipment is not on/connected, you want the mouth inflow to use the flow associated with the mouthToEnvironment link, and NOT the mouthToEquipment link
// -- and vice versa if the equipment is on/connected
// So you will need to call the UpdateLinks with the 'active' graph, so the manager can set all the links to comparments that it should only take into account when computing flows
// So If you know how you have your compartments and graphs set up, and you want to update just a subset of your compartments associated with a graph
// you can use the UpdateLinks method that takes in a graph, BUT if you have some compartments in multiple active graphs (not sure why, but you know what your doing right?)
// You cannot use the UpdateLinks(Graph) method because you will not be able to get all links to the compartments that overlap multiple 'active' graphs
// If you run into that situation, you will need to combine all compartments and links from all your active graphs into vectors and use the UpdateLinks(vector<Compartment>, vector<Link>) method to get your comparmtnets right
// Since I don't know a use case where you would have a comparment associated with multiple active graphs, I did not write a method to combine graphs, but should be easy for you to do

void SECompartmentManager::UpdateLinks()
{
  UpdateLinks<SELiquidCompartment, SELiquidCompartmentLink>(m_LiquidCompartments, m_LiquidLinks);
  UpdateLinks<SEGasCompartment, SEGasCompartmentLink>(m_GasCompartments, m_GasLinks);
  UpdateLinks<SEThermalCompartment, SEThermalCompartmentLink>(m_ThermalCompartments, m_ThermalLinks);
}

void SECompartmentManager::UpdateLinks(SEGasCompartmentGraph& graph)
{
  UpdateLinks<SEGasCompartment, SEGasCompartmentLink>(graph.GetCompartments(), graph.GetLinks());
}

void SECompartmentManager::UpdateLinks(SELiquidCompartmentGraph& graph)
{
  UpdateLinks<SELiquidCompartment, SELiquidCompartmentLink>(graph.GetCompartments(), graph.GetLinks());
}

template<typename CompartmentType, typename LinkType>
void SECompartmentManager::UpdateLinks(const std::vector<CompartmentType*>& compartments, const std::vector<LinkType*>& links) const
{
  for (CompartmentType* cmpt : compartments)
  {
    cmpt->m_Links.clear();
    cmpt->m_IncomingLinks.clear();
    cmpt->m_OutgoingLinks.clear();
  }
  for (LinkType* link : links)
  {
    CompartmentType& src = link->GetSourceCompartment();
    CompartmentType& tgt = link->GetTargetCompartment();

    src.m_Links.push_back(link);
    src.m_OutgoingLinks.push_back(link);
    tgt.m_Links.push_back(link);
    tgt.m_IncomingLinks.push_back(link);
  }
  for (CompartmentType* pnt : compartments)
  {
    if (pnt->HasChildren())
    {
      for (CompartmentType* child : pnt->GetChildren())
        GetChildLinks<CompartmentType, LinkType>(pnt, child);
    }
  }
}

template<typename CompartmentType, typename LinkType>
void SECompartmentManager::GetChildLinks(CompartmentType* pnt, CompartmentType* child) const
{
  if (child->HasChildren())
  {
    for (CompartmentType* grandchild : child->GetChildren())
      GetChildLinks<CompartmentType, LinkType>(child, grandchild);
  }
  for (LinkType* in : child->m_Links)
    pnt->AddLink(*in);
}

template<typename CompartmentType>
void SECompartmentManager::FindLeaves(CompartmentType* cmpt, std::vector<CompartmentType*>& leaves) const
{
  if (cmpt->HasChildren())
  {
    for (CompartmentType* child : cmpt->GetChildren())
    {
      if (child->HasChildren())
        FindLeaves(child, leaves);
      else
        leaves.push_back(child);
    }
  }
}

template<typename CompartmentType>
CompartmentType& SECompartmentManager::CreateCompartment(const std::string& name, std::map<std::string, CompartmentType*>& name2cmpt, std::vector<SESubstance*>* substances)
{
  CompartmentType* cmpt = nullptr;

  auto find = name2cmpt.find(name);
  if (find == end(name2cmpt))
  {
    cmpt = new CompartmentType(name, GetLogger());
    name2cmpt[name] = cmpt;
    if (substances != nullptr)
      SetSubstances(*cmpt, *substances);
  }
  else
  {
    cmpt = find->second;
    if (cmpt->GetName() != name)
      throw CommonDataModelException("Compartment already exists for name(" + name + ")");
  }
  return *cmpt;
}

template<typename CompartmentType>
bool SECompartmentManager::HasCompartment(const std::string& name, const std::map<std::string, CompartmentType*>& name2cmpt) const
{
  return name2cmpt.find(name) != name2cmpt.end();
}

template<typename CompartmentType>
CompartmentType* SECompartmentManager::GetCompartment(const std::string& name, std::map<std::string, CompartmentType*>& name2cmpt) const
{
  auto it = name2cmpt.find(name);
  if (it != name2cmpt.end())
    return it->second;
  return nullptr;
}

template<typename CompartmentType>
const CompartmentType* SECompartmentManager::GetCompartment(const std::string& name, const std::map<std::string, CompartmentType*>& name2cmpt) const
{
  auto it = name2cmpt.find(name);
  if (it != name2cmpt.end())
    return it->second;
  return nullptr;
}

template<typename LinkType, typename CompartmentType>
LinkType& SECompartmentManager::CreateLink(CompartmentType& src, CompartmentType& tgt, const std::string& name, std::map<std::string, LinkType*>& name2link) const
{
  LinkType* link = nullptr;

  auto find = name2link.find(name);
  if (find == end(name2link))
  {
    link = new LinkType(src, tgt, name);
    name2link[name] = link;
    src.AddLink(*link);
    tgt.AddLink(*link);
  }
  else
  {
    link = find->second;
    if (link->GetName() != name)
      throw CommonDataModelException("Link already exists for name(" + name + ")");
  }
  return *link;
}

template<typename LinkType>
bool SECompartmentManager::HasLink(const std::string& name, const std::map<std::string, LinkType*>& name2link) const
{
  return name2link.find(name) != end(name2link);
}

template<typename LinkType>
LinkType* SECompartmentManager::GetLink(const std::string& name, std::map<std::string, LinkType*>&name2link) const
{
  auto it = name2link.find(name);
  if (it != name2link.end())
    return it->second;
  return nullptr;
}

template<typename LinkType>
const LinkType* SECompartmentManager::GetLink(const std::string& name, const std::map<std::string, LinkType*>& name2link) const
{
  auto it = name2link.find(name);
  if (it != name2link.end())
    return it->second;
  return nullptr;
}

template<typename CompartmentType>
void SECompartmentManager::SetSubstances(CompartmentType& cmpt, std::vector<SESubstance*>& substances) const
{
  // Add any substance quantites to this new compartment, if its a quantity holding type
  SEGasCompartment*    gcmpt = dynamic_cast<SEGasCompartment*>(&cmpt);
  SELiquidCompartment* lcmpt = dynamic_cast<SELiquidCompartment*>(&cmpt);
  if (gcmpt != nullptr || lcmpt != nullptr)
  {
    for (SESubstance* s : substances)
    {
      if (gcmpt != nullptr)
      {
        if (!AllowGasSubstance(*s, *gcmpt))
          continue;
        if (!gcmpt->HasSubstanceQuantity(*s))
          gcmpt->CreateSubstanceQuantity(*s);
      }
      else if (lcmpt != nullptr)
      {        
        if (!AllowLiquidSubstance(*s, *lcmpt))
          continue;
        SELiquidSubstanceQuantity& subQ = lcmpt->CreateSubstanceQuantity(*s);
        if (s == m_O2 || s == m_CO2 || s == m_CO)
          subQ.SetHemoglobins(*m_Hb, *m_HbO2, *m_HbCO2, *m_HbO2CO2, *m_HbCO);
      }
    }
  }
}

template<typename CompartmentType>
void SECompartmentManager::AddSubstance(SESubstance& s, CompartmentType& cmpt) const
{
  if (cmpt.HasSubstanceQuantity(s))
    return;
  auto& subQ = cmpt.CreateSubstanceQuantity(s);
  SELiquidSubstanceQuantity* lsubQ = dynamic_cast<SELiquidSubstanceQuantity*>(&subQ);
  if (lsubQ != nullptr && (&s == m_O2 || &s == m_CO2 || &s == m_CO))
    lsubQ->SetHemoglobins(*m_Hb, *m_HbO2, *m_HbCO2, *m_HbO2CO2, *m_HbCO);
}