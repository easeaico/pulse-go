/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "EngineHowTo.h"
#include "PulseEngine.h"

// Include the various types you will be using in your code
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/patient/actions/SEAsthmaAttack.h"
#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/compartment/fluid/SEGasCompartment.h"
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/system/physiology/SERespiratorySystem.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTemperature.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalar0To1.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// Usage for applying an Asthma Attack insult to the patient
///
/// \details
/// Refer to the SEAsthmaAttack class
//--------------------------------------------------------------------------------------------------
void HowToAsthmaAttack() 
{
  // Create the engine and load the patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/howto/HowTo_Asthma.cpp/HowTo_Asthma.log");
  pe->GetLogger()->Info("HowTo_Asthma");

  // Create data requests for each value that should be written to the output log as the engine is executing
  SEDataRequestManager drMgr(pe->GetLogger());
  drMgr.CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("CardiacOutput", VolumePerTimeUnit::mL_Per_min);
  drMgr.CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("HemoglobinContent", MassUnit::g);
  drMgr.CreatePhysiologyDataRequest("InspiratoryExpiratoryRatio");
  drMgr.CreateGasCompartmentDataRequest(pulse::PulmonaryCompartment::Carina, "InFlow");
  drMgr.SetResultsFilename("./test_results/howto/HowTo_Asthma.cpp/HowTo_Asthma.csv");

  if (!pe->SerializeFromFile("./states/StandardMale@0s.json", &drMgr))
  {
    pe->GetLogger()->Error("Could not load state, check the error");
    return;
  }

  // Advance some time to get some healthy data
  pe->AdvanceModelTime(50, TimeUnit::s);
  // Cache off compartments of interest!
  const SEGasCompartment* carina = pe->GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::Carina);
  
  pe->GetLogger()->Info("The patient is nice and healthy");
  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Hemoglobin Content : " << pe->GetBloodChemistrySystem()->GetHemoglobinContent(MassUnit::g) << MassUnit::g);
  pe->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"InspiratoryExpiratoryRatio : " << pe->GetRespiratorySystem()->GetInspiratoryExpiratoryRatio());
  pe->GetLogger()->Info(std::stringstream() <<"Carina InFlow : " << carina->GetInFlow(VolumePerTimeUnit::L_Per_s) << VolumePerTimeUnit::L_Per_s);;

  // Asthma Attack Starts - instantiate an asthma attack action and have the engine process it
  // Asthma is a common inflammatory disease of the airways where air flow into the lungs is partially obstructed. 
  // Acute asthma is an exacerbation of asthma that does not respond to standard treatments. 
  // Pulse models asthma by increasing the airway flow resistance in the circuit model. 
  // The higher the severity, the more severe the asthma attack.
  SEAsthmaAttack asthmaAttack;
  asthmaAttack.GetSeverity().SetValue(0.3);
  pe->ProcessAction(asthmaAttack);

  pe->AdvanceModelTime(550, TimeUnit::s);

  pe->GetLogger()->Info("The patient has been having an asthma attack for 550s");
  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Hemoglobin Content : " << pe->GetBloodChemistrySystem()->GetHemoglobinContent(MassUnit::g) << MassUnit::g);
  pe->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"InspiratoryExpiratoryRatio : " << pe->GetRespiratorySystem()->GetInspiratoryExpiratoryRatio());
  pe->GetLogger()->Info(std::stringstream() <<"Carina InFlow : " << carina->GetInFlow(VolumePerTimeUnit::L_Per_s) << VolumePerTimeUnit::L_Per_s);;

  // Asthma Attack Stops
  asthmaAttack.GetSeverity().SetValue(0.0);
  pe->ProcessAction(asthmaAttack);
  
  // Advance some time while the patient catches their breath
  pe->AdvanceModelTime(200, TimeUnit::s);

  pe->GetLogger()->Info("The patient has NOT had an asthma attack for 200s");
  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Hemoglobin Content : " << pe->GetBloodChemistrySystem()->GetHemoglobinContent(MassUnit::g) << MassUnit::g);
  pe->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"InspiratoryExpiratoryRatio : " << pe->GetRespiratorySystem()->GetInspiratoryExpiratoryRatio());
  pe->GetLogger()->Info(std::stringstream() <<"Carina InFlow : " << carina->GetInFlow(VolumePerTimeUnit::L_Per_s) << VolumePerTimeUnit::L_Per_s);;
  pe->GetLogger()->Info("Finished");
}
