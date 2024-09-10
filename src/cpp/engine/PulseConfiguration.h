/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

#include "engine/CommonDefs.h"
#include "cdm/engine/SEEngineConfiguration.h"
class SENutrition;
class SEEngineStabilization;
class SEDynamicStabilization;
class SETimedStabilization;
class SEEnvironmentalConditions;
class SEElectroCardioGram;

namespace pulse { class PBConfiguration; }
/**
* @brief %Pulse specific configuration parameters for all systems/equipment
*/
class PULSE_DECL PulseConfiguration : public SEEngineConfiguration
{
  friend pulse::PBConfiguration;//friend the serialization class
public:

  PulseConfiguration(Logger* logger = nullptr);
  virtual ~PulseConfiguration();

  virtual void Clear();
  void Merge(const PulseConfiguration&, SESubstanceManager& subMgr);
  virtual void Initialize(const std::string& dataDir = "", SESubstanceManager* subMgr = nullptr);

  bool SerializeToString(std::string& output, eSerializationFormat m) const;
  bool SerializeToFile(const std::string& filename) const;
  bool SerializeFromString(const std::string& src, eSerializationFormat m, SESubstanceManager& subMgr);
  bool SerializeFromFile(const std::string& filename, SESubstanceManager& subMgr);


  virtual bool HasTimeStep() const;
  virtual SEScalarTime& GetTimeStep();
  virtual double GetTimeStep(const TimeUnit& unit) const;

  virtual eSwitch AllowDynamicTimeStep() const { return m_AllowDynamicTimeStep; }
  virtual void AllowDynamicTimeStep(eSwitch s) { m_AllowDynamicTimeStep = s; }

  // You can have either a timed or dynamic stabilization object (cannot have both)
  virtual bool HasStabilization() const;
  virtual SEEngineStabilization* GetStabilization();
  virtual void RemoveStabilization();
  // Timed Methods, If you have dynamic, calling GetTimedStabilization will remove the dynamic object
  virtual bool HasTimedStabilization() const;
  virtual SETimedStabilization& GetTimedStabilization();
  virtual const SETimedStabilization* GetTimedStabilization() const;
  virtual void RemoveTimedStabilization();
  // Dynamic Methods, If you have timed, calling GetDynamicStabilization will remove the timed object
  virtual bool HasDynamicStabilization() const;
  virtual SEDynamicStabilization& GetDynamicStabilization();
  virtual const SEDynamicStabilization* GetDynamicStabilization() const;
  virtual void RemoveDynamicStabilization();

  virtual bool HasOverrides() const;
  virtual SEScalarProperties& GetOverrides();
  virtual const SEScalarProperties& GetOverrides() const;

  virtual bool HasModifiers() const;
  virtual SEScalarProperties& GetModifiers();
  virtual const SEScalarProperties& GetModifiers() const;
protected:

  SEScalarTime*                 m_TimeStep;
  eSwitch                       m_AllowDynamicTimeStep;
  SETimedStabilization*         m_TimedStabilization;
  SEDynamicStabilization*       m_DynamicStabilization;

  SEScalarProperties            m_Overrides;
  SEScalarProperties            m_Modifiers;

  //////////////////////
  /** Blood Chemistry */
  //////////////////////
public:
  virtual bool HasMeanCorpuscularHemoglobin() const;
  virtual SEScalarMassPerAmount& GetMeanCorpuscularHemoglobin();
  virtual double GetMeanCorpuscularHemoglobin(const MassPerAmountUnit& unit) const;

  virtual bool HasMeanCorpuscularVolume() const;
  virtual SEScalarVolume& GetMeanCorpuscularVolume();
  virtual double GetMeanCorpuscularVolume(const VolumeUnit& unit) const;

  virtual bool HasStandardDiffusionDistance() const;
  virtual SEScalarLength& GetStandardDiffusionDistance();
  virtual double GetStandardDiffusionDistance(const LengthUnit& unit) const;

  virtual bool HasStandardOxygenDiffusionCoefficient() const;
  virtual SEScalarAreaPerTimePressure& GetStandardOxygenDiffusionCoefficient();
  virtual double GetStandardOxygenDiffusionCoefficient(const AreaPerTimePressureUnit& unit) const;
protected:
  SEScalarMassPerAmount*       m_MeanCorpuscularHemoglobin;
  SEScalarVolume*              m_MeanCorpuscularVolume;
  SEScalarLength*              m_StandardDiffusionDistance;
  SEScalarAreaPerTimePressure* m_StandardOxygenDiffusionCoefficient;

  /////////////////////
  /** Cardiovascular */
  /////////////////////
public:
  virtual bool HasLeftHeartElastanceMaximum() const;
  virtual SEScalarPressurePerVolume& GetLeftHeartElastanceMaximum();
  virtual double GetLeftHeartElastanceMaximum(const PressurePerVolumeUnit& unit) const;

  virtual bool HasLeftHeartElastanceMinimum() const;
  virtual SEScalarPressurePerVolume& GetLeftHeartElastanceMinimum();
  virtual double GetLeftHeartElastanceMinimum(const PressurePerVolumeUnit& unit) const;

  virtual bool HasMinimumBloodVolumeFraction() const;
  virtual SEScalar0To1& GetMinimumBloodVolumeFraction();
  virtual double GetMinimumBloodVolumeFraction() const;

  virtual bool HasRightHeartElastanceMaximum() const;
  virtual SEScalarPressurePerVolume& GetRightHeartElastanceMaximum();
  virtual double GetRightHeartElastanceMaximum(const PressurePerVolumeUnit& unit) const;

  virtual bool HasRightHeartElastanceMinimum() const;
  virtual SEScalarPressurePerVolume& GetRightHeartElastanceMinimum();
  virtual double GetRightHeartElastanceMinimum(const PressurePerVolumeUnit& unit) const;

  virtual bool HasStandardPulmonaryCapillaryCoverage() const;
  virtual SEScalar& GetStandardPulmonaryCapillaryCoverage();
  virtual double GetStandardPulmonaryCapillaryCoverage() const;

  virtual eSwitch UseExpandedVasculature() const { return m_UseExpandedVasculature; }
  virtual void UseExpandedVasculature(eSwitch s);
  
  virtual eSwitch TuneCardiovascularCircuit() const { return m_TuneCardiovascularCircuit; }
  virtual void TuneCardiovascularCircuit(eSwitch s) { m_TuneCardiovascularCircuit = s; }

  virtual std::string CardiovascularTuningFile() const { return m_CardiovascularTuningFile; }
  virtual void CardiovascularTuningFile(const std::string& s) { m_CardiovascularTuningFile = s; }

protected:
  SEScalarPressurePerVolume* m_LeftHeartElastanceMaximum;
  SEScalarPressurePerVolume* m_LeftHeartElastanceMinimum;
  SEScalar0To1*              m_MinimumBloodVolumeFraction;
  SEScalarPressurePerVolume* m_RightHeartElastanceMaximum;
  SEScalarPressurePerVolume* m_RightHeartElastanceMinimum;
  SEScalar*                  m_StandardPulmonaryCapillaryCoverage;
  eSwitch                    m_UseExpandedVasculature;
  eSwitch                    m_TuneCardiovascularCircuit;
  std::string                m_CardiovascularTuningFile;

  //////////////
  /** Circuit */
  //////////////
public:

  virtual bool HasCardiovascularOpenResistance() const;
  virtual SEScalarPressureTimePerVolume& GetCardiovascularOpenResistance();
  virtual double GetCardiovascularOpenResistance(const PressureTimePerVolumeUnit& unit) const;

  virtual bool HasDefaultClosedElectricResistance() const;
  virtual SEScalarElectricResistance& GetDefaultClosedElectricResistance();
  virtual double GetDefaultClosedElectricResistance(const ElectricResistanceUnit& unit) const;

  virtual bool HasDefaultClosedFlowResistance() const;
  virtual SEScalarPressureTimePerVolume& GetDefaultClosedFlowResistance();
  virtual double GetDefaultClosedFlowResistance(const PressureTimePerVolumeUnit& unit) const;

  virtual bool HasDefaultClosedHeatResistance() const;
  virtual SEScalarHeatResistance& GetDefaultClosedHeatResistance();
  virtual double GetDefaultClosedHeatResistance(const HeatResistanceUnit& unit) const;

  virtual bool HasDefaultOpenElectricResistance() const;
  virtual SEScalarElectricResistance& GetDefaultOpenElectricResistance();
  virtual double GetDefaultOpenElectricResistance(const ElectricResistanceUnit& unit) const;

  virtual bool HasDefaultOpenFlowResistance() const;
  virtual SEScalarPressureTimePerVolume& GetDefaultOpenFlowResistance();
  virtual double GetDefaultOpenFlowResistance(const PressureTimePerVolumeUnit& unit) const;

  virtual bool HasDefaultOpenHeatResistance() const;
  virtual SEScalarHeatResistance& GetDefaultOpenHeatResistance();
  virtual double GetDefaultOpenHeatResistance(const HeatResistanceUnit& unit) const;

  virtual bool HasMachineClosedResistance() const;
  virtual SEScalarPressureTimePerVolume& GetMachineClosedResistance();
  virtual double GetMachineClosedResistance(const PressureTimePerVolumeUnit& unit) const;

  virtual bool HasMachineOpenResistance() const;
  virtual SEScalarPressureTimePerVolume& GetMachineOpenResistance();
  virtual double GetMachineOpenResistance(const PressureTimePerVolumeUnit& unit) const;

  virtual bool HasRespiratoryClosedResistance() const;
  virtual SEScalarPressureTimePerVolume& GetRespiratoryClosedResistance();
  virtual double GetRespiratoryClosedResistance(const PressureTimePerVolumeUnit& unit) const;

  virtual bool HasRespiratoryOpenResistance() const;
  virtual SEScalarPressureTimePerVolume& GetRespiratoryOpenResistance();
  virtual double GetRespiratoryOpenResistance(const PressureTimePerVolumeUnit& unit) const;

protected:
  SEScalarPressureTimePerVolume* m_CardiovascularOpenResistance;
  SEScalarElectricResistance* m_DefaultClosedElectricResistance;
  SEScalarPressureTimePerVolume* m_DefaultClosedFlowResistance;
  SEScalarHeatResistance* m_DefaultClosedHeatResistance;
  SEScalarElectricResistance* m_DefaultOpenElectricResistance;
  SEScalarPressureTimePerVolume* m_DefaultOpenFlowResistance;
  SEScalarHeatResistance* m_DefaultOpenHeatResistance;
  SEScalarPressureTimePerVolume* m_MachineClosedResistance;
  SEScalarPressureTimePerVolume* m_MachineOpenResistance;
  SEScalarPressureTimePerVolume* m_RespiratoryClosedResistance;
  SEScalarPressureTimePerVolume* m_RespiratoryOpenResistance;

  ////////////////
  /** Constants */
  ////////////////
public:
  virtual bool HasOxygenMetabolicConstant() const;
  virtual SEScalar& GetOxygenMetabolicConstant();
  virtual double GetOxygenMetabolicConstant() const;

  virtual bool HasStefanBoltzmann() const;
  virtual SEScalarPowerPerAreaTemperatureToTheFourth& GetStefanBoltzmann();
  virtual double GetStefanBoltzmann(const PowerPerAreaTemperatureToTheFourthUnit& unit) const;

  virtual bool HasUniversalGasConstant() const;
  virtual SEScalarHeatCapacitancePerAmount& GetUniversalGasConstant();
  virtual double GetUniversalGasConstant(const HeatCapacitancePerAmountUnit& unit) const;
protected:
  SEScalar* m_OxygenMetabolicConstant;
  SEScalarPowerPerAreaTemperatureToTheFourth* m_StefanBoltzmann;
  SEScalarHeatCapacitancePerAmount* m_UniversalGasConstant;

  ////////////
  /** Drugs */
  ////////////
public:
  virtual bool IsPDEnabled() const { return m_PDEnabled == eSwitch::On; }
  virtual void UsePDModel(eSwitch s) { m_PDEnabled = (s == eSwitch::NullSwitch) ? eSwitch::On : s; }
protected:
  eSwitch m_PDEnabled;

  ///////////
  /** ECG */
  //////////
public:
  virtual bool HasECG() const;
  virtual SEElectroCardioGram& GetECG();
  virtual const SEElectroCardioGram* GetECG() const;
  virtual void RemoveECG();
  //protected:
  SEElectroCardioGram* m_ECG;

  /////////////
  /** Energy */
  /////////////
public:
  virtual bool HasBodySpecificHeat() const;
  virtual SEScalarHeatCapacitancePerMass& GetBodySpecificHeat();
  virtual double GetBodySpecificHeat(const HeatCapacitancePerMassUnit& unit) const;

  virtual bool HasCarbonDioxideProductionFromOxygenConsumptionConstant() const;
  virtual SEScalar& GetCarbonDioxideProductionFromOxygenConsumptionConstant();
  virtual double GetCarbonDioxideProductionFromOxygenConsumptionConstant() const;

  virtual bool HasCoreTemperatureHigh() const;
  virtual SEScalarTemperature& GetCoreTemperatureHigh();
  virtual double GetCoreTemperatureHigh(const TemperatureUnit& unit) const;

  virtual bool HasCoreTemperatureLow() const;
  virtual SEScalarTemperature& GetCoreTemperatureLow();
  virtual double GetCoreTemperatureLow(const TemperatureUnit& unit) const;

  virtual bool HasDeltaCoreTemperatureLow() const;
  virtual SEScalarTemperature& GetDeltaCoreTemperatureLow();
  virtual double GetDeltaCoreTemperatureLow(const TemperatureUnit& unit) const;

  virtual bool HasEnergyPerATP() const;
  virtual SEScalarEnergyPerAmount& GetEnergyPerATP();
  virtual double GetEnergyPerATP(const EnergyPerAmountUnit& unit) const;

  virtual bool HasSweatHeatTransfer() const;
  virtual SEScalarHeatConductance& GetSweatHeatTransfer();
  virtual double GetSweatHeatTransfer(const HeatConductanceUnit& unit) const;

  virtual bool HasVaporizationEnergy() const;
  virtual SEScalarEnergyPerMass& GetVaporizationEnergy();
  virtual double GetVaporizationEnergy(const EnergyPerMassUnit& unit) const;

  virtual bool HasVaporSpecificHeat() const;
  virtual SEScalarHeatCapacitancePerMass& GetVaporSpecificHeat();
  virtual double GetVaporSpecificHeat(const HeatCapacitancePerMassUnit& unit) const;
protected:
  SEScalarHeatCapacitancePerMass* m_BodySpecificHeat;
  SEScalar* m_CarbonDioxideProductionFromOxygenConsumptionConstant;
  SEScalarTemperature* m_CoreTemperatureLow;
  SEScalarTemperature* m_CoreTemperatureHigh;
  SEScalarTemperature* m_DeltaCoreTemperatureLow;
  SEScalarEnergyPerAmount* m_EnergyPerATP;
  SEScalarHeatConductance* m_SweatHeatTransfer;
  SEScalarEnergyPerMass* m_VaporizationEnergy;
  SEScalarHeatCapacitancePerMass* m_VaporSpecificHeat;

  //////////////////
  /** Environment */
  //////////////////
public:
  virtual bool HasAirDensity() const;
  virtual SEScalarMassPerVolume& GetAirDensity();
  virtual double GetAirDensity(const MassPerVolumeUnit& unit) const;

  virtual bool HasAirSpecificHeat() const;
  virtual SEScalarHeatCapacitancePerMass& GetAirSpecificHeat();
  virtual double GetAirSpecificHeat(const HeatCapacitancePerMassUnit& unit) const;

  virtual bool HasMolarMassOfDryAir() const;
  virtual SEScalarMassPerAmount& GetMolarMassOfDryAir();
  virtual double GetMolarMassOfDryAir(const MassPerAmountUnit& unit) const;

  virtual bool HasMolarMassOfWaterVapor() const;
  virtual SEScalarMassPerAmount& GetMolarMassOfWaterVapor();
  virtual double GetMolarMassOfWaterVapor(const MassPerAmountUnit& unit) const;

  virtual bool HasInitialEnvironmentalConditions() const;
  virtual SEEnvironmentalConditions& GetInitialEnvironmentalConditions();
  virtual const SEEnvironmentalConditions* GetInitialEnvironmentalConditions() const;

  virtual bool HasWaterDensity() const;
  virtual SEScalarMassPerVolume& GetWaterDensity();
  virtual double GetWaterDensity(const MassPerVolumeUnit& unit) const;
protected:
  SEScalarMassPerVolume* m_AirDensity;
  SEScalarHeatCapacitancePerMass* m_AirSpecificHeat;
  SEScalarMassPerAmount* m_MolarMassOfDryAir;
  SEScalarMassPerAmount* m_MolarMassOfWaterVapor;
  SEEnvironmentalConditions* m_InitialEnvironmentalConditions;
  SEScalarMassPerVolume* m_WaterDensity;

  ///////////////////////
  /** Gastrointestinal */
  ///////////////////////
public:
  virtual bool HasCalciumDigestionRate() const;
  virtual SEScalarMassPerTime& GetCalciumDigestionRate();
  virtual double GetCalciumDigestionRate(const MassPerTimeUnit& unit) const;

  virtual bool HasCalciumAbsorptionFraction() const;
  virtual SEScalar0To1& GetCalciumAbsorptionFraction();
  virtual double GetCalciumAbsorptionFraction() const;

  virtual bool HasCarbohydrateAbsorptionFraction() const;
  virtual SEScalar0To1& GetCarbohydrateAbsorptionFraction();
  virtual double GetCarbohydrateAbsorptionFraction() const;

  virtual bool HasDefaultCarbohydrateDigestionRate() const;
  virtual SEScalarMassPerTime& GetDefaultCarbohydrateDigestionRate();
  virtual double GetDefaultCarbohydrateDigestionRate(const MassPerTimeUnit& unit) const;

  virtual bool HasDefaultFatDigestionRate() const;
  virtual SEScalarMassPerTime& GetDefaultFatDigestionRate();
  virtual double GetDefaultFatDigestionRate(const MassPerTimeUnit& unit) const;

  virtual bool HasDefaultProteinDigestionRate() const;
  virtual SEScalarMassPerTime& GetDefaultProteinDigestionRate();
  virtual double GetDefaultProteinDigestionRate(const MassPerTimeUnit& unit) const;

  virtual bool HasDefaultStomachContents() const;
  virtual SENutrition& GetDefaultStomachContents();
  virtual const SENutrition* GetDefaultStomachContents() const;

  virtual bool HasFatAbsorptionFraction() const;
  virtual SEScalar0To1& GetFatAbsorptionFraction();
  virtual double GetFatAbsorptionFraction() const;

  virtual bool HasProteinToUreaFraction() const;
  virtual SEScalar0To1& GetProteinToUreaFraction();
  virtual double GetProteinToUreaFraction() const;

  virtual bool HasWaterDigestionRate() const;
  virtual SEScalarVolumePerTime& GetWaterDigestionRate();
  virtual double GetWaterDigestionRate(const VolumePerTimeUnit& unit) const;

protected:

  SEScalarMassPerTime* m_CalciumDigestionRate;
  SEScalar0To1* m_CalciumAbsorptionFraction;
  SEScalar0To1* m_CarbohydrateAbsorptionFraction;
  SEScalarMassPerTime* m_DefaultCarbohydrateDigestionRate;
  SEScalarMassPerTime* m_DefaultFatDigestionRate;
  SEScalarMassPerTime* m_DefaultProteinDigestionRate;
  SENutrition* m_DefaultStomachContents;
  SEScalar0To1* m_FatAbsorptionFraction;
  SEScalar0To1* m_ProteinToUreaFraction;
  SEScalarVolumePerTime* m_WaterDigestionRate;

  ///////////////
  /** Nervous */
  /////////////
public:
  virtual bool IsCerebrospinalFluidEnabled() const { return m_CerebrospinalFluidEnabled == eSwitch::On; }
  virtual void EnableCerebrospinalFluid(eSwitch s) { m_CerebrospinalFluidEnabled = (s == eSwitch::NullSwitch) ? eSwitch::On : s; }

  virtual eSwitch GetBaroreceptorFeedback() const { return m_BaroreceptorFeedback; }
  virtual void SetBaroreceptorFeedback(eSwitch s) { m_BaroreceptorFeedback = (s == eSwitch::NullSwitch) ? eSwitch::On : s; }

  virtual eSwitch GetChemoreceptorFeedback() const { return m_ChemoreceptorFeedback; }
  virtual void SetChemoreceptorFeedback(eSwitch s) { m_ChemoreceptorFeedback = (s == eSwitch::NullSwitch) ? eSwitch::On : s; }

  virtual bool HasHeartElastanceDistributedTimeDelay() const;
  virtual SEScalarTime& GetHeartElastanceDistributedTimeDelay();
  virtual double GetHeartElastanceDistributedTimeDelay(const TimeUnit& unit) const;

  virtual bool HasHeartRateDistributedTimeDelay() const;
  virtual SEScalarTime& GetHeartRateDistributedTimeDelay();
  virtual double GetHeartRateDistributedTimeDelay(const TimeUnit& unit) const;

  virtual bool HasNormalizedHeartRateIntercept() const;
  virtual SEScalar& GetNormalizedHeartRateIntercept();
  virtual double GetNormalizedHeartRateIntercept() const;

  virtual bool HasNormalizedHeartRateSympatheticSlope() const;
  virtual SEScalar& GetNormalizedHeartRateSympatheticSlope();
  virtual double GetNormalizedHeartRateSympatheticSlope() const;

  virtual bool HasNormalizedHeartRateParasympatheticSlope() const;
  virtual SEScalar& GetNormalizedHeartRateParasympatheticSlope();
  virtual double GetNormalizedHeartRateParasympatheticSlope() const;

  virtual bool HasNormalizedHeartElastanceIntercept() const;
  virtual SEScalar& GetNormalizedHeartElastanceIntercept();
  virtual double GetNormalizedHeartElastanceIntercept() const;

  virtual bool HasNormalizedHeartElastanceSympatheticSlope() const;
  virtual SEScalar& GetNormalizedHeartElastanceSympatheticSlope();
  virtual double GetNormalizedHeartElastanceSympatheticSlope() const;

  virtual bool HasNormalizedResistanceIntercept() const;
  virtual SEScalar& GetNormalizedResistanceIntercept();
  virtual double GetNormalizedResistanceIntercept() const;

  virtual bool HasNormalizedResistanceSympatheticSlope() const;
  virtual SEScalar& GetNormalizedResistanceSympatheticSlope();
  virtual double GetNormalizedResistanceSympatheticSlope() const;

  virtual bool HasNormalizedComplianceIntercept() const;
  virtual SEScalar& GetNormalizedComplianceIntercept();
  virtual double GetNormalizedComplianceIntercept() const;

  virtual bool HasNormalizedComplianceParasympatheticSlope() const;
  virtual SEScalar& GetNormalizedComplianceParasympatheticSlope();
  virtual double GetNormalizedComplianceParasympatheticSlope() const;

  virtual bool HasPupilDiameterBaseline() const;
  virtual SEScalarLength& GetPupilDiameterBaseline();
  virtual double GetPupilDiameterBaseline(const LengthUnit& unit) const;

  virtual bool HasResponseSlope() const;
  virtual SEScalar& GetResponseSlope();
  virtual double GetResponseSlope() const;

  virtual bool HasSystemicResistanceDistributedTimeDelay() const;
  virtual SEScalarTime& GetSystemicResistanceDistributedTimeDelay();
  virtual double GetSystemicResistanceDistributedTimeDelay(const TimeUnit& unit) const;

  virtual bool HasVenousComplianceDistributedTimeDelay() const;
  virtual SEScalarTime& GetVenousComplianceDistributedTimeDelay();
  virtual double GetVenousComplianceDistributedTimeDelay(const TimeUnit& unit) const;

protected:
  eSwitch         m_CerebrospinalFluidEnabled;
  eSwitch         m_ChemoreceptorFeedback;
  eSwitch         m_BaroreceptorFeedback;
  SEScalarTime*   m_HeartElastanceDistributedTimeDelay;
  SEScalarTime*   m_HeartRateDistributedTimeDelay;
  SEScalar*       m_NormalizedHeartRateIntercept;
  SEScalar*       m_NormalizedHeartRateSympatheticSlope;
  SEScalar*       m_NormalizedHeartRateParasympatheticSlope;
  SEScalar*       m_NormalizedHeartElastanceIntercept;
  SEScalar*       m_NormalizedHeartElastanceSympatheticSlope;
  SEScalar*       m_NormalizedResistanceIntercept;
  SEScalar*       m_NormalizedResistanceSympatheticSlope;
  SEScalar*       m_NormalizedComplianceIntercept;
  SEScalar*       m_NormalizedComplianceParasympatheticSlope;
  SEScalarLength* m_PupilDiameterBaseline;
  SEScalar*       m_ResponseSlope;
  SEScalarTime*   m_SystemicResistanceDistributedTimeDelay;
  SEScalarTime*   m_VenousComplianceDistributedTimeDelay;

  ////////////
  /** Renal */
  ////////////
public:
  virtual bool IsRenalEnabled() const { return m_RenalEnabled == eSwitch::On; }
  virtual void EnableRenal(eSwitch s) { m_RenalEnabled = (s == eSwitch::NullSwitch) ? eSwitch::On : s; }

  virtual bool HasPlasmaSodiumConcentrationSetPoint() const;
  virtual SEScalarMassPerVolume& GetPlasmaSodiumConcentrationSetPoint();
  virtual double GetPlasmaSodiumConcentrationSetPoint(const MassPerVolumeUnit& unit) const;

  virtual bool HasPeritubularPotassiumConcentrationSetPoint() const;
  virtual SEScalarMassPerVolume& GetPeritubularPotassiumConcentrationSetPoint();
  virtual double GetPeritubularPotassiumConcentrationSetPoint(const MassPerVolumeUnit& unit) const;

  virtual bool HasLeftGlomerularFilteringSurfaceAreaBaseline() const;
  virtual SEScalarArea& GetLeftGlomerularFilteringSurfaceAreaBaseline();
  virtual double GetLeftGlomerularFilteringSurfaceAreaBaseline(const AreaUnit& unit) const;

  virtual bool HasLeftGlomerularFluidPermeabilityBaseline() const;
  virtual SEScalarVolumePerTimePressureArea& GetLeftGlomerularFluidPermeabilityBaseline();
  virtual double GetLeftGlomerularFluidPermeabilityBaseline(const VolumePerTimePressureAreaUnit& unit) const;

  virtual bool HasLeftTubularReabsorptionFilteringSurfaceAreaBaseline() const;
  virtual SEScalarArea& GetLeftTubularReabsorptionFilteringSurfaceAreaBaseline();
  virtual double GetLeftTubularReabsorptionFilteringSurfaceAreaBaseline(const AreaUnit& unit) const;

  virtual bool HasLeftTubularReabsorptionFluidPermeabilityBaseline() const;
  virtual SEScalarVolumePerTimePressureArea& GetLeftTubularReabsorptionFluidPermeabilityBaseline();
  virtual double GetLeftTubularReabsorptionFluidPermeabilityBaseline(const VolumePerTimePressureAreaUnit& unit) const;

  virtual bool HasMaximumAfferentResistance() const;
  virtual SEScalarPressureTimePerVolume& GetMaximumAfferentResistance();
  virtual double GetMaximumAfferentResistance(const PressureTimePerVolumeUnit& unit) const;

  virtual bool HasMinimumAfferentResistance() const;
  virtual SEScalarPressureTimePerVolume& GetMinimumAfferentResistance();
  virtual double GetMinimumAfferentResistance(const PressureTimePerVolumeUnit& unit) const;

  virtual bool HasRightGlomerularFilteringSurfaceAreaBaseline() const;
  virtual SEScalarArea& GetRightGlomerularFilteringSurfaceAreaBaseline();
  virtual double GetRightGlomerularFilteringSurfaceAreaBaseline(const AreaUnit& unit) const;

  virtual bool HasRightGlomerularFluidPermeabilityBaseline() const;
  virtual SEScalarVolumePerTimePressureArea& GetRightGlomerularFluidPermeabilityBaseline();
  virtual double GetRightGlomerularFluidPermeabilityBaseline(const VolumePerTimePressureAreaUnit& unit) const;

  virtual bool HasRightTubularReabsorptionFilteringSurfaceAreaBaseline() const;
  virtual SEScalarArea& GetRightTubularReabsorptionFilteringSurfaceAreaBaseline();
  virtual double GetRightTubularReabsorptionFilteringSurfaceAreaBaseline(const AreaUnit& unit) const;

  virtual bool HasRightTubularReabsorptionFluidPermeabilityBaseline() const;
  virtual SEScalarVolumePerTimePressureArea& GetRightTubularReabsorptionFluidPermeabilityBaseline();
  virtual double GetRightTubularReabsorptionFluidPermeabilityBaseline(const VolumePerTimePressureAreaUnit& unit) const;

  virtual bool HasTargetSodiumDelivery();
  virtual SEScalarMassPerTime& GetTargetSodiumDelivery();
protected:
  eSwitch                              m_RenalEnabled;

  SEScalarMassPerVolume* m_PlasmaSodiumConcentrationSetPoint;
  SEScalarMassPerVolume* m_PeritubularPotassiumConcentrationSetPoint;

  SEScalarArea* m_LeftGlomerularFilteringSurfaceAreaBaseline;
  SEScalarVolumePerTimePressureArea* m_LeftGlomerularFluidPermeabilityBaseline;
  SEScalarArea* m_LeftTubularReabsorptionFilteringSurfaceAreaBaseline;
  SEScalarVolumePerTimePressureArea* m_LeftTubularReabsorptionFluidPermeabilityBaseline;

  SEScalarArea* m_RightGlomerularFilteringSurfaceAreaBaseline;
  SEScalarVolumePerTimePressureArea* m_RightGlomerularFluidPermeabilityBaseline;
  SEScalarArea* m_RightTubularReabsorptionFilteringSurfaceAreaBaseline;
  SEScalarVolumePerTimePressureArea* m_RightTubularReabsorptionFluidPermeabilityBaseline;

  SEScalarPressureTimePerVolume* m_MaximumAfferentResistance;
  SEScalarPressureTimePerVolume* m_MinimumAfferentResistance;

  SEScalarMassPerTime* m_TargetSodiumDelivery;


  //////////////////
  /** Respiratory */
  //////////////////
public:
  virtual bool HasCentralControllerCO2PressureSetPoint() const;
  virtual SEScalarPressure& GetCentralControllerCO2PressureSetPoint();
  virtual double GetCentralControllerCO2PressureSetPoint(const PressureUnit& unit) const;

  virtual bool HasCentralVentilatoryControllerGain() const;
  virtual SEScalar& GetCentralVentilatoryControllerGain();
  virtual double GetCentralVentilatoryControllerGain() const;

  virtual bool HasMinimumAllowableInpiratoryAndExpiratoryPeriod() const;
  virtual SEScalarTime& GetMinimumAllowableInpiratoryAndExpiratoryPeriod();
  virtual double GetMinimumAllowableInpiratoryAndExpiratoryPeriod(const TimeUnit& unit) const;

  virtual bool HasMinimumAllowableTidalVolume() const;
  virtual SEScalarVolume& GetMinimumAllowableTidalVolume();
  virtual double GetMinimumAllowableTidalVolume(const VolumeUnit& unit) const;


  virtual bool HasPeripheralControllerCO2PressureSetPoint() const;
  virtual SEScalarPressure& GetPeripheralControllerCO2PressureSetPoint();
  virtual double GetPeripheralControllerCO2PressureSetPoint(const PressureUnit& unit) const;

  virtual bool HasPeripheralVentilatoryControllerGain() const;
  virtual SEScalar& GetPeripheralVentilatoryControllerGain();
  virtual double GetPeripheralVentilatoryControllerGain() const;

  virtual bool HasPulmonaryVentilationRateMaximum() const;
  virtual SEScalarVolumePerTime& GetPulmonaryVentilationRateMaximum();
  virtual double GetPulmonaryVentilationRateMaximum(const VolumePerTimeUnit& unit) const;

  virtual bool HasVentilationTidalVolumeIntercept() const;
  virtual SEScalarVolume& GetVentilationTidalVolumeIntercept();
  virtual double GetVentilationTidalVolumeIntercept(const VolumeUnit& unit) const;

  virtual bool HasVentilatoryOcclusionPressure() const;
  virtual SEScalarPressure& GetVentilatoryOcclusionPressure();
  virtual double GetVentilatoryOcclusionPressure(const PressureUnit& unit) const;

  virtual eSwitch UseExpandedRespiratory() const { return m_UseExpandedRespiratory; }
  virtual void UseExpandedRespiratory(eSwitch s);

protected:
  SEScalarPressure*       m_CentralControllerCO2PressureSetPoint;
  SEScalar*               m_CentralVentilatoryControllerGain;
  SEScalarTime*           m_MinimumAllowableInpiratoryAndExpiratoryPeriod;
  SEScalarVolume*         m_MinimumAllowableTidalVolume;
  SEScalarPressure*       m_PeripheralControllerCO2PressureSetPoint;
  SEScalar*               m_PeripheralVentilatoryControllerGain;
  SEScalarVolumePerTime*  m_PulmonaryVentilationRateMaximum;
  SEScalarVolume*         m_VentilationTidalVolumeIntercept;
  SEScalarPressure*       m_VentilatoryOcclusionPressure;
  eSwitch                 m_UseExpandedRespiratory;

  /////////////
  /** Tissue */
  /////////////
public:
  virtual bool IsTissueEnabled() const { return m_TissueEnabled == eSwitch::On; }
  virtual void EnableTissue(eSwitch s) { m_TissueEnabled = (s == eSwitch::NullSwitch) ? eSwitch::On : s; }
protected:
  eSwitch m_TissueEnabled;
};
