/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.scenario;

import java.util.ArrayList;
import java.util.List;

import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.util.JsonFormat;
import com.kitware.pulse.cdm.bind.Enums.eSwitch;
import com.kitware.pulse.cdm.bind.Scenario.ScenarioExecData;
import com.kitware.pulse.cdm.bind.Scenario.eSerializationFormat;
import com.kitware.pulse.utilities.Log;


public class SEScenarioExec
{
  protected eSwitch logToConsole;
  protected String  dataRootDirectory;
  protected String  outputRootDirectory;
  protected eSwitch organizeOutputDirectory;

  protected eSwitch autoSerializeAfterActions;
  protected double  autoSerializePeriod_s;
  protected eSwitch timeStampSerializedStates;

  protected String  engineConfigurationContent;
  protected String  engineConfigurationFilename;

  protected String  scenarioContent;
  protected String  scenarioFilename;
  protected String  scenarioDirectory;
  protected String  scenarioExecListFilename;

  protected List<String>  dataRequestFilesSearch = new ArrayList<String>();

  protected eSerializationFormat  contentFormat;
  protected int threadCount;

  public SEScenarioExec()
  {
    clear();
  }

  public void clear()
  {
    logToConsole = eSwitch.Off;
    dataRootDirectory = "./";
    outputRootDirectory = "";
    organizeOutputDirectory = eSwitch.Off;

    autoSerializeAfterActions = eSwitch.Off;
    autoSerializePeriod_s = 0;
    timeStampSerializedStates = eSwitch.On;

    engineConfigurationContent = "";
    engineConfigurationFilename = "";

    scenarioContent = "";
    scenarioFilename = "";
    scenarioDirectory = "";
    scenarioExecListFilename = "";

    dataRequestFilesSearch.clear();

    contentFormat = eSerializationFormat.JSON;
    threadCount = -1;
  }

  public void copy(SEScenarioExec src)
  {
    SEScenarioExec.load(SEScenarioExec.unload(src), this);
  }

  public static void load(ScenarioExecData src, SEScenarioExec dst)
  {
    dst.clear();
    dst.logToConsole = src.getLogToConsole();
    if(src.getDataRootDirectory()!=null)
      dst.dataRootDirectory = src.getDataRootDirectory();
    if(src.getOutputRootDirectory()!=null)
      dst.outputRootDirectory = src.getOutputRootDirectory();
    dst.organizeOutputDirectory = src.getOrganizeOutputDirectory();

    dst.autoSerializeAfterActions = src.getAutoSerializeAfterActions();
    dst.autoSerializePeriod_s = src.getAutoSerializePeriodS();
    dst.timeStampSerializedStates = src.getTimeStampSerializedStates();

    if(src.getEngineConfigurationContent()!=null)
      dst.engineConfigurationContent = src.getEngineConfigurationContent();
    else if(src.getEngineConfigurationFilename()!=null)
      dst.engineConfigurationFilename = src.getEngineConfigurationFilename();

    if(src.getScenarioContent()!=null)
      dst.scenarioContent = src.getScenarioContent();
    else if(src.getScenarioFilename()!=null)
      dst.scenarioFilename = src.getScenarioFilename();
    else if(src.getScenarioDirectory()!=null)
      dst.scenarioDirectory = src.getScenarioDirectory();
    else if(src.getScenarioExecListFilename()!=null)
      dst.scenarioExecListFilename = src.getScenarioExecListFilename();

    for(String s : src.getDataRequestFilesSearchList())
      dst.dataRequestFilesSearch.add(s);

    dst.contentFormat = src.getContentFormat();
    dst.threadCount = src.getThreadCount();
  }
  public static ScenarioExecData unload(SEScenarioExec src)
  {
    ScenarioExecData.Builder dst = ScenarioExecData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(SEScenarioExec src, ScenarioExecData.Builder dst)
  {
    dst.setLogToConsole(src.logToConsole);
    if (!src.dataRootDirectory.isEmpty())
      dst.setDataRootDirectory(src.dataRootDirectory);
    if (!src.outputRootDirectory.isEmpty())
      dst.setOutputRootDirectory(src.outputRootDirectory);
    dst.setOrganizeOutputDirectory(src.organizeOutputDirectory);

    dst.setAutoSerializeAfterActions(src.autoSerializeAfterActions);
    dst.setAutoSerializePeriodS(src.autoSerializePeriod_s);
    dst.setTimeStampSerializedStates(src.timeStampSerializedStates);

    if (!src.engineConfigurationContent.isEmpty())
      dst.setEngineConfigurationContent(src.engineConfigurationContent);
    else if (!src.engineConfigurationFilename.isEmpty())
      dst.setEngineConfigurationFilename(src.engineConfigurationFilename);

    if (!src.scenarioContent.isEmpty())
      dst.setScenarioContent(src.scenarioContent);
    else if (!src.scenarioFilename.isEmpty())
      dst.setScenarioFilename(src.scenarioFilename);
    else if (!src.scenarioDirectory.isEmpty())
      dst.setScenarioDirectory(src.scenarioDirectory);
    else if (!src.scenarioExecListFilename.isEmpty())
      dst.setScenarioExecListFilename(src.scenarioExecListFilename);

    for(String s : src.dataRequestFilesSearch)
      dst.addDataRequestFilesSearch(s);

    dst.setContentFormat(src.contentFormat);
    dst.setThreadCount(src.threadCount);
  }

  public String toJSON()
  {
    String json;
    try
    {
      json = JsonFormat.printer().print(SEScenarioExec.unload(this));
    }
    catch (InvalidProtocolBufferException e)
    {
      Log.error("Unable to generate json from SEScenarioExec");
      json = "";
    }
    return json;
  }

  public eSwitch getLogToConsole() { return logToConsole; }
  public void setLogToConsole(eSwitch s) { logToConsole=s; }

  public String getDataRootDirectory() { return dataRootDirectory; }
  public void setDataRootDirectory(String s) { dataRootDirectory=s; }

  public String getOutputRootDirectory() { return outputRootDirectory; }
  public void setOutputRootDirectory(String s) { outputRootDirectory=s; }

  public eSwitch getOrganizeOutputDirectory() { return organizeOutputDirectory; }
  public void setOrganizeOutputDirectory(eSwitch s) { organizeOutputDirectory=s; }

  public eSwitch getAutoSerializeAfterActions() { return autoSerializeAfterActions; }
  public void setAutoSerializeAfterActions(eSwitch s) { autoSerializeAfterActions=s; }

  public double getAutoSerializePeriod_s() { return autoSerializePeriod_s; }
  public void setAutoSerializePeriod_s(double d) { autoSerializePeriod_s=d; }

  public eSwitch getTimeStampSerializedStates() { return timeStampSerializedStates; }
  public void setTimeStampSerializedStates(eSwitch s) { timeStampSerializedStates=s; }

  public String getEngineConfigurationContent() { return engineConfigurationContent; }
  public void setEngineConfigurationContent(String s) { engineConfigurationContent=s; }

  public String getEngineConfigurationFilename() { return engineConfigurationFilename; }
  public void setEngineConfigurationFilename(String s) { engineConfigurationFilename=s; }

  public String getScenarioContent() { return scenarioContent; }
  public void setScenarioContent(String s) { scenarioContent=s; }

  public String getScenarioFilename() { return scenarioFilename; }
  public void setScenarioFilename(String s) { scenarioFilename=s; }

  public String getScenarioDirectory() { return scenarioDirectory; }
  public void setScenarioDirectory(String s) { scenarioDirectory=s; }
  
  public String getScenarioExecListFilename() { return scenarioExecListFilename; }
  public void setScenarioExecListFilename(String s) { scenarioExecListFilename=s; }

  public List<String> getDataRequestFilesSearch() { return dataRequestFilesSearch; }

  public eSerializationFormat getContentFormat() { return contentFormat; }
  public void setContentFormat(eSerializationFormat s) { contentFormat=s; }

  public int getThreadCount() { return threadCount; }
  public void setThreadCount(int c) { threadCount=c; }

}
