/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

namespace Pulse.CDM
{
  public class PBBagValveMask
  {
    #region SEBagValveMask
    public static void Load(pulse.cdm.bind.BagValveMaskData src, SEBagValveMask dst/*, SESubstanceManager subMgr*/)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.BagValveMaskData src, SEBagValveMask dst/*, SESubstanceManager subMgr*/)
    {
      dst.Clear();
      dst.SetConnection((eSwitch)src.Connection);
      if (src.BagResistance != null)
        PBProperty.Load(src.BagResistance, dst.GetBagResistance());
      if (src.ConnectionVolume != null)
        PBProperty.Load(src.ConnectionVolume, dst.GetConnectionVolume());
      if (src.FilterResistance != null)
        PBProperty.Load(src.FilterResistance, dst.GetFilterResistance());
      if (src.FilterVolume != null)
        PBProperty.Load(src.FilterVolume, dst.GetFilterVolume());
      if (src.SealResistance != null)
        PBProperty.Load(src.SealResistance, dst.GetSealResistance());
      if (src.ValvePositiveEndExpiratoryPressure != null)
        PBProperty.Load(src.ValvePositiveEndExpiratoryPressure, dst.GetValvePositiveEndExpiratoryPressure());
      if (src.ValveResistance != null)
        PBProperty.Load(src.ValveResistance, dst.GetValveResistance());
      if (src.ValveVolume != null)
        PBProperty.Load(src.ValveVolume, dst.GetValveVolume());

      foreach (pulse.cdm.bind.SubstanceFractionData sf in src.FractionInspiredGas)
      {
        PBProperty.Load(sf.Amount, dst.GetFractionInspiredGas(sf.Name).GetFractionAmount());
      }

      foreach (pulse.cdm.bind.SubstanceConcentrationData sc in src.ConcentrationInspiredAerosol)
      {
        PBProperty.Load(sc.Concentration, dst.GetConcentrationInspiredAerosol(sc.Name).GetConcentration());
      }
    }

    public static pulse.cdm.bind.BagValveMaskData Unload(SEBagValveMask src)
    {
      pulse.cdm.bind.BagValveMaskData dst = new pulse.cdm.bind.BagValveMaskData();
      Unload(src, dst);
      return dst;
    }
    protected static void Unload(SEBagValveMask src, pulse.cdm.bind.BagValveMaskData dst)
    {
      dst.Connection = (pulse.cdm.bind.eSwitch)(int)src.GetConnection();
      if (src.HasBagResistance())
        dst.BagResistance = PBProperty.Unload(src.GetBagResistance());
      if (src.HasConnectionVolume())
        dst.ConnectionVolume = PBProperty.Unload(src.GetConnectionVolume());
      if (src.HasFilterResistance())
        dst.FilterResistance = PBProperty.Unload(src.GetFilterResistance());
      if (src.HasFilterVolume())
        dst.FilterVolume = PBProperty.Unload(src.GetFilterVolume());
      if (src.HasSealResistance())
        dst.SealResistance = PBProperty.Unload(src.GetSealResistance());
      if (src.HasValvePositiveEndExpiratoryPressure())
        dst.ValvePositiveEndExpiratoryPressure = PBProperty.Unload(src.GetValvePositiveEndExpiratoryPressure());
      if (src.HasValveResistance())
        dst.ValveResistance = PBProperty.Unload(src.GetValveResistance());
      if (src.HasValveVolume())
        dst.ValveVolume = PBProperty.Unload(src.GetValveVolume());

      foreach (SESubstanceFraction aGas in src.GetFractionInspiredGases())
      {
        pulse.cdm.bind.SubstanceFractionData sf = new pulse.cdm.bind.SubstanceFractionData();
        sf.Name = aGas.GetSubstance();
        sf.Amount = PBProperty.Unload(aGas.GetFractionAmount());
        dst.FractionInspiredGas.Add(sf);
      }

      foreach (SESubstanceConcentration aAerosol in src.GetConcentrationInspiredAerosols())
      {
        pulse.cdm.bind.SubstanceConcentrationData sc = new pulse.cdm.bind.SubstanceConcentrationData();
        sc.Name = aAerosol.GetSubstance();
        sc.Concentration = PBProperty.Unload(aAerosol.GetConcentration());
        dst.ConcentrationInspiredAerosol.Add(sc);
      }
    }
    #endregion
  }
}
