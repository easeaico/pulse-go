/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using pulse.cdm.bind;

namespace Pulse.CDM
{
  static class eAnesthesiaMachine
  {
    public static string OxygenSource_Name(this eAnesthesiaMachine_OxygenSource os)
    {
      return ((pulse.cdm.bind.AnesthesiaMachineData.Types.eOxygenSource)(int)os).ToString();
    }
    public static string PrimaryGas_Name(this eAnesthesiaMachine_PrimaryGas pg)
    {
      return ((pulse.cdm.bind.AnesthesiaMachineData.Types.ePrimaryGas)(int)pg).ToString();
    }
  }

  public class PBAnesthesiaMachine
  {
    #region SEAnesthesiaMachine
    public static void Load(pulse.cdm.bind.AnesthesiaMachineData src, SEAnesthesiaMachine dst/*, SESubstanceManager subMgr*/)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.AnesthesiaMachineData src, SEAnesthesiaMachine dst/*, SESubstanceManager subMgr*/)
    {
      dst.Clear();
      dst.SetConnection((eSwitch)src.Connection);
      if (src.InletFlow != null)
        PBProperty.Load(src.InletFlow, dst.GetInletFlow());
      if (src.InspiratoryExpiratoryRatio != null)
        PBProperty.Load(src.InspiratoryExpiratoryRatio, dst.GetInspiratoryExpiratoryRatio());
      if (src.OxygenFraction != null)
        PBProperty.Load(src.OxygenFraction, dst.GetOxygenFraction());
      dst.SetOxygenSource((eAnesthesiaMachine_OxygenSource)src.OxygenSource);
      if (src.PeakInspiratoryPressure != null)
        PBProperty.Load(src.PeakInspiratoryPressure, dst.GetPeakInspiratoryPressure());
      if (src.PositiveEndExpiratoryPressure != null)
        PBProperty.Load(src.PositiveEndExpiratoryPressure, dst.GetPositiveEndExpiratoryPressure());
      dst.SetPrimaryGas((eAnesthesiaMachine_PrimaryGas)src.PrimaryGas);
      if (src.RespiratoryRate != null)
        PBProperty.Load(src.RespiratoryRate, dst.GetRespiratoryRate());
      if (src.ReliefValvePressure != null)
        PBProperty.Load(src.ReliefValvePressure, dst.GetReliefValvePressure());
      if (src.LeftChamber != null)
        PBAnesthesiaMachine.Load(src.LeftChamber, dst.GetLeftChamber());
      if (src.RightChamber != null)
        PBAnesthesiaMachine.Load(src.RightChamber, dst.GetRightChamber());
      if (src.OxygenBottleOne != null)
        PBAnesthesiaMachine.Load(src.OxygenBottleOne, dst.GetOxygenBottleOne());
      if (src.OxygenBottleTwo != null)
        PBAnesthesiaMachine.Load(src.OxygenBottleTwo, dst.GetOxygenBottleTwo());
    }

    public static pulse.cdm.bind.AnesthesiaMachineData Unload(SEAnesthesiaMachine src)
    {
      pulse.cdm.bind.AnesthesiaMachineData dst = new pulse.cdm.bind.AnesthesiaMachineData();
      Unload(src, dst);
      return dst;
    }
    protected static void Unload(SEAnesthesiaMachine src, pulse.cdm.bind.AnesthesiaMachineData dst)
    {
      dst.Connection = (pulse.cdm.bind.eSwitch)(int)src.GetConnection();
      if (src.HasInletFlow())
        dst.InletFlow = PBProperty.Unload(src.GetInletFlow());
      if (src.HasInspiratoryExpiratoryRatio())
        dst.InspiratoryExpiratoryRatio = PBProperty.Unload(src.GetInspiratoryExpiratoryRatio());
      if (src.HasOxygenFraction())
        dst.OxygenFraction = PBProperty.Unload(src.GetOxygenFraction());
      dst.OxygenSource = (pulse.cdm.bind.AnesthesiaMachineData.Types.eOxygenSource)(int)src.GetOxygenSource();
      if (src.HasPeakInspiratoryPressure())
        dst.PeakInspiratoryPressure = PBProperty.Unload(src.GetPeakInspiratoryPressure());
      if (src.HasPositiveEndExpiratoryPressure())
        dst.PositiveEndExpiratoryPressure = PBProperty.Unload(src.GetPositiveEndExpiratoryPressure());
      dst.PrimaryGas = (pulse.cdm.bind.AnesthesiaMachineData.Types.ePrimaryGas)(int)src.GetPrimaryGas();
      if (src.HasRespiratoryRate())
        dst.RespiratoryRate = PBProperty.Unload(src.GetRespiratoryRate());
      if (src.HasReliefValvePressure())
        dst.ReliefValvePressure = PBProperty.Unload(src.GetReliefValvePressure());

      if (src.HasLeftChamber())
        dst.LeftChamber = Unload(src.GetLeftChamber());
      if (src.HasRightChamber())
        dst.RightChamber = Unload(src.GetRightChamber());
      if (src.HasOxygenBottleOne())
        dst.OxygenBottleOne = Unload(src.GetOxygenBottleOne());
      if (src.HasOxygenBottleTwo())
        dst.OxygenBottleTwo = Unload(src.GetOxygenBottleTwo());
    }
    #endregion

    #region SEAnesthesiaMachineChamber
    public static void Load(AnesthesiaMachineChamberData src, SEAnesthesiaMachineChamber dst)
    {
      dst.Clear();
      if (src.State != pulse.cdm.bind.eSwitch.NullSwitch)
        dst.SetState((eSwitch)src.State);
      if (src.Substance != null)
        dst.SetSubstance(src.Substance);
      if (src.SubstanceFraction != null)
        PBProperty.Load(src.SubstanceFraction, dst.GetSubstanceFraction());
    }
    public static AnesthesiaMachineChamberData Unload(SEAnesthesiaMachineChamber src)
    {
      AnesthesiaMachineChamberData dst = new AnesthesiaMachineChamberData();
      Unload(src, dst);
      return dst;
    }
    protected static void Unload(SEAnesthesiaMachineChamber src, AnesthesiaMachineChamberData dst)
    {
      dst.State = (pulse.cdm.bind.eSwitch)src.GetState();
      if (src.HasSubstance())
        dst.Substance = src.GetSubstance();
      if (src.HasSubstanceFraction())
        dst.SubstanceFraction = PBProperty.Unload(src.GetSubstanceFraction());
    }
    #endregion

    #region SEAnesthesiaMachineOxygenBottle
    public static void Load(AnesthesiaMachineOxygenBottleData src, SEAnesthesiaMachineOxygenBottle dst)
    {
      dst.Clear();
      if (src.Volume != null)
        PBProperty.Load(src.Volume, dst.GetVolume());
    }
    public static AnesthesiaMachineOxygenBottleData Unload(SEAnesthesiaMachineOxygenBottle src)
    {
      AnesthesiaMachineOxygenBottleData dst = new AnesthesiaMachineOxygenBottleData();
      Unload(src, dst);
      return dst;
    }
    protected static void Unload(SEAnesthesiaMachineOxygenBottle src, AnesthesiaMachineOxygenBottleData dst)
    {
      if (src.HasVolume())
        dst.Volume = PBProperty.Unload(src.GetVolume());
    }
    #endregion
  }
}
