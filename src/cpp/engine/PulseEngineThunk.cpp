/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/PulseEngineThunk.h"
#include "engine/PulseScenarioExec.h"
#include "engine/human_adult/whole_body/Engine.h"
#include "engine/human_adult/hemodynamics/Engine.h"
#include "engine/human_adult/ventilation_mechanics/Engine.h"

PulseEngineThunk::PulseEngineThunk(eModelType t, const std::string& dataDir) : PhysiologyEngineThunk(dataDir)
{
  m_type = t;
  AllocateEngine();
  m_engine->GetLogger()->LogToConsole(false);
  m_engine->GetLogger()->AddForward(this);
  m_cfg = new PulseConfiguration(m_engine->GetLogger());
}
PulseEngineThunk::~PulseEngineThunk()
{
  delete m_cfg;
}

bool PulseEngineThunk::SetConfigurationOverride(std::string const& cfg, eSerializationFormat format)
{
  if (cfg.empty())
    return m_engine->SetConfigurationOverride(nullptr);

  if (!m_cfg->SerializeFromString(cfg, format, *m_subMgr))
  {
    m_engine->GetLogger()->Error("Unable to load engine configuration string");
    return false;
  }
  return m_engine->SetConfigurationOverride(m_cfg);
}

bool PulseEngineThunk::ExecuteScenario(std::string const& sceExecOpts, eSerializationFormat format, Logger* logger)
{
  PulseScenarioExec opts(logger);
  if (!opts.SerializeFromString(sceExecOpts, format))
    return false;
  if (logger == nullptr)// No logger provided, so log per the opts
    opts.GetLogger()->LogToConsole(opts.LogToConsole() == eSwitch::On);
  return opts.Execute();
}

void PulseEngineThunk::AllocateEngine()
{
  m_engine = CreatePulseEngine(m_type);
}

std::unique_ptr<PhysiologyEngine> CreatePulseEngine(eModelType type, Logger* logger)
{
  switch (type)
  {
  case eModelType::HumanAdultWholeBody:
    return std::make_unique<pulse::human_adult_whole_body::Engine>(logger);
  case eModelType::HumanAdultHemodynamics:
    return std::make_unique<pulse::human_adult_hemodynamics::Engine>(logger);
  case eModelType::HumanAdultVentilationMechanics:
    return std::make_unique<pulse::human_adult_ventilation_mechanics::Engine>(logger);
  default:
    throw CommonDataModelException("Unsupported Physiology Engine Type");
  }
}

bool eModelType_ValueOf(const std::string s, eModelType& t)
{
  if (s == "HumanAdultWholeBody")
    t = eModelType::HumanAdultWholeBody;
  else if (s == "HumanAdultHemodynamics")
    t = eModelType::HumanAdultHemodynamics;
  else if (s == "HumanAdultVentilationMechanics")
    t = eModelType::HumanAdultVentilationMechanics;
  else
    return false;
  return true;
}
