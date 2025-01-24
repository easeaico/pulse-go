/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
package com.kitware.pulse.engine;

import java.util.ArrayList;
import java.util.List;

import com.google.common.primitives.Doubles;
import com.google.protobuf.util.JsonFormat;
import com.kitware.pulse.SerializationType;

import com.kitware.pulse.engine.bind.Enums.eModelType;
import com.kitware.pulse.cdm.actions.SEAction;
import com.kitware.pulse.cdm.bind.Engine.ActionListData;
import com.kitware.pulse.cdm.bind.Engine.AnyActionData;
import com.kitware.pulse.cdm.bind.Engine.AnyConditionData;
import com.kitware.pulse.cdm.bind.Engine.ConditionListData;
import com.kitware.pulse.cdm.bind.Engine.LogMessagesData;
import com.kitware.pulse.cdm.bind.Events.ActiveEventData;
import com.kitware.pulse.cdm.bind.Events.ActiveEventListData;
import com.kitware.pulse.cdm.bind.Events.EventChangeData;
import com.kitware.pulse.cdm.bind.Events.EventChangeListData;
import com.kitware.pulse.cdm.bind.Patient.PatientData;
import com.kitware.pulse.cdm.bind.PatientAssessments.ArterialBloodGasTestData;
import com.kitware.pulse.cdm.bind.PatientAssessments.CompleteBloodCountData;
import com.kitware.pulse.cdm.bind.PatientAssessments.ComprehensiveMetabolicPanelData;
import com.kitware.pulse.cdm.bind.PatientAssessments.UrinalysisData;
import com.kitware.pulse.cdm.bind.PatientAssessments.ePatientAssessmentType;
import com.kitware.pulse.cdm.conditions.SECondition;
import com.kitware.pulse.cdm.engine.SEDataRequestManager;
import com.kitware.pulse.cdm.engine.SEActiveEvent;
import com.kitware.pulse.cdm.engine.SEEventHandler;
import com.kitware.pulse.cdm.engine.SEPatientConfiguration;
import com.kitware.pulse.cdm.patient.assessments.SEArterialBloodGasTest;
import com.kitware.pulse.cdm.patient.assessments.SECompleteBloodCount;
import com.kitware.pulse.cdm.patient.assessments.SEComprehensiveMetabolicPanel;
import com.kitware.pulse.cdm.patient.assessments.SEPatientAssessment;
import com.kitware.pulse.cdm.patient.assessments.SEUrinalysis;
import com.kitware.pulse.cdm.properties.SEScalarTime;
import com.kitware.pulse.cdm.properties.CommonUnits.TimeUnit;
import com.kitware.pulse.cdm.patient.SEPatient;
import com.kitware.pulse.utilities.Log;
import com.kitware.pulse.utilities.LogListener;
import com.kitware.pulse.utilities.JNIBridge;

public class PulseEngine
{
  protected boolean        alive  = false;
  protected double         timeStep_s = 0;
  protected double         timeRemainder = 0;
  protected LogListener    logListener = null;
  protected SEEventHandler eventHandler = null;
  
  static protected String  version="";
  static protected String  hash="";
  
  static public String version()
  {
    if(version.isEmpty())
    {
      JNIBridge.initialize();
      version = nativeGetVersion();
    }
    return version;
  }
  
  static public String hash()
  {
    if(hash.isEmpty())
    {
      JNIBridge.initialize();
      hash = nativeGetHash();
    }
    return hash;
  }

  public PulseEngine()
  {
    this("./", eModelType.HumanAdultWholeBody);
  }
  public PulseEngine(eModelType m)
  {
    this("./", m);
  }
  public PulseEngine(String dataDir)
  {
    this(dataDir, eModelType.HumanAdultWholeBody);
  }
  public PulseEngine(String dataDir, eModelType m)
  {
    JNIBridge.initialize();
    if(m == eModelType.UNRECOGNIZED)
      m = eModelType.HumanAdultWholeBody;
    nativeObj=nativeAllocate(dataDir, m.ordinal());
    timeStep_s = nativeGetTimeStep(nativeObj,"s");
  }
  
  // Release any files
  public void clear()
  {
    nativeClear(nativeObj);
  }

  public void finalize()
  {
    this.cleanUp();
  }

  public void cleanUp()
  {
    if(nativeObj!=0)
    {
      nativeDelete(nativeObj);
      nativeObj=0;
    }
    this.alive = false;
    this.logListener = null;
    this.eventHandler = null;
  }
  
  ////////////////////
  // STATE FILE I/O //
  ////////////////////
  
  public boolean serializeFromFile(String stateFile, SEDataRequestManager dataRequests)
  {
    try
    {
      String drString = JsonFormat.printer().print(SEDataRequestManager.unload(dataRequests));
      alive = nativeSerializeFromFile(nativeObj, stateFile, drString, thunkType.value());
    }
    catch(Exception ex)
    {
      if(logListener!=null)
      {
        logListener.error("Unable to serialize state file");
        logListener.error(ex.getMessage());
      }
      alive = false;
    }
    return alive;
  }
  public boolean serializeToFile(String stateFile)
  {
    if(!alive)
    {
      Log.error("Engine has not been initialized");
      return alive;
    }
    return nativeSerializeToFile(nativeObj, stateFile);
  }

  ////////////////////////
  // STRING/STREAM I/O //
  ///////////////////////
  
  public boolean serializeFromString(String state, SEDataRequestManager dataRequests)
  {
    try
    {
      String drString = JsonFormat.printer().print(SEDataRequestManager.unload(dataRequests));
      alive = nativeSerializeFromString(nativeObj, state, drString, thunkType.value());
    }
    catch(Exception ex)
    {
      if(logListener!=null)
      {
        logListener.error("Unable to serialize state file");
        logListener.error(ex.getMessage());
      }
      alive = false;
    }
    return alive;
  }
  public String serializeToString(String stateFile, SerializationType format)
  {
    if(!alive)
    {
      Log.error("Engine has not been initialized");
      return "";
    }
    return nativeSerializeToString(nativeObj, stateFile, format.value());
  }

  //////////////////////
  // PATIENT CREATION //
  //////////////////////
  
  public boolean initializeEngine(SEPatientConfiguration patientConfiguration, SEDataRequestManager dataRequests)
  {
    try
    {
      String pcString = JsonFormat.printer().print(SEPatientConfiguration.unload(patientConfiguration));
      String drString = JsonFormat.printer().print(SEDataRequestManager.unload(dataRequests));
      alive = nativeInitializeEngine(nativeObj, pcString, drString, thunkType.value());
    }
    catch(Exception ex)
    {
      if(logListener!=null)
      {
        logListener.error("Unable to initialize engine");
        logListener.error(ex.getMessage());
      }
      alive = false;
    }
    return alive;
  }

  public boolean getInitialPatient(SEPatient p)
  {
    if(!alive)
    {
      Log.error("Engine has not been initialized");
      return alive;
    }
    try
    {
      PatientData.Builder b = PatientData.newBuilder();
      String str = nativeGetInitialPatient(nativeObj, thunkType.value());
      JsonFormat.parser().merge(str, b);
      SEPatient.load(b.build(),p);
    }
    catch(Exception ex)
    {
      if(logListener!=null)
      {
        logListener.error("Unable to get initial patient");
        logListener.error(ex.getMessage());
      }
      return false;
    }
    return true;
  }
  
  public boolean getConditions(List<SECondition> conditions)
  {
    if(!alive)
    {
      Log.error("Engine has not been initialized");
      return alive;
    }
    try
    {
      conditions.clear();
      ConditionListData.Builder b = ConditionListData.newBuilder();
      String str = nativeGetConditions(nativeObj, thunkType.value());
      JsonFormat.parser().merge(str, b);
      for(AnyConditionData any : b.getAnyConditionList())
        conditions.add(SECondition.ANY2CDM(any));
    }
    catch(Exception ex)
    {
      if(logListener!=null)
      {
        logListener.error("Unable to get conditions");
        logListener.error(ex.getMessage());
      }
      return false;
    }
    return true;
  }
  
  public boolean getPatientAssessment(SEPatientAssessment assessment)
  {
    if(!alive)
    {
      Log.error("Engine has not been initialized");
      return alive;
    }
    try
    {
      if(assessment instanceof SEArterialBloodGasTest)
      {
        ArterialBloodGasTestData.Builder b = ArterialBloodGasTestData.newBuilder();
        String str = nativeGetAssessment(nativeObj, ePatientAssessmentType.ArterialBloodGasTest.ordinal(), thunkType.value());
        JsonFormat.parser().merge(str, b);
        SEArterialBloodGasTest.load(b.build(),((SEArterialBloodGasTest)assessment));
        return true;
      }
      
      if(assessment instanceof SECompleteBloodCount)
      {
        CompleteBloodCountData.Builder b = CompleteBloodCountData.newBuilder();
        String str = nativeGetAssessment(nativeObj, ePatientAssessmentType.CompleteBloodCount.ordinal(), thunkType.value());
        JsonFormat.parser().merge(str, b);
        SECompleteBloodCount.load(b.build(),((SECompleteBloodCount)assessment));
        return true;
      }
      
      if(assessment instanceof SEComprehensiveMetabolicPanel)
      {
        ComprehensiveMetabolicPanelData.Builder b = ComprehensiveMetabolicPanelData.newBuilder();
        String str = nativeGetAssessment(nativeObj, ePatientAssessmentType.ComprehensiveMetabolicPanel.ordinal(), thunkType.value());
        JsonFormat.parser().merge(str, b);
        SEComprehensiveMetabolicPanel.load(b.build(),((SEComprehensiveMetabolicPanel)assessment));
        return true;
      }
      
      if(assessment instanceof SEUrinalysis)
      {
        UrinalysisData.Builder b = UrinalysisData.newBuilder();
        String str = nativeGetAssessment(nativeObj, ePatientAssessmentType.Urinalysis.ordinal(), thunkType.value());
        JsonFormat.parser().merge(str, b);
        SEUrinalysis.load(b.build(),((SEUrinalysis)assessment));
        return true;
      }
    }
    catch(Exception ex)
    {
      if(logListener!=null)
      {
        logListener.error("Unable to get patient assessment");
        logListener.error(ex.getMessage());
      }
    }
    return false;
  }
  
  /////////////////////
  // ADVANCE SUPPORT //
  /////////////////////
  
  public synchronized boolean advanceTime()
  {
    if(!alive)
    {
      Log.error("Engine has not been initialized");
      return alive;
    }
    if(!nativeAdvanceTimeStep(nativeObj))
    {
      Log.error("Engine could not advance time");
      alive=false;
    }
    pullLogMessages();
    // Grab any event changes and pass them to handler
    if (eventHandler != null)
    {
      String evts = nativePullEvents(nativeObj, thunkType.value());
      if(evts!=null && !evts.isEmpty())
      {
        try
        {
          SEScalarTime time = new SEScalarTime();
          EventChangeListData.Builder b = EventChangeListData.newBuilder();
          JsonFormat.parser().merge(evts, b);
          for(EventChangeData change : b.getChangeList())
          {
            time.invalidate();
            if(change.hasSimTime())
              SEScalarTime.load(change.getSimTime(), time);
            eventHandler.handleEvent(change.getEvent(), change.getActive(), time);
          }
        }
        catch(Exception ex)
        {
          Log.error("Unable to process events");
          Log.error(ex.getMessage());
        }
      }
    }
    return alive;
  }
  
  public synchronized boolean advanceTime(SEScalarTime time)
  {
    return advanceTime_s(time.getValue(TimeUnit.s));
  }
  
  public synchronized boolean advanceTime_s(double time)
  {
    if(!alive)
    {
      Log.error("Engine has not been initialized");
      return alive;
    }
    timeRemainder += time;
    int cnt = (int)(timeRemainder/timeStep_s);
    timeRemainder = timeRemainder - (timeStep_s*cnt);
    for(int i=0; i<cnt; i++)
    {
      if(!advanceTime())
        return alive;
    }
    return true;
  }
  
  public List<Double> pullData()
  {
    if(!alive)
    {
      Log.error("Engine has not been initialized");
      return null;
    }
    return Doubles.asList(nativePullData(nativeObj));
  }
  
  public void pullLogMessages()
  {
    if (logListener != null)
    {
      String msgs = nativePullLogMessages(nativeObj, thunkType.value());
      if(msgs!=null && !msgs.isEmpty())
      {
        try
        {
          LogMessagesData.Builder b = LogMessagesData.newBuilder();
          JsonFormat.parser().merge(msgs, b);
          for(String msg : b.getDebugMessagesList())
            logListener.debug(msg);
          for(String msg : b.getInfogMessagesList())
            logListener.info(msg);
          for(String msg : b.getErrorMessagesList())
            logListener.error(msg);
          for(String msg : b.getFatalMessagesList())
            logListener.fatal(msg);
        }
        catch(Exception ex)
        {
          Log.error("Unable to process log messages");
          Log.error(ex.getMessage());
        }
      }
    }
  }
  
  ////////////////////
  // ACTION SUPPORT //
  ////////////////////
  
  public synchronized boolean processAction(SEAction action)
  {
    List <SEAction> actions = new ArrayList<>();
    actions.add(action);
    return processActions(actions);
  }
  public synchronized boolean processActions(List<SEAction> actions)
  {
    if(!alive)
    {
      Log.error("Engine has not been initialized");
      return alive;
    }
    if(actions !=null && !actions.isEmpty())
    {
      ActionListData.Builder aData = ActionListData.newBuilder();
      for(SEAction a : actions)
        aData.addAnyAction(SEAction.CDM2ANY(a));
      try
      {
        String actionsStr = JsonFormat.printer().print(aData);
        if(!nativeProcessActions(nativeObj,actionsStr, thunkType.value()))
        {
          Log.error("Engine could not process actions");
        }
      }
      catch(Exception ex)
      {
        Log.error("Unable to convert action to json");
        Log.error(ex.getMessage());
      }
    }
    pullLogMessages();
    return alive;
  }
  
  public boolean getActiveActions(List<SEAction> actions)
  {
    if(!alive)
    {
      Log.error("Engine has not been initialized");
      return alive;
    }
    try
    {
      actions.clear();
      ActionListData.Builder b = ActionListData.newBuilder();
      String str = nativePullActiveActions(nativeObj, thunkType.value());
      JsonFormat.parser().merge(str, b);
      for(AnyActionData any : b.getAnyActionList())
        actions.add(SEAction.ANY2CDM(any));
    }
    catch(Exception ex)
    {
      if(logListener!=null)
      {
        logListener.error("Unable to get active actions");
        logListener.error(ex.getMessage());
      }
      return false;
    }
    return true;
  }
  
  ///////////////////////
  // LISTENER/HANDLERS //
  ///////////////////////
  
  public void setLogListener(LogListener listener)
  {
    logListener = listener;
    nativeKeepLogMessages(nativeObj, logListener!=null);
  }
  public void setLogFilename(String logFilename)
  {
    nativeSetLogFilename(nativeObj, logFilename);
  }
  
  public void setEventHandler(SEEventHandler eh)
  {
    this.eventHandler = eh;
    nativeKeepEventChanges(nativeObj, eventHandler!=null);
  }
  
  public void getActiveEvents(List<SEActiveEvent> activeEvents)
  {
    try
    {
      activeEvents.clear();
      String aEvents = nativePullActiveEvents(nativeObj, thunkType.value());
      if(aEvents == null || aEvents.isEmpty())
        return;
      ActiveEventListData.Builder b = ActiveEventListData.newBuilder();
      JsonFormat.parser().merge(aEvents, b);
      for(ActiveEventData ae : b.getActiveEventList())
      {
        SEActiveEvent active = new SEActiveEvent();
        active.type = ae.getEvent();
        SEScalarTime.load(ae.getDuration(), active.duration);
        activeEvents.add(active);
      }
    }
    catch(Exception ex)
    {
      Log.error("Unable to pull active events");
      Log.error(ex.getMessage());
    }
  }
  
  ////////////
  // NATIVE //
  ////////////
  
  protected long nativeObj;
  protected SerializationType thunkType = SerializationType.JSON;
  protected native long nativeAllocate(String dataDir, int phyiologyModel);
  protected native void nativeDelete(long nativeObj);

  protected native double nativeGetTimeStep(long nativeObj, String unit);
  static protected native String nativeGetVersion();
  static protected native String nativeGetHash();
  static protected native void nativeClear(long nativeObj);
  
  protected native boolean nativeSerializeFromFile(long nativeObj, String stateFile, String dataRequests, int dataRequestsFormat);
  protected native boolean nativeSerializeToFile(long nativeObj, String stateFile);
  
  protected native boolean nativeSerializeFromString(long nativeObj, String state, String dataRequests, int format);
  protected native String  nativeSerializeToString(long nativeObj, String stateFile, int format);
  
  protected native boolean nativeInitializeEngine(long nativeObj, String patientConfiguration, String dataRequests, int format);
  
  protected native void nativeSetLogFilename(long nativeObj, String filename);
  protected native void nativeKeepLogMessages(long nativeObj, boolean b);
  protected native String nativePullLogMessages(long nativeObj, int format);
  
  protected native void nativeKeepEventChanges(long nativeObj, boolean b);
  protected native String nativePullEvents(long nativeObj, int format);
  protected native String nativePullActiveEvents(long nativeObj, int format);
  
  protected native boolean nativeProcessActions(long nativeObj, String actions, int format);
  protected native String  nativePullActiveActions(long nativeObj, int format);
  
  protected native boolean nativeAdvanceTimeStep(long nativeObj);
  protected native double[] nativePullData(long nativeObj);
  
  protected native String nativeGetInitialPatient(long nativeObj, int format);
  protected native String nativeGetConditions(long nativeObj, int format);
  protected native String nativeGetAssessment(long nativeObj, int type, int format);
}
