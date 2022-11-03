/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using System.Collections.Generic;
using Google.Protobuf;

namespace Pulse.CDM
{
  public class PBLogMessages
  {
    public static void Load(pulse.cdm.bind.LogMessagesData src, SELogMessages dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.LogMessagesData src, SELogMessages dst)
    {
      dst.Clear();
      foreach (var msg in src.DebugMessages)
        dst.debug_msgs.Add(msg);
      foreach (var msg in src.InfoMessages)
        dst.info_msgs.Add(msg);
      foreach (var msg in src.WarningMessages)
        dst.warning_msgs.Add(msg);
      foreach (var msg in src.ErrorMessages)
        dst.error_msgs.Add(msg);
      foreach (var msg in src.FatalMessages)
        dst.fatal_msgs.Add(msg);
    }
    public static bool SerializeFromString(string src, SELogMessages dst)
    {
      try
      {
        pulse.cdm.bind.LogMessagesData data = JsonParser.Default.Parse<pulse.cdm.bind.LogMessagesData>(src);
        PBLogMessages.Load(data, dst);
      }
      catch (Google.Protobuf.InvalidJsonException)
      {
        return false;
      }
      return true;
    }

    public static pulse.cdm.bind.LogMessagesData Unload(SELogMessages src)
    {
      pulse.cdm.bind.LogMessagesData dst = new pulse.cdm.bind.LogMessagesData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SELogMessages src, pulse.cdm.bind.LogMessagesData dst)
    {
      foreach (string msg in src.debug_msgs)
        dst.DebugMessages.Add(msg);
      foreach (string msg in src.info_msgs)
        dst.InfoMessages.Add(msg);
      foreach (string msg in src.warning_msgs)
        dst.WarningMessages.Add(msg);
      foreach (string msg in src.error_msgs)
        dst.ErrorMessages.Add(msg);
      foreach (string msg in src.fatal_msgs)
        dst.FatalMessages.Add(msg);
    }
    public static string SerializeToString(SELogMessages src)
    {
      var pb = PBLogMessages.Unload(src);
      return pb.ToString();
    }

  }
}
