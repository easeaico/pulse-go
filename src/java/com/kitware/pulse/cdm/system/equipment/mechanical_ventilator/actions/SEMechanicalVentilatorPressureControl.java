/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.system.equipment.mechanical_ventilator.actions;

import com.kitware.pulse.cdm.properties.*;
import com.kitware.pulse.cdm.bind.Enums.eDriverWaveform;
import com.kitware.pulse.cdm.bind.MechanicalVentilatorActions.MechanicalVentilatorPressureControlData;
import com.kitware.pulse.cdm.bind.MechanicalVentilatorActions.MechanicalVentilatorPressureControlData.eMode;

public class SEMechanicalVentilatorPressureControl extends SEMechanicalVentilatorMode
  {
    protected eMode                 mode;
    protected SEScalar0To1          fractionInspiredOxygen;
    protected SEScalarVolumePerTime inspirationPatientTriggerFlow;
    protected SEScalarPressure      inspirationPatientTriggerPressure;
    protected eDriverWaveform       inspirationWaveform;
    protected SEScalarTime          inspiratoryPeriod;
    protected SEScalarPressure      inspiratoryPressure;
    protected SEScalarPressure      positiveEndExpiredPressure;
    protected SEScalarFrequency     respirationRate;
    protected SEScalarTime          slope;

    public SEMechanicalVentilatorPressureControl()
    {
      mode = eMode.AssistedControl;
      fractionInspiredOxygen = null;
      inspirationPatientTriggerFlow = null;
      inspirationPatientTriggerPressure = null;
      inspirationWaveform = eDriverWaveform.NullDriverWaveform;
      inspiratoryPeriod = null;
      inspiratoryPressure = null;
      positiveEndExpiredPressure = null;
      respirationRate = null;
      slope = null;
    }

    public SEMechanicalVentilatorPressureControl(SEMechanicalVentilatorPressureControl other)
    {
      copy(other);
    }

    public void copy(SEMechanicalVentilatorPressureControl other)
    {
      super.copy(other);
      setMode(other.getMode());
      if (other.hasFractionInspiredOxygen())
        getFractionInspiredOxygen().set(other.getFractionInspiredOxygen());
      if (other.hasInspirationPatientTriggerFlow())
        getInspirationPatientTriggerFlow().set(other.getInspirationPatientTriggerFlow());
      if (other.hasInspirationPatientTriggerPressure())
        getInspirationPatientTriggerPressure().set(other.getInspirationPatientTriggerPressure());
      setInspirationWaveform(other.getInspirationWaveform());
      if (other.hasInspiratoryPeriod())
        getInspiratoryPeriod().set(other.getInspiratoryPeriod());
      if (other.hasInspiratoryPressure())
        getInspiratoryPressure().set(other.getInspiratoryPressure());
      if (other.hasPositiveEndExpiratoryPressure())
        getPositiveEndExpiratoryPressure().set(other.getPositiveEndExpiratoryPressure());
      if (other.hasRespirationRate())
        getRespirationRate().set(other.getRespirationRate());
      if (other.hasSlope())
        getSlope().set(other.getSlope());
    }

    public void clear()
    {
      super.clear();
      mode = eMode.AssistedControl;
      if (fractionInspiredOxygen != null)
        fractionInspiredOxygen.invalidate();
      if (inspirationPatientTriggerFlow != null)
        inspirationPatientTriggerFlow.invalidate();
      if (inspirationPatientTriggerPressure != null)
        inspirationPatientTriggerPressure.invalidate();
      inspirationWaveform = eDriverWaveform.NullDriverWaveform;
      if (inspiratoryPeriod != null)
        inspiratoryPeriod.invalidate();
      if (inspiratoryPressure != null)
        inspiratoryPressure.invalidate();
      if (positiveEndExpiredPressure != null)
        positiveEndExpiredPressure.invalidate();
      if (respirationRate != null)
        respirationRate.invalidate();
      if (slope != null)
        slope.invalidate();
    }
    
    public static void load(MechanicalVentilatorPressureControlData src, SEMechanicalVentilatorPressureControl dst)
    {
      dst.clear();
      SEMechanicalVentilatorMode.load(src.getMechanicalVentilatorMode(),dst);
      dst.setMode(src.getMode());
      if (src.hasFractionInspiredOxygen())
        SEScalar0To1.load(src.getFractionInspiredOxygen(), dst.getFractionInspiredOxygen());
      if (src.hasInspirationPatientTriggerFlow())
        SEScalarVolumePerTime.load(src.getInspirationPatientTriggerFlow(), dst.getInspirationPatientTriggerFlow());
      if (src.hasInspirationPatientTriggerPressure())
        SEScalarPressure.load(src.getInspirationPatientTriggerPressure(), dst.getInspirationPatientTriggerPressure());
      dst.setInspirationWaveform(src.getInspirationWaveform());
      if (src.hasInspiratoryPeriod())
        SEScalarTime.load(src.getInspiratoryPeriod(), dst.getInspiratoryPeriod());
      if (src.hasInspiratoryPressure())
        SEScalarPressure.load(src.getInspiratoryPressure(), dst.getInspiratoryPressure());
      if (src.hasPositiveEndExpiratoryPressure())
        SEScalarPressure.load(src.getPositiveEndExpiratoryPressure(), dst.getPositiveEndExpiratoryPressure());
      if (src.hasRespirationRate())
        SEScalarFrequency.load(src.getRespirationRate(), dst.getRespirationRate());
      if (src.hasSlope())
        SEScalarTime.load(src.getSlope(), dst.getSlope());
    }
    public static MechanicalVentilatorPressureControlData unload(SEMechanicalVentilatorPressureControl src)
    {
      MechanicalVentilatorPressureControlData.Builder dst = MechanicalVentilatorPressureControlData.newBuilder();
      unload(src,dst);
      return dst.build();
    }
    protected static void unload(SEMechanicalVentilatorPressureControl src, MechanicalVentilatorPressureControlData.Builder dst)
    {
      SEMechanicalVentilatorMode.unload(src, dst.getMechanicalVentilatorModeBuilder());
      dst.setMode(src.getMode());
      if(src.hasFractionInspiredOxygen())
        dst.setFractionInspiredOxygen(SEScalar0To1.unload(src.getFractionInspiredOxygen()));
      if (src.hasInspirationPatientTriggerFlow())
        dst.setInspirationPatientTriggerFlow(SEScalarVolumePerTime.unload(src.getInspirationPatientTriggerFlow()));
      if (src.hasInspirationPatientTriggerPressure())
        dst.setInspirationPatientTriggerPressure(SEScalarPressure.unload(src.getInspirationPatientTriggerPressure()));
      dst.setInspirationWaveform(src.inspirationWaveform);
      if (src.hasInspiratoryPeriod())
        dst.setInspiratoryPeriod(SEScalarTime.unload(src.getInspiratoryPeriod()));
      if (src.hasInspiratoryPressure())
        dst.setInspiratoryPressure(SEScalarPressure.unload(src.getInspiratoryPressure()));
      if (src.hasPositiveEndExpiratoryPressure())
        dst.setPositiveEndExpiratoryPressure(SEScalarPressure.unload(src.getPositiveEndExpiratoryPressure()));
      if (src.hasRespirationRate())
        dst.setRespirationRate(SEScalarFrequency.unload(src.getRespirationRate()));
      if (src.hasSlope())
        dst.setSlope(SEScalarTime.unload(src.getSlope()));
    }

    public boolean isValid()
    {
      return super.isValid() &&
        hasFractionInspiredOxygen() &&
        hasInspiratoryPressure() &&
        hasPositiveEndExpiratoryPressure() &&
        hasRespirationRate();
    }

    public eMode getMode()
    {
      return mode;
    }
    public void setMode(eMode m)
    {
      mode = m;
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

    public boolean hasInspiratoryPeriod()
    {
      return inspiratoryPeriod == null ? false : inspiratoryPeriod.isValid();
    }
    public SEScalarTime getInspiratoryPeriod()
    {
      if (inspiratoryPeriod == null)
        inspiratoryPeriod = new SEScalarTime();
      return inspiratoryPeriod;
    }

    public boolean hasInspiratoryPressure()
    {
      return inspiratoryPressure == null ? false : inspiratoryPressure.isValid();
    }
    public SEScalarPressure getInspiratoryPressure()
    {
      if (inspiratoryPressure == null)
        inspiratoryPressure = new SEScalarPressure();
      return inspiratoryPressure;
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
    
    public boolean hasRespirationRate()
    {
      return respirationRate == null ? false : respirationRate.isValid();
    }
    public SEScalarFrequency getRespirationRate()
    {
      if (respirationRate == null)
        respirationRate = new SEScalarFrequency();
      return respirationRate;
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
      String str = "Mechanical Ventilator Pressure Control";
      str += "\n\tMode: " + getMode();
      str += "\n\tFractionInspiredOxygen: " + (hasFractionInspiredOxygen() ? getFractionInspiredOxygen().toString() : "Not Provided");
      str += "\n\tInspirationPatientTriggerFlow: " + (hasInspirationPatientTriggerFlow() ? getInspirationPatientTriggerFlow().toString() : "Not Provided");
      str += "\n\tInspirationPatientTriggerPressure: " + (hasInspirationPatientTriggerPressure() ? getInspirationPatientTriggerPressure().toString() : "Not Provided");
      str += "\n\tInspirationWaveform: " + getInspirationWaveform();
      str += "\n\tInspiratoryPeriod: " + (hasInspiratoryPeriod() ? getInspiratoryPeriod().toString() : "Not Provided");
      str += "\n\tInspiratoryPressure: " + (hasInspiratoryPressure() ? getInspiratoryPressure().toString() : "Not Provided");
      str += "\n\tPositiveEndExpiratoryPressure: " + (hasPositiveEndExpiratoryPressure() ? getPositiveEndExpiratoryPressure().toString() : "Not Provided");
      str += "\n\tRespirationRate: " + (hasRespirationRate() ? getRespirationRate().toString() : "Not Provided");
      str += "\n\tSlope: " + (hasSlope() ? getSlope().toString() : "Not Provided");


      return str;
    }
  }
