/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#include "EngineHowTo.h"
#include "PulseEngine.h"

#include "cdm/engine/SEActionManager.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/engine/SESerializeRequested.h"
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/system/physiology/SEEnergySystem.h"
#include "cdm/system/physiology/SERespiratorySystem.h"
#include "cdm/patient/actions/SEAirwayObstruction.h"
#include "cdm/patient/actions/SEHemorrhage.h"
#include "cdm/patient/actions/SETensionPneumothorax.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTemperature.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// Serializing a patient state to use later in your Pulse based application
///
/// \details
/// This will show you how to setup a patient and save it out for use in your application
//--------------------------------------------------------------------------------------------------
void HowToSerialize()
{
  std::stringstream ss;
  // Create a Pulse Engine and load the standard patient
  // This is a healty patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/howto/HowTo_Serialize.cpp/HowTo_Serialize.log");

  SEDataRequestManager drMgr(pe->GetLogger());
  drMgr.CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("TidalVolume", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("OxygenSaturation");
  drMgr.CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::Aorta, "Oxygen", "PartialPressure");
  drMgr.CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::Aorta, "CarbonDioxide", "PartialPressure");
  drMgr.CreateGasCompartmentDataRequest(pulse::PulmonaryCompartment::Lungs, "Volume");
  drMgr.CreateGasCompartmentDataRequest(pulse::PulmonaryCompartment::Carina, "InFlow");
  drMgr.SetResultsFilename("./test_results/howto/HowTo_Serialize.cpp/HowTo_Serialize.csv");


  if (!pe->SerializeFromFile("./states/StandardMale@0s.json", &drMgr))
  {
    pe->GetLogger()->Error("Could not load state, check the error");
    return;
  }

  SESerializeRequested serializeRequested;
  serializeRequested.SetFilename("./test_results/howto/HowTo_Serialize.cpp/HowTo_Serialize.requested.json");
  pe->ProcessAction(serializeRequested);

  pe->GetLogger()->Info("Healthy patient vitals");
  pe->GetLogger()->Info(std::stringstream() << "Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Arterial Pressure : " << pe->GetCardiovascularSystem()->GetArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());
  pe->GetLogger()->Info(std::stringstream() << "End Tidal Carbon Dioxide Fraction : " << pe->GetRespiratorySystem()->GetEndTidalCarbonDioxideFraction());
  pe->GetLogger()->Info(std::stringstream() << "End Tidal Carbon Dioxide Pressure : " << pe->GetRespiratorySystem()->GetEndTidalCarbonDioxidePressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Respiration Rate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Skin Temperature : " << pe->GetEnergySystem()->GetSkinTemperature(TemperatureUnit::C) << "C");
  pe->GetLogger()->Info(std::stringstream() << "Blood Volume : " << pe->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL) << VolumeUnit::mL);

  // Next we will want to make the patient unhealthy in some way.
  // This will allow you to start your application with unhealty state

  // Let's mimic a battle field injury

  // Add a Hemorrhage
  SEHemorrhage hemorrhageLeg;
  hemorrhageLeg.SetCompartment(eHemorrhage_Compartment::RightLeg);//the location of the hemorrhage
  hemorrhageLeg.GetSeverity().SetValue(0.5);//the severity of hemorrhage
  pe->ProcessAction(hemorrhageLeg);

  // Add a Tension Pnuemothorax
  SETensionPneumothorax pneumo;
  pneumo.SetSide(eSide::Right);
  pneumo.SetType(eGate::Closed);
  pneumo.GetSeverity().SetValue(0.5);
  pe->ProcessAction(pneumo);

  // Add an Airway Obstruction
  SEAirwayObstruction obstruction;
  obstruction.GetSeverity().SetValue(0.25);
  pe->ProcessAction(obstruction);

  // Now let's advance 5 minutes with these actions to get the patient into a injured state
  pe->AdvanceModelTime(5, TimeUnit::min);

  pe->GetLogger()->Info("Wounded patient vitals");
  pe->GetLogger()->Info(std::stringstream() << "Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Arterial Pressure : " << pe->GetCardiovascularSystem()->GetArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());
  pe->GetLogger()->Info(std::stringstream() << "End Tidal Carbon Dioxide Fraction : " << pe->GetRespiratorySystem()->GetEndTidalCarbonDioxideFraction());
  pe->GetLogger()->Info(std::stringstream() << "End Tidal Carbon Dioxide Pressure : " << pe->GetRespiratorySystem()->GetEndTidalCarbonDioxidePressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Respiration Rate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Skin Temperature : " << pe->GetEnergySystem()->GetSkinTemperature(TemperatureUnit::C) << "C");
  pe->GetLogger()->Info(std::stringstream() << "Blood Volume : " << pe->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL) << VolumeUnit::mL);

  // Save this state out.
  // You an then load this state in your application
  pe->SerializeToFile("./test_results/howto/HowTo_Serialize.cpp/WoundedSoldier.json");
}
