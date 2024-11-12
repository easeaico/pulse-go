/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/PulseConfiguration.h"
#include "engine/common/controller/CircuitManager.h"
#include "engine/common/controller/CompartmentManager.h"
#include "engine/common/controller/SubstanceManager.h"
#include "engine/common/system/physiology/CardiovascularModel.h"
#include "engine/common/system/physiology/Saturation.h"
   // Conditions
#include "cdm/engine/SEConditionManager.h"
#include "cdm/patient/conditions/SEChronicAnemia.h"
#include "cdm/patient/conditions/SEChronicPericardialEffusion.h"
#include "cdm/patient/conditions/SEChronicRenalStenosis.h"
// Actions
#include "cdm/engine/SEActionManager.h"
#include "cdm/engine/SEPatientActionCollection.h"
#include "cdm/patient/actions/SEArrhythmia.h"
#include "cdm/patient/actions/SEBrainInjury.h"
#include "cdm/patient/actions/SECardiovascularMechanicsModification.h"
#include "cdm/patient/actions/SEChestCompression.h"
#include "cdm/patient/actions/SEChestCompressionAutomated.h"
#include "cdm/patient/actions/SEChestCompressionInstantaneous.h"
#include "cdm/patient/actions/SEHemorrhage.h"
#include "cdm/patient/actions/SEHemothorax.h"
#include "cdm/patient/actions/SEPericardialEffusion.h"
// Dependent Systems
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SECardiovascularMechanicsModifiers.h"
#include "cdm/system/physiology/SEDrugSystem.h"
#include "cdm/system/physiology/SEEnergySystem.h"
#include "cdm/system/physiology/SENervousSystem.h"
// CDM
#include "cdm/patient/SEPatient.h"
#include "cdm/engine/SEEventManager.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceTransport.h"
#include "cdm/circuit/fluid/SEFluidCircuit.h"
#include "cdm/circuit/fluid/SEFluidCircuitCalculator.h"
#include "cdm/compartment/fluid/SELiquidCompartmentGraph.h"
#include "cdm/compartment/fluid/SEGasCompartment.h"
#include "cdm/compartment/substances/SELiquidSubstanceQuantity.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarLength.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarForce.h"
#include "cdm/properties/SEScalarElectricPotential.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarPressurePerVolume.h"
#include "cdm/properties/SEScalarPressureTimePerVolume.h"
#include "cdm/properties/SEScalarPower.h"
#include "cdm/properties/SEScalarTemperature.h"
#include "cdm/properties/SEScalarAmountPerVolume.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalarVolumePerPressure.h"
#include "cdm/properties/SEScalarVolumePerTimeArea.h"
#include "cdm/properties/SEScalarArea.h"
#include "cdm/properties/SEScalarPressureTimePerVolumeArea.h"
#include "cdm/properties/SERunningAverage.h"
#include "cdm/properties/SETemporalInterpolator.h"
#include "cdm/utils/DataTrack.h"
#include "cdm/utils/GeneralMath.h"

//Flag for setting things constant to test
//Should be commented out, unless debugging/tuning
//#define TUNING

namespace pulse
{
  // Helper method adding a hierarchy of compartments for sampling
  void SampleCompartment(SECompartmentManager& cmptMgr, SELiquidCompartment& cmpt)
  {
    cmptMgr.SampleByCardiacCyle(cmpt);
    if (cmpt.HasChildren())
      for (SELiquidCompartment* child : cmpt.GetChildren())
        SampleCompartment(cmptMgr, *child);
  }

  CardiovascularModel::CardiovascularModel(Data& data) : CardiovascularSystem(data.GetLogger()), Model(data)
  {
    m_Transporter = new SELiquidTransporter(VolumePerTimeUnit::mL_Per_s, VolumeUnit::mL, MassUnit::ug, MassPerVolumeUnit::ug_Per_mL, data.GetLogger());
    m_CircuitCalculator = new SEFluidCircuitCalculator(VolumePerPressureUnit::mL_Per_mmHg, VolumePerTimeUnit::mL_Per_s, PressureTimeSquaredPerVolumeUnit::mmHg_s2_Per_mL, PressureUnit::mmHg, VolumeUnit::mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL, data.GetLogger());
    m_CardiacCycleArterialPressure_mmHg = new SERunningAverage();
    m_CardiacCycleArterialCO2PartialPressure_mmHg = new SERunningAverage();
    m_CardiacCyclePulmonaryCapillariesWedgePressure_mmHg = new SERunningAverage();
    m_CardiacCyclePulmonaryCapillariesFlow_mL_Per_s = new SERunningAverage();
    m_CardiacCyclePulmonaryShuntFlow_mL_Per_s = new SERunningAverage();
    m_CardiacCyclePulmonaryArteryPressure_mmHg = new SERunningAverage();
    m_CardiacCycleCentralVenousPressure_mmHg = new SERunningAverage();
    m_CardiacCycleSkinFlow_mL_Per_s = new SERunningAverage();

    m_HeartRateBaseline_Per_min = new SETemporalInterpolator();
    m_HeartComplianceModifier = new SETemporalInterpolator();
    m_AortaComplianceModifier = new SETemporalInterpolator();
    m_VenaCavaComplianceModifier = new SETemporalInterpolator();
    m_RespiratoryComplianceModifier = new SETemporalInterpolator();
    m_SystemicVascularResistanceModifier = new SETemporalInterpolator();
    m_SystemicVascularComplianceModifier = new SETemporalInterpolator();
    Clear();
  }

  CardiovascularModel::~CardiovascularModel()
  {
    Clear();

    delete m_Transporter;
    delete m_CircuitCalculator;
    delete m_CardiacCycleArterialPressure_mmHg;
    delete m_CardiacCycleArterialCO2PartialPressure_mmHg;
    delete m_CardiacCyclePulmonaryCapillariesWedgePressure_mmHg;
    delete m_CardiacCyclePulmonaryCapillariesFlow_mL_Per_s;
    delete m_CardiacCyclePulmonaryShuntFlow_mL_Per_s;
    delete m_CardiacCyclePulmonaryArteryPressure_mmHg;
    delete m_CardiacCycleCentralVenousPressure_mmHg;
    delete m_CardiacCycleSkinFlow_mL_Per_s;

    delete m_HeartRateBaseline_Per_min;
    delete m_HeartComplianceModifier;
    delete m_AortaComplianceModifier;
    delete m_VenaCavaComplianceModifier;
    delete m_RespiratoryComplianceModifier;
    delete m_SystemicVascularResistanceModifier;
    delete m_SystemicVascularComplianceModifier;
  }

  void CardiovascularModel::Clear()
  {
    CardiovascularSystem::Clear();

    m_CirculatoryCircuit = nullptr;
    m_CirculatoryGraph = nullptr;

    m_AortaCompliancePath = nullptr;
    m_AortaResistancePath = nullptr;
    m_VenaCavaCompliancePath = nullptr;
    m_RightHeartResistancePath = nullptr;

    m_InternalHemorrhageToAorta = nullptr;
    m_AortaToBrain = nullptr;
    m_BrainToVenaCava = nullptr;
    m_AortaToMyocardium = nullptr;
    m_MyocardiumToVenaCava = nullptr;

    m_GndToAbdominalCavity = nullptr;
    m_AbdominalCavityToGnd = nullptr;

    m_GndToPericardium = nullptr;
    m_PericardiumToGnd = nullptr;
    m_RightHeartToGnd = nullptr;
    m_RightHeart = nullptr;
    m_LeftHeartToGnd = nullptr;
    m_LeftHeart = nullptr;
    m_LeftHeartToAorta = nullptr;

    m_Aorta = nullptr;
    m_AortaCO2 = nullptr;
    m_Ground = nullptr;
    m_LeftHeart = nullptr;
    m_LeftPulmonaryCapillaries = nullptr;
    m_LeftPulmonaryArteries = nullptr;
    m_LeftPulmonaryVeins = nullptr;
    m_Pericardium = nullptr;
    m_RightHeart = nullptr;
    m_RightPulmonaryCapillaries = nullptr;
    m_RightPulmonaryArteries = nullptr;
    m_RightPulmonaryVeins = nullptr;
    m_VenaCava = nullptr;
    m_AbdominalCavity = nullptr;

    m_LeftPleuralCavity = nullptr;
    m_RightPleuralCavity = nullptr;
    m_PleuralCavity = nullptr;
    m_Ambient = nullptr;

    m_StartSystole = false;
    m_StartCardiacArrest = false;
    m_TransitionArrhythmia = false;
    m_MAPCollapse_mmHg = 20;
    m_MinIndividialSystemicResistance_mmHg_s_Per_mL = 0.1;

    m_HeartCompliancePaths.clear();
    m_AortaCompliancePaths.clear();
    m_AortaResistancePaths.clear();
    m_VenaCavaCompliancePaths.clear();
    m_VenaCavaResistancePaths.clear();
    m_RespiratoryCompliancePaths.clear();
    m_PulmonaryResistancePaths.clear();
    m_SystemicCompliancePaths.clear();
    m_SystemicResistancePaths.clear();
    m_MuscleResistancePaths.clear();
    m_SkinPaths.clear();

    m_LeftPulmonaryArteriesToVeins.clear();
    m_LeftPulmonaryArteriesToCapillaries.clear();
    m_RightPulmonaryArteriesToVeins.clear();
    m_RightPulmonaryArteriesToCapillaries.clear();

    m_CardiacCycleArterialPressure_mmHg->Invalidate();
    m_CardiacCycleArterialCO2PartialPressure_mmHg->Invalidate();
    m_CardiacCyclePulmonaryCapillariesWedgePressure_mmHg->Invalidate();
    m_CardiacCyclePulmonaryCapillariesFlow_mL_Per_s->Invalidate();
    m_CardiacCyclePulmonaryShuntFlow_mL_Per_s->Invalidate();
    m_CardiacCyclePulmonaryArteryPressure_mmHg->Invalidate();
    m_CardiacCycleCentralVenousPressure_mmHg->Invalidate();
    m_CardiacCycleSkinFlow_mL_Per_s->Invalidate();
    m_LeftCardiacCyclePerfusionVolumes_mL.clear();
    m_RightCardiacCyclePerfusionVolumes_mL.clear();


    m_HeartRateBaseline_Per_min->Invalidate();
    m_HeartComplianceModifier->Invalidate();
    m_AortaComplianceModifier->Invalidate();
    m_VenaCavaComplianceModifier->Invalidate();
    m_RespiratoryComplianceModifier->Invalidate();
    m_SystemicVascularResistanceModifier->Invalidate();
    m_SystemicVascularComplianceModifier->Invalidate();

    DELETE_MAP_SECOND(m_HemorrhageTrack);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Initializes system properties.
  ///
  /// \details
  /// For stabilization only!
  /// Called AFTER Setup when stabilizing a new patient
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::Initialize()
  {
    Model::Initialize();

    m_HeartRhythm = eHeartRhythm::NormalSinus;

    m_StartSystole = false;
    m_HeartFlowDetected = false;
    m_FullyCompressedHeart = false;

    m_DriverCyclePeriod_s = 60 / m_data.GetCurrentPatient().GetHeartRateBaseline(FrequencyUnit::Per_min);
    m_CurrentDriverCycleTime_s = 0.0;

    //Heart Elastance Parameters
    m_LeftHeartElastance_mmHg_Per_mL = 0.0;
    m_LeftHeartElastanceModifier = 1.0; //Utilized for reducing the maximum elastance to represent left ventricular systolic dysfunction
    m_RightHeartElastance_mmHg_Per_mL = 0.0;
    m_LeftHeartElastanceMin_mmHg_Per_mL  = m_data.GetConfiguration().GetLeftHeartElastanceMinimum(PressurePerVolumeUnit::mmHg_Per_mL);
    m_RightHeartElastanceMin_mmHg_Per_mL = m_data.GetConfiguration().GetRightHeartElastanceMinimum(PressurePerVolumeUnit::mmHg_Per_mL);
    m_LeftHeartElastanceMax_mmHg_Per_mL  = m_data.GetConfiguration().GetLeftHeartElastanceMaximum(PressurePerVolumeUnit::mmHg_Per_mL);
    m_RightHeartElastanceMax_mmHg_Per_mL = m_data.GetConfiguration().GetRightHeartElastanceMaximum(PressurePerVolumeUnit::mmHg_Per_mL);

    // Arrhythmia Parameters
    m_StartCardiacArrest = false;
    m_TransitionArrhythmia = false;
    m_CardiacArrestVitalsUpdateTimer_s = 0;

    m_EnableFeedbackAfterArrhythmiaTrasition = eSwitch::On;

    m_StabilizedHeartRateBaseline_Per_min = 0;
    m_StabilizedMeanArterialPressureBaseline_mmHg = 0;
    m_HeartRateBaseline_Per_min->Set(m_data.GetCurrentPatient().GetHeartRateBaseline(FrequencyUnit::Per_min));
    m_HeartComplianceModifier->Set(1.0);
    m_AortaComplianceModifier->Set(1.0);
    m_VenaCavaComplianceModifier->Set(1.0);
    m_RespiratoryComplianceModifier->Set(1.0);
    m_SystemicVascularResistanceModifier->Set(1.0);
    m_SystemicVascularComplianceModifier->Set(1.0);

    // CPR
    m_CompressionFrequencyCurrentTime_s = 0;
    m_CompressionFrequencyDuration_s = 0;
    m_CompressionPeriod_s = 0;
    m_CompressionPeriodCurrentTime_s = 0;

    //Initialize system data based on patient file inputs
    GetHeartRate().Set(m_data.GetCurrentPatient().GetHeartRateBaseline());
    GetBloodVolume().Set(m_data.GetCurrentPatient().GetBloodVolumeBaseline());
    GetSystolicArterialPressure().Set(m_data.GetCurrentPatient().GetSystolicArterialPressureBaseline());
    GetDiastolicArterialPressure().Set(m_data.GetCurrentPatient().GetDiastolicArterialPressureBaseline());
    m_CardiacCycleAortaPressureHigh_mmHg = m_data.GetCurrentPatient().GetSystolicArterialPressureBaseline(PressureUnit::mmHg);
    m_CardiacCycleAortaPressureLow_mmHg = m_data.GetCurrentPatient().GetDiastolicArterialPressureBaseline(PressureUnit::mmHg);
    m_CardiacCycleDiastolicVolume_mL = 0.0;
    m_CardiacCycleLeftHeartPressureHigh_mmHg = m_CardiacCycleAortaPressureHigh_mmHg;
    m_CardiacCycleLeftHeartPressureLow_mmHg = m_CardiacCycleAortaPressureLow_mmHg;
    m_CardiacCycleStrokeVolume_mL = 0;
    m_CardiacCyclePulmonaryArteryPressureHigh_mmHg = 26;
    m_CardiacCyclePulmonaryArteryPressureLow_mmHg = 9;
    m_CardiacCycleRightHeartPressureHigh_mmHg = m_CardiacCycleAortaPressureHigh_mmHg;
    m_CardiacCycleRightHeartPressureLow_mmHg = m_CardiacCycleAortaPressureLow_mmHg;
    GetMeanArterialPressure().SetValue((2. / 3. * m_CardiacCycleAortaPressureLow_mmHg) + (1. / 3. * m_CardiacCycleAortaPressureHigh_mmHg), PressureUnit::mmHg);
    m_CardiacCycleArterialPressure_mmHg->Sample(GetMeanArterialPressure().GetValue(PressureUnit::mmHg));
    GetPulmonaryMeanArterialPressure().SetValue(15, PressureUnit::mmHg);
    RecordAndResetCardiacCycle();

    // Set system data based on physiology norms
    GetMeanCentralVenousPressure().SetValue(5.0, PressureUnit::mmHg);
    m_CardiacCycleArterialCO2PartialPressure_mmHg->Sample(60.0);
    m_LastCardiacCycleMeanArterialCO2PartialPressure_mmHg = 60.0;
    GetMeanArterialCarbonDioxidePartialPressure().SetValue(60, PressureUnit::mmHg);
    GetMeanArterialCarbonDioxidePartialPressureDelta().SetValue(0.0, PressureUnit::mmHg);
    GetPulmonaryCapillariesWedgePressure().SetValue(0, PressureUnit::mmHg);
    GetPulmonaryMeanArterialPressure().SetValue(90, PressureUnit::mmHg);
    GetPulmonaryArterialPressure().SetValue(90, PressureUnit::mmHg);
    GetPulmonaryMeanCapillaryFlow().SetValue(0, VolumePerTimeUnit::mL_Per_s);
    GetPulmonaryMeanShuntFlow().SetValue(0, VolumePerTimeUnit::mL_Per_s);
    GetTotalPulmonaryPerfusion().SetValue(0, VolumePerTimeUnit::mL_Per_s);
    GetPulmonaryCapillariesCoverageFraction().SetValue(m_data.GetConfiguration().GetStandardPulmonaryCapillaryCoverage());

    GetMeanSkinFlow().SetValue(0, VolumePerTimeUnit::mL_Per_s);
    GetCardiacOutput().SetValue(m_data.GetCurrentPatient().GetSex() == ePatient_Sex::Male ? 5600 : 4900, VolumePerTimeUnit::mL_Per_min);
    GetHeartStrokeVolume().SetValue(GetCardiacOutput(VolumePerTimeUnit::mL_Per_min)/GetHeartRate(FrequencyUnit::Per_min), VolumeUnit::mL);
    GetHeartEjectionFraction().SetValue(0.55);
    GetCardiacIndex().SetValue(3.0, VolumePerTimeAreaUnit::mL_Per_min_m2);
    GetPulmonaryVascularResistance().SetValue(0.14, PressureTimePerVolumeUnit::mmHg_min_Per_mL);
    GetPulmonaryVascularResistanceIndex().SetValue(0.082, PressureTimePerVolumeAreaUnit::mmHg_min_Per_mL_m2);


    CalculateHeartElastance();

    double systemicVascularResistance_mmHg_s_Per_mL = (GetMeanArterialPressure().GetValue(PressureUnit::mmHg) - GetMeanCentralVenousPressure().GetValue(PressureUnit::mmHg)) / GetCardiacOutput().GetValue(VolumePerTimeUnit::mL_Per_s);
    GetSystemicVascularResistance().SetValue(systemicVascularResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    // This is not part of stabilization due to not knowing when we hit the patient parameters with a circuit configuration
    TuneCircuit();
    systemicVascularResistance_mmHg_s_Per_mL = (GetMeanArterialPressure().GetValue(PressureUnit::mmHg) - GetMeanCentralVenousPressure().GetValue(PressureUnit::mmHg)) / GetCardiacOutput().GetValue(VolumePerTimeUnit::mL_Per_s);
    GetSystemicVascularResistance().SetValue(systemicVascularResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Initializes parameters for Cardiovascular Class
  ///
  /// \details
  /// Called during both State loading and Patient Stabilization
  /// Pull and setup up our data (can be from other systems)
  /// Initialize will be called after this and can overwrite any of this data (only if stabilizing)
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::SetUp()
  {
    // Grab the modifiers pointer from the action manager
    m_MechanicsModifiers = &m_data.GetActions().GetPatientActions().GetCardiovascularMechanicsModification().GetModifiers();
    m_MechanicsModifiers->Activate(); // Ensure all multipliers have a value so we can write cleaner code

    m_HemorrhageTrack.clear();

    m_LeftCardiacCyclePerfusionVolumes_mL.clear();
    m_RightCardiacCyclePerfusionVolumes_mL.clear();

    //Compartments
    m_Aorta = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Aorta);
    m_AortaCO2 = m_Aorta->GetSubstanceQuantity(m_data.GetSubstances().GetCO2());
    m_Ground = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Ground);
    m_LeftPulmonaryArteries = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftPulmonaryArteries);
    m_RightPulmonaryArteries = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightPulmonaryArteries);
    m_LeftPulmonaryVeins = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftPulmonaryVeins);
    m_RightPulmonaryVeins = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightPulmonaryVeins);
    m_LeftPulmonaryCapillaries = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftPulmonaryCapillaries);
    m_RightPulmonaryCapillaries = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightPulmonaryCapillaries);
    m_VenaCava = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::VenaCava);
    m_Pericardium = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Pericardium);
    m_LeftHeart = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftHeart);
    m_RightHeart = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightHeart);
    m_Abdomen = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Abdomen);
    m_AbdominalCavity = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::AbdominalCavity);
    m_LeftArm = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftArm);
    m_LeftLeg = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftLeg);
    m_RightArm = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightArm);
    m_RightLeg = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightLeg);
    //Respiratory Compartments
    m_LeftPleuralCavity = m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::LeftPleuralCavity);
    m_RightPleuralCavity = m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::RightPleuralCavity);
    m_PleuralCavity = m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::PleuralCavity);
    m_Ambient = m_data.GetCompartments().GetGasCompartment(pulse::EnvironmentCompartment::Ambient);

    //Circuits
    m_CirculatoryCircuit = &m_data.GetCircuits().GetActiveCardiovascularCircuit();
    m_CirculatoryGraph = &m_data.GetCompartments().GetActiveCardiovascularGraph();
    //Nodes
    m_GroundNode = m_CirculatoryCircuit->GetNode(pulse::CardiovascularNode::Ground);
    m_LeftPulmonaryVeinsNode = m_CirculatoryCircuit->GetNode(pulse::CardiovascularNode::LeftPulmonaryVeins1);
    m_RightPulmonaryVeinsNode = m_CirculatoryCircuit->GetNode(pulse::CardiovascularNode::RightPulmonaryVeins1);
    m_AbdominalCavityNode = m_CirculatoryCircuit->GetNode(pulse::CardiovascularNode::AbdominalCavity1);
    //Paths
    if (m_data.GetConfiguration().UseExpandedLungs() == eSwitch::On)
    {
      m_RightPulmonaryArteriesToVeins.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightSuperiorLobeApicalShunt));
      m_RightPulmonaryArteriesToVeins.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightSuperiorLobePosteriorShunt));
      m_RightPulmonaryArteriesToVeins.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightSuperiorLobeAnteriorShunt));
      m_RightPulmonaryArteriesToVeins.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightMiddleLobeLateralShunt));
      m_RightPulmonaryArteriesToVeins.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightMiddleLobeMedialShunt));
      m_RightPulmonaryArteriesToVeins.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeSuperiorShunt));
      m_RightPulmonaryArteriesToVeins.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeMedialBasalShunt));
      m_RightPulmonaryArteriesToVeins.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeAnteriorBasalShunt));
      m_RightPulmonaryArteriesToVeins.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeLateralBasalShunt));
      m_RightPulmonaryArteriesToVeins.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightInferiorLobePosteriorBasalShunt));

      m_LeftPulmonaryArteriesToVeins.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobePosteriorBasalShunt));
      m_LeftPulmonaryArteriesToVeins.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobeLateralBasalShunt));
      m_LeftPulmonaryArteriesToVeins.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobeAnteromedialBasalShunt));
      m_LeftPulmonaryArteriesToVeins.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobeSuperiorShunt));
      m_LeftPulmonaryArteriesToVeins.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeInferiorLingulaShunt));
      m_LeftPulmonaryArteriesToVeins.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeSuperiorLingulaShunt));
      m_LeftPulmonaryArteriesToVeins.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeAnteriorShunt));
      m_LeftPulmonaryArteriesToVeins.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeApicoposteriorShunt));

      m_RightPulmonaryArteriesToCapillaries.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightSuperiorLobeApicalPulmonaryCapillaries));
      m_RightPulmonaryArteriesToCapillaries.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightSuperiorLobePosteriorPulmonaryCapillaries));
      m_RightPulmonaryArteriesToCapillaries.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightSuperiorLobeAnteriorPulmonaryCapillaries));
      m_RightPulmonaryArteriesToCapillaries.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightMiddleLobeLateralPulmonaryCapillaries));
      m_RightPulmonaryArteriesToCapillaries.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightMiddleLobeMedialPulmonaryCapillaries));
      m_RightPulmonaryArteriesToCapillaries.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightInferiorLobeSuperiorPulmonaryCapillaries));
      m_RightPulmonaryArteriesToCapillaries.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightInferiorLobeMedialBasalPulmonaryCapillaries));
      m_RightPulmonaryArteriesToCapillaries.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightInferiorLobeAnteriorBasalPulmonaryCapillaries));
      m_RightPulmonaryArteriesToCapillaries.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightInferiorLobeLateralBasalPulmonaryCapillaries));
      m_RightPulmonaryArteriesToCapillaries.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightInferiorLobePosteriorBasalPulmonaryCapillaries));

      m_LeftPulmonaryArteriesToCapillaries.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftInferiorLobePosteriorBasalPulmonaryCapillaries));
      m_LeftPulmonaryArteriesToCapillaries.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftInferiorLobeLateralBasalPulmonaryCapillaries));
      m_LeftPulmonaryArteriesToCapillaries.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries));
      m_LeftPulmonaryArteriesToCapillaries.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftInferiorLobeSuperiorPulmonaryCapillaries));
      m_LeftPulmonaryArteriesToCapillaries.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries));
      m_LeftPulmonaryArteriesToCapillaries.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries));
      m_LeftPulmonaryArteriesToCapillaries.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftSuperiorLobeAnteriorPulmonaryCapillaries));
      m_LeftPulmonaryArteriesToCapillaries.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftSuperiorLobeApicoposteriorPulmonaryCapillaries));
    }
    else
    {
      m_LeftPulmonaryArteriesToVeins.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftPulmonaryArteries1ToLeftPulmonaryVeins1));
      m_LeftPulmonaryArteriesToCapillaries.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftPulmonaryArteries1ToLeftPulmonaryCapillaries1));

      m_RightPulmonaryArteriesToVeins.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::RightPulmonaryArteries1ToRightPulmonaryVeins1));
      m_RightPulmonaryArteriesToCapillaries.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::RightPulmonaryArteries1ToRightPulmonaryCapillaries1));
    }

    m_AortaCompliancePath = m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta1ToAorta4);
    m_AortaResistancePath = m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta3ToAorta1);
    m_VenaCavaCompliancePath = m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::VenaCava1ToGround);
    m_VenaCavaResistancePath = m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::VenaCava1ToRightHeart2);

    m_InternalHemorrhageToAorta = m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::GroundToAorta4);
    m_GndToAbdominalCavity = m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::GroundToAbdominalCavity1);
    m_AbdominalCavityToGnd = m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::AbdominalCavity1ToGround);

    m_LeftPulmonaryVeinsLeak =  m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftPulmonaryVeinsLeak1ToGround);
    m_RightPulmonaryVeinsLeak =  m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::RightPulmonaryVeinsLeak1ToGround);

    m_LeftRenalArteryPath = m_CirculatoryCircuit->GetPath(pulse::RenalPath::LeftRenalArteryToAfferentArteriole);
    m_RightRenalArteryPath = m_CirculatoryCircuit->GetPath(pulse::RenalPath::RightRenalArteryToAfferentArteriole);

    m_GndToPericardium = m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::GroundToPericardium1);
    m_PericardiumToGnd = m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Pericardium1ToGround);
    m_RightHeartToGnd = m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::RightHeart3ToGround);
    m_RightHeartCompliancePath = m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::RightHeart1ToRightHeart3);
    m_RightHeartResistancePath = m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::VenaCava1ToRightHeart2);
    m_LeftHeartToGnd = m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftHeart3ToGround);
    m_LeftHeartCompliancePath = m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftHeart1ToLeftHeart3);
    m_LeftHeartToAorta = m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftHeart1ToAorta2);

    m_RightPulmonaryVenousReturnResistancePath = m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::RightPulmonaryVeins1ToRightIntermediatePulmonaryVeins1);
    m_LeftPulmonaryVenousReturnResistancePath = m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftPulmonaryVeins1ToLeftIntermediatePulmonaryVeins1);

    // Setup Default vs. Expanded Circuit Parameters
    if (m_data.GetConfiguration().UseComputationalLifeExpansion() == eSwitch::On)
    {
      // For Metabolic Tone Response
      m_AortaToBrain = m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Aorta5ToIntracranial1);
      m_AortaToMyocardium = m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Aorta5ToMyocardium1);
      m_BrainToVenaCava = m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Intracranial3ToVenaCava2);
      m_MyocardiumToVenaCava = m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Myocardium3ToVenaCava2);
      m_MuscleResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Torso1ToTorsoMuscle1));
      m_MuscleResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::TorsoMuscle1ToTorso2));
      m_MuscleResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightArm1ToRightArmMuscle1));
      m_MuscleResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightArmMuscle1ToRightArm2));
      m_MuscleResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftArm1ToLeftArmMuscle1));
      m_MuscleResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftArmMuscle1ToLeftArm2));
      m_MuscleResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightLeg1ToRightLegMuscle1));
      m_MuscleResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightLegMuscle1ToRightLeg2));
      m_MuscleResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftLeg1ToLeftLegMuscle1));
      m_MuscleResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftLegMuscle1ToLeftLeg2));

      m_Brain = m_data.GetCompartments().GetLiquidCompartment(pulse::ComputationalLifeVascularCompartment::Intracranial);
      m_BrainResistanceDownstreamPath = m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Intracranial2ToIntracranial3);
      m_BrainResistanceUpstreamPath = m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Intracranial1ToIntracranial2);

      m_SkinPaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Torso1ToTorsoSkin1));
      m_SkinPaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightArm1ToRightArmSkin1));
      m_SkinPaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftArm1ToLeftArmSkin1));
      m_SkinPaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightLeg1ToRightLegSkin1));
      m_SkinPaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftLeg1ToLeftLegSkin1));

      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Extracranial1ToExtracranial2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Extracranial2ToExtracranial3));

      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Intracranial1ToIntracranial2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Intracranial2ToIntracranial3));

      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Myocardium1ToMyocardium2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Myocardium2ToMyocardium3));

      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Gut1ToLiver1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Gut1ToSplanchnic1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Gut1ToSpleen1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Gut1ToSmallIntestine1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Gut1ToLargeIntestine1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::PortalVein1ToLiver1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Liver1ToLiver2));

      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightArm1ToRightArmBone1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightArmBone1ToRightArm2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightArm1ToRightArmFat1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightArmFat1ToRightArm2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightArm1ToRightArmMuscle1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightArmMuscle1ToRightArm2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightArm1ToRightArmSkin1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightArmSkin1ToRightArm2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightArm1ToRightArmMicrovasculature1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightArmMicrovasculature1ToRightArm2));

      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftArm1ToLeftArmBone1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftArmBone1ToLeftArm2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftArm1ToLeftArmFat1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftArmFat1ToLeftArm2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftArm1ToLeftArmMuscle1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftArmMuscle1ToLeftArm2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftArm1ToLeftArmSkin1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftArmSkin1ToLeftArm2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftArm1ToLeftArmMicrovasculature1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftArmMicrovasculature1ToLeftArm2));

      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Torso1ToTorsoBone1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::TorsoBone1ToTorso2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Torso1ToTorsoFat1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::TorsoFat1ToTorso2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Torso1ToTorsoMuscle1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::TorsoMuscle1ToTorso2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Torso1ToTorsoSkin1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::TorsoSkin1ToTorso2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Torso1ToTorsoMicrovasculature1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::TorsoMicrovasculature1ToTorso2));

      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightLeg1ToRightLegBone1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightLegBone1ToRightLeg2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightLeg1ToRightLegFat1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightLegFat1ToRightLeg2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightLeg1ToRightLegMuscle1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightLegMuscle1ToRightLeg2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightLeg1ToRightLegSkin1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightLegSkin1ToRightLeg2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightLeg1ToRightLegMicrovasculature1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightLegMicrovasculature1ToRightLeg2));

      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftLeg1ToLeftLegBone1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftLegBone1ToLeftLeg2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftLeg1ToLeftLegFat1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftLegFat1ToLeftLeg2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftLeg1ToLeftLegMuscle1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftLegMuscle1ToLeftLeg2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftLeg1ToLeftLegSkin1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftLegSkin1ToLeftLeg2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftLeg1ToLeftLegMicrovasculature1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftLegMicrovasculature1ToLeftLeg2));


      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Extracranial1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Intracranial1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Extracranial3ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Intracranial3ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Myocardium1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Myocardium2ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Myocardium3ToGround));

      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::RightArm1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightArmBone1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightArmFat1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightArmMuscle1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightArmSkin1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightArmMicrovasculature1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightArm2ToGround));

      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftArm1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftArmBone1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftArmFat1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftArmMuscle1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftArmSkin1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftArmMicrovasculature1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftArm2ToGround));

      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Torso1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::TorsoBone1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::TorsoFat1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::TorsoMuscle1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::TorsoSkin1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::TorsoMicrovasculature1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Torso2ToGround));

      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::RightLeg1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightLegBone1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightLegFat1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightLegMuscle1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightLegSkin1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightLegMicrovasculature1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightLeg2ToGround));

      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftLeg1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftLegBone1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftLegFat1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftLegMuscle1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftLegSkin1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftLegMicrovasculature1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftLeg2ToGround));

      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Gut1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Liver1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::Liver2ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Splanchnic1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Spleen1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LargeIntestine1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::SmallIntestine1ToGround));

      if (m_data.GetConfiguration().UseExpandedKidneys() != eSwitch::On)
      {
        m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftKidney1ToLeftKidney2));
        m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::RightKidney1ToRightKidney2));
        m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::LeftKidney3ToVenaCava2));
        m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ComputationalLifeCardiovascularPath::RightKidney3ToVenaCava2));

        m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::RenalPath::LeftRenalArteryCompliance));
        m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::RenalPath::RightRenalArteryCompliance));

        m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::RenalPath::LeftRenalVeinCompliance));
        m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::RenalPath::RightRenalVeinCompliance));
      }
    }
    else
    {
      // For Metabolic Tone Response
      m_AortaToBrain = m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta1ToBrain1);
      m_BrainToVenaCava = m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Brain1ToBrain2);
      m_AortaToMyocardium = m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta1ToMyocardium1);
      m_MyocardiumToVenaCava = m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Myocardium1ToMyocardium2);
      m_MuscleResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta1ToMuscle1));
      m_MuscleResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Muscle1ToMuscle2));

      m_Brain = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Brain);
      m_BrainResistanceDownstreamPath = m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Brain1ToBrain2);
      m_BrainResistanceUpstreamPath = m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta1ToBrain1);

      m_SkinPaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta1ToSkin1));

      // Arterial Side
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta1ToBrain1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta1ToMyocardium1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta1ToRightArm1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta1ToLeftArm1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta1ToRightLeg1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta1ToLeftLeg1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta1ToFat1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta1ToMuscle1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta1ToSkin1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta1ToBone1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta1ToLiver1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta1ToSplanchnic1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta1ToSpleen1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta1ToLargeIntestine1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta1ToSmallIntestine1));
      // Venous Side
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Brain1ToBrain2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Myocardium1ToMyocardium2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::RightArm1ToRightArm2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftArm1ToLeftArm2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::RightLeg1ToRightLeg2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftLeg1ToLeftLeg2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Fat1ToFat2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Muscle1ToMuscle2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Skin1ToSkin2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Bone1ToBone2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Liver1ToLiver2));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LargeIntestine1ToPortalVein1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Splanchnic1ToPortalVein1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Spleen1ToPortalVein1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::SmallIntestine1ToPortalVein1));
      m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::PortalVein1ToLiver1));

      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Brain1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Myocardium1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::RightArm1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftArm1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::RightLeg1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftLeg1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Fat1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Muscle1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Skin1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Bone1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Liver1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Splanchnic1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Spleen1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LargeIntestine1ToGround));
      m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::SmallIntestine1ToGround));

      if (m_data.GetConfiguration().UseExpandedKidneys() != eSwitch::On)
      {
        m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta1ToLeftKidney1));
        m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::Aorta1ToRightKidney1));
        m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftKidney1ToLeftKidney2));
        m_SystemicResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::RightKidney1ToRightKidney2));

        //TODO: Aaron: isn't this path removed when the renal system is enabled? Shouldn't you take here the paths RightRenalArteryCompliance, RightGlomerularCapillariesCompliance, etc?
        // Probably you don't want to optimize/modify these values, but some blood volume are not considered in this way. Right?
        m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftKidney1ToGround));
        m_SystemicCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::RightKidney1ToGround));
      }
    }

    // Setup our vectors for vascular tone modifiers
    m_AortaCompliancePaths.push_back(m_AortaCompliancePath);
    m_VenaCavaCompliancePaths.push_back(m_VenaCavaCompliancePath);
    m_HeartCompliancePaths.push_back(m_LeftHeartCompliancePath);
    m_HeartCompliancePaths.push_back(m_RightHeartCompliancePath);

    m_AortaResistancePaths.push_back(m_AortaResistancePath);
    m_VenaCavaResistancePaths.push_back(m_VenaCavaResistancePath);

    if (m_data.GetConfiguration().UseExpandedLungs() == eSwitch::On)
    {
      m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightSuperiorLobeApicalPulmonaryCapillariesToGround));
      m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightSuperiorLobePosteriorPulmonaryCapillariesToGround));
      m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightSuperiorLobeAnteriorPulmonaryCapillariesToGround));
      m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightMiddleLobeLateralPulmonaryCapillariesToGround));
      m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightMiddleLobeMedialPulmonaryCapillariesToGround));
      m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeSuperiorPulmonaryCapillariesToGround));
      m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeMedialBasalPulmonaryCapillariesToGround));
      m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeAnteriorBasalPulmonaryCapillariesToGround));
      m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeLateralBasalPulmonaryCapillariesToGround));
      m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightInferiorLobePosteriorBasalPulmonaryCapillariesToGround));

      m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobePosteriorBasalPulmonaryCapillariesToGround));
      m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobeLateralBasalPulmonaryCapillariesToGround));
      m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobeAnteromedialBasalPulmonaryCapillariesToGround));
      m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobeSuperiorPulmonaryCapillariesToGround));
      m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeInferiorLingulaPulmonaryCapillariesToGround));
      m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeSuperiorLingulaPulmonaryCapillariesToGround));
      m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeAnteriorPulmonaryCapillariesToGround));
      m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeApicoposteriorPulmonaryCapillariesToGround));
    }
    else
    {
      m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftPulmonaryCapillaries1ToGround));
      m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::RightPulmonaryCapillaries1ToGround));
    }

    m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftPulmonaryArteries1ToGround));
    m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::RightPulmonaryArteries1ToGround));
    
    m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftPulmonaryVeins1ToGround));
    m_RespiratoryCompliancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::RightPulmonaryVeins1ToGround));

    if (m_data.GetConfiguration().UseExpandedLungs() == eSwitch::On)
    {
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightSuperiorLobeApicalShunt));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightSuperiorLobePosteriorShunt));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightSuperiorLobeAnteriorShunt));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightMiddleLobeLateralShunt));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightMiddleLobeMedialShunt));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeSuperiorShunt));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeMedialBasalShunt));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeAnteriorBasalShunt));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeLateralBasalShunt));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightInferiorLobePosteriorBasalShunt));

      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobePosteriorBasalShunt));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobeLateralBasalShunt));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobeAnteromedialBasalShunt));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobeSuperiorShunt));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeInferiorLingulaShunt));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeSuperiorLingulaShunt));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeAnteriorShunt));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeApicoposteriorShunt));

      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightSuperiorLobeApicalPulmonaryCapillaries));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightSuperiorLobePosteriorPulmonaryCapillaries));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightSuperiorLobeAnteriorPulmonaryCapillaries));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightMiddleLobeLateralPulmonaryCapillaries));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightMiddleLobeMedialPulmonaryCapillaries));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightInferiorLobeSuperiorPulmonaryCapillaries));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightInferiorLobeMedialBasalPulmonaryCapillaries));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightInferiorLobeAnteriorBasalPulmonaryCapillaries));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightInferiorLobeLateralBasalPulmonaryCapillaries));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightPulmonaryArteries1ToRightInferiorLobePosteriorBasalPulmonaryCapillaries));

      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftInferiorLobePosteriorBasalPulmonaryCapillaries));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftInferiorLobeLateralBasalPulmonaryCapillaries));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftInferiorLobeSuperiorPulmonaryCapillaries));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftSuperiorLobeAnteriorPulmonaryCapillaries));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftPulmonaryArteries1ToLeftSuperiorLobeApicoposteriorPulmonaryCapillaries));

      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightSuperiorLobeApicalPulmonaryCapillariesToRightPulmonaryVeins1));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightSuperiorLobePosteriorPulmonaryCapillariesToRightPulmonaryVeins1));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightSuperiorLobeAnteriorPulmonaryCapillariesToRightPulmonaryVeins1));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightMiddleLobeLateralPulmonaryCapillariesToRightPulmonaryVeins1));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightMiddleLobeMedialPulmonaryCapillariesToRightPulmonaryVeins1));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeSuperiorPulmonaryCapillariesToRightPulmonaryVeins1));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeMedialBasalPulmonaryCapillariesToRightPulmonaryVeins1));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeAnteriorBasalPulmonaryCapillariesToRightPulmonaryVeins1));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightInferiorLobeLateralBasalPulmonaryCapillariesToRightPulmonaryVeins1));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::RightInferiorLobePosteriorBasalPulmonaryCapillariesToRightPulmonaryVeins1));

      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobePosteriorBasalPulmonaryCapillariesToLeftPulmonaryVeins1));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobeLateralBasalPulmonaryCapillariesToLeftPulmonaryVeins1));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobeAnteromedialBasalPulmonaryCapillariesToLeftPulmonaryVeins1));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftInferiorLobeSuperiorPulmonaryCapillariesToLeftPulmonaryVeins1));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeInferiorLingulaPulmonaryCapillariesToLeftPulmonaryVeins1));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeSuperiorLingulaPulmonaryCapillariesToLeftPulmonaryVeins1));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeAnteriorPulmonaryCapillariesToLeftPulmonaryVeins1));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::ExpandedLungsCardiovascularPath::LeftSuperiorLobeApicoposteriorPulmonaryCapillariesToLeftPulmonaryVeins1));
    }
    else
    {
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftPulmonaryArteries1ToLeftPulmonaryVeins1));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftPulmonaryArteries1ToLeftPulmonaryCapillaries1));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftPulmonaryCapillaries1ToLeftPulmonaryVeins1));

      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::RightPulmonaryArteries1ToRightPulmonaryVeins1));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::RightPulmonaryArteries1ToRightPulmonaryCapillaries1));
      m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::RightPulmonaryCapillaries1ToRightPulmonaryVeins1));
    }

    m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftIntermediatePulmonaryArteries1ToLeftPulmonaryArteries1));
    m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::LeftPulmonaryVeins1ToLeftIntermediatePulmonaryVeins1));

    m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::RightIntermediatePulmonaryArteries1ToRightPulmonaryArteries1));
    m_PulmonaryResistancePaths.push_back(m_CirculatoryCircuit->GetPath(pulse::CardiovascularPath::RightPulmonaryVeins1ToRightIntermediatePulmonaryVeins1));

    // Add the expanded kidney paths
    if (m_data.GetConfiguration().UseExpandedKidneys() == eSwitch::On)
    {
      SEFluidCircuitPath* p;
      // Add kidney resistance paths
      p = m_CirculatoryCircuit->GetPath(pulse::RenalPath::RightRenalArteryToAfferentArteriole);
      if (!Contains(m_SystemicResistancePaths, (*p)))
        m_SystemicResistancePaths.push_back(p);
      p = m_CirculatoryCircuit->GetPath(pulse::RenalPath::RightAfferentArterioleToGlomerularCapillaries);
      if (!Contains(m_SystemicResistancePaths, (*p)))
        m_SystemicResistancePaths.push_back(p);
      p = m_CirculatoryCircuit->GetPath(pulse::RenalPath::RightGlomerularCapillariesToEfferentArteriole);
      if (!Contains(m_SystemicResistancePaths, (*p)))
        m_SystemicResistancePaths.push_back(p);
      p = m_CirculatoryCircuit->GetPath(pulse::RenalPath::RightEfferentArterioleToPeritubularCapillaries);
      if (!Contains(m_SystemicResistancePaths, (*p)))
        m_SystemicResistancePaths.push_back(p);
      p = m_CirculatoryCircuit->GetPath(pulse::RenalPath::RightPeritubularCapillariesToRenalVein);
      if (!Contains(m_SystemicResistancePaths, (*p)))
        m_SystemicResistancePaths.push_back(p);
      //p = m_CirculatoryCircuit->GetPath(pulse::RenalPath::RightRenalVeinToVenaCavaConnection);
      //if (!Contains(m_SystemicResistancePaths, (*p)))
      //  m_SystemicResistancePaths.push_back(p);
    
      p = m_CirculatoryCircuit->GetPath(pulse::RenalPath::LeftRenalArteryToAfferentArteriole);
      if (!Contains(m_SystemicResistancePaths, (*p)))
        m_SystemicResistancePaths.push_back(p);
      p = m_CirculatoryCircuit->GetPath(pulse::RenalPath::LeftAfferentArterioleToGlomerularCapillaries);
      if (!Contains(m_SystemicResistancePaths, (*p)))
        m_SystemicResistancePaths.push_back(p);
      p = m_CirculatoryCircuit->GetPath(pulse::RenalPath::LeftGlomerularCapillariesToEfferentArteriole);
      if (!Contains(m_SystemicResistancePaths, (*p)))
        m_SystemicResistancePaths.push_back(p);
      p = m_CirculatoryCircuit->GetPath(pulse::RenalPath::LeftEfferentArterioleToPeritubularCapillaries);
      if (!Contains(m_SystemicResistancePaths, (*p)))
        m_SystemicResistancePaths.push_back(p);
      p = m_CirculatoryCircuit->GetPath(pulse::RenalPath::LeftPeritubularCapillariesToRenalVein);
      if (!Contains(m_SystemicResistancePaths, (*p)))
        m_SystemicResistancePaths.push_back(p);
      //p = m_CirculatoryCircuit->GetPath(pulse::RenalPath::LeftRenalVeinToVenaCavaConnection);
      //if (!Contains(m_SystemicResistancePaths, (*p)))
      //  m_SystemicResistancePaths.push_back(p);
    
      // Add Compliance Paths
      p = m_CirculatoryCircuit->GetPath(pulse::RenalPath::RightRenalArteryCompliance);
      if (!Contains(m_SystemicCompliancePaths, (*p)))
        m_SystemicCompliancePaths.push_back(p);
      p = m_CirculatoryCircuit->GetPath(pulse::RenalPath::RightGlomerularCapillariesCompliance);
      if (!Contains(m_SystemicCompliancePaths, (*p)))
        m_SystemicCompliancePaths.push_back(p);
      p = m_CirculatoryCircuit->GetPath(pulse::RenalPath::RightRenalVeinCompliance);
      if (!Contains(m_SystemicCompliancePaths, (*p)))
        m_SystemicCompliancePaths.push_back(p);
    
      p = m_CirculatoryCircuit->GetPath(pulse::RenalPath::LeftRenalArteryCompliance);
      if (!Contains(m_SystemicCompliancePaths, (*p)))
        m_SystemicCompliancePaths.push_back(p);
      p = m_CirculatoryCircuit->GetPath(pulse::RenalPath::LeftGlomerularCapillariesCompliance);
      if (!Contains(m_SystemicCompliancePaths, (*p)))
        m_SystemicCompliancePaths.push_back(p);
      p = m_CirculatoryCircuit->GetPath(pulse::RenalPath::LeftRenalVeinCompliance);
      if (!Contains(m_SystemicCompliancePaths, (*p)))
        m_SystemicCompliancePaths.push_back(p);
    }
    // Track average flows by cardiac cycle the following compartments and their children
    SampleCompartment(m_data.GetCompartments(), *m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Aorta));
    SampleCompartment(m_data.GetCompartments(), *m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Brain));
    SampleCompartment(m_data.GetCompartments(), *m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Muscle));
    SampleCompartment(m_data.GetCompartments(), *m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LargeIntestine));
    SampleCompartment(m_data.GetCompartments(), *m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftArm));
    SampleCompartment(m_data.GetCompartments(), *m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftKidney));
    SampleCompartment(m_data.GetCompartments(), *m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftLeg));
    SampleCompartment(m_data.GetCompartments(), *m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Liver));
    SampleCompartment(m_data.GetCompartments(), *m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightArm));
    SampleCompartment(m_data.GetCompartments(), *m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightKidney));
    SampleCompartment(m_data.GetCompartments(), *m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightLeg));
    SampleCompartment(m_data.GetCompartments(), *m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Skin));
    SampleCompartment(m_data.GetCompartments(), *m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::SmallIntestine));
    SampleCompartment(m_data.GetCompartments(), *m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Splanchnic));
    SampleCompartment(m_data.GetCompartments(), *m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Spleen));
    SampleCompartment(m_data.GetCompartments(), *m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::VenaCava));
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Cardiovascular system at steady state
  ///
  /// \details
  /// Initializes Cardiovascular conditions if any are present.
  ///  <UL>
  ///   <LI>Anemia</LI>
  ///   <LI>Renal Stenosis</LI>
  ///   <LI>Heart Failure</LI>
  ///   <LI>Pericardial Effusion</LI>
  ///  </UL>
  ///
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::AtSteadyState()
  {
    std::string typeString;
    if (m_data.GetState() == EngineState::Active)
      typeString = "Starting ";
    else
    {
      if (!(m_data.GetState() > EngineState::AtInitialStableState && m_data.GetConditions().HasDehydration()))
      {
        m_data.GetCurrentPatient().GetHeartRateBaseline().Set(GetHeartRate());
        m_data.GetCurrentPatient().GetDiastolicArterialPressureBaseline().Set(GetDiastolicArterialPressure());
        m_data.GetCurrentPatient().GetSystolicArterialPressureBaseline().Set(GetSystolicArterialPressure());
        m_data.GetCurrentPatient().GetMeanArterialPressureBaseline().Set(GetMeanArterialPressure());
        // Keep these for moving between arrhythmia's, note InitialPatient is pre conditions
        m_StabilizedHeartRateBaseline_Per_min = m_data.GetCurrentPatient().GetHeartRateBaseline(FrequencyUnit::Per_min);
        m_HeartRateBaseline_Per_min->Set(m_StabilizedHeartRateBaseline_Per_min);
        m_StabilizedMeanArterialPressureBaseline_mmHg = m_data.GetCurrentPatient().GetMeanArterialPressureBaseline(PressureUnit::mmHg);
      }

      if (m_data.GetState() == EngineState::AtInitialStableState)
      {// At Resting State, apply conditions if we have them
        typeString = "Initial Stabilization Homeostasis: ";
        if (m_data.GetConditions().HasChronicAnemia())
          ChronicAnemia();
        if (m_data.GetConditions().HasChronicRenalStenosis())
          ChronicRenalStenosis();
        if (m_data.GetConditions().HasChronicVentricularSystolicDysfunction())
          ChronicHeartFailure();
        if (m_data.GetConditions().HasChronicPericardialEffusion())
          ChronicPericardialEffusion();
      }
      else if (m_data.GetState() == EngineState::AtSecondaryStableState)
        typeString = "Secondary Stabilization Homeostasis: ";
    }

    m_ss << typeString << "Patient heart rate = " << GetHeartRate();
    Info(m_ss);
    m_ss << typeString << "Patient diastolic arterial pressure = " << GetDiastolicArterialPressure();
    Info(m_ss);
    m_ss << typeString << "Patient systolic arterial pressure = " << GetSystolicArterialPressure();
    Info(m_ss);
    m_ss << typeString << "Patient mean arterial pressure = " << GetMeanArterialPressure();
    Info(m_ss);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Establishes the anemia condition by removing hemoglobin and reducing blood viscosity (simulated by vascular resistance reduction).
  ///
  /// \details
  /// We are modeling iron deficiency anemia as a chronic reduction in hemoglobin in the blood as well as
  /// a reduction in the cardiovascular resistances to lower the systemic vascular resistance that is
  /// seen with the reduced viscosity. The oxygen carrying capacity of the blood is reduced due to the
  /// decrease in hemoglobin content.
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::ChronicAnemia()
  {
    SEChronicAnemia& anemia = m_data.GetConditions().GetChronicAnemia();
    double rf = anemia.GetReductionFactor().GetValue();

    // Maximum 30% reduction
    if (rf > 0.3)
    {
      Error("Cannot remove more than 30% of hemoglobin in anemia in the Pulse Engine. Setting value to 30% and continuing.");
      rf = 0.3;
    }
    // Empirical resistance modification
    double viscousModifier = 1.0 - (0.15 * rf);

    std::vector<SESubstance*> hemoglobinSubs;
    hemoglobinSubs.push_back(&m_data.GetSubstances().GetHb());
    hemoglobinSubs.push_back(&m_data.GetSubstances().GetHbO2());
    hemoglobinSubs.push_back(&m_data.GetSubstances().GetHbCO2());
    hemoglobinSubs.push_back(&m_data.GetSubstances().GetHbO2CO2());

    double newMass_g;
    SELiquidSubstanceQuantity* subQ;
    // Reduce all hemoglobin mass in all compartments
    for (SELiquidCompartment* cmpt : m_data.GetCompartments().GetVascularLeafCompartments())
    {
      if (!cmpt->HasVolume())
        continue;

      for (SESubstance* subst : hemoglobinSubs)
      {
        subQ = cmpt->GetSubstanceQuantity(*subst);
        newMass_g = subQ->GetMass(MassUnit::g) * (1 - rf);
        subQ->GetMass().SetValue(newMass_g, MassUnit::g);
        subQ->Balance(BalanceLiquidBy::Mass);
      }
    }

    // Only the cardiovascular paths are adjusted. This is obviously an inconsistency, but 
    // other vascular paths may contain non-blood fluids for which viscosity is unaffected by
    // anemia. This gets us close enough.
    for (SEFluidCircuitPath* path : m_data.GetCircuits().GetCardiovascularCircuit().GetPaths())
    {
      if (path->HasResistanceBaseline())
      {
        path->GetResistanceBaseline().SetValue(path->GetResistanceBaseline(PressureTimePerVolumeUnit::mmHg_s_Per_mL) * viscousModifier, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
      }
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Establishes the chronic heart failure condition.
  ///
  /// \details
  /// Heart failure is a chronic condition that is modeled by a permanent
  /// reduction in the left heart contractility. The user may scale the severity of this action
  /// with a fraction from 0 to 1, with 0 being non-existent to 1 being severe heart failure.
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::ChronicHeartFailure()
  {
    //Decrease left heart contractility
    m_LeftHeartElastanceModifier *= 0.27;
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Establishes the pericardial effusion condition.
  ///
  /// \details
  /// Pericardial effusion can be either chronic (slow) or acute (fast).
  /// Chronic effusion will eventually lead to tamponade, acute effusion leads
  /// immediately to tamponade and imminent death. The chronic effusion parameters 
  /// are set in the Pulse engine so that life-threatening tamponade will occur 
  ///  in about 30 minutes after the insult.
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::ChronicPericardialEffusion()
  {
    double deltaVolume_mL = m_data.GetConditions().GetChronicPericardialEffusion().GetAccumulatedVolume().GetValue(VolumeUnit::mL);
    if (deltaVolume_mL > 1000.0)
    {
      Error("Cannot specify volume accumulation greater than 1000 mL. Accumulated volume is now set at 1000 mL.");
      deltaVolume_mL = 1000.0;
    }
    else if (deltaVolume_mL < 0.0)
    {
      Error("Cannot specify volume accumulation less than 0 mL. Accumulated volume is now set at 0 mL.");
      deltaVolume_mL = 0.0;
    }

    //Just throw this all on at once
    //Only do this for a single time-step!
    m_GndToPericardium->GetNextFlowSource().SetValue(deltaVolume_mL / m_data.GetTimeStep_s(), VolumePerTimeUnit::mL_Per_s);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Establishes the renal stenosis condition in one or both of the renal arteries.
  ///
  /// \details
  /// Stenosed renal arteries are slightly occluded, which increases the resistance from the aorta to the kidney.
  /// This is implemented as a condition, which specifies what percent of the artery is occluded and which artery
  /// or arteries are being affected. 
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::ChronicRenalStenosis()
  {
    //todo: move this to CV
    double LeftOcclusionFraction = m_data.GetConditions().GetChronicRenalStenosis().GetLeftKidneySeverity().GetValue();
    double RightOcclusionFraction = m_data.GetConditions().GetChronicRenalStenosis().GetRightKidneySeverity().GetValue();

    if (LeftOcclusionFraction < 0.0)
    {
      Error("Cannot specify left occlusion fraction less than zero. Renal resistances remain unchanged."); //Specify resistance is the same in error
      return;
    }

    if (RightOcclusionFraction < 0.0)
    {
      Error("Cannot specify right occlusion fraction less than zero. Renal resistances remain unchanged.");
      return;
    }

    if (LeftOcclusionFraction > 1.0)
    {
      Error("Cannot specify left occlusion fraction greater than one. Renal resistances remain unchanged.");
      return;
    }

    if (RightOcclusionFraction > 1.0)
    {
      Error("Cannot specify right occlusion fraction greater than  one. Renal resistances remain unchanged.");
      return;
    }

    //Aorta1ToAfferentArteriole paths are equivalent to the renal artery in Pulse. Resistance increases on these paths to represent renal arterial stenosis
    double currentLeftResistance_mmHg_s_Per_mL = m_LeftRenalArteryPath->GetResistanceBaseline(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    double currentRightResistance_mmHg_s_Per_mL = m_RightRenalArteryPath->GetResistanceBaseline(PressureTimePerVolumeUnit::mmHg_s_Per_mL);

    //The base resistance is a tuned parameter that allows for adequate flow reduction to the kidneys with the logarithmic functional form chosen
    double baseResistance_mmHg_s_Per_mL = 10.0;
    //Open resistance indicates a completely occluded artery. This value is 100 mmHg/mL/s for the cardiovascular circuit.
    double openResistance_mmHg_s_Per_mL = m_data.GetConfiguration().GetCardiovascularOpenResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL);

    double newLeftResistance_mmHg_s_Per_mL = GeneralMath::ExponentialDecayFunction(baseResistance_mmHg_s_Per_mL, openResistance_mmHg_s_Per_mL, currentLeftResistance_mmHg_s_Per_mL, LeftOcclusionFraction);
    double newRightResistance_mmHg_s_Per_mL = GeneralMath::ExponentialDecayFunction(baseResistance_mmHg_s_Per_mL, openResistance_mmHg_s_Per_mL, currentRightResistance_mmHg_s_Per_mL, RightOcclusionFraction);

    m_LeftRenalArteryPath->GetResistanceBaseline().SetValue(newLeftResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    m_RightRenalArteryPath->GetResistanceBaseline().SetValue(newRightResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Preprocess prepares the cardiovascular system for the circuit solver
  ///
  /// \details
  /// This function calculates the appropriate modifications to the cardiovascular
  /// circuit for the time within the cardiac cycle and it processes the actions on the
  /// cardiovascular system.
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::PreProcess()
  {
    //m_data.GetDataTrack().Probe(*m_CirculatoryCircuit);
    ProcessActions();

    HeartDriver();
    UpdatePulmonaryCapillaries();
    CalculateHemothorax();
    CalculatePleuralCavityVenousEffects();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Process solves the cardiovascular circuit
  ///
  /// \details
  /// Modifications to the cardiovascular system are made during the preprocess
  /// step of the cardiovascular and other systems. The new state of the circuit 
  /// is solved using %Pulse @ref CircuitMethodology. Advective substance transport
  /// is computed using the %Pulse @ref SubstanceTransportMethodology.
  /// Finally, vitals sign data is computed and system data is populated in the 
  /// CalculateVitalSigns method.
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::Process(bool solve_and_transport)
  {
    if (solve_and_transport)
    {
      m_CircuitCalculator->Process(*m_CirculatoryCircuit, m_data.GetTimeStep_s());
      m_Transporter->Transport(*m_CirculatoryGraph, m_data.GetTimeStep_s());
    }
    CalculateVitalSigns();
    ComputeExposedModelParameters();
  }
  void CardiovascularModel::ComputeExposedModelParameters()
  {

  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Update the cardiovascular circuit
  ///
  /// \details
  /// The current time-step's circuit solution is set to the next time-step when it is passed to PostProcess.
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::PostProcess(bool solve_and_transport)
  {
    if (solve_and_transport)
      m_CircuitCalculator->PostProcess(*m_CirculatoryCircuit);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// CalculateVitalSigns computes and/or updates cardiovascular system level data
  ///
  /// \details
  /// Calculate vital signs obtains the pressures in the aorta, pulmonary arteries, pulmonary veins and vena cava. 
  /// Waveform data for the system, such as arterial pressure, is set every at every time slice. Mean data, such
  /// as mean arterial pressure, is set using a running average. Data that are more useful filtered are also set
  /// from a running mean. 
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::CalculateVitalSigns()
  {
    // Tell the cmpt mgr to sample any cmpts by cardiac cycle
    m_data.GetCompartments().SampleByCardiacCyle(m_data.GetEvents().IsEventActive(eEvent::StartOfCardiacCycle));
    // Grab data from the circuit in order to calculate a running mean
    const double AortaNodePressure_mmHg = m_Aorta->GetPressure(PressureUnit::mmHg);
    const double AortaNodeCO2PartialPressure_mmHg = m_AortaCO2 == nullptr ? 0 : m_AortaCO2->GetPartialPressure(PressureUnit::mmHg); // This is here so we can Tune circuit w/o substances
    const double LeftPulmonaryArteryVolume_mL = m_LeftPulmonaryArteries->GetVolume(VolumeUnit::mL);
    const double RightPulmonaryArteryVolume_mL = m_RightPulmonaryArteries->GetVolume(VolumeUnit::mL);
    const double TotalPulmonaryArteryVolume_mL = LeftPulmonaryArteryVolume_mL + RightPulmonaryArteryVolume_mL;
    const double LeftPulmonaryArteryPressure_mmHg = m_LeftPulmonaryArteries->GetPressure(PressureUnit::mmHg);
    const double RightPulmonaryArteryPressure_mmHg = m_RightPulmonaryArteries->GetPressure(PressureUnit::mmHg);

    const double LeftPulmonaryVeinVolume_mL = m_LeftPulmonaryVeins->GetVolume(VolumeUnit::mL);
    const double RightPulmonaryVeinVolume_mL = m_RightPulmonaryVeins->GetVolume(VolumeUnit::mL);
    const double TotalPulmonaryVeinVolume_mL = LeftPulmonaryVeinVolume_mL + RightPulmonaryVeinVolume_mL;
    const double LeftPulmonaryVeinPressure_mmHg = m_LeftPulmonaryVeins->GetPressure(PressureUnit::mmHg);
    const double RightPulmonaryVeinPressure_mmHg = m_RightPulmonaryVeins->GetPressure(PressureUnit::mmHg);

    const double PulmonaryArteryNodePressure_mmHg = (LeftPulmonaryArteryVolume_mL * LeftPulmonaryArteryPressure_mmHg + RightPulmonaryArteryVolume_mL * RightPulmonaryArteryPressure_mmHg) / TotalPulmonaryArteryVolume_mL;
    const double PulmVeinNodePressure_mmHg = (LeftPulmonaryVeinVolume_mL * LeftPulmonaryVeinPressure_mmHg + RightPulmonaryVeinVolume_mL * RightPulmonaryVeinPressure_mmHg) / TotalPulmonaryVeinVolume_mL;

    double PulmCapFlow_mL_Per_s = 0.0;
    for (unsigned int iter = 0; iter < m_LeftPulmonaryArteriesToCapillaries.size(); iter++)
    {
      double pulmonaryCapillaryFlow_mL_Per_s = m_LeftPulmonaryArteriesToCapillaries.at(iter)->GetNextFlow(VolumePerTimeUnit::mL_Per_s);
      PulmCapFlow_mL_Per_s += pulmonaryCapillaryFlow_mL_Per_s;
      double perfusionVolume_mL = pulmonaryCapillaryFlow_mL_Per_s * m_data.GetTimeStep_s();
      m_LeftCardiacCyclePerfusionVolumes_mL.at(iter) += perfusionVolume_mL;
    }
    for (unsigned int iter = 0; iter < m_RightPulmonaryArteriesToCapillaries.size(); iter++)
    {
      double pulmonaryCapillaryFlow_mL_Per_s = m_RightPulmonaryArteriesToCapillaries.at(iter)->GetNextFlow(VolumePerTimeUnit::mL_Per_s);
      PulmCapFlow_mL_Per_s += pulmonaryCapillaryFlow_mL_Per_s;
      double perfusionVolume_mL = pulmonaryCapillaryFlow_mL_Per_s * m_data.GetTimeStep_s();
      m_RightCardiacCyclePerfusionVolumes_mL.at(iter) += perfusionVolume_mL;
    }

    double PulmShuntFlow_mL_Per_s = 0.0;
    for (auto leftShuntPath : m_LeftPulmonaryArteriesToVeins)
    {
      PulmShuntFlow_mL_Per_s += leftShuntPath->GetNextFlow(VolumePerTimeUnit::mL_Per_s);
    }
    for (auto rightShuntPath : m_RightPulmonaryArteriesToVeins)
    {
      PulmShuntFlow_mL_Per_s += rightShuntPath->GetNextFlow(VolumePerTimeUnit::mL_Per_s);
    }

    const double VenaCavaPressure_mmHg = m_VenaCava->GetPressure(PressureUnit::mmHg);
    const double LHeartFlow_mL_Per_s = m_LeftHeartToAorta->GetNextFlow(VolumePerTimeUnit::mL_Per_s);
    const double LHeartVolume_mL = m_LeftHeart->GetVolume(VolumeUnit::mL);
    const double LHeartPressure_mmHg = m_LeftHeart->GetPressure(PressureUnit::mmHg);
    const double RHeartPressure_mmHg = m_RightHeart->GetPressure(PressureUnit::mmHg);

    double SkinFlow_mL_Per_s = 0;
    for(SEFluidCircuitPath* path : m_SkinPaths)
      SkinFlow_mL_Per_s += path->GetNextFlow(VolumePerTimeUnit::mL_Per_s);

    m_FullyCompressedHeart = false;
    if (m_data.GetEvents().IsEventActive(eEvent::CardiacArrest))
    {
      m_CardiacArrestVitalsUpdateTimer_s += m_data.GetTimeStep_s();
      if (m_CardiacArrestVitalsUpdateTimer_s > 1)
      {
        RecordAndResetCardiacCycle();
        m_CardiacArrestVitalsUpdateTimer_s = 0;
      }
    }
    else
    {
      // Test to see if the cardiac cycle is starting, compressing blood out of the heart
      if (LHeartFlow_mL_Per_s > 0.1 && !m_HeartFlowDetected)
      {
        // Threshold of 0.1 is empirically determined. Approximate zero makes it too noisy.
        m_HeartFlowDetected = true;
      }
      // Check to see if the cardiac cycle has completed, fully compressed the blood from the heart
      if (LHeartFlow_mL_Per_s < 0.1 && m_HeartFlowDetected)
      {
        m_HeartFlowDetected = false;
        m_FullyCompressedHeart = true;
      }
    }

    // Record high and low values to compute for systolic and diastolic pressures:
    if (AortaNodePressure_mmHg > m_CardiacCycleAortaPressureHigh_mmHg)
      m_CardiacCycleAortaPressureHigh_mmHg = AortaNodePressure_mmHg;
    if (AortaNodePressure_mmHg < m_CardiacCycleAortaPressureLow_mmHg)
      m_CardiacCycleAortaPressureLow_mmHg = AortaNodePressure_mmHg;
    if (LHeartPressure_mmHg > m_CardiacCycleLeftHeartPressureHigh_mmHg)
      m_CardiacCycleLeftHeartPressureHigh_mmHg = LHeartPressure_mmHg;
    if (LHeartPressure_mmHg < m_CardiacCycleLeftHeartPressureLow_mmHg)
      m_CardiacCycleLeftHeartPressureLow_mmHg = LHeartPressure_mmHg;
    if (PulmonaryArteryNodePressure_mmHg > m_CardiacCyclePulmonaryArteryPressureHigh_mmHg)
      m_CardiacCyclePulmonaryArteryPressureHigh_mmHg = PulmonaryArteryNodePressure_mmHg;
    if (PulmonaryArteryNodePressure_mmHg < m_CardiacCyclePulmonaryArteryPressureLow_mmHg)
      m_CardiacCyclePulmonaryArteryPressureLow_mmHg = PulmonaryArteryNodePressure_mmHg;
    if (RHeartPressure_mmHg > m_CardiacCycleRightHeartPressureHigh_mmHg)
      m_CardiacCycleRightHeartPressureHigh_mmHg = RHeartPressure_mmHg;
    if (RHeartPressure_mmHg < m_CardiacCycleRightHeartPressureLow_mmHg)
      m_CardiacCycleRightHeartPressureLow_mmHg = RHeartPressure_mmHg;

    // Record high and low values to compute for peripheral perfusion index:
    double totalPeripheralVolume_mL = m_LeftArm->GetVolume(VolumeUnit::mL);
    totalPeripheralVolume_mL += m_LeftLeg->GetVolume(VolumeUnit::mL);
    totalPeripheralVolume_mL += m_RightArm->GetVolume(VolumeUnit::mL);
    totalPeripheralVolume_mL += m_RightLeg->GetVolume(VolumeUnit::mL);
    m_PeripheralVolumeHigh_mL = MAX(m_PeripheralVolumeHigh_mL, totalPeripheralVolume_mL);
    m_PeripheralVolumeLow_mL = MIN(m_PeripheralVolumeLow_mL, totalPeripheralVolume_mL);

    // Get Max of Left Ventricle Volume over the course of a heart beat for end diastolic volume
    if (LHeartVolume_mL > m_CardiacCycleDiastolicVolume_mL)
      m_CardiacCycleDiastolicVolume_mL = LHeartVolume_mL;

    // Increment stroke volume and pulmonary flows
    m_CardiacCycleStrokeVolume_mL += LHeartFlow_mL_Per_s * m_data.GetTimeStep_s();

    //Get samples for running means
    m_CardiacCycleArterialPressure_mmHg->Sample(AortaNodePressure_mmHg);
    m_CardiacCycleArterialCO2PartialPressure_mmHg->Sample(AortaNodeCO2PartialPressure_mmHg);
    m_CardiacCyclePulmonaryCapillariesWedgePressure_mmHg->Sample(PulmVeinNodePressure_mmHg);
    m_CardiacCyclePulmonaryCapillariesFlow_mL_Per_s->Sample(PulmCapFlow_mL_Per_s);
    m_CardiacCyclePulmonaryShuntFlow_mL_Per_s->Sample(PulmShuntFlow_mL_Per_s);
    m_CardiacCyclePulmonaryArteryPressure_mmHg->Sample(PulmonaryArteryNodePressure_mmHg);
    m_CardiacCycleCentralVenousPressure_mmHg->Sample(VenaCavaPressure_mmHg);
    m_CardiacCycleSkinFlow_mL_Per_s->Sample(SkinFlow_mL_Per_s);

    if (m_CardiacCycleAortaPressureLow_mmHg < -2.0)
    {
      Fatal("Diastolic pressure has fallen below zero.");
    }
    if (m_CardiacCycleAortaPressureHigh_mmHg > 700.0)
    {
      Fatal("Systolic pressure has exceeded physiologic range.");
    }

    // Pressures\Flows from circuit
    GetArterialPressure().SetValue(AortaNodePressure_mmHg, PressureUnit::mmHg);
    GetPulmonaryArterialPressure().SetValue(PulmonaryArteryNodePressure_mmHg, PressureUnit::mmHg);
    GetCentralVenousPressure().SetValue(VenaCavaPressure_mmHg, PressureUnit::mmHg);
    GetCerebralBloodFlow().Set(m_Brain->GetInFlow());
    GetIntracranialPressure().Set(m_Brain->GetPressure());
    GetCerebralPerfusionPressure().SetValue(GetMeanArterialPressure(PressureUnit::mmHg) - GetIntracranialPressure(PressureUnit::mmHg), PressureUnit::mmHg);

    if (m_data.GetState() > EngineState::AtSecondaryStableState)
    {// Don't throw events if we are initializing

    // Check for hypovolemic shock
      if (GetBloodVolume().GetValue(VolumeUnit::mL) <= (m_data.GetConfiguration().GetMinimumBloodVolumeFraction() * m_data.GetCurrentPatient().GetBloodVolumeBaseline(VolumeUnit::mL)))
      {
        m_data.GetEvents().SetEvent(eEvent::HypovolemicShock, true, m_data.GetSimulationTime());
      }
      else
      {
        m_data.GetEvents().SetEvent(eEvent::HypovolemicShock, false, m_data.GetSimulationTime());
      }

      if (GetMeanArterialPressure().GetValue(PressureUnit::mmHg) <= m_MAPCollapse_mmHg)
      {
        m_data.GetEvents().SetEvent(eEvent::CardiovascularCollapse, true, m_data.GetSimulationTime());
      }
      else
      {
        m_data.GetEvents().SetEvent(eEvent::CardiovascularCollapse, false, m_data.GetSimulationTime());
      }

      //Check for cardiogenic shock
      if (GetCardiacIndex().GetValue(VolumePerTimeAreaUnit::L_Per_min_m2) < 2.2 &&
        GetSystolicArterialPressure(PressureUnit::mmHg) < 90.0 &&
        GetPulmonaryCapillariesWedgePressure(PressureUnit::mmHg) > 15.0)
      {
        /// \cite dhakam2008review
        m_data.GetEvents().SetEvent(eEvent::CardiogenicShock, true, m_data.GetSimulationTime());
      }
      else
      {
        m_data.GetEvents().SetEvent(eEvent::CardiogenicShock, false, m_data.GetSimulationTime());
      }
    }

    if (m_data.GetEvents().IsEventActive(eEvent::CardiacArrest))
    {
      m_data.GetEvents().SetEvent(eEvent::Tachycardia, false, m_data.GetSimulationTime());
      m_data.GetEvents().SetEvent(eEvent::Bradycardia, false, m_data.GetSimulationTime());
    }
    else if(!m_StartCardiacArrest)
    {
      if (GetHeartRate().GetValue(FrequencyUnit::Per_min) < 60)
      {
        SetHeartRhythm(eHeartRhythm::SinusBradycardia);
        m_data.GetEvents().SetEvent(eEvent::Tachycardia, false, m_data.GetSimulationTime());
        m_data.GetEvents().SetEvent(eEvent::Bradycardia, true, m_data.GetSimulationTime());
      }
      else  if (GetHeartRate().GetValue(FrequencyUnit::Per_min) > 100)
      {
       SetHeartRhythm(eHeartRhythm::SinusTachycardia);
        m_data.GetEvents().SetEvent(eEvent::Tachycardia, true, m_data.GetSimulationTime());
        m_data.GetEvents().SetEvent(eEvent::Bradycardia, false, m_data.GetSimulationTime());
      }
      else
      {
        SetHeartRhythm(eHeartRhythm::NormalSinus);
        m_data.GetEvents().SetEvent(eEvent::Tachycardia, false, m_data.GetSimulationTime());
        m_data.GetEvents().SetEvent(eEvent::Bradycardia, false, m_data.GetSimulationTime());
      }
    }

    // Compute blood volume
    double blood_mL = 0;
    // Info("Evaluating blood volume -----------------------------------");
    for (SELiquidCompartment* cmpt : m_data.GetCompartments().GetVascularLeafCompartments())
    {
      if (cmpt->HasVolume() && cmpt != m_Pericardium) //Don't include pericardium
      {
        // Info(cmpt->GetName() + " Has Volume");
        for (auto n : cmpt->GetNodeMapping().GetNodes())
         if(n->HasVolume()){
          //  Info("  - "+n->GetName() + " : " +std::to_string(cmpt->GetVolume(VolumeUnit::mL)) + " [mL]. Total " + std::to_string(blood_mL + cmpt->GetVolume(VolumeUnit::mL)));
            // m_ss << "Compartment - " << std::setw(30) << cmpt->GetName() <<
            //   " - " << std::setw(30) << n->GetName() <<
            //   " Volume(mL):"          << std::setw(10) << cmpt->GetVolume(VolumeUnit::mL) <<
            //   " Total(mL):"           << std::setw(10) << blood_mL + cmpt->GetVolume(VolumeUnit::mL);
            // Info(m_ss);
            }

        blood_mL += cmpt->GetVolume(VolumeUnit::mL);
      }
      //else
      //  Info(cmpt->GetName() + " No Volume");
    }
    // Info("-----------------------------------");
    GetBloodVolume().SetValue(blood_mL, VolumeUnit::mL);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Sets the systolic and diastolic pressures.
  ///
  /// \details
  /// The systemic arterial and pulmonary arterial systolic and diastolic pressures are set here. 
  /// The variables used to track the maximum and minimum pressures are then reset for the next cardiac cycle.
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::RecordAndResetCardiacCycle()
  {
    GetSystolicArterialPressure().SetValue(m_CardiacCycleAortaPressureHigh_mmHg, PressureUnit::mmHg);
    GetDiastolicArterialPressure().SetValue(m_CardiacCycleAortaPressureLow_mmHg, PressureUnit::mmHg);
    GetSystolicLeftHeartPressure().SetValue(m_CardiacCycleLeftHeartPressureHigh_mmHg, PressureUnit::mmHg);
    GetDiastolicLeftHeartPressure().SetValue(m_CardiacCycleLeftHeartPressureLow_mmHg, PressureUnit::mmHg);
    GetPulmonarySystolicArterialPressure().SetValue(m_CardiacCyclePulmonaryArteryPressureHigh_mmHg, PressureUnit::mmHg);
    GetPulmonaryDiastolicArterialPressure().SetValue(m_CardiacCyclePulmonaryArteryPressureLow_mmHg, PressureUnit::mmHg);
    GetPulsePressure().SetValue(m_CardiacCycleAortaPressureHigh_mmHg - m_CardiacCycleAortaPressureLow_mmHg, PressureUnit::mmHg);
    GetSystolicRightHeartPressure().SetValue(m_CardiacCycleRightHeartPressureHigh_mmHg, PressureUnit::mmHg);
    GetDiastolicRightHeartPressure().SetValue(m_CardiacCycleRightHeartPressureLow_mmHg, PressureUnit::mmHg);

    // As implemented now, during cardiac arrest and no flow through the system,
    // all pressures, everywhere, reach the same value, including the aorta and heart.
    // This results in a CPP of zero, until we fully understand why/how the heart is lower
    // than the aorta/map, we are going to set this to 15 mmHg to indicate the patient is 
    // able to be resusitated. We will update this if we get better information/funding to fully explore it
    double cpp_mmHg = 0.;
    cpp_mmHg = m_CardiacCycleAortaPressureLow_mmHg - m_CardiacCycleLeftHeartPressureLow_mmHg;
    cpp_mmHg = MAX(10, cpp_mmHg);
    GetCoronaryPerfusionPressure().SetValue(cpp_mmHg, PressureUnit::mmHg);

    m_data.GetCardiovascular().GetHeartStrokeVolume().SetValue(m_CardiacCycleStrokeVolume_mL, VolumeUnit::mL);
    double ejectionFraction = 0.;
    if (m_CardiacCycleDiastolicVolume_mL > ZERO_APPROX)
      ejectionFraction = m_CardiacCycleStrokeVolume_mL / m_CardiacCycleDiastolicVolume_mL;
    ejectionFraction = MAX(ejectionFraction, 0);
    ejectionFraction = MIN(1, ejectionFraction);
    GetHeartEjectionFraction().SetValue(ejectionFraction);
    GetCardiacOutput().SetValue(m_CardiacCycleStrokeVolume_mL * GetHeartRate().GetValue(FrequencyUnit::Per_min), VolumePerTimeUnit::mL_Per_min);
    GetCardiacIndex().SetValue(GetCardiacOutput().GetValue(VolumePerTimeUnit::mL_Per_min) / m_data.GetCurrentPatient().GetSkinSurfaceArea(AreaUnit::m2), VolumePerTimeAreaUnit::mL_Per_min_m2);

    double peripheralPerfusionIndex = (m_PeripheralVolumeHigh_mL - m_PeripheralVolumeLow_mL) / m_PeripheralVolumeLow_mL;

    //The normal value of PPI was suggested to range between 0.2% and 20%;
    // however, an observational study showed a median(quartiles) normal value of PPI of 4.3 (2.9–6.2)
    ///\cite savastano2021post

    //In patients with severe sepsis or septic shock, a PPI < 0.3 can predict the need for vasopressor therapy.
    // Furthermore, a PPI < 0.2 can predict patient mortality ///\cite elshal2021plethysmographic

    double max = 0.003;
    std::vector<std::pair<double, double>> interpolatorPoints =
    { {0, 0},
      {0.00038, 0.001},  //Just before hemorrhage decompensation
      {0.00065, 0.003},  //~1L hemorrhage
      {0.00142, 0.043},  //Healthy value
      {max, 0.2} };      //20% max
    peripheralPerfusionIndex = LIMIT(peripheralPerfusionIndex, 0.0, max);
    peripheralPerfusionIndex = GeneralMath::PiecewiseLinearInterpolator(interpolatorPoints, peripheralPerfusionIndex);
    GetPeripheralPerfusionIndex().SetValue(peripheralPerfusionIndex);

    double heartRate_Per_s = GetHeartRate(FrequencyUnit::Per_s);
    double totalPulmonaryPerfusion_mL_Per_s = 0.0;
    size_t leftPulmonaryCapillariesSize = 1;
    size_t rightPulmonaryCapillariesSize = 1;

    if (m_LeftPulmonaryCapillaries->HasChildren())
    {
      leftPulmonaryCapillariesSize = m_LeftPulmonaryCapillaries->GetLeaves().size();
      if (m_LeftCardiacCyclePerfusionVolumes_mL.size() < leftPulmonaryCapillariesSize)
      {
        m_LeftCardiacCyclePerfusionVolumes_mL.resize(leftPulmonaryCapillariesSize, 0.0);
      }
      for (unsigned int iter = 0; iter < leftPulmonaryCapillariesSize; iter++)
      {
        double perfusion_mL_Per_s = m_LeftCardiacCyclePerfusionVolumes_mL.at(iter) * heartRate_Per_s;
        totalPulmonaryPerfusion_mL_Per_s += perfusion_mL_Per_s;
        m_LeftPulmonaryCapillaries->GetLeaves().at(iter)->GetPerfusion().SetValue(perfusion_mL_Per_s, VolumePerTimeUnit::mL_Per_s);
        m_LeftCardiacCyclePerfusionVolumes_mL.at(iter) = 0.0;
      }
    }
    else
    {
      if (m_LeftCardiacCyclePerfusionVolumes_mL.size() < leftPulmonaryCapillariesSize)
      {
        m_LeftCardiacCyclePerfusionVolumes_mL.resize(leftPulmonaryCapillariesSize, 0.0);
      }
      double perfusion_mL_Per_s = m_LeftCardiacCyclePerfusionVolumes_mL.at(0) * heartRate_Per_s;
      totalPulmonaryPerfusion_mL_Per_s += perfusion_mL_Per_s;
      m_LeftPulmonaryCapillaries->GetPerfusion().SetValue(perfusion_mL_Per_s, VolumePerTimeUnit::mL_Per_s);
      m_LeftCardiacCyclePerfusionVolumes_mL.at(0) = 0.0;
    }

    if (m_RightPulmonaryCapillaries->HasChildren())
    {
      rightPulmonaryCapillariesSize = m_RightPulmonaryCapillaries->GetLeaves().size();
      if (m_RightCardiacCyclePerfusionVolumes_mL.size() < rightPulmonaryCapillariesSize)
      {
        m_RightCardiacCyclePerfusionVolumes_mL.resize(rightPulmonaryCapillariesSize, 0.0);
      }
      for (unsigned int iter = 0; iter < rightPulmonaryCapillariesSize; iter++)
      {
        double perfusion_mL_Per_s = m_RightCardiacCyclePerfusionVolumes_mL.at(iter) * heartRate_Per_s;
        totalPulmonaryPerfusion_mL_Per_s += perfusion_mL_Per_s;
        m_RightPulmonaryCapillaries->GetLeaves().at(iter)->GetPerfusion().SetValue(perfusion_mL_Per_s, VolumePerTimeUnit::mL_Per_s);
        m_RightCardiacCyclePerfusionVolumes_mL.at(iter) = 0.0;
      }
    }
    else
    {
      if (m_RightCardiacCyclePerfusionVolumes_mL.size() < rightPulmonaryCapillariesSize)
      {
        m_RightCardiacCyclePerfusionVolumes_mL.resize(rightPulmonaryCapillariesSize, 0.0);
      }
      double perfusion_mL_Per_s = m_RightCardiacCyclePerfusionVolumes_mL.at(0) * heartRate_Per_s;
      totalPulmonaryPerfusion_mL_Per_s += perfusion_mL_Per_s;
      m_RightPulmonaryCapillaries->GetPerfusion().SetValue(perfusion_mL_Per_s, VolumePerTimeUnit::mL_Per_s);
      m_RightCardiacCyclePerfusionVolumes_mL.at(0) = 0.0;
    }

    GetTotalPulmonaryPerfusion().SetValue(totalPulmonaryPerfusion_mL_Per_s, VolumePerTimeUnit::mL_Per_s);
    m_LeftCardiacCyclePerfusionVolumes_mL.resize(leftPulmonaryCapillariesSize, 0.0);
    m_RightCardiacCyclePerfusionVolumes_mL.resize(rightPulmonaryCapillariesSize, 0.0);

    // Running means
    // Mean Arterial Pressure
    GetMeanArterialPressure().SetValue(m_CardiacCycleArterialPressure_mmHg->Value(), PressureUnit::mmHg);
    m_CardiacCycleArterialPressure_mmHg->Invalidate();
    // Mean Aterial CO2 Partial Pressure
    GetMeanArterialCarbonDioxidePartialPressure().SetValue(m_CardiacCycleArterialCO2PartialPressure_mmHg->Value(), PressureUnit::mmHg);
    // Mean Aterial CO2 Partial Pressure Delta
    GetMeanArterialCarbonDioxidePartialPressureDelta().SetValue(m_CardiacCycleArterialCO2PartialPressure_mmHg->Value() - m_LastCardiacCycleMeanArterialCO2PartialPressure_mmHg, PressureUnit::mmHg);
    m_LastCardiacCycleMeanArterialCO2PartialPressure_mmHg = m_CardiacCycleArterialCO2PartialPressure_mmHg->Value();
    m_CardiacCycleArterialCO2PartialPressure_mmHg->Invalidate();
    // Pulmonary Capillary Wedge Pressure
    GetPulmonaryCapillariesWedgePressure().SetValue(m_CardiacCyclePulmonaryCapillariesWedgePressure_mmHg->Value(), PressureUnit::mmHg);
    m_CardiacCyclePulmonaryCapillariesWedgePressure_mmHg->Invalidate();
    // Pulmonary Capillary Mean Flow
    GetPulmonaryMeanCapillaryFlow().SetValue(m_CardiacCyclePulmonaryCapillariesFlow_mL_Per_s->Value(), VolumePerTimeUnit::mL_Per_s);
    m_CardiacCyclePulmonaryCapillariesFlow_mL_Per_s->Invalidate();
    // Pulmonary Shunt Mean Flow
    GetPulmonaryMeanShuntFlow().SetValue(m_CardiacCyclePulmonaryShuntFlow_mL_Per_s->Value(), VolumePerTimeUnit::mL_Per_s);
    m_CardiacCyclePulmonaryShuntFlow_mL_Per_s->Invalidate();
    // Mean Pulmonary Artery Pressure
    GetPulmonaryMeanArterialPressure().SetValue(m_CardiacCyclePulmonaryArteryPressure_mmHg->Value(), PressureUnit::mmHg);
    m_CardiacCyclePulmonaryArteryPressure_mmHg->Invalidate();
    // Mean Central Venous Pressure
    GetMeanCentralVenousPressure().SetValue(m_CardiacCycleCentralVenousPressure_mmHg->Value(), PressureUnit::mmHg);
    m_CardiacCycleCentralVenousPressure_mmHg->Invalidate();
    // Mean Skin Flow
    GetMeanSkinFlow().SetValue(m_CardiacCycleSkinFlow_mL_Per_s->Value(), VolumePerTimeUnit::mL_Per_s);
    m_CardiacCycleSkinFlow_mL_Per_s->Invalidate();

    // Computed systemic Vascular Resistance
    double cardiacOutput_mL_Per_s = GetCardiacOutput().GetValue(VolumePerTimeUnit::mL_Per_s);
    double systemicVascularResistance_mmHg_s_Per_mL = 0.0;
    if (cardiacOutput_mL_Per_s > ZERO_APPROX)
      systemicVascularResistance_mmHg_s_Per_mL = (GetMeanArterialPressure().GetValue(PressureUnit::mmHg) - GetMeanCentralVenousPressure().GetValue(PressureUnit::mmHg)) / cardiacOutput_mL_Per_s;
    GetSystemicVascularResistance().SetValue(systemicVascularResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);

    // Computed pulmonary Vascular Resistances
    if (SEScalar::IsZero(cardiacOutput_mL_Per_s, ZERO_APPROX))
    {
      GetPulmonaryVascularResistance().SetValue(0.0, PressureTimePerVolumeUnit::mmHg_min_Per_mL);
      GetPulmonaryVascularResistanceIndex().SetValue(0.0, PressureTimePerVolumeAreaUnit::mmHg_min_Per_mL_m2);
    }
    else
    {
      //(Mean arteral pressure - mean pulmonary wedge pressure)/Cardiac output
      double PulmonaryPressureDrop_mmHg = GetPulmonaryMeanArterialPressure(PressureUnit::mmHg) - GetPulmonaryCapillariesWedgePressure(PressureUnit::mmHg);
      GetPulmonaryVascularResistance().SetValue(PulmonaryPressureDrop_mmHg / cardiacOutput_mL_Per_s, PressureTimePerVolumeUnit::mmHg_s_Per_mL);

      //Mean arteral pressure - mean pulmonary wedge pressure)/Cardiac index where cardiac index is cardiac output / body surface area
      GetPulmonaryVascularResistanceIndex().SetValue(PulmonaryPressureDrop_mmHg / GetCardiacIndex(VolumePerTimeAreaUnit::mL_Per_s_m2), PressureTimePerVolumeAreaUnit::mmHg_s_Per_mL_m2);
    }

    m_CardiacCycleAortaPressureHigh_mmHg = 0.0;
    m_CardiacCycleAortaPressureLow_mmHg = 10000.0;
    m_CardiacCycleLeftHeartPressureHigh_mmHg = 0.0;
    m_CardiacCycleLeftHeartPressureLow_mmHg = 10000.0;
    m_CardiacCyclePulmonaryArteryPressureHigh_mmHg = 0.0;
    m_CardiacCyclePulmonaryArteryPressureLow_mmHg = 10000.0;
    m_CardiacCycleRightHeartPressureHigh_mmHg = 0.0;
    m_CardiacCycleRightHeartPressureLow_mmHg = 10000.0;
    m_CardiacCycleDiastolicVolume_mL = 0;
    m_CardiacCycleStrokeVolume_mL = 0;

    m_PeripheralVolumeHigh_mL = 0.0;
    m_PeripheralVolumeLow_mL = 10000.0;
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Holds the action methods for the cardiovascular system.
  ///
  /// \details
  /// This method holds the actions for the CV system so that only one
  /// method need be called in preprocess.
  /// <ul>
  ///   <li> Hemorrhage </li>
  ///   <li> Pericardial Effusion </li>
  ///   <li> CPR </li>
  ///   <li> CardiacArrest </li>
  /// </ul>
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::ProcessActions()
  {
    Arrhythmia();
    TraumaticBrainInjury();
    Hemorrhage();
    PericardialEffusion();
    CPR();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// The traumatic brain injury action increases flow resistance in the brain
  ///
  /// \details
  /// The user may specify a brain injury of varying severity to apply during runtime. The brain resistance
  /// is scaled based on severity, which impacts flow and pressure in the brain, simulating the effects of
  /// a non-localized brain injury.
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::TraumaticBrainInjury()
  {
    if (!m_data.GetActions().GetPatientActions().HasBrainInjury())
      return;

    //Grab info about the injury
    SEBrainInjury& b = m_data.GetActions().GetPatientActions().GetBrainInjury();
    double severity = b.GetSeverity().GetValue();

    //Interpolate linearly between multipliers of 1 (for severity of 0) to max (for severity of 1)
    //These multipliers are chosen to result in ICP > 25 mmHg and CBF < 1.8 mL/s
    double usMult = GeneralMath::LinearInterpolator(0, 1, 1, 4.775, severity);
    double dsMult = GeneralMath::LinearInterpolator(0, 1, 1, 30.409, severity);

    m_BrainResistanceDownstreamPath->GetNextResistance().SetValue(dsMult * m_BrainResistanceDownstreamPath->GetResistanceBaseline().GetValue(PressureTimePerVolumeUnit::mmHg_s_Per_mL), PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    m_BrainResistanceUpstreamPath->GetNextResistance().SetValue(usMult * m_BrainResistanceUpstreamPath->GetResistanceBaseline().GetValue(PressureTimePerVolumeUnit::mmHg_s_Per_mL), PressureTimePerVolumeUnit::mmHg_s_Per_mL);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// The hemorrhage function simulates bleeding from a specified compartment
  ///
  /// \details
  /// The user may specify multiple bleeds across the anatomical compartments. The Model creates a 
  /// separate bleeding path for each node in each anatomical compartment by volume-weighting the 
  /// flow. Hemorrhage calls for a compartment that already contains a hemorrhage will be overwritten 
  /// with the new value. Compartments can overlap.
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::Hemorrhage()
  {
    bool completeStateChange = false;
    std::vector<SEHemorrhage*> invalid_hemorrhages;
    const std::vector<SEHemorrhage*>& hems = m_data.GetActions().GetPatientActions().GetHemorrhages();
    //Loop over all hemorrhages to check for validity
    for (auto h : hems)
    {
      if (!h->IsActive())
        continue;

      // Allocate the track
      CardiovascularModel::HemorrhageTrack* trk;
      auto t = m_HemorrhageTrack.find(h);
      if (t != m_HemorrhageTrack.end())
      {
        trk = t->second;
      }
      else
      {
        trk = new CardiovascularModel::HemorrhageTrack();
        m_HemorrhageTrack[h] = trk;
      }

      // Make sure our given values are valid
      if (h->HasSeverity())
      {
        if (h->GetSeverity().GetValue() < 0.0 || h->GetSeverity().GetValue() > 1.0)
        {
          Error("A severity less than 0 or greater than 1.0 cannot be specified.");
          invalid_hemorrhages.push_back(h);
          continue;
        }
      }
      else if (h->HasFlowRate())
      {
        double rate_mL_Per_s = h->GetFlowRate().GetValue(VolumePerTimeUnit::mL_Per_s);
        if (rate_mL_Per_s < 0)
        {
          Error("Cannot specify bleeding less than 0");
          invalid_hemorrhages.push_back(h);
          continue;
        }
      }
      else
      {
        Error("A severity or a rate must be specified.");
      }

      // Get the compartment associated with this hemorrhage
      if (trk->Compartment == nullptr)
      {
        // Allow shorthand naming
        trk->Compartment = m_data.GetCompartments().GetCardiovascularGraph().GetCompartment(h->GetCompartmentName() + "Vasculature");
        //Add Vasculature to the compartment name to grab the cardiovascular compartment
        if (trk->Compartment == nullptr)
        {
          trk->Compartment = m_data.GetCompartments().GetCardiovascularGraph().GetCompartment(h->GetCompartmentName());
        }
        //Unsupported compartment
        if (trk->Compartment == nullptr)
        {
          Error("Removing invalid Hemorrhage due to unsupported compartment : " + h->GetCompartmentName());
          invalid_hemorrhages.push_back(h);
          continue;
        }
        if (h->GetType() == eHemorrhage_Type::Internal)
        {
          if (!m_Abdomen->HasChild(*trk->Compartment))
          {
            Error("Internal Hemorrhage is only supported for the abdominal region, including the right and left kidneys, liver, spleen, splanchnic, and small and large intestine vascular compartments.");
            invalid_hemorrhages.push_back(h);
            continue;
          }
        }
      }

      if (!trk->Compartment->HasVolume())
      {
        Error("A hemorrhaging compartment must have volume : " + h->GetCompartmentName());
        invalid_hemorrhages.push_back(h);
        continue;
      }

      // Create our hemorrhage path/links
      if (trk->CmptHemorrhageLinks.empty())
      {
        if (trk->Compartment->HasChildren())
        {
          for (SELiquidCompartment* leaf : trk->Compartment->GetLeaves())
            trk->CmptHemorrhageLinks[leaf].clear();
        }
        else
          trk->CmptHemorrhageLinks[trk->Compartment].clear();

        std::string hemorrhagePathPrefix = "";
        SEFluidCircuitNode* hemorrhagePathTgt = m_GroundNode;
        SELiquidCompartment* hemorrhageLinkTgt = m_Ground;
        if (h->GetType() == eHemorrhage_Type::Internal)
        {
          hemorrhagePathPrefix = "Internal";
          hemorrhagePathTgt = m_AbdominalCavityNode;
          hemorrhageLinkTgt = m_AbdominalCavity;
        }
        for (auto& itr : trk->CmptHemorrhageLinks)
        {
          auto  cmpt = itr.first;
          auto& links = itr.second;
          for (SEFluidCircuitNode* node : cmpt->GetNodeMapping().GetNodes())
          {
            if (m_CirculatoryCircuit->HasNode(node->GetName()) && node->HasVolume())
            {
              SEFluidCircuitPath& hemorrhagePath = m_CirculatoryCircuit->CreatePath(*node, *hemorrhagePathTgt, node->GetName() + hemorrhagePathPrefix + "Hemorrhage");
              SELiquidCompartmentLink& hemorrhageLink = m_data.GetCompartments().CreateLiquidLink(*cmpt, *hemorrhageLinkTgt, cmpt->GetName() + hemorrhagePathPrefix + "Hemorrhage");
              hemorrhageLink.MapPath(hemorrhagePath);
              m_CirculatoryGraph->AddLink(hemorrhageLink);
              links.push_back(&hemorrhageLink);
              completeStateChange = true;
            }
          }
        }
      }

      //Set all existing hemorrhage paths
      double totalFlow_mL_Per_s = 0;
      for (auto& itr : trk->CmptHemorrhageLinks)
      {
        if (itr.first->HasAverageInFlow())
          totalFlow_mL_Per_s += itr.first->GetAverageInFlow(VolumePerTimeUnit::mL_Per_s);
      }
      if (totalFlow_mL_Per_s == 0)
        continue;// Wait until we have average flow

      for (auto& itr : trk->CmptHemorrhageLinks)
      {
        auto  cmpt = itr.first;

        for (auto link : itr.second)
        {
          SEFluidCircuitPath* path = link->GetPath();
          if (path->HasNextFlowSource())
            path->GetNextFlowSource().SetValue(0.0, VolumePerTimeUnit::mL_Per_s);
          if (path->HasNextResistance())
            path->GetNextResistance().SetValue(0.0, PressureTimePerVolumeUnit::mmHg_s_Per_mL);

          if (h->HasSeverity())
          {
            if (path->HasFlowSource())
            {
              path->GetFlowSource().Invalidate();
              path->GetNextFlowSource().Invalidate();
              completeStateChange = true;
            }

            if (!path->HasResistanceBaseline())
            {
              // Compute the baseline resistance
              double flowRate_L_per_min = cmpt->GetAverageInFlow(VolumePerTimeUnit::L_Per_min);
              //The minimum resistance is associated with the maximum flow rate across the hemorrhage path
              //Check to see if there is a resistance baseline
              double deltaPressure_mmHg = (path->GetSourceNode().GetNextPressure(PressureUnit::mmHg) - path->GetTargetNode().GetNextPressure(PressureUnit::mmHg));
              double resistanceBaseline_mmHg_min_Per_L = deltaPressure_mmHg / (1.25 * flowRate_L_per_min);
              path->GetResistanceBaseline().SetValue(resistanceBaseline_mmHg_min_Per_L, PressureTimePerVolumeUnit::mmHg_min_Per_L);
            }

            if (h->GetSeverity().GetValue() > 0)
            {
              double resistanceBaseline_mmHg_min_Per_L = path->GetResistanceBaseline(PressureTimePerVolumeUnit::mmHg_min_Per_L);
              //use the severity to calculate the resistance
              double resistance_mmHg_min_Per_L = GeneralMath::LinearInterpolator(0.0, 1.0, (2 * resistanceBaseline_mmHg_min_Per_L) / h->GetSeverity().GetValue(), resistanceBaseline_mmHg_min_Per_L, h->GetSeverity().GetValue());
              path->GetNextResistance().SetValue(resistance_mmHg_min_Per_L, PressureTimePerVolumeUnit::mmHg_min_Per_L);
            }
            else
            {
              // Severity is 0.0
              path->GetNextResistance().SetValue(0, PressureTimePerVolumeUnit::mmHg_min_Per_L);
            }
          }
          else if (h->HasFlowRate())
          {
            if (path->HasResistanceBaseline())
            {
              path->GetResistance().Invalidate();
              path->GetNextResistance().Invalidate();
              path->GetResistanceBaseline().Invalidate();
              completeStateChange = true;
              Warning("Switching hemorrhage from severity to flow, severity resistance baseline will be removed");
              Warning("If you go back to severity, a new resistance will be calculated based on the state of the vasculature");
            }
            double mL_Per_s = h->GetFlowRate(VolumePerTimeUnit::mL_Per_s) * (cmpt->GetAverageInFlow(VolumePerTimeUnit::mL_Per_s) / totalFlow_mL_Per_s);
            path->GetNextFlowSource().SetValue(mL_Per_s, VolumePerTimeUnit::mL_Per_s);
          }
        }
      }
    }

    //Update abdominal cavity compliance
    double abdominalBloodVolume = m_AbdominalCavity->GetVolume().GetValue(VolumeUnit::mL);
    double complianceCurveExponent = 0.55;
    //Variable compliance calculation
    double compliance_mL_Per_mmHg = pow(abdominalBloodVolume, complianceCurveExponent);
    m_AbdominalCavityToGnd->GetComplianceBaseline().SetValue(compliance_mL_Per_mmHg, VolumePerPressureUnit::mL_Per_mmHg);
    //Effect the Aorta with internal hemorrhages
    // The internal pressure application function calculates the pressure applied to the aorta due to blood pooling in the abdominal cavity.
    double abdominalCavityPressure_mmHg = m_AbdominalCavity->GetPressure(PressureUnit::mmHg);
    double pressureResponseFraction = 5.0; //Tuning the pressure applied to the aorta
    // The pressure applied to the aorta is dictated by the pressure in the abdominal cavity.
    // The response is tuned to 45% of this value to achieve the correct physiologic response.
    //Set the pressure on the aorta based on the abdominal cavity pressure
    m_InternalHemorrhageToAorta->GetNextPressureSource().SetValue(pressureResponseFraction* abdominalCavityPressure_mmHg, PressureUnit::mmHg);

    // Clean up any hemorrhage tracks that are no longer being used
    double totalLossRate_mL_Per_s = 0.0;
    for (auto htItr : m_HemorrhageTrack)
    {
      SEHemorrhage* h = htItr.first;
      HemorrhageTrack* trk = htItr.second;

      bool removeHemorrhage = false;
      double hemorrhageFlow_mL_Per_s = 0;
      for (auto itr : trk->CmptHemorrhageLinks)
      {
        for (auto link : itr.second)
        {
          SEFluidCircuitPath* p = link->GetPath();
          if ((p->HasNextFlowSource() && p->GetNextFlowSource().IsZero()) ||
              (p->HasNextResistance() && p->GetNextResistance().IsZero()))
          {
            p->GetFlowSource().Invalidate();
            p->GetNextFlowSource().Invalidate();
            p->GetResistance().Invalidate();
            p->GetNextResistance().Invalidate();

            m_CirculatoryCircuit->RemovePath(*p);
            m_CirculatoryGraph->RemoveLink(*link);

            removeHemorrhage = true;
            completeStateChange = true;
          }
          else
          {
            hemorrhageFlow_mL_Per_s += p->HasNextFlow() ? p->GetNextFlow(VolumePerTimeUnit::mL_Per_s) : 0;
          }
        }
      }
      if (removeHemorrhage)
        invalid_hemorrhages.push_back(h);
      else
      {
        //Info(h->GetCompartment() + " Flow " + std::to_string(hemorrhagePath->GetNextFlow(VolumePerTimeUnit::mL_Per_s)));
        //Info(h->GetCompartment() + " Flow Source " + std::to_string(hemorrhagePath->GetNextFlowSource(VolumePerTimeUnit::mL_Per_s)));
        // Keep track of bleeding on the action
       
        if (h->HasSeverity())
          h->GetFlowRate().SetValue(hemorrhageFlow_mL_Per_s, VolumePerTimeUnit::mL_Per_s);

        totalLossRate_mL_Per_s += hemorrhageFlow_mL_Per_s;
        h->GetTotalBloodLost().Increment(hemorrhageFlow_mL_Per_s* m_data.GetTimeStep_s(), VolumeUnit::mL);
      }
    }
    GetTotalHemorrhageRate().SetValue(totalLossRate_mL_Per_s, VolumePerTimeUnit::mL_Per_s);
    GetTotalHemorrhagedVolume().Increment((totalLossRate_mL_Per_s* m_data.GetTimeStep_s()), VolumeUnit::mL);

    for (SEHemorrhage* ih : invalid_hemorrhages)
    {
      delete m_HemorrhageTrack[ih];
      m_HemorrhageTrack.erase(ih);
      m_data.GetActions().GetPatientActions().RemoveHemorrhage(ih->GetCompartment());
    }

    if (completeStateChange)
    {
      m_data.GetCompartments().StateChange();
      m_CirculatoryCircuit->StateChange();
      m_CirculatoryGraph->StateChange();
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// The function initiates a flow source on the pericardium. It is used by both the action and condition.
  ///
  /// \details
  /// The pericardial effusion action may be called during run time. It initiates a flow source on the pericardium
  /// which leads to increased pericardium volume. As the volume increases, a pressure source is applied to
  /// the left and right heart nodes, simulating the restriction of the swelling pericardium.
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::PericardialEffusion()
  {
    //We need to do this here because the circuit needs to be processed to modify the compliance pressure based on the volume change
    if (m_data.GetConditions().HasChronicPericardialEffusion() &&
      !m_data.GetActions().GetPatientActions().HasPericardialEffusion())
    {
      PericardialEffusionPressureApplication();
    }

    if (!m_data.GetActions().GetPatientActions().HasPericardialEffusion())
      return;

    double complianceSlopeParameter = 0.0;
    double complianceCurveParameter = 0.0;
    double flowToPericardium_mL_per_s = 0.0;
    double flowCubed_mL3_Per_s3 = 0.0;
    double compliance_mL_Per_mmHg = 0.0;
    double intrapericardialVolume_mL = m_Pericardium->GetVolume(VolumeUnit::mL);

    double effusionRate_mL_Per_s = m_data.GetActions().GetPatientActions().GetPericardialEffusion().GetEffusionRate().GetValue(VolumePerTimeUnit::mL_Per_s);
    if (effusionRate_mL_Per_s <= 0.1 && effusionRate_mL_Per_s > 0.0)
    {
      //Slow effusion
      complianceSlopeParameter = 0.4;
      complianceCurveParameter = 0.55;
    }
    else if (effusionRate_mL_Per_s > 0.1 && effusionRate_mL_Per_s < 1.0)
    {
      complianceSlopeParameter = 50;
      complianceCurveParameter = 0.1;
    }
    else if (effusionRate_mL_Per_s > 1.0)
    {
      Error("Effusion rate is out of physiologic bounds. Effusion rate is reset to 1.0 milliliters per second.");
      effusionRate_mL_Per_s = 1.0;
      complianceSlopeParameter = 50;
      complianceCurveParameter = 0.1;
    }
    else if (effusionRate_mL_Per_s < 0.0)
    {
      Error("Cannot specify effusion rate less than zero. Effusion rate is now set to 0.0.");
      effusionRate_mL_Per_s = 0.0;
      complianceSlopeParameter = 0.4;
      complianceCurveParameter = 0.55;
    }

    m_GndToPericardium->GetNextFlowSource().SetValue(effusionRate_mL_Per_s, VolumePerTimeUnit::mL_Per_s);
    flowToPericardium_mL_per_s = m_GndToPericardium->GetNextFlow(VolumePerTimeUnit::mL_Per_s);
    flowCubed_mL3_Per_s3 = flowToPericardium_mL_per_s * flowToPericardium_mL_per_s * flowToPericardium_mL_per_s;

    //Variable compliance calculation
    if (flowCubed_mL3_Per_s3 < 0.0001)
    {
      compliance_mL_Per_mmHg = m_PericardiumToGnd->GetNextCompliance().GetValue(VolumePerPressureUnit::mL_Per_mmHg);
    }
    else
    {
      compliance_mL_Per_mmHg = complianceSlopeParameter / flowCubed_mL3_Per_s3 - complianceCurveParameter * intrapericardialVolume_mL;
    }

    m_PericardiumToGnd->GetNextCompliance().SetValue(compliance_mL_Per_mmHg, VolumePerPressureUnit::mL_Per_mmHg);

    PericardialEffusionPressureApplication();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// The CPR function controls the force applied during a chest compression action.
  ///
  /// \details
  /// The user may apply a chest compression to continue blood circulation if the heart no longer has an effective rhythm.
  /// The compression can either be defined by an explicit force or by a fraction of the maximum allowable force. 
  /// If the compression input is a depth then the method controls the shape of the force pulse and converts the force
  /// pressure for application to the heart. 
  /// If the compression input is force, then the raw force is converted to pressure and applied to
  /// the heart. The pressure is applied at the pressure source on the LeftHeart3ToGround and 
  /// RightHeart3ToGround paths in the cardiovascular circuit.
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::CPR()
  {
    auto& actions = m_data.GetActions().GetPatientActions();
    if (!actions.HasActiveCPRAction())
      return;// Nothing to do

    double compressionForce_N = 0;
#define compressionForceMax_N 550.0 // The maximum allowed compression force (corresponds to 6 cm when depth is used)

    if (actions.HasChestCompressionInstantaneous())
    {
      m_CompressionFrequencyCurrentTime_s = 0;
      m_CompressionFrequencyDuration_s = 0;
      m_CompressionPeriod_s = 0;
      m_CompressionPeriodCurrentTime_s = 0;

      compressionForce_N = 0;
      if (actions.GetChestCompressionInstantaneous().HasForce())
        compressionForce_N = actions.GetChestCompressionInstantaneous().GetForce(ForceUnit::N);
      else if (actions.GetChestCompressionInstantaneous().HasDepth())
        compressionForce_N = CalculateDepthForce(actions.GetChestCompressionInstantaneous().GetDepth(LengthUnit::cm));
      if (compressionForce_N == 0)
      {
        actions.RemoveChestCompressionInstantaneous();
        return;
      }

      ApplyCPRForce(compressionForce_N);
      return;
    }

    if (actions.HasChestCompression())
    {
      m_CompressionFrequencyCurrentTime_s = 0;
      m_CompressionFrequencyDuration_s = 0;

      m_CompressionPeriod_s = actions.GetChestCompression().GetCompressionPeriod(TimeUnit::s);

      if (m_CompressionPeriodCurrentTime_s >= m_CompressionPeriod_s)
      {
        m_CompressionPeriodCurrentTime_s = 0;
        m_CompressionPeriod_s = 0;
        actions.RemoveChestCompression();
        return;
      }

      compressionForce_N = 0;
      if (actions.GetChestCompression().HasForce())
        compressionForce_N = actions.GetChestCompression().GetForce(ForceUnit::N);
      else if (actions.GetChestCompression().HasDepth())
        compressionForce_N = CalculateDepthForce(actions.GetChestCompression().GetDepth(LengthUnit::cm));
      if (compressionForce_N == 0)
      {
        m_CompressionPeriodCurrentTime_s = 0;
        m_CompressionPeriod_s = 0;
        actions.RemoveChestCompression();
        return;
      }

      compressionForce_N = ShapeCPRForce(compressionForce_N);

      ApplyCPRForce(compressionForce_N);

      m_CompressionPeriodCurrentTime_s += m_data.GetTimeStep_s();

      return;
    }

    if (actions.HasChestCompressionAutomated())
    {
      double compressionFrequency_Per_s = 0;
      if(actions.GetChestCompressionAutomated().HasCompressionFrequency())
        compressionFrequency_Per_s = actions.GetChestCompressionAutomated().GetCompressionFrequency(FrequencyUnit::Per_s);
      if (compressionFrequency_Per_s == 0)
      {
        m_CompressionFrequencyCurrentTime_s = 0;
        m_CompressionFrequencyDuration_s = 0;
        m_CompressionPeriodCurrentTime_s = 0;
        m_CompressionPeriod_s = 0;
        actions.RemoveChestCompressionAutomated();
        return;
      }
      m_CompressionFrequencyDuration_s = 1 / compressionFrequency_Per_s;

      m_CompressionPeriod_s = m_CompressionFrequencyDuration_s;
      if (actions.GetChestCompressionAutomated().HasAppliedForceFraction())
        m_CompressionPeriod_s *= actions.GetChestCompressionAutomated().GetAppliedForceFraction().GetValue();
      else
        m_CompressionPeriod_s *= 0.5; // Half the period should apply the pressure by default

      compressionForce_N = 0.0;
      if (actions.GetChestCompressionAutomated().HasForce())
        compressionForce_N = actions.GetChestCompressionAutomated().GetForce(ForceUnit::N);
      else if (actions.GetChestCompressionAutomated().HasDepth())
        compressionForce_N = CalculateDepthForce(actions.GetChestCompressionAutomated().GetDepth(LengthUnit::cm));
      else
      {
        m_CompressionFrequencyCurrentTime_s = 0;
        m_CompressionFrequencyDuration_s = 0;
        m_CompressionPeriodCurrentTime_s = 0;
        m_CompressionPeriod_s = 0;
        actions.RemoveChestCompressionAutomated();
        return;
      }

      if (compressionForce_N == 0)
      {
        m_CompressionPeriodCurrentTime_s = m_CompressionPeriod_s;
      }
      else
      {
        compressionForce_N = ShapeCPRForce(compressionForce_N);

        ApplyCPRForce(compressionForce_N);

        m_CompressionPeriodCurrentTime_s += m_data.GetTimeStep_s();
      }

      m_CompressionFrequencyCurrentTime_s += m_data.GetTimeStep_s();
      if (m_CompressionFrequencyCurrentTime_s >= m_CompressionFrequencyDuration_s)
      {
        // Start a new cycle
        m_CompressionPeriodCurrentTime_s = 0;
        m_CompressionFrequencyCurrentTime_s = 0;
        Info("Starting new automated CPR compression");
      }
    }

  }
  
  double CardiovascularModel::CalculateDepthForce(double compressionDepth_cm)
  {
#define compressionDepthTransition_cm 5.0
#define compressionForceTransition_N 450
#define compressionDepthMax_cm 6.0
    if (compressionDepth_cm >= 5)
      return GeneralMath::LinearInterpolator(compressionDepthTransition_cm, compressionDepthMax_cm, compressionForceTransition_N, compressionForceMax_N, compressionDepth_cm);
    else
      return GeneralMath::LinearInterpolator(0, compressionDepthTransition_cm, 0, compressionForceTransition_N, compressionDepth_cm);
  }
  
  double CardiovascularModel::ShapeCPRForce(double compressionForce_N)
  {
    // Bell curve shaping parameters
    double c = -10; // Defines the start and stop of the force bell curve given the period
    double a = 4 * c / (m_CompressionPeriod_s * m_CompressionPeriod_s);
    double b = -a * m_CompressionPeriod_s;

    double shapedCompressionForce_N = pow(2, a * m_CompressionPeriodCurrentTime_s * m_CompressionPeriodCurrentTime_s + b * m_CompressionPeriodCurrentTime_s + c);

    return shapedCompressionForce_N * compressionForce_N;
  }
  void CardiovascularModel::ApplyCPRForce(double compressionForce_N)
  {
    if (compressionForce_N > compressionForceMax_N)
    {
      compressionForce_N = compressionForceMax_N;
      Warning("The compression force exceeded the maximum compression force. Compression force limited to "+std::to_string(compressionForceMax_N)+"N.");
    }

    if (compressionForce_N < 0)
    {
      compressionForce_N = 0;
      Warning("The compression force was less than the required minimum. Compression force limited to 0N.");
    }

    if (compressionForce_N == 0)
      return;

    double leftHeartForceToPressureFactor = 0.11; // Tuning parameter to translate compression force in N to left heart pressure in mmHg
    double rightHeartForceToPressureFactor = 0.05; // Tuning parameter to translate compression force in N to right heart pressure in mmHg
    double nextLeftPressure_mmHg = leftHeartForceToPressureFactor * compressionForce_N;
    double nextRightPressure_mmHg = rightHeartForceToPressureFactor * compressionForce_N;

    m_RightHeartToGnd->GetNextPressureSource().SetValue(nextRightPressure_mmHg, PressureUnit::mmHg);
    m_LeftHeartToGnd->GetNextPressureSource().SetValue(nextLeftPressure_mmHg, PressureUnit::mmHg);
  }

  void CardiovascularModel::SetHeartRhythm(eHeartRhythm r)
  {
    SetHeartRhythm(r, false);
  }
  void CardiovascularModel::SetHeartRhythm(eHeartRhythm r, bool force)
  {
    if (force)
      SECardiovascularSystem::SetHeartRhythm(r);
    else
    {
      // Make sure that non cardiac arrest rhythms, set by an action, are not reset here
      if (GetHeartRhythm() == eHeartRhythm::StableVentricularTachycardia ||
          GetHeartRhythm() == eHeartRhythm::UnstableVentricularTachycardia)
        return;
      SECardiovascularSystem::SetHeartRhythm(r);
    }
  }
  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// The arrythmia action causes the heart to beat too quickly, too slowly, or with an irregular pattern..
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::Arrhythmia()
  {
    // Only process/transition arrhythmia actions when the heart is fully compressed
    if(m_data.GetActions().GetPatientActions().HasArrhythmia())
    {
      if (!m_data.GetEvents().IsEventActive(eEvent::CardiacArrest))
        if(!m_FullyCompressedHeart)
          return;

      auto newRhythm = m_data.GetActions().GetPatientActions().GetArrhythmia().GetRhythm();
      m_data.GetActions().GetPatientActions().RemoveArrhythmia();// Done with the action

      Info("Switching heart rhythm to " + eHeartRhythm_Name(newRhythm));

      // Using a 90% tgt heart rate below when coming out of cardiac arrest
      // This is due to setting the baseline map to whatever the map is at the end of transition
      // We always come out at a map a bit higher, so nervous tends to increase the hr to lower the map
      // So using the 90% target tends to settle out both the map and hr to where we want it

      // The transition times are tuned to get a smooth curve by the end of the hr change
      // Get the compliance / resistance changes in place and let the system stablilize out as we converge the HR

      if (GetHeartRhythm() != newRhythm)
      {
        SetHeartRhythm(newRhythm, true);
        switch (newRhythm)
        {
        case eHeartRhythm::Asystole:
        case eHeartRhythm::CoarseVentricularFibrillation:
        case eHeartRhythm::FineVentricularFibrillation:
        case eHeartRhythm::SinusPulselessElectricalActivity:
        case eHeartRhythm::PulselessVentricularTachycardia:
        {
          // Flip the cardiac arrest switch
          // This tells the CV system that a cardiac arrest has been initiated.
          m_StartCardiacArrest   = true;
          m_CardiacArrestVitalsUpdateTimer_s = 0;

          // No transition times
          m_TransitionArrhythmia = false;
          m_HeartRateBaseline_Per_min->SetTransition(GetHeartRate(FrequencyUnit::Per_min), 15, 0);
          m_HeartComplianceModifier->SetTarget(0.75, 0);
          m_AortaComplianceModifier->SetTarget(1.25, 0);
          m_VenaCavaComplianceModifier->SetTarget(0.25, 0);
          m_RespiratoryComplianceModifier->SetTarget(0.75, 0);
          m_SystemicVascularResistanceModifier->SetTarget(1.5, 0);
          m_SystemicVascularComplianceModifier->SetTarget(1.0, 0);

          m_data.GetCurrentPatient().GetMeanArterialPressureBaseline().SetValue(18, PressureUnit::mmHg);
          m_data.GetCurrentPatient().GetHeartRateBaseline().SetValue(m_HeartRateBaseline_Per_min->GetCurrent(), FrequencyUnit::Per_min);

          m_EnableFeedbackAfterArrhythmiaTrasition = eSwitch::Off;
          m_data.GetNervous().SetBaroreceptorFeedback(eSwitch::Off);
          m_data.GetNervous().SetChemoreceptorFeedback(eSwitch::Off);
          break;
        }
        case eHeartRhythm::NormalSinus:
        {
          m_TransitionArrhythmia = true;
          double tgtHeartRate_Per_min = m_StabilizedHeartRateBaseline_Per_min;
          if (m_data.GetEvents().IsEventActive(eEvent::CardiacArrest))
            tgtHeartRate_Per_min *= 0.9;// See above
          m_HeartRateBaseline_Per_min->SetTarget(tgtHeartRate_Per_min, 60);
          m_HeartComplianceModifier->SetTarget(1.0, 30);
          m_AortaComplianceModifier->SetTarget(1.0, 15);
          m_VenaCavaComplianceModifier->SetTarget(1.0, 20);
          m_RespiratoryComplianceModifier->SetTarget(1.0, 30);
          m_SystemicVascularResistanceModifier->SetTarget(1.0, 15);
          m_SystemicVascularComplianceModifier->SetTarget(1.0, 30);

          // Force a new cardiac cycle
          m_StartSystole = true;
          m_HeartFlowDetected = true;
          m_EnableFeedbackAfterArrhythmiaTrasition = eSwitch::On;
          m_data.GetNervous().SetBaroreceptorFeedback(eSwitch::Off);
          m_data.GetNervous().SetChemoreceptorFeedback(eSwitch::Off);
          m_data.GetEvents().SetEvent(eEvent::CardiacArrest, false, m_data.GetSimulationTime());
          break;
        }
        case eHeartRhythm::SinusBradycardia:
        {
          m_TransitionArrhythmia = true;
          double tgtHeartRate_Per_min = 50;
          if (m_data.GetEvents().IsEventActive(eEvent::CardiacArrest))
            tgtHeartRate_Per_min *= 0.9;// See above
          m_HeartRateBaseline_Per_min->SetTarget(tgtHeartRate_Per_min, 60);
          m_HeartComplianceModifier->SetTarget(1.0, 30);
          m_AortaComplianceModifier->SetTarget(1.0, 15);
          m_VenaCavaComplianceModifier->SetTarget(1.0, 15);
          m_RespiratoryComplianceModifier->SetTarget(1.0, 30);
          m_SystemicVascularResistanceModifier->SetTarget(1.0, 15);
          m_SystemicVascularComplianceModifier->SetTarget(1.0, 30);

          // Force a new cardiac cycle
          m_StartSystole = true;
          m_HeartFlowDetected = true;
          m_EnableFeedbackAfterArrhythmiaTrasition = eSwitch::On;
          m_data.GetNervous().SetBaroreceptorFeedback(eSwitch::Off);
          m_data.GetNervous().SetChemoreceptorFeedback(eSwitch::Off);
          m_data.GetEvents().SetEvent(eEvent::CardiacArrest, false, m_data.GetSimulationTime());
          break;
        }
        case eHeartRhythm::SinusTachycardia:
        case eHeartRhythm::StableVentricularTachycardia:
        {
          m_TransitionArrhythmia = true;
          double tgtHeartRate_Per_min = 130;
          if (m_data.GetEvents().IsEventActive(eEvent::CardiacArrest))
            tgtHeartRate_Per_min *= 0.9;// See above
          m_HeartRateBaseline_Per_min->SetTarget(tgtHeartRate_Per_min, 60);
          m_HeartComplianceModifier->SetTarget(1.05, 30);
          m_AortaComplianceModifier->SetTarget(1.0, 15);
          m_VenaCavaComplianceModifier->SetTarget(1.05, 15);
          m_RespiratoryComplianceModifier->SetTarget(1.0, 30);
          m_SystemicVascularResistanceModifier->SetTarget(0.85, 15);
          m_SystemicVascularComplianceModifier->SetTarget(1.0, 30);

          // Force a new cardiac cycle
          m_StartSystole = true;
          m_HeartFlowDetected = true;
          m_EnableFeedbackAfterArrhythmiaTrasition = eSwitch::On;
          m_data.GetNervous().SetBaroreceptorFeedback(eSwitch::Off);
          m_data.GetNervous().SetChemoreceptorFeedback(eSwitch::Off);
          m_data.GetEvents().SetEvent(eEvent::CardiacArrest, false, m_data.GetSimulationTime());
          break;
        }
        case eHeartRhythm::UnstableVentricularTachycardia:
        {
          m_TransitionArrhythmia = true;
          double tgtHeartRate_Per_min = 160;
          if (m_data.GetEvents().IsEventActive(eEvent::CardiacArrest))
            tgtHeartRate_Per_min *= 0.9;// See above
          m_HeartRateBaseline_Per_min->SetTarget(tgtHeartRate_Per_min, 60);
          m_HeartComplianceModifier->SetTarget(1.25, 30);
          m_AortaComplianceModifier->SetTarget(1.0, 15);
          m_VenaCavaComplianceModifier->SetTarget(1.25, 15);
          m_RespiratoryComplianceModifier->SetTarget(1.0, 30);
          m_SystemicVascularResistanceModifier->SetTarget(0.5, 15);
          m_SystemicVascularComplianceModifier->SetTarget(1.5, 30);

          // Force a new cardiac cycle
          m_StartSystole = true;
          m_HeartFlowDetected = true;
          m_EnableFeedbackAfterArrhythmiaTrasition = eSwitch::On;
          m_data.GetNervous().SetBaroreceptorFeedback(eSwitch::Off);
          m_data.GetNervous().SetChemoreceptorFeedback(eSwitch::Off);
          m_data.GetEvents().SetEvent(eEvent::CardiacArrest, false, m_data.GetSimulationTime());
          break;
        }
        default:// Any other rhythms take us out of cardiac arrest
          Error("Unsupported heart arrhythmia.");
        }
      }
      else
        Warning("Requesting an arrhytmia that is the current arrhytmia, ignoring this action");
    }
    else
    {
      if (m_TransitionArrhythmia)
      {
        // Check to see if everything is complete
        double dt_s = m_data.GetTimeStep_s();
        m_TransitionArrhythmia  = m_HeartRateBaseline_Per_min->Transition(dt_s);
        m_TransitionArrhythmia &= m_HeartComplianceModifier->Transition(dt_s);
        m_TransitionArrhythmia &= m_AortaComplianceModifier->Transition(dt_s);
        m_TransitionArrhythmia &= m_VenaCavaComplianceModifier->Transition(dt_s);
        m_TransitionArrhythmia &= m_RespiratoryComplianceModifier->Transition(dt_s);
        m_TransitionArrhythmia &= m_SystemicVascularResistanceModifier->Transition(dt_s);
        m_TransitionArrhythmia &= m_SystemicVascularComplianceModifier->Transition(dt_s);
        m_TransitionArrhythmia = !m_TransitionArrhythmia;

        if (!m_TransitionArrhythmia)
        {
          Info("Completed Arrhythmia Transition");
          m_data.GetNervous().SetBaroreceptorFeedback(m_EnableFeedbackAfterArrhythmiaTrasition);
          m_data.GetNervous().SetChemoreceptorFeedback(m_EnableFeedbackAfterArrhythmiaTrasition);
        }

        m_data.GetCurrentPatient().GetMeanArterialPressureBaseline().Set(GetMeanArterialPressure());
        m_data.GetCurrentPatient().GetHeartRateBaseline().SetValue(m_HeartRateBaseline_Per_min->GetCurrent(), FrequencyUnit::Per_min);
      }
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// The pericardial effusion pressure application function calculates the pressure applied to the heart due to a pericardial effusion.
  ///
  /// \details
  /// The pressure applied to the left and right heart is dictated by the pericardium pressure. The response is tuned to 40% of this value
  /// to achieve the correct physiologic response.
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::PericardialEffusionPressureApplication()
  {
    double intrapericardialPressure_mmHg = m_Pericardium->GetPressure(PressureUnit::mmHg);

    double pressureResponseFraction = 0.4; //Tuning the pressure applied to the heart

    //Set the pressure on the right and left heart from the pericardium pressure
    m_RightHeartToGnd->GetPressureSourceBaseline().SetValue(pressureResponseFraction * intrapericardialPressure_mmHg, PressureUnit::mmHg);
    m_LeftHeartToGnd->GetPressureSourceBaseline().SetValue(pressureResponseFraction * intrapericardialPressure_mmHg, PressureUnit::mmHg);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates the contraction and relaxation of the left and right heart during the driver cycle
  ///
  /// \details
  /// This function tracks the progress of the current driver cycle, and modifies the compliance of the left
  /// and right heart to drive the cardiovascular circuit. The reduced compliance at the beginning of the cycle
  /// acts to increase the pressure, driving flow out of the heart. The compliance is then reduced allowing flow into
  /// the heart. This represents the systolic and diastolic portion of the cardiac cycle. The compliance is
  /// driven by an elastance equation.
  /// This function also keeps track of the driver cycle time and calls BeginDriverCycle() at the start of
  /// systole portion of each cycle. Modifications to heart rate and heart compliance are calculated by
  /// BeginDriverCycle() and applied for the remained of the current driver cycle.
  /// NOTE: Changes to CDM values (like heart rate and heart contractility can only occur at the top of the current CARDIAC cycle
  /// The cardiac cycle is determined independently of the driver cycle. It is based on the heart flow done in CalculateVitalSigns
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::HeartDriver()
  {
    m_data.GetEvents().SetEvent(eEvent::StartOfCardiacCycle, false, m_data.GetSimulationTime());

    if (m_StartCardiacArrest)
    {
      Info("Starting Cardiac Arrest");
      m_StartSystole = false;
      m_StartCardiacArrest = false;

      m_DriverCyclePeriod_s = 1.0e9; // Not beating, so set the period to a large number (1.0e9 sec = 31.7 years)

      m_CardiacCycleArterialPressure_mmHg->Invalidate();
      m_CardiacCycleArterialCO2PartialPressure_mmHg->Invalidate();
      m_CardiacCyclePulmonaryCapillariesWedgePressure_mmHg->Invalidate();
      m_CardiacCyclePulmonaryCapillariesFlow_mL_Per_s->Invalidate();
      m_CardiacCyclePulmonaryShuntFlow_mL_Per_s->Invalidate();
      m_CardiacCyclePulmonaryArteryPressure_mmHg->Invalidate();
      m_CardiacCycleCentralVenousPressure_mmHg->Invalidate();
      m_CardiacCycleSkinFlow_mL_Per_s->Invalidate();

      GetHeartRate().SetValue(0.0, FrequencyUnit::Per_min);
      m_data.GetEvents().SetEvent(eEvent::CardiacArrest, true, m_data.GetSimulationTime());

      m_data.GetNervous().SetBaroreceptorFeedback(eSwitch::Off);
      m_data.GetNervous().SetChemoreceptorFeedback(eSwitch::Off);
      m_data.GetEvents().SetEvent(eEvent::StartOfCardiacCycle, true, m_data.GetSimulationTime());
    }

    double LeftHeartCompliance_mL_Per_mmHg;
    double RightHeartCompliance_mL_Per_mmHg;
    if (!m_data.GetEvents().IsEventActive(eEvent::CardiacArrest))
    {
      if (m_CurrentDriverCycleTime_s > m_DriverCyclePeriod_s && !m_StartCardiacArrest)
        m_StartSystole = true;

      if (m_StartSystole)
      {
        BeginDriverCycle();
        m_StartSystole = false;
        m_CurrentDriverCycleTime_s = 0.0;
        m_data.GetEvents().SetEvent(eEvent::StartOfCardiacCycle, true, m_data.GetSimulationTime());
      }

      CalculateHeartElastance();
      LeftHeartCompliance_mL_Per_mmHg = (1.0 / m_LeftHeartElastance_mmHg_Per_mL) * m_HeartComplianceModifier->GetCurrent();
      RightHeartCompliance_mL_Per_mmHg = (1.0 / m_RightHeartElastance_mmHg_Per_mL) * m_HeartComplianceModifier->GetCurrent();
    }
    else
    {
      LeftHeartCompliance_mL_Per_mmHg = m_LeftHeartCompliancePath->GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg)
                                      * m_HeartComplianceModifier->GetCurrent();
      RightHeartCompliance_mL_Per_mmHg = m_RightHeartCompliancePath->GetComplianceBaseline(VolumePerPressureUnit::mL_Per_mmHg)
                                       * m_HeartComplianceModifier->GetCurrent();
    }

    AdjustVascularTone();
    m_LeftHeartCompliancePath->GetNextCompliance().SetValue(LeftHeartCompliance_mL_Per_mmHg, VolumePerPressureUnit::mL_Per_mmHg);
    m_RightHeartCompliancePath->GetNextCompliance().SetValue(RightHeartCompliance_mL_Per_mmHg, VolumePerPressureUnit::mL_Per_mmHg);

    //m_data.GetDataTrack().Probe("LeftHeartCompliance_mL_Per_mmHg", LeftHeartCompliance_mL_Per_mmHg);
    //m_data.GetDataTrack().Probe("RightHeartCompliance_mL_Per_mmHg", RightHeartCompliance_mL_Per_mmHg);
    //m_data.GetDataTrack().Probe("RightHeartPressureSource_mmHg", m_RightHeartToGnd->GetNextPressureSource().GetValue(PressureUnit::mmHg));
    //m_data.GetDataTrack().Probe("LeftHeartPressureSource_mmHg", m_LeftHeartToGnd->GetNextPressureSource().GetValue(PressureUnit::mmHg));

    m_CurrentDriverCycleTime_s += m_data.GetTimeStep_s();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Sets up the evolution of the next driver cycle.
  ///
  /// \details
  /// This function is directed from CardiovascularModel::HeartDriver. It set's up the evolution of the proceeding cardiac
  /// cycle. It is used to apply the effects of drugs or exercise on the cardiovascular system. 
  /// These effects will persist for the remainder of the cardiac cycle, at which point this function
  /// is called again if a new heart beat is warranted (i.e. not in cardiac arrest).
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::BeginDriverCycle()
  {
    // Changes to the heart rate and other hemodynamic parameters are applied at the top of the cardiac cycle.
    // Parameters cannot change during the cardiac cycle because the heart beat is modeled as a changing compliance.

    double HeartDriverFrequency_Per_Min = m_data.GetCurrentPatient().GetHeartRateBaseline(FrequencyUnit::Per_min);
    m_LeftHeartElastanceMax_mmHg_Per_mL = m_data.GetConfiguration().GetLeftHeartElastanceMaximum(PressurePerVolumeUnit::mmHg_Per_mL);
    m_RightHeartElastanceMax_mmHg_Per_mL = m_data.GetConfiguration().GetRightHeartElastanceMaximum(PressurePerVolumeUnit::mmHg_Per_mL);

#ifndef TUNING
    // Apply baroreceptor reflex effects
    /// \todo need to reset the heart elastance min and max at the end of each stabiliation period in AtSteadyState()
    if (m_data.GetNervous().GetBaroreceptorFeedback() == eSwitch::On)
    {
      m_LeftHeartElastanceMax_mmHg_Per_mL *= m_data.GetNervous().GetBaroreceptorHeartElastanceScale().GetValue();
      m_RightHeartElastanceMax_mmHg_Per_mL *= m_data.GetNervous().GetBaroreceptorHeartElastanceScale().GetValue();
      HeartDriverFrequency_Per_Min *= m_data.GetNervous().GetBaroreceptorHeartRateScale().GetValue();
    }

    // Chemoreceptor and drug effects are deltas rather than multipliers, so they are added.
    // Apply chemoreceptor effects
    if (m_data.GetNervous().GetChemoreceptorFeedback() == eSwitch::On)
    {
      HeartDriverFrequency_Per_Min += m_data.GetNervous().GetChemoreceptorHeartRateScale().GetValue();
    }

    // Apply drug effects
    if (m_data.GetDrugs().HasHeartRateChange())
    {
      HeartDriverFrequency_Per_Min += m_data.GetDrugs().GetHeartRateChange(FrequencyUnit::Per_min);
    }

    // Custom modifier
    HeartDriverFrequency_Per_Min *= m_MechanicsModifiers->GetHeartRateMultiplier().GetValue();

    BLIM(HeartDriverFrequency_Per_Min, m_data.GetCurrentPatient().GetHeartRateMinimum(FrequencyUnit::Per_min), m_data.GetCurrentPatient().GetHeartRateMaximum(FrequencyUnit::Per_min));

    //Apply heart failure effects
    m_LeftHeartElastanceMax_mmHg_Per_mL *= m_LeftHeartElastanceModifier;

    // Custom modifier
    double strokeVolumeMultiplier = m_MechanicsModifiers->GetStrokeVolumeMultiplier().GetValue();
    if (strokeVolumeMultiplier > 1.0)
    {
      strokeVolumeMultiplier *= strokeVolumeMultiplier;
    }
    m_LeftHeartElastanceMax_mmHg_Per_mL *= strokeVolumeMultiplier;
    m_RightHeartElastanceMax_mmHg_Per_mL *= strokeVolumeMultiplier;
#endif


    if (m_data.GetState() > EngineState::InitialStabilization && //Only dampen response if we're not initializing
      !m_TransitionArrhythmia) //and not transitioning between arrhythmias
    {
      //Dampen the change to prevent craziness
      double previousHeartDriverFrequency_Per_Min = GetHeartRate(FrequencyUnit::Per_min);
      double dampenFraction_perSec = 2.0;
      HeartDriverFrequency_Per_Min = GeneralMath::Damper(HeartDriverFrequency_Per_Min, previousHeartDriverFrequency_Per_Min, dampenFraction_perSec, m_data.GetTimeStep_s());
    }

    m_DriverCyclePeriod_s = 60.0 / HeartDriverFrequency_Per_Min;
    // Snap the cycle period to the nearest time step
    m_DriverCyclePeriod_s = std::floor((m_DriverCyclePeriod_s / m_data.GetTimeStep_s()) + 0.5) * m_data.GetTimeStep_s();

    GetHeartRate().SetValue(HeartDriverFrequency_Per_Min, FrequencyUnit::Per_min);
    RecordAndResetCardiacCycle();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculation of the left and right ventricular elastance
  ///
  /// \details
  /// This function calculates the left and right ventricular elastance at the current time in the cardiac cycle.
  /// The elastance takes the form of a double hill function with a period equivalent to the cardiac cycle length.
  /// \cite stergiopulos1996elastance
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::CalculateHeartElastance()
  {
    //Shape parameters, used to define double hill functional form of the elastance
    double alpha1 = 0.303;
    double alpha2 = 0.508;
    double n1 = 1.32;
    double n2 = 21.9;
    double maxShape = 0.598;
    double oxygenDeficitEffect = 1.0;

    if (m_data.GetEvents().IsEventActive(eEvent::MyocardiumOxygenDeficit) == true)
    {
      double eventDuration = m_data.GetEvents().GetEventDuration(eEvent::MyocardiumOxygenDeficit, TimeUnit::s);
      oxygenDeficitEffect = pow(-3E-9 * eventDuration, 2) + 8E-6 * eventDuration + 0.9865;
    }

    double normalizedCardiacTime = m_CurrentDriverCycleTime_s / m_DriverCyclePeriod_s;
    double elastanceShapeFunction = (pow(normalizedCardiacTime / alpha1, n1) / (1.0 + pow(normalizedCardiacTime / alpha1, n1))) * (1.0 / (1.0 + pow(normalizedCardiacTime / alpha2, n2))) / maxShape;

    m_LeftHeartElastance_mmHg_Per_mL = oxygenDeficitEffect * ((m_LeftHeartElastanceMax_mmHg_Per_mL - m_LeftHeartElastanceMin_mmHg_Per_mL) * elastanceShapeFunction + m_LeftHeartElastanceMin_mmHg_Per_mL);
    m_RightHeartElastance_mmHg_Per_mL = oxygenDeficitEffect * ((m_RightHeartElastanceMax_mmHg_Per_mL - m_RightHeartElastanceMin_mmHg_Per_mL) * elastanceShapeFunction + m_RightHeartElastanceMin_mmHg_Per_mL);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates metabolic and thermal feedback on the systemic vascular tone
  ///
  /// \details
  /// This method controls the vascular tone response to metabolic changes. As the metabolic rate increases, 
  /// the resistances to non-vital organs is increased while the resistance to the muscle and skin is reduced,
  /// thus increasing heat convective rejection (see Energy::UpdateHeatResistance()). In the case of decreasing core temperature
  /// the resistance to the skin is increased to shunt blood flow and reduce heat transfer from the core to skin.
  //--------------------------------------------------------------------------------------------------
  /// \todo Possibly add a thermoreception model to nervous and move the computation there.
  void CardiovascularModel::MetabolicToneResponse()
  {
    double metabolicFraction = 1.0;
    if (m_data.GetEnergy().HasTotalMetabolicRate())
    {
      double TMR_kcal_Per_day = m_data.GetEnergy().GetTotalMetabolicRate(PowerUnit::kcal_Per_day);
      metabolicFraction = TMR_kcal_Per_day / m_data.GetCurrentPatient().GetBasalMetabolicRate(PowerUnit::kcal_Per_day);
    }

    if (metabolicFraction == 1.0)
      return;

    double coreTemp_degC = m_data.GetEnergy().GetCoreTemperature(TemperatureUnit::C); //Resting: 37.0 degC
    double coreTempSet_degC = m_data.GetConfiguration().GetCoreTemperatureHigh(TemperatureUnit::C); //37.1 degC
    double coreTempDelta_degC = MAX(coreTemp_degC - coreTempSet_degC, 0.0);
    coreTempDelta_degC = MIN(coreTempDelta_degC, 1.0); //A 1 degree increase in core temperature is the where the cardiovascular response on resistances is capped
    //The skin multiplier is used to increase the skin blood flow resistance, effectively reducing the skin blood flow leading to less heat transfered from core to skin.
    //double skinMultiplier = 1.0 / MAX((coreTemp_degC - 35.0), 0.001);
    //double coreTempLow_degC = m_data.GetConfiguration().GetCoreTemperatureLow(TemperatureUnit::C); //36.8 degC
    /// \cite talebipour2006sauna
    // double tempMultiplier = 1.0 - 0.4*MIN(coreTempDelta_degC, 1.0); //Approximate 40% reduction in peripheral resistance due to core temperature rise of 1 degree.
    double metabolicModifier = 1.0;
    //The metabolic multiplier is used as a tuned response to represent cardiovascular resistance effects during exercise
    double sp0 = 1.5;
    double divisor = 2.5;
    double metabolicMultiplier = (sp0 * metabolicFraction + (divisor - sp0)) / divisor;

    // Max delta approx. 20% of baseline \cite christie1997cardiac \cite foster1999left
    double metabolicRateMeanArterialPressureDelta_mmHg = (0.05 * metabolicFraction - 0.05) * m_data.GetCurrentPatient().GetMeanArterialPressureBaseline(PressureUnit::mmHg);
    m_data.GetEnergy().GetExerciseMeanArterialPressureDelta().SetValue(metabolicRateMeanArterialPressureDelta_mmHg, PressureUnit::mmHg);

    //Reducing resistances scaling with metabolic rate increase and changes in core temperature
    double resistanceNew__mmHg_s_Per_mL = 0.0;

    for (SEFluidCircuitPath* Path : m_SystemicResistancePaths)
    {
      if (Path->HasResistanceBaseline())
      {
        if (std::find(m_MuscleResistancePaths.begin(), m_MuscleResistancePaths.end(), Path) != m_MuscleResistancePaths.end())
        { // Biggest change in muscle
          resistanceNew__mmHg_s_Per_mL = (1.0 / metabolicModifier) * (1.0 / metabolicModifier) * Path->GetNextResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
        }
        else if (Path == m_AortaToBrain || Path == m_AortaToMyocardium || Path == m_BrainToVenaCava || Path == m_MyocardiumToVenaCava)
        { // No Change in the brain or myocardium
          resistanceNew__mmHg_s_Per_mL = Path->GetNextResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
        }
        else
        { // Smaller change in the rest of the tissue paths
          resistanceNew__mmHg_s_Per_mL = (1.0 / metabolicModifier) * Path->GetNextResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
        }

        // Overall reduction in flow resistance in all paths to allow for increased cardiac output with a metabolic rate increase
        /// \todo Skip over Brain and Myocardium and add arms and legs
        resistanceNew__mmHg_s_Per_mL *= (1.0 / metabolicMultiplier);
        if (resistanceNew__mmHg_s_Per_mL < m_MinIndividialSystemicResistance_mmHg_s_Per_mL)
        {
          resistanceNew__mmHg_s_Per_mL = m_MinIndividialSystemicResistance_mmHg_s_Per_mL;
        }
        Path->GetNextResistance().SetValue(resistanceNew__mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
      }
    }
   }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Adjusts vascular tone due to baroreceptor reflex, drug, and other effects
  ///
  /// \details
  /// Adjusts vascular tone due to baroreceptor reflex, drug, and other effects
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::AdjustVascularTone()
  {
    /// \todo Add dilation (decreased resistance) to myocardium supply path if myocardium oxygen deficit event is active.

    //The baroreceptor response adjusts the systemic resistances and compliances according to the multiplier calculated in Nervous.cpp
    double UpdatedResistance_mmHg_s_Per_mL = 0.0;
    double UpdatedCompliance_mL_Per_mmHg = 0.0;

    double BaroreceptorResistanceScale = 1.0;
    double BaroreceptorComplianceScale = 1.0;
    if (m_data.GetNervous().GetBaroreceptorFeedback() == eSwitch::On)
    {
      BaroreceptorResistanceScale = m_data.GetNervous().GetBaroreceptorResistanceScale().GetValue();
      BaroreceptorComplianceScale = m_data.GetNervous().GetBaroreceptorComplianceScale().GetValue();
    }

    //Drug effects
    double meanBloodPressureChange_mmHg = 0.0;
    double pulsePressureChange_mmHg = 0.0;
    if (m_data.GetDrugs().HasMeanBloodPressureChange())
    {
      meanBloodPressureChange_mmHg = m_data.GetDrugs().GetMeanBloodPressureChange(PressureUnit::mmHg);
    }
    if (m_data.GetDrugs().HasPulsePressureChange())
    {
      pulsePressureChange_mmHg = m_data.GetDrugs().GetPulsePressureChange(PressureUnit::mmHg);
    }

    double drugSystemicResistanceScale = 1;
    double drugAortaComplianceScale = 1;

    double srGain = 0.01;
    double acGain = 0.01;
    if (meanBloodPressureChange_mmHg)
    {
      drugSystemicResistanceScale += srGain * meanBloodPressureChange_mmHg;
      BLIM(drugSystemicResistanceScale, 0.5, 2.0);
    }

    if (pulsePressureChange_mmHg != 0.0)
    {
      drugAortaComplianceScale -= acGain * pulsePressureChange_mmHg;
      BLIM(drugAortaComplianceScale, 0.5, 2.0);
    }

    //Apply modifiers
    for (SEFluidCircuitPath* Path : m_SystemicResistancePaths)
    {
      /// \todo We are treating all systemic resistance paths equally, including the brain.
      UpdatedResistance_mmHg_s_Per_mL = Path->GetNextResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
      UpdatedResistance_mmHg_s_Per_mL *= BaroreceptorResistanceScale;
      UpdatedResistance_mmHg_s_Per_mL *= m_SystemicVascularResistanceModifier->GetCurrent();
      UpdatedResistance_mmHg_s_Per_mL *= drugSystemicResistanceScale;
      if (UpdatedResistance_mmHg_s_Per_mL < m_MinIndividialSystemicResistance_mmHg_s_Per_mL)
        UpdatedResistance_mmHg_s_Per_mL = m_MinIndividialSystemicResistance_mmHg_s_Per_mL;
      UpdatedResistance_mmHg_s_Per_mL *= m_MechanicsModifiers->GetSystemicResistanceMultiplier().GetValue();
      Path->GetNextResistance().SetValue(UpdatedResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    }

    for (SEFluidCircuitPath* Path : m_SystemicCompliancePaths)
    {
      UpdatedCompliance_mL_Per_mmHg = Path->GetNextCompliance(VolumePerPressureUnit::mL_Per_mmHg);
      UpdatedCompliance_mL_Per_mmHg *= BaroreceptorComplianceScale;
      UpdatedCompliance_mL_Per_mmHg *= m_SystemicVascularComplianceModifier->GetCurrent();
      UpdatedCompliance_mL_Per_mmHg *= m_MechanicsModifiers->GetSystemicComplianceMultiplier().GetValue();
      Path->GetNextCompliance().SetValue(UpdatedCompliance_mL_Per_mmHg, VolumePerPressureUnit::mL_Per_mmHg);
    }

    for (SEFluidCircuitPath* Path : m_AortaResistancePaths)
    {
      UpdatedResistance_mmHg_s_Per_mL = Path->GetNextResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
      UpdatedResistance_mmHg_s_Per_mL *= m_MechanicsModifiers->GetArterialResistanceMultiplier().GetValue();
      Path->GetNextResistance().SetValue(UpdatedResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    }

    for (SEFluidCircuitPath* Path : m_AortaCompliancePaths)
    {
      UpdatedCompliance_mL_Per_mmHg = Path->GetNextCompliance(VolumePerPressureUnit::mL_Per_mmHg);
      UpdatedCompliance_mL_Per_mmHg *= m_AortaComplianceModifier->GetCurrent();
      UpdatedCompliance_mL_Per_mmHg *= drugAortaComplianceScale;
      UpdatedCompliance_mL_Per_mmHg *= m_MechanicsModifiers->GetArterialComplianceMultiplier().GetValue();
      Path->GetNextCompliance().SetValue(UpdatedCompliance_mL_Per_mmHg, VolumePerPressureUnit::mL_Per_mmHg);
    }

    for (SEFluidCircuitPath* Path : m_VenaCavaResistancePaths)
    {
      UpdatedResistance_mmHg_s_Per_mL = Path->GetNextResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
      UpdatedResistance_mmHg_s_Per_mL *= m_MechanicsModifiers->GetVenousResistanceMultiplier().GetValue();
      Path->GetNextResistance().SetValue(UpdatedResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    }

    for (SEFluidCircuitPath* Path : m_VenaCavaCompliancePaths)
    {
      UpdatedCompliance_mL_Per_mmHg = Path->GetNextCompliance(VolumePerPressureUnit::mL_Per_mmHg);
      UpdatedCompliance_mL_Per_mmHg *= m_VenaCavaComplianceModifier->GetCurrent();
      UpdatedCompliance_mL_Per_mmHg *= m_MechanicsModifiers->GetVenousComplianceMultiplier().GetValue();
      Path->GetNextCompliance().SetValue(UpdatedCompliance_mL_Per_mmHg, VolumePerPressureUnit::mL_Per_mmHg);
    }

    for (SEFluidCircuitPath* Path : m_PulmonaryResistancePaths)
    {
      UpdatedResistance_mmHg_s_Per_mL = Path->GetNextResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
      UpdatedResistance_mmHg_s_Per_mL *= m_MechanicsModifiers->GetPulmonaryResistanceMultiplier().GetValue();
      Path->GetNextResistance().SetValue(UpdatedResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    }

    for (SEFluidCircuitPath* Path : m_RespiratoryCompliancePaths)
    {
      UpdatedCompliance_mL_Per_mmHg = Path->GetNextCompliance(VolumePerPressureUnit::mL_Per_mmHg);
      UpdatedCompliance_mL_Per_mmHg *= m_RespiratoryComplianceModifier->GetCurrent();
      UpdatedCompliance_mL_Per_mmHg *= m_MechanicsModifiers->GetPulmonaryComplianceMultiplier().GetValue();
      Path->GetNextCompliance().SetValue(UpdatedCompliance_mL_Per_mmHg, VolumePerPressureUnit::mL_Per_mmHg);
    }

    MetabolicToneResponse();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Update Pulmonary Shunt Resistance 
  ///
  /// \details
  /// This decreases the pulmonary shunt resistance in the cardiovascular system.  The resistance is 
  /// inversely proportional to the severity.  The shunt allows deoxgenated blood to pass without
  /// participating in gas exchange.  This often occurs as blood pressure falls.
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::UpdatePulmonaryCapillaries()
  {
    double baseModifier = 1;
    double shuntModifier = 1;
    double coverageModifier = 1;

    double meanArterialPressure_mmHg = m_data.GetCardiovascular().GetMeanArterialPressure(PressureUnit::mmHg);
    double standardPulmonaryCapillaryCoverage = m_data.GetConfiguration().GetStandardPulmonaryCapillaryCoverage();

    // Set a range between 10% below MAP baseline and Cardiovascular Collapse MAP
    double shuntEffect_mmHg = m_StabilizedMeanArterialPressureBaseline_mmHg * 0.9;
    if (meanArterialPressure_mmHg < m_MAPCollapse_mmHg)
      meanArterialPressure_mmHg = m_MAPCollapse_mmHg;
    if (meanArterialPressure_mmHg > shuntEffect_mmHg)
    {
      GetPulmonaryCapillariesCoverageFraction().SetValue(standardPulmonaryCapillaryCoverage);
    }
    else
    {
      baseModifier = (meanArterialPressure_mmHg - m_MAPCollapse_mmHg) /
        (shuntEffect_mmHg - m_MAPCollapse_mmHg);
      //modifier = MAX(0.02, modifier);
      //modifier = MIN(modifier, 1.0);
      coverageModifier = GeneralMath::ExponentialGrowthFunction(10, 0.75, 1.0, baseModifier);

      // Update the capillary coverage for tissue diffusion
      GetPulmonaryCapillariesCoverageFraction().SetValue(standardPulmonaryCapillaryCoverage * coverageModifier);

      shuntModifier = GeneralMath::ExponentialGrowthFunction(10, 0.5, 1.0, baseModifier);

      for (auto leftShuntPath : m_LeftPulmonaryArteriesToVeins)
      {
        // Update the pulmonary shunt
        double leftPulmonaryShuntResistance = leftShuntPath->GetNextResistance().GetValue(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
        leftPulmonaryShuntResistance *= shuntModifier;
        leftShuntPath->GetNextResistance().SetValue(leftPulmonaryShuntResistance, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
      }
      for (auto rightShuntPath : m_RightPulmonaryArteriesToVeins)
      {
        // Update the pulmonary shunt
        double rightPulmonaryShuntResistance = rightShuntPath->GetNextResistance().GetValue(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
        rightPulmonaryShuntResistance *= shuntModifier;
        rightShuntPath->GetNextResistance().SetValue(rightPulmonaryShuntResistance, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
      }

      for (auto leftPulmonaryCapillaryPath : m_LeftPulmonaryArteriesToCapillaries)
      {
        double leftPulmonaryCapillariesResistance = leftPulmonaryCapillaryPath->GetNextResistance().GetValue(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
        leftPulmonaryCapillariesResistance *= 1 / shuntModifier;
        leftPulmonaryCapillaryPath->GetNextResistance().SetValue(leftPulmonaryCapillariesResistance, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
      }
      for (auto rightPulmonaryCapillaryPath : m_RightPulmonaryArteriesToCapillaries)
      {
        double rightPulmonaryCapillariesResistance = rightPulmonaryCapillaryPath->GetNextResistance().GetValue(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
        rightPulmonaryCapillariesResistance *= 1 / shuntModifier;
        rightPulmonaryCapillaryPath->GetNextResistance().SetValue(rightPulmonaryCapillariesResistance, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
      }
    }

    //m_data.GetDataTrack().Probe("baseModifier", baseModifier);
    //m_data.GetDataTrack().Probe("coverageModifier", coverageModifier);
    //m_data.GetDataTrack().Probe("shuntModifier", shuntModifier);
    //m_data.GetDataTrack().Probe("shuntModifierI", 1/shuntModifier);
    //m_data.GetDataTrack().Probe("LeftPulmonaryArteriesToVeinsResistance", leftPulmonaryShuntResistance);
    //m_data.GetDataTrack().Probe("RightPulmonaryArteriesToVeinsResistance", rightPulmonaryShuntResistance);
    //m_data.GetDataTrack().Probe("LeftPulmonaryArteriesToCapillariesResistance", leftPulmonaryCapillariesResistance);
    //m_data.GetDataTrack().Probe("RightPulmonaryArteriesToCapillariesResistance", rightPulmonaryCapillariesResistance);
    //m_data.GetDataTrack().Probe("VenaCavaToRightHeartResistance", m_RightHeartResistance->GetNextResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL));
    //m_data.GetDataTrack().Probe("VenaCavaCompliance", m_VenaCavaCompliance->GetNextCapacitance().GetValue(VolumePerPressureUnit::mL_Per_mmHg));
    //m_data.GetDataTrack().Probe("AortaCompliance", m_AortaCompliance->GetNextCapacitance().GetValue(VolumePerPressureUnit::mL_Per_mmHg));
    //m_data.GetDataTrack().Probe("LeftPulmonaryArteriesCompliance", m_LeftPulmonaryArteriesCompliance->GetNextCapacitance().GetValue(VolumePerPressureUnit::mL_Per_mmHg));
    //m_data.GetDataTrack().Probe("RightPulmonaryArteriesCompliance", m_RightPulmonaryArteriesCompliance->GetNextCapacitance().GetValue(VolumePerPressureUnit::mL_Per_mmHg));
  }

  //--------------------------------------------------------------------------------------------------
/// \brief
/// Hemothorax
///
/// \details
/// Model both traumatic and nontraumatic hemothorax effects of the cardiovascular system. Optionally set
/// a severity or specify a blood flow out of the pulmonary capillaries. The blood out of the cardiovascular
/// system is added to the pleural space volume in the respiratory system. Hermorrhage effects are handled 
/// elsewhere in the cardiovascular model.
//--------------------------------------------------------------------------------------------------
  void CardiovascularModel::CalculateHemothorax()
  {
    bool stateChange = false;

    // Left ----------------------------------------------------------------------------------------

    double leftBloodFlow_mL_Per_s = 0.0;
    if (m_data.GetActions().GetPatientActions().HasLeftHemothorax())
    {
      SEHemothorax& leftHemothorax = m_data.GetActions().GetPatientActions().GetLeftHemothorax();

      //Blood going out
      if (leftHemothorax.HasSeverity())
      {
        if (leftHemothorax.GetSeverity().IsZero())
          m_data.GetActions().GetPatientActions().RemoveLeftHemothorax();
        else
        {
          double severity = leftHemothorax.GetSeverity().GetValue();
          //Piecewise linear
          //Massive hemothorax, often defined as greater than .2 liters/hr \cite kim2020chest
          double leftBloodFlow_L_Per_hr = 0.0;
          if (severity > 0.9)
          {
            //Massive
            leftBloodFlow_L_Per_hr = GeneralMath::LinearInterpolator(0.9, 1.0, 0.2, 1.0, severity);
          }
          else if (severity > 0.6)
          {
            //Medium
            leftBloodFlow_L_Per_hr = GeneralMath::LinearInterpolator(0.6, 0.9, 0.1, 0.2, severity);
          }
          else if (severity > 0.3)
          {
            //Minimal
            leftBloodFlow_L_Per_hr = GeneralMath::LinearInterpolator(0.3, 0.6, 0.05, 0.1, severity);
          }
          else
          {
            leftBloodFlow_L_Per_hr = GeneralMath::LinearInterpolator(0.0, 0.3, 0.0, 0.05, severity);
          }
          leftBloodFlow_mL_Per_s = Convert(leftBloodFlow_L_Per_hr, VolumePerTimeUnit::L_Per_hr, VolumePerTimeUnit::mL_Per_s);
          //leftHemothorax.GetFlowRate().SetValue(leftBloodFlow_L_Per_s, VolumePerTimeUnit::L_Per_s);
        }
      }
      else if (leftHemothorax.HasFlowRate())
      {
        if (leftHemothorax.GetFlowRate().IsZero())
          m_data.GetActions().GetPatientActions().RemoveLeftHemothorax();
        else
        {
          leftBloodFlow_mL_Per_s = leftHemothorax.GetFlowRate(VolumePerTimeUnit::mL_Per_s);
          if (leftBloodFlow_mL_Per_s > 1)
          {
            Error("Hemothorax flow rate is above maximum allowable value. Overriding to 1 mL/s.");
            leftBloodFlow_mL_Per_s = 1;
            leftHemothorax.GetFlowRate().SetValue(leftBloodFlow_mL_Per_s, VolumePerTimeUnit::mL_Per_s);
          }
        }
      }
      else
      {
        Fatal("Invalid hemothorax. The flow rate or severity must be set.");
      }
    }

    if (leftBloodFlow_mL_Per_s != 0.0)
    {
      if (m_LeftPulmonaryVeinsLeak == nullptr)
      {
        //Create the left chest leak in the circuit
        m_LeftPulmonaryVeinsLeak = &m_CirculatoryCircuit->CreatePath(*m_LeftPulmonaryVeinsNode, *m_GroundNode, pulse::CardiovascularPath::LeftPulmonaryVeinsLeak1ToGround);
        m_LeftPulmonaryVeinsLeak->GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::L_Per_s);
        //Create the left chest leak in the graph
        SELiquidCompartmentLink& vLeftPulmonaryVeinsLeak = m_data.GetCompartments().CreateLiquidLink(*m_LeftPulmonaryVeins, *m_Ground, pulse::VascularLink::LeftPulmonaryVeinsLeak);
        vLeftPulmonaryVeinsLeak.MapPath(*m_LeftPulmonaryVeinsLeak);
        m_CirculatoryGraph->AddLink(vLeftPulmonaryVeinsLeak);
        stateChange = true;
      }

      m_LeftPulmonaryVeinsLeak->GetNextFlowSource().SetValue(leftBloodFlow_mL_Per_s, VolumePerTimeUnit::mL_Per_s);
      if (m_data.GetActions().GetPatientActions().GetLeftHemothorax().HasSeverity())// Track the flow for end user
        m_data.GetActions().GetPatientActions().GetLeftHemothorax().GetFlowRate().SetValue(leftBloodFlow_mL_Per_s, VolumePerTimeUnit::mL_Per_s);
    }
    else if (m_LeftPulmonaryVeinsLeak != nullptr)
    {
      //Remove the left chest leak in the circuit and graph
      m_CirculatoryCircuit->RemovePath(pulse::CardiovascularPath::LeftPulmonaryVeinsLeak1ToGround);
      m_CirculatoryGraph->RemoveLink(pulse::VascularLink::LeftPulmonaryVeinsLeak);
      m_LeftPulmonaryVeinsLeak = nullptr;
      stateChange = true;
    }

    // Right ---------------------------------------------------------------------------------------

    double rightBloodFlow_mL_Per_s = 0.0;
    if (m_data.GetActions().GetPatientActions().HasRightHemothorax())
    {
      SEHemothorax& rightHemothorax = m_data.GetActions().GetPatientActions().GetRightHemothorax();
      //Blood going out
      if (rightHemothorax.HasSeverity())
      {
        if (rightHemothorax.GetSeverity().IsZero())
          m_data.GetActions().GetPatientActions().RemoveRightHemothorax();
        else
        {
          double severity = rightHemothorax.GetSeverity().GetValue();
          //Piecewise linear
          //Massive hemothorax, often defined as greater than .2 liters/hr \cite kim2020chest
          double rightBloodFlow_L_Per_hr = 0.0;
          if (severity > 0.9)
          {
            //Massive
            rightBloodFlow_L_Per_hr = GeneralMath::LinearInterpolator(0.9, 1.0, 0.2, 1.0, severity);
          }
          else if (severity > 0.6)
          {
            //Medium
            rightBloodFlow_L_Per_hr = GeneralMath::LinearInterpolator(0.6, 0.9, 0.1, 0.2, severity);
          }
          else if (severity > 0.3)
          {
            //Minimal
            rightBloodFlow_L_Per_hr = GeneralMath::LinearInterpolator(0.3, 0.6, 0.05, 0.1, severity);
          }
          else
          {
            rightBloodFlow_L_Per_hr = GeneralMath::LinearInterpolator(0.0, 0.3, 0.0, 0.05, severity);
          }
          rightBloodFlow_mL_Per_s = Convert(rightBloodFlow_L_Per_hr, VolumePerTimeUnit::L_Per_hr, VolumePerTimeUnit::mL_Per_s);
          //rightHemothorax.GetFlowRate().SetValue(rightBloodFlow_L_Per_s, VolumePerTimeUnit::L_Per_s);
        }
      }
      else if (rightHemothorax.HasFlowRate())
      {
        if (rightHemothorax.GetFlowRate().IsZero())
          m_data.GetActions().GetPatientActions().RemoveRightHemothorax();
        else
        {
          rightBloodFlow_mL_Per_s = rightHemothorax.GetFlowRate(VolumePerTimeUnit::mL_Per_s);
          if (rightBloodFlow_mL_Per_s > 1)
          {
            Error("Hemothorax flow rate is above maximum allowable value. Overriding to 1 mL/s.");
            rightBloodFlow_mL_Per_s = 1;
            rightHemothorax.GetFlowRate().SetValue(rightBloodFlow_mL_Per_s, VolumePerTimeUnit::mL_Per_s);
          }
        }
      }
      else
      {
        Fatal("Invalid hemothorax. The flow rate or severity must be set.");
      }
    }

    if (rightBloodFlow_mL_Per_s != 0.0)
    {
      if (m_RightPulmonaryVeinsLeak == nullptr)
      {
        //Create the right chest leak in the circuit
        m_RightPulmonaryVeinsLeak = &m_CirculatoryCircuit->CreatePath(*m_RightPulmonaryVeinsNode, *m_GroundNode, pulse::CardiovascularPath::RightPulmonaryVeinsLeak1ToGround);
        m_RightPulmonaryVeinsLeak->GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::L_Per_s);
        //Create the right chest leak in the graph
        SELiquidCompartmentLink& vRightPulmonaryVeinsLeak = m_data.GetCompartments().CreateLiquidLink(*m_RightPulmonaryVeins, *m_Ground, pulse::VascularLink::RightPulmonaryVeinsLeak);
        vRightPulmonaryVeinsLeak.MapPath(*m_RightPulmonaryVeinsLeak);
        m_CirculatoryGraph->AddLink(vRightPulmonaryVeinsLeak);
        stateChange = true;
      }

      m_RightPulmonaryVeinsLeak->GetNextFlowSource().SetValue(rightBloodFlow_mL_Per_s, VolumePerTimeUnit::mL_Per_s);
      if (m_data.GetActions().GetPatientActions().GetRightHemothorax().HasSeverity())// Track the flow for end user
        m_data.GetActions().GetPatientActions().GetRightHemothorax().GetFlowRate().SetValue(rightBloodFlow_mL_Per_s, VolumePerTimeUnit::mL_Per_s);
    }
    else if (m_RightPulmonaryVeinsLeak != nullptr)
    {
      //Remove the right chest leak in the circuit and graph
      m_CirculatoryCircuit->RemovePath(pulse::CardiovascularPath::RightPulmonaryVeinsLeak1ToGround);
      m_CirculatoryGraph->RemoveLink(pulse::VascularLink::RightPulmonaryVeinsLeak);
      m_RightPulmonaryVeinsLeak = nullptr;
      stateChange = true;
    }

    // Update circuit and graph --------------------------------------------------------------------

    if (stateChange)
    {
      m_data.GetCompartments().StateChange();
      m_CirculatoryCircuit->StateChange();
      m_CirculatoryGraph->StateChange();
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Increased pleural cavity pressures hinders venous return through increased resistance.
  ///
  /// \details
  /// When a lung collapses (as with pneumothorax), increased pleural cavity pressure pushes on the 
  /// mediastinum and great veins. As an effect, the mediastinum is displaced and the great veins 
  /// become kinked, leading to decreased venous return to the heart. This leads to increasing cardiac
  /// and respiratory embarrasment. @cite jain2008understanding
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::CalculatePleuralCavityVenousEffects()
  {
    if (!m_data.HasRespiratory())
      return;

    //-----------------------------------------------------------------------------------------------------

    //Lung volume/pressure has a direct effect on cardiac output ///\cite verhoeff2017cardiopulmonary
    //Decreased venous return occurs in disease states and with mechanical ventilation through an increased PEEP ///\cite luecke2005clinical

    //Update pulmonary vascular resistance due to increased intrathoracic pressure as with positive pressure ventilation
    //Get the current pleural cavity pressure (reletive to ambient)
    double baselineIntrapleuralPressure_cmH2O = -5.0; /// \cite Levitzky2013pulmonary
    double pleuralCavityPressureBaselineDiff_cmH2O = m_PleuralCavity->GetPressure(PressureUnit::cmH2O) - m_Ambient->GetPressure(PressureUnit::cmH2O) - baselineIntrapleuralPressure_cmH2O;

    //Increase resistance if it's above healthy PEEP
    //Healthy PEEP is always zero and pleural cavity pressure is negative during inhalation
    if (pleuralCavityPressureBaselineDiff_cmH2O > 0.0)
    {
      double maxPressureDiff_cmH2O = 10.0;
      double maxResistanceMultiplier = 10.0;
      pleuralCavityPressureBaselineDiff_cmH2O = MIN(pleuralCavityPressureBaselineDiff_cmH2O, maxPressureDiff_cmH2O);

      //Interpolate into a parabola to effect things much more at larger differences
      double factor = pleuralCavityPressureBaselineDiff_cmH2O / maxPressureDiff_cmH2O;
      double resistanceMultiplier = GeneralMath::LinearInterpolator(0.0, 1.0, 1.0, maxResistanceMultiplier, factor);

      //Use this resistance because it is after the shunt
      double rightPulmonaryVenousReturnResistance_mmHg_s_Per_mL = m_RightPulmonaryVenousReturnResistancePath->GetNextResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
      double leftPulmonaryVenousReturnResistance_mmHg_s_Per_mL = m_LeftPulmonaryVenousReturnResistancePath->GetNextResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL);

      rightPulmonaryVenousReturnResistance_mmHg_s_Per_mL *= resistanceMultiplier;
      leftPulmonaryVenousReturnResistance_mmHg_s_Per_mL *= resistanceMultiplier;

      m_RightPulmonaryVenousReturnResistancePath->GetNextResistance().SetValue(rightPulmonaryVenousReturnResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
      m_LeftPulmonaryVenousReturnResistancePath->GetNextResistance().SetValue(leftPulmonaryVenousReturnResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    }

    //-----------------------------------------------------------------------------------------------------

    //Pressure difference causes a mediastinum shift, which also effects the venous return
    //The left and right pleural pressures are likely to have large differences only due to a pneumothorax/hemothorax
    double pleuralCavityPressureDiff_cmH2O = std::abs(m_LeftPleuralCavity->GetPressure(PressureUnit::cmH2O) - m_RightPleuralCavity->GetPressure(PressureUnit::cmH2O));

    double maxPressureDiff_cmH2O = 10.0;
    double maxResistanceMultiplier = 6.0;
    pleuralCavityPressureDiff_cmH2O = MIN(pleuralCavityPressureDiff_cmH2O, maxPressureDiff_cmH2O);

    double factor = pleuralCavityPressureDiff_cmH2O / maxPressureDiff_cmH2O;
    double resistanceMultiplier = GeneralMath::LinearInterpolator(0.0, 1.0, 1.0, maxResistanceMultiplier, factor);

    double rightHeartResistance_mmHg_s_Per_mL = m_RightHeartResistancePath->GetNextResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    rightHeartResistance_mmHg_s_Per_mL *= resistanceMultiplier;

    //Dampen the change to prevent potential craziness
    //It will only change a fraction as much as it wants to each time step to ensure it's critically damped and doesn't overshoot
    double dampenFraction_perSec = 0.001 * 50.0;
    double previousRightHeartResistance_mmHg_s_Per_mL = m_RightHeartResistancePath->GetResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    rightHeartResistance_mmHg_s_Per_mL = GeneralMath::Damper(rightHeartResistance_mmHg_s_Per_mL, previousRightHeartResistance_mmHg_s_Per_mL, dampenFraction_perSec, m_data.GetTimeStep_s());

    m_RightHeartResistancePath->GetNextResistance().SetValue(rightHeartResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Tune the cardiovascular circuit during stabilization
  ///
  /// \details
  /// Tunes the resistors and capacitors associated with tissue compartments during stabilization to achieve the requested patient parameters
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::TuneCircuit()
  {
    if (m_data.GetConfiguration().TuneCardiovascularCircuit()==eSwitch::Off)
    {
      Info("Not tuning cardiovascular circuit");
      return;
    }

    DataTrack     circuitTrk;
    std::ofstream circuitFile;

    bool success = false;
    double systolicTarget_mmHg = m_data.GetCurrentPatient().GetSystolicArterialPressureBaseline(PressureUnit::mmHg);
    double diastolicTarget_mmHg = m_data.GetCurrentPatient().GetDiastolicArterialPressureBaseline(PressureUnit::mmHg);
    double heartRateTarget_bpm = m_data.GetCurrentPatient().GetHeartRateBaseline(FrequencyUnit::Per_min);

    m_ss << "Tuning to patient parameters : HeartRate(bpm):" << heartRateTarget_bpm << " Systolic(mmHg):" << systolicTarget_mmHg << " Diastolic(mmHg):" << diastolicTarget_mmHg;
    Info(m_ss);

    // Tuning variables
    double pressuretolerance = 0.01;
    double stabPercentTolerance = 0.25;
    double stabCheckTime_s = 15.0;
    double srGain = 0.01; //Systemic is sensitive
    double acGain1 = 0.06;//Gains are empirical
    double acGain2 = 0.02;//Gains are empirical
    double vrGain = 0.06; //Gains are empirical

    bool   stable;
    double map_mmHg = 0, tgt_map_mmHg = 0;
    double systolic_mmHg = 0, tgt_systolic_mmHg = 0;
    double diastolic_mmHg = 0, tgt_diastolic_mmHg = 0;
    double cardiacOutput_mL_Per_min = 0, tgt_cardiacOutput_mL_Per_min = 0;
    double meanCVP_mmHg = 0;
    double blood_mL = 0, tgt_blood_mL = 0;

    double time_s = 0;
    double stableTime_s;
    double currentStableTime_s;
    double maxStableTime_s = 800;
    double maxConfigurations = 50;
    for (int i = 0; i < maxConfigurations; i++)
    {
      stable = false;
      stableTime_s = 0;
      currentStableTime_s = 0;
      while (!stable)
      {
        m_data.AdvanceCallback(-1);
        HeartDriver();
        m_CircuitCalculator->Process(*m_CirculatoryCircuit, m_data.GetTimeStep_s());
        CalculateVitalSigns();
        m_CircuitCalculator->PostProcess(*m_CirculatoryCircuit);
        //return; //Skip stabelization for debugging

        map_mmHg = GetMeanArterialPressure(PressureUnit::mmHg);
        systolic_mmHg = GetSystolicArterialPressure(PressureUnit::mmHg);
        diastolic_mmHg = GetDiastolicArterialPressure(PressureUnit::mmHg);
        cardiacOutput_mL_Per_min = GetCardiacOutput(VolumePerTimeUnit::mL_Per_min);
        meanCVP_mmHg = GetMeanCentralVenousPressure(PressureUnit::mmHg);
        blood_mL = GetBloodVolume(VolumeUnit::mL);

        stableTime_s += m_data.GetTimeStep_s();
        currentStableTime_s += m_data.GetTimeStep_s();
        // Check MAP stability
        if (GeneralMath::PercentDifference(tgt_map_mmHg, map_mmHg) > stabPercentTolerance)
        {
          stableTime_s = 0; tgt_map_mmHg = map_mmHg;
        }
        // Check Systolic stability
        if (GeneralMath::PercentDifference(tgt_systolic_mmHg, systolic_mmHg) > stabPercentTolerance)
        {
          stableTime_s = 0; tgt_systolic_mmHg = systolic_mmHg;
        }
        // Check Diastolic stability
        if (GeneralMath::PercentDifference(tgt_diastolic_mmHg, diastolic_mmHg) > stabPercentTolerance)
        {
          stableTime_s = 0; tgt_diastolic_mmHg = diastolic_mmHg;
        }
        // Check CO stability
        if (GeneralMath::PercentDifference(tgt_cardiacOutput_mL_Per_min, cardiacOutput_mL_Per_min) > stabPercentTolerance)
        {
          stableTime_s = 0; tgt_cardiacOutput_mL_Per_min = cardiacOutput_mL_Per_min;
        }
        //bool stableMeanCVP = true;
        //if (GeneralMath::PercentDifference(tgt_meanCVP_mmHg, meanCVP_mmHg) > 0.25)
        //  { stableTime_s = 0; tgt_meanCVP_mmHg = meanCVP_mmHg; stableMeanCVP = false; }
        // Check Blood Vol stability
        if (GeneralMath::PercentDifference(tgt_blood_mL, blood_mL) > stabPercentTolerance)
        {
          stableTime_s = 0; tgt_blood_mL = blood_mL;
        }

        if (stableTime_s > stabCheckTime_s)
        {
          stable = true;
          m_ss << "We are stable at " << time_s;
          Info(m_ss);
        }
        if (currentStableTime_s > maxStableTime_s)
        {
          Error("Could not stabilize this configuration");
          break;
        }

        if (!m_data.GetConfiguration().CardiovascularTuningFile().empty())
        {
          circuitTrk.Track(time_s, *m_CirculatoryCircuit);
          circuitTrk.Track("MAP_mmHg", time_s, map_mmHg);
          circuitTrk.Track("Systolic_mmHg", time_s, systolic_mmHg);
          circuitTrk.Track("Diastolilc_mmHg", time_s, diastolic_mmHg);
          circuitTrk.Track("MeanCVP_mmHg", time_s, meanCVP_mmHg);
          circuitTrk.Track("CardiacOutput_mL_per_s", time_s, cardiacOutput_mL_Per_min);
          circuitTrk.Track("BloodVolume_mL", time_s, blood_mL);

          if (time_s == 0)
            circuitTrk.CreateFile(m_data.GetConfiguration().CardiovascularTuningFile().c_str(), circuitFile);
          circuitTrk.StreamTrackToFile(circuitFile);
        }
        time_s += m_data.GetTimeStep_s();
      }

      double systolicError_mmHg = systolicTarget_mmHg - systolic_mmHg;
      double diastolicError_mmHg = diastolicTarget_mmHg - diastolic_mmHg;
      if (stable)
      {
        // Compute the pressure errors
        if (std::abs(systolicError_mmHg / systolicTarget_mmHg) < pressuretolerance && std::abs(diastolicError_mmHg / diastolicTarget_mmHg) < pressuretolerance) //relative error check
        {
          success = true;
          break; // We met our patient parameters
        }
      }

      // This configuration did not meet the requests patient baselines, Tune the paths
      double systemicResistanceScale = 1;
      double systemicComplianceScale = 1;
      double aortaResistanceScale = 1;
      double aortaComplianceScale = 1;
      double rightHeartResistanceScale = 1;
      double venaCavaComplianceScale = 1;
      if ((systolicError_mmHg > 0 && diastolicError_mmHg > 0) || (systolicError_mmHg < 0 && diastolicError_mmHg < 0)) // Same direction
      {
        if (cardiacOutput_mL_Per_min > 4000.0)
        {
          systemicResistanceScale += srGain * (systolicError_mmHg + diastolicError_mmHg);
          BLIM(systemicResistanceScale, 0.5, 1.5);
        }
        else
        {
          rightHeartResistanceScale -= vrGain * (systolicError_mmHg + diastolicError_mmHg);
          systemicResistanceScale += srGain * (systolicError_mmHg + diastolicError_mmHg);
          BLIM(rightHeartResistanceScale, 0.5, 1.5);
          BLIM(systemicResistanceScale, 0.5, 1.5);
          Info("Using vena cava resistance gain.");
        }
      }
      else if ((systolicError_mmHg > 0 && diastolicError_mmHg < 0) || (systolicError_mmHg < 0 && diastolicError_mmHg > 0)) // Opposite directions
      {
        // Widen or narrow based on both errors
        if (meanCVP_mmHg < 2.0 || meanCVP_mmHg > 6.0) // Go ahead and do it but warn the user that CVP is out of normal range.
          Warning("We're out of CVP range");
        aortaComplianceScale -= acGain1 * systolicError_mmHg - acGain2 * diastolicError_mmHg;
        BLIM(aortaComplianceScale, 0.5, 1.5);
      }
      else
      {
        // This means that both errors are zero, which means you shouldn't have called me
        Error("Something is wrong with tuning logic");
      }
      m_ss << "Tuning paths, Current values : HeartRate(bpm):" << GetHeartRate(FrequencyUnit::Per_min) <<
        " Systolic(mmHg):" << systolic_mmHg <<
        " Diastolic(mmHg):" << diastolic_mmHg <<
        " Cardiac Output(mL/min):" << cardiacOutput_mL_Per_min <<
        " Mean CVP(mmHg):" << meanCVP_mmHg <<
        " MAP(mmHg):" << GetMeanArterialPressure(PressureUnit::mmHg) <<
        " BloodVolume(mL): " << blood_mL;
      Info(m_ss);
      TunePaths(systemicResistanceScale, systemicComplianceScale, aortaResistanceScale, aortaComplianceScale, rightHeartResistanceScale, venaCavaComplianceScale);
      m_data.GetCurrentPatient().GetHeartRateBaseline().Set(GetHeartRate());
      m_data.GetCurrentPatient().GetDiastolicArterialPressureBaseline().SetValue(diastolic_mmHg, PressureUnit::mmHg);
      m_data.GetCurrentPatient().GetSystolicArterialPressureBaseline().SetValue(systolic_mmHg, PressureUnit::mmHg);
      m_data.GetCurrentPatient().GetMeanArterialPressureBaseline().Set(GetMeanArterialPressure());
    }
    // Set the Tissue Pressure to the CV Pressures
    // Loop to decrement the Tissue Resistance, check that the flow is under 5mL/min

    if (!success)
    {
      m_ss << "Unable to tune circuit to desired patient parameters. Final values : HeartRate(bpm):" << GetHeartRate(FrequencyUnit::Per_min) <<
        " Systolic(mmHg):" << GetSystolicArterialPressure(PressureUnit::mmHg) <<
        " Diastolic(mmHg):" << GetDiastolicArterialPressure(PressureUnit::mmHg) <<
        " Cardiac Output(mL/min):" << GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) <<
        " Mean CVP(mmHg):" << GetMeanCentralVenousPressure(PressureUnit::mmHg) <<
        " MAP(mmHg):" << GetMeanArterialPressure(PressureUnit::mmHg) <<
        " BloodVolume(mL): " << GetBloodVolume(VolumeUnit::mL);
      Fatal(m_ss);
    }
    else
    {
      m_ss << "Successfully tuned circuit (No Tissues). Final values : HeartRate(bpm):" << GetHeartRate(FrequencyUnit::Per_min) <<
        " Systolic(mmHg):" << GetSystolicArterialPressure(PressureUnit::mmHg) <<
        " Diastolic(mmHg):" << GetDiastolicArterialPressure(PressureUnit::mmHg) <<
        " Cardiac Output(mL/min):" << GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) <<
        " Mean CVP(mmHg):" << GetMeanCentralVenousPressure(PressureUnit::mmHg) <<
        " MAP(mmHg):" << GetMeanArterialPressure(PressureUnit::mmHg) <<
        " BloodVolume(mL): " << GetBloodVolume(VolumeUnit::mL);
      Info(m_ss);
      m_data.GetCurrentPatient().GetHeartRateBaseline().Set(GetHeartRate());
      m_data.GetCurrentPatient().GetDiastolicArterialPressureBaseline().Set(GetDiastolicArterialPressure());
      m_data.GetCurrentPatient().GetSystolicArterialPressureBaseline().Set(GetSystolicArterialPressure());
      m_data.GetCurrentPatient().GetMeanArterialPressureBaseline().Set(GetMeanArterialPressure());

      // Now tune the tissue nodes
      if (m_data.GetConfiguration().IsTissueEnabled())
      {
        TuneTissue(time_s, circuitTrk, circuitFile);
        m_ss << "Successfully tuned circuit with tissue resistances. Final values : HeartRate(bpm):" << GetHeartRate(FrequencyUnit::Per_min) <<
          " Systolic(mmHg):" << GetSystolicArterialPressure(PressureUnit::mmHg) <<
          " Diastolic(mmHg):" << GetDiastolicArterialPressure(PressureUnit::mmHg) <<
          " Cardiac Output(mL/min):" << GetCardiacOutput(VolumePerTimeUnit::mL_Per_min) <<
          " Mean CVP(mmHg):" << GetMeanCentralVenousPressure(PressureUnit::mmHg) <<
          " MAP(mmHg):" << GetMeanArterialPressure(PressureUnit::mmHg) <<
          " BloodVolume(mL): " << GetBloodVolume(VolumeUnit::mL);
        Info(m_ss);
        m_data.GetCurrentPatient().GetHeartRateBaseline().Set(GetHeartRate());
        m_data.GetCurrentPatient().GetDiastolicArterialPressureBaseline().Set(GetDiastolicArterialPressure());
        m_data.GetCurrentPatient().GetSystolicArterialPressureBaseline().Set(GetSystolicArterialPressure());
        m_data.GetCurrentPatient().GetMeanArterialPressureBaseline().Set(GetMeanArterialPressure());
      }
      // Reset our substance masses to the new volumes
      for (SELiquidCompartment* c : m_data.GetCompartments().GetVascularLeafCompartments())
      {
        if (!c->HasVolume())
          continue;
        c->Balance(BalanceLiquidBy::Concentration);
        if (m_CirculatoryGraph->GetCompartment(c->GetName()) == nullptr)
          Info("Cardiovascular Graph does not have cmpt " + c->GetName());
        if (c->HasSubstanceQuantity(m_data.GetSubstances().GetHb()))// Unit testing does not have any Hb
          m_data.GetSaturationCalculator().CalculateBloodGasDistribution(*c);//so don't do this if we don't have Hb
      }
      for (SELiquidCompartment* c : m_data.GetCompartments().GetUrineLeafCompartments())
      {
        if (!c->HasVolume())
          continue;
        c->Balance(BalanceLiquidBy::Concentration);
      }
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Method that changes path parameters during circuit tuning.
  ///
  /// \details
  /// Method that changes path parameters during circuit tuning.
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::TunePaths(double systemicResistanceScale, double systemicComplianceScale, double aortaResistanceScale, double aortaComplianceScale, double rightHeartResistanceScale, double venaCavaComplianceScale)
  {
    double sp1_mmHg_s_Per_mL;
    if (systemicComplianceScale != 1.0)
    {
      for (SEFluidCircuitPath* p : m_SystemicCompliancePaths)
      {
        sp1_mmHg_s_Per_mL = p->GetCapacitanceBaseline().GetValue(VolumePerPressureUnit::mL_Per_mmHg) * systemicComplianceScale;
        p->GetCapacitanceBaseline().SetValue(sp1_mmHg_s_Per_mL, VolumePerPressureUnit::mL_Per_mmHg);
        p->GetCapacitance().SetValue(sp1_mmHg_s_Per_mL, VolumePerPressureUnit::mL_Per_mmHg);
        p->GetNextCapacitance().SetValue(sp1_mmHg_s_Per_mL, VolumePerPressureUnit::mL_Per_mmHg);
      }
    }
    if (systemicResistanceScale != 1.0)
    {
      for (SEFluidCircuitPath* p : m_SystemicResistancePaths)
      {
        sp1_mmHg_s_Per_mL = p->GetResistanceBaseline().GetValue(PressureTimePerVolumeUnit::mmHg_s_Per_mL) * systemicResistanceScale;

        p->GetResistanceBaseline().SetValue(sp1_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
        p->GetResistance().SetValue(sp1_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
        p->GetNextResistance().SetValue(sp1_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
      }
    }
    if (aortaComplianceScale != 1)
    {
      sp1_mmHg_s_Per_mL = m_AortaCompliancePath->GetCapacitanceBaseline().GetValue(VolumePerPressureUnit::mL_Per_mmHg) * aortaComplianceScale;
      m_AortaCompliancePath->GetCapacitanceBaseline().SetValue(sp1_mmHg_s_Per_mL, VolumePerPressureUnit::mL_Per_mmHg);
      m_AortaCompliancePath->GetCapacitance().SetValue(sp1_mmHg_s_Per_mL, VolumePerPressureUnit::mL_Per_mmHg);
      m_AortaCompliancePath->GetNextCapacitance().SetValue(sp1_mmHg_s_Per_mL, VolumePerPressureUnit::mL_Per_mmHg);
    }
    if (aortaResistanceScale != 1)
    {
      sp1_mmHg_s_Per_mL = m_AortaResistancePath->GetResistanceBaseline().GetValue(PressureTimePerVolumeUnit::mmHg_s_Per_mL) * aortaResistanceScale;
      m_AortaResistancePath->GetResistanceBaseline().SetValue(sp1_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
      m_AortaResistancePath->GetResistance().SetValue(sp1_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
      m_AortaResistancePath->GetNextResistance().SetValue(sp1_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    }
    if (venaCavaComplianceScale != 1)
    {
      sp1_mmHg_s_Per_mL = m_VenaCavaCompliancePath->GetCapacitanceBaseline().GetValue(VolumePerPressureUnit::mL_Per_mmHg) * venaCavaComplianceScale;
      m_VenaCavaCompliancePath->GetCapacitanceBaseline().SetValue(sp1_mmHg_s_Per_mL, VolumePerPressureUnit::mL_Per_mmHg);
      m_VenaCavaCompliancePath->GetCapacitance().SetValue(sp1_mmHg_s_Per_mL, VolumePerPressureUnit::mL_Per_mmHg);
      m_VenaCavaCompliancePath->GetNextCapacitance().SetValue(sp1_mmHg_s_Per_mL, VolumePerPressureUnit::mL_Per_mmHg);
    }
    if (rightHeartResistanceScale != 1)
    {
      sp1_mmHg_s_Per_mL = m_RightHeartResistancePath->GetResistanceBaseline().GetValue(PressureTimePerVolumeUnit::mmHg_s_Per_mL) * rightHeartResistanceScale;
      m_RightHeartResistancePath->GetResistanceBaseline().SetValue(sp1_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
      m_RightHeartResistancePath->GetResistance().SetValue(sp1_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
      m_RightHeartResistancePath->GetNextResistance().SetValue(sp1_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Tune the resistance between the CV circuit and Tissue Circuit during stabilization
  ///
  /// \details
  /// Tunes the resistors to achieve a desired flow between tissue and blood compartments
  //--------------------------------------------------------------------------------------------------
  void CardiovascularModel::TuneTissue(double time_s, DataTrack& circuitTrk, std::ofstream& circuitFile)
  {
    bool stable;
    bool success = false;

    // Copy all the CV pressures to the associated tissue nodes
    std::vector<double> flowMax;
    std::vector<double> flowMin;
    std::vector<SEFluidCircuitPath*> tissueResistancePaths;
    std::vector<SEFluidCircuitNode*> tissuePressureCVSources;

    if (m_data.GetConfiguration().UseComputationalLifeExpansion() == eSwitch::On)
    {
      tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::ComputationalLifeCardiovascularNode::TorsoBone1));
      tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::BoneT2ToBoneT1));

      tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::ComputationalLifeCardiovascularNode::Intracranial2));
      tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::BrainT2ToBrainT1));

      tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::ComputationalLifeCardiovascularNode::TorsoFat1));
      tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::FatT2ToFatT1));

      // Have to pick one of the 3 nodes GI is mapped to for pressure (Could use Large Intestine | Splanchnic)
      tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::CardiovascularNode::SmallIntestine1));
      tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::GutT2ToGutT1));

      tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::CardiovascularNode::Liver1));
      tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::LiverT2ToLiverT1));

      if (m_data.GetConfiguration().UseExpandedKidneys() == eSwitch::On)
      {
        tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::RenalNode::LeftGlomerularCapillaries));
        tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::LeftKidneyT2ToLeftKidneyT1));

        tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::RenalNode::RightGlomerularCapillaries));
        tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::RightKidneyT2ToRightKidneyT1));
      }
      else
      {
        tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::CardiovascularNode::LeftKidney2));
        tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::LeftKidneyT2ToLeftKidneyT1));

        tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::CardiovascularNode::RightKidney2));
        tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::RightKidneyT2ToRightKidneyT1));
      }

      tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::CardiovascularNode::LeftPulmonaryCapillaries1));
      tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::LeftLungT2ToLeftLungT1));

      tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::ComputationalLifeCardiovascularNode::TorsoMuscle1));
      tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::MuscleT2ToMuscleT1));

      tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::CardiovascularNode::Myocardium2));
      tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::MyocardiumT2ToMyocardiumT1));

      tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::CardiovascularNode::RightPulmonaryCapillaries1));
      tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::RightLungT2ToRightLungT1));

      tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::ComputationalLifeCardiovascularNode::TorsoSkin1));
      tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::SkinT2ToSkinT1));

      tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::CardiovascularNode::Spleen1));
      tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::SpleenT2ToSpleenT1));
    }
    else
    {
      tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::CardiovascularNode::Bone1));
      tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::BoneT2ToBoneT1));

      tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::CardiovascularNode::Brain1));
      tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::BrainT2ToBrainT1));

      tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::CardiovascularNode::Fat1));
      tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::FatT2ToFatT1));

      // Have to pick one of the 3 nodes GI is mapped to for pressure (Could use Large Intestine | Splanchnic)
      tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::CardiovascularNode::SmallIntestine1));
      tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::GutT2ToGutT1));

      tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::CardiovascularNode::Liver1));
      tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::LiverT2ToLiverT1));

      if (m_data.GetConfiguration().UseExpandedKidneys()  == eSwitch::On)
      {
        tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::RenalNode::LeftGlomerularCapillaries));
        tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::LeftKidneyT2ToLeftKidneyT1));

        tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::RenalNode::RightGlomerularCapillaries));
        tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::RightKidneyT2ToRightKidneyT1));
      }
      else
      {
        tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::CardiovascularNode::LeftKidney1));
        tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::LeftKidneyT2ToLeftKidneyT1));

        tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::CardiovascularNode::RightKidney1));
        tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::RightKidneyT2ToRightKidneyT1));
      }

      //TODO: Make this work for the expanded model
      if (m_data.GetConfiguration().UseExpandedLungs() == eSwitch::Off)
      {
        tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::CardiovascularNode::LeftPulmonaryCapillaries1));
        tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::LeftLungT2ToLeftLungT1));

        tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::CardiovascularNode::RightPulmonaryCapillaries1));
        tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::RightLungT2ToRightLungT1));
      }

      tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::CardiovascularNode::Muscle1));
      tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::MuscleT2ToMuscleT1));

      tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::CardiovascularNode::Myocardium1));
      tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::MyocardiumT2ToMyocardiumT1));

      tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::CardiovascularNode::Skin1));
      tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::SkinT2ToSkinT1));

      tissuePressureCVSources.push_back(m_data.GetCircuits().GetFluidNode(pulse::CardiovascularNode::Spleen1));
      tissueResistancePaths.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::SpleenT2ToSpleenT1));
    }

    // Get the tissue resistance paths to decrement until we get a flow of at least (TBD)
   
    Info("Tuning Tissue resistances at " + cdm::to_string(time_s) + "s");
    for (SEFluidCircuitPath* path : tissueResistancePaths)
    {
      size_t n = flowMax.size();
      flowMax.push_back(0);
      flowMin.push_back(0);
      path->GetTargetNode().GetPressure().Set(tissuePressureCVSources[n]->GetPressure());
      path->GetTargetNode().GetNextPressure().Set(tissuePressureCVSources[n]->GetNextPressure());
      Info("  Initial " + path->GetName() + " Flow : " + path->GetFlow().ToString());
      Info("  Initial " + path->GetName() + " Resistance : " + path->GetResistance().ToString());
    }

    // Tuning variables unused
    //const double pressuretolerance = 0.01;
    //const double stabPercentTolerance = 0.25;
    //const double stabCheckTime_s = 15.0;

    double currentStableTime_s;
    double maxStableTime_s = 20;
    double maxConfigurations = 50;
    double resistanceScale = 1.0;
    for (int i = 0; i < maxConfigurations; i++)
    {
      stable = false;
      currentStableTime_s = 0;
      for (size_t m = 0; m < flowMax.size(); m++)
      {
        flowMax[m] = -1000;
        flowMin[m] = 1000;
      }

      while (!stable)
      {
        m_data.AdvanceCallback(-1);
        HeartDriver();
        m_CircuitCalculator->Process(*m_CirculatoryCircuit, m_data.GetTimeStep_s());
        CalculateVitalSigns();
        m_CircuitCalculator->PostProcess(*m_CirculatoryCircuit);
        //return; //Skip stabelization for debugging

        time_s += m_data.GetTimeStep_s();
        currentStableTime_s += m_data.GetTimeStep_s();

        for (size_t m = 0; m < tissueResistancePaths.size(); m++)
        {
          SEFluidCircuitPath* path = tissueResistancePaths[m];
          double max = flowMax[m];
          double min = flowMin[m];
          double current = path->GetFlow(VolumePerTimeUnit::mL_Per_s);
          if (current > max)
            flowMax[m] = current;
          if (current < min)
            flowMin[m] = current;
        }

        if (!m_data.GetConfiguration().CardiovascularTuningFile().empty())
        {
          circuitTrk.Track(time_s, *m_CirculatoryCircuit);
          circuitTrk.Track("MAP_mmHg", time_s, GetMeanArterialPressure(PressureUnit::mmHg));
          circuitTrk.Track("Systolic_mmHg", time_s, GetSystolicArterialPressure(PressureUnit::mmHg));
          circuitTrk.Track("Diastolilc_mmHg", time_s, GetDiastolicArterialPressure(PressureUnit::mmHg));
          circuitTrk.Track("MeanCVP_mmHg", time_s, GetMeanCentralVenousPressure(PressureUnit::mmHg));
          circuitTrk.Track("CardiacOutput_mL_per_s", time_s, GetCardiacOutput(VolumePerTimeUnit::mL_Per_s));
          circuitTrk.Track("BloodVolume_mL", time_s, GetBloodVolume(VolumeUnit::mL));
          circuitTrk.StreamTrackToFile(circuitFile);
        }
        if (currentStableTime_s > maxStableTime_s)
          break;
      }

      // This configuration did not meet the requests patient baselines, Reduce resistance on the paths
      size_t pCnt = 0;
      double sp1_mmHg_s_Per_mL;
      resistanceScale = 0.5;
      for (size_t m = 0; m < tissueResistancePaths.size(); m++)
      {
        double flowWindow = fabs(flowMax[m] - flowMin[m]);
        SEFluidCircuitPath* p = tissueResistancePaths[m];
        sp1_mmHg_s_Per_mL = p->GetResistanceBaseline().GetValue(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
        if (flowWindow < 0.00005)// 0.003 mL/min
        {
          pCnt++;
          sp1_mmHg_s_Per_mL = sp1_mmHg_s_Per_mL - (sp1_mmHg_s_Per_mL * resistanceScale);
        }
        if (flowWindow > 0.0005)// 0.03 mL/min
        {
          pCnt++;
          sp1_mmHg_s_Per_mL = sp1_mmHg_s_Per_mL + (sp1_mmHg_s_Per_mL * resistanceScale);
        }
        p->GetResistanceBaseline().SetValue(sp1_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
        p->GetResistance().SetValue(sp1_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
        p->GetNextResistance().SetValue(sp1_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
        //Info("  " + p->GetName() + " Resistance : " + p->GetResistance().ToString());
        //Info("  " + p->GetName() + " Flow : " + p->GetFlow().ToString());
      }
      Info("Tuning " + std::to_string(pCnt) + "/" + std::to_string(tissueResistancePaths.size()) + " paths at " + cdm::to_string(time_s) + "s");

      if (pCnt == 0)
      {
        success = true;
        break; // Tissue resistances are good
      }
    }

    if (!success)
    {
      Fatal("Unable to tune circuit at " + cdm::to_string(time_s) + "s");
    }
    else
    {
      Info("Finalizing CV Circuit...");
      // Let things settle a bit
      double extra_s = time_s + 30;
      while (time_s < extra_s)
      {
        m_data.AdvanceCallback(-1);
        HeartDriver();
        m_CircuitCalculator->Process(*m_CirculatoryCircuit, m_data.GetTimeStep_s());
        CalculateVitalSigns();
        m_CircuitCalculator->PostProcess(*m_CirculatoryCircuit);
        time_s += m_data.GetTimeStep_s();
        if (!m_data.GetConfiguration().CardiovascularTuningFile().empty())
        {
          circuitTrk.Track(time_s, *m_CirculatoryCircuit);
          circuitTrk.Track("MAP_mmHg", time_s, GetMeanArterialPressure(PressureUnit::mmHg));
          circuitTrk.Track("Systolic_mmHg", time_s, GetSystolicArterialPressure(PressureUnit::mmHg));
          circuitTrk.Track("Diastolilc_mmHg", time_s, GetDiastolicArterialPressure(PressureUnit::mmHg));
          circuitTrk.Track("MeanCVP_mmHg", time_s, GetMeanCentralVenousPressure(PressureUnit::mmHg));
          circuitTrk.Track("CardiacOutput_mL_per_s", time_s, GetCardiacOutput(VolumePerTimeUnit::mL_Per_s));
          circuitTrk.Track("BloodVolume_mL", time_s, GetBloodVolume(VolumeUnit::mL));
          circuitTrk.StreamTrackToFile(circuitFile);
        }
      }
      Info("Successfully tuned tissue circuit at " + cdm::to_string(time_s) + "s");
    }
    for (SEFluidCircuitPath* p : tissueResistancePaths)
    {
      Info("  Final " + p->GetName() + " Resistance : " + p->GetResistance().ToString());
      Info("  Final " + p->GetName() + " Flow : " + p->GetFlow().ToString());
    }
  }
END_NAMESPACE
