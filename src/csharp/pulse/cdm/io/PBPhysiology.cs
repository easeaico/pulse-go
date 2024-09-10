/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using pulse.cdm.bind;
using System.Collections.Generic;

namespace Pulse.CDM
{
  public class PBPhysiology
  {
    #region SECardiovascularMechanicsModifiers
    public static void Load(pulse.cdm.bind.CardiovascularMechanicsModifiersData src, SECardiovascularMechanicsModifiers dst/*, SESubstanceManager subMgr*/)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.CardiovascularMechanicsModifiersData src, SECardiovascularMechanicsModifiers dst/*, SESubstanceManager subMgr*/)
    {
      dst.Clear();

      if (src.ArterialComplianceMultiplier != null)
        PBProperty.Load(src.ArterialComplianceMultiplier, dst.GetArterialComplianceMultiplier());
      if (src.ArterialResistanceMultiplier != null)
        PBProperty.Load(src.ArterialResistanceMultiplier, dst.GetArterialResistanceMultiplier());

      if (src.PulmonaryComplianceMultiplier != null)
        PBProperty.Load(src.PulmonaryComplianceMultiplier, dst.GetPulmonaryComplianceMultiplier());
      if (src.PulmonaryResistanceMultiplier != null)
        PBProperty.Load(src.PulmonaryResistanceMultiplier, dst.GetPulmonaryResistanceMultiplier());
      if (src.SystemicResistanceMultiplier != null)
        PBProperty.Load(src.SystemicResistanceMultiplier, dst.GetSystemicResistanceMultiplier());
      if (src.SystemicComplianceMultiplier != null)
        PBProperty.Load(src.SystemicComplianceMultiplier, dst.GetSystemicComplianceMultiplier());
      if (src.VenousComplianceMultiplier != null)
        PBProperty.Load(src.VenousComplianceMultiplier, dst.GetVenousComplianceMultiplier());
      if (src.VenousResistanceMultiplier != null)
        PBProperty.Load(src.VenousResistanceMultiplier, dst.GetVenousResistanceMultiplier());
      if (src.HeartRateMultiplier != null)
        PBProperty.Load(src.HeartRateMultiplier, dst.GetHeartRateMultiplier());
      if (src.HeartRateMultiplier != null)
        PBProperty.Load(src.HeartRateMultiplier, dst.GetHeartRateMultiplier());
    }

    public static pulse.cdm.bind.CardiovascularMechanicsModifiersData Unload(SECardiovascularMechanicsModifiers src)
    {
      pulse.cdm.bind.CardiovascularMechanicsModifiersData dst = new pulse.cdm.bind.CardiovascularMechanicsModifiersData();
      Unload(src, dst);
      return dst;
    }
    protected static void Unload(SECardiovascularMechanicsModifiers src, pulse.cdm.bind.CardiovascularMechanicsModifiersData dst)
    {
      if (src.HasArterialComplianceMultiplier())
        dst.ArterialComplianceMultiplier = PBProperty.Unload(src.GetArterialComplianceMultiplier());
      if (src.HasArterialResistanceMultiplier())
        dst.ArterialResistanceMultiplier = PBProperty.Unload(src.GetArterialResistanceMultiplier());

      if (src.HasPulmonaryComplianceMultiplier())
        dst.PulmonaryComplianceMultiplier = PBProperty.Unload(src.GetPulmonaryComplianceMultiplier());
      if (src.HasPulmonaryResistanceMultiplier())
        dst.PulmonaryResistanceMultiplier = PBProperty.Unload(src.GetPulmonaryResistanceMultiplier());
      if (src.HasSystemicResistanceMultiplier())
        dst.SystemicResistanceMultiplier = PBProperty.Unload(src.GetSystemicResistanceMultiplier());
      if (src.HasSystemicComplianceMultiplier())
        dst.SystemicComplianceMultiplier = PBProperty.Unload(src.GetSystemicComplianceMultiplier());
      if (src.HasVenousComplianceMultiplier())
        dst.VenousComplianceMultiplier = PBProperty.Unload(src.GetVenousComplianceMultiplier());
      if (src.HasVenousResistanceMultiplier())
        dst.VenousResistanceMultiplier = PBProperty.Unload(src.GetVenousResistanceMultiplier());
      if (src.HasHeartRateMultiplier())
        dst.HeartRateMultiplier = PBProperty.Unload(src.GetHeartRateMultiplier());
      if (src.HasHeartRateMultiplier())
        dst.HeartRateMultiplier = PBProperty.Unload(src.GetHeartRateMultiplier());
    }
    #endregion

    #region SERespiratoryMechanics
    public static void Load(pulse.cdm.bind.RespiratoryMechanicsData src, SERespiratoryMechanics dst/*, SESubstanceManager subMgr*/)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.RespiratoryMechanicsData src, SERespiratoryMechanics dst/*, SESubstanceManager subMgr*/)
    {
      dst.Clear();
      dst.SetActive((eSwitch)src.Active);
      dst.SetDefaultType((eDefaultType)src.DefaultType);

      if (src.LeftComplianceCurve != null)
        PBProperty.Load(src.LeftComplianceCurve, dst.GetLeftComplianceCurve());
      if (src.RightComplianceCurve != null)
        PBProperty.Load(src.RightComplianceCurve, dst.GetRightComplianceCurve());

      if (src.LeftExpiratoryResistance != null)
        PBProperty.Load(src.LeftExpiratoryResistance, dst.GetLeftExpiratoryResistance());
      if (src.LeftInspiratoryResistance != null)
        PBProperty.Load(src.LeftInspiratoryResistance, dst.GetLeftInspiratoryResistance());
      if (src.RightExpiratoryResistance != null)
        PBProperty.Load(src.RightExpiratoryResistance, dst.GetRightExpiratoryResistance());
      if (src.RightInspiratoryResistance != null)
        PBProperty.Load(src.RightInspiratoryResistance, dst.GetRightInspiratoryResistance());
      if (src.UpperExpiratoryResistance != null)
        PBProperty.Load(src.UpperExpiratoryResistance, dst.GetUpperExpiratoryResistance());
      if (src.UpperInspiratoryResistance != null)
        PBProperty.Load(src.UpperInspiratoryResistance, dst.GetUpperInspiratoryResistance());
      if (src.InspiratoryPeakPressure != null)
        PBProperty.Load(src.InspiratoryPeakPressure, dst.GetInspiratoryPeakPressure());
      if (src.ExpiratoryPeakPressure != null)
        PBProperty.Load(src.ExpiratoryPeakPressure, dst.GetExpiratoryPeakPressure());
      if (src.InspiratoryRiseTime != null)
        PBProperty.Load(src.InspiratoryRiseTime, dst.GetInspiratoryRiseTime());
      if (src.InspiratoryHoldTime != null)
        PBProperty.Load(src.InspiratoryHoldTime, dst.GetInspiratoryHoldTime());
      if (src.InspiratoryReleaseTime != null)
        PBProperty.Load(src.InspiratoryReleaseTime, dst.GetInspiratoryReleaseTime());
      if (src.InspiratoryToExpiratoryPauseTime != null)
        PBProperty.Load(src.InspiratoryToExpiratoryPauseTime, dst.GetInspiratoryToExpiratoryPauseTime());
      if (src.ExpiratoryRiseTime != null)
        PBProperty.Load(src.ExpiratoryRiseTime, dst.GetExpiratoryRiseTime());
      if (src.ExpiratoryHoldTime != null)
        PBProperty.Load(src.ExpiratoryHoldTime, dst.GetExpiratoryHoldTime());
      if (src.ExpiratoryReleaseTime != null)
        PBProperty.Load(src.ExpiratoryReleaseTime, dst.GetExpiratoryReleaseTime());
      if (src.ResidueTime != null)
        PBProperty.Load(src.ResidueTime, dst.GetResidueTime());
    }

    public static pulse.cdm.bind.RespiratoryMechanicsData Unload(SERespiratoryMechanics src)
    {
      pulse.cdm.bind.RespiratoryMechanicsData dst = new pulse.cdm.bind.RespiratoryMechanicsData();
      Unload(src, dst);
      return dst;
    }
    protected static void Unload(SERespiratoryMechanics src, pulse.cdm.bind.RespiratoryMechanicsData dst)
    {
      dst.Active = (pulse.cdm.bind.eSwitch)(int)src.GetActive();
      dst.DefaultType = (pulse.cdm.bind.eDefaultType)(int)src.GetDefaultType();

      if (src.HasLeftComplianceCurve())
        dst.LeftComplianceCurve = PBProperty.Unload(src.GetLeftComplianceCurve());
      if (src.HasRightComplianceCurve())
        dst.RightComplianceCurve = PBProperty.Unload(src.GetRightComplianceCurve());

      if (src.HasLeftExpiratoryResistance())
        dst.LeftExpiratoryResistance = PBProperty.Unload(src.GetLeftExpiratoryResistance());
      if (src.HasLeftInspiratoryResistance())
        dst.LeftInspiratoryResistance = PBProperty.Unload(src.GetLeftInspiratoryResistance());
      if (src.HasRightExpiratoryResistance())
        dst.RightExpiratoryResistance = PBProperty.Unload(src.GetRightExpiratoryResistance());
      if (src.HasRightInspiratoryResistance())
        dst.RightInspiratoryResistance = PBProperty.Unload(src.GetRightInspiratoryResistance());
      if (src.HasUpperExpiratoryResistance())
        dst.UpperExpiratoryResistance = PBProperty.Unload(src.GetUpperExpiratoryResistance());
      if (src.HasUpperInspiratoryResistance())
        dst.UpperInspiratoryResistance = PBProperty.Unload(src.GetUpperInspiratoryResistance());
      if (src.HasInspiratoryPeakPressure())
        dst.InspiratoryPeakPressure = PBProperty.Unload(src.GetInspiratoryPeakPressure());
      if (src.HasExpiratoryPeakPressure())
        dst.ExpiratoryPeakPressure = PBProperty.Unload(src.GetExpiratoryPeakPressure());
      if (src.HasInspiratoryRiseTime())
        dst.InspiratoryRiseTime = PBProperty.Unload(src.GetInspiratoryRiseTime());
      if (src.HasInspiratoryHoldTime())
        dst.InspiratoryHoldTime = PBProperty.Unload(src.GetInspiratoryHoldTime());
      if (src.HasInspiratoryReleaseTime())
        dst.InspiratoryReleaseTime = PBProperty.Unload(src.GetInspiratoryReleaseTime());
      if (src.HasInspiratoryToExpiratoryPauseTime())
        dst.InspiratoryToExpiratoryPauseTime = PBProperty.Unload(src.GetInspiratoryToExpiratoryPauseTime());
      if (src.HasExpiratoryRiseTime())
        dst.ExpiratoryRiseTime = PBProperty.Unload(src.GetExpiratoryRiseTime());
      if (src.HasExpiratoryHoldTime())
        dst.ExpiratoryHoldTime = PBProperty.Unload(src.GetExpiratoryHoldTime());
      if (src.HasExpiratoryReleaseTime())
        dst.ExpiratoryReleaseTime = PBProperty.Unload(src.GetExpiratoryReleaseTime());
      if (src.HasResidueTime())
        dst.ResidueTime = PBProperty.Unload(src.GetResidueTime());
    }
    #endregion

    #region SERespiratoryMechanicsModifiers
    public static void Load(pulse.cdm.bind.RespiratoryMechanicsModifiersData src, SERespiratoryMechanicsModifiers dst/*, SESubstanceManager subMgr*/)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.RespiratoryMechanicsModifiersData src, SERespiratoryMechanicsModifiers dst/*, SESubstanceManager subMgr*/)
    {
      dst.Clear();

      if (src.LeftComplianceMultiplier != null)
        PBProperty.Load(src.LeftComplianceMultiplier, dst.GetLeftComplianceMultiplier());
      if (src.RightComplianceMultiplier != null)
        PBProperty.Load(src.RightComplianceMultiplier, dst.GetRightComplianceMultiplier());

      if (src.LeftExpiratoryResistanceMultiplier != null)
        PBProperty.Load(src.LeftExpiratoryResistanceMultiplier, dst.GetLeftExpiratoryResistanceMultiplier());
      if (src.LeftInspiratoryResistanceMultiplier != null)
        PBProperty.Load(src.LeftInspiratoryResistanceMultiplier, dst.GetLeftInspiratoryResistanceMultiplier());
      if (src.RightExpiratoryResistanceMultiplier != null)
        PBProperty.Load(src.RightExpiratoryResistanceMultiplier, dst.GetRightExpiratoryResistanceMultiplier());
      if (src.RightInspiratoryResistanceMultiplier != null)
        PBProperty.Load(src.RightInspiratoryResistanceMultiplier, dst.GetRightInspiratoryResistanceMultiplier());
      if (src.UpperExpiratoryResistanceMultiplier != null)
        PBProperty.Load(src.UpperExpiratoryResistanceMultiplier, dst.GetUpperExpiratoryResistanceMultiplier());
      if (src.UpperInspiratoryResistanceMultiplier != null)
        PBProperty.Load(src.UpperInspiratoryResistanceMultiplier, dst.GetUpperInspiratoryResistanceMultiplier());
      if (src.RespirationRateMultiplier != null)
        PBProperty.Load(src.RespirationRateMultiplier, dst.GetRespirationRateMultiplier());
      if (src.TidalVolumeMultiplier != null)
        PBProperty.Load(src.TidalVolumeMultiplier, dst.GetTidalVolumeMultiplier());

      foreach (var lvi in src.VolumeIncrement)
        PBProperty.Load(lvi.Increment, dst.GetLungVolumeIncrement((eLungCompartment)lvi.Compartment));
    }

    public static pulse.cdm.bind.RespiratoryMechanicsModifiersData Unload(SERespiratoryMechanicsModifiers src)
    {
      pulse.cdm.bind.RespiratoryMechanicsModifiersData dst = new pulse.cdm.bind.RespiratoryMechanicsModifiersData();
      Unload(src, dst);
      return dst;
    }
    protected static void Unload(SERespiratoryMechanicsModifiers src, pulse.cdm.bind.RespiratoryMechanicsModifiersData dst)
    {
      if (src.HasLeftComplianceMultiplier())
        dst.LeftComplianceMultiplier = PBProperty.Unload(src.GetLeftComplianceMultiplier());
      if (src.HasRightComplianceMultiplier())
        dst.RightComplianceMultiplier = PBProperty.Unload(src.GetRightComplianceMultiplier());

      if (src.HasLeftExpiratoryResistanceMultiplier())
        dst.LeftExpiratoryResistanceMultiplier = PBProperty.Unload(src.GetLeftExpiratoryResistanceMultiplier());
      if (src.HasLeftInspiratoryResistanceMultiplier())
        dst.LeftInspiratoryResistanceMultiplier = PBProperty.Unload(src.GetLeftInspiratoryResistanceMultiplier());
      if (src.HasRightExpiratoryResistanceMultiplier())
        dst.RightExpiratoryResistanceMultiplier = PBProperty.Unload(src.GetRightExpiratoryResistanceMultiplier());
      if (src.HasRightInspiratoryResistanceMultiplier())
        dst.RightInspiratoryResistanceMultiplier = PBProperty.Unload(src.GetRightInspiratoryResistanceMultiplier());
      if (src.HasUpperExpiratoryResistanceMultiplier())
        dst.UpperExpiratoryResistanceMultiplier = PBProperty.Unload(src.GetUpperExpiratoryResistanceMultiplier());
      if (src.HasUpperInspiratoryResistanceMultiplier())
        dst.UpperInspiratoryResistanceMultiplier = PBProperty.Unload(src.GetUpperInspiratoryResistanceMultiplier());
      if (src.HasRespirationRateMultiplier())
        dst.RespirationRateMultiplier = PBProperty.Unload(src.GetRespirationRateMultiplier());
      if (src.HasTidalVolumeMultiplier())
        dst.TidalVolumeMultiplier = PBProperty.Unload(src.GetTidalVolumeMultiplier());

      foreach (var s in src.GetLungVolumeIncrements())
      {
        LungVolumeIncrementData d = new LungVolumeIncrementData();
        d.Compartment = (pulse.cdm.bind.eLungCompartment)s.Key;
        d.Increment = PBProperty.Unload(s.Value);
        dst.VolumeIncrement.Add(d);
      }
    }
    #endregion
  }
}
