/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/PulseConfiguration.h"
#include "engine/io/protobuf/PBConfiguration.h"
#include "engine/common/controller/CircuitManager.h"

#include "cdm/patient/SENutrition.h"
#include "cdm/engine/SEDynamicStabilization.h"
#include "cdm/engine/SETimedStabilization.h"
#include "cdm/engine/SEOverrides.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceFraction.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/system/environment/SEEnvironmentalConditions.h"
#include "cdm/system/equipment/electrocardiogram/SEElectroCardioGram.h"

#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarArea.h"
#include "cdm/properties/SEScalarAreaPerTimePressure.h"
#include "cdm/properties/SEScalarElectricResistance.h"
#include "cdm/properties/SEScalarEnergyPerAmount.h"
#include "cdm/properties/SEScalarEnergyPerMass.h"
#include "cdm/properties/SEScalarHeatCapacitancePerAmount.h"
#include "cdm/properties/SEScalarHeatCapacitancePerMass.h"
#include "cdm/properties/SEScalarHeatResistance.h"
#include "cdm/properties/SEScalarHeatResistanceArea.h"
#include "cdm/properties/SEScalarHeatConductance.h"
#include "cdm/properties/SEScalarInverseVolume.h"
#include "cdm/properties/SEScalarLength.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerAmount.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarMassPerTime.h"
#include "cdm/properties/SEScalarPowerPerAreaTemperatureToTheFourth.h"
#include "cdm/properties/SEScalarPressurePerVolume.h"
#include "cdm/properties/SEScalarPressureTimePerVolume.h"
#include "cdm/properties/SEScalarTemperature.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTimeArea.h"
#include "cdm/properties/SEScalarVolumePerTimePressureArea.h"
#include "cdm/properties/SEScalarVolumePerTime.h"

#include "cdm/utils/FileUtils.h"

PulseConfiguration::PulseConfiguration(Logger* logger) : SEEngineConfiguration(logger)
{
  m_TimeStep = nullptr;
  m_AllowDynamicTimeStep = eSwitch::Off;
  m_TimedStabilization = nullptr;
  m_DynamicStabilization = nullptr;

  // Blood Chemistry
  m_MeanCorpuscularHemoglobin = nullptr;
  m_MeanCorpuscularVolume = nullptr;
  m_StandardDiffusionDistance = nullptr;
  m_StandardOxygenDiffusionCoefficient = nullptr;

  // Cardiovascular
  m_LeftHeartElastanceMaximum = nullptr;
  m_LeftHeartElastanceMinimum = nullptr;
  m_MinimumBloodVolumeFraction = nullptr;
  m_RightHeartElastanceMaximum = nullptr;
  m_RightHeartElastanceMinimum = nullptr;
  m_StandardPulmonaryCapillaryCoverage = nullptr;
  m_UseExpandedVasculature = eSwitch::NullSwitch;
  m_TuneCardiovascularCircuit = eSwitch::On;
  m_CardiovascularTuningFile = "";

  // Circuit
  m_CardiovascularOpenResistance = nullptr;
  m_DefaultClosedElectricResistance = nullptr;
  m_DefaultClosedFlowResistance = nullptr;
  m_DefaultClosedHeatResistance = nullptr;
  m_DefaultOpenElectricResistance = nullptr;
  m_DefaultOpenFlowResistance = nullptr;
  m_DefaultOpenHeatResistance = nullptr;
  m_MachineClosedResistance = nullptr;
  m_MachineOpenResistance = nullptr;
  m_RespiratoryClosedResistance = nullptr;
  m_RespiratoryOpenResistance = nullptr;

  // Constants
  m_OxygenMetabolicConstant = nullptr;
  m_StefanBoltzmann = nullptr;
  m_UniversalGasConstant = nullptr;

  // Drugs 
  m_PDEnabled = eSwitch::On;

  // ECG
  m_ECG = nullptr;

  // Energy
  m_BodySpecificHeat = nullptr;
  m_CarbonDioxideProductionFromOxygenConsumptionConstant = nullptr;
  m_CoreTemperatureLow = nullptr;
  m_CoreTemperatureHigh = nullptr;
  m_DeltaCoreTemperatureLow = nullptr;
  m_EnergyPerATP = nullptr;
  m_SweatHeatTransfer = nullptr;
  m_VaporizationEnergy = nullptr;
  m_VaporSpecificHeat = nullptr;

  // Environment
  m_AirDensity = nullptr;
  m_AirSpecificHeat = nullptr;
  m_MolarMassOfDryAir = nullptr;
  m_MolarMassOfWaterVapor = nullptr;
  m_InitialEnvironmentalConditions = nullptr;
  m_WaterDensity = nullptr;

  // Gastrointestinal
  m_CalciumDigestionRate = nullptr;
  m_CalciumAbsorptionFraction = nullptr;
  m_CarbohydrateAbsorptionFraction = nullptr;
  m_DefaultCarbohydrateDigestionRate = nullptr;
  m_DefaultFatDigestionRate = nullptr;
  m_DefaultProteinDigestionRate = nullptr;
  m_DefaultStomachContents = nullptr;
  m_FatAbsorptionFraction = nullptr;
  m_ProteinToUreaFraction = nullptr;
  m_WaterDigestionRate = nullptr;

  // Nervous
  m_CerebrospinalFluidEnabled = eSwitch::Off;
  m_BaroreceptorFeedback = eSwitch::On;
  m_ChemoreceptorFeedback = eSwitch::On;
  m_HeartElastanceDistributedTimeDelay = nullptr;
  m_HeartRateDistributedTimeDelay = nullptr;
  m_NormalizedHeartRateIntercept = nullptr;
  m_NormalizedHeartRateSympatheticSlope = nullptr;
  m_NormalizedHeartRateParasympatheticSlope = nullptr;
  m_NormalizedHeartElastanceIntercept = nullptr;
  m_NormalizedHeartElastanceSympatheticSlope = nullptr;
  m_NormalizedResistanceIntercept = nullptr;
  m_NormalizedResistanceSympatheticSlope = nullptr;
  m_NormalizedComplianceIntercept = nullptr;
  m_NormalizedComplianceParasympatheticSlope = nullptr;
  m_PupilDiameterBaseline = nullptr;
  m_ResponseSlope = nullptr;
  m_SystemicResistanceDistributedTimeDelay = nullptr;
  m_VenousComplianceDistributedTimeDelay = nullptr;

  // Renal
  m_RenalEnabled = eSwitch::On;
  m_PlasmaSodiumConcentrationSetPoint = nullptr;
  m_PeritubularPotassiumConcentrationSetPoint = nullptr;
  m_LeftGlomerularFluidPermeabilityBaseline = nullptr;
  m_LeftGlomerularFilteringSurfaceAreaBaseline = nullptr;
  m_LeftTubularReabsorptionFluidPermeabilityBaseline = nullptr;
  m_LeftTubularReabsorptionFilteringSurfaceAreaBaseline = nullptr;
  m_MaximumAfferentResistance = nullptr;
  m_MinimumAfferentResistance = nullptr;
  m_RightGlomerularFluidPermeabilityBaseline = nullptr;
  m_RightGlomerularFilteringSurfaceAreaBaseline = nullptr;
  m_RightTubularReabsorptionFluidPermeabilityBaseline = nullptr;
  m_RightTubularReabsorptionFilteringSurfaceAreaBaseline = nullptr;
  m_TargetSodiumDelivery = nullptr;
  

  // Respiratory
  m_CentralControllerCO2PressureSetPoint = nullptr;
  m_CentralVentilatoryControllerGain = nullptr;
  m_MinimumAllowableInpiratoryAndExpiratoryPeriod = nullptr;
  m_MinimumAllowableTidalVolume = nullptr;
  m_PeripheralControllerCO2PressureSetPoint = nullptr;
  m_PeripheralVentilatoryControllerGain = nullptr;
  m_PulmonaryVentilationRateMaximum = nullptr;
  m_VentilationTidalVolumeIntercept = nullptr;
  m_VentilatoryOcclusionPressure = nullptr;
  m_UseExpandedRespiratory = eSwitch::NullSwitch;

  // Tissue
  m_TissueEnabled = eSwitch::On;
}

PulseConfiguration::~PulseConfiguration()
{
  SAFE_DELETE(m_TimeStep);
  m_AllowDynamicTimeStep = eSwitch::Off;
  m_Overrides.clear();
  m_Modifiers.clear();

  // Blood Chemistry
  SAFE_DELETE(m_MeanCorpuscularHemoglobin);
  SAFE_DELETE(m_MeanCorpuscularVolume);
  SAFE_DELETE(m_StandardDiffusionDistance);
  SAFE_DELETE(m_StandardOxygenDiffusionCoefficient);

  // Cardiovascular
  SAFE_DELETE(m_LeftHeartElastanceMaximum);
  SAFE_DELETE(m_LeftHeartElastanceMinimum);
  SAFE_DELETE(m_MinimumBloodVolumeFraction);
  SAFE_DELETE(m_RightHeartElastanceMaximum);
  SAFE_DELETE(m_RightHeartElastanceMinimum);
  SAFE_DELETE(m_StandardPulmonaryCapillaryCoverage);
  m_UseExpandedVasculature = eSwitch::NullSwitch;
  m_TuneCardiovascularCircuit = eSwitch::On;
  m_CardiovascularTuningFile = "";

  // Circuit
  SAFE_DELETE(m_CardiovascularOpenResistance);
  SAFE_DELETE(m_DefaultClosedElectricResistance);
  SAFE_DELETE(m_DefaultClosedFlowResistance);
  SAFE_DELETE(m_DefaultClosedHeatResistance);
  SAFE_DELETE(m_DefaultOpenElectricResistance);
  SAFE_DELETE(m_DefaultOpenFlowResistance);
  SAFE_DELETE(m_DefaultOpenHeatResistance);
  SAFE_DELETE(m_MachineClosedResistance);
  SAFE_DELETE(m_MachineOpenResistance);
  SAFE_DELETE(m_RespiratoryClosedResistance);
  SAFE_DELETE(m_RespiratoryOpenResistance);

  // Constants
  SAFE_DELETE(m_OxygenMetabolicConstant);
  SAFE_DELETE(m_StefanBoltzmann);
  SAFE_DELETE(m_UniversalGasConstant);

  // Drugs

  //  ECG
  SAFE_DELETE(m_ECG);

  // Energy
  SAFE_DELETE(m_BodySpecificHeat);
  SAFE_DELETE(m_CarbonDioxideProductionFromOxygenConsumptionConstant);
  SAFE_DELETE(m_CoreTemperatureLow);
  SAFE_DELETE(m_CoreTemperatureHigh);
  SAFE_DELETE(m_DeltaCoreTemperatureLow);
  SAFE_DELETE(m_EnergyPerATP);
  SAFE_DELETE(m_SweatHeatTransfer);
  SAFE_DELETE(m_VaporizationEnergy);
  SAFE_DELETE(m_VaporSpecificHeat);

  // Environment
  SAFE_DELETE(m_AirDensity);
  SAFE_DELETE(m_AirSpecificHeat);
  SAFE_DELETE(m_MolarMassOfDryAir);
  SAFE_DELETE(m_MolarMassOfWaterVapor);
  SAFE_DELETE(m_InitialEnvironmentalConditions);
  SAFE_DELETE(m_WaterDensity);

  // Gastrointestinal
  SAFE_DELETE(m_CalciumDigestionRate);
  SAFE_DELETE(m_CalciumAbsorptionFraction);
  SAFE_DELETE(m_CarbohydrateAbsorptionFraction);
  SAFE_DELETE(m_DefaultCarbohydrateDigestionRate);
  SAFE_DELETE(m_DefaultFatDigestionRate);
  SAFE_DELETE(m_DefaultProteinDigestionRate);
  SAFE_DELETE(m_DefaultStomachContents);
  SAFE_DELETE(m_FatAbsorptionFraction);
  SAFE_DELETE(m_ProteinToUreaFraction);
  SAFE_DELETE(m_WaterDigestionRate);

  // Nervous
  SAFE_DELETE(m_HeartElastanceDistributedTimeDelay);
  SAFE_DELETE(m_HeartRateDistributedTimeDelay);
  SAFE_DELETE(m_NormalizedHeartRateIntercept);
  SAFE_DELETE(m_NormalizedHeartRateSympatheticSlope);
  SAFE_DELETE(m_NormalizedHeartRateParasympatheticSlope);
  SAFE_DELETE(m_NormalizedHeartElastanceIntercept);
  SAFE_DELETE(m_NormalizedHeartElastanceSympatheticSlope);
  SAFE_DELETE(m_NormalizedResistanceIntercept);
  SAFE_DELETE(m_NormalizedResistanceSympatheticSlope);
  SAFE_DELETE(m_NormalizedComplianceIntercept);
  SAFE_DELETE(m_NormalizedComplianceParasympatheticSlope);
  SAFE_DELETE(m_PupilDiameterBaseline);
  SAFE_DELETE(m_ResponseSlope);
  SAFE_DELETE(m_SystemicResistanceDistributedTimeDelay);
  SAFE_DELETE(m_VenousComplianceDistributedTimeDelay);

  // Renal
  SAFE_DELETE(m_PlasmaSodiumConcentrationSetPoint);
  SAFE_DELETE(m_PeritubularPotassiumConcentrationSetPoint);
  SAFE_DELETE(m_LeftGlomerularFluidPermeabilityBaseline);
  SAFE_DELETE(m_LeftGlomerularFilteringSurfaceAreaBaseline);
  SAFE_DELETE(m_LeftTubularReabsorptionFluidPermeabilityBaseline);
  SAFE_DELETE(m_LeftTubularReabsorptionFilteringSurfaceAreaBaseline);
  SAFE_DELETE(m_MaximumAfferentResistance);
  SAFE_DELETE(m_MinimumAfferentResistance);
  SAFE_DELETE(m_RightGlomerularFluidPermeabilityBaseline);
  SAFE_DELETE(m_RightGlomerularFilteringSurfaceAreaBaseline);
  SAFE_DELETE(m_RightTubularReabsorptionFluidPermeabilityBaseline);
  SAFE_DELETE(m_RightTubularReabsorptionFilteringSurfaceAreaBaseline);
  SAFE_DELETE(m_TargetSodiumDelivery);

  // Respiratory
  SAFE_DELETE(m_CentralControllerCO2PressureSetPoint);
  SAFE_DELETE(m_CentralVentilatoryControllerGain);
  SAFE_DELETE(m_MinimumAllowableInpiratoryAndExpiratoryPeriod);
  SAFE_DELETE(m_MinimumAllowableTidalVolume);
  SAFE_DELETE(m_PeripheralControllerCO2PressureSetPoint);
  SAFE_DELETE(m_PeripheralVentilatoryControllerGain);
  SAFE_DELETE(m_PulmonaryVentilationRateMaximum);
  SAFE_DELETE(m_VentilationTidalVolumeIntercept);
  SAFE_DELETE(m_VentilatoryOcclusionPressure);
  m_UseExpandedRespiratory = eSwitch::NullSwitch;

  //Tissue
}

void PulseConfiguration::Clear()
{
  INVALIDATE_PROPERTY(m_TimeStep);
  m_AllowDynamicTimeStep = eSwitch::Off;
  RemoveStabilization();
  m_Overrides.clear();
  m_Modifiers.clear();
  
  // Blood Chemistry
  INVALIDATE_PROPERTY(m_MeanCorpuscularHemoglobin);
  INVALIDATE_PROPERTY(m_MeanCorpuscularVolume);
  INVALIDATE_PROPERTY(m_StandardDiffusionDistance);
  INVALIDATE_PROPERTY(m_StandardOxygenDiffusionCoefficient);

  // Cardiovascular
  INVALIDATE_PROPERTY(m_LeftHeartElastanceMaximum);
  INVALIDATE_PROPERTY(m_LeftHeartElastanceMinimum);
  INVALIDATE_PROPERTY(m_MinimumBloodVolumeFraction);
  INVALIDATE_PROPERTY(m_RightHeartElastanceMaximum);
  INVALIDATE_PROPERTY(m_RightHeartElastanceMinimum);
  INVALIDATE_PROPERTY(m_StandardPulmonaryCapillaryCoverage);
  m_UseExpandedVasculature = eSwitch::NullSwitch;
  m_TuneCardiovascularCircuit = eSwitch::On;
  m_CardiovascularTuningFile = "";

  // Circuit
  INVALIDATE_PROPERTY(m_CardiovascularOpenResistance);
  INVALIDATE_PROPERTY(m_DefaultClosedElectricResistance);
  INVALIDATE_PROPERTY(m_DefaultClosedFlowResistance);
  INVALIDATE_PROPERTY(m_DefaultClosedHeatResistance);
  INVALIDATE_PROPERTY(m_DefaultOpenElectricResistance);
  INVALIDATE_PROPERTY(m_DefaultOpenFlowResistance);
  INVALIDATE_PROPERTY(m_DefaultOpenHeatResistance);
  INVALIDATE_PROPERTY(m_MachineClosedResistance);
  INVALIDATE_PROPERTY(m_MachineOpenResistance);
  INVALIDATE_PROPERTY(m_RespiratoryClosedResistance);
  INVALIDATE_PROPERTY(m_RespiratoryOpenResistance);

  // Constants
  INVALIDATE_PROPERTY(m_OxygenMetabolicConstant);
  INVALIDATE_PROPERTY(m_StefanBoltzmann);
  INVALIDATE_PROPERTY(m_UniversalGasConstant);

  // Drugs
  m_PDEnabled = eSwitch::On;

  //  ECG
  if (m_ECG)
    m_ECG->Clear();

  // Energy
  INVALIDATE_PROPERTY(m_BodySpecificHeat);
  INVALIDATE_PROPERTY(m_CarbonDioxideProductionFromOxygenConsumptionConstant);
  INVALIDATE_PROPERTY(m_CoreTemperatureLow);
  INVALIDATE_PROPERTY(m_CoreTemperatureHigh);
  INVALIDATE_PROPERTY(m_DeltaCoreTemperatureLow);
  INVALIDATE_PROPERTY(m_EnergyPerATP);
  INVALIDATE_PROPERTY(m_SweatHeatTransfer);
  INVALIDATE_PROPERTY(m_VaporizationEnergy);
  INVALIDATE_PROPERTY(m_VaporSpecificHeat);

  // Environment
  INVALIDATE_PROPERTY(m_AirDensity);
  INVALIDATE_PROPERTY(m_AirSpecificHeat);
  INVALIDATE_PROPERTY(m_MolarMassOfDryAir);
  INVALIDATE_PROPERTY(m_MolarMassOfWaterVapor);
  if (m_InitialEnvironmentalConditions)
    m_InitialEnvironmentalConditions->Clear();
  INVALIDATE_PROPERTY(m_WaterDensity);

  // Gastrointestinal
  INVALIDATE_PROPERTY(m_CalciumDigestionRate);
  INVALIDATE_PROPERTY(m_CalciumAbsorptionFraction);
  INVALIDATE_PROPERTY(m_CarbohydrateAbsorptionFraction);
  INVALIDATE_PROPERTY(m_DefaultCarbohydrateDigestionRate);
  INVALIDATE_PROPERTY(m_DefaultFatDigestionRate);
  INVALIDATE_PROPERTY(m_DefaultProteinDigestionRate);
  if (m_DefaultStomachContents)
    m_DefaultStomachContents->Clear();
  INVALIDATE_PROPERTY(m_FatAbsorptionFraction);
  INVALIDATE_PROPERTY(m_ProteinToUreaFraction);
  INVALIDATE_PROPERTY(m_WaterDigestionRate);

  // Nervous
  m_CerebrospinalFluidEnabled = eSwitch::Off;
  m_BaroreceptorFeedback = eSwitch::On;
  m_ChemoreceptorFeedback = eSwitch::On;
  INVALIDATE_PROPERTY(m_HeartElastanceDistributedTimeDelay);
  INVALIDATE_PROPERTY(m_HeartRateDistributedTimeDelay);
  INVALIDATE_PROPERTY(m_NormalizedHeartRateIntercept);
  INVALIDATE_PROPERTY(m_NormalizedHeartRateSympatheticSlope);
  INVALIDATE_PROPERTY(m_NormalizedHeartRateParasympatheticSlope);
  INVALIDATE_PROPERTY(m_NormalizedHeartElastanceIntercept);
  INVALIDATE_PROPERTY(m_NormalizedHeartElastanceSympatheticSlope);
  INVALIDATE_PROPERTY(m_NormalizedResistanceIntercept);
  INVALIDATE_PROPERTY(m_NormalizedResistanceSympatheticSlope);
  INVALIDATE_PROPERTY(m_NormalizedComplianceIntercept);
  INVALIDATE_PROPERTY(m_NormalizedComplianceParasympatheticSlope);
  INVALIDATE_PROPERTY(m_PupilDiameterBaseline);
  INVALIDATE_PROPERTY(m_ResponseSlope);
  INVALIDATE_PROPERTY(m_SystemicResistanceDistributedTimeDelay);
  INVALIDATE_PROPERTY(m_VenousComplianceDistributedTimeDelay);

  // Renal
  m_RenalEnabled = eSwitch::On;
  INVALIDATE_PROPERTY(m_PlasmaSodiumConcentrationSetPoint);
  INVALIDATE_PROPERTY(m_PeritubularPotassiumConcentrationSetPoint);
  INVALIDATE_PROPERTY(m_LeftGlomerularFluidPermeabilityBaseline);
  INVALIDATE_PROPERTY(m_LeftGlomerularFilteringSurfaceAreaBaseline);
  INVALIDATE_PROPERTY(m_LeftTubularReabsorptionFluidPermeabilityBaseline);
  INVALIDATE_PROPERTY(m_LeftTubularReabsorptionFilteringSurfaceAreaBaseline);
  INVALIDATE_PROPERTY(m_MaximumAfferentResistance);
  INVALIDATE_PROPERTY(m_MinimumAfferentResistance);
  INVALIDATE_PROPERTY(m_RightGlomerularFluidPermeabilityBaseline);
  INVALIDATE_PROPERTY(m_RightGlomerularFilteringSurfaceAreaBaseline);
  INVALIDATE_PROPERTY(m_RightTubularReabsorptionFluidPermeabilityBaseline);
  INVALIDATE_PROPERTY(m_RightTubularReabsorptionFilteringSurfaceAreaBaseline);
  INVALIDATE_PROPERTY(m_TargetSodiumDelivery);

  // Respiratory
  INVALIDATE_PROPERTY(m_CentralControllerCO2PressureSetPoint);
  INVALIDATE_PROPERTY(m_CentralVentilatoryControllerGain);
  INVALIDATE_PROPERTY(m_MinimumAllowableInpiratoryAndExpiratoryPeriod);
  INVALIDATE_PROPERTY(m_MinimumAllowableTidalVolume);
  INVALIDATE_PROPERTY(m_PeripheralControllerCO2PressureSetPoint);
  INVALIDATE_PROPERTY(m_PeripheralVentilatoryControllerGain);
  INVALIDATE_PROPERTY(m_PulmonaryVentilationRateMaximum);
  INVALIDATE_PROPERTY(m_VentilationTidalVolumeIntercept);
  INVALIDATE_PROPERTY(m_VentilatoryOcclusionPressure);
  m_UseExpandedRespiratory = eSwitch::NullSwitch;

  //Tissue
  m_TissueEnabled = eSwitch::On;
}

void PulseConfiguration::Merge(const PulseConfiguration& src, SESubstanceManager& subMgr)
{
  // At this point, I am forcing a wholesale replacement
  // If we want to actually combine modifiers between src/tgt
  // We will need to rethink this
  // This was done when supporting 2 different cv circuits
  if (src.HasModifiers())
    m_Modifiers.clear();

  pulse::PBConfiguration::Merge(src, *this, subMgr);
}

bool PulseConfiguration::SerializeToString(std::string& output, eSerializationFormat m) const
{
  return pulse::PBConfiguration::SerializeToString(*this, output, m);
}
bool PulseConfiguration::SerializeToFile(const std::string& filename) const
{
  return pulse::PBConfiguration::SerializeToFile(*this, filename);
}
bool PulseConfiguration::SerializeFromString(const std::string& src, eSerializationFormat m, SESubstanceManager& subMgr)
{
  return pulse::PBConfiguration::SerializeFromString(src, *this, m, subMgr);
}
bool PulseConfiguration::SerializeFromFile(const std::string& filename, SESubstanceManager& subMgr)
{
  return pulse::PBConfiguration::SerializeFromFile(filename, *this, subMgr);
}

void PulseConfiguration::Initialize(const std::string& dataDir, SESubstanceManager* subMgr)
{
  Clear();

  // Reset to default values
  GetTimeStep().SetValue(1.0 / 50.0, TimeUnit::s);
  m_AllowDynamicTimeStep = eSwitch::Off;
  if (!dataDir.empty())
  {
    if (!GetECG().SerializeFromFile(dataDir + "/ecg/StandardECG.json"))
      Error("Unable to read " + dataDir + "/ecg/StandardECG.json");
    if(!GetDynamicStabilization().SerializeFromFile(dataDir + "/config/DynamicStabilization.json"))
      Error("Unable to read " + dataDir + "/config/DynamicStabilization.json");
    //if(!GetTimedStabilization().SerializeFromFile(dataDir+"/config/TimedStabilization.json"))
    //  Error("Unable to read " + dataDir + "/config/TimedStabilization.json");
  }
  //GetDynamicStabilization().TrackStabilization(eSwitch::On);// Hard coded override for debugging

  //Blood Chemistry
  GetMeanCorpuscularVolume().SetValue(9.e-8, VolumeUnit::uL);// Guyton p419
  GetMeanCorpuscularHemoglobin().SetValue(29, MassPerAmountUnit::pg_Per_ct);
  GetStandardDiffusionDistance().SetValue(0.0006, LengthUnit::mm);
  GetStandardOxygenDiffusionCoefficient().SetValue(0.00000000246, AreaPerTimePressureUnit::cm2_Per_min_mmHg);

  // Cardiovascular
  GetLeftHeartElastanceMaximum().SetValue(2.49, PressurePerVolumeUnit::mmHg_Per_mL);
  GetLeftHeartElastanceMinimum().SetValue(0.049, PressurePerVolumeUnit::mmHg_Per_mL);
  GetMinimumBloodVolumeFraction().SetValue(0.65); // \cite Guyton 11th ed p.279
  GetRightHeartElastanceMaximum().SetValue(0.523, PressurePerVolumeUnit::mmHg_Per_mL);
  GetRightHeartElastanceMinimum().SetValue(0.0243, PressurePerVolumeUnit::mmHg_Per_mL);
  GetStandardPulmonaryCapillaryCoverage().SetValue(0.70);
  UseExpandedVasculature(eSwitch::Off);// Call the function to set defaults
  m_TuneCardiovascularCircuit = eSwitch::On;
  m_CardiovascularTuningFile = "";

  // Circuits
  GetCardiovascularOpenResistance().SetValue(100.0, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
  GetDefaultOpenElectricResistance().SetValue(1E100, ElectricResistanceUnit::Ohm);
  GetDefaultOpenFlowResistance().SetValue(1E100, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  GetDefaultOpenHeatResistance().SetValue(1E100, HeatResistanceUnit::K_Per_W);
  GetDefaultClosedElectricResistance().SetValue(1E-100, ElectricResistanceUnit::Ohm);
  GetDefaultClosedFlowResistance().SetValue(1E-100, PressureTimePerVolumeUnit::Pa_s_Per_m3);
  GetDefaultClosedHeatResistance().SetValue(1E-100, HeatResistanceUnit::K_Per_W);
  GetMachineClosedResistance().SetValue(1E-3, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
  GetMachineOpenResistance().SetValue(1E3, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
  GetRespiratoryClosedResistance().SetValue(1E-3, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
  GetRespiratoryOpenResistance().SetValue(1E3, PressureTimePerVolumeUnit::cmH2O_s_Per_L);

  // Constants
  GetOxygenMetabolicConstant().SetValue(9.0);
  GetStefanBoltzmann().SetValue(5.670367E-8, PowerPerAreaTemperatureToTheFourthUnit::W_Per_m2_K4);//http://physics.nist.gov/cuu/Constants/
  GetUniversalGasConstant().SetValue(8.3144621, HeatCapacitancePerAmountUnit::J_Per_K_mol); //http://physics.nist.gov/cuu/Constants/

  // Drugs 
  m_PDEnabled = eSwitch::On;

  // Energy
  GetBodySpecificHeat().SetValue(0.83, HeatCapacitancePerMassUnit::kcal_Per_K_kg);
  GetCarbonDioxideProductionFromOxygenConsumptionConstant().SetValue(0.8);
  GetCoreTemperatureLow().SetValue(36.8, TemperatureUnit::C);
  GetCoreTemperatureHigh().SetValue(37.1, TemperatureUnit::C);
  GetDeltaCoreTemperatureLow().SetValue(1.8, TemperatureUnit::C);
  GetEnergyPerATP().SetValue(7, EnergyPerAmountUnit::kcal_Per_mol);
  GetSweatHeatTransfer().SetValue(0.20833, HeatConductanceUnit::kcal_Per_K_s);
  GetVaporizationEnergy().SetValue(2260.0, EnergyPerMassUnit::kJ_Per_kg);
  GetVaporSpecificHeat().SetValue(1.890, HeatCapacitancePerMassUnit::kJ_Per_K_kg);

  // Environment
  GetAirDensity().SetValue(1.225, MassPerVolumeUnit::kg_Per_m3);
  GetAirSpecificHeat().SetValue(1.0035, HeatCapacitancePerMassUnit::kJ_Per_K_kg);
  GetMolarMassOfDryAir().SetValue(0.028964, MassPerAmountUnit::kg_Per_mol);
  GetMolarMassOfWaterVapor().SetValue(0.018016, MassPerAmountUnit::kg_Per_mol);
  if (subMgr != nullptr)
  {
    if (FileExists(dataDir + "/environments/Standard.json"))
      GetInitialEnvironmentalConditions().SerializeFromFile(dataDir + "/environments/Standard.json", *subMgr);
    else
    {
      // This has been static for a long time, so if no files are present
      // Let's use this. This helps component engines file requirements
      Info("No environment files found, using encoded defaults");
      auto& env = GetInitialEnvironmentalConditions();
      env.SetSurroundingType(eSurroundingType::Air);
      env.GetAmbientTemperature().SetValue(22, TemperatureUnit::C);
      env.GetAtmosphericPressure().SetValue(760.0, PressureUnit::mmHg);
      env.GetClothingResistance().SetValue(0.5, HeatResistanceAreaUnit::clo);
      env.GetEmissivity().SetValue(0.95);
      env.GetMeanRadiantTemperature().SetValue(22, TemperatureUnit::C);
      env.GetRelativeHumidity().SetValue(0.6);
      env.GetRespirationAmbientTemperature().SetValue(22, TemperatureUnit::C);
      env.GetAmbientGas(*subMgr->GetSubstance("Nitrogen")).GetFractionAmount().SetValue(0.7896);
      env.GetAmbientGas(*subMgr->GetSubstance("Oxygen")).GetFractionAmount().SetValue(0.21);
      env.GetAmbientGas(*subMgr->GetSubstance("CarbonDioxide")).GetFractionAmount().SetValue(4.0E-4);
    }
  }
  GetWaterDensity().SetValue(1000, MassPerVolumeUnit::kg_Per_m3);

  // Gastrointestinal
  GetCalciumAbsorptionFraction().SetValue(0.25);// Net fractional calcium absorption is 24.9 ± 12.4% (Hunt and Johnson 2007)
  GetCalciumDigestionRate().SetValue(2.7, MassPerTimeUnit::mg_Per_min);// Wasserman1992Intestinal
  GetCarbohydrateAbsorptionFraction().SetValue(0.80);// Guyton p790
  GetDefaultCarbohydrateDigestionRate().SetValue(0.87, MassPerTimeUnit::g_Per_min);// Go through 130g in about 2 hrs, glucose levels should return to basal 2hrs after that
  GetDefaultFatDigestionRate().SetValue(0.055, MassPerTimeUnit::g_Per_min);// Guyton (About 8hr to digest the fat in the default meal)
  GetDefaultProteinDigestionRate().SetValue(0.071, MassPerTimeUnit::g_Per_min);// Dangin2001Digestion (About 5hr to digest the protein in the default meal)
  if (FileExists(dataDir+"/nutrition/Standard.json"))
    GetDefaultStomachContents().SerializeFromFile(dataDir +"/nutrition/Standard.json");// Refs are in the data spreadsheet
  GetFatAbsorptionFraction().SetValue(0.248);// Guyton p797 and the recommended daily value for saturated fat intake according to the AHA //TODO: Add this reference
  // We should be making 30 grams of urea per 100 grams of protein haussinger1990nitrogen
  GetProteinToUreaFraction().SetValue(0.405);// BUT, We should excrete 24.3 g/day on average. Guyton p 328. With an average intake of 60 g/day, that works out to approximately 40%. 
  GetWaterDigestionRate().SetValue(0.417, VolumePerTimeUnit::mL_Per_s);// Peronnet2012Pharmacokinetic, Estimated from 300mL H20 being absorbed in 9.5-12m
  
  // Nervous
  m_CerebrospinalFluidEnabled = eSwitch::Off;
  m_BaroreceptorFeedback = eSwitch::On;
  m_ChemoreceptorFeedback = eSwitch::On;
  GetHeartElastanceDistributedTimeDelay().SetValue(20.0, TimeUnit::s);
  GetHeartRateDistributedTimeDelay().SetValue(20.0, TimeUnit::s);
  GetNormalizedHeartRateIntercept().SetValue(0.26); //Gamma Heart Rate
  GetNormalizedHeartRateSympatheticSlope().SetValue(1.73); //Alpha Heart Rate
  GetNormalizedHeartRateParasympatheticSlope().SetValue(0.25); //Beta Heart Rate
  GetNormalizedHeartElastanceIntercept().SetValue(0.95); //Gamma Heart Elastance
  GetNormalizedHeartElastanceSympatheticSlope().SetValue(0.1); //Alpha Elastance
  GetNormalizedResistanceIntercept().SetValue(0.4); //Gamma Resistance
  GetNormalizedResistanceSympatheticSlope().SetValue(1.2); //Alpha Resistance
  GetNormalizedComplianceIntercept().SetValue(0.7); //Gamma Compliance
  GetNormalizedComplianceParasympatheticSlope().SetValue(0.6); //Alpha Compliance
  GetPupilDiameterBaseline().SetValue(4, LengthUnit::mm);
  GetResponseSlope().SetValue(12.0); //nu
  GetSystemicResistanceDistributedTimeDelay().SetValue(30.0, TimeUnit::s);
  GetVenousComplianceDistributedTimeDelay().SetValue(60.0, TimeUnit::s);

  // Renal
  m_RenalEnabled = eSwitch::On;
  GetPlasmaSodiumConcentrationSetPoint().SetValue(3.23, MassPerVolumeUnit::mg_Per_mL);
  GetPeritubularPotassiumConcentrationSetPoint().SetValue(0.0185, MassPerVolumeUnit::g_Per_dL);
  GetLeftGlomerularFluidPermeabilityBaseline().SetValue(3.67647, VolumePerTimePressureAreaUnit::mL_Per_min_mmHg_m2);
  GetLeftGlomerularFilteringSurfaceAreaBaseline().SetValue(2.0, AreaUnit::m2);
  GetLeftTubularReabsorptionFluidPermeabilityBaseline().SetValue(2.91747, VolumePerTimePressureAreaUnit::mL_Per_min_mmHg_m2);
  GetLeftTubularReabsorptionFilteringSurfaceAreaBaseline().SetValue(2.5, AreaUnit::m2);
  GetMaximumAfferentResistance().SetValue(11.2, PressureTimePerVolumeUnit::mmHg_s_Per_mL);  //11.2
  GetMinimumAfferentResistance().SetValue(2.2, PressureTimePerVolumeUnit::mmHg_s_Per_mL);  //2.1 
  GetRightGlomerularFluidPermeabilityBaseline().SetValue(3.67647, VolumePerTimePressureAreaUnit::mL_Per_min_mmHg_m2);
  GetRightGlomerularFilteringSurfaceAreaBaseline().SetValue(2.0, AreaUnit::m2);
  GetRightTubularReabsorptionFluidPermeabilityBaseline().SetValue(2.91747, VolumePerTimePressureAreaUnit::mL_Per_min_mmHg_m2);
  //GetRightTubularReabsorptionFluidPermeabilityBaseline().SetValue(1.0, VolumePerTimePressureAreaUnit::mL_Per_min_Per_mmHg_Per_m2);
  GetRightTubularReabsorptionFilteringSurfaceAreaBaseline().SetValue(2.5, AreaUnit::m2);
  GetTargetSodiumDelivery().SetValue(0.201, MassPerTimeUnit::g_Per_min);

  // Respiratory
  GetCentralControllerCO2PressureSetPoint().SetValue(35.5, PressureUnit::mmHg);
  GetCentralVentilatoryControllerGain().SetValue(1.44); //How much to add to the amplitude when the CO2 is off
  GetMinimumAllowableInpiratoryAndExpiratoryPeriod().SetValue(0.25, TimeUnit::s);
  GetMinimumAllowableTidalVolume().SetValue(0.1, VolumeUnit::L);
  GetPeripheralControllerCO2PressureSetPoint().SetValue(35.5, PressureUnit::mmHg);
  GetPeripheralVentilatoryControllerGain().SetValue(30.24); //How much to add to the amplitude when the CO2 is off
  GetPulmonaryVentilationRateMaximum().SetValue(150.0, VolumePerTimeUnit::L_Per_min);
  GetVentilationTidalVolumeIntercept().SetValue(0.3, VolumeUnit::L);
  GetVentilatoryOcclusionPressure().SetValue(0.75, PressureUnit::cmH2O); //This increases the absolute max driver pressure
  UseExpandedRespiratory(eSwitch::Off);

  // Tissue
  m_TissueEnabled = eSwitch::On;
}

bool PulseConfiguration::HasTimeStep() const
{
  return m_TimeStep == nullptr ? false : m_TimeStep->IsValid();
}
SEScalarTime& PulseConfiguration::GetTimeStep()
{
  if (m_TimeStep == nullptr)
    m_TimeStep = new SEScalarTime();
  return *m_TimeStep;
}
double PulseConfiguration::GetTimeStep(const TimeUnit& unit) const
{
  if (m_TimeStep == nullptr)
    return SEScalar::dNaN();
  return m_TimeStep->GetValue(unit);
}

bool PulseConfiguration::HasStabilization() const
{
  return m_TimedStabilization != nullptr || m_DynamicStabilization != nullptr;
}
SEEngineStabilization* PulseConfiguration::GetStabilization()
{
  if (m_TimedStabilization != nullptr)
    return m_TimedStabilization;
  if (m_DynamicStabilization != nullptr)
    return m_DynamicStabilization;
  return nullptr;
}
void PulseConfiguration::RemoveStabilization()
{
  SAFE_DELETE(m_TimedStabilization);
  SAFE_DELETE(m_DynamicStabilization);
}
bool PulseConfiguration::HasTimedStabilization() const
{
  return m_TimedStabilization != nullptr;
}
SETimedStabilization& PulseConfiguration::GetTimedStabilization()
{
  RemoveDynamicStabilization();
  if (m_TimedStabilization == nullptr)
    m_TimedStabilization = new SETimedStabilization(GetLogger());
  return *m_TimedStabilization;
}
const SETimedStabilization* PulseConfiguration::GetTimedStabilization() const
{
  return m_TimedStabilization;
}
void PulseConfiguration::RemoveTimedStabilization()
{
  SAFE_DELETE(m_TimedStabilization);
}
bool PulseConfiguration::HasDynamicStabilization() const
{
  return m_DynamicStabilization != nullptr;
}
SEDynamicStabilization& PulseConfiguration::GetDynamicStabilization()
{
  RemoveTimedStabilization();
  if (m_DynamicStabilization == nullptr)
    m_DynamicStabilization = new SEDynamicStabilization(GetLogger());
  return *m_DynamicStabilization;
}
const SEDynamicStabilization* PulseConfiguration::GetDynamicStabilization() const
{
  return m_DynamicStabilization;
}
void PulseConfiguration::RemoveDynamicStabilization()
{
  SAFE_DELETE(m_DynamicStabilization);
}

bool PulseConfiguration::HasOverrides() const
{
  return !m_Overrides.empty();
}
SEScalarProperties& PulseConfiguration::GetOverrides()
{
  return m_Overrides;
}
const SEScalarProperties& PulseConfiguration::GetOverrides() const
{
  return m_Overrides;
}

bool PulseConfiguration::HasModifiers() const
{
  return !m_Modifiers.empty();
}
SEScalarProperties& PulseConfiguration::GetModifiers()
{
  return m_Modifiers;
}
const SEScalarProperties& PulseConfiguration::GetModifiers() const
{
  return m_Modifiers;
}

//////////////////////
/** Blood Chemistry */
//////////////////////

bool PulseConfiguration::HasMeanCorpuscularHemoglobin() const
{
  return m_MeanCorpuscularHemoglobin == nullptr ? false : m_MeanCorpuscularHemoglobin->IsValid();
}
SEScalarMassPerAmount& PulseConfiguration::GetMeanCorpuscularHemoglobin()
{
  if (m_MeanCorpuscularHemoglobin == nullptr)
    m_MeanCorpuscularHemoglobin = new SEScalarMassPerAmount();
  return *m_MeanCorpuscularHemoglobin;
}
double PulseConfiguration::GetMeanCorpuscularHemoglobin(const MassPerAmountUnit& unit) const
{
  if (m_MeanCorpuscularHemoglobin == nullptr)
    return SEScalar::dNaN();
  return m_MeanCorpuscularHemoglobin->GetValue(unit);
}

bool PulseConfiguration::HasMeanCorpuscularVolume() const
{
  return m_MeanCorpuscularVolume == nullptr ? false : m_MeanCorpuscularVolume->IsValid();
}
SEScalarVolume& PulseConfiguration::GetMeanCorpuscularVolume()
{
  if (m_MeanCorpuscularVolume == nullptr)
    m_MeanCorpuscularVolume = new SEScalarVolume();
  return *m_MeanCorpuscularVolume;
}
double PulseConfiguration::GetMeanCorpuscularVolume(const VolumeUnit& unit) const
{
  if (m_MeanCorpuscularVolume == nullptr)
    return SEScalar::dNaN();
  return m_MeanCorpuscularVolume->GetValue(unit);
}

bool PulseConfiguration::HasStandardDiffusionDistance() const
{
  return m_StandardDiffusionDistance == nullptr ? false : m_StandardDiffusionDistance->IsValid();
}
SEScalarLength& PulseConfiguration::GetStandardDiffusionDistance()
{
  if (m_StandardDiffusionDistance == nullptr)
    m_StandardDiffusionDistance = new SEScalarLength();
  return *m_StandardDiffusionDistance;
}
double PulseConfiguration::GetStandardDiffusionDistance(const LengthUnit& unit) const
{
  if (m_StandardDiffusionDistance == nullptr)
    return SEScalar::dNaN();
  return m_StandardDiffusionDistance->GetValue(unit);
}

bool PulseConfiguration::HasStandardOxygenDiffusionCoefficient() const
{
  return m_StandardOxygenDiffusionCoefficient == nullptr ? false : m_StandardOxygenDiffusionCoefficient->IsValid();
}
SEScalarAreaPerTimePressure& PulseConfiguration::GetStandardOxygenDiffusionCoefficient()
{
  if (m_StandardOxygenDiffusionCoefficient == nullptr)
    m_StandardOxygenDiffusionCoefficient = new SEScalarAreaPerTimePressure();
  return *m_StandardOxygenDiffusionCoefficient;
}
double PulseConfiguration::GetStandardOxygenDiffusionCoefficient(const AreaPerTimePressureUnit& unit) const
{
  if (m_StandardOxygenDiffusionCoefficient == nullptr)
    return SEScalar::dNaN();
  return m_StandardOxygenDiffusionCoefficient->GetValue(unit);
}

/////////////////////
/** Cardiovascular */
/////////////////////
bool PulseConfiguration::HasLeftHeartElastanceMaximum() const
{
  return m_LeftHeartElastanceMaximum == nullptr ? false : m_LeftHeartElastanceMaximum->IsValid();
}
SEScalarPressurePerVolume& PulseConfiguration::GetLeftHeartElastanceMaximum()
{
  if (m_LeftHeartElastanceMaximum == nullptr)
    m_LeftHeartElastanceMaximum = new SEScalarPressurePerVolume();
  return *m_LeftHeartElastanceMaximum;
}
double PulseConfiguration::GetLeftHeartElastanceMaximum(const PressurePerVolumeUnit& unit) const
{
  if (m_LeftHeartElastanceMaximum == nullptr)
    return SEScalar::dNaN();
  return m_LeftHeartElastanceMaximum->GetValue(unit);
}

bool PulseConfiguration::HasLeftHeartElastanceMinimum() const
{
  return m_LeftHeartElastanceMinimum == nullptr ? false : m_LeftHeartElastanceMinimum->IsValid();
}
SEScalarPressurePerVolume& PulseConfiguration::GetLeftHeartElastanceMinimum()
{
  if (m_LeftHeartElastanceMinimum == nullptr)
    m_LeftHeartElastanceMinimum = new SEScalarPressurePerVolume();
  return *m_LeftHeartElastanceMinimum;
}
double PulseConfiguration::GetLeftHeartElastanceMinimum(const PressurePerVolumeUnit& unit) const
{
  if (m_LeftHeartElastanceMinimum == nullptr)
    return SEScalar::dNaN();
  return m_LeftHeartElastanceMinimum->GetValue(unit);
}

void PulseConfiguration::UseExpandedVasculature(eSwitch s)
{
  if (s == eSwitch::NullSwitch)
    return;
  m_UseExpandedVasculature = s;
  // Circuit Modifiers
  if (m_UseExpandedVasculature == eSwitch::On)
  {
    // Circuit Modifiers
    m_Modifiers[pulse::ExpandedCardiovascularPath::Aorta3ToAorta1] =                                               SEScalarPair(1.2902894293208438);// 1.12
    m_Modifiers[pulse::ExpandedCardiovascularPath::LeftArm1ToLeftArmBone1] =                                       SEScalarPair(1.0923362183174377);// 1.15
    m_Modifiers[pulse::ExpandedCardiovascularPath::LeftArm1ToLeftArmFat1] =                                        SEScalarPair(1.0923362183174377);// 1.15
    m_Modifiers[pulse::ExpandedCardiovascularPath::LeftArm1ToLeftArmMicrovasculature1] =                           SEScalarPair(1.0923362183174377);// 1.15
    m_Modifiers[pulse::ExpandedCardiovascularPath::LeftArm1ToLeftArmMuscle1] =                                     SEScalarPair(1.0923362183174377);// 1.15
    m_Modifiers[pulse::ExpandedCardiovascularPath::LeftArm1ToLeftArmSkin1] =                                       SEScalarPair(1.0923362183174377);// 1.15
    m_Modifiers[pulse::ExpandedCardiovascularPath::RightArm1ToRightArmFat1] =                                      SEScalarPair(1.0923362199627225);// 1.15
    m_Modifiers[pulse::ExpandedCardiovascularPath::RightArm1ToRightArmMuscle1] =                                   SEScalarPair(1.0923362199627225);// 1.15
    m_Modifiers[pulse::ExpandedCardiovascularPath::RightArm1ToRightArmSkin1] =                                     SEScalarPair(1.0923362199627225);// 1.15
    m_Modifiers[pulse::ExpandedCardiovascularPath::RightArm1ToRightArmBone1] =                                     SEScalarPair(1.0923362199627225);// 1.15
    m_Modifiers[pulse::ExpandedCardiovascularPath::RightArm1ToRightArmMicrovasculature1] =                         SEScalarPair(1.0923362199627225);// 1.15
    m_Modifiers[pulse::ExpandedCardiovascularPath::Extracranial1ToExtracranial2] =                                 SEScalarPair(0.99455963021629035);// 1.0
    m_Modifiers[pulse::ExpandedCardiovascularPath::Intracranial1ToIntracranial2] =                                 SEScalarPair(0.99455963021629035);// 1.0
    m_Modifiers[pulse::ExpandedCardiovascularPath::VenaCava1ToRightHeart2] =                                       SEScalarPair(0.009);
    m_Modifiers[pulse::ExpandedCardiovascularPath::LeftKidney1ToLeftKidney2] =                                     SEScalarPair(1.5);
    m_Modifiers[pulse::ExpandedCardiovascularPath::RightKidney1ToRightKidney2] =                                   SEScalarPair(1.5);
    m_Modifiers[pulse::ExpandedCardiovascularPath::Gut1ToLargeIntestine1] =                                        SEScalarPair(0.98471147698642625);// 1.05
    m_Modifiers[pulse::ExpandedCardiovascularPath::LeftLeg1ToLeftLegBone1] =                                       SEScalarPair(1.055513327833395);// 1.1
    m_Modifiers[pulse::ExpandedCardiovascularPath::LeftLeg1ToLeftLegFat1] =                                        SEScalarPair(1.055513327833395);// 1.1
    m_Modifiers[pulse::ExpandedCardiovascularPath::LeftLeg1ToLeftLegMicrovasculature1] =                           SEScalarPair(1.055513327833395);// 1.1
    m_Modifiers[pulse::ExpandedCardiovascularPath::LeftLeg1ToLeftLegMuscle1] =                                     SEScalarPair(1.055513327833395);// 1.1
    m_Modifiers[pulse::ExpandedCardiovascularPath::LeftLeg1ToLeftLegSkin1] =                                       SEScalarPair(1.055513327833395);// 1.1
    m_Modifiers[pulse::ExpandedCardiovascularPath::RightLeg1ToRightLegBone1] =                                     SEScalarPair(1.0555133289980438);// 1.1
    m_Modifiers[pulse::ExpandedCardiovascularPath::RightLeg1ToRightLegFat1] =                                      SEScalarPair(1.0555133289980438);// 1.1
    m_Modifiers[pulse::ExpandedCardiovascularPath::RightLeg1ToRightLegMicrovasculature1] =                         SEScalarPair(1.0555133289980438);// 1.1
    m_Modifiers[pulse::ExpandedCardiovascularPath::RightLeg1ToRightLegMuscle1] =                                   SEScalarPair(1.0555133289980438);// 1.1
    m_Modifiers[pulse::ExpandedCardiovascularPath::RightLeg1ToRightLegSkin1] =                                     SEScalarPair(1.0555133289980438);// 1.1
    m_Modifiers[pulse::ExpandedCardiovascularPath::Gut1ToLiver1] =                                                 SEScalarPair(1.5008836959051552);// 1.1
    m_Modifiers[pulse::ExpandedCardiovascularPath::Myocardium1ToMyocardium2] =                                     SEScalarPair(0.9623041322717929);// 0.95
    m_Modifiers[pulse::ExpandedCardiovascularPath::RightPulmonaryArteries1ToRightPulmonaryCapillaries1] =          SEScalarPair(0.79184301394230017);// 1.0
    m_Modifiers[pulse::ExpandedCardiovascularPath::RightPulmonaryCapillaries1ToRightPulmonaryVeins1] =             SEScalarPair(1.0917195273914668);// 1.0
    m_Modifiers[pulse::ExpandedCardiovascularPath::RightIntermediatePulmonaryArteries1ToRightPulmonaryArteries1] = SEScalarPair(1.0);
    m_Modifiers[pulse::ExpandedCardiovascularPath::LeftIntermediatePulmonaryArteries1ToLeftPulmonaryArteries1] =   SEScalarPair(1.0);
    m_Modifiers[pulse::ExpandedCardiovascularPath::LeftPulmonaryArteries1ToLeftPulmonaryCapillaries1] =            SEScalarPair(0.84990272641328168);// 1.0
    m_Modifiers[pulse::ExpandedCardiovascularPath::LeftPulmonaryCapillaries1ToLeftPulmonaryVeins1] =               SEScalarPair(1.0802879539709094);// 1.0
    m_Modifiers[pulse::ExpandedCardiovascularPath::Gut1ToSmallIntestine1] =                                        SEScalarPair(0.97271586393584319);// 1.14
    m_Modifiers[pulse::ExpandedCardiovascularPath::Gut1ToSplanchnic1] =                                            SEScalarPair(0.98750749786051784);// 0.95
    m_Modifiers[pulse::ExpandedCardiovascularPath::Gut1ToSpleen1] =                                                SEScalarPair(0.96800874795093717);// 0.95
    m_Modifiers[pulse::ExpandedCardiovascularPath::Torso1ToTorsoBone1] =                                           SEScalarPair(1.0265141845356056);// 1.02
    m_Modifiers[pulse::ExpandedCardiovascularPath::Torso1ToTorsoFat1] =                                            SEScalarPair(1.0154101337135821);// 1.02
    m_Modifiers[pulse::ExpandedCardiovascularPath::Torso1ToTorsoMuscle1] =                                         SEScalarPair(1.0968290345983671);// 1.15
    m_Modifiers[pulse::ExpandedCardiovascularPath::TorsoMuscle1ToTorso2] =                                         SEScalarPair(0.81318313089396543);// 1.0
    m_Modifiers[pulse::ExpandedCardiovascularPath::Torso1ToTorsoSkin1] =                                           SEScalarPair(0.99621251983966808);// 1.0
    m_Modifiers[pulse::ExpandedCardiovascularPath::VenaCava1ToGround] =                                            SEScalarPair(0.9);
  }
  else
  {
    m_Modifiers[pulse::CardiovascularPath::Aorta3ToAorta1] =                                                       SEScalarPair(1.12);
    m_Modifiers[pulse::CardiovascularPath::Aorta1ToLeftArm1] =                                                     SEScalarPair(1.15);
    m_Modifiers[pulse::CardiovascularPath::Aorta1ToRightArm1] =                                                    SEScalarPair(1.15);
    m_Modifiers[pulse::CardiovascularPath::Aorta1ToBone1] =                                                        SEScalarPair(1.02);
    m_Modifiers[pulse::CardiovascularPath::Aorta1ToBrain1] =                                                       SEScalarPair(1.0);
    m_Modifiers[pulse::CardiovascularPath::Aorta1ToFat1] =                                                         SEScalarPair(1.02);
    m_Modifiers[pulse::CardiovascularPath::VenaCava1ToRightHeart2] =                                               SEScalarPair(0.009);
    m_Modifiers[pulse::CardiovascularPath::Aorta1ToLeftKidney1] =                                                  SEScalarPair(1.5);
    m_Modifiers[pulse::CardiovascularPath::Aorta1ToRightKidney1] =                                                 SEScalarPair(1.5);
    m_Modifiers[pulse::CardiovascularPath::Aorta1ToLargeIntestine1] =                                              SEScalarPair(1.05);
    m_Modifiers[pulse::CardiovascularPath::Aorta1ToLeftLeg1] =                                                     SEScalarPair(1.1);
    m_Modifiers[pulse::CardiovascularPath::Aorta1ToRightLeg1] =                                                    SEScalarPair(1.1);
    m_Modifiers[pulse::CardiovascularPath::Aorta1ToLiver1] =                                                       SEScalarPair(1.1);
    m_Modifiers[pulse::CardiovascularPath::Aorta1ToMuscle1] =                                                      SEScalarPair(1.15);
    m_Modifiers[pulse::CardiovascularPath::Aorta1ToMyocardium1] =                                                  SEScalarPair(0.95);
    m_Modifiers[pulse::CardiovascularPath::RightPulmonaryArteries1ToRightPulmonaryCapillaries1] =                  SEScalarPair(1.0);
    m_Modifiers[pulse::CardiovascularPath::RightPulmonaryCapillaries1ToRightPulmonaryVeins1] =                     SEScalarPair(1.0);
    m_Modifiers[pulse::CardiovascularPath::RightIntermediatePulmonaryArteries1ToRightPulmonaryArteries1] =         SEScalarPair(1.0);
    m_Modifiers[pulse::CardiovascularPath::LeftIntermediatePulmonaryArteries1ToLeftPulmonaryArteries1] =           SEScalarPair(1.0);
    m_Modifiers[pulse::CardiovascularPath::LeftPulmonaryArteries1ToLeftPulmonaryCapillaries1] =                    SEScalarPair(1.0);
    m_Modifiers[pulse::CardiovascularPath::LeftPulmonaryCapillaries1ToLeftPulmonaryVeins1] =                       SEScalarPair(1.0);
    m_Modifiers[pulse::CardiovascularPath::Aorta1ToSkin1] =                                                        SEScalarPair(1.0);
    m_Modifiers[pulse::CardiovascularPath::Aorta1ToSmallIntestine1] =                                              SEScalarPair(1.14);
    m_Modifiers[pulse::CardiovascularPath::Aorta1ToSplanchnic1] =                                                  SEScalarPair(0.95);
    m_Modifiers[pulse::CardiovascularPath::Aorta1ToSpleen1] =                                                      SEScalarPair(0.95);
    m_Modifiers[pulse::CardiovascularPath::VenaCava1ToGround] =                                                    SEScalarPair(0.9);
  }
}

bool PulseConfiguration::HasMinimumBloodVolumeFraction() const
{
  return m_MinimumBloodVolumeFraction == nullptr ? false : m_MinimumBloodVolumeFraction->IsValid();
}
SEScalar0To1& PulseConfiguration::GetMinimumBloodVolumeFraction()
{
  if (m_MinimumBloodVolumeFraction == nullptr)
    m_MinimumBloodVolumeFraction = new SEScalar0To1();
  return *m_MinimumBloodVolumeFraction;
}
double PulseConfiguration::GetMinimumBloodVolumeFraction() const
{
  if (m_MinimumBloodVolumeFraction == nullptr)
    return SEScalar::dNaN();
  return m_MinimumBloodVolumeFraction->GetValue();
}

bool PulseConfiguration::HasRightHeartElastanceMaximum() const
{
  return m_RightHeartElastanceMaximum == nullptr ? false : m_RightHeartElastanceMaximum->IsValid();
}
SEScalarPressurePerVolume& PulseConfiguration::GetRightHeartElastanceMaximum()
{
  if (m_RightHeartElastanceMaximum == nullptr)
    m_RightHeartElastanceMaximum = new SEScalarPressurePerVolume();
  return *m_RightHeartElastanceMaximum;
}
double PulseConfiguration::GetRightHeartElastanceMaximum(const PressurePerVolumeUnit& unit) const
{
  if (m_RightHeartElastanceMaximum == nullptr)
    return SEScalar::dNaN();
  return m_RightHeartElastanceMaximum->GetValue(unit);
}

bool PulseConfiguration::HasRightHeartElastanceMinimum() const
{
  return m_RightHeartElastanceMinimum == nullptr ? false : m_RightHeartElastanceMinimum->IsValid();
}
SEScalarPressurePerVolume& PulseConfiguration::GetRightHeartElastanceMinimum()
{
  if (m_RightHeartElastanceMinimum == nullptr)
    m_RightHeartElastanceMinimum = new SEScalarPressurePerVolume();
  return *m_RightHeartElastanceMinimum;
}
double PulseConfiguration::GetRightHeartElastanceMinimum(const PressurePerVolumeUnit& unit) const
{
  if (m_RightHeartElastanceMinimum == nullptr)
    return SEScalar::dNaN();
  return m_RightHeartElastanceMinimum->GetValue(unit);
}

bool PulseConfiguration::HasStandardPulmonaryCapillaryCoverage() const
{
  return m_StandardPulmonaryCapillaryCoverage == nullptr ? false : m_StandardPulmonaryCapillaryCoverage->IsValid();
}
SEScalar& PulseConfiguration::GetStandardPulmonaryCapillaryCoverage()
{
  if (m_StandardPulmonaryCapillaryCoverage == nullptr)
    m_StandardPulmonaryCapillaryCoverage = new SEScalar();
  return *m_StandardPulmonaryCapillaryCoverage;
}
double PulseConfiguration::GetStandardPulmonaryCapillaryCoverage() const
{
  if (m_StandardPulmonaryCapillaryCoverage == nullptr)
    return SEScalar::dNaN();
  return m_StandardPulmonaryCapillaryCoverage->GetValue();
}

//////////////
/** Circuit */
//////////////

bool PulseConfiguration::HasCardiovascularOpenResistance() const
{
  return m_CardiovascularOpenResistance == nullptr ? false : m_CardiovascularOpenResistance->IsValid();
}
SEScalarPressureTimePerVolume& PulseConfiguration::GetCardiovascularOpenResistance()
{
  if (m_CardiovascularOpenResistance == nullptr)
    m_CardiovascularOpenResistance = new SEScalarPressureTimePerVolume();
  return *m_CardiovascularOpenResistance;
}
double PulseConfiguration::GetCardiovascularOpenResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_CardiovascularOpenResistance == nullptr)
    return SEScalar::dNaN();
  return m_CardiovascularOpenResistance->GetValue(unit);
}

bool PulseConfiguration::HasDefaultClosedElectricResistance() const
{
  return m_DefaultClosedElectricResistance == nullptr ? false : m_DefaultClosedElectricResistance->IsValid();
}
SEScalarElectricResistance& PulseConfiguration::GetDefaultClosedElectricResistance()
{
  if (m_DefaultClosedElectricResistance == nullptr)
    m_DefaultClosedElectricResistance = new SEScalarElectricResistance();
  return *m_DefaultClosedElectricResistance;
}
double PulseConfiguration::GetDefaultClosedElectricResistance(const ElectricResistanceUnit& unit) const
{
  if (m_DefaultClosedElectricResistance == nullptr)
    return SEScalar::dNaN();
  return m_DefaultClosedElectricResistance->GetValue(unit);
}

bool PulseConfiguration::HasDefaultClosedFlowResistance() const
{
  return m_DefaultClosedFlowResistance == nullptr ? false : m_DefaultClosedFlowResistance->IsValid();
}
SEScalarPressureTimePerVolume& PulseConfiguration::GetDefaultClosedFlowResistance()
{
  if (m_DefaultClosedFlowResistance == nullptr)
    m_DefaultClosedFlowResistance = new SEScalarPressureTimePerVolume();
  return *m_DefaultClosedFlowResistance;
}
double PulseConfiguration::GetDefaultClosedFlowResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_DefaultClosedFlowResistance == nullptr)
    return SEScalar::dNaN();
  return m_DefaultClosedFlowResistance->GetValue(unit);
}

bool PulseConfiguration::HasDefaultClosedHeatResistance() const
{
  return m_DefaultClosedHeatResistance == nullptr ? false : m_DefaultClosedHeatResistance->IsValid();
}
SEScalarHeatResistance& PulseConfiguration::GetDefaultClosedHeatResistance()
{
  if (m_DefaultClosedHeatResistance == nullptr)
    m_DefaultClosedHeatResistance = new SEScalarHeatResistance();
  return *m_DefaultClosedHeatResistance;
}
double PulseConfiguration::GetDefaultClosedHeatResistance(const HeatResistanceUnit& unit) const
{
  if (m_DefaultClosedHeatResistance == nullptr)
    return SEScalar::dNaN();
  return m_DefaultClosedHeatResistance->GetValue(unit);
}

bool PulseConfiguration::HasDefaultOpenElectricResistance() const
{
  return m_DefaultOpenElectricResistance == nullptr ? false : m_DefaultOpenElectricResistance->IsValid();
}
SEScalarElectricResistance& PulseConfiguration::GetDefaultOpenElectricResistance()
{
  if (m_DefaultOpenElectricResistance == nullptr)
    m_DefaultOpenElectricResistance = new SEScalarElectricResistance();
  return *m_DefaultOpenElectricResistance;
}
double PulseConfiguration::GetDefaultOpenElectricResistance(const ElectricResistanceUnit& unit) const
{
  if (m_DefaultOpenElectricResistance == nullptr)
    return SEScalar::dNaN();
  return m_DefaultOpenElectricResistance->GetValue(unit);
}

bool PulseConfiguration::HasDefaultOpenFlowResistance() const
{
  return m_DefaultOpenFlowResistance == nullptr ? false : m_DefaultOpenFlowResistance->IsValid();
}
SEScalarPressureTimePerVolume& PulseConfiguration::GetDefaultOpenFlowResistance()
{
  if (m_DefaultOpenFlowResistance == nullptr)
    m_DefaultOpenFlowResistance = new SEScalarPressureTimePerVolume();
  return *m_DefaultOpenFlowResistance;
}
double PulseConfiguration::GetDefaultOpenFlowResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_DefaultOpenFlowResistance == nullptr)
    return SEScalar::dNaN();
  return m_DefaultOpenFlowResistance->GetValue(unit);
}

bool PulseConfiguration::HasDefaultOpenHeatResistance() const
{
  return m_DefaultOpenHeatResistance == nullptr ? false : m_DefaultOpenHeatResistance->IsValid();
}
SEScalarHeatResistance& PulseConfiguration::GetDefaultOpenHeatResistance()
{
  if (m_DefaultOpenHeatResistance == nullptr)
    m_DefaultOpenHeatResistance = new SEScalarHeatResistance();
  return *m_DefaultOpenHeatResistance;
}
double PulseConfiguration::GetDefaultOpenHeatResistance(const HeatResistanceUnit& unit) const
{
  if (m_DefaultOpenHeatResistance == nullptr)
    return SEScalar::dNaN();
  return m_DefaultOpenHeatResistance->GetValue(unit);
}

bool PulseConfiguration::HasMachineClosedResistance() const
{
  return m_MachineClosedResistance == nullptr ? false : m_MachineClosedResistance->IsValid();
}
SEScalarPressureTimePerVolume& PulseConfiguration::GetMachineClosedResistance()
{
  if (m_MachineClosedResistance == nullptr)
    m_MachineClosedResistance = new SEScalarPressureTimePerVolume();
  return *m_MachineClosedResistance;
}
double PulseConfiguration::GetMachineClosedResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_MachineClosedResistance == nullptr)
    return SEScalar::dNaN();
  return m_MachineClosedResistance->GetValue(unit);
}

bool PulseConfiguration::HasMachineOpenResistance() const
{
  return m_MachineOpenResistance == nullptr ? false : m_MachineOpenResistance->IsValid();
}
SEScalarPressureTimePerVolume& PulseConfiguration::GetMachineOpenResistance()
{
  if (m_MachineOpenResistance == nullptr)
    m_MachineOpenResistance = new SEScalarPressureTimePerVolume();
  return *m_MachineOpenResistance;
}
double PulseConfiguration::GetMachineOpenResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_MachineOpenResistance == nullptr)
    return SEScalar::dNaN();
  return m_MachineOpenResistance->GetValue(unit);
}

bool PulseConfiguration::HasRespiratoryClosedResistance() const
{
  return m_RespiratoryClosedResistance == nullptr ? false : m_RespiratoryClosedResistance->IsValid();
}
SEScalarPressureTimePerVolume& PulseConfiguration::GetRespiratoryClosedResistance()
{
  if (m_RespiratoryClosedResistance == nullptr)
    m_RespiratoryClosedResistance = new SEScalarPressureTimePerVolume();
  return *m_RespiratoryClosedResistance;
}
double PulseConfiguration::GetRespiratoryClosedResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_RespiratoryClosedResistance == nullptr)
    return SEScalar::dNaN();
  return m_RespiratoryClosedResistance->GetValue(unit);
}

bool PulseConfiguration::HasRespiratoryOpenResistance() const
{
  return m_RespiratoryOpenResistance == nullptr ? false : m_RespiratoryOpenResistance->IsValid();
}
SEScalarPressureTimePerVolume& PulseConfiguration::GetRespiratoryOpenResistance()
{
  if (m_RespiratoryOpenResistance == nullptr)
    m_RespiratoryOpenResistance = new SEScalarPressureTimePerVolume();
  return *m_RespiratoryOpenResistance;
}
double PulseConfiguration::GetRespiratoryOpenResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_RespiratoryOpenResistance == nullptr)
    return SEScalar::dNaN();
  return m_RespiratoryOpenResistance->GetValue(unit);
}

////////////////
/** Constants */
////////////////
bool PulseConfiguration::HasOxygenMetabolicConstant() const
{
  return m_OxygenMetabolicConstant == nullptr ? false : m_OxygenMetabolicConstant->IsValid();
}
SEScalar& PulseConfiguration::GetOxygenMetabolicConstant()
{
  if (m_OxygenMetabolicConstant == nullptr)
    m_OxygenMetabolicConstant = new SEScalar();
  return *m_OxygenMetabolicConstant;
}
double PulseConfiguration::GetOxygenMetabolicConstant() const
{
  if (m_OxygenMetabolicConstant == nullptr)
    return SEScalar::dNaN();
  return m_OxygenMetabolicConstant->GetValue();
}

bool PulseConfiguration::HasStefanBoltzmann() const
{
  return m_StefanBoltzmann == nullptr ? false : m_StefanBoltzmann->IsValid();
}
SEScalarPowerPerAreaTemperatureToTheFourth& PulseConfiguration::GetStefanBoltzmann()
{
  if (m_StefanBoltzmann == nullptr)
    m_StefanBoltzmann = new SEScalarPowerPerAreaTemperatureToTheFourth();
  return *m_StefanBoltzmann;
}
double PulseConfiguration::GetStefanBoltzmann(const PowerPerAreaTemperatureToTheFourthUnit& unit) const
{
  if (m_StefanBoltzmann == nullptr)
    return SEScalar::dNaN();
  return m_StefanBoltzmann->GetValue(unit);
}

bool PulseConfiguration::HasUniversalGasConstant() const
{
  return m_UniversalGasConstant == nullptr ? false : m_UniversalGasConstant->IsValid();
}
SEScalarHeatCapacitancePerAmount& PulseConfiguration::GetUniversalGasConstant()
{
  if (m_UniversalGasConstant == nullptr)
    m_UniversalGasConstant = new SEScalarHeatCapacitancePerAmount();
  return *m_UniversalGasConstant;
}
double PulseConfiguration::GetUniversalGasConstant(const HeatCapacitancePerAmountUnit& unit) const
{
  if (m_UniversalGasConstant == nullptr)
    return SEScalar::dNaN();
  return m_UniversalGasConstant->GetValue(unit);
}

/////////
// ECG //
/////////
bool PulseConfiguration::HasECG() const
{
  return m_ECG != nullptr;
}
SEElectroCardioGram& PulseConfiguration::GetECG()
{
  if (m_ECG == nullptr)
    m_ECG = new SEElectroCardioGram(GetLogger());
  return *m_ECG;
}
const SEElectroCardioGram* PulseConfiguration::GetECG() const
{
  return m_ECG;
}
void PulseConfiguration::RemoveECG()
{
  SAFE_DELETE(m_ECG);
}

/////////////
/** Energy */
/////////////
bool PulseConfiguration::HasBodySpecificHeat() const
{
  return m_BodySpecificHeat == nullptr ? false : m_BodySpecificHeat->IsValid();
}
SEScalarHeatCapacitancePerMass& PulseConfiguration::GetBodySpecificHeat()
{
  if (m_BodySpecificHeat == nullptr)
    m_BodySpecificHeat = new SEScalarHeatCapacitancePerMass();
  return *m_BodySpecificHeat;
}
double PulseConfiguration::GetBodySpecificHeat(const HeatCapacitancePerMassUnit& unit) const
{
  if (m_BodySpecificHeat == nullptr)
    return SEScalar::dNaN();
  return m_BodySpecificHeat->GetValue(unit);
}

bool PulseConfiguration::HasCarbonDioxideProductionFromOxygenConsumptionConstant() const
{
  return m_CarbonDioxideProductionFromOxygenConsumptionConstant == nullptr ? false : m_CarbonDioxideProductionFromOxygenConsumptionConstant->IsValid();
}
SEScalar& PulseConfiguration::GetCarbonDioxideProductionFromOxygenConsumptionConstant()
{
  if (m_CarbonDioxideProductionFromOxygenConsumptionConstant == nullptr)
    m_CarbonDioxideProductionFromOxygenConsumptionConstant = new SEScalar();
  return *m_CarbonDioxideProductionFromOxygenConsumptionConstant;
}
double PulseConfiguration::GetCarbonDioxideProductionFromOxygenConsumptionConstant() const
{
  if (m_CarbonDioxideProductionFromOxygenConsumptionConstant == nullptr)
    return SEScalar::dNaN();
  return m_CarbonDioxideProductionFromOxygenConsumptionConstant->GetValue();
}

bool PulseConfiguration::HasCoreTemperatureHigh() const
{
  return m_CoreTemperatureHigh == nullptr ? false : m_CoreTemperatureHigh->IsValid();
}
SEScalarTemperature& PulseConfiguration::GetCoreTemperatureHigh()
{
  if (m_CoreTemperatureHigh == nullptr)
    m_CoreTemperatureHigh = new SEScalarTemperature();
  return *m_CoreTemperatureHigh;
}
double PulseConfiguration::GetCoreTemperatureHigh(const TemperatureUnit& unit) const
{
  if (m_CoreTemperatureHigh == nullptr)
    return SEScalar::dNaN();
  return m_CoreTemperatureHigh->GetValue(unit);
}

bool PulseConfiguration::HasCoreTemperatureLow() const
{
  return m_CoreTemperatureLow == nullptr ? false : m_CoreTemperatureLow->IsValid();
}
SEScalarTemperature& PulseConfiguration::GetCoreTemperatureLow()
{
  if (m_CoreTemperatureLow == nullptr)
    m_CoreTemperatureLow = new SEScalarTemperature();
  return *m_CoreTemperatureLow;
}
double PulseConfiguration::GetCoreTemperatureLow(const TemperatureUnit& unit) const
{
  if (m_CoreTemperatureLow == nullptr)
    return SEScalar::dNaN();
  return m_CoreTemperatureLow->GetValue(unit);
}

bool PulseConfiguration::HasDeltaCoreTemperatureLow() const
{
  return m_DeltaCoreTemperatureLow == nullptr ? false : m_DeltaCoreTemperatureLow->IsValid();
}
SEScalarTemperature& PulseConfiguration::GetDeltaCoreTemperatureLow()
{
  if (m_DeltaCoreTemperatureLow == nullptr)
    m_DeltaCoreTemperatureLow = new SEScalarTemperature();
  return *m_DeltaCoreTemperatureLow;
}
double PulseConfiguration::GetDeltaCoreTemperatureLow(const TemperatureUnit& unit) const
{
  if (m_DeltaCoreTemperatureLow == nullptr)
    return SEScalar::dNaN();
  return m_DeltaCoreTemperatureLow->GetValue(unit);
}

bool PulseConfiguration::HasEnergyPerATP() const
{
  return m_EnergyPerATP == nullptr ? false : m_EnergyPerATP->IsValid();
}
SEScalarEnergyPerAmount& PulseConfiguration::GetEnergyPerATP()
{
  if (m_EnergyPerATP == nullptr)
    m_EnergyPerATP = new SEScalarEnergyPerAmount();
  return *m_EnergyPerATP;
}
double PulseConfiguration::GetEnergyPerATP(const EnergyPerAmountUnit& unit) const
{
  if (m_EnergyPerATP == nullptr)
    return SEScalar::dNaN();
  return m_EnergyPerATP->GetValue(unit);
}

bool PulseConfiguration::HasSweatHeatTransfer() const
{
  return m_SweatHeatTransfer == nullptr ? false : m_SweatHeatTransfer->IsValid();
}
SEScalarHeatConductance& PulseConfiguration::GetSweatHeatTransfer()
{
  if (m_SweatHeatTransfer == nullptr)
    m_SweatHeatTransfer = new SEScalarHeatConductance();
  return *m_SweatHeatTransfer;
}
double PulseConfiguration::GetSweatHeatTransfer(const HeatConductanceUnit& unit) const
{
  if (m_SweatHeatTransfer == nullptr)
    return SEScalar::dNaN();
  return m_SweatHeatTransfer->GetValue(unit);
}

bool PulseConfiguration::HasVaporizationEnergy() const
{
  return m_VaporizationEnergy == nullptr ? false : m_VaporizationEnergy->IsValid();
}
SEScalarEnergyPerMass& PulseConfiguration::GetVaporizationEnergy()
{
  if (m_VaporizationEnergy == nullptr)
    m_VaporizationEnergy = new SEScalarEnergyPerMass();
  return *m_VaporizationEnergy;
}
double PulseConfiguration::GetVaporizationEnergy(const EnergyPerMassUnit& unit) const
{
  if (m_VaporizationEnergy == nullptr)
    return SEScalar::dNaN();
  return m_VaporizationEnergy->GetValue(unit);
}

bool PulseConfiguration::HasVaporSpecificHeat() const
{
  return m_VaporSpecificHeat == nullptr ? false : m_VaporSpecificHeat->IsValid();
}
SEScalarHeatCapacitancePerMass& PulseConfiguration::GetVaporSpecificHeat()
{
  if (m_VaporSpecificHeat == nullptr)
    m_VaporSpecificHeat = new SEScalarHeatCapacitancePerMass();
  return *m_VaporSpecificHeat;
}
double PulseConfiguration::GetVaporSpecificHeat(const HeatCapacitancePerMassUnit& unit) const
{
  if (m_VaporSpecificHeat == nullptr)
    return SEScalar::dNaN();
  return m_VaporSpecificHeat->GetValue(unit);
}

//////////////////
/** Environment */
//////////////////
bool PulseConfiguration::HasAirDensity() const
{
  return m_AirDensity == nullptr ? false : m_AirDensity->IsValid();
}
SEScalarMassPerVolume& PulseConfiguration::GetAirDensity()
{
  if (m_AirDensity == nullptr)
    m_AirDensity = new SEScalarMassPerVolume();
  return *m_AirDensity;
}
double PulseConfiguration::GetAirDensity(const MassPerVolumeUnit& unit) const
{
  if (m_AirDensity == nullptr)
    return SEScalar::dNaN();
  return m_AirDensity->GetValue(unit);
}

bool PulseConfiguration::HasAirSpecificHeat() const
{
  return m_AirSpecificHeat == nullptr ? false : m_AirSpecificHeat->IsValid();
}
SEScalarHeatCapacitancePerMass& PulseConfiguration::GetAirSpecificHeat()
{
  if (m_AirSpecificHeat == nullptr)
    m_AirSpecificHeat = new SEScalarHeatCapacitancePerMass();
  return *m_AirSpecificHeat;
}
double PulseConfiguration::GetAirSpecificHeat(const HeatCapacitancePerMassUnit& unit) const
{
  if (m_AirSpecificHeat == nullptr)
    return SEScalar::dNaN();
  return m_AirSpecificHeat->GetValue(unit);
}

bool PulseConfiguration::HasMolarMassOfDryAir() const
{
  return m_MolarMassOfDryAir == nullptr ? false : m_MolarMassOfDryAir->IsValid();
}
SEScalarMassPerAmount& PulseConfiguration::GetMolarMassOfDryAir()
{
  if (m_MolarMassOfDryAir == nullptr)
    m_MolarMassOfDryAir = new SEScalarMassPerAmount();
  return *m_MolarMassOfDryAir;
}
double PulseConfiguration::GetMolarMassOfDryAir(const MassPerAmountUnit& unit) const
{
  if (m_MolarMassOfDryAir == nullptr)
    return SEScalar::dNaN();
  return m_MolarMassOfDryAir->GetValue(unit);
}

bool PulseConfiguration::HasMolarMassOfWaterVapor() const
{
  return m_MolarMassOfWaterVapor == nullptr ? false : m_MolarMassOfWaterVapor->IsValid();
}
SEScalarMassPerAmount& PulseConfiguration::GetMolarMassOfWaterVapor()
{
  if (m_MolarMassOfWaterVapor == nullptr)
    m_MolarMassOfWaterVapor = new SEScalarMassPerAmount();
  return *m_MolarMassOfWaterVapor;
}
double PulseConfiguration::GetMolarMassOfWaterVapor(const MassPerAmountUnit& unit) const
{
  if (m_MolarMassOfWaterVapor == nullptr)
    return SEScalar::dNaN();
  return m_MolarMassOfWaterVapor->GetValue(unit);
}

bool PulseConfiguration::HasInitialEnvironmentalConditions() const
{
  return m_InitialEnvironmentalConditions != nullptr;
}
SEEnvironmentalConditions& PulseConfiguration::GetInitialEnvironmentalConditions()
{
  if (m_InitialEnvironmentalConditions == nullptr)
    m_InitialEnvironmentalConditions = new SEEnvironmentalConditions(GetLogger());
  return *m_InitialEnvironmentalConditions;
}
const SEEnvironmentalConditions* PulseConfiguration::GetInitialEnvironmentalConditions() const
{
  return m_InitialEnvironmentalConditions;
}

bool PulseConfiguration::HasWaterDensity() const
{
  return m_WaterDensity == nullptr ? false : m_WaterDensity->IsValid();
}
SEScalarMassPerVolume& PulseConfiguration::GetWaterDensity()
{
  if (m_WaterDensity == nullptr)
    m_WaterDensity = new SEScalarMassPerVolume();
  return *m_WaterDensity;
}
double PulseConfiguration::GetWaterDensity(const MassPerVolumeUnit& unit) const
{
  if (m_WaterDensity == nullptr)
    return SEScalar::dNaN();
  return m_WaterDensity->GetValue(unit);
}

///////////////////////
/** Gastrointestinal */
///////////////////////

bool PulseConfiguration::HasCalciumDigestionRate() const
{
  return m_CalciumDigestionRate == nullptr ? false : m_CalciumDigestionRate->IsValid();
}
SEScalarMassPerTime& PulseConfiguration::GetCalciumDigestionRate()
{
  if (m_CalciumDigestionRate == nullptr)
    m_CalciumDigestionRate = new SEScalarMassPerTime();
  return *m_CalciumDigestionRate;
}
double PulseConfiguration::GetCalciumDigestionRate(const MassPerTimeUnit& unit) const
{
  if (m_CalciumDigestionRate == nullptr)
    return SEScalar::dNaN();
  return m_CalciumDigestionRate->GetValue(unit);
}

bool PulseConfiguration::HasCalciumAbsorptionFraction() const
{
  return m_CalciumAbsorptionFraction == nullptr ? false : m_CalciumAbsorptionFraction->IsValid();
}
SEScalar0To1& PulseConfiguration::GetCalciumAbsorptionFraction()
{
  if (m_CalciumAbsorptionFraction == nullptr)
    m_CalciumAbsorptionFraction = new SEScalar0To1();
  return *m_CalciumAbsorptionFraction;
}
double PulseConfiguration::GetCalciumAbsorptionFraction() const
{
  if (m_CalciumAbsorptionFraction == nullptr)
    return SEScalar::dNaN();
  return m_CalciumAbsorptionFraction->GetValue();
}

bool PulseConfiguration::HasCarbohydrateAbsorptionFraction() const
{
  return m_CarbohydrateAbsorptionFraction == nullptr ? false : m_CarbohydrateAbsorptionFraction->IsValid();
}
SEScalar0To1& PulseConfiguration::GetCarbohydrateAbsorptionFraction()
{
  if (m_CarbohydrateAbsorptionFraction == nullptr)
    m_CarbohydrateAbsorptionFraction = new SEScalar0To1();
  return *m_CarbohydrateAbsorptionFraction;
}
double PulseConfiguration::GetCarbohydrateAbsorptionFraction() const
{
  if (m_CarbohydrateAbsorptionFraction == nullptr)
    return SEScalar::dNaN();
  return m_CarbohydrateAbsorptionFraction->GetValue();
}

bool PulseConfiguration::HasDefaultCarbohydrateDigestionRate() const
{
  return m_DefaultCarbohydrateDigestionRate == nullptr ? false : m_DefaultCarbohydrateDigestionRate->IsValid();
}
SEScalarMassPerTime& PulseConfiguration::GetDefaultCarbohydrateDigestionRate()
{
  if (m_DefaultCarbohydrateDigestionRate == nullptr)
    m_DefaultCarbohydrateDigestionRate = new SEScalarMassPerTime();
  return *m_DefaultCarbohydrateDigestionRate;
}
double PulseConfiguration::GetDefaultCarbohydrateDigestionRate(const MassPerTimeUnit& unit) const
{
  if (m_DefaultCarbohydrateDigestionRate == nullptr)
    return SEScalar::dNaN();
  return m_DefaultCarbohydrateDigestionRate->GetValue(unit);
}

bool PulseConfiguration::HasDefaultFatDigestionRate() const
{
  return m_DefaultFatDigestionRate == nullptr ? false : m_DefaultFatDigestionRate->IsValid();
}
SEScalarMassPerTime& PulseConfiguration::GetDefaultFatDigestionRate()
{
  if (m_DefaultFatDigestionRate == nullptr)
    m_DefaultFatDigestionRate = new SEScalarMassPerTime();
  return *m_DefaultFatDigestionRate;
}
double PulseConfiguration::GetDefaultFatDigestionRate(const MassPerTimeUnit& unit) const
{
  if (m_DefaultFatDigestionRate == nullptr)
    return SEScalar::dNaN();
  return m_DefaultFatDigestionRate->GetValue(unit);
}

bool PulseConfiguration::HasDefaultProteinDigestionRate() const
{
  return m_DefaultProteinDigestionRate == nullptr ? false : m_DefaultProteinDigestionRate->IsValid();
}
SEScalarMassPerTime& PulseConfiguration::GetDefaultProteinDigestionRate()
{
  if (m_DefaultProteinDigestionRate == nullptr)
    m_DefaultProteinDigestionRate = new SEScalarMassPerTime();
  return *m_DefaultProteinDigestionRate;
}
double PulseConfiguration::GetDefaultProteinDigestionRate(const MassPerTimeUnit& unit) const
{
  if (m_DefaultProteinDigestionRate == nullptr)
    return SEScalar::dNaN();
  return m_DefaultProteinDigestionRate->GetValue(unit);
}

bool PulseConfiguration::HasDefaultStomachContents() const
{
  return m_DefaultStomachContents != nullptr;
}
SENutrition& PulseConfiguration::GetDefaultStomachContents()
{
  if (m_DefaultStomachContents == nullptr)
    m_DefaultStomachContents = new SENutrition(GetLogger());
  return *m_DefaultStomachContents;
}
const SENutrition* PulseConfiguration::GetDefaultStomachContents() const
{
  return m_DefaultStomachContents;
}

bool PulseConfiguration::HasFatAbsorptionFraction() const
{
  return m_FatAbsorptionFraction == nullptr ? false : m_FatAbsorptionFraction->IsValid();
}
SEScalar0To1& PulseConfiguration::GetFatAbsorptionFraction()
{
  if (m_FatAbsorptionFraction == nullptr)
    m_FatAbsorptionFraction = new SEScalar0To1();
  return *m_FatAbsorptionFraction;
}
double PulseConfiguration::GetFatAbsorptionFraction() const
{
  if (m_FatAbsorptionFraction == nullptr)
    return SEScalar::dNaN();
  return m_FatAbsorptionFraction->GetValue();
}

bool PulseConfiguration::HasProteinToUreaFraction() const
{
  return m_ProteinToUreaFraction == nullptr ? false : m_ProteinToUreaFraction->IsValid();
}
SEScalar0To1& PulseConfiguration::GetProteinToUreaFraction()
{
  if (m_ProteinToUreaFraction == nullptr)
    m_ProteinToUreaFraction = new SEScalar0To1();
  return *m_ProteinToUreaFraction;
}
double PulseConfiguration::GetProteinToUreaFraction() const
{
  if (m_ProteinToUreaFraction == nullptr)
    return SEScalar::dNaN();
  return m_ProteinToUreaFraction->GetValue();
}

bool PulseConfiguration::HasWaterDigestionRate() const
{
  return m_WaterDigestionRate == nullptr ? false : m_WaterDigestionRate->IsValid();
}
SEScalarVolumePerTime& PulseConfiguration::GetWaterDigestionRate()
{
  if (m_WaterDigestionRate == nullptr)
    m_WaterDigestionRate = new SEScalarVolumePerTime();
  return *m_WaterDigestionRate;
}
double PulseConfiguration::GetWaterDigestionRate(const VolumePerTimeUnit& unit) const
{
  if (m_WaterDigestionRate == nullptr)
    return SEScalar::dNaN();
  return m_WaterDigestionRate->GetValue(unit);
}

///////////////
/** Nervous */
/////////////

bool PulseConfiguration::HasHeartElastanceDistributedTimeDelay() const
{
  return m_HeartElastanceDistributedTimeDelay == nullptr ? false : m_HeartElastanceDistributedTimeDelay->IsValid();
}
SEScalarTime& PulseConfiguration::GetHeartElastanceDistributedTimeDelay()
{
  if (m_HeartElastanceDistributedTimeDelay == nullptr)
    m_HeartElastanceDistributedTimeDelay = new SEScalarTime();
  return *m_HeartElastanceDistributedTimeDelay;
}
double PulseConfiguration::GetHeartElastanceDistributedTimeDelay(const TimeUnit& unit) const
{
  if (m_HeartElastanceDistributedTimeDelay == nullptr)
    return SEScalar::dNaN();
  return m_HeartElastanceDistributedTimeDelay->GetValue(unit);
}

bool PulseConfiguration::HasHeartRateDistributedTimeDelay() const
{
  return m_HeartRateDistributedTimeDelay == nullptr ? false : m_HeartRateDistributedTimeDelay->IsValid();
}
SEScalarTime& PulseConfiguration::GetHeartRateDistributedTimeDelay()
{
  if (m_HeartRateDistributedTimeDelay == nullptr)
    m_HeartRateDistributedTimeDelay = new SEScalarTime();
  return *m_HeartRateDistributedTimeDelay;
}
double PulseConfiguration::GetHeartRateDistributedTimeDelay(const TimeUnit& unit) const
{
  if (m_HeartRateDistributedTimeDelay == nullptr)
    return SEScalar::dNaN();
  return m_HeartRateDistributedTimeDelay->GetValue(unit);
}

bool PulseConfiguration::HasNormalizedHeartRateIntercept() const
{
  return m_NormalizedHeartRateIntercept == nullptr ? false : m_NormalizedHeartRateIntercept->IsValid();
}
SEScalar& PulseConfiguration::GetNormalizedHeartRateIntercept()
{
  if (m_NormalizedHeartRateIntercept == nullptr)
    m_NormalizedHeartRateIntercept = new SEScalar();
  return *m_NormalizedHeartRateIntercept;
}
double PulseConfiguration::GetNormalizedHeartRateIntercept() const
{
  if (m_NormalizedHeartRateIntercept == nullptr)
    return SEScalar::dNaN();
  return m_NormalizedHeartRateIntercept->GetValue();
}

bool PulseConfiguration::HasNormalizedHeartRateSympatheticSlope() const
{
  return m_NormalizedHeartRateSympatheticSlope == nullptr ? false : m_NormalizedHeartRateSympatheticSlope->IsValid();
}
SEScalar& PulseConfiguration::GetNormalizedHeartRateSympatheticSlope()
{
  if (m_NormalizedHeartRateSympatheticSlope == nullptr)
    m_NormalizedHeartRateSympatheticSlope = new SEScalar();
  return *m_NormalizedHeartRateSympatheticSlope;
}
double PulseConfiguration::GetNormalizedHeartRateSympatheticSlope() const
{
  if (m_NormalizedHeartRateSympatheticSlope == nullptr)
    return SEScalar::dNaN();
  return m_NormalizedHeartRateSympatheticSlope->GetValue();
}

bool PulseConfiguration::HasNormalizedHeartRateParasympatheticSlope() const
{
  return m_NormalizedHeartRateParasympatheticSlope == nullptr ? false : m_NormalizedHeartRateParasympatheticSlope->IsValid();
}
SEScalar& PulseConfiguration::GetNormalizedHeartRateParasympatheticSlope()
{
  if (m_NormalizedHeartRateParasympatheticSlope == nullptr)
    m_NormalizedHeartRateParasympatheticSlope = new SEScalar();
  return *m_NormalizedHeartRateParasympatheticSlope;
}
double PulseConfiguration::GetNormalizedHeartRateParasympatheticSlope() const
{
  if (m_NormalizedHeartRateParasympatheticSlope == nullptr)
    return SEScalar::dNaN();
  return m_NormalizedHeartRateParasympatheticSlope->GetValue();
}

bool PulseConfiguration::HasNormalizedHeartElastanceIntercept() const
{
  return m_NormalizedHeartElastanceIntercept == nullptr ? false : m_NormalizedHeartElastanceIntercept->IsValid();
}
SEScalar& PulseConfiguration::GetNormalizedHeartElastanceIntercept()
{
  if (m_NormalizedHeartElastanceIntercept == nullptr)
    m_NormalizedHeartElastanceIntercept = new SEScalar();
  return *m_NormalizedHeartElastanceIntercept;
}
double PulseConfiguration::GetNormalizedHeartElastanceIntercept() const
{
  if (m_NormalizedHeartElastanceIntercept == nullptr)
    return SEScalar::dNaN();
  return m_NormalizedHeartElastanceIntercept->GetValue();
}

bool PulseConfiguration::HasNormalizedHeartElastanceSympatheticSlope() const
{
  return m_NormalizedHeartElastanceSympatheticSlope == nullptr ? false : m_NormalizedHeartElastanceSympatheticSlope->IsValid();
}
SEScalar& PulseConfiguration::GetNormalizedHeartElastanceSympatheticSlope()
{
  if (m_NormalizedHeartElastanceSympatheticSlope == nullptr)
    m_NormalizedHeartElastanceSympatheticSlope = new SEScalar();
  return *m_NormalizedHeartElastanceSympatheticSlope;
}
double PulseConfiguration::GetNormalizedHeartElastanceSympatheticSlope() const
{
  if (m_NormalizedHeartElastanceSympatheticSlope == nullptr)
    return SEScalar::dNaN();
  return m_NormalizedHeartElastanceSympatheticSlope->GetValue();
}

bool PulseConfiguration::HasNormalizedResistanceIntercept() const
{
  return m_NormalizedResistanceIntercept == nullptr ? false : m_NormalizedResistanceIntercept->IsValid();
}
SEScalar& PulseConfiguration::GetNormalizedResistanceIntercept()
{
  if (m_NormalizedResistanceIntercept == nullptr)
    m_NormalizedResistanceIntercept = new SEScalar();
  return *m_NormalizedResistanceIntercept;
}
double PulseConfiguration::GetNormalizedResistanceIntercept() const
{
  if (m_NormalizedResistanceIntercept == nullptr)
    return SEScalar::dNaN();
  return m_NormalizedResistanceIntercept->GetValue();
}

bool PulseConfiguration::HasNormalizedResistanceSympatheticSlope() const
{
  return m_NormalizedResistanceSympatheticSlope == nullptr ? false : m_NormalizedResistanceSympatheticSlope->IsValid();
}
SEScalar& PulseConfiguration::GetNormalizedResistanceSympatheticSlope()
{
  if (m_NormalizedResistanceSympatheticSlope == nullptr)
    m_NormalizedResistanceSympatheticSlope = new SEScalar();
  return *m_NormalizedResistanceSympatheticSlope;
}
double PulseConfiguration::GetNormalizedResistanceSympatheticSlope() const
{
  if (m_NormalizedResistanceSympatheticSlope == nullptr)
    return SEScalar::dNaN();
  return m_NormalizedResistanceSympatheticSlope->GetValue();
}

bool PulseConfiguration::HasNormalizedComplianceIntercept() const
{
  return m_NormalizedComplianceIntercept == nullptr ? false : m_NormalizedComplianceIntercept->IsValid();
}
SEScalar& PulseConfiguration::GetNormalizedComplianceIntercept()
{
  if (m_NormalizedComplianceIntercept == nullptr)
    m_NormalizedComplianceIntercept = new SEScalar();
  return *m_NormalizedComplianceIntercept;
}
double PulseConfiguration::GetNormalizedComplianceIntercept() const
{
  if (m_NormalizedComplianceIntercept == nullptr)
    return SEScalar::dNaN();
  return m_NormalizedComplianceIntercept->GetValue();
}

bool PulseConfiguration::HasNormalizedComplianceParasympatheticSlope() const
{
  return m_NormalizedComplianceParasympatheticSlope == nullptr ? false : m_NormalizedComplianceParasympatheticSlope->IsValid();
}
SEScalar& PulseConfiguration::GetNormalizedComplianceParasympatheticSlope()
{
  if (m_NormalizedComplianceParasympatheticSlope == nullptr)
    m_NormalizedComplianceParasympatheticSlope = new SEScalar();
  return *m_NormalizedComplianceParasympatheticSlope;
}
double PulseConfiguration::GetNormalizedComplianceParasympatheticSlope() const
{
  if (m_NormalizedComplianceParasympatheticSlope == nullptr)
    return SEScalar::dNaN();
  return m_NormalizedComplianceParasympatheticSlope->GetValue();
}

bool PulseConfiguration::HasPupilDiameterBaseline() const
{
  return m_PupilDiameterBaseline == nullptr ? false : m_PupilDiameterBaseline->IsValid();
}
SEScalarLength& PulseConfiguration::GetPupilDiameterBaseline()
{
  if (m_PupilDiameterBaseline == nullptr)
    m_PupilDiameterBaseline = new SEScalarLength();
  return *m_PupilDiameterBaseline;
}
double PulseConfiguration::GetPupilDiameterBaseline(const LengthUnit& unit) const
{
  if (m_PupilDiameterBaseline == nullptr)
    return SEScalar::dNaN();
  return m_PupilDiameterBaseline->GetValue(unit);
}

bool PulseConfiguration::HasResponseSlope() const
{
  return m_ResponseSlope == nullptr ? false : m_ResponseSlope->IsValid();
}
SEScalar& PulseConfiguration::GetResponseSlope()
{
  if (m_ResponseSlope == nullptr)
    m_ResponseSlope = new SEScalar();
  return *m_ResponseSlope;
}
double PulseConfiguration::GetResponseSlope() const
{
  if (m_ResponseSlope == nullptr)
    return SEScalar::dNaN();
  return m_ResponseSlope->GetValue();
}

bool PulseConfiguration::HasSystemicResistanceDistributedTimeDelay() const
{
  return m_SystemicResistanceDistributedTimeDelay == nullptr ? false : m_SystemicResistanceDistributedTimeDelay->IsValid();
}
SEScalarTime& PulseConfiguration::GetSystemicResistanceDistributedTimeDelay()
{
  if (m_SystemicResistanceDistributedTimeDelay == nullptr)
    m_SystemicResistanceDistributedTimeDelay = new SEScalarTime();
  return *m_SystemicResistanceDistributedTimeDelay;
}
double PulseConfiguration::GetSystemicResistanceDistributedTimeDelay(const TimeUnit& unit) const
{
  if (m_SystemicResistanceDistributedTimeDelay == nullptr)
    return SEScalar::dNaN();
  return m_SystemicResistanceDistributedTimeDelay->GetValue(unit);
}

bool PulseConfiguration::HasVenousComplianceDistributedTimeDelay() const
{
  return m_VenousComplianceDistributedTimeDelay == nullptr ? false : m_VenousComplianceDistributedTimeDelay->IsValid();
}
SEScalarTime& PulseConfiguration::GetVenousComplianceDistributedTimeDelay()
{
  if (m_VenousComplianceDistributedTimeDelay == nullptr)
    m_VenousComplianceDistributedTimeDelay = new SEScalarTime();
  return *m_VenousComplianceDistributedTimeDelay;
}
double PulseConfiguration::GetVenousComplianceDistributedTimeDelay(const TimeUnit& unit) const
{
  if (m_VenousComplianceDistributedTimeDelay == nullptr)
    return SEScalar::dNaN();
  return m_VenousComplianceDistributedTimeDelay->GetValue(unit);
}

////////////
/** Renal */
////////////

bool PulseConfiguration::HasPlasmaSodiumConcentrationSetPoint() const
{
  return m_PlasmaSodiumConcentrationSetPoint == nullptr ? false : m_PlasmaSodiumConcentrationSetPoint->IsValid();
}
SEScalarMassPerVolume& PulseConfiguration::GetPlasmaSodiumConcentrationSetPoint()
{
  if (m_PlasmaSodiumConcentrationSetPoint == nullptr)
    m_PlasmaSodiumConcentrationSetPoint = new SEScalarMassPerVolume();
  return *m_PlasmaSodiumConcentrationSetPoint;
}
double PulseConfiguration::GetPlasmaSodiumConcentrationSetPoint(const MassPerVolumeUnit& unit) const
{
  if (m_PlasmaSodiumConcentrationSetPoint == nullptr)
    return SEScalar::dNaN();
  return m_PlasmaSodiumConcentrationSetPoint->GetValue(unit);
}

bool PulseConfiguration::HasPeritubularPotassiumConcentrationSetPoint() const
{
  return m_PeritubularPotassiumConcentrationSetPoint == nullptr ? false : m_PeritubularPotassiumConcentrationSetPoint->IsValid();
}
SEScalarMassPerVolume& PulseConfiguration::GetPeritubularPotassiumConcentrationSetPoint()
{
  if (m_PeritubularPotassiumConcentrationSetPoint == nullptr)
    m_PeritubularPotassiumConcentrationSetPoint = new SEScalarMassPerVolume();
  return *m_PeritubularPotassiumConcentrationSetPoint;
}
double PulseConfiguration::GetPeritubularPotassiumConcentrationSetPoint(const MassPerVolumeUnit& unit) const
{
  if (m_PeritubularPotassiumConcentrationSetPoint == nullptr)
    return SEScalar::dNaN();
  return m_PeritubularPotassiumConcentrationSetPoint->GetValue(unit);
}

bool PulseConfiguration::HasLeftGlomerularFluidPermeabilityBaseline() const
{
  return m_LeftGlomerularFluidPermeabilityBaseline == nullptr ? false : m_LeftGlomerularFluidPermeabilityBaseline->IsValid();
}
SEScalarVolumePerTimePressureArea& PulseConfiguration::GetLeftGlomerularFluidPermeabilityBaseline()
{
  if (m_LeftGlomerularFluidPermeabilityBaseline == nullptr)
    m_LeftGlomerularFluidPermeabilityBaseline = new SEScalarVolumePerTimePressureArea();
  return *m_LeftGlomerularFluidPermeabilityBaseline;
}
double PulseConfiguration::GetLeftGlomerularFluidPermeabilityBaseline(const VolumePerTimePressureAreaUnit& unit) const
{
  if (m_LeftGlomerularFluidPermeabilityBaseline == nullptr)
    return SEScalar::dNaN();
  return m_LeftGlomerularFluidPermeabilityBaseline->GetValue(unit);
}

bool PulseConfiguration::HasLeftGlomerularFilteringSurfaceAreaBaseline() const
{
  return m_LeftGlomerularFilteringSurfaceAreaBaseline == nullptr ? false : m_LeftGlomerularFilteringSurfaceAreaBaseline->IsValid();
}
SEScalarArea& PulseConfiguration::GetLeftGlomerularFilteringSurfaceAreaBaseline()
{
  if (m_LeftGlomerularFilteringSurfaceAreaBaseline == nullptr)
    m_LeftGlomerularFilteringSurfaceAreaBaseline = new SEScalarArea();
  return *m_LeftGlomerularFilteringSurfaceAreaBaseline;
}
double PulseConfiguration::GetLeftGlomerularFilteringSurfaceAreaBaseline(const AreaUnit& unit) const
{
  if (m_LeftGlomerularFilteringSurfaceAreaBaseline == nullptr)
    return SEScalar::dNaN();
  return m_LeftGlomerularFilteringSurfaceAreaBaseline->GetValue(unit);
}

bool PulseConfiguration::HasLeftTubularReabsorptionFluidPermeabilityBaseline() const
{
  return m_LeftTubularReabsorptionFluidPermeabilityBaseline == nullptr ? false : m_LeftTubularReabsorptionFluidPermeabilityBaseline->IsValid();
}
SEScalarVolumePerTimePressureArea& PulseConfiguration::GetLeftTubularReabsorptionFluidPermeabilityBaseline()
{
  if (m_LeftTubularReabsorptionFluidPermeabilityBaseline == nullptr)
    m_LeftTubularReabsorptionFluidPermeabilityBaseline = new SEScalarVolumePerTimePressureArea();
  return *m_LeftTubularReabsorptionFluidPermeabilityBaseline;
}
double PulseConfiguration::GetLeftTubularReabsorptionFluidPermeabilityBaseline(const VolumePerTimePressureAreaUnit& unit) const
{
  if (m_LeftTubularReabsorptionFluidPermeabilityBaseline == nullptr)
    return SEScalar::dNaN();
  return m_LeftTubularReabsorptionFluidPermeabilityBaseline->GetValue(unit);
}

bool PulseConfiguration::HasLeftTubularReabsorptionFilteringSurfaceAreaBaseline() const
{
  return m_LeftTubularReabsorptionFilteringSurfaceAreaBaseline == nullptr ? false : m_LeftTubularReabsorptionFilteringSurfaceAreaBaseline->IsValid();
}
SEScalarArea& PulseConfiguration::GetLeftTubularReabsorptionFilteringSurfaceAreaBaseline()
{
  if (m_LeftTubularReabsorptionFilteringSurfaceAreaBaseline == nullptr)
    m_LeftTubularReabsorptionFilteringSurfaceAreaBaseline = new SEScalarArea();
  return *m_LeftTubularReabsorptionFilteringSurfaceAreaBaseline;
}
double PulseConfiguration::GetLeftTubularReabsorptionFilteringSurfaceAreaBaseline(const AreaUnit& unit) const
{
  if (m_LeftTubularReabsorptionFilteringSurfaceAreaBaseline == nullptr)
    return SEScalar::dNaN();
  return m_LeftTubularReabsorptionFilteringSurfaceAreaBaseline->GetValue(unit);
}

bool PulseConfiguration::HasMaximumAfferentResistance() const
{
  return m_MaximumAfferentResistance == nullptr ? false : m_MaximumAfferentResistance->IsValid();
}
SEScalarPressureTimePerVolume& PulseConfiguration::GetMaximumAfferentResistance()
{
  if (m_MaximumAfferentResistance == nullptr)
    m_MaximumAfferentResistance = new SEScalarPressureTimePerVolume();
  return *m_MaximumAfferentResistance;
}
double PulseConfiguration::GetMaximumAfferentResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_MaximumAfferentResistance == nullptr)
    return SEScalar::dNaN();
  return m_MaximumAfferentResistance->GetValue(unit);
}

bool PulseConfiguration::HasMinimumAfferentResistance() const
{
  return m_MinimumAfferentResistance == nullptr ? false : m_MinimumAfferentResistance->IsValid();
}
SEScalarPressureTimePerVolume& PulseConfiguration::GetMinimumAfferentResistance()
{
  if (m_MinimumAfferentResistance == nullptr)
    m_MinimumAfferentResistance = new SEScalarPressureTimePerVolume();
  return *m_MinimumAfferentResistance;
}
double PulseConfiguration::GetMinimumAfferentResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_MinimumAfferentResistance == nullptr)
    return SEScalar::dNaN();
  return m_MinimumAfferentResistance->GetValue(unit);
}

bool PulseConfiguration::HasRightGlomerularFluidPermeabilityBaseline() const
{
  return m_RightGlomerularFluidPermeabilityBaseline == nullptr ? false : m_RightGlomerularFluidPermeabilityBaseline->IsValid();
}
SEScalarVolumePerTimePressureArea& PulseConfiguration::GetRightGlomerularFluidPermeabilityBaseline()
{
  if (m_RightGlomerularFluidPermeabilityBaseline == nullptr)
    m_RightGlomerularFluidPermeabilityBaseline = new SEScalarVolumePerTimePressureArea();
  return *m_RightGlomerularFluidPermeabilityBaseline;
}
double PulseConfiguration::GetRightGlomerularFluidPermeabilityBaseline(const VolumePerTimePressureAreaUnit& unit) const
{
  if (m_RightGlomerularFluidPermeabilityBaseline == nullptr)
    return SEScalar::dNaN();
  return m_RightGlomerularFluidPermeabilityBaseline->GetValue(unit);
}

bool PulseConfiguration::HasRightGlomerularFilteringSurfaceAreaBaseline() const
{
  return m_RightGlomerularFilteringSurfaceAreaBaseline == nullptr ? false : m_RightGlomerularFilteringSurfaceAreaBaseline->IsValid();
}
SEScalarArea& PulseConfiguration::GetRightGlomerularFilteringSurfaceAreaBaseline()
{
  if (m_RightGlomerularFilteringSurfaceAreaBaseline == nullptr)
    m_RightGlomerularFilteringSurfaceAreaBaseline = new SEScalarArea();
  return *m_RightGlomerularFilteringSurfaceAreaBaseline;
}
double PulseConfiguration::GetRightGlomerularFilteringSurfaceAreaBaseline(const AreaUnit& unit) const
{
  if (m_RightGlomerularFilteringSurfaceAreaBaseline == nullptr)
    return SEScalar::dNaN();
  return m_RightGlomerularFilteringSurfaceAreaBaseline->GetValue(unit);
}

bool PulseConfiguration::HasRightTubularReabsorptionFluidPermeabilityBaseline() const
{
  return m_RightTubularReabsorptionFluidPermeabilityBaseline == nullptr ? false : m_RightTubularReabsorptionFluidPermeabilityBaseline->IsValid();
}
SEScalarVolumePerTimePressureArea& PulseConfiguration::GetRightTubularReabsorptionFluidPermeabilityBaseline()
{
  if (m_RightTubularReabsorptionFluidPermeabilityBaseline == nullptr)
    m_RightTubularReabsorptionFluidPermeabilityBaseline = new SEScalarVolumePerTimePressureArea();
  return *m_RightTubularReabsorptionFluidPermeabilityBaseline;
}
double PulseConfiguration::GetRightTubularReabsorptionFluidPermeabilityBaseline(const VolumePerTimePressureAreaUnit& unit) const
{
  if (m_RightTubularReabsorptionFluidPermeabilityBaseline == nullptr)
    return SEScalar::dNaN();
  return m_RightTubularReabsorptionFluidPermeabilityBaseline->GetValue(unit);
}

bool PulseConfiguration::HasRightTubularReabsorptionFilteringSurfaceAreaBaseline() const
{
  return m_RightTubularReabsorptionFilteringSurfaceAreaBaseline == nullptr ? false : m_RightTubularReabsorptionFilteringSurfaceAreaBaseline->IsValid();
}
SEScalarArea& PulseConfiguration::GetRightTubularReabsorptionFilteringSurfaceAreaBaseline()
{
  if (m_RightTubularReabsorptionFilteringSurfaceAreaBaseline == nullptr)
    m_RightTubularReabsorptionFilteringSurfaceAreaBaseline = new SEScalarArea();
  return *m_RightTubularReabsorptionFilteringSurfaceAreaBaseline;
}
double PulseConfiguration::GetRightTubularReabsorptionFilteringSurfaceAreaBaseline(const AreaUnit& unit) const
{
  if (m_RightTubularReabsorptionFilteringSurfaceAreaBaseline == nullptr)
    return SEScalar::dNaN();
  return m_RightTubularReabsorptionFilteringSurfaceAreaBaseline->GetValue(unit);
}

bool PulseConfiguration::HasTargetSodiumDelivery()
{
  return m_TargetSodiumDelivery == nullptr ? false : m_TargetSodiumDelivery->IsValid();
}
SEScalarMassPerTime& PulseConfiguration::GetTargetSodiumDelivery()
{
  if (m_TargetSodiumDelivery == nullptr)
    m_TargetSodiumDelivery = new SEScalarMassPerTime();
  return *m_TargetSodiumDelivery;
}

//////////////////
/** Respiratory */
//////////////////
bool PulseConfiguration::HasCentralControllerCO2PressureSetPoint() const
{
  return m_CentralControllerCO2PressureSetPoint == nullptr ? false : m_CentralControllerCO2PressureSetPoint->IsValid();
}
SEScalarPressure& PulseConfiguration::GetCentralControllerCO2PressureSetPoint()
{
  if (m_CentralControllerCO2PressureSetPoint == nullptr)
    m_CentralControllerCO2PressureSetPoint = new SEScalarPressure();
  return *m_CentralControllerCO2PressureSetPoint;
}
double PulseConfiguration::GetCentralControllerCO2PressureSetPoint(const PressureUnit& unit) const
{
  if (m_CentralControllerCO2PressureSetPoint == nullptr)
    return SEScalar::dNaN();
  return m_CentralControllerCO2PressureSetPoint->GetValue(unit);
}

bool PulseConfiguration::HasCentralVentilatoryControllerGain() const
{
  return m_CentralVentilatoryControllerGain == nullptr ? false : m_CentralVentilatoryControllerGain->IsValid();
}
SEScalar& PulseConfiguration::GetCentralVentilatoryControllerGain()
{
  if (m_CentralVentilatoryControllerGain == nullptr)
    m_CentralVentilatoryControllerGain = new SEScalar();
  return *m_CentralVentilatoryControllerGain;
}
double PulseConfiguration::GetCentralVentilatoryControllerGain() const
{
  if (m_CentralVentilatoryControllerGain == nullptr)
    return SEScalar::dNaN();
  return m_CentralVentilatoryControllerGain->GetValue();
}

bool PulseConfiguration::HasMinimumAllowableInpiratoryAndExpiratoryPeriod() const
{
  return m_MinimumAllowableInpiratoryAndExpiratoryPeriod == nullptr ? false : m_MinimumAllowableInpiratoryAndExpiratoryPeriod->IsValid();
}
SEScalarTime& PulseConfiguration::GetMinimumAllowableInpiratoryAndExpiratoryPeriod()
{
  if (m_MinimumAllowableInpiratoryAndExpiratoryPeriod == nullptr)
    m_MinimumAllowableInpiratoryAndExpiratoryPeriod = new SEScalarTime();
  return *m_MinimumAllowableInpiratoryAndExpiratoryPeriod;
}
double PulseConfiguration::GetMinimumAllowableInpiratoryAndExpiratoryPeriod(const TimeUnit& unit) const
{
  if (m_MinimumAllowableInpiratoryAndExpiratoryPeriod == nullptr)
    return SEScalar::dNaN();
  return m_MinimumAllowableInpiratoryAndExpiratoryPeriod->GetValue(unit);
}

bool PulseConfiguration::HasMinimumAllowableTidalVolume() const
{
  return m_MinimumAllowableTidalVolume == nullptr ? false : m_MinimumAllowableTidalVolume->IsValid();
}
SEScalarVolume& PulseConfiguration::GetMinimumAllowableTidalVolume()
{
  if (m_MinimumAllowableTidalVolume == nullptr)
    m_MinimumAllowableTidalVolume = new SEScalarVolume();
  return *m_MinimumAllowableTidalVolume;
}
double PulseConfiguration::GetMinimumAllowableTidalVolume(const VolumeUnit& unit) const
{
  if (m_MinimumAllowableTidalVolume == nullptr)
    return SEScalar::dNaN();
  return m_MinimumAllowableTidalVolume->GetValue(unit);
}

bool PulseConfiguration::HasPeripheralControllerCO2PressureSetPoint() const
{
  return m_PeripheralControllerCO2PressureSetPoint == nullptr ? false : m_PeripheralControllerCO2PressureSetPoint->IsValid();
}
SEScalarPressure& PulseConfiguration::GetPeripheralControllerCO2PressureSetPoint()
{
  if (m_PeripheralControllerCO2PressureSetPoint == nullptr)
    m_PeripheralControllerCO2PressureSetPoint = new SEScalarPressure();
  return *m_PeripheralControllerCO2PressureSetPoint;
}
double PulseConfiguration::GetPeripheralControllerCO2PressureSetPoint(const PressureUnit& unit) const
{
  if (m_PeripheralControllerCO2PressureSetPoint == nullptr)
    return SEScalar::dNaN();
  return m_PeripheralControllerCO2PressureSetPoint->GetValue(unit);
}

bool PulseConfiguration::HasPeripheralVentilatoryControllerGain() const
{
  return m_PeripheralVentilatoryControllerGain == nullptr ? false : m_PeripheralVentilatoryControllerGain->IsValid();
}
SEScalar& PulseConfiguration::GetPeripheralVentilatoryControllerGain()
{
  if (m_PeripheralVentilatoryControllerGain == nullptr)
    m_PeripheralVentilatoryControllerGain = new SEScalar();
  return *m_PeripheralVentilatoryControllerGain;
}
double PulseConfiguration::GetPeripheralVentilatoryControllerGain() const
{
  if (m_PeripheralVentilatoryControllerGain == nullptr)
    return SEScalar::dNaN();
  return m_PeripheralVentilatoryControllerGain->GetValue();
}

bool PulseConfiguration::HasPulmonaryVentilationRateMaximum() const
{
  return m_PulmonaryVentilationRateMaximum == nullptr ? false : m_PulmonaryVentilationRateMaximum->IsValid();
}
SEScalarVolumePerTime& PulseConfiguration::GetPulmonaryVentilationRateMaximum()
{
  if (m_PulmonaryVentilationRateMaximum == nullptr)
    m_PulmonaryVentilationRateMaximum = new SEScalarVolumePerTime();
  return *m_PulmonaryVentilationRateMaximum;
}
double PulseConfiguration::GetPulmonaryVentilationRateMaximum(const VolumePerTimeUnit& unit) const
{
  if (m_PulmonaryVentilationRateMaximum == nullptr)
    return SEScalar::dNaN();
  return m_PulmonaryVentilationRateMaximum->GetValue(unit);
}

bool PulseConfiguration::HasVentilatoryOcclusionPressure() const
{
  return m_VentilatoryOcclusionPressure == nullptr ? false : m_VentilatoryOcclusionPressure->IsValid();
}
SEScalarPressure& PulseConfiguration::GetVentilatoryOcclusionPressure()
{
  if (m_VentilatoryOcclusionPressure == nullptr)
    m_VentilatoryOcclusionPressure = new SEScalarPressure();
  return *m_VentilatoryOcclusionPressure;
}
double PulseConfiguration::GetVentilatoryOcclusionPressure(const PressureUnit& unit) const
{
  if (m_VentilatoryOcclusionPressure == nullptr)
    return SEScalar::dNaN();
  return m_VentilatoryOcclusionPressure->GetValue(unit);
}

bool PulseConfiguration::HasVentilationTidalVolumeIntercept() const
{
  return m_VentilationTidalVolumeIntercept == nullptr ? false : m_VentilationTidalVolumeIntercept->IsValid();
}
SEScalarVolume& PulseConfiguration::GetVentilationTidalVolumeIntercept()
{
  if (m_VentilationTidalVolumeIntercept == nullptr)
    m_VentilationTidalVolumeIntercept = new SEScalarVolume();
  return *m_VentilationTidalVolumeIntercept;
}
double PulseConfiguration::GetVentilationTidalVolumeIntercept(const VolumeUnit& unit) const
{
  if (m_VentilationTidalVolumeIntercept == nullptr)
    return SEScalar::dNaN();
  return m_VentilationTidalVolumeIntercept->GetValue(unit);
}

void PulseConfiguration::UseExpandedRespiratory(eSwitch s)
{
  if (s == eSwitch::NullSwitch)
    return;
  m_UseExpandedRespiratory = s;
}
