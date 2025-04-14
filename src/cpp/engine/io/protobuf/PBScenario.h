/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

PULSE_BIND_DECL(Scenario)
PULSE_BIND_DECL(ScenarioExec)
#include "PulseScenario.h"
#include "PulseScenarioExec.h"

namespace pulse
{
  class PULSE_DECL PBScenario
  {
  public:

    static void Load(const PULSE_BIND::ScenarioData& src, PulseScenario& dst);
    static PULSE_BIND::ScenarioData* Unload(const PulseScenario& src);
    static void Serialize(const PULSE_BIND::ScenarioData& src, PulseScenario& dst);
    static void Serialize(const PulseScenario& src, PULSE_BIND::ScenarioData& dst);
    static void Copy(const PulseScenario& src, PulseScenario& dst);

    static bool SerializeToString(const PulseScenario& src, std::string& output, eSerializationFormat m);
    static bool SerializeToFile(const PulseScenario& src, const std::string& filename);
    static bool SerializeFromString(const std::string& src, PulseScenario& dst, eSerializationFormat m);
    static bool SerializeFromFile(const std::string& filename, PulseScenario& dst);

    static void Copy(const PulseScenarioExec& src, PulseScenarioExec& dst);
    static void Load(const PULSE_BIND::ScenarioExecData& src, PulseScenarioExec& dst);
    static PULSE_BIND::ScenarioExecData* Unload(const PulseScenarioExec& src);
    static void Serialize(const PULSE_BIND::ScenarioExecData& src, PulseScenarioExec& dst);
    static void Serialize(const PulseScenarioExec& src, PULSE_BIND::ScenarioExecData& dst);

    static bool SerializeToString(const PulseScenarioExec& src, std::string& output, eSerializationFormat m, Logger* logger);
    static bool SerializeToFile(const PulseScenarioExec& src, const std::string& filename);
    static bool SerializeFromString(const std::string& src, PulseScenarioExec& dst, eSerializationFormat m, Logger* logger);
    static bool SerializeFromFile(const std::string& filename, PulseScenarioExec& dst);
  };
}