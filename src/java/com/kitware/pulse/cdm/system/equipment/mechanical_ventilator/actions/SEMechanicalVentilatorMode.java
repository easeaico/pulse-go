/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.system.equipment.mechanical_ventilator.actions;

import com.kitware.pulse.cdm.bind.Enums.eSwitch;
import com.kitware.pulse.cdm.bind.MechanicalVentilatorActions.MechanicalVentilatorModeData;
import com.kitware.pulse.cdm.properties.SEScalarPressure;
import com.kitware.pulse.cdm.properties.SEScalarVolumePerTime;
import com.kitware.pulse.cdm.system.equipment.mechanical_ventilator.SEMechanicalVentilatorSettings;

public class SEMechanicalVentilatorMode extends SEMechanicalVentilatorAction
{
  protected eSwitch connection;
  protected SEMechanicalVentilatorSettings supplemental_settings=null;

  public SEMechanicalVentilatorMode()
  {
    clear();
  }

  public SEMechanicalVentilatorMode(SEMechanicalVentilatorMode other)
  {
    copy(other);
  }

  public void copy(SEMechanicalVentilatorMode other)
  {
    super.copy(other);
    connection = other.connection;
    if(other.supplemental_settings!=null)
      this.getSupplementalSettings().copy(other.supplemental_settings);
  }

  public void clear()
  {
    connection = eSwitch.Off;
    if (this.supplemental_settings != null)
      this.supplemental_settings.clear();
  }
  
  protected static void load(MechanicalVentilatorModeData src, SEMechanicalVentilatorMode dst)
  {
    dst.clear();
    SEMechanicalVentilatorAction.load(src.getMechanicalVentilatorAction(),dst);
    dst.setConnection(src.getConnection());
    if(src.hasSupplementalSettings())
      SEMechanicalVentilatorSettings.load(src.getSupplementalSettings(),dst.getSupplementalSettings());
  }
  protected static void unload(SEMechanicalVentilatorMode src, MechanicalVentilatorModeData.Builder dst)
  {
    SEMechanicalVentilatorAction.unload(src, dst.getMechanicalVentilatorActionBuilder());
    dst.setConnection(src.getConnection());
    if(src.hasSupplementalSettings())
      dst.setSupplementalSettings(SEMechanicalVentilatorSettings.unload(src.supplemental_settings));
  }

  public boolean isValid()
  {
    return true;
  }

  public eSwitch getConnection()
  {
    return connection;
  }

  public void setConnection(eSwitch s)
  {
    connection = s;
  }
  
  public boolean hasSupplementalSettings()
  {
    return this.supplemental_settings!=null;
  }
  public SEMechanicalVentilatorSettings getSupplementalSettings()
  {
    if(this.supplemental_settings==null)
      this.supplemental_settings=new SEMechanicalVentilatorSettings();
    return this.supplemental_settings;
  }

  public String toString()
  {
    String str = "Mechanical Ventilator Mode";
    str += "\n\tConnection: " + this.connection;
    if(hasSupplementalSettings())
      str += supplemental_settings.toString();

    return str;
  }
}
