/* Distributed under the Apache License, Version 2.0.
See accompanying NOTICE file for details.*/

#include "PulseEnginePool.h"

#include "engine/human_adult/whole_body/Engine.h"
#include "engine/human_adult/ventilation_mechanics/Engine.h"

PulseEnginePool::PulseEnginePool(size_t poolSize, const std::string& dataDir, Logger* logger)
  : SEPhysiologyEnginePool(poolSize, dataDir, logger)
{

}

SEEngineInitializationStatus PulseEnginePool::InitializeEngine(SEEngineInitialization init, eModelType type)
{
  m_creationType = type;
  return InitializeEngine(init);
}

std::vector<SEEngineInitializationStatus> PulseEnginePool::InitializeEngines(std::vector<SEEngineInitialization>& inits, eModelType type)
{
  m_creationType = type;
  return InitializeEngines(inits);
}

void PulseEnginePool::AllocateEngine(SEPhysiologyEnginePoolEngine& pe)
{
  pe.Engine = CreatePulseEngine(m_creationType);
}
