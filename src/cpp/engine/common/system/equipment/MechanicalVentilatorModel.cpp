/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/PulseConfiguration.h"
#include "engine/common/controller/CircuitManager.h"
#include "engine/common/controller/CompartmentManager.h"
#include "engine/common/controller/SubstanceManager.h"
#include "engine/common/system/equipment/MechanicalVentilatorModel.h"

#include "cdm/system/equipment/mechanical_ventilator/SEMechanicalVentilatorAlarms.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorConfiguration.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorLeak.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorHold.h"
#include "cdm/engine/SEActionManager.h"
#include "cdm/engine/SEEquipmentActionCollection.h"
#include "cdm/engine/SEPatientActionCollection.h"
#include "cdm/engine/SEEventManager.h"
#include "cdm/circuit/fluid/SEFluidCircuit.h"
#include "cdm/compartment/fluid/SEGasCompartment.h"
#include "cdm/compartment/fluid/SELiquidCompartment.h"
#include "cdm/compartment/substances/SEGasSubstanceQuantity.h"
#include "cdm/compartment/substances/SELiquidSubstanceQuantity.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceFraction.h"
#include "cdm/substance/SESubstanceConcentration.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarPressureTimePerVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SERunningAverage.h"
#include "cdm/utils/GeneralMath.h"

namespace pulse
{
  /*
  ========================
    Constructors
  ========================
  */
  MechanicalVentilatorModel::MechanicalVentilatorModel(Data& data) : MechanicalVentilatorSystem(data.GetLogger()), Model(data)
  {
    m_MeanAirwayPressure_cmH2O = new SERunningAverage();
    Clear();
  }

  MechanicalVentilatorModel::~MechanicalVentilatorModel()
  {
    Clear();
    delete m_MeanAirwayPressure_cmH2O;
  }

  void MechanicalVentilatorModel::Clear()
  {
    MechanicalVentilatorSystem::Clear();
    m_Environment = nullptr;
    m_Ventilator = nullptr;
    m_Connection = nullptr;
    m_VentilatorAerosol = nullptr;
    m_VentilatorNode = nullptr;
    m_ExpiratoryLimbNode = nullptr;
    m_ExpiratoryValveNode = nullptr;
    m_InspiratoryLimbNode = nullptr;
    m_InspiratoryValveNode = nullptr;
    m_YPieceNode = nullptr;
    m_ConnectionNode = nullptr;
    m_AmbientNode = nullptr;
    m_EnvironmentToVentilator = nullptr;
    m_YPieceToConnection = nullptr;
    m_VentilatorToExpiratoryValve = nullptr;
    m_VentilatorToEnvironment = nullptr;
    m_VentilatorToInspiratoryValve = nullptr;
    m_ExpiratoryLimbToYPiece = nullptr;
    m_InspiratoryLimbToYPiece = nullptr;
    m_LeakConnectionToEnvironment = nullptr;
    m_ConnectionToReliefValve = nullptr;
    m_EnvironmentToReliefValve = nullptr;
    m_ConnectionToAirway = nullptr;
    m_MachineClosedResistance_cmH2O_s_Per_L = 0;

    m_MeanAirwayPressure_cmH2O->Invalidate();

    GetSettings().SetConnection(eSwitch::Off);
    GetSettings().SetExpirationCycleRespiratoryModel(eSwitch::Off);
    GetSettings().SetInspirationPatientTriggerRespiratoryModel(eSwitch::Off);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Initializes system properties to valid homeostatic values.
  //--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::Initialize()
  {
    Model::Initialize();
    m_BreathState = eBreathState::NoBreath;
    m_CurrentPeriodTime_s = 0.0;
    m_DriverPressure_cmH2O = SEScalar::dNaN();
    m_DriverFlow_L_Per_s = SEScalar::dNaN();
    m_PreviousDriverPressure_cmH2O = SEScalar::dNaN();
    m_PreviousDriverFlow_L_Per_s = SEScalar::dNaN();
    m_CurrentVentilatorVolume_L = 0.0;
    m_CurrentRespiratoryVolume_L = 0.0;
    m_InspirationTime_s = 0.0;
    m_InspiratoryFlow_L_Per_s = 0.0;
    m_PreviousYPieceToConnectionFlow_L_Per_s = 0.0;
    m_PreviousConnectionPressure_cmH2O = 0.0;
    m_PauseOccurred = false;
    m_Initializing = false;
    m_PositiveEndExpiratoryPressure_cmH2O = 0.0;
    m_PeakExpiratoryFlow_L_Per_s = 0.0;
    m_PeakInspiratoryFlow_L_Per_s = 0.0;
    m_EndTidalCarbonDioxideFraction = 0.0;
    m_EndTidalCarbonDioxidePressure_cmH2O = 0.0;
    m_EndTidalOxygenFraction = 0.0;
    m_EndTidalOxygenPressure_cmH2O = 0.0;

    //System data
    GetAirwayPressure().SetValue(0.0, PressureUnit::cmH2O);
    GetDynamicRespiratoryCompliance().SetValue(0.0, VolumePerPressureUnit::L_Per_cmH2O);
    GetEndTidalCarbonDioxideFraction().SetValue(0.0);
    GetEndTidalCarbonDioxidePressure().SetValue(0.0, PressureUnit::cmH2O);
    GetEndTidalOxygenFraction().SetValue(0.0);
    GetEndTidalOxygenPressure().SetValue(0.0, PressureUnit::cmH2O);
    GetExpiratoryFlow().SetValue(0.0, VolumePerTimeUnit::L_Per_s);
    GetExpiratoryResistance().SetValue(0.0, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    GetExpiratoryTidalVolume().SetValue(0.0, VolumeUnit::L);
    GetExtrinsicPositiveEndExpiratoryPressure().SetValue(0.0, PressureUnit::cmH2O);
    GetInspiratoryExpiratoryRatio().SetValue(0.0);
    GetInspiratoryFlow().SetValue(0.0, VolumePerTimeUnit::L_Per_s);
    GetInspiratoryResistance().SetValue(0.0, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    GetInspiratoryTidalVolume().SetValue(0.0, VolumeUnit::L);
    GetIntrinsicPositiveEndExpiratoryPressure().SetValue(0.0, PressureUnit::cmH2O);
    GetLeakFraction().SetValue(0.0);
    GetMeanAirwayPressure().SetValue(0.0, PressureUnit::cmH2O);
    GetPeakExpiratoryFlow().SetValue(0.0, VolumePerTimeUnit::L_Per_s);
    GetPeakInspiratoryFlow().SetValue(0.0, VolumePerTimeUnit::L_Per_s);
    GetPeakInspiratoryPressure().SetValue(0.0, PressureUnit::cmH2O);
    GetPlateauPressure().SetValue(0.0, PressureUnit::cmH2O);
    GetRespirationRate().SetValue(0.0, FrequencyUnit::Per_min);
    GetStaticRespiratoryCompliance().SetValue(0.0, VolumePerPressureUnit::L_Per_cmH2O);
    GetTidalVolume().SetValue(0.0, VolumeUnit::L);
    GetTotalLungVolume().SetValue(0.0, VolumeUnit::L);
    GetTotalPositiveEndExpiratoryPressure().SetValue(0.0, PressureUnit::cmH2O);
    GetTotalPulmonaryVentilation().SetValue(0.0, VolumePerTimeUnit::L_Per_min);

    GetSettings().SetConnection(eSwitch::Off);
    GetSettings().SetExpirationCycleRespiratoryModel(eSwitch::Off);
    GetSettings().SetInspirationPatientTriggerRespiratoryModel(eSwitch::Off);

    StateChange();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Initializes parameters for the mechanical ventilator class
  ///
  /// \details
  /// Initializes member variables and system level values on the common data model.
  //--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::SetUp()
  {
    // Compartments
    m_Environment = m_data.GetCompartments().GetGasCompartment(pulse::EnvironmentCompartment::Ambient);
    m_Ventilator = m_data.GetCompartments().GetGasCompartment(pulse::MechanicalVentilatorCompartment::MechanicalVentilator);
    m_Connection = m_data.GetCompartments().GetGasCompartment(pulse::MechanicalVentilatorCompartment::Connection);
    m_VentilatorAerosol = m_data.GetCompartments().GetLiquidCompartment(pulse::MechanicalVentilatorCompartment::MechanicalVentilator);

    // Nodes
    m_VentilatorNode = m_data.GetCircuits().GetMechanicalVentilatorCircuit().GetNode(pulse::MechanicalVentilatorNode::Ventilator);
    m_ExpiratoryLimbNode = m_data.GetCircuits().GetMechanicalVentilatorCircuit().GetNode(pulse::MechanicalVentilatorNode::ExpiratoryLimb);
    m_ExpiratoryValveNode = m_data.GetCircuits().GetMechanicalVentilatorCircuit().GetNode(pulse::MechanicalVentilatorNode::ExpiratoryValve);
    m_InspiratoryLimbNode = m_data.GetCircuits().GetMechanicalVentilatorCircuit().GetNode(pulse::MechanicalVentilatorNode::InspiratoryLimb);
    m_InspiratoryValveNode = m_data.GetCircuits().GetMechanicalVentilatorCircuit().GetNode(pulse::MechanicalVentilatorNode::InspiratoryValve);
    m_YPieceNode = m_data.GetCircuits().GetMechanicalVentilatorCircuit().GetNode(pulse::MechanicalVentilatorNode::YPiece);
    m_ConnectionNode = m_data.GetCircuits().GetMechanicalVentilatorCircuit().GetNode(pulse::MechanicalVentilatorNode::Connection);
    m_AmbientNode = m_data.GetCircuits().GetMechanicalVentilatorCircuit().GetNode(pulse::EnvironmentNode::Ambient);

    // Paths
    m_EnvironmentToVentilator = m_data.GetCircuits().GetMechanicalVentilatorCircuit().GetPath(pulse::MechanicalVentilatorPath::EnvironmentToVentilator);
    m_VentilatorToEnvironment = m_data.GetCircuits().GetMechanicalVentilatorCircuit().GetPath(pulse::MechanicalVentilatorPath::VentilatorToEnvironment);
    m_YPieceToConnection = m_data.GetCircuits().GetMechanicalVentilatorCircuit().GetPath(pulse::MechanicalVentilatorPath::YPieceToConnection);
    m_VentilatorToExpiratoryValve = m_data.GetCircuits().GetMechanicalVentilatorCircuit().GetPath(pulse::MechanicalVentilatorPath::VentilatorToExpiratoryValve);
    m_VentilatorToInspiratoryValve = m_data.GetCircuits().GetMechanicalVentilatorCircuit().GetPath(pulse::MechanicalVentilatorPath::VentilatorToInspiratoryValve);
    m_ExpiratoryLimbToYPiece = m_data.GetCircuits().GetMechanicalVentilatorCircuit().GetPath(pulse::MechanicalVentilatorPath::ExpiratoryLimbToYPiece);
    m_InspiratoryLimbToYPiece = m_data.GetCircuits().GetMechanicalVentilatorCircuit().GetPath(pulse::MechanicalVentilatorPath::InspiratoryLimbToYPiece);
    m_ConnectionToReliefValve = m_data.GetCircuits().GetMechanicalVentilatorCircuit().GetPath(pulse::MechanicalVentilatorPath::ConnectionToReliefValve);
    m_EnvironmentToReliefValve = m_data.GetCircuits().GetMechanicalVentilatorCircuit().GetPath(pulse::MechanicalVentilatorPath::EnvironmentToReliefValve);
    m_LeakConnectionToEnvironment = m_data.GetCircuits().GetMechanicalVentilatorCircuit().GetPath(pulse::MechanicalVentilatorPath::LeakConnectionToEnvironment);

    m_ConnectionToAirway = m_data.GetCircuits().GetRespiratoryAndMechanicalVentilatorCircuit().GetPath(pulse::CombinedMechanicalVentilatorPath::ConnectionToAirway);

    // Configuration
    m_MachineClosedResistance_cmH2O_s_Per_L = m_data.GetConfiguration().GetMachineClosedResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    m_MachineOpenResistance_cmH2O_s_Per_L = m_data.GetConfiguration().GetMachineOpenResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);

    // Piecewise points
    double openResistance = m_data.GetConfiguration().GetDefaultOpenFlowResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    m_leakPoints =
    { {0, openResistance},
      {0.1, 1000.0},
      {0.3, 500.0},
      {0.6, 100.0},
      {0.9, 50.0},
      {1.0, 1.0} };
  }

  void MechanicalVentilatorModel::StateChange()
  {
    UpdateAirwayMode();
    if (m_data.GetAirwayMode() != eAirwayMode::MechanicalVentilator)
    {
      TurnOff();
      return;
    }
    m_BreathState = eBreathState::EquipmentExhale;
    m_CurrentPeriodTime_s = 0.0;
    m_CurrentVentilatorVolume_L = 0.0;
    m_CurrentRespiratoryVolume_L = 0.0;
    m_PreviousYPieceToConnectionFlow_L_Per_s = 0.0;
    m_PreviousConnectionPressure_cmH2O = 0.0;
    m_PauseOccurred = false;
    m_Initializing = true;

    //Default the relief valve threshold if not there
    if (!GetSettings().HasReliefValveThreshold())
      GetSettings().GetReliefValveThreshold().SetValue(1000, PressureUnit::cmH2O);

    //Reset the substance quantities at the connection
    for (SEGasSubstanceQuantity* subQ : m_Ventilator->GetSubstanceQuantities())
      subQ->SetToZero();

    //Start by setting everything to ambient
    for (auto s : m_Environment->GetSubstanceQuantities())
    {
      m_Ventilator->GetSubstanceQuantity(s->GetSubstance())->GetVolumeFraction().Set(s->GetVolumeFraction());
    }

    //Set the substance volume fractions from our settings
    std::vector<SESubstanceFraction*> gasFractions = GetSettings().GetFractionInspiredGases();
    //Has fractions defined
    for (auto f : gasFractions)
    {
      if (!f->GetFractionAmount().IsValid())
        continue;

      const SESubstance& sub = f->GetSubstance();
      double fraction = f->GetFractionAmount().GetValue();

      //Do this, just in case it's something new
      m_data.GetSubstances().AddActiveSubstance(sub);

      //Now set it on the ventilator compartment
      //It has a infinate volume, so this will keep the same volume fraction no matter what's going on around it
      m_Ventilator->GetSubstanceQuantity(sub)->GetVolumeFraction().SetValue(fraction);
    }

    double totalFractionDefined = 0.0;
    for (auto s : m_Ventilator->GetSubstanceQuantities())
    {
      if (s->GetVolumeFraction().IsValid())
        totalFractionDefined += s->GetVolumeFraction().GetValue();
    }

    //Add or remove Nitrogen to balance
    double gasFractionDiff = 1.0 - totalFractionDefined;
    SEGasSubstanceQuantity* N2 = m_Ventilator->GetSubstanceQuantity(m_data.GetSubstances().GetN2());
    double currentN2Fraction = 0;
    if (N2->GetVolumeFraction().IsValid())
      currentN2Fraction = N2->GetVolumeFraction().GetValue();
    SEGasSubstanceQuantity* O2 = m_Ventilator->GetSubstanceQuantity(m_data.GetSubstances().GetO2());
    double FiO2 = 0;
    if (O2->GetVolumeFraction().IsValid())
      FiO2 = O2->GetVolumeFraction().GetValue();
    if (currentN2Fraction + gasFractionDiff < 0.0)
    {
      m_ss << "FiO2 setting + ambient fractions other than N2 is greater than 1.0. Setting FiO2 to max value of " << FiO2 + currentN2Fraction + gasFractionDiff << ".";
      Error(m_ss);
      N2->GetVolumeFraction().SetValue(0.0);
      O2->GetVolumeFraction().SetValue(FiO2 + currentN2Fraction + gasFractionDiff);
    }
    else
    {
      Info("Adding "+cdm::to_string(currentN2Fraction + gasFractionDiff) + "% of N2 to the system");
      N2->GetVolumeFraction().SetValue(currentN2Fraction + gasFractionDiff);
    }

    //Set the aerosol concentrations ********************************************
    std::vector<SESubstanceConcentration*> liquidConcentrations = GetSettings().GetConcentrationInspiredAerosols();

    //Reset the substance quantities at the ventilator
    for (SELiquidSubstanceQuantity* subQ : m_VentilatorAerosol->GetSubstanceQuantities())
      subQ->SetToZero();

    if (!liquidConcentrations.empty())
    {
      //Has fractions defined
      for (auto f : liquidConcentrations)
      {
        if (!f->GetConcentration().IsValid())
          continue;

        const SESubstance& sub = f->GetSubstance();
        SEScalarMassPerVolume concentration = f->GetConcentration();

        //Do this, just in case it's something new
        m_data.GetSubstances().AddActiveSubstance(sub);
        //Now set it on the connection compartment
        //It has infinite volume, so this will keep the same volume fraction no matter what's going on around it
        m_VentilatorAerosol->GetSubstanceQuantity(sub)->GetConcentration().Set(concentration);
      }
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Connect to the patient via the ventilator mask, an endotracheal tube, or no connection
  ///
  /// \param  enumMechanicalVentilatorConnection
  /// Connectoin type : Mask, tube, or off
  /// 
  /// \details
  /// If the enum is set to mask, then the mask is applied to the face
  /// If the enum is set to tube, then the ventilator is connected to the tube
  /// If the enum is set to off, the airway mode is set to free.
  //--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::UpdateAirwayMode()
  {
    eSwitch c = GetSettings().GetConnection();
    if (c == eSwitch::On)
    {
      m_data.SetAirwayMode(eAirwayMode::MechanicalVentilator);
      return;
    }
    else if (c == eSwitch::Off && m_data.GetAirwayMode() == eAirwayMode::MechanicalVentilator)
    {
      // Make sure we are active to make sure we go back to free
      m_data.SetAirwayMode(eAirwayMode::Free);
    }
  }


  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Mechanical ventilator preprocess function
  ///
  /// \details
  /// Applies all the settings and calculates the instantaneous driver value.
  //--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::PreProcess()
  {
    if (m_data.GetActions().GetEquipmentActions().HasMechanicalVentilatorConfiguration())
    {
      GetSettings().ProcessConfiguration(m_data.GetActions().GetEquipmentActions().GetMechanicalVentilatorConfiguration(), m_data.GetSubstances());
      m_data.GetActions().GetEquipmentActions().RemoveMechanicalVentilatorConfiguration();
      StateChange();
    }

    //Do nothing if the ventilator is off and not initialized
    if (GetSettings().GetConnection() == eSwitch::Off)
    {
      m_BreathState = eBreathState::NoBreath;
      m_CurrentPeriodTime_s = 0.0;
      m_CurrentVentilatorVolume_L = 0.0;
      m_CurrentRespiratoryVolume_L = 0.0;
      m_PreviousYPieceToConnectionFlow_L_Per_s = 0.0;
      m_PreviousConnectionPressure_cmH2O = 0.0;
      m_PauseOccurred = false;
      m_Initializing = false;
      // Remove any active ventilator actions
      m_data.GetActions().GetEquipmentActions().RemoveMechanicalVentilatorActions();
      return;
    }

    CalculateRespiratoryParameters();
    CalculateInspiration();
    CalculatePause();
    CalculateExpiration();
    SetHold();
    SetLeak();
    SetValves();
    SetVentilatorDriver();
    SetResistances();
    SetCompliance();
    SetVolumes();
    CheckReliefValve();

    if (m_YPieceToConnection->HasNextFlow())
    {
      m_PreviousYPieceToConnectionFlow_L_Per_s = m_YPieceToConnection->GetNextFlow(VolumePerTimeUnit::L_Per_s);
    }

    if (m_ConnectionNode->HasNextPressure())
    {
      m_PreviousConnectionPressure_cmH2O = m_ConnectionNode->GetNextPressure(PressureUnit::cmH2O);
    }
    
    m_CurrentPeriodTime_s += m_data.GetTimeStep_s();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Mechanical ventilator process function
  ///
  /// \details
  /// The current Pulse implementation has no specific process functionality for the mechanical ventilator.
  /// Mechanical ventilator processing is currently done in the Respiratory System with the combined circuit methodology.
  //--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::Process(bool /*solve_and_transport*/)
  {
    ComputeExposedModelParameters();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Mechanical ventilator postprocess function
  ///
  /// \details
  /// The substance volumes and the volume fractions are updated for all of the nodes in the mechanical ventilator
  /// circuit during post process. 
  /// The ventilator volumes are updated based on the previously calculated nodal analysis.
  //--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::PostProcess(bool /*solve_and_transport*/)
  {

  }

  void MechanicalVentilatorModel::ComputeExposedModelParameters()
  {

  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Set the instantaneous driver pressure or flow on the circuit source.
  //--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::SetVentilatorDriver()
  {
    double driverPressure_cmH2O = m_DriverPressure_cmH2O;
    double driverFlow_L_Per_s = m_DriverFlow_L_Per_s;

    if (!std::isnan(driverPressure_cmH2O) && !std::isnan(driverFlow_L_Per_s))
      {
        Error("Ventilator driver pressure and flow both set, only one allowed. Using the pressure value.");
      }
    else if (std::isnan(driverPressure_cmH2O) && std::isnan(driverFlow_L_Per_s))
    {
      Error("Ventilator driver pressure or flow must be set. Using a pressure of 0.");

      m_DriverPressure_cmH2O = 0.0;
    }

    //Dampen the change
    double driverDampingParameter_Per_s = 25.0;
    if (GetSettings().HasDriverDampingParameter())
      driverDampingParameter_Per_s = GetSettings().GetDriverDampingParameter(FrequencyUnit::Per_s);
    driverDampingParameter_Per_s = LIMIT(driverDampingParameter_Per_s, 0.0, 1.0 / m_data.GetTimeStep_s());

    if (!std::isnan(driverPressure_cmH2O))
    {
      double previousDriverPressure_cmH2O = 0.0;
      if (std::isnan(m_PreviousDriverPressure_cmH2O))
      {
        if (m_VentilatorNode->HasNextPressure())
        {
          previousDriverPressure_cmH2O = m_VentilatorNode->GetNextPressure(PressureUnit::cmH2O) - m_AmbientNode->GetNextPressure(PressureUnit::cmH2O);
        }
      }
      else
      {
        previousDriverPressure_cmH2O = m_PreviousDriverPressure_cmH2O;
      }

      if (driverPressure_cmH2O != 0.0) //Don't dampen if it's 0 because that's probably from a limit
      {
        driverPressure_cmH2O = GeneralMath::Damper(driverPressure_cmH2O, previousDriverPressure_cmH2O, driverDampingParameter_Per_s, m_data.GetTimeStep_s());
      }

      if (m_DriverPressure_cmH2O > previousDriverPressure_cmH2O)
      {
        driverPressure_cmH2O = LIMIT(driverPressure_cmH2O, previousDriverPressure_cmH2O, m_DriverPressure_cmH2O);
      }
      else
      {
        driverPressure_cmH2O = LIMIT(driverPressure_cmH2O, m_DriverPressure_cmH2O, previousDriverPressure_cmH2O);
      }
    }

    if (!std::isnan(driverFlow_L_Per_s))
    {
      double previousDriverFlow_L_Per_s = 0.0;
      if (std::isnan(m_PreviousDriverFlow_L_Per_s))
      {
        if (m_EnvironmentToVentilator->HasNextFlow())
        {
          previousDriverFlow_L_Per_s = m_EnvironmentToVentilator->GetNextFlow(VolumePerTimeUnit::L_Per_s);
        }
      }
      else
      {
        previousDriverFlow_L_Per_s = m_PreviousDriverFlow_L_Per_s;
      }

      if (driverFlow_L_Per_s != 0.0) //Don't dampen if it's 0 because that's probably from a limit
      {
        driverFlow_L_Per_s = GeneralMath::Damper(driverFlow_L_Per_s, previousDriverFlow_L_Per_s, driverDampingParameter_Per_s, m_data.GetTimeStep_s());
      }

      if (m_DriverFlow_L_Per_s > previousDriverFlow_L_Per_s)
      {
        driverFlow_L_Per_s = LIMIT(driverFlow_L_Per_s, previousDriverFlow_L_Per_s, m_DriverFlow_L_Per_s);
      }
      else
      {
        driverFlow_L_Per_s = LIMIT(driverFlow_L_Per_s, m_DriverFlow_L_Per_s, previousDriverFlow_L_Per_s);
      }
    }

    bool stateChange = false;
    if (!std::isnan(driverPressure_cmH2O))
    {
      if (m_EnvironmentToVentilator->HasNextFlowSource())
      {
        stateChange = true;
        m_EnvironmentToVentilator->RemoveFlowSource();
      }
      if (!m_EnvironmentToVentilator->HasNextPressureSource())
      {
        stateChange = true;
        m_EnvironmentToVentilator->GetPressureSourceBaseline().SetValue(0, PressureUnit::cmH2O);
      }
      m_EnvironmentToVentilator->GetNextPressureSource().SetValue(driverPressure_cmH2O, PressureUnit::cmH2O);
    }
    else if (!std::isnan(driverFlow_L_Per_s))
    {
      if (m_EnvironmentToVentilator->HasNextPressureSource())
      {
        stateChange = true;
        m_EnvironmentToVentilator->RemovePressureSource();
      }
      if (!m_EnvironmentToVentilator->HasNextFlowSource())
      {
        stateChange = true;
        m_EnvironmentToVentilator->GetFlowSourceBaseline().SetValue(0, VolumePerTimeUnit::L_Per_s);
      }
      m_EnvironmentToVentilator->GetNextFlowSource().SetValue(driverFlow_L_Per_s, VolumePerTimeUnit::L_Per_s);
    }
    if (stateChange)
      m_data.GetCircuits().GetActiveRespiratoryCircuit().StateChange();

    m_PreviousDriverPressure_cmH2O = driverPressure_cmH2O;
    m_PreviousDriverFlow_L_Per_s = driverFlow_L_Per_s;
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Check the triggers pressure during inspiration.
  //--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::CheckInspirationTriggers()
  {
    // Check trigger
    // Any combination of triggers can be used, but there must be at least one
    bool triggerDefined = false;

    if (GetSettings().GetExpirationCycleRespiratoryModel() == eSwitch::On)
    {
      triggerDefined = true;
      if (m_data.GetEvents().IsEventActive(eEvent::StartOfExhale))
      {
        CycleMode(true);
        return;
      }
    }

    if (GetSettings().HasExpirationCyclePressure())
    {
      triggerDefined = true;
      double ambientPressure_cmH2O = m_AmbientNode->GetPressure(PressureUnit::cmH2O);
      double airwayPressure_cmH2O = m_ConnectionNode->GetPressure(PressureUnit::cmH2O);
      if (airwayPressure_cmH2O - ambientPressure_cmH2O >= GetSettings().GetExpirationCyclePressure(PressureUnit::cmH2O))
      {
        CycleMode(true);
        return;
      }
    }

    if (GetSettings().HasExpirationCycleVolume())
    {
      triggerDefined = true;
      if (m_CurrentRespiratoryVolume_L >= GetSettings().GetExpirationCycleVolume(VolumeUnit::L))
      {
        CycleMode(true);
        return;
      }
    }

    if (GetSettings().HasExpirationCycleFlow())
    {
      triggerDefined = true;
      if (m_YPieceToConnection->GetNextFlow(VolumePerTimeUnit::L_Per_s) <= GetSettings().GetExpirationCycleFlow(VolumePerTimeUnit::L_Per_s) &&
        m_CurrentPeriodTime_s > 0.0) //Check if we just cycled the mode
      {
        CycleMode(true);
        return;
      }
    }

    if (GetSettings().HasExpirationCycleTime())
    {
      triggerDefined = true;
      if (m_CurrentPeriodTime_s >= GetSettings().GetExpirationCycleTime(TimeUnit::s) - m_InspirationTime_s)
      {
        CycleMode(false);
        return;
      }
    }

    if (!triggerDefined)
    {
      Fatal("No expiration cycle defined.");
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Determine the instantaneous driver pressure during inspiration.
  //--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::CalculateInspiration()
  {
    if (m_BreathState != eBreathState::EquipmentInhale &&
      m_BreathState != eBreathState::PatientInhale)
    {
      return;
    }

    CheckInspirationTriggers();
    CheckCyclingAlarms();

    // Check limit

    if (GetSettings().HasInspirationLimitVolume())
    {
      if (m_CurrentRespiratoryVolume_L >= GetSettings().GetInspirationLimitVolume(VolumeUnit::L))
      {
        m_DriverFlow_L_Per_s = 0.0;
        m_DriverPressure_cmH2O = SEScalar::dNaN();
        CycleMode(false);
        return;
      }
    }

    if (GetSettings().HasInspirationLimitFlow())
    {
      if (m_YPieceToConnection->GetNextFlow(VolumePerTimeUnit::L_Per_s) >= GetSettings().GetInspirationLimitFlow(VolumePerTimeUnit::L_Per_s))
      {
        m_DriverFlow_L_Per_s = GetSettings().GetInspirationLimitFlow(VolumePerTimeUnit::L_Per_s);
        m_DriverPressure_cmH2O = SEScalar::dNaN();
        CycleMode(false);
        return;
      }
    }

    if (GetSettings().HasInspirationLimitPressure())
    {
      if (m_ConnectionNode->GetPressure(PressureUnit::cmH2O) >= GetSettings().GetInspirationLimitPressure(PressureUnit::cmH2O))
      {
        m_DriverFlow_L_Per_s = SEScalar::dNaN();
        m_DriverPressure_cmH2O = GetSettings().GetInspirationLimitPressure(PressureUnit::cmH2O);
        CycleMode(false);
        return;
      }
    }

    // Check waveform
    if (GetSettings().GetInspirationWaveform() != eDriverWaveform::Square &&
      !GetSettings().HasInspirationWaveformPeriod())
    {
      Fatal("Non-square waveforms require a period.");
    }

    // Apply waveform
    if (GetSettings().GetInspirationWaveform() == eDriverWaveform::Square)
    {
      if (GetSettings().HasPeakInspiratoryPressure())
      {
        m_DriverPressure_cmH2O = GetSettings().GetPeakInspiratoryPressure(PressureUnit::cmH2O);
        m_DriverFlow_L_Per_s = SEScalar::dNaN();
      }
      else if (GetSettings().HasInspirationTargetFlow())
      {
        m_DriverFlow_L_Per_s = GetSettings().GetInspirationTargetFlow(VolumePerTimeUnit::L_Per_s);
        m_DriverPressure_cmH2O = SEScalar::dNaN();
      }
      else
      {
        Fatal("Inspiration mode not yet supported.");
      }
    }
    else if (GetSettings().GetInspirationWaveform() == eDriverWaveform::AscendingRamp)
    {
      if (GetSettings().HasPeakInspiratoryPressure())
      {
        double finalPressure_cmH2O = GetSettings().GetPeakInspiratoryPressure(PressureUnit::cmH2O);
        double initialPressure_cmH2O = 0.0;
        if (GetSettings().HasPositiveEndExpiratoryPressure())
        {
          initialPressure_cmH2O = GetSettings().GetPositiveEndExpiratoryPressure(PressureUnit::cmH2O);
        }

        m_DriverPressure_cmH2O = initialPressure_cmH2O + (finalPressure_cmH2O - initialPressure_cmH2O) * m_CurrentPeriodTime_s / GetSettings().GetInspirationWaveformPeriod(TimeUnit::s);
        m_DriverPressure_cmH2O = LIMIT(m_DriverPressure_cmH2O, initialPressure_cmH2O, finalPressure_cmH2O);
        m_DriverFlow_L_Per_s = SEScalar::dNaN();
      }
      else if (GetSettings().HasInspirationTargetFlow())
      {
        double initialFlow_L_Per_s = 0.0;
        double finalFlow_L_Per_s = GetSettings().GetInspirationTargetFlow(VolumePerTimeUnit::L_Per_s);
        m_DriverFlow_L_Per_s = initialFlow_L_Per_s + (finalFlow_L_Per_s - initialFlow_L_Per_s) * m_CurrentPeriodTime_s / GetSettings().GetInspirationWaveformPeriod(TimeUnit::s);
        m_DriverFlow_L_Per_s = LIMIT(m_DriverFlow_L_Per_s, initialFlow_L_Per_s, finalFlow_L_Per_s);
        m_DriverPressure_cmH2O = SEScalar::dNaN();
      }
      else
      {
        Fatal("Inspiration mode not yet supported.");
      }
    }
    else if (GetSettings().GetInspirationWaveform() == eDriverWaveform::DescendingRamp)
    {
      if (GetSettings().HasPeakInspiratoryPressure())
      {
        double initialPressure_cmH2O = GetSettings().GetPeakInspiratoryPressure(PressureUnit::cmH2O);
        double finalPressure_cmH2O = 0.0;
        if (GetSettings().HasPositiveEndExpiratoryPressure())
        {
          finalPressure_cmH2O = GetSettings().GetPositiveEndExpiratoryPressure(PressureUnit::cmH2O);
        }

        m_DriverPressure_cmH2O = initialPressure_cmH2O + (finalPressure_cmH2O - initialPressure_cmH2O) * m_CurrentPeriodTime_s / GetSettings().GetInspirationWaveformPeriod(TimeUnit::s);
        m_DriverPressure_cmH2O = LIMIT(m_DriverPressure_cmH2O, finalPressure_cmH2O, initialPressure_cmH2O);
        m_DriverFlow_L_Per_s = SEScalar::dNaN();
      }
      else if (GetSettings().HasInspirationTargetFlow())
      {
        double initialFlow_L_Per_s = GetSettings().GetInspirationTargetFlow(VolumePerTimeUnit::L_Per_s);
        double finalFlow_L_Per_s = 0.0;
        m_DriverFlow_L_Per_s = initialFlow_L_Per_s + (finalFlow_L_Per_s - initialFlow_L_Per_s) * m_CurrentPeriodTime_s / GetSettings().GetInspirationWaveformPeriod(TimeUnit::s);
        m_DriverFlow_L_Per_s = LIMIT(m_DriverFlow_L_Per_s, finalFlow_L_Per_s, initialFlow_L_Per_s);
        m_DriverPressure_cmH2O = SEScalar::dNaN();
      }
      else
      {
        Fatal("Inspiration mode not yet supported.");
      }
    }
    else
    {
      Fatal("Waveform type not yet supported.");
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Determine the instantaneous driver pressure during pause.
  //--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::CalculatePause()
  {
    if (m_BreathState != eBreathState::EquipmentPause &&
      m_BreathState != eBreathState::PatientPause)
    {
      return;
    }

    if (GetSettings().HasInspirationPauseTime())
    {
      if (m_CurrentPeriodTime_s < GetSettings().GetInspirationPauseTime(TimeUnit::s))
      {
        // Hold with no flow
        m_DriverPressure_cmH2O = SEScalar::dNaN();
        m_DriverFlow_L_Per_s = 0.0;
      }
      else
      {
        CycleMode(false);
      }
    }
    else
    {
      // If the limit was reached, we'll hold here until the trigger
      CheckInspirationTriggers();
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Determine the instantaneous driver pressure during expiration.
  //--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::CalculateExpiration()
  {
    if (m_BreathState != eBreathState::EquipmentExhale &&
      m_BreathState != eBreathState::PatientExhale)
    {
      return;
    }

    // Check trigger
    // Any combination of triggers can be used, but there must be at least one
    bool triggerDefined = false;

    if (GetSettings().GetInspirationPatientTriggerRespiratoryModel() == eSwitch::On)
    {
      triggerDefined = true;
      if (m_data.GetEvents().IsEventActive(eEvent::StartOfInhale))
      {
        CycleMode(true);
        return;
      }
    }

    if (GetSettings().HasInspirationPatientTriggerPressure())
    {
      if (GetSettings().GetInspirationPatientTriggerPressure(PressureUnit::cmH2O) == 0.0)
      {
        //Remove triggers set to 0.0
        GetSettings().GetInspirationPatientTriggerPressure().Invalidate();
      }
      else
      {
        triggerDefined = true;

        double relativePressure_cmH2O = m_ConnectionNode->GetNextPressure(PressureUnit::cmH2O) - m_AmbientNode->GetNextPressure(PressureUnit::cmH2O);
        double previousRelativePressure_cmH2O = m_PreviousConnectionPressure_cmH2O - m_AmbientNode->GetNextPressure(PressureUnit::cmH2O);
        double triggerPressure_cmH2O = relativePressure_cmH2O - GetExtrinsicPositiveEndExpiratoryPressure(PressureUnit::cmH2O);

        if (triggerPressure_cmH2O <= -abs(GetSettings().GetInspirationPatientTriggerPressure(PressureUnit::cmH2O)) && //Allow it to be set as either positive or negative
          m_CurrentPeriodTime_s > 0.0 && //Check if we just cycled the mode
          relativePressure_cmH2O < previousRelativePressure_cmH2O) //Check if it's moving the right direction to prevent premature cycling
        {
          CycleMode(true);
          return;
        }
      }
    }
    
    if (GetSettings().HasInspirationPatientTriggerFlow())
    {
      if (GetSettings().GetInspirationPatientTriggerFlow(VolumePerTimeUnit::L_Per_s) == 0.0)
      {
        //Remove triggers set to 0.0
        GetSettings().GetInspirationPatientTriggerFlow().Invalidate();
      }
      else
      {
        triggerDefined = true;
        if (m_YPieceToConnection->GetNextFlow(VolumePerTimeUnit::L_Per_s) >= abs(GetSettings().GetInspirationPatientTriggerFlow(VolumePerTimeUnit::L_Per_s)) && //Allow it to be set as either positive or negative
          m_CurrentPeriodTime_s > 0.0 && //Check if we just cycled the mode
          m_YPieceToConnection->GetNextFlow(VolumePerTimeUnit::L_Per_s) > m_PreviousYPieceToConnectionFlow_L_Per_s) //Check if it's moving the right direction to prevent premature cycling
        {
          CycleMode(true);
          return;
        }
      }
    }

    if (GetSettings().HasInspirationMachineTriggerTime())
    {
      triggerDefined = true;

      double inspirationMachineTriggerTime_s = GetSettings().GetInspirationMachineTriggerTime(TimeUnit::s);
      if (triggerDefined && m_Initializing)
      {
        //Give extra time to hit other types of triggers the first time through to help prevent dyssynchrony
        inspirationMachineTriggerTime_s *= 2.0;
      }
      if (m_CurrentPeriodTime_s >= inspirationMachineTriggerTime_s)
      {
        CycleMode(false);
        return;
      }
    }

    if(!triggerDefined)
    {
      Fatal("No inspiration trigger defined.");
    }

    // Apply waveform
    if (GetSettings().GetExpirationWaveform() == eDriverWaveform::Square)
    {
      if (GetSettings().HasPositiveEndExpiratoryPressure())
      {
        m_DriverPressure_cmH2O = GetSettings().GetPositiveEndExpiratoryPressure(PressureUnit::cmH2O);
        m_DriverFlow_L_Per_s = SEScalar::dNaN();
      }
      else if (GetSettings().HasFunctionalResidualCapacity())
      {
        Fatal("Functional residual capacity expiratory baseline not yet supported.");
      }
      else
      {
        m_DriverPressure_cmH2O = 0.0;
        m_DriverFlow_L_Per_s = SEScalar::dNaN();
      }
    }
    else
    {
      Fatal("Non-square waveforms are not yet supported for expiration.");
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Go to next breath mode based on current mode.
  ///
  /// \details
  /// Inhale to pause, pause to exhale, or exhale to inhale.
  //--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::CycleMode(bool patientTriggered)
  {
    if (m_BreathState == eBreathState::EquipmentInhale ||
      m_BreathState == eBreathState::PatientInhale)
    {
      CalculateInspiratoryRespiratoryParameters();

      if (m_data.GetActions().GetEquipmentActions().HasMechanicalVentilatorHold() &&
        m_data.GetActions().GetEquipmentActions().GetMechanicalVentilatorHold().GetAppliedRespiratoryCycle() == eAppliedRespiratoryCycle::Inspiratory)
      {
        m_BreathState = eBreathState::InspiratoryHold;
        return;
      }

      m_InspirationTime_s += m_CurrentPeriodTime_s;
      m_BreathState = patientTriggered ? eBreathState::PatientPause : eBreathState::EquipmentPause;
    }
    else if (m_BreathState == eBreathState::InspiratoryHold)
    {
      m_InspirationTime_s += m_CurrentPeriodTime_s;
      m_BreathState = patientTriggered ? eBreathState::PatientPause : eBreathState::EquipmentPause;
    }
    else if (m_BreathState == eBreathState::EquipmentPause ||
      m_BreathState == eBreathState::PatientPause)
    {
      m_InspirationTime_s += m_CurrentPeriodTime_s;
      CalculatePauseRespiratoryParameters();
      m_BreathState = patientTriggered ? eBreathState::PatientExhale : eBreathState::EquipmentExhale;
    }
    else if (m_BreathState == eBreathState::EquipmentExhale ||
      m_BreathState == eBreathState::PatientExhale)
    {
      if (m_data.GetActions().GetEquipmentActions().HasMechanicalVentilatorHold() &&
        m_data.GetActions().GetEquipmentActions().GetMechanicalVentilatorHold().GetAppliedRespiratoryCycle() == eAppliedRespiratoryCycle::Expiratory)
      {
        m_BreathState = eBreathState::ExpiratoryHold;
        return;
      }

      CalculateExpiratoryRespiratoryParameters(patientTriggered);
      m_InspirationTime_s = 0.0;
      m_BreathState = patientTriggered ? eBreathState::PatientInhale : eBreathState::EquipmentInhale;
    }
    else if (m_BreathState == eBreathState::ExpiratoryHold)
    {
      CalculateExpiratoryRespiratoryParameters(patientTriggered);
      m_InspirationTime_s = 0.0;
      m_BreathState = patientTriggered ? eBreathState::PatientInhale : eBreathState::EquipmentInhale;
    }

    m_CurrentPeriodTime_s = 0.0;
    m_Initializing = false;
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Set the resistance to ground that causes air to leak out of the ventilator-respiratory system.
  //--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::SetLeak()
  {
    if (m_data.GetActions().GetEquipmentActions().HasMechanicalVentilatorLeak())
    {
      double severity = m_data.GetActions().GetEquipmentActions().GetMechanicalVentilatorLeak().GetSeverity().GetValue();
      double resistance_cmH2O_s_Per_L = GeneralMath::PiecewiseLinearInterpolator(m_leakPoints, severity);

      m_LeakConnectionToEnvironment->GetNextResistance().SetValue(resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Close the inspiratory valve during exhale when using a pressure trigger.
  //--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::SetValves()
  {
    if (GetSettings().HasInspirationPatientTriggerPressure() &&
      (m_BreathState == eBreathState::EquipmentExhale ||
        m_BreathState == eBreathState::PatientExhale ||
        m_BreathState == eBreathState::ExpiratoryHold))
    {
      m_VentilatorToInspiratoryValve->GetNextResistance().SetValue(m_MachineOpenResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Set the flow to zero to hold.
  //--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::SetHold()
  {
    if (m_data.GetActions().GetEquipmentActions().HasMechanicalVentilatorHold() &&
      m_data.GetActions().GetEquipmentActions().GetMechanicalVentilatorHold().GetAppliedRespiratoryCycle() == eAppliedRespiratoryCycle::Instantaneous)
    {
      m_DriverFlow_L_Per_s = 0.0;
      m_DriverPressure_cmH2O = SEScalar::dNaN();
      return;
    }
    else if (m_BreathState == eBreathState::ExpiratoryHold ||
      m_BreathState == eBreathState::InspiratoryHold)
    {
      if (!m_data.GetActions().GetEquipmentActions().HasMechanicalVentilatorHold())
      {
        //Hold turned off
        CycleMode(false);
        return;
      }

      m_DriverFlow_L_Per_s = 0.0;
      m_DriverPressure_cmH2O = SEScalar::dNaN();
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Set ventilator circuit resistances.
  ///
  /// \details
  /// If no values are specified, defaults will be used.
  //--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::SetResistances()
  {
    if (GetSettings().HasExpirationTubeResistance())
    {
      double resistance_cmH2O_s_Per_L = GetSettings().GetExpirationTubeResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
      resistance_cmH2O_s_Per_L = MIN(resistance_cmH2O_s_Per_L, m_MachineClosedResistance_cmH2O_s_Per_L);
      m_VentilatorToExpiratoryValve->GetNextResistance().SetValue(resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    }

    if (GetSettings().HasInspirationTubeResistance())
    {
      double resistance_cmH2O_s_Per_L = GetSettings().GetInspirationTubeResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
      resistance_cmH2O_s_Per_L = MIN(resistance_cmH2O_s_Per_L, m_MachineClosedResistance_cmH2O_s_Per_L);
      m_VentilatorToInspiratoryValve->GetNextResistance().SetValue(resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    }

    if (GetSettings().HasExpirationValveResistance())
    {
      double resistance_cmH2O_s_Per_L = GetSettings().GetExpirationValveResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
      resistance_cmH2O_s_Per_L = MIN(resistance_cmH2O_s_Per_L, m_MachineClosedResistance_cmH2O_s_Per_L);
      m_ExpiratoryLimbToYPiece->GetNextResistance().SetValue(resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    }

    if (GetSettings().HasInspirationValveResistance())
    {
      double resistance_cmH2O_s_Per_L = GetSettings().GetInspirationValveResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
      resistance_cmH2O_s_Per_L = MIN(resistance_cmH2O_s_Per_L, m_MachineClosedResistance_cmH2O_s_Per_L);
      m_InspiratoryLimbToYPiece->GetNextResistance().SetValue(resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    }
  }

  //--------------------------------------------------------------------------------------------------
/// \brief
/// Set ventilator circuit compliance.
///
/// \details
/// If no values are specified, defaults will be used.
//--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::SetCompliance()
  {
    if (GetSettings().HasCompliance())
    {
      double compliance_L_Per_cmH2O = GetSettings().GetCompliance(VolumePerPressureUnit::L_Per_cmH2O);
      double minCompliance_L_Per_cmH2O = 1e-20;
      compliance_L_Per_cmH2O = MIN(compliance_L_Per_cmH2O, minCompliance_L_Per_cmH2O);
      m_VentilatorToEnvironment->GetNextCompliance().SetValue(compliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Set ventialtor node/compartment volumes.
  ///
  /// \details
  /// If no values are specified, defaults will be used.
  //--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::SetVolumes()
  {
    if (GetSettings().HasExpirationLimbVolume())
    {
      m_ExpiratoryLimbNode->GetNextVolume().Set(GetSettings().GetExpirationLimbVolume());
    }
    else
    {
      //Volume doesn't reset to baseline like path elements
      m_ExpiratoryLimbNode->GetNextVolume().Set(m_ExpiratoryLimbNode->GetVolumeBaseline());
    }

    if (GetSettings().HasExpirationValveVolume())
    {
      m_ExpiratoryValveNode->GetNextVolume().Set(GetSettings().GetExpirationValveVolume());
    }
    else
    {
      //Volume doesn't reset to baseline like path elements
      m_ExpiratoryValveNode->GetNextVolume().Set(m_ExpiratoryValveNode->GetVolumeBaseline());
    }

    if (GetSettings().HasInspirationLimbVolume())
    {
      m_InspiratoryLimbNode->GetNextVolume().Set(GetSettings().GetInspirationLimbVolume());
    }
    else
    {
      //Volume doesn't reset to baseline like path elements
      m_InspiratoryLimbNode->GetNextVolume().Set(m_InspiratoryLimbNode->GetVolumeBaseline());
    }

    if (GetSettings().HasInspirationValveVolume())
    {
      m_InspiratoryValveNode->GetNextVolume().Set(GetSettings().GetInspirationValveVolume());
    }
    else
    {
      //Volume doesn't reset to baseline like path elements
      m_InspiratoryValveNode->GetNextVolume().Set(m_InspiratoryValveNode->GetVolumeBaseline());
    }

    if (GetSettings().HasYPieceVolume())
    {
      m_YPieceNode->GetNextVolume().Set(GetSettings().GetYPieceVolume());
    }
    else
    {
      //Volume doesn't reset to baseline like path elements
      m_YPieceNode->GetNextVolume().Set(m_YPieceNode->GetVolumeBaseline());
    }

    if (GetSettings().HasConnectionVolume())
    {
      m_ConnectionNode->GetNextVolume().Set(GetSettings().GetConnectionVolume());
    }
    else
    {
      //Volume doesn't reset to baseline like path elements
      m_ConnectionNode->GetNextVolume().Set(m_ConnectionNode->GetVolumeBaseline());
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates key respiratory parameters at the mechanical ventilator.
  //--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::CalculateRespiratoryParameters()
  {
    double ambientPressure_cmH2O = m_AmbientNode->GetPressure(PressureUnit::cmH2O);
    double airwayPressure_cmH2O = m_ConnectionNode->GetPressure(PressureUnit::cmH2O);
    GetAirwayPressure().SetValue(airwayPressure_cmH2O - ambientPressure_cmH2O, PressureUnit::cmH2O);
    m_MeanAirwayPressure_cmH2O->Sample(airwayPressure_cmH2O - ambientPressure_cmH2O);

    double inspiratoryFlow_L_Per_s = 0.0;
    double expiratoryFlow_L_Per_s = 0.0;
    if (m_YPieceToConnection->HasNextFlow())
    {
      inspiratoryFlow_L_Per_s = m_YPieceToConnection->GetNextFlow(VolumePerTimeUnit::L_Per_s);
      expiratoryFlow_L_Per_s = -inspiratoryFlow_L_Per_s;
    }
    GetInspiratoryFlow().SetValue(inspiratoryFlow_L_Per_s, VolumePerTimeUnit::L_Per_s);
    GetExpiratoryFlow().SetValue(expiratoryFlow_L_Per_s, VolumePerTimeUnit::L_Per_s);
    m_CurrentVentilatorVolume_L += inspiratoryFlow_L_Per_s * m_data.GetTimeStep_s();

    inspiratoryFlow_L_Per_s = 0.0;
    if (m_ConnectionToAirway->HasNextFlow())
    {
      inspiratoryFlow_L_Per_s = m_ConnectionToAirway->GetNextFlow(VolumePerTimeUnit::L_Per_s);
    }
    double previousRespiratoryVolume_L = m_CurrentRespiratoryVolume_L;
    m_CurrentRespiratoryVolume_L += inspiratoryFlow_L_Per_s * m_data.GetTimeStep_s();
    GetTotalLungVolume().SetValue(m_CurrentRespiratoryVolume_L, VolumeUnit::L);

    //Hold on to end of breath values at the transition point to inhaling
    //This is needed mostly due to trigger delays
    if (m_CurrentRespiratoryVolume_L <= previousRespiratoryVolume_L)
    {
      //Save for end of exhale
      if (m_ConnectionNode->HasNextPressure())
      {
        airwayPressure_cmH2O = m_ConnectionNode->GetNextPressure(PressureUnit::cmH2O);
      }
      m_PositiveEndExpiratoryPressure_cmH2O = airwayPressure_cmH2O - ambientPressure_cmH2O;

      m_EndTidalCarbonDioxideFraction = m_Connection->GetSubstanceQuantity(m_data.GetSubstances().GetCO2())->GetVolumeFraction().GetValue();
      m_EndTidalCarbonDioxidePressure_cmH2O = m_Connection->GetSubstanceQuantity(m_data.GetSubstances().GetCO2())->GetPartialPressure(PressureUnit::cmH2O);
      m_EndTidalOxygenFraction = m_Connection->GetSubstanceQuantity(m_data.GetSubstances().GetO2())->GetVolumeFraction().GetValue();
      m_EndTidalOxygenPressure_cmH2O = m_Connection->GetSubstanceQuantity(m_data.GetSubstances().GetO2())->GetPartialPressure(PressureUnit::cmH2O);
    }

    m_PeakExpiratoryFlow_L_Per_s = MAX(m_PeakExpiratoryFlow_L_Per_s, expiratoryFlow_L_Per_s);
    m_PeakInspiratoryFlow_L_Per_s = MAX(m_PeakInspiratoryFlow_L_Per_s, inspiratoryFlow_L_Per_s);
  }

  //--------------------------------------------------------------------------------------------------
/// \brief
/// Set key respiratory parameters at the end of inspiration.
//--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::CalculateInspiratoryRespiratoryParameters()
  {
    //Transition from inspiration to pause

    double ambientPressure_cmH2O = m_AmbientNode->GetNextPressure(PressureUnit::cmH2O);
    double airwayPressure_cmH2O = m_ConnectionNode->GetNextPressure(PressureUnit::cmH2O);
    GetPeakInspiratoryPressure().SetValue(airwayPressure_cmH2O - ambientPressure_cmH2O, PressureUnit::cmH2O);

    m_InspiratoryFlow_L_Per_s = m_ConnectionToAirway->GetNextFlow(VolumePerTimeUnit::L_Per_s);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Set key respiratory parameters at the end of inspiration.
  //--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::CalculatePauseRespiratoryParameters()
  {
    //Transition from pause to expiration

    GetInspiratoryTidalVolume().SetValue(m_CurrentVentilatorVolume_L, VolumeUnit::L);
    GetTidalVolume().SetValue(m_CurrentRespiratoryVolume_L, VolumeUnit::L);

    double flow_L_Per_s = m_YPieceToConnection->GetNextFlow(VolumePerTimeUnit::L_Per_s);
    double plateauPressure_cmH2O = 0.0;
    m_PauseOccurred = abs(flow_L_Per_s) < ZERO_APPROX;
    if (m_PauseOccurred)
    {
      //There is a plateau, so set it directly
      //if the plateau pressure is determined here, it is calculated later at the end of the breath
      double ambientPressure_cmH2O = m_AmbientNode->GetNextPressure(PressureUnit::cmH2O);
      double airwayPressure_cmH2O = m_ConnectionNode->GetNextPressure(PressureUnit::cmH2O);
      plateauPressure_cmH2O = airwayPressure_cmH2O - ambientPressure_cmH2O;
      GetPlateauPressure().SetValue(plateauPressure_cmH2O, PressureUnit::cmH2O);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Set key respiratory parameters at the end of expiration.
  //--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::CalculateExpiratoryRespiratoryParameters(bool patientTriggered)
  {
    //End of breath

    double totalTime_s = 0.0;
    double tidalVolume_L = GetTidalVolume(VolumeUnit::L);

    if (m_InspirationTime_s > 0.0) // Make sure we've actually done a breath
    {
      double expirationTime_s = m_CurrentPeriodTime_s;
      totalTime_s = m_InspirationTime_s + expirationTime_s;
      double respirationRate_Per_min = 0.0;
      if (totalTime_s > ZERO_APPROX)
        respirationRate_Per_min = 60.0 / totalTime_s;
      GetRespirationRate().SetValue(respirationRate_Per_min, FrequencyUnit::Per_min);
      double inspiratioryExpiratioryRatio = 0.0;
      if (expirationTime_s > ZERO_APPROX)
        inspiratioryExpiratioryRatio = m_InspirationTime_s / expirationTime_s;
      GetInspiratoryExpiratoryRatio().SetValue(inspiratioryExpiratioryRatio);
      GetTotalPulmonaryVentilation().SetValue(respirationRate_Per_min * tidalVolume_L, VolumePerTimeUnit::L_Per_min);
    }
    else
    {
      GetRespirationRate().SetValue(0.0, FrequencyUnit::Per_min);
      GetInspiratoryExpiratoryRatio().SetValue(0.0);
      GetTotalPulmonaryVentilation().SetValue(0.0, VolumePerTimeUnit::L_Per_min);
    }

    double inspiratoryTidalVolume_L = GetInspiratoryTidalVolume(VolumeUnit::L);
    double expiratoryTidalVolume_L = inspiratoryTidalVolume_L - m_CurrentVentilatorVolume_L;
    GetExpiratoryTidalVolume().SetValue(expiratoryTidalVolume_L, VolumeUnit::L);

    double leakFraction = 1.0;
    if (inspiratoryTidalVolume_L > ZERO_APPROX)
    {
      leakFraction = (inspiratoryTidalVolume_L - expiratoryTidalVolume_L) / inspiratoryTidalVolume_L;
    }
    BLIM(leakFraction, 0.0, 1.0);
    GetLeakFraction().SetValue(leakFraction);

    GetMeanAirwayPressure().SetValue(m_MeanAirwayPressure_cmH2O->Value(), PressureUnit::cmH2O);
    m_MeanAirwayPressure_cmH2O->Invalidate();

    GetPeakExpiratoryFlow().SetValue(m_PeakExpiratoryFlow_L_Per_s, VolumePerTimeUnit::L_Per_s);
    GetPeakInspiratoryFlow().SetValue(m_PeakInspiratoryFlow_L_Per_s, VolumePerTimeUnit::L_Per_s);

    double pressureDifference_cmH2O = GetPeakInspiratoryPressure(PressureUnit::cmH2O) - m_PositiveEndExpiratoryPressure_cmH2O;
    double compliance_L_Per_cmH2O = 0.0;
    if (pressureDifference_cmH2O > ZERO_APPROX && tidalVolume_L > 0.0)
      compliance_L_Per_cmH2O = tidalVolume_L / pressureDifference_cmH2O;
    GetDynamicRespiratoryCompliance().SetValue(compliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);

    double extrinsicPositiveEndExpiratoryPressureSetting_cmH2O = 0.0;
    if (GetSettings().HasPositiveEndExpiratoryPressure())
    {
      extrinsicPositiveEndExpiratoryPressureSetting_cmH2O = GetSettings().GetPositiveEndExpiratoryPressure(PressureUnit::cmH2O);
    }
    double intrinsicPositiveEndExpiratoryPressure_cmH2O = 0.0;
    double flow_L_Per_s = m_YPieceToConnection->GetNextFlow(VolumePerTimeUnit::L_Per_s);
    if (abs(flow_L_Per_s) < ZERO_APPROX)
    {
      //There is a plateau, so set it directly

      intrinsicPositiveEndExpiratoryPressure_cmH2O = m_PositiveEndExpiratoryPressure_cmH2O - extrinsicPositiveEndExpiratoryPressureSetting_cmH2O;
    }
    else if (HasStaticRespiratoryCompliance() && HasExpiratoryResistance() &&
      GetStaticRespiratoryCompliance(VolumePerPressureUnit::L_Per_cmH2O) != 0.0 &&
      GetExpiratoryResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L) != 0.0)
    {
      //There is not a plateau, so we will cheat and calculate it directly
      //Some real ventilators will use a an equation of motion least squares fitting

      double expirationTime_s = m_CurrentPeriodTime_s;
      double staticCompliance_L_Per_cmH2O = GetStaticRespiratoryCompliance(VolumePerPressureUnit::L_Per_cmH2O);
      double expiratoryResistance_cmH2O_s_Per_L = GetExpiratoryResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
      intrinsicPositiveEndExpiratoryPressure_cmH2O = (tidalVolume_L / staticCompliance_L_Per_cmH2O) * std::exp(-expirationTime_s / (expiratoryResistance_cmH2O_s_Per_L * staticCompliance_L_Per_cmH2O));
    }

    double totalPositiveEndExpiratoryPressure_cmH2O = extrinsicPositiveEndExpiratoryPressureSetting_cmH2O + intrinsicPositiveEndExpiratoryPressure_cmH2O;
    GetExtrinsicPositiveEndExpiratoryPressure().SetValue(extrinsicPositiveEndExpiratoryPressureSetting_cmH2O, PressureUnit::cmH2O);
    GetIntrinsicPositiveEndExpiratoryPressure().SetValue(intrinsicPositiveEndExpiratoryPressure_cmH2O, PressureUnit::cmH2O);
    GetTotalPositiveEndExpiratoryPressure().SetValue(totalPositiveEndExpiratoryPressure_cmH2O, PressureUnit::cmH2O);

    if (m_PauseOccurred)
    {
      //There is a plateau, so we will calculate static values based on that pressure

      pressureDifference_cmH2O = GetPlateauPressure(PressureUnit::cmH2O) - m_PositiveEndExpiratoryPressure_cmH2O;
      compliance_L_Per_cmH2O = 0.0;
      if (pressureDifference_cmH2O > ZERO_APPROX && tidalVolume_L > 0.0)
        compliance_L_Per_cmH2O = tidalVolume_L / pressureDifference_cmH2O;
      GetStaticRespiratoryCompliance().SetValue(compliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);

      pressureDifference_cmH2O = GetPlateauPressure(PressureUnit::cmH2O) - m_PositiveEndExpiratoryPressure_cmH2O;
      double resistance_cmH2O_s_Per_L = 0.0;
      if (m_PeakExpiratoryFlow_L_Per_s > ZERO_APPROX && pressureDifference_cmH2O > 0.0)
        resistance_cmH2O_s_Per_L = pressureDifference_cmH2O / m_PeakExpiratoryFlow_L_Per_s;
      GetExpiratoryResistance().SetValue(resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);

      pressureDifference_cmH2O = GetPeakInspiratoryPressure(PressureUnit::cmH2O) - GetPlateauPressure(PressureUnit::cmH2O);
      resistance_cmH2O_s_Per_L = 0.0;
      if (m_PeakInspiratoryFlow_L_Per_s > ZERO_APPROX && pressureDifference_cmH2O > 0.0)
        resistance_cmH2O_s_Per_L = pressureDifference_cmH2O / m_PeakInspiratoryFlow_L_Per_s;
      GetInspiratoryResistance().SetValue(resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    }
    else
    {
      //There is not a plateau, so we will cheat and get it from the respiratory system
      //Some real ventilators will use an equation of motion best fit... 
      // but I do not know how they estimate the effort/muscle pressure for assisted breathing

      GetStaticRespiratoryCompliance().Set(m_data.GetRespiratory().GetRespiratoryCompliance());
      GetExpiratoryResistance().Set(m_data.GetRespiratory().GetExpiratoryRespiratoryResistance());
      GetInspiratoryResistance().Set(m_data.GetRespiratory().GetInspiratoryRespiratoryResistance());

      double plateauPressure_cmH2O = 0.0;
      compliance_L_Per_cmH2O = GetStaticRespiratoryCompliance(VolumePerPressureUnit::L_Per_cmH2O);
      if (compliance_L_Per_cmH2O > ZERO_APPROX)
      {
        //There is no plateau, so back it out of the equation of motion
        double Vt_L = GetTidalVolume(VolumeUnit::L);
        plateauPressure_cmH2O = Vt_L / compliance_L_Per_cmH2O + totalPositiveEndExpiratoryPressure_cmH2O;
        GetPlateauPressure().SetValue(plateauPressure_cmH2O, PressureUnit::cmH2O);
      }
    }

    GetEndTidalCarbonDioxideFraction().SetValue(m_EndTidalCarbonDioxideFraction);
    GetEndTidalCarbonDioxidePressure().SetValue(m_EndTidalCarbonDioxidePressure_cmH2O, PressureUnit::cmH2O);
    GetEndTidalOxygenFraction().SetValue(m_EndTidalOxygenFraction);
    GetEndTidalOxygenPressure().SetValue(m_EndTidalOxygenPressure_cmH2O, PressureUnit::cmH2O);

    m_CurrentVentilatorVolume_L = 0.0;
    m_CurrentRespiratoryVolume_L = 0.0;
    m_PeakExpiratoryFlow_L_Per_s = 0.0;
    m_PeakInspiratoryFlow_L_Per_s = 0.0;

    m_TimeSincePatientTrigger_s += totalTime_s;
    if (patientTriggered)
    {
      m_TimeSincePatientTrigger_s = 0.0;
    }

    CheckAlarms();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Checks Relief Valve Pressure
  ///
  /// \details
  /// Assigns relief valve pressure as a pressure source based on the pressure setting and checks if 
  /// the status of the relief valve is open or closed.
  //--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::CheckReliefValve()
  {
    //Set the Pressure Source based on the setting
    double valvePressure_cmH2O = GetSettings().GetReliefValveThreshold(PressureUnit::cmH2O);
    m_EnvironmentToReliefValve->GetNextPressureSource().SetValue(valvePressure_cmH2O, PressureUnit::cmH2O);

    //Check to see if it reached the pressure threshold
    if (!m_data.GetEvents().IsEventActive(eEvent::MechanicalVentilatorReliefValveActive) && m_ConnectionToReliefValve->GetNextValve() == eGate::Closed)
    {
      m_data.GetEvents().SetEvent(eEvent::MechanicalVentilatorReliefValveActive, true, m_data.GetSimulationTime());
    }
    else if (m_data.GetEvents().IsEventActive(eEvent::MechanicalVentilatorReliefValveActive) && m_ConnectionToReliefValve->GetNextValve() == eGate::Open)
    {
      m_data.GetEvents().SetEvent(eEvent::MechanicalVentilatorReliefValveActive, false, m_data.GetSimulationTime());
    }

    //Always try to let it run without the relief valve open (i.e. not allowing flow), otherwise it will always stay shorted
    m_ConnectionToReliefValve->SetNextValve(eGate::Open);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Checks pressure-cycling alarm conditions during ventilation
  ///
  /// \details
  /// Monitors airway pressure against the high pressure threshold when pressure cycling is enabled.
  /// If the pressure limit is exceeded, triggers the high pressure alarm event and immediately
  /// cycles the ventilator to the expiration phase to prevent barotrauma.
  //--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::CheckCyclingAlarms()
  {
    // High Pressure Alarm
    if (GetSettings().GetAlarms().HasHighPressureThreshold() && 
      (GetSettings().GetAlarms().HasHighPressureCycleOption() && GetSettings().GetAlarms().GetHighPressureCycleOption() == eSwitch::On))
    {
      double highPressureThreshold_cmH2O = GetSettings().GetAlarms().GetHighPressureThreshold(PressureUnit::cmH2O);
      double ambientPressure_cmH2O = m_AmbientNode->GetPressure(PressureUnit::cmH2O);
      double airwayPressure_cmH2O = m_ConnectionNode->GetPressure(PressureUnit::cmH2O);
      bool triggered = (airwayPressure_cmH2O - ambientPressure_cmH2O) > highPressureThreshold_cmH2O;
      if (triggered)
      {
        if (!m_data.GetEvents().IsEventActive(eEvent::HighPressureAlarmTriggered))
        {
          m_data.GetEvents().SetEvent(eEvent::HighPressureAlarmTriggered, true, m_data.GetSimulationTime());
        }
        CycleMode(true);
        return;
      }
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Monitors and evaluates all mechanical ventilator alarm conditions
  ///
  /// \details
  /// Systematically checks each configured alarm threshold against current ventilator measurements
  /// and triggers or clears the corresponding alarm events based on whether measured values exceed
  /// their configured thresholds. Events are set active when limits are breached and cleared when
  /// conditions return to normal ranges or when alarm thresholds are disabled.
  //--------------------------------------------------------------------------------------------------
  void MechanicalVentilatorModel::CheckAlarms()
  {
    // High Pressure Alarm
    if (GetSettings().GetAlarms().HasHighPressureThreshold())
    {
      double highPressureThreshold_cmH2O = GetSettings().GetAlarms().GetHighPressureThreshold(PressureUnit::cmH2O);
      double peakInspiratoryPressure_cmH2O = GetPeakInspiratoryPressure(PressureUnit::cmH2O);
      bool triggered = peakInspiratoryPressure_cmH2O > highPressureThreshold_cmH2O;
      if (triggered && !m_data.GetEvents().IsEventActive(eEvent::HighPressureAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::HighPressureAlarmTriggered, true, m_data.GetSimulationTime());
      }
      else if (!triggered && m_data.GetEvents().IsEventActive(eEvent::HighPressureAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::HighPressureAlarmTriggered, false, m_data.GetSimulationTime());
      }
    }
    else if (m_data.GetEvents().IsEventActive(eEvent::HighPressureAlarmTriggered))
    {
      m_data.GetEvents().SetEvent(eEvent::HighPressureAlarmTriggered, false, m_data.GetSimulationTime());
    }

    // Low Pressure Alarm
    if (GetSettings().GetAlarms().HasLowPressureThreshold())
    {
      double lowPressureThreshold_cmH2O = GetSettings().GetAlarms().GetLowPressureThreshold(PressureUnit::cmH2O);
      double peakInspiratoryPressure_cmH2O = GetPeakInspiratoryPressure(PressureUnit::cmH2O);
      bool triggered = peakInspiratoryPressure_cmH2O < lowPressureThreshold_cmH2O;
      if (triggered && !m_data.GetEvents().IsEventActive(eEvent::LowPressureAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::LowPressureAlarmTriggered, true, m_data.GetSimulationTime());
      }
      else if (!triggered && m_data.GetEvents().IsEventActive(eEvent::LowPressureAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::LowPressureAlarmTriggered, false, m_data.GetSimulationTime());
      }
    }
    else if (m_data.GetEvents().IsEventActive(eEvent::LowPressureAlarmTriggered))
    {
      m_data.GetEvents().SetEvent(eEvent::LowPressureAlarmTriggered, false, m_data.GetSimulationTime());
    }

    // High Respiratory Rate Alarm
    if (GetSettings().GetAlarms().HasHighRespiratoryRateThreshold())
    {
      double highRespiratoryRateThreshold_Per_min = GetSettings().GetAlarms().GetHighRespiratoryRateThreshold(FrequencyUnit::Per_min);
      double respirationRate_Per_min = GetRespirationRate(FrequencyUnit::Per_min);
      bool triggered = respirationRate_Per_min > highRespiratoryRateThreshold_Per_min;
      if (triggered && !m_data.GetEvents().IsEventActive(eEvent::HighRespiratoryRateAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::HighRespiratoryRateAlarmTriggered, true, m_data.GetSimulationTime());
      }
      else if (!triggered && m_data.GetEvents().IsEventActive(eEvent::HighRespiratoryRateAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::HighRespiratoryRateAlarmTriggered, false, m_data.GetSimulationTime());
      }
    }
    else if (m_data.GetEvents().IsEventActive(eEvent::HighRespiratoryRateAlarmTriggered))
    {
      m_data.GetEvents().SetEvent(eEvent::HighRespiratoryRateAlarmTriggered, false, m_data.GetSimulationTime());
    }

    // Low Tidal Volume Alarm
    if (GetSettings().GetAlarms().HasLowTidalVolumeThreshold())
    {
      double lowTidalVolumeThreshold_mL = GetSettings().GetAlarms().GetLowTidalVolumeThreshold(VolumeUnit::mL);
      double tidalVolume_mL = GetTidalVolume(VolumeUnit::mL);
      bool triggered = tidalVolume_mL < lowTidalVolumeThreshold_mL;
      if (triggered && !m_data.GetEvents().IsEventActive(eEvent::LowTidalVolumeAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::LowTidalVolumeAlarmTriggered, true, m_data.GetSimulationTime());
      }
      else if (!triggered && m_data.GetEvents().IsEventActive(eEvent::LowTidalVolumeAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::LowTidalVolumeAlarmTriggered, false, m_data.GetSimulationTime());
      }
    }
    else if (m_data.GetEvents().IsEventActive(eEvent::LowTidalVolumeAlarmTriggered))
    {
      m_data.GetEvents().SetEvent(eEvent::LowTidalVolumeAlarmTriggered, false, m_data.GetSimulationTime());
    }

    // High Tidal Volume Alarm
    if (GetSettings().GetAlarms().HasHighTidalVolumeThreshold())
    {
      double highTidalVolumeThreshold_mL = GetSettings().GetAlarms().GetHighTidalVolumeThreshold(VolumeUnit::mL);
      double tidalVolume_mL = GetTidalVolume(VolumeUnit::mL);
      bool triggered = tidalVolume_mL > highTidalVolumeThreshold_mL;
      if (triggered && !m_data.GetEvents().IsEventActive(eEvent::HighTidalVolumeAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::HighTidalVolumeAlarmTriggered, true, m_data.GetSimulationTime());
      }
      else if (!triggered && m_data.GetEvents().IsEventActive(eEvent::HighTidalVolumeAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::HighTidalVolumeAlarmTriggered, false, m_data.GetSimulationTime());
      }
    }
    else if (m_data.GetEvents().IsEventActive(eEvent::HighTidalVolumeAlarmTriggered))
    {
      m_data.GetEvents().SetEvent(eEvent::HighTidalVolumeAlarmTriggered, false, m_data.GetSimulationTime());
    }

    // Low Minute Ventilation Alarm
    if (GetSettings().GetAlarms().HasLowMinuteVentilationThreshold())
    {
      double lowMinuteVentilationThreshold_L_Per_min = GetSettings().GetAlarms().GetLowMinuteVentilationThreshold(VolumePerTimeUnit::L_Per_min);
      double totalPulmonaryVentilation_L_Per_min = GetTotalPulmonaryVentilation(VolumePerTimeUnit::L_Per_min);
      bool triggered = totalPulmonaryVentilation_L_Per_min < lowMinuteVentilationThreshold_L_Per_min;
      if (triggered && !m_data.GetEvents().IsEventActive(eEvent::LowMinuteVentilationAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::LowMinuteVentilationAlarmTriggered, true, m_data.GetSimulationTime());
      }
      else if (!triggered && m_data.GetEvents().IsEventActive(eEvent::LowMinuteVentilationAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::LowMinuteVentilationAlarmTriggered, false, m_data.GetSimulationTime());
      }
    }
    else if (m_data.GetEvents().IsEventActive(eEvent::LowMinuteVentilationAlarmTriggered))
    {
      m_data.GetEvents().SetEvent(eEvent::LowMinuteVentilationAlarmTriggered, false, m_data.GetSimulationTime());
    }

    // High Minute Ventilation Alarm
    if (GetSettings().GetAlarms().HasHighMinuteVentilationThreshold())
    {
      double highMinuteVentilationThreshold_L_Per_min = GetSettings().GetAlarms().GetHighMinuteVentilationThreshold(VolumePerTimeUnit::L_Per_min);
      double totalPulmonaryVentilation_L_Per_min = GetTotalPulmonaryVentilation(VolumePerTimeUnit::L_Per_min);
      bool triggered = totalPulmonaryVentilation_L_Per_min > highMinuteVentilationThreshold_L_Per_min;
      if (triggered && !m_data.GetEvents().IsEventActive(eEvent::HighMinuteVentilationAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::HighMinuteVentilationAlarmTriggered, true, m_data.GetSimulationTime());
      }
      else if (!triggered && m_data.GetEvents().IsEventActive(eEvent::HighMinuteVentilationAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::HighMinuteVentilationAlarmTriggered, false, m_data.GetSimulationTime());
      }
    }
    else if (m_data.GetEvents().IsEventActive(eEvent::HighMinuteVentilationAlarmTriggered))
    {
      m_data.GetEvents().SetEvent(eEvent::HighMinuteVentilationAlarmTriggered, false, m_data.GetSimulationTime());
    }

    // Low End Tidal Carbon Dioxide Alarm
    if (GetSettings().GetAlarms().HasLowEndTidalCarbonDioxideThreshold())
    {
      double lowEndTidalCarbonDioxideThreshold_mmHg = GetSettings().GetAlarms().GetLowEndTidalCarbonDioxideThreshold(PressureUnit::mmHg);
      double endTidalCarbonDioxidePressure_mmHg = GetEndTidalCarbonDioxidePressure(PressureUnit::mmHg);
      bool triggered = endTidalCarbonDioxidePressure_mmHg < lowEndTidalCarbonDioxideThreshold_mmHg;
      if (triggered && !m_data.GetEvents().IsEventActive(eEvent::LowEndTidalCarbonDioxideAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::LowEndTidalCarbonDioxideAlarmTriggered, true, m_data.GetSimulationTime());
      }
      else if (!triggered && m_data.GetEvents().IsEventActive(eEvent::LowEndTidalCarbonDioxideAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::LowEndTidalCarbonDioxideAlarmTriggered, false, m_data.GetSimulationTime());
      }
    }
    else if (m_data.GetEvents().IsEventActive(eEvent::LowEndTidalCarbonDioxideAlarmTriggered))
    {
      m_data.GetEvents().SetEvent(eEvent::LowEndTidalCarbonDioxideAlarmTriggered, false, m_data.GetSimulationTime());
    }

    // High End Tidal Carbon Dioxide Alarm
    if (GetSettings().GetAlarms().HasHighEndTidalCarbonDioxideThreshold())
    {
      double highEndTidalCarbonDioxideThreshold_mmHg = GetSettings().GetAlarms().GetHighEndTidalCarbonDioxideThreshold(PressureUnit::mmHg);
      double endTidalCarbonDioxidePressure_mmHg = GetEndTidalCarbonDioxidePressure(PressureUnit::mmHg);
      bool triggered = endTidalCarbonDioxidePressure_mmHg > highEndTidalCarbonDioxideThreshold_mmHg;
      if (triggered && !m_data.GetEvents().IsEventActive(eEvent::HighEndTidalCarbonDioxideAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::HighEndTidalCarbonDioxideAlarmTriggered, true, m_data.GetSimulationTime());
      }
      else if (!triggered && m_data.GetEvents().IsEventActive(eEvent::HighEndTidalCarbonDioxideAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::HighEndTidalCarbonDioxideAlarmTriggered, false, m_data.GetSimulationTime());
      }
    }
    else if (m_data.GetEvents().IsEventActive(eEvent::HighEndTidalCarbonDioxideAlarmTriggered))
    {
      m_data.GetEvents().SetEvent(eEvent::HighEndTidalCarbonDioxideAlarmTriggered, false, m_data.GetSimulationTime());
    }

    // Low Positive End Expiratory Pressure Alarm
    if (GetSettings().GetAlarms().HasLowPositiveEndExpiratoryPressureThreshold())
    {
      double lowPositiveEndExpiratoryPressureThreshold_cmH2O = GetSettings().GetAlarms().GetLowPositiveEndExpiratoryPressureThreshold(PressureUnit::cmH2O);
      double totalPositiveEndExpiratoryPressure_cmH2O = GetTotalPositiveEndExpiratoryPressure(PressureUnit::cmH2O);
      bool triggered = totalPositiveEndExpiratoryPressure_cmH2O < lowPositiveEndExpiratoryPressureThreshold_cmH2O;
      if (triggered && !m_data.GetEvents().IsEventActive(eEvent::LowPositiveEndExpiratoryPressureAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::LowPositiveEndExpiratoryPressureAlarmTriggered, true, m_data.GetSimulationTime());
      }
      else if (!triggered && m_data.GetEvents().IsEventActive(eEvent::LowPositiveEndExpiratoryPressureAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::LowPositiveEndExpiratoryPressureAlarmTriggered, false, m_data.GetSimulationTime());
      }
    }
    else if (m_data.GetEvents().IsEventActive(eEvent::LowPositiveEndExpiratoryPressureAlarmTriggered))
    {
      m_data.GetEvents().SetEvent(eEvent::LowPositiveEndExpiratoryPressureAlarmTriggered, false, m_data.GetSimulationTime());
    }

    // High Positive End Expiratory Pressure Alarm
    if (GetSettings().GetAlarms().HasHighPositiveEndExpiratoryPressureThreshold())
    {
      double highPositiveEndExpiratoryPressureThreshold_cmH2O = GetSettings().GetAlarms().GetHighPositiveEndExpiratoryPressureThreshold(PressureUnit::cmH2O);
      double totalPositiveEndExpiratoryPressure_cmH2O = GetTotalPositiveEndExpiratoryPressure(PressureUnit::cmH2O);
      bool triggered = totalPositiveEndExpiratoryPressure_cmH2O > highPositiveEndExpiratoryPressureThreshold_cmH2O;
      if (triggered && !m_data.GetEvents().IsEventActive(eEvent::HighPositiveEndExpiratoryPressureAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::HighPositiveEndExpiratoryPressureAlarmTriggered, true, m_data.GetSimulationTime());
      }
      else if (!triggered && m_data.GetEvents().IsEventActive(eEvent::HighPositiveEndExpiratoryPressureAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::HighPositiveEndExpiratoryPressureAlarmTriggered, false, m_data.GetSimulationTime());
      }
    }
    else if (m_data.GetEvents().IsEventActive(eEvent::HighPositiveEndExpiratoryPressureAlarmTriggered))
    {
      m_data.GetEvents().SetEvent(eEvent::HighPositiveEndExpiratoryPressureAlarmTriggered, false, m_data.GetSimulationTime());
    }

    // Auto Positive End Expiratory Pressure Alarm (Intrinsic PEEP / Air Trapping)
    if (GetSettings().GetAlarms().HasAutoPositiveEndExpiratoryPressureThreshold())
    {
      double autoPositiveEndExpiratoryPressureThreshold_cmH2O = GetSettings().GetAlarms().GetAutoPositiveEndExpiratoryPressureThreshold(PressureUnit::cmH2O);
      double intrinsicPositiveEndExpiratoryPressure_cmH2O = GetIntrinsicPositiveEndExpiratoryPressure(PressureUnit::cmH2O);
      bool triggered = intrinsicPositiveEndExpiratoryPressure_cmH2O > autoPositiveEndExpiratoryPressureThreshold_cmH2O;
      if (triggered && !m_data.GetEvents().IsEventActive(eEvent::AutoPositiveEndExpiratoryPressureAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::AutoPositiveEndExpiratoryPressureAlarmTriggered, true, m_data.GetSimulationTime());
      }
      else if (!triggered && m_data.GetEvents().IsEventActive(eEvent::AutoPositiveEndExpiratoryPressureAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::AutoPositiveEndExpiratoryPressureAlarmTriggered, false, m_data.GetSimulationTime());
      }
    }
    else if (m_data.GetEvents().IsEventActive(eEvent::AutoPositiveEndExpiratoryPressureAlarmTriggered))
    {
      m_data.GetEvents().SetEvent(eEvent::AutoPositiveEndExpiratoryPressureAlarmTriggered, false, m_data.GetSimulationTime());
    }

    // Circuit Leak Alarm
    if (GetSettings().GetAlarms().HasCircuitLeakThreshold())
    {
      double circuitLeakThreshold = GetSettings().GetAlarms().GetCircuitLeakThreshold().GetValue();
      double leakFraction = GetLeakFraction().GetValue();
      bool triggered = leakFraction > circuitLeakThreshold;
      if (triggered && !m_data.GetEvents().IsEventActive(eEvent::CircuitLeakAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::CircuitLeakAlarmTriggered, true, m_data.GetSimulationTime());
      }
      else if (!triggered && m_data.GetEvents().IsEventActive(eEvent::CircuitLeakAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::CircuitLeakAlarmTriggered, false, m_data.GetSimulationTime());
      }
    }
    else if (m_data.GetEvents().IsEventActive(eEvent::CircuitLeakAlarmTriggered))
    {
      m_data.GetEvents().SetEvent(eEvent::CircuitLeakAlarmTriggered, false, m_data.GetSimulationTime());
    }

    // Low Oxygen Saturation Alarm
    if (GetSettings().GetAlarms().HasLowOxygenSaturationThreshold())
    {
      double lowOxygenSaturationThreshold = GetSettings().GetAlarms().GetLowOxygenSaturationThreshold().GetValue();
      double oxygenSaturation = m_data.GetBloodChemistry().GetOxygenSaturation().GetValue();
      bool triggered = oxygenSaturation < lowOxygenSaturationThreshold;
      if (triggered && !m_data.GetEvents().IsEventActive(eEvent::LowOxygenSaturationAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::LowOxygenSaturationAlarmTriggered, true, m_data.GetSimulationTime());
      }
      else if (!triggered && m_data.GetEvents().IsEventActive(eEvent::LowOxygenSaturationAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::LowOxygenSaturationAlarmTriggered, false, m_data.GetSimulationTime());
      }
    }
    else if (m_data.GetEvents().IsEventActive(eEvent::LowOxygenSaturationAlarmTriggered))
    {
      m_data.GetEvents().SetEvent(eEvent::LowOxygenSaturationAlarmTriggered, false, m_data.GetSimulationTime());
    }

    // High Oxygen Saturation Alarm
    if (GetSettings().GetAlarms().HasHighOxygenSaturationThreshold())
    {
      double highOxygenSaturationThreshold = GetSettings().GetAlarms().GetHighOxygenSaturationThreshold().GetValue();
      double oxygenSaturation = m_data.GetBloodChemistry().GetOxygenSaturation().GetValue();
      bool triggered = oxygenSaturation > highOxygenSaturationThreshold;
      if (triggered && !m_data.GetEvents().IsEventActive(eEvent::HighOxygenSaturationAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::HighOxygenSaturationAlarmTriggered, true, m_data.GetSimulationTime());
      }
      else if (!triggered && m_data.GetEvents().IsEventActive(eEvent::HighOxygenSaturationAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::HighOxygenSaturationAlarmTriggered, false, m_data.GetSimulationTime());
      }
    }
    else if (m_data.GetEvents().IsEventActive(eEvent::HighOxygenSaturationAlarmTriggered))
    {
      m_data.GetEvents().SetEvent(eEvent::HighOxygenSaturationAlarmTriggered, false, m_data.GetSimulationTime());
    }

    // Apnea Time Alarm
    if (GetSettings().GetAlarms().HasApneaTimeThreshold())
    {
      double apneaTimeThreshold_s = GetSettings().GetAlarms().GetApneaTimeThreshold(TimeUnit::s);
      bool triggered = m_TimeSincePatientTrigger_s > apneaTimeThreshold_s;
      if (triggered && !m_data.GetEvents().IsEventActive(eEvent::ApneaTimeAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::ApneaTimeAlarmTriggered, true, m_data.GetSimulationTime());
      }
      else if (!triggered && m_data.GetEvents().IsEventActive(eEvent::ApneaTimeAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::ApneaTimeAlarmTriggered, false, m_data.GetSimulationTime());
      }
    }
    else if (m_data.GetEvents().IsEventActive(eEvent::ApneaTimeAlarmTriggered))
    {
      m_data.GetEvents().SetEvent(eEvent::ApneaTimeAlarmTriggered, false, m_data.GetSimulationTime());
      m_TimeSincePatientTrigger_s = 0.0;
    }
    else
    {
      m_TimeSincePatientTrigger_s = 0.0;
    }

    // Oxygen Supply Failure Alarm
    if (GetSettings().GetAlarms().HasOxygenSupplyFailureThreshold())
    {
      double oxygenSupplyFailureThreshold = GetSettings().GetAlarms().GetOxygenSupplyFailureThreshold().GetValue();
      double measuredFiO2 = m_Connection->GetSubstanceQuantity(m_data.GetSubstances().GetO2())->GetVolumeFraction().GetValue();
      bool triggered = measuredFiO2 < oxygenSupplyFailureThreshold;
      if (triggered && !m_data.GetEvents().IsEventActive(eEvent::OxygenSupplyFailureAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::OxygenSupplyFailureAlarmTriggered, true, m_data.GetSimulationTime());
      }
      else if (!triggered && m_data.GetEvents().IsEventActive(eEvent::OxygenSupplyFailureAlarmTriggered))
      {
        m_data.GetEvents().SetEvent(eEvent::OxygenSupplyFailureAlarmTriggered, false, m_data.GetSimulationTime());
      }
    }
    else if (m_data.GetEvents().IsEventActive(eEvent::OxygenSupplyFailureAlarmTriggered))
    {
      m_data.GetEvents().SetEvent(eEvent::OxygenSupplyFailureAlarmTriggered, false, m_data.GetSimulationTime());
    }
  }
END_NAMESPACE
