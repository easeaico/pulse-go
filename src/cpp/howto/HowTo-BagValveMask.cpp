/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "EngineHowTo.h"
#include "PulseEngine.h"

// Include the various types you will be using in your code
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/patient/actions/SEDyspnea.h"
#include "cdm/system/equipment/bag_valve_mask/SEBagValveMask.h"
#include "cdm/system/equipment/bag_valve_mask/actions/SEBagValveMaskConfiguration.h"
#include "cdm/system/equipment/bag_valve_mask/actions/SEBagValveMaskAutomated.h"
#include "cdm/system/equipment/bag_valve_mask/actions/SEBagValveMaskInstantaneous.h"
#include "cdm/system/equipment/bag_valve_mask/actions/SEBagValveMaskSqueeze.h"
#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/compartment/fluid/SEGasCompartment.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/substance/SESubstanceFraction.h"
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/system/physiology/SERespiratorySystem.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"


//--------------------------------------------------------------------------------------------------
/// \brief
/// Usage for applying  Bag,Valve,Mask equipment to the patient
///
/// \details
/// Refer to the SEBagValveMask class
//--------------------------------------------------------------------------------------------------
void HowToBagValveMask()
{
  // Create the engine and load the patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/HowTo_BagValveMask.log");
  pe->GetLogger()->Info("HowTo_BagValveMask");
  if (!pe->SerializeFromFile("./states/StandardMale@0s.json"))
  {
    pe->GetLogger()->Error("Could not load state, check the error");
    return;
  }

  // Create data requests for each value that should be written to the output log as the engine is executing
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("CardiacOutput", VolumePerTimeUnit::mL_Per_min);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("HemoglobinContent",MassUnit::g);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("InspiratoryExpiratoryRatio");
  pe->GetEngineTracker()->GetDataRequestManager().CreateGasCompartmentDataRequest(pulse::PulmonaryCompartment::Carina, "InFlow");

  pe->GetEngineTracker()->GetDataRequestManager().SetResultsFilename("./test_results/HowToBagValveMask.csv");
  // Cache off compartments of interest!
  const SEGasCompartment* carina = pe->GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::Carina);

  // You can apply the BVM at any point, the patient may or may not be sedated
  // For this example, the patient is not breathing
  SEDyspnea dyspnea;
  dyspnea.GetTidalVolumeSeverity().SetValue(1.0);
  pe->ProcessAction(dyspnea);

  // Using the BVM requires 2 actions
  // 1.) to put it on the patient
  SEBagValveMaskConfiguration cfg;
  // If the patient is intubated, use the Tube connection instead
  cfg.GetConfiguration().SetConnection(eSwitch::On);
  // There are several parameters that have defaults if not set
  // We will set the PEEP and FiO2
  cfg.GetConfiguration().GetValvePositiveEndExpiratoryPressure().SetValue(5, PressureUnit::cmH2O);
  const SESubstance* oxygen = pe->GetSubstanceManager().GetSubstance("Oxygen");
  cfg.GetConfiguration().GetFractionInspiredGas(*oxygen).GetFractionAmount().SetValue(0.3);
  pe->ProcessAction(cfg);

  // 2.) specify how you will work with the BVM
  // Any of the below actions (a, b, c) will remove the other
  // ex. If you are in automatic mode, then you provide a squeeze action, the automatic mode will be removed then the squeeze added
  // (The automatic mode will NOT come back on)
  
  // 2a.)This is the automatic BVM action, you specify the timing and profile you want Pulse to continuously sqeeze the bag for you
  SEBagValveMaskAutomated automatic;
  // You don't need to set anything, Pulse will use default timing/power values to continuously squeeze the bag for you
  // But you can customize 
  automatic.GetBreathFrequency().SetValue(12, FrequencyUnit::Per_min);
  automatic.GetInspiratoryExpiratoryRatio().SetValue(0.5);
  // When setting the squeeze 'profile' you can set the pressure OR the Volume
  automatic.GetSqueezePressure().SetValue(9.3, PressureUnit::cmH2O);
  //automatic.GetSqueezeVolume().SetValue(500, VolumeUnit::mL);
  pe->ProcessAction(automatic);

  // Advance some time
  AdvanceAndTrackTime_s(5, *pe);

  pe->GetLogger()->Info("The patient is nice and healthy");
  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Hemoglobin Content : " << pe->GetBloodChemistrySystem()->GetHemoglobinContent(MassUnit::g) << MassUnit::g);
  pe->GetLogger()->Info(std::stringstream() <<"Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"InspiratoryExpiratoryRatio : " << pe->GetRespiratorySystem()->GetInspiratoryExpiratoryRatio());
  pe->GetLogger()->Info(std::stringstream() <<"Carina InFlow : " << carina->GetInFlow(VolumePerTimeUnit::L_Per_s) << VolumePerTimeUnit::L_Per_s);

  // 2b.)This is the manual squeeze BVM action, you specify the profile of ONE sqeeze of the bag
  SEBagValveMaskSqueeze squeeze;
  // You don't need to set anything, Pulse will use default profile values of the squeeze  for you
  // But you can customize 
  squeeze.GetExpiratoryPeriod().SetValue(2, TimeUnit::s);
  squeeze.GetInspiratoryPeriod().SetValue(3, TimeUnit::s);
  // When setting the squeeze 'profile' you can set the pressure OR the Volume
  //squeeze.GetSqueezePressure().SetValue(15, PressureUnit::cmH2O);
  squeeze.GetSqueezeVolume().SetValue(500, VolumeUnit::mL);
  pe->ProcessAction(squeeze);

  // Advance some time (Not too much, its only one squeeze!)
  AdvanceAndTrackTime_s(10, *pe);

  pe->GetLogger()->Info("The patient is nice and healthy");
  pe->GetLogger()->Info(std::stringstream() << "Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() << "Hemoglobin Content : " << pe->GetBloodChemistrySystem()->GetHemoglobinContent(MassUnit::g) << MassUnit::g);
  pe->GetLogger()->Info(std::stringstream() << "Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "InspiratoryExpiratoryRatio : " << pe->GetRespiratorySystem()->GetInspiratoryExpiratoryRatio());
  pe->GetLogger()->Info(std::stringstream() << "Carina InFlow : " << carina->GetInFlow(VolumePerTimeUnit::L_Per_s) << VolumePerTimeUnit::L_Per_s);

  // 2c.)This is the instantaneous value of the current time step, generally this is for connecting to a hardware sensor
  SEBagValveMaskInstantaneous instantaneous;
  // When setting the instantaneous 'profile' you can set the pressure OR the flow
  instantaneous.GetFlow().SetValue(15, VolumePerTimeUnit::mL_Per_s);
  //instantaneous.GetPressure().SetValue(15, PressureUnit::cmH2O);
  pe->ProcessAction(instantaneous);

  // Advance some time (Not too much, its only inhale!)
  AdvanceAndTrackTime_s(2, *pe);

  // Set it to release the bag to zero or the PEEP to exhale
  instantaneous.GetPressure().SetValue(5, PressureUnit::cmH2O);
  pe->ProcessAction(instantaneous);

  // Advance some time (Not too much, its only exhale!)
  AdvanceAndTrackTime_s(3, *pe);

  pe->GetLogger()->Info("The patient is nice and healthy");
  pe->GetLogger()->Info(std::stringstream() << "Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() << "Hemoglobin Content : " << pe->GetBloodChemistrySystem()->GetHemoglobinContent(MassUnit::g) << MassUnit::g);
  pe->GetLogger()->Info(std::stringstream() << "Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "InspiratoryExpiratoryRatio : " << pe->GetRespiratorySystem()->GetInspiratoryExpiratoryRatio());
  pe->GetLogger()->Info(std::stringstream() << "Carina InFlow : " << carina->GetInFlow(VolumePerTimeUnit::L_Per_s) << VolumePerTimeUnit::L_Per_s);


  pe->GetLogger()->Info("Finished");
}
