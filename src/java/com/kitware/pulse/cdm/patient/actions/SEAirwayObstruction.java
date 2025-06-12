/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.patient.actions;

import com.kitware.pulse.cdm.bind.PatientActions.AirwayObstructionData;
import com.kitware.pulse.cdm.properties.SEScalar0To1;

public class SEAirwayObstruction extends SEPatientAction
{
  private static final long serialVersionUID = 447938500845345205L;
  
  protected SEScalar0To1 severity;
  protected boolean hasSecretions;
  
  public SEAirwayObstruction()
  {
    severity = null;
    hasSecretions = false;
  }
  
  @Override
  public void clear()
  {
    super.clear();
    if (severity != null)
      severity.invalidate();
    hasSecretions = false;
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
    hasSecretions = other.hasSecretions;
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
    dst.setHasSecretions(src.getHasSecretions());
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
    dst.setHasSecretions(src.getHasSecretions());
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
  
  public boolean getHasSecretions()
  {
    return hasSecretions;
  }
  public void setHasSecretions(boolean hasSecretions)
  {
    this.hasSecretions = hasSecretions;
  }

  @Override
  public String toString()
  {
    if (severity != null)
      return "Airway Obstruction" 
          + "\n\tSeverity: " + getSeverity()
          + "\n\tHas Secretions: " + getHasSecretions();
    else
      return "Action not specified properly";
  }
}
