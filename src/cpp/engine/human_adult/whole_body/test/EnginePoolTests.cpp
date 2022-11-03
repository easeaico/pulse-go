/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/human_adult/whole_body/test/EngineTest.h"
#include "engine/human_adult/whole_body/Engine.h"
#include "PulseEnginePool.h"

#include "cdm/utils/TimingProfile.h"
#include "cdm/utils/testing/SETestReport.h"
#include "cdm/utils/testing/SETestCase.h"
#include "cdm/utils/testing/SETestSuite.h"

namespace pulse { namespace human_adult_whole_body
{
  void EngineTest::AddEnginesTest(SETestSuite& testSuite)
  {
    TimingProfile timer;
    testSuite.GetLogger()->Info("Running AddEnginesTest");

    PulseEnginePool pool(/*poolSize*/ 5, /*datadir*/ "./", m_Logger);
    pool.GetLogger()->LogToConsole(false);

    pool.Info("Creating a pool with " + std::to_string(pool.GetWorkerCount()) + " workers");

    std::vector<SETestCase*> testCases;
    std::vector<SEEngineInitialization> eis;

    testCases.push_back(&testSuite.CreateTestCase());
    testCases.back()->SetName("patient0");
    eis.push_back(SEEngineInitialization(pool.GetLogger()));
    eis.back().SetStateFilename("./states/StandardMale@0s.json");

    testCases.push_back(&testSuite.CreateTestCase());
    testCases.back()->SetName("patient1");
    eis.push_back(SEEngineInitialization(pool.GetLogger()));
    eis.back().SetStateFilename("./states/StandardMale@0s.json");

    std::vector<SEEngineInitializationStatus> statuses = pool.InitializeEngines(eis, eModelType::HumanAdultWholeBody);

    /*for (unsigned int idx = 0; idx < statuses.size(); ++idx)
    {
      if (!(statuses[idx].Created()))
      {
        testCases[idx]->AddFailure("Failed to create engine at index " + std::to_string(idx));
      }
    }*/
  }
  
  void EngineTest::AddRemoveEnginesTests(const std::string& rptDirectory)
  {
    m_Logger->SetLogFile(rptDirectory + "/EnginePoolAddRemoveEnginesTests.log");

    SETestReport testReport = SETestReport(m_Logger);

    SETestSuite& tsEngineCreation = testReport.CreateTestSuite();
    tsEngineCreation.SetName("EngineCreation");
    AddEnginesTest(tsEngineCreation);

    testReport.SerializeToFile(rptDirectory + "/EnginePoolAddRemoveEnginesTests.json");
  }
END_NAMESPACE_EX