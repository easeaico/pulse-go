/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/system/physiology/SETissueSystem.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"

SETissueSystem::SETissueSystem(Logger* logger) : SESystem(logger)
{
  m_CarbonDioxideProductionRate = nullptr;
  m_ExtracellularFluidVolume = nullptr;
  m_ExtravascularFluidVolume = nullptr;
  m_IntracellularFluidVolume = nullptr;
  m_IntracellularFluidPH = nullptr;
  m_OxygenConsumptionRate = nullptr;
  m_RespiratoryExchangeRatio = nullptr;
  m_TotalFluidVolume = nullptr;
}

SETissueSystem::~SETissueSystem()
{
  SAFE_DELETE(m_CarbonDioxideProductionRate);
  SAFE_DELETE(m_ExtracellularFluidVolume);
  SAFE_DELETE(m_ExtravascularFluidVolume);
  SAFE_DELETE(m_IntracellularFluidVolume);
  SAFE_DELETE(m_IntracellularFluidPH);
  SAFE_DELETE(m_OxygenConsumptionRate);
  SAFE_DELETE(m_RespiratoryExchangeRatio);
  SAFE_DELETE(m_TotalFluidVolume);
}

void SETissueSystem::Clear()
{
  SESystem::Clear();
  INVALIDATE_PROPERTY(m_CarbonDioxideProductionRate);
  INVALIDATE_PROPERTY(m_ExtracellularFluidVolume);
  INVALIDATE_PROPERTY(m_ExtravascularFluidVolume);
  INVALIDATE_PROPERTY(m_IntracellularFluidVolume);
  INVALIDATE_PROPERTY(m_IntracellularFluidPH);
  INVALIDATE_PROPERTY(m_OxygenConsumptionRate);
  INVALIDATE_PROPERTY(m_RespiratoryExchangeRatio);
  INVALIDATE_PROPERTY(m_TotalFluidVolume);
}

const SEScalar* SETissueSystem::GetScalar(const std::string& name)
{
  if (name.compare("CarbonDioxideProductionRate") == 0)
    return &GetCarbonDioxideProductionRate();
  if (name.compare("ExtracellularFluidVolume") == 0)
    return &GetExtracellularFluidVolume();
  if (name.compare("ExtravascularFluidVolume") == 0)
    return &GetExtravascularFluidVolume();
  if (name.compare("IntracellularFluidPH") == 0)
    return &GetIntracellularFluidPH();
  if (name.compare("IntracellularFluidVolume") == 0)
    return &GetIntracellularFluidVolume();
  if (name.compare("OxygenConsumptionRate") == 0)
    return &GetOxygenConsumptionRate();
  if (name.compare("RespiratoryExchangeRatio") == 0)
    return &GetRespiratoryExchangeRatio();
  if (name.compare("TotalFluidVolume") == 0)
    return &GetTotalFluidVolume();
  return nullptr;
}

bool SETissueSystem::HasCarbonDioxideProductionRate() const
{
  return m_CarbonDioxideProductionRate == nullptr ? false : m_CarbonDioxideProductionRate->IsValid();
}
SEScalarVolumePerTime& SETissueSystem::GetCarbonDioxideProductionRate()
{
  if (m_CarbonDioxideProductionRate == nullptr)
    m_CarbonDioxideProductionRate = new SEScalarVolumePerTime();
  return *m_CarbonDioxideProductionRate;
}
double SETissueSystem::GetCarbonDioxideProductionRate(const VolumePerTimeUnit& unit) const
{
  if (m_CarbonDioxideProductionRate == nullptr)
    return SEScalar::dNaN();
  return m_CarbonDioxideProductionRate->GetValue(unit);
}

bool SETissueSystem::HasExtracellularFluidVolume() const
{
  return m_ExtracellularFluidVolume == nullptr ? false : m_ExtracellularFluidVolume->IsValid();
}
SEScalarVolume& SETissueSystem::GetExtracellularFluidVolume()
{
  if (m_ExtracellularFluidVolume == nullptr)
    m_ExtracellularFluidVolume = new SEScalarVolume();
  return *m_ExtracellularFluidVolume;
}
double SETissueSystem::GetExtracellularFluidVolume(const VolumeUnit& unit) const
{
  if (m_ExtracellularFluidVolume == nullptr)
    return SEScalar::dNaN();
  return m_ExtracellularFluidVolume->GetValue(unit);
}

bool SETissueSystem::HasExtravascularFluidVolume() const
{
  return m_ExtravascularFluidVolume == nullptr ? false : m_ExtravascularFluidVolume->IsValid();
}
SEScalarVolume& SETissueSystem::GetExtravascularFluidVolume()
{
  if (m_ExtravascularFluidVolume == nullptr)
    m_ExtravascularFluidVolume = new SEScalarVolume();
  return *m_ExtravascularFluidVolume;
}
double SETissueSystem::GetExtravascularFluidVolume(const VolumeUnit& unit) const
{
  if (m_ExtravascularFluidVolume == nullptr)
    return SEScalar::dNaN();
  return m_ExtravascularFluidVolume->GetValue(unit);
}

bool SETissueSystem::HasIntracellularFluidVolume() const
{
  return m_IntracellularFluidVolume == nullptr ? false : m_IntracellularFluidVolume->IsValid();
}
SEScalarVolume& SETissueSystem::GetIntracellularFluidVolume()
{
  if (m_IntracellularFluidVolume == nullptr)
    m_IntracellularFluidVolume = new SEScalarVolume();
  return *m_IntracellularFluidVolume;
}
double SETissueSystem::GetIntracellularFluidVolume(const VolumeUnit& unit) const
{
  if (m_IntracellularFluidVolume == nullptr)
    return SEScalar::dNaN();
  return m_IntracellularFluidVolume->GetValue(unit);
}

bool SETissueSystem::HasIntracellularFluidPH() const
{
  return m_IntracellularFluidPH == nullptr ? false : m_IntracellularFluidPH->IsValid();
}
SEScalar& SETissueSystem::GetIntracellularFluidPH()
{
  if (m_IntracellularFluidPH == nullptr)
    m_IntracellularFluidPH = new SEScalar();
  return *m_IntracellularFluidPH;
}
double SETissueSystem::GetIntracellularFluidPH() const
{
  if (m_IntracellularFluidPH == nullptr)
    return SEScalar::dNaN();
  return m_IntracellularFluidPH->GetValue();
}

bool SETissueSystem::HasOxygenConsumptionRate() const
{
  return m_OxygenConsumptionRate == nullptr ? false : m_OxygenConsumptionRate->IsValid();
}
SEScalarVolumePerTime& SETissueSystem::GetOxygenConsumptionRate()
{
  if (m_OxygenConsumptionRate == nullptr)
    m_OxygenConsumptionRate = new SEScalarVolumePerTime();
  return *m_OxygenConsumptionRate;
}
double SETissueSystem::GetOxygenConsumptionRate(const VolumePerTimeUnit& unit) const
{
  if (m_OxygenConsumptionRate == nullptr)
    return SEScalar::dNaN();
  return m_OxygenConsumptionRate->GetValue(unit);
}

bool SETissueSystem::HasRespiratoryExchangeRatio() const
{
  return m_RespiratoryExchangeRatio == nullptr ? false : m_RespiratoryExchangeRatio->IsValid();
}
SEScalar& SETissueSystem::GetRespiratoryExchangeRatio()
{
  if (m_RespiratoryExchangeRatio == nullptr)
    m_RespiratoryExchangeRatio = new SEScalar();
  return *m_RespiratoryExchangeRatio;
}
double SETissueSystem::GetRespiratoryExchangeRatio() const
{
  if (m_RespiratoryExchangeRatio == nullptr)
    return SEScalar::dNaN();
  return m_RespiratoryExchangeRatio->GetValue();
}

bool SETissueSystem::HasTotalFluidVolume() const
{
  return m_TotalFluidVolume == nullptr ? false : m_TotalFluidVolume->IsValid();
}
SEScalarVolume& SETissueSystem::GetTotalFluidVolume()
{
  if (m_TotalFluidVolume == nullptr)
    m_TotalFluidVolume = new SEScalarVolume();
  return *m_TotalFluidVolume;
}
double SETissueSystem::GetTotalFluidVolume(const VolumeUnit& unit) const
{
  if (m_TotalFluidVolume == nullptr)
    return SEScalar::dNaN();
  return m_TotalFluidVolume->GetValue(unit);
}
