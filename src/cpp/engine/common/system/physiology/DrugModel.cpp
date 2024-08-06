/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/PulseConfiguration.h"
#include "engine/common/controller/CircuitManager.h"
#include "engine/common/controller/CompartmentManager.h"
#include "engine/common/controller/SubstanceManager.h"
#include "engine/common/system/physiology/DrugModel.h"
// Dependent Systems
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/system/physiology/SETissueSystem.h"
#include "cdm/system/physiology/SEPupillaryResponse.h"
// Actions
#include "cdm/engine/SEActionManager.h"
#include "cdm/engine/SEPatientActionCollection.h"
#include "cdm/patient/actions/SESubstanceBolus.h"
#include "cdm/patient/actions/SESubstanceInfusion.h"
#include "cdm/patient/actions/SESubstanceCompoundInfusion.h"
// CDM
#include "patient/SEPatient.h"
#include "substance/SESubstance.h"
#include "substance/SESubstanceCompound.h"
#include "substance/SESubstanceConcentration.h"
#include "substance/SESubstanceClearance.h"
#include "substance/SESubstancePhysicochemicals.h"
#include "substance/SESubstancePharmacodynamics.h"
#include "substance/SESubstancePharmacokinetics.h"
#include "substance/SESubstanceTissuePharmacokinetics.h"
#include "circuit/fluid/SEFluidCircuit.h"
#include "compartment/tissue/SETissueCompartment.h"
#include "compartment/fluid/SELiquidCompartmentGraph.h"
#include "compartment/substances/SELiquidSubstanceQuantity.h"
#include "properties/SEScalarPressure.h"
#include "properties/SEScalarMassPerVolume.h"
#include "properties/SEScalarVolumePerTime.h"
#include "properties/SEScalar0To1.h"
#include "properties/SEScalarInversePressure.h"
#include "properties/SEScalarFrequency.h"
#include "properties/SEScalarVolume.h"
#include "properties/SEScalarMass.h"
#include "properties/SEScalarMassPerMass.h"
#include "properties/SEScalarArea.h"
#include "properties/SEScalarAreaPerTimePressure.h"
#include "properties/SEScalarTemperature.h"
#include "properties/SEScalarMassPerTime.h"
#include "properties/SEScalarTime.h"
#include "properties/SEScalarLength.h"
#include "properties/SEScalarMassPerAmount.h"
#include "properties/SEScalarAmountPerMass.h"
#include "properties/SEScalarVolumePerTimeMass.h"
#include "properties/SEScalarVolumePerTimePressure.h"
#include "properties/SEScalarNegative1To1.h"

namespace pulse
{
  DrugModel::DrugModel(Data& data) : DrugSystem(data.GetLogger()), Model(data)
  {
    Clear();
  }

  DrugModel::~DrugModel()
  {
    Clear();
  }

  void DrugModel::Clear()
  {
    DrugSystem::Clear();
    m_aortaVascular = nullptr;
    m_venaCavaVascular = nullptr;
    m_muscleIntracellular = nullptr;
    m_liverVascular = nullptr;
    m_liverTissue = nullptr;
    m_IVToVenaCava = nullptr;
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Initializes system properties to valid homeostatic values.
  ///
  /// \details
  /// For stabilization only!
  /// Called AFTER Setup when stabilizing a new patient
  //--------------------------------------------------------------------------------------------------
  void DrugModel::Initialize()
  {
    Model::Initialize();

    GetBronchodilationLevel().SetValue(0.0);
    GetHeartRateChange().SetValue(0.0, FrequencyUnit::Per_min);
    GetMeanBloodPressureChange().SetValue(0.0, PressureUnit::mmHg);
    GetNeuromuscularBlockLevel().SetValue(0.0);
    GetPulsePressureChange().SetValue(0.0, PressureUnit::mmHg);
    GetPupillaryResponse().GetSizeModifier().SetValue(0);
    GetPupillaryResponse().GetReactivityModifier().SetValue(0);
    GetRespirationRateChange().SetValue(0.0, FrequencyUnit::Per_min);
    GetSedationLevel().SetValue(0.0);
    GetTidalVolumeChange().SetValue(0.0, VolumeUnit::mL);
    GetTubularPermeabilityChange().SetValue(0);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Initializes parameters for the Drugs Class
  ///
  /// \details
  /// Called during both State loading and Patient Stabilization
  /// Pull and setup up our data (can be from other systems)
  /// Initialize will be called after this and can overwrite any of this data (only if stabilizing)
  //--------------------------------------------------------------------------------------------------
  void DrugModel::SetUp()
  {
    m_Saline = m_data.GetSubstances().GetCompound("Saline");
    m_Blood = m_data.GetSubstances().GetCompound("Blood");
    m_muscleIntracellular = m_data.GetCompartments().GetLiquidCompartment(pulse::ExtravascularCompartment::MuscleIntracellular);
    m_aortaVascular = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Aorta);
    m_venaCavaVascular = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::VenaCava);
    m_fatTissue = m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::Fat);
    m_liverVascular = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Liver);
    m_liverTissue = m_data.GetCompartments().GetTissueCompartment(pulse::TissueCompartment::Liver);
    m_IVToVenaCava = m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::IVToVenaCava1);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Determines if the engine is stable.
  ///
  /// \details
  /// When the engine is stable, the CDM makes this call to update the member variable.
  //--------------------------------------------------------------------------------------------------
  void DrugModel::AtSteadyState()
  {

  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Substance Preprocess function
  ///
  /// \details
  /// The AdminsterSubstances, AdministerIVSubstances, and Hemorrhage actions are processed in this step. The masses
  /// of the substances associated with these actions are updated as the actions are processed.
  //--------------------------------------------------------------------------------------------------
  void DrugModel::PreProcess()
  {
    AdministerSubstanceBolus();
    AdministerSubstanceInfusion();
    AdministerSubstanceCompoundInfusion();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Substances process function
  ///
  /// \details
  /// For any drugs circulating in the system the renal and hepatic clearance factors and metabolic factors
  /// are assigned to the appropriate compartments.
  /// The drug effects categories and system level drug effects are calculated and the values are set on the common data
  /// model for use in other systems. Drug effects are only calculated if the PD is enabled.
  //--------------------------------------------------------------------------------------------------
  void DrugModel::Process(bool /*solve_and_transport*/)
  {
    CalculatePartitionCoefficients();

    CalculateSubstanceClearance();

    CalculatePlasmaSubstanceConcentration();

    if (m_data.GetConfiguration().IsPDEnabled())
      CalculateDrugEffects();
    ComputeExposedModelParameters();
  }
  void DrugModel::PostProcess(bool /*solve_and_transport*/)
  {

  }

  void DrugModel::ComputeExposedModelParameters()
  {

  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Increments the mass of a substance to represent drug injection
  ///
  /// \details
  /// The mass of a substance is increased based on the administration of a substance. For a bolus administration,
  /// the user specifies a dose (amount), a concentration, and a substance. The calculated mass is then incrementally
  /// added to the vena cava over two seconds. 
  //--------------------------------------------------------------------------------------------------
  void DrugModel::AdministerSubstanceBolus()
  {
    //Need to loop over Bolus Dose Objects
    const std::vector<SESubstanceBolus*>& boluses = m_data.GetActions().GetPatientActions().GetSubstanceBoluses();
    if (boluses.empty())
      return;

    std::vector<const SESubstance*> completedBolus;
    const SESubstance* sub;
    SELiquidSubstanceQuantity* subQ;
    double dose_mL;
    double concentration_ugPermL;
    double massIncrement_ug = 0;
    double administrationTime_s;

    for (auto bolus : boluses)
    {
      if (!bolus->IsActive())
        continue;

      dose_mL = bolus->GetDose(VolumeUnit::mL);
      if (bolus->GetTotalInfusedDose().GetValue(VolumeUnit::mL) >= dose_mL)
      {
        // Finished, remove it
        completedBolus.push_back(&bolus->GetSubstance());
        continue;
      }

      sub = &bolus->GetSubstance();
      switch (bolus->GetAdminRoute())
      {
      case eSubstanceAdministration_Route::Intraarterial:
        subQ = m_aortaVascular->GetSubstanceQuantity(*sub);
        break;
      case eSubstanceAdministration_Route::Intravenous:
        subQ = m_venaCavaVascular->GetSubstanceQuantity(*sub);
        break;
      case eSubstanceAdministration_Route::Intramuscular:
        subQ = m_muscleIntracellular->GetSubstanceQuantity(*sub);
        break;
      default:
        Error("Unavailable Bolus Administration Route for substance " + sub->GetName());
        completedBolus.push_back(sub);// Remove it
        continue;
      }

      if (!bolus->HasAdminDuration())
      {
        bolus->GetAdminDuration().SetValue(2, TimeUnit::s);
        Info("Defaulting bolus administration duration of " + sub->GetName() + " to 2s");
      }
      administrationTime_s = bolus->GetAdminDuration(TimeUnit::s);
      concentration_ugPermL = bolus->GetConcentration(MassPerVolumeUnit::ug_Per_mL);
      massIncrement_ug = dose_mL * concentration_ugPermL * m_data.GetTimeStep_s() / administrationTime_s;
      subQ->GetMass().Increment(massIncrement_ug, MassUnit::ug);
      subQ->Balance(BalanceLiquidBy::Mass);
      bolus->GetTotalInfusedDose().Increment(massIncrement_ug / concentration_ugPermL, VolumeUnit::mL);
      /// \todo Add fluid amount to fluid system
    }
    // Remove any bolus that are complete
    for (const SESubstance* s : completedBolus)
      m_data.GetActions().GetPatientActions().RemoveSubstanceBolus(*s);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Increments the mass of a substance to represent drug infusion
  ///
  /// \details
  /// The mass of a substance is increased based on the administration of a substance. 
  /// For an infusion, the user specifies a concentration and a rate. The calculated mass
  /// is then added to the vena cava every time step until the infusion is stopped.
  //--------------------------------------------------------------------------------------------------
  void DrugModel::AdministerSubstanceInfusion()
  {
    const std::vector<SESubstanceInfusion*>& infusions = m_data.GetActions().GetPatientActions().GetSubstanceInfusions();
    if (infusions.empty())
      return;

    SELiquidSubstanceQuantity* subQ;
    double concentration_ug_Per_mL;
    double rate_mL_Per_s;
    double massIncrement_ug = 0;
    std::vector<const SESubstance*> finished;

    for (auto infusion : infusions)
    {
      if (!infusion->IsActive())
        continue;

      if (!infusion->HasVolume())
        infusion->GetVolume().SetValue(1000, VolumeUnit::mL);

      concentration_ug_Per_mL = infusion->GetConcentration().GetValue(MassPerVolumeUnit::ug_Per_mL);
      rate_mL_Per_s = infusion->GetRate().GetValue(VolumePerTimeUnit::mL_Per_s);
      infusion->GetVolume().Increment(-rate_mL_Per_s * m_data.GetTimeStep_s(), VolumeUnit::mL);
      if (infusion->GetVolume().IsZero() || infusion->GetVolume().IsNegative())
      { /// \todo correct the mass based on what we have left in the bag
        finished.push_back(&infusion->GetSubstance());
        continue;
      }

      massIncrement_ug = rate_mL_Per_s * concentration_ug_Per_mL * m_data.GetTimeStep_s();
      subQ = m_venaCavaVascular->GetSubstanceQuantity(infusion->GetSubstance());
      subQ->GetMass().Increment(massIncrement_ug, MassUnit::ug);
      //todo: Enforce limits and remove the fatal error
      if (massIncrement_ug < 0)
      {
        std::stringstream AdministeredTitrationDoseError;
        AdministeredTitrationDoseError << "Cannot specify a dose of less than 0. Current dose is: " << massIncrement_ug << " ug";
        Fatal(AdministeredTitrationDoseError);
        return;
      }

      /// \todo Need to add fluid amount to fluid system
    }
    for (const SESubstance* sub : finished)
    {
      Info("Completed infusion of " + sub->GetName());
      m_data.GetActions().GetPatientActions().RemoveSubstanceInfusion(*sub);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Increases the substance masses for compounds
  ///
  /// \details
  /// The masses of all substances associated with the compound substance are increased.
  /// The mass of each substance is incremented in the vena cava based on the rate specified by the user 
  /// and a concentration specified in the compound substance file.
  //--------------------------------------------------------------------------------------------------
  void DrugModel::AdministerSubstanceCompoundInfusion()
  {
    const std::vector<SESubstanceCompoundInfusion*>& infusions = m_data.GetActions().GetPatientActions().GetSubstanceCompoundInfusions();
    if (infusions.empty())
      return;

    const SESubstanceCompound* compound;
    SELiquidSubstanceQuantity* subQ;
    double rate_mL_Per_s = 0;
    double totalRate_mL_Per_s = 0;
    double massIncrement_ug = 0;

    std::vector<const SESubstanceCompound*> emptyBags;

    for (auto infusion : infusions)
    {
      compound = &infusion->GetSubstanceCompound();

      if (!infusion->IsActive())
        continue;

      rate_mL_Per_s = infusion->GetRate().GetValue(VolumePerTimeUnit::mL_Per_s);
      totalRate_mL_Per_s += rate_mL_Per_s;
      //todo: Enforce limits and remove the fatal error
      if (rate_mL_Per_s > 285) // from http://emedsa.org.au/EDHandbook/resuscitation/IVCannula.htm... 1000mL/3.5 min ~= 285 mL/min
      {
        m_ss << "Cannot specify an Infusion rate greater than 285 mL/min. Current administration rate is: " << infusion->GetRate();
        Fatal(m_ss);
        return;
      }

      infusion->GetBagVolume().Increment(-rate_mL_Per_s * m_data.GetTimeStep_s(), VolumeUnit::mL);
      if (infusion->GetBagVolume().IsZero() || infusion->GetBagVolume().IsNegative())
      { /// \todo correct the mass based on what we have left in the bag
        emptyBags.push_back(compound);
        continue;
      }

      for (const SESubstanceConcentration* component : compound->GetComponents())
      {
        subQ = m_venaCavaVascular->GetSubstanceQuantity(component->GetSubstance());
        massIncrement_ug = rate_mL_Per_s * component->GetConcentration(MassPerVolumeUnit::ug_Per_mL) * m_data.GetTimeStep_s();
        subQ->GetMass().Increment(massIncrement_ug, MassUnit::ug);
        subQ->Balance(BalanceLiquidBy::Mass);
      }
    }

    for (const SESubstanceCompound* c : emptyBags)
    {
      Info("Completed infusion of " + c->GetName());
      m_data.GetActions().GetPatientActions().RemoveSubstanceCompoundInfusion(*c);
    }

    m_IVToVenaCava->GetNextFlowSource().SetValue(totalRate_mL_Per_s, VolumePerTimeUnit::mL_Per_s);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculate partition coefficients for the PK Model
  ///
  /// \param  vSubstances
  /// Coefficients are calculated for the substances in the vector with IonicState. 
  /// 
  /// \details
  /// The partition coefficients are calculated for each substance in the input vector (active substances). 
  /// To ensure the diffusion is modeled with PK for the substance, a check is performed for Ionic State. If 
  /// the property is present the coefficients are calculated. They are calculated for all tissue compartments.
  /// coefficients are set on the substance compartment effects for use during the diffusion calculation.
  //--------------------------------------------------------------------------------------------------
  void DrugModel::CalculatePartitionCoefficients()
  {
    SEBloodChemistrySystem& BloodChemistry = m_data.GetBloodChemistry();
    double IntracellularPH = m_data.GetTissue().GetIntracellularFluidPH().GetValue();
    double PlasmaPH = BloodChemistry.GetBloodPH().GetValue();
    double NeutralLipidInPlasmaVolumeFraction = BloodChemistry.GetVolumeFractionNeutralLipidInPlasma().GetValue();
    double NeutralPhosphoLipidInPlasmaVolumeFraction = BloodChemistry.GetVolumeFractionNeutralPhospholipidInPlasma().GetValue();
    double IntracellularPHEffects = 0;
    double PlasmaPHEffects = 0;
    double PHEffectPower = 0;
    double EquationPartA = 0;
    double EquationPartB = 0;
    double EquationPartC = 0;
    double PartitionCoefficient = 0;
    double TissueToPlasmaProteinRatio = 0;

    //Loop over tissue nodes
    for (SETissueCompartment* tissue : m_data.GetCompartments().GetTissueLeafCompartments())
    {
      if (!tissue->HasMatrixVolume())
      {
        continue;
      }
      SELiquidCompartment& ExtracellularFluid = tissue->GetExtracellular();
      SELiquidCompartment& IntracellularFluid = tissue->GetIntracellular();

      //Loop over substances
      for (SESubstance* sub : m_data.GetCompartments().GetLiquidCompartmentSubstances())
      {
        if (!sub->HasPK())
          continue;
        if (!sub->GetPK().HasPhysicochemicals())
          continue;

        SESubstancePhysicochemicals& pk = sub->GetPK().GetPhysicochemicals();
        eSubstance_IonicState IonicState = pk.GetIonicState();
        double AcidDissociationConstant = pk.GetAcidDissociationConstant().GetValue();
        double P = exp(log(10) * pk.GetLogP().GetValue()); //Getting P from logP value
        if (tissue == m_fatTissue)
        {
          P = 1.115 * pk.GetLogP().GetValue() - 1.35;
          P = exp(log(10) * P);
        }
        if (pk.GetBindingProtein() == eSubstance_BindingProtein::AAG)
        {
          TissueToPlasmaProteinRatio = tissue->GetTissueToPlasmaAlphaAcidGlycoproteinRatio().GetValue();
        }
        else if (pk.GetBindingProtein() == eSubstance_BindingProtein::Albumin)
        {
          TissueToPlasmaProteinRatio = tissue->GetTissueToPlasmaAlbuminRatio().GetValue();
        }
        else if (pk.GetBindingProtein() == eSubstance_BindingProtein::Lipoprotein)
        {
          TissueToPlasmaProteinRatio = tissue->GetTissueToPlasmaLipoproteinRatio().GetValue();
        }
        else
        {
          std::stringstream ss;
          ss << "The binding protein is not supported. Unable to compute partition coefficient for: ";
          ss << sub->GetName();
          ss << " in compartment: ";
          ss << tissue->GetName();
          Fatal(ss);
        }
        //Based on the ionic state, the partition coefficient equation and/or pH effect equations are varied.
        if (IonicState == eSubstance_IonicState::Base)
        {
          IntracellularPHEffects = pow(10.0, (AcidDissociationConstant - IntracellularPH));
          PHEffectPower = PlasmaPH - AcidDissociationConstant;
          PlasmaPHEffects = 1.0 + pow(10.0, PHEffectPower);
          EquationPartA = 1.0 + IntracellularPHEffects * IntracellularFluid.GetWaterVolumeFraction().GetValue() / PlasmaPHEffects;
          /// \todo How to support oral absorption - should I check if oral administration then use Oral absorption rate otherwise assume 1?
          EquationPartB = tissue->GetAcidicPhospohlipidConcentration().GetValue(MassPerMassUnit::mg_Per_g) * IntracellularPHEffects / PlasmaPHEffects;
          EquationPartC = P * tissue->GetNeutralLipidsVolumeFraction().GetValue() + (0.3 * P + 0.7) * tissue->GetNeutralPhospholipidsVolumeFraction().GetValue() / PlasmaPHEffects;
        }
        else
        {
          if (IonicState == eSubstance_IonicState::Acid)
          {
            PHEffectPower = IntracellularPH - AcidDissociationConstant;
            IntracellularPHEffects = 1.0 + pow(10.0, PHEffectPower);
            PHEffectPower = PlasmaPH - AcidDissociationConstant;
            PlasmaPHEffects = 1.0 + pow(10.0, PHEffectPower);
          }
          else if (IonicState == eSubstance_IonicState::WeakBase)
          {
            PHEffectPower = AcidDissociationConstant - IntracellularPH;
            IntracellularPHEffects = 1.0 + pow(10.0, PHEffectPower);
            PHEffectPower = AcidDissociationConstant - PlasmaPH;
            PlasmaPHEffects = 1.0 + pow(10.0, PHEffectPower);
          }
          else
          {
            IntracellularPHEffects = 1.0;
            PlasmaPHEffects = 1.0;
          }
          EquationPartA = IntracellularPHEffects * IntracellularFluid.GetWaterVolumeFraction().GetValue() / PlasmaPHEffects;
          EquationPartB = (P * tissue->GetNeutralLipidsVolumeFraction().GetValue() + (0.3 * P + 0.7) * tissue->GetNeutralPhospholipidsVolumeFraction().GetValue())
            / PlasmaPHEffects;
          EquationPartC = ((1 / pk.GetFractionUnboundInPlasma().GetValue()) - 1.0 - ((P * NeutralLipidInPlasmaVolumeFraction + (0.3 * P + 0.7) * NeutralPhosphoLipidInPlasmaVolumeFraction)
            / PlasmaPHEffects)) * TissueToPlasmaProteinRatio;

        }
        //Calculate the partition coefficient and set it on the substance compartment effects
        PartitionCoefficient = EquationPartA + ExtracellularFluid.GetWaterVolumeFraction().GetValue() + EquationPartB + EquationPartC;
        PartitionCoefficient = PartitionCoefficient * pk.GetFractionUnboundInPlasma().GetValue() / pk.GetBloodPlasmaRatio().GetValue();
        sub->GetPK().GetTissueKinetics(tissue->GetName()).GetPartitionCoefficient().SetValue(PartitionCoefficient);
      }

    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates the drug effects on other system parameters
  ///
  /// \details
  /// If the substance is a drug with an EC50 value, the effects on heart rate, blood pressure, respiration rate,
  /// tidal volume, neuromuscular block level, sedation level, bronchodilation level, and pupillary state are 
  /// calculated using the current plasma concentration, the EC50, and the maximum drug response. 
  //--------------------------------------------------------------------------------------------------
  void DrugModel::CalculateDrugEffects()
  {
    double deltaHeartRate_Per_min = 0;
    double deltaDiastolicBP_mmHg = 0;
    double deltaSystolicBP_mmHg = 0;
    double deltaRespirationRate_Per_min = 0;
    double deltaTidalVolume_mL = 0;
    double neuromuscularBlockLevel = 0;
    double sedationLevel = 0;
    double bronchodilationLevel = 0;
    double plasmaConcentration_ug_Per_mL = 0;
    double concentrationEffects_unitless = 0;
    double deltaTubularPermeability = 0.0;
    double pupilSizeResponseLevel = 0;
    double pupilReactivityResponseLevel = 0;
    double shapeParameter = 1.;
    SEPatient& patient = m_data.GetCurrentPatient();
    double HRBaseline_per_min = patient.GetHeartRateBaseline(FrequencyUnit::Per_min);

    //Loop over substances
    for (SESubstance* sub : m_data.GetCompartments().GetLiquidCompartmentSubstances())
    {
      if (!sub->HasPD())
        continue;

      const SESubstancePharmacodynamics& pd = sub->GetPD();
      plasmaConcentration_ug_Per_mL = sub->GetPlasmaConcentration(MassPerVolumeUnit::ug_Per_mL);
      shapeParameter = pd.GetEMaxShapeParameter();
      if (shapeParameter == 1) // Avoiding using pow if we don't have to. I don't know if this is good practice or not, but seems legit.
        concentrationEffects_unitless = plasmaConcentration_ug_Per_mL / (pd.GetEC50(MassPerVolumeUnit::ug_Per_mL) + plasmaConcentration_ug_Per_mL);
      else
        concentrationEffects_unitless = pow(plasmaConcentration_ug_Per_mL, shapeParameter) / (pow(pd.GetEC50(MassPerVolumeUnit::ug_Per_mL), shapeParameter) + pow(plasmaConcentration_ug_Per_mL, shapeParameter));
      /// \todo The drug effect is being applied to the baseline, so if the baseline changes the delta heart rate changes.
      // This would be a problem for something like a continuous infusion of a drug or an environmental drug
      // where we need to establish a new homeostatic point. Once the patient stabilizes with the drug effect included, a new baseline is
      // set, and suddenly the drug effect is being computed using the new baseline. We may need to add another layer of 
      // stabilization and restrict drugs to post-feedback stabilization. Alternatively, we could base the drug effect on a baseline
      // concentration which is normally zero but which gets set to a new baseline concentration at the end of feedback (see chemoreceptor
      // and the blood gas setpoint reset for example).
      deltaHeartRate_Per_min += HRBaseline_per_min * pd.GetHeartRateModifier() * concentrationEffects_unitless;

      deltaDiastolicBP_mmHg += patient.GetDiastolicArterialPressureBaseline(PressureUnit::mmHg) * pd.GetDiastolicPressureModifier() * concentrationEffects_unitless;

      deltaSystolicBP_mmHg += patient.GetSystolicArterialPressureBaseline(PressureUnit::mmHg) * pd.GetSystolicPressureModifier() * concentrationEffects_unitless;

      sedationLevel += pd.GetSedation() * concentrationEffects_unitless;

      deltaTubularPermeability += (pd.GetTubularPermeabilityModifier()) * concentrationEffects_unitless;

      if (sedationLevel > 0.999)//0.15)
      {
        deltaRespirationRate_Per_min += patient.GetRespirationRateBaseline(FrequencyUnit::Per_min) * pd.GetRespirationRateModifier();
        deltaTidalVolume_mL += patient.GetTidalVolumeBaseline(VolumeUnit::mL) * pd.GetTidalVolumeModifier();
      }
      else
      {
        deltaRespirationRate_Per_min += patient.GetRespirationRateBaseline(FrequencyUnit::Per_min) * pd.GetRespirationRateModifier() * concentrationEffects_unitless;
        deltaTidalVolume_mL += patient.GetTidalVolumeBaseline(VolumeUnit::mL) * pd.GetTidalVolumeModifier() * concentrationEffects_unitless;
      }

      neuromuscularBlockLevel += pd.GetNeuromuscularBlock() * concentrationEffects_unitless;
      bronchodilationLevel += pd.GetBronchodilation() * concentrationEffects_unitless;

      const SEPupillaryResponse* pr = pd.GetPupillaryResponse();
      pupilSizeResponseLevel += pr->GetSizeModifier() * concentrationEffects_unitless;
      pupilReactivityResponseLevel += pr->GetReactivityModifier() * concentrationEffects_unitless;
    }

    //Translate Diastolic and Systolic Pressure to pulse pressure and mean pressure
    double deltaMeanPressure_mmHg = (2 * deltaDiastolicBP_mmHg + deltaSystolicBP_mmHg) / 3;

    double deltaPulsePressure_mmHg = deltaSystolicBP_mmHg - deltaDiastolicBP_mmHg;

    //Bound things that are fractions
    sedationLevel = LIMIT(sedationLevel, 0.0, 1.0);
    bronchodilationLevel = LIMIT(bronchodilationLevel, -1.0, 1.0);
    neuromuscularBlockLevel = LIMIT(neuromuscularBlockLevel, 0.0, 1.0);
    deltaTubularPermeability = LIMIT(deltaTubularPermeability, -1.0, 1.0);

    //Set values on the CDM System Values
    GetHeartRateChange().SetValue(deltaHeartRate_Per_min, FrequencyUnit::Per_min);
    GetMeanBloodPressureChange().SetValue(deltaMeanPressure_mmHg, PressureUnit::mmHg);
    GetPulsePressureChange().SetValue(deltaPulsePressure_mmHg, PressureUnit::mmHg);
    GetRespirationRateChange().SetValue(deltaRespirationRate_Per_min, FrequencyUnit::Per_min);
    GetTidalVolumeChange().SetValue(deltaTidalVolume_mL, VolumeUnit::mL);
    GetNeuromuscularBlockLevel().SetValue(neuromuscularBlockLevel);
    GetSedationLevel().SetValue(sedationLevel);
    GetBronchodilationLevel().SetValue(bronchodilationLevel);
    GetTubularPermeabilityChange().SetValue(deltaTubularPermeability);


    // Bound the pupil modifiers
    BLIM(pupilSizeResponseLevel, -1, 1);
    BLIM(pupilReactivityResponseLevel, -1, 1);
    GetPupillaryResponse().GetSizeModifier().SetValue(pupilSizeResponseLevel);
    GetPupillaryResponse().GetReactivityModifier().SetValue(pupilReactivityResponseLevel);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates the concentration of a substance in the plasma
  ///
  /// \details
  /// If the substance has PK properties, the concentration of the substance in the plasma is calculated. 
  /// PlasmaConcentration = TotalMassInTheBlood / PlasmaVolume.
  /// The plasma concentration is then set on the substance.
  /// The concentration computation is obviously erroneous. This is a known issue. See @ref pharmacokinetics
  //--------------------------------------------------------------------------------------------------
  void DrugModel::CalculatePlasmaSubstanceConcentration()
  {
    double PlasmaMass_ug = 0;
    double PlasmaVolume_mL = m_data.GetBloodChemistry().GetPlasmaVolume(VolumeUnit::mL);

    for (SESubstance* sub : m_data.GetCompartments().GetLiquidCompartmentSubstances())
    {
      PlasmaMass_ug = m_data.GetSubstances().GetSubstanceMass(*sub, m_data.GetCompartments().GetVascularLeafCompartments(), MassUnit::ug);
      sub->GetPlasmaConcentration().SetValue(PlasmaMass_ug / PlasmaVolume_mL, MassPerVolumeUnit::ug_Per_mL);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates the substance mass cleared
  ///
  /// \details
  /// If the substance has PK properties, the substance clearance is calculated. The renal, liver, and systemic 
  /// clearance terms are applied to each anatomy compartment per substance. The amount of mass is then removed from the
  /// tissue node each time step. The tissue node mass and concentration are then updated. Sets the amount of substance
  /// cleared as a compartment effect for each substance.
  //--------------------------------------------------------------------------------------------------
  void DrugModel::CalculateSubstanceClearance()
  {
    double PatientWeight_kg = SEScalar::Truncate(m_data.GetCurrentPatient().GetWeight(MassUnit::kg), 4);
    double HepaticClearance_mLPers = 0;
    double FractionUnboundInPlasma = 0;
    double IntrinsicClearance_mLPersPerkg = 0;
    double TotalVolumeCleared_mL = 0;
    double RenalVolumeCleared_mL = 0;
    double HepaticVolumeCleared_mL = 0;
    double OtherSystemicVolumeCleared_mL = 0;

    for (SESubstance* sub : m_data.GetCompartments().GetLiquidCompartmentSubstances())
    {
      if (!sub->HasClearance() || !sub->GetClearance().HasSystemic())
        continue;
      SESubstanceClearance& clearance = sub->GetClearance();

      //Renal Volume Cleared - Clearance happens through the renal system
      RenalVolumeCleared_mL = (clearance.GetRenalClearance().GetValue(VolumePerTimeMassUnit::mL_Per_s_kg) * PatientWeight_kg * m_data.GetTimeStep_s());

      //Intrinsic Clearance
      IntrinsicClearance_mLPersPerkg = clearance.GetIntrinsicClearance().GetValue(VolumePerTimeMassUnit::mL_Per_s_kg);
      FractionUnboundInPlasma = clearance.GetFractionUnboundInPlasma().GetValue();
      //Hepatic Clearance
      double LiverVascularFlow_mL_Per_s = m_liverVascular->GetInFlow().GetValue(VolumePerTimeUnit::mL_Per_s);
      HepaticClearance_mLPers = (LiverVascularFlow_mL_Per_s * FractionUnboundInPlasma * IntrinsicClearance_mLPersPerkg * PatientWeight_kg) / (LiverVascularFlow_mL_Per_s + (FractionUnboundInPlasma * IntrinsicClearance_mLPersPerkg * PatientWeight_kg));
      HepaticVolumeCleared_mL = HepaticClearance_mLPers * m_data.GetTimeStep_s();

      //Systemic Clearance
      TotalVolumeCleared_mL = clearance.GetSystemicClearance().GetValue(VolumePerTimeMassUnit::mL_Per_s_kg) * PatientWeight_kg * m_data.GetTimeStep_s();
      OtherSystemicVolumeCleared_mL = TotalVolumeCleared_mL - RenalVolumeCleared_mL - HepaticVolumeCleared_mL;
      // The following makes it so that you cannot have more than the total systemic clearance for the substance.
      LLIM(OtherSystemicVolumeCleared_mL, 0.);

      //Hepatic Clearance    
      m_data.GetSubstances().CalculateGenericClearance(HepaticVolumeCleared_mL, *m_liverTissue, *sub);

      //Systemic Clearance
      m_data.GetSubstances().CalculateGenericClearance(OtherSystemicVolumeCleared_mL, *m_venaCavaVascular, *sub);

      //Hepatic Excretion
      m_data.GetSubstances().CalculateGenericExcretion(LiverVascularFlow_mL_Per_s, *m_liverTissue, *sub, clearance.GetFractionExcretedInFeces().GetValue(), m_data.GetTimeStep_s());
    }
  }
END_NAMESPACE
