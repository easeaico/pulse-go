/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

namespace Pulse.CDM
{
  public class SECardiovascularMechanicsModifiers
  {
    protected SEScalarUnsigned arterial_compliance_multiplier;
    protected SEScalarUnsigned arterial_resistance_multiplier;
    protected SEScalarUnsigned pulmonary_compliance_multiplier;
    protected SEScalarUnsigned pulmonary_resistance_multiplier;
    protected SEScalarUnsigned systemic_resistance_multiplier;
    protected SEScalarUnsigned systemic_compliance_multiplier;
    protected SEScalarUnsigned venous_compliance_multiplier;
    protected SEScalarUnsigned venous_resistance_multiplier;

    protected SEScalarUnsigned heart_rate_multiplier;
    protected SEScalarUnsigned stroke_volume_multiplier;

    public SECardiovascularMechanicsModifiers()
    {
      arterial_compliance_multiplier = null;
      arterial_resistance_multiplier = null;
      pulmonary_compliance_multiplier = null;
      pulmonary_resistance_multiplier = null;
      systemic_resistance_multiplier = null;
      systemic_compliance_multiplier = null;
      venous_compliance_multiplier = null;
      venous_resistance_multiplier = null;
      heart_rate_multiplier = null;
      stroke_volume_multiplier = null;
    }

    public void Clear()
    {
      if (arterial_compliance_multiplier != null)
        arterial_compliance_multiplier.Invalidate();
      if (arterial_resistance_multiplier != null)
        arterial_resistance_multiplier.Invalidate();
      if (pulmonary_compliance_multiplier != null)
        pulmonary_compliance_multiplier.Invalidate();
      if (pulmonary_resistance_multiplier != null)
        pulmonary_resistance_multiplier.Invalidate();
      if (systemic_resistance_multiplier != null)
        systemic_resistance_multiplier.Invalidate();
      if (systemic_compliance_multiplier != null)
        systemic_compliance_multiplier.Invalidate();
      if (venous_compliance_multiplier != null)
        venous_compliance_multiplier.Invalidate();
      if (venous_resistance_multiplier != null)
        venous_resistance_multiplier.Invalidate();
      if (heart_rate_multiplier != null)
        heart_rate_multiplier.Invalidate();
      if (stroke_volume_multiplier != null)
        stroke_volume_multiplier.Invalidate();
    }

    public void Copy(SECardiovascularMechanicsModifiers from)
    {
      Clear();
      if (from.HasArterialComplianceMultiplier())
        GetArterialComplianceMultiplier().Copy(from.GetArterialComplianceMultiplier());
      if (from.HasArterialResistanceMultiplier())
        GetArterialResistanceMultiplier().Copy(from.GetArterialResistanceMultiplier());
      if (from.HasPulmonaryComplianceMultiplier())
        GetPulmonaryComplianceMultiplier().Copy(from.GetPulmonaryComplianceMultiplier());
      if (from.HasPulmonaryResistanceMultiplier())
        GetPulmonaryResistanceMultiplier().Copy(from.GetPulmonaryResistanceMultiplier());
      if (from.HasSystemicResistanceMultiplier())
        GetSystemicResistanceMultiplier().Copy(from.GetSystemicResistanceMultiplier());
      if (from.HasSystemicComplianceMultiplier())
        GetSystemicComplianceMultiplier().Copy(from.GetSystemicComplianceMultiplier());
      if (from.HasVenousComplianceMultiplier())
        GetVenousComplianceMultiplier().Copy(from.GetVenousComplianceMultiplier());
      if (from.HasVenousResistanceMultiplier())
        GetVenousResistanceMultiplier().Copy(from.GetVenousResistanceMultiplier());
      if (from.HasHeartRateMultiplier())
        GetHeartRateMultiplier().Copy(from.GetHeartRateMultiplier());
      if (from.HasStrokeVolumeMultiplier())
        GetStrokeVolumeMultiplier().Copy(from.GetStrokeVolumeMultiplier());
    }

    public bool HasArterialComplianceMultiplier()
    {
      if (arterial_compliance_multiplier != null)
        return arterial_compliance_multiplier.IsValid();
      return false;
    }
    public SEScalarUnsigned GetArterialComplianceMultiplier()
    {
      if (arterial_compliance_multiplier == null)
        arterial_compliance_multiplier = new SEScalarUnsigned();
      return arterial_compliance_multiplier;
    }

    public bool HasArterialResistanceMultiplier()
    {
      if (arterial_resistance_multiplier != null)
        return arterial_resistance_multiplier.IsValid();
      return false;
    }
    public SEScalarUnsigned GetArterialResistanceMultiplier()
    {
      if (arterial_resistance_multiplier == null)
        arterial_resistance_multiplier = new SEScalarUnsigned();
      return arterial_resistance_multiplier;
    }

    public bool HasPulmonaryComplianceMultiplier()
    {
      if (pulmonary_compliance_multiplier != null)
        return pulmonary_compliance_multiplier.IsValid();
      return false;
    }
    public SEScalarUnsigned GetPulmonaryComplianceMultiplier()
    {
      if (pulmonary_compliance_multiplier == null)
        pulmonary_compliance_multiplier = new SEScalarUnsigned();
      return pulmonary_compliance_multiplier;
    }

    public bool HasPulmonaryResistanceMultiplier()
    {
      if (pulmonary_resistance_multiplier != null)
        return pulmonary_resistance_multiplier.IsValid();
      return false;
    }
    public SEScalarUnsigned GetPulmonaryResistanceMultiplier()
    {
      if (pulmonary_resistance_multiplier == null)
        pulmonary_resistance_multiplier = new SEScalarUnsigned();
      return pulmonary_resistance_multiplier;
    }

    public bool HasSystemicResistanceMultiplier()
    {
      if (systemic_resistance_multiplier != null)
        return systemic_resistance_multiplier.IsValid();
      return false;
    }
    public SEScalarUnsigned GetSystemicResistanceMultiplier()
    {
      if (systemic_resistance_multiplier == null)
        systemic_resistance_multiplier = new SEScalarUnsigned();
      return systemic_resistance_multiplier;
    }

    public bool HasSystemicComplianceMultiplier()
    {
      if (systemic_compliance_multiplier != null)
        return systemic_compliance_multiplier.IsValid();
      return false;
    }
    public SEScalarUnsigned GetSystemicComplianceMultiplier()
    {
      if (systemic_compliance_multiplier == null)
        systemic_compliance_multiplier = new SEScalarUnsigned();
      return systemic_compliance_multiplier;
    }

    public bool HasVenousComplianceMultiplier()
    {
      if (venous_compliance_multiplier != null)
        return venous_compliance_multiplier.IsValid();
      return false;
    }
    public SEScalarUnsigned GetVenousComplianceMultiplier()
    {
      if (venous_compliance_multiplier == null)
        venous_compliance_multiplier = new SEScalarUnsigned();
      return venous_compliance_multiplier;
    }

    public bool HasVenousResistanceMultiplier()
    {
      if (venous_resistance_multiplier != null)
        return venous_resistance_multiplier.IsValid();
      return false;
    }
    public SEScalarUnsigned GetVenousResistanceMultiplier()
    {
      if (venous_resistance_multiplier == null)
        venous_resistance_multiplier = new SEScalarUnsigned();
      return venous_resistance_multiplier;
    }

    public bool HasHeartRateMultiplier()
    {
      if (heart_rate_multiplier != null)
        return heart_rate_multiplier.IsValid();
      return false;
    }
    public SEScalarUnsigned GetHeartRateMultiplier()
    {
      if (heart_rate_multiplier == null)
        heart_rate_multiplier = new SEScalarUnsigned();
      return heart_rate_multiplier;
    }

    public bool HasStrokeVolumeMultiplier()
    {
      if (stroke_volume_multiplier != null)
        return stroke_volume_multiplier.IsValid();
      return false;
    }
    public SEScalarUnsigned GetStrokeVolumeMultiplier()
    {
      if (stroke_volume_multiplier == null)
        stroke_volume_multiplier = new SEScalarUnsigned();
      return stroke_volume_multiplier;
    }

    public override string ToString()
    {
      string str = "Cardiovascular Mechanics Modifiers";
      str += "\n\tArterialComplianceMultiplier: "; if (HasArterialComplianceMultiplier()) str += arterial_compliance_multiplier.ToString(); else str += "Not Set";
      str += "\n\tArterialResistanceMultiplier: "; if (HasArterialResistanceMultiplier()) str += arterial_resistance_multiplier.ToString(); else str += "Not Set";
      str += "\n\tPulmonaryComplianceMultiplier: "; if (HasPulmonaryComplianceMultiplier()) str += pulmonary_compliance_multiplier.ToString(); else str += "Not Set";
      str += "\n\tPulmonaryResistanceMultiplier: "; if (HasPulmonaryResistanceMultiplier()) str += pulmonary_resistance_multiplier.ToString(); else str += "Not Set";
      str += "\n\tSystemicResistanceMultiplier: "; if (HasSystemicResistanceMultiplier()) str += systemic_resistance_multiplier.ToString(); else str += "Not Set";
      str += "\n\tSystemicComplianceMultiplier: "; if (HasSystemicComplianceMultiplier()) str += systemic_compliance_multiplier.ToString(); else str += "Not Set";
      str += "\n\tVenousComplianceMultiplier: "; if (HasVenousComplianceMultiplier()) str += venous_compliance_multiplier.ToString(); else str += "Not Set";
      str += "\n\tVenousResistanceMultiplier: "; if (HasVenousResistanceMultiplier()) str += venous_resistance_multiplier.ToString(); else str += "Not Set";
      str += "\n\tHeartRateMultiplier: "; if (HasHeartRateMultiplier()) str += heart_rate_multiplier.ToString(); else str += "Not Set";
      str += "\n\tStrokeVolumeMultiplier: "; if (HasStrokeVolumeMultiplier()) str += stroke_volume_multiplier.ToString(); else str += "Not Set";
      return str;
    }

  }
}
