/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#pragma once

#include "engine/CommonDefs.h"

namespace pulse
{
  class Controller;
  //--------------------------------------------------------------------------------------------------
  /// @brief  
  /// This is the implementation of the PhysiologyEngine interface for the this engines.
  /// @details
  /// It contains the necessary execution calls, patient customization calls, insult and intervention 
  /// calls as well as assessment calls for obtaining the results. During engine execution a log files
  /// is generated containing information, warning and error data.
  //--------------------------------------------------------------------------------------------------
  class PULSE_DECL Engine : public PhysiologyEngine
  {
  public:
    Engine(Logger* logger=nullptr);
    virtual ~Engine();

    Logger* GetLogger() const override;

    std::string GetTypeName() const override;

    bool SerializeFromFile(const std::string& file) override;
    bool SerializeToFile(const std::string& file) const override;

    bool SerializeFromString(const std::string& state, eSerializationFormat m) override;
    bool SerializeToString(std::string& state, eSerializationFormat m) const override;

    bool InitializeEngine(const std::string& patient_configuration, eSerializationFormat m) override;
    bool InitializeEngine(const SEPatientConfiguration& patient_configuration) override;
    eEngineInitializationState GetInitializationState() const override;

    void Clear() override;

    bool SetConfigurationOverride(const SEEngineConfiguration* config) override;

    const SEConditionManager& GetConditionManager() const override;

    SEEngineTracker* GetEngineTracker() const override;

    const SEEngineConfiguration* GetConfiguration() const override;

    double GetTimeStep(const TimeUnit& unit) const override;
    double GetSimulationTime(const TimeUnit& unit) const override;
    void   SetSimulationTime(const SEScalarTime& time) override;
    double GetStabilizationTime(const TimeUnit& unit) const override;

    bool  AdvanceModelTime() override;
    bool  AdvanceModelTime(double time, const TimeUnit& unit) override;
    bool  ProcessAction(const SEAction& action) override;
    const SEActionManager& GetActionManager() const override;

    const SESubstanceManager& GetSubstanceManager() const override;
    void  SetAdvanceHandler(SEAdvanceHandler* handler) override;
    const SEPatient& GetPatient() const override;
    const SEPatient& GetInitialPatient() const override;
    bool  GetPatientAssessment(SEPatientAssessment& assessment) const override;

    const SEEnvironment* GetEnvironment() const override;
    const SEBloodChemistrySystem* GetBloodChemistrySystem() const override;
    const SECardiovascularSystem* GetCardiovascularSystem() const override;
    const SEDrugSystem* GetDrugSystem() const override;
    const SEEndocrineSystem* GetEndocrineSystem() const override;
    const SEEnergySystem* GetEnergySystem() const override;
    const SEGastrointestinalSystem* GetGastrointestinalSystem() const override;
    const SEHepaticSystem* GetHepaticSystem() const override;
    const SENervousSystem* GetNervousSystem() const override;
    const SERenalSystem* GetRenalSystem() const override;
    const SERespiratorySystem* GetRespiratorySystem() const override;
    const SETissueSystem* GetTissueSystem() const override;
    const SEAnesthesiaMachine* GetAnesthesiaMachine() const override;
    const SEBagValveMask* GetBagValveMask() const override;
    const SEECMO* GetECMO() const override;
    const SEElectroCardioGram* GetElectroCardioGram() const override;
    const SEInhaler* GetInhaler() const override;
    const SEMechanicalVentilator* GetMechanicalVentilator() const override;

    const SECompartmentManager& GetCompartments() const override;
    SEBlackBoxManager& GetBlackBoxes() const override;

    const SEEventManager& GetEventManager() const override;

    Controller& GetController() const;
  protected:
    virtual void AllocateController() const = 0;
    mutable Controller* m_Controller=nullptr;
  };
END_NAMESPACE
