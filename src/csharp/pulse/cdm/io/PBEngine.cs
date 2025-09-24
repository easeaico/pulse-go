/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using Google.Protobuf;

namespace Pulse.CDM
{
  public class PBEngine
  {
    #region SEEngineInitializationStatus
    public static void Load(pulse.cdm.bind.EngineInitializationStatusData src, SEEngineInitializationStatus dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.EngineInitializationStatusData src, SEEngineInitializationStatus dst)
    {
      dst.Clear();
      dst.SetEngineInitializationState((eEngineInitializationState)(int)src.InitializationState);
      dst.SetCSVFilename(src.CSVFilename);
      dst.SetLogFilename(src.LogFilename);
      dst.SetStabilizationTime_s(src.StabilizationTimeS);
    }
    public static bool SerializeFromString(string src, SEEngineInitializationStatus dst)
    {
      try
      {
        pulse.cdm.bind.EngineInitializationStatusData data = JsonParser.Default.Parse<pulse.cdm.bind.EngineInitializationStatusData>(src);
        PBEngine.Load(data, dst);
      }
      catch (Google.Protobuf.InvalidJsonException)
      {
        return false;
      }
      return true;
    }

    public static pulse.cdm.bind.EngineInitializationStatusData Unload(SEEngineInitializationStatus src)
    {
      pulse.cdm.bind.EngineInitializationStatusData dst = new pulse.cdm.bind.EngineInitializationStatusData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEEngineInitializationStatus src, pulse.cdm.bind.EngineInitializationStatusData dst)
    {
      dst.InitializationState = (pulse.cdm.bind.eEngineInitializationState)(int)src.GetEngineInitializationState();
      dst.CSVFilename = src.GetCSVFilename();
      dst.LogFilename = src.GetLogFilename();
      dst.StabilizationTimeS = src.GetStabilizationTime_s();
    }
    public static string SerializeToString(SEEngineInitializationStatus src)
    {
      var pb = PBEngine.Unload(src);
      return pb.ToString();
    }
    #endregion
  }
}

