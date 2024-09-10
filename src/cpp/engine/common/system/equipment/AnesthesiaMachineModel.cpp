/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/PulseConfiguration.h"
#include "engine/common/controller/SubstanceManager.h"
#include "engine/common/controller/CircuitManager.h"
#include "engine/common/controller/CompartmentManager.h"
#include "engine/common/system/equipment/AnesthesiaMachineModel.h"

#include "cdm/system/equipment/anesthesia_machine/SEAnesthesiaMachineOxygenBottle.h"
#include "cdm/system/equipment/anesthesia_machine/SEAnesthesiaMachineChamber.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineConfiguration.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineOxygenWallPortPressureLoss.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineOxygenTankPressureLoss.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineExpiratoryValveLeak.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineExpiratoryValveObstruction.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineInspiratoryValveLeak.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineInspiratoryValveObstruction.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineMaskLeak.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineSodaLimeFailure.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineTubeCuffLeak.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineVaporizerFailure.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineVentilatorPressureLoss.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineYPieceDisconnect.h"
#include "cdm/engine/SEActionManager.h"
#include "cdm/engine/SEEquipmentActionCollection.h"
#include "cdm/engine/SEPatientActionCollection.h"
#include "cdm/engine/SEEventManager.h"
#include "cdm/circuit/fluid/SEFluidCircuit.h"
#include "cdm/compartment/fluid/SEGasCompartment.h"
#include "cdm/compartment/substances/SEGasSubstanceQuantity.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarPressureTimePerVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/utils/GeneralMath.h"

/*
========================
  Constructors
========================
*/

namespace pulse
{
  AnesthesiaMachineModel::AnesthesiaMachineModel(Data& data) : AnesthesiaMachineSystem(data.GetLogger()), Model(data)
  {
    Clear();
  }

  AnesthesiaMachineModel::~AnesthesiaMachineModel()
  {
    Clear();
  }

  void AnesthesiaMachineModel::Clear()
  {
    AnesthesiaMachineSystem::Clear();
    m_actions = nullptr;
    m_ambient = nullptr;
    m_ambientCO2 = nullptr;
    m_ambientN2 = nullptr;
    m_ambientO2 = nullptr;
    m_gasSource = nullptr;
    m_gasSourceCO2 = nullptr;
    m_gasSourceN2 = nullptr;
    m_gasSourceO2 = nullptr;
    m_scrubber = nullptr;
    m_scubberCO2 = nullptr;
    m_scrubberN2 = nullptr;
    m_pAnesthesiaConnectionToEnvironment = nullptr;
    m_pYPieceToExpiratoryLimb = nullptr;
    m_pGasSourceToGasInlet = nullptr;
    m_pInspiratoryLimbToYPiece = nullptr;
    m_pSelectorToReliefValve = nullptr;
    m_pEnvironmentToReliefValve = nullptr;
    m_pEnvironmentToVentilator = nullptr;
    m_pEnvironmentToGasSource = nullptr;
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Initializes system properties to valid homeostatic values.
  //--------------------------------------------------------------------------------------------------
  void AnesthesiaMachineModel::Initialize()
  {
    Model::Initialize();

    SetConnection(eSwitch::Off);
    GetInletFlow().SetValue(5.0, VolumePerTimeUnit::L_Per_min);
    GetRespiratoryRate().SetValue(12.0, FrequencyUnit::Per_min);
    GetPositiveEndExpiratoryPressure().SetValue(3.0, PressureUnit::cmH2O);
    GetInspiratoryExpiratoryRatio().SetValue(0.5);
    GetOxygenFraction().SetValue(0.5);
    SetOxygenSource(eAnesthesiaMachine_OxygenSource::Wall);
    SetPrimaryGas(eAnesthesiaMachine_PrimaryGas::Nitrogen);
    GetPeakInspiratoryPressure().SetValue(15.0, PressureUnit::cmH2O);
    GetOxygenBottleOne().GetVolume().SetValue(660.0, VolumeUnit::L);
    GetOxygenBottleTwo().GetVolume().SetValue(660.0, VolumeUnit::L);
    GetReliefValvePressure().SetValue(100.0, PressureUnit::cmH2O);

    m_inhaling = true;
    m_inspirationTime_s = 0.0;
    m_O2InletVolumeFraction = 0.0;
    m_totalBreathingCycleTime_s = 0.0;
    m_currentbreathingCycleTime_s = 0.0;

    StateChange();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Initializes parameters for the anesthesia machine class
  ///
  /// \details
  /// Initializes member variables and system level values on the common data model.
  //--------------------------------------------------------------------------------------------------
  void AnesthesiaMachineModel::SetUp()
  {
    m_actions = &m_data.GetActions().GetEquipmentActions();
    m_dValveOpenResistance_cmH2O_s_Per_L = m_data.GetConfiguration().GetMachineOpenResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    m_dValveClosedResistance_cmH2O_s_Per_L = m_data.GetConfiguration().GetMachineClosedResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    m_dSwitchOpenResistance_cmH2O_s_Per_L = m_data.GetConfiguration().GetDefaultOpenFlowResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    m_dSwitchClosedResistance_cmH2O_s_Per_L = m_data.GetConfiguration().GetDefaultClosedFlowResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);

    // Compartments
    m_ambient = m_data.GetCompartments().GetGasCompartment(pulse::EnvironmentCompartment::Ambient);
    m_ambientCO2 = m_ambient->GetSubstanceQuantity(m_data.GetSubstances().GetCO2());
    m_ambientN2 = m_ambient->GetSubstanceQuantity(m_data.GetSubstances().GetN2());
    m_ambientO2 = m_ambient->GetSubstanceQuantity(m_data.GetSubstances().GetO2());

    m_gasSource = m_data.GetCompartments().GetGasCompartment(pulse::AnesthesiaMachineCompartment::GasSource);
    m_gasSourceCO2 = m_gasSource->GetSubstanceQuantity(m_data.GetSubstances().GetCO2());
    m_gasSourceN2 = m_gasSource->GetSubstanceQuantity(m_data.GetSubstances().GetN2());
    m_gasSourceO2 = m_gasSource->GetSubstanceQuantity(m_data.GetSubstances().GetO2());

    m_scrubber = m_data.GetCompartments().GetGasCompartment(pulse::AnesthesiaMachineCompartment::Scrubber);
    m_scubberCO2 = m_scrubber->GetSubstanceQuantity(m_data.GetSubstances().GetCO2());
    m_scrubberN2 = m_scrubber->GetSubstanceQuantity(m_data.GetSubstances().GetN2());

    // Circuit Nodes
    m_nVentilator = m_data.GetCircuits().GetAnesthesiaMachineCircuit().GetNode(pulse::AnesthesiaMachineNode::Ventilator);

    // Circuit Paths
    m_pAnesthesiaConnectionToEnvironment = m_data.GetCircuits().GetAnesthesiaMachineCircuit().GetPath(pulse::AnesthesiaMachinePath::ConnectionToEnvironment);
    m_pYPieceToExpiratoryLimb = m_data.GetCircuits().GetAnesthesiaMachineCircuit().GetPath(pulse::AnesthesiaMachinePath::YPieceToExpiratoryLimb);
    m_pGasSourceToGasInlet = m_data.GetCircuits().GetAnesthesiaMachineCircuit().GetPath(pulse::AnesthesiaMachinePath::GasSourceToGasInlet);
    m_pInspiratoryLimbToYPiece = m_data.GetCircuits().GetAnesthesiaMachineCircuit().GetPath(pulse::AnesthesiaMachinePath::InspiratoryLimbToYPiece);
    m_pSelectorToReliefValve = m_data.GetCircuits().GetAnesthesiaMachineCircuit().GetPath(pulse::AnesthesiaMachinePath::SelectorToReliefValve);
    m_pEnvironmentToReliefValve = m_data.GetCircuits().GetAnesthesiaMachineCircuit().GetPath(pulse::AnesthesiaMachinePath::EnvironmentToReliefValve);
    m_pEnvironmentToVentilator = m_data.GetCircuits().GetAnesthesiaMachineCircuit().GetPath(pulse::AnesthesiaMachinePath::EnvironmentToVentilator);
    m_pExpiratoryLimbToSelector = m_data.GetCircuits().GetAnesthesiaMachineCircuit().GetPath(pulse::AnesthesiaMachinePath::ExpiratoryLimbToSelector);
    m_pSelectorToScrubber = m_data.GetCircuits().GetAnesthesiaMachineCircuit().GetPath(pulse::AnesthesiaMachinePath::SelectorToScrubber);
    m_pEnvironmentToGasSource = m_data.GetCircuits().GetAnesthesiaMachineCircuit().GetPath(pulse::AnesthesiaMachinePath::EnvironmentToGasSource);
  }

  void AnesthesiaMachineModel::StateChange()
  {
    UpdateAirwayMode();
    if (m_data.GetAirwayMode() != eAirwayMode::AnesthesiaMachine)
    {
      // TurnOff();
      return;
    }
    if (HasLeftChamber() && GetLeftChamber().GetState() == eSwitch::On)
    {
      if (GetLeftChamber().HasSubstance())
        m_data.GetSubstances().AddActiveSubstance(*m_LeftChamber->GetSubstance());
      else
        Error("Active left chamber was not provided a substance, chamber will not be used.");

      if (!GetLeftChamber().HasSubstanceFraction())
        Error("Active left chamber was not provided a substance fraction, chamber will not be used.");
    }
    if (HasRightChamber() && GetRightChamber().GetState() == eSwitch::On)
    {
      if (GetRightChamber().HasSubstance())
        m_data.GetSubstances().AddActiveSubstance(*m_RightChamber->GetSubstance());
      else
        Error("Active right chamber was not provided a substance, chamber will not be used.");

      if (!GetRightChamber().HasSubstanceFraction())
        Error("Active right chamber was not provided a substance fraction, chamber will not be used.");
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Connect to the patient via the ventilator mask, an endotracheal tube, or no connection
  ///
  /// \param  enumAnesthesiaMachineConnection  
  /// Connectoin type : Mask, tube, or off
  /// 
  /// \details
  /// If the enum is set to mask, then the mask is applied to the face
  /// If the enum is set to tube, then the machine is connected to the tube
  /// If the enum is set to off, the airway mode is set to free.
  //--------------------------------------------------------------------------------------------------
  void AnesthesiaMachineModel::UpdateAirwayMode()
  {
    eSwitch c = GetConnection();
    if (c == eSwitch::On)
    {
      m_data.SetAirwayMode(eAirwayMode::AnesthesiaMachine);
      return;
    }
    else if (c == eSwitch::Off && m_data.GetAirwayMode() == eAirwayMode::AnesthesiaMachine)
    {
      // Make sure we are active to make sure we go back to free
      m_data.SetAirwayMode(eAirwayMode::Free);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Anesthesia machine preprocess function
  ///
  /// \details
  /// Checks the anesthesia machine settings to ensure all are within the acceptable ranges. 
  /// Processes all equipment failures.
  /// Verifies the oxygen source.
  /// Processes any failures with the oxygen source. 
  /// Calculates the gas inlet volume fractions based on equipment failures and gas sources.
  //--------------------------------------------------------------------------------------------------
  void AnesthesiaMachineModel::PreProcess()
  {
    if (m_actions->HasAnesthesiaMachineConfiguration())
    {
      ProcessConfiguration(m_actions->GetAnesthesiaMachineConfiguration(), m_data.GetSubstances());
      m_actions->RemoveAnesthesiaMachineConfiguration();
      StateChange();
    }

    //Do nothing if the machine is off and not initialized
    if (GetConnection() == eSwitch::Off)
    {
      m_inhaling = true;
      m_currentbreathingCycleTime_s = 0.0;
      return;
    }

    CalculateCyclePhase();
    CalculateSourceStatus();
    CalculateValveResistances();
    CalculateEquipmentLeak();
    CalculateVentilatorPressure();
    CalculateGasSourceSubstances();
    CalculateGasSourceResistance();
    CheckReliefValve();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Anesthesia machine process function
  ///
  /// \details
  /// The current Pulse implementation has no specific process functionality for the anesthesia machine.
  /// Anesthesia machine processing is currently done in the Respiratory System with the combined circuit methodology.
  //--------------------------------------------------------------------------------------------------
  void AnesthesiaMachineModel::Process(bool /*solve_and_transport*/)
  {
    if (GetConnection() == eSwitch::On)
      CalculateScrubber();
    ComputeExposedModelParameters();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Anesthesia machine postprocess function
  ///
  /// \details
  /// The substance volumes and the volume fractions are updated for all of the nodes in the anesthesia machine
  /// circuit during post process. 
  /// The ventilator volumes are updated based on the previously calculated nodal analysis.
  //--------------------------------------------------------------------------------------------------
  void AnesthesiaMachineModel::PostProcess(bool /*solve_and_transport*/)
  {

  }

  void AnesthesiaMachineModel::ComputeExposedModelParameters()
  {

  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates the carbon dioxide removed by the scrubber
  ///
  /// \details
  /// Calculates the amount of carbon dioxide removed from the anesthesia machine by the scrubber. If a soda lime 
  /// failure is present, the full amount of carbon dioxide is not removed. The amount removed is dependent on the 
  /// severity of the failure. The substance volume in the scrubber node is decremented based on the amount of 
  /// carbon dioxide removed. This function occurs during anesthesia machine preprocess.
  //--------------------------------------------------------------------------------------------------
  void AnesthesiaMachineModel::CalculateScrubber()
  {
    // Calculate incremental volume changes due to scrubber
    double CO2Volume = 0.0;

    if (m_actions->HasAnesthesiaMachineSodaLimeFailure())
    {
      double SodaLimeFailSeverity = m_actions->GetAnesthesiaMachineSodaLimeFailure().GetSeverity().GetValue();
      CO2Volume = SodaLimeFailSeverity * m_scubberCO2->GetVolume(VolumeUnit::L);
    }

    double CO2PreviousVolume = m_scubberCO2->GetVolume(VolumeUnit::L);
    m_scrubberN2->GetVolume().Increment(CO2PreviousVolume - CO2Volume, VolumeUnit::L);
    m_scubberCO2->GetVolume().SetValue(CO2Volume, VolumeUnit::L);
    m_scrubber->Balance(BalanceGasBy::Volume);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Updates the volume fractions for the gas inlet node
  ///
  /// \details
  /// The final step of the anesthesia machine preprocess is to update the volume fractions for the gas inlet node.
  /// The gas mixture is determined based on the right and left chamber activity and specified substances and the oxygen percent
  /// setting on the anesthesia machine. The oxygen source (bottle and wall) are checked to ensure no equipment failures exist. 
  /// The volume fractions are adjusted according to gas composition, the sources, and any failures present. 
  //--------------------------------------------------------------------------------------------------
  void AnesthesiaMachineModel::CalculateGasSourceSubstances()
  {
    double LeftInhaledAgentVolumeFraction = 0.0;
    double RightInhaledAgentVolumeFraction = 0.0;
    //Vaporizer Failure
    double VaporizerFailureSeverity = 0.0;

    if (m_actions->HasAnesthesiaMachineVaporizerFailure())
    {
      VaporizerFailureSeverity = m_actions->GetAnesthesiaMachineVaporizerFailure().GetSeverity().GetValue();
    }
    if (GetLeftChamber().GetState() == eSwitch::On && GetLeftChamber().HasSubstance() && GetLeftChamber().HasSubstanceFraction())
    {
      SEGasSubstanceQuantity* gasSrcSubQ = m_gasSource->GetSubstanceQuantity(*GetLeftChamber().GetSubstance());
      LeftInhaledAgentVolumeFraction = GetLeftChamber().GetSubstanceFraction().GetValue();
      LeftInhaledAgentVolumeFraction = LeftInhaledAgentVolumeFraction * (1 - VaporizerFailureSeverity);
      gasSrcSubQ->GetVolumeFraction().SetValue(LeftInhaledAgentVolumeFraction);
    }
    if (GetRightChamber().GetState() == eSwitch::On && GetRightChamber().HasSubstance() && GetRightChamber().HasSubstanceFraction())
    {
      SEGasSubstanceQuantity* gasSrcSubQ = m_gasSource->GetSubstanceQuantity(*GetRightChamber().GetSubstance());
      RightInhaledAgentVolumeFraction = GetRightChamber().GetSubstanceFraction().GetValue();
      RightInhaledAgentVolumeFraction = RightInhaledAgentVolumeFraction * (1 - VaporizerFailureSeverity);
      gasSrcSubQ->GetVolumeFraction().SetValue(RightInhaledAgentVolumeFraction);
    }

    double RemainingVolumeFraction = 1.0 - (LeftInhaledAgentVolumeFraction + RightInhaledAgentVolumeFraction);

    //Check the O2 Source
    //Note: You're only allowed to use one at a time
    if (GetOxygenSource() == eAnesthesiaMachine_OxygenSource::Wall)
    {
      if (m_actions->HasAnesthesiaMachineOxygenWallPortPressureLoss())
      {
        m_O2InletVolumeFraction = 0.0;
      }
      else
      {
        m_O2InletVolumeFraction = RemainingVolumeFraction * GetOxygenFraction().GetValue();
      }
    }
    else if (GetOxygenSource() == eAnesthesiaMachine_OxygenSource::BottleOne || GetOxygenSource() == eAnesthesiaMachine_OxygenSource::BottleTwo)
    {
      if (m_actions->HasAnesthesiaMachineOxygenTankPressureLoss())
      {
        m_O2InletVolumeFraction = 0.0;
      }
      else
      {
        m_O2InletVolumeFraction = RemainingVolumeFraction * GetOxygenFraction().GetValue();
      }
    }

    //Check if the inlet bottle is empty
    if (GetOxygenSource() == eAnesthesiaMachine_OxygenSource::BottleOne)
    {
      if (GetOxygenBottleOne().GetVolume().GetValue(VolumeUnit::L) <= 0.0)
      {
        m_O2InletVolumeFraction = 0.0;
      }
    }
    else if (GetOxygenSource() == eAnesthesiaMachine_OxygenSource::BottleTwo)
    {
      if (GetOxygenBottleTwo().GetVolume().GetValue(VolumeUnit::L) <= 0.0)
      {
        m_O2InletVolumeFraction = 0.0;
      }
    }

    double dO2VolumeFraction = 0.0;
    double dCO2VolumeFraction = 0.0;

    ///\todo: Do this stuff more generically by looping over all substances

    if (GetPrimaryGas() == eAnesthesiaMachine_PrimaryGas::Air)
    {
      dO2VolumeFraction = RemainingVolumeFraction * m_ambientO2->GetVolumeFraction().GetValue();
      dCO2VolumeFraction = RemainingVolumeFraction * m_ambientCO2->GetVolumeFraction().GetValue();
    }
    //else Nitrogen, no additional O2 or CO2

    dO2VolumeFraction += m_O2InletVolumeFraction;
    double dN2VolumeFraction = 1.0 - dO2VolumeFraction - dCO2VolumeFraction - LeftInhaledAgentVolumeFraction - RightInhaledAgentVolumeFraction;

    //Make sure we don't get a super small negative value
    dN2VolumeFraction = MAX(dN2VolumeFraction, 0.0);

    m_gasSourceO2->GetVolumeFraction().SetValue(dO2VolumeFraction);
    m_gasSourceCO2->GetVolumeFraction().SetValue(dCO2VolumeFraction);
    m_gasSourceN2->GetVolumeFraction().SetValue(dN2VolumeFraction);
    m_gasSource->Balance(BalanceGasBy::VolumeFraction);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Updates the circuit to supply gas at the flow setting.
  ///
  /// \details
  /// Calculates a resistance to mimic the flow knob based on the constant, large pressure and flow setting.
  //--------------------------------------------------------------------------------------------------
  void AnesthesiaMachineModel::CalculateGasSourceResistance()
  {
    double flow_L_Per_s = GetInletFlow().GetValue(VolumePerTimeUnit::L_Per_s);

    //Determine flow control resistance
    //Assume pressure outside tank is comparatively approximately ambient
    double tankPressure_cmH2O = m_pEnvironmentToGasSource->GetNextPressureSource(PressureUnit::cmH2O);
    double resistance_cmH2O_s_Per_L = tankPressure_cmH2O / flow_L_Per_s;
    resistance_cmH2O_s_Per_L = LIMIT(resistance_cmH2O_s_Per_L, m_dSwitchClosedResistance_cmH2O_s_Per_L, m_dSwitchOpenResistance_cmH2O_s_Per_L);

    //Apply flow
    m_pGasSourceToGasInlet->GetNextResistance().SetValue(resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Checks the status of the oxygen source.
  ///
  /// \details
  /// Checks the status of the oxygen source. If an oxygen bottle is in use, ensures that the bottle still has oxygen. 
  /// If the oxygen bottle no longer has oxygen, then an event is sent to the log notifying the user.
  //--------------------------------------------------------------------------------------------------
  void AnesthesiaMachineModel::CalculateSourceStatus()
  {
    double dFlow_LPerS = 0.0;
    if (m_pGasSourceToGasInlet->HasFlow())
    {
      dFlow_LPerS = m_pGasSourceToGasInlet->GetFlow(VolumePerTimeUnit::L_Per_s);
    }

    if (GetOxygenSource() == eAnesthesiaMachine_OxygenSource::BottleOne)
    {
      double dBottle1Volume_L = GetOxygenBottleOne().GetVolume().GetValue(VolumeUnit::L);
      if (dBottle1Volume_L > 0.0)
      {
        dBottle1Volume_L -= m_data.GetTimeStep_s() * dFlow_LPerS * m_O2InletVolumeFraction;
      }
      else if (dBottle1Volume_L <= 0.0) //Empty
      {
        m_data.GetEvents().SetEvent(eEvent::AnesthesiaMachineOxygenBottleOneExhausted, true, m_data.GetSimulationTime());
        dBottle1Volume_L = 0.0;
      }
      GetOxygenBottleOne().GetVolume().SetValue(dBottle1Volume_L, VolumeUnit::L);
    }
    else if (GetOxygenSource() == eAnesthesiaMachine_OxygenSource::BottleTwo)
    {
      double dBottle2Volume_L = GetOxygenBottleTwo().GetVolume().GetValue(VolumeUnit::L);
      if (dBottle2Volume_L > 0.0)
      {
        dBottle2Volume_L -= m_data.GetTimeStep_s() * dFlow_LPerS * m_O2InletVolumeFraction;
      }
      else if (dBottle2Volume_L <= 0.0)
      {
        m_data.GetEvents().SetEvent(eEvent::AnesthesiaMachineOxygenBottleTwoExhausted, true, m_data.GetSimulationTime());
        dBottle2Volume_L = 0.0;
      }
      GetOxygenBottleTwo().GetVolume().SetValue(dBottle2Volume_L, VolumeUnit::L);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Scales the resistance of anesthesia machine equipment in the presence of a leak.
  ///
  /// \details
  /// The resistance of the mask, tube, and y piece are scaled based on the presence and severity of a leak.
  /// This will influence the circuit analyis and will effect the flow and gas mixture provided by the
  /// anesthesia machine.
  /// This occurs during preprocess. 
  //--------------------------------------------------------------------------------------------------
  void AnesthesiaMachineModel::CalculateEquipmentLeak()
  {
    //Note: You should be able to stack failures, if you're so inclined

    if (m_data.GetAirwayMode() == eAirwayMode::Free)
    {
      return;
    }
    else if (m_data.GetAirwayMode() == eAirwayMode::AnesthesiaMachine)
    {
      if (m_data.GetIntubation() == eSwitch::On)
      {
        if (m_actions->HasAnesthesiaMachineTubeCuffLeak() || m_actions->HasAnesthesiaMachineYPieceDisconnect())
        {
          double CuffLeakSeverity = 0.0;
          double YPieceDisconnectSeverity = 0.0;
          double TotalSeverity = 0.0;

          if (m_actions->HasAnesthesiaMachineTubeCuffLeak())
          {
            CuffLeakSeverity = m_actions->GetAnesthesiaMachineTubeCuffLeak().GetSeverity().GetValue();
          }
          if (m_actions->HasAnesthesiaMachineYPieceDisconnect())
          {
            YPieceDisconnectSeverity = m_actions->GetAnesthesiaMachineYPieceDisconnect().GetSeverity().GetValue();
          }

          //Combine the severities
          TotalSeverity = CuffLeakSeverity + YPieceDisconnectSeverity;
          if (TotalSeverity > 1.0)
          {
            TotalSeverity = 1.0;
          }

          double dResistance = GeneralMath::ExponentialDecayFunction(10.0, m_dValveClosedResistance_cmH2O_s_Per_L, m_dValveOpenResistance_cmH2O_s_Per_L, TotalSeverity);
          m_pAnesthesiaConnectionToEnvironment->GetNextResistance().SetValue(dResistance, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
        }
      }
      else
      {
        if (m_actions->HasAnesthesiaMachineMaskLeak() || m_actions->HasAnesthesiaMachineYPieceDisconnect())
        {
          double MaskLeakSeverity = 0.0;
          double YPieceDisconnectSeverity = 0.0;
          double TotalSeverity = 0.0;

          if (m_actions->HasAnesthesiaMachineMaskLeak())
          {
            MaskLeakSeverity = m_actions->GetAnesthesiaMachineMaskLeak().GetSeverity().GetValue();
          }
          if (m_actions->HasAnesthesiaMachineYPieceDisconnect())
          {
            YPieceDisconnectSeverity = m_actions->GetAnesthesiaMachineYPieceDisconnect().GetSeverity().GetValue();
          }

          //Combine the severities
          TotalSeverity = MaskLeakSeverity + YPieceDisconnectSeverity;
          if (TotalSeverity > 1.0)
          {
            TotalSeverity = 1.0;
          }

          double dResistance = GeneralMath::ExponentialDecayFunction(10.0, m_dValveClosedResistance_cmH2O_s_Per_L, m_dValveOpenResistance_cmH2O_s_Per_L, TotalSeverity);
          m_pAnesthesiaConnectionToEnvironment->GetNextResistance().SetValue(dResistance, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
        }
      }
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Valve resistance calculation
  ///
  /// \details
  /// The method checks for the breathing cycle and sets the inspiratory or expiratory valves as open or closed
  /// by adjusting the corresponding valve resistances. The method also changes valve resistances to simulate equipment  
  /// failures that correspond to valve leak and obstructions  
  //--------------------------------------------------------------------------------------------------
  void AnesthesiaMachineModel::CalculateValveResistances()
  {
    //Assume there's no leak or obstruction
    double dInspValveOpenResistance = m_dValveOpenResistance_cmH2O_s_Per_L;
    double dInspValveClosedResistance = m_pInspiratoryLimbToYPiece->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    double dExpValveOpenResistance = m_dValveOpenResistance_cmH2O_s_Per_L;
    double dExpValveClosedResistance = m_pYPieceToExpiratoryLimb->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);

    //Handle leaks and obstructions
    if (m_actions->HasAnesthesiaMachineInspiratoryValveLeak())
    {
      double severity = m_actions->GetAnesthesiaMachineInspiratoryValveLeak().GetSeverity().GetValue();
      dInspValveOpenResistance = GeneralMath::ExponentialDecayFunction(10.0, dInspValveClosedResistance, dInspValveOpenResistance, severity);
    }
    else if (m_actions->HasAnesthesiaMachineInspiratoryValveObstruction())
    {
      double severity = m_actions->GetAnesthesiaMachineInspiratoryValveObstruction().GetSeverity().GetValue();
      dInspValveClosedResistance = GeneralMath::ExponentialDecayFunction(10.0, dInspValveOpenResistance, dInspValveClosedResistance, severity);
    }

    if (m_actions->HasAnesthesiaMachineExpiratoryValveLeak())
    {
      double severity = m_actions->GetAnesthesiaMachineExpiratoryValveLeak().GetSeverity().GetValue();
      dExpValveOpenResistance = GeneralMath::ExponentialDecayFunction(10.0, dExpValveClosedResistance, dExpValveOpenResistance, severity);
    }
    else if (m_actions->HasAnesthesiaMachineExpiratoryValveObstruction())
    {
      double severity = m_actions->GetAnesthesiaMachineExpiratoryValveObstruction().GetSeverity().GetValue();
      dExpValveClosedResistance = GeneralMath::ExponentialDecayFunction(10.0, dExpValveOpenResistance, dExpValveClosedResistance, severity);
    }

    //Set the value based on where we are in the cycle
    if (m_inhaling)
    {
      m_pInspiratoryLimbToYPiece->GetNextResistance().SetValue(dInspValveClosedResistance, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
      m_pYPieceToExpiratoryLimb->GetNextResistance().SetValue(dExpValveOpenResistance, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    }
    else
    {
      m_pInspiratoryLimbToYPiece->GetNextResistance().SetValue(dInspValveOpenResistance, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
      m_pYPieceToExpiratoryLimb->GetNextResistance().SetValue(dExpValveClosedResistance, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates ventilator pressure
  ///
  /// \details
  /// The Anesthesia machine employs pressure-control ventilation mode. This method calculates the 
  /// control ventilator pressure that drives the gas flow in the breathing circle. During inspiration,
  /// the ventilator pressure is set to pre-defined constant value to serve as an input pressure source.
  /// This causes gas to flow into the inspiratory limb path. The pressure is dropped to much low pressure during the expiration 
  /// phase to allow gas return to the ventilator.
  //--------------------------------------------------------------------------------------------------
  void AnesthesiaMachineModel::CalculateVentilatorPressure()
  {
    //Calculate the driver pressure
    double dDriverPressure = 0.0;
    if (m_inhaling)
    {
      dDriverPressure = GetPeakInspiratoryPressure(PressureUnit::cmH2O);
    }
    else
    {
      dDriverPressure = GetPositiveEndExpiratoryPressure(PressureUnit::cmH2O);
    }
    if (m_actions->HasAnesthesiaMachineVentilatorPressureLoss())
    {
      double severity = m_actions->GetAnesthesiaMachineVentilatorPressureLoss().GetSeverity().GetValue();
      dDriverPressure *= (1 - severity);
    }
    m_pEnvironmentToVentilator->GetNextPressureSource().SetValue(dDriverPressure, PressureUnit::cmH2O);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates respiration cycle
  ///
  /// \details
  /// The inspiratory and expiratory phase times are calculated based on
  /// a pre-set respiration rate and inspiration-expiration ratio parameters. These parameters are selected as
  /// input parameters for the anesthesia machine configurations.
  //--------------------------------------------------------------------------------------------------
  void AnesthesiaMachineModel::CalculateCyclePhase()
  {
    //Determine where we are in the cycle
    m_currentbreathingCycleTime_s += m_data.GetTimeStep_s();
    if (m_currentbreathingCycleTime_s > m_totalBreathingCycleTime_s) //End of the cycle
    {
      m_totalBreathingCycleTime_s = 0.0;
      m_currentbreathingCycleTime_s = 0.0;

      double dVentilationFrequency_PerMin = GetRespiratoryRate(FrequencyUnit::Per_min);
      if (dVentilationFrequency_PerMin > 0)
      {
        m_totalBreathingCycleTime_s = 60.0 / dVentilationFrequency_PerMin; //Total time of one breathing cycle  
      }

      double IERatio = GetInspiratoryExpiratoryRatio().GetValue();
      m_inspirationTime_s = IERatio * m_totalBreathingCycleTime_s / (1.0 + IERatio);
    }

    if (m_currentbreathingCycleTime_s < m_inspirationTime_s) //Inspiration
    {
      m_inhaling = true;
    }
    else //Expiration
    {
      m_inhaling = false;
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Checks Relief Valve Pressure
  ///
  /// \details
  /// Assigns relief valve pressure as a pressure source based on the pressure setting and checks if the status 
  /// of the relief valve is open or closed.
  //--------------------------------------------------------------------------------------------------
  void AnesthesiaMachineModel::CheckReliefValve()
  {
    //Set the Pressure Source based on the setting
    double dValvePressure_cmH2O = GetReliefValvePressure(PressureUnit::cmH2O);
    m_pEnvironmentToReliefValve->GetNextPressureSource().SetValue(dValvePressure_cmH2O, PressureUnit::cmH2O);

    //Check to see if it reached the pressure threshold
    if (!m_data.GetEvents().IsEventActive(eEvent::AnesthesiaMachineReliefValveActive) && m_pSelectorToReliefValve->GetNextValve() == eGate::Closed)
    {
      m_data.GetEvents().SetEvent(eEvent::AnesthesiaMachineReliefValveActive, true, m_data.GetSimulationTime());
    }
    else if (m_data.GetEvents().IsEventActive(eEvent::AnesthesiaMachineReliefValveActive) && m_pSelectorToReliefValve->GetNextValve() == eGate::Open)
    {
      m_data.GetEvents().SetEvent(eEvent::AnesthesiaMachineReliefValveActive, false, m_data.GetSimulationTime());
    }

    //Always try to let it run without the relief valve open (i.e. not allowing flow), otherwise it will always stay shorted
    m_pSelectorToReliefValve->SetNextValve(eGate::Open);
  }
END_NAMESPACE
