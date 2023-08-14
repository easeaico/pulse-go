/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/human_adult/whole_body/test/EngineTest.h"
#include "engine/human_adult/whole_body/Engine.h"
#include "engine/human_adult/whole_body/controller/Controller.h"
#include "engine/human_adult/whole_body/controller/CompartmentManager.h"
#include "engine/human_adult/whole_body/controller/SubstanceManager.h"
#include "engine/human_adult/whole_body/system/physiology/Saturation.h"

#include "cdm/patient/SEPatient.h"
#include "cdm/circuit/fluid/SEFluidCircuit.h"
#include "cdm/circuit/fluid/SEFluidCircuitCalculator.h"
#include "cdm/substance/SESubstanceFraction.h"
#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/compartment/fluid/SEGasCompartmentGraph.h"
#include "cdm/compartment/fluid/SELiquidCompartmentGraph.h"
#include "cdm/compartment/substances/SEGasSubstanceQuantity.h"
#include "cdm/system/environment/SEEnvironment.h"
#include "cdm/system/environment/SEEnvironmentalConditions.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalarVolumePerPressure.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarInverseVolume.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/utils/TimingProfile.h"
#include "cdm/utils/DataTrack.h"

namespace pulse { namespace human_adult_whole_body
{
  void EngineTest::RespiratoryCircuitAndTransportTest(RespiratoryConfiguration config, const std::string & sTestDirectory)
  {
    TimingProfile tmr;
    tmr.Start("Test");
    //Output files
    DataTrack outTrkCircuit;
    DataTrack outTrkGraph;
    DataTrack aerosolGraphTrk;
    std::ofstream fileCircuit;
    std::ofstream fileGraph;
    std::ofstream fAerosolGraph;

    Engine pe;
    Controller& pc = (Controller&)pe.GetController();
    pc.GetLogger()->SetLogFile(sTestDirectory + "/RespiratoryCircuitAndTransportTest.log");
    SEPatient patient(pc.GetLogger());
    patient.SerializeFromFile("./patients/StandardMale.json");
    pc.SetupPatient(patient);
    pc.GetSubstances().LoadSubstanceDirectory("./");
    pc.GetSaturationCalculator().Setup();
    pc.m_Config->Initialize("./", &pc.GetSubstances());
    pc.m_Config->EnableRenal(eSwitch::Off);
    pc.m_Config->EnableTissue(eSwitch::Off);
    //jbw - Make a seperate test that turns this on
    pc.m_Config->UseExpandedRespiratory(eSwitch::On);
    pc.CreateCircuitsAndCompartments();
    pc.GetSubstances().InitializeGasCompartments();
    SEEnvironmentalConditions& env = pc.GetEnvironment().GetEnvironmentalConditions();

    SEFluidCircuit* rCircuit = nullptr;
    SEGasCompartmentGraph* rGraph = nullptr;
    SELiquidCompartmentGraph* aGraph = nullptr;
    std::string sCircuitFileName;
    std::string sTransportFileName;
    std::string sAerosolTxptFileName;
    if (config == RespiratorySolo)
    {
      rCircuit = &pc.GetCircuits().GetRespiratoryCircuit();
      rGraph = &pc.GetCompartments().GetRespiratoryGraph();
      aGraph = nullptr;
      sCircuitFileName = "/RespiratoryCircuitOutput.csv";
      sTransportFileName = "/RespiratoryTransportOutput.csv";
      sAerosolTxptFileName = "";
    }
    else if (config == RespiratoryWithInhaler)
    {
      rCircuit = &pc.GetCircuits().GetRespiratoryAndInhalerCircuit();
      rGraph = &pc.GetCompartments().GetRespiratoryAndInhalerGraph();
      aGraph = &pc.GetCompartments().GetAerosolAndInhalerGraph();
      sCircuitFileName = "/RespiratoryAndInhalerCircuitOutput.csv";
      sTransportFileName = "/RespiratoryAndInhalerTransportOutput.csv";
      sAerosolTxptFileName = "/AerosolInhalerTransportOutput.csv";

      // Get an aerosolized substance
      SESubstance* albuterol = pc.GetSubstances().GetSubstance("Albuterol");
      if (albuterol == nullptr)
      {
        pc.Error("Could not find the aerosol substance : Albuterol");
      }
      else
      {
        pc.GetSubstances().AddActiveSubstance(*albuterol);
        SELiquidCompartment* mouthpiece = pc.GetCompartments().GetLiquidCompartment(pulse::InhalerCompartment::Mouthpiece);
        mouthpiece->GetSubstanceQuantity(*albuterol)->GetMass().SetValue(90, MassUnit::ug);
        mouthpiece->Balance(BalanceLiquidBy::Mass);
      }
    }
    else if (config == RespiratoryWithMechanicalVentilation)
    {
      rCircuit = &pc.GetCircuits().GetRespiratoryAndMechanicalVentilationCircuit();
      rGraph = &pc.GetCompartments().GetRespiratoryAndMechanicalVentilationGraph();
      aGraph = &pc.GetCompartments().GetAerosolAndMechanicalVentilationGraph();
      sCircuitFileName = "/RespiratoryAndMechanicalVentilationCircuitOutput.csv";
      sTransportFileName = "/RespiratoryAndMechanicalVentilationTransportOutput.csv";
      sAerosolTxptFileName = "/AerosolMechanicalVentilationTransportOutput.csv";

      // Get an aerosolized substance
      SESubstance* albuterol = pc.GetSubstances().GetSubstance("Albuterol");
      if (albuterol == nullptr)
      {
        pc.Error("Could not find the aerosol substance : Albuterol");
      }
      else
      {
        pc.GetSubstances().AddActiveSubstance(*albuterol);
        SELiquidCompartment* connection = pc.GetCompartments().GetLiquidCompartment(pulse::MechanicalVentilationCompartment::Connection);
        //It has a NaN volume, so this will keep the same volume fraction no matter what's going on around it
        connection->GetSubstanceQuantity(*albuterol)->GetConcentration().SetValue(10, MassPerVolumeUnit::ug_Per_L);
      }
    }
    else
    {
      return;
    }

    SEFluidCircuitPath* driverPath = rCircuit->GetPath(pulse::RespiratoryPath::EnvironmentToRespiratoryMuscle);
    SEGasTransporter    gtxpt(VolumePerTimeUnit::L_Per_s, VolumeUnit::L, VolumeUnit::L, NoUnit::unitless, pc.GetLogger());
    SELiquidTransporter ltxpt(VolumePerTimeUnit::mL_Per_s, VolumeUnit::mL, MassUnit::ug, MassPerVolumeUnit::ug_Per_mL, pc.GetLogger());
    SEFluidCircuitCalculator calc(VolumePerPressureUnit::L_Per_cmH2O, VolumePerTimeUnit::L_Per_s, PressureTimeSquaredPerVolumeUnit::cmH2O_s2_Per_L, PressureUnit::cmH2O, VolumeUnit::L, PressureTimePerVolumeUnit::cmH2O_s_Per_L, pc.GetLogger());

    //Set the reference not pressure to the standard environment
    //This is needed because we're not setting the Environment during initialization in this unit test
    rCircuit->GetNode(pulse::EnvironmentNode::Ambient)->GetNextPressure().Set(env.GetAtmosphericPressure());
    rCircuit->GetNode(pulse::EnvironmentNode::Ambient)->GetPressure().Set(env.GetAtmosphericPressure());
    //Precharge the stomach to prevent negative volume
    rCircuit->GetNode(pulse::RespiratoryNode::Stomach)->GetNextPressure().Set(env.GetAtmosphericPressure());
    rCircuit->GetNode(pulse::RespiratoryNode::Stomach)->GetPressure().Set(env.GetAtmosphericPressure());

    //Circuit Analysis Test --------------------------------------------------
    //Execution parameters
    double time = 0;
    double deltaT_s = 1.0 / 50.0;
    double runTime_min = 1.0;
    //Drive waveform parameters
    double period = 5.0;
    double alpha = (2 * M_PI) / (period);
    double driverPressure_cmH2O = 0.0;
    double amplitude_cmH2O = 5.8 / 2.0;
    double yOffset = -amplitude_cmH2O;

    for (unsigned int i = 0; i < runTime_min * 60.0 / deltaT_s; i++)
    {
      //PreProcess - Push driver pressure and  variable compliance data into the Circuit
      driverPressure_cmH2O = yOffset + amplitude_cmH2O * sin(alpha * time);   //compute new pressure
      driverPath->GetNextPressureSource().SetValue(driverPressure_cmH2O, PressureUnit::cmH2O);

      //Process - Execute the circuit
      calc.Process(*rCircuit, deltaT_s);
      //Execute the substance transport function
      gtxpt.Transport(*rGraph, deltaT_s);
      // Do it again for aerosols
      if (aGraph != nullptr)
        ltxpt.Transport(*aGraph, deltaT_s);
      //convert 'Next' values to current
      calc.PostProcess(*rCircuit);

      outTrkCircuit.Track(time, *rCircuit);
      outTrkGraph.Track(time, *rGraph);
      if (aGraph != nullptr)
        aerosolGraphTrk.Track(time, *aGraph);
      time += deltaT_s;

      if (i == 0)
      {
        outTrkCircuit.CreateFile(std::string(sTestDirectory + sCircuitFileName).c_str(), fileCircuit);
        outTrkGraph.CreateFile(std::string(sTestDirectory + sTransportFileName).c_str(), fileGraph);
        if (aGraph != nullptr)
          aerosolGraphTrk.CreateFile(std::string(sTestDirectory + sAerosolTxptFileName).c_str(), fAerosolGraph);
      }
      outTrkCircuit.StreamTrackToFile(fileCircuit);
      outTrkGraph.StreamTrackToFile(fileGraph);
      if (aGraph != nullptr)
        aerosolGraphTrk.StreamTrackToFile(fAerosolGraph);
    }
    fileCircuit.close();
    fileGraph.close();
    fAerosolGraph.close();
    std::stringstream ss;
    ss << "It took " << tmr.GetElapsedTime_s("Test") << "s to run";
    pc.GetLogger()->Info(ss.str());
  }

  void EngineTest::RespiratoryCircuitAndTransportTest(const std::string & sTestDirectory)
  {
    RespiratoryCircuitAndTransportTest(RespiratorySolo, sTestDirectory);
  }

  void EngineTest::RespiratoryWithInhalerCircuitAndTransportTest(const std::string & sTestDirectory)
  {
    RespiratoryCircuitAndTransportTest(RespiratoryWithInhaler, sTestDirectory);
  }

  void EngineTest::RespiratoryWithMechanicalVentilationCircuitAndTransportTest(const std::string & sTestDirectory)
  {
    RespiratoryCircuitAndTransportTest(RespiratoryWithMechanicalVentilation, sTestDirectory);
  }

  void EngineTest::RespiratoryDriverTest(const std::string & sTestDirectory)
  {
    TimingProfile tmr;
    tmr.Start("Test");
    Engine pe;
    Controller& pc = (Controller&)pe.GetController();
    pc.GetLogger()->SetLogFile(sTestDirectory + "/RespiratoryDriverTest.log");
    SEPatient patient(pc.GetLogger());
    patient.SerializeFromFile("./patients/StandardMale.json");
    pc.SetupPatient(patient);
    pc.GetSubstances().LoadSubstanceDirectory("./");
    pc.GetSaturationCalculator().Setup();
    pc.m_Config->Initialize("./", &pc.GetSubstances());
    pc.m_Config->EnableRenal(eSwitch::Off);
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

    DataTrack trk1;
    SEFluidCircuit& RespCircuit = pc.GetCircuits().GetRespiratoryCircuit();
    SEFluidCircuitCalculator calc(VolumePerPressureUnit::L_Per_cmH2O, VolumePerTimeUnit::L_Per_s, PressureTimeSquaredPerVolumeUnit::cmH2O_s2_Per_L, PressureUnit::cmH2O, VolumeUnit::L, PressureTimePerVolumeUnit::cmH2O_s_Per_L, pc.GetLogger());

    double deltaT_s = 1.0 / 50.0;

    SEFluidCircuitPath* driverPressurePath = RespCircuit.GetPath(pulse::RespiratoryPath::EnvironmentToRespiratoryMuscle);
    SEFluidCircuitPath* rightPleuralToRespiratoryMuscle = RespCircuit.GetPath(pulse::RespiratoryPath::RightPleuralToRespiratoryMuscle);
    SEFluidCircuitPath* leftPleuralToRespiratoryMuscle = RespCircuit.GetPath(pulse::RespiratoryPath::LeftPleuralToRespiratoryMuscle);
    SEFluidCircuitPath* RightAlveoliToRightPleuralConnection = RespCircuit.GetPath(pulse::RespiratoryPath::RightAlveoliToRightPleuralConnection);
    SEFluidCircuitPath* LeftAlveoliToLeftPleuralConnection = RespCircuit.GetPath(pulse::RespiratoryPath::LeftAlveoliToLeftPleuralConnection);

    SEFluidCircuitNode* rightPleuralNode = RespCircuit.GetNode(pulse::RespiratoryNode::RightPleural);
    SEFluidCircuitNode* leftPleuralNode = RespCircuit.GetNode(pulse::RespiratoryNode::LeftPleural);
    SEFluidCircuitNode* rightDeadSpaceNode = RespCircuit.GetNode(pulse::RespiratoryNode::RightAnatomicDeadSpace);
    SEFluidCircuitNode* leftDeadSpaceNode = RespCircuit.GetNode(pulse::RespiratoryNode::LeftAnatomicDeadSpace);
    SEFluidCircuitNode* rightAlveoliNode = RespCircuit.GetNode(pulse::RespiratoryNode::RightAlveoli);
    SEFluidCircuitNode* leftAlveoliNode = RespCircuit.GetNode(pulse::RespiratoryNode::LeftAlveoli);
    SEFluidCircuitNode* muscleNode = RespCircuit.GetNode(pulse::RespiratoryNode::RespiratoryMuscle);

    driverPressurePath->GetNextPressureSource().SetValue(0.0, PressureUnit::cmH2O);
    double PressureIncrement_cmH2O = 0.1;
    double DriverPressure_cmH2O = 0.0;
    bool bSettled = false;
    bool bIRVReached = false;
    bool bRVReached = false;
    int iTime = 0;
    double AmbientPresure_cmH2O = 1033.23; // = 1 atm
    double calculatedTotalCompliance_L_Per_cmH2O = 0.0;

    RespCircuit.GetNode(pulse::EnvironmentNode::Ambient)->GetNextPressure().SetValue(AmbientPresure_cmH2O, PressureUnit::cmH2O);

    while (!bIRVReached)
    {
      double TotalVolume_L = 0.0;
      double PreviousTotalVolume_L = 0.0;
      while (!bSettled)
      {
        //Set the driver pressure
        //Note: the driver pressure should be negative for inhaling
        driverPressurePath->GetNextPressureSource().SetValue(DriverPressure_cmH2O, PressureUnit::cmH2O);

        //Process the circuit
        calc.Process(RespCircuit, deltaT_s);
        //Advance time
        calc.PostProcess(RespCircuit);

        //Calculate the total lung volume
        TotalVolume_L =
          leftDeadSpaceNode->GetVolume(VolumeUnit::L) +
          leftAlveoliNode->GetVolume(VolumeUnit::L) +
          rightDeadSpaceNode->GetVolume(VolumeUnit::L) +
          rightAlveoliNode->GetVolume(VolumeUnit::L);

        //Check to see if the circuit has stabilized
        if (std::abs(TotalVolume_L - PreviousTotalVolume_L) < 1e-10)
        {
          bool polarityReversed = RightAlveoliToRightPleuralConnection->GetPolarizedState() == eGate::Open &&
            LeftAlveoliToLeftPleuralConnection->GetPolarizedState() == eGate::Open;
          if ((!bRVReached && TotalVolume_L <= pc.GetCurrentPatient().GetResidualVolume(VolumeUnit::L)) || polarityReversed)
          {
            bRVReached = true;
          }

          if (bRVReached)
          {
            //Output values
            double driverPressure = driverPressurePath->GetPressureSource(PressureUnit::cmH2O);

            trk1.Track("LungVolume_L", iTime, TotalVolume_L);
            trk1.Track("DriverPressure_cmH2O", iTime, driverPressure);

            double leftAlveoliPressure = leftAlveoliNode->GetPressure(PressureUnit::cmH2O);
            double leftAlveoliVolume = leftAlveoliNode->GetVolume(VolumeUnit::L);
            double leftPleuralPressure = leftPleuralNode->GetPressure(PressureUnit::cmH2O);
            double leftPleuralVolume = leftPleuralNode->GetVolume(VolumeUnit::L);
            double leftChestWallCompliance_L_Per_cmH2O = leftPleuralToRespiratoryMuscle->GetCompliance(VolumePerPressureUnit::L_Per_cmH2O);
            double leftLungCompliance_L_Per_cmH2O = leftPleuralToRespiratoryMuscle->GetCompliance(VolumePerPressureUnit::L_Per_cmH2O);

            double rightAlveoliPressure = rightAlveoliNode->GetPressure(PressureUnit::cmH2O);
            double rightAlveoliVolume = rightAlveoliNode->GetVolume(VolumeUnit::L);
            double rightPleuralPressure = rightPleuralNode->GetPressure(PressureUnit::cmH2O);
            double rightPleuralVolume = rightPleuralNode->GetVolume(VolumeUnit::L);
            double rightChestWallCompliance_L_Per_cmH2O = rightPleuralToRespiratoryMuscle->GetCompliance(VolumePerPressureUnit::L_Per_cmH2O);
            double rightLungCompliance_L_Per_cmH2O = rightPleuralToRespiratoryMuscle->GetCompliance(VolumePerPressureUnit::L_Per_cmH2O);

            double leftSideCompliance_L_Per_cmH2O = 1.0 / (1.0 / leftChestWallCompliance_L_Per_cmH2O + 1.0 / leftLungCompliance_L_Per_cmH2O);
            double rightSideCompliance_L_Per_cmH2O = 1.0 / (1.0 / rightChestWallCompliance_L_Per_cmH2O + 1.0 / rightLungCompliance_L_Per_cmH2O);

            double totalCompliance_L_Per_cmH2O = leftSideCompliance_L_Per_cmH2O + rightSideCompliance_L_Per_cmH2O;

            double approxZero = 1e-10;
            if (!(driverPressure < approxZero && driverPressure > -approxZero))
            {
              calculatedTotalCompliance_L_Per_cmH2O = -(TotalVolume_L - pc.GetCurrentPatient().GetFunctionalResidualCapacity(VolumeUnit::L)) / driverPressure;
            }

            trk1.Track("ChestWallPressure_cmH2O", iTime, muscleNode->GetPressure(PressureUnit::cmH2O) - AmbientPresure_cmH2O);
            trk1.Track("RightAlveoliVolume_L", iTime, rightAlveoliVolume);
            trk1.Track("LeftAlveoliVolume_L", iTime, leftAlveoliVolume);
            trk1.Track("RightAlveoliPressure_cmH2O", iTime, rightAlveoliPressure - AmbientPresure_cmH2O);
            trk1.Track("LeftAlveoliPressure_cmH2O", iTime, leftAlveoliPressure - AmbientPresure_cmH2O);
            trk1.Track("RightPleuralVolume_L", iTime, rightPleuralVolume);
            trk1.Track("LeftPleuralVolume_L", iTime, leftPleuralVolume);
            trk1.Track("RightPleuralPressure_cmH2O", iTime, rightPleuralPressure - AmbientPresure_cmH2O);
            trk1.Track("LeftPleuralPressure_cmH2O", iTime, leftPleuralPressure - AmbientPresure_cmH2O);
            trk1.Track("RightTranspulmonaryPressure_cmH2O", iTime, rightAlveoliPressure - rightPleuralPressure);
            trk1.Track("LeftTranspulmonaryPressure_cmH2O", iTime, leftAlveoliPressure - leftPleuralPressure);

            trk1.Track("leftSideCompliance_L_Per_cmH2O", iTime, leftSideCompliance_L_Per_cmH2O);
            trk1.Track("rightSideCompliance_L_Per_cmH2O", iTime, rightSideCompliance_L_Per_cmH2O);
            trk1.Track("totalCompliance_L_Per_cmH2O", iTime, totalCompliance_L_Per_cmH2O);

            trk1.Track("calculatedTotalCompliance_L_Per_cmH2O", iTime, calculatedTotalCompliance_L_Per_cmH2O);

            iTime++;
          }
          bSettled = true;
        }
        PreviousTotalVolume_L = TotalVolume_L;
      }
      bSettled = false;

      //Check to see if we've gone all the way to the max volume
      if (TotalVolume_L >= pc.GetCurrentPatient().GetTotalLungCapacity(VolumeUnit::L))
      {
        bIRVReached = true;
      }
      else if (!bRVReached)
      {
        //Exhale more
        DriverPressure_cmH2O = DriverPressure_cmH2O + PressureIncrement_cmH2O;
      }
      else
      {
        //Inhale more
        DriverPressure_cmH2O = DriverPressure_cmH2O - PressureIncrement_cmH2O;
      }
    }
    trk1.WriteTrackToFile(std::string(sTestDirectory + "/RespiratoryDriverOutput.csv").c_str());
    std::stringstream ss;
    ss << "It took " << tmr.GetElapsedTime_s("Test") << "s to run RespiratoryDriverTest";
    pc.GetLogger()->Info(ss.str());
  }
END_NAMESPACE_EX
