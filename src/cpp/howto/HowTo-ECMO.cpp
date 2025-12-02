/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#define _USE_MATH_DEFINES

#include "EngineHowTo.h"
#include "PulseEngine.h"

// Include the various types you will be using in your code
#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/compartment/fluid/SELiquidCompartment.h"
#include "cdm/engine/SEConditionManager.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEDataRequestTracker.h"
#include "cdm/engine/SEEventManager.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/patient/SEPatient.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceFraction.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/system/equipment/ecmo/SEECMO.h"
#include "cdm/system/equipment/ecmo/actions/SEECMOConfiguration.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerAmount.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarPressurePerVolume.h"
#include "cdm/properties/SEScalarPressureTimePerVolume.h"
#include "cdm/properties/SEScalarPressureTimePerVolumeArea.h"
#include "cdm/properties/SEScalarTemperature.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerPressure.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalarVolumePerTimeArea.h"
#include "cdm/properties/SEScalarLengthPerTime.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// Usage for the ECMO
///
/// \details
/// Refer to the SEECMO class
//--------------------------------------------------------------------------------------------------
void HowToECMO()
{
  //Note: Setting circuit values (resistance/compliances/etc.) needs to be done in the engine code - they currently are not directly exposed
  
  std::stringstream ss;
  // Create a Pulse Engine and load the standard patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/HowTo/HowTo_ECMO.cpp/HowTo_ECMO.log");
  
  // Patient Data
  SEDataRequestManager drMgr(pe->GetLogger());
  drMgr.CreatePhysiologyDataRequest("BloodPH");
  drMgr.CreatePhysiologyDataRequest("BloodVolume", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("CarbonDioxideSaturation", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("CardiacOutput", VolumePerTimeUnit::L_Per_min);
  drMgr.CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("EndTidalCarbonDioxidePressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("Hematocrit");
  drMgr.CreatePhysiologyDataRequest("OxygenSaturation");
  drMgr.CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("TidalVolume", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("TotalPulmonaryVentilation", VolumePerTimeUnit::L_Per_min);
  drMgr.CreateSubstanceDataRequest("CarbonDioxide", "AlveolarTransfer", VolumePerTimeUnit::mL_Per_s);
  drMgr.CreateSubstanceDataRequest("Oxygen", "AlveolarTransfer", VolumePerTimeUnit::mL_Per_s);
 
  drMgr.CreateSubstanceDataRequest("Albumin", "BloodConcentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateSubstanceDataRequest("Bicarbonate", "BloodConcentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateSubstanceDataRequest("Calcium", "BloodConcentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateSubstanceDataRequest("CarbonDioxide", "BloodConcentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateSubstanceDataRequest("Chloride", "BloodConcentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateSubstanceDataRequest("Creatinine", "BloodConcentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateSubstanceDataRequest("Glucose", "BloodConcentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateSubstanceDataRequest("Lactate", "BloodConcentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateSubstanceDataRequest("Oxygen", "BloodConcentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateSubstanceDataRequest("Potassium", "BloodConcentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateSubstanceDataRequest("Sodium", "BloodConcentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateSubstanceDataRequest("Urea", "BloodConcentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::Aorta, "CarbonDioxide", "PartialPressure", PressureUnit::mmHg);
  drMgr.CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::Aorta, "Oxygen", "PartialPressure", PressureUnit::mmHg);
   // ECMO Data
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::BloodSamplingPort, "InFlow");
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::BloodSamplingPort, "OutFlow");
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::BloodSamplingPort, "Volume", VolumeUnit::mL);
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::BloodSamplingPort, "PH");
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::BloodSamplingPort, "Albumin", "Concentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::BloodSamplingPort, "Bicarbonate", "Concentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::BloodSamplingPort, "Calcium", "Concentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::BloodSamplingPort, "CarbonDioxide", "Concentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::BloodSamplingPort, "Chloride", "Concentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::BloodSamplingPort, "Creatinine", "Concentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::BloodSamplingPort, "Glucose", "Concentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::BloodSamplingPort, "Lactate", "Concentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::BloodSamplingPort, "Oxygen", "Concentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::BloodSamplingPort, "Potassium", "Concentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::BloodSamplingPort, "Sodium", "Concentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::BloodSamplingPort, "Urea", "Concentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::BloodSamplingPort, "CarbonDioxide", "PartialPressure", PressureUnit::mmHg);
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::BloodSamplingPort, "Oxygen", "PartialPressure", PressureUnit::mmHg);
   // Various Hb concentrations
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::BloodSamplingPort, "Hemoglobin", "Concentration", MassPerVolumeUnit::g_Per_L);
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::BloodSamplingPort, "Carbaminohemoglobin", "Concentration", MassPerVolumeUnit::g_Per_L);
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::BloodSamplingPort, "Carboxyhemoglobin", "Concentration", MassPerVolumeUnit::g_Per_L);
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::BloodSamplingPort, "OxyCarbaminohemoglobin", "Concentration", MassPerVolumeUnit::g_Per_L);
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::BloodSamplingPort, "Oxyhemoglobin", "Concentration", MassPerVolumeUnit::g_Per_L);
 
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::Oxygenator, "InFlow");
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::Oxygenator, "OutFlow");
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::Oxygenator, "Volume", VolumeUnit::mL);
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::Oxygenator, "Sodium", "Concentration", MassPerVolumeUnit::g_Per_mL);
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::Oxygenator, "Oxygen", "Concentration", MassPerVolumeUnit::g_Per_L);
  drMgr.CreateLiquidCompartmentDataRequest(pulse::ECMOCompartment::Oxygenator, "CarbonDioxide", "Concentration", MassPerVolumeUnit::g_Per_L);
 
  drMgr.SetResultsFilename("./test_results/HowTo/HowTo_ECMO.cpp/HowTo_ECMO.csv");

  if (!pe->SerializeFromFile("./states/StandardMale@0s.json", &drMgr))
  {
    pe->GetLogger()->Error("Unable to load initial state file");
    return;
  }

  // Let's run for 30s to get a normal baseline
  pe->AdvanceModelTime(3.0, TimeUnit::s);
  pe->GetDataRequestTracker().LogRequestedValues();

  // Start the ECMO with blood
  // We are NOT changing any values
  SEECMOConfiguration cfg(pe->GetLogger());
  SEECMOSettings& settings = cfg.GetSettings();
  settings.SetInflowLocation(eECMO_CannulationLocation::InternalJugular);
  settings.SetOutflowLocation(eECMO_CannulationLocation::InternalJugular);
  settings.GetOxygenatorVolume().SetValue(500, VolumeUnit::mL);
  settings.GetTransfusionFlow().SetValue(250, VolumePerTimeUnit::mL_Per_min);
  const SESubstanceCompound* blood = pe->GetSubstanceManager().GetCompound("Blood");
  settings.SetSubstanceCompound(*blood);
  pe->ProcessAction(cfg);

  // Let's run for 30s to get another baseline
  pe->AdvanceModelTime(3.0, TimeUnit::s);
  pe->GetDataRequestTracker().LogRequestedValues();

  // Ok, now lets setup a new substance therapy

  const SESubstance* HCO3 = pe->GetSubstanceManager().GetSubstance("Bicarbonate");
  const SESubstance* Na = pe->GetSubstanceManager().GetSubstance("Sodium");
  const SESubstance* K = pe->GetSubstanceManager().GetSubstance("Potassium");
  const SESubstance* Ca = pe->GetSubstanceManager().GetSubstance("Calcium");
  //const SESubstance* H = pe->GetSubstanceManager().GetSubstance("Hydrogen");
  const SESubstance* Cl = pe->GetSubstanceManager().GetSubstance("Chloride");

  const SELiquidCompartment* bsp = pe->GetCompartments().GetLiquidCompartment(pulse::ECMOCompartment::Oxygenator);
  SESubstanceQuantity* bsp_HCO3 = bsp->GetSubstanceQuantity(*HCO3);
  SESubstanceQuantity* bsp_Na = bsp->GetSubstanceQuantity(*Na);
  SESubstanceQuantity* bsp_K = bsp->GetSubstanceQuantity(*K);
  SESubstanceQuantity* bsp_Ca = bsp->GetSubstanceQuantity(*Ca);
  //SESubstanceQuantity* bsp_H = bsp->GetSubstanceQuantity(*H);
  SESubstanceQuantity* bsp_Cl = bsp->GetSubstanceQuantity(*Cl);

  double bsp_HCO3_mM = bsp->GetSubstanceQuantity(*HCO3)->GetMass(MassUnit::g) / HCO3->GetMolarMass(MassPerAmountUnit::g_Per_mmol);
  double bsp_Na_mM   = bsp->GetSubstanceQuantity(*Na)->GetMass(MassUnit::g) / Na->GetMolarMass(MassPerAmountUnit::g_Per_mmol);
  double bsp_K_mM    = bsp->GetSubstanceQuantity(*K)->GetMass(MassUnit::g) / K->GetMolarMass(MassPerAmountUnit::g_Per_mmol);
  double bsp_Ca_mM   = bsp->GetSubstanceQuantity(*Ca)->GetMass(MassUnit::g) / Ca->GetMolarMass(MassPerAmountUnit::g_Per_mmol);
  //double bsp_H_mM    = bsp->GetSubstanceQuantity(*H)->GetMass(MassUnit::g) / H->GetMolarMass(MassPerAmountUnit::g_Per_mmol);
  double bsp_Cl_mM   = bsp->GetSubstanceQuantity(*Cl)->GetMass(MassUnit::g) / Cl->GetMolarMass(MassPerAmountUnit::g_Per_mmol);

  double new_HCO3_g_Per_mL = bsp_HCO3_mM - 20;
  double new_Na_g_Per_mL = bsp_Na_mM - 5;
  double new_K_g_Per_mL = bsp_K_mM - 0.1;
  double new_Ca_g_Per_mL = bsp_Ca_mM - 0.01;
  //double new_H_g_Per_mL = bsp_H_mM - 3e-8;
  double new_Cl_g_Per_mL = bsp_Cl_mM - 20;

  settings.GetSubstanceConcentration(*HCO3).GetConcentration().SetValue(new_HCO3_g_Per_mL, MassPerVolumeUnit::g_Per_mL);
  settings.GetSubstanceConcentration(*HCO3).GetConcentration().SetValue(new_Na_g_Per_mL, MassPerVolumeUnit::g_Per_mL);
  settings.GetSubstanceConcentration(*HCO3).GetConcentration().SetValue(new_K_g_Per_mL, MassPerVolumeUnit::g_Per_mL);
  settings.GetSubstanceConcentration(*HCO3).GetConcentration().SetValue(new_Ca_g_Per_mL, MassPerVolumeUnit::g_Per_mL);
  //settings.GetSubstanceConcentration(*HCO3).GetConcentration().SetValue(new_H_g_Per_mL, MassPerVolumeUnit::g_Per_mL);
  settings.GetSubstanceConcentration(*HCO3).GetConcentration().SetValue(new_Cl_g_Per_mL, MassPerVolumeUnit::g_Per_mL);
  pe->ProcessAction(cfg);

  pe->AdvanceModelTime(30.0, TimeUnit::s);
  pe->GetDataRequestTracker().LogRequestedValues();

  pe->GetLogger()->Info("Finished");
}
