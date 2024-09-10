/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include "cdm/properties/SEScalar.h"

class CDM_DECL SEScalarUnsigned : public SEScalar
{

public:
  SEScalarUnsigned();
  virtual ~SEScalarUnsigned() {}

  double GetValue() const { return SEScalar::GetValue(); }
  double GetValue(const NoUnit& /*unitless*/) const { return SEScalar::GetValue(); }
  void SetValue(double d);
  void SetValue(double d, const NoUnit& unitless);
  void ForceValue(double d);
  void ForceValue(double d, const NoUnit& unitless);
};
