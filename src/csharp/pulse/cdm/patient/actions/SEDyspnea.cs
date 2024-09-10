/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

namespace Pulse.CDM
{
  public class SEDyspnea : SEPatientAction
  {
    protected SEScalar0To1 respiration_rate_severity;
    protected SEScalar0To1 tidal_volume_severity;

    public SEDyspnea()
    {
      respiration_rate_severity = null;
      tidal_volume_severity = null;
    }

    public override void Clear()
    {
      base.Clear();
      if (respiration_rate_severity != null)
        respiration_rate_severity.Invalidate();
      if (tidal_volume_severity != null)
        tidal_volume_severity.Invalidate();
    }

    public override bool IsValid()
    {
      return HasRespirationRateSeverity() || HasTidalVolumeSeverity();
    }

    public bool HasRespirationRateSeverity()
    {
      return respiration_rate_severity == null ? false : respiration_rate_severity.IsValid();
    }
    public SEScalar0To1 GetRespirationRateSeverity()
    {
      if (respiration_rate_severity == null)
        respiration_rate_severity = new SEScalar0To1();
      return respiration_rate_severity;
    }

    public bool HasTidalVolumeSeverity()
    {
      return tidal_volume_severity == null ? false : tidal_volume_severity.IsValid();
    }
    public SEScalar0To1 GetTidalVolumeSeverity()
    {
      if (tidal_volume_severity == null)
        tidal_volume_severity = new SEScalar0To1();
      return tidal_volume_severity;
    }
  }
}
