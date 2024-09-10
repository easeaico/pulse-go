/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "io/protobuf/PBConfiguration.h"
PUSH_PROTO_WARNINGS
#include "pulse/engine/bind/Configuration.pb.h"
POP_PROTO_WARNINGS

#include "cdm/io/protobuf/PBActions.h"
#include "cdm/io/protobuf/PBEngine.h"
#include "cdm/io/protobuf/PBEnvironment.h"
#include "cdm/io/protobuf/PBPatientNutrition.h"
#include "cdm/io/protobuf/PBProperties.h"
#include "cdm/io/protobuf/PBUtils.h"
#include "cdm/patient/SENutrition.h"
#include "cdm/engine/SEDynamicStabilization.h"
#include "cdm/engine/SETimedStabilization.h"
#include "cdm/system/environment/SEEnvironmentalConditions.h"
#include "cdm/properties/SEScalarMassPerTime.h"
#include "cdm/utils/FileUtils.h"

namespace pulse
{
  void PBConfiguration::Merge(const PulseConfiguration& src, PulseConfiguration& dst, SESubstanceManager& subMgr)
  {
    PULSE_BIND::ConfigurationData data;
    PBConfiguration::Serialize(src, data);
    PBConfiguration::Serialize(data, dst, subMgr, true);
  }

  void PBConfiguration::Load(const PULSE_BIND::ConfigurationData& src, PulseConfiguration& dst, SESubstanceManager& subMgr)
  {
    PBConfiguration::Serialize(src, dst, subMgr);
  }
  void PBConfiguration::Serialize(const PULSE_BIND::ConfigurationData& src, PulseConfiguration& dst, SESubstanceManager& subMgr, bool merge)
  {
    if (!merge)
      dst.Clear();

    if (src.has_timestep())
      PBProperty::Load(src.timestep(), dst.GetTimeStep());
    if (src.allowdynamictimestep() != CDM_BIND::eSwitch::NullSwitch)
      dst.AllowDynamicTimeStep((eSwitch)src.allowdynamictimestep());
    if (src.has_timedstabilization())
      PBEngine::Load(src.timedstabilization(), dst.GetTimedStabilization());
    else if (src.has_dynamicstabilization())
      PBEngine::Load(src.dynamicstabilization(), dst.GetDynamicStabilization());
    else if (!src.stabilizationfilename().empty())
    {
      if (!dst.GetTimedStabilization().SerializeFromFile(src.stabilizationfilename()))
        if (!dst.GetDynamicStabilization().SerializeFromFile(src.stabilizationfilename()))
        {
          dst.Error("Unable to load stabilization file");
          dst.RemoveStabilization();
        }
    }

    for (auto& [name, m] : src.modifiers())
      dst.GetModifiers()[name] = SEScalarPair(m.value(), m.unit());
    for (auto& [name, o] : src.overrides())
      dst.GetOverrides()[name] = SEScalarPair(o.value(), o.unit());

    // Blood Chemistry
    if (src.has_bloodchemistryconfiguration())
    {
      const PULSE_BIND::ConfigurationData_BloodChemistryConfigurationData& config = src.bloodchemistryconfiguration();
      if (config.has_meancorpuscularvolume())
        PBProperty::Load(config.meancorpuscularvolume(), dst.GetMeanCorpuscularVolume());
      if (config.has_meancorpuscularhemoglobin())
        PBProperty::Load(config.meancorpuscularhemoglobin(), dst.GetMeanCorpuscularHemoglobin());
      if (config.has_standarddiffusiondistance())
        PBProperty::Load(config.standarddiffusiondistance(), dst.GetStandardDiffusionDistance());
      if (config.has_standardoxygendiffusioncoefficient())
        PBProperty::Load(config.standardoxygendiffusioncoefficient(), dst.GetStandardOxygenDiffusionCoefficient());
    }

    // Cardiovascular
    if (src.has_cardiovascularconfiguration())
    {
      const PULSE_BIND::ConfigurationData_CardiovascularConfigurationData& config = src.cardiovascularconfiguration();
      if (config.has_leftheartelastancemaximum())
        PBProperty::Load(config.leftheartelastancemaximum(), dst.GetLeftHeartElastanceMaximum());
      if (config.has_leftheartelastanceminimum())
        PBProperty::Load(config.leftheartelastanceminimum(), dst.GetLeftHeartElastanceMinimum());
      if (config.has_minimumbloodvolumefraction())
        PBProperty::Load(config.minimumbloodvolumefraction(), dst.GetMinimumBloodVolumeFraction());
      if (config.has_rightheartelastancemaximum())
        PBProperty::Load(config.rightheartelastancemaximum(), dst.GetRightHeartElastanceMaximum());
      if (config.has_rightheartelastanceminimum())
        PBProperty::Load(config.rightheartelastanceminimum(), dst.GetRightHeartElastanceMinimum());
      if (config.has_standardpulmonarycapillarycoverage())
        PBProperty::Load(config.standardpulmonarycapillarycoverage(), dst.GetStandardPulmonaryCapillaryCoverage());
      if (config.useexpandedvasculature() != CDM_BIND::eSwitch::NullSwitch)
        dst.UseExpandedVasculature((eSwitch)config.useexpandedvasculature());
      if (config.tunecardiovascularcircuit() != CDM_BIND::eSwitch::NullSwitch)
        dst.TuneCardiovascularCircuit((eSwitch)config.tunecardiovascularcircuit());
      dst.CardiovascularTuningFile(config.cardiovasculartuningfile());
    }

    // Circuit
    if (src.has_circuitconfiguration())
    {
      const PULSE_BIND::ConfigurationData_CircuitConfigurationData& config = src.circuitconfiguration();
      if (config.has_cardiovascularopenresistance())
        PBProperty::Load(config.cardiovascularopenresistance(), dst.GetCardiovascularOpenResistance());
      if (config.has_defaultopenelectricresistance())
        PBProperty::Load(config.defaultopenelectricresistance(), dst.GetDefaultOpenElectricResistance());
      if (config.has_defaultopenflowresistance())
        PBProperty::Load(config.defaultopenflowresistance(), dst.GetDefaultOpenFlowResistance());
      if (config.has_defaultopenheatresistance())
        PBProperty::Load(config.defaultopenheatresistance(), dst.GetDefaultOpenHeatResistance());
      if (config.has_defaultclosedelectricresistance())
        PBProperty::Load(config.defaultclosedelectricresistance(), dst.GetDefaultClosedElectricResistance());
      if (config.has_defaultclosedflowresistance())
        PBProperty::Load(config.defaultclosedflowresistance(), dst.GetDefaultClosedFlowResistance());
      if (config.has_defaultclosedheatresistance())
        PBProperty::Load(config.defaultclosedheatresistance(), dst.GetDefaultClosedHeatResistance());
      if (config.has_machineclosedresistance())
        PBProperty::Load(config.machineclosedresistance(), dst.GetMachineClosedResistance());
      if (config.has_machineopenresistance())
        PBProperty::Load(config.machineopenresistance(), dst.GetMachineOpenResistance());
      if (config.has_respiratoryclosedresistance())
        PBProperty::Load(config.respiratoryclosedresistance(), dst.GetRespiratoryClosedResistance());
      if (config.has_respiratoryopenresistance())
        PBProperty::Load(config.respiratoryopenresistance(), dst.GetRespiratoryOpenResistance());
    }

    // Constants
    if (src.has_constantsconfiguration())
    {
      const PULSE_BIND::ConfigurationData_ConstantsConfigurationData& config = src.constantsconfiguration();
      if (config.has_oxygenmetabolicconstant())
        PBProperty::Load(config.oxygenmetabolicconstant(), dst.GetOxygenMetabolicConstant());
      if (config.has_stefanboltzmann())
        PBProperty::Load(config.stefanboltzmann(), dst.GetStefanBoltzmann());
      if (config.has_universalgasconstant())
        PBProperty::Load(config.universalgasconstant(), dst.GetUniversalGasConstant());
    }

    // Drugs
    if (src.has_drugsconfiguration())
    {
      const PULSE_BIND::ConfigurationData_DrugsConfigurationData& config = src.drugsconfiguration();
      if (config.pdmodel() != CDM_BIND::eSwitch::NullSwitch)
        dst.UsePDModel((eSwitch)config.pdmodel());
    }

    // Energy
    if (src.has_energyconfiguration())
    {
      const PULSE_BIND::ConfigurationData_EnergyConfigurationData& config = src.energyconfiguration();
      if (config.has_bodyspecificheat())
        PBProperty::Load(config.bodyspecificheat(), dst.GetBodySpecificHeat());
      if (config.has_carbondioxideproductionfromoxygenconsumptionconstant())
        PBProperty::Load(config.carbondioxideproductionfromoxygenconsumptionconstant(), dst.GetCarbonDioxideProductionFromOxygenConsumptionConstant());
      if (config.has_coretemperaturelow())
        PBProperty::Load(config.coretemperaturelow(), dst.GetCoreTemperatureLow());
      if (config.has_coretemperaturehigh())
        PBProperty::Load(config.coretemperaturehigh(), dst.GetCoreTemperatureHigh());
      if (config.has_deltacoretemperaturelow())
        PBProperty::Load(config.deltacoretemperaturelow(), dst.GetDeltaCoreTemperatureLow());
      if (config.has_energyperatp())
        PBProperty::Load(config.energyperatp(), dst.GetEnergyPerATP());
      if (config.has_sweatheattransfer())
        PBProperty::Load(config.sweatheattransfer(), dst.GetSweatHeatTransfer());
      if (config.has_vaporizationenergy())
        PBProperty::Load(config.vaporizationenergy(), dst.GetVaporizationEnergy());
      if (config.has_vaporspecificheat())
        PBProperty::Load(config.vaporspecificheat(), dst.GetVaporSpecificHeat());
    }

    // Environment
    if (src.has_environmentconfiguration())
    {
      const PULSE_BIND::ConfigurationData_EnvironmentConfigurationData& config = src.environmentconfiguration();
      if (config.has_airdensity())
        PBProperty::Load(config.airdensity(), dst.GetAirDensity());
      if (config.has_airspecificheat())
        PBProperty::Load(config.airspecificheat(), dst.GetAirSpecificHeat());
      if (config.has_molarmassofdryair())
        PBProperty::Load(config.molarmassofdryair(), dst.GetMolarMassOfDryAir());
      if (config.has_molarmassofwatervapor())
        PBProperty::Load(config.molarmassofwatervapor(), dst.GetMolarMassOfWaterVapor());
      if (!config.initialconditionsfile().empty())
      {
        if (!dst.GetInitialEnvironmentalConditions().SerializeFromFile(config.initialconditionsfile(), subMgr))
        {
          dst.Error("Unable to load InitialEnvironmentalConditions file");
        }
      }
      else if (config.has_initialconditions())
      {
        PBEnvironment::Load(config.initialconditions(), dst.GetInitialEnvironmentalConditions(), subMgr);
      }
      if (config.has_waterdensity())
        PBProperty::Load(config.waterdensity(), dst.GetWaterDensity());
    }

    // Gastrointestinal
    if (src.has_gastrointestinalconfiguration())
    {
      const PULSE_BIND::ConfigurationData_GastrointestinalConfigurationData& config = src.gastrointestinalconfiguration();
      if (config.has_calciumabsorptionfraction())
        PBProperty::Load(config.calciumabsorptionfraction(), dst.GetCalciumAbsorptionFraction());
      if (config.has_calciumdigestionrate())
        PBProperty::Load(config.calciumdigestionrate(), dst.GetCalciumDigestionRate());
      if (config.has_carbohydrateabsorptionfraction())
        PBProperty::Load(config.carbohydrateabsorptionfraction(), dst.GetCarbohydrateAbsorptionFraction());
      if (config.has_defaultcarbohydratedigestionrate())
        PBProperty::Load(config.defaultcarbohydratedigestionrate(), dst.GetDefaultCarbohydrateDigestionRate());
      if (config.has_defaultfatdigestionrate())
        PBProperty::Load(config.defaultfatdigestionrate(), dst.GetDefaultFatDigestionRate());
      if (config.has_defaultproteindigestionrate())
        PBProperty::Load(config.defaultproteindigestionrate(), dst.GetDefaultProteinDigestionRate());
      if (!config.initialstomachcontentsfile().empty())
      {
        if (!dst.GetDefaultStomachContents().SerializeFromFile(config.initialstomachcontentsfile()))
        {
          dst.Error("Unable to load Standard Stomach Contents file");
        }
      }
      else if (config.has_initialstomachcontents())
      {
        PBPatientNutrition::Load(config.initialstomachcontents(), dst.GetDefaultStomachContents());
      }
      // Use default rate if they are not set
      if (dst.m_DefaultStomachContents != nullptr)
      {
        if (dst.m_DefaultStomachContents->HasCarbohydrate() && !dst.m_DefaultStomachContents->HasCarbohydrateDigestionRate())
          dst.m_DefaultStomachContents->GetCarbohydrateDigestionRate().Set(dst.GetDefaultCarbohydrateDigestionRate());
        if (dst.m_DefaultStomachContents->HasFat() && !dst.m_DefaultStomachContents->HasFatDigestionRate())
          dst.m_DefaultStomachContents->GetFatDigestionRate().Set(dst.GetDefaultFatDigestionRate());
        if (dst.m_DefaultStomachContents->HasProtein() && !dst.m_DefaultStomachContents->HasProteinDigestionRate())
          dst.m_DefaultStomachContents->GetProteinDigestionRate().Set(dst.GetDefaultProteinDigestionRate());
      }
      if (config.has_fatabsorptionfraction())
        PBProperty::Load(config.fatabsorptionfraction(), dst.GetFatAbsorptionFraction());
      if (config.has_proteintoureafraction())
        PBProperty::Load(config.proteintoureafraction(), dst.GetProteinToUreaFraction());
      if (config.has_waterdigestionrate())
        PBProperty::Load(config.waterdigestionrate(), dst.GetWaterDigestionRate());
    }

    // Nervous
    if (src.has_nervousconfiguration())
    {
      const PULSE_BIND::ConfigurationData_NervousConfigurationData& config = src.nervousconfiguration();
      if (config.enablecerebrospinalfluid() != CDM_BIND::eSwitch::NullSwitch)
        dst.EnableCerebrospinalFluid((eSwitch)config.enablecerebrospinalfluid());
      if (config.baroreceptorfeedback() != CDM_BIND::eSwitch::NullSwitch)
        dst.SetBaroreceptorFeedback((eSwitch)config.baroreceptorfeedback());
      if (config.chemoreceptorfeedback() != CDM_BIND::eSwitch::NullSwitch)
        dst.SetChemoreceptorFeedback((eSwitch)config.chemoreceptorfeedback());
      if (config.has_heartelastancedistributedtimedelay())
        PBProperty::Load(config.heartelastancedistributedtimedelay(), dst.GetHeartElastanceDistributedTimeDelay());
      if (config.has_heartratedistributedtimedelay())
        PBProperty::Load(config.heartratedistributedtimedelay(), dst.GetHeartRateDistributedTimeDelay());
      if (config.has_normalizedheartrateintercept())
        PBProperty::Load(config.normalizedheartrateintercept(), dst.GetNormalizedHeartRateIntercept());
      if (config.has_normalizedheartratesympatheticslope())
        PBProperty::Load(config.normalizedheartratesympatheticslope(), dst.GetNormalizedHeartRateSympatheticSlope());
      if (config.has_normalizedheartrateparasympatheticslope())
        PBProperty::Load(config.normalizedheartrateparasympatheticslope(), dst.GetNormalizedHeartRateParasympatheticSlope());
      if (config.has_normalizedheartelastanceintercept())
        PBProperty::Load(config.normalizedheartelastanceintercept(), dst.GetNormalizedHeartElastanceIntercept());
      if (config.has_normalizedheartelastancesympatheticslope())
        PBProperty::Load(config.normalizedheartelastancesympatheticslope(), dst.GetNormalizedHeartElastanceSympatheticSlope());
      if (config.has_normalizedresistanceintercept())
        PBProperty::Load(config.normalizedresistanceintercept(), dst.GetNormalizedResistanceIntercept());
      if (config.has_normalizedresistancesympatheticslope())
        PBProperty::Load(config.normalizedresistancesympatheticslope(), dst.GetNormalizedResistanceSympatheticSlope());
      if (config.has_normalizedcomplianceintercept())
        PBProperty::Load(config.normalizedcomplianceintercept(), dst.GetNormalizedComplianceIntercept());
      if (config.has_normalizedcomplianceparasympatheticslope())
        PBProperty::Load(config.normalizedcomplianceparasympatheticslope(), dst.GetNormalizedComplianceParasympatheticSlope());
      if (config.has_responseslope())
        PBProperty::Load(config.responseslope(), dst.GetResponseSlope());
      if (config.has_pupildiameterbaseline())
        PBProperty::Load(config.pupildiameterbaseline(), dst.GetPupilDiameterBaseline());
      if (config.has_systemicresistancedistributedtimedelay())
        PBProperty::Load(config.systemicresistancedistributedtimedelay(), dst.GetSystemicResistanceDistributedTimeDelay());
      if (config.has_venouscompliancedistributedtimedelay())
        PBProperty::Load(config.venouscompliancedistributedtimedelay(), dst.GetVenousComplianceDistributedTimeDelay());
    }

    // Renal
    if (src.has_renalconfiguration())
    {
      const PULSE_BIND::ConfigurationData_RenalConfigurationData& config = src.renalconfiguration();

      if (config.enablerenal() != CDM_BIND::eSwitch::NullSwitch)
        dst.EnableRenal((eSwitch)config.enablerenal());

      if (config.has_plasmasodiumconcentrationsetpoint())
        PBProperty::Load(config.plasmasodiumconcentrationsetpoint(), dst.GetPlasmaSodiumConcentrationSetPoint());
      if (config.has_peritubularpotassiumconcentrationsetpoint())
        PBProperty::Load(config.peritubularpotassiumconcentrationsetpoint(), dst.GetPeritubularPotassiumConcentrationSetPoint());

      if (config.has_leftglomerularfluidpermeabilitybaseline())
        PBProperty::Load(config.leftglomerularfluidpermeabilitybaseline(), dst.GetLeftGlomerularFluidPermeabilityBaseline());
      if (config.has_leftglomerularfilteringsurfaceareabaseline())
        PBProperty::Load(config.leftglomerularfilteringsurfaceareabaseline(), dst.GetLeftGlomerularFilteringSurfaceAreaBaseline());
      if (config.has_lefttubularreabsorptionfluidpermeabilitybaseline())
        PBProperty::Load(config.lefttubularreabsorptionfluidpermeabilitybaseline(), dst.GetLeftTubularReabsorptionFluidPermeabilityBaseline());
      if (config.has_lefttubularreabsorptionfilteringsurfaceareabaseline())
        PBProperty::Load(config.lefttubularreabsorptionfilteringsurfaceareabaseline(), dst.GetLeftTubularReabsorptionFilteringSurfaceAreaBaseline());

      if (config.has_maximumafferentresistance())
        PBProperty::Load(config.maximumafferentresistance(), dst.GetMaximumAfferentResistance());
      if (config.has_minimumafferentresistance())
        PBProperty::Load(config.minimumafferentresistance(), dst.GetMinimumAfferentResistance());

      if (config.has_rightglomerularfluidpermeabilitybaseline())
        PBProperty::Load(config.rightglomerularfluidpermeabilitybaseline(), dst.GetRightGlomerularFluidPermeabilityBaseline());
      if (config.has_rightglomerularfilteringsurfaceareabaseline())
        PBProperty::Load(config.rightglomerularfilteringsurfaceareabaseline(), dst.GetRightGlomerularFilteringSurfaceAreaBaseline());
      if (config.has_righttubularreabsorptionfluidpermeabilitybaseline())
        PBProperty::Load(config.righttubularreabsorptionfluidpermeabilitybaseline(), dst.GetRightTubularReabsorptionFluidPermeabilityBaseline());
      if (config.has_righttubularreabsorptionfilteringsurfaceareabaseline())
        PBProperty::Load(config.righttubularreabsorptionfilteringsurfaceareabaseline(), dst.GetRightTubularReabsorptionFilteringSurfaceAreaBaseline());

      if (config.has_targetsodiumdelivery())
        PBProperty::Load(config.targetsodiumdelivery(), dst.GetTargetSodiumDelivery());
    }

    // Respiratory
    if (src.has_respiratoryconfiguration())
    {
      const PULSE_BIND::ConfigurationData_RespiratoryConfigurationData& config = src.respiratoryconfiguration();
      if (config.has_centralcontrollerco2pressuresetpoint())
        PBProperty::Load(config.centralcontrollerco2pressuresetpoint(), dst.GetCentralControllerCO2PressureSetPoint());
      if (config.has_centralventilatorycontrollergain())
        PBProperty::Load(config.centralventilatorycontrollergain(), dst.GetCentralVentilatoryControllerGain());
      if (config.has_minimumallowableinspiratoryandexpiratoryperiod())
        PBProperty::Load(config.minimumallowableinspiratoryandexpiratoryperiod(), dst.GetMinimumAllowableInpiratoryAndExpiratoryPeriod());
      if (config.has_minimumallowabletidalvolume())
        PBProperty::Load(config.minimumallowabletidalvolume(), dst.GetMinimumAllowableTidalVolume());
      if (config.has_peripheralcontrollerco2pressuresetpoint())
        PBProperty::Load(config.peripheralcontrollerco2pressuresetpoint(), dst.GetPeripheralControllerCO2PressureSetPoint());
      if (config.has_peripheralventilatorycontrollergain())
        PBProperty::Load(config.peripheralventilatorycontrollergain(), dst.GetPeripheralVentilatoryControllerGain());
      if (config.has_pulmonaryventilationratemaximum())
        PBProperty::Load(config.pulmonaryventilationratemaximum(), dst.GetPulmonaryVentilationRateMaximum());
      if (config.has_ventilationtidalvolumeintercept())
        PBProperty::Load(config.ventilationtidalvolumeintercept(), dst.GetVentilationTidalVolumeIntercept());
      if (config.has_ventilatoryocclusionpressure())
        PBProperty::Load(config.ventilatoryocclusionpressure(), dst.GetVentilatoryOcclusionPressure());
      if (config.useexpandedrespiratory() != CDM_BIND::eSwitch::NullSwitch)
        dst.UseExpandedRespiratory((eSwitch)config.useexpandedrespiratory());
    }

    // Tissue
    if (src.has_tissueconfiguration())
    {
      const PULSE_BIND::ConfigurationData_TissueConfigurationData& config = src.tissueconfiguration();
      if (config.enabletissue() != CDM_BIND::eSwitch::NullSwitch)
        dst.EnableTissue((eSwitch)config.enabletissue());
    }
  }

  PULSE_BIND::ConfigurationData* PBConfiguration::Unload(const PulseConfiguration& src)
  {
    PULSE_BIND::ConfigurationData* dst = new PULSE_BIND::ConfigurationData();
    PBConfiguration::Serialize(src, *dst);
    return dst;
  }
  void PBConfiguration::Serialize(const PulseConfiguration& src, PULSE_BIND::ConfigurationData& dst)
  {
    if (src.HasTimedStabilization())
      dst.set_allocated_timedstabilization(PBEngine::Unload(*src.m_TimedStabilization));
    else if (src.HasDynamicStabilization())
      dst.set_allocated_dynamicstabilization(PBEngine::Unload(*src.m_DynamicStabilization));
    if (src.HasTimeStep())
      dst.set_allocated_timestep(PBProperty::Unload(*src.m_TimeStep));
    dst.set_allowdynamictimestep((CDM_BIND::eSwitch)src.m_AllowDynamicTimeStep);

    for (auto const& [name, m] : src.GetModifiers())
    {
      CDM_BIND::ScalarData& scalar =
        (*dst.mutable_modifiers())[name];
      scalar.set_value(m.value);
      scalar.set_unit(m.unit);
    }
    for (auto const& [name, o] : src.GetOverrides())
    {
      CDM_BIND::ScalarData& scalar =
        (*dst.mutable_overrides())[name];
      scalar.set_value(o.value);
      scalar.set_unit(o.unit);
    }

    // Blood Chemistry
    PULSE_BIND::ConfigurationData_BloodChemistryConfigurationData* bc = dst.mutable_bloodchemistryconfiguration();
    if (src.HasMeanCorpuscularHemoglobin())
      bc->set_allocated_meancorpuscularhemoglobin(PBProperty::Unload(*src.m_MeanCorpuscularHemoglobin));
    if (src.HasMeanCorpuscularVolume())
      bc->set_allocated_meancorpuscularvolume(PBProperty::Unload(*src.m_MeanCorpuscularVolume));
    if (src.HasStandardDiffusionDistance())
      bc->set_allocated_standarddiffusiondistance(PBProperty::Unload(*src.m_StandardDiffusionDistance));
    if (src.HasStandardOxygenDiffusionCoefficient())
      bc->set_allocated_standardoxygendiffusioncoefficient(PBProperty::Unload(*src.m_StandardOxygenDiffusionCoefficient));

    // Cardiovascular
    PULSE_BIND::ConfigurationData_CardiovascularConfigurationData* cv = dst.mutable_cardiovascularconfiguration();
    if (src.HasLeftHeartElastanceMaximum())
      cv->set_allocated_leftheartelastancemaximum(PBProperty::Unload(*src.m_LeftHeartElastanceMaximum));
    if (src.HasLeftHeartElastanceMinimum())
      cv->set_allocated_leftheartelastanceminimum(PBProperty::Unload(*src.m_LeftHeartElastanceMinimum));
    if (src.HasMinimumBloodVolumeFraction())
      cv->set_allocated_minimumbloodvolumefraction(PBProperty::Unload(*src.m_MinimumBloodVolumeFraction));
    if (src.HasRightHeartElastanceMaximum())
      cv->set_allocated_rightheartelastancemaximum(PBProperty::Unload(*src.m_RightHeartElastanceMaximum));
    if (src.HasRightHeartElastanceMinimum())
      cv->set_allocated_rightheartelastanceminimum(PBProperty::Unload(*src.m_RightHeartElastanceMinimum));
    if (src.HasStandardPulmonaryCapillaryCoverage())
      cv->set_allocated_standardpulmonarycapillarycoverage(PBProperty::Unload(*src.m_StandardPulmonaryCapillaryCoverage));
    cv->set_useexpandedvasculature((CDM_BIND::eSwitch)src.m_UseExpandedVasculature);
    cv->set_tunecardiovascularcircuit((CDM_BIND::eSwitch)src.m_TuneCardiovascularCircuit);
    cv->set_cardiovasculartuningfile(src.m_CardiovascularTuningFile);


    // Circuits
    PULSE_BIND::ConfigurationData_CircuitConfigurationData* circuit = dst.mutable_circuitconfiguration();
    if (src.HasCardiovascularOpenResistance())
      circuit->set_allocated_cardiovascularopenresistance(PBProperty::Unload(*src.m_CardiovascularOpenResistance));
    if (src.HasDefaultClosedElectricResistance())
      circuit->set_allocated_defaultclosedelectricresistance(PBProperty::Unload(*src.m_DefaultClosedElectricResistance));
    if (src.HasDefaultClosedFlowResistance())
      circuit->set_allocated_defaultclosedflowresistance(PBProperty::Unload(*src.m_DefaultClosedFlowResistance));
    if (src.HasDefaultClosedHeatResistance())
      circuit->set_allocated_defaultclosedheatresistance(PBProperty::Unload(*src.m_DefaultClosedHeatResistance));
    if (src.HasDefaultOpenElectricResistance())
      circuit->set_allocated_defaultopenelectricresistance(PBProperty::Unload(*src.m_DefaultOpenElectricResistance));
    if (src.HasDefaultOpenFlowResistance())
      circuit->set_allocated_defaultopenflowresistance(PBProperty::Unload(*src.m_DefaultOpenFlowResistance));
    if (src.HasDefaultOpenHeatResistance())
      circuit->set_allocated_defaultopenheatresistance(PBProperty::Unload(*src.m_DefaultOpenHeatResistance));
    if (src.HasMachineClosedResistance())
      circuit->set_allocated_machineclosedresistance(PBProperty::Unload(*src.m_MachineClosedResistance));
    if (src.HasMachineOpenResistance())
      circuit->set_allocated_machineopenresistance(PBProperty::Unload(*src.m_MachineOpenResistance));
    if (src.HasRespiratoryClosedResistance())
      circuit->set_allocated_respiratoryclosedresistance(PBProperty::Unload(*src.m_RespiratoryClosedResistance));
    if (src.HasRespiratoryOpenResistance())
      circuit->set_allocated_respiratoryopenresistance(PBProperty::Unload(*src.m_RespiratoryOpenResistance));

    // Constants
    PULSE_BIND::ConfigurationData_ConstantsConfigurationData* consts = dst.mutable_constantsconfiguration();
    if (src.HasOxygenMetabolicConstant())
      consts->set_allocated_oxygenmetabolicconstant(PBProperty::Unload(*src.m_OxygenMetabolicConstant));
    if (src.HasStefanBoltzmann())
      consts->set_allocated_stefanboltzmann(PBProperty::Unload(*src.m_StefanBoltzmann));
    if (src.HasUniversalGasConstant())
      consts->set_allocated_universalgasconstant(PBProperty::Unload(*src.m_UniversalGasConstant));

    // Drugs
    PULSE_BIND::ConfigurationData_DrugsConfigurationData* drugs = dst.mutable_drugsconfiguration();
    drugs->set_pdmodel((CDM_BIND::eSwitch)src.m_PDEnabled);

    // Energy
    PULSE_BIND::ConfigurationData_EnergyConfigurationData* energy = dst.mutable_energyconfiguration();
    if (src.HasBodySpecificHeat())
      energy->set_allocated_bodyspecificheat(PBProperty::Unload(*src.m_BodySpecificHeat));
    if (src.HasCarbonDioxideProductionFromOxygenConsumptionConstant())
      energy->set_allocated_carbondioxideproductionfromoxygenconsumptionconstant(PBProperty::Unload(*src.m_CarbonDioxideProductionFromOxygenConsumptionConstant));
    if (src.HasCoreTemperatureLow())
      energy->set_allocated_coretemperaturelow(PBProperty::Unload(*src.m_CoreTemperatureLow));
    if (src.HasCoreTemperatureHigh())
      energy->set_allocated_coretemperaturehigh(PBProperty::Unload(*src.m_CoreTemperatureHigh));
    if (src.HasDeltaCoreTemperatureLow())
      energy->set_allocated_deltacoretemperaturelow(PBProperty::Unload(*src.m_DeltaCoreTemperatureLow));
    if (src.HasEnergyPerATP())
      energy->set_allocated_energyperatp(PBProperty::Unload(*src.m_EnergyPerATP));
    if (src.HasSweatHeatTransfer())
      energy->set_allocated_sweatheattransfer(PBProperty::Unload(*src.m_SweatHeatTransfer));
    if (src.HasVaporSpecificHeat())
      energy->set_allocated_vaporspecificheat(PBProperty::Unload(*src.m_VaporSpecificHeat));
    if (src.HasVaporizationEnergy())
      energy->set_allocated_vaporizationenergy(PBProperty::Unload(*src.m_VaporizationEnergy));

    // Environment
    PULSE_BIND::ConfigurationData_EnvironmentConfigurationData* env = dst.mutable_environmentconfiguration();
    if (src.HasAirDensity())
      env->set_allocated_airdensity(PBProperty::Unload(*src.m_AirDensity));
    if (src.HasAirSpecificHeat())
      env->set_allocated_airspecificheat(PBProperty::Unload(*src.m_AirSpecificHeat));
    if (src.HasMolarMassOfDryAir())
      env->set_allocated_molarmassofdryair(PBProperty::Unload(*src.m_MolarMassOfDryAir));
    if (src.HasMolarMassOfWaterVapor())
      env->set_allocated_molarmassofwatervapor(PBProperty::Unload(*src.m_MolarMassOfWaterVapor));
    if (src.HasInitialEnvironmentalConditions())
      env->set_allocated_initialconditions(PBEnvironment::Unload(*src.m_InitialEnvironmentalConditions));
    if (src.HasWaterDensity())
      env->set_allocated_waterdensity(PBProperty::Unload(*src.m_WaterDensity));

    // Gastrointestinal
    PULSE_BIND::ConfigurationData_GastrointestinalConfigurationData* gi = dst.mutable_gastrointestinalconfiguration();
    if (src.HasCalciumAbsorptionFraction())
      gi->set_allocated_calciumabsorptionfraction(PBProperty::Unload(*src.m_CalciumAbsorptionFraction));
    if (src.HasCalciumDigestionRate())
      gi->set_allocated_calciumdigestionrate(PBProperty::Unload(*src.m_CalciumDigestionRate));
    if (src.HasCarbohydrateAbsorptionFraction())
      gi->set_allocated_carbohydrateabsorptionfraction(PBProperty::Unload(*src.m_CarbohydrateAbsorptionFraction));
    if (src.HasDefaultCarbohydrateDigestionRate())
      gi->set_allocated_defaultcarbohydratedigestionrate(PBProperty::Unload(*src.m_DefaultCarbohydrateDigestionRate));
    if (src.HasDefaultFatDigestionRate())
      gi->set_allocated_defaultfatdigestionrate(PBProperty::Unload(*src.m_DefaultFatDigestionRate));
    if (src.HasDefaultProteinDigestionRate())
      gi->set_allocated_defaultproteindigestionrate(PBProperty::Unload(*src.m_DefaultProteinDigestionRate));
    if (src.HasDefaultStomachContents())
      gi->set_allocated_initialstomachcontents(PBPatientNutrition::Unload(*src.m_DefaultStomachContents));
    if (src.HasFatAbsorptionFraction())
      gi->set_allocated_fatabsorptionfraction(PBProperty::Unload(*src.m_FatAbsorptionFraction));
    if (src.HasProteinToUreaFraction())
      gi->set_allocated_proteintoureafraction(PBProperty::Unload(*src.m_ProteinToUreaFraction));
    if (src.HasWaterDigestionRate())
      gi->set_allocated_waterdigestionrate(PBProperty::Unload(*src.m_WaterDigestionRate));

    // Nervous
    PULSE_BIND::ConfigurationData_NervousConfigurationData* n = dst.mutable_nervousconfiguration();
    n->set_enablecerebrospinalfluid((CDM_BIND::eSwitch)src.m_CerebrospinalFluidEnabled);
    n->set_baroreceptorfeedback((CDM_BIND::eSwitch)src.m_BaroreceptorFeedback);
    n->set_chemoreceptorfeedback((CDM_BIND::eSwitch)src.m_ChemoreceptorFeedback);
    if (src.HasHeartElastanceDistributedTimeDelay())
      n->set_allocated_heartelastancedistributedtimedelay(PBProperty::Unload(*src.m_HeartElastanceDistributedTimeDelay));
    if (src.HasHeartRateDistributedTimeDelay())
      n->set_allocated_heartratedistributedtimedelay(PBProperty::Unload(*src.m_HeartRateDistributedTimeDelay));
    if (src.HasNormalizedHeartRateIntercept())
      n->set_allocated_normalizedheartrateintercept(PBProperty::Unload(*src.m_NormalizedHeartRateIntercept));
    if (src.HasNormalizedHeartRateSympatheticSlope())
      n->set_allocated_normalizedheartratesympatheticslope(PBProperty::Unload(*src.m_NormalizedHeartRateSympatheticSlope));
    if (src.HasNormalizedHeartRateParasympatheticSlope())
      n->set_allocated_normalizedheartrateparasympatheticslope(PBProperty::Unload(*src.m_NormalizedHeartRateParasympatheticSlope));
    if (src.HasNormalizedHeartElastanceIntercept())
      n->set_allocated_normalizedheartelastanceintercept(PBProperty::Unload(*src.m_NormalizedHeartElastanceIntercept));
    if (src.HasNormalizedHeartElastanceSympatheticSlope())
      n->set_allocated_normalizedheartelastancesympatheticslope(PBProperty::Unload(*src.m_NormalizedHeartElastanceSympatheticSlope));
    if (src.HasNormalizedResistanceIntercept())
      n->set_allocated_normalizedresistanceintercept(PBProperty::Unload(*src.m_NormalizedResistanceIntercept));
    if (src.HasNormalizedResistanceSympatheticSlope())
      n->set_allocated_normalizedresistancesympatheticslope(PBProperty::Unload(*src.m_NormalizedResistanceSympatheticSlope));
    if (src.HasNormalizedComplianceIntercept())
      n->set_allocated_normalizedcomplianceintercept(PBProperty::Unload(*src.m_NormalizedComplianceIntercept));
    if (src.HasNormalizedComplianceParasympatheticSlope())
      n->set_allocated_normalizedcomplianceparasympatheticslope(PBProperty::Unload(*src.m_NormalizedComplianceParasympatheticSlope));
    if (src.HasResponseSlope())
      n->set_allocated_responseslope(PBProperty::Unload(*src.m_ResponseSlope));
    if (src.HasPupilDiameterBaseline())
      n->set_allocated_pupildiameterbaseline(PBProperty::Unload(*src.m_PupilDiameterBaseline));
    if (src.HasSystemicResistanceDistributedTimeDelay())
      n->set_allocated_systemicresistancedistributedtimedelay(PBProperty::Unload(*src.m_SystemicResistanceDistributedTimeDelay));
    if (src.HasVenousComplianceDistributedTimeDelay())
      n->set_allocated_venouscompliancedistributedtimedelay(PBProperty::Unload(*src.m_VenousComplianceDistributedTimeDelay));

    // Renal
    PULSE_BIND::ConfigurationData_RenalConfigurationData* renal = dst.mutable_renalconfiguration();
    renal->set_enablerenal((CDM_BIND::eSwitch)src.m_RenalEnabled);
    if (src.HasPlasmaSodiumConcentrationSetPoint())
      renal->set_allocated_plasmasodiumconcentrationsetpoint(PBProperty::Unload(*src.m_PlasmaSodiumConcentrationSetPoint));
    if (src.HasPeritubularPotassiumConcentrationSetPoint())
      renal->set_allocated_peritubularpotassiumconcentrationsetpoint(PBProperty::Unload(*src.m_PeritubularPotassiumConcentrationSetPoint));

    if (src.HasLeftGlomerularFilteringSurfaceAreaBaseline())
      renal->set_allocated_leftglomerularfilteringsurfaceareabaseline(PBProperty::Unload(*src.m_LeftGlomerularFilteringSurfaceAreaBaseline));
    if (src.HasLeftGlomerularFluidPermeabilityBaseline())
      renal->set_allocated_leftglomerularfluidpermeabilitybaseline(PBProperty::Unload(*src.m_LeftGlomerularFluidPermeabilityBaseline));
    if (src.HasLeftTubularReabsorptionFilteringSurfaceAreaBaseline())
      renal->set_allocated_lefttubularreabsorptionfilteringsurfaceareabaseline(PBProperty::Unload(*src.m_LeftTubularReabsorptionFilteringSurfaceAreaBaseline));
    if (src.HasLeftTubularReabsorptionFluidPermeabilityBaseline())
      renal->set_allocated_lefttubularreabsorptionfluidpermeabilitybaseline(PBProperty::Unload(*src.m_LeftTubularReabsorptionFluidPermeabilityBaseline));

    if (src.HasMaximumAfferentResistance())
      renal->set_allocated_maximumafferentresistance(PBProperty::Unload(*src.m_MaximumAfferentResistance));
    if (src.HasMinimumAfferentResistance())
      renal->set_allocated_minimumafferentresistance(PBProperty::Unload(*src.m_MinimumAfferentResistance));

    if (src.HasRightGlomerularFilteringSurfaceAreaBaseline())
      renal->set_allocated_rightglomerularfilteringsurfaceareabaseline(PBProperty::Unload(*src.m_RightGlomerularFilteringSurfaceAreaBaseline));
    if (src.HasRightGlomerularFluidPermeabilityBaseline())
      renal->set_allocated_rightglomerularfluidpermeabilitybaseline(PBProperty::Unload(*src.m_RightGlomerularFluidPermeabilityBaseline));
    if (src.HasRightTubularReabsorptionFilteringSurfaceAreaBaseline())
      renal->set_allocated_righttubularreabsorptionfilteringsurfaceareabaseline(PBProperty::Unload(*src.m_RightTubularReabsorptionFilteringSurfaceAreaBaseline));
    if (src.HasRightTubularReabsorptionFluidPermeabilityBaseline())
      renal->set_allocated_righttubularreabsorptionfluidpermeabilitybaseline(PBProperty::Unload(*src.m_RightTubularReabsorptionFluidPermeabilityBaseline));

    // Respiratory
    PULSE_BIND::ConfigurationData_RespiratoryConfigurationData* resp = dst.mutable_respiratoryconfiguration();
    if (src.HasCentralControllerCO2PressureSetPoint())
      resp->set_allocated_centralcontrollerco2pressuresetpoint(PBProperty::Unload(*src.m_CentralControllerCO2PressureSetPoint));
    if (src.HasCentralVentilatoryControllerGain())
      resp->set_allocated_centralventilatorycontrollergain(PBProperty::Unload(*src.m_CentralVentilatoryControllerGain));
    if (src.HasMinimumAllowableInpiratoryAndExpiratoryPeriod())
      resp->set_allocated_minimumallowableinspiratoryandexpiratoryperiod(PBProperty::Unload(*src.m_MinimumAllowableInpiratoryAndExpiratoryPeriod));
    if (src.HasMinimumAllowableTidalVolume())
      resp->set_allocated_minimumallowabletidalvolume(PBProperty::Unload(*src.m_MinimumAllowableTidalVolume));
    if (src.HasPeripheralControllerCO2PressureSetPoint())
      resp->set_allocated_peripheralcontrollerco2pressuresetpoint(PBProperty::Unload(*src.m_PeripheralControllerCO2PressureSetPoint));
    if (src.HasPeripheralVentilatoryControllerGain())
      resp->set_allocated_peripheralventilatorycontrollergain(PBProperty::Unload(*src.m_PeripheralVentilatoryControllerGain));
    if (src.HasPulmonaryVentilationRateMaximum())
      resp->set_allocated_pulmonaryventilationratemaximum(PBProperty::Unload(*src.m_PulmonaryVentilationRateMaximum));
    if (src.HasVentilationTidalVolumeIntercept())
      resp->set_allocated_ventilationtidalvolumeintercept(PBProperty::Unload(*src.m_VentilationTidalVolumeIntercept));
    if (src.HasVentilatoryOcclusionPressure())
      resp->set_allocated_ventilatoryocclusionpressure(PBProperty::Unload(*src.m_VentilatoryOcclusionPressure));
    resp->set_useexpandedrespiratory((CDM_BIND::eSwitch)src.m_UseExpandedRespiratory);

    // Tissue
    PULSE_BIND::ConfigurationData_TissueConfigurationData* tissue = dst.mutable_tissueconfiguration();
    tissue->set_enabletissue((CDM_BIND::eSwitch)src.m_TissueEnabled);
  }

  bool PBConfiguration::SerializeToString(const PulseConfiguration& src, std::string& output, eSerializationFormat m)
  {
    PULSE_BIND::ConfigurationData data;
    PBConfiguration::Serialize(src, data);
    return PBUtils::SerializeToString(data, output, m, src.GetLogger());
  }
  bool PBConfiguration::SerializeToFile(const PulseConfiguration& src, const std::string& filename)
  {
    PULSE_BIND::ConfigurationData data;
    PBConfiguration::Serialize(src, data);
    return PBUtils::SerializeToFile(data, filename, src.GetLogger());
  }

  bool PBConfiguration::SerializeFromString(const std::string& src, PulseConfiguration& dst, eSerializationFormat m, SESubstanceManager& subMgr)
  {
    PULSE_BIND::ConfigurationData data;
    if (!PBUtils::SerializeFromString(src, data, m, dst.GetLogger()))
      return false;
    PBConfiguration::Load(data, dst, subMgr);
    return true;
  }
  bool PBConfiguration::SerializeFromFile(const std::string& filename, PulseConfiguration& dst, SESubstanceManager& subMgr)
  {
    PULSE_BIND::ConfigurationData data;
    if (!PBUtils::SerializeFromFile(filename, data, dst.GetLogger()))
      return false;
    PBConfiguration::Load(data, dst, subMgr);
    return true;
  }
}
