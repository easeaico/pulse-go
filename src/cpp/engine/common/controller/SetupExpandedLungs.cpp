/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/PulseConfiguration.h"
#include "engine/common/controller/Controller.h"
#include "engine/common/controller/CircuitManager.h"
#include "engine/common/controller/CompartmentManager.h"

#include "cdm/circuit/SECircuitManager.h"
#include "cdm/circuit/fluid/SEFluidCircuit.h"
#include "cdm/circuit/fluid/SEFluidCircuitNode.h"
#include "cdm/circuit/fluid/SEFluidCircuitPath.h"
#include "cdm/compartment/fluid/SELiquidCompartment.h"
#include "cdm/compartment/fluid/SELiquidCompartmentLink.h"
#include "cdm/compartment/fluid/SELiquidCompartmentGraph.h"
#include "cdm/compartment/fluid/SEGasCompartment.h"
#include "cdm/compartment/fluid/SEGasCompartmentLink.h"
#include "cdm/compartment/fluid/SEGasCompartmentGraph.h"
#include "cdm/patient/SEPatient.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarArea.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarLength.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerMass.h"

namespace pulse
{
  void Controller::SetupExpandedLungsRespiratory()
  {
    Info("Setting Up Expanded Lungs Respiratory");
    double RightLungRatio = m_InitialPatient->GetRightLungRatio().GetValue();
    double LeftLungRatio = 1 - RightLungRatio;

    SEFluidCircuit& cRespiratory = m_Circuits->GetRespiratoryCircuit();
    SEFluidCircuitNode& Ambient = *m_Circuits->GetFluidNode(pulse::EnvironmentNode::Ambient);
    cRespiratory.AddNode(Ambient);

    //Input parameters
    const double RespiratorySystemCompliance_L_Per_cmH20 = 0.1; /// \cite Levitzky2013pulmonary
    const double RespiratorySideCompliance_L_Per_cmH2O = RespiratorySystemCompliance_L_Per_cmH20 / 2.0; //compliances in parallel sum, so divide by 2 for each lung
    const double LungCompliance_L_Per_cmH2O = 2.0 * RespiratorySideCompliance_L_Per_cmH2O; //compliances in series, so multiply by 2 for equal split
    const double ChestWallCompliance_L_Per_cmH2O = LungCompliance_L_Per_cmH2O; // =0.1 L/cmH2O each /// \cite kacmarek2016egan p233
    const double IntrapleuralPressure_cmH2O = -5.0; /// \cite Levitzky2013pulmonary
    //const double TotalAirwayResistance_cmH2O_s_Per_L = 1.5; /// \cite Levitzky2013pulmonary

    //Should add up to 100% of total airway resistance
    /// \cite kacmarek2016egan
    // const double TracheaResistanceFraction = 0.5;
    // TracheaResistanceFraction is implied
    //const double BronchiResistanceFraction = 0.3;
    //const double AlveoliDuctResistanceFraction = 0.2;

    double functionalResidualCapacity_L = m_InitialPatient->GetFunctionalResidualCapacity(VolumeUnit::L);
    double anatomicDeadSpaceVolume_L = 0.002 * m_InitialPatient->GetWeight(MassUnit::kg); /// \cite Levitzky2013pulmonary
    double alveolarDeadSpaceVolume_L = 0.001;  //Should change with certain diseases /// \cite Levitzky2013pulmonary
    double physiologicDeadSpaceVolume_L = anatomicDeadSpaceVolume_L + alveolarDeadSpaceVolume_L;
    //double pleuralVolume_L = 20.0 / 1000.0; //this is a liquid volume  /// \cite Levitzky2013pulmonary
    double pleuralVolume_L = functionalResidualCapacity_L; //Make this a gas volume to mimic the liquid volume
    double alveoliVolume_L = functionalResidualCapacity_L - physiologicDeadSpaceVolume_L;

    double openResistance_cmH2O_s_Per_L = m_Config->GetDefaultOpenFlowResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);

    SEFluidCircuitNode& RightMainBronchus = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightMainBronchus);
    SEFluidCircuitNode& LeftMainBronchus = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftMainBronchus);
    SEFluidCircuitNode& RightIntermediateBronchus = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightIntermediateBronchus);
    SEFluidCircuitNode& RightSuperiorLobarBronchus = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightSuperiorLobarBronchus);
    SEFluidCircuitNode& RightMiddleLobarBronchus = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightMiddleLobarBronchus);
    SEFluidCircuitNode& RightInferiorLobarBronchus1 = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightInferiorLobarBronchus1);
    SEFluidCircuitNode& LeftInferiorLobarBronchus1 = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftInferiorLobarBronchus1);
    SEFluidCircuitNode& LeftSuperiorLobarBronchus = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftSuperiorLobarBronchus);
    SEFluidCircuitNode& RightInferiorLobarBronchus2 = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightInferiorLobarBronchus2);
    SEFluidCircuitNode& LeftInferiorLobarBronchus2 = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftInferiorLobarBronchus2);
    SEFluidCircuitNode& RightSuperiorApicoposteriorBronchus = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightSuperiorApicoposteriorBronchus);
    SEFluidCircuitNode& RightInferiorLobarBronchus3 = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightInferiorLobarBronchus3);
    SEFluidCircuitNode& LeftInferiorLobarBronchus3 = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftInferiorLobarBronchus3);
    SEFluidCircuitNode& LeftLingularBonchus = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftLingularBonchus);
    SEFluidCircuitNode& LeftSuperiorApicoposteriorBronchus = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftSuperiorApicoposteriorBronchus);
    SEFluidCircuitNode& RightInferiorLobarBronchus4 = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightInferiorLobarBronchus4);

    SEFluidCircuitNode& RightSuperiorLobeApicalBronchiole = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightSuperiorLobeApicalBronchiole);
    SEFluidCircuitNode& RightSuperiorLobePosteriorBronchiole = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightSuperiorLobePosteriorBronchiole);
    SEFluidCircuitNode& RightSuperiorLobeAnteriorBronchiole = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightSuperiorLobeAnteriorBronchiole);
    SEFluidCircuitNode& RightMiddleLobeLateralBronchiole = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightMiddleLobeLateralBronchiole);
    SEFluidCircuitNode& RightMiddleLobeMedialBronchiole = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightMiddleLobeMedialBronchiole);
    SEFluidCircuitNode& RightInferiorLobeSuperiorBronchiole = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightInferiorLobeSuperiorBronchiole);
    SEFluidCircuitNode& RightInferiorLobeMedialBasalBronchiole = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightInferiorLobeMedialBasalBronchiole);
    SEFluidCircuitNode& RightInferiorLobeAnteriorBasalBronchiole = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightInferiorLobeAnteriorBasalBronchiole);
    SEFluidCircuitNode& RightInferiorLobeLateralBasalBronchiole = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightInferiorLobeLateralBasalBronchiole);
    SEFluidCircuitNode& RightInferiorLobePosteriorBasalBronchiole = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightInferiorLobePosteriorBasalBronchiole);
    SEFluidCircuitNode& LeftInferiorLobePosteriorBasalBronchiole = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftInferiorLobePosteriorBasalBronchiole);
    SEFluidCircuitNode& LeftInferiorLobeLateralBasalBronchiole = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftInferiorLobeLateralBasalBronchiole);
    SEFluidCircuitNode& LeftInferiorLobeAnteromedialBasalBronchiole = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftInferiorLobeAnteromedialBasalBronchiole);
    SEFluidCircuitNode& LeftInferiorLobeSuperiorBronchiole = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftInferiorLobeSuperiorBronchiole);
    SEFluidCircuitNode& LeftSuperiorLobeInferiorLingulaBronchiole = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftSuperiorLobeInferiorLingulaBronchiole);
    SEFluidCircuitNode& LeftSuperiorLobeSuperiorLingulaBronchiole = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftSuperiorLobeSuperiorLingulaBronchiole);
    SEFluidCircuitNode& LeftSuperiorLobeAnteriorBronchiole = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftSuperiorLobeAnteriorBronchiole);
    SEFluidCircuitNode& LeftSuperiorLobeApicoposteriorBronchiole = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftSuperiorLobeApicoposteriorBronchiole);

    SEFluidCircuitNode& RightSuperiorLobeApicalAlveolarDeadSpace = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightSuperiorLobeApicalAlveolarDeadSpace);
    SEFluidCircuitNode& RightSuperiorLobePosteriorAlveolarDeadSpace = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightSuperiorLobePosteriorAlveolarDeadSpace);
    SEFluidCircuitNode& RightSuperiorLobeAnteriorAlveolarDeadSpace = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightSuperiorLobeAnteriorAlveolarDeadSpace);
    SEFluidCircuitNode& RightMiddleLobeLateralAlveolarDeadSpace = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightMiddleLobeLateralAlveolarDeadSpace);
    SEFluidCircuitNode& RightMiddleLobeMedialAlveolarDeadSpace = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightMiddleLobeMedialAlveolarDeadSpace);
    SEFluidCircuitNode& RightInferiorLobeSuperiorAlveolarDeadSpace = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightInferiorLobeSuperiorAlveolarDeadSpace);
    SEFluidCircuitNode& RightInferiorLobeMedialBasalAlveolarDeadSpace = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightInferiorLobeMedialBasalAlveolarDeadSpace);
    SEFluidCircuitNode& RightInferiorLobeAnteriorBasalAlveolarDeadSpace = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightInferiorLobeAnteriorBasalAlveolarDeadSpace);
    SEFluidCircuitNode& RightInferiorLobeLateralBasalAlveolarDeadSpace = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightInferiorLobeLateralBasalAlveolarDeadSpace);
    SEFluidCircuitNode& RightInferiorLobePosteriorBasalAlveolarDeadSpace = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightInferiorLobePosteriorBasalAlveolarDeadSpace);
    SEFluidCircuitNode& LeftInferiorLobePosteriorBasalAlveolarDeadSpace = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftInferiorLobePosteriorBasalAlveolarDeadSpace);
    SEFluidCircuitNode& LeftInferiorLobeLateralBasalAlveolarDeadSpace = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftInferiorLobeLateralBasalAlveolarDeadSpace);
    SEFluidCircuitNode& LeftInferiorLobeAnteromedialBasalAlveolarDeadSpace = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftInferiorLobeAnteromedialBasalAlveolarDeadSpace);
    SEFluidCircuitNode& LeftInferiorLobeSuperiorAlveolarDeadSpace = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftInferiorLobeSuperiorAlveolarDeadSpace);
    SEFluidCircuitNode& LeftSuperiorLobeInferiorLingulaAlveolarDeadSpace = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftSuperiorLobeInferiorLingulaAlveolarDeadSpace);
    SEFluidCircuitNode& LeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace);
    SEFluidCircuitNode& LeftSuperiorLobeAnteriorAlveolarDeadSpace = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftSuperiorLobeAnteriorAlveolarDeadSpace);
    SEFluidCircuitNode& LeftSuperiorLobeApicoposteriorAlveolarDeadSpace = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftSuperiorLobeApicoposteriorAlveolarDeadSpace);

    SEFluidCircuitNode& RightSuperiorLobeApicalAlveoli = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightSuperiorLobeApicalAlveoli);
    SEFluidCircuitNode& RightSuperiorLobePosteriorAlveoli = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightSuperiorLobePosteriorAlveoli);
    SEFluidCircuitNode& RightSuperiorLobeAnteriorAlveoli = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightSuperiorLobeAnteriorAlveoli);
    SEFluidCircuitNode& RightMiddleLobeLateralAlveoli = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightMiddleLobeLateralAlveoli);
    SEFluidCircuitNode& RightMiddleLobeMedialAlveoli = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightMiddleLobeMedialAlveoli);
    SEFluidCircuitNode& RightInferiorLobeSuperiorAlveoli = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightInferiorLobeSuperiorAlveoli);
    SEFluidCircuitNode& RightInferiorLobeMedialBasalAlveoli = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightInferiorLobeMedialBasalAlveoli);
    SEFluidCircuitNode& RightInferiorLobeAnteriorBasalAlveoli = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightInferiorLobeAnteriorBasalAlveoli);
    SEFluidCircuitNode& RightInferiorLobeLateralBasalAlveoli = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightInferiorLobeLateralBasalAlveoli);
    SEFluidCircuitNode& RightInferiorLobePosteriorBasalAlveoli = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::RightInferiorLobePosteriorBasalAlveoli);
    SEFluidCircuitNode& LeftInferiorLobePosteriorBasalAlveoli = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftInferiorLobePosteriorBasalAlveoli);
    SEFluidCircuitNode& LeftInferiorLobeLateralBasalAlveoli = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftInferiorLobeLateralBasalAlveoli);
    SEFluidCircuitNode& LeftInferiorLobeAnteromedialBasalAlveoli = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftInferiorLobeAnteromedialBasalAlveoli);
    SEFluidCircuitNode& LeftInferiorLobeSuperiorAlveoli = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftInferiorLobeSuperiorAlveoli);
    SEFluidCircuitNode& LeftSuperiorLobeInferiorLingulaAlveoli = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftSuperiorLobeInferiorLingulaAlveoli);
    SEFluidCircuitNode& LeftSuperiorLobeSuperiorLingulaAlveoli = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftSuperiorLobeSuperiorLingulaAlveoli);
    SEFluidCircuitNode& LeftSuperiorLobeAnteriorAlveoli = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftSuperiorLobeAnteriorAlveoli);
    SEFluidCircuitNode& LeftSuperiorLobeApicoposteriorAlveoli = cRespiratory.CreateNode(pulse::ExpandedLungsRespiratoryNode::LeftSuperiorLobeApicoposteriorAlveoli);

    RightMainBronchus.GetPressure().Set(Ambient.GetNextPressure());
    LeftMainBronchus.GetPressure().Set(Ambient.GetNextPressure());
    RightIntermediateBronchus.GetPressure().Set(Ambient.GetNextPressure());
    RightSuperiorLobarBronchus.GetPressure().Set(Ambient.GetNextPressure());
    RightMiddleLobarBronchus.GetPressure().Set(Ambient.GetNextPressure());
    RightInferiorLobarBronchus1.GetPressure().Set(Ambient.GetNextPressure());
    LeftInferiorLobarBronchus1.GetPressure().Set(Ambient.GetNextPressure());
    LeftSuperiorLobarBronchus.GetPressure().Set(Ambient.GetNextPressure());
    RightInferiorLobarBronchus2.GetPressure().Set(Ambient.GetNextPressure());
    LeftInferiorLobarBronchus2.GetPressure().Set(Ambient.GetNextPressure());
    RightSuperiorApicoposteriorBronchus.GetPressure().Set(Ambient.GetNextPressure());
    RightInferiorLobarBronchus3.GetPressure().Set(Ambient.GetNextPressure());
    LeftInferiorLobarBronchus3.GetPressure().Set(Ambient.GetNextPressure());
    LeftLingularBonchus.GetPressure().Set(Ambient.GetNextPressure());
    LeftSuperiorApicoposteriorBronchus.GetPressure().Set(Ambient.GetNextPressure());
    RightInferiorLobarBronchus4.GetPressure().Set(Ambient.GetNextPressure());

    RightSuperiorLobeApicalBronchiole.GetPressure().Set(Ambient.GetNextPressure());
    RightSuperiorLobePosteriorBronchiole.GetPressure().Set(Ambient.GetNextPressure());
    RightSuperiorLobeAnteriorBronchiole.GetPressure().Set(Ambient.GetNextPressure());
    RightMiddleLobeLateralBronchiole.GetPressure().Set(Ambient.GetNextPressure());
    RightMiddleLobeMedialBronchiole.GetPressure().Set(Ambient.GetNextPressure());
    RightInferiorLobeSuperiorBronchiole.GetPressure().Set(Ambient.GetNextPressure());
    RightInferiorLobeMedialBasalBronchiole.GetPressure().Set(Ambient.GetNextPressure());
    RightInferiorLobeAnteriorBasalBronchiole.GetPressure().Set(Ambient.GetNextPressure());
    RightInferiorLobeLateralBasalBronchiole.GetPressure().Set(Ambient.GetNextPressure());
    RightInferiorLobePosteriorBasalBronchiole.GetPressure().Set(Ambient.GetNextPressure());
    LeftInferiorLobePosteriorBasalBronchiole.GetPressure().Set(Ambient.GetNextPressure());
    LeftInferiorLobeLateralBasalBronchiole.GetPressure().Set(Ambient.GetNextPressure());
    LeftInferiorLobeAnteromedialBasalBronchiole.GetPressure().Set(Ambient.GetNextPressure());
    LeftInferiorLobeSuperiorBronchiole.GetPressure().Set(Ambient.GetNextPressure());
    LeftSuperiorLobeInferiorLingulaBronchiole.GetPressure().Set(Ambient.GetNextPressure());
    LeftSuperiorLobeSuperiorLingulaBronchiole.GetPressure().Set(Ambient.GetNextPressure());
    LeftSuperiorLobeAnteriorBronchiole.GetPressure().Set(Ambient.GetNextPressure());
    LeftSuperiorLobeApicoposteriorBronchiole.GetPressure().Set(Ambient.GetNextPressure());

    RightSuperiorLobeApicalAlveolarDeadSpace.GetPressure().Set(Ambient.GetNextPressure());
    RightSuperiorLobePosteriorAlveolarDeadSpace.GetPressure().Set(Ambient.GetNextPressure());
    RightSuperiorLobeAnteriorAlveolarDeadSpace.GetPressure().Set(Ambient.GetNextPressure());
    RightMiddleLobeLateralAlveolarDeadSpace.GetPressure().Set(Ambient.GetNextPressure());
    RightMiddleLobeMedialAlveolarDeadSpace.GetPressure().Set(Ambient.GetNextPressure());
    RightInferiorLobeSuperiorAlveolarDeadSpace.GetPressure().Set(Ambient.GetNextPressure());
    RightInferiorLobeMedialBasalAlveolarDeadSpace.GetPressure().Set(Ambient.GetNextPressure());
    RightInferiorLobeAnteriorBasalAlveolarDeadSpace.GetPressure().Set(Ambient.GetNextPressure());
    RightInferiorLobeLateralBasalAlveolarDeadSpace.GetPressure().Set(Ambient.GetNextPressure());
    RightInferiorLobePosteriorBasalAlveolarDeadSpace.GetPressure().Set(Ambient.GetNextPressure());
    LeftInferiorLobePosteriorBasalAlveolarDeadSpace.GetPressure().Set(Ambient.GetNextPressure());
    LeftInferiorLobeLateralBasalAlveolarDeadSpace.GetPressure().Set(Ambient.GetNextPressure());
    LeftInferiorLobeAnteromedialBasalAlveolarDeadSpace.GetPressure().Set(Ambient.GetNextPressure());
    LeftInferiorLobeSuperiorAlveolarDeadSpace.GetPressure().Set(Ambient.GetNextPressure());
    LeftSuperiorLobeInferiorLingulaAlveolarDeadSpace.GetPressure().Set(Ambient.GetNextPressure());
    LeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace.GetPressure().Set(Ambient.GetNextPressure());
    LeftSuperiorLobeAnteriorAlveolarDeadSpace.GetPressure().Set(Ambient.GetNextPressure());
    LeftSuperiorLobeApicoposteriorAlveolarDeadSpace.GetPressure().Set(Ambient.GetNextPressure());

    RightSuperiorLobeApicalAlveoli.GetPressure().Set(Ambient.GetNextPressure());
    RightSuperiorLobePosteriorAlveoli.GetPressure().Set(Ambient.GetNextPressure());
    RightSuperiorLobeAnteriorAlveoli.GetPressure().Set(Ambient.GetNextPressure());
    RightMiddleLobeLateralAlveoli.GetPressure().Set(Ambient.GetNextPressure());
    RightMiddleLobeMedialAlveoli.GetPressure().Set(Ambient.GetNextPressure());
    RightInferiorLobeSuperiorAlveoli.GetPressure().Set(Ambient.GetNextPressure());
    RightInferiorLobeMedialBasalAlveoli.GetPressure().Set(Ambient.GetNextPressure());
    RightInferiorLobeAnteriorBasalAlveoli.GetPressure().Set(Ambient.GetNextPressure());
    RightInferiorLobeLateralBasalAlveoli.GetPressure().Set(Ambient.GetNextPressure());
    RightInferiorLobePosteriorBasalAlveoli.GetPressure().Set(Ambient.GetNextPressure());
    LeftInferiorLobePosteriorBasalAlveoli.GetPressure().Set(Ambient.GetNextPressure());
    LeftInferiorLobeLateralBasalAlveoli.GetPressure().Set(Ambient.GetNextPressure());
    LeftInferiorLobeAnteromedialBasalAlveoli.GetPressure().Set(Ambient.GetNextPressure());
    LeftInferiorLobeSuperiorAlveoli.GetPressure().Set(Ambient.GetNextPressure());
    LeftSuperiorLobeInferiorLingulaAlveoli.GetPressure().Set(Ambient.GetNextPressure());
    LeftSuperiorLobeSuperiorLingulaAlveoli.GetPressure().Set(Ambient.GetNextPressure());
    LeftSuperiorLobeAnteriorAlveoli.GetPressure().Set(Ambient.GetNextPressure());
    LeftSuperiorLobeApicoposteriorAlveoli.GetPressure().Set(Ambient.GetNextPressure());

    double TracheaVolume_Percent = 13.40;
    double RightMainBronchusVolume_Percent = 1.42;
    double LeftMainBronchusVolume_Percent = 3.77;
    double RightIntermediateBronchusVolume_Percent = 1.08;
    double RightSuperiorLobarBronchusVolume_Percent = 0.44;
    double RightMiddleLobarBronchusVolume_Percent = 0.30;
    double RightInferiorLobarBronchus1Volume_Percent = 0.17;
    double LeftInferiorLobarBronchus1Volume_Percent = 0.37;
    double LeftSuperiorLobarBronchusVolume_Percent = 0.47;
    double RightInferiorLobarBronchus2Volume_Percent = 0.16;
    double LeftInferiorLobarBronchus2Volume_Percent = 0.40;
    double RightSuperiorApicoposteriorBronchusVolume_Percent = 0.24;
    double RightInferiorLobarBronchus3Volume_Percent = 0.30;
    double LeftInferiorLobarBronchus3Volume_Percent = 0.12;
    double LeftLingularBonchusVolume_Percent = 0.17;
    double LeftSuperiorApicoposteriorBronchusVolume_Percent = 0.39;
    double RightInferiorLobarBronchus4Volume_Percent = 0.19;

    double RightSuperiorLobeApicalBronchioleVolume_Percent = 7.10;
    double RightSuperiorLobePosteriorBronchioleVolume_Percent = 2.67;
    double RightSuperiorLobeAnteriorBronchioleVolume_Percent = 5.73;
    double RightMiddleLobeLateralBronchioleVolume_Percent = 2.22;
    double RightMiddleLobeMedialBronchioleVolume_Percent = 4.81;
    double RightInferiorLobeSuperiorBronchioleVolume_Percent = 4.74;
    double RightInferiorLobeMedialBasalBronchioleVolume_Percent = 1.30;
    double RightInferiorLobeAnteriorBasalBronchioleVolume_Percent = 4.20;
    double RightInferiorLobeLateralBasalBronchioleVolume_Percent = 3.67;
    double RightInferiorLobePosteriorBasalBronchioleVolume_Percent = 5.27;
    double LeftInferiorLobePosteriorBasalBronchioleVolume_Percent = 4.28;
    double LeftInferiorLobeLateralBasalBronchioleVolume_Percent = 4.89;
    double LeftInferiorLobeAnteromedialBasalBronchioleVolume_Percent = 3.21;
    double LeftInferiorLobeSuperiorBronchioleVolume_Percent = 3.90;
    double LeftSuperiorLobeInferiorLingulaBronchioleVolume_Percent = 3.67;
    double LeftSuperiorLobeSuperiorLingulaBronchioleVolume_Percent = 3.06;
    double LeftSuperiorLobeAnteriorBronchioleVolume_Percent = 5.73;
    double LeftSuperiorLobeApicoposteriorBronchioleVolume_Percent = 6.19;

    double RightSuperiorLobeApicalAlveoliVolume_Percent = 9.3;
    double RightSuperiorLobePosteriorAlveoliVolume_Percent = 3.5;
    double RightSuperiorLobeAnteriorAlveoliVolume_Percent = 7.5;
    double RightMiddleLobeLateralAlveoliVolume_Percent = 2.9;
    double RightMiddleLobeMedialAlveoliVolume_Percent = 6.3;
    double RightInferiorLobeSuperiorAlveoliVolume_Percent = 6.2;
    double RightInferiorLobeMedialBasalAlveoliVolume_Percent = 1.7;
    double RightInferiorLobeAnteriorBasalAlveoliVolume_Percent = 5.5;
    double RightInferiorLobeLateralBasalAlveoliVolume_Percent = 4.8;
    double RightInferiorLobePosteriorBasalAlveoliVolume_Percent = 6.9;
    double LeftInferiorLobePosteriorBasalAlveoliVolume_Percent = 5.6;
    double LeftInferiorLobeLateralBasalAlveoliVolume_Percent = 6.4;
    double LeftInferiorLobeAnteromedialBasalAlveoliVolume_Percent = 4.2;
    double LeftInferiorLobeSuperiorAlveoliVolume_Percent = 5.1;
    double LeftSuperiorLobeInferiorLingulaAlveoliVolume_Percent = 4.8;
    double LeftSuperiorLobeSuperiorLingulaAlveoliVolume_Percent = 4.0;
    double LeftSuperiorLobeAnteriorAlveoliVolume_Percent = 7.5;
    double LeftSuperiorLobeApicoposteriorAlveoliVolume_Percent = 8.1;

    double remainingVolume_Percent = 100.0 - TracheaVolume_Percent;
    RightMainBronchus.GetVolumeBaseline().SetValue(RightMainBronchusVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    LeftMainBronchus.GetVolumeBaseline().SetValue(LeftMainBronchusVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    RightIntermediateBronchus.GetVolumeBaseline().SetValue(RightIntermediateBronchusVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    RightSuperiorLobarBronchus.GetVolumeBaseline().SetValue(RightSuperiorLobarBronchusVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    RightMiddleLobarBronchus.GetVolumeBaseline().SetValue(RightMiddleLobarBronchusVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    RightInferiorLobarBronchus1.GetVolumeBaseline().SetValue(RightInferiorLobarBronchus1Volume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    LeftInferiorLobarBronchus1.GetVolumeBaseline().SetValue(LeftInferiorLobarBronchus1Volume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    LeftSuperiorLobarBronchus.GetVolumeBaseline().SetValue(LeftSuperiorLobarBronchusVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    RightInferiorLobarBronchus2.GetVolumeBaseline().SetValue(RightInferiorLobarBronchus2Volume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    LeftInferiorLobarBronchus2.GetVolumeBaseline().SetValue(LeftInferiorLobarBronchus2Volume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    RightSuperiorApicoposteriorBronchus.GetVolumeBaseline().SetValue(RightSuperiorApicoposteriorBronchusVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    RightInferiorLobarBronchus3.GetVolumeBaseline().SetValue(RightInferiorLobarBronchus3Volume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    LeftInferiorLobarBronchus3.GetVolumeBaseline().SetValue(LeftInferiorLobarBronchus3Volume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    LeftLingularBonchus.GetVolumeBaseline().SetValue(LeftLingularBonchusVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    LeftSuperiorApicoposteriorBronchus.GetVolumeBaseline().SetValue(LeftSuperiorApicoposteriorBronchusVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    RightInferiorLobarBronchus4.GetVolumeBaseline().SetValue(RightInferiorLobarBronchus4Volume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);

    RightSuperiorLobeApicalBronchiole.GetVolumeBaseline().SetValue(RightSuperiorLobeApicalBronchioleVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    RightSuperiorLobePosteriorBronchiole.GetVolumeBaseline().SetValue(RightSuperiorLobePosteriorBronchioleVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    RightSuperiorLobeAnteriorBronchiole.GetVolumeBaseline().SetValue(RightSuperiorLobeAnteriorBronchioleVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    RightMiddleLobeLateralBronchiole.GetVolumeBaseline().SetValue(RightMiddleLobeLateralBronchioleVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    RightMiddleLobeMedialBronchiole.GetVolumeBaseline().SetValue(RightMiddleLobeMedialBronchioleVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    RightInferiorLobeSuperiorBronchiole.GetVolumeBaseline().SetValue(RightInferiorLobeSuperiorBronchioleVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    RightInferiorLobeMedialBasalBronchiole.GetVolumeBaseline().SetValue(RightInferiorLobeMedialBasalBronchioleVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    RightInferiorLobeAnteriorBasalBronchiole.GetVolumeBaseline().SetValue(RightInferiorLobeAnteriorBasalBronchioleVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    RightInferiorLobeLateralBasalBronchiole.GetVolumeBaseline().SetValue(RightInferiorLobeLateralBasalBronchioleVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    RightInferiorLobePosteriorBasalBronchiole.GetVolumeBaseline().SetValue(RightInferiorLobePosteriorBasalBronchioleVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    LeftInferiorLobePosteriorBasalBronchiole.GetVolumeBaseline().SetValue(LeftInferiorLobePosteriorBasalBronchioleVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    LeftInferiorLobeLateralBasalBronchiole.GetVolumeBaseline().SetValue(LeftInferiorLobeLateralBasalBronchioleVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    LeftInferiorLobeAnteromedialBasalBronchiole.GetVolumeBaseline().SetValue(LeftInferiorLobeAnteromedialBasalBronchioleVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    LeftInferiorLobeSuperiorBronchiole.GetVolumeBaseline().SetValue(LeftInferiorLobeSuperiorBronchioleVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    LeftSuperiorLobeInferiorLingulaBronchiole.GetVolumeBaseline().SetValue(LeftSuperiorLobeInferiorLingulaBronchioleVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    LeftSuperiorLobeSuperiorLingulaBronchiole.GetVolumeBaseline().SetValue(LeftSuperiorLobeSuperiorLingulaBronchioleVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    LeftSuperiorLobeAnteriorBronchiole.GetVolumeBaseline().SetValue(LeftSuperiorLobeAnteriorBronchioleVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);
    LeftSuperiorLobeApicoposteriorBronchiole.GetVolumeBaseline().SetValue(LeftSuperiorLobeApicoposteriorBronchioleVolume_Percent / remainingVolume_Percent * anatomicDeadSpaceVolume_L, VolumeUnit::L);

    RightSuperiorLobeApicalAlveoli.GetVolumeBaseline().SetValue(RightSuperiorLobeApicalAlveoliVolume_Percent / 100.0 * alveoliVolume_L, VolumeUnit::L);
    RightSuperiorLobePosteriorAlveoli.GetVolumeBaseline().SetValue(RightSuperiorLobePosteriorAlveoliVolume_Percent / 100.0 * alveoliVolume_L, VolumeUnit::L);
    RightSuperiorLobeAnteriorAlveoli.GetVolumeBaseline().SetValue(RightSuperiorLobeAnteriorAlveoliVolume_Percent / 100.0 * alveoliVolume_L, VolumeUnit::L);
    RightMiddleLobeLateralAlveoli.GetVolumeBaseline().SetValue(RightMiddleLobeLateralAlveoliVolume_Percent / 100.0 * alveoliVolume_L, VolumeUnit::L);
    RightMiddleLobeMedialAlveoli.GetVolumeBaseline().SetValue(RightMiddleLobeMedialAlveoliVolume_Percent / 100.0 * alveoliVolume_L, VolumeUnit::L);
    RightInferiorLobeSuperiorAlveoli.GetVolumeBaseline().SetValue(RightInferiorLobeSuperiorAlveoliVolume_Percent / 100.0 * alveoliVolume_L, VolumeUnit::L);
    RightInferiorLobeMedialBasalAlveoli.GetVolumeBaseline().SetValue(RightInferiorLobeMedialBasalAlveoliVolume_Percent / 100.0 * alveoliVolume_L, VolumeUnit::L);
    RightInferiorLobeAnteriorBasalAlveoli.GetVolumeBaseline().SetValue(RightInferiorLobeAnteriorBasalAlveoliVolume_Percent / 100.0 * alveoliVolume_L, VolumeUnit::L);
    RightInferiorLobeLateralBasalAlveoli.GetVolumeBaseline().SetValue(RightInferiorLobeLateralBasalAlveoliVolume_Percent / 100.0 * alveoliVolume_L, VolumeUnit::L);
    RightInferiorLobePosteriorBasalAlveoli.GetVolumeBaseline().SetValue(RightInferiorLobePosteriorBasalAlveoliVolume_Percent / 100.0 * alveoliVolume_L, VolumeUnit::L);
    LeftInferiorLobePosteriorBasalAlveoli.GetVolumeBaseline().SetValue(LeftInferiorLobePosteriorBasalAlveoliVolume_Percent / 100.0 * alveoliVolume_L, VolumeUnit::L);
    LeftInferiorLobeLateralBasalAlveoli.GetVolumeBaseline().SetValue(LeftInferiorLobeLateralBasalAlveoliVolume_Percent / 100.0 * alveoliVolume_L, VolumeUnit::L);
    LeftInferiorLobeAnteromedialBasalAlveoli.GetVolumeBaseline().SetValue(LeftInferiorLobeAnteromedialBasalAlveoliVolume_Percent / 100.0 * alveoliVolume_L, VolumeUnit::L);
    LeftInferiorLobeSuperiorAlveoli.GetVolumeBaseline().SetValue(LeftInferiorLobeSuperiorAlveoliVolume_Percent / 100.0 * alveoliVolume_L, VolumeUnit::L);
    LeftSuperiorLobeInferiorLingulaAlveoli.GetVolumeBaseline().SetValue(LeftSuperiorLobeInferiorLingulaAlveoliVolume_Percent / 100.0 * alveoliVolume_L, VolumeUnit::L);
    LeftSuperiorLobeSuperiorLingulaAlveoli.GetVolumeBaseline().SetValue(LeftSuperiorLobeSuperiorLingulaAlveoliVolume_Percent / 100.0 * alveoliVolume_L, VolumeUnit::L);
    LeftSuperiorLobeAnteriorAlveoli.GetVolumeBaseline().SetValue(LeftSuperiorLobeAnteriorAlveoliVolume_Percent / 100.0 * alveoliVolume_L, VolumeUnit::L);
    LeftSuperiorLobeApicoposteriorAlveoli.GetVolumeBaseline().SetValue(LeftSuperiorLobeApicoposteriorAlveoliVolume_Percent / 100.0 * alveoliVolume_L, VolumeUnit::L);

    RightSuperiorLobeApicalAlveolarDeadSpace.GetVolumeBaseline().SetValue(alveolarDeadSpaceVolume_L / 18.0, VolumeUnit::L);
    RightSuperiorLobePosteriorAlveolarDeadSpace.GetVolumeBaseline().SetValue(alveolarDeadSpaceVolume_L / 18.0, VolumeUnit::L);
    RightSuperiorLobeAnteriorAlveolarDeadSpace.GetVolumeBaseline().SetValue(alveolarDeadSpaceVolume_L / 18.0, VolumeUnit::L);
    RightMiddleLobeLateralAlveolarDeadSpace.GetVolumeBaseline().SetValue(alveolarDeadSpaceVolume_L / 18.0, VolumeUnit::L);
    RightMiddleLobeMedialAlveolarDeadSpace.GetVolumeBaseline().SetValue(alveolarDeadSpaceVolume_L / 18.0, VolumeUnit::L);
    RightInferiorLobeSuperiorAlveolarDeadSpace.GetVolumeBaseline().SetValue(alveolarDeadSpaceVolume_L / 18.0, VolumeUnit::L);
    RightInferiorLobeMedialBasalAlveolarDeadSpace.GetVolumeBaseline().SetValue(alveolarDeadSpaceVolume_L / 18.0, VolumeUnit::L);
    RightInferiorLobeAnteriorBasalAlveolarDeadSpace.GetVolumeBaseline().SetValue(alveolarDeadSpaceVolume_L / 18.0, VolumeUnit::L);
    RightInferiorLobeLateralBasalAlveolarDeadSpace.GetVolumeBaseline().SetValue(alveolarDeadSpaceVolume_L / 18.0, VolumeUnit::L);
    RightInferiorLobePosteriorBasalAlveolarDeadSpace.GetVolumeBaseline().SetValue(alveolarDeadSpaceVolume_L / 18.0, VolumeUnit::L);
    LeftInferiorLobePosteriorBasalAlveolarDeadSpace.GetVolumeBaseline().SetValue(alveolarDeadSpaceVolume_L / 18.0, VolumeUnit::L);
    LeftInferiorLobeLateralBasalAlveolarDeadSpace.GetVolumeBaseline().SetValue(alveolarDeadSpaceVolume_L / 18.0, VolumeUnit::L);
    LeftInferiorLobeAnteromedialBasalAlveolarDeadSpace.GetVolumeBaseline().SetValue(alveolarDeadSpaceVolume_L / 18.0, VolumeUnit::L);
    LeftInferiorLobeSuperiorAlveolarDeadSpace.GetVolumeBaseline().SetValue(alveolarDeadSpaceVolume_L / 18.0, VolumeUnit::L);
    LeftSuperiorLobeInferiorLingulaAlveolarDeadSpace.GetVolumeBaseline().SetValue(alveolarDeadSpaceVolume_L / 18.0, VolumeUnit::L);
    LeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace.GetVolumeBaseline().SetValue(alveolarDeadSpaceVolume_L / 18.0, VolumeUnit::L);
    LeftSuperiorLobeAnteriorAlveolarDeadSpace.GetVolumeBaseline().SetValue(alveolarDeadSpaceVolume_L / 18.0, VolumeUnit::L);
    LeftSuperiorLobeApicoposteriorAlveolarDeadSpace.GetVolumeBaseline().SetValue(alveolarDeadSpaceVolume_L / 18.0, VolumeUnit::L);

    // Airway
    SEFluidCircuitNode& Airway = cRespiratory.CreateNode(pulse::RespiratoryNode::Airway);
    Airway.GetPressure().Set(Ambient.GetNextPressure());
    Airway.GetVolumeBaseline().SetValue(0.0206, VolumeUnit::L);
    // Pharynx
    SEFluidCircuitNode& Pharynx = cRespiratory.CreateNode(pulse::RespiratoryNode::Pharynx);
    Pharynx.GetPressure().Set(Ambient.GetNextPressure());
    Pharynx.GetVolumeBaseline().SetValue(0.001, VolumeUnit::L);
    // Carina
    SEFluidCircuitNode& Carina = cRespiratory.CreateNode(pulse::RespiratoryNode::Carina);
    Carina.GetPressure().Set(Ambient.GetNextPressure());
    Carina.GetVolumeBaseline().SetValue(TracheaVolume_Percent / 100.0 * anatomicDeadSpaceVolume_L, VolumeUnit::L); //Trachea Volume

    // Right Pleural Connection - no volume, so it doesn't get modified by compliances
    SEFluidCircuitNode& RightPleuralConnection = cRespiratory.CreateNode(pulse::RespiratoryNode::RightPleuralConnection);
    RightPleuralConnection.GetPressure().SetValue(Ambient.GetNextPressure(PressureUnit::cmH2O) + IntrapleuralPressure_cmH2O, PressureUnit::cmH2O);
    // Left Pleural Connection - no volume, so it doesn't get modified by compliances
    SEFluidCircuitNode& LeftPleuralConnection = cRespiratory.CreateNode(pulse::RespiratoryNode::LeftPleuralConnection);
    LeftPleuralConnection.GetPressure().SetValue(Ambient.GetNextPressure(PressureUnit::cmH2O) + IntrapleuralPressure_cmH2O, PressureUnit::cmH2O);
    // Right Pleural
    SEFluidCircuitNode& RightPleural = cRespiratory.CreateNode(pulse::RespiratoryNode::RightPleural);
    RightPleural.GetPressure().SetValue(Ambient.GetNextPressure(PressureUnit::cmH2O) + IntrapleuralPressure_cmH2O, PressureUnit::cmH2O);
    RightPleural.GetVolumeBaseline().SetValue(RightLungRatio * pleuralVolume_L, VolumeUnit::L);
    // Left Pleural 
    SEFluidCircuitNode& LeftPleural = cRespiratory.CreateNode(pulse::RespiratoryNode::LeftPleural);
    LeftPleural.GetPressure().SetValue(Ambient.GetNextPressure(PressureUnit::cmH2O) + IntrapleuralPressure_cmH2O, PressureUnit::cmH2O);
    LeftPleural.GetVolumeBaseline().SetValue(LeftLungRatio * pleuralVolume_L, VolumeUnit::L);
    // Respiratory Muscle - corresponds to a node representing the inspiratory muscles, particularly diaphragm 
    SEFluidCircuitNode& RespiratoryMuscle = cRespiratory.CreateNode(pulse::RespiratoryNode::RespiratoryMuscle);
    RespiratoryMuscle.GetPressure().Set(Ambient.GetNextPressure());
    // Stomach
    SEFluidCircuitNode& Stomach = cRespiratory.CreateNode(pulse::RespiratoryNode::Stomach);
    Stomach.GetPressure().Set(Ambient.GetNextPressure());
    Stomach.GetVolumeBaseline().SetValue(0.1, VolumeUnit::L);

    //-------------------------------------------------------------------------------------------------------------------------------------
    // Environment to Airway connections, the path has no element.
    double TracheaResistance_cmH2O_s_Per_L = 0.8;// Tuned from 1.125;
    SEFluidCircuitPath& EnvironmentToAirway = cRespiratory.CreatePath(Ambient, Airway, pulse::RespiratoryPath::EnvironmentToAirway);
    SEFluidCircuitPath& AirwayToPharynx = cRespiratory.CreatePath(Airway, Pharynx, pulse::RespiratoryPath::AirwayToPharynx);
    SEFluidCircuitPath& PharynxToCarina = cRespiratory.CreatePath(Pharynx, Carina, pulse::RespiratoryPath::PharynxToCarina);
    PharynxToCarina.GetResistanceBaseline().SetValue(TracheaResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    SEFluidCircuitPath& PharynxToEnvironment = cRespiratory.CreatePath(Pharynx, Ambient, pulse::RespiratoryPath::PharynxToEnvironment);
    PharynxToEnvironment.GetResistanceBaseline().SetValue(openResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);

    SEFluidCircuitPath& CarinaToRightMainBronchus = cRespiratory.CreatePath(Carina, RightMainBronchus, pulse::ExpandedLungsRespiratoryPath::CarinaToRightMainBronchus);
    SEFluidCircuitPath& RightMainBronchusToRightIntermediateBronchus = cRespiratory.CreatePath(RightMainBronchus, RightIntermediateBronchus, pulse::ExpandedLungsRespiratoryPath::RightMainBronchusToRightIntermediateBronchus);
    SEFluidCircuitPath& RightMainBronchusToRightSuperiorLobarBronchus = cRespiratory.CreatePath(RightMainBronchus, RightSuperiorLobarBronchus, pulse::ExpandedLungsRespiratoryPath::RightMainBronchusToRightSuperiorLobarBronchus);
    SEFluidCircuitPath& RightIntermediateBronchusToRightMiddleLobarBronchus = cRespiratory.CreatePath(RightIntermediateBronchus, RightMiddleLobarBronchus, pulse::ExpandedLungsRespiratoryPath::RightIntermediateBronchusToRightMiddleLobarBronchus);
    SEFluidCircuitPath& RightIntermediateBronchusToRightInferiorLobarBronchus1 = cRespiratory.CreatePath(RightIntermediateBronchus, RightInferiorLobarBronchus1, pulse::ExpandedLungsRespiratoryPath::RightIntermediateBronchusToRightInferiorLobarBronchus1);
    SEFluidCircuitPath& RightInferiorLobarBronchus1ToRightInferiorLobarBronchus2 = cRespiratory.CreatePath(RightInferiorLobarBronchus1, RightInferiorLobarBronchus2, pulse::ExpandedLungsRespiratoryPath::RightInferiorLobarBronchus1ToRightInferiorLobarBronchus2);
    SEFluidCircuitPath& RightSuperiorLobarBronchusToRightSuperiorApicoposteriorBronchus = cRespiratory.CreatePath(RightSuperiorLobarBronchus, RightSuperiorApicoposteriorBronchus, pulse::ExpandedLungsRespiratoryPath::RightSuperiorLobarBronchusToRightSuperiorApicoposteriorBronchus);
    SEFluidCircuitPath& RightInferiorLobarBronchus2ToRightInferiorLobarBronchus3 = cRespiratory.CreatePath(RightInferiorLobarBronchus2, RightInferiorLobarBronchus3, pulse::ExpandedLungsRespiratoryPath::RightInferiorLobarBronchus2ToRightInferiorLobarBronchus3);
    SEFluidCircuitPath& RightInferiorLobarBronchus2ToRightInferiorLobarBronchus4 = cRespiratory.CreatePath(RightInferiorLobarBronchus2, RightInferiorLobarBronchus4, pulse::ExpandedLungsRespiratoryPath::RightInferiorLobarBronchus2ToRightInferiorLobarBronchus4);

    SEFluidCircuitPath& CarinaToLeftMainBronchus = cRespiratory.CreatePath(Carina, LeftMainBronchus, pulse::ExpandedLungsRespiratoryPath::CarinaToLeftMainBronchus);
    SEFluidCircuitPath& LeftMainBronchusToLeftInferiorLobarBronchus1 = cRespiratory.CreatePath(LeftMainBronchus, LeftInferiorLobarBronchus1, pulse::ExpandedLungsRespiratoryPath::LeftMainBronchusToLeftInferiorLobarBronchus1);
    SEFluidCircuitPath& LeftMainBronchusToLeftSuperiorLobarBronchus = cRespiratory.CreatePath(LeftMainBronchus, LeftSuperiorLobarBronchus, pulse::ExpandedLungsRespiratoryPath::LeftMainBronchusToLeftSuperiorLobarBronchus);
    SEFluidCircuitPath& LeftInferiorLobarBronchus1ToLeftInferiorLobarBronchus2 = cRespiratory.CreatePath(LeftInferiorLobarBronchus1, LeftInferiorLobarBronchus2, pulse::ExpandedLungsRespiratoryPath::LeftInferiorLobarBronchus1ToLeftInferiorLobarBronchus2);
    SEFluidCircuitPath& LeftInferiorLobarBronchus2ToLeftInferiorLobarBronchus3 = cRespiratory.CreatePath(LeftInferiorLobarBronchus2, LeftInferiorLobarBronchus3, pulse::ExpandedLungsRespiratoryPath::LeftInferiorLobarBronchus2ToLeftInferiorLobarBronchus3);
    SEFluidCircuitPath& LeftSuperiorLobarBronchusToLeftLingularBonchus = cRespiratory.CreatePath(LeftSuperiorLobarBronchus, LeftLingularBonchus, pulse::ExpandedLungsRespiratoryPath::LeftSuperiorLobarBronchusToLeftLingularBonchus);
    SEFluidCircuitPath& LeftSuperiorLobarBronchusToLeftSuperiorApicoposteriorBronchus = cRespiratory.CreatePath(LeftSuperiorLobarBronchus, LeftSuperiorApicoposteriorBronchus, pulse::ExpandedLungsRespiratoryPath::LeftSuperiorLobarBronchusToLeftSuperiorApicoposteriorBronchus);

    SEFluidCircuitPath& RightSuperiorApicoposteriorBronchusToRightSuperiorLobeApicalBronchiole = cRespiratory.CreatePath(RightSuperiorApicoposteriorBronchus, RightSuperiorLobeApicalBronchiole, pulse::ExpandedLungsRespiratoryPath::RightSuperiorApicoposteriorBronchusToRightSuperiorLobeApicalBronchiole);
    SEFluidCircuitPath& RightSuperiorApicoposteriorBronchusToRightSuperiorLobePosteriorBronchiole = cRespiratory.CreatePath(RightSuperiorApicoposteriorBronchus, RightSuperiorLobePosteriorBronchiole, pulse::ExpandedLungsRespiratoryPath::RightSuperiorApicoposteriorBronchusToRightSuperiorLobePosteriorBronchiole);
    SEFluidCircuitPath& RightSuperiorLobarBronchusToRightSuperiorLobeAnteriorBronchiole = cRespiratory.CreatePath(RightSuperiorLobarBronchus, RightSuperiorLobeAnteriorBronchiole, pulse::ExpandedLungsRespiratoryPath::RightSuperiorLobarBronchusToRightSuperiorLobeAnteriorBronchiole);
    SEFluidCircuitPath& RightMiddleLobarBronchusToRightMiddleLobeLateralBronchiole = cRespiratory.CreatePath(RightMiddleLobarBronchus, RightMiddleLobeLateralBronchiole, pulse::ExpandedLungsRespiratoryPath::RightMiddleLobarBronchusToRightMiddleLobeLateralBronchiole);
    SEFluidCircuitPath& RightMiddleLobarBronchusToRightMiddleLobeMedialBronchiole = cRespiratory.CreatePath(RightMiddleLobarBronchus, RightMiddleLobeMedialBronchiole, pulse::ExpandedLungsRespiratoryPath::RightMiddleLobarBronchusToRightMiddleLobeMedialBronchiole);
    SEFluidCircuitPath& RightInferiorLobarBronchus1ToRightInferiorLobeSuperiorBronchiole = cRespiratory.CreatePath(RightInferiorLobarBronchus1, RightInferiorLobeSuperiorBronchiole, pulse::ExpandedLungsRespiratoryPath::RightInferiorLobarBronchus1ToRightInferiorLobeSuperiorBronchiole);
    SEFluidCircuitPath& RightInferiorLobarBronchus2ToRightInferiorLobeMedialBasalBronchiole = cRespiratory.CreatePath(RightInferiorLobarBronchus2, RightInferiorLobeMedialBasalBronchiole, pulse::ExpandedLungsRespiratoryPath::RightInferiorLobarBronchus2ToRightInferiorLobeMedialBasalBronchiole);
    SEFluidCircuitPath& RightInferiorLobarBronchus3ToRightInferiorLobeAnteriorBasalBronchiole = cRespiratory.CreatePath(RightInferiorLobarBronchus3, RightInferiorLobeAnteriorBasalBronchiole, pulse::ExpandedLungsRespiratoryPath::RightInferiorLobarBronchus3ToRightInferiorLobeAnteriorBasalBronchiole);
    SEFluidCircuitPath& RightInferiorLobarBronchus4ToRightInferiorLobeLateralBasalBronchiole = cRespiratory.CreatePath(RightInferiorLobarBronchus4, RightInferiorLobeLateralBasalBronchiole, pulse::ExpandedLungsRespiratoryPath::RightInferiorLobarBronchus4ToRightInferiorLobeLateralBasalBronchiole);
    SEFluidCircuitPath& RightInferiorLobarBronchus4ToRightInferiorLobePosteriorBasalBronchiole = cRespiratory.CreatePath(RightInferiorLobarBronchus4, RightInferiorLobePosteriorBasalBronchiole, pulse::ExpandedLungsRespiratoryPath::RightInferiorLobarBronchus4ToRightInferiorLobePosteriorBasalBronchiole);

    SEFluidCircuitPath& LeftInferiorLobarBronchus3ToLeftInferiorLobeLateralBasalBronchiole = cRespiratory.CreatePath(LeftInferiorLobarBronchus3, LeftInferiorLobeLateralBasalBronchiole, pulse::ExpandedLungsRespiratoryPath::LeftInferiorLobarBronchus3ToLeftInferiorLobeLateralBasalBronchiole);
    SEFluidCircuitPath& LeftInferiorLobarBronchus3ToLeftInferiorLobePosteriorBasalBronchiole = cRespiratory.CreatePath(LeftInferiorLobarBronchus3, LeftInferiorLobePosteriorBasalBronchiole, pulse::ExpandedLungsRespiratoryPath::LeftInferiorLobarBronchus3ToLeftInferiorLobePosteriorBasalBronchiole);
    SEFluidCircuitPath& LeftInferiorLobarBronchus2ToLeftInferiorLobeAnteromedialBasalBronchiole = cRespiratory.CreatePath(LeftInferiorLobarBronchus2, LeftInferiorLobeAnteromedialBasalBronchiole, pulse::ExpandedLungsRespiratoryPath::LeftInferiorLobarBronchus2ToLeftInferiorLobeAnteromedialBasalBronchiole);
    SEFluidCircuitPath& LeftInferiorLobarBronchus1ToLeftInferiorLobeSuperiorBronchiole = cRespiratory.CreatePath(LeftInferiorLobarBronchus1, LeftInferiorLobeSuperiorBronchiole, pulse::ExpandedLungsRespiratoryPath::LeftInferiorLobarBronchus1ToLeftInferiorLobeSuperiorBronchiole);
    SEFluidCircuitPath& LeftLingularBonchusToLeftSuperiorLobeInferiorLingulaBronchiole = cRespiratory.CreatePath(LeftLingularBonchus, LeftSuperiorLobeInferiorLingulaBronchiole, pulse::ExpandedLungsRespiratoryPath::LeftLingularBonchusToLeftSuperiorLobeInferiorLingulaBronchiole);
    SEFluidCircuitPath& LeftLingularBonchusToLeftSuperiorLobeSuperiorLingulaBronchiole = cRespiratory.CreatePath(LeftLingularBonchus, LeftSuperiorLobeSuperiorLingulaBronchiole, pulse::ExpandedLungsRespiratoryPath::LeftLingularBonchusToLeftSuperiorLobeSuperiorLingulaBronchiole);
    SEFluidCircuitPath& LeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeAnteriorBronchiole = cRespiratory.CreatePath(LeftSuperiorApicoposteriorBronchus, LeftSuperiorLobeAnteriorBronchiole, pulse::ExpandedLungsRespiratoryPath::LeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeAnteriorBronchiole);
    SEFluidCircuitPath& LeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeApicoposteriorBronchiole = cRespiratory.CreatePath(LeftSuperiorApicoposteriorBronchus, LeftSuperiorLobeApicoposteriorBronchiole, pulse::ExpandedLungsRespiratoryPath::LeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeApicoposteriorBronchiole);

    SEFluidCircuitPath& RightSuperiorLobeApicalBronchioleToRightSuperiorLobeApicalAlveolarDeadSpace = cRespiratory.CreatePath(RightSuperiorLobeApicalBronchiole, RightSuperiorLobeApicalAlveolarDeadSpace, pulse::ExpandedLungsRespiratoryPath::RightSuperiorLobeApicalBronchioleToRightSuperiorLobeApicalAlveolarDeadSpace);
    SEFluidCircuitPath& RightSuperiorLobePosteriorBronchioleToRightSuperiorLobePosteriorAlveolarDeadSpace = cRespiratory.CreatePath(RightSuperiorLobePosteriorBronchiole, RightSuperiorLobePosteriorAlveolarDeadSpace, pulse::ExpandedLungsRespiratoryPath::RightSuperiorLobePosteriorBronchioleToRightSuperiorLobePosteriorAlveolarDeadSpace);
    SEFluidCircuitPath& RightSuperiorLobeAnteriorBronchioleToRightSuperiorLobeAnteriorAlveolarDeadSpace = cRespiratory.CreatePath(RightSuperiorLobeAnteriorBronchiole, RightSuperiorLobeAnteriorAlveolarDeadSpace, pulse::ExpandedLungsRespiratoryPath::RightSuperiorLobeAnteriorBronchioleToRightSuperiorLobeAnteriorAlveolarDeadSpace);
    SEFluidCircuitPath& RightMiddleLobeLateralBronchioleToRightMiddleLobeLateralAlveolarDeadSpace = cRespiratory.CreatePath(RightMiddleLobeLateralBronchiole, RightMiddleLobeLateralAlveolarDeadSpace, pulse::ExpandedLungsRespiratoryPath::RightMiddleLobeLateralBronchioleToRightMiddleLobeLateralAlveolarDeadSpace);
    SEFluidCircuitPath& RightMiddleLobeMedialBronchioleToRightMiddleLobeMedialAlveolarDeadSpace = cRespiratory.CreatePath(RightMiddleLobeMedialBronchiole, RightMiddleLobeMedialAlveolarDeadSpace, pulse::ExpandedLungsRespiratoryPath::RightMiddleLobeMedialBronchioleToRightMiddleLobeMedialAlveolarDeadSpace);
    SEFluidCircuitPath& RightInferiorLobeSuperiorBronchioleToRightInferiorLobeSuperiorAlveolarDeadSpace = cRespiratory.CreatePath(RightInferiorLobeSuperiorBronchiole, RightInferiorLobeSuperiorAlveolarDeadSpace, pulse::ExpandedLungsRespiratoryPath::RightInferiorLobeSuperiorBronchioleToRightInferiorLobeSuperiorAlveolarDeadSpace);
    SEFluidCircuitPath& RightInferiorLobeMedialBasalBronchioleToRightInferiorLobeMedialBasalAlveolarDeadSpace = cRespiratory.CreatePath(RightInferiorLobeMedialBasalBronchiole, RightInferiorLobeMedialBasalAlveolarDeadSpace, pulse::ExpandedLungsRespiratoryPath::RightInferiorLobeMedialBasalBronchioleToRightInferiorLobeMedialBasalAlveolarDeadSpace);
    SEFluidCircuitPath& RightInferiorLobeAnteriorBasalBronchioleToRightInferiorLobeAnteriorBasalAlveolarDeadSpace = cRespiratory.CreatePath(RightInferiorLobeAnteriorBasalBronchiole, RightInferiorLobeAnteriorBasalAlveolarDeadSpace, pulse::ExpandedLungsRespiratoryPath::RightInferiorLobeAnteriorBasalBronchioleToRightInferiorLobeAnteriorBasalAlveolarDeadSpace);
    SEFluidCircuitPath& RightInferiorLobeLateralBasalBronchioleToRightInferiorLobeLateralBasalAlveolarDeadSpace = cRespiratory.CreatePath(RightInferiorLobeLateralBasalBronchiole, RightInferiorLobeLateralBasalAlveolarDeadSpace, pulse::ExpandedLungsRespiratoryPath::RightInferiorLobeLateralBasalBronchioleToRightInferiorLobeLateralBasalAlveolarDeadSpace);
    SEFluidCircuitPath& RightInferiorLobePosteriorBasalBronchioleToRightInferiorLobePosteriorBasalAlveolarDeadSpace = cRespiratory.CreatePath(RightInferiorLobePosteriorBasalBronchiole, RightInferiorLobePosteriorBasalAlveolarDeadSpace, pulse::ExpandedLungsRespiratoryPath::RightInferiorLobePosteriorBasalBronchioleToRightInferiorLobePosteriorBasalAlveolarDeadSpace);

    SEFluidCircuitPath& LeftInferiorLobePosteriorBasalBronchioleToLeftInferiorLobePosteriorBasalAlveolarDeadSpace = cRespiratory.CreatePath(LeftInferiorLobePosteriorBasalBronchiole, LeftInferiorLobePosteriorBasalAlveolarDeadSpace, pulse::ExpandedLungsRespiratoryPath::LeftInferiorLobePosteriorBasalBronchioleToLeftInferiorLobePosteriorBasalAlveolarDeadSpace);
    SEFluidCircuitPath& LeftInferiorLobeLateralBasalBronchioleToLeftInferiorLobeLateralBasalAlveolarDeadSpace = cRespiratory.CreatePath(LeftInferiorLobeLateralBasalBronchiole, LeftInferiorLobeLateralBasalAlveolarDeadSpace, pulse::ExpandedLungsRespiratoryPath::LeftInferiorLobeLateralBasalBronchioleToLeftInferiorLobeLateralBasalAlveolarDeadSpace);
    SEFluidCircuitPath& LeftInferiorLobeAnteromedialBasalBronchioleToLeftInferiorLobeAnteromedialBasalAlveolarDeadSpace = cRespiratory.CreatePath(LeftInferiorLobeAnteromedialBasalBronchiole, LeftInferiorLobeAnteromedialBasalAlveolarDeadSpace, pulse::ExpandedLungsRespiratoryPath::LeftInferiorLobeAnteromedialBasalBronchioleToLeftInferiorLobeAnteromedialBasalAlveolarDeadSpace);
    SEFluidCircuitPath& LeftInferiorLobeSuperiorBronchioleToLeftInferiorLobeSuperiorAlveolarDeadSpace = cRespiratory.CreatePath(LeftInferiorLobeSuperiorBronchiole, LeftInferiorLobeSuperiorAlveolarDeadSpace, pulse::ExpandedLungsRespiratoryPath::LeftInferiorLobeSuperiorBronchioleToLeftInferiorLobeSuperiorAlveolarDeadSpace);
    SEFluidCircuitPath& LeftSuperiorLobeInferiorLingulaBronchioleToLeftSuperiorLobeInferiorLingulaAlveolarDeadSpace = cRespiratory.CreatePath(LeftSuperiorLobeInferiorLingulaBronchiole, LeftSuperiorLobeInferiorLingulaAlveolarDeadSpace, pulse::ExpandedLungsRespiratoryPath::LeftSuperiorLobeInferiorLingulaBronchioleToLeftSuperiorLobeInferiorLingulaAlveolarDeadSpace);
    SEFluidCircuitPath& LeftSuperiorLobeSuperiorLingulaBronchioleToLeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace = cRespiratory.CreatePath(LeftSuperiorLobeSuperiorLingulaBronchiole, LeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace, pulse::ExpandedLungsRespiratoryPath::LeftSuperiorLobeSuperiorLingulaBronchioleToLeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace);
    SEFluidCircuitPath& LeftSuperiorLobeAnteriorBronchioleToLeftSuperiorLobeAnteriorAlveolarDeadSpace = cRespiratory.CreatePath(LeftSuperiorLobeAnteriorBronchiole, LeftSuperiorLobeAnteriorAlveolarDeadSpace, pulse::ExpandedLungsRespiratoryPath::LeftSuperiorLobeAnteriorBronchioleToLeftSuperiorLobeAnteriorAlveolarDeadSpace);
    SEFluidCircuitPath& LeftSuperiorLobeApicoposteriorBronchioleToLeftSuperiorLobeApicoposteriorAlveolarDeadSpace = cRespiratory.CreatePath(LeftSuperiorLobeApicoposteriorBronchiole, LeftSuperiorLobeApicoposteriorAlveolarDeadSpace, pulse::ExpandedLungsRespiratoryPath::LeftSuperiorLobeApicoposteriorBronchioleToLeftSuperiorLobeApicoposteriorAlveolarDeadSpace);

    SEFluidCircuitPath& RightSuperiorLobeApicalAlveolarDeadSpaceToRightSuperiorLobeApicalAlveoli = cRespiratory.CreatePath(RightSuperiorLobeApicalAlveolarDeadSpace, RightSuperiorLobeApicalAlveoli, pulse::ExpandedLungsRespiratoryPath::RightSuperiorLobeApicalAlveolarDeadSpaceToRightSuperiorLobeApicalAlveoli);
    SEFluidCircuitPath& RightSuperiorLobePosteriorAlveolarDeadSpaceToRightSuperiorLobePosteriorAlveoli = cRespiratory.CreatePath(RightSuperiorLobePosteriorAlveolarDeadSpace, RightSuperiorLobePosteriorAlveoli, pulse::ExpandedLungsRespiratoryPath::RightSuperiorLobePosteriorAlveolarDeadSpaceToRightSuperiorLobePosteriorAlveoli);
    SEFluidCircuitPath& RightSuperiorLobeAnteriorAlveolarDeadSpaceToRightSuperiorLobeAnteriorAlveoli = cRespiratory.CreatePath(RightSuperiorLobeAnteriorAlveolarDeadSpace, RightSuperiorLobeAnteriorAlveoli, pulse::ExpandedLungsRespiratoryPath::RightSuperiorLobeAnteriorAlveolarDeadSpaceToRightSuperiorLobeAnteriorAlveoli);
    SEFluidCircuitPath& RightMiddleLobeLateralAlveolarDeadSpaceToRightMiddleLobeLateralAlveoli = cRespiratory.CreatePath(RightMiddleLobeLateralAlveolarDeadSpace, RightMiddleLobeLateralAlveoli, pulse::ExpandedLungsRespiratoryPath::RightMiddleLobeLateralAlveolarDeadSpaceToRightMiddleLobeLateralAlveoli);
    SEFluidCircuitPath& RightMiddleLobeMedialAlveolarDeadSpaceToRightMiddleLobeMedialAlveoli = cRespiratory.CreatePath(RightMiddleLobeMedialAlveolarDeadSpace, RightMiddleLobeMedialAlveoli, pulse::ExpandedLungsRespiratoryPath::RightMiddleLobeMedialAlveolarDeadSpaceToRightMiddleLobeMedialAlveoli);
    SEFluidCircuitPath& RightInferiorLobeSuperiorAlveolarDeadSpaceToRightInferiorLobeSuperiorAlveoli = cRespiratory.CreatePath(RightInferiorLobeSuperiorAlveolarDeadSpace, RightInferiorLobeSuperiorAlveoli, pulse::ExpandedLungsRespiratoryPath::RightInferiorLobeSuperiorAlveolarDeadSpaceToRightInferiorLobeSuperiorAlveoli);
    SEFluidCircuitPath& RightInferiorLobeMedialBasalAlveolarDeadSpaceToRightInferiorLobeMedialBasalAlveoli = cRespiratory.CreatePath(RightInferiorLobeMedialBasalAlveolarDeadSpace, RightInferiorLobeMedialBasalAlveoli, pulse::ExpandedLungsRespiratoryPath::RightInferiorLobeMedialBasalAlveolarDeadSpaceToRightInferiorLobeMedialBasalAlveoli);
    SEFluidCircuitPath& RightInferiorLobeAnteriorBasalAlveolarDeadSpaceToRightInferiorLobeAnteriorBasalAlveoli = cRespiratory.CreatePath(RightInferiorLobeAnteriorBasalAlveolarDeadSpace, RightInferiorLobeAnteriorBasalAlveoli, pulse::ExpandedLungsRespiratoryPath::RightInferiorLobeAnteriorBasalAlveolarDeadSpaceToRightInferiorLobeAnteriorBasalAlveoli);
    SEFluidCircuitPath& RightInferiorLobeLateralBasalAlveolarDeadSpaceToRightInferiorLobeLateralBasalAlveoli = cRespiratory.CreatePath(RightInferiorLobeLateralBasalAlveolarDeadSpace, RightInferiorLobeLateralBasalAlveoli, pulse::ExpandedLungsRespiratoryPath::RightInferiorLobeLateralBasalAlveolarDeadSpaceToRightInferiorLobeLateralBasalAlveoli);
    SEFluidCircuitPath& RightInferiorLobePosteriorBasalAlveolarDeadSpaceToRightInferiorLobePosteriorBasalAlveoli = cRespiratory.CreatePath(RightInferiorLobePosteriorBasalAlveolarDeadSpace, RightInferiorLobePosteriorBasalAlveoli, pulse::ExpandedLungsRespiratoryPath::RightInferiorLobePosteriorBasalAlveolarDeadSpaceToRightInferiorLobePosteriorBasalAlveoli);

    SEFluidCircuitPath& LeftInferiorLobePosteriorBasalAlveolarDeadSpaceToLeftInferiorLobePosteriorBasalAlveoli = cRespiratory.CreatePath(LeftInferiorLobePosteriorBasalAlveolarDeadSpace, LeftInferiorLobePosteriorBasalAlveoli, pulse::ExpandedLungsRespiratoryPath::LeftInferiorLobePosteriorBasalAlveolarDeadSpaceToLeftInferiorLobePosteriorBasalAlveoli);
    SEFluidCircuitPath& LeftInferiorLobeLateralBasalAlveolarDeadSpaceToLeftInferiorLobeLateralBasalAlveoli = cRespiratory.CreatePath(LeftInferiorLobeLateralBasalAlveolarDeadSpace, LeftInferiorLobeLateralBasalAlveoli, pulse::ExpandedLungsRespiratoryPath::LeftInferiorLobeLateralBasalAlveolarDeadSpaceToLeftInferiorLobeLateralBasalAlveoli);
    SEFluidCircuitPath& LeftInferiorLobeAnteromedialBasalAlveolarDeadSpaceToLeftInferiorLobeAnteromedialBasalAlveoli = cRespiratory.CreatePath(LeftInferiorLobeAnteromedialBasalAlveolarDeadSpace, LeftInferiorLobeAnteromedialBasalAlveoli, pulse::ExpandedLungsRespiratoryPath::LeftInferiorLobeAnteromedialBasalAlveolarDeadSpaceToLeftInferiorLobeAnteromedialBasalAlveoli);
    SEFluidCircuitPath& LeftInferiorLobeSuperiorAlveolarDeadSpaceToLeftInferiorLobeSuperiorAlveoli = cRespiratory.CreatePath(LeftInferiorLobeSuperiorAlveolarDeadSpace, LeftInferiorLobeSuperiorAlveoli, pulse::ExpandedLungsRespiratoryPath::LeftInferiorLobeSuperiorAlveolarDeadSpaceToLeftInferiorLobeSuperiorAlveoli);
    SEFluidCircuitPath& LeftSuperiorLobeInferiorLingulaAlveolarDeadSpaceToLeftSuperiorLobeInferiorLingulaAlveoli = cRespiratory.CreatePath(LeftSuperiorLobeInferiorLingulaAlveolarDeadSpace, LeftSuperiorLobeInferiorLingulaAlveoli, pulse::ExpandedLungsRespiratoryPath::LeftSuperiorLobeInferiorLingulaAlveolarDeadSpaceToLeftSuperiorLobeInferiorLingulaAlveoli);
    SEFluidCircuitPath& LeftSuperiorLobeSuperiorLingulaAlveolarDeadSpaceToLeftSuperiorLobeSuperiorLingulaAlveoli = cRespiratory.CreatePath(LeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace, LeftSuperiorLobeSuperiorLingulaAlveoli, pulse::ExpandedLungsRespiratoryPath::LeftSuperiorLobeSuperiorLingulaAlveolarDeadSpaceToLeftSuperiorLobeSuperiorLingulaAlveoli);
    SEFluidCircuitPath& LeftSuperiorLobeAnteriorAlveolarDeadSpaceToLeftSuperiorLobeAnteriorAlveoli = cRespiratory.CreatePath(LeftSuperiorLobeAnteriorAlveolarDeadSpace, LeftSuperiorLobeAnteriorAlveoli, pulse::ExpandedLungsRespiratoryPath::LeftSuperiorLobeAnteriorAlveolarDeadSpaceToLeftSuperiorLobeAnteriorAlveoli);
    SEFluidCircuitPath& LeftSuperiorLobeApicoposteriorAlveolarDeadSpaceToLeftSuperiorLobeApicoposteriorAlveoli = cRespiratory.CreatePath(LeftSuperiorLobeApicoposteriorAlveolarDeadSpace, LeftSuperiorLobeApicoposteriorAlveoli, pulse::ExpandedLungsRespiratoryPath::LeftSuperiorLobeApicoposteriorAlveolarDeadSpaceToLeftSuperiorLobeApicoposteriorAlveoli);

    SEFluidCircuitPath& RightSuperiorLobeApicalAlveoliToRightPleuralConnection = cRespiratory.CreatePath(RightSuperiorLobeApicalAlveoli, RightPleuralConnection, pulse::ExpandedLungsRespiratoryPath::RightSuperiorLobeApicalAlveoliToRightPleuralConnection);
    SEFluidCircuitPath& RightSuperiorLobePosteriorAlveoliToRightPleuralConnection = cRespiratory.CreatePath(RightSuperiorLobePosteriorAlveoli, RightPleuralConnection, pulse::ExpandedLungsRespiratoryPath::RightSuperiorLobePosteriorAlveoliToRightPleuralConnection);
    SEFluidCircuitPath& RightSuperiorLobeAnteriorAlveoliToRightPleuralConnection = cRespiratory.CreatePath(RightSuperiorLobeAnteriorAlveoli, RightPleuralConnection, pulse::ExpandedLungsRespiratoryPath::RightSuperiorLobeAnteriorAlveoliToRightPleuralConnection);
    SEFluidCircuitPath& RightMiddleLobeLateralAlveoliToRightPleuralConnection = cRespiratory.CreatePath(RightMiddleLobeLateralAlveoli, RightPleuralConnection, pulse::ExpandedLungsRespiratoryPath::RightMiddleLobeLateralAlveoliToRightPleuralConnection);
    SEFluidCircuitPath& RightMiddleLobeMedialAlveoliToRightPleuralConnection = cRespiratory.CreatePath(RightMiddleLobeMedialAlveoli, RightPleuralConnection, pulse::ExpandedLungsRespiratoryPath::RightMiddleLobeMedialAlveoliToRightPleuralConnection);
    SEFluidCircuitPath& RightInferiorLobeSuperiorAlveoliToRightPleuralConnection = cRespiratory.CreatePath(RightInferiorLobeSuperiorAlveoli, RightPleuralConnection, pulse::ExpandedLungsRespiratoryPath::RightInferiorLobeSuperiorAlveoliToRightPleuralConnection);
    SEFluidCircuitPath& RightInferiorLobeMedialBasalAlveoliToRightPleuralConnection = cRespiratory.CreatePath(RightInferiorLobeMedialBasalAlveoli, RightPleuralConnection, pulse::ExpandedLungsRespiratoryPath::RightInferiorLobeMedialBasalAlveoliToRightPleuralConnection);
    SEFluidCircuitPath& RightInferiorLobeAnteriorBasalAlveoliToRightPleuralConnection = cRespiratory.CreatePath(RightInferiorLobeAnteriorBasalAlveoli, RightPleuralConnection, pulse::ExpandedLungsRespiratoryPath::RightInferiorLobeAnteriorBasalAlveoliToRightPleuralConnection);
    SEFluidCircuitPath& RightInferiorLobeLateralBasalAlveoliToRightPleuralConnection = cRespiratory.CreatePath(RightInferiorLobeLateralBasalAlveoli, RightPleuralConnection, pulse::ExpandedLungsRespiratoryPath::RightInferiorLobeLateralBasalAlveoliToRightPleuralConnection);
    SEFluidCircuitPath& RightInferiorLobePosteriorBasalAlveoliToRightPleuralConnection = cRespiratory.CreatePath(RightInferiorLobePosteriorBasalAlveoli, RightPleuralConnection, pulse::ExpandedLungsRespiratoryPath::RightInferiorLobePosteriorBasalAlveoliToRightPleuralConnection);

    SEFluidCircuitPath& LeftInferiorLobePosteriorBasalAlveoliToLeftPleuralConnection = cRespiratory.CreatePath(LeftInferiorLobePosteriorBasalAlveoli, LeftPleuralConnection, pulse::ExpandedLungsRespiratoryPath::LeftInferiorLobePosteriorBasalAlveoliToLeftPleuralConnection);
    SEFluidCircuitPath& LeftInferiorLobeLateralBasalAlveoliToLeftPleuralConnection = cRespiratory.CreatePath(LeftInferiorLobeLateralBasalAlveoli, LeftPleuralConnection, pulse::ExpandedLungsRespiratoryPath::LeftInferiorLobeLateralBasalAlveoliToLeftPleuralConnection);
    SEFluidCircuitPath& LeftInferiorLobeAnteromedialBasalAlveoliToLeftPleuralConnection = cRespiratory.CreatePath(LeftInferiorLobeAnteromedialBasalAlveoli, LeftPleuralConnection, pulse::ExpandedLungsRespiratoryPath::LeftInferiorLobeAnteromedialBasalAlveoliToLeftPleuralConnection);
    SEFluidCircuitPath& LeftInferiorLobeSuperiorAlveoliToLeftPleuralConnection = cRespiratory.CreatePath(LeftInferiorLobeSuperiorAlveoli, LeftPleuralConnection, pulse::ExpandedLungsRespiratoryPath::LeftInferiorLobeSuperiorAlveoliToLeftPleuralConnection);
    SEFluidCircuitPath& LeftSuperiorLobeInferiorLingulaAlveoliToLeftPleuralConnection = cRespiratory.CreatePath(LeftSuperiorLobeInferiorLingulaAlveoli, LeftPleuralConnection, pulse::ExpandedLungsRespiratoryPath::LeftSuperiorLobeInferiorLingulaAlveoliToLeftPleuralConnection);
    SEFluidCircuitPath& LeftSuperiorLobeSuperiorLingulaAlveoliToLeftPleuralConnection = cRespiratory.CreatePath(LeftSuperiorLobeSuperiorLingulaAlveoli, LeftPleuralConnection, pulse::ExpandedLungsRespiratoryPath::LeftSuperiorLobeSuperiorLingulaAlveoliToLeftPleuralConnection);
    SEFluidCircuitPath& LeftSuperiorLobeAnteriorAlveoliToLeftPleuralConnection = cRespiratory.CreatePath(LeftSuperiorLobeAnteriorAlveoli, LeftPleuralConnection, pulse::ExpandedLungsRespiratoryPath::LeftSuperiorLobeAnteriorAlveoliToLeftPleuralConnection);
    SEFluidCircuitPath& LeftSuperiorLobeApicoposteriorAlveoliToLeftPleuralConnection = cRespiratory.CreatePath(LeftSuperiorLobeApicoposteriorAlveoli, LeftPleuralConnection, pulse::ExpandedLungsRespiratoryPath::LeftSuperiorLobeApicoposteriorAlveoliToLeftPleuralConnection);

    double CarinaToRightMainBronchusResistance_cmH2O_s_Per_L = 0.231;
    double RightMainBronchusToRightIntermediateBronchusResistance_cmH2O_s_Per_L = 0.150;
    double RightMainBronchusToRightSuperiorLobarBronchusResistance_cmH2O_s_Per_L = 0.387;
    double RightIntermediateBronchusToRightMiddleLobarBronchusResistance_cmH2O_s_Per_L = 0.877;
    double RightIntermediateBronchusToRightInferiorLobarBronchus1Resistance_cmH2O_s_Per_L = 0.150;
    double RightInferiorLobarBronchus1ToRightInferiorLobarBronchus2Resistance_cmH2O_s_Per_L = 0.186;
    double RightSuperiorLobarBronchusToRightSuperiorApicoposteriorBronchusResistance_cmH2O_s_Per_L = 0.449;
    double RightInferiorLobarBronchus2ToRightInferiorLobarBronchus3Resistance_cmH2O_s_Per_L = 0.150;
    double RightInferiorLobarBronchus2ToRightInferiorLobarBronchus4Resistance_cmH2O_s_Per_L = 0.578;

    double CarinaToLeftMainBronchusResistance_cmH2O_s_Per_L = 0.276;
    double LeftMainBronchusToLeftInferiorLobarBronchus1Resistance_cmH2O_s_Per_L = 0.337;
    double LeftMainBronchusToLeftSuperiorLobarBronchusResistance_cmH2O_s_Per_L = 0.410;
    double LeftInferiorLobarBronchus1ToLeftInferiorLobarBronchus2Resistance_cmH2O_s_Per_L = 0.150;
    double LeftInferiorLobarBronchus2ToLeftInferiorLobarBronchus3Resistance_cmH2O_s_Per_L = 0.150;
    double LeftSuperiorLobarBronchusToLeftLingularBonchusResistance_cmH2O_s_Per_L = 0.771;
    double LeftSuperiorLobarBronchusToLeftSuperiorApicoposteriorBronchusResistance_cmH2O_s_Per_L = 0.350;

    double RightSuperiorApicoposteriorBronchusToRightSuperiorLobeApicalBronchioleResistance_cmH2O_s_Per_L = 1.486;
    double RightSuperiorApicoposteriorBronchusToRightSuperiorLobePosteriorBronchioleResistance_cmH2O_s_Per_L = 3.621;
    double RightSuperiorLobarBronchusToRightSuperiorLobeAnteriorBronchioleResistance_cmH2O_s_Per_L = 1.544;
    double RightMiddleLobarBronchusToRightMiddleLobeLateralBronchioleResistance_cmH2O_s_Per_L = 2.414;
    double RightMiddleLobarBronchusToRightMiddleLobeMedialBronchioleResistance_cmH2O_s_Per_L = 2.414;
    double RightInferiorLobarBronchus1ToRightInferiorLobeSuperiorBronchioleResistance_cmH2O_s_Per_L = 2.395;
    double RightInferiorLobarBronchus2ToRightInferiorLobeMedialBasalBronchioleResistance_cmH2O_s_Per_L = 3.748;
    double RightInferiorLobarBronchus3ToRightInferiorLobeAnteriorBasalBronchioleResistance_cmH2O_s_Per_L = 1.811;
    double RightInferiorLobarBronchus4ToRightInferiorLobeLateralBasalBronchioleResistance_cmH2O_s_Per_L = 2.465;
    double RightInferiorLobarBronchus4ToRightInferiorLobePosteriorBasalBronchioleResistance_cmH2O_s_Per_L = 2.465;

    double LeftInferiorLobarBronchus3ToLeftInferiorLobeLateralBasalBronchioleResistance_cmH2O_s_Per_L = 1.486;
    double LeftInferiorLobarBronchus3ToLeftInferiorLobePosteriorBasalBronchioleResistance_cmH2O_s_Per_L = 2.465;
    double LeftInferiorLobarBronchus2ToLeftInferiorLobeAnteromedialBasalBronchioleResistance_cmH2O_s_Per_L = 1.730;
    double LeftInferiorLobarBronchus1ToLeftInferiorLobeSuperiorBronchioleResistance_cmH2O_s_Per_L = 3.515;
    double LeftLingularBonchusToLeftSuperiorLobeInferiorLingulaBronchioleResistance_cmH2O_s_Per_L = 4.292;
    double LeftLingularBonchusToLeftSuperiorLobeSuperiorLingulaBronchioleResistance_cmH2O_s_Per_L = 3.219;
    double LeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeAnteriorBronchioleResistance_cmH2O_s_Per_L = 1.811;
    double LeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeApicoposteriorBronchioleResistance_cmH2O_s_Per_L = 1.486;

    CarinaToRightMainBronchus.GetResistanceBaseline().SetValue(CarinaToRightMainBronchusResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    RightMainBronchusToRightIntermediateBronchus.GetResistanceBaseline().SetValue(RightMainBronchusToRightIntermediateBronchusResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    RightMainBronchusToRightSuperiorLobarBronchus.GetResistanceBaseline().SetValue(RightMainBronchusToRightSuperiorLobarBronchusResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    RightIntermediateBronchusToRightMiddleLobarBronchus.GetResistanceBaseline().SetValue(RightIntermediateBronchusToRightMiddleLobarBronchusResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    RightIntermediateBronchusToRightInferiorLobarBronchus1.GetResistanceBaseline().SetValue(RightIntermediateBronchusToRightInferiorLobarBronchus1Resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    RightInferiorLobarBronchus1ToRightInferiorLobarBronchus2.GetResistanceBaseline().SetValue(RightInferiorLobarBronchus1ToRightInferiorLobarBronchus2Resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    RightSuperiorLobarBronchusToRightSuperiorApicoposteriorBronchus.GetResistanceBaseline().SetValue(RightSuperiorLobarBronchusToRightSuperiorApicoposteriorBronchusResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    RightInferiorLobarBronchus2ToRightInferiorLobarBronchus3.GetResistanceBaseline().SetValue(RightInferiorLobarBronchus2ToRightInferiorLobarBronchus3Resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    RightInferiorLobarBronchus2ToRightInferiorLobarBronchus4.GetResistanceBaseline().SetValue(RightInferiorLobarBronchus2ToRightInferiorLobarBronchus4Resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);

    CarinaToLeftMainBronchus.GetResistanceBaseline().SetValue(CarinaToLeftMainBronchusResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    LeftMainBronchusToLeftInferiorLobarBronchus1.GetResistanceBaseline().SetValue(LeftMainBronchusToLeftInferiorLobarBronchus1Resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    LeftMainBronchusToLeftSuperiorLobarBronchus.GetResistanceBaseline().SetValue(LeftMainBronchusToLeftSuperiorLobarBronchusResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    LeftInferiorLobarBronchus1ToLeftInferiorLobarBronchus2.GetResistanceBaseline().SetValue(LeftInferiorLobarBronchus1ToLeftInferiorLobarBronchus2Resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    LeftInferiorLobarBronchus2ToLeftInferiorLobarBronchus3.GetResistanceBaseline().SetValue(LeftInferiorLobarBronchus2ToLeftInferiorLobarBronchus3Resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    LeftSuperiorLobarBronchusToLeftLingularBonchus.GetResistanceBaseline().SetValue(LeftSuperiorLobarBronchusToLeftLingularBonchusResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    LeftSuperiorLobarBronchusToLeftSuperiorApicoposteriorBronchus.GetResistanceBaseline().SetValue(LeftSuperiorLobarBronchusToLeftSuperiorApicoposteriorBronchusResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);

    RightSuperiorApicoposteriorBronchusToRightSuperiorLobeApicalBronchiole.GetResistanceBaseline().SetValue(RightSuperiorApicoposteriorBronchusToRightSuperiorLobeApicalBronchioleResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    RightSuperiorApicoposteriorBronchusToRightSuperiorLobePosteriorBronchiole.GetResistanceBaseline().SetValue(RightSuperiorApicoposteriorBronchusToRightSuperiorLobePosteriorBronchioleResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    RightSuperiorLobarBronchusToRightSuperiorLobeAnteriorBronchiole.GetResistanceBaseline().SetValue(RightSuperiorLobarBronchusToRightSuperiorLobeAnteriorBronchioleResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    RightMiddleLobarBronchusToRightMiddleLobeLateralBronchiole.GetResistanceBaseline().SetValue(RightMiddleLobarBronchusToRightMiddleLobeLateralBronchioleResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    RightMiddleLobarBronchusToRightMiddleLobeMedialBronchiole.GetResistanceBaseline().SetValue(RightMiddleLobarBronchusToRightMiddleLobeMedialBronchioleResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    RightInferiorLobarBronchus1ToRightInferiorLobeSuperiorBronchiole.GetResistanceBaseline().SetValue(RightInferiorLobarBronchus1ToRightInferiorLobeSuperiorBronchioleResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    RightInferiorLobarBronchus2ToRightInferiorLobeMedialBasalBronchiole.GetResistanceBaseline().SetValue(RightInferiorLobarBronchus2ToRightInferiorLobeMedialBasalBronchioleResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    RightInferiorLobarBronchus3ToRightInferiorLobeAnteriorBasalBronchiole.GetResistanceBaseline().SetValue(RightInferiorLobarBronchus3ToRightInferiorLobeAnteriorBasalBronchioleResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    RightInferiorLobarBronchus4ToRightInferiorLobeLateralBasalBronchiole.GetResistanceBaseline().SetValue(RightInferiorLobarBronchus4ToRightInferiorLobeLateralBasalBronchioleResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    RightInferiorLobarBronchus4ToRightInferiorLobePosteriorBasalBronchiole.GetResistanceBaseline().SetValue(RightInferiorLobarBronchus4ToRightInferiorLobePosteriorBasalBronchioleResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);

    LeftInferiorLobarBronchus3ToLeftInferiorLobeLateralBasalBronchiole.GetResistanceBaseline().SetValue(LeftInferiorLobarBronchus3ToLeftInferiorLobeLateralBasalBronchioleResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    LeftInferiorLobarBronchus3ToLeftInferiorLobePosteriorBasalBronchiole.GetResistanceBaseline().SetValue(LeftInferiorLobarBronchus3ToLeftInferiorLobePosteriorBasalBronchioleResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    LeftInferiorLobarBronchus2ToLeftInferiorLobeAnteromedialBasalBronchiole.GetResistanceBaseline().SetValue(LeftInferiorLobarBronchus2ToLeftInferiorLobeAnteromedialBasalBronchioleResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    LeftInferiorLobarBronchus1ToLeftInferiorLobeSuperiorBronchiole.GetResistanceBaseline().SetValue(LeftInferiorLobarBronchus1ToLeftInferiorLobeSuperiorBronchioleResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    LeftLingularBonchusToLeftSuperiorLobeInferiorLingulaBronchiole.GetResistanceBaseline().SetValue(LeftLingularBonchusToLeftSuperiorLobeInferiorLingulaBronchioleResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    LeftLingularBonchusToLeftSuperiorLobeSuperiorLingulaBronchiole.GetResistanceBaseline().SetValue(LeftLingularBonchusToLeftSuperiorLobeSuperiorLingulaBronchioleResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    LeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeAnteriorBronchiole.GetResistanceBaseline().SetValue(LeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeAnteriorBronchioleResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    LeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeApicoposteriorBronchiole.GetResistanceBaseline().SetValue(LeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeApicoposteriorBronchioleResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);

    double RightSuperiorLobeApicalCompliance_Percent = 4.0;
    double RightSuperiorLobePosteriorCompliance_Percent = 4.4;
    double RightSuperiorLobeAnteriorCompliance_Percent = 5.0;
    double RightMiddleLobeLateralCompliance_Percent = 5.9;
    double RightMiddleLobeMedialCompliance_Percent = 5.8;
    double RightInferiorLobeSuperiorCompliance_Percent = 5.3;
    double RightInferiorLobeMedialBasalCompliance_Percent = 6.0;
    double RightInferiorLobeAnteriorBasalCompliance_Percent = 6.3;
    double RightInferiorLobeLateralBasalCompliance_Percent = 6.5;
    double RightInferiorLobePosteriorBasalCompliance_Percent = 6.2;

    double LeftInferiorLobePosteriorBasalCompliance_Percent = 6.2;
    double LeftInferiorLobeLateralBasalCompliance_Percent = 6.4;
    double LeftInferiorLobeAnteromedialBasalCompliance_Percent = 6.1;
    double LeftInferiorLobeSuperiorCompliance_Percent = 5.3;
    double LeftSuperiorLobeInferiorLingulaCompliance_Percent = 6.0;
    double LeftSuperiorLobeSuperiorLingulaCompliance_Percent = 5.6;
    double LeftSuperiorLobeAnteriorCompliance_Percent = 4.9;
    double LeftSuperiorLobeApicoposteriorCompliance_Percent = 4.2;

    //Both lungs in parallel
    double LungsCompliance_L_Per_cmH2O = LungCompliance_L_Per_cmH2O * 2.0;

    RightSuperiorLobeApicalAlveoliToRightPleuralConnection.GetComplianceBaseline().SetValue(RightSuperiorLobeApicalCompliance_Percent / 100.0 * LungsCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    RightSuperiorLobePosteriorAlveoliToRightPleuralConnection.GetComplianceBaseline().SetValue(RightSuperiorLobePosteriorCompliance_Percent / 100.0 * LungsCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    RightSuperiorLobeAnteriorAlveoliToRightPleuralConnection.GetComplianceBaseline().SetValue(RightSuperiorLobeAnteriorCompliance_Percent / 100.0 * LungsCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    RightMiddleLobeLateralAlveoliToRightPleuralConnection.GetComplianceBaseline().SetValue(RightMiddleLobeLateralCompliance_Percent / 100.0 * LungsCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    RightMiddleLobeMedialAlveoliToRightPleuralConnection.GetComplianceBaseline().SetValue(RightMiddleLobeMedialCompliance_Percent / 100.0 * LungsCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    RightInferiorLobeSuperiorAlveoliToRightPleuralConnection.GetComplianceBaseline().SetValue(RightInferiorLobeSuperiorCompliance_Percent / 100.0 * LungsCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    RightInferiorLobeMedialBasalAlveoliToRightPleuralConnection.GetComplianceBaseline().SetValue(RightInferiorLobeMedialBasalCompliance_Percent / 100.0 * LungsCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    RightInferiorLobeAnteriorBasalAlveoliToRightPleuralConnection.GetComplianceBaseline().SetValue(RightInferiorLobeAnteriorBasalCompliance_Percent / 100.0 * LungsCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    RightInferiorLobeLateralBasalAlveoliToRightPleuralConnection.GetComplianceBaseline().SetValue(RightInferiorLobeLateralBasalCompliance_Percent / 100.0 * LungsCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    RightInferiorLobePosteriorBasalAlveoliToRightPleuralConnection.GetComplianceBaseline().SetValue(RightInferiorLobePosteriorBasalCompliance_Percent / 100.0 * LungsCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);

    LeftInferiorLobePosteriorBasalAlveoliToLeftPleuralConnection.GetComplianceBaseline().SetValue(LeftInferiorLobePosteriorBasalCompliance_Percent / 100.0 * LungsCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    LeftInferiorLobeLateralBasalAlveoliToLeftPleuralConnection.GetComplianceBaseline().SetValue(LeftInferiorLobeLateralBasalCompliance_Percent / 100.0 * LungsCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    LeftInferiorLobeAnteromedialBasalAlveoliToLeftPleuralConnection.GetComplianceBaseline().SetValue(LeftInferiorLobeAnteromedialBasalCompliance_Percent / 100.0 * LungsCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    LeftInferiorLobeSuperiorAlveoliToLeftPleuralConnection.GetComplianceBaseline().SetValue(LeftInferiorLobeSuperiorCompliance_Percent / 100.0 * LungsCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    LeftSuperiorLobeInferiorLingulaAlveoliToLeftPleuralConnection.GetComplianceBaseline().SetValue(LeftSuperiorLobeInferiorLingulaCompliance_Percent / 100.0 * LungsCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    LeftSuperiorLobeSuperiorLingulaAlveoliToLeftPleuralConnection.GetComplianceBaseline().SetValue(LeftSuperiorLobeSuperiorLingulaCompliance_Percent / 100.0 * LungsCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    LeftSuperiorLobeAnteriorAlveoliToLeftPleuralConnection.GetComplianceBaseline().SetValue(LeftSuperiorLobeAnteriorCompliance_Percent / 100.0 * LungsCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    LeftSuperiorLobeApicoposteriorAlveoliToLeftPleuralConnection.GetComplianceBaseline().SetValue(LeftSuperiorLobeApicoposteriorCompliance_Percent / 100.0 * LungsCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);

    //Need a no element path to be able to include a node with no volume, so it doesn't get modified by compliances
    cRespiratory.CreatePath(RightPleuralConnection, RightPleural, pulse::RespiratoryPath::RightPleuralConnectionToRightPleural);
    cRespiratory.CreatePath(LeftPleuralConnection, LeftPleural, pulse::RespiratoryPath::LeftPleuralConnectionToLeftPleural);
    // Paths for the Driver
    SEFluidCircuitPath& EnvironmentToRespiratoryMuscle = cRespiratory.CreatePath(Ambient, RespiratoryMuscle, pulse::RespiratoryPath::EnvironmentToRespiratoryMuscle);
    EnvironmentToRespiratoryMuscle.GetPressureSourceBaseline().SetValue(RespiratoryMuscle.GetPressure(PressureUnit::cmH2O) - Ambient.GetNextPressure(PressureUnit::cmH2O), PressureUnit::cmH2O);
    // Esophageal (Stomach) path
    SEFluidCircuitPath& AirwayToStomach = cRespiratory.CreatePath(Airway, Stomach, pulse::RespiratoryPath::AirwayToStomach);
    AirwayToStomach.GetResistanceBaseline().SetValue(openResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    SEFluidCircuitPath& StomachToEnvironment = cRespiratory.CreatePath(Stomach, Ambient, pulse::RespiratoryPath::StomachToEnvironment);
    StomachToEnvironment.GetComplianceBaseline().SetValue(0.05, VolumePerPressureUnit::L_Per_cmH2O);
    StomachToEnvironment.SetNextPolarizedState(eGate::Closed);
    // Paths to RespiratoryMuscle
    SEFluidCircuitPath& RightPleuralToRespiratoryMuscle = cRespiratory.CreatePath(RightPleural, RespiratoryMuscle, pulse::RespiratoryPath::RightPleuralToRespiratoryMuscle);
    RightPleuralToRespiratoryMuscle.GetComplianceBaseline().SetValue(ChestWallCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    SEFluidCircuitPath& LeftPleuralToRespiratoryMuscle = cRespiratory.CreatePath(LeftPleural, RespiratoryMuscle, pulse::RespiratoryPath::LeftPleuralToRespiratoryMuscle);
    LeftPleuralToRespiratoryMuscle.GetComplianceBaseline().SetValue(ChestWallCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);

    cRespiratory.SetNextAndCurrentFromBaselines();
    cRespiratory.StateChange();

    // Setup Compartments // 

    // Pulmonary Compartments
    SEGasCompartment& pAirway = m_Compartments->CreateGasCompartment(pulse::PulmonaryCompartment::Airway);
    pAirway.MapNode(Airway);
    SEGasCompartment& pStomach = m_Compartments->CreateGasCompartment(pulse::PulmonaryCompartment::Stomach);
    pStomach.MapNode(Stomach);
    SEGasCompartment& pPharynx = m_Compartments->CreateGasCompartment(pulse::PulmonaryCompartment::Pharynx);
    pPharynx.MapNode(Pharynx);
    SEGasCompartment& pCarina = m_Compartments->CreateGasCompartment(pulse::PulmonaryCompartment::Carina);
    pCarina.MapNode(Carina);
    SEGasCompartment& pLeftPleural = m_Compartments->CreateGasCompartment(pulse::PulmonaryCompartment::LeftPleuralCavity);
    pLeftPleural.MapNode(LeftPleural);
    SEGasCompartment& pRightPleural = m_Compartments->CreateGasCompartment(pulse::PulmonaryCompartment::RightPleuralCavity);
    pRightPleural.MapNode(RightPleural);

    SEGasCompartment& pRightMainBronchus = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightMainBronchus);
    SEGasCompartment& pLeftMainBronchus = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftMainBronchus);
    SEGasCompartment& pRightIntermediateBronchus = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightIntermediateBronchus);
    SEGasCompartment& pRightSuperiorLobarBronchus = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightSuperiorLobarBronchus);
    SEGasCompartment& pRightMiddleLobarBronchus = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightMiddleLobarBronchus);
    SEGasCompartment& pRightInferiorLobarBronchus1 = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightInferiorLobarBronchus1);
    SEGasCompartment& pLeftInferiorLobarBronchus1 = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftInferiorLobarBronchus1);
    SEGasCompartment& pLeftSuperiorLobarBronchus = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftSuperiorLobarBronchus);
    SEGasCompartment& pRightInferiorLobarBronchus2 = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightInferiorLobarBronchus2);
    SEGasCompartment& pLeftInferiorLobarBronchus2 = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftInferiorLobarBronchus2);
    SEGasCompartment& pRightSuperiorApicoposteriorBronchus = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightSuperiorApicoposteriorBronchus);
    SEGasCompartment& pRightInferiorLobarBronchus3 = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightInferiorLobarBronchus3);
    SEGasCompartment& pLeftInferiorLobarBronchus3 = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftInferiorLobarBronchus3);
    SEGasCompartment& pLeftLingularBonchus = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftLingularBonchus);
    SEGasCompartment& pLeftSuperiorApicoposteriorBronchus = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftSuperiorApicoposteriorBronchus);
    SEGasCompartment& pRightInferiorLobarBronchus4 = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightInferiorLobarBronchus4);

    SEGasCompartment& pRightSuperiorLobeApicalBronchiole = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightSuperiorLobeApicalBronchiole);
    SEGasCompartment& pRightSuperiorLobePosteriorBronchiole = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightSuperiorLobePosteriorBronchiole);
    SEGasCompartment& pRightSuperiorLobeAnteriorBronchiole = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightSuperiorLobeAnteriorBronchiole);
    SEGasCompartment& pRightMiddleLobeLateralBronchiole = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightMiddleLobeLateralBronchiole);
    SEGasCompartment& pRightMiddleLobeMedialBronchiole = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightMiddleLobeMedialBronchiole);
    SEGasCompartment& pRightInferiorLobeSuperiorBronchiole = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightInferiorLobeSuperiorBronchiole);
    SEGasCompartment& pRightInferiorLobeMedialBasalBronchiole = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightInferiorLobeMedialBasalBronchiole);
    SEGasCompartment& pRightInferiorLobeAnteriorBasalBronchiole = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightInferiorLobeAnteriorBasalBronchiole);
    SEGasCompartment& pRightInferiorLobeLateralBasalBronchiole = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightInferiorLobeLateralBasalBronchiole);
    SEGasCompartment& pRightInferiorLobePosteriorBasalBronchiole = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightInferiorLobePosteriorBasalBronchiole);

    SEGasCompartment& pLeftInferiorLobePosteriorBasalBronchiole = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftInferiorLobePosteriorBasalBronchiole);
    SEGasCompartment& pLeftInferiorLobeLateralBasalBronchiole = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftInferiorLobeLateralBasalBronchiole);
    SEGasCompartment& pLeftInferiorLobeAnteromedialBasalBronchiole = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftInferiorLobeAnteromedialBasalBronchiole);
    SEGasCompartment& pLeftInferiorLobeSuperiorBronchiole = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftInferiorLobeSuperiorBronchiole);
    SEGasCompartment& pLeftSuperiorLobeInferiorLingulaBronchiole = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftSuperiorLobeInferiorLingulaBronchiole);
    SEGasCompartment& pLeftSuperiorLobeSuperiorLingulaBronchiole = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftSuperiorLobeSuperiorLingulaBronchiole);
    SEGasCompartment& pLeftSuperiorLobeAnteriorBronchiole = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftSuperiorLobeAnteriorBronchiole);
    SEGasCompartment& pLeftSuperiorLobeApicoposteriorBronchiole = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftSuperiorLobeApicoposteriorBronchiole);

    SEGasCompartment& pRightSuperiorLobeApicalAlveoli = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightSuperiorLobeApicalAlveoli);
    SEGasCompartment& pRightSuperiorLobePosteriorAlveoli = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightSuperiorLobePosteriorAlveoli);
    SEGasCompartment& pRightSuperiorLobeAnteriorAlveoli = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightSuperiorLobeAnteriorAlveoli);
    SEGasCompartment& pRightMiddleLobeLateralAlveoli = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightMiddleLobeLateralAlveoli);
    SEGasCompartment& pRightMiddleLobeMedialAlveoli = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightMiddleLobeMedialAlveoli);
    SEGasCompartment& pRightInferiorLobeSuperiorAlveoli = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightInferiorLobeSuperiorAlveoli);
    SEGasCompartment& pRightInferiorLobeMedialBasalAlveoli = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightInferiorLobeMedialBasalAlveoli);
    SEGasCompartment& pRightInferiorLobeAnteriorBasalAlveoli = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightInferiorLobeAnteriorBasalAlveoli);
    SEGasCompartment& pRightInferiorLobeLateralBasalAlveoli = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightInferiorLobeLateralBasalAlveoli);
    SEGasCompartment& pRightInferiorLobePosteriorBasalAlveoli = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightInferiorLobePosteriorBasalAlveoli);

    SEGasCompartment& pLeftInferiorLobePosteriorBasalAlveoli = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftInferiorLobePosteriorBasalAlveoli);
    SEGasCompartment& pLeftInferiorLobeLateralBasalAlveoli = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftInferiorLobeLateralBasalAlveoli);
    SEGasCompartment& pLeftInferiorLobeAnteromedialBasalAlveoli = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftInferiorLobeAnteromedialBasalAlveoli);
    SEGasCompartment& pLeftInferiorLobeSuperiorAlveoli = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftInferiorLobeSuperiorAlveoli);
    SEGasCompartment& pLeftSuperiorLobeInferiorLingulaAlveoli = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftSuperiorLobeInferiorLingulaAlveoli);
    SEGasCompartment& pLeftSuperiorLobeSuperiorLingulaAlveoli = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftSuperiorLobeSuperiorLingulaAlveoli);
    SEGasCompartment& pLeftSuperiorLobeAnteriorAlveoli = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftSuperiorLobeAnteriorAlveoli);
    SEGasCompartment& pLeftSuperiorLobeApicoposteriorAlveoli = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftSuperiorLobeApicoposteriorAlveoli);

    SEGasCompartment& pRightSuperiorLobeApicalAlveolarDeadSpace = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightSuperiorLobeApicalAlveolarDeadSpace);
    SEGasCompartment& pRightSuperiorLobePosteriorAlveolarDeadSpace = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightSuperiorLobePosteriorAlveolarDeadSpace);
    SEGasCompartment& pRightSuperiorLobeAnteriorAlveolarDeadSpace = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightSuperiorLobeAnteriorAlveolarDeadSpace);
    SEGasCompartment& pRightMiddleLobeLateralAlveolarDeadSpace = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightMiddleLobeLateralAlveolarDeadSpace);
    SEGasCompartment& pRightMiddleLobeMedialAlveolarDeadSpace = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightMiddleLobeMedialAlveolarDeadSpace);
    SEGasCompartment& pRightInferiorLobeSuperiorAlveolarDeadSpace = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightInferiorLobeSuperiorAlveolarDeadSpace);
    SEGasCompartment& pRightInferiorLobeMedialBasalAlveolarDeadSpace = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightInferiorLobeMedialBasalAlveolarDeadSpace);
    SEGasCompartment& pRightInferiorLobeAnteriorBasalAlveolarDeadSpace = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightInferiorLobeAnteriorBasalAlveolarDeadSpace);
    SEGasCompartment& pRightInferiorLobeLateralBasalAlveolarDeadSpace = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightInferiorLobeLateralBasalAlveolarDeadSpace);
    SEGasCompartment& pRightInferiorLobePosteriorBasalAlveolarDeadSpace = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightInferiorLobePosteriorBasalAlveolarDeadSpace);

    SEGasCompartment& pLeftInferiorLobePosteriorBasalAlveolarDeadSpace = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftInferiorLobePosteriorBasalAlveolarDeadSpace);
    SEGasCompartment& pLeftInferiorLobeLateralBasalAlveolarDeadSpace = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftInferiorLobeLateralBasalAlveolarDeadSpace);
    SEGasCompartment& pLeftInferiorLobeAnteromedialBasalAlveolarDeadSpace = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftInferiorLobeAnteromedialBasalAlveolarDeadSpace);
    SEGasCompartment& pLeftInferiorLobeSuperiorAlveolarDeadSpace = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftInferiorLobeSuperiorAlveolarDeadSpace);
    SEGasCompartment& pLeftSuperiorLobeInferiorLingulaAlveolarDeadSpace = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftSuperiorLobeInferiorLingulaAlveolarDeadSpace);
    SEGasCompartment& pLeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace);
    SEGasCompartment& pLeftSuperiorLobeAnteriorAlveolarDeadSpace = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftSuperiorLobeAnteriorAlveolarDeadSpace);
    SEGasCompartment& pLeftSuperiorLobeApicoposteriorAlveolarDeadSpace = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftSuperiorLobeApicoposteriorAlveolarDeadSpace);

    pRightMainBronchus.MapNode(RightMainBronchus);
    pLeftMainBronchus.MapNode(LeftMainBronchus);
    pRightIntermediateBronchus.MapNode(RightIntermediateBronchus);
    pRightSuperiorLobarBronchus.MapNode(RightSuperiorLobarBronchus);
    pRightMiddleLobarBronchus.MapNode(RightMiddleLobarBronchus);
    pRightInferiorLobarBronchus1.MapNode(RightInferiorLobarBronchus1);
    pLeftInferiorLobarBronchus1.MapNode(LeftInferiorLobarBronchus1);
    pLeftSuperiorLobarBronchus.MapNode(LeftSuperiorLobarBronchus);
    pRightInferiorLobarBronchus2.MapNode(RightInferiorLobarBronchus2);
    pLeftInferiorLobarBronchus2.MapNode(LeftInferiorLobarBronchus2);
    pRightSuperiorApicoposteriorBronchus.MapNode(RightSuperiorApicoposteriorBronchus);
    pRightInferiorLobarBronchus3.MapNode(RightInferiorLobarBronchus3);
    pLeftInferiorLobarBronchus3.MapNode(LeftInferiorLobarBronchus3);
    pLeftLingularBonchus.MapNode(LeftLingularBonchus);
    pLeftSuperiorApicoposteriorBronchus.MapNode(LeftSuperiorApicoposteriorBronchus);
    pRightInferiorLobarBronchus4.MapNode(RightInferiorLobarBronchus4);

    pRightSuperiorLobeApicalBronchiole.MapNode(RightSuperiorLobeApicalBronchiole);
    pRightSuperiorLobePosteriorBronchiole.MapNode(RightSuperiorLobePosteriorBronchiole);
    pRightSuperiorLobeAnteriorBronchiole.MapNode(RightSuperiorLobeAnteriorBronchiole);
    pRightMiddleLobeLateralBronchiole.MapNode(RightMiddleLobeLateralBronchiole);
    pRightMiddleLobeMedialBronchiole.MapNode(RightMiddleLobeMedialBronchiole);
    pRightInferiorLobeSuperiorBronchiole.MapNode(RightInferiorLobeSuperiorBronchiole);
    pRightInferiorLobeMedialBasalBronchiole.MapNode(RightInferiorLobeMedialBasalBronchiole);
    pRightInferiorLobeAnteriorBasalBronchiole.MapNode(RightInferiorLobeAnteriorBasalBronchiole);
    pRightInferiorLobeLateralBasalBronchiole.MapNode(RightInferiorLobeLateralBasalBronchiole);
    pRightInferiorLobePosteriorBasalBronchiole.MapNode(RightInferiorLobePosteriorBasalBronchiole);

    pLeftInferiorLobePosteriorBasalBronchiole.MapNode(LeftInferiorLobePosteriorBasalBronchiole);
    pLeftInferiorLobeLateralBasalBronchiole.MapNode(LeftInferiorLobeLateralBasalBronchiole);
    pLeftInferiorLobeAnteromedialBasalBronchiole.MapNode(LeftInferiorLobeAnteromedialBasalBronchiole);
    pLeftInferiorLobeSuperiorBronchiole.MapNode(LeftInferiorLobeSuperiorBronchiole);
    pLeftSuperiorLobeInferiorLingulaBronchiole.MapNode(LeftSuperiorLobeInferiorLingulaBronchiole);
    pLeftSuperiorLobeSuperiorLingulaBronchiole.MapNode(LeftSuperiorLobeSuperiorLingulaBronchiole);
    pLeftSuperiorLobeAnteriorBronchiole.MapNode(LeftSuperiorLobeAnteriorBronchiole);
    pLeftSuperiorLobeApicoposteriorBronchiole.MapNode(LeftSuperiorLobeApicoposteriorBronchiole);

    pRightSuperiorLobeApicalAlveolarDeadSpace.MapNode(RightSuperiorLobeApicalAlveolarDeadSpace);
    pRightSuperiorLobePosteriorAlveolarDeadSpace.MapNode(RightSuperiorLobePosteriorAlveolarDeadSpace);
    pRightSuperiorLobeAnteriorAlveolarDeadSpace.MapNode(RightSuperiorLobeAnteriorAlveolarDeadSpace);
    pRightMiddleLobeLateralAlveolarDeadSpace.MapNode(RightMiddleLobeLateralAlveolarDeadSpace);
    pRightMiddleLobeMedialAlveolarDeadSpace.MapNode(RightMiddleLobeMedialAlveolarDeadSpace);
    pRightInferiorLobeSuperiorAlveolarDeadSpace.MapNode(RightInferiorLobeSuperiorAlveolarDeadSpace);
    pRightInferiorLobeMedialBasalAlveolarDeadSpace.MapNode(RightInferiorLobeMedialBasalAlveolarDeadSpace);
    pRightInferiorLobeAnteriorBasalAlveolarDeadSpace.MapNode(RightInferiorLobeAnteriorBasalAlveolarDeadSpace);
    pRightInferiorLobeLateralBasalAlveolarDeadSpace.MapNode(RightInferiorLobeLateralBasalAlveolarDeadSpace);
    pRightInferiorLobePosteriorBasalAlveolarDeadSpace.MapNode(RightInferiorLobePosteriorBasalAlveolarDeadSpace);

    pLeftInferiorLobePosteriorBasalAlveolarDeadSpace.MapNode(LeftInferiorLobePosteriorBasalAlveolarDeadSpace);
    pLeftInferiorLobeLateralBasalAlveolarDeadSpace.MapNode(LeftInferiorLobeLateralBasalAlveolarDeadSpace);
    pLeftInferiorLobeAnteromedialBasalAlveolarDeadSpace.MapNode(LeftInferiorLobeAnteromedialBasalAlveolarDeadSpace);
    pLeftInferiorLobeSuperiorAlveolarDeadSpace.MapNode(LeftInferiorLobeSuperiorAlveolarDeadSpace);
    pLeftSuperiorLobeInferiorLingulaAlveolarDeadSpace.MapNode(LeftSuperiorLobeInferiorLingulaAlveolarDeadSpace);
    pLeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace.MapNode(LeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace);
    pLeftSuperiorLobeAnteriorAlveolarDeadSpace.MapNode(LeftSuperiorLobeAnteriorAlveolarDeadSpace);
    pLeftSuperiorLobeApicoposteriorAlveolarDeadSpace.MapNode(LeftSuperiorLobeApicoposteriorAlveolarDeadSpace);

    pRightSuperiorLobeApicalAlveoli.MapNode(RightSuperiorLobeApicalAlveoli);
    pRightSuperiorLobePosteriorAlveoli.MapNode(RightSuperiorLobePosteriorAlveoli);
    pRightSuperiorLobeAnteriorAlveoli.MapNode(RightSuperiorLobeAnteriorAlveoli);
    pRightMiddleLobeLateralAlveoli.MapNode(RightMiddleLobeLateralAlveoli);
    pRightMiddleLobeMedialAlveoli.MapNode(RightMiddleLobeMedialAlveoli);
    pRightInferiorLobeSuperiorAlveoli.MapNode(RightInferiorLobeSuperiorAlveoli);
    pRightInferiorLobeMedialBasalAlveoli.MapNode(RightInferiorLobeMedialBasalAlveoli);
    pRightInferiorLobeAnteriorBasalAlveoli.MapNode(RightInferiorLobeAnteriorBasalAlveoli);
    pRightInferiorLobeLateralBasalAlveoli.MapNode(RightInferiorLobeLateralBasalAlveoli);
    pRightInferiorLobePosteriorBasalAlveoli.MapNode(RightInferiorLobePosteriorBasalAlveoli);

    pLeftInferiorLobePosteriorBasalAlveoli.MapNode(LeftInferiorLobePosteriorBasalAlveoli);
    pLeftInferiorLobeLateralBasalAlveoli.MapNode(LeftInferiorLobeLateralBasalAlveoli);
    pLeftInferiorLobeAnteromedialBasalAlveoli.MapNode(LeftInferiorLobeAnteromedialBasalAlveoli);
    pLeftInferiorLobeSuperiorAlveoli.MapNode(LeftInferiorLobeSuperiorAlveoli);
    pLeftSuperiorLobeInferiorLingulaAlveoli.MapNode(LeftSuperiorLobeInferiorLingulaAlveoli);
    pLeftSuperiorLobeSuperiorLingulaAlveoli.MapNode(LeftSuperiorLobeSuperiorLingulaAlveoli);
    pLeftSuperiorLobeAnteriorAlveoli.MapNode(LeftSuperiorLobeAnteriorAlveoli);
    pLeftSuperiorLobeApicoposteriorAlveoli.MapNode(LeftSuperiorLobeApicoposteriorAlveoli);

    // Set up hierarchy
    SEGasCompartment& pLeftBranches = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::LeftBranches);
    pLeftBranches.AddChild(pLeftMainBronchus);
    pLeftBranches.AddChild(pLeftInferiorLobarBronchus1);
    pLeftBranches.AddChild(pLeftSuperiorLobarBronchus);
    pLeftBranches.AddChild(pLeftInferiorLobarBronchus2);
    pLeftBranches.AddChild(pLeftInferiorLobarBronchus3);
    pLeftBranches.AddChild(pLeftLingularBonchus);
    pLeftBranches.AddChild(pLeftSuperiorApicoposteriorBronchus);

    pLeftBranches.AddChild(pLeftInferiorLobePosteriorBasalBronchiole);
    pLeftBranches.AddChild(pLeftInferiorLobeLateralBasalBronchiole);
    pLeftBranches.AddChild(pLeftInferiorLobeAnteromedialBasalBronchiole);
    pLeftBranches.AddChild(pLeftInferiorLobeSuperiorBronchiole);
    pLeftBranches.AddChild(pLeftSuperiorLobeInferiorLingulaBronchiole);
    pLeftBranches.AddChild(pLeftSuperiorLobeSuperiorLingulaBronchiole);
    pLeftBranches.AddChild(pLeftSuperiorLobeAnteriorBronchiole);
    pLeftBranches.AddChild(pLeftSuperiorLobeApicoposteriorBronchiole);

    SEGasCompartment& pRightBranches = m_Compartments->CreateGasCompartment(pulse::ExpandedLungsPulmonaryCompartment::RightBranches);
    pRightBranches.AddChild(pRightMainBronchus);
    pRightBranches.AddChild(pRightIntermediateBronchus);
    pRightBranches.AddChild(pRightSuperiorLobarBronchus);
    pRightBranches.AddChild(pRightMiddleLobarBronchus);
    pRightBranches.AddChild(pRightInferiorLobarBronchus1);
    pRightBranches.AddChild(pRightInferiorLobarBronchus2);
    pRightBranches.AddChild(pRightSuperiorApicoposteriorBronchus);
    pRightBranches.AddChild(pRightInferiorLobarBronchus3);
    pRightBranches.AddChild(pRightInferiorLobarBronchus4);

    pRightBranches.AddChild(pRightSuperiorLobeApicalBronchiole);
    pRightBranches.AddChild(pRightSuperiorLobePosteriorBronchiole);
    pRightBranches.AddChild(pRightSuperiorLobeAnteriorBronchiole);
    pRightBranches.AddChild(pRightMiddleLobeLateralBronchiole);
    pRightBranches.AddChild(pRightMiddleLobeMedialBronchiole);
    pRightBranches.AddChild(pRightInferiorLobeSuperiorBronchiole);
    pRightBranches.AddChild(pRightInferiorLobeMedialBasalBronchiole);
    pRightBranches.AddChild(pRightInferiorLobeAnteriorBasalBronchiole);
    pRightBranches.AddChild(pRightInferiorLobeLateralBasalBronchiole);
    pRightBranches.AddChild(pRightInferiorLobePosteriorBasalBronchiole);

    SEGasCompartment& pLeftAnatomicDeadSpace = m_Compartments->CreateGasCompartment(pulse::PulmonaryCompartment::LeftAnatomicDeadSpace);
    pLeftAnatomicDeadSpace.AddChild(pLeftBranches);

    SEGasCompartment& pRightAnatomicDeadSpace = m_Compartments->CreateGasCompartment(pulse::PulmonaryCompartment::RightAnatomicDeadSpace);
    pRightAnatomicDeadSpace.AddChild(pRightBranches);

    SEGasCompartment& pLeftAlveolarDeadSpace = m_Compartments->CreateGasCompartment(pulse::PulmonaryCompartment::LeftAlveolarDeadSpace);
    pLeftAlveolarDeadSpace.AddChild(pLeftInferiorLobePosteriorBasalAlveolarDeadSpace);
    pLeftAlveolarDeadSpace.AddChild(pLeftInferiorLobeLateralBasalAlveolarDeadSpace);
    pLeftAlveolarDeadSpace.AddChild(pLeftInferiorLobeAnteromedialBasalAlveolarDeadSpace);
    pLeftAlveolarDeadSpace.AddChild(pLeftInferiorLobeSuperiorAlveolarDeadSpace);
    pLeftAlveolarDeadSpace.AddChild(pLeftSuperiorLobeInferiorLingulaAlveolarDeadSpace);
    pLeftAlveolarDeadSpace.AddChild(pLeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace);
    pLeftAlveolarDeadSpace.AddChild(pLeftSuperiorLobeAnteriorAlveolarDeadSpace);
    pLeftAlveolarDeadSpace.AddChild(pLeftSuperiorLobeApicoposteriorAlveolarDeadSpace);

    SEGasCompartment& pRightAlveolarDeadSpace = m_Compartments->CreateGasCompartment(pulse::PulmonaryCompartment::RightAlveolarDeadSpace);
    pRightAlveolarDeadSpace.AddChild(pRightSuperiorLobeApicalAlveolarDeadSpace);
    pRightAlveolarDeadSpace.AddChild(pRightSuperiorLobePosteriorAlveolarDeadSpace);
    pRightAlveolarDeadSpace.AddChild(pRightSuperiorLobeAnteriorAlveolarDeadSpace);
    pRightAlveolarDeadSpace.AddChild(pRightMiddleLobeLateralAlveolarDeadSpace);
    pRightAlveolarDeadSpace.AddChild(pRightMiddleLobeMedialAlveolarDeadSpace);
    pRightAlveolarDeadSpace.AddChild(pRightInferiorLobeSuperiorAlveolarDeadSpace);
    pRightAlveolarDeadSpace.AddChild(pRightInferiorLobeMedialBasalAlveolarDeadSpace);
    pRightAlveolarDeadSpace.AddChild(pRightInferiorLobeAnteriorBasalAlveolarDeadSpace);
    pRightAlveolarDeadSpace.AddChild(pRightInferiorLobeLateralBasalAlveolarDeadSpace);
    pRightAlveolarDeadSpace.AddChild(pRightInferiorLobePosteriorBasalAlveolarDeadSpace);

    SEGasCompartment& pLeftAlveoli = m_Compartments->CreateGasCompartment(pulse::PulmonaryCompartment::LeftAlveoli);
    pLeftAlveoli.AddChild(pLeftInferiorLobePosteriorBasalAlveoli);
    pLeftAlveoli.AddChild(pLeftInferiorLobeLateralBasalAlveoli);
    pLeftAlveoli.AddChild(pLeftInferiorLobeAnteromedialBasalAlveoli);
    pLeftAlveoli.AddChild(pLeftInferiorLobeSuperiorAlveoli);
    pLeftAlveoli.AddChild(pLeftSuperiorLobeInferiorLingulaAlveoli);
    pLeftAlveoli.AddChild(pLeftSuperiorLobeSuperiorLingulaAlveoli);
    pLeftAlveoli.AddChild(pLeftSuperiorLobeAnteriorAlveoli);
    pLeftAlveoli.AddChild(pLeftSuperiorLobeApicoposteriorAlveoli);

    SEGasCompartment& pRightAlveoli = m_Compartments->CreateGasCompartment(pulse::PulmonaryCompartment::RightAlveoli);
    pRightAlveoli.AddChild(pRightSuperiorLobeApicalAlveoli);
    pRightAlveoli.AddChild(pRightSuperiorLobePosteriorAlveoli);
    pRightAlveoli.AddChild(pRightSuperiorLobeAnteriorAlveoli);
    pRightAlveoli.AddChild(pRightMiddleLobeLateralAlveoli);
    pRightAlveoli.AddChild(pRightMiddleLobeMedialAlveoli);
    pRightAlveoli.AddChild(pRightInferiorLobeSuperiorAlveoli);
    pRightAlveoli.AddChild(pRightInferiorLobeMedialBasalAlveoli);
    pRightAlveoli.AddChild(pRightInferiorLobeAnteriorBasalAlveoli);
    pRightAlveoli.AddChild(pRightInferiorLobeLateralBasalAlveoli);
    pRightAlveoli.AddChild(pRightInferiorLobePosteriorBasalAlveoli);

    // Set up hierarchy
    SEGasCompartment& pLeftLung = m_Compartments->CreateGasCompartment(pulse::PulmonaryCompartment::LeftLung);
    pLeftLung.AddChild(pLeftBranches);
    pLeftLung.AddChild(pLeftAlveoli);
    SEGasCompartment& pRightLung = m_Compartments->CreateGasCompartment(pulse::PulmonaryCompartment::RightLung);
    pRightLung.AddChild(pRightBranches);
    pRightLung.AddChild(pRightAlveoli);
    SEGasCompartment& pLungs = m_Compartments->CreateGasCompartment(pulse::PulmonaryCompartment::Lungs);
    pLungs.AddChild(pLeftLung);
    pLungs.AddChild(pRightLung);
    SEGasCompartment& pPleuralCavity = m_Compartments->CreateGasCompartment(pulse::PulmonaryCompartment::PleuralCavity);
    pPleuralCavity.AddChild(pLeftPleural);
    pPleuralCavity.AddChild(pRightPleural);

    // Set up other groupings
    SEGasCompartment& pAlveoli = m_Compartments->CreateGasCompartment(pulse::PulmonaryCompartment::Alveoli);
    pAlveoli.AddChild(pLeftAlveoli);
    pAlveoli.AddChild(pRightAlveoli);
    SEGasCompartment& pAlveolarDeadSpace = m_Compartments->CreateGasCompartment(pulse::PulmonaryCompartment::AlveolarDeadSpace);
    pAlveolarDeadSpace.AddChild(pLeftAlveolarDeadSpace);
    pAlveolarDeadSpace.AddChild(pRightAlveolarDeadSpace);
    SEGasCompartment& pAnatomicDeadSpace = m_Compartments->CreateGasCompartment(pulse::PulmonaryCompartment::AnatomicDeadSpace);
    /// \TODO: Add these to the dead space, and make sure to account for it in node volume
    //pAnatomicDeadSpace.AddChild(pAirway);
    //pAnatomicDeadSpace.AddChild(pPharynx);
    //pAnatomicDeadSpace.AddChild(pCarina);
    pAnatomicDeadSpace.AddChild(pLeftAnatomicDeadSpace);
    pAnatomicDeadSpace.AddChild(pRightAnatomicDeadSpace);

    //// Setup Links //
    SEGasCompartment* gEnvironment = m_Compartments->GetGasCompartment(pulse::EnvironmentCompartment::Ambient);
    SEGasCompartmentLink& pEnvironmentToAirway = m_Compartments->CreateGasLink(*gEnvironment, pAirway, pulse::PulmonaryLink::EnvironmentToAirway);
    pEnvironmentToAirway.MapPath(EnvironmentToAirway);
    SEGasCompartmentLink& pAirwayToPharynx = m_Compartments->CreateGasLink(pAirway, pPharynx, pulse::PulmonaryLink::AirwayToPharynx);
    pAirwayToPharynx.MapPath(AirwayToPharynx);
    SEGasCompartmentLink& pPharynxToCarina = m_Compartments->CreateGasLink(pPharynx, pCarina, pulse::PulmonaryLink::PharynxToCarina);
    pPharynxToCarina.MapPath(PharynxToCarina);
    SEGasCompartmentLink& pPharynxToEnvironment = m_Compartments->CreateGasLink(pPharynx, *gEnvironment, pulse::PulmonaryLink::PharynxToEnvironment);
    pPharynxToEnvironment.MapPath(PharynxToEnvironment);
    SEGasCompartmentLink& pAirwayToStomach = m_Compartments->CreateGasLink(pAirway, pStomach, pulse::PulmonaryLink::AirwayToStomach);
    pAirwayToStomach.MapPath(AirwayToStomach);

    SEGasCompartmentLink& pCarinaToRightMainBronchus = m_Compartments->CreateGasLink(pCarina, pRightMainBronchus, pulse::ExpandedLungsPulmonaryLink::CarinaToRightMainBronchus);
    SEGasCompartmentLink& pRightMainBronchusToRightIntermediateBronchus = m_Compartments->CreateGasLink(pRightMainBronchus, pRightIntermediateBronchus, pulse::ExpandedLungsPulmonaryLink::RightMainBronchusToRightIntermediateBronchus);
    SEGasCompartmentLink& pRightMainBronchusToRightSuperiorLobarBronchus = m_Compartments->CreateGasLink(pRightMainBronchus, pRightSuperiorLobarBronchus, pulse::ExpandedLungsPulmonaryLink::RightMainBronchusToRightSuperiorLobarBronchus);
    SEGasCompartmentLink& pRightIntermediateBronchusToRightMiddleLobarBronchus = m_Compartments->CreateGasLink(pRightIntermediateBronchus, pRightMiddleLobarBronchus, pulse::ExpandedLungsPulmonaryLink::RightIntermediateBronchusToRightMiddleLobarBronchus);
    SEGasCompartmentLink& pRightIntermediateBronchusToRightInferiorLobarBronchus1 = m_Compartments->CreateGasLink(pRightIntermediateBronchus, pRightInferiorLobarBronchus1, pulse::ExpandedLungsPulmonaryLink::RightIntermediateBronchusToRightInferiorLobarBronchus1);
    SEGasCompartmentLink& pRightInferiorLobarBronchus1ToRightInferiorLobarBronchus2 = m_Compartments->CreateGasLink(pRightInferiorLobarBronchus1, pRightInferiorLobarBronchus2, pulse::ExpandedLungsPulmonaryLink::RightInferiorLobarBronchus1ToRightInferiorLobarBronchus2);
    SEGasCompartmentLink& pRightSuperiorLobarBronchusToRightSuperiorApicoposteriorBronchus = m_Compartments->CreateGasLink(pRightSuperiorLobarBronchus, pRightSuperiorApicoposteriorBronchus, pulse::ExpandedLungsPulmonaryLink::RightSuperiorLobarBronchusToRightSuperiorApicoposteriorBronchus);
    SEGasCompartmentLink& pRightInferiorLobarBronchus2ToRightInferiorLobarBronchus3 = m_Compartments->CreateGasLink(pRightInferiorLobarBronchus2, pRightInferiorLobarBronchus3, pulse::ExpandedLungsPulmonaryLink::RightInferiorLobarBronchus2ToRightInferiorLobarBronchus3);
    SEGasCompartmentLink& pRightInferiorLobarBronchus2ToRightInferiorLobarBronchus4 = m_Compartments->CreateGasLink(pRightInferiorLobarBronchus2, pRightInferiorLobarBronchus4, pulse::ExpandedLungsPulmonaryLink::RightInferiorLobarBronchus2ToRightInferiorLobarBronchus4);

    SEGasCompartmentLink& pCarinaToLeftMainBronchus = m_Compartments->CreateGasLink(pCarina, pLeftMainBronchus, pulse::ExpandedLungsPulmonaryLink::CarinaToLeftMainBronchus);
    SEGasCompartmentLink& pLeftMainBronchusToLeftInferiorLobarBronchus1 = m_Compartments->CreateGasLink(pLeftMainBronchus, pLeftInferiorLobarBronchus1, pulse::ExpandedLungsPulmonaryLink::LeftMainBronchusToLeftInferiorLobarBronchus1);
    SEGasCompartmentLink& pLeftMainBronchusToLeftSuperiorLobarBronchus = m_Compartments->CreateGasLink(pLeftMainBronchus, pLeftSuperiorLobarBronchus, pulse::ExpandedLungsPulmonaryLink::LeftMainBronchusToLeftSuperiorLobarBronchus);
    SEGasCompartmentLink& pLeftInferiorLobarBronchus1ToLeftInferiorLobarBronchus2 = m_Compartments->CreateGasLink(pLeftInferiorLobarBronchus1, pLeftInferiorLobarBronchus2, pulse::ExpandedLungsPulmonaryLink::LeftInferiorLobarBronchus1ToLeftInferiorLobarBronchus2);
    SEGasCompartmentLink& pLeftInferiorLobarBronchus2ToLeftInferiorLobarBronchus3 = m_Compartments->CreateGasLink(pLeftInferiorLobarBronchus2, pLeftInferiorLobarBronchus3, pulse::ExpandedLungsPulmonaryLink::LeftInferiorLobarBronchus2ToLeftInferiorLobarBronchus3);
    SEGasCompartmentLink& pLeftSuperiorLobarBronchusToLeftLingularBonchus = m_Compartments->CreateGasLink(pLeftSuperiorLobarBronchus, pLeftLingularBonchus, pulse::ExpandedLungsPulmonaryLink::LeftSuperiorLobarBronchusToLeftLingularBonchus);
    SEGasCompartmentLink& pLeftSuperiorLobarBronchusToLeftSuperiorApicoposteriorBronchus = m_Compartments->CreateGasLink(pLeftSuperiorLobarBronchus, pLeftSuperiorApicoposteriorBronchus, pulse::ExpandedLungsPulmonaryLink::LeftSuperiorLobarBronchusToLeftSuperiorApicoposteriorBronchus);

    SEGasCompartmentLink& pRightSuperiorApicoposteriorBronchusToRightSuperiorLobeApicalBronchiole = m_Compartments->CreateGasLink(pRightSuperiorApicoposteriorBronchus, pRightSuperiorLobeApicalBronchiole, pulse::ExpandedLungsPulmonaryLink::RightSuperiorApicoposteriorBronchusToRightSuperiorLobeApicalBronchiole);
    SEGasCompartmentLink& pRightSuperiorApicoposteriorBronchusToRightSuperiorLobePosteriorBronchiole = m_Compartments->CreateGasLink(pRightSuperiorApicoposteriorBronchus, pRightSuperiorLobePosteriorBronchiole, pulse::ExpandedLungsPulmonaryLink::RightSuperiorApicoposteriorBronchusToRightSuperiorLobePosteriorBronchiole);
    SEGasCompartmentLink& pRightSuperiorLobarBronchusToRightSuperiorLobeAnteriorBronchiole = m_Compartments->CreateGasLink(pRightSuperiorLobarBronchus, pRightSuperiorLobeAnteriorBronchiole, pulse::ExpandedLungsPulmonaryLink::RightSuperiorLobarBronchusToRightSuperiorLobeAnteriorBronchiole);
    SEGasCompartmentLink& pRightMiddleLobarBronchusToRightMiddleLobeLateralBronchiole = m_Compartments->CreateGasLink(pRightMiddleLobarBronchus, pRightMiddleLobeLateralBronchiole, pulse::ExpandedLungsPulmonaryLink::RightMiddleLobarBronchusToRightMiddleLobeLateralBronchiole);
    SEGasCompartmentLink& pRightMiddleLobarBronchusToRightMiddleLobeMedialBronchiole = m_Compartments->CreateGasLink(pRightMiddleLobarBronchus, pRightMiddleLobeMedialBronchiole, pulse::ExpandedLungsPulmonaryLink::RightMiddleLobarBronchusToRightMiddleLobeMedialBronchiole);
    SEGasCompartmentLink& pRightInferiorLobarBronchus1ToRightInferiorLobeSuperiorBronchiole = m_Compartments->CreateGasLink(pRightInferiorLobarBronchus1, pRightInferiorLobeSuperiorBronchiole, pulse::ExpandedLungsPulmonaryLink::RightInferiorLobarBronchus1ToRightInferiorLobeSuperiorBronchiole);
    SEGasCompartmentLink& pRightInferiorLobarBronchus2ToRightInferiorLobeMedialBasalBronchiole = m_Compartments->CreateGasLink(pRightInferiorLobarBronchus2, pRightInferiorLobeMedialBasalBronchiole, pulse::ExpandedLungsPulmonaryLink::RightInferiorLobarBronchus2ToRightInferiorLobeMedialBasalBronchiole);
    SEGasCompartmentLink& pRightInferiorLobarBronchus3ToRightInferiorLobeAnteriorBasalBronchiole = m_Compartments->CreateGasLink(pRightInferiorLobarBronchus3, pRightInferiorLobeAnteriorBasalBronchiole, pulse::ExpandedLungsPulmonaryLink::RightInferiorLobarBronchus3ToRightInferiorLobeAnteriorBasalBronchiole);
    SEGasCompartmentLink& pRightInferiorLobarBronchus4ToRightInferiorLobeLateralBasalBronchiole = m_Compartments->CreateGasLink(pRightInferiorLobarBronchus4, pRightInferiorLobeLateralBasalBronchiole, pulse::ExpandedLungsPulmonaryLink::RightInferiorLobarBronchus4ToRightInferiorLobeLateralBasalBronchiole);
    SEGasCompartmentLink& pRightInferiorLobarBronchus4ToRightInferiorLobePosteriorBasalBronchiole = m_Compartments->CreateGasLink(pRightInferiorLobarBronchus4, pRightInferiorLobePosteriorBasalBronchiole, pulse::ExpandedLungsPulmonaryLink::RightInferiorLobarBronchus4ToRightInferiorLobePosteriorBasalBronchiole);

    SEGasCompartmentLink& pLeftInferiorLobarBronchus3ToLeftInferiorLobeLateralBasalBronchiole = m_Compartments->CreateGasLink(pLeftInferiorLobarBronchus3, pLeftInferiorLobeLateralBasalBronchiole, pulse::ExpandedLungsPulmonaryLink::LeftInferiorLobarBronchus3ToLeftInferiorLobeLateralBasalBronchiole);
    SEGasCompartmentLink& pLeftInferiorLobarBronchus3ToLeftInferiorLobePosteriorBasalBronchiole = m_Compartments->CreateGasLink(pLeftInferiorLobarBronchus3, pLeftInferiorLobePosteriorBasalBronchiole, pulse::ExpandedLungsPulmonaryLink::LeftInferiorLobarBronchus3ToLeftInferiorLobePosteriorBasalBronchiole);
    SEGasCompartmentLink& pLeftInferiorLobarBronchus2ToLeftInferiorLobeAnteromedialBasalBronchiole = m_Compartments->CreateGasLink(pLeftInferiorLobarBronchus2, pLeftInferiorLobeAnteromedialBasalBronchiole, pulse::ExpandedLungsPulmonaryLink::LeftInferiorLobarBronchus2ToLeftInferiorLobeAnteromedialBasalBronchiole);
    SEGasCompartmentLink& pLeftInferiorLobarBronchus1ToLeftInferiorLobeSuperiorBronchiole = m_Compartments->CreateGasLink(pLeftInferiorLobarBronchus1, pLeftInferiorLobeSuperiorBronchiole, pulse::ExpandedLungsPulmonaryLink::LeftInferiorLobarBronchus1ToLeftInferiorLobeSuperiorBronchiole);
    SEGasCompartmentLink& pLeftLingularBonchusToLeftSuperiorLobeInferiorLingulaBronchiole = m_Compartments->CreateGasLink(pLeftLingularBonchus, pLeftSuperiorLobeInferiorLingulaBronchiole, pulse::ExpandedLungsPulmonaryLink::LeftLingularBonchusToLeftSuperiorLobeInferiorLingulaBronchiole);
    SEGasCompartmentLink& pLeftLingularBonchusToLeftSuperiorLobeSuperiorLingulaBronchiole = m_Compartments->CreateGasLink(pLeftLingularBonchus, pLeftSuperiorLobeSuperiorLingulaBronchiole, pulse::ExpandedLungsPulmonaryLink::LeftLingularBonchusToLeftSuperiorLobeSuperiorLingulaBronchiole);
    SEGasCompartmentLink& pLeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeAnteriorBronchiole = m_Compartments->CreateGasLink(pLeftSuperiorApicoposteriorBronchus, pLeftSuperiorLobeAnteriorBronchiole, pulse::ExpandedLungsPulmonaryLink::LeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeAnteriorBronchiole);
    SEGasCompartmentLink& pLeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeApicoposteriorBronchiole = m_Compartments->CreateGasLink(pLeftSuperiorApicoposteriorBronchus, pLeftSuperiorLobeApicoposteriorBronchiole, pulse::ExpandedLungsPulmonaryLink::LeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeApicoposteriorBronchiole);

    SEGasCompartmentLink& pRightSuperiorLobeApicalBronchioleToRightSuperiorLobeApicalAlveolarDeadSpace = m_Compartments->CreateGasLink(pRightSuperiorLobeApicalBronchiole, pRightSuperiorLobeApicalAlveolarDeadSpace, pulse::ExpandedLungsPulmonaryLink::RightSuperiorLobeApicalBronchioleToRightSuperiorLobeApicalAlveolarDeadSpace);
    SEGasCompartmentLink& pRightSuperiorLobePosteriorBronchioleToRightSuperiorLobePosteriorAlveolarDeadSpace = m_Compartments->CreateGasLink(pRightSuperiorLobePosteriorBronchiole, pRightSuperiorLobePosteriorAlveolarDeadSpace, pulse::ExpandedLungsPulmonaryLink::RightSuperiorLobePosteriorBronchioleToRightSuperiorLobePosteriorAlveolarDeadSpace);
    SEGasCompartmentLink& pRightSuperiorLobeAnteriorBronchioleToRightSuperiorLobeAnteriorAlveolarDeadSpace = m_Compartments->CreateGasLink(pRightSuperiorLobeAnteriorBronchiole, pRightSuperiorLobeAnteriorAlveolarDeadSpace, pulse::ExpandedLungsPulmonaryLink::RightSuperiorLobeAnteriorBronchioleToRightSuperiorLobeAnteriorAlveolarDeadSpace);
    SEGasCompartmentLink& pRightMiddleLobeLateralBronchioleToRightMiddleLobeLateralAlveolarDeadSpace = m_Compartments->CreateGasLink(pRightMiddleLobeLateralBronchiole, pRightMiddleLobeLateralAlveolarDeadSpace, pulse::ExpandedLungsPulmonaryLink::RightMiddleLobeLateralBronchioleToRightMiddleLobeLateralAlveolarDeadSpace);
    SEGasCompartmentLink& pRightMiddleLobeMedialBronchioleToRightMiddleLobeMedialAlveolarDeadSpace = m_Compartments->CreateGasLink(pRightMiddleLobeMedialBronchiole, pRightMiddleLobeMedialAlveolarDeadSpace, pulse::ExpandedLungsPulmonaryLink::RightMiddleLobeMedialBronchioleToRightMiddleLobeMedialAlveolarDeadSpace);
    SEGasCompartmentLink& pRightInferiorLobeSuperiorBronchioleToRightInferiorLobeSuperiorAlveolarDeadSpace = m_Compartments->CreateGasLink(pRightInferiorLobeSuperiorBronchiole, pRightInferiorLobeSuperiorAlveolarDeadSpace, pulse::ExpandedLungsPulmonaryLink::RightInferiorLobeSuperiorBronchioleToRightInferiorLobeSuperiorAlveolarDeadSpace);
    SEGasCompartmentLink& pRightInferiorLobeMedialBasalBronchioleToRightInferiorLobeMedialBasalAlveolarDeadSpace = m_Compartments->CreateGasLink(pRightInferiorLobeMedialBasalBronchiole, pRightInferiorLobeMedialBasalAlveolarDeadSpace, pulse::ExpandedLungsPulmonaryLink::RightInferiorLobeMedialBasalBronchioleToRightInferiorLobeMedialBasalAlveolarDeadSpace);
    SEGasCompartmentLink& pRightInferiorLobeAnteriorBasalBronchioleToRightInferiorLobeAnteriorBasalAlveolarDeadSpace = m_Compartments->CreateGasLink(pRightInferiorLobeAnteriorBasalBronchiole, pRightInferiorLobeAnteriorBasalAlveolarDeadSpace, pulse::ExpandedLungsPulmonaryLink::RightInferiorLobeAnteriorBasalBronchioleToRightInferiorLobeAnteriorBasalAlveolarDeadSpace);
    SEGasCompartmentLink& pRightInferiorLobeLateralBasalBronchioleToRightInferiorLobeLateralBasalAlveolarDeadSpace = m_Compartments->CreateGasLink(pRightInferiorLobeLateralBasalBronchiole, pRightInferiorLobeLateralBasalAlveolarDeadSpace, pulse::ExpandedLungsPulmonaryLink::RightInferiorLobeLateralBasalBronchioleToRightInferiorLobeLateralBasalAlveolarDeadSpace);
    SEGasCompartmentLink& pRightInferiorLobePosteriorBasalBronchioleToRightInferiorLobePosteriorBasalAlveolarDeadSpace = m_Compartments->CreateGasLink(pRightInferiorLobePosteriorBasalBronchiole, pRightInferiorLobePosteriorBasalAlveolarDeadSpace, pulse::ExpandedLungsPulmonaryLink::RightInferiorLobePosteriorBasalBronchioleToRightInferiorLobePosteriorBasalAlveolarDeadSpace);
    
    SEGasCompartmentLink& pLeftInferiorLobePosteriorBasalBronchioleToLeftInferiorLobePosteriorBasalAlveolarDeadSpace = m_Compartments->CreateGasLink(pLeftInferiorLobePosteriorBasalBronchiole, pLeftInferiorLobePosteriorBasalAlveolarDeadSpace, pulse::ExpandedLungsPulmonaryLink::LeftInferiorLobePosteriorBasalBronchioleToLeftInferiorLobePosteriorBasalAlveolarDeadSpace);
    SEGasCompartmentLink& pLeftInferiorLobeLateralBasalBronchioleToLeftInferiorLobeLateralBasalAlveolarDeadSpace = m_Compartments->CreateGasLink(pLeftInferiorLobeLateralBasalBronchiole, pLeftInferiorLobeLateralBasalAlveolarDeadSpace, pulse::ExpandedLungsPulmonaryLink::LeftInferiorLobeLateralBasalBronchioleToLeftInferiorLobeLateralBasalAlveolarDeadSpace);
    SEGasCompartmentLink& pLeftInferiorLobeAnteromedialBasalBronchioleToLeftInferiorLobeAnteromedialBasalAlveolarDeadSpace = m_Compartments->CreateGasLink(pLeftInferiorLobeAnteromedialBasalBronchiole, pLeftInferiorLobeAnteromedialBasalAlveolarDeadSpace, pulse::ExpandedLungsPulmonaryLink::LeftInferiorLobeAnteromedialBasalBronchioleToLeftInferiorLobeAnteromedialBasalAlveolarDeadSpace);
    SEGasCompartmentLink& pLeftInferiorLobeSuperiorBronchioleToLeftInferiorLobeSuperiorAlveolarDeadSpace = m_Compartments->CreateGasLink(pLeftInferiorLobeSuperiorBronchiole, pLeftInferiorLobeSuperiorAlveolarDeadSpace, pulse::ExpandedLungsPulmonaryLink::LeftInferiorLobeSuperiorBronchioleToLeftInferiorLobeSuperiorAlveolarDeadSpace);
    SEGasCompartmentLink& pLeftSuperiorLobeInferiorLingulaBronchioleToLeftSuperiorLobeInferiorLingulaAlveolarDeadSpace = m_Compartments->CreateGasLink(pLeftSuperiorLobeInferiorLingulaBronchiole, pLeftSuperiorLobeInferiorLingulaAlveolarDeadSpace, pulse::ExpandedLungsPulmonaryLink::LeftSuperiorLobeInferiorLingulaBronchioleToLeftSuperiorLobeInferiorLingulaAlveolarDeadSpace);
    SEGasCompartmentLink& pLeftSuperiorLobeSuperiorLingulaBronchioleToLeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace = m_Compartments->CreateGasLink(pLeftSuperiorLobeSuperiorLingulaBronchiole, pLeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace, pulse::ExpandedLungsPulmonaryLink::LeftSuperiorLobeSuperiorLingulaBronchioleToLeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace);
    SEGasCompartmentLink& pLeftSuperiorLobeAnteriorBronchioleToLeftSuperiorLobeAnteriorAlveolarDeadSpace = m_Compartments->CreateGasLink(pLeftSuperiorLobeAnteriorBronchiole, pLeftSuperiorLobeAnteriorAlveolarDeadSpace, pulse::ExpandedLungsPulmonaryLink::LeftSuperiorLobeAnteriorBronchioleToLeftSuperiorLobeAnteriorAlveolarDeadSpace);
    SEGasCompartmentLink& pLeftSuperiorLobeApicoposteriorBronchioleToLeftSuperiorLobeApicoposteriorAlveolarDeadSpace = m_Compartments->CreateGasLink(pLeftSuperiorLobeApicoposteriorBronchiole, pLeftSuperiorLobeApicoposteriorAlveolarDeadSpace, pulse::ExpandedLungsPulmonaryLink::LeftSuperiorLobeApicoposteriorBronchioleToLeftSuperiorLobeApicoposteriorAlveolarDeadSpace);
   
    SEGasCompartmentLink& pRightSuperiorLobeApicalAlveolarDeadSpaceToRightSuperiorLobeApicalAlveoli = m_Compartments->CreateGasLink(pRightSuperiorLobeApicalAlveolarDeadSpace, pRightSuperiorLobeApicalAlveoli, pulse::ExpandedLungsPulmonaryLink::RightSuperiorLobeApicalAlveolarDeadSpaceToRightSuperiorLobeApicalAlveoli);
    SEGasCompartmentLink& pRightSuperiorLobePosteriorAlveolarDeadSpaceToRightSuperiorLobePosteriorAlveoli = m_Compartments->CreateGasLink(pRightSuperiorLobePosteriorAlveolarDeadSpace, pRightSuperiorLobePosteriorAlveoli, pulse::ExpandedLungsPulmonaryLink::RightSuperiorLobePosteriorAlveolarDeadSpaceToRightSuperiorLobePosteriorAlveoli);
    SEGasCompartmentLink& pRightSuperiorLobeAnteriorAlveolarDeadSpaceToRightSuperiorLobeAnteriorAlveoli = m_Compartments->CreateGasLink(pRightSuperiorLobeAnteriorAlveolarDeadSpace, pRightSuperiorLobeAnteriorAlveoli, pulse::ExpandedLungsPulmonaryLink::RightSuperiorLobeAnteriorAlveolarDeadSpaceToRightSuperiorLobeAnteriorAlveoli);
    SEGasCompartmentLink& pRightMiddleLobeLateralAlveolarDeadSpaceToRightMiddleLobeLateralAlveoli = m_Compartments->CreateGasLink(pRightMiddleLobeLateralAlveolarDeadSpace, pRightMiddleLobeLateralAlveoli, pulse::ExpandedLungsPulmonaryLink::RightMiddleLobeLateralAlveolarDeadSpaceToRightMiddleLobeLateralAlveoli);
    SEGasCompartmentLink& pRightMiddleLobeMedialAlveolarDeadSpaceToRightMiddleLobeMedialAlveoli = m_Compartments->CreateGasLink(pRightMiddleLobeMedialAlveolarDeadSpace, pRightMiddleLobeMedialAlveoli, pulse::ExpandedLungsPulmonaryLink::RightMiddleLobeMedialAlveolarDeadSpaceToRightMiddleLobeMedialAlveoli);
    SEGasCompartmentLink& pRightInferiorLobeSuperiorAlveolarDeadSpaceToRightInferiorLobeSuperiorAlveoli = m_Compartments->CreateGasLink(pRightInferiorLobeSuperiorAlveolarDeadSpace, pRightInferiorLobeSuperiorAlveoli, pulse::ExpandedLungsPulmonaryLink::RightInferiorLobeSuperiorAlveolarDeadSpaceToRightInferiorLobeSuperiorAlveoli);
    SEGasCompartmentLink& pRightInferiorLobeMedialBasalAlveolarDeadSpaceToRightInferiorLobeMedialBasalAlveoli = m_Compartments->CreateGasLink(pRightInferiorLobeMedialBasalAlveolarDeadSpace, pRightInferiorLobeMedialBasalAlveoli, pulse::ExpandedLungsPulmonaryLink::RightInferiorLobeMedialBasalAlveolarDeadSpaceToRightInferiorLobeMedialBasalAlveoli);
    SEGasCompartmentLink& pRightInferiorLobeAnteriorBasalAlveolarDeadSpaceToRightInferiorLobeAnteriorBasalAlveoli = m_Compartments->CreateGasLink(pRightInferiorLobeAnteriorBasalAlveolarDeadSpace, pRightInferiorLobeAnteriorBasalAlveoli, pulse::ExpandedLungsPulmonaryLink::RightInferiorLobeAnteriorBasalAlveolarDeadSpaceToRightInferiorLobeAnteriorBasalAlveoli);
    SEGasCompartmentLink& pRightInferiorLobeLateralBasalAlveolarDeadSpaceToRightInferiorLobeLateralBasalAlveoli = m_Compartments->CreateGasLink(pRightInferiorLobeLateralBasalAlveolarDeadSpace, pRightInferiorLobeLateralBasalAlveoli, pulse::ExpandedLungsPulmonaryLink::RightInferiorLobeLateralBasalAlveolarDeadSpaceToRightInferiorLobeLateralBasalAlveoli);
    SEGasCompartmentLink& pRightInferiorLobePosteriorBasalAlveolarDeadSpaceToRightInferiorLobePosteriorBasalAlveoli = m_Compartments->CreateGasLink(pRightInferiorLobePosteriorBasalAlveolarDeadSpace, pRightInferiorLobePosteriorBasalAlveoli, pulse::ExpandedLungsPulmonaryLink::RightInferiorLobePosteriorBasalAlveolarDeadSpaceToRightInferiorLobePosteriorBasalAlveoli);
    
    SEGasCompartmentLink& pLeftInferiorLobePosteriorBasalAlveolarDeadSpaceToLeftInferiorLobePosteriorBasalAlveoli = m_Compartments->CreateGasLink(pLeftInferiorLobePosteriorBasalAlveolarDeadSpace, pLeftInferiorLobePosteriorBasalAlveoli, pulse::ExpandedLungsPulmonaryLink::LeftInferiorLobePosteriorBasalAlveolarDeadSpaceToLeftInferiorLobePosteriorBasalAlveoli);
    SEGasCompartmentLink& pLeftInferiorLobeLateralBasalAlveolarDeadSpaceToLeftInferiorLobeLateralBasalAlveoli = m_Compartments->CreateGasLink(pLeftInferiorLobeLateralBasalAlveolarDeadSpace, pLeftInferiorLobeLateralBasalAlveoli, pulse::ExpandedLungsPulmonaryLink::LeftInferiorLobeLateralBasalAlveolarDeadSpaceToLeftInferiorLobeLateralBasalAlveoli);
    SEGasCompartmentLink& pLeftInferiorLobeAnteromedialBasalAlveolarDeadSpaceToLeftInferiorLobeAnteromedialBasalAlveoli = m_Compartments->CreateGasLink(pLeftInferiorLobeAnteromedialBasalAlveolarDeadSpace, pLeftInferiorLobeAnteromedialBasalAlveoli, pulse::ExpandedLungsPulmonaryLink::LeftInferiorLobeAnteromedialBasalAlveolarDeadSpaceToLeftInferiorLobeAnteromedialBasalAlveoli);
    SEGasCompartmentLink& pLeftInferiorLobeSuperiorAlveolarDeadSpaceToLeftInferiorLobeSuperiorAlveoli = m_Compartments->CreateGasLink(pLeftInferiorLobeSuperiorAlveolarDeadSpace, pLeftInferiorLobeSuperiorAlveoli, pulse::ExpandedLungsPulmonaryLink::LeftInferiorLobeSuperiorAlveolarDeadSpaceToLeftInferiorLobeSuperiorAlveoli);
    SEGasCompartmentLink& pLeftSuperiorLobeInferiorLingulaAlveolarDeadSpaceToLeftSuperiorLobeInferiorLingulaAlveoli = m_Compartments->CreateGasLink(pLeftSuperiorLobeInferiorLingulaAlveolarDeadSpace, pLeftSuperiorLobeInferiorLingulaAlveoli, pulse::ExpandedLungsPulmonaryLink::LeftSuperiorLobeInferiorLingulaAlveolarDeadSpaceToLeftSuperiorLobeInferiorLingulaAlveoli);
    SEGasCompartmentLink& pLeftSuperiorLobeSuperiorLingulaAlveolarDeadSpaceToLeftSuperiorLobeSuperiorLingulaAlveoli = m_Compartments->CreateGasLink(pLeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace, pLeftSuperiorLobeSuperiorLingulaAlveoli, pulse::ExpandedLungsPulmonaryLink::LeftSuperiorLobeSuperiorLingulaAlveolarDeadSpaceToLeftSuperiorLobeSuperiorLingulaAlveoli);
    SEGasCompartmentLink& pLeftSuperiorLobeAnteriorAlveolarDeadSpaceToLeftSuperiorLobeAnteriorAlveoli = m_Compartments->CreateGasLink(pLeftSuperiorLobeAnteriorAlveolarDeadSpace, pLeftSuperiorLobeAnteriorAlveoli, pulse::ExpandedLungsPulmonaryLink::LeftSuperiorLobeAnteriorAlveolarDeadSpaceToLeftSuperiorLobeAnteriorAlveoli);
    SEGasCompartmentLink& pLeftSuperiorLobeApicoposteriorAlveolarDeadSpaceToLeftSuperiorLobeApicoposteriorAlveoli = m_Compartments->CreateGasLink(pLeftSuperiorLobeApicoposteriorAlveolarDeadSpace, pLeftSuperiorLobeApicoposteriorAlveoli, pulse::ExpandedLungsPulmonaryLink::LeftSuperiorLobeApicoposteriorAlveolarDeadSpaceToLeftSuperiorLobeApicoposteriorAlveoli);

    pCarinaToRightMainBronchus.MapPath(CarinaToRightMainBronchus);
    pRightMainBronchusToRightIntermediateBronchus.MapPath(RightMainBronchusToRightIntermediateBronchus);
    pRightMainBronchusToRightSuperiorLobarBronchus.MapPath(RightMainBronchusToRightSuperiorLobarBronchus);
    pRightIntermediateBronchusToRightMiddleLobarBronchus.MapPath(RightIntermediateBronchusToRightMiddleLobarBronchus);
    pRightIntermediateBronchusToRightInferiorLobarBronchus1.MapPath(RightIntermediateBronchusToRightInferiorLobarBronchus1);
    pRightInferiorLobarBronchus1ToRightInferiorLobarBronchus2.MapPath(RightInferiorLobarBronchus1ToRightInferiorLobarBronchus2);
    pRightSuperiorLobarBronchusToRightSuperiorApicoposteriorBronchus.MapPath(RightSuperiorLobarBronchusToRightSuperiorApicoposteriorBronchus);
    pRightInferiorLobarBronchus2ToRightInferiorLobarBronchus3.MapPath(RightInferiorLobarBronchus2ToRightInferiorLobarBronchus3);
    pRightInferiorLobarBronchus2ToRightInferiorLobarBronchus4.MapPath(RightInferiorLobarBronchus2ToRightInferiorLobarBronchus4);

    pCarinaToLeftMainBronchus.MapPath(CarinaToLeftMainBronchus);
    pLeftMainBronchusToLeftInferiorLobarBronchus1.MapPath(LeftMainBronchusToLeftInferiorLobarBronchus1);
    pLeftMainBronchusToLeftSuperiorLobarBronchus.MapPath(LeftMainBronchusToLeftSuperiorLobarBronchus);
    pLeftInferiorLobarBronchus1ToLeftInferiorLobarBronchus2.MapPath(LeftInferiorLobarBronchus1ToLeftInferiorLobarBronchus2);
    pLeftInferiorLobarBronchus2ToLeftInferiorLobarBronchus3.MapPath(LeftInferiorLobarBronchus2ToLeftInferiorLobarBronchus3);
    pLeftSuperiorLobarBronchusToLeftLingularBonchus.MapPath(LeftSuperiorLobarBronchusToLeftLingularBonchus);
    pLeftSuperiorLobarBronchusToLeftSuperiorApicoposteriorBronchus.MapPath(LeftSuperiorLobarBronchusToLeftSuperiorApicoposteriorBronchus);

    pRightSuperiorApicoposteriorBronchusToRightSuperiorLobeApicalBronchiole.MapPath(RightSuperiorApicoposteriorBronchusToRightSuperiorLobeApicalBronchiole);
    pRightSuperiorApicoposteriorBronchusToRightSuperiorLobePosteriorBronchiole.MapPath(RightSuperiorApicoposteriorBronchusToRightSuperiorLobePosteriorBronchiole);
    pRightSuperiorLobarBronchusToRightSuperiorLobeAnteriorBronchiole.MapPath(RightSuperiorLobarBronchusToRightSuperiorLobeAnteriorBronchiole);
    pRightMiddleLobarBronchusToRightMiddleLobeLateralBronchiole.MapPath(RightMiddleLobarBronchusToRightMiddleLobeLateralBronchiole);
    pRightMiddleLobarBronchusToRightMiddleLobeMedialBronchiole.MapPath(RightMiddleLobarBronchusToRightMiddleLobeMedialBronchiole);
    pRightInferiorLobarBronchus1ToRightInferiorLobeSuperiorBronchiole.MapPath(RightInferiorLobarBronchus1ToRightInferiorLobeSuperiorBronchiole);
    pRightInferiorLobarBronchus2ToRightInferiorLobeMedialBasalBronchiole.MapPath(RightInferiorLobarBronchus2ToRightInferiorLobeMedialBasalBronchiole);
    pRightInferiorLobarBronchus3ToRightInferiorLobeAnteriorBasalBronchiole.MapPath(RightInferiorLobarBronchus3ToRightInferiorLobeAnteriorBasalBronchiole);
    pRightInferiorLobarBronchus4ToRightInferiorLobeLateralBasalBronchiole.MapPath(RightInferiorLobarBronchus4ToRightInferiorLobeLateralBasalBronchiole);
    pRightInferiorLobarBronchus4ToRightInferiorLobePosteriorBasalBronchiole.MapPath(RightInferiorLobarBronchus4ToRightInferiorLobePosteriorBasalBronchiole);

    pLeftInferiorLobarBronchus3ToLeftInferiorLobeLateralBasalBronchiole.MapPath(LeftInferiorLobarBronchus3ToLeftInferiorLobeLateralBasalBronchiole);
    pLeftInferiorLobarBronchus3ToLeftInferiorLobePosteriorBasalBronchiole.MapPath(LeftInferiorLobarBronchus3ToLeftInferiorLobePosteriorBasalBronchiole);
    pLeftInferiorLobarBronchus2ToLeftInferiorLobeAnteromedialBasalBronchiole.MapPath(LeftInferiorLobarBronchus2ToLeftInferiorLobeAnteromedialBasalBronchiole);
    pLeftInferiorLobarBronchus1ToLeftInferiorLobeSuperiorBronchiole.MapPath(LeftInferiorLobarBronchus1ToLeftInferiorLobeSuperiorBronchiole);
    pLeftLingularBonchusToLeftSuperiorLobeInferiorLingulaBronchiole.MapPath(LeftLingularBonchusToLeftSuperiorLobeInferiorLingulaBronchiole);
    pLeftLingularBonchusToLeftSuperiorLobeSuperiorLingulaBronchiole.MapPath(LeftLingularBonchusToLeftSuperiorLobeSuperiorLingulaBronchiole);
    pLeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeAnteriorBronchiole.MapPath(LeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeAnteriorBronchiole);
    pLeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeApicoposteriorBronchiole.MapPath(LeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeApicoposteriorBronchiole);

    pRightSuperiorLobeApicalBronchioleToRightSuperiorLobeApicalAlveolarDeadSpace.MapPath(RightSuperiorLobeApicalBronchioleToRightSuperiorLobeApicalAlveolarDeadSpace);
    pRightSuperiorLobePosteriorBronchioleToRightSuperiorLobePosteriorAlveolarDeadSpace.MapPath(RightSuperiorLobePosteriorBronchioleToRightSuperiorLobePosteriorAlveolarDeadSpace);
    pRightSuperiorLobeAnteriorBronchioleToRightSuperiorLobeAnteriorAlveolarDeadSpace.MapPath(RightSuperiorLobeAnteriorBronchioleToRightSuperiorLobeAnteriorAlveolarDeadSpace);
    pRightMiddleLobeLateralBronchioleToRightMiddleLobeLateralAlveolarDeadSpace.MapPath(RightMiddleLobeLateralBronchioleToRightMiddleLobeLateralAlveolarDeadSpace);
    pRightMiddleLobeMedialBronchioleToRightMiddleLobeMedialAlveolarDeadSpace.MapPath(RightMiddleLobeMedialBronchioleToRightMiddleLobeMedialAlveolarDeadSpace);
    pRightInferiorLobeSuperiorBronchioleToRightInferiorLobeSuperiorAlveolarDeadSpace.MapPath(RightInferiorLobeSuperiorBronchioleToRightInferiorLobeSuperiorAlveolarDeadSpace);
    pRightInferiorLobeMedialBasalBronchioleToRightInferiorLobeMedialBasalAlveolarDeadSpace.MapPath(RightInferiorLobeMedialBasalBronchioleToRightInferiorLobeMedialBasalAlveolarDeadSpace);
    pRightInferiorLobeAnteriorBasalBronchioleToRightInferiorLobeAnteriorBasalAlveolarDeadSpace.MapPath(RightInferiorLobeAnteriorBasalBronchioleToRightInferiorLobeAnteriorBasalAlveolarDeadSpace);
    pRightInferiorLobeLateralBasalBronchioleToRightInferiorLobeLateralBasalAlveolarDeadSpace.MapPath(RightInferiorLobeLateralBasalBronchioleToRightInferiorLobeLateralBasalAlveolarDeadSpace);
    pRightInferiorLobePosteriorBasalBronchioleToRightInferiorLobePosteriorBasalAlveolarDeadSpace.MapPath(RightInferiorLobePosteriorBasalBronchioleToRightInferiorLobePosteriorBasalAlveolarDeadSpace);

    pLeftInferiorLobePosteriorBasalBronchioleToLeftInferiorLobePosteriorBasalAlveolarDeadSpace.MapPath(LeftInferiorLobePosteriorBasalBronchioleToLeftInferiorLobePosteriorBasalAlveolarDeadSpace);
    pLeftInferiorLobeLateralBasalBronchioleToLeftInferiorLobeLateralBasalAlveolarDeadSpace.MapPath(LeftInferiorLobeLateralBasalBronchioleToLeftInferiorLobeLateralBasalAlveolarDeadSpace);
    pLeftInferiorLobeAnteromedialBasalBronchioleToLeftInferiorLobeAnteromedialBasalAlveolarDeadSpace.MapPath(LeftInferiorLobeAnteromedialBasalBronchioleToLeftInferiorLobeAnteromedialBasalAlveolarDeadSpace);
    pLeftInferiorLobeSuperiorBronchioleToLeftInferiorLobeSuperiorAlveolarDeadSpace.MapPath(LeftInferiorLobeSuperiorBronchioleToLeftInferiorLobeSuperiorAlveolarDeadSpace);
    pLeftSuperiorLobeInferiorLingulaBronchioleToLeftSuperiorLobeInferiorLingulaAlveolarDeadSpace.MapPath(LeftSuperiorLobeInferiorLingulaBronchioleToLeftSuperiorLobeInferiorLingulaAlveolarDeadSpace);
    pLeftSuperiorLobeSuperiorLingulaBronchioleToLeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace.MapPath(LeftSuperiorLobeSuperiorLingulaBronchioleToLeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace);
    pLeftSuperiorLobeAnteriorBronchioleToLeftSuperiorLobeAnteriorAlveolarDeadSpace.MapPath(LeftSuperiorLobeAnteriorBronchioleToLeftSuperiorLobeAnteriorAlveolarDeadSpace);
    pLeftSuperiorLobeApicoposteriorBronchioleToLeftSuperiorLobeApicoposteriorAlveolarDeadSpace.MapPath(LeftSuperiorLobeApicoposteriorBronchioleToLeftSuperiorLobeApicoposteriorAlveolarDeadSpace);

    pRightSuperiorLobeApicalAlveolarDeadSpaceToRightSuperiorLobeApicalAlveoli.MapPath(RightSuperiorLobeApicalAlveolarDeadSpaceToRightSuperiorLobeApicalAlveoli);
    pRightSuperiorLobePosteriorAlveolarDeadSpaceToRightSuperiorLobePosteriorAlveoli.MapPath(RightSuperiorLobePosteriorAlveolarDeadSpaceToRightSuperiorLobePosteriorAlveoli);
    pRightSuperiorLobeAnteriorAlveolarDeadSpaceToRightSuperiorLobeAnteriorAlveoli.MapPath(RightSuperiorLobeAnteriorAlveolarDeadSpaceToRightSuperiorLobeAnteriorAlveoli);
    pRightMiddleLobeLateralAlveolarDeadSpaceToRightMiddleLobeLateralAlveoli.MapPath(RightMiddleLobeLateralAlveolarDeadSpaceToRightMiddleLobeLateralAlveoli);
    pRightMiddleLobeMedialAlveolarDeadSpaceToRightMiddleLobeMedialAlveoli.MapPath(RightMiddleLobeMedialAlveolarDeadSpaceToRightMiddleLobeMedialAlveoli);
    pRightInferiorLobeSuperiorAlveolarDeadSpaceToRightInferiorLobeSuperiorAlveoli.MapPath(RightInferiorLobeSuperiorAlveolarDeadSpaceToRightInferiorLobeSuperiorAlveoli);
    pRightInferiorLobeMedialBasalAlveolarDeadSpaceToRightInferiorLobeMedialBasalAlveoli.MapPath(RightInferiorLobeMedialBasalAlveolarDeadSpaceToRightInferiorLobeMedialBasalAlveoli);
    pRightInferiorLobeAnteriorBasalAlveolarDeadSpaceToRightInferiorLobeAnteriorBasalAlveoli.MapPath(RightInferiorLobeAnteriorBasalAlveolarDeadSpaceToRightInferiorLobeAnteriorBasalAlveoli);
    pRightInferiorLobeLateralBasalAlveolarDeadSpaceToRightInferiorLobeLateralBasalAlveoli.MapPath(RightInferiorLobeLateralBasalAlveolarDeadSpaceToRightInferiorLobeLateralBasalAlveoli);
    pRightInferiorLobePosteriorBasalAlveolarDeadSpaceToRightInferiorLobePosteriorBasalAlveoli.MapPath(RightInferiorLobePosteriorBasalAlveolarDeadSpaceToRightInferiorLobePosteriorBasalAlveoli);

    pLeftInferiorLobePosteriorBasalAlveolarDeadSpaceToLeftInferiorLobePosteriorBasalAlveoli.MapPath(LeftInferiorLobePosteriorBasalAlveolarDeadSpaceToLeftInferiorLobePosteriorBasalAlveoli);
    pLeftInferiorLobeLateralBasalAlveolarDeadSpaceToLeftInferiorLobeLateralBasalAlveoli.MapPath(LeftInferiorLobeLateralBasalAlveolarDeadSpaceToLeftInferiorLobeLateralBasalAlveoli);
    pLeftInferiorLobeAnteromedialBasalAlveolarDeadSpaceToLeftInferiorLobeAnteromedialBasalAlveoli.MapPath(LeftInferiorLobeAnteromedialBasalAlveolarDeadSpaceToLeftInferiorLobeAnteromedialBasalAlveoli);
    pLeftInferiorLobeSuperiorAlveolarDeadSpaceToLeftInferiorLobeSuperiorAlveoli.MapPath(LeftInferiorLobeSuperiorAlveolarDeadSpaceToLeftInferiorLobeSuperiorAlveoli);
    pLeftSuperiorLobeInferiorLingulaAlveolarDeadSpaceToLeftSuperiorLobeInferiorLingulaAlveoli.MapPath(LeftSuperiorLobeInferiorLingulaAlveolarDeadSpaceToLeftSuperiorLobeInferiorLingulaAlveoli);
    pLeftSuperiorLobeSuperiorLingulaAlveolarDeadSpaceToLeftSuperiorLobeSuperiorLingulaAlveoli.MapPath(LeftSuperiorLobeSuperiorLingulaAlveolarDeadSpaceToLeftSuperiorLobeSuperiorLingulaAlveoli);
    pLeftSuperiorLobeAnteriorAlveolarDeadSpaceToLeftSuperiorLobeAnteriorAlveoli.MapPath(LeftSuperiorLobeAnteriorAlveolarDeadSpaceToLeftSuperiorLobeAnteriorAlveoli);
    pLeftSuperiorLobeApicoposteriorAlveolarDeadSpaceToLeftSuperiorLobeApicoposteriorAlveoli.MapPath(LeftSuperiorLobeApicoposteriorAlveolarDeadSpaceToLeftSuperiorLobeApicoposteriorAlveoli);

    // Create the respiratory graph for transport //
    SEGasCompartmentGraph& gRespiratory = m_Compartments->GetRespiratoryGraph();

    gRespiratory.AddCompartment(*gEnvironment);

    gRespiratory.AddCompartment(pAirway);
    gRespiratory.AddCompartment(pPharynx);
    gRespiratory.AddCompartment(pCarina);

    gRespiratory.AddCompartment(pStomach);

    gRespiratory.AddCompartment(pRightMainBronchus);
    gRespiratory.AddCompartment(pLeftMainBronchus);
    gRespiratory.AddCompartment(pRightIntermediateBronchus);
    gRespiratory.AddCompartment(pRightSuperiorLobarBronchus);
    gRespiratory.AddCompartment(pRightMiddleLobarBronchus);
    gRespiratory.AddCompartment(pRightInferiorLobarBronchus1);
    gRespiratory.AddCompartment(pLeftInferiorLobarBronchus1);
    gRespiratory.AddCompartment(pLeftSuperiorLobarBronchus);
    gRespiratory.AddCompartment(pRightInferiorLobarBronchus2);
    gRespiratory.AddCompartment(pLeftInferiorLobarBronchus2);
    gRespiratory.AddCompartment(pRightSuperiorApicoposteriorBronchus);
    gRespiratory.AddCompartment(pRightInferiorLobarBronchus3);
    gRespiratory.AddCompartment(pLeftInferiorLobarBronchus3);
    gRespiratory.AddCompartment(pLeftLingularBonchus);
    gRespiratory.AddCompartment(pLeftSuperiorApicoposteriorBronchus);
    gRespiratory.AddCompartment(pRightInferiorLobarBronchus4);

    gRespiratory.AddCompartment(pRightSuperiorLobeApicalBronchiole);
    gRespiratory.AddCompartment(pRightSuperiorLobePosteriorBronchiole);
    gRespiratory.AddCompartment(pRightSuperiorLobeAnteriorBronchiole);
    gRespiratory.AddCompartment(pRightMiddleLobeLateralBronchiole);
    gRespiratory.AddCompartment(pRightMiddleLobeMedialBronchiole);
    gRespiratory.AddCompartment(pRightInferiorLobeSuperiorBronchiole);
    gRespiratory.AddCompartment(pRightInferiorLobeMedialBasalBronchiole);
    gRespiratory.AddCompartment(pRightInferiorLobeAnteriorBasalBronchiole);
    gRespiratory.AddCompartment(pRightInferiorLobeLateralBasalBronchiole);
    gRespiratory.AddCompartment(pRightInferiorLobePosteriorBasalBronchiole);

    gRespiratory.AddCompartment(pLeftInferiorLobePosteriorBasalBronchiole);
    gRespiratory.AddCompartment(pLeftInferiorLobeLateralBasalBronchiole);
    gRespiratory.AddCompartment(pLeftInferiorLobeAnteromedialBasalBronchiole);
    gRespiratory.AddCompartment(pLeftInferiorLobeSuperiorBronchiole);
    gRespiratory.AddCompartment(pLeftSuperiorLobeInferiorLingulaBronchiole);
    gRespiratory.AddCompartment(pLeftSuperiorLobeSuperiorLingulaBronchiole);
    gRespiratory.AddCompartment(pLeftSuperiorLobeAnteriorBronchiole);
    gRespiratory.AddCompartment(pLeftSuperiorLobeApicoposteriorBronchiole);

    gRespiratory.AddCompartment(pRightSuperiorLobeApicalAlveolarDeadSpace);
    gRespiratory.AddCompartment(pRightSuperiorLobePosteriorAlveolarDeadSpace);
    gRespiratory.AddCompartment(pRightSuperiorLobeAnteriorAlveolarDeadSpace);
    gRespiratory.AddCompartment(pRightMiddleLobeLateralAlveolarDeadSpace);
    gRespiratory.AddCompartment(pRightMiddleLobeMedialAlveolarDeadSpace);
    gRespiratory.AddCompartment(pRightInferiorLobeSuperiorAlveolarDeadSpace);
    gRespiratory.AddCompartment(pRightInferiorLobeMedialBasalAlveolarDeadSpace);
    gRespiratory.AddCompartment(pRightInferiorLobeAnteriorBasalAlveolarDeadSpace);
    gRespiratory.AddCompartment(pRightInferiorLobeLateralBasalAlveolarDeadSpace);
    gRespiratory.AddCompartment(pRightInferiorLobePosteriorBasalAlveolarDeadSpace);

    gRespiratory.AddCompartment(pLeftInferiorLobePosteriorBasalAlveolarDeadSpace);
    gRespiratory.AddCompartment(pLeftInferiorLobeLateralBasalAlveolarDeadSpace);
    gRespiratory.AddCompartment(pLeftInferiorLobeAnteromedialBasalAlveolarDeadSpace);
    gRespiratory.AddCompartment(pLeftInferiorLobeSuperiorAlveolarDeadSpace);
    gRespiratory.AddCompartment(pLeftSuperiorLobeInferiorLingulaAlveolarDeadSpace);
    gRespiratory.AddCompartment(pLeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace);
    gRespiratory.AddCompartment(pLeftSuperiorLobeAnteriorAlveolarDeadSpace);
    gRespiratory.AddCompartment(pLeftSuperiorLobeApicoposteriorAlveolarDeadSpace);

    gRespiratory.AddCompartment(pRightSuperiorLobeApicalAlveoli);
    gRespiratory.AddCompartment(pRightSuperiorLobePosteriorAlveoli);
    gRespiratory.AddCompartment(pRightSuperiorLobeAnteriorAlveoli);
    gRespiratory.AddCompartment(pRightMiddleLobeLateralAlveoli);
    gRespiratory.AddCompartment(pRightMiddleLobeMedialAlveoli);
    gRespiratory.AddCompartment(pRightInferiorLobeSuperiorAlveoli);
    gRespiratory.AddCompartment(pRightInferiorLobeMedialBasalAlveoli);
    gRespiratory.AddCompartment(pRightInferiorLobeAnteriorBasalAlveoli);
    gRespiratory.AddCompartment(pRightInferiorLobeLateralBasalAlveoli);
    gRespiratory.AddCompartment(pRightInferiorLobePosteriorBasalAlveoli);

    gRespiratory.AddCompartment(pLeftInferiorLobePosteriorBasalAlveoli);
    gRespiratory.AddCompartment(pLeftInferiorLobeLateralBasalAlveoli);
    gRespiratory.AddCompartment(pLeftInferiorLobeAnteromedialBasalAlveoli);
    gRespiratory.AddCompartment(pLeftInferiorLobeSuperiorAlveoli);
    gRespiratory.AddCompartment(pLeftSuperiorLobeInferiorLingulaAlveoli);
    gRespiratory.AddCompartment(pLeftSuperiorLobeSuperiorLingulaAlveoli);
    gRespiratory.AddCompartment(pLeftSuperiorLobeAnteriorAlveoli);
    gRespiratory.AddCompartment(pLeftSuperiorLobeApicoposteriorAlveoli);

    gRespiratory.AddLink(pEnvironmentToAirway);
    gRespiratory.AddLink(pAirwayToPharynx);
    gRespiratory.AddLink(pAirwayToStomach);
    gRespiratory.AddLink(pPharynxToCarina);
    gRespiratory.AddLink(pPharynxToEnvironment);

    gRespiratory.AddLink(pCarinaToRightMainBronchus);
    gRespiratory.AddLink(pRightMainBronchusToRightIntermediateBronchus);
    gRespiratory.AddLink(pRightMainBronchusToRightSuperiorLobarBronchus);
    gRespiratory.AddLink(pRightIntermediateBronchusToRightMiddleLobarBronchus);
    gRespiratory.AddLink(pRightIntermediateBronchusToRightInferiorLobarBronchus1);
    gRespiratory.AddLink(pRightInferiorLobarBronchus1ToRightInferiorLobarBronchus2);
    gRespiratory.AddLink(pRightSuperiorLobarBronchusToRightSuperiorApicoposteriorBronchus);
    gRespiratory.AddLink(pRightInferiorLobarBronchus2ToRightInferiorLobarBronchus3);
    gRespiratory.AddLink(pRightInferiorLobarBronchus2ToRightInferiorLobarBronchus4);

    gRespiratory.AddLink(pCarinaToLeftMainBronchus);
    gRespiratory.AddLink(pLeftMainBronchusToLeftInferiorLobarBronchus1);
    gRespiratory.AddLink(pLeftMainBronchusToLeftSuperiorLobarBronchus);
    gRespiratory.AddLink(pLeftInferiorLobarBronchus1ToLeftInferiorLobarBronchus2);
    gRespiratory.AddLink(pLeftInferiorLobarBronchus2ToLeftInferiorLobarBronchus3);
    gRespiratory.AddLink(pLeftSuperiorLobarBronchusToLeftLingularBonchus);
    gRespiratory.AddLink(pLeftSuperiorLobarBronchusToLeftSuperiorApicoposteriorBronchus);

    gRespiratory.AddLink(pRightSuperiorApicoposteriorBronchusToRightSuperiorLobeApicalBronchiole);
    gRespiratory.AddLink(pRightSuperiorApicoposteriorBronchusToRightSuperiorLobePosteriorBronchiole);
    gRespiratory.AddLink(pRightSuperiorLobarBronchusToRightSuperiorLobeAnteriorBronchiole);
    gRespiratory.AddLink(pRightMiddleLobarBronchusToRightMiddleLobeLateralBronchiole);
    gRespiratory.AddLink(pRightMiddleLobarBronchusToRightMiddleLobeMedialBronchiole);
    gRespiratory.AddLink(pRightInferiorLobarBronchus1ToRightInferiorLobeSuperiorBronchiole);
    gRespiratory.AddLink(pRightInferiorLobarBronchus2ToRightInferiorLobeMedialBasalBronchiole);
    gRespiratory.AddLink(pRightInferiorLobarBronchus3ToRightInferiorLobeAnteriorBasalBronchiole);
    gRespiratory.AddLink(pRightInferiorLobarBronchus4ToRightInferiorLobeLateralBasalBronchiole);
    gRespiratory.AddLink(pRightInferiorLobarBronchus4ToRightInferiorLobePosteriorBasalBronchiole);

    gRespiratory.AddLink(pLeftInferiorLobarBronchus3ToLeftInferiorLobeLateralBasalBronchiole);
    gRespiratory.AddLink(pLeftInferiorLobarBronchus3ToLeftInferiorLobePosteriorBasalBronchiole);
    gRespiratory.AddLink(pLeftInferiorLobarBronchus2ToLeftInferiorLobeAnteromedialBasalBronchiole);
    gRespiratory.AddLink(pLeftInferiorLobarBronchus1ToLeftInferiorLobeSuperiorBronchiole);
    gRespiratory.AddLink(pLeftLingularBonchusToLeftSuperiorLobeInferiorLingulaBronchiole);
    gRespiratory.AddLink(pLeftLingularBonchusToLeftSuperiorLobeSuperiorLingulaBronchiole);
    gRespiratory.AddLink(pLeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeAnteriorBronchiole);
    gRespiratory.AddLink(pLeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeApicoposteriorBronchiole);

    gRespiratory.AddLink(pRightSuperiorLobeApicalBronchioleToRightSuperiorLobeApicalAlveolarDeadSpace);
    gRespiratory.AddLink(pRightSuperiorLobePosteriorBronchioleToRightSuperiorLobePosteriorAlveolarDeadSpace);
    gRespiratory.AddLink(pRightSuperiorLobeAnteriorBronchioleToRightSuperiorLobeAnteriorAlveolarDeadSpace);
    gRespiratory.AddLink(pRightMiddleLobeLateralBronchioleToRightMiddleLobeLateralAlveolarDeadSpace);
    gRespiratory.AddLink(pRightMiddleLobeMedialBronchioleToRightMiddleLobeMedialAlveolarDeadSpace);
    gRespiratory.AddLink(pRightInferiorLobeSuperiorBronchioleToRightInferiorLobeSuperiorAlveolarDeadSpace);
    gRespiratory.AddLink(pRightInferiorLobeMedialBasalBronchioleToRightInferiorLobeMedialBasalAlveolarDeadSpace);
    gRespiratory.AddLink(pRightInferiorLobeAnteriorBasalBronchioleToRightInferiorLobeAnteriorBasalAlveolarDeadSpace);
    gRespiratory.AddLink(pRightInferiorLobeLateralBasalBronchioleToRightInferiorLobeLateralBasalAlveolarDeadSpace);
    gRespiratory.AddLink(pRightInferiorLobePosteriorBasalBronchioleToRightInferiorLobePosteriorBasalAlveolarDeadSpace);

    gRespiratory.AddLink(pLeftInferiorLobePosteriorBasalBronchioleToLeftInferiorLobePosteriorBasalAlveolarDeadSpace);
    gRespiratory.AddLink(pLeftInferiorLobeLateralBasalBronchioleToLeftInferiorLobeLateralBasalAlveolarDeadSpace);
    gRespiratory.AddLink(pLeftInferiorLobeAnteromedialBasalBronchioleToLeftInferiorLobeAnteromedialBasalAlveolarDeadSpace);
    gRespiratory.AddLink(pLeftInferiorLobeSuperiorBronchioleToLeftInferiorLobeSuperiorAlveolarDeadSpace);
    gRespiratory.AddLink(pLeftSuperiorLobeInferiorLingulaBronchioleToLeftSuperiorLobeInferiorLingulaAlveolarDeadSpace);
    gRespiratory.AddLink(pLeftSuperiorLobeSuperiorLingulaBronchioleToLeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace);
    gRespiratory.AddLink(pLeftSuperiorLobeAnteriorBronchioleToLeftSuperiorLobeAnteriorAlveolarDeadSpace);
    gRespiratory.AddLink(pLeftSuperiorLobeApicoposteriorBronchioleToLeftSuperiorLobeApicoposteriorAlveolarDeadSpace);

    gRespiratory.AddLink(pRightSuperiorLobeApicalAlveolarDeadSpaceToRightSuperiorLobeApicalAlveoli);
    gRespiratory.AddLink(pRightSuperiorLobePosteriorAlveolarDeadSpaceToRightSuperiorLobePosteriorAlveoli);
    gRespiratory.AddLink(pRightSuperiorLobeAnteriorAlveolarDeadSpaceToRightSuperiorLobeAnteriorAlveoli);
    gRespiratory.AddLink(pRightMiddleLobeLateralAlveolarDeadSpaceToRightMiddleLobeLateralAlveoli);
    gRespiratory.AddLink(pRightMiddleLobeMedialAlveolarDeadSpaceToRightMiddleLobeMedialAlveoli);
    gRespiratory.AddLink(pRightInferiorLobeSuperiorAlveolarDeadSpaceToRightInferiorLobeSuperiorAlveoli);
    gRespiratory.AddLink(pRightInferiorLobeMedialBasalAlveolarDeadSpaceToRightInferiorLobeMedialBasalAlveoli);
    gRespiratory.AddLink(pRightInferiorLobeAnteriorBasalAlveolarDeadSpaceToRightInferiorLobeAnteriorBasalAlveoli);
    gRespiratory.AddLink(pRightInferiorLobeLateralBasalAlveolarDeadSpaceToRightInferiorLobeLateralBasalAlveoli);
    gRespiratory.AddLink(pRightInferiorLobePosteriorBasalAlveolarDeadSpaceToRightInferiorLobePosteriorBasalAlveoli);

    gRespiratory.AddLink(pLeftInferiorLobePosteriorBasalAlveolarDeadSpaceToLeftInferiorLobePosteriorBasalAlveoli);
    gRespiratory.AddLink(pLeftInferiorLobeLateralBasalAlveolarDeadSpaceToLeftInferiorLobeLateralBasalAlveoli);
    gRespiratory.AddLink(pLeftInferiorLobeAnteromedialBasalAlveolarDeadSpaceToLeftInferiorLobeAnteromedialBasalAlveoli);
    gRespiratory.AddLink(pLeftInferiorLobeSuperiorAlveolarDeadSpaceToLeftInferiorLobeSuperiorAlveoli);
    gRespiratory.AddLink(pLeftSuperiorLobeInferiorLingulaAlveolarDeadSpaceToLeftSuperiorLobeInferiorLingulaAlveoli);
    gRespiratory.AddLink(pLeftSuperiorLobeSuperiorLingulaAlveolarDeadSpaceToLeftSuperiorLobeSuperiorLingulaAlveoli);
    gRespiratory.AddLink(pLeftSuperiorLobeAnteriorAlveolarDeadSpaceToLeftSuperiorLobeAnteriorAlveoli);
    gRespiratory.AddLink(pLeftSuperiorLobeApicoposteriorAlveolarDeadSpaceToLeftSuperiorLobeApicoposteriorAlveoli);

    gRespiratory.StateChange();

    // Generically set up the Aerosol Graph, this is a mirror of the Respiratory Gas Graph, only it's a liquid graph
    SELiquidCompartmentGraph& lAerosol = m_Compartments->GetAerosolGraph();
    SELiquidCompartment* lEnvironment = m_Compartments->GetLiquidCompartment(pulse::EnvironmentCompartment::Ambient);
    lAerosol.AddCompartment(*lEnvironment);
    // First Create the compartments and map the same nodes
    for (auto gasCmpt : gRespiratory.GetCompartments())
    {
      std::string name = gasCmpt->GetName();
      SELiquidCompartment& liquidCmpt = m_Compartments->CreateLiquidCompartment(name);
      if (gasCmpt->HasNodeMapping())
      {
        for (auto node : gasCmpt->GetNodeMapping().GetNodes())
          liquidCmpt.MapNode(*node);
      }
    }
    // Hook up any hierarchies
    for (auto gasCmpt : gRespiratory.GetCompartments())
    {
      std::string name = gasCmpt->GetName();
      SELiquidCompartment* liquidCmpt = m_Compartments->GetLiquidCompartment(name);
      if (gasCmpt->HasChildren())
      {
        for (auto child : gasCmpt->GetChildren())
          liquidCmpt->AddChild(*m_Compartments->GetLiquidCompartment(child->GetName()));
      }
    }
    // Add leaf compartments to the graph
    for (auto gasCmpt : gRespiratory.GetCompartments())
    {
      std::string name = gasCmpt->GetName();
      SELiquidCompartment* liquidCmpt = m_Compartments->GetLiquidCompartment(name);
      if (!liquidCmpt->HasChildren())
        lAerosol.AddCompartment(*liquidCmpt);
    }
    // Create Links
    for (auto gasLink : gRespiratory.GetLinks())
    {
      std::string name = gasLink->GetName();
      SELiquidCompartment* src = m_Compartments->GetLiquidCompartment(gasLink->GetSourceCompartment().GetName());
      SELiquidCompartment* tgt = m_Compartments->GetLiquidCompartment(gasLink->GetTargetCompartment().GetName());
      SELiquidCompartmentLink& liquidLink = m_Compartments->CreateLiquidLink(*src, *tgt, name);
      if (gasLink->HasPath())
        liquidLink.MapPath(*gasLink->GetPath());
      lAerosol.AddLink(liquidLink);
    }
    lAerosol.StateChange();
  }

  void Controller::SetupExpandedLungsCardiovascular()
  {
    Info("Setting Up Expanded Lungs Cardiovascular");
    bool male = m_InitialPatient->GetSex() == ePatient_Sex::Male ? true : false;
    double RightLungRatio = m_InitialPatient->GetRightLungRatio().GetValue();
    double LeftLungRatio = 1 - RightLungRatio;
    double bloodVolume_mL = m_InitialPatient->GetBloodVolumeBaseline(VolumeUnit::mL);

    double systolicPressureTarget_mmHg = m_InitialPatient->GetSystolicArterialPressureBaseline(PressureUnit::mmHg);

    const double cardiacOutputTarget_mL_Per_min = male ? 5600.0 : 4900.0;
    const double cardiacOutputTarget_mL_Per_s = cardiacOutputTarget_mL_Per_min / 60;

    const double pulmonaryShuntFractionFactor = 0.009; // Used to set the pulmonary shunt fraction. Actual shunt will be roughly double this value (two lungs).
    // The way this works is we compute resistances and compliances based on the hemodynamic variables above that are either in the patient
    // file or we use the defaults if nothing is there. Because the actual impedance depends on the frequency, the computations assume a resting heart rate.
    // So if a user needs to put pressures in the patient file assuming that the pts baseline hr is in the normal range (around 72).
    // If someone wants a patient with a high hr because s/he is exercising or something, then they need to use the action.
    // If a user wants a patient with a ridiculously high resting hr, then they will need to estimate what the pressures and CO would be if the pt had a normal resting hr.

    // Our configuration can apply a modifier to adjust any of these targets to get closer to the target systolic and diastolic pressures from the patient file
    // The tuning method in cardiovascular will do the fine tuning, but modifiers can speed up the process.
    /// \todo Make these a function of the systolic and diastolic pressure by fitting a curve to the data from the variations test
    const double systemicResistanceModifier = 1.0; // 0.849;
    const double largeArteriesComplianceModifier = 1.0;//0.4333;
    const double expandedPulmonaryCapillariesComplianceModifier = 1.0;
    const double expandedPulmonaryLeftCapillariesVolumeModifier = 0.83;
    const double expandedPulmonaryRightCapillariesVolumeModifier = 0.66;
    //const double expandedPulmonaryLeftCapillariesVolumeModifier = 1.0;
    //const double expandedPulmonaryRightCapillariesVolumeModifier = 1.0;

    //Flow rates from \cite valentin2002icrp
    double VascularFlowTargetAorta = 1.0 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetArm = 0.015 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetBone = 0.05 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetBrain = 0.12 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetFat = male ? 0.05 * cardiacOutputTarget_mL_Per_s : 0.085 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetKidney = male ? 0.085 * cardiacOutputTarget_mL_Per_s : 0.075 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetLargeIntestine = male ? 0.04 * cardiacOutputTarget_mL_Per_s : 0.05 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetLeg = 0.0525 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetMuscle = male ? 0.17 * cardiacOutputTarget_mL_Per_s : 0.12 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetMyocardium = male ? 0.04 * cardiacOutputTarget_mL_Per_s : 0.05 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetPulmCapRight = RightLungRatio * cardiacOutputTarget_mL_Per_s * (1 - pulmonaryShuntFractionFactor);
    double VascularFlowTargetPulmVeinsRight = RightLungRatio * cardiacOutputTarget_mL_Per_s * (1 - pulmonaryShuntFractionFactor);
    double VascularFlowTargetPulmArtLeft = LeftLungRatio * cardiacOutputTarget_mL_Per_s * (1 - pulmonaryShuntFractionFactor);
    double VascularFlowTargetPulmCapLeft = LeftLungRatio * cardiacOutputTarget_mL_Per_s * (1 - pulmonaryShuntFractionFactor);
    double VascularFlowTargetPulmVeinsLeft = LeftLungRatio * cardiacOutputTarget_mL_Per_s * (1 - pulmonaryShuntFractionFactor);
    double VascularFlowTargetSkin = 0.05 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetSmallIntestine = male ? 0.1 * cardiacOutputTarget_mL_Per_s : 0.11 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetSplanchnic = 0.01 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetSpleen = 0.03 * cardiacOutputTarget_mL_Per_s;
    double VascularFlowTargetPortalVein = VascularFlowTargetLargeIntestine + VascularFlowTargetSmallIntestine + VascularFlowTargetSplanchnic + VascularFlowTargetSpleen;
    double VascularFlowTargetLiver = (male ? 0.255 * cardiacOutputTarget_mL_Per_s : 0.27 * cardiacOutputTarget_mL_Per_s) - VascularFlowTargetPortalVein;

    double VascularFlowTargetPulmCapTotal = VascularFlowTargetPulmCapRight + VascularFlowTargetPulmCapLeft;

    double VascularFlowTargetRightSuperiorLobeApicalPulmonaryCapillaries = 2.04 / 100.0 * VascularFlowTargetPulmCapTotal;
    double VascularFlowTargetRightSuperiorLobePosteriorPulmonaryCapillaries = 2.95 / 100.0 * VascularFlowTargetPulmCapTotal;
    double VascularFlowTargetRightSuperiorLobeAnteriorPulmonaryCapillaries = 4.23 / 100.0 * VascularFlowTargetPulmCapTotal;
    double VascularFlowTargetRightMiddleLobeLateralPulmonaryCapillaries = 6.34 / 100.0 * VascularFlowTargetPulmCapTotal;
    double VascularFlowTargetRightMiddleLobeMedialPulmonaryCapillaries = 6.19 / 100.0 * VascularFlowTargetPulmCapTotal;
    double VascularFlowTargetRightInferiorLobeSuperiorPulmonaryCapillaries = 4.91 / 100.0 * VascularFlowTargetPulmCapTotal;
    double VascularFlowTargetRightInferiorLobeMedialBasalPulmonaryCapillaries = 6.64 / 100.0 * VascularFlowTargetPulmCapTotal;
    double VascularFlowTargetRightInferiorLobeAnteriorBasalPulmonaryCapillaries = 7.32 / 100.0 * VascularFlowTargetPulmCapTotal;
    double VascularFlowTargetRightInferiorLobeLateralBasalPulmonaryCapillaries = 7.62 / 100.0 * VascularFlowTargetPulmCapTotal;
    double VascularFlowTargetRightInferiorLobePosteriorBasalPulmonaryCapillaries = 6.94 / 100.0 * VascularFlowTargetPulmCapTotal;

    double VascularFlowTargetLeftInferiorLobePosteriorBasalPulmonaryCapillaries = 7.02 / 100.0 * VascularFlowTargetPulmCapTotal;
    double VascularFlowTargetLeftInferiorLobeLateralBasalPulmonaryCapillaries = 7.47 / 100.0 * VascularFlowTargetPulmCapTotal;
    double VascularFlowTargetLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries = 6.79 / 100.0 * VascularFlowTargetPulmCapTotal;
    double VascularFlowTargetLeftInferiorLobeSuperiorPulmonaryCapillaries = 4.91 / 100.0 * VascularFlowTargetPulmCapTotal;
    double VascularFlowTargetLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries = 6.49 / 100.0 * VascularFlowTargetPulmCapTotal;
    double VascularFlowTargetLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries = 5.58 / 100.0 * VascularFlowTargetPulmCapTotal;
    double VascularFlowTargetLeftSuperiorLobeAnteriorPulmonaryCapillaries = 4.08 / 100.0 * VascularFlowTargetPulmCapTotal;
    double VascularFlowTargetLeftSuperiorLobeApicoposteriorPulmonaryCapillaries = 2.50 / 100.0 * VascularFlowTargetPulmCapTotal;

    // Pressure targets derived from information available in \cite guyton2006medical and \cite van2013davis
    double VascularPressureTargetAorta = 1.0 * systolicPressureTarget_mmHg;
    double VascularPressureTargetArm = 0.33 * systolicPressureTarget_mmHg;
    double VascularPressureTargetBone = 0.33 * systolicPressureTarget_mmHg;
    double VascularPressureTargetBrain = 0.08 * systolicPressureTarget_mmHg;
    double VascularPressureTargetFat = 0.33 * systolicPressureTarget_mmHg;
    double VascularPressureTargetHeartLeft = 1.06667 * systolicPressureTarget_mmHg;
    double VascularPressureTargetHeartRight = 0.16667 * systolicPressureTarget_mmHg;
    double VascularPressureTargetKidney = 0.33 * systolicPressureTarget_mmHg;
    double VascularPressureTargetLargeIntestine = 0.33 * systolicPressureTarget_mmHg;
    double VascularPressureTargetLeg = 0.33 * systolicPressureTarget_mmHg;
    double VascularPressureTargetLiver = 0.25 * systolicPressureTarget_mmHg;
    double VascularPressureTargetMuscle = 0.33 * systolicPressureTarget_mmHg;
    double VascularPressureTargetMyocardium = 0.33 * systolicPressureTarget_mmHg;
    double VascularPressureTargetPulmArtRight = 0.13333 * systolicPressureTarget_mmHg;
    double VascularPressureTargetPulmCapRight = 0.0650 * systolicPressureTarget_mmHg;
    double VascularPressureTargetPulmVeinsRight = 0.03846 * systolicPressureTarget_mmHg;
    double VascularPressureTargetPulmArtLeft = 0.13333 * systolicPressureTarget_mmHg;
    double VascularPressureTargetPulmCapLeft = 0.0650 * systolicPressureTarget_mmHg;
    double VascularPressureTargetPulmVeinsLeft = 0.03846 * systolicPressureTarget_mmHg;
    double VascularPressureTargetSkin = 0.0833 * systolicPressureTarget_mmHg;
    double VascularPressureTargetSmallIntestine = 0.33 * systolicPressureTarget_mmHg;
    double VascularPressureTargetSplanchnic = 0.33 * systolicPressureTarget_mmHg;
    double VascularPressureTargetSpleen = 0.33 * systolicPressureTarget_mmHg;
    double VascularPressureTargetVenaCava = 0.0333 * systolicPressureTarget_mmHg;

    // Volume fractions from \cite valentin2002icrp
    double VolumeFractionAorta = 0.05;
    double VolumeFractionArm = 0.02;
    double VolumeFractionBone = 0.07;
    double VolumeFractionBrain = 0.012;
    double VolumeFractionFat = male ? 0.05 : 0.085;
    double VolumeFractionHeartLeft = 0.0025;
    double VolumeFractionHeartRight = 0.0025;
    double VolumeFractionKidney = 0.02;
    double VolumeFractionLargeIntestine = 0.022;
    double VolumeFractionLeg = 0.04;
    double VolumeFractionLiver = 0.1;
    double VolumeFractionMuscle = male ? 0.14 : 0.105;
    double VolumeFractionMyocardium = 0.01;
    double VolumeFractionPulmArtRight = 0.03 * RightLungRatio;
    double VolumeFractionPulmCapRight = 0.02 * RightLungRatio;
    double VolumeFractionPulmVeinsRight = 0.055 * RightLungRatio;
    double VolumeFractionPulmArtLeft = 0.03 * LeftLungRatio;
    double VolumeFractionPulmCapLeft = 0.02 * LeftLungRatio;
    double VolumeFractionPulmVeinsLeft = 0.055 * LeftLungRatio;
    double VolumeFractionSkin = 0.03;
    double VolumeFractionSmallIntestine = 0.038;
    double VolumeFractionSplanchnic = 0.01;
    double VolumeFractionSpleen = 0.014;
    double VolumeFractionVenaCava = 0.175;

    double VolumeFractionPulmCapTotal = VolumeFractionPulmCapRight + VolumeFractionPulmCapLeft;

    double VolumeFractionRightSuperiorLobeApicalPulmonaryCapillaries = 9.3 / 100.0 * VolumeFractionPulmCapTotal;
    double VolumeFractionRightSuperiorLobePosteriorPulmonaryCapillaries = 3.5 / 100.0 * VolumeFractionPulmCapTotal;
    double VolumeFractionRightSuperiorLobeAnteriorPulmonaryCapillaries = 7.5 / 100.0 * VolumeFractionPulmCapTotal;
    double VolumeFractionRightMiddleLobeLateralPulmonaryCapillaries = 2.9 / 100.0 * VolumeFractionPulmCapTotal;
    double VolumeFractionRightMiddleLobeMedialPulmonaryCapillaries = 6.3 / 100.0 * VolumeFractionPulmCapTotal;
    double VolumeFractionRightInferiorLobeSuperiorPulmonaryCapillaries = 6.2 / 100.0 * VolumeFractionPulmCapTotal;
    double VolumeFractionRightInferiorLobeMedialBasalPulmonaryCapillaries = 1.7 / 100.0 * VolumeFractionPulmCapTotal;
    double VolumeFractionRightInferiorLobeAnteriorBasalPulmonaryCapillaries = 5.5 / 100.0 * VolumeFractionPulmCapTotal;
    double VolumeFractionRightInferiorLobeLateralBasalPulmonaryCapillaries = 4.8 / 100.0 * VolumeFractionPulmCapTotal;
    double VolumeFractionRightInferiorLobePosteriorBasalPulmonaryCapillaries = 6.9 / 100.0 * VolumeFractionPulmCapTotal;

    double VolumeFractionLeftInferiorLobePosteriorBasalPulmonaryCapillaries = 5.6 / 100.0 * VolumeFractionPulmCapTotal;
    double VolumeFractionLeftInferiorLobeLateralBasalPulmonaryCapillaries = 6.4 / 100.0 * VolumeFractionPulmCapTotal;
    double VolumeFractionLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries = 4.2 / 100.0 * VolumeFractionPulmCapTotal;
    double VolumeFractionLeftInferiorLobeSuperiorPulmonaryCapillaries = 5.1 / 100.0 * VolumeFractionPulmCapTotal;
    double VolumeFractionLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries = 4.8 / 100.0 * VolumeFractionPulmCapTotal;
    double VolumeFractionLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries = 4.0 / 100.0 * VolumeFractionPulmCapTotal;
    double VolumeFractionLeftSuperiorLobeAnteriorPulmonaryCapillaries = 7.5 / 100.0 * VolumeFractionPulmCapTotal;
    double VolumeFractionLeftSuperiorLobeApicoposteriorPulmonaryCapillaries = 8.1 / 100.0 * VolumeFractionPulmCapTotal;

    // Compute resistances from mean flow rates and pressure targets
    double ResistanceAorta = (VascularPressureTargetHeartLeft - systolicPressureTarget_mmHg) / VascularFlowTargetAorta;
    double ResistanceArm = (systolicPressureTarget_mmHg - VascularPressureTargetArm) / VascularFlowTargetArm;
    double ResistanceArmVenous = (VascularPressureTargetArm - VascularPressureTargetVenaCava) / VascularFlowTargetArm;
    double ResistanceBone = (systolicPressureTarget_mmHg - VascularPressureTargetBone) / VascularFlowTargetBone;
    double ResistanceBoneVenous = (VascularPressureTargetBone - VascularPressureTargetVenaCava) / VascularFlowTargetBone;
    double ResistanceBrain = (systolicPressureTarget_mmHg - VascularPressureTargetBrain) / VascularFlowTargetBrain;
    double ResistanceBrainVenous = (VascularPressureTargetBrain - VascularPressureTargetVenaCava) / VascularFlowTargetBrain;
    double ResistanceFat = (systolicPressureTarget_mmHg - VascularPressureTargetFat) / VascularFlowTargetFat;
    double ResistanceFatVenous = (VascularPressureTargetFat - VascularPressureTargetVenaCava) / VascularFlowTargetFat;
    double ResistanceHeartLeft = 0.0008;
    double ResistanceHeartRight = (systolicPressureTarget_mmHg - VascularPressureTargetVenaCava) / cardiacOutputTarget_mL_Per_s;
    double ResistanceKidney = (systolicPressureTarget_mmHg - VascularPressureTargetKidney) / VascularFlowTargetKidney;
    double ResistanceKidneyVenous = (VascularPressureTargetKidney - VascularPressureTargetVenaCava) / VascularFlowTargetKidney;
    double ResistanceLargeIntestine = (systolicPressureTarget_mmHg - VascularPressureTargetLargeIntestine) / VascularFlowTargetLargeIntestine;
    double ResistanceLargeIntestineVenous = (VascularPressureTargetLargeIntestine - VascularPressureTargetLiver) / VascularFlowTargetLargeIntestine;
    double ResistanceLeg = (systolicPressureTarget_mmHg - VascularPressureTargetLeg) / VascularFlowTargetLeg;
    double ResistanceLegVenous = (VascularPressureTargetLeg - VascularPressureTargetVenaCava) / VascularFlowTargetLeg;
    double ResistanceLiver = (systolicPressureTarget_mmHg - VascularPressureTargetLiver) / VascularFlowTargetLiver;
    double ResistanceLiverVenous = (VascularPressureTargetLiver - VascularPressureTargetVenaCava) / (VascularFlowTargetLiver + VascularFlowTargetPortalVein);
    double ResistanceMuscle = (systolicPressureTarget_mmHg - VascularPressureTargetMuscle) / VascularFlowTargetMuscle;
    double ResistanceMuscleVenous = (VascularPressureTargetMuscle - VascularPressureTargetVenaCava) / VascularFlowTargetMuscle;
    double ResistanceMyocardium = (systolicPressureTarget_mmHg - VascularPressureTargetMyocardium) / VascularFlowTargetMyocardium;
    double ResistanceMyocardiumVenous = (VascularPressureTargetMyocardium - VascularPressureTargetVenaCava) / VascularFlowTargetMyocardium;
    //double ResistancePulmCapRight = (VascularPressureTargetPulmArtRight - VascularPressureTargetPulmCapRight) / VascularFlowTargetPulmCapRight;
    double ResistancePulmVeinsRight = (VascularPressureTargetPulmCapRight - VascularPressureTargetPulmVeinsRight) / VascularFlowTargetPulmVeinsRight;
    double ResistancePulmArt = (VascularPressureTargetHeartRight - VascularPressureTargetPulmArtLeft) / VascularFlowTargetPulmArtLeft;
    //double ResistancePulmCapLeft = (VascularPressureTargetPulmArtLeft - VascularPressureTargetPulmCapLeft) / VascularFlowTargetPulmCapLeft;
    double ResistancePulmVeinsLeft = (VascularPressureTargetPulmCapLeft - VascularPressureTargetPulmVeinsLeft) / VascularFlowTargetPulmVeinsLeft;
    double ResistanceSkin = (systolicPressureTarget_mmHg - VascularPressureTargetSkin) / VascularFlowTargetSkin;
    double ResistanceSkinVenous = (VascularPressureTargetSkin - VascularPressureTargetVenaCava) / VascularFlowTargetSkin;
    double ResistanceSmallIntestine = (systolicPressureTarget_mmHg - VascularPressureTargetSmallIntestine) / VascularFlowTargetSmallIntestine;
    double ResistanceSmallIntestineVenous = (VascularPressureTargetSmallIntestine - VascularPressureTargetLiver) / VascularFlowTargetSmallIntestine;
    double ResistanceSplanchnic = (systolicPressureTarget_mmHg - VascularPressureTargetSplanchnic) / VascularFlowTargetSplanchnic;
    double ResistanceSplanchnicVenous = (VascularPressureTargetSplanchnic - VascularPressureTargetLiver) / VascularFlowTargetSplanchnic;
    double ResistanceSpleen = (systolicPressureTarget_mmHg - VascularPressureTargetSpleen) / VascularFlowTargetSpleen;
    double ResistanceSpleenVenous = (VascularPressureTargetSpleen - VascularPressureTargetLiver) / VascularFlowTargetSpleen;

    double ResistanceRightSuperiorLobeApicalPulmonaryCapillaries = (VascularPressureTargetPulmArtRight - VascularPressureTargetPulmCapRight) / VascularFlowTargetRightSuperiorLobeApicalPulmonaryCapillaries;
    double ResistanceRightSuperiorLobePosteriorPulmonaryCapillaries = (VascularPressureTargetPulmArtRight - VascularPressureTargetPulmCapRight) / VascularFlowTargetRightSuperiorLobePosteriorPulmonaryCapillaries;
    double ResistanceRightSuperiorLobeAnteriorPulmonaryCapillaries = (VascularPressureTargetPulmArtRight - VascularPressureTargetPulmCapRight) / VascularFlowTargetRightSuperiorLobeAnteriorPulmonaryCapillaries;
    double ResistanceRightMiddleLobeLateralPulmonaryCapillaries = (VascularPressureTargetPulmArtRight - VascularPressureTargetPulmCapRight) / VascularFlowTargetRightMiddleLobeLateralPulmonaryCapillaries;
    double ResistanceRightMiddleLobeMedialPulmonaryCapillaries = (VascularPressureTargetPulmArtRight - VascularPressureTargetPulmCapRight) / VascularFlowTargetRightMiddleLobeMedialPulmonaryCapillaries;
    double ResistanceRightInferiorLobeSuperiorPulmonaryCapillaries = (VascularPressureTargetPulmArtRight - VascularPressureTargetPulmCapRight) / VascularFlowTargetRightInferiorLobeSuperiorPulmonaryCapillaries;
    double ResistanceRightInferiorLobeMedialBasalPulmonaryCapillaries = (VascularPressureTargetPulmArtRight - VascularPressureTargetPulmCapRight) / VascularFlowTargetRightInferiorLobeMedialBasalPulmonaryCapillaries;
    double ResistanceRightInferiorLobeAnteriorBasalPulmonaryCapillaries = (VascularPressureTargetPulmArtRight - VascularPressureTargetPulmCapRight) / VascularFlowTargetRightInferiorLobeAnteriorBasalPulmonaryCapillaries;
    double ResistanceRightInferiorLobeLateralBasalPulmonaryCapillaries = (VascularPressureTargetPulmArtRight - VascularPressureTargetPulmCapRight) / VascularFlowTargetRightInferiorLobeLateralBasalPulmonaryCapillaries;
    double ResistanceRightInferiorLobePosteriorBasalPulmonaryCapillaries = (VascularPressureTargetPulmArtRight - VascularPressureTargetPulmCapRight) / VascularFlowTargetRightInferiorLobePosteriorBasalPulmonaryCapillaries;

    double ResistanceLeftInferiorLobePosteriorBasalPulmonaryCapillaries = (VascularPressureTargetPulmArtLeft - VascularPressureTargetPulmCapLeft) / VascularFlowTargetLeftInferiorLobePosteriorBasalPulmonaryCapillaries;
    double ResistanceLeftInferiorLobeLateralBasalPulmonaryCapillaries = (VascularPressureTargetPulmArtLeft - VascularPressureTargetPulmCapLeft) / VascularFlowTargetLeftInferiorLobeLateralBasalPulmonaryCapillaries;
    double ResistanceLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries = (VascularPressureTargetPulmArtLeft - VascularPressureTargetPulmCapLeft) / VascularFlowTargetLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries;
    double ResistanceLeftInferiorLobeSuperiorPulmonaryCapillaries = (VascularPressureTargetPulmArtLeft - VascularPressureTargetPulmCapLeft) / VascularFlowTargetLeftInferiorLobeSuperiorPulmonaryCapillaries;
    double ResistanceLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries = (VascularPressureTargetPulmArtLeft - VascularPressureTargetPulmCapLeft) / VascularFlowTargetLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries;
    double ResistanceLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries = (VascularPressureTargetPulmArtLeft - VascularPressureTargetPulmCapLeft) / VascularFlowTargetLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries;
    double ResistanceLeftSuperiorLobeAnteriorPulmonaryCapillaries = (VascularPressureTargetPulmArtLeft - VascularPressureTargetPulmCapLeft) / VascularFlowTargetLeftSuperiorLobeAnteriorPulmonaryCapillaries;
    double ResistanceLeftSuperiorLobeApicoposteriorPulmonaryCapillaries = (VascularPressureTargetPulmArtLeft - VascularPressureTargetPulmCapLeft) / VascularFlowTargetLeftSuperiorLobeApicoposteriorPulmonaryCapillaries;

    double ResistanceRightSuperiorLobeApicalPulmonaryVeins = (VascularPressureTargetPulmCapRight - VascularPressureTargetPulmVeinsRight) / VascularFlowTargetRightSuperiorLobeApicalPulmonaryCapillaries;
    double ResistanceRightSuperiorLobePosteriorPulmonaryVeins = (VascularPressureTargetPulmCapRight - VascularPressureTargetPulmVeinsRight) / VascularFlowTargetRightSuperiorLobePosteriorPulmonaryCapillaries;
    double ResistanceRightSuperiorLobeAnteriorPulmonaryVeins = (VascularPressureTargetPulmCapRight - VascularPressureTargetPulmVeinsRight) / VascularFlowTargetRightSuperiorLobeAnteriorPulmonaryCapillaries;
    double ResistanceRightMiddleLobeLateralPulmonaryVeins = (VascularPressureTargetPulmCapRight - VascularPressureTargetPulmVeinsRight) / VascularFlowTargetRightMiddleLobeLateralPulmonaryCapillaries;
    double ResistanceRightMiddleLobeMedialPulmonaryVeins = (VascularPressureTargetPulmCapRight - VascularPressureTargetPulmVeinsRight) / VascularFlowTargetRightMiddleLobeMedialPulmonaryCapillaries;
    double ResistanceRightInferiorLobeSuperiorPulmonaryVeins = (VascularPressureTargetPulmCapRight - VascularPressureTargetPulmVeinsRight) / VascularFlowTargetRightInferiorLobeSuperiorPulmonaryCapillaries;
    double ResistanceRightInferiorLobeMedialBasalPulmonaryVeins = (VascularPressureTargetPulmCapRight - VascularPressureTargetPulmVeinsRight) / VascularFlowTargetRightInferiorLobeMedialBasalPulmonaryCapillaries;
    double ResistanceRightInferiorLobeAnteriorBasalPulmonaryVeins = (VascularPressureTargetPulmCapRight - VascularPressureTargetPulmVeinsRight) / VascularFlowTargetRightInferiorLobeAnteriorBasalPulmonaryCapillaries;
    double ResistanceRightInferiorLobeLateralBasalPulmonaryVeins = (VascularPressureTargetPulmCapRight - VascularPressureTargetPulmVeinsRight) / VascularFlowTargetRightInferiorLobeLateralBasalPulmonaryCapillaries;
    double ResistanceRightInferiorLobePosteriorBasalPulmonaryVeins = (VascularPressureTargetPulmCapRight - VascularPressureTargetPulmVeinsRight) / VascularFlowTargetRightInferiorLobePosteriorBasalPulmonaryCapillaries;

    double ResistanceLeftInferiorLobePosteriorBasalPulmonaryVeins = (VascularPressureTargetPulmCapLeft - VascularPressureTargetPulmVeinsLeft) / VascularFlowTargetLeftInferiorLobePosteriorBasalPulmonaryCapillaries;
    double ResistanceLeftInferiorLobeLateralBasalPulmonaryVeins = (VascularPressureTargetPulmCapLeft - VascularPressureTargetPulmVeinsLeft) / VascularFlowTargetLeftInferiorLobeLateralBasalPulmonaryCapillaries;
    double ResistanceLeftInferiorLobeAnteromedialBasalPulmonaryVeins = (VascularPressureTargetPulmCapLeft - VascularPressureTargetPulmVeinsLeft) / VascularFlowTargetLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries;
    double ResistanceLeftInferiorLobeSuperiorPulmonaryVeins = (VascularPressureTargetPulmCapLeft - VascularPressureTargetPulmVeinsLeft) / VascularFlowTargetLeftInferiorLobeSuperiorPulmonaryCapillaries;
    double ResistanceLeftSuperiorLobeInferiorLingulaPulmonaryVeins = (VascularPressureTargetPulmCapLeft - VascularPressureTargetPulmVeinsLeft) / VascularFlowTargetLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries;
    double ResistanceLeftSuperiorLobeSuperiorLingulaPulmonaryVeins = (VascularPressureTargetPulmCapLeft - VascularPressureTargetPulmVeinsLeft) / VascularFlowTargetLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries;
    double ResistanceLeftSuperiorLobeAnteriorPulmonaryVeins = (VascularPressureTargetPulmCapLeft - VascularPressureTargetPulmVeinsLeft) / VascularFlowTargetLeftSuperiorLobeAnteriorPulmonaryCapillaries;
    double ResistanceLeftSuperiorLobeApicoposteriorPulmonaryVeins = (VascularPressureTargetPulmCapLeft - VascularPressureTargetPulmVeinsLeft) / VascularFlowTargetLeftSuperiorLobeApicoposteriorPulmonaryCapillaries;

    // Portal vein and shunt are just paths - only have resistance
    double ResistancePortalVein = 0.001; // The portal vein is just a pathway in Pulse. The pressure across this path does not represent portal vein pressure (if it did our patient would always be portal hypertensive)
    double ResistanceShuntRight = (VascularPressureTargetPulmArtRight - VascularPressureTargetPulmCapRight) / (cardiacOutputTarget_mL_Per_s * pulmonaryShuntFractionFactor);
    double ResistanceShuntLeft = (VascularPressureTargetPulmArtLeft - VascularPressureTargetPulmCapLeft) / (cardiacOutputTarget_mL_Per_s * pulmonaryShuntFractionFactor);

    // Make a circuit
    SEFluidCircuit& cCardiovascular = m_Circuits->GetCardiovascularCircuit();

    // Create Nodes, set volume baselines and pressures where appropriate
    SEFluidCircuitNode& RightHeart1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::RightHeart1);
    RightHeart1.GetPressure().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitNode& RightHeart2 = cCardiovascular.CreateNode(pulse::CardiovascularNode::RightHeart2);
    SEFluidCircuitNode& RightHeart3 = cCardiovascular.CreateNode(pulse::CardiovascularNode::RightHeart3);
    RightHeart3.GetPressure().SetValue(0.0, PressureUnit::mmHg);
    RightHeart1.GetVolumeBaseline().SetValue(VolumeFractionHeartRight * bloodVolume_mL, VolumeUnit::mL);

    SEFluidCircuitNode& MainPulmonaryArteries1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::MainPulmonaryArteries1);

    SEFluidCircuitNode& RightIntermediatePulmonaryArteries1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::RightIntermediatePulmonaryArteries1);
    SEFluidCircuitNode& RightPulmonaryArteries1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::RightPulmonaryArteries1);
    RightPulmonaryArteries1.GetVolumeBaseline().SetValue(VolumeFractionPulmArtRight * bloodVolume_mL, VolumeUnit::mL);
    RightPulmonaryArteries1.GetPressure().SetValue(VascularPressureTargetPulmArtRight, PressureUnit::mmHg);

    SEFluidCircuitNode& LeftIntermediatePulmonaryArteries1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::LeftIntermediatePulmonaryArteries1);
    SEFluidCircuitNode& LeftPulmonaryArteries1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::LeftPulmonaryArteries1);
    LeftPulmonaryArteries1.GetVolumeBaseline().SetValue(VolumeFractionPulmArtLeft * bloodVolume_mL, VolumeUnit::mL);
    LeftPulmonaryArteries1.GetPressure().SetValue(VascularPressureTargetPulmArtLeft, PressureUnit::mmHg);

    //SEFluidCircuitNode& RightPulmonaryCapillaries1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::RightPulmonaryCapillaries1);
    //RightPulmonaryCapillaries1.GetVolumeBaseline().SetValue(VolumeFractionPulmCapRight * bloodVolume_mL, VolumeUnit::mL);
    //RightPulmonaryCapillaries1.GetPressure().SetValue(VascularPressureTargetPulmCapRight, PressureUnit::mmHg);

    //SEFluidCircuitNode& LeftPulmonaryCapillaries1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::LeftPulmonaryCapillaries1);
    //LeftPulmonaryCapillaries1.GetVolumeBaseline().SetValue(VolumeFractionPulmCapLeft * bloodVolume_mL, VolumeUnit::mL);
    //LeftPulmonaryCapillaries1.GetPressure().SetValue(VascularPressureTargetPulmCapLeft, PressureUnit::mmHg);

    SEFluidCircuitNode& RightIntermediatePulmonaryVeins1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::RightIntermediatePulmonaryVeins1);
    SEFluidCircuitNode& RightPulmonaryVeins1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::RightPulmonaryVeins1);
    RightPulmonaryVeins1.GetVolumeBaseline().SetValue(VolumeFractionPulmVeinsRight * bloodVolume_mL, VolumeUnit::mL);
    RightPulmonaryVeins1.GetPressure().SetValue(VascularPressureTargetPulmVeinsRight, PressureUnit::mmHg);

    SEFluidCircuitNode& LeftIntermediatePulmonaryVeins1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::LeftIntermediatePulmonaryVeins1);
    SEFluidCircuitNode& LeftPulmonaryVeins1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::LeftPulmonaryVeins1);
    LeftPulmonaryVeins1.GetVolumeBaseline().SetValue(VolumeFractionPulmVeinsLeft * bloodVolume_mL, VolumeUnit::mL);
    LeftPulmonaryVeins1.GetPressure().SetValue(VascularPressureTargetPulmVeinsLeft, PressureUnit::mmHg);

    SEFluidCircuitNode& LeftHeart1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::LeftHeart1);
    LeftHeart1.GetPressure().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitNode& LeftHeart2 = cCardiovascular.CreateNode(pulse::CardiovascularNode::LeftHeart2);
    SEFluidCircuitNode& LeftHeart3 = cCardiovascular.CreateNode(pulse::CardiovascularNode::LeftHeart3);
    LeftHeart3.GetPressure().SetValue(0.0, PressureUnit::mmHg);
    LeftHeart1.GetVolumeBaseline().SetValue(VolumeFractionHeartLeft * bloodVolume_mL, VolumeUnit::mL);

    SEFluidCircuitNode& Aorta1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::Aorta1);
    SEFluidCircuitNode& Aorta2 = cCardiovascular.CreateNode(pulse::CardiovascularNode::Aorta2);
    SEFluidCircuitNode& Aorta3 = cCardiovascular.CreateNode(pulse::CardiovascularNode::Aorta3);
    SEFluidCircuitNode& Aorta4 = cCardiovascular.CreateNode(pulse::CardiovascularNode::Aorta4);
    Aorta4.GetPressure().SetValue(0.0, PressureUnit::mmHg);
    Aorta1.GetVolumeBaseline().SetValue(VolumeFractionAorta * bloodVolume_mL, VolumeUnit::mL);
    Aorta1.GetPressure().SetValue(VascularPressureTargetAorta, PressureUnit::mmHg);

    SEFluidCircuitNode& Brain1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::Brain1);
    SEFluidCircuitNode& Brain2 = cCardiovascular.CreateNode(pulse::CardiovascularNode::Brain2);
    Brain1.GetVolumeBaseline().SetValue(VolumeFractionBrain * bloodVolume_mL, VolumeUnit::mL);
    Brain1.GetPressure().SetValue(0.0, PressureUnit::mmHg);
    Brain1.GetPressure().SetValue(VascularPressureTargetBrain, PressureUnit::mmHg);

    SEFluidCircuitNode& Bone1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::Bone1);
    SEFluidCircuitNode& Bone2 = cCardiovascular.CreateNode(pulse::CardiovascularNode::Bone2);
    Bone1.GetVolumeBaseline().SetValue(VolumeFractionBone * bloodVolume_mL, VolumeUnit::mL);
    Bone1.GetPressure().SetValue(VascularPressureTargetBone, PressureUnit::mmHg);

    SEFluidCircuitNode& Fat1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::Fat1);
    SEFluidCircuitNode& Fat2 = cCardiovascular.CreateNode(pulse::CardiovascularNode::Fat2);
    Fat1.GetVolumeBaseline().SetValue(VolumeFractionFat * bloodVolume_mL, VolumeUnit::mL);
    Fat1.GetPressure().SetValue(VascularPressureTargetFat, PressureUnit::mmHg);

    SEFluidCircuitNode& LargeIntestine1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::LargeIntestine1);
    LargeIntestine1.GetVolumeBaseline().SetValue(VolumeFractionLargeIntestine * bloodVolume_mL, VolumeUnit::mL);
    LargeIntestine1.GetPressure().SetValue(VascularPressureTargetLargeIntestine, PressureUnit::mmHg);

    SEFluidCircuitNode& Liver1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::Liver1);
    SEFluidCircuitNode& Liver2 = cCardiovascular.CreateNode(pulse::CardiovascularNode::Liver2);
    Liver1.GetVolumeBaseline().SetValue(VolumeFractionLiver * bloodVolume_mL, VolumeUnit::mL);
    Liver1.GetPressure().SetValue(VascularPressureTargetLiver, PressureUnit::mmHg);

    SEFluidCircuitNode& LeftArm1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::LeftArm1);
    SEFluidCircuitNode& LeftArm2 = cCardiovascular.CreateNode(pulse::CardiovascularNode::LeftArm2);
    LeftArm1.GetVolumeBaseline().SetValue(VolumeFractionArm * bloodVolume_mL, VolumeUnit::mL);
    LeftArm1.GetPressure().SetValue(VascularPressureTargetArm, PressureUnit::mmHg);

    SEFluidCircuitNode& LeftKidney1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::LeftKidney1);
    SEFluidCircuitNode& LeftKidney2 = cCardiovascular.CreateNode(pulse::CardiovascularNode::LeftKidney2);
    LeftKidney1.GetVolumeBaseline().SetValue(0.5 * VolumeFractionKidney * bloodVolume_mL, VolumeUnit::mL);
    LeftKidney1.GetPressure().SetValue(VascularPressureTargetKidney, PressureUnit::mmHg);

    SEFluidCircuitNode& LeftLeg1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::LeftLeg1);
    SEFluidCircuitNode& LeftLeg2 = cCardiovascular.CreateNode(pulse::CardiovascularNode::LeftLeg2);
    LeftLeg1.GetVolumeBaseline().SetValue(VolumeFractionLeg * bloodVolume_mL, VolumeUnit::mL);
    LeftLeg1.GetPressure().SetValue(VascularPressureTargetLeg, PressureUnit::mmHg);

    SEFluidCircuitNode& Muscle1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::Muscle1);
    SEFluidCircuitNode& Muscle2 = cCardiovascular.CreateNode(pulse::CardiovascularNode::Muscle2);
    Muscle1.GetVolumeBaseline().SetValue(VolumeFractionMuscle * bloodVolume_mL, VolumeUnit::mL);
    Muscle1.GetPressure().SetValue(VascularPressureTargetMuscle, PressureUnit::mmHg);

    SEFluidCircuitNode& Myocardium1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::Myocardium1);
    SEFluidCircuitNode& Myocardium2 = cCardiovascular.CreateNode(pulse::CardiovascularNode::Myocardium2);
    Myocardium1.GetVolumeBaseline().SetValue(VolumeFractionMyocardium * bloodVolume_mL, VolumeUnit::mL);
    Myocardium1.GetPressure().SetValue(VascularPressureTargetMyocardium, PressureUnit::mmHg);

    SEFluidCircuitNode& PortalVein1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::PortalVein1);

    SEFluidCircuitNode& RightArm1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::RightArm1);
    SEFluidCircuitNode& RightArm2 = cCardiovascular.CreateNode(pulse::CardiovascularNode::RightArm2);
    RightArm1.GetVolumeBaseline().SetValue(VolumeFractionArm * bloodVolume_mL, VolumeUnit::mL);
    RightArm1.GetPressure().SetValue(VascularPressureTargetArm, PressureUnit::mmHg);

    SEFluidCircuitNode& RightKidney1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::RightKidney1);
    SEFluidCircuitNode& RightKidney2 = cCardiovascular.CreateNode(pulse::CardiovascularNode::RightKidney2);
    RightKidney1.GetVolumeBaseline().SetValue(0.5 * VolumeFractionKidney * bloodVolume_mL, VolumeUnit::mL);
    RightKidney1.GetPressure().SetValue(VascularPressureTargetKidney, PressureUnit::mmHg);

    SEFluidCircuitNode& RightLeg1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::RightLeg1);
    SEFluidCircuitNode& RightLeg2 = cCardiovascular.CreateNode(pulse::CardiovascularNode::RightLeg2);
    RightLeg1.GetVolumeBaseline().SetValue(VolumeFractionLeg * bloodVolume_mL, VolumeUnit::mL);
    RightLeg1.GetPressure().SetValue(VascularPressureTargetLeg, PressureUnit::mmHg);

    SEFluidCircuitNode& Skin1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::Skin1);
    SEFluidCircuitNode& Skin2 = cCardiovascular.CreateNode(pulse::CardiovascularNode::Skin2);
    Skin1.GetVolumeBaseline().SetValue(VolumeFractionSkin * bloodVolume_mL, VolumeUnit::mL);
    Skin1.GetPressure().SetValue(VascularPressureTargetSkin, PressureUnit::mmHg);

    SEFluidCircuitNode& SmallIntestine1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::SmallIntestine1);
    SmallIntestine1.GetVolumeBaseline().SetValue(VolumeFractionSmallIntestine * bloodVolume_mL, VolumeUnit::mL);
    SmallIntestine1.GetPressure().SetValue(VascularPressureTargetSmallIntestine, PressureUnit::mmHg);

    SEFluidCircuitNode& Splanchnic1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::Splanchnic1);
    Splanchnic1.GetVolumeBaseline().SetValue(VolumeFractionSplanchnic * bloodVolume_mL, VolumeUnit::mL);
    Splanchnic1.GetPressure().SetValue(VascularPressureTargetSplanchnic, PressureUnit::mmHg);

    SEFluidCircuitNode& Spleen1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::Spleen1);
    Spleen1.GetVolumeBaseline().SetValue(VolumeFractionSpleen * bloodVolume_mL, VolumeUnit::mL);
    Spleen1.GetPressure().SetValue(VascularPressureTargetSpleen, PressureUnit::mmHg);

    SEFluidCircuitNode& VenaCava1 = cCardiovascular.CreateNode(pulse::CardiovascularNode::VenaCava1);
    VenaCava1.GetVolumeBaseline().SetValue(VolumeFractionVenaCava * bloodVolume_mL, VolumeUnit::mL);
    VenaCava1.GetPressure().SetValue(VascularPressureTargetVenaCava, PressureUnit::mmHg);

    SEFluidCircuitNode& Ground = cCardiovascular.CreateNode(pulse::CardiovascularNode::Ground);
    Ground.SetAsReferenceNode();
    Ground.GetPressure().SetValue(0.0, PressureUnit::mmHg);

    double blood_mL = 0;
    for (SEFluidCircuitNode* n : cCardiovascular.GetNodes())
    {
      if (n->HasVolumeBaseline())
        blood_mL += n->GetVolumeBaseline(VolumeUnit::mL);
    }
    if (blood_mL > bloodVolume_mL)
      Error("Blood volume greater than total blood volume");

    SEFluidCircuitNode& Pericardium = cCardiovascular.CreateNode(pulse::CardiovascularNode::Pericardium1);
    Pericardium.GetVolumeBaseline().SetValue(15.0, VolumeUnit::mL);
    Pericardium.GetPressure().SetValue(1.0, PressureUnit::mmHg);

    SEFluidCircuitNode& AbdominalCavity = cCardiovascular.CreateNode(pulse::CardiovascularNode::AbdominalCavity1);
    AbdominalCavity.GetVolumeBaseline().SetValue(10.0, VolumeUnit::mL);
    AbdominalCavity.GetPressure().SetValue(0.0, PressureUnit::mmHg);

    SEFluidCircuitNode& RightSuperiorLobeApicalPulmonaryCapillaries = cCardiovascular.CreateNode(pulse::ExpandedLungsCardiovascularNode::RightSuperiorLobeApicalPulmonaryCapillaries);
    SEFluidCircuitNode& RightSuperiorLobePosteriorPulmonaryCapillaries = cCardiovascular.CreateNode(pulse::ExpandedLungsCardiovascularNode::RightSuperiorLobePosteriorPulmonaryCapillaries);
    SEFluidCircuitNode& RightSuperiorLobeAnteriorPulmonaryCapillaries = cCardiovascular.CreateNode(pulse::ExpandedLungsCardiovascularNode::RightSuperiorLobeAnteriorPulmonaryCapillaries);
    SEFluidCircuitNode& RightMiddleLobeLateralPulmonaryCapillaries = cCardiovascular.CreateNode(pulse::ExpandedLungsCardiovascularNode::RightMiddleLobeLateralPulmonaryCapillaries);
    SEFluidCircuitNode& RightMiddleLobeMedialPulmonaryCapillaries = cCardiovascular.CreateNode(pulse::ExpandedLungsCardiovascularNode::RightMiddleLobeMedialPulmonaryCapillaries);
    SEFluidCircuitNode& RightInferiorLobeSuperiorPulmonaryCapillaries = cCardiovascular.CreateNode(pulse::ExpandedLungsCardiovascularNode::RightInferiorLobeSuperiorPulmonaryCapillaries);
    SEFluidCircuitNode& RightInferiorLobeMedialBasalPulmonaryCapillaries = cCardiovascular.CreateNode(pulse::ExpandedLungsCardiovascularNode::RightInferiorLobeMedialBasalPulmonaryCapillaries);
    SEFluidCircuitNode& RightInferiorLobeAnteriorBasalPulmonaryCapillaries = cCardiovascular.CreateNode(pulse::ExpandedLungsCardiovascularNode::RightInferiorLobeAnteriorBasalPulmonaryCapillaries);
    SEFluidCircuitNode& RightInferiorLobeLateralBasalPulmonaryCapillaries = cCardiovascular.CreateNode(pulse::ExpandedLungsCardiovascularNode::RightInferiorLobeLateralBasalPulmonaryCapillaries);
    SEFluidCircuitNode& RightInferiorLobePosteriorBasalPulmonaryCapillaries = cCardiovascular.CreateNode(pulse::ExpandedLungsCardiovascularNode::RightInferiorLobePosteriorBasalPulmonaryCapillaries);

    SEFluidCircuitNode& LeftInferiorLobePosteriorBasalPulmonaryCapillaries = cCardiovascular.CreateNode(pulse::ExpandedLungsCardiovascularNode::LeftInferiorLobePosteriorBasalPulmonaryCapillaries);
    SEFluidCircuitNode& LeftInferiorLobeLateralBasalPulmonaryCapillaries = cCardiovascular.CreateNode(pulse::ExpandedLungsCardiovascularNode::LeftInferiorLobeLateralBasalPulmonaryCapillaries);
    SEFluidCircuitNode& LeftInferiorLobeAnteromedialBasalPulmonaryCapillaries = cCardiovascular.CreateNode(pulse::ExpandedLungsCardiovascularNode::LeftInferiorLobeAnteromedialBasalPulmonaryCapillaries);
    SEFluidCircuitNode& LeftInferiorLobeSuperiorPulmonaryCapillaries = cCardiovascular.CreateNode(pulse::ExpandedLungsCardiovascularNode::LeftInferiorLobeSuperiorPulmonaryCapillaries);
    SEFluidCircuitNode& LeftSuperiorLobeInferiorLingulaPulmonaryCapillaries = cCardiovascular.CreateNode(pulse::ExpandedLungsCardiovascularNode::LeftSuperiorLobeInferiorLingulaPulmonaryCapillaries);
    SEFluidCircuitNode& LeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries = cCardiovascular.CreateNode(pulse::ExpandedLungsCardiovascularNode::LeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries);
    SEFluidCircuitNode& LeftSuperiorLobeAnteriorPulmonaryCapillaries = cCardiovascular.CreateNode(pulse::ExpandedLungsCardiovascularNode::LeftSuperiorLobeAnteriorPulmonaryCapillaries);
    SEFluidCircuitNode& LeftSuperiorLobeApicoposteriorPulmonaryCapillaries = cCardiovascular.CreateNode(pulse::ExpandedLungsCardiovascularNode::LeftSuperiorLobeApicoposteriorPulmonaryCapillaries);

    RightSuperiorLobeApicalPulmonaryCapillaries.GetVolumeBaseline().SetValue(VolumeFractionRightSuperiorLobeApicalPulmonaryCapillaries * bloodVolume_mL, VolumeUnit::mL);
    RightSuperiorLobePosteriorPulmonaryCapillaries.GetVolumeBaseline().SetValue(VolumeFractionRightSuperiorLobePosteriorPulmonaryCapillaries * bloodVolume_mL, VolumeUnit::mL);
    RightSuperiorLobeAnteriorPulmonaryCapillaries.GetVolumeBaseline().SetValue(VolumeFractionRightSuperiorLobeAnteriorPulmonaryCapillaries * bloodVolume_mL, VolumeUnit::mL);
    RightMiddleLobeLateralPulmonaryCapillaries.GetVolumeBaseline().SetValue(VolumeFractionRightMiddleLobeLateralPulmonaryCapillaries * bloodVolume_mL, VolumeUnit::mL);
    RightMiddleLobeMedialPulmonaryCapillaries.GetVolumeBaseline().SetValue(VolumeFractionRightMiddleLobeMedialPulmonaryCapillaries * bloodVolume_mL, VolumeUnit::mL);
    RightInferiorLobeSuperiorPulmonaryCapillaries.GetVolumeBaseline().SetValue(VolumeFractionRightInferiorLobeSuperiorPulmonaryCapillaries * bloodVolume_mL, VolumeUnit::mL);
    RightInferiorLobeMedialBasalPulmonaryCapillaries.GetVolumeBaseline().SetValue(VolumeFractionRightInferiorLobeMedialBasalPulmonaryCapillaries * bloodVolume_mL, VolumeUnit::mL);
    RightInferiorLobeAnteriorBasalPulmonaryCapillaries.GetVolumeBaseline().SetValue(VolumeFractionRightInferiorLobeAnteriorBasalPulmonaryCapillaries * bloodVolume_mL, VolumeUnit::mL);
    RightInferiorLobeLateralBasalPulmonaryCapillaries.GetVolumeBaseline().SetValue(VolumeFractionRightInferiorLobeLateralBasalPulmonaryCapillaries * bloodVolume_mL, VolumeUnit::mL);
    RightInferiorLobePosteriorBasalPulmonaryCapillaries.GetVolumeBaseline().SetValue(VolumeFractionRightInferiorLobePosteriorBasalPulmonaryCapillaries * bloodVolume_mL, VolumeUnit::mL);

    LeftInferiorLobePosteriorBasalPulmonaryCapillaries.GetVolumeBaseline().SetValue(VolumeFractionLeftInferiorLobePosteriorBasalPulmonaryCapillaries * bloodVolume_mL, VolumeUnit::mL);
    LeftInferiorLobeLateralBasalPulmonaryCapillaries.GetVolumeBaseline().SetValue(VolumeFractionLeftInferiorLobeLateralBasalPulmonaryCapillaries * bloodVolume_mL, VolumeUnit::mL);
    LeftInferiorLobeAnteromedialBasalPulmonaryCapillaries.GetVolumeBaseline().SetValue(VolumeFractionLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries * bloodVolume_mL, VolumeUnit::mL);
    LeftInferiorLobeSuperiorPulmonaryCapillaries.GetVolumeBaseline().SetValue(VolumeFractionLeftInferiorLobeSuperiorPulmonaryCapillaries * bloodVolume_mL, VolumeUnit::mL);
    LeftSuperiorLobeInferiorLingulaPulmonaryCapillaries.GetVolumeBaseline().SetValue(VolumeFractionLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries * bloodVolume_mL, VolumeUnit::mL);
    LeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries.GetVolumeBaseline().SetValue(VolumeFractionLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries * bloodVolume_mL, VolumeUnit::mL);
    LeftSuperiorLobeAnteriorPulmonaryCapillaries.GetVolumeBaseline().SetValue(VolumeFractionLeftSuperiorLobeAnteriorPulmonaryCapillaries * bloodVolume_mL, VolumeUnit::mL);
    LeftSuperiorLobeApicoposteriorPulmonaryCapillaries.GetVolumeBaseline().SetValue(VolumeFractionLeftSuperiorLobeApicoposteriorPulmonaryCapillaries * bloodVolume_mL, VolumeUnit::mL);

    double totalRightVolume_mL =
      RightSuperiorLobeApicalPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) +
      RightSuperiorLobePosteriorPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) +
      RightSuperiorLobeAnteriorPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) +
      RightMiddleLobeLateralPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) +
      RightMiddleLobeMedialPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) +
      RightInferiorLobeSuperiorPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) +
      RightInferiorLobeMedialBasalPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) +
      RightInferiorLobeAnteriorBasalPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) +
      RightInferiorLobeLateralBasalPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) +
      RightInferiorLobePosteriorBasalPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL);

    double totalLeftVolume_mL =
      LeftInferiorLobePosteriorBasalPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) +
      LeftInferiorLobeLateralBasalPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) +
      LeftInferiorLobeAnteromedialBasalPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) +
      LeftInferiorLobeSuperiorPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) +
      LeftSuperiorLobeInferiorLingulaPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) +
      LeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) +
      LeftSuperiorLobeAnteriorPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) +
      LeftSuperiorLobeApicoposteriorPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL);

    double totalVolume_mL = totalRightVolume_mL + totalLeftVolume_mL;

    RightSuperiorLobeApicalPulmonaryCapillaries.GetPressure().SetValue(VascularPressureTargetPulmCapRight, PressureUnit::mmHg);
    RightSuperiorLobePosteriorPulmonaryCapillaries.GetPressure().SetValue(VascularPressureTargetPulmCapRight, PressureUnit::mmHg);
    RightSuperiorLobeAnteriorPulmonaryCapillaries.GetPressure().SetValue(VascularPressureTargetPulmCapRight, PressureUnit::mmHg);
    RightMiddleLobeLateralPulmonaryCapillaries.GetPressure().SetValue(VascularPressureTargetPulmCapRight, PressureUnit::mmHg);
    RightMiddleLobeMedialPulmonaryCapillaries.GetPressure().SetValue(VascularPressureTargetPulmCapRight, PressureUnit::mmHg);
    RightInferiorLobeSuperiorPulmonaryCapillaries.GetPressure().SetValue(VascularPressureTargetPulmCapRight, PressureUnit::mmHg);
    RightInferiorLobeMedialBasalPulmonaryCapillaries.GetPressure().SetValue(VascularPressureTargetPulmCapRight, PressureUnit::mmHg);
    RightInferiorLobeAnteriorBasalPulmonaryCapillaries.GetPressure().SetValue(VascularPressureTargetPulmCapRight, PressureUnit::mmHg);
    RightInferiorLobeLateralBasalPulmonaryCapillaries.GetPressure().SetValue(VascularPressureTargetPulmCapRight, PressureUnit::mmHg);
    RightInferiorLobePosteriorBasalPulmonaryCapillaries.GetPressure().SetValue(VascularPressureTargetPulmCapRight, PressureUnit::mmHg);

    LeftInferiorLobePosteriorBasalPulmonaryCapillaries.GetPressure().SetValue(VascularPressureTargetPulmCapLeft, PressureUnit::mmHg);
    LeftInferiorLobeLateralBasalPulmonaryCapillaries.GetPressure().SetValue(VascularPressureTargetPulmCapLeft, PressureUnit::mmHg);
    LeftInferiorLobeAnteromedialBasalPulmonaryCapillaries.GetPressure().SetValue(VascularPressureTargetPulmCapLeft, PressureUnit::mmHg);
    LeftInferiorLobeSuperiorPulmonaryCapillaries.GetPressure().SetValue(VascularPressureTargetPulmCapLeft, PressureUnit::mmHg);
    LeftSuperiorLobeInferiorLingulaPulmonaryCapillaries.GetPressure().SetValue(VascularPressureTargetPulmCapLeft, PressureUnit::mmHg);
    LeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries.GetPressure().SetValue(VascularPressureTargetPulmCapLeft, PressureUnit::mmHg);
    LeftSuperiorLobeAnteriorPulmonaryCapillaries.GetPressure().SetValue(VascularPressureTargetPulmCapLeft, PressureUnit::mmHg);
    LeftSuperiorLobeApicoposteriorPulmonaryCapillaries.GetPressure().SetValue(VascularPressureTargetPulmCapLeft, PressureUnit::mmHg);

    // Create Paths, set switch (diodes), compliances, and resistances where appropriate
    SEFluidCircuitPath& VenaCava1ToRightHeart2 = cCardiovascular.CreatePath(VenaCava1, RightHeart2, pulse::CardiovascularPath::VenaCava1ToRightHeart2);
    VenaCava1ToRightHeart2.GetResistanceBaseline().SetValue(ResistanceHeartRight, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightHeart2ToRightHeart1 = cCardiovascular.CreatePath(RightHeart2, RightHeart1, pulse::CardiovascularPath::RightHeart2ToRightHeart1);
    RightHeart2ToRightHeart1.SetNextValve(eGate::Closed);
    SEFluidCircuitPath& RightHeart1ToRightHeart3 = cCardiovascular.CreatePath(RightHeart1, RightHeart3, pulse::CardiovascularPath::RightHeart1ToRightHeart3);
    SEFluidCircuitPath& RightHeart3ToGround = cCardiovascular.CreatePath(Ground, RightHeart3, pulse::CardiovascularPath::RightHeart3ToGround);
    RightHeart3ToGround.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);

    SEFluidCircuitPath& RightHeart1ToMainPulmonaryArteries1 = cCardiovascular.CreatePath(RightHeart1, MainPulmonaryArteries1, pulse::CardiovascularPath::RightHeart1ToMainPulmonaryArteries1);
    RightHeart1ToMainPulmonaryArteries1.SetNextValve(eGate::Closed);

    SEFluidCircuitPath& MainPulmonaryArteries1ToRightIntermediatePulmonaryArteries1 = cCardiovascular.CreatePath(MainPulmonaryArteries1, RightIntermediatePulmonaryArteries1, pulse::CardiovascularPath::MainPulmonaryArteries1ToRightIntermediatePulmonaryArteries1);
    //MainPulmonaryArteries1ToRightIntermediatePulmonaryArteries1.SetNextValve(eGate::Closed);
    SEFluidCircuitPath& RightIntermediatePulmonaryArteries1ToRightPulmonaryArteries1 = cCardiovascular.CreatePath(RightIntermediatePulmonaryArteries1, RightPulmonaryArteries1, pulse::CardiovascularPath::RightIntermediatePulmonaryArteries1ToRightPulmonaryArteries1);
    RightIntermediatePulmonaryArteries1ToRightPulmonaryArteries1.GetResistanceBaseline().SetValue(ResistancePulmArt, PressureTimePerVolumeUnit::mmHg_s_Per_mL);

    //SEFluidCircuitPath& RightPulmonaryArteries1ToRightPulmonaryVeins1 = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightPulmonaryVeins1, pulse::CardiovascularPath::RightPulmonaryArteries1ToRightPulmonaryVeins1);
    //RightPulmonaryArteries1ToRightPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceShuntRight, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    //SEFluidCircuitPath& RightPulmonaryArteries1ToRightPulmonaryCapillaries1 = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightPulmonaryCapillaries1, pulse::CardiovascularPath::RightPulmonaryArteries1ToRightPulmonaryCapillaries1);
    //RightPulmonaryArteries1ToRightPulmonaryCapillaries1.GetResistanceBaseline().SetValue(ResistancePulmCapRight, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightPulmonaryArteries1ToGround = cCardiovascular.CreatePath(RightPulmonaryArteries1, Ground, pulse::CardiovascularPath::RightPulmonaryArteries1ToGround);
    RightPulmonaryArteries1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    //SEFluidCircuitPath& RightPulmonaryCapillaries1ToRightPulmonaryVeins1 = cCardiovascular.CreatePath(RightPulmonaryCapillaries1, RightPulmonaryVeins1, pulse::CardiovascularPath::RightPulmonaryCapillaries1ToRightPulmonaryVeins1);
    //RightPulmonaryCapillaries1ToRightPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistancePulmVeinsRight, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    //SEFluidCircuitPath& RightPulmonaryCapillaries1ToGround = cCardiovascular.CreatePath(RightPulmonaryCapillaries1, Ground, pulse::CardiovascularPath::RightPulmonaryCapillaries1ToGround);
    //RightPulmonaryCapillaries1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);

    SEFluidCircuitPath& RightPulmonaryVeins1ToRightIntermediatePulmonaryVeins1 = cCardiovascular.CreatePath(RightPulmonaryVeins1, RightIntermediatePulmonaryVeins1, pulse::CardiovascularPath::RightPulmonaryVeins1ToRightIntermediatePulmonaryVeins1);
    RightPulmonaryVeins1ToRightIntermediatePulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceHeartLeft, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightPulmonaryVeins1ToGround = cCardiovascular.CreatePath(RightPulmonaryVeins1, Ground, pulse::CardiovascularPath::RightPulmonaryVeins1ToGround);
    RightPulmonaryVeins1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& RightIntermediatePulmonaryVeins1ToLeftHeart2 = cCardiovascular.CreatePath(RightIntermediatePulmonaryVeins1, LeftHeart2, pulse::CardiovascularPath::RightIntermediatePulmonaryVeins1ToLeftHeart2);
    //RightIntermediatePulmonaryVeins1ToLeftHeart2.SetNextValve(eGate::Closed);

    SEFluidCircuitPath& MainPulmonaryArteries1ToLeftIntermediatePulmonaryArteries1 = cCardiovascular.CreatePath(MainPulmonaryArteries1, LeftIntermediatePulmonaryArteries1, pulse::CardiovascularPath::MainPulmonaryArteries1ToLeftIntermediatePulmonaryArteries1);
    //MainPulmonaryArteries1ToLeftIntermediatePulmonaryArteries1.SetNextValve(eGate::Closed);
    SEFluidCircuitPath& LeftIntermediatePulmonaryArteries1ToLeftPulmonaryArteries1 = cCardiovascular.CreatePath(LeftIntermediatePulmonaryArteries1, LeftPulmonaryArteries1, pulse::CardiovascularPath::LeftIntermediatePulmonaryArteries1ToLeftPulmonaryArteries1);
    LeftIntermediatePulmonaryArteries1ToLeftPulmonaryArteries1.GetResistanceBaseline().SetValue(ResistancePulmArt, PressureTimePerVolumeUnit::mmHg_s_Per_mL);

    //SEFluidCircuitPath& LeftPulmonaryArteries1ToLeftPulmonaryVeins1 = cCardiovascular.CreatePath(LeftPulmonaryArteries1, LeftPulmonaryVeins1, pulse::CardiovascularPath::LeftPulmonaryArteries1ToLeftPulmonaryVeins1);
    //LeftPulmonaryArteries1ToLeftPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceShuntLeft, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    //SEFluidCircuitPath& LeftPulmonaryArteries1ToLeftPulmonaryCapillaries1 = cCardiovascular.CreatePath(LeftPulmonaryArteries1, LeftPulmonaryCapillaries1, pulse::CardiovascularPath::LeftPulmonaryArteries1ToLeftPulmonaryCapillaries1);
    //LeftPulmonaryArteries1ToLeftPulmonaryCapillaries1.GetResistanceBaseline().SetValue(ResistancePulmCapLeft, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftPulmonaryArteries1ToGround = cCardiovascular.CreatePath(LeftPulmonaryArteries1, Ground, pulse::CardiovascularPath::LeftPulmonaryArteries1ToGround);
    LeftPulmonaryArteries1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    //SEFluidCircuitPath& LeftPulmonaryCapillaries1ToGround = cCardiovascular.CreatePath(LeftPulmonaryCapillaries1, Ground, pulse::CardiovascularPath::LeftPulmonaryCapillaries1ToGround);
    //LeftPulmonaryCapillaries1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    //SEFluidCircuitPath& LeftPulmonaryCapillaries1ToLeftPulmonaryVeins1 = cCardiovascular.CreatePath(LeftPulmonaryCapillaries1, LeftPulmonaryVeins1, pulse::CardiovascularPath::LeftPulmonaryCapillaries1ToLeftPulmonaryVeins1);
    //LeftPulmonaryCapillaries1ToLeftPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistancePulmVeinsLeft, PressureTimePerVolumeUnit::mmHg_s_Per_mL);

    SEFluidCircuitPath& LeftPulmonaryVeins1ToLeftIntermediatePulmonaryVeins1 = cCardiovascular.CreatePath(LeftPulmonaryVeins1, LeftIntermediatePulmonaryVeins1, pulse::CardiovascularPath::LeftPulmonaryVeins1ToLeftIntermediatePulmonaryVeins1);
    LeftPulmonaryVeins1ToLeftIntermediatePulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceHeartLeft, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftPulmonaryVeins1ToGround = cCardiovascular.CreatePath(LeftPulmonaryVeins1, Ground, pulse::CardiovascularPath::LeftPulmonaryVeins1ToGround);
    LeftPulmonaryVeins1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& LeftIntermediatePulmonaryVeins1ToLeftHeart2 = cCardiovascular.CreatePath(LeftIntermediatePulmonaryVeins1, LeftHeart2, pulse::CardiovascularPath::LeftIntermediatePulmonaryVeins1ToLeftHeart2);
    //LeftIntermediatePulmonaryVeins1ToLeftHeart2.SetNextValve(eGate::Closed);
    SEFluidCircuitPath& LeftHeart2ToLeftHeart1 = cCardiovascular.CreatePath(LeftHeart2, LeftHeart1, pulse::CardiovascularPath::LeftHeart2ToLeftHeart1);
    LeftHeart2ToLeftHeart1.SetNextValve(eGate::Closed);
    SEFluidCircuitPath& LeftHeart1ToLeftHeart3 = cCardiovascular.CreatePath(LeftHeart1, LeftHeart3, pulse::CardiovascularPath::LeftHeart1ToLeftHeart3);

    SEFluidCircuitPath& LeftHeart3ToGround = cCardiovascular.CreatePath(Ground, LeftHeart3, pulse::CardiovascularPath::LeftHeart3ToGround);
    LeftHeart3ToGround.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);
    SEFluidCircuitPath& LeftHeart1ToAorta2 = cCardiovascular.CreatePath(LeftHeart1, Aorta2, pulse::CardiovascularPath::LeftHeart1ToAorta2);
    LeftHeart1ToAorta2.SetNextValve(eGate::Closed);
    cCardiovascular.CreatePath(Aorta2, Aorta3, pulse::CardiovascularPath::Aorta2ToAorta3);
    SEFluidCircuitPath& Aorta3ToAorta1 = cCardiovascular.CreatePath(Aorta3, Aorta1, pulse::CardiovascularPath::Aorta3ToAorta1);
    Aorta3ToAorta1.GetResistanceBaseline().SetValue(ResistanceAorta, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Aorta1ToAorta4 = cCardiovascular.CreatePath(Aorta1, Aorta4, pulse::CardiovascularPath::Aorta1ToAorta4);
    Aorta1ToAorta4.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);

    SEFluidCircuitPath& Aorta1ToBrain1 = cCardiovascular.CreatePath(Aorta1, Brain1, pulse::CardiovascularPath::Aorta1ToBrain1);
    Aorta1ToBrain1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceBrain, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Brain1ToGround = cCardiovascular.CreatePath(Brain1, Ground, pulse::CardiovascularPath::Brain1ToGround);
    Brain1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    /*SEFluidCircuitPath& GroundToBrain1 = cCardiovascular.CreatePath(Ground, Brain1, pulse::CardiovascularPath::GroundToBrain1);
    GroundToBrain1.GetPressureSourceBaseline().SetValue(1.0, PressureUnit::mmHg);*/
    SEFluidCircuitPath& Brain1ToBrain2 = cCardiovascular.CreatePath(Brain1, Brain2, pulse::CardiovascularPath::Brain1ToBrain2);
    Brain1ToBrain2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceBrainVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Brain2ToVenaCava1 = cCardiovascular.CreatePath(Brain2, VenaCava1, pulse::CardiovascularPath::Brain2ToVenaCava1);

    SEFluidCircuitPath& Aorta1ToBone1 = cCardiovascular.CreatePath(Aorta1, Bone1, pulse::CardiovascularPath::Aorta1ToBone1);
    Aorta1ToBone1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceBone, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Bone1ToGround = cCardiovascular.CreatePath(Bone1, Ground, pulse::CardiovascularPath::Bone1ToGround);
    Bone1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& Bone1ToBone2 = cCardiovascular.CreatePath(Bone1, Bone2, pulse::CardiovascularPath::Bone1ToBone2);
    Bone1ToBone2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceBoneVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Bone2ToVenaCava1 = cCardiovascular.CreatePath(Bone2, VenaCava1, pulse::CardiovascularPath::Bone2ToVenaCava1);

    SEFluidCircuitPath& Aorta1ToFat1 = cCardiovascular.CreatePath(Aorta1, Fat1, pulse::CardiovascularPath::Aorta1ToFat1);
    Aorta1ToFat1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceFat, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Fat1ToGround = cCardiovascular.CreatePath(Fat1, Ground, pulse::CardiovascularPath::Fat1ToGround);
    Fat1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& Fat1ToFat2 = cCardiovascular.CreatePath(Fat1, Fat2, pulse::CardiovascularPath::Fat1ToFat2);
    Fat1ToFat2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceFatVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Fat2ToVenaCava1 = cCardiovascular.CreatePath(Fat2, VenaCava1, pulse::CardiovascularPath::Fat2ToVenaCava1);

    SEFluidCircuitPath& Aorta1ToLargeIntestine1 = cCardiovascular.CreatePath(Aorta1, LargeIntestine1, pulse::CardiovascularPath::Aorta1ToLargeIntestine1);
    Aorta1ToLargeIntestine1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLargeIntestine, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LargeIntestine1ToGround = cCardiovascular.CreatePath(LargeIntestine1, Ground, pulse::CardiovascularPath::LargeIntestine1ToGround);
    LargeIntestine1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& LargeIntestine1ToPortalVein1 = cCardiovascular.CreatePath(LargeIntestine1, PortalVein1, pulse::CardiovascularPath::LargeIntestine1ToPortalVein1);
    LargeIntestine1ToPortalVein1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLargeIntestineVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);

    SEFluidCircuitPath& Aorta1ToLeftArm1 = cCardiovascular.CreatePath(Aorta1, LeftArm1, pulse::CardiovascularPath::Aorta1ToLeftArm1);
    Aorta1ToLeftArm1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArm, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftArm1ToGround = cCardiovascular.CreatePath(LeftArm1, Ground, pulse::CardiovascularPath::LeftArm1ToGround);
    LeftArm1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& LeftArm1ToLeftArm2 = cCardiovascular.CreatePath(LeftArm1, LeftArm2, pulse::CardiovascularPath::LeftArm1ToLeftArm2);
    LeftArm1ToLeftArm2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArmVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftArm2ToVenaCava1 = cCardiovascular.CreatePath(LeftArm2, VenaCava1, pulse::CardiovascularPath::LeftArm2ToVenaCava1);

    SEFluidCircuitPath& Aorta1ToLeftKidney1 = cCardiovascular.CreatePath(Aorta1, LeftKidney1, pulse::CardiovascularPath::Aorta1ToLeftKidney1);
    Aorta1ToLeftKidney1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceKidney, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftKidney1ToGround = cCardiovascular.CreatePath(LeftKidney1, Ground, pulse::CardiovascularPath::LeftKidney1ToGround);
    LeftKidney1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& LeftKidney1ToLeftKidney2 = cCardiovascular.CreatePath(LeftKidney1, LeftKidney2, pulse::CardiovascularPath::LeftKidney1ToLeftKidney2);
    LeftKidney1ToLeftKidney2.GetResistanceBaseline().SetValue(ResistanceKidneyVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftKidney2ToVenaCava1 = cCardiovascular.CreatePath(LeftKidney2, VenaCava1, pulse::CardiovascularPath::LeftKidney2ToVenaCava1);

    SEFluidCircuitPath& Aorta1ToLeftLeg1 = cCardiovascular.CreatePath(Aorta1, LeftLeg1, pulse::CardiovascularPath::Aorta1ToLeftLeg1);
    Aorta1ToLeftLeg1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLeg, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftLeg1ToGround = cCardiovascular.CreatePath(LeftLeg1, Ground, pulse::CardiovascularPath::LeftLeg1ToGround);
    LeftLeg1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& LeftLeg1ToLeftLeg2 = cCardiovascular.CreatePath(LeftLeg1, LeftLeg2, pulse::CardiovascularPath::LeftLeg1ToLeftLeg2);
    LeftLeg1ToLeftLeg2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLegVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& LeftLeg2ToVenaCava1 = cCardiovascular.CreatePath(LeftLeg2, VenaCava1, pulse::CardiovascularPath::LeftLeg2ToVenaCava1);

    SEFluidCircuitPath& Aorta1ToLiver1 = cCardiovascular.CreatePath(Aorta1, Liver1, pulse::CardiovascularPath::Aorta1ToLiver1);
    Aorta1ToLiver1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLiver, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Liver1ToGround = cCardiovascular.CreatePath(Liver1, Ground, pulse::CardiovascularPath::Liver1ToGround);
    Liver1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& PortalVein1ToLiver1 = cCardiovascular.CreatePath(PortalVein1, Liver1, pulse::CardiovascularPath::PortalVein1ToLiver1);
    PortalVein1ToLiver1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistancePortalVein, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Liver1ToLiver2 = cCardiovascular.CreatePath(Liver1, Liver2, pulse::CardiovascularPath::Liver1ToLiver2);
    Liver1ToLiver2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLiverVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Liver2ToVenaCava1 = cCardiovascular.CreatePath(Liver2, VenaCava1, pulse::CardiovascularPath::Liver2ToVenaCava1);

    SEFluidCircuitPath& Aorta1ToMuscle1 = cCardiovascular.CreatePath(Aorta1, Muscle1, pulse::CardiovascularPath::Aorta1ToMuscle1);
    Aorta1ToMuscle1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceMuscle, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Muscle1ToGround = cCardiovascular.CreatePath(Muscle1, Ground, pulse::CardiovascularPath::Muscle1ToGround);
    Muscle1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& Muscle1ToMuscle2 = cCardiovascular.CreatePath(Muscle1, Muscle2, pulse::CardiovascularPath::Muscle1ToMuscle2);
    Muscle1ToMuscle2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceMuscleVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Muscle2ToVenaCava1 = cCardiovascular.CreatePath(Muscle2, VenaCava1, pulse::CardiovascularPath::Muscle2ToVenaCava1);

    SEFluidCircuitPath& Aorta1ToMyocardium1 = cCardiovascular.CreatePath(Aorta1, Myocardium1, pulse::CardiovascularPath::Aorta1ToMyocardium1);
    Aorta1ToMyocardium1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceMyocardium, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Myocardium1ToGround = cCardiovascular.CreatePath(Myocardium1, Ground, pulse::CardiovascularPath::Myocardium1ToGround);
    Myocardium1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& Myocardium1ToMyocardium2 = cCardiovascular.CreatePath(Myocardium1, Myocardium2, pulse::CardiovascularPath::Myocardium1ToMyocardium2);
    Myocardium1ToMyocardium2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceMyocardiumVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Myocardium2ToVenaCava1 = cCardiovascular.CreatePath(Myocardium2, VenaCava1, pulse::CardiovascularPath::Myocardium2ToVenaCava1);

    SEFluidCircuitPath& PericardiumToGround = cCardiovascular.CreatePath(Pericardium, Ground, pulse::CardiovascularPath::Pericardium1ToGround);
    SEFluidCircuitPath& GroundToPericardium = cCardiovascular.CreatePath(Ground, Pericardium, pulse::CardiovascularPath::GroundToPericardium1);
    GroundToPericardium.GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::mL_Per_s);

    SEFluidCircuitPath& Aorta1ToRightArm1 = cCardiovascular.CreatePath(Aorta1, RightArm1, pulse::CardiovascularPath::Aorta1ToRightArm1);
    Aorta1ToRightArm1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArm, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightArm1ToGround = cCardiovascular.CreatePath(RightArm1, Ground, pulse::CardiovascularPath::RightArm1ToGround);
    RightArm1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& RightArm1ToRightArm2 = cCardiovascular.CreatePath(RightArm1, RightArm2, pulse::CardiovascularPath::RightArm1ToRightArm2);
    RightArm1ToRightArm2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceArmVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightArm2ToVenaCava1 = cCardiovascular.CreatePath(RightArm2, VenaCava1, pulse::CardiovascularPath::RightArm2ToVenaCava1);

    SEFluidCircuitPath& Aorta1ToRightKidney1 = cCardiovascular.CreatePath(Aorta1, RightKidney1, pulse::CardiovascularPath::Aorta1ToRightKidney1);
    Aorta1ToRightKidney1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceKidney, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightKidney1ToGround = cCardiovascular.CreatePath(RightKidney1, Ground, pulse::CardiovascularPath::RightKidney1ToGround);
    RightKidney1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& RightKidney1ToRightKidney2 = cCardiovascular.CreatePath(RightKidney1, RightKidney2, pulse::CardiovascularPath::RightKidney1ToRightKidney2);
    RightKidney1ToRightKidney2.GetResistanceBaseline().SetValue(ResistanceKidneyVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightKidney2ToVenaCava1 = cCardiovascular.CreatePath(RightKidney2, VenaCava1, pulse::CardiovascularPath::RightKidney2ToVenaCava1);

    SEFluidCircuitPath& Aorta1ToRightLeg1 = cCardiovascular.CreatePath(Aorta1, RightLeg1, pulse::CardiovascularPath::Aorta1ToRightLeg1);
    Aorta1ToRightLeg1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLeg, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightLeg1ToGround = cCardiovascular.CreatePath(RightLeg1, Ground, pulse::CardiovascularPath::RightLeg1ToGround);
    RightLeg1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& RightLeg1ToRightLeg2 = cCardiovascular.CreatePath(RightLeg1, RightLeg2, pulse::CardiovascularPath::RightLeg1ToRightLeg2);
    RightLeg1ToRightLeg2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceLegVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& RightLeg2ToVenaCava1 = cCardiovascular.CreatePath(RightLeg2, VenaCava1, pulse::CardiovascularPath::RightLeg2ToVenaCava1);

    SEFluidCircuitPath& Aorta1ToSkin1 = cCardiovascular.CreatePath(Aorta1, Skin1, pulse::CardiovascularPath::Aorta1ToSkin1);
    Aorta1ToSkin1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceSkin, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Skin1ToGround = cCardiovascular.CreatePath(Skin1, Ground, pulse::CardiovascularPath::Skin1ToGround);
    Skin1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& Skin1ToSkin2 = cCardiovascular.CreatePath(Skin1, Skin2, pulse::CardiovascularPath::Skin1ToSkin2);
    Skin1ToSkin2.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceSkinVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Skin2ToVenaCava1 = cCardiovascular.CreatePath(Skin2, VenaCava1, pulse::CardiovascularPath::Skin2ToVenaCava1);

    SEFluidCircuitPath& Aorta1ToSmallIntestine1 = cCardiovascular.CreatePath(Aorta1, SmallIntestine1, pulse::CardiovascularPath::Aorta1ToSmallIntestine1);
    Aorta1ToSmallIntestine1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceSmallIntestine, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& SmallIntestine1ToGround = cCardiovascular.CreatePath(SmallIntestine1, Ground, pulse::CardiovascularPath::SmallIntestine1ToGround);
    SmallIntestine1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& SmallIntestine1ToPortalVein1 = cCardiovascular.CreatePath(SmallIntestine1, PortalVein1, pulse::CardiovascularPath::SmallIntestine1ToPortalVein1);
    SmallIntestine1ToPortalVein1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceSmallIntestineVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);

    SEFluidCircuitPath& Aorta1ToSplanchnic1 = cCardiovascular.CreatePath(Aorta1, Splanchnic1, pulse::CardiovascularPath::Aorta1ToSplanchnic1);
    Aorta1ToSplanchnic1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceSplanchnic, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Splanchnic1ToGround = cCardiovascular.CreatePath(Splanchnic1, Ground, pulse::CardiovascularPath::Splanchnic1ToGround);
    Splanchnic1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& SplanchnicToPortalVein = cCardiovascular.CreatePath(Splanchnic1, PortalVein1, pulse::CardiovascularPath::Splanchnic1ToPortalVein1);
    SplanchnicToPortalVein.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceSplanchnicVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);

    SEFluidCircuitPath& Aorta1ToSpleen1 = cCardiovascular.CreatePath(Aorta1, Spleen1, pulse::CardiovascularPath::Aorta1ToSpleen1);
    Aorta1ToSpleen1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceSpleen, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    SEFluidCircuitPath& Spleen1ToGround = cCardiovascular.CreatePath(Spleen1, Ground, pulse::CardiovascularPath::Spleen1ToGround);
    Spleen1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& Spleen1ToPortalVein1 = cCardiovascular.CreatePath(Spleen1, PortalVein1, pulse::CardiovascularPath::Spleen1ToPortalVein1);
    Spleen1ToPortalVein1.GetResistanceBaseline().SetValue(systemicResistanceModifier * ResistanceSpleenVenous, PressureTimePerVolumeUnit::mmHg_s_Per_mL);

    SEFluidCircuitPath& AbdominalCavityToGround = cCardiovascular.CreatePath(AbdominalCavity, Ground, pulse::CardiovascularPath::AbdominalCavity1ToGround);
    SEFluidCircuitPath& GroundToAbdominalCavity = cCardiovascular.CreatePath(Ground, AbdominalCavity, pulse::CardiovascularPath::GroundToAbdominalCavity1);
    GroundToAbdominalCavity.GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::mL_Per_s);

    SEFluidCircuitPath& GroundToAorta4 = cCardiovascular.CreatePath(Ground, Aorta4, pulse::CardiovascularPath::GroundToAorta4);
    GroundToAorta4.GetPressureSourceBaseline().SetValue(0.0, PressureUnit::mmHg);

    SEFluidCircuitPath& VenaCava1ToGround = cCardiovascular.CreatePath(VenaCava1, Ground, pulse::CardiovascularPath::VenaCava1ToGround);
    VenaCava1ToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    SEFluidCircuitPath& IVToVenaCava1 = cCardiovascular.CreatePath(Ground, VenaCava1, pulse::CardiovascularPath::IVToVenaCava1);
    IVToVenaCava1.GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::mL_Per_s);

    SEFluidCircuitPath& RightPulmonaryArteries1ToRightSuperiorLobeApicalPulmonaryCapillaries = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightSuperiorLobeApicalPulmonaryCapillaries, pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightSuperiorLobeApicalPulmonaryCapillaries);
    SEFluidCircuitPath& RightPulmonaryArteries1ToRightSuperiorLobePosteriorPulmonaryCapillaries = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightSuperiorLobePosteriorPulmonaryCapillaries, pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightSuperiorLobePosteriorPulmonaryCapillaries);
    SEFluidCircuitPath& RightPulmonaryArteries1ToRightSuperiorLobeAnteriorPulmonaryCapillaries = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightSuperiorLobeAnteriorPulmonaryCapillaries, pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightSuperiorLobeAnteriorPulmonaryCapillaries);
    SEFluidCircuitPath& RightPulmonaryArteries1ToRightMiddleLobeLateralPulmonaryCapillaries = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightMiddleLobeLateralPulmonaryCapillaries, pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightMiddleLobeLateralPulmonaryCapillaries);
    SEFluidCircuitPath& RightPulmonaryArteries1ToRightMiddleLobeMedialPulmonaryCapillaries = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightMiddleLobeMedialPulmonaryCapillaries, pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightMiddleLobeMedialPulmonaryCapillaries);
    SEFluidCircuitPath& RightPulmonaryArteries1ToRightInferiorLobeSuperiorPulmonaryCapillaries = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightInferiorLobeSuperiorPulmonaryCapillaries, pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightInferiorLobeSuperiorPulmonaryCapillaries);
    SEFluidCircuitPath& RightPulmonaryArteries1ToRightInferiorLobeMedialBasalPulmonaryCapillaries = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightInferiorLobeMedialBasalPulmonaryCapillaries, pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightInferiorLobeMedialBasalPulmonaryCapillaries);
    SEFluidCircuitPath& RightPulmonaryArteries1ToRightInferiorLobeAnteriorBasalPulmonaryCapillaries = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightInferiorLobeAnteriorBasalPulmonaryCapillaries, pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightInferiorLobeAnteriorBasalPulmonaryCapillaries);
    SEFluidCircuitPath& RightPulmonaryArteries1ToRightInferiorLobeLateralBasalPulmonaryCapillaries = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightInferiorLobeLateralBasalPulmonaryCapillaries, pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightInferiorLobeLateralBasalPulmonaryCapillaries);
    SEFluidCircuitPath& RightPulmonaryArteries1ToRightInferiorLobePosteriorBasalPulmonaryCapillaries = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightInferiorLobePosteriorBasalPulmonaryCapillaries, pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightInferiorLobePosteriorBasalPulmonaryCapillaries);

    SEFluidCircuitPath& LeftPulmonaryArteries1ToLeftInferiorLobePosteriorBasalPulmonaryCapillaries = cCardiovascular.CreatePath(LeftPulmonaryArteries1, LeftInferiorLobePosteriorBasalPulmonaryCapillaries, pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftInferiorLobePosteriorBasalPulmonaryCapillaries);
    SEFluidCircuitPath& LeftPulmonaryArteries1ToLeftInferiorLobeLateralBasalPulmonaryCapillaries = cCardiovascular.CreatePath(LeftPulmonaryArteries1, LeftInferiorLobeLateralBasalPulmonaryCapillaries, pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftInferiorLobeLateralBasalPulmonaryCapillaries);
    SEFluidCircuitPath& LeftPulmonaryArteries1ToLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries = cCardiovascular.CreatePath(LeftPulmonaryArteries1, LeftInferiorLobeAnteromedialBasalPulmonaryCapillaries, pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries);
    SEFluidCircuitPath& LeftPulmonaryArteries1ToLeftInferiorLobeSuperiorPulmonaryCapillaries = cCardiovascular.CreatePath(LeftPulmonaryArteries1, LeftInferiorLobeSuperiorPulmonaryCapillaries, pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftInferiorLobeSuperiorPulmonaryCapillaries);
    SEFluidCircuitPath& LeftPulmonaryArteries1ToLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries = cCardiovascular.CreatePath(LeftPulmonaryArteries1, LeftSuperiorLobeInferiorLingulaPulmonaryCapillaries, pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries);
    SEFluidCircuitPath& LeftPulmonaryArteries1ToLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries = cCardiovascular.CreatePath(LeftPulmonaryArteries1, LeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries, pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries);
    SEFluidCircuitPath& LeftPulmonaryArteries1ToLeftSuperiorLobeAnteriorPulmonaryCapillaries = cCardiovascular.CreatePath(LeftPulmonaryArteries1, LeftSuperiorLobeAnteriorPulmonaryCapillaries, pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftSuperiorLobeAnteriorPulmonaryCapillaries);
    SEFluidCircuitPath& LeftPulmonaryArteries1ToLeftSuperiorLobeApicoposteriorPulmonaryCapillaries = cCardiovascular.CreatePath(LeftPulmonaryArteries1, LeftSuperiorLobeApicoposteriorPulmonaryCapillaries, pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftSuperiorLobeApicoposteriorPulmonaryCapillaries);

    SEFluidCircuitPath& RightSuperiorLobeApicalPulmonaryCapillariesToRightPulmonaryVeins1 = cCardiovascular.CreatePath(RightSuperiorLobeApicalPulmonaryCapillaries, RightPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::RightSuperiorLobeApicalPulmonaryCapillariesToRightPulmonaryVeins1);
    SEFluidCircuitPath& RightSuperiorLobePosteriorPulmonaryCapillariesToRightPulmonaryVeins1 = cCardiovascular.CreatePath(RightSuperiorLobePosteriorPulmonaryCapillaries, RightPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::RightSuperiorLobePosteriorPulmonaryCapillariesToRightPulmonaryVeins1);
    SEFluidCircuitPath& RightSuperiorLobeAnteriorPulmonaryCapillariesToRightPulmonaryVeins1 = cCardiovascular.CreatePath(RightSuperiorLobeAnteriorPulmonaryCapillaries, RightPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::RightSuperiorLobeAnteriorPulmonaryCapillariesToRightPulmonaryVeins1);
    SEFluidCircuitPath& RightMiddleLobeLateralPulmonaryCapillariesToRightPulmonaryVeins1 = cCardiovascular.CreatePath(RightMiddleLobeLateralPulmonaryCapillaries, RightPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::RightMiddleLobeLateralPulmonaryCapillariesToRightPulmonaryVeins1);
    SEFluidCircuitPath& RightMiddleLobeMedialPulmonaryCapillariesToRightPulmonaryVeins1 = cCardiovascular.CreatePath(RightMiddleLobeMedialPulmonaryCapillaries, RightPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::RightMiddleLobeMedialPulmonaryCapillariesToRightPulmonaryVeins1);
    SEFluidCircuitPath& RightInferiorLobeSuperiorPulmonaryCapillariesToRightPulmonaryVeins1 = cCardiovascular.CreatePath(RightInferiorLobeSuperiorPulmonaryCapillaries, RightPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeSuperiorPulmonaryCapillariesToRightPulmonaryVeins1);
    SEFluidCircuitPath& RightInferiorLobeMedialBasalPulmonaryCapillariesToRightPulmonaryVeins1 = cCardiovascular.CreatePath(RightInferiorLobeMedialBasalPulmonaryCapillaries, RightPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeMedialBasalPulmonaryCapillariesToRightPulmonaryVeins1);
    SEFluidCircuitPath& RightInferiorLobeAnteriorBasalPulmonaryCapillariesToRightPulmonaryVeins1 = cCardiovascular.CreatePath(RightInferiorLobeAnteriorBasalPulmonaryCapillaries, RightPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeAnteriorBasalPulmonaryCapillariesToRightPulmonaryVeins1);
    SEFluidCircuitPath& RightInferiorLobeLateralBasalPulmonaryCapillariesToRightPulmonaryVeins1 = cCardiovascular.CreatePath(RightInferiorLobeLateralBasalPulmonaryCapillaries, RightPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeLateralBasalPulmonaryCapillariesToRightPulmonaryVeins1);
    SEFluidCircuitPath& RightInferiorLobePosteriorBasalPulmonaryCapillariesToRightPulmonaryVeins1 = cCardiovascular.CreatePath(RightInferiorLobePosteriorBasalPulmonaryCapillaries, RightPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::RightInferiorLobePosteriorBasalPulmonaryCapillariesToRightPulmonaryVeins1);

    SEFluidCircuitPath& LeftInferiorLobePosteriorBasalPulmonaryCapillariesToLeftPulmonaryVeins1 = cCardiovascular.CreatePath(LeftInferiorLobePosteriorBasalPulmonaryCapillaries, LeftPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobePosteriorBasalPulmonaryCapillariesToLeftPulmonaryVeins1);
    SEFluidCircuitPath& LeftInferiorLobeLateralBasalPulmonaryCapillariesToLeftPulmonaryVeins1 = cCardiovascular.CreatePath(LeftInferiorLobeLateralBasalPulmonaryCapillaries, LeftPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobeLateralBasalPulmonaryCapillariesToLeftPulmonaryVeins1);
    SEFluidCircuitPath& LeftInferiorLobeAnteromedialBasalPulmonaryCapillariesToLeftPulmonaryVeins1 = cCardiovascular.CreatePath(LeftInferiorLobeAnteromedialBasalPulmonaryCapillaries, LeftPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobeAnteromedialBasalPulmonaryCapillariesToLeftPulmonaryVeins1);
    SEFluidCircuitPath& LeftInferiorLobeSuperiorPulmonaryCapillariesToLeftPulmonaryVeins1 = cCardiovascular.CreatePath(LeftInferiorLobeSuperiorPulmonaryCapillaries, LeftPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobeSuperiorPulmonaryCapillariesToLeftPulmonaryVeins1);
    SEFluidCircuitPath& LeftSuperiorLobeInferiorLingulaPulmonaryCapillariesToLeftPulmonaryVeins1 = cCardiovascular.CreatePath(LeftSuperiorLobeInferiorLingulaPulmonaryCapillaries, LeftPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeInferiorLingulaPulmonaryCapillariesToLeftPulmonaryVeins1);
    SEFluidCircuitPath& LeftSuperiorLobeSuperiorLingulaPulmonaryCapillariesToLeftPulmonaryVeins1 = cCardiovascular.CreatePath(LeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries, LeftPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeSuperiorLingulaPulmonaryCapillariesToLeftPulmonaryVeins1);
    SEFluidCircuitPath& LeftSuperiorLobeAnteriorPulmonaryCapillariesToLeftPulmonaryVeins1 = cCardiovascular.CreatePath(LeftSuperiorLobeAnteriorPulmonaryCapillaries, LeftPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeAnteriorPulmonaryCapillariesToLeftPulmonaryVeins1);
    SEFluidCircuitPath& LeftSuperiorLobeApicoposteriorPulmonaryCapillariesToLeftPulmonaryVeins1 = cCardiovascular.CreatePath(LeftSuperiorLobeApicoposteriorPulmonaryCapillaries, LeftPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeApicoposteriorPulmonaryCapillariesToLeftPulmonaryVeins1);

    SEFluidCircuitPath& RightSuperiorLobeApicalPulmonaryCapillariesToGround = cCardiovascular.CreatePath(RightSuperiorLobeApicalPulmonaryCapillaries, Ground, pulse::ExpandedLungsCardiovascularPath::RightSuperiorLobeApicalPulmonaryCapillariesToGround);
    SEFluidCircuitPath& RightSuperiorLobePosteriorPulmonaryCapillariesToGround = cCardiovascular.CreatePath(RightSuperiorLobePosteriorPulmonaryCapillaries, Ground, pulse::ExpandedLungsCardiovascularPath::RightSuperiorLobePosteriorPulmonaryCapillariesToGround);
    SEFluidCircuitPath& RightSuperiorLobeAnteriorPulmonaryCapillariesToGround = cCardiovascular.CreatePath(RightSuperiorLobeAnteriorPulmonaryCapillaries, Ground, pulse::ExpandedLungsCardiovascularPath::RightSuperiorLobeAnteriorPulmonaryCapillariesToGround);
    SEFluidCircuitPath& RightMiddleLobeLateralPulmonaryCapillariesToGround = cCardiovascular.CreatePath(RightMiddleLobeLateralPulmonaryCapillaries, Ground, pulse::ExpandedLungsCardiovascularPath::RightMiddleLobeLateralPulmonaryCapillariesToGround);
    SEFluidCircuitPath& RightMiddleLobeMedialPulmonaryCapillariesToGround = cCardiovascular.CreatePath(RightMiddleLobeMedialPulmonaryCapillaries, Ground, pulse::ExpandedLungsCardiovascularPath::RightMiddleLobeMedialPulmonaryCapillariesToGround);
    SEFluidCircuitPath& RightInferiorLobeSuperiorPulmonaryCapillariesToGround = cCardiovascular.CreatePath(RightInferiorLobeSuperiorPulmonaryCapillaries, Ground, pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeSuperiorPulmonaryCapillariesToGround);
    SEFluidCircuitPath& RightInferiorLobeMedialBasalPulmonaryCapillariesToGround = cCardiovascular.CreatePath(RightInferiorLobeMedialBasalPulmonaryCapillaries, Ground, pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeMedialBasalPulmonaryCapillariesToGround);
    SEFluidCircuitPath& RightInferiorLobeAnteriorBasalPulmonaryCapillariesToGround = cCardiovascular.CreatePath(RightInferiorLobeAnteriorBasalPulmonaryCapillaries, Ground, pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeAnteriorBasalPulmonaryCapillariesToGround);
    SEFluidCircuitPath& RightInferiorLobeLateralBasalPulmonaryCapillariesToGround = cCardiovascular.CreatePath(RightInferiorLobeLateralBasalPulmonaryCapillaries, Ground, pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeLateralBasalPulmonaryCapillariesToGround);
    SEFluidCircuitPath& RightInferiorLobePosteriorBasalPulmonaryCapillariesToGround = cCardiovascular.CreatePath(RightInferiorLobePosteriorBasalPulmonaryCapillaries, Ground, pulse::ExpandedLungsCardiovascularPath::RightInferiorLobePosteriorBasalPulmonaryCapillariesToGround);

    SEFluidCircuitPath& LeftInferiorLobePosteriorBasalPulmonaryCapillariesToGround = cCardiovascular.CreatePath(LeftInferiorLobePosteriorBasalPulmonaryCapillaries, Ground, pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobePosteriorBasalPulmonaryCapillariesToGround);
    SEFluidCircuitPath& LeftInferiorLobeLateralBasalPulmonaryCapillariesToGround = cCardiovascular.CreatePath(LeftInferiorLobeLateralBasalPulmonaryCapillaries, Ground, pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobeLateralBasalPulmonaryCapillariesToGround);
    SEFluidCircuitPath& LeftInferiorLobeAnteromedialBasalPulmonaryCapillariesToGround = cCardiovascular.CreatePath(LeftInferiorLobeAnteromedialBasalPulmonaryCapillaries, Ground, pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobeAnteromedialBasalPulmonaryCapillariesToGround);
    SEFluidCircuitPath& LeftInferiorLobeSuperiorPulmonaryCapillariesToGround = cCardiovascular.CreatePath(LeftInferiorLobeSuperiorPulmonaryCapillaries, Ground, pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobeSuperiorPulmonaryCapillariesToGround);
    SEFluidCircuitPath& LeftSuperiorLobeInferiorLingulaPulmonaryCapillariesToGround = cCardiovascular.CreatePath(LeftSuperiorLobeInferiorLingulaPulmonaryCapillaries, Ground, pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeInferiorLingulaPulmonaryCapillariesToGround);
    SEFluidCircuitPath& LeftSuperiorLobeSuperiorLingulaPulmonaryCapillariesToGround = cCardiovascular.CreatePath(LeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries, Ground, pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeSuperiorLingulaPulmonaryCapillariesToGround);
    SEFluidCircuitPath& LeftSuperiorLobeAnteriorPulmonaryCapillariesToGround = cCardiovascular.CreatePath(LeftSuperiorLobeAnteriorPulmonaryCapillaries, Ground, pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeAnteriorPulmonaryCapillariesToGround);
    SEFluidCircuitPath& LeftSuperiorLobeApicoposteriorPulmonaryCapillariesToGround = cCardiovascular.CreatePath(LeftSuperiorLobeApicoposteriorPulmonaryCapillaries, Ground, pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeApicoposteriorPulmonaryCapillariesToGround);

    RightPulmonaryArteries1ToRightSuperiorLobeApicalPulmonaryCapillaries.GetResistanceBaseline().SetValue(ResistanceRightSuperiorLobeApicalPulmonaryCapillaries, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightPulmonaryArteries1ToRightSuperiorLobePosteriorPulmonaryCapillaries.GetResistanceBaseline().SetValue(ResistanceRightSuperiorLobePosteriorPulmonaryCapillaries, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightPulmonaryArteries1ToRightSuperiorLobeAnteriorPulmonaryCapillaries.GetResistanceBaseline().SetValue(ResistanceRightSuperiorLobeAnteriorPulmonaryCapillaries, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightPulmonaryArteries1ToRightMiddleLobeLateralPulmonaryCapillaries.GetResistanceBaseline().SetValue(ResistanceRightMiddleLobeLateralPulmonaryCapillaries, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightPulmonaryArteries1ToRightMiddleLobeMedialPulmonaryCapillaries.GetResistanceBaseline().SetValue(ResistanceRightMiddleLobeMedialPulmonaryCapillaries, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightPulmonaryArteries1ToRightInferiorLobeSuperiorPulmonaryCapillaries.GetResistanceBaseline().SetValue(ResistanceRightInferiorLobeSuperiorPulmonaryCapillaries, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightPulmonaryArteries1ToRightInferiorLobeMedialBasalPulmonaryCapillaries.GetResistanceBaseline().SetValue(ResistanceRightInferiorLobeMedialBasalPulmonaryCapillaries, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightPulmonaryArteries1ToRightInferiorLobeAnteriorBasalPulmonaryCapillaries.GetResistanceBaseline().SetValue(ResistanceRightInferiorLobeAnteriorBasalPulmonaryCapillaries, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightPulmonaryArteries1ToRightInferiorLobeLateralBasalPulmonaryCapillaries.GetResistanceBaseline().SetValue(ResistanceRightInferiorLobeLateralBasalPulmonaryCapillaries, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightPulmonaryArteries1ToRightInferiorLobePosteriorBasalPulmonaryCapillaries.GetResistanceBaseline().SetValue(ResistanceRightInferiorLobePosteriorBasalPulmonaryCapillaries, PressureTimePerVolumeUnit::mmHg_s_Per_mL);

    LeftPulmonaryArteries1ToLeftInferiorLobePosteriorBasalPulmonaryCapillaries.GetResistanceBaseline().SetValue(ResistanceLeftInferiorLobePosteriorBasalPulmonaryCapillaries, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    LeftPulmonaryArteries1ToLeftInferiorLobeLateralBasalPulmonaryCapillaries.GetResistanceBaseline().SetValue(ResistanceLeftInferiorLobeLateralBasalPulmonaryCapillaries, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    LeftPulmonaryArteries1ToLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries.GetResistanceBaseline().SetValue(ResistanceLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    LeftPulmonaryArteries1ToLeftInferiorLobeSuperiorPulmonaryCapillaries.GetResistanceBaseline().SetValue(ResistanceLeftInferiorLobeSuperiorPulmonaryCapillaries, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    LeftPulmonaryArteries1ToLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries.GetResistanceBaseline().SetValue(ResistanceLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    LeftPulmonaryArteries1ToLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries.GetResistanceBaseline().SetValue(ResistanceLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    LeftPulmonaryArteries1ToLeftSuperiorLobeAnteriorPulmonaryCapillaries.GetResistanceBaseline().SetValue(ResistanceLeftSuperiorLobeAnteriorPulmonaryCapillaries, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    LeftPulmonaryArteries1ToLeftSuperiorLobeApicoposteriorPulmonaryCapillaries.GetResistanceBaseline().SetValue(ResistanceLeftSuperiorLobeApicoposteriorPulmonaryCapillaries, PressureTimePerVolumeUnit::mmHg_s_Per_mL);

    RightSuperiorLobeApicalPulmonaryCapillariesToRightPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceRightSuperiorLobeApicalPulmonaryVeins, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightSuperiorLobePosteriorPulmonaryCapillariesToRightPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceRightSuperiorLobePosteriorPulmonaryVeins, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightSuperiorLobeAnteriorPulmonaryCapillariesToRightPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceRightSuperiorLobeAnteriorPulmonaryVeins, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightMiddleLobeLateralPulmonaryCapillariesToRightPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceRightMiddleLobeLateralPulmonaryVeins, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightMiddleLobeMedialPulmonaryCapillariesToRightPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceRightMiddleLobeMedialPulmonaryVeins, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightInferiorLobeSuperiorPulmonaryCapillariesToRightPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceRightInferiorLobeSuperiorPulmonaryVeins, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightInferiorLobeMedialBasalPulmonaryCapillariesToRightPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceRightInferiorLobeMedialBasalPulmonaryVeins, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightInferiorLobeAnteriorBasalPulmonaryCapillariesToRightPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceRightInferiorLobeAnteriorBasalPulmonaryVeins, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightInferiorLobeLateralBasalPulmonaryCapillariesToRightPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceRightInferiorLobeLateralBasalPulmonaryVeins, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightInferiorLobePosteriorBasalPulmonaryCapillariesToRightPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceRightInferiorLobePosteriorBasalPulmonaryVeins, PressureTimePerVolumeUnit::mmHg_s_Per_mL);

    LeftInferiorLobePosteriorBasalPulmonaryCapillariesToLeftPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceLeftInferiorLobePosteriorBasalPulmonaryVeins, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    LeftInferiorLobeLateralBasalPulmonaryCapillariesToLeftPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceLeftInferiorLobeLateralBasalPulmonaryVeins, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    LeftInferiorLobeAnteromedialBasalPulmonaryCapillariesToLeftPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceLeftInferiorLobeAnteromedialBasalPulmonaryVeins, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    LeftInferiorLobeSuperiorPulmonaryCapillariesToLeftPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceLeftInferiorLobeSuperiorPulmonaryVeins, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    LeftSuperiorLobeInferiorLingulaPulmonaryCapillariesToLeftPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceLeftSuperiorLobeInferiorLingulaPulmonaryVeins, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    LeftSuperiorLobeSuperiorLingulaPulmonaryCapillariesToLeftPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceLeftSuperiorLobeSuperiorLingulaPulmonaryVeins, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    LeftSuperiorLobeAnteriorPulmonaryCapillariesToLeftPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceLeftSuperiorLobeAnteriorPulmonaryVeins, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    LeftSuperiorLobeApicoposteriorPulmonaryCapillariesToLeftPulmonaryVeins1.GetResistanceBaseline().SetValue(ResistanceLeftSuperiorLobeApicoposteriorPulmonaryVeins, PressureTimePerVolumeUnit::mmHg_s_Per_mL);

    RightSuperiorLobeApicalPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    RightSuperiorLobePosteriorPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    RightSuperiorLobeAnteriorPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    RightMiddleLobeLateralPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    RightMiddleLobeMedialPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    RightInferiorLobeSuperiorPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    RightInferiorLobeMedialBasalPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    RightInferiorLobeAnteriorBasalPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    RightInferiorLobeLateralBasalPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    RightInferiorLobePosteriorBasalPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);

    LeftInferiorLobePosteriorBasalPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    LeftInferiorLobeLateralBasalPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    LeftInferiorLobeAnteromedialBasalPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    LeftInferiorLobeSuperiorPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    LeftSuperiorLobeInferiorLingulaPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    LeftSuperiorLobeSuperiorLingulaPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    LeftSuperiorLobeAnteriorPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);
    LeftSuperiorLobeApicoposteriorPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(0.0, VolumePerPressureUnit::mL_Per_mmHg);

    //Shunts
    SEFluidCircuitPath& RightSuperiorLobeApicalShunt = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::RightSuperiorLobeApicalShunt);
    SEFluidCircuitPath& RightSuperiorLobePosteriorShunt = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::RightSuperiorLobePosteriorShunt);
    SEFluidCircuitPath& RightSuperiorLobeAnteriorShunt = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::RightSuperiorLobeAnteriorShunt);
    SEFluidCircuitPath& RightMiddleLobeLateralShunt = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::RightMiddleLobeLateralShunt);
    SEFluidCircuitPath& RightMiddleLobeMedialShunt = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::RightMiddleLobeMedialShunt);
    SEFluidCircuitPath& RightInferiorLobeSuperiorShunt = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeSuperiorShunt);
    SEFluidCircuitPath& RightInferiorLobeMedialBasalShunt = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeMedialBasalShunt);
    SEFluidCircuitPath& RightInferiorLobeAnteriorBasalShunt = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeAnteriorBasalShunt);
    SEFluidCircuitPath& RightInferiorLobeLateralBasalShunt = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeLateralBasalShunt);
    SEFluidCircuitPath& RightInferiorLobePosteriorBasalShunt = cCardiovascular.CreatePath(RightPulmonaryArteries1, RightPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::RightInferiorLobePosteriorBasalShunt);

    SEFluidCircuitPath& LeftInferiorLobePosteriorBasalShunt = cCardiovascular.CreatePath(LeftPulmonaryArteries1, LeftPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobePosteriorBasalShunt);
    SEFluidCircuitPath& LeftInferiorLobeLateralBasalShunt = cCardiovascular.CreatePath(LeftPulmonaryArteries1, LeftPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobeLateralBasalShunt);
    SEFluidCircuitPath& LeftInferiorLobeAnteromedialBasalShunt = cCardiovascular.CreatePath(LeftPulmonaryArteries1, LeftPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobeAnteromedialBasalShunt);
    SEFluidCircuitPath& LeftInferiorLobeSuperiorShunt = cCardiovascular.CreatePath(LeftPulmonaryArteries1, LeftPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobeSuperiorShunt);
    SEFluidCircuitPath& LeftSuperiorLobeInferiorLingulaShunt = cCardiovascular.CreatePath(LeftPulmonaryArteries1, LeftPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeInferiorLingulaShunt);
    SEFluidCircuitPath& LeftSuperiorLobeSuperiorLingulaShunt = cCardiovascular.CreatePath(LeftPulmonaryArteries1, LeftPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeSuperiorLingulaShunt);
    SEFluidCircuitPath& LeftSuperiorLobeAnteriorShunt = cCardiovascular.CreatePath(LeftPulmonaryArteries1, LeftPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeAnteriorShunt);
    SEFluidCircuitPath& LeftSuperiorLobeApicoposteriorShunt = cCardiovascular.CreatePath(LeftPulmonaryArteries1, LeftPulmonaryVeins1, pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeApicoposteriorShunt);

    RightSuperiorLobeApicalShunt.GetResistanceBaseline().SetValue(10.0 * ResistanceShuntRight, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightSuperiorLobePosteriorShunt.GetResistanceBaseline().SetValue(10.0 * ResistanceShuntRight, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightSuperiorLobeAnteriorShunt.GetResistanceBaseline().SetValue(10.0 * ResistanceShuntRight, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightMiddleLobeLateralShunt.GetResistanceBaseline().SetValue(10.0 * ResistanceShuntRight, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightMiddleLobeMedialShunt.GetResistanceBaseline().SetValue(10.0 * ResistanceShuntRight, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightInferiorLobeSuperiorShunt.GetResistanceBaseline().SetValue(10.0 * ResistanceShuntRight, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightInferiorLobeMedialBasalShunt.GetResistanceBaseline().SetValue(10.0 * ResistanceShuntRight, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightInferiorLobeAnteriorBasalShunt.GetResistanceBaseline().SetValue(10.0 * ResistanceShuntRight, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightInferiorLobeLateralBasalShunt.GetResistanceBaseline().SetValue(10.0 * ResistanceShuntRight, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    RightInferiorLobePosteriorBasalShunt.GetResistanceBaseline().SetValue(10.0 * ResistanceShuntRight, PressureTimePerVolumeUnit::mmHg_s_Per_mL);

    LeftInferiorLobePosteriorBasalShunt.GetResistanceBaseline().SetValue(8.0 * ResistanceShuntLeft, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    LeftInferiorLobeLateralBasalShunt.GetResistanceBaseline().SetValue(8.0 * ResistanceShuntLeft, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    LeftInferiorLobeAnteromedialBasalShunt.GetResistanceBaseline().SetValue(8.0 * ResistanceShuntLeft, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    LeftInferiorLobeSuperiorShunt.GetResistanceBaseline().SetValue(8.0 * ResistanceShuntLeft, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    LeftSuperiorLobeInferiorLingulaShunt.GetResistanceBaseline().SetValue(8.0 * ResistanceShuntLeft, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    LeftSuperiorLobeSuperiorLingulaShunt.GetResistanceBaseline().SetValue(8.0 * ResistanceShuntLeft, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    LeftSuperiorLobeAnteriorShunt.GetResistanceBaseline().SetValue(8.0 * ResistanceShuntLeft, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    LeftSuperiorLobeApicoposteriorShunt.GetResistanceBaseline().SetValue(8.0 * ResistanceShuntLeft, PressureTimePerVolumeUnit::mmHg_s_Per_mL);

    // Compute compliances from target pressures and baseline volumes
    for (SEFluidCircuitPath* p : cCardiovascular.GetPaths())
    {
      if (p->HasComplianceBaseline())
      {
        SEFluidCircuitNode& src = p->GetSourceNode();
        if (!src.HasVolumeBaseline())
          Fatal("Compliance paths must have a volume baseline.");
        double pressure = src.GetPressure(PressureUnit::mmHg);
        double volume = src.GetVolumeBaseline(VolumeUnit::mL);
        p->GetComplianceBaseline().SetValue(volume / pressure, VolumePerPressureUnit::mL_Per_mmHg);
      }
    }

    // Calibrate compliance for expanded components
    RightSuperiorLobeApicalPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(RightSuperiorLobeApicalPulmonaryCapillariesToGround.GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg) * expandedPulmonaryCapillariesComplianceModifier, VolumePerPressureUnit::mL_Per_mmHg);
    RightSuperiorLobePosteriorPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(RightSuperiorLobePosteriorPulmonaryCapillariesToGround.GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg) * expandedPulmonaryCapillariesComplianceModifier, VolumePerPressureUnit::mL_Per_mmHg);
    RightSuperiorLobeAnteriorPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(RightSuperiorLobeAnteriorPulmonaryCapillariesToGround.GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg) * expandedPulmonaryCapillariesComplianceModifier, VolumePerPressureUnit::mL_Per_mmHg);
    RightMiddleLobeLateralPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(RightMiddleLobeLateralPulmonaryCapillariesToGround.GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg) * expandedPulmonaryCapillariesComplianceModifier, VolumePerPressureUnit::mL_Per_mmHg);
    RightMiddleLobeMedialPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(RightMiddleLobeMedialPulmonaryCapillariesToGround.GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg) * expandedPulmonaryCapillariesComplianceModifier, VolumePerPressureUnit::mL_Per_mmHg);
    RightInferiorLobeSuperiorPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(RightInferiorLobeSuperiorPulmonaryCapillariesToGround.GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg) * expandedPulmonaryCapillariesComplianceModifier, VolumePerPressureUnit::mL_Per_mmHg);
    RightInferiorLobeMedialBasalPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(RightInferiorLobeMedialBasalPulmonaryCapillariesToGround.GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg) * expandedPulmonaryCapillariesComplianceModifier, VolumePerPressureUnit::mL_Per_mmHg);
    RightInferiorLobeAnteriorBasalPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(RightInferiorLobeAnteriorBasalPulmonaryCapillariesToGround.GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg) * expandedPulmonaryCapillariesComplianceModifier, VolumePerPressureUnit::mL_Per_mmHg);
    RightInferiorLobeLateralBasalPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(RightInferiorLobeLateralBasalPulmonaryCapillariesToGround.GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg) * expandedPulmonaryCapillariesComplianceModifier, VolumePerPressureUnit::mL_Per_mmHg);
    RightInferiorLobePosteriorBasalPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(RightInferiorLobePosteriorBasalPulmonaryCapillariesToGround.GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg) * expandedPulmonaryCapillariesComplianceModifier, VolumePerPressureUnit::mL_Per_mmHg);

    LeftInferiorLobePosteriorBasalPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(LeftInferiorLobePosteriorBasalPulmonaryCapillariesToGround.GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg) * expandedPulmonaryCapillariesComplianceModifier, VolumePerPressureUnit::mL_Per_mmHg);
    LeftInferiorLobeLateralBasalPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(LeftInferiorLobeLateralBasalPulmonaryCapillariesToGround.GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg) * expandedPulmonaryCapillariesComplianceModifier, VolumePerPressureUnit::mL_Per_mmHg);
    LeftInferiorLobeAnteromedialBasalPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(LeftInferiorLobeAnteromedialBasalPulmonaryCapillariesToGround.GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg) * expandedPulmonaryCapillariesComplianceModifier, VolumePerPressureUnit::mL_Per_mmHg);
    LeftInferiorLobeSuperiorPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(LeftInferiorLobeSuperiorPulmonaryCapillariesToGround.GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg) * expandedPulmonaryCapillariesComplianceModifier, VolumePerPressureUnit::mL_Per_mmHg);
    LeftSuperiorLobeInferiorLingulaPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(LeftSuperiorLobeInferiorLingulaPulmonaryCapillariesToGround.GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg) * expandedPulmonaryCapillariesComplianceModifier, VolumePerPressureUnit::mL_Per_mmHg);
    LeftSuperiorLobeSuperiorLingulaPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(LeftSuperiorLobeSuperiorLingulaPulmonaryCapillariesToGround.GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg) * expandedPulmonaryCapillariesComplianceModifier, VolumePerPressureUnit::mL_Per_mmHg);
    LeftSuperiorLobeAnteriorPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(LeftSuperiorLobeAnteriorPulmonaryCapillariesToGround.GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg) * expandedPulmonaryCapillariesComplianceModifier, VolumePerPressureUnit::mL_Per_mmHg);
    LeftSuperiorLobeApicoposteriorPulmonaryCapillariesToGround.GetComplianceBaseline().SetValue(LeftSuperiorLobeApicoposteriorPulmonaryCapillariesToGround.GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg) * expandedPulmonaryCapillariesComplianceModifier, VolumePerPressureUnit::mL_Per_mmHg);

    // Calibrate volume for expanded components
    RightSuperiorLobeApicalPulmonaryCapillaries.GetVolumeBaseline().SetValue(RightSuperiorLobeApicalPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) * expandedPulmonaryRightCapillariesVolumeModifier, VolumeUnit::mL);
    RightSuperiorLobePosteriorPulmonaryCapillaries.GetVolumeBaseline().SetValue(RightSuperiorLobePosteriorPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) * expandedPulmonaryRightCapillariesVolumeModifier, VolumeUnit::mL);
    RightSuperiorLobeAnteriorPulmonaryCapillaries.GetVolumeBaseline().SetValue(RightSuperiorLobeAnteriorPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) * expandedPulmonaryRightCapillariesVolumeModifier, VolumeUnit::mL);
    RightMiddleLobeLateralPulmonaryCapillaries.GetVolumeBaseline().SetValue(RightMiddleLobeLateralPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) * expandedPulmonaryRightCapillariesVolumeModifier, VolumeUnit::mL);
    RightMiddleLobeMedialPulmonaryCapillaries.GetVolumeBaseline().SetValue(RightMiddleLobeMedialPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) * expandedPulmonaryRightCapillariesVolumeModifier, VolumeUnit::mL);
    RightInferiorLobeSuperiorPulmonaryCapillaries.GetVolumeBaseline().SetValue(RightInferiorLobeSuperiorPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) * expandedPulmonaryRightCapillariesVolumeModifier, VolumeUnit::mL);
    RightInferiorLobeMedialBasalPulmonaryCapillaries.GetVolumeBaseline().SetValue(RightInferiorLobeMedialBasalPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) * expandedPulmonaryRightCapillariesVolumeModifier, VolumeUnit::mL);
    RightInferiorLobeAnteriorBasalPulmonaryCapillaries.GetVolumeBaseline().SetValue(RightInferiorLobeAnteriorBasalPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) * expandedPulmonaryRightCapillariesVolumeModifier, VolumeUnit::mL);
    RightInferiorLobeLateralBasalPulmonaryCapillaries.GetVolumeBaseline().SetValue(RightInferiorLobeLateralBasalPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) * expandedPulmonaryRightCapillariesVolumeModifier, VolumeUnit::mL);
    RightInferiorLobePosteriorBasalPulmonaryCapillaries.GetVolumeBaseline().SetValue(RightInferiorLobePosteriorBasalPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) * expandedPulmonaryRightCapillariesVolumeModifier, VolumeUnit::mL);

    LeftInferiorLobePosteriorBasalPulmonaryCapillaries.GetVolumeBaseline().SetValue(LeftInferiorLobePosteriorBasalPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) * expandedPulmonaryLeftCapillariesVolumeModifier, VolumeUnit::mL);
    LeftInferiorLobeLateralBasalPulmonaryCapillaries.GetVolumeBaseline().SetValue(LeftInferiorLobeLateralBasalPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) * expandedPulmonaryLeftCapillariesVolumeModifier, VolumeUnit::mL);
    LeftInferiorLobeAnteromedialBasalPulmonaryCapillaries.GetVolumeBaseline().SetValue(LeftInferiorLobeAnteromedialBasalPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) * expandedPulmonaryLeftCapillariesVolumeModifier, VolumeUnit::mL);
    LeftInferiorLobeSuperiorPulmonaryCapillaries.GetVolumeBaseline().SetValue(LeftInferiorLobeSuperiorPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) * expandedPulmonaryLeftCapillariesVolumeModifier, VolumeUnit::mL);
    LeftSuperiorLobeInferiorLingulaPulmonaryCapillaries.GetVolumeBaseline().SetValue(LeftSuperiorLobeInferiorLingulaPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) * expandedPulmonaryLeftCapillariesVolumeModifier, VolumeUnit::mL);
    LeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries.GetVolumeBaseline().SetValue(LeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) * expandedPulmonaryLeftCapillariesVolumeModifier, VolumeUnit::mL);
    LeftSuperiorLobeAnteriorPulmonaryCapillaries.GetVolumeBaseline().SetValue(LeftSuperiorLobeAnteriorPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) * expandedPulmonaryLeftCapillariesVolumeModifier, VolumeUnit::mL);
    LeftSuperiorLobeApicoposteriorPulmonaryCapillaries.GetVolumeBaseline().SetValue(LeftSuperiorLobeApicoposteriorPulmonaryCapillaries.GetVolumeBaseline(VolumeUnit::mL) * expandedPulmonaryLeftCapillariesVolumeModifier, VolumeUnit::mL);

    // Hearts and pericardium have special compliance computations
    double InitialComplianceHeartRight = 1.0 / 0.0243;
    double InitialComplianceHeartLeft = 1.0 / 0.049;
    // Volumes are initialized from the volume baselines. The heart volume initialization is a little tricky. To much prime and the 
    // initial pressure wave will be devastating to the rest of the CV system during the first contraction phase. Too little prime 
    // and there will be issues with available flow as the elastance decreases during the first relaxation phase.
    // The 1/4 full initialization gives decent results.
    RightHeart1ToRightHeart3.GetComplianceBaseline().SetValue(InitialComplianceHeartRight, VolumePerPressureUnit::mL_Per_mmHg);
    LeftHeart1ToLeftHeart3.GetComplianceBaseline().SetValue(InitialComplianceHeartLeft, VolumePerPressureUnit::mL_Per_mmHg);
    PericardiumToGround.GetComplianceBaseline().SetValue(100.0, VolumePerPressureUnit::mL_Per_mmHg);

    //Make these polarized, so chest compression can't give a negative volume
    RightHeart1ToRightHeart3.SetNextPolarizedState(eGate::Closed);
    LeftHeart1ToLeftHeart3.SetNextPolarizedState(eGate::Closed);

    double VolumeModifierAorta = 1.16722 * 1.018749, VolumeModifierBrain = 0.998011 * 1.038409, VolumeModifierBone = 1.175574 * 0.985629, VolumeModifierFat = 1.175573 * 0.986527;
    double VolumeModifierLargeIntestine = 1.17528 * 0.985609, VolumeModifierArmL = 1.175573 * 0.986529, VolumeModifierKidneyL = 0.737649 * 0.954339, VolumeModifierLegL = 1.175573 * 0.986529;
    double VolumeModifierPulmArtL = 0.855566 * 1.095697, VolumeModifierPulmCapL = 0.724704 * 1.079139, VolumeModifierPulmVeinL = 0.548452 * 1.056844 * 1.062, VolumeModifierLiver = 1.157475 * 0.991848;
    double VolumeModifierMuscle = 1.175573 * 0.986529, VolumeModifierMyocard = 1.175564 * 0.986531, VolumeModifierArmR = 1.175573 * 0.986529, VolumeModifierKidneyR = 0.737649 * 0.954339;
    double VolumeModifierLegR = 1.175573 * 0.986529, VolumeModifierPulmArtR = 0.756158 * 1.121167, VolumeModifierPulmCapR = 0.602545 * 1.118213, VolumeModifierPulmVeinR = 0.395656 * 1.11424 * 1.11;
    double VolumeModifierSkin = 1.007306 * 1.035695, VolumeModifierSmallIntestine = 1.17528 * 0.986509, VolumeModifierSplanchnic = 1.17528 * 0.986509, VolumeModifierSpleen = 1.17528 * 0.986509;
    double VolumeModifierVenaCava = 0.66932 * 1.134447;

    //And also modify the compliances
    Aorta1ToAorta4.GetComplianceBaseline().SetValue(largeArteriesComplianceModifier * Aorta1ToAorta4.GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg), VolumePerPressureUnit::mL_Per_mmHg);

    //For Internal Hemorrhage
    AbdominalCavityToGround.GetComplianceBaseline().SetValue(100.0, VolumePerPressureUnit::mL_Per_mmHg);

    RightPulmonaryArteries1.GetVolumeBaseline().SetValue(VolumeModifierPulmArtR * RightPulmonaryArteries1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    LeftPulmonaryArteries1.GetVolumeBaseline().SetValue(VolumeModifierPulmArtL * LeftPulmonaryArteries1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    //RightPulmonaryCapillaries1.GetVolumeBaseline().SetValue(VolumeModifierPulmCapR * RightPulmonaryCapillaries1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    //LeftPulmonaryCapillaries1.GetVolumeBaseline().SetValue(VolumeModifierPulmCapL * LeftPulmonaryCapillaries1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    RightPulmonaryVeins1.GetVolumeBaseline().SetValue(VolumeModifierPulmVeinR * RightPulmonaryVeins1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    LeftPulmonaryVeins1.GetVolumeBaseline().SetValue(VolumeModifierPulmVeinL * LeftPulmonaryVeins1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    Aorta1.GetVolumeBaseline().SetValue(VolumeModifierAorta * Aorta1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    Brain1.GetVolumeBaseline().SetValue(VolumeModifierBrain * Brain1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    Bone1.GetVolumeBaseline().SetValue(VolumeModifierBone * Bone1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    Fat1.GetVolumeBaseline().SetValue(VolumeModifierFat * Fat1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    LargeIntestine1.GetVolumeBaseline().SetValue(VolumeModifierLargeIntestine * LargeIntestine1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    Liver1.GetVolumeBaseline().SetValue(VolumeModifierLiver * Liver1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    LeftArm1.GetVolumeBaseline().SetValue(VolumeModifierArmL * LeftArm1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    LeftKidney1.GetVolumeBaseline().SetValue(VolumeModifierKidneyL * LeftKidney1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    LeftLeg1.GetVolumeBaseline().SetValue(VolumeModifierLegL * LeftLeg1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    Muscle1.GetVolumeBaseline().SetValue(VolumeModifierMuscle * Muscle1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    Myocardium1.GetVolumeBaseline().SetValue(VolumeModifierMyocard * Myocardium1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    RightArm1.GetVolumeBaseline().SetValue(VolumeModifierArmR * RightArm1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    RightKidney1.GetVolumeBaseline().SetValue(VolumeModifierKidneyR * RightKidney1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    RightLeg1.GetVolumeBaseline().SetValue(VolumeModifierLegR * RightLeg1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    Skin1.GetVolumeBaseline().SetValue(VolumeModifierSkin * Skin1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    SmallIntestine1.GetVolumeBaseline().SetValue(VolumeModifierSmallIntestine * SmallIntestine1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    Splanchnic1.GetVolumeBaseline().SetValue(VolumeModifierSplanchnic * Splanchnic1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    Spleen1.GetVolumeBaseline().SetValue(VolumeModifierSpleen * Spleen1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);
    VenaCava1.GetVolumeBaseline().SetValue(VolumeModifierVenaCava * VenaCava1.GetVolumeBaseline(VolumeUnit::mL), VolumeUnit::mL);

    // Prepare circuit for compartment creation
    cCardiovascular.SetNextAndCurrentFromBaselines();
    cCardiovascular.StateChange();

    SEFluidCircuit& cCombinedCardiovascular = m_Circuits->GetActiveCardiovascularCircuit();
    cCombinedCardiovascular.AddCircuit(cCardiovascular);
    cCombinedCardiovascular.SetNextAndCurrentFromBaselines();
    cCombinedCardiovascular.StateChange();

    /////////////////////////
    // Create Compartments //
    /////////////////////////

    /////////////////
    // Right Heart //
    SELiquidCompartment& vRightHeart = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::RightHeart);
    vRightHeart.MapNode(RightHeart1);
    vRightHeart.MapNode(RightHeart2);
    vRightHeart.MapNode(RightHeart3);
    vRightHeart.MapNode(MainPulmonaryArteries1);
    //////////////////////////////
    // Right Pulmonary Arteries //
    SELiquidCompartment& vRightPulmonaryArteries = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::RightPulmonaryArteries);
    vRightPulmonaryArteries.MapNode(RightPulmonaryArteries1);
    vRightPulmonaryArteries.MapNode(RightIntermediatePulmonaryArteries1);
    /////////////////////////////
    // Left Pulmonary Arteries //
    SELiquidCompartment& vLeftPulmonaryArteries = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::LeftPulmonaryArteries);
    vLeftPulmonaryArteries.MapNode(LeftPulmonaryArteries1);
    vLeftPulmonaryArteries.MapNode(LeftIntermediatePulmonaryArteries1);
    ////////////////////////
    // Pulmonary Arteries //
    SELiquidCompartment& vPulmonaryArteries = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::PulmonaryArteries);
    vPulmonaryArteries.AddChild(vRightPulmonaryArteries);
    vPulmonaryArteries.AddChild(vLeftPulmonaryArteries);
    /////////////////////////////////
    // Right Pulmonary Capillaries //
    //SELiquidCompartment& vRightPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::RightPulmonaryCapillaries);
    //vRightPulmonaryCapillaries.MapNode(RightPulmonaryCapillaries1);
    ////////////////////////////////
    // Left Pulmonary Capillaries //
    //SELiquidCompartment& vLeftPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::LeftPulmonaryCapillaries);
    //vLeftPulmonaryCapillaries.MapNode(LeftPulmonaryCapillaries1);
    ///////////////////////////
    // Pulmonary Capillaries //
    //SELiquidCompartment& vPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::PulmonaryCapillaries);
    //vPulmonaryCapillaries.AddChild(vRightPulmonaryCapillaries);
    //vPulmonaryCapillaries.AddChild(vLeftPulmonaryCapillaries);
    ///////////////////////////
    // Right Pulmonary Veins //
    SELiquidCompartment& vRightPulmonaryVeins = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::RightPulmonaryVeins);
    vRightPulmonaryVeins.MapNode(RightPulmonaryVeins1);
    //////////////////////////
    // Left Pulmonary Veins //
    SELiquidCompartment& vLeftPulmonaryVeins = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::LeftPulmonaryVeins);
    vLeftPulmonaryVeins.MapNode(LeftPulmonaryVeins1);
    /////////////////////
    // Pulmonary Veins //
    SELiquidCompartment& vPulmonaryVeins = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::PulmonaryVeins);
    vPulmonaryVeins.AddChild(vRightPulmonaryVeins);
    vPulmonaryVeins.AddChild(vLeftPulmonaryVeins);
    ////////////////
    // Left Heart //
    SELiquidCompartment& vLeftHeart = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::LeftHeart);
    vLeftHeart.MapNode(LeftHeart1);
    vLeftHeart.MapNode(LeftHeart2);
    vLeftHeart.MapNode(LeftHeart3);
    ///////////
    // Aorta //
    SELiquidCompartment& vAorta = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::Aorta);
    vAorta.MapNode(Aorta1);
    vAorta.MapNode(Aorta2);
    vAorta.MapNode(Aorta3);
    vAorta.MapNode(Aorta4);
    ///////////
    // Abdominal Cavity //
    SELiquidCompartment& vAbdominalCavity = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::AbdominalCavity);
    vAbdominalCavity.MapNode(AbdominalCavity);
    // Brain //
    SELiquidCompartment& vBrain = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::Brain);
    vBrain.MapNode(Brain1);
    vBrain.MapNode(Brain2);
    //////////
    // Bone //
    SELiquidCompartment& vBone = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::Bone);
    vBone.MapNode(Bone1);
    vBone.MapNode(Bone2);
    /////////
    // Fat //
    SELiquidCompartment& vFat = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::Fat);
    vFat.MapNode(Fat1);
    vFat.MapNode(Fat2);
    /////////////////////
    // Large Intestine //
    SELiquidCompartment& vLargeIntestine = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::LargeIntestine);
    vLargeIntestine.MapNode(LargeIntestine1);
    ///////////
    // Liver //
    SELiquidCompartment& vLiver = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::Liver);
    vLiver.MapNode(Liver1);
    vLiver.MapNode(Liver2);
    vLiver.MapNode(PortalVein1);
    //////////////
    // Left Arm //
    SELiquidCompartment& vLeftArm = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::LeftArm);
    vLeftArm.MapNode(LeftArm1);
    vLeftArm.MapNode(LeftArm2);
    /////////////////
    // Left Kidney //
    SELiquidCompartment& vLeftKidney = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::LeftKidney);
    vLeftKidney.MapNode(LeftKidney1);
    vLeftKidney.MapNode(LeftKidney2);
    //////////////
    // Left Leg //
    SELiquidCompartment& vLeftLeg = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::LeftLeg);
    vLeftLeg.MapNode(LeftLeg1);
    vLeftLeg.MapNode(LeftLeg2);
    ////////////
    // Muscle //
    SELiquidCompartment& vMuscle = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::Muscle);
    vMuscle.MapNode(Muscle1);
    vMuscle.MapNode(Muscle2);
    ////////////////
    // Myocardium //
    SELiquidCompartment& vMyocardium = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::Myocardium);
    vMyocardium.MapNode(Myocardium1);
    vMyocardium.MapNode(Myocardium2);
    /////////////////
    // Pericardium //
    SELiquidCompartment& vPericardium = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::Pericardium);
    vPericardium.MapNode(Pericardium);
    ///////////////
    // Right Arm //
    SELiquidCompartment& vRightArm = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::RightArm);
    vRightArm.MapNode(RightArm1);
    vRightArm.MapNode(RightArm2);
    //////////////////
    // Right Kidney // 
    SELiquidCompartment& vRightKidney = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::RightKidney);
    vRightKidney.MapNode(RightKidney1);
    vRightKidney.MapNode(RightKidney2);
    ///////////////
    // Right Leg //
    SELiquidCompartment& vRightLeg = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::RightLeg);
    vRightLeg.MapNode(RightLeg1);
    vRightLeg.MapNode(RightLeg2);
    //////////
    // Skin //
    SELiquidCompartment& vSkin = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::Skin);
    vSkin.MapNode(Skin1);
    vSkin.MapNode(Skin2);
    /////////////////////
    // Small Intestine //
    SELiquidCompartment& vSmallIntestine = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::SmallIntestine);
    vSmallIntestine.MapNode(SmallIntestine1);
    ////////////////
    // Splanchnic //
    SELiquidCompartment& vSplanchnic = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::Splanchnic);
    vSplanchnic.MapNode(Splanchnic1);
    ////////////
    // Spleen //
    SELiquidCompartment& vSpleen = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::Spleen);
    vSpleen.MapNode(Spleen1);
    //////////////
    // VenaCava //
    SELiquidCompartment& vVenaCava = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::VenaCava);
    vVenaCava.MapNode(VenaCava1);
    ////////////
    // Ground //
    SELiquidCompartment& vGround = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::Ground);
    vGround.MapNode(Ground);

    SELiquidCompartment& vRightSuperiorLobeApicalPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedLungsVascularCompartment::RightSuperiorLobeApicalPulmonaryCapillaries);
    SELiquidCompartment& vRightSuperiorLobePosteriorPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedLungsVascularCompartment::RightSuperiorLobePosteriorPulmonaryCapillaries);
    SELiquidCompartment& vRightSuperiorLobeAnteriorPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedLungsVascularCompartment::RightSuperiorLobeAnteriorPulmonaryCapillaries);
    SELiquidCompartment& vRightMiddleLobeLateralPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedLungsVascularCompartment::RightMiddleLobeLateralPulmonaryCapillaries);
    SELiquidCompartment& vRightMiddleLobeMedialPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedLungsVascularCompartment::RightMiddleLobeMedialPulmonaryCapillaries);
    SELiquidCompartment& vRightInferiorLobeSuperiorPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedLungsVascularCompartment::RightInferiorLobeSuperiorPulmonaryCapillaries);
    SELiquidCompartment& vRightInferiorLobeMedialBasalPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedLungsVascularCompartment::RightInferiorLobeMedialBasalPulmonaryCapillaries);
    SELiquidCompartment& vRightInferiorLobeAnteriorBasalPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedLungsVascularCompartment::RightInferiorLobeAnteriorBasalPulmonaryCapillaries);
    SELiquidCompartment& vRightInferiorLobeLateralBasalPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedLungsVascularCompartment::RightInferiorLobeLateralBasalPulmonaryCapillaries);
    SELiquidCompartment& vRightInferiorLobePosteriorBasalPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedLungsVascularCompartment::RightInferiorLobePosteriorBasalPulmonaryCapillaries);

    SELiquidCompartment& vLeftInferiorLobePosteriorBasalPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedLungsVascularCompartment::LeftInferiorLobePosteriorBasalPulmonaryCapillaries);
    SELiquidCompartment& vLeftInferiorLobeLateralBasalPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedLungsVascularCompartment::LeftInferiorLobeLateralBasalPulmonaryCapillaries);
    SELiquidCompartment& vLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedLungsVascularCompartment::LeftInferiorLobeAnteromedialBasalPulmonaryCapillaries);
    SELiquidCompartment& vLeftInferiorLobeSuperiorPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedLungsVascularCompartment::LeftInferiorLobeSuperiorPulmonaryCapillaries);
    SELiquidCompartment& vLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedLungsVascularCompartment::LeftSuperiorLobeInferiorLingulaPulmonaryCapillaries);
    SELiquidCompartment& vLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedLungsVascularCompartment::LeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries);
    SELiquidCompartment& vLeftSuperiorLobeAnteriorPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedLungsVascularCompartment::LeftSuperiorLobeAnteriorPulmonaryCapillaries);
    SELiquidCompartment& vLeftSuperiorLobeApicoposteriorPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::ExpandedLungsVascularCompartment::LeftSuperiorLobeApicoposteriorPulmonaryCapillaries);

    vRightSuperiorLobeApicalPulmonaryCapillaries.MapNode(RightSuperiorLobeApicalPulmonaryCapillaries);
    vRightSuperiorLobePosteriorPulmonaryCapillaries.MapNode(RightSuperiorLobePosteriorPulmonaryCapillaries);
    vRightSuperiorLobeAnteriorPulmonaryCapillaries.MapNode(RightSuperiorLobeAnteriorPulmonaryCapillaries);
    vRightMiddleLobeLateralPulmonaryCapillaries.MapNode(RightMiddleLobeLateralPulmonaryCapillaries);
    vRightMiddleLobeMedialPulmonaryCapillaries.MapNode(RightMiddleLobeMedialPulmonaryCapillaries);
    vRightInferiorLobeSuperiorPulmonaryCapillaries.MapNode(RightInferiorLobeSuperiorPulmonaryCapillaries);
    vRightInferiorLobeMedialBasalPulmonaryCapillaries.MapNode(RightInferiorLobeMedialBasalPulmonaryCapillaries);
    vRightInferiorLobeAnteriorBasalPulmonaryCapillaries.MapNode(RightInferiorLobeAnteriorBasalPulmonaryCapillaries);
    vRightInferiorLobeLateralBasalPulmonaryCapillaries.MapNode(RightInferiorLobeLateralBasalPulmonaryCapillaries);
    vRightInferiorLobePosteriorBasalPulmonaryCapillaries.MapNode(RightInferiorLobePosteriorBasalPulmonaryCapillaries);

    vLeftInferiorLobePosteriorBasalPulmonaryCapillaries.MapNode(LeftInferiorLobePosteriorBasalPulmonaryCapillaries);
    vLeftInferiorLobeLateralBasalPulmonaryCapillaries.MapNode(LeftInferiorLobeLateralBasalPulmonaryCapillaries);
    vLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries.MapNode(LeftInferiorLobeAnteromedialBasalPulmonaryCapillaries);
    vLeftInferiorLobeSuperiorPulmonaryCapillaries.MapNode(LeftInferiorLobeSuperiorPulmonaryCapillaries);
    vLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries.MapNode(LeftSuperiorLobeInferiorLingulaPulmonaryCapillaries);
    vLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries.MapNode(LeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries);
    vLeftSuperiorLobeAnteriorPulmonaryCapillaries.MapNode(LeftSuperiorLobeAnteriorPulmonaryCapillaries);
    vLeftSuperiorLobeApicoposteriorPulmonaryCapillaries.MapNode(LeftSuperiorLobeApicoposteriorPulmonaryCapillaries);

    //////////////////////////
    // Set up our hierarchy //
    //////////////////////////
    SELiquidCompartment& vRightPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::RightPulmonaryCapillaries);
    SELiquidCompartment& vLeftPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::LeftPulmonaryCapillaries);

    vRightPulmonaryCapillaries.AddChild(vRightSuperiorLobeApicalPulmonaryCapillaries);
    vRightPulmonaryCapillaries.AddChild(vRightSuperiorLobePosteriorPulmonaryCapillaries);
    vRightPulmonaryCapillaries.AddChild(vRightSuperiorLobeAnteriorPulmonaryCapillaries);
    vRightPulmonaryCapillaries.AddChild(vRightMiddleLobeLateralPulmonaryCapillaries);
    vRightPulmonaryCapillaries.AddChild(vRightMiddleLobeMedialPulmonaryCapillaries);
    vRightPulmonaryCapillaries.AddChild(vRightInferiorLobeSuperiorPulmonaryCapillaries);
    vRightPulmonaryCapillaries.AddChild(vRightInferiorLobeMedialBasalPulmonaryCapillaries);
    vRightPulmonaryCapillaries.AddChild(vRightInferiorLobeAnteriorBasalPulmonaryCapillaries);
    vRightPulmonaryCapillaries.AddChild(vRightInferiorLobeLateralBasalPulmonaryCapillaries);
    vRightPulmonaryCapillaries.AddChild(vRightInferiorLobePosteriorBasalPulmonaryCapillaries);

    vLeftPulmonaryCapillaries.AddChild(vLeftInferiorLobePosteriorBasalPulmonaryCapillaries);
    vLeftPulmonaryCapillaries.AddChild(vLeftInferiorLobeLateralBasalPulmonaryCapillaries);
    vLeftPulmonaryCapillaries.AddChild(vLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries);
    vLeftPulmonaryCapillaries.AddChild(vLeftInferiorLobeSuperiorPulmonaryCapillaries);
    vLeftPulmonaryCapillaries.AddChild(vLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries);
    vLeftPulmonaryCapillaries.AddChild(vLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries);
    vLeftPulmonaryCapillaries.AddChild(vLeftSuperiorLobeAnteriorPulmonaryCapillaries);
    vLeftPulmonaryCapillaries.AddChild(vLeftSuperiorLobeApicoposteriorPulmonaryCapillaries);

    SELiquidCompartment& vPulmonaryCapillaries = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::PulmonaryCapillaries);
    vPulmonaryCapillaries.AddChild(vRightPulmonaryCapillaries);
    vPulmonaryCapillaries.AddChild(vLeftPulmonaryCapillaries);

    SELiquidCompartment& vKidneys = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::Kidneys);
    vKidneys.AddChild(vLeftKidney);
    vKidneys.AddChild(vRightKidney);
    SELiquidCompartment& vHeart = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::Heart);
    vHeart.AddChild(vMyocardium);
    vHeart.AddChild(vLeftHeart);
    vHeart.AddChild(vRightHeart);
    vHeart.AddChild(vPericardium);
    SELiquidCompartment& vLeftLung = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::LeftLung);
    vLeftLung.AddChild(vLeftPulmonaryArteries);
    vLeftLung.AddChild(vLeftPulmonaryCapillaries);
    vLeftLung.AddChild(vLeftPulmonaryVeins);
    SELiquidCompartment& vRightLung = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::RightLung);
    vRightLung.AddChild(vRightPulmonaryArteries);
    vRightLung.AddChild(vRightPulmonaryCapillaries);
    vRightLung.AddChild(vRightPulmonaryVeins);
    SELiquidCompartment& vLungs = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::Lungs);
    vLungs.AddChild(vLeftLung);
    vLungs.AddChild(vRightLung);
    SELiquidCompartment& vGut = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::Gut);
    vGut.AddChild(vSplanchnic);
    vGut.AddChild(vSmallIntestine);
    vGut.AddChild(vLargeIntestine);
    SELiquidCompartment& vAbdomen = m_Compartments->CreateLiquidCompartment(pulse::VascularCompartment::Abdomen);
    vAbdomen.AddChild(vSplanchnic);
    vAbdomen.AddChild(vSmallIntestine);
    vAbdomen.AddChild(vLargeIntestine);
    vAbdomen.AddChild(vLeftKidney);
    vAbdomen.AddChild(vRightKidney);
    vAbdomen.AddChild(vSpleen);
    vAbdomen.AddChild(vLiver);
    vAbdomen.AddChild(vVenaCava);
    vAbdomen.AddChild(vAorta);
    vAbdomen.AddChild(vAbdominalCavity);


    //////////////////
    // Create Links //
    //////////////////

    /////////////////////
    // Heart and Lungs //
    SELiquidCompartmentLink& vVenaCavaToRightHeart = m_Compartments->CreateLiquidLink(vVenaCava, vRightHeart, pulse::VascularLink::VenaCavaToRightHeart);
    vVenaCavaToRightHeart.MapPath(VenaCava1ToRightHeart2);
    SELiquidCompartmentLink& vRightHeartToLeftPulmonaryArteries = m_Compartments->CreateLiquidLink(vRightHeart, vLeftPulmonaryArteries, pulse::VascularLink::RightHeartToLeftPulmonaryArteries);
    vRightHeartToLeftPulmonaryArteries.MapPath(MainPulmonaryArteries1ToLeftIntermediatePulmonaryArteries1);
    //SELiquidCompartmentLink& vLeftPulmonaryArteriesToCapillaries = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftPulmonaryCapillaries, pulse::VascularLink::LeftPulmonaryArteriesToCapillaries);
    //vLeftPulmonaryArteriesToCapillaries.MapPath(LeftPulmonaryArteries1ToLeftPulmonaryCapillaries1);
    //SELiquidCompartmentLink& vLeftPulmonaryArteriesToVeins = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftPulmonaryVeins, pulse::VascularLink::LeftPulmonaryArteriesToVeins);
    //vLeftPulmonaryArteriesToVeins.MapPath(LeftPulmonaryArteries1ToLeftPulmonaryVeins1);
    //SELiquidCompartmentLink& vLeftPulmonaryCapillariesToVeins = m_Compartments->CreateLiquidLink(vLeftPulmonaryCapillaries, vLeftPulmonaryVeins, pulse::VascularLink::LeftPulmonaryCapillariesToVeins);
    //vLeftPulmonaryCapillariesToVeins.MapPath(LeftPulmonaryCapillaries1ToLeftPulmonaryVeins1);
    SELiquidCompartmentLink& vLeftPulmonaryVeinsToLeftHeart = m_Compartments->CreateLiquidLink(vLeftPulmonaryVeins, vLeftHeart, pulse::VascularLink::LeftPulmonaryVeinsToLeftHeart);
    vLeftPulmonaryVeinsToLeftHeart.MapPath(LeftIntermediatePulmonaryVeins1ToLeftHeart2);
    SELiquidCompartmentLink& vRightHeartToRightPulmonaryArteries = m_Compartments->CreateLiquidLink(vRightHeart, vRightPulmonaryArteries, pulse::VascularLink::RightHeartToRightPulmonaryArteries);
    vRightHeartToRightPulmonaryArteries.MapPath(MainPulmonaryArteries1ToRightIntermediatePulmonaryArteries1);
    //SELiquidCompartmentLink& vRightPulmonaryArteriesToCapillaries = m_Compartments->CreateLiquidLink(vRightPulmonaryArteries, vRightPulmonaryCapillaries, pulse::VascularLink::RightPulmonaryArteriesToCapillaries);
    //vRightPulmonaryArteriesToCapillaries.MapPath(RightPulmonaryArteries1ToRightPulmonaryCapillaries1);
    //SELiquidCompartmentLink& vRightPulmonaryArteriesToVeins = m_Compartments->CreateLiquidLink(vRightPulmonaryArteries, vRightPulmonaryVeins, pulse::VascularLink::RightPulmonaryArteriesToVeins);
    //vRightPulmonaryArteriesToVeins.MapPath(RightPulmonaryArteries1ToRightPulmonaryVeins1);
    //SELiquidCompartmentLink& vRightPulmonaryCapillariesToVeins = m_Compartments->CreateLiquidLink(vRightPulmonaryCapillaries, vRightPulmonaryVeins, pulse::VascularLink::RightPulmonaryCapillariesToVeins);
    //vRightPulmonaryCapillariesToVeins.MapPath(RightPulmonaryCapillaries1ToRightPulmonaryVeins1);
    SELiquidCompartmentLink& vRightPulmonaryVeinsToLeftHeart = m_Compartments->CreateLiquidLink(vRightPulmonaryVeins, vLeftHeart, pulse::VascularLink::RightPulmonaryVeinsToLeftHeart);
    vRightPulmonaryVeinsToLeftHeart.MapPath(RightIntermediatePulmonaryVeins1ToLeftHeart2);
    SELiquidCompartmentLink& vLeftHeartToAorta = m_Compartments->CreateLiquidLink(vLeftHeart, vAorta, pulse::VascularLink::LeftHeartToAorta);
    vLeftHeartToAorta.MapPath(LeftHeart1ToAorta2);
    //////////
    // Bone //
    SELiquidCompartmentLink& vAortaToBone = m_Compartments->CreateLiquidLink(vAorta, vBone, pulse::VascularLink::AortaToBone);
    vAortaToBone.MapPath(Aorta1ToBone1);
    SELiquidCompartmentLink& vBoneToVenaCava = m_Compartments->CreateLiquidLink(vBone, vVenaCava, pulse::VascularLink::BoneToVenaCava);
    vBoneToVenaCava.MapPath(Bone2ToVenaCava1);
    ///////////
    // Brain //
    SELiquidCompartmentLink& vAortaToBrain = m_Compartments->CreateLiquidLink(vAorta, vBrain, pulse::VascularLink::AortaToBrain);
    vAortaToBrain.MapPath(Aorta1ToBrain1);
    SELiquidCompartmentLink& vBrainToVenaCava = m_Compartments->CreateLiquidLink(vBrain, vVenaCava, pulse::VascularLink::BrainToVenaCava);
    vBrainToVenaCava.MapPath(Brain2ToVenaCava1);
    /////////
    // Fat //
    SELiquidCompartmentLink& vAortaToFat = m_Compartments->CreateLiquidLink(vAorta, vFat, pulse::VascularLink::AortaToFat);
    vAortaToFat.MapPath(Aorta1ToFat1);
    SELiquidCompartmentLink& vFatToVenaCava = m_Compartments->CreateLiquidLink(vFat, vVenaCava, pulse::VascularLink::FatToVenaCava);
    vFatToVenaCava.MapPath(Fat2ToVenaCava1);
    /////////////////////
    // Large Intestine //
    SELiquidCompartmentLink& vAortaToLargeIntestine = m_Compartments->CreateLiquidLink(vAorta, vLargeIntestine, pulse::VascularLink::AortaToLargeIntestine);
    vAortaToLargeIntestine.MapPath(Aorta1ToLargeIntestine1);
    SELiquidCompartmentLink& vLargeIntestineToLiver = m_Compartments->CreateLiquidLink(vLargeIntestine, vLiver, pulse::VascularLink::LargeIntestineToLiver);
    vLargeIntestineToLiver.MapPath(LargeIntestine1ToPortalVein1);
    ///////////
    // Liver //
    SELiquidCompartmentLink& vAortaToLiver = m_Compartments->CreateLiquidLink(vAorta, vLiver, pulse::VascularLink::AortaToLiver);
    vAortaToLiver.MapPath(Aorta1ToLiver1);
    SELiquidCompartmentLink& vLiverToVenaCava = m_Compartments->CreateLiquidLink(vLiver, vVenaCava, pulse::VascularLink::LiverToVenaCava);
    vLiverToVenaCava.MapPath(Liver2ToVenaCava1);
    //////////////
    // Left Arm //
    SELiquidCompartmentLink& vAortaToLeftArm = m_Compartments->CreateLiquidLink(vAorta, vLeftArm, pulse::VascularLink::AortaToLeftArm);
    vAortaToLeftArm.MapPath(Aorta1ToLeftArm1);
    SELiquidCompartmentLink& vLeftArmToVenaCava = m_Compartments->CreateLiquidLink(vLeftArm, vVenaCava, pulse::VascularLink::LeftArmToVenaCava);
    vLeftArmToVenaCava.MapPath(LeftArm2ToVenaCava1);
    /////////////////
    // Left Kidney //
    SELiquidCompartmentLink& vAortaToLeftKidney = m_Compartments->CreateLiquidLink(vAorta, vLeftKidney, pulse::VascularLink::AortaToLeftKidney);
    vAortaToLeftKidney.MapPath(Aorta1ToLeftKidney1);
    SELiquidCompartmentLink& vLeftKidneyToVenaCava = m_Compartments->CreateLiquidLink(vLeftKidney, vVenaCava, pulse::VascularLink::LeftKidneyToVenaCava);
    vLeftKidneyToVenaCava.MapPath(LeftKidney2ToVenaCava1);
    //////////////
    // Left Leg //
    SELiquidCompartmentLink& vAortaToLeftLeg = m_Compartments->CreateLiquidLink(vAorta, vLeftLeg, pulse::VascularLink::AortaToLeftLeg);
    vAortaToLeftLeg.MapPath(Aorta1ToLeftLeg1);
    SELiquidCompartmentLink& vLeftLegToVenaCava = m_Compartments->CreateLiquidLink(vLeftLeg, vVenaCava, pulse::VascularLink::LeftLegToVenaCava);
    vLeftLegToVenaCava.MapPath(LeftLeg2ToVenaCava1);
    ////////////
    // Muscle //
    SELiquidCompartmentLink& vAortaToMuscle = m_Compartments->CreateLiquidLink(vAorta, vMuscle, pulse::VascularLink::AortaToMuscle);
    vAortaToMuscle.MapPath(Aorta1ToMuscle1);
    SELiquidCompartmentLink& vMuscleToVenaCava = m_Compartments->CreateLiquidLink(vMuscle, vVenaCava, pulse::VascularLink::MuscleToVenaCava);
    vMuscleToVenaCava.MapPath(Muscle2ToVenaCava1);
    ////////////////
    // Myocardium //
    SELiquidCompartmentLink& vAortaToMyocardium = m_Compartments->CreateLiquidLink(vAorta, vMyocardium, pulse::VascularLink::AortaToMyocardium);
    vAortaToMyocardium.MapPath(Aorta1ToMyocardium1);
    SELiquidCompartmentLink& vMyocardiumToVenaCava = m_Compartments->CreateLiquidLink(vMyocardium, vVenaCava, pulse::VascularLink::MyocardiumToVenaCava);
    vMyocardiumToVenaCava.MapPath(Myocardium2ToVenaCava1);
    ///////////////
    // Right Arm //
    SELiquidCompartmentLink& vAortaToRightArm = m_Compartments->CreateLiquidLink(vAorta, vRightArm, pulse::VascularLink::AortaToRightArm);
    vAortaToRightArm.MapPath(Aorta1ToRightArm1);
    SELiquidCompartmentLink& vRightArmToVenaCava = m_Compartments->CreateLiquidLink(vRightArm, vVenaCava, pulse::VascularLink::RightArmToVenaCava);
    vRightArmToVenaCava.MapPath(RightArm2ToVenaCava1);
    //////////////////
    // Right Kidney // 
    SELiquidCompartmentLink& vAortaToRightKidney = m_Compartments->CreateLiquidLink(vAorta, vRightKidney, pulse::VascularLink::AortaToRightKidney);
    vAortaToRightKidney.MapPath(Aorta1ToRightKidney1);
    SELiquidCompartmentLink& vRightKidneyToVenaCava = m_Compartments->CreateLiquidLink(vRightKidney, vVenaCava, pulse::VascularLink::RightKidneyToVenaCava);
    vRightKidneyToVenaCava.MapPath(RightKidney2ToVenaCava1);
    ///////////////
    // Right Leg //
    SELiquidCompartmentLink& vAortaToRightLeg = m_Compartments->CreateLiquidLink(vAorta, vRightLeg, pulse::VascularLink::AortaToRightLeg);
    vAortaToRightLeg.MapPath(Aorta1ToRightLeg1);
    SELiquidCompartmentLink& vRightLegToVenaCava = m_Compartments->CreateLiquidLink(vRightLeg, vVenaCava, pulse::VascularLink::RightLegToVenaCava);
    vRightLegToVenaCava.MapPath(RightLeg2ToVenaCava1);
    //////////
    // Skin //
    SELiquidCompartmentLink& vAortaToSkin = m_Compartments->CreateLiquidLink(vAorta, vSkin, pulse::VascularLink::AortaToSkin);
    vAortaToSkin.MapPath(Aorta1ToSkin1);
    SELiquidCompartmentLink& vSkinToVenaCava = m_Compartments->CreateLiquidLink(vSkin, vVenaCava, pulse::VascularLink::SkinToVenaCava);
    vSkinToVenaCava.MapPath(Skin2ToVenaCava1);
    /////////////////////
    // Small Intestine //
    SELiquidCompartmentLink& vAortaToSmallIntestine = m_Compartments->CreateLiquidLink(vAorta, vSmallIntestine, pulse::VascularLink::AortaToSmallIntestine);
    vAortaToSmallIntestine.MapPath(Aorta1ToSmallIntestine1);
    SELiquidCompartmentLink& vSmallIntestineToLiver = m_Compartments->CreateLiquidLink(vSmallIntestine, vLiver, pulse::VascularLink::SmallIntestineToLiver);
    vSmallIntestineToLiver.MapPath(SmallIntestine1ToPortalVein1);
    ////////////////
    // Splanchnic //
    SELiquidCompartmentLink& vAortaToSplanchnic = m_Compartments->CreateLiquidLink(vAorta, vSplanchnic, pulse::VascularLink::AortaToSplanchnic);
    vAortaToSplanchnic.MapPath(Aorta1ToSplanchnic1);
    SELiquidCompartmentLink& vSplanchnicToLiver = m_Compartments->CreateLiquidLink(vSplanchnic, vLiver, pulse::VascularLink::SplanchnicToLiver);
    vSplanchnicToLiver.MapPath(SplanchnicToPortalVein);
    ////////////
    // Spleen //
    SELiquidCompartmentLink& vAortaToSpleen = m_Compartments->CreateLiquidLink(vAorta, vSpleen, pulse::VascularLink::AortaToSpleen);
    vAortaToSpleen.MapPath(Aorta1ToSpleen1);
    SELiquidCompartmentLink& vSpleenToLiver = m_Compartments->CreateLiquidLink(vSpleen, vLiver, pulse::VascularLink::SpleenToLiver);
    vSpleenToLiver.MapPath(Spleen1ToPortalVein1);

    /////////////////////
    // Bleeds and IV's //
    SELiquidCompartmentLink& vVenaCavaIV = m_Compartments->CreateLiquidLink(vGround, vVenaCava, pulse::VascularLink::VenaCavaIV);
    vVenaCavaIV.MapPath(IVToVenaCava1);

    SELiquidCompartmentLink& vRightPulmonaryArteriesToRightSuperiorLobeApicalPulmonaryCapillaries = m_Compartments->CreateLiquidLink(vRightPulmonaryArteries, vRightSuperiorLobeApicalPulmonaryCapillaries, pulse::ExpandedLungsVascularLink::RightPulmonaryArteriesToRightSuperiorLobeApicalPulmonaryCapillaries);
    SELiquidCompartmentLink& vRightPulmonaryArteriesToRightSuperiorLobePosteriorPulmonaryCapillaries = m_Compartments->CreateLiquidLink(vRightPulmonaryArteries, vRightSuperiorLobePosteriorPulmonaryCapillaries, pulse::ExpandedLungsVascularLink::RightPulmonaryArteriesToRightSuperiorLobePosteriorPulmonaryCapillaries);
    SELiquidCompartmentLink& vRightPulmonaryArteriesToRightSuperiorLobeAnteriorPulmonaryCapillaries = m_Compartments->CreateLiquidLink(vRightPulmonaryArteries, vRightSuperiorLobeAnteriorPulmonaryCapillaries, pulse::ExpandedLungsVascularLink::RightPulmonaryArteriesToRightSuperiorLobeAnteriorPulmonaryCapillaries);
    SELiquidCompartmentLink& vRightPulmonaryArteriesToRightMiddleLobeLateralPulmonaryCapillaries = m_Compartments->CreateLiquidLink(vRightPulmonaryArteries, vRightMiddleLobeLateralPulmonaryCapillaries, pulse::ExpandedLungsVascularLink::RightPulmonaryArteriesToRightMiddleLobeLateralPulmonaryCapillaries);
    SELiquidCompartmentLink& vRightPulmonaryArteriesToRightMiddleLobeMedialPulmonaryCapillaries = m_Compartments->CreateLiquidLink(vRightPulmonaryArteries, vRightMiddleLobeMedialPulmonaryCapillaries, pulse::ExpandedLungsVascularLink::RightPulmonaryArteriesToRightMiddleLobeMedialPulmonaryCapillaries);
    SELiquidCompartmentLink& vRightPulmonaryArteriesToRightInferiorLobeSuperiorPulmonaryCapillaries = m_Compartments->CreateLiquidLink(vRightPulmonaryArteries, vRightInferiorLobeSuperiorPulmonaryCapillaries, pulse::ExpandedLungsVascularLink::RightPulmonaryArteriesToRightInferiorLobeSuperiorPulmonaryCapillaries);
    SELiquidCompartmentLink& vRightPulmonaryArteriesToRightInferiorLobeMedialBasalPulmonaryCapillaries = m_Compartments->CreateLiquidLink(vRightPulmonaryArteries, vRightInferiorLobeMedialBasalPulmonaryCapillaries, pulse::ExpandedLungsVascularLink::RightPulmonaryArteriesToRightInferiorLobeMedialBasalPulmonaryCapillaries);
    SELiquidCompartmentLink& vRightPulmonaryArteriesToRightInferiorLobeAnteriorBasalPulmonaryCapillaries = m_Compartments->CreateLiquidLink(vRightPulmonaryArteries, vRightInferiorLobeAnteriorBasalPulmonaryCapillaries, pulse::ExpandedLungsVascularLink::RightPulmonaryArteriesToRightInferiorLobeAnteriorBasalPulmonaryCapillaries);
    SELiquidCompartmentLink& vRightPulmonaryArteriesToRightInferiorLobeLateralBasalPulmonaryCapillaries = m_Compartments->CreateLiquidLink(vRightPulmonaryArteries, vRightInferiorLobeLateralBasalPulmonaryCapillaries, pulse::ExpandedLungsVascularLink::RightPulmonaryArteriesToRightInferiorLobeLateralBasalPulmonaryCapillaries);
    SELiquidCompartmentLink& vRightPulmonaryArteriesToRightInferiorLobePosteriorBasalPulmonaryCapillaries = m_Compartments->CreateLiquidLink(vRightPulmonaryArteries, vRightInferiorLobePosteriorBasalPulmonaryCapillaries, pulse::ExpandedLungsVascularLink::RightPulmonaryArteriesToRightInferiorLobePosteriorBasalPulmonaryCapillaries);

    SELiquidCompartmentLink& vLeftPulmonaryArteriesToLeftInferiorLobePosteriorBasalPulmonaryCapillaries = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftInferiorLobePosteriorBasalPulmonaryCapillaries, pulse::ExpandedLungsVascularLink::LeftPulmonaryArteriesToLeftInferiorLobePosteriorBasalPulmonaryCapillaries);
    SELiquidCompartmentLink& vLeftPulmonaryArteriesToLeftInferiorLobeLateralBasalPulmonaryCapillaries = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftInferiorLobeLateralBasalPulmonaryCapillaries, pulse::ExpandedLungsVascularLink::LeftPulmonaryArteriesToLeftInferiorLobeLateralBasalPulmonaryCapillaries);
    SELiquidCompartmentLink& vLeftPulmonaryArteriesToLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries, pulse::ExpandedLungsVascularLink::LeftPulmonaryArteriesToLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries);
    SELiquidCompartmentLink& vLeftPulmonaryArteriesToLeftInferiorLobeSuperiorPulmonaryCapillaries = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftInferiorLobeSuperiorPulmonaryCapillaries, pulse::ExpandedLungsVascularLink::LeftPulmonaryArteriesToLeftInferiorLobeSuperiorPulmonaryCapillaries);
    SELiquidCompartmentLink& vLeftPulmonaryArteriesToLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries, pulse::ExpandedLungsVascularLink::LeftPulmonaryArteriesToLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries);
    SELiquidCompartmentLink& vLeftPulmonaryArteriesToLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries, pulse::ExpandedLungsVascularLink::LeftPulmonaryArteriesToLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries);
    SELiquidCompartmentLink& vLeftPulmonaryArteriesToLeftSuperiorLobeAnteriorPulmonaryCapillaries = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftSuperiorLobeAnteriorPulmonaryCapillaries, pulse::ExpandedLungsVascularLink::LeftPulmonaryArteriesToLeftSuperiorLobeAnteriorPulmonaryCapillaries);
    SELiquidCompartmentLink& vLeftPulmonaryArteriesToLeftSuperiorLobeApicoposteriorPulmonaryCapillaries = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftSuperiorLobeApicoposteriorPulmonaryCapillaries, pulse::ExpandedLungsVascularLink::LeftPulmonaryArteriesToLeftSuperiorLobeApicoposteriorPulmonaryCapillaries);

    SELiquidCompartmentLink& vRightSuperiorLobeApicalPulmonaryCapillariesToRightPulmonaryVeins = m_Compartments->CreateLiquidLink(vRightSuperiorLobeApicalPulmonaryCapillaries, vRightPulmonaryVeins, pulse::ExpandedLungsVascularLink::RightSuperiorLobeApicalPulmonaryCapillariesToRightPulmonaryVeins);
    SELiquidCompartmentLink& vRightSuperiorLobePosteriorPulmonaryCapillariesToRightPulmonaryVeins = m_Compartments->CreateLiquidLink(vRightSuperiorLobePosteriorPulmonaryCapillaries, vRightPulmonaryVeins, pulse::ExpandedLungsVascularLink::RightSuperiorLobePosteriorPulmonaryCapillariesToRightPulmonaryVeins);
    SELiquidCompartmentLink& vRightSuperiorLobeAnteriorPulmonaryCapillariesToRightPulmonaryVeins = m_Compartments->CreateLiquidLink(vRightSuperiorLobeAnteriorPulmonaryCapillaries, vRightPulmonaryVeins, pulse::ExpandedLungsVascularLink::RightSuperiorLobeAnteriorPulmonaryCapillariesToRightPulmonaryVeins);
    SELiquidCompartmentLink& vRightMiddleLobeLateralPulmonaryCapillariesToRightPulmonaryVeins = m_Compartments->CreateLiquidLink(vRightMiddleLobeLateralPulmonaryCapillaries, vRightPulmonaryVeins, pulse::ExpandedLungsVascularLink::RightMiddleLobeLateralPulmonaryCapillariesToRightPulmonaryVeins);
    SELiquidCompartmentLink& vRightMiddleLobeMedialPulmonaryCapillariesToRightPulmonaryVeins = m_Compartments->CreateLiquidLink(vRightMiddleLobeMedialPulmonaryCapillaries, vRightPulmonaryVeins, pulse::ExpandedLungsVascularLink::RightMiddleLobeMedialPulmonaryCapillariesToRightPulmonaryVeins);
    SELiquidCompartmentLink& vRightInferiorLobeSuperiorPulmonaryCapillariesToRightPulmonaryVeins = m_Compartments->CreateLiquidLink(vRightInferiorLobeSuperiorPulmonaryCapillaries, vRightPulmonaryVeins, pulse::ExpandedLungsVascularLink::RightInferiorLobeSuperiorPulmonaryCapillariesToRightPulmonaryVeins);
    SELiquidCompartmentLink& vRightInferiorLobeMedialBasalPulmonaryCapillariesToRightPulmonaryVeins = m_Compartments->CreateLiquidLink(vRightInferiorLobeMedialBasalPulmonaryCapillaries, vRightPulmonaryVeins, pulse::ExpandedLungsVascularLink::RightInferiorLobeMedialBasalPulmonaryCapillariesToRightPulmonaryVeins);
    SELiquidCompartmentLink& vRightInferiorLobeAnteriorBasalPulmonaryCapillariesToRightPulmonaryVeins = m_Compartments->CreateLiquidLink(vRightInferiorLobeAnteriorBasalPulmonaryCapillaries, vRightPulmonaryVeins, pulse::ExpandedLungsVascularLink::RightInferiorLobeAnteriorBasalPulmonaryCapillariesToRightPulmonaryVeins);
    SELiquidCompartmentLink& vRightInferiorLobeLateralBasalPulmonaryCapillariesToRightPulmonaryVeins = m_Compartments->CreateLiquidLink(vRightInferiorLobeLateralBasalPulmonaryCapillaries, vRightPulmonaryVeins, pulse::ExpandedLungsVascularLink::RightInferiorLobeLateralBasalPulmonaryCapillariesToRightPulmonaryVeins);
    SELiquidCompartmentLink& vRightInferiorLobePosteriorBasalPulmonaryCapillariesToRightPulmonaryVeins = m_Compartments->CreateLiquidLink(vRightInferiorLobePosteriorBasalPulmonaryCapillaries, vRightPulmonaryVeins, pulse::ExpandedLungsVascularLink::RightInferiorLobePosteriorBasalPulmonaryCapillariesToRightPulmonaryVeins);

    SELiquidCompartmentLink& vLeftInferiorLobePosteriorBasalPulmonaryCapillariesToLeftPulmonaryVeins = m_Compartments->CreateLiquidLink(vLeftInferiorLobePosteriorBasalPulmonaryCapillaries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::LeftInferiorLobePosteriorBasalPulmonaryCapillariesToLeftPulmonaryVeins);
    SELiquidCompartmentLink& vLeftInferiorLobeLateralBasalPulmonaryCapillariesToLeftPulmonaryVeins = m_Compartments->CreateLiquidLink(vLeftInferiorLobeLateralBasalPulmonaryCapillaries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::LeftInferiorLobeLateralBasalPulmonaryCapillariesToLeftPulmonaryVeins);
    SELiquidCompartmentLink& vLeftInferiorLobeAnteromedialBasalPulmonaryCapillariesToLeftPulmonaryVeins = m_Compartments->CreateLiquidLink(vLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::LeftInferiorLobeAnteromedialBasalPulmonaryCapillariesToLeftPulmonaryVeins);
    SELiquidCompartmentLink& vLeftInferiorLobeSuperiorPulmonaryCapillariesToLeftPulmonaryVeins = m_Compartments->CreateLiquidLink(vLeftInferiorLobeSuperiorPulmonaryCapillaries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::LeftInferiorLobeSuperiorPulmonaryCapillariesToLeftPulmonaryVeins);
    SELiquidCompartmentLink& vLeftSuperiorLobeInferiorLingulaPulmonaryCapillariesToLeftPulmonaryVeins = m_Compartments->CreateLiquidLink(vLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::LeftSuperiorLobeInferiorLingulaPulmonaryCapillariesToLeftPulmonaryVeins);
    SELiquidCompartmentLink& vLeftSuperiorLobeSuperiorLingulaPulmonaryCapillariesToLeftPulmonaryVeins = m_Compartments->CreateLiquidLink(vLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::LeftSuperiorLobeSuperiorLingulaPulmonaryCapillariesToLeftPulmonaryVeins);
    SELiquidCompartmentLink& vLeftSuperiorLobeAnteriorPulmonaryCapillariesToLeftPulmonaryVeins = m_Compartments->CreateLiquidLink(vLeftSuperiorLobeAnteriorPulmonaryCapillaries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::LeftSuperiorLobeAnteriorPulmonaryCapillariesToLeftPulmonaryVeins);
    SELiquidCompartmentLink& vLeftSuperiorLobeApicoposteriorPulmonaryCapillariesToLeftPulmonaryVeins = m_Compartments->CreateLiquidLink(vLeftSuperiorLobeApicoposteriorPulmonaryCapillaries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::LeftSuperiorLobeApicoposteriorPulmonaryCapillariesToLeftPulmonaryVeins);

    SELiquidCompartmentLink& vRightSuperiorLobeApicalShunt = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::RightSuperiorLobeApicalShunt);
    SELiquidCompartmentLink& vRightSuperiorLobePosteriorShunt = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::RightSuperiorLobePosteriorShunt);
    SELiquidCompartmentLink& vRightSuperiorLobeAnteriorShunt = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::RightSuperiorLobeAnteriorShunt);
    SELiquidCompartmentLink& vRightMiddleLobeLateralShunt = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::RightMiddleLobeLateralShunt);
    SELiquidCompartmentLink& vRightMiddleLobeMedialShunt = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::RightMiddleLobeMedialShunt);
    SELiquidCompartmentLink& vRightInferiorLobeSuperiorShunt = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::RightInferiorLobeSuperiorShunt);
    SELiquidCompartmentLink& vRightInferiorLobeMedialBasalShunt = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::RightInferiorLobeMedialBasalShunt);
    SELiquidCompartmentLink& vRightInferiorLobeAnteriorBasalShunt = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::RightInferiorLobeAnteriorBasalShunt);
    SELiquidCompartmentLink& vRightInferiorLobeLateralBasalShunt = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::RightInferiorLobeLateralBasalShunt);
    SELiquidCompartmentLink& vRightInferiorLobePosteriorBasalShunt = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::RightInferiorLobePosteriorBasalShunt);

    SELiquidCompartmentLink& vLeftInferiorLobePosteriorBasalShunt = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::LeftInferiorLobePosteriorBasalShunt);
    SELiquidCompartmentLink& vLeftInferiorLobeLateralBasalShunt = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::LeftInferiorLobeLateralBasalShunt);
    SELiquidCompartmentLink& vLeftInferiorLobeAnteromedialBasalShunt = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::LeftInferiorLobeAnteromedialBasalShunt);
    SELiquidCompartmentLink& vLeftInferiorLobeSuperiorShunt = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::LeftInferiorLobeSuperiorShunt);
    SELiquidCompartmentLink& vLeftSuperiorLobeInferiorLingulaShunt = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::LeftSuperiorLobeInferiorLingulaShunt);
    SELiquidCompartmentLink& vLeftSuperiorLobeSuperiorLingulaShunt = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::LeftSuperiorLobeSuperiorLingulaShunt);
    SELiquidCompartmentLink& vLeftSuperiorLobeAnteriorShunt = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::LeftSuperiorLobeAnteriorShunt);
    SELiquidCompartmentLink& vLeftSuperiorLobeApicoposteriorShunt = m_Compartments->CreateLiquidLink(vLeftPulmonaryArteries, vLeftPulmonaryVeins, pulse::ExpandedLungsVascularLink::LeftSuperiorLobeApicoposteriorShunt);

    vRightPulmonaryArteriesToRightSuperiorLobeApicalPulmonaryCapillaries.MapPath(         RightPulmonaryArteries1ToRightSuperiorLobeApicalPulmonaryCapillaries);
    vRightPulmonaryArteriesToRightSuperiorLobePosteriorPulmonaryCapillaries.MapPath(      RightPulmonaryArteries1ToRightSuperiorLobePosteriorPulmonaryCapillaries);
    vRightPulmonaryArteriesToRightSuperiorLobeAnteriorPulmonaryCapillaries.MapPath(       RightPulmonaryArteries1ToRightSuperiorLobeAnteriorPulmonaryCapillaries);
    vRightPulmonaryArteriesToRightMiddleLobeLateralPulmonaryCapillaries.MapPath(          RightPulmonaryArteries1ToRightMiddleLobeLateralPulmonaryCapillaries);
    vRightPulmonaryArteriesToRightMiddleLobeMedialPulmonaryCapillaries.MapPath(           RightPulmonaryArteries1ToRightMiddleLobeMedialPulmonaryCapillaries);
    vRightPulmonaryArteriesToRightInferiorLobeSuperiorPulmonaryCapillaries.MapPath(       RightPulmonaryArteries1ToRightInferiorLobeSuperiorPulmonaryCapillaries);
    vRightPulmonaryArteriesToRightInferiorLobeMedialBasalPulmonaryCapillaries.MapPath(    RightPulmonaryArteries1ToRightInferiorLobeMedialBasalPulmonaryCapillaries);
    vRightPulmonaryArteriesToRightInferiorLobeAnteriorBasalPulmonaryCapillaries.MapPath(  RightPulmonaryArteries1ToRightInferiorLobeAnteriorBasalPulmonaryCapillaries);
    vRightPulmonaryArteriesToRightInferiorLobeLateralBasalPulmonaryCapillaries.MapPath(   RightPulmonaryArteries1ToRightInferiorLobeLateralBasalPulmonaryCapillaries);
    vRightPulmonaryArteriesToRightInferiorLobePosteriorBasalPulmonaryCapillaries.MapPath( RightPulmonaryArteries1ToRightInferiorLobePosteriorBasalPulmonaryCapillaries);

    vLeftPulmonaryArteriesToLeftInferiorLobePosteriorBasalPulmonaryCapillaries.MapPath(   LeftPulmonaryArteries1ToLeftInferiorLobePosteriorBasalPulmonaryCapillaries);
    vLeftPulmonaryArteriesToLeftInferiorLobeLateralBasalPulmonaryCapillaries.MapPath(     LeftPulmonaryArteries1ToLeftInferiorLobeLateralBasalPulmonaryCapillaries);
    vLeftPulmonaryArteriesToLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries.MapPath(LeftPulmonaryArteries1ToLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries);
    vLeftPulmonaryArteriesToLeftInferiorLobeSuperiorPulmonaryCapillaries.MapPath(         LeftPulmonaryArteries1ToLeftInferiorLobeSuperiorPulmonaryCapillaries);
    vLeftPulmonaryArteriesToLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries.MapPath(  LeftPulmonaryArteries1ToLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries);
    vLeftPulmonaryArteriesToLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries.MapPath(  LeftPulmonaryArteries1ToLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries);
    vLeftPulmonaryArteriesToLeftSuperiorLobeAnteriorPulmonaryCapillaries.MapPath(         LeftPulmonaryArteries1ToLeftSuperiorLobeAnteriorPulmonaryCapillaries);
    vLeftPulmonaryArteriesToLeftSuperiorLobeApicoposteriorPulmonaryCapillaries.MapPath(   LeftPulmonaryArteries1ToLeftSuperiorLobeApicoposteriorPulmonaryCapillaries);

    vRightSuperiorLobeApicalPulmonaryCapillariesToRightPulmonaryVeins.MapPath(            RightSuperiorLobeApicalPulmonaryCapillariesToRightPulmonaryVeins1);
    vRightSuperiorLobePosteriorPulmonaryCapillariesToRightPulmonaryVeins.MapPath(         RightSuperiorLobePosteriorPulmonaryCapillariesToRightPulmonaryVeins1);
    vRightSuperiorLobeAnteriorPulmonaryCapillariesToRightPulmonaryVeins.MapPath(          RightSuperiorLobeAnteriorPulmonaryCapillariesToRightPulmonaryVeins1);
    vRightMiddleLobeLateralPulmonaryCapillariesToRightPulmonaryVeins.MapPath(             RightMiddleLobeLateralPulmonaryCapillariesToRightPulmonaryVeins1);
    vRightMiddleLobeMedialPulmonaryCapillariesToRightPulmonaryVeins.MapPath(              RightMiddleLobeMedialPulmonaryCapillariesToRightPulmonaryVeins1);
    vRightInferiorLobeSuperiorPulmonaryCapillariesToRightPulmonaryVeins.MapPath(          RightInferiorLobeSuperiorPulmonaryCapillariesToRightPulmonaryVeins1);
    vRightInferiorLobeMedialBasalPulmonaryCapillariesToRightPulmonaryVeins.MapPath(       RightInferiorLobeMedialBasalPulmonaryCapillariesToRightPulmonaryVeins1);
    vRightInferiorLobeAnteriorBasalPulmonaryCapillariesToRightPulmonaryVeins.MapPath(     RightInferiorLobeAnteriorBasalPulmonaryCapillariesToRightPulmonaryVeins1);
    vRightInferiorLobeLateralBasalPulmonaryCapillariesToRightPulmonaryVeins.MapPath(      RightInferiorLobeLateralBasalPulmonaryCapillariesToRightPulmonaryVeins1);
    vRightInferiorLobePosteriorBasalPulmonaryCapillariesToRightPulmonaryVeins.MapPath(    RightInferiorLobePosteriorBasalPulmonaryCapillariesToRightPulmonaryVeins1);

    vLeftInferiorLobePosteriorBasalPulmonaryCapillariesToLeftPulmonaryVeins.MapPath(      LeftInferiorLobePosteriorBasalPulmonaryCapillariesToLeftPulmonaryVeins1);
    vLeftInferiorLobeLateralBasalPulmonaryCapillariesToLeftPulmonaryVeins.MapPath(        LeftInferiorLobeLateralBasalPulmonaryCapillariesToLeftPulmonaryVeins1);
    vLeftInferiorLobeAnteromedialBasalPulmonaryCapillariesToLeftPulmonaryVeins.MapPath(   LeftInferiorLobeAnteromedialBasalPulmonaryCapillariesToLeftPulmonaryVeins1);
    vLeftInferiorLobeSuperiorPulmonaryCapillariesToLeftPulmonaryVeins.MapPath(            LeftInferiorLobeSuperiorPulmonaryCapillariesToLeftPulmonaryVeins1);
    vLeftSuperiorLobeInferiorLingulaPulmonaryCapillariesToLeftPulmonaryVeins.MapPath(     LeftSuperiorLobeInferiorLingulaPulmonaryCapillariesToLeftPulmonaryVeins1);
    vLeftSuperiorLobeSuperiorLingulaPulmonaryCapillariesToLeftPulmonaryVeins.MapPath(     LeftSuperiorLobeSuperiorLingulaPulmonaryCapillariesToLeftPulmonaryVeins1);
    vLeftSuperiorLobeAnteriorPulmonaryCapillariesToLeftPulmonaryVeins.MapPath(            LeftSuperiorLobeAnteriorPulmonaryCapillariesToLeftPulmonaryVeins1);
    vLeftSuperiorLobeApicoposteriorPulmonaryCapillariesToLeftPulmonaryVeins.MapPath(      LeftSuperiorLobeApicoposteriorPulmonaryCapillariesToLeftPulmonaryVeins1);

    vRightSuperiorLobeApicalShunt.MapPath(RightSuperiorLobeApicalShunt);
    vRightSuperiorLobePosteriorShunt.MapPath(RightSuperiorLobePosteriorShunt);
    vRightSuperiorLobeAnteriorShunt.MapPath(RightSuperiorLobeAnteriorShunt);
    vRightMiddleLobeLateralShunt.MapPath(RightMiddleLobeLateralShunt);
    vRightMiddleLobeMedialShunt.MapPath(RightMiddleLobeMedialShunt);
    vRightInferiorLobeSuperiorShunt.MapPath(RightInferiorLobeSuperiorShunt);
    vRightInferiorLobeMedialBasalShunt.MapPath(RightInferiorLobeMedialBasalShunt);
    vRightInferiorLobeAnteriorBasalShunt.MapPath(RightInferiorLobeAnteriorBasalShunt);
    vRightInferiorLobeLateralBasalShunt.MapPath(RightInferiorLobeLateralBasalShunt);
    vRightInferiorLobePosteriorBasalShunt.MapPath(RightInferiorLobePosteriorBasalShunt);

    vLeftInferiorLobePosteriorBasalShunt.MapPath(LeftInferiorLobePosteriorBasalShunt);
    vLeftInferiorLobeLateralBasalShunt.MapPath(LeftInferiorLobeLateralBasalShunt);
    vLeftInferiorLobeAnteromedialBasalShunt.MapPath(LeftInferiorLobeAnteromedialBasalShunt);
    vLeftInferiorLobeSuperiorShunt.MapPath(LeftInferiorLobeSuperiorShunt);
    vLeftSuperiorLobeInferiorLingulaShunt.MapPath(LeftSuperiorLobeInferiorLingulaShunt);
    vLeftSuperiorLobeSuperiorLingulaShunt.MapPath(LeftSuperiorLobeSuperiorLingulaShunt);
    vLeftSuperiorLobeAnteriorShunt.MapPath(LeftSuperiorLobeAnteriorShunt);
    vLeftSuperiorLobeApicoposteriorShunt.MapPath(LeftSuperiorLobeApicoposteriorShunt);

    SELiquidCompartmentGraph& gCardiovascular = m_Compartments->GetCardiovascularGraph();
    gCardiovascular.AddCompartment(vRightHeart);
    gCardiovascular.AddCompartment(vRightPulmonaryArteries);
    gCardiovascular.AddCompartment(vLeftPulmonaryArteries);
    //gCardiovascular.AddCompartment(vRightPulmonaryCapillaries);
    //gCardiovascular.AddCompartment(vLeftPulmonaryCapillaries);
    gCardiovascular.AddCompartment(vRightPulmonaryVeins);
    gCardiovascular.AddCompartment(vLeftPulmonaryVeins);
    gCardiovascular.AddCompartment(vLeftHeart);
    gCardiovascular.AddCompartment(vAorta);
    gCardiovascular.AddCompartment(vBrain);
    gCardiovascular.AddCompartment(vBone);
    gCardiovascular.AddCompartment(vFat);
    gCardiovascular.AddCompartment(vLargeIntestine);
    gCardiovascular.AddCompartment(vLiver);
    gCardiovascular.AddCompartment(vLeftArm);
    gCardiovascular.AddCompartment(vLeftKidney);
    gCardiovascular.AddCompartment(vLeftLeg);
    gCardiovascular.AddCompartment(vMuscle);
    gCardiovascular.AddCompartment(vMyocardium);
    gCardiovascular.AddCompartment(vRightArm);
    gCardiovascular.AddCompartment(vRightKidney);
    gCardiovascular.AddCompartment(vRightLeg);
    gCardiovascular.AddCompartment(vSkin);
    gCardiovascular.AddCompartment(vSmallIntestine);
    gCardiovascular.AddCompartment(vSplanchnic);
    gCardiovascular.AddCompartment(vSpleen);
    gCardiovascular.AddCompartment(vVenaCava);

    gCardiovascular.AddCompartment(vRightSuperiorLobeApicalPulmonaryCapillaries);
    gCardiovascular.AddCompartment(vRightSuperiorLobePosteriorPulmonaryCapillaries);
    gCardiovascular.AddCompartment(vRightSuperiorLobeAnteriorPulmonaryCapillaries);
    gCardiovascular.AddCompartment(vRightMiddleLobeLateralPulmonaryCapillaries);
    gCardiovascular.AddCompartment(vRightMiddleLobeMedialPulmonaryCapillaries);
    gCardiovascular.AddCompartment(vRightInferiorLobeSuperiorPulmonaryCapillaries);
    gCardiovascular.AddCompartment(vRightInferiorLobeMedialBasalPulmonaryCapillaries);
    gCardiovascular.AddCompartment(vRightInferiorLobeAnteriorBasalPulmonaryCapillaries);
    gCardiovascular.AddCompartment(vRightInferiorLobeLateralBasalPulmonaryCapillaries);
    gCardiovascular.AddCompartment(vRightInferiorLobePosteriorBasalPulmonaryCapillaries);

    gCardiovascular.AddCompartment(vLeftInferiorLobePosteriorBasalPulmonaryCapillaries);
    gCardiovascular.AddCompartment(vLeftInferiorLobeLateralBasalPulmonaryCapillaries);
    gCardiovascular.AddCompartment(vLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries);
    gCardiovascular.AddCompartment(vLeftInferiorLobeSuperiorPulmonaryCapillaries);
    gCardiovascular.AddCompartment(vLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries);
    gCardiovascular.AddCompartment(vLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries);
    gCardiovascular.AddCompartment(vLeftSuperiorLobeAnteriorPulmonaryCapillaries);
    gCardiovascular.AddCompartment(vLeftSuperiorLobeApicoposteriorPulmonaryCapillaries);

    gCardiovascular.AddLink(vVenaCavaToRightHeart);
    gCardiovascular.AddLink(vRightHeartToLeftPulmonaryArteries);
    //gCardiovascular.AddLink(vLeftPulmonaryArteriesToCapillaries);
    //gCardiovascular.AddLink(vLeftPulmonaryArteriesToVeins);
    //gCardiovascular.AddLink(vLeftPulmonaryCapillariesToVeins);
    gCardiovascular.AddLink(vLeftPulmonaryVeinsToLeftHeart);
    gCardiovascular.AddLink(vRightHeartToRightPulmonaryArteries);
    //gCardiovascular.AddLink(vRightPulmonaryArteriesToCapillaries);
    //gCardiovascular.AddLink(vRightPulmonaryArteriesToVeins);
    //gCardiovascular.AddLink(vRightPulmonaryCapillariesToVeins);
    gCardiovascular.AddLink(vRightPulmonaryVeinsToLeftHeart);
    gCardiovascular.AddLink(vLeftHeartToAorta);
    gCardiovascular.AddLink(vAortaToBone);
    gCardiovascular.AddLink(vBoneToVenaCava);
    gCardiovascular.AddLink(vAortaToBrain);
    gCardiovascular.AddLink(vBrainToVenaCava);
    gCardiovascular.AddLink(vAortaToFat);
    gCardiovascular.AddLink(vFatToVenaCava);
    gCardiovascular.AddLink(vAortaToLargeIntestine);
    gCardiovascular.AddLink(vLargeIntestineToLiver);
    gCardiovascular.AddLink(vAortaToLeftArm);
    gCardiovascular.AddLink(vLeftArmToVenaCava);
    gCardiovascular.AddLink(vAortaToLeftKidney);
    gCardiovascular.AddLink(vLeftKidneyToVenaCava);
    gCardiovascular.AddLink(vAortaToLeftLeg);
    gCardiovascular.AddLink(vLeftLegToVenaCava);
    gCardiovascular.AddLink(vAortaToLiver);
    gCardiovascular.AddLink(vLiverToVenaCava);
    gCardiovascular.AddLink(vAortaToMuscle);
    gCardiovascular.AddLink(vMuscleToVenaCava);
    gCardiovascular.AddLink(vAortaToMyocardium);
    gCardiovascular.AddLink(vMyocardiumToVenaCava);
    gCardiovascular.AddLink(vAortaToRightArm);
    gCardiovascular.AddLink(vRightArmToVenaCava);
    gCardiovascular.AddLink(vAortaToRightKidney);
    gCardiovascular.AddLink(vRightKidneyToVenaCava);
    gCardiovascular.AddLink(vAortaToRightLeg);
    gCardiovascular.AddLink(vRightLegToVenaCava);
    gCardiovascular.AddLink(vAortaToSkin);
    gCardiovascular.AddLink(vSkinToVenaCava);
    gCardiovascular.AddLink(vAortaToSmallIntestine);
    gCardiovascular.AddLink(vSmallIntestineToLiver);
    gCardiovascular.AddLink(vAortaToSplanchnic);
    gCardiovascular.AddLink(vSplanchnicToLiver);
    gCardiovascular.AddLink(vAortaToSpleen);
    gCardiovascular.AddLink(vSpleenToLiver);
    gCardiovascular.AddLink(vVenaCavaIV);

    gCardiovascular.AddLink(vRightPulmonaryArteriesToRightSuperiorLobeApicalPulmonaryCapillaries);
    gCardiovascular.AddLink(vRightPulmonaryArteriesToRightSuperiorLobePosteriorPulmonaryCapillaries);
    gCardiovascular.AddLink(vRightPulmonaryArteriesToRightSuperiorLobeAnteriorPulmonaryCapillaries);
    gCardiovascular.AddLink(vRightPulmonaryArteriesToRightMiddleLobeLateralPulmonaryCapillaries);
    gCardiovascular.AddLink(vRightPulmonaryArteriesToRightMiddleLobeMedialPulmonaryCapillaries);
    gCardiovascular.AddLink(vRightPulmonaryArteriesToRightInferiorLobeSuperiorPulmonaryCapillaries);
    gCardiovascular.AddLink(vRightPulmonaryArteriesToRightInferiorLobeMedialBasalPulmonaryCapillaries);
    gCardiovascular.AddLink(vRightPulmonaryArteriesToRightInferiorLobeAnteriorBasalPulmonaryCapillaries);
    gCardiovascular.AddLink(vRightPulmonaryArteriesToRightInferiorLobeLateralBasalPulmonaryCapillaries);
    gCardiovascular.AddLink(vRightPulmonaryArteriesToRightInferiorLobePosteriorBasalPulmonaryCapillaries);

    gCardiovascular.AddLink(vLeftPulmonaryArteriesToLeftInferiorLobePosteriorBasalPulmonaryCapillaries);
    gCardiovascular.AddLink(vLeftPulmonaryArteriesToLeftInferiorLobeLateralBasalPulmonaryCapillaries);
    gCardiovascular.AddLink(vLeftPulmonaryArteriesToLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries);
    gCardiovascular.AddLink(vLeftPulmonaryArteriesToLeftInferiorLobeSuperiorPulmonaryCapillaries);
    gCardiovascular.AddLink(vLeftPulmonaryArteriesToLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries);
    gCardiovascular.AddLink(vLeftPulmonaryArteriesToLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries);
    gCardiovascular.AddLink(vLeftPulmonaryArteriesToLeftSuperiorLobeAnteriorPulmonaryCapillaries);
    gCardiovascular.AddLink(vLeftPulmonaryArteriesToLeftSuperiorLobeApicoposteriorPulmonaryCapillaries);

    gCardiovascular.AddLink(vRightSuperiorLobeApicalPulmonaryCapillariesToRightPulmonaryVeins);
    gCardiovascular.AddLink(vRightSuperiorLobePosteriorPulmonaryCapillariesToRightPulmonaryVeins);
    gCardiovascular.AddLink(vRightSuperiorLobeAnteriorPulmonaryCapillariesToRightPulmonaryVeins);
    gCardiovascular.AddLink(vRightMiddleLobeLateralPulmonaryCapillariesToRightPulmonaryVeins);
    gCardiovascular.AddLink(vRightMiddleLobeMedialPulmonaryCapillariesToRightPulmonaryVeins);
    gCardiovascular.AddLink(vRightInferiorLobeSuperiorPulmonaryCapillariesToRightPulmonaryVeins);
    gCardiovascular.AddLink(vRightInferiorLobeMedialBasalPulmonaryCapillariesToRightPulmonaryVeins);
    gCardiovascular.AddLink(vRightInferiorLobeAnteriorBasalPulmonaryCapillariesToRightPulmonaryVeins);
    gCardiovascular.AddLink(vRightInferiorLobeLateralBasalPulmonaryCapillariesToRightPulmonaryVeins);
    gCardiovascular.AddLink(vRightInferiorLobePosteriorBasalPulmonaryCapillariesToRightPulmonaryVeins);

    gCardiovascular.AddLink(vLeftInferiorLobePosteriorBasalPulmonaryCapillariesToLeftPulmonaryVeins);
    gCardiovascular.AddLink(vLeftInferiorLobeLateralBasalPulmonaryCapillariesToLeftPulmonaryVeins);
    gCardiovascular.AddLink(vLeftInferiorLobeAnteromedialBasalPulmonaryCapillariesToLeftPulmonaryVeins);
    gCardiovascular.AddLink(vLeftInferiorLobeSuperiorPulmonaryCapillariesToLeftPulmonaryVeins);
    gCardiovascular.AddLink(vLeftSuperiorLobeInferiorLingulaPulmonaryCapillariesToLeftPulmonaryVeins);
    gCardiovascular.AddLink(vLeftSuperiorLobeSuperiorLingulaPulmonaryCapillariesToLeftPulmonaryVeins);
    gCardiovascular.AddLink(vLeftSuperiorLobeAnteriorPulmonaryCapillariesToLeftPulmonaryVeins);
    gCardiovascular.AddLink(vLeftSuperiorLobeApicoposteriorPulmonaryCapillariesToLeftPulmonaryVeins);

    gCardiovascular.AddLink(vRightSuperiorLobeApicalShunt);
    gCardiovascular.AddLink(vRightSuperiorLobePosteriorShunt);
    gCardiovascular.AddLink(vRightSuperiorLobeAnteriorShunt);
    gCardiovascular.AddLink(vRightMiddleLobeLateralShunt);
    gCardiovascular.AddLink(vRightMiddleLobeMedialShunt);
    gCardiovascular.AddLink(vRightInferiorLobeSuperiorShunt);
    gCardiovascular.AddLink(vRightInferiorLobeMedialBasalShunt);
    gCardiovascular.AddLink(vRightInferiorLobeAnteriorBasalShunt);
    gCardiovascular.AddLink(vRightInferiorLobeLateralBasalShunt);
    gCardiovascular.AddLink(vRightInferiorLobePosteriorBasalShunt);

    gCardiovascular.AddLink(vLeftInferiorLobePosteriorBasalShunt);
    gCardiovascular.AddLink(vLeftInferiorLobeLateralBasalShunt);
    gCardiovascular.AddLink(vLeftInferiorLobeAnteromedialBasalShunt);
    gCardiovascular.AddLink(vLeftInferiorLobeSuperiorShunt);
    gCardiovascular.AddLink(vLeftSuperiorLobeInferiorLingulaShunt);
    gCardiovascular.AddLink(vLeftSuperiorLobeSuperiorLingulaShunt);
    gCardiovascular.AddLink(vLeftSuperiorLobeAnteriorShunt);
    gCardiovascular.AddLink(vLeftSuperiorLobeApicoposteriorShunt);

    // Remove modifiers of paths we removed
    m_Config->GetModifiers().erase(pulse::CardiovascularPath::RightPulmonaryArteries1ToRightPulmonaryCapillaries1);
    m_Config->GetModifiers().erase(pulse::CardiovascularPath::RightPulmonaryCapillaries1ToRightPulmonaryVeins1);
    m_Config->GetModifiers().erase(pulse::CardiovascularPath::RightPulmonaryArteries1ToRightPulmonaryVeins1);
    m_Config->GetModifiers().erase(pulse::CardiovascularPath::LeftPulmonaryArteries1ToLeftPulmonaryCapillaries1);
    m_Config->GetModifiers().erase(pulse::CardiovascularPath::LeftPulmonaryCapillaries1ToLeftPulmonaryVeins1);
    m_Config->GetModifiers().erase(pulse::CardiovascularPath::LeftPulmonaryArteries1ToLeftPulmonaryVeins1);

    gCardiovascular.StateChange();

    SELiquidCompartmentGraph& gCombinedCardiovascular = m_Compartments->GetActiveCardiovascularGraph();
    gCombinedCardiovascular.AddGraph(gCardiovascular);
    gCombinedCardiovascular.StateChange();

    totalLeftVolume_mL = m_Compartments->GetLiquidCompartment(pulse::VascularCompartment::LeftPulmonaryCapillaries)->GetVolume(VolumeUnit::mL);
    totalRightVolume_mL = m_Compartments->GetLiquidCompartment(pulse::VascularCompartment::RightPulmonaryCapillaries)->GetVolume(VolumeUnit::mL);
  }
}