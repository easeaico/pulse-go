/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
package com.kitware.pulse.howto;

import java.util.List;

import com.kitware.pulse.cdm.bind.ECMO.eECMOCannulationLocation;
import com.kitware.pulse.cdm.engine.SEDataRequestManager;
import com.kitware.pulse.cdm.properties.CommonUnits.*;
import com.kitware.pulse.cdm.system.equipment.ecmo.SEECMOSettings;
import com.kitware.pulse.cdm.system.equipment.ecmo.actions.SEECMOConfiguration;
import com.kitware.pulse.engine.PulseEngine;
import com.kitware.pulse.utilities.JNIBridge;
import com.kitware.pulse.utilities.Log;
import com.kitware.pulse.utilities.LogListener;

public class HowTo_ECMO
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
    pulse.setLogFilename("./test_results/howto/HowTo_ECMO.java.log");
    
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
    // Equipment Data
    dataRequests.createLiquidCompartmentDataRequest("ECMOBloodSamplingPort", "Inflow");
    dataRequests.createLiquidCompartmentDataRequest("ECMOBloodSamplingPort", "Outflow");
    dataRequests.createLiquidCompartmentDataRequest("ECMOOxygenator", "Inflow");
    dataRequests.createLiquidCompartmentDataRequest("ECMOOxygenator", "Outflow");
    dataRequests.createLiquidCompartmentDataRequest("RightArmVasculature", "Inflow");
    dataRequests.createLiquidCompartmentDataRequest("RightArmVasculature", "Outflow");
    
    dataRequests.setResultsFilename("./test_results/howto/HowTo_ECMO.java.csv");
    
    pulse.serializeFromFile("./states/StandardMale@0s.json", dataRequests);
    
    pulse.advanceTime_s(10);
    // Get the values of the data you requested at this time
    data_values = pulse.pullData();
    // And write it out to the console
    dataRequests.writeData(data_values);

    SEECMOConfiguration cfg = new SEECMOConfiguration();
    SEECMOSettings settings = cfg.getSettings();
    settings.setInflowLocation(eECMOCannulationLocation.InternalJugular);
    settings.setOutflowLocation(eECMOCannulationLocation.RightSubclavianVein);
    settings.getOxygenatorVolume().setValue(500, VolumeUnit.mL);
    settings.getTransfusionFlow().setValue(1.0, VolumePerTimeUnit.mL_Per_s);
    settings.setSubstanceCompound("Saline");
    pulse.processAction(cfg);

    pulse.advanceTime_s(10);
    // Get the values of the data you requested at this time
    data_values = pulse.pullData();
    // And write it out to the console
    dataRequests.writeData(data_values);
    
  }

}
