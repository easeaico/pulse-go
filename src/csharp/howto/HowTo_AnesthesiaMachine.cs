/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using System;
using System.Collections.Generic;
using System.Runtime.Remoting.Messaging;
using Pulse;
using Pulse.CDM;

namespace HowTo_AnesthesiaMachine
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
        SEDataRequest.CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit.mmHg),
        SEDataRequest.CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit.mmHg),
        SEDataRequest.CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit.mmHg),
        SEDataRequest.CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit.Per_min),
        SEDataRequest.CreatePhysiologyDataRequest("TidalVolume", VolumeUnit.mL),
        SEDataRequest.CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit.mL),
        SEDataRequest.CreatePhysiologyDataRequest("OxygenSaturation"),
        SEDataRequest.CreatePhysiologyDataRequest("SedationLevel"),
        SEDataRequest.CreateSubstanceDataRequest("Desflurane", "PlasmaConcentration", MassPerVolumeUnit.g_Per_mL),
      };
      SEDataRequestManager data_mgr = new SEDataRequestManager(data_requests);
      data_mgr.SetResultsFilename("./test_results/howto/HowTo_AnesthesiaMachine.cs.csv");
      // Create a reference to a double[] that will contain the data returned from Pulse
      double[] data_values;
      // data_values[0] is ALWAYS the simulation time in seconds
      // The rest of the data values are in order of the data_requests list provided

      if (!pulse.SerializeFromFile("./states/StandardMale@0s.json", data_mgr))
      {
        Console.WriteLine("Error Initializing Pulse!");
        return;
      }
      Console.WriteLine("The patient is nice and healthy");
      // Get the values of the data you requested at this time
      data_values = pulse.PullData();
      // And write it out to the console
      data_mgr.WriteData(data_values);

      pulse.AdvanceTime_s(50);

      // Turn the anesthesia machine on and get it configured for spontaneous breathing
      // Create an Anesthesia Machine and configure it as needed
      SEAnesthesiaMachineConfiguration AMConfig = new SEAnesthesiaMachineConfiguration();

      // You can set configuration by modifing the configuration class directly
      // Or you can point to an json with configuration data.
      // Modifying the class will keep any old settings that are not provided in the config
      // Using a json will set the anesthesia machine to only the property states specified in the file
      SEAnesthesiaMachine config = AMConfig.GetConfiguration();
      config.SetConnection(eSwitch.On);
      config.GetInletFlow().SetValue(2.0, VolumePerTimeUnit.L_Per_min);
      config.GetInspiratoryExpiratoryRatio().SetValue(.5);
      config.GetOxygenFraction().SetValue(.5);
      config.SetOxygenSource(eAnesthesiaMachine_OxygenSource.Wall);
      config.GetPositiveEndExpiratoryPressure().SetValue(0.0, PressureUnit.cmH2O);
      config.SetPrimaryGas(eAnesthesiaMachine_PrimaryGas.Nitrogen);
      config.GetReliefValvePressure().SetValue(20.0, PressureUnit.cmH2O);
      config.GetRespiratoryRate().SetValue(12, FrequencyUnit.Per_min);
      config.GetPeakInspiratoryPressure().SetValue(0.0, PressureUnit.cmH2O);
      config.GetOxygenBottleOne().GetVolume().SetValue(660.0, VolumeUnit.L);
      config.GetOxygenBottleTwo().GetVolume().SetValue(660.0, VolumeUnit.L);
      // Add a gas drug
      config.GetLeftChamber().SetState(eSwitch.On);
      config.GetLeftChamber().SetSubstance("Desflurane");
      config.GetLeftChamber().GetSubstanceFraction().SetValue(0.06);


      // Process the action to propagate state into the engine
      pulse.ProcessAction(AMConfig);
      Console.WriteLine("Turning on the Anesthesia Machine and placing mask on patient for spontaneous breathing with machine connection.");

      pulse.AdvanceTime_s(60);

      Console.WriteLine("The patient is attempting to breath normally with Anesthesia Machine connected");
      data_values = pulse.PullData();
      // And write it out to the console
      data_mgr.WriteData(data_values);

      // Get the Succinylcholine substance from the substance manager
      const string succs = "Succinylcholine";

      // Create a substance bolus action to administer the substance
      SESubstanceBolus bolus = new SESubstanceBolus();
      bolus.SetSubstance(succs);
      bolus.GetConcentration().SetValue(4820, MassPerVolumeUnit.ug_Per_mL);
      bolus.GetDose().SetValue(20, VolumeUnit.mL);
      bolus.SetAdminRoute(eSubstanceAdministration_Route.Intravenous);
      pulse.ProcessAction(bolus);

      Console.WriteLine("Giving the patient Succinylcholine to test machine-driven ventilation.");

      pulse.AdvanceTime_s(60);

      Console.WriteLine("It has been 60s since the Succinylcholine administration.");
      data_values = pulse.PullData();
      // And write it out to the console
      data_mgr.WriteData(data_values);

      config.GetInletFlow().SetValue(5.0, VolumePerTimeUnit.L_Per_min);
      config.GetPositiveEndExpiratoryPressure().SetValue(3.0, PressureUnit.cmH2O);
      config.GetPeakInspiratoryPressure().SetValue(22.0, PressureUnit.cmH2O);
      pulse.ProcessAction(AMConfig);
      Console.WriteLine("Setting the ventilator pressure to drive the machine. Also increasing the inlet flow and positive end expired pressure to test machine controls.");

      pulse.AdvanceTime_s(60);

      Console.WriteLine("Patient breathing is being controlled by the machine.");
      data_values = pulse.PullData();
      // And write it out to the console
      data_mgr.WriteData(data_values);

      config.GetInspiratoryExpiratoryRatio().SetValue(1.0);
      config.GetPositiveEndExpiratoryPressure().SetValue(1.0, PressureUnit.cmH2O);
      config.GetRespiratoryRate().SetValue(18.0, FrequencyUnit.Per_min);
      config.GetPeakInspiratoryPressure().SetValue(10.0, PressureUnit.cmH2O);
      pulse.ProcessAction(AMConfig);
      Console.WriteLine("More Anesthesia Machine control manipulation. Increasing respiratory rate, reducing driving pressure and increasing the inspiratory-expiratory ratio.");

      pulse.AdvanceTime_s(60);

      Console.WriteLine("Patient breathing is being controlled by the machine.");
      data_values = pulse.PullData();
      // And write it out to the console
      data_mgr.WriteData(data_values);

      SEAnesthesiaMachineMaskLeak AMleak = new SEAnesthesiaMachineMaskLeak();
      AMleak.GetSeverity().SetValue(0.5);
      pulse.ProcessAction(AMleak);
      Console.WriteLine("Testing an anesthesia machine failure mode. The mask is leaking with a severity of 0.5.");

      pulse.AdvanceTime_s(60);

      Console.WriteLine("Patient breathing is being controlled by the machine. The mask has been leaking for 60 seconds.");
      data_values = pulse.PullData();
      // And write it out to the console
      data_mgr.WriteData(data_values);

      AMleak.GetSeverity().SetValue(0.0);
      pulse.ProcessAction(AMleak);
      Console.WriteLine("Removing the mask leak.");

      pulse.AdvanceTime_s(60);

      SEAnesthesiaMachineOxygenWallPortPressureLoss AMpressureloss = new SEAnesthesiaMachineOxygenWallPortPressureLoss();
      AMpressureloss.SetState(eSwitch.On);
      pulse.ProcessAction(AMpressureloss);
      Console.WriteLine("Testing the oxygen pressure loss failure mode. The oxygen pressure from the wall source is dropping.");

      pulse.AdvanceTime_s(60);

      Console.WriteLine("Patient breathing is being controlled by the machine. The wall oxygen pressure loss occurred 60 seconds ago.");
      data_values = pulse.PullData();
      // And write it out to the console
      data_mgr.WriteData(data_values);

      AMpressureloss.SetState(eSwitch.On);
      pulse.ProcessAction(AMpressureloss);
      Console.WriteLine("Removing the wall oxygen pressure loss action.");

      pulse.AdvanceTime_s(60);

      Console.WriteLine("The anesthesia machine is operating normally");
      data_values = pulse.PullData();
      // And write it out to the console
      data_mgr.WriteData(data_values);
      Console.WriteLine("Finished");
    }
  }
}
