/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/patient/actions/SEAirwayObstruction.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/io/protobuf/PBPatientActions.h"

SEAirwayObstruction::SEAirwayObstruction(Logger* logger) : SEPatientAction(logger)
{
  m_Severity=nullptr;
  m_ResistanceType = eAirwayObstruction_ResistanceType::Constant;
}

SEAirwayObstruction::~SEAirwayObstruction()
{
  SAFE_DELETE(m_Severity);
}

void SEAirwayObstruction::Clear()
{
  SEPatientAction::Clear();
  INVALIDATE_PROPERTY(m_Severity);
  m_ResistanceType = eAirwayObstruction_ResistanceType::Constant;
}

void SEAirwayObstruction::Copy(const SEAirwayObstruction& src, bool /*preserveState*/)
{
  //if(preserveState) // Cache any state before copy,
  PBPatientAction::Copy(src, *this);
  //if(preserveState) // Put back any state
}

bool SEAirwayObstruction::IsValid() const
{
  return SEPatientAction::IsValid() && HasSeverity();
}

bool SEAirwayObstruction::IsActive() const
{
  if (!SEPatientAction::IsActive())
    return false;
  return !m_Severity->IsZero();
}
void SEAirwayObstruction::Deactivate()
{
  SEPatientAction::Deactivate();
  Clear();//No stateful properties
}

const SEScalar* SEAirwayObstruction::GetScalar(const std::string& name)
{
  if (name.compare("Severity") == 0)
    return &GetSeverity();
  return nullptr;
}

bool SEAirwayObstruction::HasSeverity() const
{
  return m_Severity==nullptr?false:m_Severity->IsValid();
}
SEScalar0To1& SEAirwayObstruction::GetSeverity()
{
  if(m_Severity==nullptr)
    m_Severity=new SEScalar0To1();
  return *m_Severity;
}
double SEAirwayObstruction::GetSeverity() const
{
  if (m_Severity == nullptr)
    return SEScalar::dNaN();
  return m_Severity->GetValue();
}

eAirwayObstruction_ResistanceType SEAirwayObstruction::GetResistanceType() const
{
  return m_ResistanceType;
}
void SEAirwayObstruction::SetResistanceType(eAirwayObstruction_ResistanceType rt)
{
  m_ResistanceType = rt;
}
