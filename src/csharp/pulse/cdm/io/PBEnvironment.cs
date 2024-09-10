/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using System.Collections.Generic;
using Google.Protobuf;

namespace Pulse.CDM
{
  public class PBEnvironment
  {
    #region SEActiveConditioning

    public static void Load(pulse.cdm.bind.ActiveConditioningData src, SEActiveConditioning dst)
    {
      dst.Clear();
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.ActiveConditioningData src, SEActiveConditioning dst)
    {
      if(src.Power != null)
        PBProperty.Load(src.Power, dst.GetPower());
      if (src.SurfaceArea != null)
        PBProperty.Load(src.SurfaceArea, dst.GetSurfaceArea());
      if (src.SurfaceAreaFraction != null)
        PBProperty.Load(src.SurfaceAreaFraction, dst.GetSurfaceAreaFraction());
    }

    public static pulse.cdm.bind.ActiveConditioningData Unload(SEActiveConditioning src)
    {
      pulse.cdm.bind.ActiveConditioningData dst = new pulse.cdm.bind.ActiveConditioningData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEActiveConditioning src, pulse.cdm.bind.ActiveConditioningData dst)
    {
      if (src.HasPower())
        dst.Power = PBProperty.Unload(src.GetPower());
      if (src.HasSurfaceArea())
        dst.SurfaceArea = PBProperty.Unload(src.GetSurfaceArea());
      if (src.HasSurfaceAreaFraction())
        dst.SurfaceAreaFraction = PBProperty.Unload(src.GetSurfaceAreaFraction());
    }

    #endregion

    #region SEAppliedTemperature

    public static void Load(pulse.cdm.bind.AppliedTemperatureData src, SEAppliedTemperature dst)
    {
      dst.Clear();
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.AppliedTemperatureData src, SEAppliedTemperature dst)
    {
      dst.SetState((eSwitch)(int)src.State);
      if (src.Temperature != null)
        PBProperty.Load(src.Temperature, dst.GetTemperature());
      if (src.SurfaceArea != null)
        PBProperty.Load(src.SurfaceArea, dst.GetSurfaceArea());
      if (src.SurfaceAreaFraction != null)
        PBProperty.Load(src.SurfaceAreaFraction, dst.GetSurfaceAreaFraction());
    }

    public static pulse.cdm.bind.AppliedTemperatureData Unload(SEAppliedTemperature src)
    {
      pulse.cdm.bind.AppliedTemperatureData dst = new pulse.cdm.bind.AppliedTemperatureData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEAppliedTemperature src, pulse.cdm.bind.AppliedTemperatureData dst)
    {
      dst.State = (pulse.cdm.bind.eSwitch)(int)src.GetState();
      if (src.HasTemperature())
        dst.Temperature = PBProperty.Unload(src.GetTemperature());
      if (src.HasSurfaceArea())
        dst.SurfaceArea = PBProperty.Unload(src.GetSurfaceArea());
      if (src.HasSurfaceAreaFraction())
        dst.SurfaceAreaFraction = PBProperty.Unload(src.GetSurfaceAreaFraction());
    }

    #endregion

    #region SEEnvironmentalConditions

    public static void Load(pulse.cdm.bind.EnvironmentalConditionsData src, SEEnvironmentalConditions dst)
    {
      dst.Clear();
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.EnvironmentalConditionsData src, SEEnvironmentalConditions dst)
    {
      dst.SetSurroundingType((eSurroundingType)(int)src.SurroundingType);
      if (src.AirDensity != null)
        PBProperty.Load(src.AirDensity, dst.GetAirDensity());
      if (src.AirVelocity != null)
        PBProperty.Load(src.AirVelocity, dst.GetAirVelocity());
      if (src.AmbientTemperature != null)
        PBProperty.Load(src.AmbientTemperature, dst.GetAmbientTemperature());
      if (src.AtmosphericPressure != null)
        PBProperty.Load(src.AtmosphericPressure, dst.GetAtmosphericPressure());
      if (src.ClothingResistance != null)
        PBProperty.Load(src.ClothingResistance, dst.GetClothingResistance());
      if (src.Emissivity != null)
        PBProperty.Load(src.Emissivity, dst.GetEmissivity());
      if (src.MeanRadiantTemperature != null)
        PBProperty.Load(src.MeanRadiantTemperature, dst.GetMeanRadiantTemperature());
      if (src.MechanicalDeadSpace != null)
        PBProperty.Load(src.MechanicalDeadSpace, dst.GetMechanicalDeadSpace());
      if (src.RelativeHumidity != null)
        PBProperty.Load(src.RelativeHumidity, dst.GetRelativeHumidity());
      if (src.RespirationAmbientTemperature != null)
        PBProperty.Load(src.RespirationAmbientTemperature, dst.GetRespirationAmbientTemperature());

      foreach (pulse.cdm.bind.SubstanceFractionData sf in src.AmbientGas)
      {
        PBProperty.Load(sf.Amount, dst.GetAmbientGas(sf.Name).GetFractionAmount());
      }

      foreach (pulse.cdm.bind.SubstanceConcentrationData sc in src.AmbientAerosol)
      {
        PBProperty.Load(sc.Concentration, dst.GetAmbientAerosol(sc.Name).GetConcentration());
      }
    }

    public static pulse.cdm.bind.EnvironmentalConditionsData Unload(SEEnvironmentalConditions src)
    {
      pulse.cdm.bind.EnvironmentalConditionsData dst = new pulse.cdm.bind.EnvironmentalConditionsData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEEnvironmentalConditions src, pulse.cdm.bind.EnvironmentalConditionsData dst)
    {
      dst.SurroundingType = (pulse.cdm.bind.EnvironmentalConditionsData.Types.eSurroundingType)(int)src.GetSurroundingType();
      if (src.HasAirDensity())
        dst.AirDensity = PBProperty.Unload(src.GetAirDensity());
      if (src.HasAirVelocity())
        dst.AirVelocity = PBProperty.Unload(src.GetAirVelocity());
      if (src.HasAmbientTemperature())
        dst.AmbientTemperature = PBProperty.Unload(src.GetAmbientTemperature());
      if (src.HasAtmosphericPressure())
        dst.AtmosphericPressure = PBProperty.Unload(src.GetAtmosphericPressure());
      if (src.HasClothingResistance())
        dst.ClothingResistance = PBProperty.Unload(src.GetClothingResistance());
      if (src.HasEmissivity())
        dst.Emissivity = PBProperty.Unload(src.GetEmissivity());
      if (src.HasMeanRadiantTemperature())
        dst.MeanRadiantTemperature = PBProperty.Unload(src.GetMeanRadiantTemperature());
      if (src.HasMechanicalDeadSpace())
        dst.MechanicalDeadSpace = PBProperty.Unload(src.GetMechanicalDeadSpace());
      if (src.HasRelativeHumidity())
        dst.RelativeHumidity = PBProperty.Unload(src.GetRelativeHumidity());
      if (src.HasRespirationAmbientTemperature())
        dst.RespirationAmbientTemperature = PBProperty.Unload(src.GetRespirationAmbientTemperature());

      foreach (SESubstanceFraction aGas in src.GetAmbientGases())
      {
        pulse.cdm.bind.SubstanceFractionData sf = new pulse.cdm.bind.SubstanceFractionData();
        sf.Name = aGas.GetSubstance();
        sf.Amount = PBProperty.Unload(aGas.GetFractionAmount());
        dst.AmbientGas.Add(sf);
      }

      foreach (SESubstanceConcentration aAerosol in src.GetAmbientAerosols())
      {
        pulse.cdm.bind.SubstanceConcentrationData sc = new pulse.cdm.bind.SubstanceConcentrationData();
        sc.Name = aAerosol.GetSubstance();
        sc.Concentration = PBProperty.Unload(aAerosol.GetConcentration());
        dst.AmbientAerosol.Add(sc);
      }

    }
    
    public static bool SerializeFromString(string src, SEEnvironmentalConditions dst)
    {
      try
      {
        pulse.cdm.bind.EnvironmentalConditionsData data = JsonParser.Default.Parse<pulse.cdm.bind.EnvironmentalConditionsData>(src);
        PBEnvironment.Load(data, dst);
      }
      catch (Google.Protobuf.InvalidJsonException)
      {
        return false;
      }
      return true;
    }
    
    public static string SerializeToString(SEEnvironmentalConditions src)
    {
      var pb = PBEnvironment.Unload(src);
      return pb.ToString();
    }

    #endregion

  }
}
