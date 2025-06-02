/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using Pulse.CDM;
using System.Collections.Generic;

namespace Pulse.CDM
{
  public class SEMechanicalVentilatorAlarms
  {
    protected SEScalarTime apnea_time_threshold;
    protected SEScalarPressure auto_positive_end_expiratory_pressure_threshold;
    protected SEScalar0To1 circuit_leak_threshold;
    protected SEScalarPressure high_end_tidal_carbon_dioxide_threshold;
    protected SEScalarVolumePerTime high_minute_ventilation_threshold;
    protected SEScalar0To1 high_oxygen_saturation_threshold;
    protected SEScalarPressure high_positive_end_expiratory_pressure_threshold;
    protected eSwitch high_pressure_cycle_option;
    protected SEScalarPressure high_pressure_threshold;
    protected SEScalarFrequency high_respiratory_rate_threshold;
    protected SEScalarVolume high_tidal_volume_threshold;
    protected SEScalarPressure low_end_tidal_carbon_dioxide_threshold;
    protected SEScalarVolumePerTime low_minute_ventilation_threshold;
    protected SEScalar0To1 low_oxygen_saturation_threshold;
    protected SEScalarPressure low_positive_end_expiratory_pressure_threshold;
    protected SEScalarPressure low_pressure_threshold;
    protected SEScalarVolume low_tidal_volume_threshold;
    protected SEScalar0To1 oxygen_supply_failure_threshold;

    public SEMechanicalVentilatorAlarms()
    {
      apnea_time_threshold = null;
      auto_positive_end_expiratory_pressure_threshold = null;
      circuit_leak_threshold = null;
      high_end_tidal_carbon_dioxide_threshold = null;
      high_minute_ventilation_threshold = null;
      high_oxygen_saturation_threshold = null;
      high_positive_end_expiratory_pressure_threshold = null;
      high_pressure_cycle_option = eSwitch.Off;
      high_pressure_threshold = null;
      high_respiratory_rate_threshold = null;
      high_tidal_volume_threshold = null;
      low_end_tidal_carbon_dioxide_threshold = null;
      low_minute_ventilation_threshold = null;
      low_oxygen_saturation_threshold = null;
      low_positive_end_expiratory_pressure_threshold = null;
      low_pressure_threshold = null;
      low_tidal_volume_threshold = null;
      oxygen_supply_failure_threshold = null;
    }

    public void Clear()
    {
      if (apnea_time_threshold != null)
        apnea_time_threshold.Invalidate();
      if (auto_positive_end_expiratory_pressure_threshold != null)
        auto_positive_end_expiratory_pressure_threshold.Invalidate();
      if (circuit_leak_threshold != null)
        circuit_leak_threshold.Invalidate();
      if (high_end_tidal_carbon_dioxide_threshold != null)
        high_end_tidal_carbon_dioxide_threshold.Invalidate();
      if (high_minute_ventilation_threshold != null)
        high_minute_ventilation_threshold.Invalidate();
      if (high_oxygen_saturation_threshold != null)
        high_oxygen_saturation_threshold.Invalidate();
      if (high_positive_end_expiratory_pressure_threshold != null)
        high_positive_end_expiratory_pressure_threshold.Invalidate();
      high_pressure_cycle_option = eSwitch.NullSwitch;
      if (high_pressure_threshold != null)
        high_pressure_threshold.Invalidate();
      if (high_respiratory_rate_threshold != null)
        high_respiratory_rate_threshold.Invalidate();
      if (high_tidal_volume_threshold != null)
        high_tidal_volume_threshold.Invalidate();
      if (low_end_tidal_carbon_dioxide_threshold != null)
        low_end_tidal_carbon_dioxide_threshold.Invalidate();
      if (low_minute_ventilation_threshold != null)
        low_minute_ventilation_threshold.Invalidate();
      if (low_oxygen_saturation_threshold != null)
        low_oxygen_saturation_threshold.Invalidate();
      if (low_positive_end_expiratory_pressure_threshold != null)
        low_positive_end_expiratory_pressure_threshold.Invalidate();
      if (low_pressure_threshold != null)
        low_pressure_threshold.Invalidate();
      if (low_tidal_volume_threshold != null)
        low_tidal_volume_threshold.Invalidate();
      if (oxygen_supply_failure_threshold != null)
        oxygen_supply_failure_threshold.Invalidate();
    }

    public void Copy(SEMechanicalVentilatorAlarms from)
    {
      this.Clear();
      if (from.HasApneaTimeThreshold())
        this.GetApneaTimeThreshold().Set(from.GetApneaTimeThreshold());
      if (from.HasAutoPositiveEndExpiratoryPressureThreshold())
        this.GetAutoPositiveEndExpiratoryPressureThreshold().Set(from.GetAutoPositiveEndExpiratoryPressureThreshold());
      if (from.HasCircuitLeakThreshold())
        this.GetCircuitLeakThreshold().Set(from.GetCircuitLeakThreshold());
      if (from.HasHighEndTidalCarbonDioxideThreshold())
        this.GetHighEndTidalCarbonDioxideThreshold().Set(from.GetHighEndTidalCarbonDioxideThreshold());
      if (from.HasHighMinuteVentilationThreshold())
        this.GetHighMinuteVentilationThreshold().Set(from.GetHighMinuteVentilationThreshold());
      if (from.HasHighOxygenSaturationThreshold())
        this.GetHighOxygenSaturationThreshold().Set(from.GetHighOxygenSaturationThreshold());
      if (from.HasHighPositiveEndExpiratoryPressureThreshold())
        this.GetHighPositiveEndExpiratoryPressureThreshold().Set(from.GetHighPositiveEndExpiratoryPressureThreshold());
      if (from.high_pressure_cycle_option != eSwitch.NullSwitch)
        this.high_pressure_cycle_option = from.high_pressure_cycle_option;
      if (from.HasHighPressureThreshold())
        this.GetHighPressureThreshold().Set(from.GetHighPressureThreshold());
      if (from.HasHighRespiratoryRateThreshold())
        this.GetHighRespiratoryRateThreshold().Set(from.GetHighRespiratoryRateThreshold());
      if (from.HasHighTidalVolumeThreshold())
        this.GetHighTidalVolumeThreshold().Set(from.GetHighTidalVolumeThreshold());
      if (from.HasLowEndTidalCarbonDioxideThreshold())
        this.GetLowEndTidalCarbonDioxideThreshold().Set(from.GetLowEndTidalCarbonDioxideThreshold());
      if (from.HasLowMinuteVentilationThreshold())
        this.GetLowMinuteVentilationThreshold().Set(from.GetLowMinuteVentilationThreshold());
      if (from.HasLowOxygenSaturationThreshold())
        this.GetLowOxygenSaturationThreshold().Set(from.GetLowOxygenSaturationThreshold());
      if (from.HasLowPositiveEndExpiratoryPressureThreshold())
        this.GetLowPositiveEndExpiratoryPressureThreshold().Set(from.GetLowPositiveEndExpiratoryPressureThreshold());
      if (from.HasLowPressureThreshold())
        this.GetLowPressureThreshold().Set(from.GetLowPressureThreshold());
      if (from.HasLowTidalVolumeThreshold())
        this.GetLowTidalVolumeThreshold().Set(from.GetLowTidalVolumeThreshold());
      if (from.HasOxygenSupplyFailureThreshold())
        this.GetOxygenSupplyFailureThreshold().Set(from.GetOxygenSupplyFailureThreshold());
    }

    public bool HasApneaTimeThreshold()
    {
      return apnea_time_threshold == null ? false : apnea_time_threshold.IsValid();
    }
    public SEScalarTime GetApneaTimeThreshold()
    {
      if (apnea_time_threshold == null)
        apnea_time_threshold = new SEScalarTime();
      return apnea_time_threshold;
    }

    public bool HasAutoPositiveEndExpiratoryPressureThreshold()
    {
      return auto_positive_end_expiratory_pressure_threshold == null ? false : auto_positive_end_expiratory_pressure_threshold.IsValid();
    }
    public SEScalarPressure GetAutoPositiveEndExpiratoryPressureThreshold()
    {
      if (auto_positive_end_expiratory_pressure_threshold == null)
        auto_positive_end_expiratory_pressure_threshold = new SEScalarPressure();
      return auto_positive_end_expiratory_pressure_threshold;
    }

    public bool HasCircuitLeakThreshold()
    {
      return circuit_leak_threshold == null ? false : circuit_leak_threshold.IsValid();
    }
    public SEScalar0To1 GetCircuitLeakThreshold()
    {
      if (circuit_leak_threshold == null)
        circuit_leak_threshold = new SEScalar0To1();
      return circuit_leak_threshold;
    }

    public bool HasHighEndTidalCarbonDioxideThreshold()
    {
      return high_end_tidal_carbon_dioxide_threshold == null ? false : high_end_tidal_carbon_dioxide_threshold.IsValid();
    }
    public SEScalarPressure GetHighEndTidalCarbonDioxideThreshold()
    {
      if (high_end_tidal_carbon_dioxide_threshold == null)
        high_end_tidal_carbon_dioxide_threshold = new SEScalarPressure();
      return high_end_tidal_carbon_dioxide_threshold;
    }

    public bool HasHighMinuteVentilationThreshold()
    {
      return high_minute_ventilation_threshold == null ? false : high_minute_ventilation_threshold.IsValid();
    }
    public SEScalarVolumePerTime GetHighMinuteVentilationThreshold()
    {
      if (high_minute_ventilation_threshold == null)
        high_minute_ventilation_threshold = new SEScalarVolumePerTime();
      return high_minute_ventilation_threshold;
    }

    public bool HasHighOxygenSaturationThreshold()
    {
      return high_oxygen_saturation_threshold == null ? false : high_oxygen_saturation_threshold.IsValid();
    }
    public SEScalar0To1 GetHighOxygenSaturationThreshold()
    {
      if (high_oxygen_saturation_threshold == null)
        high_oxygen_saturation_threshold = new SEScalar0To1();
      return high_oxygen_saturation_threshold;
    }

    public bool HasHighPositiveEndExpiratoryPressureThreshold()
    {
      return high_positive_end_expiratory_pressure_threshold == null ? false : high_positive_end_expiratory_pressure_threshold.IsValid();
    }
    public SEScalarPressure GetHighPositiveEndExpiratoryPressureThreshold()
    {
      if (high_positive_end_expiratory_pressure_threshold == null)
        high_positive_end_expiratory_pressure_threshold = new SEScalarPressure();
      return high_positive_end_expiratory_pressure_threshold;
    }

    public bool HasHighPressureCycleOption()
    {
      return high_pressure_cycle_option != eSwitch.NullSwitch;
    }
    public eSwitch GetHighPressureCycleOption()
    {
      return high_pressure_cycle_option;
    }
    public void SetHighPressureCycleOption(eSwitch option)
    {
      high_pressure_cycle_option = (option == eSwitch.NullSwitch) ? eSwitch.Off : option;
    }

    public bool HasHighPressureThreshold()
    {
      return high_pressure_threshold == null ? false : high_pressure_threshold.IsValid();
    }
    public SEScalarPressure GetHighPressureThreshold()
    {
      if (high_pressure_threshold == null)
        high_pressure_threshold = new SEScalarPressure();
      return high_pressure_threshold;
    }

    public bool HasHighRespiratoryRateThreshold()
    {
      return high_respiratory_rate_threshold == null ? false : high_respiratory_rate_threshold.IsValid();
    }
    public SEScalarFrequency GetHighRespiratoryRateThreshold()
    {
      if (high_respiratory_rate_threshold == null)
        high_respiratory_rate_threshold = new SEScalarFrequency();
      return high_respiratory_rate_threshold;
    }

    public bool HasHighTidalVolumeThreshold()
    {
      return high_tidal_volume_threshold == null ? false : high_tidal_volume_threshold.IsValid();
    }
    public SEScalarVolume GetHighTidalVolumeThreshold()
    {
      if (high_tidal_volume_threshold == null)
        high_tidal_volume_threshold = new SEScalarVolume();
      return high_tidal_volume_threshold;
    }

    public bool HasLowEndTidalCarbonDioxideThreshold()
    {
      return low_end_tidal_carbon_dioxide_threshold == null ? false : low_end_tidal_carbon_dioxide_threshold.IsValid();
    }
    public SEScalarPressure GetLowEndTidalCarbonDioxideThreshold()
    {
      if (low_end_tidal_carbon_dioxide_threshold == null)
        low_end_tidal_carbon_dioxide_threshold = new SEScalarPressure();
      return low_end_tidal_carbon_dioxide_threshold;
    }

    public bool HasLowMinuteVentilationThreshold()
    {
      return low_minute_ventilation_threshold == null ? false : low_minute_ventilation_threshold.IsValid();
    }
    public SEScalarVolumePerTime GetLowMinuteVentilationThreshold()
    {
      if (low_minute_ventilation_threshold == null)
        low_minute_ventilation_threshold = new SEScalarVolumePerTime();
      return low_minute_ventilation_threshold;
    }

    public bool HasLowOxygenSaturationThreshold()
    {
      return low_oxygen_saturation_threshold == null ? false : low_oxygen_saturation_threshold.IsValid();
    }
    public SEScalar0To1 GetLowOxygenSaturationThreshold()
    {
      if (low_oxygen_saturation_threshold == null)
        low_oxygen_saturation_threshold = new SEScalar0To1();
      return low_oxygen_saturation_threshold;
    }

    public bool HasLowPositiveEndExpiratoryPressureThreshold()
    {
      return low_positive_end_expiratory_pressure_threshold == null ? false : low_positive_end_expiratory_pressure_threshold.IsValid();
    }
    public SEScalarPressure GetLowPositiveEndExpiratoryPressureThreshold()
    {
      if (low_positive_end_expiratory_pressure_threshold == null)
        low_positive_end_expiratory_pressure_threshold = new SEScalarPressure();
      return low_positive_end_expiratory_pressure_threshold;
    }

    public bool HasLowPressureThreshold()
    {
      return low_pressure_threshold == null ? false : low_pressure_threshold.IsValid();
    }
    public SEScalarPressure GetLowPressureThreshold()
    {
      if (low_pressure_threshold == null)
        low_pressure_threshold = new SEScalarPressure();
      return low_pressure_threshold;
    }

    public bool HasLowTidalVolumeThreshold()
    {
      return low_tidal_volume_threshold == null ? false : low_tidal_volume_threshold.IsValid();
    }
    public SEScalarVolume GetLowTidalVolumeThreshold()
    {
      if (low_tidal_volume_threshold == null)
        low_tidal_volume_threshold = new SEScalarVolume();
      return low_tidal_volume_threshold;
    }

    public bool HasOxygenSupplyFailureThreshold()
    {
      return oxygen_supply_failure_threshold == null ? false : oxygen_supply_failure_threshold.IsValid();
    }
    public SEScalar0To1 GetOxygenSupplyFailureThreshold()
    {
      if (oxygen_supply_failure_threshold == null)
        oxygen_supply_failure_threshold = new SEScalar0To1();
      return oxygen_supply_failure_threshold;
    }

    public override string ToString()
    {
      string str = "Mechanical Ventilator Alarms"
      + "\n\tApneaTimeThreshold: " + (HasApneaTimeThreshold() ? GetApneaTimeThreshold().ToString() : "Not Provided")
      + "\n\tAutoPositiveEndExpiratoryPressureThreshold: " + (HasAutoPositiveEndExpiratoryPressureThreshold() ? GetAutoPositiveEndExpiratoryPressureThreshold().ToString() : "Not Provided")
      + "\n\tCircuitLeakThreshold: " + (HasCircuitLeakThreshold() ? GetCircuitLeakThreshold().ToString() : "Not Provided")
      + "\n\tHighEndTidalCarbonDioxideThreshold: " + (HasHighEndTidalCarbonDioxideThreshold() ? GetHighEndTidalCarbonDioxideThreshold().ToString() : "Not Provided")
      + "\n\tHighMinuteVentilationThreshold: " + (HasHighMinuteVentilationThreshold() ? GetHighMinuteVentilationThreshold().ToString() : "Not Provided")
      + "\n\tHighOxygenSaturationThreshold: " + (HasHighOxygenSaturationThreshold() ? GetHighOxygenSaturationThreshold().ToString() : "Not Provided")
      + "\n\tHighPositiveEndExpiratoryPressureThreshold: " + (HasHighPositiveEndExpiratoryPressureThreshold() ? GetHighPositiveEndExpiratoryPressureThreshold().ToString() : "Not Provided")
      + "\n\tHighPressureCycleOption: " + (HasHighPressureCycleOption() ? eEnum.Name(GetHighPressureCycleOption()) : "NotProvided")
      + "\n\tHighPressureThreshold: " + (HasHighPressureThreshold() ? GetHighPressureThreshold().ToString() : "Not Provided")
      + "\n\tHighRespiratoryRateThreshold: " + (HasHighRespiratoryRateThreshold() ? GetHighRespiratoryRateThreshold().ToString() : "Not Provided")
      + "\n\tHighTidalVolumeThreshold: " + (HasHighTidalVolumeThreshold() ? GetHighTidalVolumeThreshold().ToString() : "Not Provided")
      + "\n\tLowEndTidalCarbonDioxideThreshold: " + (HasLowEndTidalCarbonDioxideThreshold() ? GetLowEndTidalCarbonDioxideThreshold().ToString() : "Not Provided")
      + "\n\tLowMinuteVentilationThreshold: " + (HasLowMinuteVentilationThreshold() ? GetLowMinuteVentilationThreshold().ToString() : "Not Provided")
      + "\n\tLowOxygenSaturationThreshold: " + (HasLowOxygenSaturationThreshold() ? GetLowOxygenSaturationThreshold().ToString() : "Not Provided")
      + "\n\tLowPositiveEndExpiratoryPressureThreshold: " + (HasLowPositiveEndExpiratoryPressureThreshold() ? GetLowPositiveEndExpiratoryPressureThreshold().ToString() : "Not Provided")
      + "\n\tLowPressureThreshold: " + (HasLowPressureThreshold() ? GetLowPressureThreshold().ToString() : "Not Provided")
      + "\n\tLowTidalVolumeThreshold: " + (HasLowTidalVolumeThreshold() ? GetLowTidalVolumeThreshold().ToString() : "Not Provided")
      + "\n\tOxygenSupplyFailureThreshold: " + (HasOxygenSupplyFailureThreshold() ? GetOxygenSupplyFailureThreshold().ToString() : "Not Provided");

      return str;
    }
  }
}