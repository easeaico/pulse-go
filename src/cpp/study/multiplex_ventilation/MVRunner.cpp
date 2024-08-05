/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "MVRunner.h"
#include "cdm/io/protobuf/PBUtils.h"

using namespace pulse;

namespace pulse::study::multiplex_ventilation
{
  MVRunner::MVRunner(const std::string& logfileName, const std::string& dataDir) : Loggable(logfileName)
  {
    m_DataDir = dataDir;
    m_SimulationList = nullptr;
    m_SimulationResultsList = nullptr;
  }
  MVRunner::~MVRunner()
  {
    SAFE_DELETE(m_SimulationList);
    SAFE_DELETE(m_SimulationResultsList);
  }

  bool MVRunner::Run(pulse::study::bind::multiplex_ventilation::SimulationListData& simList, Mode m, const std::string& resultsFilename)
  {
    m_Mode = m;
    m_SimulationResultsListFile = simList.outputrootdir() + "/" + resultsFilename;
    SAFE_DELETE(m_SimulationList);
    SAFE_DELETE(m_SimulationResultsList);
    m_SimulationList = &simList;
    m_SimulationResultsList = new pulse::study::bind::multiplex_ventilation::SimulationListData();
    if (FileExists(m_SimulationResultsListFile))
    {
      if (!SerializeFromFile(m_SimulationResultsListFile, *m_SimulationResultsList))
      {
        GetLogger()->Warning("Unable to read found results file");
      }
    }
    bool b = Run();
    m_SimulationList = nullptr;
    SAFE_DELETE(m_SimulationResultsList);
    return b;
  }

  bool MVRunner::Run(const std::string& filename, Mode m)
  {
    SAFE_DELETE(m_SimulationList);
    SAFE_DELETE(m_SimulationResultsList);
    m_SimulationList = new pulse::study::bind::multiplex_ventilation::SimulationListData();
    m_SimulationResultsList = new pulse::study::bind::multiplex_ventilation::SimulationListData();

    m_Mode = m;
    if (!SerializeFromFile(filename, *m_SimulationList))
      return false;
    // Let's try to read in a results file
    m_SimulationResultsListFile = filename.substr(0, filename.length() - 5) + "_results.json";
    if (FileExists(m_SimulationResultsListFile))
    {
      if (!SerializeFromFile(m_SimulationResultsListFile, *m_SimulationResultsList))
      {
        GetLogger()->Warning("Unable to read found results file");
      }
    }
    return Run();
  }
  bool MVRunner::Run()
  {
    TimingProfile profiler;
    profiler.Start("Total");

    // Get the ID's of simulations we need to run
    m_SimulationsToRun.clear();
    for (int i = 0; i < m_SimulationList->simulations_size(); i++)
      m_SimulationsToRun.insert(m_SimulationList->simulations()[i].id());
    // Remove any id's we have in the results
    if (m_SimulationResultsList->simulations_size() > 0)
    {
      Info("Found " + std::to_string(m_SimulationResultsList->simulations_size()) + " previously run simulations");
      for (int i = 0; i < m_SimulationResultsList->simulations_size(); i++)
        m_SimulationsToRun.erase(m_SimulationResultsList->simulations()[i].id());
    }

    size_t numSimsToRun = m_SimulationList->simulations_size() - m_SimulationResultsList->simulations_size();
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
      m_Threads.push_back(std::thread(&MVRunner::ControllerLoop, this));

    for (size_t p = 0; p < processor_count; p++)
      m_Threads[p].join();

    Info("It took " + std::to_string(profiler.GetElapsedTime_s("Total")) + "s to run this simulation list");
    profiler.Clear();
    return true;
  }

  void MVRunner::ControllerLoop()
  {
    pulse::study::bind::multiplex_ventilation::SimulationData* sim = nullptr;
    while (true)
    {
      try
      {
        sim = GetNextSimulation();
        if (sim == nullptr)
          break;
        if (m_Mode == Mode::StableSpO2)
          RunSimulationToStableSpO2(*sim);
        else
          StepSimulationFiO2(*sim);
        FinalizeSimulation(*sim);
      }
      catch (CommonDataModelException& cdm_ex)
      {
        GetLogger()->Fatal("Exception caught runnning simulation " + sim->outputbasefilename());
        GetLogger()->Fatal(cdm_ex.what());
        std::cerr << cdm_ex.what() << std::endl;
      }
      catch (std::exception& ex)
      {
        GetLogger()->Fatal("Exception caught runnning simulation " + sim->outputbasefilename());
        GetLogger()->Fatal(ex.what());
        std::cerr << ex.what() << std::endl;
      }
      catch (...)
      {
        std::cerr << "Unable to run simulation " << sim->outputbasefilename() << std::endl;
      }
    }
  }

  bool MVRunner::StepSimulationFiO2(pulse::study::bind::multiplex_ventilation::SimulationData& sim, const std::string& dataDir)
  {
    TimingProfile profiler;
    profiler.Start("Total");
    profiler.Start("Status");

    MVEngine mve(sim.outputbasefilename() + "runner_thread.log", false, dataDir);
    if (!mve.CreateEngine(sim))
      return false;

    double currentFiO2 = sim.fio2();

    double timeStep_s = mve.m_TimeStep_s;
    double currentTime_s = 0;

    bool stabilized = false;
    int stabilizationPasses = 0;
    int totalIterations = 0;
    double stabilizationTimer_s = 0;
    double stabilizationCheck_s = 2;
    // Let's shoot for with in 0.25% for 10s straight
    double pctDiffSpO2 = 0.25;
    double previousSpO2 = mve.GetMinSpO2();

    double statusTimer_s = 0;  // Current time of this status cycle
    double statusStep_s = 60; // How long did it take to simulate this much time

    // Minimum of 2 breaths befor we check stabilization
    int minIterations = (int)(60 / sim.respirationrate_per_min() * 2 / timeStep_s);
    int maxIterations = (int)(60 / timeStep_s);
    while (true)
    {
      totalIterations++;
      if (totalIterations == minIterations)
        mve.GetLogger()->Info("Minimum Runtime Achieved");

      if (!mve.AdvanceTime(timeStep_s))
        return false;
      // Increment our timers
      currentTime_s += timeStep_s;
      statusTimer_s += timeStep_s;
      // Start stabilization timer after minimum run time
      if (totalIterations > minIterations)
        stabilizationTimer_s += timeStep_s;

      // Generally, we do not enable this, but do take this into account in the data analysis
      //if (mve.GetMinPAO2_mmHg() >= 200)
      //{
      //  mve.GetLogger()->Info("Reached maximum PAO2, unable to stabilize");
      //  break;
      //}

      // How are we running?
      if (statusTimer_s > statusStep_s)
      {
        statusTimer_s = 0;
        mve.GetLogger()->Info("Current Time is " + cdm::to_string(currentTime_s) + "s, it took " + cdm::to_string(profiler.GetElapsedTime_s("Status")) + "s to simulate the past " + cdm::to_string(statusStep_s) + "s");
        profiler.Reset("Status");
      }

      // Check to see if we are stable
      if (stabilizationTimer_s > stabilizationCheck_s)
      {
        bool allPassed = true;
        double currentSpO2 = mve.GetMinSpO2();
        double pctDiff = GeneralMath::PercentDifference(previousSpO2, currentSpO2);
        if (pctDiff > pctDiffSpO2)
          allPassed = false;
        if (allPassed)
          stabilizationPasses++;
        else
        {
          stabilizationPasses = 0;
          previousSpO2 = currentSpO2;
        }
        stabilizationTimer_s = 0;
        if (stabilizationPasses == 5)
        {
          if (currentSpO2 > 0.89)
          {
            stabilized = true;
            double stabilizationTime = (totalIterations - minIterations) * timeStep_s;
            mve.GetLogger()->Info("Stabilized SpO2 at " + cdm::to_string(currentSpO2));
            mve.GetLogger()->Info("With FiO2 of " + cdm::to_string(currentFiO2));
            mve.GetLogger()->Info("It took " + cdm::to_string(stabilizationTime) + "s to stabilize");
            break;
          }
          else
          {
            stabilizationPasses = 0;
            if (currentSpO2 < previousSpO2)// Going down...
            {
              if (currentSpO2 < 0.85)
                currentFiO2 += 0.1;
              else
                currentFiO2 += 0.025;
            }
            else
            {
              if (currentSpO2 < 0.88)
                currentFiO2 += 0.05;
              else
                currentFiO2 += 0.01;
            }
            if (currentFiO2 >= 1.0)
            {
              currentFiO2 = 1.0;
              mve.GetLogger()->Info("Reached maximum FiO2, unable to stabilize");
              break;
            }
            mve.SetFiO2(currentFiO2);
            previousSpO2 = currentSpO2;
            mve.GetLogger()->Info("Stabilized SpO2 at " + cdm::to_string(currentSpO2));
            mve.GetLogger()->Info("Increasing FiO2 to " + cdm::to_string(currentFiO2));
          }
        }
        if (totalIterations > maxIterations)
        {
          currentFiO2 += 0.05;
          totalIterations = 0;
          stabilizationPasses = 0;
          stabilizationTimer_s = 0;
          if (currentFiO2 >= 1.0)
          {
            currentFiO2 = 1.0;
            mve.GetLogger()->Info("Reached maximum FiO2, unable to stabilize");
            break;
          }
          mve.GetLogger()->Info("Reached maximum iterations, upping the FiO2 to " + cdm::to_string(currentFiO2));
          mve.SetFiO2(currentFiO2);
        }
      }
    }

    mve.GetSimulationState(sim);
    // Add our stabilization numbers to the results
    for (int p = 0; p < sim.patientcomparisons_size(); p++)
    {
      Controller* pc = mve.m_Controllers[p];
      double SpO2 = pc->GetBloodChemistry().GetOxygenSaturation().GetValue();
      auto* multiVentilation = (*sim.mutable_patientcomparisons())[p].mutable_multiplexventilation();
      multiVentilation->set_achievedstabilization(SpO2 >= 0.89);
      multiVentilation->set_oxygensaturationstabilizationtrend((SpO2 - previousSpO2) / timeStep_s);
    }
    sim.set_achievedstabilization(stabilized);
    sim.set_stabilizationtime_s((totalIterations - minIterations) * timeStep_s);
    mve.GetLogger()->Info("It took " + cdm::to_string(profiler.GetElapsedTime_s("Total")) + "s to run this simulation");

    profiler.Clear();
    return true;
  }

  bool MVRunner::RunSimulationToStableSpO2(pulse::study::bind::multiplex_ventilation::SimulationData& sim, const std::string& dataDir)
  {
    TimingProfile profiler;
    profiler.Start("Total");
    profiler.Start("Status");

    MVEngine mve(sim.outputbasefilename() + ".log", false, dataDir);
    if (!mve.CreateEngine(sim))
      return false;

    double timeStep_s = mve.m_TimeStep_s;
    double currentTime_s = 0;

    int stabilizationPasses = 0;
    int totalIterations = 0;
    double stabilizationTimer_s = 0;
    double stabilizationCheck_s = 2;
    // Let's shoot for with in 0.25% for 10s straight
    double pctDiffSpO2 = 0.25;
    std::vector<double> previsouSpO2s;
    for (Controller* pc : mve.m_Controllers)
      previsouSpO2s.push_back(pc->GetBloodChemistry().GetOxygenSaturation().GetValue());

    double statusTimer_s = 0;  // Current time of this status cycle
    double statusStep_s = 60; // How long did it take to simulate this much time

    int minIterations = (int)(60 / timeStep_s);
    int maxIterations = (int)(300 / timeStep_s);
    while (true)
    {
      totalIterations++;
      if (totalIterations == minIterations)
        mve.GetLogger()->Info("Minimum Runtime Achieved");

      if (!mve.AdvanceTime(timeStep_s))
        return false;
      // Increment our timers
      currentTime_s += timeStep_s;
      statusTimer_s += timeStep_s;
      // Start stabilization timer after minimum run time
      if (totalIterations > minIterations)
        stabilizationTimer_s += timeStep_s;

      // How are we running?
      if (statusTimer_s > statusStep_s)
      {
        statusTimer_s = 0;
        mve.GetLogger()->Info("Current Time is " + cdm::to_string(currentTime_s) + "s, it took " + cdm::to_string(profiler.GetElapsedTime_s("Status")) + "s to simulate the past " + cdm::to_string(statusStep_s) + "s");
        profiler.Reset("Status");
      }

      // Check to see if we are stable
      if (stabilizationTimer_s > stabilizationCheck_s)
      {
        size_t idx = 0;
        bool allPassed = true;
        for (Controller* pc : mve.m_Controllers)
        {
          double currentSpO2 = pc->GetBloodChemistry().GetOxygenSaturation().GetValue();
          double pctDiff = GeneralMath::PercentDifference(previsouSpO2s[idx++], currentSpO2);
          if (pctDiff > pctDiffSpO2)
            allPassed = false;
        }
        if (allPassed)
          stabilizationPasses++;
        else
        {
          idx = 0;
          stabilizationPasses = 0;
          for (Controller* pc : mve.m_Controllers)
          {
            previsouSpO2s[idx++] = pc->GetBloodChemistry().GetOxygenSaturation().GetValue();
          }
        }
        stabilizationTimer_s = 0;
        if (stabilizationPasses == 5)
        {
          break;
          double stabilizationTime = (totalIterations - minIterations) * timeStep_s;
          mve.GetLogger()->Info("Stabilization took " + cdm::to_string(stabilizationTime) + "s to for this simulation");
        }
        if (totalIterations > maxIterations)
        {
          mve.GetLogger()->Info("Reached maximum iterations. Ending simulation.");
          break;
        }
      }
    }
    mve.GetSimulationState(sim);
    // Add our stabilization numbers to the results
    for (int p = 0; p < sim.patientcomparisons_size(); p++)
    {
      Controller* pc = mve.m_Controllers[p];
      double SpO2 = pc->GetBloodChemistry().GetOxygenSaturation().GetValue();
      auto* multiVentilation = (*sim.mutable_patientcomparisons())[p].mutable_multiplexventilation();
      multiVentilation->set_achievedstabilization(!(totalIterations > maxIterations));
      multiVentilation->set_oxygensaturationstabilizationtrend((SpO2 - previsouSpO2s[p]) / timeStep_s);
    }
    mve.GetLogger()->Info("It took " + cdm::to_string(profiler.GetElapsedTime_s("Total")) + "s to run this simulation");

    profiler.Clear();
    return true;
  }

  pulse::study::bind::multiplex_ventilation::SimulationData* MVRunner::GetNextSimulation()
  {
    m_mutex.lock();
    pulse::study::bind::multiplex_ventilation::SimulationData* sim = nullptr;
    if (!m_SimulationsToRun.empty())
    {
      auto id = *m_SimulationsToRun.begin();
      for (int i = 0; i < m_SimulationList->simulations_size(); i++)
      {
        sim = &(*m_SimulationList->mutable_simulations())[i];
        if (sim->id() == id)
          break;
      }
      Info("Simulating ID " + std::to_string(id) + " " + sim->outputbasefilename());
      m_SimulationsToRun.erase(id);
    }
    m_mutex.unlock();
    return sim;
  }

  void MVRunner::FinalizeSimulation(pulse::study::bind::multiplex_ventilation::SimulationData& sim)
  {
    m_mutex.lock();
    auto rSim = m_SimulationResultsList->mutable_simulations()->Add();
    rSim->CopyFrom(sim);
    SerializeToFile(*m_SimulationResultsList, m_SimulationResultsListFile);
    Info("Completed Simulation " + std::to_string(m_SimulationResultsList->simulations_size()) + " of " + std::to_string(m_SimulationList->simulations_size()));
    if (sim.achievedstabilization())
      Info("  Stabilized : " + sim.outputbasefilename() + "_fio2=" + cdm::to_string(sim.fio2()));
    else
      Info("  FAILED STABILIZATION : " + sim.outputbasefilename() + "_fio2=" + cdm::to_string(sim.fio2()));
    std::ofstream plots;
    plots.open(m_SimulationList->outputrootdir() + "/plot_pairs.config", std::fstream::in | std::fstream::out | std::fstream::app);
    plots << sim.outputbasefilename() << "multiplex_patient_0_results.csv, " << sim.outputbasefilename() << "multiplex_patient_1_results.csv\n";
    plots.close();
    m_mutex.unlock();
  }

  bool MVRunner::SerializeToString(pulse::study::bind::multiplex_ventilation::SimulationListData& src, std::string& output, eSerializationFormat f) const
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
  bool MVRunner::SerializeToFile(pulse::study::bind::multiplex_ventilation::SimulationListData& src, const std::string& filename) const
  {
    return PBUtils::SerializeToFile(src, filename, GetLogger());
  }
  bool MVRunner::SerializeFromString(const std::string& src, pulse::study::bind::multiplex_ventilation::SimulationListData& dst, eSerializationFormat f)
  {
    google::protobuf::util::JsonParseOptions parseOpts;
    auto status = google::protobuf::util::JsonStringToMessage(src, &dst, parseOpts);
    if (!status.ok())
    {
      PBUtils::LogError("MVRunner::SerializeFromString", status.ToString(), GetLogger());
      return false;
    }
    return true;
  }
  bool MVRunner::SerializeFromFile(const std::string& filename, pulse::study::bind::multiplex_ventilation::SimulationListData& dst)
  {
    return PBUtils::SerializeFromFile(filename, dst, GetLogger());
  }
}
