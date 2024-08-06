/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "HRunner.h"

#include "PulseConfiguration.h"
#include "engine/common/controller/CircuitManager.h"
#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/engine/SEEventManager.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/io/protobuf/PBUtils.h"

namespace pulse::study::hydrocephalus
{

  HRunner::HRunner(const std::string& logfileName, const std::string& dataDir) : Loggable(logfileName)
  {
    m_DataDir = dataDir;
    m_SimulationList = nullptr;
    m_SimulationResultsList = nullptr;
  }
  HRunner::~HRunner()
  {
    SAFE_DELETE(m_SimulationList);
    SAFE_DELETE(m_SimulationResultsList);
  }

  bool HRunner::Run(pulse::study::bind::hydrocephalus::SimulationListData& simList)
  {
    m_OutDir = simList.outputrootdir();
    m_SimulationResultsListFile = m_OutDir + "/simlist_results.json";
    SAFE_DELETE(m_SimulationList);
    SAFE_DELETE(m_SimulationResultsList);
    m_SimulationList = &simList;
    m_SimulationResultsList = new pulse::study::bind::hydrocephalus::SimulationListData();
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

  bool HRunner::Run(const std::string& filename, eSerializationFormat f)
  {
    SAFE_DELETE(m_SimulationList);
    SAFE_DELETE(m_SimulationResultsList);
    m_SimulationList = new pulse::study::bind::hydrocephalus::SimulationListData();
    m_SimulationResultsList = new pulse::study::bind::hydrocephalus::SimulationListData();

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
  bool HRunner::Run()
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

    size_t numSimsToRun = m_SimulationList->simulation_size() - m_SimulationResultsList->simulation_size();
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
      m_Threads.push_back(std::thread(&HRunner::ControllerLoop, this));

    for (size_t p = 0; p < processor_count; p++)
      m_Threads[p].join();

    Info("It took " + cdm::to_string(profiler.GetElapsedTime_s("Total")) + "s to run this simulation list");
    profiler.Clear();
    return true;
  }

  void HRunner::ControllerLoop()
  {
    pulse::study::bind::hydrocephalus::SimulationData* sim = nullptr;
    while (true)
    {
      try
      {
        sim = GetNextSimulation();
        if (sim == nullptr)
          break;
        RunSimulationUntilStable(m_OutDir, *sim);
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

  bool HRunner::RunSimulationUntilStable(std::string const& outDir, pulse::study::bind::hydrocephalus::SimulationData& sim, const std::string& dataDir)
  {
    TimingProfile profiler;
    profiler.Start("Total");
    profiler.Start("Status");

    auto pulse = CreatePulseEngine();
    pulse->GetLogger()->LogToConsole(true);
    pulse->GetLogger()->SetLogFile(outDir + "/" + std::to_string(sim.id()) + " - " + sim.name() + ".log");

    // Setup data requests
    pulse->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("CardiacOutput", VolumePerTimeUnit::mL_Per_min);
    pulse->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("BloodVolume", VolumeUnit::mL);
    pulse->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
    pulse->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
    pulse->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("IntracranialPressure", PressureUnit::mmHg);
    pulse->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("CerebralPerfusionPressure", PressureUnit::mmHg);
    pulse->GetEngineTracker()->GetDataRequestManager().CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::Brain, "Pressure", PressureUnit::mmHg);
    pulse->GetEngineTracker()->GetDataRequestManager().CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::Brain, "Volume", VolumeUnit::mL);
    pulse->GetEngineTracker()->GetDataRequestManager().CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::Brain, "InFlow", VolumePerTimeUnit::mL_Per_min);
    pulse->GetEngineTracker()->GetDataRequestManager().CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::Brain, "Oxygen", "PartialPressure");
    pulse->GetEngineTracker()->GetDataRequestManager().CreateLiquidCompartmentDataRequest(pulse::CerebrospinalFluidCompartment::IntracranialSpace, "Volume", VolumeUnit::mL);
    pulse->GetEngineTracker()->GetDataRequestManager().CreateLiquidCompartmentDataRequest(pulse::CerebrospinalFluidCompartment::IntracranialSpace, "Pressure", PressureUnit::mmHg);
    pulse->GetEngineTracker()->GetDataRequestManager().SetResultsFilename(outDir + "/" + std::to_string(sim.id()) + " - " + sim.name() + ".csv");

    // Setup Circuit Overrides
    PulseConfiguration cfg(pulse->GetLogger());
    cfg.GetOverrides()[pulse::CerebrospinalFluidNode::IntracranialSpace2] = SEScalarPair(sim.intracranialspacevolumebaseline_ml(), VolumeUnit::mL);
    cfg.GetOverrides()[pulse::CerebrospinalFluidPath::IntracranialSpace1ToIntracranialSpace2] = SEScalarPair(sim.intracranialspacecompliance_ml_per_mmhg(), VolumePerPressureUnit::mL_Per_mmHg);
    cfg.GetOverrides()["CerebrospinalFluidAbsorptionRate"] = SEScalarPair(sim.cerebrospinalfluidabsorptionrate_ml_per_min(), VolumePerTimeUnit::mL_Per_min);
    cfg.GetOverrides()["CerebrospinalFluidProductionRate"] = SEScalarPair(sim.cerebrospinalfluidproductionrate_ml_per_min(), VolumePerTimeUnit::mL_Per_min);
    pulse->SetConfigurationOverride(&cfg);

    // Stabilize the engine
    SEPatientConfiguration pc;
    pc.SetPatientFile("./patients/StandardMale.json");
    if (!pulse->InitializeEngine(pc))
    {
      sim.set_achievedstabilization(false);
      sim.set_stabilizationtime_s(profiler.GetElapsedTime_s("Total"));
      sim.set_totalsimulationtime_s(0);
      return false;
    }

    std::unordered_map<std::string, RunningAverages> runningAverages =
    {
      {"MeanBrainPressure_mmHg", RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Brain), PressureUnit::mmHg)},
      {"MeanBrainVasculatureInFlow_mL_Per_s", RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Brain), VolumePerTimeUnit::mL_Per_s)},
      {"MeanBrainVasculatureOutFlow_mL_Per_s",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Brain), VolumePerTimeUnit::mL_Per_s)},
      {"MeanBrainCarbonDioxidePartialPressure_mmHg",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Brain)->GetSubstanceQuantity(*pulse->GetSubstanceManager().GetSubstance("CarbonDioxide")), PressureUnit::mmHg)},
      {"MeanBrainOxygenPartialPressure_mmHg",  RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Brain)->GetSubstanceQuantity(*pulse->GetSubstanceManager().GetSubstance("Oxygen")), PressureUnit::mmHg)},
      {"MeanIntracranialSpacePressure_mmHg", RunningAverages(pulse->GetCompartments().GetLiquidCompartment(pulse::CerebrospinalFluidCompartment::IntracranialSpace), PressureUnit::mmHg)},
    };

    for (auto& element : runningAverages)
    {
      element.second.Sample();
      element.second.instantaneousAverage = element.second.runningAverage.Value();
    }


    double timeStep_s = pulse->GetTimeStep(TimeUnit::s);
    size_t steps = (size_t)(30 / timeStep_s);

    for (size_t i = 0; i < steps; i++)
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

      pulse->GetEngineTracker()->TrackData(pulse->GetSimulationTime(TimeUnit::s));
    }

    const SELiquidCompartment* b = pulse->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Brain);
    const SELiquidCompartment* is = pulse->GetCompartments().GetLiquidCompartment(pulse::CerebrospinalFluidCompartment::IntracranialSpace);

    // Fill out our results
    sim.set_achievedstabilization(true);
    sim.set_totalsimulationtime_s(30);
    sim.set_stabilizationtime_s(profiler.GetElapsedTime_s("Total"));
    sim.set_brainvolume_ml(b->GetVolume(VolumeUnit::mL));
    sim.set_intracranialspacevolume_ml(is->GetVolume(VolumeUnit::mL));

    sim.set_meanbrainpressure_mmhg(runningAverages.at("MeanBrainPressure_mmHg").instantaneousAverage);
    sim.set_meanbrainvasculatureinflow_ml_per_s(runningAverages.at("MeanBrainVasculatureInFlow_mL_Per_s").instantaneousAverage);
    sim.set_meanbrainvasculatureoutflow_ml_per_s(runningAverages.at("MeanBrainVasculatureOutFlow_mL_Per_s").instantaneousAverage);
    sim.set_meanbraincarbondioxidepartialpressure_mmhg(runningAverages.at("MeanBrainCarbonDioxidePartialPressure_mmHg").instantaneousAverage);
    sim.set_meanbrainoxygenpartialpressure_mmhg(runningAverages.at("MeanBrainOxygenPartialPressure_mmHg").instantaneousAverage);
    sim.set_meanintracranialspacepressure_mmhg(runningAverages.at("MeanIntracranialSpacePressure_mmHg").instantaneousAverage);

    sim.set_cardiacoutput_ml_per_min(pulse->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min));
    sim.set_cerebralbloodflow_ml_per_min(pulse->GetCardiovascularSystem()->GetCerebralBloodFlow(VolumePerTimeUnit::mL_Per_min));
    sim.set_cerebralperfusionpressure_mmhg(pulse->GetCardiovascularSystem()->GetCerebralPerfusionPressure(PressureUnit::mmHg));
    sim.set_diastolicarterialpressure_mmhg(pulse->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg));
    sim.set_heartejectionfraction(pulse->GetCardiovascularSystem()->GetHeartEjectionFraction());
    sim.set_intracranialpressure_mmhg(pulse->GetCardiovascularSystem()->GetIntracranialPressure(PressureUnit::mmHg));
    sim.set_heartrate_bpm(pulse->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min));
    sim.set_heartstrokevolume_ml(pulse->GetCardiovascularSystem()->GetHeartStrokeVolume(VolumeUnit::mL));
    sim.set_meanarterialpressure_mmhg(pulse->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg));
    sim.set_meancentralvenouspressure_mmhg(pulse->GetCardiovascularSystem()->GetMeanCentralVenousPressure(PressureUnit::mmHg));
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

  pulse::study::bind::hydrocephalus::SimulationData* HRunner::GetNextSimulation()
  {
    m_mutex.lock();
    pulse::study::bind::hydrocephalus::SimulationData* sim = nullptr;
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

  void HRunner::FinalizeSimulation(pulse::study::bind::hydrocephalus::SimulationData& sim)
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

  bool HRunner::SerializeToString(pulse::study::bind::hydrocephalus::SimulationListData& src, std::string& output, eSerializationFormat f) const
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
  bool HRunner::SerializeToFile(pulse::study::bind::hydrocephalus::SimulationListData& src, const std::string& filename, eSerializationFormat f) const
  {
    std::string content;
    if (!SerializeToString(src, content, f))
      return false;
    return WriteFile(content, filename);
  }
  bool HRunner::SerializeFromString(const std::string& src, pulse::study::bind::hydrocephalus::SimulationListData& dst, eSerializationFormat f)
  {
    google::protobuf::util::JsonParseOptions parseOpts;
    auto status = google::protobuf::util::JsonStringToMessage(src, &dst, parseOpts);
    if (!status.ok())
    {
      PBUtils::LogError("HRunner::SerializeFromString", status.ToString(), GetLogger());
      return false;
    }
    return true;
  }
  bool HRunner::SerializeFromFile(const std::string& filename, pulse::study::bind::hydrocephalus::SimulationListData& dst, eSerializationFormat f)
  {
    std::string content;
    if (!ReadFile(filename, content))
    {
      Error("Unable to read file " + filename);
      return false;
    }
    return SerializeFromString(content, dst, f);
  }
}
