/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

#include "engine/CommonDefs.h"
#include "engine/PulseConfiguration.h"
#include "engine/common/controller/BlackBoxManager.h"
#include "engine/common/controller/CircuitManager.h"
#include "engine/common/controller/CompartmentManager.h"
#include "engine/common/controller/SubstanceManager.h"

// CDM
class DataTrack;
class SEActionManager;
class SEConditionManager;
class SEDataRequested;
class SEEngineTracker;
#include "cdm/engine/SEEventManager.h"
#include "cdm/engine/SEEngineStabilization.h"
#include "cdm/properties/SEScalarTime.h"

namespace pulse
{
  class StabilizationController;
  class Model;

  class EnvironmentModel;

  class BloodChemistryModel;
  class CardiovascularModel;
  class EndocrineModel;
  class EnergyModel;
  class GastrointestinalModel;
  class HepaticModel;
  class NervousModel;
  class RenalModel;
  class RespiratoryModel;
  class DrugModel;
  class TissueModel;

  class AnesthesiaMachineModel;
  class BagValveMaskModel;
  class ECMOModel;
  class ElectroCardioGramModel;
  class InhalerModel;
  class MechanicalVentilatorModel;

  class SaturationCalculator;

  enum class EngineState
  {
    NotReady = 0,
    Initialization,
    InitialStabilization,
    AtInitialStableState,
    SecondaryStabilization,
    AtSecondaryStableState,
    Active,
    Fatal
  };

  // Keep enums in sync with appropriate proto file !!
  enum class eAirwayMode
  {
    Free = 0,
    AnesthesiaMachine,
    BagValveMask,
    Inhaler,
    MechanicalVentilation,// Action
    MechanicalVentilator, // Equipment
    NasalCannula,
    NonRebreatherMask,
    SimpleMask
  };
  extern PULSE_DECL const std::string& eAirwayMode_Name(eAirwayMode m);

  class PULSE_DECL Data : public Loggable
  {
    friend class Engine;
  protected:// Create in an Engine
    Data(Logger* logger = nullptr);
  public:
    virtual ~Data();

    virtual std::string GetDataRoot() const { return m_DataDir; }
    virtual void SetDataRoot(const std::string& dir) { m_DataDir = dir; }

    virtual void                          AdvanceCallback(double time_s);

    virtual EngineState                   GetState() const { return m_State; }

    virtual SEEngineTracker&              GetEngineTracker() const;
    virtual DataTrack&                    GetDataTrack() const;
    virtual SaturationCalculator&         GetSaturationCalculator() const;

    virtual SubstanceManager&             GetSubstances() const;

    virtual const SEPatient&              GetInitialPatient() const;
    virtual SEPatient&                    GetCurrentPatient() const;

    virtual bool                          HasBloodChemistry() const;
    virtual SEBloodChemistrySystem&       GetBloodChemistry() const;
    virtual bool                          HasCardiovascular() const;
    virtual SECardiovascularSystem&       GetCardiovascular() const;
    virtual bool                          HasDrugs() const;
    virtual SEDrugSystem&                 GetDrugs() const;
    virtual bool                          HasEndocrine() const;
    virtual SEEndocrineSystem&            GetEndocrine() const;
    virtual bool                          HasEnergy() const;
    virtual SEEnergySystem&               GetEnergy() const;
    virtual bool                          HasGastrointestinal() const;
    virtual SEGastrointestinalSystem&     GetGastrointestinal() const;
    virtual bool                          HasHepatic() const;
    virtual SEHepaticSystem&              GetHepatic() const;
    virtual bool                          HasNervous() const;
    virtual SENervousSystem&              GetNervous() const;
    virtual bool                          HasRenal() const;
    virtual SERenalSystem&                GetRenal() const;
    virtual bool                          HasRespiratory() const;
    virtual SERespiratorySystem&          GetRespiratory() const;
    virtual bool                          HasTissue() const;
    virtual SETissueSystem&               GetTissue() const;

    virtual bool                          HasEnvironment() const;
    virtual SEEnvironment&                GetEnvironment() const;
    virtual bool                          HasAnesthesiaMachine() const;
    virtual SEAnesthesiaMachine&          GetAnesthesiaMachine() const;
    virtual bool                          HasBagValveMask() const;
    virtual SEBagValveMask&               GetBagValveMask() const;
    virtual bool                          HasECG() const;
    virtual SEElectroCardioGram&          GetECG() const;
    virtual bool                          HasECMO() const;
    virtual SEECMO&                       GetECMO() const;
    virtual bool                          HasInhaler() const;
    virtual SEInhaler&                    GetInhaler() const;
    virtual bool                          HasMechanicalVentilator() const;
    virtual SEMechanicalVentilator&       GetMechanicalVentilator() const;

    virtual SEActionManager&              GetActions() const;

    virtual SEConditionManager&           GetConditions() const;

    virtual SEEventManager&               GetEvents() const;

    virtual CircuitManager&               GetCircuits() const;

    virtual CompartmentManager&           GetCompartments() const;

    virtual BlackBoxManager&              GetBlackBoxes() const;

    virtual const PulseConfiguration&     GetConfiguration() const;

    virtual double                        GetTimeStep_s() const;
    virtual const SEScalarTime&           GetTimeStep() const;
    virtual const SEScalarTime&           GetEngineTime() const;
    virtual const SEScalarTime&           GetSimulationTime() const;
    virtual const SEScalarTime&           GetStabilizationTime() const;

    virtual bool                          IsAirwayModeSupported(eAirwayMode /*mode*/) { return true; }
    virtual eAirwayMode                   GetAirwayMode() const { return m_AirwayMode; }
    virtual void                          SetAirwayMode(eAirwayMode mode);

    virtual eSwitch                       GetIntubation() const { return m_Intubation; }

    virtual void                          SetAdvanceHandler(SEAdvanceHandler* handler) { m_AdvanceHandler = handler; }

    virtual const SEScalarProperties&     GetOverrides() const;

    std::stringstream                     m_ss;
  protected:
    virtual void                          SetupTracker();

    EngineState                           m_State;
    SEEngineTracker*                      m_EngineTrack;
    SEDataRequested*                      m_DataRequested;
    eEngineInitializationState            m_EngineInitializationState;

    SEScalarTime                          m_CurrentTime;
    SEScalarTime                          m_SimulationTime;
    SEScalarTime                          m_StabilizationTime;
    double                                m_SpareAdvanceTime_s;
    eAirwayMode                           m_AirwayMode;
    eSwitch                               m_Intubation;

    PulseConfiguration*                   m_Config = nullptr;
    SaturationCalculator*                 m_SaturationCalculator = nullptr;

    SubstanceManager*                     m_Substances = nullptr;

    SEActionManager*                      m_Actions = nullptr;
    SEConditionManager*                   m_Conditions = nullptr;
    CircuitManager*                       m_Circuits = nullptr;
    CompartmentManager*                   m_Compartments = nullptr;
    BlackBoxManager*                      m_BlackBoxes = nullptr;

    EnvironmentModel*                     m_EnvironmentModel = nullptr;

    BloodChemistryModel*                  m_BloodChemistryModel = nullptr;
    CardiovascularModel*                  m_CardiovascularModel = nullptr;
    EndocrineModel*                       m_EndocrineModel = nullptr;
    EnergyModel*                          m_EnergyModel = nullptr;
    GastrointestinalModel*                m_GastrointestinalModel = nullptr;
    HepaticModel*                         m_HepaticModel = nullptr;
    NervousModel*                         m_NervousModel = nullptr;
    RenalModel*                           m_RenalModel = nullptr;
    RespiratoryModel*                     m_RespiratoryModel = nullptr;
    DrugModel*                            m_DrugModel = nullptr;
    TissueModel*                          m_TissueModel = nullptr;

    AnesthesiaMachineModel*               m_AnesthesiaMachineModel = nullptr;
    BagValveMaskModel*                    m_BagValveMaskModel = nullptr;
    ECMOModel*                            m_ECMOModel = nullptr;
    ElectroCardioGramModel*               m_ElectroCardioGramModel = nullptr;
    InhalerModel*                         m_InhalerModel = nullptr;
    MechanicalVentilatorModel*            m_MechanicalVentilatorModel = nullptr;

    SEPatient*                            m_InitialPatient = nullptr;
    SEPatient*                            m_CurrentPatient = nullptr;

    SEEventManager*                       m_EventManager = nullptr;
    LoggerForward*                        m_LogForward = nullptr;

    SEAdvanceHandler*                     m_AdvanceHandler = nullptr;

    std::string                           m_DataDir;

    SEScalarProperties                    m_ScalarOverrides;
  protected:
    std::vector<pulse::Model*>            m_Models;
  };

  /**
  * @brief Manages and controls execution of all data/Models in %Pulse
  */
  class PULSE_DECL Controller : public Data
  {
    friend class Engine;
    friend class EngineTest;
    friend class PBState;//friend the serialization class
  protected:// Create via Engine
    Controller(Logger* logger = nullptr);
  public:
    virtual ~Controller();

    virtual Data& GetData() { return (*this); }
    virtual const Data& GetData() const { return (*this); }

    virtual bool SerializeFromFile(const std::string& file);
    virtual bool SerializeToFile(const std::string& file) const;

    virtual bool SerializeFromString(const std::string& state, eSerializationFormat m);
    virtual bool SerializeToString(std::string& state, eSerializationFormat m) const;

    virtual bool InitializeEngine(const std::string& patient_configuration, eSerializationFormat m);
    virtual bool InitializeEngine(const SEPatientConfiguration& patient_configuration);
    virtual bool IsReady() const;
    virtual eEngineInitializationState GetInitializationState() const;

    virtual void Clear();

    virtual bool SetConfigurationOverride(const SEEngineConfiguration* config);

    virtual void SetSimulationTime(const SEScalarTime& time);

    virtual bool  AdvanceModelTime();
    virtual bool  AdvanceModelTime(double time, const TimeUnit& unit);
    virtual bool  ProcessAction(const SEAction& action);

    virtual bool GetPatientAssessment(SEPatientAssessment& assessment) const = 0;

    virtual bool CreateCircuitsAndCompartments();

    virtual void CheckIntubation();
  protected:
    virtual std::string GetTypeName() const = 0;
    virtual void LogBuildInfo() const;
    // Setup Circuit/Compartments for systems

    // Default/Optimal Cardiovascular
    virtual void SetupCardiovascular();
    virtual void SetupRenal();
    virtual void SetupTissue();
    virtual void SetupCerebrospinalFluid();

    // Optional Expanded Cardiovascular Circuit Setup Methods
    virtual void SetupExpandedCardiovascular();
    virtual void SetupExpandedCardiovascularRenal();
    virtual void SetupExpandedCardiovascularTissue();
    virtual void SetupExpandedCardiovascularCerebrospinalFluid();

    // Default/Optimal Gastrointestinal
    virtual void SetupGastrointestinal();

    // Default/Optimal Respiratory
    virtual void SetupRespiratory();

    // Optional Expanded Respiratory and Cardiovascular Circuit Setup Methods
    virtual void SetupExpandedPulmonaryRespiratory();
    virtual void SetupExpandedPulmonaryCardiovascular();

    // Default/Optimal Equipment
    virtual void SetupAnesthesiaMachine();
    virtual void SetupBagValveMask();
    virtual void SetupECMO();
    virtual void SetupInhaler();
    virtual void SetupMechanicalVentilation();
    virtual void SetupMechanicalVentilator();
    virtual void SetupNasalCannula();
    virtual void SetupSimpleMask();
    virtual void SetupNonRebreatherMask();

    // Default/Optimal Energy
    virtual void SetupExternalTemperature();
    virtual void SetupInternalTemperature();

    virtual bool OverrideCircuits();
    virtual bool ModifyCircuits(const SEScalarProperties& modifiers);

    virtual bool Initialize(const SEPatient& patient);
    virtual bool Stabilize(const SEPatientConfiguration& patient_configuration);

    // Allocate all the models this engine supports
    virtual void Allocate();
    virtual bool SetupPatient(const SEPatient& patient) = 0;
    // Based on what modles are used, setup order for the following
    virtual void InitializeModels();
    // Notify Models that steady state has been achieved
    virtual void AtSteadyState(EngineState state);
    virtual void PreProcess();
    virtual void Process();
    virtual void PostProcess();

    PulseConfiguration const*m_ConfigOverride = nullptr;
    StabilizationController *m_Stabilizer = nullptr;
  };

  class PULSE_DECL StabilizationController : public SEEngineStabilization::Controller
  {
  public:
    StabilizationController(pulse::Controller& pc) : _pc(pc) {}
    virtual ~StabilizationController() = default;

    bool AdvanceTime() override { return _pc.AdvanceModelTime(); }
    SEEngineTracker* GetEngineTracker() override
    {
      return &_pc.GetData().GetEngineTracker();
    }
    double GetTimeStep(const TimeUnit& unit) override
    {
      return _pc.GetData().GetTimeStep().GetValue(unit);
    }
    double GetSimulationTime(const TimeUnit& unit) override
    {
      return _pc.GetData().GetSimulationTime().GetValue(unit);
    }

  protected:
    pulse::Controller& _pc;
  };

  class FatalListner : public LoggerForward
  {
  public:
    FatalListner(SEEventManager& mgr, SEScalarTime& ct) : m_Events(mgr), m_CurrentTime(ct) {};
    ~FatalListner() = default;

    void ForwardDebug(const std::string& /*msg*/) override { }
    void ForwardInfo(const std::string& /*msg*/) override  { }
    void ForwardWarning(const std::string& /*msg*/) override  { }
    void ForwardError(const std::string& /*msg*/) override  { }
    void ForwardFatal(const std::string& /*msg*/) override
    {
      m_Events.SetEvent(eEvent::IrreversibleState, true, m_CurrentTime);
      throw IrreversibleStateException(); // Caught in Common::AdvanceModelTime, so we do not do anything more in the engine
    }

  protected:
    SEEventManager& m_Events;
    SEScalarTime& m_CurrentTime;
  };
END_NAMESPACE
