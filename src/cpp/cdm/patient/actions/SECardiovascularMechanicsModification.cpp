/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/patient/actions/SECardiovascularMechanicsModification.h"
#include "cdm/system/physiology/SECardiovascularMechanicsModifiers.h"
#include "cdm/io/protobuf/PBPatientActions.h"

SECardiovascularMechanicsModification::SECardiovascularMechanicsModification(Logger* logger) : SEPatientAction(logger)
{
  m_ModifiersFile = "";
  m_Modifiers = nullptr;
  m_Incremental = false;
}

SECardiovascularMechanicsModification::~SECardiovascularMechanicsModification()
{
  m_ModifiersFile = "";
  SAFE_DELETE(m_Modifiers);
  m_Incremental = false;
}

void SECardiovascularMechanicsModification::Clear()
{
  SEPatientAction::Clear();
  m_ModifiersFile = "";
  if (m_Modifiers)
    m_Modifiers->Clear();
  m_Incremental = false;
}

void SECardiovascularMechanicsModification::Copy(const SECardiovascularMechanicsModification& src, bool /*preserveState*/)
{
  //if(preserveState) // Cache any state before copy,
  PBPatientAction::Copy(src, *this);
  m_Incremental = src.m_Incremental;
  //if(preserveState) // Put back any state
}

void SECardiovascularMechanicsModification::Activate()
{
  SEPatientAction::Activate();
  if (HasModifiersFile())
  {
    if (!GetModifiers().SerializeFromFile(GetModifiersFile()))
      Error("Unable to serialize cardiovascular modifier file: " + GetModifiersFile());
    m_ModifiersFile = "";
  }
  GetModifiers().Activate();
}
bool SECardiovascularMechanicsModification::IsValid() const
{
  return SEPatientAction::IsValid() &&(HasModifiers() || HasModifiersFile());
}

bool SECardiovascularMechanicsModification::IsActive() const
{
  return SEPatientAction::IsActive() && GetModifiers()->IsActive();
}
void SECardiovascularMechanicsModification::Deactivate()
{
  SEPatientAction::Deactivate();
  Clear();//No stateful properties
  GetModifiers().Activate();
}

const SEScalar* SECardiovascularMechanicsModification::GetScalar(const std::string& name)
{
  return GetModifiers().GetScalar(name);
}

bool SECardiovascularMechanicsModification::GetIncremental() const
{
  return m_Incremental;
}
void SECardiovascularMechanicsModification::SetIncremental(bool b)
{
  m_Incremental = b;
}

bool SECardiovascularMechanicsModification::HasModifiers() const
{
  return m_Modifiers != nullptr;
}
SECardiovascularMechanicsModifiers& SECardiovascularMechanicsModification::GetModifiers()
{
  if (m_Modifiers == nullptr)
    m_Modifiers = new SECardiovascularMechanicsModifiers(GetLogger());
  return *m_Modifiers;
}
const SECardiovascularMechanicsModifiers* SECardiovascularMechanicsModification::GetModifiers() const
{
  return m_Modifiers;
}

std::string SECardiovascularMechanicsModification::GetModifiersFile() const
{
  return m_ModifiersFile;
}
void SECardiovascularMechanicsModification::SetModifiersFile(const std::string& fileName)
{
  m_ModifiersFile = fileName;
}
bool SECardiovascularMechanicsModification::HasModifiersFile() const
{
  return !m_ModifiersFile.empty();
}
