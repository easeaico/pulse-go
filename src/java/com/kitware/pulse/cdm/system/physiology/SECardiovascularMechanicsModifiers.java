/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.system.physiology;

import com.kitware.pulse.cdm.bind.Physiology.CardiovascularMechanicsModifiersData;
import com.kitware.pulse.cdm.properties.*;

public class SECardiovascularMechanicsModifiers
  {
    protected SEScalarUnsigned arterialComplianceMultiplier;
    protected SEScalarUnsigned arterialResistanceMultiplier;
    protected SEScalarUnsigned pulmonaryComplianceMultiplier;
    protected SEScalarUnsigned pulmonaryResistanceMultiplier;
    protected SEScalarUnsigned systemicResistanceMultiplier;
    protected SEScalarUnsigned systemicComplianceMultiplier;
    protected SEScalarUnsigned venousComplianceMultiplier;
    protected SEScalarUnsigned heartRateMultiplier;
    protected SEScalarUnsigned strokeVolumeMultiplier;

    public SECardiovascularMechanicsModifiers()
    {
      arterialComplianceMultiplier = null;
      arterialResistanceMultiplier = null;
      pulmonaryComplianceMultiplier = null;
      pulmonaryResistanceMultiplier = null;
      systemicResistanceMultiplier = null;
      systemicComplianceMultiplier = null;
      venousComplianceMultiplier = null;
      heartRateMultiplier = null;
      strokeVolumeMultiplier = null;
    }

    public void clear()
    {
      if (arterialComplianceMultiplier != null)
        arterialComplianceMultiplier.invalidate();
      if (arterialResistanceMultiplier != null)
        arterialResistanceMultiplier.invalidate();
      if (pulmonaryComplianceMultiplier != null)
        pulmonaryComplianceMultiplier.invalidate();
      if (pulmonaryResistanceMultiplier != null)
        pulmonaryResistanceMultiplier.invalidate();
      if (systemicResistanceMultiplier != null)
        systemicResistanceMultiplier.invalidate();
      if (systemicComplianceMultiplier != null)
        systemicComplianceMultiplier.invalidate();
      if (venousComplianceMultiplier != null)
        venousComplianceMultiplier.invalidate();
      if (heartRateMultiplier != null)
        heartRateMultiplier.invalidate();
      if (strokeVolumeMultiplier != null)
        strokeVolumeMultiplier.invalidate();
    }

    public void copy(SECardiovascularMechanicsModifiers from)
    {
      clear();
      if (from.hasArterialComplianceMultiplier())
        getArterialComplianceMultiplier().set(from.getArterialComplianceMultiplier());
      if (from.hasArterialResistanceMultiplier())
        getArterialResistanceMultiplier().set(from.getArterialResistanceMultiplier());
      if (from.hasPulmonaryComplianceMultiplier())
        getPulmonaryComplianceMultiplier().set(from.getPulmonaryComplianceMultiplier());
      if (from.hasPulmonaryResistanceMultiplier())
        getPulmonaryResistanceMultiplier().set(from.getPulmonaryResistanceMultiplier());
      if (from.hasSystemicResistanceMultiplier())
        getSystemicResistanceMultiplier().set(from.getSystemicResistanceMultiplier());
      if (from.hasSystemicComplianceMultiplier())
        getSystemicComplianceMultiplier().set(from.getSystemicComplianceMultiplier());
      if (from.hasVenousComplianceMultiplier())
        getVenousComplianceMultiplier().set(from.getVenousComplianceMultiplier());
      if (from.hasHeartRateMultiplier())
        getHeartRateMultiplier().set(from.getHeartRateMultiplier());
      if (from.hasStrokeVolumeMultiplier())
        getStrokeVolumeMultiplier().set(from.getStrokeVolumeMultiplier());
    }
    
    public static void load(CardiovascularMechanicsModifiersData src, SECardiovascularMechanicsModifiers dst)
    {
      if (src.hasArterialComplianceMultiplier())
        SEScalarUnsigned.load(src.getArterialComplianceMultiplier(),dst.getArterialComplianceMultiplier());
      if (src.hasArterialResistanceMultiplier())
        SEScalarUnsigned.load(src.getArterialResistanceMultiplier(),dst.getArterialResistanceMultiplier());
      if (src.hasPulmonaryComplianceMultiplier())
        SEScalarUnsigned.load(src.getPulmonaryComplianceMultiplier(),dst.getPulmonaryComplianceMultiplier());
      if (src.hasPulmonaryResistanceMultiplier())
        SEScalarUnsigned.load(src.getPulmonaryResistanceMultiplier(),dst.getPulmonaryResistanceMultiplier());
      if (src.hasSystemicResistanceMultiplier())
        SEScalarUnsigned.load(src.getSystemicResistanceMultiplier(),dst.getSystemicResistanceMultiplier());
      if (src.hasSystemicComplianceMultiplier())
        SEScalarUnsigned.load(src.getSystemicComplianceMultiplier(),dst.getSystemicComplianceMultiplier());
      if (src.hasVenousComplianceMultiplier())
        SEScalarUnsigned.load(src.getVenousComplianceMultiplier(),dst.getVenousComplianceMultiplier());
      if (src.hasHeartRateMultiplier())
        SEScalarUnsigned.load(src.getHeartRateMultiplier(),dst.getHeartRateMultiplier());
      if (src.hasStrokeVolumeMultiplier())
        SEScalarUnsigned.load(src.getStrokeVolumeMultiplier(),dst.getStrokeVolumeMultiplier());   
    }

    public static CardiovascularMechanicsModifiersData unload(SECardiovascularMechanicsModifiers src)
    {
      CardiovascularMechanicsModifiersData.Builder dst = CardiovascularMechanicsModifiersData.newBuilder();
      unload(src,dst);
      return dst.build();
    }

    protected static void unload(SECardiovascularMechanicsModifiers src, CardiovascularMechanicsModifiersData.Builder dst)
    {
      if (src.hasArterialComplianceMultiplier())
        dst.setArterialComplianceMultiplier(SEScalarUnsigned.unload(src.getArterialComplianceMultiplier()));
      if (src.hasArterialResistanceMultiplier())
        dst.setArterialResistanceMultiplier(SEScalarUnsigned.unload(src.getArterialResistanceMultiplier()));
      
      if (src.hasPulmonaryComplianceMultiplier())
        dst.setPulmonaryComplianceMultiplier(SEScalarUnsigned.unload(src.getPulmonaryComplianceMultiplier()));
      if (src.hasPulmonaryResistanceMultiplier())
        dst.setPulmonaryResistanceMultiplier(SEScalarUnsigned.unload(src.getPulmonaryResistanceMultiplier()));
      if (src.hasSystemicResistanceMultiplier())
        dst.setSystemicResistanceMultiplier(SEScalarUnsigned.unload(src.getSystemicResistanceMultiplier()));
      if (src.hasSystemicComplianceMultiplier())
        dst.setSystemicComplianceMultiplier(SEScalarUnsigned.unload(src.getSystemicComplianceMultiplier()));
      if (src.hasVenousComplianceMultiplier())
        dst.setVenousComplianceMultiplier(SEScalarUnsigned.unload(src.getVenousComplianceMultiplier()));
      if (src.hasVenousComplianceMultiplier())
        dst.setVenousComplianceMultiplier(SEScalarUnsigned.unload(src.getVenousComplianceMultiplier()));
      
      if (src.hasHeartRateMultiplier())
        dst.setHeartRateMultiplier(SEScalarUnsigned.unload(src.getHeartRateMultiplier()));
      if (src.hasStrokeVolumeMultiplier())
        dst.setStrokeVolumeMultiplier(SEScalarUnsigned.unload(src.getStrokeVolumeMultiplier()));
    }

    public boolean hasArterialComplianceMultiplier()
    {
      if (arterialComplianceMultiplier != null)
        return arterialComplianceMultiplier.isValid();
      return false;
    }
    public SEScalarUnsigned getArterialComplianceMultiplier()
    {
      if (arterialComplianceMultiplier == null)
        arterialComplianceMultiplier = new SEScalarUnsigned();
      return arterialComplianceMultiplier;
    }

    public boolean hasArterialResistanceMultiplier()
    {
      if (arterialResistanceMultiplier != null)
        return arterialResistanceMultiplier.isValid();
      return false;
    }
    public SEScalarUnsigned getArterialResistanceMultiplier()
    {
      if (arterialResistanceMultiplier == null)
        arterialResistanceMultiplier = new SEScalarUnsigned();
      return arterialResistanceMultiplier;
    }

    public boolean hasPulmonaryComplianceMultiplier()
    {
      if (pulmonaryComplianceMultiplier != null)
        return pulmonaryComplianceMultiplier.isValid();
      return false;
    }
    public SEScalarUnsigned getPulmonaryComplianceMultiplier()
    {
      if (pulmonaryComplianceMultiplier == null)
        pulmonaryComplianceMultiplier = new SEScalarUnsigned();
      return pulmonaryComplianceMultiplier;
    }

    public boolean hasPulmonaryResistanceMultiplier()
    {
      if (pulmonaryResistanceMultiplier != null)
        return pulmonaryResistanceMultiplier.isValid();
      return false;
    }
    public SEScalarUnsigned getPulmonaryResistanceMultiplier()
    {
      if (pulmonaryResistanceMultiplier == null)
        pulmonaryResistanceMultiplier = new SEScalarUnsigned();
      return pulmonaryResistanceMultiplier;
    }

    public boolean hasSystemicResistanceMultiplier()
    {
      if (systemicResistanceMultiplier != null)
        return systemicResistanceMultiplier.isValid();
      return false;
    }
    public SEScalarUnsigned getSystemicResistanceMultiplier()
    {
      if (systemicResistanceMultiplier == null)
        systemicResistanceMultiplier = new SEScalarUnsigned();
      return systemicResistanceMultiplier;
    }

    public boolean hasSystemicComplianceMultiplier()
    {
      if (systemicComplianceMultiplier != null)
        return systemicComplianceMultiplier.isValid();
      return false;
    }
    public SEScalarUnsigned getSystemicComplianceMultiplier()
    {
      if (systemicComplianceMultiplier == null)
        systemicComplianceMultiplier = new SEScalarUnsigned();
      return systemicComplianceMultiplier;
    }

    public boolean hasVenousComplianceMultiplier()
    {
      if (venousComplianceMultiplier != null)
        return venousComplianceMultiplier.isValid();
      return false;
    }
    public SEScalarUnsigned getVenousComplianceMultiplier()
    {
      if (venousComplianceMultiplier == null)
        venousComplianceMultiplier = new SEScalarUnsigned();
      return venousComplianceMultiplier;
    }

    public boolean hasHeartRateMultiplier()
    {
      if (heartRateMultiplier != null)
        return heartRateMultiplier.isValid();
      return false;
    }
    public SEScalarUnsigned getHeartRateMultiplier()
    {
      if (heartRateMultiplier == null)
        heartRateMultiplier = new SEScalarUnsigned();
      return heartRateMultiplier;
    }

    public boolean hasStrokeVolumeMultiplier()
    {
      if (strokeVolumeMultiplier != null)
        return strokeVolumeMultiplier.isValid();
      return false;
    }
    public SEScalarUnsigned getStrokeVolumeMultiplier()
    {
      if (strokeVolumeMultiplier == null)
        strokeVolumeMultiplier = new SEScalarUnsigned();
      return strokeVolumeMultiplier;
    }

    public String toString()
    {
      String str = "Cardiovascular Mechanics Modifiers";
      str += "\n\tArterialComplianceMultiplier: "; if (hasArterialComplianceMultiplier()) str += arterialComplianceMultiplier.toString(); else str += "Not Set";
      str += "\n\tArterialResistanceMultiplier: "; if (hasArterialResistanceMultiplier()) str += arterialResistanceMultiplier.toString(); else str += "Not Set";
      str += "\n\tPulmonaryComplianceMultiplier: "; if (hasPulmonaryComplianceMultiplier()) str += pulmonaryComplianceMultiplier.toString(); else str += "Not Set";
      str += "\n\tPulmonaryResistanceMultiplier: "; if (hasPulmonaryResistanceMultiplier()) str += pulmonaryResistanceMultiplier.toString(); else str += "Not Set";
      str += "\n\tSystemicResistanceMultiplier: "; if (hasSystemicResistanceMultiplier()) str += systemicResistanceMultiplier.toString(); else str += "Not Set";
      str += "\n\tSystemicComplianceMultiplier: "; if (hasSystemicComplianceMultiplier()) str += systemicComplianceMultiplier.toString(); else str += "Not Set";
      str += "\n\tVenousComplianceMultiplier: "; if (hasVenousComplianceMultiplier()) str += venousComplianceMultiplier.toString(); else str += "Not Set";
      str += "\n\tHeartRateMultiplier: "; if (hasHeartRateMultiplier()) str += heartRateMultiplier.toString(); else str += "Not Set";
      str += "\n\tStrokeVolumeMultiplier: "; if (hasStrokeVolumeMultiplier()) str += strokeVolumeMultiplier.toString(); else str += "Not Set";
      return str;
    }

  }
