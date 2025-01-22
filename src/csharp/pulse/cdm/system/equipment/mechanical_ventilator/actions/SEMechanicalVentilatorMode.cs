/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

namespace Pulse.CDM
{
  public class SEMechanicalVentilatorMode : SEMechanicalVentilatorAction
  {
    protected eSwitch connection;
    protected SEMechanicalVentilatorSettings supplemetal_settings = null;

    public SEMechanicalVentilatorMode()
    {
      Clear();
    }

    public SEMechanicalVentilatorMode(SEMechanicalVentilatorMode other)
    {
      Copy(other);
    }

    public void copy(SEMechanicalVentilatorMode other)
    {
      base.Copy(other);
      connection = other.connection;
      if (other.supplemetal_settings != null)
        this.GetSupplementalSettings().Copy(other.supplemetal_settings);
    }

    public override void Clear()
    {
      base.Clear();
      connection = eSwitch.Off;
      if (this.supplemetal_settings != null)
        this.supplemetal_settings.Clear();
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

    public bool HasSupplementalSettings()
    {
      return this.supplemetal_settings != null;
    }
    public SEMechanicalVentilatorSettings GetSupplementalSettings()
    {
      if (this.supplemetal_settings == null)
        this.supplemetal_settings = new SEMechanicalVentilatorSettings();
      return this.supplemetal_settings;
    }

    public override string ToString()
    {
      string str = "Mechanical Ventilator Mode";
      str += "\n\tConnection: " + this.connection;
      if (HasSupplementalSettings())
        str += supplemetal_settings.ToString();

      return str;
    }
  }
}
