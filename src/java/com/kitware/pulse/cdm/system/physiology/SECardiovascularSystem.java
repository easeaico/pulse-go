/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
package com.kitware.pulse.cdm.system.physiology;

import com.kitware.pulse.cdm.bind.Physiology.CardiovascularSystemData;
import com.kitware.pulse.cdm.bind.Physiology.eHeartRhythm;
import com.kitware.pulse.cdm.properties.SEScalar0To1;
import com.kitware.pulse.cdm.properties.SEScalarFrequency;
import com.kitware.pulse.cdm.properties.SEScalarPressure;
import com.kitware.pulse.cdm.properties.SEScalarPressureTimePerVolume;
import com.kitware.pulse.cdm.properties.SEScalarPressureTimePerVolumeArea;
import com.kitware.pulse.cdm.properties.SEScalarVolume;
import com.kitware.pulse.cdm.properties.SEScalarVolumePerTime;
import com.kitware.pulse.cdm.properties.SEScalarVolumePerTimeArea;
import com.kitware.pulse.cdm.system.SESystem;

public class SECardiovascularSystem extends SEPhysiologySystem implements SESystem
{
  protected SEScalarPressure                  arterialPressure;
  protected SEScalarVolume                    bloodVolume;
  protected SEScalarVolumePerTimeArea         cardiacIndex;
  protected SEScalarVolumePerTime             cardiacOutput;
  protected SEScalarPressure                  centralVenousPressure;
  protected SEScalarVolumePerTime             cerebralBloodFlow;
  protected SEScalarPressure                  cerebralPerfusionPressure;
  protected SEScalarPressure                  diastolicArterialPressure;
  protected SEScalarPressure                  diastolicLeftHeartPressure;
  protected SEScalarPressure                  diastolicRightHeartPressure;
  protected SEScalar0To1                      heartEjectionFraction;
  protected SEScalarFrequency                 heartRate;
  protected eHeartRhythm                      heartRhythm;
  protected SEScalarVolume                    heartStrokeVolume;
  protected SEScalarPressure                  intracranialPressure;
  protected SEScalarPressure                  meanArterialPressure;
  protected SEScalarPressure                  meanArterialCarbonDioxidePartialPressure;
  protected SEScalarPressure                  meanArterialCarbonDioxidePartialPressureDelta;
  protected SEScalarPressure                  meanCentralVenousPressure;
  protected SEScalarVolumePerTime             meanSkinFlow;
  protected SEScalar0To1                      peripheralPerfusionIndex;
  protected SEScalarPressure                  pulmonaryArterialPressure;
  protected SEScalarPressure                  pulmonaryCapillariesWedgePressure;
  protected SEScalarPressure                  pulmonaryDiastolicArterialPressure;
  protected SEScalarPressure                  pulmonaryMeanArterialPressure;
  protected SEScalarVolumePerTime             pulmonaryMeanCapillaryFlow;
  protected SEScalarVolumePerTime             pulmonaryMeanShuntFlow;
  protected SEScalarPressure                  pulmonarySystolicArterialPressure;
  protected SEScalarPressureTimePerVolume     pulmonaryVascularResistance;
  protected SEScalarPressureTimePerVolumeArea pulmonaryVascularResistanceIndex;
  protected SEScalarPressure                  pulsePressure;
  protected SEScalarPressureTimePerVolume     systemicVascularResistance;
  protected SEScalarPressure                  systolicArterialPressure;
  protected SEScalarPressure                  systolicLeftHeartPressure;
  protected SEScalarPressure                  systolicRightHeartPressure;
  protected SEScalarVolumePerTime             totalHemorrhageRate;
  protected SEScalarVolume                    totalHemorrhagedVolume;

  public SECardiovascularSystem()
  {
    arterialPressure = null;
    meanArterialPressure = null;
    bloodVolume = null;
    cardiacIndex = null;
    cardiacOutput = null;
    centralVenousPressure = null;
    cerebralBloodFlow = null;
    cerebralPerfusionPressure = null;
    meanCentralVenousPressure = null;
    diastolicArterialPressure = null;
    diastolicLeftHeartPressure = null;
    diastolicRightHeartPressure = null;
    heartEjectionFraction = null;
    heartRate = null;
    heartRhythm = null;
    heartStrokeVolume = null;
    intracranialPressure = null;
    peripheralPerfusionIndex = null;
    pulmonaryArterialPressure = null;
    pulmonaryCapillariesWedgePressure = null;
    pulmonaryDiastolicArterialPressure = null;
    pulmonaryMeanArterialPressure = null;
    pulmonaryMeanCapillaryFlow = null;
    pulmonaryMeanShuntFlow = null;
    pulmonarySystolicArterialPressure = null;
    pulmonaryVascularResistance = null;
    pulmonaryVascularResistanceIndex = null;
    pulsePressure = null;
    systemicVascularResistance = null;
    systolicArterialPressure = null;
    systolicLeftHeartPressure = null;
    systolicRightHeartPressure = null;
    totalHemorrhageRate = null;
    totalHemorrhagedVolume = null;
  }

  @Override
  public void clear()
  {
    if (arterialPressure != null)
      arterialPressure.invalidate();
    if (meanArterialPressure != null)
      meanArterialPressure.invalidate();
    if (bloodVolume != null)
      bloodVolume.invalidate();
    if (cardiacIndex != null)
      cardiacIndex.invalidate();
    if (cardiacOutput != null)
      cardiacOutput.invalidate();
    if (centralVenousPressure != null)
      centralVenousPressure.invalidate();
    if (cerebralBloodFlow != null)
      cerebralBloodFlow.invalidate();
    if (cerebralPerfusionPressure != null)
      cerebralPerfusionPressure.invalidate();
    if (meanCentralVenousPressure != null)
      meanCentralVenousPressure.invalidate();
    if (diastolicArterialPressure != null)
      diastolicArterialPressure.invalidate();
    if (diastolicLeftHeartPressure != null)
      diastolicLeftHeartPressure.invalidate();
    if (diastolicRightHeartPressure != null)
      diastolicRightHeartPressure.invalidate();
    if (heartEjectionFraction != null)
      heartEjectionFraction.invalidate();
    this.heartRhythm = null;
    if (heartStrokeVolume != null)
      heartStrokeVolume.invalidate();
    if (intracranialPressure != null)
      intracranialPressure.invalidate();
    if (meanArterialCarbonDioxidePartialPressure != null)
      meanArterialCarbonDioxidePartialPressure.invalidate();
    if (meanArterialCarbonDioxidePartialPressureDelta != null)
      meanArterialCarbonDioxidePartialPressureDelta.invalidate();
    if (meanSkinFlow != null)
      meanSkinFlow.invalidate();
    if (peripheralPerfusionIndex != null)
      peripheralPerfusionIndex.invalidate();
    if (pulmonaryArterialPressure != null)
      pulmonaryArterialPressure.invalidate();
    if (pulmonaryCapillariesWedgePressure != null)
      pulmonaryCapillariesWedgePressure.invalidate();
    if (pulmonaryDiastolicArterialPressure != null)
      pulmonaryDiastolicArterialPressure.invalidate();
    if (pulmonaryMeanArterialPressure != null)
      pulmonaryMeanArterialPressure.invalidate();
    if (pulmonaryMeanCapillaryFlow != null)
      pulmonaryMeanCapillaryFlow.invalidate();
    if (pulmonaryMeanShuntFlow != null)
      pulmonaryMeanShuntFlow.invalidate();
    if (pulmonarySystolicArterialPressure != null)
      pulmonarySystolicArterialPressure.invalidate();
    if (pulmonaryVascularResistance != null)
      pulmonaryVascularResistance.invalidate();
    if (pulmonaryVascularResistanceIndex != null)
      pulmonaryVascularResistanceIndex.invalidate();
    if (pulsePressure != null)
      pulsePressure.invalidate();
    if (systemicVascularResistance != null)
      systemicVascularResistance.invalidate();
    if (systolicArterialPressure != null)
      systolicArterialPressure.invalidate();
    if (systolicLeftHeartPressure != null)
      systolicLeftHeartPressure.invalidate();
    if (systolicRightHeartPressure != null)
      systolicRightHeartPressure.invalidate();
    if (totalHemorrhageRate != null)
      totalHemorrhageRate.invalidate();
    if (totalHemorrhagedVolume != null)
      totalHemorrhagedVolume.invalidate();
  }

  public static void load(CardiovascularSystemData src, SECardiovascularSystem dst)
  {
    if (src.hasArterialPressure())
      SEScalarPressure.load(src.getArterialPressure(),dst.getArterialPressure());
    if (src.hasMeanArterialPressure())
      SEScalarPressure.load(src.getMeanArterialPressure(),dst.getMeanArterialPressure());
    if (src.hasBloodVolume())
      SEScalarVolume.load(src.getBloodVolume(),dst.getBloodVolume());
    if (src.hasCardiacIndex())
      SEScalarVolumePerTimeArea.load(src.getCardiacIndex(),dst.getCardiacIndex());
    if (src.hasCardiacOutput())
      SEScalarVolumePerTime.load(src.getCardiacOutput(),dst.getCardiacOutput());
    if (src.hasCentralVenousPressure())
      SEScalarPressure.load(src.getCentralVenousPressure(),dst.getCentralVenousPressure());
    if (src.hasCerebralBloodFlow())
      SEScalarVolumePerTime.load(src.getCerebralBloodFlow(),dst.getCerebralBloodFlow());
    if (src.hasCerebralPerfusionPressure())
      SEScalarPressure.load(src.getCerebralPerfusionPressure(),dst.getCerebralPerfusionPressure());
    if (src.hasMeanCentralVenousPressure())
      SEScalarPressure.load(src.getMeanCentralVenousPressure(),dst.getMeanCentralVenousPressure());
    if (src.hasDiastolicArterialPressure())
      SEScalarPressure.load(src.getDiastolicArterialPressure(),dst.getDiastolicArterialPressure());
    if (src.hasDiastolicLeftHeartPressure())
      SEScalarPressure.load(src.getDiastolicLeftHeartPressure(),dst.getDiastolicLeftHeartPressure());
    if (src.hasDiastolicRightHeartPressure())
      SEScalarPressure.load(src.getDiastolicRightHeartPressure(),dst.getDiastolicRightHeartPressure());
    if (src.hasHeartEjectionFraction())
      SEScalar0To1.load(src.getHeartEjectionFraction(),dst.getHeartEjectionFraction());
    if (src.hasHeartRate())
      SEScalarFrequency.load(src.getHeartRate(),dst.getHeartRate());    
    if(src.getHeartRhythm()!=eHeartRhythm.UNRECOGNIZED)
      dst.setHeartRhythm(src.getHeartRhythm());
    if (src.hasHeartStrokeVolume())
      SEScalarVolume.load(src.getHeartStrokeVolume(),dst.getHeartStrokeVolume());
    if (src.hasIntracranialPressure())
      SEScalarPressure.load(src.getIntracranialPressure(),dst.getIntracranialPressure());
    if (src.hasMeanArterialCarbonDioxidePartialPressure())
      SEScalarPressure.load(src.getMeanArterialCarbonDioxidePartialPressure(),dst.getMeanArterialCarbonDioxidePartialPressure());
    if (src.hasMeanArterialCarbonDioxidePartialPressureDelta())
      SEScalarPressure.load(src.getMeanArterialCarbonDioxidePartialPressureDelta(),dst.getMeanArterialCarbonDioxidePartialPressureDelta());
    if (src.hasMeanSkinFlow())
      SEScalarVolumePerTime.load(src.getMeanSkinFlow(),dst.getMeanSkinFlow());
    if (src.hasPeripheralPerfusionIndex())
      SEScalar0To1.load(src.getPeripheralPerfusionIndex(),dst.getPeripheralPerfusionIndex());
    if (src.hasPulmonaryArterialPressure())
      SEScalarPressure.load(src.getPulmonaryArterialPressure(),dst.getPulmonaryArterialPressure());
    if (src.hasPulmonaryCapillariesWedgePressure())
      SEScalarPressure.load(src.getPulmonaryCapillariesWedgePressure(),dst.getPulmonaryCapillariesWedgePressure());
    if (src.hasPulmonaryDiastolicArterialPressure())
      SEScalarPressure.load(src.getPulmonaryDiastolicArterialPressure(),dst.getPulmonaryDiastolicArterialPressure());
    if (src.hasPulmonaryMeanArterialPressure())
      SEScalarPressure.load(src.getPulmonaryMeanArterialPressure(),dst.getPulmonaryMeanArterialPressure());
    if (src.hasPulmonaryMeanCapillaryFlow())
      SEScalarVolumePerTime.load(src.getPulmonaryMeanCapillaryFlow(),dst.getPulmonaryMeanCapillaryFlow());
    if (src.hasPulmonaryMeanShuntFlow())
      SEScalarVolumePerTime.load(src.getPulmonaryMeanShuntFlow(),dst.getPulmonaryMeanShuntFlow());
    if (src.hasPulmonarySystolicArterialPressure())
      SEScalarPressure.load(src.getPulmonarySystolicArterialPressure(),dst.getPulmonarySystolicArterialPressure());
    if (src.hasPulmonaryVascularResistance())
      SEScalarPressureTimePerVolume.load(src.getPulmonaryVascularResistance(),dst.getPulmonaryVascularResistance());
    if (src.hasPulmonaryVascularResistanceIndex())
      SEScalarPressureTimePerVolumeArea.load(src.getPulmonaryVascularResistanceIndex(),dst.getPulmonaryVascularResistanceIndex());
    if (src.hasPulsePressure())
      SEScalarPressure.load(src.getPulsePressure(),dst.getPulsePressure());
    if (src.hasSystemicVascularResistance())
      SEScalarPressureTimePerVolume.load(src.getSystemicVascularResistance(),dst.getSystemicVascularResistance());
    if (src.hasSystolicArterialPressure())
      SEScalarPressure.load(src.getSystolicArterialPressure(),dst.getSystolicArterialPressure());
    if (src.hasSystolicLeftHeartPressure())
      SEScalarPressure.load(src.getSystolicLeftHeartPressure(),dst.getSystolicLeftHeartPressure());
    if (src.hasSystolicRightHeartPressure())
      SEScalarPressure.load(src.getSystolicRightHeartPressure(),dst.getSystolicRightHeartPressure());
    if (src.hasTotalHemorrhageRate())
      SEScalarVolumePerTime.load(src.getTotalHemorrhageRate(),dst.getTotalHemorrhageRate());
    if (src.hasTotalHemorrhagedVolume())
      SEScalarVolume.load(src.getTotalHemorrhagedVolume(),dst.getTotalHemorrhagedVolume());

  }

  public static CardiovascularSystemData unload(SECardiovascularSystem src)
  {
    CardiovascularSystemData.Builder dst = CardiovascularSystemData.newBuilder();
    unload(src,dst);
    return dst.build();
  }

  protected static void unload(SECardiovascularSystem src, CardiovascularSystemData.Builder dst)
  {
    if (src.hasArterialPressure())
      dst.setArterialPressure(SEScalarPressure.unload(src.getArterialPressure())); 
    if (src.hasMeanArterialPressure())
      dst.setMeanArterialPressure(SEScalarPressure.unload(src.getMeanArterialPressure())); 
    if (src.hasBloodVolume())
      dst.setBloodVolume(SEScalarVolume.unload(src.getBloodVolume())); 
    if (src.hasCardiacIndex())
      dst.setCardiacIndex(SEScalarVolumePerTimeArea.unload(src.getCardiacIndex())); 
    if (src.hasCardiacOutput())
      dst.setCardiacOutput(SEScalarVolumePerTime.unload(src.getCardiacOutput())); 
    if (src.hasCentralVenousPressure())
      dst.setCentralVenousPressure(SEScalarPressure.unload(src.getCentralVenousPressure())); 
    if (src.hasCerebralBloodFlow())
      dst.setCerebralBloodFlow(SEScalarVolumePerTime.unload(src.getCerebralBloodFlow())); 
    if (src.hasCerebralPerfusionPressure())
      dst.setCerebralPerfusionPressure(SEScalarPressure.unload(src.getCerebralPerfusionPressure()));
    if (src.hasMeanCentralVenousPressure())
      dst.setMeanCentralVenousPressure(SEScalarPressure.unload(src.getMeanCentralVenousPressure()));
    if (src.hasDiastolicArterialPressure())
      dst.setDiastolicArterialPressure(SEScalarPressure.unload(src.getDiastolicArterialPressure()));
    if (src.hasDiastolicLeftHeartPressure())
      dst.setDiastolicLeftHeartPressure(SEScalarPressure.unload(src.getDiastolicLeftHeartPressure()));
    if (src.hasDiastolicRightHeartPressure())
      dst.setDiastolicRightHeartPressure(SEScalarPressure.unload(src.getDiastolicRightHeartPressure()));
    if (src.hasHeartEjectionFraction())
      dst.setHeartEjectionFraction(SEScalar0To1.unload(src.getHeartEjectionFraction())); 
    if (src.hasHeartRate())
      dst.setHeartRate(SEScalarFrequency.unload(src.getHeartRate())); 
    if(src.hasHeartRhythm())
      dst.setHeartRhythm(src.heartRhythm);
    if (src.hasHeartStrokeVolume())
      dst.setHeartStrokeVolume(SEScalarVolume.unload(src.getHeartStrokeVolume()));
    if (src.hasIntracranialPressure())
      dst.setIntracranialPressure(SEScalarPressure.unload(src.getIntracranialPressure()));
    if (src.hasMeanArterialCarbonDioxidePartialPressure())
      dst.setMeanArterialCarbonDioxidePartialPressure(SEScalarPressure.unload(src.getMeanArterialCarbonDioxidePartialPressure()));
    if (src.hasMeanArterialCarbonDioxidePartialPressureDelta())
      dst.setMeanArterialCarbonDioxidePartialPressureDelta(SEScalarPressure.unload(src.getMeanArterialCarbonDioxidePartialPressureDelta()));
    if (src.hasMeanSkinFlow())
      dst.setMeanSkinFlow(SEScalarVolumePerTime.unload(src.getMeanSkinFlow()));
    if (src.hasPeripheralPerfusionIndex())
      dst.setPeripheralPerfusionIndex(SEScalar0To1.unload(src.getPeripheralPerfusionIndex()));
    if (src.hasPulmonaryArterialPressure())
      dst.setPulmonaryArterialPressure(SEScalarPressure.unload(src.getPulmonaryArterialPressure()));
    if (src.hasPulmonaryCapillariesWedgePressure())
      dst.setPulmonaryCapillariesWedgePressure(SEScalarPressure.unload(src.getPulmonaryCapillariesWedgePressure())); 
    if (src.hasPulmonaryDiastolicArterialPressure())
      dst.setPulmonaryDiastolicArterialPressure(SEScalarPressure.unload(src.getPulmonaryDiastolicArterialPressure())); 
    if (src.hasPulmonaryMeanArterialPressure())
      dst.setPulmonaryMeanArterialPressure(SEScalarPressure.unload(src.getPulmonaryMeanArterialPressure())); 
    if (src.hasPulmonaryMeanCapillaryFlow())
      dst.setPulmonaryMeanCapillaryFlow(SEScalarVolumePerTime.unload(src.getPulmonaryMeanCapillaryFlow())); 
    if (src.hasPulmonaryMeanShuntFlow())
      dst.setPulmonaryMeanShuntFlow(SEScalarVolumePerTime.unload(src.getPulmonaryMeanShuntFlow())); 
    if (src.hasPulmonarySystolicArterialPressure())
      dst.setPulmonarySystolicArterialPressure(SEScalarPressure.unload(src.getPulmonarySystolicArterialPressure())); 
    if (src.hasPulmonaryVascularResistance())
      dst.setPulmonaryVascularResistance(SEScalarPressureTimePerVolume.unload(src.getPulmonaryVascularResistance())); 
    if (src.hasPulmonaryVascularResistanceIndex())
      dst.setPulmonaryVascularResistanceIndex(SEScalarPressureTimePerVolumeArea.unload(src.getPulmonaryVascularResistanceIndex())); 
    if (src.hasPulsePressure())
      dst.setPulsePressure(SEScalarPressure.unload(src.getPulsePressure()));
    if (src.hasSystemicVascularResistance())
      dst.setSystemicVascularResistance(SEScalarPressureTimePerVolume.unload(src.getSystemicVascularResistance()));
    if (src.hasSystolicArterialPressure())
      dst.setSystolicArterialPressure(SEScalarPressure.unload(src.getSystolicArterialPressure()));
    if (src.hasSystolicLeftHeartPressure())
      dst.setSystolicLeftHeartPressure(SEScalarPressure.unload(src.getSystolicLeftHeartPressure()));
    if (src.hasSystolicRightHeartPressure())
      dst.setSystolicRightHeartPressure(SEScalarPressure.unload(src.getSystolicRightHeartPressure()));
    if (src.hasTotalHemorrhageRate())
      dst.setTotalHemorrhageRate(SEScalarVolumePerTime.unload(src.getTotalHemorrhageRate()));
    if (src.hasTotalHemorrhagedVolume())
      dst.setTotalHemorrhagedVolume(SEScalarVolume.unload(src.getTotalHemorrhagedVolume()));
  }

  public boolean hasArterialPressure()
  {
    return arterialPressure == null ? false : arterialPressure.isValid();
  }
  public SEScalarPressure getArterialPressure()
  {
    if (arterialPressure == null)
      arterialPressure = new SEScalarPressure();
    return arterialPressure;
  }

  public boolean hasMeanArterialPressure()
  {
    return meanArterialPressure == null ? false : meanArterialPressure.isValid();
  }
  public SEScalarPressure getMeanArterialPressure()
  {
    if (meanArterialPressure == null)
      meanArterialPressure = new SEScalarPressure();
    return meanArterialPressure;
  }

  public boolean hasBloodVolume()
  {
    return bloodVolume == null ? false : bloodVolume.isValid();
  }
  public SEScalarVolume getBloodVolume()
  {
    if (bloodVolume == null)
      bloodVolume = new SEScalarVolume();
    return bloodVolume;
  }
  
  public boolean hasCardiacIndex()
  {
    return cardiacIndex == null ? false : cardiacIndex.isValid();
  }
  public SEScalarVolumePerTimeArea getCardiacIndex()
  {
    if (cardiacIndex == null)
      cardiacIndex = new SEScalarVolumePerTimeArea();
    return cardiacIndex;
  }

  public boolean hasCardiacOutput()
  {
    return cardiacOutput == null ? false : cardiacOutput.isValid();
  }
  public SEScalarVolumePerTime getCardiacOutput()
  {
    if (cardiacOutput == null)
      cardiacOutput = new SEScalarVolumePerTime();
    return cardiacOutput;
  }

  public boolean hasCentralVenousPressure()
  {
    return centralVenousPressure == null ? false : centralVenousPressure.isValid();
  }
  public SEScalarPressure getCentralVenousPressure()
  {
    if (centralVenousPressure == null)
      centralVenousPressure = new SEScalarPressure();
    return centralVenousPressure;
  }

  public boolean hasCerebralBloodFlow()
  {
    return cerebralBloodFlow == null ? false : cerebralBloodFlow.isValid();
  }
  public SEScalarVolumePerTime getCerebralBloodFlow()
  {
    if (cerebralBloodFlow == null)
      cerebralBloodFlow = new SEScalarVolumePerTime();
    return cerebralBloodFlow;
  }

  public boolean hasCerebralPerfusionPressure()
  {
    return cerebralPerfusionPressure == null ? false : cerebralPerfusionPressure.isValid();
  }
  public SEScalarPressure getCerebralPerfusionPressure()
  {
    if (cerebralPerfusionPressure == null)
      cerebralPerfusionPressure = new SEScalarPressure();
    return cerebralPerfusionPressure;
  }

  public boolean hasMeanCentralVenousPressure()
  {
    return meanCentralVenousPressure == null ? false : meanCentralVenousPressure.isValid();
  }
  public SEScalarPressure getMeanCentralVenousPressure()
  {
    if (meanCentralVenousPressure == null)
      meanCentralVenousPressure = new SEScalarPressure();
    return meanCentralVenousPressure;
  }

  public boolean hasDiastolicArterialPressure()
  {
    return diastolicArterialPressure == null ? false : diastolicArterialPressure.isValid();
  }
  public SEScalarPressure getDiastolicArterialPressure()
  {
    if (diastolicArterialPressure == null)
      diastolicArterialPressure = new SEScalarPressure();
    return diastolicArterialPressure;
  }
  
  public boolean hasDiastolicLeftHeartPressure()
  {
    return diastolicLeftHeartPressure == null ? false : diastolicLeftHeartPressure.isValid();
  }
  public SEScalarPressure getDiastolicLeftHeartPressure()
  {
    if (diastolicLeftHeartPressure == null)
      diastolicLeftHeartPressure = new SEScalarPressure();
    return diastolicLeftHeartPressure;
  }

  
  public boolean hasDiastolicRightHeartPressure()
  {
    return diastolicRightHeartPressure == null ? false : diastolicRightHeartPressure.isValid();
  }
  public SEScalarPressure getDiastolicRightHeartPressure()
  {
    if (diastolicRightHeartPressure == null)
      diastolicRightHeartPressure = new SEScalarPressure();
    return diastolicRightHeartPressure;
  }


  public boolean hasHeartEjectionFraction()
  {
    return heartEjectionFraction == null ? false : heartEjectionFraction.isValid();
  }
  public SEScalar0To1 getHeartEjectionFraction()
  {
    if (heartEjectionFraction == null)
      heartEjectionFraction = new SEScalar0To1();
    return heartEjectionFraction;
  }

  public boolean hasHeartRate()
  {
    return heartRate == null ? false : heartRate.isValid();
  }
  public SEScalarFrequency getHeartRate()
  {
    if (heartRate == null)
      heartRate = new SEScalarFrequency();
    return heartRate;
  }

  public eHeartRhythm    getHeartRhythm() { return this.heartRhythm;}
  public void            setHeartRhythm(eHeartRhythm heartRhythm){this.heartRhythm=heartRhythm;}
  public boolean         hasHeartRhythm(){return this.heartRhythm==null?false:true;}

  public boolean hasHeartStrokeVolume()
  {
    return heartStrokeVolume == null ? false : heartStrokeVolume.isValid();
  }
  public SEScalarVolume getHeartStrokeVolume()
  {
    if (heartStrokeVolume == null)
      heartStrokeVolume = new SEScalarVolume();
    return heartStrokeVolume;
  }

  public boolean hasIntracranialPressure()
  {
    return intracranialPressure == null ? false : intracranialPressure.isValid();
  }
  public SEScalarPressure getIntracranialPressure()
  {
    if (intracranialPressure == null)
      intracranialPressure = new SEScalarPressure();
    return intracranialPressure;
  }
  

  public boolean hasMeanArterialCarbonDioxidePartialPressure()
  {
    return meanArterialCarbonDioxidePartialPressure == null ? false : meanArterialCarbonDioxidePartialPressure.isValid();
  }
  public SEScalarPressure getMeanArterialCarbonDioxidePartialPressure()
  {
    if (meanArterialCarbonDioxidePartialPressure == null)
      meanArterialCarbonDioxidePartialPressure = new SEScalarPressure();
    return meanArterialCarbonDioxidePartialPressure;
  }

  public boolean hasMeanArterialCarbonDioxidePartialPressureDelta()
  {
    return meanArterialCarbonDioxidePartialPressureDelta == null ? false : meanArterialCarbonDioxidePartialPressureDelta.isValid();
  }
  public SEScalarPressure getMeanArterialCarbonDioxidePartialPressureDelta()
  {
    if (meanArterialCarbonDioxidePartialPressureDelta == null)
      meanArterialCarbonDioxidePartialPressureDelta = new SEScalarPressure();
    return meanArterialCarbonDioxidePartialPressureDelta;
  }

  public boolean hasMeanSkinFlow()
  {
    return meanSkinFlow == null ? false : meanSkinFlow.isValid();
  }
  public SEScalarVolumePerTime getMeanSkinFlow()
  {
    if (meanSkinFlow == null)
      meanSkinFlow = new SEScalarVolumePerTime();
    return meanSkinFlow;
  }
  
  public boolean hasPeripheralPerfusionIndex()
  {
    return peripheralPerfusionIndex == null ? false : peripheralPerfusionIndex.isValid();
  }
  public SEScalar0To1 getPeripheralPerfusionIndex()
  {
    if (peripheralPerfusionIndex == null)
      peripheralPerfusionIndex = new SEScalar0To1();
    return peripheralPerfusionIndex;
  }

  public boolean hasPulmonaryArterialPressure()
  {
    return pulmonaryArterialPressure == null ? false : pulmonaryArterialPressure.isValid();
  }
  public SEScalarPressure getPulmonaryArterialPressure()
  {
    if (pulmonaryArterialPressure == null)
      pulmonaryArterialPressure = new SEScalarPressure();
    return pulmonaryArterialPressure;
  }

  public boolean hasPulmonaryCapillariesWedgePressure()
  {
    return pulmonaryCapillariesWedgePressure == null ? false : pulmonaryCapillariesWedgePressure.isValid();
  }
  public SEScalarPressure getPulmonaryCapillariesWedgePressure()
  {
    if (pulmonaryCapillariesWedgePressure == null)
      pulmonaryCapillariesWedgePressure = new SEScalarPressure();
    return pulmonaryCapillariesWedgePressure;
  }

  public boolean hasPulmonaryDiastolicArterialPressure()
  {
    return pulmonaryDiastolicArterialPressure == null ? false : pulmonaryDiastolicArterialPressure.isValid();
  }
  public SEScalarPressure getPulmonaryDiastolicArterialPressure()
  {
    if (pulmonaryDiastolicArterialPressure == null)
      pulmonaryDiastolicArterialPressure = new SEScalarPressure();
    return pulmonaryDiastolicArterialPressure;
  }

  public boolean hasPulmonaryMeanArterialPressure()
  {
    return pulmonaryMeanArterialPressure == null ? false : pulmonaryMeanArterialPressure.isValid();
  }
  public SEScalarPressure getPulmonaryMeanArterialPressure()
  {
    if (pulmonaryMeanArterialPressure == null)
      pulmonaryMeanArterialPressure = new SEScalarPressure();
    return pulmonaryMeanArterialPressure;
  }

  public boolean hasPulmonaryMeanCapillaryFlow()
  {
    return pulmonaryMeanCapillaryFlow == null ? false : pulmonaryMeanCapillaryFlow.isValid();
  }
  public SEScalarVolumePerTime getPulmonaryMeanCapillaryFlow()
  {
    if (pulmonaryMeanCapillaryFlow == null)
      pulmonaryMeanCapillaryFlow = new SEScalarVolumePerTime();
    return pulmonaryMeanCapillaryFlow;
  }

  public boolean hasPulmonaryMeanShuntFlow()
  {
    return pulmonaryMeanShuntFlow == null ? false : pulmonaryMeanShuntFlow.isValid();
  }
  public SEScalarVolumePerTime getPulmonaryMeanShuntFlow()
  {
    if (pulmonaryMeanShuntFlow == null)
      pulmonaryMeanShuntFlow = new SEScalarVolumePerTime();
    return pulmonaryMeanShuntFlow;
  }

  public boolean hasPulmonarySystolicArterialPressure()
  {
    return pulmonarySystolicArterialPressure == null ? false : pulmonarySystolicArterialPressure.isValid();
  }
  public SEScalarPressure getPulmonarySystolicArterialPressure()
  {
    if (pulmonarySystolicArterialPressure == null)
      pulmonarySystolicArterialPressure = new SEScalarPressure();
    return pulmonarySystolicArterialPressure;
  }
  
  public boolean hasPulmonaryVascularResistance()
  {
    return pulmonaryVascularResistance == null ? false : pulmonaryVascularResistance.isValid();
  }
  public SEScalarPressureTimePerVolume getPulmonaryVascularResistance()
  {
    if (pulmonaryVascularResistance == null)
      pulmonaryVascularResistance = new SEScalarPressureTimePerVolume();
    return pulmonaryVascularResistance;
  }
  
  public boolean hasPulmonaryVascularResistanceIndex()
  {
    return pulmonaryVascularResistanceIndex == null ? false : pulmonaryVascularResistanceIndex.isValid();
  }
  public SEScalarPressureTimePerVolumeArea getPulmonaryVascularResistanceIndex()
  {
    if (pulmonaryVascularResistanceIndex == null)
      pulmonaryVascularResistanceIndex = new SEScalarPressureTimePerVolumeArea();
    return pulmonaryVascularResistanceIndex;
  }

  public boolean hasPulsePressure()
  {
    return pulsePressure == null ? false : pulsePressure.isValid();
  }
  public SEScalarPressure getPulsePressure()
  {
    if (pulsePressure == null)
      pulsePressure = new SEScalarPressure();
    return pulsePressure;
  }

  public boolean hasSystemicVascularResistance()
  {
    return systemicVascularResistance == null ? false : systemicVascularResistance.isValid();
  }
  public SEScalarPressureTimePerVolume getSystemicVascularResistance()
  {
    if (systemicVascularResistance == null)
      systemicVascularResistance = new SEScalarPressureTimePerVolume();
    return systemicVascularResistance;
  }

  public boolean hasSystolicArterialPressure()
  {
    return systolicArterialPressure == null ? false : systolicArterialPressure.isValid();
  }
  public SEScalarPressure getSystolicArterialPressure()
  {
    if (systolicArterialPressure == null)
      systolicArterialPressure = new SEScalarPressure();
    return systolicArterialPressure;
  }
  
  public boolean hasSystolicLeftHeartPressure()
  {
    return systolicLeftHeartPressure == null ? false : systolicLeftHeartPressure.isValid();
  }
  public SEScalarPressure getSystolicLeftHeartPressure()
  {
    if (systolicLeftHeartPressure == null)
      systolicLeftHeartPressure = new SEScalarPressure();
    return systolicLeftHeartPressure;
  }
  
  public boolean hasSystolicRightHeartPressure()
  {
    return systolicRightHeartPressure == null ? false : systolicRightHeartPressure.isValid();
  }
  public SEScalarPressure getSystolicRightHeartPressure()
  {
    if (systolicRightHeartPressure == null)
      systolicRightHeartPressure = new SEScalarPressure();
    return systolicRightHeartPressure;
  }
  
  public boolean hasTotalHemorrhageRate()
  {
    return totalHemorrhageRate == null ? false : totalHemorrhageRate.isValid();
  }
  public SEScalarVolumePerTime getTotalHemorrhageRate()
  {
    if (totalHemorrhageRate == null)
      totalHemorrhageRate = new SEScalarVolumePerTime();
    return totalHemorrhageRate;
  }

  public boolean hasTotalHemorrhagedVolume()
  {
    return totalHemorrhagedVolume == null ? false : totalHemorrhagedVolume.isValid();
  }
  public SEScalarVolume getTotalHemorrhagedVolume()
  {
    if (totalHemorrhagedVolume == null)
      totalHemorrhagedVolume = new SEScalarVolume();
    return totalHemorrhagedVolume;
  }
}
