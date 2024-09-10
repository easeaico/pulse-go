/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/common/Engine.h"
#include "engine/PulseConfiguration.h"
#include "engine/common/controller/Controller.h"
#include "engine/common/controller/BlackBoxManager.h"
#include "engine/common/controller/CompartmentManager.h"
#include "engine/common/controller/SubstanceManager.h"

namespace pulse
{
  Engine::Engine(Logger* logger) : PhysiologyEngine(logger)
  {
    m_Logger->LogToConsole(true);
  }

  Engine::~Engine()
  {
    GetLogger()->SetLogTime(nullptr);
    SAFE_DELETE(m_Controller);
  }

  Controller& Engine::GetController() const
  {
    if (m_Controller == nullptr)
      AllocateController();
    return *m_Controller;
  }

  Logger* Engine::GetLogger() const
  {
    GetController();
    return Loggable::GetLogger();
  }

  std::string Engine::GetTypeName() const
  {
    return GetController().GetTypeName();
  }

  bool Engine::SerializeFromFile(const std::string& filename)
  {
    if (m_Controller == nullptr) AllocateController();
    return GetController().SerializeFromFile(filename);
  }
  bool Engine::SerializeToFile(const std::string& filename) const
  {
    return GetController().SerializeToFile(filename);
  }

  bool Engine::SerializeFromString(const std::string& src, eSerializationFormat m)
  {
    return GetController().SerializeFromString(src, m);
  }
  bool Engine::SerializeToString(std::string& output, eSerializationFormat m) const
  {
    return GetController().SerializeToString(output, m);
  }

  bool Engine::InitializeEngine(const std::string& patient_configuration, eSerializationFormat m)
  {
    return GetController().InitializeEngine(patient_configuration, m);
  }
  bool Engine::InitializeEngine(const SEPatientConfiguration& patient_configuration)
  {
    return GetController().InitializeEngine(patient_configuration);
  }
  eEngineInitializationState Engine::GetInitializationState() const
  {
    return GetController().GetInitializationState();
  }

  void Engine::Clear()
  {
    GetController().Clear();
  }

  bool Engine::SetConfigurationOverride(const SEEngineConfiguration* config)
  {
    return GetController().SetConfigurationOverride(config);
  }

  SEEngineTracker* Engine::GetEngineTracker() const
  {
    return &GetController().GetData().GetEngineTracker();
  }


  const SEConditionManager& Engine::GetConditionManager() const
  {
    return GetController().GetConditions();
  }

  double Engine::GetTimeStep(const TimeUnit& unit) const
  {
    return GetController().GetData().GetTimeStep().GetValue(unit);
  }

  double Engine::GetSimulationTime(const TimeUnit& unit) const
  {
    return  GetController().GetData().GetSimulationTime().GetValue(unit);
  }
  void Engine::SetSimulationTime(const SEScalarTime& time)
  {
    return  GetController().SetSimulationTime(time);
  }

  double Engine::GetStabilizationTime(const TimeUnit& unit) const
  {
    return  GetController().GetData().GetStabilizationTime().GetValue(unit);
  }

  bool Engine::AdvanceModelTime()
  {
    return GetController().AdvanceModelTime();
  }

  bool Engine::AdvanceModelTime(double time, const TimeUnit& unit)
  {
    return GetController().AdvanceModelTime(time, unit);
  }

  bool Engine::ProcessAction(const SEAction& action)
  {
    return GetController().ProcessAction(action);
  }

  const SEActionManager& Engine::GetActionManager() const
  {
    return GetController().GetData().GetActions();
  }

  const SEEventManager& Engine::GetEventManager() const
  {
    return GetController().GetData().GetEvents();
  }

  void Engine::SetAdvanceHandler(SEAdvanceHandler* handler)
  {
    GetController().SetAdvanceHandler(handler);
  }

  const SEEngineConfiguration* Engine::GetConfiguration() const
  {
    return &GetController().GetData().GetConfiguration();
  }

  const SEPatient& Engine::GetPatient() const
  {
    return GetController().GetData().GetCurrentPatient();
  }

  const SEPatient& Engine::GetInitialPatient() const
  {
    return GetController().GetData().GetInitialPatient();
  }

  bool Engine::GetPatientAssessment(SEPatientAssessment& assessment) const
  {
    return GetController().GetPatientAssessment(assessment);
  }

  const SEEnvironment* Engine::GetEnvironment() const
  {
    return &GetController().GetData().GetEnvironment();
  }

  const SESubstanceManager& Engine::GetSubstanceManager() const
  {
    return GetController().GetData().GetSubstances();
  }

  const SEBloodChemistrySystem* Engine::GetBloodChemistrySystem() const
  {
    return &GetController().GetData().GetBloodChemistry();
  }

  const SECardiovascularSystem* Engine::GetCardiovascularSystem() const
  {
    return &GetController().GetData().GetCardiovascular();
  }

  const SEDrugSystem* Engine::GetDrugSystem() const
  {
    return &GetController().GetData().GetDrugs();
  }

  const SEEndocrineSystem* Engine::GetEndocrineSystem() const
  {
    return &GetController().GetData().GetEndocrine();
  }

  const SEEnergySystem* Engine::GetEnergySystem() const
  {
    return &GetController().GetData().GetEnergy();
  }

  const SEGastrointestinalSystem* Engine::GetGastrointestinalSystem() const
  {
    return &GetController().GetData().GetGastrointestinal();
  }

  const SEHepaticSystem* Engine::GetHepaticSystem() const
  {
    return &GetController().GetData().GetHepatic();
  }

  const SENervousSystem* Engine::GetNervousSystem() const
  {
    return &GetController().GetData().GetNervous();
  }

  const SERenalSystem* Engine::GetRenalSystem() const
  {
    return &GetController().GetData().GetRenal();
  }

  const SERespiratorySystem* Engine::GetRespiratorySystem() const
  {
    return &GetController().GetData().GetRespiratory();
  }

  const SETissueSystem* Engine::GetTissueSystem() const
  {
    return &GetController().GetData().GetTissue();
  }

  const SEAnesthesiaMachine* Engine::GetAnesthesiaMachine() const
  {
    return &GetController().GetData().GetAnesthesiaMachine();
  }

  const SEBagValveMask* Engine::GetBagValveMask() const
  {
    return &GetController().GetData().GetBagValveMask();
  }

  const SEECMO* Engine::GetECMO() const
  {
    return &GetController().GetData().GetECMO();
  }

  const SEElectroCardioGram* Engine::GetElectroCardioGram() const
  {
    return &GetController().GetData().GetECG();
  }

  const SEInhaler* Engine::GetInhaler() const
  {
    return &GetController().GetData().GetInhaler();
  }

  const SEMechanicalVentilator* Engine::GetMechanicalVentilator() const
  {
    return &GetController().GetData().GetMechanicalVentilator();
  }

  const SECompartmentManager& Engine::GetCompartments() const
  {
    return GetController().GetData().GetCompartments();
  }

  SEBlackBoxManager& Engine::GetBlackBoxes() const
  {
    return GetController().GetData().GetBlackBoxes();
  }
END_NAMESPACE
