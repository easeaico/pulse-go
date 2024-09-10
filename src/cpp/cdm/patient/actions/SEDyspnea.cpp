/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/patient/actions/SEDyspnea.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/io/protobuf/PBPatientActions.h"

SEDyspnea::SEDyspnea(Logger* logger) : SEPatientAction(logger)
{
  m_RespirationRateSeverity = nullptr;
  m_TidalVolumeSeverity = nullptr;
}

SEDyspnea::~SEDyspnea()
{
  SAFE_DELETE(m_RespirationRateSeverity);
  SAFE_DELETE(m_TidalVolumeSeverity);
}

void SEDyspnea::Clear()
{
  SEPatientAction::Clear();
  INVALIDATE_PROPERTY(m_RespirationRateSeverity);
  INVALIDATE_PROPERTY(m_TidalVolumeSeverity);
}

void SEDyspnea::Copy(const SEDyspnea& src, bool /*preserveState*/)
{
  //if(preserveState) // Cache any state before copy,
  PBPatientAction::Copy(src, *this);
  //if(preserveState) // Put back any state
}

bool SEDyspnea::IsValid() const
{
  return SEPatientAction::IsValid() &&
    (HasRespirationRateSeverity() || HasTidalVolumeSeverity());
}

bool SEDyspnea::IsActive() const
{
  if (!SEPatientAction::IsActive())
    return false;
  if (HasRespirationRateSeverity() && GetRespirationRateSeverity() > 0)
    return true;
  if (HasTidalVolumeSeverity() && GetTidalVolumeSeverity() > 0)
    return true;
  return false;
}
void SEDyspnea::Deactivate()
{
  SEPatientAction::Deactivate();
  Clear();//No stateful properties
}

const SEScalar* SEDyspnea::GetScalar(const std::string& name)
{
  if (name.compare("RespirationRateSeverity") == 0)
    return &GetRespirationRateSeverity();
  if (name.compare("TidalVolumeSeverity") == 0)
    return &GetTidalVolumeSeverity();
  return nullptr;
}

bool SEDyspnea::HasRespirationRateSeverity() const
{
  return m_RespirationRateSeverity ==nullptr?false:m_RespirationRateSeverity->IsValid();
}
SEScalar0To1& SEDyspnea::GetRespirationRateSeverity()
{
  if(m_RespirationRateSeverity ==nullptr)
    m_RespirationRateSeverity =new SEScalar0To1();
  return *m_RespirationRateSeverity;
}
double SEDyspnea::GetRespirationRateSeverity() const
{
  if (m_RespirationRateSeverity == nullptr)
    return SEScalar::dNaN();
  return m_RespirationRateSeverity->GetValue();
}

bool SEDyspnea::HasTidalVolumeSeverity() const
{
  return m_TidalVolumeSeverity == nullptr ? false : m_TidalVolumeSeverity->IsValid();
}
SEScalar0To1& SEDyspnea::GetTidalVolumeSeverity()
{
  if (m_TidalVolumeSeverity == nullptr)
    m_TidalVolumeSeverity = new SEScalar0To1();
  return *m_TidalVolumeSeverity;
}
double SEDyspnea::GetTidalVolumeSeverity() const
{
  if (m_TidalVolumeSeverity == nullptr)
    return SEScalar::dNaN();
  return m_TidalVolumeSeverity->GetValue();
}
