/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.patient.conditions;

import com.kitware.pulse.cdm.bind.PatientConditions.AnyPatientConditionData;
import com.kitware.pulse.cdm.bind.PatientConditions.PatientConditionData;
import com.kitware.pulse.cdm.conditions.SECondition;
import com.kitware.pulse.utilities.Log;

public abstract class SEPatientCondition extends SECondition
{

  private static final long serialVersionUID = -4005416863982138600L;
  public SEPatientCondition()
  {
    
  }
  
  public void copy(SEPatientCondition other)
  {
    super.copy(other);
  }
  
  @Override
  public void clear()
  {
    super.clear();
  }
  
  public static void load(PatientConditionData src, SEPatientCondition dst)
  {
    SECondition.load(src.getCondition(), dst);
  }
  
  protected static void unload(SEPatientCondition src, PatientConditionData.Builder dst)
  {
    SECondition.unload(src,dst.getConditionBuilder());
  }
  
  @Override
  public abstract String toString();

  public static SEPatientCondition ANY2CDM(AnyPatientConditionData c) 
  {
    switch(c.getConditionCase())
    {
    case ACUTERESPIRATORYDISTRESSSYNDROME:
    {
      SEAcuteRespiratoryDistressSyndrome newC = new SEAcuteRespiratoryDistressSyndrome();
      SEAcuteRespiratoryDistressSyndrome.load(c.getAcuteRespiratoryDistressSyndrome(), newC);
      return newC;
    }
    case CHRONICANEMIA:
    {
      SEChronicAnemia newC = new SEChronicAnemia();
      SEChronicAnemia.load(c.getChronicAnemia(), newC);
      return newC;
    }
    case CHRONICOBSTRUCTIVEPULMONARYDISEASE:
    {
      SEChronicObstructivePulmonaryDisease newC = new SEChronicObstructivePulmonaryDisease();
      SEChronicObstructivePulmonaryDisease.load(c.getChronicObstructivePulmonaryDisease(), newC);
      return newC;
    }
    case CHRONICPERICARDIALEFFUSION:
    {
      SEChronicPericardialEffusion newC = new SEChronicPericardialEffusion();
      SEChronicPericardialEffusion.load(c.getChronicPericardialEffusion(), newC);
      return newC;
    }
    case CHRONICRENALSTENOSIS:
    {
      SEChronicRenalStenosis newC = new SEChronicRenalStenosis();
      SEChronicRenalStenosis.load(c.getChronicRenalStenosis(), newC);
      return newC;
    }
    case CHRONICVENTRICULARSYSTOLICDYSFUNCTION:
    {
      SEChronicVentricularSystolicDysfunction newC = new SEChronicVentricularSystolicDysfunction();
      SEChronicVentricularSystolicDysfunction.load(c.getChronicVentricularSystolicDysfunction(), newC);
      return newC;
    }
    case CONSUMEMEAL:
    {
      SEConsumeMeal newC = new SEConsumeMeal();
      SEConsumeMeal.load(c.getConsumeMeal(), newC);
      return newC;      
    }
    case DEHYDRATION:
    {
      SEDehydration newC = new SEDehydration();
      SEDehydration.load(c.getDehydration(), newC);
      return newC;      
    }
    case IMPAIREDALVEOLAREXCHANGE:
    {
      SEImpairedAlveolarExchange newC = new SEImpairedAlveolarExchange();
      SEImpairedAlveolarExchange.load(c.getImpairedAlveolarExchange(), newC);
      return newC;      
    }
    case PNEUMONIA:
    {
      SEPneumonia newC = new SEPneumonia();
      SEPneumonia.load(c.getPneumonia(), newC);
      return newC;
    }
    case PULMONARYFIBROSIS:
    {
      SEPulmonaryFibrosis newC = new SEPulmonaryFibrosis();
      SEPulmonaryFibrosis.load(c.getPulmonaryFibrosis(), newC);
      return newC;
    }
    case PULMONARYSHUNT:
    {
      SEPulmonaryShunt newC = new SEPulmonaryShunt();
      SEPulmonaryShunt.load(c.getPulmonaryShunt(), newC);
      return newC;
    }
    case SEPSIS:
    {
      SESepsis newC = new SESepsis();
      SESepsis.load(c.getSepsis(), newC);
      return newC;
    }
    case CONDITION_NOT_SET:
      Log.warn("AnyPatientConditionData was empty...was that intended?");
      return null;
    }
    Log.error("Unsupported Patient condition type "+c.getConditionCase());
    return null;
  }
  public static AnyPatientConditionData CDM2ANY(SEPatientCondition c)
  {
    AnyPatientConditionData.Builder dst = AnyPatientConditionData.newBuilder();
    if(c instanceof SEAcuteRespiratoryDistressSyndrome)
    {
      dst.setAcuteRespiratoryDistressSyndrome(SEAcuteRespiratoryDistressSyndrome.unload((SEAcuteRespiratoryDistressSyndrome)c));
      return dst.build();
    }
    if(c instanceof SEChronicAnemia)
    {
      dst.setChronicAnemia(SEChronicAnemia.unload((SEChronicAnemia)c));
      return dst.build();
    }
    if(c instanceof SEChronicObstructivePulmonaryDisease)
    {
      dst.setChronicObstructivePulmonaryDisease(SEChronicObstructivePulmonaryDisease.unload((SEChronicObstructivePulmonaryDisease)c));
      return dst.build();
    }
    if(c instanceof SEChronicPericardialEffusion)
    {
      dst.setChronicPericardialEffusion(SEChronicPericardialEffusion.unload((SEChronicPericardialEffusion)c));    
      return dst.build();
    }
    if(c instanceof SEChronicRenalStenosis)
    {
      dst.setChronicRenalStenosis(SEChronicRenalStenosis.unload((SEChronicRenalStenosis)c));
      return dst.build();
    }
    if(c instanceof SEChronicVentricularSystolicDysfunction)
    {
      dst.setChronicVentricularSystolicDysfunction(SEChronicVentricularSystolicDysfunction.unload((SEChronicVentricularSystolicDysfunction)c));
      return dst.build();
    }
    if(c instanceof SEConsumeMeal)
    {
      dst.setConsumeMeal(SEConsumeMeal.unload((SEConsumeMeal)c));
      return dst.build();
    }
    if(c instanceof SEDehydration)
    {
      dst.setDehydration(SEDehydration.unload((SEDehydration)c));
      return dst.build();
    }
    if(c instanceof SEImpairedAlveolarExchange)
    {
      dst.setImpairedAlveolarExchange(SEImpairedAlveolarExchange.unload((SEImpairedAlveolarExchange)c));
      return dst.build();
    }
    if(c instanceof SEPneumonia)
    {
      dst.setPneumonia(SEPneumonia.unload((SEPneumonia)c));
      return dst.build();
    }
    if(c instanceof SEPulmonaryFibrosis)
    {
      dst.setPulmonaryFibrosis(SEPulmonaryFibrosis.unload((SEPulmonaryFibrosis)c));
      return dst.build();
    }
    if(c instanceof SEPulmonaryShunt)
    {
      dst.setPulmonaryShunt(SEPulmonaryShunt.unload((SEPulmonaryShunt)c));
      return dst.build();
    }
    if(c instanceof SESepsis)
    {
      dst.setSepsis(SESepsis.unload((SESepsis)c));
      return dst.build();
    }
    Log.error("Unsupported Patient condition type "+c);
    return dst.build();
  }
}
