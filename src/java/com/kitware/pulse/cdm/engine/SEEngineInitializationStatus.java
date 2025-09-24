/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
package com.kitware.pulse.cdm.engine;

import com.kitware.pulse.cdm.bind.Engine.EngineInitializationStatusData;
import com.kitware.pulse.cdm.bind.Enums.eEngineInitializationState;

public class SEEngineInitializationStatus
{
  protected eEngineInitializationState initializationState;
  protected String csvFilename;
  protected String logFilename;
  protected double stabilizationTime_s;

  public SEEngineInitializationStatus()
  {
    clear();
  }

  public void clear()
  {
    initializationState = eEngineInitializationState.Uninitialized;
    csvFilename = "";
    logFilename = "";
    stabilizationTime_s = 0;
  }
  
  public static void load(EngineInitializationStatusData src, SEEngineInitializationStatus dst)
  {
    dst.clear();
    dst.initializationState = src.getInitializationState();
    dst.csvFilename = src.getCSVFilename();
    dst.logFilename = src.getLogFilename();
    dst.stabilizationTime_s = src.getStabilizationTimeS();
  }

  public static EngineInitializationStatusData unload(SEEngineInitializationStatus src)
  {
    EngineInitializationStatusData.Builder dst = EngineInitializationStatusData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(SEEngineInitializationStatus src, EngineInitializationStatusData.Builder dst)
  {
    dst.setInitializationState(src.initializationState);
    dst.setCSVFilename(src.csvFilename);
    dst.setLogFilename(src.logFilename);
    dst.setStabilizationTimeS(src.stabilizationTime_s);
  }

  public eEngineInitializationState getEngineInitializationState()
  {
    return initializationState;
  }
  public void setEngineInitializationState(eEngineInitializationState s)
  {
    initializationState = s;
  }

  public boolean hasCSVFilename() { return !csvFilename.isEmpty(); }
  public  String getCSVFilename() { return csvFilename; }
  public void setCSVFilename(String s) { csvFilename = s; }

  public boolean hasLogFilename() { return !logFilename.isEmpty(); }
  public String getLogFilename() { return logFilename; }
  public  void setLogFilename(String s) { logFilename = s; }

  public double getStabilizationTime_s() { return stabilizationTime_s; }
  public void setStabilizationTime_s(double t) { stabilizationTime_s = t; }
}
