/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "EngineHowTo.h"
#include "PulseEngine.h"

// Include the various types you will be using in your code
#include "cdm/scenario/SEScenario.h"
#include "cdm/engine/SEAdvanceTime.h"
#include "engine/SEDataRequestManager.h"
#include "engine/SEDataRequestTracker.h"
#include "compartment/SECompartmentManager.h"
#include "patient/actions/SEArrhythmia.h"
#include "patient/actions/SEChestCompression.h"
#include "patient/actions/SEChestCompressionAutomated.h"
#include "patient/actions/SEChestCompressionInstantaneous.h"
#include "patient/SEPatient.h"
#include "patient/actions/SEHemorrhage.h"
#include "system/physiology/SEBloodChemistrySystem.h"
#include "system/physiology/SECardiovascularSystem.h"
#include "system/physiology/SEEnergySystem.h"
#include "system/physiology/SERespiratorySystem.h"
#include "properties/SEScalar0To1.h"
#include "properties/SEScalarForce.h"
#include "properties/SEScalarFrequency.h"
#include "properties/SEScalarLength.h"
#include "properties/SEScalarMass.h"
#include "properties/SEScalarMassPerVolume.h"
#include "properties/SEScalarPressure.h"
#include "properties/SEScalarTemperature.h"
#include "properties/SEScalarTime.h"
#include "properties/SEScalarVolume.h"
#include "properties/SEScalarVolumePerTime.h"
#include "engine/SEEventManager.h"

//--------------------------------------------------------------------------------------------------
/// \brief
/// A custom class for end users to listen to patient events
///
/// \details
/// It is possible during the course of the simulation for the patient or equipment to reach certain
/// states (hypoxia, tachycardia, etc.).  When a state change occurs, a physiology engine event
/// describing the state change is sent to the appropriate handlers.  Below is an example of a custom
/// event handler which we use to listen for patient and anesthesia machine events.
//--------------------------------------------------------------------------------------------------
class MyListener : public SEEventHandler, Loggable // We want to gain easy access to the engine logger
{
public:
  MyListener(Logger* logger) : Loggable(logger) {};
  virtual void HandleEvent(eEvent type, bool active, const SEScalarTime* time) override
  {
    if(type != eEvent::StartOfCardiacCycle && type != eEvent::StartOfExhale && type != eEvent::StartOfInhale)
      GetLogger()->Info(std::stringstream() <<"Recieved Event : " << eEvent_Name(type));
  }
};

//--------------------------------------------------------------------------------------------------
/// \brief
/// Usage for adminstering CPR to a patient
///
/// \details
/// Give patient Succinylcholine to stop heart, then give CPR
/// Refer to the SESubstanceBolus class
/// Refer to the SESubstanceManager class
/// This example also shows how to listen to patient events.
//--------------------------------------------------------------------------------------------------
void HowToCPR()
{
  SEAdvanceTime                   adv;
  SEArrhythmia                    arrhythmia;
  SEChestCompression              cpr;
  SEChestCompressionAutomated     cprA;
  SEChestCompressionInstantaneous cprI;

  SEScenario sce;

  // Create the engine and load the patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/howto/HowTo_CPR.cpp/HowTo_CPR.log");
  pe->GetLogger()->Info("HowTo_CPR");

  // Create data requests for each value that should be written to the output log as the engine is executing
  SEDataRequestManager& dMgr = sce.GetDataRequestManager();
  dMgr.CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  dMgr.CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  dMgr.CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
  dMgr.CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
  dMgr.CreatePhysiologyDataRequest("HeartStrokeVolume", VolumeUnit::mL);
  dMgr.CreatePhysiologyDataRequest("HeartEjectionFraction");
  dMgr.CreatePhysiologyDataRequest("CardiacOutput", VolumePerTimeUnit::mL_Per_min);
  dMgr.CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::Brain, "InFlow", VolumePerTimeUnit::mL_Per_min);
  dMgr.SetResultsFilename("./test_results/howto/HowTo_CPR.cpp/HowTo_CPR.csv");

  std::string stateFile = "./states/StandardMale@0s.json";
  if (!pe->SerializeFromFile(stateFile, &dMgr))
  {
    pe->GetLogger()->Error("Could not load state, check the error");
    return;
  }
  sce.SetEngineStateFile(stateFile);

  // 3 possible modes of performing CPR
  //std::string mode = "single";
  std::string mode = "instantaneous";
  //std::string mode = "automated";

  

  // This is the total amount of time that CPR will be administered in seconds
  double durationOfCPR_s = 240;

  // This is the frequency at which CPR is administered
  double compressionRate_bpm = 100;

  // This is where you specify how much force to apply to the chest. We have capped the applicable force at 550 N.
  double compressionForce_N = 500;

  // Force can also be specified as a depth.
  double compressionDepth_cm = 5.5;

  // This is the percent of time per period that the chest will be compressed e.g. if I have a 1 second period
  // (60 beats per minute) the chest will be compressed for 0.3 seconds
  double percentOn = 0.8;

  pe->GetLogger()->Info("The patient is nice and healthy");
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Stroke Volume : " << pe->GetCardiovascularSystem()->GetHeartStrokeVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Arterial Pressure : " << pe->GetCardiovascularSystem()->GetArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Ejection Fraction : " << pe->GetCardiovascularSystem()->GetHeartEjectionFraction());

  adv.GetTime().SetValue(30, TimeUnit::s);
  pe->AdvanceModelTime(adv.GetTime(TimeUnit::s), TimeUnit::s);
  sce.AddAction(adv);

  // Put the patient into cardiac arrest
  arrhythmia.SetRhythm(eHeartRhythm::Asystole);
  pe->ProcessAction(arrhythmia);
  sce.AddAction(arrhythmia);

  pe->GetLogger()->Info("Giving the patient Cardiac Arrest.");

  // Let's add a listener which will print any state changes that patient undergoes
  MyListener l(pe->GetLogger());
  pe->GetEventManager().ForwardEvents(&l);

  adv.GetTime().SetValue(30, TimeUnit::s);
  pe->AdvanceModelTime(adv.GetTime(TimeUnit::s), TimeUnit::s);
  sce.AddAction(adv);

  pe->GetLogger()->Info("It has been 30s since cardiac arrest onset, not doing well...");
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Stroke Volume : " << pe->GetCardiovascularSystem()->GetHeartStrokeVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Arterial Pressure : " << pe->GetCardiovascularSystem()->GetArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Ejection Fraction : " << pe->GetCardiovascularSystem()->GetHeartEjectionFraction());

  // After patient's heart is not beating, start doing CPR
  pe->GetLogger()->Info("Patient is in asystole. Begin performing CPR");

  double timeStep_s = pe->GetTimeStep(TimeUnit::s);

  // This mode performs CPR via single chest compression actions.
  // You specify the timing and force of a single chest compression.
  if (mode == "single")
  {  
    // The period is calculated via 1 / compressionRate.  Because the compression rate is given
    // in beats per minute it is divided by 60 to give a period in seconds.
    double pulsePeriod_s = 1.0 / (compressionRate_bpm / 60.0);
  
    // The amount of time the chest will be compressed, calculated from the period and percentOn
    double timeOn = percentOn * pulsePeriod_s;
  
    // This timer is used to keep track of how long CPR has been administered
    double timer1 = 0;
    // Boolean to determine which way to specify force for demonstration purposes.
    bool useExplicitForce = true;
    while (timer1 < durationOfCPR_s) // CPR is administered in this loop. It is time based, so after timer1 has exceeded the specified duration of CPR it will stop. set pulseState to true so that it will only exit AFTER a compression has been removed
    {
      // This calls the CPR function in the Cardiovascular system.  It sets the chest compression at the specified force.
      if (useExplicitForce)
      {
        cpr.GetForce().SetValue(compressionForce_N, ForceUnit::N);
        cpr.GetDepth().Invalidate();
      }
      else
      {
        cpr.GetForce().Invalidate();
        cpr.GetDepth().SetValue(compressionDepth_cm, LengthUnit::cm);
      }
      useExplicitForce = !useExplicitForce;
      cpr.GetCompressionPeriod().SetValue(timeOn, TimeUnit::s);
      pe->ProcessAction(cpr);
      sce.AddAction(cpr);

      // Increment timers and advance time
      adv.GetTime().SetValue(pulsePeriod_s, TimeUnit::s);
      pe->AdvanceModelTime(adv.GetTime(TimeUnit::s), TimeUnit::s);
      sce.AddAction(adv);

      timer1 += pulsePeriod_s;
    }
  }
  // This mode performs CPR by providing the instantaneous value of the current time
  // step of a compression. Generally this is for connecting to a hardware sensor.
  else if (mode == "instantaneous")
  {
    // The period is calculated via 1 / compressionRate.  Because the compression rate is given
    // in beats per minute it is divided by 60 to give a period in seconds.
    double pulsePeriod_s = 1.0 / (compressionRate_bpm / 60.0);
  
    // The amount of time the chest will be compressed, calculated from the period and percentOn
    double timeOn = percentOn * pulsePeriod_s;
  
    // This timer is used to keep track of how long CPR has been administered
    double timer1 = 0;
    double compressionTimer = 0;

    // Boolean to determine which way to specify force for demonstration purposes.
    bool useExplicitForce = true;
  
    // This boolean is used to determine if the chest is compressed or not. It starts as true
    // so the chest will be compressed on the next calculation from the engine.
    bool pulseState = true;
  
    while (timer1 < durationOfCPR_s) // CPR is administered in this loop. It is time based, so after timer1 has exceeded the specified duration of CPR it will stop. set pulseState to true so that it will only exit AFTER a compression has been removed
    {
      if (pulseState) // check if the chest is supposed to be compressed. If yes...
      {
        // Switch to a scaled force halfway through for demo purposes
        if (useExplicitForce && timer1 >= (durationOfCPR_s / 2.0))
        {
          cprI.GetForce().Invalidate();
          useExplicitForce = false;
        }

        // This calls the CPR function in the Cardiovascular system.  It sets the chest compression at the specified force.
        if(useExplicitForce)
          cprI.GetForce().SetValue(compressionForce_N, ForceUnit::N);
        else
          cprI.GetDepth().SetValue(compressionDepth_cm, LengthUnit::cm);
        pe->ProcessAction(cprI);
        sce.AddAction(cprI);
  
        // Specify that the compression has been started
        pulseState = false;
      }

      // Increment timers and advance time
      timer1 += timeStep_s;
      compressionTimer += timeStep_s;
      adv.GetTime().SetValue(timeStep_s, TimeUnit::s);
      pe->AdvanceModelTime(adv.GetTime(TimeUnit::s), TimeUnit::s);
      sce.AddAction(adv);

      if (compressionTimer >= pulsePeriod_s) // New compression
      {
        compressionTimer = 0;
        pulseState = true;
      }
      else if (compressionTimer >= timeOn) // Stop compressing
      {
        // This removes the chest compression by specifying the applied force as 0 N
        if(useExplicitForce)
          cprI.GetForce().SetValue(0, ForceUnit::N);
        else
          cprI.GetDepth().SetValue(0, LengthUnit::cm);
        pe->ProcessAction(cprI);
        sce.AddAction(cprI);
      }  
    }    
  
    // Make sure that the chest is no longer being compressed
    if((cprI.HasDepth() && cprI.GetDepth().GetValue(LengthUnit::cm) != 0) || (cprI.HasForce() && cprI.GetForce().GetValue(ForceUnit::N) != 0))
    {
      // If it is compressed, set force to 0 to turn off
      if (cprI.HasForce())
        cprI.GetForce().SetValue(0, ForceUnit::N);
      else
        cprI.GetDepth().SetValue(0, LengthUnit::cm);
      pe->ProcessAction(cprI);
      sce.AddAction(cprI);
    }
  }
  // This mode uses the automated chest compression action to perform CPR.
  // You specify the timing and force, then Pulse will continuously perform chest compressions.
  else if (mode == "automated")
  {
    // Start automated CPR using explicit force value
    cprA.GetForce().SetValue(compressionForce_N, ForceUnit::N);
    cprA.GetCompressionFrequency().SetValue(compressionRate_bpm, FrequencyUnit::Per_min);
    cprA.GetAppliedForceFraction().SetValue(percentOn);
    pe->ProcessAction(cprA);
    sce.AddAction(cprA);

    // Advance time for desired duration of CPR
    adv.GetTime().SetValue(durationOfCPR_s/2.0, TimeUnit::s);
    pe->AdvanceModelTime(adv.GetTime(TimeUnit::s), TimeUnit::s);
    sce.AddAction(adv);

    // Can also do automated CPR using a scaled force value
    cprA.GetForce().Invalidate();
    cprA.GetDepth().SetValue(compressionDepth_cm, LengthUnit::cm);
    pe->ProcessAction(cprA);
    sce.AddAction(cprA);

    // Advance time for desired duration of CPR
    adv.GetTime().SetValue(durationOfCPR_s/2.0, TimeUnit::s);
    pe->AdvanceModelTime(adv.GetTime(TimeUnit::s), TimeUnit::s);
    sce.AddAction(adv);

    // Stop automated CPR
    cprA.GetCompressionFrequency().SetValue(0, FrequencyUnit::Per_min);
    pe->ProcessAction(cprA);
    sce.AddAction(cprA);
  }

  // Restore normal sinus rhythm
  arrhythmia.SetRhythm(eHeartRhythm::NormalSinus);
  pe->ProcessAction(arrhythmia);
  sce.AddAction(arrhythmia);

  adv.GetTime().SetValue(60, TimeUnit::s);
  pe->AdvanceModelTime(adv.GetTime(TimeUnit::s), TimeUnit::s);
  sce.AddAction(adv);

  // Do one last output to show status after CPR.
  pe->GetLogger()->Info("Check on the patient's status after CPR has been performed");
  pe->GetLogger()->Info(std::stringstream() <<"Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() <<"Stroke Volume : " << pe->GetCardiovascularSystem()->GetHeartStrokeVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() <<"Cardiac Output : " << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) << VolumePerTimeUnit::mL_Per_min);
  pe->GetLogger()->Info(std::stringstream() <<"Arterial Pressure : " << pe->GetCardiovascularSystem()->GetArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() <<"Heart Ejection Fraction : " << pe->GetCardiovascularSystem()->GetHeartEjectionFraction());
  pe->GetLogger()->Info("Finished");

  sce.SerializeToFile("./test_results/howto/HowTo_CPR.cpp/HowTo_CPR.json");
}