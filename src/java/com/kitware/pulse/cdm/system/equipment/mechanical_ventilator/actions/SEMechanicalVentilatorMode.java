/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.system.equipment.mechanical_ventilator.actions;

import com.kitware.pulse.cdm.bind.Actions.eMergeType;
import com.kitware.pulse.cdm.bind.Enums.eSwitch;
import com.kitware.pulse.cdm.bind.MechanicalVentilatorActions.MechanicalVentilatorModeData;
import com.kitware.pulse.cdm.system.equipment.mechanical_ventilator.SEMechanicalVentilatorSettings;
import com.kitware.pulse.utilities.Log;

public class SEMechanicalVentilatorMode extends SEMechanicalVentilatorAction
{
  private static final long serialVersionUID = -1487014080271791164L;
  protected eSwitch connection;
  protected eMergeType                     mergeType=eMergeType.Replace;
  protected SEMechanicalVentilatorSettings supplementalSettings=null;
  protected String                         supplementalSettingsFile="";

  public SEMechanicalVentilatorMode()
  {
    clear();
  }

  public SEMechanicalVentilatorMode(SEMechanicalVentilatorMode other)
  {
    this();
    copy(other);
  }

  public void copy(SEMechanicalVentilatorMode other)
  {
    super.copy(other);
    connection = other.connection;
    this.mergeType = other.mergeType;
    if(other.supplementalSettings!=null)
      this.getSupplementalSettings().copy(other.supplementalSettings);
    this.supplementalSettingsFile=other.supplementalSettingsFile;
  }

  public void clear()
  {
    connection = eSwitch.Off;
    this.mergeType = eMergeType.Replace;
    if (this.supplementalSettings != null)
      this.supplementalSettings.clear();
    this.supplementalSettingsFile="";
  }

  public boolean isValid()
  {
    return true;
  }

  protected static void load(MechanicalVentilatorModeData src, SEMechanicalVentilatorMode dst)
  {
    dst.clear();
    SEMechanicalVentilatorAction.load(src.getMechanicalVentilatorAction(),dst);
    dst.setConnection(src.getConnection());
    dst.setMergeType(src.getMergeType());
    switch(src.getOptionCase())
    {
    case SUPPLEMENTALSETTINGSFILE:
      dst.supplementalSettingsFile = src.getSupplementalSettingsFile();
      break;
    case SUPPLEMENTALSETTINGS:
      SEMechanicalVentilatorSettings.load(src.getSupplementalSettings(),dst.getSupplementalSettings());
      break;
    default:
      Log.error("Unknown MechanicalVentilatorModeData Option");
    }

  }
  protected static void unload(SEMechanicalVentilatorMode src, MechanicalVentilatorModeData.Builder dst)
  {
    SEMechanicalVentilatorAction.unload(src, dst.getMechanicalVentilatorActionBuilder());
    dst.setConnection(src.getConnection());
    if(src.hasSupplementalSettings())
      dst.setSupplementalSettings(SEMechanicalVentilatorSettings.unload(src.supplementalSettings));
  }

  public eSwitch getConnection()
  {
    return connection;
  }

  public void setConnection(eSwitch s)
  {
    connection = s;
  }
  
  public eMergeType getMergeType()
  {
    return this.mergeType;
  }

  public void setMergeType(eMergeType mt)
  {
    this.mergeType = mt;
  }
  
  public boolean hasSupplementalSettings()
  {
    return this.supplementalSettings!=null;
  }
  public SEMechanicalVentilatorSettings getSupplementalSettings()
  {
    if(this.supplementalSettings==null)
      this.supplementalSettings=new SEMechanicalVentilatorSettings();
    return this.supplementalSettings;
  }
  
  public boolean hasSupplementalSettingsFile()
  {
    return this.supplementalSettingsFile!=null&&!this.supplementalSettingsFile.isEmpty();
  }
  public String getSupplementalSettingsFile()
  {
    return this.supplementalSettingsFile;
  }
  public void setSupplementalSettingsFile(String s)
  {
    this.supplementalSettingsFile = s;
  }

  public String toString()
  {
    String str = "Mechanical Ventilator Mode";
    str += "\n\tConnection: " + this.connection;
    if(hasSupplementalSettings())
      str += supplementalSettings.toString();

    return str;
  }
}
