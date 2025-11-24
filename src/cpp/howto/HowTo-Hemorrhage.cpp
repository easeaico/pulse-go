/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "EngineHowTo.h"
#include "PulseEngine.h"

// Include the various types you will be using in your code
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/engine/SEActionManager.h"
#include "cdm/engine/SEPatientActionCollection.h"
#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/patient/actions/SEHemorrhage.h"
#include "cdm/patient/actions/SESubstanceCompoundInfusion.h"
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/system/physiology/SEEnergySystem.h"
#include "cdm/system/physiology/SERespiratorySystem.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/substance/SESubstanceCompound.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTemperature.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// Usage for applying a Hemorrhage insult to the patient
///
/// \details
/// Refer to the SEHemorrhage class
/// Refer to the SESubstanceManager class
/// Refer to the SESubstanceIVFluids class for applying an IV to the patient
//--------------------------------------------------------------------------------------------------
void HowToHemorrhage()
{
  // Create the engine and load the patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->LogToConsole(true);
  pe->GetLogger()->SetLogFile("./test_results/HowTo_Hemorrhage.cpp/HowTo_Hemorrhage.log");
  pe->GetLogger()->Info("HowTo_Hemorrhage");

  // Create data requests for each value that should be written to the output log as the engine is executing
  SEDataRequestManager drMgr(pe->GetLogger());
  drMgr.CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("BloodVolume", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("CardiacOutput", VolumePerTimeUnit::mL_Per_min);
  drMgr.CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("HemoglobinContent", MassUnit::g);
  drMgr.CreatePhysiologyDataRequest("TotalHemorrhageRate", VolumePerTimeUnit::mL_Per_s);
  drMgr.CreatePhysiologyDataRequest("TotalHemorrhagedVolume", VolumeUnit::mL);
  drMgr.CreateActionCompartmentDataRequest("Hemorrhage", pulse::VascularCompartment::RightLeg, "FlowRate", VolumePerTimeUnit::mL_Per_s);
  drMgr.CreateActionCompartmentDataRequest("Hemorrhage", pulse::VascularCompartment::RightLeg, "TotalBloodLost", VolumeUnit::mL);
  drMgr.SetResultsFilename("./test_results/HowTo_Hemorrhage.cpp/HowToHemorrhage.csv");

  if (!pe->SerializeFromFile("./states/StandardMale@0s.json", &drMgr))
  {
    pe->GetLogger()->Error("Could not load state, check the error");
    return;
  }

  pe->GetLogger()->Info("The patient is nice and healthy");
  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Hemoglobin Content : " << pe->GetBloodChemistrySystem()->GetHemoglobinContent(MassUnit::g) << MassUnit::g);
  pe->GetLogger()->Info(std::stringstream() <<"Blood Volume : " << pe->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Total Hemorrhage Rate : " << pe->GetCardiovascularSystem()->GetTotalHemorrhageRate(VolumePerTimeUnit::mL_Per_s) << "mL/s");
  pe->GetLogger()->Info(std::stringstream() << "Total Hemorrhaged Volume : " << pe->GetCardiovascularSystem()->GetTotalHemorrhagedVolume(VolumeUnit::mL) << "mL");

  // Hemorrhage Starts - instantiate a hemorrhage action and have the engine process it
  SEHemorrhage hemorrhageLeg;
  hemorrhageLeg.GetSeverity().SetValue(0.8);
  hemorrhageLeg.SetCompartment(eHemorrhage_Compartment::RightLeg);
  // Optionally, You can set the flow rate of the hemorrhage,
  // This needs to be provided the proper flow rate associated with the anatomy
  // This is implemented as a flow source, this rate will be constant, and will not be affected by dropping blood pressures
  // It is intended to interact with sensors or with something continuously monitoring physiology and updating the flow
  //hemorrhageLeg.GetFlowRate().SetValue(250,VolumePerTimeUnit::mL_Per_min);//the rate of hemorrhage
  pe->ProcessAction(hemorrhageLeg);

  // You can set hemorrhages on internal organs, and also set the type for Internal or External
  // Using Internal will pool the blood in the abdominal cavity
  // Using External will let the blood flow out of the body

  // Advance some time to let the body bleed out a bit
  if(!pe->AdvanceModelTime(300, TimeUnit::s)) // Check the return of advance time, if your hemorrhage is too extreme, the engine will enter an unsolvable/irreversable state
  {
    pe->GetLogger()->Fatal("Unable to advance engine time");
    return;
  }

  // You can get the current flow rate based on the severity, as well as the total blood lost from each hemorrhage via the action in the action manager
  // Note this is a copy of hemorrhageLeg internal to the engine
  auto hemorrhages = pe->GetActionManager().GetPatientActions().GetHemorrhages();
  for (auto h : hemorrhages)
    std::cout << *h << "\n";


  pe->GetLogger()->Info("The patient has been hemorrhaging for 300s");
  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Hemoglobin Content : " << pe->GetBloodChemistrySystem()->GetHemoglobinContent(MassUnit::g) << MassUnit::g);
  pe->GetLogger()->Info(std::stringstream() <<"Blood Volume : " << pe->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Total Hemorrhage Rate : " << pe->GetCardiovascularSystem()->GetTotalHemorrhageRate(VolumePerTimeUnit::mL_Per_s) << "mL/s");
  pe->GetLogger()->Info(std::stringstream() << "Total Hemorrhaged Volume : " << pe->GetCardiovascularSystem()->GetTotalHemorrhagedVolume(VolumeUnit::mL) << "mL");

  // Hemorrhage is sealed
  hemorrhageLeg.SetCompartment(eHemorrhage_Compartment::RightLeg);//location of hemorrhage
  //hemorrhageLeg.GetSeverity().SetValue(0.);// Stop the hemorrhage
  // You can switch between severity and flow if you want,
  // But you will need to invalidate the severity for the flow to be used
  // If provided both severity and flow, Pulse will use the severity
  hemorrhageLeg.GetSeverity().Invalidate();
  hemorrhageLeg.GetFlowRate().SetValue(0,VolumePerTimeUnit::mL_Per_min);//rate is set to 0 to close the bleed
  pe->ProcessAction(hemorrhageLeg);
  
  
  // Advance some time while the medic gets the drugs ready
  if(!pe->AdvanceModelTime(100, TimeUnit::s))
  {
    pe->GetLogger()->Fatal("Unable to advance engine time");
    return;
  }

  pe->GetLogger()->Info("The patient has NOT been hemorrhaging for 100s");
  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Hemoglobin Content : " << pe->GetBloodChemistrySystem()->GetHemoglobinContent(MassUnit::g) << MassUnit::g);
  pe->GetLogger()->Info(std::stringstream() <<"Blood Volume : " << pe->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Total Hemorrhage Rate : " << pe->GetCardiovascularSystem()->GetTotalHemorrhageRate(VolumePerTimeUnit::mL_Per_s) << "mL/s");
  pe->GetLogger()->Info(std::stringstream() << "Total Hemorrhaged Volume : " << pe->GetCardiovascularSystem()->GetTotalHemorrhagedVolume(VolumeUnit::mL) << "mL");

  // Patient is stabilizing, but not great

  // Let's administer a saline drip, we need to get saline from the substance maganer
  const SESubstanceCompound* saline = pe->GetSubstanceManager().GetCompound("Saline");
  SESubstanceCompoundInfusion iVSaline(*saline);
  iVSaline.GetBagVolume().SetValue(500,VolumeUnit::mL);//the total volume in the bag of Saline
  iVSaline.GetRate().SetValue(100,VolumePerTimeUnit::mL_Per_min);//The rate to admnister the compound in the bag in this case saline
  pe->ProcessAction(iVSaline);

  if (!pe->AdvanceModelTime(400, TimeUnit::s))
  {
    pe->GetLogger()->Fatal("Unable to advance engine time");
    return;
  }

  pe->GetLogger()->Info("The patient has been getting fluids for the past 400s");
  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Hemoglobin Content : " << pe->GetBloodChemistrySystem()->GetHemoglobinContent(MassUnit::g) << MassUnit::g);
  pe->GetLogger()->Info(std::stringstream() <<"Blood Volume : " << pe->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Total Hemorrhage Rate : " << pe->GetCardiovascularSystem()->GetTotalHemorrhageRate(VolumePerTimeUnit::mL_Per_s) << "mL/s");
  pe->GetLogger()->Info(std::stringstream() << "Total Hemorrhaged Volume : " << pe->GetCardiovascularSystem()->GetTotalHemorrhagedVolume(VolumeUnit::mL) << "mL");
  pe->GetLogger()->Info("Finished");
}
