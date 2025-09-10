/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include "cdm/patient/actions/SEPatientAction.h"
#include "cdm/system/physiology/SERespiratorySystem.h"

class CDM_DECL SEPrimaryBlastLungInjury : public SEPatientAction
{
  friend class PBPatientAction;//friend the serialization class
public:

  SEPrimaryBlastLungInjury(Logger* logger=nullptr);
  virtual ~SEPrimaryBlastLungInjury();

  static constexpr char const* Name = "ARDS Exacerbation";
  std::string GetName() const override { return Name; }

  void Clear() override;
  void Copy(const SEPrimaryBlastLungInjury& src, bool preserveState=false);

  const SEScalar* GetScalar(const std::string& name) override;

  bool IsValid() const override;
  bool IsActive() const override;
  void Activate() override;
  void Deactivate() override;

  virtual bool HasSeverity() const;
  virtual LungImpairmentMap& GetSeverities();
  virtual const LungImpairmentMap& GetSeverities() const;
  virtual bool HasSeverity(eLungCompartment cmpt) const;
  virtual SEScalar0To1& GetSeverity(eLungCompartment cmpt);
  virtual double GetSeverity(eLungCompartment cmpt) const;

protected:
  LungImpairmentMap m_Severities;
};