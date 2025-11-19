/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/io/protobuf/PBEngine.h"

SEDataRequestManager::SEDataRequestManager(Logger* logger) : Loggable(logger)
{
  m_DefaultDecimalFormatting = nullptr;
  m_OverrideDecimalFormatting = nullptr;
  m_ResultsFilename = "";
  m_SamplesPerSecond = 0; // Sample every time step
}

SEDataRequestManager::~SEDataRequestManager()
{
  m_ResultsFilename = "";
  m_SamplesPerSecond = 0; // Sample every time step
  DELETE_VECTOR(m_Requests);
  SAFE_DELETE(m_DefaultDecimalFormatting);
  SAFE_DELETE(m_OverrideDecimalFormatting);
}

void SEDataRequestManager::Clear()
{
  m_ResultsFilename = "";
  m_SamplesPerSecond = 0; // Sample every time step
  DELETE_VECTOR(m_Requests);
  SAFE_DELETE(m_DefaultDecimalFormatting);
  SAFE_DELETE(m_OverrideDecimalFormatting);
}

void SEDataRequestManager::Copy(const SEDataRequestManager& src)
{
  PBEngine::Copy(src, *this);
}

bool SEDataRequestManager::SerializeToString(std::string& output, eSerializationFormat m) const
{
  return PBEngine::SerializeToString(*this, output, m);
}
bool SEDataRequestManager::SerializeToFile(const std::string& filename) const
{
  return PBEngine::SerializeToFile(*this, filename);
}
bool SEDataRequestManager::SerializeFromString(const std::string& src, eSerializationFormat m)
{
  return PBEngine::SerializeFromString(src, *this, m);
}
bool SEDataRequestManager::SerializeFromFile(const std::string& filename)
{
  return PBEngine::SerializeFromFile(filename, *this);
}

bool SEDataRequestManager::SerializeDataRequestsToString(std::string& output, eSerializationFormat m) const
{
  return PBEngine::SerializeToString(m_Requests, output, m);
}
bool SEDataRequestManager::SerializeDataRequestsToFile(const std::string& filename) const
{
  return PBEngine::SerializeToFile(m_Requests, filename);
}
bool SEDataRequestManager::SerializeDataRequestsFromString(const std::string& src, eSerializationFormat m)
{
  return PBEngine::SerializeFromString(src, m_Requests, m);
}
bool SEDataRequestManager::SerializeDataRequestsFromFile(const std::string& filename)
{
  return PBEngine::SerializeFromFile(filename, m_Requests);
}
bool SEDataRequestManager::MergeDataRequestFile(const std::string& filename)
{
  std::vector<SEDataRequest*> drs;
  if (!PBEngine::SerializeFromFile(filename, drs))
    return false;

  for (auto dr : drs)
    CopyDataRequest(*dr, false);

  return true;
}

bool SEDataRequestManager::HasDefaultDecimalFormatting() const
{
  return m_DefaultDecimalFormatting != nullptr;
}
SEDecimalFormat& SEDataRequestManager::GetDefaultDecimalFormatting()
{
  if (m_DefaultDecimalFormatting == nullptr)
    m_DefaultDecimalFormatting = new SEDecimalFormat();
  return *m_DefaultDecimalFormatting;
}
void SEDataRequestManager::RemoveDefaultDecimalFormatting()
{
  SAFE_DELETE(m_DefaultDecimalFormatting);
}

bool SEDataRequestManager::HasOverrideDecimalFormatting() const
{
  return m_OverrideDecimalFormatting != nullptr;
}
SEDecimalFormat& SEDataRequestManager::GetOverrideDecimalFormatting()
{
  if (m_OverrideDecimalFormatting == nullptr)
    m_OverrideDecimalFormatting = new SEDecimalFormat();
  return *m_OverrideDecimalFormatting;
}
void SEDataRequestManager::RemoveOverrideDecimalFormatting()
{
  SAFE_DELETE(m_OverrideDecimalFormatting);
}

SEDataRequest& SEDataRequestManager::CopyDataRequest(const SEDataRequest& src, bool updateExisting)
{
  SEDataRequest* dst = FindDataRequest(src);
  if (dst == nullptr)
  {
    dst = new SEDataRequest(src);
    m_Requests.push_back(dst);
  }
  else if (!updateExisting)
  {
    Warning("Ignoring duplicate data request:\n" + dst->ToString());
    return *dst;
  }

  dst->Copy(src);

  return *dst;
}

SEDataRequest& SEDataRequestManager::CopyDataRequest(const SEDataRequest& dr)
{
  return CopyDataRequest(dr, true);
}

SEDataRequest* SEDataRequestManager::FindDataRequest(const SEDataRequest& dr)
{
  SEDataRequest* my_dr;
  switch (dr.GetCategory())
  {
  case eDataRequest_Category::Patient:
    my_dr = FindPatientDataRequest(dr.GetPropertyName());
    return my_dr;
  case eDataRequest_Category::Physiology:
    my_dr = FindPhysiologyDataRequest(dr.GetPropertyName());
    return my_dr;
  case eDataRequest_Category::Environment:
    my_dr = FindEnvironmentDataRequest(dr.GetPropertyName());
    return my_dr;
  case eDataRequest_Category::Action:
    my_dr = FindActionDataRequest(dr.GetActionName(), dr.GetCompartmentName(), dr.GetSubstanceName(), dr.GetPropertyName());
    return my_dr;
  case eDataRequest_Category::GasCompartment:
    if (dr.HasSubstanceName())
      my_dr = FindGasCompartmentDataRequest(dr.GetCompartmentName(), dr.GetSubstanceName(), dr.GetPropertyName());
    else
      my_dr = FindGasCompartmentDataRequest(dr.GetCompartmentName(), dr.GetPropertyName());
    return my_dr;
  case eDataRequest_Category::LiquidCompartment:
    if (dr.HasSubstanceName())
      my_dr = FindLiquidCompartmentDataRequest(dr.GetCompartmentName(), dr.GetSubstanceName(), dr.GetPropertyName());
    else
      my_dr = FindLiquidCompartmentDataRequest(dr.GetCompartmentName(), dr.GetPropertyName());
    return my_dr;
  case eDataRequest_Category::ThermalCompartment:
    my_dr = FindThermalCompartmentDataRequest(dr.GetCompartmentName(), dr.GetPropertyName());
    return my_dr;
  case eDataRequest_Category::TissueCompartment:
    my_dr = FindTissueCompartmentDataRequest(dr.GetCompartmentName(), dr.GetPropertyName());
    return my_dr;
  case eDataRequest_Category::Substance:
    my_dr = FindSubstanceDataRequest(dr.GetSubstanceName(), dr.GetPropertyName());
    return my_dr;
  case eDataRequest_Category::AnesthesiaMachine:
    my_dr = FindAnesthesiaMachineDataRequest(dr.GetPropertyName());
    return my_dr;
  case eDataRequest_Category::ECG:
    my_dr = FindECGDataRequest(dr.GetPropertyName());
    return my_dr;
  case eDataRequest_Category::ECMO:
    my_dr = FindECMODataRequest(dr.GetPropertyName());
    return my_dr;
  case eDataRequest_Category::Inhaler:
    my_dr = FindInhalerDataRequest(dr.GetPropertyName());
    return my_dr;
  case eDataRequest_Category::MechanicalVentilator:
    my_dr = FindMechanicalVentilatorDataRequest(dr.GetPropertyName());
    return my_dr;
  default:
    return nullptr;
  }
}
SEDataRequest* SEDataRequestManager::FindDataRequest(const std::string& header) const
{
  for (SEDataRequest* dr : m_Requests)
  {
    if (dr->GetHeaderName() == header)
      return dr;
  }
  return nullptr;
}


SEDataRequest& SEDataRequestManager::CreateDataRequest(eDataRequest_Category category, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = new SEDataRequest(category, dfault);
  m_Requests.push_back(dr);
  return *dr;
}

SEDataRequest& SEDataRequestManager::CreatePatientDataRequest(const std::string& property, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindPatientDataRequest(property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::Patient, dfault);
  m_Requests.push_back(dr);
  dr->SetPropertyName(property);
  return *dr;
}
SEDataRequest& SEDataRequestManager::CreatePatientDataRequest(const std::string& property, const CCompoundUnit& unit, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindPatientDataRequest(property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::Patient, dfault);
  m_Requests.push_back(dr);
  dr->SetPropertyName(property);
  dr->SetUnit(unit);
  return *dr;
}
SEDataRequest* SEDataRequestManager::FindPatientDataRequest(const std::string& property)
{
  for (SEDataRequest* dr : m_Requests)
  {
    if (dr->GetCategory() == eDataRequest_Category::Patient && dr->GetPropertyName() == property)
      return dr;
  }
  return nullptr;
}

SEDataRequest& SEDataRequestManager::CreatePhysiologyDataRequest(const std::string& property, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindPhysiologyDataRequest(property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::Physiology, dfault);
  m_Requests.push_back(dr);
  dr->SetPropertyName(property);
  return *dr;
}
SEDataRequest& SEDataRequestManager::CreatePhysiologyDataRequest(const std::string& property, const CCompoundUnit& unit, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindPhysiologyDataRequest(property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::Physiology, dfault);
  m_Requests.push_back(dr);
  dr->SetPropertyName(property);
  dr->SetUnit(unit);
  return *dr;
}
SEDataRequest* SEDataRequestManager::FindPhysiologyDataRequest(const std::string& property)
{
  for (SEDataRequest* dr : m_Requests)
  {
    if (dr->GetCategory() == eDataRequest_Category::Physiology && dr->GetPropertyName() == property)
      return dr;
  }
  return nullptr;
}

SEDataRequest& SEDataRequestManager::CreateEnvironmentDataRequest(const std::string& property, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindEnvironmentDataRequest(property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::Environment, dfault);
  m_Requests.push_back(dr);
  dr->SetPropertyName(property);
  return *dr;
}
SEDataRequest& SEDataRequestManager::CreateEnvironmentDataRequest(const std::string& property, const CCompoundUnit& unit, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindEnvironmentDataRequest(property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::Environment, dfault);
  m_Requests.push_back(dr);
  dr->SetPropertyName(property);
  dr->SetUnit(unit);
  return *dr;
}
SEDataRequest* SEDataRequestManager::FindEnvironmentDataRequest(const std::string& property)
{
  for (SEDataRequest* dr : m_Requests)
  {
    if (dr->GetCategory() == eDataRequest_Category::Environment && dr->GetPropertyName() == property)
      return dr;
  }
  return nullptr;
}

SEDataRequest& SEDataRequestManager::CreateActionDataRequest(const std::string& actionName, const std::string& property, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindActionDataRequest(actionName, "", "", property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::Action, dfault);
  m_Requests.push_back(dr);
  dr->SetActionName(actionName);
  dr->SetPropertyName(property);
  return *dr;
}
SEDataRequest& SEDataRequestManager::CreateActionDataRequest(const std::string& actionName, const std::string& property, const CCompoundUnit& unit, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindActionDataRequest(actionName, "", "", property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::Action, dfault);
  m_Requests.push_back(dr);
  dr->SetActionName(actionName);
  dr->SetPropertyName(property);
  dr->SetUnit(unit);
  return *dr;
}
SEDataRequest& SEDataRequestManager::CreateActionCompartmentDataRequest(const std::string& actionName, const std::string& cmptName, const std::string& property, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindActionDataRequest(actionName, cmptName, "", property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::Action, dfault);
  m_Requests.push_back(dr);
  dr->SetActionName(actionName);
  dr->SetCompartmentName(cmptName);
  dr->SetPropertyName(property);
  return *dr;
}
SEDataRequest& SEDataRequestManager::CreateActionCompartmentDataRequest(const std::string& actionName, const std::string& cmptName, const std::string& property, const CCompoundUnit& unit, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindActionDataRequest(actionName, cmptName, "", property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::Action, dfault);
  m_Requests.push_back(dr);
  dr->SetActionName(actionName);
  dr->SetCompartmentName(cmptName);
  dr->SetPropertyName(property);
  dr->SetUnit(unit);
  return *dr;
}
SEDataRequest& SEDataRequestManager::CreateActionSubstanceDataRequest(const std::string& actionName, const std::string& substance, const std::string& property, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindActionDataRequest(actionName, "", substance, property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::Action, dfault);
  m_Requests.push_back(dr);
  dr->SetActionName(actionName);
  dr->SetSubstanceName(substance);
  dr->SetPropertyName(property);
  return *dr;
}
SEDataRequest& SEDataRequestManager::CreateActionSubstanceDataRequest(const std::string& actionName, const std::string& substance, const std::string& property, const CCompoundUnit& unit, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindActionDataRequest(actionName, "", substance, property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::Action, dfault);
  m_Requests.push_back(dr);
  dr->SetActionName(actionName);
  dr->SetSubstanceName(substance);
  dr->SetPropertyName(property);
  dr->SetUnit(unit);
  return *dr;
}
SEDataRequest* SEDataRequestManager::FindActionDataRequest(const std::string& actionName, const std::string& cmptName, const std::string& substance, const std::string& property)
{
  for (SEDataRequest* dr : m_Requests)
  {
    if (dr->GetCategory() == eDataRequest_Category::Action &&
      dr->GetPropertyName() == property &&
      dr->GetActionName() == actionName &&
      dr->GetCompartmentName() == cmptName &&
      dr->GetSubstanceName() == substance)
      return dr;
  }
  return nullptr;
}

SEDataRequest& SEDataRequestManager::CreateGasCompartmentDataRequest(const std::string& cmptName, const std::string& property, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindGasCompartmentDataRequest(cmptName, property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::GasCompartment, dfault);
  m_Requests.push_back(dr);
  dr->SetCompartmentName(cmptName);
  dr->SetPropertyName(property);
  return *dr;
}
SEDataRequest& SEDataRequestManager::CreateGasCompartmentDataRequest(const std::string& cmptName, const std::string& property, const CCompoundUnit& unit, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindGasCompartmentDataRequest(cmptName, property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::GasCompartment, dfault);
  m_Requests.push_back(dr);
  dr->SetCompartmentName(cmptName);
  dr->SetPropertyName(property);
  dr->SetUnit(unit);
  return *dr;
}
SEDataRequest* SEDataRequestManager::FindGasCompartmentDataRequest(const std::string& cmptName, const std::string& property)
{
  for (SEDataRequest* dr : m_Requests)
  {
    if (dr->GetCategory() == eDataRequest_Category::GasCompartment &&
      dr->GetPropertyName() == property &&
      dr->GetCompartmentName() == cmptName)
      return dr;
  }
  return nullptr;
}

SEDataRequest& SEDataRequestManager::CreateGasCompartmentDataRequest(const std::string& cmptName, const std::string& substance, const std::string& property, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindGasCompartmentDataRequest(cmptName, substance, property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::GasCompartment, dfault);
  m_Requests.push_back(dr);
  dr->SetCompartmentName(cmptName);
  dr->SetSubstanceName(substance);
  dr->SetPropertyName(property);
  return *dr;
}
SEDataRequest& SEDataRequestManager::CreateGasCompartmentDataRequest(const std::string& cmptName, const std::string& substance, const std::string& property, const CCompoundUnit& unit, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindGasCompartmentDataRequest(cmptName, substance, property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::GasCompartment, dfault);
  m_Requests.push_back(dr);
  dr->SetCompartmentName(cmptName);
  dr->SetSubstanceName(substance);
  dr->SetPropertyName(property);
  dr->SetUnit(unit);
  return *dr;
}
SEDataRequest* SEDataRequestManager::FindGasCompartmentDataRequest(const std::string& cmptName, const std::string& substance, const std::string& property)
{
  for (SEDataRequest* dr : m_Requests)
  {
    if (dr->GetCategory() == eDataRequest_Category::GasCompartment &&
      dr->GetPropertyName() == property &&
      dr->GetCompartmentName() == cmptName &&
      dr->GetSubstanceName() == substance)
      return dr;
  }
  return nullptr;
}

SEDataRequest& SEDataRequestManager::CreateLiquidCompartmentDataRequest(const std::string& cmptName, const std::string& property, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindLiquidCompartmentDataRequest(cmptName, property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::LiquidCompartment, dfault);
  m_Requests.push_back(dr);
  dr->SetCompartmentName(cmptName);
  dr->SetPropertyName(property);
  return *dr;
}
SEDataRequest& SEDataRequestManager::CreateLiquidCompartmentDataRequest(const std::string& cmptName, const std::string& property, const CCompoundUnit& unit, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindLiquidCompartmentDataRequest(cmptName, property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::LiquidCompartment, dfault);
  m_Requests.push_back(dr);
  dr->SetCompartmentName(cmptName);
  dr->SetPropertyName(property);
  dr->SetUnit(unit);
  return *dr;
}
SEDataRequest* SEDataRequestManager::FindLiquidCompartmentDataRequest(const std::string& cmptName, const std::string& property)
{
  for (SEDataRequest* dr : m_Requests)
  {
    if (dr->GetCategory() == eDataRequest_Category::LiquidCompartment &&
      dr->GetPropertyName() == property &&
      dr->GetCompartmentName() == cmptName)
      return dr;
  }
  return nullptr;
}

SEDataRequest& SEDataRequestManager::CreateLiquidCompartmentDataRequest(const std::string& cmptName, const std::string& substance, const std::string& property, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindLiquidCompartmentDataRequest(cmptName, substance, property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::LiquidCompartment, dfault);
  m_Requests.push_back(dr);
  dr->SetCompartmentName(cmptName);
  dr->SetSubstanceName(substance);
  dr->SetPropertyName(property);
  return *dr;
}
SEDataRequest& SEDataRequestManager::CreateLiquidCompartmentDataRequest(const std::string& cmptName, const std::string& substance, const std::string& property, const CCompoundUnit& unit, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindLiquidCompartmentDataRequest(cmptName, substance, property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::LiquidCompartment, dfault);
  m_Requests.push_back(dr);
  dr->SetCompartmentName(cmptName);
  dr->SetSubstanceName(substance);
  dr->SetPropertyName(property);
  dr->SetUnit(unit);
  return *dr;
}
SEDataRequest* SEDataRequestManager::FindLiquidCompartmentDataRequest(const std::string& cmptName, const std::string& substance, const std::string& property)
{
  for (SEDataRequest* dr : m_Requests)
  {
    if (dr->GetCategory() == eDataRequest_Category::LiquidCompartment &&
        dr->GetPropertyName() == property &&
        dr->GetCompartmentName() == cmptName &&
        dr->GetSubstanceName() == substance)
      return dr;
  }
  return nullptr;
}

SEDataRequest& SEDataRequestManager::CreateThermalCompartmentDataRequest(const std::string& cmptName, const std::string& property, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindThermalCompartmentDataRequest(cmptName, property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::ThermalCompartment, dfault);
  m_Requests.push_back(dr);
  dr->SetCompartmentName(cmptName);
  dr->SetPropertyName(property);
  return *dr;
}
SEDataRequest& SEDataRequestManager::CreateThermalCompartmentDataRequest(const std::string& cmptName, const std::string& property, const CCompoundUnit& unit, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindThermalCompartmentDataRequest(cmptName, property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::ThermalCompartment, dfault);
  m_Requests.push_back(dr);
  dr->SetCompartmentName(cmptName);
  dr->SetPropertyName(property);
  dr->SetUnit(unit);
  return *dr;
}
SEDataRequest* SEDataRequestManager::FindThermalCompartmentDataRequest(const std::string& cmptName, const std::string& property)
{
  for (SEDataRequest* dr : m_Requests)
  {
    if (dr->GetCategory() == eDataRequest_Category::ThermalCompartment &&
        dr->GetCompartmentName() == cmptName &&
        dr->GetPropertyName() == property)
      return dr;
  }
  return nullptr;
}

SEDataRequest& SEDataRequestManager::CreateTissueCompartmentDataRequest(const std::string& cmptName, const std::string& property, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindTissueCompartmentDataRequest(cmptName, property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::TissueCompartment, dfault);
  m_Requests.push_back(dr);
  dr->SetCompartmentName(cmptName);
  dr->SetPropertyName(property);
  return *dr;
}
SEDataRequest& SEDataRequestManager::CreateTissueCompartmentDataRequest(const std::string& cmptName, const std::string& property, const CCompoundUnit& unit, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindTissueCompartmentDataRequest(cmptName, property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::TissueCompartment, dfault);
  m_Requests.push_back(dr);
  dr->SetCompartmentName(cmptName);
  dr->SetPropertyName(property);
  dr->SetUnit(unit);
  return *dr;
}
SEDataRequest& SEDataRequestManager::CreateTissueCompartmentDataRequest(const std::string& cmptName, const std::string& substance, const std::string& property, const CCompoundUnit& unit, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindTissueCompartmentDataRequest(cmptName, property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::TissueCompartment, dfault);
  m_Requests.push_back(dr);
  dr->SetCompartmentName(cmptName);
  dr->SetSubstanceName(substance);
  dr->SetPropertyName(property);
  dr->SetUnit(unit);
  return *dr;
}
SEDataRequest* SEDataRequestManager::FindTissueCompartmentDataRequest(const std::string& cmptName, const std::string& property)
{
  for (SEDataRequest* dr : m_Requests)
  {
    if (dr->GetCategory() == eDataRequest_Category::TissueCompartment &&
        dr->GetCompartmentName() == cmptName &&
        dr->GetPropertyName() == property)
      return dr;
  }
  return nullptr;
}

SEDataRequest& SEDataRequestManager::CreateSubstanceDataRequest(const std::string& substance, const std::string& property, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindSubstanceDataRequest(substance, property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::Substance, dfault);
  m_Requests.push_back(dr);
  dr->SetSubstanceName(substance);
  dr->SetPropertyName(property);
  return *dr;
}
SEDataRequest& SEDataRequestManager::CreateSubstanceDataRequest(const std::string& substance, const std::string& property, const CCompoundUnit& unit, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindSubstanceDataRequest(substance, property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::Substance, dfault);
  m_Requests.push_back(dr);
  dr->SetSubstanceName(substance);
  dr->SetPropertyName(property);
  dr->SetUnit(unit);
  return *dr;
}
SEDataRequest& SEDataRequestManager::CreateSubstanceDataRequest(const std::string& substance, const std::string& cmptName, const std::string& property, const CCompoundUnit& unit, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindSubstanceDataRequest(substance, property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::Substance, dfault);
  m_Requests.push_back(dr);
  dr->SetSubstanceName(substance);
  dr->SetCompartmentName(cmptName);
  dr->SetPropertyName(property);
  dr->SetUnit(unit);
  return *dr;
}
SEDataRequest* SEDataRequestManager::FindSubstanceDataRequest(const std::string& substance, const std::string& property)
{
  for (SEDataRequest* dr : m_Requests)
  {
    if (dr->GetCategory() == eDataRequest_Category::Substance &&
        dr->GetPropertyName() == property &&
        dr->GetSubstanceName() == substance)
      return dr;
  }
  return nullptr;
}

SEDataRequest& SEDataRequestManager::CreateAnesthesiaMachineDataRequest(const std::string& property, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindAnesthesiaMachineDataRequest(property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::AnesthesiaMachine, dfault);
  m_Requests.push_back(dr);
  dr->SetPropertyName(property);
  return *dr;
}
SEDataRequest& SEDataRequestManager::CreateAnesthesiaMachineDataRequest(const std::string& property, const CCompoundUnit& unit, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindAnesthesiaMachineDataRequest(property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::AnesthesiaMachine, dfault);
  m_Requests.push_back(dr);
  dr->SetPropertyName(property);
  dr->SetUnit(unit);
  return *dr;
}
SEDataRequest* SEDataRequestManager::FindAnesthesiaMachineDataRequest(const std::string& property)
{
  for (SEDataRequest* dr : m_Requests)
  {
    if (dr->GetCategory() == eDataRequest_Category::AnesthesiaMachine &&
        dr->GetPropertyName() == property)
      return dr;
  }
  return nullptr;
}

SEDataRequest& SEDataRequestManager::CreateECGDataRequest(const std::string& property, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindECGDataRequest(property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::ECG, dfault);
  m_Requests.push_back(dr);
  dr->SetPropertyName(property);
  return *dr;
}
SEDataRequest& SEDataRequestManager::CreateECGDataRequest(const std::string& property, const CCompoundUnit& unit, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindECGDataRequest(property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::ECG, dfault);
  m_Requests.push_back(dr);
  dr->SetPropertyName(property);
  dr->SetUnit(unit);
  return *dr;
}
SEDataRequest* SEDataRequestManager::FindECGDataRequest(const std::string& property)
{
  for (SEDataRequest* dr : m_Requests)
  {
    if (dr->GetCategory() == eDataRequest_Category::ECG && dr->GetPropertyName() == property)
      return dr;
  }
  return nullptr;
}

SEDataRequest& SEDataRequestManager::CreateECMODataRequest(const std::string& property, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindECMODataRequest(property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::ECMO, dfault);
  m_Requests.push_back(dr);
  dr->SetPropertyName(property);
  return *dr;
}
SEDataRequest& SEDataRequestManager::CreateECMODataRequest(const std::string& property, const CCompoundUnit& unit, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindECMODataRequest(property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::ECMO, dfault);
  m_Requests.push_back(dr);
  dr->SetPropertyName(property);
  dr->SetUnit(unit);
  return *dr;
}
SEDataRequest* SEDataRequestManager::FindECMODataRequest(const std::string& property)
{
  for (SEDataRequest* dr : m_Requests)
  {
    if (dr->GetCategory() == eDataRequest_Category::ECMO &&
      dr->GetPropertyName() == property)
      return dr;
  }
  return nullptr;
}

SEDataRequest& SEDataRequestManager::CreateInhalerDataRequest(const std::string& property, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindInhalerDataRequest(property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::Inhaler, dfault);
  m_Requests.push_back(dr);
  dr->SetPropertyName(property);
  return *dr;
}
SEDataRequest& SEDataRequestManager::CreateInhalerDataRequest(const std::string& property, const CCompoundUnit& unit, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindInhalerDataRequest(property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::Inhaler, dfault);
  m_Requests.push_back(dr);
  dr->SetPropertyName(property);
  dr->SetUnit(unit);
  return *dr;
}
SEDataRequest* SEDataRequestManager::FindInhalerDataRequest(const std::string& property)
{
  for (SEDataRequest* dr : m_Requests)
  {
    if (dr->GetCategory() == eDataRequest_Category::Inhaler &&
        dr->GetPropertyName() == property)
      return dr;
  }
  return nullptr;
}

SEDataRequest& SEDataRequestManager::CreateMechanicalVentilatorDataRequest(const std::string& property, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindMechanicalVentilatorDataRequest(property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::MechanicalVentilator, dfault);
  m_Requests.push_back(dr);
  dr->SetPropertyName(property);
  return *dr;
}
SEDataRequest& SEDataRequestManager::CreateMechanicalVentilatorDataRequest(const std::string& property, const CCompoundUnit& unit, const SEDecimalFormat* dfault)
{
  SEDataRequest* dr = FindMechanicalVentilatorDataRequest(property);
  if (dr != nullptr)
    return *dr;
  dr = new SEDataRequest(eDataRequest_Category::MechanicalVentilator, dfault);
  m_Requests.push_back(dr);
  dr->SetPropertyName(property);
  dr->SetUnit(unit);
  return *dr;
}
SEDataRequest* SEDataRequestManager::FindMechanicalVentilatorDataRequest(const std::string& property)
{
  for (SEDataRequest* dr : m_Requests)
  {
    if (dr->GetCategory() == eDataRequest_Category::MechanicalVentilator &&
      dr->GetPropertyName() == property)
      return dr;
  }
  return nullptr;
}

void SEDataRequestManager::Remove(const SEDataRequest& dr)
{
  for (size_t i=0; i<m_Requests.size(); ++i)
  {
    if (m_Requests[i] == &dr)
    {
      m_Requests.erase(m_Requests.begin() + 1);
      break;
    }
  }
}
