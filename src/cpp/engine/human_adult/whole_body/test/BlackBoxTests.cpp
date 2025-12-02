/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/


#include "engine/human_adult/whole_body/test/EngineTest.h"
#include "engine/PulseConfiguration.h"

#include "cdm/blackbox/SEBlackBoxManager.h"
#include "cdm/blackbox/fluid/SELiquidBlackBox.h"
#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/engine/SEDataRequestTracker.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/utils/DataTrack.h"
#include "cdm/utils/FileUtils.h"
#include "cdm/utils/TimingProfile.h"

//#define VERBOSE

namespace pulse { namespace human_adult_whole_body
{
  struct BlackBoxes
  {
    enum class locations { AORTA = 0, VENACAVA = 1, BOTH = 2 };
    BlackBoxes(locations l = locations::AORTA) { at = l; }

    locations at;
    SELiquidBlackBox* aortaToRightLeg = nullptr;
    SELiquidBlackBox* rightLegToVenaCava = nullptr;
  };
  bool SetupBBDataRequests(BlackBoxes& bbz, SEDataRequestManager& drMgr, const std::string& csvFilename)
  {
    drMgr.CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
    drMgr.CreatePhysiologyDataRequest("BloodVolume", VolumeUnit::mL);
    drMgr.CreatePhysiologyDataRequest("CardiacOutput", VolumePerTimeUnit::mL_Per_min);
    drMgr.CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
    drMgr.CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
    drMgr.CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);

    drMgr.CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::Aorta, "Pressure", PressureUnit::mmHg);
    drMgr.CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::Aorta, "InFlow", VolumePerTimeUnit::mL_Per_s);
    drMgr.CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::Aorta, "OutFlow", VolumePerTimeUnit::mL_Per_s);
    drMgr.CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::Aorta, "Volume", VolumeUnit::mL);

    drMgr.CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::RightLeg, "Pressure", PressureUnit::mmHg);
    drMgr.CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::RightLeg, "InFlow", VolumePerTimeUnit::mL_Per_s);
    drMgr.CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::RightLeg, "OutFlow", VolumePerTimeUnit::mL_Per_s);
    drMgr.CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::RightLeg, "Volume", VolumeUnit::mL);

    drMgr.CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::VenaCava, "Pressure", PressureUnit::mmHg);
    drMgr.CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::VenaCava, "InFlow", VolumePerTimeUnit::mL_Per_s);
    drMgr.CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::VenaCava, "OutFlow", VolumePerTimeUnit::mL_Per_s);
    drMgr.CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::VenaCava, "Volume", VolumeUnit::mL);

    if (bbz.at == BlackBoxes::locations::AORTA || bbz.at == BlackBoxes::locations::BOTH)
    {
      std::string bbCmpt = SEBlackBoxManager::GetBlackBoxName(pulse::VascularCompartment::Aorta, pulse::VascularCompartment::RightLeg);
      drMgr.CreateLiquidCompartmentDataRequest(bbCmpt, "Pressure", PressureUnit::mmHg);
      drMgr.CreateLiquidCompartmentDataRequest(bbCmpt, "InFlow", VolumePerTimeUnit::mL_Per_s);
      drMgr.CreateLiquidCompartmentDataRequest(bbCmpt, "OutFlow", VolumePerTimeUnit::mL_Per_s);
      drMgr.CreateLiquidCompartmentDataRequest(bbCmpt, "Volume", VolumeUnit::mL);
    }
    if (bbz.at == BlackBoxes::locations::VENACAVA || bbz.at == BlackBoxes::locations::BOTH)
    {
      std::string bbCmpt = SEBlackBoxManager::GetBlackBoxName(pulse::VascularCompartment::RightLeg, pulse::VascularCompartment::VenaCava);
      drMgr.CreateLiquidCompartmentDataRequest(bbCmpt, "Pressure", PressureUnit::mmHg);
      drMgr.CreateLiquidCompartmentDataRequest(bbCmpt, "InFlow", VolumePerTimeUnit::mL_Per_s);
      drMgr.CreateLiquidCompartmentDataRequest(bbCmpt, "OutFlow", VolumePerTimeUnit::mL_Per_s);
      drMgr.CreateLiquidCompartmentDataRequest(bbCmpt, "Volume", VolumeUnit::mL);
    }

    drMgr.SetResultsFilename(csvFilename);

    return true;
  }

  void EngineTest::EmptyBlackBoxTest(const std::string& outputDir)
  {
    std::unique_ptr<PhysiologyEngine> pulse = CreatePulseEngine(eModelType::HumanAdultWholeBody, m_Logger);
    Info("--------EmptyBlackBoxTest--------");

    BlackBoxes bbz;
    SEDataRequestManager drMgr(pulse->GetLogger());
    if (!SetupBBDataRequests(bbz, drMgr, outputDir + "/EmptyBlackBoxTest.csv"))
    {
      pulse->GetLogger()->Error("Could not create black boxes");
      Error("Could not create black boxes");
      return;
    }

    if (!pulse->SerializeFromFile("./states/StandardMale@0s.json", &drMgr))
    {
      pulse->GetLogger()->Error("Could not load state, check the error");
      Error("Could not load state, check the error");
      return;
    }

    // Run for two mins
    TimingProfile profile;
    profile.Start("eBB");
    for (size_t i=0; i<120/pulse->GetTimeStep(TimeUnit::s); i++)
    {
      if (!pulse->AdvanceModelTime())
      {
        Error("Unable to advance time");
        return;
      }
      if (i == 3000)
        Info("It took " + std::to_string(profile.GetElapsedTime_s("eBB")) + "(s) to simulate 60s");
    }
    Info("It took " + std::to_string(profile.GetElapsedTime_s("eBB")) + "(s) to simulate 60s");
    profile.Stop("eBB");
  }

  void EngineTest::ImposeFlowBlackBoxTest(const std::string& outputDir)
  {
    std::unique_ptr<PhysiologyEngine> pulse = CreatePulseEngine(eModelType::HumanAdultWholeBody, m_Logger);
    Info("--------ImposeFlowBlackBoxTest--------");

    PulseConfiguration config;
    config.AllowDynamicTimeStep(eSwitch::On);
    pulse->SetConfigurationOverride(&config);
    // Not provided a drMgr since we are dynamically making new cmpts and requesting data from them
    if (!pulse->SerializeFromFile("./states/StandardMale@0s.json"))
    {
      pulse->GetLogger()->Error("Could not load state, check the error");
      Error("Could not load state, check the error");
      return;
    }

    BlackBoxes bbz(BlackBoxes::locations::VENACAVA);
    SEDataRequestManager drMgr(pulse->GetLogger());
    if(!SetupBBDataRequests(bbz, drMgr, outputDir+"/ImposeFlowBlackBoxTest.csv"))
    {
      pulse->GetLogger()->Error("Could not create black boxes");
      Error("Could not create black boxes");
      return;
    }

    bbz.rightLegToVenaCava = pulse->GetBlackBoxes().GetLiquidBlackBox(pulse::VascularCompartment::RightLeg, pulse::VascularCompartment::VenaCava);
    if (bbz.rightLegToVenaCava == nullptr)
    {
      pulse->GetLogger()->Error("Unable to create VENACAVA blackbox");
      Error("Unable to create VENACAVA blackbox");
      return;
    }
    bbz.rightLegToVenaCava->GetCompartment().GetVolume().SetValue(10, VolumeUnit::mL);

    // Close csv file so it gets remade with all the new headers
    pulse->GetDataRequestTracker().CloseResultsFile();
    if (!pulse->GetDataRequestTracker().SetupDataRequests(drMgr))
    {
      pulse->GetLogger()->Error("Could not setup data requests");
      Error("Could not setup data requests");
      return;
    }

    double aortaToRightLegInflow = 2.0;
    double aortaToRightLegOutflow = 1.5;

    double rightLegToVenaCavaInflow = -1.5;
    double rightLegToVenaCavaOutflow = 1.5;

    // Run for two mins
    TimingProfile profile;
    profile.Start("BB");
    for (size_t i = 0; i < 120 / pulse->GetTimeStep(TimeUnit::s); i++)
    {
      if (bbz.aortaToRightLeg != nullptr)
      {
#ifdef VERBOSE
        Info("Current Aorta/Right Leg Source Node Potential : " + std::to_string(bbz.aortaToRightLeg->GetSourcePotential(PressureUnit::mmHg)) + " mmHg");
        Info("Current Aorta/Right Leg Source Compartment Potential : " + std::to_string(bbz.aortaToRightLeg->GetSourceCompartment().GetPressure(PressureUnit::mmHg)) + " mmHg");
        Info("Current Aorta/Right Leg Source Path Flux : " + std::to_string(bbz.aortaToRightLeg->GetSourceFlux(VolumePerTimeUnit::mL_Per_s)) + " mL/s");
        Info("Imposing " + bbz.aortaToRightLeg->GetName() + " source flow, to " + std::to_string(aortaToRightLegInflow) + " mL/s");
#endif
        bbz.aortaToRightLeg->ImposeSourceFlux(aortaToRightLegInflow, VolumePerTimeUnit::mL_Per_s);
#ifdef VERBOSE
        Info("New Aorta/Right Leg Source Path Flux : " + std::to_string(bbz.aortaToRightLeg->GetSourceFlux(VolumePerTimeUnit::mL_Per_s)) + " mL/s\n");

        Info("Current Aorta/Right Leg Target Node Potential : " + std::to_string(bbz.aortaToRightLeg->GetTargetPotential(PressureUnit::mmHg)) + " mmHg");
        Info("Current Aorta/Right Leg Target Compartment Potential : " + std::to_string(bbz.aortaToRightLeg->GetTargetCompartment().GetPressure(PressureUnit::mmHg)) + " mmHg");
        Info("Current Aorta/Right Leg Target Path Flux : " + std::to_string(bbz.aortaToRightLeg->GetTargetFlux(VolumePerTimeUnit::mL_Per_s)) + " mL/s");
        Info("Imposing " + bbz.aortaToRightLeg->GetName() + " target flow, to " + std::to_string(aortaToRightLegOutflow) + " mL/s");
#endif
        bbz.aortaToRightLeg->ImposeTargetFlux(aortaToRightLegOutflow, VolumePerTimeUnit::mL_Per_s);
#ifdef VERBOSE
        Info("New Aorta/Right Leg Target Path Flux : " + std::to_string(bbz.aortaToRightLeg->GetTargetFlux(VolumePerTimeUnit::mL_Per_s)) + " mL/s\n");
#endif
      }

      if (bbz.rightLegToVenaCava != nullptr)
      {
#ifdef VERBOSE
        Info("Current Right Leg/Vena Cava Source Node Potential : " + std::to_string(bbz.rightLegToVenaCava->GetSourcePotential(PressureUnit::mmHg)) + " mmHg");
        Info("Current Right Leg/Vena Cava Source Compartment Potential : " + std::to_string(bbz.rightLegToVenaCava->GetSourceCompartment().GetPressure(PressureUnit::mmHg)) + " mmHg");
        Info("Current Right Leg/Vena Cava Source Path Flux : " + std::to_string(bbz.rightLegToVenaCava->GetSourceFlux(VolumePerTimeUnit::mL_Per_s)) + " mL/s");
        Info("Imposing " + bbz.rightLegToVenaCava->GetName() + " source flow, to " + std::to_string(rightLegToVenaCavaInflow) + " mL/s");
#endif
        bbz.rightLegToVenaCava->ImposeSourceFlux(rightLegToVenaCavaInflow, VolumePerTimeUnit::mL_Per_s);
#ifdef VERBOSE
        Info("New Vena Right Leg/Cava Source Flux : " + std::to_string(bbz.rightLegToVenaCava->GetSourceFlux(VolumePerTimeUnit::mL_Per_s)) + " mL/s\n");

        Info("Current Right Leg/Vena Cava Target Node Potential : " + std::to_string(bbz.rightLegToVenaCava->GetTargetPotential(PressureUnit::mmHg)) + " mmHg");
        Info("Current Right Leg/Vena Cava Target Compartment Potential : " + std::to_string(bbz.rightLegToVenaCava->GetTargetCompartment().GetPressure(PressureUnit::mmHg)) + " mmHg");
        Info("Current Right Leg/Vena Cava Target Path Flux : " + std::to_string(bbz.rightLegToVenaCava->GetTargetFlux(VolumePerTimeUnit::mL_Per_s)) + " mL/s");
        Info("Imposing " + bbz.rightLegToVenaCava->GetName() + " target flow, to " + std::to_string(rightLegToVenaCavaOutflow) + " mL/s");
#endif
        bbz.rightLegToVenaCava->ImposeTargetFlux(rightLegToVenaCavaOutflow, VolumePerTimeUnit::mL_Per_s);
#ifdef VERBOSE
        Info("New Right Leg/Vena Cava Target Path Flux : " + std::to_string(bbz.rightLegToVenaCava->GetTargetFlux(VolumePerTimeUnit::mL_Per_s)) + " mL/s\n");
#endif
      }

      if (!pulse->AdvanceModelTime(0.02, TimeUnit::s))
      {
        Error("Unable to advance time");
        return;
      }
#ifdef VERBOSE
      Info("--------------- Advance Time ---------------\n");
#endif

      if (i == 3000)
        Info("It took " + std::to_string(profile.GetElapsedTime_s("BB")) + "(s) to simulate 60s");
    }
    Info("It took " + std::to_string(profile.GetElapsedTime_s("BB")) + "(s) to simulate 60s");
    profile.Stop("BB");
  }

  void EngineTest::ImposePressureAndFlowBlackBoxTest(const std::string& outputDir)
  {
    std::unique_ptr<PhysiologyEngine> pulse = CreatePulseEngine(eModelType::HumanAdultWholeBody, m_Logger);
    Info("--------ImposePressureAndFlowBlackBoxTest--------");

    // Not provided a drMgr since we are dynamically making new cmpts and requesting data from them
    if (!pulse->SerializeFromFile("./states/StandardMale@0s.json"))
    {
      pulse->GetLogger()->Error("Could not load state, check the error");
      Error("Could not load state, check the error");
      return;
    }

    BlackBoxes bbz(BlackBoxes::locations::AORTA);
    SEDataRequestManager drMgr(pulse->GetLogger());
    if (!SetupBBDataRequests(bbz, drMgr, outputDir + "/ImposePressureAndFlowBlackBoxTest.csv"))
    {
      pulse->GetLogger()->Error("Could not create black boxes");
      Error("Could not create black boxes");
      return;
    }

    bbz.aortaToRightLeg = pulse->GetBlackBoxes().GetLiquidBlackBox(pulse::VascularCompartment::Aorta, pulse::VascularCompartment::RightLeg);
    if (bbz.aortaToRightLeg == nullptr)
    {
      pulse->GetLogger()->Error("Unable to create AORTA blackbox");
      Error("Unable to create AORTA blackbox");
      return;
    }
    bbz.aortaToRightLeg->GetCompartment().GetVolume().SetValue(10, VolumeUnit::mL);

    // Close csv file so it gets remade with all the new headers
    pulse->GetDataRequestTracker().CloseResultsFile();
    if (!pulse->GetDataRequestTracker().SetupDataRequests(drMgr))
    {
      pulse->GetLogger()->Error("Could not setup data requests");
      Error("Could not setup data requests");
      return;
    }

    double resistance_mmHg_s_Per_mL = 0.1;
    double dampenFraction = 0.01;

    // Run for two mins
    TimingProfile profile;
    profile.Start("BB");
    for (size_t i = 0; i < 120 / pulse->GetTimeStep(TimeUnit::s); i++)
    {
      // Calculate a change due to a small resistance
      //It will only change half as much as it wants to each time step to ensure it's critically damped and doesn't overshoot

      double inletFlow_mL_Per_s = bbz.aortaToRightLeg->GetSourceFlux(VolumePerTimeUnit::mL_Per_s);
      double outletPressure_mmHg = bbz.aortaToRightLeg->GetTargetPotential(PressureUnit::mmHg);

      double inletPressure_mmHg = bbz.aortaToRightLeg->GetSourcePotential(PressureUnit::mmHg);
      double outletFlow_mL_Per_s = bbz.aortaToRightLeg->GetTargetFlux(VolumePerTimeUnit::mL_Per_s);

      double nextInletPressure_mmhg = inletFlow_mL_Per_s * resistance_mmHg_s_Per_mL + outletPressure_mmHg;
      double next_OutletFlow_mL_Per_s = (nextInletPressure_mmhg - outletPressure_mmHg) / resistance_mmHg_s_Per_mL;

      double inletPressureChange_mmhg = (nextInletPressure_mmhg - inletPressure_mmHg) * dampenFraction;
      double outletFlowChange_mL_Per_s = (next_OutletFlow_mL_Per_s - outletFlow_mL_Per_s) * dampenFraction;

      bbz.aortaToRightLeg->ImposeSourcePotential(inletPressure_mmHg + inletPressureChange_mmhg, PressureUnit::mmHg);
      bbz.aortaToRightLeg->ImposeTargetFlux(outletFlow_mL_Per_s + outletFlowChange_mL_Per_s, VolumePerTimeUnit::mL_Per_s);

      if (!pulse->AdvanceModelTime())
      {
        Error("Unable to advance time");
        return;
      }
      if (i == 3000)
        Info("It took " + std::to_string(profile.GetElapsedTime_s("BB")) + "(s) to simulate 60s");
    }
    Info("It took " + std::to_string(profile.GetElapsedTime_s("BB")) + "(s) to simulate 60s");
    profile.Stop("BB");
  }
END_NAMESPACE_EX
