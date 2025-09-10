Physiology Engine Interface {#physeng}
===========================

The following information is a method by method walk through of the %PhysiologyEngine.h class that everything revolves around.
Reading this in parallel with the HowTo-EngineUse.cpp provided in the SDK will give you a firm understanding of using the engine.

When you create an instance of an engine, you will be returned a pointer to a PhysiologyEngine object.
This generic interface is the controlling class for a physiology engine modeling a single patient.

@code 
  std::unique_ptr<PhysiologyEngine> bg = CreatePulseEngine("MyEngine.log");
@endcode

@anchor LoggingInterface
### Logging

The Pulse Logger class can write messages from the engine to the console and/or a file.
You can also provide a callback class to the logger for the engine to forward all logging events to your application so it can easily monitor the engine.
The %Logger class is used to specify the name of a file to write messages to. (An empty string will disable writing to file),
You may also turn on and off writing of messages to the console.

It is highly recommended to create a log, and check it often. Many problems can be identified through the log.
You can access the log at any time, the engine does not have to be initialized or have a state loaded.

For more details, please consult the HowTo-EngineUse.cpp file in the SDK.
You can access and utilize this logger with in your own application as such:
@code
  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Retrieve the Logger associated with this engine
  //--------------------------------------------------------------------------------------------------
  virtual Logger* GetLogger() = 0;
@endcode

@anchor EngineInitialization
### Initializing the Engine

There are two ways to initialize the engine once created.

@anchor EngineState
#### Engine State

An engine state file is the exact state of the engine written into an file/string. 
Once loaded, the engine is instantly ready to process instructions.
You can specify a file on disk, or the string contents of a state held in memory.
The Pulse build can provide a set of state files for various patients at simulation time 0 located in the bin/states directory.
When providing a state (from file or string) Pulse will not need to access any files on disk.
All required data is provided in the state itself.

@code
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
  /// Reset engine and set it to the state in the provided string.
  /// The string can contain JSON or binary.
  /// Note that a string of bytes are binary, not text; we only use the string class as a convenient container.
  /// Return value indicates engine was able to load provided state file.
  /// Engine will be in a cleared state if this method fails.
  //--------------------------------------------------------------------------------------------------
  virtual bool SerializeFromString(const std::string& state, SerializationFormat m) = 0;
@endcode

At any point during the life of an engine, you create your own state object for later use.
@code
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

  //--------------------------------------------------------------------------------------------------
  virtual bool SerializeFromString(const std::string& state, SerializationFormat m) = 0;
@endcode

@anchor PatientCreation
#### Patient Creation

If you would like to create your own patient or apply a condition to a specific patient, 
you will need to initialize the engine with a patient definition.
When creating a new patient state, Pulse need to access files on disk.
You can specify the root directory containing these files in the provided SEPatientConfiguration object/string.
More about files required by Pulse can be found on our [wiki](https://gitlab.kitware.com/physiology/engine/-/wikis/how-to-connect-to-pulse#application-execution)

@code
  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// locates the xml patient file and reads in the values. 
  ///
  /// This will create an engine that you can send instructions (patient,actions,conditions) to dynamically.
  /// The return value will indicate success failure of the creation of the engine.  
  /// Some combinations of patients and conditions may prevent the engine from stabilizing
  ///
  //--------------------------------------------------------------------------------------------------
  virtual bool InitializeEngine(const std::string& patientConfiguration, SerializationFormat m) = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  ///
  /// This will create an engine that you can send instructions (patient,actions,conditions) to dynamically.
  /// The return value will indicate success failure of the creation of the engine.  
  /// Some combinations of patients and conditions may prevent the engine from stabilizing
  ///
  //--------------------------------------------------------------------------------------------------
  virtual bool InitializeEngine(const SEPatientConfiguration& patientConfiguration) = 0;
@endcode

@anchor ConditionsInterface
When a patient definition is provided, the engine will go through an initialization algorithm that will take several minutes
as to tune the engine to model the specific state requested of the patient. 
This initialization method is also the only way to specify any conditions (chronic/disease states).
The following conditions are specified via the SEPatientConfiguration object.

#### Patient Conditions
@secreflist
  @refitem AcuteRespiratoryDistressSyndromeTable "ARDS"
  @refitem ChronicAnemiaTable "Chronic Anemia"
  @refitem ChronicObstructivePulmonaryDiseaseTable "COPD"
  @refitem ChronicPericardialEffusionTable "Chronic Pericardial Effusion"
  @refitem ChronicRenalStenosisTable "Chronic Renal Stenosis"
  @refitem ConsumeMealTable "Consume Meal"
  @refitem ImpairedAlveolarExchangeTable "Impaired Alveolar Exchange"
  @refitem PneumoniaTable "Pneumonia"
  @refitem PulmonaryFibrosisTable "Pulmonary Fibrosis"
  @refitem PulmonaryShuntTable "Pulmonary Shunt"
  @refitem SepsisTable "Sepsis"
@endsecreflist

#### Environment Conditions
@secreflist
  @refitem InitialEnvironmentConditionsTable "Initial Environment"
@endsecreflist

<br>
Adding conditions to the patient configuration object will extend the initialization time by a few more minutes.
Once the InitalizeEngine method returns, the engine is stabilize, and it is recommended to save the engine state for future use.
(Assuming the patient vitals are acceptable, it may take some adjusting of condition severity to get a desired patient state with conditions applied.)
The SDK provides multiple tested patient files for use in the bin/patients directory. For more information on this look at the @ref PatientMethodology.

@code
  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Get the Condition Manager.
  /// Allows a user to check the state of active conditions
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SEConditionManager& GetConditionManager() const = 0;
@endcode

@anchor EngineConfiguration
### Engine Configuration

The engine configuration is used to tweek various variables associated in the engine methodology.
Configuration modification requires a very in-depth knowledge of the engine.
It is not recommended to provide another configuration unless you know what effects it will have.
Note that if you provide a configuration, you can specify any number of configuration properties (1 or more).
And only those specified values will be used to replace the default configuration values.
There are some useful configuration options that you may want to change, such as writing data to a csv file while the initialization algorithm executes.
Come visit us on the forums if this is something you want to know more about.

You can provide an engine configuration parameter to the engine via this method:
(Note you will need to instantiate a PulseConfiguration object to pass to this method)

@code
  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Engines can have a configuration for allowing a user to set certain internal parameters
  /// Engines with configurations will have all configuration parameters defaulted,
  /// This allow you to change one or more or those parameters.
  /// The parameters provided will be applied during SerializeFrom* and InitializeEngine methods.
  /// Use with caution! (Use nullptr to revert back to using all engine defaults)
  //--------------------------------------------------------------------------------------------------
  virtual bool SetConfigurationOverride(const SEEngineConfiguration* config) = 0;
@endcode

You can retrieve and view the configuration with this method: 
(note you will need to cast it to the %PulseConfiguration class to see configuration data)

@code
  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// returns the engine configuration.   
  //--------------------------------------------------------------------------------------------------
  virtual const PhysiologyEngineConfiguration* GetConfiguration() = 0;
@endcode

@anchor DataTracking
### Data Tracking

The engine has the ability to write specifically requested data to a comma seperated text file as the engine advances time.
These csv file are very helpful in debugging and ensuring the engine is modeling correctly.
How to utilize this functionality is demonstrated in all the HowTo files provided in the SDK.

@code
  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Retrieve the PhysiologyEngineTrack associated with tracking data from this engine to a file
  //--------------------------------------------------------------------------------------------------
  virtual PhysiologyEngineTrack* GetEngineTrack() = 0;
@endcode

@anchor AdvanceTime
### Advancing The Simulation In Time

Once the engine is initialized, it is ready to accept direction.
The engine does not advance time on its own, you must explicitly tell the engine to simulate a specific amount of time.
It is recommended to not to pull data from or provide actions to the engine during simulation.
It is the responsibility of the end user to ensure that a multi-threaded system adheres to this practice as the Pulse engine does not block I/O access during simulation.
There is a HowTo example that shows how to encapsulate the engine in a thread class that automatically advances time and processes actions.
How you decide to drive the simulation is up to you, but you must explicitly advance time in order for the models to simulate physiology.

@code
  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// executes one pass through the time loop of the engine at the fixed time step
  ///
  /// Events, errors, and warning as are logged to file not errors are returned
  /// through the API at this time. 
  ///
  //--------------------------------------------------------------------------------------------------
  virtual void AdvanceModelTime() = 0;

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// executes one pass through the time loop of the engine at the fixed time step
  ///
  /// Events, errors, and warning as are logged to file not errors are returned
  /// through the API at this time. 
  ///
  //--------------------------------------------------------------------------------------------------
  virtual void AdvanceModelTime(double time, const TimeUnit& unit) = 0;
@endcode

If no time is provided, the engine will simulate for the smallest amount of time it can.
The minimum amount of time the engine can simulate is the engines' time step, you can retrieve this duration with this method:

@code
  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// returns the engine time step that is used when advancing time. 
  ///
  //--------------------------------------------------------------------------------------------------
  virtual double GetTimeStep(const TimeUnit& unit) = 0;
@endcode

You can retrieve the total amount of time the engine has simulated with the following call:

@code
  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// returns the current time of the simulation.   
  //--------------------------------------------------------------------------------------------------
  virtual double GetSimulationTime(const TimeUnit& unit) = 0;
@endcode
Note that the simulation time is 0 when the InitializeEngine method returns.

You can set the simulation time of the engine with the following call:
It will then be incremented from that point on whenever you call AdvanceModelTime.

@code
  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Set the current time of the simulation.
  /// Engine Simulation time will be advanced from this time point
  //--------------------------------------------------------------------------------------------------
  virtual void SetSimulationTime(const SEScalarTime& time) = 0;
@endcode

You can provide a callback object with the following call:
The engine will call the method implementation after each time step.

@code
  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Add a callback object that will be called after each timestep
  //--------------------------------------------------------------------------------------------------
  virtual void SetAdvanceHandler(SEAdvanceHandler* handler) = 0;
@endcode

@anchor ProcessActions
### Process Action

Actions are the means by which instructions are provided to a physiology engine.
You will need to create an instance of an action class, fill it out with the necessary data and pass it into this method.
@code
  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Execute the provided action.
  /// true will be returned if the engine supports the action
  /// false will be returned if the engine does not support the action
  ///
  //--------------------------------------------------------------------------------------------------
  virtual bool ProcessAction(const SEAction& action) = 0;
@endcode

#### Patient Actions
@secreflist
  @refitem AcuteRespiratoryDistressSyndromeExacerbationTable "ARDS Exacerbation"
  @refitem AcuteStressTable "Acute Stress"
  @refitem AirwayObstructionTable "Airway Obstruction"
  @refitem ArrhythmiaTable "Arrhythmia"
  @refitem AsthmaAttackTable "Asthma Attack"
  @refitem BrainInjuryTable "Brain Injury"
  @refitem BronchoconstrictionTable "Bronchoconstriction"
  @refitem ChestCompressionTable "Chest Compression"
  @refitem ChestCompressionAutomatedTable "Chest Compression Automated"
  @refitem ChestCompressionInstantaneousTable "Chest Compression Instantaneous Scale"
  @refitem ChestOcclusiveDressingTable "Chest Occlusive Dressing"
  @refitem ConsciousRespirationTable "Conscious Respiration"
  @refitem ConsumeNutrientsTable "Consume Nutrients"
  @refitem ChronicObstructivePulmonaryDiseaseExacerbationTable "COPD Exacerbation"
  @refitem ConsciousRespirationTable "Conscious Respiration"
  @refitem ConsumeNutrientsTable "Consume Nutrients"
  @refitem DyspneaTable "Dyspnea"
  @refitem ExerciseTable "Exercise"
  @refitem HemorrhageTable "Hemorrhage"
  @refitem ImpairedAlveolarExchangeExacerbationTable "Impaired Alveolar Exchange Exacerbation"
  @refitem IntubationTable "Intubation"
  @refitem PneumoniaExacerbationTable "Pneumonia Exacerbation"
  @refitem MechanicalVentilationTable "Mechanical Ventilation"
  @refitem NeedleDecompressionTable "Needle Decompression"
  @refitem PericardialEffusionTable "Pericardial Effusion"
  @refitem PrimaryBlastLungInjuryTable "Primary Blast Lung Injury"
  @refitem PulmonaryShuntExacerbationTable "Pulmonary Shunt Exacerbation"
  @refitem RespiratoryFatigueTable "Respiratory Fatigue"
  @refitem RespiratoryMechanicsConfigurationTable "Respiratory Mechanics Configuration"
  @refitem SubstanceBolusTable "Substance Bolus"
  @refitem SubstanceCompoundInfusionTable "Substance Compound Infusion"
  @refitem SubstanceInfusionTable "Substance Infusion"
  @refitem SupplementalOxygenTable "Supplemental Oxygen"
  @refitem TensionPneumothoraxTable "Tension Pneumothorax"
  @refitem UrinateTable "Urinate"
@endsecreflist

#### Environment Actions
@secreflist
  @refitem ChangeEnvironmentConditionsTable "Environment Change"
  @refitem ThermalApplicationTable "Thermal Application"
@endsecreflist

#### Anesthesia Machine Actions
@secreflist
  @refitem AnesthesiaMachineConfigurationTable "Anesthesia Machine Configuration"
  @refitem AnesthesiaMachineExpiratoryValveLeakTable "Expiratory Valve Leak"
  @refitem AnesthesiaMachineExpiratoryValveObstructionTable "Expiratory Valve Obstruction"
  @refitem AnesthesiaMachineInspiratoryValveLeakTable "Inspiratory Valve Leak"
  @refitem AnesthesiaMachineInspiratoryValveObstructionTable "Inspiratory Valve Obstruction"
  @refitem AnesthesiaMachineMaskLeakTable "Mask Leak"
  @refitem AnesthesiaMachineOxygenTankPressureLossTable "Oxygen Tank Pressure Loss"
  @refitem AnesthesiaMachineOxygenWallPortPressureLossTable "Oxygen Wall Port Pressure Loss"
  @refitem AnesthesiaMachineSodaLimeFailureTable "SodaLime Failure"
  @refitem AnesthesiaMachineTubeCuffLeakTable "Tube Cuff Leak"
  @refitem AnesthesiaMachineVaporizerFailureTable "Vaporizer Failure"
  @refitem AnesthesiaMachineVentilatorPressureLossTable "Ventilator Pressure Loss"
  @refitem AnesthesiaMachineYPieceDisconnectTable "Y-Piece Disconnect"
@endsecreflist

#### Bag Valve Mask Actions
@secreflist
  @refitem BagValveMaskConfigurationTable "Bag Valve Mask Configuration"
  @refitem BagValveMaskAutomatedTable "Bag Valve Mask Automated"
  @refitem BagValveMaskInstantaneousTable "Bag Valve Mask Instantaneous"
  @refitem BagValveMaskSqueezeTable "Bag Valve Mask Squeeze"
@endsecreflist

#### Inhaler Actions
@secreflist
  @refitem InhalerConfigurationTable "Inhaler Configuration"
@endsecreflist

#### Mechanical Ventilator Actions
@secreflist
  @refitem MechanicalVentilatorConfigurationTable "Mechanical Ventilator Configuration"
  @refitem MechanicalVentilatorHoldTable "Mechanical Ventilator Hold"
  @refitem MechanicalVentilatorLeakTable "Mechanical Ventilator Leak"
  @refitem MechanicalVentilatorContinuousPositiveAirwayPressureTable "Mechanical Ventilator CPAP Mode"
  @refitem MechanicalVentilatorPressureControlTable "Mechanical Ventilator Pressure Control Mode"
  @refitem MechanicalVentilatorVolumeControlTable "Mechanical Ventilator Volume Control Mode"
@endsecreflist

You can query the engine's action manager to get action information.
For example, this may be useful to understand the current state of a infusion.

@code
  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Get the Action Manager.
  /// Allows a user to check the state of active actions
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SEActionManager& GetActionManager() const = 0;
@endcode

@anchor PatientState
### Patient State

As the engine runs, it can change the patient settings.
If you would like to get the original, stabilized healthy patient values,
you can get the Initial Patient object via this method:
@code
  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Returns the initial simulation patient object used by the engine 
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SEPatient& GetInitialPatient() = 0;
@endcode

Once you start advancing time and processing actions, the patient state can start to change.
Depending on what actions you process, it can change slightly, such as a slight increase weight or a change in a baseline value, such as the mean arterial pressure baseline.

You can get the Current Patient object via this method:
@code
  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Returns the patient object used by the engine 
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SEPatient& GetPatient() = 0;
@endcode

### Engine Events

As the engine runs and depending on actions taken the engine can enter and exit various clinically based patient states that can affect multiple systems through various physiological feedback mechanisms. 
These dramatic changes in the engine are listed in our @ref eEventTable "Engine Events Table". 

You can access event states by getting the SEEventManager object with this method:

@code
  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Retrieves the associated event manager.
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SEEventManager& GetEventManager() const = 0;

@endcode

You can then gain access to events by either

- Polling the active events directly on the event manager
- Set up a callback method on the %SEEventManager to notify you when the engine enters and exits any event.

Events are primarily associated with the patient, but there are events associated with Equipment.

Look at the SDK HowTo-UseEngine.cpp for a full example of using engine events.

@anchor AssessmentsInterface
### Patient Assessments

Patient assessments are intended to give general patient overviews, formed at the level of a clinician's report.
The following assessments are available  :

@secreflist
  @refitem CompleteBloodCountTable "Complete Blood Count"
  @refitem ComprehensiveMetabolicPanelTable "Comprehensive Metabolic Panel"
  @refitem UrinalysisTable "Urinalysis"
@endsecreflist 

You must create and provide an assessment object to the physiology engine via this method:<br>
*Note that assessments could add extra computation  time to gather and format data for the assessment.*

@code
  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Determines the assessment type and fills the data object with current data. 
  ///
  /// Assessments can be queried at any point in the calculation and as many times are desired. 
  ///
  //--------------------------------------------------------------------------------------------------
  virtual bool GetPatientAssessment(SEPatientAssessment& assessment) = 0; 
@endcode

@anchor SystemsInterface
### Systems Data

The bodies physiology, equipment, and the environment are all systems and each system has a method to retrieve its associated class in order to access this system data.<br>

The engine supports the following systems:


|  Code Method                                                                           | CDM Table                                             |
|---                                                                                     |---                                                    |
|@code virtual const SEEnvironment* GetEnvironment() = 0; @endcode                       | @ref EnvironmentTable "Environment"                   |
| <b>Equipment</b>                                                                       |                                                       |
|@code virtual const SEAnesthesiaMachine* GetAnesthesiaMachine() = 0; @endcode           | @ref AnesthesiaMachineTable "Anesthesia Machine"      |
|@code virtual const SEBagValveMaskSystem* GetBagValveMaskSystem() = 0; @endcode         | @ref BagValveMaskTable "BagValveMask"                 |
|@code virtual const SEElectroCardioGram* GetElectroCardioGram() = 0; @endcode           | @ref ElectroCardioGramTable "ElectroCardioGram"       |
|@code virtual const SEInhaler* GetInhaler() = 0; @endcode                               | @ref InhalerTable "Inhaler"                           |
|@code virtual const SEMechanicalVentilator* GetMechanicalVentilator() = 0; @endcode     | @ref MechanicalVentilatorTable "MechanicalVentilator" |
| <b>Physiology</b>                                                         |                                                       |
|@code virtual const SEBloodChemistrySystem* GetBloodChemistrySystem() = 0; @endcode     | @ref BloodChemistrySystemTable "BloodChemistry"       |
|@code virtual const SECardiovascularSystem* GetCardiovascularSystem() = 0; @endcode     | @ref CardiovascularSystemTable "Cardiovascular"       |
|@code virtual const SEEndocrineSystem* GetEndocrineSystem() = 0; @endcode               | @ref EndocrineSystemTable "Endocrine"                 |
|@code virtual const SEEnergySystem* GetEnergySystem() = 0; @endcode                     | @ref EnergySystemTable "Energy"                       |
|@code virtual const SERenalSystem* GetRenalSystem() = 0; @endcode                       | @ref RenalSystemTable "Renal"                         |
|@code virtual const SEGastrointestinalSystem* GetGastrointestinalSystem() = 0; @endcode | @ref GastrointestinalSystemTable "Gastrointestinal"   |
|@code virtual const SENervousSystem* GetNervousSystem() = 0; @endcode                   | @ref NervousSystemTable "Nervous"                     |
|@code virtual const SERespiratorySystem* GetRespiratorySystem() = 0; @endcode           | @ref RespiratorySystemTable "Respiratory"             |
|@code virtual const SEDrugSystem* GetDrugSystem() = 0; @endcode                         | @ref DrugSystemTable "Drug"                           |
|@code virtual const SETissueSystem* GetTissueSystem() = 0; @endcode                     | @ref TissueSystemTable "Tissue"                       |

@anchor CompartmentsInterface
### Compartments

A compartment represents the fluid dynamics of an anatomical organ or equipment component. 
Compartments can represent various fidelities of data for these components, such as:
  - An anatomical space, such as the body's skin, muscles
  - An organ, such as the liver
  - An organ substructure, such as the Left Heart
  - Extravascular tissue of an organ
  - A component of a piece of equipment, such as an anesthesia machine ventilator
  
The following compartment types are used to represent various anatomical structures for both physiology and equipment:

@secreflist
 @refitem GasCompartmentTable "Gas Compartment"
 @refitem LiquidCompartmentTable "Liquid Compartment"
 @refitem ThermalCompartmentTable "Thermal Compartment"
 @refitem TissueCompartmentTable "Tissue Compartment"
@endsecreflist 

Various compartments  can be used to represent a parent/child hierarchical  structure, and compartments can be a parent for one or more child compartments.
Parent compartments aggregate  data from their children, such as the parent compartment volume is simply the sum of each of its childrens' volume.<br>
For example, the heart has the following hierarchy :

  - Heart
    - Myocardium
    - Right Heart
    - Left Heart
    - Pericardium

Compartments also contain a substance quatity for each substance in the compartment as it moves through the body/equipment. <br>
The following types are used to hold compartment substance information

@secreflist
 @refitem GasSubstanceQuantityTable "Gas Substance Quantity"
 @refitem LiquidSubstanceQuantityTable "Liquid Substance Quantity"
@endsecreflist

The enumerations for compartments available is found in the PulsePhysiologyEngine.h file.
As these are programmatic enumerations, you can use the auto-complete feature of your favorite programming IDE to view these enumerations as you code.
The engine discretizes it's compartments into enumerations based on fluid type and equipment. Here is a list of the various enumerated compartment names:

@secreflist
 @refitem LiquidCompartmentTable  "Chyme"
 @refitem GasCompartmentTable     "Pulmonary"
 @refitem LiquidCompartmentTable  "Pulmonary(Aerosols)"
 @refitem TissueCompartmentTable  "Tissue"
 @refitem LiquidCompartmentTable  "Extracellular"
 @refitem LiquidCompartmentTable  "Intracellular"
 @refitem LiquidCompartmentTable  "Vascular"
 @refitem LiquidCompartmentTable  "Urine"
 @refitem LiquidCompartmentTable  "Lymph"
 @refitem ThermalCompartmentTable "Temperature"
 @refitem GasCompartmentTable     "Environment"
 @refitem GasCompartmentTable     "Anesthesia Machine"
 @refitem GasCompartmentTable     "Inhaler"
@endsecreflist

As we make changes to models, some of the naming/hierarchy of compartments used could change, it's best to refer to the code file itself 
for the latest list of available compartments and their hierarchy.

Note that there are both liquid and gas compartments available for pulmonary spaces. 
The gas compartments represent the air flow and gaseous substances through the pulmonary tract.
The liquid compartments represent the air flow and aerosolized liquid/solid substances through the pulmonary tract.

All compartments are accessed via the SECompartmentManager retrieved from this method:

@code
  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Retrieves the engine compartments, providing such data as:
  /// flows, pressure, volume as well as substance volumes and volume fractions.
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SECompartmentManager& GetCompartments() = 0;
@endcode

@anchor SubstancesInterface
#### Substances

Various Substances are available in the body and there is a %SESubstanceManager associated with the engine. <br>
You can retrieve the SESubstanceManager with this method:

@code
  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Retrieves the associated substance manager.
  ///
  //--------------------------------------------------------------------------------------------------
  virtual const SESubstanceManager& GetSubstanceManager() = 0;
@endcode

In practice, only a substance reference are used with compartments to get the Substance Quantity object associated with a particular compartment.
The data contained in the @ref SubstanceTable "substance definition" is primarily used in engine methodology, 
but there may be substance data you maybe interested in, such as the @ref Substance_SubstanceTissuePharmacokineticsData "Partition Coefficient" of a substance.


