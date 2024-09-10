/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include "cdm/patient/conditions/SEPatientCondition.h"

class CDM_DECL SEDehydration : public SEPatientCondition
{
  friend class PBPatientCondition;//friend the serialization class
public:

  SEDehydration(Logger* logger=nullptr);
  virtual ~SEDehydration();

  static constexpr char const* Name = "Dehydration";
  std::string GetName() const override { return Name; }

  void Clear() override;
  void Copy(const SEDehydration& src);

  bool IsValid() const override;
  bool IsActive() const override;

  virtual bool HasSeverity() const;
  virtual SEScalar0To1& GetSeverity();
  virtual double GetSeverity() const;

protected:
  SEScalar0To1*           m_Severity;
};
