/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using System.Collections.Generic;
using Google.Protobuf;

namespace Pulse.CDM
{
  public class PBPatientConfiguration
  {
    #region SEAction
    public static void Load(pulse.cdm.bind.PatientConfigurationData src, SEPatientConfiguration dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.PatientConfigurationData src, SEPatientConfiguration dst)
    {
      if (!string.IsNullOrEmpty(src.PatientFile))
        dst.SetPatientFile(src.PatientFile);
      else if (src.Patient != null)
        PBPatient.Serialize(src.Patient, dst.GetPatient());
      if (src.Conditions != null)
        Serialize(src.Conditions, dst.GetConditions());
      dst.SetDataRootDir(src.DataRoot);
    }
    public static bool SerializeFromString(string src, SEPatientConfiguration dst)
    {
      try
      {
        pulse.cdm.bind.PatientConfigurationData data = JsonParser.Default.Parse<pulse.cdm.bind.PatientConfigurationData>(src);
        PBPatientConfiguration.Load(data, dst);
      }
      catch (Google.Protobuf.InvalidJsonException)
      {
        return false;
      }
      return true;
    }

    public static pulse.cdm.bind.PatientConfigurationData Unload(SEPatientConfiguration src)
    {
      pulse.cdm.bind.PatientConfigurationData dst = new pulse.cdm.bind.PatientConfigurationData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEPatientConfiguration src, pulse.cdm.bind.PatientConfigurationData dst)
    {
      if (src.HasPatientFile())
        dst.PatientFile = src.GetPatientFile();
      else if (src.HasPatient())
        dst.Patient = PBPatient.Unload(src.GetPatient());
      if (src.HasConditions())
        dst.Conditions = PBPatientConfiguration.Unload(src.GetConditions());
      dst.DataRoot = src.GetDataRootDir();
    }
    public static string SerializeToString(SEPatientConfiguration src)
    {
      var pb = PBPatientConfiguration.Unload(src);
      return pb.ToString();
    }
    #endregion

    #region SEConditionManager
    public static void Load(pulse.cdm.bind.ConditionListData src, SEConditionManager dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.ConditionListData src, SEConditionManager dst)
    {
      foreach (var condition in src.AnyCondition)
      {
        if (!dst.Add(PBCondition.Load(condition)))
          System.Console.WriteLine("Failed to serialize an unsupported condition " + condition.ToString());
      }
    }
    public static pulse.cdm.bind.ConditionListData Unload(SEConditionManager src)
    {
      pulse.cdm.bind.ConditionListData dst = new pulse.cdm.bind.ConditionListData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEConditionManager src, pulse.cdm.bind.ConditionListData dst)
    {
      if (src.HasAcuteRespiratoryDistressSyndrome())
        dst.AnyCondition.Add(PBCondition.Unload(src.GetAcuteRespiratoryDistressSyndrome()));
      if (src.HasChronicAnemia())
        dst.AnyCondition.Add(PBCondition.Unload(src.GetChronicAnemia()));
      if (src.HasChronicObstructivePulmonaryDisease())
        dst.AnyCondition.Add(PBCondition.Unload(src.GetChronicObstructivePulmonaryDisease()));
      if (src.HasChronicPericardialEffusion())
        dst.AnyCondition.Add(PBCondition.Unload(src.GetChronicPericardialEffusion()));
      if (src.HasChronicRenalStenosis())
        dst.AnyCondition.Add(PBCondition.Unload(src.GetChronicRenalStenosis()));
      if (src.HasChronicVentricularSystolicDysfunction())
        dst.AnyCondition.Add(PBCondition.Unload(src.GetChronicVentricularSystolicDysfunction()));
      if (src.HasDehydration())
        dst.AnyCondition.Add(PBCondition.Unload(src.GetDehydration()));
      if (src.HasImpairedAlveolarExchange())
        dst.AnyCondition.Add(PBCondition.Unload(src.GetImpairedAlveolarExchange()));
      if (src.HasPneumonia())
        dst.AnyCondition.Add(PBCondition.Unload(src.GetPneumonia()));
      if (src.HasSepsis())
        dst.AnyCondition.Add(PBCondition.Unload(src.GetSepsis()));

      if (src.HasInitialEnvironmentalConditions())
        dst.AnyCondition.Add(PBCondition.Unload(src.GetInitialEnvironmentalConditions()));
    }
    #endregion
  }
}
