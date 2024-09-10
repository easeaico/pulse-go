/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "EngineHowTo.h"
#include "PulseEngine.h"

// Include the various types you will be using in your code
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/patient/actions/SECardiovascularMechanicsModification.h"
#include "cdm/system/physiology/SECardiovascularMechanicsModifiers.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/system/physiology/SENervousSystem.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarPressurePerVolume.h"
#include "cdm/properties/SEScalarPressureTimePerVolume.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarUnsigned.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerPressure.h"
#include "cdm/properties/SEScalarVolumePerTime.h"

#include <iomanip>

//--------------------------------------------------------------------------------------------------
/// \brief
/// Custom driver settings for our Cardiovascular system
/// Drive respiration with your own driver
///
/// \details
/// Refer to the SECardiovascularMechanicsModifiers class
//--------------------------------------------------------------------------------------------------
void HowToCardiovascularMechanicsModification()
{
  // Create a Pulse Engine and load the standard patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/howto/HowTo_CardiovascularMechanicsModification.cpp.log");
  pe->GetLogger()->Info("HowTo_CardiovascularMechanicsModification");

  // With this engine, you do not initialize it, its already ready to go at construction time

  // You can load a previously saved state, but this is optional!
  if (!pe->SerializeFromFile("./states/StandardMale@0s.json"))// Select patient
  {
    pe->GetLogger()->Error("Could not load state, check the error");
    return;
  }

  // Setup data requests to write to a csv file so we can plot data
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("TidalVolume", VolumeUnit::mL);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit::mL);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("ExpiratoryRespiratoryResistance", PressureTimePerVolumeUnit::cmH2O_s_Per_L);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("InspiratoryRespiratoryResistance", PressureTimePerVolumeUnit::cmH2O_s_Per_L);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("PulmonaryCompliance", VolumePerPressureUnit::L_Per_cmH2O);
  pe->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("TotalPulmonaryVentilation", VolumePerTimeUnit::L_Per_min);
  pe->GetEngineTracker()->GetDataRequestManager().SetResultsFilename("./test_results/howto/HowTo_CardiovascularMechanicsModification.cpp.csv");

  for (size_t i = 0; i < 3; i++)
  {
    AdvanceAndTrackTime_s(10, *pe);
    pe->GetEngineTracker()->LogRequestedValues();
  }

  SECardiovascularMechanicsModification config;
  SECardiovascularMechanicsModifiers& mechanics = config.GetModifiers();
  mechanics.GetHeartRateMultiplier().SetValue(2.0);
  // By default, the engine will run a stabilization stage to get to a new homeostatis based on the provided modifiers
  // You can listen to the Stabilization event to see when the stabilization stage ends(and starts)
  // If you are slowly modifying the system with your own logic, and don't want the stabilization stage to run
  // set the incremental flag to true, and the engine apply this action and not run a stabilization stage
  pe->ProcessAction(config);

  for (size_t i = 0; i < 36; i++)
  {
    AdvanceAndTrackTime_s(10, *pe);
    pe->GetEngineTracker()->LogRequestedValues();
  }
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Create table of cardiovascular modifier combinations
///
/// \details
/// Refer to the SECardiovascularMechanicsModifiers class
//--------------------------------------------------------------------------------------------------
void HowToCardiovascularMechanicsModificationAnalysis()
{
  // Create a Pulse Engine and load the standard patient
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->SetLogFile("./test_results/howto/HowTo_CardiovascularMechanicsModification.cpp.log");
  pe->GetLogger()->Info("HowToCardiovascularMechanicsModificationAnalysis");

  std::ofstream outFile("./test_results/howto/CardiovascularMechanicsModificationTable.txt"); // Create an ofstream object for output file

  // With this engine, you do not initialize it, its already ready to go at construction time

  std::stringstream results;

  SECardiovascularMechanicsModification config;
  SECardiovascularMechanicsModifiers& mechanics = config.GetModifiers();

  std::vector<double> multiplierList = { 0.5, 0.75, 1.0, 1.5, 2.0 };

  // Header for the table
  results << std::left
    << std::setw(30) << "HeartRateMultiplier"
    << std::setw(30) << "StrokeVolumeMultiplier"
    << std::setw(30) << "ArterialComplianceMultiplier"
    << std::setw(30) << "ArterialResistanceMultiplier"
    << std::setw(30) << "SystemicResistanceMultiplier"
    << std::setw(30) << "SystemicComplianceMultiplier"
    << std::setw(30) << "HeartRate(bpm)"
    << std::setw(30) << "StrokeVolume(mL)"
    << std::setw(30) << "CardiacOutput(L/min)"
    << std::setw(30) << "DiastolicPressure(mmHg)"
    << std::setw(30) << "SystolicPressure(mmHg)"
    << std::setw(30) << "MeanArterialPressure(mmHg)"
    << std::setw(30) << "PulsePressure(mmHg)"
    << std::setw(30) << "BaroreceptorHeartRateScale"
    << std::setw(30) << "ChemoreceptorHeartRateScale"
    << "\n";

  // Loop through all combinations
  for (double HeartRateMultiplier : multiplierList) {
    for (double StrokeVolumeMultiplier : multiplierList) {
      for (double ArterialComplianceMultiplier : multiplierList) {
        for (double ArterialResistanceMultiplier : multiplierList) {
          for (double SystemicResistanceMultiplier : multiplierList) {
            for (double SystemicComplianceMultiplier : multiplierList) {
              mechanics.GetHeartRateMultiplier().SetValue(HeartRateMultiplier);
              mechanics.GetStrokeVolumeMultiplier().SetValue(StrokeVolumeMultiplier);
              mechanics.GetArterialComplianceMultiplier().SetValue(ArterialComplianceMultiplier);
              mechanics.GetArterialResistanceMultiplier().SetValue(ArterialResistanceMultiplier);
              mechanics.GetSystemicResistanceMultiplier().SetValue(SystemicResistanceMultiplier);
              mechanics.GetSystemicComplianceMultiplier().SetValue(SystemicComplianceMultiplier);

              // Load the state to start fresh each run
              if (!pe->SerializeFromFile("./states/StandardMale@0s.json"))// Select patient
              {
                pe->GetLogger()->Error("Could not load state, check the error");
                return;
              }

              pe->ProcessAction(config);
              AdvanceAndTrackTime_s(10, *pe);

              // Log the combination
              results << std::left
                << std::setw(30) << HeartRateMultiplier
                << std::setw(30) << StrokeVolumeMultiplier
                << std::setw(30) << ArterialComplianceMultiplier
                << std::setw(30) << ArterialResistanceMultiplier
                << std::setw(30) << SystemicResistanceMultiplier
                << std::setw(30) << SystemicComplianceMultiplier
                << std::setw(30) << pe->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min)
                << std::setw(30) << pe->GetCardiovascularSystem()->GetHeartStrokeVolume(VolumeUnit::mL)
                << std::setw(30) << pe->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::L_Per_min)
                << std::setw(30) << pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg)
                << std::setw(30) << pe->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg)
                << std::setw(30) << pe->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg)
                << std::setw(30) << pe->GetCardiovascularSystem()->GetPulsePressure(PressureUnit::mmHg)
                << std::setw(30) << pe->GetNervousSystem()->GetBaroreceptorHeartRateScale()
                << std::setw(30) << pe->GetNervousSystem()->GetChemoreceptorHeartRateScale()
                << "\n";

              std::cout << results.str();

              // Output the results
              outFile << results.str(); // Output to file
              outFile.flush(); // Flush after each write
              results.str("");          // Clear the stringstream for the next iteration
            }
          }
        }
      }
    }
  }

  // Output the results
  outFile.close();          // Close the file stream
}