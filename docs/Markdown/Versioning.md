Version 4.3.0 {#version}
=============

Our versioning follows the <a href="http://semver.org">Semantic Versioning 2.0.0</a> format.

Our version number sematic is Major.Minor.Patch-ReleaseStage, where :
- MAJOR changes when we make incompatible API changes,
- MINOR changes when we add functionality in a backwards-compatible manner, and
- PATCH changes when we make backwards-compatible bug fixes.
- Release Stage - We have extended this versioning with a <a href="http://en.wikipedia.org/wiki/Software_release_life_cycle">release stage</a>

---

## Pulse Integration (Current new feature set)

- Software Architecture Improvements
  - Automated Validation Updates
    - Improved organization for automated validation modules
    - Fix the need for duplicate data request files
    - Support multiple md files for a validation xlsx
    - Change argument from xlsx filename to the directory
    - Changed enum based comparisons to a formula based comparison
    - Custom validation bounds per property (default Good=10%, Fair=30%)
    - Support referencing other sheet segments (within the same workbook) as an expected value
    - Support defining patients in validation workbooks
    - Support configuration json in validation workbooks
    - Include comparison formula in expected value column of segment validation tables
    - Shifting default to 3 significant digits for values in validation tables
  - Improved Batch Study Support
    - Created Python based HowTo for running/analyzing large amounts of patients and insult/intervention combinations

- Physiology Model Improvements
    - Moved hemorrrhage model validation to an automated validation module


## Pulse v4.3.1 (January 2025)

- ThirdParty
  - Update to Protobuf 29.2
    - Fixes the Microsoft SDK infinity issue

- CDM API Changes
  - Ventilator API Updates
    - Remove triggers with zero values
    - Address issues when combining mode and configuration actions
    - Ignore invalid mode actions and log as error, rather than log a fatal error
    - Added a mechanical ventilator settings object to mode actions
  - Fix issues found in the C# API
    - Anesthesia Machine was not full implemented
    - ARDS/COPD/Pneumonia actions/conditions were not using dicts properly

- Software Architecture Improvements
  - Link to static C++ runtime when building for Android
  - Fix logging in Java API
  - Correct 4.3.0 release bug that had MSVC configuration with optimizations turned off :(
  - Improved configurability of dockcross builds (i.e. building cross platform PulseJNI binaries)

- Physiology Model Improvements
  - Improve severe airway obstruction validation
  - Improve blood gas regulation during assisted mechanical ventilation
      - Updated the interactions between the respiratory mechanoreceptor and chemoreceptor models

---

## Pulse v4.3.0 (September 2024)

- CDM API Changes
  - Blood Chemistry System Data
    - Added PlasmaOsmolality and PlasmaOsmolarity
  - Respiratory System Data
    - Added ExtrinsicPositiveEndExpiratoryPressure
    - Changed InspiratoryPulmonaryResistance to InspiratoryRespiratoryResistance
    - Changed IntrinsicPositiveEndExpiredPressure to IntrinsicPositiveEndExpiratoryPressure
    - Changed ExpiratoryPulmonaryResistance to ExpiratoryRespiratoryResistance
    - Changed PulmonaryCompliance to RespiratoryCompliance
    - Changed PulmonaryElastance to RespiratoryElastance
    - Changed PositiveEndExpiratoryPressure to TotalPositiveEndExpiratoryPressure
  - Environmental Conditions Data
    - Added MechanicalDeadSpace
  - Anesthesia Machine Data
    - Changed PositiveEndExpiredPressure to PositiveEndExpiratoryPressure
  - Bag Valve Mask Data
    - Changed ValvePositiveEndExpiredPressure to ValvePositiveEndExpiratoryPressure
  - Mechanical Ventilator Data
    - Added PeakInspiratoryFlow
    - Added ExtrinsicPositiveEndExpiratoryPressure
    - Changed PositiveEndExpiratoryPressure to TotalPositiveEndExpiratoryPressure
    - Changed DynamicPulmonaryCompliance to DynamicRespiratoryCompliance
    - Changed IntrinsicPositiveEndExpiredPressure to IntrinsicPositiveEndExpiratoryPressure
    - Changed StaticPulmonaryCompliance to StaticRespiratoryCompliance
  - Mechanical Ventilator Settings Data
    - Added ExpiratoryResistance and InspiratoryResistance
    - Changed PositiveEndExpiredPressure to PositiveEndExpiratoryPressure
  - Mechanical Ventilator Continuous Positive Airway Pressure Data
    - Changed PositiveEndExpiredPressure to PositiveEndExpiratoryPressure
  -  Mechanical Ventilator Pressure Control Data
    - Changed PositiveEndExpiredPressure to PositiveEndExpiratoryPressure
  - Mechanical Ventilator Volume Control Data
    - Changed PositiveEndExpiredPressure to PositiveEndExpiratoryPressure

- Software Architecture Improvements
  - Data Set Generation Tools
    - Initial patient set generator
      - Can create large patient sets using permutations of starting HR, RR, MAP, Age, Height, BMI, BFF and other patient properties
    - Injury set generator
      - For each patient in a patient set, will apply and run a permutation of inujuries such as (but not limited to) Hemorrhage, AirwayObstruction, Tension Pneumothorax
      - Convience modes provided to generate a set of preprogrammed TCCC injuries 
  - Combined DataModelBindings, CommonDataModel and PulseEngine into a single Pulse library
    - This single library can be built as a shared/dynamic library or a static (default) library
  - Implement system/patient validation framework in Python
    - Provides access to more discrete modules in our validation pipeline for more versatile use
  - Initial architecture for automated validation
    - Currently only AirwayObstruction has been migrated
    - Created a xlsx template for automating the our validation of our models (is currently being done by hand)
    - The validation pipeline can also create a single doxygen report with a plot file and md files associated with an xlsx
    - Added the ability to create vitals monitor and ventilator monitor plots from CSV files
  - Created an Advance To Stable Action, this will run the engine until stable criteria is met
  - Threaded execution of a group of scenarios now can be provided a single json file containing all scenario to run
    - Information for every executed scenario is then updated in the provided file:
      - The locations of the: scenario file and its generated log file, and csv file
      - Various error states if encountered: Unable to initialize and why, if there were any other errors encountered
  - Post processing pipeline
    - Written in python, allows us to process a log and csv file generated from a scenario and pull out (and even generate new) data into a more machine learning friendly format
      - Unstructure Text Module looks at map of vitals to a string vector and randomly takes strings based on the vitals values to create an unstructured description of the simulation at that time. The strings and bounds are read in from a spreadsheet
  - Testing Utils
    - Add a run.cmake option to generate a config of all the failures for a quick and easy rebase (once they have been reviewed and approved of course)
    - Remove action vertical lines in our verification plots of actions that occur many many times. These actions are usually testing sensor driven inputs and make data interpretation difficult.
  - Fixed improperly mapped events

- Physiology Model Improvements
  - Updated dyspnea implementation
    - Split single severity into a Respiration Rate severity and Tidal Volume severity
    - This allows users to define breathing impairments with more precision
    - **Note** any previous scenarios using Dyspnea severity should apply that value to the Tidal Volume severity
  - Mechanical Ventilator Model
    - Ventilation will immediately stop at limits for more precise targets
    - Apneic patients will no long trigger the ventilator when the model trigger is selected
    - Update naming conventions and parameter calculations to be match more widely accepted definitions
  - Respiratory Model
    - Improved handling of lung recruitment based on acinar ventilation for showing the pulmonary shunt changes due to increased ventilator PEEP
    - Calibrated respiratory diseases for mechanically ventilated patients, including ARDS and COPD
    - Added mechanoreceptor feedback that inhibits the inspiratory drive and reduces the respiratory muscle pressure during an assisted breath
  - Modifier Actions (SECardiovascularMechanicsModification, SERespiratoryMechanicsModification)
    - We now provide 2 new actions to modify the respiratory and cardiovascular model parameters
    - For example, you can provide a multipliers to modify the heart rate, respiration rate, systemic vascular and pulmonary resistances
    - This provides the end user more low level control to fine tune the physiology to their specific needs
  - The application of drug pharmacodynamics changes to the cardiovascular system now include pulse pressure as well as mean arterial pressure
    - Acute stress is more inline with validation data
  - Updated body fluid and perspiration methodology
    - Added sweat substance loss using a substance compound definition
    - Updated the sweat rate methodology to better meet validation
    - Calibrated the convective/evaporative heat loss due to sweating
    - Fixed a bug for substance tissue diffusion between the vascular and extracellular spaces
    - Added new system data outputs for total body fluid volume, plasma osmolality, and plasma osmolarity
  - Added a dehydration condition model that directly affects the Tissue, Cardiovascular and Energy systems
    - Associated new events: Hypernatremia Hyponatremia, MildDehydration, ModerateDehydration, SevereDehydration
  - Added a mechanical dead space parameter to the Environment
  - Updated the urinalysis assessment with more appropriate substance thresholds
  - Added logic for several missing events: hyperglycemia, hypoglycemia, ketoacidosis, and lactic acidosis

---

## Pulse v4.2.0 (October 2023)

- Software Architecture Improvements
  - Addressed many compiler warnings
  - iOS build support
  - New utility to translate a log file into a scenario
    - Logs now contain json for patient, actions and conditions
  - More serialization testing
  - Reusable data request files
    - Scenarios can now reference external files for data requests
  - Added support for expanding an engine's systems and/or circuits
    - Provides the ability to define and run a more complex circuit within a common system model
  - Improved BlackBox support for external model coupling
  - We are slowly migrating our Java based documentation and test suite to Python
    - Python and Java are **Now Required** to run the Pulse test suite
    - Added Python plotting tools for improved verification, validation, and documentation
  
- Physiology Model Improvements
  - Patient variability
    - Added system validation for standard female patients
    - Added optional pulse pressure, mean arterial pressure, and body mass index settings
  - Mechanical ventilator
    - Improved triggering
    - Improved error handling
    - Additional waveform types
    - Expose more control parameters
    - More validation
  - Blood Chemistry
    - Added Base Excess
  - Respiratory
    - Corrected respiratory inhale/exhale transition timing
    - Added hemothorax model 
      - Includes cardiovascular effects
      - Includes tube thoracostomy model for relief  
    - Added shunting and labored breathing effects to collapsing lung functionality
    - Improved lung collapse (e.g., tension pneumothorax and hemothorax) responses
    - Updated restrictive and obstructive conditions/actions with severity mapping to compartments
    - Updated airway obstruction, bronchoconstriction, and asthma resistance severity mapping to better match expected outcomes
  - Intubation
    - Added a severity to control flow
    - Esophageal intubation can provide air to lungs via a severity
  - Bag-Valve-Mask
    - Automation Support - Set a frequency and pressure to repeat
    - Single Squeeze - Set a pressure for a single squeeze
    - Instantaneous - Set a pressure to apply (Intended for hardware integration)
  - Cardiovascular
    - Improved heart driver and cardiac cycle
    - More control to adjust vascular tone
    - Pulmonary capillary coverage adjusts based on Mean Arterial Pressure
  - Cardiac Arrest
    - Improved End Tidal Carbon Dioxide Pressure response
    - Improved SpO2 response
    - Improved recovery transition model
  - Hemorrhage
    - Improved Baroreceptor response
    - Severity now calculates resistance using compartment inflow average
  - CPR
    - Automation Support - Set a frequency and severity or depth to repeat
    - Single Squeeze - Set a severity or depth for a single squeeze
    - Instantaneous - Set a severity or depth to apply (Intended for hardware integration)
  - ECMO support
    - End user can adjust substance values and flow rates back into Pulse
    - *NOT A MODEL* Intended for external users to prescribe flow rates and substance concentrations
  - Removed Pulmonary Function Test Assessment


---

## Pulse v4.1.0 (March 2022)

- Software Architecture Improvements
  - Including build information
    - Added to all client facing API's (C++, Java, Python, C#) for external applications to get Pulse version
    - Added to state files to help identify compatibility with engine builds
  - New Hemodynamics Engine
    - Provides the ability to execute and optimize our cardiovascular circuit
    - Compares optimization iterations to validation data to converve on an optimal and valid circuit configuration
  - Updated to Eigen 3.4.0 and Protobuf 3.18.0
- Physiology Model Improvements
  - Added Lorazepam 
  - Added an oversedation drug as a test case
  - New Advanced Cardiac Life Support (ACLS) Arrhythimias with ECG waveforms
    - NormalSinus : Normal electrical signal and mechanical function
    - Sinus Bradycardia : Normal electrical signal with reduced heart rate
    - Sinus Tachycardia : Normal electrical signal and increased heart rate
    - Sinus Pulseless Electrical Activity : Organized electrical signal, but no mechanical function
    - Asystole : No electrical signal or mechanical function
    - Coarse Ventricular Fibrillation : Unorganized electrical signal, no mechanical function
    - Fine Ventricular Fibrillation : Unorganized electrical signal, reduced amplitude compared to coarse VF, no mechanical function
    - Pulseless Ventricular Tachycardia : Ventricular tachycardia electrical signal with reduced amplitude, but no mechanical function
    - Stable Ventricular Tachycardia : Ventricular tachycardia electrical signal with increased heart rate
    - Unstable Ventricular Tachycardia : Ventricular tachycardia electrical signal with increased heart rate, reduced heart elastance
  - ECG Waveforms use an interpolation algorithm to fit the waveform to the cardiac cycle
  - Ventilation equipment provides breath information to differentiate patient or equipment initiated breaths
  - Added configurable relief valve to mechanical ventilator
  - Added a suite of black box unit tests to verify data exchange combinations
  - Bug Fixes
    - Corrected flow calculations for compartments
    - Increased the frequency data is updated in the cardiovascular system during cardiac arrest
    - Replaced Irreversible states with more relavent clinical events: Pulse continues running, unlike during Irreversible states
    - Improved calculations of patient lung volumes due to disease


---

## Pulse v4.0.0 (September 2021)

- Software Architecture Improvements
  - Multiple Engine Support
    - Provides the ability to break up, extend, and group various model implementations into a specific physiology engine
    - A foundation for future whole body physiology engines, such as a pediatric engine
    - All of our supported languages support the ability to specify the back end engine model at construction time
    - All engines are driven using the same API
  - New Ventilation Mechanics Engine
    - Encapsulates only our respiratory and mechanical ventilation models
    - *NOT* a whole body physiology engine, only the math models for respiration and ventilation
  - Upgrade code base to C++17
  - Added namespaces
  - Improved header organization
  - Normalized API's between all supported languages
  - Moved autogenerated protobuf files to build directory
- Physiology Model Improvements
  - New Arterial Blood Gasses Assessment
  - Added Etomidate and Phenylephrine
  - Validated Norepinephrine and added support for a basal rate and clearance
  - New Respiratory Mechanics Action
    - Provides the ability to fully customize the respiratory compliances, resistances, and spontaneous breath timings
  - New Bag Valve Mask Equipment with support for various levels of interaction
    - Squeeze: A single squeeze profile to perform once
    - Automatic: A squeeze profile and rate to continuously occur
    - Instantaneous: Provide flow/pressure values of a squeeze (For sensor integration)
  - New %Mechanical Ventilator Actions
    - Hold: Keep the current delivered pressure/volume static until released
    - Leak: Prescribe a leak of air between the ventilator and patient 
    - CPAP Mode: Intuitive settings associated with CPAP ventilation
    - Pressure Control Mode: Intuitive settings associated with PC-CMV and PC-AC
    - Volume Control Mode: Intuitive settings associated with VC-CMV and VC-AC
  - Expanded data available in Respiratory and Mechanical Ventilator Systems


---

## Pulse v3.2.0 (May 2021)

- Software Architecture Improvements
  - Added support for Python to create and manage a pool for engines to optimize integration of Pulse with ML frameworks
  - Added support and examples for defining complex, scenario specific death state logic
  - Added support for using a dynamic time step provided by the users
  - Improved interface for scenario execution
    - Now available to run scenarios from C# and Java
    - Provide batch utilities to organize and execute scenarios that generate application specific states
  - Added serialization testing support to verification and validation suite
    - Now testing serialization of all actions and fixed many bugs related to missing action state between loading
    - Improvements for reusing engine to stabilize new patients
  - Added support C#,Java to query active actions and conditions after loading a state
  - Added support to create data requests for action data and enum properties
  - Improved patient loading logic
  - Improved error handling and logging, including adding more than one log listener
  - Cleaned compiler warnings and unused variables
  - Added Java module support
- Physiology Model Improvements
  - Improved %Respiratory system validation
    - Added sinusoid curve for muscle driver
    - Improved I:E ratio validation (including obstructive I:E ratio changes)
    - Improved interface to easily switch between various airway management equipment
    - Improved pulmonary shunting model
    - Improved dead space model
  - Improved Mechanical Ventilator equipment validation
  - Improved various actions
    - Added ability to create extremely cold environments
    - Added more data associated with hemorrhages
      - Exposed flows per hemorrhage and total flow
      - Exposed blood lost volume per hemorrhage and total blood lost


---

## Pulse v3.1.0 (December 2020)

- Added Severity/Resistance Hemorrhage model
- Black Box support for integration of external engines with Pulse circuits
- Configuration Actions Update
- Added (optional) Administration Time to Bolus Injection Action
- Update/Fix Supplemental Oxygen
- Update exception handling in Java
- Improved C++ memory management
- Improvements to C++ architecture to allow fully customized engines
- Optimizations and Improvements to Data Requests, Plotting, and Reporting
- Improvements to support more platforms:
  - Android ARMv7a
  - Android ARMv8a
  - WinAMR64 (HoloLens 2)
  - Magic Leap's Lumin OS
  - Many Linux


---

## Pulse v3.0.0 (July 2020)

- Software Architecture Improvements
  - Major refactor of our repository structure
    - Enables implementation of specific engines (adult, pediatric, animal) to both share and extend common models with target specific models
    - Provides a framework for performing and sharing code between various sensitivity and correlation analysis studies with a Pulse engine
    - Improved CMake structure to simplify integration of Pulse into custom applications
  - Streamlining of engine creation and management
    - Pulse states are self contained and no longer depend on any files on disk
    - Pulse states now support binary serialization
    - Location of required files for patient creation can be specified by users
  - Improvements to cross language support
    - Java interface has been refactored to conform with C# and Python usage
    - C# supports most actions, conditions and equipment types 
    - Python is now a fully supported language
  - Pulse now supports the ability to override and hold specific system and circuit parameters to a certain values, such as Lung Compliance
- Physiology Model Improvements
  - Added generic mechanical ventilator equipment model with PC-CMV and VC-AC mode implementation
  - Added pulmonary shunting as a standalone model and in conjunction with other respiratory diseases
  - Improved and validated ARDS
  - Added more respiratory clinical measurements with validation
  - Added hyperoxemia and hypocapnia events
  - Improved cardiovascular validation for resting physiology
  - Improved the tissue to cardiovascular interface to improve fluid movement between these regions of the body
  - Hemorrhagic shock and improved hemorrhage methodology to meet validation through different stages of hemorrhagic shock to death
  - A second order baroreceptor model to moderate the effectiveness of the baroreceptors over time

### Congratulations

Congratulations and thank you to everyone who contributed to this release. This release, we would like to specially recognize our 2 new contributors:

  - Bob Marinier of [SoarTech](https://soartech.com/) for helping improve the Java interface
  - Wenye He of [Innovision](https://innovisionllc.com/) for helping to start our Python API, and extend our C# API

### Planned Improvements

- Sepsis
- Cerebrospinal fluid model for improved intracranial pressure
- Work of breathing and respiratory fatigue models
- Circuit/Compartment black boxing to support modularity for system/model/circuit swapping
- Pediatric physiology prototype
- An official contribution guide for merge requests for methodology/model changes
- Pypi support to pip install Pulse into a python environment
- Maven build for easy inclusion into Java projects
- Integration with Hololens 2


---

## Pulse v2.3.0 (January 2020)

- Software Architecture
  - C# API Updates
    - Added Data Request support for requesting any data from the engine
    - Added support for patient creation, with or without chronic conditions
    - Added support for more actions
  - C++ API Updates
    - Created separate <i>Initial</i> and <i>Current</i> patient definitions
    - Updated general math exponential functions to be more intuitive
    - Added a <i>Multiply</i> value setting, similar to <i>Increment</i>
- Physiology Models
  - Patient lung volumes are now determined using ideal body weight rather than actual body weight
  - Significant respiratory model updates
    - Changed standard respiration rate from 16 bpm to 12 bpm to better match standards in literature and validation
    - Tweaked respiratory circuit for improved modeling
    - Refactored the respiratory muscle driver with a new waveform
    - Added a sigmoidal chest wall compliance model
    - Refactored respiratory conditions and actions for improved restrictive and obstructive disease results and combined effects
    - Added an ARDS condition
    - Added a pulmonary fibrosis condition
    - Added exacerbation action to degrade/improve respiratory conditions during simulations
    - Renamed apnea action to dyspnea    
    - Refactored conscious respiration, leading to improved spirometry curves
    - More/better validation
  - Updated anesthesia machine circuit to use pressure sources for supplying gas, rather than flow sources to avoid issues with flow source pressure calculations
  
### Congratulations

Congratulations and thank you to everyone who contributed to this release. This release, we would like to specially recognize our 3 new contributors:

  - Hongpeng Liu
  - Mattias Lantz Cronqvist
  - Anusha Muralidharan

### Planned Improvements

- Python bindings
- Black box circuit/compartment components
- Sepsis
- Hemorrhagic Shock
- Hemorrhage methodology update for better performance and usability
- Work of breathing and respiratory fatigue models
- A second order baroreceptor model
- Modularity improvements for system/model/circuit swapping
- Pediatric physiology prototype
- Official contribution plan for merge requests for methodology/model changes

@anchor known-issues
### Known Physiology Model Issues and Limitations

The following are known issues with the current version of the software:
- Lack of a full sympathetic/parasympathetic nervous system
- Extravascular fluid exchange model is incomplete
- Peripheral resistance currently does not scale with core temperature
- Only tested a simulation up to 12 hours in length (No sleep model)
- Limited Consumption model
  - Limited number of macronutrients available
  - Limited conversion and use within the engine
- Oxygen saturation drops too sharply


---

## Pulse v2.2.0 (August 2019)

- Software Architecture
  - CMake integration improvements
  - Improved Data Request API
  - Scenario Execution Improvements
  - Unified API for Engine Events
  - MSVC Build Sped up
- Physiology Models
  - Added the ability to provide supplemental oxygen through a nasal cannula, simple mask, and nonrebreather mask.
  - Updated vascular effects caused by respiratory pleural cavity imbalances, mainly to increased venous return resistance when the patient has a pneumothorax / collapsed lungs.


---

## Pulse v2.1.0 (February 2019)

- Software updates necessary for integration with Unity


---

## Pulse v2.0.0 (January 2019)

- Software Architecture
  - Converted ASCII file I/O to JSON (Compliant to Protobuf IDLs)
  - Updated Engine Interface to support Both ASCII and Binary serialization
  - Started a formal C# CDM Library for use in .NET applications
  - Created a C interface to Pulse for external application integration
  - Updated to the latest versions of Eigen/Protobufs/log4cplus
  - Unity integration support and examples
- Physiology Models
  - Internal Hemorrhage Support
  - External hemorrhage from specific compartments (only Vena Cava before)
  - Added support for Packed Red Blood Cells infusion
  - Added dissolved oxygen and carbon dioxide to IV fluids to resolve issues with fluid resuscitation


---

## Pulse v1.1.0 (June 2018)

- Updated Wiki for using Pulse in your application
- Added C# example interface
- Refactored respiratory system for improvements determining the transition between inhale and exhale
  - Important for determining correct vital signs (e.g., respiration rate, tidal volume, end tidal CO2)
  - Most noticeable improvement is with external sources, such as a ventilator
- Added aerosols (e.g., albuterol) to the mechanical ventilator
- Added the ability to remove the cardiac arrest action and transition from asystole to normal sinus
- Added norepinephrine to the drug library
- Switched the included logger from log4cpp to log4cplus
- Updated to the latest versions of Eigen and Protobufs
- Transitioned results file extension from .txt to .csv
- Exposed action and condition information via the API
- Improved exception handling
- Discretized verification data sets for easier management
- Cleaned up headers
- CMake build improvements
  - Pulse builds and runs on single board pc's and all major operating systems


---

## Pulse v1.0.0 (September 2017)

- Fixed multi-platform compiling bugs
- Moved from an in-source to out-of-source build
  - src tree is treated as read only
  - See a description <a href="https://gitlab.kitware.com/cmake/community/-/wikis/FAQ#out-of-source-build-trees">here</a>
- Full CMake Builds
  - Replaced all ant and scripts (.sh and .bat) with CMake
    - Improves build support across all target platforms
      - Currently supporting Windows, Mac, UNIX (including AArch64) devices
    - More multi-platform/compiler compliance
      - Currently supporting MSVC (2015+), GCC (4.8.1+), and Clang (3.3+)
      - Supports the Ninja build system 
  - Created a superbuild
    - Build scripts will download and build all dependent 3rd party libraries - removes the libs from source pool
    - Turnkey build process
- Converted reporting from emailing to write html reports to the test directory
  - Removes SMTP server requirement
- Moved the verification data sets (very large) from source repository to a data server integrated with CMake
- Updates to ensure no 3rd party software license compliance issues for certain commercial applications
- Conversion of the data model from XML to Google Protocol Buffers
