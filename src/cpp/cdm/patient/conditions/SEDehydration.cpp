/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/patient/conditions/SEDehydration.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/io/protobuf/PBPatientConditions.h"

SEDehydration::SEDehydration(Logger* logger) : SEPatientCondition(logger)
{
  m_Severity=nullptr;
}

SEDehydration::~SEDehydration()
{
  SAFE_DELETE(m_Severity);
}

void SEDehydration::Clear()
{
  SEPatientCondition::Clear();
  INVALIDATE_PROPERTY(m_Severity);
}

void SEDehydration::Copy(const SEDehydration& src)
{
  PBPatientCondition::Copy(src, *this);
}

bool SEDehydration::IsValid() const
{
  return HasSeverity();
}
bool SEDehydration::IsActive() const
{
  if (!IsValid())
    return false;
  return GetSeverity() > 0;
}

bool SEDehydration::HasSeverity() const
{
  return m_Severity==nullptr?false:m_Severity->IsValid();
}

SEScalar0To1& SEDehydration::GetSeverity()
{
  if(m_Severity==nullptr)
    m_Severity=new SEScalar0To1();
  return *m_Severity;
}
double SEDehydration::GetSeverity() const
{
  if (m_Severity == nullptr)
    return SEScalar::dNaN();
  return m_Severity->GetValue();
}
