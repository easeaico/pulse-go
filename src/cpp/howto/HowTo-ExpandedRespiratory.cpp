/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "EngineHowTo.h"
#include "PulseEngine.h"
#include "PulseConfiguration.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEDataRequestTracker.h"
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
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorPressureControl.h"
#include "cdm/patient/actions/SEIntubation.h"
#include "cdm/patient/actions/SEDyspnea.h"
#include "cdm/engine/SEAdvanceUntilStable.h"
#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/compartment/fluid/SEGasCompartment.h"
#include "cdm/compartment/fluid/SELiquidCompartment.h"

#include <random>
#include <vector>
#include <atomic>
#include <future>
#include <cmath>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <memory>
#include <thread>
#include <chrono>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <numeric>


void HowToExpandedRespiratory()
{
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->LogToConsole(true);
  pe->GetLogger()->SetLogFile("./test_results/howto/HowTo_ExpandedRespiratory.cpp/HowTo_ExpandedRespiratory.log");

  PulseConfiguration config;
  config.UseExpandedLungs(eSwitch::On);
  pe->SetConfigurationOverride(&config);

  SEDataRequestManager drMgr(pe->GetLogger());
  drMgr.CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("EndTidalCarbonDioxidePressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("HorowitzIndex", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("OxygenSaturation");
  drMgr.CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
  drMgr.CreatePhysiologyDataRequest("TidalVolume", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit::mL);
  drMgr.CreatePhysiologyDataRequest("ArterialOxygenPressure", PressureUnit::mmHg);
  drMgr.CreatePhysiologyDataRequest("ArterialCarbonDioxidePressure", PressureUnit::mmHg);
  drMgr.CreateGasCompartmentDataRequest("LeftLungPulmonary", "Volume", VolumeUnit::mL);
  drMgr.CreateGasCompartmentDataRequest("RightLungPulmonary", "Volume", VolumeUnit::L);
  drMgr.SetResultsFilename("./test_results/howto/HowTo_ExpandedRespiratory.cpp/HowTo_ExpandedRespiratory.csv");

  SEPatientConfiguration pc;
  pc.SetPatientFile("./patients/StandardMale.json");

  pe->InitializeEngine(pc, &drMgr);

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

void HowToExpandedRespiratoryAnalysisInitialState()
{
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->LogToConsole(true);
  pe->GetLogger()->SetLogFile("./test_results/howto/HowTo_ExpandedRespiratoryAnalysisInitialState.log");

  PulseConfiguration config;
  config.UseExpandedLungs(eSwitch::On);
  pe->SetConfigurationOverride(&config);

  SEPatientConfiguration pc;
  pc.SetPatientFile("./patients/StandardMale.json");
  pe->InitializeEngine(pc);

  //Add a ventilator for proper PaO2/FiO2 analysis
  SEIntubation intubate;
  intubate.SetType(eIntubation_Type::Tracheal);
  pe->ProcessAction(intubate);

  SEMechanicalVentilatorPressureControl pc_cmv;
  pc_cmv.SetConnection(eSwitch::On);
  pc_cmv.SetMode(eMechanicalVentilator_PressureControlMode::ContinuousMandatoryVentilation);
  pc_cmv.SetInspirationWaveform(eDriverWaveform::Square);
  pc_cmv.GetFractionInspiredOxygen().SetValue(0.21);
  pc_cmv.GetInspiratoryPeriod().SetValue(1.7, TimeUnit::s);
  pc_cmv.GetInspiratoryPressure().SetValue(15.5, PressureUnit::cmH2O);
  pc_cmv.GetPositiveEndExpiratoryPressure().SetValue(5.0, PressureUnit::cmH2O);
  pc_cmv.GetRespirationRate().SetValue(12.0, FrequencyUnit::Per_min);
  pe->ProcessAction(pc_cmv);

  //Apneic
  SEDyspnea Dyspnea;
  Dyspnea.GetTidalVolumeSeverity().SetValue(1.0);
  Dyspnea.GetRespirationRateSeverity().SetValue(1.0);
  pe->ProcessAction(Dyspnea);

  //Run to homestasis
  SEAdvanceUntilStable RunToStable;
  pe->ProcessAction(RunToStable);

  pe->GetLogger()->Info(std::stringstream() << "Resulting Tidal Volume: " << pe->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL) << VolumeUnit::mL);

  //Serialize to use later
  pe->SerializeToFile("ExpandedRespiratoryAnalysis.json");
}

void HowToExpandedRespiratoryAnalysisOriginal()
{
  // Settings
  //std::vector<double> severityList = { 0.0, 0.9 };
  std::vector<double> severityList = { 0.0 };
  size_t totalRuns = std::pow(severityList.size(), 6);

  // Generate the initial state once to use throughout
  HowToExpandedRespiratoryAnalysisInitialState();

  // Setup everything
  std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
  pe->GetLogger()->LogToConsole(true);
  pe->GetLogger()->SetLogFile("./test_results/howto/HowTo_ExpandedRespiratoryAnalysisOriginal.log");

  PulseConfiguration config;
  config.UseExpandedLungs(eSwitch::On);
  pe->SetConfigurationOverride(&config);

  // Create an ofstream object for output file
  std::ofstream outFile("./test_results/howto/ExpandedRespiratoryAnalysisOriginalTable.csv");
  std::stringstream results;

  // Header for the table
  results << "Right Superior Lobe Apical Severity,"
    << "Right Superior Lobe Posterior Severity,"
    << "Right Superior Lobe Anterior Severity,"
    << "Right Middle Lobe Lateral Severity,"
    << "Right Middle Lobe Medial Severity,"
    << "Right Inferior Lobe Superior Severity,"
    << "Right Inferior Lobe Medial Basal Severity,"
    << "Right Inferior Lobe Anterior Basal Severity,"
    << "Right Inferior Lobe Lateral Basal Severity,"
    << "Right Inferior Lobe Posterior Basal Severity,"
    << "Left Inferior Lobe Posterior Basal Severity,"
    << "Left Inferior Lobe Lateral Basal Severity,"
    << "Left Inferior Lobe Anteromedial Basal Severity,"
    << "Left Inferior Lobe Superior Severity,"
    << "Left Superior Lobe Inferior Lingula Severity,"
    << "Left Superior Lobe Superior Lingula Severity,"
    << "Left Superior Lobe Anterior Severity,"
    << "Left Superior Lobe Apicoposterior Severity,"
    << "Total Pulmonary Ventilation (L/min),"
    << "Total Pulmonary Perfusion (L/min),"
    << "Ventilation Perfusion Ratio,"
    << "PaO2 / FiO2 (mmHg),"
    << "Capillaries PO2 (mmHg),"
    << "Capillaries CO2 (mmHg),";

  // Add headers for ventilation and perfusion for all compartments
  std::vector<std::string> compartments = {
    "RightSuperiorLobeApical", "RightSuperiorLobePosterior", "RightSuperiorLobeAnterior",
    "RightMiddleLobeLateral", "RightMiddleLobeMedial", "RightInferiorLobeSuperior",
    "RightInferiorLobeMedialBasal", "RightInferiorLobeAnteriorBasal", "RightInferiorLobeLateralBasal",
    "RightInferiorLobePosteriorBasal", "LeftInferiorLobePosteriorBasal", "LeftInferiorLobeLateralBasal",
    "LeftInferiorLobeAnteromedialBasal", "LeftInferiorLobeSuperior", "LeftSuperiorLobeInferiorLingula",
    "LeftSuperiorLobeSuperiorLingula", "LeftSuperiorLobeAnterior", "LeftSuperiorLobeApicoposterior"
  };

  for (const auto& compartment : compartments) {
    results << compartment << " Ventilation (L/min),"
      << compartment << " Perfusion (L/min),"
      << compartment << " O2 (mmHg),"
      << compartment << " CO2 (mmHg),";
  }

  results << "\n";

  unsigned int currentRun = 0;

  // Loop through all combinations
  for (double RightSuperiorLobeApicalSeverity : severityList) {
    for (double RightSuperiorLobePosteriorSeverity : severityList) {
      for (double RightSuperiorLobeAnteriorSeverity : severityList) {
        for (double RightMiddleLobeLateralSeverity : severityList) {
          for (double RightMiddleLobeMedialSeverity : severityList) {
            for (double RightInferiorLobeSuperiorSeverity : severityList) {
              for (double RightInferiorLobeMedialBasalSeverity : severityList) {
                for (double RightInferiorLobeAnteriorBasalSeverity : severityList) {
                  for (double RightInferiorLobeLateralBasalSeverity : severityList) {
                    for (double RightInferiorLobePosteriorBasalSeverity : severityList) {
                      for (double LeftInferiorLobePosteriorBasalSeverity : severityList) {
                        for (double LeftInferiorLobeLateralBasalSeverity : severityList) {
                          for (double LeftInferiorLobeAnteromedialBasalSeverity : severityList) {
                            for (double LeftInferiorLobeSuperiorSeverity : severityList) {
                              for (double LeftSuperiorLobeInferiorLingulaSeverity : severityList) {
                                for (double LeftSuperiorLobeSuperiorLingulaSeverity : severityList) {
                                  for (double LeftSuperiorLobeAnteriorSeverity : severityList) {
                                    for (double LeftSuperiorLobeApicoposteriorSeverity : severityList) {

                                      double percentCompleted = static_cast<double>(currentRun) / totalRuns * 100.0;
                                      pe->GetLogger()->Info(std::stringstream() << "Starting Run " << currentRun << " of " << totalRuns << " total (" << percentCompleted << "% complete)");

                                      // Load the state to start fresh each run
                                      if (!pe->SerializeFromFile("ExpandedRespiratoryAnalysis.json")) {
                                        pe->GetLogger()->Error("Could not load state.");
                                        continue;
                                      }

                                      SEPrimaryBlastLungInjury pbli;
                                      pbli.GetSeverity(eLungCompartment::RightSuperiorLobeApical).SetValue(RightSuperiorLobeApicalSeverity);
                                      pbli.GetSeverity(eLungCompartment::RightSuperiorLobePosterior).SetValue(RightSuperiorLobePosteriorSeverity);
                                      pbli.GetSeverity(eLungCompartment::RightSuperiorLobeAnterior).SetValue(RightSuperiorLobeAnteriorSeverity);
                                      pbli.GetSeverity(eLungCompartment::RightMiddleLobeLateral).SetValue(RightMiddleLobeLateralSeverity);
                                      pbli.GetSeverity(eLungCompartment::RightMiddleLobeMedial).SetValue(RightMiddleLobeMedialSeverity);
                                      pbli.GetSeverity(eLungCompartment::RightInferiorLobeSuperior).SetValue(RightInferiorLobeSuperiorSeverity);
                                      pbli.GetSeverity(eLungCompartment::RightInferiorLobeMedialBasal).SetValue(RightInferiorLobeMedialBasalSeverity);
                                      pbli.GetSeverity(eLungCompartment::RightInferiorLobeAnteriorBasal).SetValue(RightInferiorLobeAnteriorBasalSeverity);
                                      pbli.GetSeverity(eLungCompartment::RightInferiorLobeLateralBasal).SetValue(RightInferiorLobeLateralBasalSeverity);
                                      pbli.GetSeverity(eLungCompartment::RightInferiorLobePosteriorBasal).SetValue(RightInferiorLobePosteriorBasalSeverity);
                                      pbli.GetSeverity(eLungCompartment::LeftInferiorLobePosteriorBasal).SetValue(LeftInferiorLobePosteriorBasalSeverity);
                                      pbli.GetSeverity(eLungCompartment::LeftInferiorLobeLateralBasal).SetValue(LeftInferiorLobeLateralBasalSeverity);
                                      pbli.GetSeverity(eLungCompartment::LeftInferiorLobeAnteromedialBasal).SetValue(LeftInferiorLobeAnteromedialBasalSeverity);
                                      pbli.GetSeverity(eLungCompartment::LeftInferiorLobeSuperior).SetValue(LeftInferiorLobeSuperiorSeverity);
                                      pbli.GetSeverity(eLungCompartment::LeftSuperiorLobeInferiorLingula).SetValue(LeftSuperiorLobeInferiorLingulaSeverity);
                                      pbli.GetSeverity(eLungCompartment::LeftSuperiorLobeSuperiorLingula).SetValue(LeftSuperiorLobeSuperiorLingulaSeverity);
                                      pbli.GetSeverity(eLungCompartment::LeftSuperiorLobeAnterior).SetValue(LeftSuperiorLobeAnteriorSeverity);
                                      pbli.GetSeverity(eLungCompartment::LeftSuperiorLobeApicoposterior).SetValue(LeftSuperiorLobeApicoposteriorSeverity);
                                      pe->ProcessAction(pbli);

                                      // Run to homestasis
                                      SEAdvanceUntilStable RunToStable;
                                      RunToStable.SetCriteria("AnalysisAdvanceUntilStable");
                                      pe->ProcessAction(RunToStable);

                                      // Log the combination
                                      const SELiquidCompartment* PulmonaryCapillaries = pe->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::PulmonaryCapillaries);
                                      const SESubstance* O2 = pe->GetSubstanceManager().GetSubstance("Oxygen");
                                      const SESubstance* CO2 = pe->GetSubstanceManager().GetSubstance("CarbonDioxide");

                                      results << RightSuperiorLobeApicalSeverity << ","
                                        << RightSuperiorLobePosteriorSeverity << ","
                                        << RightSuperiorLobeAnteriorSeverity << ","
                                        << RightMiddleLobeLateralSeverity << ","
                                        << RightMiddleLobeMedialSeverity << ","
                                        << RightInferiorLobeSuperiorSeverity << ","
                                        << RightInferiorLobeMedialBasalSeverity << ","
                                        << RightInferiorLobeAnteriorBasalSeverity << ","
                                        << RightInferiorLobeLateralBasalSeverity << ","
                                        << RightInferiorLobePosteriorBasalSeverity << ","
                                        << LeftInferiorLobePosteriorBasalSeverity << ","
                                        << LeftInferiorLobeLateralBasalSeverity << ","
                                        << LeftInferiorLobeAnteromedialBasalSeverity << ","
                                        << LeftInferiorLobeSuperiorSeverity << ","
                                        << LeftSuperiorLobeInferiorLingulaSeverity << ","
                                        << LeftSuperiorLobeSuperiorLingulaSeverity << ","
                                        << LeftSuperiorLobeAnteriorSeverity << ","
                                        << LeftSuperiorLobeApicoposteriorSeverity << ","
                                        << pe->GetRespiratorySystem()->GetTotalPulmonaryVentilation(VolumePerTimeUnit::L_Per_min) << ","
                                        << pe->GetCardiovascularSystem()->GetTotalPulmonaryPerfusion(VolumePerTimeUnit::L_Per_min) << ","
                                        << pe->GetRespiratorySystem()->GetVentilationPerfusionRatio() << ","
                                        << pe->GetRespiratorySystem()->GetHorowitzIndex(PressureUnit::mmHg) << ","
                                        << PulmonaryCapillaries->GetSubstanceQuantity(*O2)->GetPartialPressure(PressureUnit::mmHg) << ","
                                        << PulmonaryCapillaries->GetSubstanceQuantity(*CO2)->GetPartialPressure(PressureUnit::mmHg) << ",";

                                      // Add ventilation and perfusion values for each compartment
                                      for (const auto& compartment : compartments) {
                                        const SEGasCompartment* alveoli = pe->GetCompartments().GetGasCompartment(compartment + "Alveoli");
                                        const SELiquidCompartment* pulmonaryCapillaries = pe->GetCompartments().GetLiquidCompartment(compartment + "PulmonaryCapillaries");

                                        results << alveoli->GetVentilation(VolumePerTimeUnit::L_Per_min) << ","
                                          << pulmonaryCapillaries->GetPerfusion(VolumePerTimeUnit::L_Per_min) << ","
                                          << pulmonaryCapillaries->GetSubstanceQuantity(*O2)->GetPartialPressure(PressureUnit::mmHg) << ","
                                          << pulmonaryCapillaries->GetSubstanceQuantity(*CO2)->GetPartialPressure(PressureUnit::mmHg) << ",";
                                      }

                                      results << "\n";

                                      // Output the results
                                      outFile << results.str(); // Output to file
                                      outFile.flush(); // Flush after each write
                                      results.str(""); // Clear the stringstream for the next iteration

                                      currentRun++;
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  // Close the file stream
  outFile.close();
}


class SilentLogger : public LoggerForward
{
public:
  SilentLogger(std::atomic<unsigned int>& successfulRuns, std::atomic<unsigned int>& unsuccessfulRuns)
    : successfulRuns(successfulRuns), unsuccessfulRuns(unsuccessfulRuns) {}

  void ForwardDebug(const std::string& msg) override {}
  void ForwardInfo(const std::string& msg) override {}
  void ForwardWarning(const std::string& msg) override {}
  void ForwardError(const std::string& msg) override {}
  void ForwardFatal(const std::string& msg) override {
    // Increment the unsuccessful runs counter silently
    unsuccessfulRuns++;
  }

private:
  std::atomic<unsigned int>& successfulRuns;
  std::atomic<unsigned int>& unsuccessfulRuns;
};

// Function to print a status bar with additional information
void printStatusBar(unsigned int completed, unsigned int total, unsigned int successful, unsigned int unsuccessful)
{
  const int barWidth = 50;
  float progress = (float)completed / total;
  int pos = barWidth * progress;

  std::cout << "[";
  for (int i = 0; i < barWidth; ++i) {
    if (i < pos) std::cout << "=";
    else if (i == pos) std::cout << ">";
    else std::cout << " ";
  }
  std::cout << "] " << int(progress * 100.0) << " % "
    << "Completed: " << completed << "/" << total
    << " | Successful: " << successful
    << " | Unsuccessful: " << unsuccessful << "\r";
  std::cout.flush();
}

// Thread pool class to manage a fixed number of threads
class ThreadPool
{
public:
  ThreadPool(size_t numThreads)
    : stop(false)
  {
    for (size_t i = 0; i < numThreads; ++i)
      workers.emplace_back(
        [this]
        {
          for (;;)
          {
            std::function<void()> task;
            {
              std::unique_lock<std::mutex> lock(this->queueMutex);
              this->condition.wait(lock,
                [this] { return this->stop || !this->tasks.empty(); });
              if (this->stop && this->tasks.empty())
                return;
              task = std::move(this->tasks.front());
              this->tasks.pop();
            }
            task();
          }
        }
      );
  }

  template<class F, class... Args>
  auto enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>
  {
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> res = task->get_future();
    {
      std::unique_lock<std::mutex> lock(queueMutex);

      // Don't allow enqueueing after stopping the pool
      if (stop)
        throw std::runtime_error("enqueue on stopped ThreadPool");

      tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    return res;
  }

  ~ThreadPool()
  {
    {
      std::unique_lock<std::mutex> lock(queueMutex);
      stop = true;
    }
    condition.notify_all();
    for (std::thread& worker : workers)
      worker.join();
  }

private:
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;

  std::mutex queueMutex;
  std::condition_variable condition;
  bool stop;
};

// Function to generate Latin Hypercube Samples
std::vector<std::vector<double>> generateLHSamples(int numSamples, int numParameters) {
  std::vector<std::vector<double>> samples(numSamples, std::vector<double>(numParameters));
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0.0, 1.0);

  for (int i = 0; i < numParameters; ++i) {
    std::vector<double> perm(numSamples);
    std::iota(perm.begin(), perm.end(), 0);
    std::shuffle(perm.begin(), perm.end(), gen);
    for (int j = 0; j < numSamples; ++j) {
      samples[j][i] = (perm[j] + dis(gen)) / numSamples;
    }
  }

  return samples;
}

void HowToExpandedRespiratoryAnalysis()
{
  int numSamples = 1000; // Adjust the number of samples for LHS
  int numParameters = 18; // Number of compartments
  auto lhsSamples = generateLHSamples(numSamples, numParameters);
  unsigned int totalRuns = lhsSamples.size();

  // Generate the initial state once to use throughout
  HowToExpandedRespiratoryAnalysisInitialState();

  // Store the healthy values
  HowToExpandedRespiratoryAnalysisOriginal();

  // Create an ofstream object for output file
  std::ofstream outFile("./test_results/howto/ExpandedRespiratoryAnalysisTable.csv");

  // Header for the table
  std::stringstream results;
  results << "Right Superior Lobe Apical Severity,"
    << "Right Superior Lobe Posterior Severity,"
    << "Right Superior Lobe Anterior Severity,"
    << "Right Middle Lobe Lateral Severity,"
    << "Right Middle Lobe Medial Severity,"
    << "Right Inferior Lobe Superior Severity,"
    << "Right Inferior Lobe Medial Basal Severity,"
    << "Right Inferior Lobe Anterior Basal Severity,"
    << "Right Inferior Lobe Lateral Basal Severity,"
    << "Right Inferior Lobe Posterior Basal Severity,"
    << "Left Inferior Lobe Posterior Basal Severity,"
    << "Left Inferior Lobe Lateral Basal Severity,"
    << "Left Inferior Lobe Anteromedial Basal Severity,"
    << "Left Inferior Lobe Superior Severity,"
    << "Left Superior Lobe Inferior Lingula Severity,"
    << "Left Superior Lobe Superior Lingula Severity,"
    << "Left Superior Lobe Anterior Severity,"
    << "Left Superior Lobe Apicoposterior Severity,"
    << "Total Pulmonary Ventilation (L/min),"
    << "Total Pulmonary Perfusion (L/min),"
    << "Ventilation Perfusion Ratio,"
    << "PaO2 / FiO2 (mmHg),"
    << "Capillaries PO2 (mmHg),"
    << "Capillaries CO2 (mmHg),";

  // Add headers for ventilation and perfusion for all compartments
  std::vector<std::string> compartments = {
      "RightSuperiorLobeApical", "RightSuperiorLobePosterior", "RightSuperiorLobeAnterior",
      "RightMiddleLobeLateral", "RightMiddleLobeMedial", "RightInferiorLobeSuperior",
      "RightInferiorLobeMedialBasal", "RightInferiorLobeAnteriorBasal", "RightInferiorLobeLateralBasal",
      "RightInferiorLobePosteriorBasal", "LeftInferiorLobePosteriorBasal", "LeftInferiorLobeLateralBasal",
      "LeftInferiorLobeAnteromedialBasal", "LeftInferiorLobeSuperior", "LeftSuperiorLobeInferiorLingula",
      "LeftSuperiorLobeSuperiorLingula", "LeftSuperiorLobeAnterior", "LeftSuperiorLobeApicoposterior"
  };

  for (const auto& compartment : compartments) {
    results << compartment << " Ventilation (L/min),"
      << compartment << " Perfusion (L/min),";
  }

  results << "\n";
  outFile << results.str();
  results.str("");  // Clear the stringstream for the next iteration

  // Helper function to run a single simulation
  auto run_simulation = [&](const std::vector<double>& severities, std::atomic<unsigned int>& successfulRuns, std::atomic<unsigned int>& unsuccessfulRuns) -> std::string {
    try {
      std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
      pe->GetLogger()->LogToConsole(false);
      pe->GetLogger()->SetLogFile("");

      SilentLogger myLogger(successfulRuns, unsuccessfulRuns);
      pe->GetLogger()->AddForward(&myLogger);

      PulseConfiguration config;
      config.UseExpandedLungs(eSwitch::On);
      pe->SetConfigurationOverride(&config);

      // Load the state to start fresh each run
      if (!pe->SerializeFromFile("ExpandedRespiratoryAnalysis.json")) {
        unsuccessfulRuns++;
        return "";
      }

      SEPrimaryBlastLungInjury pbli;
      pbli.GetSeverity(eLungCompartment::RightSuperiorLobeApical).SetValue(severities[0]);
      pbli.GetSeverity(eLungCompartment::RightSuperiorLobePosterior).SetValue(severities[1]);
      pbli.GetSeverity(eLungCompartment::RightSuperiorLobeAnterior).SetValue(severities[2]);
      pbli.GetSeverity(eLungCompartment::RightMiddleLobeLateral).SetValue(severities[3]);
      pbli.GetSeverity(eLungCompartment::RightMiddleLobeMedial).SetValue(severities[4]);
      pbli.GetSeverity(eLungCompartment::RightInferiorLobeSuperior).SetValue(severities[5]);
      pbli.GetSeverity(eLungCompartment::RightInferiorLobeMedialBasal).SetValue(severities[6]);
      pbli.GetSeverity(eLungCompartment::RightInferiorLobeAnteriorBasal).SetValue(severities[7]);
      pbli.GetSeverity(eLungCompartment::RightInferiorLobeLateralBasal).SetValue(severities[8]);
      pbli.GetSeverity(eLungCompartment::RightInferiorLobePosteriorBasal).SetValue(severities[9]);
      pbli.GetSeverity(eLungCompartment::LeftInferiorLobePosteriorBasal).SetValue(severities[10]);
      pbli.GetSeverity(eLungCompartment::LeftInferiorLobeLateralBasal).SetValue(severities[11]);
      pbli.GetSeverity(eLungCompartment::LeftInferiorLobeAnteromedialBasal).SetValue(severities[12]);
      pbli.GetSeverity(eLungCompartment::LeftInferiorLobeSuperior).SetValue(severities[13]);
      pbli.GetSeverity(eLungCompartment::LeftSuperiorLobeInferiorLingula).SetValue(severities[14]);
      pbli.GetSeverity(eLungCompartment::LeftSuperiorLobeSuperiorLingula).SetValue(severities[15]);
      pbli.GetSeverity(eLungCompartment::LeftSuperiorLobeAnterior).SetValue(severities[16]);
      pbli.GetSeverity(eLungCompartment::LeftSuperiorLobeApicoposterior).SetValue(severities[17]);
      pe->ProcessAction(pbli);

      // Run to homestasis
      SEAdvanceUntilStable RunToStable;
      RunToStable.SetCriteria("AnalysisAdvanceUntilStable");
      pe->ProcessAction(RunToStable);

      // Log the combination
      const SELiquidCompartment* PulmonaryCapillaries = pe->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::PulmonaryCapillaries);
      const SESubstance* O2 = pe->GetSubstanceManager().GetSubstance("Oxygen");
      const SESubstance* CO2 = pe->GetSubstanceManager().GetSubstance("CarbonDioxide");

      std::stringstream result;
      for (const auto& severity : severities) {
        result << severity << ",";
      }
      result << pe->GetRespiratorySystem()->GetTotalPulmonaryVentilation(VolumePerTimeUnit::L_Per_min) << ","
        << pe->GetCardiovascularSystem()->GetTotalPulmonaryPerfusion(VolumePerTimeUnit::L_Per_min) << ","
        << pe->GetRespiratorySystem()->GetVentilationPerfusionRatio() << ","
        << pe->GetRespiratorySystem()->GetHorowitzIndex(PressureUnit::mmHg) << ","
        << PulmonaryCapillaries->GetSubstanceQuantity(*O2)->GetPartialPressure(PressureUnit::mmHg) << ","
        << PulmonaryCapillaries->GetSubstanceQuantity(*CO2)->GetPartialPressure(PressureUnit::mmHg) << ",";

      // Add ventilation and perfusion values for each compartment
      for (const auto& compartment : compartments) {
        const SEGasCompartment* alveoli = pe->GetCompartments().GetGasCompartment(compartment + "Alveoli");
        const SELiquidCompartment* pulmonaryCapillaries = pe->GetCompartments().GetLiquidCompartment(compartment + "PulmonaryCapillaries");

        result << alveoli->GetVentilation(VolumePerTimeUnit::L_Per_min) << ","
          << pulmonaryCapillaries->GetPerfusion(VolumePerTimeUnit::L_Per_min) << ",";
      }

      result << "\n";
      successfulRuns++;
      return result.str();
    }
    catch (...) {
      unsuccessfulRuns++;
      return "";
    }
    };

  // Vector to hold futures
  std::vector<std::future<std::string>> futures;
  std::atomic<unsigned int> completedRuns(0);
  std::atomic<unsigned int> successfulRuns(0);
  std::atomic<unsigned int> unsuccessfulRuns(0);

  // Create a thread pool with 10 threads
  ThreadPool pool(10);

  // Launch tasks in the thread pool
  for (const auto& sample : lhsSamples) {
    futures.push_back(pool.enqueue([&] {
      auto result = run_simulation(sample, successfulRuns, unsuccessfulRuns);
      completedRuns++;
      return result;
      }));
  }

  // Create a thread to update the status bar
  std::thread statusThread([&] {
    while (completedRuns < totalRuns) {
      printStatusBar(completedRuns, totalRuns, successfulRuns, unsuccessfulRuns);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    printStatusBar(completedRuns, totalRuns, successfulRuns, unsuccessfulRuns);
    std::cout << std::endl; // Move to the next line after the progress bar is complete
    });

  // Collect results
  for (auto& future : futures) {
    auto result = future.get();
    if (!result.empty()) {
      outFile << result;
    }
  }

  // Wait for the status thread to finish
  statusThread.join();

  // Close the file stream
  outFile.close();
}
