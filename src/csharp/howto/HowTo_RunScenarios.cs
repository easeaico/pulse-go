/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using Pulse;
using Pulse.CDM;
using System;
using System.Collections.Generic;

namespace HowTo_RunScenarios
{
  class Example
  {
    public static void Run(string dir = "")
    {
      SEAdvanceTime adv = new SEAdvanceTime();
      adv.GetTime().SetValue(2, TimeUnit.min);

      List<SEDataRequest> data_requests = new List<SEDataRequest>
      {
        // Vitals Monitor Data
        SEDataRequest.CreatePhysiologyDataRequest("HeartRate", FrequencyUnit.Per_min),
        SEDataRequest.CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit.Per_min),
        SEDataRequest.CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit.mL)
      };

      // Let's make a scenario (you could just point the executor to a scenario json file on disk as well)
      SEScenario sce1 = new SEScenario();
      sce1.SetName("HowToRunScenarios1");
      sce1.SetDescription("Simple Scenario to demonstrate building a scenario by the CDM API");
      sce1.SetEngineState("./states/StandardMale@0s.json");
      sce1.GetDataRequestManager().SetDataRequests(data_requests);
      sce1.GetDataRequestManager().SetResultsFilename("./test_results/howto/HowTo-RunScenarios.cs/Scenario1.csv");
      sce1.GetActions().Add(adv);
      // Save it to disk
      string sce1Filename = "./test_results/howto/HowTo-RunScenarios.cs/Scenario1.json";
      if (!sce1.SerializeToFile(sce1Filename))
      {
        Console.WriteLine("Failed to serialize scenario 1");
        return;
      }

      // Create another scenario
      SEScenario sce2 = new SEScenario();
      sce2.SetName("HowToRunScenarios2");
      sce2.SetDescription("Simple Scenario to demonstrate using data requests in a file");
      sce2.SetEngineState("./states/StandardFemale@0s.json");
      sce2.GetDataRequestManager().SetDataRequests(data_requests);
      sce2.GetDataRequestManager().SetResultsFilename("./test_results/howto/HowTo-RunScenarios.cs/Scenario2.csv");
      sce2.GetActions().Add(adv);

      string sce2Filename = "./test_results/howto/HowTo-RunScenarios.cs/Scenario2.json";
      if (!sce2.SerializeToFile(sce2Filename))
      {
        Console.WriteLine("Failed to serialize scenario 2");
        return;
      }

      // Run a single scenario
      PulseScenarioExec execOpts = new PulseScenarioExec();
      execOpts.SetScenarioFilename(sce2Filename);
      if (!execOpts.Execute())
        System.Console.Out.WriteLine("Error running scenario");
      System.Console.Out.WriteLine("Finished!");

      // Write out our exec list of scenarios to execute
      string sceExecStatusFilename = "./test_results/howto/HowTo-RunScenarios.cs/ScenarioExecStatus.json";
      List<SEScenarioExecStatus> sces = new List<SEScenarioExecStatus>();
      sces.Add(new SEScenarioExecStatus());
      sces.Add(new SEScenarioExecStatus());
      sces[0].SetScenarioFilename(sce1Filename);
      sces[1].SetScenarioFilename(sce2Filename);
      SEScenarioExecStatus.SerializeToFile(sces, sceExecStatusFilename);

      // Run multiple scenarios in parallel
      // Set up our executor
      execOpts.Clear(); // Need to clear out the previous run
      // Turn off console logging since we will be running many scenarios at the same time
      execOpts.SetLogToConsole(eSwitch.Off);
      // Tell the execOpts that we want to run that set of scenarios
      execOpts.SetScenarioExecListFilename(sceExecStatusFilename);
      // You can set the number of threads to process your set of scenarios.
      // -1 will figure out how many cores your machine has and create 1 less that that number of threads.
      // If you have less that that scenarios, we just create a thread per scenario.
      // In this case, 2 threads will be created to run 2 scenarios
      execOpts.SetThreadCount(-1);
      // Run them
      if (!execOpts.Execute())
        System.Console.Out.WriteLine("Error running scenarios");
      System.Console.Out.WriteLine("Finished!");

      // Now read the status back in and make sure everything ran ok
      List<SEScenarioExecStatus> sts = new List<SEScenarioExecStatus>();
      SEScenarioExecStatus.SerializeFromFile(sceExecStatusFilename, sts);
      foreach (SEScenarioExecStatus s in sts)
      {
        if (s.GetEngineInitializationState() != eEngineInitializationState.Initialized)
        {
          System.Console.Out.WriteLine("Unable to initialize scenario: " + s.GetScenarioFilename());
          continue;
        }
        if (s.GetScenarioExecutionState() != eScenarioExecutionState.Complete)
        {
          System.Console.Out.WriteLine("Error running scenario: " + s.GetScenarioFilename());
          continue;
        }
        System.Console.Out.WriteLine("Successfully ran scenario " + s.GetScenarioFilename());
        System.Console.Out.WriteLine("\t CSV: " + s.GetCSVFilename());
        System.Console.Out.WriteLine("\t Log: " + s.GetLogFilename());
      }
    }
  }
}
