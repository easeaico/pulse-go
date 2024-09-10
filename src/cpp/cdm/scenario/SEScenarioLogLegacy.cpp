/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/scenario/SEScenarioLog.h"
#include "cdm/scenario/SEScenario.h"
#include "cdm/utils/FileUtils.h"
   // Supported Actions
#include "cdm/engine/SEAdvanceTime.h"
#include "cdm/patient/actions/SEArrhythmia.h"
#include "cdm/patient/actions/SEChestCompressionInstantaneous.h"
#include "cdm/system/equipment/bag_valve_mask/SEBagValveMask.h"
#include "cdm/system/equipment/bag_valve_mask/actions/SEBagValveMaskConfiguration.h"
#include "cdm/system/equipment/bag_valve_mask/actions/SEBagValveMaskSqueeze.h"

#include "cdm/properties/SEScalarForce.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/utils/FileUtils.h"

std::string actionToken = "[Action]";
std::string tab = "    ";
struct ScenarioAction
{
  std::string Name;
  std::string Type = "";
  double ScenarioTime_s;
  SEScalarProperties Properties;
  std::map<std::string, std::string> Enums;
};

std::pair<bool, SEScalarPair> GetProperty(const std::string& name, const SEScalarProperties& properties)
{
  auto i = properties.find(name);
  if (i != properties.end())
  {
    if (!std::isnan(i->second.value))
      return std::pair<bool, SEScalarPair>(true, SEScalarPair(i->second.value,i->second.unit));
  }
  return std::pair<bool, SEScalarPair>(false, SEScalarPair(SEScalar::dNaN()));
}
std::pair<bool, std::string> GetEnum(const std::string& name, const std::map<std::string, std::string>& enums)
{
  auto i = enums.find(name);
  if (i != enums.end())
    return std::pair<bool, std::string>(true, i->second);
  return std::pair<bool, std::string>(false, "");
}

// Manually translate ScenarioActions to SEActions
// Yes, this is a pain, but this log format is now OBE
bool AddAction(const ScenarioAction& sa, SEScenario& sce)
{
  std::pair<bool, SEScalarPair> s;
  std::pair<bool, std::string> e;
  if (sa.Type == SEBagValveMaskAction::ActionType)
  {
    if (sa.Name == SEBagValveMaskConfiguration::Name)
    {
      SEBagValveMaskConfiguration a;
      s = GetProperty("ValvePositiveEndExpiratoryPressure", sa.Properties);
      if (s.first)
        a.GetConfiguration().GetValvePositiveEndExpiratoryPressure().SetValue(s.second.value, PressureUnit::GetCompoundUnit(s.second.unit));

      e = GetEnum("Connection", sa.Enums);
      if (e.first)
        a.GetConfiguration().SetConnection(e.second == "On" ? eSwitch::On : eSwitch::Off);

      sce.AddAction(a);
      return true;
    }

    if (sa.Name == SEBagValveMaskSqueeze::Name)
    {
      SEBagValveMaskSqueeze a;
      s = GetProperty("SqueezePressure", sa.Properties);
      if (s.first)
        a.GetSqueezePressure().SetValue(s.second.value, PressureUnit::GetCompoundUnit(s.second.unit));

      s = GetProperty("SqueezeVolume", sa.Properties);
      if (s.first)
        a.GetSqueezeVolume().SetValue(s.second.value, VolumeUnit::GetCompoundUnit(s.second.unit));

      s = GetProperty("ExpiratoryPeriod", sa.Properties);
      if (s.first)
        a.GetExpiratoryPeriod().SetValue(s.second.value, TimeUnit::GetCompoundUnit(s.second.unit));

      s = GetProperty("InspiratoryPeriod", sa.Properties);
      if (s.first)
        a.GetInspiratoryPeriod().SetValue(s.second.value, TimeUnit::GetCompoundUnit(s.second.unit));

      sce.AddAction(a);
      return true;
    }
  }

  if (sa.Type == SEPatientAction::ActionType)
  {
    if (sa.Name == SEArrhythmia::Name)
    {
      SEArrhythmia a;
      a.SetRhythm(eHeartRhythm::NormalSinus);

      sce.AddAction(a);
      return true;
    }
    if (sa.Name == SEChestCompressionInstantaneous::Name)
    {
      SEChestCompressionInstantaneous a;
      s = GetProperty("Force", sa.Properties);
      if (s.first)
        a.GetForce().SetValue(s.second.value, ForceUnit::GetCompoundUnit(s.second.unit));

      sce.AddAction(a);
      return true;
    }
  }

  return false;
}

bool ParseLogAction(const std::string& allLines, std::vector<ScenarioAction>& actions, Logger* logger)
{
  ScenarioAction a;

  // Get scenario time
  size_t beginIdx = actionToken.length();
  size_t endIdx = allLines.find("(", actionToken.length());
  if (endIdx == std::string::npos)
  {
    logger->Error("Unable to parse scenario time");
    return false;
  }
  a.ScenarioTime_s = std::stod(allLines.substr(actionToken.length(), endIdx - beginIdx));

  // Get action name
  beginIdx = allLines.find(",", endIdx);
  if (beginIdx == std::string::npos)
  {
    logger->Error("Unable to parse action name");
    return false;
  }
  beginIdx += 2;
  endIdx = allLines.find("\n", beginIdx + 1);
  if (endIdx == std::string::npos)
  {
    logger->Error("Unable to parse action name");
    return false;
  }
  std::string line = allLines.substr(beginIdx, endIdx - beginIdx);
  size_t colonIdx;
  if ((colonIdx = line.find(":", 0)) != std::string::npos)
  {
    a.Type = line.substr(0, colonIdx - 1);
    line = line.substr(colonIdx + 2);
  }
  a.Name = line;

  // Extract properties from remaining lines
  while (true)
  {
    beginIdx = endIdx + 1 + tab.length();
    if (beginIdx >= allLines.length())
    {
      break;
    }

    // Get property name from before colon
    endIdx = allLines.find(":", beginIdx + 1);
    if (endIdx == std::string::npos)
    {
      logger->Error("Unable to parse property name");
      return false;
    }
    std::string propName = allLines.substr(beginIdx, endIdx - beginIdx);

    // Get property value after colon
    beginIdx = endIdx + 2;
    endIdx = allLines.find("\n", beginIdx);
    if (endIdx == std::string::npos)
    {
      logger->Error("Unable to parse property value");
      return false;
    }
    line = allLines.substr(beginIdx, endIdx - beginIdx);

    // Check for paren for potential number(unit) format
    size_t unitBeginIdx = line.find("(", 0);
    size_t unitEndIdx=0;
    size_t valueEndIdx = line.length() - 1;
    if (unitBeginIdx != std::string::npos)
    {
      valueEndIdx = unitBeginIdx - 1;
    }
    try
    {
      double value = std::stod(line.substr(0, valueEndIdx + 1));
      if (unitBeginIdx != std::string::npos && (unitEndIdx = line.find(")", unitBeginIdx + 1)) != std::string::npos)
      {
        std::string unit = line.substr(unitBeginIdx + 1, unitEndIdx - (unitBeginIdx + 1));
        a.Properties[propName] = SEScalarPair(value, unit);
      }
      else if (unitBeginIdx != std::string::npos && unitEndIdx == std::string::npos)
      {
        logger->Error("Unable to parse unit");
        return false;
      }
      else
      {
        a.Properties[propName] = SEScalarPair(value);
      }
    }
    catch (std::invalid_argument&) // No number found
    {
      a.Enums[propName] = line;
    }
  }
  actions.push_back(a);

  return true;
}


//--------------------------------------------------------------------------------------------------
/// \brief
/// Generates previously executed scenario based on log file messages.
//--------------------------------------------------------------------------------------------------
bool SEScenarioLog::GenerateScenarioFromLegacyLog(const std::string& filename, const std::string& stateFilename, double extraTime_s)
{
  SEScenario sce;
  sce.GetLogger()->LogToConsole(true);
  sce.GetLogger()->SetLogFile("./LegacyLog2Scenario.log");

  sce.SetName(filename);
  sce.SetDescription("Created from a log file");
  sce.SetEngineStateFile(stateFilename);

  std::string content;
  if (!ReadFile(filename, content))
  {
    sce.Error("Unable to read file " + filename);
    return false;
  }

  // Identify beginning of an action with token
  size_t idx = 0;
  std::vector<ScenarioAction> actions;
  while ((idx = content.find(actionToken, idx)) != std::string::npos)
  {
    size_t actionBegin = idx;
    size_t actionEnd = idx + 1;

    // Identify end of an action based on indentation
    while ((actionEnd = content.find("\n", actionEnd)) != std::string::npos)
    {
      if (actionEnd + 1 >= content.length() || tab.compare(content.substr(actionEnd + 1, tab.length())) != 0)
      {
        break;
      }
      actionEnd++;
    }

    // Action identified
    if (actionEnd != std::string::npos)
    {
      std::string action = content.substr(actionBegin, actionEnd - actionBegin + 1);

      if (!ParseLogAction(action, actions, sce.GetLogger()))
      {
        sce.Error("Unable to parse action: " + action);
        return false;
      }
    }
    idx = actionEnd + 1;
  }

  //Create SEActions from the actions we found
  double time_s = 0;
  for (ScenarioAction& sa : actions)
  {
    if (sa.ScenarioTime_s > time_s)
    {
      SEAdvanceTime adv;
      adv.GetTime().SetValue(sa.ScenarioTime_s - time_s, TimeUnit::s);
      time_s += sa.ScenarioTime_s - time_s;
      sce.AddAction(adv);
    }

    if (!AddAction(sa, sce))
    {
      sce.Error("Unsupportd Action: " + sa.Name);
      continue;
    }
  }

  // Add our extra time if provided
  if (extraTime_s > 0)
  {
    SEAdvanceTime adv;
    adv.GetTime().SetValue(extraTime_s, TimeUnit::s);
    sce.AddAction(adv);
  }

  std::string scenarioFilename = filename;
  scenarioFilename = Replace(scenarioFilename, ".log", ".sce.json");
  sce.SerializeToFile(scenarioFilename);
  return true;
}
