/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

namespace Pulse.CDM
{
  public class PBMechanicalVentilator
  {
    #region SEMechanicalVentilator
    public static void Load(pulse.cdm.bind.MechanicalVentilatorSettingsData src, SEMechanicalVentilatorSettings dst/*, SESubstanceManager subMgr*/)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.MechanicalVentilatorSettingsData src, SEMechanicalVentilatorSettings dst/*, SESubstanceManager subMgr*/)
    {
      dst.Clear();
      dst.SetConnection((eSwitch)src.Connection);
      if (src.ConnectionVolume != null)
        PBProperty.Load(src.ConnectionVolume, dst.GetConnectionVolume());
      if (src.Compliance != null)
        PBProperty.Load(src.Compliance, dst.GetCompliance());
      if (src.DriverDampingParameter != null)
        PBProperty.Load(src.DriverDampingParameter, dst.GetDriverDampingParameter());

      if (src.PositiveEndExpiratoryPressure != null)
        PBProperty.Load(src.PositiveEndExpiratoryPressure, dst.GetPositiveEndExpiratoryPressure());
      else if (src.FunctionalResidualCapacity != null)
        PBProperty.Load(src.FunctionalResidualCapacity, dst.GetFunctionalResidualCapacity());

      if (src.ExpirationCycleFlow != null)
        PBProperty.Load(src.ExpirationCycleFlow, dst.GetExpirationCycleFlow());
      else if (src.ExpirationCyclePressure != null)
        PBProperty.Load(src.ExpirationCyclePressure, dst.GetExpirationCyclePressure());
      else if (src.ExpirationCycleTime != null)
        PBProperty.Load(src.ExpirationCycleTime, dst.GetExpirationCycleTime());
      else if (src.ExpirationCycleVolume != null)
        PBProperty.Load(src.ExpirationCycleVolume, dst.GetExpirationCycleVolume());
      else
        dst.SetExpirationCycleRespiratoryModel((eSwitch)src.ExpirationCycleRespiratoryModel);

      if (src.ExpirationLimbVolume != null)
        PBProperty.Load(src.ExpirationLimbVolume, dst.GetExpirationLimbVolume());
      if (src.ExpirationTubeResistance != null)
        PBProperty.Load(src.ExpirationTubeResistance, dst.GetExpirationTubeResistance());
      if (src.ExpirationValveResistance != null)
        PBProperty.Load(src.ExpirationValveResistance, dst.GetExpirationValveResistance());
      if (src.ExpirationValveVolume != null)
        PBProperty.Load(src.ExpirationValveVolume, dst.GetExpirationValveVolume());
      dst.SetExpirationWaveform((eDriverWaveform)src.ExpirationWaveform);
      if (src.ExpirationWaveformPeriod != null)
        PBProperty.Load(src.ExpirationWaveformPeriod, dst.GetExpirationWaveformPeriod());

      if (src.InspirationLimitFlow != null)
        PBProperty.Load(src.InspirationLimitFlow, dst.GetInspirationLimitFlow());
      else if (src.InspirationLimitPressure != null)
        PBProperty.Load(src.InspirationLimitPressure, dst.GetInspirationLimitPressure());
      else if (src.InspirationLimitVolume != null)
        PBProperty.Load(src.InspirationLimitVolume, dst.GetInspirationLimitVolume());
      
      if (src.InspirationPauseTime != null)
        PBProperty.Load(src.InspirationPauseTime, dst.GetInspirationPauseTime());

      if (src.PeakInspiratoryPressure != null)
        PBProperty.Load(src.PeakInspiratoryPressure, dst.GetPeakInspiratoryPressure());
      else if (src.InspirationTargetFlow != null)
        PBProperty.Load(src.InspirationTargetFlow, dst.GetInspirationTargetFlow());

      if (src.InspirationMachineTriggerTime != null)
        PBProperty.Load(src.InspirationMachineTriggerTime, dst.GetInspirationMachineTriggerTime());

      if (src.InspirationPatientTriggerFlow != null)
        PBProperty.Load(src.InspirationPatientTriggerFlow, dst.GetInspirationPatientTriggerFlow());
      else if (src.InspirationPatientTriggerPressure != null)
        PBProperty.Load(src.InspirationPatientTriggerPressure, dst.GetInspirationPatientTriggerPressure());
      else
        dst.SetInspirationPatientTriggerRespiratoryModel((eSwitch)src.InspirationPatientTriggerRespiratoryModel);

      if (src.InspirationLimbVolume != null)
        PBProperty.Load(src.InspirationLimbVolume, dst.GetInspirationLimbVolume());
      if (src.InspirationTubeResistance != null)
        PBProperty.Load(src.InspirationTubeResistance, dst.GetInspirationTubeResistance());
      if (src.InspirationValveResistance != null)
        PBProperty.Load(src.InspirationValveResistance, dst.GetInspirationValveResistance());
      if (src.InspirationValveVolume != null)
        PBProperty.Load(src.InspirationValveVolume, dst.GetInspirationValveVolume());
      dst.SetInspirationWaveform((eDriverWaveform)src.InspirationWaveform);
      if (src.InspirationWaveformPeriod != null)
        PBProperty.Load(src.InspirationWaveformPeriod, dst.GetInspirationWaveformPeriod());

      if (src.ReliefValveThreshold != null)
        PBProperty.Load(src.ReliefValveThreshold, dst.GetReliefValveThreshold());
      if (src.YPieceVolume != null)
        PBProperty.Load(src.YPieceVolume, dst.GetYPieceVolume());

      foreach (pulse.cdm.bind.SubstanceFractionData sf in src.FractionInspiredGas)
      {
        PBProperty.Load(sf.Amount, dst.GetFractionInspiredGas(sf.Name).GetFractionAmount());
      }

      foreach (pulse.cdm.bind.SubstanceConcentrationData sc in src.ConcentrationInspiredAerosol)
      {
        PBProperty.Load(sc.Concentration, dst.GetConcentrationInspiredAerosol(sc.Name).GetConcentration());
      }
    }

    public static pulse.cdm.bind.MechanicalVentilatorSettingsData Unload(SEMechanicalVentilatorSettings src)
    {
      pulse.cdm.bind.MechanicalVentilatorSettingsData dst = new pulse.cdm.bind.MechanicalVentilatorSettingsData();
      Unload(src, dst);
      return dst;
    }
    protected static void Unload(SEMechanicalVentilatorSettings src, pulse.cdm.bind.MechanicalVentilatorSettingsData dst)
    {
      dst.Connection = (pulse.cdm.bind.eSwitch)(int)src.GetConnection();
      if (src.HasConnectionVolume())
        dst.ConnectionVolume = PBProperty.Unload(src.GetConnectionVolume());
      if (src.HasCompliance())
        dst.Compliance = PBProperty.Unload(src.GetCompliance());
      if (src.HasDriverDampingParameter())
        dst.DriverDampingParameter = PBProperty.Unload(src.GetDriverDampingParameter());

      if (src.HasPositiveEndExpiratoryPressure())
        dst.PositiveEndExpiratoryPressure = PBProperty.Unload(src.GetPositiveEndExpiratoryPressure());
      else if (src.HasFunctionalResidualCapacity())
        dst.FunctionalResidualCapacity = PBProperty.Unload(src.GetFunctionalResidualCapacity());

      if (src.HasExpirationCycleFlow())
        dst.ExpirationCycleFlow = PBProperty.Unload(src.GetExpirationCycleFlow());
      else if (src.HasExpirationCyclePressure())
        dst.ExpirationCyclePressure = PBProperty.Unload(src.GetExpirationCyclePressure());
      else if (src.HasExpirationCycleTime())
        dst.ExpirationCycleTime = PBProperty.Unload(src.GetExpirationCycleTime());
      else if (src.HasExpirationCycleVolume())
        dst.ExpirationCycleVolume = PBProperty.Unload(src.GetExpirationCycleVolume());
      else
        dst.ExpirationCycleRespiratoryModel = (pulse.cdm.bind.eSwitch)(int)src.GetExpirationCycleRespiratoryModel();

      if (src.HasExpirationLimbVolume())
        dst.ExpirationLimbVolume = PBProperty.Unload(src.GetExpirationLimbVolume());
      if (src.HasExpirationTubeResistance())
        dst.ExpirationTubeResistance = PBProperty.Unload(src.GetExpirationTubeResistance());
      if (src.HasExpirationValveResistance())
        dst.ExpirationValveResistance = PBProperty.Unload(src.GetExpirationValveResistance());
      if (src.HasExpirationValveVolume())
        dst.ExpirationValveVolume = PBProperty.Unload(src.GetExpirationValveVolume());
      dst.ExpirationWaveform = (pulse.cdm.bind.eDriverWaveform)(int)src.GetExpirationWaveform();
      if (src.HasExpirationWaveformPeriod())
        dst.ExpirationWaveformPeriod = PBProperty.Unload(src.GetExpirationWaveformPeriod());

      if (src.HasInspirationLimitFlow())
        dst.InspirationLimitFlow = PBProperty.Unload(src.GetInspirationLimitFlow());
      else if (src.HasInspirationLimitPressure())
        dst.InspirationLimitPressure = PBProperty.Unload(src.GetInspirationLimitPressure());
      else if (src.HasInspirationLimitVolume())
        dst.InspirationLimitVolume = PBProperty.Unload(src.GetInspirationLimitVolume());

      if (src.HasInspirationPauseTime())
        dst.InspirationPauseTime = PBProperty.Unload(src.GetInspirationPauseTime());

      if (src.HasPeakInspiratoryPressure())
        dst.PeakInspiratoryPressure = PBProperty.Unload(src.GetPeakInspiratoryPressure());
      else if (src.HasInspirationTargetFlow())
        dst.InspirationTargetFlow = PBProperty.Unload(src.GetInspirationTargetFlow());

      if (src.HasInspirationMachineTriggerTime())
        dst.InspirationMachineTriggerTime = PBProperty.Unload(src.GetInspirationMachineTriggerTime());

      if (src.HasInspirationPatientTriggerFlow())
        dst.InspirationPatientTriggerFlow = PBProperty.Unload(src.GetInspirationPatientTriggerFlow());
      else if (src.HasInspirationPatientTriggerPressure())
        dst.InspirationPatientTriggerPressure = PBProperty.Unload(src.GetInspirationPatientTriggerPressure());
      else
        dst.InspirationPatientTriggerRespiratoryModel = (pulse.cdm.bind.eSwitch)(int)src.GetInspirationPatientTriggerRespiratoryModel();

      if (src.HasInspirationLimbVolume())
        dst.InspirationLimbVolume = PBProperty.Unload(src.GetInspirationLimbVolume());
      if (src.HasInspirationTubeResistance())
        dst.InspirationTubeResistance = PBProperty.Unload(src.GetInspirationTubeResistance());
      if (src.HasInspirationValveResistance())
        dst.InspirationValveResistance = PBProperty.Unload(src.GetInspirationValveResistance());
      if (src.HasInspirationValveVolume())
        dst.InspirationValveVolume = PBProperty.Unload(src.GetInspirationValveVolume());
      dst.InspirationWaveform = (pulse.cdm.bind.eDriverWaveform)(int)src.GetInspirationWaveform();
      if (src.HasInspirationWaveformPeriod())
        dst.InspirationWaveformPeriod = PBProperty.Unload(src.GetInspirationWaveformPeriod());

      if (src.HasReliefValveThreshold())
        dst.ReliefValveThreshold = PBProperty.Unload(src.GetReliefValveThreshold());
      if (src.HasYPieceVolume())
        dst.YPieceVolume = PBProperty.Unload(src.GetYPieceVolume());

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
