/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using System;
using System.Collections.Generic;
using Pulse;
using Pulse.CDM;

namespace HowTo_ECMO
{
  class Example
  {
    public static void Run()
    {
      // Create our pulse engine
      PulseEngine pulse = new PulseEngine();
      pulse.LogToConsole(true);// Easily view what is happening

      List<SEDataRequest> data_requests = new List<SEDataRequest>
      {
        // Vitals Monitor Data
        SEDataRequest.CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit.Per_min),
        SEDataRequest.CreatePhysiologyDataRequest("TidalVolume", VolumeUnit.mL),
        SEDataRequest.CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit.mL),
        SEDataRequest.CreatePhysiologyDataRequest("ExpiratoryRespiratoryResistance", PressureTimePerVolumeUnit.cmH2O_s_Per_L),
        SEDataRequest.CreatePhysiologyDataRequest("InspiratoryRespiratoryResistance", PressureTimePerVolumeUnit.cmH2O_s_Per_L),
        SEDataRequest.CreatePhysiologyDataRequest("RespiratoryCompliance", VolumePerPressureUnit.L_Per_cmH2O),
        SEDataRequest.CreatePhysiologyDataRequest("TotalPulmonaryVentilation", VolumePerTimeUnit.L_Per_min),
        SEDataRequest.CreateLiquidCompartmentDataRequest("ECMOBloodSamplingPort", "InFlow"),
        SEDataRequest.CreateLiquidCompartmentDataRequest("ECMOBloodSamplingPort", "OutFlow"),
        SEDataRequest.CreateLiquidCompartmentDataRequest("ECMOOxygenator", "InFlow"),
        SEDataRequest.CreateLiquidCompartmentDataRequest("ECMOOxygenator", "OutFlow"),
        SEDataRequest.CreateLiquidCompartmentDataRequest("RightArmVasculature", "InFlow"),
        SEDataRequest.CreateLiquidCompartmentDataRequest("RightArmVasculature", "OutFlow"),
      };
      SEDataRequestManager data_mgr = new SEDataRequestManager(data_requests);
      data_mgr.SetResultsFilename("./test_results/howto/HowTo_ECMO.cs.csv");
      // Create a reference to a double[] that will contain the data returned from Pulse
      double[] data_values;
      // data_values[0] is ALWAYS the simulation time in seconds
      // The rest of the data values are in order of the data_requests list provided

      if (!pulse.SerializeFromFile("./states/StandardMale@0s.json", data_mgr))
      {
        Console.WriteLine("Error Initializing Pulse!");
        return;
      }
      pulse.AdvanceTime_s(10);
      // Get the values of the data you requested at this time
      data_values = pulse.PullData();
      // And write it out to the console
      data_mgr.WriteData(data_values);

      // Give the patient Dyspnea
      SEECMOConfiguration cfg = new SEECMOConfiguration();
      SEECMOSettings settings = cfg.GetSettings();
      settings.SetInflowLocation(eECMO_CannulationLocation.InternalJugular);
      settings.SetOutflowLocation(eECMO_CannulationLocation.RightSubclavianVein);
      settings.GetOxygenatorVolume().SetValue(500, VolumeUnit.mL);
      settings.GetTransfusionFlow().SetValue(1.0, VolumePerTimeUnit.mL_Per_s);
      settings.SetSubstanceCompound("Saline");
      pulse.ProcessAction(cfg);

      pulse.AdvanceTime_s(10);
      // Get the values of the data you requested at this time
      data_values = pulse.PullData();
      // And write it out to the console
      data_mgr.WriteData(data_values);

    }
  }
}
