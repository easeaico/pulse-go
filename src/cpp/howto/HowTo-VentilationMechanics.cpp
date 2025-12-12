/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#include "EngineHowTo.h"
#include "PulseEngine.h"

#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEDataRequestTracker.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/patient/SEPatient.h"
#include "cdm/patient/actions/SERespiratoryMechanicsConfiguration.h"
#include "cdm/patient/actions/SEIntubation.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceFraction.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/system/equipment/mechanical_ventilator/SEMechanicalVentilator.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorPressureControl.h"
#include "cdm/system/physiology/SERespiratorySystem.h"
#include "cdm/system/physiology/SERespiratoryMechanics.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarPressurePerVolume.h"
#include "cdm/properties/SEScalarPressureTimePerVolume.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalarVolumePerPressure.h"
#include "cdm/properties/SECurve.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// Example for running the Human Adult Ventilation Mechanics Engine
//--------------------------------------------------------------------------------------------------
void HowToVentilationMechanics()
{
  std::stringstream ss;
  // Create a Pulse Engine and load the standard patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine(eModelType::HumanAdultVentilationMechanics);
  pe->GetLogger()->SetLogFile("./test_results/howto/HowTo_VentilationMechanics.cpp/HowTo_VentilationMechanics.log");
  pe->GetLogger()->Info("HowToVentilationMechanics");

  // Create data requests for each value that should be written to the output log as the engine is executing
  SEDataRequestManager drMgr(pe->GetLogger());
  drMgr.CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("TidalVolume", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("InspiratoryExpiratoryRatio");
  drMgr.CreatePhysiologyDataRequest("MeanAirwayPressure", PressureUnit::cmH2O);
  drMgr.CreatePhysiologyDataRequest("TransrespiratoryPressure", PressureUnit::cmH2O);
  drMgr.CreatePhysiologyDataRequest("IntrapulmonaryPressure", PressureUnit::cmH2O);
  drMgr.CreatePhysiologyDataRequest("InspiratoryFlow", VolumePerTimeUnit::L_Per_min);
  drMgr.CreatePhysiologyDataRequest("ExpiratoryRespiratoryResistance", PressureTimePerVolumeUnit::cmH2O_s_Per_mL);
  drMgr.CreatePhysiologyDataRequest("InspiratoryRespiratoryResistance", PressureTimePerVolumeUnit::cmH2O_s_Per_mL);
  drMgr.CreatePhysiologyDataRequest("RespiratoryCompliance", VolumePerPressureUnit::mL_Per_cmH2O);
  drMgr.CreatePhysiologyDataRequest("TotalPulmonaryVentilation", VolumePerTimeUnit::mL_Per_min);
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
  drMgr.SetResultsFilename("./test_results/howto/HowTo_VentilationMechanics.cpp/HowTo_VentilationMechanics.csv");

  // You only need to set a sex
  SEPatientConfiguration pCfg(pe->GetLogger());
  pCfg.GetPatient().SetSex(ePatient_Sex::Male);

  // State serialization is not needed, this initializes instantaneously
  pe->InitializeEngine(pCfg, &drMgr);

  // Since this engine is just the ventilation mechanics
  // You configure the patient respiratory via the RespiratoryMechanics structure
  SERespiratoryMechanicsConfiguration config;
  SERespiratoryMechanics& mechanics = config.GetSettings();
  mechanics.SetActive(eSwitch::On);
  // Create a mechanics profile
  double resistance_cmH2O_s_Per_L = 13.0;
  double compliance_mL_Per_cmH2O = 50.0;
  double musclePressure_cmH2O = 13.0;
  double respirationRate_bpm = 15.0;
  double inspiratoryTime_s = 0.9;

  // These need to add to 1.0 (100%)
  const double UpperResistanceFraction = 0.5;
  const double BronchiResistanceFraction = 0.3;
  const double AlveoliDuctResistanceFraction = 0.2;

  double upperResistance = resistance_cmH2O_s_Per_L - (BronchiResistanceFraction * resistance_cmH2O_s_Per_L + AlveoliDuctResistanceFraction * resistance_cmH2O_s_Per_L) / 2;
  double bronchiResistance = 2 * (resistance_cmH2O_s_Per_L - upperResistance) - AlveoliDuctResistanceFraction * resistance_cmH2O_s_Per_L;
  double alveoliDuctResistance = 2 * (resistance_cmH2O_s_Per_L - upperResistance) - bronchiResistance;
  double sideResistance = bronchiResistance + alveoliDuctResistance;

  double RespiratorySideCompliance_L_Per_cmH2O = compliance_mL_Per_cmH2O / 2.0;
  double LungCompliance_L_Per_cmH2O = 2.0 * RespiratorySideCompliance_L_Per_cmH2O;
  double ChestWallCompliance_L_Per_cmH2O = LungCompliance_L_Per_cmH2O;

  double totalBreathTime_s = 1.0 / (respirationRate_bpm / 60.0);
  double inspiratoryFraction = inspiratoryTime_s / totalBreathTime_s;
  double expiratoryFraction = 1.0 - inspiratoryFraction;

  double InspiratoryRiseFraction = inspiratoryFraction;
  double InspiratoryHoldFraction = 0.0;
  double InspiratoryReleaseFraction = MIN(inspiratoryFraction, expiratoryFraction * 0.5);
  double InspiratoryToExpiratoryPauseFraction = 1.0 - InspiratoryRiseFraction - InspiratoryReleaseFraction;
  double ExpiratoryRiseFraction = 0.0;
  double ExpiratoryHoldFraction = 0.0;
  double ExpiratoryReleaseFraction = 0.0;

  mechanics.GetUpperInspiratoryResistance().SetValue(upperResistance, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
  mechanics.GetUpperExpiratoryResistance().SetValue(upperResistance, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
  mechanics.GetLeftInspiratoryResistance().SetValue(sideResistance, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
  mechanics.GetLeftExpiratoryResistance().SetValue(sideResistance, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
  mechanics.GetRightInspiratoryResistance().SetValue(sideResistance, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
  mechanics.GetRightExpiratoryResistance().SetValue(sideResistance, PressureTimePerVolumeUnit::cmH2O_s_Per_L);

  SECurve& lcc = mechanics.GetLeftComplianceCurve();
  SESegmentConstant& lc = lcc.AddConstantSegment();
  lc.GetBeginVolume().SetValue(-std::numeric_limits<double>::infinity(), VolumeUnit::mL);
  lc.GetEndVolume().SetValue(std::numeric_limits<double>::infinity(), VolumeUnit::mL);
  lc.GetCompliance().SetValue(RespiratorySideCompliance_L_Per_cmH2O, VolumePerPressureUnit::mL_Per_cmH2O);
  SECurve& rcc = mechanics.GetRightComplianceCurve();
  SESegmentConstant& rc = rcc.AddConstantSegment();
  rc.GetBeginVolume().SetValue(-std::numeric_limits<double>::infinity(), VolumeUnit::mL);
  rc.GetEndVolume().SetValue(std::numeric_limits<double>::infinity(), VolumeUnit::mL);
  rc.GetCompliance().SetValue(RespiratorySideCompliance_L_Per_cmH2O, VolumePerPressureUnit::mL_Per_cmH2O);

  mechanics.GetInspiratoryRiseTime().SetValue(InspiratoryRiseFraction * totalBreathTime_s, TimeUnit::s);
  mechanics.GetInspiratoryHoldTime().SetValue(InspiratoryHoldFraction * totalBreathTime_s, TimeUnit::s);
  mechanics.GetInspiratoryReleaseTime().SetValue(InspiratoryReleaseFraction * totalBreathTime_s, TimeUnit::s);
  mechanics.GetInspiratoryToExpiratoryPauseTime().SetValue(InspiratoryToExpiratoryPauseFraction * totalBreathTime_s, TimeUnit::s);
  mechanics.GetExpiratoryRiseTime().SetValue(ExpiratoryRiseFraction * totalBreathTime_s, TimeUnit::s);
  mechanics.GetExpiratoryHoldTime().SetValue(ExpiratoryHoldFraction * totalBreathTime_s, TimeUnit::s);
  mechanics.GetExpiratoryReleaseTime().SetValue(ExpiratoryReleaseFraction * totalBreathTime_s, TimeUnit::s);
  mechanics.GetInspiratoryPeakPressure().SetValue(-13.0, PressureUnit::cmH2O);
  mechanics.GetExpiratoryPeakPressure().SetValue(0.0, PressureUnit::cmH2O);
  pe->ProcessAction(config);

  // Intubate
  SEIntubation intubation;
  intubation.SetType(eIntubation_Type::Tracheal);
  pe->ProcessAction(intubation);

  // Setup the ventilator
  SEMechanicalVentilatorPressureControl pc_ac;
  pc_ac.SetConnection(eSwitch::On);
  pc_ac.SetMode(eMechanicalVentilator_PressureControlMode::AssistedControl);
  pc_ac.GetFractionInspiredOxygen().SetValue(0.21);
  pc_ac.GetInspiratoryPeriod().SetValue(1.0, TimeUnit::s);
  pc_ac.GetInspiratoryPressure().SetValue(19.0, PressureUnit::cmH2O);
  pc_ac.GetPositiveEndExpiratoryPressure().SetValue(5.0, PressureUnit::cmH2O);
  pc_ac.GetRespirationRate().SetValue(12.0, FrequencyUnit::Per_min);
  pc_ac.GetSlope().SetValue(0.2, TimeUnit::s);
  pe->ProcessAction(pc_ac);

  for (int i = 0; i < 12; i++)
  {
    pe->AdvanceModelTime(10.0, TimeUnit::s);
    pe->GetDataRequestTracker().LogRequestedValues();
  }
}
