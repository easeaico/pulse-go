/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
package com.kitware.pulse.cdm.engine;

import java.util.HashMap;
import java.util.Map;

import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.util.JsonFormat;

import com.kitware.pulse.cdm.bind.Engine.TimedStabilizationData;
import com.kitware.pulse.cdm.bind.Enums.eSwitch;
import com.kitware.pulse.cdm.properties.SEScalarTime;
import com.kitware.pulse.utilities.FileUtils;

public class SETimedStabilization
{
  protected eSwitch                  trackingStabilization;
  protected Map<String,SEScalarTime> convergenceCriteria;
  
  public SETimedStabilization()
  {
    super();
    this.convergenceCriteria=new HashMap<>();
  }
  
  public void clear()
  {
    clear();
    this.trackingStabilization=eSwitch.Off;
    this.convergenceCriteria.clear();
  }
  
  public void readFile(String fileName) throws InvalidProtocolBufferException
  {
    TimedStabilizationData.Builder builder = TimedStabilizationData.newBuilder();
    JsonFormat.parser().merge(FileUtils.readFile(fileName), builder);
    SETimedStabilization.load(builder.build(), this);
  }
  public void writeFile(String fileName) throws InvalidProtocolBufferException
  {
    FileUtils.writeFile(fileName, JsonFormat.printer().print(SETimedStabilization.unload(this)));
  }
  
  public static void load(TimedStabilizationData src, SETimedStabilization dst) 
  {
    if(src.getTrackingStabilization()!=eSwitch.UNRECOGNIZED && src.getTrackingStabilization()!=eSwitch.NullSwitch)
      dst.trackingStabilization=src.getTrackingStabilization();
    for(String name : src.getConvergenceCriteriaMap().keySet())
    {
      SEScalarTime.load(src.getConvergenceCriteriaMap().get(name),dst.createConvergenceCriteria(name));     
    }
  }
  
  public static TimedStabilizationData unload(SETimedStabilization src)
  {
    TimedStabilizationData.Builder dst = TimedStabilizationData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  
  protected static void unload(SETimedStabilization src, TimedStabilizationData.Builder dst)
  {
  	if(src.trackingStabilization!=null)
  		dst.setTrackingStabilization(src.trackingStabilization);
    for(String name : src.convergenceCriteria.keySet())
    {
    	dst.putConvergenceCriteria(name, SEScalarTime.unload(src.convergenceCriteria.get(name)));
    }
  }
  
  public eSwitch isTrackingStabilization()
  {
    return this.trackingStabilization;
  }
  public void TrackStabilization(eSwitch b)
  {
  	this.trackingStabilization = (b==eSwitch.NullSwitch) ? eSwitch.Off : b;
  }
  
  public SEScalarTime createConvergenceCriteria(String type)
  {
    SEScalarTime stabilizationTime = new SEScalarTime();
    this.convergenceCriteria.put(type, stabilizationTime);
    return stabilizationTime;
  }
  public boolean hasConvergenceCriteria(String type)
  {
    return this.convergenceCriteria.containsKey(type);
  }
  public SEScalarTime getConvergenceCriteria(String type)
  {
    return this.convergenceCriteria.get(type);
  }
}
