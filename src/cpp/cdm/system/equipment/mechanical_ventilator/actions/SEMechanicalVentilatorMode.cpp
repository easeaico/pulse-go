/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#include "cdm/CommonDefs.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorMode.h"
#include "cdm/system/equipment/mechanical_ventilator/SEMechanicalVentilatorSettings.h"

SEMechanicalVentilatorMode::SEMechanicalVentilatorMode(Logger* logger) : SEMechanicalVentilatorAction(logger)
{
  m_Connection = eSwitch::Off;
  m_SupplementalSettings = nullptr;
}

SEMechanicalVentilatorMode::~SEMechanicalVentilatorMode()
{
  m_Connection = eSwitch::Off;
  m_SupplementalSettings = nullptr;
}

void SEMechanicalVentilatorMode::Clear()
{
  SEMechanicalVentilatorAction::Clear();
  m_Connection = eSwitch::Off;
  if (m_SupplementalSettings)
    m_SupplementalSettings->Clear();
}

bool SEMechanicalVentilatorMode::IsValid() const
{
  if (m_Connection == eSwitch::NullSwitch)
    return false;
  return SEMechanicalVentilatorAction::IsValid();
}

bool SEMechanicalVentilatorMode::IsActive() const
{
  return m_Connection == eSwitch::On;
}

bool SEMechanicalVentilatorMode::ToSettings(SEMechanicalVentilatorSettings& s, const SESubstanceManager& subMgr)
{
  if (!IsValid())
    return false;

  s.Clear();
  if (HasSupplementalSettings())
    s.Copy(*m_SupplementalSettings, subMgr);
  s.SetConnection(m_Connection);

  return true;
}

void SEMechanicalVentilatorMode::SetConnection(eSwitch c)
{
  m_Connection = c;
}
eSwitch SEMechanicalVentilatorMode::GetConnection() const
{
  return m_Connection;
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
