/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
package com.kitware.pulse.howto;

import java.util.List;

import com.kitware.pulse.cdm.bind.Enums.eSwitch;
import com.kitware.pulse.cdm.engine.SEDataRequestManager;
import com.kitware.pulse.cdm.patient.actions.SERespiratoryMechanicsConfiguration;
import com.kitware.pulse.cdm.properties.CommonUnits.FrequencyUnit;
import com.kitware.pulse.cdm.properties.CommonUnits.PressureTimePerVolumeUnit;
import com.kitware.pulse.cdm.properties.CommonUnits.PressureUnit;
import com.kitware.pulse.cdm.properties.CommonUnits.TimeUnit;
import com.kitware.pulse.cdm.properties.CommonUnits.VolumePerPressureUnit;
import com.kitware.pulse.cdm.properties.CommonUnits.VolumePerTimeUnit;
import com.kitware.pulse.cdm.properties.CommonUnits.VolumeUnit;
import com.kitware.pulse.cdm.properties.SECurve;
import com.kitware.pulse.cdm.properties.SEScalarTime;
import com.kitware.pulse.cdm.properties.SESegmentConstant;
import com.kitware.pulse.cdm.system.physiology.SERespiratoryMechanics;
import com.kitware.pulse.engine.PulseEngine;
import com.kitware.pulse.utilities.JNIBridge;
import com.kitware.pulse.utilities.Log;
import com.kitware.pulse.utilities.LogListener;

public class HowTo_RespiratoryMechanics
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
    PulseEngine pulse = new PulseEngine();
    pulse.setLogListener(new MyListener());
    pulse.setLogFilename("./test_results/howto/HowTo_RespiratoryMechanics.java.log");
    
    // setup data requests to write to a csv file so we can plot data
    SEDataRequestManager dataRequests = new SEDataRequestManager();
    dataRequests.createPhysiologyDataRequest("RespirationRate", FrequencyUnit.Per_min);
    dataRequests.createPhysiologyDataRequest("TidalVolume", VolumeUnit.mL);
    dataRequests.createPhysiologyDataRequest("TotalLungVolume", VolumeUnit.mL);
    dataRequests.createPhysiologyDataRequest("ExpiratoryRespiratoryResistance", PressureTimePerVolumeUnit.cmH2O_s_Per_L);
    dataRequests.createPhysiologyDataRequest("InspiratoryRespiratoryResistance", PressureTimePerVolumeUnit.cmH2O_s_Per_L);
    dataRequests.createPhysiologyDataRequest("RespiratoryCompliance", VolumePerPressureUnit.L_Per_cmH2O);
    dataRequests.createPhysiologyDataRequest("TotalPulmonaryVentilation", VolumePerTimeUnit.L_Per_min);
    dataRequests.setResultsFilename("./test_results/howto/HowTo_RespiratoryMechanics.java.csv");
    
    pulse.serializeFromFile("./states/StandardMale@0s.json", dataRequests);

    for (int i = 0; i < 6; i++)
    {
      pulse.advanceTime_s(10);
      dataValues = pulse.pullData();
      dataRequests.writeData(dataValues);
    }

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
    // const double UpperResistanceFraction = 0.5;
    // UpperResistanceFraction is implied
    double BronchiResistanceFraction = 0.3;
    double AlveoliDuctResistanceFraction = 0.2;

    double upperResistance = resistance_cmH2O_s_Per_L - (BronchiResistanceFraction * resistance_cmH2O_s_Per_L + AlveoliDuctResistanceFraction * resistance_cmH2O_s_Per_L) / 2.0;
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

    for (int i = 0; i < 12; i++)
    {
      pulse.advanceTime_s(10);
      dataValues = pulse.pullData();
      dataRequests.writeData(dataValues);
    }
  }

}
