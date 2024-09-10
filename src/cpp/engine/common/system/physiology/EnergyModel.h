/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

#include "engine/common/system/Model.h"
#include "engine/common/system/Systems.h"
#include "cdm/circuit/thermal/SEThermalCircuitCalculator.h"

class SEPatient;
class SELiquidSubstanceQuantity;
class SEFluidCircuitPath;
class SEThermalCircuit;
class SEThermalCircuitNode;
class SEThermalCircuitPath;

namespace pulse
{
  class PBPhysiology;
  class EngineTest;
  /**
   * @brief @copydoc Physiology_EnergySystemData
   */  
  class PULSE_DECL EnergyModel : public EnergySystem, public Model
  {
    friend PBPhysiology;//friend the serialization class
    friend EngineTest;
  public:
    EnergyModel(Data& data);
    ~EnergyModel(void);

    void Clear() override;

    // Set members to a stable homeostatic state
    void Initialize() override;
    // Set pointers and other member varialbes common to both homeostatic initialization and loading a state
    void SetUp() override;

    void AtSteadyState() override;
    void PreProcess() override;
    void Process(bool solve_and_transport=true) override;
    void PostProcess(bool solve_and_transport=true) override;

  protected:
    void ComputeExposedModelParameters() override;

    // Preprocess Methods
    void CalculateMetabolicHeatGeneration();
    void Perspiration();
    void UpdateHeatResistance();
    // Actions/Conditions
    void Exercise();
    // These are both part of the consume meal condition.
    //void Dehydration(double time); // Need to revisit
    //void Starvation(double time); // Need to revisit
    void Dehydration();
    // Used in Reset & Starvation
    void CalculateBasalMetabolicRate();

    // Process Methods
    void CalculateVitalSigns();

    // Serializable member variables (Set in Initialize and in schema)
    double m_UsableEnergyStore_J;
    double m_PeakPowerEnergyStore_J;
    double m_MediumPowerEnergyStore_J;
    double m_EnduranceEnergyStore_J;
    SERunningAverage* m_BloodpH;
    SERunningAverage* m_BicarbonateMolarity_mmol_Per_L;

    // Stateless member variable (Set in SetUp())
    // Cmpts,Substance, and nodes
    SELiquidCompartment*        m_SkinExtracellular;
    SELiquidSubstanceQuantity*  m_AortaHCO3;
    //Nodes
    SEThermalCircuitNode*       m_coreNode;
    SEThermalCircuitNode*       m_skinNode;
    //Paths
    SEThermalCircuitPath*       m_groundToCorePath;
    SEThermalCircuitPath*       m_coreToGroundPath;
    SEThermalCircuitPath*       m_coreToSkinPath;
    SEFluidCircuitPath*         m_skinExtravascularToSweatingGroundPath;
    //Circuits
    SEThermalCircuit*           m_InternalTemperatureCircuit;
    SEThermalCircuit*           m_TemperatureCircuit;
    SEThermalCircuitCalculator* m_circuitCalculator;
    //Substances
    SESubstanceCompound*        m_Sweat;
  };
END_NAMESPACE

