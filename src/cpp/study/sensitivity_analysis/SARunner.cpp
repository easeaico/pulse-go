/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "SARunner.h"

#include "PulseConfiguration.h"
#include "engine/common/Engine.h"
#include "engine/common/controller/Controller.h"
#include "engine/common/controller/CircuitManager.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/io/protobuf/PBUtils.h"

namespace pulse::study::sensitivity_analysis
{

  SARunner::SARunner(const std::string& logfileName, const std::string& dataDir) : Loggable(logfileName)
  {
    m_DataDir = dataDir;
    m_SimulationList = nullptr;
    m_SimulationResultsList = nullptr;
  }
  SARunner::~SARunner()
  {
    SAFE_DELETE(m_SimulationList);
    SAFE_DELETE(m_SimulationResultsList);
  }

  bool SARunner::Run(pulse::study::bind::sensitivity_analysis::SimulationListData& simList)
  {
    m_OutDir = simList.outputrootdir();
    m_SimulationResultsListFile = m_OutDir + "/simlist_results.json";
    SAFE_DELETE(m_SimulationList);
    SAFE_DELETE(m_SimulationResultsList);
    m_SimulationList = &simList;
    m_SimulationResultsList = new pulse::study::bind::sensitivity_analysis::SimulationListData();
    if (FileExists(m_SimulationResultsListFile))
    {
      if (!SerializeFromFile(m_SimulationResultsListFile, *m_SimulationResultsList, eSerializationFormat::JSON))
      {
        GetLogger()->Warning("Unable to read found results file");
      }
    }
    bool b = Run();
    m_SimulationList = nullptr;
    SAFE_DELETE(m_SimulationResultsList);
    return b;
  }

  bool SARunner::Run(const std::string& filename, eSerializationFormat f)
  {
    SAFE_DELETE(m_SimulationList);
    SAFE_DELETE(m_SimulationResultsList);
    m_SimulationList = new pulse::study::bind::sensitivity_analysis::SimulationListData();
    m_SimulationResultsList = new pulse::study::bind::sensitivity_analysis::SimulationListData();

    if (!SerializeFromFile(filename, *m_SimulationList, f))
      return false;
    // Let's try to read in a results file
    m_OutDir = m_SimulationList->outputrootdir();
    m_SimulationResultsListFile = m_OutDir + "/simlist_results.json";
    if (FileExists(m_SimulationResultsListFile))
    {
      if (!SerializeFromFile(m_SimulationResultsListFile, *m_SimulationResultsList, f))
      {
        GetLogger()->Warning("Unable to read found results file");
      }
    }
    return Run();
  }
  bool SARunner::Run()
  {
    TimingProfile profiler;
    profiler.Start("Total");

    // Ensure our output dir exists
    CreatePath(m_OutDir);

    // Get the ID's of simulations we need to run
    m_SimulationsToRun.clear();
    for (int i = 0; i < m_SimulationList->simulation_size(); i++)
      m_SimulationsToRun.insert(m_SimulationList->simulation()[i].id());
    // Remove any id's we have in the results
    if (m_SimulationResultsList->simulation_size() > 0)
    {
      Info("Found " + std::to_string(m_SimulationResultsList->simulation_size()) + " previously run simulations");
      for (int i = 0; i < m_SimulationResultsList->simulation_size(); i++)
        m_SimulationsToRun.erase(m_SimulationResultsList->simulation()[i].id());
    }

    size_t numSimsToRun = (size_t)m_SimulationList->simulation_size() - (size_t)m_SimulationResultsList->simulation_size();
    if (numSimsToRun == 0)
    {
      Info("All simulations are run in the results file");
      return true;
    }
    size_t processor_count = std::thread::hardware_concurrency();
    if (processor_count == 0)
    {
      Fatal("Unable to detect number of processors");
      return false;
    }
    // Let's not kill the computer this is running on...
    if (processor_count > 1)
      processor_count -= 1;
    // Let's not kick off more threads than we need
    if (processor_count > numSimsToRun)
      processor_count = numSimsToRun;
    Info("Starting " + std::to_string(processor_count) + " Threads to run " + std::to_string(m_SimulationsToRun.size()) + " simulations");
    // Crank up our threads
    for (size_t p = 0; p < processor_count; p++)
      m_Threads.push_back(std::thread(&SARunner::ControllerLoop, this));

    for (size_t p = 0; p < processor_count; p++)
      m_Threads[p].join();

    Info("It took " + cdm::to_string(profiler.GetElapsedTime_s("Total")) + "s to run this simulation list");
    profiler.Clear();
    return true;
  }

  void SARunner::ControllerLoop()
  {
    pulse::study::bind::sensitivity_analysis::SimulationData* sim = nullptr;
    while (true)
    {
      try
      {
        sim = GetNextSimulation();
        if (sim == nullptr)
          break;
        //RunSimulationUntilStable(m_OutDir, *sim);
        RunSimulation(m_OutDir, *sim);
        FinalizeSimulation(*sim);
      }
      catch (CommonDataModelException& cdm_ex)
      {
        GetLogger()->Fatal("Exception caught runnning simulation " + sim->name());
        GetLogger()->Fatal(cdm_ex.what());
        std::cerr << cdm_ex.what() << std::endl;
      }
      catch (std::exception& ex)
      {
        GetLogger()->Fatal("Exception caught runnning simulation " + sim->name());
        GetLogger()->Fatal(ex.what());
        std::cerr << ex.what() << std::endl;
      }
      catch (...)
      {
        std::cerr << "Unable to run simulation " << sim->name() << std::endl;
      }
    }
  }

  bool SARunner::RunSimulationUntilStable(std::string const& outDir, pulse::study::bind::sensitivity_analysis::SimulationData& sim, const std::string& dataDir)
  {
    TimingProfile profiler;
    profiler.Start("Total");
    profiler.Start("Status");

    auto pulse = CreatePulseEngine();
    pulse->GetLogger()->LogToConsole(false); // No logging to console (when threaded)
    pulse->GetLogger()->SetLogFile(outDir + "/" + std::to_string(sim.id()) + " - " + sim.name() + ".log");

    // Setup data requests
    SEDataRequestManager drMgr(pulse->GetLogger());
    //drMgr.CreatePhysiologyDataRequest("BloodVolume", VolumeUnit::mL);
    //drMgr.CreatePhysiologyDataRequest("CardiacOutput", VolumePerTimeUnit::mL_Per_min);
    //drMgr.CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
    //drMgr.CreatePhysiologyDataRequest("HeartEjectionFraction");
    //drMgr.CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
    //drMgr.CreatePhysiologyDataRequest("HeartStrokeVolume", VolumeUnit::mL);
    //drMgr.CreatePhysiologyDataRequest("MeanArterialCarbonDioxidePartialPressure", PressureUnit::mmHg);
    //drMgr.CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
    //drMgr.CreatePhysiologyDataRequest("PulmonaryDiastolicArterialPressure", PressureUnit::mmHg);
    //drMgr.CreatePhysiologyDataRequest("PulmonaryMeanArterialPressure", PressureUnit::mmHg);
    //drMgr.CreatePhysiologyDataRequest("PulmonaryMeanCapillaryFlow", VolumePerTimeUnit::mL_Per_min);
    //drMgr.CreatePhysiologyDataRequest("PulmonarySystolicArterialPressure", PressureUnit::mmHg);
    //drMgr.CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
    //drMgr.CreatePhysiologyDataRequest("SystemicVascularResistance", PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    //drMgr.SetResultsFilename(outDir + "/" + cdm::to_string(sim.id()) + " - " + sim.name() + ".csv");

    // TODO amb Clean this up (cfg should have a default ctor that makes its own Sub Mgr)
    PulseConfiguration cfg(pulse->GetLogger());
    cfg.SetBaroreceptorFeedback(eSwitch::Off);
    cfg.SetChemoreceptorFeedback(eSwitch::Off);
    pulse->SetConfigurationOverride(&cfg);
    if (!pulse->SerializeFromFile("./states/StandardMale@0s.json", &drMgr))
      return false;

    // Apply Overrides (Note using Force, as these values are locked (for good reason)
    // But we know what we are doing, right?
    Controller& pc =
      ((pulse::Engine*)pulse.get())->GetController();
    SEFluidCircuit& cv = pc.GetCircuits().GetActiveCardiovascularCircuit();
    SEFluidCircuit& resp = pc.GetCircuits().GetActiveRespiratoryCircuit();
    bool hasRespOverride = false;

    for (auto const& [name, o] : sim.overrides())
    {
      SEFluidCircuitPath* path = nullptr;
      if (resp.HasPath(name))
      {
        path = resp.GetPath(name);
        hasRespOverride = true;
      }
      else if (cv.HasPath(name))
      {
        path = cv.GetPath(name);
      }
      if (path == nullptr)
      {
        Error("No path found for override " + name + ", for " + std::to_string(sim.id()) + " - " + sim.name());
        return false;
      }
      if (PressureTimePerVolumeUnit::IsValidUnit(o.unit()))
      {// Assume its a resistor
        const PressureTimePerVolumeUnit& u = PressureTimePerVolumeUnit::GetCompoundUnit(o.unit());
        path->GetResistance().ForceValue(o.value(), u);
        path->GetNextResistance().ForceValue(o.value(), u);
        path->GetResistanceBaseline().ForceValue(o.value(), u);
      }
      else if (VolumePerPressureUnit::IsValidUnit(o.unit()))
      {
        const VolumePerPressureUnit& u = VolumePerPressureUnit::GetCompoundUnit(o.unit());
        path->GetCompliance().ForceValue(o.value(), u);
        path->GetNextCompliance().ForceValue(o.value(), u);
        path->GetComplianceBaseline().ForceValue(o.value(), u);
      }
      else
      {
        Error("Could not process override " + name + ", for " + std::to_string(sim.id()) + " - " + sim.name());
        return false;
      }
    }

    std::unordered_map<std::string, RunningAverages> runningAverages =
    {
      {"MeanAortaInflow_mL_Per_s", RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Aorta), VolumePerTimeUnit::mL_Per_s)},
      {"MeanArterialOxygenPartialPressure_mmHg",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Aorta)->GetSubstanceQuantity(*pulse->GetSubstanceManager().GetSubstance("Oxygen")), PressureUnit::mmHg)},
      {"MeanBoneVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Bone), VolumePerTimeUnit::mL_Per_s)},
      {"MeanBrainVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Brain), VolumePerTimeUnit::mL_Per_s)},
      {"MeanFatVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Fat), VolumePerTimeUnit::mL_Per_s)},
      {"MeanKidneyVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Kidneys), VolumePerTimeUnit::mL_Per_s)},
      {"MeanLargeIntestineVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LargeIntestine), VolumePerTimeUnit::mL_Per_s)},
      {"MeanLeftArmVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftArm), VolumePerTimeUnit::mL_Per_s)},
      {"MeanLeftHeartInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftHeart), VolumePerTimeUnit::mL_Per_s)},
      {"MeanLeftHeartPressure_mmHg",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftHeart), PressureUnit::mmHg)},
      {"MeanLeftKidneyVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftKidney), VolumePerTimeUnit::mL_Per_s)},
      {"MeanLeftLegVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftLeg), VolumePerTimeUnit::mL_Per_s)},
      {"MeanLeftPulmonaryArteriesInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftPulmonaryArteries), VolumePerTimeUnit::mL_Per_s)},
      {"MeanLeftPulmonaryCapillariesInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftPulmonaryCapillaries), VolumePerTimeUnit::mL_Per_s)},
      {"MeanLeftPulmonaryVeinsInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftPulmonaryVeins), VolumePerTimeUnit::mL_Per_s)},
      {"MeanLiverVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Liver), VolumePerTimeUnit::mL_Per_s)},
      {"MeanMuscleVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Muscle), VolumePerTimeUnit::mL_Per_s)},
      {"MeanMyocardiumVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Myocardium), VolumePerTimeUnit::mL_Per_s)},
      {"MeanPulmonaryArteriesInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::PulmonaryArteries), VolumePerTimeUnit::mL_Per_s)},
      {"MeanPulmonaryCapillariesInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::PulmonaryCapillaries), VolumePerTimeUnit::mL_Per_s)},
      {"MeanPulmonaryCapillariesPressure_mmHg",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::PulmonaryCapillaries), PressureUnit::mmHg)},
      {"MeanPulmonaryVeinsInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::PulmonaryVeins), VolumePerTimeUnit::mL_Per_s)},
      {"MeanPulmonaryVeinsPressure_mmHg",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::PulmonaryVeins), PressureUnit::mmHg)},
      {"MeanRightArmVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightArm), VolumePerTimeUnit::mL_Per_s)},
      {"MeanRightHeartInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightHeart), VolumePerTimeUnit::mL_Per_s)},
      {"MeanRightHeartPressure_mmHg",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightHeart), PressureUnit::mmHg)},
      {"MeanRightKidneyVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightKidney), VolumePerTimeUnit::mL_Per_s)},
      {"MeanRightLegVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightLeg), VolumePerTimeUnit::mL_Per_s)},
      {"MeanRightPulmonaryArteriesInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightPulmonaryArteries), VolumePerTimeUnit::mL_Per_s)},
      {"MeanRightPulmonaryCapillariesInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightPulmonaryCapillaries), VolumePerTimeUnit::mL_Per_s)},
      {"MeanRightPulmonaryVeinsInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightPulmonaryVeins), VolumePerTimeUnit::mL_Per_s)},
      {"MeanSkinVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Skin), VolumePerTimeUnit::mL_Per_s)},
      {"MeanSmallIntestineVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::SmallIntestine), VolumePerTimeUnit::mL_Per_s)},
      {"MeanSplanchnicVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Splanchnic), VolumePerTimeUnit::mL_Per_s)},
      {"MeanSpleenVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Spleen), VolumePerTimeUnit::mL_Per_s)},
      {"MeanVenaCavaInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::VenaCava), VolumePerTimeUnit::mL_Per_s)},
      {"MeanVenousCarbonDioxidePartialPressure_mmHg",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::VenaCava)->GetSubstanceQuantity(*pulse->GetSubstanceManager().GetSubstance("CarbonDioxide")), PressureUnit::mmHg)},
      {"MeanVenousOxygenPartialPressure_mmHg",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::VenaCava)->GetSubstanceQuantity(*pulse->GetSubstanceManager().GetSubstance("Oxygen")), PressureUnit::mmHg)}
    };

    for (auto& element : runningAverages)
    {
      element.second.Sample();
      element.second.instantaneousAverage = element.second.runningAverage.Value();
    }

    // Run until stable
    // Let's shoot for within 0.25% for 10s straight
    double timeStep_s = pulse->GetTimeStep(TimeUnit::s);
    double stabPercentTolerance = 0.25;
    double stabPercentToleranceO2 = 0.05;
    double stabCheckTime_s = 10.0;
    double time_s = 0;
    double maxTime_s = 2000;
    // Here are our variable we will check for stability
    double previousMap_mmHg = pulse->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg);
    double previousSystolic_mmHg = pulse->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg);
    double previousDiastolic_mmHg = pulse->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg);
    double previousCardiacOutput_mL_Per_min = pulse->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min);
    //double previousMeanCVP_mmHg = pulse->GetCardiovascularSystem()->GetMeanCentralVenousPressure(PressureUnit::mmHg);
    double previousBlood_mL = pulse->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL);
    double previousArterialO2P_mmHg = runningAverages.at("MeanArterialOxygenPartialPressure_mmHg").runningAverage.Value();

    bool stable = false;
    double stableTime_s = 0;

    while (!stable)
    {
      pulse->AdvanceModelTime(timeStep_s, TimeUnit::s);
      if (pulse->GetEventManager().IsEventActive(eEvent::StartOfCardiacCycle))
      {
        for (auto& element : runningAverages)
        {
          element.second.Sample();
          element.second.instantaneousAverage = element.second.runningAverage.Value();
          element.second.runningAverage.Invalidate();
          element.second.Sample();// Start the running average off with the same last average sample
        }
      }
      else
      {
        for (auto& element : runningAverages)
          element.second.Sample();
      }

      double currentMap_mmHg = pulse->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg);
      double currentSystolic_mmHg = pulse->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg);
      double currentDiastolic_mmHg = pulse->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg);
      double currentCardiacOutput_mL_Per_min = pulse->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min);
      //double currentMeanCVP_mmHg = pulse->GetCardiovascularSystem()->GetMeanCentralVenousPressure(PressureUnit::mmHg);
      double currentBlood_mL = pulse->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL);
      double currentArterialO2P_mmHg = runningAverages.at("MeanArterialOxygenPartialPressure_mmHg").runningAverage.Value();

      //for (auto& element : runningAverages)
      //  pulse->GetEngineTracker()->GetDataTrack().Probe(element.first, element.second.instantaneousAverage);
      //pulse->GetEngineTracker()->TrackData(time_s);

      time_s += timeStep_s;
      stableTime_s += timeStep_s;
      if (GeneralMath::PercentDifference(previousMap_mmHg, currentMap_mmHg) > stabPercentTolerance)
      {
        stableTime_s = 0; previousMap_mmHg = currentMap_mmHg;
      }
      if (GeneralMath::PercentDifference(previousSystolic_mmHg, currentSystolic_mmHg) > stabPercentTolerance)
      {
        stableTime_s = 0; previousSystolic_mmHg = currentSystolic_mmHg;
      }
      if (GeneralMath::PercentDifference(previousDiastolic_mmHg, currentDiastolic_mmHg) > stabPercentTolerance)
      {
        stableTime_s = 0; previousDiastolic_mmHg = currentDiastolic_mmHg;
      }
      if (GeneralMath::PercentDifference(previousCardiacOutput_mL_Per_min, currentCardiacOutput_mL_Per_min) > stabPercentTolerance)
      {
        stableTime_s = 0; previousCardiacOutput_mL_Per_min = currentCardiacOutput_mL_Per_min;
      }
      //bool stableMeanCVP = true;
      //if (GeneralMath::PercentDifference(tgt_meanCVP_mmHg, meanCVP_mmHg) > 0.25)
      //  { stableTime_s = 0; tgt_meanCVP_mmHg = meanCVP_mmHg; stableMeanCVP = false; }
      if (GeneralMath::PercentDifference(previousBlood_mL, currentBlood_mL) > stabPercentTolerance)
      {
        stableTime_s = 0; previousBlood_mL = currentBlood_mL;
      }
      if (hasRespOverride)
      {
        if (GeneralMath::PercentDifference(previousArterialO2P_mmHg, currentArterialO2P_mmHg) > stabPercentToleranceO2)
        {
          stableTime_s = 0;
          previousArterialO2P_mmHg = currentArterialO2P_mmHg;
        }
      }

      if (stableTime_s > stabCheckTime_s)
      {
        stable = true;
        pulse->GetLogger()->Info("We are stable at " + cdm::to_string(time_s));
      }
      if (time_s > maxTime_s)
      {
        pulse->GetLogger()->Info("Could not stabilize this configuration");
        break;
      }
    }

    // Fill out our results
    sim.set_achievedstabilization(stable);
    sim.set_stabilizationtime_s(profiler.GetElapsedTime_s("Total"));
    sim.set_totalsimulationtime_s(time_s);
    sim.set_bloodvolume_ml(pulse->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL));
    sim.set_cardiacoutput_ml_per_min(pulse->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min));
    sim.set_diastolicarterialpressure_mmhg(pulse->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg));
    sim.set_heartejectionfraction(pulse->GetCardiovascularSystem()->GetHeartEjectionFraction());
    sim.set_heartstrokevolume_ml(pulse->GetCardiovascularSystem()->GetHeartStrokeVolume(VolumeUnit::mL));
    sim.set_meanaortainflow_ml_per_s(runningAverages.at("MeanAortaInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanarterialcarbondioxidepartialpressure_mmhg(pulse->GetCardiovascularSystem()->GetMeanArterialCarbonDioxidePartialPressure(PressureUnit::mmHg));
    sim.set_meanarterialoxygenpartialpressure_mmhg(runningAverages.at("MeanArterialOxygenPartialPressure_mmHg").instantaneousAverage);
    sim.set_meanarterialpressure_mmhg(pulse->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg));
    sim.set_meanbonevasculatureinflow_ml_per_s(runningAverages.at("MeanBoneVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanbrainvasculatureinflow_ml_per_s(runningAverages.at("MeanBrainVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meancentralvenouspressure_mmhg(pulse->GetCardiovascularSystem()->GetMeanCentralVenousPressure(PressureUnit::mmHg));
    sim.set_meanfatvasculatureinflow_ml_per_s(runningAverages.at("MeanFatVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meankidneyvasculatureinflow_ml_per_s(runningAverages.at("MeanKidneyVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanlargeintestinevasculatureinflow_ml_per_s(runningAverages.at("MeanLargeIntestineVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanleftarmvasculatureinflow_ml_per_s(runningAverages.at("MeanLeftArmVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanleftheartinflow_ml_per_s(runningAverages.at("MeanLeftHeartInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanleftheartpressure_mmhg(runningAverages.at("MeanLeftHeartPressure_mmHg").instantaneousAverage);
    sim.set_meanleftkidneyvasculatureinflow_ml_per_s(runningAverages.at("MeanLeftKidneyVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanleftlegvasculatureinflow_ml_per_s(runningAverages.at("MeanLeftLegVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanleftpulmonaryarteriesinflow_ml_per_s(runningAverages.at("MeanLeftPulmonaryArteriesInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanleftpulmonarycapillariesinflow_ml_per_s(runningAverages.at("MeanLeftPulmonaryCapillariesInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanleftpulmonaryveinsinflow_ml_per_s(runningAverages.at("MeanLeftPulmonaryVeinsInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanlivervasculatureinflow_ml_per_s(runningAverages.at("MeanLiverVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanmusclevasculatureinflow_ml_per_s(runningAverages.at("MeanMuscleVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanmyocardiumvasculatureinflow_ml_per_s(runningAverages.at("MeanMyocardiumVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanpulmonaryarteriesinflow_ml_per_s(runningAverages.at("MeanPulmonaryArteriesInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanpulmonarycapillariesinflow_ml_per_s(runningAverages.at("MeanPulmonaryCapillariesInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanpulmonarycapillariespressure_mmhg(runningAverages.at("MeanPulmonaryCapillariesPressure_mmHg").instantaneousAverage);
    sim.set_meanpulmonaryveinsinflow_ml_per_s(runningAverages.at("MeanPulmonaryVeinsInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanpulmonaryveinspressure_mmhg(runningAverages.at("MeanPulmonaryVeinsPressure_mmHg").instantaneousAverage);
    sim.set_meanrightarmvasculatureinflow_ml_per_s(runningAverages.at("MeanRightArmVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanrightheartinflow_ml_per_s(runningAverages.at("MeanRightHeartInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanrightheartpressure_mmhg(runningAverages.at("MeanRightHeartPressure_mmHg").instantaneousAverage);
    sim.set_meanrightkidneyvasculatureinflow_ml_per_s(runningAverages.at("MeanRightKidneyVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanrightlegvasculatureinflow_ml_per_s(runningAverages.at("MeanRightLegVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanrightpulmonaryarteriesinflow_ml_per_s(runningAverages.at("MeanRightPulmonaryArteriesInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanrightpulmonarycapillariesinflow_ml_per_s(runningAverages.at("MeanRightPulmonaryCapillariesInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanrightpulmonaryveinsinflow_ml_per_s(runningAverages.at("MeanRightPulmonaryVeinsInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanskinvasculatureinflow_ml_per_s(runningAverages.at("MeanSkinVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meansmallintestinevasculatureinflow_ml_per_s(runningAverages.at("MeanSmallIntestineVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meansplanchnicvasculatureinflow_ml_per_s(runningAverages.at("MeanSplanchnicVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanspleenvasculatureinflow_ml_per_s(runningAverages.at("MeanSpleenVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanvenacavainflow_ml_per_s(runningAverages.at("MeanVenaCavaInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanvenouscarbondioxidepartialpressure_mmhg(runningAverages.at("MeanVenousCarbonDioxidePartialPressure_mmHg").instantaneousAverage);
    sim.set_meanvenousoxygenpartialpressure_mmhg(runningAverages.at("MeanVenousOxygenPartialPressure_mmHg").instantaneousAverage);
    sim.set_pulmonarydiastolicarterialpressure_mmhg(pulse->GetCardiovascularSystem()->GetPulmonaryDiastolicArterialPressure(PressureUnit::mmHg));
    sim.set_pulmonarymeanarterialpressure_mmhg(pulse->GetCardiovascularSystem()->GetPulmonaryMeanArterialPressure(PressureUnit::mmHg));
    sim.set_pulmonarymeancapillaryflow_ml_per_s(pulse->GetCardiovascularSystem()->GetPulmonaryMeanCapillaryFlow(VolumePerTimeUnit::mL_Per_s));
    sim.set_pulmonarysystolicarterialpressure_mmhg(pulse->GetCardiovascularSystem()->GetPulmonarySystolicArterialPressure(PressureUnit::mmHg));
    sim.set_systolicarterialpressure_mmhg(pulse->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg));
    sim.set_systemicvascularresistance_mmhg_s_per_l(pulse->GetCardiovascularSystem()->GetSystemicVascularResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL));

    pulse->GetLogger()->Info("It took " + cdm::to_string(sim.stabilizationtime_s()) + "s to run this simulation");

    profiler.Clear();
    return true;
  }

  bool SARunner::RunSimulation(std::string const& outDir, pulse::study::bind::sensitivity_analysis::SimulationData& sim, const std::string& dataDir)
  {
    TimingProfile profiler;
    profiler.Start("Total");
    profiler.Start("Status");

    auto pulse = CreatePulseEngine();
    pulse->GetLogger()->LogToConsole(false); // No logging to console (when threaded)
    pulse->GetLogger()->SetLogFile(outDir + "/" + std::to_string(sim.id()) + " - " + sim.name() + ".log");

    // Setup data requests
    SEDataRequestManager drMgr(pulse->GetLogger());
    //drMgr.CreatePhysiologyDataRequest("BloodVolume", VolumeUnit::mL);
    //drMgr.CreatePhysiologyDataRequest("CardiacOutput", VolumePerTimeUnit::mL_Per_min);
    //drMgr.CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
    //drMgr.CreatePhysiologyDataRequest("HeartEjectionFraction");
    //drMgr.CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
    //drMgr.CreatePhysiologyDataRequest("HeartStrokeVolume", VolumeUnit::mL);
    //drMgr.CreatePhysiologyDataRequest("MeanArterialCarbonDioxidePartialPressure", PressureUnit::mmHg);
    //drMgr.CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
    //drMgr.CreatePhysiologyDataRequest("PulmonaryDiastolicArterialPressure", PressureUnit::mmHg);
    //drMgr.CreatePhysiologyDataRequest("PulmonaryMeanArterialPressure", PressureUnit::mmHg);
    //drMgr.CreatePhysiologyDataRequest("PulmonaryMeanCapillaryFlow", VolumePerTimeUnit::mL_Per_min);
    //drMgr.CreatePhysiologyDataRequest("PulmonarySystolicArterialPressure", PressureUnit::mmHg);
    //drMgr.CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
    //drMgr.CreatePhysiologyDataRequest("SystemicVascularResistance", PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    //drMgr.SetResultsFilename(outDir + "/" + cdm::to_string(sim.id()) + " - " + sim.name() + ".csv");

    // TODO amb Clean this up (cfg should have a default ctor that makes its own Sub Mgr)
    PulseConfiguration cfg(pulse->GetLogger());
    // Serialize Overrides
    for (auto& [name, m] : sim.overrides())
      cfg.GetOverrides()[name] = SEScalarPair(m.value(), m.unit());
    //cfg.SetBaroreceptorFeedback(eSwitch::Off);
    //cfg.SetChemoreceptorFeedback(eSwitch::Off);
    pulse->SetConfigurationOverride(&cfg);

    SEPatientConfiguration pc;
    pc.SetPatientFile("./patients/StandardMale.json");
    if (!pulse->InitializeEngine(pc, &drMgr)) return false;

    // check if overriding respiratory path
    pulse::Controller& pctrl =
      ((pulse::Engine*)pulse.get())->GetController();
    SEFluidCircuit& resp = pctrl.GetCircuits().GetRespiratoryCircuit();
    bool hasRespOverride = false;
    for (auto const& [name, o] : cfg.GetOverrides())
    {
      if (resp.HasPath(name))
      {
        hasRespOverride = true;
        break;
      }
    }

    std::unordered_map<std::string, RunningAverages> runningAverages =
    {
      {"MeanAortaInflow_mL_Per_s", RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Aorta), VolumePerTimeUnit::mL_Per_s)},
      {"MeanArterialOxygenPartialPressure_mmHg",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Aorta)->GetSubstanceQuantity(*pulse->GetSubstanceManager().GetSubstance("Oxygen")), PressureUnit::mmHg)},
      {"MeanBoneVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Bone), VolumePerTimeUnit::mL_Per_s)},
      {"MeanBrainVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Brain), VolumePerTimeUnit::mL_Per_s)},
      {"MeanFatVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Fat), VolumePerTimeUnit::mL_Per_s)},
      {"MeanKidneyVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Kidneys), VolumePerTimeUnit::mL_Per_s)},
      {"MeanLargeIntestineVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LargeIntestine), VolumePerTimeUnit::mL_Per_s)},
      {"MeanLeftArmVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftArm), VolumePerTimeUnit::mL_Per_s)},
      {"MeanLeftHeartInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftHeart), VolumePerTimeUnit::mL_Per_s)},
      {"MeanLeftHeartPressure_mmHg",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftHeart), PressureUnit::mmHg)},
      {"MeanLeftKidneyVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftKidney), VolumePerTimeUnit::mL_Per_s)},
      {"MeanLeftLegVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftLeg), VolumePerTimeUnit::mL_Per_s)},
      {"MeanLeftPulmonaryArteriesInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftPulmonaryArteries), VolumePerTimeUnit::mL_Per_s)},
      {"MeanLeftPulmonaryCapillariesInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftPulmonaryCapillaries), VolumePerTimeUnit::mL_Per_s)},
      {"MeanLeftPulmonaryVeinsInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftPulmonaryVeins), VolumePerTimeUnit::mL_Per_s)},
      {"MeanLiverVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Liver), VolumePerTimeUnit::mL_Per_s)},
      {"MeanMuscleVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Muscle), VolumePerTimeUnit::mL_Per_s)},
      {"MeanMyocardiumVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Myocardium), VolumePerTimeUnit::mL_Per_s)},
      {"MeanPulmonaryArteriesInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::PulmonaryArteries), VolumePerTimeUnit::mL_Per_s)},
      {"MeanPulmonaryCapillariesInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::PulmonaryCapillaries), VolumePerTimeUnit::mL_Per_s)},
      {"MeanPulmonaryCapillariesPressure_mmHg",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::PulmonaryCapillaries), PressureUnit::mmHg)},
      {"MeanPulmonaryVeinsInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::PulmonaryVeins), VolumePerTimeUnit::mL_Per_s)},
      {"MeanPulmonaryVeinsPressure_mmHg",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::PulmonaryVeins), PressureUnit::mmHg)},
      {"MeanRightArmVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightArm), VolumePerTimeUnit::mL_Per_s)},
      {"MeanRightHeartInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightHeart), VolumePerTimeUnit::mL_Per_s)},
      {"MeanRightHeartPressure_mmHg",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightHeart), PressureUnit::mmHg)},
      {"MeanRightKidneyVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightKidney), VolumePerTimeUnit::mL_Per_s)},
      {"MeanRightLegVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightLeg), VolumePerTimeUnit::mL_Per_s)},
      {"MeanRightPulmonaryArteriesInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightPulmonaryArteries), VolumePerTimeUnit::mL_Per_s)},
      {"MeanRightPulmonaryCapillariesInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightPulmonaryCapillaries), VolumePerTimeUnit::mL_Per_s)},
      {"MeanRightPulmonaryVeinsInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightPulmonaryVeins), VolumePerTimeUnit::mL_Per_s)},
      {"MeanSkinVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Skin), VolumePerTimeUnit::mL_Per_s)},
      {"MeanSmallIntestineVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::SmallIntestine), VolumePerTimeUnit::mL_Per_s)},
      {"MeanSplanchnicVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Splanchnic), VolumePerTimeUnit::mL_Per_s)},
      {"MeanSpleenVasculatureInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Spleen), VolumePerTimeUnit::mL_Per_s)},
      {"MeanVenaCavaInflow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::VenaCava), VolumePerTimeUnit::mL_Per_s)},
      {"MeanVenousCarbonDioxidePartialPressure_mmHg",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::VenaCava)->GetSubstanceQuantity(*pulse->GetSubstanceManager().GetSubstance("CarbonDioxide")), PressureUnit::mmHg)},
      {"MeanVenousOxygenPartialPressure_mmHg",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::VenaCava)->GetSubstanceQuantity(*pulse->GetSubstanceManager().GetSubstance("Oxygen")), PressureUnit::mmHg)}
    };

    for (auto& element : runningAverages)
    {
      element.second.Sample();
      element.second.instantaneousAverage = element.second.runningAverage.Value();
    }

    // Run until stable
    // Let's shoot for within 0.25% for 10s straight
    double timeStep_s = pulse->GetTimeStep(TimeUnit::s);
    double stabPercentTolerance = 0.25;
    double stabPercentToleranceO2 = 0.05;
    double stabCheckTime_s = 5.0;
    double time_s = 0;
    double maxTime_s = 2000;
    // Here are our variable we will check for stability
    double previousMap_mmHg = pulse->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg);
    double previousSystolic_mmHg = pulse->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg);
    double previousDiastolic_mmHg = pulse->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg);
    double previousCardiacOutput_mL_Per_min = pulse->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min);
    //double previousMeanCVP_mmHg = pulse->GetCardiovascularSystem()->GetMeanCentralVenousPressure(PressureUnit::mmHg);
    double previousBlood_mL = pulse->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL);
    double previousArterialO2P_mmHg = runningAverages.at("MeanArterialOxygenPartialPressure_mmHg").runningAverage.Value();

    bool stable = false;
    double stableTime_s = 0;

    while (!stable)
    {
      pulse->AdvanceModelTime(timeStep_s, TimeUnit::s);
      if (pulse->GetEventManager().IsEventActive(eEvent::StartOfCardiacCycle))
      {
        for (auto& element : runningAverages)
        {
          element.second.Sample();
          element.second.instantaneousAverage = element.second.runningAverage.Value();
          element.second.runningAverage.Invalidate();
          element.second.Sample();// Start the running average off with the same last average sample
        }
      }
      else
      {
        for (auto& element : runningAverages)
          element.second.Sample();
      }

      double currentMap_mmHg = pulse->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg);
      double currentSystolic_mmHg = pulse->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg);
      double currentDiastolic_mmHg = pulse->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg);
      double currentCardiacOutput_mL_Per_min = pulse->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min);
      //double currentMeanCVP_mmHg = pulse->GetCardiovascularSystem()->GetMeanCentralVenousPressure(PressureUnit::mmHg);
      double currentBlood_mL = pulse->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL);
      double currentArterialO2P_mmHg = runningAverages.at("MeanArterialOxygenPartialPressure_mmHg").runningAverage.Value();

      //for (auto& element : runningAverages)
      //  pulse->GetEngineTracker()->GetDataTrack().Probe(element.first, element.second.instantaneousAverage);
      //pulse->GetEngineTracker()->TrackData(time_s);

      time_s += timeStep_s;
      stableTime_s += timeStep_s;
      if (GeneralMath::PercentDifference(previousMap_mmHg, currentMap_mmHg) > stabPercentTolerance)
      {
        stableTime_s = 0; previousMap_mmHg = currentMap_mmHg;
      }
      if (GeneralMath::PercentDifference(previousSystolic_mmHg, currentSystolic_mmHg) > stabPercentTolerance)
      {
        stableTime_s = 0; previousSystolic_mmHg = currentSystolic_mmHg;
      }
      if (GeneralMath::PercentDifference(previousDiastolic_mmHg, currentDiastolic_mmHg) > stabPercentTolerance)
      {
        stableTime_s = 0; previousDiastolic_mmHg = currentDiastolic_mmHg;
      }
      if (GeneralMath::PercentDifference(previousCardiacOutput_mL_Per_min, currentCardiacOutput_mL_Per_min) > stabPercentTolerance)
      {
        stableTime_s = 0; previousCardiacOutput_mL_Per_min = currentCardiacOutput_mL_Per_min;
      }
      //bool stableMeanCVP = true;
      //if (GeneralMath::PercentDifference(tgt_meanCVP_mmHg, meanCVP_mmHg) > 0.25)
      //  { stableTime_s = 0; tgt_meanCVP_mmHg = meanCVP_mmHg; stableMeanCVP = false; }
      if (GeneralMath::PercentDifference(previousBlood_mL, currentBlood_mL) > stabPercentTolerance)
      {
        stableTime_s = 0; previousBlood_mL = currentBlood_mL;
      }
      if (hasRespOverride)
      {
        if (GeneralMath::PercentDifference(previousArterialO2P_mmHg, currentArterialO2P_mmHg) > stabPercentToleranceO2)
        {
          stableTime_s = 0; previousArterialO2P_mmHg = currentArterialO2P_mmHg;
        }
      }

      if (stableTime_s > stabCheckTime_s)
      {
        stable = true;
        pulse->GetLogger()->Info("We are stable at " + cdm::to_string(time_s));
      }
      if (time_s > maxTime_s)
      {
        pulse->GetLogger()->Info("Could not stabilize this configuration");
        break;
      }
    }

    // Fill out our results
    sim.set_achievedstabilization(stable);
    sim.set_stabilizationtime_s(profiler.GetElapsedTime_s("Total"));
    sim.set_totalsimulationtime_s(time_s);
    sim.set_bloodvolume_ml(pulse->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL));
    sim.set_cardiacoutput_ml_per_min(pulse->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min));
    sim.set_diastolicarterialpressure_mmhg(pulse->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg));
    sim.set_heartejectionfraction(pulse->GetCardiovascularSystem()->GetHeartEjectionFraction());
    sim.set_heartstrokevolume_ml(pulse->GetCardiovascularSystem()->GetHeartStrokeVolume(VolumeUnit::mL));
    sim.set_meanaortainflow_ml_per_s(runningAverages.at("MeanAortaInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanarterialcarbondioxidepartialpressure_mmhg(pulse->GetCardiovascularSystem()->GetMeanArterialCarbonDioxidePartialPressure(PressureUnit::mmHg));
    sim.set_meanarterialoxygenpartialpressure_mmhg(runningAverages.at("MeanArterialOxygenPartialPressure_mmHg").instantaneousAverage);
    sim.set_meanarterialpressure_mmhg(pulse->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg));
    sim.set_meanbonevasculatureinflow_ml_per_s(runningAverages.at("MeanBoneVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanbrainvasculatureinflow_ml_per_s(runningAverages.at("MeanBrainVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meancentralvenouspressure_mmhg(pulse->GetCardiovascularSystem()->GetMeanCentralVenousPressure(PressureUnit::mmHg));
    sim.set_meanfatvasculatureinflow_ml_per_s(runningAverages.at("MeanFatVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meankidneyvasculatureinflow_ml_per_s(runningAverages.at("MeanKidneyVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanlargeintestinevasculatureinflow_ml_per_s(runningAverages.at("MeanLargeIntestineVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanleftarmvasculatureinflow_ml_per_s(runningAverages.at("MeanLeftArmVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanleftheartinflow_ml_per_s(runningAverages.at("MeanLeftHeartInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanleftheartpressure_mmhg(runningAverages.at("MeanLeftHeartPressure_mmHg").instantaneousAverage);
    sim.set_meanleftkidneyvasculatureinflow_ml_per_s(runningAverages.at("MeanLeftKidneyVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanleftlegvasculatureinflow_ml_per_s(runningAverages.at("MeanLeftLegVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanleftpulmonaryarteriesinflow_ml_per_s(runningAverages.at("MeanLeftPulmonaryArteriesInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanleftpulmonarycapillariesinflow_ml_per_s(runningAverages.at("MeanLeftPulmonaryCapillariesInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanleftpulmonaryveinsinflow_ml_per_s(runningAverages.at("MeanLeftPulmonaryVeinsInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanlivervasculatureinflow_ml_per_s(runningAverages.at("MeanLiverVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanmusclevasculatureinflow_ml_per_s(runningAverages.at("MeanMuscleVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanmyocardiumvasculatureinflow_ml_per_s(runningAverages.at("MeanMyocardiumVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanpulmonaryarteriesinflow_ml_per_s(runningAverages.at("MeanPulmonaryArteriesInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanpulmonarycapillariesinflow_ml_per_s(runningAverages.at("MeanPulmonaryCapillariesInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanpulmonarycapillariespressure_mmhg(runningAverages.at("MeanPulmonaryCapillariesPressure_mmHg").instantaneousAverage);
    sim.set_meanpulmonaryveinsinflow_ml_per_s(runningAverages.at("MeanPulmonaryVeinsInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanpulmonaryveinspressure_mmhg(runningAverages.at("MeanPulmonaryVeinsPressure_mmHg").instantaneousAverage);
    sim.set_meanrightarmvasculatureinflow_ml_per_s(runningAverages.at("MeanRightArmVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanrightheartinflow_ml_per_s(runningAverages.at("MeanRightHeartInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanrightheartpressure_mmhg(runningAverages.at("MeanRightHeartPressure_mmHg").instantaneousAverage);
    sim.set_meanrightkidneyvasculatureinflow_ml_per_s(runningAverages.at("MeanRightKidneyVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanrightlegvasculatureinflow_ml_per_s(runningAverages.at("MeanRightLegVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanrightpulmonaryarteriesinflow_ml_per_s(runningAverages.at("MeanRightPulmonaryArteriesInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanrightpulmonarycapillariesinflow_ml_per_s(runningAverages.at("MeanRightPulmonaryCapillariesInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanrightpulmonaryveinsinflow_ml_per_s(runningAverages.at("MeanRightPulmonaryVeinsInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanskinvasculatureinflow_ml_per_s(runningAverages.at("MeanSkinVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meansmallintestinevasculatureinflow_ml_per_s(runningAverages.at("MeanSmallIntestineVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meansplanchnicvasculatureinflow_ml_per_s(runningAverages.at("MeanSplanchnicVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanspleenvasculatureinflow_ml_per_s(runningAverages.at("MeanSpleenVasculatureInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanvenacavainflow_ml_per_s(runningAverages.at("MeanVenaCavaInflow_mL_Per_s").instantaneousAverage);
    sim.set_meanvenouscarbondioxidepartialpressure_mmhg(runningAverages.at("MeanVenousCarbonDioxidePartialPressure_mmHg").instantaneousAverage);
    sim.set_meanvenousoxygenpartialpressure_mmhg(runningAverages.at("MeanVenousOxygenPartialPressure_mmHg").instantaneousAverage);
    sim.set_pulmonarydiastolicarterialpressure_mmhg(pulse->GetCardiovascularSystem()->GetPulmonaryDiastolicArterialPressure(PressureUnit::mmHg));
    sim.set_pulmonarymeanarterialpressure_mmhg(pulse->GetCardiovascularSystem()->GetPulmonaryMeanArterialPressure(PressureUnit::mmHg));
    sim.set_pulmonarymeancapillaryflow_ml_per_s(pulse->GetCardiovascularSystem()->GetPulmonaryMeanCapillaryFlow(VolumePerTimeUnit::mL_Per_s));
    sim.set_pulmonarysystolicarterialpressure_mmhg(pulse->GetCardiovascularSystem()->GetPulmonarySystolicArterialPressure(PressureUnit::mmHg));
    sim.set_systolicarterialpressure_mmhg(pulse->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg));
    sim.set_systemicvascularresistance_mmhg_s_per_l(pulse->GetCardiovascularSystem()->GetSystemicVascularResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL));

    pulse->GetLogger()->Info("It took " + cdm::to_string(sim.stabilizationtime_s()) + "s to run this simulation");

    profiler.Clear();
    return true;
  }

  pulse::study::bind::sensitivity_analysis::SimulationData* SARunner::GetNextSimulation()
  {
    m_mutex.lock();
    pulse::study::bind::sensitivity_analysis::SimulationData* sim = nullptr;
    if (!m_SimulationsToRun.empty())
    {
      auto id = *m_SimulationsToRun.begin();
      for (int i = 0; i < m_SimulationList->simulation_size(); i++)
      {
        sim = &(*m_SimulationList->mutable_simulation())[i];
        if (sim->id() == id)
          break;
      }
      Info("Simulating Run " + std::to_string(id) + " : " + sim->name());
      m_SimulationsToRun.erase(id);
    }
    m_mutex.unlock();
    return sim;
  }

  void SARunner::FinalizeSimulation(pulse::study::bind::sensitivity_analysis::SimulationData& sim)
  {
    m_mutex.lock();
    auto rSim = m_SimulationResultsList->mutable_simulation()->Add();
    rSim->CopyFrom(sim);
    SerializeToFile(*m_SimulationResultsList, m_SimulationResultsListFile, eSerializationFormat::JSON);
    Info("Completed Simulation " + std::to_string(m_SimulationResultsList->simulation_size()) + " of " + std::to_string(m_SimulationList->simulation_size()));
    if (sim.achievedstabilization())
      Info("  Stabilized Run " + std::to_string(sim.id()) + " : " + sim.name());
    else
      Info("  FAILED STABILIZATION FOR RUN " + std::to_string(sim.id()) + " : " + sim.name());
    m_mutex.unlock();
  }

  bool SARunner::SerializeToString(pulse::study::bind::sensitivity_analysis::SimulationListData& src, std::string& output, eSerializationFormat f) const
  {
    google::protobuf::util::JsonPrintOptions printOpts;
    printOpts.add_whitespace = true;
    printOpts.preserve_proto_field_names = true;
    printOpts.always_print_fields_with_no_presence = true;
    if (!google::protobuf::util::MessageToJsonString(src, &output, printOpts).ok())
    {
      Error("Unable to serialize simulation list");
      return false;
    }
    return true;
  }
  bool SARunner::SerializeToFile(pulse::study::bind::sensitivity_analysis::SimulationListData& src, const std::string& filename, eSerializationFormat f) const
  {
    std::string content;
    if (!SerializeToString(src, content, f))
      return false;
    return WriteFile(content, filename);
  }
  bool SARunner::SerializeFromString(const std::string& src, pulse::study::bind::sensitivity_analysis::SimulationListData& dst, eSerializationFormat f)
  {
    google::protobuf::util::JsonParseOptions parseOpts;
    auto status = google::protobuf::util::JsonStringToMessage(src, &dst, parseOpts);
    if (!status.ok())
    {
      PBUtils::LogError("SARunner::SerializeFromString", status.ToString(), GetLogger());
      return false;
    }
    return true;
  }
  bool SARunner::SerializeFromFile(const std::string& filename, pulse::study::bind::sensitivity_analysis::SimulationListData& dst, eSerializationFormat f)
  {
    std::string content;
    if (ReadFile(filename, content))
    {
      Error("Unable to read file " + filename);
      return false;
    }
    return SerializeFromString(content, dst, f);
  }
}
