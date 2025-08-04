/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.patient.actions;

import com.kitware.pulse.cdm.bind.PatientActions.AirwayObstructionData;
import com.kitware.pulse.cdm.bind.PatientActions.AirwayObstructionData.eResistanceType;
import com.kitware.pulse.cdm.properties.SEScalar0To1;

public class SEAirwayObstruction extends SEPatientAction
{
  private static final long serialVersionUID = 447938500845345205L;
  
  protected SEScalar0To1 severity;
  protected eResistanceType resistanceType;
  
  public SEAirwayObstruction()
  {
    severity = null;
    resistanceType = eResistanceType.Constant;
  }
  
  @Override
  public void clear()
  {
    super.clear();
    if (severity != null)
      severity.invalidate();
    resistanceType = eResistanceType.Constant;
  }
  
  public void copy(SEAirwayObstruction other)
  {
    if(this==other)
      return;
    super.copy(other);
    if (other.severity != null)
      getSeverity().set(other.getSeverity());
    else if (severity != null)
      severity.invalidate();
    resistanceType = other.resistanceType;
  }
  
  @Override
  public boolean isValid()
  {
    return hasSeverity();
  }
  
  public static void load(AirwayObstructionData src, SEAirwayObstruction dst) 
  {
    SEPatientAction.load(src.getPatientAction(), dst);
    if(src.hasSeverity())
      SEScalar0To1.load(src.getSeverity(),dst.getSeverity());
    dst.setResistanceType(src.getResistanceType());
  }
  
  public static AirwayObstructionData unload(SEAirwayObstruction src)
  {
    AirwayObstructionData.Builder dst = AirwayObstructionData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  
  protected static void unload(SEAirwayObstruction src, AirwayObstructionData.Builder dst)
  {
    SEPatientAction.unload(src,dst.getPatientActionBuilder());
    if (src.hasSeverity())
      dst.setSeverity(SEScalar0To1.unload(src.severity));
    dst.setResistanceType(src.getResistanceType());
  }
  
  public boolean hasSeverity()
  {
    return severity == null ? false : severity.isValid();
  }
  public SEScalar0To1 getSeverity()
  {
    if (severity == null)
      severity = new SEScalar0To1();
    return severity;
  }
  
  public eResistanceType getResistanceType()
  {
    return resistanceType;
  }
  public void setResistanceType(eResistanceType rt)
  {
    this.resistanceType = rt;
  }

  @Override
  public String toString()
  {
    if (severity != null)
      return "Airway Obstruction" 
          + "\n\tSeverity: " + getSeverity()
          + "\n\tResistance Type: " + getResistanceType();
    else
      return "Action not specified properly";
  }
}
