/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
package com.kitware.pulse.cdm.system.equipment.bag_valve_mask;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;

import com.kitware.pulse.cdm.bind.BagValveMask.BagValveMaskData;
import com.kitware.pulse.cdm.bind.Enums.eBreathState;
import com.kitware.pulse.cdm.bind.Enums.eSwitch;
import com.kitware.pulse.cdm.bind.Substance.SubstanceConcentrationData;
import com.kitware.pulse.cdm.bind.Substance.SubstanceFractionData;
import com.kitware.pulse.cdm.properties.*;
import com.kitware.pulse.cdm.substance.SESubstanceConcentration;
import com.kitware.pulse.cdm.substance.SESubstanceFraction;
import com.kitware.pulse.cdm.system.equipment.SEEquipment;

public class SEBagValveMask implements SEEquipment
{
  protected eBreathState                      breathState;
  protected eSwitch                           connection;
  
  protected SEScalarPressureTimePerVolume     bagResistance;
  protected SEScalarVolume                    connectionVolume;
  protected SEScalarPressureTimePerVolume     filterResistance;
  protected SEScalarVolume                    filterVolume;
  protected SEScalarPressureTimePerVolume     sealResistance;
  protected SEScalarPressure                  valvePositiveEndExpiratoryPressure;
  protected SEScalarPressureTimePerVolume     valveResistance;
  protected SEScalarVolume                    valveVolume;
  
  protected List<SESubstanceFraction>         fractionInspiredGases;
  protected List<SESubstanceConcentration>    concentrationInspiredAerosol;


  public SEBagValveMask()
  {
    breathState = null;
    connection = null;
    bagResistance = null;
    connectionVolume = null;
    filterResistance = null;
    filterVolume = null;
    sealResistance = null;
    valvePositiveEndExpiratoryPressure = null;
    valveResistance = null;
    valveVolume = null;

    this.fractionInspiredGases=new ArrayList<SESubstanceFraction>();
    this.concentrationInspiredAerosol=new ArrayList<SESubstanceConcentration>();
  }

  @Override
  public void clear()
  {
    breathState = null;
    connection = null;
    if (bagResistance != null)
      bagResistance.invalidate();
    if (connectionVolume != null)
      connectionVolume.invalidate();
    if (filterResistance != null)
      filterResistance.invalidate();
    if (filterVolume != null)
      filterVolume.invalidate();
    if (sealResistance != null)
      sealResistance.invalidate();
    if (valvePositiveEndExpiratoryPressure != null)
      valvePositiveEndExpiratoryPressure.invalidate();
    if (valveResistance != null)
      valveResistance.invalidate();
    if (valveVolume != null)
      valveVolume.invalidate();
    
    this.fractionInspiredGases.clear();
    this.concentrationInspiredAerosol.clear();
  }

  public void copy(SEBagValveMask from)
  {
    clear();
    if(from.breathState!=null)
      this.breathState=from.breathState;
    if(from.connection!=null && from.connection != eSwitch.NullSwitch)
    	this.connection=from.connection;
    
    if(from.hasBagResistance())
      this.getBagResistance().set(from.getBagResistance());
    if(from.hasConnectionVolume())
      this.getConnectionVolume().set(from.getConnectionVolume());
    if(from.hasFilterResistance())
      this.getFilterResistance().set(from.getFilterResistance());
    if(from.hasFilterVolume())
      this.getFilterVolume().set(from.getFilterVolume());
    if(from.hasSealResistance())
      this.getSealResistance().set(from.getSealResistance());
    if(from.hasValvePositiveEndExpiratoryPressure())
      this.getValvePositiveEndExpiratoryPressure().set(from.getValvePositiveEndExpiratoryPressure());
    if(from.hasValveResistance())
      this.getValveResistance().set(from.getValveResistance());
    if(from.hasValveVolume())
      this.getValveVolume().set(from.getValveVolume());
    
    if(from.fractionInspiredGases!=null)
    {
      SESubstanceFraction mine;
      for(SESubstanceFraction sf : from.fractionInspiredGases)
      {
        mine=this.createFractionInspiredGas(sf.getSubstance());
        if(sf.hasAmount())
          mine.getAmount().set(sf.getAmount());
      }
    }
    if(from.concentrationInspiredAerosol!=null)
    {
      SESubstanceConcentration mine;
      for(SESubstanceConcentration sc : from.concentrationInspiredAerosol)
      {
        mine=this.createConcentrationInspiredAerosol(sc.getSubstance());
        if(sc.hasConcentration())
          mine.getConcentration().set(sc.getConcentration());
      }
    }
  }

  public static void load(BagValveMaskData src, SEBagValveMask dst)
  {
    dst.clear();
    if (src.getBreathState()!=eBreathState.UNRECOGNIZED)
      dst.setBreathState(src.getBreathState());
    if (src.getConnection()!=eSwitch.UNRECOGNIZED)
      dst.setConnection(src.getConnection());
    
    if (src.hasBagResistance())
      SEScalarPressureTimePerVolume.load(src.getBagResistance(), dst.getBagResistance());
    if (src.hasConnectionVolume())
      SEScalarVolume.load(src.getConnectionVolume(), dst.getConnectionVolume());
    if (src.hasFilterResistance())
      SEScalarPressureTimePerVolume.load(src.getFilterResistance(), dst.getFilterResistance());
    if (src.hasFilterVolume())
      SEScalarVolume.load(src.getFilterVolume(), dst.getFilterVolume());
    if (src.hasSealResistance())
      SEScalarPressureTimePerVolume.load(src.getSealResistance(), dst.getSealResistance());
    if (src.hasValvePositiveEndExpiratoryPressure())
      SEScalarPressure.load(src.getValvePositiveEndExpiratoryPressure(), dst.getValvePositiveEndExpiratoryPressure());
    if (src.hasValveResistance())
      SEScalarPressureTimePerVolume.load(src.getValveResistance(), dst.getValveResistance());
    if (src.hasValveVolume())
      SEScalarVolume.load(src.getValveVolume(), dst.getValveVolume());
    
    if(src.getFractionInspiredGasList()!=null)
    {
      for(SubstanceFractionData subData : src.getFractionInspiredGasList())
      {
        SEScalar0To1.load(subData.getAmount(),dst.createFractionInspiredGas(subData.getName()).getAmount());
      }
    }
    
    if(src.getConcentrationInspiredAerosolList()!=null)
    {
      for(SubstanceConcentrationData subData : src.getConcentrationInspiredAerosolList())
      {
        SEScalarMassPerVolume.load(subData.getConcentration(),dst.createConcentrationInspiredAerosol(subData.getName()).getConcentration());
      }
    }
  }
  public static BagValveMaskData unload(SEBagValveMask src)
  {
    BagValveMaskData.Builder dst = BagValveMaskData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(SEBagValveMask src, BagValveMaskData.Builder dst)
  {
    if (src.hasBreathState())
      dst.setBreathState(src.breathState);
    if (src.hasConnection())
      dst.setConnection(src.connection);
    
    if (src.hasConnectionVolume())
      dst.setConnectionVolume(SEScalarVolume.unload(src.getConnectionVolume()));
    if(src.hasBagResistance())
      dst.setBagResistance(SEScalarPressureTimePerVolume.unload(src.getBagResistance()));
    if (src.hasFilterResistance())
      dst.setFilterResistance(SEScalarPressureTimePerVolume.unload(src.getFilterResistance()));
    if (src.hasFilterVolume())
      dst.setFilterVolume(SEScalarVolume.unload(src.getFilterVolume()));
    if(src.hasSealResistance())
        dst.setSealResistance(SEScalarPressureTimePerVolume.unload(src.getSealResistance()));
    if(src.hasValvePositiveEndExpiratoryPressure())
      dst.setValvePositiveEndExpiratoryPressure(SEScalarPressure.unload(src.getValvePositiveEndExpiratoryPressure()));
    if (src.hasValveResistance())
      dst.setValveResistance(SEScalarPressureTimePerVolume.unload(src.getValveResistance()));
    if (src.hasValveVolume())
      dst.setValveVolume(SEScalarVolume.unload(src.getValveVolume()));
    
    for(SESubstanceFraction ambSub : src.fractionInspiredGases)
      dst.addFractionInspiredGas(SESubstanceFraction.unload(ambSub));
    
    for(SESubstanceConcentration ambSub : src.concentrationInspiredAerosol)
      dst.addConcentrationInspiredAerosol(SESubstanceConcentration.unload(ambSub));
  }
  
  public eBreathState getBreathState()
  {
    return breathState;
  }
  public void setBreathState(eBreathState b)
  {
    breathState = (b == eBreathState.UNRECOGNIZED) ? null : b;
  }
  public boolean hasBreathState()
  {
    return breathState != null;
  }
  
  public eSwitch getConnection()
  {
    return connection;
  }
  public void setConnection(eSwitch c)
  {
    connection = (c == eSwitch.UNRECOGNIZED) ? null : c;
  }
  public boolean hasConnection()
  {
    return connection != null;
  }
  
  public SEScalarPressureTimePerVolume getBagResistance()
  {
    if (bagResistance == null)
      bagResistance = new SEScalarPressureTimePerVolume();
    return bagResistance;
  }
  public boolean hasBagResistance()
  {
    return bagResistance == null ? false : bagResistance.isValid();
  }
  
  public SEScalarVolume getConnectionVolume()
  {
    if (connectionVolume == null)
      connectionVolume = new SEScalarVolume();
    return connectionVolume;
  }
  public boolean hasConnectionVolume()
  {
    return connectionVolume == null ? false : connectionVolume.isValid();
  }
  
  public SEScalarPressureTimePerVolume getFilterResistance()
  {
    if (filterResistance == null)
      filterResistance = new SEScalarPressureTimePerVolume();
    return filterResistance;
  }
  public boolean hasFilterResistance()
  {
    return filterResistance == null ? false : filterResistance.isValid();
  }
  
  public SEScalarVolume getFilterVolume()
  {
    if (filterVolume == null)
      filterVolume = new SEScalarVolume();
    return filterVolume;
  }
  public boolean hasFilterVolume()
  {
    return filterVolume == null ? false : filterVolume.isValid();
  }
  
  public SEScalarPressureTimePerVolume getSealResistance()
  {
    if (sealResistance == null)
      sealResistance = new SEScalarPressureTimePerVolume();
    return sealResistance;
  }
  public boolean hasSealResistance()
  {
    return sealResistance == null ? false : sealResistance.isValid();
  }
  
  public SEScalarPressure getValvePositiveEndExpiratoryPressure()
  {
    if (valvePositiveEndExpiratoryPressure == null)
      valvePositiveEndExpiratoryPressure = new SEScalarPressure();
    return valvePositiveEndExpiratoryPressure;
  }
  public boolean hasValvePositiveEndExpiratoryPressure()
  {
    return valvePositiveEndExpiratoryPressure == null ? false : valvePositiveEndExpiratoryPressure.isValid();
  }
  
  public SEScalarPressureTimePerVolume getValveResistance()
  {
    if (valveResistance == null)
      valveResistance = new SEScalarPressureTimePerVolume();
    return valveResistance;
  }
  public boolean hasValveResistance()
  {
    return valveResistance == null ? false : valveResistance.isValid();
  }
  
  public SEScalarVolume getValveVolume()
  {
    if (valveVolume == null)
      valveVolume = new SEScalarVolume();
    return valveVolume;
  }
  public boolean hasValveVolume()
  {
    return valveVolume == null ? false : valveVolume.isValid();
  }

  //////////////////////////////
  // Fraction Of Inspired Gas //
  //////////////////////////////
  
  public SESubstanceFraction createFractionInspiredGas(String substance)
  {
    return getFractionInspiredGas(substance);
  }
  public SESubstanceFraction getFractionInspiredGas(String substance)
  {
    for(SESubstanceFraction sf : this.fractionInspiredGases)
    {
      if(sf.getSubstance().equals(substance))
      {        
        return sf;
      }
    }    
    SESubstanceFraction sf = new SESubstanceFraction(substance);
    this.fractionInspiredGases.add(sf);
    return sf;
  }
  public boolean hasFractionInspiredGas()
  {
    return !this.fractionInspiredGases.isEmpty();
  }
  public boolean hasFractionInspiredGas(String substance)
  {
    for(SESubstanceFraction sf : this.fractionInspiredGases)
    {
      if(sf.getSubstance().equals(substance))
      {        
        return true;
      }
    }
    return false;
  }
  public List<SESubstanceFraction> getFractionInspiredGas()
  {
    return this.fractionInspiredGases;
  }
  public void removeFractionInspiredGas(String substance)
  {
    for(SESubstanceFraction sf : this.fractionInspiredGases)
    {
      if(sf.getSubstance().equals(substance))
      {
        this.fractionInspiredGases.remove(sf);
        return;
      }
    }  
  }
  

  public SESubstanceConcentration createConcentrationInspiredAerosol(String substance)
  {
    return getConcentrationInspiredAerosol(substance);
  }
  public SESubstanceConcentration getConcentrationInspiredAerosol(String substance)
  {
    for(SESubstanceConcentration sc : this.concentrationInspiredAerosol)
    {
      if(sc.getSubstance().equals(substance))
      {
        return sc;
      }
    }    
    SESubstanceConcentration sc = new SESubstanceConcentration(substance);
    this.concentrationInspiredAerosol.add(sc);
    return sc;
  }
  public boolean hasConcentrationInspiredAerosol()
  {
    return !this.concentrationInspiredAerosol.isEmpty();
  }
  public boolean hasConcentrationInspiredAerosol(String substance)
  {
    for(SESubstanceConcentration sc : this.concentrationInspiredAerosol)
    {
      if(sc.getSubstance().equals(substance))
      {
        return true;
      }
    }
    return false;
  }
  public List<SESubstanceConcentration> getConcentrationInspiredAerosol()
  {
    return Collections.unmodifiableList(this.concentrationInspiredAerosol);
  }
  public void removeConcentrationInspiredAerosol(String substance)
  {
    for(SESubstanceConcentration sc : this.concentrationInspiredAerosol)
    {
      if(sc.getSubstance().equals(substance))
      {
        this.concentrationInspiredAerosol.remove(sc);
        return;
      }
    }  
  }
  
  public void trim()
  {
    SESubstanceFraction sf;
    Iterator<SESubstanceFraction> ifrac = this.fractionInspiredGases.iterator();
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
    String str = "Bag Valve Mask"
        + "\n\tConnection: " + (hasConnection()?getConnection():"NotProvided")
        + "\n\tBagResistance: " + (hasBagResistance()?getBagResistance():"NotProvided")
        + "\n\tFilterResistance: " + (hasFilterResistance()?getFilterResistance():"NotProvided")
        + "\n\tFilterVolume: " + (hasFilterVolume()?getFilterVolume():"NotProvided")
        + "\n\tValvePositiveEndExpiratoryPressure: " + (hasValvePositiveEndExpiratoryPressure()?getValvePositiveEndExpiratoryPressure():"NotProvided")
        + "\n\tValveResistance: " + (hasValveResistance()?getValveResistance():"NotProvided");
    for(SESubstanceFraction sf : this.fractionInspiredGases)
      str += "\n\t"+sf.toString();
    for(SESubstanceConcentration sc : this.concentrationInspiredAerosol)
    str += "\n\t"+sc.toString();
    return str;
  }
}