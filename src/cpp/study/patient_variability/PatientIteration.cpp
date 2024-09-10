/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "PatientIteration.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarLength.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTemperature.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerPressure.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/utils/FileUtils.h"
#include "cdm/utils/GeneralMath.h"

namespace pulse::study::patient_variability
{
  PatientIteration::PatientIteration(Logger& logger) : ScenarioIteration(logger)
  {
    SetSex(ePatient_Sex::Male);
    m_IterationName = "default_male";
    SetStateDirectory("");
    SetScenarioExecListFilename("");


    m_Actions.push_back(&m_Serialize);
    m_Patient = &GetPatientConfiguration().GetPatient();
  }

  PatientIteration::~PatientIteration()
  {
    Clear();
  }

  void PatientIteration::Clear()
  {
    m_Actions.clear();
    PulseScenario::Clear();
  }

  void PatientIteration::FixUp()
  {
    if(m_Age_yr.Empty())
      m_Age_yr.SetValues({ stdAge_yr });
    if (m_Sex == ePatient_Sex::Male)
    {
      if (m_Height_cm.Empty())
        m_Height_cm.SetValues({ stdMaleHeight_cm });
      if (m_BMI.Empty())
        m_BMI.SetValues({ stdMaleBMI });
      if (m_BFF.Empty())
        m_BFF.SetValues({ stdMaleBFF });
    }
    else
    {
      if (m_Height_cm.Empty())
        m_Height_cm.SetValues({ stdFemaleHeight_cm });
      if (m_BMI.Empty())
        m_BMI.SetValues({ stdFemaleBMI });
      if (m_BFF.Empty())
        m_BFF.SetValues({ stdFemaleBFF });

    }
    if (m_HR_bpm.Empty())
      m_HR_bpm.SetValues({ stdHR_bpm });
    if (m_MAP_mmHg.Empty())
      m_MAP_mmHg.SetValues({ stdMAP_mmHg });
    if (m_PP_mmHg.Empty())
      m_PP_mmHg.SetValues({ stdPulsePressure_mmHg });
    if (m_RR_bpm.Empty())
      m_RR_bpm.SetValues({ stdRR_bpm });
  }

  void PatientIteration::GenerateScenarios()
  {
    FixUp();
    m_Duplicates = 0;
    m_PatientStates.clear();

    Info("Generating patient scenarios to: " + m_ScenarioDirectory);
    Info("Generating patient states to: " + m_StateDirectory);

    switch (m_GenStyle)
    {
    case eGenStyle::Combo:
      GenerateCombinationPatientList();
      break;
    case eGenStyle::Slice:
      GenerateSlicedPatientList();
      break;
    }

    Info("Removed " + std::to_string(m_Duplicates) + " duplicate patients");
    Info("Defined " + std::to_string(m_PatientStates.size()) + " patients");
    WriteScenarioList();
  }

  void PatientIteration::GenerateSlicedPatientList()
  {
    SEPatient basePatient(GetLogger());
    basePatient.SetSex(m_Sex);
    basePatient.GetAge().SetValue(m_Age_yr.GetSlice(), TimeUnit::yr);
    basePatient.GetHeight().SetValue(m_Height_cm.GetSlice(), LengthUnit::cm);
    basePatient.GetBodyMassIndex().SetValue(m_BMI.GetSlice());
    basePatient.GetBodyFatFraction().SetValue(m_BFF.GetSlice());
    basePatient.GetHeartRateBaseline().SetValue(m_HR_bpm.GetSlice(), FrequencyUnit::Per_min);
    basePatient.GetMeanArterialPressureBaseline().SetValue(m_MAP_mmHg.GetSlice(), PressureUnit::mmHg);
    basePatient.GetPulsePressureBaseline().SetValue(m_PP_mmHg.GetSlice(), PressureUnit::mmHg);
    basePatient.GetRespirationRateBaseline().SetValue(m_RR_bpm.GetSlice(), FrequencyUnit::Per_min);

    Info("Generating Age slice data set");
    for (double age_yr : m_Age_yr.GetValues())
    {
      m_Patient->Copy(basePatient);
      m_Patient->GetAge().SetValue(age_yr, TimeUnit::yr);
      GenerateScenario();
    }
    basePatient.GetAge().SetValue(m_Age_yr.GetSlice(), TimeUnit::yr);

    Info("Generating Height slice data set");
    for (double height_cm : m_Height_cm.GetValues())
    {
      m_Patient->Copy(basePatient);
      m_Patient->GetHeight().SetValue(height_cm, LengthUnit::cm);
      GenerateScenario();
    }
    basePatient.GetHeight().SetValue(m_Height_cm.GetSlice(), LengthUnit::cm);

    Info("Generating BMI slice data set");
    for (double bmi : m_BMI.GetValues())
    {
      m_Patient->Copy(basePatient);
      m_Patient->GetBodyMassIndex().SetValue(bmi);
      GenerateScenario();
    }
    basePatient.GetBodyMassIndex().SetValue(m_BMI.GetSlice());

    Info("Generating BFF slice data set");
    for (double bff : m_BFF.GetValues())
    {
      m_Patient->Copy(basePatient);
      m_Patient->GetBodyFatFraction().SetValue(bff);
      GenerateScenario();
    }
    basePatient.GetBodyFatFraction().SetValue(m_BFF.GetSlice());

    Info("Generating HR slice data set");
    for (double hr_bpm : m_HR_bpm.GetValues())
    {
      m_Patient->Copy(basePatient);
      m_Patient->GetHeartRateBaseline().SetValue(hr_bpm, FrequencyUnit::Per_min);
      GenerateScenario();
    }
    basePatient.GetHeartRateBaseline().SetValue(m_HR_bpm.GetSlice(), FrequencyUnit::Per_min);

    Info("Generating MAP/PP combo slice data set");
    // Iterating MAP and Pulse Pressure by themselves don't effectivly cover the variability we would like
    // They are dependent on each other and Systolic/Diastolic, so to get a good coverage,
    // we are going to combo these two values and toss invalid patients.
    // This gives us a pretty good coverage of variabile cv pressures
    Logger null;// I don't want to log the SetupPatient failures
    null.LogToConsole(false);
    //size_t cnt = 0, bad = 0;
    for (double map : m_MAP_mmHg.GetValues())
    {
      for (double pp_mmHg : m_PP_mmHg.GetValues())
      {
        m_Patient->Copy(basePatient);
        m_Patient->GetSystolicArterialPressureBaseline().Invalidate();
        m_Patient->GetDiastolicArterialPressureBaseline().Invalidate();
        m_Patient->GetMeanArterialPressureBaseline().SetValue(map, PressureUnit::mmHg);
        m_Patient->GetPulsePressureBaseline().SetValue(pp_mmHg, PressureUnit::mmHg);
        GenerateScenario();
      }
    }
    basePatient.GetMeanArterialPressureBaseline().SetValue(m_MAP_mmHg.GetSlice(), PressureUnit::mmHg);
    basePatient.GetPulsePressureBaseline().SetValue(m_PP_mmHg.GetSlice(), PressureUnit::mmHg);

    Info("Generating RR slice data set");
    for (double rr : m_RR_bpm.GetValues())
    {
      m_Patient->Copy(basePatient);
      m_Patient->GetRespirationRateBaseline().SetValue(rr, FrequencyUnit::Per_min);
      GenerateScenario();
    }
    basePatient.GetRespirationRateBaseline().SetValue(m_RR_bpm.GetSlice(), FrequencyUnit::Per_min);
  }

  void PatientIteration::GenerateCombinationPatientList()
  {
    Info("Generating combinatorial data set");
    // Each parameter we want to include in our permutations, has 3 options, min, max, standard/default
    std::vector<size_t> opts; // -1 as the opts holds the max index of that option
    opts.push_back(m_Age_yr.GetValues().size()-1);
    opts.push_back(m_Height_cm.GetValues().size()-1);
    opts.push_back(m_BFF.GetValues().size()-1);
    opts.push_back(m_BMI.GetValues().size()-1);
    opts.push_back(m_HR_bpm.GetValues().size()-1);
    opts.push_back(m_MAP_mmHg.GetValues().size()-1);
    opts.push_back(m_PP_mmHg.GetValues().size()-1);
    opts.push_back(m_RR_bpm.GetValues().size()-1);
    std::vector<std::vector<size_t>> permutations;
    GeneralMath::Combinations(opts, permutations);

    for (auto idxs : permutations)
    {
      m_Patient->SetSex(m_Sex);
      m_Patient->GetAge().SetValue(m_Age_yr.GetValues()[idxs[0]], TimeUnit::yr);
      m_Patient->GetHeight().SetValue(m_Height_cm.GetValues()[idxs[1]], LengthUnit::cm);
      m_Patient->GetBodyFatFraction().SetValue(m_BFF.GetValues()[idxs[2]]);
      m_Patient->GetBodyMassIndex().SetValue(m_BMI.GetValues()[idxs[3]]);
      m_Patient->GetHeartRateBaseline().SetValue(m_HR_bpm.GetValues()[idxs[4]], FrequencyUnit::Per_min);
      m_Patient->GetMeanArterialPressureBaseline().SetValue(m_MAP_mmHg.GetValues()[idxs[5]], PressureUnit::mmHg);
      m_Patient->GetPulsePressureBaseline().SetValue(m_PP_mmHg.GetValues()[idxs[6]], PressureUnit::mmHg);
      m_Patient->GetRespirationRateBaseline().SetValue(m_RR_bpm.GetValues()[idxs[7]], FrequencyUnit::Per_min);
      GenerateScenario();
    }
  }

  void PatientIteration::GenerateScenario()
  {
    std::string name = ToString(*m_Patient);
    if (m_PatientStates.find(name) != m_PatientStates.end())
    {
      Info("Ignoring duplicate patient: " + name);
      m_Duplicates++;
      return;
    }
    m_Name = name;
    m_Patient->SetName(name);
    GetDataRequestManager().SetResultsFilename(m_ResultsDirectory+m_Name+".csv");
    m_Serialize.SetFilename(m_StateDirectory+name+".pbb");
    m_PatientStates[name] = m_Serialize.GetFilename();

    WriteScenario();
  }

  std::string PatientIteration::ToString(SEPatient& p)
  {
    std::string out;
    out =  ePatient_Sex_Name(p.GetSex()) + "_";
    out += p.GetAge().ToString() + "_";
    out += p.GetHeight().ToString() + "_";
    out += p.GetBodyMassIndex().ToString() + "bmi_";
    out += p.GetBodyFatFraction().ToString() + "bff_";
    out += p.GetHeartRateBaseline().ToString() + "_";
    out += p.GetMeanArterialPressureBaseline().ToString() + "_";
    out += p.GetPulsePressureBaseline().ToString() + "_";
    out += p.GetRespirationRateBaseline().ToString();
    out = Replace(out, "(", "");
    out = Replace(out, ")", "");
    out = Replace(out, "1/min", "bpm");
    return out;
  }
}
