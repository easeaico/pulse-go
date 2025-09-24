/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using pulse.cdm.bind;

namespace Pulse.CDM
{
  public class SEEngineInitializationStatus
  {
    protected eEngineInitializationState initialization_state;
    protected string csv_filename;
    protected string log_filename;
    protected double stabilization_time_s;

    public SEEngineInitializationStatus()
    {
      Clear();
    }

    public virtual void Clear()
    {
      initialization_state = eEngineInitializationState.Uninitialized;
      csv_filename = "";
      log_filename = "";
      stabilization_time_s = 0;
    }

    public eEngineInitializationState GetEngineInitializationState()
    {
      return initialization_state;
    }
    public void SetEngineInitializationState(eEngineInitializationState s)
    {
      initialization_state = s;
    }

    public bool HasCSVFilename() { return csv_filename.Length != 0; }
    public  string GetCSVFilename() { return csv_filename; }
    public void SetCSVFilename(string s) { csv_filename = s; }

    public bool HasLogFilename() { return log_filename.Length != 0; }
    public string GetLogFilename() { return log_filename; }
    public  void SetLogFilename(string s) { log_filename = s; }

    public double GetStabilizationTime_s() { return stabilization_time_s; }
    public void SetStabilizationTime_s(double t) { stabilization_time_s = t; }
  }
}
