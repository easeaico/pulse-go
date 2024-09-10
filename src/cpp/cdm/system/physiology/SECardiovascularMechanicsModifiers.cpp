/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/system/physiology/SECardiovascularMechanicsModifiers.h"
#include "cdm/io/protobuf/PBProperties.h"
#include "cdm/io/protobuf/PBPhysiology.h"
#include "cdm/patient/actions/SECardiovascularMechanicsModification.h"
#include "cdm/properties/SEScalarUnsigned.h"

SECardiovascularMechanicsModifiers::SECardiovascularMechanicsModifiers(Logger* logger) : Loggable(logger)
{
  m_ArterialComplianceMultiplier = nullptr;
  m_ArterialResistanceMultiplier = nullptr;
  m_PulmonaryComplianceMultiplier = nullptr;
  m_PulmonaryResistanceMultiplier = nullptr;
  m_SystemicResistanceMultiplier = nullptr;
  m_SystemicComplianceMultiplier = nullptr;
  m_VenousComplianceMultiplier = nullptr;
  m_VenousResistanceMultiplier = nullptr;

  m_HeartRateMultiplier = nullptr;
  m_StrokeVolumeMultiplier = nullptr;
}

SECardiovascularMechanicsModifiers::~SECardiovascularMechanicsModifiers()
{
  SAFE_DELETE(m_ArterialComplianceMultiplier);
  SAFE_DELETE(m_ArterialResistanceMultiplier);
  SAFE_DELETE(m_PulmonaryComplianceMultiplier);
  SAFE_DELETE(m_PulmonaryResistanceMultiplier);
  SAFE_DELETE(m_SystemicResistanceMultiplier);
  SAFE_DELETE(m_SystemicComplianceMultiplier);
  SAFE_DELETE(m_VenousComplianceMultiplier);
  SAFE_DELETE(m_VenousResistanceMultiplier);

  SAFE_DELETE(m_HeartRateMultiplier);
  SAFE_DELETE(m_StrokeVolumeMultiplier);
}

void SECardiovascularMechanicsModifiers::Clear()
{
  INVALIDATE_PROPERTY(m_ArterialComplianceMultiplier);
  INVALIDATE_PROPERTY(m_ArterialResistanceMultiplier);
  INVALIDATE_PROPERTY(m_PulmonaryComplianceMultiplier);
  INVALIDATE_PROPERTY(m_PulmonaryResistanceMultiplier);
  INVALIDATE_PROPERTY(m_SystemicResistanceMultiplier);
  INVALIDATE_PROPERTY(m_SystemicComplianceMultiplier);
  INVALIDATE_PROPERTY(m_VenousComplianceMultiplier);
  INVALIDATE_PROPERTY(m_VenousResistanceMultiplier);

  INVALIDATE_PROPERTY(m_HeartRateMultiplier);
  INVALIDATE_PROPERTY(m_StrokeVolumeMultiplier);
}

// We want our models to have the assumption that all modifiers are present
// So if we don't have anything, just use 1, as this would be a no op in code
void SECardiovascularMechanicsModifiers::Activate()
{
  if (!HasArterialComplianceMultiplier())
    GetArterialComplianceMultiplier().SetValue(1.0);
  if (!HasArterialResistanceMultiplier())
    GetArterialResistanceMultiplier().SetValue(1.0);
  if (!HasPulmonaryComplianceMultiplier())
    GetPulmonaryComplianceMultiplier().SetValue(1.0);
  if (!HasPulmonaryResistanceMultiplier())
    GetPulmonaryResistanceMultiplier().SetValue(1.0);
  if (!HasSystemicResistanceMultiplier())
    GetSystemicResistanceMultiplier().SetValue(1.0);
  if (!HasSystemicComplianceMultiplier())
    GetSystemicComplianceMultiplier().SetValue(1.0);
  if (!HasVenousComplianceMultiplier())
    GetVenousComplianceMultiplier().SetValue(1.0);
  if (!HasVenousResistanceMultiplier())
    GetVenousResistanceMultiplier().SetValue(1.0);
  if (!HasHeartRateMultiplier())
    GetHeartRateMultiplier().SetValue(1.0);
  if (!HasStrokeVolumeMultiplier())
    GetStrokeVolumeMultiplier().SetValue(1.0);
}
bool SECardiovascularMechanicsModifiers::IsActive() const
{
  if (GetArterialComplianceMultiplier() != 1.0)
    return true;
  if (GetArterialResistanceMultiplier() != 1.0)
    return true;
  if (GetPulmonaryComplianceMultiplier() != 1.0)
    return true;
  if (GetPulmonaryResistanceMultiplier() != 1.0)
    return true;
  if (GetSystemicResistanceMultiplier() != 1.0)
    return true;
  if (GetSystemicComplianceMultiplier() != 1.0)
    return true;
  if (GetVenousComplianceMultiplier() != 1.0)
    return true;
  if (GetVenousResistanceMultiplier() != 1.0)
    return true;
  if (GetHeartRateMultiplier() != 1.0)
    return true;
  if (GetStrokeVolumeMultiplier() != 1.0)
    return true;
  return false;
}

bool SECardiovascularMechanicsModifiers::SerializeToString(std::string& output, eSerializationFormat m) const
{
  return PBPhysiology::SerializeToString(*this, output, m);
}
bool SECardiovascularMechanicsModifiers::SerializeToFile(const std::string& filename) const
{
  return PBPhysiology::SerializeToFile(*this, filename);
}
bool SECardiovascularMechanicsModifiers::SerializeFromString(const std::string& src, eSerializationFormat m)
{
  return PBPhysiology::SerializeFromString(src, *this, m);
}
bool SECardiovascularMechanicsModifiers::SerializeFromFile(const std::string& filename)
{
  return PBPhysiology::SerializeFromFile(filename, *this);
}

const SEScalar* SECardiovascularMechanicsModifiers::GetScalar(const std::string& name)
{
  if (name.compare("ArterialComplianceMultiplier") == 0)
    return &GetArterialComplianceMultiplier();
  if (name.compare("ArterialResistanceMultiplier") == 0)
    return &GetArterialResistanceMultiplier();
  if (name.compare("PulmonaryComplianceMultiplier") == 0)
    return &GetPulmonaryComplianceMultiplier();
  if (name.compare("PulmonaryResistanceMultiplier") == 0)
    return &GetPulmonaryResistanceMultiplier();
  if (name.compare("SystemicResistanceMultiplier") == 0)
    return &GetSystemicResistanceMultiplier();
  if (name.compare("SystemicComplianceMultiplier") == 0)
    return &GetSystemicComplianceMultiplier();
  if (name.compare("VenousComplianceMultiplier") == 0)
    return &GetVenousComplianceMultiplier();
  if (name.compare("VenousResistanceMultiplier") == 0)
    return &GetVenousResistanceMultiplier();

  if (name.compare("HeartRateMultiplier") == 0)
    return &GetHeartRateMultiplier();
  if (name.compare("StrokeVolumeMultiplier") == 0)
    return &GetStrokeVolumeMultiplier();

  return nullptr;
}

bool SECardiovascularMechanicsModifiers::HasArterialComplianceMultiplier() const
{
  return m_ArterialComplianceMultiplier == nullptr ? false : m_ArterialComplianceMultiplier->IsValid();
}
SEScalarUnsigned& SECardiovascularMechanicsModifiers::GetArterialComplianceMultiplier()
{
  if (m_ArterialComplianceMultiplier == nullptr)
    m_ArterialComplianceMultiplier = new SEScalarUnsigned();
  return *m_ArterialComplianceMultiplier;
}
double SECardiovascularMechanicsModifiers::GetArterialComplianceMultiplier() const
{
  if (m_ArterialComplianceMultiplier == nullptr)
    return SEScalar::dNaN();
  return m_ArterialComplianceMultiplier->GetValue();
}

bool SECardiovascularMechanicsModifiers::HasArterialResistanceMultiplier() const
{
  return m_ArterialResistanceMultiplier == nullptr ? false : m_ArterialResistanceMultiplier->IsValid();
}
SEScalarUnsigned& SECardiovascularMechanicsModifiers::GetArterialResistanceMultiplier()
{
  if (m_ArterialResistanceMultiplier == nullptr)
    m_ArterialResistanceMultiplier = new SEScalarUnsigned();
  return *m_ArterialResistanceMultiplier;
}
double SECardiovascularMechanicsModifiers::GetArterialResistanceMultiplier() const
{
  if (m_ArterialResistanceMultiplier == nullptr)
    return SEScalar::dNaN();
  return m_ArterialResistanceMultiplier->GetValue();
}

bool SECardiovascularMechanicsModifiers::HasPulmonaryComplianceMultiplier() const
{
  return m_PulmonaryComplianceMultiplier == nullptr ? false : m_PulmonaryComplianceMultiplier->IsValid();
}
SEScalarUnsigned& SECardiovascularMechanicsModifiers::GetPulmonaryComplianceMultiplier()
{
  if (m_PulmonaryComplianceMultiplier == nullptr)
    m_PulmonaryComplianceMultiplier = new SEScalarUnsigned();
  return *m_PulmonaryComplianceMultiplier;
}
double SECardiovascularMechanicsModifiers::GetPulmonaryComplianceMultiplier() const
{
  if (m_PulmonaryComplianceMultiplier == nullptr)
    return SEScalar::dNaN();
  return m_PulmonaryComplianceMultiplier->GetValue();
}

bool SECardiovascularMechanicsModifiers::HasPulmonaryResistanceMultiplier() const
{
  return m_PulmonaryResistanceMultiplier == nullptr ? false : m_PulmonaryResistanceMultiplier->IsValid();
}
SEScalarUnsigned& SECardiovascularMechanicsModifiers::GetPulmonaryResistanceMultiplier()
{
  if (m_PulmonaryResistanceMultiplier == nullptr)
    m_PulmonaryResistanceMultiplier = new SEScalarUnsigned();
  return *m_PulmonaryResistanceMultiplier;
}
double SECardiovascularMechanicsModifiers::GetPulmonaryResistanceMultiplier() const
{
  if (m_PulmonaryResistanceMultiplier == nullptr)
    return SEScalar::dNaN();
  return m_PulmonaryResistanceMultiplier->GetValue();
}

bool SECardiovascularMechanicsModifiers::HasSystemicResistanceMultiplier() const
{
  return m_SystemicResistanceMultiplier == nullptr ? false : m_SystemicResistanceMultiplier->IsValid();
}
SEScalarUnsigned& SECardiovascularMechanicsModifiers::GetSystemicResistanceMultiplier()
{
  if (m_SystemicResistanceMultiplier == nullptr)
    m_SystemicResistanceMultiplier = new SEScalarUnsigned();
  return *m_SystemicResistanceMultiplier;
}
double SECardiovascularMechanicsModifiers::GetSystemicResistanceMultiplier() const
{
  if (m_SystemicResistanceMultiplier == nullptr)
    return SEScalar::dNaN();
  return m_SystemicResistanceMultiplier->GetValue();
}

bool SECardiovascularMechanicsModifiers::HasSystemicComplianceMultiplier() const
{
  return m_SystemicComplianceMultiplier == nullptr ? false : m_SystemicComplianceMultiplier->IsValid();
}
SEScalarUnsigned& SECardiovascularMechanicsModifiers::GetSystemicComplianceMultiplier()
{
  if (m_SystemicComplianceMultiplier == nullptr)
    m_SystemicComplianceMultiplier = new SEScalarUnsigned();
  return *m_SystemicComplianceMultiplier;
}
double SECardiovascularMechanicsModifiers::GetSystemicComplianceMultiplier() const
{
  if (m_SystemicComplianceMultiplier == nullptr)
    return SEScalar::dNaN();
  return m_SystemicComplianceMultiplier->GetValue();
}

bool SECardiovascularMechanicsModifiers::HasVenousComplianceMultiplier() const
{
  return m_VenousComplianceMultiplier == nullptr ? false : m_VenousComplianceMultiplier->IsValid();
}
SEScalarUnsigned& SECardiovascularMechanicsModifiers::GetVenousComplianceMultiplier()
{
  if (m_VenousComplianceMultiplier == nullptr)
    m_VenousComplianceMultiplier = new SEScalarUnsigned();
  return *m_VenousComplianceMultiplier;
}
double SECardiovascularMechanicsModifiers::GetVenousComplianceMultiplier() const
{
  if (m_VenousComplianceMultiplier == nullptr)
    return SEScalar::dNaN();
  return m_VenousComplianceMultiplier->GetValue();
}

bool SECardiovascularMechanicsModifiers::HasVenousResistanceMultiplier() const
{
  return m_VenousResistanceMultiplier == nullptr ? false : m_VenousResistanceMultiplier->IsValid();
}
SEScalarUnsigned& SECardiovascularMechanicsModifiers::GetVenousResistanceMultiplier()
{
  if (m_VenousResistanceMultiplier == nullptr)
    m_VenousResistanceMultiplier = new SEScalarUnsigned();
  return *m_VenousResistanceMultiplier;
}
double SECardiovascularMechanicsModifiers::GetVenousResistanceMultiplier() const
{
  if (m_VenousResistanceMultiplier == nullptr)
    return SEScalar::dNaN();
  return m_VenousResistanceMultiplier->GetValue();
}

bool SECardiovascularMechanicsModifiers::HasHeartRateMultiplier() const
{
  return m_HeartRateMultiplier == nullptr ? false : m_HeartRateMultiplier->IsValid();
}
SEScalarUnsigned& SECardiovascularMechanicsModifiers::GetHeartRateMultiplier()
{
  if (m_HeartRateMultiplier == nullptr)
    m_HeartRateMultiplier = new SEScalarUnsigned();
  return *m_HeartRateMultiplier;
}
double SECardiovascularMechanicsModifiers::GetHeartRateMultiplier() const
{
  if (m_HeartRateMultiplier == nullptr)
    return SEScalar::dNaN();
  return m_HeartRateMultiplier->GetValue();
}

bool SECardiovascularMechanicsModifiers::HasStrokeVolumeMultiplier() const
{
  return m_StrokeVolumeMultiplier == nullptr ? false : m_StrokeVolumeMultiplier->IsValid();
}
SEScalarUnsigned& SECardiovascularMechanicsModifiers::GetStrokeVolumeMultiplier()
{
  if (m_StrokeVolumeMultiplier == nullptr)
    m_StrokeVolumeMultiplier = new SEScalarUnsigned();
  return *m_StrokeVolumeMultiplier;
}
double SECardiovascularMechanicsModifiers::GetStrokeVolumeMultiplier() const
{
  if (m_StrokeVolumeMultiplier == nullptr)
    return SEScalar::dNaN();
  return m_StrokeVolumeMultiplier->GetValue();
}


std::string SECardiovascularMechanicsModifiers::ToString() const
{
  std::string str = "Cardiovascular Mechanics Modifiers";
  str += "\n\tArterialComplianceMultiplier: "; HasArterialComplianceMultiplier() ? str += m_ArterialComplianceMultiplier->ToString() : str += "Not Set";
  str += "\n\tArterialResistanceMultiplier: "; HasArterialResistanceMultiplier() ? str += m_ArterialResistanceMultiplier->ToString() : str += "Not Set";
  str += "\n\tPulmonaryComplianceMultiplier: "; HasPulmonaryComplianceMultiplier() ? str += m_PulmonaryComplianceMultiplier->ToString() : str += "Not Set";
  str += "\n\tPulmonaryResistanceMultiplier: "; HasPulmonaryResistanceMultiplier() ? str += m_PulmonaryResistanceMultiplier->ToString() : str += "Not Set";
  str += "\n\tSystemicResistanceMultiplier: "; HasSystemicResistanceMultiplier() ? str += m_SystemicResistanceMultiplier->ToString() : str += "Not Set";
  str += "\n\tSystemicComplianceMultiplier: "; HasSystemicComplianceMultiplier() ? str += m_SystemicComplianceMultiplier->ToString() : str += "Not Set";
  str += "\n\tVenousComplianceMultiplier: "; HasVenousComplianceMultiplier() ? str += m_VenousComplianceMultiplier->ToString() : str += "Not Set";
  str += "\n\tVenousResistanceMultiplier: "; HasVenousResistanceMultiplier() ? str += m_VenousResistanceMultiplier->ToString() : str += "Not Set";
  str += "\n\tHeartRateMultiplier: "; HasHeartRateMultiplier() ? str += m_HeartRateMultiplier->ToString() : str += "Not Set";
  str += "\n\tStrokeVolumeMultiplier: "; HasStrokeVolumeMultiplier() ? str += m_StrokeVolumeMultiplier->ToString() : str += "Not Set";
  return str;
}
void SECardiovascularMechanicsModifiers::ToString(std::ostream& str) const
{
  str << ToString();
}
