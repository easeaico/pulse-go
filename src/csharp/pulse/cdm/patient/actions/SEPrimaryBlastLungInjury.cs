/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using System.Collections;
using System.Collections.Generic;

namespace Pulse.CDM
{
  using LungImpairmentMap = Dictionary<eLungCompartment, SEScalar0To1>;
  public class SEPrimaryBlastLungInjury : SEPatientAction
  {
    protected LungImpairmentMap severities;

    public SEPrimaryBlastLungInjury()
    {
      severities = new LungImpairmentMap();
    }

    public override void Clear()
    {
      base.Clear();
      foreach (var itr in severities)
        itr.Value.Invalidate();
    }

    public override bool IsValid()
    {
      return HasSeverity();
    }

    public bool HasSeverity()
    {
      foreach (var itr in severities)
        if (itr.Value.IsValid())
          return true;
      return false;
    }
    public bool HasSeverity(eLungCompartment c)
    {
      if (!severities.ContainsKey(c))
        return false;
      return severities[c].IsValid();
    }
    public SEScalar0To1 GetSeverity(eLungCompartment c)
    {
      SEScalar0To1 s = severities[c];
      if(s == null)
      {
        s = new SEScalar0To1();
        severities[c] = s;
      }
      return s;
    }
    public LungImpairmentMap GetSeverities()
    {
      return severities;
    }
  }
}
