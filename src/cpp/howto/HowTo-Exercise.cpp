/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "EngineHowTo.h"
#include "PulseEngine.h"

// Include the various types you will be using in your code
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEDataRequestTracker.h"
#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/patient/actions/SEExercise.h"
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/system/physiology/SEEnergySystem.h"
#include "cdm/system/physiology/SERespiratorySystem.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/substance/SESubstanceCompound.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTemperature.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalarPower.h"
#include "cdm/properties/SEScalar0To1.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// Usage for applying an Exercise insult to the patient
///
/// \details
/// Refer to the SEExercise class
//--------------------------------------------------------------------------------------------------
void HowToExercise() 
{
  // Create the engine and load the patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/HowTo_Exercise.log");
  pe->GetLogger()->Info("HowTo_Exercise");

  // Create data requests for each value that should be written to the output log as the engine is executing
  SEDataRequestManager drMgr(pe->GetLogger());
  drMgr.CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("CardiacOutput", VolumePerTimeUnit::mL_Per_min);
  drMgr.CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("TotalMetabolicRate", PowerUnit::kcal_Per_day);
  drMgr.CreatePhysiologyDataRequest("CoreTemperature", TemperatureUnit::C);
  drMgr.CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("AchievedExerciseLevel");
  drMgr.CreatePhysiologyDataRequest("FatigueLevel");
  drMgr.CreatePhysiologyDataRequest("TotalMetabolicRate", PowerUnit::W);
  drMgr.CreatePhysiologyDataRequest("TotalWorkRateLevel");
  drMgr.SetResultsFilename("HowToExercise.csv");

  if (!pe->SerializeFromFile("./states/StandardMale@0s.json", &drMgr))
  {
    pe->GetLogger()->Error("Could not load state, check the error");
    return;
  }

  // Advance some time to get some resting data
  pe->AdvanceModelTime(20, TimeUnit::s);

  pe->GetLogger()->Info("The patient is nice and healthy");
  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Total Metabolic Rate : " << pe->GetEnergySystem()->GetTotalMetabolicRate(PowerUnit::kcal_Per_day) << PowerUnit::kcal_Per_day);
  pe->GetLogger()->Info(std::stringstream() <<"Core Temperature : " << pe->GetEnergySystem()->GetCoreTemperature(TemperatureUnit::C) << TemperatureUnit::C);
  pe->GetLogger()->Info(std::stringstream() <<"RespirationRate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  //pe->GetLogger()->Info(std::stringstream() <<"AchievedExerciseLevel : " << pe->GetEnergySystem()->GetAchievedExerciseLevel()); // This will be NaN as the patient is not doing any exercise
  //pe->GetLogger()->Info(std::stringstream() <<"FatigueLevel : " << pe->GetEnergySystem()->GetFatigueLevel()); // No fatigue either
  pe->GetLogger()->Info(std::stringstream() <<"TotalMetabolicRate : " << pe->GetEnergySystem()->GetTotalMetabolicRate(PowerUnit::W) << PowerUnit::W);//This will be at Basal Level
  pe->GetLogger()->Info(std::stringstream() <<"TotalWorkRateLevel : " << pe->GetEnergySystem()->GetTotalWorkRateLevel());; // Nothing here either as the patient is resting

  // Exercise Starts - instantiate an Exercise action and have the engine process it.
  // After initiating exercise the patient’s metabolic rate begins to increased. 
  // An intensity of 1.0 asks the patient to exercise at the maximum work rate capable by the patient (an output of ~1200W for a patient in relativly good shape).
  // The patient will not be able to sustain this pace for long... and will get fatigued
  // This leads to an increase in core temperature, cardiac output, respiration rate and tidal volume.
  // The increase in core temperature leads to an elevated sweat rate, which causes the patient’s skin temperature to drop due to evaporation. 
  SEExercise ex;
  ex.GetIntensity().SetValue(0.5);
  pe->ProcessAction(ex);
  pe->AdvanceModelTime(30, TimeUnit::s);

  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Total Metabolic Rate : " << pe->GetEnergySystem()->GetTotalMetabolicRate(PowerUnit::kcal_Per_day) << PowerUnit::kcal_Per_day);
  pe->GetLogger()->Info(std::stringstream() <<"Core Temperature : " << pe->GetEnergySystem()->GetCoreTemperature(TemperatureUnit::C) << TemperatureUnit::C);
  pe->GetLogger()->Info(std::stringstream() <<"RespirationRate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"AchievedExerciseLevel : " << pe->GetEnergySystem()->GetAchievedExerciseLevel()); // this is the fraction of what we asked for, 1.0 means we are doing what you asked for.
  pe->GetLogger()->Info(std::stringstream() <<"FatigueLevel : " << pe->GetEnergySystem()->GetFatigueLevel()); // Patient is very tired
  pe->GetLogger()->Info(std::stringstream() <<"TotalMetabolicRate : " << pe->GetEnergySystem()->GetTotalMetabolicRate(PowerUnit::W) << PowerUnit::W);// We are burning 
  pe->GetLogger()->Info(std::stringstream() <<"TotalWorkRateLevel : " << pe->GetEnergySystem()->GetTotalWorkRateLevel());; // How much work we are getting / the max work rate possible

  // Once exercise has ended, the patient is in a recovery period where the metabolic rate begins to return to its basal value. 
  // The cardiac output, respiration rate and tidal volume follow this recovery trend towards their normal values.
  ex.GetIntensity().SetValue(0);
  pe->ProcessAction(ex);
  
  // Advance some time while the medic gets the drugs ready
  pe->AdvanceModelTime(30, TimeUnit::s);

  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Total Metabolic Rate : " << pe->GetEnergySystem()->GetTotalMetabolicRate(PowerUnit::kcal_Per_day) << PowerUnit::kcal_Per_day);
  pe->GetLogger()->Info(std::stringstream() <<"Core Temperature : " << pe->GetEnergySystem()->GetCoreTemperature(TemperatureUnit::C) << TemperatureUnit::C);
  pe->GetLogger()->Info(std::stringstream() <<"RespirationRate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  //pe->GetLogger()->Info(std::stringstream() <<"AchievedExerciseLevel : " << pe->GetEnergySystem()->GetAchievedExerciseLevel()); // This will be NaN as the patient is not doing any exercise
  //pe->GetLogger()->Info(std::stringstream() <<"FatigueLevel : " << pe->GetEnergySystem()->GetFatigueLevel()); // We are not working out but we are still fatigued
  pe->GetLogger()->Info(std::stringstream() <<"TotalMetabolicRate : " << pe->GetEnergySystem()->GetTotalMetabolicRate(PowerUnit::W) << PowerUnit::W);// We are still burning 
  pe->GetLogger()->Info(std::stringstream() <<"TotalWorkRateLevel : " << pe->GetEnergySystem()->GetTotalWorkRateLevel());; // How much work we are getting over the max work rate possible
  pe->GetLogger()->Info("Finished");
}
