/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

namespace Pulse.CDM
{
  public class SEAnesthesiaMachineOxygenBottle
  {
    protected SEScalarVolume volume;

    public SEAnesthesiaMachineOxygenBottle()
    {
      volume = null;
    }

    public void Clear()
    {
      if (volume != null)
        volume.Invalidate();
    }

    public void Copy(SEAnesthesiaMachineOxygenBottle from)
    {
      Clear();
      if(from.HasVolume())
        this.GetVolume().Set(from.GetVolume());
    }

    public bool HasVolume()
    {
      return volume == null ? false : volume.IsValid();
    }

    public SEScalarVolume GetVolume()
    {
      if (volume == null)
        volume = new SEScalarVolume();
      return volume;
    }

    public override string ToString()
    {
      return "Anesthesia Machine Oxygen Bottle"
          + "\n\tVolume: " + GetVolume();
    }
  }
}