/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

#include "engine/common/system/Model.h"
#include "engine/common/system/Systems.h"

class SEFluidCircuitNode;
class SEFluidCircuitPath;
class SEFluidCircuit;

namespace pulse
{
  class PBPhysiology;
  class EngineTest;
  /**
   * @brief
   * The nervous class holds models of the peripheral and central nervous system. Currently, on the baroreceptor reflex is modeled.
   */
  class PULSE_DECL NervousModel : public NervousSystem, public Model
  {
    friend PBPhysiology;//friend the serialization class
    friend EngineTest;
  public:
    NervousModel(Data& data);
    virtual ~NervousModel();

    void Clear() override;

    // Set members to a stable homeostatic state
    void Initialize() override;
    // Set pointers and other member variables common to both homeostatic initialization and loading a state
    void SetUp() override;

    void AtSteadyState() override;
    void PreProcess() override;
    void Process(bool solve_and_transport = true) override;
    void PostProcess(bool solve_and_transport = true) override;


    virtual void SetBaroreceptorFeedback(eSwitch s) override;
    virtual void SetChemoreceptorFeedback(eSwitch s) override;

  protected:
    void ComputeExposedModelParameters() override;

    void BaroreceptorFeedback();
    void InitializeBaroreceptors();
    void CheckBrainStatus();
    void ChemoreceptorFeedback();
    void SetPupilEffects();
    void CerebralSpinalFluidUpdates();
    void SetBaselines();


    // Serializable member variables (Set in Initialize and in schema)
    bool   m_BaroreceptorFeedbackStatus;
    bool   m_BaroreceptorSaturationStatus;
    double m_ArterialOxygenBaseline_mmHg;
    double m_ArterialCarbonDioxideBaseline_mmHg;
    double m_BaroreceptorActiveTime_s;
    double m_BaroreceptorEffectivenessParameter;
    double m_BaroreceptorSaturationTime_s;
    double m_LastMeanArterialPressure_mmHg;
    double m_PreviousBloodVolume_mL;
    // Arrhythmia Support
    eHeartRhythm   m_PreviousHeartRhythm;
    //double m_BaroreceptorPauseTimer;
    //bool   m_BaroreceptorPauseTimerStatus;

    //SERIALIZE THESE AARON - should be system values
    double m_CSFAbsorptionRate_mLPermin;
    double m_CSFProductionRate_mlPermin;

    // Stateless member variable (Set in SetUp())
    // Configuration fractions representing the amount by which each quantity can change due to feedback;
    double m_NormalizedGammaHeartRate;
    double m_NormalizedGammaElastance;
    double m_NormalizedGammaCompliance;
    double m_NormalizedGammaResistance;
    double m_NormalizedAlphaHeartRate;
    double m_NormalizedAlphaElastance;
    double m_NormalizedAlphaCompliance;
    double m_NormalizedAlphaResistance;
    double m_NormalizedBetaHeartRate;

    SEFluidCircuitNode* m_IntracranialSpace;
    SEFluidCircuitPath* m_CSFProductAbsorptionPath;
    SEFluidCircuitPath* m_BrainVasculatureCompliancePath;
    SEFluidCircuitPath* m_BrainVasculatureResistancePath;
  };
END_NAMESPACE
