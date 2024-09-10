/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#pragma once
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorMode.h"
class SEMechanicalVentilator;
class SESubstanceManager;

class CDM_DECL SEMechanicalVentilatorContinuousPositiveAirwayPressure : public SEMechanicalVentilatorMode
{
  friend class PBEquipmentAction;//friend the serialization class
public:

  SEMechanicalVentilatorContinuousPositiveAirwayPressure(Logger* logger = nullptr);
  virtual ~SEMechanicalVentilatorContinuousPositiveAirwayPressure();

  static constexpr char const* Name = "CPAP";
  std::string GetName() const override { return Name; }

  void Clear() override;
  virtual void Copy(const SEMechanicalVentilatorContinuousPositiveAirwayPressure& src, bool /*preserveState*/ = false);
  virtual bool ToSettings(SEMechanicalVentilatorSettings& s, const SESubstanceManager& subMgr) override;

  bool IsValid() const override;
  bool IsActive() const override;
  void Deactivate() override;

  virtual bool HasDeltaPressureSupport() const;
  virtual SEScalarPressure& GetDeltaPressureSupport();
  virtual double GetDeltaPressureSupport(const PressureUnit& unit) const;

  virtual bool HasExpirationWaveform() const;
  virtual eDriverWaveform GetExpirationWaveform() const;
  virtual void SetExpirationWaveform(eDriverWaveform w);

  virtual bool HasExpirationCycleFlow() const;
  virtual SEScalarVolumePerTime& GetExpirationCycleFlow();
  virtual double GetExpirationCycleFlow(const VolumePerTimeUnit& unit) const;

  virtual bool HasExpirationCyclePressure() const;
  virtual SEScalarPressure& GetExpirationCyclePressure();
  virtual double GetExpirationCyclePressure(const PressureUnit& unit) const;

  virtual bool HasFractionInspiredOxygen() const;
  virtual SEScalar0To1& GetFractionInspiredOxygen();
  virtual double GetFractionInspiredOxygen() const;

  virtual bool HasInspirationWaveform() const;
  virtual eDriverWaveform GetInspirationWaveform() const;
  virtual void SetInspirationWaveform(eDriverWaveform w);

  virtual bool HasInspirationPatientTriggerFlow() const;
  virtual SEScalarVolumePerTime& GetInspirationPatientTriggerFlow();
  virtual double GetInspirationPatientTriggerFlow(const VolumePerTimeUnit& unit) const;

  virtual bool HasInspirationPatientTriggerPressure() const;
  virtual SEScalarPressure& GetInspirationPatientTriggerPressure();
  virtual double GetInspirationPatientTriggerPressure(const PressureUnit& unit) const;

  virtual bool HasPositiveEndExpiratoryPressure() const;
  virtual SEScalarPressure& GetPositiveEndExpiratoryPressure();
  virtual double GetPositiveEndExpiratoryPressure(const PressureUnit& unit) const;

  virtual bool HasSlope() const;
  virtual SEScalarTime& GetSlope();
  virtual double GetSlope(const TimeUnit& unit) const;

  const SEScalar* GetScalar(const std::string& name) override;

protected:

  SEScalarPressure*                    m_DeltaPressureSupport;
  eDriverWaveform                      m_ExpirationWaveform;
  SEScalarVolumePerTime*               m_ExpirationCycleFlow;
  SEScalarPressure*                    m_ExpirationCyclePressure;
  SEScalar0To1*                        m_FractionInspiredOxygen;
  eDriverWaveform                      m_InspirationWaveform;
  SEScalarVolumePerTime*               m_InspirationPatientTriggerFlow;
  SEScalarPressure*                    m_InspirationPatientTriggerPressure;
  SEScalarPressure*                    m_PositiveEndExpiratoryPressure;
  SEScalarTime*                        m_Slope;
};
