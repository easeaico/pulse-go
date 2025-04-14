/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/CommonDefs.h"
PUSH_PROTO_WARNINGS
#include "pulse/engine/bind/Scenario.pb.h"
POP_PROTO_WARNINGS
#include "engine/io/protobuf/PBScenario.h"
#include "engine/io/protobuf/PBConfiguration.h"

#include "cdm/io/protobuf/PBScenario.h"
#include "cdm/io/protobuf/PBUtils.h"
#include "cdm/utils/FileUtils.h"

namespace pulse
{
  void PBScenario::Load(const PULSE_BIND::ScenarioData& src, PulseScenario& dst)
  {
    dst.Clear();
    PBScenario::Serialize(src, dst);
  }
  void PBScenario::Serialize(const PULSE_BIND::ScenarioData& src, PulseScenario& dst)
  {
    ::PBScenario::Serialize(src.scenario(), dst);

    if (src.has_configuration())
      PBConfiguration::Load(src.configuration(), dst.GetConfiguration(), dst.GetSubstanceManager());
  }
  PULSE_BIND::ScenarioData* PBScenario::Unload(const PulseScenario& src)
  {
    PULSE_BIND::ScenarioData* dst = new PULSE_BIND::ScenarioData();
    PBScenario::Serialize(src, *dst);
    return dst;
  }
  void PBScenario::Serialize(const PulseScenario& src, PULSE_BIND::ScenarioData& dst)
  {
    ::PBScenario::Serialize(src, *dst.mutable_scenario());
    if (src.HasConfiguration())
      dst.set_allocated_configuration(PBConfiguration::Unload(*src.GetConfiguration()));
  }
  void PBScenario::Copy(const PulseScenario& src, PulseScenario& dst)
  {
    dst.Clear();
    PULSE_BIND::ScenarioData data;
    PBScenario::Serialize(src, data);
    PBScenario::Serialize(data, dst);
  }

  bool PBScenario::SerializeToString(const PulseScenario& src, std::string& output, eSerializationFormat m)
  {
    PULSE_BIND::ScenarioData data;
    PBScenario::Serialize(src, data);
    return PBUtils::SerializeToString(data, output, m, src.GetLogger());
  }
  bool PBScenario::SerializeToFile(const PulseScenario& src, const std::string& filename)
  {
    PULSE_BIND::ScenarioData data;
    PBScenario::Serialize(src, data);
    return PBUtils::SerializeToFile(data, filename, src.GetLogger());
  }

  bool PBScenario::SerializeFromString(const std::string& src, PulseScenario& dst, eSerializationFormat m)
  {
    PULSE_BIND::ScenarioData data;
    dst.GetLogger()->Info("Loading scenario...");
    if (!PBUtils::SerializeFromString(src, data, m, dst.GetLogger()))
    {
      dst.GetLogger()->Info("Attempting to resolve errors...");
      // Try our base class
      CDM_BIND::ScenarioData cdm_data;
      if (!PBUtils::SerializeFromString(src, cdm_data, m, dst.GetLogger()))
        return false;
      dst.GetLogger()->Info("Successfully loaded scenario as a base SEScenario");
      ::PBScenario::Load(cdm_data, dst);
      return true;
    }
    PBScenario::Load(data, dst);
    dst.GetLogger()->Info("Successfully loaded scenario");
    return true;
  }
  bool PBScenario::SerializeFromFile(const std::string& filename, PulseScenario& dst)
  {
    PULSE_BIND::ScenarioData data;
    if (PBUtils::SerializeFromFile(filename, data, dst.GetLogger()))
    {
      PBScenario::Load(data, dst);
      return true;
    }
    return ::PBScenario::SerializeFromFile(filename, dst);
  }


  void PBScenario::Copy(const PulseScenarioExec& src, PulseScenarioExec& dst)
  {
    dst.Clear();
    PULSE_BIND::ScenarioExecData data;
    PBScenario::Serialize(src, data);
    PBScenario::Serialize(data, dst);
  }
  void PBScenario::Load(const PULSE_BIND::ScenarioExecData& src, PulseScenarioExec& dst)
  {
    dst.Clear();
    PBScenario::Serialize(src, dst);
  }
  void PBScenario::Serialize(const PULSE_BIND::ScenarioExecData& src, PulseScenarioExec& dst)
  {
    ::PBScenario::Serialize(src.scenarioexec(), dst);
    dst.SetModelType((eModelType)src.modeltype());
  }

  PULSE_BIND::ScenarioExecData* PBScenario::Unload(const PulseScenarioExec& src)
  {
    PULSE_BIND::ScenarioExecData* dst = new PULSE_BIND::ScenarioExecData();
    PBScenario::Serialize(src, *dst);
    return dst;
  }
  void PBScenario::Serialize(const PulseScenarioExec& src, PULSE_BIND::ScenarioExecData& dst)
  {
    ::PBScenario::Serialize(src, *dst.mutable_scenarioexec());
    dst.set_modeltype((PULSE_BIND::eModelType)src.GetModelType());
  }

  bool PBScenario::SerializeToString(const PulseScenarioExec& src, std::string& output, eSerializationFormat m, Logger* logger)
  {
    PULSE_BIND::ScenarioExecData data;
    PBScenario::Serialize(src, data);
    return PBUtils::SerializeToString(data, output, m, logger);
  }
  bool PBScenario::SerializeToFile(const PulseScenarioExec& src, const std::string& filename)
  {
    PULSE_BIND::ScenarioExecData data;
    PBScenario::Serialize(src, data);
    return PBUtils::SerializeToFile(data, filename, src.GetLogger());
  }
  bool PBScenario::SerializeFromString(const std::string& src, PulseScenarioExec& dst, eSerializationFormat m, Logger* logger)
  {
    PULSE_BIND::ScenarioExecData data;
    if (!PBUtils::SerializeFromString(src, data, m, logger))
      return false;
    PBScenario::Load(data, dst);
    return true;
  }
  bool PBScenario::SerializeFromFile(const std::string& filename, PulseScenarioExec& dst)
  {
    PULSE_BIND::ScenarioExecData data;
    if (PBUtils::SerializeFromFile(filename, data, dst.GetLogger()))
    {
      PBScenario::Load(data, dst);
      return true;
    }
    return ::PBScenario::SerializeFromFile(filename, dst);
  }
}
