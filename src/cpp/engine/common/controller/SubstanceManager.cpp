/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/common/controller/SubstanceManager.h"
#include "engine/common/controller/Controller.h"
#include "engine/common/controller/CompartmentManager.h"
#include "engine/common/system/physiology/Saturation.h"

#include "cdm/patient/SEPatient.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceAerosolization.h"
#include "cdm/substance/SESubstancePharmacodynamics.h"
#include "cdm/system/environment/SEEnvironment.h"
#include "cdm/compartment/fluid/SEGasCompartment.h"
#include "cdm/compartment/fluid/SEGasCompartmentLink.h"
#include "cdm/compartment/substances/SEGasSubstanceQuantity.h"
#include "cdm/compartment/fluid/SELiquidCompartment.h"
#include "cdm/compartment/fluid/SELiquidCompartmentLink.h"
#include "cdm/compartment/substances/SELiquidSubstanceQuantity.h"
#include "cdm/compartment/tissue/SETissueCompartment.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarAmountPerVolume.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarMassPerAmount.h"
#include "cdm/properties/SEHistogramFractionVsLength.h"
#include "cdm/utils/DataTrack.h"
#include "cdm/utils/GeneralMath.h"

namespace pulse
{
  SubstanceManager::SubstanceManager(Data& data) : SESubstanceManager(data.GetLogger()), m_data(data)
  {
    m_isCOActive = false;
  }

  void SubstanceManager::Clear()
  {
    SESubstanceManager::Clear();
    m_O2 = nullptr;
    m_CO = nullptr;
    m_CO2 = nullptr;
    m_N2 = nullptr;
    m_Hb = nullptr;
    m_HbO2 = nullptr;
    m_HbCO2 = nullptr;
    m_HbCO = nullptr;
    m_HbO2CO2 = nullptr;
    m_HCO3 = nullptr;
    m_epi = nullptr;
    m_norepi = nullptr;

    m_acetoacetate = nullptr;
    m_albumin = nullptr;
    m_calcium = nullptr;
    m_chloride = nullptr;
    m_creatinine = nullptr;
    m_globulin = nullptr;
    m_glucose = nullptr;
    m_insulin = nullptr;
    m_lactate = nullptr;
    m_potassium = nullptr;
    m_tristearin = nullptr;
    m_sodium = nullptr;
    m_urea = nullptr;
  }

  bool SubstanceManager::Setup()
  {
    m_O2 = GetSubstance("Oxygen");
    m_CO = GetSubstance("CarbonMonoxide");
    m_CO2 = GetSubstance("CarbonDioxide");
    m_N2 = GetSubstance("Nitrogen");
    m_Hb = GetSubstance("Hemoglobin");
    m_HbO2 = GetSubstance("Oxyhemoglobin");
    m_HbCO2 = GetSubstance("Carbaminohemoglobin");
    m_HbCO = GetSubstance("Carboxyhemoglobin");
    m_HbO2CO2 = GetSubstance("OxyCarbaminohemoglobin");
    m_HCO3 = GetSubstance("Bicarbonate");
    m_epi = GetSubstance("Epinephrine");
    m_norepi = GetSubstance("Norepinephrine");

    if (m_O2 == nullptr)
      Error("Oxygen Definition not found");
    if (m_CO == nullptr)
      Error("CarbonMonoxide Definition not found");
    if (m_CO2 == nullptr)
      Error("CarbonDioxide Definition not found");
    if (m_N2 == nullptr)
      Error("Nitrogen Definition not found");
    if (m_Hb == nullptr)
      Error("Hemoglobin Definition not found");
    if (m_HbO2 == nullptr)
      Error("Oxyhemoglobin Definition not found");
    if (m_HbCO2 == nullptr)
      Error("Carbaminohemoglobin Definition not found");
    if (m_HbCO == nullptr)
      Error("Carboxyhemoglobin Definition not found");
    if (m_HbO2CO2 == nullptr)
      Error("OxyCarbaminohemoglobin Definition not found");
    if (m_HCO3 == nullptr)
      Error("Bicarbonate Definition not found");
    if (m_epi == nullptr)
      Error("Epinephrine Definition not found");
    if (m_norepi == nullptr)
      Error("Norepinephrine Definition not found");

    if (m_O2 == nullptr || m_CO == nullptr || m_CO2 == nullptr || m_N2 == nullptr ||
      m_Hb == nullptr || m_HbO2 == nullptr || m_HbCO2 == nullptr || m_HbCO == nullptr || m_HbO2CO2 == nullptr ||
      m_epi == nullptr || m_norepi == nullptr || m_HCO3 == nullptr)
      return false;

    m_acetoacetate = GetSubstance("Acetoacetate");
    m_albumin = GetSubstance("Albumin");
    m_calcium = GetSubstance("Calcium");
    m_chloride = GetSubstance("Chloride");
    m_creatinine = GetSubstance("Creatinine");
    m_globulin = GetSubstance("Globulin");
    m_glucose = GetSubstance("Glucose");
    m_insulin = GetSubstance("Insulin");
    m_lactate = GetSubstance("Lactate");
    m_potassium = GetSubstance("Potassium");
    m_sodium = GetSubstance("Sodium");
    m_tristearin = GetSubstance("Tristearin");
    m_urea = GetSubstance("Urea");

    if (m_acetoacetate == nullptr)
      Error("Acetoacetate Definition not found");
    if (m_albumin == nullptr)
      Error("Albumin Definition not found");
    if (m_calcium == nullptr)
      Error("Calcium Definition not found");
    if (m_chloride == nullptr)
      Error("Chloride Definition not found");
    if (m_creatinine == nullptr)
      Error("Creatinine Definition not found");
    if (m_globulin == nullptr)
      Error("Globulin Definition not found");
    if (m_glucose == nullptr)
      Error("Glucose Definition not found");
    if (m_insulin == nullptr)
      Error("Insulin Definition not found");
    if (m_lactate == nullptr)
      Error("Lactate Definition not found");
    if (m_potassium == nullptr)
      Error("Potassium Definition not found");
    if (m_sodium == nullptr)
      Error("Sodium Definition not found");
    if (m_tristearin == nullptr)
      Error("Tristearin Definition not found");
    if (m_urea == nullptr)
      Error("Urea Definition not found");
    // These metabolites will be activated in initialization

    // Check that drugs have what we need
    for (SESubstance* sub : m_Substances)
    {
      if (sub->HasPD())
      {
        if (sub->GetPD().GetEC50().IsZero() || sub->GetPD().GetEC50().IsNegative())
        {
          Error(sub->GetName() + " cannot have EC50 <= 0");
          return false;
        }
      }
    }
    return true;
  }

  void SubstanceManager::InitializeSubstances()
  {
    // NOTE!!
    // The way Pulse sets up,
    // Substance initialization relies on environmental state,
    // so the environment will read the environment file associated
    // with this engine instance, and it will call AddActiveSubstance
    // and those gases will be added in the order they are read from the file
    // So your substance lists are not guaranteed to be in this order.
    // If this is ever a huge problem, or order is necessary for optimization or something
    // Just make an activate substances method that has to be called first,
    // That just makes an extra thing for unit tests and all that jazz to worry about
    // so I chose not to worry about it as I consider it a minor annoyance

    AddActiveSubstance(*m_O2);
    AddActiveSubstance(*m_CO2);
    AddActiveSubstance(*m_N2);

    AddActiveSubstance(*m_Hb);
    AddActiveSubstance(*m_HbO2);
    AddActiveSubstance(*m_HbCO2);
    AddActiveSubstance(*m_HbO2CO2);
    AddActiveSubstance(*m_HCO3);
    AddActiveSubstance(*m_epi);
    AddActiveSubstance(*m_norepi);

    AddActiveSubstance(*m_acetoacetate);
    AddActiveSubstance(*m_albumin);
    AddActiveSubstance(*m_calcium);
    AddActiveSubstance(*m_chloride);
    AddActiveSubstance(*m_creatinine);
    //AddActiveSubstance(*m_globulin);//We don't transport this
    AddActiveSubstance(*m_glucose);
    AddActiveSubstance(*m_insulin);
    AddActiveSubstance(*m_lactate);
    AddActiveSubstance(*m_potassium);
    AddActiveSubstance(*m_sodium);
    AddActiveSubstance(*m_tristearin);
    AddActiveSubstance(*m_urea);
  }

  void SubstanceManager::CopyConcentrations(SELiquidCompartment& src, SELiquidCompartment& tgt)
  {
    for (SELiquidSubstanceQuantity* subQ : src.GetSubstanceQuantities())
    {
      tgt.GetSubstanceQuantity(subQ->GetSubstance())->GetConcentration().Set(subQ->GetConcentration());
    }
    tgt.Balance(BalanceLiquidBy::Concentration);
    tgt.GetPH().Set(src.GetPH());
    m_data.GetSaturationCalculator().CalculateBloodGasDistribution(tgt);
  }

  void SubstanceManager::InitializeGasCompartments()
  {
    SEGasCompartment* Ambient = m_data.GetCompartments().GetGasCompartment(pulse::EnvironmentCompartment::Ambient);
    double AmbientO2VF = Ambient->GetSubstanceQuantity(*m_O2)->GetVolumeFraction().GetValue();
    double AmbientCO2VF = Ambient->GetSubstanceQuantity(*m_CO2)->GetVolumeFraction().GetValue();
    double AmbientN2VF = Ambient->GetSubstanceQuantity(*m_N2)->GetVolumeFraction().GetValue();

    //Initialize the compartments to Ambient values
    for (SEGasCompartment* cmpt : m_data.GetCompartments().GetPulmonaryLeafCompartments())
    {
      if (cmpt->HasVolume())
      {
        cmpt->GetSubstanceQuantity(*m_O2)->GetVolumeFraction().SetValue(AmbientO2VF);
        cmpt->GetSubstanceQuantity(*m_CO2)->GetVolumeFraction().SetValue(AmbientCO2VF);
        cmpt->GetSubstanceQuantity(*m_N2)->GetVolumeFraction().SetValue(AmbientN2VF);
        cmpt->Balance(BalanceGasBy::VolumeFraction);
      }
    }

    if (m_data.GetConfiguration().UseExpandedRespiratory() == eSwitch::On)
    {
      for (SEGasCompartment* cmpt : m_data.GetCompartments().GetExpandedPulmonaryLeafCompartments())
      {
        if (cmpt->HasVolume())
        {
          cmpt->GetSubstanceQuantity(*m_O2)->GetVolumeFraction().SetValue(AmbientO2VF);
          cmpt->GetSubstanceQuantity(*m_CO2)->GetVolumeFraction().SetValue(AmbientCO2VF);
          cmpt->GetSubstanceQuantity(*m_N2)->GetVolumeFraction().SetValue(AmbientN2VF);
          cmpt->Balance(BalanceGasBy::VolumeFraction);
        }
      }
    }

    for (SEGasCompartment* cmpt : m_data.GetCompartments().GetAnesthesiaMachineLeafCompartments())
    {
      if (cmpt->HasVolume())
      {
        cmpt->GetSubstanceQuantity(*m_O2)->GetVolumeFraction().SetValue(AmbientO2VF);
        cmpt->GetSubstanceQuantity(*m_CO2)->GetVolumeFraction().SetValue(AmbientCO2VF);
        cmpt->GetSubstanceQuantity(*m_N2)->GetVolumeFraction().SetValue(AmbientN2VF);
        cmpt->Balance(BalanceGasBy::VolumeFraction);
      }
    }

    for (SEGasCompartment* cmpt : m_data.GetCompartments().GetMechanicalVentilatorLeafCompartments())
    {
      if (cmpt->HasVolume())
      {
        cmpt->GetSubstanceQuantity(*m_O2)->GetVolumeFraction().SetValue(AmbientO2VF);
        cmpt->GetSubstanceQuantity(*m_CO2)->GetVolumeFraction().SetValue(AmbientCO2VF);
        cmpt->GetSubstanceQuantity(*m_N2)->GetVolumeFraction().SetValue(AmbientN2VF);
        cmpt->Balance(BalanceGasBy::VolumeFraction);
      }
    }

    for (SEGasCompartment* cmpt : m_data.GetCompartments().GetInhalerLeafCompartments())
    {
      if (cmpt->HasVolume())
      {
        cmpt->GetSubstanceQuantity(m_data.GetSubstances().GetO2())->GetVolumeFraction().SetValue(AmbientO2VF);
        cmpt->GetSubstanceQuantity(m_data.GetSubstances().GetCO2())->GetVolumeFraction().SetValue(AmbientCO2VF);
        cmpt->GetSubstanceQuantity(m_data.GetSubstances().GetN2())->GetVolumeFraction().SetValue(AmbientN2VF);
        cmpt->Balance(BalanceGasBy::VolumeFraction);
      }
    }

    for (SEGasCompartment* cmpt : m_data.GetCompartments().GetNasalCannulaLeafCompartments())
    {
      if (cmpt->HasVolume())
      {
        if (cmpt->GetName() == pulse::NasalCannulaCompartment::NasalCannula)
        {
          cmpt->GetSubstanceQuantity(m_data.GetSubstances().GetO2())->GetVolumeFraction().SetValue(AmbientO2VF);
          cmpt->GetSubstanceQuantity(m_data.GetSubstances().GetCO2())->GetVolumeFraction().SetValue(AmbientCO2VF);
          cmpt->GetSubstanceQuantity(m_data.GetSubstances().GetN2())->GetVolumeFraction().SetValue(AmbientN2VF);
        }
        else
        {
          cmpt->GetSubstanceQuantity(m_data.GetSubstances().GetO2())->GetVolumeFraction().SetValue(1.0);
        }
        cmpt->Balance(BalanceGasBy::VolumeFraction);
      }
    }

    for (SEGasCompartment* cmpt : m_data.GetCompartments().GetNonRebreatherMaskLeafCompartments())
    {
      if (cmpt->HasVolume())
      {
        cmpt->GetSubstanceQuantity(m_data.GetSubstances().GetO2())->GetVolumeFraction().SetValue(1.0);
        cmpt->Balance(BalanceGasBy::VolumeFraction);
      }
    }

    for (SEGasCompartment* cmpt : m_data.GetCompartments().GetSimpleMaskLeafCompartments())
    {
      if (cmpt->HasVolume())
      {
        cmpt->GetSubstanceQuantity(m_data.GetSubstances().GetO2())->GetVolumeFraction().SetValue(1.0);
        cmpt->Balance(BalanceGasBy::VolumeFraction);
      }
    }
  }

  void SubstanceManager::InitializeLiquidCompartmentGases()
  {
    CompartmentManager& cmpts = m_data.GetCompartments();

    SEScalarMassPerVolume   albuminConcentration;
    SEScalar0To1            hematocrit;
    SEScalarTemperature     bodyTemp;
    SEScalarAmountPerVolume strongIonDifference;
    SEScalarAmountPerVolume phosphate;

    albuminConcentration.SetValue(45.0, MassPerVolumeUnit::g_Per_L);
    double Hb_total_g_Per_dL = m_data.GetCurrentPatient().GetSex() == ePatient_Sex::Male ? 15.0 : 14.0;
    hematocrit.SetValue(Hb_total_g_Per_dL / 34.0);
    bodyTemp.SetValue(37.0, TemperatureUnit::C);
    strongIonDifference.SetValue(40.5, AmountPerVolumeUnit::mmol_Per_L);
    phosphate.SetValue(1.1, AmountPerVolumeUnit::mmol_Per_L);

    m_data.GetSaturationCalculator().SetBodyState(albuminConcentration, hematocrit, bodyTemp, strongIonDifference, phosphate);

    // Convert to mmol/L
    // Note that we would need to use 1/4 of the molecular weight of the hemoglobin tetramer
    // to compute a site-based concentration in accordance with lab custom.
    // However, we should account with the actual molarity not the "lab" molarity.
    // So we need to note that our hemoglobin molarity is 1/4 times the reference values
    // because we are using the tetramer concentration not the site concentration.
    // For details please see @cite lodemann2010wrong
    double Hb_total_mM = Hb_total_g_Per_dL / m_Hb->GetMolarMass(MassPerAmountUnit::g_Per_mmol) * 10.0;

    // Bootstrapped values from many runs (using standard patient)
    if (m_data.GetConfiguration().UseExpandedVasculature() == eSwitch::On)
    {
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::Aorta), Hb_total_mM, 0.974905, 0.129608, 0.02817, 1.3012, 25.9389, 7.39961);
      CopyConcentrations  (*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::Aorta), *cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::ArterialBuffer));
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::Extracranial), Hb_total_mM, 0.827281, 0.0612181, 0.133288, 1.38958, 26.0916, 7.37362);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::Intracranial), Hb_total_mM, 0.827281, 0.0612181, 0.133288, 1.38958, 26.0916, 7.37362);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::Gut1), Hb_total_mM, 0.859657, 0.0668517, 0.112249, 1.37857, 26.0731, 7.37676);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LargeIntestine), Hb_total_mM, 0.807872, 0.0584747, 0.149044, 1.39821, 26.106, 7.37117);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftAfferentArteriole), Hb_total_mM, 0.927186, 0.0870192, 0.0595885, 1.35232, 26.0285, 7.38437);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftArmArterioles), Hb_total_mM, 0.830363, 0.0616873, 0.13056, 1.3881, 26.0892, 7.37404);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftArmBone), Hb_total_mM, 0.830363, 0.0616873, 0.13056, 1.3881, 26.0892, 7.37404);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftArmFat), Hb_total_mM, 0.830303, 0.0616616, 0.128914, 1.38717, 26.0876, 7.3743);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftArmMicrovasculature), Hb_total_mM, 0.974914, 0.129637, 0.0281665, 1.30153, 25.9395, 7.39951);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftArmMuscle), Hb_total_mM, 0.830253, 0.0616652, 0.130118, 1.38785, 26.0887, 7.37411);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftArmSkin), Hb_total_mM, 0.82836, 0.061373, 0.131491, 1.38858, 26.09, 7.3739);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftArmVenules), Hb_total_mM, 0.830363, 0.0616873, 0.13056, 1.3881, 26.0892, 7.37404);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftEfferentArteriole), Hb_total_mM, 0.907413, 0.0791142, 0.072315, 1.35842, 26.0389, 7.38259);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftGlomerularCapillaries), Hb_total_mM, 0.9209, 0.0773075, 0.031322, 1.07857, 25.5034, 7.47375);
      //No Hb through glomerular ultrafiltration
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftBowmansCapsules), 0.0, 0.0, 0.0773075, 0.0, 1.07857, 25.5034, 7.47375);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftTubules), 0.0, 0.0, 0.0773075, 0.0, 1.07857, 25.5034, 7.47375);
      //----------------------------------------
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftHeart), Hb_total_mM, 0.974919, 0.129622, 0.0281731, 1.30089, 25.9384, 7.3997);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftLegArterioles), Hb_total_mM, 0.830363, 0.0616873, 0.13056, 1.3881, 26.0892, 7.37404);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftLegBone), Hb_total_mM, 0.830363, 0.0616873, 0.13056, 1.3881, 26.0892, 7.37404);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftLegFat), Hb_total_mM, 0.830303, 0.0616616, 0.128914, 1.38717, 26.0876, 7.3743);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftLegMicrovasculature), Hb_total_mM, 0.974914, 0.129637, 0.0281665, 1.30153, 25.9395, 7.39951);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftLegMuscle), Hb_total_mM, 0.830253, 0.0616652, 0.130118, 1.38785, 26.0887, 7.37411);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftLegSkin), Hb_total_mM, 0.82836, 0.061373, 0.131491, 1.38858, 26.09, 7.3739);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftLegVenules), Hb_total_mM, 0.82836, 0.061373, 0.131491, 1.38858, 26.09, 7.3739);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftPeritubularCapillaries), Hb_total_mM, 0.89667, 0.0757891, 0.0873866, 1.36599, 26.0518, 7.38039);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftPulmonaryArteries), Hb_total_mM, 0.813117, 0.0591941, 0.146207, 1.39669, 26.1035, 7.3716);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftPulmonaryCapillaries), Hb_total_mM, 0.983029, 0.149863, 0.0281766, 1.29193, 25.9223, 7.40244);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftPulmonaryVeins), Hb_total_mM, 0.974924, 0.129359, 0.0282471, 1.29371, 25.9255, 7.40189);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftRenalArtery), Hb_total_mM, 0.941145, 0.0945363, 0.0484668, 1.34704, 26.0194, 7.38592);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftRenalVein), Hb_total_mM, 0.859657, 0.0668517, 0.112249, 1.37857, 26.0731, 7.37676);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::Liver), Hb_total_mM, 0.729017, 0.0500882, 0.21436, 1.43708, 26.1698, 7.36032);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::Myocardium), Hb_total_mM, 0.829366, 0.0615371, 0.13173, 1.38874, 26.0902, 7.37386);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::Pericardium), Hb_total_mM, 0.725881, 0.0491881, 0.142507, 1.39215, 26.0959, 7.37289);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightAfferentArteriole), Hb_total_mM, 0.927186, 0.0870192, 0.0595885, 1.35232, 26.0285, 7.38437);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightArmArterioles), Hb_total_mM, 0.830363, 0.0616873, 0.13056, 1.3881, 26.0892, 7.37404);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightArmBone), Hb_total_mM, 0.830363, 0.0616873, 0.13056, 1.3881, 26.0892, 7.37404);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightArmFat), Hb_total_mM, 0.830303, 0.0616616, 0.128914, 1.38717, 26.0876, 7.3743);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightArmMicrovasculature), Hb_total_mM, 0.974914, 0.129637, 0.0281665, 1.30153, 25.9395, 7.39951); //todo
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightArmMuscle), Hb_total_mM, 0.830253, 0.0616652, 0.130118, 1.38785, 26.0887, 7.37411);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightArmSkin), Hb_total_mM, 0.82836, 0.061373, 0.131491, 1.38858, 26.09, 7.3739);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightArmVenules), Hb_total_mM, 0.82836, 0.061373, 0.131491, 1.38858, 26.09, 7.3739);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightEfferentArteriole), Hb_total_mM, 0.907413, 0.0791142, 0.072315, 1.35842, 26.0389, 7.38259);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightGlomerularCapillaries), Hb_total_mM, 0.9209, 0.0773075, 0.031322, 1.07857, 25.5034, 7.47375);
      //No Hb through glomerular ultrafiltration
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightBowmansCapsules), 0.0, 0.0, 0.0773075, 0.0, 1.07857, 25.5034, 7.47375);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightTubules), 0.0, 0.0, 0.0773075, 0.0, 1.07857, 25.5034, 7.47375);
      //----------------------------------------
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightHeart), Hb_total_mM, 0.813115, 0.0591938, 0.14621, 1.39669, 26.1035, 7.3716);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightLegArterioles), Hb_total_mM, 0.830363, 0.0616873, 0.13056, 1.3881, 26.0892, 7.37404);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightLegBone), Hb_total_mM, 0.830363, 0.0616873, 0.13056, 1.3881, 26.0892, 7.37404);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightLegFat), Hb_total_mM, 0.830303, 0.0616616, 0.128914, 1.38717, 26.0876, 7.3743);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightLegMicrovasculature), Hb_total_mM, 0.974914, 0.129637, 0.0281665, 1.30153, 25.9395, 7.39951);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightLegMuscle), Hb_total_mM, 0.830253, 0.0616652, 0.130118, 1.38785, 26.0887, 7.37411);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightLegSkin), Hb_total_mM, 0.82836, 0.061373, 0.131491, 1.38858, 26.09, 7.3739);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightLegVenules), Hb_total_mM, 0.830363, 0.0616873, 0.13056, 1.3881, 26.0892, 7.37404);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightPeritubularCapillaries), Hb_total_mM, 0.89667, 0.0757891, 0.0873866, 1.36599, 26.0518, 7.38039);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightPulmonaryArteries), Hb_total_mM, 0.813117, 0.0591941, 0.146207, 1.39669, 26.1035, 7.3716);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightPulmonaryCapillaries), Hb_total_mM, 0.982298, 0.148493, 0.0279498, 1.31484, 25.9632, 7.39548);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightPulmonaryVeins), Hb_total_mM, 0.974936, 0.130199, 0.0280263, 1.31523, 25.9638, 7.39537);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightRenalArtery), Hb_total_mM, 0.941145, 0.0945363, 0.0484668, 1.34704, 26.0194, 7.38592);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightRenalVein), Hb_total_mM, 0.859657, 0.0668517, 0.112249, 1.37857, 26.0731, 7.37676);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::SmallIntestine), Hb_total_mM, 0.842176, 0.0636112, 0.122046, 1.3836, 26.0816, 7.37532);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::Splanchnic), Hb_total_mM, 0.817051, 0.0597293, 0.141898, 1.39428, 26.0995, 7.37228);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::Spleen), Hb_total_mM, 0.830039, 0.0616463, 0.131757, 1.38878, 26.0903, 7.37385);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::TorsoArterioles), Hb_total_mM, 0.82836, 0.061373, 0.131491, 1.38858, 26.09, 7.3739);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::TorsoBone), Hb_total_mM, 0.830363, 0.0616873, 0.13056, 1.3881, 26.0892, 7.37404);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::TorsoFat), Hb_total_mM, 0.830303, 0.0616616, 0.128914, 1.38717, 26.0876, 7.3743);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::TorsoMicrovasculature), Hb_total_mM, 0.830253, 0.0616652, 0.130118, 1.38785, 26.0887, 7.37411);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::TorsoMuscle), Hb_total_mM, 0.830253, 0.0616652, 0.130118, 1.38785, 26.0887, 7.37411);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::TorsoSkin), Hb_total_mM, 0.82836, 0.061373, 0.131491, 1.38858, 26.09, 7.3739);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::TorsoVenules), Hb_total_mM, 0.82836, 0.061373, 0.131491, 1.38858, 26.09, 7.3739);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::VenaCava), Hb_total_mM, 0.813122, 0.0591948, 0.146203, 1.39669, 26.1035, 7.3716);
      CopyConcentrations(*cmpts.GetLiquidCompartment(  pulse::ExpandedVascularCompartment::VenaCava), *cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::VenousBuffer));

      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::Bone), *cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::TorsoBone));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::Brain), *cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::Intracranial));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::Fat), *cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::TorsoFat));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::Gut), *cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::Gut));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::LeftKidney), *cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftKidney));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::LeftLung), *cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::LeftLung));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::Liver), *cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::Liver));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::Muscle), *cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::TorsoMuscle));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::Myocardium), *cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::Myocardium));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::RightKidney), *cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightKidney));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::RightLung), *cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::RightLung));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::Skin), *cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::TorsoSkin));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::Spleen), *cmpts.GetLiquidCompartment(pulse::ExpandedVascularCompartment::Spleen));
    }
    else
    {
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::Aorta), Hb_total_mM, 0.974905, 0.129608, 0.02817, 1.3012, 25.9389, 7.39961);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::Bone), Hb_total_mM, 0.830363, 0.0616873, 0.13056, 1.3881, 26.0892, 7.37404);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::Brain), Hb_total_mM, 0.827281, 0.0612181, 0.133288, 1.38958, 26.0916, 7.37362);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::Fat), Hb_total_mM, 0.830303, 0.0616616, 0.128914, 1.38717, 26.0876, 7.3743);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::LargeIntestine), Hb_total_mM, 0.807872, 0.0584747, 0.149044, 1.39821, 26.106, 7.37117);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::LeftAfferentArteriole), Hb_total_mM, 0.927186, 0.0870192, 0.0595885, 1.35232, 26.0285, 7.38437);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::LeftArm), Hb_total_mM, 0.974914, 0.129637, 0.0281665, 1.30153, 25.9395, 7.39951);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::LeftEfferentArteriole), Hb_total_mM, 0.907413, 0.0791142, 0.072315, 1.35842, 26.0389, 7.38259);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::LeftGlomerularCapillaries), Hb_total_mM, 0.9209, 0.0773075, 0.031322, 1.07857, 25.5034, 7.47375);
      //No Hb through glomerular ultrafiltration
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::LeftBowmansCapsules), 0.0, 0.0, 0.0773075, 0.0, 1.07857, 25.5034, 7.47375);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::LeftTubules), 0.0, 0.0, 0.0773075, 0.0, 1.07857, 25.5034, 7.47375);
      //----------------------------------------
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::LeftHeart), Hb_total_mM, 0.974919, 0.129622, 0.0281731, 1.30089, 25.9384, 7.3997);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::LeftLeg), Hb_total_mM, 0.974914, 0.129637, 0.0281665, 1.30153, 25.9395, 7.39951);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::LeftPeritubularCapillaries), Hb_total_mM, 0.89667, 0.0757891, 0.0873866, 1.36599, 26.0518, 7.38039);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::LeftPulmonaryArteries), Hb_total_mM, 0.813117, 0.0591941, 0.146207, 1.39669, 26.1035, 7.3716);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::LeftPulmonaryCapillaries), Hb_total_mM, 0.983029, 0.149863, 0.0281766, 1.29193, 25.9223, 7.40244);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::LeftPulmonaryVeins), Hb_total_mM, 0.974924, 0.129359, 0.0282471, 1.29371, 25.9255, 7.40189);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::LeftRenalArtery), Hb_total_mM, 0.941145, 0.0945363, 0.0484668, 1.34704, 26.0194, 7.38592);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::LeftRenalVein), Hb_total_mM, 0.859657, 0.0668517, 0.112249, 1.37857, 26.0731, 7.37676);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::Liver), Hb_total_mM, 0.729017, 0.0500882, 0.21436, 1.43708, 26.1698, 7.36032);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::Muscle), Hb_total_mM, 0.830253, 0.0616652, 0.130118, 1.38785, 26.0887, 7.37411);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::Myocardium), Hb_total_mM, 0.829366, 0.0615371, 0.13173, 1.38874, 26.0902, 7.37386);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::Pericardium), Hb_total_mM, 0.725881, 0.0491881, 0.142507, 1.39215, 26.0959, 7.37289);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::RightAfferentArteriole), Hb_total_mM, 0.927186, 0.0870192, 0.0595885, 1.35232, 26.0285, 7.38437);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::RightArm), Hb_total_mM, 0.974914, 0.129637, 0.0281665, 1.30153, 25.9395, 7.39951);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::RightEfferentArteriole), Hb_total_mM, 0.907413, 0.0791142, 0.072315, 1.35842, 26.0389, 7.38259);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::RightGlomerularCapillaries), Hb_total_mM, 0.9209, 0.0773075, 0.031322, 1.07857, 25.5034, 7.47375);
      //No Hb through glomerular ultrafiltration
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::RightBowmansCapsules), 0.0, 0.0, 0.0773075, 0.0, 1.07857, 25.5034, 7.47375);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::RightTubules), 0.0, 0.0, 0.0773075, 0.0, 1.07857, 25.5034, 7.47375);
      //----------------------------------------
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::RightHeart), Hb_total_mM, 0.813115, 0.0591938, 0.14621, 1.39669, 26.1035, 7.3716);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::RightLeg), Hb_total_mM, 0.974914, 0.129637, 0.0281665, 1.30153, 25.9395, 7.39951);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::RightPeritubularCapillaries), Hb_total_mM, 0.89667, 0.0757891, 0.0873866, 1.36599, 26.0518, 7.38039);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::RightPulmonaryArteries), Hb_total_mM, 0.813117, 0.0591941, 0.146207, 1.39669, 26.1035, 7.3716);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::RightPulmonaryCapillaries), Hb_total_mM, 0.982298, 0.148493, 0.0279498, 1.31484, 25.9632, 7.39548);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::RightPulmonaryVeins), Hb_total_mM, 0.974936, 0.130199, 0.0280263, 1.31523, 25.9638, 7.39537);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::RightRenalArtery), Hb_total_mM, 0.941145, 0.0945363, 0.0484668, 1.34704, 26.0194, 7.38592);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::RightRenalVein), Hb_total_mM, 0.859657, 0.0668517, 0.112249, 1.37857, 26.0731, 7.37676);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::Skin), Hb_total_mM, 0.82836, 0.061373, 0.131491, 1.38858, 26.09, 7.3739);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::SmallIntestine), Hb_total_mM, 0.842176, 0.0636112, 0.122046, 1.3836, 26.0816, 7.37532);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::Splanchnic), Hb_total_mM, 0.817051, 0.0597293, 0.141898, 1.39428, 26.0995, 7.37228);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::Spleen), Hb_total_mM, 0.830039, 0.0616463, 0.131757, 1.38878, 26.0903, 7.37385);
      InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::VenaCava), Hb_total_mM, 0.813122, 0.0591948, 0.146203, 1.39669, 26.1035, 7.3716);

      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::Bone), *cmpts.GetLiquidCompartment(pulse::VascularCompartment::Bone));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::Brain), *cmpts.GetLiquidCompartment(pulse::VascularCompartment::Brain));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::Fat), *cmpts.GetLiquidCompartment(pulse::VascularCompartment::Fat));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::Gut), *cmpts.GetLiquidCompartment(pulse::VascularCompartment::Gut));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::LeftKidney), *cmpts.GetLiquidCompartment(pulse::VascularCompartment::LeftKidney));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::LeftLung), *cmpts.GetLiquidCompartment(pulse::VascularCompartment::LeftLung));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::Liver), *cmpts.GetLiquidCompartment(pulse::VascularCompartment::Liver));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::Muscle), *cmpts.GetLiquidCompartment(pulse::VascularCompartment::Muscle));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::Myocardium), *cmpts.GetLiquidCompartment(pulse::VascularCompartment::Myocardium));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::RightKidney), *cmpts.GetLiquidCompartment(pulse::VascularCompartment::RightKidney));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::RightLung), *cmpts.GetLiquidCompartment(pulse::VascularCompartment::RightLung));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::Skin), *cmpts.GetLiquidCompartment(pulse::VascularCompartment::Skin));
      InitializeBloodGases(*cmpts.GetTissueCompartment(pulse::TissueCompartment::Spleen), *cmpts.GetLiquidCompartment(pulse::VascularCompartment::Spleen));
    }

    SEScalarMassPerVolume concentration;
    concentration.SetValue(0.146448, MassPerVolumeUnit::g_Per_dL);
    SetSubstanceConcentration(*m_HCO3, cmpts.GetUrineLeafCompartments(), concentration);
  }
  void SubstanceManager::InitializeBloodGases(SELiquidCompartment& inputCmpt, double Hb_total_mM, double O2_sat, double O2_mmol_Per_L, double CO2_sat, double CO2_mmol_Per_L, double HCO3_mmol_Per_L, double pH, bool distribute)
  {
    SELiquidCompartment* cmpt = &inputCmpt;

    bool hasChildren = inputCmpt.HasChildren();
    size_t numCmpt = 1;
    if (hasChildren)
    {
      numCmpt = inputCmpt.GetLeaves().size();
    }
    unsigned int leafIter = 0;
    while (leafIter < numCmpt)
    {
      if (hasChildren)
      {
        cmpt = inputCmpt.GetLeaves().at(leafIter);
      }

      // N2 is inert
      SEGasCompartment* Ambient = m_data.GetCompartments().GetGasCompartment(pulse::EnvironmentCompartment::Ambient);
      double N2_mmHg = Ambient->GetPressure(PressureUnit::mmHg) * Ambient->GetSubstanceQuantity(*m_N2)->GetVolumeFraction().GetValue();
      SELiquidSubstanceQuantity* N2 = cmpt->GetSubstanceQuantity(*m_N2);
      N2->GetPartialPressure().SetValue(N2_mmHg, PressureUnit::mmHg);
      N2->Balance(BalanceLiquidBy::PartialPressure);

      SELiquidSubstanceQuantity* O2 = cmpt->GetSubstanceQuantity(*m_O2);
      SELiquidSubstanceQuantity* CO2 = cmpt->GetSubstanceQuantity(*m_CO2);
      SELiquidSubstanceQuantity* Hb = cmpt->GetSubstanceQuantity(*m_Hb);
      SELiquidSubstanceQuantity* HbO2 = cmpt->GetSubstanceQuantity(*m_HbO2);
      SELiquidSubstanceQuantity* HbCO2 = cmpt->GetSubstanceQuantity(*m_HbCO2);
      SELiquidSubstanceQuantity* HbO2CO2 = cmpt->GetSubstanceQuantity(*m_HbO2CO2);
      SELiquidSubstanceQuantity* HCO3 = cmpt->GetSubstanceQuantity(*m_HCO3);

      //Assume no HbO2CO2 at first (O2sat + CO2sat < 100%)
      double HbUnbound_mM = Hb_total_mM * (1 - O2_sat - CO2_sat);
      if (std::abs(HbUnbound_mM) <= ZERO_APPROX) HbUnbound_mM = 0;

      //If our assumption was wrong, that means there was HbO2CO2 contributing to sat values
      //Any negative is due to HbO2CO2
      if (HbUnbound_mM < 0)
      {
        double HbO2CO2_mM = -HbUnbound_mM;
        HbO2CO2->GetMolarity().SetValue(HbO2CO2_mM, AmountPerVolumeUnit::mmol_Per_L);
        HbO2CO2->Balance(BalanceLiquidBy::Molarity);

        HbUnbound_mM = 0;
        Hb->GetMolarity().SetValue(HbUnbound_mM, AmountPerVolumeUnit::mmol_Per_L);
        Hb->Balance(BalanceLiquidBy::Molarity);

        //Now we know HbUnbound and HbO2CO2, and we can solve HbO2 and HbCO2 using saturation values
        double HbO2_mM = O2_sat * Hb_total_mM - HbO2CO2_mM;
        double HbCO2_mM = CO2_sat * Hb_total_mM - HbO2CO2_mM;

        if (std::abs(HbCO2_mM) <= ZERO_APPROX) HbCO2_mM = 0;
        if (std::abs(HbO2_mM) <= ZERO_APPROX) HbO2_mM = 0;

        HbO2->GetMolarity().SetValue(HbO2_mM, AmountPerVolumeUnit::mmol_Per_L);
        HbO2->Balance(BalanceLiquidBy::Molarity);
        HbCO2->GetMolarity().SetValue(HbCO2_mM, AmountPerVolumeUnit::mmol_Per_L);
        HbCO2->Balance(BalanceLiquidBy::Molarity);
      }
      //If our assumption was right, we have excess Hb, and need to use the total Hb to solve
      else
      {
        double HbO2CO2_mM = (O2_sat * Hb_total_mM) - Hb_total_mM + HbUnbound_mM + (CO2_sat * Hb_total_mM);
        double HbO2_mM = O2_sat * Hb_total_mM - HbO2CO2_mM;
        double HbCO2_mM = CO2_sat * Hb_total_mM - HbO2CO2_mM;

        if (std::abs(HbO2CO2_mM) <= ZERO_APPROX) HbO2CO2_mM = 0;
        if (std::abs(HbCO2_mM) <= ZERO_APPROX) HbCO2_mM = 0;
        if (std::abs(HbO2_mM) <= ZERO_APPROX) HbO2_mM = 0;

        Hb->GetMolarity().SetValue(HbUnbound_mM, AmountPerVolumeUnit::mmol_Per_L);
        Hb->Balance(BalanceLiquidBy::Molarity);
        HbO2CO2->GetMolarity().SetValue(HbO2CO2_mM, AmountPerVolumeUnit::mmol_Per_L);
        HbO2CO2->Balance(BalanceLiquidBy::Molarity);
        HbO2->GetMolarity().SetValue(HbO2_mM, AmountPerVolumeUnit::mmol_Per_L);
        HbO2->Balance(BalanceLiquidBy::Molarity);
        HbCO2->GetMolarity().SetValue(HbCO2_mM, AmountPerVolumeUnit::mmol_Per_L);
        HbCO2->Balance(BalanceLiquidBy::Molarity);
      }

      CO2->GetMolarity().SetValue(CO2_mmol_Per_L, AmountPerVolumeUnit::mmol_Per_L);
      CO2->Balance(BalanceLiquidBy::Molarity);
      CO2->GetSaturation().SetValue(CO2_sat);
      HCO3->GetMolarity().SetValue(HCO3_mmol_Per_L, AmountPerVolumeUnit::mmol_Per_L);
      HCO3->Balance(BalanceLiquidBy::Molarity);
      O2->GetMolarity().SetValue(O2_mmol_Per_L, AmountPerVolumeUnit::mmol_Per_L);
      O2->Balance(BalanceLiquidBy::Molarity);
      O2->GetSaturation().SetValue(O2_sat);

      cmpt->GetPH().SetValue(pH);

      if (distribute)
        m_data.GetSaturationCalculator().CalculateBloodGasDistribution(*cmpt);

      /*std::cout << cmpt->GetName() << " O2 Partial Pressure " << O2->GetPartialPressure() << std::endl;
      std::cout << cmpt->GetName() << " CO2 Partial Pressure  " << CO2->GetPartialPressure() << std::endl;
      std::cout << cmpt->GetName() << " O2 Concentration " << O2->GetConcentration() << std::endl;
      std::cout << cmpt->GetName() << " CO2 Concentration " << CO2->GetConcentration() << std::endl;
      std::cout << cmpt->GetName() << " HCO3 Molarity " << HCO3->GetMolarity() << std::endl;
      std::cout << " " << std::endl;*/

      leafIter++;
    }
  }
  void SubstanceManager::InitializeBloodGases(SETissueCompartment& tissue, SELiquidCompartment& vascular)
  {
    SELiquidCompartment& extracellular = tissue.GetExtracellular();
    extracellular.GetSubstanceQuantity(*m_N2)->GetMolarity().Set(vascular.GetSubstanceQuantity(*m_N2)->GetMolarity());
    extracellular.GetSubstanceQuantity(*m_O2)->GetMolarity().Set(vascular.GetSubstanceQuantity(*m_O2)->GetMolarity());
    extracellular.GetSubstanceQuantity(*m_CO2)->GetMolarity().Set(vascular.GetSubstanceQuantity(*m_CO2)->GetMolarity());
    extracellular.Balance(BalanceLiquidBy::Molarity);

    SELiquidCompartment& intracellular = tissue.GetIntracellular();
    intracellular.GetSubstanceQuantity(*m_N2)->GetMolarity().Set(vascular.GetSubstanceQuantity(*m_N2)->GetMolarity());
    intracellular.GetSubstanceQuantity(*m_O2)->GetMolarity().Set(vascular.GetSubstanceQuantity(*m_O2)->GetMolarity());
    intracellular.GetSubstanceQuantity(*m_CO2)->GetMolarity().Set(vascular.GetSubstanceQuantity(*m_CO2)->GetMolarity());
    intracellular.Balance(BalanceLiquidBy::Molarity);
  }

  void SubstanceManager::WriteBloodGases()
  {
    std::stringstream ss;
    for (SELiquidCompartment* cmpt : m_data.GetCompartments().GetVascularLeafCompartments())
    {
      if (cmpt->HasVolume())
      {
        SELiquidSubstanceQuantity* O2 = cmpt->GetSubstanceQuantity(*m_O2);
        SELiquidSubstanceQuantity* CO2 = cmpt->GetSubstanceQuantity(*m_CO2);
        SELiquidSubstanceQuantity* HCO3 = cmpt->GetSubstanceQuantity(*m_HCO3);

        ss << "InitializeBloodGases(*cmpts.GetLiquidCompartment(pulse::VascularCompartment::" << cmpt->GetName() << "), Hb_total_mM, " << O2->GetSaturation() << ", " << O2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L) << ", " << CO2->GetSaturation() << ", " << CO2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L) << ", " << HCO3->GetMolarity(AmountPerVolumeUnit::mmol_Per_L) << ", " << cmpt->GetPH().GetValue() << ");";
        Info(ss);
      }
    }
  }
  void SubstanceManager::WritePulmonaryGases()
  {
    std::stringstream ss;
    std::vector<SEGasCompartment*> cmpts;
    cmpts.push_back(m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::Carina));
    cmpts.push_back(m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::LeftAnatomicDeadSpace));
    cmpts.push_back(m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::LeftAlveoli));
    cmpts.push_back(m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::RightAnatomicDeadSpace));
    cmpts.push_back(m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::RightAlveoli));
    for (SEGasCompartment* cmpt : cmpts)
    {
      if (cmpt->HasVolume())
      {
        SEGasSubstanceQuantity* O2 = cmpt->GetSubstanceQuantity(*m_O2);
        SEGasSubstanceQuantity* CO2 = cmpt->GetSubstanceQuantity(*m_CO2);
        SEGasSubstanceQuantity* N2 = cmpt->GetSubstanceQuantity(*m_N2);
        ss << cmpt->GetName() << "->GetSubstanceQuantity(*m_CO2)->GetVolumeFraction().SetValue(" << CO2->GetVolumeFraction() << ");" << std::endl;
        ss << cmpt->GetName() << "->GetSubstanceQuantity(*m_O2)->GetVolumeFraction().SetValue(" << O2->GetVolumeFraction() << ");" << std::endl;
        ss << cmpt->GetName() << "->GetSubstanceQuantity(*m_N2)->GetVolumeFraction().SetValue(" << N2->GetVolumeFraction() << ");" << std::endl;
        Info(ss);
      }
    }
  }

  void SubstanceManager::InitializeLiquidCompartmentNonGases()
  {
    const std::vector<SELiquidCompartment*>& urine = m_data.GetCompartments().GetUrineLeafCompartments();
    const std::vector<SELiquidCompartment*>& vascular = m_data.GetCompartments().GetVascularLeafCompartments();
    const std::vector<SETissueCompartment*>& tissue = m_data.GetCompartments().GetTissueLeafCompartments();

    SELiquidSubstanceQuantity* subQ;
    // Initialize Substances throughout the body
    SEScalarMassPerVolume   concentration;
    SEScalarAmountPerVolume molarity1;
    SEScalarAmountPerVolume molarity2;

    // Urine cmpts have to be explicitly set to values
    // Upper Tract
    SELiquidCompartment* leftBowmansCapsules = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftBowmansCapsules);
    SELiquidCompartment* rightBowmansCapsules = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightBowmansCapsules);
    SELiquidCompartment* leftTubules = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftTubules);
    SELiquidCompartment* rightTubules = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightTubules);
    SELiquidCompartment* leftUreter = m_data.GetCompartments().GetLiquidCompartment(pulse::UrineCompartment::LeftUreter);
    SELiquidCompartment* rightUreter = m_data.GetCompartments().GetLiquidCompartment(pulse::UrineCompartment::RightUreter);
    // Lower Tract
    // Note I don't modify the urethra, it's just a flow pipe, with no volume, hence, no substance quantities (NaN)
    //SELiquidCompartment* urethra = m_data.GetCompartments().GetUrineCompartment("Urethra");
    SELiquidCompartment* bladder = m_data.GetCompartments().GetLiquidCompartment(pulse::UrineCompartment::Bladder);

    // ACETOACETATE //
    concentration.SetValue(9.19, MassPerVolumeUnit::mg_Per_L);
    SetSubstanceConcentration(*m_acetoacetate, vascular, concentration);
    // Set Urine
    subQ = leftBowmansCapsules->GetSubstanceQuantity(*m_acetoacetate);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    subQ = rightBowmansCapsules->GetSubstanceQuantity(*m_acetoacetate);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    leftTubules->GetSubstanceQuantity(*m_acetoacetate)->GetConcentration().SetValue(0.0, MassPerVolumeUnit::mg_Per_dL);
    leftTubules->GetSubstanceQuantity(*m_acetoacetate)->Balance(BalanceLiquidBy::Concentration);
    rightTubules->GetSubstanceQuantity(*m_acetoacetate)->GetConcentration().SetValue(0.0, MassPerVolumeUnit::mg_Per_dL);
    rightTubules->GetSubstanceQuantity(*m_acetoacetate)->Balance(BalanceLiquidBy::Concentration);
    leftUreter->GetSubstanceQuantity(*m_acetoacetate)->GetConcentration().SetValue(0.0, MassPerVolumeUnit::mg_Per_dL);
    leftUreter->GetSubstanceQuantity(*m_acetoacetate)->Balance(BalanceLiquidBy::Concentration);
    rightUreter->GetSubstanceQuantity(*m_acetoacetate)->GetConcentration().SetValue(0.0, MassPerVolumeUnit::mg_Per_dL);
    rightUreter->GetSubstanceQuantity(*m_acetoacetate)->Balance(BalanceLiquidBy::Concentration);
    bladder->GetSubstanceQuantity(*m_acetoacetate)->GetConcentration().SetValue(0.0, MassPerVolumeUnit::mg_Per_dL);
    bladder->GetSubstanceQuantity(*m_acetoacetate)->Balance(BalanceLiquidBy::Concentration);
    // Tissue
    molarity1.SetValue(0.09, AmountPerVolumeUnit::mmol_Per_L);
    SetSubstanceMolarity(*m_acetoacetate, tissue, molarity1);

    // ALBUMIN //
    concentration.SetValue(3.7, MassPerVolumeUnit::g_Per_dL);
    SetSubstanceConcentration(*m_albumin, vascular, concentration);
    // Set Urine
    concentration.SetValue(0.155, MassPerVolumeUnit::mg_Per_dL);
    subQ = leftBowmansCapsules->GetSubstanceQuantity(*m_albumin);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    subQ = rightBowmansCapsules->GetSubstanceQuantity(*m_albumin);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    leftTubules->GetSubstanceQuantity(*m_albumin)->SetToZero();
    rightTubules->GetSubstanceQuantity(*m_albumin)->SetToZero();
    leftUreter->GetSubstanceQuantity(*m_albumin)->SetToZero();
    rightUreter->GetSubstanceQuantity(*m_albumin)->SetToZero();
    bladder->GetSubstanceQuantity(*m_albumin)->SetToZero();
    // Tissue
    molarity1.SetValue(0.0, AmountPerVolumeUnit::mmol_Per_L);
    SetSubstanceMolarity(*m_albumin, tissue, molarity1);

    // BICARBONATE IS SET IN GASES SECTION //

    // CALCIUM //
    concentration.SetValue(48.1, MassPerVolumeUnit::mg_Per_L);
    SetSubstanceConcentration(*m_calcium, vascular, concentration);
    SetSubstanceConcentration(*m_calcium, urine, concentration);
    // Tissue
    molarity1.SetValue(1.2, AmountPerVolumeUnit::mmol_Per_L);
    molarity2.SetValue(0.0001, AmountPerVolumeUnit::mmol_Per_L);
    SetSubstanceMolarity(*m_calcium, tissue, molarity1, molarity2);

    // CHLORIDE //
    concentration.SetValue(0.362, MassPerVolumeUnit::g_Per_dL);
    SetSubstanceConcentration(*m_chloride, vascular, concentration);
    SetSubstanceConcentration(*m_chloride, urine, concentration);
    // Tissue
    molarity1.SetValue(116, AmountPerVolumeUnit::mmol_Per_L);
    molarity2.SetValue(20, AmountPerVolumeUnit::mmol_Per_L);
    SetSubstanceMolarity(*m_chloride, tissue, molarity1, molarity2);

    // CREATININE //
    concentration.SetValue(1.2, MassPerVolumeUnit::mg_Per_dL);
    SetSubstanceConcentration(*m_creatinine, vascular, concentration);
    // Set Urine
    concentration.SetValue(0.006, MassPerVolumeUnit::mg_Per_dL);
    subQ = leftBowmansCapsules->GetSubstanceQuantity(*m_creatinine);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    subQ = rightBowmansCapsules->GetSubstanceQuantity(*m_creatinine);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    concentration.SetValue(1.25, MassPerVolumeUnit::g_Per_L);
    subQ = leftTubules->GetSubstanceQuantity(*m_creatinine);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    subQ = rightTubules->GetSubstanceQuantity(*m_creatinine);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    subQ = leftUreter->GetSubstanceQuantity(*m_creatinine);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    subQ = rightUreter->GetSubstanceQuantity(*m_creatinine);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    subQ = bladder->GetSubstanceQuantity(*m_creatinine);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    // Tissue
    molarity1.SetValue(0.044, AmountPerVolumeUnit::mmol_Per_L);
    SetSubstanceMolarity(*m_creatinine, tissue, molarity1);

    // EPINEPHRINE //
    concentration.SetValue(0.034, MassPerVolumeUnit::ug_Per_L);
    SetSubstanceConcentration(*m_epi, vascular, concentration);
    // Tissue
    molarity1.SetValue(1.8558e-7, AmountPerVolumeUnit::mmol_Per_L); //epinephrine: 183.2044 g/mol
    SetSubstanceMolarity(*m_epi, tissue, molarity1);

    // NOREPINEPHRINE //
    concentration.SetValue(0.275, MassPerVolumeUnit::ug_Per_L);
    SetSubstanceConcentration(*m_norepi, vascular, concentration);
    // Tissue
    molarity1.SetValue(2.7137e-7, AmountPerVolumeUnit::mmol_Per_L); //norepinephrine: 169.18 g/mol
    SetSubstanceMolarity(*m_norepi, tissue, molarity1);

    // GLUCOSE //
    concentration.SetValue(95, MassPerVolumeUnit::mg_Per_dL);
    SetSubstanceConcentration(*m_glucose, vascular, concentration);
    //Overwriting initialization for liver glucose since acts as the glucose pool for the body
    //subQ = liver->GetExtracellularFluid().GetSubstanceQuantity(*m_glucose);
    //subQ->GetMass().SetValue(500 * (m_data.GetPatient().GetWeight(MassUnit::kg) / 65.0), MassUnit::g);
    //subQ->Balance(BalanceLiquidBy::Mass);
    //subQ = liver->GetIntracellularFluid().GetSubstanceQuantity(*m_glucose);
    // subQ->GetMass().SetValue(500 * (m_data.GetPatient().GetWeight(MassUnit::kg) / 65.0), MassUnit::g);
    // subQ->Balance(BalanceLiquidBy::Mass);
    //Overwriting initialization for muscle glucose since acts as the glucose pool for the body
    //subQ = muscle->GetExtracellularFluid().GetSubstanceQuantity(*m_glucose);
    //subQ->GetMass().SetValue(100 * (m_data.GetPatient().GetWeight(MassUnit::kg) / 65.0), MassUnit::g);
    // subQ->Balance(BalanceLiquidBy::Mass);
    //subQ = muscle->GetIntracellularFluid().GetSubstanceQuantity(*m_glucose);
    // subQ->GetMass().SetValue(100 * (m_data.GetPatient().GetWeight(MassUnit::kg) / 65.0), MassUnit::g);
    // subQ->Balance(BalanceLiquidBy::Mass);
    // Only Bowmans has it
    subQ = leftBowmansCapsules->GetSubstanceQuantity(*m_glucose);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    subQ = rightBowmansCapsules->GetSubstanceQuantity(*m_glucose);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    leftTubules->GetSubstanceQuantity(*m_glucose)->SetToZero();
    rightTubules->GetSubstanceQuantity(*m_glucose)->SetToZero();
    leftUreter->GetSubstanceQuantity(*m_glucose)->SetToZero();
    rightUreter->GetSubstanceQuantity(*m_glucose)->SetToZero();
    bladder->GetSubstanceQuantity(*m_glucose)->SetToZero();
    // Tissue
    molarity1.SetValue(5.27321, AmountPerVolumeUnit::mmol_Per_L);//95 mg/dL
    molarity2.SetValue(0, AmountPerVolumeUnit::mmol_Per_L);
    SetSubstanceMolarity(*m_glucose, tissue, molarity1, molarity2);

    // INSULIN // Should be from 70-380 pmol/L (0.40656 - 2.20704 ug/L)
    concentration.SetValue(0.40656, MassPerVolumeUnit::ug_Per_L);
    SetSubstanceConcentration(*m_insulin, vascular, concentration);
    // None in Urine
    leftBowmansCapsules->GetSubstanceQuantity(*m_insulin)->SetToZero();
    rightBowmansCapsules->GetSubstanceQuantity(*m_insulin)->SetToZero();
    leftTubules->GetSubstanceQuantity(*m_insulin)->SetToZero();
    rightTubules->GetSubstanceQuantity(*m_insulin)->SetToZero();
    leftUreter->GetSubstanceQuantity(*m_insulin)->SetToZero();
    rightUreter->GetSubstanceQuantity(*m_insulin)->SetToZero();
    bladder->GetSubstanceQuantity(*m_insulin)->SetToZero();
    // Tissue
    molarity1.SetValue(71, AmountPerVolumeUnit::pmol_Per_L);
    SetSubstanceMolarity(*m_insulin, tissue, molarity1);

    // LACTATE //
    concentration.SetValue(142.5, MassPerVolumeUnit::mg_Per_L);
    SetSubstanceConcentration(*m_lactate, vascular, concentration);
    //set in tubules zero in urine
    concentration.SetValue(1.5, MassPerVolumeUnit::g_Per_L);
    subQ = leftTubules->GetSubstanceQuantity(*m_lactate);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    subQ = rightTubules->GetSubstanceQuantity(*m_lactate);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    bladder->GetSubstanceQuantity(*m_lactate)->SetToZero();
    //Clear Lactate out of the Ureter to initialize for Gluconeogenesis
    leftUreter->GetSubstanceQuantity(*m_lactate)->SetToZero();
    rightUreter->GetSubstanceQuantity(*m_lactate)->SetToZero();
    // Tissue
    molarity1.SetValue(0.09, AmountPerVolumeUnit::mmol_Per_L);
    SetSubstanceMolarity(*m_lactate, tissue, molarity1);

    // POTASSIUM //
    concentration.SetValue(0.0156, MassPerVolumeUnit::g_Per_dL);
    SetSubstanceConcentration(*m_potassium, vascular, concentration);
    SetSubstanceConcentration(*m_potassium, urine, concentration);
    // Tissue
    molarity1.SetValue(4.5, AmountPerVolumeUnit::mmol_Per_L);
    molarity2.SetValue(120, AmountPerVolumeUnit::mmol_Per_L);
    SetSubstanceMolarity(*m_potassium, tissue, molarity1, molarity2);

    // SODIUM //
    concentration.SetValue(0.323, MassPerVolumeUnit::g_Per_dL);
    SetSubstanceConcentration(*m_sodium, vascular, concentration);
    // Set Urine
    subQ = leftBowmansCapsules->GetSubstanceQuantity(*m_sodium);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    subQ = rightBowmansCapsules->GetSubstanceQuantity(*m_sodium);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    concentration.SetValue(0.375, MassPerVolumeUnit::g_Per_dL);
    subQ = leftTubules->GetSubstanceQuantity(*m_sodium);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    subQ = rightTubules->GetSubstanceQuantity(*m_sodium);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    subQ = leftUreter->GetSubstanceQuantity(*m_sodium);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    subQ = rightUreter->GetSubstanceQuantity(*m_sodium);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    subQ = bladder->GetSubstanceQuantity(*m_sodium);
    subQ->GetConcentration().Set(concentration);
    // Tissue
    molarity1.SetValue(145, AmountPerVolumeUnit::mmol_Per_L);
    molarity2.SetValue(15, AmountPerVolumeUnit::mmol_Per_L);
    SetSubstanceMolarity(*m_sodium, tissue, molarity1, molarity2);

    // TRISTEARIN
    /// \todo Tristearin is actually all triacylglycerols. Change name.
    concentration.SetValue(100.0, MassPerVolumeUnit::mg_Per_dL);
    SetSubstanceConcentration(*m_tristearin, vascular, concentration);
    //Overwriting initialization for fatty mcfat fat since it acts as the fat pool for the body
    //subQ = fat->GetExtracellularFluid().GetSubstanceQuantity(*m_tristearin);
    //subQ->GetMass().SetValue(m_data.GetPatient().GetBodyFatFraction().GetValue()*m_data.GetPatient().GetWeight(MassUnit::g), MassUnit::g);
    //subQ->Balance(BalanceLiquidBy::Mass);
    //subQ = fat->GetIntracellularFluid().GetSubstanceQuantity(*m_tristearin);
    //subQ->GetMass().SetValue(m_data.GetPatient().GetBodyFatFraction().GetValue()*m_data.GetPatient().GetWeight(MassUnit::g), MassUnit::g);
    //subQ->Balance(BalanceLiquidBy::Mass);
    // None in Urine
    leftBowmansCapsules->GetSubstanceQuantity(*m_tristearin)->SetToZero();
    rightBowmansCapsules->GetSubstanceQuantity(*m_tristearin)->SetToZero();
    leftTubules->GetSubstanceQuantity(*m_tristearin)->SetToZero();
    rightTubules->GetSubstanceQuantity(*m_tristearin)->SetToZero();
    leftUreter->GetSubstanceQuantity(*m_tristearin)->SetToZero();
    rightUreter->GetSubstanceQuantity(*m_tristearin)->SetToZero();
    bladder->GetSubstanceQuantity(*m_tristearin)->SetToZero();
    // Tissue
    molarity1.SetValue(0.09, AmountPerVolumeUnit::mmol_Per_L);
    SetSubstanceMolarity(*m_tristearin, tissue, molarity1);

    // UREA //
    concentration.SetValue(23.0, MassPerVolumeUnit::mg_Per_dL);
    SetSubstanceConcentration(*m_urea, vascular, concentration);
    // Set Urine
    concentration.SetValue(0.2, MassPerVolumeUnit::mg_Per_dL);
    subQ = leftBowmansCapsules->GetSubstanceQuantity(*m_urea);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    subQ = rightBowmansCapsules->GetSubstanceQuantity(*m_urea);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    concentration.SetValue(20.0, MassPerVolumeUnit::g_Per_L);
    subQ = leftTubules->GetSubstanceQuantity(*m_urea);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    subQ = rightTubules->GetSubstanceQuantity(*m_urea);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    subQ = leftUreter->GetSubstanceQuantity(*m_urea);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    subQ = rightUreter->GetSubstanceQuantity(*m_urea);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    subQ = bladder->GetSubstanceQuantity(*m_urea);
    subQ->GetConcentration().Set(concentration);
    subQ->Balance(BalanceLiquidBy::Concentration);
    // Tissue
    molarity1.SetValue(4.8, AmountPerVolumeUnit::mmol_Per_L);
    SetSubstanceMolarity(*m_urea, tissue, molarity1);
  }

  bool SubstanceManager::LoadSubstanceDirectory(const std::string& data_dir)
  {
    if (!SESubstanceManager::LoadSubstanceDirectory(data_dir))
      return false;
    return Setup();
  }

  void SubstanceManager::AddActiveSubstance(const SESubstance& substance)
  {
    if (IsActive(substance))
      return;// If its already active, don't do anything

    SESubstanceManager::AddActiveSubstance(substance);
    if (substance.GetState() == eSubstance_State::Gas)
      m_data.GetCompartments().AddGasCompartmentSubstance(const_cast<SESubstance&>(substance));
    m_data.GetCompartments().AddLiquidCompartmentSubstance(const_cast<SESubstance&>(substance));

    if (&substance == m_CO)// We need to put HbCO in the system is CO is in the system
    {
      m_isCOActive = true;
      AddActiveSubstance(*m_HbCO);
    }
  }

  bool SubstanceManager::IsActive(const SESubstance& sub) const
  {
    if (&sub == m_CO)
      return m_isCOActive;
    return SESubstanceManager::IsActive(sub);
  }



  /// --------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates the substance mass cleared for a node
  ///
  /// \details
  /// The volume cleared, the compartment, and the substance are provided to clear the mass of the substance
  /// from the node. This generic methodology can be used by other systems to calculate the mass cleared.
  //--------------------------------------------------------------------------------------------------
  void SubstanceManager::CalculateGenericClearance(double volumeCleared_mL, SELiquidCompartment& cmpt, SESubstance& sub, SEScalarMass* cleared)
  {
    SELiquidSubstanceQuantity* subQ = cmpt.GetSubstanceQuantity(sub);
    if (subQ == nullptr)
      throw CommonDataModelException("No Substance Quantity found for substance " + sub.GetName());
    //GetMass and Concentration from the compartment
    double mass_ug = subQ->GetMass(MassUnit::ug);
    double concentration_ug_Per_mL = subQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL);

    //Calculate Mass Cleared
    double MassCleared_ug = volumeCleared_mL * concentration_ug_Per_mL;
    //Ensure mass does not become negative
    mass_ug -= MassCleared_ug;
    if (mass_ug < 0)
    {
      mass_ug = 0;
    }

    MassCleared_ug = subQ->GetMass(MassUnit::ug) - mass_ug;
    subQ->GetMass().SetValue(mass_ug, MassUnit::ug);
    subQ->Balance(BalanceLiquidBy::Mass);

    sub.GetSystemicMassCleared().Increment(MassCleared_ug, MassUnit::ug);
    if (cleared != nullptr)
      cleared->SetValue(MassCleared_ug, MassUnit::ug);
  }

  /// --------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates the substance mass cleared for a tissue compartment based on a volume
  /// 
  /// \param VolumeCleared_mL: the volume of fluid cleared of a substance by some process
  /// \param tissue: a tissue compartment
  /// \param sub: a substance
  /// \param cleared: mass cleared
  ///
  /// \details
  /// The volume cleared, the compartment, and the substance are provided to clear the mass of the substance
  /// from the node. This generic methodology can be used by other systems to calculate the mass cleared.
  //--------------------------------------------------------------------------------------------------
  void SubstanceManager::CalculateGenericClearance(double VolumeCleared_mL, SETissueCompartment& tissue, SESubstance& sub, SEScalarMass* cleared)
  {
    SELiquidSubstanceQuantity* subQ =tissue.GetIntracellular().GetSubstanceQuantity(sub);
    if (subQ == nullptr)
      throw CommonDataModelException("No Substance Quantity found for substance " + sub.GetName());
    //GetMass and Concentration from the compartment
    double mass_ug = subQ->GetMass(MassUnit::ug);
    double concentration_ug_Per_mL;
    SEScalarMassPerVolume concentration;
    if (sub.HasPK())
    {
      if (!GeneralMath::CalculateConcentration(subQ->GetMass(), tissue.GetMatrixVolume(), concentration, m_Logger))
        Error("  Compartment : " + tissue.GetName() + ", Substance : " + sub.GetName());
      concentration_ug_Per_mL = concentration.GetValue(MassPerVolumeUnit::ug_Per_mL);
    }
    else
    {
      concentration_ug_Per_mL = subQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL);
    }

    //Calculate Mass Cleared
    double MassCleared_ug = VolumeCleared_mL * concentration_ug_Per_mL;
    //Ensure mass does not become negative
    mass_ug -= MassCleared_ug;
    if (mass_ug < 0)
    {
      mass_ug = 0;
    }

    MassCleared_ug = subQ->GetMass(MassUnit::ug) - mass_ug;
    subQ->GetMass().SetValue(mass_ug, MassUnit::ug);
    subQ->Balance(BalanceLiquidBy::Mass);
    subQ->GetMassCleared().Increment(MassCleared_ug, MassUnit::ug);
    if (cleared != nullptr)
      cleared->SetValue(MassCleared_ug, MassUnit::ug);
  }

  /// --------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates the substance mass excreted for a compartment
  ///
  /// \details
  /// The volume cleared, the compartment, and the substance are provided to clear the mass of the substance
  /// from the node. This generic methodology can be used by other systems to calculate the mass excreted.
  //--------------------------------------------------------------------------------------------------
  void SubstanceManager::CalculateGenericExcretion(double VascularFlow_mL_Per_s, SETissueCompartment& tissue, SESubstance& sub, double FractionExcreted, double timestep_s, SEScalarMass* excreted)
  {
    SELiquidSubstanceQuantity* subQ = tissue.GetIntracellular().GetSubstanceQuantity(sub);
    if (subQ == nullptr)
      throw CommonDataModelException("No Substance Quantity found for substance " + sub.GetName());
    double concentration_ug_Per_mL;
    SEScalarMassPerVolume concentration;
    if (sub.HasPK())
    {
      if (!GeneralMath::CalculateConcentration(subQ->GetMass(), tissue.GetMatrixVolume(), concentration, m_Logger))
        Error("  Compartment : " + tissue.GetName() + ", Substance : " + sub.GetName());
      concentration_ug_Per_mL = concentration.GetValue(MassPerVolumeUnit::ug_Per_mL);
    }
    else
    {
      concentration_ug_Per_mL = subQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL);
    }

    double MassExcreted_ug = VascularFlow_mL_Per_s * concentration_ug_Per_mL * timestep_s * 0.5 * FractionExcreted; //0.5  is the tuning parameter to remove the correct percentage.

    double mass_ug = subQ->GetMass().GetValue(MassUnit::ug);
    mass_ug = subQ->GetMass().GetValue(MassUnit::ug) - MassExcreted_ug;
    if (mass_ug < 0)
      mass_ug = 0;

    MassExcreted_ug = subQ->GetMass().GetValue(MassUnit::ug) - mass_ug;
    subQ->GetMass().SetValue(mass_ug, MassUnit::ug);
    subQ->Balance(BalanceLiquidBy::Mass);
    subQ->GetMassExcreted().Increment(MassExcreted_ug, MassUnit::ug);
    if (excreted != nullptr)
      excreted->SetValue(MassExcreted_ug, MassUnit::ug);
  }

  void SubstanceManager::ProbeBloodGases(SELiquidCompartment& cmpt, const std::string& prefix)
  {
    SELiquidSubstanceQuantity* O2 = cmpt.GetSubstanceQuantity(*m_O2);
    SELiquidSubstanceQuantity* Hb = cmpt.GetSubstanceQuantity(*m_Hb);
    SELiquidSubstanceQuantity* HbO2 = cmpt.GetSubstanceQuantity(*m_HbO2);
    SELiquidSubstanceQuantity* HbO2CO2 = cmpt.GetSubstanceQuantity(*m_HbO2CO2);
    SELiquidSubstanceQuantity* CO2 = cmpt.GetSubstanceQuantity(*m_CO2);
    SELiquidSubstanceQuantity* HbCO2 = cmpt.GetSubstanceQuantity(*m_HbCO2);
    SELiquidSubstanceQuantity* HCO3 = cmpt.GetSubstanceQuantity(*m_HCO3);

    m_data.GetDataTrack().Probe(cmpt.GetName() + prefix + "_pH", cmpt.GetPH().GetValue());
    m_data.GetDataTrack().Probe(cmpt.GetName() + prefix + "_O2_ug", O2->GetMass(MassUnit::ug));
    m_data.GetDataTrack().Probe(cmpt.GetName() + prefix + "_O2_mmol_Per_L", O2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L));
    m_data.GetDataTrack().Probe(cmpt.GetName() + prefix + "_O2_mmHg", O2->GetPartialPressure(PressureUnit::mmHg));
    m_data.GetDataTrack().Probe(cmpt.GetName() + prefix + "_O2_sat", O2->GetSaturation().GetValue());
    m_data.GetDataTrack().Probe(cmpt.GetName() + prefix + "_CO2_ug", CO2->GetMass(MassUnit::ug));
    m_data.GetDataTrack().Probe(cmpt.GetName() + prefix + "_CO2_mmol_Per_L", CO2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L));
    m_data.GetDataTrack().Probe(cmpt.GetName() + prefix + "_CO2_mmHg", CO2->GetPartialPressure(PressureUnit::mmHg));
    m_data.GetDataTrack().Probe(cmpt.GetName() + prefix + "_CO2_sat", CO2->GetSaturation().GetValue());
    m_data.GetDataTrack().Probe(cmpt.GetName() + prefix + "_Hb_ug", Hb->GetMass(MassUnit::ug));
    m_data.GetDataTrack().Probe(cmpt.GetName() + prefix + "_Hb_mmol_Per_L", Hb->GetMolarity(AmountPerVolumeUnit::mmol_Per_L));
    m_data.GetDataTrack().Probe(cmpt.GetName() + prefix + "_HbO2_ug", HbO2->GetMass(MassUnit::ug));
    m_data.GetDataTrack().Probe(cmpt.GetName() + prefix + "_HbO2_mmol_Per_L", HbO2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L));
    m_data.GetDataTrack().Probe(cmpt.GetName() + prefix + "_HbCO2_ug", HbCO2->GetMass(MassUnit::ug));
    m_data.GetDataTrack().Probe(cmpt.GetName() + prefix + "_HbCO2_mmol_Per_L", HbCO2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L));
    m_data.GetDataTrack().Probe(cmpt.GetName() + prefix + "_HbO2CO2_ug", HbO2CO2->GetMass(MassUnit::ug));
    m_data.GetDataTrack().Probe(cmpt.GetName() + prefix + "_HbO2CO2_mmol_Per_L", HbO2CO2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L));
    m_data.GetDataTrack().Probe(cmpt.GetName() + prefix + "_HCO3_ug", HCO3->GetMass(MassUnit::ug));
    m_data.GetDataTrack().Probe(cmpt.GetName() + prefix + "_HCO3_mmol_Per_L", HCO3->GetMolarity(AmountPerVolumeUnit::mmol_Per_L));
    m_data.GetDataTrack().Probe(cmpt.GetName() + prefix + "_TotalO2_mmol_per_L", O2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L) + HbO2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L) + HbO2CO2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L));
    m_data.GetDataTrack().Probe(cmpt.GetName() + prefix + "_TotalCO2_mmol_per_L", CO2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L) + HCO3->GetMolarity(AmountPerVolumeUnit::mmol_Per_L) + +HbCO2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L) + HbO2CO2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L));
    m_data.GetDataTrack().Probe(cmpt.GetName() + prefix + "_TotalHb_mmol_per_L", Hb->GetMolarity(AmountPerVolumeUnit::mmol_Per_L) + HbO2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L) + HbCO2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L) + HbO2CO2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L));
    m_data.GetDataTrack().Probe(cmpt.GetName() + prefix + "_TotalMass_mmol_per_L", O2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L) + CO2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L) + HCO3->GetMolarity(AmountPerVolumeUnit::mmol_Per_L) + Hb->GetMolarity(AmountPerVolumeUnit::mmol_Per_L) + HbO2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L) + HbCO2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L) + HbO2CO2->GetMolarity(AmountPerVolumeUnit::mmol_Per_L));
  }

  double SubstanceManager::GetSubstanceMass(SESubstance& sub, const std::vector<SELiquidCompartment*>& cmpts, const MassUnit& unit)
  {
    double mass = 0;
    SELiquidSubstanceQuantity* subQ;
    for (SELiquidCompartment* cmpt : cmpts)
    {
      subQ = cmpt->GetSubstanceQuantity(sub);
      mass += subQ->GetMass(unit);
    }
    return mass;
  }
  double SubstanceManager::GetSubstanceMass(SESubstance& sub, const std::vector<SETissueCompartment*>& cmpts, const MassUnit& unit)
  {
    double mass = 0;
    SELiquidSubstanceQuantity* subQ;
    for (auto tissue : m_data.GetCompartments().GetTissueCompartments())
    {
      subQ = tissue->GetExtracellular().GetSubstanceQuantity(sub);
      mass += subQ->GetMass(unit);

      subQ = tissue->GetIntracellular().GetSubstanceQuantity(sub);
      mass += subQ->GetMass(unit);
    }
    return mass;
  }

  void SubstanceManager::SetSubstanceConcentration(SESubstance& sub, const std::vector<SELiquidCompartment*>& cmpts, const SEScalarMassPerVolume& concentration)
  {
    SELiquidSubstanceQuantity* subQ;
    for (SELiquidCompartment* cmpt : cmpts)
    {
      subQ = cmpt->GetSubstanceQuantity(sub);
      subQ->GetConcentration().Set(concentration);
      subQ->Balance(BalanceLiquidBy::Concentration);
      //std::cout << sub.GetName() << " " << subQ->GetMolarity(AmountPerVolumeUnit::mmol_Per_L) <<"\n";
    }
  }
  void SubstanceManager::SetSubstanceConcentration(SESubstance& sub, const std::vector<SETissueCompartment*>& cmpts, const SEScalarMassPerVolume& concentration)
  {
    SELiquidSubstanceQuantity* subQ;
    for (auto tissue : m_data.GetCompartments().GetTissueCompartments())
    {
      subQ = tissue->GetExtracellular().GetSubstanceQuantity(sub);
      subQ->GetConcentration().Set(concentration);
      subQ->Balance(BalanceLiquidBy::Concentration);

      subQ = tissue->GetIntracellular().GetSubstanceQuantity(sub);
      subQ->GetConcentration().Set(concentration);
      subQ->Balance(BalanceLiquidBy::Concentration);
    }
  }
  void SubstanceManager::SetSubstanceConcentration(SESubstance& sub, const std::vector<SETissueCompartment*>& cmpts, const SEScalarMassPerVolume& extracellular, const SEScalarMassPerVolume& intracellular)
  {
    SELiquidSubstanceQuantity* subQ;
    for (auto tissue : m_data.GetCompartments().GetTissueCompartments())
    {
      subQ = tissue->GetExtracellular().GetSubstanceQuantity(sub);
      subQ->GetConcentration().Set(extracellular);
      subQ->Balance(BalanceLiquidBy::Concentration);

      subQ = tissue->GetIntracellular().GetSubstanceQuantity(sub);
      subQ->GetConcentration().Set(intracellular);
      subQ->Balance(BalanceLiquidBy::Concentration);
    }
  }

  void SubstanceManager::SetSubstanceMolarity(SESubstance& sub, const std::vector<SELiquidCompartment*>& cmpts, const SEScalarAmountPerVolume& molarity)
  {
    SELiquidSubstanceQuantity* subQ;
    for (SELiquidCompartment* cmpt : cmpts)
    {
      subQ = cmpt->GetSubstanceQuantity(sub);
      subQ->GetMolarity().Set(molarity);
      subQ->Balance(BalanceLiquidBy::Molarity);
    }
  }
  void SubstanceManager::SetSubstanceMolarity(SESubstance& sub, const std::vector<SETissueCompartment*>& cmpts, const SEScalarAmountPerVolume& molarity)
  {
    SELiquidSubstanceQuantity* subQ;
    for (auto tissue : m_data.GetCompartments().GetTissueCompartments())
    {
      subQ = tissue->GetExtracellular().GetSubstanceQuantity(sub);
      subQ->GetMolarity().Set(molarity);
      subQ->Balance(BalanceLiquidBy::Molarity);
      //std::cout << sub.GetName() << " " << subQ->GetConcentration(MassPerVolumeUnit::ug_Per_L) <<"\n";

      subQ = tissue->GetIntracellular().GetSubstanceQuantity(sub);
      subQ->GetMolarity().Set(molarity);
      subQ->Balance(BalanceLiquidBy::Molarity);
    }
  }
  void SubstanceManager::SetSubstanceMolarity(SESubstance& sub, const std::vector<SETissueCompartment*>& cmpts, const SEScalarAmountPerVolume& extracellular, const SEScalarAmountPerVolume& intracellular)
  {
    SELiquidSubstanceQuantity* subQ;
    for (auto tissue : m_data.GetCompartments().GetTissueCompartments())
    {
      subQ = tissue->GetExtracellular().GetSubstanceQuantity(sub);
      subQ->GetMolarity().Set(extracellular);
      subQ->Balance(BalanceLiquidBy::Molarity);
      //std::cout << sub.GetName() << subQ->GetConcentration(MassPerVolumeUnit::mg_Per_dL) << " mg/dL \n";

      subQ = tissue->GetIntracellular().GetSubstanceQuantity(sub);
      subQ->GetMolarity().Set(intracellular);
      subQ->Balance(BalanceLiquidBy::Molarity);
    }
  }

  const SizeIndependentDepositionEfficencyCoefficient& SubstanceManager::GetSizeIndependentDepositionEfficencyCoefficient(const SESubstance& substance)
  {
    auto itr = m_SIDECoefficients.find(&substance);
    if (itr != m_SIDECoefficients.end())
      return *itr->second;

    if (!substance.HasAerosolization())
      Fatal("Cannot generate a SIDE Coefficient if no aerosolization data is provided");
    if (!substance.GetAerosolization()->HasParticulateSizeDistribution())
      Fatal("Cannot generate a SIDE Coefficient if no particulate distribution is provided");

    // This fraction vs. length histogram characterizes the size distribution for a polydispersed aerosol.
    // Length is the aerodynamic diameter of a particle, and Fraction is the fraction of the total
    // that are contained in bin i, which is bounded by boundary i and boundary i+1. 
    // The deposition fraction for each bin is computed from the equations fit by Hinds to the ICRP
    // model data @cite Rostami2009computationsal with as much resolution as is provided by the histogram.
    // Regardless of the provided resolution, particles with aerodynamic diameter less than 10^-4 micrometers
    // all deposit in the head and particles greater than 100 micrometers deposit 50% in the head with 50% loss.
    // For more information see the [aerosol](@ref environment-aerosol) section in the @ref EnvironmentMethodology report.
    // A histogram with n bins must contain n+1 boundaries
    const SEHistogramFractionVsLength* concentrations = substance.GetAerosolization()->GetParticulateSizeDistribution();

    // Check to make sure everything is in the right form.
    if (!concentrations->IsVaild())
      Fatal("Particle distribution histogram is not valid");

    // First we need compartment-specific deposition fractions for each size
    SEHistogramFractionVsLength depositionsAirway;
    SEHistogramFractionVsLength depositionsCarina;
    SEHistogramFractionVsLength depositionsAnatomicalDeadspace;
    SEHistogramFractionVsLength depositionsAlveoli;
    // Copy sizes
    depositionsAirway.GetLength() = concentrations->GetLength();
    depositionsCarina.GetLength() = concentrations->GetLength();
    depositionsAnatomicalDeadspace.GetLength() = concentrations->GetLength();
    depositionsAlveoli.GetLength() = concentrations->GetLength();

    int numPerRegion = 1000; // This is how many times the equations are evaluated in a bin to get a mean deposition fraction
    for (size_t i = 0; i < concentrations->NumberOfBins(); i++)
    {
      double binLength = concentrations->GetLength()[i + 1] - concentrations->GetLength()[i];
      double stepSize = binLength / double(numPerRegion); //
      double sumHeadAirways = 0.;
      double sumAnatomicalDeadspace = 0.;
      double sumAlveoli = 0.;
      double aerodynamicDiameter;
      for (int j = 0; j < numPerRegion; j++)
      {
        aerodynamicDiameter = concentrations->GetLength()[i] + stepSize * j; //Start at the bottom of the bin and march towards the top minus one step
        if (aerodynamicDiameter == 0)
          continue;
        double inspirFrac = 1 - 0.5 * (1 - 1 / (1 + 0.00076 * pow(aerodynamicDiameter, 2.8)));
        sumHeadAirways += inspirFrac * (1 / (1 + exp(6.84 + 1.183 * log(aerodynamicDiameter))) + 1 / (1 + exp(0.924 - 1.885 * log(aerodynamicDiameter))));
        sumAnatomicalDeadspace += 0.00352 / aerodynamicDiameter * (exp(-0.23 * pow((log(aerodynamicDiameter) + 3.4), 2)) + 63.9 * exp(-0.819 * pow((log(aerodynamicDiameter) - 1.61), 2)));
        sumAlveoli += 0.0155 / aerodynamicDiameter * (exp(-0.416 * pow((log(aerodynamicDiameter) + 2.84), 2)) + 19.11 * exp(-0.482 * pow((log(aerodynamicDiameter) - 1.362), 2)));
      }
      // Mean this region
      depositionsAirway.GetFraction().push_back(sumHeadAirways / numPerRegion);
      depositionsAnatomicalDeadspace.GetFraction().push_back(sumAnatomicalDeadspace / numPerRegion);
      depositionsAlveoli.GetFraction().push_back(sumAlveoli / numPerRegion);
      // If any fractions are more than one, weight the error and distribute
      // More than 1.0 can happen with small particles, possibly due to
      // truncation in the equations in @cite Rostami2009computational.
      if (depositionsAirway.GetFraction()[i] + depositionsAnatomicalDeadspace.GetFraction()[i] + depositionsAlveoli.GetFraction()[i] > 1.0)
      {
        double mag = depositionsAirway.GetFraction()[i] + depositionsAnatomicalDeadspace.GetFraction()[i] + depositionsAlveoli.GetFraction()[i];
        double delta = 1.0 - mag;
        depositionsAirway.GetFraction()[i] = depositionsAirway.GetFraction()[i] * (1. + delta / mag);
        depositionsAnatomicalDeadspace.GetFraction()[i] = depositionsAnatomicalDeadspace.GetFraction()[i] * (1. + delta / mag);
        depositionsAlveoli.GetFraction()[i] = depositionsAlveoli.GetFraction()[i] * (1. + delta / mag);
      }
      // Head airways are split between Airway and carina
      double carinaFraction = 0.5;
      depositionsCarina.GetFraction().push_back(depositionsAirway.GetFraction()[i] * carinaFraction);
      depositionsAirway.GetFraction()[i] *= (1 - carinaFraction);
    }

    // Now we can compute the size-independent deposition efficiencies for each compartment
    // The fraction of the total that deposits in the Airway is equal to the sum of the 
    // products of the fraction of each size times the deposition fractions of each size for the Airway.
    // The fraction of the total that deposits in each subsequent windward compartment is the sum of the 
    // products of the fraction of each size times the deposition fractions of each size for the compartment
    // divided by the sum of (1-deposition fraction)*concentration fraction for the leeward compartment.
    double sumAirwayProducts = 0.;
    double sumAirwayOneMinusProducts = 0.;
    double sumCarinaProducts = 0.;
    double sumCarinaOneMinusProducts = 0.;
    double sumDeadspaceProducts = 0.;
    double sumDeadspaceOneMinusProducts = 0.;
    double sumAlveoliProducts = 0.;

    for (size_t i = 0; i < concentrations->NumberOfBins(); i++)
    {
      sumAirwayProducts += concentrations->GetFraction()[i] * depositionsAirway.GetFraction()[i];
      sumAirwayOneMinusProducts += concentrations->GetFraction()[i] * (1 - depositionsAirway.GetFraction()[i]);
      sumCarinaProducts += concentrations->GetFraction()[i] * depositionsCarina.GetFraction()[i];
      sumCarinaOneMinusProducts += concentrations->GetFraction()[i] * (1 - depositionsCarina.GetFraction()[i]);
      sumDeadspaceProducts += concentrations->GetFraction()[i] * depositionsAnatomicalDeadspace.GetFraction()[i];
      sumDeadspaceOneMinusProducts += concentrations->GetFraction()[i] * (1 - depositionsAnatomicalDeadspace.GetFraction()[i]);
      sumAlveoliProducts += concentrations->GetFraction()[i] * depositionsAlveoli.GetFraction()[i];
    }

    SizeIndependentDepositionEfficencyCoefficient* SIDECoefficients = new SizeIndependentDepositionEfficencyCoefficient();
    SIDECoefficients->m_airway = sumAirwayProducts;
    SIDECoefficients->m_carina = (sumAirwayOneMinusProducts < 1.0e-12) ? 0 : sumCarinaProducts / sumAirwayOneMinusProducts;
    SIDECoefficients->m_deadSpace = (sumCarinaOneMinusProducts < 1.0e-12) ? 0 : sumDeadspaceProducts / sumCarinaOneMinusProducts;
    SIDECoefficients->m_alveoli = (sumDeadspaceOneMinusProducts < 1.0e-12) ? 0 : sumAlveoliProducts / sumDeadspaceOneMinusProducts;
    m_SIDECoefficients[&substance] = SIDECoefficients;
    // Now we do not need to track size distributions, only mass (via concentration).

    return *SIDECoefficients;
  }
}
