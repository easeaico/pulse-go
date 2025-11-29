/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

#include "engine/common/system/Model.h"
#include "engine/common/system/Systems.h"
#include "cdm/circuit/fluid/SEFluidCircuitCalculator.h"
#include "cdm/compartment/fluid/SEGasCompartmentGraph.h"
#include "cdm/compartment/fluid/SELiquidCompartmentGraph.h"

class SEPatient;
class SEPatientActionCollection;
class SEGasCompartment;
class SEGasSubstanceQuantity;
class SELiquidCompartment;
class SELiquidSubstanceQuantity;
class SEFluidCircuit;
class SEFluidCircuitNode;
class SEFluidCircuitPath;
class SEConsciousRespirationCommand;
class SERespiratoryMechanicsModifiers;

namespace pulse
{
  class PBPhysiology;
  class EngineTest;
  /**
  * @brief The %Respiratory System class handles the analysis and storage of
  * data related the respiratory physiology and pathology.
  * @details The goal of the Respiratory System class is to handle the analysis
  * and storage of data representing the ventilation and gas exchange of the human
  * respiratory system. The class handles the analysis of changes in the lungs volume
  * and pressure data during the flow and transfer of gases between the lungs, the
  * blood and the outside environment. The class updates the volume and volume fractions
  * of gases in the lungs, and ensures the integration and flow of data between the
  * respiratory system and the anesthesia machine during mechanical ventilation.
  */
  class PULSE_DECL RespiratoryModel : public RespiratorySystem, public Model
  {
    friend PBPhysiology;//friend the serialization class
    friend EngineTest;
  public:
    RespiratoryModel(Data& data);
    virtual ~RespiratoryModel();

    virtual void Clear() override;

    // Set members to a stable homeostatic state
    virtual void Initialize() override;
    // Set pointers and other member varialbes common to both homeostatic initialization and loading a state
    virtual void SetUp() override;

    virtual void AtSteadyState() override;
    virtual void PreProcess() override;
    virtual void Process(bool solve_and_transport=true) override;
    virtual void PostProcess(bool solve_and_transport=true) override;

  protected:
    void ComputeExposedModelParameters() override;/*TODO ExpandedLungs*/
    eLungCompartment GetLungCompartment(const std::string& cmpt);
    std::string GetCompartmentName(eLungCompartment m);

    //Tuning
    void TuneCircuit();

    //PreProcess
    void CalculateWork();
    void CalculateFatigue();
    void UpdateChestWallCompliances();/*TODO ExpandedLungs*/
    /**/SESegment* GetSegement(const std::vector<SESegment*>& segments, double volume_L);
    void UpdateAlveolarCompliances();
    void UpdateVolumes();
    void UpdateResistances();/*TODO ExpandedLungs*/
    /**/double CalculateSuctioningPattern(double baseResistance_cmH2O_s_Per_L, double flow_L_Per_s);
    void UpdateInspiratoryExpiratoryRatio();
    void UpdateDiffusion();
    // If Cardiovascular
    /**/void UpdatePulmonaryCapillary();
    /**/void UpdatePulmonaryShunt();
    // Aerosol Deposition and various Effects
    void ProcessAerosolSubstances();
    //Actions
    void Pneumothorax();
    void Hemothorax();
    void MechanicalVentilation();
    void SupplementalOxygen();
    // Driver
    void CalculateDriver();
    void ApplyDriver();
    /**/void SetBreathCycleFractions();
    /**/void ConsciousRespiration();
    /**/double VolumeToDriverPressure(double TargetVolume);/*TODO ExpandedLungs*/
    /**/void UpdateDriverPressure();
    /****/void CalculateMechanoreceptors();
    /**/void UpdateDriverPeriod();
    /**/double UpdateTargetVentilation(double targetAlveolarVentilation_L_Per_min);
    //Overrides
    void SetRespiratoryResistance();
    void SetRespiratoryCompliance();

    //Process
    void CalculateVitalSigns();
    void Debugging();

    // Serializable member variables (Set in Initialize and in schema)

    // CalculateVitalSigns()
    bool   m_BreathingCycle;
    bool   m_NotBreathing;
    double m_TopBreathTotalVolume_L;
    double m_TopCarinaO2;
    double m_TopBreathElapsedTime_min;
    double m_BottomBreathElapsedTime_min;
    double m_BottomBreathTotalVolume_L;
    double m_BottomBreathAlveoliPressure_cmH2O;
    double m_BottomBreathAirwayPressure_cmH2O;
    double m_PeakAlveolarPressure_cmH2O;
    double m_MaximalAlveolarPressure_cmH2O;
    double m_LastCardiacCycleBloodPH;
    SERunningAverage* m_BloodPHRunningAverage;
    SERunningAverage* m_MeanAirwayPressure_cmH2O;
    // Compliances
    double m_PreviousLeftLungPressureDiff_cmH2O;
    double m_PreviousRightLungPressureDiff_cmH2O;
    double m_PreviousLeftLungVolume_L;
    double m_PreviousRightLungVolume_L;
    double m_PreviousLeftPleuralPressureDiff_cmH2O;
    double m_PreviousRightPleuralPressureDiff_cmH2O;
    double m_PreviousLeftPleuralVolume_L;
    double m_PreviousRightPleuralVolume_L;

    // Expanded pulmonary methodology
    std::vector<double> m_AlveoliVolumeIncrement_L;
    std::vector<double> m_TopBreathAcinarZoneVolumes_L;
    std::vector<double> m_BottomBreathAcinarZoneVolumes_L;
    std::vector<double> m_PreviousShuntScalingFactor;

    // Respiratory Driver
    double m_ArterialO2PartialPressure_mmHg;
    double m_ArterialCO2PartialPressure_mmHg;
    double m_BreathingCycleTime_s;
    double m_DriverPressure_cmH2O;
    double m_ElapsedBreathingCycleTime_min;
    double m_IERatioScaleFactor;
    double m_PeakInspiratoryPressure_cmH2O;
    double m_PeakExpiratoryPressure_cmH2O;
    double m_PreviousTargetAlveolarVentilation_L_Per_min;
    double m_VentilationFrequency_Per_min;
    double m_VentilationPeriod_s;
    double m_VentilationToTidalVolumeSlope;
    SERunningAverage* m_ArterialO2RunningAverage_mmHg;
    SERunningAverage* m_ArterialCO2RunningAverage_mmHg;
    SERunningAverage* m_MixedExpiredCO2RunningAverage_mmHg;

    // Muscle Pressure Waveform
    double m_ExpiratoryHoldFraction;
    double m_ExpiratoryReleaseFraction;
    double m_ExpiratoryRiseFraction;
    double m_InspiratoryHoldFraction;
    double m_InspiratoryReleaseFraction;
    double m_InspiratoryRiseFraction;
    double m_InspiratoryToExpiratoryPauseFraction;
    double m_ResidueFraction;
    double m_PreviousDyspneaSeverity;
    double m_MechanoreceptorsDyspneaFactor;
    double m_AppliedMechanoreceptorsDyspneaFactor;

    // Positive Pressure Ventilation
    bool m_PositivePressureVentilation;

    // Conscious Respiration
    bool m_ActiveConsciousRespirationCommand;

    // Disease States
    double m_LeftAlveoliDecrease_L;
    double m_RightAlveoliDecrease_L;

    // Overrides
    double m_RespiratoryResistanceOverride_cmH2O_s_Per_L;
    double m_RespiratoryComplianceOverride_L_Per_cmH2O;

    // Stateless member variable (Set in SetUp())
    SERespiratoryMechanicsModifiers* m_MechanicsModifiers;
    // Respiratory Driver
    double m_MaxDriverPressure_cmH2O;
    // Configuration parameters
    double m_CentralControlGainConstant;
    double m_DefaultOpenResistance_cmH2O_s_Per_L;
    double m_DefaultClosedResistance_cmH2O_s_Per_L;
    double m_PeripheralControlGainConstant;
    double m_RespOpenResistance_cmH2O_s_Per_L;
    double m_RespClosedResistance_cmH2O_s_Per_L;
    double m_VentilationTidalVolumeIntercept;
    double m_VentilatoryOcclusionPressure_cmH2O;
    double m_MinimumAllowableTidalVolume_L;
    double m_MinimumAllowableInpiratoryAndExpiratoryPeriod_s;
    // State between functions (i.e. shared between methods in preprocess, set to a default value at the start of preprocess)
    double m_AverageLocalTissueBronchodilationEffects;

    // Patient
    SEPatientActionCollection* m_PatientActions;

    // Variables independent of how the lungs are modelled
    SEFluidCircuit* m_RespiratoryCircuit;
    SEFluidCircuitCalculator* m_Calculator;
    SEGasTransporter* m_GasTransporter;
    SELiquidTransporter* m_AerosolTransporter;
    // Nodes
    SEFluidCircuitNode* m_AirwayNode;
    SEFluidCircuitNode* m_RespiratoryMuscleNode;
    SEFluidCircuitNode* m_LeftPleuralNode;
    SEFluidCircuitNode* m_RightPleuralNode;
    SEFluidCircuitNode* m_AmbientNode;
    SEFluidCircuitNode* m_StomachNode;
    // Paths
    SEFluidCircuitPath* m_LeftPleuralToRespiratoryMuscle;
    SEFluidCircuitPath* m_RightPleuralToRespiratoryMuscle;
    SEFluidCircuitPath* m_DriverPressurePath;
    SEFluidCircuitPath* m_AirwayToPharynx;
    SEFluidCircuitPath* m_PharynxToEnvironment;
    SEFluidCircuitPath* m_PharynxToCarina;
    SEFluidCircuitPath* m_AirwayToStomach;
    // Compartments
    SEGasCompartment* m_Environment;
    SEGasCompartment* m_Carina;
    SEGasSubstanceQuantity* m_CarinaO2;
    SEGasCompartment* m_Lungs;
    SEGasCompartment* m_LeftLung;
    SEGasCompartment* m_RightLung;
    SEGasCompartment* m_PleuralCavity;
    SEGasCompartment* m_LeftPleuralCavity;
    SEGasCompartment* m_RightPleuralCavity;
    SEGasCompartment* m_AnatomicDeadSpace;
    SEGasCompartment* m_AlveolarDeadSpace;
    SEGasCompartment* m_RightAlveolarDeadSpace;
    SEGasCompartment* m_LeftAlveolarDeadSpace;
    SEGasCompartment* m_Alveoli;
    SEGasCompartment* m_LeftAlveoli;
    SEGasCompartment* m_RightAlveoli;
    SEGasSubstanceQuantity* m_LeftAlveoliO2;
    SEGasSubstanceQuantity* m_RightAlveoliO2;
    // Mechanical Ventilation
    SEGasCompartment* m_MechanicalVentilationConnection;
    SEFluidCircuitNode* m_MechanicalVentilationDeadSpace;
    SELiquidCompartment* m_MechanicalVentilationAerosolConnection;
    SEFluidCircuitPath* m_ConnectionToDeadSpace;
    SEFluidCircuitPath* m_GroundToConnection;
    // Cardiovascular
    SELiquidSubstanceQuantity* m_AortaO2;
    SELiquidSubstanceQuantity* m_AortaCO2;
    // Substance
    SESubstance* m_Oversedation;

    ///////////////////////////////////////////
    // Lung Configuration Specific Variables //
    ///////////////////////////////////////////

    // These are the lung components we will iterate on for actions
    struct LungComponent
    {
      eSide                    Side;
      SEFluidCircuitNode*      AlveoliNode;
      SEFluidCircuitNode*      DeadSpaceNode;
      SEFluidCircuitPath*      ResistancePath;
      SEFluidCircuitPath*      CompliancePath;
      SEFluidCircuitPath*      ShuntPath;
      SEFluidCircuitPath*      ArteriesPath;
      SEFluidCircuitPath*      VeinsPath;
      SEGasCompartment*        AlveoliCompartment;
      SELiquidCompartment*     CapillaryCompartment;
      SELiquidCompartmentLink* ShuntLink;
      SELiquidCompartmentLink* ArteriesLink;
      SELiquidCompartmentLink* VeinsLink;
    };
    std::map<eLungCompartment, LungComponent> m_LungComponents;
    // Paths
    SEFluidCircuitPath* m_CarinaToRightLung;
    SEFluidCircuitPath* m_CarinaToLeftLung;

    ///////////////////////////////////////////////////
    // Currently only for default lung configuration //
    ///////////////////////////////////////////////////

    // Aerosol
    SELiquidCompartment* m_AerosolAirway;
    SELiquidCompartment* m_AerosolCarina;
    SELiquidCompartment* m_AerosolLeftAnatomicDeadSpace;
    SELiquidCompartment* m_AerosolLeftAlveolarDeadSpace;
    SELiquidCompartment* m_AerosolLeftAlveoli;
    SELiquidCompartment* m_AerosolRightAnatomicDeadSpace;
    SELiquidCompartment* m_AerosolRightAlveolarDeadSpace;
    SELiquidCompartment* m_AerosolRightAlveoli;
    std::vector<SELiquidCompartment*> m_AerosolEffects;
    SELiquidCompartment* m_LeftLungExtravascular;
    SELiquidCompartment* m_RightLungExtravascular;

    SEFluidCircuitPath* m_CarinaToLeftAnatomicDeadSpace;
    SEFluidCircuitPath* m_CarinaToRightAnatomicDeadSpace;
    SEFluidCircuitPath* m_LeftAnatomicDeadSpaceToLeftAlveolarDeadSpace;
    SEFluidCircuitPath* m_RightAnatomicDeadSpaceToRightAlveolarDeadSpace;
    SEFluidCircuitPath* m_LeftAlveolarDeadSpaceToLeftAlveoli;
    SEFluidCircuitPath* m_RightAlveolarDeadSpaceToRightAlveoli;

    SEFluidCircuitPath* m_EnvironmentToLeftChestLeak;
    SEFluidCircuitPath* m_EnvironmentToRightChestLeak;
    SEFluidCircuitPath* m_LeftAlveoliLeakToLeftPleural;
    SEFluidCircuitPath* m_RightAlveoliLeakToRightPleural;
    SEFluidCircuitPath* m_LeftNeedleToLeftPleural;
    SEFluidCircuitPath* m_RightNeedleToRightPleural;

    SEFluidCircuitPath* m_LeftCardiovascularLeak;
    SEFluidCircuitPath* m_RightCardiovascularLeak;
    SEFluidCircuitPath* m_LeftRespirtoryLeak;
    SEFluidCircuitPath* m_RightRespirtoryLeak;

    SEFluidCircuitPath* m_LeftAlveoliToLeftPleuralConnection;
    SEFluidCircuitPath* m_RightAlveoliToRightPleuralConnection;
  };
END_NAMESPACE
