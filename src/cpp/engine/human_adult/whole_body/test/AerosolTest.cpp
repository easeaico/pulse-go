/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/


#include "engine/human_adult/whole_body/test/EngineTest.h"
#include "engine/human_adult/whole_body/Engine.h"
#include "engine/human_adult/whole_body/controller/Controller.h"
#include "engine/human_adult/whole_body/controller/CompartmentManager.h"
#include "engine/human_adult/whole_body/controller/SubstanceManager.h"
#include "engine/human_adult/whole_body/system/physiology/Saturation.h"

#include "cdm/patient/SEPatient.h"
#include "cdm/circuit/fluid/SEFluidCircuit.h"
#include "cdm/circuit/fluid/SEFluidCircuitCalculator.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceAerosolization.h"
#include "cdm/substance/SESubstanceFraction.h"
#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/compartment/fluid/SEGasCompartmentGraph.h"
#include "cdm/compartment/fluid/SELiquidCompartmentGraph.h"
#include "cdm/compartment/substances/SEGasSubstanceQuantity.h"
#include "cdm/system/environment/SEEnvironment.h"
#include "cdm/system/environment/SEEnvironmentalConditions.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalarVolumePerPressure.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarInverseVolume.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEHistogramFractionVsLength.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarNegative1To1.h"
#include "cdm/properties/SEScalarLength.h"
#include "cdm/utils/DataTrack.h"
#include "cdm/utils/GeneralMath.h"
#include "cdm/utils/TimingProfile.h"
#include "cdm/utils/testing/SETestReport.h"
#include "cdm/utils/testing/SETestCase.h"
#include "cdm/utils/testing/SETestSuite.h"

namespace pulse { namespace human_adult_whole_body
{
  void EngineTest::AerosolTest(const std::string& sOutputDirectory)
  {
    m_Logger->SetLogFile(sOutputDirectory + "/AerosolTest.log");
    SESubstanceManager subMgr(m_Logger);

    SETestReport testReport = SETestReport(m_Logger);

    //// Create a suite
    //SETestSuite& albuteroluite = testReport.CreateTestSuite();
    //albuteroluite.SetName("Albuterol");
    //subMgr.LoadSubstanceDirectory();
    //SESubstance* albuterol = subMgr.GetSubstance("Albuterol");
    //SizeIndependentDepositionEfficencyCoefficientsTest(albuteroluite, *albuterol, 0.043737, 0.045737, 0.090432, 0.3115);
    //DepositionFractionTest(albuteroluite, *albuterol, 0.0805701, 0.0843696, 0.0701852, 0.111879);

    // Create a suite
    SETestSuite& normalDistributionSuite = testReport.CreateTestSuite();
    normalDistributionSuite.SetName("NormalDistribution");
    SESubstance& normalDistributedSubstance = *subMgr.GetSubstance("NormalDistribution");
    normalDistributedSubstance.GetAerosolization().GetBronchioleModifier().SetValue(0);//Need something here...
    normalDistributedSubstance.GetAerosolization().GetInflammationCoefficient().SetValue(0.5);//Need something here...
    SEHistogramFractionVsLength& concentrations = normalDistributedSubstance.GetAerosolization().GetParticulateSizeDistribution();
    // Boundary -1 is 0, i.e. no diameter
    concentrations.GetLength().push_back(1.0e-4); // Boundary 0
    concentrations.GetLength().push_back(1.0e-3); // Boundary 1
    concentrations.GetLength().push_back(1.0e-2); // Boundary 2
    concentrations.GetLength().push_back(1.0e-1); // Boundary 3
    concentrations.GetLength().push_back(1.0);    // Boundary 4
    concentrations.GetLength().push_back(1.0e1);  // Boundary 5
    concentrations.GetLength().push_back(1.0e2);  // Boundary 6
                                                  // Boundary n is infinity
    concentrations.SetLengthUnit(LengthUnit::um);

    // Bin -1 contains all of the particles that are less than 1.0e-4 um aerodynamic diameter
    concentrations.GetFraction().push_back(0.015); // Bin 0 (Bounded by Boundary 0 and Boundary 1)
    concentrations.GetFraction().push_back(0.035); // Bin 1 (Bounded by Boundary 1 and Boundary 2)
    concentrations.GetFraction().push_back(0.9);   // Bin 2 (Bounded by Boundary 2 and Boundary 3)
    concentrations.GetFraction().push_back(0.035); // Bin 3 (Bounded by Boundary 3 and Boundary 4)
    concentrations.GetFraction().push_back(0.015); // Bin 4 (Bounded by Boundary 4 and Boundary 5)
    concentrations.GetFraction().push_back(0);     // Bin 5 (Bounded by Boundary 5 and Boundary 6)
                                                   // Bin n contains 

    SizeIndependentDepositionEfficencyCoefficientsTest(normalDistributionSuite, normalDistributedSubstance, 0.043737, 0.045737, 0.090432, 0.3115);
    DepositionFractionTest(normalDistributionSuite, normalDistributedSubstance, 0.074548, 0.0774748, 0.0750439, 0.144502, 0.0754057, 0.145212);

    // Create another suite...
    // Create a suite
    SETestSuite& monodispersedSuite = testReport.CreateTestSuite();
    monodispersedSuite.SetName("Monodispersed");
    SESubstance& monodispersedSubstance = *subMgr.GetSubstance("MonoDispersed");
    monodispersedSubstance.GetAerosolization().GetBronchioleModifier().SetValue(0);//Need something here...
    monodispersedSubstance.GetAerosolization().GetInflammationCoefficient().SetValue(0.5);//Need something here...
    SEHistogramFractionVsLength& monoConcentrations = monodispersedSubstance.GetAerosolization().GetParticulateSizeDistribution();
    // Boundary -1 is 0, i.e. no diameter
    monoConcentrations.GetLength().push_back(1.0e-4); // Boundary 0
    monoConcentrations.GetLength().push_back(1.0e-3); // Boundary 1
    monoConcentrations.GetLength().push_back(1.0e-2); // Boundary 2
    monoConcentrations.GetLength().push_back(1.0e-1); // Boundary 3
    monoConcentrations.GetLength().push_back(1.0);    // Boundary 4
    monoConcentrations.GetLength().push_back(1.0e1);  // Boundary 5
    monoConcentrations.GetLength().push_back(1.0e2);  // Boundary 6
                                                  // Boundary n is infinity
    monoConcentrations.SetLengthUnit(LengthUnit::um);

    monoConcentrations.GetFraction().push_back(0);     // Bin 0 (Bounded by Boundary 0 and Boundary 1)
    monoConcentrations.GetFraction().push_back(0);     // Bin 1 (Bounded by Boundary 1 and Boundary 2)
    monoConcentrations.GetFraction().push_back(1.0);   // Bin 2 (Bounded by Boundary 2 and Boundary 3)
    monoConcentrations.GetFraction().push_back(0);     // Bin 3 (Bounded by Boundary 3 and Boundary 4)
    monoConcentrations.GetFraction().push_back(0);     // Bin 4 (Bounded by Boundary 4 and Boundary 5)
    monoConcentrations.GetFraction().push_back(0);     // Bin 5 (Bounded by Boundary 5 and Boundary 6)

    SizeIndependentDepositionEfficencyCoefficientsTest(monodispersedSuite, monodispersedSubstance, 0.024864, 0.025498, 0.083839, 0.32856);
    DepositionFractionTest(monodispersedSuite, monodispersedSubstance, 0.0438201, 0.0447655, 0.0714445, 0.15274, 0.0717642, 0.153452);

    // Create a suite
    SETestSuite& mono2Suite = testReport.CreateTestSuite();
    mono2Suite.SetName("Monodispersed_2");
    SESubstance& mono2Substance = *subMgr.GetSubstance("MonoDispersed2");
    mono2Substance.GetAerosolization().GetBronchioleModifier().SetValue(0);//Need something here...
    mono2Substance.GetAerosolization().GetInflammationCoefficient().SetValue(0.5);//Need something here...
    SEHistogramFractionVsLength& mono2Concentrations = mono2Substance.GetAerosolization().GetParticulateSizeDistribution();
    // Boundary -1 is 0, i.e. no diameter
    mono2Concentrations.GetLength().push_back(1.0e-4); // Boundary 0
    mono2Concentrations.GetLength().push_back(1.0e-3); // Boundary 1
    mono2Concentrations.GetLength().push_back(1.0e-2); // Boundary 2
    mono2Concentrations.GetLength().push_back(1.0e-1); // Boundary 3
    mono2Concentrations.GetLength().push_back(1.0);    // Boundary 4
    mono2Concentrations.GetLength().push_back(1.0e1);  // Boundary 5
    mono2Concentrations.GetLength().push_back(1.0e2);  // Boundary 6
                                                      // Boundary n is infinity
    mono2Concentrations.SetLengthUnit(LengthUnit::um);

    mono2Concentrations.GetFraction().push_back(0);     // Bin 0 (Bounded by Boundary 0 and Boundary 1)
    mono2Concentrations.GetFraction().push_back(0);     // Bin 1 (Bounded by Boundary 1 and Boundary 2)
    mono2Concentrations.GetFraction().push_back(0);   // Bin 2 (Bounded by Boundary 2 and Boundary 3)
    mono2Concentrations.GetFraction().push_back(0);     // Bin 3 (Bounded by Boundary 3 and Boundary 4)
    mono2Concentrations.GetFraction().push_back(0);     // Bin 4 (Bounded by Boundary 4 and Boundary 5)
    mono2Concentrations.GetFraction().push_back(1.0);     // Bin 5 (Bounded by Boundary 5 and Boundary 6)

    SizeIndependentDepositionEfficencyCoefficientsTest(mono2Suite, mono2Substance, 0.26841, 0.36689, 0.0012658, 0.0014995);
    DepositionFractionTest(mono2Suite, mono2Substance, 0.344719, 0.443596, 0.000941221, 0.000709455, 0.000946139, 0.000712776);

    // Create a suite
    SETestSuite& mono3Suite = testReport.CreateTestSuite();
    mono3Suite.SetName("Monodispersed_3");
    SESubstance& mono3Substance = *subMgr.GetSubstance("Monodispersed3");
    mono3Substance.GetAerosolization().GetBronchioleModifier().SetValue(0);//Need something here...
    mono3Substance.GetAerosolization().GetInflammationCoefficient().SetValue(0.5);//Need something here...
    SEHistogramFractionVsLength& mono3Concentrations = mono3Substance.GetAerosolization().GetParticulateSizeDistribution();
    // Boundary -1 is 0, i.e. no diameter
    mono3Concentrations.GetLength().push_back(1.0e-4); // Boundary 0
    mono3Concentrations.GetLength().push_back(1.0e-3); // Boundary 1
    mono3Concentrations.GetLength().push_back(1.0e-2); // Boundary 2
    mono3Concentrations.GetLength().push_back(1.0e-1); // Boundary 3
    mono3Concentrations.GetLength().push_back(1.0);    // Boundary 4
    mono3Concentrations.GetLength().push_back(1.0e1);  // Boundary 5
    mono3Concentrations.GetLength().push_back(1.0e2);  // Boundary 6
                                                       // Boundary n is infinity
    mono3Concentrations.SetLengthUnit(LengthUnit::um);

    mono3Concentrations.GetFraction().push_back(1.0);   // Bin 0 (Bounded by Boundary 0 and Boundary 1)
    mono3Concentrations.GetFraction().push_back(0);     // Bin 1 (Bounded by Boundary 1 and Boundary 2)
    mono3Concentrations.GetFraction().push_back(0);     // Bin 2 (Bounded by Boundary 2 and Boundary 3)
    mono3Concentrations.GetFraction().push_back(0);     // Bin 3 (Bounded by Boundary 3 and Boundary 4)
    mono3Concentrations.GetFraction().push_back(0);     // Bin 4 (Bounded by Boundary 4 and Boundary 5)
    mono3Concentrations.GetFraction().push_back(0);     // Bin 5 (Bounded by Boundary 5 and Boundary 6)

    SizeIndependentDepositionEfficencyCoefficientsTest(mono3Suite, mono3Substance, 0.43617, 0.77358, 0.21788, 0.005489);
    DepositionFractionTest(mono3Suite, mono3Substance, 0.417621, 0.592992, 0.112428, 0.00248646, 0.113745, 0.00250667);

    // Create a suite
    SETestSuite& zhangDispersion = testReport.CreateTestSuite();
    zhangDispersion.SetName("ZhangDispersion");
    SESubstance& zhangSubstance = *subMgr.GetSubstance("Zhang");
    zhangSubstance.GetAerosolization().GetBronchioleModifier().SetValue(0);//Need something here...
    zhangSubstance.GetAerosolization().GetInflammationCoefficient().SetValue(0.5);//Need something here...
    SEHistogramFractionVsLength& zhangConcentrations = zhangSubstance.GetAerosolization().GetParticulateSizeDistribution();
    // This histogram based on a digitization of Figure 4b in @cite zhang2012chemical
    zhangConcentrations.GetLength().push_back(10.308639); // Boundary 0
    zhangConcentrations.GetLength().push_back(16.263765); // Boundary 1
    zhangConcentrations.GetLength().push_back(25.854805); // Boundary 2
    zhangConcentrations.GetLength().push_back(41.101852); // Boundary 3
    zhangConcentrations.GetLength().push_back(65.83879);  // Boundary 4
    zhangConcentrations.GetLength().push_back(103.87276);  // Boundary 5
    zhangConcentrations.GetLength().push_back(165.1284);  // Boundary 6
    zhangConcentrations.GetLength().push_back(260.5203);  // Boundary 7
    zhangConcentrations.GetLength().push_back(417.31314);  // Boundary 8
    zhangConcentrations.GetLength().push_back(663.4103);  // Boundary 9
    zhangConcentrations.GetLength().push_back(1062.6803);  // Boundary 10
                                                       // Boundary n is infinity
    zhangConcentrations.SetLengthUnit(LengthUnit::um);

    zhangConcentrations.GetFraction().push_back(0.0028992);       // Bin 0 (Bounded by Boundary 0 and Boundary 1)
    zhangConcentrations.GetFraction().push_back(0.018143616);     // Bin 1 (Bounded by Boundary 1 and Boundary 2)
    zhangConcentrations.GetFraction().push_back(0.069563083);     // Bin 2 (Bounded by Boundary 2 and Boundary 3)
    zhangConcentrations.GetFraction().push_back(0.170665942);     // Bin 3 (Bounded by Boundary 3 and Boundary 4)
    zhangConcentrations.GetFraction().push_back(0.251295383);     // Bin 4 (Bounded by Boundary 4 and Boundary 5)
    zhangConcentrations.GetFraction().push_back(0.245390533);     // Bin 5 (Bounded by Boundary 5 and Boundary 6)
    zhangConcentrations.GetFraction().push_back(0.145956494);     // Bin 6 (Bounded by Boundary 6 and Boundary 7)
    zhangConcentrations.GetFraction().push_back(0.058885618);     // Bin 7 (Bounded by Boundary 7 and Boundary 8)
    zhangConcentrations.GetFraction().push_back(0.020538896);     // Bin 8 (Bounded by Boundary 8 and Boundary 9)
    zhangConcentrations.GetFraction().push_back(0.016661235);     // Bin 9 (Bounded by Boundary 9 and Boundary 10)

    SizeIndependentDepositionEfficencyCoefficientsTest(zhangDispersion, zhangSubstance, 0.25368, 0.3399, 0.00013825, 0.00022882);
    DepositionFractionTest(zhangDispersion, zhangSubstance, 0.333249, 0.423409, 0.000104459, 0.000108576, 0.000105008, 0.000109058);

    testReport.SerializeToFile(sOutputDirectory + "/AerosolTestReport.json");
  }

  void EngineTest::SizeIndependentDepositionEfficencyCoefficientsTest(SETestSuite& suite, SESubstance& substance,
    double expectedAirwayCoeff, double expectedCarinaCoeff, double expectedDeadSpaceCoeff, double expectedAlveoliCoeff)
  {
    double PercentTolerance = 0.1;
    // Set up our test report
    TimingProfile pTimer;
    pTimer.Start("Test");
    SETestCase& tc1 = suite.CreateTestCase();
    tc1.SetName(substance.GetName() + "SIDECo");

    Engine pe;;
    Controller& pc = (Controller&)pe.GetController();
    const pulse::SizeIndependentDepositionEfficencyCoefficient& SIDECoeff = pc.GetSubstances().GetSizeIndependentDepositionEfficencyCoefficient(substance);
    m_ss << "Airway: " << SIDECoeff.GetAirway();
    Info(m_ss);
    if (GeneralMath::PercentTolerance(SIDECoeff.GetAirway(), expectedAirwayCoeff) > PercentTolerance)
    {
      m_ss << " Carina SIDE Coefficient is not correct : " << SIDECoeff.GetAirway() << " expected " << expectedAirwayCoeff;
      tc1.AddFailure(m_ss);
    }
    m_ss << "Carina: " << SIDECoeff.GetCarina();
    Info(m_ss);
    if (GeneralMath::PercentTolerance(SIDECoeff.GetCarina(), expectedCarinaCoeff) > PercentTolerance)
    {
      m_ss << " Deadspace SIDE Coefficient is not correct : " << SIDECoeff.GetCarina() << " expected " << expectedCarinaCoeff;
      tc1.AddFailure(m_ss);
    }
    m_ss << "Deadspace: " << SIDECoeff.GetDeadSpace();
    Info(m_ss);
    if (GeneralMath::PercentTolerance(SIDECoeff.GetDeadSpace(), expectedDeadSpaceCoeff) > PercentTolerance)
    {
      m_ss << " Airway SIDE Coefficient is not correct : " << SIDECoeff.GetDeadSpace() << " expected " << expectedDeadSpaceCoeff;
      tc1.AddFailure(m_ss);
    }
    m_ss << "Alveoli: " << SIDECoeff.GetAlveoli();
    Info(m_ss);
    if (GeneralMath::PercentTolerance(SIDECoeff.GetAlveoli(), expectedAlveoliCoeff) > PercentTolerance)
    {
      m_ss << " Alveoli SIDE Coefficient is not correct : " << SIDECoeff.GetAlveoli() << " expected " << expectedAlveoliCoeff;
      tc1.AddFailure(m_ss);
    }
    tc1.GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);
  }

  void EngineTest::DepositionFractionTest(SETestSuite& suite, SESubstance& substance, double expectedAirwayDepFrac, double expectedCarinaDepFrac,
    double expectedLeftDeadSpaceDepFrac, double expectedLeftAlveoliDepFrac, double expectedRightDeadSpaceDepFrac, double expectedRightAlveoliDepFrac)
  {
    TimingProfile pTimer;
    pTimer.Start("Test");
    double PercentTolerance = 0.1;
    SETestCase& tc = suite.CreateTestCase();
    tc.SetName(substance.GetName() + "DepositionFraction");

    Engine pe;
    Controller& pc = (Controller&)pe.GetController();
    SEPatient patient(pc.GetLogger());
    patient.SerializeFromFile("./patients/StandardMale.json");
    pc.SetupPatient(patient);
    pc.GetSubstances().LoadSubstanceDirectory("./");
    pc.GetSaturationCalculator().Setup();
    pc.m_Config->Initialize("./", &pc.GetSubstances());
    pc.m_Config->UseExpandedKidneys(eSwitch::Off);
    pc.m_Config->EnableTissue(eSwitch::Off);
    pc.CreateCircuitsAndCompartments();
    pc.GetSubstances().InitializeGasCompartments();
    SEEnvironmentalConditions& env = pc.GetEnvironment().GetEnvironmentalConditions();
    SELiquidCompartment* ambient = pc.GetCompartments().GetLiquidCompartment(pulse::EnvironmentCompartment::Ambient);

    SEFluidCircuit* rCircuit = &pc.GetCircuits().GetRespiratoryCircuit();
    SEGasCompartmentGraph* rGraph = &pc.GetCompartments().GetRespiratoryGraph();
    SELiquidCompartmentGraph* aGraph = &pc.GetCompartments().GetAerosolGraph();

    const pulse::SizeIndependentDepositionEfficencyCoefficient& SIDECoeff = pc.GetSubstances().GetSizeIndependentDepositionEfficencyCoefficient(substance);

    SEFluidCircuitPath* driverPath = rCircuit->GetPath(pulse::RespiratoryPath::EnvironmentToRespiratoryMuscle);
    SEGasTransporter    gtxpt(VolumePerTimeUnit::L_Per_s, VolumeUnit::L, VolumeUnit::L, NoUnit::unitless, pc.GetLogger());
    SELiquidTransporter ltxpt(VolumePerTimeUnit::mL_Per_s, VolumeUnit::mL, MassUnit::ug, MassPerVolumeUnit::ug_Per_mL, pc.GetLogger());
    SEFluidCircuitCalculator calc(VolumePerPressureUnit::L_Per_cmH2O, VolumePerTimeUnit::L_Per_s, PressureTimeSquaredPerVolumeUnit::cmH2O_s2_Per_L, PressureUnit::cmH2O, VolumeUnit::L, PressureTimePerVolumeUnit::cmH2O_s_Per_L, pc.GetLogger());

    //Set the reference not pressure to the standard environment
    //This is needed because we're not setting the Environment during initialization in this unit test
    rCircuit->GetNode(pulse::EnvironmentNode::Ambient)->GetNextPressure().Set(env.GetAtmosphericPressure());
    rCircuit->GetNode(pulse::EnvironmentNode::Ambient)->GetPressure().Set(env.GetAtmosphericPressure());
    //Precharge the stomach to prevent negative volume
    rCircuit->GetNode(pulse::RespiratoryNode::Stomach)->GetNextPressure().Set(env.GetAtmosphericPressure());
    rCircuit->GetNode(pulse::RespiratoryNode::Stomach)->GetPressure().Set(env.GetAtmosphericPressure());

    SEFluidCircuitPath* Env2Airway = rCircuit->GetPath(pulse::RespiratoryPath::EnvironmentToAirway);

    SELiquidSubstanceQuantity* airwayParticulate = nullptr;
    SELiquidSubstanceQuantity* carinaParticulate = nullptr;
    SELiquidSubstanceQuantity* leftAnatomicDeadSpaceParticulate = nullptr;
    SELiquidSubstanceQuantity* leftAlveoliParticulate = nullptr;
    SELiquidSubstanceQuantity* rightAnatomicDeadSpaceParticulate = nullptr;
    SELiquidSubstanceQuantity* rightAlveoliParticulate = nullptr;

    //Circuit Analysis Test --------------------------------------------------
    //Execution parameters
    double time = 0;
    double deltaT_s = 1.0 / 50.0;

    //Drive waveform parameters
    double period = 5.0;
    double runTime_s = period * 2;
    double alpha = (2 * 3.1415) / (period);
    double driverPressure_cmH2O = 0.0;
    double amplitude_cmH2O = 6.0;
    double yOffset = -12.0;
    double sinusoid = 0;

    DataTrack trk;
    std::ofstream file;
    double totalInspiredAir_mL = 0;
    double totalExspiredAir_mL = 0;
    double totalInspiredParticulate_ug = 0;

    double totalParticulateDeposited_ug = 0;
    double depositedAirwayParticulate_ug = 0;
    double depositedCarinaParticulate_ug = 0;
    double depositedLeftAnatomicDeadSpaceParticulate_ug = 0;
    double depositedLeftAlveoliParticulate_ug = 0;
    double depositedRightAnatomicDeadSpaceParticulate_ug = 0;
    double depositedRightAlveoliParticulate_ug = 0;

    for (unsigned int i = 0; i < runTime_s / deltaT_s; i++)
    {
      sinusoid = cos(alpha * time);
      //PreProcess - Push driver pressure and  variable compliance data into the Circuit
      driverPressure_cmH2O = yOffset + amplitude_cmH2O * sinusoid;   //compute new pressure
      driverPath->GetNextPressureSource().SetValue(driverPressure_cmH2O, PressureUnit::cmH2O);

      //Process - Execute the circuit
      calc.Process(*rCircuit, deltaT_s);
      //Execute the substance transport function
      gtxpt.Transport(*rGraph, deltaT_s);
      // Do it again for aerosols
      if (aGraph != nullptr)
        ltxpt.Transport(*aGraph, deltaT_s);
      //convert 'Next' values to current
      calc.PostProcess(*rCircuit);

      trk.Track("DriverPressure_cmH2O", time, driverPressure_cmH2O);
      trk.Track("Env2Airway_mL/s", time, Env2Airway->GetFlow(VolumePerTimeUnit::mL_Per_s));

      trk.Track("TotalAirwayPreParticulate_ug", time, airwayParticulate == nullptr ? 0 : airwayParticulate->GetMass(MassUnit::ug));
      trk.Track("AirwayAerosol_mL", time, aGraph->GetCompartment(pulse::PulmonaryCompartment::Airway)->GetVolume(VolumeUnit::mL));
      trk.Track("AirwayAir_mL", time, rGraph->GetCompartment(pulse::PulmonaryCompartment::Airway)->GetVolume(VolumeUnit::mL));

      if (i > runTime_s * 0.5 / deltaT_s)
      {// Completed a cycle now start tracking Deposition and total inspired Air

        if (airwayParticulate == nullptr)
        {
          double aerosolConcentration_g_Per_m3 = 2.5;
          pc.GetSubstances().AddActiveSubstance(substance);
          ambient->GetSubstanceQuantity(substance)->GetConcentration().SetValue(aerosolConcentration_g_Per_m3, MassPerVolumeUnit::g_Per_m3);
          ambient->Balance(BalanceLiquidBy::Concentration);

          airwayParticulate = aGraph->GetCompartment(pulse::PulmonaryCompartment::Airway)->GetSubstanceQuantity(substance);
          carinaParticulate = aGraph->GetCompartment(pulse::PulmonaryCompartment::Carina)->GetSubstanceQuantity(substance);
          leftAnatomicDeadSpaceParticulate = aGraph->GetCompartment(pulse::PulmonaryCompartment::LeftAnatomicDeadSpace)->GetSubstanceQuantity(substance);
          leftAlveoliParticulate = aGraph->GetCompartment(pulse::PulmonaryCompartment::LeftAlveoli)->GetSubstanceQuantity(substance);
          rightAnatomicDeadSpaceParticulate = aGraph->GetCompartment(pulse::PulmonaryCompartment::RightAnatomicDeadSpace)->GetSubstanceQuantity(substance);
          rightAlveoliParticulate = aGraph->GetCompartment(pulse::PulmonaryCompartment::RightAlveoli)->GetSubstanceQuantity(substance);

          // Initialize to the env concentrations
          // aerosolConcentration_g_Per_m3 = 0.; // Or initialize to zero if you want.
          airwayParticulate->GetConcentration().SetValue(aerosolConcentration_g_Per_m3, MassPerVolumeUnit::g_Per_m3); airwayParticulate->Balance(BalanceLiquidBy::Concentration);
          carinaParticulate->GetConcentration().SetValue(aerosolConcentration_g_Per_m3, MassPerVolumeUnit::g_Per_m3); carinaParticulate->Balance(BalanceLiquidBy::Concentration);
          leftAnatomicDeadSpaceParticulate->GetConcentration().SetValue(aerosolConcentration_g_Per_m3, MassPerVolumeUnit::g_Per_m3); leftAnatomicDeadSpaceParticulate->Balance(BalanceLiquidBy::Concentration);
          leftAlveoliParticulate->GetConcentration().SetValue(aerosolConcentration_g_Per_m3, MassPerVolumeUnit::g_Per_m3); leftAlveoliParticulate->Balance(BalanceLiquidBy::Concentration);
          rightAnatomicDeadSpaceParticulate->GetConcentration().SetValue(aerosolConcentration_g_Per_m3, MassPerVolumeUnit::g_Per_m3); rightAnatomicDeadSpaceParticulate->Balance(BalanceLiquidBy::Concentration);
          rightAlveoliParticulate->GetConcentration().SetValue(aerosolConcentration_g_Per_m3, MassPerVolumeUnit::g_Per_m3); rightAlveoliParticulate->Balance(BalanceLiquidBy::Concentration);
        }

        if (Env2Airway->GetFlow().IsPositive())
        {
          // Grab instantaneous        
          double inspiredAir_mL = Env2Airway->GetFlow(VolumePerTimeUnit::mL_Per_s) * deltaT_s;
          double inspiredParticulate_ug = Env2Airway->GetFlow(VolumePerTimeUnit::mL_Per_s) * ambient->GetSubstanceQuantity(substance)->GetConcentration(MassPerVolumeUnit::ug_Per_mL) * deltaT_s;
          // Totals from instantaneous
          totalInspiredAir_mL += inspiredAir_mL;
          totalInspiredParticulate_ug += inspiredParticulate_ug;
        }
        else
        {
          double exspiredAir_mL = Env2Airway->GetFlow(VolumePerTimeUnit::mL_Per_s) * deltaT_s;
          totalExspiredAir_mL += exspiredAir_mL;
        }
        if (true)
        {
          // Calculate the deposited mass by concentration and flow
          depositedAirwayParticulate_ug = airwayParticulate->GetConcentration(MassPerVolumeUnit::ug_Per_mL) * aGraph->GetCompartment(pulse::PulmonaryCompartment::Airway)->GetInFlow(VolumePerTimeUnit::mL_Per_s) * deltaT_s * SIDECoeff.GetAirway();
          depositedCarinaParticulate_ug = carinaParticulate->GetConcentration(MassPerVolumeUnit::ug_Per_mL) * aGraph->GetCompartment(pulse::PulmonaryCompartment::Carina)->GetInFlow(VolumePerTimeUnit::mL_Per_s) * deltaT_s * SIDECoeff.GetCarina();
          depositedLeftAnatomicDeadSpaceParticulate_ug = leftAnatomicDeadSpaceParticulate->GetConcentration(MassPerVolumeUnit::ug_Per_mL) * aGraph->GetCompartment(pulse::PulmonaryCompartment::LeftAnatomicDeadSpace)->GetInFlow(VolumePerTimeUnit::mL_Per_s) * deltaT_s * SIDECoeff.GetDeadSpace();
          depositedLeftAlveoliParticulate_ug = leftAlveoliParticulate->GetConcentration(MassPerVolumeUnit::ug_Per_mL) * aGraph->GetCompartment(pulse::PulmonaryCompartment::LeftAlveoli)->GetInFlow(VolumePerTimeUnit::mL_Per_s) * deltaT_s * SIDECoeff.GetAlveoli();
          depositedRightAnatomicDeadSpaceParticulate_ug = rightAnatomicDeadSpaceParticulate->GetConcentration(MassPerVolumeUnit::ug_Per_mL) * aGraph->GetCompartment(pulse::PulmonaryCompartment::RightAnatomicDeadSpace)->GetInFlow(VolumePerTimeUnit::mL_Per_s) * deltaT_s * SIDECoeff.GetDeadSpace();
          depositedRightAlveoliParticulate_ug = rightAlveoliParticulate->GetConcentration(MassPerVolumeUnit::ug_Per_mL) * aGraph->GetCompartment(pulse::PulmonaryCompartment::RightAlveoli)->GetInFlow(VolumePerTimeUnit::mL_Per_s) * deltaT_s * SIDECoeff.GetAlveoli();

          // Total deposited
          totalParticulateDeposited_ug += depositedAirwayParticulate_ug + depositedCarinaParticulate_ug +
            depositedLeftAnatomicDeadSpaceParticulate_ug + depositedLeftAlveoliParticulate_ug +
            depositedRightAnatomicDeadSpaceParticulate_ug + depositedRightAlveoliParticulate_ug;

          // Remove the delta from the compartment substance quantity mass
          airwayParticulate->GetMass().Increment(-depositedAirwayParticulate_ug, MassUnit::ug); airwayParticulate->Balance(BalanceLiquidBy::Mass);
          carinaParticulate->GetMass().Increment(-depositedCarinaParticulate_ug, MassUnit::ug); carinaParticulate->Balance(BalanceLiquidBy::Mass);
          leftAnatomicDeadSpaceParticulate->GetMass().Increment(-depositedLeftAnatomicDeadSpaceParticulate_ug, MassUnit::ug); leftAnatomicDeadSpaceParticulate->Balance(BalanceLiquidBy::Mass);
          leftAlveoliParticulate->GetMass().Increment(-depositedLeftAlveoliParticulate_ug, MassUnit::ug); leftAlveoliParticulate->Balance(BalanceLiquidBy::Mass);
          rightAnatomicDeadSpaceParticulate->GetMass().Increment(-depositedRightAnatomicDeadSpaceParticulate_ug, MassUnit::ug); rightAnatomicDeadSpaceParticulate->Balance(BalanceLiquidBy::Mass);
          rightAlveoliParticulate->GetMass().Increment(-depositedRightAlveoliParticulate_ug, MassUnit::ug); rightAlveoliParticulate->Balance(BalanceLiquidBy::Mass);
          // Add the delta to the compartment substance quantity mass deposited
          airwayParticulate->GetMassDeposited().Increment(depositedAirwayParticulate_ug, MassUnit::ug);
          carinaParticulate->GetMassDeposited().Increment(depositedCarinaParticulate_ug, MassUnit::ug);
          leftAnatomicDeadSpaceParticulate->GetMassDeposited().Increment(depositedLeftAnatomicDeadSpaceParticulate_ug, MassUnit::ug);
          leftAlveoliParticulate->GetMassDeposited().Increment(depositedLeftAlveoliParticulate_ug, MassUnit::ug);
          rightAnatomicDeadSpaceParticulate->GetMassDeposited().Increment(depositedRightAnatomicDeadSpaceParticulate_ug, MassUnit::ug);
          rightAlveoliParticulate->GetMassDeposited().Increment(depositedRightAlveoliParticulate_ug, MassUnit::ug);
        }
      }

      trk.Track("AirwayConcentration_ug_Per_mL", time, airwayParticulate == nullptr ? 0 : airwayParticulate->GetConcentration(MassPerVolumeUnit::ug_Per_mL));
      trk.Track("CarinaConcentration_ug_Per_mL", time, carinaParticulate == nullptr ? 0 : carinaParticulate->GetConcentration(MassPerVolumeUnit::ug_Per_mL));

      trk.Track("AirwayInFlow_mL_Per_s", time, aGraph->GetCompartment(pulse::PulmonaryCompartment::Airway)->GetInFlow(VolumePerTimeUnit::mL_Per_s));
      trk.Track("CarinaInFlow_mL_Per_s", time, aGraph->GetCompartment(pulse::PulmonaryCompartment::Carina)->GetInFlow(VolumePerTimeUnit::mL_Per_s));

      trk.Track("TotalInspiredAir_mL", time, totalInspiredAir_mL);
      trk.Track("TotalExspiredAir_mL", time, totalExspiredAir_mL);
      trk.Track("TotalInspiredParticulate_ug", time, totalInspiredParticulate_ug);
      trk.Track("TotalDepositedParticulate_ug", time, totalParticulateDeposited_ug);
      trk.Track("TotalAirwayPostParticulate_ug", time, airwayParticulate == nullptr ? 0 : airwayParticulate->GetMass(MassUnit::ug));
      trk.Track("AirwayParticulateDeposited_ug", time, depositedAirwayParticulate_ug);
      trk.Track("TotalAirwayParticulateDeposited_ug", time, airwayParticulate == nullptr ? 0 : airwayParticulate->GetMassDeposited(MassUnit::ug));
      trk.Track("CarinaParticulate_ug", time, carinaParticulate == nullptr ? 0 : carinaParticulate->GetMass(MassUnit::ug));
      trk.Track("CarinaParticulateDeposited_ug", time, carinaParticulate == nullptr ? 0 : carinaParticulate->GetMassDeposited(MassUnit::ug));
      trk.Track("LeftDeadSpaceParticulate_ug", time, leftAnatomicDeadSpaceParticulate == nullptr ? 0 : leftAnatomicDeadSpaceParticulate->GetMass(MassUnit::ug));
      trk.Track("LeftDeadSpaceParticulateDeposited_ug", time, leftAnatomicDeadSpaceParticulate == nullptr ? 0 : leftAnatomicDeadSpaceParticulate->GetMassDeposited(MassUnit::ug));
      trk.Track("LeftAlveoliParticulate_ug", time, leftAlveoliParticulate == nullptr ? 0 : leftAlveoliParticulate->GetMass(MassUnit::ug));
      trk.Track("LeftAlveoliParticulateDeposited_ug", time, leftAlveoliParticulate == nullptr ? 0 : leftAlveoliParticulate->GetMassDeposited(MassUnit::ug));
      trk.Track("RightDeadSpaceParticulate_ug", time, rightAnatomicDeadSpaceParticulate == nullptr ? 0 : rightAnatomicDeadSpaceParticulate->GetMass(MassUnit::ug));
      trk.Track("RightDeadSpaceParticulateDeposited_ug", time, rightAnatomicDeadSpaceParticulate == nullptr ? 0 : rightAnatomicDeadSpaceParticulate->GetMassDeposited(MassUnit::ug));
      trk.Track("RightAlveoliParticulate_ug", time, rightAlveoliParticulate == nullptr ? 0 : rightAlveoliParticulate->GetMass(MassUnit::ug));
      trk.Track("RightAlveoliParticulateDeposited_ug", time, rightAlveoliParticulate == nullptr ? 0 : rightAlveoliParticulate->GetMassDeposited(MassUnit::ug));

      if (i == 0)
        trk.CreateFile(std::string("./test_results/unit_tests/Pulse/" + substance.GetName() + "DepositionFraction.csv").c_str(), file);
      trk.StreamTrackToFile(file);

      time += deltaT_s;
    }

    // Do all the deltas match?
    double expectedInspiredParticulate_ug = totalInspiredAir_mL * ambient->GetSubstanceQuantity(substance)->GetConcentration(MassPerVolumeUnit::ug_Per_mL);
    if (GeneralMath::PercentTolerance(totalInspiredParticulate_ug, expectedInspiredParticulate_ug) > PercentTolerance)
    {
      m_ss << " Compartment particulate mass does not expected particulate mass : " << totalInspiredParticulate_ug << " expected " << expectedInspiredParticulate_ug;
      tc.AddFailure(m_ss);
    }

    m_ss << "Total Particulate Inhaled : " << totalInspiredParticulate_ug;
    Info(m_ss);
    m_ss << "Total Particulate Deposited : " << totalParticulateDeposited_ug;
    Info(m_ss);

    m_ss << "Particulate Deposited in Airway: " << airwayParticulate->GetMassDeposited(MassUnit::ug);
    Info(m_ss);
    if (GeneralMath::PercentTolerance(expectedAirwayDepFrac, airwayParticulate->GetMassDeposited(MassUnit::ug) / totalInspiredParticulate_ug) > PercentTolerance)
    {
      m_ss << " Airway Particle Deposition is not correct : " << airwayParticulate->GetMassDeposited(MassUnit::ug) / totalInspiredParticulate_ug << " expected " << expectedAirwayDepFrac;
      tc.AddFailure(m_ss);
    }

    m_ss << "Particulate Deposited in Carina: " << carinaParticulate->GetMassDeposited(MassUnit::ug);
    Info(m_ss);
    if (GeneralMath::PercentTolerance(expectedCarinaDepFrac, carinaParticulate->GetMassDeposited(MassUnit::ug) / totalInspiredParticulate_ug) > PercentTolerance)
    {
      m_ss << " Carina Particle Deposition is not correct : " << carinaParticulate->GetMassDeposited(MassUnit::ug) / totalInspiredParticulate_ug << " expected " << expectedCarinaDepFrac;
      tc.AddFailure(m_ss);
    }

    m_ss << "Particulate Deposited in Left Dead Space : " << leftAnatomicDeadSpaceParticulate->GetMassDeposited(MassUnit::ug);
    Info(m_ss);
    if (GeneralMath::PercentTolerance(expectedLeftDeadSpaceDepFrac, leftAnatomicDeadSpaceParticulate->GetMassDeposited(MassUnit::ug) / totalInspiredParticulate_ug) > PercentTolerance)
    {
      m_ss << " Left Dead Space Particle Deposition is not correct : " << leftAnatomicDeadSpaceParticulate->GetMassDeposited(MassUnit::ug) / totalInspiredParticulate_ug << " expected " << expectedLeftDeadSpaceDepFrac;
      tc.AddFailure(m_ss);
    }

    m_ss << "Particulate Deposited in Left Alveoli : " << leftAlveoliParticulate->GetMassDeposited(MassUnit::ug);
    Info(m_ss);
    if (GeneralMath::PercentTolerance(expectedLeftAlveoliDepFrac, leftAlveoliParticulate->GetMassDeposited(MassUnit::ug) / totalInspiredParticulate_ug) > PercentTolerance)
    {
      m_ss << " Left Alveoli Particle Deposition is not correct : " << leftAlveoliParticulate->GetMassDeposited(MassUnit::ug) / totalInspiredParticulate_ug << " expected " << expectedLeftAlveoliDepFrac;
      tc.AddFailure(m_ss);
    }

    m_ss << "Particulate Deposited in Right Dead Space : " << rightAnatomicDeadSpaceParticulate->GetMassDeposited(MassUnit::ug);
    Info(m_ss);
    if (GeneralMath::PercentTolerance(expectedRightDeadSpaceDepFrac, rightAnatomicDeadSpaceParticulate->GetMassDeposited(MassUnit::ug) / totalInspiredParticulate_ug) > PercentTolerance)
    {
      m_ss << " Right Dead Space Particle Deposition is not correct : " << rightAnatomicDeadSpaceParticulate->GetMassDeposited(MassUnit::ug) / totalInspiredParticulate_ug << " expected " << expectedRightDeadSpaceDepFrac;
      tc.AddFailure(m_ss);
    }

    m_ss << "Particulate Deposited in Right Alveoli : " << rightAlveoliParticulate->GetMassDeposited(MassUnit::ug);
    Info(m_ss);
    if (GeneralMath::PercentTolerance(expectedRightAlveoliDepFrac, rightAlveoliParticulate->GetMassDeposited(MassUnit::ug) / totalInspiredParticulate_ug) > PercentTolerance)
    {
      m_ss << " Right Alveoli Particle Deposition is not correct : " << rightAlveoliParticulate->GetMassDeposited(MassUnit::ug) / totalInspiredParticulate_ug << " expected " << expectedRightAlveoliDepFrac;
      tc.AddFailure(m_ss);
    }

    tc.GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);
  }
END_NAMESPACE_EX
