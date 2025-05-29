/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/PulseConfiguration.h"
#include "engine/common/controller/CompartmentManager.h"
#include "engine/common/controller/SubstanceManager.h"
#include "engine/common/system/physiology/BloodChemistryModel.h"

// Dependent Systems
#include "cdm/system/physiology/SECardiovascularSystem.h"
// Assessments
#include "cdm/patient/assessments/SEArterialBloodGasTest.h"
#include "cdm/patient/assessments/SECompleteBloodCount.h"
#include "cdm/patient/assessments/SEComprehensiveMetabolicPanel.h"
// CDM
#include "cdm/patient/SEPatient.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/engine/SEEventManager.h"
#include "cdm/compartment/fluid/SELiquidCompartment.h"
#include "cdm/compartment/fluid/SEGasCompartment.h"
#include "cdm/compartment/substances/SELiquidSubstanceQuantity.h"
#include "cdm/compartment/substances/SEGasSubstanceQuantity.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarAmountPerVolume.h"
#include "cdm/properties/SEScalarEquivalentWeightPerVolume.h"
#include "cdm/properties/SEScalarHeatCapacitancePerMass.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerAmount.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SERunningAverage.h"
#include "cdm/utils/DataTrack.h"

namespace pulse
{
  BloodChemistryModel::BloodChemistryModel(Data& data) : BloodChemistrySystem(data.GetLogger()), Model(data)
  {
    m_ArterialOxygen_mmHg = new SERunningAverage();
    m_ArterialCarbonDioxide_mmHg = new SERunningAverage();
    Clear();
  }

  BloodChemistryModel::~BloodChemistryModel()
  {
    Clear();
    delete m_ArterialOxygen_mmHg;
    delete m_ArterialCarbonDioxide_mmHg;
  }

  void BloodChemistryModel::Clear()
  {
    BloodChemistrySystem::Clear();
    m_Aorta = nullptr;
    m_AortaO2 = nullptr;
    m_AortaCO2 = nullptr;
    m_AortaHCO3 = nullptr;
    m_BrainO2 = nullptr;
    m_MyocardiumO2 = nullptr;
    m_PulmonaryArteries = nullptr;
    m_PulmonaryArteriesO2 = nullptr;
    m_PulmonaryArteriesCO2 = nullptr;
    m_PulmonaryVeins = nullptr;
    m_PulmonaryVeinsO2 = nullptr;
    m_PulmonaryVeinsCO2 = nullptr;
    m_LeftPulmonaryCapillaries = nullptr;
    m_LeftPulmonaryCapillariesO2 = nullptr;
    m_RightPulmonaryCapillaries = nullptr;
    m_RightPulmonaryCapillariesO2 = nullptr;
    m_RightArm = nullptr;
    m_RightArmCO = nullptr;
    m_RightArmO2 = nullptr;
    m_VenaCava = nullptr;
    m_VenaCavaO2 = nullptr;
    m_VenaCavaCO2 = nullptr;
    m_ArterialOxygen_mmHg->Invalidate();
    m_ArterialCarbonDioxide_mmHg->Invalidate();
    m_Alveoli = nullptr;
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Initializes system properties.
  ///
  /// \details
  /// For stabilization only!
  /// Called AFTER Setup when stabilizing a new patient
  //--------------------------------------------------------------------------------------------------
  void BloodChemistryModel::Initialize()
  {
    Model::Initialize();
    GetBloodDensity().SetValue(1050, MassPerVolumeUnit::kg_Per_m3);
    GetBloodSpecificHeat().SetValue(3617, HeatCapacitancePerMassUnit::J_Per_K_kg);
    GetVolumeFractionNeutralLipidInPlasma().SetValue(0.0023);
    GetVolumeFractionNeutralPhospholipidInPlasma().SetValue(0.0013);
    GetWhiteBloodCellCount().SetValue(7000, AmountPerVolumeUnit::ct_Per_uL);
    GetPhosphate().SetValue(1.1, AmountPerVolumeUnit::mmol_Per_L);
    GetStrongIonDifference().SetValue(40.5, AmountPerVolumeUnit::mmol_Per_L);

    GetAnatomicShuntFraction().SetValue(0);
    GetClinicalShuntFraction().SetValue(0);

    GetArterialOxygenContent().SetValue(0);
    GetMixedVenousOxygenContent().SetValue(0);
    GetArteriovenousOxygenDifference().SetValue(0);
    GetOxygenDelivery().SetValue(0, VolumePerTimeUnit::mL_Per_min);
    GetClinicalOxygenConsumption().SetValue(0, VolumePerTimeUnit::mL_Per_min);
    GetOxygenDeliveryToOxygenConsumptionRatio().SetValue(0);

    m_ArterialOxygen_mmHg->Sample(m_AortaO2->GetPartialPressure(PressureUnit::mmHg));
    m_ArterialCarbonDioxide_mmHg->Sample(m_AortaCO2->GetPartialPressure(PressureUnit::mmHg));

    GetCarbonMonoxideSaturation().SetValue(0);
    Process();// Calculate the initial system values
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Initializes parameters for BloodChemistry Class
  ///
  /// \details
  /// Called during both State loading and Patient Stabilization
  /// Pull and setup up our data (can be from other systems)
  /// Initialize will be called after this and can overwrite any of this data (only if stabilizing)
  //--------------------------------------------------------------------------------------------------
  void BloodChemistryModel::SetUp()
  {
    const PulseConfiguration& ConfigData = m_data.GetConfiguration();
    m_RedBloodCellVolume_mL = ConfigData.GetMeanCorpuscularVolume(VolumeUnit::mL);
    m_HbPerRedBloodCell_ug_Per_ct = ConfigData.GetMeanCorpuscularHemoglobin(MassPerAmountUnit::ug_Per_ct);

    m_Aorta = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Aorta);
    m_AortaO2 = m_Aorta->GetSubstanceQuantity(m_data.GetSubstances().GetO2());
    m_AortaCO2 = m_Aorta->GetSubstanceQuantity(m_data.GetSubstances().GetCO2());
    m_AortaHCO3 = m_Aorta->GetSubstanceQuantity(m_data.GetSubstances().GetHCO3());
    if(m_data.GetConfiguration().UseExpandedVasculature() == eSwitch::Off)
      m_BrainO2 = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Brain)->GetSubstanceQuantity(m_data.GetSubstances().GetO2());
    else
      m_BrainO2 = m_data.GetCompartments().GetLiquidCompartment(pulse::ExpandedVascularCompartment::Intracranial)->GetSubstanceQuantity(m_data.GetSubstances().GetO2());

    m_MyocardiumO2 = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Myocardium)->GetSubstanceQuantity(m_data.GetSubstances().GetO2());
    m_RightArm = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightArm);
    m_RightArmO2 = m_RightArm->GetSubstanceQuantity(m_data.GetSubstances().GetO2());
    m_VenaCava = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::VenaCava);
    m_VenaCavaO2 = m_VenaCava->GetSubstanceQuantity(m_data.GetSubstances().GetO2());
    m_VenaCavaCO2 = m_VenaCava->GetSubstanceQuantity(m_data.GetSubstances().GetCO2());

    m_PulmonaryArteries = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::PulmonaryArteries);
    m_PulmonaryArteriesO2 = m_PulmonaryArteries->GetSubstanceQuantity(m_data.GetSubstances().GetO2());
    m_PulmonaryArteriesCO2 = m_PulmonaryArteries->GetSubstanceQuantity(m_data.GetSubstances().GetCO2());

    m_PulmonaryVeins = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::PulmonaryVeins);
    m_PulmonaryVeinsO2 = m_PulmonaryVeins->GetSubstanceQuantity(m_data.GetSubstances().GetO2());
    m_PulmonaryVeinsCO2 = m_PulmonaryVeins->GetSubstanceQuantity(m_data.GetSubstances().GetCO2());

    m_LeftPulmonaryCapillaries = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftPulmonaryCapillaries);
    m_LeftPulmonaryCapillariesO2 = m_LeftPulmonaryCapillaries->GetSubstanceQuantity(m_data.GetSubstances().GetO2());
    m_RightPulmonaryCapillaries = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightPulmonaryCapillaries);
    m_RightPulmonaryCapillariesO2 = m_RightPulmonaryCapillaries->GetSubstanceQuantity(m_data.GetSubstances().GetO2());

    m_Alveoli = m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::Alveoli);
  }

  void BloodChemistryModel::AtSteadyState()
  {

  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Blood Chemistry Preprocess method
  ///
  /// \details
  /// The current Pulse implementation has no functionality in the preprocess function for blood chemistry.
  //--------------------------------------------------------------------------------------------------
  void BloodChemistryModel::PreProcess()
  {
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Blood Chemistry process method
  ///
  /// \details
  /// The system data (blood concentrations, blood gases, and other blood properties) that are calculated
  /// or changed by other systems are set on the blood chemistry system data objects. Events 
  /// are triggered at specific blood concentrations of certain substances in CheckBloodGasLevels().
  //--------------------------------------------------------------------------------------------------
  void BloodChemistryModel::Process(bool /*solve_and_transport*/)
  {
    //Push the compartment values of O2 and CO2 partial pressures on the corresponding system data.
    GetOxygenSaturation().Set(m_AortaO2->GetSaturation());
    GetCarbonDioxideSaturation().Set(m_AortaCO2->GetSaturation());
    
    if (m_data.GetEvents().IsEventActive(eEvent::CardiacArrest))
    {
      GetPulseOximetry().SetValue(0);
    }
    else
    {
      if (m_RightArmCO == nullptr && m_data.GetSubstances().IsActive(m_data.GetSubstances().GetCO()))
        m_RightArmCO = m_RightArm->GetSubstanceQuantity(m_data.GetSubstances().GetCO());
      if (m_RightArmCO != nullptr)
        GetPulseOximetry().SetValue(m_RightArmO2->GetSaturation().GetValue() + m_RightArmCO->GetSaturation().GetValue());
      else
        GetPulseOximetry().Set(m_RightArmO2->GetSaturation());
    }

    // This Hemoglobin Content is the mass of the hemoglobin only, not the hemoglobin and bound gas.
    // So we have to take our 4 Hb species masses and remove the mass of the gas.
    // Step 1) Get the mass of the bound species, which includes the mass of the bound gas.
    // Step 2) Convert to moles using the molar weight of the bound species (molar mass of bound species includes the mass of the bound gas and the mass of the unbound species).
    // Step 3) Covert moles of the bound species to moles of the unbound species. i.e.multiply by 1 (this step is implied)
    // Step 4) Convert moles of the unbound species to mass in order to get a total mass of hemoglobin (as opposed to a total mass of hemoglobin plus the bound gases).
    double totalHb_g = m_data.GetSubstances().GetSubstanceMass(m_data.GetSubstances().GetHb(), m_data.GetCompartments().GetVascularLeafCompartments(), MassUnit::g);
    double totalHbO2_g = (m_data.GetSubstances().GetSubstanceMass(m_data.GetSubstances().GetHbO2(), m_data.GetCompartments().GetVascularLeafCompartments(), MassUnit::g) / m_data.GetSubstances().GetHbO2().GetMolarMass(MassPerAmountUnit::g_Per_mol)) * m_data.GetSubstances().GetHb().GetMolarMass(MassPerAmountUnit::g_Per_mol);
    double totalHbCO2_g = (m_data.GetSubstances().GetSubstanceMass(m_data.GetSubstances().GetHbCO2(), m_data.GetCompartments().GetVascularLeafCompartments(), MassUnit::g) / m_data.GetSubstances().GetHbCO2().GetMolarMass(MassPerAmountUnit::g_Per_mol)) * m_data.GetSubstances().GetHb().GetMolarMass(MassPerAmountUnit::g_Per_mol);
    double totalHBO2CO2_g = (m_data.GetSubstances().GetSubstanceMass(m_data.GetSubstances().GetHbO2CO2(), m_data.GetCompartments().GetVascularLeafCompartments(), MassUnit::g) / m_data.GetSubstances().GetHbO2CO2().GetMolarMass(MassPerAmountUnit::g_Per_mol)) * m_data.GetSubstances().GetHb().GetMolarMass(MassPerAmountUnit::g_Per_mol);
    double totalHBCO_g = 0.0;
    if (m_RightArmCO != nullptr)
      totalHBCO_g = (m_data.GetSubstances().GetSubstanceMass(m_data.GetSubstances().GetHbCO(), m_data.GetCompartments().GetVascularLeafCompartments(), MassUnit::g) / m_data.GetSubstances().GetHbCO().GetMolarMass(MassPerAmountUnit::g_Per_mol)) * m_data.GetSubstances().GetHb().GetMolarMass(MassPerAmountUnit::g_Per_mol);

    double totalHemoglobin_g = totalHb_g + totalHbO2_g + totalHbCO2_g + totalHBO2CO2_g + totalHBCO_g;
    GetHemoglobinContent().SetValue(totalHemoglobin_g, MassUnit::g);

    // Calculate Blood Cell Counts
    double RedBloodCellCount_ct = GetHemoglobinContent(MassUnit::ug) / m_HbPerRedBloodCell_ug_Per_ct;
    double RedBloodCellVolume_mL = RedBloodCellCount_ct * m_RedBloodCellVolume_mL;
    double TotalBloodVolume_mL = m_data.GetCardiovascular().GetBloodVolume(VolumeUnit::mL);
    GetHematocrit().SetValue((RedBloodCellVolume_mL / TotalBloodVolume_mL));
    // Yes, we are giving GetRedBloodCellCount a concentration, because that is what it is, but clinically, it is known as RedBloodCellCount
    GetRedBloodCellCount().SetValue(RedBloodCellCount_ct / m_data.GetCardiovascular().GetBloodVolume(VolumeUnit::L), AmountPerVolumeUnit::ct_Per_L);
    GetPlasmaVolume().SetValue(TotalBloodVolume_mL - RedBloodCellVolume_mL, VolumeUnit::mL);

    /// \todo Change system data so that we have ArterialBloodPH (from aorta) and VenousBloodPH (from vena cava)
    // Calculate Base Excess
    double pH = m_Aorta->GetPH().GetValue();
    GetBloodPH().SetValue(pH);
    double HCO3_mEq_Per_L = m_AortaHCO3->GetMolarity(AmountPerVolumeUnit::mEq_Per_L);
    // http://www-users.med.cornell.edu/~spon/picu/calc/basecalc.htm
    double baseExcess = (0.9287 * HCO3_mEq_Per_L) + (13.77 * pH) - 124.58;
    GetBaseExcess().SetValue(baseExcess, AmountPerVolumeUnit::mEq_Per_L);


    // Pressures
    // arterial gas partial pressures
    GetArterialOxygenPressure().Set(m_AortaO2->GetPartialPressure());
    GetArterialCarbonDioxidePressure().Set(m_AortaCO2->GetPartialPressure());
    // pulmonary arteries
    GetPulmonaryArterialOxygenPressure().Set(m_PulmonaryArteriesO2->GetPartialPressure());
    GetPulmonaryArterialCarbonDioxidePressure().Set(m_PulmonaryArteriesCO2->GetPartialPressure());
    // pulmonary vein gas partial pressures -  the average of right and left pulmonary vein gas pressures
    GetPulmonaryVenousOxygenPressure().Set(m_PulmonaryVeinsO2->GetPartialPressure());
    GetPulmonaryVenousCarbonDioxidePressure().Set(m_PulmonaryVeinsCO2->GetPartialPressure());
    // venous gas partial pressures
    GetVenousOxygenPressure().Set(m_VenaCavaO2->GetPartialPressure());
    GetVenousCarbonDioxidePressure().Set(m_VenaCavaCO2->GetPartialPressure());

    double totalFlow_mL_Per_min = m_data.GetCardiovascular().GetCardiacOutput(VolumePerTimeUnit::mL_Per_min);
    double shuntFlow_mL_Per_min = m_data.GetCardiovascular().GetPulmonaryMeanShuntFlow(VolumePerTimeUnit::mL_Per_min);
    double shunt = 0.0;
    if (totalFlow_mL_Per_min > 0.0)
    {
      shunt = shuntFlow_mL_Per_min / totalFlow_mL_Per_min;
    }
    shunt = MIN(shunt, 1.0);
    GetAnatomicShuntFraction().SetValue(shunt);

    // Calculate the Clinical Shunt Fraction (Qs/Qt) based on the pulmonary compartments oxygen content
    // We're going to use the capillaries value from the side with the highest value to account for heterogenous insults 
    // (e.g., mainstem intubation, pneumothorax, etc.)

    // Get pulmonary capillaries O2 quantities (only the part that participates in gas exchange)
    double rightPulmonaryCapillariesO2_mmol_Per_L = m_RightPulmonaryCapillariesO2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
    double rightPulmonaryCapillariesHbO2_mmol_Per_L = m_RightPulmonaryCapillaries->GetSubstanceQuantity(m_data.GetSubstances().GetHbO2())->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
    double rightPulmonaryCapillariesHbO2CO2_mmol_Per_L = m_RightPulmonaryCapillaries->GetSubstanceQuantity(m_data.GetSubstances().GetHbO2CO2())->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
    double rightCapillaryO2Content_mmol_Per_L = rightPulmonaryCapillariesO2_mmol_Per_L + 4.0 * rightPulmonaryCapillariesHbO2_mmol_Per_L + 4.0 * rightPulmonaryCapillariesHbO2CO2_mmol_Per_L;

    double leftPulmonaryCapillariesO2_mmol_Per_L = m_LeftPulmonaryCapillariesO2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
    double leftPulmonaryCapillariesHbO2_mmol_Per_L = m_LeftPulmonaryCapillaries->GetSubstanceQuantity(m_data.GetSubstances().GetHbO2())->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
    double leftPulmonaryCapillariesHbO2CO2_mmol_Per_L = m_LeftPulmonaryCapillaries->GetSubstanceQuantity(m_data.GetSubstances().GetHbO2CO2())->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
    double leftCapillaryO2Content_mmol_Per_L = leftPulmonaryCapillariesO2_mmol_Per_L + 4.0 * leftPulmonaryCapillariesHbO2_mmol_Per_L + 4.0 * leftPulmonaryCapillariesHbO2CO2_mmol_Per_L;

    // Get pulmonary arteries O2 quantities
    double pulmonaryArteriesO2_mmol_Per_L = m_PulmonaryArteries->GetSubstanceQuantity(m_data.GetSubstances().GetO2())->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
    double pulmonaryArteriesHbO2_mmol_Per_L = m_PulmonaryArteries->GetSubstanceQuantity(m_data.GetSubstances().GetHbO2())->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
    double pulmonaryArteriesHbO2CO2_mmol_Per_L = m_PulmonaryArteries->GetSubstanceQuantity(m_data.GetSubstances().GetHbO2CO2())->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);

    // Get pulmonary veins O2 quantities
    double pulmonaryVeinsO2_mmol_Per_L = m_PulmonaryVeins->GetSubstanceQuantity(m_data.GetSubstances().GetO2())->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
    double pulmonaryVeinsHbO2_mmol_Per_L = m_PulmonaryVeins->GetSubstanceQuantity(m_data.GetSubstances().GetHbO2())->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
    double pulmonaryVeinsHbO2CO2_mmol_Per_L = m_PulmonaryVeins->GetSubstanceQuantity(m_data.GetSubstances().GetHbO2CO2())->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);

    // Compute CcO2 - Pulmonary Capillary O2 Content
    double capillaryO2Content_mmol_Per_L = MAX(rightCapillaryO2Content_mmol_Per_L, leftCapillaryO2Content_mmol_Per_L);
    // Compute CaO2 - Arterial O2 Content (from pulmonary veins)
    double arterialO2Content_mmol_Per_L = pulmonaryVeinsO2_mmol_Per_L + 4.0 * pulmonaryVeinsHbO2_mmol_Per_L + 4.0 * pulmonaryVeinsHbO2CO2_mmol_Per_L;
    // Compute CvO2 - Venous O2 Content (from pulmonary arteries)
    double venousO2Content_mmol_Per_L = pulmonaryArteriesO2_mmol_Per_L + 4.0 * pulmonaryArteriesHbO2_mmol_Per_L + 4.0 * pulmonaryArteriesHbO2CO2_mmol_Per_L;

    // Compute Clinical Shunt Fraction (Qs/Qt)
    double clinicalShuntFraction = 
      (capillaryO2Content_mmol_Per_L - arterialO2Content_mmol_Per_L) / 
      (capillaryO2Content_mmol_Per_L - venousO2Content_mmol_Per_L);
    clinicalShuntFraction = LIMIT(clinicalShuntFraction, 0.0, 1.0);

    //Dampen the change to prevent oscillations
    double previousClinicalShuntFraction = GetClinicalShuntFraction().GetValue();
    double dampenFraction_perSec = 0.01 * 50.0;
    clinicalShuntFraction = GeneralMath::Damper(clinicalShuntFraction, previousClinicalShuntFraction, dampenFraction_perSec, m_data.GetTimeStep_s());

    GetClinicalShuntFraction().SetValue(clinicalShuntFraction);

    CheckBloodSubstanceLevels();

    // Total up all active substances
    double bloodMass_ug;
    double tissueMass_ug;
    for (SESubstance* sub : m_data.GetSubstances().GetActiveSubstances())
    {
      bloodMass_ug = m_data.GetSubstances().GetSubstanceMass(*sub, m_data.GetCompartments().GetVascularLeafCompartments(), MassUnit::ug);
      tissueMass_ug = m_data.GetSubstances().GetSubstanceMass(*sub, m_data.GetCompartments().GetTissueLeafCompartments(), MassUnit::ug);
      sub->GetMassInBody().SetValue(bloodMass_ug + tissueMass_ug, MassUnit::ug);
      sub->GetMassInBlood().SetValue(bloodMass_ug, MassUnit::ug);
      sub->GetMassInTissue().SetValue(tissueMass_ug, MassUnit::ug);
      sub->GetBloodConcentration().SetValue(bloodMass_ug / TotalBloodVolume_mL, MassPerVolumeUnit::ug_Per_mL);
    }
    // Compute Special Concentrations
    GetBloodUreaNitrogenConcentration().SetValue(m_data.GetSubstances().GetUrea().GetBloodConcentration(MassPerVolumeUnit::ug_Per_mL) / 2.14, MassPerVolumeUnit::ug_Per_mL);
    double albuminConcentration_ug_Per_mL = m_data.GetSubstances().GetAlbumin().GetBloodConcentration(MassPerVolumeUnit::ug_Per_mL);
    m_data.GetSubstances().GetGlobulin().GetBloodConcentration().SetValue(albuminConcentration_ug_Per_mL * 1.6 - albuminConcentration_ug_Per_mL, MassPerVolumeUnit::ug_Per_mL);
    double HemoglobinConcentration = totalHemoglobin_g / TotalBloodVolume_mL;
    m_data.GetSubstances().GetHb().GetBloodConcentration().SetValue(HemoglobinConcentration, MassPerVolumeUnit::g_Per_mL);
    GetTotalProteinConcentration().SetValue(albuminConcentration_ug_Per_mL * 1.6, MassPerVolumeUnit::ug_Per_mL);
    // 1.6 comes from reading http://www.drkaslow.com/html/proteins_-_albumin-_globulins-_etc.html
    ComputeExposedModelParameters();

    // Compute plasma osmolality and osmolarity by looking at the aorta
    // Specific gravity is calculated by dividing the mass of all the substances in a fluid & the fluid itself by the weight of just the fluid
    SEScalarMass substanceMass;
    for (SELiquidSubstanceQuantity* subQ : m_Aorta->GetSubstanceQuantities())
    {
      if (subQ->HasMass())
        substanceMass.Increment(subQ->GetMass());
    }

    // Increment water mass onto substance mass to get total mass:
    SEScalar specificGravity;
    if (!GeneralMath::CalculateSpecificGravity(substanceMass, m_Aorta->GetVolume(), specificGravity))
      Error("Unable to calculate specific gravity of aorta substances");

    // Osmolality is the osmotic pressure of sodium, glucose and urea over the weight of the fluid
    GeneralMath::CalculateOsmolality(m_Aorta->GetSubstanceQuantity(m_data.GetSubstances().GetSodium())->GetMolarity(),
      m_Aorta->GetSubstanceQuantity(m_data.GetSubstances().GetPotassium())->GetMolarity(),
      m_Aorta->GetSubstanceQuantity(m_data.GetSubstances().GetGlucose())->GetMolarity(),
      m_Aorta->GetSubstanceQuantity(m_data.GetSubstances().GetUrea())->GetMolarity(),
      specificGravity,
      GetPlasmaOsmolality());

    GeneralMath::CalculateOsmolarity(m_Aorta->GetSubstanceQuantity(m_data.GetSubstances().GetSodium())->GetMolarity(),
      m_Aorta->GetSubstanceQuantity(m_data.GetSubstances().GetPotassium())->GetMolarity(),
      m_Aorta->GetSubstanceQuantity(m_data.GetSubstances().GetGlucose())->GetMolarity(),
      m_Aorta->GetSubstanceQuantity(m_data.GetSubstances().GetUrea())->GetMolarity(), 
      GetPlasmaOsmolarity());


    // Molar volume of an ideal gas at NTP is 24.04 L_Per_mol
    double mmol_To_mL = 24.04; // mL O2 per mmol O2

    double aortaO2_mmol_Per_L = m_AortaO2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
    double aortaHbO2_mmol_Per_L = m_Aorta->GetSubstanceQuantity(m_data.GetSubstances().GetHbO2())->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
    double aortaHbO2CO2_mmol_Per_L = m_Aorta->GetSubstanceQuantity(m_data.GetSubstances().GetHbO2CO2())->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
    //CaO2
    arterialO2Content_mmol_Per_L = aortaO2_mmol_Per_L + 4.0 * aortaHbO2_mmol_Per_L + 4.0 * aortaHbO2CO2_mmol_Per_L;
    double arterialO2Content_mL_Per_dL = arterialO2Content_mmol_Per_L * mmol_To_mL / 10.0;

    double venaCavaO2_mmol_Per_L = m_VenaCavaO2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
    double venaCavaHbO2_mmol_Per_L = m_VenaCava->GetSubstanceQuantity(m_data.GetSubstances().GetHbO2())->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
    double venaCavaHbO2CO2_mmol_Per_L = m_VenaCava->GetSubstanceQuantity(m_data.GetSubstances().GetHbO2CO2())->GetMolarity(AmountPerVolumeUnit::mmol_Per_L);
    //CvO2
    venousO2Content_mmol_Per_L = venaCavaO2_mmol_Per_L + 4.0 * venaCavaHbO2_mmol_Per_L + 4.0 * venaCavaHbO2CO2_mmol_Per_L;
    double venousO2Content_mL_Per_dL = venousO2Content_mmol_Per_L * mmol_To_mL / 10.0;

    //CO
    double cardiacOutput_L_Per_min = m_data.GetCardiovascular().GetCardiacOutput(VolumePerTimeUnit::L_Per_min);
    double cardiacOutput_dL_Per_min = cardiacOutput_L_Per_min * 10.0;

    // a-vO2 diff (mL O2 / dL blood)
    double arteriovenousO2Difference_mL_Per_dL = arterialO2Content_mL_Per_dL - venousO2Content_mL_Per_dL;

    // VO2 = CO x (CaO2 - CvO2)
    double O2Consumption_mL_Per_min = cardiacOutput_dL_Per_min * arteriovenousO2Difference_mL_Per_dL;
    // DO2 = CO x CaO2
    double O2Delivery_mL_Per_min = cardiacOutput_dL_Per_min * arterialO2Content_mL_Per_dL;
    // DO2/VO2 ratio (unitless)
    double O2DeliveryToO2ConsumptionRatio = O2Delivery_mL_Per_min / O2Consumption_mL_Per_min;

    GetArterialOxygenContent().SetValue(arterialO2Content_mL_Per_dL * 0.01); //Make unitless
    GetMixedVenousOxygenContent().SetValue(venousO2Content_mL_Per_dL * 0.01); //Make unitless
    GetArteriovenousOxygenDifference().SetValue(arteriovenousO2Difference_mL_Per_dL * 0.01); //Make unitless
    GetOxygenDelivery().SetValue(O2Delivery_mL_Per_min, VolumePerTimeUnit::mL_Per_min);
    GetClinicalOxygenConsumption().SetValue(O2Consumption_mL_Per_min, VolumePerTimeUnit::mL_Per_min);
    GetOxygenDeliveryToOxygenConsumptionRatio().SetValue(O2DeliveryToO2ConsumptionRatio);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Blood Chemistry postprocess method
  ///
  /// \details
  /// The current Pulse implementation has no specific postprocess functionality.
  //--------------------------------------------------------------------------------------------------
  void BloodChemistryModel::PostProcess(bool /*solve_and_transport*/)
  {

  }

  void BloodChemistryModel::ComputeExposedModelParameters()
  {

  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Checks the blood gas (oxygen, carbon dioxide) and other substance levels and sets events.
  ///
  /// \details
  /// Checks the oxygen and carbon dioxide levels in specific compartments of the body. 
  /// Set events for hypercapnia, Hypoxia, Brain, Oxygen Deficit, and MyoCardium Oxygen Deficit.
  /// Also checks sodium levels and sets the hyponatremia and hypernatremia events.
  //--------------------------------------------------------------------------------------------------
  void BloodChemistryModel::CheckBloodSubstanceLevels()
  {
    m_ArterialOxygen_mmHg->Sample(m_AortaO2->GetPartialPressure(PressureUnit::mmHg));
    m_ArterialCarbonDioxide_mmHg->Sample(m_AortaCO2->GetPartialPressure(PressureUnit::mmHg));
    //Only check these at the end of a cardiac cycle and reset at start of cardiac cycle 
    if (m_data.GetEvents().IsEventActive(eEvent::StartOfCardiacCycle))
    {
      if (m_data.GetState() == EngineState::Active)
      {// Don't throw events if we are initializing
        double arterialOxygen_mmHg = m_ArterialOxygen_mmHg->Value();
        double arterialCarbonDioxide_mmHg = m_ArterialCarbonDioxide_mmHg->Value();
        // hypercapnia check
        double hypercapniaFlag = 60.0; // \cite Guyton11thEd p.531 
        if (arterialCarbonDioxide_mmHg >= hypercapniaFlag)
        {
          m_data.GetEvents().SetEvent(eEvent::Hypercapnia, true, m_data.GetSimulationTime());

        }
        else if (m_data.GetEvents().IsEventActive(eEvent::Hypercapnia) && arterialCarbonDioxide_mmHg < (hypercapniaFlag - 3))
        {
          /// This event is triggered if the patient was hypercapnic and is now considered to be recovered.
          m_data.GetEvents().SetEvent(eEvent::Hypercapnia, false, m_data.GetSimulationTime());
        }

        // hypoxia check
        double hypoxiaFlag = 65.0; //Arterial O2 Partial Pressure in mmHg \cite Pierson2000Pathophysiology
        if (arterialOxygen_mmHg <= hypoxiaFlag)
        {
          m_data.GetEvents().SetEvent(eEvent::Hypoxia, true, m_data.GetSimulationTime());
        }
        else if (arterialOxygen_mmHg > (hypoxiaFlag + 3))
        {
          /// The patient is no longer considered to be hypoxic.
          m_data.GetEvents().SetEvent(eEvent::Hypoxia, false, m_data.GetSimulationTime());
        }

        // hyperoxemia check
        double hyperoxemiaModerateFlag = 120.0; //Arterial O2 Partial Pressure in mmHg
        double hyperoxemiaSevereFlag = 200.0; //Arterial O2 Partial Pressure in mmHg
        if (arterialOxygen_mmHg > hyperoxemiaSevereFlag)
        {
          m_data.GetEvents().SetEvent(eEvent::SevereHyperoxemia, true, m_data.GetSimulationTime());
          m_data.GetEvents().SetEvent(eEvent::ModerateHyperoxemia, false, m_data.GetSimulationTime());
        }
        else if (arterialOxygen_mmHg > hyperoxemiaModerateFlag)
        {
          m_data.GetEvents().SetEvent(eEvent::SevereHyperoxemia, false, m_data.GetSimulationTime());
          m_data.GetEvents().SetEvent(eEvent::ModerateHyperoxemia, true, m_data.GetSimulationTime());
        }
        else if (arterialOxygen_mmHg < hyperoxemiaModerateFlag - 3)
        {
          m_data.GetEvents().SetEvent(eEvent::SevereHyperoxemia, false, m_data.GetSimulationTime());
          m_data.GetEvents().SetEvent(eEvent::ModerateHyperoxemia, false, m_data.GetSimulationTime());
        }

        // hypocapnia check
        double hypocapniaModerateFlag = 30.0; //Arterial CO2 Partial Pressure in mmHg
        double hypocapniaSevereFlag = 15.0; //Arterial CO2 Partial Pressure in mmHg
        if (arterialOxygen_mmHg < hypocapniaSevereFlag)
        {
          m_data.GetEvents().SetEvent(eEvent::SevereHypocapnia, true, m_data.GetSimulationTime());
          m_data.GetEvents().SetEvent(eEvent::ModerateHypocapnia, false, m_data.GetSimulationTime());
        }
        else if (arterialOxygen_mmHg < hypocapniaModerateFlag)
        {
          m_data.GetEvents().SetEvent(eEvent::SevereHypocapnia, false, m_data.GetSimulationTime());
          m_data.GetEvents().SetEvent(eEvent::ModerateHypocapnia, true, m_data.GetSimulationTime());
        }
        else if (arterialOxygen_mmHg > hypocapniaModerateFlag + 3)
        {
          m_data.GetEvents().SetEvent(eEvent::SevereHypocapnia, false, m_data.GetSimulationTime());
          m_data.GetEvents().SetEvent(eEvent::ModerateHypocapnia, false, m_data.GetSimulationTime());
        }

        // sodium check
        double sodiumMolarity_mEq_Per_L = m_Aorta->GetSubstanceQuantity(m_data.GetSubstances().GetSodium())->GetMolarity(AmountPerVolumeUnit::mEq_Per_L);

        // Give little buffers to prevent lots of flipping at inflection points
        double buffer_mEq_Per_L = 1.0;

        if (sodiumMolarity_mEq_Per_L < 135.0 || ///\cite rondon2017hyponatremia
          (m_data.GetEvents().IsEventActive(eEvent::Hyponatremia) && sodiumMolarity_mEq_Per_L < 135.0 + buffer_mEq_Per_L))
        {
          m_data.GetEvents().SetEvent(eEvent::Hyponatremia, true, m_data.GetSimulationTime());
          m_data.GetEvents().SetEvent(eEvent::Hypernatremia, false, m_data.GetSimulationTime());
        }
        else if (sodiumMolarity_mEq_Per_L > 145.0 || ///\cite Lewis2023Hypernatremia
          (m_data.GetEvents().IsEventActive(eEvent::Hypernatremia) && sodiumMolarity_mEq_Per_L > 145.0 - buffer_mEq_Per_L))
        {
          m_data.GetEvents().SetEvent(eEvent::Hyponatremia, false, m_data.GetSimulationTime());
          m_data.GetEvents().SetEvent(eEvent::Hypernatremia, true, m_data.GetSimulationTime());
        }
        else
        {
          m_data.GetEvents().SetEvent(eEvent::Hyponatremia, false, m_data.GetSimulationTime());
          m_data.GetEvents().SetEvent(eEvent::Hypernatremia, false, m_data.GetSimulationTime());
        }

        double bloodGlucoseConcentration_mg_Per_dL = m_data.GetSubstances().GetGlucose().GetBloodConcentration(MassPerVolumeUnit::mg_Per_dL);
        if (bloodGlucoseConcentration_mg_Per_dL > 180.0)
        {
          m_data.GetEvents().SetEvent(eEvent::Hyperglycemia, true, m_data.GetSimulationTime());
          m_data.GetEvents().SetEvent(eEvent::Hypoglycemia, false, m_data.GetSimulationTime());
        }
        else if (bloodGlucoseConcentration_mg_Per_dL < 70.0)
        {
          m_data.GetEvents().SetEvent(eEvent::Hyperglycemia, false, m_data.GetSimulationTime());
          m_data.GetEvents().SetEvent(eEvent::Hypoglycemia, true, m_data.GetSimulationTime());
        }
        else if (m_data.GetEvents().IsEventActive(eEvent::Hyperglycemia) && bloodGlucoseConcentration_mg_Per_dL < 178.0)
        {
          m_data.GetEvents().SetEvent(eEvent::Hyperglycemia, false, m_data.GetSimulationTime());
        }
        else if (m_data.GetEvents().IsEventActive(eEvent::Hypoglycemia) && bloodGlucoseConcentration_mg_Per_dL >  72.0)
        {
          m_data.GetEvents().SetEvent(eEvent::Hypoglycemia, false, m_data.GetSimulationTime());
        }

        if (bloodGlucoseConcentration_mg_Per_dL > 250.0 && m_data.GetEvents().IsEventActive(eEvent::MetabolicAcidosis))
        {
          m_data.GetEvents().SetEvent(eEvent::Ketoacidosis, true, m_data.GetSimulationTime());
        }
        else if (m_data.GetEvents().IsEventActive(eEvent::Ketoacidosis) && !m_data.GetEvents().IsEventActive(eEvent::MetabolicAcidosis))
        {
          m_data.GetEvents().SetEvent(eEvent::Ketoacidosis, false, m_data.GetSimulationTime());
        }
        else if (m_data.GetEvents().IsEventActive(eEvent::Ketoacidosis) && bloodGlucoseConcentration_mg_Per_dL < 240.0)
        {
          m_data.GetEvents().SetEvent(eEvent::Ketoacidosis, false, m_data.GetSimulationTime());
        }

        double bloodLactateConcentration_mg_Per_dL = m_data.GetSubstances().GetLactate().GetBloodConcentration(MassPerVolumeUnit::mg_Per_dL);
        if (bloodLactateConcentration_mg_Per_dL > 36.04 && m_data.GetEvents().IsEventActive(eEvent::MetabolicAcidosis)) // > 4 mmol/L
        {
          m_data.GetEvents().SetEvent(eEvent::LacticAcidosis, true, m_data.GetSimulationTime());
        }
        else if (m_data.GetEvents().IsEventActive(eEvent::LacticAcidosis) && !m_data.GetEvents().IsEventActive(eEvent::MetabolicAcidosis))
        {
          m_data.GetEvents().SetEvent(eEvent::LacticAcidosis, false, m_data.GetSimulationTime());
        }
        else if (m_data.GetEvents().IsEventActive(eEvent::LacticAcidosis) && bloodLactateConcentration_mg_Per_dL < 35.04)
        {
          m_data.GetEvents().SetEvent(eEvent::LacticAcidosis, false, m_data.GetSimulationTime());
        }
      }

      m_ArterialOxygen_mmHg->Invalidate();
      m_ArterialCarbonDioxide_mmHg->Invalidate();
    }


    if (m_data.GetState() > EngineState::InitialStabilization)
    {// Don't throw events if we are initializing

      // When the brain oxygen partial pressure is too low, events are triggered and event duration is tracked.
      // The threshold values are chosen based on empirical data reviewed in summary in @cite dhawan2011neurointensive 
      // and from data presented in @cite purins2012brain and @cite doppenberg1998determination.
      if (m_BrainO2->GetPartialPressure(PressureUnit::mmHg) < 19.0) // We are using the mean from dhawan
      {
        m_data.GetEvents().SetEvent(eEvent::BrainOxygenDeficit, true, m_data.GetSimulationTime());

        // If the O2 tension is below a critical threshold, the damage occurs more quickly
        if (m_BrainO2->GetPartialPressure(PressureUnit::mmHg) < 10.0)
        {
          m_data.GetEvents().SetEvent(eEvent::CriticalBrainOxygenDeficit, true, m_data.GetSimulationTime());
        }
        else if (m_BrainO2->GetPartialPressure(PressureUnit::mmHg) > 12.0)
        {
          /// The brain is not in a critical oxygen deficit.
          m_data.GetEvents().SetEvent(eEvent::CriticalBrainOxygenDeficit, false, m_data.GetSimulationTime());
        }
      }
      else if (m_BrainO2->GetPartialPressure(PressureUnit::mmHg) > 25.0)
      {
        /// The brain is getting oxygen.
        m_data.GetEvents().SetEvent(eEvent::BrainOxygenDeficit, false, m_data.GetSimulationTime());
        // The critical deficit event is also set to false just in case there is an unrealistically rapid transition in oxygen partial pressure.
        m_data.GetEvents().SetEvent(eEvent::CriticalBrainOxygenDeficit, false, m_data.GetSimulationTime());
      }

      //Myocardium Oxygen Check
      if (m_MyocardiumO2->GetPartialPressure(PressureUnit::mmHg) < 5)
      {
        m_data.GetEvents().SetEvent(eEvent::MyocardiumOxygenDeficit, true, m_data.GetSimulationTime());
      }
      else if (m_MyocardiumO2->GetPartialPressure(PressureUnit::mmHg) > 8)
      {
        /// The brain is getting oxygen.
        m_data.GetEvents().SetEvent(eEvent::MyocardiumOxygenDeficit, false, m_data.GetSimulationTime());
      }
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Sets data on the complete blood count object.
  ///
  /// \details
  /// Sets data on the complete blood count object to create the [CBC](@ref bloodchemistry-assessments).
  //--------------------------------------------------------------------------------------------------
  bool BloodChemistryModel::CalculateArterialBloodGasTest(SEArterialBloodGasTest& abg) const
  {
    abg.Clear();
    abg.GetBloodPH().SetValue(GetBloodPH());
    double HCO3_mg_Per_L = m_data.GetSubstances().GetBicarbonate().GetBloodConcentration(MassPerVolumeUnit::mg_Per_L);
    double HCO3_g_Per_mol = m_data.GetSubstances().GetBicarbonate().GetMolarMass(MassPerAmountUnit::g_Per_mol);
    double HCO3_valence = m_data.GetSubstances().GetBicarbonate().GetValence().GetValue();
    std::cout << HCO3_mg_Per_L << " " << HCO3_g_Per_mol << " " << HCO3_valence << std::endl;
    abg.GetBicarbonate().SetValue(HCO3_mg_Per_L / HCO3_g_Per_mol * HCO3_valence, EquivalentWeightPerVolumeUnit::mEq_Per_L);
    abg.GetPartialPressureOfOxygen().SetValue(GetArterialOxygenPressure(PressureUnit::mmHg), PressureUnit::mmHg);
    abg.GetPartialPressureOfCarbonDioxide().SetValue(GetArterialCarbonDioxidePressure(PressureUnit::mmHg), PressureUnit::mmHg);
    abg.GetOxygenSaturation().SetValue(GetOxygenSaturation());
    return true;
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Sets data on the metabolic panel object.
  ///
  /// \details
  /// Sets data on the metabolic panel object to create the metabolic panel.  
  /// Uses information from the chem 14 substances that are in %Pulse (see @ref bloodchemistry-assessments)
  //--------------------------------------------------------------------------------------------------
  bool BloodChemistryModel::CalculateComprehensiveMetabolicPanel(SEComprehensiveMetabolicPanel& cmp) const
  {
    cmp.Clear();
    cmp.GetAlbumin().Set(m_data.GetSubstances().GetAlbumin().GetBloodConcentration());
    //cmp.GetALP().SetValue();
    //cmp.GetALT().SetValue();
    //cmp.GetAST().SetValue();
    cmp.GetBUN().SetValue(GetBloodUreaNitrogenConcentration(MassPerVolumeUnit::ug_Per_mL), MassPerVolumeUnit::ug_Per_mL);
    cmp.GetCalcium().Set(m_data.GetSubstances().GetCalcium().GetBloodConcentration());
    double CL_mmol_Per_L = m_data.GetSubstances().GetChloride().GetBloodConcentration(MassPerVolumeUnit::g_Per_L) /
      m_data.GetSubstances().GetChloride().GetMolarMass(MassPerAmountUnit::g_Per_mmol);
    cmp.GetChloride().SetValue(CL_mmol_Per_L, AmountPerVolumeUnit::mmol_Per_L);
    // CO2 is predominantly Bicarbonate, so going to put that in this slot
    double HCO3_mmol_Per_L = m_data.GetSubstances().GetBicarbonate().GetBloodConcentration(MassPerVolumeUnit::g_Per_L) /
      m_data.GetSubstances().GetHCO3().GetMolarMass(MassPerAmountUnit::g_Per_mmol);
    cmp.GetCO2().SetValue(HCO3_mmol_Per_L, AmountPerVolumeUnit::mmol_Per_L);
    cmp.GetCreatinine().Set(m_data.GetSubstances().GetCreatinine().GetBloodConcentration());
    cmp.GetGlucose().Set(m_data.GetSubstances().GetGlucose().GetBloodConcentration());
    double K_mmol_Per_L = m_data.GetSubstances().GetPotassium().GetBloodConcentration(MassPerVolumeUnit::g_Per_L) /
      m_data.GetSubstances().GetPotassium().GetMolarMass(MassPerAmountUnit::g_Per_mmol);
    cmp.GetPotassium().SetValue(K_mmol_Per_L, AmountPerVolumeUnit::mmol_Per_L);
    double Sodium_Mmol_Per_L = m_data.GetSubstances().GetSodium().GetBloodConcentration(MassPerVolumeUnit::g_Per_L) /
      m_data.GetSubstances().GetSodium().GetMolarMass(MassPerAmountUnit::g_Per_mmol);
    cmp.GetSodium().SetValue(Sodium_Mmol_Per_L, AmountPerVolumeUnit::mmol_Per_L);
    //cmp.GetTotalBelirubin().SetValue();
    cmp.GetTotalProtein().SetValue(GetTotalProteinConcentration(MassPerVolumeUnit::ug_Per_mL), MassPerVolumeUnit::ug_Per_mL);
    return true;
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Sets data on the complete blood count object.
  ///
  /// \details
  /// Sets data on the complete blood count object to create the [CBC](@ref bloodchemistry-assessments).
  //--------------------------------------------------------------------------------------------------
  bool BloodChemistryModel::CalculateCompleteBloodCount(SECompleteBloodCount& cbc) const
  {
    cbc.Clear();
    cbc.GetHematocrit().SetValue(GetHematocrit());
    cbc.GetHemoglobin().Set(m_data.GetSubstances().GetHb().GetBloodConcentration());
    cbc.GetPlateletCount().SetValue(325000, AmountPerVolumeUnit::ct_Per_uL);  // Hardcoded for now, don't support PlateletCount yet
    cbc.GetMeanCorpuscularHemoglobin().SetValue(m_data.GetConfiguration().GetMeanCorpuscularHemoglobin(MassPerAmountUnit::pg_Per_ct), MassPerAmountUnit::pg_Per_ct);
    cbc.GetMeanCorpuscularHemoglobinConcentration().SetValue(m_data.GetSubstances().GetHb().GetBloodConcentration(MassPerVolumeUnit::g_Per_dL) / GetHematocrit(), MassPerVolumeUnit::g_Per_dL);//Average range should be 32-36 g/dL. (https://en.wikipedia.org/wiki/Mean_corpuscular_hemoglobin_concentration)
    cbc.GetMeanCorpuscularVolume().SetValue(m_data.GetConfiguration().GetMeanCorpuscularVolume(VolumeUnit::uL), VolumeUnit::uL);
    cbc.GetRedBloodCellCount().SetValue(GetRedBloodCellCount(AmountPerVolumeUnit::ct_Per_L), AmountPerVolumeUnit::ct_Per_L);
    cbc.GetWhiteBloodCellCount().SetValue(GetWhiteBloodCellCount(AmountPerVolumeUnit::ct_Per_L), AmountPerVolumeUnit::ct_Per_L);
    return true;
  }
END_NAMESPACE
