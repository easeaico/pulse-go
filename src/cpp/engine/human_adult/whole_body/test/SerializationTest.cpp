/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/human_adult/whole_body/test/EngineTest.h"
#include "engine/human_adult/whole_body/Engine.h"
#include "engine/human_adult/whole_body/controller/Controller.h"

#include "cdm/engine/SETimedStabilization.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/patient/actions/SEConsciousRespiration.h"
#include "cdm/patient/actions/SEForcedExhale.h"
#include "cdm/patient/actions/SEForcedInhale.h"
#include "cdm/patient/actions/SEUseInhaler.h"
#include "cdm/patient/actions/SEForcedPause.h"
#include "cdm/patient/actions/SESubstanceBolus.h"
#include "cdm/system/equipment/anesthesia_machine/SEAnesthesiaMachine.h"
#include "cdm/system/equipment/anesthesia_machine/SEAnesthesiaMachineOxygenBottle.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineConfiguration.h"
#include "cdm/system/equipment/inhaler/SEInhaler.h"
#include "cdm/system/equipment/inhaler/actions/SEInhalerConfiguration.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarElectricPotential.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarHeatResistanceArea.h"
#include "cdm/properties/SEScalarLengthPerTime.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTemperature.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEFunctionVolumeVsTime.h"
#include "cdm/properties/SEScalarPower.h"
#include "cdm/utils/FileUtils.h"
#include "cdm/utils/testing/SETestReport.h"
#include "cdm/utils/testing/SETestCase.h"
#include "cdm/utils/testing/SETestSuite.h"

class HowToTracker
{
private:
  double m_dT_s;  // Cached Engine Time Step
  PhysiologyEngine& m_Engine;
public:
  HowToTracker(PhysiologyEngine& engine) : m_Engine(engine)
  {
    m_dT_s = m_Engine.GetTimeStep(TimeUnit::s);
  }
  ~HowToTracker(){}

  // This class will operate on seconds
  void AdvanceModelTime(double time_s)
  {
    int count = static_cast<int>(time_s / m_dT_s);
    for (int i = 0; i <= count; i++)
    {
      m_Engine.AdvanceModelTime();  // Compute 1 time step

      // Pull Track will pull data from the engine and append it to the file
      m_Engine.GetEngineTracker()->TrackData(m_Engine.GetSimulationTime(TimeUnit::s));
    }
  }
};

namespace pulse { namespace human_adult_whole_body
{
  void EngineTest::InhalerState(PhysiologyEngine* pc, HowToTracker& tracker)
  {
    pc->GetEngineTracker()->GetDataRequestManager().SetResultsFilename("InhalerResults.csv");
    SEPatientConfiguration pconfig;
    pconfig.SetPatientFile("StandardMale.json");
    if (!pc->InitializeEngine(pconfig))
    {
      std::cerr << "Could not load initialize engine, check the error" << std::endl;
      return;
    }

    tracker.AdvanceModelTime(30);

    SEInhalerConfiguration config(pc->GetLogger());
    config.GetConfiguration().SetSubstance(pc->GetSubstanceManager().GetSubstance("Albuterol"));
    config.GetConfiguration().GetMeteredDose().SetValue(90.0, MassUnit::ug);
    config.GetConfiguration().GetNozzleLoss().SetValue(0.040);
    pc->ProcessAction(config);

    SEConsciousRespiration cResp;

    SEForcedExhale& exhale = cResp.AddForcedExhale();
    exhale.GetExpiratoryReserveVolumeFraction().SetValue(1.0);
    exhale.GetExhalePeriod().SetValue(3.0, TimeUnit::s);

    SEForcedInhale& inhale = cResp.AddForcedInhale();
    inhale.GetInspiratoryCapacityFraction().SetValue(1.0);
    inhale.GetInhalePeriod().SetValue(5.0, TimeUnit::s);

    cResp.AddUseInhaler();

    SEForcedPause& pause = cResp.AddForcedPause();
    pause.GetPeriod().SetValue(10, TimeUnit::s);

    pc->ProcessAction(cResp);
    tracker.AdvanceModelTime(5);

    SEScalarTime now;// Make sure to tell the engine that we are at the same time
    // Save and Load the Engine State
    pc->GetLogger()->Info("Serializing");
    std::string state;
    pc->SerializeToString(state, eSerializationFormat::BINARY);
    now.SetValue(pc->GetSimulationTime(TimeUnit::s), TimeUnit::s);
    pc->SerializeFromString(state, eSerializationFormat::BINARY);
    pc->SetSimulationTime(now);

    // Change the results file
    pc->GetLogger()->SetLogFile("InhalerSerialization.log");
    std::remove("InhalerSerializationResults.csv");
    pc->GetEngineTracker()->GetDataRequestManager().SetResultsFilename("InhalerSerializationResults.csv");

    tracker.AdvanceModelTime(145);
  }

  void EngineTest::InjectSuccsState(PhysiologyEngine* pc, HowToTracker& tracker, const SESubstance& succs)
  {
    pc->GetEngineTracker()->GetDataRequestManager().SetResultsFilename("InjectSuccsResults.csv");
    SEPatientConfiguration pconfig;
    pconfig.SetPatientFile("StandardMale.json");
    if (!pc->InitializeEngine(pconfig))
    {
      std::cerr << "Could not load initialize engine, check the error" << std::endl;
      return;
    }

    SESubstanceBolus injection(succs);
    injection.SetAdminRoute(eSubstanceAdministration_Route::Intravenous);
    injection.GetConcentration().SetValue(4820, MassPerVolumeUnit::ug_Per_mL);
    injection.GetDose().SetValue(30, VolumeUnit::mL);
    pc->ProcessAction(injection);
    pc->AdvanceModelTime(1, TimeUnit::s);// Not tracking

    SEScalarTime now;// Make sure to tell the engine that we are at the same time

    // Change our results file name
    pc->GetLogger()->SetLogFile("InjectSuccsSerialization.log");
    std::remove("InjectSuccsSerialization.csv");
    pc->GetEngineTracker()->GetDataRequestManager().SetResultsFilename("InjectSuccsSerialization.csv");

    // Save and Load the Engine State
    pc->SerializeToFile("./MidBolusState.json");
    now.SetValue(pc->GetSimulationTime(TimeUnit::s), TimeUnit::s);
    pc->SerializeFromFile("./MidBolusState.json");
    pc->SetSimulationTime(now);

    tracker.AdvanceModelTime(15);

    SEAnesthesiaMachineConfiguration amConfig(pc->GetLogger());
    amConfig.GetConfiguration().SetConnection(eSwitch::On);
    amConfig.GetConfiguration().GetInletFlow().SetValue(5, VolumePerTimeUnit::L_Per_min);
    amConfig.GetConfiguration().GetInspiratoryExpiratoryRatio().SetValue(0.5);
    amConfig.GetConfiguration().GetOxygenFraction().SetValue(0.4);
    amConfig.GetConfiguration().SetOxygenSource(eAnesthesiaMachine_OxygenSource::Wall);
    amConfig.GetConfiguration().GetPositiveEndExpiratoryPressure().SetValue(3.0, PressureUnit::cmH2O);
    amConfig.GetConfiguration().SetPrimaryGas(eAnesthesiaMachine_PrimaryGas::Nitrogen);
    amConfig.GetConfiguration().GetReliefValvePressure().SetValue(20, PressureUnit::cmH2O);
    amConfig.GetConfiguration().GetRespiratoryRate().SetValue(16.0, FrequencyUnit::Per_min);
    amConfig.GetConfiguration().GetPeakInspiratoryPressure().SetValue(10, PressureUnit::cmH2O);
    amConfig.GetConfiguration().GetOxygenBottleOne().GetVolume().SetValue(660, VolumeUnit::L);
    amConfig.GetConfiguration().GetOxygenBottleTwo().GetVolume().SetValue(660, VolumeUnit::L);
    pc->ProcessAction(amConfig);
    tracker.AdvanceModelTime(5);

    pc->SerializeToFile("./AnesthesiaMachineState.json");
    now.SetValue(pc->GetSimulationTime(TimeUnit::s), TimeUnit::s);
    pc->SerializeFromFile("./AnesthesiaMachineState.json");
    pc->SetSimulationTime(now);

    tracker.AdvanceModelTime(40);
  }

  void EngineTest::SerializationTest(const std::string& sTestDirectory)
  {
    std::string testName = "SerializationTest";
    SETestReport testReport(m_Logger);
    SETestSuite& testSuite = testReport.CreateTestSuite();
    testSuite.SetName(testName);
    SETestCase& testCase = testSuite.CreateTestCase();

    std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
    pe->GetLogger()->LogToConsole(true);
    pe->GetLogger()->SetLogFile(sTestDirectory+"/SerializationTest.log");
    pe->GetLogger()->Info(testName);
    if (!pe->SerializeFromFile("./states/StandardMale@0s.json"))
    {
      testCase.AddFailure("Could not load state, check the error");
    }
    if (!pe->AdvanceModelTime(1, TimeUnit::s))
    {
      testCase.AddFailure("Could not advance time");
    }
    // Do it again
    if (!pe->SerializeFromFile("./states/StandardMale@0s.json"))
    {
      testCase.AddFailure("Could not load second state, check the error");
    }
    if (!pe->AdvanceModelTime(1, TimeUnit::s))
    {
      testCase.AddFailure("Could not advance time");
    }
    std::string ext = ".pbb";//".json";
    // If things don't match, change back to json and inspect files to see what is different

    size_t numLoads = 10;
    std::string stateBasename = "./test_results/unit_tests/pulse/SerializationTest/StandardMale@1s_";
    for (size_t i = 0; i < numLoads; i++)
    {
      std::string itrName = stateBasename + std::to_string(i);
      if (!pe->SerializeToFile(itrName + ext))
      {
        testCase.AddFailure("Could not save state, check the error");
      }
      if (!pe->SerializeFromFile(itrName + ext))
      {
        testCase.AddFailure("Could not load saved state, check the error");
      }
      if (!pe->SerializeToFile(itrName + ".reload" + ext))
      {
        testCase.AddFailure("Could not save state, check the error");
      }
      auto f1Size = FileSize(itrName + ext);
      auto f2Size = FileSize(itrName + ".reload" + ext);
      auto diff = f1Size - f2Size;
      testCase.Info("File size diff of " + std::to_string(diff) + " bytes");
      if (f1Size != f2Size)
      //if (std::fabs(f1Size - f2Size) > 200) // If we do json
      {// Protobuf does not write out default values to json (not doing VERBOSE_JSON, so things might be a tiny bit different character wise
        testCase.AddFailure("File sizes not equal : " + itrName + ext);
      }
    }

    testReport.SerializeToFile(sTestDirectory + "/" + testName + "Report.json");
  }
END_NAMESPACE_EX
