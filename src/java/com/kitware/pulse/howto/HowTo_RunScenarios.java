/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
package com.kitware.pulse.howto;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

import com.google.protobuf.InvalidProtocolBufferException;
import com.kitware.pulse.cdm.actions.SEAdvanceTime;
import com.kitware.pulse.cdm.bind.Enums.eEngineInitializationState;
import com.kitware.pulse.cdm.bind.Enums.eSwitch;
import com.kitware.pulse.cdm.bind.Scenario.eScenarioExecutionState;
import com.kitware.pulse.cdm.engine.SEDataRequestManager;
import com.kitware.pulse.cdm.properties.CommonUnits.FrequencyUnit;
import com.kitware.pulse.cdm.properties.CommonUnits.TimeUnit;
import com.kitware.pulse.cdm.properties.CommonUnits.VolumeUnit;
import com.kitware.pulse.cdm.scenario.SEScenario;
import com.kitware.pulse.cdm.scenario.SEScenarioExecStatus;
import com.kitware.pulse.engine.PulseScenarioExec;
import com.kitware.pulse.utilities.JNIBridge;

public class HowTo_RunScenarios
{ 
  public static void main(String[] args)
  {
    // Load all Pulse native libraries
    JNIBridge.initialize();
    
    String out_dir = "./test_results/howto/HowTo-RunScenarios.java";
    try {
      Files.createDirectories(Paths.get(out_dir));
    } catch (IOException e) {
      e.printStackTrace();
    }
    
    
    SEAdvanceTime adv = new SEAdvanceTime();
    adv.getTime().setValue(2,TimeUnit.min);
    
    // Create and run a scenario
    SEScenario sce1 = new SEScenario();
    sce1.setName("HowToRunScenarios1");
    sce1.setDescription("Simple Scenario to demonstrate building a scenario by the CDM API");
    sce1.setEngineState("./states/StandardMale@0s.json");
    SEDataRequestManager drm1 = sce1.getDataRequestManager();
    drm1.setResultsFilename(out_dir+"/Scenario1.csv");
    drm1.createPhysiologyDataRequest("HeartRate", FrequencyUnit.Per_min);
    drm1.createPhysiologyDataRequest("RespirationRate", FrequencyUnit.Per_min);
    drm1.createPhysiologyDataRequest("TotalLungVolume", VolumeUnit.mL);
    sce1.getActions().add(adv);
    String sce1Filename = out_dir + "/Scenario1.json";
    sce1.writeFile(sce1Filename);
    
    SEScenario sce2 = new SEScenario();
    sce2.setName("HowToRunScenarios2");
    sce2.setDescription("Simple Scenario to demonstrate building a scenario by the CDM API");
    sce2.setEngineState("./states/StandardFemale@0s.json");
    SEDataRequestManager drm2 = sce2.getDataRequestManager();
    drm2.createPhysiologyDataRequest("HeartRate", FrequencyUnit.Per_min);
    drm2.createPhysiologyDataRequest("RespirationRate", FrequencyUnit.Per_min);
    drm2.createPhysiologyDataRequest("TotalLungVolume", VolumeUnit.mL);
    drm2.setResultsFilename(out_dir+"/Scenario2.csv");
    sce2.getActions().add(adv);
    String sce2Filename = out_dir + "/Scenario2.json";
    sce2.writeFile(sce2Filename);
    
    PulseScenarioExec execOpts = new PulseScenarioExec();
    // Run a single scenario
    execOpts.setScenarioFilename(sce1Filename);
    System.out.println("Running a single scenario.");
    if(!execOpts.execute())
    {
      System.out.println("Error running scenario.");
      return;
    }
    
    // Write out our exec list of scenarios to execute
    String sceExecStatusFilename = out_dir+"/ScenarioExecStatus.json";
    List<SEScenarioExecStatus> sces = new ArrayList<SEScenarioExecStatus>();
    sces.add(new SEScenarioExecStatus());
    sces.add(new SEScenarioExecStatus());
    sces.get(0).setScenarioFilename(sce1Filename);
    sces.get(1).setScenarioFilename(sce2Filename);
    SEScenarioExecStatus.writeFile(sces, sceExecStatusFilename);
    
     // Run multiple scenarios in parallel
     // Set up our executor
     execOpts.clear(); // Need to clear out the previous run
     // Turn off console logging since we will be running many scenarios at the same time
     execOpts.setLogToConsole(eSwitch.Off);
     // Tell the execOpts that we want to run that set of scenarios
     execOpts.setScenarioExecListFilename(sceExecStatusFilename);
     // You can set the number of threads to process your set of scenarios.
     // -1 will figure out how many cores your machine has and create 1 less that that number of threads.
     // If you have less that that scenarios, we just create a thread per scenario.
     // In this case, 2 threads will be created to run 2 scenarios
     //execOpts.setThreadCount(-1);
     System.out.println("Running scenarios with a thread pool.");
     if(!execOpts.execute())
     {
       System.out.println("Error running scenario.");
       return;
     }
  
    // Now read the status back in and make sure everything ran ok
    List<SEScenarioExecStatus> sts = new ArrayList<SEScenarioExecStatus>();
    try {
      SEScenarioExecStatus.readFile(sceExecStatusFilename, sts);
    } catch (InvalidProtocolBufferException e) {
      e.printStackTrace();
      return;
    }
    for (SEScenarioExecStatus s : sts)
    {
      if (s.getEngineInitializationState() != eEngineInitializationState.Initialized)
      {
        System.out.println("Unable to initialize scenario: " + s.getScenarioFilename());
        continue;
      }
      if (s.getScenarioExecutionState() != eScenarioExecutionState.Complete)
      {
        System.out.println("Error running scenario: " + s.getScenarioFilename());
        continue;
      }
      System.out.println("Successfully ran scenario " + s.getScenarioFilename());
      System.out.println("\t CSV: " + s.getCSVFilename());
      System.out.println("\t Log: " + s.getLogFilename());
    }
  }
}
