/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.patient.actions;

import com.kitware.pulse.cdm.bind.PatientActions.DyspneaData;
import com.kitware.pulse.cdm.properties.SEScalar0To1;

public class SEDyspnea extends SEPatientAction
{

  private static final long serialVersionUID = 4569172078077405942L;
  protected SEScalar0To1 respirationRateSeverity;
  protected SEScalar0To1 tidalVolumeSeverity;
  
  public SEDyspnea()
  {
    respirationRateSeverity = null;
    tidalVolumeSeverity = null;
  }
  
  @Override
  public void clear()
  {
    super.clear();
    if (respirationRateSeverity != null)
      respirationRateSeverity.invalidate();
    if (tidalVolumeSeverity != null)
      tidalVolumeSeverity.invalidate();
  }
  
  public void copy(SEDyspnea other)
  {
    if(this==other)
      return;
    super.copy(other);
    if (other.respirationRateSeverity != null)
      getRespirationRateSeverity().set(other.getRespirationRateSeverity());
    else if (respirationRateSeverity != null)
      respirationRateSeverity.invalidate();
    if (other.tidalVolumeSeverity != null)
      getTidalVolumeSeverity().set(other.getTidalVolumeSeverity());
    else if (tidalVolumeSeverity != null)
      tidalVolumeSeverity.invalidate();
  }
  
  @Override
  public boolean isValid()
  {
    return hasRespirationRateSeverity() || hasTidalVolumeSeverity();
  }
  
  public static void load(DyspneaData src, SEDyspnea dst) 
  {
    SEPatientAction.load(src.getPatientAction(), dst);
    if(src.hasRespirationRateSeverity())
      SEScalar0To1.load(src.getRespirationRateSeverity(),dst.getRespirationRateSeverity());
    if(src.hasTidalVolumeSeverity())
      SEScalar0To1.load(src.getTidalVolumeSeverity(),dst.getTidalVolumeSeverity());
  }
  
  public static DyspneaData unload(SEDyspnea src)
  {
    DyspneaData.Builder dst = DyspneaData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  
  protected static void unload(SEDyspnea src, DyspneaData.Builder dst)
  {
    SEPatientAction.unload(src,dst.getPatientActionBuilder());
    if (src.hasRespirationRateSeverity())
      dst.setRespirationRateSeverity(SEScalar0To1.unload(src.respirationRateSeverity));
    if (src.hasTidalVolumeSeverity())
      dst.setTidalVolumeSeverity(SEScalar0To1.unload(src.tidalVolumeSeverity));
  }
  
  public boolean hasRespirationRateSeverity()
  {
    return respirationRateSeverity == null ? false : respirationRateSeverity.isValid();
  }
  public SEScalar0To1 getRespirationRateSeverity()
  {
    if (respirationRateSeverity == null)
      respirationRateSeverity = new SEScalar0To1();
    return respirationRateSeverity;
  }

  public boolean hasTidalVolumeSeverity()
  {
    return tidalVolumeSeverity == null ? false : tidalVolumeSeverity.isValid();
  }
  public SEScalar0To1 getTidalVolumeSeverity()
  {
    if (tidalVolumeSeverity == null)
      tidalVolumeSeverity = new SEScalar0To1();
    return tidalVolumeSeverity;
  }

  @Override
  public String toString()
  {
    return "Dyspnea" 
        + "\n\tRespirationRateSeverity: " + getRespirationRateSeverity()
        + "\n\tTidalVolumeSeverity: " + getTidalVolumeSeverity();
  }
}
