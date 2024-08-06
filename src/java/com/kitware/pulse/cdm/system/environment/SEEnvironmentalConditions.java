/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.system.environment;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;

import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.util.JsonFormat;

import com.kitware.pulse.cdm.bind.Environment.EnvironmentalConditionsData;
import com.kitware.pulse.cdm.bind.Environment.EnvironmentalConditionsData.eSurroundingType;
import com.kitware.pulse.cdm.bind.Substance.SubstanceConcentrationData;
import com.kitware.pulse.cdm.bind.Substance.SubstanceFractionData;
import com.kitware.pulse.cdm.properties.SEScalar0To1;
import com.kitware.pulse.cdm.properties.SEScalarHeatResistanceArea;
import com.kitware.pulse.cdm.properties.SEScalarLengthPerTime;
import com.kitware.pulse.cdm.properties.SEScalarMassPerVolume;
import com.kitware.pulse.cdm.properties.SEScalarPressure;
import com.kitware.pulse.cdm.properties.SEScalarTemperature;
import com.kitware.pulse.cdm.properties.SEScalarVolume;
import com.kitware.pulse.cdm.substance.SESubstanceConcentration;
import com.kitware.pulse.cdm.substance.SESubstanceFraction;
import com.kitware.pulse.utilities.FileUtils;

public class SEEnvironmentalConditions
{
  protected eSurroundingType                surroundingType;
  protected SEScalarMassPerVolume           airDensity;
  protected SEScalarLengthPerTime           airVelocity;
  protected SEScalarTemperature             ambientTemperature;
  protected SEScalarPressure                atmosphericPressure;
  protected SEScalarHeatResistanceArea      clothingResistance;
  protected SEScalar0To1                    emissivity;
  protected SEScalarTemperature             meanRadiantTemperature;
  protected SEScalarVolume                  mechanicalDeadSpace;
  protected SEScalar0To1                    relativeHumidity;
  protected SEScalarTemperature             respirationAmbientTemperature;

  protected List<SESubstanceFraction>       ambientGases;
  protected List<SESubstanceConcentration>  ambientAerosols;
  
  
  public SEEnvironmentalConditions()
  {
    this.surroundingType=null;

    this.airDensity=null;
    this.airVelocity=null;
    this.ambientTemperature=null;
    this.atmosphericPressure=null;
    this.clothingResistance=null;
    this.emissivity=null;
    this.meanRadiantTemperature=null;
    this.mechanicalDeadSpace=null;
    this.relativeHumidity=null;
    this.respirationAmbientTemperature=null;

    this.ambientGases=new ArrayList<>();
    this.ambientAerosols=new ArrayList<>();

  }
  
  public void clear()
  {
    surroundingType = null;
    if (airDensity != null)
      airDensity.invalidate();
    if (airVelocity != null)
      airVelocity.invalidate();
    if (ambientTemperature != null)
      ambientTemperature.invalidate();
    if (atmosphericPressure != null)
      atmosphericPressure.invalidate();
    if (clothingResistance != null)
      clothingResistance.invalidate();
    if (emissivity != null)
      emissivity.invalidate();
    if (meanRadiantTemperature != null)
      meanRadiantTemperature.invalidate();
    if (mechanicalDeadSpace != null)
      mechanicalDeadSpace.invalidate();
    if (relativeHumidity != null)
      relativeHumidity.invalidate();
    if (respirationAmbientTemperature != null)
      respirationAmbientTemperature.invalidate();
    
    this.ambientGases.clear();
    this.ambientAerosols.clear();
  }
  
  public void copy(SEEnvironmentalConditions from)
  {
    this.clear();
    if(from.surroundingType != eSurroundingType.NullSurrounding)
      this.setSurroundingType(from.surroundingType);
    if(from.hasAirDensity())
      this.getAirDensity().set(from.getAirDensity());
    if(from.hasAirVelocity())
      this.getAirVelocity().set(from.getAirVelocity());
    if(from.hasAmbientTemperature())
      this.getAmbientTemperature().set(from.getAmbientTemperature());
    if(from.hasAtmosphericPressure())
      this.getAtmosphericPressure().set(from.getAtmosphericPressure());
    if(from.hasClothingResistance())
      this.getClothingResistance().set(from.getClothingResistance());
    if(from.hasEmissivity())
      this.getEmissivity().set(from.getEmissivity());
    if(from.hasMeanRadiantTemperature())
      this.getMeanRadiantTemperature().set(from.getMeanRadiantTemperature());
    if(from.hasMechanicalDeadSpace())
      this.getMechanicalDeadSpace().set(from.getMechanicalDeadSpace());
    if(from.hasRelativeHumidity())
      this.getRelativeHumidity().set(from.getRelativeHumidity());
    if(from.hasRespirationAmbientTemperature())
      this.getRespirationAmbientTemperature().set(from.getRespirationAmbientTemperature());
    
    if(from.ambientGases!=null)
    {
      SESubstanceFraction mine;
      for(SESubstanceFraction sf : from.ambientGases)
      {
        mine=this.createAmbientGas(sf.getSubstance());
        if(sf.hasAmount())
          mine.getAmount().set(sf.getAmount());
      }
    }
    
    if(from.ambientAerosols!=null)
    {
      SESubstanceConcentration mine;
      for(SESubstanceConcentration sc : from.ambientAerosols)
      {
        mine=this.createAmbientAerosol(sc.getSubstance());
        if(sc.hasConcentration())
          mine.getConcentration().set(sc.getConcentration());
      }
    }    
  }
  
  public void readFile(String fileName) throws InvalidProtocolBufferException
  {
    EnvironmentalConditionsData.Builder builder = EnvironmentalConditionsData.newBuilder();
    JsonFormat.parser().merge(FileUtils.readFile(fileName), builder);
    SEEnvironmentalConditions.load(builder.build(), this);
  }
  public void writeFile(String fileName) throws InvalidProtocolBufferException
  {
    FileUtils.writeFile(fileName, JsonFormat.printer().print(SEEnvironmentalConditions.unload(this)));
  }
  
  public static void load(EnvironmentalConditionsData src, SEEnvironmentalConditions dst)
  {
    dst.clear();
    if (src.getSurroundingType() != eSurroundingType.UNRECOGNIZED)
      dst.setSurroundingType(src.getSurroundingType());
    if (src.hasAirDensity())
      SEScalarMassPerVolume.load(src.getAirDensity(),dst.getAirDensity());
    if (src.hasAirVelocity())
      SEScalarLengthPerTime.load(src.getAirVelocity(),dst.getAirVelocity());
    if (src.hasAmbientTemperature())
      SEScalarTemperature.load(src.getAmbientTemperature(),dst.getAmbientTemperature());
    if (src.hasAtmosphericPressure())
      SEScalarPressure.load(src.getAtmosphericPressure(),dst.getAtmosphericPressure());
    if (src.hasClothingResistance())
      SEScalarHeatResistanceArea.load(src.getClothingResistance(),dst.getClothingResistance());
    if (src.hasEmissivity())
      SEScalar0To1.load(src.getEmissivity(),dst.getEmissivity());
    if (src.hasMeanRadiantTemperature())
      SEScalarTemperature.load(src.getMeanRadiantTemperature(),dst.getMeanRadiantTemperature());
    if (src.hasMechanicalDeadSpace())
      SEScalarVolume.load(src.getMechanicalDeadSpace(),dst.getMechanicalDeadSpace());
    if (src.hasRelativeHumidity())
      SEScalar0To1.load(src.getRelativeHumidity(),dst.getRelativeHumidity());
    if (src.hasRespirationAmbientTemperature())
      SEScalarTemperature.load(src.getRespirationAmbientTemperature(),dst.getRespirationAmbientTemperature());
    
    if(src.getAmbientGasList()!=null)
    {
      for(SubstanceFractionData subData : src.getAmbientGasList())
      {
        SEScalar0To1.load(subData.getAmount(),dst.createAmbientGas(subData.getName()).getAmount());
      }
    }
    
    if(src.getAmbientAerosolList()!=null)
    {
      for(SubstanceConcentrationData subData : src.getAmbientAerosolList())
      {
        SEScalarMassPerVolume.load(subData.getConcentration(),dst.createAmbientAerosol(subData.getName()).getConcentration());
      }
    }
  }
  public static EnvironmentalConditionsData unload(SEEnvironmentalConditions src)
  {
    EnvironmentalConditionsData.Builder dst = EnvironmentalConditionsData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(SEEnvironmentalConditions src, EnvironmentalConditionsData.Builder dst)
  {
  	if(src.hasSurroundingType())
  		dst.setSurroundingType(src.surroundingType);
    if (src.hasAirDensity())
      dst.setAirDensity(SEScalarMassPerVolume.unload(src.airDensity));
    if (src.hasAirVelocity())
      dst.setAirVelocity(SEScalarLengthPerTime.unload(src.airVelocity));
    if (src.hasAmbientTemperature())
      dst.setAmbientTemperature(SEScalarTemperature.unload(src.ambientTemperature));
    if (src.hasAtmosphericPressure())
      dst.setAtmosphericPressure(SEScalarPressure.unload(src.atmosphericPressure));
    if (src.hasClothingResistance())
      dst.setClothingResistance(SEScalarHeatResistanceArea.unload(src.clothingResistance));
    if (src.hasEmissivity())
      dst.setEmissivity(SEScalar0To1.unload(src.emissivity));  
    if (src.hasMeanRadiantTemperature())
      dst.setMeanRadiantTemperature(SEScalarTemperature.unload(src.meanRadiantTemperature));
    if (src.hasMechanicalDeadSpace())
      dst.setMechanicalDeadSpace(SEScalarVolume.unload(src.mechanicalDeadSpace));
    if (src.hasRelativeHumidity())
      dst.setRelativeHumidity(SEScalar0To1.unload(src.relativeHumidity));
    if (src.hasRespirationAmbientTemperature())
      dst.setRespirationAmbientTemperature(SEScalarTemperature.unload(src.respirationAmbientTemperature));    
    
    for(SESubstanceFraction ambSub : src.ambientGases)
      dst.addAmbientGas(SESubstanceFraction.unload(ambSub));
    
    for(SESubstanceConcentration ambSub : src.ambientAerosols)
      dst.addAmbientAerosol(SESubstanceConcentration.unload(ambSub));
  }
  
  public boolean hasSurroundingType()
  {
    return surroundingType != null;
  }
  public eSurroundingType getSurroundingType()
  {
    return surroundingType;
  }
  public void setSurroundingType(eSurroundingType st)
  {
    this.surroundingType = (st==eSurroundingType.UNRECOGNIZED) ? null : st;
  }
  public SEScalarMassPerVolume getAirDensity()
  {
    if (airDensity == null)
      airDensity = new SEScalarMassPerVolume();
    return airDensity;
  }
  public boolean hasAirDensity()
  {
    return airDensity == null ? false : airDensity.isValid();
  }
  
  public SEScalarLengthPerTime getAirVelocity()
  {
    if (airVelocity == null)
      airVelocity = new SEScalarLengthPerTime();
    return airVelocity;
  }
  public boolean hasAirVelocity()
  {
    return airVelocity == null ? false : airVelocity.isValid();
  }
  
  public SEScalarTemperature getAmbientTemperature()
  {
    if (ambientTemperature == null)
      ambientTemperature = new SEScalarTemperature();
    return ambientTemperature;
  }
  public boolean hasAmbientTemperature()
  {
    return ambientTemperature == null ? false : ambientTemperature.isValid();
  }

  public SEScalarPressure getAtmosphericPressure()
  {
    if (atmosphericPressure == null)
      atmosphericPressure = new SEScalarPressure();
    return atmosphericPressure;
  }
  public boolean hasAtmosphericPressure()
  {
    return atmosphericPressure == null ? false : atmosphericPressure.isValid();
  }
  
  public SEScalarHeatResistanceArea getClothingResistance()
  {
    if (clothingResistance == null)
      clothingResistance = new SEScalarHeatResistanceArea();
    return clothingResistance;
  }
  public boolean hasClothingResistance()
  {
    return clothingResistance == null ? false : clothingResistance.isValid();
  }
  
  public SEScalar0To1 getEmissivity()
  {
      if (emissivity == null)
        emissivity = new SEScalar0To1();
      return emissivity;
  }
  public boolean hasEmissivity()
  {
      return emissivity == null ? false : emissivity.isValid();
  }
  
  public SEScalarTemperature getMeanRadiantTemperature()
  {
    if (meanRadiantTemperature == null)
      meanRadiantTemperature = new SEScalarTemperature();
    return meanRadiantTemperature;
  }
  public boolean hasMeanRadiantTemperature()
  {
    return meanRadiantTemperature == null ? false : meanRadiantTemperature.isValid();
  }
  
  public SEScalarVolume getMechanicalDeadSpace()
  {
    if (mechanicalDeadSpace == null)
      mechanicalDeadSpace = new SEScalarVolume();
    return mechanicalDeadSpace;
  }
  public boolean hasMechanicalDeadSpace()
  {
    return mechanicalDeadSpace == null ? false : mechanicalDeadSpace.isValid();
  }
  
  public SEScalar0To1 getRelativeHumidity()
  {
    if (relativeHumidity == null)
      relativeHumidity = new SEScalar0To1();
    return relativeHumidity;
  }
  public boolean hasRelativeHumidity()
  {
    return relativeHumidity == null ? false : relativeHumidity.isValid();
  }
  
  public SEScalarTemperature getRespirationAmbientTemperature()
  {
    if (respirationAmbientTemperature == null)
      respirationAmbientTemperature = new SEScalarTemperature();
    return respirationAmbientTemperature;
  }
  public boolean hasRespirationAmbientTemperature()
  {
    return respirationAmbientTemperature == null ? false : respirationAmbientTemperature.isValid();
  }
  
  public SESubstanceFraction createAmbientGas(String substance)
  {
    return getAmbientGas(substance);
  }
  public SESubstanceFraction getAmbientGas(String substance)
  {
    for(SESubstanceFraction sf : this.ambientGases)
    {
      if(sf.getSubstance().equals(substance))
      {        
        return sf;
      }
    }    
    SESubstanceFraction sf = new SESubstanceFraction(substance);    
    this.ambientGases.add(sf);
    return sf;
  }
  public boolean hasAmbientGas()
  {
    return !this.ambientGases.isEmpty();
  }
  public boolean hasAmbientGas(String substance)
  {
    for(SESubstanceFraction sf : this.ambientGases)
    {
      if(sf.getSubstance().equals(substance))
      {        
        return true;
      }
    }
    return false;
  }
  public List<SESubstanceFraction> getAmbientGas()
  {
    return this.ambientGases;
  }
  public void removeAmbientGas(String substance)
  {
    for(SESubstanceFraction sf : this.ambientGases)
    {
      if(sf.getSubstance().equals(substance))
      {
        this.ambientGases.remove(sf);
        return;
      }
    }  
  }

  
  public SESubstanceConcentration createAmbientAerosol(String substance)
  {
    return getAmbientAerosol(substance);
  }
  public SESubstanceConcentration getAmbientAerosol(String substance)
  {
    for(SESubstanceConcentration sc : this.ambientAerosols)
    {
      if(sc.getSubstance().equals(substance))
      {
        return sc;
      }
    }    
    SESubstanceConcentration sc = new SESubstanceConcentration(substance);
    this.ambientAerosols.add(sc);
    return sc;
  }
  public boolean hasAmbientAerosol()
  {
    return !this.ambientAerosols.isEmpty();
  }
  public boolean hasAmbientAerosol(String substance)
  {
    for(SESubstanceConcentration sc : this.ambientAerosols)
    {
      if(sc.getSubstance().equals(substance))
      {
        return true;
      }
    }
    return false;
  }
  public List<SESubstanceConcentration> getAmbientAerosol()
  {
    return Collections.unmodifiableList(this.ambientAerosols);
  }
  public void removeAmbientAerosol(String substance)
  {
    for(SESubstanceConcentration sc : this.ambientAerosols)
    {
      if(sc.getSubstance().equals(substance))
      {
        this.ambientAerosols.remove(sc);
        return;
      }
    }  
  }
  
  
  public void trim()
  {
    SESubstanceConcentration sc;
    Iterator<SESubstanceConcentration> icon = this.ambientAerosols.iterator();
    while(icon.hasNext())
    {
      sc=icon.next();
      if(sc.getConcentration().getValue()==0)
        icon.remove();
    }    
    
    SESubstanceFraction sf;
    Iterator<SESubstanceFraction> ifrac = this.ambientGases.iterator();
    while(ifrac.hasNext())
    {
      sf=ifrac.next();
      if(sf.getAmount().getValue()==0)
        ifrac.remove();
    }    
  }
  
  @Override
  public String toString()
  {
      String str = "Envriomental Conditions:" 
          + "\n\tSurroundingType: " + getSurroundingType()
          + "\n\tAirDensity: " + (hasAirDensity()?getAirDensity():"None")
          + "\n\tAirVelocity: " + (hasAirVelocity()?getAirVelocity():"None")
          + "\n\tAmbientTemperature: " + (hasAmbientTemperature()?getAmbientTemperature():"None")
          + "\n\tAtmosphericPressure: " + (hasAtmosphericPressure()?getAtmosphericPressure():"None")
          + "\n\tClothingResistance: " + (hasClothingResistance()?getClothingResistance():"None")
          + "\n\tEmissivity: " + (hasEmissivity()?getEmissivity():"None")
          + "\n\tMeanRadiantTemperature: " + (hasMeanRadiantTemperature()?getMeanRadiantTemperature():"None")
          + "\n\tMechanicalDeadSpace: " + (hasMechanicalDeadSpace()?getMechanicalDeadSpace():"None")
          + "\n\tRelativeHumidity: " + (hasRelativeHumidity()?getRelativeHumidity():"None")
          + "\n\tRespirationAmbientTemperature: " + (hasRespirationAmbientTemperature()?getRespirationAmbientTemperature():"None");
      for(SESubstanceFraction sf : this.ambientGases)
        str += "\n\t"+sf.toString();
      for(SESubstanceConcentration sc : this.ambientAerosols)
      str += "\n\t"+sc.toString();
      
      return str;
  }
  
  
}