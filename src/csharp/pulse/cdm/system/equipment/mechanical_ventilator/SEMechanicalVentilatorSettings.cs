/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using System.Collections.Generic;

namespace Pulse.CDM
{
  // TODO Substance Fractions and Concentrations!!!
  public class SEMechanicalVentilatorSettings : SEEquipment
  {
    protected eSwitch connection;
    protected SEScalarVolume connection_volume;
    protected SEScalarVolumePerPressure compliance;
    protected SEScalarFrequency driver_damping_parameter;

    // Expiratory Baseline (Only set one)
    protected SEScalarPressure positive_end_expired_pressure;
    protected SEScalarPressure functional_residual_capacity;

    // Expiration Cyclye (Only set one)
    protected SEScalarVolumePerTime expiration_cycle_flow;
    protected SEScalarPressure expiration_cycle_pressure;
    protected SEScalarTime expiration_cycle_time;
    protected SEScalarVolume expiration_cycle_volume;
    protected eSwitch expiration_cycle_respiratory_model;

    protected SEScalarVolume expiration_limb_volume;
    protected SEScalarPressureTimePerVolume expiration_tube_resistance;
    protected SEScalarPressureTimePerVolume expiration_valve_resistance;
    protected SEScalarVolume expiration_valve_volume;
    protected eDriverWaveform expiration_waveform;
    protected SEScalarTime expiration_waveform_period;

    // Inspiration Limit (Only set one)
    protected SEScalarVolumePerTime inspiration_limit_flow;
    protected SEScalarPressure inspiration_limit_pressure;
    protected SEScalarVolume inspiration_limit_volume;

    protected SEScalarTime inspiration_pause_time;

    // Inspiratory Target (Only set one)
    protected SEScalarPressure peak_inpiratory_pressure;
    protected SEScalarVolumePerTime inspiration_target_flow;

    // Inspiration Machine Trigger (Only set one)
    protected SEScalarTime inspiration_machine_trigger_time;

    // Inspiration Patient Trigger (Only set one)
    protected SEScalarVolumePerTime inspiration_patient_trigger_flow;
    protected SEScalarPressure inspiration_patient_trigger_pressure;
    protected eSwitch inspiration_patient_trigger_respiratory_model;

    protected SEScalarVolume inspiration_limb_volume;
    protected SEScalarPressureTimePerVolume inspiration_tube_resistance;
    protected SEScalarPressureTimePerVolume inspiration_valve_resistance;
    protected SEScalarVolume inspiration_valve_volume;
    protected eDriverWaveform inspiration_waveform;
    protected SEScalarTime inspiration_waveform_period;

    protected SEScalarPressure relief_valve_threshold;
    protected SEScalarVolume y_piece_volume;

    protected List<SESubstanceFraction> fraction_inspired_gases;
    protected List<SESubstanceConcentration> concentration_inspired_aerosols;

    public SEMechanicalVentilatorSettings()
    {
      connection = eSwitch.NullSwitch;
      connection_volume = null;
      compliance = null;
      driver_damping_parameter = null;

      functional_residual_capacity = null;
      expiration_cycle_flow = null;
      expiration_cycle_pressure = null;
      expiration_cycle_time = null;
      expiration_cycle_volume = null;
      expiration_cycle_respiratory_model = eSwitch.NullSwitch;

      expiration_limb_volume = null;
      expiration_tube_resistance = null;
      expiration_valve_resistance = null;
      expiration_valve_volume = null;
      expiration_waveform = eDriverWaveform.NullDriverWaveform;
      expiration_waveform_period = null;

      inspiration_limit_flow = null;
      inspiration_limit_pressure = null;
      inspiration_limit_volume = null;
      inspiration_pause_time = null;
      peak_inpiratory_pressure = null;
      inspiration_target_flow = null;
      inspiration_machine_trigger_time = null;
      inspiration_patient_trigger_flow = null;
      inspiration_patient_trigger_pressure = null;
      inspiration_patient_trigger_respiratory_model = eSwitch.NullSwitch;

      inspiration_limb_volume = null;
      inspiration_tube_resistance = null;
      inspiration_valve_resistance = null;
      inspiration_valve_volume = null;
      inspiration_waveform = eDriverWaveform.NullDriverWaveform;
      inspiration_waveform_period = null;

      relief_valve_threshold = null;
      y_piece_volume = null;

      this.fraction_inspired_gases = new List<SESubstanceFraction>();
      this.concentration_inspired_aerosols = new List<SESubstanceConcentration>();
    }

    public override void Clear()
    {
      base.Clear();
      connection = eSwitch.NullSwitch;
      if (connection_volume != null)
        connection_volume.Invalidate();
      if (compliance != null)
        compliance.Invalidate();
      if (driver_damping_parameter != null)
        driver_damping_parameter.Invalidate();

      if (positive_end_expired_pressure != null)
        positive_end_expired_pressure.Invalidate();
      if (functional_residual_capacity != null)
        functional_residual_capacity.Invalidate();
      if (expiration_cycle_flow != null)
        expiration_cycle_flow.Invalidate();
      if (expiration_cycle_pressure != null)
        expiration_cycle_pressure.Invalidate();
      if (expiration_cycle_time != null)
        expiration_cycle_time.Invalidate();
      if (expiration_cycle_volume != null)
        expiration_cycle_volume.Invalidate();

      if (expiration_limb_volume != null)
        expiration_limb_volume.Invalidate();
      if (expiration_tube_resistance != null)
        expiration_tube_resistance.Invalidate();
      if (expiration_valve_resistance != null)
        expiration_valve_resistance.Invalidate();
      if (expiration_valve_volume != null)
        expiration_valve_volume.Invalidate();
      expiration_waveform = eDriverWaveform.NullDriverWaveform;
      if (expiration_waveform_period != null)
        expiration_waveform_period.Invalidate();

      if (inspiration_limit_flow != null)
        inspiration_limit_flow.Invalidate();
      if (inspiration_limit_pressure != null)
        inspiration_limit_pressure.Invalidate();
      if (inspiration_limit_volume != null)
        inspiration_limit_volume.Invalidate();
      if (inspiration_pause_time != null)
        inspiration_pause_time.Invalidate();
      if (peak_inpiratory_pressure != null)
        peak_inpiratory_pressure.Invalidate();
      if (inspiration_target_flow != null)
        inspiration_target_flow.Invalidate();
      if (inspiration_machine_trigger_time != null)
        inspiration_machine_trigger_time.Invalidate();
      if (inspiration_patient_trigger_flow != null)
        inspiration_patient_trigger_flow.Invalidate();
      if (inspiration_patient_trigger_pressure != null)
        inspiration_patient_trigger_pressure.Invalidate();

      if (inspiration_limb_volume != null)
        inspiration_limb_volume.Invalidate();
      if (inspiration_tube_resistance != null)
        inspiration_tube_resistance.Invalidate();
      if (inspiration_valve_resistance != null)
        inspiration_valve_resistance.Invalidate();
      if (inspiration_valve_volume != null)
        inspiration_valve_volume.Invalidate();
      inspiration_waveform = eDriverWaveform.NullDriverWaveform;
      if (inspiration_waveform_period != null)
        inspiration_waveform_period.Invalidate();


      if (relief_valve_threshold != null)
        relief_valve_threshold.Invalidate();

      if (y_piece_volume != null)
        y_piece_volume.Invalidate();

      fraction_inspired_gases.Clear();
      concentration_inspired_aerosols.Clear();
    }

    public void Copy(SEMechanicalVentilatorSettings from)
    {
      base.Copy(from);
      if (from.connection != eSwitch.NullSwitch)
        this.connection = from.connection;
      if (from.HasConnectionVolume())
        this.GetConnectionVolume().Set(from.GetConnectionVolume());
      if (from.HasCompliance())
        this.GetCompliance().Set(from.GetCompliance());
      if (from.HasDriverDampingParameter())
        this.GetDriverDampingParameter().Set(from.GetDriverDampingParameter());

      if (from.HasPositiveEndExpiratoryPressure())
        this.GetPositiveEndExpiratoryPressure().Set(from.GetPositiveEndExpiratoryPressure());
      if (from.HasFunctionalResidualCapacity())
        this.GetFunctionalResidualCapacity().Set(from.GetFunctionalResidualCapacity());
      if (from.HasExpirationCycleFlow())
        this.GetExpirationCycleFlow().Set(from.GetExpirationCycleFlow());
      if (from.HasExpirationCyclePressure())
        this.GetExpirationCyclePressure().Set(from.GetExpirationCyclePressure());
      if (from.HasExpirationCycleTime())
        this.GetExpirationCycleTime().Set(from.GetExpirationCycleTime());
      if (from.HasExpirationCycleVolume())
        this.GetExpirationCycleVolume().Set(from.GetExpirationCycleVolume());
      if (from.expiration_cycle_respiratory_model != eSwitch.NullSwitch)
        this.expiration_cycle_respiratory_model = from.expiration_cycle_respiratory_model;

      if (from.HasExpirationLimbVolume())
        this.GetExpirationLimbVolume().Set(from.GetExpirationLimbVolume());
      if (from.HasExpirationTubeResistance())
        this.GetExpirationTubeResistance().Set(from.GetExpirationTubeResistance());
      if (from.HasExpirationValveResistance())
        this.GetExpirationValveResistance().Set(from.GetExpirationValveResistance());
      if (from.HasExpirationValveVolume())
        this.GetExpirationValveVolume().Set(from.GetExpirationValveVolume());
      if (from.expiration_waveform != eDriverWaveform.NullDriverWaveform)
        this.expiration_waveform = from.expiration_waveform;
      if (from.HasExpirationWaveformPeriod())
        this.GetExpirationWaveformPeriod().Set(from.GetExpirationWaveformPeriod());

      if (from.HasInspirationLimitFlow())
        this.GetInspirationLimitFlow().Set(from.GetInspirationLimitFlow());
      if (from.HasInspirationLimitPressure())
        this.GetInspirationLimitPressure().Set(from.GetInspirationLimitPressure());
      if (from.HasInspirationLimitVolume())
        this.GetInspirationLimitVolume().Set(from.GetInspirationLimitVolume());
      if (from.HasInspirationPauseTime())
        this.GetInspirationPauseTime().Set(from.GetInspirationPauseTime());
      if (from.HasPeakInspiratoryPressure())
        this.GetPeakInspiratoryPressure().Set(from.GetPeakInspiratoryPressure());
      if (from.HasInspirationTargetFlow())
        this.GetInspirationTargetFlow().Set(from.GetInspirationTargetFlow());
      if (from.HasInspirationMachineTriggerTime())
        this.GetInspirationMachineTriggerTime().Set(from.GetInspirationMachineTriggerTime());
      if (from.HasInspirationPatientTriggerFlow())
        this.GetInspirationPatientTriggerFlow().Set(from.GetInspirationPatientTriggerFlow());
      if (from.HasInspirationPatientTriggerPressure())
        this.GetInspirationPatientTriggerPressure().Set(from.GetInspirationPatientTriggerPressure());
      if (from.inspiration_patient_trigger_respiratory_model != eSwitch.NullSwitch)
        this.inspiration_patient_trigger_respiratory_model = from.inspiration_patient_trigger_respiratory_model;

      if (from.HasInspirationLimbVolume())
        this.GetInspirationLimbVolume().Set(from.GetInspirationLimbVolume());
      if (from.HasInspirationTubeResistance())
        this.GetInspirationTubeResistance().Set(from.GetInspirationTubeResistance());
      if (from.HasInspirationValveResistance())
        this.GetInspirationValveResistance().Set(from.GetInspirationValveResistance());
      if (from.HasInspirationValveVolume())
        this.GetInspirationValveVolume().Set(from.GetInspirationValveVolume());
      if (from.inspiration_waveform != eDriverWaveform.NullDriverWaveform)
        this.inspiration_waveform = from.inspiration_waveform;
      if (from.HasInspirationWaveformPeriod())
        this.GetInspirationWaveformPeriod().Set(from.GetInspirationWaveformPeriod());

      if (from.HasYPieceVolume())
        this.GetYPieceVolume().Set(from.GetYPieceVolume());

      if (from.fraction_inspired_gases != null)
      {
        SESubstanceFraction mine;
        foreach (SESubstanceFraction sf in from.fraction_inspired_gases)
        {
          mine = this.CreateFractionInspiredGas(sf.GetSubstance());
          if (sf.HasFractionAmount())
            mine.GetFractionAmount().Set(sf.GetFractionAmount());
        }
      }

      if (from.concentration_inspired_aerosols != null)
      {
        SESubstanceConcentration mine;
        foreach (SESubstanceConcentration sc in from.concentration_inspired_aerosols)
        {
          mine = this.CreateConcentrationInspiredAerosol(sc.GetSubstance());
          if (sc.HasConcentration())
            mine.GetConcentration().Set(sc.GetConcentration());
        }
      }
    }

    public eSwitch GetConnection()
    {
      return connection;
    }
    public void SetConnection(eSwitch c)
    {
      connection = c;
    }
    public bool HasConnection()
    {
      return connection != eSwitch.NullSwitch;
    }

    public SEScalarVolume GetConnectionVolume()
    {
      if (connection_volume == null)
        connection_volume = new SEScalarVolume();
      return connection_volume;
    }
    public bool HasConnectionVolume()
    {
      return connection_volume == null ? false : connection_volume.IsValid();
    }

    public SEScalarVolumePerPressure GetCompliance()
    {
      if (compliance == null)
        compliance = new SEScalarVolumePerPressure();
      return compliance;
    }
    public bool HasCompliance()
    {
      return compliance == null ? false : compliance.IsValid();
    }

    public SEScalarFrequency GetDriverDampingParameter()
    {
      if (driver_damping_parameter == null)
        driver_damping_parameter = new SEScalarFrequency();
      return driver_damping_parameter;
    }
    public bool HasDriverDampingParameter()
    {
      return driver_damping_parameter == null ? false : driver_damping_parameter.IsValid();
    }

    public SEScalarPressure GetPositiveEndExpiratoryPressure()
    {
      if (positive_end_expired_pressure == null)
        positive_end_expired_pressure = new SEScalarPressure();
      return positive_end_expired_pressure;
    }
    public bool HasPositiveEndExpiratoryPressure()
    {
      return positive_end_expired_pressure == null ? false : positive_end_expired_pressure.IsValid();
    }

    public SEScalarPressure GetFunctionalResidualCapacity()
    {
      if (functional_residual_capacity == null)
        functional_residual_capacity = new SEScalarPressure();
      return functional_residual_capacity;
    }
    public bool HasFunctionalResidualCapacity()
    {
      return functional_residual_capacity == null ? false : functional_residual_capacity.IsValid();
    }

    public SEScalarVolumePerTime GetExpirationCycleFlow()
    {
      if (expiration_cycle_flow == null)
        expiration_cycle_flow = new SEScalarVolumePerTime();
      return expiration_cycle_flow;
    }
    public bool HasExpirationCycleFlow()
    {
      return expiration_cycle_flow == null ? false : expiration_cycle_flow.IsValid();
    }

    public SEScalarPressure GetExpirationCyclePressure()
    {
      if (expiration_cycle_pressure == null)
        expiration_cycle_pressure = new SEScalarPressure();
      return expiration_cycle_pressure;
    }
    public bool HasExpirationCyclePressure()
    {
      return expiration_cycle_pressure == null ? false : expiration_cycle_pressure.IsValid();
    }

    public SEScalarTime GetExpirationCycleTime()
    {
      if (expiration_cycle_time == null)
        expiration_cycle_time = new SEScalarTime();
      return expiration_cycle_time;
    }
    public bool HasExpirationCycleTime()
    {
      return expiration_cycle_time == null ? false : expiration_cycle_time.IsValid();
    }

    public SEScalarVolume GetExpirationCycleVolume()
    {
      if (expiration_cycle_volume == null)
        expiration_cycle_volume = new SEScalarVolume();
      return expiration_cycle_volume;
    }
    public bool HasExpirationCycleVolume()
    {
      return expiration_cycle_volume == null ? false : expiration_cycle_volume.IsValid();
    }

    public eSwitch GetExpirationCycleRespiratoryModel()
    {
      return expiration_cycle_respiratory_model;
    }
    public void SetExpirationCycleRespiratoryModel(eSwitch s)
    {
      expiration_cycle_respiratory_model = s;
    }
    public bool HasExpirationCycleRespiratoryModel()
    {
      return expiration_cycle_respiratory_model != eSwitch.NullSwitch;
    }

    public SEScalarVolume GetExpirationLimbVolume()
    {
      if (expiration_limb_volume == null)
        expiration_limb_volume = new SEScalarVolume();
      return expiration_limb_volume;
    }
    public bool HasExpirationLimbVolume()
    {
      return expiration_limb_volume == null ? false : expiration_limb_volume.IsValid();
    }

    public SEScalarPressureTimePerVolume GetExpirationTubeResistance()
    {
      if (expiration_tube_resistance == null)
        expiration_tube_resistance = new SEScalarPressureTimePerVolume();
      return expiration_tube_resistance;
    }
    public bool HasExpirationTubeResistance()
    {
      return expiration_tube_resistance == null ? false : expiration_tube_resistance.IsValid();
    }

    public SEScalarPressureTimePerVolume GetExpirationValveResistance()
    {
      if (expiration_valve_resistance == null)
        expiration_valve_resistance = new SEScalarPressureTimePerVolume();
      return expiration_valve_resistance;
    }
    public bool HasExpirationValveResistance()
    {
      return expiration_valve_resistance == null ? false : expiration_valve_resistance.IsValid();
    }

    public SEScalarVolume GetExpirationValveVolume()
    {
      if (expiration_valve_volume == null)
        expiration_valve_volume = new SEScalarVolume();
      return expiration_valve_volume;
    }
    public bool HasExpirationValveVolume()
    {
      return expiration_valve_volume == null ? false : expiration_valve_volume.IsValid();
    }

    public eDriverWaveform GetExpirationWaveform()
    {
      return expiration_waveform;
    }
    public void SetExpirationWaveform(eDriverWaveform w)
    {
      expiration_waveform = w;
    }
    public bool HasExpirationWaveform()
    {
      return expiration_waveform != eDriverWaveform.NullDriverWaveform;
    }

    public SEScalarTime GetExpirationWaveformPeriod()
    {
      if (expiration_waveform_period == null)
        expiration_waveform_period = new SEScalarTime();
      return expiration_waveform_period;
    }
    public bool HasExpirationWaveformPeriod()
    {
      return expiration_waveform_period == null ? false : expiration_waveform_period.IsValid();
    }

    public SEScalarVolumePerTime GetInspirationLimitFlow()
    {
      if (inspiration_limit_flow == null)
        inspiration_limit_flow = new SEScalarVolumePerTime();
      return inspiration_limit_flow;
    }
    public bool HasInspirationLimitFlow()
    {
      return inspiration_limit_flow == null ? false : inspiration_limit_flow.IsValid();
    }

    public SEScalarPressure GetInspirationLimitPressure()
    {
      if (inspiration_limit_pressure == null)
        inspiration_limit_pressure = new SEScalarPressure();
      return inspiration_limit_pressure;
    }
    public bool HasInspirationLimitPressure()
    {
      return inspiration_limit_pressure == null ? false : inspiration_limit_pressure.IsValid();
    }

    public SEScalarVolume GetInspirationLimitVolume()
    {
      if (inspiration_limit_volume == null)
        inspiration_limit_volume = new SEScalarVolume();
      return inspiration_limit_volume;
    }
    public bool HasInspirationLimitVolume()
    {
      return inspiration_limit_volume == null ? false : inspiration_limit_volume.IsValid();
    }

    public SEScalarTime GetInspirationPauseTime()
    {
      if (inspiration_pause_time == null)
        inspiration_pause_time = new SEScalarTime();
      return inspiration_pause_time;
    }
    public bool HasInspirationPauseTime()
    {
      return inspiration_pause_time == null ? false : inspiration_pause_time.IsValid();
    }

    public SEScalarPressure GetPeakInspiratoryPressure()
    {
      if (peak_inpiratory_pressure == null)
        peak_inpiratory_pressure = new SEScalarPressure();
      return peak_inpiratory_pressure;
    }
    public bool HasPeakInspiratoryPressure()
    {
      return peak_inpiratory_pressure == null ? false : peak_inpiratory_pressure.IsValid();
    }

    public SEScalarVolumePerTime GetInspirationTargetFlow()
    {
      if (inspiration_target_flow == null)
        inspiration_target_flow = new SEScalarVolumePerTime();
      return inspiration_target_flow;
    }
    public bool HasInspirationTargetFlow()
    {
      return inspiration_target_flow == null ? false : inspiration_target_flow.IsValid();
    }

    public SEScalarTime GetInspirationMachineTriggerTime()
    {
      if (inspiration_machine_trigger_time == null)
        inspiration_machine_trigger_time = new SEScalarTime();
      return inspiration_machine_trigger_time;
    }
    public bool HasInspirationMachineTriggerTime()
    {
      return inspiration_machine_trigger_time == null ? false : inspiration_machine_trigger_time.IsValid();
    }

    public SEScalarVolumePerTime GetInspirationPatientTriggerFlow()
    {
      if (inspiration_patient_trigger_flow == null)
        inspiration_patient_trigger_flow = new SEScalarVolumePerTime();
      return inspiration_patient_trigger_flow;
    }
    public bool HasInspirationPatientTriggerFlow()
    {
      return inspiration_patient_trigger_flow == null ? false : inspiration_patient_trigger_flow.IsValid();
    }

    public SEScalarPressure GetInspirationPatientTriggerPressure()
    {
      if (inspiration_patient_trigger_pressure == null)
        inspiration_patient_trigger_pressure = new SEScalarPressure();
      return inspiration_patient_trigger_pressure;
    }
    public bool HasInspirationPatientTriggerPressure()
    {
      return inspiration_patient_trigger_pressure == null ? false : inspiration_patient_trigger_pressure.IsValid();
    }

    public eSwitch GetInspirationPatientTriggerRespiratoryModel()
    {
      return inspiration_patient_trigger_respiratory_model;
    }
    public void SetInspirationPatientTriggerRespiratoryModel(eSwitch s)
    {
      inspiration_patient_trigger_respiratory_model = s;
    }
    public bool HasInspirationPatientTriggerRespiratoryModel()
    {
      return inspiration_patient_trigger_respiratory_model != eSwitch.NullSwitch;
    }

    public SEScalarVolume GetInspirationLimbVolume()
    {
      if (inspiration_limb_volume == null)
        inspiration_limb_volume = new SEScalarVolume();
      return inspiration_limb_volume;
    }
    public bool HasInspirationLimbVolume()
    {
      return inspiration_limb_volume == null ? false : inspiration_limb_volume.IsValid();
    }

    public SEScalarPressureTimePerVolume GetInspirationTubeResistance()
    {
      if (inspiration_tube_resistance == null)
        inspiration_tube_resistance = new SEScalarPressureTimePerVolume();
      return inspiration_tube_resistance;
    }
    public bool HasInspirationTubeResistance()
    {
      return inspiration_tube_resistance == null ? false : inspiration_tube_resistance.IsValid();
    }

    public SEScalarPressureTimePerVolume GetInspirationValveResistance()
    {
      if (inspiration_valve_resistance == null)
        inspiration_valve_resistance = new SEScalarPressureTimePerVolume();
      return inspiration_valve_resistance;
    }
    public bool HasInspirationValveResistance()
    {
      return inspiration_valve_resistance == null ? false : inspiration_valve_resistance.IsValid();
    }

    public SEScalarVolume GetInspirationValveVolume()
    {
      if (inspiration_valve_volume == null)
        inspiration_valve_volume = new SEScalarVolume();
      return inspiration_valve_volume;
    }
    public bool HasInspirationValveVolume()
    {
      return inspiration_valve_volume == null ? false : inspiration_valve_volume.IsValid();
    }

    public eDriverWaveform GetInspirationWaveform()
    {
      return inspiration_waveform;
    }
    public void SetInspirationWaveform(eDriverWaveform w)
    {
      inspiration_waveform = w;
    }
    public bool HasInspirationWaveform()
    {
      return inspiration_waveform != eDriverWaveform.NullDriverWaveform;
    }

    public SEScalarTime GetInspirationWaveformPeriod()
    {
      if (inspiration_waveform_period == null)
        inspiration_waveform_period = new SEScalarTime();
      return inspiration_waveform_period;
    }
    public bool HasInspirationWaveformPeriod()
    {
      return inspiration_waveform_period == null ? false : inspiration_waveform_period.IsValid();
    }

    public SEScalarPressure GetReliefValveThreshold()
    {
      if (relief_valve_threshold == null)
        relief_valve_threshold = new SEScalarPressure();
      return relief_valve_threshold;
    }
    public bool HasReliefValveThreshold()
    {
      return relief_valve_threshold == null ? false : relief_valve_threshold.IsValid();
    }

    public SEScalarVolume GetYPieceVolume()
    {
      if (y_piece_volume == null)
        y_piece_volume = new SEScalarVolume();
      return y_piece_volume;
    }
    public bool HasYPieceVolume()
    {
      return y_piece_volume == null ? false : y_piece_volume.IsValid();
    }

    public SESubstanceFraction CreateFractionInspiredGas(string substance)
    {
      return GetFractionInspiredGas(substance);
    }
    public SESubstanceFraction GetFractionInspiredGas(string substance)
    {
      foreach (SESubstanceFraction sf in fraction_inspired_gases)
      {
        if (sf.GetSubstance() == substance)
        {
          return sf;
        }
      }
      SESubstanceFraction nsf = new SESubstanceFraction(substance);
      fraction_inspired_gases.Add(nsf);
      return nsf;
    }
    public bool HasFractionInspiredGas()
    {
      return fraction_inspired_gases.Count != 0;
    }
    public bool HasFractionInspiredGas(string substance)
    {
      foreach (SESubstanceFraction sf in fraction_inspired_gases)
      {
        if (sf.GetSubstance() == substance)
        {
          return true;
        }
      }
      return false;
    }
    public List<SESubstanceFraction> GetFractionInspiredGases()
    {
      return fraction_inspired_gases;
    }
    public void RemoveFractionInspiredGas(string substance)
    {
      foreach (SESubstanceFraction sf in fraction_inspired_gases)
      {
        if (sf.GetSubstance() == substance)
        {
          fraction_inspired_gases.Remove(sf);
          return;
        }
      }
    }
    public void RemoveFractionInspiredGases()
    {
      fraction_inspired_gases.Clear();
    }

    public SESubstanceConcentration CreateConcentrationInspiredAerosol(string substance)
    {
      return GetConcentrationInspiredAerosol(substance);
    }
    public SESubstanceConcentration GetConcentrationInspiredAerosol(string substance)
    {
      foreach (SESubstanceConcentration sc in concentration_inspired_aerosols)
      {
        if (sc.GetSubstance() == substance)
        {
          return sc;
        }
      }
      SESubstanceConcentration nsc = new SESubstanceConcentration(substance);
      concentration_inspired_aerosols.Add(nsc);
      return nsc;
    }
    public bool HasConcentrationInspiredAerosol()
    {
      return concentration_inspired_aerosols.Count != 0;
    }
    public bool HasConcentrationInspiredAerosol(string substance)
    {
      foreach (SESubstanceConcentration sc in concentration_inspired_aerosols)
      {
        if (sc.GetSubstance() == substance)
        {
          return true;
        }
      }
      return false;
    }
    public List<SESubstanceConcentration> GetConcentrationInspiredAerosols()
    {
      return concentration_inspired_aerosols;
    }
    public void RemoveConcentrationInspiredAerosol(string substance)
    {
      foreach (SESubstanceConcentration sc in concentration_inspired_aerosols)
      {
        if (sc.GetSubstance() == substance)
        {
          concentration_inspired_aerosols.Remove(sc);
          return;
        }
      }
    }
    public void RemoveConcentrationInspiredAerosols()
    {
      concentration_inspired_aerosols.Clear();
    }

    public override string ToString()
    {
      string str = "Mechanical Ventilator Settings"
      + "\n\tConnection: " + (HasConnection() ? eEnum.Name(GetConnection()) : "NotProvided")
      + "\n\tPositiveEndExpiratoryPressure: " + (HasPositiveEndExpiratoryPressure() ? GetPositiveEndExpiratoryPressure().ToString() : "Not Provided")
      + "\n\tFunctionalResidualCapacity: " + (HasFunctionalResidualCapacity() ? GetFunctionalResidualCapacity().ToString() : "Not Provided")
      + "\n\tExpirationCycleFlow: " + (HasExpirationCycleFlow() ? GetExpirationCycleFlow().ToString() : "Not Provided")
      + "\n\tExpirationCyclePressure: " + (HasExpirationCyclePressure() ? GetExpirationCyclePressure().ToString() : "Not Provided")
      + "\n\tExpirationCycleTime: " + (HasExpirationCycleTime() ? GetExpirationCycleTime().ToString() : "Not Provided")
      + "\n\tExpirationCycleVolume: " + (HasExpirationCycleVolume() ? GetExpirationCycleVolume().ToString() : "Not Provided")
      + "\n\tExpirationCycleRespiratoryModel: " + (HasExpirationCycleRespiratoryModel() ? eEnum.Name(GetExpirationCycleRespiratoryModel()) : "NotProvided")
      + "\n\tExpirationTubeResistance: " + (HasExpirationTubeResistance() ? GetExpirationTubeResistance().ToString() : "Not Provided")
      + "\n\tExpirationValveResistance: " + (HasExpirationValveResistance() ? GetExpirationValveResistance().ToString() : "Not Provided")
      + "\n\tExpirationValveVolume: " + (HasExpirationValveVolume() ? GetExpirationValveVolume().ToString() : "Not Provided")
      + "\n\tExpirationWaveform: " + (HasExpirationWaveform() ? eEnum.Name(expiration_waveform) : "Not Provided")
      + "\n\tExpirationWaveformPeriod: " + (HasExpirationWaveformPeriod() ? GetExpirationWaveformPeriod().ToString() : "Not Provided")
      + "\n\tInspirationLimitFlow: " + (HasInspirationLimitFlow() ? GetInspirationLimitFlow().ToString() : "Not Provided")
      + "\n\tInspirationLimitPressure: " + (HasInspirationLimitPressure() ? GetInspirationLimitPressure().ToString() : "Not Provided")
      + "\n\tInspirationLimitVolume: " + (HasInspirationLimitVolume() ? GetInspirationLimitVolume().ToString() : "Not Provided")
      + "\n\tInspirationPauseTime: " + (HasInspirationPauseTime() ? GetInspirationPauseTime().ToString() : "Not Provided")
      + "\n\tPeakInspiratoryPressure: " + (HasPeakInspiratoryPressure() ? GetPeakInspiratoryPressure().ToString() : "Not Provided")
      + "\n\tInspirationTargetFlow: " + (HasInspirationTargetFlow() ? GetInspirationTargetFlow().ToString() : "Not Provided")
      + "\n\tInspirationMachineTriggerTime: " + (HasInspirationMachineTriggerTime() ? GetInspirationMachineTriggerTime().ToString() : "Not Provided")
      + "\n\tInspirationPatientTriggerFlow: " + (HasInspirationPatientTriggerFlow() ? GetInspirationPatientTriggerFlow().ToString() : "Not Provided")
      + "\n\tInspirationPatientTriggerPressure: " + (HasInspirationPatientTriggerPressure() ? GetInspirationPatientTriggerPressure().ToString() : "Not Provided")
      + "\n\tInspirationPatientTriggerRespiratoryModel: " + (HasInspirationPatientTriggerRespiratoryModel() ? eEnum.Name(GetInspirationPatientTriggerRespiratoryModel()) : "NotProvided")
      + "\n\tInspirationTubeResistance: " + (HasInspirationTubeResistance() ? GetInspirationTubeResistance().ToString() : "Not Provided")
      + "\n\tInspirationValveResistance: " + (HasInspirationValveResistance() ? GetInspirationValveResistance().ToString() : "Not Provided")
      + "\n\tInspirationValveVolume: " + (HasInspirationValveVolume() ? GetInspirationValveVolume().ToString() : "Not Provided")
      + "\n\tInspirationWaveform: " + (HasInspirationWaveform() ? eEnum.Name(inspiration_waveform) : "Not Provided")
      + "\n\tInspirationWaveformPeriod: " + (HasInspirationWaveformPeriod() ? GetInspirationWaveformPeriod().ToString() : "Not Provided");
      foreach (SESubstanceFraction sf in this.fraction_inspired_gases)
        str += "\n\t" + sf.GetSubstance();
      foreach (SESubstanceConcentration sc in this.concentration_inspired_aerosols)
        str += "\n\t" + sc.GetSubstance();

      return str;
    }
  }
}