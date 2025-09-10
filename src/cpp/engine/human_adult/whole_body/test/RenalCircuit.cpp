/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/human_adult/whole_body/test/EngineTest.h"
#include "engine/human_adult/whole_body/Engine.h"
#include "engine/human_adult/whole_body/controller/Controller.h"
#include "engine/human_adult/whole_body/controller/CompartmentManager.h"
#include "engine/human_adult/whole_body/controller/SubstanceManager.h"
#include "engine/human_adult/whole_body/system/physiology/Saturation.h"
#include "engine/human_adult/whole_body/system/physiology/RenalModel.h"

#include "cdm/patient/SEPatient.h"
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/circuit/fluid/SEFluidCircuit.h"
#include "cdm/circuit/fluid/SEFluidCircuitCalculator.h"
#include "cdm/compartment/fluid/SELiquidCompartment.h"
#include "cdm/compartment/fluid/SELiquidCompartmentGraph.h"
#include "cdm/compartment/substances/SELiquidSubstanceQuantity.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceFraction.h"
#include "cdm/substance/SESubstanceClearance.h"
#include "cdm/engine/SEActionManager.h"
#include "cdm/engine/SEEventManager.h"
#include "cdm/engine/SEPatientActionCollection.h"
#include "cdm/patient/actions/SEUrinate.h"
#include "cdm/properties/SEScalarPressureTimePerVolume.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerTime.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarAmountPerVolume.h"
#include "cdm/properties/SEScalarVolumePerTimePressureArea.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/utils/TimingProfile.h"
#include "cdm/utils/DataTrack.h"

namespace pulse { namespace human_adult_whole_body
{
  void EngineTest::RenalCircuitAndTransportTest(const std::string& sTestDirectory)
  {
    TimingProfile tmr;
    tmr.Start("Test");
    //Output files
    DataTrack     circiutTrk;
    std::ofstream circuitFile;
    DataTrack     graphTrk;
    std::ofstream graphFile;

    Engine pe;
    Controller& pc = (Controller&)pe.GetController();
    SubstanceManager& subMgr = (SubstanceManager&)pc.GetSubstances();
    pc.GetLogger()->SetLogFile(sTestDirectory + "/RenalCircuitAndTransportTest.log");
    SEPatient patient(pc.GetLogger());
    patient.SerializeFromFile("./patients/StandardMale.json");
    pc.SetupPatient(patient);
    subMgr.LoadSubstanceDirectory("./");
    pc.GetSaturationCalculator().Setup();
    pc.m_Config->Initialize("./", &subMgr);
    pc.m_Config->UseExpandedKidneys(eSwitch::On);
    pc.m_Config->EnableTissue(eSwitch::Off);
    pc.CreateCircuitsAndCompartments();
    // Renal needs these tissue compartments
    // Let's make them manually, without the tissue circuit
    pc.GetCompartments().CreateTissueCompartment(pulse::TissueCompartment::LeftKidney);
    pc.GetCompartments().CreateTissueCompartment(pulse::TissueCompartment::RightKidney);
    pc.GetCompartments().CreateLiquidCompartment(pulse::ExtravascularCompartment::LeftKidneyExtracellular);
    pc.GetCompartments().CreateLiquidCompartment(pulse::ExtravascularCompartment::RightKidneyExtracellular);
    pc.GetCompartments().CreateLiquidCompartment(pulse::ExtravascularCompartment::LeftKidneyIntracellular);
    pc.GetCompartments().CreateLiquidCompartment(pulse::ExtravascularCompartment::RightKidneyIntracellular);
    pc.m_Config->EnableTissue(eSwitch::On);// This needs to be on for making the tissue to extravascular mapping
    pc.GetCompartments().StateChange();
    //Add just N2
    subMgr.AddActiveSubstance(subMgr.GetN2());
    SEScalarMassPerVolume N2_ug_per_mL;
    N2_ug_per_mL.SetValue(0.5, MassPerVolumeUnit::ug_Per_mL);
    subMgr.SetSubstanceConcentration(subMgr.GetN2(), pc.GetCompartments().GetUrineLeafCompartments(), N2_ug_per_mL);
    subMgr.SetSubstanceConcentration(subMgr.GetN2(), pc.GetCompartments().GetVascularLeafCompartments(), N2_ug_per_mL);

    //Get the renal stuff
    SEFluidCircuit& rCircuit = pc.GetCircuits().GetRenalCircuit();
    SELiquidCompartmentGraph& rGraph = pc.GetCompartments().GetRenalGraph();

    SEFluidCircuitNode* Ground = rCircuit.GetNode(pulse::RenalNode::Ground);

    SEFluidCircuitNode* LeftRenalArtery = rCircuit.GetNode(pulse::RenalNode::LeftRenalArtery);
    SEFluidCircuitNode* RightRenalArtery = rCircuit.GetNode(pulse::RenalNode::RightRenalArtery);
    SEFluidCircuitNode* LeftRenalVein = rCircuit.GetNode(pulse::RenalNode::LeftRenalVein);
    SEFluidCircuitNode* RightRenalVein = rCircuit.GetNode(pulse::RenalNode::RightRenalVein);

    SELiquidCompartment* cLeftRenalArtery = pc.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftRenalArtery);
    SELiquidCompartment* cRightRenalArtery = pc.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightRenalArtery);

    //Set up the N2 source to keep a constant concentrations to supply the system
    LeftRenalArtery->GetVolumeBaseline().SetValue(std::numeric_limits<double>::infinity(), VolumeUnit::mL);
    LeftRenalArtery->GetNextVolume().SetValue(std::numeric_limits<double>::infinity(), VolumeUnit::mL);
    cLeftRenalArtery->GetVolume().SetValue(std::numeric_limits<double>::infinity(), VolumeUnit::mL);
    cLeftRenalArtery->GetSubstanceQuantity(subMgr.GetN2())->GetMass().SetValue(std::numeric_limits<double>::infinity(), MassUnit::mg);
    cLeftRenalArtery->Balance(BalanceLiquidBy::Concentration);

    RightRenalArtery->GetVolumeBaseline().SetValue(std::numeric_limits<double>::infinity(), VolumeUnit::mL);
    RightRenalArtery->GetNextVolume().SetValue(std::numeric_limits<double>::infinity(), VolumeUnit::mL);
    cRightRenalArtery->GetVolume().SetValue(std::numeric_limits<double>::infinity(), VolumeUnit::mL);
    cRightRenalArtery->GetSubstanceQuantity(subMgr.GetN2())->GetMass().SetValue(std::numeric_limits<double>::infinity(), MassUnit::mg);
    cRightRenalArtery->Balance(BalanceLiquidBy::Concentration);

    SEFluidCircuitPath& RightAortaSourcePath = rCircuit.CreatePath(*Ground, *RightRenalArtery, "RightAortaSource");
    SEFluidCircuitPath& LeftAortaSourcePath = rCircuit.CreatePath(*Ground, *LeftRenalArtery, "LeftAortaSource");
    SEFluidCircuitPath& RightVenaCavaSourcePath = rCircuit.CreatePath(*Ground, *RightRenalVein, "RightVenaCavaSource");
    SEFluidCircuitPath& LeftVenaCavaSourcePath = rCircuit.CreatePath(*Ground, *LeftRenalVein, "LeftVenaCavaSource");

    //set baseline values, if it's anything other than 0, the sinusoid will not be used and the circuit will just be driven with this static pressure, for debugging or something
    double aortaPressure_mmHg = 0.0;
    RightAortaSourcePath.GetPressureSourceBaseline().SetValue(aortaPressure_mmHg, PressureUnit::mmHg);
    LeftAortaSourcePath.GetPressureSourceBaseline().SetValue(aortaPressure_mmHg, PressureUnit::mmHg);
    RightAortaSourcePath.GetNextPressureSource().SetValue(aortaPressure_mmHg, PressureUnit::mmHg);
    LeftAortaSourcePath.GetNextPressureSource().SetValue(aortaPressure_mmHg, PressureUnit::mmHg);

    RightVenaCavaSourcePath.GetPressureSourceBaseline().SetValue(4.0, PressureUnit::mmHg);// Set to give cv equivalent pressure
    LeftVenaCavaSourcePath.GetPressureSourceBaseline().SetValue(4.0, PressureUnit::mmHg);// Set to give cv equivalent pressure
    RightVenaCavaSourcePath.GetNextPressureSource().SetValue(4.0, PressureUnit::mmHg);// Set to give cv equivalent pressure
    LeftVenaCavaSourcePath.GetNextPressureSource().SetValue(4.0, PressureUnit::mmHg);// Set to give cv equivalent pressure

    rCircuit.SetNextAndCurrentFromBaselines();
    rCircuit.StateChange();

    //Execution parameters
    double time_s = 0;
    double deltaT_s = 1.0 / 90.0;
    double runTime_min = 10.0;
    //Drive waveform parameters
    double period = 1.0;
    double alpha = (2 * M_PI) / (period);
    double driverPressure_mmHg = 0.0;
    double amplitude_cmH2O = 6.0;
    double yOffset = 75.0;

    SELiquidTransporter txpt(VolumePerTimeUnit::mL_Per_s, VolumeUnit::mL, MassUnit::ug, MassPerVolumeUnit::ug_Per_mL, pc.GetLogger());
    SEFluidCircuitCalculator calc(VolumePerPressureUnit::mL_Per_mmHg, VolumePerTimeUnit::mL_Per_s, PressureTimeSquaredPerVolumeUnit::mmHg_s2_Per_mL, PressureUnit::mmHg, VolumeUnit::mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL, pc.GetLogger());
    for (unsigned int i = 0; i < runTime_min * 60.0 / deltaT_s; i++)
    {
      // Drive the circuit
      driverPressure_mmHg = yOffset + amplitude_cmH2O * sin(alpha * time_s);   //compute new pressure
      RightAortaSourcePath.GetNextPressureSource().SetValue(driverPressure_mmHg, PressureUnit::mmHg);
      LeftAortaSourcePath.GetNextPressureSource().SetValue(driverPressure_mmHg, PressureUnit::mmHg);

      //Process - Execute the circuit
      calc.Process(rCircuit, deltaT_s);
      //There is a compliance on the renal arteries, so the infinite volume is getting overwritten
      //Just keep it infinite
      LeftRenalArtery->GetNextVolume().SetValue(std::numeric_limits<double>::infinity(), VolumeUnit::mL);
      RightRenalArtery->GetNextVolume().SetValue(std::numeric_limits<double>::infinity(), VolumeUnit::mL);
      //Execute the substance transport function
      txpt.Transport(rGraph, deltaT_s);
      //convert 'Next' values to current
      calc.PostProcess(rCircuit);

      circiutTrk.Track(time_s, rCircuit);
      graphTrk.Track(time_s, rGraph);
      time_s += deltaT_s;

      if (i == 0)
      {
        graphTrk.CreateFile(std::string(sTestDirectory + "/RenalTransportOutput.csv").c_str(), graphFile);
        circiutTrk.CreateFile(std::string(sTestDirectory + "/RenalCircuitOutput.csv").c_str(), circuitFile);
      }
      graphTrk.StreamTrackToFile(graphFile);
      circiutTrk.StreamTrackToFile(circuitFile);
    }
    graphFile.close();
    circuitFile.close();
    std::stringstream ss;
    ss << "It took " << tmr.GetElapsedTime_s("Test") << "s to run";
    pc.GetLogger()->Info(ss.str());
  }

  // runs renal system at constant MAP to test TGF feedback function
  // Resistance values at 80 & 180 mmHg are used for the resistance bounds in Pulse Circuits
  void EngineTest::RenalFeedbackTest(RenalFeedback feedback, const std::string& sTestDirectory, const std::string& sTestName)
  {
    TimingProfile tmr;
    tmr.Start("Test");
    Engine pe;
    Controller& pc = (Controller&)pe.GetController();
    SubstanceManager& subMgr = (SubstanceManager&)pc.GetSubstances();
    pc.GetLogger()->SetLogFile(sTestDirectory + "/RenalFeedbackTest.log");
    SEPatient patient(pc.GetLogger());
    patient.SerializeFromFile("./patients/StandardMale.json");
    pc.SetupPatient(patient);
    subMgr.LoadSubstanceDirectory("./");
    pc.GetSaturationCalculator().Setup();
    pc.m_Config->Initialize("./", &subMgr);
    pc.m_Config->UseExpandedKidneys(eSwitch::On);
    pc.m_Config->EnableTissue(eSwitch::Off);
    pc.CreateCircuitsAndCompartments();
    // Renal needs these tissue compartments
    // Let's make them manually, without the tissue circuit
    pc.GetCompartments().CreateTissueCompartment(pulse::TissueCompartment::LeftKidney);
    pc.GetCompartments().CreateTissueCompartment(pulse::TissueCompartment::RightKidney);
    pc.GetCompartments().CreateLiquidCompartment(pulse::ExtravascularCompartment::LeftKidneyExtracellular);
    pc.GetCompartments().CreateLiquidCompartment(pulse::ExtravascularCompartment::RightKidneyExtracellular);
    pc.GetCompartments().CreateLiquidCompartment(pulse::ExtravascularCompartment::LeftKidneyIntracellular);
    pc.GetCompartments().CreateLiquidCompartment(pulse::ExtravascularCompartment::RightKidneyIntracellular);
    pc.m_Config->EnableTissue(eSwitch::On);// This needs to be on for making the tissue to extravascular mapping
    pc.GetCompartments().StateChange();

    SEFluidCircuit& rCircuit = pc.GetCircuits().GetRenalCircuit();
    SELiquidCompartmentGraph& rGraph = pc.GetCompartments().GetRenalGraph();

    // Renal needs these present for Gluconeogenesis
    subMgr.AddActiveSubstance(subMgr.GetLactate());
    subMgr.AddActiveSubstance(subMgr.GetGlucose());
    subMgr.AddActiveSubstance(subMgr.GetPotassium());
    subMgr.AddActiveSubstance(subMgr.GetUrea());
    subMgr.AddActiveSubstance(subMgr.GetSodium());

    SEFluidCircuitNode* Ground = rCircuit.GetNode(pulse::RenalNode::Ground);
    SEFluidCircuitNode* LeftRenalArtery = rCircuit.GetNode(pulse::RenalNode::LeftRenalArtery);
    SEFluidCircuitNode* RightRenalArtery = rCircuit.GetNode(pulse::RenalNode::RightRenalArtery);
    SEFluidCircuitNode* LeftRenalVein = rCircuit.GetNode(pulse::RenalNode::LeftRenalVein);
    SEFluidCircuitNode* RightRenalVein = rCircuit.GetNode(pulse::RenalNode::RightRenalVein);

    SELiquidCompartment* cLeftRenalArtery = pc.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftRenalArtery);
    SELiquidCompartment* cRightRenalArtery = pc.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightRenalArtery);

    subMgr.GetSodium().GetBloodConcentration().SetValue(pc.GetConfiguration().GetPlasmaSodiumConcentrationSetPoint(MassPerVolumeUnit::g_Per_L), MassPerVolumeUnit::g_Per_L);
    subMgr.SetSubstanceConcentration(subMgr.GetSodium(), pc.GetCompartments().GetUrineLeafCompartments(), subMgr.GetSodium().GetBloodConcentration());
    subMgr.SetSubstanceConcentration(subMgr.GetSodium(), pc.GetCompartments().GetVascularLeafCompartments(), subMgr.GetSodium().GetBloodConcentration());

    //Set up the sodium concentration on the source to keep a constant concentrations to supply the system
    LeftRenalArtery->GetVolumeBaseline().SetValue(std::numeric_limits<double>::infinity(), VolumeUnit::mL);
    LeftRenalArtery->GetNextVolume().SetValue(std::numeric_limits<double>::infinity(), VolumeUnit::mL);
    cLeftRenalArtery->GetVolume().SetValue(std::numeric_limits<double>::infinity(), VolumeUnit::mL);
    cLeftRenalArtery->GetSubstanceQuantity(subMgr.GetSodium())->GetConcentration().SetValue(4.5, MassPerVolumeUnit::g_Per_L); //tubules sodium concentration in engine
    cLeftRenalArtery->Balance(BalanceLiquidBy::Concentration);

    RightRenalArtery->GetVolumeBaseline().SetValue(std::numeric_limits<double>::infinity(), VolumeUnit::mL);
    RightRenalArtery->GetNextVolume().SetValue(std::numeric_limits<double>::infinity(), VolumeUnit::mL);
    cRightRenalArtery->GetVolume().SetValue(std::numeric_limits<double>::infinity(), VolumeUnit::mL);
    cRightRenalArtery->GetSubstanceQuantity(subMgr.GetSodium())->GetConcentration().SetValue(4.5, MassPerVolumeUnit::g_Per_L); //tubules sodium concentration in engine
    cRightRenalArtery->Balance(BalanceLiquidBy::Concentration);

    SEFluidCircuitPath& RightAortaSourcePath = rCircuit.CreatePath(*Ground, *RightRenalArtery, "RightAortaSource");
    SEFluidCircuitPath& LeftAortaSourcePath = rCircuit.CreatePath(*Ground, *LeftRenalArtery, "LeftAortaSource");
    SEFluidCircuitPath& RightVenaCavaSourcePath = rCircuit.CreatePath(*Ground, *RightRenalVein, "RightVenaCavaSource");
    SEFluidCircuitPath& LeftVenaCavaSourcePath = rCircuit.CreatePath(*Ground, *LeftRenalVein, "LeftVenaCavaSource");

    SEFluidCircuitPath* LeftAfferentArterioleToGlomerularCapillaries = rCircuit.GetPath(pulse::RenalPath::LeftAfferentArterioleToGlomerularCapillaries);
    SEFluidCircuitPath* LeftGlomerularCapillariesToNetGlomerularCapillaries = rCircuit.GetPath(pulse::RenalPath::LeftGlomerularCapillariesToNetGlomerularCapillaries);
    SEFluidCircuitPath* LeftBowmansCapsulesToNetBowmansCapsules = rCircuit.GetPath(pulse::RenalPath::LeftBowmansCapsulesToNetBowmansCapsules);
    SEFluidCircuitPath* LeftTubulesToNetTubules = rCircuit.GetPath(pulse::RenalPath::LeftTubulesToNetTubules);
    SEFluidCircuitPath* LeftNetTubulesToNetPeritubularCapillaries = rCircuit.GetPath(pulse::RenalPath::LeftNetTubulesToNetPeritubularCapillaries);
    SEFluidCircuitPath* LeftPeritubularCapillariesToNetPeritubularCapillaries = rCircuit.GetPath(pulse::RenalPath::LeftPeritubularCapillariesToNetPeritubularCapillaries);

    SEFluidCircuitPath* RightAfferentArterioleToGlomerularCapillaries = rCircuit.GetPath(pulse::RenalPath::RightAfferentArterioleToGlomerularCapillaries);
    SEFluidCircuitPath* RightGlomerularCapillariesToNetGlomerularCapillaries = rCircuit.GetPath(pulse::RenalPath::RightGlomerularCapillariesToNetGlomerularCapillaries);
    SEFluidCircuitPath* RightBowmansCapsulesToNetBowmansCapsules = rCircuit.GetPath(pulse::RenalPath::RightBowmansCapsulesToNetBowmansCapsules);
    SEFluidCircuitPath* RightTubulesToNetTubules = rCircuit.GetPath(pulse::RenalPath::RightTubulesToNetTubules);
    SEFluidCircuitPath* RightNetTubulesToNetPeritubularCapillaries = rCircuit.GetPath(pulse::RenalPath::RightNetTubulesToNetPeritubularCapillaries);
    SEFluidCircuitPath* RightPeritubularCapillariesToNetPeritubularCapillaries = rCircuit.GetPath(pulse::RenalPath::RightPeritubularCapillariesToNetPeritubularCapillaries);

    SELiquidTransporter txpt(VolumePerTimeUnit::mL_Per_s, VolumeUnit::mL, MassUnit::ug, MassPerVolumeUnit::ug_Per_mL, pc.GetLogger());
    SEFluidCircuitCalculator calc(VolumePerPressureUnit::mL_Per_mmHg, VolumePerTimeUnit::mL_Per_s, PressureTimeSquaredPerVolumeUnit::mmHg_s2_Per_mL, PressureUnit::mmHg, VolumeUnit::mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL, pc.GetLogger());

    DataTrack trk;
    double time_s = 0.0;
    double deltaT_s = 1.0 / 90.0;
    double aortaPressure_mmHg = 100.0;
    double venaCavaPressure_mmHg = 4.0;
    double maxSteadyCycles = 1.0 / deltaT_s * 30.0; // must be steady for 30 second
    double convergencePercentage = 0.01; // within 1%
    SEScalarTime eventTime;
    eventTime.SetValue(0, TimeUnit::s);

    //initialize pressure
    RightAortaSourcePath.GetPressureSourceBaseline().SetValue(aortaPressure_mmHg, PressureUnit::mmHg);
    RightVenaCavaSourcePath.GetPressureSourceBaseline().SetValue(venaCavaPressure_mmHg, PressureUnit::mmHg);
    LeftAortaSourcePath.GetPressureSourceBaseline().SetValue(aortaPressure_mmHg, PressureUnit::mmHg);
    LeftVenaCavaSourcePath.GetPressureSourceBaseline().SetValue(venaCavaPressure_mmHg, PressureUnit::mmHg);

    // Simple system setup  
    RenalModel& bgRenal = (RenalModel&)pc.GetRenal();
    bgRenal.Initialize();
    // Renal needs this
    pc.GetBloodChemistry().GetHematocrit().SetValue(0.45817);

    //Update the circuit
    rCircuit.SetNextAndCurrentFromBaselines();
    rCircuit.StateChange();
    calc.Process(rCircuit, deltaT_s);//Preprocess wants a circuit full of data, need to calc it once

    //This can't think it doing resting stabilization, or it will just keep overwriting the TGF setpoint
    pc.m_State = pulse::EngineState::Active;

    //Do it every 10 mmHg between 80 and 200
    for (double MAP = 70.0; MAP <= 200.0; MAP += 10.0)
    {
      std::cout << "MAP = " << MAP << std::endl;

      double leftAffResistance_mmHg_s_Per_mL = 0.0;
      double leftReabsorptionResistance_mmHg_s_Per_mL = 0.0;
      double leftReabsorptionFlow_mL_Per_min = 0.0;
      double rightAffResistance_mmHg_s_Per_mL = 0.0;
      double rightReabsorptionResistance_mmHg_s_Per_mL = 0.0;
      double rightReabsorptionFlow_mL_Per_min = 0.0;
      //Loop until the GFR and RBF are steady
      double steadyGFR_L_Per_min = 0.0;
      double steadyRBF_L_Per_min = 0.0;
      double steadyUPR_mL_Per_min = 0.0;
      double currentGFR_L_Per_min = 0.0;
      double currentRBF_L_Per_min = 0.0;
      double currentUPR_mL_Per_min = 0.0;
      double steadyCycles = 0.0;
      bool GFRSteady = false;
      bool RBFSteady = false;
      bool UPRSteady = false;
      bool convergedCheck = false;
      //validation data of urine production:
      // unused: double urineValidation = (a*pow(MAP, 2) + b*MAP + c);
      //get the permeability and resistance for output:
      double permeabilityCurrentLeft_mL_Per_s_Per_mmHg_Per_m2 = 0.0;
      double permeabilityCurrentRight_mL_Per_s_Per_mmHg_Per_m2 = 0.0;
      double permeabilitySteadyLeft_mL_Per_s_Per_mmHg_Per_m2 = 0.0;
      double permeabilitySteadyRight_mL_Per_s_Per_mmHg_Per_m2 = 0.0;

      // Stabilize the circuit
      for (unsigned int i = 0; i < 3e6; i++)
      {
        //Flag beginning of cardiac cycle - this will make it just use the current value instead of a running average
        pc.GetEvents().SetEvent(eEvent::StartOfCardiacCycle, true, eventTime);

        GFRSteady = false;
        RBFSteady = false;
        UPRSteady = false;

        //Set the MAP as a static value
        LeftAortaSourcePath.GetNextPressureSource().SetValue(MAP, PressureUnit::mmHg);
        RightAortaSourcePath.GetNextPressureSource().SetValue(MAP, PressureUnit::mmHg);

        //Do all normal preprocessing for feedback for all tests but the urine production curve
        bgRenal.PreProcess();

        //Don't do any Albumin feedback by overwriting
        LeftGlomerularCapillariesToNetGlomerularCapillaries->GetNextPressureSource().Set(LeftGlomerularCapillariesToNetGlomerularCapillaries->GetPressureSourceBaseline());
        LeftBowmansCapsulesToNetBowmansCapsules->GetNextPressureSource().Set(LeftBowmansCapsulesToNetBowmansCapsules->GetPressureSourceBaseline());
        LeftTubulesToNetTubules->GetNextPressureSource().Set(LeftTubulesToNetTubules->GetPressureSourceBaseline());
        LeftPeritubularCapillariesToNetPeritubularCapillaries->GetNextPressureSource().Set(LeftPeritubularCapillariesToNetPeritubularCapillaries->GetPressureSourceBaseline());

        RightGlomerularCapillariesToNetGlomerularCapillaries->GetNextPressureSource().Set(RightGlomerularCapillariesToNetGlomerularCapillaries->GetPressureSourceBaseline());
        RightBowmansCapsulesToNetBowmansCapsules->GetNextPressureSource().Set(RightBowmansCapsulesToNetBowmansCapsules->GetPressureSourceBaseline());
        RightTubulesToNetTubules->GetNextPressureSource().Set(RightTubulesToNetTubules->GetPressureSourceBaseline());
        RightPeritubularCapillariesToNetPeritubularCapillaries->GetNextPressureSource().Set(RightPeritubularCapillariesToNetPeritubularCapillaries->GetPressureSourceBaseline());

        //Overwrite unwanted feedback
        switch (feedback)
        {
        case TGF:
        {
          //Turn off UPR feedback to just test tubuloglomerular feedback methodology
          LeftNetTubulesToNetPeritubularCapillaries->GetNextResistance().Set(LeftNetTubulesToNetPeritubularCapillaries->GetResistance());
          RightNetTubulesToNetPeritubularCapillaries->GetNextResistance().Set(RightNetTubulesToNetPeritubularCapillaries->GetResistance());
          break;
        }
        case TGFandUPR:
        {
          break;
        }
        }

        //These are normally calculated in the cardiovascular system as part of the combined circulatory circuit
        calc.Process(rCircuit, deltaT_s);
        //We only care about sodium for this test
        txpt.Transport(rGraph, deltaT_s);
        //Do the normal processing to do active transport
        bgRenal.Process();

        //Get flows and resistances to output and test:
        currentGFR_L_Per_min = bgRenal.GetGlomerularFiltrationRate(VolumePerTimeUnit::L_Per_min);
        currentRBF_L_Per_min = bgRenal.GetRenalBloodFlow(VolumePerTimeUnit::L_Per_min);
        currentUPR_mL_Per_min = bgRenal.GetUrineProductionRate(VolumePerTimeUnit::mL_Per_min);
        leftReabsorptionFlow_mL_Per_min = LeftNetTubulesToNetPeritubularCapillaries->GetNextFlow(VolumePerTimeUnit::mL_Per_min);
        leftAffResistance_mmHg_s_Per_mL = LeftAfferentArterioleToGlomerularCapillaries->GetNextResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
        leftReabsorptionResistance_mmHg_s_Per_mL = LeftNetTubulesToNetPeritubularCapillaries->GetNextResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
        rightReabsorptionFlow_mL_Per_min = RightNetTubulesToNetPeritubularCapillaries->GetNextFlow(VolumePerTimeUnit::mL_Per_min);
        rightAffResistance_mmHg_s_Per_mL = RightAfferentArterioleToGlomerularCapillaries->GetNextResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
        rightReabsorptionResistance_mmHg_s_Per_mL = RightNetTubulesToNetPeritubularCapillaries->GetNextResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
        permeabilityCurrentLeft_mL_Per_s_Per_mmHg_Per_m2 = bgRenal.GetLeftTubularReabsorptionFluidPermeability().GetValue(VolumePerTimePressureAreaUnit::mL_Per_s_mmHg_m2);
        permeabilityCurrentRight_mL_Per_s_Per_mmHg_Per_m2 = bgRenal.GetRightTubularReabsorptionFluidPermeability().GetValue(VolumePerTimePressureAreaUnit::mL_Per_s_mmHg_m2);

        //These is normally calculated in the cardiovascular system as part of the combined circulatory circuit
        calc.PostProcess(rCircuit);

        //steady conditions on the flows: 
        if (feedback == TGF)
        {
          steadyGFR_L_Per_min = currentGFR_L_Per_min;
          steadyRBF_L_Per_min = currentRBF_L_Per_min;
          steadyUPR_mL_Per_min = currentUPR_mL_Per_min;
        }

        // all must be steady to satisfy criteria
        if (currentGFR_L_Per_min <= (steadyGFR_L_Per_min * (1 + convergencePercentage)) && currentGFR_L_Per_min >= (steadyGFR_L_Per_min * (1 - convergencePercentage)))
          GFRSteady = true;
        else
          steadyGFR_L_Per_min = currentGFR_L_Per_min;

        if (currentRBF_L_Per_min <= (steadyRBF_L_Per_min * (1 + convergencePercentage)) && currentRBF_L_Per_min >= (steadyRBF_L_Per_min * (1 - convergencePercentage)))
          RBFSteady = true;
        else
          steadyRBF_L_Per_min = currentRBF_L_Per_min;

        //set upr to steady to mitigate low map fluctuations around zero
        UPRSteady = true;
        steadyUPR_mL_Per_min = currentUPR_mL_Per_min;

        permeabilitySteadyLeft_mL_Per_s_Per_mmHg_Per_m2 = permeabilityCurrentLeft_mL_Per_s_Per_mmHg_Per_m2;
        permeabilitySteadyRight_mL_Per_s_Per_mmHg_Per_m2 = permeabilityCurrentRight_mL_Per_s_Per_mmHg_Per_m2;

        if (GFRSteady && RBFSteady && UPRSteady)
          steadyCycles += 1;
        else
          steadyCycles = 0;

        if (steadyCycles == maxSteadyCycles)
        {
          convergedCheck = true;
          break;
        }
      }

      //Output the data
      if (convergedCheck)
      {
        trk.Track("MeanArterialPressure(mmHg)", time_s, MAP);
        trk.Track("GlomerularFiltrationRate(L/min)", time_s, steadyGFR_L_Per_min);
        trk.Track("RenalBloodFlow(L/min)", time_s, steadyRBF_L_Per_min);
        trk.Track("UrineProductionRate(mL/min)", time_s, steadyUPR_mL_Per_min);
        trk.Track("LeftReabsorptionFlow(mL/min)", time_s, leftReabsorptionFlow_mL_Per_min);
        trk.Track("LeftAfferentResistance(mmHg-s/mL)", time_s, leftAffResistance_mmHg_s_Per_mL);
        trk.Track("LeftReabsorptionResistance(mmHg-s/mL)", time_s, leftReabsorptionResistance_mmHg_s_Per_mL);
        trk.Track("LeftTubulesPermeability_mL_Per_s_Per_mmHg_Per_m2", time_s, permeabilitySteadyLeft_mL_Per_s_Per_mmHg_Per_m2);
        trk.Track("RightReabsorptionFlow(mL/min)", time_s, rightReabsorptionFlow_mL_Per_min);
        trk.Track("RightAfferentResistance(mmHg-s/mL)", time_s, rightAffResistance_mmHg_s_Per_mL);
        trk.Track("RightReabsorptionResistance(mmHg-s/mL)", time_s, rightReabsorptionResistance_mmHg_s_Per_mL);
        trk.Track("RightTubulesPermeability_mL_Per_s_Per_mmHg_Per_m2", time_s, permeabilitySteadyRight_mL_Per_s_Per_mmHg_Per_m2);
      }
      else
      {
        std::cerr << "Could not converge flows, check criteria" << std::endl;
        return;
      }
      time_s += 1.0;
      switch (feedback)
      {
      case TGF:
      {
        std::cout << "GFR: " << currentGFR_L_Per_min << std::endl;
        std::cout << "RBF: " << currentRBF_L_Per_min << std::endl;
        break;
      }
      case TGFandUPR:
      {
        std::cout << "UPR" << steadyUPR_mL_Per_min << std::endl;
        std::cout << "GFR: " << currentGFR_L_Per_min << std::endl;
        std::cout << "RBF: " << currentRBF_L_Per_min << std::endl;
        break;
      }
      }
    }
    trk.WriteTrackToFile(std::string(sTestDirectory + "/" + sTestName + ".csv").c_str());
    std::stringstream ss;
    ss << "It took " << tmr.GetElapsedTime_s("Test") << "s to run " << sTestName << "CircuitAndTransportTest";
    pc.GetLogger()->Info(ss.str());
  }

  void EngineTest::RenalTGFFeedbackTest(const std::string& sTestDirectory)
  {
    RenalFeedbackTest(TGF, sTestDirectory, "RenalTGFFeedbackOutput");
  }
  void EngineTest::RenalTGFandUPRFeedbackTest(const std::string& sTestDirectory)
  {
    RenalFeedbackTest(TGFandUPR, sTestDirectory, "RenalTGFandUPRFeedbackOutput");
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// checks secretion function and osmolarity/lality calculations as a function of substance quantities
  ///
  /// \details
  /// increment postassium values above average blood concentration levels and let run for a number of 
  /// seconds. After stabilization time has run compute osmolarity/lality
  /// output potassium blood concentration levels and osmolarity/lality, check against data.
  //--------------------------------------------------------------------------------------------------
  void EngineTest::RenalSystemTest(RenalSystems systemtest, const std::string& sTestDirectory, const std::string& sTestName)
  {

    TimingProfile tmr;
    tmr.Start("Test");
    Engine pe;
    Controller& pc = (Controller&)pe.GetController();
    SubstanceManager& subMgr = (SubstanceManager&)pc.GetSubstances();
    pc.GetLogger()->SetLogFile(sTestDirectory + "/RenalSystemTest.log");
    SEPatient patient(pc.GetLogger());
    patient.SerializeFromFile("./patients/StandardMale.json");
    pc.SetupPatient(patient);
    subMgr.LoadSubstanceDirectory("./");
    pc.GetSaturationCalculator().Setup();
    pc.m_Config->Initialize("./", &subMgr);
    pc.m_Config->UseExpandedKidneys(eSwitch::On);
    pc.m_Config->EnableTissue(eSwitch::Off);
    pc.CreateCircuitsAndCompartments();
    // Renal needs these tissue compartments
    // Let's make them manually, without the tissue circuit
    pc.GetCompartments().CreateTissueCompartment(pulse::TissueCompartment::LeftKidney);
    pc.GetCompartments().CreateTissueCompartment(pulse::TissueCompartment::RightKidney);
    pc.GetCompartments().CreateLiquidCompartment(pulse::ExtravascularCompartment::LeftKidneyExtracellular);
    pc.GetCompartments().CreateLiquidCompartment(pulse::ExtravascularCompartment::RightKidneyExtracellular);
    pc.GetCompartments().CreateLiquidCompartment(pulse::ExtravascularCompartment::LeftKidneyIntracellular);
    pc.GetCompartments().CreateLiquidCompartment(pulse::ExtravascularCompartment::RightKidneyIntracellular);
    pc.m_Config->EnableTissue(eSwitch::On);// This needs to be on for making the tissue to extravascular mapping
    pc.GetCompartments().StateChange();
    SESubstance& potassium = subMgr.GetPotassium();

    // Renal needs these present for Gluconeogenesis
    subMgr.AddActiveSubstance(subMgr.GetPotassium());
    subMgr.AddActiveSubstance(subMgr.GetSodium());
    subMgr.AddActiveSubstance(subMgr.GetLactate());
    subMgr.AddActiveSubstance(subMgr.GetGlucose());
    subMgr.AddActiveSubstance(subMgr.GetUrea());

    // Removing const in order to fill out and test
    //SERenalSystem &RenalSystem = pc.GetRenal();
    RenalModel& bgRenal = (RenalModel&)pc.GetRenal();
    bgRenal.Initialize();

    // VIPs only
    SEFluidCircuit& RenalCircuit = pc.GetCircuits().GetRenalCircuit();
    SELiquidCompartmentGraph& rGraph = pc.GetCompartments().GetRenalGraph();

    //Initialize potassium to baseline:
    double baselinePotassiumConcentration_g_Per_dl = 0.0185;
    SEScalarMassPerVolume K_g_Per_dL;
    K_g_Per_dL.SetValue(baselinePotassiumConcentration_g_Per_dl, MassPerVolumeUnit::g_Per_dL);   //set to normal concentration values
    subMgr.SetSubstanceConcentration(subMgr.GetPotassium(), pc.GetCompartments().GetUrineLeafCompartments(), K_g_Per_dL);
    subMgr.SetSubstanceConcentration(subMgr.GetPotassium(), pc.GetCompartments().GetVascularLeafCompartments(), K_g_Per_dL);

    //Initialize sodium
    SEScalarMassPerVolume Na_g_Per_dL;
    Na_g_Per_dL.SetValue(pc.GetConfiguration().GetPlasmaSodiumConcentrationSetPoint(MassPerVolumeUnit::g_Per_dL), MassPerVolumeUnit::g_Per_dL);
    subMgr.SetSubstanceConcentration(subMgr.GetSodium(), pc.GetCompartments().GetUrineLeafCompartments(), Na_g_Per_dL);
    subMgr.SetSubstanceConcentration(subMgr.GetSodium(), pc.GetCompartments().GetVascularLeafCompartments(), Na_g_Per_dL);

    //Initialize the things in BloodChemistry Renal needs
    pc.GetBloodChemistry().GetHematocrit().SetValue(0.45817);
    subMgr.GetSodium().GetBloodConcentration().SetValue(pc.GetConfiguration().GetPlasmaSodiumConcentrationSetPoint(MassPerVolumeUnit::g_Per_L), MassPerVolumeUnit::g_Per_L);

    //Renal nodes
    SEFluidCircuitNode* ReferenceNode = RenalCircuit.GetNode(pulse::RenalNode::Ground);
    SEFluidCircuitNode* RightRenalArteryNode = RenalCircuit.GetNode(pulse::RenalNode::RightRenalArtery);
    SEFluidCircuitNode* RightRenalVenaCavaConnectionNode = RenalCircuit.GetNode(pulse::RenalNode::RightVenaCavaConnection);
    SEFluidCircuitNode* LeftRenalArteryNode = RenalCircuit.GetNode(pulse::RenalNode::LeftRenalArtery);
    SEFluidCircuitNode* LeftRenalVenaCavaConnectionNode = RenalCircuit.GetNode(pulse::RenalNode::LeftVenaCavaConnection);

    //Renal/vascular compartments
    SELiquidCompartment* BladderCompartment = pc.GetCompartments().GetLiquidCompartment(pulse::UrineCompartment::Bladder);
    SELiquidCompartment* RightUreterCompartment = pc.GetCompartments().GetLiquidCompartment(pulse::UrineCompartment::RightUreter);
    SELiquidCompartment* LeftUreterCompartment = pc.GetCompartments().GetLiquidCompartment(pulse::UrineCompartment::LeftUreter);
    SELiquidCompartment* RightArteryCompartment = pc.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightRenalArtery);
    SELiquidCompartment* LeftArteryCompartment = pc.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftRenalArtery);
    SELiquidCompartment* RightPeritubularCapillariesCompartment = pc.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightPeritubularCapillaries);
    SELiquidCompartment* LeftPeritubularCapillariesCompartment = pc.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftPeritubularCapillaries);

    //Renal paths
    SEFluidCircuitPath& RightAortaSourcePath = RenalCircuit.CreatePath(*ReferenceNode, *RightRenalArteryNode, "RightAortaSourcePath");
    SEFluidCircuitPath& RightVenaCavaSourcePath = RenalCircuit.CreatePath(*ReferenceNode, *RightRenalVenaCavaConnectionNode, "RightVenaCavaSourcePath");
    SEFluidCircuitPath& LeftAortaSourcePath = RenalCircuit.CreatePath(*ReferenceNode, *LeftRenalArteryNode, "LeftAortaSourcePath");
    SEFluidCircuitPath& LeftVenaCavaSourcePath = RenalCircuit.CreatePath(*ReferenceNode, *LeftRenalVenaCavaConnectionNode, "LeftVenaCavaSourcePath");

    SEFluidCircuitCalculator calc(DefaultFluidCircuitCalculatorUnits, pc.GetLogger());
    SELiquidTransporter txpt(VolumePerTimeUnit::mL_Per_s, VolumeUnit::mL, MassUnit::ug, MassPerVolumeUnit::ug_Per_mL, pc.GetLogger());

    double deltaT_s = 1.0 / 90.0;
    double time_s = 0.0;

    DataTrack trk;

    double initialPotassiumConcentration_g_Per_dL = 0.0;
    double aortaPressure_mmHg = 100.0;
    double venaCavaPressure_mmHg = 4.0;
    //double convergencePercentage = 0.01; // within 1%
    double percentIncrease = 0.0;   //increment potassium concentration
    double peritubularCapillariesPotassium_g_Per_dL = 0.0;
    double bladderPotassium_g_Per_dL = 0.0;
    double ureterPotassium_g_Per_dL = 0.0;
    double runTime_min = 5.0;   //run system for 5000 iterations before checking data
    int halftime = (int)(runTime_min * 60 / deltaT_s * 0.5);

    //initialize pressure on sources:
    RightAortaSourcePath.GetPressureSourceBaseline().SetValue(aortaPressure_mmHg, PressureUnit::mmHg);
    RightVenaCavaSourcePath.GetPressureSourceBaseline().SetValue(venaCavaPressure_mmHg, PressureUnit::mmHg);
    LeftAortaSourcePath.GetPressureSourceBaseline().SetValue(aortaPressure_mmHg, PressureUnit::mmHg);
    LeftVenaCavaSourcePath.GetPressureSourceBaseline().SetValue(venaCavaPressure_mmHg, PressureUnit::mmHg);

    RightAortaSourcePath.GetNextPressureSource().SetValue(aortaPressure_mmHg, PressureUnit::mmHg);
    RightVenaCavaSourcePath.GetNextPressureSource().SetValue(venaCavaPressure_mmHg, PressureUnit::mmHg);
    LeftAortaSourcePath.GetNextPressureSource().SetValue(aortaPressure_mmHg, PressureUnit::mmHg);
    LeftVenaCavaSourcePath.GetNextPressureSource().SetValue(venaCavaPressure_mmHg, PressureUnit::mmHg);

    //Update the circuit
    RenalCircuit.SetNextAndCurrentFromBaselines();
    RenalCircuit.StateChange();
    calc.Process(RenalCircuit, deltaT_s);//Preprocess wants a circuit full of data, need to calc it once

    SEScalarTime time;
    time.SetValue(0, TimeUnit::s);

    //initialize concentrations to zero:
    double currentPotassium_g_dl = 0.0;

    switch (systemtest)
    {
    case Urinating:
    {
      break;
    }
    case Secretion:
    {
      percentIncrease = 0.1;  //10 percent increase

      //adjust initial potassium concentrations: 
      initialPotassiumConcentration_g_Per_dL = baselinePotassiumConcentration_g_Per_dl * (1 + percentIncrease);

      //set concentrations
      LeftArteryCompartment->GetSubstanceQuantity(subMgr.GetPotassium())->GetConcentration().SetValue(initialPotassiumConcentration_g_Per_dL, MassPerVolumeUnit::g_Per_dL);
      RightArteryCompartment->GetSubstanceQuantity(subMgr.GetPotassium())->GetConcentration().SetValue(initialPotassiumConcentration_g_Per_dL, MassPerVolumeUnit::g_Per_dL);
      LeftArteryCompartment->Balance(BalanceLiquidBy::Concentration);
      RightArteryCompartment->Balance(BalanceLiquidBy::Concentration);
      break;
    }
    }


    //begin renal process:
    for (int j = 0; j < runTime_min * 60.0 / deltaT_s; j++)
    {
      //Do all normal preprocessing for feedback for all tests but the urine production curve
      bgRenal.PreProcess();

      //call urinate at half simulation time:
      if (j == halftime)
      {
        switch (systemtest)
        {
        case Urinating:
        {
          pc.GetActions().GetPatientActions().HasUrinate();
          bgRenal.Urinate();
          break;
        }
        case Secretion:
        {
          break;
        }
        }
      }

      //These are normally calculated in the cardiovascular system as part of the combined circulatory circuit
      calc.Process(RenalCircuit, deltaT_s);

      txpt.Transport(rGraph, deltaT_s); //not sure if this is the same as the above commented out line

      bgRenal.Process();

      //These is normally calculated in the cardiovascular system as part of the combined circulatory circuit
      calc.PostProcess(RenalCircuit);

      //data call:
      peritubularCapillariesPotassium_g_Per_dL = (LeftPeritubularCapillariesCompartment->GetSubstanceQuantity(potassium)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_dL) +
        RightPeritubularCapillariesCompartment->GetSubstanceQuantity(potassium)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_dL)) / 2.0;
      currentPotassium_g_dl = (LeftArteryCompartment->GetSubstanceQuantity(potassium)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_dL) +
        RightArteryCompartment->GetSubstanceQuantity(potassium)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_dL)) / 2.0;
      bladderPotassium_g_Per_dL = BladderCompartment->GetSubstanceQuantity(potassium)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_dL);
      ureterPotassium_g_Per_dL = (LeftUreterCompartment->GetSubstanceQuantity(potassium)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_dL) +
        RightUreterCompartment->GetSubstanceQuantity(potassium)->GetConcentration().GetValue(MassPerVolumeUnit::g_Per_dL)) / 2.0;

      time_s += deltaT_s;

      switch (systemtest)
      {
      case Urinating:
      {
        trk.Track(time_s, rGraph);
        break;
      }
      case Secretion:
      {
        //track the potassium value in the peritubular capillaries:
        trk.Track("PeritubularPotassium(g/dl)", time_s, peritubularCapillariesPotassium_g_Per_dL);
        trk.Track("RenalArteryPotassium(g/dl)", time_s, currentPotassium_g_dl);
        trk.Track("BladderPotassium(g/dl)", time_s, bladderPotassium_g_Per_dL);
        trk.Track("UreterPotassium(g/dl)", time_s, ureterPotassium_g_Per_dL);
        break;
      }
      }
    }
    trk.WriteTrackToFile(std::string(sTestDirectory + "/" + sTestName + ".csv").c_str());
    std::stringstream ss;
    ss << "It took " << tmr.GetElapsedTime_s("Test") << "s to run " << sTestName << "SecretionandUrinatingTest";
    pc.GetLogger()->Info(ss.str());
  }

  void EngineTest::RenalSecretionTest(const std::string& sTestDirectory)
  {
    RenalSystemTest(Secretion, sTestDirectory, "RenalSecretionOutput");
  }
  void EngineTest::RenalUrinateTest(const std::string& sTestDirectory)
  {
    RenalSystemTest(Urinating, sTestDirectory, "RenalUrinateOutput");
  }
END_NAMESPACE_EX
