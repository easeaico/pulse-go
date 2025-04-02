/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using System;
using System.Collections.Generic;
using HowTo_UseEngine;
using Pulse;
using Pulse.CDM;

namespace HowTo_Hemorrhage
{
  class Example
  {
    public static void Run()
    {
      // Create our pulse engine
      PulseEngine pulse = new PulseEngine();
      pulse.LogToConsole(false);// We don't want C++ writing to console
      pulse.SetLogListener(new MyLogListener());

      List<SEDataRequest> data_requests = new List<SEDataRequest>
      {
        // Vitals Monitor Data
        SEDataRequest.CreatePhysiologyDataRequest("HeartRate", FrequencyUnit.Per_min),
        SEDataRequest.CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit.mmHg),
        SEDataRequest.CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit.mmHg),
        SEDataRequest.CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit.mmHg),
        SEDataRequest.CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit.Per_min),
        SEDataRequest.CreatePhysiologyDataRequest("TidalVolume", VolumeUnit.mL),
        SEDataRequest.CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit.mL),
        SEDataRequest.CreatePhysiologyDataRequest("OxygenSaturation"),
        SEDataRequest.CreatePhysiologyDataRequest("CardiacOutput", VolumePerTimeUnit.mL_Per_min),
        SEDataRequest.CreatePhysiologyDataRequest("BloodVolume", VolumeUnit.mL),
        SEDataRequest.CreatePhysiologyDataRequest("TotalHemorrhageRate", VolumePerTimeUnit.mL_Per_min),
        SEDataRequest.CreatePhysiologyDataRequest("TotalHemorrhagedVolume", VolumeUnit.mL),
        SEDataRequest.CreateActionCompartmentDataRequest("Hemorrhage", "RightLeg", "FlowRate", VolumePerTimeUnit.mL_Per_min),
        SEDataRequest.CreateActionCompartmentDataRequest("Hemorrhage", "RightLeg", "TotalBloodLost", VolumeUnit.mL)
      };
      SEDataRequestManager data_mgr = new SEDataRequestManager(data_requests);
      data_mgr.SetResultsFilename("./test_results/howto/HowToHemorrhage.cs.csv");
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

      // Create a Hemorrhage
      // Set the severity (a fraction between 0 and 1)
      SEHemorrhage hemo = new SEHemorrhage();
      hemo.SetComment("I am a comment");
      hemo.GetSeverity().SetValue(0.75);
      //hemo.GetFlowRate().SetValue(1, VolumePerTimeUnit.mL_Per_s);
      hemo.SetCompartment(eHemorrhage_Compartment.RightLeg);
      pulse.ProcessAction(hemo);

      pulse.AdvanceTime_s(120);
      // Get the values of the data you requested at this time
      data_values = pulse.PullData();
      // And write it out to the console
      data_mgr.WriteData(data_values);

      hemo.GetSeverity().SetValue(0.0);
      hemo.SetCompartment(eHemorrhage_Compartment.RightLeg);
      pulse.ProcessAction(hemo);

      SESubstanceBolus bolus = new SESubstanceBolus();
      bolus.SetSubstance("Morphine");
      bolus.GetConcentration().SetValue(5000, MassPerVolumeUnit.ug_Per_mL);
      bolus.GetDose().SetValue(10, VolumeUnit.mL);
      bolus.SetAdminRoute(eSubstanceAdministration_Route.Intravenous);
      bolus.GetAdminDuration().SetValue(20, TimeUnit.s);
      pulse.ProcessAction(bolus);

      pulse.AdvanceTime_s(240);
      // Get the values of the data you requested at this time
      data_values = pulse.PullData();
      // And write it out to the console
      data_mgr.WriteData(data_values);
    }
  }
}
