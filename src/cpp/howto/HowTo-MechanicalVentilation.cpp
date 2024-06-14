/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#define _USE_MATH_DEFINES

#include "EngineHowTo.h"
#include "PulseEngine.h"

// Include the various types you will be using in your code
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/engine/SEEventManager.h"
#include "cdm/engine/SEConditionManager.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceFraction.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/patient/conditions/SEChronicObstructivePulmonaryDisease.h"
#include "cdm/patient/conditions/SEImpairedAlveolarExchange.h"
#include "cdm/patient/conditions/SEPneumonia.h"
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/system/physiology/SERespiratorySystem.h"
#include "cdm/system/environment/SEEnvironmentalConditions.h"
#include "cdm/system/environment/actions/SEChangeEnvironmentalConditions.h"
#include "cdm/patient/actions/SEMechanicalVentilation.h"
#include "cdm/patient/actions/SEAirwayObstruction.h"
#include "cdm/patient/actions/SEAsthmaAttack.h"
#include "cdm/patient/actions/SEAcuteStress.h"
#include "cdm/patient/actions/SEDyspnea.h"
#include "cdm/patient/actions/SETensionPneumothorax.h"
#include "cdm/patient/actions/SEBrainInjury.h"
#include "cdm/patient/actions/SESubstanceBolus.h"
#include "cdm/patient/actions/SEConsciousRespiration.h"
#include "cdm/patient/actions/SEForcedInhale.h"
#include "cdm/patient/actions/SEForcedExhale.h"
#include "cdm/patient/actions/SEForcedPause.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPressure.h"
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

// Make a custom event handler that you can connect to your code (See EngineUse for more info)
class MechVentHandler : public Loggable, public SEEventHandler
{
public:
  MechVentHandler(Logger *logger) : Loggable(logger), SEEventHandler() { }
  void HandleEvent(eEvent type, bool active, const SEScalarTime* time = nullptr) override
  {
    switch (type)
    {     
      case eEvent::CardiogenicShock:
      {
        if (active)
          m_Logger->Info("Do something for CardiogenicShock");
        else
          m_Logger->Info("Stop doing something for CardiogenicShock");
        break;
      }
      default:
        break;// Nothing
    }
  }
};

//--------------------------------------------------------------------------------------------------
/// \brief
/// Usage for the Mechanical Ventilation Patient Action
/// Drive respiration with your own driver
///
/// \details
/// Refer to the SEMechanicalVentilation class
//--------------------------------------------------------------------------------------------------
void HowToMechanicalVentilation()
{
  //Note: Setting circuit values (resistance/compliances/etc.) needs to be done in the engine code - they currently are not directly exposed
  
  std::stringstream ss;
  // Create a Pulse Engine and load the standard patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/HowTo_MechanicalVentilation.log");
  
  pe->GetLogger()->Info("HowTo_MechanicalVentilation");
  
  //Initialize the patient with any conditions
  //Change the following true/false flags to give the patient different conditions
  //If no conditions, just load the serialized healthy state
  if (true) //Healthy - i.e., no chronic conditions
  {
    if (!pe->SerializeFromFile("./states/StandardMale@0s.json"))// Select patient
    {
      pe->GetLogger()->Error("Could not load state, check the error");
      return;
    }
  }
  else
  {
    SEPatientConfiguration pc;
    pc.SetPatientFile("StandardMale.json");

    if (false) //COPD
    {
      SEChronicObstructivePulmonaryDisease& COPD = pc.GetConditions().GetChronicObstructivePulmonaryDisease();
      COPD.GetBronchitisSeverity().SetValue(0.5);
      COPD.GetEmphysemaSeverity(eLungCompartment::LeftLung).SetValue(0.7);
      COPD.GetEmphysemaSeverity(eLungCompartment::RightLung).SetValue(0.7);
    }
    if (false) //Pneumonia
    {
      SEPneumonia& Pneumonia = pc.GetConditions().GetPneumonia();
      Pneumonia.GetSeverity(eLungCompartment::LeftLung).SetValue(0.2);
      Pneumonia.GetSeverity(eLungCompartment::RightLung).SetValue(0.3);
    }
    if (false) //Generic ImpairedAlveolarExchange (no specified reason)
    {
      SEImpairedAlveolarExchange& ImpairedAlveolarExchange = pc.GetConditions().GetImpairedAlveolarExchange();
      ImpairedAlveolarExchange.GetImpairedFraction().SetValue(0.25);
    }

    //Select the patient and initialize with conditions
    //You can optionally define the patient here - see HowTo-CreateAPatient.cpp
    if (!pe->InitializeEngine(pc))
    {
      pe->GetLogger()->Error("Could not load initialize engine, check the error");
      return;
    }
  }

  // Let's add our event listener callback
  MechVentHandler myEventHandler(pe->GetLogger());
  pe->GetEventManager().ForwardEvents(&myEventHandler);

  // Create data requests for each value that should be written to the output log as the engine is executing
  // Physiology System Names are defined on the System Objects 
  //System data
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("TidalVolume", VolumeUnit::mL);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit::mL);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("RelativeTotalLungVolume", VolumeUnit::mL);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("OxygenSaturation");
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("ExpiratoryRespiratoryResistance", PressureTimePerVolumeUnit::cmH2O_s_Per_L);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("InspiratoryRespiratoryResistance", PressureTimePerVolumeUnit::cmH2O_s_Per_L);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("RespiratoryCompliance", VolumePerPressureUnit::L_Per_cmH2O);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("PulmonaryCapillariesWedgePressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("PulmonaryArterialPressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("PulmonaryMeanArterialPressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("CardiacIndex", VolumePerTimeAreaUnit::L_Per_min_m2);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("SystemicVascularResistance", PressureTimePerVolumeUnit::cmH2O_s_Per_L);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("PulmonaryVascularResistance", PressureTimePerVolumeUnit::cmH2O_s_Per_L);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("PulmonaryVascularResistanceIndex", PressureTimePerVolumeAreaUnit::dyn_s_Per_cm5_m2);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("BloodPH");
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("ArterialOxygenPressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("ArterialCarbonDioxidePressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("VenousOxygenPressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("VenousCarbonDioxidePressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("TotalPulmonaryVentilation", VolumePerTimeUnit::L_Per_min);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("IntracranialPressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("HorowitzIndex", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("AlveolarArterialGradient", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("SedationLevel");
  //Patient data
  pe->GetEngineTracker()->GetDataRequestManager().CreatePatientDataRequest("FunctionalResidualCapacity", VolumeUnit::L);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePatientDataRequest("VitalCapacity", VolumeUnit::L);
  //Compartment data
  //Arteriole bicarbonate
  pe->GetEngineTracker()->GetDataRequestManager().CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::Aorta, "Bicarbonate", "Concentration", MassPerVolumeUnit::ug_Per_mL);
  //Lactate - this should have a relationship to lactic acid
  pe->GetEngineTracker()->GetDataRequestManager().CreateSubstanceDataRequest("Lactate", "BloodConcentration", MassPerVolumeUnit::ug_Per_mL);
  
  pe->GetEngineTracker()->GetDataRequestManager().SetResultsFilename("HowToMechanicalVentilation.csv");

  //Output some random stuff to the log
  pe->GetLogger()->Info(std::stringstream() << "Tidal Volume : " << pe->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);
  pe->GetLogger()->Info(std::stringstream() << "Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
  pe->GetLogger()->Info(std::stringstream() << "Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Respiration Rate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
  pe->GetLogger()->Info(std::stringstream() << "Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());

  //Go 1 min before doing anything
  //The patient is just doing spontaneous breathing
  AdvanceAndTrackTime_s(60.0, *pe);

  //Let's do a bunch of different actions at the same time!

  //Use conscious respiration to cough - as expected, this won't do much to the patient's physiology
  //Conscious respiration could be used force certain breathing patterns, but will no longer take into account any feedback
  SEConsciousRespiration consciousRespiration;
  // Create commands in the order you want them processed.
  // Inhale is the first command we want to process
  SEForcedInhale& forcedInhale = consciousRespiration.AddForcedInhale();
  forcedInhale.GetInspiratoryCapacityFraction().SetValue(0.25);
  forcedInhale.GetInhalePeriod().SetValue(0.7, TimeUnit::s);
  // Next we will hold our breath
  consciousRespiration.AddForcedPause().GetPeriod().SetValue(0.25, TimeUnit::s);
  // Then exhale
  SEForcedExhale& forcedExhale = consciousRespiration.AddForcedExhale();
  forcedExhale.GetExpiratoryReserveVolumeFraction().SetValue(0.0);
  forcedExhale.GetExhalePeriod().SetValue(0.05, TimeUnit::s);
  // Then hold our breath again
  consciousRespiration.AddForcedPause().GetPeriod().SetValue(0.5, TimeUnit::s);
  // Once ProcessAction is called, the engine will make a copy of these commands.
  // You cannont modify them, 
  //  you will need to either clear out this command and reprocess it, 
  //  or process a whole new command.
  // If you plan on reusing this consciousRespiration action, you need to clear it if you want to add a new set of commands.
  pe->ProcessAction(consciousRespiration);
  // NOTE : The engine is going to need  to run for the total sum of the command periods provided above
  // for the action to be completly processed by the engine
  // You can add other actions while this action is being processed.
  // Just be aware that this action is still being processed.
  // It is recommended that you advance time for at least the sum of the command periods.
  AdvanceAndTrackTime_s(60.0, *pe);

  //Airway obstruction
  SEAirwayObstruction obstruction;
  obstruction.GetSeverity().SetValue(0.2);
  pe->ProcessAction(obstruction);
  
  AdvanceAndTrackTime_s(60.0, *pe);

  //Pneumothorax
  // Create a Tension Pnuemothorax 
  // Set the severity (a fraction between 0 and 1)
  SETensionPneumothorax pneumo;
  // You can have a Closed or Open Tension Pneumothorax
  pneumo.SetType(eGate::Open);
  pneumo.GetSeverity().SetValue(0.3);
  // It can be on the Left or right side
  pneumo.SetSide(eSide::Right);
  pe->ProcessAction(pneumo);

  AdvanceAndTrackTime_s(60.0, *pe);

  //Asthma attack
  SEAsthmaAttack asthmaAttack;
  asthmaAttack.GetSeverity().SetValue(0.3);
  pe->ProcessAction(asthmaAttack);  
  
  //Stress response - release epinephrine
  SEAcuteStress acuteStress;
  acuteStress.GetSeverity().SetValue(0.3);
  pe->ProcessAction(acuteStress);
  
  //TBI
  //See HowTo-BrainInjury for an example of getting the Glasgow Scale
  SEBrainInjury tbi;
  tbi.SetType(eBrainInjury_Type::Diffuse);// Can also be LeftFocal or RightFocal, and you will get pupillary effects in only one eye 
  tbi.GetSeverity().SetValue(0.2);
  pe->ProcessAction(tbi);

  //Environment change
  SEChangeEnvironmentalConditions env(pe->GetLogger());
  SEEnvironmentalConditions& envConditions = env.GetEnvironmentalConditions();
  envConditions.GetAirVelocity().SetValue(2.0, LengthPerTimeUnit::m_Per_s);
  envConditions.GetAmbientTemperature().SetValue(15.0, TemperatureUnit::C);
  envConditions.GetAtmosphericPressure().SetValue(740., PressureUnit::mmHg);
  envConditions.GetMeanRadiantTemperature().SetValue(15.0, TemperatureUnit::C);
  pe->ProcessAction(env);

  AdvanceAndTrackTime_s(60.0, *pe);

  //Make the patient stop breathing
  //Dyspnea
  //Maybe the muscles are getting weak?
  SEDyspnea Dyspnea;
  Dyspnea.GetTidalVolumeSeverity().SetValue(1.0);
  pe->ProcessAction(Dyspnea);

  AdvanceAndTrackTime_s(10.0, *pe);

  //Mechanical Ventilation
  // Create an SEMechanicalVentilation object
  SEMechanicalVentilation mechVent;
  mechVent.SetState(eSwitch::On);// Turn it on
                      // Grab the substance fractions so we can quickly modify them
  SESubstanceFraction& O2frac = mechVent.GetGasFraction(*pe->GetSubstanceManager().GetSubstance("Oxygen"));
  SESubstanceFraction& CO2frac = mechVent.GetGasFraction(*pe->GetSubstanceManager().GetSubstance("CarbonDioxide"));
  SESubstanceFraction& N2frac = mechVent.GetGasFraction(*pe->GetSubstanceManager().GetSubstance("Nitrogen"));

  //We'll mimic inputs from real-time sensors by just driving the mechanical ventilation pressure using a sinusoid
  //Pressure waveform parameters
  double period = 5.0;
  double alpha = (2 * M_PI) / (period);
  double inputPressure_cmH2O = 0.0;
  double amplitude_cmH2O = 6.0;
  double yOffset = 10.0;

  // Drive the system for 5 mins
  for (unsigned int time_s = 0; time_s < 300; time_s++)
  {    
    // Going to update values every second
    //The tracker with write to the results file every time-step

    //Difference from ambient pressure
    inputPressure_cmH2O = yOffset + amplitude_cmH2O * sin(alpha * time_s);   //compute new pressure

    mechVent.GetPressure().SetValue(inputPressure_cmH2O, PressureUnit::cmH2O);
    //You can set flow, but we aren't
    double O2fraction = 0.55; //Can be read from sensor
    double CO2fraction = 4.0E-4; //Can be read from sensor
    O2frac.GetFractionAmount().SetValue(O2fraction);
    CO2frac.GetFractionAmount().SetValue(CO2fraction);
    //Remaining will go to N2
    double N2fraction = 1.0 - O2fraction - CO2fraction;
    N2frac.GetFractionAmount().SetValue(N2fraction);

    pe->ProcessAction(mechVent);

    //Simulate
    AdvanceAndTrackTime_s(1, *pe);

    //Output some random stuff to the log
    pe->GetLogger()->Info(std::stringstream() << "Tidal Volume : " << pe->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);
    pe->GetLogger()->Info(std::stringstream() << "Systolic Pressure : " << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
    pe->GetLogger()->Info(std::stringstream() << "Diastolic Pressure : " << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg) << PressureUnit::mmHg);
    pe->GetLogger()->Info(std::stringstream() << "Heart Rate : " << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min) << "bpm");
    pe->GetLogger()->Info(std::stringstream() << "Respiration Rate : " << pe->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min) << "bpm");
    pe->GetLogger()->Info(std::stringstream() << "Oxygen Saturation : " << pe->GetBloodChemistrySystem()->GetOxygenSaturation());

    //If you are doing a control system and need the volume, use this variable that is referenced to the FRC
    pe->GetLogger()->Info(std::stringstream() << "Relative Total Lung Volume : " << pe->GetRespiratorySystem()->GetRelativeTotalLungVolume(VolumeUnit::mL) << VolumeUnit::mL);
  }

  pe->GetLogger()->Info("Finished");
}