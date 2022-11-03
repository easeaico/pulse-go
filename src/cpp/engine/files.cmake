set(ENGINE_H_FILES
  engine/PulseConfiguration.h
  engine/PulseEngine.h
  engine/PulseEnginePool.h
  engine/PulseEngineThunk.h
  engine/PulseScenario.h
  engine/PulseScenarioExec.h
  # Common
  engine/CommonDefs.h
  engine/common/Engine.h
  engine/common/controller/BlackBoxManager.h
  engine/common/controller/CircuitManager.h
  engine/common/controller/CompartmentManager.h
  engine/common/controller/Controller.h
  engine/common/controller/SubstanceManager.h
  engine/common/system/Model.h
  engine/common/system/Systems.h
  engine/common/system/environment/EnvironmentModel.h
  engine/common/system/equipment/AnesthesiaMachineModel.h
  engine/common/system/equipment/BagValveMaskModel.h
  engine/common/system/equipment/ECMOModel.h
  engine/common/system/equipment/ElectroCardioGramModel.h
  engine/common/system/equipment/InhalerModel.h
  engine/common/system/equipment/MechanicalVentilatorModel.h
  engine/common/system/physiology/BloodChemistryModel.h
  engine/common/system/physiology/CardiovascularModel.h
  engine/common/system/physiology/DrugModel.h
  engine/common/system/physiology/EndocrineModel.h
  engine/common/system/physiology/EnergyModel.h
  engine/common/system/physiology/GastrointestinalModel.h
  engine/common/system/physiology/HepaticModel.h
  engine/common/system/physiology/NervousModel.h
  engine/common/system/physiology/RenalModel.h
  engine/common/system/physiology/RespiratoryModel.h
  engine/common/system/physiology/Saturation.h
  engine/common/system/physiology/TissueModel.h
  # Human Adult
  #  Whole Body
  engine/human_adult/whole_body/Engine.h
  engine/human_adult/whole_body/controller/BlackBoxManager.h
  engine/human_adult/whole_body/controller/CircuitManager.h
  engine/human_adult/whole_body/controller/CompartmentManager.h
  engine/human_adult/whole_body/controller/Controller.h
  engine/human_adult/whole_body/controller/SubstanceManager.h
  engine/human_adult/whole_body/system/environment/EnvironmentModel.h
  engine/human_adult/whole_body/system/equipment/AnesthesiaMachineModel.h
  engine/human_adult/whole_body/system/equipment/BagValveMaskModel.h
  engine/human_adult/whole_body/system/equipment/ECMOModel.h
  engine/human_adult/whole_body/system/equipment/ElectroCardioGramModel.h
  engine/human_adult/whole_body/system/equipment/InhalerModel.h
  engine/human_adult/whole_body/system/equipment/MechanicalVentilatorModel.h
  engine/human_adult/whole_body/system/physiology/BloodChemistryModel.h
  engine/human_adult/whole_body/system/physiology/CardiovascularModel.h
  engine/human_adult/whole_body/system/physiology/DrugModel.h
  engine/human_adult/whole_body/system/physiology/EndocrineModel.h
  engine/human_adult/whole_body/system/physiology/EnergyModel.h
  engine/human_adult/whole_body/system/physiology/GastrointestinalModel.h
  engine/human_adult/whole_body/system/physiology/HepaticModel.h
  engine/human_adult/whole_body/system/physiology/NervousModel.h
  engine/human_adult/whole_body/system/physiology/RenalModel.h
  engine/human_adult/whole_body/system/physiology/RespiratoryModel.h
  engine/human_adult/whole_body/system/physiology/Saturation.h
  engine/human_adult/whole_body/system/physiology/TissueModel.h
  #  Hemodynamics
  engine/human_adult/hemodynamics/Engine.h
  engine/human_adult/hemodynamics/controller/CompartmentManager.h
  engine/human_adult/hemodynamics/controller/Controller.h
  #  Ventilation Mechanics
  engine/human_adult/ventilation_mechanics/Engine.h
  engine/human_adult/ventilation_mechanics/controller/CompartmentManager.h
  engine/human_adult/ventilation_mechanics/controller/Controller.h
  # Serialization I/O
  engine/io/protobuf/PBConfiguration.h
  engine/io/protobuf/PBEnvironment.h
  engine/io/protobuf/PBEquipment.h
  engine/io/protobuf/PBPhysiology.h
  engine/io/protobuf/PBScenario.h
  engine/io/protobuf/PBState.h
)

set(ENGINE_CPP_FILES
  engine/PulseConfiguration.cpp
  engine/PulseEnginePool.cpp
  engine/PulseEngineThunk.cpp
  engine/PulseScenario.cpp
  engine/PulseScenarioExec.cpp
  # Common
  engine/common/Engine.cpp
  engine/common/controller/BlackBoxManager.cpp
  engine/common/controller/CircuitManager.cpp
  engine/common/controller/CompartmentManager.cpp
  engine/common/controller/Controller.cpp
  engine/common/controller/SetupCircuitsAndCompartments.cpp
  engine/common/controller/SetupExpandedCardiovascular.cpp
  engine/common/controller/SetupExpandedRespiratory.cpp
  engine/common/controller/SubstanceManager.cpp
  engine/common/system/environment/EnvironmentModel.cpp
  engine/common/system/equipment/AnesthesiaMachineModel.cpp
  engine/common/system/equipment/BagValveMaskModel.cpp
  engine/common/system/equipment/ECMOModel.cpp
  engine/common/system/equipment/ElectroCardioGramModel.cpp
  engine/common/system/equipment/InhalerModel.cpp
  engine/common/system/equipment/MechanicalVentilatorModel.cpp
  engine/common/system/physiology/BloodChemistryModel.cpp
  engine/common/system/physiology/CardiovascularModel.cpp
  engine/common/system/physiology/DrugModel.cpp
  engine/common/system/physiology/EndocrineModel.cpp
  engine/common/system/physiology/EnergyModel.cpp
  engine/common/system/physiology/GastrointestinalModel.cpp
  engine/common/system/physiology/HepaticModel.cpp
  engine/common/system/physiology/NervousModel.cpp
  engine/common/system/physiology/RenalModel.cpp
  engine/common/system/physiology/RespiratoryModel.cpp
  engine/common/system/physiology/Saturation.cpp
  engine/common/system/physiology/TissueModel.cpp
  # Human Adult
  #  Whole Body
  engine/human_adult/whole_body/Engine.cpp
  engine/human_adult/whole_body/controller/Controller.cpp
  engine/human_adult/whole_body/controller/SetupPatient.cpp
  #  Hemodynamics
  engine/human_adult/hemodynamics/Engine.cpp
  engine/human_adult/hemodynamics/controller/CompartmentManager.cpp
  engine/human_adult/hemodynamics/controller/Controller.cpp
  engine/human_adult/hemodynamics/controller/SetupCircuitsAndCompartments.cpp
  #  Ventilation Mechanics
  engine/human_adult/ventilation_mechanics/Engine.cpp
  engine/human_adult/ventilation_mechanics/controller/CompartmentManager.cpp
  engine/human_adult/ventilation_mechanics/controller/Controller.cpp
  engine/human_adult/ventilation_mechanics/controller/SetupCircuitsAndCompartments.cpp
  # Serialization I/O
  engine/io/protobuf/PBConfiguration.cpp
  engine/io/protobuf/PBEnums.cpp
  engine/io/protobuf/PBEnvironment.cpp
  engine/io/protobuf/PBEquipment.cpp
  engine/io/protobuf/PBPhysiology.cpp
  engine/io/protobuf/PBScenario.cpp
  engine/io/protobuf/PBState.cpp
)

set(ENGINE_H_FILES ${BINDING_H_FILES} PARENT_SCOPE)
set(ENGINE_CPP_FILES ${BINDING_CPP_FILES} PARENT_SCOPE)

set(ENGINE_TEST_H_FILES
  # Human Adult
  #  Whole Body
  engine/human_adult/whole_body/test/EngineTest.h
  #  Ventilation Mechanics
  engine/human_adult/ventilation_mechanics/test/EngineTest.h
)
set(ENGINE_TEST_CPP_FILES
  # Human Adult
  #  Whole Body
  engine/human_adult/whole_body/test/AcidBaseTests.cpp
  engine/human_adult/whole_body/test/AerosolTest.cpp
  engine/human_adult/whole_body/test/AnesthesiaCircuit.cpp
  engine/human_adult/whole_body/test/BlackBoxTests.cpp
  engine/human_adult/whole_body/test/CardiovascularCircuit.cpp
  engine/human_adult/whole_body/test/ConditionsTest.cpp
  engine/human_adult/whole_body/test/EngineTest.cpp
  engine/human_adult/whole_body/test/EngineTest.h
  engine/human_adult/whole_body/test/EnginePoolTests.cpp
  engine/human_adult/whole_body/test/FourCompartmentTest.cpp
  engine/human_adult/whole_body/test/MechanicalVentilatorCircuit.cpp
  engine/human_adult/whole_body/test/NervousTests.cpp
  engine/human_adult/whole_body/test/ReadScenarios.cpp
  engine/human_adult/whole_body/test/RenalCircuit.cpp
  engine/human_adult/whole_body/test/RespiratoryCircuit.cpp
  engine/human_adult/whole_body/test/ReuseEngine.cpp
  engine/human_adult/whole_body/test/SerializationTest.cpp
  engine/human_adult/whole_body/test/SetupPatientTest.cpp
  engine/human_adult/whole_body/test/SolverTests.cpp
  engine/human_adult/whole_body/test/TemperatureCircuit.cpp
  engine/human_adult/whole_body/test/TissueTests.cpp
  #  Ventilation Mechanics
  engine/human_adult/ventilation_mechanics/test/EngineTest.cpp
)

set(ENGINE_TEST_H_FILES ${ENGINE_TEST_H_FILES} PARENT_SCOPE)
set(ENGINE_TEST_CPP_FILES ${ENGINE_TEST_CPP_FILES} PARENT_SCOPE)

