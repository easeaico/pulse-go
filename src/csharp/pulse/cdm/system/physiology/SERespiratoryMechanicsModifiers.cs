/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using System.Collections;
using System.Collections.Generic;

namespace Pulse.CDM
{
  using LungVolumeIncrementMap = Dictionary<eLungCompartment, SEScalarVolume>;
  public class SERespiratoryMechanicsModifiers
  {
    protected SEScalarUnsigned left_compliance_multiplier;
    protected SEScalarUnsigned right_compliance_multiplier;

    protected SEScalarUnsigned left_expiratory_resistance_multiplier;
    protected SEScalarUnsigned left_inspiratory_resistance_multiplier;
    protected SEScalarUnsigned right_expiratory_resistance_multiplier;
    protected SEScalarUnsigned right_inspiratory_resistance_multiplier;
    protected SEScalarUnsigned upper_expiratory_resistance_multiplier;
    protected SEScalarUnsigned upper_inspiratory_resistance_multiplier;

    protected SEScalarUnsigned respiration_rate_multiplier;
    protected SEScalarUnsigned tidal_volume_multiplier;

    protected LungVolumeIncrementMap volume_increments;

    public SERespiratoryMechanicsModifiers()
    {
      left_compliance_multiplier = null;
      right_compliance_multiplier = null;
      left_expiratory_resistance_multiplier = null;
      left_inspiratory_resistance_multiplier = null;
      right_expiratory_resistance_multiplier = null;
      right_inspiratory_resistance_multiplier = null;
      upper_expiratory_resistance_multiplier = null;
      upper_inspiratory_resistance_multiplier = null;
      respiration_rate_multiplier = null;
      tidal_volume_multiplier = null;

      volume_increments = new LungVolumeIncrementMap();
    }

    public void Clear()
    {
      if (left_compliance_multiplier != null)
        left_compliance_multiplier.Invalidate();
      if (right_compliance_multiplier != null)
        right_compliance_multiplier.Invalidate();
      if (left_expiratory_resistance_multiplier != null)
        left_expiratory_resistance_multiplier.Invalidate();
      if (left_inspiratory_resistance_multiplier != null)
        left_inspiratory_resistance_multiplier.Invalidate();
      if (right_expiratory_resistance_multiplier != null)
        right_expiratory_resistance_multiplier.Invalidate();
      if (right_inspiratory_resistance_multiplier != null)
        right_inspiratory_resistance_multiplier.Invalidate();
      if (upper_expiratory_resistance_multiplier != null)
        upper_expiratory_resistance_multiplier.Invalidate();
      if (upper_inspiratory_resistance_multiplier != null)
        upper_inspiratory_resistance_multiplier.Invalidate();
      if (respiration_rate_multiplier != null)
        respiration_rate_multiplier.Invalidate();
      if (tidal_volume_multiplier != null)
        tidal_volume_multiplier.Invalidate();

      volume_increments.Clear();
    }

    public void Copy(SERespiratoryMechanicsModifiers from)
    {
      Clear();
      if (from.HasLeftComplianceMultiplier())
        GetLeftComplianceMultiplier().Copy(from.GetLeftComplianceMultiplier());
      if (from.HasRightComplianceMultiplier())
        GetRightComplianceMultiplier().Copy(from.GetRightComplianceMultiplier());
      if (from.HasLeftExpiratoryResistanceMultiplier())
        GetLeftExpiratoryResistanceMultiplier().Copy(from.GetLeftExpiratoryResistanceMultiplier());
      if (from.HasLeftInspiratoryResistanceMultiplier())
        GetLeftInspiratoryResistanceMultiplier().Copy(from.GetLeftInspiratoryResistanceMultiplier());
      if (from.HasRightExpiratoryResistanceMultiplier())
        GetRightExpiratoryResistanceMultiplier().Copy(from.GetRightExpiratoryResistanceMultiplier());
      if (from.HasRightInspiratoryResistanceMultiplier())
        GetRightInspiratoryResistanceMultiplier().Copy(from.GetRightInspiratoryResistanceMultiplier());
      if (from.HasUpperExpiratoryResistanceMultiplier())
        GetUpperExpiratoryResistanceMultiplier().Copy(from.GetUpperExpiratoryResistanceMultiplier());
      if (from.HasUpperInspiratoryResistanceMultiplier())
        GetUpperInspiratoryResistanceMultiplier().Copy(from.GetUpperInspiratoryResistanceMultiplier());
      if (from.HasRespirationRateMultiplier())
        GetRespirationRateMultiplier().Copy(from.GetRespirationRateMultiplier());
      if (from.HasTidalVolumeMultiplier())
        GetTidalVolumeMultiplier().Copy(from.GetTidalVolumeMultiplier());
    }

    public bool HasLeftComplianceMultiplier()
    {
      if (left_compliance_multiplier != null)
        return left_compliance_multiplier.IsValid();
      return false;
    }
    public SEScalarUnsigned GetLeftComplianceMultiplier()
    {
      if (left_compliance_multiplier == null)
        left_compliance_multiplier = new SEScalarUnsigned();
      return left_compliance_multiplier;
    }

    public bool HasRightComplianceMultiplier()
    {
      if (right_compliance_multiplier != null)
        return right_compliance_multiplier.IsValid();
      return false;
    }
    public SEScalarUnsigned GetRightComplianceMultiplier()
    {
      if (right_compliance_multiplier == null)
        right_compliance_multiplier = new SEScalarUnsigned();
      return right_compliance_multiplier;
    }

    public bool HasLeftExpiratoryResistanceMultiplier()
    {
      if (left_expiratory_resistance_multiplier != null)
        return left_expiratory_resistance_multiplier.IsValid();
      return false;
    }
    public SEScalarUnsigned GetLeftExpiratoryResistanceMultiplier()
    {
      if (left_expiratory_resistance_multiplier == null)
        left_expiratory_resistance_multiplier = new SEScalarUnsigned();
      return left_expiratory_resistance_multiplier;
    }

    public bool HasLeftInspiratoryResistanceMultiplier()
    {
      if (left_inspiratory_resistance_multiplier != null)
        return left_inspiratory_resistance_multiplier.IsValid();
      return false;
    }
    public SEScalarUnsigned GetLeftInspiratoryResistanceMultiplier()
    {
      if (left_inspiratory_resistance_multiplier == null)
        left_inspiratory_resistance_multiplier = new SEScalarUnsigned();
      return left_inspiratory_resistance_multiplier;
    }

    public bool HasRightExpiratoryResistanceMultiplier()
    {
      if (right_expiratory_resistance_multiplier != null)
        return right_expiratory_resistance_multiplier.IsValid();
      return false;
    }
    public SEScalarUnsigned GetRightExpiratoryResistanceMultiplier()
    {
      if (right_expiratory_resistance_multiplier == null)
        right_expiratory_resistance_multiplier = new SEScalarUnsigned();
      return right_expiratory_resistance_multiplier;
    }

    public bool HasRightInspiratoryResistanceMultiplier()
    {
      if (right_inspiratory_resistance_multiplier != null)
        return right_inspiratory_resistance_multiplier.IsValid();
      return false;
    }
    public SEScalarUnsigned GetRightInspiratoryResistanceMultiplier()
    {
      if (right_inspiratory_resistance_multiplier == null)
        right_inspiratory_resistance_multiplier = new SEScalarUnsigned();
      return right_inspiratory_resistance_multiplier;
    }

    public bool HasUpperExpiratoryResistanceMultiplier()
    {
      if (upper_expiratory_resistance_multiplier != null)
        return upper_expiratory_resistance_multiplier.IsValid();
      return false;
    }
    public SEScalarUnsigned GetUpperExpiratoryResistanceMultiplier()
    {
      if (upper_expiratory_resistance_multiplier == null)
        upper_expiratory_resistance_multiplier = new SEScalarUnsigned();
      return upper_expiratory_resistance_multiplier;
    }

    public bool HasUpperInspiratoryResistanceMultiplier()
    {
      if (upper_inspiratory_resistance_multiplier != null)
        return upper_inspiratory_resistance_multiplier.IsValid();
      return false;
    }
    public SEScalarUnsigned GetUpperInspiratoryResistanceMultiplier()
    {
      if (upper_inspiratory_resistance_multiplier == null)
        upper_inspiratory_resistance_multiplier = new SEScalarUnsigned();
      return upper_inspiratory_resistance_multiplier;
    }

    public bool HasRespirationRateMultiplier()
    {
      if (respiration_rate_multiplier != null)
        return respiration_rate_multiplier.IsValid();
      return false;
    }
    public SEScalarUnsigned GetRespirationRateMultiplier()
    {
      if (respiration_rate_multiplier == null)
        respiration_rate_multiplier = new SEScalarUnsigned();
      return respiration_rate_multiplier;
    }

    public bool HasTidalVolumeMultiplier()
    {
      if (tidal_volume_multiplier != null)
        return tidal_volume_multiplier.IsValid();
      return false;
    }
    public SEScalarUnsigned GetTidalVolumeMultiplier()
    {
      if (tidal_volume_multiplier == null)
        tidal_volume_multiplier = new SEScalarUnsigned();
      return tidal_volume_multiplier;
    }

    public bool HasLungVolumeIncrement()
    {
      foreach (var itr in volume_increments)
        if (itr.Value.IsValid())
          return true;
      return false;
    }
    public bool HasLungVolumeIncrement(eLungCompartment c)
    {
      if (!volume_increments.ContainsKey(c))
        return false;
      return volume_increments[c].IsValid();
    }
    public SEScalarVolume GetLungVolumeIncrement(eLungCompartment c)
    {
      SEScalarVolume s = volume_increments[c];
      if (s == null)
      {
        s = new SEScalarVolume();
        volume_increments[c] = s;
      }
      return s;
    }
    public LungVolumeIncrementMap GetLungVolumeIncrements()
    {
      return volume_increments;
    }

    public override string ToString()
    {
      string str = "Respiratory Mechanics Modifiers";
      str += "\n\tLeftComplianceMultiplier: "; if (HasLeftComplianceMultiplier()) str += left_compliance_multiplier.ToString(); else str += "Not Set";
      str += "\n\tRightComplianceMultiplier: "; if (HasRightComplianceMultiplier()) str += right_compliance_multiplier.ToString(); else str += "Not Set";
      str += "\n\tLeftExpiratoryResistanceMultiplier: "; if (HasLeftExpiratoryResistanceMultiplier()) str += left_expiratory_resistance_multiplier.ToString(); else str += "Not Set";
      str += "\n\tLeftInspiratoryResistanceMultiplier: "; if (HasLeftInspiratoryResistanceMultiplier()) str += left_inspiratory_resistance_multiplier.ToString(); else str += "Not Set";
      str += "\n\tRightExpiratoryResistanceMultiplier: "; if (HasRightExpiratoryResistanceMultiplier()) str += right_expiratory_resistance_multiplier.ToString(); else str += "Not Set";
      str += "\n\tRightInspiratoryResistanceMultiplier: "; if (HasRightInspiratoryResistanceMultiplier()) str += right_inspiratory_resistance_multiplier.ToString(); else str += "Not Set";
      str += "\n\tUpperExpiratoryResistanceMultiplier: "; if (HasUpperExpiratoryResistanceMultiplier()) str += upper_expiratory_resistance_multiplier.ToString(); else str += "Not Set";
      str += "\n\tUpperInspiratoryResistanceMultiplier: "; if (HasUpperInspiratoryResistanceMultiplier()) str += upper_inspiratory_resistance_multiplier.ToString(); else str += "Not Set";
      str += "\n\tRespirationRateMultiplier: "; if (HasRespirationRateMultiplier()) str += respiration_rate_multiplier.ToString(); else str += "Not Set";
      str += "\n\tTidalVolumeMultiplier: "; if (HasTidalVolumeMultiplier()) str += tidal_volume_multiplier.ToString(); else str += "Not Set";

      foreach (KeyValuePair<eLungCompartment, SEScalarVolume> i in volume_increments)
        str += "\n\tIncrement: " + i.Key.ToString() + " Volume by " + i.Value.ToString();
      return str;
    }

  }
}
