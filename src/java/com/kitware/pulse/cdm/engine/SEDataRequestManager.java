/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
package com.kitware.pulse.cdm.engine;

import java.util.ArrayList;
import java.util.List;

import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.util.JsonFormat;
import com.kitware.pulse.cdm.bind.Engine.DataRequestData;
import com.kitware.pulse.cdm.bind.Engine.DataRequestManagerData;
import com.kitware.pulse.cdm.bind.Engine.DataRequestData.eCategory;
import com.kitware.pulse.cdm.properties.CommonUnits.Unit;
import com.kitware.pulse.utilities.FileUtils;
import com.kitware.pulse.utilities.Log;

public class SEDataRequestManager
{
  protected String                        resultsFilename;
  // TODO Decimal Formatting Data
  protected double                        samplesPerSecond;
  protected List<SEDataRequest>           dataRequests = new ArrayList<>();
  
  public SEDataRequestManager()
  {
    clear();
  }
  
  public void clear() 
  {
    this.resultsFilename = "";
    this.samplesPerSecond = 0;
    dataRequests.clear();
  }
  
  public void readFile(String fileName) throws InvalidProtocolBufferException
  {
    DataRequestManagerData.Builder builder = DataRequestManagerData.newBuilder();
    JsonFormat.parser().merge(FileUtils.readFile(fileName), builder);
    SEDataRequestManager.load(builder.build(), this);
  }
  public void writeFile(String fileName) throws InvalidProtocolBufferException
  {
    FileUtils.writeFile(fileName, JsonFormat.printer().print(SEDataRequestManager.unload(this)));
  }
  
  public static void load(DataRequestManagerData src, SEDataRequestManager dst)
  {
    dst.clear();
    dst.setResultsFilename(src.getResultsFilename());
    if(src.getSamplesPerSecond()>0)
      dst.setSamplesPerSecond(src.getSamplesPerSecond());
    for (DataRequestData drData : src.getDataRequestList())
    {
      SEDataRequest dr = new SEDataRequest(drData.getCategory());
      SEDataRequest.load(drData,dr);
      dst.dataRequests.add(dr);
    }
  }
  
  public static DataRequestManagerData unload(SEDataRequestManager src)
  {
    DataRequestManagerData.Builder dst = DataRequestManagerData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(SEDataRequestManager src, DataRequestManagerData.Builder dst)
  {
    if(src.hasResultsFilename())
      dst.setResultsFilename(src.resultsFilename);
    if(src.samplesPerSecond>0)
      dst.setSamplesPerSecond(src.samplesPerSecond);
    for(SEDataRequest dr : src.dataRequests)
      dst.addDataRequest(SEDataRequest.unload(dr));
  }
  
  public boolean hasResultsFilename(){ return this.resultsFilename!=null&&!this.resultsFilename.isEmpty(); }
  public void setResultsFilename(String f) { this.resultsFilename = f; }
  public String getResultsFilename(){ return this.resultsFilename; }
  
  public void setSamplesPerSecond(double d){ this.samplesPerSecond=d; }
  public double getSamplesPerSecond(){ return this.samplesPerSecond; }
  
  public List<SEDataRequest> getRequestedData(){ return dataRequests; }
  
  public void writeData(List<Double> data)
  {
    Log.info("SimTime(s)="+data.get(0));
    for (int d = 1; d < data.size(); d++)
    {
      Log.info(dataRequests.get(d-1).toString() + "=" + data.get(d));
    }
  }
  
  public SEDataRequest createPatientDataRequest(String property)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.Patient);
    dr.propertyName = property;
    dataRequests.add(dr);
    return dr;
  }
  public SEDataRequest createPatientDataRequest(String property, Unit unit)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.Patient);
    dr.propertyName = property;
    dr.unit = unit;
    dataRequests.add(dr);
    return dr;
  }

  public SEDataRequest createPhysiologyDataRequest(String property)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.Physiology);
    dr.propertyName = property;
    dataRequests.add(dr);
    return dr;
  }
  public SEDataRequest createPhysiologyDataRequest(String property, Unit unit)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.Physiology);
    dr.propertyName = property;
    dr.unit = unit;
    dataRequests.add(dr);
    return dr;
  }

  public SEDataRequest createEnvironmentDataRequest(String property)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.Environment);
    dr.propertyName = property;
    dataRequests.add(dr);
    return dr;
  }
  public SEDataRequest createEnvironmentDataRequest(String property, Unit unit)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.Environment);
    dr.propertyName = property;
    dr.unit = unit;
    dataRequests.add(dr);
    return dr;
  }

  public SEDataRequest createActionDataRequest(String action, String property)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.Action);
    dr.actionName = action;
    dr.propertyName = property;
    dataRequests.add(dr);
    return dr;
  }
  public SEDataRequest createActionDataRequest(String action, String property, Unit unit)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.Action);
    dr.actionName = action;
    dr.propertyName = property;
    dr.unit = unit;
    dataRequests.add(dr);
    return dr;
  }
  public SEDataRequest createActionCompartmentDataRequest(String action, String compartment, String property)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.Action);
    dr.actionName = action;
    dr.compartmentName = compartment;
    dr.propertyName = property;
    dataRequests.add(dr);
    return dr;
  }
  public SEDataRequest createActionCompartmentDataRequest(String action, String compartment, String property, Unit unit)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.Action);
    dr.actionName = action;
    dr.compartmentName = compartment;
    dr.propertyName = property;
    dr.unit = unit;
    dataRequests.add(dr);
    return dr;
  }
  public SEDataRequest createActionSubstanceDataRequest(String action, String substance, String property)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.Action);
    dr.actionName = action;
    dr.substanceName = substance;
    dr.propertyName = property;
    dataRequests.add(dr);
    return dr;
  }
  public SEDataRequest createActionSubstanceDataRequest(String action, String substance, String property, Unit unit)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.Action);
    dr.actionName = action;
    dr.substanceName = substance;
    dr.propertyName = property;
    dr.unit = unit;
    dataRequests.add(dr);
    return dr;
  }

  public SEDataRequest createGasCompartmentDataRequest(String compartment, String property)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.GasCompartment);
    dr.propertyName = property;
    dr.compartmentName = compartment;
    dataRequests.add(dr);
    return dr;
  }
  public SEDataRequest createGasCompartmentDataRequest(String compartment, String property, Unit unit)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.GasCompartment);
    dr.propertyName = property;
    dr.compartmentName = compartment;
    dr.unit = unit;
    dataRequests.add(dr);
    return dr;
  }
  public SEDataRequest createGasCompartmentDataRequest(String compartment, String substance, String property)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.GasCompartment);
    dr.propertyName = property;
    dr.compartmentName = compartment;
    dr.substanceName = substance;
    dataRequests.add(dr);
    return dr;
  }
  public  SEDataRequest createGasCompartmentDataRequest(String compartment, String substance, String property, Unit unit)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.GasCompartment);
    dr.propertyName = property;
    dr.compartmentName = compartment;
    dr.substanceName = substance;
    dr.unit = unit;
    dataRequests.add(dr);
    return dr;
  }

  public SEDataRequest createLiquidCompartmentDataRequest(String compartment, String property)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.LiquidCompartment);
    dr.propertyName = property;
    dr.compartmentName = compartment;
    dataRequests.add(dr);
    return dr;
  }
  public SEDataRequest createLiquidCompartmentDataRequest(String compartment, String property, Unit unit)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.LiquidCompartment);
    dr.propertyName = property;
    dr.compartmentName = compartment;
    dr.unit = unit;
    dataRequests.add(dr);
    return dr;
  }
  public SEDataRequest createLiquidCompartmentDataRequest(String compartment, String substance, String property)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.LiquidCompartment);
    dr.propertyName = property;
    dr.compartmentName = compartment;
    dr.substanceName = substance;
    dataRequests.add(dr);
    return dr;
  }
  public SEDataRequest createLiquidCompartmentDataRequest(String compartment, String substance, String property, Unit unit)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.LiquidCompartment);
    dr.propertyName = property;
    dr.compartmentName = compartment;
    dr.substanceName = substance;
    dr.unit = unit;
    dataRequests.add(dr);
    return dr;
  }

  public SEDataRequest createThermalCompartmentDataRequest(String compartment, String property)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.ThermalCompartment);
    dr.propertyName = property;
    dr.compartmentName = compartment;
    dataRequests.add(dr);
    return dr;
  }
  public SEDataRequest createThermalCompartmentDataRequest(String compartment, String property, Unit unit)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.ThermalCompartment);
    dr.propertyName = property;
    dr.compartmentName = compartment;
    dr.unit = unit;
    dataRequests.add(dr);
    return dr;
  }

  public SEDataRequest createTissueCompartmentDataRequest(String compartment, String property)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.TissueCompartment);
    dr.propertyName = property;
    dr.compartmentName = compartment;
    dataRequests.add(dr);
    return dr;
  }
  public SEDataRequest createTissueCompartmentDataRequest(String compartment, String property, Unit unit)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.TissueCompartment);
    dr.propertyName = property;
    dr.compartmentName = compartment;
    dr.unit = unit;
    dataRequests.add(dr);
    return dr;
  }

  public SEDataRequest createSubstanceDataRequest(String substance, String property)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.Substance);
    dr.propertyName = property;
    dr.substanceName = substance;
    dataRequests.add(dr);
    return dr;
  }
  public SEDataRequest createSubstanceDataRequest(String substance, String property, Unit unit)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.Substance);
    dr.propertyName = property;
    dr.substanceName = substance;
    dr.unit = unit;
    dataRequests.add(dr);
    return dr;
  }

  public SEDataRequest createECGDataRequest(String property)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.ECG);
    dr.propertyName = property;
    dataRequests.add(dr);
    return dr;
  }
  public SEDataRequest createECGDataRequest(String property, Unit unit)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.ECG);
    dr.propertyName = property;
    dr.unit = unit;
    dataRequests.add(dr);
    return dr;
  }

  public SEDataRequest createMechanicalVentilatorDataRequest(String property)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.MechanicalVentilator);
    dr.propertyName = property;
    dataRequests.add(dr);
    return dr;
  }
  public SEDataRequest createMechanicalVentilatorDataRequest(String property, Unit unit)
  {
    SEDataRequest dr = new SEDataRequest(eCategory.MechanicalVentilator);
    dr.propertyName = property;
    dr.unit = unit;
    dataRequests.add(dr);
    return dr;
  }

}
