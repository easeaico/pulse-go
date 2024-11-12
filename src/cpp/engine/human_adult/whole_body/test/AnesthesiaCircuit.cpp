/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/human_adult/whole_body/test/EngineTest.h"
#include "engine/human_adult/whole_body/Engine.h"
#include "engine/human_adult/whole_body/controller/Controller.h"
#include "engine/human_adult/whole_body/controller/CompartmentManager.h"
#include "engine/human_adult/whole_body/controller/SubstanceManager.h"
#include "engine/human_adult/whole_body/system/physiology/Saturation.h"

#include "cdm/patient/SEPatient.h"
#include "cdm/substance/SESubstanceFraction.h"
#include "cdm/substance/SESubstanceTransport.h"
#include "cdm/circuit/fluid/SEFluidCircuit.h"
#include "cdm/circuit/fluid/SEFluidCircuitCalculator.h"
#include "cdm/compartment/fluid/SEGasCompartmentGraph.h"
#include "cdm/compartment/fluid/SEFluidCompartmentLink.h"
#include "cdm/system/environment/SEEnvironmentalConditions.h"
#include "cdm/properties/SEScalarVolumePerPressure.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarPressureTimePerVolume.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/utils/DataTrack.h"
#include "cdm/utils/TimingProfile.h"

namespace pulse { namespace human_adult_whole_body
{
  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Runs the test for the circuit math in the anesthesia machine
  ///
  /// \param  sTestDirectory  Contains the path to the directory where the output file is stored
  ///
  /// \details
  /// Tests the anesthesia machine circuit mechanics by reading in a text file that contains the driver
  /// and variable values for the circuit elements.The outputs are the resultant flows and pressures
  /// on the circuit nodes and paths. These are then stored in a file in sTestDirectory
  //--------------------------------------------------------------------------------------------------
  void EngineTest::AnesthesiaMachineCircuitAndTransportTest(RespiratoryConfiguration config, bool expandedLungs, const std::string& sTestDirectory)
  {
    TimingProfile tmr;
    tmr.Start("Test");
    //Output files
    DataTrack outTrkCircuit;
    DataTrack outTrkGraph;
    std::ofstream fileCircuit;
    std::ofstream fileGraph;

    Engine pe;
    Controller& pc = (Controller&)pe.GetController();
    pc.GetLogger()->SetLogFile(sTestDirectory + "/AnesthesiaMachineCircuitAndTransportTest.log");
    SEPatient patient(pc.GetLogger());
    patient.SerializeFromFile("./patients/StandardMale.json");
    pc.SetupPatient(patient);
    pc.GetSubstances().LoadSubstanceDirectory("./");
    pc.GetSaturationCalculator().Setup();
    pc.m_Config->Initialize("./", &pc.GetSubstances());
    pc.m_Config->UseExpandedKidneys(eSwitch::Off);
    pc.m_Config->UseExpandedLungs(expandedLungs ? eSwitch::On : eSwitch::Off);
    pc.m_Config->EnableTissue(eSwitch::Off);
    pc.CreateCircuitsAndCompartments();
    SEEnvironmentalConditions env(pc.GetLogger());
    env.SerializeFromFile("./environments/Standard.json", pc.GetSubstances());
    SEGasCompartment* cEnv = pc.GetCompartments().GetGasCompartment(pulse::EnvironmentCompartment::Ambient);
    for (SESubstanceFraction* subFrac : env.GetAmbientGases())
    {
      pc.GetSubstances().AddActiveSubstance(subFrac->GetSubstance());
      cEnv->GetSubstanceQuantity(subFrac->GetSubstance())->GetVolumeFraction().Set(subFrac->GetFractionAmount());
    }
    pc.GetSubstances().InitializeGasCompartments();

    //set environment pressure
    SEFluidCircuitNode* nEnvironment = pc.GetCircuits().GetFluidNode(pulse::EnvironmentNode::Ambient);
    nEnvironment->GetPressure().Set(env.GetAtmosphericPressure());
    nEnvironment->GetNextPressure().Set(env.GetAtmosphericPressure());

    //Grab the anesthesia circuits and graphs
    SEFluidCircuit* amCircuit = nullptr;
    SEGasCompartmentGraph* amGraph = nullptr;
    std::string sCircuitFileName;
    std::string sTransportFileName;
    if (config == AnesthesiaMachineSolo)
    {
      amCircuit = &pc.GetCircuits().GetAnesthesiaMachineCircuit();
      amGraph = &pc.GetCompartments().GetAnesthesiaMachineGraph();
      sCircuitFileName = "/AnesthesiaMachineCircuitOutput.csv";
      sTransportFileName = "/AnesthesiaMachineTransportOutput.csv";

      //Allow things to flow to ground, since the respiratory circuit isn't here
      //This approximates the total respiratory system resistance
      SEFluidCircuitPath* AnesthesiaConnectionToEnvironment = amCircuit->GetPath(pulse::AnesthesiaMachinePath::ConnectionToEnvironment);
      AnesthesiaConnectionToEnvironment->GetResistanceBaseline().SetValue(1.5, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
      AnesthesiaConnectionToEnvironment->GetNextResistance().SetValue(1.5, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    }
    else if (config == RespiratoryWithAnesthesiaMachine)
    {
      pc.GetSubstances().InitializeGasCompartments();

      amCircuit = &pc.GetCircuits().GetRespiratoryAndAnesthesiaMachineCircuit();
      amGraph = &pc.GetCompartments().GetRespiratoryAndAnesthesiaMachineGraph();
      sCircuitFileName = "/RespiratoryAndAnesthesiaMachineCircuitOutput.csv";
      sTransportFileName = "/RespiratoryAndAnesthesiaMachineTransportOutput.csv";

      //Precharge the stomach to prevent negative volume
      amCircuit->GetNode(pulse::RespiratoryNode::Stomach)->GetNextPressure().Set(env.GetAtmosphericPressure());
      amCircuit->GetNode(pulse::RespiratoryNode::Stomach)->GetPressure().Set(env.GetAtmosphericPressure());

      SEFluidCircuitPath* driverPath = amCircuit->GetPath(pulse::RespiratoryPath::EnvironmentToRespiratoryMuscle);
      driverPath->GetPressureSourceBaseline().SetValue(-12.0, PressureUnit::cmH2O);
      driverPath->GetNextPressureSource().SetValue(-12.0, PressureUnit::cmH2O);
    }
    else
    {
      return;
    }

    // Pull Pressure Source Paths
    SEFluidCircuitPath* EnvironmentToVentilatorPath = amCircuit->GetPath(pulse::AnesthesiaMachinePath::EnvironmentToVentilator);
    SEFluidCircuitPath* EnvironmentToReliefValve = amCircuit->GetPath(pulse::AnesthesiaMachinePath::EnvironmentToReliefValve);
    SEFluidCircuitPath* GasSourceToGasInlet = amCircuit->GetPath(pulse::AnesthesiaMachinePath::GasSourceToGasInlet);

    SEGasTransporter txpt(VolumePerTimeUnit::L_Per_s, VolumeUnit::L, VolumeUnit::L, NoUnit::unitless, pc.GetLogger());
    SEFluidCircuitCalculator calc(VolumePerPressureUnit::L_Per_cmH2O, VolumePerTimeUnit::L_Per_s, PressureTimeSquaredPerVolumeUnit::cmH2O_s2_Per_L, PressureUnit::cmH2O, VolumeUnit::L, PressureTimePerVolumeUnit::cmH2O_s_Per_L, pc.GetLogger());

    //Execution parameters
    double time = 0;
    double deltaT_s = 1.0 / 90.0;
    double runTime_min = 10.0;
    //Drive waveform parameters
    double period = 5.0;
    double alpha = (2 * M_PI) / (period);
    double driverPressure_cmH2O = 0.0;
    double amplitude_cmH2O = 6.0;
    double yOffset = 6.0;

    for (unsigned int i = 0; i < runTime_min * 60.0 / deltaT_s; i++)
    {
      //PreProcess
      //Set some constant elements - they're inside the loop to overwrite any baselines
      EnvironmentToReliefValve->GetNextPressureSource().SetValue(100.0, PressureUnit::cmH2O);
      GasSourceToGasInlet->GetNextFlowSource().SetValue(5.0, VolumePerTimeUnit::L_Per_min);

      //Set the driver pressure
      driverPressure_cmH2O = yOffset + amplitude_cmH2O * sin(alpha * time);   //compute new pressure
      EnvironmentToVentilatorPath->GetNextPressureSource().SetValue(driverPressure_cmH2O, PressureUnit::cmH2O);

      //Process - Execute the circuit
      calc.Process(*amCircuit, deltaT_s);
      //Execute the substance transport function
      txpt.Transport(*amGraph, deltaT_s);
      //convert 'Next' values to current
      calc.PostProcess(*amCircuit);

      outTrkCircuit.Track(time, *amCircuit);
      outTrkGraph.Track(time, *amGraph);
      time += deltaT_s;

      if (i == 0)
      {
        outTrkCircuit.CreateFile(std::string(sTestDirectory + sCircuitFileName).c_str(), fileCircuit);
        outTrkGraph.CreateFile(std::string(sTestDirectory + sTransportFileName).c_str(), fileGraph);
      }

      outTrkCircuit.StreamTrackToFile(fileCircuit);
      outTrkGraph.StreamTrackToFile(fileGraph);
    }
    fileCircuit.close();
    fileGraph.close();
    std::stringstream ss;
    ss << "It took " << tmr.GetElapsedTime_s("Test") << "s to run AnesthesiaMachineCircuitAndTransportTest";
    pc.GetLogger()->Info(ss.str());
  }

  void EngineTest::AnesthesiaMachineCircuitAndTransportTest(const std::string& sTestDirectory)
  {
    AnesthesiaMachineCircuitAndTransportTest(AnesthesiaMachineSolo, false, sTestDirectory);
  }

  void EngineTest::RespiratoryWithAnesthesiaMachineCircuitAndTransportTest(const std::string& sTestDirectory)
  {
    AnesthesiaMachineCircuitAndTransportTest(RespiratoryWithAnesthesiaMachine, false, sTestDirectory);
  }

  void EngineTest::RespiratoryExpandedLungsWithAnesthesiaMachineCircuitAndTransportTest(const std::string& sTestDirectory)
  {
    AnesthesiaMachineCircuitAndTransportTest(RespiratoryWithAnesthesiaMachine, true, sTestDirectory);
  }
END_NAMESPACE_EX
