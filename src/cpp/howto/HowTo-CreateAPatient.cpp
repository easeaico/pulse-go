/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "EngineHowTo.h"
#include "PulseEngine.h"

   // Include the various types you will be using in your code
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/patient/SEPatient.h"
#include "cdm/patient/conditions/SEAcuteRespiratoryDistressSyndrome.h"
#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/system/physiology/SEEnergySystem.h"
#include "cdm/system/physiology/SERespiratorySystem.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/engine/SEConditionManager.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTemperature.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEFunctionVolumeVsTime.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarLength.h"

#include "cdm/utils/FileUtils.h"

void TrackData(PhysiologyEngine& pe, const std::string& csvFilename)
{
  pe.GetLogger()->Info("Tracking data to file: " + csvFilename);
  // Let's run for 30s and collect some data to plot
  // Create data requests for each value that should be written to the output log as the engine is executing
  pe.GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  pe.GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
  pe.GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  pe.GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
  pe.GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
  pe.GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("TidalVolume", VolumeUnit::mL);
  pe.GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit::mL);
  pe.GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("BloodVolume", VolumeUnit::mL);
  pe.GetEngineTracker()->GetDataRequestManager().SetResultsFilename(csvFilename);

  // We are ready to execute the engine
  // simply tell the engine how long you would like it to execute
  if (!AdvanceAndTrackTime_s(30, pe))// Note this tracker class takes in seconds
  {
    pe.GetLogger()->Fatal("Unable to advance engine time");
    return;
  }
}

void PatientAPI(PhysiologyEngine& pe)
{
  pe.GetLogger()->Info("Creating a patient using the patient API");
  SEPatientConfiguration pc;
  SEPatient& patient = pc.GetPatient();
  patient.SetName("Patient");
  //Patient sex is the only thing that is absolutely required to be set.
  //All value not explicitly set based or standard values or calculations.
  //If you do something out of bounds or set something you're not allowed to, it will alert you with a warning/error.
  patient.SetSex(ePatient_Sex::Female);
  // Jane
  //patient.GetAge().SetValue(18, TimeUnit::yr);
  //patient.GetWeight().SetValue(81, MassUnit::lb);
  //patient.GetHeight().SetValue(60, LengthUnit::in);
  //patient.GetBodyFatFraction().SetValue(0.179);
  //patient.GetDiastolicArterialPressureBaseline().SetValue(75, PressureUnit::mmHg);
  //patient.GetHeartRateBaseline().SetValue(58, FrequencyUnit::Per_min);
  //patient.GetRespirationRateBaseline().SetValue(19.5, FrequencyUnit::Per_min);
  //patient.GetSystolicArterialPressureBaseline().SetValue(118, PressureUnit::mmHg);

  // Carol
  //patient.GetAge().SetValue(49, TimeUnit::yr);
  //patient.GetWeight().SetValue(120, MassUnit::lb);
  //patient.GetHeight().SetValue(63, LengthUnit::in);
  //patient.GetBodyFatFraction().SetValue(0.219);
  //patient.GetDiastolicArterialPressureBaseline().SetValue(75, PressureUnit::mmHg);
  //patient.GetHeartRateBaseline().SetValue(58, FrequencyUnit::Per_min);
  //patient.GetRespirationRateBaseline().SetValue(19.5, FrequencyUnit::Per_min);
  //patient.GetSystolicArterialPressureBaseline().SetValue(118, PressureUnit::mmHg);

  // You can save off the patient if you want to use it later
  patient.SerializeToFile("./test_results/howto/HowToCreateAPatient.cpp/patient.json");

  SEAcuteRespiratoryDistressSyndrome& ards = pc.GetConditions().GetAcuteRespiratoryDistressSyndrome();
  ards.GetSeverity(eLungCompartment::LeftLung).SetValue(0.9);
  ards.GetSeverity(eLungCompartment::RightLung).SetValue(0.9);

  if (!pe.InitializeEngine(pc))
  {
    pe.GetLogger()->Error("Could not load state, check the error");
    return;
  }

  // You can save off the initial patient state if you want to use it later
  pe.SerializeToFile("./states/HowToCreateAPatient@0s.json");
}

void PatientFile(PhysiologyEngine& pe, const std::string& patientFile)
{
  pe.GetLogger()->Info("Creating a patient using patient file: " + patientFile);

  SEPatientConfiguration pc;
  pc.SetPatientFile(patientFile);
  if (!pe.InitializeEngine(pc))
  {
    pe.GetLogger()->Fatal("Could stabilize this patient file");
    return;
  }
  // Check to see how different the engine is from the requested patient
  SEPatient patient(pe.GetLogger());
  patient.SerializeFromFile(patientFile);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Creating a patient
///
/// \details
/// Creating a customized patient in Pulse
//--------------------------------------------------------------------------------------------------
void HowToCreateAPatient()
{
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/howto/CreateAPatient.cpp/HowTo_CreateAPatient.log");

  // Create patients using the Pulse patient API
  //PatientAPI(*pe.get());

  // Use a patient file
  std::string base, ext;
  std::vector<std::string> patientFiles;
  ListFiles("./patients/", patientFiles, false);
  // ListFiles("./patients/Hemorrhage", patientFiles, false, "Group");
  for (std::string patientFile : patientFiles)
  {
    PatientFile(*pe.get(), patientFile);
    SplitFilenameExt(patientFile, base, ext);
    TrackData(*pe.get(), "./test_results/howto/CreateAPatient.cpp/" + base + ".csv");
  }
}
