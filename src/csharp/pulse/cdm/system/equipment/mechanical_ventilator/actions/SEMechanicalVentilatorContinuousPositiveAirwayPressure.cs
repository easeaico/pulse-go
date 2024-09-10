/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

namespace Pulse.CDM
{
  public class SEMechanicalVentilatorContinuousPositiveAirwayPressure : SEMechanicalVentilatorMode
  {
    protected SEScalarPressure deltaPressureSupport;
    SEScalarVolumePerTime      expirationCycleFlow;
    SEScalarPressure           expirationCyclePressure;
    eDriverWaveform            expirationWaveform;
    protected SEScalar0To1     fractionInspiredOxygen;
    SEScalarVolumePerTime      inspirationPatientTriggerFlow;
    SEScalarPressure           inspirationPatientTriggerPressure;
    eDriverWaveform            inspirationWaveform;
    protected SEScalarPressure positiveEndExpiredPressure;
    protected SEScalarTime     slope;

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

    public SEMechanicalVentilatorContinuousPositiveAirwayPressure(SEMechanicalVentilatorContinuousPositiveAirwayPressure other)
    {
      Copy(other);
    }

    public void copy(SEMechanicalVentilatorContinuousPositiveAirwayPressure other)
    {
      base.Copy(other);
      if (other.HasDeltaPressureSupport())
        GetDeltaPressureSupport().Set(other.GetDeltaPressureSupport());
      if (other.HasExpirationCycleFlow())
        GetExpirationCycleFlow().Set(other.GetExpirationCycleFlow());
      if (other.HasExpirationCyclePressure())
        GetExpirationCyclePressure().Set(other.GetExpirationCyclePressure());
      if (other.HasExpirationWaveform())
        SetExpirationWaveform(other.GetExpirationWaveform());
      if (other.HasFractionInspiredOxygen())
        GetFractionInspiredOxygen().Set(other.GetFractionInspiredOxygen());
      if (other.HasInspirationPatientTriggerFlow())
        GetInspirationPatientTriggerFlow().Set(other.GetInspirationPatientTriggerFlow());
      if (other.HasInspirationPatientTriggerPressure())
        GetInspirationPatientTriggerPressure().Set(other.GetInspirationPatientTriggerPressure());
      if (other.HasInspirationWaveform())
        SetInspirationWaveform(other.GetInspirationWaveform());
      if (other.HasPositiveEndExpiratoryPressure())
        GetPositiveEndExpiratoryPressure().Set(other.GetPositiveEndExpiratoryPressure());
      if (other.HasSlope())
        GetSlope().Set(other.GetSlope());
    }

    public override void Clear()
    {
      base.Clear();
      if (deltaPressureSupport != null)
        deltaPressureSupport.Invalidate();
      if (expirationCycleFlow != null)
        expirationCycleFlow.Invalidate();
      if (expirationCyclePressure != null)
        expirationCyclePressure.Invalidate();
      expirationWaveform = eDriverWaveform.NullDriverWaveform;
      if (fractionInspiredOxygen != null)
        fractionInspiredOxygen.Invalidate();
      if (inspirationPatientTriggerFlow != null)
        inspirationPatientTriggerFlow.Invalidate();
      if (inspirationPatientTriggerPressure != null)
        inspirationPatientTriggerPressure.Invalidate();
      inspirationWaveform = eDriverWaveform.NullDriverWaveform;
      if (positiveEndExpiredPressure != null)
        positiveEndExpiredPressure.Invalidate();
      if (slope != null)
        slope.Invalidate();
    }

    public override bool IsValid()
    {
      return HasDeltaPressureSupport() &&
             HasFractionInspiredOxygen() &&
             HasPositiveEndExpiratoryPressure();
      //Everything else is optional
    }

    public bool HasDeltaPressureSupport()
    {
      return deltaPressureSupport == null ? false : deltaPressureSupport.IsValid();
    }
    public SEScalarPressure GetDeltaPressureSupport()
    {
      if (deltaPressureSupport == null)
        deltaPressureSupport = new SEScalarPressure();
      return deltaPressureSupport;
    }

    public bool HasFractionInspiredOxygen()
    {
      return fractionInspiredOxygen == null ? false : fractionInspiredOxygen.IsValid();
    }
    public SEScalar0To1 GetFractionInspiredOxygen()
    {
      if (fractionInspiredOxygen == null)
        fractionInspiredOxygen = new SEScalar0To1();
      return fractionInspiredOxygen;
    }

    public bool HasExpirationWaveform()
    {
      return expirationWaveform != eDriverWaveform.NullDriverWaveform;
    }
    public eDriverWaveform GetExpirationWaveform()
    {
      return expirationWaveform;
    }
    public void SetExpirationWaveform(eDriverWaveform w)
    {
      expirationWaveform = w;
    }

    public bool HasExpirationCycleFlow()
    {
      return expirationCycleFlow == null ? false : expirationCycleFlow.IsValid();
    }
    public SEScalarVolumePerTime GetExpirationCycleFlow()
    {
      if (expirationCycleFlow == null)
        expirationCycleFlow = new SEScalarVolumePerTime();
      return expirationCycleFlow;
    }

    public bool HasExpirationCyclePressure()
    {
      return expirationCyclePressure == null ? false : expirationCyclePressure.IsValid();
    }
    public SEScalarPressure GetExpirationCyclePressure()
    {
      if (expirationCyclePressure == null)
        expirationCyclePressure = new SEScalarPressure();
      return expirationCyclePressure;
    }

    public bool HasPositiveEndExpiratoryPressure()
    {
      return positiveEndExpiredPressure == null ? false : positiveEndExpiredPressure.IsValid();
    }
    public SEScalarPressure GetPositiveEndExpiratoryPressure()
    {
      if (positiveEndExpiredPressure == null)
        positiveEndExpiredPressure = new SEScalarPressure();
      return positiveEndExpiredPressure;
    }

    public bool HasInspirationWaveform()
    {
      return inspirationWaveform != eDriverWaveform.NullDriverWaveform;
    }
    public eDriverWaveform GetInspirationWaveform()
    {
      return inspirationWaveform;
    }
    public void SetInspirationWaveform(eDriverWaveform w)
    {
      inspirationWaveform = w;
    }

    public bool HasInspirationPatientTriggerFlow()
    {
      return inspirationPatientTriggerFlow == null ? false : inspirationPatientTriggerFlow.IsValid();
    }
    public SEScalarVolumePerTime GetInspirationPatientTriggerFlow()
    {
      if (inspirationPatientTriggerFlow == null)
        inspirationPatientTriggerFlow = new SEScalarVolumePerTime();
      return inspirationPatientTriggerFlow;
    }

    public bool HasInspirationPatientTriggerPressure()
    {
      return inspirationPatientTriggerPressure == null ? false : inspirationPatientTriggerPressure.IsValid();
    }
    public SEScalarPressure GetInspirationPatientTriggerPressure()
    {
      if (inspirationPatientTriggerPressure == null)
        inspirationPatientTriggerPressure = new SEScalarPressure();
      return inspirationPatientTriggerPressure;
    }

    public bool HasSlope()
    {
      return slope == null ? false : slope.IsValid();
    }
    public SEScalarTime GetSlope()
    {
      if (slope == null)
        slope = new SEScalarTime();
      return slope;
    }

    public override string ToString()
    {
      string str = "Mechanical Ventilator Volume Control";
      str += "\n\tDeltaPressureSupport: " + (HasDeltaPressureSupport() ? GetDeltaPressureSupport().ToString() : "Not Provided");
      str += "\n\tExpirationCycleFlow: " + (HasExpirationCycleFlow() ? GetExpirationCycleFlow().ToString() : "Not Provided");
      str += "\n\tExpirationCyclePressure: " + (HasExpirationCyclePressure() ? GetExpirationCyclePressure().ToString() : "Not Provided");
      str += "\n\tExpirationWaveform: " + (HasExpirationWaveform() ? GetExpirationWaveform().ToString() : "Not Provided");
      str += "\n\tFractionInspiredOxygen: " + (HasFractionInspiredOxygen() ? GetFractionInspiredOxygen().ToString() : "Not Provided");
      str += "\n\tInspirationPatientTriggerFlow: " + (HasInspirationPatientTriggerFlow() ? GetInspirationPatientTriggerFlow().ToString() : "Not Provided");
      str += "\n\tInspirationPatientTriggerPressure: " + (HasInspirationPatientTriggerPressure() ? GetInspirationPatientTriggerPressure().ToString() : "Not Provided");
      str += "\n\tInspirationWaveform: " + (HasInspirationWaveform() ? GetInspirationWaveform().ToString() : "Not Provided");
      str += "\n\tPositiveEndExpiratoryPressure: " + (HasPositiveEndExpiratoryPressure() ? GetPositiveEndExpiratoryPressure().ToString() : "Not Provided");
      str += "\n\tSlope: " + (HasSlope() ? GetSlope().ToString() : "Not Provided");


      return str;
    }
  }
}
