/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/engine/SEEngineStabilization.h"
#include "cdm/properties/SEScalarTime.h"

SEEngineStabilization::SEEngineStabilization(Logger *logger) : Loggable(logger)
{
  m_Cancelled = true;
  m_LogProgress = true;
  m_StabilizationDuration = nullptr;
}

SEEngineStabilization::~SEEngineStabilization()
{
  Clear();
}

void SEEngineStabilization::Clear()
{
  m_LogProgress = true;
  SAFE_DELETE(m_StabilizationDuration);
}

void SEEngineStabilization::LogProgress(bool b)
{
  m_LogProgress = b;
}

void SEEngineStabilization::CancelStabilization()
{
  m_Cancelled = true;
}

bool SEEngineStabilization::HasStabilizationDuration()
{
  return m_StabilizationDuration == nullptr ? false : m_StabilizationDuration->IsValid();
}
SEScalarTime& SEEngineStabilization::GetStabilizationDuration()
{
  if (m_StabilizationDuration == nullptr)
    m_StabilizationDuration = new SEScalarTime();
  return *m_StabilizationDuration;
}