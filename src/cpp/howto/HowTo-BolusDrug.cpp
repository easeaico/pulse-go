/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "EngineHowTo.h"
#include "PulseEngine.h"

// Include the various types you will be using in your code
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEDataRequestTracker.h"
#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/patient/actions/SESubstanceBolus.h"
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/system/physiology/SEEnergySystem.h"
#include "cdm/system/physiology/SERespiratorySystem.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/patient/SEPatient.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTemperature.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// Usage for adminstering a substance to the patient via a bolus injection
///
/// \details
/// Refer to the SESubstanceBolus class
/// Refer to the SESubstanceManager class
//--------------------------------------------------------------------------------------------------
void HowToBolusDrug()
{
  // Create the engine and load the patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/howto/HowTo_BolusDrug.cpp/HowTo_BolusDrug.log");
  pe->GetLogger()->Info("HowTo_BolusDrug");

  // Create data requests for each value that should be written to the output log as the engine is executing
  SEDataRequestManager drMgr(pe->GetLogger());
  drMgr.CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("CardiacOutput", VolumePerTimeUnit::mL_Per_min);
  drMgr.CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("TidalVolume", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("NeuromuscularBlockLevel");
  drMgr.SetResultsFilename("./test_results/howto/HowTo_BolusDrug.cpp/HowTo_BolusDrug.csv");

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
  pe->GetLogger()->Info(std::stringstream() <<"Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());;

  pe->AdvanceModelTime(50, TimeUnit::s);

  // Get the Succinylcholine substance from the substance manager
  const SESubstance* succs = pe->GetSubstanceManager().GetSubstance("Succinylcholine");

  // Create a substance bolus action to administer the substance
  SESubstanceBolus bolus(*succs);
  bolus.GetConcentration().SetValue(4820,MassPerVolumeUnit::ug_Per_mL);
  bolus.GetDose().SetValue(20,VolumeUnit::mL);
  bolus.SetAdminRoute(eSubstanceAdministration_Route::Intravenous);
  // Pulse also supports Intramuscular as an admin route as well
  pe->ProcessAction(bolus);
  pe->GetLogger()->Info("Giving the patient Succinylcholine.");

  pe->AdvanceModelTime(200, TimeUnit::s);

  pe->GetLogger()->Info("It has been 200s since the administration, not doing well...");
  pe->GetLogger()->Info(std::stringstream() <<"Tidal Volume : " << pe->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Respiration Rate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());;
  pe->GetLogger()->Info("Finished");
}