/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

namespace Pulse.CDM
{
  public class SEMechanicalVentilatorMode : SEMechanicalVentilatorAction
  {
    protected eSwitch connection;
    protected eMergeType mergeType = eMergeType.Append;
    protected SEMechanicalVentilatorSettings SupplementalSettings = null;
    protected string SupplementalSettingsFile = "";

    public SEMechanicalVentilatorMode()
    {
      Clear();
    }

    public SEMechanicalVentilatorMode(SEMechanicalVentilatorMode other)
    {
      Copy(other);
    }

    public void Copy(SEMechanicalVentilatorMode other)
    {
      base.Copy(other);
      connection = other.connection;
      mergeType = other.mergeType;
      if (other.SupplementalSettings != null)
        this.GetSupplementalSettings().Copy(other.SupplementalSettings);
      this.SupplementalSettingsFile = other.SupplementalSettingsFile;
    }

    public override void Clear()
    {
      base.Clear();
      connection = eSwitch.Off;
      mergeType = eMergeType.Append;
      if (this.SupplementalSettings != null)
        this.SupplementalSettings.Clear();
      this.SupplementalSettingsFile = "";
    }

    public override bool IsValid()
    {
      return true;
    }

    public eSwitch GetConnection()
    {
      return connection;
    }
    public void SetConnection(eSwitch s)
    {
      connection = s;
    }

    public eMergeType GetMergeType()
    {
      return mergeType;
    }
    public void SetMergeType(eMergeType m)
    {
      mergeType = m;
    }

    public bool HasSupplementalSettings()
    {
      return this.SupplementalSettings != null;
    }
    public SEMechanicalVentilatorSettings GetSupplementalSettings()
    {
      if (this.SupplementalSettings == null)
        this.SupplementalSettings = new SEMechanicalVentilatorSettings();
      return this.SupplementalSettings;
    }

    public bool HasSupplementalSettingsFile()
    {
      return !string.IsNullOrEmpty(this.SupplementalSettingsFile);
    }
    public string GetSupplementalSettingsFile()
    {
      return this.SupplementalSettingsFile;
    }
    public void SetSupplementalSettingsFile(string s)
    {
      this.SupplementalSettingsFile = s;
    }

    public override string ToString()
    {
      string str = "Mechanical Ventilator Mode";
      str += "\n\tConnection: " + this.connection;
      str += "\n\tMerge Type: " + this.mergeType;
      if (this.HasSupplementalSettingsFile())
        str += "\n\tSupplemental Settings File: " + this.SupplementalSettingsFile;
      else if(HasSupplementalSettings())
        str += SupplementalSettings.ToString();

      return str;
    }
  }
}
