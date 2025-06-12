/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

namespace Pulse.CDM
{
  public class SEAirwayObstruction : SEPatientAction
  {
    protected SEScalar0To1 severity;
    protected bool hasSecretions;

    public SEAirwayObstruction()
    {
      severity = null;
      hasSecretions = false;
    }

    public override void Clear()
    {
      base.Clear();
      if (severity != null)
        severity.Invalidate();
      hasSecretions = false;
    }

    public override bool IsValid()
    {
      return HasSeverity();
    }

    public bool HasSeverity()
    {
      return severity == null ? false : severity.IsValid();
    }
    public SEScalar0To1 GetSeverity()
    {
      if (severity == null)
        severity = new SEScalar0To1();
      return severity;
    }

    public bool GetHasSecretions()
    {
      return hasSecretions;
    }
    public void SetHasSecretions(bool hasSecretions)
    {
      this.hasSecretions = hasSecretions;
    }
  }
}
