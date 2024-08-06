/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/PulseConfiguration.h"
#include "engine/common/controller/CircuitManager.h"
#include "engine/common/controller/CompartmentManager.h"
#include "engine/common/controller/SubstanceManager.h"
#include "engine/common/system/equipment/BagValveMaskModel.h"

#include "cdm/system/equipment/bag_valve_mask/actions/SEBagValveMaskConfiguration.h"
#include "cdm/system/equipment/bag_valve_mask/actions/SEBagValveMaskAutomated.h"
#include "cdm/system/equipment/bag_valve_mask/actions/SEBagValveMaskInstantaneous.h"
#include "cdm/system/equipment/bag_valve_mask/actions/SEBagValveMaskSqueeze.h"
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
#include "cdm/patient/SEPatient.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarPressureTimePerVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/utils/GeneralMath.h"

namespace pulse
{
  /*
  ========================
    Constructors
  ========================
  */

  BagValveMaskModel::BagValveMaskModel(Data& data) : BagValveMaskSystem(data.GetLogger()), Model(data)
  {
    Clear();
  }

  BagValveMaskModel::~BagValveMaskModel()
  {
    Clear();
  }

  void BagValveMaskModel::Clear()
  {
    BagValveMaskSystem::Clear();
    m_Environment = nullptr;
    m_Reservoir = nullptr;
    m_ReservoirAerosol = nullptr;
    m_FilterNode = nullptr;
    m_ConnectionNode = nullptr;
    m_ValveNode = nullptr;
    m_ReservoirToBag = nullptr;
    m_BagToValve = nullptr;
    m_ValveToFilter = nullptr;
    m_FilterToConnection = nullptr;
    m_ConnectionToEnvironment = nullptr;
    m_DefaultClosedFlowResistance_cmH2O_s_Per_L = 0;
    m_LastAction = nullptr;
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Initializes system properties to valid homeostatic values.
  //--------------------------------------------------------------------------------------------------
  void BagValveMaskModel::Initialize()
  {
    Model::Initialize();
    SetConnection(eSwitch::Off);
    m_BreathState = eBreathState::NoBreath;
    m_CurrentPeriodTime_s = 0.0;
    m_SqueezePressure_cmH2O = SEScalar::dNaN();
    m_SqueezeFlow_L_Per_s = SEScalar::dNaN();
    StateChange();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Initializes parameters for the bag valve mask class
  ///
  /// \details
  /// Initializes member variables and system level values on the common data model.
  //--------------------------------------------------------------------------------------------------
  void BagValveMaskModel::SetUp()
  {
    // Compartments
    m_Environment = m_data.GetCompartments().GetGasCompartment(pulse::EnvironmentCompartment::Ambient);
    m_Reservoir = m_data.GetCompartments().GetGasCompartment(pulse::BagValveMaskCompartment::Reservoir);
    m_ReservoirAerosol = m_data.GetCompartments().GetLiquidCompartment(pulse::BagValveMaskCompartment::Reservoir);

    // Nodes
    m_FilterNode = m_data.GetCircuits().GetBagValveMaskCircuit().GetNode(pulse::BagValveMaskNode::Filter);
    m_ConnectionNode = m_data.GetCircuits().GetBagValveMaskCircuit().GetNode(pulse::BagValveMaskNode::Connection);
    m_ValveNode = m_data.GetCircuits().GetBagValveMaskCircuit().GetNode(pulse::BagValveMaskNode::Valve);

    // Paths
    m_ReservoirToBag = m_data.GetCircuits().GetBagValveMaskCircuit().GetPath(pulse::BagValveMaskPath::ReservoirToBag);
    m_BagToValve = m_data.GetCircuits().GetBagValveMaskCircuit().GetPath(pulse::BagValveMaskPath::BagToValve);
    m_ValveToFilter = m_data.GetCircuits().GetBagValveMaskCircuit().GetPath(pulse::BagValveMaskPath::ValveToFilter);
    m_FilterToConnection = m_data.GetCircuits().GetBagValveMaskCircuit().GetPath(pulse::BagValveMaskPath::FilterToConnection);
    m_ConnectionToEnvironment = m_data.GetCircuits().GetBagValveMaskCircuit().GetPath(pulse::BagValveMaskPath::ConnectionToEnvironment);

    // Configuration
    m_DefaultClosedFlowResistance_cmH2O_s_Per_L = m_data.GetConfiguration().GetDefaultClosedFlowResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);

    if (m_data.GetActions().GetEquipmentActions().HasBagValveMaskInstantaneous())
      m_LastAction = &m_data.GetActions().GetEquipmentActions().GetBagValveMaskInstantaneous();
    else if (m_data.GetActions().GetEquipmentActions().HasBagValveMaskSqueeze())
      m_LastAction = &m_data.GetActions().GetEquipmentActions().GetBagValveMaskSqueeze();
    else if (m_data.GetActions().GetEquipmentActions().HasBagValveMaskAutomated())
      m_LastAction = &m_data.GetActions().GetEquipmentActions().GetBagValveMaskAutomated();
  }

  void BagValveMaskModel::StateChange()
  {
    m_LastAction = nullptr;
    UpdateAirwayMode();
    if (m_data.GetAirwayMode() != eAirwayMode::BagValveMask)
      return;
    m_BreathState = eBreathState::EquipmentInhale;
    m_CurrentPeriodTime_s = 0.0;

    // If you have one substance, make sure its Oxygen and add the standard CO2 and N2 to fill the difference

    //Set the substance volume fractions ********************************************
    std::vector<SESubstanceFraction*> gasFractions = GetFractionInspiredGases();

    //Reset the substance quantities at the connection
    for (SEGasSubstanceQuantity* subQ : m_Reservoir->GetSubstanceQuantities())
      subQ->SetToZero();

    //Start by setting everything to ambient
    for (auto s : m_Environment->GetSubstanceQuantities())
    {
      m_Reservoir->GetSubstanceQuantity(s->GetSubstance())->GetVolumeFraction().Set(s->GetVolumeFraction());
    }

    //Has fractions defined
    for (auto f : gasFractions)
    {
      const SESubstance& sub = f->GetSubstance();
      double fraction = f->GetFractionAmount().GetValue();

      //Do this, just in case it's something new
      m_data.GetSubstances().AddActiveSubstance(sub);

      //Now set it on the compartment
      //It has a infinate volume, so this will keep the same volume fraction no matter what's going on around it
      m_Reservoir->GetSubstanceQuantity(sub)->GetVolumeFraction().SetValue(fraction);
    }

    double totalFractionDefined = 0.0;
    for (auto s : m_Reservoir->GetSubstanceQuantities())
    {
      totalFractionDefined += m_Reservoir->GetSubstanceQuantity(s->GetSubstance())->GetVolumeFraction().GetValue();
    }

    //Add or remove Nitrogen to balance
    double gasFractionDiff = 1.0 - totalFractionDefined;
    double currentN2Fraction = m_Reservoir->GetSubstanceQuantity(m_data.GetSubstances().GetN2())->GetVolumeFraction().GetValue();
    if (currentN2Fraction + gasFractionDiff < 0.0)
    {
      double FiO2 = m_Reservoir->GetSubstanceQuantity(m_data.GetSubstances().GetO2())->GetVolumeFraction().GetValue();

      m_ss << "FiO2 setting + ambient fractions other than N2 is greater than 1.0. Setting FiO2 to max value of " << FiO2 + currentN2Fraction + gasFractionDiff << ".";
      Error(m_ss);
      m_Reservoir->GetSubstanceQuantity(m_data.GetSubstances().GetN2())->GetVolumeFraction().SetValue(0.0);
      m_Reservoir->GetSubstanceQuantity(m_data.GetSubstances().GetO2())->GetVolumeFraction().SetValue(FiO2 + currentN2Fraction + gasFractionDiff);
    }
    else
    {
      Info("Adding " + cdm::to_string(currentN2Fraction + gasFractionDiff) + "% of N2 to the system");
      m_Reservoir->GetSubstanceQuantity(m_data.GetSubstances().GetN2())->GetVolumeFraction().SetValue(currentN2Fraction + gasFractionDiff);
    }

    //Set the aerosol concentrations ********************************************
    std::vector<SESubstanceConcentration*> liquidConcentrations = GetConcentrationInspiredAerosols();

    //Reset the substance quantities
    for (SELiquidSubstanceQuantity* subQ : m_ReservoirAerosol->GetSubstanceQuantities())
      subQ->SetToZero();

    if (!liquidConcentrations.empty())
    {
      //Has fractions defined
      for (auto f : liquidConcentrations)
      {
        const SESubstance& sub = f->GetSubstance();
        SEScalarMassPerVolume concentration = f->GetConcentration();

        //Do this, just in case it's something new
        m_data.GetSubstances().AddActiveSubstance(sub);
        //Now set it on the connection compartment
        //It has infinite volume, so this will keep the same volume fraction no matter what's going on around it
        m_ReservoirAerosol->GetSubstanceQuantity(sub)->GetConcentration().Set(concentration);
      }
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Connect to the patient via the mask, a tube, or no connection
  ///
  /// \param  enumBagValveMaskConnection
  /// Connectoin type : Mask, tube, or off
  /// 
  /// \details
  /// If the enum is set to mask, then the mask is applied to the face
  /// If the enum is set to tube, then the BVM is connected to the tube
  /// If the enum is set to off, the airway mode is set to free.
  //--------------------------------------------------------------------------------------------------
  void BagValveMaskModel::UpdateAirwayMode()
  {
    eSwitch c = GetConnection();
    if (c == eSwitch::On)
    {
      m_data.SetAirwayMode(eAirwayMode::BagValveMask);
      return;
    }
    else if (c == eSwitch::Off && m_data.GetAirwayMode() == eAirwayMode::BagValveMask)
    {
      // Make sure we are active to make sure we go back to free
      m_data.SetAirwayMode(eAirwayMode::Free);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Removes the connection to the patient
  ///
  /// \details
  /// If the mask is on or the tube is connected, it is removed and the airway mode is set to free. The action is then removed from 
  /// the action manager.
  //--------------------------------------------------------------------------------------------------
  void BagValveMaskModel::InvalidateConnection()
  {
    // Set airway mode to free
    m_data.SetAirwayMode(eAirwayMode::Free);
    // THEN invalidate
    m_Connection = eSwitch::Off;
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Initializes gas volumes and volume fractions supplied by the bag valve mask depending on the airway mode
  ///
  /// \details
  /// The gas volumes and volume fractions are initialized and updated based on the airway mode (mask, free, or tube)
  /// and the volume associated with each airway mode.
  //--------------------------------------------------------------------------------------------------
  void BagValveMaskModel::UpdateConnection()
  {
    switch (m_data.GetAirwayMode())
    {
    case eAirwayMode::Free:
    {
      if (GetConnection() != eSwitch::Off)
        SetConnection(eSwitch::Off);
      break;
    }
    case eAirwayMode::BagValveMask:
    {
      if (GetConnection() == eSwitch::Off)
        SetConnection(eSwitch::On);
      break;
    }
    default:
    {
      // Something else is hooked up
      break;
    }
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Bag Valve Mask preprocess function
  ///
  /// \details
  /// Applies all the settings and calculates the instantaneous driver value.
  //--------------------------------------------------------------------------------------------------
  void BagValveMaskModel::PreProcess()
  {
    if (GetConnection() == eSwitch::Off && !m_data.GetActions().GetEquipmentActions().HasBagValveMaskConfiguration())
    {
      if (m_data.GetActions().GetEquipmentActions().HasBagValveMaskAutomated())
      {
        Error("The Bag Valve Mask must be connected (i.e., connection type configured) to run the automated action.");
        m_data.GetActions().GetEquipmentActions().RemoveBagValveMaskAutomated();
      }
      if (m_data.GetActions().GetEquipmentActions().HasBagValveMaskInstantaneous())
      {
        Error("The Bag Valve Mask must be connected (i.e., connection type configured) to run the Instantaneous action.");
        m_data.GetActions().GetEquipmentActions().RemoveBagValveMaskInstantaneous();
      }
      if (m_data.GetActions().GetEquipmentActions().HasBagValveMaskSqueeze())
      {
        Error("The Bag Valve Mask must be connected (i.e., connection type configured) to run the Squeeze action.");
        m_data.GetActions().GetEquipmentActions().RemoveBagValveMaskSqueeze();
      }
      return;
    }

    if (m_data.GetActions().GetEquipmentActions().HasBagValveMaskConfiguration())
    {
      ProcessConfiguration(m_data.GetActions().GetEquipmentActions().GetBagValveMaskConfiguration(), m_data.GetSubstances());
      m_data.GetActions().GetEquipmentActions().RemoveBagValveMaskConfiguration();
      StateChange();
    }

    UpdateConnection();
    // BVM is being turned off, remove any bvm actions and stop our preprocess
    if (GetConnection() == eSwitch::Off)
    {
      m_BreathState = eBreathState::NoBreath;
      m_CurrentPeriodTime_s = 0.0;

      m_data.GetActions().GetEquipmentActions().RemoveBagValveMaskAutomated();
      m_data.GetActions().GetEquipmentActions().RemoveBagValveMaskInstantaneous();
      m_data.GetActions().GetEquipmentActions().RemoveBagValveMaskSqueeze();
      m_LastAction = nullptr;
      return;
    }

    if (m_data.GetActions().GetEquipmentActions().HasBagValveMaskInstantaneous())
    {
      CheckLastAction(&m_data.GetActions().GetEquipmentActions().GetBagValveMaskInstantaneous());
      if (m_data.GetActions().GetEquipmentActions().GetBagValveMaskInstantaneous().HasPressure())
      {
        m_SqueezeFlow_L_Per_s = SEScalar::dNaN();
        m_SqueezePressure_cmH2O = m_data.GetActions().GetEquipmentActions().GetBagValveMaskInstantaneous().GetPressure(PressureUnit::cmH2O);
      }
      else if (m_data.GetActions().GetEquipmentActions().GetBagValveMaskInstantaneous().HasFlow())
      {
        m_SqueezeFlow_L_Per_s = m_data.GetActions().GetEquipmentActions().GetBagValveMaskInstantaneous().GetFlow(VolumePerTimeUnit::L_Per_s);
        m_SqueezePressure_cmH2O = SEScalar::dNaN();
      }
      SetSqeezeDriver();
      return;
    }

    m_BreathFrequency = nullptr;
    m_InspiratoryExpiratoryRatio = nullptr;
    m_SqueezePressure = nullptr;
    m_SqueezeVolume = nullptr;
    m_ExpiratoryPeriod = nullptr;
    m_InspiratoryPeriod = nullptr;

    if (m_data.GetActions().GetEquipmentActions().HasBagValveMaskSqueeze())
    {
      CheckLastAction(&m_data.GetActions().GetEquipmentActions().GetBagValveMaskSqueeze());
      if (m_data.GetActions().GetEquipmentActions().GetBagValveMaskSqueeze().HasSqueezePressure())
        m_SqueezePressure = &m_data.GetActions().GetEquipmentActions().GetBagValveMaskSqueeze().GetSqueezePressure();
      if (m_data.GetActions().GetEquipmentActions().GetBagValveMaskSqueeze().HasSqueezeVolume())
        m_SqueezeVolume = &m_data.GetActions().GetEquipmentActions().GetBagValveMaskSqueeze().GetSqueezeVolume();
      if (m_data.GetActions().GetEquipmentActions().GetBagValveMaskSqueeze().HasExpiratoryPeriod())
        m_ExpiratoryPeriod = &m_data.GetActions().GetEquipmentActions().GetBagValveMaskSqueeze().GetExpiratoryPeriod();
      if (m_data.GetActions().GetEquipmentActions().GetBagValveMaskSqueeze().HasExpiratoryPeriod())
        m_InspiratoryPeriod = &m_data.GetActions().GetEquipmentActions().GetBagValveMaskSqueeze().GetInspiratoryPeriod();
    }
    else if (m_data.GetActions().GetEquipmentActions().HasBagValveMaskAutomated())
    {
      CheckLastAction(&m_data.GetActions().GetEquipmentActions().GetBagValveMaskAutomated());
      if (m_data.GetActions().GetEquipmentActions().GetBagValveMaskAutomated().HasBreathFrequency())
        m_BreathFrequency = &m_data.GetActions().GetEquipmentActions().GetBagValveMaskAutomated().GetBreathFrequency();
      if (m_data.GetActions().GetEquipmentActions().GetBagValveMaskAutomated().HasInspiratoryExpiratoryRatio())
        m_InspiratoryExpiratoryRatio = &m_data.GetActions().GetEquipmentActions().GetBagValveMaskAutomated().GetInspiratoryExpiratoryRatio();
      if (m_data.GetActions().GetEquipmentActions().GetBagValveMaskAutomated().HasSqueezePressure())
        m_SqueezePressure = &m_data.GetActions().GetEquipmentActions().GetBagValveMaskAutomated().GetSqueezePressure();
      if (m_data.GetActions().GetEquipmentActions().GetBagValveMaskAutomated().HasSqueezeVolume())
        m_SqueezeVolume = &m_data.GetActions().GetEquipmentActions().GetBagValveMaskAutomated().GetSqueezeVolume();
    }
    else
    {
      //Hold this pressure until an action is called or the BVM is turned off
      m_SqueezePressure_cmH2O = 0.0;
      if (HasValvePositiveEndExpiratoryPressure())
      {
        m_SqueezePressure_cmH2O = GetValvePositiveEndExpiratoryPressure(PressureUnit::cmH2O);
      }
      m_SqueezeFlow_L_Per_s = SEScalar::dNaN();

      SetSqeezeDriver();
      return;
    }

    CalculateInspiration();
    CalculateExpiration();
    SetSqeezeDriver();
    SetResistances();
    SetVolumes();

    ///\todo We should add the ability to manually handle source flow and reservoir volume like the NonRebreatherMask.
    ///      This would entail adding something like ReservoirSourceFlow and ReservoirVolume (default = 2.5 L).
    ///      If the volume goes to zero, ambient substance values should be used

    m_CurrentPeriodTime_s += m_data.GetTimeStep_s();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Bag Valve Mask process function
  ///
  /// \details
  /// The current Pulse implementation has no specific process functionality for the bag valve mask.
  /// Bag Valve Mask processing is currently done in the Respiratory System with the combined circuit methodology.
  //--------------------------------------------------------------------------------------------------
  void BagValveMaskModel::Process(bool /*solve_and_transport*/)
  {
    ComputeExposedModelParameters();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Bag Valve Mask postprocess function
  ///
  /// \details
  /// The substance volumes and the volume fractions are updated for all of the nodes in the bag valve mask
  /// circuit during post process. 
  /// The volumes are updated based on the previously calculated nodal analysis.
  //--------------------------------------------------------------------------------------------------
  void BagValveMaskModel::PostProcess(bool /*solve_and_transport*/)
  {

  }

  void BagValveMaskModel::ComputeExposedModelParameters()
  {

  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Set the instantaneous squeeze pressure or flow on the circuit source.
  //--------------------------------------------------------------------------------------------------
  void BagValveMaskModel::SetSqeezeDriver()
  {
    if (!std::isnan(m_SqueezePressure_cmH2O) && !std::isnan(m_SqueezeFlow_L_Per_s))
    {
      Error("Bag Valve Mask squeeze pressure and flow both set, only one allowed. Using the pressure value.");
    }
    else if (std::isnan(m_SqueezePressure_cmH2O) && std::isnan(m_SqueezeFlow_L_Per_s))
    {
      Error(" Bag Valve Mask squeeze pressure or flow must be set. Using a pressure of 0.");

      m_SqueezePressure_cmH2O = 0.0;
    }

    bool stateChange = false;
    if (!std::isnan(m_SqueezePressure_cmH2O))
    {
      if (m_ReservoirToBag->HasNextFlowSource())
      {
        stateChange = true;
        m_ReservoirToBag->RemoveFlowSource();
      }
      if (!m_ReservoirToBag->HasNextPressureSource())
      {
        stateChange = true;
        m_ReservoirToBag->GetPressureSourceBaseline().SetValue(0, PressureUnit::cmH2O);
      }
      m_ReservoirToBag->GetNextPressureSource().SetValue(m_SqueezePressure_cmH2O, PressureUnit::cmH2O);
    }
    else if (!std::isnan(m_SqueezeFlow_L_Per_s))
    {
      if (m_ReservoirToBag->HasNextPressureSource())
      {
        stateChange = true;
        m_ReservoirToBag->RemovePressureSource();
      }
      if (!m_ReservoirToBag->HasNextFlowSource())
      {
        stateChange = true;
        m_ReservoirToBag->GetFlowSourceBaseline().SetValue(0, VolumePerTimeUnit::L_Per_s);
      }
      m_ReservoirToBag->GetNextFlowSource().SetValue(m_SqueezeFlow_L_Per_s, VolumePerTimeUnit::L_Per_s);
    }
    if (stateChange)
      m_data.GetCircuits().GetRespiratoryAndBagValveMaskCircuit().StateChange();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Determine the instantaneous squeeze pressure during inspiration.
  //--------------------------------------------------------------------------------------------------
  void BagValveMaskModel::CalculateInspiration()
  {
    if (m_BreathState != eBreathState::EquipmentInhale)
    {
      return;
    }

    // Check trigger
    // Give defaults, if not user defined
    double breathFrequency_Per_s = 12.0 / 60.0; //12 bpm
    if (m_BreathFrequency != nullptr)
    {
      breathFrequency_Per_s = m_BreathFrequency->GetValue(FrequencyUnit::Per_s);
    }

    double totalBreathTime_s = 1.0 / breathFrequency_Per_s;

    double inspiratoryExpiratoryRatio = 0.5; //Default
    if (m_InspiratoryExpiratoryRatio != nullptr)
    {
      inspiratoryExpiratoryRatio = m_InspiratoryExpiratoryRatio->GetValue();
    }

    double inspirationPeriod_s = inspiratoryExpiratoryRatio * totalBreathTime_s / (1.0 + inspiratoryExpiratoryRatio);

    if (m_InspiratoryPeriod != nullptr)
    {
      inspirationPeriod_s = m_InspiratoryPeriod->GetValue(TimeUnit::s);
    }

    if (m_CurrentPeriodTime_s >= inspirationPeriod_s)
    {
      CycleMode();
      return;
    }

    // Calculate source - constant pressure or flow during inspiration phase
    if (m_SqueezePressure != nullptr)
    {
      m_SqueezePressure_cmH2O = m_SqueezePressure->GetValue(PressureUnit::cmH2O);
      m_SqueezeFlow_L_Per_s = SEScalar::dNaN();
    }
    else if (m_SqueezeVolume != nullptr)
    {
      m_SqueezeFlow_L_Per_s = m_SqueezeVolume->GetValue(VolumeUnit::L) / inspirationPeriod_s;
      m_SqueezePressure_cmH2O = SEScalar::dNaN();
    }
    else
    {
      // Set a default squeeze volume that attempts to acheive the ideal tidal volume (7 mL/kg(ideal))
      double idealBodyWeight_kg = m_data.GetInitialPatient().GetIdealBodyWeight(MassUnit::kg);
      double tidalVolume_mL = 7.0 * idealBodyWeight_kg;
      m_SqueezeFlow_L_Per_s = (tidalVolume_mL / 1000.0) / inspirationPeriod_s;
      m_SqueezePressure_cmH2O = SEScalar::dNaN();
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Determine the instantaneous squeeze pressure during expiration.
  //--------------------------------------------------------------------------------------------------
  void BagValveMaskModel::CalculateExpiration()
  {
    if (m_BreathState != eBreathState::EquipmentExhale)
    {
      return;
    }

    // Check trigger
    // Give defaults, if not user defined
    double breathFrequency_Per_s = 12.0 / 60.0; //12 bpm
    if (m_BreathFrequency != nullptr)
    {
      breathFrequency_Per_s = m_BreathFrequency->GetValue(FrequencyUnit::Per_s);
    }

    double totalBreathTime_s = 1.0 / breathFrequency_Per_s;

    double inspiratoryExpiratoryRatio = 0.5; //Default
    if (m_InspiratoryExpiratoryRatio != nullptr)
    {
      inspiratoryExpiratoryRatio = m_InspiratoryExpiratoryRatio->GetValue();
    }

    double inspirationPeriod_s = inspiratoryExpiratoryRatio * totalBreathTime_s / (1.0 + inspiratoryExpiratoryRatio);
    double expirationPeriod_s = totalBreathTime_s - inspirationPeriod_s;

    if (m_ExpiratoryPeriod != nullptr)
    {
      expirationPeriod_s = m_ExpiratoryPeriod->GetValue(TimeUnit::s);
    }

    if (m_CurrentPeriodTime_s >= expirationPeriod_s)
    {
      CycleMode();
      return;
    }

    m_SqueezePressure_cmH2O = 0.0;
    if (HasValvePositiveEndExpiratoryPressure())
    {
      m_SqueezePressure_cmH2O = GetValvePositiveEndExpiratoryPressure(PressureUnit::cmH2O);
    }

    m_SqueezeFlow_L_Per_s = SEScalar::dNaN();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Go to next breath mode based on current mode.
  ///
  /// \details
  /// Inhale to exhale or exhale to inhale.
  //--------------------------------------------------------------------------------------------------
  void BagValveMaskModel::CycleMode()
  {
    m_CurrentPeriodTime_s = 0.0;

    if (m_BreathState == eBreathState::EquipmentInhale)
    {
      m_BreathState = eBreathState::EquipmentExhale;
    }
    else if (m_BreathState == eBreathState::EquipmentExhale)
    {
      m_BreathState = eBreathState::EquipmentInhale;

      //End of breath, so remove the squeeze action
      if (m_data.GetActions().GetEquipmentActions().HasBagValveMaskSqueeze())
      {
        m_data.GetActions().GetEquipmentActions().RemoveBagValveMaskSqueeze();
        Info("BVM Squeeze Complete");
      }
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Set Bag Valve Mask circuit resistances.
  ///
  /// \details
  /// If no values are specified, defaults will be used.
  //--------------------------------------------------------------------------------------------------
  void BagValveMaskModel::SetResistances()
  {
    if (HasBagResistance())
    {
      double resistance_cmH2O_s_Per_L = GetBagResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
      resistance_cmH2O_s_Per_L = MIN(resistance_cmH2O_s_Per_L, m_DefaultClosedFlowResistance_cmH2O_s_Per_L);
      m_BagToValve->GetNextResistance().SetValue(resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    }

    if (HasValveResistance())
    {
      double resistance_cmH2O_s_Per_L = GetValveResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
      resistance_cmH2O_s_Per_L = MIN(resistance_cmH2O_s_Per_L, m_DefaultClosedFlowResistance_cmH2O_s_Per_L);
      m_ValveToFilter->GetNextResistance().SetValue(resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    }

    if (HasFilterResistance())
    {
      double resistance_cmH2O_s_Per_L = GetFilterResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
      resistance_cmH2O_s_Per_L = MIN(resistance_cmH2O_s_Per_L, m_DefaultClosedFlowResistance_cmH2O_s_Per_L);
      m_FilterToConnection->GetNextResistance().SetValue(resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    }

    if (HasSealResistance())
    {
      double resistance_cmH2O_s_Per_L = GetSealResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
      resistance_cmH2O_s_Per_L = MIN(resistance_cmH2O_s_Per_L, m_DefaultClosedFlowResistance_cmH2O_s_Per_L);
      m_ConnectionToEnvironment->GetNextResistance().SetValue(resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Set Bag Valve Mask node/compartment volumes.
  ///
  /// \details
  /// If no values are specified, defaults will be used.
  //--------------------------------------------------------------------------------------------------
  void BagValveMaskModel::SetVolumes()
  {
    if (HasFilterVolume())
    {
      m_FilterNode->GetNextVolume().Set(GetFilterVolume());
    }
    else
    {
      //Volume doesn't reset to baseline like path elements
      m_FilterNode->GetNextVolume().Set(m_FilterNode->GetVolumeBaseline());
    }

    if (HasConnectionVolume())
    {
      m_ConnectionNode->GetNextVolume().Set(GetConnectionVolume());
    }
    else
    {
      //Volume doesn't reset to baseline like path elements
      m_ConnectionNode->GetNextVolume().Set(m_ConnectionNode->GetVolumeBaseline());
    }

    if (HasValveVolume())
    {
      m_ValveNode->GetNextVolume().Set(GetValveVolume());
    }
    else
    {
      //Volume doesn't reset to baseline like path elements
      m_ValveNode->GetNextVolume().Set(m_ValveNode->GetVolumeBaseline());
    }
  }

//--------------------------------------------------------------------------------------------------
/// \brief
/// Check if the Bag Valve Mask action changed from one type to another
///
/// \details
/// We need to reset timing if we chang to a new type without stopping the previous action
/// This was put in when going from Automated to Squeeze, skipped the first squeeze due
/// to the squeeze timing picking up on the tail end of an automated squeeze
//--------------------------------------------------------------------------------------------------
  void BagValveMaskModel::CheckLastAction(SEBagValveMaskAction* a)
  {
    if (m_LastAction == nullptr)
    {
      m_LastAction = a;
      return;
    }

    if (m_LastAction != a)
    {
      m_LastAction = a;
      m_CurrentPeriodTime_s = 0;
      m_BreathState = eBreathState::EquipmentInhale;
    }
  }
END_NAMESPACE
