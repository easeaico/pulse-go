/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using System;
using System.Collections.Generic;
using Pulse;
using Pulse.CDM;

namespace HowTo_VentilationMechanics
{
  class Example
  {
    public static void Run()
    {
      // Create our pulse engine
      PulseEngine pulse = new PulseEngine(eModelType.HumanAdultVentilationMechanics);
      pulse.LogToConsole(true);// Easily view what is happening

      List<SEDataRequest> data_requests = new List<SEDataRequest>
      {
        // Vitals Monitor Data
        SEDataRequest.CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit.Per_min),
        SEDataRequest.CreatePhysiologyDataRequest("TidalVolume", VolumeUnit.mL),
        SEDataRequest.CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit.mL),
        SEDataRequest.CreatePhysiologyDataRequest("InspiratoryExpiratoryRatio"),
        SEDataRequest.CreatePhysiologyDataRequest("MeanAirwayPressure", PressureUnit.cmH2O),
        SEDataRequest.CreatePhysiologyDataRequest("TransrespiratoryPressure", PressureUnit.cmH2O),
        SEDataRequest.CreatePhysiologyDataRequest("IntrapulmonaryPressure", PressureUnit.cmH2O),
        SEDataRequest.CreatePhysiologyDataRequest("InspiratoryFlow", VolumePerTimeUnit.L_Per_min),
        SEDataRequest.CreatePhysiologyDataRequest("ExpiratoryRespiratoryResistance", PressureTimePerVolumeUnit.cmH2O_s_Per_mL),
        SEDataRequest.CreatePhysiologyDataRequest("InspiratoryRespiratoryResistance", PressureTimePerVolumeUnit.cmH2O_s_Per_mL),
        SEDataRequest.CreatePhysiologyDataRequest("RespiratoryCompliance",VolumePerPressureUnit.mL_Per_cmH2O),
        SEDataRequest.CreatePhysiologyDataRequest("TotalPulmonaryVentilation", VolumePerTimeUnit.mL_Per_min),
        // Ventilator Monitor Data
        SEDataRequest.CreateMechanicalVentilatorDataRequest("AirwayPressure", PressureUnit.cmH2O),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("BreathState"),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("DynamicRespiratoryCompliance", VolumePerPressureUnit.mL_Per_cmH2O),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("EndTidalCarbonDioxideFraction"),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("EndTidalCarbonDioxidePressure", PressureUnit.cmH2O),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("EndTidalOxygenFraction"),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("EndTidalOxygenPressure", PressureUnit.cmH2O),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("ExpiratoryFlow", VolumePerTimeUnit.L_Per_s),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("ExpiratoryResistance", PressureTimePerVolumeUnit.cmH2O_s_Per_L),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("ExpiratoryTidalVolume", VolumeUnit.L),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("ExtrinsicPositiveEndExpiratoryPressure", PressureUnit.cmH2O),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("InspiratoryExpiratoryRatio"),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("InspiratoryFlow", VolumePerTimeUnit.L_Per_s),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("InspiratoryResistance", PressureTimePerVolumeUnit.cmH2O_s_Per_L),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("InspiratoryTidalVolume", VolumeUnit.L),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("IntrinsicPositiveEndExpiratoryPressure", PressureUnit.cmH2O),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("LeakFraction"),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("MeanAirwayPressure", PressureUnit.cmH2O),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("PeakInspiratoryFlow", VolumePerTimeUnit.L_Per_s),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("PeakInspiratoryPressure", PressureUnit.cmH2O),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("PlateauPressure", PressureUnit.cmH2O),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("RespirationRate", FrequencyUnit.Per_min),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("StaticRespiratoryCompliance", VolumePerPressureUnit.mL_Per_cmH2O),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("TidalVolume", VolumeUnit.L),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("TotalLungVolume", VolumeUnit.L),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("TotalPositiveEndExpiratoryPressure", PressureUnit.cmH2O),
        SEDataRequest.CreateMechanicalVentilatorDataRequest("TotalPulmonaryVentilation", VolumePerTimeUnit.L_Per_s),
      };
      SEDataRequestManager data_mgr = new SEDataRequestManager(data_requests);
      data_mgr.SetResultsFilename("./test_results/howto/HowTo_VentilationMechanics.cs.csv");
      // Create a reference to a double[] that will contain the data returned from Pulse
      double[] data_values;
      // data_values[0] is ALWAYS the simulation time in seconds
      // The rest of the data values are in order of the data_requests list provided

      // You only need to set a sex
      SEPatientConfiguration pCfg = new SEPatientConfiguration();
      pCfg.GetPatient().SetSex(ePatient_Sex.Male);

      // State serialization is not needed, this initializes instantaneously
      pulse.InitializeEngine(pCfg, data_mgr);

      // Since this engine is just the ventilation mechanics
      // You configure the patient respiratory via the RespiratoryMechanics structure
      SERespiratoryMechanicsConfiguration config = new SERespiratoryMechanicsConfiguration();
      SERespiratoryMechanics mechanics = config.GetSettings();
      mechanics.SetActive(eSwitch.On);
      // Create a mechanics profile
      double resistance_cmH2O_s_Per_L = 13.0;
      double compliance_mL_Per_cmH2O = 50.0;
      double musclePressure_cmH2O = 13.0;
      double respirationRate_bpm = 15.0;
      double inspiratoryTime_s = 0.9;

      // These need to add to 1.0 (100%)
      const double UpperResistanceFraction = 0.5;
      const double BronchiResistanceFraction = 0.3;
      const double AlveoliDuctResistanceFraction = 0.2;

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
      double InspiratoryReleaseFraction = Math.Min(inspiratoryFraction, expiratoryFraction * 0.5);
      double InspiratoryToExpiratoryPauseFraction = 1.0 - InspiratoryRiseFraction - InspiratoryReleaseFraction;
      double ExpiratoryRiseFraction = 0.0;
      double ExpiratoryHoldFraction = 0.0;
      double ExpiratoryReleaseFraction = 0.0;

      mechanics.GetUpperInspiratoryResistance().SetValue(upperResistance, PressureTimePerVolumeUnit.cmH2O_s_Per_L);
      mechanics.GetUpperExpiratoryResistance().SetValue(upperResistance, PressureTimePerVolumeUnit.cmH2O_s_Per_L);
      mechanics.GetLeftInspiratoryResistance().SetValue(sideResistance, PressureTimePerVolumeUnit.cmH2O_s_Per_L);
      mechanics.GetLeftExpiratoryResistance().SetValue(sideResistance, PressureTimePerVolumeUnit.cmH2O_s_Per_L);
      mechanics.GetRightInspiratoryResistance().SetValue(sideResistance, PressureTimePerVolumeUnit.cmH2O_s_Per_L);
      mechanics.GetRightExpiratoryResistance().SetValue(sideResistance, PressureTimePerVolumeUnit.cmH2O_s_Per_L);

      SECurve lcc = mechanics.GetLeftComplianceCurve();
      SESegmentConstant lc = lcc.AddConstantSegment();
      lc.GetBeginVolume().SetValue(Double.NegativeInfinity, VolumeUnit.mL);
      lc.GetEndVolume().SetValue(Double.PositiveInfinity, VolumeUnit.mL);
      lc.GetCompliance().SetValue(RespiratorySideCompliance_L_Per_cmH2O, VolumePerPressureUnit.mL_Per_cmH2O);
      SECurve rcc = mechanics.GetRightComplianceCurve();
      SESegmentConstant rc = rcc.AddConstantSegment();
      rc.GetBeginVolume().SetValue(Double.NegativeInfinity, VolumeUnit.mL);
      rc.GetEndVolume().SetValue(Double.PositiveInfinity, VolumeUnit.mL);
      rc.GetCompliance().SetValue(RespiratorySideCompliance_L_Per_cmH2O, VolumePerPressureUnit.mL_Per_cmH2O);

      mechanics.GetInspiratoryRiseTime().SetValue(InspiratoryRiseFraction * totalBreathTime_s, TimeUnit.s);
      mechanics.GetInspiratoryHoldTime().SetValue(InspiratoryHoldFraction * totalBreathTime_s, TimeUnit.s);
      mechanics.GetInspiratoryReleaseTime().SetValue(InspiratoryReleaseFraction * totalBreathTime_s, TimeUnit.s);
      mechanics.GetInspiratoryToExpiratoryPauseTime().SetValue(InspiratoryToExpiratoryPauseFraction * totalBreathTime_s, TimeUnit.s);
      mechanics.GetExpiratoryRiseTime().SetValue(ExpiratoryRiseFraction * totalBreathTime_s, TimeUnit.s);
      mechanics.GetExpiratoryHoldTime().SetValue(ExpiratoryHoldFraction * totalBreathTime_s, TimeUnit.s);
      mechanics.GetExpiratoryReleaseTime().SetValue(ExpiratoryReleaseFraction * totalBreathTime_s, TimeUnit.s);
      mechanics.GetInspiratoryPeakPressure().SetValue(-13.0, PressureUnit.cmH2O);
      mechanics.GetExpiratoryPeakPressure().SetValue(0.0, PressureUnit.cmH2O);
      pulse.ProcessAction(config);

      // Intubate
      SEIntubation intubation = new SEIntubation();
      intubation.SetType(eIntubation_Type.Tracheal);
      pulse.ProcessAction(intubation);

      // Setup the ventilator
      SEMechanicalVentilatorPressureControl pc_ac = new SEMechanicalVentilatorPressureControl();
      pc_ac.SetConnection(eSwitch.On);
      pc_ac.SetMode(eMechanicalVentilator_PressureControlMode.AssistedControl);
      pc_ac.GetFractionInspiredOxygen().SetValue(0.21);
      pc_ac.GetInspiratoryPeriod().SetValue(1.0, TimeUnit.s);
      pc_ac.GetInspiratoryPressure().SetValue(19.0, PressureUnit.cmH2O);
      pc_ac.GetPositiveEndExpiratoryPressure().SetValue(5.0, PressureUnit.cmH2O);
      pc_ac.GetRespirationRate().SetValue(12.0, FrequencyUnit.Per_min);
      pc_ac.GetSlope().SetValue(0.2, TimeUnit.s);
      pulse.ProcessAction(pc_ac);

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
