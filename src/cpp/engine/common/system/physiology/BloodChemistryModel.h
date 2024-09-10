/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

#include "engine/common/system/Model.h"
#include "engine/common/system/Systems.h"

class SEArterialBloodGasTest;
class SECompleteBloodCount;
class SEComprehensiveMetabolicPanel;
class SELiquidCompartment;
class SELiquidSubstanceQuantity;

namespace pulse
{
  class PBPhysiology;
  class EngineTest;
  /**
   * @brief @copydoc Physiology_BloodChemistrySystemData
   * The Blood Chemistry System holds the system-level blood substance data that is computed on the compartment level by other systems.
   * @details
   * The goal of the blood chemistry system in the Pulse engine is complete analysis on the blood at any given point during the simulation.
   * The blood chemistry system houses all of the blood concentrations and compositions needed to assess a patient's health. This system is under development
   * and will be improved in future releases to include more substances that can provide clinician level details and assessments, such as a CBC and blood panel.
   */
  class PULSE_DECL BloodChemistryModel : public BloodChemistrySystem, public Model
  {
    friend PBPhysiology;//friend the serialization class
    friend EngineTest;
  public:
    BloodChemistryModel(Data& data);
    virtual ~BloodChemistryModel();

    void Clear() override;

    // Set members to a stable homeostatic state
    void Initialize() override;
    // Set pointers and other member varialbes common to both homeostatic initialization and loading a state
    void SetUp() override;

    void AtSteadyState() override;
    void PreProcess() override;
    void Process(bool solve_and_transport = true) override;
    void PostProcess(bool solve_and_transport = true) override;

    bool CalculateArterialBloodGasTest(SEArterialBloodGasTest& abg) const;
    bool CalculateCompleteBloodCount(SECompleteBloodCount& cbc) const;
    bool CalculateComprehensiveMetabolicPanel(SEComprehensiveMetabolicPanel& cmp) const;

  protected:
    void ComputeExposedModelParameters() override;

    void CheckBloodSubstanceLevels();
    // Serializable member variables (Set in Initialize and in schema)
    SERunningAverage*          m_ArterialOxygen_mmHg;
    SERunningAverage*          m_ArterialCarbonDioxide_mmHg;

    // Stateless member variable (Set in SetUp())
    double                     m_RedBloodCellVolume_mL;
    double                     m_HbPerRedBloodCell_ug_Per_ct;
    SELiquidCompartment*       m_Aorta;
    SELiquidSubstanceQuantity* m_AortaO2;
    SELiquidSubstanceQuantity* m_AortaCO2;
    SELiquidSubstanceQuantity* m_AortaHCO3;
    SELiquidSubstanceQuantity* m_BrainO2;
    SELiquidSubstanceQuantity* m_MyocardiumO2;
    SELiquidSubstanceQuantity* m_PulmonaryArteriesO2;
    SELiquidSubstanceQuantity* m_PulmonaryArteriesCO2;
    SELiquidSubstanceQuantity* m_PulmonaryVeinsO2;
    SELiquidSubstanceQuantity* m_PulmonaryVeinsCO2;
    SELiquidCompartment*       m_RightArm;
    SELiquidSubstanceQuantity* m_RightArmO2;
    SELiquidSubstanceQuantity* m_RightArmCO;
    SELiquidCompartment*       m_VenaCava;
    SELiquidSubstanceQuantity* m_VenaCavaO2;
    SELiquidSubstanceQuantity* m_VenaCavaCO2;
  };
END_NAMESPACE
