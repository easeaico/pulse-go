/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

#include "cdm/PhysiologyEngineThunk.h"
#include "engine/PulseConfiguration.h"

class PULSE_DECL PulseEngineThunk : public PhysiologyEngineThunk
{
public:
  PulseEngineThunk(eModelType t, const std::string& dataDir = "./");
  virtual ~PulseEngineThunk();

  bool SetConfigurationOverride(std::string const& cfg, eSerializationFormat format);

  static bool ExecuteScenario(std::string const& sceExecOpts, eSerializationFormat format, Logger* logger=nullptr);

protected:
  virtual void AllocateEngine() override;

  eModelType          m_type;
  PulseConfiguration* m_cfg;
};
