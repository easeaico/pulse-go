/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "EngineHowTo.h"
#include "PulseEngine.h"

   // Include the various types you will be using in your code
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEDataRequestTracker.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/engine/SEConditionManager.h"
#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/compartment/fluid/SEGasCompartment.h"
#include "cdm/patient/conditions/SEPulmonaryFibrosis.h"
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
/// Usage for applying a PulmonaryFibrosis condition to the patient
///
/// \details
/// Refer to the SEPulmonaryFibrosis class
//--------------------------------------------------------------------------------------------------
void HowToPulmonaryFibrosis()
{
  // Create the engine and load the patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/howto/HowTo_PulmonaryFibrosis.cpp/HowTo_PulmonaryFibrosis.log");
  pe->GetLogger()->Info("HowTo_PulmonaryFibrosis");

  // Create data requests for each value that should be written to the output log as the engine is executing
  SEDataRequestManager drMgr(pe->GetLogger());
  drMgr.CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("CardiacOutput", VolumePerTimeUnit::mL_Per_min);
  drMgr.CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("TidalVolume", VolumeUnit::L);
  drMgr.CreatePhysiologyDataRequest("HemoglobinContent", MassUnit::g);
  drMgr.CreatePhysiologyDataRequest("InspiratoryExpiratoryRatio");
  drMgr.CreatePhysiologyDataRequest("ExpiratoryFlow", VolumePerTimeUnit::L_Per_s);
  drMgr.SetResultsFilename("./test_results/howto/HowTo_PulmonaryFibrosis.cpp/HowTo_PulmonaryFibrosis.csv");

  // Since this is a condition, we do not provide a starting state
  // You will need to initialize the engine to this patient configuration
  // You could then save out your own state and use it in the future
  SEPatientConfiguration pc;
  pc.SetPatientFile("StandardMale.json");
  pc.GetConditions().GetPulmonaryFibrosis().GetSeverity().SetValue(0.9);

  if (!pe->InitializeEngine(pc, &drMgr))
  {
    pe->GetLogger()->Error("Could not load initialize engine, check the error");
    return;
  }

  // Advance some time to get some data
  pe->AdvanceModelTime(120, TimeUnit::s);

  pe->GetLogger()->Info("The patient is not very healthy");
  pe->GetLogger()->Info(std::stringstream() << "Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() << "Hemoglobin Content : " << pe->GetBloodChemistrySystem()->GetHemoglobinContent(MassUnit::g) << MassUnit::g);
  pe->GetLogger()->Info(std::stringstream() << "Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "RespirationRate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min));
  pe->GetLogger()->Info(std::stringstream() << "InspiratoryExpiratoryRatio : " << pe->GetRespiratorySystem()->GetInspiratoryExpiratoryRatio());
  pe->GetLogger()->Info(std::stringstream() << "TidalVolume : " << pe->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::L));
  pe->GetLogger()->Info(std::stringstream() << "ExpiratoryFlow : " << pe->GetRespiratorySystem()->GetExpiratoryFlow(VolumePerTimeUnit::L_Per_s));
  pe->GetLogger()->Info("Finished");
}
