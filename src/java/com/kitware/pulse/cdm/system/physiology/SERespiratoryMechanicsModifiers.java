/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.system.physiology;

import java.util.HashMap;
import java.util.Map;

import com.kitware.pulse.cdm.bind.Physiology.LungVolumeIncrementData;
import com.kitware.pulse.cdm.bind.Physiology.RespiratoryMechanicsModifiersData;
import com.kitware.pulse.cdm.bind.Physiology.eLungCompartment;
import com.kitware.pulse.cdm.properties.*;

public class SERespiratoryMechanicsModifiers
  {
    protected SEScalarUnsigned leftComplianceMultiplier;
    protected SEScalarUnsigned rightComplianceMultiplier;
    protected SEScalarUnsigned leftExpiratoryResistanceMultiplier;
    protected SEScalarUnsigned leftInspiratoryResistanceMultiplier;
    protected SEScalarUnsigned rightExpiratoryResistanceMultiplier;
    protected SEScalarUnsigned rightInspiratoryResistanceMultiplier;
    protected SEScalarUnsigned upperExpiratoryResistanceMultiplier;
    protected SEScalarUnsigned upperInspiratoryResistanceMultiplier;
    protected SEScalarUnsigned respirationRateMultiplier;
    protected SEScalarUnsigned tidalVolumeMultiplier;
    
    protected Map<eLungCompartment,SEScalarVolume> volumeIncrements;

    public SERespiratoryMechanicsModifiers()
    {
      leftComplianceMultiplier = null;
      rightComplianceMultiplier = null;
      leftExpiratoryResistanceMultiplier = null;
      leftInspiratoryResistanceMultiplier = null;
      rightExpiratoryResistanceMultiplier = null;
      rightInspiratoryResistanceMultiplier = null;
      upperExpiratoryResistanceMultiplier = null;
      upperInspiratoryResistanceMultiplier = null;
      respirationRateMultiplier = null;
      tidalVolumeMultiplier = null;
      
      volumeIncrements = new HashMap<eLungCompartment,SEScalarVolume>();
    }

    public void clear()
    {
      if (leftComplianceMultiplier != null)
        leftComplianceMultiplier.invalidate();
      if (rightComplianceMultiplier != null)
        rightComplianceMultiplier.invalidate();
      if (leftExpiratoryResistanceMultiplier != null)
        leftExpiratoryResistanceMultiplier.invalidate();
      if (leftInspiratoryResistanceMultiplier != null)
        leftInspiratoryResistanceMultiplier.invalidate();
      if (rightExpiratoryResistanceMultiplier != null)
        rightExpiratoryResistanceMultiplier.invalidate();
      if (rightInspiratoryResistanceMultiplier != null)
        rightInspiratoryResistanceMultiplier.invalidate();
      if (upperExpiratoryResistanceMultiplier != null)
        upperExpiratoryResistanceMultiplier.invalidate();
      if (upperInspiratoryResistanceMultiplier != null)
        upperInspiratoryResistanceMultiplier.invalidate();
      if (respirationRateMultiplier != null)
        respirationRateMultiplier.invalidate();
      if (tidalVolumeMultiplier != null)
        tidalVolumeMultiplier.invalidate();
      
      for(SEScalarVolume s : volumeIncrements.values())
        s.invalidate();
    }

    public void copy(SERespiratoryMechanicsModifiers from)
    {
      clear();
      if (from.hasLeftComplianceMultiplier())
        getLeftComplianceMultiplier().set(from.getLeftComplianceMultiplier());
      if (from.hasRightComplianceMultiplier())
        getRightComplianceMultiplier().set(from.getRightComplianceMultiplier());
      if (from.hasLeftExpiratoryResistanceMultiplier())
        getLeftExpiratoryResistanceMultiplier().set(from.getLeftExpiratoryResistanceMultiplier());
      if (from.hasLeftInspiratoryResistanceMultiplier())
        getLeftInspiratoryResistanceMultiplier().set(from.getLeftInspiratoryResistanceMultiplier());
      if (from.hasRightExpiratoryResistanceMultiplier())
        getRightExpiratoryResistanceMultiplier().set(from.getRightExpiratoryResistanceMultiplier());
      if (from.hasRightInspiratoryResistanceMultiplier())
        getRightInspiratoryResistanceMultiplier().set(from.getRightInspiratoryResistanceMultiplier());
      if (from.hasUpperExpiratoryResistanceMultiplier())
        getUpperExpiratoryResistanceMultiplier().set(from.getUpperExpiratoryResistanceMultiplier());
      if (from.hasUpperInspiratoryResistanceMultiplier())
        getUpperInspiratoryResistanceMultiplier().set(from.getUpperInspiratoryResistanceMultiplier());
      if (from.hasRespirationRateMultiplier())
        getRespirationRateMultiplier().set(from.getRespirationRateMultiplier());
      if (from.hasTidalVolumeMultiplier())
        getTidalVolumeMultiplier().set(from.getTidalVolumeMultiplier());
      
      for (Map.Entry<eLungCompartment, SEScalarVolume> entry : from.volumeIncrements.entrySet())
        getVolumeIncrement(entry.getKey()).set(entry.getValue());
    }
    
    public static void load(RespiratoryMechanicsModifiersData src, SERespiratoryMechanicsModifiers dst)
    {
      if (src.hasLeftComplianceMultiplier())
        SEScalarUnsigned.load(src.getLeftComplianceMultiplier(),dst.getLeftComplianceMultiplier());
      if (src.hasRightComplianceMultiplier())
        SEScalarUnsigned.load(src.getRightComplianceMultiplier(),dst.getRightComplianceMultiplier());
      if (src.hasLeftExpiratoryResistanceMultiplier())
        SEScalarUnsigned.load(src.getLeftExpiratoryResistanceMultiplier(),dst.getLeftExpiratoryResistanceMultiplier());
      if (src.hasLeftInspiratoryResistanceMultiplier())
        SEScalarUnsigned.load(src.getLeftInspiratoryResistanceMultiplier(),dst.getLeftInspiratoryResistanceMultiplier());
      if (src.hasRightExpiratoryResistanceMultiplier())
        SEScalarUnsigned.load(src.getRightExpiratoryResistanceMultiplier(),dst.getRightExpiratoryResistanceMultiplier());
      if (src.hasRightInspiratoryResistanceMultiplier())
        SEScalarUnsigned.load(src.getRightInspiratoryResistanceMultiplier(),dst.getRightInspiratoryResistanceMultiplier());
      if (src.hasUpperExpiratoryResistanceMultiplier())
        SEScalarUnsigned.load(src.getUpperExpiratoryResistanceMultiplier(),dst.getUpperExpiratoryResistanceMultiplier());
      if (src.hasUpperInspiratoryResistanceMultiplier())
        SEScalarUnsigned.load(src.getUpperInspiratoryResistanceMultiplier(),dst.getUpperInspiratoryResistanceMultiplier());
      if (src.hasRespirationRateMultiplier())
        SEScalarUnsigned.load(src.getRespirationRateMultiplier(),dst.getRespirationRateMultiplier());
      if (src.hasTidalVolumeMultiplier())
        SEScalarUnsigned.load(src.getTidalVolumeMultiplier(),dst.getTidalVolumeMultiplier());   
    
      for (LungVolumeIncrementData d : src.getVolumeIncrementList())
        SEScalarVolume.load(d.getIncrement(), dst.getVolumeIncrement(d.getCompartment()));
    }

    public static RespiratoryMechanicsModifiersData unload(SERespiratoryMechanicsModifiers src)
    {
      RespiratoryMechanicsModifiersData.Builder dst = RespiratoryMechanicsModifiersData.newBuilder();
      unload(src,dst);
      return dst.build();
    }

    protected static void unload(SERespiratoryMechanicsModifiers src, RespiratoryMechanicsModifiersData.Builder dst)
    {
      if (src.hasLeftComplianceMultiplier())
        dst.setLeftComplianceMultiplier(SEScalarUnsigned.unload(src.getLeftComplianceMultiplier()));
      if (src.hasRightComplianceMultiplier())
        dst.setRightComplianceMultiplier(SEScalarUnsigned.unload(src.getRightComplianceMultiplier()));
      
      if (src.hasLeftExpiratoryResistanceMultiplier())
        dst.setLeftExpiratoryResistanceMultiplier(SEScalarUnsigned.unload(src.getLeftExpiratoryResistanceMultiplier()));
      if (src.hasLeftInspiratoryResistanceMultiplier())
        dst.setLeftInspiratoryResistanceMultiplier(SEScalarUnsigned.unload(src.getLeftInspiratoryResistanceMultiplier()));
      if (src.hasRightExpiratoryResistanceMultiplier())
        dst.setRightExpiratoryResistanceMultiplier(SEScalarUnsigned.unload(src.getRightExpiratoryResistanceMultiplier()));
      if (src.hasRightInspiratoryResistanceMultiplier())
        dst.setRightInspiratoryResistanceMultiplier(SEScalarUnsigned.unload(src.getRightInspiratoryResistanceMultiplier()));
      if (src.hasUpperExpiratoryResistanceMultiplier())
        dst.setUpperExpiratoryResistanceMultiplier(SEScalarUnsigned.unload(src.getUpperExpiratoryResistanceMultiplier()));
      if (src.hasUpperExpiratoryResistanceMultiplier())
        dst.setUpperExpiratoryResistanceMultiplier(SEScalarUnsigned.unload(src.getUpperExpiratoryResistanceMultiplier()));
      
      if (src.hasRespirationRateMultiplier())
        dst.setRespirationRateMultiplier(SEScalarUnsigned.unload(src.getRespirationRateMultiplier()));
      if (src.hasTidalVolumeMultiplier())
        dst.setTidalVolumeMultiplier(SEScalarUnsigned.unload(src.getTidalVolumeMultiplier()));
    
      for (Map.Entry<eLungCompartment, SEScalarVolume> entry : src.volumeIncrements.entrySet())
      {
        LungVolumeIncrementData.Builder builder = LungVolumeIncrementData.newBuilder();
        builder.setCompartment(entry.getKey());
        builder.setIncrement(SEScalarVolume.unload(entry.getValue()));
        dst.addVolumeIncrement(builder);
      }
    }

    public boolean hasLeftComplianceMultiplier()
    {
      if (leftComplianceMultiplier != null)
        return leftComplianceMultiplier.isValid();
      return false;
    }
    public SEScalarUnsigned getLeftComplianceMultiplier()
    {
      if (leftComplianceMultiplier == null)
        leftComplianceMultiplier = new SEScalarUnsigned();
      return leftComplianceMultiplier;
    }

    public boolean hasRightComplianceMultiplier()
    {
      if (rightComplianceMultiplier != null)
        return rightComplianceMultiplier.isValid();
      return false;
    }
    public SEScalarUnsigned getRightComplianceMultiplier()
    {
      if (rightComplianceMultiplier == null)
        rightComplianceMultiplier = new SEScalarUnsigned();
      return rightComplianceMultiplier;
    }

    public boolean hasLeftExpiratoryResistanceMultiplier()
    {
      if (leftExpiratoryResistanceMultiplier != null)
        return leftExpiratoryResistanceMultiplier.isValid();
      return false;
    }
    public SEScalarUnsigned getLeftExpiratoryResistanceMultiplier()
    {
      if (leftExpiratoryResistanceMultiplier == null)
        leftExpiratoryResistanceMultiplier = new SEScalarUnsigned();
      return leftExpiratoryResistanceMultiplier;
    }

    public boolean hasLeftInspiratoryResistanceMultiplier()
    {
      if (leftInspiratoryResistanceMultiplier != null)
        return leftInspiratoryResistanceMultiplier.isValid();
      return false;
    }
    public SEScalarUnsigned getLeftInspiratoryResistanceMultiplier()
    {
      if (leftInspiratoryResistanceMultiplier == null)
        leftInspiratoryResistanceMultiplier = new SEScalarUnsigned();
      return leftInspiratoryResistanceMultiplier;
    }

    public boolean hasRightExpiratoryResistanceMultiplier()
    {
      if (rightExpiratoryResistanceMultiplier != null)
        return rightExpiratoryResistanceMultiplier.isValid();
      return false;
    }
    public SEScalarUnsigned getRightExpiratoryResistanceMultiplier()
    {
      if (rightExpiratoryResistanceMultiplier == null)
        rightExpiratoryResistanceMultiplier = new SEScalarUnsigned();
      return rightExpiratoryResistanceMultiplier;
    }

    public boolean hasRightInspiratoryResistanceMultiplier()
    {
      if (rightInspiratoryResistanceMultiplier != null)
        return rightInspiratoryResistanceMultiplier.isValid();
      return false;
    }
    public SEScalarUnsigned getRightInspiratoryResistanceMultiplier()
    {
      if (rightInspiratoryResistanceMultiplier == null)
        rightInspiratoryResistanceMultiplier = new SEScalarUnsigned();
      return rightInspiratoryResistanceMultiplier;
    }

    public boolean hasUpperExpiratoryResistanceMultiplier()
    {
      if (upperExpiratoryResistanceMultiplier != null)
        return upperExpiratoryResistanceMultiplier.isValid();
      return false;
    }
    public SEScalarUnsigned getUpperExpiratoryResistanceMultiplier()
    {
      if (upperExpiratoryResistanceMultiplier == null)
        upperExpiratoryResistanceMultiplier = new SEScalarUnsigned();
      return upperExpiratoryResistanceMultiplier;
    }

    public boolean hasUpperInspiratoryResistanceMultiplier()
    {
      if (upperInspiratoryResistanceMultiplier != null)
        return upperInspiratoryResistanceMultiplier.isValid();
      return false;
    }
    public SEScalarUnsigned getUpperInspiratoryResistanceMultiplier()
    {
      if (upperInspiratoryResistanceMultiplier == null)
        upperInspiratoryResistanceMultiplier = new SEScalarUnsigned();
      return upperInspiratoryResistanceMultiplier;
    }

    public boolean hasRespirationRateMultiplier()
    {
      if (respirationRateMultiplier != null)
        return respirationRateMultiplier.isValid();
      return false;
    }
    public SEScalarUnsigned getRespirationRateMultiplier()
    {
      if (respirationRateMultiplier == null)
        respirationRateMultiplier = new SEScalarUnsigned();
      return respirationRateMultiplier;
    }

    public boolean hasTidalVolumeMultiplier()
    {
      if (tidalVolumeMultiplier != null)
        return tidalVolumeMultiplier.isValid();
      return false;
    }
    public SEScalarUnsigned getTidalVolumeMultiplier()
    {
      if (tidalVolumeMultiplier == null)
        tidalVolumeMultiplier = new SEScalarUnsigned();
      return tidalVolumeMultiplier;
    }
    
    public boolean hasVolumeIncrement()
    {
      for(SEScalarVolume s : volumeIncrements.values())
        if(s.isValid())
          return true;
      return false;
    }
    public boolean hasVolumeIncrement(eLungCompartment c)
    {
      SEScalarVolume s = volumeIncrements.get(c);
      if(s!=null)
        return s.isValid();
      return false;
    }
    public SEScalarVolume getVolumeIncrement(eLungCompartment c)
    {
      SEScalarVolume s = volumeIncrements.get(c);
      if(s==null)
      {
        s = new SEScalarVolume();
        volumeIncrements.put(c, s);
      }
      return s;
    }

    public String toString()
    {
      String str = "Respiratory Mechanics Modifiers";
      str += "\n\tLeftComplianceMultiplier: "; if (hasLeftComplianceMultiplier()) str += leftComplianceMultiplier.toString(); else str += "Not Set";
      str += "\n\tRightComplianceMultiplier: "; if (hasRightComplianceMultiplier()) str += rightComplianceMultiplier.toString(); else str += "Not Set";
      str += "\n\tLeftExpiratoryResistanceMultiplier: "; if (hasLeftExpiratoryResistanceMultiplier()) str += leftExpiratoryResistanceMultiplier.toString(); else str += "Not Set";
      str += "\n\tLeftInspiratoryResistanceMultiplier: "; if (hasLeftInspiratoryResistanceMultiplier()) str += leftInspiratoryResistanceMultiplier.toString(); else str += "Not Set";
      str += "\n\tRightExpiratoryResistanceMultiplier: "; if (hasRightExpiratoryResistanceMultiplier()) str += rightExpiratoryResistanceMultiplier.toString(); else str += "Not Set";
      str += "\n\tRightInspiratoryResistanceMultiplier: "; if (hasRightInspiratoryResistanceMultiplier()) str += rightInspiratoryResistanceMultiplier.toString(); else str += "Not Set";
      str += "\n\tUpperExpiratoryResistanceMultiplier: "; if (hasUpperExpiratoryResistanceMultiplier()) str += upperExpiratoryResistanceMultiplier.toString(); else str += "Not Set";
      str += "\n\tUpperInspiratoryResistanceMultiplier: "; if (hasUpperInspiratoryResistanceMultiplier()) str += upperInspiratoryResistanceMultiplier.toString(); else str += "Not Set";
      str += "\n\trespirationRateMultiplier: "; if (hasRespirationRateMultiplier()) str += respirationRateMultiplier.toString(); else str += "Not Set";
      str += "\n\ttidalVolumeMultiplier: "; if (hasTidalVolumeMultiplier()) str += tidalVolumeMultiplier.toString(); else str += "Not Set";
      for (Map.Entry<eLungCompartment, SEScalarVolume> entry : volumeIncrements.entrySet())
        str += "\n\t"+ entry.getKey().toString()+" VolumeIncrement: " + getVolumeIncrement(entry.getKey());
      return str;
    }

  }
