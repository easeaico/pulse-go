/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using System;
using System.Collections.Generic;
using Pulse;
using Pulse.CDM;

namespace HowTo_Environment
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
        SEDataRequest.CreatePhysiologyDataRequest("OxygenSaturation"),
        SEDataRequest.CreatePhysiologyDataRequest("EndTidalCarbonDioxidePressure", PressureUnit.mmHg),
        SEDataRequest.CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit.Per_min),
        SEDataRequest.CreatePhysiologyDataRequest("SkinTemperature", TemperatureUnit.C),
        SEDataRequest.CreateGasCompartmentDataRequest("Carina", "CarbonDioxide", "PartialPressure", PressureUnit.mmHg),
        SEDataRequest.CreatePhysiologyDataRequest("BloodVolume", VolumeUnit.mL),
        SEDataRequest.CreateECGDataRequest("Lead3ElectricPotential", ElectricPotentialUnit.mV),
      };
      SEDataRequestManager data_mgr = new SEDataRequestManager(data_requests);
      // Create a reference to a double[] that will contain the data returned from Pulse
      double[] data_values;
      // data_values[0] is ALWAYS the simulation time in seconds
      // The rest of the data values are in order of the data_requests list provided

      // Initialize the environment to a specific envirionment
      SEPatientConfiguration pc = new SEPatientConfiguration();
      pc.SetPatientFile("./patients/Soldier.json");
      SEInitialEnvironmentalConditions envC = pc.GetConditions().GetInitialEnvironmentalConditions();
      SEEnvironmentalConditions env = envC.GetEnvironmentalConditions();
      // Let's  load up a file from disk (You don't have to start with a file)
      env.SerializeFromFile("./environments/ExerciseEnvironment.json");
      // Now let's modify a few properties
      env.GetAirDensity().SetValue(1.225, MassPerVolumeUnit.kg_Per_m3);
      env.GetAmbientTemperature().SetValue(33, TemperatureUnit.C);
      env.GetRespirationAmbientTemperature().SetValue(33, TemperatureUnit.C);

      // Initialize the engine with our configuration
      // NOTE: No data requests are being provided, so Pulse will return the default vitals data
      if (!pulse.InitializeEngine(pc, data_mgr))
      {
        Console.WriteLine("Error Initializing Pulse!");
        return;
      }

      // Get the values of the data you requested at this time
      data_values = pulse.PullData();
      // And write it out to the console
      data_mgr.WriteData(data_values);

      // Change the environment
      SEChangeEnvironmentalConditions change = new SEChangeEnvironmentalConditions();
      change.SetComment("Modify Environmental Conditions");
      // You could just point to an environment file if you want
      // We provide a few in the bin/environments folder
      // change.set_environmental_conditions_file("/path/to/file")
      SEEnvironmentalConditions envConditions = change.GetEnvironmentalConditions();
      // Not all properties have to be set on the environmental conditions
      // Properties not included will retain their current value
      envConditions.GetAirVelocity().SetValue(0.2, LengthPerTimeUnit.m_Per_s);
      envConditions.GetAmbientTemperature().SetValue(22, TemperatureUnit.C);
      envConditions.GetAtmosphericPressure().SetValue(525, PressureUnit.mmHg);
      envConditions.GetClothingResistance().SetValue(0.5, HeatResistanceAreaUnit.clo);
      envConditions.GetMechanicalDeadSpace().SetValue(0.5, VolumeUnit.L);
      envConditions.GetRelativeHumidity().SetValue(0.5);
      // When changing ambient gas mixture, fractions must sum to 1
      envConditions.RemoveAmbientGases();
      envConditions.GetAmbientGas("Nitrogen").GetFractionAmount().SetValue(0.7896);
      envConditions.GetAmbientGas("Oxygen").GetFractionAmount().SetValue(0.21);
      envConditions.GetAmbientGas("CarbonDioxide").GetFractionAmount().SetValue(4.0E-4);
      // Note CarbonMonoxide can be added to the ambient gas mixture this way
      pulse.ProcessAction(change);

      SEThermalApplication ta = new SEThermalApplication();
      ta.GetActiveCooling().GetPower().SetValue(5, PowerUnit.BTU_Per_hr);
      ta.GetActiveCooling().GetSurfaceAreaFraction().SetValue(0.2);
      ta.GetActiveHeating().GetPower().SetValue(5, PowerUnit.BTU_Per_hr);
      ta.GetActiveHeating().GetSurfaceArea().SetValue(1, AreaUnit.cm2);
      ta.GetAppliedTemperature().SetState(eSwitch.On);
      ta.GetAppliedTemperature().GetTemperature().SetValue(15, TemperatureUnit.C);
      ta.GetAppliedTemperature().GetSurfaceAreaFraction().SetValue(0.4);
      pulse.ProcessAction(ta);

      // Advance some time and print out the vitals
      pulse.AdvanceTime_s(300);
      // Get the values of the data you requested at this time
      data_values = pulse.PullData();
      // And write it out to the console
      data_mgr.WriteData(data_values);
    }
  }
}
