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
#include "cdm/patient/actions/SEAcuteRespiratoryDistressSyndromeExacerbation.h"
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
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("TidalVolume", VolumeUnit::mL);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit::mL);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("OxygenSaturation");

  SEPatientConfiguration pc;
  pc.SetPatientFile("./patients/StandardMale.json");

  pe->InitializeEngine(pc);

  pe->GetLogger()->Info("Initial vitals");
  pe->GetLogger()->Info(std::stringstream() << "Tidal Volume : " << pe->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() << "Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Respiration Rate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());

  pe->AdvanceModelTime(30, TimeUnit::s);

  pe->GetLogger()->Info("Healthy patient");
  pe->GetLogger()->Info(std::stringstream() << "Tidal Volume : " << pe->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() << "Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Respiration Rate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());

  SEAcuteRespiratoryDistressSyndromeExacerbation ARDS;
  ARDS.GetSeverity(eLungCompartment::RightSuperiorLobeApical).SetValue(0.3);
  ARDS.GetSeverity(eLungCompartment::RightSuperiorLobePosterior).SetValue(0.3);
  ARDS.GetSeverity(eLungCompartment::RightSuperiorLobeAnterior).SetValue(0.3);
  ARDS.GetSeverity(eLungCompartment::RightMiddleLobeLateral).SetValue(0.3);
  ARDS.GetSeverity(eLungCompartment::RightMiddleLobeMedial).SetValue(0.3);
  ARDS.GetSeverity(eLungCompartment::RightInferiorLobeSuperior).SetValue(0.3);
  ARDS.GetSeverity(eLungCompartment::RightInferiorLobeMedialBasal).SetValue(0.3);
  ARDS.GetSeverity(eLungCompartment::RightInferiorLobeAnteriorBasal).SetValue(0.3);
  ARDS.GetSeverity(eLungCompartment::RightInferiorLobeLateralBasal).SetValue(0.3);
  ARDS.GetSeverity(eLungCompartment::RightInferiorLobePosteriorBasal).SetValue(0.3);
  ARDS.GetSeverity(eLungCompartment::LeftInferiorLobePosteriorBasal).SetValue(0.3);
  ARDS.GetSeverity(eLungCompartment::LeftInferiorLobeLateralBasal).SetValue(0.3);
  ARDS.GetSeverity(eLungCompartment::LeftInferiorLobeAnteromedialBasal).SetValue(0.3);
  ARDS.GetSeverity(eLungCompartment::LeftInferiorLobeSuperior).SetValue(0.3);
  ARDS.GetSeverity(eLungCompartment::LeftSuperiorLobeInferiorLingula).SetValue(0.3);
  ARDS.GetSeverity(eLungCompartment::LeftSuperiorLobeSuperiorLingula).SetValue(0.3);
  ARDS.GetSeverity(eLungCompartment::LeftSuperiorLobeAnterior).SetValue(0.3);
  ARDS.GetSeverity(eLungCompartment::LeftSuperiorLobeApicoposterior).SetValue(0.3);
  pe->ProcessAction(ARDS);

  pe->AdvanceModelTime(300, TimeUnit::s);

  pe->GetLogger()->Info("Moderate ARDS");
  pe->GetLogger()->Info(std::stringstream() << "Tidal Volume : " << pe->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() << "Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Respiration Rate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());
}
