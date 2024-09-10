/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/system/physiology/SERespiratoryMechanicsModifiers.h"
#include "cdm/io/protobuf/PBProperties.h"
#include "cdm/io/protobuf/PBPhysiology.h"
#include "cdm/patient/actions/SERespiratoryMechanicsModification.h"
#include "cdm/properties/SEScalarUnsigned.h"
#include "cdm/properties/SEScalarVolume.h"

SERespiratoryMechanicsModifiers::SERespiratoryMechanicsModifiers(Logger* logger) : Loggable(logger)
{
  m_LeftComplianceMultiplier = nullptr;
  m_RightComplianceMultiplier = nullptr;
  m_LeftExpiratoryResistanceMultiplier = nullptr;
  m_LeftInspiratoryResistanceMultiplier = nullptr;
  m_RightExpiratoryResistanceMultiplier = nullptr;
  m_RightInspiratoryResistanceMultiplier = nullptr;
  m_UpperExpiratoryResistanceMultiplier = nullptr;
  m_UpperInspiratoryResistanceMultiplier = nullptr;
  m_RespirationRateMultiplier = nullptr;
  m_TidalVolumeMultiplier = nullptr;
}

SERespiratoryMechanicsModifiers::~SERespiratoryMechanicsModifiers()
{
  SAFE_DELETE(m_LeftComplianceMultiplier);
  SAFE_DELETE(m_RightComplianceMultiplier);
  SAFE_DELETE(m_LeftExpiratoryResistanceMultiplier);
  SAFE_DELETE(m_LeftInspiratoryResistanceMultiplier);
  SAFE_DELETE(m_RightExpiratoryResistanceMultiplier);
  SAFE_DELETE(m_RightInspiratoryResistanceMultiplier);
  SAFE_DELETE(m_UpperExpiratoryResistanceMultiplier);
  SAFE_DELETE(m_UpperInspiratoryResistanceMultiplier);
  SAFE_DELETE(m_RespirationRateMultiplier);
  SAFE_DELETE(m_TidalVolumeMultiplier);

  DELETE_MAP_SECOND(m_VolumeIncrements);
}

void SERespiratoryMechanicsModifiers::Clear()
{
  INVALIDATE_PROPERTY(m_LeftComplianceMultiplier);
  INVALIDATE_PROPERTY(m_RightComplianceMultiplier);
  INVALIDATE_PROPERTY(m_LeftExpiratoryResistanceMultiplier);
  INVALIDATE_PROPERTY(m_LeftInspiratoryResistanceMultiplier);
  INVALIDATE_PROPERTY(m_RightExpiratoryResistanceMultiplier);
  INVALIDATE_PROPERTY(m_RightInspiratoryResistanceMultiplier);
  INVALIDATE_PROPERTY(m_UpperExpiratoryResistanceMultiplier);
  INVALIDATE_PROPERTY(m_UpperInspiratoryResistanceMultiplier);
  INVALIDATE_PROPERTY(m_RespirationRateMultiplier);
  INVALIDATE_PROPERTY(m_TidalVolumeMultiplier);

  for (auto itr : m_VolumeIncrements)
    if (itr.second != nullptr)
      itr.second->Invalidate();
}

// We want our models to have the assumption that all modifiers are present
// So if we don't have anything, just use 1, as this would be a no op in code
void SERespiratoryMechanicsModifiers::Activate()
{
  if (!HasLeftComplianceMultiplier())
    GetLeftComplianceMultiplier().SetValue(1.0);
  if (!HasRightComplianceMultiplier())
    GetRightComplianceMultiplier().SetValue(1.0);
  if (!HasLeftExpiratoryResistanceMultiplier())
    GetLeftExpiratoryResistanceMultiplier().SetValue(1.0);
  if (!HasLeftInspiratoryResistanceMultiplier())
    GetLeftInspiratoryResistanceMultiplier().SetValue(1.0);
  if (!HasRightExpiratoryResistanceMultiplier())
    GetRightExpiratoryResistanceMultiplier().SetValue(1.0);
  if (!HasRightInspiratoryResistanceMultiplier())
    GetRightInspiratoryResistanceMultiplier().SetValue(1.0);
  if (!HasUpperExpiratoryResistanceMultiplier())
    GetUpperExpiratoryResistanceMultiplier().SetValue(1.0);
  if (!HasUpperInspiratoryResistanceMultiplier())
    GetUpperInspiratoryResistanceMultiplier().SetValue(1.0);
  if (!HasRespirationRateMultiplier())
    GetRespirationRateMultiplier().SetValue(1.0);
  if (!HasTidalVolumeMultiplier())
    GetTidalVolumeMultiplier().SetValue(1.0);
}

bool SERespiratoryMechanicsModifiers::IsActive() const
{
  if (GetLeftComplianceMultiplier() != 1.0)
    return true;
  if (GetRightComplianceMultiplier() != 1.0)
    return true;
  if (GetLeftExpiratoryResistanceMultiplier() != 1.0)
    return true;
  if (GetLeftInspiratoryResistanceMultiplier() != 1.0)
    return true;
  if (GetRightExpiratoryResistanceMultiplier() != 1.0)
    return true;
  if (GetRightInspiratoryResistanceMultiplier() != 1.0)
    return true;
  if (GetUpperExpiratoryResistanceMultiplier() != 1.0)
    return true;
  if (GetUpperInspiratoryResistanceMultiplier() != 1.0)
    return true;
  if (GetRespirationRateMultiplier() != 1.0)
    return true;
  if (GetTidalVolumeMultiplier() != 1.0)
    return true;

  for (auto itr : m_VolumeIncrements)
    if (itr.second != nullptr && itr.second->IsPositive())
      return true;

  return false;
}

bool SERespiratoryMechanicsModifiers::SerializeToString(std::string& output, eSerializationFormat m) const
{
  return PBPhysiology::SerializeToString(*this, output, m);
}
bool SERespiratoryMechanicsModifiers::SerializeToFile(const std::string& filename) const
{
  return PBPhysiology::SerializeToFile(*this, filename);
}
bool SERespiratoryMechanicsModifiers::SerializeFromString(const std::string& src, eSerializationFormat m)
{
  return PBPhysiology::SerializeFromString(src, *this, m);
}
bool SERespiratoryMechanicsModifiers::SerializeFromFile(const std::string& filename)
{
  return PBPhysiology::SerializeFromFile(filename, *this);
}

const SEScalar* SERespiratoryMechanicsModifiers::GetScalar(const std::string& name)
{
  if (name.compare("LeftComplianceMultiplier") == 0)
    return &GetLeftComplianceMultiplier();
  if (name.compare("RightComplianceMultiplier") == 0)
    return &GetRightComplianceMultiplier();
  if (name.compare("LeftExpiratoryResistanceMultiplier") == 0)
    return &GetLeftExpiratoryResistanceMultiplier();
  if (name.compare("LeftInspiratoryResistanceMultiplier") == 0)
    return &GetLeftInspiratoryResistanceMultiplier();
  if (name.compare("RightExpiratoryResistanceMultiplier") == 0)
    return &GetRightExpiratoryResistanceMultiplier();
  if (name.compare("RightInspiratoryResistanceMultiplier") == 0)
    return &GetRightInspiratoryResistanceMultiplier();
  if (name.compare("UpperExpiratoryResistanceMultiplier") == 0)
    return &GetUpperExpiratoryResistanceMultiplier();
  if (name.compare("UpperInspiratoryResistanceMultiplier") == 0)
    return &GetUpperInspiratoryResistanceMultiplier();
  if (name.compare("RespirationRateMultiplier") == 0)
    return &GetRespirationRateMultiplier();
  if (name.compare("TidalVolumeMultiplier") == 0)
    return &GetTidalVolumeMultiplier();

  return nullptr;
}

bool SERespiratoryMechanicsModifiers::HasLeftComplianceMultiplier() const
{
  return m_LeftComplianceMultiplier == nullptr ? false : m_LeftComplianceMultiplier->IsValid();
}
SEScalarUnsigned& SERespiratoryMechanicsModifiers::GetLeftComplianceMultiplier()
{
  if (m_LeftComplianceMultiplier == nullptr)
    m_LeftComplianceMultiplier = new SEScalarUnsigned();
  return *m_LeftComplianceMultiplier;
}
double SERespiratoryMechanicsModifiers::GetLeftComplianceMultiplier() const
{
  if (m_LeftComplianceMultiplier == nullptr)
    return SEScalar::dNaN();
  return m_LeftComplianceMultiplier->GetValue();
}

bool SERespiratoryMechanicsModifiers::HasRightComplianceMultiplier() const
{
  return m_RightComplianceMultiplier == nullptr ? false : m_RightComplianceMultiplier->IsValid();
}
SEScalarUnsigned& SERespiratoryMechanicsModifiers::GetRightComplianceMultiplier()
{
  if (m_RightComplianceMultiplier == nullptr)
    m_RightComplianceMultiplier = new SEScalarUnsigned();
  return *m_RightComplianceMultiplier;
}
double SERespiratoryMechanicsModifiers::GetRightComplianceMultiplier() const
{
  if (m_RightComplianceMultiplier == nullptr)
    return SEScalar::dNaN();
  return m_RightComplianceMultiplier->GetValue();
}

bool SERespiratoryMechanicsModifiers::HasLeftExpiratoryResistanceMultiplier() const
{
  return m_LeftExpiratoryResistanceMultiplier == nullptr ? false : m_LeftExpiratoryResistanceMultiplier->IsValid();
}
SEScalarUnsigned& SERespiratoryMechanicsModifiers::GetLeftExpiratoryResistanceMultiplier()
{
  if (m_LeftExpiratoryResistanceMultiplier == nullptr)
    m_LeftExpiratoryResistanceMultiplier = new SEScalarUnsigned();
  return *m_LeftExpiratoryResistanceMultiplier;
}
double SERespiratoryMechanicsModifiers::GetLeftExpiratoryResistanceMultiplier() const
{
  if (m_LeftExpiratoryResistanceMultiplier == nullptr)
    return SEScalar::dNaN();
  return m_LeftExpiratoryResistanceMultiplier->GetValue();
}

bool SERespiratoryMechanicsModifiers::HasLeftInspiratoryResistanceMultiplier() const
{
  return m_LeftInspiratoryResistanceMultiplier == nullptr ? false : m_LeftInspiratoryResistanceMultiplier->IsValid();
}
SEScalarUnsigned& SERespiratoryMechanicsModifiers::GetLeftInspiratoryResistanceMultiplier()
{
  if (m_LeftInspiratoryResistanceMultiplier == nullptr)
    m_LeftInspiratoryResistanceMultiplier = new SEScalarUnsigned();
  return *m_LeftInspiratoryResistanceMultiplier;
}
double SERespiratoryMechanicsModifiers::GetLeftInspiratoryResistanceMultiplier() const
{
  if (m_LeftInspiratoryResistanceMultiplier == nullptr)
    return SEScalar::dNaN();
  return m_LeftInspiratoryResistanceMultiplier->GetValue();
}

bool SERespiratoryMechanicsModifiers::HasRightExpiratoryResistanceMultiplier() const
{
  return m_RightExpiratoryResistanceMultiplier == nullptr ? false : m_RightExpiratoryResistanceMultiplier->IsValid();
}
SEScalarUnsigned& SERespiratoryMechanicsModifiers::GetRightExpiratoryResistanceMultiplier()
{
  if (m_RightExpiratoryResistanceMultiplier == nullptr)
    m_RightExpiratoryResistanceMultiplier = new SEScalarUnsigned();
  return *m_RightExpiratoryResistanceMultiplier;
}
double SERespiratoryMechanicsModifiers::GetRightExpiratoryResistanceMultiplier() const
{
  if (m_RightExpiratoryResistanceMultiplier == nullptr)
    return SEScalar::dNaN();
  return m_RightExpiratoryResistanceMultiplier->GetValue();
}

bool SERespiratoryMechanicsModifiers::HasRightInspiratoryResistanceMultiplier() const
{
  return m_RightInspiratoryResistanceMultiplier == nullptr ? false : m_RightInspiratoryResistanceMultiplier->IsValid();
}
SEScalarUnsigned& SERespiratoryMechanicsModifiers::GetRightInspiratoryResistanceMultiplier()
{
  if (m_RightInspiratoryResistanceMultiplier == nullptr)
    m_RightInspiratoryResistanceMultiplier = new SEScalarUnsigned();
  return *m_RightInspiratoryResistanceMultiplier;
}
double SERespiratoryMechanicsModifiers::GetRightInspiratoryResistanceMultiplier() const
{
  if (m_RightInspiratoryResistanceMultiplier == nullptr)
    return SEScalar::dNaN();
  return m_RightInspiratoryResistanceMultiplier->GetValue();
}

bool SERespiratoryMechanicsModifiers::HasUpperExpiratoryResistanceMultiplier() const
{
  return m_UpperExpiratoryResistanceMultiplier == nullptr ? false : m_UpperExpiratoryResistanceMultiplier->IsValid();
}
SEScalarUnsigned& SERespiratoryMechanicsModifiers::GetUpperExpiratoryResistanceMultiplier()
{
  if (m_UpperExpiratoryResistanceMultiplier == nullptr)
    m_UpperExpiratoryResistanceMultiplier = new SEScalarUnsigned();
  return *m_UpperExpiratoryResistanceMultiplier;
}
double SERespiratoryMechanicsModifiers::GetUpperExpiratoryResistanceMultiplier() const
{
  if (m_UpperExpiratoryResistanceMultiplier == nullptr)
    return SEScalar::dNaN();
  return m_UpperExpiratoryResistanceMultiplier->GetValue();
}

bool SERespiratoryMechanicsModifiers::HasUpperInspiratoryResistanceMultiplier() const
{
  return m_UpperInspiratoryResistanceMultiplier == nullptr ? false : m_UpperInspiratoryResistanceMultiplier->IsValid();
}
SEScalarUnsigned& SERespiratoryMechanicsModifiers::GetUpperInspiratoryResistanceMultiplier()
{
  if (m_UpperInspiratoryResistanceMultiplier == nullptr)
    m_UpperInspiratoryResistanceMultiplier = new SEScalarUnsigned();
  return *m_UpperInspiratoryResistanceMultiplier;
}
double SERespiratoryMechanicsModifiers::GetUpperInspiratoryResistanceMultiplier() const
{
  if (m_UpperInspiratoryResistanceMultiplier == nullptr)
    return SEScalar::dNaN();
  return m_UpperInspiratoryResistanceMultiplier->GetValue();
}

bool SERespiratoryMechanicsModifiers::HasRespirationRateMultiplier() const
{
  return m_RespirationRateMultiplier == nullptr ? false : m_RespirationRateMultiplier->IsValid();
}
SEScalarUnsigned& SERespiratoryMechanicsModifiers::GetRespirationRateMultiplier()
{
  if (m_RespirationRateMultiplier == nullptr)
    m_RespirationRateMultiplier = new SEScalarUnsigned();
  return *m_RespirationRateMultiplier;
}
double SERespiratoryMechanicsModifiers::GetRespirationRateMultiplier() const
{
  if (m_RespirationRateMultiplier == nullptr)
    return SEScalar::dNaN();
  return m_RespirationRateMultiplier->GetValue();
}

bool SERespiratoryMechanicsModifiers::HasTidalVolumeMultiplier() const
{
  return m_TidalVolumeMultiplier == nullptr ? false : m_TidalVolumeMultiplier->IsValid();
}
SEScalarUnsigned& SERespiratoryMechanicsModifiers::GetTidalVolumeMultiplier()
{
  if (m_TidalVolumeMultiplier == nullptr)
    m_TidalVolumeMultiplier = new SEScalarUnsigned();
  return *m_TidalVolumeMultiplier;
}
double SERespiratoryMechanicsModifiers::GetTidalVolumeMultiplier() const
{
  if (m_TidalVolumeMultiplier == nullptr)
    return SEScalar::dNaN();
  return m_TidalVolumeMultiplier->GetValue();
}

bool SERespiratoryMechanicsModifiers::HasLungVolumeIncrement() const
{
  for (auto itr : m_VolumeIncrements)
    if (itr.second != nullptr && itr.second->IsValid())
      return true;
  return false;
}
LungVolumeIncrementMap& SERespiratoryMechanicsModifiers::GetLungVolumeIncrements()
{
  return m_VolumeIncrements;
}
const LungVolumeIncrementMap& SERespiratoryMechanicsModifiers::GetLungVolumeIncrements() const
{
  return m_VolumeIncrements;
}

bool SERespiratoryMechanicsModifiers::HasLungVolumeIncrement(eLungCompartment cmpt) const
{
  auto s = m_VolumeIncrements.find(cmpt);
  if (s == m_VolumeIncrements.end())
    return false;
  if (s->second == nullptr)
    return false;
  return s->second->IsValid();
}
SEScalarVolume& SERespiratoryMechanicsModifiers::GetLungVolumeIncrement(eLungCompartment cmpt)
{
  SEScalarVolume* s = m_VolumeIncrements[cmpt];
  if (s == nullptr)
  {
    s = new SEScalarVolume();
    m_VolumeIncrements[cmpt] = s;
  }
  return *s;
}
double SERespiratoryMechanicsModifiers::GetLungVolumeIncrement(eLungCompartment cmpt, const VolumeUnit& unit) const
{
  auto s = m_VolumeIncrements.find(cmpt);
  if (s == m_VolumeIncrements.end())
    return SEScalar::dNaN();
  return s->second->GetValue(unit);
}

std::string SERespiratoryMechanicsModifiers::ToString() const
{
  std::string str = "Respiratory Mechanics Modifiers";
  str += "\n\tLeftComplianceMultiplier: "; HasLeftComplianceMultiplier() ? str += m_LeftComplianceMultiplier->ToString() : str += "Not Set";
  str += "\n\tRightComplianceMultiplier: "; HasRightComplianceMultiplier() ? str += m_RightComplianceMultiplier->ToString() : str += "Not Set";
  str += "\n\tLeftExpiratoryResistanceMultiplier: "; HasLeftExpiratoryResistanceMultiplier() ? str += m_LeftExpiratoryResistanceMultiplier->ToString() : str += "Not Set";
  str += "\n\tLeftInspiratoryResistanceMultiplier: "; HasLeftInspiratoryResistanceMultiplier() ? str += m_LeftInspiratoryResistanceMultiplier->ToString() : str += "Not Set";
  str += "\n\tRightExpiratoryResistanceMultiplier: "; HasRightExpiratoryResistanceMultiplier() ? str += m_RightExpiratoryResistanceMultiplier->ToString() : str += "Not Set";
  str += "\n\tRightInspiratoryResistanceMultiplier: "; HasRightInspiratoryResistanceMultiplier() ? str += m_RightInspiratoryResistanceMultiplier->ToString() : str += "Not Set";
  str += "\n\tUpperExpiratoryResistanceMultiplier: "; HasUpperExpiratoryResistanceMultiplier() ? str += m_UpperExpiratoryResistanceMultiplier->ToString() : str += "Not Set";
  str += "\n\tUpperInspiratoryResistanceMultiplier: "; HasUpperInspiratoryResistanceMultiplier() ? str += m_UpperInspiratoryResistanceMultiplier->ToString() : str += "Not Set";
  str += "\n\tRespirationRateMultiplier: "; HasRespirationRateMultiplier() ? str += m_RespirationRateMultiplier->ToString() : str += "Not Set";
  str += "\n\tTidalVolumeMultiplier: "; HasTidalVolumeMultiplier() ? str += m_TidalVolumeMultiplier->ToString() : str += "Not Set";

  for (auto itr : m_VolumeIncrements)
    str += "\n\t" + eLungCompartment_Name(itr.first) + " Volume: " + itr.second->ToString();

  return str;
}
void SERespiratoryMechanicsModifiers::ToString(std::ostream& str) const
{
  str << ToString();
}
