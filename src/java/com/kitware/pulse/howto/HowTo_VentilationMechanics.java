/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
package com.kitware.pulse.howto;

import java.util.List;

import com.kitware.pulse.cdm.bind.Enums.eSwitch;
import com.kitware.pulse.cdm.bind.MechanicalVentilatorActions.MechanicalVentilatorPressureControlData.eMode;
import com.kitware.pulse.cdm.bind.Patient.PatientData.eSex;
import com.kitware.pulse.cdm.bind.PatientActions.IntubationData.eType;
import com.kitware.pulse.cdm.engine.SEDataRequestManager;
import com.kitware.pulse.cdm.engine.SEPatientConfiguration;
import com.kitware.pulse.cdm.patient.actions.SEIntubation;
import com.kitware.pulse.cdm.patient.actions.SERespiratoryMechanicsConfiguration;
import com.kitware.pulse.cdm.properties.CommonUnits.FrequencyUnit;
import com.kitware.pulse.cdm.properties.CommonUnits.PressureTimePerVolumeUnit;
import com.kitware.pulse.cdm.properties.CommonUnits.PressureUnit;
import com.kitware.pulse.cdm.properties.CommonUnits.TimeUnit;
import com.kitware.pulse.cdm.properties.CommonUnits.VolumePerPressureUnit;
import com.kitware.pulse.cdm.properties.CommonUnits.VolumePerTimeUnit;
import com.kitware.pulse.cdm.properties.CommonUnits.VolumeUnit;
import com.kitware.pulse.cdm.properties.SECurve;
import com.kitware.pulse.cdm.properties.SESegmentConstant;
import com.kitware.pulse.cdm.system.equipment.mechanical_ventilator.actions.*;
import com.kitware.pulse.cdm.system.physiology.SERespiratoryMechanics;
import com.kitware.pulse.engine.PulseEngine;
import com.kitware.pulse.engine.bind.Enums.eModelType;
import com.kitware.pulse.utilities.JNIBridge;
import com.kitware.pulse.utilities.Log;
import com.kitware.pulse.utilities.LogListener;

public class HowTo_VentilationMechanics
{
  protected static class MyListener extends LogListener
  {
    protected MyListener()
    {
      super();
      listen(false);
    }    
    @Override public void handleDebug(String msg) { Log.debug(msg); }
    @Override public void handleInfo(String msg)  { Log.info(msg); }
    @Override public void handleWarn(String msg)  { Log.warn(msg); }
    @Override public void handleError(String msg) { Log.error(msg); }
    @Override public void handleFatal(String msg) { Log.fatal(msg); }
  }
  
  public static void main(String[] args)
  {
    List<Double> dataValues;
    
    JNIBridge.initialize();
    PulseEngine pulse = new PulseEngine(eModelType.HumanAdultVentilationMechanics);
    pulse.setLogListener(new MyListener());
    pulse.setLogFilename("./test_results/howto/HowTo_VentilationMechanics.java.log");
    
    // Create data requests for each value that should be written to the output log as the engine is executing
    // Physiology System Names are defined on the System Objects
    SEDataRequestManager dataRequests = new SEDataRequestManager();
    dataRequests.createPhysiologyDataRequest("RespirationRate", FrequencyUnit.Per_min);
    dataRequests.createPhysiologyDataRequest("TidalVolume", VolumeUnit.mL);
    dataRequests.createPhysiologyDataRequest("TotalLungVolume", VolumeUnit.mL);
    dataRequests.createPhysiologyDataRequest("InspiratoryExpiratoryRatio");
    dataRequests.createPhysiologyDataRequest("MeanAirwayPressure", PressureUnit.cmH2O);
    dataRequests.createPhysiologyDataRequest("TransrespiratoryPressure", PressureUnit.cmH2O);
    dataRequests.createPhysiologyDataRequest("IntrapulmonaryPressure", PressureUnit.cmH2O);
    dataRequests.createPhysiologyDataRequest("InspiratoryFlow", VolumePerTimeUnit.L_Per_min);
    dataRequests.createPhysiologyDataRequest("ExpiratoryRespiratoryResistance", PressureTimePerVolumeUnit.cmH2O_s_Per_mL);
    dataRequests.createPhysiologyDataRequest("InspiratoryRespiratoryResistance", PressureTimePerVolumeUnit.cmH2O_s_Per_mL);
    dataRequests.createPhysiologyDataRequest("RespiratoryCompliance", VolumePerPressureUnit.mL_Per_cmH2O);
    dataRequests.createPhysiologyDataRequest("TotalPulmonaryVentilation", VolumePerTimeUnit.mL_Per_min);
    // Ventilator Monitor Data
    dataRequests.createMechanicalVentilatorDataRequest("AirwayPressure", PressureUnit.cmH2O);
    dataRequests.createMechanicalVentilatorDataRequest("BreathState");
    dataRequests.createMechanicalVentilatorDataRequest("DynamicRespiratoryCompliance", VolumePerPressureUnit.mL_Per_cmH2O);
    dataRequests.createMechanicalVentilatorDataRequest("EndTidalCarbonDioxideFraction");
    dataRequests.createMechanicalVentilatorDataRequest("EndTidalCarbonDioxidePressure", PressureUnit.cmH2O);
    dataRequests.createMechanicalVentilatorDataRequest("EndTidalOxygenFraction");
    dataRequests.createMechanicalVentilatorDataRequest("EndTidalOxygenPressure", PressureUnit.cmH2O);
    dataRequests.createMechanicalVentilatorDataRequest("ExpiratoryFlow", VolumePerTimeUnit.L_Per_s);
    dataRequests.createMechanicalVentilatorDataRequest("ExpiratoryResistance", PressureTimePerVolumeUnit.cmH2O_s_Per_L);
    dataRequests.createMechanicalVentilatorDataRequest("ExpiratoryTidalVolume", VolumeUnit.L);
    dataRequests.createMechanicalVentilatorDataRequest("ExtrinsicPositiveEndExpiratoryPressure", PressureUnit.cmH2O);
    dataRequests.createMechanicalVentilatorDataRequest("InspiratoryExpiratoryRatio");
    dataRequests.createMechanicalVentilatorDataRequest("InspiratoryFlow", VolumePerTimeUnit.L_Per_s);
    dataRequests.createMechanicalVentilatorDataRequest("InspiratoryResistance", PressureTimePerVolumeUnit.cmH2O_s_Per_L);
    dataRequests.createMechanicalVentilatorDataRequest("InspiratoryTidalVolume", VolumeUnit.L);
    dataRequests.createMechanicalVentilatorDataRequest("IntrinsicPositiveEndExpiratoryPressure", PressureUnit.cmH2O);
    dataRequests.createMechanicalVentilatorDataRequest("LeakFraction");
    dataRequests.createMechanicalVentilatorDataRequest("MeanAirwayPressure", PressureUnit.cmH2O);
    dataRequests.createMechanicalVentilatorDataRequest("PeakInspiratoryFlow", VolumePerTimeUnit.L_Per_s);
    dataRequests.createMechanicalVentilatorDataRequest("PeakInspiratoryPressure", PressureUnit.cmH2O);
    dataRequests.createMechanicalVentilatorDataRequest("PlateauPressure", PressureUnit.cmH2O);
    dataRequests.createMechanicalVentilatorDataRequest("RespirationRate", FrequencyUnit.Per_min);
    dataRequests.createMechanicalVentilatorDataRequest("StaticRespiratoryCompliance", VolumePerPressureUnit.mL_Per_cmH2O);
    dataRequests.createMechanicalVentilatorDataRequest("TidalVolume", VolumeUnit.L);
    dataRequests.createMechanicalVentilatorDataRequest("TotalLungVolume", VolumeUnit.L);
    dataRequests.createMechanicalVentilatorDataRequest("TotalPositiveEndExpiratoryPressure", PressureUnit.cmH2O);
    dataRequests.createMechanicalVentilatorDataRequest("TotalPulmonaryVentilation", VolumePerTimeUnit.L_Per_s);
    dataRequests.setResultsFilename("./test_results/howto/HowTo_VentilationMechanics.cpp.csv");

    // You only need to set a sex
    SEPatientConfiguration pCfg = new SEPatientConfiguration();
    pCfg.getPatient().setSex(eSex.Male);

    // State serialization is not needed, this initializes instantaneously
    pulse.initializeEngine(pCfg, dataRequests);

    // Since this engine is just the ventilation mechanics
    // You configure the patient respiratory via the RespiratoryMechanics structure
    SERespiratoryMechanicsConfiguration config = new SERespiratoryMechanicsConfiguration();
    SERespiratoryMechanics mechanics = config.getSettings();
    mechanics.setActive(eSwitch.On);
    // Create a mechanics profile
    double resistance_cmH2O_s_Per_L = 13.0;
    double compliance_mL_Per_cmH2O = 50.0;
    double musclePressure_cmH2O = 13.0;
    double respirationRate_bpm = 15.0;
    double inspiratoryTime_s = 0.9;

    // These need to add to 1.0 (100%)
    double UpperResistanceFraction = 0.5;
    double BronchiResistanceFraction = 0.3;
    double AlveoliDuctResistanceFraction = 0.2;

    double upperResistance = resistance_cmH2O_s_Per_L - (BronchiResistanceFraction * resistance_cmH2O_s_Per_L + AlveoliDuctResistanceFraction * resistance_cmH2O_s_Per_L) / 2;
    double bronchiResistance = 2 * (resistance_cmH2O_s_Per_L - upperResistance) - AlveoliDuctResistanceFraction * resistance_cmH2O_s_Per_L;
    double alveoliDuctResistance = 2 * (resistance_cmH2O_s_Per_L - upperResistance) - bronchiResistance;
    double sideResistance = bronchiResistance + alveoliDuctResistance;

    double RespiratorySideCompliance_L_Per_cmH2O = compliance_mL_Per_cmH2O / 2.0;
    double LungCompliance_L_Per_cmH2O = 2.0 * RespiratorySideCompliance_L_Per_cmH2O;
    double ChestWallCompliance_L_Per_cmH2O = LungCompliance_L_Per_cmH2O;

    double totalBreathTime_s = 1.0 / (respirationRate_bpm / 60.0);
    double inspiratoryFraction = inspiratoryTime_s / totalBreathTime_s;
    double expiratoryFraction = 1.0 - inspiratoryFraction;

    double InspiratoryRiseFraction = inspiratoryFraction;
    double InspiratoryHoldFraction = 0.0;
    double InspiratoryReleaseFraction = Math.min(inspiratoryFraction, expiratoryFraction * 0.5);
    double InspiratoryToExpiratoryPauseFraction = 1.0 - InspiratoryRiseFraction - InspiratoryReleaseFraction;
    double ExpiratoryRiseFraction = 0.0;
    double ExpiratoryHoldFraction = 0.0;
    double ExpiratoryReleaseFraction = 0.0;

    mechanics.getUpperInspiratoryResistance().setValue(upperResistance, PressureTimePerVolumeUnit.cmH2O_s_Per_L);
    mechanics.getUpperExpiratoryResistance().setValue(upperResistance, PressureTimePerVolumeUnit.cmH2O_s_Per_L);
    mechanics.getLeftInspiratoryResistance().setValue(sideResistance, PressureTimePerVolumeUnit.cmH2O_s_Per_L);
    mechanics.getLeftExpiratoryResistance().setValue(sideResistance, PressureTimePerVolumeUnit.cmH2O_s_Per_L);
    mechanics.getRightInspiratoryResistance().setValue(sideResistance, PressureTimePerVolumeUnit.cmH2O_s_Per_L);
    mechanics.getRightExpiratoryResistance().setValue(sideResistance, PressureTimePerVolumeUnit.cmH2O_s_Per_L);

    SECurve lcc = mechanics.getLeftComplianceCurve();
    SESegmentConstant lc = lcc.addConstantSegment();
    lc.getBeginVolume().setValue(Double.NEGATIVE_INFINITY, VolumeUnit.mL);
    lc.getEndVolume().setValue(Double.POSITIVE_INFINITY, VolumeUnit.mL);
    lc.getCompliance().setValue(RespiratorySideCompliance_L_Per_cmH2O, VolumePerPressureUnit.mL_Per_cmH2O);
    SECurve rcc = mechanics.getRightComplianceCurve();
    SESegmentConstant rc = rcc.addConstantSegment();
    rc.getBeginVolume().setValue(Double.NEGATIVE_INFINITY, VolumeUnit.mL);
    rc.getEndVolume().setValue(Double.POSITIVE_INFINITY, VolumeUnit.mL);
    rc.getCompliance().setValue(RespiratorySideCompliance_L_Per_cmH2O, VolumePerPressureUnit.mL_Per_cmH2O);

    mechanics.getInspiratoryRiseTime().setValue(InspiratoryRiseFraction * totalBreathTime_s, TimeUnit.s);
    mechanics.getInspiratoryHoldTime().setValue(InspiratoryHoldFraction * totalBreathTime_s, TimeUnit.s);
    mechanics.getInspiratoryReleaseTime().setValue(InspiratoryReleaseFraction * totalBreathTime_s, TimeUnit.s);
    mechanics.getInspiratoryToExpiratoryPauseTime().setValue(InspiratoryToExpiratoryPauseFraction * totalBreathTime_s, TimeUnit.s);
    mechanics.getExpiratoryRiseTime().setValue(ExpiratoryRiseFraction * totalBreathTime_s, TimeUnit.s);
    mechanics.getExpiratoryHoldTime().setValue(ExpiratoryHoldFraction * totalBreathTime_s, TimeUnit.s);
    mechanics.getExpiratoryReleaseTime().setValue(ExpiratoryReleaseFraction * totalBreathTime_s, TimeUnit.s);
    mechanics.getInspiratoryPeakPressure().setValue(-13.0, PressureUnit.cmH2O);
    mechanics.getExpiratoryPeakPressure().setValue(0.0, PressureUnit.cmH2O);
    pulse.processAction(config);

    // Intubate
    SEIntubation intubation = new SEIntubation();
    intubation.setType(eType.Tracheal);
    pulse.processAction(intubation);

    // Setup the ventilator
    SEMechanicalVentilatorPressureControl pc_ac = new SEMechanicalVentilatorPressureControl();
    pc_ac.setConnection(eSwitch.On);
    pc_ac.setMode(eMode.AssistedControl);
    pc_ac.getFractionInspiredOxygen().setValue(0.21);
    pc_ac.getInspiratoryPeriod().setValue(1.0, TimeUnit.s);
    pc_ac.getInspiratoryPressure().setValue(19.0, PressureUnit.cmH2O);
    pc_ac.getPositiveEndExpiratoryPressure().setValue(5.0, PressureUnit.cmH2O);
    pc_ac.getRespirationRate().setValue(12.0, FrequencyUnit.Per_min);
    pc_ac.getSlope().setValue(0.2, TimeUnit.s);
    pulse.processAction(pc_ac);

    for (int i = 0; i < 12; i++)
    {
      pulse.advanceTime_s(10);
      dataValues = pulse.pullData();
      dataRequests.writeData(dataValues);
    }
  }

}
