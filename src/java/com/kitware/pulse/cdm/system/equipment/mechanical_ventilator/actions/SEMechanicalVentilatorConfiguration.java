/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.system.equipment.mechanical_ventilator.actions;

import com.kitware.pulse.cdm.bind.Actions.eMergeType;
import com.kitware.pulse.cdm.bind.MechanicalVentilatorActions.MechanicalVentilatorConfigurationData;
import com.kitware.pulse.cdm.system.equipment.mechanical_ventilator.SEMechanicalVentilatorSettings;
import com.kitware.pulse.utilities.Log;

public class SEMechanicalVentilatorConfiguration extends SEMechanicalVentilatorAction
{
  private static final long serialVersionUID = -1487014080271791164L;
  protected eMergeType                     mergeType=eMergeType.Append;
  protected SEMechanicalVentilatorSettings settings=null;
  protected String                         settingsFile="";
  
  public SEMechanicalVentilatorConfiguration()
  {
    
  }
  
  public SEMechanicalVentilatorConfiguration(SEMechanicalVentilatorConfiguration other)
  {
    this();
    copy(other);
  }
  
  public void copy(SEMechanicalVentilatorConfiguration other)
  {
    if(this==other)
      return;
    super.copy(other);
    this.mergeType = other.mergeType;
    if(other.settings!=null)
      this.getSettings().copy(other.settings);
    this.settingsFile=other.settingsFile;
  }
  
  @Override
  public void clear()
  {
    super.clear();
    
    this.mergeType = eMergeType.Append;
    if (this.settings != null)
      this.settings.clear();
    this.settingsFile="";
  }
  
  @Override
  public boolean isValid()
  {
    return hasSettings() || hasSettingsFile();
  }
  
  public static void load(MechanicalVentilatorConfigurationData src, SEMechanicalVentilatorConfiguration dst)
  {
    SEMechanicalVentilatorAction.load(src.getMechanicalVentilatorAction(),dst);
    dst.setMergeType(src.getMergeType());
    switch(src.getOptionCase())
    {
    case SETTINGSFILE:
      dst.settingsFile = src.getSettingsFile();
      break;
    case SETTINGS:
      SEMechanicalVentilatorSettings.load(src.getSettings(),dst.getSettings());
      break;
    default:
    	Log.error("Unknown MechanicalVentilatorConfigurationData Option");
    }
  }
  public static MechanicalVentilatorConfigurationData unload(SEMechanicalVentilatorConfiguration src)
  {
    MechanicalVentilatorConfigurationData.Builder dst = MechanicalVentilatorConfigurationData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(SEMechanicalVentilatorConfiguration src, MechanicalVentilatorConfigurationData.Builder dst)
  {
    SEMechanicalVentilatorAction.unload(src, dst.getMechanicalVentilatorActionBuilder());
    dst.setMergeType(src.mergeType);
    if(src.hasSettingsFile())
      dst.setSettingsFile(src.settingsFile);
    else if(src.hasSettings())
      dst.setSettings(SEMechanicalVentilatorSettings.unload(src.settings));
  }
  
  public eMergeType getMergeType()
  {
    return this.mergeType;
  }

  public void setMergeType(eMergeType mt)
  {
    this.mergeType = mt;
  }
  
  public boolean hasSettings()
  {
    return this.settings!=null;
  }
  public SEMechanicalVentilatorSettings getSettings()
  {
    if(this.settings==null)
      this.settings=new SEMechanicalVentilatorSettings();
    return this.settings;
  }
  
  public boolean hasSettingsFile()
  {
    return this.settingsFile!=null&&!this.settingsFile.isEmpty();
  }
  public String getSettingsFile()
  {
    return this.settingsFile;
  }
  public void setSettingsFile(String s)
  {
    this.settingsFile = s;
  }
  
  @Override
  public String toString()
  {
    String str = "Mechanical Ventilator Configuration";
    if(this.hasSettingsFile())
      str +="\n\tSettings File: "+this.settingsFile;
    else if(hasSettings())
    {
      str += settings.toString();
    }
    
    return str;
  }
}