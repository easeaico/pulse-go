/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/system/equipment/mechanical_ventilator/SEMechanicalVentilatorAlarms.h"
#include "cdm/io/protobuf/PBMechanicalVentilator.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"

SEMechanicalVentilatorAlarms::SEMechanicalVentilatorAlarms(Logger* logger) : Loggable(logger)
{
  m_ApneaTimeThreshold = nullptr;
  m_AutoPositiveEndExpiratoryPressureThreshold = nullptr;
  m_CircuitLeakThreshold = nullptr;
  m_HighEndTidalCarbonDioxideThreshold = nullptr;
  m_HighMinuteVentilationThreshold = nullptr;
  m_HighOxygenSaturationThreshold = nullptr;
  m_HighPositiveEndExpiratoryPressureThreshold = nullptr;
  m_HighPressureCycleOption = eSwitch::Off;
  m_HighPressureThreshold = nullptr;
  m_HighRespiratoryRateThreshold = nullptr;
  m_HighTidalVolumeThreshold = nullptr;
  m_LowEndTidalCarbonDioxideThreshold = nullptr;
  m_LowMinuteVentilationThreshold = nullptr;
  m_LowOxygenSaturationThreshold = nullptr;
  m_LowPositiveEndExpiratoryPressureThreshold = nullptr;
  m_LowPressureThreshold = nullptr;
  m_LowTidalVolumeThreshold = nullptr;
  m_OxygenSupplyFailureThreshold = nullptr;
}

SEMechanicalVentilatorAlarms::~SEMechanicalVentilatorAlarms()
{
  SAFE_DELETE(m_ApneaTimeThreshold);
  SAFE_DELETE(m_AutoPositiveEndExpiratoryPressureThreshold);
  SAFE_DELETE(m_CircuitLeakThreshold);
  SAFE_DELETE(m_HighEndTidalCarbonDioxideThreshold);
  SAFE_DELETE(m_HighMinuteVentilationThreshold);
  SAFE_DELETE(m_HighOxygenSaturationThreshold);
  SAFE_DELETE(m_HighPositiveEndExpiratoryPressureThreshold);
  m_HighPressureCycleOption = eSwitch::Off;
  SAFE_DELETE(m_HighPressureThreshold);
  SAFE_DELETE(m_HighRespiratoryRateThreshold);
  SAFE_DELETE(m_HighTidalVolumeThreshold);
  SAFE_DELETE(m_LowEndTidalCarbonDioxideThreshold);
  SAFE_DELETE(m_LowMinuteVentilationThreshold);
  SAFE_DELETE(m_LowOxygenSaturationThreshold);
  SAFE_DELETE(m_LowPositiveEndExpiratoryPressureThreshold);
  SAFE_DELETE(m_LowPressureThreshold);
  SAFE_DELETE(m_LowTidalVolumeThreshold);
  SAFE_DELETE(m_OxygenSupplyFailureThreshold);
}

void SEMechanicalVentilatorAlarms::Clear()
{
  INVALIDATE_PROPERTY(m_ApneaTimeThreshold);
  INVALIDATE_PROPERTY(m_AutoPositiveEndExpiratoryPressureThreshold);
  INVALIDATE_PROPERTY(m_CircuitLeakThreshold);
  INVALIDATE_PROPERTY(m_HighEndTidalCarbonDioxideThreshold);
  INVALIDATE_PROPERTY(m_HighMinuteVentilationThreshold);
  INVALIDATE_PROPERTY(m_HighOxygenSaturationThreshold);
  INVALIDATE_PROPERTY(m_HighPositiveEndExpiratoryPressureThreshold);
  m_HighPressureCycleOption = eSwitch::Off;
  INVALIDATE_PROPERTY(m_HighPressureThreshold);
  INVALIDATE_PROPERTY(m_HighRespiratoryRateThreshold);
  INVALIDATE_PROPERTY(m_HighTidalVolumeThreshold);
  INVALIDATE_PROPERTY(m_LowEndTidalCarbonDioxideThreshold);
  INVALIDATE_PROPERTY(m_LowMinuteVentilationThreshold);
  INVALIDATE_PROPERTY(m_LowOxygenSaturationThreshold);
  INVALIDATE_PROPERTY(m_LowPositiveEndExpiratoryPressureThreshold);
  INVALIDATE_PROPERTY(m_LowPressureThreshold);
  INVALIDATE_PROPERTY(m_LowTidalVolumeThreshold);
  INVALIDATE_PROPERTY(m_OxygenSupplyFailureThreshold);
}

void SEMechanicalVentilatorAlarms::Copy(const SEMechanicalVentilatorAlarms& src)
{
  PBMechanicalVentilator::Copy(src, *this);
}

void SEMechanicalVentilatorAlarms::Merge(const SEMechanicalVentilatorAlarms& from)
{
  COPY_PROPERTY(ApneaTimeThreshold);
  COPY_PROPERTY(AutoPositiveEndExpiratoryPressureThreshold);
  COPY_PROPERTY(CircuitLeakThreshold);
  COPY_PROPERTY(HighEndTidalCarbonDioxideThreshold);
  COPY_PROPERTY(HighMinuteVentilationThreshold);
  COPY_PROPERTY(HighOxygenSaturationThreshold);
  COPY_PROPERTY(HighPositiveEndExpiratoryPressureThreshold);
  if (from.HasHighPressureCycleOption())
    SetHighPressureCycleOption(from.m_HighPressureCycleOption);
  COPY_PROPERTY(HighPressureThreshold);
  COPY_PROPERTY(HighRespiratoryRateThreshold);
  COPY_PROPERTY(HighTidalVolumeThreshold);
  COPY_PROPERTY(LowEndTidalCarbonDioxideThreshold);
  COPY_PROPERTY(LowMinuteVentilationThreshold);
  COPY_PROPERTY(LowOxygenSaturationThreshold);
  COPY_PROPERTY(LowPositiveEndExpiratoryPressureThreshold);
  COPY_PROPERTY(LowPressureThreshold);
  COPY_PROPERTY(LowTidalVolumeThreshold);
  COPY_PROPERTY(OxygenSupplyFailureThreshold);
}

const SEScalar* SEMechanicalVentilatorAlarms::GetScalar(const std::string& name)
{
  if (name == "ApneaTimeThreshold")
    return &GetApneaTimeThreshold();
  if (name == "AutoPositiveEndExpiratoryPressureThreshold")
    return &GetAutoPositiveEndExpiratoryPressureThreshold();
  if (name == "CircuitLeakThreshold")
    return &GetCircuitLeakThreshold();
  if (name == "HighEndTidalCarbonDioxideThreshold")
    return &GetHighEndTidalCarbonDioxideThreshold();
  if (name == "HighMinuteVentilationThreshold")
    return &GetHighMinuteVentilationThreshold();
  if (name == "HighOxygenSaturationThreshold")
    return &GetHighOxygenSaturationThreshold();
  if (name == "HighPositiveEndExpiratoryPressureThreshold")
    return &GetHighPositiveEndExpiratoryPressureThreshold();
  if (name == "HighPressureThreshold")
    return &GetHighPressureThreshold();
  if (name == "HighRespiratoryRateThreshold")
    return &GetHighRespiratoryRateThreshold();
  if (name == "HighTidalVolumeThreshold")
    return &GetHighTidalVolumeThreshold();
  if (name == "LowEndTidalCarbonDioxideThreshold")
    return &GetLowEndTidalCarbonDioxideThreshold();
  if (name == "LowMinuteVentilationThreshold")
    return &GetLowMinuteVentilationThreshold();
  if (name == "LowOxygenSaturationThreshold")
    return &GetLowOxygenSaturationThreshold();
  if (name == "LowPositiveEndExpiratoryPressureThreshold")
    return &GetLowPositiveEndExpiratoryPressureThreshold();
  if (name == "LowPressureThreshold")
    return &GetLowPressureThreshold();
  if (name == "LowTidalVolumeThreshold")
    return &GetLowTidalVolumeThreshold();
  if (name == "OxygenSupplyFailureThreshold")
    return &GetOxygenSupplyFailureThreshold();

  return nullptr;
}

bool SEMechanicalVentilatorAlarms::HasApneaTimeThreshold() const
{
  return m_ApneaTimeThreshold == nullptr ? false : m_ApneaTimeThreshold->IsValid();
}
SEScalarTime& SEMechanicalVentilatorAlarms::GetApneaTimeThreshold()
{
  if (m_ApneaTimeThreshold == nullptr)
    m_ApneaTimeThreshold = new SEScalarTime();
  return *m_ApneaTimeThreshold;
}
double SEMechanicalVentilatorAlarms::GetApneaTimeThreshold(const TimeUnit& unit) const
{
  if (m_ApneaTimeThreshold == nullptr)
    return SEScalar::dNaN();
  return m_ApneaTimeThreshold->GetValue(unit);
}

bool SEMechanicalVentilatorAlarms::HasAutoPositiveEndExpiratoryPressureThreshold() const
{
  return m_AutoPositiveEndExpiratoryPressureThreshold == nullptr ? false : m_AutoPositiveEndExpiratoryPressureThreshold->IsValid();
}
SEScalarPressure& SEMechanicalVentilatorAlarms::GetAutoPositiveEndExpiratoryPressureThreshold()
{
  if (m_AutoPositiveEndExpiratoryPressureThreshold == nullptr)
    m_AutoPositiveEndExpiratoryPressureThreshold = new SEScalarPressure();
  return *m_AutoPositiveEndExpiratoryPressureThreshold;
}
double SEMechanicalVentilatorAlarms::GetAutoPositiveEndExpiratoryPressureThreshold(const PressureUnit& unit) const
{
  if (m_AutoPositiveEndExpiratoryPressureThreshold == nullptr)
    return SEScalar::dNaN();
  return m_AutoPositiveEndExpiratoryPressureThreshold->GetValue(unit);
}

bool SEMechanicalVentilatorAlarms::HasCircuitLeakThreshold() const
{
  return m_CircuitLeakThreshold == nullptr ? false : m_CircuitLeakThreshold->IsValid();
}
SEScalar0To1& SEMechanicalVentilatorAlarms::GetCircuitLeakThreshold()
{
  if (m_CircuitLeakThreshold == nullptr)
    m_CircuitLeakThreshold = new SEScalar0To1();
  return *m_CircuitLeakThreshold;
}
double SEMechanicalVentilatorAlarms::GetCircuitLeakThreshold() const
{
  if (m_CircuitLeakThreshold == nullptr)
    return SEScalar::dNaN();
  return m_CircuitLeakThreshold->GetValue();
}

bool SEMechanicalVentilatorAlarms::HasHighEndTidalCarbonDioxideThreshold() const
{
  return m_HighEndTidalCarbonDioxideThreshold == nullptr ? false : m_HighEndTidalCarbonDioxideThreshold->IsValid();
}
SEScalarPressure& SEMechanicalVentilatorAlarms::GetHighEndTidalCarbonDioxideThreshold()
{
  if (m_HighEndTidalCarbonDioxideThreshold == nullptr)
    m_HighEndTidalCarbonDioxideThreshold = new SEScalarPressure();
  return *m_HighEndTidalCarbonDioxideThreshold;
}
double SEMechanicalVentilatorAlarms::GetHighEndTidalCarbonDioxideThreshold(const PressureUnit& unit) const
{
  if (m_HighEndTidalCarbonDioxideThreshold == nullptr)
    return SEScalar::dNaN();
  return m_HighEndTidalCarbonDioxideThreshold->GetValue(unit);
}

bool SEMechanicalVentilatorAlarms::HasHighMinuteVentilationThreshold() const
{
  return m_HighMinuteVentilationThreshold == nullptr ? false : m_HighMinuteVentilationThreshold->IsValid();
}
SEScalarVolumePerTime& SEMechanicalVentilatorAlarms::GetHighMinuteVentilationThreshold()
{
  if (m_HighMinuteVentilationThreshold == nullptr)
    m_HighMinuteVentilationThreshold = new SEScalarVolumePerTime();
  return *m_HighMinuteVentilationThreshold;
}
double SEMechanicalVentilatorAlarms::GetHighMinuteVentilationThreshold(const VolumePerTimeUnit& unit) const
{
  if (m_HighMinuteVentilationThreshold == nullptr)
    return SEScalar::dNaN();
  return m_HighMinuteVentilationThreshold->GetValue(unit);
}

bool SEMechanicalVentilatorAlarms::HasHighOxygenSaturationThreshold() const
{
  return m_HighOxygenSaturationThreshold == nullptr ? false : m_HighOxygenSaturationThreshold->IsValid();
}
SEScalar0To1& SEMechanicalVentilatorAlarms::GetHighOxygenSaturationThreshold()
{
  if (m_HighOxygenSaturationThreshold == nullptr)
    m_HighOxygenSaturationThreshold = new SEScalar0To1();
  return *m_HighOxygenSaturationThreshold;
}
double SEMechanicalVentilatorAlarms::GetHighOxygenSaturationThreshold() const
{
  if (m_HighOxygenSaturationThreshold == nullptr)
    return SEScalar::dNaN();
  return m_HighOxygenSaturationThreshold->GetValue();
}

bool SEMechanicalVentilatorAlarms::HasHighPositiveEndExpiratoryPressureThreshold() const
{
  return m_HighPositiveEndExpiratoryPressureThreshold == nullptr ? false : m_HighPositiveEndExpiratoryPressureThreshold->IsValid();
}
SEScalarPressure& SEMechanicalVentilatorAlarms::GetHighPositiveEndExpiratoryPressureThreshold()
{
  if (m_HighPositiveEndExpiratoryPressureThreshold == nullptr)
    m_HighPositiveEndExpiratoryPressureThreshold = new SEScalarPressure();
  return *m_HighPositiveEndExpiratoryPressureThreshold;
}
double SEMechanicalVentilatorAlarms::GetHighPositiveEndExpiratoryPressureThreshold(const PressureUnit& unit) const
{
  if (m_HighPositiveEndExpiratoryPressureThreshold == nullptr)
    return SEScalar::dNaN();
  return m_HighPositiveEndExpiratoryPressureThreshold->GetValue(unit);
}

bool SEMechanicalVentilatorAlarms::HasHighPressureCycleOption() const
{
  return m_HighPressureCycleOption != eSwitch::NullSwitch;
}
eSwitch SEMechanicalVentilatorAlarms::GetHighPressureCycleOption() const
{
  return m_HighPressureCycleOption;
}
void SEMechanicalVentilatorAlarms::SetHighPressureCycleOption(eSwitch option)
{
  m_HighPressureCycleOption = (option == eSwitch::NullSwitch) ? eSwitch::Off : option;
}

bool SEMechanicalVentilatorAlarms::HasHighPressureThreshold() const
{
  return m_HighPressureThreshold == nullptr ? false : m_HighPressureThreshold->IsValid();
}
SEScalarPressure& SEMechanicalVentilatorAlarms::GetHighPressureThreshold()
{
  if (m_HighPressureThreshold == nullptr)
    m_HighPressureThreshold = new SEScalarPressure();
  return *m_HighPressureThreshold;
}
double SEMechanicalVentilatorAlarms::GetHighPressureThreshold(const PressureUnit& unit) const
{
  if (m_HighPressureThreshold == nullptr)
    return SEScalar::dNaN();
  return m_HighPressureThreshold->GetValue(unit);
}

bool SEMechanicalVentilatorAlarms::HasHighRespiratoryRateThreshold() const
{
  return m_HighRespiratoryRateThreshold == nullptr ? false : m_HighRespiratoryRateThreshold->IsValid();
}
SEScalarFrequency& SEMechanicalVentilatorAlarms::GetHighRespiratoryRateThreshold()
{
  if (m_HighRespiratoryRateThreshold == nullptr)
    m_HighRespiratoryRateThreshold = new SEScalarFrequency();
  return *m_HighRespiratoryRateThreshold;
}
double SEMechanicalVentilatorAlarms::GetHighRespiratoryRateThreshold(const FrequencyUnit& unit) const
{
  if (m_HighRespiratoryRateThreshold == nullptr)
    return SEScalar::dNaN();
  return m_HighRespiratoryRateThreshold->GetValue(unit);
}

bool SEMechanicalVentilatorAlarms::HasHighTidalVolumeThreshold() const
{
  return m_HighTidalVolumeThreshold == nullptr ? false : m_HighTidalVolumeThreshold->IsValid();
}
SEScalarVolume& SEMechanicalVentilatorAlarms::GetHighTidalVolumeThreshold()
{
  if (m_HighTidalVolumeThreshold == nullptr)
    m_HighTidalVolumeThreshold = new SEScalarVolume();
  return *m_HighTidalVolumeThreshold;
}
double SEMechanicalVentilatorAlarms::GetHighTidalVolumeThreshold(const VolumeUnit& unit) const
{
  if (m_HighTidalVolumeThreshold == nullptr)
    return SEScalar::dNaN();
  return m_HighTidalVolumeThreshold->GetValue(unit);
}

bool SEMechanicalVentilatorAlarms::HasLowEndTidalCarbonDioxideThreshold() const
{
  return m_LowEndTidalCarbonDioxideThreshold == nullptr ? false : m_LowEndTidalCarbonDioxideThreshold->IsValid();
}
SEScalarPressure& SEMechanicalVentilatorAlarms::GetLowEndTidalCarbonDioxideThreshold()
{
  if (m_LowEndTidalCarbonDioxideThreshold == nullptr)
    m_LowEndTidalCarbonDioxideThreshold = new SEScalarPressure();
  return *m_LowEndTidalCarbonDioxideThreshold;
}
double SEMechanicalVentilatorAlarms::GetLowEndTidalCarbonDioxideThreshold(const PressureUnit& unit) const
{
  if (m_LowEndTidalCarbonDioxideThreshold == nullptr)
    return SEScalar::dNaN();
  return m_LowEndTidalCarbonDioxideThreshold->GetValue(unit);
}

bool SEMechanicalVentilatorAlarms::HasLowMinuteVentilationThreshold() const
{
  return m_LowMinuteVentilationThreshold == nullptr ? false : m_LowMinuteVentilationThreshold->IsValid();
}
SEScalarVolumePerTime& SEMechanicalVentilatorAlarms::GetLowMinuteVentilationThreshold()
{
  if (m_LowMinuteVentilationThreshold == nullptr)
    m_LowMinuteVentilationThreshold = new SEScalarVolumePerTime();
  return *m_LowMinuteVentilationThreshold;
}
double SEMechanicalVentilatorAlarms::GetLowMinuteVentilationThreshold(const VolumePerTimeUnit& unit) const
{
  if (m_LowMinuteVentilationThreshold == nullptr)
    return SEScalar::dNaN();
  return m_LowMinuteVentilationThreshold->GetValue(unit);
}

bool SEMechanicalVentilatorAlarms::HasLowOxygenSaturationThreshold() const
{
  return m_LowOxygenSaturationThreshold == nullptr ? false : m_LowOxygenSaturationThreshold->IsValid();
}
SEScalar0To1& SEMechanicalVentilatorAlarms::GetLowOxygenSaturationThreshold()
{
  if (m_LowOxygenSaturationThreshold == nullptr)
    m_LowOxygenSaturationThreshold = new SEScalar0To1();
  return *m_LowOxygenSaturationThreshold;
}
double SEMechanicalVentilatorAlarms::GetLowOxygenSaturationThreshold() const
{
  if (m_LowOxygenSaturationThreshold == nullptr)
    return SEScalar::dNaN();
  return m_LowOxygenSaturationThreshold->GetValue();
}

bool SEMechanicalVentilatorAlarms::HasLowPositiveEndExpiratoryPressureThreshold() const
{
  return m_LowPositiveEndExpiratoryPressureThreshold == nullptr ? false : m_LowPositiveEndExpiratoryPressureThreshold->IsValid();
}
SEScalarPressure& SEMechanicalVentilatorAlarms::GetLowPositiveEndExpiratoryPressureThreshold()
{
  if (m_LowPositiveEndExpiratoryPressureThreshold == nullptr)
    m_LowPositiveEndExpiratoryPressureThreshold = new SEScalarPressure();
  return *m_LowPositiveEndExpiratoryPressureThreshold;
}
double SEMechanicalVentilatorAlarms::GetLowPositiveEndExpiratoryPressureThreshold(const PressureUnit& unit) const
{
  if (m_LowPositiveEndExpiratoryPressureThreshold == nullptr)
    return SEScalar::dNaN();
  return m_LowPositiveEndExpiratoryPressureThreshold->GetValue(unit);
}

bool SEMechanicalVentilatorAlarms::HasLowPressureThreshold() const
{
  return m_LowPressureThreshold == nullptr ? false : m_LowPressureThreshold->IsValid();
}
SEScalarPressure& SEMechanicalVentilatorAlarms::GetLowPressureThreshold()
{
  if (m_LowPressureThreshold == nullptr)
    m_LowPressureThreshold = new SEScalarPressure();
  return *m_LowPressureThreshold;
}
double SEMechanicalVentilatorAlarms::GetLowPressureThreshold(const PressureUnit& unit) const
{
  if (m_LowPressureThreshold == nullptr)
    return SEScalar::dNaN();
  return m_LowPressureThreshold->GetValue(unit);
}

bool SEMechanicalVentilatorAlarms::HasLowTidalVolumeThreshold() const
{
  return m_LowTidalVolumeThreshold == nullptr ? false : m_LowTidalVolumeThreshold->IsValid();
}
SEScalarVolume& SEMechanicalVentilatorAlarms::GetLowTidalVolumeThreshold()
{
  if (m_LowTidalVolumeThreshold == nullptr)
    m_LowTidalVolumeThreshold = new SEScalarVolume();
  return *m_LowTidalVolumeThreshold;
}
double SEMechanicalVentilatorAlarms::GetLowTidalVolumeThreshold(const VolumeUnit& unit) const
{
  if (m_LowTidalVolumeThreshold == nullptr)
    return SEScalar::dNaN();
  return m_LowTidalVolumeThreshold->GetValue(unit);
}

bool SEMechanicalVentilatorAlarms::HasOxygenSupplyFailureThreshold() const
{
  return m_OxygenSupplyFailureThreshold == nullptr ? false : m_OxygenSupplyFailureThreshold->IsValid();
}
SEScalar0To1& SEMechanicalVentilatorAlarms::GetOxygenSupplyFailureThreshold()
{
  if (m_OxygenSupplyFailureThreshold == nullptr)
    m_OxygenSupplyFailureThreshold = new SEScalar0To1();
  return *m_OxygenSupplyFailureThreshold;
}
double SEMechanicalVentilatorAlarms::GetOxygenSupplyFailureThreshold() const
{
  if (m_OxygenSupplyFailureThreshold == nullptr)
    return SEScalar::dNaN();
  return m_OxygenSupplyFailureThreshold->GetValue();
}