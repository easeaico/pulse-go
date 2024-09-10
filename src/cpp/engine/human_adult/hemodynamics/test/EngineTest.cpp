/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/human_adult/ventilation_mechanics/test/EngineTest.h"
#include "cdm/utils/testing/SETestReport.h"
#include "cdm/utils/testing/SETestCase.h"
#include "cdm/utils/testing/SETestSuite.h"
#include "cdm/utils/ConfigParser.h"

#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEAdvanceTime.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/patient/SEPatient.h"
#include "cdm/patient/actions/SERespiratoryMechanicsConfiguration.h"
#include "cdm/patient/actions/SEIntubation.h"
#include "cdm/scenario/SEScenario.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceFraction.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/system/equipment/mechanical_ventilator/SEMechanicalVentilator.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorContinuousPositiveAirwayPressure.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorPressureControl.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorVolumeControl.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorLeak.h"
#include "cdm/system/physiology/SERespiratorySystem.h"
#include "cdm/system/physiology/SERespiratoryMechanics.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarPressurePerVolume.h"
#include "cdm/properties/SEScalarPressureTimePerVolume.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalarVolumePerPressure.h"
#include "cdm/properties/SECurve.h"

namespace pulse { namespace human_adult_ventilation_mechanics
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

        m_ss << "Could not find " << testName << " or " << testName << "Test in PulseEngineTest." << std::endl;
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
    testMap.insert(std::make_pair("Smoketest", &EngineTest::SmokeTest));
  }

  void EngineTest::SmokeTest(const std::string& sTestDirectory)
  {
    m_Logger->SetLogFile(sTestDirectory + "/VentilationMechanicsSmokeTest.log");

    std::unique_ptr<PhysiologyEngine> e;

    SETestReport testReport(m_Logger);
    SETestSuite& testSuite = testReport.CreateTestSuite();
    SETestCase& ctor = testSuite.CreateTestCase("ctor");
    try
    {
      e = CreatePulseEngine(eModelType::HumanAdultVentilationMechanics, m_Logger);
    }
    catch (...)
    {
      ctor.AddFailure("Unable to instantiate a Respiratory Engine");
      return; // No sense in running the rest
    }

    std::string output;
    SETestCase& unsupported = testSuite.CreateTestCase("serialization");
    if (e->SerializeFromFile("Not Suported"))
      unsupported.AddFailure("Respiratory Engine does not support SerializeFromFile, but says it does?");
    if (e->SerializeToFile("Not Suported"))
      unsupported.AddFailure("Respiratory Engine does not support SerializeToFile, but says it does?");
    if (e->SerializeFromString("Not Suported", eSerializationFormat::JSON))
      unsupported.AddFailure("Respiratory Engine does not support SerializeFromString, but says it does?");
    if (e->SerializeToString(output, eSerializationFormat::JSON))
      unsupported.AddFailure("Respiratory Engine does not support SerializeToString, but says it does?");
  }

//#define RUN_PULSE
//#define TRIM_CSV

  void EngineTest::GenerateScenarios()
  {

    enum PatientType { Normal = 0, ARDS, COPD };
    enum VentilatorMode { VC_AC = 0, PC_AC, CPAP };

    std::map<PatientType, std::string> patientTypeString;
    patientTypeString[PatientType::Normal] = "Healthy";

#ifdef RUN_PULSE
    patientTypeString[PatientType::ARDS] = "Restrictive";
    patientTypeString[PatientType::COPD] = "Obstructive";
#else
    patientTypeString[PatientType::ARDS] = "Restrictive";
    patientTypeString[PatientType::COPD] = "Obstructive";
#endif

    std::map<VentilatorMode, std::string> ventilatorModeString;
    ventilatorModeString[VentilatorMode::VC_AC] = "VC_AC";
    ventilatorModeString[VentilatorMode::PC_AC] = "PC_AC";
    ventilatorModeString[VentilatorMode::CPAP] = "CPAP";

    for (int patientIter = PatientType::Normal; patientIter <= PatientType::COPD; patientIter++)
    {
      PatientType currentPatientType = static_cast<PatientType>(patientIter);

      for (int ventilatorIter = VentilatorMode::VC_AC; ventilatorIter <= VentilatorMode::CPAP; ventilatorIter++)
      {
        VentilatorMode currentVentilatorMode = static_cast<VentilatorMode>(ventilatorIter);

        std::string testName = patientTypeString[currentPatientType] + "_" + ventilatorModeString[currentVentilatorMode];

#ifdef RUN_PULSE
        std::string sTestDirectory = "./test_results/scenarios/miscellaneous/ventilation_mechanics/";
        std::unique_ptr<PhysiologyEngine> e = CreatePulseEngine(eModelType::HumanAdultVentilationMechanics);
        SEDataRequestManager& drMgr = e->GetEngineTracker()->GetDataRequestManager();
        drMgr.SetResultsFilename(sTestDirectory + "/" + testName + "Results.csv");
        e->GetLogger()->SetLogFile(sTestDirectory + "/" + testName + ".log");
#else
        SEAdvanceTime adv;
        SEScenario s;
        SEDataRequestManager& drMgr = s.GetDataRequestManager();
#endif

        // Setup data requests
        drMgr.CreateMechanicalVentilatorDataRequest("AirwayPressure", PressureUnit::cmH2O);
        drMgr.CreateMechanicalVentilatorDataRequest("DynamicRespiratoryCompliance", VolumePerPressureUnit::L_Per_cmH2O);
        drMgr.CreateMechanicalVentilatorDataRequest("EndTidalCarbonDioxideFraction");
        drMgr.CreateMechanicalVentilatorDataRequest("EndTidalCarbonDioxidePressure", PressureUnit::cmH2O);
        drMgr.CreateMechanicalVentilatorDataRequest("EndTidalOxygenFraction");
        drMgr.CreateMechanicalVentilatorDataRequest("EndTidalOxygenPressure", PressureUnit::cmH2O);
        drMgr.CreateMechanicalVentilatorDataRequest("ExpiratoryFlow", VolumePerTimeUnit::L_Per_s);
        drMgr.CreateMechanicalVentilatorDataRequest("ExpiratoryTidalVolume", VolumeUnit::mL);
        drMgr.CreateMechanicalVentilatorDataRequest("InspiratoryExpiratoryRatio");
        drMgr.CreateMechanicalVentilatorDataRequest("InspiratoryFlow", VolumePerTimeUnit::L_Per_min);
        drMgr.CreateMechanicalVentilatorDataRequest("InspiratoryTidalVolume", VolumeUnit::mL);
        drMgr.CreateMechanicalVentilatorDataRequest("IntrinsicPositiveEndExpiratoryPressure", PressureUnit::cmH2O);
        drMgr.CreateMechanicalVentilatorDataRequest("LeakFraction");
        drMgr.CreateMechanicalVentilatorDataRequest("MeanAirwayPressure", PressureUnit::cmH2O);
        drMgr.CreateMechanicalVentilatorDataRequest("PeakInspiratoryPressure", PressureUnit::cmH2O);
        drMgr.CreateMechanicalVentilatorDataRequest("PlateauPressure", PressureUnit::cmH2O);
        drMgr.CreateMechanicalVentilatorDataRequest("PositiveEndExpiratoryPressure", PressureUnit::cmH2O);
        drMgr.CreateMechanicalVentilatorDataRequest("PulmonaryResistance", PressureTimePerVolumeUnit::cmH2O_s_Per_L);
        drMgr.CreateMechanicalVentilatorDataRequest("RespirationRate", FrequencyUnit::Per_min);
        drMgr.CreateMechanicalVentilatorDataRequest("StaticRespiratoryCompliance", VolumePerPressureUnit::L_Per_cmH2O);
        drMgr.CreateMechanicalVentilatorDataRequest("TidalVolume", VolumeUnit::mL);
        drMgr.CreateMechanicalVentilatorDataRequest("TotalLungVolume", VolumeUnit::mL);
        drMgr.CreateMechanicalVentilatorDataRequest("TotalPulmonaryVentilation", VolumePerTimeUnit::L_Per_s);

#ifdef RUN_PULSE
        SEPatientConfiguration pCfg(e->GetLogger());
        pCfg.GetPatient().SetSex(ePatient_Sex::Male);
        e->InitializeEngine(pCfg);
#else
        s.GetPatientConfiguration().GetPatient().SetSex(ePatient_Sex::Male);
#endif

        SERespiratoryMechanicsConfiguration config;
        SERespiratoryMechanics& mechanics = config.GetSettings();
        mechanics.SetActive(eSwitch::On);
        // Create a mechanics profile
        double resistance_cmH2O_s_Per_L = 13.0;
        double compliance_mL_Per_cmH2O = 50.0;
        double musclePressure_cmH2O = 13.0;
        double respirationRate_bpm = 15.0;
        double inspiratoryTime_s = 0.9;

        if (currentPatientType == PatientType::ARDS)
        {
          resistance_cmH2O_s_Per_L = 20.0;
          compliance_mL_Per_cmH2O = 30.0;
          musclePressure_cmH2O = 8.0;
          respirationRate_bpm = 25.0;
          inspiratoryTime_s = 0.6;
        }
        else if (currentPatientType == PatientType::COPD)
        {
          resistance_cmH2O_s_Per_L = 21.0;
          compliance_mL_Per_cmH2O = 53.0;
          musclePressure_cmH2O = 14.0;
          respirationRate_bpm = 18.0;
          inspiratoryTime_s = 1.0;
        }

        // These need to add to 1.0 (100%)
        const double UpperResistanceFraction = 0.5;
        const double BronchiResistanceFraction = 0.3;
        const double AlveoliDuctResistanceFraction = 0.2;

        double upperResistance = resistance_cmH2O_s_Per_L - (BronchiResistanceFraction * resistance_cmH2O_s_Per_L + AlveoliDuctResistanceFraction * resistance_cmH2O_s_Per_L) / 2;
        double bronchiResistance = 2 * (resistance_cmH2O_s_Per_L - upperResistance) - AlveoliDuctResistanceFraction * resistance_cmH2O_s_Per_L;
        double alveoliDuctResistance = 2 * (resistance_cmH2O_s_Per_L - upperResistance) - bronchiResistance;
        double sideResistance = bronchiResistance + alveoliDuctResistance;

        double RespiratorySideCompliance_L_Per_cmH2O = compliance_mL_Per_cmH2O / 2.0;
        double LungCompliance_L_Per_cmH2O = 2.0 * RespiratorySideCompliance_L_Per_cmH2O;
        double ChestWallCompliance_L_Per_cmH2O = LungCompliance_L_Per_cmH2O;

        double totalBreathTime_s = 1.0 / (respirationRate_bpm / 60.0);
        double inspiratoryFraction = inspiratoryTime_s / totalBreathTime_s;
        double expiratoryFraction = 1.0 - inspiratoryFraction;

        double InspiratoryRiseFraction = inspiratoryFraction;
        double InspiratoryHoldFraction = 0.0;
        double InspiratoryReleaseFraction = 0.1;
        double InspiratoryToExpiratoryPauseFraction = 1.0 - InspiratoryRiseFraction - InspiratoryReleaseFraction;
        double ExpiratoryRiseFraction = 0.0;
        double ExpiratoryHoldFraction = 0.0;
        double ExpiratoryReleaseFraction = 0.0;

        mechanics.GetUpperInspiratoryResistance().SetValue(upperResistance, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
        mechanics.GetUpperExpiratoryResistance().SetValue(upperResistance, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
        mechanics.GetLeftInspiratoryResistance().SetValue(sideResistance, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
        mechanics.GetLeftExpiratoryResistance().SetValue(sideResistance, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
        mechanics.GetRightInspiratoryResistance().SetValue(sideResistance, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
        mechanics.GetRightExpiratoryResistance().SetValue(sideResistance, PressureTimePerVolumeUnit::cmH2O_s_Per_L);

        SECurve& lcc = mechanics.GetLeftComplianceCurve();
        SESegmentConstant& lc = lcc.AddConstantSegment();
        lc.GetBeginVolume().SetValue(-std::numeric_limits<double>::infinity(), VolumeUnit::mL);
        lc.GetEndVolume().SetValue(std::numeric_limits<double>::infinity(), VolumeUnit::mL);
        lc.GetCompliance().SetValue(RespiratorySideCompliance_L_Per_cmH2O, VolumePerPressureUnit::mL_Per_cmH2O);
        SECurve& rcc = mechanics.GetRightComplianceCurve();
        SESegmentConstant& rc = rcc.AddConstantSegment();
        rc.GetBeginVolume().SetValue(-std::numeric_limits<double>::infinity(), VolumeUnit::mL);
        rc.GetEndVolume().SetValue(std::numeric_limits<double>::infinity(), VolumeUnit::mL);
        rc.GetCompliance().SetValue(RespiratorySideCompliance_L_Per_cmH2O, VolumePerPressureUnit::mL_Per_cmH2O);

        mechanics.GetInspiratoryRiseTime().SetValue(InspiratoryRiseFraction * totalBreathTime_s, TimeUnit::s);
        mechanics.GetInspiratoryHoldTime().SetValue(InspiratoryHoldFraction * totalBreathTime_s, TimeUnit::s);
        mechanics.GetInspiratoryReleaseTime().SetValue(InspiratoryReleaseFraction * totalBreathTime_s, TimeUnit::s);
        mechanics.GetInspiratoryToExpiratoryPauseTime().SetValue(InspiratoryToExpiratoryPauseFraction * totalBreathTime_s, TimeUnit::s);
        mechanics.GetExpiratoryRiseTime().SetValue(ExpiratoryRiseFraction * totalBreathTime_s, TimeUnit::s);
        mechanics.GetExpiratoryHoldTime().SetValue(ExpiratoryHoldFraction * totalBreathTime_s, TimeUnit::s);
        mechanics.GetExpiratoryReleaseTime().SetValue(ExpiratoryReleaseFraction * totalBreathTime_s, TimeUnit::s);

        mechanics.GetInspiratoryPeakPressure().SetValue(-13.0, PressureUnit::cmH2O);
        mechanics.GetExpiratoryPeakPressure().SetValue(0.0, PressureUnit::cmH2O);

#ifdef RUN_PULSE
        e->ProcessAction(config);
#else
        s.AddAction(config);
#endif

        // Intubate
        SEIntubation intubation;
        intubation.SetType(eIntubation_Type::Tracheal);
#ifdef RUN_PULSE
        e->ProcessAction(intubation);
#else
        s.AddAction(intubation);
#endif

        // Setup the ventilator

        if (currentPatientType == PatientType::Normal && currentVentilatorMode == VentilatorMode::VC_AC)
        {
          SEMechanicalVentilatorVolumeControl vc_ac;
          vc_ac.SetConnection(eSwitch::On);
          vc_ac.SetMode(eMechanicalVentilator_VolumeControlMode::AssistedControl);
          vc_ac.GetFlow().SetValue(60.0, VolumePerTimeUnit::L_Per_min);
          vc_ac.GetFractionInspiredOxygen().SetValue(0.21);
          vc_ac.GetInspiratoryPeriod().SetValue(1.0, TimeUnit::s);
          vc_ac.GetPositiveEndExpiratoryPressure().SetValue(5.0, PressureUnit::cmH2O);
          vc_ac.GetRespirationRate().SetValue(12.0, FrequencyUnit::Per_min);
          vc_ac.GetTidalVolume().SetValue(900.0, VolumeUnit::mL);

          SEMechanicalVentilatorLeak leak;
          leak.GetSeverity().SetValue(0.49);
#ifdef RUN_PULSE
          e->ProcessAction(vc_ac);
          e->ProcessAction(leak);
#else
          s.AddAction(vc_ac);
          s.AddAction(leak);
#endif
        }
        else if (currentPatientType == PatientType::Normal && currentVentilatorMode == VentilatorMode::PC_AC)
        {
          SEMechanicalVentilatorPressureControl pc_ac;
          pc_ac.SetConnection(eSwitch::On);
          pc_ac.GetFractionInspiredOxygen().SetValue(0.21);
          pc_ac.GetInspiratoryPeriod().SetValue(1.0, TimeUnit::s);
          pc_ac.GetInspiratoryPressure().SetValue(19.0, PressureUnit::cmH2O);
          pc_ac.GetPositiveEndExpiratoryPressure().SetValue(5.0, PressureUnit::cmH2O);
          pc_ac.GetRespirationRate().SetValue(12.0, FrequencyUnit::Per_min);
          pc_ac.GetSlope().SetValue(0.0, TimeUnit::s);

          SEMechanicalVentilatorLeak leak;
          leak.GetSeverity().SetValue(0.47);
#ifdef RUN_PULSE
          e->ProcessAction(pc_ac);
          e->ProcessAction(leak);
#else
          s.AddAction(pc_ac);
          s.AddAction(leak);
#endif
        }
        else if (currentPatientType == PatientType::Normal && currentVentilatorMode == VentilatorMode::CPAP)
        {
          SEMechanicalVentilatorContinuousPositiveAirwayPressure cpap;
          cpap.SetConnection(eSwitch::On);
          cpap.GetFractionInspiredOxygen().SetValue(0.21);
          cpap.GetDeltaPressureSupport().SetValue(10.0, PressureUnit::cmH2O);
          cpap.GetPositiveEndExpiratoryPressure().SetValue(5.0, PressureUnit::cmH2O);
          cpap.GetSlope().SetValue(0.2, TimeUnit::s);

          SEMechanicalVentilatorLeak leak;
          leak.GetSeverity().SetValue(0.42);
#ifdef RUN_PULSE
          e->ProcessAction(cpap);
          e->ProcessAction(leak);
#else
          s.AddAction(cpap);
          s.AddAction(leak);
#endif
        }
        else if (currentPatientType == PatientType::ARDS && currentVentilatorMode == VentilatorMode::VC_AC)
        {
          SEMechanicalVentilatorVolumeControl vc_ac;
          vc_ac.SetConnection(eSwitch::On);
          vc_ac.SetMode(eMechanicalVentilator_VolumeControlMode::AssistedControl);
          vc_ac.GetFlow().SetValue(40.0, VolumePerTimeUnit::L_Per_min);
          vc_ac.GetFractionInspiredOxygen().SetValue(0.21);
          vc_ac.GetInspiratoryPeriod().SetValue(1.1, TimeUnit::s);
          vc_ac.GetPositiveEndExpiratoryPressure().SetValue(5.0, PressureUnit::cmH2O);
          vc_ac.GetRespirationRate().SetValue(12.0, FrequencyUnit::Per_min);
          vc_ac.GetTidalVolume().SetValue(550.0, VolumeUnit::mL);

          SEMechanicalVentilatorLeak leak;
          leak.GetSeverity().SetValue(0.38);
#ifdef RUN_PULSE
          e->ProcessAction(vc_ac);
          e->ProcessAction(leak);
#else
          s.AddAction(vc_ac);
          s.AddAction(leak);
#endif
        }
        else if (currentPatientType == PatientType::ARDS && currentVentilatorMode == VentilatorMode::PC_AC)
        {
          SEMechanicalVentilatorPressureControl pc_ac;
          pc_ac.SetConnection(eSwitch::On);
          pc_ac.GetFractionInspiredOxygen().SetValue(0.21);
          pc_ac.GetInspiratoryPeriod().SetValue(1.1, TimeUnit::s);
          pc_ac.GetInspiratoryPressure().SetValue(23.0, PressureUnit::cmH2O);
          pc_ac.GetPositiveEndExpiratoryPressure().SetValue(5.0, PressureUnit::cmH2O);
          pc_ac.GetRespirationRate().SetValue(12.0, FrequencyUnit::Per_min);
          pc_ac.GetSlope().SetValue(0.0, TimeUnit::s);

          SEMechanicalVentilatorLeak leak;
          leak.GetSeverity().SetValue(0.32);
#ifdef RUN_PULSE
          e->ProcessAction(pc_ac);
          e->ProcessAction(leak);
#else
          s.AddAction(pc_ac);
          s.AddAction(leak);
#endif
        }
        else if (currentPatientType == PatientType::ARDS && currentVentilatorMode == VentilatorMode::CPAP)
        {
          SEMechanicalVentilatorContinuousPositiveAirwayPressure cpap;
          cpap.SetConnection(eSwitch::On);
          cpap.GetFractionInspiredOxygen().SetValue(0.21);
          cpap.GetDeltaPressureSupport().SetValue(10.0, PressureUnit::cmH2O);
          cpap.GetPositiveEndExpiratoryPressure().SetValue(5.0, PressureUnit::cmH2O);
          cpap.GetSlope().SetValue(0.2, TimeUnit::s);

          SEMechanicalVentilatorLeak leak;
          leak.GetSeverity().SetValue(0.26);
#ifdef RUN_PULSE
          e->ProcessAction(cpap);
          e->ProcessAction(leak);
#else
          s.AddAction(cpap);
          s.AddAction(leak);
#endif
        }
        else if (currentPatientType == PatientType::COPD && currentVentilatorMode == VentilatorMode::VC_AC)
        {
          SEMechanicalVentilatorVolumeControl vc_ac;
          vc_ac.SetConnection(eSwitch::On);
          vc_ac.SetMode(eMechanicalVentilator_VolumeControlMode::AssistedControl);
          vc_ac.GetFlow().SetValue(40.0, VolumePerTimeUnit::L_Per_min);
          vc_ac.GetFractionInspiredOxygen().SetValue(0.21);
          vc_ac.GetInspiratoryPeriod().SetValue(1.1, TimeUnit::s);
          vc_ac.GetPositiveEndExpiratoryPressure().SetValue(5.0, PressureUnit::cmH2O);
          vc_ac.GetRespirationRate().SetValue(12.0, FrequencyUnit::Per_min);
          vc_ac.GetTidalVolume().SetValue(500.0, VolumeUnit::mL);

          SEMechanicalVentilatorLeak leak;
          leak.GetSeverity().SetValue(0.37);
#ifdef RUN_PULSE
          e->ProcessAction(vc_ac);
          e->ProcessAction(leak);
#else
          s.AddAction(vc_ac);
          s.AddAction(leak);
#endif
        }
        else if (currentPatientType == PatientType::COPD && currentVentilatorMode == VentilatorMode::PC_AC)
        {
          SEMechanicalVentilatorPressureControl pc_ac;
          pc_ac.SetConnection(eSwitch::On);
          pc_ac.GetFractionInspiredOxygen().SetValue(0.21);
          pc_ac.GetInspiratoryPeriod().SetValue(1.2, TimeUnit::s);
          pc_ac.GetInspiratoryPressure().SetValue(12.0, PressureUnit::cmH2O);
          pc_ac.GetPositiveEndExpiratoryPressure().SetValue(5.0, PressureUnit::cmH2O);
          pc_ac.GetRespirationRate().SetValue(12.0, FrequencyUnit::Per_min);
          pc_ac.GetSlope().SetValue(0.0, TimeUnit::s);

          SEMechanicalVentilatorLeak leak;
          leak.GetSeverity().SetValue(0.36);
#ifdef RUN_PULSE
          e->ProcessAction(pc_ac);
          e->ProcessAction(leak);
#else
          s.AddAction(pc_ac);
          s.AddAction(leak);
#endif
        }
        else if (currentPatientType == PatientType::COPD && currentVentilatorMode == VentilatorMode::CPAP)
        {
          SEMechanicalVentilatorContinuousPositiveAirwayPressure cpap;
          cpap.SetConnection(eSwitch::On);
          cpap.GetFractionInspiredOxygen().SetValue(0.21);
          cpap.GetDeltaPressureSupport().SetValue(10.0, PressureUnit::cmH2O);
          cpap.GetPositiveEndExpiratoryPressure().SetValue(5.0, PressureUnit::cmH2O);
          cpap.GetSlope().SetValue(0.2, TimeUnit::s);

          SEMechanicalVentilatorLeak leak;
          leak.GetSeverity().SetValue(0.45);
#ifdef RUN_PULSE
          e->ProcessAction(cpap);
          e->ProcessAction(leak);
#else
          s.AddAction(cpap);
          s.AddAction(leak);
#endif
        }

        double fiveCycles_s = 5.0 * 60.0 / respirationRate_bpm;
        double simiulationTime_s = 15.0 + fiveCycles_s;
#ifdef RUN_PULSE
        // Advance time
        
        for (int i = 0; i < simiulationTime_s * 50.0; i++)
        {
          if (!e->AdvanceModelTime())  // Compute 1 time step
          {
            e->Error("Could not advance time in Respiratory Engine.");
            break;
          }

          // Do some cycles cycle before recording data
#ifdef TRIM_CSV
          if (i > fiveCycles_s * 50.0)
#endif
          {
            // Pull Track will pull data from the engine and append it to the file
            e->GetEngineTracker()->TrackData(e->GetSimulationTime(TimeUnit::s));
          }
        }
#else
        adv.GetTime().SetValue(simiulationTime_s,TimeUnit::s);
        s.AddAction(adv);

        std::string scenario_dir;
        ConfigSet* cfg = ConfigParser::FileToConfigSet("run.config");
        if (cfg->HasKey("scenario_dir"))
        {
          scenario_dir = cfg->GetValue("scenario_dir");
          s.SerializeToFile(scenario_dir+"/miscellaneous/ventilation_mechanics/"+testName+".json");
        }
        else
        {
          s.Error("Cannot read run.config");
        }
        delete cfg;
#endif

      }
    }
  }
END_NAMESPACE_EX
