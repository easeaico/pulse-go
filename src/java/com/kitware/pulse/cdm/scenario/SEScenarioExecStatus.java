/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.scenario;

import java.util.List;

import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.util.JsonFormat;
import com.kitware.pulse.cdm.bind.Engine.EngineInitializationStatusData;
import com.kitware.pulse.cdm.bind.Enums.eEngineInitializationState;
import com.kitware.pulse.cdm.bind.Enums.eSwitch;
import com.kitware.pulse.cdm.bind.Scenario.ScenarioData;
import com.kitware.pulse.cdm.bind.Scenario.ScenarioExecData;
import com.kitware.pulse.cdm.bind.Scenario.ScenarioExecStatusData;
import com.kitware.pulse.cdm.bind.Scenario.ScenarioExecStatusData.Builder;
import com.kitware.pulse.cdm.bind.Scenario.ScenarioExecStatusListData;
import com.kitware.pulse.cdm.bind.Scenario.eScenarioExecutionState;
import com.kitware.pulse.cdm.bind.Scenario.eSerializationFormat;
import com.kitware.pulse.cdm.engine.SEEngineInitializationStatus;
import com.kitware.pulse.utilities.FileUtils;
import com.kitware.pulse.utilities.Log;

public class SEScenarioExecStatus extends SEEngineInitializationStatus
{
  protected String scenarioFilename;
  protected eScenarioExecutionState executionState;
  protected boolean runtimeError;
  protected boolean fatalRuntimeError;
  protected double finalSimulationTime_s;

  public SEScenarioExecStatus()
  {
    clear();
  }

  public void clear()
  {
    super.clear();
    scenarioFilename = "";
    executionState = eScenarioExecutionState.Waiting;
    runtimeError = false;
    fatalRuntimeError = false;
    finalSimulationTime_s = 0;
  }

  public static boolean readFile(String filename, List<SEScenarioExecStatus> dst) throws InvalidProtocolBufferException
  {
    dst.clear();
    ScenarioExecStatusListData.Builder builder = ScenarioExecStatusListData.newBuilder();
    JsonFormat.parser().merge(FileUtils.readFile(filename), builder);
    for (Builder stsBuilder : builder.getScenarioExecStatusBuilderList())
    {
      SEScenarioExecStatus sts = new SEScenarioExecStatus();
      load(stsBuilder.build(), sts);
      dst.add(sts);
    }
    return true;
  }

  public static boolean writeFile(List<SEScenarioExecStatus> src, String filename)
  {
    ScenarioExecStatusListData.Builder dst = ScenarioExecStatusListData.newBuilder();
    for (SEScenarioExecStatus sts : src)
    {
      dst.addScenarioExecStatus(unload(sts));
    }
    String json;
    try
    {
      json = JsonFormat.printer().print(dst.build());
    }
    catch (InvalidProtocolBufferException e)
    {
      Log.error("Unable to generate json from SEScenario");
      return false;
    }
    return FileUtils.writeFile(filename, json);
  }
  
  public static void load(ScenarioExecStatusData src, SEScenarioExecStatus dst)
  {
    SEEngineInitializationStatus.load(src.getInitializationStatus(), dst);
    dst.scenarioFilename = src.getScenarioFilename();
    dst.executionState = src.getScenarioExecutionState();
    dst.runtimeError = src.getRuntimeError();
    dst.fatalRuntimeError = src.getFatalRuntimeError();
    dst.finalSimulationTime_s = src.getFinalSimulationTimeS();
  }

  public static ScenarioExecStatusData unload(SEScenarioExecStatus src)
  {
    ScenarioExecStatusData.Builder dst = ScenarioExecStatusData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(SEScenarioExecStatus src, ScenarioExecStatusData.Builder dst)
  {
    SEEngineInitializationStatus.unload(src, dst.getInitializationStatusBuilder());
    dst.setScenarioFilename(src.scenarioFilename);
    dst.setScenarioExecutionState(src.executionState);
    dst.setRuntimeError(src.runtimeError);
    dst.setFatalRuntimeError(src.fatalRuntimeError);
    dst.setFinalSimulationTimeS(src.finalSimulationTime_s);
  }

  public boolean hasScenarioFilename() { return !scenarioFilename.isEmpty(); }
  public String getScenarioFilename() { return scenarioFilename; }
  public void setScenarioFilename(String fn) { scenarioFilename = fn; }

  public eScenarioExecutionState getScenarioExecutionState() { return executionState; }
  public void setScenarioExecutionState(eScenarioExecutionState s) { executionState = s; }

  public boolean getRuntimeError() { return runtimeError; }
  public void setRuntimeError(boolean e) { runtimeError = e; }

  public boolean getFatalRuntimeError() { return fatalRuntimeError; }
  public void setFatalRuntimeError(boolean e) { fatalRuntimeError = e; }

  public double getFinalSimulationTime_s() { return finalSimulationTime_s; }
  public void setFinalSimulationTime_s(double t) { finalSimulationTime_s = t; }
}
