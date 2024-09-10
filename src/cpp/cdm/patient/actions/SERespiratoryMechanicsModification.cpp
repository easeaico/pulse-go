/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/patient/actions/SERespiratoryMechanicsModification.h"
#include "cdm/system/physiology/SERespiratoryMechanicsModifiers.h"
#include "cdm/io/protobuf/PBPatientActions.h"

SERespiratoryMechanicsModification::SERespiratoryMechanicsModification(Logger* logger) : SEPatientAction(logger)
{
  m_ModifiersFile = "";
  m_Modifiers = nullptr;
  m_Incremental = false;
}

SERespiratoryMechanicsModification::~SERespiratoryMechanicsModification()
{
  m_ModifiersFile = "";
  SAFE_DELETE(m_Modifiers);
  m_Incremental = false;
}

void SERespiratoryMechanicsModification::Clear()
{
  SEPatientAction::Clear();
  m_ModifiersFile = "";
  if (m_Modifiers)
    m_Modifiers->Clear();
  m_Incremental = false;
}

void SERespiratoryMechanicsModification::Copy(const SERespiratoryMechanicsModification& src, bool /*preserveState*/)
{
  //if(preserveState) // Cache any state before copy,
  PBPatientAction::Copy(src, *this);
  m_Incremental = src.m_Incremental;
  //if(preserveState) // Put back any state
}

bool SERespiratoryMechanicsModification::IsValid() const
{
  return SEPatientAction::IsValid() &&(HasModifiers() || HasModifiersFile());
}

bool SERespiratoryMechanicsModification::IsActive() const
{
  return SEPatientAction::IsActive();
}
void SERespiratoryMechanicsModification::Deactivate()
{
  SEPatientAction::Deactivate();
  Clear();//No stateful properties
}

const SEScalar* SERespiratoryMechanicsModification::GetScalar(const std::string& name)
{
  return GetModifiers().GetScalar(name);
}

bool SERespiratoryMechanicsModification::GetIncremental() const
{
  return m_Incremental;
}
void SERespiratoryMechanicsModification::SetIncremental(bool b)
{
  m_Incremental = b;
}

bool SERespiratoryMechanicsModification::HasModifiers() const
{
  return m_Modifiers != nullptr;
}
SERespiratoryMechanicsModifiers& SERespiratoryMechanicsModification::GetModifiers()
{
  if (m_Modifiers == nullptr)
    m_Modifiers = new SERespiratoryMechanicsModifiers(GetLogger());
  return *m_Modifiers;
}
const SERespiratoryMechanicsModifiers* SERespiratoryMechanicsModification::GetModifiers() const
{
  return m_Modifiers;
}

std::string SERespiratoryMechanicsModification::GetModifiersFile() const
{
  return m_ModifiersFile;
}
void SERespiratoryMechanicsModification::SetModifiersFile(const std::string& fileName)
{
  m_ModifiersFile = fileName;
}
bool SERespiratoryMechanicsModification::HasModifiersFile() const
{
  return !m_ModifiersFile.empty();
}
