/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/patient/actions/SEPrimaryBlastLungInjury.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/io/protobuf/PBPatientActions.h"

SEPrimaryBlastLungInjury::SEPrimaryBlastLungInjury(Logger* logger) : SEPatientAction(logger)
{

}

SEPrimaryBlastLungInjury::~SEPrimaryBlastLungInjury()
{
  DELETE_MAP_SECOND(m_Severities);
}

void SEPrimaryBlastLungInjury::Clear()
{
  SEPatientAction::Clear();
  for (auto itr : m_Severities)
    if (itr.second != nullptr)
      itr.second->Invalidate();
}

void SEPrimaryBlastLungInjury::Copy(const SEPrimaryBlastLungInjury& src, bool /*preserveState*/)
{
  //if(preserveState) // Cache any state before copy,
  PBPatientAction::Copy(src, *this);
  //if(preserveState) // Put back any state
}

bool SEPrimaryBlastLungInjury::IsValid() const
{
  if (!SEPatientAction::IsValid())
    return false;
  return HasSeverity();
}
bool SEPrimaryBlastLungInjury::IsActive() const
{
  if (!SEPatientAction::IsActive())
    return false;
  if (!HasSeverity())
    return false;
  return true;
}
void SEPrimaryBlastLungInjury::Activate()
{
  SEPatientAction::Activate();
  SERespiratorySystem::FillLungImpairmentMap(m_Severities);
}
void SEPrimaryBlastLungInjury::Deactivate()
{
  SEPatientAction::Deactivate();
  Clear();//No stateful properties
}

const SEScalar* SEPrimaryBlastLungInjury::GetScalar(const std::string& /*name*/)
{
  return nullptr;
}

bool SEPrimaryBlastLungInjury::HasSeverity() const
{
  for (auto itr : m_Severities)
    if (itr.second != nullptr && itr.second->IsValid())
      return true;
  return false;
}
LungImpairmentMap& SEPrimaryBlastLungInjury::GetSeverities()
{
  return m_Severities;
}
const LungImpairmentMap& SEPrimaryBlastLungInjury::GetSeverities() const
{
  return m_Severities;
}

bool SEPrimaryBlastLungInjury::HasSeverity(eLungCompartment cmpt) const
{
  auto s = m_Severities.find(cmpt);
  if (s == m_Severities.end())
    return false;
  if (s->second == nullptr)
    return false;
  return s->second->IsValid();
}
SEScalar0To1& SEPrimaryBlastLungInjury::GetSeverity(eLungCompartment cmpt)
{
  SEScalar0To1* s = m_Severities[cmpt];
  if (s == nullptr)
  {
    s = new SEScalar0To1();
    m_Severities[cmpt] = s;
  }
  return *s;
}
double SEPrimaryBlastLungInjury::GetSeverity(eLungCompartment cmpt) const
{
  auto s = m_Severities.find(cmpt);
  if (s == m_Severities.end())
    return SEScalar::dNaN();
  return s->second->GetValue();
}
