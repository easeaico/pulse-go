/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

namespace Pulse.CDM
{
  public class SEAnesthesiaMachineChamber
  {
    protected eSwitch state;
    protected string substance;
    protected SEScalar0To1 substanceFraction;

    public SEAnesthesiaMachineChamber()
    {
      this.state = eSwitch.Off;
      this.substance = null;
      this.substanceFraction = null;
    }

    public void Clear()
    {
      state = eSwitch.Off;
      this.substance = null;
      if (substanceFraction != null)
        substanceFraction.Invalidate();
    }

    public void Copy(SEAnesthesiaMachineChamber from)
    {
      Clear();
      this.state = from.state;
      this.substance = from.substance;
      if (from.HasSubstanceFraction())
        this.GetSubstanceFraction().Set(from.substanceFraction);
    }

    public eSwitch GetState()
    {
      return state;
    }
    public void SetState(eSwitch s)
    {
      this.state = (s == eSwitch.NullSwitch) ? eSwitch.Off : s;
    }

    public bool HasSubstanceFraction()
    {
      return substanceFraction == null ? false : substanceFraction.IsValid();
    }
    public SEScalar0To1 GetSubstanceFraction()
    {
      if (substanceFraction == null)
        substanceFraction = new SEScalar0To1();
      return substanceFraction;
    }

    public bool HasSubstance()
    {
      return this.substance!=null;
    }
    public string GetSubstance()
    {
      return substance;
    }
    public void SetSubstance(string substance)
    {
      this.substance = substance;
    }

    public string ToString()
    {
      return "Anesthesia Machine Chamber"
      + "\n\tState: " + GetState()
      + "\n\tSubstance Fraction: " + GetSubstanceFraction()
      + "\n\tSubstance: " + (HasSubstance()?GetSubstance():"NotProvided");
    }
  }
}
