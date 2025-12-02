/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/engine/SEDataRequested.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEEventManager.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/io/protobuf/PBEngine.h"
#include "cdm/utils/DataTrack.h"

SEDataRequested::SEDataRequested()
{
  m_KeepLogMessages = false;
  m_KeepEventChanges = false;
  Clear();
}

SEDataRequested::~SEDataRequested()
{

}

bool SEDataRequested::SerializeToString(std::string& dst, eSerializationFormat m) const
{
  return PBEngine::SerializeToString(*this, dst, m);
}
bool SEDataRequested::SerializeToString(const std::vector<SEDataRequested*>& src, std::string& dst, eSerializationFormat m)
{
  return PBEngine::SerializeToString(src, dst, m);
}

void SEDataRequested::Clear()
{
  m_ID = -1;
  m_IsActive = false;
  m_EventChanges.clear();
  m_LogMessages.Clear();
  m_Headers.clear();
  m_Segments.clear();
}

int SEDataRequested::GetID() const
{
  return m_ID;
}
void SEDataRequested::SetID(int id)
{
  m_ID = id;
}

bool SEDataRequested::IsActive() const
{
  return m_IsActive;
}
void SEDataRequested::SetIsActive(bool b)
{
  m_IsActive = b;
}

void SEDataRequested::PullDataRequested(int id, double currentTime_s, DataTrack& tracker)
{
  size_t length = tracker.NumTracks();
  if (m_Headers.empty())
  {
    for (size_t i = 0; i < length; i++)
      m_Headers.push_back(tracker.GetProbeName(i));
  }
  Segment s;
  s.id = id;
  s.time_s = currentTime_s;
  for (size_t i = 0; i < length; i++)
    s.values.push_back(tracker.GetProbe(i));
  m_Segments.push_back(s);
}
void SEDataRequested::ClearDataRequested()
{
  m_Segments.clear();
  m_EventChanges.clear();
  m_LogMessages.Clear();
}

const std::vector<SEEventChange>& SEDataRequested::GetEventChanges() const
{
  return m_EventChanges;
}
void SEDataRequested::HandleEvent(eEvent type, bool active, const SEScalarTime* time)
{
  if (m_KeepEventChanges)
    m_EventChanges.push_back({ type, active, time });
  if (type == eEvent::IrreversibleState && active)
    m_IsActive = false;
}

const LogMessages& SEDataRequested::GetLogMessages() const
{
  return m_LogMessages;
}

void SEDataRequested::ForwardDebug(const std::string& msg)
{
  if (m_KeepLogMessages)
    m_LogMessages.debug_msgs.push_back(msg);
}
void SEDataRequested::ForwardInfo(const std::string& msg)
{
  if (m_KeepLogMessages)
    m_LogMessages.info_msgs.push_back(msg);
}
void SEDataRequested::ForwardWarning(const std::string& msg)
{
  if (m_KeepLogMessages)
    m_LogMessages.warning_msgs.push_back(msg);
}
void SEDataRequested::ForwardError(const std::string& msg)
{
  m_IsActive = false;
  if (m_KeepLogMessages)
    m_LogMessages.error_msgs.push_back(msg);
}
void SEDataRequested::ForwardFatal(const std::string& msg)
{
  m_IsActive = false;
  if (m_KeepLogMessages)
    m_LogMessages.fatal_msgs.push_back(msg);
}

const std::vector<std::string>& SEDataRequested::GetHeaders() const
{
  return m_Headers;
}
const std::vector<SEDataRequested::Segment>& SEDataRequested::GetSegments() const
{
  return m_Segments;
}
