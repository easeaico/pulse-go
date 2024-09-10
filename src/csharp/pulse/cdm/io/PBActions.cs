/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using System.Collections.Generic;
using Google.Protobuf;

namespace Pulse.CDM
{
  public class PBAction
  {
    public static void Load(pulse.cdm.bind.ActionListData src, List<SEAction> dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.ActionListData src, List<SEAction> dst)
    {
      foreach (var action in src.AnyAction)
        dst.Add(PBAction.Load(action));
    }
    public static bool SerializeFromString(string src, List<SEAction> dst)
    {
      try
      {
        pulse.cdm.bind.ActionListData data = JsonParser.Default.Parse<pulse.cdm.bind.ActionListData>(src);
        PBAction.Load(data, dst);
      }
      catch (Google.Protobuf.InvalidJsonException)
      {
        return false;
      }
      return true;
    }

    public static pulse.cdm.bind.ActionListData Unload(List<SEAction> src)
    {
      pulse.cdm.bind.ActionListData dst = new pulse.cdm.bind.ActionListData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(List<SEAction> src, pulse.cdm.bind.ActionListData dst)
    {
      foreach (var action in src)
        dst.AnyAction.Add(PBAction.Unload(action));
    }
    public static string SerializeToString(List<SEAction> src)
    {
      var pb = PBAction.Unload(src);
      return pb.ToString();
    }

    #region AnyAction
    /** Create a new action based on the binding object,
     *  load that data into the new action, and return said action */
    public static SEAction Load(pulse.cdm.bind.AnyActionData action)
    {
      switch (action.ActionCase)
      {
        case pulse.cdm.bind.AnyActionData.ActionOneofCase.PatientAction:
          return PBPatientAction.Load(action.PatientAction);
        case pulse.cdm.bind.AnyActionData.ActionOneofCase.EquipmentAction:
          return PBEquipmentAction.Load(action.EquipmentAction);
        case pulse.cdm.bind.AnyActionData.ActionOneofCase.EnvironmentAction:
            return PBEnvironmentAction.Load(action.EnvironmentAction);
        case pulse.cdm.bind.AnyActionData.ActionOneofCase.AdvanceTime:
          {
            SEAdvanceTime adv = new SEAdvanceTime();
            PBAction.Load(action.AdvanceTime, adv);
            return adv;
          }
        case pulse.cdm.bind.AnyActionData.ActionOneofCase.AdvanceUntilStable:
          {
            SEAdvanceUntilStable adv = new SEAdvanceUntilStable();
            PBAction.Load(action.AdvanceUntilStable, adv);
            return adv;
          }
        case pulse.cdm.bind.AnyActionData.ActionOneofCase.SerializeRequested:
          {
            SESerializeRequested ss = new SESerializeRequested();
            PBAction.Load(action.SerializeRequested, ss);
            return ss;
          }
        case pulse.cdm.bind.AnyActionData.ActionOneofCase.SerializeState:
          {
            SESerializeState ss = new SESerializeState();
            PBAction.Load(action.SerializeState, ss);
            return ss;
          }
      }
      throw new System.InvalidOperationException("Cannot load action " + action.ToString());
    }
    /** Create a new bind object, unload the action,
     *  put that in the bind object, and return said bind object */
    public static pulse.cdm.bind.AnyActionData Unload(SEAction action)
    {
      pulse.cdm.bind.AnyActionData any = new pulse.cdm.bind.AnyActionData();
      if (action.GetType().IsSubclassOf(typeof(SEPatientAction)))
        any.PatientAction = PBPatientAction.Unload((SEPatientAction)action);
      else if (action.GetType().IsSubclassOf(typeof(SEEquipmentAction)))
        any.EquipmentAction = PBEquipmentAction.Unload((SEEquipmentAction)action);
      else if (action.GetType().IsSubclassOf(typeof(SEEnvironmentAction)))
        any.EnvironmentAction = PBEnvironmentAction.Unload((SEEnvironmentAction)action);
      else if (action is SEAdvanceTime)
        any.AdvanceTime = PBAction.Unload((SEAdvanceTime)action);
      else if (action is SEAdvanceUntilStable)
        any.AdvanceUntilStable = PBAction.Unload((SEAdvanceUntilStable)action);
      else if (action is SESerializeRequested)
        any.SerializeRequested = PBAction.Unload((SESerializeRequested)action);
      else if (action is SESerializeState)
        any.SerializeState = PBAction.Unload((SESerializeState)action);
      else
        throw new System.InvalidOperationException("Cannot unload action " + action.ToString());
      return any;
    }
    #endregion

    #region SEAction
    public static void Serialize(pulse.cdm.bind.ActionData src, SEAction dst)
    {
      dst.Clear();
      if (src != null)
      {
        if (src.Comment != null)
          dst.SetComment(src.Comment);
      }
    }

    public static void Serialize(SEAction src, pulse.cdm.bind.ActionData dst)
    {
      if (src.HasComment())
        dst.Comment = src.GetComment();
    }
    public static string SerializeToString(SEAction src)
    {
      var pb = PBAction.Unload(src);
      return pb.ToString();
    }
    #endregion

    #region SEAdvanceTime
    public static void Load(pulse.cdm.bind.AdvanceTimeData src, SEAdvanceTime dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.AdvanceTimeData src, SEAdvanceTime dst)
    {
      Serialize(src.Action, dst);
      if (src.Time != null)
        PBProperty.Load(src.Time, dst.GetTime());
    }
    public static pulse.cdm.bind.AdvanceTimeData Unload(SEAdvanceTime src)
    {
      pulse.cdm.bind.AdvanceTimeData dst = new pulse.cdm.bind.AdvanceTimeData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEAdvanceTime src, pulse.cdm.bind.AdvanceTimeData dst)
    {
      dst.Action = new pulse.cdm.bind.ActionData();
      Serialize(src, dst.Action);
      if (src.HasTime())
        dst.Time = PBProperty.Unload(src.GetTime());
    }
    #endregion

    #region SEAdvanceUntilStable
    public static void Load(pulse.cdm.bind.AdvanceUntilStableData src, SEAdvanceUntilStable dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.AdvanceUntilStableData src, SEAdvanceUntilStable dst)
    {
      Serialize(src.Action, dst);
      dst.SetCriteria(src.Criteria);
    }
    public static pulse.cdm.bind.AdvanceUntilStableData Unload(SEAdvanceUntilStable src)
    {
      pulse.cdm.bind.AdvanceUntilStableData dst = new pulse.cdm.bind.AdvanceUntilStableData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEAdvanceUntilStable src, pulse.cdm.bind.AdvanceUntilStableData dst)
    {
      dst.Action = new pulse.cdm.bind.ActionData();
      Serialize(src, dst.Action);
      dst.Criteria = src.GetCriteria();
    }
    #endregion

    #region SESerializeRequested
    public static void Load(pulse.cdm.bind.SerializeRequestedData src, SESerializeRequested dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.SerializeRequestedData src, SESerializeRequested dst)
    {
      Serialize(src.Action, dst);
      if (!string.IsNullOrEmpty(src.Filename))
        dst.SetFilename(src.Filename);
    }
    public static pulse.cdm.bind.SerializeRequestedData Unload(SESerializeRequested src)
    {
      pulse.cdm.bind.SerializeRequestedData dst = new pulse.cdm.bind.SerializeRequestedData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SESerializeRequested src, pulse.cdm.bind.SerializeRequestedData dst)
    {
      dst.Action = new pulse.cdm.bind.ActionData();
      Serialize(src, dst.Action);
      if (src.HasFilename())
        dst.Filename = src.GetFilename();
    }
    #endregion

    #region SESerializeState
    public static void Load(pulse.cdm.bind.SerializeStateData src, SESerializeState dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.SerializeStateData src, SESerializeState dst)
    {
      Serialize(src.Action, dst);
      if (!string.IsNullOrEmpty(src.Filename))
        dst.SetFilename(src.Filename);
      dst.SetMode((eSerialization_Mode)src.Mode);
    }
    public static pulse.cdm.bind.SerializeStateData Unload(SESerializeState src)
    {
      pulse.cdm.bind.SerializeStateData dst = new pulse.cdm.bind.SerializeStateData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SESerializeState src, pulse.cdm.bind.SerializeStateData dst)
    {
      dst.Action = new pulse.cdm.bind.ActionData();
      Serialize(src, dst.Action);
      if (src.HasFilename())
        dst.Filename = src.GetFilename();
      dst.Mode = (pulse.cdm.bind.SerializeStateData.Types.eMode)src.GetMode();
    }
    #endregion
  }
}