/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "EngineHowTo.h"
#include "PulseEngine.h"

// Include the various types you will be using in your code
#include "cdm/engine/SEConditionManager.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEDataRequestTracker.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/compartment/fluid/SELiquidCompartment.h"
#include "cdm/compartment/fluid/SEGasCompartment.h"
#include "cdm/system/environment/SEActiveConditioning.h"
#include "cdm/system/environment/conditions/SEInitialEnvironmentalConditions.h"
#include "cdm/system/environment/actions/SEChangeEnvironmentalConditions.h"
#include "cdm/system/environment/SEEnvironmentalConditions.h"
#include "cdm/substance/SESubstanceFraction.h"
#include "cdm/substance/SESubstanceConcentration.h"
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


//--------------------------------------------------------------------------------------------------
/// \brief
/// Usage for applying a smokey environment to the patient
///
/// \details
/// Refer to the SEEnvironmentChange class
//--------------------------------------------------------------------------------------------------
void HowToSmoke()
{
  // Create the engine and load the patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/howto/HowTo_Smoke.cpp/HowTo_Smoke.log");
  pe->GetLogger()->Info("HowTo_Smoke");

  // Create data requests for each value that should be written to the output log as the engine is executing
  SEDataRequestManager drMgr(pe->GetLogger());
  drMgr.CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("CardiacOutput", VolumePerTimeUnit::mL_Per_min);
  drMgr.CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("SkinTemperature", TemperatureUnit::C);
  drMgr.CreatePhysiologyDataRequest("CoreTemperature", TemperatureUnit::C);
  drMgr.CreatePhysiologyDataRequest("TotalMetabolicRate", PowerUnit::W);
  drMgr.CreatePhysiologyDataRequest("SystemicVascularResistance", PressureTimePerVolumeUnit::mmHg_s_Per_mL);
  drMgr.SetResultsFilename("./test_results/howto/HowTo_Smoke.cpp/HowTo_Smoke.csv");

  // Smoke is made up of many things.
  // You will need to add 2 things to the environement to effectively model a smokey environment
  // A solid particle substance, and CarbonMonoxide
  // You can create your own environment file with these, the following would work

  SEPatientConfiguration pc;
  pc.SetPatientFile("StandardMale.json");
  SEInitialEnvironmentalConditions& ienv = pc.GetConditions().GetInitialEnvironmentalConditions();
  ienv.SetEnvironmentalConditionsFile("./environments/CheyenneMountainFireFighter.json");
  if (!pe->InitializeEngine(pc, &drMgr))
  {
    pe->GetLogger()->Error("Could not load initialize engine, check the error");
    return;
  }

  /*
  if (!pe->SerializeFromFile("./states/StandardMale@0s.json", &drMgr))
  {
    pe->GetLogger()->Error("Could not load state, check the error");
    return;
  }
  */

  // Get some substances out we will use
  const SESubstance* N2 = pe->GetSubstanceManager().GetSubstance("Nitrogen");
  const SESubstance* O2 = pe->GetSubstanceManager().GetSubstance("Oxygen");
  const SESubstance* CO2 = pe->GetSubstanceManager().GetSubstance("CarbonDioxide");
  const SESubstance* CO = pe->GetSubstanceManager().GetSubstance("CarbonMonoxide");
  const SESubstance* Particulate = pe->GetSubstanceManager().GetSubstance("ForestFireParticulate");

  // Advance some time to get some resting data
  pe->AdvanceModelTime(5, TimeUnit::s);

  pe->GetLogger()->Info("The patient is nice and healthy");
  pe->GetLogger()->Info(std::stringstream() << "Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());
  pe->GetLogger()->Info(std::stringstream() << "CarbonDioxide Saturation : " << pe->GetBloodChemistrySystem()->GetCarbonDioxideSaturation());
  pe->GetLogger()->Info(std::stringstream() << "Carbon Monoxide Saturation : " << pe->GetBloodChemistrySystem()->GetCarbonMonoxideSaturation());
  pe->GetLogger()->Info(std::stringstream() << "Pulse Oximetry : " << pe->GetBloodChemistrySystem()->GetPulseOximetry());
  // There are liquid compartments for each of the gas pulmonary compartments, these track the trasportation of liquid and solid substances through the pulmonary tract, and their deposition
  // Currently, since we have not changed the environment there is no Particulate or CO in the system, so the GetSubstanceQuantity call will return nullptr, so keep this commented
  //pe->GetLogger()->Info(std::stringstream() << "Particulate Deposition : " << pe->GetCompartments().GetLiquidCompartment(pulse::PulmonaryCompartment::RightAlveoli)->GetSubstanceQuantity(*Particulate)->GetMassDeposited(MassUnit::ug) << MassUnit::ug);

  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Skin Temperature : " << pe->GetEnergySystem()->GetSkinTemperature(TemperatureUnit::C) << TemperatureUnit::C);
  pe->GetLogger()->Info(std::stringstream() <<"Core Temperature : " << pe->GetEnergySystem()->GetCoreTemperature(TemperatureUnit::C) << TemperatureUnit::C);
  pe->GetLogger()->Info(std::stringstream() <<"Total Metabolic Rate : " << pe->GetEnergySystem()->GetTotalMetabolicRate(PowerUnit::W) << PowerUnit::W);
  pe->GetLogger()->Info(std::stringstream() <<"Systemic Vascular Resistance : " << pe->GetCardiovascularSystem()->GetSystemicVascularResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL) << PressureTimePerVolumeUnit::mmHg_s_Per_mL);;

  // Here we will put this healty patient into a smokey environment.
  SEChangeEnvironmentalConditions envChange(pe->GetLogger());
  // NOTE YOUR FRACTIONS MUST ADD UP TO 1.0
  envChange.GetEnvironmentalConditions().GetAmbientGas(*N2).GetFractionAmount().SetValue(0.79008);
  envChange.GetEnvironmentalConditions().GetAmbientGas(*O2).GetFractionAmount().SetValue(0.2095);
  envChange.GetEnvironmentalConditions().GetAmbientGas(*CO2).GetFractionAmount().SetValue(4.0E-4);
  envChange.GetEnvironmentalConditions().GetAmbientGas(*CO).GetFractionAmount().SetValue(2.0E-5);
  // Concentrations are independent and do not need to add up to 1.0
  envChange.GetEnvironmentalConditions().GetAmbientAerosol(*Particulate).GetConcentration().SetValue(2.9, MassPerVolumeUnit::mg_Per_m3);
  pe->ProcessAction(envChange);
  pe->AdvanceModelTime(30, TimeUnit::s);

  pe->GetLogger()->Info(std::stringstream() << "Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());
  pe->GetLogger()->Info(std::stringstream() << "CarbonDioxide Saturation : " << pe->GetBloodChemistrySystem()->GetCarbonDioxideSaturation());
  pe->GetLogger()->Info(std::stringstream() << "Carbon Monoxide Saturation : " << pe->GetBloodChemistrySystem()->GetCarbonMonoxideSaturation());
  pe->GetLogger()->Info(std::stringstream() << "Pulse Oximetry : " << pe->GetBloodChemistrySystem()->GetPulseOximetry());
  // There are liquid compartments for each of the gas pulmonary compartments, these track the trasportation of liquid and solid substances through the pulmonary tract, and their deposition
  pe->GetLogger()->Info(std::stringstream() << "Particulate Deposition : " << pe->GetCompartments().GetLiquidCompartment(pulse::PulmonaryCompartment::RightAlveoli)->GetSubstanceQuantity(*Particulate)->GetMassDeposited(MassUnit::ug) << MassUnit::ug);

  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Skin Temperature : " << pe->GetEnergySystem()->GetSkinTemperature(TemperatureUnit::C) << TemperatureUnit::C);
  pe->GetLogger()->Info(std::stringstream() <<"Core Temperature : " << pe->GetEnergySystem()->GetCoreTemperature(TemperatureUnit::C) << TemperatureUnit::C);
  pe->GetLogger()->Info(std::stringstream() <<"Total Metabolic Rate : " << pe->GetEnergySystem()->GetTotalMetabolicRate(PowerUnit::W) << PowerUnit::W);
  pe->GetLogger()->Info(std::stringstream() <<"Systemic Vascular Resistance : " << pe->GetCardiovascularSystem()->GetSystemicVascularResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL) << PressureTimePerVolumeUnit::mmHg_s_Per_mL);;

  // Here is the amount of particulate 

  pe->GetLogger()->Info("Finished");
}
