/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/patient/actions/SEPneumoniaExacerbation.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/io/protobuf/PBPatientActions.h"

SEPneumoniaExacerbation::SEPneumoniaExacerbation(Logger* logger) : SEPatientAction(logger)
{

}

SEPneumoniaExacerbation::~SEPneumoniaExacerbation()
{
  DELETE_MAP_SECOND(m_Severities);
}

void SEPneumoniaExacerbation::Clear()
{
  SEPatientAction::Clear();
  for (auto itr : m_Severities)
    if (itr.second != nullptr)
      itr.second->Invalidate();
}

void SEPneumoniaExacerbation::Copy(const SEPneumoniaExacerbation& src, bool /*preserveState*/)
{
  //if(preserveState) // Cache any state before copy,
  PBPatientAction::Copy(src, *this);
  //if(preserveState) // Put back any state
}

bool SEPneumoniaExacerbation::IsValid() const
{
  return HasSeverity();
}

bool SEPneumoniaExacerbation::IsActive() const
{
  if (!SEPatientAction::IsActive())
    return false;
  if (!HasSeverity())
    return false;
  return true;
}
void SEPneumoniaExacerbation::Activate()
{
  SEPatientAction::Activate();
  SERespiratorySystem::FillLungImpairmentMap(m_Severities);
}
void SEPneumoniaExacerbation::Deactivate()
{
  SEPatientAction::Deactivate();
  Clear();//No stateful properties
}

const SEScalar* SEPneumoniaExacerbation::GetScalar(const std::string& /*name*/)
{
  return nullptr;
}

bool SEPneumoniaExacerbation::HasSeverity() const
{
  for (auto itr : m_Severities)
    if (itr.second != nullptr && itr.second->IsValid())
      return true;
  return false;
}
LungImpairmentMap& SEPneumoniaExacerbation::GetSeverities()
{
  return m_Severities;
}
const LungImpairmentMap& SEPneumoniaExacerbation::GetSeverities() const
{
  return m_Severities;
}

bool SEPneumoniaExacerbation::HasSeverity(eLungCompartment cmpt) const
{
  auto s = m_Severities.find(cmpt);
  if (s == m_Severities.end())
    return false;
  if (s->second == nullptr)
    return false;
  return s->second->IsValid();
}
SEScalar0To1& SEPneumoniaExacerbation::GetSeverity(eLungCompartment cmpt)
{
  SEScalar0To1* s = m_Severities[cmpt];
  if (s == nullptr)
  {
    s = new SEScalar0To1();
    m_Severities[cmpt] = s;
  }
  return *s;
}
double SEPneumoniaExacerbation::GetSeverity(eLungCompartment cmpt) const
{
  auto s = m_Severities.find(cmpt);
  if (s == m_Severities.end())
    return SEScalar::dNaN();
  return s->second->GetValue();
}
