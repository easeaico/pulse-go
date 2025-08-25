/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#include "cdm/CommonDefs.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorPressureControl.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/io/protobuf/PBEquipmentActions.h"
#include "cdm/io/protobuf/PBMechanicalVentilator.h"

SEMechanicalVentilatorPressureControl::SEMechanicalVentilatorPressureControl(Logger* logger) : SEMechanicalVentilatorMode(logger)
{
  m_Mode = eMechanicalVentilator_PressureControlMode::AssistedControl;
  m_FractionInspiredOxygen = nullptr;
  m_InspirationPatientTriggerFlow = nullptr;
  m_InspirationPatientTriggerPressure = nullptr;
  m_InspirationWaveform = eDriverWaveform::NullDriverWaveform;
  m_InspiratoryPeriod = nullptr;
  m_InspiratoryPressure = nullptr;
  m_PositiveEndExpiratoryPressure = nullptr;
  m_RespirationRate = nullptr;
  m_Slope = nullptr;
}

SEMechanicalVentilatorPressureControl::~SEMechanicalVentilatorPressureControl()
{
  m_Mode = eMechanicalVentilator_PressureControlMode::AssistedControl;
  SAFE_DELETE(m_FractionInspiredOxygen);
  SAFE_DELETE(m_InspirationPatientTriggerFlow);
  SAFE_DELETE(m_InspirationPatientTriggerPressure);
  m_InspirationWaveform = eDriverWaveform::NullDriverWaveform;
  SAFE_DELETE(m_InspiratoryPeriod);
  SAFE_DELETE(m_InspiratoryPressure);
  SAFE_DELETE(m_PositiveEndExpiratoryPressure);
  SAFE_DELETE(m_RespirationRate);
  SAFE_DELETE(m_Slope);
}

void SEMechanicalVentilatorPressureControl::Clear()
{
  SEMechanicalVentilatorMode::Clear();
  m_Mode = eMechanicalVentilator_PressureControlMode::AssistedControl;
  INVALIDATE_PROPERTY(m_FractionInspiredOxygen);
  INVALIDATE_PROPERTY(m_InspirationPatientTriggerFlow);
  INVALIDATE_PROPERTY(m_InspirationPatientTriggerPressure);
  m_InspirationWaveform = eDriverWaveform::NullDriverWaveform;
  INVALIDATE_PROPERTY(m_InspiratoryPeriod);
  INVALIDATE_PROPERTY(m_InspiratoryPressure);
  INVALIDATE_PROPERTY(m_PositiveEndExpiratoryPressure);
  INVALIDATE_PROPERTY(m_RespirationRate);
  INVALIDATE_PROPERTY(m_Slope);
}

void SEMechanicalVentilatorPressureControl::MergeMode(const SEMechanicalVentilatorPressureControl& src, const SESubstanceManager& subMgr, eMergeType mt)
{
  if (mt == eMergeType::Replace)
    PBEquipmentAction::Copy(src, *this, subMgr);
  else
  {
    m_Mode = src.m_Mode;
    if (src.HasFractionInspiredOxygen())
      GetFractionInspiredOxygen().Set(*src.m_FractionInspiredOxygen);
    if (src.HasInspirationPatientTriggerFlow())
      GetInspirationPatientTriggerFlow().Set(*src.m_InspirationPatientTriggerFlow);
    if (src.HasInspirationPatientTriggerPressure())
      GetInspirationPatientTriggerPressure().Set(*src.m_InspirationPatientTriggerPressure);
    if (src.HasInspirationWaveform())
      SetInspirationWaveform(src.m_InspirationWaveform);
    if (src.HasInspiratoryPeriod())
      GetInspiratoryPeriod().Set(*src.m_InspiratoryPeriod);
    if (src.HasInspiratoryPressure())
      GetInspiratoryPressure().Set(*src.m_InspiratoryPressure);
    if (src.HasPositiveEndExpiratoryPressure())
      GetPositiveEndExpiratoryPressure().Set(*src.m_PositiveEndExpiratoryPressure);
    if (src.HasRespirationRate())
      GetRespirationRate().Set(*src.m_RespirationRate);
    if (src.HasSlope())
      GetSlope().Set(*src.m_Slope);

    if (src.HasSupplementalSettings())
      PBMechanicalVentilator::Copy(*src.GetSupplementalSettings(), GetSupplementalSettings(), subMgr);
    else if (HasSupplementalSettings())
      m_SupplementalSettings->Clear();
  }
}

void SEMechanicalVentilatorPressureControl::Copy(const SEMechanicalVentilatorPressureControl& src, const SESubstanceManager& subMgr, bool /*preserveState*/)
{// Using Bindings to make a copy
  PBEquipmentAction::Copy(src, *this, subMgr);
}

bool SEMechanicalVentilatorPressureControl::ToSettings(SEMechanicalVentilatorSettings& s, SESubstanceManager& subMgr, eMergeType mt)
{
  if (!SEMechanicalVentilatorMode::ToSettings(s, subMgr, mt))
    return false;
  if (SEMechanicalVentilatorMode::IsActive() && GetSupplementalSettings().GetConnection() != eSwitch::Off)
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
    else
    {
      Error("No expiration cycle time defined. The Inspiratory Period and/or the Slope must be set.");
      return false;
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

    double peakInspiratoryPressure_cmH2O = GetInspiratoryPressure(PressureUnit::cmH2O);
    double positiveEndExpiredPressure_cmH2O = GetPositiveEndExpiratoryPressure(PressureUnit::cmH2O);
    if (positiveEndExpiredPressure_cmH2O > peakInspiratoryPressure_cmH2O)
    {
        Error("Positive End Expired Pressure cannot be higher than the Peak Inspiratory Pressure.");
        return false;
    }
    s.GetExpirationCycleTime().SetValue(inspiratoryPeriod_s, TimeUnit::s);
    s.GetInspirationMachineTriggerTime().SetValue(expiratoryPeriod_s, TimeUnit::s);
    s.GetInspirationWaveformPeriod().SetValue(inspirationWaveformPeriod_s, TimeUnit::s);
    s.GetPeakInspiratoryPressure().SetValue(peakInspiratoryPressure_cmH2O, PressureUnit::cmH2O);
    s.GetPositiveEndExpiratoryPressure().SetValue(positiveEndExpiredPressure_cmH2O, PressureUnit::cmH2O);
    s.GetFractionInspiredGas(*subMgr.GetSubstance("Oxygen")).GetFractionAmount().Set(GetFractionInspiredOxygen());

    // Optional Values (Transfer data, let the SEMechanicalVentilatorSettings class handle precedence)

    s.SetInspirationPatientTriggerRespiratoryModel(eSwitch::Off);
    if (GetMode() == eMechanicalVentilator_PressureControlMode::AssistedControl)
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

bool SEMechanicalVentilatorPressureControl::IsValid() const
{
  if (m_MergeType == eMergeType::Replace)
  {
    if (!SEMechanicalVentilatorMode::IsValid())
      return false;
    if (HasSupplementalSettings())
      if (GetConnection() == eSwitch::Off)
        return true;

      return HasFractionInspiredOxygen() &&
             HasInspiratoryPressure() &&
             HasPositiveEndExpiratoryPressure() &&
             HasRespirationRate();
             // Everything else is optional

    // Revisit how setting file works when in usage
  }
  return true;
}

bool SEMechanicalVentilatorPressureControl::IsActive() const
{
  return SEMechanicalVentilatorMode::IsActive();
}
void SEMechanicalVentilatorPressureControl::Deactivate()
{
  SEMechanicalVentilatorMode::Deactivate();
  Clear();//No stateful properties
}

const SEScalar* SEMechanicalVentilatorPressureControl::GetScalar(const std::string& name)
{
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
  if (name.compare("InspiratoryPressure") == 0)
    return &GetInspiratoryPressure();
  if (name.compare("PositiveEndExpiratoryPressure") == 0)
    return &GetPositiveEndExpiratoryPressure();
  if (name.compare("RespirationRate") == 0)
    return &GetRespirationRate();
  if (name.compare("Slope") == 0)
    return &GetSlope();
  return nullptr;
}

void SEMechanicalVentilatorPressureControl::SetMode(eMechanicalVentilator_PressureControlMode m)
{
  m_Mode = m;
}
eMechanicalVentilator_PressureControlMode SEMechanicalVentilatorPressureControl::GetMode() const
{
  return m_Mode;
}

bool SEMechanicalVentilatorPressureControl::HasFractionInspiredOxygen() const
{
  return m_FractionInspiredOxygen != nullptr;
}
SEScalar0To1& SEMechanicalVentilatorPressureControl::GetFractionInspiredOxygen()
{
  if (m_FractionInspiredOxygen == nullptr)
    m_FractionInspiredOxygen = new SEScalar0To1();
  return *m_FractionInspiredOxygen;
}
double SEMechanicalVentilatorPressureControl::GetFractionInspiredOxygen() const
{
  if (m_FractionInspiredOxygen == nullptr)
    return SEScalar::dNaN();
  return m_FractionInspiredOxygen->GetValue();
}

bool SEMechanicalVentilatorPressureControl::HasInspirationPatientTriggerFlow() const
{
  return m_InspirationPatientTriggerFlow == nullptr ? false : m_InspirationPatientTriggerFlow->IsValid();
}
SEScalarVolumePerTime& SEMechanicalVentilatorPressureControl::GetInspirationPatientTriggerFlow()
{
  if (m_InspirationPatientTriggerFlow == nullptr)
    m_InspirationPatientTriggerFlow = new SEScalarVolumePerTime();
  return *m_InspirationPatientTriggerFlow;
}
double SEMechanicalVentilatorPressureControl::GetInspirationPatientTriggerFlow(const VolumePerTimeUnit& unit) const
{
  if (m_InspirationPatientTriggerFlow == nullptr)
    return SEScalar::dNaN();
  return m_InspirationPatientTriggerFlow->GetValue(unit);
}

bool SEMechanicalVentilatorPressureControl::HasInspirationPatientTriggerPressure() const
{
  return m_InspirationPatientTriggerPressure == nullptr ? false : m_InspirationPatientTriggerPressure->IsValid();
}
SEScalarPressure& SEMechanicalVentilatorPressureControl::GetInspirationPatientTriggerPressure()
{
  if (m_InspirationPatientTriggerPressure == nullptr)
    m_InspirationPatientTriggerPressure = new SEScalarPressure();
  return *m_InspirationPatientTriggerPressure;
}
double SEMechanicalVentilatorPressureControl::GetInspirationPatientTriggerPressure(const PressureUnit& unit) const
{
  if (m_InspirationPatientTriggerPressure == nullptr)
    return SEScalar::dNaN();
  return m_InspirationPatientTriggerPressure->GetValue(unit);
}

bool SEMechanicalVentilatorPressureControl::HasInspirationWaveform() const
{
  return m_InspirationWaveform != eDriverWaveform::NullDriverWaveform;
}
void SEMechanicalVentilatorPressureControl::SetInspirationWaveform(eDriverWaveform w)
{
  m_InspirationWaveform = w;
}
eDriverWaveform SEMechanicalVentilatorPressureControl::GetInspirationWaveform() const
{
  return m_InspirationWaveform;
}

bool SEMechanicalVentilatorPressureControl::HasInspiratoryPeriod() const
{
  return m_InspiratoryPeriod != nullptr;
}
SEScalarTime& SEMechanicalVentilatorPressureControl::GetInspiratoryPeriod()
{
  if (m_InspiratoryPeriod == nullptr)
    m_InspiratoryPeriod = new SEScalarTime();
  return *m_InspiratoryPeriod;
}
double SEMechanicalVentilatorPressureControl::GetInspiratoryPeriod(const TimeUnit& unit) const
{
  if (m_InspiratoryPeriod == nullptr)
    return SEScalar::dNaN();
  return m_InspiratoryPeriod->GetValue(unit);
}

bool SEMechanicalVentilatorPressureControl::HasInspiratoryPressure() const
{
  return m_InspiratoryPressure != nullptr;
}
SEScalarPressure& SEMechanicalVentilatorPressureControl::GetInspiratoryPressure()
{
  if (m_InspiratoryPressure == nullptr)
    m_InspiratoryPressure = new SEScalarPressure();
  return *m_InspiratoryPressure;
}
double SEMechanicalVentilatorPressureControl::GetInspiratoryPressure(const PressureUnit& unit) const
{
  if (m_InspiratoryPressure == nullptr)
    return SEScalar::dNaN();
  return m_InspiratoryPressure->GetValue(unit);
}

bool SEMechanicalVentilatorPressureControl::HasPositiveEndExpiratoryPressure() const
{
  return m_PositiveEndExpiratoryPressure != nullptr;
}
SEScalarPressure& SEMechanicalVentilatorPressureControl::GetPositiveEndExpiratoryPressure()
{
  if (m_PositiveEndExpiratoryPressure == nullptr)
    m_PositiveEndExpiratoryPressure = new SEScalarPressure();
  return *m_PositiveEndExpiratoryPressure;
}
double SEMechanicalVentilatorPressureControl::GetPositiveEndExpiratoryPressure(const PressureUnit& unit) const
{
  if (m_PositiveEndExpiratoryPressure == nullptr)
    return SEScalar::dNaN();
  return m_PositiveEndExpiratoryPressure->GetValue(unit);
}

bool SEMechanicalVentilatorPressureControl::HasRespirationRate() const
{
  return m_RespirationRate != nullptr;
}
SEScalarFrequency& SEMechanicalVentilatorPressureControl::GetRespirationRate()
{
  if (m_RespirationRate == nullptr)
    m_RespirationRate = new SEScalarFrequency();
  return *m_RespirationRate;
}
double SEMechanicalVentilatorPressureControl::GetRespirationRate(const FrequencyUnit& unit) const
{
  if (m_RespirationRate == nullptr)
    return SEScalar::dNaN();
  return m_RespirationRate->GetValue(unit);
}

bool SEMechanicalVentilatorPressureControl::HasSlope() const
{
  return m_Slope != nullptr;
}
SEScalarTime& SEMechanicalVentilatorPressureControl::GetSlope()
{
  if (m_Slope == nullptr)
    m_Slope = new SEScalarTime();
  return *m_Slope;
}
double SEMechanicalVentilatorPressureControl::GetSlope(const TimeUnit& unit) const
{
  if (m_Slope == nullptr)
    return SEScalar::dNaN();
  return m_Slope->GetValue(unit);
}
