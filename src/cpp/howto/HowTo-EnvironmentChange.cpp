/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "EngineHowTo.h"
#include "PulseEngine.h"

// Include the various types you will be using in your code
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/compartment/fluid/SEGasCompartment.h"
#include "cdm/system/environment/SEActiveConditioning.h"
#include "cdm/system/environment/SEEnvironmentalConditions.h"
#include "cdm/system/environment/conditions/SEInitialEnvironmentalConditions.h"
#include "cdm/system/environment/actions/SEChangeEnvironmentalConditions.h"
#include "cdm/system/environment/actions/SEThermalApplication.h"
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/system/physiology/SEEnergySystem.h"
#include "cdm/system/physiology/SERespiratorySystem.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/substance/SESubstanceCompound.h"
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
#include "cdm/properties/SEScalarPower.h"
#include "cdm/properties/SEScalarPressureTimePerVolume.h"
#include "cdm/substance/SESubstanceFraction.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// Usage for applying an Environment Change to the patient
///
/// \details
/// Refer to the SEEnvironmentChange class
/// Refer to the SEEnvironmentHeating class
//--------------------------------------------------------------------------------------------------
void HowToEnvironmentChange()
{
  // Create the engine and load the patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/HowTo_EnvironmentChange.log");
  pe->GetLogger()->Info("HowTo_EnvironmentChange");
  /*
  // You have the option to change the enviornmental conditions of the patient 
  // with a condition or an action. By default the standard environment file is used
  // when stabilizing the patient, all initial states provided are with that environment.
  // You will need to initialize the engine to this condition
  // You could then save out your own state and use it in the future

  SEInitialEnvironment ienv(pe->GetSubstanceManager());
  ienv.SetConditionsFile("./environments/AnchorageDecember.json");
  // You can set a file or the conditions object just like is shown below
  std::vector<const SECondition*> conditions;
  conditions.push_back(&ienv);

  if (!pe->InitializeEngine("StandardMale.json", &conditions))
  {
    pe->GetLogger()->Error("Could not load initialize engine, check the error");
    return;
  }
  */

  if (!pe->SerializeFromFile("./states/StandardMale@0s.json"))
  {
    pe->GetLogger()->Error("Could not load state, check the error");
    return;
  }

  // Get some substances out we will use
  const SESubstance* N2 = pe->GetSubstanceManager().GetSubstance("Nitrogen");
  const SESubstance* O2 = pe->GetSubstanceManager().GetSubstance("Oxygen");
  const SESubstance* CO2 = pe->GetSubstanceManager().GetSubstance("CarbonDioxide");

  // Create data requests for each value that should be written to the output log as the engine is executing
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("CardiacOutput", VolumePerTimeUnit::mL_Per_min);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("SkinTemperature", TemperatureUnit::C);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("CoreTemperature", TemperatureUnit::C);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("TotalMetabolicRate", PowerUnit::W);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("SystemicVascularResistance", PressureTimePerVolumeUnit::mmHg_s_Per_mL);

  pe->GetEngineTracker()->GetDataRequestManager().SetResultsFilename("HowToEnvironmentChange.csv");

  // Advance some time to get some resting data
  AdvanceAndTrackTime_s(50, *pe);

  pe->GetLogger()->Info("The patient is nice and healthy");
  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Skin Temperature : " << pe->GetEnergySystem()->GetSkinTemperature(TemperatureUnit::C) << TemperatureUnit::C);
  pe->GetLogger()->Info(std::stringstream() <<"Core Temperature : " << pe->GetEnergySystem()->GetCoreTemperature(TemperatureUnit::C) << TemperatureUnit::C);
  pe->GetLogger()->Info(std::stringstream() <<"Total Metabolic Rate : " << pe->GetEnergySystem()->GetTotalMetabolicRate(PowerUnit::W) << PowerUnit::W);
  pe->GetLogger()->Info(std::stringstream() <<"Systemic Vascular Resistance : " << pe->GetCardiovascularSystem()->GetSystemicVascularResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL) << PressureTimePerVolumeUnit::mmHg_s_Per_mL);;

  // EnvironmentChange Starts - instantiate a EnvironmentChange action and have the engine process it
  // The patient is instantly submerged in 10 degree Celsius water. 
  // This causes an immediate drop in the skin temperature due to increased convective heat transfer from the skin surface. 
  // The core temperature follows the skin temperature, and the metabolic rate increases due to shivering.
  SEChangeEnvironmentalConditions env(pe->GetLogger());
  SEEnvironmentalConditions& conditions = env.GetEnvironmentalConditions();
  conditions.GetMechanicalDeadSpace().SetValue(500, VolumeUnit::mL);
  conditions.SetSurroundingType(eSurroundingType::Water);
  conditions.GetAirVelocity().SetValue(0, LengthPerTimeUnit::m_Per_s);
  conditions.GetAmbientTemperature().SetValue(10.0, TemperatureUnit::C);
  conditions.GetAtmosphericPressure().SetValue(760., PressureUnit::mmHg);
  conditions.GetClothingResistance().SetValue(0.01, HeatResistanceAreaUnit::clo);
  conditions.GetEmissivity().SetValue(0.0);
  conditions.GetMeanRadiantTemperature().SetValue(22.0, TemperatureUnit::C);
  conditions.GetRelativeHumidity().SetValue(1.0);
  conditions.GetRespirationAmbientTemperature().SetValue(22.0, TemperatureUnit::C);
  conditions.GetAmbientGas(*N2).GetFractionAmount().SetValue(0.7901);
  conditions.GetAmbientGas(*O2).GetFractionAmount().SetValue(0.2095);
  conditions.GetAmbientGas(*CO2).GetFractionAmount().SetValue(4.0E-4);
  pe->ProcessAction(env);
  AdvanceAndTrackTime_s(30, *pe);

  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Skin Temperature : " << pe->GetEnergySystem()->GetSkinTemperature(TemperatureUnit::C) << TemperatureUnit::C);
  pe->GetLogger()->Info(std::stringstream() <<"Core Temperature : " << pe->GetEnergySystem()->GetCoreTemperature(TemperatureUnit::C) << TemperatureUnit::C);
  pe->GetLogger()->Info(std::stringstream() <<"Total Metabolic Rate : " << pe->GetEnergySystem()->GetTotalMetabolicRate(PowerUnit::W) << PowerUnit::W);
  pe->GetLogger()->Info(std::stringstream() <<"Systemic Vascular Resistance : " << pe->GetCardiovascularSystem()->GetSystemicVascularResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL) << PressureTimePerVolumeUnit::mmHg_s_Per_mL);;

  // The patient is removed from the water and returns to the normal environment. 
  // The skin temperature rises, leading to a subsequent rise in core temperature
  conditions.SetSurroundingType(eSurroundingType::Water);
  conditions.GetAirVelocity().SetValue(0.1, LengthPerTimeUnit::m_Per_s);
  conditions.GetAmbientTemperature().SetValue(22.0, TemperatureUnit::C);
  conditions.GetAtmosphericPressure().SetValue(760., PressureUnit::mmHg);
  conditions.GetClothingResistance().SetValue(0.01, HeatResistanceAreaUnit::clo);
  conditions.GetEmissivity().SetValue(0.95);
  conditions.GetMeanRadiantTemperature().SetValue(22.0, TemperatureUnit::C);
  conditions.GetRelativeHumidity().SetValue(0.6);
  conditions.GetRespirationAmbientTemperature().SetValue(19, TemperatureUnit::C);
  conditions.GetAmbientGas(*N2).GetFractionAmount().SetValue(0.7896);
  conditions.GetAmbientGas(*O2).GetFractionAmount().SetValue(0.21);
  conditions.GetAmbientGas(*CO2).GetFractionAmount().SetValue(4.0E-4);
  pe->ProcessAction(env);
  AdvanceAndTrackTime_s(60, *pe);

  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Skin Temperature : " << pe->GetEnergySystem()->GetSkinTemperature(TemperatureUnit::C) << TemperatureUnit::C);
  pe->GetLogger()->Info(std::stringstream() <<"Core Temperature : " << pe->GetEnergySystem()->GetCoreTemperature(TemperatureUnit::C) << TemperatureUnit::C);
  pe->GetLogger()->Info(std::stringstream() <<"Total Metabolic Rate : " << pe->GetEnergySystem()->GetTotalMetabolicRate(PowerUnit::W) << PowerUnit::W);
  pe->GetLogger()->Info(std::stringstream() <<"Systemic Vascular Resistance : " << pe->GetCardiovascularSystem()->GetSystemicVascularResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL) << PressureTimePerVolumeUnit::mmHg_s_Per_mL);
  pe->GetLogger()->Info("");

  //The patient’s skin temperature continues to rise as they are actively heated at 340 btu/hr. Both core temperature and skin
    // temperature increase as a result of this.  The patient’s metabolic rate has returned to normal since shivering has ceased.
  SEThermalApplication heat;
  heat.GetActiveHeating().GetPower().SetValue(340, PowerUnit::BTU_Per_hr);
  pe->ProcessAction(heat);
  AdvanceAndTrackTime_s(120, *pe);

  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Skin Temperature : " << pe->GetEnergySystem()->GetSkinTemperature(TemperatureUnit::C) << TemperatureUnit::C);
  pe->GetLogger()->Info(std::stringstream() <<"Core Temperature : " << pe->GetEnergySystem()->GetCoreTemperature(TemperatureUnit::C) << TemperatureUnit::C);
  pe->GetLogger()->Info(std::stringstream() <<"Total Metabolic Rate : " << pe->GetEnergySystem()->GetTotalMetabolicRate(PowerUnit::W) << PowerUnit::W);
  pe->GetLogger()->Info(std::stringstream() <<"Systemic Vascular Resistance : " << pe->GetCardiovascularSystem()->GetSystemicVascularResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL) << PressureTimePerVolumeUnit::mmHg_s_Per_mL);;
  pe->GetLogger()->Info("Finished");
}
