/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using System.Collections.Generic;

namespace Pulse.CDM
{
  public class SEMechanicalVentilation : SEPatientAction
  {
    protected eSwitch               state;
    protected SEScalarVolumePerTime flow;
    protected SEScalarPressure      pressure;

    protected List<SESubstanceFraction> gas_fractions;
    protected List<SESubstanceConcentration> aerosols;

    public SEMechanicalVentilation()
    {
      this.state = 0;
      this.flow = null;
      this.pressure = null;
      this.mechanicaldeadspacevolume = null;

      this.gas_fractions = new List<SESubstanceFraction>();
      this.aerosols = new List<SESubstanceConcentration>();
    }

    public SEMechanicalVentilation(SEMechanicalVentilation other)
    {
      Copy(other);
    }

    public override void Clear()
    {
      base.Clear();
      state = 0;
      if (flow != null)
        flow.Invalidate();
      if (pressure != null)
        pressure.Invalidate();
      if (mechanicaldeadspacevolume != null)
        mechanicaldeadspacevolume.Invalidate();

      gas_fractions.Clear();
      aerosols.Clear();

    }

    public void Copy(SEMechanicalVentilation from)
    {
      if (this == from)
        return;
      base.Copy(from);
      if (from.state != eSwitch.NullSwitch)
        this.SetState(from.state);
      if (from.HasFlow())
        this.GetFlow().Set(from.GetFlow());
      if (from.HasPressure())
        this.GetPressure().Set(from.GetPressure());
      if (from.HasMechanicalDeadSpaceVolume())
        this.GetMechanicalDeadSpaceVolume().Set(from.GetMechanicalDeadSpaceVolume());

      if (from.gas_fractions != null)
      {
        SESubstanceFraction mine;
        foreach (SESubstanceFraction sf in from.gas_fractions)
        {
          mine = this.CreateGasFraction(sf.GetSubstance());
          if (sf.HasFractionAmount())
            mine.GetFractionAmount().Set(sf.GetFractionAmount());
        }
      }

      if (from.aerosols != null)
      {
        SESubstanceConcentration mine;
        foreach (SESubstanceConcentration sc in from.aerosols)
        {
          mine = this.CreateAerosol(sc.GetSubstance());
          if (sc.HasConcentration())
            mine.GetConcentration().Set(sc.GetConcentration());
        }
      }
    }

    public bool IsActive()
    {
      return IsValid();
    }

    public override bool IsValid()
    {
      if (GetState() == eSwitch.Off)
        return true;
      else
      {
        if (HasGasFraction())
        {
          double total = 0;
          foreach (SESubstanceFraction sf in gas_fractions)
          {
            total += sf.GetFractionAmount().GetValue();
          }
          if (!SEScalar.IsValue(1, total))
          {
            //Error("Mechanical Ventilation Gas fractions do not sum to 1");
            return false;
          }
        }
      }
      if (!HasPressure() && !HasFlow())
      {
        //Error("Mechanical Ventilation must have pressure and/or flow");
        return false;
      }
      return true;
    }

    public bool HasState()
    {
      return state != 0;
    }
    public eSwitch GetState()
    {
      return state;
    }
    public void SetState(eSwitch st)
    {
      state = st;
    }

    public SEScalarVolumePerTime GetFlow()
    {
      if (flow == null)
        flow = new SEScalarVolumePerTime();
      return flow;
    }
    public bool HasFlow()
    {
      return flow == null ? false : flow.IsValid();
    }

    public SEScalarPressure GetPressure()
    {
      if (pressure == null)
        pressure = new SEScalarPressure();
      return pressure;
    }
    public bool HasPressure()
    {
      return pressure == null ? false : pressure.IsValid();
    }

    public SEScalarVolume GetMechanicalDeadSpaceVolume()
    {
      if (mechanicaldeadspacevolume == null)
        mechanicaldeadspacevolume = new SEScalarVolume();
      return mechanicaldeadspacevolume;
    }
    public bool HasPressure()
    {
      return pressure == null ? false : pressure.IsValid();
    }

    public SESubstanceFraction CreateGasFraction(string substance)
    {
      return GetGasFraction(substance);
    }
    public SESubstanceFraction GetGasFraction(string substance)
    {
      foreach (SESubstanceFraction sf in gas_fractions)
      {
        if (sf.GetSubstance() == substance)
        {
          return sf;
        }
      }
      SESubstanceFraction nsf = new SESubstanceFraction(substance);
      gas_fractions.Add(nsf);
      return nsf;
    }
    public bool HasGasFraction()
    {
      return gas_fractions.Count != 0;
    }
    public bool HasGasFraction(string substance)
    {
      foreach (SESubstanceFraction sf in gas_fractions)
      {
        if (sf.GetSubstance() == substance)
        {
          return true;
        }
      }
      return false;
    }
    public List<SESubstanceFraction> GetGasFractions()
    {
      return gas_fractions;
    }
    public void RemoveGasFraction(string substance)
    {
      foreach (SESubstanceFraction sf in gas_fractions)
      {
        if (sf.GetSubstance() == substance)
        {
          gas_fractions.Remove(sf);
          return;
        }
      }
    }
    public void RemoveGasFractions()
    {
      gas_fractions.Clear();
    }

    public SESubstanceConcentration CreateAerosol(string substance)
    {
      return GetAerosol(substance);
    }
    public SESubstanceConcentration GetAerosol(string substance)
    {
      foreach (SESubstanceConcentration sc in aerosols)
      {
        if (sc.GetSubstance() == substance)
        {
          return sc;
        }
      }
      SESubstanceConcentration nsc = new SESubstanceConcentration(substance);
      aerosols.Add(nsc);
      return nsc;
    }
    public bool HasAerosol()
    {
      return aerosols.Count != 0;
    }
    public bool HasAerosol(string substance)
    {
      foreach (SESubstanceConcentration sc in aerosols)
      {
        if (sc.GetSubstance() == substance)
        {
          return true;
        }
      }
      return false;
    }
    public List<SESubstanceConcentration> GetAerosols()
    {
      return aerosols;
    }
    public void RemoveAerosol(string substance)
    {
      foreach (SESubstanceConcentration sc in aerosols)
      {
        if (sc.GetSubstance() == substance)
        {
          aerosols.Remove(sc);
          return;
        }
      }
    }
    public void RemoveAerosols()
    {
      aerosols.Clear();
    }

    public new string ToString()
    {
      string str = "Envriomental Conditions:"
          + "\n\tState: " + GetState()
          + "\n\tFlow: " + (HasFlow() ? GetFlow().ToString() : "None")
          + "\n\tPressure: " + (HasPressure() ? GetPressure().ToString() : "None");
          + "\n\tMechanicalDeadSpaceVolume: " + (HasMechanicalDeadSpaceVolume() ? GetMechanicalDeadSpaceVolume().ToString() : "None");
      foreach (SESubstanceFraction sf in this.gas_fractions)
        str += "\n\t" + sf.GetSubstance();
      foreach (SESubstanceConcentration sc in this.aerosols)
        str += "\n\t" + sc.GetSubstance();

      return str;
    }

  }
}
