/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.patient.conditions;

import com.kitware.pulse.cdm.bind.PatientConditions.DehydrationData;
import com.kitware.pulse.cdm.properties.SEScalar0To1;

public class SEDehydration extends SEPatientCondition
{

  private static final long serialVersionUID = 8862611857864194588L;
  protected SEScalar0To1 severity;
  
  public SEDehydration()
  {
    severity = null;
  }
  
  @Override
  public void clear()
  {
    super.clear();
    if (severity != null)
      severity.invalidate();
  }
  
  public void copy(SEDehydration other)
  {
    if(this==other)
      return;
    super.copy(other);
    if (other.severity != null)
      getSeverity().set(other.getSeverity());
    else if (severity != null)
      severity.invalidate();
  }
  
  public static void load(DehydrationData src, SEDehydration dst) 
  {
    SEPatientCondition.load(src.getPatientCondition(), dst);
    if(src.hasSeverity())
      SEScalar0To1.load(src.getSeverity(),dst.getSeverity());
  }
  
  public static DehydrationData unload(SEDehydration src) 
  {
    DehydrationData.Builder dst = DehydrationData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  
  protected static void unload(SEDehydration src, DehydrationData.Builder dst)
  {
    SEPatientCondition.unload(src, dst.getPatientConditionBuilder());
    if (src.hasSeverity())
      dst.setSeverity(SEScalar0To1.unload(src.severity));
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
  
  @Override
  public String toString()
  {
    return "Dehydration" 
        + "\n\tSeverity: " + getSeverity();
  }

  
}
