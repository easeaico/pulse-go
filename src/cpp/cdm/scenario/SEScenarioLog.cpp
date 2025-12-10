/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/scenario/SEScenarioLog.h"
#include "cdm/scenario/SEScenario.h"
#include "cdm/engine/SEAdvanceTime.h"
#include "cdm/engine/SECondition.h"
#include "cdm/engine/SEConditionManager.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/patient/SEPatient.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/utils/FileUtils.h"

#include <regex>

SEScenarioLog::SEScenarioLog(Logger* logger) : Loggable(logger)
{
  Clear();
}

SEScenarioLog::~SEScenarioLog()
{
  Clear();
}

void SEScenarioLog::Clear()
{
  m_Actions.clear();
  m_AdditionalTime.SetValue(2, TimeUnit::min);;
  m_Conditions.clear();
  m_EOL = "\n";
  m_FinalSimTime_s = 0;
  m_Patient = "";
  m_StateFilename = "";
  m_State = "";
}

bool SEScenarioLog::Convert(const std::string& logFilename, SEScenario& dst)
{
  Clear();
  if (!Extract(logFilename))
  {
    Error("Unable to extract data from log file : " + logFilename);
    return false;
  }

  bool err = false;
  dst.Clear();
  dst.SetName(logFilename);
  dst.SetDescription("Converted from log file : " + logFilename);
  if (!m_Patient.empty())
  {
    dst.GetPatientConfiguration().GetPatient().SerializeFromString(m_Patient, eSerializationFormat::JSON);
    for (std::string condition : m_Conditions)
    {
      SECondition* c = SECondition::SerializeFromString(condition, eSerializationFormat::JSON, dst.GetSubstanceManager());
      if (c == nullptr)
      {
        dst.Error("Unable to serialize condition : " + condition);
        err = true;
      }
      else
      {
        if(!dst.GetPatientConfiguration().GetConditions().Copy(*c, dst.GetSubstanceManager()))
          err = true;
      }
      delete c;
    }
  }
  else if (!m_StateFilename.empty())
  {
    if (!FileExists(m_StateFilename))
    {
      Warning("Could not find the state file from the log : " + m_StateFilename);
      Warning("Update the scenario or ensure the state file is in the expected location");
    }
    dst.SetEngineStateFile(m_StateFilename);
  }
  else if (!m_State.empty())
  {
    // TODO
    Error("Inline state information not yet supported");
    err = true;
  }
  else
  {
    Error("Unable to find a starting patient or state file in log.");
    Info("To define a patient, you can add this line \"PatientConfiguration\": { \"PatientFile\": \"StandardMale.json\" }, after the Description");
    err = true;
  }

  double time_s = 0;
  for (auto itr : m_Actions)
  {
    double actionTime_s = itr.first;
    if (actionTime_s > time_s)
    {
      SEAdvanceTime adv;
      adv.GetTime().SetValue(actionTime_s - time_s, TimeUnit::s);
      time_s += actionTime_s - time_s;
      dst.AddAction(adv);
    }

    for (std::string s : itr.second)
    {
      SEAction* a = SEAction::SerializeFromString(s, eSerializationFormat::JSON, dst.GetSubstanceManager());
      if (a == nullptr)
      {
        dst.Error("Unable to serialize action : " + s);
        err = true;
      }
      else
      {
        dst.AddAction(*a);
        if (a->GetName().compare("Advance Time") == 0)
        {
          SEAdvanceTime* adv = static_cast<SEAdvanceTime*>(a);
          if (adv->HasTime())
            time_s += adv->GetTime(TimeUnit::s);
        }
      }
      delete a;
    }
  }

  double additionalTime_s;
  if (m_FinalSimTime_s > 0)
    additionalTime_s = m_FinalSimTime_s - time_s;
  else
    additionalTime_s = m_AdditionalTime.GetValue(TimeUnit::s);

  if (additionalTime_s > 0)
  {
    SEAdvanceTime adv;
    adv.GetTime().SetValue(additionalTime_s, TimeUnit::s);
    time_s += additionalTime_s;
    dst.AddAction(adv);
  }

  return !err;
}

void SEScenarioLog::AbsentAdditionalTime(const SEScalarTime& time)
{
  m_AdditionalTime = time;
  Info("Additional time set to " + m_AdditionalTime.ToString());
}

bool SEScenarioLog::Extract(const std::string& filename)
{
  std::string content;
  if (!ReadFile(filename, content))
  {
    Error("Failed to read file : " + filename);
    return false;
  }

  Clear();
  DetectEOL(content);

  if (!GetPatient(content))
  {
    Error("Failed to retrieve patient information");
    return false;
  }

  if (!GetSerializeFromFile(content))
  {
    Error("Failed to retrieve serializing from file information");
    return false;
  }

  if (!GetSerializeFromString(content))
  {
    Error("Failed to retrieve serializing from string information");
    return false;
  }

  if (!GetConditions(content))
  {
    Error("Failed to retrieve scenario conditions");
    return false;
  }

  if (!GetActions(content))
  {
    Error("Failed to retrieve scenario actions");
    return false;
  }

  if (!GetFinalSimTime(content))
  {
    Error("Failed to retrieve final sim time");
    return false;
  }

  return true;
}

void SEScenarioLog::DetectEOL(const std::string& content)
{
  std::string EOLchars = "\r\n";
  size_t foundFirst = content.find_first_of(EOLchars);
  if (foundFirst != std::string::npos)
  {
    if (foundFirst + 1 >= content.length())
      return;

    // Remove found character from possibilities so we don't end up getting something like \n\n
    EOLchars.erase(std::remove(EOLchars.begin(), EOLchars.end(), content[foundFirst]), EOLchars.end());

    size_t foundLast = content.substr(foundFirst+1).find_first_not_of(EOLchars);

    if (foundLast == std::string::npos)
      foundLast = foundFirst;
    else
      foundLast += foundFirst;

    std::string EOL = content.substr(foundFirst, foundLast + 1 - foundFirst);

    // Double escape escape-chars so that regex can understand
    m_EOL = "";
    for(char c : EOL)
    {
      if (c == '\r')
        m_EOL += "\\r";
      else if (c == '\n')
        m_EOL += "\\n";
      else
        m_EOL += c;
    }
  }
}

bool SEScenarioLog::ExtractTagStrings(const std::string& tag, const std::string& content, std::vector<std::string>& tagStrs, bool braces)
{

  std::string tagPattern = R"(\[)" + tag + R"(\][ \t]*((?!\r\n|\r|\n).*)*(?:\r\n|\r|\n)(\{?))";
  std::string text = content;

  // Capture groups:
  //  0: Whole match
  //  1: Remainder of line after tag
  //  2: Open brace if it exists
  std::smatch mTagBegin;
  std::regex rTagBegin(tagPattern);
  while (std::regex_search(text, mTagBegin, rTagBegin))
  {
    std::string tagStr = "";

    // Locate end of tag string
    size_t endIdx = 0;
    if (braces && mTagBegin[2].compare("{") != 0)
    {
      Error("Is this a legacy log? Unable to identify opening brace : " + std::string(mTagBegin[0]));
      return false;
    }
    else if (braces && !IdentifyTagStringEnd(mTagBegin.suffix().str(), endIdx))
    {
      Error("Unable to identify tag string terminator : " + std::string(mTagBegin[0]));
      return false;
    }
    size_t idx = braces? 2: 1;
    tagStr = text.substr(mTagBegin.position(idx), mTagBegin.length(idx) + endIdx);

    tagStrs.push_back(tagStr);
    text = mTagBegin.suffix().str();
  }

  return true;
}

bool SEScenarioLog::ExtractTagStrings(const std::string& tag, const std::string& content, std::map<double, std::vector<std::string>>& tagStrs, bool braces)
{
  std::string tagPattern = R"((\[\d*\.?\d*\(.*\)\])[ \t]*\[)" + tag + R"(\][ \t]*((?!\r\n|\r|\n).*)*(?:\r\n|\r|\n)(\{?))";
  std::string text = content;

  // Capture groups:
  //  0: Whole match
  //  1: [Time(unit)]
  //  2: Remainder of line after tag
  //  3: Open brace if it exists
  std::smatch mTagBegin;
  std::regex rTagBegin(tagPattern);
  while (std::regex_search(text, mTagBegin, rTagBegin))
  {
    std::string tagStr = "";

    // Parse time
    double time_s;
    ParseTime(mTagBegin[1], time_s);

    // Locate end of tag string
    size_t endIdx = 0;
    if (braces && mTagBegin[3].compare("{") != 0)
    {
      Error("Is this a legacy log? Unable to identify opening brace : " + std::string(mTagBegin[0]));
      return false;
    }
    else if (braces && !IdentifyTagStringEnd(mTagBegin.suffix().str(), endIdx))
    {
      Error("Unable to identify tag string terminator : " + std::string(mTagBegin[0]));
      return false;
    }
    size_t idx = braces? 3: 2;
    tagStr = text.substr(mTagBegin.position(idx), mTagBegin.length(idx) + endIdx);

    if (tagStrs.find(time_s) == tagStrs.end())
    {
      tagStrs[time_s] = std::vector<std::string>();
    }
    tagStrs[time_s].push_back(tagStr);
    text = mTagBegin.suffix().str();
  }

  return true;
}

bool SEScenarioLog::IdentifyTagStringEnd(const std::string& content, size_t& endIdx)
{
  std::smatch mTagEnd;
  std::regex rTagEnd("\\}(" + m_EOL + "){2,}");
  if (std::regex_search(content, mTagEnd, rTagEnd))
  {
    endIdx = mTagEnd.position(0) + m_EOL.length() + 1;
    return true;
  }
  // Failed to find terminating brace
  return false;
}

bool SEScenarioLog::ParseTime(const std::string& timeStr, double& time_s)
{
  std::smatch mTime;
  std::string timePattern = R"((\d*\.?\d*)\((.*)\))";
  std::regex rTime(timePattern);

  if (!std::regex_search(timeStr, mTime, rTime))
    return false;

  // Capture groups:
  //  0: Whole match
  //  1: Time
  //  2: Time unit
  double rawTime = std::stod(mTime[1]);
  std::string rawUnit = mTime[2];

  // Convert to seconds
  SEScalarTime time;
  time.SetValue(rawTime, TimeUnit(rawUnit));
  time_s = time.GetValue(TimeUnit::s);

  return true;
}

bool SEScenarioLog::GetActions(const std::string& content)
{
  if (!ExtractTagStrings("Action", content, m_Actions))
    return false;

  return true;
}

bool SEScenarioLog::GetConditions(const std::string& content)
{
  if (!ExtractTagStrings("Condition", content, m_Conditions))
    return false;

  return true;
}

bool SEScenarioLog::GetFinalSimTime(const std::string& content)
{
  std::vector<std::string> finalSimTimeStrs;
  if (!ExtractTagStrings("Final SimTime", content, finalSimTimeStrs, false))
  {
    m_FinalSimTime_s = 0;
    return false;
  }

  if (finalSimTimeStrs.size() == 0)
  {
    m_FinalSimTime_s = 0;
    Info("No Final SimTime string provided in log");
    return true;
  }

  if (!ParseTime(finalSimTimeStrs[0], m_FinalSimTime_s))
  {
    Warning("Unable to parse Final SimTime string");
    return true;
  }

  return true;
}

bool SEScenarioLog::GetPatient(const std::string& content)
{
  // Capture groups:
  //  0: Whole match
  //  1: Remainder of line, left trimmed (name info)
  std::string patientPattern = R"(\[Patient\][ \t]*([^\n\r]*))";
  std::smatch mPatient;
  std::regex rPatient(patientPattern);
  std::string text = content;
  if (!std::regex_search(text, mPatient, rPatient))
  {
    Info("No patient information");
    return true;
  }

  // Locate end of patient info
  // Capture groups:
  //  0: Whole match
  //  1: Opening bracket '[' indicating end
  size_t endIdx;
  std::smatch mTagEnd;
  std::regex rTagEnd(R"([^\[]*(\[))");
  std::string remainingText = mPatient.suffix().str();
  if (std::regex_search(remainingText, mTagEnd, rTagEnd))
  {
    endIdx = mTagEnd.position(1);
  }
  else // Failed to find end of patient string
  {
    Error("Unable to identify patient string terminator : " + std::string(mPatient[0]));
    return false;
  }
  m_Patient = text.substr(mPatient.position(1), mPatient.length(1) + endIdx);

  return true;
}

bool SEScenarioLog::GetSerializeFromFile(const std::string& content)
{
  std::vector<std::string> serializeFromFileStrs;
  if (!ExtractTagStrings("SerializingFromFile", content, serializeFromFileStrs, false))
    return false;

  if (serializeFromFileStrs.size() > 0)
  {
    m_StateFilename = serializeFromFileStrs[serializeFromFileStrs.size() - 1];
  }
  else
    Info("No State filename found");

  return true;
}

bool SEScenarioLog::GetSerializeFromString(const std::string& content)
{
  std::vector<std::string> serializeFromStringStrs;
  if (!ExtractTagStrings("SerializingFromString", content, serializeFromStringStrs, false))
    return false;

  if (serializeFromStringStrs.size() > 0)
  {
    m_State = serializeFromStringStrs[serializeFromStringStrs.size() - 1];
  }
  else
    Info("No State string found");

  return true;
}
