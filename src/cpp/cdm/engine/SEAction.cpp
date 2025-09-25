/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/engine/SEAction.h"
#include "cdm/engine/SEAdvanceTime.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/io/protobuf/PBActions.h"


SEAction::SEAction(Logger* logger) : Loggable(logger)
{
  Clear();
}

SEAction::~SEAction()
{
  Clear();
}

void SEAction::Clear()
{
  m_Comment="";
  m_Active = false;
}

bool SEAction::SerializeToString(std::string& dst, eSerializationFormat fmt) const
{
  return PBAction::SerializeToString(*this, dst, fmt);
}
SEAction* SEAction::SerializeFromString(const std::string src, eSerializationFormat fmt, const SESubstanceManager& subMgr)
{
  return PBAction::SerializeFromString(src, fmt, subMgr);
}

std::string SEAction::GetComment() const
{
  return m_Comment;
}
void SEAction::SetComment(const std::string& comment)
{
  m_Comment = comment;
}
bool SEAction::HasComment() const
{
  return m_Comment.empty()?false:true;
}
void SEAction::InvalidateComment()
{
  m_Comment = "";
}

SEAction* SEAction::Copy(const SEAction& action, const SESubstanceManager& subMgr)
{
  return PBAction::Copy(action, subMgr);
}

std::string SEAction::ToJSON() const
{
  std::string s;
  if (!SerializeToString(s, eSerializationFormat::VERBOSE_JSON))
    Error("Unable to serialize action to json");
  return s;
}
std::string SEAction::ToString() const
{
  return PrettyPrint(this->ToJSON());
}

std::string SEAction::PrettyPrint(const std::string& str)
{
  return pulse::cdm::PrettyPrint(str, ePrettyPrintType::Action);
}
