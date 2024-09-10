/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.system.equipment.mechanical_ventilator.actions;

import com.kitware.pulse.cdm.bind.Enums.eDriverWaveform;
import com.kitware.pulse.cdm.bind.MechanicalVentilatorActions.MechanicalVentilatorContinuousPositiveAirwayPressureData;
import com.kitware.pulse.cdm.properties.*;

public class SEMechanicalVentilatorContinuousPositiveAirwayPressure extends SEMechanicalVentilatorMode
{
  protected SEScalarPressure      deltaPressureSupport;
  protected SEScalarVolumePerTime expirationCycleFlow;
  protected SEScalarPressure      expirationCyclePressure;
  protected eDriverWaveform       expirationWaveform;
  protected SEScalar0To1          fractionInspiredOxygen;
  protected SEScalarVolumePerTime inspirationPatientTriggerFlow;
  protected SEScalarPressure      inspirationPatientTriggerPressure;
  protected eDriverWaveform       inspirationWaveform;
  protected SEScalarPressure      positiveEndExpiredPressure;
  protected SEScalarTime          slope;

  public SEMechanicalVentilatorContinuousPositiveAirwayPressure()
  {
    deltaPressureSupport = null;
    expirationCycleFlow = null;
    expirationCyclePressure = null;
    expirationWaveform = eDriverWaveform.NullDriverWaveform;
    fractionInspiredOxygen = null;
    inspirationPatientTriggerFlow = null;
    inspirationPatientTriggerPressure = null;
    inspirationWaveform = eDriverWaveform.NullDriverWaveform;
    positiveEndExpiredPressure = null;
    slope = null;
  }

  public SEMechanicalVentilatorContinuousPositiveAirwayPressure(
      SEMechanicalVentilatorContinuousPositiveAirwayPressure other)
  {
    copy(other);
  }

  public void copy(SEMechanicalVentilatorContinuousPositiveAirwayPressure other)
  {
    super.copy(other);
    if (other.hasDeltaPressureSupport())
      getDeltaPressureSupport().set(other.getDeltaPressureSupport());
    if (other.hasExpirationCycleFlow())
      getExpirationCycleFlow().set(other.getExpirationCycleFlow());
    if (other.hasExpirationCyclePressure())
      getExpirationCyclePressure().set(other.getExpirationCyclePressure());
    setExpirationWaveform(other.getExpirationWaveform());
    if (other.hasFractionInspiredOxygen())
      getFractionInspiredOxygen().set(other.getFractionInspiredOxygen());
    if (other.hasInspirationPatientTriggerFlow())
      getInspirationPatientTriggerFlow().set(other.getInspirationPatientTriggerFlow());
    if (other.hasInspirationPatientTriggerPressure())
      getInspirationPatientTriggerPressure().set(other.getInspirationPatientTriggerPressure());
    setInspirationWaveform(other.getInspirationWaveform());
    if (other.hasPositiveEndExpiratoryPressure())
      getPositiveEndExpiratoryPressure().set(other.getPositiveEndExpiratoryPressure());
    if (other.hasSlope())
      getSlope().set(other.getSlope());
  }

  public void clear()
  {
    super.clear();
    if (deltaPressureSupport != null)
      deltaPressureSupport.invalidate();
    if (expirationCycleFlow != null)
      expirationCycleFlow.invalidate();
    if (expirationCyclePressure != null)
      expirationCyclePressure.invalidate();
    expirationWaveform = eDriverWaveform.NullDriverWaveform;
    if (fractionInspiredOxygen != null)
      fractionInspiredOxygen.invalidate();
    if (inspirationPatientTriggerFlow != null)
      inspirationPatientTriggerFlow.invalidate();
    if (inspirationPatientTriggerPressure != null)
      inspirationPatientTriggerPressure.invalidate();
    inspirationWaveform = eDriverWaveform.NullDriverWaveform;
    if (positiveEndExpiredPressure != null)
      positiveEndExpiredPressure.invalidate();
    if (slope != null)
      slope.invalidate();
  }
  
  public static void load(MechanicalVentilatorContinuousPositiveAirwayPressureData src, SEMechanicalVentilatorContinuousPositiveAirwayPressure dst)
  {
    dst.clear();
    SEMechanicalVentilatorMode.load(src.getMechanicalVentilatorMode(),dst);
    if (src.hasDeltaPressureSupport())
      SEScalarPressure.load(src.getDeltaPressureSupport(), dst.getDeltaPressureSupport());
    if (src.hasExpirationCycleFlow())
      SEScalarVolumePerTime.load(src.getExpirationCycleFlow(), dst.getExpirationCycleFlow());
    if (src.hasExpirationCyclePressure())
      SEScalarPressure.load(src.getExpirationCyclePressure(), dst.getExpirationCyclePressure());
    dst.setExpirationWaveform(src.getExpirationWaveform());
    if (src.hasFractionInspiredOxygen())
      SEScalar0To1.load(src.getFractionInspiredOxygen(), dst.getFractionInspiredOxygen());
    if (src.hasInspirationPatientTriggerFlow())
      SEScalarVolumePerTime.load(src.getInspirationPatientTriggerFlow(), dst.getInspirationPatientTriggerFlow());
    if (src.hasInspirationPatientTriggerPressure())
      SEScalarPressure.load(src.getInspirationPatientTriggerPressure(), dst.getInspirationPatientTriggerPressure());
    dst.setInspirationWaveform(src.getInspirationWaveform());
    if (src.hasPositiveEndExpiratoryPressure())
      SEScalarPressure.load(src.getPositiveEndExpiratoryPressure(), dst.getPositiveEndExpiratoryPressure());
    if (src.hasSlope())
      SEScalarTime.load(src.getSlope(), dst.getSlope());
  }
  public static MechanicalVentilatorContinuousPositiveAirwayPressureData unload(SEMechanicalVentilatorContinuousPositiveAirwayPressure src)
  {
    MechanicalVentilatorContinuousPositiveAirwayPressureData.Builder dst = MechanicalVentilatorContinuousPositiveAirwayPressureData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(SEMechanicalVentilatorContinuousPositiveAirwayPressure src, MechanicalVentilatorContinuousPositiveAirwayPressureData.Builder dst)
  {
    SEMechanicalVentilatorMode.unload(src, dst.getMechanicalVentilatorModeBuilder());
    if (src.hasDeltaPressureSupport())
      dst.setDeltaPressureSupport(SEScalarPressure.unload(src.getDeltaPressureSupport()));
    if (src.hasExpirationCycleFlow())
      dst.setExpirationCycleFlow(SEScalarVolumePerTime.unload(src.getExpirationCycleFlow()));
    if (src.hasExpirationCyclePressure())
      dst.setExpirationCyclePressure(SEScalarPressure.unload(src.getExpirationCyclePressure()));
    dst.setExpirationWaveform(src.getExpirationWaveform());
    if (src.hasFractionInspiredOxygen())
      dst.setFractionInspiredOxygen(SEScalar0To1.unload(src.getFractionInspiredOxygen()));
    if (src.hasInspirationPatientTriggerFlow())
      dst.setInspirationPatientTriggerFlow(SEScalarVolumePerTime.unload(src.getInspirationPatientTriggerFlow()));
    if (src.hasInspirationPatientTriggerPressure())
      dst.setInspirationPatientTriggerPressure(SEScalarPressure.unload(src.getInspirationPatientTriggerPressure()));
    dst.setInspirationWaveform(src.getInspirationWaveform());
    if (src.hasPositiveEndExpiratoryPressure())
      dst.setPositiveEndExpiratoryPressure(SEScalarPressure.unload(src.getPositiveEndExpiratoryPressure()));
    if (src.hasSlope())
      dst.setSlope(SEScalarTime.unload(src.getSlope()));
  }

  public boolean isValid()
  {
    return super.isValid() &&
      hasDeltaPressureSupport() &&
      hasFractionInspiredOxygen() &&
      hasPositiveEndExpiratoryPressure();
  }

  public boolean hasDeltaPressureSupport()
  {
    return deltaPressureSupport == null ? false : deltaPressureSupport.isValid();
  }
  public SEScalarPressure getDeltaPressureSupport()
  {
    if (deltaPressureSupport == null)
      deltaPressureSupport = new SEScalarPressure();
    return deltaPressureSupport;
  }

  public boolean hasExpirationCycleFlow()
  {
    return expirationCycleFlow == null ? false : expirationCycleFlow.isValid();
  }
  public SEScalarVolumePerTime getExpirationCycleFlow()
  {
    if (expirationCycleFlow == null)
      expirationCycleFlow = new SEScalarVolumePerTime();
    return expirationCycleFlow;
  }

  public boolean hasExpirationCyclePressure()
  {
    return expirationCyclePressure == null ? false : expirationCyclePressure.isValid();
  }
  public SEScalarPressure getExpirationCyclePressure()
  {
    if (expirationCyclePressure == null)
      expirationCyclePressure = new SEScalarPressure();
    return expirationCyclePressure;
  }

  public eDriverWaveform getExpirationWaveform()
  {
    return expirationWaveform;
  }
  public void setExpirationWaveform(eDriverWaveform w)
  {
    expirationWaveform = w;
  }

  public boolean hasFractionInspiredOxygen()
  {
    return fractionInspiredOxygen == null ? false : fractionInspiredOxygen.isValid();
  }
  public SEScalar0To1 getFractionInspiredOxygen()
  {
    if (fractionInspiredOxygen == null)
      fractionInspiredOxygen = new SEScalar0To1();
    return fractionInspiredOxygen;
  }

  public boolean hasInspirationPatientTriggerFlow()
  {
    return inspirationPatientTriggerFlow == null ? false : inspirationPatientTriggerFlow.isValid();
  }
  public SEScalarVolumePerTime getInspirationPatientTriggerFlow()
  {
    if (inspirationPatientTriggerFlow == null)
    inspirationPatientTriggerFlow = new SEScalarVolumePerTime();
    return inspirationPatientTriggerFlow;
  }

  public boolean hasInspirationPatientTriggerPressure()
  {
    return inspirationPatientTriggerPressure == null ? false : inspirationPatientTriggerPressure.isValid();
  }
  public SEScalarPressure getInspirationPatientTriggerPressure()
  {
    if (inspirationPatientTriggerPressure == null)
      inspirationPatientTriggerPressure = new SEScalarPressure();
    return inspirationPatientTriggerPressure;
  }

  public eDriverWaveform getInspirationWaveform()
  {
    return inspirationWaveform;
  }
  public void setInspirationWaveform(eDriverWaveform w)
  {
    inspirationWaveform = w;
  }

  public boolean hasPositiveEndExpiratoryPressure()
  {
    return positiveEndExpiredPressure == null ? false : positiveEndExpiredPressure.isValid();
  }
  public SEScalarPressure getPositiveEndExpiratoryPressure()
  {
    if (positiveEndExpiredPressure == null)
      positiveEndExpiredPressure = new SEScalarPressure();
    return positiveEndExpiredPressure;
  }

  public boolean hasSlope()
  {
    return slope == null ? false : slope.isValid();
  }
  public SEScalarTime getSlope()
  {
    if (slope == null)
      slope = new SEScalarTime();
    return slope;
  }

  public String toString()
  {
    String str = "Mechanical Ventilator Volume Control";
    str += "\n\tDeltaPressureSupport: "
        + (hasDeltaPressureSupport() ? getDeltaPressureSupport().toString() : "Not Provided");
    str += "\n\tExpirationCycleFlow: "
        + (hasExpirationCycleFlow() ? getExpirationCycleFlow().toString() : "Not Provided");
    str += "\n\tExpirationCyclePressure: "
        + (hasExpirationCyclePressure() ? getExpirationCyclePressure().toString() : "Not Provided");
    str += "\n\tExpirationWaveform: " + getExpirationWaveform();
    str += "\n\tFractionInspiredOxygen: "
        + (hasFractionInspiredOxygen() ? getFractionInspiredOxygen().toString() : "Not Provided");
    str += "\n\tInspirationPatientTriggerFlow: "
        + (hasInspirationPatientTriggerFlow() ? getInspirationPatientTriggerFlow().toString() : "Not Provided");
    str += "\n\tInspirationPatientTriggerPressure: "
        + (hasInspirationPatientTriggerPressure() ? getInspirationPatientTriggerPressure().toString() : "Not Provided");
    str += "\n\tInspirationWaveform: " + getInspirationWaveform();
    str += "\n\tPositiveEndExpiratoryPressure: "
        + (hasPositiveEndExpiratoryPressure() ? getPositiveEndExpiratoryPressure().toString() : "Not Provided");
    str += "\n\tSlope: " + (hasSlope() ? getSlope().toString() : "Not Provided");

    return str;
  }
}
