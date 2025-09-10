/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "EngineHowTo.h"
#include "PulseEngine.h"
#include "PulseConfiguration.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/properties/SEScalarTime.h"


void HowToExpandedVasculature()
{
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->LogToConsole(true);
  pe->GetLogger()->SetLogFile("./test_results/howto/HowTo_ExpandedVasculature.log");

  SEPatientConfiguration pc;
  pc.SetPatientFile("./patients/StandardMale.json");

  PulseConfiguration config;
  config.UseComputationalLifeExpansion(eSwitch::On);

  pe->SetConfigurationOverride(&config);
  pe->InitializeEngine(pc);

  pe->AdvanceModelTime(10, TimeUnit::s);
}
