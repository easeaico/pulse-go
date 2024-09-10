/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#define _USE_MATH_DEFINES

#include "EngineHowTo.h"
#include "PulseEngine.h"

// Include the various types you will be using in your code
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/engine/SEEventManager.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceFraction.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/system/physiology/SERespiratorySystem.h"
#include "cdm/patient/SEPatient.h"
#include "cdm/patient/actions/SEMechanicalVentilation.h"
#include "cdm/patient/actions/SEAcuteRespiratoryDistressSyndromeExacerbation.h"
#include "cdm/patient/actions/SEDyspnea.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarLength.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorVolumeControl.h"

// Make a custom event handler that you can connect to your code (See EngineUse for more info)
class MechVentHandler : public Loggable, public SEEventHandler
{
public:
  MechVentHandler(Logger *logger) : Loggable(logger), SEEventHandler() { }
  void HandleEvent(eEvent type, bool active, const SEScalarTime* time = nullptr) override
  {
    switch (type)
    {
      case eEvent::IrreversibleState:
      {
        if (active)
          m_Logger->Info("Patient is in an irreversible state. Stop the simulation!");
        //else
          // Do nothing
        break;
      }
      default:
        break;// Nothing
    }
  }
};

//--------------------------------------------------------------------------------------------------
/// \brief
/// C-STARS closed-loop ventilator control demo
///
/// \details
/// Refer to the SEVentilatorFeedbackLoop class
//--------------------------------------------------------------------------------------------------
void HowToVentilatorFeedbackLoop()
{
  std::stringstream ss;
  // Create a Pulse Engine and load the standard patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/HowTo_VentilatorFeedbackLoop.log");  
  pe->GetLogger()->Info("HowTo_VentilatorFeedbackLoop");

  //Load the starting state that was saved at the end of Segment 1
  //Patient has mild ARDS
  if (!pe->SerializeFromFile("CSTARS-Scenario1.json"))
  {
    pe->GetLogger()->Error("Could not load state, loading Standard Male instead.");
    if (!pe->SerializeFromFile("./states/StandardMale@0s.json"))// Select patient
    {
      pe->GetLogger()->Error("Could not load state, initializing Standard Male instead.");
      SEPatientConfiguration pc;
      pc.SetPatientFile("StandardMale.json");
      if (!pe->InitializeEngine(pc))
      {
        pe->GetLogger()->Error("Could not load initialize engine, check the error.");
        return;
      }
    }
  }

  // Add an event listener callback
  MechVentHandler myEventHandler(pe->GetLogger());
  pe->GetEventManager().ForwardEvents(&myEventHandler);

  //Output patient information
  pe->GetLogger()->Info(std::stringstream() << "Patient Sex: " << ((pe->GetPatient().GetSex() == ePatient_Sex::Male) ? "Male" : "Female"));
  pe->GetLogger()->Info(std::stringstream() << "Patient Age: " << pe->GetPatient().GetAge(TimeUnit::yr) << TimeUnit::yr);
  pe->GetLogger()->Info(std::stringstream() << "Patient Height: " << pe->GetPatient().GetHeight(LengthUnit::ft) << LengthUnit::ft);
  pe->GetLogger()->Info(std::stringstream() << "Patient Weight: " << pe->GetPatient().GetWeight(MassUnit::lb) << MassUnit::lb);

  //Remove the virtual ventilator
  SEMechanicalVentilatorVolumeControl vc_ac;
  vc_ac.Deactivate();
  pe->ProcessAction(vc_ac);

  //Simulate one timestep to set the airway mode to free
  pe->AdvanceModelTime();

  //Segment 2 changes

  //Increase ARDS severity to moderate
  SEAcuteRespiratoryDistressSyndromeExacerbation ARDS;
  ARDS.GetSeverity(eLungCompartment::LeftLung).SetValue(0.57);
  ARDS.GetSeverity(eLungCompartment::RightLung).SetValue(0.57);
  pe->ProcessAction(ARDS);

  //Start spontaneously breathing
  SEDyspnea Dyspnea;
  Dyspnea.GetTidalVolumeSeverity().SetValue(0.0);
  Dyspnea.GetRespirationRateSeverity().SetValue(0.0);
  pe->ProcessAction(Dyspnea);

  //Mechanical Ventilation
  // Create an SEMechanicalVentilation object
  SEMechanicalVentilation mechVent;
  mechVent.SetState(eSwitch::On);// Turn it on
                      // Grab the substance fractions so we can quickly modify them
  SESubstanceFraction& O2frac = mechVent.GetGasFraction(*pe->GetSubstanceManager().GetSubstance("Oxygen"));
  SESubstanceFraction& CO2frac = mechVent.GetGasFraction(*pe->GetSubstanceManager().GetSubstance("CarbonDioxide"));
  SESubstanceFraction& N2frac = mechVent.GetGasFraction(*pe->GetSubstanceManager().GetSubstance("Nitrogen"));

  //We'll mimic inputs from real-time sensors by just driving the mechanical ventilation pressure and FiO2 using a sinusoid

  //Pressure waveform parameters
  double pressurePeriod = 3.0;
  double pressureAlpha = (2.0 * M_PI) / (pressurePeriod);
  double inputPressure_cmH2O = 0.0;
  double pressureAmplitude_cmH2O = 10.0;
  double pressureYOffset = 10.0;

  //FiO2 waveform parameters
  double O2Period = 100.0;
  double O2Alpha = (2.0 * M_PI) / (O2Period);
  double O2Fraction = 0.55;
  double O2Amplitude = 0.10;
  double O2YOffset = 0.55;

  //We're going to stop spontaneously breathing for this example, since we're not triggering
  Dyspnea;
  Dyspnea.GetTidalVolumeSeverity().SetValue(1.0);
  Dyspnea.GetRespirationRateSeverity().SetValue(1.0);
  pe->ProcessAction(Dyspnea);

  // Drive the system for 5 mins
  double timeStep_s = pe->GetTimeStep(TimeUnit::s);
  double time_s = 0.0;
  while (time_s < 300.0)
  {
    //Going to update values every second

    //Get pressure from sensor - be sure to guard against crazy readings
    //We will compute pressure waveform for this example
    //Difference from ambient pressure
    inputPressure_cmH2O = pressureYOffset + pressureAmplitude_cmH2O * sin(pressureAlpha * time_s);
    mechVent.GetPressure().SetValue(inputPressure_cmH2O, PressureUnit::cmH2O);

    //Get FiO2 from sensor
    O2Fraction = O2YOffset + O2Amplitude * sin(O2Alpha * time_s);
    double CO2Fraction = 4.0E-4;
    O2frac.GetFractionAmount().SetValue(O2Fraction);
    CO2frac.GetFractionAmount().SetValue(CO2Fraction);
    //Remaining will go to N2
    double N2Fraction = 1.0 - O2Fraction - CO2Fraction;
    N2frac.GetFractionAmount().SetValue(N2Fraction);

    //Update the input values
    pe->ProcessAction(mechVent);

    //Simulate one timestep (50 ms)
    pe->AdvanceModelTime();

    //Output some vitals to the log
    pe->GetLogger()->Info(std::stringstream() << "Tidal Volume: " << pe->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);
    pe->GetLogger()->Info(std::stringstream() << "Systolic Pressure: " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
    pe->GetLogger()->Info(std::stringstream() << "Diastolic Pressure: " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
    pe->GetLogger()->Info(std::stringstream() << "Heart Rate: " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
    pe->GetLogger()->Info(std::stringstream() << "Respiration Rate: " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
    pe->GetLogger()->Info(std::stringstream() << "Oxygen Saturation: " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());

    //Use this variable that is referenced to the FRC for the control system - you may need to interpolate for higher frequency changes
    pe->GetLogger()->Info(std::stringstream() << "Relative Total Lung Volume: " << pe->GetRespiratorySystem()->GetRelativeTotalLungVolume(VolumeUnit::mL) << VolumeUnit::mL);

    time_s += timeStep_s;
  }

  pe->GetLogger()->Info("Finished");
}