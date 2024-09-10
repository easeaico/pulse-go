/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using System;
using System.Collections.Generic;
using Pulse;
using Pulse.CDM;

namespace HowTo_SystemModifiers
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
        SEDataRequest.CreatePhysiologyDataRequest("HeartRate", FrequencyUnit.Per_min),
        SEDataRequest.CreatePhysiologyDataRequest("ArterialPressure", PressureUnit.mmHg),
        SEDataRequest.CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit.mmHg),
        SEDataRequest.CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit.mmHg),
        SEDataRequest.CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit.mmHg),
        SEDataRequest.CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit.Per_min),
        SEDataRequest.CreatePhysiologyDataRequest("TidalVolume", VolumeUnit.mL),
        SEDataRequest.CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit.mL),
        SEDataRequest.CreatePhysiologyDataRequest("ExpiratoryRespiratoryResistance", PressureTimePerVolumeUnit.cmH2O_s_Per_L),
        SEDataRequest.CreatePhysiologyDataRequest("InspiratoryRespiratoryResistance", PressureTimePerVolumeUnit.cmH2O_s_Per_L),
        SEDataRequest.CreatePhysiologyDataRequest("RespiratoryCompliance",VolumePerPressureUnit.L_Per_cmH2O),
        SEDataRequest.CreatePhysiologyDataRequest("TotalPulmonaryVentilation", VolumePerTimeUnit.L_Per_min),
      };
      SEDataRequestManager data_mgr = new SEDataRequestManager(data_requests);
      data_mgr.SetResultsFilename("./test_results/howto/HowTo_SystemModifiers.cs.csv");
      // Create a reference to a double[] that will contain the data returned from Pulse
      double[] data_values;
      // data_values[0] is ALWAYS the simulation time in seconds
      // The rest of the data values are in order of the data_requests list provided

      if (!pulse.SerializeFromFile("./states/StandardMale@0s.json", data_mgr))
      {
        Console.WriteLine("Error Initializing Pulse!");
        return;
      }
      // Get the values of the data you requested at this time
      data_values = pulse.PullData();
      // And write it out to the console
      data_mgr.WriteData(data_values);

      for (int i = 0; i < 3; i++)
      {
        pulse.AdvanceTime_s(10);
        // Get the values of the data you requested at this time
        data_values = pulse.PullData();
        // And write it out to the console
        data_mgr.WriteData(data_values);
      }

      SERespiratoryMechanicsModification rMod = new SERespiratoryMechanicsModification();
      SERespiratoryMechanicsModifiers rMods = rMod.GetModifiers();
      rMods.GetRespirationRateMultiplier().SetValue(1.05);
      pulse.ProcessAction(rMod);

      SECardiovascularMechanicsModification cMod = new SECardiovascularMechanicsModification();
      SECardiovascularMechanicsModifiers cMods = cMod.GetModifiers();
      cMods.GetHeartRateMultiplier().SetValue(1.05);
      pulse.ProcessAction(cMod);

      // By default, the engine will run a stabilization stage to get to a new homeostatis based on the provided modifiers
      // You can listen to the Stabilization event to see when the stabilization stage ends(and starts)
      // If you are slowly modifying the system with your own logic, and don't want the stabilization stage to run
      // set the incremental flag to true, and the engine apply this action and not run a stabilization stage

      for (int i = 0; i < 12; i++)
      {
        pulse.AdvanceTime_s(10);
        // Get the values of the data you requested at this time
        data_values = pulse.PullData();
        // And write it out to the console
        data_mgr.WriteData(data_values);
      }
    }
  }
}
