/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/engine/SEEngineInitialization.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/io/protobuf/PBEngine.h"

SEEngineInitialization::SEEngineInitialization(Logger* logger) : Loggable(logger)
{
  m_PatientConfiguration = nullptr;
  m_DataRequestManager = nullptr;
  Clear();
}

SEEngineInitialization::~SEEngineInitialization()
{
  SAFE_DELETE(m_PatientConfiguration);
  SAFE_DELETE(m_DataRequestManager);
}

bool SEEngineInitialization::SerializeToString(std::string& output, eSerializationFormat m) const
{
  return PBEngine::SerializeToString(*this, output, m);
}
bool SEEngineInitialization::SerializeFromString(const std::string& src, eSerializationFormat m, const SESubstanceManager& subMgr)
{
  return PBEngine::SerializeFromString(src, *this, m, subMgr);
}
bool SEEngineInitialization::SerializeFromString(const std::string& src, std::vector<SEEngineInitialization*>& dst, eSerializationFormat m, const SESubstanceManager& subMgr)
{
  return PBEngine::SerializeFromString(src, dst, m, subMgr);
}

void SEEngineInitialization::Clear()
{
  SAFE_DELETE(m_PatientConfiguration);
  m_StateFilename = "";
  m_State = "";
  SAFE_DELETE(m_DataRequestManager);
  m_LogFilename = "";
  m_KeepLogMessages = false;
  m_KeepEventChanges = false;
}

void SEEngineInitialization::Copy(const SEEngineInitialization& from, const SESubstanceManager& subMgr)
{
  PBEngine::Copy(from, *this, subMgr);
}

bool SEEngineInitialization::IsValid() const
{
  return (HasPatientConfiguration() || HasStateFilename() || HasState());
}

bool SEEngineInitialization::HasPatientConfiguration() const
{
  return m_PatientConfiguration != nullptr;
}
SEPatientConfiguration& SEEngineInitialization::GetPatientConfiguration()
{
  if (m_PatientConfiguration == nullptr)
    m_PatientConfiguration = new SEPatientConfiguration(GetLogger());
  return *m_PatientConfiguration;
}
const SEPatientConfiguration* SEEngineInitialization::GetPatientConfiguration() const
{
  return m_PatientConfiguration;
}

std::string SEEngineInitialization::GetStateFilename() const
{
  return m_StateFilename;
}
void SEEngineInitialization::SetStateFilename(const std::string& StateFilename)
{
  m_StateFilename = StateFilename;
}
bool SEEngineInitialization::HasStateFilename() const
{
  return !m_StateFilename.empty();
}

bool SEEngineInitialization::HasState() const
{
  return !m_State.empty();
}
std::string SEEngineInitialization::GetState() const
{
  return m_State;
}
void SEEngineInitialization::SetState(const std::string& State, eSerializationFormat fmt)
{
  m_State = State;
  m_StateFormat = fmt;
}
eSerializationFormat SEEngineInitialization::GetStateFormat() const
{
  return m_StateFormat;
}

bool SEEngineInitialization::HasDataRequestManager() const
{
  return m_DataRequestManager != nullptr;
}
SEDataRequestManager& SEEngineInitialization::GetDataRequestManager()
{
  if (m_DataRequestManager == nullptr)
    m_DataRequestManager = new SEDataRequestManager(GetLogger());
  return *m_DataRequestManager;
}
const SEDataRequestManager* SEEngineInitialization::GetDataRequestManager() const
{
  return m_DataRequestManager;
}

std::string SEEngineInitialization::GetLogFilename() const
{
  return m_LogFilename;
}
void SEEngineInitialization::SetLogFilename(const std::string& LogFilename)
{
  m_LogFilename = LogFilename;
}
bool SEEngineInitialization::HasLogFilename() const
{
  return !m_LogFilename.empty();
}

bool SEEngineInitialization::KeepLogMessages() const
{
  return m_KeepLogMessages;
}
void SEEngineInitialization::KeepLogMessages(bool b)
{
  m_KeepLogMessages = b;
}

bool SEEngineInitialization::KeepEventChanges() const
{
  return m_KeepEventChanges;
}
void SEEngineInitialization::KeepEventChanges(bool b)
{
  m_KeepEventChanges = b;
}

SEEngineInitializationStatus::SEEngineInitializationStatus()
{
  Clear();
}
SEEngineInitializationStatus::~SEEngineInitializationStatus()
{

}

void SEEngineInitializationStatus::Clear()
{
  m_EngineInitializationState = eEngineInitializationState::Uninitialized;
  m_CSVFilename = "";
  m_LogFilename = "";
  m_StabilizationTime_s = 0;
}

void SEEngineInitializationStatus::Copy(const SEEngineInitializationStatus& from)
{
  PBEngine::Copy(from, *this);
}
bool SEEngineInitializationStatus::SerializeToString(std::string& output, eSerializationFormat m, Logger* logger) const
{
  return PBEngine::SerializeToString(*this, output, m, logger);
}
bool SEEngineInitializationStatus::SerializeFromString(const std::string& src, eSerializationFormat m, Logger* logger)
{
  return PBEngine::SerializeFromString(src, *this, m, logger);
}
