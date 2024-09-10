/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include <cstring>

#include "engine/CommonDefs.h"
#include "engine/PulseScenarioExec.h"

#include "cdm/utils/FileUtils.h"

#include "PatientIteration.h"
#include "TCCCIteration.h"
#include "ValidationIteration.h"

using namespace pulse::study::patient_variability;

int main(int argc, char* argv[])
{
  bool clear = false;// TODO
  bool generateOnly = false;
  std::string combinedPatientStatusFile;

  std::vector<PatientIteration*> iPatients;
  std::vector<ActionIteration*>  iActions;

  std::string mode = "test";
  // Process arguments
  if (argc > 1)
  {
    mode = argv[1];
    for (int i = 2; i < argc; ++i)
    {
      // Clear
      if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--clear"))
        clear = true;

      // Only generate
      if (!strcmp(argv[i], "-g") || !strcmp(argv[i], "--generate"))
        generateOnly = true;
    }
  }
  std::string rootDir = "./test_results/patient_variability/" + mode;
  if (clear)// Then we can delete the specific solo run results
    DeleteDirectory(rootDir);
  CreatePath(rootDir);

  Logger logger;
  logger.LogToConsole(true);
  logger.SetLogFile(rootDir + "/PatientVariability.log");

  if (mode == "validation" || mode == "hemorrhage" || mode == "itm")
  {
    if (mode == "validation")
    {
      combinedPatientStatusFile = rootDir + "/scenarios/" + "Validation.json";
      ValidationIteration* male = new ValidationIteration(logger);
      male->SetIterationName("male");
      male->SetScenarioExecListFilename(rootDir + "/scenarios/" + male->GetIterationName() + ".json");
      male->SetStateDirectory(rootDir + "/states/" + male->GetIterationName());
      male->SetResultsDirectory(rootDir + "/results/" + male->GetIterationName());
      male->SetGenStyle(eGenStyle::Combo);
      male->SetSex(ePatient_Sex::Male);
      male->GetAge_yr().SetValues({ minAge_yr, maxAge_yr, stdAge_yr }, 2);
      male->GetHR_bpm().SetValues({ minHR_bpm, maxHR_bpm, stdHR_bpm }, 2);
      male->GetMAP_mmHg().SetValues({ minMAP_mmHg, maxMAP_mmHg, stdMAP_mmHg }, 2);
      male->GetPP_mmHg().SetValues({ minPulsePressure_mmHg, maxPulsePressure_mmHg, stdPulsePressure_mmHg }, 2);
      male->GetRR_bpm().SetValues({ minRR_bpm, maxRR_bpm, stdRR_bpm }, 2);
      male->GetHeight_cm().SetValues({ minMaleHeight_cm, maxMaleHeight_cm, stdMaleHeight_cm }, 2);
      male->GetBMI().SetValues({ minBMI, maxBMI, stdMaleBMI }, 2);
      male->GetBFF().SetValues({ minMaleBFF, maxMaleBFF, stdMaleBFF }, 2);
      iPatients.push_back(male);

      ValidationIteration* female = new ValidationIteration(logger);
      female->SetIterationName("female");
      female->SetScenarioExecListFilename(rootDir + "/scenarios/" + female->GetIterationName() + ".json");
      female->SetStateDirectory(rootDir + "/states/" + female->GetIterationName());
      female->SetResultsDirectory(rootDir + "/results/" + female->GetIterationName());
      female->SetGenStyle(eGenStyle::Combo);
      female->SetSex(ePatient_Sex::Female);
      female->GetAge_yr().SetValues({ minAge_yr, maxAge_yr, stdAge_yr }, 2);
      female->GetHR_bpm().SetValues({ minHR_bpm, maxHR_bpm, stdHR_bpm }, 2);
      female->GetMAP_mmHg().SetValues({ minMAP_mmHg, maxMAP_mmHg, stdMAP_mmHg }, 2);
      female->GetPP_mmHg().SetValues({ minPulsePressure_mmHg, maxPulsePressure_mmHg, stdPulsePressure_mmHg }, 2);
      female->GetRR_bpm().SetValues({ minRR_bpm, maxRR_bpm, stdRR_bpm }, 2);
      female->GetHeight_cm().SetValues({ minFemaleHeight_cm, maxFemaleHeight_cm, stdFemaleHeight_cm }, 2);
      female->GetBMI().SetValues({ minBMI, maxBMI, stdFemaleBMI }, 2);
      female->GetBFF().SetValues({ minFemaleBFF, maxFemaleBFF, stdFemaleBFF }, 2);
      iPatients.push_back(female);
    }
    else if (mode == "hemorrhage")
    {
      PatientIteration* male = new PatientIteration(logger);
      male->SetIterationName("male");
      male->SetScenarioExecListFilename(rootDir + "/scenarios/" + male->GetIterationName() + ".json");
      male->SetStateDirectory(rootDir + "/states/" + male->GetIterationName());
      male->SetResultsDirectory(rootDir + "/results/" + male->GetIterationName());
      male->SetGenStyle(eGenStyle::Combo);
      male->SetSex(ePatient_Sex::Male);
      //male->GetAge_yr().SetValues({minAge_yr, maxAge_yr, stdAge_yr }, 2);
      //male->GetHR_bpm().SetValues({minHR_bpm, maxHR_bpm, stdHR_bpm }, 2);
      //male->GetMAP_mmHg().SetValues({ minMAP_mmHg, maxMAP_mmHg, stdMAP_mmHg }, 2);
      //male->GetPP_mmHg().SetValues({ minPulsePressure_mmHg, maxPulsePressure_mmHg, stdPulsePressure_mmHg }, 2);
      //male->GetRR_bpm().SetValues({ minRR_bpm, maxRR_bpm, stdRR_bpm }, 2);
      //male->GetHeight_cm().SetValues({ minMaleHeight_cm, maxMaleHeight_cm, stdMaleHeight_cm }, 2);
      //male->GetBMI().SetValues({ minBMI, maxBMI, stdMaleBMI }, 2);
      //male->GetBFF().SetValues({ minMaleBFF, maxMaleBFF, stdMaleBFF }, 2);
      iPatients.push_back(male);

      PatientIteration* female = new PatientIteration(logger);
      female->SetIterationName("female");
      female->SetScenarioExecListFilename(rootDir + "/scenarios/" + female->GetIterationName() + ".json");
      female->SetStateDirectory(rootDir + "/states/" + female->GetIterationName());
      female->SetResultsDirectory(rootDir + "/results/" + female->GetIterationName());
      female->SetGenStyle(eGenStyle::Combo);
      female->SetSex(ePatient_Sex::Female);
      //female->GetAge_yr().SetValues({ minAge_yr, maxAge_yr, stdAge_yr }, 2);
      //female->GetHR_bpm().SetValues({ minHR_bpm, maxHR_bpm, stdHR_bpm }, 2);
      //female->GetMAP_mmHg().SetValues({ minMAP_mmHg, maxMAP_mmHg, stdMAP_mmHg }, 2);
      //female->GetPP_mmHg().SetValues({ minPulsePressure_mmHg, maxPulsePressure_mmHg, stdPulsePressure_mmHg }, 2);
      //female->GetRR_bpm().SetValues({ minRR_bpm, maxRR_bpm, stdRR_bpm }, 2);
      //female->GetHeight_cm().SetValues({ minFemaleHeight_cm, maxFemaleHeight_cm, stdFemaleHeight_cm }, 2);
      //female->GetBMI().SetValues({ minBMI, maxBMI, stdFemaleBMI }, 2);
      //female->GetBFF().SetValues({ minFemaleBFF, maxFemaleBFF, stdFemaleBFF }, 2);
      iPatients.push_back(female);

      TCCCIteration* tccc = new TCCCIteration(logger);
      tccc->SetGenStyle(eGenStyle::Slice);
      tccc->SetBaselineDuration_s(15);
      tccc->SetMaxSimTime_min(60);
      tccc->PerformInterventions(false);
      tccc->GetHemorrhageSeverity().SetMinMaxStep(0.1, 1.0, 0.1);
      std::vector<size_t> hemorrhageWounds;
      for (size_t i = 0; i < (size_t)eHemorrhageWound::_COUNT; ++i)
        hemorrhageWounds.push_back(i);
      tccc->GetHemorrhageWound().SetValues(hemorrhageWounds);
      tccc->GetLeftHemothoraxSeverity().SetMinMaxStep(0.1, 1.0, 0.1);
      iActions.push_back(tccc);
    }
    else if (mode == "itm")
    {
      PatientIteration* male = new PatientIteration(logger);
      male->SetIterationName("male");
      male->SetScenarioExecListFilename(rootDir + "/scenarios/" + male->GetIterationName() + ".json");
      male->SetStateDirectory(rootDir + "/states/" + male->GetIterationName());
      male->SetResultsDirectory(rootDir + "/results/" + male->GetIterationName());
      male->SetGenStyle(eGenStyle::Combo);
      male->SetSex(ePatient_Sex::Male);
      iPatients.push_back(male);

      PatientIteration* female = new PatientIteration(logger);
      female->SetIterationName("female");
      female->SetScenarioExecListFilename(rootDir + "/scenarios/" + female->GetIterationName() + ".json");
      female->SetStateDirectory(rootDir + "/states/" + female->GetIterationName());
      female->SetResultsDirectory(rootDir + "/results/" + female->GetIterationName());
      female->SetGenStyle(eGenStyle::Combo);
      female->SetSex(ePatient_Sex::Female);
      iPatients.push_back(female);

      TCCCIteration* tccc = new TCCCIteration(logger);
      tccc->SetIterationName("tccc");
      tccc->SetBaselineDuration_s(15);
      tccc->SetMaxSimTime_min(60);
      tccc->PerformInterventions(false);
      tccc->GetAirwayObstructionSeverity().SetMinMaxStep(0., 1.0, 0.2);
      tccc->GetHemorrhageSeverity().SetMinMaxStep(0., 1.0, 0.2);
      std::vector<size_t> hemorrhageWounds;
      //for(size_t i = 0; i < (size_t)eHemorrhageWound::_COUNT; ++i)
      //  hemorrhageWounds.push_back(i);
      hemorrhageWounds.push_back((size_t)eHemorrhageWound::LeftLegLaceration);
      tccc->GetHemorrhageWound().SetValues(hemorrhageWounds);
      tccc->GetStressSeverity().SetMinMaxStep(0., 1.0, 0.3);
      tccc->GetTBISeverity().SetMinMaxStep(0., 1.0, 0.3);
      tccc->GetLeftTensionPneumothoraxSeverity().SetMinMaxStep(0., 1.0, 0.2);
      std::vector<size_t> tensionPneumothoraxWounds;
      //for(size_t i = 0; i < (size_t)eTensionPneumothoraxWound::_COUNT; ++i)
      //  tensionPneumothoraxWounds.push_back(i);
      tensionPneumothoraxWounds.push_back((size_t)eTensionPneumothoraxWound::Closed);
      tccc->GetLeftTensionPneumothoraxWound().SetValues(tensionPneumothoraxWounds);

      // What is our equipment variability?
      // Let's assume we have everything in our bag
      //tccc->GetInsultDuration_s().SetValues(5. * 60, 40. * 60, 5. * 60));
      //tccc->GetSalineAvailable().SetValues({ 1 });
      //tccc->GetNeedleAvailable().SetValues({ 1 });
      //tccc->GetChestWrapAvailable().SetValues({ 1 });
      iActions.push_back(tccc);
    }
  }
  else
  {
    combinedPatientStatusFile = rootDir + "/scenarios/" + "Validation.json";

    ValidationIteration* male = new ValidationIteration(logger);
    male->SetIterationName("default_male");
    male->SetScenarioExecListFilename(rootDir + "/scenarios/" + male->GetIterationName() + ".json");
    male->SetStateDirectory(rootDir + "/states/" + male->GetIterationName());
    male->SetResultsDirectory(rootDir + "/results/" + male->GetIterationName());
    iPatients.push_back(male);

    ValidationIteration* female = new ValidationIteration(logger);
    female->SetIterationName("default_female");
    female->SetGenStyle(eGenStyle::Combo);
    female->SetSex(ePatient_Sex::Female);
    female->SetScenarioExecListFilename(rootDir + "/scenarios/" + female->GetIterationName() + ".json");
    female->SetStateDirectory(rootDir + "/states/" + female->GetIterationName());
    female->SetResultsDirectory(rootDir + "/results/" + female->GetIterationName());
    iPatients.push_back(female);

    if (false)
    {
      TCCCIteration* tccc = new TCCCIteration(logger);
      tccc->SetIterationName("tccc");
      tccc->CreateStates(true);
      tccc->SetBaselineDuration_s(15);
      tccc->SetMaxSimTime_min(60);
      tccc->PerformInterventions(false);
      tccc->GetTBISeverity().SetValues({ 0.2, 0.5 });
      tccc->GetHemorrhageSeverity().SetValues({ 0.2, 0.5 });
      std::vector<size_t> hemorrhageWounds;
      //for(size_t i = 0; i < (size_t)eHemorrhageWound::_LOC_COUNT; ++i)
      //  hemorrhageWounds.push_back(i);
      hemorrhageWounds.push_back((size_t)eHemorrhageWound::LeftLegLaceration);
      tccc->GetHemorrhageWound().SetValues(hemorrhageWounds);
      tccc->GetInsultDuration_s().SetValues({ 5 });
      iActions.push_back(tccc);
    }
  }

  // Should Encapsulate this execution logic into a method
  {
    for (PatientIteration* pi : iPatients)
    {
      if (!clear && FileExists(pi->GetScenarioExecListFilename()))
        logger.Info("Using previously run scenario exec list file: " + pi->GetScenarioExecListFilename());
      else
        pi->GenerateScenarios();

      // Note, not setting the output directory on the opts
      // Our scenarios aleady have a relative (to the working dir) results csv location set,
      // If you provide an output dir, that relative location will be concatenated to the csv relative location

      if (!generateOnly)
      {
        PulseScenarioExec opts(&logger);
        opts.SetModelType(eModelType::HumanAdultWholeBody);
        opts.LogToConsole(eSwitch::Off);
        opts.SetScenarioExecListFilename(pi->GetScenarioExecListFilename());
        opts.Execute();
      }
    }
    if (!combinedPatientStatusFile.empty())
    {
      std::vector<SEScenarioExecStatus> allPatients;
      for (PatientIteration* pi : iPatients)
      {
        std::vector<SEScenarioExecStatus> execStatus;
        SEScenarioExecStatus::SerializeFromFile(pi->GetScenarioExecListFilename(), execStatus, &logger);
        for (const SEScenarioExecStatus& s : execStatus)
          allPatients.push_back(s);
      }
      SEScenarioExecStatus::SerializeToFile(allPatients, combinedPatientStatusFile, &logger);
      logger.Info("Writing all patient status to file: " + combinedPatientStatusFile);
    }

    for (ActionIteration* ai : iActions)
    {
      ai->SetScenarioExecListFilename(rootDir + "/scenarios/" + ai->GetIterationName() + ".json");
      ai->SetStateDirectory(rootDir + "/states/" + ai->GetIterationName());
      ai->SetResultsDirectory(rootDir + "/results/" + ai->GetIterationName());

      if (!clear && FileExists(ai->GetScenarioExecListFilename()))
        logger.Info("Using previously run scenario exec list file: " + ai->GetScenarioExecListFilename());
      else
      {
        for (PatientIteration* pi : iPatients)
          ai->GenerateScenarios(*pi);
      }

      if (!generateOnly)
      {
        PulseScenarioExec opts(&logger);
        opts.SetModelType(eModelType::HumanAdultWholeBody);
        opts.LogToConsole(eSwitch::Off);
        opts.SetScenarioExecListFilename(ai->GetScenarioExecListFilename());
        opts.Execute();
      }
    }
  }

  SAFE_DELETE_VECTOR(iPatients);
  SAFE_DELETE_VECTOR(iActions);
  return 0;
}
