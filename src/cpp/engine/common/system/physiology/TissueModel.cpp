/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/PulseConfiguration.h"
#include "engine/common/controller/CircuitManager.h"
#include "engine/common/controller/CompartmentManager.h"
#include "engine/common/controller/SubstanceManager.h"
#include "engine/common/system/physiology/Saturation.h"
#include "engine/common/system/physiology/TissueModel.h"

   // Conditions
#include "cdm/engine/SEConditionManager.h"
#include "cdm/patient/conditions/SEConsumeMeal.h"
#include "cdm/patient/conditions/SEDehydration.h"
// Actions
#include "cdm/engine/SEActionManager.h"
#include "cdm/engine/SEPatientActionCollection.h"
#include "cdm/patient/actions/SEExercise.h"
// Dependent Systems
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SEEnergySystem.h"
// CDM
#include "cdm/patient/SEPatient.h"
#include "cdm/patient/SEMeal.h"
#include "cdm/engine/SEEventManager.h"
#include "cdm/patient/conditions/SEConsumeMeal.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceClearance.h"
#include "cdm/substance/SESubstanceCompound.h"
#include "cdm/substance/SESubstancePharmacokinetics.h"
#include "cdm/substance/SESubstanceTissuePharmacokinetics.h"
#include "cdm/circuit/fluid/SEFluidCircuit.h"
#include "cdm/compartment/fluid/SEGasCompartment.h"
#include "cdm/compartment/fluid/SELiquidCompartment.h"
#include "cdm/compartment/tissue/SETissueCompartment.h"
#include "cdm/compartment/substances/SEGasSubstanceQuantity.h"
#include "cdm/compartment/substances/SELiquidSubstanceQuantity.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarAmountPerVolume.h"
#include "cdm/properties/SEScalarAmountPerTime.h"
#include "cdm/properties/SEScalarArea.h"
#include "cdm/properties/SEScalarAreaPerTimePressure.h"
#include "cdm/properties/SEScalarEnergyPerAmount.h"
#include "cdm/properties/SEScalarInversePressure.h"
#include "cdm/properties/SEScalarLength.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerAmount.h"
#include "cdm/properties/SEScalarMassPerTime.h"
#include "cdm/properties/SEScalarMassPerAreaTime.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPower.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTemperature.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalarVolumePerTimePressure.h"
#include "cdm/properties/SEScalarVolumePerTimeMass.h"
#include "cdm/utils/DataTrack.h"
#include "cdm/utils/GeneralMath.h"

namespace pulse
{
  TissueModel::TissueModel(Data& data) : TissueSystem(data.GetLogger()), Model(data)
  {
    Clear();
  }

  TissueModel::~TissueModel()
  {
    Clear();
  }

  void TissueModel::Clear()
  {
    TissueSystem::Clear();
    m_Albumin = nullptr;
    m_Glucose = nullptr;
    m_Tristearin = nullptr;
    m_O2 = nullptr;
    m_CO2 = nullptr;
    m_CO = nullptr;
    m_Lactate = nullptr;
    m_Acetoacetate = nullptr;
    m_Creatinine = nullptr;
    m_Sodium = nullptr;
    m_Calcium = nullptr;
    m_Insulin = nullptr;

    m_LiverAcetoacetate = nullptr;
    m_LiverTissueAlbumin = nullptr;
    m_LeftLungTissue = nullptr;
    m_RightLungTissue = nullptr;
    m_MuscleTissue = nullptr;
    m_LiverTissue = nullptr;
    m_FatTissue = nullptr;

    m_FatVascular = nullptr;
    m_FatVascularLipid = nullptr;
    m_LiverVascularGlucose = nullptr;
    m_MuscleVascular = nullptr;
    m_MuscleVascularGlucose = nullptr;

    m_LeftAlveoli = nullptr;
    m_RightAlveoli = nullptr;
    m_LeftPulmonaryCapillaries = nullptr;
    m_RightPulmonaryCapillaries = nullptr;

    m_ConsumptionProdutionTissues.clear();
    m_CardiacArrestVascularFlows_ml_per_min.clear();
    m_VascularResistances.clear();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Initializes system properties to valid homeostatic values.
  ///
  /// \details
  /// For stabilization only!
  /// Called AFTER Setup when stabilizing a new patient
  //--------------------------------------------------------------------------------------------------
  void TissueModel::Initialize()
  {
    Model::Initialize();

    /*Tissue System*/
    // Get total tissue resting values for substances
    SETissueCompartment* tissue;
    SELiquidCompartment* vascular;
    m_RestingFluidMass_kg = 0.0;
    m_RestingTissueGlucose_g = 0.0;
    for (auto tissueVascular : m_TissueToVascular)
    {
      tissue = tissueVascular.first;
      vascular = tissueVascular.second;
      SELiquidCompartment& extracellular = tissue->GetExtracellular();
      SELiquidCompartment& intracellular = tissue->GetIntracellular();

      m_RestingTissueGlucose_g += extracellular.GetSubstanceQuantity(*m_Glucose)->GetMass(MassUnit::g);
      m_RestingFluidMass_kg += vascular->GetVolume(VolumeUnit::mL) * m_data.GetBloodChemistry().GetBloodDensity(MassPerVolumeUnit::kg_Per_mL);
      m_RestingFluidMass_kg += intracellular.GetVolume(VolumeUnit::mL) * m_data.GetConfiguration().GetWaterDensity(MassPerVolumeUnit::kg_Per_mL);
      m_RestingFluidMass_kg += extracellular.GetVolume(VolumeUnit::mL) * m_data.GetConfiguration().GetWaterDensity(MassPerVolumeUnit::kg_Per_mL);
    }
    m_PreviousFluidMass_kg = m_RestingFluidMass_kg;
    m_RestingBloodGlucose_mg_Per_mL = m_data.GetSubstances().GetGlucose().GetBloodConcentration(MassPerVolumeUnit::mg_Per_mL);
    m_RestingBloodLipid_mg_Per_mL = m_data.GetSubstances().GetTristearin().GetBloodConcentration(MassPerVolumeUnit::mg_Per_mL);
    m_RestingBloodInsulin_mg_Per_mL = m_data.GetSubstances().GetInsulin().GetBloodConcentration(MassPerVolumeUnit::mg_Per_mL);
    GetIntracellularFluidPH().SetValue(7.0);

    /// \cite guyton2006medical
    GetOxygenConsumptionRate().SetValue(250.0, VolumePerTimeUnit::mL_Per_min);
    GetCarbonDioxideProductionRate().SetValue(200.0, VolumePerTimeUnit::mL_Per_min);
    GetRespiratoryExchangeRatio().SetValue(0.8);

    GetExtracellularFluidVolume().SetValue(0.0, VolumeUnit::mL);
    GetIntracellularFluidVolume().SetValue(0.0, VolumeUnit::mL);
    GetExtravascularFluidVolume().SetValue(0.0, VolumeUnit::mL);
    GetTotalFluidVolume().SetValue(0.0, VolumeUnit::mL);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Initializes the tissue specific quantities
  ///
  /// \details
  /// Called during both State loading and Patient Stabilization
  /// This is called before we serialize
  /// Pull and setup up our data (can be from other systems)
  /// Initialize will be called after this and can overwrite any of this data (only if stabilizing)
  //--------------------------------------------------------------------------------------------------
  void TissueModel::SetUp()
  {
    m_Acetoacetate = &m_data.GetSubstances().GetAcetoacetate();
    m_LiverAcetoacetate = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Liver)->GetSubstanceQuantity(*m_Acetoacetate);

    m_AlbuminProdutionRate_g_Per_s = 1.5e-4; /// \cite jarnum1972plasma
    m_Albumin = &m_data.GetSubstances().GetAlbumin();
    m_LiverTissueAlbumin = m_data.GetCompartments().GetLiquidCompartment(pulse::ExtravascularCompartment::LiverExtracellular)->GetSubstanceQuantity(*m_Albumin);
    m_Glucose = &m_data.GetSubstances().GetGlucose();

    m_O2 = &m_data.GetSubstances().GetO2();
    m_CO2 = &m_data.GetSubstances().GetCO2();
    m_CO = &m_data.GetSubstances().GetCO();

    m_Lactate = &m_data.GetSubstances().GetLactate();
    m_Creatinine = &m_data.GetSubstances().GetCreatinine();
    m_Sodium = &m_data.GetSubstances().GetSodium();
    m_Calcium = &m_data.GetSubstances().GetCalcium();
    m_Insulin = &m_data.GetSubstances().GetInsulin();
    m_Tristearin = &m_data.GetSubstances().GetTristearin();

    m_Sweat = m_data.GetSubstances().GetCompound("Sweat");

    m_GutT1 = m_data.GetCircuits().GetActiveCardiovascularCircuit().GetNode(pulse::TissueNode::GutT1);
    m_GutT1ToGutT3 = m_data.GetCircuits().GetActiveCardiovascularCircuit().GetPath(pulse::TissuePath::GutT1ToGutT3);

    m_MuscleVascular = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Muscle);
    m_FatVascular = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Fat);

    m_FatVascularLipid = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Fat)->GetSubstanceQuantity(*m_Tristearin);
    m_LiverVascularGlucose = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Liver)->GetSubstanceQuantity(*m_Glucose);
    m_MuscleVascularGlucose = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Muscle)->GetSubstanceQuantity(*m_Glucose);

    m_LeftLungTissue = m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::LeftLung);
    m_RightLungTissue = m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::RightLung);
    m_LiverTissue = m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::Liver);
    m_FatTissue = m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::Fat);
    m_MuscleTissue = m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::Muscle);

    m_LiverIntracellular = m_data.GetCompartments().GetLiquidCompartment(pulse::ExtravascularCompartment::LiverIntracellular);
    m_FatIntracellular = m_data.GetCompartments().GetLiquidCompartment(pulse::ExtravascularCompartment::FatIntracellular);
    m_MuscleIntracellular = m_data.GetCompartments().GetLiquidCompartment(pulse::ExtravascularCompartment::MuscleIntracellular);

    m_LeftAlveoli = m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::LeftAlveoli);
    m_RightAlveoli = m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::RightAlveoli);
    m_LeftPulmonaryCapillaries = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftPulmonaryCapillaries);
    m_RightPulmonaryCapillaries = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightPulmonaryCapillaries);

    std::string BrainCmpt = m_data.GetConfiguration().UseExpandedVasculature() == eSwitch::On ? pulse::ExpandedVascularCompartment::Intracranial : pulse::VascularCompartment::Brain;
    //Store tissue-blood pairs
    m_TissueToVascular.clear();
    m_TissueToVascular[m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::Fat)] = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Fat);
    m_TissueToVascular[m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::Bone)] = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Bone);
    m_TissueToVascular[m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::Brain)] = m_data.GetCompartments().GetLiquidCompartment(BrainCmpt);
    m_TissueToVascular[m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::Gut)] = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Gut);
    m_TissueToVascular[m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::LeftKidney)] = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftKidney);
    m_TissueToVascular[m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::RightKidney)] = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightKidney);
    m_TissueToVascular[m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::Liver)] = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Liver);
    m_TissueToVascular[m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::LeftLung)] = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftLung);
    m_TissueToVascular[m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::RightLung)] = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightLung);
    m_TissueToVascular[m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::Muscle)] = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Muscle);
    m_TissueToVascular[m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::Myocardium)] = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Myocardium);
    m_TissueToVascular[m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::Skin)] = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Skin);
    m_TissueToVascular[m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::Spleen)] = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Spleen);

    m_ConsumptionProdutionTissues.push_back(m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::Fat));
    m_ConsumptionProdutionTissues.push_back(m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::Bone));
    m_ConsumptionProdutionTissues.push_back(m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::Brain));
    m_ConsumptionProdutionTissues.push_back(m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::Gut));
    m_ConsumptionProdutionTissues.push_back(m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::LeftKidney));
    m_ConsumptionProdutionTissues.push_back(m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::RightKidney));
    m_ConsumptionProdutionTissues.push_back(m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::Liver));
    m_ConsumptionProdutionTissues.push_back(m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::Muscle));
    m_ConsumptionProdutionTissues.push_back(m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::Myocardium));
    m_ConsumptionProdutionTissues.push_back(m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::Skin));
    m_ConsumptionProdutionTissues.push_back(m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::Spleen));

    m_CardiacArrestVascularFlows_ml_per_min[m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Fat)] = 0;
    m_CardiacArrestVascularFlows_ml_per_min[m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Bone)] = 0;
    m_CardiacArrestVascularFlows_ml_per_min[m_data.GetCompartments().GetLiquidCompartment(BrainCmpt)] = 0;
    m_CardiacArrestVascularFlows_ml_per_min[m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Gut)] = 0;
    m_CardiacArrestVascularFlows_ml_per_min[m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftKidney)] = 0;
    m_CardiacArrestVascularFlows_ml_per_min[m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightKidney)] = 0;
    m_CardiacArrestVascularFlows_ml_per_min[m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Liver)] = 0;
    m_CardiacArrestVascularFlows_ml_per_min[m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Muscle)] = 0;
    m_CardiacArrestVascularFlows_ml_per_min[m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Myocardium)] = 0;
    m_CardiacArrestVascularFlows_ml_per_min[m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Skin)] = 0;
    m_CardiacArrestVascularFlows_ml_per_min[m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Spleen)] = 0;

    // Here is some code to cross check our maps with what is in the compartment manager
    // If by some chance, some other system added a new tissue compartment we don't know about
    // this will put out a warning
    for (SETissueCompartment* tissue : m_data.GetCompartments().GetTissueLeafCompartments())
    {
      if (m_TissueToVascular.find(tissue) == m_TissueToVascular.end() || m_TissueToVascular[tissue] == nullptr)
        Warning("Tissue found a tissue compartment that is not mapped to a vascular compartment  : " + tissue->GetName());

      if (m_LeftLungTissue != tissue && m_RightLungTissue != tissue)
      {// We don't use the lungs in the consuption/production methodology
        if (!Contains(m_ConsumptionProdutionTissues, (*tissue)))
          Warning("Tissue found a tissue compartment that it is not using in Consumption/Prodution : " + tissue->GetName());
      }
    }

    // Collect the vascular to tissue paths to tune druing cardiac arrest
    m_VascularResistances.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::BoneT2ToBoneT1));
    m_VascularResistances.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::BrainT2ToBrainT1));
    m_VascularResistances.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::FatT2ToFatT1));
    m_VascularResistances.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::LiverT2ToLiverT1));
    m_VascularResistances.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::LeftKidneyT2ToLeftKidneyT1));
    m_VascularResistances.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::RightKidneyT2ToRightKidneyT1));
    m_VascularResistances.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::LeftLungT2ToLeftLungT1));
    m_VascularResistances.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::RightLungT2ToRightLungT1));
    m_VascularResistances.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::MuscleT2ToMuscleT1));
    m_VascularResistances.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::SkinT2ToSkinT1));
    m_VascularResistances.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::MyocardiumT2ToMyocardiumT1));
    m_VascularResistances.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::SpleenT2ToSpleenT1));
    m_VascularResistances.push_back(m_data.GetCircuits().GetFluidPath(pulse::TissuePath::GutT2ToGutT1));
  }

  //#define logMeal
  void TissueModel::AtSteadyState()
  {
    if (m_data.GetState() == EngineState::AtInitialStableState)
    {// Apply our conditions
      if (m_data.GetConditions().HasConsumeMeal())
      {
        SEScalarMass mass;
        SEMeal& meal = m_data.GetConditions().GetConsumeMeal().GetMeal();
        double elapsedTime_s = meal.GetElapsedTime().GetValue(TimeUnit::s);
        double patientWeight_kg = SEScalar::Truncate(m_data.GetCurrentPatient().GetWeight(MassUnit::kg), 4);
        double renalVolumeCleared = m_Albumin->GetClearance().GetRenalClearance(VolumePerTimeMassUnit::mL_Per_s_kg) * patientWeight_kg * elapsedTime_s;
        double systemicVolumeCleared = m_Albumin->GetClearance().GetSystemicClearance(VolumePerTimeMassUnit::mL_Per_s_kg) * patientWeight_kg * elapsedTime_s - renalVolumeCleared;
        SEScalarVolume integratedVolume;
        integratedVolume.SetValue(systemicVolumeCleared, VolumeUnit::mL);
        mass.SetValue(m_AlbuminProdutionRate_g_Per_s * elapsedTime_s, MassUnit::g);
        // Distribute the mass over the body via a total volume weighting
#ifdef logMeal
        std::stringstream m_ss;
        double cvPreAbsorbed_g = m_data.GetCircuits()->TotalSubstanceInBlood(*m_Albumin, MassUnit::g);
        double evPreAbsorbed_g = m_data.GetCircuits()->TotalSubstanceInTissue(*m_Albumin, MassUnit::g);
        m_ss << "Preabsorbed Total Albumin in body : " << cvPreAbsorbed_g + evPreAbsorbed_g << "(g) cv(" << cvPreAbsorbed_g << ") ev(" << evPreAbsorbed_g << ")";
        Info(m_ss);
        m_ss << "Distributing " << mass << "of Albumin into body : ";
        Info(m_ss);
#endif
        /// \todo Replace with new Consume Methodology
        // m_data.GetCircuits().DistributeTissueMass(*m_Albumin, mass);
        // m_data.GetCircuits().BalanceBloodMassByClearedVolume(*m_Albumin, integratedVolume);
#ifdef logMeal
        double cvPostAbsorbed_g = m_data.GetCircuits()->TotalSubstanceInBlood(*m_Albumin, MassUnit::g);
        double evPostAbsorbed_g = m_data.GetCircuits()->TotalSubstanceInTissue(*m_Albumin, MassUnit::g);
        m_ss << "PostAbsorbed Total Albumin in body : " << cvPostAbsorbed_g + evPostAbsorbed_g << "(g), body absorbed : " << (cvPostAbsorbed_g + evPostAbsorbed_g) - (cvPreAbsorbed_g + evPreAbsorbed_g) << "(g) cv(" << cvPostAbsorbed_g << ") ev(" << evPostAbsorbed_g << ")";
        Info(m_ss);
#endif
      }

      Dehydration();
    }
    for (SETissueCompartment* tissue : m_ConsumptionProdutionTissues)
    {
      SELiquidCompartment* vascular = m_TissueToVascular[tissue];
      if (vascular->HasInFlow())
        m_CardiacArrestVascularFlows_ml_per_min[vascular] = vascular->GetInFlow(VolumePerTimeUnit::mL_Per_min);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Preprocess performs the systems interactions steps required for processing of the substances
  ///
  /// \details
  /// This function calculates the amount of albumin produced for a defined time period and adds it to the liver.
  //--------------------------------------------------------------------------------------------------
  void TissueModel::PreProcess()
  {
    ProduceAlbumin(m_data.GetTimeStep_s());
    // Glucose and lipid control is currently deactivated for model improvements.
    // GlucoseLipidControl(m_data.GetTimeStep_s());
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Process completes substance transport by performing diffusion and alveoli transfer
  ///
  /// \details
  /// This function completes substance transport between systems by calculating the oxygen and carbon dioxide saturation
  /// throughout the cardiovascular system and calculating diffusion and alveoli transfer of substances.
  //--------------------------------------------------------------------------------------------------
  void TissueModel::Process(bool /*solve_and_transport*/)
  {
    if (m_data.GetEvents().IsEventActive(eEvent::CardiacArrest))
    {
      double UpdatedResistance_mmHg_s_Per_mL = 0;
      double m_minIndividialSystemicResistance_mmHg_s_Per_mL = 0.1;
      for (SEFluidCircuitPath* Path : m_VascularResistances)
      {
        /// \todo We are treating all systemic resistance paths equally, including the brain.
        UpdatedResistance_mmHg_s_Per_mL = Path->GetNextResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
        UpdatedResistance_mmHg_s_Per_mL *= 50;
        if (UpdatedResistance_mmHg_s_Per_mL < m_minIndividialSystemicResistance_mmHg_s_Per_mL)
        {
          UpdatedResistance_mmHg_s_Per_mL = m_minIndividialSystemicResistance_mmHg_s_Per_mL;
        }
        Path->GetNextResistance().SetValue(UpdatedResistance_mmHg_s_Per_mL, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
        //m_data.GetDataTrack().Probe(Path->GetName() + "Resistance", Path->GetNextResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL));
      }
    }

    CalculateMetabolicConsumptionAndProduction(m_data.GetTimeStep_s());
    CalculatePulmonaryCapillarySubstanceTransfer();
    CalculateDiffusion();
    SEScalarMassPerVolume albuminConcentration;
    albuminConcentration.SetValue(45.0, MassPerVolumeUnit::g_Per_L);
    // Currently, substances are not where they need to be, we will hard code this for now until we fix them
    /// \todo Remove SetBodyState hardcode and replace with computed values after substance redux is complete
    m_data.GetSaturationCalculator().SetBodyState(albuminConcentration,
      m_data.GetBloodChemistry().GetHematocrit(),
      m_data.GetEnergy().GetCoreTemperature(),
      m_data.GetBloodChemistry().GetStrongIonDifference(),
      m_data.GetBloodChemistry().GetPhosphate());
    for (SELiquidCompartment* cmpt : m_data.GetCompartments().GetVascularLeafCompartments())
    {
      if (cmpt->HasVolume())
      {
        m_data.GetSaturationCalculator().CalculateBloodGasDistribution(*cmpt);
      }
    }
    CalculateVitals();
    ComputeExposedModelParameters();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Postprocess step
  ///
  /// \details
  /// There are currently no postprocess steps in the tissue system.
  //--------------------------------------------------------------------------------------------------
  void TissueModel::PostProcess(bool /*solve_and_transport*/)
  {

  }

  void TissueModel::ComputeExposedModelParameters()
  {

  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Determines the correct type of diffusion for a substance and executes the diffusion.
  ///
  ///
  /// \details
  /// Determines the type of diffusion for each substance and calls the function for that type of diffusion. 
  /// This is executed for all compartments in the tissue system. Options include, perfusion limited diffusion, 
  /// permeability limited diffusion (untested), pressure gradient diffusion, and flat rate diffusion. The appropriate
  /// diffusion methodology is chosen based on the substance parameters.
  //--------------------------------------------------------------------------------------------------
  void TissueModel::CalculateDiffusion()
  {
    SETissueCompartment* tissue;
    SELiquidCompartment* vascular;
    const SESubstanceTissuePharmacokinetics* tissueKinetics;

#ifdef PROBE_BLOOD_GASES
    double totalO2movedVtoE_ug = 0;
    double totalO2movedEtoI_ug = 0;
    double totalCO2movedVtoE_ug = 0;
    double totalCO2movedEtoI_ug = 0;
#endif

    for (auto tissueVascular : m_TissueToVascular)
    {
      tissue = tissueVascular.first;
      vascular = tissueVascular.second;
      SELiquidCompartment& extracellular = tissue->GetExtracellular();
      SELiquidCompartment& intracellular = tissue->GetIntracellular();
      for (const SESubstance* sub : m_data.GetCompartments().GetLiquidCompartmentSubstances())
      {
        tissueKinetics = nullptr;
        if (sub->HasPK())
          tissueKinetics = sub->GetPK()->GetTissueKinetics(tissue->GetName());
        //Check to see if substance is a drug with the appropriate parameters to calculate PK diffusion
        // If the substance is a PBPK drug, then diffusion is computed by perfusion limited diffusion, as described in \cite huisinga2012modeling
        if (tissueKinetics != nullptr)
        {
          if (!tissueKinetics->HasPartitionCoefficient())
            continue;// wtf...? why would it not have it?
          PerfusionLimitedDiffusion(*tissue, *vascular, *sub, tissueKinetics->GetPartitionCoefficient(), m_data.GetTimeStep_s()); //Balance happens in the method      
        }
        // Otherwise, the diffusion is computed by either:
        // Instantaneous diffusion (gases and very small molecules), Simple diffusion (all substances), Facilitated diffusion (glucose), or Active diffusion (pumps)
        else
        {
          // Skip the lungs because they have their own methodology with AlveolarPartialPressureGradientDiffusion
          // This is not true. AlveolarPartialPressureGradientDiffusion goes from a gas compartment to a vascular cmpt. Still need to move stuff into the extravascular space. TODO
          if (tissue == m_LeftLungTissue || tissue == m_RightLungTissue)
            continue;

          // Currently, only drugs and gases transport across the capillary
          /// \todo Enable non-advective transport for all substances
          if (sub->GetState() != eSubstance_State::Gas)
          {
            // Sodium is special. We need to diffuse for renal function.
            // We will not treat sodium any differently once diffusion functionality is fully implemented.
            if (sub == m_Sodium)
            {
              MoveMassByInstantDiffusion(*vascular, extracellular, *sub);
              vascular->GetSubstanceQuantity(*sub)->Balance(BalanceLiquidBy::Mass);
              extracellular.GetSubstanceQuantity(*sub)->Balance(BalanceLiquidBy::Mass);
            }
            continue;
          }

          /////////////////////////////////////////////////////////////////////////////////////////////////////////
          /////////////////// Vascular to Extravascular-Extracellular /////////////////////////////////////////////
          /////////////////////////////////////////////////////////////////////////////////////////////////////////

          // --- First, instant diffusion ---
          MoveMassByInstantDiffusion(*vascular, extracellular, *sub);

          // --- Second, simple diffusion ---
            // Compute the vascular to extracellular permeability coefficient
            // This is the coefficient per gram of tissue independent of the tissue type.
            // This uses the Renkin and Curry data for capillary exchange as reported in \cite fournier2011basic
            // Divide by 100 is because the Renkin-Curry equations are in per hectogram units, and 100 g/hg
            /// \todo I believe we can optimize with a cache of these values. Also, we can cache permeabilityCoefficient_mL_Per_s_g which is not a function of the tissue properties
          double molarMass_g_Per_mol = sub->GetMolarMass(MassPerAmountUnit::g_Per_mol);
          double molecularRadius_nm = 0.0348 * pow(molarMass_g_Per_mol, 0.4175);
          double vToECpermeabilityCoefficient_mL_Per_s_g = 0.0287 * pow(molecularRadius_nm, -2.920) / 100.0; // This is only valid if the molecular radius is > 1.0 nm.
          if (molecularRadius_nm < 1.0)
            vToECpermeabilityCoefficient_mL_Per_s_g = 0.0184 * pow(molecularRadius_nm, -1.223) / 100.0;

          // Multiply by tissue mass to get the tissue-dependent coefficient. 
          double vToECpermeabilityCoefficient_mL_Per_s = vToECpermeabilityCoefficient_mL_Per_s_g * tissue->GetTotalMass(MassUnit::g);
          // A tuning factor helps tune the dynamics - note that concentrations will ALWAYS equilibrate in steady state given enough time regardless of the permeability
          double vToECPermeabilityTuningFactor = 1.0;
          MoveMassBySimpleDiffusion(*vascular, extracellular, *sub, vToECPermeabilityTuningFactor * vToECpermeabilityCoefficient_mL_Per_s, m_data.GetTimeStep_s());

          // --- Third facilitated diffusion ---
          if (sub->HasMaximumDiffusionFlux())
          {
            double massToAreaCoefficient_cm2_Per_g = 1.0; /// \todo Define relationship between tissue mass and membrane area.
            double capCoverage_cm2 = massToAreaCoefficient_cm2_Per_g * tissue->GetTotalMass(MassUnit::g);
            double maximumMassFlux = sub->GetMaximumDiffusionFlux(MassPerAreaTimeUnit::g_Per_cm2_s);
            double combinedCoefficient_g_Per_s = maximumMassFlux * capCoverage_cm2;
            MoveMassByFacilitatedDiffusion(*vascular, extracellular, *sub, combinedCoefficient_g_Per_s, m_data.GetTimeStep_s());
          }

          // --- Fourth, and final vascular to EV-EC, Active diffusion ---
          double pumpRate_g_Per_s = 0.0;
          /// \todo Compute the pump rate from an empirically-determined baseline pump rate.
          MoveMassByActiveTransport(*vascular, extracellular, *sub, pumpRate_g_Per_s, m_data.GetTimeStep_s());

#ifdef PROBE_BLOOD_GASES
          if (sub == &m_data.GetSubstances().GetO2() || sub == &m_data.GetSubstances().GetCO2())
            m_data.GetDataTrack().Probe(tissue->GetName() + "_" + sub->GetName() + "_VascularToExtracelluar_ug", moved_ug);
          if (sub == &m_data.GetSubstances().GetO2())
            totalO2movedVtoE_ug += moved_ug;
          if (sub == &m_data.GetSubstances().GetCO2())
            totalCO2movedVtoE_ug += moved_ug;
#endif

          /////////////////////////////////////////////////////////////////////////////////////////////////////////
          /////////////////// Extravascular-Extracellular to Extravascular-Intracellular //////////////////////////
          /////////////////////////////////////////////////////////////////////////////////////////////////////////

          // --- First, instant diffusion ---
          MoveMassByInstantDiffusion(extracellular, intracellular, *sub);

          // --- Second, simple diffusion ---
            // Assuming that the capillary permeability coefficient is proportional to the cellular membrane permeability coefficient for a given tissue and substance
          double ECtoICPermeabilityFactor = 1.0; // This is the permeability constant
          MoveMassBySimpleDiffusion(extracellular, intracellular, *sub, ECtoICPermeabilityFactor * vToECpermeabilityCoefficient_mL_Per_s, m_data.GetTimeStep_s());

          // --- Third facilitated diffusion ---
            // In Pulse, only glucose moves by facilitated diffusion, and it is assumed that all glucose that gets to the 
            // intracellular space is used for energy or converted to glycogen for storage. So no facilitated diffusion between EC and IC.
            /// \todo Decrement glucose from EC for energy and decrement/increment from EC for conversions (glycogen, gluconeogenesis, etc).

          // --- Fourth, and final vascular to EV-EC, Active diffusion ---
          pumpRate_g_Per_s = 0.0;
          /// \todo Compute the pump rate from an empirically-determined baseline pump rate.
          MoveMassByActiveTransport(extracellular, intracellular, *sub, pumpRate_g_Per_s, m_data.GetTimeStep_s());

#ifdef PROBE_BLOOD_GASES
          if (sub == &m_data.GetSubstances().GetO2() || sub == &m_data.GetSubstances().GetCO2())
            m_data.GetDataTrack().Probe(tissue->GetName() + "_" + sub->GetName() + "_ExtracelluarToIntracelluar_ug", moved_ug);
          if (sub == &m_data.GetSubstances().GetO2())
            totalO2movedEtoI_ug += moved_ug;
          if (sub == &m_data.GetSubstances().GetCO2())
            totalCO2movedEtoI_ug += moved_ug;
#endif
          vascular->GetSubstanceQuantity(*sub)->Balance(BalanceLiquidBy::Mass);
          extracellular.GetSubstanceQuantity(*sub)->Balance(BalanceLiquidBy::Mass);
          intracellular.GetSubstanceQuantity(*sub)->Balance(BalanceLiquidBy::Mass);
        }
      }
    }
#ifdef PROBE_BLOOD_GASES
    m_data.GetDataTrack().Probe("VascularToExtracelluarO2Diffused_ug", totalO2movedVtoE_ug);
    m_data.GetDataTrack().Probe("ExtracelluarToIntracellularO2Diffused_ug", totalO2movedEtoI_ug);
    m_data.GetDataTrack().Probe("VascularToExtracelluarCO2Diffused_ug", totalCO2movedVtoE_ug);
    m_data.GetDataTrack().Probe("ExtracelluarToIntracellularCO2Diffused_ug", totalCO2movedEtoI_ug);
#endif
  }


  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Generic substance alveoli transfer calculations
  ///
  /// \param  vSubstances
  /// Alveoli transfer is completed for each substance in the input vector of substances.
  /// 
  /// \details
  /// The mass transferred between the alveoli and the capillaries is calculated for all substances in the 
  /// input vector. The concentration gradient between the alveoli and the capillaries, the substance-specific 
  /// alveoli transfer factor, the pulmonary capillary flow, the time step, and the patient-specific alveoli
  /// transfer factor are used to calculate the mass transfer. The mass in both the alveoli and the 
  /// capillaries is then updated. This represents gases moving from the respiratory to the cardiovascular
  /// systems and vice versa.
  //--------------------------------------------------------------------------------------------------
  void TissueModel::CalculatePulmonaryCapillarySubstanceTransfer()
  {
    const PulseConfiguration& Configuration = m_data.GetConfiguration();

    double pulmonaryCapillaryCoverageFraction = m_data.GetCardiovascular().GetPulmonaryCapillariesCoverageFraction().GetValue();

    for (SESubstance* sub : m_data.GetSubstances().GetActiveGases())
    {
      sub->GetAlveolarTransfer().SetValue(0, VolumePerTimeUnit::mL_Per_s);
      sub->GetDiffusingCapacity().SetValue(0, VolumePerTimePressureUnit::mL_Per_s_mmHg);

      //Left Side Alveoli Transfer
      if (m_LeftAlveoli->HasChildren())
      {
        for (unsigned int iter = 0; iter < m_LeftAlveoli->GetLeaves().size(); iter++)
        {
          SEGasCompartment* alveoli = m_LeftAlveoli->GetLeaves().at(iter);
          SELiquidCompartment* capillaries = m_LeftPulmonaryCapillaries->GetLeaves().at(iter);
          double leftAlveoliSurfaceArea_cm2 = alveoli->GetDiffusionSurfaceArea().GetValue(AreaUnit::cm2);
          double leftDiffusionSurfaceArea_cm2 = leftAlveoliSurfaceArea_cm2 * pulmonaryCapillaryCoverageFraction;
          double leftDiffusingCapacityOfOxygen_mL_Per_s_mmHg = (leftDiffusionSurfaceArea_cm2 * Configuration.GetStandardOxygenDiffusionCoefficient(AreaPerTimePressureUnit::cm2_Per_s_mmHg)) / Configuration.GetStandardDiffusionDistance(LengthUnit::cm);
          AlveolarPartialPressureGradientDiffusion(*alveoli, *capillaries, *sub, leftDiffusingCapacityOfOxygen_mL_Per_s_mmHg, m_data.GetTimeStep_s());

          if (alveoli->GetSubstanceQuantity(*sub)->GetVolume(VolumeUnit::mL) < 0.0 || capillaries->GetSubstanceQuantity(*sub)->GetMass(MassUnit::ug) < 0.0)
          {
            Fatal("Diffusion mass cannot be negative");
          }

          alveoli->Balance(BalanceGasBy::Volume);
        }
      }
      else
      {
        double leftAlveoliSurfaceArea_cm2 = m_LeftAlveoli->GetDiffusionSurfaceArea().GetValue(AreaUnit::cm2);
        double leftDiffusionSurfaceArea_cm2 = leftAlveoliSurfaceArea_cm2 * pulmonaryCapillaryCoverageFraction;
        double leftDiffusingCapacityOfOxygen_mL_Per_s_mmHg = (leftDiffusionSurfaceArea_cm2 * Configuration.GetStandardOxygenDiffusionCoefficient(AreaPerTimePressureUnit::cm2_Per_s_mmHg)) / Configuration.GetStandardDiffusionDistance(LengthUnit::cm);
        AlveolarPartialPressureGradientDiffusion(*m_LeftAlveoli, *m_LeftPulmonaryCapillaries, *sub, leftDiffusingCapacityOfOxygen_mL_Per_s_mmHg, m_data.GetTimeStep_s());

        if (m_LeftAlveoli->GetSubstanceQuantity(*sub)->GetVolume(VolumeUnit::mL) < 0.0 || m_LeftPulmonaryCapillaries->GetSubstanceQuantity(*sub)->GetMass(MassUnit::ug) < 0.0)
        {
          Fatal("Diffusion mass cannot be negative");
        }

        m_LeftAlveoli->Balance(BalanceGasBy::Volume);
      }

      //Right Side Alveoli Transfer
      if (m_RightAlveoli->HasChildren())
      {
        for (unsigned int iter = 0; iter < m_RightAlveoli->GetLeaves().size(); iter++)
        {
          SEGasCompartment* alveoli = m_RightAlveoli->GetLeaves().at(iter);
          SELiquidCompartment* capillaries = m_RightPulmonaryCapillaries->GetLeaves().at(iter);
          double rightAlveoliSurfaceArea_cm2 = alveoli->GetDiffusionSurfaceArea().GetValue(AreaUnit::cm2);
          double rightDiffusionSurfaceArea_cm2 = rightAlveoliSurfaceArea_cm2 * pulmonaryCapillaryCoverageFraction;
          double rightDiffusingCapacityOfOxygen_mL_Per_s_mmHg = (rightDiffusionSurfaceArea_cm2 * Configuration.GetStandardOxygenDiffusionCoefficient(AreaPerTimePressureUnit::cm2_Per_s_mmHg)) / Configuration.GetStandardDiffusionDistance(LengthUnit::cm);
          AlveolarPartialPressureGradientDiffusion(*alveoli, *capillaries, *sub, rightDiffusingCapacityOfOxygen_mL_Per_s_mmHg, m_data.GetTimeStep_s());

          if (alveoli->GetSubstanceQuantity(*sub)->GetVolume(VolumeUnit::mL) < 0.0 || capillaries->GetSubstanceQuantity(*sub)->GetMass(MassUnit::ug) < 0.0)
          {
            Fatal("Diffusion mass cannot be negative");
          }

          alveoli->Balance(BalanceGasBy::Volume);
        }
      }
      else
      {
        double rightAlveoliSurfaceArea_cm2 = m_RightAlveoli->GetDiffusionSurfaceArea().GetValue(AreaUnit::cm2);
        double rightDiffusionSurfaceArea_cm2 = rightAlveoliSurfaceArea_cm2 * pulmonaryCapillaryCoverageFraction;
        double rightDiffusingCapacityOfOxygen_mL_Per_s_mmHg = (rightDiffusionSurfaceArea_cm2 * Configuration.GetStandardOxygenDiffusionCoefficient(AreaPerTimePressureUnit::cm2_Per_s_mmHg)) / Configuration.GetStandardDiffusionDistance(LengthUnit::cm);
        AlveolarPartialPressureGradientDiffusion(*m_RightAlveoli, *m_RightPulmonaryCapillaries, *sub, rightDiffusingCapacityOfOxygen_mL_Per_s_mmHg, m_data.GetTimeStep_s());

        if (m_RightAlveoli->GetSubstanceQuantity(*sub)->GetVolume(VolumeUnit::mL) < 0.0 || m_RightPulmonaryCapillaries->GetSubstanceQuantity(*sub)->GetMass(MassUnit::ug) < 0.0)
        {
          Fatal("Diffusion mass cannot be negative");
        }

        m_RightAlveoli->Balance(BalanceGasBy::Volume);
      }
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Flat Rate production of albumin in the liver
  ///
  /// \param  duration_s
  /// Time period for production of albumin in seconds.
  /// 
  /// \details
  /// The mass of albumin to produce is calculated from a flat rate production value and the time passed. 
  /// This mass is added to the liver. The rate is currently specified as .15 mg/s as found in Jarnum, et al 1972.
  //--------------------------------------------------------------------------------------------------
  void TissueModel::ProduceAlbumin(double duration_s)
  {
    double massConverted_g = m_AlbuminProdutionRate_g_Per_s * duration_s;
    m_LiverTissueAlbumin->GetMass().Increment(massConverted_g, MassUnit::g);
    m_LiverTissueAlbumin->Balance(BalanceLiquidBy::Mass);
  }


  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates the production and consumption of substances in the tissues in a given time period
  ///
  /// \param  time
  /// Time period over which consumption and/or production occurs
  ///
  /// \details
  /// This method computes all of the metabolic consumption and production of substances.
  /// There is some consumption and production in all of the tissue compartments.
  /// The oxygen consumption, carbon dioxide production, and nutrient production/consumption are all
  /// determined from the current metabolic rate and the respiratory quotient. 
  /// The total consumption and production rates are then proportioned to
  /// each tissue compartment based on the fraction of blood flow to that compartment. Compartment-specific and 
  /// substance-specific production and consumption is computed in this method.
  /// The system properties for oxygen consumption rate and carbon dioxide production rate are recorded here.
  /// Consumption and production all takes place in the intracellular space.
  //--------------------------------------------------------------------------------------------------
  void TissueModel::CalculateMetabolicConsumptionAndProduction(double time_s)
  {
    bool gasOnly = false; // To easily disable non-gas substance production and consumption
    const PulseConfiguration& config = m_data.GetConfiguration();
    double TMR_kcal_Per_s = m_data.GetEnergy().GetTotalMetabolicRate(PowerUnit::kcal_Per_s);
    double BMR_kcal_Per_s = m_data.GetCurrentPatient().GetBasalMetabolicRate(PowerUnit::kcal_Per_s);
    double ATPUseRate_mol_Per_s = TMR_kcal_Per_s / config.GetEnergyPerATP(EnergyPerAmountUnit::kcal_Per_mol);
    double BloodFlowFraction;         // Fraction of blood flow entering a tissue compartment. Used to determine the fraction of total metabolic activity in a tissue compartment.
    double LocalATPUseRate_mol_Per_s; // Holds the ATP use rate for a tissue compartment
    double anaerobicWeight;           // Determines how much lactate is produced and oxygen consumed with glucose consumption.
    double massConverted_g;

    // The following fractions are used to compute the metabolic conversion of substances.
    // Stoichiometric ratios can be found in any physiology text, such as \cite guyton2006medical
    const double FractionOfO2CO2ToGlucose = 0.157894737;      // Ratio of o2/co2 required to produce ATP for glucose consumption. = 6.0 / 38.0 
    const double FractionOfO2ToLipid = 0.212239583;           // Ratio of o2 required to produce ATP for lipid consumption. = 163.0 / 768.0;
    const double FractionOfCO2ToLipid = 0.1484375;            // ratio of co2 required to produce ATP for lipid consumption. = 114.0 / 768.0;
    SEScalarPressure ppO2;
    SEScalarMassPerVolume atConc;
    ppO2.SetValue(40.0, PressureUnit::mmHg);
    GeneralMath::CalculateHenrysLawConcentration(*m_O2, ppO2, atConc, m_Logger);
    const double anaerobicThresholdConcentration_mM = atConc.GetValue(MassPerVolumeUnit::g_Per_L) / m_O2->GetMolarMass(MassPerAmountUnit::g_Per_mmol);
    const double FractionOfGlucoseToATP = 0.026315789;        // Ratio of glucose required to ATP produced. = 1.0 / 38.0;
    const double FractionOfLactateToGlucose = 0.5;            // Ratio of glucose required to lactate produced during anaerobic metabolism. = 1.0 / 2.0;
    //const double FractionOfAcetoacetateToATP = 0.041666667; // Ratio of acetoacetate required to ATP produced. = 1.0 / 24.0; (unused)
    //const double FractionOfLactateToATP = 0.027777778;        // Ratio of lactate required to ATP produced. = 1.0 / 36.0;
    const double FractionOfLipidToATP = 0.002604167;          // Ratio of of lipid required to ATP produced. = 2.0 / 768.0;
    const double FractionLipidsAsTristearin = 0.256;          // This is an empirically determined value specific to the Pulse implementation

    const double exerciseTuningFactor = 1.0; // 2.036237;           // A tuning factor to adjust production and consumption during exercise

    double insulinConc_ug_Per_L = m_data.GetSubstances().GetInsulin().GetBloodConcentration(MassPerVolumeUnit::ug_Per_L);


    /// The respiratory quotient (RQ) is calculated as a value between 0.7 and 1.0 and linearly increasing 
    /// with the ratio of current glucose to resting glucose at a rate of 0.15. As stored glucose levels
    /// increase, the RQ shifts towards glucose, reaching 1.0 when glucose stores are twice resting.
    /// Note that in human physiology the RQ can increase above 1.0, but the current 
    /// method of computing the fraction of carbohydrates consumed assumes that the RQ is upper bounded at 1.0.
    double currentTotalGlucoseStored_g = 0.0;
    double RespiratoryQuotient;
    for (auto t2v : m_TissueToVascular)
    {
      // We use extracellular glucose to quantify stored glucose with the assumption that intracellular glucose is instantly used for energy.
      currentTotalGlucoseStored_g += t2v.first->GetExtracellular().GetSubstanceQuantity(*m_Glucose)->GetMass(MassUnit::g);
    }
    RespiratoryQuotient = currentTotalGlucoseStored_g / m_RestingTissueGlucose_g * 0.15 + 0.7;
    // If the patient is exercising, an emphasis is placed on glucose consumption (higher RQ) as long as plenty of glucose is available.
    // But if glucose stores are depleting, we want to emphasize glucose consumption a little less.
    if (m_data.GetActions().GetPatientActions().HasExercise() && RespiratoryQuotient > 0.8)
    {
      double totalWorkRateLevel = m_data.GetEnergy().GetTotalWorkRateLevel().GetValue();
      if (RespiratoryQuotient > 0.775)
      {
        RespiratoryQuotient = RespiratoryQuotient + 0.15 * totalWorkRateLevel * exerciseTuningFactor;
      }
      else
      {
        RespiratoryQuotient = RespiratoryQuotient + (2 * RespiratoryQuotient - 1.4) * totalWorkRateLevel * exerciseTuningFactor;
      }
    }
    RespiratoryQuotient = MIN(RespiratoryQuotient, 1.0);
    //m_data.GetDataTrack().Probe("RespiratoryQuotient", RespiratoryQuotient);
    //m_data.GetDataTrack().Probe("CurrentTotalGlucoseStored_g", currentTotalGlucoseStored_g);

    // The fraction of glucose consumed is determined from a linear relation of the non-protein respiratory quotient.
    // A respiratory quotient of 0.7 would specify that the metabolism is completely derived from lipids,
    // while a respiratory quotient of 1.0 would be completely derived from carbohydrates.
    double FractionCarbConsumed = (RespiratoryQuotient - 0.7) / 0.3;
    BLIM(FractionCarbConsumed, 0., 1.);

    //The basal ketone production rate a resting patient and maximum production rate in starvation are derived from data in @cite garber1974ketone
    double basalKetoneProductionRate_mmol_Per_kg_Per_s = (4.0E-3) / 60.0;
    double maxKetoneProductionRate_mmol_Per_kg_Per_s = (12.0E-3) / 60.0;
    // The ketone production rate is calculated as linear equation, with basal ketone production rate as the intercept. 
    // The current ketone production rate will be equal to the basal value when the patient has a normal metabolism (RespiratoryQuotient=0.85, FractionCarbConsumed=0.5).
    // For increasing starvation levels, FractionCarbConsumed decreases leading to increased ketone production
    double ketoneProductionRateChange_mmol_Per_kg_Per_s = (1.0 - 2.0 * FractionCarbConsumed) * (maxKetoneProductionRate_mmol_Per_kg_Per_s - basalKetoneProductionRate_mmol_Per_kg_Per_s);
    double KetoneProductionRate_mmol_Per_kg_s = basalKetoneProductionRate_mmol_Per_kg_Per_s + ketoneProductionRateChange_mmol_Per_kg_Per_s;
    LLIM(KetoneProductionRate_mmol_Per_kg_s, 0.0);

    /// \todo Remove this temporary blood increment when diffusion is operational (0.125 is tuning factor)
    double acetoacetateIncrement_mg = 0.375 * KetoneProductionRate_mmol_Per_kg_s * m_Acetoacetate->GetMolarMass(MassPerAmountUnit::mg_Per_mmol)
      * SEScalar::Truncate(m_data.GetCurrentPatient().GetWeight(MassUnit::kg), 4) * time_s;
    m_LiverAcetoacetate->GetMass().Increment(acetoacetateIncrement_mg, MassUnit::mg);
    if (m_LiverAcetoacetate->GetMass(MassUnit::ug) < ZERO_APPROX)
    {
      m_LiverAcetoacetate->GetMass().SetValue(0.0, MassUnit::ug);
    }
    m_LiverAcetoacetate->Balance(BalanceLiquidBy::Mass);
    // End temporary blood increment

    // Lactate production is computed per tissue
    //double lactateProduction_mol_Per_s = 0.0;
    double lactateProductionTuningParameter = 0.6;
    double lactateConsumptionTuningParameter = 0.000075;

    double tissueO2_mM;

    SELiquidCompartment* vascular;
    SELiquidSubstanceQuantity* TissueO2;
    SELiquidSubstanceQuantity* TissueCO2;
    SELiquidSubstanceQuantity* TissueGlucose;
    SELiquidSubstanceQuantity* TissueTristearin;
    SELiquidSubstanceQuantity* TissueLactate;
    SELiquidSubstanceQuantity* TissueAcetoacetate;
    SELiquidSubstanceQuantity* TissueCreatinine;

    //--------------------------------------------------------------------------------------------------
    /// The metabolic fraction for each tissue compartment is computed as the fraction of the total vascular blood flow
    /// that is going into the specific vascular compartment that is associated with the tissue compartment.
    //--------------------------------------------------------------------------------------------------
    double totalFlowRate_mL_Per_min = 0.0;
    // First sum the vascular flow into all of the vascular compartments that are associated with the tissue compartments.
    // We do not use the cardiac output as total flow rate because all of the fractions must sum to one at each time slice.
    for (SETissueCompartment* tissue : m_ConsumptionProdutionTissues)
    {
      vascular = m_TissueToVascular[tissue];
      if (m_data.GetEvents().IsEventActive(eEvent::CardiacArrest))
        totalFlowRate_mL_Per_min += m_CardiacArrestVascularFlows_ml_per_min[vascular];
      else if (vascular->HasInFlow())
        totalFlowRate_mL_Per_min += vascular->GetInFlow(VolumePerTimeUnit::mL_Per_min);
    }

    double oxygenConsumptionRate_g_Per_s = 0.0;
    double carbonDioxideProductionRate_g_Per_s = 0.0;
    double bloodGlucose_mg_Per_dL = m_data.GetSubstances().GetGlucose().GetBloodConcentration(MassPerVolumeUnit::mg_Per_dL);

    for (SETissueCompartment* tissue : m_ConsumptionProdutionTissues)
    {
      double acidDissociationFraction = 0.5;
      vascular = m_TissueToVascular[tissue];
      SELiquidCompartment& intracellular = tissue->GetIntracellular();
      TissueO2 = intracellular.GetSubstanceQuantity(*m_O2);
      TissueCO2 = intracellular.GetSubstanceQuantity(*m_CO2);
      TissueGlucose = intracellular.GetSubstanceQuantity(*m_Glucose);
      TissueTristearin = intracellular.GetSubstanceQuantity(*m_Tristearin);
      TissueLactate = intracellular.GetSubstanceQuantity(*m_Lactate);
      TissueAcetoacetate = intracellular.GetSubstanceQuantity(*m_Acetoacetate);
      TissueCreatinine = intracellular.GetSubstanceQuantity(*m_Creatinine);

      tissueO2_mM = TissueO2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);

      BloodFlowFraction = 0;
      if (m_data.GetEvents().IsEventActive(eEvent::CardiacArrest))
        BloodFlowFraction = m_CardiacArrestVascularFlows_ml_per_min[vascular] / totalFlowRate_mL_Per_min;
      else if (vascular->HasInFlow() && totalFlowRate_mL_Per_min > 0)
        BloodFlowFraction = vascular->GetInFlow(VolumePerTimeUnit::mL_Per_min) / totalFlowRate_mL_Per_min;

      //Reduced blood volumes cause reduced tissue perfusion
      double vasularBaselineVolume_mL = 0.0;
      double vascularCurrentVolulme_mL = 0.0;
      for (SEFluidCircuitNode* node : vascular->GetNodeMapping().GetNodes())
      {
        if (node->HasNextVolume() && node->HasVolumeBaseline())
        {
          vascularCurrentVolulme_mL += node->GetNextVolume(VolumeUnit::mL);
          vasularBaselineVolume_mL += node->GetVolumeBaseline(VolumeUnit::mL);
        }
      }

      double vascularVolumeMultiplier = 1.0;
      if (vasularBaselineVolume_mL > 0.0)
      {
        double vascularVolumeFraction = MIN(vascularCurrentVolulme_mL / vasularBaselineVolume_mL, 1.0);
        double bufferCuttoffFraction = 0.85;
        if (vascularVolumeFraction < bufferCuttoffFraction) //Give it a little buffer
        {
          vascularVolumeMultiplier = GeneralMath::LinearInterpolator(0.0, bufferCuttoffFraction, 5.0, 1.0, vascularVolumeFraction);
        }
      }

      LocalATPUseRate_mol_Per_s = BloodFlowFraction * ATPUseRate_mol_Per_s * vascularVolumeMultiplier;
      anaerobicWeight = MIN(1.0, tissueO2_mM / anaerobicThresholdConcentration_mM);
      /// \todo This is the reason for the decrease in O2 consumption and CO2 production with oxygen depletion. It looks like we are mixing models here. 
      // At a minimum, we know that CO2 production should not decrease at the same rate as O2 consumption during anaerobic metabolism.

      /////////////////////////////////////////////////////////////////////
      /////////////// Gas production and consumption //////////////////////
      /////////////////////////////////////////////////////////////////////
      // Oxygen Consumption
      double oxygenConsumption_mol_Per_s = FractionCarbConsumed * anaerobicWeight * FractionOfO2CO2ToGlucose * LocalATPUseRate_mol_Per_s
        + (1.0 - FractionCarbConsumed) * FractionOfO2ToLipid * LocalATPUseRate_mol_Per_s;
      massConverted_g = oxygenConsumption_mol_Per_s * (m_O2->GetMolarMass(MassPerAmountUnit::g_Per_mol)) * time_s;

#ifdef PROBE_BLOOD_GASES
      m_data.GetDataTrack().Probe(tissue->GetName() + "_Converted_O2_ug", massConverted_g * 1e6);
#endif

      //massConverted_g = MIN(TissueO2->GetMass(MassUnit::g), massConverted_g); //Bound oxygen consumed mass to prevent negative mass in the compartment
      //massConverted_g = MAX(massConverted_g, 0.0);

      double O2TuningParameter = 0.38 + 0.15 * (TMR_kcal_Per_s / BMR_kcal_Per_s - 1.0);
      ULIM(O2TuningParameter, 1.0);
      massConverted_g *= O2TuningParameter;
      BLIM(massConverted_g, 0.0, TissueO2->GetMass(MassUnit::g));
      TissueO2->GetMass().Increment(-massConverted_g, MassUnit::g);
      if (std::abs(TissueO2->GetMass(MassUnit::ug)) < ZERO_APPROX)
      {
        TissueO2->GetMass().SetValue(0.0, MassUnit::ug);
      }
      oxygenConsumptionRate_g_Per_s += massConverted_g / time_s;

#ifdef PROBE_BLOOD_GASES
      m_data.GetDataTrack().Probe(tissue->GetName() + "_Consumed_O2_ug", O2TuningParameter * massConverted_g * 1e6);
#endif

      // CO2 production
      double co2Production_mol_Per_s = FractionCarbConsumed * anaerobicWeight * FractionOfO2CO2ToGlucose * LocalATPUseRate_mol_Per_s
        + (1.0 - FractionCarbConsumed) * FractionOfCO2ToLipid * LocalATPUseRate_mol_Per_s;
      massConverted_g = co2Production_mol_Per_s * (m_CO2->GetMolarMass(MassPerAmountUnit::g_Per_mol)) * time_s;

      double CO2TuningParameter = 0.36 + m_data.GetEnergy().GetTotalWorkRateLevel().GetValue() * exerciseTuningFactor;
      TissueCO2->GetMass().Increment(CO2TuningParameter * massConverted_g, MassUnit::g);
      if (std::abs(TissueCO2->GetMass(MassUnit::ug)) < ZERO_APPROX)
      {
        TissueCO2->GetMass().SetValue(0.0, MassUnit::ug);
      }
      carbonDioxideProductionRate_g_Per_s += CO2TuningParameter * massConverted_g / time_s;

      if (!gasOnly)
      {
        ///////////////////////////////////////////////////////////////
        /////// Non-gas substance production //////////////////////////
        ///////////////////////////////////////////////////////////////
        //Lactate Production occurs due to anaerobic metabolism of glucose. The anaerobic weighting factor is used to dictate the fraction of glucose metabolized anaerobically.
        double currentlactateProduction_mol_Per_s = acidDissociationFraction * lactateProductionTuningParameter * (1 - anaerobicWeight) * LocalATPUseRate_mol_Per_s;
        massConverted_g = currentlactateProduction_mol_Per_s * (m_Lactate->GetMolarMass(MassPerAmountUnit::g_Per_mol)) * time_s;
        //lactateProduction_mol_Per_s += (massConverted_g / m_Lactate->GetMolarMass(MassPerAmountUnit::g_Per_mol)) / time_s;

        /// \todo Remove this temporary blood increment when diffusion is fully operational
        if (massConverted_g < 0.)
          DistributeMassbyMassWeighted(*vascular, *m_Lactate, massConverted_g, MassUnit::mg);
        else
          DistributeMassbyVolumeWeighted(*vascular, *m_Lactate, massConverted_g, MassUnit::mg);
        vascular->GetSubstanceQuantity(*m_Lactate)->Balance(BalanceLiquidBy::Mass);
        // End temporary blood increment

        ///////////////////////////////////////////////////////////////
        /////// Non-gas substance consumption /////////////////////////
        ///////////////////////////////////////////////////////////////
        double glucoseConsumption_mol_Per_s = FractionCarbConsumed * LocalATPUseRate_mol_Per_s * (anaerobicWeight * FractionOfGlucoseToATP + (1.0 - anaerobicWeight) * FractionOfLactateToGlucose);
        double glucoseConsumption_mg_Per_s = glucoseConsumption_mol_Per_s * m_Glucose->GetMolarMass(MassPerAmountUnit::g_Per_mol);
        massConverted_g = glucoseConsumption_mg_Per_s * time_s;
        massConverted_g = MIN(massConverted_g, TissueGlucose->GetMass(MassUnit::g));
        TissueGlucose->GetMass().Increment(-massConverted_g, MassUnit::g);

        /// \todo Remove this temporary blood increment when diffusion is fully operational 
        // The insulin effect is based on the insulin dependent term in the model described in \cite tolic2000modeling
        // Tuning parameter to convert existing model max to values found in \cite keener2009mathematical
        double InsulinIndependRate_mg_Per_s = 1.0 * glucoseConsumption_mg_Per_s;
        double insulinDependMax = 0.01; // 0.01
        double insulinEffectTimeK = 0.8; // 0.8
        double insulinDependRate_mg_Per_s = insulinDependMax - (insulinDependMax / exp(insulinEffectTimeK * insulinConc_ug_Per_L));
        double glucoseIncrement_mg = -(insulinDependRate_mg_Per_s + InsulinIndependRate_mg_Per_s) * bloodGlucose_mg_Per_dL * time_s;
        if (glucoseIncrement_mg < 0.)
          DistributeMassbyMassWeighted(*vascular, *m_Glucose, glucoseIncrement_mg, MassUnit::mg);
        else
          DistributeMassbyVolumeWeighted(*vascular, *m_Glucose, glucoseIncrement_mg, MassUnit::mg);
        vascular->GetSubstanceQuantity(*m_Glucose)->Balance(BalanceLiquidBy::Mass);

        if (std::abs(TissueGlucose->GetMass(MassUnit::ug)) < ZERO_APPROX)
        {
          TissueGlucose->GetMass().SetValue(0.0, MassUnit::ug);
        }

        double TristearinConsumption_mol_Per_s = FractionLipidsAsTristearin * (1.0 - FractionCarbConsumed) * FractionOfLipidToATP * LocalATPUseRate_mol_Per_s;
        massConverted_g = TristearinConsumption_mol_Per_s * (m_Tristearin->GetMolarMass(MassPerAmountUnit::g_Per_mol)) * time_s;
        massConverted_g = MIN(massConverted_g, TissueTristearin->GetMass(MassUnit::g));
        TissueTristearin->GetMass().Increment(-massConverted_g, MassUnit::g);

        /// \todo Remove this temporary blood increment when diffusion is operational (0.125 is tuning factor)
        double tristearinIncrement_mg = -0.125 * TristearinConsumption_mol_Per_s * m_Tristearin->GetMolarMass(MassPerAmountUnit::mg_Per_mol) * time_s;
        if (tristearinIncrement_mg < 0.)
          DistributeMassbyMassWeighted(*vascular, *m_Tristearin, tristearinIncrement_mg, MassUnit::mg);
        else
          DistributeMassbyVolumeWeighted(*vascular, *m_Tristearin, tristearinIncrement_mg, MassUnit::mg);
        vascular->GetSubstanceQuantity(*m_Tristearin)->Balance(BalanceLiquidBy::Mass);
        // End temporary blood increment

        if (std::abs(TissueTristearin->GetMass(MassUnit::ug)) < ZERO_APPROX)
        {
          TissueTristearin->GetMass().SetValue(0.0, MassUnit::ug);
        }

        ///////////////////////////////////////////////////////////////
        // Tissue-specific non-gas substance production and consumption 
        ///////////////////////////////////////////////////////////////
        if (tissue == m_MuscleTissue)
        {
          double creatinineProductionRate_mg_Per_s = 2.0e-5; /// \todo Creatinine production rate should be a function of muscle mass.
          massConverted_g = creatinineProductionRate_mg_Per_s * time_s;
          TissueCreatinine->GetMass().Increment(massConverted_g, MassUnit::g);
          if (std::abs(TissueCreatinine->GetMass(MassUnit::ug)) < ZERO_APPROX)
          {
            TissueCreatinine->GetMass().SetValue(0.0, MassUnit::ug);
          }
          m_data.GetEnergy().GetCreatinineProductionRate().SetValue(massConverted_g / time_s * 1. / m_Creatinine->GetMolarMass(MassPerAmountUnit::g_Per_umol), AmountPerTimeUnit::umol_Per_s);
          /// \todo Remove this temporary blood increment when diffusion is fully operational
          double creatinineTuning = 1.0;
          if (massConverted_g < 0.)
            DistributeMassbyMassWeighted(*vascular, *m_Creatinine, massConverted_g * creatinineTuning, MassUnit::g);
          else
            DistributeMassbyVolumeWeighted(*vascular, *m_Creatinine, massConverted_g * creatinineTuning, MassUnit::g);
          vascular->GetSubstanceQuantity(*m_Creatinine)->Balance(BalanceLiquidBy::Mass);
        }
        else if (tissue == m_LiverTissue)
        {
          massConverted_g = 0;
          if (TissueLactate->GetMass(MassUnit::g) > 0.0)
          {
            massConverted_g = lactateConsumptionTuningParameter * FractionOfLactateToGlucose * (TissueLactate->GetMass(MassUnit::g) /
              m_Lactate->GetMolarMass(MassPerAmountUnit::g_Per_mol)) * m_Glucose->GetMolarMass(MassPerAmountUnit::g_Per_mol);
            TissueGlucose->GetMass().Increment(massConverted_g, MassUnit::g);

            if (std::abs(TissueGlucose->GetMass(MassUnit::ug)) < ZERO_APPROX)
            {
              TissueGlucose->GetMass().SetValue(0.0, MassUnit::ug);
            }
          }
          //m_data.GetDataTrack().Probe("Glucose_Stored_mg", massConverted_g * 1000);
          /// \todo Fully implement endocrine glucose control and remove this temporary blood increment once diffusion is fully operational
          // If blood glucose is low, glucose will be pulled from the liver tissue into the vascular region. This is the equivalent of a glucagon response.
          // 0.02 is a time tuning factor.
          double massReleased_mg = 0;
          if (bloodGlucose_mg_Per_dL < 85.0)
          {
            massReleased_mg = 0.1 * (85.0 - bloodGlucose_mg_Per_dL) * vascular->GetVolume(VolumeUnit::dL) * time_s;
            //double massReleased_mg = 1.68 * time_s;
            DistributeMassbyVolumeWeighted(*vascular, *m_Glucose, massReleased_mg, MassUnit::mg);
            vascular->GetSubstanceQuantity(*m_Glucose)->Balance(BalanceLiquidBy::Mass);
          } // End temporary endocrine control of glucose
          //m_data.GetDataTrack().Probe("Glucose_Released_mg", massReleased_mg);

          massConverted_g = acidDissociationFraction * KetoneProductionRate_mmol_Per_kg_s
            * SEScalar::Truncate(m_data.GetCurrentPatient().GetWeight(MassUnit::kg), 4)
            * m_Acetoacetate->GetMolarMass(MassPerAmountUnit::g_Per_mmol);
        }
      }

      TissueO2->Balance(BalanceLiquidBy::Mass);
      TissueCO2->Balance(BalanceLiquidBy::Mass);
      TissueGlucose->Balance(BalanceLiquidBy::Mass);
      TissueTristearin->Balance(BalanceLiquidBy::Mass);
      TissueLactate->Balance(BalanceLiquidBy::Mass);
      TissueAcetoacetate->Balance(BalanceLiquidBy::Mass);
      TissueCreatinine->Balance(BalanceLiquidBy::Mass);

#ifdef PROBE_BLOOD_GASES
      m_data.GetSubstances().ProbeBloodGases(*vascular);

      //m_data.GetDataTrack().Probe(tissue->GetName() + "_Extracellular_mL", intracellular.GetVolume(VolumeUnit::mL));
      //m_data.GetDataTrack().Probe(tissue->GetName() + "_Intracellular_mL", intracellular.GetVolume(VolumeUnit::mL));

      //SELiquidSubstanceQuantity *O2 = intracellular.GetSubstanceQuantity(*m_O2);
      //SELiquidSubstanceQuantity *CO2 = intracellular.GetSubstanceQuantity(*m_CO2);
      //m_data.GetDataTrack().Probe(tissue->GetName() + "_Intracellular_O2_ug", O2->GetMass(MassUnit::ug));
      //m_data.GetDataTrack().Probe(tissue->GetName() + "_Intracellular_O2_ug_Per_mL", O2->GetConcentration(MassPerVolumeUnit::ug_Per_mL));
      ////m_data.GetDataTrack().Probe(tissue->GetName() + "_Intracellular_O2_mmHg", O2->GetPartialPressure(PressureUnit::mmHg));
      //m_data.GetDataTrack().Probe(tissue->GetName() + "_Intracellular_CO2_ug", CO2->GetMass(MassUnit::ug));
      //m_data.GetDataTrack().Probe(tissue->GetName() + "_Intracellular_CO2_ug_Per_mL", CO2->GetConcentration(MassPerVolumeUnit::ug_Per_mL));
      ////m_data.GetDataTrack().Probe(tissue->GetName() + "_Intracellular_CO2_mmHg", CO2->GetPartialPressure(PressureUnit::mmHg));

      //O2 = extracellular.GetSubstanceQuantity(*m_O2);
      //CO2 = extracellular.GetSubstanceQuantity(*m_CO2);
      //m_data.GetDataTrack().Probe(tissue->GetName() + "_Extracellular_O2_ug", O2->GetMass(MassUnit::ug));
      //m_data.GetDataTrack().Probe(tissue->GetName() + "_Extracellular_O2_ug_Per_mL", O2->GetConcentration(MassPerVolumeUnit::ug_Per_mL));
      ////m_data.GetDataTrack().Probe(tissue->GetName() + "_Extracellular_O2_mmHg", O2->GetPartialPressure(PressureUnit::mmHg));
      //m_data.GetDataTrack().Probe(tissue->GetName() + "_Extracellular_CO2_ug", CO2->GetMass(MassUnit::ug));
      //m_data.GetDataTrack().Probe(tissue->GetName() + "_Extracellular_CO2_ug_Per_mL", CO2->GetConcentration(MassPerVolumeUnit::ug_Per_mL));
      ////m_data.GetDataTrack().Probe(tissue->GetName() + "_Extracellular_CO2_mmHg", CO2->GetPartialPressure(PressureUnit::mmHg));

      //O2 = vascular->GetSubstanceQuantity(*m_O2);
      //CO2 = vascular->GetSubstanceQuantity(*m_CO2);
      //m_data.GetDataTrack().Probe(tissue->GetName() + "_Vascular_O2_ug", O2->GetMass(MassUnit::ug));
      //m_data.GetDataTrack().Probe(tissue->GetName() + "_Vascular_O2_ug_Per_mL", O2->GetConcentration(MassPerVolumeUnit::ug_Per_mL));
      ////m_data.GetDataTrack().Probe(tissue->GetName() + "_Vascular_O2_mmHg", O2->GetPartialPressure(PressureUnit::mmHg));
      //m_data.GetDataTrack().Probe(tissue->GetName() + "_Vascular_CO2_ug", CO2->GetMass(MassUnit::ug));
      //m_data.GetDataTrack().Probe(tissue->GetName() + "_Vascular_CO2_ug_Per_mL", CO2->GetConcentration(MassPerVolumeUnit::ug_Per_mL));
      ////m_data.GetDataTrack().Probe(tissue->GetName() + "_Vascular_CO2_mmHg", CO2->GetPartialPressure(PressureUnit::mmHg));
#endif

    }
    //m_data.GetDataTrack().Probe("Glucose_Consumed_mg", glucoseConsumed_mg);
    double oxygenConsumptionRate__mL_Per_s = oxygenConsumptionRate_g_Per_s / m_O2->GetDensity(MassPerVolumeUnit::g_Per_mL);
    double carbonDioxideProductionRate_mL_Per_s = carbonDioxideProductionRate_g_Per_s / m_CO2->GetDensity(MassPerVolumeUnit::g_Per_mL);
    GetOxygenConsumptionRate().SetValue(oxygenConsumptionRate__mL_Per_s, VolumePerTimeUnit::mL_Per_s);
    GetCarbonDioxideProductionRate().SetValue(carbonDioxideProductionRate_mL_Per_s, VolumePerTimeUnit::mL_Per_s);
    GetRespiratoryExchangeRatio().SetValue(carbonDioxideProductionRate_mL_Per_s / oxygenConsumptionRate__mL_Per_s);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Regulates blood glucose through movement into/out of the muscle and liver, and lipids into/out of fat.
  ///
  ///
  /// \param time
  /// The time step
  ///
  /// \details
  /// The blood glucose is controlled by driving blood glucose into the muscle tissue or removing it from the liver tissue.
  /// The rate of blood glucose transfer in each of these tissues is driven by the difference between the current blood
  /// glucose and the resting blood glucose. An insulin gain is used to modify the rate of glucose transfer into the muscle.
  /// A similar effect occurs with fatty acids. As insulin is released, this drives fatty acids into the adipose tissue. When
  /// fatty acids in the blood are low, they may be taken from the adipose pool.
  /// An improvement to this model is currently in development, and the exsiting model has been rendered inactive.
  //--------------------------------------------------------------------------------------------------
  void TissueModel::GlucoseLipidControl(double time_s)
  {
    double currentBloodGlucose_mg_Per_mL = m_data.GetSubstances().GetGlucose().GetBloodConcentration(MassPerVolumeUnit::mg_Per_mL);
    double currentBloodLipid_mg_Per_mL = m_data.GetSubstances().GetTristearin().GetBloodConcentration(MassPerVolumeUnit::mg_Per_mL);
    double currentInsulinConcentration_mg_Per_mL = m_data.GetSubstances().GetInsulin().GetBloodConcentration(MassPerVolumeUnit::mg_Per_mL);

    double bloodGlucoseDelta_mg_Per_mL = (currentBloodGlucose_mg_Per_mL - m_RestingBloodGlucose_mg_Per_mL);
    double bloodLipidDelta_mg_Per_mL = (currentBloodLipid_mg_Per_mL - m_RestingBloodLipid_mg_Per_mL);
    double insulinFeedback = currentInsulinConcentration_mg_Per_mL / m_RestingBloodInsulin_mg_Per_mL;
    double massDelta_mg = 0.0;

    double vascularVolume = 0.0;
    double extravascularVolume = 0.0;
    double transferTimeConstant_per_s = 0.1;

    /// \todo Switch to active transport methodology when ready
    if (bloodGlucoseDelta_mg_Per_mL >= 0.0) //Insulin is required to return blood glucose to normal values
    {
      //If blood glucose is above normal levels, the insulin response will drive glucose into the muscle tissue.
      //This process is limited by the transfer time constant, which is currently set to achieve adequate glucose regulation during resting conditions.
      /// \todo Determine better value for the transfer time constant from 24 hour glucose and insulin profiles
      vascularVolume = m_MuscleVascular->GetVolume().GetValue(VolumeUnit::mL);
      massDelta_mg = insulinFeedback * bloodGlucoseDelta_mg_Per_mL * transferTimeConstant_per_s * vascularVolume * time_s;
      if (massDelta_mg > m_MuscleVascularGlucose->GetMass(MassUnit::mg))
        massDelta_mg = m_MuscleVascularGlucose->GetMass(MassUnit::mg);
      m_MuscleVascularGlucose->GetMass().Increment(-massDelta_mg, MassUnit::mg);
      if (std::abs(m_MuscleVascularGlucose->GetMass(MassUnit::ug)) < ZERO_APPROX)
      {
        m_MuscleVascularGlucose->GetMass().SetValue(0.0, MassUnit::ug);
      }
      m_MuscleIntracellular->GetSubstanceQuantity(*m_Glucose)->GetMass().Increment(massDelta_mg, MassUnit::mg);
      if (std::abs(m_MuscleIntracellular->GetSubstanceQuantity(*m_Glucose)->GetMass(MassUnit::ug)) < ZERO_APPROX)
      {
        m_MuscleIntracellular->GetSubstanceQuantity(*m_Glucose)->GetMass().SetValue(0.0, MassUnit::ug);
      }
    }
    else
    {
      //If blood glucose is low, glucose will be pulled from the liver tissue into the vascular region. This is the equivalent of a glucagon response
      extravascularVolume = m_LiverIntracellular->GetVolume().GetValue(VolumeUnit::mL);
      massDelta_mg = bloodGlucoseDelta_mg_Per_mL * transferTimeConstant_per_s * extravascularVolume * time_s;
      if (massDelta_mg > m_LiverIntracellular->GetSubstanceQuantity(*m_Glucose)->GetMass(MassUnit::mg))
        massDelta_mg = m_LiverIntracellular->GetSubstanceQuantity(*m_Glucose)->GetMass(MassUnit::mg);
      m_LiverVascularGlucose->GetMass().Increment(-massDelta_mg, MassUnit::mg);
      if (std::abs(m_LiverVascularGlucose->GetMass(MassUnit::ug)) < ZERO_APPROX)
      {
        m_LiverVascularGlucose->GetMass().SetValue(0.0, MassUnit::ug);
      }
      m_LiverIntracellular->GetSubstanceQuantity(*m_Glucose)->GetMass().Increment(massDelta_mg, MassUnit::mg);
      if (std::abs(m_LiverIntracellular->GetSubstanceQuantity(*m_Glucose)->GetMass(MassUnit::ug)) < ZERO_APPROX)
      {
        m_LiverIntracellular->GetSubstanceQuantity(*m_Glucose)->GetMass().SetValue(0.0, MassUnit::ug);
      }
    }

    if (bloodLipidDelta_mg_Per_mL >= 0.0)
    {
      vascularVolume = m_FatVascular->GetVolume().GetValue(VolumeUnit::mL);
      massDelta_mg = insulinFeedback * bloodLipidDelta_mg_Per_mL * transferTimeConstant_per_s * vascularVolume * time_s;
      if (massDelta_mg > m_FatIntracellular->GetSubstanceQuantity(*m_Tristearin)->GetMass(MassUnit::mg))
        massDelta_mg = m_FatIntracellular->GetSubstanceQuantity(*m_Tristearin)->GetMass(MassUnit::mg);
      m_FatVascularLipid->GetMass().Increment(-massDelta_mg, MassUnit::mg);
      if (std::abs(m_FatVascularLipid->GetMass(MassUnit::ug)) < ZERO_APPROX)
      {
        m_FatVascularLipid->GetMass().SetValue(0.0, MassUnit::ug);
      }
      m_FatIntracellular->GetSubstanceQuantity(*m_Tristearin)->GetMass().Increment(massDelta_mg, MassUnit::mg);
      if (std::abs(m_FatIntracellular->GetSubstanceQuantity(*m_Tristearin)->GetMass(MassUnit::ug)) < ZERO_APPROX)
      {
        m_FatIntracellular->GetSubstanceQuantity(*m_Tristearin)->GetMass().SetValue(0.0, MassUnit::ug);
      }
    }
    else
    {
      vascularVolume = m_FatVascular->GetVolume().GetValue(VolumeUnit::mL);
      massDelta_mg = bloodLipidDelta_mg_Per_mL * transferTimeConstant_per_s * vascularVolume * time_s;
      if (massDelta_mg > m_FatIntracellular->GetSubstanceQuantity(*m_Tristearin)->GetMass(MassUnit::mg))
        massDelta_mg = m_FatIntracellular->GetSubstanceQuantity(*m_Tristearin)->GetMass(MassUnit::mg);
      m_FatVascularLipid->GetMass().Increment(-massDelta_mg, MassUnit::mg);
      if (std::abs(m_FatVascularLipid->GetMass(MassUnit::ug)) < ZERO_APPROX)
      {
        m_FatVascularLipid->GetMass().SetValue(0.0, MassUnit::ug);
      }
      m_FatIntracellular->GetSubstanceQuantity(*m_Tristearin)->GetMass().Increment(massDelta_mg, MassUnit::mg);
      if (std::abs(m_FatIntracellular->GetSubstanceQuantity(*m_Tristearin)->GetMass(MassUnit::ug)) < ZERO_APPROX)
      {
        m_FatIntracellular->GetSubstanceQuantity(*m_Tristearin)->GetMass().SetValue(0.0, MassUnit::ug);
      }
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Computes and sets patient system level data.
  ///
  ///
  /// \param time
  /// The time step
  ///
  /// \details
  /// All of the extravascular fluid, including extravascular-intracellular and extravascular-extracellular,
  /// is computed for total volume accounting.
  //--------------------------------------------------------------------------------------------------
  void TissueModel::CalculateVitals()
  {
    double extracellularFluidVolume_mL = 0.0;
    double intracellularFluidVolume_mL = 0.0;
    double currentFluidMass_kg = 0.0;
    SETissueCompartment* tissue;
    SELiquidCompartment* vascular;
    for (auto tissueVascular : m_TissueToVascular)
    {
      tissue = tissueVascular.first;
      vascular = tissueVascular.second;
      currentFluidMass_kg += vascular->GetVolume(VolumeUnit::mL) * m_data.GetBloodChemistry().GetBloodDensity(MassPerVolumeUnit::kg_Per_mL);
      currentFluidMass_kg += tissue->GetIntracellular().GetVolume(VolumeUnit::mL) * m_data.GetConfiguration().GetWaterDensity(MassPerVolumeUnit::kg_Per_mL);
      currentFluidMass_kg += tissue->GetExtracellular().GetVolume(VolumeUnit::mL) * m_data.GetConfiguration().GetWaterDensity(MassPerVolumeUnit::kg_Per_mL);
      extracellularFluidVolume_mL += tissue->GetExtracellular().GetVolume(VolumeUnit::mL);
      intracellularFluidVolume_mL += tissue->GetIntracellular().GetVolume(VolumeUnit::mL);
    }

    // Total Volumes
    double totalFluidVolume_mL = extracellularFluidVolume_mL + intracellularFluidVolume_mL + m_data.GetCardiovascular().GetBloodVolume(VolumeUnit::mL);
    GetExtracellularFluidVolume().SetValue(extracellularFluidVolume_mL, VolumeUnit::mL);
    GetIntracellularFluidVolume().SetValue(intracellularFluidVolume_mL, VolumeUnit::mL);
    GetExtravascularFluidVolume().SetValue(extracellularFluidVolume_mL + intracellularFluidVolume_mL, VolumeUnit::mL);
    GetTotalFluidVolume().SetValue(totalFluidVolume_mL, VolumeUnit::mL);

    //Patient weight decrease due to fluid mass lost from all sources
    double patientMassLost_kg = m_PreviousFluidMass_kg - currentFluidMass_kg;
    m_data.GetCurrentPatient().GetWeight().Increment(-patientMassLost_kg, MassUnit::kg);

    double restingPatientMass_kg = m_data.GetInitialPatient().GetWeight(MassUnit::kg);
    double massLossFraction = (m_RestingFluidMass_kg - currentFluidMass_kg) / restingPatientMass_kg;

    // Hydration Status
    // Give little buffers to prevent lots of flipping at inflection points
    if (massLossFraction > 0.10 ||
      (m_data.GetEvents().IsEventActive(eEvent::SevereDehydration) && massLossFraction > 0.095))
    {
      m_data.GetEvents().SetEvent(eEvent::MildDehydration, false, m_data.GetSimulationTime());
      m_data.GetEvents().SetEvent(eEvent::ModerateDehydration, false, m_data.GetSimulationTime());
      m_data.GetEvents().SetEvent(eEvent::SevereDehydration, true, m_data.GetSimulationTime());
    }
    else if (massLossFraction > 0.05 ||
      (m_data.GetEvents().IsEventActive(eEvent::ModerateDehydration) && massLossFraction > 0.045))
    {
      m_data.GetEvents().SetEvent(eEvent::MildDehydration, false, m_data.GetSimulationTime());
      m_data.GetEvents().SetEvent(eEvent::ModerateDehydration, true, m_data.GetSimulationTime());
      m_data.GetEvents().SetEvent(eEvent::SevereDehydration, false, m_data.GetSimulationTime());
    }
    else if (massLossFraction > 0.03 ||
      (m_data.GetEvents().IsEventActive(eEvent::MildDehydration) && massLossFraction > 0.025))
    {
      m_data.GetEvents().SetEvent(eEvent::MildDehydration, true, m_data.GetSimulationTime());
      m_data.GetEvents().SetEvent(eEvent::ModerateDehydration, false, m_data.GetSimulationTime());
      m_data.GetEvents().SetEvent(eEvent::SevereDehydration, false, m_data.GetSimulationTime());
    }
    else
    {
      m_data.GetEvents().SetEvent(eEvent::MildDehydration, false, m_data.GetSimulationTime());
      m_data.GetEvents().SetEvent(eEvent::ModerateDehydration, false, m_data.GetSimulationTime());
      m_data.GetEvents().SetEvent(eEvent::SevereDehydration, false, m_data.GetSimulationTime());
    }

    m_PreviousFluidMass_kg = currentFluidMass_kg;

    // Fasciculations (due to calcium deficiency) - Currently inactive for model improvement
    // The leading causes of fasciculation include magnesium deficiency, succinylcholine, nerve agents, and ALS.
    // Electrolyte imbalance may cause fasciculations.
    /*if (m_Muscleintracellular.GetSubstanceQuantity(*m_Calcium)->GetConcentration(MassPerVolumeUnit::g_Per_L) < 1.0)
    {
      m_data.GetPatient().SetEvent(ePatient_Event::Fasciculation, true, m_data.GetSimulationTime());
    }
    else if (m_Muscleintracellular.GetSubstanceQuantity(*m_Calcium)->GetConcentration(MassPerVolumeUnit::g_Per_L) > 3.0)
    {
      m_data.GetPatient().SetEvent(ePatient_Event::Fasciculation, false, m_data.GetSimulationTime());
    }*/
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Distributes mass to child compartments by volume-weighted distribution
  ///
  /// \param  cmpt  Parent compartment
  /// \param  sub   Substance being distribute.
  /// \param  mass  mass of substance to distribute.
  /// \param  unit  unit of mass.
  ///
  /// \details
  /// This method is intended to be used to distribute a mass INCREMENT amongst child compartments
  /// for transport between parent compartments, but it can be used for a decrement and for transport 
  /// between compartments that do not have children.
  //--------------------------------------------------------------------------------------------------
  void TissueModel::DistributeMassbyVolumeWeighted(SELiquidCompartment& cmpt, const SESubstance& sub, double mass, const MassUnit& unit)
  {
    if (mass == 0)
      return;
    SELiquidSubstanceQuantity* subQ = cmpt.GetSubstanceQuantity(sub);
    if (mass < 0.0 && -mass > subQ->GetMass(unit))
    {
      mass = -subQ->GetMass(unit);
      Info("The amount of mass decrement to distribute by volume weighted was greater than available. High probability of negative mass. DistributeMassbyMassWeighted is preferred for decrements.");
    }

    if (!cmpt.HasChildren())
    {
      subQ->GetMass().Increment(mass, unit);
      if (std::abs(subQ->GetMass(MassUnit::ug)) < ZERO_APPROX)
      {
        subQ->GetMass().SetValue(0.0, MassUnit::ug);
      }
    }
    else
    {
      double volume_mL = cmpt.GetVolume(VolumeUnit::mL);
      for (SELiquidCompartment* leaf : cmpt.GetLeaves())
      {
        double leafMass = mass * (leaf->GetVolume(VolumeUnit::mL) / volume_mL);
        SELiquidSubstanceQuantity* leafSubQ = leaf->GetSubstanceQuantity(sub);
        leafSubQ->GetMass().Increment(leafMass, unit);
        if (std::abs(leafSubQ->GetMass(MassUnit::ug)) < ZERO_APPROX)
        {
          leafSubQ->GetMass().SetValue(0.0, MassUnit::ug);
        }
      }
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Distributes mass to child compartments by mass-weighted distribution
  ///
  /// \param  cmpt  Parent compartment
  /// \param  sub   Substance being distribute.
  /// \param  mass  mass of substance to distribute.
  /// \param  unit  unit of mass.
  ///
  /// \details
  /// This method is intended to be used to distribute a mass DECREMENT amongst child compartments
  /// for transport between parent compartments, but it can be used for a increment and for transport 
  /// between compartments that do not have children.
  //--------------------------------------------------------------------------------------------------
  void TissueModel::DistributeMassbyMassWeighted(SELiquidCompartment& cmpt, const SESubstance& sub, double mass, const MassUnit& unit)
  {
    SELiquidSubstanceQuantity* subQ = cmpt.GetSubstanceQuantity(sub);
    if (mass < 0.0)
    {
      mass = -mass > subQ->GetMass(unit) ? -subQ->GetMass(unit) : mass;
    }

    if (!cmpt.HasChildren())
    {
      subQ->GetMass().Increment(mass, unit);
      if (std::abs(subQ->GetMass(MassUnit::ug)) < ZERO_APPROX)
      {
        subQ->GetMass().SetValue(0.0, MassUnit::ug);
      }
    }
    else
    {
      double mass_ug = subQ->GetMass(MassUnit::ug);
      for (SELiquidCompartment* leaf : cmpt.GetLeaves())
      {
        SELiquidSubstanceQuantity* leafSubQ = leaf->GetSubstanceQuantity(sub);
        double leafMass = 0.0;
        if (mass_ug != 0.0)
        {
          leafMass = mass * (leafSubQ->GetMass(MassUnit::ug) / mass_ug);
        }
        leafSubQ->GetMass().Increment(leafMass, unit);
        if (std::abs(leafSubQ->GetMass(MassUnit::ug)) < ZERO_APPROX)
        {
          leafSubQ->GetMass().SetValue(0.0, MassUnit::ug);
        }
      }
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates the mass transfer between tissue and vascular based on perfusion limited diffusion
  ///
  /// \param  acmpt  compartment for diffusion.
  /// \param  sub Substance being transferred.
  /// \param  fx  compartment effects for the given substance.
  /// \param  timestep Time step of the model.
  ///
  /// \details
  /// Uses the input compartment to find the corresponding vascular and tissue flow and concentration values to calculate diffusion. 
  /// MassDiffused = VascularFlow * dT * (VascularConcentration - TissueConcentration) / PartitionCoefficient
  /// Where TissueConcentration is the Intracelluar Substance Mass / Tissue Matrix Volume
  /// The mass is then updated on the vascular and tissue components of the compartment. 
  /// Concentration will be automatically recalculated when the compartment is refreshed.
  //--------------------------------------------------------------------------------------------------
  double TissueModel::PerfusionLimitedDiffusion(SETissueCompartment& tissue, SELiquidCompartment& vascular, const SESubstance& sub, double partitionCoeff, double timestep_s)
  {
    //Put an if statement here to check if tissue volume is nullptr, if so continue.

    SELiquidCompartment& intracellular = tissue.GetIntracellular();

    //Calculate Diffusion
    SELiquidSubstanceQuantity* vSubQ = vascular.GetSubstanceQuantity(sub);
    if (vSubQ == nullptr)
      throw CommonDataModelException("No Vascular Substance Quantity found for substance " + sub.GetName());
    double VascularFlow_m_LPer_s = vascular.GetInFlow(VolumePerTimeUnit::mL_Per_s);
    double VascularConcentration_ug_Per_mL = vSubQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL);

    SELiquidSubstanceQuantity* tSubQ = intracellular.GetSubstanceQuantity(sub);
    if (tSubQ == nullptr)
      throw CommonDataModelException("No Tissue-Intracellular Substance Quantity found for substance " + sub.GetName());
    SEScalarMassPerVolume tissueConcentration;
    if (!GeneralMath::CalculateConcentration(tSubQ->GetMass(), tissue.GetMatrixVolume(), tissueConcentration, m_Logger))
      Error("  Compartment : " + tissue.GetName() + ", Substance : " + sub.GetName());
    double TissueConcentration_ug_Per_mL = tissueConcentration.GetValue(MassPerVolumeUnit::ug_Per_mL);
    double MassIncrement_ug = 0;
    if (partitionCoeff != 0)
    {
      MassIncrement_ug = VascularFlow_m_LPer_s * timestep_s * (VascularConcentration_ug_Per_mL - (TissueConcentration_ug_Per_mL / partitionCoeff));
    }
    else
    {
      MassIncrement_ug = 0;
    }

    //Update the mass and concentration on the nodes
    if (MassIncrement_ug != 0)
    {
      // If it's going in, distribute by volume
      // If it's coming out, distribute by mass
        // If mass is exactly zero then going in by mass weighted won't work.
      if (MassIncrement_ug > 0.)
      {
        if (MassIncrement_ug > vSubQ->GetMass(MassUnit::ug))
          MassIncrement_ug = vSubQ->GetMass(MassUnit::ug);
        DistributeMassbyMassWeighted(vascular, sub, -MassIncrement_ug, MassUnit::ug);
        DistributeMassbyVolumeWeighted(intracellular, sub, MassIncrement_ug, MassUnit::ug);
      }
      else
      {
        if (-MassIncrement_ug > tSubQ->GetMass(MassUnit::ug))
          MassIncrement_ug = -tSubQ->GetMass(MassUnit::ug);
        DistributeMassbyVolumeWeighted(vascular, sub, -MassIncrement_ug, MassUnit::ug);
        DistributeMassbyMassWeighted(intracellular, sub, MassIncrement_ug, MassUnit::ug);
      }

      vSubQ->Balance(BalanceLiquidBy::Mass);
      tSubQ->Balance(BalanceLiquidBy::Mass);
    }
    return MassIncrement_ug;
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates the mass transfer due to a partial pressure gradient
  ///
  /// \param  Source pulmonary compartment for the mass transfer.
  /// \param  Target vascular compartment for the mass transfer.
  /// \param  sub Substance being transferred.
  /// \param  DiffusingCapacityO2_mL_Per_s_mmHg the current diffusion capacity of oxygen for this alveolar exchange region.
  /// \param  timestep Time step of the engine.
  ///
  /// \details
  /// Calculates the mass transfer from a pulmonary compartment/fluid to a vascular compartment/fluid 
  /// based on the partial pressure gradient of the gas in the two compartments/fluids.
  ///
  /// We cannot balance gas compartment substance quantities on a per substance basis like we can a liquid
  /// When the gas substance volume changes, the compartment volume changes as well, 
  /// which would then change the volume fraction for ALL the substances in the compartment.
  /// So it is up to the end user to call balance on the compartment when they are done updating substances on it.
  /// It would be inefficient to balance the compartment in this method, as this diffusion could be in a loop over all
  /// active substances, and we would be looping over all substance repeatedly.
  /// The alveolar exchange of any gas is computed from diffusion capacities relative to oxygen.
  //--------------------------------------------------------------------------------------------------
  void TissueModel::AlveolarPartialPressureGradientDiffusion(SEGasCompartment& pulmonary, SELiquidCompartment& vascular, SESubstance& sub, double DiffusingCapacityO2_mL_Per_s_mmHg, double timestep_s)
  {
    SEGasSubstanceQuantity* pSubQ = pulmonary.GetSubstanceQuantity(sub);
    SELiquidSubstanceQuantity* vSubQ = vascular.GetSubstanceQuantity(sub);
    if (pSubQ == nullptr || vSubQ == nullptr)
      throw CommonDataModelException("No Substance Quantity found for substance " + sub.GetName());

    double PressureGradient_mmHg = pSubQ->GetPartialPressure(PressureUnit::mmHg) - vSubQ->GetPartialPressure(PressureUnit::mmHg);

    double DiffusedVolume_mL = PressureGradient_mmHg * DiffusingCapacityO2_mL_Per_s_mmHg * sub.GetRelativeDiffusionCoefficient().GetValue() * timestep_s;

    //Get special diffused volume if sub is CO
    if (PressureGradient_mmHg > 0 && &sub == m_CO) // Wants to come into the blood
    {
      DiffusedVolume_mL = PressureGradient_mmHg * DiffusingCapacityO2_mL_Per_s_mmHg * sub.GetRelativeDiffusionCoefficient().GetValue() *
        (1.0 / (5.404e-05 * vascular.GetSubstanceQuantity(*m_O2)->GetPartialPressure(PressureUnit::mmHg) + 0.02885)) * timestep_s; //Modify the relative diffusion coefficient
    }
    double DiffusedMass_ug = DiffusedVolume_mL * sub.GetDensity(MassPerVolumeUnit::ug_Per_mL);

    //Check to make sure we're not moving more than exists
    if (SEScalar::IsZero(DiffusedVolume_mL, ZERO_APPROX))
    {
      DiffusedVolume_mL = 0.0;
      DiffusedMass_ug = 0.0;
    }
    else if (DiffusedVolume_mL > 0)
    {
      //It's moving from pulmonary to vascular
      double pVolume_mL = pSubQ->GetVolume(VolumeUnit::mL);
      if (DiffusedVolume_mL > pVolume_mL)
      {
        //Move all we can
        DiffusedVolume_mL = pVolume_mL;
        DiffusedVolume_mL *= 0.99; //Prevent numerical issues
        DiffusedMass_ug = DiffusedVolume_mL * sub.GetDensity(MassPerVolumeUnit::ug_Per_mL);
      }
    }
    else
    {
      //It's moving from vascular to pulmonary
      double vMass_ug = vSubQ->GetMass(MassUnit::ug);
      if (-DiffusedMass_ug > vMass_ug)
      {
        //Move all we can
        DiffusedMass_ug = -vMass_ug;
        DiffusedMass_ug *= 0.99; //Prevent numerical issues
        DiffusedVolume_mL = DiffusedMass_ug / sub.GetDensity(MassPerVolumeUnit::ug_Per_mL);
      }
    }

    pSubQ->GetVolume().Increment(-DiffusedVolume_mL, VolumeUnit::mL);
    sub.GetAlveolarTransfer().Increment(DiffusedVolume_mL / timestep_s, VolumePerTimeUnit::mL_Per_s);
    sub.GetDiffusingCapacity().Increment(DiffusingCapacityO2_mL_Per_s_mmHg * sub.GetRelativeDiffusionCoefficient().GetValue(), VolumePerTimePressureUnit::mL_Per_s_mmHg);

    vSubQ->GetMass().Increment(DiffusedMass_ug, MassUnit::ug);
    vSubQ->Balance(BalanceLiquidBy::Mass);
  }

  /// --------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates the mass transport of a substance between compartments by simple diffusion and updates other data after the mass increment
  ///
  /// \param source: source compartment
  /// \param target: target compartment
  /// \param sub: substance that is diffusing 
  /// \param permeabilityCofficient_mL_Per_s: the area-independent constant of proportionality
  /// \param timestep_s: the time step
  ///
  /// \details
  /// This method adjusts the mass in the source and target compartments using the concentration
  /// gradient based on the principles of simple diffusion. The method does not automatically update the concentration,
  /// molarity, partial pressure, and other data in the compartment following the mass increment.
  /// ***MUST CALL BALANCE TO UPDATE CONCENTRATION, MOLARITY, ETC.***
  /// Note that source and target are used to define a sign convention for readability only. the direction
  /// of mass flow is controlled entirely by the concentration gradient and is independent of which compartment
  /// is the source and which is the target.
  //--------------------------------------------------------------------------------------------------
  double TissueModel::MoveMassBySimpleDiffusion(SELiquidCompartment& source, SELiquidCompartment& target, const SESubstance& sub, double permeabilityCofficient_mL_Per_s, double timestep_s)
  {
    const SELiquidSubstanceQuantity* srcQ = source.GetSubstanceQuantity(sub);
    const SELiquidSubstanceQuantity* tgtQ = target.GetSubstanceQuantity(sub);

    double amountIncrement_ug = permeabilityCofficient_mL_Per_s * (srcQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL) - tgtQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL)) * timestep_s;
    // Need to limit the increment to what is available... just in case
    if (amountIncrement_ug > 0.0)
    {
      if (amountIncrement_ug > srcQ->GetMass(MassUnit::ug))
      {
        amountIncrement_ug = srcQ->GetMass(MassUnit::ug);
      }
      DistributeMassbyMassWeighted(source, sub, -amountIncrement_ug, MassUnit::ug);
      DistributeMassbyVolumeWeighted(target, sub, amountIncrement_ug, MassUnit::ug);
    }
    else // negative increment means it is coming from the target and going to the source
    {
      if (-amountIncrement_ug > tgtQ->GetMass(MassUnit::ug))
      {
        amountIncrement_ug = -tgtQ->GetMass(MassUnit::ug);
      }
      DistributeMassbyVolumeWeighted(source, sub, -amountIncrement_ug, MassUnit::ug);
      DistributeMassbyMassWeighted(target, sub, amountIncrement_ug, MassUnit::ug);
    }

    // Note we are only changing mass and NOT balancing on purpose
    // We don't want the concentrations changing until we have completed our 
    // diffusion methodology in its entirety 

    return amountIncrement_ug;
  } // End SimpleMassDiffusion

    /// --------------------------------------------------------------------------------------------------
    /// \brief
    /// Calculates the mass transport of a substance between compartments by instantaneous diffusion
    ///
    /// \param source: source compartment
    /// \param target: target compartment
    /// \param sub: substance that is diffusing 
    /// \param timestep_s: the time step
    ///
    /// \details
    /// Instantaneous diffusion assumes that the entire diffusion process happens within the bounds of a time step.
    //--------------------------------------------------------------------------------------------------
  double TissueModel::MoveMassByInstantDiffusion(SELiquidCompartment& source, SELiquidCompartment& target, const SESubstance& sub)
  {
    const SELiquidSubstanceQuantity* srcQ = source.GetSubstanceQuantity(sub);
    const SELiquidSubstanceQuantity* tgtQ = target.GetSubstanceQuantity(sub);

    double sConc_ug_Per_mL = srcQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL);
    double tConc_ug_Per_mL = tgtQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL);
    double sVol_mL = source.GetVolume(VolumeUnit::mL);
    double tVol_mL = target.GetVolume(VolumeUnit::mL);

    double amountIncrement_ug = (sConc_ug_Per_mL - tConc_ug_Per_mL) * (sVol_mL * tVol_mL) / (sVol_mL + tVol_mL);

    if (amountIncrement_ug > 0.0)
    {
      if (amountIncrement_ug > srcQ->GetMass(MassUnit::ug))
      {
        amountIncrement_ug = srcQ->GetMass(MassUnit::ug);
      }
      DistributeMassbyMassWeighted(source, sub, -amountIncrement_ug, MassUnit::ug);
      DistributeMassbyVolumeWeighted(target, sub, amountIncrement_ug, MassUnit::ug);
    }
    else // negative increment means it is coming from the target and going to the source
    {
      if (-amountIncrement_ug > tgtQ->GetMass(MassUnit::ug))
      {
        amountIncrement_ug = -tgtQ->GetMass(MassUnit::ug);
      }
      DistributeMassbyVolumeWeighted(source, sub, -amountIncrement_ug, MassUnit::ug);
      DistributeMassbyMassWeighted(target, sub, amountIncrement_ug, MassUnit::ug);
    }

    // Note we are only changing mass and NOT balancing on purpose
    // We don't want the concentrations changing until we have completed our 
    // diffusion methodology in its entirety 

    return amountIncrement_ug;
  }


  /// --------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates the mass transport of a substance between compartments by facilitated diffusion
  ///
  /// \param source: source compartment
  /// \param target: target compartment
  /// \param sub: substance that is diffusing 
  /// \param combinedCoefficient_g_Per_s: defines the maximum mass rate
  /// \param timestep_s: the time step
  ///
  /// This method adjusts the mass in the source and target compartments using the concentration
  /// gradient based on the principles of facilitated diffusion. The method does not automatically update the concentration,
  /// molarity, partial pressure, and other data in the compartment following the mass increment.
  /// ***MUST CALL BALANCE TO UPDATE CONCENTRATION, MOLARITY, ETC.***
  /// Note that source and target are used to define a sign convention for readability only. the direction
  /// of mass flow is controlled entirely by the concentration gradient and is independent of which compartment
  /// is the source and which is the target.
  //--------------------------------------------------------------------------------------------------
  double TissueModel::MoveMassByFacilitatedDiffusion(SELiquidCompartment& source, SELiquidCompartment& target, const SESubstance& sub, double combinedCoefficient_g_Per_s, double timestep_s)
  {
    const SELiquidSubstanceQuantity* sSubQ = source.GetSubstanceQuantity(sub);
    const SELiquidSubstanceQuantity* tSubQ = target.GetSubstanceQuantity(sub);

    double amountIncrement_g = combinedCoefficient_g_Per_s * (sSubQ->GetConcentration(MassPerVolumeUnit::g_Per_mL) - tSubQ->GetConcentration(MassPerVolumeUnit::g_Per_mL))
      / (sub.GetMichaelisCoefficient() + (sSubQ->GetConcentration(MassPerVolumeUnit::g_Per_mL) - tSubQ->GetConcentration(MassPerVolumeUnit::g_Per_mL))) * timestep_s;

    if (amountIncrement_g > 0.0)
    {
      if (amountIncrement_g > sSubQ->GetMass(MassUnit::g))
      {
        amountIncrement_g = sSubQ->GetMass(MassUnit::g);
      }
      DistributeMassbyMassWeighted(source, sub, -amountIncrement_g, MassUnit::g);
      DistributeMassbyVolumeWeighted(target, sub, amountIncrement_g, MassUnit::g);
    }
    else // negative increment means it is coming from the target and going to the source
    {
      if (-amountIncrement_g > tSubQ->GetMass(MassUnit::g))
      {
        amountIncrement_g = -tSubQ->GetMass(MassUnit::g);
      }
      DistributeMassbyVolumeWeighted(source, sub, -amountIncrement_g, MassUnit::g);
      DistributeMassbyMassWeighted(target, sub, amountIncrement_g, MassUnit::g);
    }
    // Note we are only changing mass and NOT balancing on purpose
    // We don't want the concentrations changing until we have completed our 
    // diffusion methodology in its entirety 

    return amountIncrement_g;
  } // End FacilitatedMassDiffusion

  /// --------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates the mass transport of a substance between compartments by active transport
  ///
  /// \param source: source compartment
  /// \param target: target compartment
  /// \param sub: substance that is diffusing 
  /// \param pumpRate_g_Per_s: the rate at which the pump is currently working
  /// \param timestep_s: the time step
  ///
  /// \details
  /// This is a simplified pumping mechanism. This single mechanism lumps together all of the 
  /// biological mechanisms which "pump" a substance across a membrane against an electrochemical
  /// or other gradient (all energy-requiring transport mechanisms). It computes a mass increment
  /// based on a pump rate and then increments the mass. TThe method does not automatically update the concentration,
  /// molarity, partial pressure, and other data in the compartment following the mass increment.
  /// ***MUST CALL BALANCE TO UPDATE CONCENTRATION, MOLARITY, ETC.***
  /// Note that the sign of the increment is determined by the source and target designation.
  //--------------------------------------------------------------------------------------------------
  double TissueModel::MoveMassByActiveTransport(SELiquidCompartment& source, SELiquidCompartment& target, const SESubstance& sub, double pumpRate_g_Per_s, double timestep_s)
  {
    const SELiquidSubstanceQuantity* srcQ = source.GetSubstanceQuantity(sub);
    const SELiquidSubstanceQuantity* tgtQ = target.GetSubstanceQuantity(sub);

    double amountIncrement_g = pumpRate_g_Per_s * timestep_s;

    if (amountIncrement_g > 0.0)
    {
      if (amountIncrement_g > srcQ->GetMass(MassUnit::g))
      {
        amountIncrement_g = srcQ->GetMass(MassUnit::g);
      }
      DistributeMassbyMassWeighted(source, sub, -amountIncrement_g, MassUnit::g);
      DistributeMassbyVolumeWeighted(target, sub, amountIncrement_g, MassUnit::g);
    }
    else // negative increment means it is coming from the target and going to the source
    {
      if (-amountIncrement_g > tgtQ->GetMass(MassUnit::g))
      {
        amountIncrement_g = -tgtQ->GetMass(MassUnit::g);
      }
      DistributeMassbyVolumeWeighted(source, sub, -amountIncrement_g, MassUnit::g);
      DistributeMassbyMassWeighted(target, sub, amountIncrement_g, MassUnit::g);
    }

    // Note we are only changing mass and NOT balancing on purpose
    // We don't want the concentrations changing until we have completed our 
    // diffusion methodology in its entirety 

    return amountIncrement_g;
  } // End ActiveMassTransport

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Apply dehydration condition
  ///
  /// \details
  /// Apply nearly isotonic dehydration from excessive sweating by looping through tissue vasculature,
  /// intracellular spaces, and extracellular spaces and removing the same fraction of liquid in each.
  /// Also, remove substances based on defined sweat compound concentrations.
  //--------------------------------------------------------------------------------------------------
  void TissueModel::Dehydration()
  {
    if (!m_data.GetConditions().HasDehydration())
      return;

    double severity = m_data.GetConditions().GetDehydration().GetSeverity().GetValue();

    double initialBodyWeight_kg = m_data.GetInitialPatient().GetWeight(MassUnit::kg);
    double bodyWeightReductionFraction = (13.614 * (severity * severity) + 0.3094 * severity) / 100.0; //Best fit curve
    double fluidMassReduction_kg = initialBodyWeight_kg * bodyWeightReductionFraction;

    double vascularVolume_mL = m_data.GetCardiovascular().GetBloodVolume(VolumeUnit::mL);
    double intracellularVolume_mL = GetIntracellularFluidVolume(VolumeUnit::mL);
    double extracellularVolume_mL = GetExtracellularFluidVolume(VolumeUnit::mL);

    //--------------------------------------------------------------------

    double vascularVolumeReduction_mL = 0.0;
    double intracellularVolumeReduction_mL = 0.0;
    double extracellularVolumeReduction_mL = 0.0;

    double vascularVolumeChangeFraction = 0.0;
    double intracellularVolumeChangeFraction = 0.0;
    double extracellularVolumeChangeFraction = 0.0;

    //Distribute the change accross all three tissue compartments
    //Just iterate instead of doing crazy arithmetic due to different initial volumes and densities
    double massIncrement_kg = 0.001;
    while (fluidMassReduction_kg > 0.0)
    {
      vascularVolumeChangeFraction = vascularVolumeReduction_mL / vascularVolume_mL;
      intracellularVolumeChangeFraction = intracellularVolumeReduction_mL/ intracellularVolume_mL;
      extracellularVolumeChangeFraction = extracellularVolumeReduction_mL / extracellularVolume_mL;

      // Find the minimum value and corresponding variable
      double min_value = std::min({ vascularVolumeChangeFraction, intracellularVolumeChangeFraction, extracellularVolumeChangeFraction });
      char min_var = 'a';
      if (min_value == intracellularVolumeChangeFraction) {
        min_var = 'b';
      }
      else if (min_value == extracellularVolumeChangeFraction) {
        min_var = 'c';
      }

      // Perform operation on the variable with the lowest value
      switch (min_var) {
      case 'a':
        vascularVolumeReduction_mL += massIncrement_kg / m_data.GetBloodChemistry().GetBloodDensity(MassPerVolumeUnit::kg_Per_mL);
        break;
      case 'b':
        intracellularVolumeReduction_mL += massIncrement_kg / m_data.GetConfiguration().GetWaterDensity(MassPerVolumeUnit::kg_Per_mL);
        break;
      case 'c':
        extracellularVolumeReduction_mL += massIncrement_kg / m_data.GetConfiguration().GetWaterDensity(MassPerVolumeUnit::kg_Per_mL);
        break;
      }

      fluidMassReduction_kg -= massIncrement_kg;
    }

    //These should all be nominally the same
    double vascularVolumeChangeMultiplier = 1.0 - vascularVolumeChangeFraction;
    double intracellularVolumeChangeMultiplier = 1.0 - intracellularVolumeChangeFraction;
    //double extracellularVolumeChangeMultiplier = 1.0 - extracellularVolumeChangeFraction;

    //Update all active cardiovascular circuit node volumes - this includes intravascular and extracellular nodes (not intracellular)
    for (auto node : m_data.GetCircuits().GetActiveCardiovascularCircuit().GetNodes())
    {
      if (node->HasNextVolume())
      {
        node->GetNextVolume().SetValue(vascularVolumeChangeMultiplier * node->GetNextVolume(VolumeUnit::mL), VolumeUnit::mL);

        //Handle manual node volume updates that occured during preprocessing by updating the pressure
        auto paths = m_data.GetCircuits().GetActiveCardiovascularCircuit().GetConnectedPaths(*node);
        for (unsigned int iter = 0; iter < paths->size(); iter++)
        {
          SEFluidCircuitPath* path = paths->at(iter);
          SEFluidCircuitNode* nSrc = &path->GetSourceNode();

          if (path->HasCompliance())
          {
            double nextVolume_mL = nSrc->GetNextVolume(VolumeUnit::mL);
            double currentVolume_mL = nSrc->GetVolume(VolumeUnit::mL);

            if (nextVolume_mL != currentVolume_mL)
            {
              double pressure_mmHg = nSrc->GetPressure(PressureUnit::mmHg);
              pressure_mmHg += (nextVolume_mL - currentVolume_mL) / path->GetCompliance(VolumePerPressureUnit::mL_Per_mmHg);
              nSrc->GetPressure().SetValue(pressure_mmHg, PressureUnit::mmHg);
              break; //Sometimes there are more than one compliance connected, so just do the first one (probably connected to ground/reference)
            }
          }
        }
      }
    }

    SETissueCompartment* tissue;
    SELiquidCompartment* vascular;
    for (auto tissueVascular : m_TissueToVascular)
    {
      tissue = tissueVascular.first;
      vascular = tissueVascular.second;

      //Update all intracellular compartment volumes (they have no circuit nodes)
      tissue->GetIntracellular().GetVolume().SetValue((intracellularVolumeChangeMultiplier) * tissue->GetIntracellular().GetVolume(VolumeUnit::mL), VolumeUnit::mL);

      //--------------------------------------------------------------------
      //Remove substances that are lost through perspiration
      double vascularPerspiredVolume_mL = vascularVolumeChangeFraction * vascular->GetVolume(VolumeUnit::mL);
      double intracellularPerspiredVolume_mL = intracellularVolumeChangeFraction * tissue->GetIntracellular().GetVolume(VolumeUnit::mL);
      double extracellularPerspiredVolume_mL = extracellularVolumeChangeFraction * tissue->GetExtracellular().GetVolume(VolumeUnit::mL);

      for (const SESubstanceConcentration* component : m_Sweat->GetComponents())
      {
        double concentration_g_Per_L = component->GetConcentration(MassPerVolumeUnit::g_Per_L);
        // Vasculature
        double vascularMassLost_g = concentration_g_Per_L * vascularPerspiredVolume_mL / 1000.0;
        SELiquidSubstanceQuantity* vascularQuantity = vascular->GetSubstanceQuantity(component->GetSubstance());
        vascularMassLost_g = MIN(vascularQuantity->GetMass(MassUnit::g), vascularMassLost_g);
        if (vascularQuantity->GetMass().IsReadOnly())
        {
          //TODO: Why doesn't this work?
          //quantity->GetMass().SetReadOnly(false);
          //quantity->GetMass().IncrementValue(-massLost_g, MassUnit::g);
          //quantity->GetMass().SetReadOnly(true);
        }
        else
        {
          vascularQuantity->GetMass().Increment(-vascularMassLost_g, MassUnit::g);
        }
        vascularQuantity->Balance(BalanceLiquidBy::Mass);

        // Intracellular
        double intracellularMassLost_g = concentration_g_Per_L * intracellularPerspiredVolume_mL / 1000.0;
        SELiquidSubstanceQuantity* intracellularQuantity = tissue->GetIntracellular().GetSubstanceQuantity(component->GetSubstance());
        intracellularMassLost_g = MIN(intracellularQuantity->GetMass(MassUnit::g), intracellularMassLost_g);
        intracellularQuantity->GetMass().Increment(-intracellularMassLost_g, MassUnit::g);
        intracellularQuantity->Balance(BalanceLiquidBy::Mass);

        // Extracellular
        double extracellularMassLost_g = concentration_g_Per_L * extracellularPerspiredVolume_mL / 1000.0;
        SELiquidSubstanceQuantity* extracellularQuantity = tissue->GetExtracellular().GetSubstanceQuantity(component->GetSubstance());
        extracellularMassLost_g = MIN(extracellularQuantity->GetMass(MassUnit::g), extracellularMassLost_g);
        extracellularQuantity->GetMass().Increment(-extracellularMassLost_g, MassUnit::g);
        extracellularQuantity->Balance(BalanceLiquidBy::Mass);

      }
    }
  }
END_NAMESPACE
