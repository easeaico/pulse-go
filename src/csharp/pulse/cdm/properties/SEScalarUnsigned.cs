/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

namespace Pulse.CDM
{
  public class SEScalarUnsigned : SEScalar
  {
    public SEScalarUnsigned() : base()
    {

    }

    public SEScalarUnsigned(SEScalarUnsigned from)
    {
      Set(from);
    }

    public SEScalarUnsigned(double value)
    {
      SetValue(value);
    }

    public void Set(SEScalarUnsigned from)
    {
      if (from == null)
      {
        Invalidate();
        return;
      }
      SetValue(from.value);
    }

    public override void SetValue(double val)
    {
      if (val >= 0)
        value = val;
      else
        throw new System.Exception("Value must be >= 0");
    }

    public new string ToString()
    {
      return this.value + "";
    }
  }
}
