/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.system.equipment.mechanical_ventilator;

import com.kitware.pulse.cdm.bind.Enums.eSwitch;
import com.kitware.pulse.cdm.bind.MechanicalVentilator.MechanicalVentilatorAlarmsData;
import com.kitware.pulse.cdm.properties.*;

public class SEMechanicalVentilatorAlarms
{
  protected SEScalarTime apneaTimeThreshold;
  protected SEScalarPressure autoPositiveEndExpiratoryPressureThreshold;
  protected SEScalar0To1 circuitLeakThreshold;
  protected SEScalarPressure highEndTidalCarbonDioxideThreshold;
  protected SEScalarVolumePerTime highMinuteVentilationThreshold;
  protected SEScalar0To1 highOxygenSaturationThreshold;
  protected SEScalarPressure highPositiveEndExpiratoryPressureThreshold;
  protected eSwitch highPressureCycleOption;
  protected SEScalarPressure highPressureThreshold;
  protected SEScalarFrequency highRespiratoryRateThreshold;
  protected SEScalarVolume highTidalVolumeThreshold;
  protected SEScalarPressure lowEndTidalCarbonDioxideThreshold;
  protected SEScalarVolumePerTime lowMinuteVentilationThreshold;
  protected SEScalar0To1 lowOxygenSaturationThreshold;
  protected SEScalarPressure lowPositiveEndExpiratoryPressureThreshold;
  protected SEScalarPressure lowPressureThreshold;
  protected SEScalarVolume lowTidalVolumeThreshold;
  protected SEScalar0To1 oxygenSupplyFailureThreshold;


  public SEMechanicalVentilatorAlarms()
  {
    apneaTimeThreshold = null;
    autoPositiveEndExpiratoryPressureThreshold = null;
    circuitLeakThreshold = null;
    highEndTidalCarbonDioxideThreshold = null;
    highMinuteVentilationThreshold = null;
    highOxygenSaturationThreshold = null;
    highPositiveEndExpiratoryPressureThreshold = null;
    highPressureCycleOption = eSwitch.Off;
    highPressureThreshold = null;
    highRespiratoryRateThreshold = null;
    highTidalVolumeThreshold = null;
    lowEndTidalCarbonDioxideThreshold = null;
    lowMinuteVentilationThreshold = null;
    lowOxygenSaturationThreshold = null;
    lowPositiveEndExpiratoryPressureThreshold = null;
    lowPressureThreshold = null;
    lowTidalVolumeThreshold = null;
    oxygenSupplyFailureThreshold = null;
  }

  public void clear()
  {
    if (apneaTimeThreshold != null)
      apneaTimeThreshold.invalidate();
    if (autoPositiveEndExpiratoryPressureThreshold != null)
      autoPositiveEndExpiratoryPressureThreshold.invalidate();
    if (circuitLeakThreshold != null)
      circuitLeakThreshold.invalidate();
    if (highEndTidalCarbonDioxideThreshold != null)
      highEndTidalCarbonDioxideThreshold.invalidate();
    if (highMinuteVentilationThreshold != null)
      highMinuteVentilationThreshold.invalidate();
    if (highOxygenSaturationThreshold != null)
      highOxygenSaturationThreshold.invalidate();
    if (highPositiveEndExpiratoryPressureThreshold != null)
      highPositiveEndExpiratoryPressureThreshold.invalidate();
    highPressureCycleOption = eSwitch.NullSwitch;
    if (highPressureThreshold != null)
      highPressureThreshold.invalidate();
    if (highRespiratoryRateThreshold != null)
      highRespiratoryRateThreshold.invalidate();
    if (highTidalVolumeThreshold != null)
      highTidalVolumeThreshold.invalidate();
    if (lowEndTidalCarbonDioxideThreshold != null)
      lowEndTidalCarbonDioxideThreshold.invalidate();
    if (lowMinuteVentilationThreshold != null)
      lowMinuteVentilationThreshold.invalidate();
    if (lowOxygenSaturationThreshold != null)
      lowOxygenSaturationThreshold.invalidate();
    if (lowPositiveEndExpiratoryPressureThreshold != null)
      lowPositiveEndExpiratoryPressureThreshold.invalidate();
    if (lowPressureThreshold != null)
      lowPressureThreshold.invalidate();
    if (lowTidalVolumeThreshold != null)
      lowTidalVolumeThreshold.invalidate();
    if (oxygenSupplyFailureThreshold != null)
      oxygenSupplyFailureThreshold.invalidate();
  }

  public void copy(SEMechanicalVentilatorAlarms from)
  {
    clear();
    if (from.hasApneaTimeThreshold())
      this.getApneaTimeThreshold().set(from.getApneaTimeThreshold());
    if (from.hasAutoPositiveEndExpiratoryPressureThreshold())
      this.getAutoPositiveEndExpiratoryPressureThreshold().set(from.getAutoPositiveEndExpiratoryPressureThreshold());
    if (from.hasCircuitLeakThreshold())
      this.getCircuitLeakThreshold().set(from.getCircuitLeakThreshold());
    if (from.hasHighEndTidalCarbonDioxideThreshold())
      this.getHighEndTidalCarbonDioxideThreshold().set(from.getHighEndTidalCarbonDioxideThreshold());
    if (from.hasHighMinuteVentilationThreshold())
      this.getHighMinuteVentilationThreshold().set(from.getHighMinuteVentilationThreshold());
    if (from.hasHighOxygenSaturationThreshold())
      this.getHighOxygenSaturationThreshold().set(from.getHighOxygenSaturationThreshold());
    if (from.hasHighPositiveEndExpiratoryPressureThreshold())
      this.getHighPositiveEndExpiratoryPressureThreshold().set(from.getHighPositiveEndExpiratoryPressureThreshold());
    if (from.highPressureCycleOption != eSwitch.NullSwitch)
      this.highPressureCycleOption = from.highPressureCycleOption;
    if (from.hasHighPressureThreshold())
      this.getHighPressureThreshold().set(from.getHighPressureThreshold());
    if (from.hasHighRespiratoryRateThreshold())
      this.getHighRespiratoryRateThreshold().set(from.getHighRespiratoryRateThreshold());
    if (from.hasHighTidalVolumeThreshold())
      this.getHighTidalVolumeThreshold().set(from.getHighTidalVolumeThreshold());
    if (from.hasLowEndTidalCarbonDioxideThreshold())
      this.getLowEndTidalCarbonDioxideThreshold().set(from.getLowEndTidalCarbonDioxideThreshold());
    if (from.hasLowMinuteVentilationThreshold())
      this.getLowMinuteVentilationThreshold().set(from.getLowMinuteVentilationThreshold());
    if (from.hasLowOxygenSaturationThreshold())
      this.getLowOxygenSaturationThreshold().set(from.getLowOxygenSaturationThreshold());
    if (from.hasLowPositiveEndExpiratoryPressureThreshold())
      this.getLowPositiveEndExpiratoryPressureThreshold().set(from.getLowPositiveEndExpiratoryPressureThreshold());
    if (from.hasLowPressureThreshold())
      this.getLowPressureThreshold().set(from.getLowPressureThreshold());
    if (from.hasLowTidalVolumeThreshold())
      this.getLowTidalVolumeThreshold().set(from.getLowTidalVolumeThreshold());
    if (from.hasOxygenSupplyFailureThreshold())
      this.getOxygenSupplyFailureThreshold().set(from.getOxygenSupplyFailureThreshold());
  }

  public static void load(MechanicalVentilatorAlarmsData src, SEMechanicalVentilatorAlarms dst)
  {
    dst.clear();
    if (src.hasApneaTimeThreshold())
      SEScalarTime.load(src.getApneaTimeThreshold(), dst.getApneaTimeThreshold());
    if (src.hasAutoPositiveEndExpiratoryPressureThreshold())
      SEScalarPressure.load(src.getAutoPositiveEndExpiratoryPressureThreshold(), dst.getAutoPositiveEndExpiratoryPressureThreshold());
    if (src.hasCircuitLeakThreshold())
      SEScalar0To1.load(src.getCircuitLeakThreshold(), dst.getCircuitLeakThreshold());
    if (src.hasHighEndTidalCarbonDioxideThreshold())
      SEScalarPressure.load(src.getHighEndTidalCarbonDioxideThreshold(), dst.getHighEndTidalCarbonDioxideThreshold());
    if (src.hasHighMinuteVentilationThreshold())
      SEScalarVolumePerTime.load(src.getHighMinuteVentilationThreshold(), dst.getHighMinuteVentilationThreshold());
    if (src.hasHighOxygenSaturationThreshold())
      SEScalar0To1.load(src.getHighOxygenSaturationThreshold(), dst.getHighOxygenSaturationThreshold());
    if (src.hasHighPositiveEndExpiratoryPressureThreshold())
      SEScalarPressure.load(src.getHighPositiveEndExpiratoryPressureThreshold(), dst.getHighPositiveEndExpiratoryPressureThreshold());
    if (src.getHighPressureCycleOption()!=eSwitch.UNRECOGNIZED)
      dst.setHighPressureCycleOption(src.getHighPressureCycleOption());
    if (src.hasHighPressureThreshold())
      SEScalarPressure.load(src.getHighPressureThreshold(), dst.getHighPressureThreshold());
    if (src.hasHighRespiratoryRateThreshold())
      SEScalarFrequency.load(src.getHighRespiratoryRateThreshold(), dst.getHighRespiratoryRateThreshold());
    if (src.hasHighTidalVolumeThreshold())
      SEScalarVolume.load(src.getHighTidalVolumeThreshold(), dst.getHighTidalVolumeThreshold());
    if (src.hasLowEndTidalCarbonDioxideThreshold())
      SEScalarPressure.load(src.getLowEndTidalCarbonDioxideThreshold(), dst.getLowEndTidalCarbonDioxideThreshold());
    if (src.hasLowMinuteVentilationThreshold())
      SEScalarVolumePerTime.load(src.getLowMinuteVentilationThreshold(), dst.getLowMinuteVentilationThreshold());
    if (src.hasLowOxygenSaturationThreshold())
      SEScalar0To1.load(src.getLowOxygenSaturationThreshold(), dst.getLowOxygenSaturationThreshold());
    if (src.hasLowPositiveEndExpiratoryPressureThreshold())
      SEScalarPressure.load(src.getLowPositiveEndExpiratoryPressureThreshold(), dst.getLowPositiveEndExpiratoryPressureThreshold());
    if (src.hasLowPressureThreshold())
      SEScalarPressure.load(src.getLowPressureThreshold(), dst.getLowPressureThreshold());
    if (src.hasLowTidalVolumeThreshold())
      SEScalarVolume.load(src.getLowTidalVolumeThreshold(), dst.getLowTidalVolumeThreshold());
    if (src.hasOxygenSupplyFailureThreshold())
      SEScalar0To1.load(src.getOxygenSupplyFailureThreshold(), dst.getOxygenSupplyFailureThreshold());
  }
  public static MechanicalVentilatorAlarmsData unload(SEMechanicalVentilatorAlarms src)
  {
    MechanicalVentilatorAlarmsData.Builder dst = MechanicalVentilatorAlarmsData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(SEMechanicalVentilatorAlarms src, MechanicalVentilatorAlarmsData.Builder dst)
  {
    if(src.hasApneaTimeThreshold())
      dst.setApneaTimeThreshold(SEScalarTime.unload(src.getApneaTimeThreshold()));
    if(src.hasAutoPositiveEndExpiratoryPressureThreshold())
      dst.setAutoPositiveEndExpiratoryPressureThreshold(SEScalarPressure.unload(src.getAutoPositiveEndExpiratoryPressureThreshold()));
    if(src.hasCircuitLeakThreshold())
      dst.setCircuitLeakThreshold(SEScalar0To1.unload(src.getCircuitLeakThreshold()));
    if(src.hasHighEndTidalCarbonDioxideThreshold())
      dst.setHighEndTidalCarbonDioxideThreshold(SEScalarPressure.unload(src.getHighEndTidalCarbonDioxideThreshold()));
    if(src.hasHighMinuteVentilationThreshold())
      dst.setHighMinuteVentilationThreshold(SEScalarVolumePerTime.unload(src.getHighMinuteVentilationThreshold()));
    if(src.hasHighOxygenSaturationThreshold())
      dst.setHighOxygenSaturationThreshold(SEScalar0To1.unload(src.getHighOxygenSaturationThreshold()));
    if(src.hasHighPositiveEndExpiratoryPressureThreshold())
      dst.setHighPositiveEndExpiratoryPressureThreshold(SEScalarPressure.unload(src.getHighPositiveEndExpiratoryPressureThreshold()));
    if (src.hasHighPressureCycleOption())
      dst.setHighPressureCycleOption(src.highPressureCycleOption);
    if(src.hasHighPressureThreshold())
      dst.setHighPressureThreshold(SEScalarPressure.unload(src.getHighPressureThreshold()));
    if(src.hasHighRespiratoryRateThreshold())
      dst.setHighRespiratoryRateThreshold(SEScalarFrequency.unload(src.getHighRespiratoryRateThreshold()));
    if(src.hasHighTidalVolumeThreshold())
      dst.setHighTidalVolumeThreshold(SEScalarVolume.unload(src.getHighTidalVolumeThreshold()));
    if(src.hasLowEndTidalCarbonDioxideThreshold())
      dst.setLowEndTidalCarbonDioxideThreshold(SEScalarPressure.unload(src.getLowEndTidalCarbonDioxideThreshold()));
    if(src.hasLowMinuteVentilationThreshold())
      dst.setLowMinuteVentilationThreshold(SEScalarVolumePerTime.unload(src.getLowMinuteVentilationThreshold()));
    if(src.hasLowOxygenSaturationThreshold())
      dst.setLowOxygenSaturationThreshold(SEScalar0To1.unload(src.getLowOxygenSaturationThreshold()));
    if(src.hasLowPositiveEndExpiratoryPressureThreshold())
      dst.setLowPositiveEndExpiratoryPressureThreshold(SEScalarPressure.unload(src.getLowPositiveEndExpiratoryPressureThreshold()));
    if(src.hasLowPressureThreshold())
      dst.setLowPressureThreshold(SEScalarPressure.unload(src.getLowPressureThreshold()));
    if(src.hasLowTidalVolumeThreshold())
      dst.setLowTidalVolumeThreshold(SEScalarVolume.unload(src.getLowTidalVolumeThreshold()));
    if(src.hasOxygenSupplyFailureThreshold())
      dst.setOxygenSupplyFailureThreshold(SEScalar0To1.unload(src.getOxygenSupplyFailureThreshold()));
  }

  public boolean hasApneaTimeThreshold()
  {
    return apneaTimeThreshold == null ? false : apneaTimeThreshold.isValid();
  }
  public SEScalarTime getApneaTimeThreshold()
  {
    if (apneaTimeThreshold == null)
      apneaTimeThreshold = new SEScalarTime();
    return apneaTimeThreshold;
  }

  public boolean hasAutoPositiveEndExpiratoryPressureThreshold()
  {
    return autoPositiveEndExpiratoryPressureThreshold == null ? false : autoPositiveEndExpiratoryPressureThreshold.isValid();
  }
  public SEScalarPressure getAutoPositiveEndExpiratoryPressureThreshold()
  {
    if (autoPositiveEndExpiratoryPressureThreshold == null)
      autoPositiveEndExpiratoryPressureThreshold = new SEScalarPressure();
    return autoPositiveEndExpiratoryPressureThreshold;
  }

  public boolean hasCircuitLeakThreshold()
  {
    return circuitLeakThreshold == null ? false : circuitLeakThreshold.isValid();
  }
  public SEScalar0To1 getCircuitLeakThreshold()
  {
    if (circuitLeakThreshold == null)
      circuitLeakThreshold = new SEScalar0To1();
    return circuitLeakThreshold;
  }

  public boolean hasHighEndTidalCarbonDioxideThreshold()
  {
    return highEndTidalCarbonDioxideThreshold == null ? false : highEndTidalCarbonDioxideThreshold.isValid();
  }
  public SEScalarPressure getHighEndTidalCarbonDioxideThreshold()
  {
    if (highEndTidalCarbonDioxideThreshold == null)
      highEndTidalCarbonDioxideThreshold = new SEScalarPressure();
    return highEndTidalCarbonDioxideThreshold;
  }

  public boolean hasHighMinuteVentilationThreshold()
  {
    return highMinuteVentilationThreshold == null ? false : highMinuteVentilationThreshold.isValid();
  }
  public SEScalarVolumePerTime getHighMinuteVentilationThreshold()
  {
    if (highMinuteVentilationThreshold == null)
      highMinuteVentilationThreshold = new SEScalarVolumePerTime();
    return highMinuteVentilationThreshold;
  }

  public boolean hasHighOxygenSaturationThreshold()
  {
    return highOxygenSaturationThreshold == null ? false : highOxygenSaturationThreshold.isValid();
  }
  public SEScalar0To1 getHighOxygenSaturationThreshold()
  {
    if (highOxygenSaturationThreshold == null)
      highOxygenSaturationThreshold = new SEScalar0To1();
    return highOxygenSaturationThreshold;
  }

  public boolean hasHighPositiveEndExpiratoryPressureThreshold()
  {
    return highPositiveEndExpiratoryPressureThreshold == null ? false : highPositiveEndExpiratoryPressureThreshold.isValid();
  }
  public SEScalarPressure getHighPositiveEndExpiratoryPressureThreshold()
  {
    if (highPositiveEndExpiratoryPressureThreshold == null)
      highPositiveEndExpiratoryPressureThreshold = new SEScalarPressure();
    return highPositiveEndExpiratoryPressureThreshold;
  }

  public boolean hasHighPressureCycleOption()
  {
    return highPressureCycleOption != eSwitch.NullSwitch;
  }
  public eSwitch getHighPressureCycleOption()
  {
    return highPressureCycleOption;
  }
  public void setHighPressureCycleOption(eSwitch option)
  {
    highPressureCycleOption = (option == eSwitch.NullSwitch) ? eSwitch.Off : option;
  }

  public boolean hasHighPressureThreshold()
  {
    return highPressureThreshold == null ? false : highPressureThreshold.isValid();
  }
  public SEScalarPressure getHighPressureThreshold()
  {
    if (highPressureThreshold == null)
      highPressureThreshold = new SEScalarPressure();
    return highPressureThreshold;
  }

  public boolean hasHighRespiratoryRateThreshold()
  {
    return highRespiratoryRateThreshold == null ? false : highRespiratoryRateThreshold.isValid();
  }
  public SEScalarFrequency getHighRespiratoryRateThreshold()
  {
    if (highRespiratoryRateThreshold == null)
      highRespiratoryRateThreshold = new SEScalarFrequency();
    return highRespiratoryRateThreshold;
  }

  public boolean hasHighTidalVolumeThreshold()
  {
    return highTidalVolumeThreshold == null ? false : highTidalVolumeThreshold.isValid();
  }
  public SEScalarVolume getHighTidalVolumeThreshold()
  {
    if (highTidalVolumeThreshold == null)
      highTidalVolumeThreshold = new SEScalarVolume();
    return highTidalVolumeThreshold;
  }

  public boolean hasLowEndTidalCarbonDioxideThreshold()
  {
    return lowEndTidalCarbonDioxideThreshold == null ? false : lowEndTidalCarbonDioxideThreshold.isValid();
  }
  public SEScalarPressure getLowEndTidalCarbonDioxideThreshold()
  {
    if (lowEndTidalCarbonDioxideThreshold == null)
      lowEndTidalCarbonDioxideThreshold = new SEScalarPressure();
    return lowEndTidalCarbonDioxideThreshold;
  }

  public boolean hasLowMinuteVentilationThreshold()
  {
    return lowMinuteVentilationThreshold == null ? false : lowMinuteVentilationThreshold.isValid();
  }
  public SEScalarVolumePerTime getLowMinuteVentilationThreshold()
  {
    if (lowMinuteVentilationThreshold == null)
      lowMinuteVentilationThreshold = new SEScalarVolumePerTime();
    return lowMinuteVentilationThreshold;
  }

  public boolean hasLowOxygenSaturationThreshold()
  {
    return lowOxygenSaturationThreshold == null ? false : lowOxygenSaturationThreshold.isValid();
  }
  public SEScalar0To1 getLowOxygenSaturationThreshold()
  {
    if (lowOxygenSaturationThreshold == null)
      lowOxygenSaturationThreshold = new SEScalar0To1();
    return lowOxygenSaturationThreshold;
  }

  public boolean hasLowPositiveEndExpiratoryPressureThreshold()
  {
    return lowPositiveEndExpiratoryPressureThreshold == null ? false : lowPositiveEndExpiratoryPressureThreshold.isValid();
  }
  public SEScalarPressure getLowPositiveEndExpiratoryPressureThreshold()
  {
    if (lowPositiveEndExpiratoryPressureThreshold == null)
      lowPositiveEndExpiratoryPressureThreshold = new SEScalarPressure();
    return lowPositiveEndExpiratoryPressureThreshold;
  }

  public boolean hasLowPressureThreshold()
  {
    return lowPressureThreshold == null ? false : lowPressureThreshold.isValid();
  }
  public SEScalarPressure getLowPressureThreshold()
  {
    if (lowPressureThreshold == null)
      lowPressureThreshold = new SEScalarPressure();
    return lowPressureThreshold;
  }

  public boolean hasLowTidalVolumeThreshold()
  {
    return lowTidalVolumeThreshold == null ? false : lowTidalVolumeThreshold.isValid();
  }
  public SEScalarVolume getLowTidalVolumeThreshold()
  {
    if (lowTidalVolumeThreshold == null)
      lowTidalVolumeThreshold = new SEScalarVolume();
    return lowTidalVolumeThreshold;
  }

  public boolean hasOxygenSupplyFailureThreshold()
  {
    return oxygenSupplyFailureThreshold == null ? false : oxygenSupplyFailureThreshold.isValid();
  }
  public SEScalar0To1 getOxygenSupplyFailureThreshold()
  {
    if (oxygenSupplyFailureThreshold == null)
      oxygenSupplyFailureThreshold = new SEScalar0To1();
    return oxygenSupplyFailureThreshold;
  }

  public String toString()
  {
    String str = "Mechanical Ventilator Alarms"
    + "\n\tApneaTimeThreshold: " + (hasApneaTimeThreshold() ? getApneaTimeThreshold().toString() : "Not Provided")
    + "\n\tAutoPositiveEndExpiratoryPressureThreshold: " + (hasAutoPositiveEndExpiratoryPressureThreshold() ? getAutoPositiveEndExpiratoryPressureThreshold().toString() : "Not Provided")
    + "\n\tCircuitLeakThreshold: " + (hasCircuitLeakThreshold() ? getCircuitLeakThreshold().toString() : "Not Provided")
    + "\n\tHighEndTidalCarbonDioxideThreshold: " + (hasHighEndTidalCarbonDioxideThreshold() ? getHighEndTidalCarbonDioxideThreshold().toString() : "Not Provided")
    + "\n\tHighMinuteVentilationThreshold: " + (hasHighMinuteVentilationThreshold() ? getHighMinuteVentilationThreshold().toString() : "Not Provided")
    + "\n\tHighOxygenSaturationThreshold: " + (hasHighOxygenSaturationThreshold() ? getHighOxygenSaturationThreshold().toString() : "Not Provided")
    + "\n\tHighPositiveEndExpiratoryPressureThreshold: " + (hasHighPositiveEndExpiratoryPressureThreshold() ? getHighPositiveEndExpiratoryPressureThreshold().toString() : "Not Provided")
    + "\n\tHighPressureCycleOption: " + (hasHighPressureCycleOption()?getHighPressureCycleOption():"NotProvided")
    + "\n\tHighPressureThreshold: " + (hasHighPressureThreshold() ? getHighPressureThreshold().toString() : "Not Provided")
    + "\n\tHighRespiratoryRateThreshold: " + (hasHighRespiratoryRateThreshold() ? getHighRespiratoryRateThreshold().toString() : "Not Provided")
    + "\n\tHighTidalVolumeThreshold: " + (hasHighTidalVolumeThreshold() ? getHighTidalVolumeThreshold().toString() : "Not Provided")
    + "\n\tLowEndTidalCarbonDioxideThreshold: " + (hasLowEndTidalCarbonDioxideThreshold() ? getLowEndTidalCarbonDioxideThreshold().toString() : "Not Provided")
    + "\n\tLowMinuteVentilationThreshold: " + (hasLowMinuteVentilationThreshold() ? getLowMinuteVentilationThreshold().toString() : "Not Provided")
    + "\n\tLowOxygenSaturationThreshold: " + (hasLowOxygenSaturationThreshold() ? getLowOxygenSaturationThreshold().toString() : "Not Provided")
    + "\n\tLowPositiveEndExpiratoryPressureThreshold: " + (hasLowPositiveEndExpiratoryPressureThreshold() ? getLowPositiveEndExpiratoryPressureThreshold().toString() : "Not Provided")
    + "\n\tLowPressureThreshold: " + (hasLowPressureThreshold() ? getLowPressureThreshold().toString() : "Not Provided")
    + "\n\tLowTidalVolumeThreshold: " + (hasLowTidalVolumeThreshold() ? getLowTidalVolumeThreshold().toString() : "Not Provided")
    + "\n\tOxygenSupplyFailureThreshold: " + (hasOxygenSupplyFailureThreshold() ? getOxygenSupplyFailureThreshold().toString() : "Not Provided");

    return str;
  }

}
