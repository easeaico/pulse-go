/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/blackbox/SEBlackBoxManager.h"

#include "cdm/compartment/electrical/SEElectricalCompartment.h"
#include "cdm/compartment/electrical/SEElectricalCompartmentLink.h"
#include "cdm/compartment/fluid/SEGasCompartment.h"
#include "cdm/compartment/fluid/SEGasCompartmentLink.h"
#include "cdm/compartment/fluid/SELiquidCompartment.h"
#include "cdm/compartment/fluid/SELiquidCompartmentLink.h"
#include "cdm/compartment/thermal/SEThermalCompartment.h"
#include "cdm/compartment/thermal/SEThermalCompartmentLink.h"

#include "cdm/circuit/electrical/SEElectricalCircuitPath.h"
#include "cdm/circuit/fluid/SEFluidCircuitPath.h"
#include "cdm/circuit/thermal/SEThermalCircuitPath.h"

SEBlackBoxManager::SEBlackBoxManager(Logger* logger) : Loggable(logger)
{

}
SEBlackBoxManager::~SEBlackBoxManager()
{
  DELETE_MAP_SECOND(m_ElectricalBoxes);
  DELETE_MAP_SECOND(m_GasBoxes);
  DELETE_MAP_SECOND(m_LiquidBoxes);
  DELETE_MAP_SECOND(m_ThermalBoxes);
}

std::string SEBlackBoxManager::GetBlackBoxName(const std::string& srcCmptName, const std::string& tgtCmptName)
{
  std::string name = srcCmptName + "_to_" + tgtCmptName;
  return name;
}

bool SEBlackBoxManager::HasElectricalBlackBox(const std::string& name) const
{
  auto bb = m_ElectricalBoxes.find(name);
  return bb != m_ElectricalBoxes.end();
}
bool SEBlackBoxManager::HasElectricalBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName) const
{
  auto bb = m_ElectricalBoxes.find(GetBlackBoxName(srcCmptName, tgtCmptName));
  return bb != m_ElectricalBoxes.end();
}
SEElectricalBlackBox* SEBlackBoxManager::GetElectricalBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName, std::string name)
{
  if (name.empty())
    name = GetBlackBoxName(srcCmptName, tgtCmptName);
  auto bb = m_ElectricalBoxes.find(name);
  if (bb != m_ElectricalBoxes.end())
    return bb->second;
  return CreateElectricalBlackBox(srcCmptName, tgtCmptName, name);
}
SEElectricalBlackBox* SEBlackBoxManager::CreateElectricalBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName, const std::string& name)
{
  SEElectricalBlackBox* bb = CreateBlackBox<SEElectricalBlackBox>(name, m_ElectricalBoxes);
  m_ElectricalBoxes[name] = bb;
  return bb;
}

bool SEBlackBoxManager::HasGasBlackBox(const std::string& name) const
{
  auto bb = m_GasBoxes.find(name);
  return bb != m_GasBoxes.end();
}
bool SEBlackBoxManager::HasGasBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName) const
{
  auto bb = m_GasBoxes.find(GetBlackBoxName(srcCmptName, tgtCmptName));
  return bb != m_GasBoxes.end();
}
SEGasBlackBox* SEBlackBoxManager::GetGasBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName, std::string name)
{
  if (name.empty())
    name = GetBlackBoxName(srcCmptName, tgtCmptName);
  auto bb = m_GasBoxes.find(name);
  if (bb != m_GasBoxes.end())
    return bb->second;
  return CreateGasBlackBox(srcCmptName, tgtCmptName, name);
}
SEGasBlackBox* SEBlackBoxManager::CreateGasBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName, const std::string& name)
{
  SEGasBlackBox* bb = CreateBlackBox<SEGasBlackBox>(name, m_GasBoxes);
 m_GasBoxes[name] = bb;
  return bb;
}

bool SEBlackBoxManager::HasLiquidBlackBox(const std::string& name) const
{
  auto bb = m_LiquidBoxes.find(name);
  return bb != m_LiquidBoxes.end();
}
bool SEBlackBoxManager::HasLiquidBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName) const
{
  auto bb = m_LiquidBoxes.find(GetBlackBoxName(srcCmptName, tgtCmptName));
  return bb != m_LiquidBoxes.end();
}
SELiquidBlackBox* SEBlackBoxManager::GetLiquidBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName, std::string name)
{
  if (name.empty())
    name = GetBlackBoxName(srcCmptName, tgtCmptName);
  auto bb = m_LiquidBoxes.find(name);
  if (bb != m_LiquidBoxes.end())
    return bb->second;
  return CreateLiquidBlackBox(srcCmptName, tgtCmptName, name);
}
SELiquidBlackBox* SEBlackBoxManager::CreateLiquidBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName, const std::string& name)
{
  SELiquidBlackBox* bb = CreateBlackBox<SELiquidBlackBox>(name, m_LiquidBoxes);
 m_LiquidBoxes[name] = bb;
  return bb;
}

bool SEBlackBoxManager::HasThermalBlackBox(const std::string& name) const
{
  auto bb = m_ThermalBoxes.find(name);
  return bb != m_ThermalBoxes.end();
}
bool SEBlackBoxManager::HasThermalBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName) const
{
  auto bb = m_ThermalBoxes.find(GetBlackBoxName(srcCmptName, tgtCmptName));
  return bb != m_ThermalBoxes.end();
}
SEThermalBlackBox* SEBlackBoxManager::GetThermalBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName, std::string name)
{
  if (name.empty())
    name = GetBlackBoxName(srcCmptName, tgtCmptName);
  auto bb = m_ThermalBoxes.find(name);
  if (bb != m_ThermalBoxes.end())
    return bb->second;
  return CreateThermalBlackBox(srcCmptName, tgtCmptName, name);
}
SEThermalBlackBox* SEBlackBoxManager::CreateThermalBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName, const std::string& name)
{
  SEThermalBlackBox* bb = CreateBlackBox<SEThermalBlackBox>(name, m_ThermalBoxes);
  m_ThermalBoxes[name] = bb;
  return bb;
}

template<typename BlackBoxType>
BlackBoxType* SEBlackBoxManager::CreateBlackBox(const std::string& name, std::map<std::string, BlackBoxType*>& ledger)
{
  BlackBoxType* bb = ledger[name];
  if (bb != nullptr)
  {
    Error("Black box already exists with name " + name);
    return nullptr;
  }
  bb = new BlackBoxType(name, GetLogger());
  ledger[name] = bb;

  return bb;
}
