/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/PulseConfiguration.h"
#include "engine/common/controller/CircuitManager.h"
#include "engine/common/controller/CompartmentManager.h"
#include "engine/common/controller/SubstanceManager.h"
#include "engine/common/system/physiology/RespiratoryModel.h"
   // Conditions 
#include "cdm/engine/SEConditionManager.h"
#include "cdm/patient/conditions/SEChronicObstructivePulmonaryDisease.h"
#include "cdm/patient/conditions/SEImpairedAlveolarExchange.h"
#include "cdm/patient/conditions/SEPneumonia.h"
#include "cdm/patient/conditions/SEPulmonaryFibrosis.h"
#include "cdm/patient/conditions/SEPulmonaryShunt.h"
#include "cdm/patient/conditions/SEAcuteRespiratoryDistressSyndrome.h"
// Actions
#include "cdm/engine/SEActionManager.h"
#include "cdm/engine/SEOverrides.h"
#include "cdm/engine/SEPatientActionCollection.h"
#include "cdm/patient/actions/SEAcuteRespiratoryDistressSyndromeExacerbation.h"
#include "cdm/patient/actions/SEAirwayObstruction.h"
#include "cdm/patient/actions/SEAsthmaAttack.h"
#include "cdm/patient/actions/SEBronchoconstriction.h"
#include "cdm/patient/actions/SEChestOcclusiveDressing.h"
#include "cdm/patient/actions/SEChronicObstructivePulmonaryDiseaseExacerbation.h"
#include "cdm/patient/actions/SEConsciousRespiration.h"
#include "cdm/patient/actions/SEHemothorax.h"
#include "cdm/patient/actions/SEForcedExhale.h"
#include "cdm/patient/actions/SEForcedInhale.h"
#include "cdm/patient/actions/SEForcedPause.h"
#include "cdm/patient/actions/SEImpairedAlveolarExchangeExacerbation.h"
#include "cdm/patient/actions/SEUseInhaler.h"
#include "cdm/patient/actions/SEDyspnea.h"
#include "cdm/patient/actions/SEIntubation.h"
#include "cdm/patient/actions/SEMechanicalVentilation.h"
#include "cdm/patient/actions/SENeedleDecompression.h"
#include "cdm/patient/actions/SEPneumoniaExacerbation.h"
#include "cdm/patient/actions/SEPrimaryBlastLungInjury.h"
#include "cdm/patient/actions/SEPulmonaryShuntExacerbation.h"
#include "cdm/patient/actions/SERespiratoryFatigue.h"
#include "cdm/patient/actions/SERespiratoryMechanicsConfiguration.h"
#include "cdm/patient/actions/SERespiratoryMechanicsModification.h"
#include "cdm/patient/actions/SESupplementalOxygen.h"
#include "cdm/patient/actions/SETensionPneumothorax.h"
#include "cdm/patient/actions/SETubeThoracostomy.h"
// Dependent Systems
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SEDrugSystem.h"
#include "cdm/system/physiology/SEEnergySystem.h"
#include "cdm/system/physiology/SERespiratoryMechanics.h"
#include "cdm/system/physiology/SERespiratoryMechanicsModifiers.h"
#include "cdm/system/environment/SEEnvironmentalConditions.h"
// CDM
#include "cdm/patient/SEPatient.h"
#include "cdm/engine/SEEventManager.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceAerosolization.h"
#include "cdm/substance/SESubstanceConcentration.h"
#include "cdm/substance/SESubstanceFraction.h"
#include "cdm/substance/SESubstanceTransport.h"
#include "cdm/circuit/fluid/SEFluidCircuitCalculator.h"
#include "cdm/circuit/fluid/SEFluidCircuit.h"
#include "cdm/circuit/fluid/SEFluidCircuitNode.h"
#include "cdm/circuit/fluid/SEFluidCircuitPath.h"
#include "cdm/compartment/fluid/SEGasCompartmentGraph.h"
#include "cdm/compartment/fluid/SELiquidCompartmentGraph.h"
// Properties
#include "cdm/properties/SECurve.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarArea.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarInversePressure.h"
#include "cdm/properties/SEScalarInverseVolume.h"
#include "cdm/properties/SEScalarLength.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarNegative1To1.h"
#include "cdm/properties/SEScalarPower.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarPressurePerVolume.h"
#include "cdm/properties/SEScalarPressureTimePerVolume.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarUnsigned.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalarVolumePerPressure.h"
#include "cdm/properties/SEFunctionVolumeVsTime.h"
#include "cdm/properties/SERunningAverage.h"
// Utilitiy Classes
#include "cdm/utils/DataTrack.h"
#include "cdm/utils/GeneralMath.h"

//Flag for data tracks
//Should be commented out, unless debugging
//#define DEBUG

//Flag for setting things constant to test
//Should be commented out, unless debugging/tuning
//#define TUNING

namespace pulse
{

  RespiratoryModel::RespiratoryModel(Data& data) : RespiratorySystem(data.GetLogger()), Model(data)
  {
    m_BloodPHRunningAverage = new SERunningAverage();
    m_MeanAirwayPressure_cmH2O = new SERunningAverage();
    m_ArterialO2RunningAverage_mmHg = new SERunningAverage();
    m_ArterialCO2RunningAverage_mmHg = new SERunningAverage();
    m_MixedExpiredCO2RunningAverage_mmHg = new SERunningAverage();

    m_Calculator = new SEFluidCircuitCalculator(VolumePerPressureUnit::L_Per_cmH2O, VolumePerTimeUnit::L_Per_s, PressureTimeSquaredPerVolumeUnit::cmH2O_s2_Per_L, PressureUnit::cmH2O, VolumeUnit::L, PressureTimePerVolumeUnit::cmH2O_s_Per_L, GetLogger());
    m_GasTransporter = new SEGasTransporter(VolumePerTimeUnit::L_Per_s, VolumeUnit::L, VolumeUnit::L, NoUnit::unitless, GetLogger());
    m_AerosolTransporter = new SELiquidTransporter(VolumePerTimeUnit::mL_Per_s, VolumeUnit::mL, MassUnit::ug, MassPerVolumeUnit::ug_Per_mL, GetLogger());
    Clear();
  }

  RespiratoryModel::~RespiratoryModel()
  {
    Clear();
    delete m_BloodPHRunningAverage;
    delete m_MeanAirwayPressure_cmH2O;
    delete m_ArterialO2RunningAverage_mmHg;
    delete m_ArterialCO2RunningAverage_mmHg;
    delete m_MixedExpiredCO2RunningAverage_mmHg;

    delete m_Calculator;
    delete m_GasTransporter;
    delete m_AerosolTransporter;
  }

  void RespiratoryModel::Clear()
  {
    RespiratorySystem::Clear();

    // Patient
    m_PatientActions = nullptr;

    // Compartments
    m_Environment = nullptr;
    m_Lungs = nullptr;
    m_PleuralCavity = nullptr;
    m_Carina = nullptr;
    m_LeftLung = nullptr;
    m_RightLung = nullptr;
    m_LeftAlveoli = nullptr;
    m_RightAlveoli = nullptr;
    m_AnatomicDeadSpace = nullptr;
    m_Alveoli = nullptr;
    m_LeftAlveoli = nullptr;
    m_RightAlveoli = nullptr;
    m_AnatomicDeadSpace = nullptr;
    m_AlveolarDeadSpace = nullptr;
    m_LeftAlveolarDeadSpace = nullptr;
    m_RightAlveolarDeadSpace = nullptr;
    m_CarinaO2 = nullptr;
    m_LeftAlveoliO2 = nullptr;
    m_RightAlveoliO2 = nullptr;
    // Mechanical Ventilation
    m_MechanicalVentilationConnection = nullptr;
    m_MechanicalVentilationAerosolConnection = nullptr;
    // Aerosol
    m_AerosolAirway = nullptr;
    m_AerosolCarina = nullptr;
    m_AerosolLeftAnatomicDeadSpace = nullptr;
    m_AerosolLeftAlveolarDeadSpace = nullptr;
    m_AerosolLeftAlveoli = nullptr;
    m_AerosolRightAnatomicDeadSpace = nullptr;
    m_AerosolRightAlveolarDeadSpace = nullptr;
    m_AerosolRightAlveoli = nullptr;
    m_LeftLungExtravascular = nullptr;
    m_RightLungExtravascular = nullptr;
    // Cardiovascular
    m_LeftPulmonaryCapillaries = nullptr;
    m_RightPulmonaryCapillaries = nullptr;
    m_AortaO2 = nullptr;
    m_AortaCO2 = nullptr;
    m_LeftPleuralCavity = nullptr;
    m_RightPleuralCavity = nullptr;

    // Circuits
    m_RespiratoryCircuit = nullptr;
    // Nodes
    m_AirwayNode = nullptr;
    m_LeftPleuralNode = nullptr;
    m_RespiratoryMuscleNode = nullptr;
    m_RightPleuralNode = nullptr;
    m_AmbientNode = nullptr;
    m_StomachNode = nullptr;
    // Paths
    m_CarinaToLeftAnatomicDeadSpace = nullptr;
    m_CarinaToRightAnatomicDeadSpace = nullptr;
    m_LeftAnatomicDeadSpaceToLeftAlveolarDeadSpace = nullptr;
    m_RightAnatomicDeadSpaceToRightAlveolarDeadSpace = nullptr;
    m_LeftAlveolarDeadSpaceToLeftAlveoli = nullptr;
    m_RightAlveolarDeadSpaceToRightAlveoli = nullptr;
    m_LeftPleuralToRespiratoryMuscle = nullptr;
    m_RightPleuralToRespiratoryMuscle = nullptr;
    m_DriverPressurePath = nullptr;
    m_AirwayToPharynx = nullptr;
    m_PharynxToEnvironment = nullptr;
    m_PharynxToCarina = nullptr;
    m_AirwayToStomach = nullptr;
    m_EnvironmentToLeftChestLeak = nullptr;
    m_EnvironmentToRightChestLeak = nullptr;
    m_LeftAlveoliLeakToLeftPleural = nullptr;
    m_RightAlveoliLeakToRightPleural = nullptr;
    m_LeftNeedleToLeftPleural = nullptr;
    m_RightNeedleToRightPleural = nullptr;
    m_LeftAlveoliToLeftPleuralConnection = nullptr;
    m_RightAlveoliToRightPleuralConnection = nullptr;
    m_LeftPulmonaryCapillary = nullptr;
    m_RightPulmonaryCapillary = nullptr;
    m_ConnectionToAirway = nullptr;
    m_GroundToConnection = nullptr;
    // Substance
    m_Oversedation = nullptr;

    m_AlveoliVolumeIncrement_L.clear();
    m_TopBreathAcinarZoneVolumes_L.clear();
    m_BottomBreathAcinarZoneVolumes_L.clear();
    m_PreviousShuntScalingFactor.clear();

    //Running Averages
    m_BloodPHRunningAverage->Invalidate();
    m_MeanAirwayPressure_cmH2O->Invalidate();
    m_ArterialO2RunningAverage_mmHg->Invalidate();
    m_ArterialCO2RunningAverage_mmHg->Invalidate();
    m_MixedExpiredCO2RunningAverage_mmHg->Invalidate();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Initializes system properties to valid homeostatic values.
  ///
  /// \details
  /// For stabilization only!
  /// Called AFTER Setup when stabilizing a new patient
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::Initialize()
  {
    Model::Initialize();

    //Vital signs
    m_NotBreathing = false;
    m_TopBreathTotalVolume_L = 0.0;
    m_LastCardiacCycleBloodPH = 7.4;
    m_TopCarinaO2 = 0.2;
    m_TopBreathElapsedTime_min = 0.0;
    m_BottomBreathElapsedTime_min = 0.0;
    m_BottomBreathTotalVolume_L = 0.0;
    m_BottomBreathAlveoliPressure_cmH2O = 0.0;
    m_BottomBreathAirwayPressure_cmH2O = 0.0;
    m_PeakAlveolarPressure_cmH2O = 0.0;
    m_MaximalAlveolarPressure_cmH2O = 0.0;
    //Compliances
    m_PreviousLeftLungPressureDiff_cmH2O = 0.0;
    m_PreviousRightLungPressureDiff_cmH2O = 0.0;
    m_PreviousLeftLungVolume_L = 0.0;
    m_PreviousRightLungVolume_L = 0.0;
    m_PreviousLeftPleuralPressureDiff_cmH2O = 0.0;
    m_PreviousRightPleuralPressureDiff_cmH2O = 0.0;
    m_PreviousLeftPleuralVolume_L = 0.0;
    m_PreviousRightPleuralVolume_L = 0.0;

    //Driver
    m_ElapsedBreathingCycleTime_min = 0.0;
    m_TopBreathElapsedTime_min = 0.0;
    m_BreathingCycle = false;  
    m_VentilationFrequency_Per_min = m_data.GetCurrentPatient().GetRespirationRateBaseline(FrequencyUnit::Per_min);
    m_VentilationPeriod_s = 60.0 / m_VentilationFrequency_Per_min;
    m_BreathingCycleTime_s = m_VentilationPeriod_s + m_data.GetTimeStep_s(); //Make the engine start at the beginning of a breath
    m_DriverPressure_cmH2O = 0.0;
    m_VentilationToTidalVolumeSlope = 30.0;
    //The peak driver pressure is the pressure above the default pressure
    m_PeakInspiratoryPressure_cmH2O = 0.0;
    m_PeakExpiratoryPressure_cmH2O = 0.0;
    m_PreviousTargetAlveolarVentilation_L_Per_min = m_data.GetCurrentPatient().GetTidalVolumeBaseline(VolumeUnit::L) * m_VentilationFrequency_Per_min;
    m_PreviousDyspneaSeverity = 0.0;
    m_MechanoreceptorsDyspneaFactor = 0.0;
    m_AppliedMechanoreceptorsDyspneaFactor = 0.0;

    m_IERatioScaleFactor = 1.0;

    m_LeftAlveoliDecrease_L = 0;
    m_RightAlveoliDecrease_L = 0;

    //System data
    GetTotalLungVolume().Set(m_data.GetCurrentPatient().GetFunctionalResidualCapacity());
    GetRelativeTotalLungVolume().SetValue(0.0, VolumeUnit::L);
    double TidalVolume_L = m_data.GetCurrentPatient().GetTidalVolumeBaseline(VolumeUnit::L);
    double RespirationRate_Per_min = m_data.GetCurrentPatient().GetRespirationRateBaseline(FrequencyUnit::Per_min);
    GetTidalVolume().SetValue(TidalVolume_L, VolumeUnit::L);
    GetExpiratoryTidalVolume().SetValue(TidalVolume_L, VolumeUnit::L);
    GetInspiratoryTidalVolume().SetValue(TidalVolume_L, VolumeUnit::L);
    GetRespirationRate().SetValue(RespirationRate_Per_min, FrequencyUnit::Per_min);
    GetInspiratoryExpiratoryRatio().SetValue(0.5);
    GetHorowitzIndex().SetValue(452.0, PressureUnit::mmHg);
    GetFractionOfInspiredOxygen().SetValue(0.21);
    GetSaturationAndFractionOfInspiredOxygenRatio().SetValue(0.0);
    GetOxygenationIndex().SetValue(0.0);
    GetOxygenSaturationIndex().SetValue(0.0, PressureUnit::cmH2O);
    GetAirwayPressure().SetValue(0.0, PressureUnit::cmH2O);
    GetMeanAirwayPressure().SetValue(0.0, PressureUnit::cmH2O);
    GetExtrinsicPositiveEndExpiratoryPressure().SetValue(0.0, PressureUnit::cmH2O);
    GetIntrinsicPositiveEndExpiratoryPressure().SetValue(0.0, PressureUnit::cmH2O);
    GetTotalPositiveEndExpiratoryPressure().SetValue(0.0, PressureUnit::cmH2O);
    GetInspiratoryFlow().SetValue(0.0, VolumePerTimeUnit::L_Per_s);
    GetExpiratoryFlow().SetValue(0.0, VolumePerTimeUnit::L_Per_s);
    GetPhysiologicDeadSpaceTidalVolumeRatio().SetValue(0.0);
    GetPhysiologicShuntFraction().SetValue(0.0);
    GetClinicalPhysiologicDeadSpaceTidalVolumeRatio().SetValue(0.0);
    GetVentilationPerfusionRatio().SetValue(0.0);

    double AnatomicDeadSpace_L = m_AnatomicDeadSpace->GetVolume(VolumeUnit::L);
    double AlveolarDeadSpace_L = m_AlveolarDeadSpace->GetVolume(VolumeUnit::L);
    GetAnatomicDeadSpace().SetValue(AnatomicDeadSpace_L, VolumeUnit::L);
    GetAlveolarDeadSpace().SetValue(AlveolarDeadSpace_L, VolumeUnit::L);
    GetPhysiologicDeadSpace().SetValue(AnatomicDeadSpace_L + AlveolarDeadSpace_L, VolumeUnit::L);


    GetTotalAlveolarVentilation().SetValue(RespirationRate_Per_min * TidalVolume_L, VolumePerTimeUnit::L_Per_min);
    GetTotalPulmonaryVentilation().SetValue(RespirationRate_Per_min * TidalVolume_L, VolumePerTimeUnit::L_Per_min);
    GetTotalDeadSpaceVentilation().SetValue(AnatomicDeadSpace_L * RespirationRate_Per_min, VolumePerTimeUnit::L_Per_min);
    GetSpecificVentilation().SetValue(0.21);

    GetEndTidalCarbonDioxideFraction().SetValue(0.0827);
    GetEndTidalCarbonDioxidePressure().SetValue(0.0, PressureUnit::mmHg);
    GetEndTidalOxygenFraction().SetValue(0.0);
    GetEndTidalOxygenPressure().SetValue(0.0, PressureUnit::mmHg);

    GetRespiratoryMusclePressure().SetValue(0.0, PressureUnit::cmH2O);
    GetIntrapleuralPressure().SetValue(0.0, PressureUnit::cmH2O);
    GetIntrapulmonaryPressure().SetValue(0.0, PressureUnit::cmH2O);
    GetTransrespiratoryPressure().SetValue(0.0, PressureUnit::cmH2O);
    GetTransairwayPressure().SetValue(0.0, PressureUnit::cmH2O);
    GetTranspulmonaryPressure().SetValue(0.0, PressureUnit::cmH2O);
    GetTransalveolarPressure().SetValue(0.0, PressureUnit::cmH2O);
    GetTransthoracicPressure().SetValue(0.0, PressureUnit::cmH2O);
    GetTransChestWallPressure().SetValue(0.0, PressureUnit::cmH2O);
    GetTransMusclePressure().SetValue(0.0, PressureUnit::cmH2O);

    GetRespiratoryCompliance().SetValue(0.1, VolumePerPressureUnit::L_Per_cmH2O);
    GetLungCompliance().SetValue(0.1, VolumePerPressureUnit::L_Per_cmH2O);
    GetChestWallCompliance().SetValue(0.2, VolumePerPressureUnit::L_Per_cmH2O);
    GetRespiratoryElastance().SetValue(1.0 / 0.1, PressurePerVolumeUnit::cmH2O_Per_L);

    GetTotalRespiratoryModelCompliance().SetValue(0.1, VolumePerPressureUnit::L_Per_cmH2O);
    GetTotalRespiratoryModelInspiratoryResistance().SetValue(1.5, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    GetTotalRespiratoryModelExpiratoryResistance().SetValue(1.5, PressureTimePerVolumeUnit::cmH2O_s_Per_L);

    GetInspiratoryRespiratoryResistance().SetValue(1.5, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    GetExpiratoryRespiratoryResistance().SetValue(1.5, PressureTimePerVolumeUnit::cmH2O_s_Per_L);

    // Muscle Pressure Waveform
    m_InspiratoryRiseFraction = 0;
    m_InspiratoryHoldFraction = 0;
    m_InspiratoryReleaseFraction = 0;
    m_InspiratoryToExpiratoryPauseFraction = 0;
    m_ExpiratoryRiseFraction = 0;
    m_ExpiratoryHoldFraction = 0;
    m_ExpiratoryReleaseFraction = 0;
    m_ResidueFraction = 0;

    // Disease State
    m_LeftAlveoliDecrease_L = 0;
    m_RightAlveoliDecrease_L = 0;

    //Positive Pressure Ventilation
    m_PositivePressureVentilation = false;

    //Conscious Respiration
    m_ActiveConsciousRespirationCommand = false;

    //Overrides
    m_RespiratoryResistanceOverride_cmH2O_s_Per_L = -1.0;
    m_RespiratoryComplianceOverride_L_Per_cmH2O = -1.0;

    if (m_data.HasCardiovascular())
    {
      m_ArterialO2PartialPressure_mmHg = m_AortaO2->GetPartialPressure(PressureUnit::mmHg);
      m_ArterialCO2PartialPressure_mmHg = m_AortaCO2->GetPartialPressure(PressureUnit::mmHg);
    }

    //Initialize the initial patient diffusion surfice area on each alveoli compartment
    double alveoliDiffusionArea_cm2 = m_data.GetInitialPatient().GetAlveoliSurfaceArea(AreaUnit::cm2);
    double totalBaselineAlveoliVolume_L = 0.0;
    for (auto& itr : m_LungComponents)
    {
      LungComponent& cpt = itr.second;
      totalBaselineAlveoliVolume_L += cpt.AlveoliCompartment->GetVolume(VolumeUnit::L);
    }

    for (auto& itr : m_LungComponents)
    {
      LungComponent& cpt = itr.second;
      double alveoliVolumeBaseline_L = cpt.AlveoliCompartment->GetVolume(VolumeUnit::L);
      double alveoliVolumeRatio = alveoliVolumeBaseline_L / totalBaselineAlveoliVolume_L;
      cpt.AlveoliCompartment->GetDiffusionSurfaceArea().SetValue(alveoliDiffusionArea_cm2 * alveoliVolumeRatio, AreaUnit::cm2);
    }

    m_TopBreathAcinarZoneVolumes_L.resize(m_LungComponents.size(), 0.0);
    m_BottomBreathAcinarZoneVolumes_L.resize(m_LungComponents.size(), 0.0);
    m_AlveoliVolumeIncrement_L.resize(m_LungComponents.size(), 0.0);
    m_PreviousShuntScalingFactor.resize(m_LungComponents.size(), 0.0);

    //Get the substances to a good starting point
    TuneCircuit();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Initializes parameters for Respiratory Class
  ///
  /// \details
  /// Called during both State loading and Patient Stabilization
  /// Pull and setup up our data (can be from other systems)
  /// Initialize will be called after this and can overwrite any of this data (only if stabilizing)
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::SetUp()
  {
    // Grab the modifiers pointer from the action manager
    m_MechanicsModifiers = &m_data.GetActions().GetPatientActions().GetRespiratoryMechanicsModification().GetModifiers();
    m_MechanicsModifiers->Activate(); // Ensure all multipliers have a value so we can write cleaner code

    //Patient
    m_PatientActions = &m_data.GetActions().GetPatientActions();
    //Driver
    m_MaxDriverPressure_cmH2O = -50.0;
    m_AverageLocalTissueBronchodilationEffects = 0.0;

    m_TopBreathAcinarZoneVolumes_L.clear();
    m_BottomBreathAcinarZoneVolumes_L.clear();

    //Configuration parameters
    m_DefaultOpenResistance_cmH2O_s_Per_L = m_data.GetConfiguration().GetDefaultOpenFlowResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    m_DefaultClosedResistance_cmH2O_s_Per_L = m_data.GetConfiguration().GetDefaultClosedFlowResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    m_RespOpenResistance_cmH2O_s_Per_L = m_data.GetConfiguration().GetRespiratoryOpenResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    m_RespClosedResistance_cmH2O_s_Per_L = m_data.GetConfiguration().GetRespiratoryClosedResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    m_PeripheralControlGainConstant = m_data.GetConfiguration().GetPeripheralVentilatoryControllerGain();
    m_CentralControlGainConstant = m_data.GetConfiguration().GetCentralVentilatoryControllerGain();
    m_VentilationTidalVolumeIntercept = m_data.GetConfiguration().GetVentilationTidalVolumeIntercept(VolumeUnit::L);
    m_VentilatoryOcclusionPressure_cmH2O = m_data.GetConfiguration().GetVentilatoryOcclusionPressure(PressureUnit::cmH2O); //This increases the absolute max driver pressure
    m_MinimumAllowableTidalVolume_L = m_data.GetConfiguration().GetMinimumAllowableTidalVolume(VolumeUnit::L);
    m_MinimumAllowableInpiratoryAndExpiratoryPeriod_s = m_data.GetConfiguration().GetMinimumAllowableInpiratoryAndExpiratoryPeriod(TimeUnit::s);
    //Compartments
    m_Environment = m_data.GetCompartments().GetGasCompartment(pulse::EnvironmentCompartment::Ambient);
    m_AerosolAirway = m_data.GetCompartments().GetLiquidCompartment(pulse::PulmonaryCompartment::Airway);
    m_AerosolCarina = m_data.GetCompartments().GetLiquidCompartment(pulse::PulmonaryCompartment::Carina);
    m_AerosolLeftAnatomicDeadSpace = m_data.GetCompartments().GetLiquidCompartment(pulse::PulmonaryCompartment::LeftAnatomicDeadSpace);
    m_AerosolLeftAlveolarDeadSpace = m_data.GetCompartments().GetLiquidCompartment(pulse::PulmonaryCompartment::LeftAlveolarDeadSpace);
    m_AerosolLeftAlveoli = m_data.GetCompartments().GetLiquidCompartment(pulse::PulmonaryCompartment::LeftAlveoli);
    m_AerosolRightAnatomicDeadSpace = m_data.GetCompartments().GetLiquidCompartment(pulse::PulmonaryCompartment::RightAnatomicDeadSpace);
    m_AerosolRightAlveolarDeadSpace = m_data.GetCompartments().GetLiquidCompartment(pulse::PulmonaryCompartment::RightAlveolarDeadSpace);
    m_AerosolRightAlveoli = m_data.GetCompartments().GetLiquidCompartment(pulse::PulmonaryCompartment::RightAlveoli);
    m_Lungs = m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::Lungs);
    m_LeftLung = m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::LeftLung);
    m_RightLung = m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::RightLung);
    m_PleuralCavity = m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::PleuralCavity);
    m_LeftPleuralCavity = m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::LeftPleuralCavity);
    m_RightPleuralCavity = m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::RightPleuralCavity);
    m_LeftLungExtravascular = m_data.GetCompartments().GetLiquidCompartment(pulse::ExtravascularCompartment::LeftLungIntracellular);
    m_RightLungExtravascular = m_data.GetCompartments().GetLiquidCompartment(pulse::ExtravascularCompartment::RightLungIntracellular);
    m_Carina = m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::Carina);
    m_CarinaO2 = m_Carina->GetSubstanceQuantity(m_data.GetSubstances().GetO2());
    m_LeftAlveoli = m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::LeftAlveoli);
    m_RightAlveoli = m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::RightAlveoli);
    m_AnatomicDeadSpace = m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::AnatomicDeadSpace);
    m_AlveolarDeadSpace = m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::AlveolarDeadSpace);
    m_LeftAlveolarDeadSpace = m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::LeftAlveolarDeadSpace);
    m_RightAlveolarDeadSpace = m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::RightAlveolarDeadSpace);
    m_Alveoli = m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::Alveoli);
    m_LeftAlveoliO2 = m_LeftAlveoli->GetSubstanceQuantity(m_data.GetSubstances().GetO2());
    m_RightAlveoliO2 = m_RightAlveoli->GetSubstanceQuantity(m_data.GetSubstances().GetO2());
    // Compartments for aerosol effects
    m_AerosolEffects.clear();
    m_AerosolEffects.push_back(m_data.GetCompartments().GetLiquidCompartment(pulse::PulmonaryCompartment::Carina));
    m_AerosolEffects.push_back(m_data.GetCompartments().GetLiquidCompartment(pulse::PulmonaryCompartment::LeftAlveoli));
    m_AerosolEffects.push_back(m_data.GetCompartments().GetLiquidCompartment(pulse::PulmonaryCompartment::LeftAnatomicDeadSpace));
    m_AerosolEffects.push_back(m_data.GetCompartments().GetLiquidCompartment(pulse::PulmonaryCompartment::LeftAlveolarDeadSpace));
    m_AerosolEffects.push_back(m_data.GetCompartments().GetLiquidCompartment(pulse::PulmonaryCompartment::RightAlveoli));
    m_AerosolEffects.push_back(m_data.GetCompartments().GetLiquidCompartment(pulse::PulmonaryCompartment::RightAnatomicDeadSpace));
    m_AerosolEffects.push_back(m_data.GetCompartments().GetLiquidCompartment(pulse::PulmonaryCompartment::RightAlveolarDeadSpace));
    //Circuits
    m_RespiratoryCircuit = &m_data.GetCircuits().GetRespiratoryCircuit();
    //Nodes
    m_AirwayNode = m_RespiratoryCircuit->GetNode(pulse::RespiratoryNode::Airway);
    m_LeftPleuralNode = m_RespiratoryCircuit->GetNode(pulse::RespiratoryNode::LeftPleural);
    m_RespiratoryMuscleNode = m_RespiratoryCircuit->GetNode(pulse::RespiratoryNode::RespiratoryMuscle);
    m_RightPleuralNode = m_RespiratoryCircuit->GetNode(pulse::RespiratoryNode::RightPleural);
    m_AmbientNode = m_RespiratoryCircuit->GetNode(pulse::EnvironmentNode::Ambient);
    m_StomachNode = m_RespiratoryCircuit->GetNode(pulse::RespiratoryNode::Stomach);

    // Side
    // Alveoli Node
    // Dead Space Node
    // Compliance Path
    // Shunt Path
    // Capillary Path
    // Alveoli Compartment
    // Capillary Compartment
    if (m_data.GetConfiguration().UseExpandedRespiratory() == eSwitch::Off)
    {
      // Side
      // Alveoli Node
      // Dead Space Node
      // Resistance Path
      // Compliance Path
      // Shunt Link
      // Arteries Link
      // Veins Link
      // Veins Path
      // Alveoli Compartment
      // Capillary Compartment
      m_LungComponents[eLungCompartment::LeftLung] =
      {
        eSide::Left,
        m_RespiratoryCircuit->GetNode(pulse::RespiratoryNode::LeftAlveoli),
        m_RespiratoryCircuit->GetNode(pulse::RespiratoryNode::LeftAlveolarDeadSpace),
        m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::LeftAnatomicDeadSpaceToLeftAlveolarDeadSpace),
        m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::LeftAlveoliToLeftPleuralConnection),
        m_data.GetCompartments().GetLiquidLink(pulse::VascularLink::LeftPulmonaryArteriesToVeins),
        m_data.GetCompartments().GetLiquidLink(pulse::VascularLink::LeftPulmonaryArteriesToCapillaries),
        m_data.GetCompartments().GetLiquidLink(pulse::VascularLink::LeftPulmonaryCapillariesToVeins),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::LeftPulmonaryArteries1ToLeftPulmonaryVeins1),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::LeftPulmonaryArteries1ToLeftPulmonaryCapillaries1),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::LeftPulmonaryCapillaries1ToLeftPulmonaryVeins1),
        m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::LeftAlveoli),
        m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftPulmonaryCapillaries)
      };
      m_LungComponents[eLungCompartment::RightLung] =
      {
        eSide::Right,
        m_RespiratoryCircuit->GetNode(pulse::RespiratoryNode::RightAlveoli),
        m_RespiratoryCircuit->GetNode(pulse::RespiratoryNode::RightAlveolarDeadSpace),
        m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::RightAnatomicDeadSpaceToRightAlveolarDeadSpace),
        m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::RightAlveoliToRightPleuralConnection),
        m_data.GetCompartments().GetLiquidLink(pulse::VascularLink::RightPulmonaryArteriesToVeins),
        m_data.GetCompartments().GetLiquidLink(pulse::VascularLink::RightPulmonaryArteriesToCapillaries),
        m_data.GetCompartments().GetLiquidLink(pulse::VascularLink::RightPulmonaryCapillariesToVeins),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightPulmonaryArteries1ToRightPulmonaryVeins1),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightPulmonaryArteries1ToRightPulmonaryCapillaries1),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightPulmonaryCapillaries1ToRightPulmonaryVeins1),
        m_data.GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::RightAlveoli),
        m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightPulmonaryCapillaries)
      };
    }
    else
    {
      m_LungComponents[eLungCompartment::RightSuperiorLobeApical] =
      {
        eSide::Right,
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::RightSuperiorLobeApicalAlveoli),
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::RightSuperiorLobeApicalAlveolarDeadSpace),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::RightSuperiorApicoposteriorBronchusToRightSuperiorLobeApicalBronchiole),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::RightSuperiorLobeApicalAlveoliToRightPleuralConnection),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightSuperiorLobeApicalShunt),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightSuperiorLobeApicalPulmonaryCapillariesToRightPulmonaryVeins1),
        m_data.GetCompartments().GetGasCompartment(pulse::ExpandedPulmonaryCompartment::RightSuperiorLobeApicalAlveoli),
        m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightSuperiorLobeApicalPulmonaryCapillaries)
      };
      m_LungComponents[eLungCompartment::RightSuperiorLobePosterior] =
      {
        eSide::Right,
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::RightSuperiorLobePosteriorAlveoli),
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::RightSuperiorLobePosteriorAlveolarDeadSpace),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::RightSuperiorApicoposteriorBronchusToRightSuperiorLobePosteriorBronchiole),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::RightSuperiorLobePosteriorAlveoliToRightPleuralConnection),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightSuperiorLobePosteriorShunt),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightSuperiorLobePosteriorPulmonaryCapillariesToRightPulmonaryVeins1),
        m_data.GetCompartments().GetGasCompartment(pulse::ExpandedPulmonaryCompartment::RightSuperiorLobePosteriorAlveoli),
        m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightSuperiorLobePosteriorPulmonaryCapillaries)
      };
      m_LungComponents[eLungCompartment::RightSuperiorLobeAnterior] =
      {
        eSide::Right,
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::RightSuperiorLobeAnteriorAlveoli),
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::RightSuperiorLobeAnteriorAlveolarDeadSpace),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::RightSuperiorLobarBronchusToRightSuperiorLobeAnteriorBronchiole),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::RightSuperiorLobeAnteriorAlveoliToRightPleuralConnection),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightSuperiorLobeAnteriorShunt),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightSuperiorLobeAnteriorPulmonaryCapillariesToRightPulmonaryVeins1),
        m_data.GetCompartments().GetGasCompartment(pulse::ExpandedPulmonaryCompartment::RightSuperiorLobeAnteriorAlveoli),
        m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightSuperiorLobeAnteriorPulmonaryCapillaries)
      };
      m_LungComponents[eLungCompartment::RightMiddleLobeLateral] =
      {
        eSide::Right,
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::RightMiddleLobeLateralAlveoli),
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::RightMiddleLobeLateralAlveolarDeadSpace),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::RightMiddleLobarBronchusToRightMiddleLobeLateralBronchiole),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::RightMiddleLobeLateralAlveoliToRightPleuralConnection),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightMiddleLobeLateralShunt),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightMiddleLobeLateralPulmonaryCapillariesToRightPulmonaryVeins1),
        m_data.GetCompartments().GetGasCompartment(pulse::ExpandedPulmonaryCompartment::RightMiddleLobeLateralAlveoli),
        m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightMiddleLobeLateralPulmonaryCapillaries)
      };
      m_LungComponents[eLungCompartment::RightMiddleLobeMedial] =
      {
        eSide::Right,
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::RightMiddleLobeMedialAlveoli),
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::RightMiddleLobeMedialAlveolarDeadSpace),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::RightMiddleLobarBronchusToRightMiddleLobeMedialBronchiole),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::RightMiddleLobeMedialAlveoliToRightPleuralConnection),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightMiddleLobeMedialShunt),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightMiddleLobeMedialPulmonaryCapillariesToRightPulmonaryVeins1),
        m_data.GetCompartments().GetGasCompartment(pulse::ExpandedPulmonaryCompartment::RightMiddleLobeMedialAlveoli),
        m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightMiddleLobeMedialPulmonaryCapillaries)
      };
      m_LungComponents[eLungCompartment::RightInferiorLobeSuperior] =
      {
        eSide::Right,
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::RightInferiorLobeSuperiorAlveoli),
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::RightInferiorLobeSuperiorAlveolarDeadSpace),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::RightInferiorLobarBronchus1ToRightInferiorLobeSuperiorBronchiole),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::RightInferiorLobeSuperiorAlveoliToRightPleuralConnection),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightInferiorLobeSuperiorShunt),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightInferiorLobeSuperiorPulmonaryCapillariesToRightPulmonaryVeins1),
        m_data.GetCompartments().GetGasCompartment(pulse::ExpandedPulmonaryCompartment::RightInferiorLobeSuperiorAlveoli),
        m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightInferiorLobeSuperiorPulmonaryCapillaries)
      };
      m_LungComponents[eLungCompartment::RightInferiorLobeMedialBasal] =
      {
        eSide::Right,
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::RightInferiorLobeMedialBasalAlveoli),
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::RightInferiorLobeMedialBasalAlveolarDeadSpace),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::RightInferiorLobarBronchus2ToRightInferiorLobeMedialBasalBronchiole),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::RightInferiorLobeMedialBasalAlveoliToRightPleuralConnection),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightInferiorLobeMedialBasalShunt),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightInferiorLobeMedialBasalPulmonaryCapillariesToRightPulmonaryVeins1),
        m_data.GetCompartments().GetGasCompartment(pulse::ExpandedPulmonaryCompartment::RightInferiorLobeMedialBasalAlveoli),
        m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightInferiorLobeMedialBasalPulmonaryCapillaries)
      };
      m_LungComponents[eLungCompartment::RightInferiorLobeAnteriorBasal] =
      {
        eSide::Right,
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::RightInferiorLobeAnteriorBasalAlveoli),
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::RightInferiorLobeAnteriorBasalAlveolarDeadSpace),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::RightInferiorLobarBronchus3ToRightInferiorLobeAnteriorBasalBronchiole),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::RightInferiorLobeAnteriorBasalAlveoliToRightPleuralConnection),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightInferiorLobeAnteriorBasalShunt),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightInferiorLobeAnteriorBasalPulmonaryCapillariesToRightPulmonaryVeins1),
        m_data.GetCompartments().GetGasCompartment(pulse::ExpandedPulmonaryCompartment::RightInferiorLobeAnteriorBasalAlveoli),
        m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightInferiorLobeAnteriorBasalPulmonaryCapillaries)
      };
      m_LungComponents[eLungCompartment::RightInferiorLobeLateralBasal] =
      {
        eSide::Right,
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::RightInferiorLobeLateralBasalAlveoli),
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::RightInferiorLobeLateralBasalAlveolarDeadSpace),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::RightInferiorLobarBronchus4ToRightInferiorLobeLateralBasalBronchiole),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::RightInferiorLobeLateralBasalAlveoliToRightPleuralConnection),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightInferiorLobeLateralBasalShunt),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightInferiorLobeLateralBasalPulmonaryCapillariesToRightPulmonaryVeins1),
        m_data.GetCompartments().GetGasCompartment(pulse::ExpandedPulmonaryCompartment::RightInferiorLobeLateralBasalAlveoli),
        m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightInferiorLobeLateralBasalPulmonaryCapillaries)
      };
      m_LungComponents[eLungCompartment::RightInferiorLobePosteriorBasal] =
      {
        eSide::Right,
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::RightInferiorLobePosteriorBasalAlveoli),
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::RightInferiorLobePosteriorBasalAlveolarDeadSpace),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::RightInferiorLobarBronchus4ToRightInferiorLobePosteriorBasalBronchiole),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::RightInferiorLobePosteriorBasalAlveoliToRightPleuralConnection),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightInferiorLobePosteriorBasalShunt),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightInferiorLobePosteriorBasalPulmonaryCapillariesToRightPulmonaryVeins1),
        m_data.GetCompartments().GetGasCompartment(pulse::ExpandedPulmonaryCompartment::RightInferiorLobePosteriorBasalAlveoli),
        m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightInferiorLobePosteriorBasalPulmonaryCapillaries)
      };
      m_LungComponents[eLungCompartment::LeftInferiorLobePosteriorBasal] =
      {
        eSide::Left,
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::LeftInferiorLobePosteriorBasalAlveoli),
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::LeftInferiorLobePosteriorBasalAlveolarDeadSpace),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::LeftInferiorLobarBronchus3ToLeftInferiorLobeLateralBasalBronchiole),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::LeftInferiorLobePosteriorBasalAlveoliToLeftPleuralConnection),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::LeftInferiorLobePosteriorBasalShunt),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::LeftInferiorLobePosteriorBasalPulmonaryCapillariesToLeftPulmonaryVeins1),
        m_data.GetCompartments().GetGasCompartment(pulse::ExpandedPulmonaryCompartment::LeftInferiorLobePosteriorBasalAlveoli),
        m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftInferiorLobePosteriorBasalPulmonaryCapillaries)
      };
      m_LungComponents[eLungCompartment::LeftInferiorLobeLateralBasal] =
      {
        eSide::Left,
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::LeftInferiorLobeLateralBasalAlveoli),
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::LeftInferiorLobeLateralBasalAlveolarDeadSpace),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::LeftInferiorLobarBronchus3ToLeftInferiorLobePosteriorBasalBronchiole),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::LeftInferiorLobeLateralBasalAlveoliToLeftPleuralConnection),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::LeftInferiorLobeLateralBasalShunt),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::LeftInferiorLobeLateralBasalPulmonaryCapillariesToLeftPulmonaryVeins1),
        m_data.GetCompartments().GetGasCompartment(pulse::ExpandedPulmonaryCompartment::LeftInferiorLobeLateralBasalAlveoli),
        m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftInferiorLobeLateralBasalPulmonaryCapillaries)
      };
      m_LungComponents[eLungCompartment::LeftInferiorLobeAnteromedialBasal] =
      {
        eSide::Left,
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::LeftInferiorLobeAnteromedialBasalAlveoli),
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::LeftInferiorLobeAnteromedialBasalAlveolarDeadSpace),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::LeftInferiorLobarBronchus2ToLeftInferiorLobeAnteromedialBasalBronchiole),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::LeftInferiorLobeAnteromedialBasalAlveoliToLeftPleuralConnection),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::LeftInferiorLobeAnteromedialBasalShunt),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::LeftInferiorLobeAnteromedialBasalPulmonaryCapillariesToLeftPulmonaryVeins1),
        m_data.GetCompartments().GetGasCompartment(pulse::ExpandedPulmonaryCompartment::LeftInferiorLobeAnteromedialBasalAlveoli),
        m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftInferiorLobeAnteromedialBasalPulmonaryCapillaries)
      };
      m_LungComponents[eLungCompartment::LeftInferiorLobeSuperior] =
      {
        eSide::Left,
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::LeftInferiorLobeSuperiorAlveoli),
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::LeftInferiorLobeSuperiorAlveolarDeadSpace),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::LeftInferiorLobarBronchus1ToLeftInferiorLobeSuperiorBronchiole),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::LeftInferiorLobeSuperiorAlveoliToLeftPleuralConnection),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::LeftInferiorLobeSuperiorShunt),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::LeftInferiorLobeSuperiorPulmonaryCapillariesToLeftPulmonaryVeins1),
        m_data.GetCompartments().GetGasCompartment(pulse::ExpandedPulmonaryCompartment::LeftInferiorLobeSuperiorAlveoli),
        m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftInferiorLobeSuperiorPulmonaryCapillaries)
      };
      m_LungComponents[eLungCompartment::LeftSuperiorLobeInferiorLingula] =
      {
        eSide::Left,
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::LeftSuperiorLobeInferiorLingulaAlveoli),
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::LeftSuperiorLobeInferiorLingulaAlveolarDeadSpace),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::LeftLingularBonchusToLeftSuperiorLobeInferiorLingulaBronchiole),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::LeftSuperiorLobeInferiorLingulaAlveoliToLeftPleuralConnection),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::LeftSuperiorLobeInferiorLingulaShunt),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::LeftSuperiorLobeInferiorLingulaPulmonaryCapillariesToLeftPulmonaryVeins1),
        m_data.GetCompartments().GetGasCompartment(pulse::ExpandedPulmonaryCompartment::LeftSuperiorLobeInferiorLingulaAlveoli),
        m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftSuperiorLobeInferiorLingulaPulmonaryCapillaries)
      };
      m_LungComponents[eLungCompartment::LeftSuperiorLobeSuperiorLingula] =
      {
        eSide::Left,
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::LeftSuperiorLobeSuperiorLingulaAlveoli),
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::LeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::LeftLingularBonchusToLeftSuperiorLobeSuperiorLingulaBronchiole),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::LeftSuperiorLobeSuperiorLingulaAlveoliToLeftPleuralConnection),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::LeftSuperiorLobeSuperiorLingulaShunt),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::LeftSuperiorLobeSuperiorLingulaPulmonaryCapillariesToLeftPulmonaryVeins1),
        m_data.GetCompartments().GetGasCompartment(pulse::ExpandedPulmonaryCompartment::LeftSuperiorLobeSuperiorLingulaAlveoli),
        m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries)
      };
      m_LungComponents[eLungCompartment::LeftSuperiorLobeAnterior] =
      {
        eSide::Left,
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::LeftSuperiorLobeAnteriorAlveoli),
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::LeftSuperiorLobeAnteriorAlveolarDeadSpace),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::LeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeAnteriorBronchiole),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::LeftSuperiorLobeAnteriorAlveoliToLeftPleuralConnection),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::LeftSuperiorLobeAnteriorShunt),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::LeftSuperiorLobeAnteriorPulmonaryCapillariesToLeftPulmonaryVeins1),
        m_data.GetCompartments().GetGasCompartment(pulse::ExpandedPulmonaryCompartment::LeftSuperiorLobeAnteriorAlveoli),
        m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftSuperiorLobeAnteriorPulmonaryCapillaries)
      };
      m_LungComponents[eLungCompartment::LeftSuperiorLobeApicoposterior] =
      {
        eSide::Left,
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::LeftSuperiorLobeApicoposteriorAlveoli),
        m_RespiratoryCircuit->GetNode(pulse::ExpandedRespiratoryNode::LeftSuperiorLobeApicoposteriorAlveolarDeadSpace),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::LeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeApicoposteriorBronchiole),
        m_RespiratoryCircuit->GetPath(pulse::ExpandedRespiratoryPath::LeftSuperiorLobeApicoposteriorAlveoliToLeftPleuralConnection),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::LeftSuperiorLobeApicoposteriorShunt),
        m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::LeftSuperiorLobeApicoposteriorPulmonaryCapillariesToLeftPulmonaryVeins1),
        m_data.GetCompartments().GetGasCompartment(pulse::ExpandedPulmonaryCompartment::LeftSuperiorLobeApicoposteriorAlveoli),
        m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftSuperiorLobeApicoposteriorPulmonaryCapillaries)
      };
    }

    //Paths
    m_CarinaToLeftAnatomicDeadSpace = m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::CarinaToLeftAnatomicDeadSpace);
    m_CarinaToRightAnatomicDeadSpace = m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::CarinaToRightAnatomicDeadSpace);
    m_LeftAnatomicDeadSpaceToLeftAlveolarDeadSpace = m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::LeftAnatomicDeadSpaceToLeftAlveolarDeadSpace);
    m_RightAnatomicDeadSpaceToRightAlveolarDeadSpace = m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::RightAnatomicDeadSpaceToRightAlveolarDeadSpace);
    m_LeftAlveolarDeadSpaceToLeftAlveoli = m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::LeftAlveolarDeadSpaceToLeftAlveoli);
    m_RightAlveolarDeadSpaceToRightAlveoli = m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::RightAlveolarDeadSpaceToRightAlveoli);

    m_RightPleuralToRespiratoryMuscle = m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::RightPleuralToRespiratoryMuscle);
    m_LeftPleuralToRespiratoryMuscle = m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::LeftPleuralToRespiratoryMuscle);
    m_DriverPressurePath = m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::EnvironmentToRespiratoryMuscle);
    m_AirwayToPharynx = m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::AirwayToPharynx);
    m_PharynxToCarina = m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::PharynxToCarina);
    m_PharynxToEnvironment = m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::PharynxToEnvironment);
    m_AirwayToStomach = m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::AirwayToStomach);
    m_EnvironmentToLeftChestLeak = m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::EnvironmentToLeftChestLeak);
    m_EnvironmentToRightChestLeak = m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::EnvironmentToRightChestLeak);
    m_LeftAlveoliLeakToLeftPleural = m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::LeftAlveoliLeakToLeftPleural);
    m_RightAlveoliLeakToRightPleural = m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::RightAlveoliLeakToRightPleural);
    m_LeftNeedleToLeftPleural = m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::LeftNeedleToLeftPleural);
    m_RightNeedleToRightPleural = m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::RightNeedleToRightPleural);
    m_RightAlveoliToRightPleuralConnection = m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::RightAlveoliToRightPleuralConnection);
    m_LeftAlveoliToLeftPleuralConnection = m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::LeftAlveoliToLeftPleuralConnection);
    m_ConnectionToAirway = m_data.GetCircuits().GetRespiratoryAndMechanicalVentilationCircuit().GetPath(pulse::MechanicalVentilationPath::ConnectionToAirway);
    m_GroundToConnection = m_data.GetCircuits().GetRespiratoryAndMechanicalVentilationCircuit().GetPath(pulse::MechanicalVentilationPath::GroundToConnection);
    m_LeftRespirtoryLeak = m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::EnvironmentToLeftPleural);
    m_RightRespirtoryLeak = m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::EnvironmentToRightPleural);
    m_LeftCardiovascularLeak = m_data.GetCircuits().GetActiveCardiovascularCircuit().GetPath(pulse::CardiovascularPath::LeftPulmonaryVeinsLeak1ToGround);
    m_RightCardiovascularLeak = m_data.GetCircuits().GetActiveCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightPulmonaryVeinsLeak1ToGround);

    //Mechanical Ventilation Compartments
    m_MechanicalVentilationConnection = m_data.GetCompartments().GetGasCompartment(pulse::MechanicalVentilationCompartment::Connection);
    m_MechanicalVentilationAerosolConnection = m_data.GetCompartments().GetLiquidCompartment(pulse::MechanicalVentilationCompartment::Connection);

    if (m_data.HasCardiovascular())
    {
      /// \todo figure out how to modify these resistances without getting the cv circuit - maybe add a parameter, like baroreceptors does
      //Venous Return
      m_RightPulmonaryCapillary = m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightPulmonaryCapillaries1ToRightPulmonaryVeins1);
      m_LeftPulmonaryCapillary = m_data.GetCircuits().GetCardiovascularCircuit().GetPath(pulse::CardiovascularPath::LeftPulmonaryCapillaries1ToLeftPulmonaryVeins1);
      //Pulmonary Shunt

      m_LeftPulmonaryCapillaries = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::LeftPulmonaryCapillaries);
      m_RightPulmonaryCapillaries = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::RightPulmonaryCapillaries);
      SELiquidCompartment* Aorta = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Aorta);
      m_AortaO2 = Aorta->GetSubstanceQuantity(m_data.GetSubstances().GetO2());
      m_AortaCO2 = Aorta->GetSubstanceQuantity(m_data.GetSubstances().GetCO2());
    }

    //Substance - Overdose
    m_Oversedation = m_data.GetSubstances().GetSubstance("Oversedation");
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Respiratory system at steady state
  ///
  /// \details
  /// Initializes respiratory conditions if any are present.
  ///
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::AtSteadyState()
  {
    //Experimentally determined
    double tidalVolumeBaseline_L = GetTidalVolume(VolumeUnit::L);
    //Calculated
    double totalLungCapacity_L = m_data.GetCurrentPatient().GetTotalLungCapacity(VolumeUnit::L);
    double functionalResidualCapacity_L = m_data.GetCurrentPatient().GetFunctionalResidualCapacity(VolumeUnit::L);
    double inspiratoryReserveVolume_L = totalLungCapacity_L - functionalResidualCapacity_L - tidalVolumeBaseline_L;

    std::string typeString;
    if (m_data.GetState() == EngineState::AtInitialStableState)
    {
      typeString = "Initial Stabilization Homeostasis: ";
      //At Resting State
      //Note: Respiratory conditions are applied each timestep to handle combined effects properly

      //Update healthy patient volumes
      //Tidal Volume is experimentally determined and is dependent on other patient settings
      //Inspiratory Reserve Volume is calcualted from Tidal Volume
      m_data.GetCurrentPatient().GetTidalVolumeBaseline().SetValue(tidalVolumeBaseline_L, VolumeUnit::L);
      m_data.GetCurrentPatient().GetInspiratoryReserveVolume().SetValue(inspiratoryReserveVolume_L, VolumeUnit::L);
    }
    else if (m_data.GetState() == EngineState::AtSecondaryStableState)
      typeString = "Secondary Stabilization Homeostasis: ";
    else if (m_data.GetState() == EngineState::Active)
      typeString = "Starting ";

    std::stringstream ss;
    ss << typeString << "Patient tidal volume = " << tidalVolumeBaseline_L << " L.";
    Info(ss);
    ss << typeString << "Patient inspiratory reserve volume = " << inspiratoryReserveVolume_L << " L.";
    Info(ss);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Respiratory system preprocess function
  ///
  /// \details
  /// Calculates the muscle pressure source that drives the respiratory system.
  /// Updates airway resistance to account for changes arising from factors 
  /// like drugs and respiratory insults and interventions. 
  //// Updates the chest wall variable compliance. Handles all respiratory
  /// insults and actions.
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::PreProcess()
  {
    // Look for any known overrides
    for (auto const& [name, modifier] : m_data.GetOverrides())
    {
      if (name == "RespiratoryResistance")
      {
        if (std::isnan(modifier.value))
          m_RespiratoryResistanceOverride_cmH2O_s_Per_L = -1;
        else
          m_RespiratoryResistanceOverride_cmH2O_s_Per_L = Convert(modifier.value, PressureTimePerVolumeUnit::GetCompoundUnit(modifier.unit), PressureTimePerVolumeUnit::cmH2O_s_Per_L);
      }
      else if (name == "RespiratoryCompliance")
      {
        if (std::isnan(modifier.value))
          m_RespiratoryComplianceOverride_L_Per_cmH2O = -1;
        else
          m_RespiratoryComplianceOverride_L_Per_cmH2O = Convert(modifier.value, VolumePerPressureUnit::GetCompoundUnit(modifier.unit), VolumePerPressureUnit::L_Per_cmH2O);
      }
    }

    // Configure Mechanics
    if (m_PatientActions->HasRespiratoryMechanicsConfiguration())
    {
      GetMechanics().ProcessConfiguration(m_PatientActions->GetRespiratoryMechanicsConfiguration());
      m_PatientActions->RemoveRespiratoryMechanicsConfiguration();
    }

    //Positive pressure ventilation
    m_PositivePressureVentilation = 
      (m_data.GetAirwayMode() == eAirwayMode::AnesthesiaMachine ||
      m_data.GetAirwayMode() == eAirwayMode::MechanicalVentilation ||
      m_data.GetAirwayMode() == eAirwayMode::MechanicalVentilator);

    CalculateWork();
    CalculateFatigue();

    UpdateChestWallCompliances();
    UpdateAlveolarCompliances();
    UpdateVolumes();
    if (m_data.GetConfiguration().UseExpandedRespiratory() == eSwitch::Off)
    {
      //jbw - Fix
      UpdateResistances();
    }
    UpdateInspiratoryExpiratoryRatio();
    UpdateDiffusion();
    if (m_data.HasCardiovascular())
    {
      UpdatePulmonaryCapillary();
      UpdatePulmonaryShunt();
    }

    ProcessAerosolSubstances();
    Pneumothorax();
    Hemothorax();

    MechanicalVentilation();
    SupplementalOxygen();

    CalculateDriver();
    ApplyDriver();

    //Do the overrides
    SetRespiratoryResistance();
    SetRespiratoryCompliance();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Respiratory system process function
  ///
  /// \details
  /// Ensures the selection of the respiratory system with or without the anesthesia machine. 
  ///  Handles the integration of the anesthesia machine to the respiratory system when the anesthesia machine is turned on.
  /// The integration of the anesthesia machine to the respiratory system is handled at run time by constructing a combined circuit of 
  /// the respiratory and anesthesia machine.  
  /// Handles lung volume changes during alveolar gas transfer. 
  /// Calculates physiological parameters such as respiration rate, tidal volume and others that belonging to the respiratory system.
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::Process(bool solve_and_transport)
  {
    if (solve_and_transport)
    {
      // Respiration circuit changes based on if Anesthesia Machine is on or off
      // When dynamic intercircuit connections work, we can stash off the respiration circuit in a member variable
      SEFluidCircuit& RespirationCircuit = m_data.GetCircuits().GetActiveRespiratoryCircuit();

      // Calc the circuits
      m_Calculator->Process(RespirationCircuit, m_data.GetTimeStep_s());

      //ModifyPleuralVolume();
      SEGasCompartmentGraph& RespirationGraph = m_data.GetCompartments().GetActiveRespiratoryGraph();
      SELiquidCompartmentGraph& AerosolGraph = m_data.GetCompartments().GetActiveAerosolGraph();

      // Transport substances
      m_GasTransporter->Transport(RespirationGraph, m_data.GetTimeStep_s());
      if (m_AerosolAirway->HasSubstanceQuantities())
        m_AerosolTransporter->Transport(AerosolGraph, m_data.GetTimeStep_s());

  #ifdef DEBUG
      Debugging();
  #endif
    }
    //Update system data
    CalculateVitalSigns();

    if (m_data.GetConfiguration().UseExpandedRespiratory() == eSwitch::Off)
    {
      //jbw - Fix
      ComputeExposedModelParameters();
    }
  }
  void RespiratoryModel::ComputeExposedModelParameters()
  {
    double leftChestWallCompliance_L_Per_cmH2O = m_LeftPleuralToRespiratoryMuscle->GetNextCompliance(VolumePerPressureUnit::L_Per_cmH2O);
    double rightChestWallCompliance_L_Per_cmH2O = m_RightPleuralToRespiratoryMuscle->GetNextCompliance(VolumePerPressureUnit::L_Per_cmH2O);
    double leftLungCompliance_L_Per_cmH2O = m_LeftAlveoliToLeftPleuralConnection->GetNextCompliance(VolumePerPressureUnit::L_Per_cmH2O);
    double rightLungCompliance_L_Per_cmH2O = m_RightAlveoliToRightPleuralConnection->GetNextCompliance(VolumePerPressureUnit::L_Per_cmH2O);
    double leftSideCompliance_L_Per_cmH2O = 1.0 / (1.0 / leftChestWallCompliance_L_Per_cmH2O + 1.0 / leftLungCompliance_L_Per_cmH2O);
    double rightSideCompliance_L_Per_cmH2O = 1.0 / (1.0 / rightChestWallCompliance_L_Per_cmH2O + 1.0 / rightLungCompliance_L_Per_cmH2O);
    double totalCompliance_L_Per_cmH2O = leftSideCompliance_L_Per_cmH2O + rightSideCompliance_L_Per_cmH2O;
    GetTotalRespiratoryModelCompliance().SetValue(totalCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);

    double airwayResistance_cmH2O_s_Per_L = m_PharynxToCarina->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    double leftBronchiResistance_cmH2O_s_Per_L = m_CarinaToLeftAnatomicDeadSpace->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    double rightBronchiResistance_cmH2O_s_Per_L = m_CarinaToRightAnatomicDeadSpace->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    double leftAlveoliDuctResistance_cmH2O_s_Per_L = m_LeftAnatomicDeadSpaceToLeftAlveolarDeadSpace->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    double rightAlveoliDuctResistance_cmH2O_s_Per_L = m_RightAnatomicDeadSpaceToRightAlveolarDeadSpace->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    double leftSideResistance_cmH2O_s_Per_L = leftBronchiResistance_cmH2O_s_Per_L + leftAlveoliDuctResistance_cmH2O_s_Per_L;
    double rightSideResistance_cmH2O_s_Per_L = rightBronchiResistance_cmH2O_s_Per_L + rightAlveoliDuctResistance_cmH2O_s_Per_L;
    double totalDownstreamResistance_cmH2O_s_Per_L = 1.0 / (1.0 / leftSideResistance_cmH2O_s_Per_L + 1.0 / rightSideResistance_cmH2O_s_Per_L);
    double totalResistance_cmH2O_s_Per_L = airwayResistance_cmH2O_s_Per_L + totalDownstreamResistance_cmH2O_s_Per_L;
    if (m_PharynxToCarina->GetFlow(VolumePerTimeUnit::L_Per_s) > 0.0)
    {
      GetTotalRespiratoryModelInspiratoryResistance().SetValue(totalResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    }
    else
    {
      GetTotalRespiratoryModelExpiratoryResistance().SetValue(totalResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Respiratory system postprocess function
  ///
  /// \details
  /// Updates the current values of the gas volume fraction and gas volumes for the nodes in the respiratory circuit 
  /// or the nodes in the combined (respiratory + anesthesia machine) circuit when the anesthesia machine is turned on.
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::PostProcess(bool solve_and_transport)
  {
    if (solve_and_transport)
    {
      // Respiration circuit changes based on if Anesthesia Machine is on or off
      // When dynamic intercircuit connections work, we can stash off the respiration circuit in a member variable
      SEFluidCircuit& RespirationCircuit = m_data.GetCircuits().GetActiveRespiratoryCircuit();
      m_Calculator->PostProcess(RespirationCircuit);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Compute deposited mass, update localized PD effects 
  ///
  /// \details
  /// For each aerosol get the SIDE coefficient to determine deposited mass in each respiratory compartment.
  /// Adjust the resistances between compartments as a function of deposited mass to reach validated data.
  /// Liquid and solid aerosols are handeled here. 
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::ProcessAerosolSubstances()
  {
    m_AverageLocalTissueBronchodilationEffects = 0.0; //No effect

    size_t numAerosols = m_AerosolAirway->GetSubstanceQuantities().size();
    if (numAerosols == 0)
      return;

    double inflammationCoefficient;

    // For this time step
    double airwayDepositied_ug = 0;
    double carinaDepositied_ug = 0;
    double leftDeadSpaceDepositied_ug = 0;
    double leftAlveoliDepositied_ug = 0;
    double rightDeadSpaceDepositied_ug = 0;
    double rightAlveoliDepositied_ug = 0;

    // Total amount deposited (including this time step)
    double carinaTotalDepositied_ug = 0;
    double leftDeadSpaceTotalDepositied_ug = 0;
    double leftAlveoliTotalDepositied_ug = 0;
    double rightDeadSpaceTotalDepositied_ug = 0;
    double rightAlveoliTotalDepositied_ug = 0;

    // Resistance Modifier Sum
    double carinaResistanceModifier=1;
    double leftDeadSpaceResistanceModifier=1;
    double leftAlveoliResistanceModifier=1;
    double rightDeadSpaceResistanceModifier=1;
    double rightAlveoliResistanceModifier=1;

    // Currently, There is no way to clear out depositied particulate out of the respiratory systems.
    // Maybe we could have it to cough or some other excretion related method... 
    
    SELiquidSubstanceQuantity* subQ;
    SELiquidSubstanceQuantity* tSubQ;
    const SizeIndependentDepositionEfficencyCoefficient* SIDECoeff;
    double combinedRightBronchodilationEffects = 0.0;
    double combinedLeftBronchodilationEffects = 0.0;
    for (size_t i=0; i<numAerosols; i++)
    {
      //initialize substance
      subQ = m_AerosolAirway->GetSubstanceQuantities()[i];
      //Adjust inflammation coefficient (scaled down):
      inflammationCoefficient = subQ->GetSubstance().GetAerosolization().GetInflammationCoefficient().GetValue();// Once for each subQ
      inflammationCoefficient *= 0.01;
      //Airway
      SIDECoeff = &m_data.GetSubstances().GetSizeIndependentDepositionEfficencyCoefficient(subQ->GetSubstance());// Once for each subQ
      airwayDepositied_ug = subQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL)*m_AerosolAirway->GetInFlow(VolumePerTimeUnit::mL_Per_s)*m_data.GetTimeStep_s()*SIDECoeff->GetAirway();
      if (airwayDepositied_ug > subQ->GetMass(MassUnit::ug))
      {
        airwayDepositied_ug = subQ->GetMass(MassUnit::ug);
        subQ->GetMass().SetValue(0, MassUnit::ug);
      }
      else
        subQ->GetMass().Increment(-airwayDepositied_ug, MassUnit::ug);
      subQ->Balance(BalanceLiquidBy::Mass);
      subQ->GetMassDeposited().Increment(airwayDepositied_ug, MassUnit::ug);
      //Carina
      subQ = m_AerosolCarina->GetSubstanceQuantities()[i];
      carinaDepositied_ug = subQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL)*m_AerosolCarina->GetInFlow(VolumePerTimeUnit::mL_Per_s)*m_data.GetTimeStep_s()*SIDECoeff->GetCarina();
      if (carinaDepositied_ug > subQ->GetMass(MassUnit::ug))
      {
        carinaDepositied_ug = subQ->GetMass(MassUnit::ug);
        subQ->GetMass().SetValue(0, MassUnit::ug);
      }
      else
        subQ->GetMass().Increment(-carinaDepositied_ug, MassUnit::ug);
      subQ->Balance(BalanceLiquidBy::Mass);
      carinaTotalDepositied_ug = subQ->GetMassDeposited().Increment(carinaDepositied_ug, MassUnit::ug);
      carinaResistanceModifier += carinaTotalDepositied_ug*inflammationCoefficient;
      //Left DeadSpace
      subQ = m_AerosolLeftAnatomicDeadSpace->GetSubstanceQuantities()[i];
      leftDeadSpaceDepositied_ug = subQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL)*m_AerosolLeftAnatomicDeadSpace->GetInFlow(VolumePerTimeUnit::mL_Per_s)*m_data.GetTimeStep_s()*SIDECoeff->GetDeadSpace();
      if (leftDeadSpaceDepositied_ug > subQ->GetMass(MassUnit::ug))
      {
        leftDeadSpaceDepositied_ug = subQ->GetMass(MassUnit::ug);
        subQ->GetMass().SetValue(0, MassUnit::ug);
      }
      else
        subQ->GetMass().Increment(-leftDeadSpaceDepositied_ug, MassUnit::ug);
      subQ->Balance(BalanceLiquidBy::Mass);
      leftDeadSpaceTotalDepositied_ug = subQ->GetMassDeposited().Increment(leftDeadSpaceDepositied_ug, MassUnit::ug);
      leftDeadSpaceResistanceModifier += leftDeadSpaceTotalDepositied_ug*inflammationCoefficient;
      //Left Alveoli
      subQ = m_AerosolLeftAlveoli->GetSubstanceQuantities()[i];
      leftAlveoliDepositied_ug = subQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL)*m_AerosolLeftAlveoli->GetInFlow(VolumePerTimeUnit::mL_Per_s)*m_data.GetTimeStep_s()*SIDECoeff->GetAlveoli();
      if (leftAlveoliDepositied_ug > subQ->GetMass(MassUnit::ug))
      {
        leftAlveoliDepositied_ug = subQ->GetMass(MassUnit::ug);
        subQ->GetMass().SetValue(0, MassUnit::ug);
      }
      else
        subQ->GetMass().Increment(-leftAlveoliDepositied_ug, MassUnit::ug);
      subQ->Balance(BalanceLiquidBy::Mass);
      leftAlveoliTotalDepositied_ug = subQ->GetMassDeposited().Increment(leftAlveoliDepositied_ug, MassUnit::ug);
      leftAlveoliResistanceModifier += leftAlveoliTotalDepositied_ug*inflammationCoefficient;
      //Right DeadSpace
      subQ = m_AerosolRightAnatomicDeadSpace->GetSubstanceQuantities()[i];
      rightDeadSpaceDepositied_ug = subQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL)*m_AerosolRightAnatomicDeadSpace->GetInFlow(VolumePerTimeUnit::mL_Per_s)*m_data.GetTimeStep_s()*SIDECoeff->GetDeadSpace();
      if (rightDeadSpaceDepositied_ug > subQ->GetMass(MassUnit::ug))
      {
        rightDeadSpaceDepositied_ug = subQ->GetMass(MassUnit::ug);
        subQ->GetMass().SetValue(0, MassUnit::ug);
      }
      else
        subQ->GetMass().Increment(-rightDeadSpaceDepositied_ug, MassUnit::ug);
      subQ->Balance(BalanceLiquidBy::Mass);
      rightDeadSpaceTotalDepositied_ug = subQ->GetMassDeposited().Increment(rightDeadSpaceDepositied_ug, MassUnit::ug);
      rightDeadSpaceResistanceModifier += rightDeadSpaceTotalDepositied_ug*inflammationCoefficient;
      //Right Alveoli
      subQ = m_AerosolRightAlveoli->GetSubstanceQuantities()[i];
      rightAlveoliDepositied_ug = subQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL)*m_AerosolRightAlveoli->GetInFlow(VolumePerTimeUnit::mL_Per_s)*m_data.GetTimeStep_s()*SIDECoeff->GetAlveoli();
      if (rightAlveoliDepositied_ug > subQ->GetMass(MassUnit::ug))
      {
        rightAlveoliDepositied_ug = subQ->GetMass(MassUnit::ug);
        subQ->GetMass().SetValue(0, MassUnit::ug);
      }
      else
        subQ->GetMass().Increment(-rightAlveoliDepositied_ug, MassUnit::ug);
      subQ->Balance(BalanceLiquidBy::Mass);
      rightAlveoliTotalDepositied_ug = subQ->GetMassDeposited().Increment(rightAlveoliDepositied_ug, MassUnit::ug);
      rightAlveoliResistanceModifier += rightAlveoliTotalDepositied_ug*inflammationCoefficient;
      
      // Apply the BronchileModifier dilation effect
      // This is all just tuned to Albuterol - it'll work for other substances, and can be tuned using the other parameters (especially BronchioleModifier)
      if (subQ->GetSubstance().GetState() == eSubstance_State::Liquid)
      {
        // Sum the Bronchiole Effects
        // Must be positive
        double bronchioleModifier = subQ->GetSubstance().GetAerosolization().GetBronchioleModifier().GetValue();

        // Diffuse into Tissues
        // We only process mass deposited on the lungs (dead space and alveoli)
        // We do not currently do anything with the mass in the airway and carina
        // Could possibly let it go into the stomach somehow... 
        tSubQ = m_LeftLungExtravascular->GetSubstanceQuantity(subQ->GetSubstance());
        tSubQ->GetMass().Increment(leftDeadSpaceDepositied_ug + leftAlveoliDepositied_ug, MassUnit::ug); tSubQ->Balance(BalanceLiquidBy::Mass);    
        combinedLeftBronchodilationEffects += bronchioleModifier * tSubQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL);

        tSubQ = m_RightLungExtravascular->GetSubstanceQuantity(subQ->GetSubstance());
        tSubQ->GetMass().Increment(rightDeadSpaceDepositied_ug + rightAlveoliDepositied_ug, MassUnit::ug); tSubQ->Balance(BalanceLiquidBy::Mass);
        combinedRightBronchodilationEffects += bronchioleModifier * tSubQ->GetConcentration(MassPerVolumeUnit::ug_Per_mL);
      }
    }

    //We're going to make the bronchodilation effects be based off of Albuterol.
    //Experimentally, 1mg of Albuterol given via a spacer device on an endotracheal tube caused a pulmonary resistance change of ~20% @cite mancebo1991effects.
    //The bronchi are ~30% of the total pulmonary resistance, so we'll make a dilation effect really strong with a normal dose.
    //This was tuned using a standard inhaler dose of 90ug
    m_AverageLocalTissueBronchodilationEffects = (combinedLeftBronchodilationEffects + combinedRightBronchodilationEffects) / 2.0;
    combinedLeftBronchodilationEffects = exp(-32894.0 * combinedLeftBronchodilationEffects);
    combinedRightBronchodilationEffects = exp(-32894.0 * combinedRightBronchodilationEffects);  

    // Change resistances due to deposition
    double dTracheaResistance = m_PharynxToCarina->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L)*carinaResistanceModifier;
    double dLeftAlveoliResistance = m_LeftAnatomicDeadSpaceToLeftAlveolarDeadSpace->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L)*leftAlveoliResistanceModifier;
    double dRightAlveoliResistance = m_RightAnatomicDeadSpaceToRightAlveolarDeadSpace->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L)*rightAlveoliResistanceModifier;

    double dLeftBronchiResistance = m_CarinaToLeftAnatomicDeadSpace->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    double dRightBronchiResistance = m_CarinaToRightAnatomicDeadSpace->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    dLeftBronchiResistance *= leftDeadSpaceResistanceModifier * combinedLeftBronchodilationEffects;
    dRightBronchiResistance *= rightDeadSpaceResistanceModifier * combinedRightBronchodilationEffects;
    dLeftBronchiResistance = LIMIT(dLeftBronchiResistance, m_RespClosedResistance_cmH2O_s_Per_L, m_RespOpenResistance_cmH2O_s_Per_L);
    dRightBronchiResistance = LIMIT(dRightBronchiResistance, m_RespClosedResistance_cmH2O_s_Per_L, m_RespOpenResistance_cmH2O_s_Per_L);

    m_PharynxToCarina->GetNextResistance().SetValue(dTracheaResistance, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    m_CarinaToLeftAnatomicDeadSpace->GetNextResistance().SetValue(dLeftBronchiResistance, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    m_CarinaToRightAnatomicDeadSpace->GetNextResistance().SetValue(dRightBronchiResistance, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    m_LeftAnatomicDeadSpaceToLeftAlveolarDeadSpace->GetNextResistance().SetValue(dLeftAlveoliResistance, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    m_RightAnatomicDeadSpaceToRightAlveolarDeadSpace->GetNextResistance().SetValue(dRightAlveoliResistance, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Modifies the pressure and/or flow at the airway
  ///
  /// \details
  /// Handles the mechanical ventilation action that adds a flow and pressure source to instantaneously
  /// set the respiratory connection (airway) to user specified values.  
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::MechanicalVentilation()
  {
    if (m_PatientActions->HasMechanicalVentilation())
    {
      SEMechanicalVentilation& mv = m_PatientActions->GetMechanicalVentilation();
      // You only get here if action is On
      m_data.SetAirwayMode(eAirwayMode::MechanicalVentilation);

      //Set the substance volume fractions ********************************************
      std::vector<SESubstanceFraction*> gasFractions = mv.GetGasFractions();

      //Reset the substance quantities at the connection
      for (SEGasSubstanceQuantity* subQ : m_MechanicalVentilationConnection->GetSubstanceQuantities())
        subQ->SetToZero();

      //If no gas fractions specified, assume ambient
      if (gasFractions.empty())
      {
        for (auto s : m_Environment->GetSubstanceQuantities())
        {
          m_MechanicalVentilationConnection->GetSubstanceQuantity(s->GetSubstance())->GetVolumeFraction().Set(s->GetVolumeFraction());
        }
      }
      else
      {
        //Has fractions defined
        for (auto f : gasFractions)
        {
          const SESubstance& sub = f->GetSubstance();
          double fraction = f->GetFractionAmount().GetValue();

          //Do this, just in case it's something new
          m_data.GetSubstances().AddActiveSubstance(sub);

          //Now set it on the connection compartment
          //It has a NaN volume, so this will keep the same volume fraction no matter what's going on around it
          m_MechanicalVentilationConnection->GetSubstanceQuantity(sub)->GetVolumeFraction().SetValue(fraction);
        }
      }

      //Set the aerosol concentrations ********************************************
      std::vector<SESubstanceConcentration*> liquidConcentrations = mv.GetAerosols();

      //Reset the substance quantities at the connection
      for (SELiquidSubstanceQuantity* subQ : m_MechanicalVentilationAerosolConnection->GetSubstanceQuantities())
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
          //It has a NaN volume, so this will keep the same volume fraction no matter what's going on around it
          m_MechanicalVentilationAerosolConnection->GetSubstanceQuantity(sub)->GetConcentration().Set(concentration);
        }
      }

      bool stateChange = false;
      //Apply the instantaneous flow ********************************************
      if (mv.HasFlow())
      {
        if (!m_ConnectionToAirway->HasNextFlowSource())
        {
          m_ConnectionToAirway->GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::L_Per_s);
          stateChange = true;
        }
        m_ConnectionToAirway->GetNextFlowSource().Set(mv.GetFlow());
      }
      else
      {
        //If there's no flow specified, we need to remove the flow source
        if (m_ConnectionToAirway->HasNextFlowSource())
        {
          m_ConnectionToAirway->RemoveFlowSource();
          stateChange = true;
        }
      }

      if (stateChange)
      {
        m_data.GetCircuits().GetRespiratoryAndMechanicalVentilationCircuit().StateChange();
      }

      //Apply the instantaneous pressure ********************************************  
      if (mv.HasPressure())
      {
        //This is the pressure above ambient
        m_GroundToConnection->GetNextPressureSource().Set(mv.GetPressure());
      }
      else
      {
        //Pressure is same as ambient
        m_GroundToConnection->GetNextPressureSource().SetValue(0.0, PressureUnit::cmH2O);
      }
    }
    else if (m_data.GetAirwayMode() == eAirwayMode::MechanicalVentilation)
    {
      // Was just turned off
      m_data.SetAirwayMode(eAirwayMode::Free);
      if (m_ConnectionToAirway->HasNextFlowSource())
      {
        m_ConnectionToAirway->GetNextFlowSource().Invalidate();
        m_data.GetCircuits().GetRespiratoryAndMechanicalVentilationCircuit().StateChange();
      }
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Applys supplemental oxygen equipment
  ///
  /// \details
  /// This will provide supplemental oxygen by connecting and updating the circuit and graph for nasal
  /// cannula, simple mask, or nonrebreather mask
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::SupplementalOxygen()
  {
    ///\todo - Maybe this and mechanical ventilation should be broken out to their own class, like anesthesia machine?

    if (!m_PatientActions->HasSupplementalOxygen())
      return;
    SESupplementalOxygen& so = m_PatientActions->GetSupplementalOxygen();

    // Get flow
    double flow_L_Per_min = 0;
    if(so.HasFlow())
      flow_L_Per_min = so.GetFlow(VolumePerTimeUnit::L_Per_min);
    //Get tank pressure node and flow control resistor path
    SEFluidCircuitPath* Tank = nullptr;
    SEFluidCircuitPath* OxygenInlet = nullptr;
    SEFluidCircuit* RespirationCircuit = nullptr;
    switch (so.GetDevice())
    {
      case eSupplementalOxygen_Device::None:
      {
        m_data.SetAirwayMode(eAirwayMode::Free);
        m_PatientActions->RemoveSupplementalOxygen();
        return;
      }
      case eSupplementalOxygen_Device::NasalCannula:
      {
        // This implementation can replace other Supplemental O2 modes
        if (m_data.GetAirwayMode() == eAirwayMode::SimpleMask ||
            m_data.GetAirwayMode() == eAirwayMode::NonRebreatherMask)
          m_data.SetAirwayMode(eAirwayMode::Free);

        m_data.SetAirwayMode(eAirwayMode::NasalCannula);
        if (!so.HasFlow())
        {
          flow_L_Per_min = 3.5;
          so.GetFlow().SetValue(flow_L_Per_min, VolumePerTimeUnit::L_Per_min);
          Info("Supplemental oxygen flow not set. Using default value of " + cdm::to_string(flow_L_Per_min) + " L/min.");
        }
        RespirationCircuit = &m_data.GetCircuits().GetActiveRespiratoryCircuit();
        OxygenInlet = RespirationCircuit->GetPath(pulse::NasalCannulaPath::NasalCannulaOxygenInlet);
        Tank = RespirationCircuit->GetPath(pulse::NasalCannulaPath::NasalCannulaPressure);
        break;
      }
      case eSupplementalOxygen_Device::NonRebreatherMask:
      {
        // This implementation can replace other Supplemental O2 modes
        if (m_data.GetAirwayMode() == eAirwayMode::SimpleMask ||
            m_data.GetAirwayMode() == eAirwayMode::NasalCannula)
          m_data.SetAirwayMode(eAirwayMode::Free);
        m_data.SetAirwayMode(eAirwayMode::NonRebreatherMask);

        if (!so.HasFlow())
        {
          flow_L_Per_min = 10.0;
          so.GetFlow().SetValue(flow_L_Per_min, VolumePerTimeUnit::L_Per_min);
          Info("Supplemental oxygen flow not set. Using default value of " + cdm::to_string(flow_L_Per_min) + " L/min.");
        }
        RespirationCircuit = &m_data.GetCircuits().GetActiveRespiratoryCircuit();
        OxygenInlet = RespirationCircuit->GetPath(pulse::NonRebreatherMaskPath::NonRebreatherMaskOxygenInlet);
        Tank = RespirationCircuit->GetPath(pulse::NonRebreatherMaskPath::NonRebreatherMaskPressure);
        break;
      }
      case eSupplementalOxygen_Device::SimpleMask:
      {
        // This implementation can replace other Supplemental O2 modes
        if (m_data.GetAirwayMode() == eAirwayMode::NasalCannula ||
            m_data.GetAirwayMode() == eAirwayMode::NonRebreatherMask)
          m_data.SetAirwayMode(eAirwayMode::Free);
        m_data.SetAirwayMode(eAirwayMode::SimpleMask);

        if (!so.HasFlow())
        {
          flow_L_Per_min = 7.5;
          so.GetFlow().SetValue(flow_L_Per_min, VolumePerTimeUnit::L_Per_min);
          Info("Supplemental oxygen flow not set. Using default value of " + cdm::to_string(flow_L_Per_min) + " L/min.");
        }
        RespirationCircuit = &m_data.GetCircuits().GetActiveRespiratoryCircuit();
        OxygenInlet = RespirationCircuit->GetPath(pulse::SimpleMaskPath::SimpleMaskOxygenInlet);
        Tank = RespirationCircuit->GetPath(pulse::SimpleMaskPath::SimpleMaskPressure);
        break;
      }
      default:
      {
        Error("Ignoring unsupported supplemental oxygen type : " + eSupplementalOxygen_Device_Name(so.GetDevice()));
        return;
      }
    }

    //Use a default tank volume if it wasn't explicity set
    if (!so.HasVolume())
    {
      so.GetVolume().SetValue(425.0, VolumeUnit::L);
      Info("Supplemental oxygen initial tank volume not set. Using default value of 425 L.");
    }

    //Decrement volume from the tank
    //Inf volume is assumed to be a wall connection that will never run out
    if (so.GetVolume(VolumeUnit::L) != std::numeric_limits<double>::infinity())
    {
      so.GetVolume().Increment(-flow_L_Per_min * m_data.GetTimeStep_s()/60, VolumeUnit::L);
      //Check if the tank is depleated
      if (so.GetVolume(VolumeUnit::L) <= 0.0)
      {
        so.GetVolume().SetValue(0.0, VolumeUnit::L);
        flow_L_Per_min = 0.0;
        m_data.GetEvents().SetEvent(eEvent::SupplementalOxygenBottleExhausted, true, m_data.GetSimulationTime());
      }
      else
        m_data.GetEvents().SetEvent(eEvent::SupplementalOxygenBottleExhausted, false, m_data.GetSimulationTime());
    }
    else
      m_data.GetEvents().SetEvent(eEvent::SupplementalOxygenBottleExhausted, false, m_data.GetSimulationTime());

    //Nonrebreather mask works differently with the bag and doesn't have a pressure source for the tank
    if (so.GetDevice() != eSupplementalOxygen_Device::NonRebreatherMask)
    {
      //Determine flow control resistance
      //Assume pressure outside tank is comparatively approximately ambient
      double tankPressure_cmH2O = Tank->GetNextPressureSource(PressureUnit::cmH2O);
      double resistance_cmH2O_s_Per_L = tankPressure_cmH2O / (flow_L_Per_min / 60.0); //convert from min to s
      resistance_cmH2O_s_Per_L = LIMIT(resistance_cmH2O_s_Per_L, m_DefaultClosedResistance_cmH2O_s_Per_L, m_DefaultOpenResistance_cmH2O_s_Per_L);
      
      //Apply flow
      OxygenInlet->GetNextResistance().SetValue(resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    }
    else
    {
      //Handle nonrebreather mask bag
      SEFluidCircuitNode* NonRebreatherMaskBag = RespirationCircuit->GetNode(pulse::NonRebreatherMaskNode::NonRebreatherMaskBag);
      SEFluidCircuitPath* NonRebreatherMaskReservoirValve = RespirationCircuit->GetPath(pulse::NonRebreatherMaskPath::NonRebreatherMaskReservoirValve);

      double flowOut_L_Per_min = 0.0;
      if (NonRebreatherMaskReservoirValve->HasNextFlow())
      {
        flowOut_L_Per_min = NonRebreatherMaskReservoirValve->GetNextFlow(VolumePerTimeUnit::L_Per_min);
      }

      double bagVolume_L = NonRebreatherMaskBag->GetNextVolume(VolumeUnit::L);
      bagVolume_L = bagVolume_L + (flow_L_Per_min - flowOut_L_Per_min) * m_data.GetTimeStep_s()/60;
      if (bagVolume_L < 0.0)
      {
        bagVolume_L = 0.0;

        //No air can come from the bag
        OxygenInlet->GetNextResistance().SetValue(m_RespOpenResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
        m_data.GetEvents().SetEvent(eEvent::NonRebreatherMaskOxygenBagEmpty, true, m_data.GetSimulationTime());
      }
      else
      {
        if (bagVolume_L > 1.0)
        {
          bagVolume_L = 1.0;
        }
        m_data.GetEvents().SetEvent(eEvent::NonRebreatherMaskOxygenBagEmpty, false, m_data.GetSimulationTime());
      }
      
      NonRebreatherMaskBag->GetNextVolume().SetValue(bagVolume_L, VolumeUnit::L);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Respiratory driver pressure source determination
  ///
  /// \details
  /// Calculates the muscle pressure source pressure by using the chemical stimuli as feedback control mechanism.
  /// The method reads the arterial O2 and CO2 partial pressures. Using these partial pressures, the method calculates the
  /// alveolar ventilation from which the method calculates the target breathing frequency. The target breathing frequency is
  /// used in the equation for muscle pressure source. The muscle pressure source is used as a driver for ventilation.
  /// This method also calculates the drug modifiers that adjusts the depth and frequency of respiration.
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::CalculateDriver()
  {
    //Start of exhale/inhale
    if (m_data.GetEvents().IsEventActive(eEvent::StartOfExhale))
      m_data.GetEvents().SetEvent(eEvent::StartOfExhale, false, m_data.GetSimulationTime());
    if (m_data.GetEvents().IsEventActive(eEvent::StartOfInhale))
      m_data.GetEvents().SetEvent(eEvent::StartOfInhale, false, m_data.GetSimulationTime());

    if (m_data.HasCardiovascular())
    {
      //Keep a running average of the Arterial Partial Pressures
      m_ArterialO2RunningAverage_mmHg->Sample(m_AortaO2->GetPartialPressure(PressureUnit::mmHg));
      m_ArterialCO2RunningAverage_mmHg->Sample(m_AortaCO2->GetPartialPressure(PressureUnit::mmHg));
      //Reset at start of cardiac cycle 
      if (m_data.GetEvents().IsEventActive(eEvent::StartOfCardiacCycle))
      {
        m_ArterialO2PartialPressure_mmHg = m_ArterialO2RunningAverage_mmHg->Value();
        m_ArterialCO2PartialPressure_mmHg = m_ArterialCO2RunningAverage_mmHg->Value();

        m_ArterialO2RunningAverage_mmHg->Invalidate();
        m_ArterialCO2RunningAverage_mmHg->Invalidate();
      }

  #ifdef TUNING
      m_ArterialO2PartialPressure_mmHg = 100.0;
      m_ArterialCO2PartialPressure_mmHg = 42.0;
  #endif
    }

    m_MixedExpiredCO2RunningAverage_mmHg->Sample(m_Carina->GetSubstanceQuantity(m_data.GetSubstances().GetCO2())->GetPartialPressure(PressureUnit::mmHg));

    //Prepare for the next cycle -------------------------------------------------------------------------------
    if ((m_BreathingCycleTime_s > m_VentilationPeriod_s - m_data.GetTimeStep_s()) ||                              //End of the cycle or currently not breathing
      (m_PatientActions->HasConsciousRespiration() && !m_ActiveConsciousRespirationCommand)) //Or new consious respiration command to start immediately
    {
      m_BreathingCycleTime_s = 0.0;

      if (m_PatientActions->HasConsciousRespiration())
      {
        m_ActiveConsciousRespirationCommand = true;
        ConsciousRespiration();
      }
      else //Spontaneous breathing
      {
        if (!m_data.HasCardiovascular())
        {
          //No feedback, so this will always just keep the same waveform
          m_VentilationFrequency_Per_min = m_data.GetCurrentPatient().GetRespirationRateBaseline(FrequencyUnit::Per_min);
          m_IERatioScaleFactor = 1.0;

          double TargetVolume_L = m_data.GetInitialPatient().GetFunctionalResidualCapacity(VolumeUnit::L) + m_data.GetInitialPatient().GetTidalVolumeBaseline(VolumeUnit::L);
          m_PeakInspiratoryPressure_cmH2O = VolumeToDriverPressure(TargetVolume_L);
          m_PeakExpiratoryPressure_cmH2O = 0.0;
        }
        else
        {
          m_ActiveConsciousRespirationCommand = false;

          //Ventilatory Negative Feedback Control *************************************************************************
          double PeripheralCO2PartialPressureSetPoint = m_data.GetConfiguration().GetPeripheralControllerCO2PressureSetPoint(PressureUnit::mmHg);
          double CentralCO2PartialPressureSetPoint = m_data.GetConfiguration().GetCentralControllerCO2PressureSetPoint(PressureUnit::mmHg);

          //Get Drug Effects
          SEDrugSystem& Drugs = m_data.GetDrugs();
          double DrugRRChange_Per_min = Drugs.GetRespirationRateChange(FrequencyUnit::Per_min);
          double DrugsTVChange_L = Drugs.GetTidalVolumeChange(VolumeUnit::L);
          double NMBModifier = 1.0;
          double SedationModifier = 1.0;
          //Make changes to Respiration Rate change based on the neuromuscular block level
          if (Drugs.GetNeuromuscularBlockLevel().GetValue() > 0.135)
          {
            NMBModifier = 0.0;
            DrugRRChange_Per_min = 0.0;
            DrugsTVChange_L = 0.0;
          }
          else if (Drugs.GetNeuromuscularBlockLevel().GetValue() > 0.11)
          {
            NMBModifier = 0.5;
            DrugRRChange_Per_min = 0.0;
            DrugsTVChange_L = 0.0;
          }
          else if (Drugs.GetSedationLevel().GetValue() > 0.15 && std::abs(m_data.GetCurrentPatient().GetRespirationRateBaseline(FrequencyUnit::Per_min) + DrugRRChange_Per_min) < 1.0)
          {
            SedationModifier = 0.0;
            DrugRRChange_Per_min = 0.0;
            DrugsTVChange_L = 0.0;
          }

          //Calculate the target Alveolar Ventilation based on the Arterial O2 and CO2 concentrations
          double peripheralCO2SetPoint = PeripheralCO2PartialPressureSetPoint;
          double centralCO2SetPoint = CentralCO2PartialPressureSetPoint;
          if (Drugs.GetSedationLevel().GetValue() > 0)
          {
              if (m_data.GetSubstances().IsActive(*m_Oversedation))
              {
                  peripheralCO2SetPoint = PeripheralCO2PartialPressureSetPoint / Drugs.GetSedationLevel().GetValue();
                  centralCO2SetPoint = CentralCO2PartialPressureSetPoint / Drugs.GetSedationLevel().GetValue();
              }
              else
              {
                  peripheralCO2SetPoint = PeripheralCO2PartialPressureSetPoint / (1.0 - 0.15*Drugs.GetSedationLevel().GetValue());
                  centralCO2SetPoint = CentralCO2PartialPressureSetPoint / (1.0 - 0.15*Drugs.GetSedationLevel().GetValue());
              }
          }

          double targetAlveolarVentilation_L_Per_min = m_PeripheralControlGainConstant * exp(-0.05 * m_ArterialO2PartialPressure_mmHg) * MAX(0., m_ArterialCO2PartialPressure_mmHg - peripheralCO2SetPoint); //Peripheral portion
          targetAlveolarVentilation_L_Per_min += m_CentralControlGainConstant * MAX(0., m_ArterialCO2PartialPressure_mmHg - centralCO2SetPoint); //Central portion

          targetAlveolarVentilation_L_Per_min = UpdateTargetVentilation(targetAlveolarVentilation_L_Per_min);

          //Only move it part way to where it wants to be.
          //If it stays there, it will get there, just more controlled/slowly.
          //This is needed so we don't overshoot and introduce low frequency oscillations into the system (i.e. overdamped).
          double targetCO2PP_mmHg = 40.0;
          double changeFraction = 1.0;
          if (m_data.GetState() <= EngineState::InitialStabilization)
          {
            //This gets it nice and stable
            changeFraction = 0.1;
          }
          else
          {
            //This keeps it from going crazy with modifiers applied
            changeFraction = std::abs(m_ArterialCO2PartialPressure_mmHg - targetCO2PP_mmHg) / targetCO2PP_mmHg * 0.5;
          }
          changeFraction = MIN(changeFraction, 1.0);

  #ifdef TUNING
          changeFraction = 1.0;
  #endif

          targetAlveolarVentilation_L_Per_min = m_PreviousTargetAlveolarVentilation_L_Per_min + (targetAlveolarVentilation_L_Per_min - m_PreviousTargetAlveolarVentilation_L_Per_min) * changeFraction;
          m_PreviousTargetAlveolarVentilation_L_Per_min = targetAlveolarVentilation_L_Per_min;

          //Target Tidal Volume (i.e. Driver amplitude) *************************************************************************
          //Calculate the target Tidal Volume based on the Alveolar Ventilation
          double targetPulmonaryVentilation_L_Per_min = targetAlveolarVentilation_L_Per_min;

          double maximumPulmonaryVentilationRate_L_Per_min = m_data.GetConfiguration().GetPulmonaryVentilationRateMaximum(VolumePerTimeUnit::L_Per_min);
          if (targetPulmonaryVentilation_L_Per_min > maximumPulmonaryVentilationRate_L_Per_min)
          {
            targetPulmonaryVentilation_L_Per_min = maximumPulmonaryVentilationRate_L_Per_min;
            m_data.GetEvents().SetEvent(eEvent::MaximumPulmonaryVentilationRate, true, m_data.GetSimulationTime());
          }

          if (targetPulmonaryVentilation_L_Per_min < maximumPulmonaryVentilationRate_L_Per_min && m_data.GetEvents().IsEventActive(eEvent::MaximumPulmonaryVentilationRate))
          {
            m_data.GetEvents().SetEvent(eEvent::MaximumPulmonaryVentilationRate, false, m_data.GetSimulationTime());
          }

          //Calculate the target Tidal Volume based on the calculated target pulmonary ventilation, plot slope (determined during initialization), and x-intercept
          double targetTidalVolume_L = targetPulmonaryVentilation_L_Per_min / m_VentilationToTidalVolumeSlope + m_VentilationTidalVolumeIntercept;

          //Modify the target tidal volume due to other external effects - probably eventually replaced by the Nervous system
          //Apply Drug Effects to the target tidal volume
          double modifiedTargetTidalVolume_L = targetTidalVolume_L * NMBModifier + DrugsTVChange_L;

          //This is a piecewise function that plateaus at the Tidal Volume equal to 1/2 * Vital Capacity
          //The Respiration Rate will make up for the Alveoli Ventilation difference
          double halfVitalCapacity_L = m_data.GetInitialPatient().GetVitalCapacity(VolumeUnit::L) / 2.0;
          modifiedTargetTidalVolume_L = MIN(modifiedTargetTidalVolume_L, halfVitalCapacity_L);
          targetTidalVolume_L = MIN(targetTidalVolume_L, halfVitalCapacity_L);

          //Map the Target Tidal Volume to the Driver
          double targetVolume_L = m_data.GetInitialPatient().GetFunctionalResidualCapacity(VolumeUnit::L) + modifiedTargetTidalVolume_L;
          m_PeakInspiratoryPressure_cmH2O = VolumeToDriverPressure(targetVolume_L);
          //There's a maximum force the driver can try to achieve
          m_PeakInspiratoryPressure_cmH2O = MAX(m_PeakInspiratoryPressure_cmH2O, m_MaxDriverPressure_cmH2O);

          //This is always zero, unless there's conscious respiration
          m_PeakExpiratoryPressure_cmH2O = 0.0;

          //Respiration Rate (i.e. Driver frequency) *************************************************************************
          //Calculate the Respiration Rate given the Alveolar Ventilation and the Target Tidal Volume
          if (SEScalar::IsZero(modifiedTargetTidalVolume_L, ZERO_APPROX)) //Can't divide by zero
          {
            m_VentilationFrequency_Per_min = 0.0;
            m_NotBreathing = true;
          }
          else if (!m_NotBreathing)
          {
            //Mechanoreceptors (limited tidal volume)
            //Temper it some
            double targetTidalVolumeMechanoreceptorMultiplier = GeneralMath::LinearInterpolator(0.0, 1.0, 0.35, 1.0, (1.0 - m_AppliedMechanoreceptorsDyspneaFactor));
            targetTidalVolume_L *= targetTidalVolumeMechanoreceptorMultiplier;

            m_VentilationFrequency_Per_min = targetPulmonaryVentilation_L_Per_min / targetTidalVolume_L;
            m_VentilationFrequency_Per_min = m_VentilationFrequency_Per_min * NMBModifier * SedationModifier + DrugRRChange_Per_min;

            m_NotBreathing = false;
          }

          m_VentilationFrequency_Per_min = LIMIT(m_VentilationFrequency_Per_min, 0.0, maximumPulmonaryVentilationRate_L_Per_min / halfVitalCapacity_L);

          //Patient Definition *************************************************************************
          //We need to hit the patient's defined Respiration Rate Baseline, no matter what,
          //so we'll keep adjusting the slope of the function to achieve this
          if (m_data.GetState() <= EngineState::InitialStabilization)
          {
            double respirationRateBaseline_Per_min = m_data.GetCurrentPatient().GetRespirationRateBaseline(FrequencyUnit::Per_min);
            double percentError = (m_VentilationFrequency_Per_min - respirationRateBaseline_Per_min) / respirationRateBaseline_Per_min; //negative if too low

            //Amplitude set-point - this will set the Tidal Volume baseline when O2 and CO2 are at the correct/balanced level
            m_VentilationToTidalVolumeSlope = m_VentilationToTidalVolumeSlope - m_VentilationToTidalVolumeSlope * percentError;

            //Put bounds on this
            m_VentilationToTidalVolumeSlope = LIMIT(m_VentilationToTidalVolumeSlope, 1.0, 100.0);
          }

          UpdateDriverPeriod();

  #ifdef TUNING
          m_VentilationToTidalVolumeSlope = 40.0;
          m_VentilationFrequency_Per_min = 12.0;
  #endif
        }

        SetBreathCycleFractions();
      }

      m_MechanoreceptorsDyspneaFactor = 0.0;
    }

    if (HasActiveMechanics())
    {
      if (m_Mechanics->GetDefaultType() == eDefaultType::Zero)
      {
        m_PeakInspiratoryPressure_cmH2O = 0.0;
        m_PeakExpiratoryPressure_cmH2O = 0.0;
      }
      if (m_Mechanics->HasInspiratoryPeakPressure())
        m_PeakInspiratoryPressure_cmH2O = m_Mechanics->GetInspiratoryPeakPressure(PressureUnit::cmH2O);
      if (m_Mechanics->HasExpiratoryPeakPressure())
        m_PeakExpiratoryPressure_cmH2O = m_Mechanics->GetExpiratoryPeakPressure(PressureUnit::cmH2O);
      SetBreathCycleFractions();
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Respiratory driver pressure source application
  ///
  /// \details
  /// Apply the already calculated pressure source for inhalation/exhalation.
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::ApplyDriver()
  {
    //Run the driver based on the waveform -------------------------------------------------------------------------------
    double InspiratoryRiseTimeStart_s = 0.0;
    double InspiratoryHoldTimeStart_s = InspiratoryRiseTimeStart_s + m_InspiratoryRiseFraction * m_VentilationPeriod_s;
    double InspiratoryReleaseTimeStart_s = InspiratoryHoldTimeStart_s + m_InspiratoryHoldFraction * m_VentilationPeriod_s;
    double InspiratoryToExpiratoryPauseTimeStart_s = InspiratoryReleaseTimeStart_s + m_InspiratoryReleaseFraction * m_VentilationPeriod_s;
    double ExpiratoryRiseTimeStart_s = InspiratoryToExpiratoryPauseTimeStart_s + m_InspiratoryToExpiratoryPauseFraction * m_VentilationPeriod_s;
    double ExpiratoryHoldTimeStart_s = ExpiratoryRiseTimeStart_s + m_ExpiratoryRiseFraction * m_VentilationPeriod_s;
    double ExpiratoryReleaseTimeStart_s = ExpiratoryHoldTimeStart_s + m_ExpiratoryHoldFraction * m_VentilationPeriod_s;
    double ResidueFractionTimeStart_s = ExpiratoryReleaseTimeStart_s + m_ExpiratoryReleaseFraction * m_VentilationPeriod_s;

    if (!m_NotBreathing)
    {
      if (SEScalar::IsZero(m_BreathingCycleTime_s, ZERO_APPROX) &&
        m_InspiratoryRiseFraction != 0.0) //Only call this once per cycle - needed here for conscious respiration
      {
        m_data.GetEvents().SetEvent(eEvent::StartOfInhale, true, m_data.GetSimulationTime());
      }

      if (m_BreathingCycleTime_s >= InspiratoryReleaseTimeStart_s &&
        m_BreathingCycleTime_s < InspiratoryReleaseTimeStart_s + m_data.GetTimeStep_s()) //Only call this once per cycle
      {
        m_data.GetEvents().SetEvent(eEvent::StartOfExhale, true, m_data.GetSimulationTime());
      }
    }

    double pi = 3.14159265359;
    if (m_BreathingCycleTime_s >= ResidueFractionTimeStart_s)
    {
      m_DriverPressure_cmH2O = 0.0;
    }
    else if (m_BreathingCycleTime_s >= ExpiratoryReleaseTimeStart_s)
    {
      double segmentTime_s = ResidueFractionTimeStart_s - ExpiratoryReleaseTimeStart_s;
      m_DriverPressure_cmH2O = m_PeakExpiratoryPressure_cmH2O * sin(pi / 2.0 * (m_BreathingCycleTime_s + segmentTime_s - ExpiratoryReleaseTimeStart_s) / segmentTime_s);
    }
    else if (m_BreathingCycleTime_s >= ExpiratoryHoldTimeStart_s)
    {
      m_DriverPressure_cmH2O = m_PeakExpiratoryPressure_cmH2O;
    }
    else if (m_BreathingCycleTime_s >= ExpiratoryRiseTimeStart_s)
    {
      double segmentTime_s = ExpiratoryHoldTimeStart_s - ExpiratoryRiseTimeStart_s;
      m_DriverPressure_cmH2O = m_PeakExpiratoryPressure_cmH2O * sin(pi / 2.0 * (m_BreathingCycleTime_s - ExpiratoryRiseTimeStart_s) / segmentTime_s);
    }
    else if (m_BreathingCycleTime_s >= InspiratoryToExpiratoryPauseTimeStart_s)
    {
      m_DriverPressure_cmH2O = 0.0;
    }
    else if (m_BreathingCycleTime_s >= InspiratoryReleaseTimeStart_s)
    {
      double segmentTime_s = InspiratoryToExpiratoryPauseTimeStart_s - InspiratoryReleaseTimeStart_s;
      m_DriverPressure_cmH2O = m_PeakInspiratoryPressure_cmH2O * sin(pi / 2.0 * (m_BreathingCycleTime_s + segmentTime_s - InspiratoryReleaseTimeStart_s) / segmentTime_s);
    }
    else if (m_BreathingCycleTime_s >= InspiratoryHoldTimeStart_s)
    {
      m_DriverPressure_cmH2O = m_PeakInspiratoryPressure_cmH2O;
    }
    else //(m_BreathingCycleTime_s >= InspiratoryRiseTimeStart_s)
    {
      m_DriverPressure_cmH2O = m_PeakInspiratoryPressure_cmH2O * sin(pi / 2.0 * m_BreathingCycleTime_s / InspiratoryHoldTimeStart_s);
    }

    if (!m_PatientActions->HasConsciousRespiration() && !HasActiveMechanics())
    {
      UpdateDriverPressure();

      if (m_NotBreathing)
      {
        m_DriverPressure_cmH2O = 0.0;
      }
    }

    //Push Driving Data to the Circuit -------------------------------------------------------------------------------
    m_DriverPressurePath->GetNextPressureSource().SetValue(m_DriverPressure_cmH2O, PressureUnit::cmH2O);

    //We need to do this here to allow for the inhaler to get called before the next go-around
    m_BreathingCycleTime_s += m_data.GetTimeStep_s();
    if (m_BreathingCycleTime_s > m_VentilationPeriod_s - m_data.GetTimeStep_s()) //End of the cycle or currently not breathing
    {
      if (m_ActiveConsciousRespirationCommand)
      {
        SEConsciousRespiration& cr = m_PatientActions->GetConsciousRespiration();
        SEConsciousRespirationCommand* cmd = cr.GetActiveCommand();
        m_ss << "Completed Conscious Respiration Command : " << cmd->GetName();
        Info(m_ss);
        cr.RemoveActiveCommand();
      }
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Set inspiratory and expiratory timing
  ///
  /// \details
  /// This sets the fraction/percentage of each segment of a breath cycle. The values are used by the
  /// driver to produce the pressure waveform for a single breath. The fractions must sum to 1.0, and 
  /// anything remaining will be added at the end as a hold at zero.
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::SetBreathCycleFractions()
  {
    if (m_VentilationFrequency_Per_min < 1.0)
    {
      m_VentilationPeriod_s = 0.0;
    }
    else
    {
      m_VentilationPeriod_s = 60.0 / m_VentilationFrequency_Per_min;
    }

    if (m_ActiveConsciousRespirationCommand)
    {
      return;
    }

    m_IERatioScaleFactor *= 0.94; //Tuning factor determined from healthy validation

    //Healthy = ~1:2 IE Ratio = 0.33 inpiration and 0.67 expiration
    ///\cite Fresnel2014musclePressure
    //Adjust for standard 12 bpm giving ~0.33 instead of 16 bpm by adding 4
    double inspiratoryFraction = (0.0125 * (m_VentilationFrequency_Per_min + 4.0) + 0.125) * m_IERatioScaleFactor;
    inspiratoryFraction = LIMIT(inspiratoryFraction, 0.1, 0.5); //Max I:E Ratio set to 1:1
    double expiratoryFraction = 1.0 - inspiratoryFraction;

    m_InspiratoryRiseFraction = inspiratoryFraction;
    m_InspiratoryHoldFraction = 0.0;
    m_InspiratoryReleaseFraction = MIN(inspiratoryFraction, expiratoryFraction * 0.5);
    m_InspiratoryToExpiratoryPauseFraction = 1.0 - m_InspiratoryRiseFraction - m_InspiratoryReleaseFraction;
    m_ExpiratoryRiseFraction = 0.0;
    m_ExpiratoryHoldFraction = 0.0;
    m_ExpiratoryReleaseFraction = 0.0;
    m_ResidueFraction = 0.0;

    if (HasActiveMechanics())
    {
      if (m_Mechanics->GetDefaultType() == eDefaultType::Zero)
      {
        m_InspiratoryRiseFraction = 0.0;
        m_InspiratoryHoldFraction = 0.0;
        m_InspiratoryReleaseFraction = 0.0;
        m_InspiratoryToExpiratoryPauseFraction = 0.0;
        m_ExpiratoryRiseFraction = 0.0;
        m_ExpiratoryHoldFraction = 0.0;
        m_ExpiratoryReleaseFraction = 0.0;
        m_ResidueFraction = 0.0;
      }

      double inspiratoryRisePeriod_s = m_InspiratoryRiseFraction * m_VentilationPeriod_s;
      double inspiratoryHoldPeriod_s = m_InspiratoryHoldFraction * m_VentilationPeriod_s;
      double inspiratoryReleasePeriod_s = m_InspiratoryReleaseFraction * m_VentilationPeriod_s;
      double inspiratoryToExpiratoryPausePeriod_s = m_InspiratoryToExpiratoryPauseFraction * m_VentilationPeriod_s;
      double expiratoryRisePeriod_s = m_ExpiratoryRiseFraction * m_VentilationPeriod_s;
      double expiratoryHoldPeriod_s = m_ExpiratoryHoldFraction * m_VentilationPeriod_s;
      double expiratoryReleasePeriod_s = m_ExpiratoryReleaseFraction * m_VentilationPeriod_s;
      double residuePeriod_s = m_ResidueFraction * m_VentilationPeriod_s;

      if(m_Mechanics->HasInspiratoryRiseTime())
        inspiratoryRisePeriod_s = m_Mechanics->GetInspiratoryRiseTime(TimeUnit::s);
      if (m_Mechanics->HasInspiratoryHoldTime())
        inspiratoryHoldPeriod_s = m_Mechanics->GetInspiratoryHoldTime(TimeUnit::s);
      if (m_Mechanics->HasInspiratoryReleaseTime())
        inspiratoryReleasePeriod_s = m_Mechanics->GetInspiratoryReleaseTime(TimeUnit::s);
      if (m_Mechanics->HasInspiratoryToExpiratoryPauseTime())
        inspiratoryToExpiratoryPausePeriod_s = m_Mechanics->GetInspiratoryToExpiratoryPauseTime(TimeUnit::s);
      if (m_Mechanics->HasExpiratoryRiseTime())
        expiratoryRisePeriod_s = m_Mechanics->GetExpiratoryRiseTime(TimeUnit::s);
      if (m_Mechanics->HasExpiratoryHoldTime())
        expiratoryHoldPeriod_s = m_Mechanics->GetExpiratoryHoldTime(TimeUnit::s);
      if (m_Mechanics->HasExpiratoryReleaseTime())
        expiratoryReleasePeriod_s = m_Mechanics->GetExpiratoryReleaseTime(TimeUnit::s);
      if (m_Mechanics->HasResidueTime())
        residuePeriod_s = m_Mechanics->GetResidueTime(TimeUnit::s);

      m_VentilationPeriod_s =
        inspiratoryRisePeriod_s +
        inspiratoryHoldPeriod_s +
        inspiratoryReleasePeriod_s +
        inspiratoryToExpiratoryPausePeriod_s +
        expiratoryRisePeriod_s +
        expiratoryHoldPeriod_s +
        expiratoryReleasePeriod_s +
        residuePeriod_s;

      //Set all fractions
      m_InspiratoryRiseFraction = inspiratoryRisePeriod_s / m_VentilationPeriod_s;
      m_InspiratoryHoldFraction = inspiratoryHoldPeriod_s / m_VentilationPeriod_s;
      m_InspiratoryReleaseFraction = inspiratoryReleasePeriod_s / m_VentilationPeriod_s;
      m_InspiratoryToExpiratoryPauseFraction = inspiratoryToExpiratoryPausePeriod_s / m_VentilationPeriod_s;
      m_ExpiratoryRiseFraction = expiratoryRisePeriod_s / m_VentilationPeriod_s;
      m_ExpiratoryHoldFraction = expiratoryHoldPeriod_s / m_VentilationPeriod_s;
      m_ExpiratoryReleaseFraction = expiratoryReleasePeriod_s / m_VentilationPeriod_s;
      m_ResidueFraction = residuePeriod_s / m_VentilationPeriod_s;
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Tension pneumothorax
  ///
  /// \details
  /// Pneumothorax is an abnormal accumulation of air in the pleural cavity. Tension pneumothorax occurs when 
  /// air escapes into the pleural space on inspiration, but cannot return on expiration. Different types of pneumothorces are known.
  /// The engine models  open (sucking chest wound) and closed (intact thoracic cage) pnumothoraces whereas occlusive dressing
  /// and needle decompression are modeled as interventions. 
  /// The function models pneumothorax and needle decompression by opening respiratory circuits that allow air flows into 
  /// and out of the pleural cavity. Under normal condition, the path between the lung node or the outside environment and the intrapleural cavity 
  /// is blocked via significantly high resistances that serve as open switches. When open or closed pneumothorax is triggered, these resistances 
  /// are reduced in proportion to the severity of the pneumothorax. This reduction permits air flow into the pleural cavity a
  /// and cause respiratory and cardiovascular distress.
  /// The function allows the selection of tension pneumothorax and/or needle decompression events on the left or right lung side. The effects on 
  /// the cardiovascular system parameters is accounted through modifiers that scale in proportion to the accumulated gas. 
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::Pneumothorax()
  {
    if (m_PatientActions->HasTensionPneumothorax())
    {
      // Minimum flow resistance for the chest cavity or alveoli leak 
      double PneumoOpenMinResistance_cmH2O_s_Per_L = 100.0;
      double PneumoClosedMinResistance_cmH2O_s_Per_L = 10.0;
      // Maximum flow resistance for the chest cavity or alveoli leak
      double PneumoMaxResistance_cmH2O_s_Per_L = m_DefaultOpenResistance_cmH2O_s_Per_L;
      // Flow resistance for the decompression needle, if used
      double NeedleResistance_cmH2O_s_Per_L = 100.0;

      if (m_PatientActions->HasLeftOpenTensionPneumothorax())
      {
        // Scale the flow resistance through the chest opening based on severity
        double severity = m_PatientActions->GetLeftOpenTensionPneumothorax().GetSeverity().GetValue();
        double resistance_cmH2O_s_Per_L = PneumoMaxResistance_cmH2O_s_Per_L;
        if (severity > 0.0 && !m_PatientActions->HasLeftChestOcclusiveDressing())
        {
          resistance_cmH2O_s_Per_L = PneumoOpenMinResistance_cmH2O_s_Per_L / pow(severity, 2.0);
        }
        resistance_cmH2O_s_Per_L = MIN(resistance_cmH2O_s_Per_L, PneumoMaxResistance_cmH2O_s_Per_L);
        m_EnvironmentToLeftChestLeak->GetNextResistance().SetValue(resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);

        if (m_PatientActions->HasLeftNeedleDecompression())
        {
          m_LeftNeedleToLeftPleural->GetNextResistance().SetValue(NeedleResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
        }
      }

      if (m_PatientActions->HasRightOpenTensionPneumothorax())
      {
        // Scale the flow resistance through the chest opening based on severity
        double severity = m_PatientActions->GetRightOpenTensionPneumothorax().GetSeverity().GetValue();
        double resistance_cmH2O_s_Per_L = PneumoMaxResistance_cmH2O_s_Per_L;
        if (severity > 0.0 && !m_PatientActions->HasRightChestOcclusiveDressing())
        {
          resistance_cmH2O_s_Per_L = PneumoOpenMinResistance_cmH2O_s_Per_L / pow(severity, 2.0);
        }
        resistance_cmH2O_s_Per_L = MIN(resistance_cmH2O_s_Per_L, PneumoMaxResistance_cmH2O_s_Per_L);
        m_EnvironmentToRightChestLeak->GetNextResistance().SetValue(resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);

        if (m_PatientActions->HasRightNeedleDecompression())
        {
          m_RightNeedleToRightPleural->GetNextResistance().SetValue(NeedleResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
        }
        m_EnvironmentToRightChestLeak->GetNextResistance().SetValue(resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
      }

      if (m_PatientActions->HasLeftClosedTensionPneumothorax())
      {
        if (m_PatientActions->HasLeftNeedleDecompression())
        {
          m_LeftNeedleToLeftPleural->GetNextResistance().SetValue(NeedleResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
        }
        else
        {
          // Scale the flow resistance through the chest opening based on severity
          double severity = m_PatientActions->GetLeftClosedTensionPneumothorax().GetSeverity().GetValue();
          double resistance_cmH2O_s_Per_L = PneumoMaxResistance_cmH2O_s_Per_L;
          if (severity > 0.0)
          {
            resistance_cmH2O_s_Per_L = PneumoClosedMinResistance_cmH2O_s_Per_L / pow(severity, 2.0);
          }
          resistance_cmH2O_s_Per_L = MIN(resistance_cmH2O_s_Per_L, PneumoMaxResistance_cmH2O_s_Per_L);
          m_LeftAlveoliLeakToLeftPleural->GetNextResistance().SetValue(resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
        }
      }

      if (m_PatientActions->HasRightClosedTensionPneumothorax())
      {
        if (m_PatientActions->HasRightNeedleDecompression())
        {
          m_RightNeedleToRightPleural->GetNextResistance().SetValue(NeedleResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
        }
        else
        {
          // Scale the flow resistance through the chest opening based on severity
          double severity = m_PatientActions->GetRightClosedTensionPneumothorax().GetSeverity().GetValue();
          double resistance_cmH2O_s_Per_L = PneumoMaxResistance_cmH2O_s_Per_L;
          if (severity > 0.0)
          {
            resistance_cmH2O_s_Per_L = PneumoClosedMinResistance_cmH2O_s_Per_L / pow(severity, 2.0);
          }
          resistance_cmH2O_s_Per_L = MIN(resistance_cmH2O_s_Per_L, PneumoMaxResistance_cmH2O_s_Per_L);
          m_RightAlveoliLeakToRightPleural->GetNextResistance().SetValue(resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
        }
      }

      //Check for interventions without insult
      if (!m_PatientActions->HasLeftClosedTensionPneumothorax() && !m_PatientActions->HasLeftOpenTensionPneumothorax())
      {
        if (m_PatientActions->HasLeftChestOcclusiveDressing() || m_PatientActions->HasLeftNeedleDecompression())
        {
          Error("Cannot perform an intervention if Tension Pneumothorax is not present on that side.");
          m_PatientActions->RemoveLeftChestOcclusiveDressing();
          m_PatientActions->RemoveLeftNeedleDecompression();
          return;
        }
      }
      if (!m_PatientActions->HasRightClosedTensionPneumothorax() && !m_PatientActions->HasRightOpenTensionPneumothorax())
      {
        if (m_PatientActions->HasRightChestOcclusiveDressing() || m_PatientActions->HasRightNeedleDecompression())
        {
          Error("Cannot perform an intervention if Tension Pneumothorax is not present on that side.");
          m_PatientActions->RemoveRightChestOcclusiveDressing();
          m_PatientActions->RemoveRightNeedleDecompression();
          return;
        }
      }
    }
    else
    {
      // Check for cases where the needle decompression or occlusive dressing actions are called without having
      // initiated a pneumothorax action
      if (m_PatientActions->HasNeedleDecompression())
      {
        Error("Cannot perform a Needle Decompression intervention if Tension Pneumothorax is not present");
        m_PatientActions->RemoveLeftNeedleDecompression();
        m_PatientActions->RemoveRightNeedleDecompression();
        return;
      }
      if (m_PatientActions->HasChestOcclusiveDressing())
      {
        Error("Cannot perform a Chest Occlusive Dressing intervention if Tension Pneumothorax is not present");
        m_PatientActions->RemoveLeftChestOcclusiveDressing();
        m_PatientActions->RemoveRightChestOcclusiveDressing();
        return;
      }
    }
  }

//--------------------------------------------------------------------------------------------------
/// \brief
/// Hemothorax
///
/// \details
/// Model both traumatic and nontraumatic hemothorax effects of the respiratory system. Optionally set
/// a severity or specify a blood flow into the pleural space. The blood out of the cardiovascular
/// system is used to determine the volume change in the pleural space. The pleural space is modeled
/// with gas in the circuit rather than fluid. Changes to lung compliance and gas exchange parameters
/// are handled elsewhere in the respirtory model.
//--------------------------------------------------------------------------------------------------
  void RespiratoryModel::Hemothorax()
  {
    double factor = 1.2; //Tuned factor to balance hemorrhage with lung collapse

    double maxVolume_L = 2.0;
    bool stateChange = false;

    // Left ----------------------------------------------------------------------------------------

    double leftBloodFlow_L_Per_s = 0.0;
    if (m_data.GetActions().GetPatientActions().HasLeftHemothorax())
    {
      if(m_LeftCardiovascularLeak == nullptr)
       m_LeftCardiovascularLeak = m_data.GetCircuits().GetActiveCardiovascularCircuit().GetPath(pulse::CardiovascularPath::LeftPulmonaryVeinsLeak1ToGround);

      if (m_LeftCardiovascularLeak != nullptr)
      {
        //Blood coming in
        if (m_LeftCardiovascularLeak->HasNextFlow())
        {
          leftBloodFlow_L_Per_s += m_LeftCardiovascularLeak->GetNextFlow(VolumePerTimeUnit::L_Per_s);
        }
      }
    }

    bool hasLeftTubeThoracostomy = m_PatientActions->HasLeftTubeThoracostomy();
    if (hasLeftTubeThoracostomy)
    {
      //Default 200 mL/day \cite hessami2009volume
      double thoracostomyFlowRate_L_Per_s = 2.31e-6;
      if (m_PatientActions->GetLeftTubeThoracostomy().HasFlowRate())
      {
        thoracostomyFlowRate_L_Per_s = m_PatientActions->GetLeftTubeThoracostomy().GetFlowRate(VolumePerTimeUnit::L_Per_s);
        if (thoracostomyFlowRate_L_Per_s > 0.001)
        {
          Error("Tube thoracostomy flow rate is above maximum allowable value. Overriding to 1 mL/s.");
          thoracostomyFlowRate_L_Per_s = 0.001;
          m_data.GetActions().GetPatientActions().GetLeftTubeThoracostomy().GetFlowRate().SetValue(thoracostomyFlowRate_L_Per_s, VolumePerTimeUnit::L_Per_s);
        }
      }
      //Blood going out
      leftBloodFlow_L_Per_s -= thoracostomyFlowRate_L_Per_s;
    }

    if (leftBloodFlow_L_Per_s != 0.0)
    {
      if (m_LeftRespirtoryLeak == nullptr)
      {
        //Create the left chest leak in the circuit (no updates to graph needed)
        m_LeftRespirtoryLeak = &m_RespiratoryCircuit->CreatePath(*m_AmbientNode, *m_LeftPleuralNode, pulse::RespiratoryPath::EnvironmentToLeftPleural);
        m_LeftRespirtoryLeak->GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::L_Per_s);
        stateChange = true;
      }

      if (m_PatientActions->GetLeftHemothorax().HasTotalBloodVolume())
      {
        m_PatientActions->GetLeftHemothorax().GetTotalBloodVolume().Increment(leftBloodFlow_L_Per_s * m_data.GetTimeStep_s(), VolumeUnit::L);
      }
      else
      {
        m_PatientActions->GetLeftHemothorax().GetTotalBloodVolume().SetValue(leftBloodFlow_L_Per_s * m_data.GetTimeStep_s(), VolumeUnit::L);
      }

      if (m_PatientActions->GetLeftHemothorax().GetTotalBloodVolume(VolumeUnit::L) > maxVolume_L)
      {
        Error("Maximum left hemothorax volume exceeded. Removing left hemothorax");
        m_PatientActions->GetLeftHemothorax().GetFlowRate().SetValue(0.0, VolumePerTimeUnit::L_Per_s);
        if (m_PatientActions->GetLeftHemothorax().HasSeverity())
        {
          m_PatientActions->GetLeftHemothorax().GetSeverity().SetValue(0.0);
        }
      }
      else
      {
        m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::EnvironmentToLeftPleural)->GetNextFlowSource().SetValue(factor * leftBloodFlow_L_Per_s, VolumePerTimeUnit::L_Per_s);
      }

      if (hasLeftTubeThoracostomy &&
          m_PatientActions->GetLeftHemothorax().GetTotalBloodVolume().IsNegative() &&
          m_PatientActions->GetLeftTubeThoracostomy().GetFlowRate().IsPositive())
      {
        Info("Left Tube Thoracostomy has removed all blood. Setting flow rate to 0.");
        m_PatientActions->GetLeftTubeThoracostomy().GetFlowRate().SetValue(0, VolumePerTimeUnit::L_Per_s);
      }
    }
    else if (m_LeftRespirtoryLeak != nullptr)
    {
      //Remove the left chest leak in the circuit (no updates to graph needed)
      m_RespiratoryCircuit->RemovePath(*m_LeftRespirtoryLeak);
      m_LeftRespirtoryLeak = nullptr;
      stateChange = true;
    }

    if (hasLeftTubeThoracostomy && m_PatientActions->GetLeftTubeThoracostomy().GetFlowRate().IsZero())
      m_PatientActions->RemoveLeftTubeThoracostomy();

    // Right ---------------------------------------------------------------------------------------
    double rightBloodFlow_L_Per_s = 0.0;
    if (m_data.GetActions().GetPatientActions().HasRightHemothorax())
    {
      if (m_RightCardiovascularLeak == nullptr)
        m_RightCardiovascularLeak = m_data.GetCircuits().GetActiveCardiovascularCircuit().GetPath(pulse::CardiovascularPath::RightPulmonaryVeinsLeak1ToGround);

      if (m_RightCardiovascularLeak != nullptr)
      {
        //Blood coming in
        if (m_RightCardiovascularLeak->HasNextFlow())
        {
          rightBloodFlow_L_Per_s += m_RightCardiovascularLeak->GetNextFlow(VolumePerTimeUnit::L_Per_s);
        }
      }
    }

    bool hasRightTubeThoracostomy = m_PatientActions->HasRightTubeThoracostomy();
    if (hasRightTubeThoracostomy)
    {
      //Default 200 mL/day \cite hessami2009volume
      double thoracostomyFlowRate_L_Per_s = 2.31e-6;
      if (m_PatientActions->GetRightTubeThoracostomy().HasFlowRate())
      {
        thoracostomyFlowRate_L_Per_s = m_PatientActions->GetRightTubeThoracostomy().GetFlowRate(VolumePerTimeUnit::L_Per_s);
        if (thoracostomyFlowRate_L_Per_s > 0.001)
        {
          Error("Tube thoracostomy flow rate is above maximum allowable value. Overriding to 1 mL/s.");
          thoracostomyFlowRate_L_Per_s = 0.001;
          m_data.GetActions().GetPatientActions().GetRightTubeThoracostomy().GetFlowRate().SetValue(thoracostomyFlowRate_L_Per_s, VolumePerTimeUnit::L_Per_s);
        }
      }
      //Blood going out
      rightBloodFlow_L_Per_s -= thoracostomyFlowRate_L_Per_s;
    }

    if (rightBloodFlow_L_Per_s != 0.0)
    {
      if (m_RightRespirtoryLeak ==nullptr)
      {
        //Create the right chest leak in the circuit (no updates to graph needed)
        m_RightRespirtoryLeak = &m_RespiratoryCircuit->CreatePath(*m_AmbientNode, *m_RightPleuralNode, pulse::RespiratoryPath::EnvironmentToRightPleural);
        m_RightRespirtoryLeak->GetFlowSourceBaseline().SetValue(0.0, VolumePerTimeUnit::L_Per_s);
        stateChange = true;
      }

      if (m_PatientActions->GetRightHemothorax().HasTotalBloodVolume())
      {
        m_PatientActions->GetRightHemothorax().GetTotalBloodVolume().Increment(rightBloodFlow_L_Per_s * m_data.GetTimeStep_s(), VolumeUnit::L);
      }
      else
      {
        m_PatientActions->GetRightHemothorax().GetTotalBloodVolume().SetValue(rightBloodFlow_L_Per_s * m_data.GetTimeStep_s(), VolumeUnit::L);
      }

      if (m_PatientActions->GetRightHemothorax().GetTotalBloodVolume(VolumeUnit::L) > maxVolume_L)
      {
        Error("Maximum right hemothorax volume exceeded. Removing right hemothorax");
        m_PatientActions->GetRightHemothorax().GetFlowRate().SetValue(0.0, VolumePerTimeUnit::L_Per_s);
        if (m_PatientActions->GetRightHemothorax().HasSeverity())
        {
          m_PatientActions->GetRightHemothorax().GetSeverity().SetValue(0.0);
        }
      }
      else
      {
        m_RespiratoryCircuit->GetPath(pulse::RespiratoryPath::EnvironmentToRightPleural)->GetNextFlowSource().SetValue(factor * rightBloodFlow_L_Per_s, VolumePerTimeUnit::L_Per_s);
      }

      if (hasRightTubeThoracostomy &&
          m_PatientActions->GetRightHemothorax().GetTotalBloodVolume().IsNegative() &&
          m_PatientActions->GetRightTubeThoracostomy().GetFlowRate().IsPositive())
      {
        Info("Right Tube Thoracostomy has removed all blood. Setting flow rate to 0.");
        m_PatientActions->GetRightTubeThoracostomy().GetFlowRate().SetValue(0, VolumePerTimeUnit::L_Per_s);
      }
    }
    else if (m_RightRespirtoryLeak !=nullptr)
    {
      //Remove the right chest leak in the circuit (no updates to graph needed)
      m_RespiratoryCircuit->RemovePath(*m_RightRespirtoryLeak);
      m_RightRespirtoryLeak = nullptr;
      stateChange = true;
    }

    if (hasRightTubeThoracostomy && m_PatientActions->GetRightTubeThoracostomy().GetFlowRate().IsZero())
      m_PatientActions->RemoveRightTubeThoracostomy();

    // Update circuit ------------------------------------------------------------------------------

    if (stateChange)
    {
      m_RespiratoryCircuit->StateChange();
    }

    //Events ---------------------------------------------------------------------------------------

    double totalBloodVolume_L = 0.0;
    if (m_PatientActions->GetLeftHemothorax().HasTotalBloodVolume())
    {
      totalBloodVolume_L += m_PatientActions->GetLeftHemothorax().GetTotalBloodVolume(VolumeUnit::L);
    }
    if (m_PatientActions->GetRightHemothorax().HasTotalBloodVolume())
    {
      totalBloodVolume_L += m_PatientActions->GetRightHemothorax().GetTotalBloodVolume(VolumeUnit::L);
    }

    if (totalBloodVolume_L > 1.0)
    {
      m_data.GetEvents().SetEvent(eEvent::MassiveHemothorax, true, m_data.GetSimulationTime());
      m_data.GetEvents().SetEvent(eEvent::MediumHemothorax, false, m_data.GetSimulationTime());
      m_data.GetEvents().SetEvent(eEvent::MinimalHemothorax, false, m_data.GetSimulationTime());
    }
    else if (totalBloodVolume_L > 0.4)
    {
      m_data.GetEvents().SetEvent(eEvent::MassiveHemothorax, false, m_data.GetSimulationTime());
      m_data.GetEvents().SetEvent(eEvent::MediumHemothorax, true, m_data.GetSimulationTime());
      m_data.GetEvents().SetEvent(eEvent::MinimalHemothorax, false, m_data.GetSimulationTime());
    }
    else if (totalBloodVolume_L > 0.001)
    {
      m_data.GetEvents().SetEvent(eEvent::MassiveHemothorax, false, m_data.GetSimulationTime());
      m_data.GetEvents().SetEvent(eEvent::MediumHemothorax, false, m_data.GetSimulationTime());
      m_data.GetEvents().SetEvent(eEvent::MinimalHemothorax, true, m_data.GetSimulationTime());
    }
    else
    {
      m_data.GetEvents().SetEvent(eEvent::MassiveHemothorax, false, m_data.GetSimulationTime());
      m_data.GetEvents().SetEvent(eEvent::MediumHemothorax, false, m_data.GetSimulationTime());
      m_data.GetEvents().SetEvent(eEvent::MinimalHemothorax, false, m_data.GetSimulationTime());
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Conscious respiration
  ///
  /// \param  None
  ///
  /// \return void
  ///
  /// \details
  /// This method processes a conscious respiration command and removes by setting the effort waveform
  /// parameters. New commands will overwrite incomplete commands.
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::ConsciousRespiration()
  {
    SEConsciousRespiration& cr = m_PatientActions->GetConsciousRespiration();

    m_InspiratoryRiseFraction = 0.0;
    m_InspiratoryHoldFraction = 0.0;
    m_InspiratoryReleaseFraction = 0.0;
    m_InspiratoryToExpiratoryPauseFraction = 0.0;
    m_ExpiratoryRiseFraction = 0.0;
    m_ExpiratoryHoldFraction = 0.0;
    m_ExpiratoryReleaseFraction = 0.0;
    m_ResidueFraction = 0.0;

    SEConsciousRespirationCommand* cmd = cr.GetActiveCommand();

    SEForcedInhale* fi = dynamic_cast<SEForcedInhale*>(cmd);
    if (fi != nullptr)
    {
      double pressureFraction = 0.0;
      double risePeriod_s = 0.0;
      double holdPeriod_s = 0.0;
      double releasePeriod_s = 0.0;

      if (fi->HasInspiratoryCapacityFraction())
      {
        pressureFraction = fi->GetInspiratoryCapacityFraction().GetValue();
      }

      if (fi->HasInhalePeriod())
      {
        risePeriod_s = fi->GetInhalePeriod().GetValue(TimeUnit::s);
      }
      if (fi->HasHoldPeriod())
      {
        holdPeriod_s = fi->GetHoldPeriod().GetValue(TimeUnit::s);
      }
      if (fi->HasReleasePeriod())
      {
        releasePeriod_s = fi->GetReleasePeriod().GetValue(TimeUnit::s);
      }

      double TargetVolume_L = m_data.GetInitialPatient().GetFunctionalResidualCapacity(VolumeUnit::L) + m_data.GetInitialPatient().GetInspiratoryCapacity(VolumeUnit::L) * pressureFraction;
      m_PeakInspiratoryPressure_cmH2O = VolumeToDriverPressure(TargetVolume_L);

      double totalPeriod = risePeriod_s + holdPeriod_s + releasePeriod_s;

      if (SEScalar::IsZero(totalPeriod, ZERO_APPROX))
      {
        m_VentilationFrequency_Per_min = 0.0;
        m_InspiratoryRiseFraction = 0.0;
        m_InspiratoryHoldFraction = 0.0;
        m_InspiratoryReleaseFraction = 0.0;
      }
      else
      {
        m_VentilationFrequency_Per_min = 60.0 / totalPeriod;
        m_InspiratoryRiseFraction = risePeriod_s / totalPeriod;
        m_InspiratoryHoldFraction = holdPeriod_s / totalPeriod;
        m_InspiratoryReleaseFraction = releasePeriod_s / totalPeriod;
      }    

      SetBreathCycleFractions();
      return;
    }

    SEForcedExhale* fe = dynamic_cast<SEForcedExhale*>(cmd);
    if (fe != nullptr)
    {
      double pressureFraction = 0.0;
      double risePeriod_s = 0.0;
      double holdPeriod_s = 0.0;
      double releasePeriod_s = 0.0;

      if (fe->HasExpiratoryReserveVolumeFraction())
      {
        pressureFraction = fe->GetExpiratoryReserveVolumeFraction().GetValue();
      }

      if (fe->HasExhalePeriod())
      {
        risePeriod_s = fe->GetExhalePeriod().GetValue(TimeUnit::s);
      }
      if (fe->HasHoldPeriod())
      {
        holdPeriod_s = fe->GetHoldPeriod().GetValue(TimeUnit::s);
      }
      if (fe->HasReleasePeriod())
      {
        releasePeriod_s = fe->GetReleasePeriod().GetValue(TimeUnit::s);
      }

      double TargetVolume_L = m_data.GetInitialPatient().GetResidualVolume(VolumeUnit::L) + m_data.GetInitialPatient().GetExpiratoryReserveVolume(VolumeUnit::L) * (1.0 - pressureFraction);

      m_PeakExpiratoryPressure_cmH2O = VolumeToDriverPressure(TargetVolume_L);

      double totalPeriod = risePeriod_s + holdPeriod_s + releasePeriod_s;
      if (SEScalar::IsZero(totalPeriod, ZERO_APPROX))
      {
        m_VentilationFrequency_Per_min = 0.0;
        m_ExpiratoryRiseFraction = 0.0;
        m_ExpiratoryHoldFraction = 0.0;
        m_ExpiratoryReleaseFraction = 0.0;
      }
      else
      {
        m_VentilationFrequency_Per_min = 60.0 / totalPeriod;
        m_ExpiratoryRiseFraction = risePeriod_s / totalPeriod;
        m_ExpiratoryHoldFraction = holdPeriod_s / totalPeriod;
        m_ExpiratoryReleaseFraction = releasePeriod_s / totalPeriod;
      }

      SetBreathCycleFractions();
      return;
    }

    SEForcedPause* fp = dynamic_cast<SEForcedPause*>(cmd);
    if (fp != nullptr)
    {
      double period_s = 0.0;
      if (fp->HasPeriod())
      {
        period_s = fp->GetPeriod().GetValue(TimeUnit::s);
      }

      m_VentilationFrequency_Per_min = (SEScalar::IsZero(period_s, ZERO_APPROX)) ? 0.0 : 60.0 / period_s;
      m_InspiratoryToExpiratoryPauseFraction = 1.0;

      SetBreathCycleFractions();
      return;
    } 
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates key respiratory physiological parameters
  ///
  /// \details
  /// Calculates the respiration rate and tidal volume. For each breathing cycle, the tidal volume is calculated by identifying 
  /// the peaks in the total lung volume.
  /// The tidal volume of a particular breathing cycle is then calculated by taking the difference between the 
  /// maximum and minimum values of the total lung volume. 
  /// The respiration rate is calculated by measuring the duration for one complete breathing cycle and then 
  /// converting the duration to the number of breaths per minute.
  // The method also checks for bradypnea and tachypnea events and reports to the use.
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::CalculateVitalSigns()
  {
    //Record values each time-step
    std::stringstream ss;

    //Total Respiratory Volume - this should not include the Pleural Space
    double totalLungVolume_L = m_Lungs->GetVolume(VolumeUnit::L);
    GetTotalLungVolume().SetValue(totalLungVolume_L, VolumeUnit::L);

    double functionalResidualCapacity_L = m_data.GetCurrentPatient().GetFunctionalResidualCapacity(VolumeUnit::L);
    double relativeTotalLungVolume_L = totalLungVolume_L - functionalResidualCapacity_L;
    GetRelativeTotalLungVolume().SetValue(relativeTotalLungVolume_L, VolumeUnit::L);

    double AnatomicDeadSpace_L = m_AnatomicDeadSpace->GetVolume(VolumeUnit::L);
    double AlveolarDeadSpace_L = m_AlveolarDeadSpace->GetVolume(VolumeUnit::L);
    GetAnatomicDeadSpace().SetValue(AnatomicDeadSpace_L, VolumeUnit::L);
    GetAlveolarDeadSpace().SetValue(AlveolarDeadSpace_L, VolumeUnit::L);
    GetPhysiologicDeadSpace().SetValue(AnatomicDeadSpace_L + AlveolarDeadSpace_L, VolumeUnit::L);

    double tracheaFlow_L_Per_s = m_PharynxToCarina->GetNextFlow(VolumePerTimeUnit::L_Per_s);
    double previousInspiratoryFlow_L_Per_s = GetInspiratoryFlow(VolumePerTimeUnit::L_Per_s);
    GetInspiratoryFlow().SetValue(tracheaFlow_L_Per_s, VolumePerTimeUnit::L_Per_s);
    GetExpiratoryFlow().SetValue(-tracheaFlow_L_Per_s, VolumePerTimeUnit::L_Per_s);

    /// \cite kacmarek2016egan page 227-228 ---------------------------------------------------
    double airwayOpeningPressure_cmH2O = m_AirwayNode->GetNextPressure(PressureUnit::cmH2O);
    double alveolarPressure_cmH2O = m_Alveoli->GetPressure(PressureUnit::cmH2O);
    double pleuralPressure_cmH2O = m_PleuralCavity->GetPressure(PressureUnit::cmH2O);
    double musclePressure_cmH2O = m_RespiratoryMuscleNode->GetNextPressure(PressureUnit::cmH2O);
    double bodySurfacePressure_cmH2O = m_AmbientNode->GetNextPressure(PressureUnit::cmH2O);

    double transrespiratoryPressure_cmH2O = airwayOpeningPressure_cmH2O - bodySurfacePressure_cmH2O;
    double transairwayPressure_cmH2O = airwayOpeningPressure_cmH2O - alveolarPressure_cmH2O;
    double transpulmonaryPressure_cmH2O = airwayOpeningPressure_cmH2O - pleuralPressure_cmH2O;
    double transalveolarPressure_cmH2O = alveolarPressure_cmH2O - pleuralPressure_cmH2O;
    double transthoracicPressure_cmH2O = alveolarPressure_cmH2O - bodySurfacePressure_cmH2O;
    double transChestWallPressure_cmH2O = pleuralPressure_cmH2O - bodySurfacePressure_cmH2O;
    double transMusclePressure_cmH2O = musclePressure_cmH2O - bodySurfacePressure_cmH2O;

    GetAirwayPressure().SetValue(airwayOpeningPressure_cmH2O, PressureUnit::cmH2O);
    GetRespiratoryMusclePressure().SetValue(musclePressure_cmH2O, PressureUnit::cmH2O);
    GetIntrapleuralPressure().SetValue(transChestWallPressure_cmH2O, PressureUnit::cmH2O);
    GetIntrapulmonaryPressure().SetValue(-transMusclePressure_cmH2O, PressureUnit::cmH2O);
    GetTransrespiratoryPressure().SetValue(transrespiratoryPressure_cmH2O, PressureUnit::cmH2O);
    GetTransairwayPressure().SetValue(transairwayPressure_cmH2O, PressureUnit::cmH2O);
    GetTranspulmonaryPressure().SetValue(transpulmonaryPressure_cmH2O, PressureUnit::cmH2O);
    GetTransalveolarPressure().SetValue(transalveolarPressure_cmH2O, PressureUnit::cmH2O);
    GetTransthoracicPressure().SetValue(transthoracicPressure_cmH2O, PressureUnit::cmH2O);
    GetTransChestWallPressure().SetValue(transChestWallPressure_cmH2O, PressureUnit::cmH2O);
    GetTransMusclePressure().SetValue(transMusclePressure_cmH2O, PressureUnit::cmH2O);

    if (m_data.HasCardiovascular())
    {
      double averageAlveoliO2PartialPressure_mmHg = (m_LeftAlveoliO2->GetPartialPressure(PressureUnit::mmHg) + m_RightAlveoliO2->GetPartialPressure(PressureUnit::mmHg)) / 2.0;
      GetAlveolarArterialGradient().SetValue(averageAlveoliO2PartialPressure_mmHg - m_AortaO2->GetPartialPressure(PressureUnit::mmHg), PressureUnit::mmHg);
    }
    else
    {
      GetAlveolarArterialGradient().Invalidate();
    }

    m_MeanAirwayPressure_cmH2O->Sample(transrespiratoryPressure_cmH2O);


    // Resistance based on simulated values
    if (abs(tracheaFlow_L_Per_s) > 0.1)
    {
      double rightFlow_L_Per_s = m_CarinaToRightAnatomicDeadSpace->GetNextFlow(VolumePerTimeUnit::L_Per_s);
      double leftFlow_L_Per_s = m_CarinaToLeftAnatomicDeadSpace->GetNextFlow(VolumePerTimeUnit::L_Per_s);

      double rightAlveoliPressure_cmH2O = m_RightAlveoli->GetPressure(PressureUnit::cmH2O);
      double leftAlveoliPressure_cmH2O = m_LeftAlveoli->GetPressure(PressureUnit::cmH2O);
      // Flow-weighted alveolar pressure
      double averageAlveoliPressure_cmH2O = (rightFlow_L_Per_s * rightAlveoliPressure_cmH2O + leftFlow_L_Per_s * leftAlveoliPressure_cmH2O) / tracheaFlow_L_Per_s;
      double pressureDifference_cmH2O = airwayOpeningPressure_cmH2O - averageAlveoliPressure_cmH2O;

      if (abs(pressureDifference_cmH2O) > 0.01)
      {
        double resistance_cmH2O_s_Per_L = pressureDifference_cmH2O / tracheaFlow_L_Per_s;

        // Ensure resistance is non-negative
        if (resistance_cmH2O_s_Per_L > 0.0)
        {
          double dampenFraction_perSec = 0.01 * 50.0;
          if (tracheaFlow_L_Per_s > 0.0)
          {
            double previousResistance_cmH2O_s_Per_L = GetInspiratoryRespiratoryResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
            resistance_cmH2O_s_Per_L = GeneralMath::Damper(resistance_cmH2O_s_Per_L, previousResistance_cmH2O_s_Per_L, dampenFraction_perSec, m_data.GetTimeStep_s());
            GetInspiratoryRespiratoryResistance().SetValue(resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
          }
          else
          {
            double previousResistance_cmH2O_s_Per_L = GetExpiratoryRespiratoryResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
            resistance_cmH2O_s_Per_L = GeneralMath::Damper(resistance_cmH2O_s_Per_L, previousResistance_cmH2O_s_Per_L, dampenFraction_perSec, m_data.GetTimeStep_s());
            GetExpiratoryRespiratoryResistance().SetValue(resistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
          }
        }
      }
    }


    // Compliances based on simulated values
    // Use compartments so this will work with expanded lungs
    double leftLungPressureDiff_cmH2O = m_LeftAlveoli->GetPressure(PressureUnit::cmH2O) - m_LeftPleuralCavity->GetPressure(PressureUnit::cmH2O);
    double rightLungPressureDiff_cmH2O = m_RightAlveoli->GetPressure(PressureUnit::cmH2O) - m_RightPleuralCavity->GetPressure(PressureUnit::cmH2O);
    double leftPleuralPressureDiff_cmH2O = m_LeftPleuralCavity->GetPressure(PressureUnit::cmH2O) - musclePressure_cmH2O;
    double rightPleuralPressureDiff_cmH2O = m_RightPleuralCavity->GetPressure(PressureUnit::cmH2O) - musclePressure_cmH2O;

    double leftLungVolume_L = m_LeftLung->GetVolume(VolumeUnit::L);
    double rightLungVolume_L = m_RightLung->GetVolume(VolumeUnit::L);
    double leftPleuralVolume_L = m_LeftPleuralCavity->GetVolume(VolumeUnit::L);
    double rightPleuralVolume_L = m_RightPleuralCavity->GetVolume(VolumeUnit::L);

    double leftLungPressureChange_cmH2O = leftLungPressureDiff_cmH2O - m_PreviousLeftLungPressureDiff_cmH2O;
    double rightLungPressureChange_cmH2O = rightLungPressureDiff_cmH2O - m_PreviousRightLungPressureDiff_cmH2O;
    double leftPleuralPressureChange_cmH2O = leftPleuralPressureDiff_cmH2O - m_PreviousLeftPleuralPressureDiff_cmH2O;
    double rightPleuralPressureChange_cmH2O = rightPleuralPressureDiff_cmH2O - m_PreviousRightPleuralPressureDiff_cmH2O;

    double leftLungVolumeChange_L = leftLungVolume_L - m_PreviousLeftLungVolume_L;
    double rightLungVolumeChange_L = rightLungVolume_L - m_PreviousRightLungVolume_L;
    double leftPleuralVolumeChange_L = leftPleuralVolume_L - m_PreviousLeftPleuralVolume_L;
    double rightPleuralVolumeChange_L = rightPleuralVolume_L - m_PreviousRightPleuralVolume_L;

    // Calculate individual compliances
    double leftLungCompliance_L_Per_cmH2O = 0.0;
    double rightLungCompliance_L_Per_cmH2O = 0.0;
    double leftChestWallCompliance_L_Per_cmH2O = 0.0;
    double rightChestWallCompliance_L_Per_cmH2O = 0.0;

    double zeroApprox = 1e-4;
    double dampenFraction_perSec = 0.005 * 50.0;

    if (std::abs(leftLungPressureChange_cmH2O) > zeroApprox)
    {
      leftLungCompliance_L_Per_cmH2O = leftLungVolumeChange_L / leftLungPressureChange_cmH2O;
    }
    if (std::abs(rightLungPressureChange_cmH2O) > zeroApprox)
    {
      rightLungCompliance_L_Per_cmH2O = rightLungVolumeChange_L / rightLungPressureChange_cmH2O;
    }
    if (std::abs(leftPleuralPressureChange_cmH2O) > zeroApprox)
    {
      leftChestWallCompliance_L_Per_cmH2O = leftPleuralVolumeChange_L / leftPleuralPressureChange_cmH2O;
    }
    if (std::abs(rightPleuralPressureChange_cmH2O) > zeroApprox)
    {
      rightChestWallCompliance_L_Per_cmH2O = rightPleuralVolumeChange_L / rightPleuralPressureChange_cmH2O;
    }

    // Calculate total compliances
    // Total lung compliance (lungs in parallel)
    double lungCompliance_L_Per_cmH2O = leftLungCompliance_L_Per_cmH2O + rightLungCompliance_L_Per_cmH2O;
    if (lungCompliance_L_Per_cmH2O > zeroApprox)
    {
      //Dampen the change to prevent potential craziness
      double previousLungCompliance_L_Per_cmH2O = GetLungCompliance(VolumePerPressureUnit::L_Per_cmH2O);
      lungCompliance_L_Per_cmH2O = GeneralMath::Damper(lungCompliance_L_Per_cmH2O, previousLungCompliance_L_Per_cmH2O, dampenFraction_perSec, m_data.GetTimeStep_s());
      GetLungCompliance().SetValue(lungCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    }

    // Total chest wall compliance (pleural cavities in parallel)
    double chestWallCompliance_L_Per_cmH2O = leftChestWallCompliance_L_Per_cmH2O + rightChestWallCompliance_L_Per_cmH2O;
    if (chestWallCompliance_L_Per_cmH2O > zeroApprox)
    {
      //Dampen the change to prevent potential craziness
      double previousChestWallCompliance_L_Per_cmH2O = GetChestWallCompliance(VolumePerPressureUnit::L_Per_cmH2O);
      chestWallCompliance_L_Per_cmH2O = GeneralMath::Damper(chestWallCompliance_L_Per_cmH2O, previousChestWallCompliance_L_Per_cmH2O, dampenFraction_perSec, m_data.GetTimeStep_s());
      GetChestWallCompliance().SetValue(chestWallCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    }

    // Compute overall respiratory system compliance
    double leftSideCompliance_L_Per_cmH2O = 0.0;
    if (leftChestWallCompliance_L_Per_cmH2O > zeroApprox && leftLungCompliance_L_Per_cmH2O > zeroApprox)
    {
      leftSideCompliance_L_Per_cmH2O = 1.0 / (1.0 / leftChestWallCompliance_L_Per_cmH2O + 1.0 / leftLungCompliance_L_Per_cmH2O);
    }
    double rightSideCompliance_L_Per_cmH2O = 0.0;
    if (rightChestWallCompliance_L_Per_cmH2O > zeroApprox && rightLungCompliance_L_Per_cmH2O > zeroApprox)
    {
      rightSideCompliance_L_Per_cmH2O = 1.0 / (1.0 / rightChestWallCompliance_L_Per_cmH2O + 1.0 / rightLungCompliance_L_Per_cmH2O);
    }

    // Weight it by volume change for things like mainstem intubation and pneumothorax
    double totalVolumeChange_L = std::abs(leftLungVolumeChange_L) + std::abs(rightLungVolumeChange_L);
    double leftVolumeChangeWeight = 1.0;  // Default to 1.0 for 50/50 split (2.0 * 0.5 = 1.0)
    double rightVolumeChangeWeight = 1.0; // Default to 1.0 for 50/50 split (2.0 * 0.5 = 1.0)

    if (std::abs(totalVolumeChange_L) > zeroApprox)
    {
      double leftFraction = std::abs(leftLungVolumeChange_L) / totalVolumeChange_L;
      double rightFraction = std::abs(rightLungVolumeChange_L) / totalVolumeChange_L;

      leftVolumeChangeWeight = 2.0 * leftFraction;   // Scale weight by fraction
      rightVolumeChangeWeight = 2.0 * rightFraction; // Scale weight by fraction

      leftVolumeChangeWeight = LIMIT(leftVolumeChangeWeight, 0.0, 1.0);
      rightVolumeChangeWeight = LIMIT(rightVolumeChangeWeight, 0.0, 1.0);
    }

    double respiratoryCompliance_L_Per_cmH2O = leftVolumeChangeWeight * leftSideCompliance_L_Per_cmH2O + rightVolumeChangeWeight * rightSideCompliance_L_Per_cmH2O;
    if (respiratoryCompliance_L_Per_cmH2O > zeroApprox)
    {
      //Dampen the change to prevent potential craziness
      double previousRespiratoryCompliance_L_Per_cmH2O = GetRespiratoryCompliance(VolumePerPressureUnit::L_Per_cmH2O);
      respiratoryCompliance_L_Per_cmH2O = GeneralMath::Damper(respiratoryCompliance_L_Per_cmH2O, previousRespiratoryCompliance_L_Per_cmH2O, dampenFraction_perSec, m_data.GetTimeStep_s());
      GetRespiratoryCompliance().SetValue(respiratoryCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
      GetRespiratoryElastance().SetValue(1.0 / respiratoryCompliance_L_Per_cmH2O, PressurePerVolumeUnit::cmH2O_Per_L);
    }

    // Store values for next calculation
    m_PreviousLeftLungPressureDiff_cmH2O = leftLungPressureDiff_cmH2O;
    m_PreviousRightLungPressureDiff_cmH2O = rightLungPressureDiff_cmH2O;
    m_PreviousLeftLungVolume_L = leftLungVolume_L;
    m_PreviousRightLungVolume_L = rightLungVolume_L;
    m_PreviousLeftPleuralPressureDiff_cmH2O = leftPleuralPressureDiff_cmH2O;
    m_PreviousRightPleuralPressureDiff_cmH2O = rightPleuralPressureDiff_cmH2O;
    m_PreviousLeftPleuralVolume_L = leftPleuralVolume_L;
    m_PreviousRightPleuralVolume_L = rightPleuralVolume_L;


    //It's a pain to figure out how to hold onto this data, so let's just set it at a sensitive transition point
    if (GetInspiratoryFlow(VolumePerTimeUnit::L_Per_s) > 0.0 //We're inhaling
      && previousInspiratoryFlow_L_Per_s <= 0.0) //We were exhaling
    {
      //Transition from exhale to inhale
      // Set the End Tidal Concentration for our gases at the end of a respiration cycle
      for (SEGasSubstanceQuantity* subQ : m_Carina->GetSubstanceQuantities())
      {
        subQ->GetSubstance().GetEndTidalFraction().Set(subQ->GetVolumeFraction());
        subQ->GetSubstance().GetEndTidalPressure().Set(subQ->GetPartialPressure());
      }
      GetEndTidalCarbonDioxideFraction().Set(m_data.GetSubstances().GetCO2().GetEndTidalFraction());
      GetEndTidalCarbonDioxidePressure().Set(m_data.GetSubstances().GetCO2().GetEndTidalPressure());
      GetEndTidalOxygenFraction().Set(m_data.GetSubstances().GetO2().GetEndTidalFraction());
      GetEndTidalOxygenPressure().Set(m_data.GetSubstances().GetO2().GetEndTidalPressure());
    }

    //Record values at the breathing inflection points (i.e. switch between inhale and exhale)  
    // Temporal tolerance to avoid accidental entry in the the inhalation and exhalation code blocks 
    m_ElapsedBreathingCycleTime_min += m_data.GetTimeStep_s() / 60.0;

    if (m_BreathingCycle) //Exhaling
    {
      if (totalLungVolume_L <= m_BottomBreathTotalVolume_L)
      {
        m_BottomBreathTotalVolume_L = totalLungVolume_L;
        m_BottomBreathElapsedTime_min = m_ElapsedBreathingCycleTime_min - m_TopBreathElapsedTime_min;
        m_BottomBreathAlveoliPressure_cmH2O = alveolarPressure_cmH2O;
        m_BottomBreathAirwayPressure_cmH2O = airwayOpeningPressure_cmH2O;

        unsigned int iter = 0;
        for (auto& itr : m_LungComponents)
        {
          LungComponent& cpt = itr.second;
          double alveoliVolume_L = cpt.AlveoliCompartment->GetVolume(VolumeUnit::L);
          m_BottomBreathAcinarZoneVolumes_L.at(iter) = alveoliVolume_L;
          iter++;
        }
      }

      if (totalLungVolume_L - m_BottomBreathTotalVolume_L > m_MinimumAllowableTidalVolume_L //Volume has transitioned sufficiently
        && m_BottomBreathElapsedTime_min > m_MinimumAllowableInpiratoryAndExpiratoryPeriod_s / 60.0 / 10.0) //We've waited a sufficient amount of time to transition
      {
        //Transition to inhale

        // Calculate Respiration Rate and track time and update cycle flag
        double expirationTime_min = m_BottomBreathElapsedTime_min;
        double inspirationTime_min = m_TopBreathElapsedTime_min;
        double RespirationRate_Per_min = 1.0 / (expirationTime_min + inspirationTime_min);
        GetRespirationRate().SetValue(RespirationRate_Per_min, FrequencyUnit::Per_min);
        GetInspiratoryExpiratoryRatio().SetValue(inspirationTime_min / expirationTime_min);

        m_ElapsedBreathingCycleTime_min -= (expirationTime_min + inspirationTime_min);
        m_BreathingCycle = false;

        // Calculate the Tidal Volume from the last peak
        double TidalVolume_L = std::abs(m_TopBreathTotalVolume_L - m_BottomBreathTotalVolume_L);
        GetTidalVolume().SetValue(TidalVolume_L, VolumeUnit::L);
        GetExpiratoryTidalVolume().SetValue(TidalVolume_L, VolumeUnit::L);

        double totalPositiveEndExpiratoryPressure_cmH2O = m_BottomBreathAlveoliPressure_cmH2O - bodySurfacePressure_cmH2O;
        double extrinsicPositiveEndExpiratoryPressure_cmH2O = m_BottomBreathAirwayPressure_cmH2O - bodySurfacePressure_cmH2O;
        double intrinsicPositiveEndExpiratoryPressure_cmH2O = totalPositiveEndExpiratoryPressure_cmH2O - extrinsicPositiveEndExpiratoryPressure_cmH2O;
        GetTotalPositiveEndExpiratoryPressure().SetValue(totalPositiveEndExpiratoryPressure_cmH2O, PressureUnit::cmH2O);
        GetExtrinsicPositiveEndExpiratoryPressure().SetValue(extrinsicPositiveEndExpiratoryPressure_cmH2O, PressureUnit::cmH2O);
        GetIntrinsicPositiveEndExpiratoryPressure().SetValue(intrinsicPositiveEndExpiratoryPressure_cmH2O, PressureUnit::cmH2O);

        GetPeakInspiratoryPressure().SetValue(m_PeakAlveolarPressure_cmH2O - bodySurfacePressure_cmH2O, PressureUnit::cmH2O);
        GetMaximalInspiratoryPressure().SetValue(m_MaximalAlveolarPressure_cmH2O - bodySurfacePressure_cmH2O, PressureUnit::cmH2O);

        // Calculate Ventilations
        GetTotalPulmonaryVentilation().SetValue(TidalVolume_L * RespirationRate_Per_min, VolumePerTimeUnit::L_Per_min);
        GetSpecificVentilation().SetValue(TidalVolume_L / m_BottomBreathTotalVolume_L);
        GetTotalDeadSpaceVentilation().SetValue((AnatomicDeadSpace_L + AlveolarDeadSpace_L) * RespirationRate_Per_min, VolumePerTimeUnit::L_Per_min);

        // Calculate Compartment Ventilations
        double totalAlveolarVentilation_L_Per_min = 0.0;
        unsigned int iter = 0;
        for (auto& itr : m_LungComponents)
        {
          LungComponent& cpt = itr.second;
          SEGasCompartment* acinarCompartment = cpt.AlveoliCompartment;
          SELiquidCompartment* acinarCapillaryCompartment = cpt.CapillaryCompartment;

          double ventilation_L_Per_min = (m_TopBreathAcinarZoneVolumes_L.at(iter) - m_BottomBreathAcinarZoneVolumes_L.at(iter)) * RespirationRate_Per_min;
          totalAlveolarVentilation_L_Per_min += ventilation_L_Per_min;
          acinarCompartment->GetVentilation().SetValue(ventilation_L_Per_min, VolumePerTimeUnit::L_Per_min);

          if (m_data.HasCardiovascular())
          {
            double perfusion_L_Per_min = acinarCapillaryCompartment->GetPerfusion(VolumePerTimeUnit::mL_Per_s);
            if (perfusion_L_Per_min > ZERO_APPROX)
            {
              acinarCompartment->GetVentilationPerfusionRatio().SetValue(ventilation_L_Per_min / perfusion_L_Per_min);
            }
            else
            {
              acinarCompartment->GetVentilationPerfusionRatio().SetValue(0.0);
            }
          }

          iter++;
        }

        GetTotalAlveolarVentilation().SetValue(totalAlveolarVentilation_L_Per_min, VolumePerTimeUnit::L_Per_min);

        //Calculate Ratios
        //Vd/Vt is based on the patient healthy spontaneous tidal volume
        double tidalVolumeBaseline_L = m_data.GetInitialPatient().GetTidalVolumeBaseline(VolumeUnit::L);
        GetPhysiologicDeadSpaceTidalVolumeRatio().SetValue(GetPhysiologicDeadSpace(VolumeUnit::L) / tidalVolumeBaseline_L);

        //Bohr-Enghoff equation
        double clinicalPhysiologicDeadSpaceTidalVolumeRatio = (m_ArterialCO2PartialPressure_mmHg - m_MixedExpiredCO2RunningAverage_mmHg->Value()) / m_ArterialCO2PartialPressure_mmHg;
        GetClinicalPhysiologicDeadSpaceTidalVolumeRatio().SetValue(clinicalPhysiologicDeadSpaceTidalVolumeRatio);
        m_MixedExpiredCO2RunningAverage_mmHg->Invalidate();

        if (m_data.HasCardiovascular())
        {
          if (m_data.GetCardiovascular().GetTotalPulmonaryPerfusion(VolumePerTimeUnit::L_Per_min) > ZERO_APPROX)
          {
            GetVentilationPerfusionRatio().SetValue(GetTotalPulmonaryVentilation(VolumePerTimeUnit::L_Per_min) / m_data.GetCardiovascular().GetTotalPulmonaryPerfusion(VolumePerTimeUnit::L_Per_min));
          }
          else
          {
            GetVentilationPerfusionRatio().SetValue(0.0);
          }
        }
        else
        {
          GetVentilationPerfusionRatio().Invalidate();
        }

        m_TopBreathTotalVolume_L = totalLungVolume_L;
        m_TopBreathElapsedTime_min = m_ElapsedBreathingCycleTime_min;
        m_PeakAlveolarPressure_cmH2O = m_BottomBreathAlveoliPressure_cmH2O;
        m_MaximalAlveolarPressure_cmH2O = m_BottomBreathAlveoliPressure_cmH2O;

        //We can approximate the mean here, since we got a full waveform
        //It will be off a little when each breath isn't the same
        //It's too hard to keep a runnning average otherwise
        GetMeanAirwayPressure().SetValue(m_MeanAirwayPressure_cmH2O->Value(), PressureUnit::cmH2O);
        m_MeanAirwayPressure_cmH2O->Invalidate();
      }
    }
    else //Inhaling
    {
      if (totalLungVolume_L >= m_TopBreathTotalVolume_L)
      {
        m_TopBreathTotalVolume_L = totalLungVolume_L;
        m_TopBreathElapsedTime_min = m_ElapsedBreathingCycleTime_min;
        m_TopCarinaO2 = m_CarinaO2->GetVolumeFraction().GetValue();

        unsigned int iter = 0;
        for (auto& itr : m_LungComponents)
        {
          LungComponent& cpt = itr.second;
          double alveoliVolume_L = cpt.AlveoliCompartment->GetVolume(VolumeUnit::L);
          m_TopBreathAcinarZoneVolumes_L.at(iter) = alveoliVolume_L;
          iter++;
        }
      }

      m_PeakAlveolarPressure_cmH2O = MAX(m_PeakAlveolarPressure_cmH2O, alveolarPressure_cmH2O);
      m_MaximalAlveolarPressure_cmH2O = MIN(m_MaximalAlveolarPressure_cmH2O, alveolarPressure_cmH2O);

      if (m_TopBreathTotalVolume_L - totalLungVolume_L > m_MinimumAllowableTidalVolume_L //Volume has transitioned sufficiently
        && m_TopBreathElapsedTime_min > m_MinimumAllowableInpiratoryAndExpiratoryPeriod_s / 60.0 / 2.0) //We've waited a sufficient amount of time to transition
      {
        //Transition to exhaling
        m_BreathingCycle = true;

        // Calculate the Tidal Volume from the last trough
        double TidalVolume_L = std::abs(m_TopBreathTotalVolume_L - m_BottomBreathTotalVolume_L);
        GetInspiratoryTidalVolume().SetValue(TidalVolume_L, VolumeUnit::L);

        //We want the peak Carina O2 value - this should be the inspired value
        double FiO2 = m_TopCarinaO2;
        GetFractionOfInspiredOxygen().SetValue(FiO2);
        double meanAirwayPressure_mmHg = Convert(m_MeanAirwayPressure_cmH2O->Value(), PressureUnit::cmH2O, PressureUnit::mmHg);

        if (m_data.HasCardiovascular())
        {
          GetHorowitzIndex().SetValue(m_ArterialO2PartialPressure_mmHg / FiO2, PressureUnit::mmHg);
          GetOxygenationIndex().SetValue(FiO2 * meanAirwayPressure_mmHg * 100.0 / m_ArterialO2PartialPressure_mmHg);
        }
        else
        {
          GetHorowitzIndex().Invalidate();
          GetOxygenationIndex().Invalidate();
        }

        if (m_data.HasBloodChemistry())
        {
          GetSaturationAndFractionOfInspiredOxygenRatio().SetValue(m_data.GetBloodChemistry().GetOxygenSaturation().GetValue() / FiO2);
          GetOxygenSaturationIndex().SetValue(FiO2 * meanAirwayPressure_mmHg / m_data.GetBloodChemistry().GetOxygenSaturation().GetValue(), PressureUnit::cmH2O);
        }
        else
        {
          GetSaturationAndFractionOfInspiredOxygenRatio().Invalidate();
          GetOxygenSaturationIndex().Invalidate();
        }

        m_BottomBreathTotalVolume_L = totalLungVolume_L;
        m_BottomBreathElapsedTime_min = m_ElapsedBreathingCycleTime_min - m_TopBreathElapsedTime_min;
      }
    }

    // Zero out if not breathing with out any assistance
    if ((m_data.GetAirwayMode() == eAirwayMode::Free && m_NotBreathing) || m_ElapsedBreathingCycleTime_min > 0.25)
    {
      GetRespirationRate().SetValue(0.0, FrequencyUnit::Per_min);
      GetTidalVolume().SetValue(0.0, VolumeUnit::L);
      GetExpiratoryTidalVolume().SetValue(0.0, VolumeUnit::L);
      GetInspiratoryTidalVolume().SetValue(0.0, VolumeUnit::L);
      GetTotalAlveolarVentilation().SetValue(0.0, VolumePerTimeUnit::L_Per_min);
      GetTotalPulmonaryVentilation().SetValue(0.0, VolumePerTimeUnit::L_Per_min);
      GetMeanAirwayPressure().SetValue(m_MeanAirwayPressure_cmH2O->Value(), PressureUnit::cmH2O);
      m_MeanAirwayPressure_cmH2O->Invalidate();
      GetInspiratoryExpiratoryRatio().SetValue(0);
      GetPeakInspiratoryPressure().SetValue(0, PressureUnit::cmH2O);
      GetExtrinsicPositiveEndExpiratoryPressure().SetValue(0.0, PressureUnit::cmH2O);
      GetIntrinsicPositiveEndExpiratoryPressure().SetValue(0.0, PressureUnit::cmH2O);
      GetTotalPositiveEndExpiratoryPressure().SetValue(0.0, PressureUnit::cmH2O);
      GetMaximalInspiratoryPressure().SetValue(0, PressureUnit::cmH2O);
      GetSpecificVentilation().SetValue(0);
      GetTotalDeadSpaceVentilation().SetValue(0, VolumePerTimeUnit::L_Per_min);
      GetEndTidalCarbonDioxideFraction().SetValue(0);
      GetEndTidalCarbonDioxidePressure().SetValue(0, PressureUnit::cmH2O);
      GetEndTidalOxygenFraction().SetValue(0);
      GetEndTidalOxygenPressure().SetValue(0, PressureUnit::cmH2O);

      for (SESubstance* sub : m_data.GetSubstances().GetActiveGases())
      {
        sub->GetEndTidalFraction().SetValue(0);
        sub->GetEndTidalPressure().SetValue(0, PressureUnit::cmH2O);
      }

      m_MixedExpiredCO2RunningAverage_mmHg->Invalidate();
    }

    if (m_data.GetState() > EngineState::InitialStabilization)
    {// Don't throw events if we are initializing

      //Bradypnea
      if (GetRespirationRate().GetValue(FrequencyUnit::Per_min) < 10)
      {
        /// The patient has bradypnea.
        m_data.GetEvents().SetEvent(eEvent::Bradypnea, true, m_data.GetSimulationTime());  /// \cite overdyk2007continuous
      }
      else if (GetRespirationRate().GetValue(FrequencyUnit::Per_min) >= 10.5)  // offset by .5 
      {
        /// The patient is no longer considered to have bradypnea.
        m_data.GetEvents().SetEvent(eEvent::Bradypnea, false, m_data.GetSimulationTime());
      }

      //Tachypnea
      if (GetRespirationRate().GetValue(FrequencyUnit::Per_min) > 20)
      {
        /// The patient has tachypnea.
        m_data.GetEvents().SetEvent(eEvent::Tachypnea, true, m_data.GetSimulationTime());  /// \cite 
      }
      else if (GetRespirationRate().GetValue(FrequencyUnit::Per_min) <= 19.5) // offset by .5 
      {
        /// The patient is no longer considered to have tachypnea.
        m_data.GetEvents().SetEvent(eEvent::Tachypnea, false, m_data.GetSimulationTime());
      }
    }

    if (m_data.HasBloodChemistry() && m_data.HasCardiovascular())
    {
      /// \todo Move to blood chemistry
       // Although it is called respiratory acidosis/alkalosis, the detection and event setting is actually a part of the @ref BloodChemistrySystem
       // The terms "metabolic" and "respiratory" refer to the origin of the acid-base disturbance
       // The hydrogen ion concentration is a property of the blood
       // The events related to blood concentrations should be detected and set in blood chemistry.
       //Keep a running average of the pH
      m_BloodPHRunningAverage->Sample(m_data.GetBloodChemistry().GetBloodPH().GetValue());
      //Reset at start of cardiac cycle 
      if (m_data.GetEvents().IsEventActive(eEvent::StartOfCardiacCycle))
      {
        m_LastCardiacCycleBloodPH = m_BloodPHRunningAverage->Value();
        m_BloodPHRunningAverage->Invalidate();
      }

      if (m_data.GetState() > EngineState::InitialStabilization)
      {// Don't throw events if we are initializing

        //double highPh = 8.5;
        //double lowPh = 6.5;   //\cite Edge2006AcidosisConscious
        //// Respiratory Acidosis
        if (m_LastCardiacCycleBloodPH < 7.35 && m_ArterialCO2PartialPressure_mmHg > 47.0)
        {
          /// The patient has respiratory acidosis.
          m_data.GetEvents().SetEvent(eEvent::RespiratoryAcidosis, true, m_data.GetSimulationTime());

        }
        else if (m_LastCardiacCycleBloodPH >= 7.38 && m_ArterialCO2PartialPressure_mmHg < 44.0)
        {
          /// The patient is no longer considered to have respiratory acidosis.
          m_data.GetEvents().SetEvent(eEvent::RespiratoryAcidosis, false, m_data.GetSimulationTime());
        }

        ////Respiratory Alkalosis
        if (m_LastCardiacCycleBloodPH > 7.45 && m_ArterialCO2PartialPressure_mmHg < 37.0)
        {
          /// The patient has respiratory alkalosis.
          m_data.GetEvents().SetEvent(eEvent::RespiratoryAlkalosis, true, m_data.GetSimulationTime());

        }
        else if (m_LastCardiacCycleBloodPH <= 7.43 && m_ArterialCO2PartialPressure_mmHg > 39.0)
        {
          /// The patient is no longer considered to have respiratory alkalosis.
          m_data.GetEvents().SetEvent(eEvent::RespiratoryAlkalosis, false, m_data.GetSimulationTime());
        }
      }
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Peak Driver Pressure Based on Target Volume 
  ///
  /// \param  TargetVolume_L        Target volume (L)
  ///
  /// \return Peak Driver Pressure    The driver pressure required to achieve target volume (cmH2O)
  ///
  /// \details
  /// This method returns an estimated driver pressure as a function of target volume. The equation
  /// uses a constant total compliance from the basline values. This is an approximation and will
  /// not acheive the desired volume exactly.
  //--------------------------------------------------------------------------------------------------
  double RespiratoryModel::VolumeToDriverPressure(double TargetVolume_L)
  {
    //Calculate as if constant compliances
    //jbw - Either sum all paths or add a configuration value
    double leftHealthyLungCompliance_L_Per_cmH2O = 0.1;
    double rightHealthyLungCompliance_L_Per_cmH2O = 0.1;
    if (m_data.GetConfiguration().UseExpandedRespiratory() == eSwitch::Off)
    {
      leftHealthyLungCompliance_L_Per_cmH2O = m_LeftAlveoliToLeftPleuralConnection->GetComplianceBaseline(VolumePerPressureUnit::L_Per_cmH2O);
      rightHealthyLungCompliance_L_Per_cmH2O = m_RightAlveoliToRightPleuralConnection->GetComplianceBaseline(VolumePerPressureUnit::L_Per_cmH2O);
    }

    double leftHealthyChestWallCompliance_L_Per_cmH2O = m_LeftPleuralToRespiratoryMuscle->GetComplianceBaseline(VolumePerPressureUnit::L_Per_cmH2O);
    double rightHealthyChestWallCompliance_L_Per_cmH2O = m_RightPleuralToRespiratoryMuscle->GetComplianceBaseline(VolumePerPressureUnit::L_Per_cmH2O);
    
    double totalCompliance_L_Per_cmH2O = 1.0 / (1.0 / leftHealthyChestWallCompliance_L_Per_cmH2O + 1.0 / leftHealthyLungCompliance_L_Per_cmH2O) +
      1.0 / (1.0 / rightHealthyChestWallCompliance_L_Per_cmH2O + 1.0 / rightHealthyLungCompliance_L_Per_cmH2O);

    double driverPressure_cmH2O = -(TargetVolume_L - m_data.GetInitialPatient().GetFunctionalResidualCapacity(VolumeUnit::L)) / totalCompliance_L_Per_cmH2O;
    return driverPressure_cmH2O;
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Run the circuit standalone once to get it into a good starting state 
  /// - keep volume fractions constant
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::TuneCircuit()
  {
    SEFluidCircuit& RespiratoryCircuit = m_data.GetCircuits().GetRespiratoryCircuit();
    //Set the starting/default driver pressure
    m_DriverPressurePath->GetNextPressureSource().Set(m_DriverPressurePath->GetPressureSourceBaseline());
    m_Calculator->Process(RespiratoryCircuit, m_data.GetTimeStep_s());
    m_Calculator->PostProcess(RespiratoryCircuit);

    //Make sure the new volumes are accounted for with all the substance stuff
    //Keep the same volume fraction originally initialized
    SEGasCompartmentGraph* RespiratoryGraph = &m_data.GetCompartments().GetRespiratoryGraph();
    for (SEGasCompartment* compartment : RespiratoryGraph->GetCompartments())
    {
      if (compartment->HasVolume())
        compartment->Balance(BalanceGasBy::VolumeFraction);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Modify the chest wall compliance based on lung volume.
  ///
  /// \details
  /// The chest wall compliance is modified using a sigmoidal function. Extreme values are defined by 
  /// a volume vs. pressure curve between reserve volume (minimum lung volume) and total lung capacity
  /// (maximum lung volume). The instantaneous compliance is calculated each time step to follow this 
  /// defined volume vs. pressure piecewise function.
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::UpdateChestWallCompliances()
  {
    double rightLungRatio = m_data.GetCurrentPatient().GetRightLungRatio().GetValue();
    double leftLungRatio = 1.0 - rightLungRatio;

    for (unsigned int iterLung = 0; iterLung < 2; iterLung++)
    {
      //0 = right lung, 1 = left lung

      // Get properties ---------------------------------------------------------

      double lungRatio = 0.0;
      double lungVolume_L = 0.0;
      SEFluidCircuitPath* chestWallPath = nullptr;
      SEFluidCircuitPath* lungPath = nullptr;
      double healthyChestWallCompliance_L_Per_cmH2O = 0.0;
      double healthyLungCompliance_L_Per_cmH2O = 0.0;
      double healthySideCompliance_L_Per_cmH2O = 0.0;

      SESegment* segment = nullptr;
      const std::vector<SESegment*>* segments = nullptr;
      bool hasRespiratoryMechanicsCompliance = false;

      if (iterLung == 0) //right lung
      {
        lungRatio = rightLungRatio;
        lungVolume_L = m_RightLung->GetVolume(VolumeUnit::L) - m_RightAlveolarDeadSpace->GetVolume(VolumeUnit::L) + m_RightAlveoliDecrease_L;
        chestWallPath = m_RightPleuralToRespiratoryMuscle;
        lungPath = m_RightAlveoliToRightPleuralConnection;
        healthyChestWallCompliance_L_Per_cmH2O = m_RightPleuralToRespiratoryMuscle->GetComplianceBaseline(VolumePerPressureUnit::L_Per_cmH2O);

        if (HasActiveMechanics())
        {
          hasRespiratoryMechanicsCompliance = m_Mechanics->HasRightComplianceCurve();
          if (hasRespiratoryMechanicsCompliance)
          {
            segments = &m_Mechanics->GetRightComplianceCurve().GetSegments();
          }
        }
      }
      else //left lung
      {
        lungRatio = leftLungRatio;
        lungVolume_L = m_LeftLung->GetVolume(VolumeUnit::L) - m_LeftAlveolarDeadSpace->GetVolume(VolumeUnit::L) + m_LeftAlveoliDecrease_L;
        chestWallPath = m_LeftPleuralToRespiratoryMuscle;
        lungPath = m_LeftAlveoliToLeftPleuralConnection;
        healthyChestWallCompliance_L_Per_cmH2O = m_LeftPleuralToRespiratoryMuscle->GetComplianceBaseline(VolumePerPressureUnit::L_Per_cmH2O);

        if (HasActiveMechanics())
        {
          hasRespiratoryMechanicsCompliance = m_Mechanics->HasLeftComplianceCurve();
          if (hasRespiratoryMechanicsCompliance)
          {
            segments = &m_Mechanics->GetLeftComplianceCurve().GetSegments();
          }
        }
      }

      // Define/Get compliance curve ---------------------------------------------------------

      double functionalResidualCapacity_L = m_data.GetInitialPatient().GetFunctionalResidualCapacity(VolumeUnit::L) * lungRatio;

      if (m_data.GetConfiguration().UseExpandedRespiratory() == eSwitch::On)
      {
        //jbw - Either sum all paths or add a configuration value
        healthyLungCompliance_L_Per_cmH2O = 0.1;
      }
      else
      {
        healthyLungCompliance_L_Per_cmH2O = lungPath->GetComplianceBaseline(VolumePerPressureUnit::L_Per_cmH2O);
      }

      double baselineCompliance_L_Per_cmH2O = 0.0;
      double lowerCorner_cmH2O = 0.0;
      double upperCorner_cmH2O = 0.0;

      if (hasRespiratoryMechanicsCompliance)
      {
        //Specified externally
        segment = GetSegement(*segments, lungVolume_L);
      }
      else
      {
        /// \cite venegas1998comprehensive

        double residualVolume_L = m_data.GetInitialPatient().GetResidualVolume(VolumeUnit::L) * lungRatio;
        double vitalCapacity_L = m_data.GetInitialPatient().GetVitalCapacity(VolumeUnit::L) * lungRatio;
        baselineCompliance_L_Per_cmH2O = 1.0 / (1.0 / healthyChestWallCompliance_L_Per_cmH2O + 1.0 / healthyLungCompliance_L_Per_cmH2O);
        // \\\todo Figure out why the upper corner uses vital capacity instead of total lung capacity
        upperCorner_cmH2O = (vitalCapacity_L - functionalResidualCapacity_L) / baselineCompliance_L_Per_cmH2O;
        lowerCorner_cmH2O = (residualVolume_L - functionalResidualCapacity_L) / baselineCompliance_L_Per_cmH2O;
      }

      // Apply compliance curve ---------------------------------------------------------

      double minCompliance_L_Per_cmH2O = 0.01; //Minimum possible compliance
      double sideCompliance_L_Per_cmH2O = minCompliance_L_Per_cmH2O;

      const SESegmentSigmoidal* sigmoidal = dynamic_cast<const SESegmentSigmoidal*>(segment);
      const SESegmentConstant* constant = dynamic_cast<const SESegmentConstant*>(segment);
      const SESegmentLinear* linear = dynamic_cast<const SESegmentLinear*>(segment);
      const SESegmentParabolic* parabolic = dynamic_cast<const SESegmentParabolic*>(segment);

      double chestWallCompliance_L_Per_cmH2O = healthyChestWallCompliance_L_Per_cmH2O;

      if (segment == nullptr &&
        HasActiveMechanics() &&
        m_Mechanics->GetDefaultType() == eDefaultType::Zero)
      {
        //Left blank, so set to zero
        chestWallPath->GetNextCompliance().SetValue(0.0, VolumePerPressureUnit::L_Per_cmH2O);
        lungPath->GetNextCompliance().SetValue(0.0, VolumePerPressureUnit::L_Per_cmH2O);
        continue;
      }
      else if (segment == nullptr || //default
        sigmoidal != nullptr)
      {
        if (segment != nullptr)
        {
          baselineCompliance_L_Per_cmH2O = sigmoidal->GetBaselineCompliance(VolumePerPressureUnit::L_Per_cmH2O);
          lowerCorner_cmH2O = sigmoidal->GetLowerCorner(PressureUnit::cmH2O);
          upperCorner_cmH2O = sigmoidal->GetUpperCorner(PressureUnit::cmH2O);
        }

        double d = (upperCorner_cmH2O - lowerCorner_cmH2O) / 4.0;
        double c = lowerCorner_cmH2O + 2.0 * d;
        double residualVolume_L = baselineCompliance_L_Per_cmH2O * lowerCorner_cmH2O + functionalResidualCapacity_L;
        double vitalCapacity_L = baselineCompliance_L_Per_cmH2O * upperCorner_cmH2O + functionalResidualCapacity_L;

        healthySideCompliance_L_Per_cmH2O = baselineCompliance_L_Per_cmH2O;
        healthyChestWallCompliance_L_Per_cmH2O = 1.0 / (1.0 / healthySideCompliance_L_Per_cmH2O - 1.0 / healthyLungCompliance_L_Per_cmH2O);

        if (lungVolume_L > residualVolume_L && lungVolume_L < vitalCapacity_L + residualVolume_L)
        {
          double expectedPressure_cmH2O = d * log((lungVolume_L - residualVolume_L) / (residualVolume_L + vitalCapacity_L - lungVolume_L)) + c;
          double volumeAtZeroPressure = residualVolume_L + (vitalCapacity_L / (1.0 + exp(c / d)));

          sideCompliance_L_Per_cmH2O = healthySideCompliance_L_Per_cmH2O;
          if (!SEScalar::IsZero(expectedPressure_cmH2O, ZERO_APPROX))
          {
            //Not dividing by ~0
            sideCompliance_L_Per_cmH2O = (lungVolume_L - volumeAtZeroPressure) / expectedPressure_cmH2O;
          }

          if (SEScalar::IsZero(sideCompliance_L_Per_cmH2O, ZERO_APPROX))
          {
            sideCompliance_L_Per_cmH2O = healthySideCompliance_L_Per_cmH2O;
          }
          sideCompliance_L_Per_cmH2O = MAX(sideCompliance_L_Per_cmH2O, minCompliance_L_Per_cmH2O);
        }
      }
      else if (constant != nullptr)
      {
        sideCompliance_L_Per_cmH2O = constant->GetCompliance(VolumePerPressureUnit::L_Per_cmH2O);
      }
      else if (linear != nullptr)
      {
        //Assumes 0L at FRC
        double apparentVolume_L = lungVolume_L - functionalResidualCapacity_L;
        double a0 = linear->GetYIntercept(PressureUnit::cmH2O);
        double a1 = linear->GetSlope(PressurePerVolumeUnit::cmH2O_Per_L);
        double expectedPressure_cmH2O = a0 + a1 * apparentVolume_L;
        sideCompliance_L_Per_cmH2O = apparentVolume_L / expectedPressure_cmH2O;
      }
      else if (parabolic != nullptr)
      {
        //Assumes 0L at FRC
        double apparentVolume_L = lungVolume_L - functionalResidualCapacity_L;
        double a0 = parabolic->GetCoefficient1();
        double a1 = parabolic->GetCoefficient2();
        double a2 = parabolic->GetCoefficient3();
        double a3 = parabolic->GetCoefficient4();
        double expectedPressure_cmH2O = (a0 + a1 * apparentVolume_L + a2 * std::pow(apparentVolume_L, 2.0)) / (a3 - apparentVolume_L);
        sideCompliance_L_Per_cmH2O = apparentVolume_L / expectedPressure_cmH2O;
      }
      else
      {
        Error("Unknown respiratory compliance type.");
      }

      //Break up between healthy lung compliance and chest wall compliance
      //Set chest wall compliance only to allow for conditions/actions changing lung compliance

      //Can't divide by zero
      if (sideCompliance_L_Per_cmH2O != 0.0 && sideCompliance_L_Per_cmH2O != healthyLungCompliance_L_Per_cmH2O)
      {
        chestWallCompliance_L_Per_cmH2O = 1.0 / (1.0 / sideCompliance_L_Per_cmH2O - 1.0 / healthyLungCompliance_L_Per_cmH2O);
      }

      if (!hasRespiratoryMechanicsCompliance)
      {
        //Dampen the change to prevent potential craziness
        //It will only change half as much as it wants to each time step to ensure it's critically damped and doesn't overshoot
        double dampenFraction_perSec = 0.5 * 50.0; //Default timestep = 20ms = 50Hz, ensuring any timestep will work the same here
        double previousChestWallCompliance_L_Per_cmH2O = chestWallPath->GetCompliance(VolumePerPressureUnit::L_Per_cmH2O);
        chestWallCompliance_L_Per_cmH2O = GeneralMath::Damper(chestWallCompliance_L_Per_cmH2O, previousChestWallCompliance_L_Per_cmH2O, dampenFraction_perSec, m_data.GetTimeStep_s());
      }

      chestWallPath->GetNextCompliance().SetValue(chestWallCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculate the instantaneous work of breathing.
  ///
  /// \details
  /// ToDo
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::CalculateWork()
  {
    ///\ToDo Add work calculation
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculate and apply repiratory fatigue effects.
  ///
  /// \details
  /// ToDo
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::CalculateFatigue()
  {
    ///\ToDo Add fatigue calculation
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Modify lung volumes based on actions and conditions.
  ///
  /// \details
  /// Increase the alveolar dead space volume due to COPD. Also, make sure the stomach volume does
  /// not cause a problem with ambient changes.
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::UpdateVolumes()
  {
    //Don't modify the stomach on environment changes
    if (m_AmbientNode->GetNextPressure(PressureUnit::cmH2O) != m_AmbientNode->GetPressure(PressureUnit::cmH2O))
    {
      //The environment just changed
      //Keep volume the same by changing the pressure equally on both sides
      double pressureChange_cmH2O = m_AmbientNode->GetNextPressure(PressureUnit::cmH2O) - m_AmbientNode->GetPressure(PressureUnit::cmH2O);
      m_StomachNode->GetNextPressure().Increment(pressureChange_cmH2O, PressureUnit::cmH2O);
    }

    double functionalResidualCapacityChange_L = 0.0;
    double totalBaselineAlveoliVolume_L = 0.0;
    for (auto& itr : m_LungComponents)
    {
      LungComponent& cpt = itr.second;
      totalBaselineAlveoliVolume_L += cpt.AlveoliNode->GetVolumeBaseline(VolumeUnit::L);
    }

    double leftAlveoliDecrease_L = 0.0;
    double rightAlveoliDecrease_L = 0.0;

    double totalHealthyCompliance_L_Per_cmH2O = 0.0;
    double totalCompliance_L_Per_cmH2O = 0.0;

    unsigned int iter = 0;
    for (auto& itr : m_LungComponents)
    {
      double deadSpaceIncrement_L = 0.0;
      double alveoliIncrement_L = 0.0;

      eLungCompartment cmpt = itr.first;
      LungComponent& cpt = itr.second;

      SEFluidCircuitNode* deadSpaceNode = cpt.DeadSpaceNode;
      SEFluidCircuitNode* alveoliNode = cpt.AlveoliNode;

      double alveoliVolumeBaseline_L = alveoliNode->GetVolumeBaseline(VolumeUnit::L);
      double alveoliVolumeRatio = alveoliVolumeBaseline_L / totalBaselineAlveoliVolume_L;

      //------------------------------------------------------------------------------------------------------
      //Restrictive
      //The dead space cannot be greater than the FRC in our model
      double restrictiveSeverity = 0.0;

      //Pneumonia
      //Exacerbation will overwrite the condition, even if it means improvement
      if (m_data.GetConditions().HasPneumonia() || m_PatientActions->HasPneumoniaExacerbation())
      {
        double severity = 0.0;
        if (m_PatientActions->HasPneumoniaExacerbation())
        {
          severity = m_PatientActions->GetPneumoniaExacerbation().GetSeverity(cmpt).GetValue();
        }
        else
        {
          severity = m_data.GetConditions().GetPneumonia().GetSeverity(cmpt).GetValue();
        }

        restrictiveSeverity = MAX(restrictiveSeverity, severity);
      }

      //ARDS
      //Exacerbation will overwrite the condition, even if it means improvement
      if (m_data.GetConditions().HasAcuteRespiratoryDistressSyndrome() || m_PatientActions->HasAcuteRespiratoryDistressSyndromeExacerbation())
      {
        double severity = 0.0;
        if (m_PatientActions->HasAcuteRespiratoryDistressSyndromeExacerbation())
        {
          severity = m_PatientActions->GetAcuteRespiratoryDistressSyndromeExacerbation().GetSeverity(cmpt).GetValue();
        }
        else
        {
          severity = m_data.GetConditions().GetAcuteRespiratoryDistressSyndrome().GetSeverity(cmpt).GetValue();
        }

        restrictiveSeverity = MAX(restrictiveSeverity, severity);
      }

      //Pulmonary Fibrosis
      //Not per compartment, so just distribute to all evenly
      if (m_data.GetConditions().HasPulmonaryFibrosis())
      {
        double severity = m_data.GetConditions().GetPulmonaryFibrosis().GetSeverity().GetValue();

        restrictiveSeverity = MAX(restrictiveSeverity, severity);
      }

      //PBLI
      //Same as ARDS
      if (m_PatientActions->HasPrimaryBlastLungInjury())
      {
        double severity = m_PatientActions->GetPrimaryBlastLungInjury().GetSeverity(cmpt).GetValue();

        restrictiveSeverity = MAX(restrictiveSeverity, severity);
      }

      std::vector<std::pair<double, double>>  interpolatorPoints =
      {
        {0.0, 0.0}, //None
        {0.3, 0.12}, //Mild
        {0.6, 0.14}, //Moderate
        {0.9, 0.18}, //Severe
        {1.0, 0.2}  //Max
      };

      deadSpaceIncrement_L = alveoliVolumeRatio * GeneralMath::PiecewiseLinearInterpolator(interpolatorPoints, restrictiveSeverity);
      alveoliIncrement_L = -alveoliVolumeRatio * GeneralMath::LinearInterpolator(0.0, 1.0, 0.0, 1.2, restrictiveSeverity);

      //------------------------------------------------------------------------------------------------------
      //Obstructive

      //COPD
      //Exacerbation will overwrite the condition, even if it means improvement

      ///\cite guyton2006medical p504: In a person with partially functional or nonfunctional alveoli in some parts of the lungs, 
      //the physiological dead space may be as much as 10 times the volume of the anatomical dead space, or 1 to 2 liters.

      if (m_data.GetConditions().HasChronicObstructivePulmonaryDisease() || m_PatientActions->HasChronicObstructivePulmonaryDiseaseExacerbation())
      {
        double emphysemaSeverity = 0.0;
        if (m_PatientActions->HasChronicObstructivePulmonaryDiseaseExacerbation())
        {
          emphysemaSeverity = m_PatientActions->GetChronicObstructivePulmonaryDiseaseExacerbation().GetEmphysemaSeverity(cmpt).GetValue();
        }
        else
        {
          emphysemaSeverity = m_data.GetConditions().GetChronicObstructivePulmonaryDisease().GetEmphysemaSeverity(cmpt).GetValue();
        }

        interpolatorPoints =
        {
          {0.0, 0.0}, //None
          {0.3, 0.015}, //Mild
          {0.6, 0.050}, //Moderate
          {0.9, 0.120}, //Severe
          {1.0, 0.200}  //Max
        };

        deadSpaceIncrement_L = MAX(deadSpaceIncrement_L, GeneralMath::PiecewiseLinearInterpolator(interpolatorPoints, emphysemaSeverity));
      }

      //---------------------------------------------------------------------------------------------------------------------------------------------
      //Modify based on the specific patient
      double standardMaleIdealWeight_kg = 75.3;
      double standardFunctionalResidualCapacity_L = 30.0 * standardMaleIdealWeight_kg / 1000.0;
      double standardRedesidualVolume_L = 16.0 * standardMaleIdealWeight_kg / 1000.0;
      double pateintMultiplier = (m_data.GetInitialPatient().GetFunctionalResidualCapacity(VolumeUnit::L) - m_data.GetInitialPatient().GetResidualVolume(VolumeUnit::L))
        / (standardFunctionalResidualCapacity_L - standardRedesidualVolume_L);

      deadSpaceIncrement_L *= pateintMultiplier;
      alveoliIncrement_L *= pateintMultiplier;

      double deadSpace_L = deadSpaceNode->GetVolumeBaseline(VolumeUnit::L) + deadSpaceIncrement_L;

      //---------------------------------------------------------------------------------------------------------------------------------------------
      //Modifiers
      if (m_MechanicsModifiers->HasLungVolumeIncrement(cmpt))
      {
        double increment_L = m_MechanicsModifiers->GetLungVolumeIncrement(cmpt).GetValue(VolumeUnit::L);
        deadSpaceIncrement_L += increment_L;
        deadSpace_L += increment_L;
      }

      //---------------------------------------------------------------------------------------------------------------------------------------------
      //Update alveoli volume that participates in gas exchange

      //Only do this once on the timestep that it changes
      //Change both next and current volume to handle compliance properly
      //The increment only applies what has changed since last time
      if (alveoliIncrement_L != m_AlveoliVolumeIncrement_L[iter])
      {
        double currentAlveoliVolumeIncrement_L = alveoliIncrement_L - m_AlveoliVolumeIncrement_L[iter];

        alveoliNode->GetNextVolume().SetReadOnly(false);
        alveoliNode->GetVolume().SetReadOnly(false);
        alveoliNode->GetNextVolume().Increment(currentAlveoliVolumeIncrement_L, VolumeUnit::L);
        alveoliNode->GetVolume().Increment(currentAlveoliVolumeIncrement_L, VolumeUnit::L);
        alveoliNode->GetNextVolume().SetReadOnly(true);
        alveoliNode->GetVolume().SetReadOnly(true);

        SEFluidCircuitNode* pleuralNode;
        pleuralNode = cpt.Side == eSide::Right ? m_RightPleuralNode : m_LeftPleuralNode;

        pleuralNode->GetNextVolume().SetReadOnly(false);
        pleuralNode->GetVolume().SetReadOnly(false);
        pleuralNode->GetNextVolume().Increment(currentAlveoliVolumeIncrement_L, VolumeUnit::L);
        pleuralNode->GetVolume().Increment(currentAlveoliVolumeIncrement_L, VolumeUnit::L);
        pleuralNode->GetNextVolume().SetReadOnly(true);
        pleuralNode->GetVolume().SetReadOnly(true);

        m_AlveoliVolumeIncrement_L[iter] = alveoliIncrement_L;
      }

      if (cpt.Side == eSide::Left)
      {
        leftAlveoliDecrease_L -= alveoliIncrement_L;
      }
      else// if (cpt.Side == eSide::Right) // Assuming right if not left, this is all set internally in ::Setup()
      {
        rightAlveoliDecrease_L -= alveoliIncrement_L;
      }

      deadSpaceNode->GetNextVolume().SetValue(deadSpace_L, VolumeUnit::L);

      //Update lung volumes
      functionalResidualCapacityChange_L += deadSpaceIncrement_L + alveoliIncrement_L;

      //Track total compliance change
      SEFluidCircuitPath* compliancePath = cpt.CompliancePath;
      totalHealthyCompliance_L_Per_cmH2O += compliancePath->GetComplianceBaseline(VolumePerPressureUnit::L_Per_cmH2O);
      totalCompliance_L_Per_cmH2O += compliancePath->GetNextCompliance(VolumePerPressureUnit::L_Per_cmH2O);

      iter++;
    }

    m_RightAlveoliDecrease_L = rightAlveoliDecrease_L;
    m_LeftAlveoliDecrease_L = leftAlveoliDecrease_L;

    //---------------------------------------------------------------------------------------------------------------------------------------------
    //Update patient parameter volumes due to compliance change
    //Lung compliance change method must happen before this method
    double functionalResidualCapacity_L = m_data.GetInitialPatient().GetFunctionalResidualCapacity(VolumeUnit::L);
    double residualVolume_L = m_data.GetInitialPatient().GetResidualVolume(VolumeUnit::L);
    double totalLungCapacity_L = m_data.GetInitialPatient().GetTotalLungCapacity(VolumeUnit::L);

    double chestWallCompliance_L_Per_cmH2O = m_LeftPleuralToRespiratoryMuscle->GetComplianceBaseline(VolumePerPressureUnit::L_Per_cmH2O) +
      m_RightPleuralToRespiratoryMuscle->GetComplianceBaseline(VolumePerPressureUnit::L_Per_cmH2O);

    double totalComplianceChangeFraction = ((totalHealthyCompliance_L_Per_cmH2O + chestWallCompliance_L_Per_cmH2O) * totalCompliance_L_Per_cmH2O) / 
      ((totalHealthyCompliance_L_Per_cmH2O + totalCompliance_L_Per_cmH2O) * chestWallCompliance_L_Per_cmH2O);

    //TODO: I have no idea why only moving the volume 1/3 as far works
    totalComplianceChangeFraction += (1.0 - totalComplianceChangeFraction) * 2.0 / 3.0;

    functionalResidualCapacity_L = functionalResidualCapacity_L * totalComplianceChangeFraction + functionalResidualCapacityChange_L;
    residualVolume_L = residualVolume_L * totalComplianceChangeFraction + functionalResidualCapacityChange_L;
    totalLungCapacity_L = totalLungCapacity_L * totalComplianceChangeFraction + functionalResidualCapacityChange_L;

    double tidalVolumeBaseline_L = m_data.GetCurrentPatient().GetTidalVolumeBaseline(VolumeUnit::L);

    double expiratoryReserveVolume_L = functionalResidualCapacity_L - residualVolume_L;
    double inspiratoryReserveVolume_L = totalLungCapacity_L - functionalResidualCapacity_L - tidalVolumeBaseline_L;
    double inspiratoryCapacity_L = totalLungCapacity_L - functionalResidualCapacity_L;
    double vitalCapacity_L = totalLungCapacity_L - residualVolume_L;

    m_data.GetCurrentPatient().GetFunctionalResidualCapacity().SetValue(functionalResidualCapacity_L, VolumeUnit::L);
    m_data.GetCurrentPatient().GetResidualVolume().SetValue(residualVolume_L, VolumeUnit::L);
    m_data.GetCurrentPatient().GetTotalLungCapacity().SetValue(totalLungCapacity_L, VolumeUnit::L);
    m_data.GetCurrentPatient().GetExpiratoryReserveVolume().SetValue(expiratoryReserveVolume_L, VolumeUnit::L);
    m_data.GetCurrentPatient().GetInspiratoryReserveVolume().SetValue(inspiratoryReserveVolume_L, VolumeUnit::L);
    m_data.GetCurrentPatient().GetInspiratoryCapacity().SetValue(inspiratoryCapacity_L, VolumeUnit::L);
    m_data.GetCurrentPatient().GetVitalCapacity().SetValue(vitalCapacity_L, VolumeUnit::L);


    //---------------------------------------------------------------------------------------------------------------------------------------------
    //Mechanical Dead Space
    //This is from the environment settings
    //Equipment models include their own dead space values and should be used instead where possible

    double airwayBaselineVolume_L = m_AirwayNode->GetVolumeBaseline(VolumeUnit::L);
    double mechanicalDeadSpace_L = 0.0;
    if (m_data.GetEnvironment().GetEnvironmentalConditions().HasMechanicalDeadSpace())
    {
      mechanicalDeadSpace_L = m_data.GetEnvironment().GetEnvironmentalConditions().GetMechanicalDeadSpace(VolumeUnit::L);
    }
    m_AirwayNode->GetNextVolume().SetValue(airwayBaselineVolume_L + mechanicalDeadSpace_L, VolumeUnit::L);


    //------------------------------------------------------------------------------------------------------
    //Guard against volume going negative... or even below the residual volume at all
    //This is probably only needed for collapsing lungs or with the expanded respiratory system
    iter = 0;
    for (auto& itr : m_LungComponents)
    {
      LungComponent& cpt = itr.second;

      SEFluidCircuitNode* alveoliNode = cpt.AlveoliNode;

      double alveoliVolumeBaseline_L = alveoliNode->GetVolumeBaseline(VolumeUnit::L);
      double alveoliVolumeRatio = alveoliVolumeBaseline_L / totalBaselineAlveoliVolume_L;
      double alveoliVolume_L = alveoliNode->GetNextVolume(VolumeUnit::L);
      double cptResidualVolume_L = m_data.GetCurrentPatient().GetResidualVolume(VolumeUnit::L) * alveoliVolumeRatio;

      if (alveoliVolume_L < cptResidualVolume_L)
      {
        double volumeIncrement_L = alveoliVolume_L - cptResidualVolume_L;

        alveoliNode->GetNextVolume().SetReadOnly(false);
        alveoliNode->GetVolume().SetReadOnly(false);
        alveoliNode->GetNextVolume().SetValue(cptResidualVolume_L, VolumeUnit::L);
        alveoliNode->GetVolume().SetValue(cptResidualVolume_L, VolumeUnit::L);
        alveoliNode->GetNextVolume().SetReadOnly(true);
        alveoliNode->GetVolume().SetReadOnly(true);

        SEFluidCircuitNode* pleuralNode;
        pleuralNode = cpt.Side == eSide::Right ? m_RightPleuralNode : m_LeftPleuralNode;

        pleuralNode->GetNextVolume().SetReadOnly(false);
        pleuralNode->GetVolume().SetReadOnly(false);
        pleuralNode->GetNextVolume().Increment(volumeIncrement_L, VolumeUnit::L);
        pleuralNode->GetVolume().Increment(volumeIncrement_L, VolumeUnit::L);
        pleuralNode->GetNextVolume().SetReadOnly(true);
        pleuralNode->GetVolume().SetReadOnly(true);
      }
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Update respiratory resistances due to actions and conditions.
  ///
  /// \details
  /// Update the trachea, bronchial, and alveoli resistances as appropriate. Effects are combined by
  /// taking the action/conditions that has the greatest change from baseline.
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::UpdateResistances()
  {
    double pharynxResistance_cmH2O_s_Per_L = 0.0;
    double tracheaResistance_cmH2O_s_Per_L = m_PharynxToCarina->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    double rightBronchiResistance_cmH2O_s_Per_L = m_CarinaToRightAnatomicDeadSpace->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    double leftBronchiResistance_cmH2O_s_Per_L = m_CarinaToLeftAnatomicDeadSpace->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    double rightAlveoliResistance_cmH2O_s_Per_L = m_RightAnatomicDeadSpaceToRightAlveolarDeadSpace->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    double leftAlveoliResistance_cmH2O_s_Per_L = m_LeftAnatomicDeadSpaceToLeftAlveolarDeadSpace->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    double esophagusResistance_cmH2O_s_Per_L = m_AirwayToStomach->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);

    bool inhaling = false;
    if (m_PharynxToCarina->GetNextFlow(VolumePerTimeUnit::L_Per_s) > 0.0)
    {
      inhaling = true;
    }

    //Specified externally
    //Resistances in series sum
    if (HasActiveMechanics())
    {
      if (m_Mechanics->GetDefaultType() == eDefaultType::Zero)
      {
        tracheaResistance_cmH2O_s_Per_L = m_RespClosedResistance_cmH2O_s_Per_L;
        rightBronchiResistance_cmH2O_s_Per_L = m_RespClosedResistance_cmH2O_s_Per_L;
        leftBronchiResistance_cmH2O_s_Per_L = m_RespClosedResistance_cmH2O_s_Per_L;
        rightAlveoliResistance_cmH2O_s_Per_L = m_RespClosedResistance_cmH2O_s_Per_L;
        leftAlveoliResistance_cmH2O_s_Per_L = m_RespClosedResistance_cmH2O_s_Per_L;
      }

      //Should sum to 1.0
      double bronchiResistanceFraction = 0.6;
      double alveoliDuctResistanceFraction = 0.4;
      if (inhaling)
      {
        if(m_Mechanics->HasUpperInspiratoryResistance())
          tracheaResistance_cmH2O_s_Per_L = m_Mechanics->GetUpperInspiratoryResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
        if (m_Mechanics->HasLeftInspiratoryResistance())
        {
          double leftResistance_cmH2O_s_Per_L = m_Mechanics->GetLeftInspiratoryResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
          leftBronchiResistance_cmH2O_s_Per_L = leftResistance_cmH2O_s_Per_L * bronchiResistanceFraction;
          leftAlveoliResistance_cmH2O_s_Per_L = leftResistance_cmH2O_s_Per_L * alveoliDuctResistanceFraction;
        }
        if (m_Mechanics->HasRightInspiratoryResistance())
        {
          double rightResistance_cmH2O_s_Per_L = m_Mechanics->GetRightInspiratoryResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
          rightBronchiResistance_cmH2O_s_Per_L = rightResistance_cmH2O_s_Per_L * bronchiResistanceFraction;
          rightAlveoliResistance_cmH2O_s_Per_L = rightResistance_cmH2O_s_Per_L * alveoliDuctResistanceFraction;
        }
      }
      else //exhaling
      {
        if (m_Mechanics->HasUpperExpiratoryResistance())
          tracheaResistance_cmH2O_s_Per_L = m_Mechanics->GetUpperExpiratoryResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
        if (m_Mechanics->HasLeftExpiratoryResistance())
        {
          double leftResistance_cmH2O_s_Per_L = m_Mechanics->GetLeftExpiratoryResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
          leftBronchiResistance_cmH2O_s_Per_L = leftResistance_cmH2O_s_Per_L * bronchiResistanceFraction;
          leftAlveoliResistance_cmH2O_s_Per_L = leftResistance_cmH2O_s_Per_L * alveoliDuctResistanceFraction;
        }
        if (m_Mechanics->HasRightExpiratoryResistance())
        {
          double rightResistance_cmH2O_s_Per_L = m_Mechanics->GetRightExpiratoryResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
          rightBronchiResistance_cmH2O_s_Per_L = rightResistance_cmH2O_s_Per_L * bronchiResistanceFraction;
          rightAlveoliResistance_cmH2O_s_Per_L = rightResistance_cmH2O_s_Per_L * alveoliDuctResistanceFraction;
        }
      }
    }
    else //Don't do this if we've specified externally - other conditions/actions will still work
    {
      //Intubation
      double intubationTracheatracheaResistanceMultiplier_cmH2O_s_Per_L = 8.8;
      if (m_PatientActions->HasIntubation())
      {
        switch (m_PatientActions->GetIntubation().GetType())
        {
        case eIntubation_Type::Tracheal:
        {
          if (m_PatientActions->GetIntubation().HasAirwayResistance())
          {
            tracheaResistance_cmH2O_s_Per_L = m_PatientActions->GetIntubation().GetAirwayResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
          }
          else if (m_PatientActions->GetIntubation().HasSeverity())
          {
            double severity = m_PatientActions->GetIntubation().GetSeverity().GetValue();
            //Severity 0 = Default
            //Severity 1 = Fully closed
            if (severity != 0.0) //Prevent small numerical error
            {
              tracheaResistance_cmH2O_s_Per_L = GeneralMath::ExponentialGrowthFunction(20.0, tracheaResistance_cmH2O_s_Per_L, m_RespOpenResistance_cmH2O_s_Per_L, severity);
            }
          }
          else
          {
            //Tuned based on mechanical ventilator validation data
            tracheaResistance_cmH2O_s_Per_L *= intubationTracheatracheaResistanceMultiplier_cmH2O_s_Per_L;
          }

          break;
        }
        case eIntubation_Type::Esophageal:
        {
          // Allow air flow between Airway and Stomach
          esophagusResistance_cmH2O_s_Per_L = 1.2;
          pharynxResistance_cmH2O_s_Per_L = m_DefaultOpenResistance_cmH2O_s_Per_L;

          if (m_PatientActions->GetIntubation().HasAirwayResistance())
          {
            tracheaResistance_cmH2O_s_Per_L = m_PatientActions->GetIntubation().GetAirwayResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
          }

          double leakResistance_cmH2O_s_Per_L = m_RespClosedResistance_cmH2O_s_Per_L;
          if (m_PatientActions->GetIntubation().HasSeverity())
          {
            double severity = m_PatientActions->GetIntubation().GetSeverity().GetValue();
            //Severity 0 = Default
            //Severity 1 = Fully closed
            if (severity != 0.0) //Prevent small numerical error
            {
              if (severity == 1.0)
              {
                leakResistance_cmH2O_s_Per_L = m_DefaultOpenResistance_cmH2O_s_Per_L;
              }
              else
              {
                leakResistance_cmH2O_s_Per_L = GeneralMath::ExponentialGrowthFunction(20.0, leakResistance_cmH2O_s_Per_L, m_RespOpenResistance_cmH2O_s_Per_L, severity);
              }
            }
          }

          m_PharynxToEnvironment->GetNextResistance().SetValue(leakResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);

          break;
        }
        case eIntubation_Type::RightMainstem:
        {
          //Update trachea for intubation
          tracheaResistance_cmH2O_s_Per_L *= intubationTracheatracheaResistanceMultiplier_cmH2O_s_Per_L;

          if (m_PatientActions->GetIntubation().HasAirwayResistance())
          {
            tracheaResistance_cmH2O_s_Per_L = m_PatientActions->GetIntubation().GetAirwayResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
          }

          if (m_PatientActions->GetIntubation().HasSeverity())
          {
            double severity = m_PatientActions->GetIntubation().GetSeverity().GetValue();
            //Severity 0 = Default
            //Severity 1 = Fully closed
            if (severity != 0.0) //Prevent small numerical error
            {
              if (severity == 1.0)
              {
                leftBronchiResistance_cmH2O_s_Per_L = m_DefaultOpenResistance_cmH2O_s_Per_L;
              }
              else
              {
                leftBronchiResistance_cmH2O_s_Per_L = GeneralMath::ExponentialGrowthFunction(20.0, leftBronchiResistance_cmH2O_s_Per_L, m_RespOpenResistance_cmH2O_s_Per_L, severity);
              }
            }
          }
          else
          {
            leftBronchiResistance_cmH2O_s_Per_L = m_DefaultOpenResistance_cmH2O_s_Per_L;
          }

          break;
        }
        case eIntubation_Type::LeftMainstem:
        {
          //Update trachea for intubation
          tracheaResistance_cmH2O_s_Per_L *= intubationTracheatracheaResistanceMultiplier_cmH2O_s_Per_L;

          if (m_PatientActions->GetIntubation().HasAirwayResistance())
          {
            tracheaResistance_cmH2O_s_Per_L = m_PatientActions->GetIntubation().GetAirwayResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
          }

          if (m_PatientActions->GetIntubation().HasSeverity())
          {
            double severity = m_PatientActions->GetIntubation().GetSeverity().GetValue();
            //Severity 0 = Default
            //Severity 1 = Fully closed
            if (severity != 0.0) //Prevent small numerical error
            {
              if (severity == 1.0)
              {
                rightBronchiResistance_cmH2O_s_Per_L = m_DefaultOpenResistance_cmH2O_s_Per_L;
              }
              else
              {
                rightBronchiResistance_cmH2O_s_Per_L = GeneralMath::ExponentialGrowthFunction(20.0, rightBronchiResistance_cmH2O_s_Per_L, m_RespOpenResistance_cmH2O_s_Per_L, severity);
              }
            }
          }
          else
          {
            rightBronchiResistance_cmH2O_s_Per_L = m_DefaultOpenResistance_cmH2O_s_Per_L;
          }

          break;
        }
        case eIntubation_Type::Oropharyngeal:
        {
          if (m_PatientActions->GetIntubation().HasAirwayResistance())
          {
            tracheaResistance_cmH2O_s_Per_L = m_PatientActions->GetIntubation().GetAirwayResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
          }
          else if (m_PatientActions->GetIntubation().HasSeverity())
          {
            double severity = m_PatientActions->GetIntubation().GetSeverity().GetValue();
            //Severity 0 = Default
            //Severity 1 = Fully closed
            if (severity != 0.0) //Prevent small numerical error
            {
              if (severity == 1.0)
              {
                tracheaResistance_cmH2O_s_Per_L = m_DefaultOpenResistance_cmH2O_s_Per_L;
              }
              else
              {
                tracheaResistance_cmH2O_s_Per_L = GeneralMath::ExponentialGrowthFunction(20.0, tracheaResistance_cmH2O_s_Per_L, m_RespOpenResistance_cmH2O_s_Per_L, severity);
              }
            }
          }
          else
          {
            //Airway adjunct
            tracheaResistance_cmH2O_s_Per_L *= 15.0;
          }

          break;
        }
        case eIntubation_Type::Nasopharyngeal:
        {
          if (m_PatientActions->GetIntubation().HasAirwayResistance())
          {
            tracheaResistance_cmH2O_s_Per_L = m_PatientActions->GetIntubation().GetAirwayResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
          }
          else if (m_PatientActions->GetIntubation().HasSeverity())
          {
            double severity = m_PatientActions->GetIntubation().GetSeverity().GetValue();
            //Severity 0 = Default
            //Severity 1 = Fully closed
            if (severity != 0.0) //Prevent small numerical error
            {
              if (severity == 1.0)
              {
                tracheaResistance_cmH2O_s_Per_L = m_DefaultOpenResistance_cmH2O_s_Per_L;
              }
              else
              {
                tracheaResistance_cmH2O_s_Per_L = GeneralMath::ExponentialGrowthFunction(20.0, tracheaResistance_cmH2O_s_Per_L, m_RespOpenResistance_cmH2O_s_Per_L, severity);
              }
            }
          }
          else
          {
            //Airway adjunct
            tracheaResistance_cmH2O_s_Per_L *= 15.0;
          }

          break;
        }
        default:
          break;
        }
      }
      else
      {
        //Positive Pressure Ventilation assuming a mask if not intubated
        if (m_PositivePressureVentilation || m_data.GetAirwayMode() == eAirwayMode::BagValveMask)
        {
          tracheaResistance_cmH2O_s_Per_L *= 20.0;
        }
      }
    }

    //------------------------------------------------------------------------------------------------------
    //Airway obstruction
    if (m_PatientActions->HasAirwayObstruction())
    {
      double severity = m_PatientActions->GetAirwayObstruction().GetSeverity().GetValue();
      if (severity > ZERO_APPROX) //Prevent small numerical error
      {
        //Piecewise to ensure fully blocked at a severity of 1.0;
        double startSeverity = 0.9;
        if (severity == 1.0)
        {
          tracheaResistance_cmH2O_s_Per_L = m_DefaultOpenResistance_cmH2O_s_Per_L;
        }
        else if (severity > startSeverity)
        {
          double minResistance_cmH2O_s_Per_L = GeneralMath::ExponentialGrowthFunction(5.0, tracheaResistance_cmH2O_s_Per_L, m_RespOpenResistance_cmH2O_s_Per_L, startSeverity);
          double maxResistance_cmH2O_s_Per_L = m_RespOpenResistance_cmH2O_s_Per_L;

          tracheaResistance_cmH2O_s_Per_L = GeneralMath::LinearInterpolator(startSeverity, 1.0, minResistance_cmH2O_s_Per_L, maxResistance_cmH2O_s_Per_L, severity);
        }
        else
        {
          tracheaResistance_cmH2O_s_Per_L = GeneralMath::ExponentialGrowthFunction(5.0, tracheaResistance_cmH2O_s_Per_L, m_RespOpenResistance_cmH2O_s_Per_L, severity);
        }
      }

      // Apply suctioning sawtooth pattern if secretions are present and during expiration
      if (!inhaling && m_PatientActions->GetAirwayObstruction().GetResistanceType()==eAirwayObstruction_ResistanceType::Oscillating)
      {
        double currentFlow_L_Per_s = std::abs(m_PharynxToCarina->GetNextFlow(VolumePerTimeUnit::L_Per_s));
        tracheaResistance_cmH2O_s_Per_L = CalculateSuctioningPattern(tracheaResistance_cmH2O_s_Per_L, currentFlow_L_Per_s);
      }
    }

    //------------------------------------------------------------------------------------------------------
    //Broncho constriction
    if (m_PatientActions->HasBronchoconstriction())
    {
      double severity = m_PatientActions->GetBronchoconstriction().GetSeverity().GetValue();
      if (severity > ZERO_APPROX) //Prevent small numerical error
      {
        leftBronchiResistance_cmH2O_s_Per_L = GeneralMath::ExponentialGrowthFunction(10.0, leftBronchiResistance_cmH2O_s_Per_L, m_RespOpenResistance_cmH2O_s_Per_L, severity);
        rightBronchiResistance_cmH2O_s_Per_L = GeneralMath::ExponentialGrowthFunction(10.0, rightBronchiResistance_cmH2O_s_Per_L, m_RespOpenResistance_cmH2O_s_Per_L, severity);
      }
    }

    if (m_data.HasDrugs())
    {
      //------------------------------------------------------------------------------------------------------
      //Bronchodilation
      //Note: this could constrict with a negative number from PD
      //We're going to make the bronchodilation effects be based off of Albuterol.
      //A value of 1.0 will be the effective Albuterol EMax (with the current, non-overdose implementation), and -1.0 will be a bronchconstriction the same percentage in the other direction.
      //Experimentally, 1mg of Albuterol given via a spacer device on an endotracheal tube caused a pulmonary resistance change of ~20% @cite mancebo1991effects.
      //The bronchi are ~30% of the total pulmonary resistance, so we'll make a dilation effect of 1.0 be at the respiratory open resistance.
      //Dilation effect values have max effect at 1 and below -1, so anything outside of that will maintain that effect.
      double bronchoDilationEffect = m_data.GetDrugs().GetBronchodilationLevel().GetValue();
      if (bronchoDilationEffect != 0.0)
      {
        //It'll pretty much always get in here because there's epinephrine present
        //Get the path resistances 
        if (bronchoDilationEffect >= 0.0)// positive, therefore dilation
        {
          bronchoDilationEffect = MIN(bronchoDilationEffect, 1.0);
          leftBronchiResistance_cmH2O_s_Per_L = GeneralMath::ExponentialDecayFunction(10.0, m_RespClosedResistance_cmH2O_s_Per_L, leftBronchiResistance_cmH2O_s_Per_L, bronchoDilationEffect);
          rightBronchiResistance_cmH2O_s_Per_L = GeneralMath::ExponentialDecayFunction(10.0, m_RespClosedResistance_cmH2O_s_Per_L, rightBronchiResistance_cmH2O_s_Per_L, bronchoDilationEffect);
        }
        else //negative, therefore constriction
        {
          bronchoDilationEffect = MIN(-bronchoDilationEffect, 1.0);
          leftBronchiResistance_cmH2O_s_Per_L = GeneralMath::ExponentialGrowthFunction(10.0, leftBronchiResistance_cmH2O_s_Per_L, m_RespOpenResistance_cmH2O_s_Per_L, bronchoDilationEffect);
          rightBronchiResistance_cmH2O_s_Per_L = GeneralMath::ExponentialGrowthFunction(10.0, rightBronchiResistance_cmH2O_s_Per_L, m_RespOpenResistance_cmH2O_s_Per_L, bronchoDilationEffect);
        }
      }
    }

    //------------------------------------------------------------------------------------------------------
    //Obstructive
    double obstructiveResistanceScalingFactor = 1.0;

    //------------------------------------------------------------------------------------------------------
    //Asthma
    if (m_PatientActions->HasAsthmaAttack())
    {
      double severity = m_PatientActions->GetAsthmaAttack().GetSeverity().GetValue();
      if (severity != 0.0) //Prevent small numerical error
      {
        obstructiveResistanceScalingFactor = GeneralMath::ExponentialGrowthFunction(8.0, rightBronchiResistance_cmH2O_s_Per_L, m_RespOpenResistance_cmH2O_s_Per_L, severity);
      }
    }

    //------------------------------------------------------------------------------------------------------
    //COPD
    //Exacerbation will overwrite the condition, even if it means improvement
    if (m_data.GetConditions().HasChronicObstructivePulmonaryDisease() || m_PatientActions->HasChronicObstructivePulmonaryDiseaseExacerbation())
    {
      double bronchitisSeverity = 0.0;
      if (m_PatientActions->HasChronicObstructivePulmonaryDiseaseExacerbation())
      {
        bronchitisSeverity = m_PatientActions->GetChronicObstructivePulmonaryDiseaseExacerbation().GetBronchitisSeverity().GetValue();
      }
      else
      {
        bronchitisSeverity = m_data.GetConditions().GetChronicObstructivePulmonaryDisease().GetBronchitisSeverity().GetValue();
      }

      std::vector<std::pair<double, double>> inhaleInterpolatorPoints =
      {
        {0.0, 1.0}, //None
        {0.3, 10.0}, //Mild
        {0.6, 60.0}, //Moderate
        {0.9, 100.0}, //Severe
        {1.0, 150.0}  //Max
      };
      std::vector<std::pair<double, double>> exhaleInterpolatorPoints =
      {
        {0.0, 1.0}, //None
        {0.3, 35.0}, //Mild
        {0.6, 120.0}, //Moderate
        {0.9, 180.0}, //Severe
        {1.0, 250.0}  //Max
      };

      double resistanceScalingFactor = 1.0;
      if (inhaling)
        resistanceScalingFactor = GeneralMath::PiecewiseLinearInterpolator(inhaleInterpolatorPoints, bronchitisSeverity);
      else //exhaling
        resistanceScalingFactor = GeneralMath::PiecewiseLinearInterpolator(exhaleInterpolatorPoints, bronchitisSeverity);
      obstructiveResistanceScalingFactor = MAX(obstructiveResistanceScalingFactor, resistanceScalingFactor);
    }

    //------------------------------------------------------------------------------------------------------
    leftBronchiResistance_cmH2O_s_Per_L *= obstructiveResistanceScalingFactor;
    rightBronchiResistance_cmH2O_s_Per_L *= obstructiveResistanceScalingFactor;

    //------------------------------------------------------------------------------------------------------
    //Modifiers
    if (inhaling)
    {
      if (m_MechanicsModifiers->HasUpperInspiratoryResistanceMultiplier())
      {
        tracheaResistance_cmH2O_s_Per_L *= m_MechanicsModifiers->GetUpperInspiratoryResistanceMultiplier().GetValue();
      }
      if (m_MechanicsModifiers->HasLeftInspiratoryResistanceMultiplier())
      {
        leftBronchiResistance_cmH2O_s_Per_L *= m_MechanicsModifiers->GetLeftInspiratoryResistanceMultiplier().GetValue();
      }
      if (m_MechanicsModifiers->HasRightInspiratoryResistanceMultiplier())
      {
        rightBronchiResistance_cmH2O_s_Per_L *= m_MechanicsModifiers->GetRightInspiratoryResistanceMultiplier().GetValue();
      }
    }
    else //exhaling
    {
      if (m_MechanicsModifiers->HasUpperExpiratoryResistanceMultiplier())
      {
        tracheaResistance_cmH2O_s_Per_L *= m_MechanicsModifiers->GetUpperExpiratoryResistanceMultiplier().GetValue();
      }
      if (m_MechanicsModifiers->HasLeftExpiratoryResistanceMultiplier())
      {
        leftBronchiResistance_cmH2O_s_Per_L *= m_MechanicsModifiers->GetLeftExpiratoryResistanceMultiplier().GetValue();
      }
      if (m_MechanicsModifiers->HasRightExpiratoryResistanceMultiplier())
      {
        rightBronchiResistance_cmH2O_s_Per_L *= m_MechanicsModifiers->GetRightExpiratoryResistanceMultiplier().GetValue();
      }
    }

    //------------------------------------------------------------------------------------------------------
    // Make sure things don't go crazy
    BLIM(tracheaResistance_cmH2O_s_Per_L, m_DefaultClosedResistance_cmH2O_s_Per_L, m_DefaultOpenResistance_cmH2O_s_Per_L);
    BLIM(leftBronchiResistance_cmH2O_s_Per_L, m_DefaultClosedResistance_cmH2O_s_Per_L, m_DefaultOpenResistance_cmH2O_s_Per_L);
    BLIM(rightBronchiResistance_cmH2O_s_Per_L, m_DefaultClosedResistance_cmH2O_s_Per_L, m_DefaultOpenResistance_cmH2O_s_Per_L);
    BLIM(rightAlveoliResistance_cmH2O_s_Per_L, m_DefaultClosedResistance_cmH2O_s_Per_L, m_DefaultOpenResistance_cmH2O_s_Per_L);
    BLIM(leftAlveoliResistance_cmH2O_s_Per_L, m_DefaultClosedResistance_cmH2O_s_Per_L, m_DefaultOpenResistance_cmH2O_s_Per_L);

    //Set new values
    m_PharynxToCarina->GetNextResistance().SetValue(tracheaResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    m_CarinaToRightAnatomicDeadSpace->GetNextResistance().SetValue(rightBronchiResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    m_CarinaToLeftAnatomicDeadSpace->GetNextResistance().SetValue(leftBronchiResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    m_RightAnatomicDeadSpaceToRightAlveolarDeadSpace->GetNextResistance().SetValue(rightAlveoliResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    m_LeftAnatomicDeadSpaceToLeftAlveolarDeadSpace->GetNextResistance().SetValue(leftAlveoliResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    m_AirwayToStomach->GetNextResistance().SetValue(esophagusResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);

    //------------------------------------------------------------------------------------------------------
    //Restrictive
    //Some need to be done at the compartment level and need to handle expanded compartment definitions
    for (auto& itr : m_LungComponents)
    {
      eLungCompartment cmpt = itr.first;
      LungComponent& cpt = itr.second;

      double restrictiveSeverity = 0.0;

      //------------------------------------------------------------------------------------------------------
      //Pneumonia
      //Exacerbation will overwrite the condition, even if it means improvement
      if (m_data.GetConditions().HasPneumonia() || m_PatientActions->HasPneumoniaExacerbation())
      {
        double severity = 0.0;
        if (m_PatientActions->HasPneumoniaExacerbation())
        {
          severity = m_PatientActions->GetPneumoniaExacerbation().GetSeverity(cmpt).GetValue();
        }
        else
        {
          severity = m_data.GetConditions().GetPneumonia().GetSeverity(cmpt).GetValue();
        }

        restrictiveSeverity = MAX(restrictiveSeverity, severity);
      }

      //------------------------------------------------------------------------------------------------------
      //Pulmonary Fibrosis
      if (m_data.GetConditions().HasPulmonaryFibrosis())
      {
        double severity = m_data.GetConditions().GetPulmonaryFibrosis().GetSeverity().GetValue();

        restrictiveSeverity = MAX(restrictiveSeverity, severity);
      }

      //------------------------------------------------------------------------------------------------------
      //ARDS
      //Exacerbation will overwrite the condition, even if it means improvement
      if (m_data.GetConditions().HasAcuteRespiratoryDistressSyndrome() || m_PatientActions->HasAcuteRespiratoryDistressSyndromeExacerbation())
      {
        double severity = 0.0;
        if (m_PatientActions->HasAcuteRespiratoryDistressSyndromeExacerbation())
        {
          severity = m_PatientActions->GetAcuteRespiratoryDistressSyndromeExacerbation().GetSeverity(cmpt).GetValue();
        }
        else
        {
          severity = m_data.GetConditions().GetAcuteRespiratoryDistressSyndrome().GetSeverity(cmpt).GetValue();
        }

        restrictiveSeverity = MAX(restrictiveSeverity, severity);
      }

      //------------------------------------------------------------------------------------------------------
      //PBLI
      //Same as ARDS
      if (m_PatientActions->HasPrimaryBlastLungInjury())
      {
        double severity = m_PatientActions->GetPrimaryBlastLungInjury().GetSeverity(cmpt).GetValue();
        restrictiveSeverity = MAX(restrictiveSeverity, severity);
      }


      std::vector<std::pair<double, double>> interpolatorPoints =
      {
        {0.0, 1.0}, //None
        {0.3, 10.0}, //Mild
        {0.6, 15.0}, //Moderate
        {0.9, 20.0}, //Severe
        {1.0, 25.0}  //Max
      };

      double resistanceScalingFactor = GeneralMath::PiecewiseLinearInterpolator(interpolatorPoints, restrictiveSeverity);

      SEFluidCircuitPath* alveoliResistancePath = cpt.ResistancePath;
      double alveoliResistance_cmH2O_s_Per_L = alveoliResistancePath->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
      alveoliResistancePath->GetNextResistance().SetValue(alveoliResistance_cmH2O_s_Per_L * resistanceScalingFactor, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    }

    bool stateChange = false;
    if (pharynxResistance_cmH2O_s_Per_L > 0.0)
    {
      if (!m_AirwayToPharynx->HasNextResistance())
      {
        // Add a new resitor element, since this path doesn't have an element by default
        m_AirwayToPharynx->GetResistanceBaseline().SetValue(pharynxResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
        stateChange = true;
      }

      m_AirwayToPharynx->GetNextResistance().SetValue(pharynxResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    }
    else if (m_AirwayToPharynx->HasNextResistance())
    {
      // Remove the element
      m_AirwayToPharynx->RemoveResistance();
      stateChange = true;
    }

    if (stateChange)
    {
      m_data.GetCircuits().GetActiveRespiratoryCircuit().StateChange();
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Update lung compliance due to actions and conditions.
  ///
  /// \details
  /// The alveoli/lung compliance is a constant value (linear volume vs. presure function) that is
  /// modified and tuned to provide the desired total pulmonary compliance. The maximal obstructive and 
  /// maximal restrictive effects are combined through individual multipliers.
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::UpdateAlveolarCompliances()
  {
    for (auto& itr : m_LungComponents)
    {
      eLungCompartment cmpt = itr.first;
      LungComponent& cpt = itr.second;

      //------------------------------------------------------------------------------------------------------
      //Positive Pressure Ventilation
      double positivePressureComplianceScalingFactor = 1.0;

      bool isEsophagealIntubation = m_PatientActions->HasIntubation() && m_PatientActions->GetIntubation().GetType() == eIntubation_Type::Esophageal;

      if (m_PositivePressureVentilation && !isEsophagealIntubation)
      {
        if (cpt.Side == eSide::Right)
        {
          if (!HasActiveMechanics() ||
            (HasActiveMechanics() && !m_Mechanics->HasRightComplianceCurve()))
          {
            positivePressureComplianceScalingFactor = 0.45;
          }
        }
        else //Left
        {
          if (!HasActiveMechanics() ||
            (HasActiveMechanics() && !m_Mechanics->HasLeftComplianceCurve()))
          {
            positivePressureComplianceScalingFactor = 0.45;
          }
        }
      }

      //------------------------------------------------------------------------------------------------------
      //Obstructive = Increase
      double obstructiveComplianceScalingFactor = 1.0;

      //------------------------------------------------------------------------------------------------------
      //Asthma
      ///\TODO

      //------------------------------------------------------------------------------------------------------
      //COPD - Moves enough based on lung volume increase alone
      //Exacerbation will overwrite the condition, even if it means improvement
      if (m_data.GetConditions().HasChronicObstructivePulmonaryDisease() || m_PatientActions->HasChronicObstructivePulmonaryDiseaseExacerbation())
      {
        double emphysemaSeverity = 0.0;
        if (m_PatientActions->HasChronicObstructivePulmonaryDiseaseExacerbation())
        {
          emphysemaSeverity = m_PatientActions->GetChronicObstructivePulmonaryDiseaseExacerbation().GetEmphysemaSeverity(cmpt).GetValue();
        }
        else
        {
          emphysemaSeverity = m_data.GetConditions().GetChronicObstructivePulmonaryDisease().GetEmphysemaSeverity(cmpt).GetValue();
        }

        std::vector<std::pair<double, double>> interpolatorPoints =
        {
          {0.0, 1.0}, //None
          {0.3, 1.06}, //Mild
          {0.6, 1.08}, //Moderate
          {0.9, 1.09}, //Severe
          {1.0, 1.20}  //Max
        };
        obstructiveComplianceScalingFactor = GeneralMath::PiecewiseLinearInterpolator(interpolatorPoints, emphysemaSeverity);
      }

      //------------------------------------------------------------------------------------------------------
      //Restrictive = Decrease
      double restrictiveSeverity = 0.0;

      std::vector<std::pair<double, double>> restrictiveInterpolatorPoints =
      {
        {0.0, 1.0}, //None
        {0.3, 0.65}, //Mild
        {0.6, 0.55}, //Moderate
        {0.9, 0.5}, //Severe
        {1.0, 0.4}  //Max
      };

      //------------------------------------------------------------------------------------------------------
      //Pneumonia
      //Exacerbation will overwrite the condition, even if it means improvement
      if (m_data.GetConditions().HasPneumonia() || m_PatientActions->HasPneumoniaExacerbation())
      {
        double severity;
        if (m_PatientActions->HasPneumoniaExacerbation())
        {
          severity = m_PatientActions->GetPneumoniaExacerbation().GetSeverity(cmpt).GetValue();
        }
        else
        {
          severity = m_data.GetConditions().GetPneumonia().GetSeverity(cmpt).GetValue();
        }

        restrictiveSeverity = MAX(restrictiveSeverity, severity);
      }

      //------------------------------------------------------------------------------------------------------
      //Pulmonary Fibrosis
      if (m_data.GetConditions().HasPulmonaryFibrosis())
      {
        double severity = m_data.GetConditions().GetPulmonaryFibrosis().GetSeverity().GetValue();

        restrictiveSeverity = MAX(restrictiveSeverity, severity);
      }

      //------------------------------------------------------------------------------------------------------
      //ARDS
      //Exacerbation will overwrite the condition, even if it means improvement
      if (m_data.GetConditions().HasAcuteRespiratoryDistressSyndrome() || m_PatientActions->HasAcuteRespiratoryDistressSyndromeExacerbation())
      {
        double severity = 0.0;
        if (m_PatientActions->HasAcuteRespiratoryDistressSyndromeExacerbation())
        {
          severity = m_PatientActions->GetAcuteRespiratoryDistressSyndromeExacerbation().GetSeverity(cmpt).GetValue();
        }
        else
        {
          severity = m_data.GetConditions().GetAcuteRespiratoryDistressSyndrome().GetSeverity(cmpt).GetValue();
        }

        restrictiveSeverity = MAX(restrictiveSeverity, severity);
      }

      //------------------------------------------------------------------------------------------------------
      //PBLI
      //Same as ARDS
      if (m_PatientActions->HasPrimaryBlastLungInjury())
      {
        double severity = m_PatientActions->GetPrimaryBlastLungInjury().GetSeverity(cmpt).GetValue();
        restrictiveSeverity = MAX(restrictiveSeverity, severity);
      }

      double restrictiveComplianceScalingFactor = GeneralMath::PiecewiseLinearInterpolator(restrictiveInterpolatorPoints, restrictiveSeverity);

      //------------------------------------------------------------------------------------------------------
      //Set new values
      SEFluidCircuitPath* alveoliCompliancePath = cpt.CompliancePath;
      double alveoliCompliance_L_Per_cmH2O = alveoliCompliancePath->GetNextCompliance(VolumePerPressureUnit::L_Per_cmH2O);
      alveoliCompliance_L_Per_cmH2O *= positivePressureComplianceScalingFactor * obstructiveComplianceScalingFactor * restrictiveComplianceScalingFactor;

      //------------------------------------------------------------------------------------------------------
      //Modifiers
      if (cpt.Side == eSide::Left && m_MechanicsModifiers->HasLeftComplianceMultiplier())
      {
        alveoliCompliance_L_Per_cmH2O *= m_MechanicsModifiers->GetLeftComplianceMultiplier().GetValue();
      }
      if (cpt.Side == eSide::Right && m_MechanicsModifiers->HasRightComplianceMultiplier())
      {
        alveoliCompliance_L_Per_cmH2O *= m_MechanicsModifiers->GetRightComplianceMultiplier().GetValue();
      }

      //------------------------------------------------------------------------------------------------------
      alveoliCompliancePath->GetNextCompliance().SetValue(alveoliCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Modulates airway resistance with a flow-dependent sinusoidal pattern to simulate secretion buildup
  ///
  /// \param baseResistance_cmH2O_s_Per_L - The baseline upper airway resistance
  /// \param flow_L_Per_s - Current expiratory flow rate (positive value)
  /// \return Resistance with sinusoidal variation applied
  ///
  /// \details
  /// Applies a sinusoidal oscillation to the airway resistance during expiration when secretions
  /// are present. The amplitude and frequency of the sinusoid scale with the expiratory flow rate,
  /// producing visible ripples in the expiratory flow signal. This mimics the dynamic resistance
  /// caused by fluid buildup or mucus fluttering in the airway.
  /// 
  /// The waveform is continuous and smooth, with higher flow generating higher-frequency and
  /// higher-amplitude oscillations. The decaying envelope of the flow is not modeled here —
  /// it is governed by the fluid dynamics of the Pulse Physiology Engine.
  //--------------------------------------------------------------------------------------------------
  double RespiratoryModel::CalculateSuctioningPattern(double baseResistance_cmH2O_s_Per_L, double flow_L_Per_s)
  {
    if (flow_L_Per_s < 0.01)
    {
      return baseResistance_cmH2O_s_Per_L;
    }

    // Normalize flow magnitude (0–1 range based on expected expiratory peak ~1 L/s)
    double flowNormalized = std::min(flow_L_Per_s / 1.0, 1.0);

    // Amplitude and frequency scale with flow
    double amplitude = baseResistance_cmH2O_s_Per_L * (0.3 + 0.2 * flowNormalized); // 30–50% of base
    double frequency_Hz = 1.0 + 4.0 * flowNormalized; // 1–5 Hz

    // Time in seconds
    double t = m_ElapsedBreathingCycleTime_min * 60.0;
    double omega = 2.0 * M_PI * frequency_Hz;

    // Sinusoidal oscillation: -1 to 1
    double sinusoid = sin(omega * t);

    // Apply to resistance
    double modulatedResistance = baseResistance_cmH2O_s_Per_L + amplitude * sinusoid;

    // Clamp to stay positive
    return std::max(modulatedResistance, baseResistance_cmH2O_s_Per_L * 0.1);
  }


  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Update the inspiratory:expiratory ratio due to actions and conditions.
  ///
  /// \details
  /// The inspiratory and expiratory ratio scaling factors are determine thorugh the tuned combined 
  /// effects of respiratory conditions and actions, as well as drug PD effects. The resulting modifier
  /// is then applied to the breath cycle fraction to ultimately modify the relaive inspiratory and 
  /// expiratory time.
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::UpdateInspiratoryExpiratoryRatio()
  {
    //Make inspiratory time short if ventilated
    if (m_PositivePressureVentilation)
    {
      m_IERatioScaleFactor = 0.5;
      return;
    }

    m_IERatioScaleFactor = 1.0; //Reset

    double totalBaselineAlveoliVolume_L = 0.0;
    for (auto& itr : m_LungComponents)
    {
      LungComponent& cpt = itr.second;
      totalBaselineAlveoliVolume_L += cpt.AlveoliNode->GetVolumeBaseline(VolumeUnit::L);
    }

    //Adjust the inspiration/expiration ratio based on severity
    double combinedObstructiveSeverity = 0.0;
    double combinedRestrictiveSeverity = 0.0;

    for (auto& itr : m_LungComponents)
    {
      eLungCompartment cmpt = itr.first;
      LungComponent& cpt = itr.second;

      double alveoliVolumeBaseline_L = cpt.AlveoliNode->GetVolumeBaseline(VolumeUnit::L);
      double alveoliVolumeRatio = alveoliVolumeBaseline_L / totalBaselineAlveoliVolume_L;

      double obstructiveSeverity = 0.0;
      double restrictiveSeverity = 0.0;

      //------------------------------------------------------------------------------------------------------
      //Obsructive = Decrease (prolonged expiration)

      //------------------------------------------------------------------------------------------------------
      //Asthma
      if (m_PatientActions->HasAsthmaAttack())
      {
        obstructiveSeverity = m_PatientActions->GetAsthmaAttack().GetSeverity().GetValue();
      }

      //------------------------------------------------------------------------------------------------------
      //COPD
      //Exacerbation will overwrite the condition, even if it means improvement
      if (m_data.GetConditions().HasChronicObstructivePulmonaryDisease() || m_PatientActions->HasChronicObstructivePulmonaryDiseaseExacerbation())
      {
        double bronchitisSeverity = 0.0;
        double emphysemaSeverity = 0.0;
        if (m_PatientActions->HasChronicObstructivePulmonaryDiseaseExacerbation())
        {
          bronchitisSeverity = m_PatientActions->GetChronicObstructivePulmonaryDiseaseExacerbation().GetBronchitisSeverity().GetValue();
          emphysemaSeverity = m_PatientActions->GetChronicObstructivePulmonaryDiseaseExacerbation().GetEmphysemaSeverity(cmpt).GetValue();
        }
        else
        {
          bronchitisSeverity = m_data.GetConditions().GetChronicObstructivePulmonaryDisease().GetBronchitisSeverity().GetValue();
          emphysemaSeverity = m_data.GetConditions().GetChronicObstructivePulmonaryDisease().GetEmphysemaSeverity(cmpt).GetValue();
        }

        obstructiveSeverity = MAX(obstructiveSeverity, bronchitisSeverity);
        obstructiveSeverity = MAX(obstructiveSeverity, emphysemaSeverity);
      }

      // Bronchodilators
      //When a bronchodilator (like albuterol) is administered, the bronchodilation also causes the IE ratio to correct itself
      obstructiveSeverity *= 1.0 - m_AverageLocalTissueBronchodilationEffects / 0.001;
      obstructiveSeverity = LIMIT(obstructiveSeverity, 0.0, 1.0);

      //------------------------------------------------------------------------------------------------------
      //Restrictive = Increase

      //------------------------------------------------------------------------------------------------------
      //Pneumonia
      //Exacerbation will overwrite the condition, even if it means improvement
      if (m_data.GetConditions().HasPneumonia() || m_PatientActions->HasPneumoniaExacerbation())
      {
        double severity = 0.0;
        if (m_PatientActions->HasPneumoniaExacerbation())
        {
          severity = m_PatientActions->GetPneumoniaExacerbation().GetSeverity(cmpt).GetValue();
        }
        else
        {
          severity = m_data.GetConditions().GetPneumonia().GetSeverity(cmpt).GetValue();
        }

        restrictiveSeverity = MAX(restrictiveSeverity, severity);
      }

      //------------------------------------------------------------------------------------------------------
      // Pulmonary Fibrosis
      if (m_data.GetConditions().HasPulmonaryFibrosis())
      {
        double Severity = m_data.GetConditions().GetPulmonaryFibrosis().GetSeverity().GetValue();

        restrictiveSeverity = MAX(restrictiveSeverity, Severity);
      }

      //------------------------------------------------------------------------------------------------------
      //ARDS
      //Exacerbation will overwrite the condition, even if it means improvement
      if (m_data.GetConditions().HasAcuteRespiratoryDistressSyndrome() || m_PatientActions->HasAcuteRespiratoryDistressSyndromeExacerbation())
      {
        double severity = 0.0;
        if (m_PatientActions->HasAcuteRespiratoryDistressSyndromeExacerbation())
        {
          severity = m_PatientActions->GetAcuteRespiratoryDistressSyndromeExacerbation().GetSeverity(cmpt).GetValue();
        }
        else
        {
          severity = m_data.GetConditions().GetAcuteRespiratoryDistressSyndrome().GetSeverity(cmpt).GetValue();
        }
        restrictiveSeverity = MAX(restrictiveSeverity, severity);
      }

      //------------------------------------------------------------------------------------------------------
      //PBLI
      //Same as ARDS
      if (m_PatientActions->HasPrimaryBlastLungInjury())
      {
        double severity = m_PatientActions->GetPrimaryBlastLungInjury().GetSeverity(cmpt).GetValue();

        restrictiveSeverity = MAX(restrictiveSeverity, severity);
      }

      combinedObstructiveSeverity += obstructiveSeverity * alveoliVolumeRatio;
      combinedRestrictiveSeverity += restrictiveSeverity * alveoliVolumeRatio;
    }

    //------------------------------------------------------------------------------------------------------
    //Set new value

    // Approximate mapping without frequency effects (standard respiration rate)
    //
    // Factor | Inspiratory Fraction | IE Ratio
    // -------|----------------------|-----------
    // 0.5    | 0.17                 | 1:5 (0.2)
    // 0.75   | 0.25                 | 1:3 (0.33)
    // 1.0    | 0.33                 | 1:2 (0.5)
    // 1.5    | 0.5                  | 1:1 (1.0)
    // 2.0    | 0.67                 | 2:1 (2.0)

    //Restrictive = Increased IE Ratio
    m_IERatioScaleFactor *= GeneralMath::LinearInterpolator(0.0, 1.0, 1.0, 2.0, combinedRestrictiveSeverity);

    //Obstructive = Decreased IE Ratio
    m_IERatioScaleFactor *= GeneralMath::LinearInterpolator(0.0, 1.0, 1.0, 0.5, combinedObstructiveSeverity);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Update the diffusion surface area due to actions and conditions.
  ///
  /// \details
  /// The surface area is an important factor for gas exchange between the respiratory and 
  /// cardiovascular systems. Several conditions effectively reduce the area of alveoli that
  /// participate in gas exchange and cause increased CO2 and decreased O2 in the blood. This models 
  /// the destruction of alveoli membranes and/or lung consolidation.
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::UpdateDiffusion()
  {

    double initialPatientAlveoliDiffusionArea_cm2 = m_data.GetInitialPatient().GetAlveoliSurfaceArea(AreaUnit::cm2);

    //------------------------------------------------------------------------------------------------------
    //Impaired Alveolar Exchange
    if (m_data.GetConditions().HasImpairedAlveolarExchange() || m_PatientActions->HasImpairedAlveolarExchangeExacerbation())
    {
      if (m_PatientActions->HasImpairedAlveolarExchangeExacerbation())
      {
        SEImpairedAlveolarExchangeExacerbation& iaee = m_PatientActions->GetImpairedAlveolarExchangeExacerbation();
        unsigned int total = int(iaee.HasImpairedSurfaceArea()) +
          int(iaee.HasImpairedFraction()) +
          int(iaee.HasSeverity());

        if (total == 0)
        {
          Fatal("The Impaired Alveolar Exchange action must include a surface area, fraction, or severity.");
        }

        if (total > 1)
        {
          Warning("The Impaired Alveolar Exchange action is defined with mulitple values. Defaulting to the surface area value first, impaired fraction next, and severity last.");
        }

        if (iaee.HasImpairedSurfaceArea())
        {

          initialPatientAlveoliDiffusionArea_cm2 = initialPatientAlveoliDiffusionArea_cm2 - iaee.GetImpairedSurfaceArea(AreaUnit::cm2);
        }
        else if (iaee.HasImpairedFraction())
        {
          initialPatientAlveoliDiffusionArea_cm2 = (1.0 - iaee.GetImpairedFraction().GetValue()) * initialPatientAlveoliDiffusionArea_cm2;
        }
        else if (iaee.HasSeverity())
        {
          double severity = iaee.GetSeverity().GetValue();
          double gasDiffusionScalingFactor = GeneralMath::ExponentialDecayFunction(10, 0.1, 1.0, severity);
          initialPatientAlveoliDiffusionArea_cm2 *= gasDiffusionScalingFactor;
        }
      }
      else
      {
        unsigned int total = int(m_data.GetConditions().GetImpairedAlveolarExchange().HasImpairedSurfaceArea()) +
          int(m_data.GetConditions().GetImpairedAlveolarExchange().HasImpairedFraction()) +
          int(m_data.GetConditions().GetImpairedAlveolarExchange().HasSeverity());

        if (total == 0)
        {
          Fatal("The Impaired Alveolar Exchange action must include a surface area, fraction, or severity.");
        }

        if (total > 1)
        {
          Warning("The Impaired Alveolar Exchange condition is defined with mulitple values. Defaulting to the surface area value first, impaired fraction next, and severity last.");
        }

        if (m_data.GetConditions().GetImpairedAlveolarExchange().HasImpairedSurfaceArea())
        {
          initialPatientAlveoliDiffusionArea_cm2 = initialPatientAlveoliDiffusionArea_cm2 - m_data.GetConditions().GetImpairedAlveolarExchange().GetImpairedSurfaceArea(AreaUnit::cm2);
        }
        else if (m_data.GetConditions().GetImpairedAlveolarExchange().HasImpairedFraction())
        {
          initialPatientAlveoliDiffusionArea_cm2 = (1.0 - m_data.GetConditions().GetImpairedAlveolarExchange().GetImpairedFraction().GetValue()) * initialPatientAlveoliDiffusionArea_cm2;
        }
        else if (m_data.GetConditions().GetImpairedAlveolarExchange().HasSeverity())
        {
          double severity = m_data.GetConditions().GetImpairedAlveolarExchange().GetSeverity().GetValue();
          double gasDiffusionScalingFactor = GeneralMath::ExponentialDecayFunction(10, 0.1, 1.0, severity);
          initialPatientAlveoliDiffusionArea_cm2 *= gasDiffusionScalingFactor;
        }
      }
    }

    double totalScalingFactor = 1.0;
    double damageScalingFactor = 1.0;
    double recruitmentScalingFactor = 1.0;

    double totalBaselineAlveoliVolume_L = 0.0;
    for (auto& itr : m_LungComponents)
    {
      LungComponent& cpt = itr.second;
      totalBaselineAlveoliVolume_L += cpt.AlveoliNode->GetVolumeBaseline(VolumeUnit::L);
    }

    double totalAlveoliDiffusionArea_cm2 = 0.0;

    for (auto& itr : m_LungComponents)
    {
      eLungCompartment cmpt = itr.first;
      LungComponent& cpt = itr.second;

      SEGasCompartment* alveoliCompartment = cpt.AlveoliCompartment;
      double alveoliVolumeBaseline_L = cpt.AlveoliNode->GetVolumeBaseline(VolumeUnit::L);
      double alveoliVolumeRatio = alveoliVolumeBaseline_L / totalBaselineAlveoliVolume_L;
      double initialAlveoliDiffusionArea_cm2 = initialPatientAlveoliDiffusionArea_cm2 * alveoliVolumeRatio;
      //double residualVolume_L = m_data.GetInitialPatient().GetResidualVolume(VolumeUnit::L);

      //------------------------------------------------------------------------------------------------------
      //Restrictive - includes recruitment effects
      //------------------------------------------------------------------------------------------------------

      //------------------------------------------------------------------------------------------------------
      //Acinar recruitment
      //Alveoli volume decreases cause more shunting, such as with ARDS
      //Collapsed lung causes more shunting, such as with pneumothorax and hemothorax
      //Aeration improves when mechanically ventilated with increased PEEP

      //double alveoliVolume_L = cpt.AlveoliNode->GetNextVolume(VolumeUnit::L);
      //double cptResidualVolume_L = residualVolume_L * alveoliVolumeRatio;

      //0.01 is sensitivity factor to prevent hitting this when healthy
      //double recruitedFraction = (alveoliVolume_L - cptResidualVolume_L + 0.01) / (alveoliVolumeBaseline_L - cptResidualVolume_L);
      //recruitedFraction = LIMIT(recruitedFraction, 0.0, 1.0);

      //recruitmentScalingFactor = GeneralMath::ExponentialDecayFunction(10, 0.15, 1.0, 1.0 - recruitedFraction);

      double combinedSeverity = 0.0;
      //------------------------------------------------------------------------------------------------------
      //ARDS
      //Exacerbation will overwrite the condition, even if it means improvement
      if (m_data.GetConditions().HasAcuteRespiratoryDistressSyndrome() || m_PatientActions->HasAcuteRespiratoryDistressSyndromeExacerbation())
      {
        double severity = 0.0;
        if (m_PatientActions->HasAcuteRespiratoryDistressSyndromeExacerbation())
        {
          severity = m_PatientActions->GetAcuteRespiratoryDistressSyndromeExacerbation().GetSeverity(cmpt).GetValue();
        }
        else
        {
          severity = m_data.GetConditions().GetAcuteRespiratoryDistressSyndrome().GetSeverity(cmpt).GetValue();
        }

        combinedSeverity = MAX(combinedSeverity, severity);
      }

      //------------------------------------------------------------------------------------------------------
      //Pneumonia
      //Exacerbation will overwrite the condition, even if it means improvement
      if (m_data.GetConditions().HasPneumonia() || m_PatientActions->HasPneumoniaExacerbation())
      {
        double severity = 0.0;
        if (m_PatientActions->HasPneumoniaExacerbation())
        {
          severity = m_PatientActions->GetPneumoniaExacerbation().GetSeverity(cmpt).GetValue();
        }
        else
        {
          severity = m_data.GetConditions().GetPneumonia().GetSeverity(cmpt).GetValue();
        }

        combinedSeverity = MAX(combinedSeverity, severity);
      }

      //------------------------------------------------------------------------------------------------------
      //PulmonaryFibrosis
      if (m_data.GetConditions().HasPulmonaryFibrosis())
      {
        double severity = m_data.GetConditions().GetPulmonaryFibrosis().GetSeverity().GetValue();

        combinedSeverity = MAX(combinedSeverity, severity);
      }

      //------------------------------------------------------------------------------------------------------
      //PBLI
      //Same as ARDS with a multiplier
      double PBLIMultiplier = 1.0;
      if (m_PatientActions->HasPrimaryBlastLungInjury())
      {
        double severity = m_PatientActions->GetPrimaryBlastLungInjury().GetSeverity(cmpt).GetValue();

        PBLIMultiplier = GeneralMath::LinearInterpolator(0.0, 1.0, 1.0, 0.75, severity);

        combinedSeverity = MAX(combinedSeverity, severity);
      }

      std::vector<std::pair<double, double>>  interpolatorPoints =
      {
        {0.0, 1.000}, //None
        {0.3, 0.500}, //Mild
        {0.6, 0.200}, //Moderate
        {0.9, 0.100}, //Severe
        {1.0, 0.050}  //Max
      };

      damageScalingFactor = GeneralMath::PiecewiseLinearInterpolator(interpolatorPoints, combinedSeverity) * PBLIMultiplier;

      //------------------------------------------------------------------------------------------------------
      //Combine effects
      totalScalingFactor = MIN(recruitmentScalingFactor, damageScalingFactor);

      //------------------------------------------------------------------------------------------------------
      //Obstructive - does not includes recruitment effects
      //------------------------------------------------------------------------------------------------------

      //------------------------------------------------------------------------------------------------------
      //COPD
      //Exacerbation will overwrite the condition, even if it means improvement
      if (m_data.GetConditions().HasChronicObstructivePulmonaryDisease() || m_PatientActions->HasChronicObstructivePulmonaryDiseaseExacerbation())
      {
        double emphysemaSeverity = 0.0;
        if (m_PatientActions->HasChronicObstructivePulmonaryDiseaseExacerbation())
        {
          emphysemaSeverity = m_PatientActions->GetChronicObstructivePulmonaryDiseaseExacerbation().GetEmphysemaSeverity(cmpt).GetValue();
        }
        else
        {
          emphysemaSeverity = m_data.GetConditions().GetChronicObstructivePulmonaryDisease().GetEmphysemaSeverity(cmpt).GetValue();
        }

        interpolatorPoints =
        {
          {0.0, 1.000}, //None
          {0.3, 0.500}, //Mild
          {0.6, 0.200}, //Moderate
          {0.9, 0.100}, //Severe
          {1.0, 0.050}  //Max
        };

        totalScalingFactor = MIN(totalScalingFactor, GeneralMath::PiecewiseLinearInterpolator(interpolatorPoints, emphysemaSeverity));
      }

      //------------------------------------------------------------------------------------------------------
      double alveoliDiffusionArea_cm2 = initialAlveoliDiffusionArea_cm2 * totalScalingFactor;
      double perviousAlveoliDiffusionArea_cm2 = alveoliCompartment->GetDiffusionSurfaceArea(AreaUnit::cm2);

      if (m_data.GetState() > EngineState::InitialStabilization) //Only dampen response if we're not initializing
      {
        //Dampen the change to prevent potential craziness
        //It will only change a fraction as much as it wants to each time step to ensure it's critically damped and doesn't overshoot
        double dampenFraction_perSec = 0.001 * 50.0;
        alveoliDiffusionArea_cm2 = GeneralMath::Damper(alveoliDiffusionArea_cm2, perviousAlveoliDiffusionArea_cm2, dampenFraction_perSec, m_data.GetTimeStep_s());
      }
      alveoliCompartment->GetDiffusionSurfaceArea().SetValue(alveoliDiffusionArea_cm2, AreaUnit::cm2);

      totalAlveoliDiffusionArea_cm2 += alveoliDiffusionArea_cm2;
    }

    //------------------------------------------------------------------------------------------------------
    m_data.GetCurrentPatient().GetAlveoliSurfaceArea().SetValue(totalAlveoliDiffusionArea_cm2, AreaUnit::cm2);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Update Pulmonary Capillary Resistance 
  ///
  /// \details
  /// This method determines a resistance scaling factor from COPD severity to updates the pulmonary 
  /// capillary resistance in order to model the destruction of capillaries in the alveolus membrane.
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::UpdatePulmonaryCapillary()
  {
    for (auto& itr : m_LungComponents)
    {
      eLungCompartment cmpt = itr.first;
      LungComponent& cpt = itr.second;

      SEFluidCircuitPath* pulmonaryCapillaryPath = cpt.VeinsPath;

      double combinedSeverity = 0.0;

      double pulmonaryResistanceMultiplier = 1.0;
      //-------------------------------------------------------------------------------------------------------------------
      //COPD
      //Exacerbation will overwrite the condition, even if it means improvement
      if (m_data.GetConditions().HasChronicObstructivePulmonaryDisease() || m_PatientActions->HasChronicObstructivePulmonaryDiseaseExacerbation())
      {
        double severity = 0.0;
        if (m_PatientActions->HasChronicObstructivePulmonaryDiseaseExacerbation())
        {
          severity = m_PatientActions->GetChronicObstructivePulmonaryDiseaseExacerbation().GetEmphysemaSeverity(cmpt).GetValue();
        }
        else
        {
          severity = m_data.GetConditions().GetChronicObstructivePulmonaryDisease().GetEmphysemaSeverity(cmpt).GetValue();
        }

        combinedSeverity = severity;
        pulmonaryResistanceMultiplier = GeneralMath::LinearInterpolator(0.0, 1.0, 1.0, 6.0, combinedSeverity);
      }

      //-------------------------------------------------------------------------------------------------------------------
      //PulmonaryFibrosis
      if (m_data.GetConditions().HasPulmonaryFibrosis())
      {
        double severity = m_data.GetConditions().GetPulmonaryFibrosis().GetSeverity().GetValue();
        combinedSeverity = MAX(combinedSeverity, severity);
        pulmonaryResistanceMultiplier = GeneralMath::LinearInterpolator(0.0, 1.0, 1.0, 6.0, combinedSeverity);
      }

      //-------------------------------------------------------------------------------------------------------------------

      double previousPulmonaryCapillaryResistance = pulmonaryCapillaryPath->GetResistance().GetValue(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
      double pulmonaryCapillaryResistance = pulmonaryCapillaryPath->GetNextResistance().GetValue(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
      pulmonaryCapillaryResistance *= pulmonaryResistanceMultiplier;

      if (m_data.GetState() > EngineState::InitialStabilization) //Only dampen response if we're not initializing
      {
        //Dampen the change to prevent potential craziness
        double dampenFraction_perSec = 0.001 * 50.0;
        pulmonaryCapillaryResistance = GeneralMath::Damper(pulmonaryCapillaryResistance, previousPulmonaryCapillaryResistance, dampenFraction_perSec, m_data.GetTimeStep_s());
      }

      pulmonaryCapillaryPath->GetNextResistance().SetValue(pulmonaryCapillaryResistance, PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Update Pulmonary Shunt Resistance 
  ///
  /// \details
  /// This decreases the physiologic pulmonary shunt in the cardiovascular system.  The shunt allows 
  /// deoxgenated blood to pass without participating in gas exchange.  This often occurs with alveoli
  /// filled with fluid.
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::UpdatePulmonaryShunt()
  {
    double residualVolume_L = m_data.GetInitialPatient().GetResidualVolume(VolumeUnit::L);

    double totalBaselineAlveoliVolume_L = 0.0;
    for (auto& itr : m_LungComponents)
    {
      LungComponent& cpt = itr.second;
      totalBaselineAlveoliVolume_L += cpt.AlveoliNode->GetVolumeBaseline(VolumeUnit::L);
    }

    double totalShuntFlow_mL_Per_s = 0.0;
    double totalPulmonaryFlow_mL_Per_s = 0.0;

    unsigned int iter = 0;
    for (auto& itr : m_LungComponents)
    {
      eLungCompartment cmpt = itr.first;
      LungComponent& cpt = itr.second;

      SEFluidCircuitNode* alveoliNode = cpt.AlveoliNode;
      SELiquidCompartmentLink* shuntLink = cpt.ShuntLink;
      SELiquidCompartmentLink* arteriesLink = cpt.ArteriesLink;
      SELiquidCompartmentLink* veinsLink = cpt.VeinsLink;
      SEFluidCircuitPath* shuntPath = cpt.ShuntPath;
      SEFluidCircuitPath* arteriesPath = cpt.ArteriesPath;
      SEFluidCircuitPath* veinsPath = cpt.VeinsPath;

      double combinedSeverity = 0.0;
      double recruitedFraction = 0.0;
      double recruitmentScalingFactor = 0.0;
      double totalScalingFactor = 0.0;
      double damageScalingFactor = 0.0;

      //------------------------------------------------------------------------------------------------------
      //Acinar recruitment
      //Alveoli volume decreases cause more shunting, such as with ARDS
      //Collapsed lung causes more shunting, such as with pneumothorax and hemothorax
      //Aeration improves when mechanically ventilated with increased PEEP

      double alveoliVolumeBaseline_L = alveoliNode->GetVolumeBaseline(VolumeUnit::L);
      double alveoliVolumeRatio = alveoliVolumeBaseline_L / totalBaselineAlveoliVolume_L;
      double alveoliVolume_L = alveoliNode->GetNextVolume(VolumeUnit::L);
      double cptResidualVolume_L = residualVolume_L * alveoliVolumeRatio;

      //0.01 is sensitivity factor to prevent hitting this when healthy
      recruitedFraction = (alveoliVolume_L - cptResidualVolume_L + 0.01) / (alveoliVolumeBaseline_L - cptResidualVolume_L);
      recruitedFraction = LIMIT(recruitedFraction, 0.0, 1.0);

      //------------------------------------------------------------------------------------------------------
      //Pneumonia
      //Exacerbation will overwrite the condition, even if it means improvement
      if (m_data.GetConditions().HasPneumonia() || m_PatientActions->HasPneumoniaExacerbation())
      {
        double severity = 0.0;
        if (m_PatientActions->HasPneumoniaExacerbation())
        {
          severity = m_PatientActions->GetPneumoniaExacerbation().GetSeverity(cmpt).GetValue();
        }
        else
        {
          severity = m_data.GetConditions().GetPneumonia().GetSeverity(cmpt).GetValue();
        }

        combinedSeverity = MAX(combinedSeverity, severity);
      }

      //------------------------------------------------------------------------------------------------------
      //ARDS
      //Exacerbation will overwrite the condition, even if it means improvement
      if (m_data.GetConditions().HasAcuteRespiratoryDistressSyndrome() || m_PatientActions->HasAcuteRespiratoryDistressSyndromeExacerbation())
      {
        double severity = 0.0;
        if (m_PatientActions->HasAcuteRespiratoryDistressSyndromeExacerbation())
        {
          severity = m_PatientActions->GetAcuteRespiratoryDistressSyndromeExacerbation().GetSeverity(cmpt).GetValue();
        }
        else
        {
          severity = m_data.GetConditions().GetAcuteRespiratoryDistressSyndrome().GetSeverity(cmpt).GetValue();
        }

        combinedSeverity = MAX(combinedSeverity, severity);
      }

      //------------------------------------------------------------------------------------------------------
      //PBLI
      //Use a multiplier on ARDS equation
      double PBLIMultiplier = 1.0;
      if (m_PatientActions->HasPrimaryBlastLungInjury())
      {
        double severity = m_PatientActions->GetPrimaryBlastLungInjury().GetSeverity(cmpt).GetValue();

        PBLIMultiplier = GeneralMath::LinearInterpolator(0.0, 1.0, 1.0, 0.5, severity);

        combinedSeverity = MAX(combinedSeverity, severity);
      }

      //------------------------------------------------------------------------------------------------------
      //Combine effects
      //Damage factor acts as floor if fully recruited
      std::vector<std::pair<double, double>> interpolatorPoints =
      {
        {0.0, 0.0}, //None
        {0.3, 0.35}, //Mild
        {0.6, 0.35}, //Moderate
        {0.9, 0.5}, //Severe
        {1.0, 1.0}  //Max
      };
      double damageScalingFactor = GeneralMath::PiecewiseLinearInterpolator(interpolatorPoints, combinedSeverity) * PBLIMultiplier;

      interpolatorPoints =
      {
        {0.0, 0.0}, //None
        {0.1, 0.2},
        {0.2, 0.2},
        {0.3, 0.3},
        {0.4, 0.4},
        {0.5, 0.5},
        {0.6, 0.6},
        {0.7, 0.6},
        {0.8, 0.6},
        {0.9, 0.7},
        {1.0, 0.8} //Max
      };
      recruitmentScalingFactor = GeneralMath::PiecewiseLinearInterpolator(interpolatorPoints, 1.0 - recruitedFraction);
      //Dampen the change to prevent potential craziness
      double dampenFraction_perSec = 0.001 * 50.0;
      recruitmentScalingFactor = GeneralMath::Damper(recruitmentScalingFactor, m_PreviousShuntScalingFactor[iter], dampenFraction_perSec, m_data.GetTimeStep_s());
      m_PreviousShuntScalingFactor[iter] = recruitmentScalingFactor;

      double totalScalingFactor = MIN(recruitmentScalingFactor, damageScalingFactor) * PBLIMultiplier;

      //------------------------------------------------------------------------------------------------------
      //COPD
      //Exacerbation will overwrite the condition, even if it means improvement
      if (m_data.GetConditions().HasChronicObstructivePulmonaryDisease() || m_PatientActions->HasChronicObstructivePulmonaryDiseaseExacerbation())
      {
        double emphysemaSeverity = 0.0;
        if (m_PatientActions->HasChronicObstructivePulmonaryDiseaseExacerbation())
        {
          emphysemaSeverity = m_PatientActions->GetChronicObstructivePulmonaryDiseaseExacerbation().GetEmphysemaSeverity(cmpt).GetValue();
        }
        else
        {
          emphysemaSeverity = m_data.GetConditions().GetChronicObstructivePulmonaryDisease().GetEmphysemaSeverity(cmpt).GetValue();
        }

        interpolatorPoints =
        {
          {0.0, 0.0}, //None
          {0.3, 0.2}, //Mild
          {0.6, 0.3}, //Moderate
          {0.9, 0.35}, //Severe
          {1.0, 0.5}  //Max
        };
        double scalingFactor = GeneralMath::PiecewiseLinearInterpolator(interpolatorPoints, emphysemaSeverity);
        damageScalingFactor = MAX(damageScalingFactor, scalingFactor);
      }

      //------------------------------------------------------------------------------------------------------
      //PulmonaryShunt
      //Exacerbation will overwrite the condition, even if it means improvement
      if (m_data.GetConditions().HasPulmonaryShunt() || m_PatientActions->HasPulmonaryShuntExacerbation())
      {
        double severity = 0.0;
        if (m_PatientActions->HasPulmonaryShuntExacerbation())
        {
          severity = m_PatientActions->GetPulmonaryShuntExacerbation().GetSeverity().GetValue();
        }
        else
        {
          severity = m_data.GetConditions().GetPulmonaryShunt().GetSeverity().GetValue();
        }

        double scalingFactor = severity;
        damageScalingFactor = MAX(damageScalingFactor, scalingFactor);
      }

      //Apply Damage threshold
      totalScalingFactor = MAX(recruitmentScalingFactor, damageScalingFactor);

      //------------------------------------------------------------------------------------------------------
      //Apply the shunt
      if (totalScalingFactor > ZERO_APPROX)
      {
        //This is phsyiologic (not anatomic) shunt flow, so we'll manually update the links,
        //which will allow the fluid mechanics to remain constant, especially the capillary pressures
        double shuntFlow_mL_Per_s = shuntPath->GetNextFlow(VolumePerTimeUnit::mL_Per_s);
        double arteriesFlow_mL_Per_s = arteriesPath->GetNextFlow(VolumePerTimeUnit::mL_Per_s);
        double veinsFlow_mL_Per_s = veinsPath->GetNextFlow(VolumePerTimeUnit::mL_Per_s);

        //A scaling factor of 1.0 is approximately 50% shunt
        double initialArteriesFlow_mL_Per_s = arteriesFlow_mL_Per_s;
        arteriesFlow_mL_Per_s *= (1.0 - totalScalingFactor / 2.0);

        double flowChange__mL_Per_s = initialArteriesFlow_mL_Per_s - arteriesFlow_mL_Per_s;
        veinsFlow_mL_Per_s -= flowChange__mL_Per_s;
        shuntFlow_mL_Per_s += flowChange__mL_Per_s;

        arteriesFlow_mL_Per_s = LIMIT(arteriesFlow_mL_Per_s, 0.0, 2000.0);
        veinsFlow_mL_Per_s = LIMIT(veinsFlow_mL_Per_s, 0.0, 2000.0);
        shuntFlow_mL_Per_s = LIMIT(shuntFlow_mL_Per_s, 0.0, 2000.0);

        shuntLink->GetFlow().SetReadOnly(false);
        arteriesLink->GetFlow().SetReadOnly(false);
        veinsLink->GetFlow().SetReadOnly(false);

        shuntLink->RemovePath();
        arteriesLink->RemovePath();
        veinsLink->RemovePath();

        shuntLink->GetFlow().SetValue(shuntFlow_mL_Per_s, VolumePerTimeUnit::mL_Per_s);
        arteriesLink->GetFlow().SetValue(arteriesFlow_mL_Per_s, VolumePerTimeUnit::mL_Per_s);
        veinsLink->GetFlow().SetValue(veinsFlow_mL_Per_s, VolumePerTimeUnit::mL_Per_s);

        shuntLink->GetFlow().SetReadOnly(true);
        arteriesLink->GetFlow().SetReadOnly(true);
        veinsLink->GetFlow().SetReadOnly(true);

        totalShuntFlow_mL_Per_s += shuntFlow_mL_Per_s;
        totalPulmonaryFlow_mL_Per_s += shuntFlow_mL_Per_s + arteriesFlow_mL_Per_s;
      }
      else
      {
        if (!shuntLink->HasPath())
        {
          shuntLink->MapPath(*shuntPath);
        }
        if (!arteriesLink->HasPath())
        {
          arteriesLink->MapPath(*arteriesPath);
        }
        if (!veinsLink->HasPath())
        {
          veinsLink->MapPath(*veinsPath);
        }
      }

      iter++;
    }

    if (totalPulmonaryFlow_mL_Per_s > ZERO_APPROX)
    {
      GetPhysiologicShuntFraction().SetValue(totalShuntFlow_mL_Per_s / totalPulmonaryFlow_mL_Per_s);
    }
    else
    {
      GetPhysiologicShuntFraction().SetValue(0.0);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Reduce the driver pressure based on actions and conditions.
  ///
  /// \details
  /// This method scales the driver pressure lower than "requested" by the chemoreceptors and other
  /// nervous system / PD effects. This ultamitely caused lower O2 in the blood and higher respiration
  /// rates compared to lower tidal volumes.
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::UpdateDriverPressure()
  {
    double totalBaselineAlveoliVolume_L = 0.0;
    for (auto& itr : m_LungComponents)
    {
      LungComponent& cpt = itr.second;
      totalBaselineAlveoliVolume_L += cpt.AlveoliNode->GetVolumeBaseline(VolumeUnit::L);
    }

    double dyspneaSeverity = 0.0;
    for (auto& itr : m_LungComponents)
    {
      eLungCompartment cmpt = itr.first;
      LungComponent& cpt = itr.second;

      double alveoliVolumeBaseline_L = cpt.AlveoliNode->GetVolumeBaseline(VolumeUnit::L);
      double alveoliVolumeRatio = alveoliVolumeBaseline_L / totalBaselineAlveoliVolume_L;

      double obstructiveModifier = 0.0;
      //------------------------------------------------------------------------------------------------------
      //COPD
      //Exacerbation will overwrite the condition, even if it means improvement
      if (m_data.GetConditions().HasChronicObstructivePulmonaryDisease() || m_PatientActions->HasChronicObstructivePulmonaryDiseaseExacerbation())
      {
        double bronchitisSeverity = 0.0;
        double emphysemaSeverity = 0.0;
        if (m_PatientActions->HasChronicObstructivePulmonaryDiseaseExacerbation())
        {
          emphysemaSeverity = m_PatientActions->GetChronicObstructivePulmonaryDiseaseExacerbation().GetEmphysemaSeverity(cmpt).GetValue();
          bronchitisSeverity = m_PatientActions->GetChronicObstructivePulmonaryDiseaseExacerbation().GetBronchitisSeverity().GetValue();
        }
        else
        {
          emphysemaSeverity = m_data.GetConditions().GetChronicObstructivePulmonaryDisease().GetEmphysemaSeverity(cmpt).GetValue();
          bronchitisSeverity = m_data.GetConditions().GetChronicObstructivePulmonaryDisease().GetBronchitisSeverity().GetValue();
        }

        //It seems that the resistance does the job, so this doesn't really do anything
        obstructiveModifier = GeneralMath::LinearInterpolator(0.0, 1.0, 0.0, 0.01, MAX(emphysemaSeverity, bronchitisSeverity));
      }

      double restrictiveSeverity = 0.0;
      //------------------------------------------------------------------------------------------------------
      //Pneumonia
      //Exacerbation will overwrite the condition, even if it means improvement
      if (m_data.GetConditions().HasPneumonia() || m_PatientActions->HasPneumoniaExacerbation())
      {
        double severity = 0.0;
        if (m_PatientActions->HasPneumoniaExacerbation())
        {
          severity = m_PatientActions->GetPneumoniaExacerbation().GetSeverity(cmpt).GetValue();
        }
        else
        {
          severity = m_data.GetConditions().GetPneumonia().GetSeverity(cmpt).GetValue();
        }

        restrictiveSeverity = MAX(restrictiveSeverity, severity);
      }

      //------------------------------------------------------------------------------------------------------
      //Pulmonary Fibrosis
      if (m_data.GetConditions().HasPulmonaryFibrosis())
      {
        double severity = m_data.GetConditions().GetPulmonaryFibrosis().GetSeverity().GetValue();

        restrictiveSeverity = MAX(restrictiveSeverity, severity);
      }

      //------------------------------------------------------------------------------------------------------
      //ARDS
      //Exacerbation will overwrite the condition, even if it means improvement
      if (m_data.GetConditions().HasAcuteRespiratoryDistressSyndrome() || m_PatientActions->HasAcuteRespiratoryDistressSyndromeExacerbation())
      {
        double severity = 0.0;
        if (m_PatientActions->HasAcuteRespiratoryDistressSyndromeExacerbation())
        {
          severity = m_PatientActions->GetAcuteRespiratoryDistressSyndromeExacerbation().GetSeverity(cmpt).GetValue();
        }
        else
        {
          severity = m_data.GetConditions().GetAcuteRespiratoryDistressSyndrome().GetSeverity(cmpt).GetValue();
        }

        restrictiveSeverity = MAX(restrictiveSeverity, severity);
      }

      //------------------------------------------------------------------------------------------------------
      //PBLI
      //Same as ARDS
      if (m_PatientActions->HasPrimaryBlastLungInjury())
      {
        double severity = m_PatientActions->GetPrimaryBlastLungInjury().GetSeverity(cmpt).GetValue();

        restrictiveSeverity = MAX(restrictiveSeverity, severity);
      }

      double restrictiveModifier = GeneralMath::LinearInterpolator(0.0, 1.0, 0.0, 0.8, restrictiveSeverity);

      //------------------------------------------------------------------------------------------------------
      if (!m_PositivePressureVentilation)
      {
        double combinedSeverity = MAX(obstructiveModifier, restrictiveModifier);
        dyspneaSeverity += combinedSeverity * alveoliVolumeRatio;
      }
    }

    //------------------------------------------------------------------------------------------------------
    //Dyspnea
    if (m_PatientActions->HasDyspnea() && m_PatientActions->GetDyspnea().HasTidalVolumeSeverity())
    {
      dyspneaSeverity = MAX(dyspneaSeverity, m_PatientActions->GetDyspnea().GetTidalVolumeSeverity().GetValue());
    }

    //------------------------------------------------------------------------------------------------------
    //Fatigue
    if (m_PatientActions->HasRespiratoryFatigue())
    {
      dyspneaSeverity = MAX(dyspneaSeverity, m_PatientActions->GetRespiratoryFatigue().GetSeverity().GetValue());
    }

    //------------------------------------------------------------------------------------------------------
    //Cardiac Arrest
    if (m_data.GetEvents().IsEventActive(eEvent::CardiacArrest))
    {
      dyspneaSeverity = 1.0;
    }

    //------------------------------------------------------------------------------------------------------
    //Collapsed lung - Pneumothorax and Hemothorax
    //Causes labored breathing
    double rightLungRatio = m_data.GetCurrentPatient().GetRightLungRatio().GetValue();
    double leftLungRatio = 1.0 - rightLungRatio;
    double functionalResidualCapacity_L = m_data.GetCurrentPatient().GetFunctionalResidualCapacity(VolumeUnit::L);
    double residualVolume_L = m_data.GetCurrentPatient().GetResidualVolume(VolumeUnit::L);

    double leftLungVolume_L = m_LeftLung->GetVolume(VolumeUnit::L);
    double rightLungVolume_L = m_RightLung->GetVolume(VolumeUnit::L);
    double leftLungFunctionalResidualCapacity_L = functionalResidualCapacity_L * leftLungRatio;
    double rightLungFunctionalResidualCapacity_L = functionalResidualCapacity_L * rightLungRatio;
    double leftLungResidualVolume_L = residualVolume_L * leftLungRatio;
    double rightLungResidualVolume_L = residualVolume_L * rightLungRatio;

    double volumeSensitivity_L = 0.01;
    if (leftLungVolume_L < leftLungFunctionalResidualCapacity_L - volumeSensitivity_L)
    {
      leftLungVolume_L = LIMIT(leftLungVolume_L, leftLungResidualVolume_L, leftLungFunctionalResidualCapacity_L);
      //Linear function: Min = 0.0, Max = 0.8 (increasing with severity)
      double thisDyspneaSeverity = GeneralMath::LinearInterpolator(leftLungResidualVolume_L, leftLungFunctionalResidualCapacity_L, 0.5, 0.0, leftLungVolume_L);
      dyspneaSeverity = MAX(dyspneaSeverity, thisDyspneaSeverity);
    }
    if (rightLungVolume_L < rightLungFunctionalResidualCapacity_L - volumeSensitivity_L)
    {
      rightLungVolume_L = LIMIT(rightLungVolume_L, rightLungResidualVolume_L, rightLungFunctionalResidualCapacity_L);
      //Linear function: Min = 0.0, Max = 0.8 (increasing with severity)
      double thisDyspneaSeverity = GeneralMath::LinearInterpolator(rightLungResidualVolume_L, rightLungFunctionalResidualCapacity_L, 0.5, 0.0, rightLungVolume_L);
      dyspneaSeverity = MAX(dyspneaSeverity, thisDyspneaSeverity);
    }

    //------------------------------------------------------------------------------------------------------
    m_NotBreathing = false;
    double respirationRateDyspneaSeverity = 0.0;
    if (m_PatientActions->GetDyspnea().HasRespirationRateSeverity())
    {
      respirationRateDyspneaSeverity = m_PatientActions->GetDyspnea().GetRespirationRateSeverity().GetValue();
    }
    if (SEScalar::IsZero(1.0 - dyspneaSeverity, ZERO_APPROX) ||
      SEScalar::IsZero(1.0 - respirationRateDyspneaSeverity, ZERO_APPROX)) //~1.0
    {
      dyspneaSeverity = 1.0;
      m_NotBreathing = true;
      m_MechanoreceptorsDyspneaFactor = 0.0;
      m_AppliedMechanoreceptorsDyspneaFactor = 0.0;
    }
    else
    {
      // ------------------------------------------------------------------------------------------------------
      if (m_PreviousDyspneaSeverity != dyspneaSeverity &&
        m_data.GetState() == EngineState::Active) //Only dampen response if we're not initializing
      {
        //Dampen the change to prevent potential craziness
        //It will only change a fraction as much as it wants to each time step to ensure it's critically damped and doesn't overshoot
        double dampenFraction_perSec = 0.0006 * 50.0;
        dyspneaSeverity = GeneralMath::Damper(dyspneaSeverity, m_PreviousDyspneaSeverity, dampenFraction_perSec, m_data.GetTimeStep_s());
      }

      //------------------------------------------------------------------------------------------------------
      //Mechanoreceptors
      CalculateMechanoreceptors();

      if (m_data.GetAirwayMode() == eAirwayMode::MechanicalVentilation ||
        m_data.GetAirwayMode() == eAirwayMode::MechanicalVentilator)
      {
        //Dampen the change to prevent pressure waveform strangeness
        //This needs to be way faster than other dyspnea reasons because it's applied during each breath seperately
        double dampenFraction_perSec = 0.01;
        m_AppliedMechanoreceptorsDyspneaFactor = GeneralMath::Damper(m_MechanoreceptorsDyspneaFactor, m_PreviousDyspneaSeverity, dampenFraction_perSec, m_data.GetTimeStep_s());
        dyspneaSeverity = MAX(dyspneaSeverity, m_AppliedMechanoreceptorsDyspneaFactor);
      }
    }

    m_PreviousDyspneaSeverity = dyspneaSeverity;


    //------------------------------------------------------------------------------------------------------
    //Reduce the tidal volume by the percentage given
    m_DriverPressure_cmH2O = m_DriverPressure_cmH2O * (1 - dyspneaSeverity);

    //Modifiers
    if (m_MechanicsModifiers->HasTidalVolumeMultiplier())
    {
      m_DriverPressure_cmH2O *= m_MechanicsModifiers->GetTidalVolumeMultiplier().GetValue();
    }

#ifdef DEBUG
    m_data.GetDataTrack().Probe("dyspneaSeverity", dyspneaSeverity);
#endif
  }

//--------------------------------------------------------------------------------------------------
/// \brief
/// Reduce the driver pressure based on pulmonary mechanoreceptors.
///
/// \details
/// This method scales the driver pressure lower due to the pulmonary mechanoreceptors. There is no
/// scaling unless inflation is caused by an outside source.
//--------------------------------------------------------------------------------------------------
  void RespiratoryModel::CalculateMechanoreceptors()
  {
    if (!(m_data.GetAirwayMode() == eAirwayMode::MechanicalVentilation ||
      m_data.GetAirwayMode() == eAirwayMode::MechanicalVentilator))
    {
      //Only do this with certain equipement
      //For example, the simple mask and anesthesia machine act weird
      return;
    }

    double airwayPressure_cmH2O = m_AirwayNode->GetNextPressure(PressureUnit::cmH2O) - m_AmbientNode->GetNextPressure(PressureUnit::cmH2O);
    double musclePressure_cmH2O = m_RespiratoryMuscleNode->GetNextPressure(PressureUnit::cmH2O) - m_AmbientNode->GetNextPressure(PressureUnit::cmH2O);
    double flow_L_Per_s = -m_DriverPressurePath->GetNextFlow(VolumePerTimeUnit::mL_Per_s);

    if (airwayPressure_cmH2O <= 0.0 //Not assisted
      || !(flow_L_Per_s > 0.01) //Exhaling or not breathing
      || musclePressure_cmH2O > -5.0) //Not breathing hard
    {
      return;
    }

    //What it has
    double pressureDifference_cmH2O = airwayPressure_cmH2O - musclePressure_cmH2O;
    double respiratoryResistance_cmH2O_s_Per_L = std::abs(pressureDifference_cmH2O / flow_L_Per_s);

    //What it wants
    double unassistedPressureDifference_cmH2O = 0.0 - m_DriverPressure_cmH2O;
    double unassistedFlow_L_Per_s = unassistedPressureDifference_cmH2O / respiratoryResistance_cmH2O_s_Per_L;

    //To achieve target
    double assistedDriverPressure_cmH2O = -(respiratoryResistance_cmH2O_s_Per_L * unassistedFlow_L_Per_s - airwayPressure_cmH2O);
    double mechanoreceptorsDyspneaFactor = 1.0 - assistedDriverPressure_cmH2O / -m_DriverPressure_cmH2O;

    mechanoreceptorsDyspneaFactor = LIMIT(mechanoreceptorsDyspneaFactor, 0.0, 0.8); //Cap it

    //Hold onto it in a member veriable so it does reduce later and mess up the waveform
    m_MechanoreceptorsDyspneaFactor = MAX(m_MechanoreceptorsDyspneaFactor, mechanoreceptorsDyspneaFactor);
  }

//--------------------------------------------------------------------------------------------------
/// \brief
/// Reduce the driver period based on actions and conditions.
///
/// \details
/// This method scales the driver period lower than "requested" by the chemoreceptors and other
/// nervous system / PD effects. This ultamitely caused lower O2 in the blood and higher tidal 
/// volumes compared to lower respiration rates volumes.
//--------------------------------------------------------------------------------------------------
  void RespiratoryModel::UpdateDriverPeriod()
  {
    //Dyspnea
    if (m_PatientActions->HasDyspnea() && m_PatientActions->GetDyspnea().HasRespirationRateSeverity())
    {
      double dyspneaSeverity = m_PatientActions->GetDyspnea().GetRespirationRateSeverity().GetValue();
      m_VentilationFrequency_Per_min = m_VentilationFrequency_Per_min * (1 - dyspneaSeverity);
    }

    //------------------------------------------------------------------------------------------------------
    //Modifiers
    if (m_MechanicsModifiers->HasRespirationRateMultiplier())
    {
      m_VentilationFrequency_Per_min *= m_MechanicsModifiers->GetRespirationRateMultiplier().GetValue();
    }
  }

  //--------------------------------------------------------------------------------------------------
/// \brief
/// Reduce the driver respiration rate and tidal volume based on actions and conditions.
///
/// \details
/// This method scales the chemoreceptor driver and results in higher arterial CO2 and lower O2.
//--------------------------------------------------------------------------------------------------
  double RespiratoryModel::UpdateTargetVentilation(double targetAlveolarVentilation_L_Per_min)
  {
    //Metabolic modifier is used to drive the system to reasonable levels achievable during increased metabolic exertion
    //The modifier is tuned to achieve the correct respiratory response for near maximal exercise. A linear relationship is assumed
    // for the respiratory effects due to increased metabolic exertion
    double tunedVolumeMetabolicSlope = 0.2; //Tuned fractional increase of the tidal volume due to increased metabolic rate
    double metabolicModifier = 1.0;
    double TMR_W = m_data.GetEnergy().GetTotalMetabolicRate(PowerUnit::W);
    double BMR_W = m_data.GetCurrentPatient().GetBasalMetabolicRate(PowerUnit::W);
    double metabolicFraction = TMR_W / BMR_W;
    
    metabolicModifier = 1.0 + tunedVolumeMetabolicSlope * (metabolicFraction - 1.0);
    double newTargetAlveolarVentilation_L_Per_min = metabolicModifier * targetAlveolarVentilation_L_Per_min;

    //Don't let dyspnea just max out by backing off of what the chemoreceptors want to acheive
    if (m_PatientActions->HasDyspnea())
    {
      double dyspneaSeverity = 0.0; //This is a sum, so value between 0 and 2

      if (m_PatientActions->GetDyspnea().HasRespirationRateSeverity())
      {
        dyspneaSeverity += m_PatientActions->GetDyspnea().GetRespirationRateSeverity().GetValue();
      }
      if (m_PatientActions->GetDyspnea().HasTidalVolumeSeverity())
      {
        dyspneaSeverity += m_PatientActions->GetDyspnea().GetTidalVolumeSeverity().GetValue();
      }

      double modifier = GeneralMath::LinearInterpolator(0.0, 2.0, 1.0, 0.0, dyspneaSeverity);
      newTargetAlveolarVentilation_L_Per_min *= modifier;
    }

    return newTargetAlveolarVentilation_L_Per_min;
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Override the total system resistance.
  ///
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::SetRespiratoryResistance()
  {
    if (m_RespiratoryResistanceOverride_cmH2O_s_Per_L < 0.0)
    {
      return;
    }

    //Set the Airway resistance to get achieve this given value
    double leftBronchiResistance_cmH2O_s_Per_L = m_CarinaToLeftAnatomicDeadSpace->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    double rightBronchiResistance_cmH2O_s_Per_L = m_CarinaToRightAnatomicDeadSpace->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    double leftAlveoliDuctResistance_cmH2O_s_Per_L = m_LeftAnatomicDeadSpaceToLeftAlveolarDeadSpace->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    double rightAlveoliDuctResistance_cmH2O_s_Per_L = m_RightAnatomicDeadSpaceToRightAlveolarDeadSpace->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);

    //Do the circuit math
    double leftSideResistance_cmH2O_s_Per_L = leftBronchiResistance_cmH2O_s_Per_L + leftAlveoliDuctResistance_cmH2O_s_Per_L;
    double rightSideResistance_cmH2O_s_Per_L = rightBronchiResistance_cmH2O_s_Per_L + rightAlveoliDuctResistance_cmH2O_s_Per_L;
    double totalDownstreamResistance_cmH2O_s_Per_L = 1.0 / (1.0 / leftSideResistance_cmH2O_s_Per_L + 1.0 / rightSideResistance_cmH2O_s_Per_L);
    double airwayResistance_cmH2O_s_Per_L = m_RespiratoryResistanceOverride_cmH2O_s_Per_L - totalDownstreamResistance_cmH2O_s_Per_L;

    if (airwayResistance_cmH2O_s_Per_L <= 0)
    {
      Error("Ignoring the resistance override.  The airway resistance cannot be lowered enough to meet the criteria.");
    }

    m_PharynxToCarina->GetNextResistance().SetValue(airwayResistance_cmH2O_s_Per_L, PressureTimePerVolumeUnit::cmH2O_s_Per_L);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Override the total system compliance.
  ///
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::SetRespiratoryCompliance()
  {
    if (m_RespiratoryComplianceOverride_L_Per_cmH2O < 0.0)
    {
      return;
    }

    //Set all compliances the same
    double RespiratorySideCompliance_L_Per_cmH2O = m_RespiratoryComplianceOverride_L_Per_cmH2O / 2.0;
    double LungCompliance_L_Per_cmH2O = 2.0 * RespiratorySideCompliance_L_Per_cmH2O;
    double ChestWallCompliance_L_Per_cmH2O = LungCompliance_L_Per_cmH2O;

    m_LeftAlveoliToLeftPleuralConnection->GetNextCompliance().SetValue(LungCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    m_RightAlveoliToRightPleuralConnection->GetNextCompliance().SetValue(LungCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    m_LeftPleuralToRespiratoryMuscle->GetNextCompliance().SetValue(ChestWallCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
    m_RightPleuralToRespiratoryMuscle->GetNextCompliance().SetValue(ChestWallCompliance_L_Per_cmH2O, VolumePerPressureUnit::L_Per_cmH2O);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Return the appropriate compliance segment.
  ///
  //--------------------------------------------------------------------------------------------------
  SESegment* RespiratoryModel::GetSegement(const std::vector<SESegment*>& segments, double volume_L)
  {
    // Find and return the appropriate segment for the given volume
    for (SESegment* segment : segments)
    {
      if (segment->GetBeginVolume().IsInfinity())
      {
        if (segment->GetBeginVolume().IsPositive())
        {
          Error("The beginning respiratory compliance segment volume cannot be positive infinity");
          return nullptr;
        }

        if (volume_L <= segment->GetEndVolume(VolumeUnit::L))
          return segment;
      }

      if (segment->GetEndVolume().IsInfinity())
      {
        if (segment->GetBeginVolume().IsNegative())
        {
          Error("The ending respiratory compliance segment volume cannot be negative infinity");
          return nullptr;
        }

        if (volume_L >= segment->GetBeginVolume(VolumeUnit::L))
          return segment;
      }

      if (volume_L >= segment->GetBeginVolume(VolumeUnit::L) && volume_L <= segment->GetEndVolume(VolumeUnit::L))
        return segment;
    }

    Error("Could not find a respiratory compliance segment that bounds the provided volume " + std::to_string(volume_L) + "L");
    return nullptr;
  }


  std::string RespiratoryModel::GetCompartmentName(eLungCompartment m)
  {
    switch (m)
    {
    case eLungCompartment::LeftLung:
      return pulse::PulmonaryCompartment::LeftLung;
    case eLungCompartment::RightLung:
      return pulse::PulmonaryCompartment::RightLung;

    //case eLungCompartment::LeftInferiorLobe:
    //  return pulse::ExpandedPulmonaryCompartment::LeftInferiorLobeLung;
    //case eLungCompartment::LeftSuperiorLobe:
    //  return pulse::ExpandedPulmonaryCompartment::LeftSuperiorLobe;
    //case eLungCompartment::RightInferiorLobe:
    //  return pulse::ExpandedPulmonaryCompartment::RightInferiorLobe;
    //case eLungCompartment::RightMiddleLobe:
    //  return pulse::ExpandedPulmonaryCompartment::RightMiddleLobe;
    //case eLungCompartment::RightSuperiorLobe:
    //  return pulse::ExpandedPulmonaryCompartment::RightSuperiorLobe;

    default:
      Warning("Unsupported eLungCompartment");
    }
    return "";
  }

  eLungCompartment RespiratoryModel::GetLungCompartment(const std::string& cmpt)
  {
    if (cmpt == pulse::PulmonaryCompartment::LeftLung)
      return eLungCompartment::LeftLung;
    if (cmpt == pulse::PulmonaryCompartment::RightLung)
      return eLungCompartment::RightLung;

    //if (cmpt == pulse::ExpandedPulmonaryCompartment::LeftInferiorLobe)
    //  return eLungCompartment::LeftInferiorLobe;
    //if (cmpt == pulse::ExpandedPulmonaryCompartment::LeftSuperiorLobe)
    //  return eLungCompartment::LeftSuperiorLobe;
    //if (cmpt == pulse::ExpandedPulmonaryCompartment::RightInferiorLobe)
    //  return eLungCompartment::RightInferiorLobe;
    //if (cmpt == pulse::ExpandedPulmonaryCompartment::RightMiddleLobe)
    //  return eLungCompartment::RightMiddleLobe;
    //if (cmpt == pulse::ExpandedPulmonaryCompartment::RightSuperiorLobe)
    //  return eLungCompartment::RightSuperiorLobe;

    return (eLungCompartment)-1;
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Output data tracks for debugging purposes
  //--------------------------------------------------------------------------------------------------
  void RespiratoryModel::Debugging()
  {
    //m_data.GetDataTrack().Probe(RespirationCircuit);

    double leftAlveoliPressure = m_LeftAlveoli->GetPressure(PressureUnit::cmH2O);
    double leftAlveoliVolume = m_LeftAlveoli->GetVolume(VolumeUnit::L);
    double leftPleuralPressure = m_LeftPleuralNode->GetNextPressure(PressureUnit::cmH2O);
    double leftPleuralVolume = m_LeftPleuralNode->GetNextVolume(VolumeUnit::L);
    double leftFlow = m_LeftAlveolarDeadSpaceToLeftAlveoli->GetNextFlow(VolumePerTimeUnit::L_Per_s);
    double leftChestWallCompliance_L_Per_cmH2O = m_LeftPleuralToRespiratoryMuscle->GetNextCompliance(VolumePerPressureUnit::L_Per_cmH2O);
    double leftLungCompliance_L_Per_cmH2O = m_LeftAlveoliToLeftPleuralConnection->GetNextCompliance(VolumePerPressureUnit::L_Per_cmH2O);

    double rightAlveoliPressure = m_RightAlveoli->GetPressure(PressureUnit::cmH2O);
    double rightAlveoliVolume = m_RightAlveoli->GetVolume(VolumeUnit::L);
    double rightPleuralPressure = m_RightPleuralNode->GetNextPressure(PressureUnit::cmH2O);
    double rightPleuralVolume = m_RightPleuralNode->GetNextVolume(VolumeUnit::L);
    double rightFlow = m_RightAlveolarDeadSpaceToRightAlveoli->GetNextFlow(VolumePerTimeUnit::L_Per_s);
    double rightChestWallCompliance_L_Per_cmH2O = m_RightPleuralToRespiratoryMuscle->GetNextCompliance(VolumePerPressureUnit::L_Per_cmH2O);
    double rightLungCompliance_L_Per_cmH2O = m_RightAlveoliToRightPleuralConnection->GetNextCompliance(VolumePerPressureUnit::L_Per_cmH2O);

    m_data.GetDataTrack().Probe("leftAlveoliPressure", leftAlveoliPressure);
    m_data.GetDataTrack().Probe("leftAlveoliVolume", leftAlveoliVolume);
    m_data.GetDataTrack().Probe("leftPleuralPressure", leftPleuralPressure);
    m_data.GetDataTrack().Probe("leftPleuralVolume", leftPleuralVolume);
    m_data.GetDataTrack().Probe("leftFlow", leftFlow);
    m_data.GetDataTrack().Probe("leftChestWallCompliance_L_Per_cmH2O", leftChestWallCompliance_L_Per_cmH2O);
    m_data.GetDataTrack().Probe("leftLungCompliance_L_Per_cmH2O", leftLungCompliance_L_Per_cmH2O);

    m_data.GetDataTrack().Probe("rightAlveoliPressure", rightAlveoliPressure);
    m_data.GetDataTrack().Probe("rightAlveoliVolume", rightAlveoliVolume);
    m_data.GetDataTrack().Probe("rightPleuralPressure", rightPleuralPressure);
    m_data.GetDataTrack().Probe("rightPleuralVolume", rightPleuralVolume);
    m_data.GetDataTrack().Probe("rightFlow", rightFlow);
    m_data.GetDataTrack().Probe("rightChestWallCompliance_L_Per_cmH2O", rightChestWallCompliance_L_Per_cmH2O);
    m_data.GetDataTrack().Probe("rightLungCompliance_L_Per_cmH2O", rightLungCompliance_L_Per_cmH2O);

    double AmbientPresure = 1033.23; // = 1 atm
    m_data.GetDataTrack().Probe("LeftRelativePleuralPressure_cmH2O", leftPleuralPressure - AmbientPresure);
    m_data.GetDataTrack().Probe("RightRelativePleuralPressure_cmH2O", rightPleuralPressure - AmbientPresure);

    m_data.GetDataTrack().Probe("LeftRelativeAlveoliPressure_cmH2O", leftAlveoliPressure - AmbientPresure);
    m_data.GetDataTrack().Probe("RightRelativeAlveoliPressure_cmH2O", rightAlveoliPressure - AmbientPresure);

    double leftSideCompliance_L_Per_cmH2O = 1.0 / (1.0 / leftChestWallCompliance_L_Per_cmH2O + 1.0 / leftLungCompliance_L_Per_cmH2O);
    double rightSideCompliance_L_Per_cmH2O = 1.0 / (1.0 / rightChestWallCompliance_L_Per_cmH2O + 1.0 / rightLungCompliance_L_Per_cmH2O);

    double totalCompliance_L_Per_cmH2O = leftSideCompliance_L_Per_cmH2O + rightSideCompliance_L_Per_cmH2O;
    m_data.GetDataTrack().Probe("totalCompliance_L_Per_cmH2O", totalCompliance_L_Per_cmH2O);

    //Parameter table outputs for methodology report
    double leftAlveolarDeadSpace_L = 0.0;
    double rightAlveolarDeadSpace_L = 0.0;
    if (m_LeftAlveolarDeadSpace->HasVolume())
      leftAlveolarDeadSpace_L = m_LeftAlveolarDeadSpace->GetVolume(VolumeUnit::L);
    if (m_RightAlveolarDeadSpace->HasVolume())
      rightAlveolarDeadSpace_L = m_RightAlveolarDeadSpace->GetVolume(VolumeUnit::L);
    double totalAlveolarDeadSpace_L = leftAlveolarDeadSpace_L + rightAlveolarDeadSpace_L;
    m_data.GetDataTrack().Probe("totalAlveolarDeadSpace_L", totalAlveolarDeadSpace_L);

    double airwayResistance_cmH2O_s_Per_L = m_PharynxToCarina->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    m_data.GetDataTrack().Probe("airwayResistance_cmH2O_s_Per_L", airwayResistance_cmH2O_s_Per_L);

    double rightBronchiResistance_cmH2O_s_Per_L = m_CarinaToRightAnatomicDeadSpace->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    double leftBronchiResistance_cmH2O_s_Per_L = m_CarinaToLeftAnatomicDeadSpace->GetNextResistance(PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    double averageBronchiResistance_cmH2O_s_Per_L = (rightBronchiResistance_cmH2O_s_Per_L + leftBronchiResistance_cmH2O_s_Per_L) / 2.0;
    m_data.GetDataTrack().Probe("averageBronchiResistance_cmH2O_s_Per_L", averageBronchiResistance_cmH2O_s_Per_L);

    double averageLungCompliance_L_Per_cmH2O = (rightLungCompliance_L_Per_cmH2O + leftLungCompliance_L_Per_cmH2O) / 2.0;
    m_data.GetDataTrack().Probe("averageLungCompliance_L_Per_cmH2O", averageLungCompliance_L_Per_cmH2O);

    double inspiratoryExpiratoryRatio = m_InspiratoryRiseFraction / (1.0 - m_InspiratoryRiseFraction);
    m_data.GetDataTrack().Probe("inspiratoryExpiratoryRatio", inspiratoryExpiratoryRatio);

    double diffusionSurfaceArea_m2 = m_data.GetCurrentPatient().GetAlveoliSurfaceArea(AreaUnit::m2);
    m_data.GetDataTrack().Probe("diffusionSurfaceArea_m2", diffusionSurfaceArea_m2);

    double rightPulmonaryCapillaryResistance_mmHg_s_Per_mL = m_RightPulmonaryCapillary->GetNextResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    double leftPulmonaryCapillaryResistance_mmHg_s_Per_mL = m_LeftPulmonaryCapillary->GetNextResistance(PressureTimePerVolumeUnit::mmHg_s_Per_mL);
    double averagePulmonaryCapillaryResistance_mmHg_s_Per_mL = (rightPulmonaryCapillaryResistance_mmHg_s_Per_mL + leftPulmonaryCapillaryResistance_mmHg_s_Per_mL) / 2.0;
    m_data.GetDataTrack().Probe("averagePulmonaryCapillaryResistance_mmHg_s_Per_mL", averagePulmonaryCapillaryResistance_mmHg_s_Per_mL);
  }
END_NAMESPACE
