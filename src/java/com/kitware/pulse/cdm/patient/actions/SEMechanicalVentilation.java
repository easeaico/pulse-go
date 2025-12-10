/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
package com.kitware.pulse.cdm.patient.actions;

import java.util.*;

import com.kitware.pulse.cdm.bind.Enums.eSwitch;
import com.kitware.pulse.cdm.bind.PatientActions.MechanicalVentilationData;
import com.kitware.pulse.cdm.bind.Substance.SubstanceConcentrationData;
import com.kitware.pulse.cdm.bind.Substance.SubstanceFractionData;
import com.kitware.pulse.cdm.properties.SEScalar0To1;
import com.kitware.pulse.cdm.properties.SEScalarMassPerVolume;
import com.kitware.pulse.cdm.properties.SEScalarPressure;
import com.kitware.pulse.cdm.properties.SEScalarVolume;
import com.kitware.pulse.cdm.properties.SEScalarVolumePerTime;
import com.kitware.pulse.cdm.substance.SESubstance;
import com.kitware.pulse.cdm.substance.SESubstanceConcentration;
import com.kitware.pulse.cdm.substance.SESubstanceFraction;
import com.kitware.pulse.utilities.DoubleUtils;
import com.kitware.pulse.utilities.Log;

public class SEMechanicalVentilation extends SEPatientAction
{

  private static final long serialVersionUID = -1840027159469073231L;
  protected SEScalarVolumePerTime flow;
  protected SEScalarPressure      pressure;
  protected SEScalarVolume        mechanicalDeadSpaceVolume;
  protected eSwitch               state;

  protected List<SESubstanceFraction>        gasFractions=new ArrayList<SESubstanceFraction>();
  protected List<SESubstanceConcentration>   aerosols=new ArrayList<SESubstanceConcentration>();

  public SEMechanicalVentilation()
  {
    clear();
  }

  @Override
  public void clear()
  {
    state = eSwitch.Off;
    if (flow != null)
      flow.invalidate();
    if (pressure != null)
      pressure.invalidate();
    this.gasFractions.clear();
    this.aerosols.clear();
  }

  public void copy(SEMechanicalVentilation from)
  {
    clear();

    setState(from.getState());
    if(from.hasFlow())
      getFlow().set(from.getFlow());
    if(from.hasPressure())
      getPressure().set(from.getPressure());
    if(from.hasMechanicalDeadSpaceVolume())
      getMechanicalDeadSpaceVolume().set(from.getMechanicalDeadSpaceVolume());
    
    if(from.gasFractions!=null)
    {
      SESubstanceFraction mine;
      for(SESubstanceFraction sf : from.gasFractions)
      {
        mine=this.createGasFraction(sf.getSubstance());
        if(sf.hasAmount())
          mine.getAmount().set(sf.getAmount());
      }
    }   
    
    if(from.aerosols!=null)
    {
      SESubstanceConcentration mine;
      for(SESubstanceConcentration sc : from.aerosols)
      {
        mine=this.createAerosol(sc.getSubstance());
        if(sc.hasConcentration())
          mine.getConcentration().set(sc.getConcentration());
      }
    }    
  }
  
  @Override
  public boolean isValid()
  {
    if (getState() == eSwitch.Off)
      return true;
    if (!hasGasFraction())
    {
      Log.error("Mechanical Ventilation must have gas fractions.");
      return false;
    }
    else
    {
      double total = 0;
      for (SESubstanceFraction sf : gasFractions)
      {
        total += sf.getAmount().getValue();
      }
      if (!DoubleUtils.equals(1.0, total))
      {
        Log.error("Mechanical Ventilation Gas fractions do not sum to 1");
        return false;
      }
    }
    if (!hasPressure() && !hasFlow())
    {
      Log.error("Mechanical Ventilation must have pressure and/or flow");
      return false;
    }
    return true;
  }

  public static void load(MechanicalVentilationData src, SEMechanicalVentilation dst)
  {
    SEPatientAction.load(src.getPatientAction(), dst);
    if(src.getState()!=eSwitch.UNRECOGNIZED && src.getState()!=eSwitch.NullSwitch)
      dst.setState(src.getState());
    if (src.hasFlow())
      SEScalarVolumePerTime.load(src.getFlow(),dst.getFlow());
    if (src.hasPressure())
      SEScalarPressure.load(src.getPressure(),dst.getPressure());
    if (src.hasMechanicalDeadSpaceVolume())
      SEScalarVolume.load(src.getMechanicalDeadSpaceVolume(),dst.getMechanicalDeadSpaceVolume());
    
    for(SubstanceFractionData subData : src.getGasFractionList())
    {
      SEScalar0To1.load(subData.getAmount(),dst.createGasFraction(subData.getName()).getAmount());
    }
    for(SubstanceConcentrationData subData : src.getAerosolList())
    {
      SEScalarMassPerVolume.load(subData.getConcentration(),dst.createAerosol(subData.getName()).getConcentration());
    }
  }

  public static MechanicalVentilationData unload(SEMechanicalVentilation src)
  {
    MechanicalVentilationData.Builder dst = MechanicalVentilationData.newBuilder();
    unload(src,dst);
    return dst.build();
  }

  protected static void unload(SEMechanicalVentilation src, MechanicalVentilationData.Builder dst)
  {
    dst.setState(src.state);
    if (src.hasFlow())
      dst.setFlow(SEScalarVolumePerTime.unload(src.flow));
    if (src.hasPressure())
      dst.setPressure(SEScalarPressure.unload(src.pressure));
    if (src.hasMechanicalDeadSpaceVolume())
      dst.setMechanicalDeadSpaceVolume(SEScalarVolume.unload(src.mechanicalDeadSpaceVolume));
    
    for(SESubstanceFraction gf : src.gasFractions)
      dst.addGasFraction(SESubstanceFraction.unload(gf));
    for(SESubstanceConcentration a : src.aerosols)
      dst.addAerosol(SESubstanceConcentration.unload(a));
  }

  public eSwitch getState()
  {
    return state;
  }
  public void setState(eSwitch s)
  {
  	this.state = (s==eSwitch.NullSwitch) ? eSwitch.Off : s;
  }

  public boolean hasFlow()
  {
    return flow == null ? false : flow.isValid();
  }
  public SEScalarVolumePerTime getFlow()
  {
    if (flow == null)
      flow = new SEScalarVolumePerTime();
    return flow;
  }

  public boolean hasPressure()
  {
    return pressure == null ? false : pressure.isValid();
  }
  public SEScalarPressure getPressure()
  {
    if (pressure == null)
      pressure = new SEScalarPressure();
    return pressure;
  }
  
  public boolean hasMechanicalDeadSpaceVolume()
  {
    return mechanicalDeadSpaceVolume == null ? false : mechanicalDeadSpaceVolume.isValid();
  }
  public SEScalarVolume getMechanicalDeadSpaceVolume()
  {
    if (mechanicalDeadSpaceVolume == null)
      mechanicalDeadSpaceVolume = new SEScalarVolume();
    return mechanicalDeadSpaceVolume;
  }
  
  public SESubstanceFraction createGasFraction(String substance)
  {
    return getGasFraction(substance);
  }
  public SESubstanceFraction getGasFraction(String substance)
  {
    for(SESubstanceFraction sf : this.gasFractions)
    {
      if(sf.getSubstance().equals(substance))
      {        
        return sf;
      }
    }    
    SESubstanceFraction sf = new SESubstanceFraction(substance);
    this.gasFractions.add(sf);
    return sf;
  }
  public boolean hasGasFraction()
  {
    return !this.gasFractions.isEmpty();
  }
  public boolean hasGasFraction(SESubstance substance)
  {
    for(SESubstanceFraction sf : this.gasFractions)
    {
      if(sf.getSubstance().equals(substance))
      {        
        return true;
      }
    }
    return false;
  }
  public List<SESubstanceFraction> getGasFraction()
  {
    return this.gasFractions;
  }
  public void removeGasFraction(SESubstance substance)
  {
    for(SESubstanceFraction sf : this.gasFractions)
    {
      if(sf.getSubstance().equals(substance))
      {
        this.gasFractions.remove(sf);
        return;
      }
    }  
  }

  public SESubstanceConcentration createAerosol(String substance)
  {
    return getAerosol(substance);
  }
  public SESubstanceConcentration getAerosol(String substance)
  {
    for(SESubstanceConcentration sc : this.aerosols)
    {
      if(sc.getSubstance().equals(substance))
      {        
        return sc;
      }
    }    
    SESubstanceConcentration sc = new SESubstanceConcentration(substance);
    this.aerosols.add(sc);
    return sc;
  }
  public boolean hasAerosol()
  {
    return !this.aerosols.isEmpty();
  }
  public boolean hasAerosol(SESubstance substance)
  {
    for(SESubstanceConcentration sc : this.aerosols)
    {
      if(sc.getSubstance().equals(substance))
      {        
        return true;
      }
    }
    return false;
  }
  public List<SESubstanceConcentration> getAerosol()
  {
    return this.aerosols;
  }
  public void removeAerosol(SESubstance substance)
  {
    for(SESubstanceConcentration sc : this.aerosols)
    {
      if(sc.getSubstance().equals(substance))
      {
        this.aerosols.remove(sc);
        return;
      }
    }  
  }
  
  @Override
  public String toString()
  {
    String cnts = "Mechanical Ventilation"
        + "\n\tState: " + getState()
        + "\n\tFlow: " + (hasFlow() ? getFlow() : "Not Provided")
        + "\n\tPressure: " + (hasPressure() ? getPressure() : "Not Provided");
        + "\n\tMechanicalDeadSpaceVolume: " + (hasMechanicalDeadSpaceVolume() ? getMechanicalDeadSpaceVolume() : "Not Provided");
    for(SESubstanceFraction sf : this.gasFractions)
      cnts += "\n\tSubstanceFraction: " + sf.getSubstance() + " : " + sf.getAmount();
    for(SESubstanceConcentration sc : this.aerosols)
        cnts += "\n\tSubstanceConcentration: " + sc.getSubstance() + " : " + sc.getConcentration();
    
    return cnts;
  }
}