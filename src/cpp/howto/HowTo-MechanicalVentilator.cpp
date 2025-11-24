/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#define _USE_MATH_DEFINES

#include "EngineHowTo.h"
#include "PulseEngine.h"

// Include the various types you will be using in your code
#include "cdm/engine/SEActionManager.h"
#include "cdm/engine/SEConditionManager.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/engine/SEEquipmentActionCollection.h"
#include "cdm/engine/SEEventManager.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceFraction.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/system/equipment/mechanical_ventilator/SEMechanicalVentilator.h"
#include "cdm/system/equipment/mechanical_ventilator/SEMechanicalVentilatorAlarms.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorConfiguration.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorHold.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorLeak.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorContinuousPositiveAirwayPressure.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorPressureControl.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorVolumeControl.h"
#include "cdm/patient/actions/SEDyspnea.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarElectricPotential.h"
#include "cdm/properties/SEScalarFrequency.h"
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
#include "cdm/properties/SEScalar0To1.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// Usage for the Mechanical Ventilator
/// Drive respiration with equipment
///
/// \details
/// Refer to the SEMechanicalVentilator class
//--------------------------------------------------------------------------------------------------
void HowToMechanicalVentilator()
{
  std::stringstream ss;
  // Create a Pulse Engine and load the standard patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/HowTo/HowTo_MechanicalVentilator.cpp/HowTo_MechanicalVentilator.log");
  pe->GetLogger()->Info("HowTo_MechanicalVentilator");

  // Vitals Monitor Data
  SEDataRequestManager drMgr(pe->GetLogger());
  drMgr.CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("TidalVolume", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("ExpiratoryRespiratoryResistance", PressureTimePerVolumeUnit::cmH2O_s_Per_L);
  drMgr.CreatePhysiologyDataRequest("InspiratoryRespiratoryResistance", PressureTimePerVolumeUnit::cmH2O_s_Per_L);
  drMgr.CreatePhysiologyDataRequest("RespiratoryCompliance", VolumePerPressureUnit::L_Per_cmH2O);
  drMgr.CreatePhysiologyDataRequest("TotalPulmonaryVentilation", VolumePerTimeUnit::L_Per_min);
  // Ventilator Monitor Data
  drMgr.CreateMechanicalVentilatorDataRequest("AirwayPressure", PressureUnit::cmH2O);
  drMgr.CreateMechanicalVentilatorDataRequest("EndTidalCarbonDioxideFraction");
  drMgr.CreateMechanicalVentilatorDataRequest("EndTidalCarbonDioxidePressure", PressureUnit::cmH2O);
  drMgr.CreateMechanicalVentilatorDataRequest("EndTidalOxygenFraction");
  drMgr.CreateMechanicalVentilatorDataRequest("EndTidalOxygenPressure", PressureUnit::cmH2O);
  drMgr.CreateMechanicalVentilatorDataRequest("ExpiratoryFlow", VolumePerTimeUnit::L_Per_s);
  drMgr.CreateMechanicalVentilatorDataRequest("ExpiratoryTidalVolume", VolumeUnit::L);
  drMgr.CreateMechanicalVentilatorDataRequest("ExtrinsicPositiveEndExpiratoryPressure", PressureUnit::cmH2O);
  drMgr.CreateMechanicalVentilatorDataRequest("InspiratoryExpiratoryRatio");
  drMgr.CreateMechanicalVentilatorDataRequest("InspiratoryFlow", VolumePerTimeUnit::L_Per_s);
  drMgr.CreateMechanicalVentilatorDataRequest("InspiratoryTidalVolume", VolumeUnit::L);
  drMgr.CreateMechanicalVentilatorDataRequest("IntrinsicPositiveEndExpiratoryPressure", PressureUnit::cmH2O);
  drMgr.CreateMechanicalVentilatorDataRequest("LeakFraction");
  drMgr.CreateMechanicalVentilatorDataRequest("MeanAirwayPressure", PressureUnit::cmH2O);
  drMgr.CreateMechanicalVentilatorDataRequest("PeakInspiratoryPressure", PressureUnit::cmH2O);
  drMgr.CreateMechanicalVentilatorDataRequest("PlateauPressure", PressureUnit::cmH2O);
  drMgr.CreateMechanicalVentilatorDataRequest("RespirationRate", FrequencyUnit::Per_min);
  drMgr.CreateMechanicalVentilatorDataRequest("TidalVolume", VolumeUnit::L);
  drMgr.CreateMechanicalVentilatorDataRequest("TotalLungVolume", VolumeUnit::L);
  drMgr.CreateMechanicalVentilatorDataRequest("TotalPositiveEndExpiratoryPressure", PressureUnit::cmH2O);
  drMgr.CreateMechanicalVentilatorDataRequest("TotalPulmonaryVentilation", VolumePerTimeUnit::L_Per_s);
  // Substances
  drMgr.CreateSubstanceDataRequest("Albuterol", "PlasmaConcentration", MassPerVolumeUnit::ug_Per_L);
  drMgr.CreateSubstanceDataRequest("Desflurane", "PlasmaConcentration", MassPerVolumeUnit::ug_Per_L);

  drMgr.SetResultsFilename("./test_results/HowTo/HowTo_MechanicalVentilator.cpp/HowTo_MechanicalVentilator.csv");


  if (!pe->SerializeFromFile("./states/StandardMale@0s.json", &drMgr))
  {
    pe->GetLogger()->Error("Could not load state, check the error");
    return;
  }

  //Dyspnea
  SEDyspnea Dyspnea;
  Dyspnea.GetTidalVolumeSeverity().SetValue(1.0);
  pe->ProcessAction(Dyspnea);
  pe->AdvanceModelTime(10.0, TimeUnit::s);
  pe->GetTrackedData().LogRequestedValues();

  // We have action support for several commonly used ventilator modes
  // Pulse is not limited to these modes, These modes are designe for simple understanding
  // Our implementation supports any ventilator mode, you will just need to translate the user facing inputs
  // to a timing/control profile using our configuration settings
  // These modes are internally converted into a configuration setting,
  // You can retrieve the resulting settings action and use it as a basis for more configurations if you want.
  // For example, if you wanted to lengthen the InspirationPatientTriggerFlow of the mode

  SEMechanicalVentilatorContinuousPositiveAirwayPressure cpap;
  cpap.SetConnection(eSwitch::On);
  cpap.SetInspirationWaveform(eDriverWaveform::AscendingRamp);
  cpap.GetFractionInspiredOxygen().SetValue(0.21);
  cpap.GetDeltaPressureSupport().SetValue(10.0, PressureUnit::cmH2O);
  cpap.GetPositiveEndExpiratoryPressure().SetValue(5.0, PressureUnit::cmH2O);
  cpap.GetSlope().SetValue(0.2, TimeUnit::s);

  cpap.GetInspirationPatientTriggerFlow().SetValue(5.0, VolumePerTimeUnit::L_Per_min);
  //Other trigger options (choose one):
  //  cpap.GetInspirationPatientTriggerPressure().SetValue(-1.0, PressureUnit::cmH2O);
  //  cpap.SetInspirationPatientTriggerRespiratoryModel(eSwitch::On);
  
  cpap.GetExpirationCycleFlow().SetValue(5.0, VolumePerTimeUnit::L_Per_min);
  //Other cycle options (choose one):
  //  cpap.GetExpirationCyclePressure().SetValue(0.000001, PressureUnit::cmH2O);
  //  cpap.SetExpirationCycleRespiratoryModel(eSwitch::On);
  
  pe->ProcessAction(cpap);
  pe->AdvanceModelTime(10.0, TimeUnit::s);
  pe->GetTrackedData().LogRequestedValues();


  SEMechanicalVentilatorPressureControl pc_ac;
  pc_ac.SetConnection(eSwitch::On);
  pc_ac.SetMode(eMechanicalVentilator_PressureControlMode::AssistedControl);
  pc_ac.SetInspirationWaveform(eDriverWaveform::Square);
  pc_ac.GetFractionInspiredOxygen().SetValue(0.21);
  pc_ac.GetInspiratoryPeriod().SetValue(1.1, TimeUnit::s); //This is optional
  pc_ac.GetInspiratoryPressure().SetValue(23.0, PressureUnit::cmH2O);
  pc_ac.GetPositiveEndExpiratoryPressure().SetValue(5.0, PressureUnit::cmH2O);
  pc_ac.GetRespirationRate().SetValue(12.0, FrequencyUnit::Per_min);
  //pc_ac.GetSlope().SetValue(0.0, TimeUnit::s); //No slope for square waveform

  pc_ac.GetInspirationPatientTriggerFlow().SetValue(5.0, VolumePerTimeUnit::L_Per_min);
  //Other trigger options (choose one):
  //  pc_ac.GetInspirationPatientTriggerPressure().SetValue(-1.0, PressureUnit::cmH2O);
  //  pc_ac.SetInspirationPatientTriggerRespiratoryModel(eSwitch::On);

  pe->ProcessAction(pc_ac);
  pe->AdvanceModelTime(10.0, TimeUnit::s);
  pe->GetTrackedData().LogRequestedValues();


  SEMechanicalVentilatorVolumeControl vc_ac;
  vc_ac.SetConnection(eSwitch::On);
  vc_ac.SetMode(eMechanicalVentilator_VolumeControlMode::AssistedControl);
  vc_ac.SetInspirationWaveform(eDriverWaveform::DescendingRamp);
  vc_ac.GetFlow().SetValue(60.0, VolumePerTimeUnit::L_Per_min);
  vc_ac.GetFractionInspiredOxygen().SetValue(0.21);
  vc_ac.GetInspiratoryPeriod().SetValue(1.0, TimeUnit::s); //This is optional
  vc_ac.GetPositiveEndExpiratoryPressure().SetValue(5.0, PressureUnit::cmH2O);
  vc_ac.GetRespirationRate().SetValue(12.0, FrequencyUnit::Per_min);
  vc_ac.GetTidalVolume().SetValue(540.0, VolumeUnit::mL);
  vc_ac.GetSlope().SetValue(0.2, TimeUnit::s); //This is optional and would be left out for square waveforms

  // Trigger options (choose one):
  //  vc_ac.GetInspirationPatientTriggerPressure().SetValue(-1.0, PressureUnit::cmH2O);
  //  vc_ac.GetInspirationPatientTriggerFlow().SetValue(5.0, VolumePerTimeUnit::L_Per_min);
  // If neither are provided, we will use the respiratory model trigger

  // Add an alarm that will cycle from inhale to exhale when it is reached
  vc_ac.GetSupplementalSettings().GetAlarms().GetHighPressureThreshold().SetValue(20.0, PressureUnit::cmH2O);
  vc_ac.GetSupplementalSettings().GetAlarms().SetHighPressureCycleOption(eSwitch::On);

  pe->ProcessAction(vc_ac);
  pe->AdvanceModelTime(10.0, TimeUnit::s);
  pe->GetTrackedData().LogRequestedValues();

  // Now add an aerosol
  const SESubstance* Albuterol = pe->GetSubstanceManager().GetSubstance("Albuterol");
  SESubstanceConcentration& concentrationAlbuterol = vc_ac.GetSupplementalSettings().GetConcentrationInspiredAerosol(*Albuterol);
  concentrationAlbuterol.GetConcentration().SetValue(1.0, MassPerVolumeUnit::mg_Per_L);

  // Now add a gas
  const SESubstance* Desflurane = pe->GetSubstanceManager().GetSubstance("Desflurane");
  SESubstanceFraction& fractionDesflurane = vc_ac.GetSupplementalSettings().GetFractionInspiredGas(*Desflurane);
  fractionDesflurane.GetFractionAmount().SetValue(0.06);

  pe->ProcessAction(vc_ac);
  pe->AdvanceModelTime(10.0, TimeUnit::s);
  pe->GetTrackedData().LogRequestedValues();

  // Here is an example of programming a custom ventilator mode
  SEMechanicalVentilatorConfiguration mv_config;
  SEMechanicalVentilatorSettings& mv = mv_config.GetSettings();
  mv.SetConnection(eSwitch::On);
  mv.SetInspirationWaveform(eDriverWaveform::Square);
  mv.SetExpirationWaveform(eDriverWaveform::Square);
  mv.GetPeakInspiratoryPressure().SetValue(21.0, PressureUnit::cmH2O);
  mv.GetPositiveEndExpiratoryPressure().SetValue(10.0, PressureUnit::cmH2O);
  const SESubstance* O2 = pe->GetSubstanceManager().GetSubstance("Oxygen");
  SESubstanceFraction& fractionFiO2 = mv.GetFractionInspiredGas(*O2);
  fractionFiO2.GetFractionAmount().SetValue(0.5);
  double respirationRate_per_min = 20.0;
  double IERatio = 0.5;
  // Translate ventilator settings
  double totalPeriod_s = 60.0 / respirationRate_per_min;
  double inspiratoryPeriod_s = IERatio * totalPeriod_s / (1 + IERatio);
  double expiratoryPeriod_s = totalPeriod_s - inspiratoryPeriod_s;
  mv.GetInspirationMachineTriggerTime().SetValue(expiratoryPeriod_s, TimeUnit::s);
  mv.GetExpirationCycleTime().SetValue(inspiratoryPeriod_s, TimeUnit::s);
  pe->ProcessAction(mv_config);
  pe->AdvanceModelTime(10.0, TimeUnit::s);
  pe->GetTrackedData().LogRequestedValues();


  // You can also perform holds
  SEMechanicalVentilatorHold hold;

  //Do an instantaneous hold
  hold.SetState(eSwitch::On);
  hold.SetAppliedRespiratoryCycle(eAppliedRespiratoryCycle::Instantaneous);
  pe->ProcessAction(hold);
  pe->AdvanceModelTime(3, TimeUnit::s);
  pe->GetTrackedData().LogRequestedValues();
  hold.SetState(eSwitch::Off);
  pe->ProcessAction(hold);

  pe->AdvanceModelTime(10, TimeUnit::s);
  pe->GetTrackedData().LogRequestedValues();

  //Activate inspiratory hold for 3s during plateau of the next inspiratory phase
  hold.SetAppliedRespiratoryCycle(eAppliedRespiratoryCycle::Inspiratory);
  hold.SetState(eSwitch::On);
  pe->ProcessAction(hold);
  //Keep advancing until the hold is applied
  while (pe->GetMechanicalVentilator()->GetBreathState() != eBreathState::InspiratoryHold)
  {
    pe->AdvanceModelTime();
  }
  //Hold for 3s
  pe->AdvanceModelTime(3, TimeUnit::s);
  pe->GetTrackedData().LogRequestedValues();
  hold.SetState(eSwitch::Off);
  pe->ProcessAction(hold);
  //Output the resulting plateau pressure at end-inspiration
  double airwayPressure_cmH2O = pe->GetMechanicalVentilator()->GetAirwayPressure(PressureUnit::cmH2O);
  ss << "Inspiratory hold plateau pressure is " << airwayPressure_cmH2O << " cmH2O";
  pe->GetLogger()->Info(ss);

  pe->AdvanceModelTime(10, TimeUnit::s);
  pe->GetTrackedData().LogRequestedValues();

  //Activate expiratory hold for 3s at the end of expiration
  hold.SetAppliedRespiratoryCycle(eAppliedRespiratoryCycle::Expiratory);
  hold.SetState(eSwitch::On);
  pe->ProcessAction(hold);
  //Keep advancing until the hold is applied
  while (pe->GetMechanicalVentilator()->GetBreathState() != eBreathState::ExpiratoryHold)
  {
    pe->AdvanceModelTime();
  }
  //Hold for 3s
  pe->AdvanceModelTime(3, TimeUnit::s);
  pe->GetTrackedData().LogRequestedValues();
  hold.SetState(eSwitch::Off);
  pe->ProcessAction(hold);
  //Output the resulting auto PEEP value
  airwayPressure_cmH2O = pe->GetMechanicalVentilator()->GetAirwayPressure(PressureUnit::cmH2O);
  ss << "Expiratory hold auto PEEP pressure is " << airwayPressure_cmH2O << " cmH2O";
  pe->GetLogger()->Info(ss);

  pe->AdvanceModelTime(10, TimeUnit::s);
  pe->GetTrackedData().LogRequestedValues();


  // A leak can be specified
  SEMechanicalVentilatorLeak leak;
  leak.GetSeverity().SetValue(0.5);
  pe->ProcessAction(leak);
  pe->AdvanceModelTime(5, TimeUnit::s);
  pe->GetTrackedData().LogRequestedValues();
  leak.GetSeverity().SetValue(0.0);// Turn off the leak
  pe->ProcessAction(leak);
  pe->AdvanceModelTime(5, TimeUnit::s);
  pe->GetTrackedData().LogRequestedValues();

  pe->GetLogger()->Info("Finished");
}
