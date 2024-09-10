/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
package com.kitware.pulse.howto;

import java.util.List;

import com.kitware.pulse.cdm.bind.Enums.eDriverWaveform;
import com.kitware.pulse.cdm.bind.Enums.eSwitch;
import com.kitware.pulse.cdm.bind.MechanicalVentilatorActions.*;
import com.kitware.pulse.cdm.bind.Physiology.eLungCompartment;
import com.kitware.pulse.cdm.engine.SEDataRequestManager;
import com.kitware.pulse.cdm.patient.actions.SEAcuteRespiratoryDistressSyndromeExacerbation;
import com.kitware.pulse.cdm.patient.actions.SEDyspnea;
import com.kitware.pulse.cdm.patient.conditions.SEAcuteRespiratoryDistressSyndrome;
import com.kitware.pulse.cdm.properties.CommonUnits.*;
import com.kitware.pulse.cdm.properties.SEScalarTime;
import com.kitware.pulse.cdm.substance.SESubstanceFraction;
import com.kitware.pulse.cdm.system.equipment.mechanical_ventilator.SEMechanicalVentilatorSettings;
import com.kitware.pulse.cdm.system.equipment.mechanical_ventilator.actions.*;
import com.kitware.pulse.engine.PulseEngine;
import com.kitware.pulse.utilities.JNIBridge;
import com.kitware.pulse.utilities.Log;
import com.kitware.pulse.utilities.LogListener;

public class HowTo_MechanicalVentilator
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
    List<Double> data_values;
    
    JNIBridge.initialize();
    PulseEngine pulse = new PulseEngine();
    pulse.setLogListener(new MyListener());
    pulse.setLogFilename("./test_results/howto/HowTo_MechanicalVentilator.java.log");
    
    SEDataRequestManager dataRequests = new SEDataRequestManager();// Vitals Monitor Data
    dataRequests.createPhysiologyDataRequest("HeartRate", FrequencyUnit.Per_min);
    dataRequests.createPhysiologyDataRequest("ArterialPressure", PressureUnit.mmHg);
    dataRequests.createPhysiologyDataRequest("MeanArterialPressure", PressureUnit.mmHg);
    dataRequests.createPhysiologyDataRequest("SystolicArterialPressure", PressureUnit.mmHg);
    dataRequests.createPhysiologyDataRequest("DiastolicArterialPressure", PressureUnit.mmHg);
    dataRequests.createPhysiologyDataRequest("OxygenSaturation");
    dataRequests.createPhysiologyDataRequest("EndTidalCarbonDioxidePressure", PressureUnit.mmHg);
    dataRequests.createPhysiologyDataRequest("RespirationRate", FrequencyUnit.Per_min);
    dataRequests.createPhysiologyDataRequest("SkinTemperature", TemperatureUnit.C);
    dataRequests.createGasCompartmentDataRequest("Carina", "CarbonDioxide", "PartialPressure", PressureUnit.mmHg);
    dataRequests.createPhysiologyDataRequest("BloodVolume", VolumeUnit.mL);
    dataRequests.createECGDataRequest("Lead3ElectricPotential", ElectricPotentialUnit.mV);
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
    
    pulse.serializeFromFile("./states/StandardMale@0s.json", dataRequests);
    
    SEAcuteRespiratoryDistressSyndromeExacerbation ards = new SEAcuteRespiratoryDistressSyndromeExacerbation();
    ards.getSeverity(eLungCompartment.LeftLung).setValue(0.5);
    ards.getSeverity(eLungCompartment.RightLung).setValue(0.5);
    pulse.processAction(ards);
    
    SEDyspnea dyspnea = new SEDyspnea();
    dyspnea.getTidalVolumeSeverity().setValue(1.0);
    pulse.processAction(dyspnea);
    
 // We have action support for several commonly used ventilator modes
    // Pulse is not limited to these modes, These modes are designe for simple understanding
    // Our implementation supports any ventilator mode, you will just need to translate the user facing inputs
    // to a timing/control profile using our configuration settings
    // These modes are internally converted into a configuration setting,
    // You can retrieve the resulting settings action and use it as a basis for more configurations if you want.
    // For example, if you wanted to lengthen the InspirationPatientTriggerFlow of the mode

    SEMechanicalVentilatorContinuousPositiveAirwayPressure cpap = new SEMechanicalVentilatorContinuousPositiveAirwayPressure();
    cpap.setConnection(eSwitch.On);
    cpap.getFractionInspiredOxygen().setValue(0.21);
    cpap.getDeltaPressureSupport().setValue(10.0, PressureUnit.cmH2O);
    cpap.getPositiveEndExpiratoryPressure().setValue(5.0, PressureUnit.cmH2O);
    cpap.getSlope().setValue(0.2, TimeUnit.s);
    pulse.processAction(cpap);
    pulse.advanceTime_s(10);
    // get the values of the data you requested at this time
    data_values = pulse.pullData();
    // And write it out to the console
    dataRequests.writeData(data_values);

    SEMechanicalVentilatorPressureControl pc_ac = new SEMechanicalVentilatorPressureControl();
    pc_ac.setConnection(eSwitch.On);
    pc_ac.setMode(MechanicalVentilatorPressureControlData.eMode.AssistedControl);
    pc_ac.getFractionInspiredOxygen().setValue(0.21);
    pc_ac.getInspiratoryPeriod().setValue(1.0,TimeUnit.s);
    pc_ac.getInspiratoryPressure().setValue(19.0, PressureUnit.cmH2O);
    pc_ac.getPositiveEndExpiratoryPressure().setValue(5.0, PressureUnit.cmH2O);
    pc_ac.getRespirationRate().setValue(12.0, FrequencyUnit.Per_min);
    pc_ac.getSlope().setValue(0, TimeUnit.s);
    pulse.processAction(pc_ac);
    pulse.advanceTime_s(10);
    // get the values of the data you requested at this time
    data_values = pulse.pullData();
    // And write it out to the console
    dataRequests.writeData(data_values);

    SEMechanicalVentilatorVolumeControl vc_ac = new SEMechanicalVentilatorVolumeControl();
    vc_ac.setConnection(eSwitch.On);
    vc_ac.setMode(MechanicalVentilatorVolumeControlData.eMode.AssistedControl);
    vc_ac.getFlow().setValue(60.0, VolumePerTimeUnit.L_Per_min);
    vc_ac.getFractionInspiredOxygen().setValue(0.21);
    vc_ac.getInspiratoryPeriod().setValue(1.0, TimeUnit.s);
    vc_ac.getPositiveEndExpiratoryPressure().setValue(5.0, PressureUnit.cmH2O);
    vc_ac.getRespirationRate().setValue(12.0, FrequencyUnit.Per_min);
    vc_ac.getTidalVolume().setValue(900.0, VolumeUnit.mL);
    pulse.processAction(vc_ac);
    pulse.advanceTime_s(10);
    // get the values of the data you requested at this time
    data_values = pulse.pullData();
    // And write it out to the console
    dataRequests.writeData(data_values);

    // Here is an example of programming a custom ventilator mode
    SEMechanicalVentilatorConfiguration mv_config = new SEMechanicalVentilatorConfiguration();
    SEMechanicalVentilatorSettings mv = mv_config.getSettings();
    mv.setConnection(eSwitch.On);
    mv.setInspirationWaveform(eDriverWaveform.Square);
    mv.setExpirationWaveform(eDriverWaveform.Square);
    mv.getPeakInspiratoryPressure().setValue(21.0, PressureUnit.cmH2O);
    mv.getPositiveEndExpiratoryPressure().setValue(10.0, PressureUnit.cmH2O);
    SESubstanceFraction fractionFiO2 = mv.getFractionInspiredGas("Oxygen");
    fractionFiO2.getAmount().setValue(0.5);
    double respirationRate_per_min = 20.0;
    double IERatio = 0.5;
    // Translate ventilator settings
    double totalPeriod_s = 60.0 / respirationRate_per_min;
    double inspiratoryPeriod_s = IERatio * totalPeriod_s / (1 + IERatio);
    double expiratoryPeriod_s = totalPeriod_s - inspiratoryPeriod_s;
    mv.getInspirationMachineTriggerTime().setValue(expiratoryPeriod_s, TimeUnit.s);
    mv.getExpirationCycleTime().setValue(inspiratoryPeriod_s, TimeUnit.s);
    pulse.processAction(mv_config);
    // Advance some time and print out the vitals
    pulse.advanceTime_s(10);
    // get the values of the data you requested at this time
    data_values = pulse.pullData();
    // And write it out to the console
    dataRequests.writeData(data_values);

    // You can also perform a hold
    SEMechanicalVentilatorHold hold = new SEMechanicalVentilatorHold();
    hold.setState(eSwitch.On);
    pulse.processAction(hold);
    // Advance some time and print out the vitals
    pulse.advanceTime_s(5);
    // get the values of the data you requested at this time
    data_values = pulse.pullData();
    // And write it out to the console
    dataRequests.writeData(data_values);
    hold.setState(eSwitch.Off);
    pulse.processAction(hold);
    // Advance some time and print out the vitals
    pulse.advanceTime_s(5);
    // get the values of the data you requested at this time
    data_values = pulse.pullData();
    // And write it out to the console
    dataRequests.writeData(data_values);

    // A leak can be specified
    SEMechanicalVentilatorLeak leak = new SEMechanicalVentilatorLeak();
    leak.getSeverity().setValue(0.5);
    pulse.processAction(leak);
    // Advance some time and print out the vitals
    pulse.advanceTime_s(5);
    // get the values of the data you requested at this time
    data_values = pulse.pullData();
    // And write it out to the console
    dataRequests.writeData(data_values);
    leak.getSeverity().setValue(0.0);// Turn off the leak
    pulse.processAction(leak);
    // Advance some time and print out the vitals
    pulse.advanceTime_s(5);
    // get the values of the data you requested at this time
    data_values = pulse.pullData();
    // And write it out to the console
    dataRequests.writeData(data_values);
  }

}
