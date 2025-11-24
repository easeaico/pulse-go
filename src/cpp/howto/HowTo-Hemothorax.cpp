/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "EngineHowTo.h"
#include "PulseEngine.h"

// Include the various types you will be using in your code
#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/engine/SEActionManager.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/engine/SEPatientActionCollection.h"
#include "cdm/patient/actions/SEHemothorax.h"
#include "cdm/patient/actions/SETubeThoracostomy.h"
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
/// Usage for applying a Hemothorax insult to the patient
///
/// \details
/// Refer to the SEHemothorax class
/// Refer to the SETubeThoracostomy class for hemothorax intervention
//--------------------------------------------------------------------------------------------------
void HowToHemothorax()
{
  // Create the engine and load the patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->LogToConsole(true);
  pe->GetLogger()->SetLogFile("./test_results/howto/HowTo_Hemothorax.cpp/HowTo_Hemothorax.log");
  pe->GetLogger()->Info("HowTo_Hemothorax");

  // Create data requests for each value that should be written to the output log as the engine is executing
  SEDataRequestManager drMgr(pe->GetLogger());
  drMgr.CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("TidalVolume", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("OxygenSaturation");
  drMgr.CreatePhysiologyDataRequest("CardiacOutput", VolumePerTimeUnit::mL_Per_min);
  drMgr.CreatePhysiologyDataRequest("BloodVolume", VolumeUnit::mL);
  drMgr.CreateActionDataRequest("LeftHemothorax", "FlowRate", VolumePerTimeUnit::mL_Per_min);
  drMgr.CreateActionDataRequest("LeftHemothorax", "TotalBloodVolume", VolumeUnit::mL);
  drMgr.SetResultsFilename("./test_results/howto/HowToHemothorax.cpp/HowTo_Hemothorax.csv");

  if (!pe->SerializeFromFile("./states/StandardMale@0s.json", &drMgr))
  {
    pe->GetLogger()->Error("Could not load state, check the error");
    return;
  }

  pe->GetLogger()->Info("The patient is nice and healthy");
  pe->GetLogger()->Info(std::stringstream() <<"Tidal Volume : " << pe->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Respiration Rate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());
  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Blood Volume : " << pe->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL) << VolumeUnit::mL);

  if (!pe->AdvanceModelTime(50, TimeUnit::s))
  {
    pe->GetLogger()->Fatal("Unable to advance engine time");
    return;
  }

  // Create a Hemothorax
  // Set the severity (a fraction between 0 and 1)
  SEHemothorax hemo;
  hemo.GetSeverity().SetValue(0.75);
  // Optionally, You can set the flow rate of the hemothorax.
  // This is implemented as a flow source, this rate will be constant, and will not be affected by dropping blood pressures
  // It is intended to interact with sensors or with something continuously monitoring physiology and updating the flow
  //hemo.GetFlowRate().SetValue(20, VolumePerTimeUnit::mL_Per_min); //the flow rate of hemothorax
  // When using using a flow rate, you can also optionally set a target volume for the hemothorax
  //hemo.GetTargetVolume().SetValue(600, VolumeUnit::mL); //the target volume of the hemothorax

  // It can be on the left or right side
  hemo.SetSide(eSide::Left);
  //hemo.SetSide(eSide::Right);
  pe->ProcessAction(hemo);

  pe->GetLogger()->Info("Giving the patient a hemothorax");

  if (!pe->AdvanceModelTime(120, TimeUnit::s))
  {
    pe->GetLogger()->Fatal("Unable to advance engine time");
    return;
  }

  pe->GetLogger()->Info("The patient has had a hemothorax for 120s");
  pe->GetLogger()->Info(std::stringstream() <<"Tidal Volume : " << pe->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Respiration Rate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());
  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Blood Volume : " << pe->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL) << VolumeUnit::mL);

  // You can get the current flow rate based on the severity, as well as the blood volume via the action in the action manager
  // Note this is a copy of hemo internal to the engine
  auto hemoInfo = pe->GetActionManager().GetPatientActions().GetLeftHemothorax();
  //auto hemoInfo = pe->GetActionManager().GetPatientActions().GetLeftHemothorax();
  pe->GetLogger()->Info(std::stringstream() <<"Hemothorax Flow Rate : " << hemoInfo->GetFlowRate(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Hemothorax Total Blood Volume : " << hemoInfo->GetTotalBloodVolume(VolumeUnit::mL) << VolumeUnit::mL);

  // Tube thoracostomy should help the patient out
  SETubeThoracostomy tubeThoracostomy;
  // Optionally, you can set the flow rate of the tube thoracostomy.
  //tubeThoracostomy.GetFlowRate().SetValue(15, VolumePerTimeUnit::mL_Per_min);

  // You can set the flow rate to 0 when you would like to remove the intervention
  tubeThoracostomy.GetFlowRate().SetValue(0, VolumePerTimeUnit::mL_Per_min);

  // It can be on the left or right side (it's a good idea to do it on the side of the hemothorax ;)
  tubeThoracostomy.SetSide(eSide::Left);
  //tubeThoracostomy.SetSide(eSide::Right);

  tubeThoracostomy.GetFlowRate().SetValue(15, VolumePerTimeUnit::mL_Per_min);
  pe->ProcessAction(tubeThoracostomy);
  pe->GetLogger()->Info("Giving the patient a tube thoracostomy");

  if (!pe->AdvanceModelTime(400, TimeUnit::s))
  {
    pe->GetLogger()->Fatal("Unable to advance engine time");
    return;
  }

  pe->GetLogger()->Info("The patient has had a chest tube for 400s");
  pe->GetLogger()->Info(std::stringstream() <<"Tidal Volume : " << pe->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Respiration Rate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());
  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Blood Volume : " << pe->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() <<"Hemothorax Flow Rate : " << hemoInfo->GetFlowRate(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Hemothorax Total Blood Volume : " << hemoInfo->GetTotalBloodVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info("Finished");
}
