/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include "cdm/patient/actions/SEPatientAction.h"

class CDM_DECL SEAirwayObstruction : public SEPatientAction
{
  friend class PBPatientAction;//friend the serialization class
public:

  SEAirwayObstruction(Logger* logger=nullptr);
  virtual ~SEAirwayObstruction();

  static constexpr char const* Name = "Airway Obstruction";
  std::string GetName() const override { return Name; }

  void Clear() override;
  void Copy(const SEAirwayObstruction& src, bool /*preserveState*/=false);

  const SEScalar* GetScalar(const std::string& name) override;

  bool IsValid() const override;
  bool IsActive() const override;
  void Deactivate() override;

  virtual bool HasSeverity() const;
  virtual SEScalar0To1& GetSeverity();
  virtual double GetSeverity() const;

  virtual bool GetHasSecretions() const;
  virtual void SetHasSecretions(bool hasSecretions);

protected:
  SEScalar0To1*           m_Severity;
  bool                    m_HasSecretions;
};
