/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
class SEMechanicalVentilator;

class CDM_DECL SEMechanicalVentilatorAlarms : Loggable
{
  friend class PBMechanicalVentilator;//friend the serialization class
  friend SEMechanicalVentilator;
public:

  SEMechanicalVentilatorAlarms(Logger* logger);
  virtual ~SEMechanicalVentilatorAlarms();

  virtual void Clear();

  virtual const SEScalar* GetScalar(const std::string& name);

  virtual bool HasApneaTimeThreshold() const;
  virtual SEScalarTime& GetApneaTimeThreshold();
  virtual double GetApneaTimeThreshold(const TimeUnit& unit) const;

  virtual bool HasAutoPositiveEndExpiratoryPressureThreshold() const;
  virtual SEScalarPressure& GetAutoPositiveEndExpiratoryPressureThreshold();
  virtual double GetAutoPositiveEndExpiratoryPressureThreshold(const PressureUnit& unit) const;

  virtual bool HasCircuitLeakThreshold() const;
  virtual SEScalar0To1& GetCircuitLeakThreshold();
  virtual double GetCircuitLeakThreshold() const;

  virtual bool HasHighEndTidalCarbonDioxideThreshold() const;
  virtual SEScalarPressure& GetHighEndTidalCarbonDioxideThreshold();
  virtual double GetHighEndTidalCarbonDioxideThreshold(const PressureUnit& unit) const;

  virtual bool HasHighMinuteVentilationThreshold() const;
  virtual SEScalarVolumePerTime& GetHighMinuteVentilationThreshold();
  virtual double GetHighMinuteVentilationThreshold(const VolumePerTimeUnit& unit) const;

  virtual bool HasHighOxygenSaturationThreshold() const;
  virtual SEScalar0To1& GetHighOxygenSaturationThreshold();
  virtual double GetHighOxygenSaturationThreshold() const;

  virtual bool HasHighPositiveEndExpiratoryPressureThreshold() const;
  virtual SEScalarPressure& GetHighPositiveEndExpiratoryPressureThreshold();
  virtual double GetHighPositiveEndExpiratoryPressureThreshold(const PressureUnit& unit) const;

  virtual bool HasHighPressureCycleOption() const;
  virtual eSwitch GetHighPressureCycleOption() const;
  virtual void SetHighPressureCycleOption(eSwitch option);

  virtual bool HasHighPressureThreshold() const;
  virtual SEScalarPressure& GetHighPressureThreshold();
  virtual double GetHighPressureThreshold(const PressureUnit& unit) const;

  virtual bool HasHighRespiratoryRateThreshold() const;
  virtual SEScalarFrequency& GetHighRespiratoryRateThreshold();
  virtual double GetHighRespiratoryRateThreshold(const FrequencyUnit& unit) const;

  virtual bool HasHighTidalVolumeThreshold() const;
  virtual SEScalarVolume& GetHighTidalVolumeThreshold();
  virtual double GetHighTidalVolumeThreshold(const VolumeUnit& unit) const;

  virtual bool HasLowEndTidalCarbonDioxideThreshold() const;
  virtual SEScalarPressure& GetLowEndTidalCarbonDioxideThreshold();
  virtual double GetLowEndTidalCarbonDioxideThreshold(const PressureUnit& unit) const;

  virtual bool HasLowMinuteVentilationThreshold() const;
  virtual SEScalarVolumePerTime& GetLowMinuteVentilationThreshold();
  virtual double GetLowMinuteVentilationThreshold(const VolumePerTimeUnit& unit) const;

  virtual bool HasLowOxygenSaturationThreshold() const;
  virtual SEScalar0To1& GetLowOxygenSaturationThreshold();
  virtual double GetLowOxygenSaturationThreshold() const;

  virtual bool HasLowPositiveEndExpiratoryPressureThreshold() const;
  virtual SEScalarPressure& GetLowPositiveEndExpiratoryPressureThreshold();
  virtual double GetLowPositiveEndExpiratoryPressureThreshold(const PressureUnit& unit) const;

  virtual bool HasLowPressureThreshold() const;
  virtual SEScalarPressure& GetLowPressureThreshold();
  virtual double GetLowPressureThreshold(const PressureUnit& unit) const;

  virtual bool HasLowTidalVolumeThreshold() const;
  virtual SEScalarVolume& GetLowTidalVolumeThreshold();
  virtual double GetLowTidalVolumeThreshold(const VolumeUnit& unit) const;

  virtual bool HasOxygenSupplyFailureThreshold() const;
  virtual SEScalar0To1& GetOxygenSupplyFailureThreshold();
  virtual double GetOxygenSupplyFailureThreshold() const;

protected:
  SEScalarTime*               m_ApneaTimeThreshold;
  SEScalarPressure*           m_AutoPositiveEndExpiratoryPressureThreshold;
  SEScalar0To1*               m_CircuitLeakThreshold;
  SEScalarPressure*           m_HighEndTidalCarbonDioxideThreshold;
  SEScalarVolumePerTime*      m_HighMinuteVentilationThreshold;
  SEScalar0To1*               m_HighOxygenSaturationThreshold;
  SEScalarPressure*           m_HighPositiveEndExpiratoryPressureThreshold;
  eSwitch                     m_HighPressureCycleOption;
  SEScalarPressure*           m_HighPressureThreshold;
  SEScalarFrequency*          m_HighRespiratoryRateThreshold;
  SEScalarVolume*             m_HighTidalVolumeThreshold;
  SEScalarPressure*           m_LowEndTidalCarbonDioxideThreshold;
  SEScalarVolumePerTime*      m_LowMinuteVentilationThreshold;
  SEScalar0To1*               m_LowOxygenSaturationThreshold;
  SEScalarPressure*           m_LowPositiveEndExpiratoryPressureThreshold;
  SEScalarPressure*           m_LowPressureThreshold;
  SEScalarVolume*             m_LowTidalVolumeThreshold;
  SEScalar0To1*               m_OxygenSupplyFailureThreshold;
};
