/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

#include "engine/common/system/Model.h"
#include "engine/common/system/Systems.h"
#include "cdm/circuit/fluid/SEFluidCircuitCalculator.h"
#include "cdm/compartment/fluid/SELiquidCompartmentGraph.h"

class SEPatient;
class SEHemorrhage;
class SELiquidCompartment;
class SEGasCompartment;
class SELiquidSubstanceQuantity;
class SELiquidCompartmentGraph;
class SELiquidCompartmentLink;
class SEFluidCircuit;
class SEFluidCircuitNode;
class SEFluidCircuitPath;
class SECardiovascularMechanicsModifiers;

namespace pulse
{
  class PBPhysiology;
  class EngineTest;
  /**
  * @brief 
  * The %Cardiovascular system utilizes circuit methodology to characterize the intravascular fluid dynamics throughout the body.
  * @copydoc Physiology_CardiovascularSystemData
  * @details
  * The %Cardiovascular system utilizes circuit methodology to evaluate the hemodynamics throughout the human body.
  * The cardiovascular system consists of a representation of the human vasculature. The heart model is the driving force that creates blood flow
  * through the cardiovascular system. The resulting blood flow is utilized by other physiologic systems as a means of transporting the oxygen, carbon dioxide
  * and other substances throughout the human body. The system may be modified to mimic real world insults and interventions. Due to its interconnectedness
  * with other systems, if alterations are made to the cardiovascular system then the feedback will be felt in the other physiologic systems.
  *
  */
  class PULSE_DECL CardiovascularModel : public CardiovascularSystem, public Model
  {
    friend PBPhysiology;//friend the serialization class
    friend EngineTest;
  public:
    CardiovascularModel(Data& data);
    virtual ~CardiovascularModel();

    void Clear() override;

    // Set members to a stable homeostatic state
    void Initialize() override;
    // Set pointers and other member varialbes common to both homeostatic initialization and loading a state
    void SetUp() override;

    void SetHeartRhythm(eHeartRhythm Rhythm) override;
    void SetHeartRhythm(eHeartRhythm Rhythm, bool force);

    void AtSteadyState() override;
    void PreProcess() override;
    void Process(bool solve_and_transport=true) override;
    void PostProcess(bool solve_and_transport=true) override;

  protected:
    void ComputeExposedModelParameters() override;
  
    // Initialize
    void TuneCircuit();
    /**/void TunePaths(double systemicResistanceScale, double systemicComplianceScale, double aortaResistanceScale, double aortaComplianceScale, double rightHeartResistanceScale, double venaCavaComplianceScale);
    /**/void TuneTissue(double time_s, DataTrack& track, std::ofstream& circuitFile);

    //Condition Methods
    void ChronicRenalStenosis();
    void ChronicAnemia();
    void ChronicHeartFailure();
    void ChronicPericardialEffusion();

    // PreProcess:
    void HeartDriver();
    /**/void AdjustVascularTone();
    /****/void MetabolicToneResponse();
    /**/void BeginDriverCycle();
    /****/void RecordAndResetCardiacCycle();//Could be called out of order by arrythma
    /**/void CalculateHeartElastance();
    void ProcessActions();
    //Action methods
    /**/void CPR();
    /****/double CalculateDepthForce(double compressionDepth_cm);
    /****/double ShapeCPRForce(double compressionForce_N);
    /****/void   ApplyCPRForce(double compressionForce_N);
    /**/void Arrhythmia();
    /**/void Hemorrhage();
    /**/void PericardialEffusion();
    /**/void PericardialEffusionPressureApplication();
    /**/void TraumaticBrainInjury();
    void UpdatePulmonaryCapillaries();
    void CalculateHemothorax();
    //Respiratory effects
    void CalculatePleuralCavityVenousEffects();

    // Process:
    void CalculateVitalSigns();

    // Serializable member variables (Set in Initialize and in schema)
    //Driver
    bool   m_StartSystole;
    bool   m_HeartFlowDetected;
    bool   m_FullyCompressedHeart;
    double m_StabilizedHeartRateBaseline_Per_min; // store for moving between arrhytmias
    double m_StabilizedMeanArterialPressureBaseline_mmHg; // store for moving between arrhytmias
    double m_CurrentDriverCycleTime_s;
    double m_DriverCyclePeriod_s;
    double m_LeftHeartElastanceModifier;// from Heart Failure and such
    double m_LeftHeartElastance_mmHg_Per_mL;
    double m_LeftHeartElastanceMax_mmHg_Per_mL;
    double m_LeftHeartElastanceMin_mmHg_Per_mL;
    double m_RightHeartElastance_mmHg_Per_mL;
    double m_RightHeartElastanceMax_mmHg_Per_mL;
    double m_RightHeartElastanceMin_mmHg_Per_mL;
    // Arrhythmia
    eSwitch m_EnableFeedbackAfterArrhythmiaTrasition;
    bool   m_StartCardiacArrest; // Can't go into cardiac arrest during the middle of a cycle
    bool   m_TransitionArrhythmia;
    double m_CardiacArrestVitalsUpdateTimer_s;
    // Transition Modifiers
    SETemporalInterpolator* m_HeartRateBaseline_Per_min;
    SETemporalInterpolator* m_HeartComplianceModifier;
    SETemporalInterpolator* m_AortaComplianceModifier;
    SETemporalInterpolator* m_VenaCavaComplianceModifier;
    SETemporalInterpolator* m_PulmonaryComplianceModifier;
    SETemporalInterpolator* m_SystemicVascularResistanceModifier;
    SETemporalInterpolator* m_SystemicVascularComplianceModifier;
    //CPR
    double m_CompressionFrequencyCurrentTime_s;
    double m_CompressionFrequencyDuration_s;
    double m_CompressionPeriod_s;
    double m_CompressionPeriodCurrentTime_s;
    // Vitals and Averages
    double m_CardiacCycleDiastolicVolume_mL; // Maximum left heart volume for the current cardiac cycle
    double m_CardiacCycleAortaPressureLow_mmHg; // The current low for this cycle - Reset at the start of systole
    double m_CardiacCycleAortaPressureHigh_mmHg; // The current high for this cycle - Reset at the start of systole
    double m_CardiacCycleLeftHeartPressureLow_mmHg; // The current low for this cycle - Reset at the start of systole
    double m_CardiacCycleLeftHeartPressureHigh_mmHg; // The current high for this cycle - Reset at the start of systole
    double m_CardiacCyclePulmonaryArteryPressureLow_mmHg;
    double m_CardiacCyclePulmonaryArteryPressureHigh_mmHg;
    double m_CardiacCycleRightHeartPressureLow_mmHg; // The current low for this cycle - Reset at the start of systole
    double m_CardiacCycleRightHeartPressureHigh_mmHg; // The current high for this cycle - Reset at the start of systole
    double m_LastCardiacCycleMeanArterialCO2PartialPressure_mmHg;
    double m_CardiacCycleStrokeVolume_mL; // Total volume of the left heart flow for the current cardiac cycle
    double m_PeripheralVolumeHigh_mL; // Max cycle volume in arms and legs
    double m_PeripheralVolumeLow_mL; // Min cycle volume in arms and legs
    //Needed for expanded pulmonary methodology
    std::vector<double> m_LeftCardiacCyclePerfusionVolumes_mL;
    std::vector<double> m_RightCardiacCyclePerfusionVolumes_mL;

    SERunningAverage* m_CardiacCycleArterialPressure_mmHg;
    SERunningAverage* m_CardiacCycleArterialCO2PartialPressure_mmHg;
    SERunningAverage* m_CardiacCyclePulmonaryCapillariesWedgePressure_mmHg;
    SERunningAverage* m_CardiacCyclePulmonaryCapillariesFlow_mL_Per_s;
    SERunningAverage* m_CardiacCyclePulmonaryShuntFlow_mL_Per_s;
    SERunningAverage* m_CardiacCyclePulmonaryArteryPressure_mmHg;
    SERunningAverage* m_CardiacCycleCentralVenousPressure_mmHg;
    SERunningAverage* m_CardiacCycleSkinFlow_mL_Per_s;


    // Stateless member variable (Set in SetUp())
    SECardiovascularMechanicsModifiers* m_MechanicsModifiers;
    // Hemorrhage
    struct HemorrhageTrack
    {
      SELiquidCompartment* Compartment=nullptr;
      std::map<SELiquidCompartment* , std::vector<SELiquidCompartmentLink*>> CmptHemorrhageLinks;
    };
    std::map<SEHemorrhage*, HemorrhageTrack*> m_HemorrhageTrack;
    SEFluidCircuitPath*                       m_InternalHemorrhageToAorta;

    double                           m_MAPCollapse_mmHg;
    double                           m_MinIndividialSystemicResistance_mmHg_s_Per_mL;
  
    SEFluidCircuitCalculator*        m_CircuitCalculator;
    SELiquidTransporter*             m_Transporter;

    SEFluidCircuit*                  m_CirculatoryCircuit;
    SELiquidCompartmentGraph*        m_CirculatoryGraph;

    SEFluidCircuitNode*              m_GroundNode;
    SEFluidCircuitNode*              m_AbdominalCavityNode;
    
    SEFluidCircuitPath*              m_AortaToBrain;
    SEFluidCircuitPath*              m_AortaToMyocardium;
    SEFluidCircuitPath*              m_AortaCompliancePath;
    SEFluidCircuitPath*              m_AortaResistancePath;
    SEFluidCircuitNode*              m_LeftPulmonaryVeinsNode;
    SEFluidCircuitNode*              m_RightPulmonaryVeinsNode;

    SEFluidCircuitPath*              m_VenaCavaCompliancePath;
    SEFluidCircuitPath*              m_VenaCavaResistancePath;

    SEFluidCircuitPath*              m_BrainToVenaCava;
    SEFluidCircuitPath*              m_MyocardiumToVenaCava;

    SEFluidCircuitPath*              m_LeftPulmonaryVeinsLeak;
    SEFluidCircuitPath*              m_RightPulmonaryVeinsLeak;

    SEFluidCircuitPath*              m_GndToPericardium;
    SEFluidCircuitPath*              m_PericardiumToGnd;

    SEFluidCircuitPath*              m_RightHeartToGnd;
    SEFluidCircuitPath*              m_RightHeartCompliancePath;
    SEFluidCircuitPath*              m_RightHeartResistancePath;

    SEFluidCircuitPath*              m_LeftHeartToGnd;
    SEFluidCircuitPath*              m_LeftHeartToAorta;
    SEFluidCircuitPath*              m_LeftHeartCompliancePath;

    SEFluidCircuitPath*              m_RightPulmonaryVenousReturnResistancePath;
    SEFluidCircuitPath*              m_LeftPulmonaryVenousReturnResistancePath;

    SEFluidCircuitPath*              m_BrainResistanceUpstreamPath;
    SEFluidCircuitPath*              m_BrainResistanceDownstreamPath;

    SEFluidCircuitPath*              m_GndToAbdominalCavity;
    SEFluidCircuitPath*              m_AbdominalCavityToGnd;

    SEFluidCircuitPath*              m_LeftRenalArteryPath;
    SEFluidCircuitPath*              m_RightRenalArteryPath;

    SELiquidCompartment*             m_Abdomen;
    SELiquidCompartment*             m_AbdominalCavity;
    SELiquidCompartment*             m_Aorta;
    SELiquidSubstanceQuantity*       m_AortaCO2;
    SELiquidCompartment*             m_Brain;
    SELiquidCompartment*             m_Ground;
    SELiquidCompartment*             m_LeftArm;
    SELiquidCompartment*             m_LeftHeart;
    SELiquidCompartment*             m_LeftLeg;
    SELiquidCompartment*             m_LeftPulmonaryCapillaries;
    SELiquidCompartment*             m_LeftPulmonaryArteries;
    SELiquidCompartment*             m_LeftPulmonaryVeins;
    SELiquidCompartment*             m_Pericardium;
    SELiquidCompartment*             m_RightArm;
    SELiquidCompartment*             m_RightHeart;
    SELiquidCompartment*             m_RightLeg;
    SELiquidCompartment*             m_RightPulmonaryCapillaries;
    SELiquidCompartment*             m_RightPulmonaryArteries;
    SELiquidCompartment*             m_RightPulmonaryVeins;
    SELiquidCompartment*             m_VenaCava;

    SEGasCompartment*                m_LeftPleuralCavity;
    SEGasCompartment*                m_RightPleuralCavity;
    SEGasCompartment*                m_PleuralCavity;
    SEGasCompartment*                m_Ambient;

    std::vector<SEFluidCircuitPath*> m_HeartCompliancePaths;
    std::vector<SEFluidCircuitPath*> m_AortaCompliancePaths;
    std::vector<SEFluidCircuitPath*> m_AortaResistancePaths;
    std::vector<SEFluidCircuitPath*> m_VenaCavaCompliancePaths;
    std::vector<SEFluidCircuitPath*> m_VenaCavaResistancePaths;
    std::vector<SEFluidCircuitPath*> m_PulmonaryCompliancePaths;
    std::vector<SEFluidCircuitPath*> m_PulmonaryResistancePaths;
    std::vector<SEFluidCircuitPath*> m_SystemicCompliancePaths;
    std::vector<SEFluidCircuitPath*> m_SystemicResistancePaths;
    std::vector<SEFluidCircuitPath*> m_MuscleResistancePaths;
    std::vector<SEFluidCircuitPath*> m_SkinPaths;

    //Needed for expanded pulmonary methodology
    std::vector<SEFluidCircuitPath*> m_LeftPulmonaryArteriesToVeins;
    std::vector<SEFluidCircuitPath*> m_LeftPulmonaryArteriesToCapillaries;
    std::vector<SEFluidCircuitPath*> m_RightPulmonaryArteriesToVeins;
    std::vector<SEFluidCircuitPath*> m_RightPulmonaryArteriesToCapillaries;
  };
END_NAMESPACE
