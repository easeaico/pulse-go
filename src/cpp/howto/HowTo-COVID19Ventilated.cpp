/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "EngineHowTo.h"
#include "PulseEngine.h"

// Include the various types you will be using in your code
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/engine/SEOverrides.h"
#include "cdm/system/equipment/mechanical_ventilator/SEMechanicalVentilator.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorConfiguration.h"
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/system/physiology/SERespiratorySystem.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/substance/SESubstanceFraction.h"
#include "cdm/patient/SEPatient.h"
#include "cdm/patient/actions/SEAcuteRespiratoryDistressSyndromeExacerbation.h"
#include "cdm/patient/actions/SEDyspnea.h"
#include "cdm/patient/actions/SEIntubation.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTemperature.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerPressure.h"
#include "cdm/properties/SEScalarPressureTimePerVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalar0To1.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// COVID-19 treatment example leveraging ARDS, parameter override, and mechanical ventilator.
///
/// \details
/// Refer to the SEAirwayObstruction class
//--------------------------------------------------------------------------------------------------
void HowToCOVID19Ventilated()
{
  std::stringstream ss;
  // Create a Pulse Engine and load the standard patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("HowTo_COVID19Ventilated.log");
  
  pe->GetLogger()->Info("HowTo_COVID19Ventilated");
  if (!pe->SerializeFromFile("./states/StandardMale@0s.json"))
  {
    pe->GetLogger()->Error("Could not load state, check the error");
    return;
  }

  // Create data requests for each value that should be written to the output log as the engine is executing
  // Physiology System Names are defined on the System Objects
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("TidalVolume", VolumeUnit::mL);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit::mL);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("OxygenSaturation");

  pe->GetEngineTracker()->GetDataRequestManager().SetResultsFilename("HowToCOVID19Ventilated.csv");

  // Set moderate ARDS because COVID-19 pneumonia patients that received invasive mechanical ventilation have shown the PaO2/FiO2 ratios 
  // were consistent with the Berlin criteria of moderate-to-severe ARDS. @cite bhatraju2020covid @cite yang2020clinical
  SEAcuteRespiratoryDistressSyndromeExacerbation ARDS;
  ARDS.GetSeverity(eLungCompartment::LeftLung).SetValue(0.6);
  ARDS.GetSeverity(eLungCompartment::RightLung).SetValue(0.6);
  pe->ProcessAction(ARDS);

  // Update the total respiratory compliance, since COVID-19 patients present an atypical form of ARDS with a dissociation between 
  // well-preserved lung mechanics (i.e., compliance), when compared to the magnitude of pulmonary shunt fraction and severity of 
  // hypoxemia. @cite gattinoni2020covid @cite gattinoni2020covid2
  pe->GetLogger()->Info("Updating the patient's total respiratory static compliance to a specific constant/linear value.");
  SEOverrides overrides;
  overrides.GetScalarProperties()["RespiratoryCompliance"] = SEScalarPair(100.0, VolumePerPressureUnit::mL_Per_cmH2O);
  pe->ProcessAction(overrides);

  // Advance time enough to achieve a new pathophysiogical homeostatic state
  AdvanceAndTrackTime_s(10.0 * 60.0, *pe); // 10 min

  pe->GetLogger()->Info(std::stringstream() << "The patient has moderate COVID-19");
  pe->GetLogger()->Info(std::stringstream() << "Tidal Volume : " << pe->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() << "Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Respiration Rate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation() * 100 << "%");
  pe->GetLogger()->Info(std::stringstream() << "Pulmonary Compliance : " << pe->GetRespiratorySystem()->GetRespiratoryCompliance(VolumePerPressureUnit::mL_Per_cmH2O) << VolumePerPressureUnit::mL_Per_cmH2O);
  pe->GetLogger()->Info(std::stringstream() << "End Tidal Carbon Dioxide Pressure : " << pe->GetRespiratorySystem()->GetEndTidalCarbonDioxidePressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "P/F Ratio : " << pe->GetRespiratorySystem()->GetHorowitzIndex(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Pulmonary Shunt Fraction : " << pe->GetBloodChemistrySystem()->GetShuntFraction() * 100 << "%");
  pe->GetLogger()->Info(std::stringstream() << "Mean Airway Pressure : " << pe->GetRespiratorySystem()->GetMeanAirwayPressure(PressureUnit::cmH2O) << PressureUnit::cmH2O);
  pe->GetLogger()->Info(std::stringstream() << "Oxygen Saturation Index : " << pe->GetRespiratorySystem()->GetOxygenSaturationIndex(PressureUnit::mmHg) << PressureUnit::mmHg);


  // Mimic a neuromuscular blockade by setting apnea (dyspnea = 1)
  // Alternatively, there are drugs in the library that can be administered that stop spontaneous breathing (e.g., succinylcholine)
  SEDyspnea dyspnea;
  dyspnea.GetTidalVolumeSeverity().SetValue(1.0);
  pe->ProcessAction(dyspnea);

  // Mechanically ventilate the patient in PC-CMV mode with setting to acheive good lung recruitment, tidal volume ~6mL/kg, and SpO2 ~95%
  SEIntubation intubation;
  intubation.SetType(eIntubation_Type::Tracheal);
  pe->ProcessAction(intubation);

  // Setup the PC-CMV ventilator
  SEMechanicalVentilatorConfiguration MVConfig(pe->GetLogger());
  SEMechanicalVentilatorSettings& mv = MVConfig.GetSettings();
  mv.SetConnection(eSwitch::On);
  mv.SetInspirationWaveform(eDriverWaveform::Square);
  mv.SetExpirationWaveform(eDriverWaveform::Square);
  mv.GetPeakInspiratoryPressure().SetValue(21.0, PressureUnit::cmH2O);
  mv.GetPositiveEndExpiratoryPressure().SetValue(10.0, PressureUnit::cmH2O);
  const SESubstance* oxygen = pe->GetSubstanceManager().GetSubstance("Oxygen");
  mv.GetFractionInspiredGas(*oxygen).GetFractionAmount().SetValue(0.5);
  double respirationRate_per_min = 20.0;
  double IERatio = 0.5;

  // Translate ventilator settings
  double totalPeriod_s = 60.0 / respirationRate_per_min;
  double inspiratoryPeriod_s = IERatio * totalPeriod_s / (1 + IERatio);
  double expiratoryPeriod_s = totalPeriod_s - inspiratoryPeriod_s;
  mv.GetInspirationMachineTriggerTime().SetValue(expiratoryPeriod_s, TimeUnit::s);
  mv.GetExpirationCycleTime().SetValue(inspiratoryPeriod_s, TimeUnit::s);
  pe->ProcessAction(MVConfig);

  // Update the compliance for postive pressure ventilation, which differs from negative pressure spontaneous breathing compliance @cite arnal2018parameters
  overrides.Clear();
  overrides.GetScalarProperties()["RespiratoryCompliance"] = SEScalarPair(50.0, VolumePerPressureUnit::mL_Per_cmH2O);
  pe->ProcessAction(overrides);

  // Advance time enough to achieve a new homeostatic state
  AdvanceAndTrackTime_s(10.0 * 60.0, *pe); // 10 min

  pe->GetLogger()->Info("The patient has been successfully ventilated and stabilized");
  pe->GetLogger()->Info(std::stringstream() << "Tidal Volume : " << pe->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() << "Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Respiration Rate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation() * 100 << "%");
  pe->GetLogger()->Info(std::stringstream() << "Pulmonary Compliance : " << pe->GetRespiratorySystem()->GetRespiratoryCompliance(VolumePerPressureUnit::mL_Per_cmH2O) << VolumePerPressureUnit::mL_Per_cmH2O);
  pe->GetLogger()->Info(std::stringstream() << "End Tidal Carbon Dioxide Pressure : " << pe->GetRespiratorySystem()->GetEndTidalCarbonDioxidePressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "P/F Ratio : " << pe->GetRespiratorySystem()->GetHorowitzIndex(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Pulmonary Shunt Fraction : " << pe->GetBloodChemistrySystem()->GetShuntFraction() * 100 << "%");
  pe->GetLogger()->Info(std::stringstream() << "Mean Airway Pressure : " << pe->GetRespiratorySystem()->GetMeanAirwayPressure(PressureUnit::cmH2O) << PressureUnit::cmH2O);
  pe->GetLogger()->Info(std::stringstream() << "Oxygen Saturation Index : " << pe->GetRespiratorySystem()->GetOxygenSaturationIndex(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info("Finished");
}