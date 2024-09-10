/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#include "cdm/test/CommonDataModelTest.h"

#define ENGINE_TESTS
#ifdef ENGINE_TESTS
  #include "engine/human_adult/whole_body/test/EngineTest.h"
  #include "engine/human_adult/ventilation_mechanics/test/EngineTest.h"
#endif
#include "cdm/utils/FileUtils.h"

int main(int argc, char* argv[])
{
  std::string cdmDir = "./test_results/unit_tests/cdm";
  Logger cdmLogger(cdmDir + "/CDMUnitTests.log");
  CommonDataModelTest cdmTest(&cdmLogger);

  std::string hawbDir = "./test_results/unit_tests/human_adult/whole_body";
  Logger hawbLogger(hawbDir + "/PulseUnitTests.log");
  pulse::human_adult_whole_body::EngineTest hawbTest(&hawbLogger);

  std::string havmDir = "./test_results/unit_tests/human_adult/ventilation_mechanics";
  Logger havmLogger(havmDir + "/RespiratoryEngineUnitTests.log");
  pulse::human_adult_ventilation_mechanics::EngineTest havmTest(&havmLogger);

  if (argc == 1)
  {
    try
    {
      // Easily run various tests of your choosing

      //////////////////////////////
      // Respiratory Engine Tests //
      //////////////////////////////
      //havmTest.SmokeTest(havmDir);
      //havmTest.GenerateScenarios();

      ////////////////////
      // Pulse Tests //
      ////////////////////

      //hawbTest.ReadScenarios(hawbDir);
      //hawbTest.ReuseEngine(hawbDir);
      //hawbTest.SerializationTest(hawbDir);
      //hawbTest.ConditionCombinations(hawbDir);
      //hawbTest.MultiEngineTest(hawbDir);
      //hawbTest.SolverSpeedTest(hawbDir);
      //hawbTest.SetupPatientTest(hawbDir);

      //hawbTest.CardiovascularCircuitAndTransportTest(hawbDir);
      //hawbTest.CardiovascularAndRenalCircuitAndTransportTest(hawbDir);
      //hawbTest.CardiovascularAndTissueCircuitAndTransportTest(hawbDir);
      //hawbTest.CardiovascularAndCerebrospinalFluidCircuitAndTransportTest(hawbDir);
      //hawbTest.FullCardiovascularCircuitAndTransportTest(hawbDir);
      //hawbTest.CardiovascularBloodGasesTest(hawbDir);
      //hawbTest.TuneCardiovascularCircuitTest(hawbDir);

      //hawbTest.RenalCircuitAndTransportTest(hawbDir);
      //hawbTest.RenalTGFFeedbackTest(hawbDir);
      //hawbTest.RenalTGFandUPRFeedbackTest(hawbDir);
      //hawbTest.RenalSecretionTest(hawbDir);
      //hawbTest.RenalUrinateTest(hawbDir);

      //hawbTest.RespiratoryCircuitAndTransportTest(hawbDir);
      //hawbTest.RespiratoryDriverTest(hawbDir);
      //hawbTest.AnesthesiaMachineCircuitAndTransportTest(hawbDir);
      //hawbTest.RespiratoryWithAnesthesiaMachineCircuitAndTransportTest(hawbDir);
      //hawbTest.MechanicalVentilatorCircuitAndTransportTest(hawbDir);
      //hawbTest.RespiratoryWithMechanicalVentilatorCircuitAndTransportTest(hawbDir);
      //hawbTest.RespiratoryWithInhalerCircuitAndTransportTest(hawbDir);
      //hawbTest.RespiratoryWithMechanicalVentilatorCircuitAndTransportTest(hawbDir);

      //hawbTest.InternalTemperatureVariableBMRCircuitTest(hawbDir);
      //hawbTest.InternalTemperatureVariableSkinCircuitTest(hawbDir);
      //hawbTest.InternalTemperatureVariableCoreCircuitTest(hawbDir);
      //hawbTest.EnvironmentVariableTemperatureCircuitTest(hawbDir);
      //hawbTest.CombinedInternalAndEnvironmentVariableBMRandTemperatureCircuitTest(hawbDir);
      //hawbTest.CombinedInternalAndEnvironmentSkinTempDropCircuitTest(hawbDir);
      //hawbTest.EnvironmentISO7730ComparisonTest(hawbDir);

      //hawbTest.DiffusionClearanceExcretionTests(hawbDir);
      //hawbTest.AlveolarOxygenDiffusionTest(hawbDir);
      //hawbTest.AlveolarCarbonDioxideDiffusionTest(hawbDir);
      //hawbTest.InstantPlusSimpleDiffusionTest(hawbDir);
      //hawbTest.SimpleDiffusionTwoCompartmentTest(hawbDir);
      //hawbTest.SimpleDiffusionFourCompartmentTest(hawbDir);
      //hawbTest.SimpleDiffusionHierarchyTest(hawbDir);
      //hawbTest.FacilitatedDiffusionTest(hawbDir);

      //hawbTest.BrainInjuryTest(hawbDir);

      //Acid Base Tests
      //hawbTest.AcidBaseMathTest(hawbDir);
      //hawbTest.AcidBaseFeedbackTest(hawbDir);
      //hawbTest.AcidBaseLimitsTest(hawbDir);
      //hawbTest.AcidBaseExtremeTest(hawbDir);
      //hawbTest.AcidBaseBloodGasTests(hawbDir);

      //Aerosol Tests

      //Black Box Tests
      //hawbTest.EmptyBlackBoxTest(hawbDir);
      //hawbTest.ImposeFlowBlackBoxTest(hawbDir);
      //hawbTest.ImposePressureAndFlowBlackBoxTest(hawbDir);

      //hawbTest.FourCompartmentTestSimple(hawbDir);
      //hawbTest.AcidBaseFourCompartmentTest(hawbDir);
      //hawbTest.FiveCompartmentTestWithDiffusion(hawbDir);
      //hawbTest.AcidBaseFourCompartmentTestWithProductionConsumption(hawbDir);
      //hawbTest.AcidBaseFiveCompartmentTestWithDiffusion(hawbDir);
      //hawbTest.AcidBaseFiveCompartmentTestWithProductionConsumptionAndDiffusion(hawbDir);

      // Our CV Characterization Driver  (Not part of the test suite)
      //std::string cvDir = "./test_results/unit_tests/pulse/CVTuningTests";
      //MKDIR(cvDir.c_str());
      //hawbTest.CardiovascularCircuitScaleTests(cvDir);

      ///////////////
      // CDM Tests //
      ///////////////

      // Compartment Tests
      //cdmTest.GasCompartmentTest(cdmDir);
      //cdmTest.LiquidCompartmentTest(cdmDir);
      //cdmTest.ThermalCompartmentTest(cdmDir);
      //cdmTest.TissueCompartmentTest(cdmDir);

      // Transport Tests
      //cdmTest.LargeFlowTransportTest(cdmDir);
      //cdmTest.LiquidTransportTest(cdmDir);
      //cdmTest.GasTransportTest(cdmDir);

      // BlackBox Tests
      //cdmTest.BasicBlackBoxComparisonTest(cdmDir);
      //cdmTest.BasicBlackBoxTest(cdmDir);
      //cdmTest.WindkesselBlackBoxTest(cdmDir);
      //cdmTest.BlackBoxComplianceTest(cdmDir);
      //cdmTest.BlackBoxSourcesTest(cdmDir);

      // I/O Tests
      //cdmTest.ActionTest(cdmDir);
      //cdmTest.ReadPatientDirectory(cdmDir);
      //cdmTest.ReadSubstanceDirectory(cdmDir);
      //cdmTest.ConvertScenarioLogs(cdmDir);

      // Property Tests
      cdmTest.ScalarTest(cdmDir);
      //cdmTest.UnitsTest(cdmDir);
      //cdmTest.WaveformInterpolatorTest(cdmDir);

      // Advanced Circuit Tests
      //cdmTest.ElectricalCircuitTest(cdmDir);
      //cdmTest.FluidCircuitTest(cdmDir);
      //cdmTest.ThermalCircuitTest(cdmDir);
      //cdmTest.CombinedCircuitTest(cdmDir);
      //cdmTest.InterCircuitComparisonTest(cdmDir);
      //cdmTest.InterCircuitIndividualTest(cdmDir);
      //cdmTest.CircuitErrorTest(cdmDir);
      //cdmTest.DynamicallyChangingCircuitTest(cdmDir);
      //cdmTest.NonZeroReferencePositive(cdmDir);
      //cdmTest.NonZeroReferenceNegative(cdmDir);
      //cdmTest.PolarizedCapacitorTest(cdmDir);
      //cdmTest.PreChargeComplianceZeroVolume(cdmDir);
      //cdmTest.PreChargeComplianceNonZeroVolume(cdmDir);
      //cdmTest.CircuitLockingTest(cdmDir);
      //cdmTest.ComplianceVolumeChange(cdmDir);

      // Basic Circuit Tests
      //cdmTest.BasicCircuitTest(cdmDir);
      //cdmTest.MultiphysicsElectricalCircuitTest(cdmDir);
      //cdmTest.MultiphysicsFluidCircuitTest(cdmDir);
      //cdmTest.MultiphysicsThermalCircuitTest(cdmDir);

      //cdmTest.SeriesRCDCTest(cdmDir);
      //cdmTest.SeriesRCSINTest(cdmDir);
      //cdmTest.SeriesRCPULSETest(cdmDir);

      //cdmTest.SeriesRDCTest(cdmDir);
      //cdmTest.SeriesRSINTest(cdmDir);
      //cdmTest.SeriesRPULSETest(cdmDir);

      //cdmTest.SeriesRLDCTest(cdmDir);
      //cdmTest.SeriesRLSINTest(cdmDir);
      //cdmTest.SeriesRLPULSETest(cdmDir);

      //cdmTest.ParallelRDCTest(cdmDir);
      //cdmTest.ParallelRSINTest(cdmDir);
      //cdmTest.ParallelRPULSETest(cdmDir);

      //cdmTest.ParallelRCDCTest(cdmDir);
      //cdmTest.ParallelRCSINTest(cdmDir);
      //cdmTest.ParallelRCPULSETest(cdmDir);

      //cdmTest.ParallelRLDCTest(cdmDir);
      //cdmTest.ParallelRLSINTest(cdmDir);
      //cdmTest.ParallelRLPULSETest(cdmDir);
      //cdmTest.ParallelRLSINCenteredTest(cdmDir);

      //cdmTest.SeriesRLCDCTest(cdmDir);
      //cdmTest.SeriesRLCSINTest(cdmDir);
      //cdmTest.SeriesRLCPULSETest(cdmDir);

      //cdmTest.ParallelRLCDCTest(cdmDir);
      //cdmTest.ParallelRLCSINTest(cdmDir);
      //cdmTest.ParallelRLCPULSETest(cdmDir);

      //cdmTest.SwitchRCDCTest(cdmDir);
      //cdmTest.SwitchRCSINTest(cdmDir);
      //cdmTest.SwitchRCPULSETest(cdmDir);

      //cdmTest.Comprehensive1DCTest(cdmDir);
      //cdmTest.Comprehensive1SINTest(cdmDir);
      //cdmTest.Comprehensive1PULSETest(cdmDir);

      //cdmTest.SimpleDiodeDCTest(cdmDir);
      //cdmTest.SimpleDiodeSINTest(cdmDir);
      //cdmTest.SimpleDiodePULSETest(cdmDir);

      //cdmTest.SeriesRCDCCurrentTest(cdmDir);
      //cdmTest.SeriesRCSINCurrentTest(cdmDir);
      //cdmTest.SeriesRCPULSECurrentTest(cdmDir);

      //cdmTest.ParallelRCDCCurrentTest(cdmDir);
      //cdmTest.ParallelRCSINCurrentTest(cdmDir);
      //cdmTest.ParallelRCPULSECurrentTest(cdmDir);

      //cdmTest.SeriesCapDCTest(cdmDir);
      //cdmTest.SeriesCapSINTest(cdmDir);
      //cdmTest.SeriesCapPULSETest(cdmDir);

      //cdmTest.ParallelRDCCurrentTest(cdmDir);
      //cdmTest.ParallelRSINCurrentTest(cdmDir);
      //cdmTest.ParallelRPULSECurrentTest(cdmDir);

      //cdmTest.CurrentCompDCTest(cdmDir);
      //cdmTest.CurrentCompSINTest(cdmDir);
      //cdmTest.CurrentCompPULSETest(cdmDir);

      //cdmTest.SeriesRLDCCurrentTest(cdmDir);
      //cdmTest.SeriesRLSINCurrentTest(cdmDir);
      //cdmTest.SeriesRLPULSECurrentTest(cdmDir);

      //cdmTest.ParallelRLDCCurrentTest(cdmDir);
      //cdmTest.ParallelRLSINCurrentTest(cdmDir);
      //cdmTest.ParallelRLPULSECurrentTest(cdmDir);

      //cdmTest.BasicDiodeDCCurrentTest(cdmDir);
      //cdmTest.BasicDiodeSINCurrentTest(cdmDir);
      //cdmTest.BasicDiodePULSECurrentTest(cdmDir);

      // NOTE: These next three tests seem to give errors while running, but they produce output...
      //cdmTest.SwitchRCDCCurrentTest(cdmDir);
      //cdmTest.SwitchRCSINCurrentTest(cdmDir);
      //cdmTest.SwitchRCPULSECurrentTest(cdmDir);

      //cdmTest.SeriesRLCDCCurrentTest(cdmDir);
      //cdmTest.SeriesRLCSINCurrentTest(cdmDir);
      //cdmTest.SeriesRLCPULSECurrentTest(cdmDir);

      //cdmTest.ParallelRLCDCCurrentTest(cdmDir);
      //cdmTest.ParallelRLCSINCurrentTest(cdmDir);
      //cdmTest.ParallelRLCPULSECurrentTest(cdmDir);

      //cdmTest.SeriesPressureSourceAdditionDCTest(cdmDir);
      //cdmTest.SeriesPressureSourceAdditionSINTest(cdmDir);
      //cdmTest.SeriesPressureSourceAdditionPULSETest(cdmDir);

      // NOTE: These next six tests seem to give errors while running, but it's intentional
      //cdmTest.SeriesCurrentSourceAdditionDCTest(cdmDir);
      //cdmTest.SeriesCurrentSourceAdditionSINTest(cdmDir);
      //cdmTest.SeriesCurrentSourceAdditionPULSETest(cdmDir);

      //cdmTest.ParallelPressureSourceAdditionDCTest(cdmDir);
      //cdmTest.ParallelPressureSourceAdditionSINTest(cdmDir);
      //cdmTest.ParallelPressureSourceAdditionPULSETest(cdmDir);

      //cdmTest.ParallelCurrentSourceAdditionDCTest(cdmDir);
      //cdmTest.ParallelCurrentSourceAdditionSINTest(cdmDir);
      //cdmTest.ParallelCurrentSourceAdditionPULSETest(cdmDir);

      //cdmTest.SeriesCapDCCurrentTest(cdmDir);
      //cdmTest.SeriesCapSINCurrentTest(cdmDir);
      //cdmTest.SeriesCapPULSECurrentTest(cdmDir);

      //cdmTest.ParallelCapDCTest(cdmDir);
      //cdmTest.ParallelCapSINTest(cdmDir);
      //cdmTest.ParallelCapPULSETest(cdmDir);

      //cdmTest.ParallelCapDCCurrentTest(cdmDir);
      //cdmTest.ParallelCapSINCurrentTest(cdmDir);
      //cdmTest.ParallelCapPULSECurrentTest(cdmDir);

      //cdmTest.SeriesIndDCTest(cdmDir);
      //cdmTest.SeriesIndSINTest(cdmDir);
      //cdmTest.SeriesIndPULSETest(cdmDir);

      //cdmTest.SeriesIndDCCurrentTest(cdmDir);
      //cdmTest.SeriesIndSINCurrentTest(cdmDir);
      //cdmTest.SeriesIndPULSECurrentTest(cdmDir);

      //cdmTest.ParallelIndDCTest(cdmDir);
      //cdmTest.ParallelIndSINTest(cdmDir);
      //cdmTest.ParallelIndPULSETest(cdmDir);

      //cdmTest.ParallelIndDCCurrentTest(cdmDir);
      //cdmTest.ParallelIndSINCurrentTest(cdmDir);
      //cdmTest.ParallelIndPULSECurrentTest(cdmDir);

      //These next three tests intentionally fail
      //cdmTest.BadDiodeDCTest(cdmDir);
      //cdmTest.BadDiodeSINTest(cdmDir);
      //cdmTest.BadDiodePULSETest(cdmDir);

      //cdmTest.ValidationComprehensive1DCTest(cdmDir);
      //cdmTest.ValidationComprehensive1SINTest(cdmDir);
      //cdmTest.ValidationComprehensive1PULSETest(cdmDir);
      //cdmTest.ValidationComprehensive1SINCenteredTest(cdmDir);

      //cdmTest.ValidationComprehensive2DCTest(cdmDir);
      //cdmTest.ValidationComprehensive2SINTest(cdmDir);
      //cdmTest.ValidationComprehensive2PULSETest(cdmDir);
      //cdmTest.ValidationComprehensive2SINCenteredTest(cdmDir);
    }
    catch (CommonDataModelException& ex)
    {
      std::cerr << ex.what() << std::endl;
    }
    catch (std::exception& ex)
    {
      std::cerr << ex.what() << std::endl;
    }
    catch (...)
    {
      std::cerr << "Exception!!" << std::endl;
    }
    return 0;
  }
  else
  {
    if (cdmTest.RunTest(argv[1], "./test_results/unit_tests/cdm"))
      return 0;
    //if (hawbTest.RunTest(argv[1], "./test_results/unit_tests/pulse"))
    //  return 0;
  }
  return 1;
}
