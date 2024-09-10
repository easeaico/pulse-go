/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.system.equipment.mechanical_ventilator;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;

import com.kitware.pulse.cdm.bind.Enums.eDriverWaveform;
import com.kitware.pulse.cdm.bind.Enums.eSwitch;
import com.kitware.pulse.cdm.bind.MechanicalVentilator.MechanicalVentilatorSettingsData;
import com.kitware.pulse.cdm.bind.Substance.SubstanceConcentrationData;
import com.kitware.pulse.cdm.bind.Substance.SubstanceFractionData;
import com.kitware.pulse.cdm.properties.*;
import com.kitware.pulse.cdm.substance.SESubstanceConcentration;
import com.kitware.pulse.cdm.substance.SESubstanceFraction;

public class SEMechanicalVentilatorSettings
{
  protected eSwitch                           connection;
  protected SEScalarVolume                    connectionVolume;
  protected SEScalarVolumePerPressure         compliance;
  protected SEScalarFrequency                 driverDampingParameter;
  
  // Expiratory Baseline Properties (Only set 1)
  protected SEScalarPressure                  positiveEndExpiredPressure;
  protected SEScalarPressure                  functionalResidualCapacity;
  
  // Expriatory Cycle Properties (Only Set 1)
  protected SEScalarVolumePerTime             expirationCycleFlow;
  protected SEScalarPressure                  expirationCyclePressure;
  protected SEScalarVolume                    expirationCycleVolume;
  protected SEScalarTime                      expirationCycleTime;
  protected eSwitch                           expirationCycleRespiratoryModel;

  protected SEScalarVolume                    expirationLimbVolume;
  protected SEScalarPressureTimePerVolume     expirationTubeResistance;
  protected SEScalarPressureTimePerVolume     expirationValveResistance;
  protected SEScalarVolume                    expirationValveVolume;
  protected eDriverWaveform                   expirationWaveform;
  protected SEScalarTime                      expirationWaveformPeriod;

  // Inspiratory Limit Properties (Only set 1)
  protected SEScalarVolumePerTime             inspirationLimitFlow;
  protected SEScalarPressure                  inspirationLimitPressure;
  protected SEScalarVolume                    inspirationLimitVolume;
  

  protected SEScalarTime                      inspirationPauseTime;
  
  // Inspiratory Target Properties (Only set 1)
  protected SEScalarPressure                  peakInspiratoryPressure;
  protected SEScalarVolumePerTime             inspirationTargetFlow;
  
  // Inspiratory Machine Trigger Properties (Only set 1)
  protected SEScalarTime                      inspirationMachineTriggerTime;

  // Inspiratory Patient Trigger Properties (Only set 1)
  protected SEScalarVolumePerTime             inspirationPatientTriggerFlow;
  protected SEScalarPressure                  inspirationPatientTriggerPressure;
  protected eSwitch                           inspirationPatientTriggerRespiratoryModel;
  

  protected SEScalarVolume                    inspirationLimbVolume;
  protected SEScalarPressureTimePerVolume     inspirationTubeResistance;
  protected SEScalarPressureTimePerVolume     inspirationValveResistance;
  protected SEScalarVolume                    inspirationValveVolume;
  protected eDriverWaveform                   inspirationWaveform;
  protected SEScalarTime                      inspirationWaveformPeriod;

  protected SEScalarPressure                  reliefValveThreshold;
  protected SEScalarVolume                    yPieceVolume;
  
  protected List<SESubstanceFraction>         fractionInspiredGases;
  protected List<SESubstanceConcentration>    concentrationInspiredAerosol;


  public SEMechanicalVentilatorSettings()
  {
    connection = null;
    connectionVolume = null;
    compliance = null;
    driverDampingParameter = null;

    positiveEndExpiredPressure = null;
    functionalResidualCapacity = null;

    expirationCycleFlow = null;
    expirationCyclePressure = null;
    expirationCycleVolume = null;
    expirationCycleTime = null;
    expirationCycleRespiratoryModel = null;

    expirationLimbVolume = null;
    expirationTubeResistance = null;
    expirationValveResistance = null;
    expirationValveVolume = null;
    expirationWaveform = null;
    expirationWaveformPeriod = null;

    inspirationLimitFlow = null;
    inspirationLimitPressure = null;
    inspirationLimitVolume = null;

    inspirationPauseTime = null;

    peakInspiratoryPressure = null;
    inspirationTargetFlow = null;

    inspirationMachineTriggerTime = null;

    inspirationPatientTriggerFlow = null;
    inspirationPatientTriggerPressure = null;
    inspirationPatientTriggerRespiratoryModel = null;

    inspirationLimbVolume = null;
    inspirationTubeResistance = null;
    inspirationValveResistance = null;
    inspirationValveVolume = null;
    inspirationWaveform = null;
    inspirationWaveformPeriod = null;

    reliefValveThreshold = null;
    yPieceVolume = null;

    this.fractionInspiredGases=new ArrayList<SESubstanceFraction>();
    this.concentrationInspiredAerosol=new ArrayList<SESubstanceConcentration>();
  }

  public void clear()
  {
    connection = null;
    if (connectionVolume != null)
      connectionVolume.invalidate();
    if (compliance != null)
      compliance.invalidate();
    if (driverDampingParameter != null)
      driverDampingParameter.invalidate();

    if (positiveEndExpiredPressure != null)
      positiveEndExpiredPressure.invalidate();
    if (functionalResidualCapacity != null)
      functionalResidualCapacity.invalidate();

    if (expirationCycleFlow != null)
      expirationCycleFlow.invalidate();
    if (expirationCyclePressure != null)
      expirationCyclePressure.invalidate();
    if (expirationCycleVolume != null)
      expirationCycleVolume.invalidate();
    if (expirationCycleTime != null)
      expirationCycleTime.invalidate();
    expirationCycleRespiratoryModel = null;

    if (expirationLimbVolume != null)
      expirationLimbVolume.invalidate();
    if (expirationTubeResistance != null)
      expirationTubeResistance.invalidate();
    if (expirationValveResistance != null)
      expirationValveResistance.invalidate();
    if (expirationValveVolume != null)
      expirationValveVolume.invalidate();
    expirationWaveform = null;
    if (expirationWaveformPeriod != null)
      expirationWaveformPeriod.invalidate();

    if (inspirationLimitFlow != null)
      inspirationLimitFlow.invalidate();
    if (inspirationLimitPressure != null)
      inspirationLimitPressure.invalidate();
    if (inspirationLimitVolume != null)
      inspirationLimitVolume.invalidate();

    if (inspirationPauseTime != null)
      inspirationPauseTime.invalidate();

    if (peakInspiratoryPressure != null)
      peakInspiratoryPressure.invalidate();
    if (inspirationTargetFlow != null)
      inspirationTargetFlow.invalidate();

    if (inspirationMachineTriggerTime != null)
      inspirationMachineTriggerTime.invalidate();

    if (inspirationPatientTriggerFlow != null)
      inspirationPatientTriggerFlow.invalidate();
    if (inspirationPatientTriggerPressure != null)
      inspirationPatientTriggerPressure.invalidate();
    inspirationPatientTriggerRespiratoryModel = null;

    if (inspirationLimbVolume != null)
      inspirationLimbVolume.invalidate();
    if (inspirationTubeResistance != null)
      inspirationTubeResistance.invalidate();
    if (inspirationValveResistance != null)
      inspirationValveResistance.invalidate();
    if (inspirationValveVolume != null)
      inspirationValveVolume.invalidate();
    inspirationWaveform = null;
    if (inspirationWaveformPeriod != null)
      inspirationWaveformPeriod.invalidate();
    

    if (reliefValveThreshold != null)
      reliefValveThreshold.invalidate();
    if (yPieceVolume != null)
      yPieceVolume.invalidate();
    
    this.fractionInspiredGases.clear();
    this.concentrationInspiredAerosol.clear();
  }

  public void copy(SEMechanicalVentilatorSettings from)
  {
    clear();
    if(from.connection!=null && from.connection != eSwitch.NullSwitch)
    	this.connection=from.connection;
    if(from.hasConnectionVolume())
      this.getConnectionVolume().set(from.getConnectionVolume());
    if(from.hasCompliance())
      this.getCompliance().set(from.getCompliance());
    if(from.hasDriverDampingParameter())
      this.getDriverDampingParameter().set(from.getDriverDampingParameter());
    
    if(from.hasPositiveEndExpiratoryPressure())
      this.getPositiveEndExpiratoryPressure().set(from.getPositiveEndExpiratoryPressure());
    if(from.hasFunctionalResidualCapacity())
      this.getFunctionalResidualCapacity().set(from.getFunctionalResidualCapacity());
    
    if(from.hasExpirationCycleFlow())
      this.getExpirationCycleFlow().set(from.getExpirationCycleFlow());
    if(from.hasExpirationCyclePressure())
      this.getExpirationCyclePressure().set(from.getExpirationCyclePressure());
    if(from.hasExpirationCycleVolume())
      this.getExpirationCycleVolume().set(from.getExpirationCycleVolume());
    if(from.hasExpirationCycleTime())
      this.getExpirationCycleTime().set(from.getExpirationCycleTime());
    if(from.expirationCycleRespiratoryModel!=null && from.expirationCycleRespiratoryModel != eSwitch.NullSwitch)
      this.expirationCycleRespiratoryModel=from.expirationCycleRespiratoryModel;
    
    if(from.hasExpirationLimbVolume())
      this.getExpirationLimbVolume().set(from.getExpirationLimbVolume());
    if(from.hasExpirationTubeResistance())
      this.getExpirationTubeResistance().set(from.getExpirationTubeResistance());
    if(from.hasExpirationValveResistance())
      this.getExpirationValveResistance().set(from.getExpirationValveResistance());
    if(from.hasExpirationValveVolume())
      this.getExpirationValveVolume().set(from.getExpirationValveVolume());
    if(from.expirationWaveform!=null && from.expirationWaveform != eDriverWaveform.NullDriverWaveform)
      this.expirationWaveform=from.expirationWaveform;
    if(from.hasExpirationWaveformPeriod())
      this.getExpirationWaveformPeriod().set(from.getExpirationWaveformPeriod());
    
    if(from.hasInspirationLimitFlow())
      this.getInspirationLimitFlow().set(from.getInspirationLimitFlow());
    if(from.hasInspirationLimitPressure())
      this.getInspirationLimitPressure().set(from.getInspirationLimitPressure());
    if(from.hasInspirationLimitVolume())
      this.getInspirationLimitVolume().set(from.getInspirationLimitVolume());
    
    if(from.hasInspirationPauseTime())
      this.getInspirationPauseTime().set(from.getInspirationPauseTime());
    
    if(from.hasPeakInspiratoryPressure())
      this.getPeakInspiratoryPressure().set(from.getPeakInspiratoryPressure());
    if(from.hasInspirationTargetFlow())
      this.getInspirationTargetFlow().set(from.getInspirationTargetFlow());
    
    if(from.hasInspirationMachineTriggerTime())
      this.getInspirationMachineTriggerTime().set(from.getInspirationMachineTriggerTime());

    if(from.hasInspirationPatientTriggerFlow())
      this.getInspirationPatientTriggerFlow().set(from.getInspirationPatientTriggerFlow());
    if(from.hasInspirationPatientTriggerPressure())
      this.getInspirationPatientTriggerPressure().set(from.getInspirationPatientTriggerPressure());
    if(from.inspirationPatientTriggerRespiratoryModel!=null && from.inspirationPatientTriggerRespiratoryModel != eSwitch.NullSwitch)
      this.inspirationPatientTriggerRespiratoryModel=from.inspirationPatientTriggerRespiratoryModel;

    if(from.hasInspirationLimbVolume())
      this.getInspirationLimbVolume().set(from.getInspirationLimbVolume());
    if(from.hasInspirationTubeResistance())
      this.getInspirationTubeResistance().set(from.getInspirationTubeResistance());
    if(from.hasInspirationValveResistance())
      this.getInspirationValveResistance().set(from.getInspirationValveResistance());
    if(from.hasInspirationValveVolume())
      this.getInspirationValveVolume().set(from.getInspirationValveVolume());
    if(from.inspirationWaveform!=null && from.inspirationWaveform != eDriverWaveform.NullDriverWaveform)
      this.inspirationWaveform=from.inspirationWaveform;
    if(from.hasInspirationWaveformPeriod())
      this.getInspirationWaveformPeriod().set(from.getInspirationWaveformPeriod());

    if(from.hasReliefValveThreshold())
      this.getReliefValveThreshold().set(from.getReliefValveThreshold());
    if(from.hasYPieceVolume())
      this.getYPieceVolume().set(from.getYPieceVolume());
    
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

  public static void load(MechanicalVentilatorSettingsData src, SEMechanicalVentilatorSettings dst)
  {
    dst.clear();
    if (src.getConnection()!=eSwitch.UNRECOGNIZED)
      dst.setConnection(src.getConnection());
    if (src.hasConnectionVolume())
      SEScalarVolume.load(src.getConnectionVolume(), dst.getConnectionVolume());
    if (src.hasCompliance())
      SEScalarVolumePerPressure.load(src.getCompliance(), dst.getCompliance());
    if (src.hasDriverDampingParameter())
      SEScalarFrequency.load(src.getDriverDampingParameter(), dst.getDriverDampingParameter());
    
    if (src.hasPositiveEndExpiratoryPressure())
      SEScalarPressure.load(src.getPositiveEndExpiratoryPressure(), dst.getPositiveEndExpiratoryPressure());
    else if (src.hasFunctionalResidualCapacity())
      SEScalarPressure.load(src.getFunctionalResidualCapacity(), dst.getFunctionalResidualCapacity());
    
    if (src.hasExpirationCycleFlow())
      SEScalarVolumePerTime.load(src.getExpirationCycleFlow(), dst.getExpirationCycleFlow());
    else if (src.hasExpirationCyclePressure())
      SEScalarPressure.load(src.getExpirationCyclePressure(), dst.getExpirationCyclePressure());
    else if (src.hasExpirationCycleVolume())
      SEScalarVolume.load(src.getExpirationCycleVolume(), dst.getExpirationCycleVolume());
    else if (src.hasExpirationCycleTime())
      SEScalarTime.load(src.getExpirationCycleTime(), dst.getExpirationCycleTime());
    else if (src.getExpirationCycleRespiratoryModel()!=eSwitch.UNRECOGNIZED)
      dst.setExpirationCycleRespiratoryModel(src.getExpirationCycleRespiratoryModel());
    
    if (src.hasExpirationLimbVolume())
      SEScalarVolume.load(src.getExpirationLimbVolume(), dst.getExpirationLimbVolume());
    if (src.hasExpirationTubeResistance())
      SEScalarPressureTimePerVolume.load(src.getExpirationTubeResistance(), dst.getExpirationTubeResistance());
    if (src.hasExpirationValveResistance())
      SEScalarPressureTimePerVolume.load(src.getExpirationValveResistance(), dst.getExpirationValveResistance());
    if (src.hasExpirationValveVolume())
      SEScalarVolume.load(src.getExpirationValveVolume(), dst.getExpirationValveVolume());
    if (src.getExpirationWaveform()!=eDriverWaveform.UNRECOGNIZED)
      dst.setExpirationWaveform(src.getExpirationWaveform());
    if (src.hasExpirationWaveformPeriod())
      SEScalarTime.load(src.getExpirationWaveformPeriod(), dst.getExpirationWaveformPeriod());
    
    if (src.hasInspirationLimitFlow())
      SEScalarVolumePerTime.load(src.getInspirationLimitFlow(), dst.getInspirationLimitFlow());
    else if (src.hasInspirationLimitPressure())
      SEScalarPressure.load(src.getInspirationLimitPressure(), dst.getInspirationLimitPressure());
    else if (src.hasInspirationLimitVolume())
      SEScalarVolume.load(src.getInspirationLimitVolume(), dst.getInspirationLimitVolume());
    
    if (src.hasInspirationPauseTime())
      SEScalarTime.load(src.getInspirationPauseTime(), dst.getInspirationPauseTime());
    
    if (src.hasPeakInspiratoryPressure())
      SEScalarPressure.load(src.getPeakInspiratoryPressure(), dst.getPeakInspiratoryPressure());
    else if (src.hasInspirationTargetFlow())
      SEScalarVolumePerTime.load(src.getInspirationTargetFlow(), dst.getInspirationTargetFlow());
    
    if (src.hasInspirationMachineTriggerTime())
      SEScalarTime.load(src.getInspirationMachineTriggerTime(), dst.getInspirationMachineTriggerTime());
    
    if (src.hasInspirationPatientTriggerFlow())
      SEScalarVolumePerTime.load(src.getInspirationPatientTriggerFlow(), dst.getInspirationPatientTriggerFlow());
    else if (src.hasInspirationPatientTriggerPressure())
      SEScalarPressure.load(src.getInspirationPatientTriggerPressure(), dst.getInspirationPatientTriggerPressure());
    else if (src.getInspirationPatientTriggerRespiratoryModel()!=eSwitch.UNRECOGNIZED)
      dst.setInspirationPatientTriggerRespiratoryModel(src.getInspirationPatientTriggerRespiratoryModel());

    if (src.hasInspirationLimbVolume())
      SEScalarVolume.load(src.getInspirationLimbVolume(), dst.getInspirationLimbVolume());
    if (src.hasInspirationTubeResistance())
      SEScalarPressureTimePerVolume.load(src.getInspirationTubeResistance(), dst.getInspirationTubeResistance());
    if (src.hasInspirationValveResistance())
      SEScalarPressureTimePerVolume.load(src.getInspirationValveResistance(), dst.getInspirationValveResistance());
    if (src.hasInspirationValveVolume())
      SEScalarVolume.load(src.getInspirationValveVolume(), dst.getInspirationValveVolume());
    if (src.getInspirationWaveform()!=eDriverWaveform.UNRECOGNIZED)
      dst.setInspirationWaveform(src.getInspirationWaveform());
    if (src.hasInspirationWaveformPeriod())
      SEScalarTime.load(src.getInspirationWaveformPeriod(), dst.getInspirationWaveformPeriod());

    if (src.hasReliefValveThreshold())
      SEScalarPressure.load(src.getReliefValveThreshold(), dst.getReliefValveThreshold());
    if (src.hasYPieceVolume())
      SEScalarVolume.load(src.getYPieceVolume(), dst.getYPieceVolume());
    
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
  public static MechanicalVentilatorSettingsData unload(SEMechanicalVentilatorSettings src)
  {
    MechanicalVentilatorSettingsData.Builder dst = MechanicalVentilatorSettingsData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(SEMechanicalVentilatorSettings src, MechanicalVentilatorSettingsData.Builder dst)
  {
    if (src.hasConnection())
      dst.setConnection(src.connection);
    if(src.hasConnectionVolume())
      dst.setConnectionVolume(SEScalarVolume.unload(src.getConnectionVolume()));
    if(src.hasCompliance())
      dst.setCompliance(SEScalarVolumePerPressure.unload(src.getCompliance()));
    if(src.hasDriverDampingParameter())
      dst.setDriverDampingParameter(SEScalarFrequency.unload(src.getDriverDampingParameter()));
    
    if(src.hasPositiveEndExpiratoryPressure())
      dst.setPositiveEndExpiratoryPressure(SEScalarPressure.unload(src.getPositiveEndExpiratoryPressure()));
    else if (src.hasFunctionalResidualCapacity())
      dst.setFunctionalResidualCapacity(SEScalarPressure.unload(src.getFunctionalResidualCapacity()));
    
    if(src.hasExpirationCycleFlow())
      dst.setExpirationCycleFlow(SEScalarVolumePerTime.unload(src.getExpirationCycleFlow()));
    else if (src.hasExpirationCyclePressure())
      dst.setExpirationCyclePressure(SEScalarPressure.unload(src.getExpirationCyclePressure()));
    else if (src.hasExpirationCycleVolume())
      dst.setExpirationCycleVolume(SEScalarVolume.unload(src.getExpirationCycleVolume()));
    else if (src.hasExpirationCycleTime())
      dst.setExpirationCycleTime(SEScalarTime.unload(src.getExpirationCycleTime()));
    else if (src.hasExpirationCycleRespiratoryModel())
      dst.setExpirationCycleRespiratoryModel(src.expirationCycleRespiratoryModel);
    
    if(src.hasExpirationLimbVolume())
      dst.setExpirationLimbVolume(SEScalarVolume.unload(src.getExpirationLimbVolume()));
    if(src.hasExpirationTubeResistance())
      dst.setExpirationTubeResistance(SEScalarPressureTimePerVolume.unload(src.getExpirationTubeResistance()));
    if(src.hasExpirationValveResistance())
      dst.setExpirationValveResistance(SEScalarPressureTimePerVolume.unload(src.getExpirationValveResistance()));
    if(src.hasExpirationValveVolume())
      dst.setExpirationValveVolume(SEScalarVolume.unload(src.getExpirationValveVolume()));
    if (src.hasExpirationWaveform())
      dst.setExpirationWaveform(src.expirationWaveform);
    if(src.hasExpirationWaveformPeriod())
      dst.setExpirationWaveformPeriod(SEScalarTime.unload(src.getExpirationWaveformPeriod()));
    
    if(src.hasInspirationLimitFlow())
      dst.setInspirationLimitFlow(SEScalarVolumePerTime.unload(src.getInspirationLimitFlow()));
    else if (src.hasInspirationLimitPressure())
      dst.setInspirationLimitPressure(SEScalarPressure.unload(src.getInspirationLimitPressure()));
    else if (src.hasInspirationLimitVolume())
      dst.setInspirationLimitVolume(SEScalarVolume.unload(src.getInspirationLimitVolume()));
    
    if(src.hasInspirationPauseTime())
      dst.setInspirationPauseTime(SEScalarTime.unload(src.getInspirationPauseTime()));
    
    if(src.hasPeakInspiratoryPressure())
      dst.setPeakInspiratoryPressure(SEScalarPressure.unload(src.getPeakInspiratoryPressure()));
    else if (src.hasInspirationTargetFlow())
      dst.setInspirationTargetFlow(SEScalarVolumePerTime.unload(src.getInspirationTargetFlow()));
    else if (src.hasInspirationPatientTriggerRespiratoryModel())
      dst.setInspirationPatientTriggerRespiratoryModel(src.inspirationPatientTriggerRespiratoryModel);
    
    if (src.hasInspirationMachineTriggerTime())
      dst.setInspirationMachineTriggerTime(SEScalarTime.unload(src.getInspirationMachineTriggerTime()));
    
    if(src.hasInspirationPatientTriggerFlow())
      dst.setInspirationPatientTriggerFlow(SEScalarVolumePerTime.unload(src.getInspirationPatientTriggerFlow()));
    else if (src.hasInspirationPatientTriggerPressure())
      dst.setInspirationPatientTriggerPressure(SEScalarPressure.unload(src.getInspirationPatientTriggerPressure()));
    
    if(src.hasInspirationLimbVolume())
      dst.setInspirationLimbVolume(SEScalarVolume.unload(src.getInspirationLimbVolume()));
    if(src.hasInspirationTubeResistance())
      dst.setInspirationTubeResistance(SEScalarPressureTimePerVolume.unload(src.getInspirationTubeResistance()));
    if(src.hasInspirationValveResistance())
      dst.setInspirationValveResistance(SEScalarPressureTimePerVolume.unload(src.getInspirationValveResistance()));
    if(src.hasInspirationValveVolume())
      dst.setInspirationValveVolume(SEScalarVolume.unload(src.getInspirationValveVolume()));
    if (src.hasInspirationWaveform())
      dst.setInspirationWaveform(src.inspirationWaveform);
    if(src.hasInspirationWaveformPeriod())
      dst.setInspirationWaveformPeriod(SEScalarTime.unload(src.getInspirationWaveformPeriod()));

    if(src.hasReliefValveThreshold())
      dst.setReliefValveThreshold(SEScalarPressure.unload(src.getReliefValveThreshold()));
    if(src.hasYPieceVolume())
      dst.setYPieceVolume(SEScalarVolume.unload(src.getYPieceVolume()));
    
    for(SESubstanceFraction ambSub : src.fractionInspiredGases)
      dst.addFractionInspiredGas(SESubstanceFraction.unload(ambSub));
    
    for(SESubstanceConcentration ambSub : src.concentrationInspiredAerosol)
      dst.addConcentrationInspiredAerosol(SESubstanceConcentration.unload(ambSub));
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
    return connection != null && connection != eSwitch.NullSwitch;
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
  
  public SEScalarVolumePerPressure getCompliance()
  {
    if (compliance == null)
      compliance = new SEScalarVolumePerPressure();
    return compliance;
  }
  public boolean hasCompliance()
  {
    return compliance == null ? false : compliance.isValid();
  }
  
  public SEScalarFrequency getDriverDampingParameter()
  {
    if (driverDampingParameter == null)
      driverDampingParameter = new SEScalarFrequency();
    return driverDampingParameter;
  }
  public boolean hasDriverDampingParameter()
  {
    return driverDampingParameter == null ? false : driverDampingParameter.isValid();
  }
  
  public SEScalarPressure getPositiveEndExpiratoryPressure()
  {
    if (positiveEndExpiredPressure == null)
      positiveEndExpiredPressure = new SEScalarPressure();
    return positiveEndExpiredPressure;
  }
  public boolean hasPositiveEndExpiratoryPressure()
  {
    return positiveEndExpiredPressure == null ? false : positiveEndExpiredPressure.isValid();
  }
  
  public SEScalarPressure getFunctionalResidualCapacity()
  {
    if (functionalResidualCapacity == null)
      functionalResidualCapacity = new SEScalarPressure();
    return functionalResidualCapacity;
  }public boolean hasFunctionalResidualCapacity()
  {
    return functionalResidualCapacity == null ? false : functionalResidualCapacity.isValid();
  }
  
  public SEScalarVolumePerTime getExpirationCycleFlow()
  {
    if (expirationCycleFlow == null)
      expirationCycleFlow = new SEScalarVolumePerTime();
    return expirationCycleFlow;
  }
  public boolean hasExpirationCycleFlow()
  {
    return expirationCycleFlow == null ? false : expirationCycleFlow.isValid();
  }
  
  public SEScalarPressure getExpirationCyclePressure()
  {
    if (expirationCyclePressure == null)
      expirationCyclePressure = new SEScalarPressure();
    return expirationCyclePressure;
  }
  public boolean hasExpirationCyclePressure()
  {
    return expirationCyclePressure == null ? false : expirationCyclePressure.isValid();
  }

  public SEScalarVolume getExpirationCycleVolume()
  {
    if (expirationCycleVolume == null)
      expirationCycleVolume = new SEScalarVolume();
    return expirationCycleVolume;
  }
  public boolean hasExpirationCycleVolume()
  {
    return expirationCycleVolume == null ? false : expirationCycleVolume.isValid();
  }
  
  public SEScalarTime getExpirationCycleTime()
  {
    if (expirationCycleTime == null)
      expirationCycleTime = new SEScalarTime();
    return expirationCycleTime;
  }
  public boolean hasExpirationCycleTime()
  {
    return expirationCycleTime == null ? false : expirationCycleTime.isValid();
  }
  
  public eSwitch getExpirationCycleRespiratoryModel()
  {
    return expirationCycleRespiratoryModel;
  }
  public void setExpirationCycleRespiratoryModel(eSwitch s)
  {
    expirationCycleRespiratoryModel = (s == eSwitch.UNRECOGNIZED) ? null : s;
  }
  public boolean hasExpirationCycleRespiratoryModel()
  {
    return expirationCycleRespiratoryModel != null && expirationCycleRespiratoryModel != eSwitch.NullSwitch;
  }
  
  public SEScalarVolume getExpirationLimbVolume()
  {
    if (expirationLimbVolume == null)
      expirationLimbVolume = new SEScalarVolume();
    return expirationLimbVolume;
  }
  public boolean hasExpirationLimbVolume()
  {
    return expirationLimbVolume == null ? false : expirationLimbVolume.isValid();
  }
  
  public SEScalarPressureTimePerVolume getExpirationTubeResistance()
  {
    if (expirationTubeResistance == null)
      expirationTubeResistance = new SEScalarPressureTimePerVolume();
    return expirationTubeResistance;
  }
  public boolean hasExpirationTubeResistance()
  {
    return expirationTubeResistance == null ? false : expirationTubeResistance.isValid();
  }
  
  public SEScalarPressureTimePerVolume getExpirationValveResistance()
  {
    if (expirationValveResistance == null)
      expirationValveResistance = new SEScalarPressureTimePerVolume();
    return expirationValveResistance;
  }
  public boolean hasExpirationValveResistance()
  {
    return expirationValveResistance == null ? false : expirationValveResistance.isValid();
  }
  
  public SEScalarVolume getExpirationValveVolume()
  {
    if (expirationValveVolume == null)
      expirationValveVolume = new SEScalarVolume();
    return expirationValveVolume;
  }
  public boolean hasExpirationValveVolume()
  {
    return expirationValveVolume == null ? false : expirationValveVolume.isValid();
  }
  
  public eDriverWaveform getExpirationWaveform()
  {
    return expirationWaveform;
  }
  public void setExpirationWaveform(eDriverWaveform w)
  {
    expirationWaveform = (w == eDriverWaveform.UNRECOGNIZED) ? null : w;
  }
  public boolean hasExpirationWaveform()
  {
    return expirationWaveform != null;
  }
  
  public SEScalarTime getExpirationWaveformPeriod()
  {
    if (expirationWaveformPeriod == null)
      expirationWaveformPeriod = new SEScalarTime();
    return expirationWaveformPeriod;
  }
  public boolean hasExpirationWaveformPeriod()
  {
    return expirationWaveformPeriod == null ? false : expirationWaveformPeriod.isValid();
  }
  
  public SEScalarVolumePerTime getInspirationLimitFlow()
  {
    if (inspirationLimitFlow == null)
      inspirationLimitFlow = new SEScalarVolumePerTime();
    return inspirationLimitFlow;
  }
  public boolean hasInspirationLimitFlow()
  {
    return inspirationLimitFlow == null ? false : inspirationLimitFlow.isValid();
  }
  
  public SEScalarPressure getInspirationLimitPressure()
  {
    if (inspirationLimitPressure == null)
      inspirationLimitPressure = new SEScalarPressure();
    return inspirationLimitPressure;
  }
  public boolean hasInspirationLimitPressure()
  {
    return inspirationLimitPressure == null ? false : inspirationLimitPressure.isValid();
  }
  
  public SEScalarVolume getInspirationLimitVolume()
  {
    if (inspirationLimitVolume == null)
      inspirationLimitVolume = new SEScalarVolume();
    return inspirationLimitVolume;
  }
  public boolean hasInspirationLimitVolume()
  {
    return inspirationLimitVolume == null ? false : inspirationLimitVolume.isValid();
  }
  
  public SEScalarTime getInspirationPauseTime()
  {
    if (inspirationPauseTime == null)
      inspirationPauseTime = new SEScalarTime();
    return inspirationPauseTime;
  }
  public boolean hasInspirationPauseTime()
  {
    return inspirationPauseTime == null ? false : inspirationPauseTime.isValid();
  }

  public SEScalarPressure getPeakInspiratoryPressure()
  {
    if (peakInspiratoryPressure == null)
      peakInspiratoryPressure = new SEScalarPressure();
    return peakInspiratoryPressure;
  }
  public boolean hasPeakInspiratoryPressure()
  {
    return peakInspiratoryPressure == null ? false : peakInspiratoryPressure.isValid();
  }
  
  public SEScalarVolumePerTime getInspirationTargetFlow()
  {
    if (inspirationTargetFlow == null)
      inspirationTargetFlow = new SEScalarVolumePerTime();
    return inspirationTargetFlow;
  }
  public boolean hasInspirationTargetFlow()
  {
    return inspirationTargetFlow == null ? false : inspirationTargetFlow.isValid();
  }
  
  public SEScalarTime getInspirationMachineTriggerTime()
  {
    if (inspirationMachineTriggerTime == null)
      inspirationMachineTriggerTime = new SEScalarTime();
    return inspirationMachineTriggerTime;
  }
  public boolean hasInspirationMachineTriggerTime()
  {
    return inspirationMachineTriggerTime == null ? false : inspirationMachineTriggerTime.isValid();
  }

  public SEScalarVolumePerTime getInspirationPatientTriggerFlow()
  {
    if (inspirationPatientTriggerFlow == null)
      inspirationPatientTriggerFlow = new SEScalarVolumePerTime();
    return inspirationPatientTriggerFlow;
  }
  public boolean hasInspirationPatientTriggerFlow()
  {
    return inspirationPatientTriggerFlow == null ? false : inspirationPatientTriggerFlow.isValid();
  }
  
  public SEScalarPressure getInspirationPatientTriggerPressure()
  {
    if (inspirationPatientTriggerPressure == null)
      inspirationPatientTriggerPressure = new SEScalarPressure();
    return inspirationPatientTriggerPressure;
  }
  public boolean hasInspirationPatientTriggerPressure()
  {
    return inspirationPatientTriggerPressure == null ? false : inspirationPatientTriggerPressure.isValid();
  }
  
  public eSwitch getInspirationPatientTriggerRespiratoryModel()
  {
    return inspirationPatientTriggerRespiratoryModel;
  }
  public void setInspirationPatientTriggerRespiratoryModel(eSwitch s)
  {
    inspirationPatientTriggerRespiratoryModel = (s == eSwitch.UNRECOGNIZED) ? null : s;
  }
  public boolean hasInspirationPatientTriggerRespiratoryModel()
  {
    return inspirationPatientTriggerRespiratoryModel != null && inspirationPatientTriggerRespiratoryModel != eSwitch.NullSwitch;
  }
  
  public SEScalarVolume getInspirationLimbVolume()
  {
    if (inspirationLimbVolume == null)
      inspirationLimbVolume = new SEScalarVolume();
    return inspirationLimbVolume;
  }
  public boolean hasInspirationLimbVolume()
  {
    return inspirationLimbVolume == null ? false : inspirationLimbVolume.isValid();
  }
  
  public SEScalarPressureTimePerVolume getInspirationTubeResistance()
  {
    if (inspirationTubeResistance == null)
      inspirationTubeResistance = new SEScalarPressureTimePerVolume();
    return inspirationTubeResistance;
  }
  public boolean hasInspirationTubeResistance()
  {
    return inspirationTubeResistance == null ? false : inspirationTubeResistance.isValid();
  }
  
  public SEScalarPressureTimePerVolume getInspirationValveResistance()
  {
    if (inspirationValveResistance == null)
      inspirationValveResistance = new SEScalarPressureTimePerVolume();
    return inspirationValveResistance;
  }
  public boolean hasInspirationValveResistance()
  {
    return inspirationValveResistance == null ? false : inspirationValveResistance.isValid();
  }
  
  public SEScalarVolume getInspirationValveVolume()
  {
    if (inspirationValveVolume == null)
      inspirationValveVolume = new SEScalarVolume();
    return inspirationValveVolume;
  }
  public boolean hasInspirationValveVolume()
  {
    return inspirationValveVolume == null ? false : inspirationValveVolume.isValid();
  }
  
  public eDriverWaveform getInspirationWaveform()
  {
    return inspirationWaveform;
  }
  public void setInspirationWaveform(eDriverWaveform w)
  {
    inspirationWaveform = (w == eDriverWaveform.UNRECOGNIZED) ? null : w;
  }
  public boolean hasInspirationWaveform()
  {
    return inspirationWaveform != null;
  }
  
  public SEScalarTime getInspirationWaveformPeriod()
  {
    if (inspirationWaveformPeriod == null)
      inspirationWaveformPeriod = new SEScalarTime();
    return inspirationWaveformPeriod;
  }
  public boolean hasInspirationWaveformPeriod()
  {
    return inspirationWaveformPeriod == null ? false : inspirationWaveformPeriod.isValid();
  }
  
  public SEScalarPressure getReliefValveThreshold()
  {
    if (reliefValveThreshold == null)
      reliefValveThreshold = new SEScalarPressure();
    return reliefValveThreshold;
  }
  public boolean hasReliefValveThreshold()
  {
    return reliefValveThreshold == null ? false : reliefValveThreshold.isValid();
  }
  
  public SEScalarVolume getYPieceVolume()
  {
    if (yPieceVolume == null)
      yPieceVolume = new SEScalarVolume();
    return yPieceVolume;
  }
  public boolean hasYPieceVolume()
  {
    return yPieceVolume == null ? false : yPieceVolume.isValid();
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
    String str = "Mechanical Ventilator"
        + "\n\tConnection: " + (hasConnection()?getConnection():"NotProvided")
        
        + "\n\tPositiveEndExpiratoryPressure: " + (hasPositiveEndExpiratoryPressure()?getPositiveEndExpiratoryPressure():"NotProvided")
        + "\n\tFunctionalResidualCapacity: " + (hasFunctionalResidualCapacity()?getFunctionalResidualCapacity():"NotProvided")
        
        + "\n\tExpirationCycleFlow: " + (hasExpirationCycleFlow()?getExpirationCycleFlow():"NotProvided")
        + "\n\tExpirationCyclePressure: " + (hasExpirationCyclePressure()?getExpirationCyclePressure():"NotProvided")
        + "\n\tExpirationCycleVolume: " + (hasExpirationCycleVolume()?getExpirationCycleVolume():"NotProvided")
        + "\n\tExpirationCycleTime: " + (hasExpirationCycleTime()?getExpirationCycleTime():"NotProvided")
        + "\n\tExpirationCycleRespiratoryModel: " + (hasExpirationCycleRespiratoryModel()?getExpirationCycleRespiratoryModel():"NotProvided")
        
        + "\n\tExpirationTubeResistance: " + (hasExpirationTubeResistance()?getExpirationTubeResistance():"NotProvided")
        + "\n\tExpirationValveResistance: " + (hasExpirationValveResistance()?getExpirationValveResistance():"NotProvided")
        + "\n\tExpirationWaveform: " + (hasExpirationWaveform()?getExpirationWaveform():"NotProvided")
        + "\n\tExpirationWaveformPeriod: " + (hasExpirationWaveformPeriod()?getExpirationWaveformPeriod():"NotProvided")
        
        + "\n\tInspirationLimitFlow: " + (hasInspirationLimitFlow()?getInspirationLimitFlow():"NotProvided")
        + "\n\tInspirationLimitPressure: " + (hasInspirationLimitPressure()?getInspirationLimitPressure():"NotProvided")
        + "\n\tInspirationLimitVolume: " + (hasInspirationLimitVolume()?getInspirationLimitVolume():"NotProvided")
        
        + "\n\tInspirationPauseTime: " + (hasInspirationPauseTime()?getInspirationPauseTime():"NotProvided")
        
        + "\n\tPeakInspiratoryPressure: " + (hasPeakInspiratoryPressure()?getPeakInspiratoryPressure():"NotProvided")
        + "\n\tInspirationTargetFlow: " + (hasInspirationTargetFlow()?getInspirationTargetFlow():"NotProvided")
        
        + "\n\tInspirationMachineTriggerTime: " + (hasInspirationMachineTriggerTime()?getInspirationMachineTriggerTime():"NotProvided")

        + "\n\tInspirationPatientTriggerFlow: " + (hasInspirationPatientTriggerFlow()?getInspirationPatientTriggerFlow():"NotProvided")
        + "\n\tInspirationPatientTriggerPressure: " + (hasInspirationPatientTriggerPressure()?getInspirationPatientTriggerPressure():"NotProvided")
        + "\n\tInspirationPatientTriggerRespiratoryModel: " + (hasInspirationPatientTriggerRespiratoryModel()?getInspirationPatientTriggerRespiratoryModel():"NotProvided")
        
        + "\n\tInspirationTubeResistance: " + (hasInspirationTubeResistance()?getInspirationTubeResistance():"NotProvided")
        + "\n\tInspirationValveResistance: " + (hasInspirationValveResistance()?getInspirationValveResistance():"NotProvided")
        + "\n\tInspirationWaveform: " + (hasInspirationWaveform()?getInspirationWaveform():"NotProvided")
        + "\n\tInspirationWaveformPeriod: " + (hasInspirationWaveformPeriod()?getInspirationWaveformPeriod():"NotProvided");
    
    for(SESubstanceFraction sf : this.fractionInspiredGases)
      str += "\n\t"+sf.toString();
    for(SESubstanceConcentration sc : this.concentrationInspiredAerosol)
    str += "\n\t"+sc.toString();
    return str;
  }
}