/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using System;
using System.Collections.Generic;
using System.IO;

namespace Pulse.CDM
{
  public class SEScenario
  {
    protected string                        name;
    protected string                        description;
    protected SEPatientConfiguration        patient_configuration;
    protected string                        engine_state_file;
    protected SEDataRequestManager          dr_mgr = new SEDataRequestManager();
    protected List<SEAction>                actions = new List<SEAction>();

    public SEScenario()
    {
      Clear();
    }

    public void Clear()
    {
      name = "";
      description = "";
      if (patient_configuration != null)
        patient_configuration.Clear();
      engine_state_file = "";
      dr_mgr.Clear();
      actions.Clear();
    }

    public bool IsValid()
    {
      if (!HasPatientConfiguration() && !HasEngineState())
        return false;
      return true;
    }

    public bool SerializeFromFile(string filename)
    {
      try
      {
        string json = System.IO.File.ReadAllText(filename);
        return SerializeFromString(json, eSerializationFormat.JSON);
      }
      catch (System.Exception ex)
      {
        System.Console.WriteLine("Error Reading Scenario File! " + ex.ToString());
        return false;
      }
    }
    public bool SerializeFromString(string src, eSerializationFormat fmt)
    {
      return PBScenario.SerializeFromString(src, this);
    }

    public bool SerializeToFile(string filename)
    {
      string json = SerializeToString(eSerializationFormat.JSON);
      if (json == null || json.Length == 0)
        return false;
      try
      {
        string directoryPath = Path.GetDirectoryName(filename);
        Directory.CreateDirectory(directoryPath);
        System.IO.File.WriteAllText(filename, json);
      }
      catch (System.Exception ex)
      {
        System.Console.WriteLine("Error Writing Scenario File! " + ex.ToString());
        return false;
      }
      return true;
    }
    public string SerializeToString(eSerializationFormat format)
    {
      return PBScenario.SerializeToString(this);
    }

    public String GetName()
    {
      return name;
    }
    public void SetName(String name)
    {
      this.name = name;
    }

    public String GetDescription()
    {
      return description;
    }
    public void SetDescription(String description)
    {
      this.description = description;
    }

    public bool HasEngineState()
    {
      if (HasPatientConfiguration())
        return false;
      return !string.IsNullOrEmpty(engine_state_file);
    }
    public void SetEngineState(String filename)
    {
      InvalidatePatientConfiguration();
      this.engine_state_file = filename;
    }
    public String GetEngineState() { return this.engine_state_file; }
    public void InvalidateEngineState()
    {
      this.engine_state_file = null;
    }

    public bool HasPatientConfiguration()
    {
      return patient_configuration != null && patient_configuration.IsValid();
    }
    public SEPatientConfiguration GetPatientConfiguration()
    {
      if (patient_configuration == null)
        patient_configuration = new SEPatientConfiguration();
      return patient_configuration;
    }
    public void InvalidatePatientConfiguration()
    {
      patient_configuration = null;
    }

    public List<SEAction> GetActions()
    {
      return actions;
    }

    public SEDataRequestManager GetDataRequestManager()
    {
      return this.dr_mgr;
    }
  }
}
