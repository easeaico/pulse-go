/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.system.equipment.mechanical_ventilator;

import com.kitware.pulse.cdm.bind.Enums.eBreathState;
import com.kitware.pulse.cdm.properties.*;
import com.kitware.pulse.cdm.system.equipment.SEEquipment;

public class SEMechanicalVentilator implements SEEquipment
{
  protected SEScalarPressure                  airwayPressure;
  protected eBreathState                      breathState;
  protected SEScalarVolumePerPressure         dynamicRespiratoryCompliance;
  protected SEScalar0To1                      endTidalCarbonDioxideFraction;
  protected SEScalarPressure                  endTidalCarbonDioxidePressure;
  protected SEScalar0To1                      endTidalOxygenFraction;
  protected SEScalarPressure                  endTidalOxygenPressure;
  protected SEScalarVolumePerTime             expiratoryFlow;
  protected SEScalarPressureTimePerVolume     expiratoryResistance;
  protected SEScalarVolume                    expiratoryTidalVolume;
  protected SEScalarPressure                  extrinsicPositiveEndExpiratoryPressure;
  protected SEScalar                          inspiratoryExpiratoryRatio;
  protected SEScalarVolumePerTime             inspiratoryFlow;
  protected SEScalarPressureTimePerVolume     inspiratoryResistance;
  protected SEScalarVolume                    inspiratoryTidalVolume;
  protected SEScalarPressure                  intrinsicPositiveEndExpiratoryPressure;
  protected SEScalar0To1                      leakFraction;
  protected SEScalarPressure                  meanAirwayPressure;
  protected SEScalarVolumePerTime             peakExpiratoryFlow;
  protected SEScalarVolumePerTime             peakInspiratoryFlow;
  protected SEScalarPressure                  peakInspiratoryPressure;
  protected SEScalarPressure                  plateauPressure;
  protected SEScalarFrequency                 respirationRate;
  protected SEScalarVolumePerPressure         staticRespiratoryCompliance;
  protected SEScalarVolume                    tidalVolume;
  protected SEScalarVolume                    totalLungVolume;
  protected SEScalarPressure                  totalPositiveEndExpiratoryPressure;
  protected SEScalarVolumePerTime             totalPulmonaryVentilation;
  
  protected SEMechanicalVentilatorSettings    settings;

  public SEMechanicalVentilator()
  {
    airwayPressure = null;
    breathState = null;
    dynamicRespiratoryCompliance = null;
    endTidalCarbonDioxideFraction = null;
    endTidalCarbonDioxidePressure = null;
    endTidalOxygenFraction = null;
    endTidalOxygenPressure = null;
    expiratoryFlow = null;
    expiratoryResistance = null;
    expiratoryTidalVolume = null;
    extrinsicPositiveEndExpiratoryPressure = null;
    inspiratoryExpiratoryRatio = null;
    inspiratoryFlow = null;
    inspiratoryResistance = null;
    inspiratoryTidalVolume = null;
    intrinsicPositiveEndExpiratoryPressure = null;
    leakFraction = null;
    meanAirwayPressure = null;
    peakExpiratoryFlow = null;
    peakInspiratoryFlow = null;
    peakInspiratoryPressure = null;
    plateauPressure = null;
    respirationRate = null;
    staticRespiratoryCompliance = null;
    tidalVolume = null;
    totalLungVolume = null;
    totalPositiveEndExpiratoryPressure = null;
    totalPulmonaryVentilation = null;
    
    settings = null;
  }

  @Override
  public void clear()
  {
    if (airwayPressure != null)
      airwayPressure.invalidate();
    breathState = null;
    if (dynamicRespiratoryCompliance != null)
      dynamicRespiratoryCompliance.invalidate();
    if (endTidalCarbonDioxideFraction != null)
      endTidalCarbonDioxideFraction.invalidate();
    if (endTidalCarbonDioxidePressure != null)
      endTidalCarbonDioxidePressure.invalidate();
    if (endTidalOxygenFraction != null)
      endTidalOxygenFraction.invalidate();
    if (endTidalOxygenPressure != null)
      endTidalOxygenPressure.invalidate();
    if (expiratoryFlow != null)
      expiratoryFlow.invalidate();
    if (expiratoryResistance != null)
      expiratoryResistance.invalidate();
    if (expiratoryTidalVolume != null)
      expiratoryTidalVolume.invalidate();
    if (extrinsicPositiveEndExpiratoryPressure != null)
      extrinsicPositiveEndExpiratoryPressure.invalidate();
    if (inspiratoryExpiratoryRatio != null)
      inspiratoryExpiratoryRatio.invalidate();
    if (inspiratoryFlow != null)
      inspiratoryFlow.invalidate();
    if (inspiratoryResistance != null)
      inspiratoryResistance.invalidate();
    if (inspiratoryTidalVolume != null)
      inspiratoryTidalVolume.invalidate();
    if (intrinsicPositiveEndExpiratoryPressure != null)
      intrinsicPositiveEndExpiratoryPressure.invalidate();
    if (leakFraction != null)
      leakFraction.invalidate();
    if (meanAirwayPressure != null)
      meanAirwayPressure.invalidate();
    if (peakExpiratoryFlow != null)
      peakExpiratoryFlow.invalidate();
    if (peakInspiratoryFlow != null)
      peakInspiratoryFlow.invalidate();
    if (peakInspiratoryPressure != null)
      peakInspiratoryPressure.invalidate();
    if (plateauPressure != null)
      plateauPressure.invalidate();
    if (respirationRate != null)
      respirationRate.invalidate();
    if (staticRespiratoryCompliance != null)
      staticRespiratoryCompliance.invalidate();
    if (tidalVolume != null)
      tidalVolume.invalidate();
    if (totalLungVolume != null)
      totalLungVolume.invalidate();
    if (totalPositiveEndExpiratoryPressure != null)
      totalPositiveEndExpiratoryPressure.invalidate();
    if (totalPulmonaryVentilation != null)
      totalPulmonaryVentilation.invalidate();

    if (settings != null)
      settings.clear();
  }
  
  public boolean hasAirwayPressure()
  {
    return airwayPressure == null ? false : airwayPressure.isValid();
  }
  public SEScalarPressure getAirwayPressure()
  {
    if (airwayPressure == null)
      airwayPressure = new SEScalarPressure();
    return airwayPressure;
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
  
  public boolean hasDynamicRespiratoryCompliance()
  {
    return dynamicRespiratoryCompliance == null ? false : dynamicRespiratoryCompliance.isValid();
  }
  public SEScalarVolumePerPressure getDynamicRespiratoryCompliance()
  {
    if (dynamicRespiratoryCompliance == null)
      dynamicRespiratoryCompliance = new SEScalarVolumePerPressure();
    return dynamicRespiratoryCompliance;
  }

  public boolean hasEndTidalCarbonDioxideFraction()
  {
    return endTidalCarbonDioxideFraction == null ? false : endTidalCarbonDioxideFraction.isValid();
  }
  public SEScalar0To1 getEndTidalCarbonDioxideFraction()
  {
    if (endTidalCarbonDioxideFraction == null)
      endTidalCarbonDioxideFraction = new SEScalar0To1();
    return endTidalCarbonDioxideFraction;
  }

  public boolean hasEndTidalCarbonDioxidePressure()
  {
    return endTidalCarbonDioxidePressure == null ? false : endTidalCarbonDioxidePressure.isValid();
  }
  public SEScalarPressure getEndTidalCarbonDioxidePressure()
  {
    if (endTidalCarbonDioxidePressure == null)
      endTidalCarbonDioxidePressure = new SEScalarPressure();
    return endTidalCarbonDioxidePressure;
  }
  
  public boolean hasEndTidalOxygenFraction()
  {
    return endTidalOxygenFraction == null ? false : endTidalOxygenFraction.isValid();
  }
  public SEScalar0To1 getEndTidalOxygenFraction()
  {
    if (endTidalOxygenFraction == null)
      endTidalOxygenFraction = new SEScalar0To1();
    return endTidalOxygenFraction;
  }

  public boolean hasEndTidalOxygenPressure()
  {
    return endTidalOxygenPressure == null ? false : endTidalOxygenPressure.isValid();
  }
  public SEScalarPressure getEndTidalOxygenPressure()
  {
    if (endTidalOxygenPressure == null)
      endTidalOxygenPressure = new SEScalarPressure();
    return endTidalOxygenPressure;
  }

  public boolean hasExpiratoryFlow()
  {
    return expiratoryFlow == null ? false : expiratoryFlow.isValid();
  }
  public SEScalarVolumePerTime getExpiratoryFlow()
  {
    if (expiratoryFlow == null)
      expiratoryFlow = new SEScalarVolumePerTime();
    return expiratoryFlow;
  }
  
  public boolean hasExpiratoryResistance()
  {
    return expiratoryResistance == null ? false : expiratoryResistance.isValid();
  }
  public SEScalarPressureTimePerVolume getExpiratoryResistance()
  {
    if (expiratoryResistance == null)
      expiratoryResistance = new SEScalarPressureTimePerVolume();
    return expiratoryResistance;
  }

  public boolean hasExpiratoryTidalVolume()
  {
    return expiratoryTidalVolume == null ? false : expiratoryTidalVolume.isValid();
  }
  public SEScalarVolume getExpiratoryTidalVolume()
  {
    if (expiratoryTidalVolume == null)
      expiratoryTidalVolume = new SEScalarVolume();
    return expiratoryTidalVolume;
  }

  public boolean hasExtrinsicPositiveEndExpiratoryPressure()
  {
    return extrinsicPositiveEndExpiratoryPressure == null ? false : extrinsicPositiveEndExpiratoryPressure.isValid();
  }
  public SEScalarPressure getExtrinsicPositiveEndExpiratoryPressure()
  {
    if (extrinsicPositiveEndExpiratoryPressure == null)
      extrinsicPositiveEndExpiratoryPressure = new SEScalarPressure();
    return extrinsicPositiveEndExpiratoryPressure;
  }

  public boolean hasInspiratoryExpiratoryRatio()
  {
    return inspiratoryExpiratoryRatio == null ? false : inspiratoryExpiratoryRatio.isValid();
  }
  public SEScalar getInspiratoryExpiratoryRatio()
  {
    if (inspiratoryExpiratoryRatio == null)
      inspiratoryExpiratoryRatio = new SEScalar();
    return inspiratoryExpiratoryRatio;
  }

  public boolean hasInspiratoryFlow()
  {
    return inspiratoryFlow == null ? false : inspiratoryFlow.isValid();
  }
  public SEScalarVolumePerTime getInspiratoryFlow()
  {
    if (inspiratoryFlow == null)
      inspiratoryFlow = new SEScalarVolumePerTime();
    return inspiratoryFlow;
  }
  
  public boolean hasInspiratoryResistance()
  {
    return inspiratoryResistance == null ? false : inspiratoryResistance.isValid();
  }
  public SEScalarPressureTimePerVolume getInspiratoryResistance()
  {
    if (inspiratoryResistance == null)
      inspiratoryResistance = new SEScalarPressureTimePerVolume();
    return inspiratoryResistance;
  }

  public boolean hasInspiratoryTidalVolume()
  {
    return inspiratoryTidalVolume == null ? false : inspiratoryTidalVolume.isValid();
  }
  public SEScalarVolume getInspiratoryTidalVolume()
  {
    if (inspiratoryTidalVolume == null)
      inspiratoryTidalVolume = new SEScalarVolume();
    return inspiratoryTidalVolume;
  }
  
  public boolean hasIntrinsicPositiveEndExpiratoryPressure()
  {
    return intrinsicPositiveEndExpiratoryPressure == null ? false : intrinsicPositiveEndExpiratoryPressure.isValid();
  }
  public SEScalarPressure getIntrinsicPositiveEndExpiratoryPressure()
  {
    if (intrinsicPositiveEndExpiratoryPressure == null)
      intrinsicPositiveEndExpiratoryPressure = new SEScalarPressure();
    return intrinsicPositiveEndExpiratoryPressure;
  }

  public boolean hasLeakFraction()
  {
    return leakFraction == null ? false : leakFraction.isValid();
  }
  public SEScalar0To1 getLeakFraction()
  {
    if (leakFraction == null)
      leakFraction = new SEScalar0To1();
    return leakFraction;
  }
  
  public boolean hasMeanAirwayPressure()
  {
    return meanAirwayPressure == null ? false : meanAirwayPressure.isValid();
  }
  public SEScalarPressure getMeanAirwayPressure()
  {
    if (meanAirwayPressure == null)
      meanAirwayPressure = new SEScalarPressure();
    return meanAirwayPressure;
  }
  
  public boolean hasPeakExpiratoryFlow()
  {
    return peakExpiratoryFlow == null ? false : peakExpiratoryFlow.isValid();
  }
  public SEScalarVolumePerTime getPeakExpiratoryFlow()
  {
    if (peakExpiratoryFlow == null)
      peakExpiratoryFlow = new SEScalarVolumePerTime();
    return peakExpiratoryFlow;
  }
  
  public boolean hasPeakInspiratoryFlow()
  {
    return peakInspiratoryFlow == null ? false : peakInspiratoryFlow.isValid();
  }
  public SEScalarVolumePerTime getPeakInspiratoryFlow()
  {
    if (peakInspiratoryFlow == null)
      peakInspiratoryFlow = new SEScalarVolumePerTime();
    return peakInspiratoryFlow;
  }
  
  public boolean hasPeakInspiratoryPressure()
  {
    return peakInspiratoryPressure == null ? false : peakInspiratoryPressure.isValid();
  }
  public SEScalarPressure getPeakInspiratoryPressure()
  {
    if (peakInspiratoryPressure == null)
      peakInspiratoryPressure = new SEScalarPressure();
    return peakInspiratoryPressure;
  }
  
  public boolean hasPlateauPressure()
  {
    return plateauPressure == null ? false : plateauPressure.isValid();
  }
  public SEScalarPressure getPlateauPressure()
  {
    if (plateauPressure == null)
      plateauPressure = new SEScalarPressure();
    return plateauPressure;
  }
  
  public boolean hasRespirationRate()
  {
    return respirationRate == null ? false : respirationRate.isValid();
  }
  public SEScalarFrequency getRespirationRate()
  {
    if (respirationRate == null)
      respirationRate = new SEScalarFrequency();
    return respirationRate;
  }
  
  public boolean hasStaticRespiratoryCompliance()
  {
    return staticRespiratoryCompliance == null ? false : staticRespiratoryCompliance.isValid();
  }
  public SEScalarVolumePerPressure getStaticRespiratoryCompliance()
  {
    if (staticRespiratoryCompliance == null)
      staticRespiratoryCompliance = new SEScalarVolumePerPressure();
    return staticRespiratoryCompliance;
  }

  public boolean hasTidalVolume()
  {
    return tidalVolume == null ? false : tidalVolume.isValid();
  }
  public SEScalarVolume getTidalVolume()
  {
    if (tidalVolume == null)
      tidalVolume = new SEScalarVolume();
    return tidalVolume;
  }

  public boolean hasTotalLungVolume()
  {
    return totalLungVolume == null ? false : totalLungVolume.isValid();
  }
  public SEScalarVolume getTotalLungVolume()
  {
    if (totalLungVolume == null)
      totalLungVolume = new SEScalarVolume();
    return totalLungVolume;
  }
  
    public boolean hasTotalPositiveEndExpiratoryPressure()
  {
    return totalPositiveEndExpiratoryPressure == null ? false : totalPositiveEndExpiratoryPressure.isValid();
  }
  public SEScalarPressure getTotalPositiveEndExpiratoryPressure()
  {
    if (totalPositiveEndExpiratoryPressure == null)
      totalPositiveEndExpiratoryPressure = new SEScalarPressure();
    return totalPositiveEndExpiratoryPressure;
  }
  
  public boolean hasTotalPulmonaryVentilation()
  {
    return totalPulmonaryVentilation == null ? false : totalPulmonaryVentilation.isValid();
  }
  public SEScalarVolumePerTime getTotalPulmonaryVentilation()
  {
    if (totalPulmonaryVentilation == null)
      totalPulmonaryVentilation = new SEScalarVolumePerTime();
    return totalPulmonaryVentilation;
  }
  

  public boolean hasSettings()
  {
    return settings != null;
  }
  public SEMechanicalVentilatorSettings getSettings()
  {
    if (settings == null)
      settings = new SEMechanicalVentilatorSettings();
    return settings;
  }
}