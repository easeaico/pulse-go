/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include "cdm/CommonDefs.h"
#include "cdm/utils/Logger.h"

class SEAdvanceHandler;
class SEPatient;
class SEPatientConfiguration;
class SEPatientAssessment;

class SECondition;
class SEAction;

class SESubstanceManager;
class SEBloodChemistrySystem;
class SECardiovascularSystem;
class SEDrugSystem;
class SEEndocrineSystem;
class SEEnergySystem;
class SEGastrointestinalSystem;
class SEHepaticSystem;
class SENervousSystem;
class SERenalSystem;
class SERespiratorySystem;
class SETissueSystem;
class SEEnvironment;
class SEAnesthesiaMachine;
class SEBagValveMask;
class SEECMO;
class SEElectroCardioGram;
class SEInhaler;
class SEMechanicalVentilator;
class SECompartmentManager;
class SEBlackBoxManager;
class SEActionManager;
class SEConditionManager;
class SEEventManager;

class SEEngineTracker;
class SEEngineConfiguration;

enum class eEngineInitializationState
{
  Uninitialized = 0,
  FailedState = 1,
  FailedPatientSetup = 2,
  FailedStabilization = 3,
  Initialized = 4
};
extern CDM_DECL const std::string& eEngineInitializationState_Name(eEngineInitializationState s);

/**
 * @brief
 * Base exception class that all CDM classes throw when an error occurs
 */
struct PhysiologyEngineException : public CommonDataModelException
{
  PhysiologyEngineException()
    : CommonDataModelException( "Physiology Engine Error" ) {}

  PhysiologyEngineException(const std::string& _Message)
    : CommonDataModelException(_Message) {}
};

class CDM_DECL PhysiologyEngine : public Loggable
{
public:
  PhysiologyEngine(Logger* logger = nullptr) : Loggable(logger) {}
  virtual ~PhysiologyEngine() = default;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Returns the engine type, such as "Human Adult Whole Body"
  //--------------------------------------------------------------------------------------------------
  virtual std::string GetTypeName() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Reset engine and set it to the state in the provided file.
  /// The file may contain json or binary.
  /// Anything but an extension of .json will be interpreted as binary.
  /// Return value indicates engine was able to load provided state file.
  /// Engine will be in a cleared state if this method fails.
  //--------------------------------------------------------------------------------------------------
  virtual bool SerializeFromFile(const std::string& file) = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Save the current state of the engine to provided filename.
  /// Using a .json extension will save a json/ascii file.
  /// Anything else will save as binary.
  //--------------------------------------------------------------------------------------------------
  virtual bool SerializeToFile(const std::string& filename) const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Reset engine and set it to the state in the provided string.
  /// The string can contain JSON or binary.
  /// Note that a string of bytes are binary, not text; we only use the string class as a convenient container.
  /// Return value indicates engine was able to load provided state file.
  /// Engine will be in a cleared state if this method fails.
  //--------------------------------------------------------------------------------------------------
  virtual bool SerializeFromString(const std::string& state, eSerializationFormat m) = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Save the current state of the engine.
  /// The state can be saved as JSON or bytes in the given string.
  /// Note that the bytes are binary, not text; we only use the string class as a convenient container.
  //--------------------------------------------------------------------------------------------------
  virtual bool SerializeToString(std::string& state, eSerializationFormat m) const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Initialize an engine based on the engines initialization structure
  ///
  /// An initialization structure is up to every engine.
  /// It should have a SEPatientConfiguration (or derivative)
  /// But it may also have more stuctures associated with it, it's up to the engine.
  ///
  //--------------------------------------------------------------------------------------------------
  virtual bool InitializeEngine(const std::string& patient_configuration, eSerializationFormat m) = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  ///
  /// This will create an engine that you can send instructions (patient,actions,conditions) to dynamically.
  /// The return value will indicate success failure of the creation of the engine.
  /// Some combinations of patients and conditions may prevent the engine from stabilizing
  ///
  //--------------------------------------------------------------------------------------------------
  virtual bool InitializeEngine(const SEPatientConfiguration& patient_configuration) = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  ///
  /// Get a bit more detail as to how initialization went
  ///
  //--------------------------------------------------------------------------------------------------
  virtual eEngineInitializationState GetInitializationState() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  ///
  /// Closes out any files and resets state
  ///
  //--------------------------------------------------------------------------------------------------
  virtual void Clear() = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Get the Condition Manager.
  /// Allows a user to check the state of active conditions
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SEConditionManager& GetConditionManager() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Engines can have a configuration for allowing a user to set certain internal parameters
  /// Engines with configurations will have all configuration parameters defaulted,
  /// This allows you to change one or more or those parameters.
  /// The parameters provided will be applied during SerializeFrom* and InitializeEngine methods.
  /// Use with caution! (Use nullptr to revert back to using all engine defaults)
  //--------------------------------------------------------------------------------------------------
  virtual bool SetConfigurationOverride(const SEEngineConfiguration* config) = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// returns the engine configuration.
  //--------------------------------------------------------------------------------------------------
  virtual const SEEngineConfiguration* GetConfiguration() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Retrieve the SEEngineTracker associated with tracking data from this engine to a file
  /// The SEEngineTracker is a class set up to pull data requested from the engine (via SEDataRequest's)
  /// and provide access to the data easily (ex. to easily write data to csv files)
  //--------------------------------------------------------------------------------------------------
  virtual SEEngineTracker* GetEngineTracker() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// returns the engine time step that is used when advancing time.
  ///
  //--------------------------------------------------------------------------------------------------
  virtual double GetTimeStep(const TimeUnit& unit) const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// returns the current time of the simulation.
  //--------------------------------------------------------------------------------------------------
  virtual double GetSimulationTime(const TimeUnit& unit) const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// returns the stabilization time of the simulation (if stabilized)
  //--------------------------------------------------------------------------------------------------
  virtual double GetStabilizationTime(const TimeUnit& unit) const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Set the current time of the simulation.
  /// Engine Simulation time will be advanced from this time point
  //--------------------------------------------------------------------------------------------------
  virtual void SetSimulationTime(const SEScalarTime& time) = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// executes one pass through the time loop of the engine at the fixed timestep
  ///
  /// Events, errors, and warning as are logged to file not errors are returned
  /// through the API at this time.
  ///
  //--------------------------------------------------------------------------------------------------
  virtual bool AdvanceModelTime() = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// executes time loop of the engine beginning at the current time
  ///  and running for the duration specified in the call at the fixed timestep
  ///
  /// Events, errors, and warning as are logged to file not errors are returned
  /// through the API at this time.
  ///
  //--------------------------------------------------------------------------------------------------
  virtual bool AdvanceModelTime(double time, const TimeUnit& unit) = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Add a callback object that will be called after each timestep
  //--------------------------------------------------------------------------------------------------
  virtual void SetAdvanceHandler(SEAdvanceHandler* handler) = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Execute the provided action.
  /// true will be returned if the engine supports the action
  /// false will be returned if the engine does not support the action
  ///
  //--------------------------------------------------------------------------------------------------
  virtual bool ProcessAction(const SEAction& action) = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Get the Action Manager.
  /// Allows a user to check the state of active actions
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SEActionManager& GetActionManager() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Retrieves the associated substance manager.
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SESubstanceManager& GetSubstanceManager() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Retrieves the associated event manager.
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SEEventManager& GetEventManager() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Returns the patient object used by the engine
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SEPatient& GetPatient() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Returns the initial simulation patient object used by the engine
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SEPatient& GetInitialPatient() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Determines the assessment type and fills the data object with current data.
  ///
  /// Assessments can be queried at any point in the calculation and as many times are desired.
  ///
  //--------------------------------------------------------------------------------------------------
  virtual bool GetPatientAssessment(SEPatientAssessment& assessment) const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Returns the environment object used by the engine
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SEEnvironment* GetEnvironment() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Returns the current state of the Blood Chemistry System
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SEBloodChemistrySystem* GetBloodChemistrySystem() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Returns the current state of the Cardiovascular System
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SECardiovascularSystem* GetCardiovascularSystem() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Returns the current state of the drug system
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SEDrugSystem* GetDrugSystem() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Returns the current state of the Endocrine System
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SEEndocrineSystem* GetEndocrineSystem() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Returns the current state of the Energy System
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SEEnergySystem* GetEnergySystem() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Returns the current state of the Gastrointestinal System
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SEGastrointestinalSystem* GetGastrointestinalSystem() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Returns the current state of the Hepatic System
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SEHepaticSystem* GetHepaticSystem() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Returns the current state of the Nervous System
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SENervousSystem* GetNervousSystem() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Returns the current state of the Renal System
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SERenalSystem* GetRenalSystem() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Returns the current state of the Respiratory System
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SERespiratorySystem* GetRespiratorySystem() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Returns the current state of the tissue system
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SETissueSystem* GetTissueSystem() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Returns the current state of the Anesthesia machine
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SEAnesthesiaMachine* GetAnesthesiaMachine() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Returns the current state of the Bag Valve Mask
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SEBagValveMask* GetBagValveMask() const = 0;

  //--------------------------------------------------------------------------------------------------
 /// \brief
 /// Returns the current state of the ECMO machine
 ///
 //--------------------------------------------------------------------------------------------------
  virtual const SEECMO* GetECMO() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Returns the current state of the Electrocardiogram machine
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SEElectroCardioGram* GetElectroCardioGram() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Returns the current state of the Inhaler
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SEInhaler* GetInhaler() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Returns the current state of the Mechanical ventilator
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SEMechanicalVentilator* GetMechanicalVentilator() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Retrieves the engine compartments, providing such data as:
  /// flows, pressure, volume as well as substance volumes and volume fractions.
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SECompartmentManager& GetCompartments() const = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Retrieves the engine black box manager
  /// Black boxes allow users to inject or retrieve flows, pressure, volume
  /// as well as substance volumes and volume fractions into the engine, use with caution.
  /// These are intended to allow integration between Pulse and other systems at a low level.
  ///
  //--------------------------------------------------------------------------------------------------
  virtual SEBlackBoxManager& GetBlackBoxes() const = 0;
};
