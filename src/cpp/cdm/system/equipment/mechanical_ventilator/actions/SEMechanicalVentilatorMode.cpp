/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#include "cdm/CommonDefs.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorMode.h"
#include "cdm/system/equipment/mechanical_ventilator/SEMechanicalVentilatorSettings.h"

SEMechanicalVentilatorMode::SEMechanicalVentilatorMode(Logger* logger) : SEMechanicalVentilatorAction(logger)
{
  m_SupplementalSettings = nullptr;
  m_SupplementalSettingsFile = "";
  m_MergeType = eMergeType::Replace;
}

SEMechanicalVentilatorMode::~SEMechanicalVentilatorMode()
{
  m_SupplementalSettings = nullptr;
  m_SupplementalSettingsFile = "";
  m_MergeType = eMergeType::Replace;
}

void SEMechanicalVentilatorMode::Clear()
{
  SEMechanicalVentilatorAction::Clear();
  if (m_SupplementalSettings)
    m_SupplementalSettings->Clear();
  m_SupplementalSettingsFile = "";
  m_MergeType = eMergeType::Replace;
}

bool SEMechanicalVentilatorMode::IsValid() const
{
  if (HasSupplementalSettingsFile())
    return SEMechanicalVentilatorAction::IsValid();
  if (HasSupplementalSettings() && GetSupplementalSettings()->HasConnection())
    return SEMechanicalVentilatorAction::IsValid();
  return false;
}

bool SEMechanicalVentilatorMode::IsActive() const
{
  if (HasSupplementalSettings() || HasSupplementalSettingsFile())
    return SEMechanicalVentilatorAction::IsActive();
  return false;
}

bool SEMechanicalVentilatorMode::ToSettings(SEMechanicalVentilatorSettings& s, SESubstanceManager& subMgr, eMergeType mt)
{
  if (!IsValid())
    return false;

  if (mt == eMergeType::Replace)
    s.Clear();
  if (HasSupplementalSettings())
    s.Merge(GetSupplementalSettings(), subMgr);
  else if (HasSupplementalSettingsFile())
  {
    // Update the action with the file contents
    std::string cfg_file = GetSupplementalSettingsFile();
    if (!GetSupplementalSettings().SerializeFromFile(cfg_file, subMgr))
      Error("Unable to load settings file");
    s.Merge(GetSupplementalSettings(), subMgr);
  }

  return true;
}

void SEMechanicalVentilatorMode::SetConnection(eSwitch c)
{
  GetSupplementalSettings().SetConnection(c);
}
eSwitch SEMechanicalVentilatorMode::GetConnection() const
{
  return GetSupplementalSettings()->GetConnection();
}

bool SEMechanicalVentilatorMode::HasSupplementalSettings() const
{
  return m_SupplementalSettings != nullptr;
}
SEMechanicalVentilatorSettings& SEMechanicalVentilatorMode::GetSupplementalSettings()
{
  if (m_SupplementalSettings == nullptr)
    m_SupplementalSettings = new SEMechanicalVentilatorSettings(GetLogger());
  return *m_SupplementalSettings;
}
const SEMechanicalVentilatorSettings* SEMechanicalVentilatorMode::GetSupplementalSettings() const
{
  return m_SupplementalSettings;
}

std::string SEMechanicalVentilatorMode::GetSupplementalSettingsFile() const
{
  return m_SupplementalSettingsFile;
}
void SEMechanicalVentilatorMode::SetSupplementalSettingsFile(const std::string& fileName)
{
  m_SupplementalSettingsFile = fileName;
}
bool SEMechanicalVentilatorMode::HasSupplementalSettingsFile() const
{
  return !m_SupplementalSettingsFile.empty();
}

void SEMechanicalVentilatorMode::SetMergeType(eMergeType m)
{
  m_MergeType = m;
}
eMergeType SEMechanicalVentilatorMode::GetMergeType() const
{
  return m_MergeType;
}
