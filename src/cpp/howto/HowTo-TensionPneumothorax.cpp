/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "EngineHowTo.h"
#include "PulseEngine.h"

// Include the various types you will be using in your code
#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEDataRequestTracker.h"
#include "cdm/patient/actions/SETensionPneumothorax.h"
#include "cdm/patient/actions/SENeedleDecompression.h"
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/system/physiology/SERespiratorySystem.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTemperature.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEFunctionVolumeVsTime.h"
#include "cdm/properties/SEScalar0To1.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// Usage for applying a Tension Pneumothorax insult to the patient
///
/// \details
/// Refer to the SETensionPneumothorax class
/// Refer to the SENeedleDecompression class for pneumothorax intervention 
/// Refer to the SEChestOcclusiveDressing class for pneumothorax intervention 
//--------------------------------------------------------------------------------------------------
void HowToTensionPneumothorax()
{
  // Create the engine and load the patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/howto/HowTo_TensionPneumothorax.cpp/HowTo_TensionPneumothorax.log");
  pe->GetLogger()->Info("HowTo_TensionPneumothorax");

  // Create data requests for each value that should be written to the output log as the engine is executing
  SEDataRequestManager drMgr(pe->GetLogger());
  drMgr.CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("TidalVolume", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("OxygenSaturation");
  drMgr.CreatePhysiologyDataRequest("CardiacOutput", VolumePerTimeUnit::mL_Per_min);
  drMgr.SetResultsFilename("./test_results/howto/HowTo_TensionPneumothorax.cpp/HowTo_TensionPneumothorax.csv");

  if (!pe->SerializeFromFile("./states/StandardMale@0s.json", &drMgr))
  {
    pe->GetLogger()->Error("Could not load state, check the error");
    return;
  }

  pe->GetLogger()->Info("The patient is nice and healthy");
  pe->GetLogger()->Info(std::stringstream() <<"Tidal Volume : " << pe->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Respiration Rate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());
  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);;

  pe->AdvanceModelTime(50, TimeUnit::s);

  // Create a Tension Pnuemothorax 
  // Set the severity (a fraction between 0 and 1)
  SETensionPneumothorax pneumo;
  
  // You can have a Closed or Open Tension Pneumothorax
  pneumo.SetType(eGate::Closed);
  //pneumo.SetType(eGate::Open);
  pneumo.GetSeverity().SetValue(0.75);
  
    // It can be on the Left or right side
  pneumo.SetSide(eSide::Right);  
  //pneumo.SetSide(eSide::Left);
  pneumo.SetComment("ICD-9: 860.0");
  pe->ProcessAction(pneumo);

  pe->GetLogger()->Info("Giving the patient a tension pneumothorax");
  pe->GetLogger()->Info("ICD-9: 860.0");

  pe->AdvanceModelTime(120, TimeUnit::s);//This will advance the engine

  pe->GetLogger()->Info("The patient has had a tension pneumothorax for 120");
  pe->GetLogger()->Info(std::stringstream() <<"Tidal Volume : " << pe->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Respiration Rate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());
  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);;

  // Needle Decompression should help the patient out
  SENeedleDecompression needleDecomp;
  
  // You can turn it off when you would like to remove the intervention
  needleDecomp.SetState(eSwitch::Off);
  
  // It can be on the Left or right side (it's a good idea to do it on the side of the pneumothorax ;)
  needleDecomp.SetSide(eSide::Right);
  //needleDecomp.SetSide(CDM::enumSide::Left);
  
  needleDecomp.SetState(eSwitch::On);
  pe->ProcessAction(needleDecomp);
  pe->GetLogger()->Info("Giving the patient a needle decompression");

  pe->AdvanceModelTime(400, TimeUnit::s);

  pe->GetLogger()->Info("The patient has had a needle decompressed tension pneumothorax for 400s");
  pe->GetLogger()->Info(std::stringstream() <<"Tidal Volume : " << pe->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Respiration Rate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());
  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info("Finished");
}
