/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "EngineHowTo.h"
#include "PulseEngine.h"

// Include the various types you will be using in your code
#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/patient/actions/SEHemorrhage.h"
#include "cdm/patient/actions/SETensionPneumothorax.h"
#include "cdm/patient/actions/SENeedleDecompression.h"
#include "cdm/patient/actions/SESubstanceBolus.h"
#include "cdm/patient/actions/SESubstanceCompoundInfusion.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/system/physiology/SERespiratorySystem.h"
#include "cdm/system/physiology/SETissueSystem.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTemperature.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEFunctionVolumeVsTime.h"

void SetDataRequests(SEDataRequestManager& drMgr)
{
  // Create data requests for each value that should be written to the output log as the engine is executing
  drMgr.CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("OxygenSaturation");
  drMgr.CreatePhysiologyDataRequest("BloodVolume", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("HeartStrokeVolume", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("ExtravascularFluidVolume", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("ArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("CardiacOutput", VolumePerTimeUnit::L_Per_min);
  drMgr.CreatePhysiologyDataRequest("HemoglobinContent", MassUnit::g);
  drMgr.CreatePhysiologyDataRequest("CentralVenousPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("TidalVolume", VolumeUnit::mL);
}

void CreateState()
{
  // Create the engine and load the patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/HowTo_CombatMultitrauma_CreateState.cpp/HowTo_CombatMultitrauma_CreateState.log");
  pe->GetLogger()->Info("HowTo_CombatMultitrauma_CreateState");

  SEDataRequestManager drMgr(pe->GetLogger());
  SetDataRequests(drMgr);
  drMgr.SetResultsFilename("./test_results/HowTo_CombatMultitrauma_CreateState.cpp/HowTo_CombatMultitrauma_CreateState.csv");

  // Load the Soldier patient
  // You can alternatively define your own patient (see HowTo-CreateAPatient) and apply conditions (see HowTo-Pneumonia) 
  // This would require runnning stabilization rather than loading an existing state
  if (!pe->SerializeFromFile("./states/Soldier@0s.json", &drMgr))
  {
    pe->GetLogger()->Error("Could not load state, check the error");
    return;
  }

  pe->GetLogger()->Info("Patient healthy state");
  pe->GetLogger()->Info(std::stringstream() << "Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());
  pe->GetLogger()->Info(std::stringstream() << "Blood Volume : " << pe->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() << "Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Heart Stroke Volume : " << pe->GetCardiovascularSystem()->GetHeartStrokeVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() << "Extravascular Fluid Volume : " << pe->GetTissueSystem()->GetExtravascularFluidVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() << "Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() << "Hemoglobin Content : " << pe->GetBloodChemistrySystem()->GetHemoglobinContent(MassUnit::g) << MassUnit::g);
  pe->GetLogger()->Info(std::stringstream() << "Respiration Rate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Tidal Volume : " << pe->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);

  // Give insults
  SETensionPneumothorax pneumo;
  pneumo.SetType(eGate::Closed);
  pneumo.GetSeverity().SetValue(0.75);
  pneumo.SetSide(eSide::Left);
  pe->ProcessAction(pneumo);

  SEHemorrhage hemorrhageLeg;
  hemorrhageLeg.SetCompartment(eHemorrhage_Compartment::RightLeg);//the location of the hemorrhage
  hemorrhageLeg.GetSeverity().SetValue(0.8);//the severity of hemorrhage
  pe->ProcessAction(hemorrhageLeg);

  SEHemorrhage hemorrhageVenaCava;
  hemorrhageVenaCava.SetCompartment(eHemorrhage_Compartment::VenaCava);//the location of the hemorrhage
  hemorrhageVenaCava.GetSeverity().SetValue(0.2);//the severity of hemorrhage
  pe->ProcessAction(hemorrhageVenaCava);

  //Advance the engine to the point we would like to load later
  pe->AdvanceModelTime(60, TimeUnit::s);

  pe->GetLogger()->Info("Patient injured state at serialization");
  pe->GetLogger()->Info(std::stringstream() << "Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());
  pe->GetLogger()->Info(std::stringstream() << "Blood Volume : " << pe->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() << "Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Heart Stroke Volume : " << pe->GetCardiovascularSystem()->GetHeartStrokeVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() << "Extravascular Fluid Volume : " << pe->GetTissueSystem()->GetExtravascularFluidVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() << "Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() << "Hemoglobin Content : " << pe->GetBloodChemistrySystem()->GetHemoglobinContent(MassUnit::g) << MassUnit::g);
  pe->GetLogger()->Info(std::stringstream() << "Respiration Rate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Tidal Volume : " << pe->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);

  // Save this state out.
  // You an then load this state in your application
  pe->SerializeToFile("./test_results/HowTo_CombatMultitrauma_CreateState.cpp/CombatMultirauma_Initial_Injuries.json");

  pe->GetLogger()->Info("State saved");
}

void LoadState()
{
  // Create the engine and load the patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/HowTo_CombatMultitrauma_LoadState.log");
  pe->GetLogger()->Info("HowTo_CombatMultitrauma_LoadState");

  SEDataRequestManager drMgr(pe->GetLogger());
  SetDataRequests(drMgr);
  drMgr.SetResultsFilename("./test_results/HowTo_CombatMultitrauma_CreateState.cpp/HowTo_CombatMultitrauma_LoadState.csv");

  //Load the injured state we created
  if (!pe->SerializeFromFile("./test_results/HowTo_CombatMultitrauma_CreateState.cpp/CombatMultirauma_Initial_Injuries.json", &drMgr))
  {
    pe->GetLogger()->Error("Could not load state, check the error");
    return;
  }

  pe->GetLogger()->Info("State loaded");

  pe->GetLogger()->Info("Patient loaded injured state");
  pe->GetLogger()->Info(std::stringstream() << "Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());
  pe->GetLogger()->Info(std::stringstream() << "Blood Volume : " << pe->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() << "Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Heart Stroke Volume : " << pe->GetCardiovascularSystem()->GetHeartStrokeVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() << "Extravascular Fluid Volume : " << pe->GetTissueSystem()->GetExtravascularFluidVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() << "Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() << "Hemoglobin Content : " << pe->GetBloodChemistrySystem()->GetHemoglobinContent(MassUnit::g) << MassUnit::g);
  pe->GetLogger()->Info(std::stringstream() << "Respiration Rate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Tidal Volume : " << pe->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);

  // Reduce the bleeding by applying manual pressure
  // Just set the hemorrhage rates lower
  SEHemorrhage hemorrhageLeg;
  hemorrhageLeg.SetCompartment(eHemorrhage_Compartment::RightLeg);//the location of the hemorrhage
  hemorrhageLeg.GetSeverity().SetValue(0.15);//the severity of hemorrhage
  pe->ProcessAction(hemorrhageLeg);

  SEHemorrhage hemorrhageVenaCava;
  hemorrhageVenaCava.SetCompartment(eHemorrhage_Compartment::VenaCava);//the location of the hemorrhage
  hemorrhageVenaCava.GetSeverity().SetValue(0);//the severity of hemorrhage
  pe->ProcessAction(hemorrhageVenaCava);

  pe->AdvanceModelTime(60, TimeUnit::s);

  // Needle Decompression to help with pneumothorax
  SENeedleDecompression needleDecomp;
  needleDecomp.SetState(eSwitch::On);
  needleDecomp.SetSide(eSide::Left);
  pe->ProcessAction(needleDecomp);

  // Advance the engine while you prepare to treat the patient
  pe->AdvanceModelTime(60.0 * 4.0, TimeUnit::s); //4 min

  // Apply a tournaquet and stop the bleeding completely
  hemorrhageLeg.SetCompartment(eHemorrhage_Compartment::RightLeg);//the location of the hemorrhage
  hemorrhageLeg.GetSeverity().SetValue(0);//the severity of hemorrhage
  pe->ProcessAction(hemorrhageLeg);
   
  pe->AdvanceModelTime(30, TimeUnit::s);

  // Give an IV
  const SESubstanceCompound* saline = pe->GetSubstanceManager().GetCompound("Saline");
  SESubstanceCompoundInfusion iVSaline(*saline);
  iVSaline.GetBagVolume().SetValue(500, VolumeUnit::mL);//the total volume in the bag of Saline
  iVSaline.GetRate().SetValue(100, VolumePerTimeUnit::mL_Per_min);//The rate to admnister the compound in the bag in this case saline
  pe->ProcessAction(iVSaline);

  pe->AdvanceModelTime(60.0 * 2.0, TimeUnit::s); //2 min

  // Provide morphine
  const SESubstance* morphine = pe->GetSubstanceManager().GetSubstance("Morphine");
  SESubstanceBolus bolus(*morphine);
  bolus.GetConcentration().SetValue(1000, MassPerVolumeUnit::ug_Per_mL);
  bolus.GetDose().SetValue(5, VolumeUnit::mL);
  bolus.SetAdminRoute(eSubstanceAdministration_Route::Intravenous);
  pe->ProcessAction(bolus);

  pe->AdvanceModelTime(60.0 * 3.0, TimeUnit::s); //3 min
   
  pe->GetLogger()->Info("Patient state after interventions");
  pe->GetLogger()->Info(std::stringstream() << "Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());
  pe->GetLogger()->Info(std::stringstream() << "Blood Volume : " << pe->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() << "Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Heart Stroke Volume : " << pe->GetCardiovascularSystem()->GetHeartStrokeVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() << "Extravascular Fluid Volume : " << pe->GetTissueSystem()->GetExtravascularFluidVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() << "Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() << "Hemoglobin Content : " << pe->GetBloodChemistrySystem()->GetHemoglobinContent(MassUnit::g) << MassUnit::g);
  pe->GetLogger()->Info(std::stringstream() << "Respiration Rate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Tidal Volume : " << pe->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);

  // Save this state out.
  // You an then load this state in your application
  pe->SerializeToFile("./states/CombatMultirauma_Treated_Injuries.json");

  pe->GetLogger()->Info("Finished");
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Usage for serializing and loading a showcase scenario
///
/// \details
/// This follows the Combat Multitrauma showcase scenario. Validation is included in the system
/// methodology document.
///
//--------------------------------------------------------------------------------------------------
void HowToCombatMultitrauma()
{
  // Store the injured patient state for loading later
  CreateState();

  // Load the stored injured state and perform interventions
  LoadState();
}
