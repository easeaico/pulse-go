/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#include "cdm/CommonDefs.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorContinuousPositiveAirwayPressure.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/io/protobuf/PBEquipmentActions.h"

SEMechanicalVentilatorContinuousPositiveAirwayPressure::SEMechanicalVentilatorContinuousPositiveAirwayPressure(Logger* logger) : SEMechanicalVentilatorMode(logger)
{
  m_DeltaPressureSupport = nullptr;
  m_FractionInspiredOxygen = nullptr;
  m_PositiveEndExpiratoryPressure = nullptr;
  m_Slope = nullptr;
  m_InspirationWaveform = eDriverWaveform::NullDriverWaveform;
  m_InspirationPatientTriggerFlow = nullptr;
  m_InspirationPatientTriggerPressure = nullptr;
  m_ExpirationWaveform = eDriverWaveform::NullDriverWaveform;
  m_ExpirationCycleFlow = nullptr;
  m_ExpirationCyclePressure = nullptr;
}

SEMechanicalVentilatorContinuousPositiveAirwayPressure::~SEMechanicalVentilatorContinuousPositiveAirwayPressure()
{
  SAFE_DELETE(m_DeltaPressureSupport);
  SAFE_DELETE(m_FractionInspiredOxygen);
  SAFE_DELETE(m_PositiveEndExpiratoryPressure);
  SAFE_DELETE(m_Slope);
  m_InspirationWaveform = eDriverWaveform::NullDriverWaveform;
  SAFE_DELETE(m_InspirationPatientTriggerFlow);
  SAFE_DELETE(m_InspirationPatientTriggerPressure);
  m_ExpirationWaveform = eDriverWaveform::NullDriverWaveform;
  SAFE_DELETE(m_ExpirationCycleFlow);
  SAFE_DELETE(m_ExpirationCyclePressure);
}

void SEMechanicalVentilatorContinuousPositiveAirwayPressure::Clear()
{
  SEMechanicalVentilatorMode::Clear();
  INVALIDATE_PROPERTY(m_DeltaPressureSupport);
  INVALIDATE_PROPERTY(m_FractionInspiredOxygen);
  INVALIDATE_PROPERTY(m_PositiveEndExpiratoryPressure);
  INVALIDATE_PROPERTY(m_Slope);
  m_InspirationWaveform = eDriverWaveform::NullDriverWaveform;
  INVALIDATE_PROPERTY(m_InspirationPatientTriggerFlow);
  INVALIDATE_PROPERTY(m_InspirationPatientTriggerPressure);
  m_ExpirationWaveform = eDriverWaveform::NullDriverWaveform;
  INVALIDATE_PROPERTY(m_ExpirationCycleFlow);
  INVALIDATE_PROPERTY(m_ExpirationCyclePressure);
}

void SEMechanicalVentilatorContinuousPositiveAirwayPressure::Copy(const SEMechanicalVentilatorContinuousPositiveAirwayPressure& src, const SESubstanceManager& subMgr, bool /*preserveState*/)
{// Using Bindings to make a copy
  PBEquipmentAction::Copy(src, *this, subMgr);
}

bool SEMechanicalVentilatorContinuousPositiveAirwayPressure::ToSettings(SEMechanicalVentilatorSettings& s, SESubstanceManager& subMgr, eMergeType mt)
{
  if (!SEMechanicalVentilatorMode::ToSettings(s, subMgr, mt))
    return false;
  if (SEMechanicalVentilatorMode::IsActive())
  {
    // Translate ventilator settings
    double inspirationWaveformPeriod_s = 0.0;
    if (HasSlope())// Optional
    {
      inspirationWaveformPeriod_s = GetSlope(TimeUnit::s);
    }

    double positiveEndExpiredPressure_cmH2O = GetPositiveEndExpiratoryPressure(PressureUnit::cmH2O);
    double peakInspiratoryPressure_cmH2O =
        positiveEndExpiredPressure_cmH2O + GetDeltaPressureSupport(PressureUnit::cmH2O);
    if (positiveEndExpiredPressure_cmH2O > peakInspiratoryPressure_cmH2O)
    {
        Error("Positive End Expired Pressure cannot be higher than the Peak Inspiratory Pressure.");
        return false;
    }
    s.GetInspirationWaveformPeriod().SetValue(inspirationWaveformPeriod_s, TimeUnit::s);
    s.GetPeakInspiratoryPressure().SetValue(peakInspiratoryPressure_cmH2O, PressureUnit::cmH2O);
    s.GetPositiveEndExpiratoryPressure().SetValue(positiveEndExpiredPressure_cmH2O, PressureUnit::cmH2O);
    s.GetFractionInspiredGas(*subMgr.GetSubstance("Oxygen")).GetFractionAmount().Set(GetFractionInspiredOxygen());

    // Optional Values (Transfer data, let the SEMechanicalVentilatorSettings class handle precedence)

    s.SetExpirationCycleRespiratoryModel(eSwitch::Off);
    if (HasExpirationCycleFlow())
      s.GetExpirationCycleFlow().Set(GetExpirationCycleFlow());
    if (HasExpirationCyclePressure())
      s.GetExpirationCyclePressure().Set(GetExpirationCyclePressure());
    if (!HasExpirationCycleFlow() && !HasExpirationCyclePressure())
      s.SetExpirationCycleRespiratoryModel(eSwitch::On);

    if (HasExpirationWaveform())
      s.SetExpirationWaveform(GetExpirationWaveform());
    else
      s.SetExpirationWaveform(eDriverWaveform::Square);

    s.SetInspirationPatientTriggerRespiratoryModel(eSwitch::Off);
    if (HasInspirationPatientTriggerFlow())
      s.GetInspirationPatientTriggerFlow().Set(GetInspirationPatientTriggerFlow());
    if (HasInspirationPatientTriggerPressure())
      s.GetInspirationPatientTriggerPressure().Set(GetInspirationPatientTriggerPressure());
    if (!HasInspirationPatientTriggerFlow() && !HasInspirationPatientTriggerPressure())
      s.SetInspirationPatientTriggerRespiratoryModel(eSwitch::On);

    if (HasInspirationWaveform())
      s.SetInspirationWaveform(GetInspirationWaveform());
    else
      s.SetInspirationWaveform(eDriverWaveform::Square);
  }
  return true;
}

bool SEMechanicalVentilatorContinuousPositiveAirwayPressure::IsValid() const
{
  if (!IsActive())
    return true;

  return SEMechanicalVentilatorMode::IsValid() &&
    HasDeltaPressureSupport() &&
    HasFractionInspiredOxygen() &&
    HasPositiveEndExpiratoryPressure();
    // Everything else is optional
}

bool SEMechanicalVentilatorContinuousPositiveAirwayPressure::IsActive() const
{
  return SEMechanicalVentilatorMode::IsActive();
}
void SEMechanicalVentilatorContinuousPositiveAirwayPressure::Deactivate()
{
  SEMechanicalVentilatorMode::Deactivate();
  Clear();//No stateful properties
}

const SEScalar* SEMechanicalVentilatorContinuousPositiveAirwayPressure::GetScalar(const std::string& name)
{
  if (name.compare("DeltaPressureSupport") == 0)
    return &GetDeltaPressureSupport();
  //if (name.compare("ExpirationWaveform") == 0)
  //  return &GetExpirationWaveform();
  if (name.compare("ExpirationCycleFlow") == 0)
    return &GetExpirationCycleFlow();
  if (name.compare("ExpirationCyclePressure") == 0)
    return &GetExpirationCyclePressure();
  if (name.compare("FractionInspiredOxygen") == 0)
    return &GetFractionInspiredOxygen();
  //if (name.compare("InspirationWaveform") == 0)
  //  return &GetInspirationWaveform();
  if (name.compare("InspirationPatientTriggerFlow") == 0)
    return &GetInspirationPatientTriggerFlow();
  if (name.compare("InspirationPatientTriggerPressure") == 0)
    return &GetInspirationPatientTriggerPressure();
  if (name.compare("PositiveEndExpiratoryPressure") == 0)
    return &GetPositiveEndExpiratoryPressure();
  if (name.compare("Slope") == 0)
    return &GetSlope();
  return nullptr;
}

bool SEMechanicalVentilatorContinuousPositiveAirwayPressure::HasDeltaPressureSupport() const
{
  return m_DeltaPressureSupport != nullptr;
}
SEScalarPressure& SEMechanicalVentilatorContinuousPositiveAirwayPressure::GetDeltaPressureSupport()
{
  if (m_DeltaPressureSupport == nullptr)
    m_DeltaPressureSupport = new SEScalarPressure();
  return *m_DeltaPressureSupport;
}
double SEMechanicalVentilatorContinuousPositiveAirwayPressure::GetDeltaPressureSupport(const PressureUnit& unit) const
{
  if (m_DeltaPressureSupport == nullptr)
    return SEScalar::dNaN();
  return m_DeltaPressureSupport->GetValue(unit);
}

bool SEMechanicalVentilatorContinuousPositiveAirwayPressure::HasFractionInspiredOxygen() const
{
  return m_FractionInspiredOxygen != nullptr;
}
SEScalar0To1& SEMechanicalVentilatorContinuousPositiveAirwayPressure::GetFractionInspiredOxygen()
{
  if (m_FractionInspiredOxygen == nullptr)
    m_FractionInspiredOxygen = new SEScalar0To1();
  return *m_FractionInspiredOxygen;
}
double SEMechanicalVentilatorContinuousPositiveAirwayPressure::GetFractionInspiredOxygen() const
{
  if (m_FractionInspiredOxygen == nullptr)
    return SEScalar::dNaN();
  return m_FractionInspiredOxygen->GetValue();
}

bool SEMechanicalVentilatorContinuousPositiveAirwayPressure::HasPositiveEndExpiratoryPressure() const
{
  return m_PositiveEndExpiratoryPressure != nullptr;
}
SEScalarPressure& SEMechanicalVentilatorContinuousPositiveAirwayPressure::GetPositiveEndExpiratoryPressure()
{
  if (m_PositiveEndExpiratoryPressure == nullptr)
    m_PositiveEndExpiratoryPressure = new SEScalarPressure();
  return *m_PositiveEndExpiratoryPressure;
}
double SEMechanicalVentilatorContinuousPositiveAirwayPressure::GetPositiveEndExpiratoryPressure(const PressureUnit& unit) const
{
  if (m_PositiveEndExpiratoryPressure == nullptr)
    return SEScalar::dNaN();
  return m_PositiveEndExpiratoryPressure->GetValue(unit);
}

bool SEMechanicalVentilatorContinuousPositiveAirwayPressure::HasSlope() const
{
  return m_Slope != nullptr;
}
SEScalarTime& SEMechanicalVentilatorContinuousPositiveAirwayPressure::GetSlope()
{
  if (m_Slope == nullptr)
    m_Slope = new SEScalarTime();
  return *m_Slope;
}
double SEMechanicalVentilatorContinuousPositiveAirwayPressure::GetSlope(const TimeUnit& unit) const
{
  if (m_Slope == nullptr)
    return SEScalar::dNaN();
  return m_Slope->GetValue(unit);
}

bool SEMechanicalVentilatorContinuousPositiveAirwayPressure::HasInspirationWaveform() const
{
  return m_InspirationWaveform != eDriverWaveform::NullDriverWaveform;
}
eDriverWaveform SEMechanicalVentilatorContinuousPositiveAirwayPressure::GetInspirationWaveform() const
{
  return m_InspirationWaveform;
}
void SEMechanicalVentilatorContinuousPositiveAirwayPressure::SetInspirationWaveform(eDriverWaveform w)
{
  m_InspirationWaveform = w;
}

bool SEMechanicalVentilatorContinuousPositiveAirwayPressure::HasInspirationPatientTriggerFlow() const
{
  return m_InspirationPatientTriggerFlow == nullptr ? false : m_InspirationPatientTriggerFlow->IsValid();
}
SEScalarVolumePerTime& SEMechanicalVentilatorContinuousPositiveAirwayPressure::GetInspirationPatientTriggerFlow()
{
  if (m_InspirationPatientTriggerFlow == nullptr)
    m_InspirationPatientTriggerFlow = new SEScalarVolumePerTime();
  return *m_InspirationPatientTriggerFlow;
}
double SEMechanicalVentilatorContinuousPositiveAirwayPressure::GetInspirationPatientTriggerFlow(const VolumePerTimeUnit& unit) const
{
  if (m_InspirationPatientTriggerFlow == nullptr)
    return SEScalar::dNaN();
  return m_InspirationPatientTriggerFlow->GetValue(unit);
}

bool SEMechanicalVentilatorContinuousPositiveAirwayPressure::HasInspirationPatientTriggerPressure() const
{
  return m_InspirationPatientTriggerPressure == nullptr ? false : m_InspirationPatientTriggerPressure->IsValid();
}
SEScalarPressure& SEMechanicalVentilatorContinuousPositiveAirwayPressure::GetInspirationPatientTriggerPressure()
{
  if (m_InspirationPatientTriggerPressure == nullptr)
    m_InspirationPatientTriggerPressure = new SEScalarPressure();
  return *m_InspirationPatientTriggerPressure;
}
double SEMechanicalVentilatorContinuousPositiveAirwayPressure::GetInspirationPatientTriggerPressure(const PressureUnit& unit) const
{
  if (m_InspirationPatientTriggerPressure == nullptr)
    return SEScalar::dNaN();
  return m_InspirationPatientTriggerPressure->GetValue(unit);
}

bool SEMechanicalVentilatorContinuousPositiveAirwayPressure::HasExpirationWaveform() const
{
  return m_ExpirationWaveform != eDriverWaveform::NullDriverWaveform;
}
eDriverWaveform SEMechanicalVentilatorContinuousPositiveAirwayPressure::GetExpirationWaveform() const
{
  return m_ExpirationWaveform;
}
void SEMechanicalVentilatorContinuousPositiveAirwayPressure::SetExpirationWaveform(eDriverWaveform w)
{
  m_ExpirationWaveform = w;
}

bool SEMechanicalVentilatorContinuousPositiveAirwayPressure::HasExpirationCycleFlow() const
{
  return m_ExpirationCycleFlow == nullptr ? false : m_ExpirationCycleFlow->IsValid();
}
SEScalarVolumePerTime& SEMechanicalVentilatorContinuousPositiveAirwayPressure::GetExpirationCycleFlow()
{
  if (m_ExpirationCycleFlow == nullptr)
    m_ExpirationCycleFlow = new SEScalarVolumePerTime();
  return *m_ExpirationCycleFlow;
}
double SEMechanicalVentilatorContinuousPositiveAirwayPressure::GetExpirationCycleFlow(const VolumePerTimeUnit& unit) const
{
  if (m_ExpirationCycleFlow == nullptr)
    return SEScalar::dNaN();
  return m_ExpirationCycleFlow->GetValue(unit);
}

bool SEMechanicalVentilatorContinuousPositiveAirwayPressure::HasExpirationCyclePressure() const
{
  return m_ExpirationCyclePressure == nullptr ? false : m_ExpirationCyclePressure->IsValid();
}
SEScalarPressure& SEMechanicalVentilatorContinuousPositiveAirwayPressure::GetExpirationCyclePressure()
{
  if (m_ExpirationCyclePressure == nullptr)
    m_ExpirationCyclePressure = new SEScalarPressure();
  return *m_ExpirationCyclePressure;
}
double SEMechanicalVentilatorContinuousPositiveAirwayPressure::GetExpirationCyclePressure(const PressureUnit& unit) const
{
  if (m_ExpirationCyclePressure == nullptr)
    return SEScalar::dNaN();
  return m_ExpirationCyclePressure->GetValue(unit);
}
