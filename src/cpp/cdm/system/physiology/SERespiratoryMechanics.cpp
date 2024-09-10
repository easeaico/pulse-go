/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/system/physiology/SERespiratoryMechanics.h"
#include "cdm/io/protobuf/PBProperties.h"
#include "cdm/io/protobuf/PBPhysiology.h"
#include "cdm/patient/actions/SERespiratoryMechanicsConfiguration.h"

#include "cdm/properties/SECurve.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarPressureTimePerVolume.h"
#include "cdm/properties/SEScalarTime.h"

SERespiratoryMechanics::SERespiratoryMechanics(Logger* logger) : Loggable(logger)
{
  m_Active = eSwitch::NullSwitch;
  m_DefaultType = eDefaultType::Model;
  m_LeftComplianceCurve = nullptr;
  m_RightComplianceCurve = nullptr;
  m_LeftExpiratoryResistance = nullptr;
  m_LeftInspiratoryResistance = nullptr;
  m_RightExpiratoryResistance = nullptr;
  m_RightInspiratoryResistance = nullptr;
  m_UpperExpiratoryResistance = nullptr;
  m_UpperInspiratoryResistance = nullptr;
  m_InspiratoryPeakPressure = nullptr;
  m_ExpiratoryPeakPressure = nullptr;
  m_InspiratoryRiseTime = nullptr;
  m_InspiratoryHoldTime = nullptr;
  m_InspiratoryReleaseTime = nullptr;
  m_InspiratoryToExpiratoryPauseTime = nullptr;
  m_ExpiratoryRiseTime = nullptr;
  m_ExpiratoryHoldTime = nullptr;
  m_ExpiratoryReleaseTime = nullptr;
  m_ResidueTime = nullptr;
}

SERespiratoryMechanics::~SERespiratoryMechanics()
{
  SAFE_DELETE(m_LeftComplianceCurve);
  SAFE_DELETE(m_RightComplianceCurve);
  SAFE_DELETE(m_LeftExpiratoryResistance);
  SAFE_DELETE(m_LeftInspiratoryResistance);
  SAFE_DELETE(m_RightExpiratoryResistance);
  SAFE_DELETE(m_RightInspiratoryResistance);
  SAFE_DELETE(m_UpperExpiratoryResistance);
  SAFE_DELETE(m_UpperInspiratoryResistance);
  SAFE_DELETE(m_InspiratoryPeakPressure);
  SAFE_DELETE(m_ExpiratoryPeakPressure);
  SAFE_DELETE(m_InspiratoryRiseTime);
  SAFE_DELETE(m_InspiratoryHoldTime);
  SAFE_DELETE(m_InspiratoryReleaseTime);
  SAFE_DELETE(m_InspiratoryToExpiratoryPauseTime);
  SAFE_DELETE(m_ExpiratoryRiseTime);
  SAFE_DELETE(m_ExpiratoryHoldTime);
  SAFE_DELETE(m_ExpiratoryReleaseTime);
  SAFE_DELETE(m_ResidueTime);
}

void SERespiratoryMechanics::Clear()
{
  m_Active = eSwitch::NullSwitch;
  m_DefaultType = eDefaultType::Model;
  INVALIDATE_PROPERTY(m_LeftComplianceCurve);
  INVALIDATE_PROPERTY(m_RightComplianceCurve);
  INVALIDATE_PROPERTY(m_LeftExpiratoryResistance);
  INVALIDATE_PROPERTY(m_LeftInspiratoryResistance);
  INVALIDATE_PROPERTY(m_RightExpiratoryResistance);
  INVALIDATE_PROPERTY(m_RightInspiratoryResistance);
  INVALIDATE_PROPERTY(m_UpperExpiratoryResistance);
  INVALIDATE_PROPERTY(m_UpperInspiratoryResistance);
  INVALIDATE_PROPERTY(m_InspiratoryPeakPressure);
  INVALIDATE_PROPERTY(m_ExpiratoryPeakPressure);
  INVALIDATE_PROPERTY(m_InspiratoryRiseTime);
  INVALIDATE_PROPERTY(m_InspiratoryHoldTime);
  INVALIDATE_PROPERTY(m_InspiratoryReleaseTime);
  INVALIDATE_PROPERTY(m_InspiratoryToExpiratoryPauseTime);
  INVALIDATE_PROPERTY(m_ExpiratoryRiseTime);
  INVALIDATE_PROPERTY(m_ExpiratoryHoldTime);
  INVALIDATE_PROPERTY(m_ExpiratoryReleaseTime);
  INVALIDATE_PROPERTY(m_ResidueTime);
}

bool SERespiratoryMechanics::SerializeToString(std::string& output, eSerializationFormat m) const
{
  return PBPhysiology::SerializeToString(*this, output, m);
}
bool SERespiratoryMechanics::SerializeToFile(const std::string& filename) const
{
  return PBPhysiology::SerializeToFile(*this, filename);
}
bool SERespiratoryMechanics::SerializeFromString(const std::string& src, eSerializationFormat m)
{
  return PBPhysiology::SerializeFromString(src, *this, m);
}
bool SERespiratoryMechanics::SerializeFromFile(const std::string& filename)
{
  return PBPhysiology::SerializeFromFile(filename, *this);
}

void SERespiratoryMechanics::ProcessConfiguration(SERespiratoryMechanicsConfiguration& config)
{
  if (config.GetMergeType() == eMergeType::Replace)
    Clear();
  if (config.HasSettings())
    Merge(config.GetSettings());
  else if (config.HasSettingsFile())
  {
    // Update the action with the file contents
    std::string cfg_file = config.GetSettingsFile();
    if (!config.GetSettings().SerializeFromFile(cfg_file))
      Error("Unable to load configuration file");
    Merge(config.GetSettings());
  }
}
void SERespiratoryMechanics::Merge(const SERespiratoryMechanics& from)
{
  if (from.m_Active != eSwitch::NullSwitch)
    SetActive(from.m_Active);
  m_DefaultType = from.m_DefaultType;

  if (from.HasLeftComplianceCurve())
    GetLeftComplianceCurve().Copy(*from.m_LeftComplianceCurve);
  if (from.HasRightComplianceCurve())
    GetRightComplianceCurve().Copy(*from.m_RightComplianceCurve);

  COPY_PROPERTY(LeftExpiratoryResistance);
  COPY_PROPERTY(LeftInspiratoryResistance);
  COPY_PROPERTY(RightExpiratoryResistance);
  COPY_PROPERTY(RightInspiratoryResistance);
  COPY_PROPERTY(UpperExpiratoryResistance);
  COPY_PROPERTY(UpperInspiratoryResistance);
  COPY_PROPERTY(InspiratoryPeakPressure);
  COPY_PROPERTY(ExpiratoryPeakPressure);
  COPY_PROPERTY(InspiratoryRiseTime);
  COPY_PROPERTY(InspiratoryHoldTime);
  COPY_PROPERTY(InspiratoryReleaseTime);
  COPY_PROPERTY(InspiratoryToExpiratoryPauseTime);
  COPY_PROPERTY(ExpiratoryRiseTime);
  COPY_PROPERTY(ExpiratoryHoldTime);
  COPY_PROPERTY(ExpiratoryReleaseTime);
  COPY_PROPERTY(ResidueTime);

  //std::cout << this->ToString() << std::endl;
}

const SEScalar* SERespiratoryMechanics::GetScalar(const std::string& name)
{
  if (name.compare("LeftExpiratoryResistance") == 0)
    return &GetLeftExpiratoryResistance();
  if (name.compare("LeftInspiratoryResistance") == 0)
    return &GetLeftInspiratoryResistance();
  if (name.compare("RightExpiratoryResistance") == 0)
    return &GetRightExpiratoryResistance();
  if (name.compare("RightInspiratoryResistance") == 0)
    return &GetRightInspiratoryResistance();
  if (name.compare("UpperExpiratoryResistance") == 0)
    return &GetUpperExpiratoryResistance();
  if (name.compare("UpperInspiratoryResistance") == 0)
    return &GetUpperInspiratoryResistance();
  if (name.compare("InspiratoryPeakPressure") == 0)
    return &GetInspiratoryPeakPressure();
  if (name.compare("ExpiratoryPeakPressure") == 0)
    return &GetExpiratoryPeakPressure();
  if (name.compare("InspiratoryRiseTime") == 0)
    return &GetInspiratoryRiseTime();
  if (name.compare("InspiratoryHoldTime") == 0)
    return &GetInspiratoryHoldTime();
  if (name.compare("InspiratoryReleaseTime") == 0)
    return &GetInspiratoryReleaseTime();
  if (name.compare("InspiratoryToExpiratoryPauseTime") == 0)
    return &GetInspiratoryToExpiratoryPauseTime();
  if (name.compare("ExpiratoryRiseTime") == 0)
    return &GetExpiratoryRiseTime();
  if (name.compare("ExpiratoryHoldTime") == 0)
    return &GetExpiratoryHoldTime();
  if (name.compare("ExpiratoryReleaseTime") == 0)
    return &GetExpiratoryReleaseTime();
  if (name.compare("ResidueTime") == 0)
    return &GetResidueTime();

  return nullptr;
}


bool SERespiratoryMechanics::HasActive() const
{
  return m_Active!=eSwitch::NullSwitch;
}
void SERespiratoryMechanics::SetActive(eSwitch s)
{
  m_Active = s;
}
eSwitch SERespiratoryMechanics::GetActive() const
{
  return m_Active;
}

void SERespiratoryMechanics::SetDefaultType(eDefaultType m)
{
  m_DefaultType = m;
}
eDefaultType SERespiratoryMechanics::GetDefaultType() const
{
  return m_DefaultType;
}

bool SERespiratoryMechanics::HasLeftComplianceCurve() const
{
  return m_LeftComplianceCurve == nullptr ? false : m_LeftComplianceCurve->IsValid();
}
SECurve& SERespiratoryMechanics::GetLeftComplianceCurve()
{
  if (m_LeftComplianceCurve == nullptr)
    m_LeftComplianceCurve = new SECurve(GetLogger());
  return *m_LeftComplianceCurve;
}
const SECurve* SERespiratoryMechanics::GetLeftComplianceCurve() const
{
  return m_LeftComplianceCurve;
}

bool SERespiratoryMechanics::HasRightComplianceCurve() const
{
  return m_RightComplianceCurve == nullptr ? false : m_RightComplianceCurve->IsValid();
}
SECurve& SERespiratoryMechanics::GetRightComplianceCurve()
{
  if (m_RightComplianceCurve == nullptr)
    m_RightComplianceCurve = new SECurve(GetLogger());
  return *m_RightComplianceCurve;
}
const SECurve* SERespiratoryMechanics::GetRightComplianceCurve() const
{
  return m_RightComplianceCurve;
}

bool SERespiratoryMechanics::HasLeftExpiratoryResistance() const
{
  return m_LeftExpiratoryResistance == nullptr ? false : m_LeftExpiratoryResistance->IsValid();
}
SEScalarPressureTimePerVolume& SERespiratoryMechanics::GetLeftExpiratoryResistance()
{
  if (m_LeftExpiratoryResistance == nullptr)
    m_LeftExpiratoryResistance = new SEScalarPressureTimePerVolume();
  return *m_LeftExpiratoryResistance;
}
double SERespiratoryMechanics::GetLeftExpiratoryResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_LeftExpiratoryResistance == nullptr)
    return SEScalar::dNaN();
  return m_LeftExpiratoryResistance->GetValue(unit);
}

bool SERespiratoryMechanics::HasLeftInspiratoryResistance() const
{
  return m_LeftInspiratoryResistance == nullptr ? false : m_LeftInspiratoryResistance->IsValid();
}
SEScalarPressureTimePerVolume& SERespiratoryMechanics::GetLeftInspiratoryResistance()
{
  if (m_LeftInspiratoryResistance == nullptr)
    m_LeftInspiratoryResistance = new SEScalarPressureTimePerVolume();
  return *m_LeftInspiratoryResistance;
}
double SERespiratoryMechanics::GetLeftInspiratoryResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_LeftInspiratoryResistance == nullptr)
    return SEScalar::dNaN();
  return m_LeftInspiratoryResistance->GetValue(unit);
}

bool SERespiratoryMechanics::HasRightExpiratoryResistance() const
{
  return m_RightExpiratoryResistance == nullptr ? false : m_RightExpiratoryResistance->IsValid();
}
SEScalarPressureTimePerVolume& SERespiratoryMechanics::GetRightExpiratoryResistance()
{
  if (m_RightExpiratoryResistance == nullptr)
    m_RightExpiratoryResistance = new SEScalarPressureTimePerVolume();
  return *m_RightExpiratoryResistance;
}
double SERespiratoryMechanics::GetRightExpiratoryResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_RightExpiratoryResistance == nullptr)
    return SEScalar::dNaN();
  return m_RightExpiratoryResistance->GetValue(unit);
}

bool SERespiratoryMechanics::HasRightInspiratoryResistance() const
{
  return m_RightInspiratoryResistance == nullptr ? false : m_RightInspiratoryResistance->IsValid();
}
SEScalarPressureTimePerVolume& SERespiratoryMechanics::GetRightInspiratoryResistance()
{
  if (m_RightInspiratoryResistance == nullptr)
    m_RightInspiratoryResistance = new SEScalarPressureTimePerVolume();
  return *m_RightInspiratoryResistance;
}
double SERespiratoryMechanics::GetRightInspiratoryResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_RightInspiratoryResistance == nullptr)
    return SEScalar::dNaN();
  return m_RightInspiratoryResistance->GetValue(unit);
}

bool SERespiratoryMechanics::HasUpperExpiratoryResistance() const
{
  return m_UpperExpiratoryResistance == nullptr ? false : m_UpperExpiratoryResistance->IsValid();
}
SEScalarPressureTimePerVolume& SERespiratoryMechanics::GetUpperExpiratoryResistance()
{
  if (m_UpperExpiratoryResistance == nullptr)
    m_UpperExpiratoryResistance = new SEScalarPressureTimePerVolume();
  return *m_UpperExpiratoryResistance;
}
double SERespiratoryMechanics::GetUpperExpiratoryResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_UpperExpiratoryResistance == nullptr)
    return SEScalar::dNaN();
  return m_UpperExpiratoryResistance->GetValue(unit);
}

bool SERespiratoryMechanics::HasUpperInspiratoryResistance() const
{
  return m_UpperInspiratoryResistance == nullptr ? false : m_UpperInspiratoryResistance->IsValid();
}
SEScalarPressureTimePerVolume& SERespiratoryMechanics::GetUpperInspiratoryResistance()
{
  if (m_UpperInspiratoryResistance == nullptr)
    m_UpperInspiratoryResistance = new SEScalarPressureTimePerVolume();
  return *m_UpperInspiratoryResistance;
}
double SERespiratoryMechanics::GetUpperInspiratoryResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_UpperInspiratoryResistance == nullptr)
    return SEScalar::dNaN();
  return m_UpperInspiratoryResistance->GetValue(unit);
}

bool SERespiratoryMechanics::HasInspiratoryPeakPressure() const
{
  return m_InspiratoryPeakPressure == nullptr ? false : m_InspiratoryPeakPressure->IsValid();
}
SEScalarPressure& SERespiratoryMechanics::GetInspiratoryPeakPressure()
{
  if (m_InspiratoryPeakPressure == nullptr)
    m_InspiratoryPeakPressure = new SEScalarPressure();
  return *m_InspiratoryPeakPressure;
}
double SERespiratoryMechanics::GetInspiratoryPeakPressure(const PressureUnit& unit) const
{
  if (m_InspiratoryPeakPressure == nullptr)
    return SEScalar::dNaN();
  return m_InspiratoryPeakPressure->GetValue(unit);
}

bool SERespiratoryMechanics::HasExpiratoryPeakPressure() const
{
  return m_ExpiratoryPeakPressure == nullptr ? false : m_ExpiratoryPeakPressure->IsValid();
}
SEScalarPressure& SERespiratoryMechanics::GetExpiratoryPeakPressure()
{
  if (m_ExpiratoryPeakPressure == nullptr)
    m_ExpiratoryPeakPressure = new SEScalarPressure();
  return *m_ExpiratoryPeakPressure;
}
double SERespiratoryMechanics::GetExpiratoryPeakPressure(const PressureUnit& unit) const
{
  if (m_ExpiratoryPeakPressure == nullptr)
    return SEScalar::dNaN();
  return m_ExpiratoryPeakPressure->GetValue(unit);
}

bool SERespiratoryMechanics::HasInspiratoryRiseTime() const
{
  return m_InspiratoryRiseTime == nullptr ? false : m_InspiratoryRiseTime->IsValid();
}
SEScalarTime& SERespiratoryMechanics::GetInspiratoryRiseTime()
{
  if (m_InspiratoryRiseTime == nullptr)
    m_InspiratoryRiseTime = new SEScalarTime();
  return *m_InspiratoryRiseTime;
}
double SERespiratoryMechanics::GetInspiratoryRiseTime(const TimeUnit& unit) const
{
  if (m_InspiratoryRiseTime == nullptr)
    return SEScalar::dNaN();
  return m_InspiratoryRiseTime->GetValue(unit);
}

bool SERespiratoryMechanics::HasInspiratoryHoldTime() const
{
  return m_InspiratoryHoldTime == nullptr ? false : m_InspiratoryHoldTime->IsValid();
}
SEScalarTime& SERespiratoryMechanics::GetInspiratoryHoldTime()
{
  if (m_InspiratoryHoldTime == nullptr)
    m_InspiratoryHoldTime = new SEScalarTime();
  return *m_InspiratoryHoldTime;
}
double SERespiratoryMechanics::GetInspiratoryHoldTime(const TimeUnit& unit) const
{
  if (m_InspiratoryHoldTime == nullptr)
    return SEScalar::dNaN();
  return m_InspiratoryHoldTime->GetValue(unit);
}

bool SERespiratoryMechanics::HasInspiratoryReleaseTime() const
{
  return m_InspiratoryReleaseTime == nullptr ? false : m_InspiratoryReleaseTime->IsValid();
}
SEScalarTime& SERespiratoryMechanics::GetInspiratoryReleaseTime()
{
  if (m_InspiratoryReleaseTime == nullptr)
    m_InspiratoryReleaseTime = new SEScalarTime();
  return *m_InspiratoryReleaseTime;
}
double SERespiratoryMechanics::GetInspiratoryReleaseTime(const TimeUnit& unit) const
{
  if (m_InspiratoryReleaseTime == nullptr)
    return SEScalar::dNaN();
  return m_InspiratoryReleaseTime->GetValue(unit);
}

bool SERespiratoryMechanics::HasInspiratoryToExpiratoryPauseTime() const
{
  return m_InspiratoryToExpiratoryPauseTime == nullptr ? false : m_InspiratoryToExpiratoryPauseTime->IsValid();
}
SEScalarTime& SERespiratoryMechanics::GetInspiratoryToExpiratoryPauseTime()
{
  if (m_InspiratoryToExpiratoryPauseTime == nullptr)
    m_InspiratoryToExpiratoryPauseTime = new SEScalarTime();
  return *m_InspiratoryToExpiratoryPauseTime;
}
double SERespiratoryMechanics::GetInspiratoryToExpiratoryPauseTime(const TimeUnit& unit) const
{
  if (m_InspiratoryToExpiratoryPauseTime == nullptr)
    return SEScalar::dNaN();
  return m_InspiratoryToExpiratoryPauseTime->GetValue(unit);
}

bool SERespiratoryMechanics::HasExpiratoryRiseTime() const
{
  return m_ExpiratoryRiseTime == nullptr ? false : m_ExpiratoryRiseTime->IsValid();
}
SEScalarTime& SERespiratoryMechanics::GetExpiratoryRiseTime()
{
  if (m_ExpiratoryRiseTime == nullptr)
    m_ExpiratoryRiseTime = new SEScalarTime();
  return *m_ExpiratoryRiseTime;
}
double SERespiratoryMechanics::GetExpiratoryRiseTime(const TimeUnit& unit) const
{
  if (m_ExpiratoryRiseTime == nullptr)
    return SEScalar::dNaN();
  return m_ExpiratoryRiseTime->GetValue(unit);
}

bool SERespiratoryMechanics::HasExpiratoryHoldTime() const
{
  return m_ExpiratoryHoldTime == nullptr ? false : m_ExpiratoryHoldTime->IsValid();
}
SEScalarTime& SERespiratoryMechanics::GetExpiratoryHoldTime()
{
  if (m_ExpiratoryHoldTime == nullptr)
    m_ExpiratoryHoldTime = new SEScalarTime();
  return *m_ExpiratoryHoldTime;
}
double SERespiratoryMechanics::GetExpiratoryHoldTime(const TimeUnit& unit) const
{
  if (m_ExpiratoryHoldTime == nullptr)
    return SEScalar::dNaN();
  return m_ExpiratoryHoldTime->GetValue(unit);
}

bool SERespiratoryMechanics::HasExpiratoryReleaseTime() const
{
  return m_ExpiratoryReleaseTime == nullptr ? false : m_ExpiratoryReleaseTime->IsValid();
}
SEScalarTime& SERespiratoryMechanics::GetExpiratoryReleaseTime()
{
  if (m_ExpiratoryReleaseTime == nullptr)
    m_ExpiratoryReleaseTime = new SEScalarTime();
  return *m_ExpiratoryReleaseTime;
}
double SERespiratoryMechanics::GetExpiratoryReleaseTime(const TimeUnit& unit) const
{
  if (m_ExpiratoryReleaseTime == nullptr)
    return SEScalar::dNaN();
  return m_ExpiratoryReleaseTime->GetValue(unit);
}

bool SERespiratoryMechanics::HasResidueTime() const
{
  return m_ResidueTime == nullptr ? false : m_ResidueTime->IsValid();
}
SEScalarTime& SERespiratoryMechanics::GetResidueTime()
{
  if (m_ResidueTime == nullptr)
    m_ResidueTime = new SEScalarTime();
  return *m_ResidueTime;
}
double SERespiratoryMechanics::GetResidueTime(const TimeUnit& unit) const
{
  if (m_ResidueTime == nullptr)
    return SEScalar::dNaN();
  return m_ResidueTime->GetValue(unit);
}

std::string SERespiratoryMechanics::ToString() const
{
  std::string str = "Respiratory Mechanics";
  str += "\n\tActive: "; HasActive() ? str += eSwitch_Name(m_Active) : str += "Not Set";
  str += "\n\tDefaultType: " + eDefaultType_Name(m_DefaultType);
  str += "\n\tLeftComplianceCurve: "; HasLeftComplianceCurve() ? str += m_LeftComplianceCurve->ToString() : str += "Not Set";
  str += "\n\tRightComplianceCurve: "; HasRightComplianceCurve() ? str += m_RightComplianceCurve->ToString() : str += "Not Set";
  str += "\n\tLeftExpiratoryResistance: "; HasLeftExpiratoryResistance() ? str += m_LeftExpiratoryResistance->ToString() : str += "Not Set";
  str += "\n\tLeftInspiratoryResistance: "; HasLeftInspiratoryResistance() ? str += m_LeftInspiratoryResistance->ToString() : str += "Not Set";
  str += "\n\tRightExpiratoryResistance: "; HasRightExpiratoryResistance() ? str += m_RightExpiratoryResistance->ToString() : str += "Not Set";
  str += "\n\tRightInspiratoryResistance: "; HasRightInspiratoryResistance() ? str += m_RightInspiratoryResistance->ToString() : str += "Not Set";
  str += "\n\tUpperExpiratoryResistance: "; HasUpperExpiratoryResistance() ? str += m_UpperExpiratoryResistance->ToString() : str += "Not Set";
  str += "\n\tUpperInspiratoryResistance: "; HasUpperInspiratoryResistance() ? str += m_UpperInspiratoryResistance->ToString() : str += "Not Set";
  str += "\n\tInspiratoryPeakPressure: "; HasInspiratoryPeakPressure() ? str += m_InspiratoryPeakPressure->ToString() : str += "Not Set";
  str += "\n\tExpiratoryPeakPressure: "; HasExpiratoryPeakPressure() ? str += m_ExpiratoryPeakPressure->ToString() : str += "Not Set";
  str += "\n\tInspiratoryRiseTime: "; HasInspiratoryRiseTime() ? str += m_InspiratoryRiseTime->ToString() : str += "Not Set";
  str += "\n\tInspiratoryHoldTime: "; HasInspiratoryHoldTime() ? str += m_InspiratoryHoldTime->ToString() : str += "Not Set";
  str += "\n\tInspiratoryReleaseTime: "; HasInspiratoryReleaseTime() ? str += m_InspiratoryReleaseTime->ToString() : str += "Not Set";
  str += "\n\tInspiratoryToExpiratoryPauseTime: "; HasInspiratoryToExpiratoryPauseTime() ? str += m_InspiratoryToExpiratoryPauseTime->ToString() : str += "Not Set";
  str += "\n\tExpiratoryRiseTime: "; HasExpiratoryRiseTime() ? str += m_ExpiratoryRiseTime->ToString() : str += "Not Set";
  str += "\n\tExpiratoryHoldTime: "; HasExpiratoryHoldTime() ? str += m_ExpiratoryHoldTime->ToString() : str += "Not Set";
  str += "\n\tExpiratoryReleaseTime: "; HasExpiratoryReleaseTime() ? str += m_ExpiratoryReleaseTime->ToString() : str += "Not Set";
  str += "\n\tResidueTime: "; HasResidueTime() ? str += m_ResidueTime->ToString() : str += "Not Set";
  return str;
}
void SERespiratoryMechanics::ToString(std::ostream& str) const
{
  str << ToString();
}
