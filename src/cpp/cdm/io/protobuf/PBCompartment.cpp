/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
PUSH_PROTO_WARNINGS
#include "pulse/cdm/bind/Compartment.pb.h"
POP_PROTO_WARNINGS
#include "cdm/io/protobuf/PBCompartment.h"
#include "cdm/io/protobuf/PBSubstanceQuantity.h"
#include "cdm/io/protobuf/PBProperties.h"
#include "cdm/io/protobuf/PBUtils.h"
#include "cdm/compartment/SECompartment.h"
#include "cdm/compartment/SECompartmentGraph.h"
#include "cdm/compartment/SECompartmentLink.h"
#include "cdm/compartment/SECompartmentNodes.h"
#include "cdm/compartment/SECompartmentTransportGraph.h"
#include "cdm/compartment/fluid/SEGasCompartment.h"
#include "cdm/compartment/fluid/SEGasCompartmentGraph.h"
#include "cdm/compartment/fluid/SEGasCompartmentLink.h"
#include "cdm/compartment/fluid/SELiquidCompartment.h"
#include "cdm/compartment/fluid/SELiquidCompartmentGraph.h"
#include "cdm/compartment/thermal/SEThermalCompartment.h"
#include "cdm/compartment/thermal/SEThermalCompartmentLink.h"
#include "cdm/compartment/tissue/SETissueCompartment.h"
#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/circuit/SECircuitManager.h"
#include "cdm/circuit/thermal/SEThermalCircuitPath.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceManager.h"

void PBCompartment::Serialize(const CDM_BIND::CompartmentData& /*src*/, SECompartment& /*dst*/)
{
  // Name is set in ctor
}
void PBCompartment::Serialize(const SECompartment& src, CDM_BIND::CompartmentData& dst)
{
  dst.set_name(src.m_Name);
}

void PBCompartment::Serialize(const CDM_BIND::CompartmentLinkData& src, SECompartmentLink& dst)
{
  if (!src.name().empty())
    dst.m_Name = src.name();
}
void PBCompartment::Serialize(const SECompartmentLink& src, CDM_BIND::CompartmentLinkData& dst)
{
  dst.set_name(src.m_Name);
}

bool PBCompartment::LoadCompartmentManagerFile(SECompartmentManager& mgr, const std::string& filename, SECircuitManager* circuits)
{
  CDM_BIND::CompartmentManagerData src;
  std::ifstream file_stream(filename, std::ios::in);
  std::string fmsg((std::istreambuf_iterator<char>(file_stream)), std::istreambuf_iterator<char>());
  file_stream.close();
  if (!PBUtils::SerializeFromString(fmsg, src, eSerializationFormat::JSON, mgr.GetLogger()))
    return false;
  PBCompartment::Load(src, mgr, circuits);
  return true;

  // If its a binary string in the file...
  //std::ifstream binary_istream(filename, std::ios::in | std::ios::binary);
  //src.ParseFromIstream(&binary_istream);
  // binary_istream.close();
}

void PBCompartment::SaveCompartmentManagerFile(const SECompartmentManager& mgr, const std::string& filename)
{
  std::string content;
  CDM_BIND::CompartmentManagerData* src = PBCompartment::Unload(mgr);
  PBUtils::SerializeToString(*src, content, eSerializationFormat::JSON, mgr.GetLogger());
  std::ofstream ascii_ostream(filename, std::ios::out | std::ios::trunc);
  ascii_ostream << content;
  ascii_ostream.flush();
  ascii_ostream.close();
  delete src;
}

void PBCompartment::Load(const CDM_BIND::CompartmentManagerData& src, SECompartmentManager& dst, SECircuitManager* circuits)
{
  dst.Clear();
  dst.Setup();
  PBCompartment::Serialize(src, dst, circuits);
  dst.StateChange();
}
void PBCompartment::Serialize(const CDM_BIND::CompartmentManagerData& src, SECompartmentManager& dst, SECircuitManager* circuits)
{
  for (int i = 0; i < src.gascompartment_size(); i++)
  {
    auto& cData = src.gascompartment(i);
    PBCompartment::Load(cData, dst.CreateGasCompartment(cData.fluidcompartment().compartment().name()), dst.m_subMgr, circuits);
  }
  for (int i = 0; i < src.gaslink_size(); i++)
  {
    auto& cData = src.gaslink(i);
    auto* srcCmpt = dst.GetGasCompartment(cData.fluidlink().link().sourcecompartment());
    if (srcCmpt == nullptr)
    {
      dst.Error("Unable to find source compartment " + cData.fluidlink().link().sourcecompartment() + " for link " + cData.fluidlink().link().name());
      continue;
    }
    auto* tgtCmpt = dst.GetGasCompartment(cData.fluidlink().link().targetcompartment());
    if (tgtCmpt == nullptr)
    {
      dst.Error("Unable to find target compartment " + cData.fluidlink().link().targetcompartment() + " for link " + cData.fluidlink().link().name());
      continue;
    }
    PBCompartment::Load(cData, dst.CreateGasLink(*srcCmpt, *tgtCmpt, cData.fluidlink().link().name()), circuits);
  }
  for (int i = 0; i < src.gascompartment_size(); i++)
  {
    auto& cData = src.gascompartment(i);
    auto* cmpt = dst.GetGasCompartment(cData.fluidcompartment().compartment().name());
    for (int j = 0; j < cData.fluidcompartment().compartment().child_size(); j++)
    {
      std::string name = cData.fluidcompartment().compartment().child(j);
      auto* child = dst.GetGasCompartment(name);
      if (child == nullptr)
      {
        dst.Error("Could not find child " + name + " for node " + cmpt->GetName());
        continue;
      }
      cmpt->AddChild(*child);
    }
  }
  for (int i = 0; i < src.gassubstance_size(); i++)
  {
    std::string subName = src.gassubstance(i);
    SESubstance* sub = dst.m_subMgr.GetSubstance(subName);
    if (sub == nullptr)
    {
      dst.Error("Could not find substance " + subName);
      continue;
    }
    dst.AddGasCompartmentSubstance(*sub);
  }
  for (int i = 0; i < src.gasgraph_size(); i++)
  {
    auto& cData = src.gasgraph(i);
    PBCompartment::Load(cData, dst.CreateGasGraph(cData.fluidgraph().graph().name()), dst);
  }

  for (int i = 0; i < src.liquidcompartment_size(); i++)
  {
    auto& cData = src.liquidcompartment(i);
    PBCompartment::Load(cData, dst.CreateLiquidCompartment(cData.fluidcompartment().compartment().name()), dst.m_subMgr, circuits);
  }
  for (int i = 0; i < src.liquidlink_size(); i++)
  {
    auto& cData = src.liquidlink(i);
    auto* srcCmpt = dst.GetLiquidCompartment(cData.fluidlink().link().sourcecompartment());
    if (srcCmpt == nullptr)
    {
      dst.Error("Unable to find source compartment " + cData.fluidlink().link().sourcecompartment() + " for link " + cData.fluidlink().link().name());
      continue;
    }
    auto* tgtCmpt = dst.GetLiquidCompartment(cData.fluidlink().link().targetcompartment());
    if (tgtCmpt == nullptr)
    {
      dst.Error("Unable to find target compartment " + cData.fluidlink().link().targetcompartment() + " for link " + cData.fluidlink().link().name());
      continue;
    }
    PBCompartment::Load(cData, dst.CreateLiquidLink(*srcCmpt, *tgtCmpt, cData.fluidlink().link().name()), circuits);
  }
  for (int i = 0; i < src.liquidcompartment_size(); i++)
  {
    auto& cData = src.liquidcompartment(i);
    auto* cmpt = dst.GetLiquidCompartment(cData.fluidcompartment().compartment().name());
    for (int j = 0; j < cData.fluidcompartment().compartment().child_size(); j++)
    {
      std::string name = cData.fluidcompartment().compartment().child(j);
      auto* child = dst.GetLiquidCompartment(name);
      if (child == nullptr)
      {
        dst.Error("Could not find child " + name + " for node " + cmpt->GetName());
        continue;
      }
      cmpt->AddChild(*child);
    }
  }
  for (int i = 0; i < src.liquidsubstance_size(); i++)
  {
    std::string subName = src.liquidsubstance(i);
    SESubstance* sub = dst.m_subMgr.GetSubstance(subName);
    if (sub == nullptr)
    {
      dst.Error("Could not find substance " + subName);
      continue;
    }
    dst.AddLiquidCompartmentSubstance(*sub);
  }
  for (int i = 0; i < src.liquidgraph_size(); i++)
  {
    auto& cData = src.liquidgraph(i);
    PBCompartment::Load(cData, dst.CreateLiquidGraph(cData.fluidgraph().graph().name()), dst);
  }

  for (int i = 0; i < src.thermalcompartment_size(); i++)
  {
    auto& cData = src.thermalcompartment(i);
    PBCompartment::Load(cData, dst.CreateThermalCompartment(cData.compartment().name()), circuits);
  }
  for (int i = 0; i < src.thermallink_size(); i++)
  {
    auto& cData = src.thermallink(i);
    auto* srcCmpt = dst.GetThermalCompartment(cData.link().sourcecompartment());
    if (srcCmpt == nullptr)
    {
      dst.Error("Unable to find source compartment " + cData.link().sourcecompartment() + " for link " + cData.link().name());
      continue;
    }
    auto* tgtCmpt = dst.GetThermalCompartment(cData.link().targetcompartment());
    if (tgtCmpt == nullptr)
    {
      dst.Error("Unable to find target compartment " + cData.link().targetcompartment() + " for link " + cData.link().name());
      continue;
    }
    PBCompartment::Load(cData, dst.CreateThermalLink(*srcCmpt, *tgtCmpt, cData.link().name()), circuits);
  }
  for (int i = 0; i < src.thermalcompartment_size(); i++)
  {
    auto& cData = src.thermalcompartment(i);
    auto* cmpt = dst.GetThermalCompartment(cData.compartment().name());
    for (int j = 0; j < cData.compartment().child_size(); j++)
    {
      std::string name = cData.compartment().child(j);
      auto* child = dst.GetThermalCompartment(name);
      if (child == nullptr)
      {
        dst.Error("Could not find child " + name + " for node " + cmpt->GetName());
        continue;
      }
      cmpt->AddChild(*child);
    }
  }

  for (int i = 0; i < src.tissuecompartment_size(); i++)
  {
    auto& cData = src.tissuecompartment(i);
    PBCompartment::Load(cData, dst.CreateTissueCompartment(cData.compartment().name()));
  }
  dst.StateChange();
}

CDM_BIND::CompartmentManagerData* PBCompartment::Unload(const SECompartmentManager& src)
{
  CDM_BIND::CompartmentManagerData* dst = new CDM_BIND::CompartmentManagerData();
  PBCompartment::Serialize(src, *dst);
  return dst;
}
void PBCompartment::Serialize(const SECompartmentManager& src, CDM_BIND::CompartmentManagerData& dst)
{
  for (SELiquidCompartment* cmpt : src.m_LiquidCompartments)
    dst.mutable_liquidcompartment()->AddAllocated(PBCompartment::Unload(*cmpt));
  for (SELiquidCompartmentLink* link : src.m_LiquidLinks)
    dst.mutable_liquidlink()->AddAllocated(PBCompartment::Unload(*link));
  for (SELiquidCompartmentGraph* graph : src.m_LiquidGraphs)
    dst.mutable_liquidgraph()->AddAllocated(PBCompartment::Unload(*graph));
  for (const SESubstance* sub : src.m_LiquidSubstances)
    dst.mutable_liquidsubstance()->Add(sub->GetName());

  for (SEGasCompartment* cmpt : src.m_GasCompartments)
    dst.mutable_gascompartment()->AddAllocated(PBCompartment::Unload(*cmpt));
  for (SEGasCompartmentLink* link : src.m_GasLinks)
    dst.mutable_gaslink()->AddAllocated(PBCompartment::Unload(*link));
  for (SEGasCompartmentGraph* graph : src.m_GasGraphs)
    dst.mutable_gasgraph()->AddAllocated(PBCompartment::Unload(*graph));
  for (const SESubstance* sub : src.m_GasSubstances)
    dst.mutable_gassubstance()->Add(sub->GetName());

  for (SEThermalCompartment* cmpt : src.m_ThermalCompartments)
    dst.mutable_thermalcompartment()->AddAllocated(PBCompartment::Unload(*cmpt));
  for (SEThermalCompartmentLink* link : src.m_ThermalLinks)
    dst.mutable_thermallink()->AddAllocated(PBCompartment::Unload(*link));

  for (SETissueCompartment* cmpt : src.m_TissueCompartments)
    dst.mutable_tissuecompartment()->AddAllocated(PBCompartment::Unload(*cmpt));
}

///////////////////////
// Fluid Compartment //
///////////////////////

template<FLUID_COMPARTMENT_TEMPLATE>
void PBCompartment::Serialize(const CDM_BIND::FluidCompartmentData& src, SEFluidCompartment<FLUID_COMPARTMENT_TYPES>& dst, SECircuitManager* circuits)
{
  PBCompartment::Serialize(src.compartment(), dst);
  if (src.has_averageinflow())
  {
    dst.GetAverageInflow();
    PBProperty::Load(src.averageinflow(), *dst.m_AverageInflow);
    PBProperty::Load(src.inflowrunningaverage(), *dst.m_AverageInflow_mL_Per_s);
  }
  if (src.has_averageoutflow())
  {
    dst.GetAverageOutflow();
    PBProperty::Load(src.averageoutflow(), *dst.m_AverageOutflow);
    PBProperty::Load(src.outflowrunningaverage(), *dst.m_AverageOutflow_mL_Per_s);
  }

  // This compartment has children
  // We will not load any data as those are calculated on demand based on children
  if (src.compartment().child_size() > 0)
    return;
  // Now let's see if we are mapped to a circuit node
  // If we are, the circuit node(s) hold the data, not us
  else if (src.compartment().node_size() > 0)
  {
    if (circuits == nullptr)
    {
      dst.Error("Compartment is mapped to circuit nodes, but no circuit manager was provided, cannot load");
      return;
    }
    for (int i = 0; i < src.compartment().node_size(); i++)
    {
      const std::string name = src.compartment().node(i);
      SEFluidCircuitNode* node = circuits->GetFluidNode(name);
      if (node == nullptr)
        dst.Error("Compartment is mapped to circuit node, " + name + ", but provided circuit manager did not have that node");
      else
        dst.MapNode(*node);
    }
  }
  else
  {// Only load these if you don't have children or nodes
    if (src.has_pressure())
      PBProperty::Load(src.pressure(), dst.GetPressure());
    if (src.has_volume())
      PBProperty::Load(src.volume(), dst.GetVolume());
  }
}
template<FLUID_COMPARTMENT_TEMPLATE>
void PBCompartment::Serialize(const SEFluidCompartment<FLUID_COMPARTMENT_TYPES>& src, CDM_BIND::FluidCompartmentData& dst)
{
  PBCompartment::Serialize(src, *dst.mutable_compartment());
  for (SEFluidCompartment<FLUID_COMPARTMENT_TYPES>* child : src.m_FluidChildren)
    dst.mutable_compartment()->add_child(child->GetName());
  for (SEFluidCircuitNode* nodes : src.m_Nodes.GetNodes())
    dst.mutable_compartment()->add_node(nodes->GetName());
  // Even if you have children or nodes, I am unloading everything, this makes the json actually usefull...
  if (src.HasInflow())
    dst.set_allocated_inflow(PBProperty::Unload(src.GetInflow()));
  if (src.HasAverageInflow())
  {
    dst.set_allocated_averageinflow(PBProperty::Unload(src.GetAverageInflow()));
    dst.set_allocated_inflowrunningaverage(PBProperty::Unload(*src.m_AverageInflow_mL_Per_s));
  }
  if (src.HasOutflow())
    dst.set_allocated_outflow(PBProperty::Unload(src.GetOutflow()));
  if (src.HasAverageOutflow())
  {
    dst.set_allocated_averageoutflow(PBProperty::Unload(src.GetAverageOutflow()));
    dst.set_allocated_outflowrunningaverage(PBProperty::Unload(*src.m_AverageOutflow_mL_Per_s));
  }

  // Yeah, I know
  // But, these will only modify member variables if they are being used as temporary variables
  SEFluidCompartment<FLUID_COMPARTMENT_TYPES>& mutable_src = const_cast<SEFluidCompartment<FLUID_COMPARTMENT_TYPES>&>(src);
  if (src.HasPressure())
    dst.set_allocated_pressure(PBProperty::Unload(mutable_src.GetPressure()));
  if (src.HasVolume())
    dst.set_allocated_volume(PBProperty::Unload(mutable_src.GetVolume()));
}


template<FLUID_COMPARTMENT_LINK_TEMPLATE>
void PBCompartment::Serialize(const CDM_BIND::FluidCompartmentLinkData& src, SEFluidCompartmentLink<FLUID_COMPARTMENT_LINK_TYPES>& dst, SECircuitManager* circuits)
{
  PBCompartment::Serialize(src.link(), dst);

  if (!src.link().path().empty())
  {
    if (circuits == nullptr)
    {
      dst.Error("Link is mapped to circuit path, " + src.link().path() + ", but no circuit manager was provided, cannot load");
      return;
    }
    SEFluidCircuitPath* path = circuits->GetFluidPath(src.link().path());
    if (path == nullptr)
    {
      dst.Error("Link is mapped to circuit path, " + src.link().path() + ", but provided circuit manager did not have that path");
      return;
    }
    dst.MapPath(*path);
  }
  else
  {
    if (src.has_flow())
      PBProperty::Load(src.flow(), dst.GetFlow());
  }
}
template<FLUID_COMPARTMENT_LINK_TEMPLATE>
void PBCompartment::Serialize(const SEFluidCompartmentLink<FLUID_COMPARTMENT_LINK_TYPES>& src, CDM_BIND::FluidCompartmentLinkData& dst)
{
  PBCompartment::Serialize(src, *dst.mutable_link());
  dst.mutable_link()->set_sourcecompartment(src.m_SourceCmpt.GetName());
  dst.mutable_link()->set_targetcompartment(src.m_TargetCmpt.GetName());
  if (src.m_Path != nullptr)
    dst.mutable_link()->set_path(src.m_Path->GetName());
  // Yeah, I know
  // But, these will only modify member variables if they are being used as temporary variables
  // Even if you have a path, I am unloading everything, this makes the json actually usefull...
  SEFluidCompartmentLink<FLUID_COMPARTMENT_LINK_TYPES>& mutable_src = const_cast<SEFluidCompartmentLink<FLUID_COMPARTMENT_LINK_TYPES>&>(src);
  if (src.HasFlow())
    dst.set_allocated_flow(PBProperty::Unload(mutable_src.GetFlow()));
}


void PBCompartment::Load(const CDM_BIND::GasCompartmentData& src, SEGasCompartment& dst, SESubstanceManager& subMgr, SECircuitManager* circuits)
{
  dst.Clear();
  PBCompartment::Serialize(src, dst, subMgr, circuits);
}
void PBCompartment::Serialize(const CDM_BIND::GasCompartmentData& src, SEGasCompartment& dst, SESubstanceManager& subMgr, SECircuitManager* circuits)
{
  PBCompartment::Serialize(src.fluidcompartment(), dst, circuits);

  if (src.has_diffusionsurfacearea())
    PBProperty::Load(src.diffusionsurfacearea(), dst.GetDiffusionSurfaceArea());
  if (src.has_ventilation())
    PBProperty::Load(src.ventilation(), dst.GetVentilation());
  if (src.has_ventilationperfusionratio())
    PBProperty::Load(src.ventilationperfusionratio(), dst.GetVentilationPerfusionRatio());

  if (src.substancequantity_size() > 0)
  {
    for (int i = 0; i < src.substancequantity_size(); i++)
    {
      const CDM_BIND::GasSubstanceQuantityData& d = src.substancequantity(i);
      SESubstance* sub = subMgr.GetSubstance(d.substancequantity().substance());
      if (sub == nullptr)
      {
        dst.Error("Could not find a substance for " + d.substancequantity().substance());
        continue;
      }
      PBSubstanceQuantity::Load(d, dst.CreateSubstanceQuantity(*sub, false));
    }
  }
}

CDM_BIND::GasCompartmentData* PBCompartment::Unload(const SEGasCompartment& src)
{
  CDM_BIND::GasCompartmentData* dst = new CDM_BIND::GasCompartmentData();
  PBCompartment::Serialize(src, *dst);
  return dst;
}
void PBCompartment::Serialize(const SEGasCompartment& src, CDM_BIND::GasCompartmentData& dst)
{
  PBCompartment::Serialize(src, *dst.mutable_fluidcompartment());

  if (src.HasDiffusionSurfaceArea())
    dst.set_allocated_diffusionsurfacearea(PBProperty::Unload(*src.m_DiffusionSurfaceArea));
  if (src.HasVentilation())
    dst.set_allocated_ventilation(PBProperty::Unload(*src.m_Ventilation));
  if (src.HasVentilationPerfusionRatio())
    dst.set_allocated_ventilationperfusionratio(PBProperty::Unload(*src.m_VentilationPerfusionRatio));

  for (SEGasSubstanceQuantity* subQ : src.m_SubstanceQuantities)
    dst.mutable_substancequantity()->AddAllocated(PBSubstanceQuantity::Unload(*subQ));
}


void PBCompartment::Load(const CDM_BIND::GasCompartmentGraphData& src, SEGasCompartmentGraph& dst, SECompartmentManager& cmptMgr)
{
  dst.Clear();
  PBCompartment::Serialize(src, dst, cmptMgr);
}
void PBCompartment::Serialize(const CDM_BIND::GasCompartmentGraphData& src, SEGasCompartmentGraph& dst, SECompartmentManager& cmptMgr)
{
  dst.m_Name = src.fluidgraph().graph().name();
  for (int i = 0; i < src.fluidgraph().graph().compartment_size(); i++)
  {
    std::string name = src.fluidgraph().graph().compartment(i);
    SEGasCompartment* cmpt = cmptMgr.GetGasCompartment(name);
    if (cmpt == nullptr)
    {
      dst.Error("Could not find compartment " + name + " for graph " + dst.m_Name);
      continue;
    }
    dst.AddCompartment(*cmpt);
  }
  for (int i = 0; i < src.fluidgraph().graph().link_size(); i++)
  {
    std::string name = src.fluidgraph().graph().link(i);
    SEGasCompartmentLink* link = cmptMgr.GetGasLink(name);
    if (link == nullptr)
    {
      dst.Error("Could not find link " + name + " for graph " + dst.m_Name);
      continue;
    }
    dst.AddLink(*link);
  }
}

CDM_BIND::GasCompartmentGraphData* PBCompartment::Unload(const SEGasCompartmentGraph& src)
{
  CDM_BIND::GasCompartmentGraphData* dst = new CDM_BIND::GasCompartmentGraphData();
  PBCompartment::Serialize(src, *dst);
  return dst;
}
void PBCompartment::Serialize(const SEGasCompartmentGraph& src, CDM_BIND::GasCompartmentGraphData& dst)
{
  dst.mutable_fluidgraph()->mutable_graph()->set_name(src.m_Name);
  for (SEGasCompartment* cmpt : src.m_Compartments)
    dst.mutable_fluidgraph()->mutable_graph()->add_compartment(cmpt->GetName());
  for (SEGasCompartmentLink* link : src.m_CompartmentLinks)
    dst.mutable_fluidgraph()->mutable_graph()->add_link(link->GetName());
}

void PBCompartment::Load(const CDM_BIND::GasCompartmentLinkData& src, SEGasCompartmentLink& dst, SECircuitManager* circuits)
{
  dst.Clear();
  PBCompartment::Serialize(src, dst, circuits);
}
void PBCompartment::Serialize(const CDM_BIND::GasCompartmentLinkData& src, SEGasCompartmentLink& dst, SECircuitManager* circuits)
{
  PBCompartment::Serialize(src.fluidlink(), dst, circuits);
}

CDM_BIND::GasCompartmentLinkData* PBCompartment::Unload(const SEGasCompartmentLink& src)
{
  CDM_BIND::GasCompartmentLinkData* dst = new CDM_BIND::GasCompartmentLinkData();
  PBCompartment::Serialize(src, *dst);
  return dst;
}
void PBCompartment::Serialize(const SEGasCompartmentLink& src, CDM_BIND::GasCompartmentLinkData& dst)
{
  PBCompartment::Serialize(src, *dst.mutable_fluidlink());
}


void PBCompartment::Load(const CDM_BIND::LiquidCompartmentData& src, SELiquidCompartment& dst, SESubstanceManager& subMgr, SECircuitManager* circuits)
{
  dst.Clear();
  PBCompartment::Serialize(src, dst, subMgr, circuits);
}
void PBCompartment::Serialize(const CDM_BIND::LiquidCompartmentData& src, SELiquidCompartment& dst, SESubstanceManager& subMgr, SECircuitManager* circuits)
{
  PBCompartment::Serialize(src.fluidcompartment(), dst, circuits);

  if (src.substancequantity_size() > 0)
  {
    for (int i = 0; i < src.substancequantity_size(); i++)
    {
      const CDM_BIND::LiquidSubstanceQuantityData& d = src.substancequantity(i);
      SESubstance* sub = subMgr.GetSubstance(d.substancequantity().substance());
      if (sub == nullptr)
      {
        dst.Error("Could not find a substance for " + d.substancequantity().substance());
        continue;
      }
      PBSubstanceQuantity::Serialize(d, dst.CreateSubstanceQuantity(*sub, false));
    }
  }
  if (src.has_perfusion())
    PBProperty::Load(src.perfusion(), dst.GetPerfusion());
  if (src.has_ph())
    PBProperty::Load(src.ph(), dst.GetPH());
  if (src.has_watervolumefraction())
    PBProperty::Load(src.watervolumefraction(), dst.GetWaterVolumeFraction());
}

CDM_BIND::LiquidCompartmentData* PBCompartment::Unload(const SELiquidCompartment& src)
{
  CDM_BIND::LiquidCompartmentData* dst = new CDM_BIND::LiquidCompartmentData();
  PBCompartment::Serialize(src, *dst);
  return dst;
}
void PBCompartment::Serialize(const SELiquidCompartment& src, CDM_BIND::LiquidCompartmentData& dst)
{
  PBCompartment::Serialize(src, *dst.mutable_fluidcompartment());
  for (SELiquidSubstanceQuantity* subQ : src.m_SubstanceQuantities)
    dst.mutable_substancequantity()->AddAllocated(PBSubstanceQuantity::Unload(*subQ));

  if (src.HasPerfusion())
    dst.set_allocated_perfusion(PBProperty::Unload(*src.m_Perfusion));
  if (src.HasPH())
    dst.set_allocated_ph(PBProperty::Unload(*src.m_pH));
  if (src.HasWaterVolumeFraction())
    dst.set_allocated_watervolumefraction(PBProperty::Unload(*src.m_WaterVolumeFraction));
}


void PBCompartment::Load(const CDM_BIND::LiquidCompartmentGraphData& src, SELiquidCompartmentGraph& dst, SECompartmentManager& cmptMgr)
{
  dst.Clear();
  PBCompartment::Serialize(src, dst, cmptMgr);
}
void PBCompartment::Serialize(const CDM_BIND::LiquidCompartmentGraphData& src, SELiquidCompartmentGraph& dst, SECompartmentManager& cmptMgr)
{
  dst.m_Name = src.fluidgraph().graph().name();
  for (int i = 0; i < src.fluidgraph().graph().compartment_size(); i++)
  {
    std::string name = src.fluidgraph().graph().compartment(i);
    SELiquidCompartment* cmpt = cmptMgr.GetLiquidCompartment(name);
    if (cmpt == nullptr)
    {
      dst.Error("Could not find compartment " + name + " for graph " + dst.m_Name);
      continue;
    }
    dst.AddCompartment(*cmpt);
  }
  for (int i = 0; i < src.fluidgraph().graph().link_size(); i++)
  {
    std::string name = src.fluidgraph().graph().link(i);
    SELiquidCompartmentLink* link = cmptMgr.GetLiquidLink(name);
    if (link == nullptr)
    {
      dst.Error("Could not find link " + name + " for graph " + dst.m_Name);
      continue;
    }
    dst.AddLink(*link);
  }
}

CDM_BIND::LiquidCompartmentGraphData* PBCompartment::Unload(const SELiquidCompartmentGraph& src)
{
  CDM_BIND::LiquidCompartmentGraphData* dst = new CDM_BIND::LiquidCompartmentGraphData();
  PBCompartment::Serialize(src, *dst);
  return dst;
}
void PBCompartment::Serialize(const SELiquidCompartmentGraph& src, CDM_BIND::LiquidCompartmentGraphData& dst)
{
  dst.mutable_fluidgraph()->mutable_graph()->set_name(src.m_Name);
  for (SELiquidCompartment* cmpt : src.m_Compartments)
    dst.mutable_fluidgraph()->mutable_graph()->add_compartment(cmpt->GetName());
  for (SELiquidCompartmentLink* link : src.m_CompartmentLinks)
    dst.mutable_fluidgraph()->mutable_graph()->add_link(link->GetName());
}

void PBCompartment::Load(const CDM_BIND::LiquidCompartmentLinkData& src, SELiquidCompartmentLink& dst, SECircuitManager* circuits)
{
  dst.Clear();
  PBCompartment::Serialize(src, dst, circuits);
}
void PBCompartment::Serialize(const CDM_BIND::LiquidCompartmentLinkData& src, SELiquidCompartmentLink& dst, SECircuitManager* circuits)
{
  PBCompartment::Serialize(src.fluidlink(), dst, circuits);
}

CDM_BIND::LiquidCompartmentLinkData* PBCompartment::Unload(const SELiquidCompartmentLink& src)
{
  CDM_BIND::LiquidCompartmentLinkData* dst = new CDM_BIND::LiquidCompartmentLinkData();
  PBCompartment::Serialize(src, *dst);
  return dst;
}
void PBCompartment::Serialize(const SELiquidCompartmentLink& src, CDM_BIND::LiquidCompartmentLinkData& dst)
{
  PBCompartment::Serialize(src, *dst.mutable_fluidlink());
}

//////////////////////////
// Thermal Compartments //
//////////////////////////


void PBCompartment::Load(const CDM_BIND::ThermalCompartmentData& src, SEThermalCompartment& dst, SECircuitManager* circuits)
{
  dst.Clear();
  PBCompartment::Serialize(src, dst, circuits);
}
void PBCompartment::Serialize(const CDM_BIND::ThermalCompartmentData& src, SEThermalCompartment& dst, SECircuitManager* circuits)
{
  PBCompartment::Serialize(src.compartment(), dst);
  // This compartment has children
  // We will not load any data as those are calculated on demand based on children
  if (src.compartment().child_size() > 0)
    return;
  // Now let's see if we are mapped to a circuit node
  // If we are, the circuit node(s) hold the data, not us
  else if (src.compartment().node_size() > 0)
  {
    if (circuits == nullptr)
    {
      dst.Error("Compartment is mapped to circuit nodes, but no circuit manager was provided, cannot load");
      return;
    }
    for (int i = 0; i < src.compartment().node_size(); i++)
    {
      const std::string name = src.compartment().node(i);
      SEThermalCircuitNode* node = circuits->GetThermalNode(name);
      if (node == nullptr)
        dst.Error("Compartment is mapped to circuit node, " + name + ", but provided circuit manager did not have that node");
      else
        dst.MapNode(*node);
    }
  }
  else
  {// Only load these if you don't have children or nodes
    if (src.has_heat())
      PBProperty::Load(src.heat(), dst.GetHeat());
    if (src.has_temperature())
      PBProperty::Load(src.temperature(), dst.GetTemperature());
  }
}

CDM_BIND::ThermalCompartmentData* PBCompartment::Unload(const SEThermalCompartment& src)
{
  CDM_BIND::ThermalCompartmentData* dst = new CDM_BIND::ThermalCompartmentData();
  PBCompartment::Serialize(src, *dst);
  return dst;
}
void PBCompartment::Serialize(const SEThermalCompartment& src, CDM_BIND::ThermalCompartmentData& dst)
{
  PBCompartment::Serialize(src, *dst.mutable_compartment());
  for (SEThermalCompartment* child : src.m_Children)
    dst.mutable_compartment()->add_child(child->GetName());
  for (SEThermalCircuitNode* nodes : src.m_Nodes.GetNodes())
    dst.mutable_compartment()->add_node(nodes->GetName());
  // Even if you have children or nodes, I am unloading everything, this makes the json actually usefull...
  if (src.HasHeatTransferRateIn())
    dst.set_allocated_heattransferratein(PBProperty::Unload(src.GetHeatTransferRateIn()));
  if (src.HasHeatTransferRateOut())
    dst.set_allocated_heattransferrateout(PBProperty::Unload(src.GetHeatTransferRateOut()));

  // Yeah, I know
  // But, these will only modify member variables if they are being used as temporary variables
  SEThermalCompartment& mutable_src = const_cast<SEThermalCompartment&>(src);
  if (src.HasHeat())
    dst.set_allocated_heat(PBProperty::Unload(mutable_src.GetHeat()));
  if (src.HasTemperature())
    dst.set_allocated_temperature(PBProperty::Unload(mutable_src.GetTemperature()));
}


void PBCompartment::Load(const CDM_BIND::ThermalCompartmentLinkData& src, SEThermalCompartmentLink& dst, SECircuitManager* circuits)
{
  dst.Clear();
  PBCompartment::Serialize(src, dst, circuits);
}
void PBCompartment::Serialize(const CDM_BIND::ThermalCompartmentLinkData& src, SEThermalCompartmentLink& dst, SECircuitManager* circuits)
{
  PBCompartment::Serialize(src.link(), dst);

  if (!src.link().path().empty())
  {
    if (circuits == nullptr)
    {
      dst.Error("Link is mapped to circuit path, " + src.link().path() + ", but no circuit manager was provided, cannot load");
      return;
    }
    SEThermalCircuitPath* path = circuits->GetThermalPath(src.link().path());
    if (path == nullptr)
    {
      dst.Error("Link is mapped to circuit path, " + src.link().path() + ", but provided circuit manager did not have that path");
      return;
    }
    dst.MapPath(*path);
  }
  else
  {
    if (src.has_heattransferrate())
      PBProperty::Load(src.heattransferrate(), dst.GetHeatTransferRate());
  }
}

CDM_BIND::ThermalCompartmentLinkData* PBCompartment::Unload(const SEThermalCompartmentLink& src)
{
  CDM_BIND::ThermalCompartmentLinkData* dst = new CDM_BIND::ThermalCompartmentLinkData();
  PBCompartment::Serialize(src, *dst);
  return dst;
}
void PBCompartment::Serialize(const SEThermalCompartmentLink& src, CDM_BIND::ThermalCompartmentLinkData& dst)
{
  PBCompartment::Serialize(src, *dst.mutable_link());
  dst.mutable_link()->set_sourcecompartment(src.m_SourceCmpt.GetName());
  dst.mutable_link()->set_targetcompartment(src.m_TargetCmpt.GetName());
  if (src.m_Path != nullptr)
    dst.mutable_link()->set_path(src.m_Path->GetName());
  // Yeah, I know
  // But, these will only modify member variables if they are being used as temporary variables
  // Even if you have a path, I am unloading everything, this makes the json actually usefull...
  SEThermalCompartmentLink& mutable_src = const_cast<SEThermalCompartmentLink&>(src);
  if (src.HasHeatTransferRate())
    dst.set_allocated_heattransferrate(PBProperty::Unload(mutable_src.GetHeatTransferRate()));
}

////////////////////////
// Tissue Compartment //
////////////////////////


void PBCompartment::Load(const CDM_BIND::TissueCompartmentData& src, SETissueCompartment& dst)
{
  dst.Clear();
  PBCompartment::Serialize(src, dst);
}
void PBCompartment::Serialize(const CDM_BIND::TissueCompartmentData& src, SETissueCompartment& dst)
{
  PBCompartment::Serialize(src.compartment(), dst);
  if (src.has_acidicphospohlipidconcentration())
    PBProperty::Load(src.acidicphospohlipidconcentration(), dst.GetAcidicPhospohlipidConcentration());
  if (src.has_matrixvolume())
    PBProperty::Load(src.matrixvolume(), dst.GetMatrixVolume());
  if (src.has_neutrallipidsvolumefraction())
    PBProperty::Load(src.neutrallipidsvolumefraction(), dst.GetNeutralLipidsVolumeFraction());
  if (src.has_neutralphospholipidsvolumefraction())
    PBProperty::Load(src.neutralphospholipidsvolumefraction(), dst.GetNeutralPhospholipidsVolumeFraction());
  if (src.has_tissuetoplasmaalbuminratio())
    PBProperty::Load(src.tissuetoplasmaalbuminratio(), dst.GetTissueToPlasmaAlbuminRatio());
  if (src.has_tissuetoplasmaalphaacidglycoproteinratio())
    PBProperty::Load(src.tissuetoplasmaalphaacidglycoproteinratio(), dst.GetTissueToPlasmaAlphaAcidGlycoproteinRatio());
  if (src.has_tissuetoplasmalipoproteinratio())
    PBProperty::Load(src.tissuetoplasmalipoproteinratio(), dst.GetTissueToPlasmaLipoproteinRatio());
  if (src.has_totalmass())
    PBProperty::Load(src.totalmass(), dst.GetTotalMass());
}

CDM_BIND::TissueCompartmentData* PBCompartment::Unload(const SETissueCompartment& src)
{
  CDM_BIND::TissueCompartmentData* dst = new CDM_BIND::TissueCompartmentData();
  PBCompartment::Serialize(src, *dst);
  return dst;
}
void PBCompartment::Serialize(const SETissueCompartment& src, CDM_BIND::TissueCompartmentData& dst)
{
  PBCompartment::Serialize(src, *dst.mutable_compartment());
  if (src.HasAcidicPhospohlipidConcentration())
    dst.set_allocated_acidicphospohlipidconcentration(PBProperty::Unload(*src.m_AcidicPhospohlipidConcentration));
  if (src.HasMatrixVolume())
    dst.set_allocated_matrixvolume(PBProperty::Unload(*src.m_MatrixVolume));
  if (src.HasNeutralLipidsVolumeFraction())
    dst.set_allocated_neutrallipidsvolumefraction(PBProperty::Unload(*src.m_NeutralLipidsVolumeFraction));
  if (src.HasNeutralPhospholipidsVolumeFraction())
    dst.set_allocated_neutralphospholipidsvolumefraction(PBProperty::Unload(*src.m_NeutralPhospholipidsVolumeFraction));
  if (src.HasTissueToPlasmaAlbuminRatio())
    dst.set_allocated_tissuetoplasmaalbuminratio(PBProperty::Unload(*src.m_TissueToPlasmaAlbuminRatio));
  if (src.HasTissueToPlasmaAlphaAcidGlycoproteinRatio())
    dst.set_allocated_tissuetoplasmaalphaacidglycoproteinratio(PBProperty::Unload(*src.m_TissueToPlasmaAlphaAcidGlycoproteinRatio));
  if (src.HasTissueToPlasmaLipoproteinRatio())
    dst.set_allocated_tissuetoplasmalipoproteinratio(PBProperty::Unload(*src.m_TissueToPlasmaLipoproteinRatio));
  if (src.HasTotalMass())
    dst.set_allocated_totalmass(PBProperty::Unload(*src.m_TotalMass));
}