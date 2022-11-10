/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/human_adult/whole_body/test/EngineTest.h"
#include "engine/human_adult/whole_body/Engine.h"
#include "PulseEnginePool.h"

#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/patient/SEPatient.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/utils/TimingProfile.h"
#include "cdm/utils/testing/SETestReport.h"
#include "cdm/utils/testing/SETestCase.h"
#include "cdm/utils/testing/SETestSuite.h"

namespace pulse { namespace human_adult_whole_body
{
  void EngineTest::AddEngineGroupTest(SETestSuite& testSuite)
  {
    testSuite.GetLogger()->Info("Running AddEngineGroupTest");

    PulseEnginePool pool(/*poolSize*/ 5, /*datadir*/ "./", m_Logger);
    pool.GetLogger()->LogToConsole(false);

    pool.Info("Creating a pool with " + std::to_string(pool.GetWorkerCount()) + " workers");

    std::vector<SETestCase*> testCases;
    std::vector<SEEngineInitialization*> eis;
    std::vector<bool> expectedCreation;

    testCases.push_back(&testSuite.CreateTestCase());
    testCases.back()->SetName("Patient File");
    eis.push_back(new SEEngineInitialization(pool.GetLogger()));
    eis.back()->KeepLogMessages(true);
    eis.back()->GetPatientConfiguration().SetPatientFile("./patients/StandardMale.json");
    expectedCreation.push_back(true);

    testCases.push_back(&testSuite.CreateTestCase());
    testCases.back()->SetName("Inline Patient");
    eis.push_back(new SEEngineInitialization(pool.GetLogger()));
    eis.back()->KeepLogMessages(true);
    eis.back()->GetPatientConfiguration().GetPatient().SetSex(ePatient_Sex::Female);
    expectedCreation.push_back(true);

    testCases.push_back(&testSuite.CreateTestCase());
    testCases.back()->SetName("State File");
    eis.push_back(new SEEngineInitialization(pool.GetLogger()));
    eis.back()->KeepLogMessages(true);
    eis.back()->SetStateFilename("./states/StandardMale@0s.json");
    expectedCreation.push_back(true);

    testCases.push_back(&testSuite.CreateTestCase());
    testCases.back()->SetName("Bad Patient File");
    eis.push_back(new SEEngineInitialization(pool.GetLogger()));
    eis.back()->KeepLogMessages(true);
    eis.back()->GetPatientConfiguration().SetPatientFile("./patients/DoesNotExist.json");
    expectedCreation.push_back(false);

    testCases.push_back(&testSuite.CreateTestCase());
    testCases.back()->SetName("Bad Inline Patient");
    eis.push_back(new SEEngineInitialization(pool.GetLogger()));
    eis.back()->KeepLogMessages(true);
    SEPatient& p = eis.back()->GetPatientConfiguration().GetPatient();
    p.SetSex(ePatient_Sex::Female);
    p.GetAge().SetValue(14, TimeUnit::yr);
    expectedCreation.push_back(false);

    testCases.push_back(&testSuite.CreateTestCase());
    testCases.back()->SetName("Bad State File");
    eis.push_back(new SEEngineInitialization(pool.GetLogger()));
    eis.back()->KeepLogMessages(true);
    eis.back()->SetStateFilename("./states/DoesNotExist@0s.json");
    expectedCreation.push_back(false);

    testCases.push_back(&testSuite.CreateTestCase());
    testCases.back()->SetName("nullptr Initialization");
    eis.push_back(nullptr);
    expectedCreation.push_back(false);

    std::vector<SEEngineInitializationStatus*> statuses = pool.InitializeEngines(eis, eModelType::HumanAdultWholeBody);

    for (unsigned int idx = 0; idx < statuses.size(); ++idx)
    {
      if (statuses[idx]->Created() != expectedCreation[idx])
        testCases[idx]->AddFailure("Unexpected engine creation result");
    }*/

    for (auto e: eis)
      delete e;
  }

  void EngineTest::AddEnginesTest(SETestSuite& testSuite)
  {
    TimingProfile timer;
    testSuite.GetLogger()->Info("Running AddEnginesTest");

    PulseEnginePool pool(/*poolSize*/ 5, /*datadir*/ "./", m_Logger);
    pool.GetLogger()->LogToConsole(false);

    pool.Info("Creating a pool with " + std::to_string(pool.GetWorkerCount()) + " workers");

    timer.Start("Test");
    SETestCase& testCase1 = testSuite.CreateTestCase();
    testCase1.SetName("Patient File");
    SEEngineInitialization ei = SEEngineInitialization(pool.GetLogger());
    ei.KeepLogMessages(true);
    ei.GetPatientConfiguration().SetPatientFile("./patients/StandardMale.json");
    bool expectedCreation = true;
    SEEngineInitializationStatus& status1 = pool.InitializeEngine(ei, eModelType::HumanAdultWholeBody);
    if (status1.Created() != expectedCreation)
      testCase1.AddFailure("Unexpected engine creation result");
    testCase1.GetDuration().SetValue(timer.GetElapsedTime_s("Test"), TimeUnit::s);

    timer.Start("Test");
    SETestCase& testCase2 = testSuite.CreateTestCase();
    testCase2.SetName("Inline Patient");
    ei = SEEngineInitialization(pool.GetLogger());
    ei.KeepLogMessages(true);
    ei.GetPatientConfiguration().GetPatient().SetSex(ePatient_Sex::Female);
    expectedCreation = true;
    SEEngineInitializationStatus& status2 = pool.InitializeEngine(ei, eModelType::HumanAdultWholeBody);
    if (status2.Created() != expectedCreation)
      testCase2.AddFailure("Unexpected engine creation result");
    testCase2.GetDuration().SetValue(timer.GetElapsedTime_s("Test"), TimeUnit::s);

    timer.Start("Test");
    SETestCase& testCase3 = testSuite.CreateTestCase();
    testCase3.SetName("State File");
    ei = SEEngineInitialization(pool.GetLogger());
    ei.KeepLogMessages(true);
    ei.SetStateFilename("./states/StandardMale@0s.json");
    expectedCreation = true;
    SEEngineInitializationStatus& status3 = pool.InitializeEngine(ei, eModelType::HumanAdultWholeBody);
    if (status3.Created() != expectedCreation)
      testCase3.AddFailure("Unexpected engine creation result");
    testCase3.GetDuration().SetValue(timer.GetElapsedTime_s("Test"), TimeUnit::s);

    timer.Start("Test");
    SETestCase& testCase4 = testSuite.CreateTestCase();
    testCase4.SetName("Bad Patient File");
    ei = SEEngineInitialization(pool.GetLogger());
    ei.KeepLogMessages(true);
    ei.GetPatientConfiguration().SetPatientFile("./patients/DoesNotExist.json");
    expectedCreation = false;
    SEEngineInitializationStatus& status4 = pool.InitializeEngine(ei, eModelType::HumanAdultWholeBody);
    if (status4.Created() != expectedCreation)
      testCase4.AddFailure("Unexpected engine creation result");
    testCase4.GetDuration().SetValue(timer.GetElapsedTime_s("Test"), TimeUnit::s);

    timer.Start("Test");
    SETestCase& testCase5 = testSuite.CreateTestCase();
    testCase5.SetName("Bad Inline Patient");
    ei = SEEngineInitialization(pool.GetLogger());
    ei.KeepLogMessages(true);
    SEPatient& p = ei.GetPatientConfiguration().GetPatient();
    p.SetSex(ePatient_Sex::Female);
    p.GetAge().SetValue(14, TimeUnit::yr);
    expectedCreation = false;
    SEEngineInitializationStatus& status5 = pool.InitializeEngine(ei, eModelType::HumanAdultWholeBody);
    if (status5.Created() != expectedCreation)
      testCase5.AddFailure("Unexpected engine creation result");
    testCase5.GetDuration().SetValue(timer.GetElapsedTime_s("Test"), TimeUnit::s);

    timer.Start("Test");
    SETestCase& testCase6 = testSuite.CreateTestCase();
    testCase6.SetName("Bad State File");
    ei = SEEngineInitialization(pool.GetLogger());
    ei.KeepLogMessages(true);
    ei.SetStateFilename("./states/DoesNotExist@0s.json");
    expectedCreation = false;
    SEEngineInitializationStatus& status6 = pool.InitializeEngine(ei, eModelType::HumanAdultWholeBody);
    if (status6.Created() != expectedCreation)
      testCase6.AddFailure("Unexpected engine creation result");
    testCase6.GetDuration().SetValue(timer.GetElapsedTime_s("Test"), TimeUnit::s);
  }

  void EngineTest::RemoveEnginesTest(SETestSuite& testSuite)
  {
    TimingProfile timer;
    testSuite.GetLogger()->Info("Running AddEnginesTest");

    PulseEnginePool pool(/*poolSize*/ 5, /*datadir*/ "./", m_Logger);
    pool.GetLogger()->LogToConsole(false);

    std::vector<SEEngineInitialization*> eis;

    eis.push_back(new SEEngineInitialization(pool.GetLogger()));
    eis.back()->KeepLogMessages(true);
    eis.back()->SetStateFilename("./states/StandardMale@0s.json");

    eis.push_back(new SEEngineInitialization(pool.GetLogger()));
    eis.back()->KeepLogMessages(true);
    eis.back()->SetStateFilename("./states/StandardMale@0s.json");

    eis.push_back(new SEEngineInitialization(pool.GetLogger()));
    eis.back()->KeepLogMessages(true);
    eis.back()->SetStateFilename("./states/DoesNotExist@0s.json");

    std::vector<SEEngineInitializationStatus*> statuses = pool.InitializeEngines(eis, eModelType::HumanAdultWholeBody);

    std::vector<int> ids;
    for (auto status: statuses)
      ids.push_back(status->GetID());

    int maxID = *std::max_element(ids.begin(), ids.end());
    size_t expectedNumEngines = ids.size();

    timer.Start("Test");
    SETestCase& testCase1 = testSuite.CreateTestCase();
    testCase1.SetName("Remove Valid Engine");
    bool removed = pool.RemoveEngine(ids[0]);
    bool expectedRemoval = true;
    expectedNumEngines -= expectedRemoval;
    if (removed != expectedRemoval)
      testCase1.AddFailure("Unexpected engine removal result");
    if (expectedNumEngines != pool.GetEngineCount())
      testCase1.AddFailure("Unexpected number of engines in pool");
    testCase1.GetDuration().SetValue(timer.GetElapsedTime_s("Test"), TimeUnit::s);

    timer.Start("Test");
    SETestCase& testCase2 = testSuite.CreateTestCase();
    testCase2.SetName("Remove Non-Existant Engine");
    removed = pool.RemoveEngine(maxID + 1);
    expectedRemoval = false;
    expectedNumEngines -= expectedRemoval;
    if (removed != expectedRemoval)
      testCase2.AddFailure("Unexpected engine removal result");
    if (expectedNumEngines != pool.GetEngineCount())
      testCase2.AddFailure("Unexpected number of engines in pool");
    testCase2.GetDuration().SetValue(timer.GetElapsedTime_s("Test"), TimeUnit::s);

    // Add engine after a removal
    timer.Start("Test");
    SETestCase& testCase3 = testSuite.CreateTestCase();
    testCase3.SetName("Add Engine After a Removal");
    SEEngineInitialization ei = SEEngineInitialization(pool.GetLogger());
    ei.KeepLogMessages(true);
    ei.SetStateFilename("./states/StandardMale@0s.json");
    bool expectedAdd = true;
    SEEngineInitializationStatus& status = pool.InitializeEngine(ei, eModelType::HumanAdultWholeBody);
    expectedNumEngines += status.Created();
    if (status.Created() != expectedAdd)
      testCase3.AddFailure("Failed to create engine after removal of an engine");
    if (expectedNumEngines != pool.GetEngineCount())
      testCase3.AddFailure("Unexpected number of engines in pool");
    testCase3.GetDuration().SetValue(timer.GetElapsedTime_s("Test"), TimeUnit::s);

    timer.Start("Test");
    SETestCase& testCase4 = testSuite.CreateTestCase();
    testCase4.SetName("Remove Engine Twice");
    removed = pool.RemoveEngine(ids[0]);
    expectedRemoval = false;
    expectedNumEngines -= expectedRemoval;
    if (removed != expectedRemoval)
      testCase4.AddFailure("Unexpected engine removal result");
    if (expectedNumEngines != pool.GetEngineCount())
      testCase4.AddFailure("Unexpected number of engines in pool");
    testCase4.GetDuration().SetValue(timer.GetElapsedTime_s("Test"), TimeUnit::s);

    timer.Start("Test");
    SETestCase& testCase5 = testSuite.CreateTestCase();
    testCase5.SetName("Remove Failed Engine");
    removed = pool.RemoveEngine(ids[2]);
    expectedRemoval = true;
    expectedNumEngines -= expectedRemoval;
    if (removed != expectedRemoval)
      testCase5.AddFailure("Unexpected engine removal result");
    if (expectedNumEngines != pool.GetEngineCount())
      testCase5.AddFailure("Unexpected number of engines in pool");
    testCase5.GetDuration().SetValue(timer.GetElapsedTime_s("Test"), TimeUnit::s);

    for (auto e: eis)
      delete e;
  }
  
  void EngineTest::AddRemoveEnginesTests(const std::string& rptDirectory)
  {
    m_Logger->SetLogFile(rptDirectory + "/EnginePoolAddRemoveEnginesTests.log");

    SETestReport testReport = SETestReport(m_Logger);

    SETestSuite& tsEngineGroupCreation = testReport.CreateTestSuite();
    tsEngineGroupCreation.SetName("EngineGroupCreation");
    AddEngineGroupTest(tsEngineGroupCreation);

    SETestSuite& tsEngineCreation = testReport.CreateTestSuite();
    tsEngineCreation.SetName("EngineCreation");
    AddEnginesTest(tsEngineCreation);

    SETestSuite& tsEngineRemoval = testReport.CreateTestSuite();
    tsEngineRemoval.SetName("EngineRemoval");
    RemoveEnginesTest(tsEngineRemoval);

    testReport.SerializeToFile(rptDirectory + "/EnginePoolAddRemoveEnginesTests.json");
  }
END_NAMESPACE_EX