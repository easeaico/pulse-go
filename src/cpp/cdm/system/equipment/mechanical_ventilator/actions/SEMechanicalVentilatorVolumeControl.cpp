/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#include "cdm/CommonDefs.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorVolumeControl.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/io/protobuf/PBEquipmentActions.h"

SEMechanicalVentilatorVolumeControl::SEMechanicalVentilatorVolumeControl(Logger* logger) : SEMechanicalVentilatorMode(logger)
{
  m_Mode = eMechanicalVentilator_VolumeControlMode::AssistedControl;
  m_Flow = nullptr;
  m_FractionInspiredOxygen = nullptr;
  m_InspirationPatientTriggerFlow = nullptr;
  m_InspirationPatientTriggerPressure = nullptr;
  m_InspirationWaveform = eDriverWaveform::NullDriverWaveform;
  m_InspiratoryPeriod = nullptr;
  m_PositiveEndExpiratoryPressure = nullptr;
  m_RespirationRate = nullptr;
  m_TidalVolume = nullptr;
  m_Slope = nullptr;
}

SEMechanicalVentilatorVolumeControl::~SEMechanicalVentilatorVolumeControl()
{
  m_Mode = eMechanicalVentilator_VolumeControlMode::AssistedControl;
  SAFE_DELETE(m_Flow);
  SAFE_DELETE(m_FractionInspiredOxygen);
  SAFE_DELETE(m_InspirationPatientTriggerFlow);
  SAFE_DELETE(m_InspirationPatientTriggerPressure);
  m_InspirationWaveform = eDriverWaveform::NullDriverWaveform;
  SAFE_DELETE(m_InspiratoryPeriod);
  SAFE_DELETE(m_PositiveEndExpiratoryPressure);
  SAFE_DELETE(m_RespirationRate);
  SAFE_DELETE(m_TidalVolume);
  SAFE_DELETE(m_Slope);
}

void SEMechanicalVentilatorVolumeControl::Clear()
{
  SEMechanicalVentilatorMode::Clear();
  m_Mode = eMechanicalVentilator_VolumeControlMode::AssistedControl;
  INVALIDATE_PROPERTY(m_Flow);
  INVALIDATE_PROPERTY(m_FractionInspiredOxygen);
  m_InspirationWaveform = eDriverWaveform::NullDriverWaveform;
  INVALIDATE_PROPERTY(m_InspirationPatientTriggerFlow);
  INVALIDATE_PROPERTY(m_InspirationPatientTriggerPressure);
  INVALIDATE_PROPERTY(m_InspiratoryPeriod);
  INVALIDATE_PROPERTY(m_PositiveEndExpiratoryPressure);
  INVALIDATE_PROPERTY(m_RespirationRate);
  INVALIDATE_PROPERTY(m_TidalVolume);
  INVALIDATE_PROPERTY(m_Slope);
}

void SEMechanicalVentilatorVolumeControl::Copy(const SEMechanicalVentilatorVolumeControl& src, const SESubstanceManager& subMgr, bool /*preserveState*/)
{// Using Bindings to make a copy
  PBEquipmentAction::Copy(src, *this, subMgr);
}

bool SEMechanicalVentilatorVolumeControl::ToSettings(SEMechanicalVentilatorSettings& s, const SESubstanceManager& subMgr)
{
  if (!SEMechanicalVentilatorMode::ToSettings(s, subMgr))
    return false;
  if (SEMechanicalVentilatorMode::IsActive())
  {
    // Translate ventilator settings
    double totalPeriod_s = 60.0 / GetRespirationRate(FrequencyUnit::Per_min);
    double inspiratoryPeriod_s = 0.0;

    if (HasInspiratoryPeriod())
    {
      inspiratoryPeriod_s = GetInspiratoryPeriod(TimeUnit::s);
    }
    else if (HasSlope())
    {
      inspiratoryPeriod_s = GetSlope(TimeUnit::s);
    }

    if (inspiratoryPeriod_s > totalPeriod_s)
    {
      Error("Inspiratory Period is longer than the total period applied using Respiration Rate.");
      return false;
    }

    double inspirationWaveformPeriod_s = inspiratoryPeriod_s;
    if (HasSlope())
    {
      inspirationWaveformPeriod_s = GetSlope(TimeUnit::s);
    }
    if (inspirationWaveformPeriod_s > inspiratoryPeriod_s)
    {
      Error("Inspiration Waveform Period (i.e., Slope) cannot be longer than the Inspiratory Period.");
      return false;
    }

    double expiratoryPeriod_s = totalPeriod_s - inspiratoryPeriod_s;

    s.GetInspirationLimitVolume().Set(GetTidalVolume());
    s.GetInspirationTargetFlow().Set(GetFlow());
    s.GetInspirationMachineTriggerTime().SetValue(expiratoryPeriod_s, TimeUnit::s);
    s.GetInspirationWaveformPeriod().SetValue(inspirationWaveformPeriod_s, TimeUnit::s);
    s.GetPositiveEndExpiratoryPressure().Set(GetPositiveEndExpiratoryPressure());
    s.GetFractionInspiredGas(*subMgr.GetSubstance("Oxygen")).GetFractionAmount().Set(GetFractionInspiredOxygen());

    // Optional Values (Transfer data, let the SEMechanicalVentilatorSettings class handle precedence)

    if (inspiratoryPeriod_s > 0.0)
    {
      s.GetExpirationCycleTime().SetValue(inspiratoryPeriod_s, TimeUnit::s);
    }
    else
    {
      s.GetExpirationCycleVolume().Set(GetTidalVolume());
    }

    s.SetInspirationPatientTriggerRespiratoryModel(eSwitch::Off);
    if (GetMode() == eMechanicalVentilator_VolumeControlMode::AssistedControl)
    {
      if (HasInspirationPatientTriggerFlow())
        s.GetInspirationPatientTriggerFlow().Set(GetInspirationPatientTriggerFlow());
      if (HasInspirationPatientTriggerPressure())
        s.GetInspirationPatientTriggerPressure().Set(GetInspirationPatientTriggerPressure());
      if(!HasInspirationPatientTriggerFlow() && !HasInspirationPatientTriggerPressure())
        s.SetInspirationPatientTriggerRespiratoryModel(eSwitch::On);
    }

    s.SetExpirationWaveform(eDriverWaveform::Square);
    if (HasInspirationWaveform())
      s.SetInspirationWaveform(GetInspirationWaveform());
    else
      s.SetInspirationWaveform(eDriverWaveform::Square);
  }
  return true;
}

bool SEMechanicalVentilatorVolumeControl::IsValid() const
{
  if (!IsActive())
    return true;

  return SEMechanicalVentilatorMode::IsValid() &&
    HasFlow() &&
    HasFractionInspiredOxygen() &&
    HasPositiveEndExpiratoryPressure() &&
    HasRespirationRate() &&
    HasTidalVolume();
    // Everything else is optional
}

bool SEMechanicalVentilatorVolumeControl::IsActive() const
{
  return SEMechanicalVentilatorMode::IsActive();
}
void SEMechanicalVentilatorVolumeControl::Deactivate()
{
  SEMechanicalVentilatorMode::Deactivate();
  Clear();//No stateful properties
}

const SEScalar* SEMechanicalVentilatorVolumeControl::GetScalar(const std::string& name)
{
  if (name.compare("Flow") == 0)
    return &GetFlow();
  if (name.compare("FractionInspiredOxygen") == 0)
    return &GetFractionInspiredOxygen();
  if (name.compare("InspirationPatientTriggerFlow") == 0)
    return &GetInspirationPatientTriggerFlow();
  if (name.compare("InspirationPatientTriggerPressure") == 0)
    return &GetInspirationPatientTriggerPressure();
  //if (name.compare("InspirationWaveform") == 0)
  //  return &GetInspirationWaveform();
  if (name.compare("InspiratoryPeriod") == 0)
    return &GetInspiratoryPeriod();
  if (name.compare("PositiveEndExpiratoryPressure") == 0)
    return &GetPositiveEndExpiratoryPressure();
  if (name.compare("RespirationRate") == 0)
    return &GetRespirationRate();
  if (name.compare("TidalVolume") == 0)
    return &GetTidalVolume();
  return nullptr;
}

void SEMechanicalVentilatorVolumeControl::SetMode(eMechanicalVentilator_VolumeControlMode m)
{
  m_Mode = m;
}
eMechanicalVentilator_VolumeControlMode SEMechanicalVentilatorVolumeControl::GetMode() const
{
  return m_Mode;
}

bool SEMechanicalVentilatorVolumeControl::HasFlow() const
{
  return m_Flow != nullptr;
}
SEScalarVolumePerTime& SEMechanicalVentilatorVolumeControl::GetFlow()
{
  if (m_Flow == nullptr)
    m_Flow = new SEScalarVolumePerTime();
  return *m_Flow;
}
double SEMechanicalVentilatorVolumeControl::GetFlow(const VolumePerTimeUnit& unit) const
{
  if (m_Flow == nullptr)
    return SEScalar::dNaN();
  return m_Flow->GetValue(unit);
}

bool SEMechanicalVentilatorVolumeControl::HasFractionInspiredOxygen() const
{
  return m_FractionInspiredOxygen != nullptr;
}
SEScalar0To1& SEMechanicalVentilatorVolumeControl::GetFractionInspiredOxygen()
{
  if (m_FractionInspiredOxygen == nullptr)
    m_FractionInspiredOxygen = new SEScalar0To1();
  return *m_FractionInspiredOxygen;
}
double SEMechanicalVentilatorVolumeControl::GetFractionInspiredOxygen() const
{
  if (m_FractionInspiredOxygen == nullptr)
    return SEScalar::dNaN();
  return m_FractionInspiredOxygen->GetValue();
}

bool SEMechanicalVentilatorVolumeControl::HasInspiratoryPeriod() const
{
  return m_InspiratoryPeriod != nullptr;
}
SEScalarTime& SEMechanicalVentilatorVolumeControl::GetInspiratoryPeriod()
{
  if (m_InspiratoryPeriod == nullptr)
    m_InspiratoryPeriod = new SEScalarTime();
  return *m_InspiratoryPeriod;
}
double SEMechanicalVentilatorVolumeControl::GetInspiratoryPeriod(const TimeUnit& unit) const
{
  if (m_InspiratoryPeriod == nullptr)
    return SEScalar::dNaN();
  return m_InspiratoryPeriod->GetValue(unit);
}

bool SEMechanicalVentilatorVolumeControl::HasPositiveEndExpiratoryPressure() const
{
  return m_PositiveEndExpiratoryPressure != nullptr;
}
SEScalarPressure& SEMechanicalVentilatorVolumeControl::GetPositiveEndExpiratoryPressure()
{
  if (m_PositiveEndExpiratoryPressure == nullptr)
    m_PositiveEndExpiratoryPressure = new SEScalarPressure();
  return *m_PositiveEndExpiratoryPressure;
}
double SEMechanicalVentilatorVolumeControl::GetPositiveEndExpiratoryPressure(const PressureUnit& unit) const
{
  if (m_PositiveEndExpiratoryPressure == nullptr)
    return SEScalar::dNaN();
  return m_PositiveEndExpiratoryPressure->GetValue(unit);
}

bool SEMechanicalVentilatorVolumeControl::HasRespirationRate() const
{
  return m_RespirationRate != nullptr;
}
SEScalarFrequency& SEMechanicalVentilatorVolumeControl::GetRespirationRate()
{
  if (m_RespirationRate == nullptr)
    m_RespirationRate = new SEScalarFrequency();
  return *m_RespirationRate;
}
double SEMechanicalVentilatorVolumeControl::GetRespirationRate(const FrequencyUnit& unit) const
{
  if (m_RespirationRate == nullptr)
    return SEScalar::dNaN();
  return m_RespirationRate->GetValue(unit);
}

bool SEMechanicalVentilatorVolumeControl::HasTidalVolume() const
{
  return m_TidalVolume != nullptr;
}
SEScalarVolume& SEMechanicalVentilatorVolumeControl::GetTidalVolume()
{
  if (m_TidalVolume == nullptr)
    m_TidalVolume = new SEScalarVolume();
  return *m_TidalVolume;
}
double SEMechanicalVentilatorVolumeControl::GetTidalVolume(const VolumeUnit& unit) const
{
  if (m_TidalVolume == nullptr)
    return SEScalar::dNaN();
  return m_TidalVolume->GetValue(unit);
}

bool SEMechanicalVentilatorVolumeControl::HasSlope() const
{
  return m_Slope != nullptr;
}
SEScalarTime& SEMechanicalVentilatorVolumeControl::GetSlope()
{
  if (m_Slope == nullptr)
    m_Slope = new SEScalarTime();
  return *m_Slope;
}
double SEMechanicalVentilatorVolumeControl::GetSlope(const TimeUnit& unit) const
{
  if (m_Slope == nullptr)
    return SEScalar::dNaN();
  return m_Slope->GetValue(unit);
}

bool SEMechanicalVentilatorVolumeControl::HasInspirationWaveform() const
{
  return m_InspirationWaveform != eDriverWaveform::NullDriverWaveform;
}
void SEMechanicalVentilatorVolumeControl::SetInspirationWaveform(eDriverWaveform w)
{
  m_InspirationWaveform = w;
}
eDriverWaveform SEMechanicalVentilatorVolumeControl::GetInspirationWaveform() const
{
  return m_InspirationWaveform;
}

bool SEMechanicalVentilatorVolumeControl::HasInspirationPatientTriggerFlow() const
{
  return m_InspirationPatientTriggerFlow == nullptr ? false : m_InspirationPatientTriggerFlow->IsValid();
}
SEScalarVolumePerTime& SEMechanicalVentilatorVolumeControl::GetInspirationPatientTriggerFlow()
{
  if (m_InspirationPatientTriggerFlow == nullptr)
    m_InspirationPatientTriggerFlow = new SEScalarVolumePerTime();
  return *m_InspirationPatientTriggerFlow;
}
double SEMechanicalVentilatorVolumeControl::GetInspirationPatientTriggerFlow(const VolumePerTimeUnit& unit) const
{
  if (m_InspirationPatientTriggerFlow == nullptr)
    return SEScalar::dNaN();
  return m_InspirationPatientTriggerFlow->GetValue(unit);
}

bool SEMechanicalVentilatorVolumeControl::HasInspirationPatientTriggerPressure() const
{
  return m_InspirationPatientTriggerPressure == nullptr ? false : m_InspirationPatientTriggerPressure->IsValid();
}
SEScalarPressure& SEMechanicalVentilatorVolumeControl::GetInspirationPatientTriggerPressure()
{
  if (m_InspirationPatientTriggerPressure == nullptr)
    m_InspirationPatientTriggerPressure = new SEScalarPressure();
  return *m_InspirationPatientTriggerPressure;
}
double SEMechanicalVentilatorVolumeControl::GetInspirationPatientTriggerPressure(const PressureUnit& unit) const
{
  if (m_InspirationPatientTriggerPressure == nullptr)
    return SEScalar::dNaN();
  return m_InspirationPatientTriggerPressure->GetValue(unit);
}
