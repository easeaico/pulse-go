/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

#include "PatientIteration.h"
#include "cdm/engine/SEAdvanceTime.h"

namespace pulse::study::patient_variability
{
  class ValidationIteration : public PatientIteration
  {
  public:
    ValidationIteration(Logger& logger);
    virtual ~ValidationIteration();

    void Clear() override;

  protected:

    // Stateless
    SEAdvanceTime         m_Adv;
  };
}
