/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "EngineHowTo.h"
#include "PulseEngine.h"
#include "PulseConfiguration.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/system/physiology/SERespiratorySystem.h"
#include "cdm/patient/actions/SEPrimaryBlastLungInjury.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTemperature.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalar0To1.h"

void HowToExpandedRespiratory()
{
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->LogToConsole(true);
  pe->GetLogger()->SetLogFile("./test_results/howto/HowTo_ExpandedRespiratory.log");

  PulseConfiguration config;
  config.UseExpandedRespiratory(eSwitch::On);
  pe->SetConfigurationOverride(&config);

  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("EndTidalCarbonDioxidePressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("HorowitzIndex", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("OxygenSaturation");
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("TidalVolume", VolumeUnit::mL);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit::mL);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("ArterialOxygenPressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("ArterialCarbonDioxidePressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreateGasCompartmentDataRequest("LeftLungPulmonary", "Volume", VolumeUnit::mL);
  pe->GetEngineTracker()->GetDataRequestManager().CreateGasCompartmentDataRequest("RightLungPulmonary", "Volume", VolumeUnit::L);
  pe->GetEngineTracker()->GetDataRequestManager().SetResultsFilename("./test_results/howto/HowTo_ExpandedRespiratory.csv");

  SEPatientConfiguration pc;
  pc.SetPatientFile("./patients/StandardMale.json");

  pe->InitializeEngine(pc);

  // Get default data at time 0s from the engine
  pe->AdvanceModelTime(0, TimeUnit::s);
  pe->GetLogger()->Info("Default data retrieved");

  pe->GetLogger()->Info("Initial data");
  pe->GetLogger()->Info(std::stringstream() << "Tidal Volume : " << pe->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() << "Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Respiration Rate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());
  pe->GetLogger()->Info(std::stringstream() << "Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Horowitz Index : " << pe->GetRespiratorySystem()->GetHorowitzIndex(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Total Lung Volume : " << pe->GetRespiratorySystem()->GetTotalLungVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() << "Arterial Oxygen Pressure : " << pe->GetBloodChemistrySystem()->GetArterialOxygenPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Arterial Carbon Dioxide Pressure : " << pe->GetBloodChemistrySystem()->GetArterialCarbonDioxidePressure(PressureUnit::mmHg) << PressureUnit::mmHg);

  // Apply the standalone UNDEX equation
  pe->GetLogger()->Info("Charge information");
  double chargeMass_lbs = 1000;
  double chargeDistance_ft = 100; //635 ~= 0.1; 211 ~= 0.3; 70 ~= 0.9
  double peakPressure = 13000 * pow(chargeMass_lbs, 0.33) / chargeDistance_ft;
  double severity = 0.0005 * peakPressure;
  severity = std::max(std::min(severity, 1.0), 0.0);

  pe->GetLogger()->Info(std::stringstream() << "Charge Mass: " << chargeMass_lbs << " lbs");
  pe->GetLogger()->Info(std::stringstream() << "Charge Distance: " << chargeDistance_ft << " ft");
  pe->GetLogger()->Info(std::stringstream() << "Peak Pressure: " << peakPressure << " mmHg");
  pe->GetLogger()->Info(std::stringstream() << "Severity: " << severity);

  pe->AdvanceModelTime(30, TimeUnit::s);
  pe->GetLogger()->Info("Data after 30s");

  SEPrimaryBlastLungInjury pbli;
  pbli.GetSeverity(eLungCompartment::RightSuperiorLobeApical).SetValue(severity);
  pbli.GetSeverity(eLungCompartment::RightSuperiorLobePosterior).SetValue(severity);
  pbli.GetSeverity(eLungCompartment::RightSuperiorLobeAnterior).SetValue(severity);
  pbli.GetSeverity(eLungCompartment::RightMiddleLobeLateral).SetValue(severity);
  pbli.GetSeverity(eLungCompartment::RightMiddleLobeMedial).SetValue(severity);
  pbli.GetSeverity(eLungCompartment::RightInferiorLobeSuperior).SetValue(severity);
  pbli.GetSeverity(eLungCompartment::RightInferiorLobeMedialBasal).SetValue(severity);
  pbli.GetSeverity(eLungCompartment::RightInferiorLobeAnteriorBasal).SetValue(severity);
  pbli.GetSeverity(eLungCompartment::RightInferiorLobeLateralBasal).SetValue(severity);
  pbli.GetSeverity(eLungCompartment::RightInferiorLobePosteriorBasal).SetValue(severity);
  pbli.GetSeverity(eLungCompartment::LeftInferiorLobePosteriorBasal).SetValue(severity);
  pbli.GetSeverity(eLungCompartment::LeftInferiorLobeLateralBasal).SetValue(severity);
  pbli.GetSeverity(eLungCompartment::LeftInferiorLobeAnteromedialBasal).SetValue(severity);
  pbli.GetSeverity(eLungCompartment::LeftInferiorLobeSuperior).SetValue(severity);
  pbli.GetSeverity(eLungCompartment::LeftSuperiorLobeInferiorLingula).SetValue(severity);
  pbli.GetSeverity(eLungCompartment::LeftSuperiorLobeSuperiorLingula).SetValue(severity);
  pbli.GetSeverity(eLungCompartment::LeftSuperiorLobeAnterior).SetValue(severity);
  pbli.GetSeverity(eLungCompartment::LeftSuperiorLobeApicoposterior).SetValue(severity);
  pe->ProcessAction(pbli);

  pe->AdvanceModelTime(15, TimeUnit::min);

  pe->GetLogger()->Info("Data after 15 min");
  pe->GetLogger()->Info(std::stringstream() << "Tidal Volume : " << pe->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() << "Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Respiration Rate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());
  pe->GetLogger()->Info(std::stringstream() << "Mean Arterial Pressure : " << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Horowitz Index : " << pe->GetRespiratorySystem()->GetHorowitzIndex(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Total Lung Volume : " << pe->GetRespiratorySystem()->GetTotalLungVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() << "Arterial Oxygen Pressure : " << pe->GetBloodChemistrySystem()->GetArterialOxygenPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Arterial Carbon Dioxide Pressure : " << pe->GetBloodChemistrySystem()->GetArterialCarbonDioxidePressure(PressureUnit::mmHg) << PressureUnit::mmHg);
}
