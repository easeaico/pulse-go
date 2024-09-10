/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "PVGenerator.h"
#include "cdm/patient/SEPatient.h"
#include "cdm/patient/actions/SEHemorrhage.h"
#include "cdm/io/protobuf/PBPatient.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarLength.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerPressure.h"
#include "engine/human_adult/whole_body/Engine.h"

#include "cdm/scenario/SEScenario.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalarTemperature.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEAdvanceTime.h"

#include <map>

using eFailure = pulse::study::bind::patient_variability::PatientStateData_eFailure;

// Bounds from: https://pulse.kitware.com/_patient_methodology.html
// And: https://pulse.kitware.com/_cardiovascular_methodology.html#cardiovascular-validation-resting

#define minAge_yr 18
#define maxAge_yr 65
#define minMaleHeight_cm 163
#define maxMaleHeight_cm 190
#define minFemaleHeight_cm 151
#define maxFemaleHeight_cm 175.5
#define minBMI 16
#define maxBMI 29.9
#define minMaleBFF 0.02
#define maxMaleBFF 0.25
#define minFemaleBFF 0.1
#define maxFemaleBFF 0.32
#define minHR_bpm 60
#define maxHR_bpm 100
#define minMAP_mmHg 70
#define maxMAP_mmHg 100
#define minPulsePressure_mmHg 30
#define maxPulsePressure_mmHg 60
#define minRR_bpm 8
#define maxRR_bpm 20

namespace pulse::study::patient_variability
{
  // Bounds from: https://pulse.kitware.com/_patient_methodology.html
  PVGenerator::PVGenerator(Logger* logger) : Loggable(logger),
    HemorrhageSeverity(0.1, 0.3, 0.1),
    HemorrhageTriageTime_min(1.0, 1.0)
  {
  }
  PVGenerator::~PVGenerator()
  {
  }

  // Reset ParameterSpace
  void PVGenerator::ResetParameters(ePatient_Sex sex)
  {
    // The third number is generally a compromise between standard and default
    Age_yr.Set(minAge_yr, maxAge_yr, 44.);
    HR_bpm.Set(minHR_bpm, maxHR_bpm, 72);
    MAP_mmHg.Set(minMAP_mmHg, maxMAP_mmHg, 87);
    PP_mmHg.Set(minPulsePressure_mmHg, maxPulsePressure_mmHg, 40.5);
    RR_bpm.Set(minRR_bpm, maxRR_bpm, 12);
    if (sex == ePatient_Sex::Male)
    {
      Height_cm.Set(minMaleHeight_cm, maxMaleHeight_cm, 180.34);
      BMI.Set(minBMI, maxBMI, 23.71);
      BFF.Set(minMaleBFF, maxMaleBFF, 0.21);
    }
    else
    {
      Height_cm.Set(minFemaleHeight_cm, maxFemaleHeight_cm, 162.56);
      BMI.Set(minBMI, maxBMI, 22.31);
      BFF.Set(minFemaleBFF, maxFemaleBFF, 0.28);
    }
  }

  void PVGenerator::GenerateData(eSetType t, PatientStateListData& pList)
  {
    m_TotalRuns = 0;
    m_Duplicates = 0;
    m_TotalPatients = 0;
    m_NumPatientsFailedToSetup = 0;
    m_SetType = t;

    if(t == eSetType::Both || t == eSetType::Combo)
      GenerateMultiVariableCombinationPatientList(pList);

    if (t == eSetType::Both || t == eSetType::Slice)
    {
      // Explicitly setting these:
      // We are using different parameter combinations than whats in json
      // The units we use in analysis are also different
      // And those tools are not unit converting

      SEPatient standardMale(GetLogger());
      standardMale.GetAge().SetValue(44, TimeUnit::yr);
      standardMale.GetHeight().SetValue(180.34, LengthUnit::cm);
      standardMale.GetBodyMassIndex().SetValue(23.71);
      standardMale.GetBodyFatFraction().SetValue(0.21);
      standardMale.GetHeartRateBaseline().SetValue(72, FrequencyUnit::Per_min);
      standardMale.GetMeanArterialPressureBaseline().SetValue(87, PressureUnit::mmHg);
      standardMale.GetPulsePressureBaseline().SetValue(40.5, PressureUnit::mmHg);
      standardMale.GetRespirationRateBaseline().SetValue(12, FrequencyUnit::Per_min);
      GenerateSlicedPatientList(pList, standardMale);

      SEPatient standardFemale(GetLogger());
      standardFemale.SetSex(ePatient_Sex::Female);
      standardFemale.GetAge().SetValue(44, TimeUnit::yr);
      standardFemale.GetHeight().SetValue(162.56, LengthUnit::cm);
      standardFemale.GetBodyMassIndex().SetValue(22.31);
      standardFemale.GetBodyFatFraction().SetValue(0.28);
      standardFemale.GetHeartRateBaseline().SetValue(72, FrequencyUnit::Per_min);
      standardFemale.GetMeanArterialPressureBaseline().SetValue(87, PressureUnit::mmHg);
      standardFemale.GetPulsePressureBaseline().SetValue(40.5, PressureUnit::mmHg);
      standardFemale.GetRespirationRateBaseline().SetValue(12, FrequencyUnit::Per_min);
      GenerateSlicedPatientList(pList, standardFemale);
    }

    Info("Removed " + std::to_string(m_Duplicates) + " duplicates");
    Info("Created " + std::to_string(m_TotalPatients) + " patients");
    Info("Created " + std::to_string(m_TotalRuns) + " total runs");
  }

  void PVGenerator::GenerateSlicedPatientList(PatientStateListData& pList, const SEPatient& basePatient)
  {
    Info("Generating Age slice data set for "+basePatient.GetName() + " Starting at " + std::to_string(m_TotalPatients));
    for (size_t age_yr = minAge_yr; age_yr <= maxAge_yr; age_yr++)
    {
      SEPatient patient(GetLogger());
      patient.Copy(basePatient);
      patient.GetAge().SetValue((double)age_yr, TimeUnit::yr);
      CreatePatient(pList, patient, "/Patient" + std::to_string(m_TotalPatients));
    }

    Info("Generating Height slice data set for " + basePatient.GetName() + " Starting at " + std::to_string(m_TotalPatients));
    double minHeight_cm = basePatient.GetSex() == ePatient_Sex::Male ? minMaleHeight_cm : minFemaleHeight_cm;
    double maxHeight_cm = basePatient.GetSex() == ePatient_Sex::Male ? maxMaleHeight_cm : maxFemaleHeight_cm;
    for (double height_cm = minHeight_cm; height_cm <= maxHeight_cm; height_cm += 0.5)
    {
      SEPatient patient(GetLogger());
      patient.Copy(basePatient);
      patient.GetHeight().SetValue(height_cm, LengthUnit::cm);
      CreatePatient(pList, patient, "/Patient" + std::to_string(m_TotalPatients));
    }

    Info("Generating BMI slice data set for " + basePatient.GetName() + " Starting at " + std::to_string(m_TotalPatients));
    for (double bmi = minBMI; bmi <= maxBMI; bmi += 0.5)
    {
      SEPatient patient(GetLogger());
      patient.Copy(basePatient);
      patient.GetBodyMassIndex().SetValue(bmi);
      CreatePatient(pList, patient, "/Patient" + std::to_string(m_TotalPatients));
    }

    Info("Generating BFF slice data set for " + basePatient.GetName() + " Starting at " + std::to_string(m_TotalPatients));
    double minBFF = basePatient.GetSex() == ePatient_Sex::Male ? minMaleBFF : minFemaleBFF;
    double maxBFF = basePatient.GetSex() == ePatient_Sex::Male ? maxMaleBFF : maxFemaleBFF;
    for (double bff = minBFF; bff <= maxBFF; bff += 0.01)
    {
      SEPatient patient(GetLogger());
      patient.Copy(basePatient);
      patient.GetBodyFatFraction().SetValue(bff);
      CreatePatient(pList, patient, "/Patient" + std::to_string(m_TotalPatients));
    }

    Info("Generating HR slice data set for " + basePatient.GetName() + " Starting at " + std::to_string(m_TotalPatients));
    for (size_t hr_bpm = minHR_bpm; hr_bpm <= maxHR_bpm; hr_bpm++)
    {
      SEPatient patient(GetLogger());
      patient.Copy(basePatient);
      patient.GetHeartRateBaseline().SetValue((double)hr_bpm, FrequencyUnit::Per_min);
      CreatePatient(pList, patient, "/Patient" + std::to_string(m_TotalPatients));
    }

    Info("Generating MAP/PP combo slice data set for " + basePatient.GetName() + " Starting at " + std::to_string(m_TotalPatients));
    // Iterating MAP and Pulse Pressure by themselves don't effectivly cover the variability we would like
    // They are dependent on each other and Systolic/Diastolic, so to get a good coverage,
    // we are going to combo these two values and toss invalid patients.
    // This gives us a pretty good coverage of variabile cv pressures
    Logger null;// I don't want to log the SetupPatient failures
    null.LogToConsole(false);
    //size_t cnt = 0, bad = 0;
    for (size_t map = minMAP_mmHg; map <= maxMAP_mmHg; map++)
    {
      for (size_t pp_mmHg = minPulsePressure_mmHg; pp_mmHg <= maxPulsePressure_mmHg; pp_mmHg++)
      {
        SEPatient patient(&null);
        patient.Copy(basePatient);
        patient.GetSystolicArterialPressureBaseline().Invalidate();
        patient.GetDiastolicArterialPressureBaseline().Invalidate();
        patient.GetMeanArterialPressureBaseline().SetValue((double)map, PressureUnit::mmHg);
        patient.GetPulsePressureBaseline().SetValue((double)pp_mmHg, PressureUnit::mmHg);
        CreatePatient(pList, patient, "/Patient" + std::to_string(m_TotalPatients));

        // Check that this combo is valid
        //cnt++;
        //SEPatient setupCheck(&null);
        //setupCheck.Copy(patient);// Make a copy as the patient cannot be setup twice (maybe that will change in the future)
        //if (!pulse::human_adult_whole_body::SetupPatient(setupCheck))
        //{
        //  std::cout << "VALID " << patient.GetName() << " MAP: " << map << " PP: " << pp_mmHg << " -> " << setupCheck.GetSystolicArterialPressureBaseline() << "/" << setupCheck.GetDiastolicArterialPressureBaseline() << std::endl;
        //  bad++;
        //}
        //else
        //  std::cout << "INVALID " << patient.GetName() << " MAP: " << map << " PP: " << pp_mmHg << " -> " << setupCheck.GetSystolicArterialPressureBaseline() << "/" << setupCheck.GetDiastolicArterialPressureBaseline() << std::endl;
      }
    }

    /*
    for (size_t map = minMAP_mmHg; map <= maxMAP_mmHg; map++)
    {
      SEPatient patient(GetLogger());
      patient.Copy(basePatient);
      patient.GetSystolicArterialPressureBaseline().Invalidate();
      patient.GetDiastolicArterialPressureBaseline().Invalidate();
      patient.GetMeanArterialPressureBaseline().SetValue(map, PressureUnit::mmHg);
      std::cout << "MAP: " << map;
      //if (pulse::human_adult_whole_body::SetupPatient(patient))
      //  std::cout << " -> " << patient.GetPulsePressureBaseline() << " " << patient.GetSystolicArterialPressureBaseline() << "/" << patient.GetDiastolicArterialPressureBaseline() << std::endl;
      //else
      //  std::cout << "INVALID" << std::endl;
      CreatePatient(pList, patient, "/Patient" + std::to_string(m_TotalPatients));
    }

    for (size_t pp_mmHg = minPulsePressure_mmHg; pp_mmHg <= maxPulsePressure_mmHg; pp_mmHg++)
    {
      SEPatient patient(GetLogger());
      patient.Copy(basePatient);

      //patient.GetDiastolicArterialPressureBaseline().SetValue(60, PressureUnit::mmHg);
      patient.GetDiastolicArterialPressureBaseline().Invalidate();
      patient.GetSystolicArterialPressureBaseline().Invalidate();
      patient.GetPulsePressureBaseline().SetValue(pp_mmHg, PressureUnit::mmHg);
      //std::cout << "Pulse Pressure: " << pp_mmHg;
      //if (pulse::human_adult_whole_body::SetupPatient(patient))
      //  std::cout << " -> " << patient.GetMeanArterialPressureBaseline() << " " << patient.GetSystolicArterialPressureBaseline() << "/" << patient.GetDiastolicArterialPressureBaseline() << std::endl;
      //else
      //  std::cout << "INVALID" << std::endl;
      CreatePatient(pList, patient, "/Patient" + std::to_string(m_TotalPatients));
    }
    */

    Info("Generating RR slice data set for " + basePatient.GetName() + " Starting at " + std::to_string(m_TotalPatients));
    for (double rr = minRR_bpm; rr <= maxRR_bpm; rr += 0.5)
    {
      SEPatient patient(GetLogger());
      patient.Copy(basePatient);
      patient.GetRespirationRateBaseline().SetValue(rr, FrequencyUnit::Per_min);
      CreatePatient(pList, patient, "/Patient" + std::to_string(m_TotalPatients));
    }
  }

  void PVGenerator::GenerateMultiVariableCombinationPatientList(PatientStateListData& pList)
  {
    Info("Generating combinatorial data set");
    // Each parameter we want to include in our permutations, has 3 options, min, max, standard/default
    std::vector<size_t> opts = { 2,2,2,2,2,2,2,2 }; // max index is 2
    std::vector<std::vector<size_t>> permutations;
    GeneralMath::Combinations(opts, permutations);

    std::vector<ePatient_Sex> sexes = { ePatient_Sex::Male, ePatient_Sex::Female };
    for (auto sex : sexes)
    {
      ResetParameters(sex);
      for (auto idxs : permutations)
      {
        SEPatient patient(GetLogger());
        patient.SetSex(sex);

        patient.GetAge().SetValue(Age_yr.Values()[idxs[0]], TimeUnit::yr);
        patient.GetHeight().SetValue(Height_cm.Values()[idxs[1]], LengthUnit::cm);
        patient.GetBodyFatFraction().SetValue(BFF.Values()[idxs[2]]);
        patient.GetBodyMassIndex().SetValue(BMI.Values()[idxs[3]]);
        patient.GetHeartRateBaseline().SetValue(HR_bpm.Values()[idxs[4]], FrequencyUnit::Per_min);
        patient.GetMeanArterialPressureBaseline().SetValue(MAP_mmHg.Values()[idxs[5]], PressureUnit::mmHg);
        patient.GetPulsePressureBaseline().SetValue(PP_mmHg.Values()[idxs[6]], PressureUnit::mmHg);
        patient.GetRespirationRateBaseline().SetValue(RR_bpm.Values()[idxs[7]], FrequencyUnit::Per_min);

        CreatePatient(pList, patient, "/Patient" + std::to_string(m_TotalPatients));
      }
    }
  }

  void PVGenerator::CreatePatient(PatientStateListData& pList, SEPatient& patient, const std::string& full_dir_path)
  {
    std::string pstr = ToString(patient);
    if (m_PatientSet.find(pstr) != m_PatientSet.end())
    {
      Info("Ignoring duplicate: " + pstr);
      m_Duplicates++;
      return;
    }
    m_PatientSet.insert(pstr);

    patient.SetName("Patient" + std::to_string(m_TotalPatients));
    m_TotalPatients++;
    switch (GenerateMode)
    {
    case eMode::Validation:
    {
      AddPatientToList(pList, patient, 120, full_dir_path);
      break;
    }

    case eMode::Hemorrhage:
    {
      GenerateHemorrhageOptions(pList, patient, full_dir_path);
      break;
    }
    }
  }

  // Creates a new list of patients, adding all hemorrhage variables to each patient in originalPatients
  void PVGenerator::GenerateHemorrhageOptions(PatientStateListData& pList, SEPatient& patient, const std::string& full_dir_path)
  {
    std::vector<CDM_BIND::HemorrhageData_eCompartment> HemorrhageCompartments =
    { CDM_BIND::HemorrhageData_eCompartment_RightArm, CDM_BIND::HemorrhageData_eCompartment_RightLeg };
    for (auto hemorrhageCompartment : HemorrhageCompartments)
    {
      std::string compartment_dir = +"/" + hemorrhageCompartment;

      for (auto severity : HemorrhageSeverity.Values())
      {
        std::string severity_dir = "/severity" + pulse::cdm::to_string(severity);

        for (auto triageTime_min : HemorrhageTriageTime_min.Values())
        {

          std::string triageTime_dir = "/triage_min" + pulse::cdm::to_string(triageTime_min);
          std::string hemorrhage_dir_path = full_dir_path + compartment_dir + severity_dir + triageTime_dir;

          auto patientData = AddPatientToList(pList, patient, 2 * 60 * 60, hemorrhage_dir_path);

          auto hemorrhage = patientData->mutable_hemorrhage();
          hemorrhage->set_starttime_s(10);
          hemorrhage->mutable_action()->set_compartment(hemorrhageCompartment);
          hemorrhage->mutable_action()->mutable_severity()->mutable_scalar0to1()->set_value(severity);
          hemorrhage->set_triagetime_s(triageTime_min * 60);
        }
      }
    }
  }

  pulse::study::bind::patient_variability::PatientStateData* PVGenerator::AddPatientToList(PatientStateListData& pList,
    SEPatient& patient, double runDuration_s, const std::string& full_dir_path)
  {
    Info("Creating patient: " + full_dir_path);
    //Info("  " + ToString(patient));
    pulse::study::bind::patient_variability::PatientStateData* patientStateData = pList.add_patientstate();
    patientStateData->set_id(m_TotalRuns);
    patientStateData->set_outputbasefilename(full_dir_path);
    patientStateData->set_maxsimulationtime_s(runDuration_s); // Generate 2 mins of data
    patientStateData->mutable_validation();// Create a validation object to fill
    pulse::cdm::bind::PatientData* patientData = patientStateData->mutable_patient();
    PBPatient::Serialize(patient, *patientData);
    m_TotalRuns++;

    return patientStateData;
  }

  std::string PVGenerator::ToString(SEPatient& p)
  {
    std::string out;
    out = ePatient_Sex_Name(p.GetSex()) + "_";
    out += p.GetAge().ToString() + "_";
    out += p.GetHeight().ToString() + "_";
    out += p.GetBodyMassIndex().ToString() + "_";
    out += p.GetBodyFatFraction().ToString() + "_";
    out += p.GetHeartRateBaseline().ToString() + "_";
    out += p.GetMeanArterialPressureBaseline().ToString() + "_";
    out += p.GetPulsePressureBaseline().ToString() + "_";
    out += p.GetRespirationRateBaseline().ToString();

    return out;
  }

  void PVGenerator::GenerateTriagePatientData(std::string rootDir, PatientStateListData& pList)
  {
    Logger* logger = GetLogger();
    logger->LogToConsole(true);

    std::string patientsDir = rootDir + "patients/";

    GenerateTriagePatients(pList, patientsDir);

    return;
  }

  void PVGenerator::GenerateTriageInsultData(std::string rootDir, PatientStateListData& pList, PatientStateListData& pInsultsList)
  {
    Logger* logger = GetLogger();
    logger->LogToConsole(true);

    std::string insultsDir = rootDir + "insults/";

    GenerateTriageInsults(pList, pInsultsList, insultsDir);

    return;
  }

  std::string PVGenerator::PatientToDescriptionString(SEPatient& p)
  {
    std::string out;
    out = "Patient: ";
    out += "Sex = " + ePatient_Sex_Name(p.GetSex()) + ", ";
    out += "Age = " + p.GetAge().ToString() + ", ";
    out += "Height = " + p.GetHeight().ToString() + ", ";
    out += "Body Mass Index = " + p.GetBodyMassIndex().ToString() + ", ";
    out += "Body Fat Fraction = " + p.GetBodyFatFraction().ToString();
    return out;
  }

  std::string PVGenerator::InsultsToDescriptionString(double hemorrhageSeverity, double airwayObstructionSeverity, double pneumothoraxSeverity)
  {
    std::stringstream out;
    out << "Insults: ";
    out << "Hemorrhage Severity = ";
    out << hemorrhageSeverity;
    out << ", Airway Obstruction Severity = ";
    out << airwayObstructionSeverity;
    out << ", Tension Pneumothorax Severity = ";
    out << pneumothoraxSeverity;
    return out.str();
  }

  void PVGenerator::GenerateTriagePatients(PatientStateListData& pList, const std::string& directory)
  {
    m_TotalRuns = 0;
    m_Duplicates = 0;
    m_TotalPatients = 0;
    m_NumPatientsFailedToSetup = 0;

    Info("Generating combinatorial patient data set");
    // Each parameter we want to include in our permutations, has 3 options, min, max, standard/default
    std::vector<size_t> opts = { 2,2,2,2 }; // max index is 2
    std::vector<std::vector<size_t>> combinations;
    GeneralMath::Combinations(opts, combinations);

    std::vector<ePatient_Sex> sexes = { ePatient_Sex::Male, ePatient_Sex::Female };
    for (auto sex : sexes)
    {
      ResetParameters(sex);
      for (auto idxs : combinations)
      {
        SEPatient patient(GetLogger());
        patient.SetSex(sex);

        patient.GetAge().SetValue(Age_yr.Values()[idxs[0]], TimeUnit::yr);
        patient.GetHeight().SetValue(Height_cm.Values()[idxs[1]], LengthUnit::cm);
        patient.GetBodyFatFraction().SetValue(BFF.Values()[idxs[2]]);
        patient.GetBodyMassIndex().SetValue(BMI.Values()[idxs[3]]);

        CreateTriagePatient(pList, patient, directory);
      }
    }

    Info("Removed " + std::to_string(m_Duplicates) + " duplicates");
    Info("Created " + std::to_string(m_TotalPatients) + " patients");
    Info("Created " + std::to_string(m_TotalRuns) + " total runs");
  }

  void PVGenerator::GenerateTriageInsults(PatientStateListData& pList, PatientStateListData& pInsultsList, const std::string& directory)
  {
    double severities[] = { 0.0, 0.3, 0.6, 0.9 };

    m_TotalRuns = 0;
    m_Duplicates = 0;
    m_TotalPatients = 0;
    m_NumPatientsFailedToSetup = 0;

    Info("Generating combinatorial patient data set");
    std::vector<size_t> opts = { 3,3,3 }; // max index is 3
    std::vector<std::vector<size_t>> combinations;
    GeneralMath::Combinations(opts, combinations);

    for (unsigned int i = 0; i < pList.patientstate_size(); i++)
    {
      pulse::study::bind::patient_variability::PatientStateData* startingPatientState = &(*pList.mutable_patientstate())[i];
      std::string patientStatePath = startingPatientState->statefilename();

      for (auto idxs : combinations)
      {
        std::string patientName = "Insults" + std::to_string(m_TotalPatients);
        std::string patientFileName = directory + patientName;

        Info("Creating insult: " + patientFileName);
        pulse::study::bind::patient_variability::PatientStateData* patientStateData = pInsultsList.add_patientstate();
        patientStateData->set_id(m_TotalRuns);
        patientStateData->set_outputbasefilename(patientFileName);
        patientStateData->set_statefilename(patientStatePath);
        patientStateData->mutable_validation();// Create a validation object to fill
        pulse::cdm::bind::PatientData* patientData = patientStateData->mutable_patient();

        patientData->set_name(patientName);

        double hemorrhageSeverity = severities[idxs[0]];
        double airwayObstructionSeverity = severities[idxs[1]];
        double pneumothoraxSeverity = severities[idxs[2]];

        auto triage = patientStateData->mutable_triage();
        auto hemorrhage = triage->mutable_hemorrhageaction();
        hemorrhage->set_compartment(CDM_BIND::HemorrhageData_eCompartment_VenaCava);
        hemorrhage->mutable_severity()->mutable_scalar0to1()->set_value(hemorrhageSeverity);

        auto airwayObstruction = triage->mutable_airwayobstructionaction();
        airwayObstruction->mutable_severity()->mutable_scalar0to1()->set_value(airwayObstructionSeverity);

        auto pneumothorax = triage->mutable_tensionpneumothoraxaction();
        pneumothorax->set_type(CDM_BIND::Open);
        pneumothorax->set_side(CDM_BIND::Right);
        pneumothorax->mutable_severity()->mutable_scalar0to1()->set_value(pneumothoraxSeverity);

        patientStateData->set_description(InsultsToDescriptionString(hemorrhageSeverity, airwayObstructionSeverity, pneumothoraxSeverity));

        m_TotalRuns++;
        m_TotalPatients++;
      }
    }

    Info("Removed " + std::to_string(m_Duplicates) + " duplicates");
    Info("Created " + std::to_string(m_TotalPatients) + " patients");
    Info("Created " + std::to_string(m_TotalRuns) + " total runs");
  }

  void PVGenerator::CreateTriagePatient(PatientStateListData& pList, SEPatient& patient, const std::string& directory)
  {
    std::string patientName = "Patient" + std::to_string(m_TotalPatients);
    patient.SetName(patientName);
    std::string patientFileName = directory + patientName;

    AddTriagePatientToList(pList, patient, patientFileName);
    m_TotalPatients++;
  }

  pulse::study::bind::patient_variability::PatientStateData* PVGenerator::AddTriagePatientToList(PatientStateListData& pList, SEPatient& patient, const std::string& full_dir_path)
  {
    Info("Creating patient: " + full_dir_path);
    pulse::study::bind::patient_variability::PatientStateData* patientStateData = pList.add_patientstate();
    patientStateData->set_id(m_TotalRuns);
    patientStateData->set_description(PatientToDescriptionString(patient));
    patientStateData->set_outputbasefilename(full_dir_path);
    patientStateData->set_statefilename(full_dir_path +"InitialState.json");
    patientStateData->mutable_validation();// Create a validation object to fill
    pulse::cdm::bind::PatientData* patientData = patientStateData->mutable_patient();
    PBPatient::Serialize(patient, *patientData);
    m_TotalRuns++;

    return patientStateData;
  }
}
