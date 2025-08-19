/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#include "EngineHowTo.h"
#include "PulseEngine.h"

#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/patient/SEPatient.h"
#include "cdm/patient/actions/SESubstanceInfusion.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/utils/FileUtils.h"

#include "cdm/patient/actions/SEHemorrhage.h"
#include "cdm/properties/SEScalar0To1.h"

class ActionLogger : public LoggerForward
{
public:
  void ForwardDebug(const std::string& msg) override { std::cout << "[DEBUG] " << msg << std::endl; }
  void ForwardInfo(const std::string& msg) override
  {
    if (msg.find("[Action]") != std::string::npos)
      std::cout << "[INFO] " << msg.substr(0, msg.find(",")) << "\n" << SEAction::PrettyPrint(msg);
    else
      std::cout << "[INFO] " << msg << std::endl;
  }
  void ForwardWarning(const std::string& msg) override { std::cout << "[WARN] " << msg << std::endl; }
  void ForwardError(const std::string& msg) override { std::cout << "[ERROR] " << msg << std::endl; }
  void ForwardFatal(const std::string& msg) override { std::cout << "[FATAL] " << msg << std::endl; }
};

//--------------------------------------------------------------------------------------------------
/// \brief
/// A place to do experiment with Pulse
///
/// \details
/// This is a good place to code up something that demonstrates an issue for reproduction and debugging
//--------------------------------------------------------------------------------------------------
void HowToSandbox()
{
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/howto/HowTo_Sandbox.log");
  pe->GetLogger()->Info("HowTo_Sandbox");
  ActionLogger handler;
  pe->GetLogger()->AddForward(&handler);

  // Create a specific patient near hypotention
  // This also writes out the file, and subsequent runs will load that state
  // If you want to change the initial patient state, just delete the state file
  std::string stateFile = "./states/StandardMale@0s.json";
  if (!FileExists(stateFile))
  {
    SEPatientConfiguration pc;
    SEPatient& patient = pc.GetPatient();
    patient.SetName("Daniel");
    patient.SetSex(ePatient_Sex::Male);
    patient.GetSystolicArterialPressureBaseline().SetValue(90, PressureUnit::mmHg);
    patient.GetDiastolicArterialPressureBaseline().SetValue(60, PressureUnit::mmHg);
    if (!pe->InitializeEngine(pc))
    {
      pe->GetLogger()->Error("Could not create your patient, check the error");
      return;
    }
    pe->SerializeToFile("./test_results/HowTo_SandboxPatient.json");
  }
  else
    pe->SerializeFromFile(stateFile);

  // Create a CSV file so we can plot data as we run
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("TidalVolume", VolumeUnit::mL);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit::mL);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("OxygenSaturation");
  pe->GetEngineTracker()->GetDataRequestManager().CreateSubstanceDataRequest("Norepinephrine", "BloodConcentration", MassPerVolumeUnit::ug_Per_L);
  pe->GetEngineTracker()->GetDataRequestManager().CreateSubstanceDataRequest("Norepinephrine", "PlasmaConcentration", MassPerVolumeUnit::ug_Per_L);
  pe->GetEngineTracker()->GetDataRequestManager().SetResultsFilename("./test_results/HowTo_Sandbox.csv");

  SEHemorrhage h;
  h.SetCompartment(eHemorrhage_Compartment::LargeIntestine);
  h.SetType(eHemorrhage_Type::External);
  h.GetSeverity().SetValue(0.75);
  h.GetFlowRate().SetValue(5, VolumePerTimeUnit::mL_Per_s);
  std::cout << SEAction::PrettyPrint(h.ToJSON());
  pe->ProcessAction(h);

  bool norepiActive = false;
  SESubstanceInfusion nInfuse(*pe->GetSubstanceManager().GetSubstance("Norepinephrine"));
  nInfuse.GetVolume().SetValue(1000, VolumeUnit::mL);

  double time_s = 5. * 60.; // Run loop for 5 mins
  double dT_s = pe->GetTimeStep(TimeUnit::s);
  int count = static_cast<int>(time_s / dT_s);
  for (int i = 0; i < count; i++)
  {
    if (!pe->AdvanceModelTime())  // Compute 1 time step
    {
      pe->GetLogger()->Fatal("Unable to advance time");
      break;
    }

    // Pull Track will pull data from the engine and append it to the csv file
    pe->GetEngineTracker()->TrackData(pe->GetSimulationTime(TimeUnit::s));
    // Print values every 10s
    if (i%500 == 0)
      pe->GetEngineTracker()->LogRequestedValues();

    // Check the Systolic Pressure
    double dBP = pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg);
    if (dBP < 65 && !norepiActive)
    {
      norepiActive = true;
      // Start the Norepi titration
      nInfuse.GetConcentration().SetValue(25, MassPerVolumeUnit::ug_Per_mL);
      nInfuse.GetRate().SetValue(1, VolumePerTimeUnit::mL_Per_min);
      pe->ProcessAction(nInfuse);
      pe->GetLogger()->Info("Starting Norepinephrine infusion");
    }
    else if (dBP > 72 && norepiActive)
    {
      norepiActive = false;
      // Stop the Norepi titration
      nInfuse.GetConcentration().SetValue(0, MassPerVolumeUnit::mg_Per_mL);
      nInfuse.GetRate().SetValue(0, VolumePerTimeUnit::mL_Per_min);
      pe->ProcessAction(nInfuse);
      pe->GetLogger()->Info("Stopping Norepinephrine infusion");
    }
  }
}
