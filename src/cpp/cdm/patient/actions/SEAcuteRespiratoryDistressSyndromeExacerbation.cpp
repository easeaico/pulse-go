/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/patient/actions/SEAcuteRespiratoryDistressSyndromeExacerbation.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/io/protobuf/PBPatientActions.h"

SEAcuteRespiratoryDistressSyndromeExacerbation::SEAcuteRespiratoryDistressSyndromeExacerbation(Logger* logger) : SEPatientAction(logger)
{

}

SEAcuteRespiratoryDistressSyndromeExacerbation::~SEAcuteRespiratoryDistressSyndromeExacerbation()
{
  DELETE_MAP_SECOND(m_Severities);
}

void SEAcuteRespiratoryDistressSyndromeExacerbation::Clear()
{
  SEPatientAction::Clear();
  for (auto itr : m_Severities)
    if (itr.second != nullptr)
      itr.second->Invalidate();
}

void SEAcuteRespiratoryDistressSyndromeExacerbation::Copy(const SEAcuteRespiratoryDistressSyndromeExacerbation& src, bool /*preserveState*/)
{
  //if(preserveState) // Cache any state before copy,
  PBPatientAction::Copy(src, *this);
  //if(preserveState) // Put back any state
}

bool SEAcuteRespiratoryDistressSyndromeExacerbation::IsValid() const
{
  if (!SEPatientAction::IsValid())
    return false;
  return HasSeverity();
}
bool SEAcuteRespiratoryDistressSyndromeExacerbation::IsActive() const
{
  if (!SEPatientAction::IsActive())
    return false;
  if (!HasSeverity())
    return false;
  return true;
}
void SEAcuteRespiratoryDistressSyndromeExacerbation::Activate()
{
  SEPatientAction::Activate();
  SERespiratorySystem::FillLungImpairmentMap(m_Severities);
}
void SEAcuteRespiratoryDistressSyndromeExacerbation::Deactivate()
{
  SEPatientAction::Deactivate();
  Clear();//No stateful properties
}

const SEScalar* SEAcuteRespiratoryDistressSyndromeExacerbation::GetScalar(const std::string& /*name*/)
{
  return nullptr;
}

bool SEAcuteRespiratoryDistressSyndromeExacerbation::HasSeverity() const
{
  for (auto itr : m_Severities)
    if (itr.second != nullptr && itr.second->IsValid())
      return true;
  return false;
}
LungImpairmentMap& SEAcuteRespiratoryDistressSyndromeExacerbation::GetSeverities()
{
  return m_Severities;
}
const LungImpairmentMap& SEAcuteRespiratoryDistressSyndromeExacerbation::GetSeverities() const
{
  return m_Severities;
}

bool SEAcuteRespiratoryDistressSyndromeExacerbation::HasSeverity(eLungCompartment cmpt) const
{
  auto s = m_Severities.find(cmpt);
  if (s == m_Severities.end())
    return false;
  if (s->second == nullptr)
    return false;
  return s->second->IsValid();
}
SEScalar0To1& SEAcuteRespiratoryDistressSyndromeExacerbation::GetSeverity(eLungCompartment cmpt)
{
  SEScalar0To1* s = m_Severities[cmpt];
  if (s == nullptr)
  {
    s = new SEScalar0To1();
    m_Severities[cmpt] = s;
  }
  return *s;
}
double SEAcuteRespiratoryDistressSyndromeExacerbation::GetSeverity(eLungCompartment cmpt) const
{
  auto s = m_Severities.find(cmpt);
  if (s == m_Severities.end())
    return SEScalar::dNaN();
  return s->second->GetValue();
}
