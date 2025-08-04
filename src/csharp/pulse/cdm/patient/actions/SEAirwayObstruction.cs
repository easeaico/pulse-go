/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

namespace Pulse.CDM
{

  public enum eAirwayObstruction_ResistanceType : int
  {
    Constant = 0,
    Oscillating
  }

  public class SEAirwayObstruction : SEPatientAction
  {
    protected SEScalar0To1 severity;
    protected eAirwayObstruction_ResistanceType resistance_type;

    public SEAirwayObstruction()
    {
      severity = null;
      resistance_type = eAirwayObstruction_ResistanceType.Constant;
    }

    public override void Clear()
    {
      base.Clear();
      if (severity != null)
        severity.Invalidate();
      resistance_type = eAirwayObstruction_ResistanceType.Constant;
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

    public eAirwayObstruction_ResistanceType GetResistanceType()
    {
      return resistance_type;
    }
    public void SetResistanceType(eAirwayObstruction_ResistanceType rt)
    {
      this.resistance_type = rt;
    }
  }
}
