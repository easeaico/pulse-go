/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using System.Collections.Generic;

namespace Pulse.CDM
{
  public enum eSurroundingType : int
  {
    NullSurrounding = 0,
    Air,
    Water
  }

  public class SEEnvironmentalConditions
  {
    protected eSurroundingType               surrounding_type;
    protected SEScalarMassPerVolume          air_density;
    protected SEScalarLengthPerTime          air_velocity;
    protected SEScalarTemperature            ambient_temperature;
    protected SEScalarPressure               atmospheric_pressure;
    protected SEScalarHeatResistanceArea     clothing_resistance;
    protected SEScalar0To1                   emissivity;
    protected SEScalarTemperature            mean_radiant_temperature;
    protected SEScalarVolume                 mechanical_dead_space;
    protected SEScalar0To1                   relative_humidity;
    protected SEScalarTemperature            respiration_ambient_temperature;

    protected List<SESubstanceFraction>      ambient_gases;
    protected List<SESubstanceConcentration> ambient_aerosols;

    public SEEnvironmentalConditions()
    {
      this.surrounding_type = 0;

      this.air_density = null;
      this.air_velocity = null;
      this.ambient_temperature = null;
      this.atmospheric_pressure = null;
      this.clothing_resistance = null;
      this.emissivity = null;
      this.mean_radiant_temperature = null;
      this.mechanical_dead_space = null;
      this.relative_humidity = null;
      this.respiration_ambient_temperature = null;

      this.ambient_gases = new List<SESubstanceFraction>();
      this.ambient_aerosols = new List<SESubstanceConcentration>();
    }

    public void Clear()
    {
      surrounding_type = 0;
      if (air_density != null)
        air_density.Invalidate();
      if (air_velocity != null)
        air_velocity.Invalidate();
      if (ambient_temperature != null)
        ambient_temperature.Invalidate();
      if (atmospheric_pressure != null)
        atmospheric_pressure.Invalidate();
      if (clothing_resistance != null)
        clothing_resistance.Invalidate();
      if (emissivity != null)
        emissivity.Invalidate();
      if (mean_radiant_temperature != null)
        mean_radiant_temperature.Invalidate();
      if (mechanical_dead_space != null)
        mechanical_dead_space.Invalidate();
      if (relative_humidity != null)
        relative_humidity.Invalidate();
      if (respiration_ambient_temperature != null)
        respiration_ambient_temperature.Invalidate();

      ambient_gases.Clear();
      ambient_aerosols.Clear();
    }

    public void Copy(SEEnvironmentalConditions from)
    {
      Clear();
      if (from.surrounding_type != eSurroundingType.NullSurrounding)
        this.SetSurroundingType(from.surrounding_type);
      if (from.HasAirDensity())
        this.GetAirDensity().Set(from.GetAirDensity());
      if (from.HasAirVelocity())
        this.GetAirVelocity().Set(from.GetAirVelocity());
      if (from.HasAmbientTemperature())
        this.GetAmbientTemperature().Set(from.GetAmbientTemperature());
      if (from.HasAtmosphericPressure())
        this.GetAtmosphericPressure().Set(from.GetAtmosphericPressure());
      if (from.HasClothingResistance())
        this.GetClothingResistance().Set(from.GetClothingResistance());
      if (from.HasEmissivity())
        this.GetEmissivity().Set(from.GetEmissivity());
      if (from.HasMeanRadiantTemperature())
        this.GetMeanRadiantTemperature().Set(from.GetMeanRadiantTemperature());
      if (from.HasRelativeHumidity())
        this.GetRelativeHumidity().Set(from.GetRelativeHumidity());
      if (from.HasRespirationAmbientTemperature())
        this.GetRespirationAmbientTemperature().Set(from.GetRespirationAmbientTemperature());

      if (from.ambient_gases != null)
      {
        SESubstanceFraction mine;
        foreach (SESubstanceFraction sf in from.ambient_gases)
        {
          mine = this.CreateAmbientGas(sf.GetSubstance());
          if (sf.HasFractionAmount())
            mine.GetFractionAmount().Set(sf.GetFractionAmount());
        }
      }

      if (from.ambient_aerosols != null)
      {
        SESubstanceConcentration mine;
        foreach (SESubstanceConcentration sc in from.ambient_aerosols)
        {
          mine = this.CreateAmbientAerosol(sc.GetSubstance());
          if (sc.HasConcentration())
            mine.GetConcentration().Set(sc.GetConcentration());
        }
      }
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
        System.Console.WriteLine("Error Reading Envronment File! " + ex.ToString());
        return false;
      }
    }
    public bool SerializeFromString(string src, eSerializationFormat fmt)
    {
      return PBEnvironment.SerializeFromString(src, this);
    }

    public bool SerializeToFile(string filename)
    {
      string json = SerializeToString(eSerializationFormat.JSON);
      if (json == null || json.Length == 0)
        return false;
      try
      {
        System.IO.File.WriteAllText(filename, json);
      }
      catch (System.Exception ex)
      {
        System.Console.WriteLine("Error Writing Patient File! " + ex.ToString());
        return false;
      }
      return true;
    }
    public string SerializeToString(eSerializationFormat format)
    {
      return PBEnvironment.SerializeToString(this);
    }

    public bool HasSurroundingType()
    {
      return surrounding_type != 0;
    }
    public eSurroundingType GetSurroundingType()
    {
      return surrounding_type;
    }
    public void SetSurroundingType(eSurroundingType st)
    {
      surrounding_type = st;
    }

    public SEScalarMassPerVolume GetAirDensity()
    {
      if (air_density == null)
        air_density = new SEScalarMassPerVolume();
      return air_density;
    }
    public bool HasAirDensity()
    {
      return air_density == null ? false : air_density.IsValid();
    }

    public SEScalarLengthPerTime GetAirVelocity()
    {
      if (air_velocity == null)
        air_velocity = new SEScalarLengthPerTime();
      return air_velocity;
    }
    public bool HasAirVelocity()
    {
      return air_velocity == null ? false : air_velocity.IsValid();
    }

    public SEScalarTemperature GetAmbientTemperature()
    {
      if (ambient_temperature == null)
        ambient_temperature = new SEScalarTemperature();
      return ambient_temperature;
    }
    public bool HasAmbientTemperature()
    {
      return ambient_temperature == null ? false : ambient_temperature.IsValid();
    }

    public SEScalarPressure GetAtmosphericPressure()
    {
      if (atmospheric_pressure == null)
        atmospheric_pressure = new SEScalarPressure();
      return atmospheric_pressure;
    }
    public bool HasAtmosphericPressure()
    {
      return atmospheric_pressure == null ? false : atmospheric_pressure.IsValid();
    }

    public SEScalarHeatResistanceArea GetClothingResistance()
    {
      if (clothing_resistance == null)
        clothing_resistance = new SEScalarHeatResistanceArea();
      return clothing_resistance;
    }
    public bool HasClothingResistance()
    {
      return clothing_resistance == null ? false : clothing_resistance.IsValid();
    }

    public SEScalar0To1 GetEmissivity()
    {
      if (emissivity == null)
        emissivity = new SEScalar0To1();
      return emissivity;
    }
    public bool HasEmissivity()
    {
      return emissivity == null ? false : emissivity.IsValid();
    }

    public SEScalarTemperature GetMeanRadiantTemperature()
    {
      if (mean_radiant_temperature == null)
        mean_radiant_temperature = new SEScalarTemperature();
      return mean_radiant_temperature;
    }
    public bool HasMeanRadiantTemperature()
    {
      return mean_radiant_temperature == null ? false : mean_radiant_temperature.IsValid();
    }

    public SEScalarVolume GetMechanicalDeadSpace()
    {
      if (mechanical_dead_space == null)
        mechanical_dead_space = new SEScalarVolume();
      return mechanical_dead_space;
    }
    public bool HasMechanicalDeadSpace()
    {
      return mechanical_dead_space == null ? false : mechanical_dead_space.IsValid();
    }

    public SEScalar0To1 GetRelativeHumidity()
    {
      if (relative_humidity == null)
        relative_humidity = new SEScalar0To1();
      return relative_humidity;
    }
    public bool HasRelativeHumidity()
    {
      return relative_humidity == null ? false : relative_humidity.IsValid();
    }

    public SEScalarTemperature GetRespirationAmbientTemperature()
    {
      if (respiration_ambient_temperature == null)
        respiration_ambient_temperature = new SEScalarTemperature();
      return respiration_ambient_temperature;
    }
    public bool HasRespirationAmbientTemperature()
    {
      return respiration_ambient_temperature == null ? false : respiration_ambient_temperature.IsValid();
    }

    public SESubstanceFraction CreateAmbientGas(string substance)
    {
      return GetAmbientGas(substance);
    }
    public SESubstanceFraction GetAmbientGas(string substance)
    {
      foreach (SESubstanceFraction sf in ambient_gases)
      {
        if (sf.GetSubstance() == substance)
        {
          return sf;
        }
      }
      SESubstanceFraction nsf = new SESubstanceFraction(substance);
      ambient_gases.Add(nsf);
      return nsf;
    }
    public bool HasAmbientGas()
    {
      return ambient_gases.Count != 0;
    }
    public bool HasAmbientGas(string substance)
    {
      foreach (SESubstanceFraction sf in ambient_gases)
      {
        if (sf.GetSubstance() == substance)
        {
          return true;
        }
      }
      return false;
    }
    public List<SESubstanceFraction> GetAmbientGases()
    {
      return ambient_gases;
    }
    public void RemoveAmbientGas(string substance)
    {
      foreach (SESubstanceFraction sf in ambient_gases)
      {
        if (sf.GetSubstance() == substance)
        {
          ambient_gases.Remove(sf);
          return;
        }
      }
    }
    public void RemoveAmbientGases()
    {
      ambient_gases.Clear();
    }

    public SESubstanceConcentration CreateAmbientAerosol(string substance)
    {
      return GetAmbientAerosol(substance);
    }
    public SESubstanceConcentration GetAmbientAerosol(string substance)
    {
      foreach (SESubstanceConcentration sc in ambient_aerosols)
      {
        if (sc.GetSubstance() == substance)
        {
          return sc;
        }
      }
      SESubstanceConcentration nsc = new SESubstanceConcentration(substance);
      ambient_aerosols.Add(nsc);
      return nsc;
    }
    public bool HasAmbientAerosol()
    {
      return ambient_aerosols.Count != 0;
    }
    public bool HasAmbientAerosol(string substance)
    {
      foreach (SESubstanceConcentration sc in ambient_aerosols)
      {
        if (sc.GetSubstance() == substance)
        {
          return true;
        }
      }
      return false;
    }
    public List<SESubstanceConcentration> GetAmbientAerosols()
    {
      return ambient_aerosols;
    }
    public void RemoveAmbientAerosol(string substance)
    {
      foreach (SESubstanceConcentration sc in ambient_aerosols)
      {
        if (sc.GetSubstance() == substance)
        {
          ambient_aerosols.Remove(sc);
          return;
        }
      }
    }
    public void RemoveAmbientAerosols()
    {
      ambient_aerosols.Clear();
    }

    public new string ToString()
    {
      string str = "Envriomental Conditions:"
          + "\n\tSurroundingType: " + GetSurroundingType()
          + "\n\tAirDensity: " + (HasAirDensity() ? GetAirDensity().ToString() : "None")
          + "\n\tAirVelocity: " + (HasAirVelocity() ? GetAirVelocity().ToString() : "None")
          + "\n\tAmbientTemperature: " + (HasAmbientTemperature() ? GetAmbientTemperature().ToString() : "None")
          + "\n\tAtmosphericPressure: " + (HasAtmosphericPressure() ? GetAtmosphericPressure().ToString() : "None")
          + "\n\tClothingResistance: " + (HasClothingResistance() ? GetClothingResistance().ToString() : "None")
          + "\n\tEmissivity: " + (HasEmissivity() ? GetEmissivity().ToString() : "None")
          + "\n\tMeanRadiantTemperature: " + (HasMeanRadiantTemperature() ? GetMeanRadiantTemperature().ToString() : "None")
          + "\n\tRelativeHumidity: " + (HasRelativeHumidity() ? GetRelativeHumidity().ToString() : "None")
          + "\n\tRespirationAmbientTemperature: " + (HasRespirationAmbientTemperature() ? GetRespirationAmbientTemperature().ToString() : "None");
      foreach (SESubstanceFraction sf in this.ambient_gases)
        str += "\n\t" + sf.GetSubstance();
      foreach (SESubstanceConcentration sc in this.ambient_aerosols)
        str += "\n\t" + sc.GetSubstance();

      return str;
    }

  }
}
