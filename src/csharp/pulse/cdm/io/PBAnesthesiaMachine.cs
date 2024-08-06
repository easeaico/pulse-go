/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

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
      //if (src.LeftChamber!=null)
      //  PBAnesthesiaMachine.Load(src.GetLeftChamber(), dst.GetLeftChamber(), subMgr);
      //if (src.RightChamber!=null)
      //  PBAnesthesiaMachine.Load(src.GetRightChamber(), dst.GetRightChamber(), subMgr);
      //if (src.OxygenBottleOne!=null)
      //  PBAnesthesiaMachine.Load(src.GetOxygenBottleOne(), dst.GetOxygenBottleOne());
      //if (src.OxygenBottleTwo!=null)
      //  PBAnesthesiaMachine.Load(src.GetOxygenBottleTwo(), dst.GetOxygenBottleTwo());
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

      //if (src.HasLeftChamber())
      //  dst.LeftChamber(SEAnesthesiaMachineChamber.Unload(src.leftChamber));
      //if (src.HasRightChamber())
      //  dst.RightChamber(SEAnesthesiaMachineChamber.Unload(src.rightChamber));
      //if (src.HasOxygenBottleOne())
      //  dst.OxygenBottleOne(SEAnesthesiaMachineOxygenBottle.Unload(src.oxygenBottleOne));
      //if (src.HasOxygenBottleTwo())
      //  dst.OxygenBottleTwo(SEAnesthesiaMachineOxygenBottle.Unload(src.oxygenBottleTwo));
    }
    #endregion
  }

  //public static void Load(AnesthesiaMachineChamberData src, SEAnesthesiaMachineChamber dst, SESubstanceManager subMgr)
  //{
  //  dst.reSet();
  //  if (src.GetState() != eSwitch.UNRECOGNIZED && src.GetState() != eSwitch.NullSwitch)
  //    dst.SetState(src.GetState());
  //  if (src.GetSubstance() != null)
  //    dst.SetSubstance(subMgr.GetSubstance(src.GetSubstance()));
  //  if (src.HasSubstanceFraction())
  //    SEScalar0To1.Load(src.GetSubstanceFraction(), dst.GetSubstanceFraction());
  //}
  //public static AnesthesiaMachineChamberData Unload(SEAnesthesiaMachineChamber src)
  //{
  //  AnesthesiaMachineChamberData.Builder dst = AnesthesiaMachineChamberData.newBuilder();
  //  Unload(src, dst);
  //  return dst.build();
  //}
  //protected static void Unload(SEAnesthesiaMachineChamber src, AnesthesiaMachineChamberData.Builder dst)
  //{
  //  if (src.HasSubstance())
  //    dst.SetSubstance(src.substance.GetName());
  //  dst.SetState(src.state);
  //  if (src.HasSubstanceFraction())
  //    dst.SetSubstanceFraction(SEScalar0To1.Unload(src.substanceFraction));
  //}
}
