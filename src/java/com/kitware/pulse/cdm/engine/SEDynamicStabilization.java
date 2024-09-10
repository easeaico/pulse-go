/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
package com.kitware.pulse.cdm.engine;

import java.util.HashMap;
import java.util.Map;

import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.util.JsonFormat;

import com.kitware.pulse.cdm.bind.Engine.DynamicStabilizationData;
import com.kitware.pulse.cdm.bind.Enums.eSwitch;
import com.kitware.pulse.utilities.FileUtils;

public class SEDynamicStabilization
{
  protected eSwitch                                             trackingStabilization;
  protected Map<String,SEDynamicStabilizationEngineConvergence> convergenceCriteria;
  
  public SEDynamicStabilization()
  {
    this.convergenceCriteria=new HashMap<>();
  }
  
  public void clear()
  {
    this.trackingStabilization=eSwitch.Off;
    this.convergenceCriteria.clear();
  }
  
  public void readFile(String fileName) throws InvalidProtocolBufferException
  {
    DynamicStabilizationData.Builder builder = DynamicStabilizationData.newBuilder();
    JsonFormat.parser().merge(FileUtils.readFile(fileName), builder);
    SEDynamicStabilization.load(builder.build(), this);
  }
  public void writeFile(String fileName) throws InvalidProtocolBufferException
  {
    FileUtils.writeFile(fileName, JsonFormat.printer().print(SEDynamicStabilization.unload(this)));
  }
  
  public static void load(DynamicStabilizationData src, SEDynamicStabilization dst) 
  {
    if(src.getTrackingStabilization()!=eSwitch.UNRECOGNIZED && src.getTrackingStabilization()!=eSwitch.NullSwitch)
      dst.trackingStabilization=src.getTrackingStabilization();
    for(String name : src.getConvergenceCriteriaMap().keySet())
    {
      SEDynamicStabilizationEngineConvergence.load(src.getConvergenceCriteriaMap().get(name), dst.createConvergenceCriteria(name));
    }
  }
  public static DynamicStabilizationData unload(SEDynamicStabilization src)
  {
    DynamicStabilizationData.Builder dst = DynamicStabilizationData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(SEDynamicStabilization src, DynamicStabilizationData.Builder dst)
  {
  	if(src.trackingStabilization!=null)
      dst.setTrackingStabilization(src.trackingStabilization);
    for(String name : src.convergenceCriteria.keySet())
    {
    	dst.putConvergenceCriteria(name, SEDynamicStabilizationEngineConvergence.unload(src.convergenceCriteria.get(name)));
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
  
  public SEDynamicStabilizationEngineConvergence createConvergenceCriteria(String type)
  {
    SEDynamicStabilizationEngineConvergence c = new SEDynamicStabilizationEngineConvergence();
    this.convergenceCriteria.put(type, c);
    return c;
  }
  public boolean hasConvergenceCriteria(String type)
  {
    return this.convergenceCriteria.containsKey(type);
  }
  public SEDynamicStabilizationEngineConvergence getConvergenceCriteria(String type)
  {
    return this.convergenceCriteria.get(type);
  }
}
