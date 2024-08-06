/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "CommonDataModelTest.h"
#include "utils/testing/SETestReport.h"
#include "utils/testing/SETestCase.h"
#include "utils/testing/SETestSuite.h"
#include "circuit/electrical/SEElectricalCircuit.h"
#include "circuit/electrical/SEElectricalCircuitNode.h"
#include "circuit/electrical/SEElectricalCircuitPath.h"
#include "circuit/electrical/SEElectricalCircuitCalculator.h"
#include "circuit/SECircuitManager.h"
#include "circuit/fluid/SEFluidCircuitCalculator.h"
#include "circuit/thermal/SEThermalCircuit.h"
#include "circuit/thermal/SEThermalCircuitNode.h"
#include "circuit/thermal/SEThermalCircuitPath.h"
#include "circuit/thermal/SEThermalCircuitCalculator.h"
#include "compartment/thermal/SEThermalCompartment.h"
#include "compartment/thermal/SEThermalCompartmentLink.h"
#include "properties/SEScalarPressure.h"
#include "properties/SEScalarPressureTimePerVolume.h"
#include "properties/SEScalarVolumePerPressure.h"
#include "properties/SEScalarPressureTimeSquaredPerVolume.h"
#include "properties/SEScalarTime.h"
#include "properties/SEScalarVolume.h"
#include "properties/SEScalarVolumePerTime.h"
#include "utils/DataTrack.h"
#include "utils/TimingProfile.h"

//*********************************************************
//PreProcesses
//*********************************************************
void CommonDataModelTest::FluidPreProcess(SEFluidCircuit& c, double dT)
{
  double dPotential = 20 + 20 * sin(dT);
  c.GetPath("Path1")->GetNextPressureSource().SetValue(dPotential, PressureUnit::Pa);
}


//*********************************************************
//Main Calls
//*********************************************************

void CommonDataModelTest::ElectricalCircuitTest(const std::string& sTestDirectory)
{
  m_Logger->SetLogFile(sTestDirectory + "/ElectricalCircuitTest.log");
  double timeStep_s = 1.0 / 165.0;
  Info("Electric Circuit");
  SEElectricalCircuit* ElectricalCircuit = &m_Circuits->CreateElectricalCircuit("Electric");
  SEElectricalCircuitCalculator ElectricalCircuitCalculator(DefaultElectricalCircuitCalculatorUnits, m_Logger);
  ElectricalCircuit->StateChange();
  std::string sOutputFile = sTestDirectory + "/ElectricalCircuit.csv";
  //-----------------------------------------------------------
  //Nodes
  SEElectricalCircuitNode& Node1 = ElectricalCircuit->CreateNode("Node1");
  SEElectricalCircuitNode& Node2 = ElectricalCircuit->CreateNode("Node2");
  SEElectricalCircuitNode& Node3 = ElectricalCircuit->CreateNode("Node3");
  SEElectricalCircuitNode& Node4 = ElectricalCircuit->CreateNode("Node4");
  //Reference node
  ElectricalCircuit->AddNode(Node4); Node4.SetAsReferenceNode();
  Node4.GetNextVoltage().SetValue(0, ElectricPotentialUnit::V);
  //-----------------------------------------------------------
  //Paths
  SEElectricalCircuitPath& Path2 = ElectricalCircuit->CreatePath(Node1, Node2, "Path2");
  Path2.GetNextResistance().SetValue(25, ElectricResistanceUnit::Ohm);
  SEElectricalCircuitPath& Path3 = ElectricalCircuit->CreatePath(Node2, Node3, "Path3");
  Path3.GetNextResistance().SetValue(25, ElectricResistanceUnit::Ohm);
  SEElectricalCircuitPath& Path4 = ElectricalCircuit->CreatePath(Node3, Node4, "Path4");
  Path4.GetNextResistance().SetValue(25, ElectricResistanceUnit::Ohm);
  //Pressure source
  SEElectricalCircuitPath& Path1 = ElectricalCircuit->CreatePath(Node4, Node1, "Path1");
  Path1.GetNextVoltageSource().SetValue(20, ElectricPotentialUnit::V);
  ElectricalCircuit->StateChange();

  // Lock it so you cannot modify Quantity/Potential/Flux values with out a source
  m_Circuits->SetReadOnly(true);

  DataTrack trk1;
  bool serialized = false;
  double sample = 0;
  double dT = 0.0;
  while (dT < 100)
  {
    //PreProcess - to be done by the systems
    ElectricalCircuit->GetPath("Path1")->GetNextVoltageSource().SetValue(20 + 20 * sin(dT), ElectricPotentialUnit::V);
    //Process
    ElectricalCircuitCalculator.Process(*ElectricalCircuit, timeStep_s);
    //PostProcess
    ElectricalCircuitCalculator.PostProcess(*ElectricalCircuit);
    dT += timeStep_s;
    //sampleDT += dDT;
    sample += timeStep_s;
    if (sample > 0.1)// every 0.1 seconds, track state of circuit
    {
      sample = 0;
      trk1.Track(dT, *ElectricalCircuit);
    }
    if (!serialized && dT > 80)
    {
      serialized = true;
      std::string jsonDir = sTestDirectory + "/ElectricalCircuit.json";
      TestCircuitSerialization(jsonDir);      
      ElectricalCircuit = m_Circuits->GetElectricalCircuit("Electric");
    }
  }
  trk1.WriteTrackToFile(sOutputFile.c_str());
  m_Circuits->Clear();
}

void CommonDataModelTest::FluidCircuitTest(const std::string& sTestDirectory)
{
  m_Logger->SetLogFile(sTestDirectory + "/FluidCircuitTest.log");
  double timeStep_s = 1.0 / 165.0;
  Info("Fluid Circuit");
  SEFluidCircuit*  fluidCircuit = &m_Circuits->CreateFluidCircuit("Fluid");
  SEFluidCircuitCalculator      fluidCalculator(DefaultFluidCircuitCalculatorUnits, m_Logger);
  fluidCircuit->StateChange();
  std::string sOutputFile = sTestDirectory + "/FluidCircuit.csv";
  //-----------------------------------------------------------
  //Nodes
  SEFluidCircuitNode& Node1 = fluidCircuit->CreateNode("Node1");
  SEFluidCircuitNode& Node2 = fluidCircuit->CreateNode("Node2");
  SEFluidCircuitNode& Node3 = fluidCircuit->CreateNode("Node3");
  SEFluidCircuitNode& Node4 = fluidCircuit->CreateNode("Node4");
  //Reference node
  fluidCircuit->AddNode(Node4); Node4.SetAsReferenceNode();
  Node4.GetNextPressure().SetValue(0, PressureUnit::Pa);
  //-----------------------------------------------------------
  //Paths
  SEFluidCircuitPath& Path2 = fluidCircuit->CreatePath(Node1, Node2, "Path2");
  Path2.GetNextResistance().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path3 = fluidCircuit->CreatePath(Node2, Node3, "Path3");
  Path3.GetNextResistance().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path4 = fluidCircuit->CreatePath(Node3, Node4, "Path4");
  Path4.GetNextResistance().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  //Pressure source
  SEFluidCircuitPath& Path1 = fluidCircuit->CreatePath(Node4, Node1, "Path1");
  Path1.GetNextPressureSource().SetValue(20, PressureUnit::Pa);
  fluidCircuit->StateChange();

  // Lock it so you cannot modify Quantity/Potential/Flux values with out a source
  m_Circuits->SetReadOnly(true);

  DataTrack trk1;
  bool serialized = false;
  double sample = 0;
  double dT = 0.0;
  while (dT < 100)
  {
    //PreProcess - to be done by the systems
    fluidCircuit->GetPath("Path1")->GetNextPressureSource().SetValue(20 + 20 * sin(dT), PressureUnit::Pa);
    //Process
    fluidCalculator.Process(*fluidCircuit, timeStep_s);
    //PostProcess
    fluidCalculator.PostProcess(*fluidCircuit);
    dT += timeStep_s;
    //sampleDT += dDT;
    sample += timeStep_s;
    if (sample > 0.1)// every 0.1 seconds, track state of circuit
    {
      sample = 0;
      trk1.Track(dT, *fluidCircuit);
    }
    if (!serialized && dT > 80)
    {
      serialized = true;
      std::string jsonDir = sTestDirectory + "/FluidCircuit.json";
      TestCircuitSerialization(jsonDir);
      fluidCircuit = m_Circuits->GetFluidCircuit("Fluid");
    }
  }
  trk1.WriteTrackToFile(sOutputFile.c_str());
  m_Circuits->Clear();
}

void CommonDataModelTest::ThermalCircuitTest(const std::string& sTestDirectory)
{
  m_Logger->SetLogFile(sTestDirectory + "/ThermalCircuitTest.log");
  double timeStep_s = 1.0 / 165.0;
  Info("Thermal Circuit");
  SEThermalCircuit* ThermalCircuit = &m_Circuits->CreateThermalCircuit("Thermal");
  SEThermalCircuitCalculator ThermalCircuitCalculator(DefaultThermalCircuitCalculatorUnits, m_Logger);
  ThermalCircuit->StateChange();
  std::string sOutputFile = sTestDirectory + "/ThermalCircuit.csv";
  //-----------------------------------------------------------
  //Nodes
  SEThermalCircuitNode& Node1 = ThermalCircuit->CreateNode("Node1");
  SEThermalCircuitNode& Node2 = ThermalCircuit->CreateNode("Node2");
  SEThermalCircuitNode& Node3 = ThermalCircuit->CreateNode("Node3");
  SEThermalCircuitNode& Node4 = ThermalCircuit->CreateNode("Node4");
  //Reference node
  ThermalCircuit->AddNode(Node4); Node4.SetAsReferenceNode();
  Node4.GetNextTemperature().SetValue(0, TemperatureUnit::K);
  //-----------------------------------------------------------
  //Paths
  SEThermalCircuitPath& Path2 = ThermalCircuit->CreatePath(Node1, Node2, "Path2");
  Path2.GetNextResistance().SetValue(25, HeatResistanceUnit::K_Per_W);
  SEThermalCircuitPath& Path3 = ThermalCircuit->CreatePath(Node2, Node3, "Path3");
  Path3.GetNextResistance().SetValue(25, HeatResistanceUnit::K_Per_W);
  SEThermalCircuitPath& Path4 = ThermalCircuit->CreatePath(Node3, Node4, "Path4");
  Path4.GetNextResistance().SetValue(25, HeatResistanceUnit::K_Per_W);
  //Pressure source
  SEThermalCircuitPath& Path1 = ThermalCircuit->CreatePath(Node4, Node1, "Path1");
  Path1.GetNextTemperatureSource().SetValue(20, TemperatureUnit::K);
  ThermalCircuit->StateChange();

  // Lock it so you cannot modify Quantity/Potential/Flux values with out a source
  m_Circuits->SetReadOnly(true);

  DataTrack trk1;
  bool serialized = false;
  double sample = 0;
  double dT = 0.0;
  while (dT < 100)
  {
    //PreProcess - to be done by the systems
    ThermalCircuit->GetPath("Path1")->GetNextTemperatureSource().SetValue(20 + 20 * sin(dT), TemperatureUnit::K);
    //Process
    ThermalCircuitCalculator.Process(*ThermalCircuit, timeStep_s);
    //PostProcess
    ThermalCircuitCalculator.PostProcess(*ThermalCircuit);
    dT += timeStep_s;
    //sampleDT += dDT;
    sample += timeStep_s;
    if (sample > 0.1)// every 0.1 seconds, track state of circuit
    {
      sample = 0;
      trk1.Track(dT, *ThermalCircuit);
      /*for (SEThermalCircuitCompartmentLink* link : graph.GetLinks())
      {
        if (link->HasHeatTransferRate())
          trk1.Probe(link->GetName() + "Link_HeatTransferRate", dT, link->GetHeatTransferRate().GetValue(PowerUnit::W));
      }
      for (SEThermalCircuitCompartment* cmpt : graph.GetCompartments())
      {
        if(cmpt->HasTemperature())
          trk1.Probe(cmpt->GetName() + "Cmpt_Temperature", dT, cmpt->GetTemperature().GetValue(TemperatureUnit::K));
        if(cmpt->HasHeat())
          trk1.Probe(cmpt->GetName() + "Cmpt_Heat", dT, cmpt->GetHeat().GetValue(EnergyUnit::J));
      }*/
    }
    if (!serialized && dT > 80)
    {
      serialized = true;
      std::string jsonDir = sTestDirectory + "/ThermalCircuit.json"; 
      TestCircuitSerialization(jsonDir);
      ThermalCircuit = m_Circuits->GetThermalCircuit("Thermal");
    }
  }
  trk1.WriteTrackToFile(sOutputFile.c_str());
  m_Circuits->Clear();
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Tests the combined circuit methodology
///
/// \param  sTestDirectory points to the unit test directory
///
/// \details
/// This tests the functionality of combining separate circuits into once single circuit that can be
/// calculated, such as when the anesthesia machine connects to the respiratory system.
//--------------------------------------------------------------------------------------------------
void CommonDataModelTest::CombinedCircuitTest(const std::string& sTestDirectory)
{
  std::cout << "CombinedCircuitTest\n";
  m_Logger->SetLogFile(sTestDirectory + "/CombinedCircuitTest.log");
  TimingProfile p;
  double timeStep_s = 1.0 / 165.0;
  double currentTime_s = 0.0;
  DataTrack trk1;
  SEFluidCircuitCalculator fluidCalculator(DefaultFluidCircuitCalculatorUnits, m_Logger);

  //Master Circuit
  SEFluidCircuit* MasterCircuit = &m_Circuits->CreateFluidCircuit("Master");
  //-----------------------------------------------------------
  //Nodes
  SEFluidCircuitNode& MasterNode1 = MasterCircuit->CreateNode("MasterNode1");
  SEFluidCircuitNode& MasterNode2 = MasterCircuit->CreateNode("MasterNode2");
  SEFluidCircuitNode& MasterNode3 = MasterCircuit->CreateNode("MasterNode3");
  SEFluidCircuitNode& MasterNode4 = MasterCircuit->CreateNode("MasterNode4");
  //Reference node
  //This works like ground - I added code to allow it to have a value other than 0
  MasterCircuit->AddNode(MasterNode4);
  MasterNode4.SetAsReferenceNode();
  MasterNode4.GetNextPressure().SetValue(0, PressureUnit::Pa);
  //-----------------------------------------------------------
  //Paths
  SEFluidCircuitPath& MasterPath2 = MasterCircuit->CreatePath(MasterNode1, MasterNode2, "MasterPath2");
  MasterPath2.GetNextResistance().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& MasterPath3 = MasterCircuit->CreatePath(MasterNode2, MasterNode3, "MasterPath3");
  MasterPath3.GetNextResistance().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& MasterPath4 = MasterCircuit->CreatePath(MasterNode3, MasterNode4, "MasterPath4");
  MasterPath4.GetNextResistance().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  //Pressure source
  SEFluidCircuitPath& MasterPath1 = MasterCircuit->CreatePath(MasterNode4, MasterNode1, "MasterPath1");
  MasterPath1.GetNextPressureSource().SetValue(20, PressureUnit::Pa);
  //Slave Circuit
   SEFluidCircuit* SlaveCircuit = &m_Circuits->CreateFluidCircuit("Slave");
  //-----------------------------------------------------------
  //Nodes
  SEFluidCircuitNode& SlaveNode1 = SlaveCircuit->CreateNode("SlaveNode1");
  SEFluidCircuitNode& SlaveNode2 = SlaveCircuit->CreateNode("SlaveNode2");
  SEFluidCircuitNode& SlaveNode3 = SlaveCircuit->CreateNode("SlaveNode3");
  SEFluidCircuitNode& SlaveNode4 = SlaveCircuit->CreateNode("SlaveNode4");
  //Reference node
  //This works like ground - I added code to allow it to have a value other than 0
  SlaveCircuit->AddNode(SlaveNode4);
  SlaveNode4.SetAsReferenceNode();
  SlaveNode4.GetNextPressure().SetValue(0, PressureUnit::Pa);
  //-----------------------------------------------------------
  //Paths
  SEFluidCircuitPath& SlavePath2 = SlaveCircuit->CreatePath(SlaveNode1, SlaveNode2, "SlavePath2");
  SlavePath2.GetNextResistance().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& SlavePath3 = SlaveCircuit->CreatePath(SlaveNode2, SlaveNode3, "SlavePath3");
  SlavePath3.GetNextResistance().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& SlavePath4 = SlaveCircuit->CreatePath(SlaveNode3, SlaveNode4, "SlavePath4");
  SlavePath4.GetNextResistance().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  //Pressure source
  SEFluidCircuitPath& SlavePath1 = SlaveCircuit->CreatePath(SlaveNode4, SlaveNode1, "SlavePath1");
  SlavePath1.GetNextPressureSource().SetValue(20, PressureUnit::Pa);
  //Create the combined circuit
  SEFluidCircuit* CombinedCircuit = &m_Circuits->CreateFluidCircuit("Combined");
  CombinedCircuit->AddCircuit(*MasterCircuit);
  CombinedCircuit->AddCircuit(*SlaveCircuit);
  CombinedCircuit->CreatePath(MasterNode4, SlaveNode4, "GroundPath");
  CombinedCircuit->CreatePath(MasterNode3, SlaveNode3, "CombinedPath");
  CombinedCircuit->RemovePath(SlavePath1);
  CombinedCircuit->StateChange();

  bool serialized = false;
  double sample = 0;
  while (currentTime_s < 100)
  {
    //PreProcess - to be done by the systems
    double dPotential = 20 + 20 * sin(currentTime_s);
    MasterCircuit->GetPath("MasterPath1")->GetNextPressureSource().SetValue(dPotential, PressureUnit::Pa);
    //Process
    fluidCalculator.Process(*CombinedCircuit, timeStep_s);
    //PostProcess
    fluidCalculator.PostProcess(*CombinedCircuit);
    currentTime_s += timeStep_s;
    //sampleDT += dDT;
    sample += timeStep_s;
    if (sample > 0.1)// every 0.1 seconds, track state of circuit
    {
      sample = 0;
      trk1.Track(currentTime_s, *CombinedCircuit);
    }
    if (!serialized && currentTime_s > 80)
    {
      serialized = true;
      std::string jsonDir = sTestDirectory + "/CombinedCircuitTest.json"; 
      TestCircuitSerialization(jsonDir);
      // Loading will create a new circuit, so we need to delete our old ones
      // And hook up to the new ones
      MasterCircuit = m_Circuits->GetFluidCircuit("Master");
      SlaveCircuit = m_Circuits->GetFluidCircuit("Slave");
      CombinedCircuit = m_Circuits->GetFluidCircuit("Combined");
    }
  }
  std::string sOutputFile = sTestDirectory + "/CombinedCircuit.csv";
  trk1.WriteTrackToFile(sOutputFile.c_str());
  m_Circuits->Clear();
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Tests the circuit errors
///
/// \param  sTestDirectory points to the unit test directory
///
/// \details
/// This tests the errors thrown when the circuit state is attempted to be calculated,
/// but contains something improperly setup.
//--------------------------------------------------------------------------------------------------
void CommonDataModelTest::CircuitErrorTest(const std::string& sTestDirectory)
{
  std::cout << "CircuitErrorTest\n";
  //Setup a basic circuit
  m_Logger->SetLogFile(sTestDirectory + "/CombinedCircuitTest.log");
  SEFluidCircuitCalculator fluidCalculator(DefaultFluidCircuitCalculatorUnits, m_Logger);
  SEFluidCircuit* fluidCircuit = &m_Circuits->CreateFluidCircuit("Fluid");
  //-----------------------------------------------------------
  //Nodes
  SEFluidCircuitNode& Node1 = fluidCircuit->CreateNode("Node1");
  SEFluidCircuitNode& Node2 = fluidCircuit->CreateNode("Node2");
  SEFluidCircuitNode& Node3 = fluidCircuit->CreateNode("Node3");
  SEFluidCircuitNode& Node4 = fluidCircuit->CreateNode("Node4");

  //Reference node
  //This works like ground - I added code to allow it to have a value other than 0
  fluidCircuit->AddNode(Node4); Node4.SetAsReferenceNode();
  Node4.GetNextPressure().SetValue(0, PressureUnit::Pa);
  //-----------------------------------------------------------
  //Paths
  SEFluidCircuitPath& Path2 = fluidCircuit->CreatePath(Node1, Node2, "Path2");
  Path2.GetNextResistance().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path3 = fluidCircuit->CreatePath(Node2, Node3, "Path3");
  Path3.GetNextResistance().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path4 = fluidCircuit->CreatePath(Node3, Node4, "Path4");
  Path4.GetNextResistance().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path5 = fluidCircuit->CreatePath(Node2, Node4, "Path5");
  Path5.GetNextResistance().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);

  //Pressure source
  SEFluidCircuitPath& Path1 = fluidCircuit->CreatePath(Node4, Node1, "Path1");
  Path1.GetNextPressureSource().SetValue(20, PressureUnit::Pa);

  //*****************************
  //Circuit Settings Failures
  //*****************************

  // None, as of yet, moving to templates enforced a lot of stuff
  Path2.GetNextCompliance().SetValue(25, VolumePerPressureUnit::m3_Per_Pa);
  Path2.HasValidElements();
  m_Circuits->Clear();
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Tests to ensure circuits can be changed dynamically
///
/// \param  sTestDirectory points to the unit test directory
///
/// \details
/// This tests the dynamic circuit methodology by changing path elements, as well as adding a path
/// in the middle of a run.
//--------------------------------------------------------------------------------------------------
void CommonDataModelTest::DynamicallyChangingCircuitTest(const std::string& sTestDirectory)
{
  std::cout << "DynamicallyChangingCircuitTest\n";
  m_Logger->SetLogFile(sTestDirectory + "/DynamicallyChangingCircuitTest.log");
  TimingProfile p;
  double timeStep_s = 1.0 / 165.0;
  DataTrack trk1;
  SEFluidCircuitCalculator fluidCalculator(DefaultFluidCircuitCalculatorUnits, m_Logger);

  //Test Circuit
  SEFluidCircuit* fluidCircuit = &m_Circuits->CreateFluidCircuit("Fluid");
  //-----------------------------------------------------------
  //Nodes
  SEFluidCircuitNode& Node1 = fluidCircuit->CreateNode("Node1");
  SEFluidCircuitNode& Node2 = fluidCircuit->CreateNode("Node2");
  SEFluidCircuitNode& Node3 = fluidCircuit->CreateNode("Node3");
  SEFluidCircuitNode& Node4 = fluidCircuit->CreateNode("Node4");
  //Reference node
  fluidCircuit->AddNode(Node4); Node4.SetAsReferenceNode();
  Node4.GetNextPressure().SetValue(0, PressureUnit::Pa);
  //-----------------------------------------------------------
  //Paths
  SEFluidCircuitPath& Path2 = fluidCircuit->CreatePath(Node1, Node2, "Path2");
  Path2.GetNextResistance().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path3 = fluidCircuit->CreatePath(Node2, Node3, "Path3");
  Path3.GetNextResistance().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path4 = fluidCircuit->CreatePath(Node3, Node4, "Path4");
  Path4.GetNextResistance().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  //Pressure source
  SEFluidCircuitPath& Path1 = fluidCircuit->CreatePath(Node4, Node1, "Path1");
  Path1.GetNextPressureSource().SetValue(20, PressureUnit::Pa);
  fluidCircuit->StateChange();

  bool serialized = false;
  double sample = 0;
  double dT = 0.0;
  bool bChanged = false;
  while (dT < 100)
  {
    if (!bChanged && dT > 50)
    {
      std::cout << "Modify Circuit\n";
      //Change element types
      fluidCircuit->GetPath("Path2")->GetResistance().Invalidate();
      fluidCircuit->GetPath("Path2")->GetNextResistance().Invalidate();
      fluidCircuit->GetPath("Path2")->GetResistanceBaseline().Invalidate();
      fluidCircuit->GetPath("Path2")->GetNextCompliance().SetValue(0.02, VolumePerPressureUnit::m3_Per_Pa);
      fluidCircuit->GetPath("Path2")->GetSourceNode().GetNextVolume().SetValue(10, VolumeUnit::m3);

      fluidCircuit->GetPath("Path3")->GetResistance().Invalidate();
      fluidCircuit->GetPath("Path3")->GetNextResistance().Invalidate();
      fluidCircuit->GetPath("Path3")->GetResistanceBaseline().Invalidate();
      fluidCircuit->GetPath("Path3")->GetNextInertance().SetValue(0.01, PressureTimeSquaredPerVolumeUnit::Pa_s2_Per_m3);

      //Change the value, but leave the element the same
      fluidCircuit->GetPath("Path4")->GetNextResistance().SetValue(50, PressureTimePerVolumeUnit::Pa_s_Per_m3);
      fluidCircuit->GetPath("Path4")->GetResistanceBaseline().SetValue(50, PressureTimePerVolumeUnit::Pa_s_Per_m3);

      //Add a new Path
      SEFluidCircuitPath& Path5 = fluidCircuit->CreatePath(Node2, Node4, "Path5");
      Path5.GetNextResistance().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);
      //Reset the baselines
      fluidCircuit->StateChange();
      bChanged = true;
    }
    //PreProcess - to be done by the systems
    FluidPreProcess(*fluidCircuit, dT);
    //Process
    fluidCalculator.Process(*fluidCircuit, timeStep_s);
    //PostProcess
    fluidCalculator.PostProcess(*fluidCircuit);
    dT += timeStep_s;
    sample += timeStep_s;
    if (sample > 0.1)// every 0.1 seconds, track state of circuit
    {
      sample = 0;
      trk1.Track(dT, *fluidCircuit);
    }
    if (!serialized && dT > 80)
    {
      serialized = true;
      std::string jsonDir = sTestDirectory + "/DynamicallyChangingCircuit.json";
      TestCircuitSerialization(jsonDir);
      fluidCircuit = m_Circuits->GetFluidCircuit("Fluid");
    }
  }
  std::string sOutputFile = sTestDirectory + "/DynamicallyChangingCircuit.csv";
  trk1.WriteTrackToFile(sOutputFile.c_str());
  m_Circuits->Clear();
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Tests the non-zero reference circuit methodology
///
/// \param  sTestDirectory points to the unit test directory
///
/// \details
/// This tests the ability to solve a circuit correctly with a positive and negative reference 
/// potential value.
//--------------------------------------------------------------------------------------------------
void CommonDataModelTest::NonZeroReferencePositive(const std::string& sTestDirectory)
{
  std::cout << "NonZeroReferencePositive\n";
  m_Logger->SetLogFile(sTestDirectory + "/NonZeroReferencePositive.log");
  TimingProfile p;
  double timeStep_s = 1.0 / 165.0;
  DataTrack trk1;
  SEFluidCircuitCalculator fluidCalculator(DefaultFluidCircuitCalculatorUnits, m_Logger);
  SEFluidCircuit* fluidCircuit = &m_Circuits->CreateFluidCircuit("Fluid");
  //-----------------------------------------------------------
  //Nodes
  SEFluidCircuitNode& Node1 = fluidCircuit->CreateNode("Node1");
  SEFluidCircuitNode& Node2 = fluidCircuit->CreateNode("Node2");
  SEFluidCircuitNode& Node3 = fluidCircuit->CreateNode("Node3");
  SEFluidCircuitNode& Node4 = fluidCircuit->CreateNode("Node4");
  //Reference node
  //This works like ground - I added code to allow it to have a value other than 0
  fluidCircuit->AddNode(Node4); Node4.SetAsReferenceNode();
  double dRef = 10;
  std::cout << "Ref = 10\n";
  Node4.GetNextPressure().SetValue(dRef, PressureUnit::Pa);
  //-----------------------------------------------------------
  //Paths
  SEFluidCircuitPath& Path2 = fluidCircuit->CreatePath(Node1, Node2, "Path2");
  Path2.GetNextResistance().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path3 = fluidCircuit->CreatePath(Node2, Node3, "Path3");
  Path3.GetNextResistance().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path4 = fluidCircuit->CreatePath(Node3, Node4, "Path4");
  Path4.GetNextResistance().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  //Pressure source
  SEFluidCircuitPath& Path1 = fluidCircuit->CreatePath(Node4, Node1, "Path1");
  Path1.GetNextPressureSource().SetValue(20, PressureUnit::Pa);
  fluidCircuit->StateChange();

  bool serialized = false;
  double sample = 0;
  double dT = 0.0;
  while (dT < 100)
  {
    //PreProcess - to be done by the systems
    FluidPreProcess(*fluidCircuit, dT);
    //Process
    fluidCalculator.Process(*fluidCircuit, timeStep_s);
    //PostProcess
    fluidCalculator.PostProcess(*fluidCircuit);
    dT += timeStep_s;
    sample += timeStep_s;
    if (sample > 0.1)// every 0.1 seconds, track state of circuit
    {
      sample = 0;
      trk1.Track(dT, *fluidCircuit);
    }
    if (!serialized && dT > 80)
    {
      serialized = true;
      std::string jsonDir = sTestDirectory + "/NonZeroReferencePositive.json";
      TestCircuitSerialization(jsonDir);
      fluidCircuit = m_Circuits->GetFluidCircuit("Fluid");
    }
  }
  std::string sOutputFile = sTestDirectory + "/NonZeroReferencePositive.csv";
  trk1.WriteTrackToFile(sOutputFile.c_str());
  m_Circuits->Clear();
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Tests the non-zero reference circuit methodology
///
/// \param  sTestDirectory points to the unit test directory
///
/// \details
/// This tests the ability to solve a circuit correctly with a negative reference potential value.
//--------------------------------------------------------------------------------------------------
void CommonDataModelTest::NonZeroReferenceNegative(const std::string& sTestDirectory)
{
  std::cout << "NonZeroReferenceNegative\n";
  m_Logger->SetLogFile(sTestDirectory + "/NonZeroReferenceNegative.log");
  TimingProfile p;
  double timeStep_s = 1.0 / 165.0;
  DataTrack trk1;
  SEFluidCircuitCalculator fluidCalculator(DefaultFluidCircuitCalculatorUnits, m_Logger);
  SEFluidCircuit* fluidCircuit = &m_Circuits->CreateFluidCircuit("Fluid");
  //-----------------------------------------------------------
  //Nodes
  SEFluidCircuitNode& Node1 = fluidCircuit->CreateNode("Node1");
  SEFluidCircuitNode& Node2 = fluidCircuit->CreateNode("Node2");
  SEFluidCircuitNode& Node3 = fluidCircuit->CreateNode("Node3");
  SEFluidCircuitNode& Node4 = fluidCircuit->CreateNode("Node4");
  //Reference node
  //This works like ground - I added code to allow it to have a value other than 0
  fluidCircuit->AddNode(Node4); Node4.SetAsReferenceNode();
  double dRef = -10;
  std::cout << "Ref = -10\n";
  Node4.GetNextPressure().SetValue(dRef, PressureUnit::Pa);
  //-----------------------------------------------------------
  //Paths
  SEFluidCircuitPath& Path2 = fluidCircuit->CreatePath(Node1, Node2, "Path2");
  Path2.GetNextResistance().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path3 = fluidCircuit->CreatePath(Node2, Node3, "Path3");
  Path3.GetNextResistance().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path4 = fluidCircuit->CreatePath(Node3, Node4, "Path4");
  Path4.GetNextResistance().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  //Pressure source
  SEFluidCircuitPath& Path1 = fluidCircuit->CreatePath(Node4, Node1, "Path1");
  Path1.GetNextPressureSource().SetValue(20, PressureUnit::Pa);
  fluidCircuit->StateChange();
  
  bool serialized = false;
  double sample = 0;
  double dT = 0.0;
  while (dT < 100)
  {
    //PreProcess - to be done by the systems
    FluidPreProcess(*fluidCircuit, dT);
    //Process
    fluidCalculator.Process(*fluidCircuit, timeStep_s);
    //PostProcess
    fluidCalculator.PostProcess(*fluidCircuit);
    dT += timeStep_s;
    sample += timeStep_s;
    if (sample > 0.1)// every 0.1 seconds, track state of circuit
    {
      sample = 0;
      trk1.Track(dT, *fluidCircuit);
    }
    if (!serialized && dT > 80)
    {
      serialized = true;
      std::string jsonDir = sTestDirectory + "/NonZeroReferenceNegative.json";
      TestCircuitSerialization(jsonDir);
      fluidCircuit = m_Circuits->GetFluidCircuit("Fluid");
    }
  }
  std::string sOutputFile = sTestDirectory + "/NonZeroReferenceNegative.csv";
  trk1.WriteTrackToFile(sOutputFile.c_str());
  m_Circuits->Clear();
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Tests the polarized element methodology
///
/// \param  sTestDirectory points to the unit test directory
///
/// \details
/// This tests the functionality of a polarized capacitor and ensures that it functions normally
/// when the Source Pressure is greater than the Target Pressure, and functions as an open switch
/// when the Target Pressure is greater than the Source Pressure.
//--------------------------------------------------------------------------------------------------
void CommonDataModelTest::PolarizedCapacitorTest(const std::string& sTestDirectory)
{
  std::cout << "PolarizedCapacitorTest\n";
  m_Logger->SetLogFile(sTestDirectory + "/PolarizedCapacitorTest.log");
  TimingProfile p;
  double timeStep_s = 1.0 / 100.0;
  double currentTime_s = 0.0;
  DataTrack trk1;
  SEFluidCircuitCalculator fluidCalculator(DefaultFluidCircuitCalculatorUnits, m_Logger);
  SEFluidCircuit* fluidCircuit = &m_Circuits->CreateFluidCircuit("Fluid");
  //-----------------------------------------------------------
  //Nodes
  SEFluidCircuitNode& Node1 = fluidCircuit->CreateNode("Node1");
  SEFluidCircuitNode& Node2 = fluidCircuit->CreateNode("Node2");
  SEFluidCircuitNode& Node3 = fluidCircuit->CreateNode("Node3");
  SEFluidCircuitNode& Node4 = fluidCircuit->CreateNode("Node4");
  //Reference node
  fluidCircuit->AddNode(Node4); Node4.SetAsReferenceNode();
  Node4.GetNextPressure().SetValue(0, PressureUnit::Pa);
  //-----------------------------------------------------------
  //Paths
  SEFluidCircuitPath& Path2 = fluidCircuit->CreatePath(Node2, Node1, "Path2");
  Path2.GetNextCompliance().SetValue(1, VolumePerPressureUnit::m3_Per_Pa);
  Path2.SetNextPolarizedState(eGate::Closed);
  Path2.GetSourceNode().GetNextVolume().SetValue(0, VolumeUnit::m3);
  SEFluidCircuitPath& Path3 = fluidCircuit->CreatePath(Node2, Node3, "Path3");
  Path3.GetNextResistance().SetValue(1, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path4 = fluidCircuit->CreatePath(Node4, Node3, "Path4");
  Path4.GetNextPressureSource().SetValue(20, PressureUnit::Pa);
  SEFluidCircuitPath& Path1 = fluidCircuit->CreatePath(Node4, Node1, "Path1");
  Path1.GetNextPressureSource().SetValue(0, PressureUnit::Pa);
  fluidCircuit->StateChange();

  bool serialized = false;
  while (currentTime_s < 150)
  {
    if (currentTime_s > 100)
    {
      //Change it back, but more pressure
      fluidCircuit->GetPath("Path4")->GetNextPressureSource().SetValue(30, PressureUnit::Pa);
      fluidCircuit->GetPath("Path1")->GetNextPressureSource().SetValue(0, PressureUnit::Pa);
    }
    else if (currentTime_s > 50)
    {
      //Change element types
      fluidCircuit->GetPath("Path1")->GetNextPressureSource().SetValue(20, PressureUnit::Pa);
      fluidCircuit->GetPath("Path4")->GetNextPressureSource().SetValue(0, PressureUnit::Pa);
    }
    //Process
    fluidCalculator.Process(*fluidCircuit, timeStep_s);
    //PostProcess
    fluidCalculator.PostProcess(*fluidCircuit);
    currentTime_s += timeStep_s;
    trk1.Track(currentTime_s, *fluidCircuit);
    if (!serialized && currentTime_s > 130)
    {
      serialized = true;
      std::string jsonDir = sTestDirectory + "/NonZeroReferenceNegative.json";
      TestCircuitSerialization(jsonDir);
      fluidCircuit = m_Circuits->GetFluidCircuit("Fluid");
    }
  }
  std::string sOutputFile = sTestDirectory + "/PolarizedCapacitor.csv";
  trk1.WriteTrackToFile(sOutputFile.c_str());
  m_Circuits->Clear();
}

void CommonDataModelTest::PreChargeComplianceZeroVolume(const std::string& sTestDirectory)
{
  std::cout << "PreChargeComplianceZeroVolume\n";
  m_Logger->SetLogFile(sTestDirectory + "/PreChargeComplianceZeroVolume.log");
  TimingProfile p;
  double timeStep_s = 1.0 / 165.0;
  double currentTime_s = 0.0;
  DataTrack trk1;

  SEFluidCircuitCalculator fluidCalculator(DefaultFluidCircuitCalculatorUnits, m_Logger);
  SEFluidCircuit* fluidCircuit = &m_Circuits->CreateFluidCircuit("Fluid");

  SEFluidCircuitNode& node1 = fluidCircuit->CreateNode("node1");
  node1.GetPressure().SetValue(10.0, PressureUnit::mmHg);
  SEFluidCircuitNode& ground = fluidCircuit->CreateNode("node2");
  fluidCircuit->AddNode(ground);
  ground.SetAsReferenceNode();
  ground.GetNextPressure().SetValue(0.0, PressureUnit::mmHg);
  SEFluidCircuitPath& groundTonode1 = fluidCircuit->CreatePath(ground, node1, "groundTonode1");
  groundTonode1.GetPressureSourceBaseline().SetValue(10.0, PressureUnit::mmHg);
  SEFluidCircuitPath& node1Toground = fluidCircuit->CreatePath(node1, ground, "node1Toground");
  node1Toground.GetComplianceBaseline().SetValue(1.0, VolumePerPressureUnit::mL_Per_mmHg);
  node1Toground.GetSourceNode().GetVolumeBaseline().SetValue(0.0, VolumeUnit::mL);
  fluidCircuit->SetNextAndCurrentFromBaselines();
  fluidCircuit->StateChange();

  bool serialized = false;
  while (currentTime_s < 1.0)
  {
    //Process
    fluidCalculator.Process(*fluidCircuit, timeStep_s);
    //PostProcess
    fluidCalculator.PostProcess(*fluidCircuit);
    currentTime_s += timeStep_s;
    trk1.Track(currentTime_s, *fluidCircuit);
    if (!serialized && currentTime_s > 0.8)
    {
      serialized = true;
      std::string jsonDir = sTestDirectory + "/PreChargeComplianceZeroVolume.json"; 
      TestCircuitSerialization(jsonDir);
      fluidCircuit = m_Circuits->GetFluidCircuit("Fluid");
    }
  }
  std::string sOutputFile1 = sTestDirectory + "/PreChargeComplianceZeroVolume.csv";
  trk1.WriteTrackToFile(sOutputFile1.c_str());
  m_Circuits->Clear();
}

void CommonDataModelTest::PreChargeComplianceNonZeroVolume(const std::string& sTestDirectory)
{
  std::cout << "PreChargeComplianceNonZeroVolume\n";
  m_Logger->SetLogFile(sTestDirectory + "/PreChargeComplianceNonZeroVolume.log");
  TimingProfile p;
  double timeStep_s = 1.0 / 165.0;
  double currentTime_s = 0.0;
  DataTrack trk1;

  SEFluidCircuitCalculator fluidCalculator(DefaultFluidCircuitCalculatorUnits, m_Logger);
  SEFluidCircuit* fluidCircuit = &m_Circuits->CreateFluidCircuit("Fluid");

  SEFluidCircuitNode& node1 = fluidCircuit->CreateNode("node1");
  node1.GetPressure().SetValue(10.0, PressureUnit::mmHg);
  SEFluidCircuitNode& ground = fluidCircuit->CreateNode("node2");
  fluidCircuit->AddNode(ground);
  ground.SetAsReferenceNode();
  ground.GetNextPressure().SetValue(0.0, PressureUnit::mmHg);
  SEFluidCircuitPath& groundTonode1 = fluidCircuit->CreatePath(ground, node1, "groundTonode1");
  groundTonode1.GetPressureSourceBaseline().SetValue(10.0, PressureUnit::mmHg);
  SEFluidCircuitPath& node1Toground = fluidCircuit->CreatePath(node1, ground, "node1Toground");
  node1Toground.GetComplianceBaseline().SetValue(1.0, VolumePerPressureUnit::mL_Per_mmHg);
  node1Toground.GetSourceNode().GetVolumeBaseline().SetValue(10.0, VolumeUnit::mL);
  fluidCircuit->SetNextAndCurrentFromBaselines();
  fluidCircuit->StateChange();

  bool serialized = false;
  while (currentTime_s < 1.0)
  {
    //Process
    fluidCalculator.Process(*fluidCircuit, timeStep_s);
    //PostProcess
    fluidCalculator.PostProcess(*fluidCircuit);
    currentTime_s += timeStep_s;
    trk1.Track(currentTime_s, *fluidCircuit);
    if (!serialized && currentTime_s > 0.8)
    {
      serialized = true;
      std::string jsonDir = sTestDirectory + "/PreChargeComplianceNonZeroVolume.json"; 
      TestCircuitSerialization(jsonDir);
      fluidCircuit = m_Circuits->GetFluidCircuit("Fluid");
    }
  }
  std::string sOutputFile2 = sTestDirectory + "/PreChargeComplianceNonZeroVolume.csv";
  trk1.WriteTrackToFile(sOutputFile2.c_str());
  m_Circuits->Clear();
}

void CommonDataModelTest::ComplianceVolumeChange(const std::string& sTestDirectory)
{
  std::cout << "ComplianceVolumeChange\n";
  m_Logger->SetLogFile(sTestDirectory + "/ComplianceVolumeChange.log");
  TimingProfile p;
  double timeStep_s = 1.0 / 50.0;
  double currentTime_s = 0.0;
  DataTrack trk1;

  SEFluidCircuitCalculator fluidCalculator(DefaultFluidCircuitCalculatorUnits, m_Logger);
  SEFluidCircuit* fluidCircuit = &m_Circuits->CreateFluidCircuit("Fluid");

  SEFluidCircuitNode* ground = &fluidCircuit->CreateNode("node1");
  fluidCircuit->AddNode(*ground);
  ground->SetAsReferenceNode();
  ground->GetPressure().SetValue(0.0, PressureUnit::cmH2O);
  ground->GetVolumeBaseline().SetValue(std::numeric_limits<double>::infinity(), VolumeUnit::L);
  SEFluidCircuitNode* node2 = &fluidCircuit->CreateNode("node2");
  SEFluidCircuitNode* node3 = &fluidCircuit->CreateNode("node3");
  node3->GetPressure().SetValue(10.0, PressureUnit::cmH2O);
  node3->GetVolumeBaseline().SetValue(5.0, VolumeUnit::L);
  SEFluidCircuitPath* groundTonode2 = &fluidCircuit->CreatePath(*ground, *node2, "groundTonode2");
  groundTonode2->GetPressureSourceBaseline().SetValue(0.0, PressureUnit::cmH2O);
  SEFluidCircuitPath* node2Tonode3 = &fluidCircuit->CreatePath(*node2, *node3, "node2Tonode3");
  node2Tonode3->GetResistanceBaseline().SetValue(1.0, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
  SEFluidCircuitPath* groundTonode3 = &fluidCircuit->CreatePath(*ground, *node3, "groundTonode3");
  groundTonode3->GetComplianceBaseline().SetValue(1.0, VolumePerPressureUnit::L_Per_cmH2O);
  fluidCircuit->SetNextAndCurrentFromBaselines();
  fluidCircuit->StateChange();

  bool serialized = false;
  bool volumeChanged = false;
  bool pressureChanged = false;
  while (currentTime_s < 10.0)
  {    
    if (currentTime_s > 1.0)
    {
      groundTonode2->GetNextPressureSource().SetValue(10.0, PressureUnit::cmH2O);
    }
    
    if (currentTime_s > 5.0 && !volumeChanged)
    {
      volumeChanged = true;
      node3->GetNextVolume().SetReadOnly(false);
      node3->GetNextVolume().Increment(10.0, VolumeUnit::L);
    }
    if (currentTime_s > 8.0 && !pressureChanged)
    {
      pressureChanged = true;
      node3->GetNextPressure().Increment(10.0, PressureUnit::cmH2O);
    }
    //Process
    fluidCalculator.Process(*fluidCircuit, timeStep_s);
    //PostProcess
    fluidCalculator.PostProcess(*fluidCircuit);
    currentTime_s += timeStep_s;
    trk1.Track(currentTime_s, *fluidCircuit);
  }
  std::string sOutputFile2 = sTestDirectory + "/ComplianceVolumeChange.csv";
  trk1.WriteTrackToFile(sOutputFile2.c_str());
  m_Circuits->Clear();
}

void CommonDataModelTest::CircuitLockingTest(const std::string& sOutputDirectory)
{
  SETestReport testReport(m_Logger);  
  SETestSuite& testSuite = testReport.CreateTestSuite();
  testSuite.SetName("Locking");
  SETestCase* testCase;
 
  TimingProfile pTimer;
  pTimer.Start("Suite");
  
  std::cout << "CircuitLockingTestTest\n";

  m_Logger->SetLogFile(sOutputDirectory + "/CircuitLockingTest.log");
 
  SEFluidCircuit* fluidCircuit = &m_Circuits->CreateFluidCircuit("Fluid");
  //-----------------------------------------------------------
  //Nodes
  SEFluidCircuitNode& Node1 = fluidCircuit->CreateNode("Node1");
  SEFluidCircuitNode& Node2 = fluidCircuit->CreateNode("Node2");
  SEFluidCircuitNode& Node3 = fluidCircuit->CreateNode("Node3");
  SEFluidCircuitNode& Node4 = fluidCircuit->CreateNode("Node4");
  SEFluidCircuitNode& Node5 = fluidCircuit->CreateNode("Node5");

  Node3.GetNextPressure().SetValue(1, PressureUnit::Pa);
  Node3.GetPressure().SetValue(1, PressureUnit::Pa);

  //Reference node
  fluidCircuit->AddNode(Node5);
  Node5.SetAsReferenceNode();
  Node5.GetNextPressure().SetValue(0, PressureUnit::Pa);
  //-----------------------------------------------------------
  //Paths
  SEFluidCircuitPath& inductor = fluidCircuit->CreatePath(Node1, Node2, "Inductor");
  inductor.GetInductanceBaseline().SetValue(25, PressureTimeSquaredPerVolumeUnit::Pa_s2_Per_m3);
  SEFluidCircuitPath& resistor = fluidCircuit->CreatePath(Node2, Node3, "Resistor");
  resistor.GetResistanceBaseline().SetValue(25, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& capacitor = fluidCircuit->CreatePath(Node3, Node4, "Capacitor");
  capacitor.GetComplianceBaseline().SetValue(0.01, VolumePerPressureUnit::m3_Per_Pa);
  capacitor.GetSourceNode().GetVolumeBaseline().SetValue(10, VolumeUnit::m3);
  capacitor.GetFlow().SetValue(1, VolumePerTimeUnit::m3_Per_s);
  capacitor.GetNextFlow().SetValue(1, VolumePerTimeUnit::m3_Per_s);
  SEFluidCircuitPath& flowSource = fluidCircuit->CreatePath(Node4, Node5, "Flow Source");
  flowSource.GetFlowSourceBaseline().SetValue(0.1, VolumePerTimeUnit::m3_Per_s);
  SEFluidCircuitPath& potentialSource = fluidCircuit->CreatePath(Node5, Node1, "Potential Source");
  potentialSource.GetPotentialSourceBaseline().SetValue(10, PressureUnit::Pa);
  fluidCircuit->CreatePath(Node1, Node3, "Short");
  
  fluidCircuit->SetNextAndCurrentFromBaselines();
  fluidCircuit->StateChange();


  // No need to calc the circuit since we already set some values on it
  // Lock the circuit
  m_Circuits->SetReadOnly(true);
  std::string type = " Locked Before Serialization";

  bool caught = false;
  //Try to Change things we should not be able to change
  //First time through (i = 0) locked circuit
  //Second time through (i = 1) serialized locked circuit
  //Third time through (i = 2) unlocked circuit
  for (int i = 0; i <= 2; i++)
  {
    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("VolumeBaseline"+type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Capacitor")->GetSourceNode().GetVolumeBaseline().Increment(2, VolumeUnit::m3); }
    catch (...) {
      caught = true; }
    if (i!=2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked VolumeBaseline");
    if (i==2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked VolumeBaseline");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("Volume" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Capacitor")->GetSourceNode().GetVolume().Increment(2, VolumeUnit::m3); }
    catch (...) {
      caught = true; }
    if (i != 2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked Volume");
    if (i == 2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked Volume");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("NextVolume" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Capacitor")->GetSourceNode().GetNextVolume().Increment(2, VolumeUnit::m3);
    }
    catch (...) {
      caught = true;
    }
    if (i != 2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked NextVolume");
    if (i == 2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked Volume");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    // I SHOULD BE ALBE TO CHANGE THIS, AS THERE IS NO COMPLIANCE ON THE PATH
    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("No Capacitance NextVolume" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Short")->GetSourceNode().GetNextVolume().Increment(2, VolumeUnit::m3);
    }
    catch (...) {
      caught = true;
    }
    if (caught)
      testCase->AddFailure("I should be able to modify a node Volume that has no compliance on any of its connected paths");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("Pressure" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Capacitor")->GetSourceNode().GetPressure().Increment(2, PressureUnit::Pa); }
    catch (...) {
      caught = true; }
    if (i != 2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked Pressure");
    if (i == 2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked Pressure");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("NextPressure" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Capacitor")->GetSourceNode().GetNextPressure().Increment(2, PressureUnit::Pa); }
    catch (...) {
      caught = true; }
    if (i != 2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked NextPressure");
    if (i == 2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked NextPressure");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("Flow" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Capacitor")->GetFlow().Increment(2, VolumePerTimeUnit::m3_Per_s); }
    catch (...) {
      caught = true; }
    if (i != 2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked Flow");
    if (i == 2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked Flow");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("NextFlow" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Capacitor")->GetNextFlow().Increment(2, VolumePerTimeUnit::m3_Per_s); }
    catch (...) {
      caught = true; }
    if (i != 2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked NextFlow");
    if (i == 2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked NextFlow");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("FlowSource" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Flow Source")->GetFlowSource().Increment(2, VolumePerTimeUnit::m3_Per_s); }
    catch (...) {
      caught = true; }
    if (i != 2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked FlowSource");
    if (i == 2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked FlowSource");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("Resistance" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Resistor")->GetResistance().Increment(2, PressureTimePerVolumeUnit::Pa_s_Per_m3); }
    catch (...) {
      caught = true; }
    if (i != 2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked Resistance");
    if (i == 2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked Resistance");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("Capacitance" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Capacitor")->GetCapacitance().Increment(2, VolumePerPressureUnit::m3_Per_Pa); }
    catch (...) {
      caught = true; }
    if (i != 2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked Capacitance");
    if (i == 2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked Capacitance");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("Inductance" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Inductor")->GetInductance().Increment(2, PressureTimeSquaredPerVolumeUnit::Pa_s2_Per_m3); }
    catch (...) {
      caught = true; }
    if (i != 2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked Inductance");
    if (i == 2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked Inductance");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("PotentialSource" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Potential Source")->GetPotentialSource().Increment(2, PressureUnit::Pa); }
    catch (...) {
      caught = true; }
    if (i != 2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked PotentialSource");
    if (i == 2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked PotentialSource");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    if (i == 0)
    {
      // Serialize and try again
      std::string jsonDir = sOutputDirectory + "/CircuitLockingCircuit.json";
      TestCircuitSerialization(jsonDir);
      fluidCircuit = m_Circuits->GetFluidCircuit("Fluid");
      type = " Locked After Serialization";
    }
    else if (i == 1)
    {
      m_Circuits->SetReadOnly(false);
      type = " Unlocked";
    }
  }
  testReport.SerializeToFile(sOutputDirectory + "/CircuitLockingTestReport.json");
  m_Circuits->Clear();
}