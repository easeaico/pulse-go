/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include "cdm/patient/actions/SEPatientAction.h"
class Serializer;
class SESubstance;
class SESubstanceFraction;
class SESubstanceConcentration;
class SEMechanicalVentilationConfiguration;

class CDM_DECL SEMechanicalVentilation : public SEPatientAction
{
  friend class PBPatientAction;//friend the serialization class
protected:

public:

  SEMechanicalVentilation(Logger* logger=nullptr);
  virtual ~SEMechanicalVentilation();

  static constexpr char const* Name = "Mechanical Ventilation";
  std::string GetName() const override { return Name; }

  void Clear() override;
  void Copy(const SEMechanicalVentilation& src, const SESubstanceManager& subMgr, bool /*preserveState*/=false);

  const SEScalar* GetScalar(const std::string& name) override;

  bool IsValid() const override;
  bool IsActive() const override;
  void Deactivate() override;

  virtual eSwitch GetState() const;
  virtual void SetState(eSwitch name);

  virtual bool HasFlow() const;
  virtual SEScalarVolumePerTime& GetFlow();
  virtual double GetFlow(const VolumePerTimeUnit& unit) const;

  virtual bool HasPressure() const;
  virtual SEScalarPressure& GetPressure();
  virtual double GetPressure(const PressureUnit& unit) const;

  virtual bool HasMechanicalDeadSpaceVolume() const;
  virtual SEScalarVolume& GetMechanicalDeadSpaceVolume();
  virtual double GetMechanicalDeadSpaceVolume(const VolumeUnit& unit) const;

  bool HasGasFraction() const;
  bool HasGasFraction(const SESubstance& substance) const;
  const std::vector<SESubstanceFraction*>& GetGasFractions();
  const std::vector<const SESubstanceFraction*>& GetGasFractions() const;
  SESubstanceFraction& GetGasFraction(const SESubstance& substance);
  const SESubstanceFraction* GetGasFraction(const SESubstance& substance) const;
  void RemoveGasFraction(const SESubstance& substance);
  void RemoveGasFractions();

  bool HasAerosol() const;
  bool HasAerosol(const SESubstance& substance) const;
  const std::vector<SESubstanceConcentration*>& GetAerosols();
  const std::vector<const SESubstanceConcentration*>& GetAerosols() const;
  SESubstanceConcentration& GetAerosol(const SESubstance& substance);
  const SESubstanceConcentration* GetAerosol(const SESubstance& substance) const;
  void RemoveAerosol(const SESubstance& substance);
  void RemoveAerosols();

protected:

  eSwitch                 m_State;
  SEScalarVolumePerTime*  m_Flow;
  SEScalarPressure*       m_Pressure;
  SEScalarVolume*         m_MechanicalDeadSpaceVolume;

  std::vector<SESubstanceFraction*>       m_GasFractions;
  std::vector<const SESubstanceFraction*> m_cGasFractions;

  std::vector<SESubstanceConcentration*>       m_Aerosols;
  std::vector<const SESubstanceConcentration*> m_cAerosols;
};
