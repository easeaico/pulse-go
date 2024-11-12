/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/human_adult/whole_body/test/EngineTest.h"

namespace pulse { namespace human_adult_whole_body
{
  EngineTest::EngineTest(Logger* logger) : SETestManager(logger)
  {
    FillFunctionMap();
  }

  EngineTest::~EngineTest()
  {
  }

  bool EngineTest::RunTest(const std::string& testName, const std::string& sOutputDirectory)
  {
    try
    {
      //If you find the test name, run it
      if (testMap.find(testName) != testMap.end())
      {
        testFunction func = testMap.at(testName);
        (this->*func)(sOutputDirectory);
        return true;
      }
      else
      {
        //Try to find the function with "Test" appended; run it if found
        if (testMap.find(testName + "Test") != testMap.end())
        {
          testFunction func = testMap.at(testName + "Test");
          (this->*func)(sOutputDirectory);
          return true;
        }

        m_ss << "Could not find " << testName << " or " << testName << "Test in EngineTest." << std::endl;
        Error(m_ss);
        return false;
      }
    }
    catch (CommonDataModelException& ex)
    {
      Error(ex.what());
    }
    catch (std::exception& ex)
    {
      Error(ex.what());
    }
    catch (...)
    {
      Error("Exception!!");
    }
    return false;
  }

  void EngineTest::FillFunctionMap()
  {
    testMap.insert(std::make_pair("ReadScenarios", &EngineTest::ReadScenarios));
    testMap.insert(std::make_pair("ReuseEngine", &EngineTest::ReuseEngine));
    testMap.insert(std::make_pair("SerializationTest", &EngineTest::SerializationTest));
    testMap.insert(std::make_pair("SetupPatientTest", &EngineTest::SetupPatientTest));

    //Fill a map that ties unit test names to their actual functions
    testMap.insert(std::make_pair("CardiovascularCircuitAndTransportTest", (testFunction) &EngineTest::CardiovascularCircuitAndTransportTest));
    testMap.insert(std::make_pair("CardiovascularAndRenalCircuitAndTransportTest", &EngineTest::CardiovascularAndRenalCircuitAndTransportTest));
    testMap.insert(std::make_pair("CardiovascularAndTissueCircuitAndTransportTest", &EngineTest::CardiovascularAndTissueCircuitAndTransportTest));
    testMap.insert(std::make_pair("CardiovascularAndCerebrospinalFluidCircuitAndTransportTest", &EngineTest::CardiovascularAndCerebrospinalFluidCircuitAndTransportTest));
    testMap.insert(std::make_pair("CardiovascularAndExpandedLungsCircuitAndTransportTest", &EngineTest::CardiovascularAndExpandedLungsCircuitAndTransportTest));
    testMap.insert(std::make_pair("FullCardiovascularCircuitAndTransportTest", &EngineTest::FullCardiovascularCircuitAndTransportTest));
    testMap.insert(std::make_pair("FullExpandedLungsCardiovascularCircuitAndTransportTest", &EngineTest::FullExpandedLungsCardiovascularCircuitAndTransportTest));
    testMap.insert(std::make_pair("CardiovascularBloodGasesTest", &EngineTest::CardiovascularBloodGasesTest));
    testMap.insert(std::make_pair("CardiovascularExpandedLungsBloodGasesTest", &EngineTest::CardiovascularExpandedLungsBloodGasesTest));
    testMap.insert(std::make_pair("TuneCardiovascularCircuitTest", (testFunction) & EngineTest::TuneCardiovascularCircuitTest));
    testMap.insert(std::make_pair("CardiovascularCircuitScaleTests", &EngineTest::CardiovascularCircuitScaleTests));

    testMap.insert(std::make_pair("RenalCircuitAndTransportTest", &EngineTest::RenalCircuitAndTransportTest));
    testMap.insert(std::make_pair("RenalTGFFeedbackTest", &EngineTest::RenalTGFFeedbackTest));
    testMap.insert(std::make_pair("RenalTGFandUPRFeedbackTest", &EngineTest::RenalTGFandUPRFeedbackTest));
    testMap.insert(std::make_pair("RenalSecretionTest", &EngineTest::RenalSecretionTest));
    testMap.insert(std::make_pair("RenalUrinateTest", &EngineTest::RenalUrinateTest));

    testMap.insert(std::make_pair("RespiratoryCircuitAndTransportTest", (testFunction) & EngineTest::RespiratoryCircuitAndTransportTest));
    testMap.insert(std::make_pair("RespiratoryExpandedLungsCircuitAndTransportTest", &EngineTest::RespiratoryExpandedLungsCircuitAndTransportTest));
    testMap.insert(std::make_pair("RespiratoryDriverTest", &EngineTest::RespiratoryDriverTest));

    testMap.insert(std::make_pair("AnesthesiaMachineCircuitAndTransportTest", (testFunction) & EngineTest::AnesthesiaMachineCircuitAndTransportTest));
    testMap.insert(std::make_pair("RespiratoryWithAnesthesiaMachineCircuitAndTransportTest", &EngineTest::RespiratoryWithAnesthesiaMachineCircuitAndTransportTest));
    testMap.insert(std::make_pair("RespiratoryExpandedLungsWithAnesthesiaMachineCircuitAndTransportTest", &EngineTest::RespiratoryExpandedLungsWithAnesthesiaMachineCircuitAndTransportTest));

    testMap.insert(std::make_pair("MechanicalVentilatorCircuitAndTransportTest", (testFunction) & EngineTest::MechanicalVentilatorCircuitAndTransportTest));
    testMap.insert(std::make_pair("RespiratoryWithMechanicalVentilatorCircuitAndTransportTest", &EngineTest::RespiratoryWithMechanicalVentilatorCircuitAndTransportTest));
    testMap.insert(std::make_pair("RespiratoryExpandedLungsWithMechanicalVentilatorCircuitAndTransportTest", &EngineTest::RespiratoryExpandedLungsWithMechanicalVentilatorCircuitAndTransportTest));

    testMap.insert(std::make_pair("RespiratoryWithInhalerCircuitAndTransportTest", &EngineTest::RespiratoryWithInhalerCircuitAndTransportTest));
    testMap.insert(std::make_pair("RespiratoryExpandedLungsWithInhalerCircuitAndTransportTest", &EngineTest::RespiratoryExpandedLungsWithInhalerCircuitAndTransportTest));

    testMap.insert(std::make_pair("RespiratoryWithMechanicalVentilationCircuitAndTransportTest", &EngineTest::RespiratoryWithMechanicalVentilationCircuitAndTransportTest));
    testMap.insert(std::make_pair("RespiratoryExpandedLungsWithMechanicalVentilationCircuitAndTransportTest", &EngineTest::RespiratoryExpandedLungsWithMechanicalVentilationCircuitAndTransportTest));

    testMap.insert(std::make_pair("InternalTemperatureVariableBMRCircuitTest", &EngineTest::InternalTemperatureVariableBMRCircuitTest));
    testMap.insert(std::make_pair("InternalTemperatureVariableSkinCircuitTest", &EngineTest::InternalTemperatureVariableSkinCircuitTest));
    testMap.insert(std::make_pair("InternalTemperatureVariableCoreCircuitTest", &EngineTest::InternalTemperatureVariableCoreCircuitTest));
    testMap.insert(std::make_pair("EnvironmentVariableTemperatureCircuitTest", &EngineTest::EnvironmentVariableTemperatureCircuitTest));
    testMap.insert(std::make_pair("CombinedInternalAndEnvironmentVariableBMRandTemperatureCircuitTest", &EngineTest::CombinedInternalAndEnvironmentVariableBMRandTemperatureCircuitTest));
    testMap.insert(std::make_pair("CombinedInternalAndEnvironmentSkinTempDropCircuitTest", &EngineTest::CombinedInternalAndEnvironmentSkinTempDropCircuitTest));
    testMap.insert(std::make_pair("EnvironmentISO7730ComparisonTest", &EngineTest::EnvironmentISO7730ComparisonTest));

    testMap.insert(std::make_pair("DiffusionClearanceExcretionTests", &EngineTest::DiffusionClearanceExcretionTests));
    testMap.insert(std::make_pair("AlveolarCarbonDioxideDiffusionTest", &EngineTest::AlveolarCarbonDioxideDiffusionTest));
    testMap.insert(std::make_pair("AlveolarOxygenDiffusionTest", &EngineTest::AlveolarOxygenDiffusionTest));
    testMap.insert(std::make_pair("InstantPlusSimpleDiffusionTest", &EngineTest::InstantPlusSimpleDiffusionTest));
    testMap.insert(std::make_pair("SimpleDiffusionTwoCompartmentTest", &EngineTest::SimpleDiffusionTwoCompartmentTest));
    testMap.insert(std::make_pair("SimpleDiffusionFourCompartmentTest", &EngineTest::SimpleDiffusionFourCompartmentTest));
    testMap.insert(std::make_pair("SimpleDiffusionHierarchyTest", &EngineTest::SimpleDiffusionHierarchyTest));
    testMap.insert(std::make_pair("FacilitatedDiffusionTest", &EngineTest::FacilitatedDiffusionTest));

    testMap.insert(std::make_pair("BrainInjuryTest", &EngineTest::BrainInjuryTest));

    testMap.insert(std::make_pair("ConditionCombinations", &EngineTest::ConditionCombinations));
    testMap.insert(std::make_pair("SolverSpeedTest", &EngineTest::SolverSpeedTest));

    testMap.insert(std::make_pair("AcidBaseMathTest", &EngineTest::AcidBaseMathTest));
    testMap.insert(std::make_pair("AcidBaseFeedbackTest", &EngineTest::AcidBaseFeedbackTest));
    testMap.insert(std::make_pair("AcidBaseLimitsTest", &EngineTest::AcidBaseLimitsTest));
    testMap.insert(std::make_pair("AcidBaseExtremeTest", &EngineTest::AcidBaseExtremeTest));
    testMap.insert(std::make_pair("AcidBaseBloodGasTests", &EngineTest::AcidBaseBloodGasTests));

    testMap.insert(std::make_pair("AerosolTest", &EngineTest::AerosolTest));

    testMap.insert(std::make_pair("FourCompartmentTestSimple", &EngineTest::FourCompartmentTestSimple));
    testMap.insert(std::make_pair("AcidBaseFourCompartmentTest", &EngineTest::AcidBaseFourCompartmentTest));
    testMap.insert(std::make_pair("FiveCompartmentTestWithDiffusion", &EngineTest::FiveCompartmentTestWithDiffusion));
    testMap.insert(std::make_pair("AcidBaseFourCompartmentTestWithProductionConsumption", &EngineTest::AcidBaseFourCompartmentTestWithProductionConsumption));
    testMap.insert(std::make_pair("AcidBaseFiveCompartmentTestWithDiffusion", &EngineTest::AcidBaseFiveCompartmentTestWithDiffusion));
    testMap.insert(std::make_pair("AcidBaseFiveCompartmentTestWithProductionConsumptionAndDiffusion", &EngineTest::AcidBaseFiveCompartmentTestWithProductionConsumptionAndDiffusion));

    testMap.insert(std::make_pair("EmptyBlackBoxTest", &EngineTest::EmptyBlackBoxTest));
    testMap.insert(std::make_pair("ImposeFlowBlackBoxTest", &EngineTest::ImposeFlowBlackBoxTest));
    testMap.insert(std::make_pair("ImposePressureAndFlowBlackBoxTest", &EngineTest::ImposePressureAndFlowBlackBoxTest));
  }
END_NAMESPACE_EX
