/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include "cdm/system/SESystem.h"

class CDM_DECL SETissueSystem : public SESystem
{
  friend class PBPhysiology;//friend the serialization class
public:

  SETissueSystem(Logger* logger);
  virtual ~SETissueSystem();

  void Clear() override;// Deletes all members
  
  const SEScalar* GetScalar(const std::string& name) override;

  virtual bool HasCarbonDioxideProductionRate() const;
  virtual SEScalarVolumePerTime& GetCarbonDioxideProductionRate();
  virtual double GetCarbonDioxideProductionRate(const VolumePerTimeUnit& unit) const;

  virtual bool HasExtracellularFluidVolume() const;
  virtual SEScalarVolume& GetExtracellularFluidVolume();
  virtual double GetExtracellularFluidVolume(const VolumeUnit& unit) const;
 
  virtual bool HasExtravascularFluidVolume() const;
  virtual SEScalarVolume& GetExtravascularFluidVolume();
  virtual double GetExtravascularFluidVolume(const VolumeUnit& unit) const;

  virtual bool HasIntracellularFluidVolume() const;
  virtual SEScalarVolume& GetIntracellularFluidVolume();
  virtual double GetIntracellularFluidVolume(const VolumeUnit& unit) const;

  virtual bool HasIntracellularFluidPH() const;
  virtual SEScalar& GetIntracellularFluidPH();
  virtual double GetIntracellularFluidPH() const;

  virtual bool HasOxygenConsumptionRate() const;
  virtual SEScalarVolumePerTime& GetOxygenConsumptionRate();
  virtual double GetOxygenConsumptionRate(const VolumePerTimeUnit& unit) const;

  virtual bool HasRespiratoryExchangeRatio() const;
  virtual SEScalar& GetRespiratoryExchangeRatio();
  virtual double GetRespiratoryExchangeRatio() const;

  virtual bool HasTotalFluidVolume() const;
  virtual SEScalarVolume& GetTotalFluidVolume();
  virtual double GetTotalFluidVolume(const VolumeUnit& unit) const;


protected:
  SEScalarVolumePerTime*  m_CarbonDioxideProductionRate;
  SEScalarVolume*         m_ExtracellularFluidVolume;
  SEScalarVolume*         m_ExtravascularFluidVolume;
  SEScalarVolume*         m_IntracellularFluidVolume;
  SEScalar*               m_IntracellularFluidPH;
  SEScalarVolumePerTime*  m_OxygenConsumptionRate;
  SEScalar*               m_RespiratoryExchangeRatio;
  SEScalarVolume*         m_TotalFluidVolume;
};
