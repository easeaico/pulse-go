/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

namespace Pulse.CDM
{
  public class SEMechanicalVentilatorConfiguration : SEMechanicalVentilatorAction
  {
    protected eMergeType mergeType = eMergeType.Append;
    protected SEMechanicalVentilatorSettings settings = null;
    protected string settingsFile = "";

    public SEMechanicalVentilatorConfiguration()
    {
      Clear();
    }

    public SEMechanicalVentilatorConfiguration(SEMechanicalVentilatorConfiguration other)
    {
      Copy(other);
    }

    public void Copy(SEMechanicalVentilatorConfiguration other)
    {
      base.Copy(other);
      mergeType = other.mergeType;
      if (other.settings != null)
        this.GetSettings().Copy(other.settings);
      this.settingsFile = other.settingsFile;
    }

    public override void Clear()
    {
      base.Clear();
      mergeType = eMergeType.Append;
      if (this.settings != null)
        this.settings.Clear();
      this.settingsFile = "";
    }

    public override bool IsValid()
    {
      return HasSettings() || HasSettingsFile();
    }

    public eMergeType GetMergeType()
    {
      return mergeType;
    }
    public void SetMergeType(eMergeType m)
    {
      mergeType = m;
    }

    public bool HasSettings()
    {
      return this.settings != null;
    }
    public SEMechanicalVentilatorSettings GetSettings()
    {
      if (this.settings == null)
        this.settings = new SEMechanicalVentilatorSettings();
      return this.settings;
    }

    public bool HasSettingsFile()
    {
      return !string.IsNullOrEmpty(this.settingsFile);
    }
    public string GetSettingsFile()
    {
      return this.settingsFile;
    }
    public void SetSettingsFile(string s)
    {
      this.settingsFile = s;
    }

    public override string ToString()
    {
      string str = "Mechanical Ventilator Configuration";
      str += "\n\tMerge Type: " + this.mergeType;
      if (this.HasSettingsFile())
        str += "\n\tSettings File: " + this.settingsFile;
      else if (HasSettings())
      {
        str += settings.ToString();
      }
      return str;
    }
  }
}
