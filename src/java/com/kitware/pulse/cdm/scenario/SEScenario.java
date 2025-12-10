/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.scenario;

import java.util.ArrayList;
import java.util.List;

import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.util.JsonFormat;

import com.kitware.pulse.cdm.actions.SEAction;
import com.kitware.pulse.cdm.actions.SEAdvanceTime;
import com.kitware.pulse.cdm.bind.Engine.AnyActionData;
import com.kitware.pulse.cdm.bind.Scenario.ScenarioData;
import com.kitware.pulse.cdm.engine.SEDataRequestManager;
import com.kitware.pulse.cdm.engine.SEPatientConfiguration;
import com.kitware.pulse.cdm.properties.CommonUnits.TimeUnit;
import com.kitware.pulse.utilities.FileUtils;
import com.kitware.pulse.utilities.Log;
import com.kitware.pulse.utilities.RunConfiguration;
import com.kitware.pulse.utilities.JNIBridge;

public class SEScenario 
{

  protected String                        name;
  protected String                        description;
  protected SEPatientConfiguration        params;
  protected String                        engineStateFile;
  protected SEDataRequestManager          drMgr = new SEDataRequestManager();
  protected List<String>                  drFiles = new ArrayList<String>();
  protected List<SEAction>                actions = new ArrayList<SEAction>();

  public SEScenario()
  {
    JNIBridge.initialize();
    clear();
  }

  public void clear() 
  {
    this.name            = "";
    this.description     = "";
    this.params          = null;
    this.engineStateFile = null;
    this.actions.clear();
    this.drMgr.clear();
    this.drFiles.clear();
  }

  public void readFile(String fileName) throws InvalidProtocolBufferException
  {
    try
    {
      ScenarioData.Builder builder = ScenarioData.newBuilder();
      JsonFormat.parser().merge(FileUtils.readFile(fileName), builder);
      SEScenario.load(builder.build(), this);
    }
    catch(InvalidProtocolBufferException ex)
    {
      //System.err.println("Not a CDM scenario, checking if engine scenario..."+ex.getMessage());
      // Try to load it as a Pulse Scenario
      // Currently, we don't support the engine configuration in Java
      com.kitware.pulse.engine.bind.Scenario.ScenarioData.Builder builder = 
          com.kitware.pulse.engine.bind.Scenario.ScenarioData.newBuilder();
      JsonFormat.parser().merge(FileUtils.readFile(fileName), builder);
      SEScenario.load(builder.build().getScenario(), this);
    }
  }
  public void writeFile(String fileName)
  {
    FileUtils.writeFile(fileName, toJSON());
  }
  
  public String toJSON() 
  {
    String json;
    try
    {
      json = JsonFormat.printer().print(SEScenario.unload(this));
    }
    catch (InvalidProtocolBufferException e)
    {
      Log.error("Unable to generate json from SEScenario");
      json = "";
    }
    return json;
  }

  public static void load(ScenarioData src, SEScenario dst)
  {
    dst.clear();

    dst.name = src.getName();
    dst.description = src.getDescription();

    if(src.hasPatientConfiguration())
      SEPatientConfiguration.load(src.getPatientConfiguration(),dst.getPatientConfiguration());
    else 
      dst.engineStateFile = src.getEngineStateFile();

    if(src.hasDataRequestManager())
      SEDataRequestManager.load(src.getDataRequestManager(), dst.getDataRequestManager());
    
    for(String filename : src.getDataRequestFileList())
      dst.drFiles.add(filename);

    for(AnyActionData aData : src.getAnyActionList())
      dst.actions.add(SEAction.ANY2CDM(aData)); 
    dst.deriveActionTimes();
  }

  public static ScenarioData unload(SEScenario src)
  {
    ScenarioData.Builder dst = ScenarioData.newBuilder();
    unload(src,dst);
    return dst.build();
  }

  protected static void unload(SEScenario src, ScenarioData.Builder dst)
  {
    if(src.hasName())
      dst.setName(src.name);

    if(src.hasDescription())
      dst.setDescription(src.description);

    if(src.hasPatientConfiguration())
      dst.setPatientConfiguration(SEPatientConfiguration.unload(src.params));
    else if(src.hasEngineState())
      dst.setEngineStateFile(src.engineStateFile);

    if(!src.drMgr.getRequestedData().isEmpty())
      dst.setDataRequestManager(SEDataRequestManager.unload(src.drMgr));
    
    for(String filename : src.drFiles)
      dst.addDataRequestFile(filename);

    for(SEAction a : src.actions)
      dst.addAnyAction(SEAction.CDM2ANY(a));
  }

  public boolean isValid()
  {
    if (actions.size() == 0)
      return false;
    if(!hasPatientConfiguration() && !hasEngineState())
      return false;
    return true;
  }

  public String getName()
  {
    return name;
  }
  public void setName(String name)
  {
    this.name = name;
  }
  public boolean hasName()
  {
    return name.isEmpty() ? false : true;
  }
  public void invalidateName()
  {
    name = "";
  }

  public String getDescription()
  {
    return description;
  }
  public void setDescription(String description)
  {
    this.description = description;
  }
  public boolean hasDescription()
  {
    return description.isEmpty() ? false : true;
  }
  public void invalidateDescription()
  {
    description = "";
  }

  public boolean hasEngineState()
  {
    if(hasPatientConfiguration())
      return false;
    return this.engineStateFile != null && !this.engineStateFile.isEmpty();
  }
  public void setEngineState(String stateFile)
  {
    invalidatePatientConfiguration();
    this.engineStateFile = stateFile;
  }
  public String getEngineState(){ return this.engineStateFile; }
  public void invalidateEngineState()
  {
    this.engineStateFile = null;
  }

  public boolean hasPatientConfiguration()
  {
    return params!=null && params.isValid();
  }
  public SEPatientConfiguration getPatientConfiguration()
  {
    if(this.params==null)
      this.params=new SEPatientConfiguration();
    return this.params;
  }
  public void invalidatePatientConfiguration()
  {
    this.params = null;
  }

  public List<SEAction> getActions() 
  {
    return actions;
  }

  public SEDataRequestManager getDataRequestManager() 
  {
    return this.drMgr;
  }

  public void deriveActionTimes()
  {
    double time_s=0;
    for(SEAction a : this.actions)
    {
      a.getScenarioTime().setValue(time_s, TimeUnit.s);
      if(a instanceof SEAdvanceTime)
        time_s += ((SEAdvanceTime)a).getTime().getValue(TimeUnit.s);
    }
  }
  
  public List<String> getDataRequestFiles()
  {
    return this.drFiles;
  }
  
  public static void main(String[] args)
  {
    // Simple main to read all our scenarios
    RunConfiguration cfg = new RunConfiguration();
    List<String> files = FileUtils.findFiles(cfg.getScenarioDirectory(), ".json", true);
    
    for(String file : files)
    {
      if(file.contains("@"))
        continue;// Ignore the assessments
      if(file.contains("DataRequests"))
        continue;// Ignore the data request files
      
      System.out.println("Reading scenario: "+file);
      SEScenario sce = new SEScenario();
      try
      {
        sce.readFile(file);
      } catch (InvalidProtocolBufferException e)
      {
        // TODO Auto-generated catch block
        System.err.println(" -- UNABLE TO READ SCENARIO " + e.getMessage());
      }
    }
  }
}
