/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

#include "engine/CommonDefs.h"
#include "cdm/circuit/SECircuitManager.h"
class SEFluidCircuit;
class SEThermalCircuit;

namespace pulse
{
  class Data;
  /**
  * @brief Manages all circuits associated with all %Pulse systems/equipement
  */
  class PULSE_DECL CircuitManager : public SECircuitManager
  {
  public:

    CircuitManager(Data& data);
    virtual ~CircuitManager();

    void Clear() override;
    void StateChange() override;

    void SetReadOnlyFluid(bool b) override;

    // Active methods return the circuit configuration to be calculated
    virtual SEFluidCircuit& GetActiveCardiovascularCircuit();
    virtual SEFluidCircuit& GetCardiovascularCircuit();
    virtual SEFluidCircuit& GetRenalCircuit();

    virtual SEFluidCircuit& GetActiveRespiratoryCircuit();
    virtual SEFluidCircuit& GetRespiratoryCircuit();
    virtual SEFluidCircuit& GetRespiratoryAndAnesthesiaMachineCircuit();
    virtual SEFluidCircuit& GetRespiratoryAndBagValveMaskCircuit();
    virtual SEFluidCircuit& GetRespiratoryAndInhalerCircuit();
    virtual SEFluidCircuit& GetRespiratoryAndNasalCannulaCircuit();
    virtual SEFluidCircuit& GetRespiratoryAndSimpleMaskCircuit();
    virtual SEFluidCircuit& GetRespiratoryAndNonRebreatherMaskCircuit();
    virtual SEFluidCircuit& GetRespiratoryAndMechanicalVentilationCircuit();
    virtual SEFluidCircuit& GetRespiratoryAndMechanicalVentilatorCircuit();

    virtual SEThermalCircuit& GetTemperatureCircuit();
    virtual SEThermalCircuit& GetExternalTemperatureCircuit();
    virtual SEThermalCircuit& GetInternalTemperatureCircuit();

    virtual SEFluidCircuit& GetAnesthesiaMachineCircuit();
    virtual SEFluidCircuit& GetBagValveMaskCircuit();
    virtual SEFluidCircuit& GetMechanicalVentilatorCircuit();

  protected:

    Data& m_data;

    // Note, I am not making a tissue or inhaler standalone circuit
    // this are super simple circuits and can't be tested by themselves 

    // The combined cardiovascular circuit only needs to be built once,
    // but we can build it different ways, with or without renal and tissue subcircuits.
    // It depends on how the engine is configured, but this configuration does not change
    // once it's set. Tests can also configure how it's built and test appropriately.

    SEFluidCircuit* m_CombinedCardiovascularCircuit;

    SEFluidCircuit* m_CardiovascularCircuit;
    SEFluidCircuit* m_RenalCircuit;

    SEFluidCircuit* m_RespiratoryCircuit;

    SEFluidCircuit* m_AnesthesiaMachineCircuit;
    SEFluidCircuit* m_BagValveMaskCircuit;
    SEFluidCircuit* m_MechanicalVentilatorCircuit;

    // Based on if equipment is hooked up, we have to build out the combination of 
    // these 2 circuits and graphs as we don't want to dynamically modify circuits
    // It's quicker and easier to test these combiniation circuits
    SEFluidCircuit* m_CombinedRespiratoryAnesthesiaCircuit;
    SEFluidCircuit* m_CombinedRespiratoryBagValveMaskCircuit;
    SEFluidCircuit* m_CombinedRespiratoryInhalerCircuit;
    SEFluidCircuit* m_CombinedRespiratoryNasalCannulaCircuit;
    SEFluidCircuit* m_CombinedRespiratorySimpleMaskCircuit;
    SEFluidCircuit* m_CombinedRespiratoryNonRebreatherMaskCircuit;
    SEFluidCircuit* m_CombinedRespiratoryMechanicalVentilationCircuit;
    SEFluidCircuit* m_CombinedRespiratoryMechanicalVentilatorCircuit;

    SEThermalCircuit* m_TemperatureCircuit;
    SEThermalCircuit* m_InternalTemperatureCircuit;// Attempt at circuit independence
    SEThermalCircuit* m_ExternalTemperatureCircuit;// Attempt at circuit independence
  };
}

///////////////////
// Circuit Enums //
///////////////////

namespace pulse
{
  class Circuits
  {
  public:
    DEFINE_STATIC_STRING(FullCardiovascular);
    DEFINE_STATIC_STRING(Cardiovascular);
    DEFINE_STATIC_STRING(Renal);
    DEFINE_STATIC_STRING(Respiratory);
    // Equipment
    DEFINE_STATIC_STRING(AnesthesiaMachine);
    DEFINE_STATIC_STRING(BagValveMask);
    DEFINE_STATIC_STRING(MechanicalVentilator);
    DEFINE_STATIC_STRING(RespiratoryAnesthesia);
    DEFINE_STATIC_STRING(RespiratoryBagValveMask);
    DEFINE_STATIC_STRING(RespiratoryInhaler);
    DEFINE_STATIC_STRING(RespiratoryMechanicalVentilation);
    DEFINE_STATIC_STRING(RespiratoryMechanicalVentilator);
    DEFINE_STATIC_STRING(RespiratoryNasalCannula);
    DEFINE_STATIC_STRING(RespiratoryNonRebreatherMask);
    DEFINE_STATIC_STRING(RespiratorySimpleMask);
    // Environment
    DEFINE_STATIC_STRING(Temperature);
    DEFINE_STATIC_STRING(InternalTemperature);
    DEFINE_STATIC_STRING(ExternalTemperature);
  };

  ///////////////////////////
  // Chyme Circuit Enums //
  ///////////////////////////

  class ChymeNode
  {
  public:
    DEFINE_STATIC_STRING(SmallIntestineC1);
  };

  class ChymePath
  {
  public:
    DEFINE_STATIC_STRING(SmallIntestineC1ToSmallIntestine1);
    DEFINE_STATIC_STRING(GroundToSmallIntestineC1);
    DEFINE_STATIC_STRING(GutT1ToGround);
  };

  ////////////////////////////////
  // Respiratoruy Circuit Enums //
  ////////////////////////////////

  class RespiratoryNode
  {
  public:
    DEFINE_STATIC_STRING(Carina);

    DEFINE_STATIC_STRING(LeftAlveoli);
    DEFINE_STATIC_STRING(LeftAlveoliLeak);
    DEFINE_STATIC_STRING(LeftAnatomicDeadSpace);
    DEFINE_STATIC_STRING(LeftAlveolarDeadSpace);
    DEFINE_STATIC_STRING(LeftChestLeak);
    DEFINE_STATIC_STRING(LeftPleural);
    DEFINE_STATIC_STRING(LeftPleuralConnection);
    DEFINE_STATIC_STRING(LeftNeedle);

    DEFINE_STATIC_STRING(Airway);
    DEFINE_STATIC_STRING(Pharynx);

    DEFINE_STATIC_STRING(RightAlveoli);
    DEFINE_STATIC_STRING(RightAlveoliLeak);
    DEFINE_STATIC_STRING(RightAnatomicDeadSpace);
    DEFINE_STATIC_STRING(RightAlveolarDeadSpace);
    DEFINE_STATIC_STRING(RightChestLeak);
    DEFINE_STATIC_STRING(RightPleural);
    DEFINE_STATIC_STRING(RightPleuralConnection);
    DEFINE_STATIC_STRING(RightNeedle);

    DEFINE_STATIC_STRING(Stomach);
    DEFINE_STATIC_STRING(RespiratoryMuscle);

  };

  class RespiratoryPath
  {
  public:
    DEFINE_STATIC_STRING(CarinaToLeftAnatomicDeadSpace);
    DEFINE_STATIC_STRING(CarinaToRightAnatomicDeadSpace);

    DEFINE_STATIC_STRING(EnvironmentToLeftChestLeak);
    DEFINE_STATIC_STRING(EnvironmentToAirway);
    DEFINE_STATIC_STRING(EnvironmentToRightChestLeak);

    DEFINE_STATIC_STRING(LeftAlveoliLeakToLeftPleural);
    DEFINE_STATIC_STRING(LeftAlveoliToLeftAlveoliLeak);
    DEFINE_STATIC_STRING(LeftAlveoliToLeftPleuralConnection);
    DEFINE_STATIC_STRING(LeftPleuralConnectionToLeftPleural);
    DEFINE_STATIC_STRING(LeftAnatomicDeadSpaceToLeftAlveolarDeadSpace);
    DEFINE_STATIC_STRING(LeftAlveolarDeadSpaceToLeftAlveoli);
    DEFINE_STATIC_STRING(LeftChestLeakToLeftPleural);
    DEFINE_STATIC_STRING(LeftNeedleToLeftPleural);
    DEFINE_STATIC_STRING(EnvironmentToLeftNeedle);
    DEFINE_STATIC_STRING(EnvironmentToLeftPleural);
    DEFINE_STATIC_STRING(LeftPleuralToRespiratoryMuscle);

    DEFINE_STATIC_STRING(AirwayToPharynx);
    DEFINE_STATIC_STRING(AirwayToStomach);
    DEFINE_STATIC_STRING(PharynxToCarina);
    DEFINE_STATIC_STRING(PharynxToEnvironment);

    DEFINE_STATIC_STRING(RightAlveoliLeakToRightPleural);
    DEFINE_STATIC_STRING(RightAlveoliToRightAlveoliLeak);
    DEFINE_STATIC_STRING(RightAlveoliToRightPleuralConnection);
    DEFINE_STATIC_STRING(RightPleuralConnectionToRightPleural);
    DEFINE_STATIC_STRING(RightAnatomicDeadSpaceToRightAlveolarDeadSpace);
    DEFINE_STATIC_STRING(RightAlveolarDeadSpaceToRightAlveoli);
    DEFINE_STATIC_STRING(RightChestLeakToRightPleural);
    DEFINE_STATIC_STRING(RightNeedleToRightPleural);
    DEFINE_STATIC_STRING(EnvironmentToRightNeedle);
    DEFINE_STATIC_STRING(EnvironmentToRightPleural);
    DEFINE_STATIC_STRING(RightPleuralToRespiratoryMuscle);

    DEFINE_STATIC_STRING(EnvironmentToRespiratoryMuscle);
    DEFINE_STATIC_STRING(StomachToEnvironment);
  };

  class ExpandedRespiratoryNode
  {
  public:
    DEFINE_STATIC_STRING(RightMainBronchus);
    DEFINE_STATIC_STRING(LeftMainBronchus);
    DEFINE_STATIC_STRING(RightIntermediateBronchus);
    DEFINE_STATIC_STRING(RightSuperiorLobarBronchus);
    DEFINE_STATIC_STRING(RightMiddleLobarBronchus);
    DEFINE_STATIC_STRING(RightInferiorLobarBronchus1);
    DEFINE_STATIC_STRING(LeftInferiorLobarBronchus1);
    DEFINE_STATIC_STRING(LeftSuperiorLobarBronchus);
    DEFINE_STATIC_STRING(RightInferiorLobarBronchus2);
    DEFINE_STATIC_STRING(LeftInferiorLobarBronchus2);
    DEFINE_STATIC_STRING(RightSuperiorApicoposteriorBronchus);
    DEFINE_STATIC_STRING(RightInferiorLobarBronchus3);
    DEFINE_STATIC_STRING(LeftInferiorLobarBronchus3);
    DEFINE_STATIC_STRING(LeftLingularBonchus);
    DEFINE_STATIC_STRING(LeftSuperiorApicoposteriorBronchus);
    DEFINE_STATIC_STRING(RightInferiorLobarBronchus4);

    DEFINE_STATIC_STRING(RightSuperiorLobeApicalBronchiole);
    DEFINE_STATIC_STRING(RightSuperiorLobePosteriorBronchiole);
    DEFINE_STATIC_STRING(RightSuperiorLobeAnteriorBronchiole);
    DEFINE_STATIC_STRING(RightMiddleLobeLateralBronchiole);
    DEFINE_STATIC_STRING(RightMiddleLobeMedialBronchiole);
    DEFINE_STATIC_STRING(RightInferiorLobeSuperiorBronchiole);
    DEFINE_STATIC_STRING(RightInferiorLobeMedialBasalBronchiole);
    DEFINE_STATIC_STRING(RightInferiorLobeAnteriorBasalBronchiole);
    DEFINE_STATIC_STRING(RightInferiorLobeLateralBasalBronchiole);
    DEFINE_STATIC_STRING(RightInferiorLobePosteriorBasalBronchiole);

    DEFINE_STATIC_STRING(LeftInferiorLobePosteriorBasalBronchiole);
    DEFINE_STATIC_STRING(LeftInferiorLobeLateralBasalBronchiole);
    DEFINE_STATIC_STRING(LeftInferiorLobeAnteromedialBasalBronchiole);
    DEFINE_STATIC_STRING(LeftInferiorLobeSuperiorBronchiole);
    DEFINE_STATIC_STRING(LeftSuperiorLobeInferiorLingulaBronchiole);
    DEFINE_STATIC_STRING(LeftSuperiorLobeSuperiorLingulaBronchiole);
    DEFINE_STATIC_STRING(LeftSuperiorLobeAnteriorBronchiole);
    DEFINE_STATIC_STRING(LeftSuperiorLobeApicoposteriorBronchiole);

    DEFINE_STATIC_STRING(RightSuperiorLobeApicalAlveolarDeadSpace);
    DEFINE_STATIC_STRING(RightSuperiorLobePosteriorAlveolarDeadSpace);
    DEFINE_STATIC_STRING(RightSuperiorLobeAnteriorAlveolarDeadSpace);
    DEFINE_STATIC_STRING(RightMiddleLobeLateralAlveolarDeadSpace);
    DEFINE_STATIC_STRING(RightMiddleLobeMedialAlveolarDeadSpace);
    DEFINE_STATIC_STRING(RightInferiorLobeSuperiorAlveolarDeadSpace);
    DEFINE_STATIC_STRING(RightInferiorLobeMedialBasalAlveolarDeadSpace);
    DEFINE_STATIC_STRING(RightInferiorLobeAnteriorBasalAlveolarDeadSpace);
    DEFINE_STATIC_STRING(RightInferiorLobeLateralBasalAlveolarDeadSpace);
    DEFINE_STATIC_STRING(RightInferiorLobePosteriorBasalAlveolarDeadSpace);

    DEFINE_STATIC_STRING(LeftInferiorLobePosteriorBasalAlveolarDeadSpace);
    DEFINE_STATIC_STRING(LeftInferiorLobeLateralBasalAlveolarDeadSpace);
    DEFINE_STATIC_STRING(LeftInferiorLobeAnteromedialBasalAlveolarDeadSpace);
    DEFINE_STATIC_STRING(LeftInferiorLobeSuperiorAlveolarDeadSpace);
    DEFINE_STATIC_STRING(LeftSuperiorLobeInferiorLingulaAlveolarDeadSpace);
    DEFINE_STATIC_STRING(LeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace);
    DEFINE_STATIC_STRING(LeftSuperiorLobeAnteriorAlveolarDeadSpace);
    DEFINE_STATIC_STRING(LeftSuperiorLobeApicoposteriorAlveolarDeadSpace);

    DEFINE_STATIC_STRING(RightSuperiorLobeApicalAlveoli);
    DEFINE_STATIC_STRING(RightSuperiorLobePosteriorAlveoli);
    DEFINE_STATIC_STRING(RightSuperiorLobeAnteriorAlveoli);
    DEFINE_STATIC_STRING(RightMiddleLobeLateralAlveoli);
    DEFINE_STATIC_STRING(RightMiddleLobeMedialAlveoli);
    DEFINE_STATIC_STRING(RightInferiorLobeSuperiorAlveoli);
    DEFINE_STATIC_STRING(RightInferiorLobeMedialBasalAlveoli);
    DEFINE_STATIC_STRING(RightInferiorLobeAnteriorBasalAlveoli);
    DEFINE_STATIC_STRING(RightInferiorLobeLateralBasalAlveoli);
    DEFINE_STATIC_STRING(RightInferiorLobePosteriorBasalAlveoli);

    DEFINE_STATIC_STRING(LeftInferiorLobePosteriorBasalAlveoli);
    DEFINE_STATIC_STRING(LeftInferiorLobeLateralBasalAlveoli);
    DEFINE_STATIC_STRING(LeftInferiorLobeAnteromedialBasalAlveoli);
    DEFINE_STATIC_STRING(LeftInferiorLobeSuperiorAlveoli);
    DEFINE_STATIC_STRING(LeftSuperiorLobeInferiorLingulaAlveoli);
    DEFINE_STATIC_STRING(LeftSuperiorLobeSuperiorLingulaAlveoli);
    DEFINE_STATIC_STRING(LeftSuperiorLobeAnteriorAlveoli);
    DEFINE_STATIC_STRING(LeftSuperiorLobeApicoposteriorAlveoli);
  };

  class ExpandedRespiratoryPath
  {
  public:
    DEFINE_STATIC_STRING(CarinaToRightMainBronchus);
    DEFINE_STATIC_STRING(RightMainBronchusToRightIntermediateBronchus);
    DEFINE_STATIC_STRING(RightMainBronchusToRightSuperiorLobarBronchus);
    DEFINE_STATIC_STRING(RightIntermediateBronchusToRightMiddleLobarBronchus);
    DEFINE_STATIC_STRING(RightIntermediateBronchusToRightInferiorLobarBronchus1);
    DEFINE_STATIC_STRING(RightInferiorLobarBronchus1ToRightInferiorLobarBronchus2);
    DEFINE_STATIC_STRING(RightSuperiorLobarBronchusToRightSuperiorApicoposteriorBronchus);
    DEFINE_STATIC_STRING(RightInferiorLobarBronchus2ToRightInferiorLobarBronchus3);
    DEFINE_STATIC_STRING(RightInferiorLobarBronchus2ToRightInferiorLobarBronchus4);

    DEFINE_STATIC_STRING(CarinaToLeftMainBronchus);
    DEFINE_STATIC_STRING(LeftMainBronchusToLeftInferiorLobarBronchus1);
    DEFINE_STATIC_STRING(LeftMainBronchusToLeftSuperiorLobarBronchus);
    DEFINE_STATIC_STRING(LeftInferiorLobarBronchus1ToLeftInferiorLobarBronchus2);
    DEFINE_STATIC_STRING(LeftInferiorLobarBronchus2ToLeftInferiorLobarBronchus3);
    DEFINE_STATIC_STRING(LeftSuperiorLobarBronchusToLeftLingularBonchus);
    DEFINE_STATIC_STRING(LeftSuperiorLobarBronchusToLeftSuperiorApicoposteriorBronchus);

    DEFINE_STATIC_STRING(RightSuperiorApicoposteriorBronchusToRightSuperiorLobeApicalBronchiole);
    DEFINE_STATIC_STRING(RightSuperiorApicoposteriorBronchusToRightSuperiorLobePosteriorBronchiole);
    DEFINE_STATIC_STRING(RightSuperiorLobarBronchusToRightSuperiorLobeAnteriorBronchiole);
    DEFINE_STATIC_STRING(RightMiddleLobarBronchusToRightMiddleLobeLateralBronchiole);
    DEFINE_STATIC_STRING(RightMiddleLobarBronchusToRightMiddleLobeMedialBronchiole);
    DEFINE_STATIC_STRING(RightInferiorLobarBronchus1ToRightInferiorLobeSuperiorBronchiole);
    DEFINE_STATIC_STRING(RightInferiorLobarBronchus2ToRightInferiorLobeMedialBasalBronchiole);
    DEFINE_STATIC_STRING(RightInferiorLobarBronchus3ToRightInferiorLobeAnteriorBasalBronchiole);
    DEFINE_STATIC_STRING(RightInferiorLobarBronchus4ToRightInferiorLobeLateralBasalBronchiole);
    DEFINE_STATIC_STRING(RightInferiorLobarBronchus4ToRightInferiorLobePosteriorBasalBronchiole);

    DEFINE_STATIC_STRING(LeftInferiorLobarBronchus3ToLeftInferiorLobeLateralBasalBronchiole);
    DEFINE_STATIC_STRING(LeftInferiorLobarBronchus3ToLeftInferiorLobePosteriorBasalBronchiole);
    DEFINE_STATIC_STRING(LeftInferiorLobarBronchus2ToLeftInferiorLobeAnteromedialBasalBronchiole);
    DEFINE_STATIC_STRING(LeftInferiorLobarBronchus1ToLeftInferiorLobeSuperiorBronchiole);
    DEFINE_STATIC_STRING(LeftLingularBonchusToLeftSuperiorLobeInferiorLingulaBronchiole);
    DEFINE_STATIC_STRING(LeftLingularBonchusToLeftSuperiorLobeSuperiorLingulaBronchiole);
    DEFINE_STATIC_STRING(LeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeAnteriorBronchiole);
    DEFINE_STATIC_STRING(LeftSuperiorApicoposteriorBronchusToLeftSuperiorLobeApicoposteriorBronchiole);

    DEFINE_STATIC_STRING(RightSuperiorLobeApicalBronchioleToRightSuperiorLobeApicalAlveolarDeadSpace);
    DEFINE_STATIC_STRING(RightSuperiorLobePosteriorBronchioleToRightSuperiorLobePosteriorAlveolarDeadSpace);
    DEFINE_STATIC_STRING(RightSuperiorLobeAnteriorBronchioleToRightSuperiorLobeAnteriorAlveolarDeadSpace);
    DEFINE_STATIC_STRING(RightMiddleLobeLateralBronchioleToRightMiddleLobeLateralAlveolarDeadSpace);
    DEFINE_STATIC_STRING(RightMiddleLobeMedialBronchioleToRightMiddleLobeMedialAlveolarDeadSpace);
    DEFINE_STATIC_STRING(RightInferiorLobeSuperiorBronchioleToRightInferiorLobeSuperiorAlveolarDeadSpace);
    DEFINE_STATIC_STRING(RightInferiorLobeMedialBasalBronchioleToRightInferiorLobeMedialBasalAlveolarDeadSpace);
    DEFINE_STATIC_STRING(RightInferiorLobeAnteriorBasalBronchioleToRightInferiorLobeAnteriorBasalAlveolarDeadSpace);
    DEFINE_STATIC_STRING(RightInferiorLobeLateralBasalBronchioleToRightInferiorLobeLateralBasalAlveolarDeadSpace);
    DEFINE_STATIC_STRING(RightInferiorLobePosteriorBasalBronchioleToRightInferiorLobePosteriorBasalAlveolarDeadSpace);

    DEFINE_STATIC_STRING(LeftInferiorLobePosteriorBasalBronchioleToLeftInferiorLobePosteriorBasalAlveolarDeadSpace);
    DEFINE_STATIC_STRING(LeftInferiorLobeLateralBasalBronchioleToLeftInferiorLobeLateralBasalAlveolarDeadSpace);
    DEFINE_STATIC_STRING(LeftInferiorLobeAnteromedialBasalBronchioleToLeftInferiorLobeAnteromedialBasalAlveolarDeadSpace);
    DEFINE_STATIC_STRING(LeftInferiorLobeSuperiorBronchioleToLeftInferiorLobeSuperiorAlveolarDeadSpace);
    DEFINE_STATIC_STRING(LeftSuperiorLobeInferiorLingulaBronchioleToLeftSuperiorLobeInferiorLingulaAlveolarDeadSpace);
    DEFINE_STATIC_STRING(LeftSuperiorLobeSuperiorLingulaBronchioleToLeftSuperiorLobeSuperiorLingulaAlveolarDeadSpace);
    DEFINE_STATIC_STRING(LeftSuperiorLobeAnteriorBronchioleToLeftSuperiorLobeAnteriorAlveolarDeadSpace);
    DEFINE_STATIC_STRING(LeftSuperiorLobeApicoposteriorBronchioleToLeftSuperiorLobeApicoposteriorAlveolarDeadSpace);

    DEFINE_STATIC_STRING(RightSuperiorLobeApicalAlveolarDeadSpaceToRightSuperiorLobeApicalAlveoli);
    DEFINE_STATIC_STRING(RightSuperiorLobePosteriorAlveolarDeadSpaceToRightSuperiorLobePosteriorAlveoli);
    DEFINE_STATIC_STRING(RightSuperiorLobeAnteriorAlveolarDeadSpaceToRightSuperiorLobeAnteriorAlveoli);
    DEFINE_STATIC_STRING(RightMiddleLobeLateralAlveolarDeadSpaceToRightMiddleLobeLateralAlveoli);
    DEFINE_STATIC_STRING(RightMiddleLobeMedialAlveolarDeadSpaceToRightMiddleLobeMedialAlveoli);
    DEFINE_STATIC_STRING(RightInferiorLobeSuperiorAlveolarDeadSpaceToRightInferiorLobeSuperiorAlveoli);
    DEFINE_STATIC_STRING(RightInferiorLobeMedialBasalAlveolarDeadSpaceToRightInferiorLobeMedialBasalAlveoli);
    DEFINE_STATIC_STRING(RightInferiorLobeAnteriorBasalAlveolarDeadSpaceToRightInferiorLobeAnteriorBasalAlveoli);
    DEFINE_STATIC_STRING(RightInferiorLobeLateralBasalAlveolarDeadSpaceToRightInferiorLobeLateralBasalAlveoli);
    DEFINE_STATIC_STRING(RightInferiorLobePosteriorBasalAlveolarDeadSpaceToRightInferiorLobePosteriorBasalAlveoli);

    DEFINE_STATIC_STRING(LeftInferiorLobePosteriorBasalAlveolarDeadSpaceToLeftInferiorLobePosteriorBasalAlveoli);
    DEFINE_STATIC_STRING(LeftInferiorLobeLateralBasalAlveolarDeadSpaceToLeftInferiorLobeLateralBasalAlveoli);
    DEFINE_STATIC_STRING(LeftInferiorLobeAnteromedialBasalAlveolarDeadSpaceToLeftInferiorLobeAnteromedialBasalAlveoli);
    DEFINE_STATIC_STRING(LeftInferiorLobeSuperiorAlveolarDeadSpaceToLeftInferiorLobeSuperiorAlveoli);
    DEFINE_STATIC_STRING(LeftSuperiorLobeInferiorLingulaAlveolarDeadSpaceToLeftSuperiorLobeInferiorLingulaAlveoli);
    DEFINE_STATIC_STRING(LeftSuperiorLobeSuperiorLingulaAlveolarDeadSpaceToLeftSuperiorLobeSuperiorLingulaAlveoli);
    DEFINE_STATIC_STRING(LeftSuperiorLobeAnteriorAlveolarDeadSpaceToLeftSuperiorLobeAnteriorAlveoli);
    DEFINE_STATIC_STRING(LeftSuperiorLobeApicoposteriorAlveolarDeadSpaceToLeftSuperiorLobeApicoposteriorAlveoli);

    DEFINE_STATIC_STRING(RightSuperiorLobeApicalAlveoliToRightPleuralConnection);
    DEFINE_STATIC_STRING(RightSuperiorLobePosteriorAlveoliToRightPleuralConnection);
    DEFINE_STATIC_STRING(RightSuperiorLobeAnteriorAlveoliToRightPleuralConnection);
    DEFINE_STATIC_STRING(RightMiddleLobeLateralAlveoliToRightPleuralConnection);
    DEFINE_STATIC_STRING(RightMiddleLobeMedialAlveoliToRightPleuralConnection);
    DEFINE_STATIC_STRING(RightInferiorLobeSuperiorAlveoliToRightPleuralConnection);
    DEFINE_STATIC_STRING(RightInferiorLobeMedialBasalAlveoliToRightPleuralConnection);
    DEFINE_STATIC_STRING(RightInferiorLobeAnteriorBasalAlveoliToRightPleuralConnection);
    DEFINE_STATIC_STRING(RightInferiorLobeLateralBasalAlveoliToRightPleuralConnection);
    DEFINE_STATIC_STRING(RightInferiorLobePosteriorBasalAlveoliToRightPleuralConnection);

    DEFINE_STATIC_STRING(LeftInferiorLobePosteriorBasalAlveoliToLeftPleuralConnection);
    DEFINE_STATIC_STRING(LeftInferiorLobeLateralBasalAlveoliToLeftPleuralConnection);
    DEFINE_STATIC_STRING(LeftInferiorLobeAnteromedialBasalAlveoliToLeftPleuralConnection);
    DEFINE_STATIC_STRING(LeftInferiorLobeSuperiorAlveoliToLeftPleuralConnection);
    DEFINE_STATIC_STRING(LeftSuperiorLobeInferiorLingulaAlveoliToLeftPleuralConnection);
    DEFINE_STATIC_STRING(LeftSuperiorLobeSuperiorLingulaAlveoliToLeftPleuralConnection);
    DEFINE_STATIC_STRING(LeftSuperiorLobeAnteriorAlveoliToLeftPleuralConnection);
    DEFINE_STATIC_STRING(LeftSuperiorLobeApicoposteriorAlveoliToLeftPleuralConnection);
  };

  //////////////////////////////////////
  // Anesthesia Machine Circuit Enums //
  //////////////////////////////////////

  class AnesthesiaMachineNode
  {
  public:
    DEFINE_STATIC_STRING_EX(Connection, AnesthesiaMachineConnection);
    DEFINE_STATIC_STRING_EX(ExpiratoryLimb, AnesthesiaMachineExpiratoryLimb);
    DEFINE_STATIC_STRING_EX(GasInlet, AnesthesiaMachineGasInlet);
    DEFINE_STATIC_STRING_EX(GasSource, AnesthesiaMachineGasSource);
    DEFINE_STATIC_STRING_EX(InspiratoryLimb, AnesthesiaMachineInspiratoryLimb);
    DEFINE_STATIC_STRING_EX(ReliefValve, AnesthesiaMachineReliefValve);
    DEFINE_STATIC_STRING_EX(Scrubber, AnesthesiaMachineScrubber);
    DEFINE_STATIC_STRING_EX(Selector, AnesthesiaMachineSelector);
    DEFINE_STATIC_STRING_EX(Ventilator, AnesthesiaMachineVentilator);
    DEFINE_STATIC_STRING_EX(YPiece, AnesthesiaMachineYPiece);
  };

  class AnesthesiaMachinePath
  {
  public:
    DEFINE_STATIC_STRING_EX(EnvironmentToVentilator, EnvironmentToAnesthesiaMachineVentilator);
    DEFINE_STATIC_STRING_EX(VentilatorToEnvironment, AnesthesiaMachineVentilatorToEnvironment);
    DEFINE_STATIC_STRING_EX(EnvironmentToReliefValve, EnvironmentToAnesthesiaMachineReliefValve);
    DEFINE_STATIC_STRING_EX(VentilatorToSelector, AnesthesiaMachineVentilatorToSelector);
    DEFINE_STATIC_STRING_EX(SelectorToReliefValve, AnesthesiaMachineSelectorToReliefValve);
    DEFINE_STATIC_STRING_EX(SelectorToScrubber, AnesthesiaMachineSelectorToScrubber);
    DEFINE_STATIC_STRING_EX(ScrubberToGasInlet, AnesthesiaMachineScrubberToGasInlet);
    DEFINE_STATIC_STRING_EX(EnvironmentToGasSource, EnvironmentToAnesthesiaMachineGasSource);
    DEFINE_STATIC_STRING_EX(GasSourceToGasInlet, AnesthesiaMachineGasSourceToGasInlet);
    DEFINE_STATIC_STRING_EX(GasInletToInspiratoryLimb, AnesthesiaMachineGasInletToInspiratoryLimb);
    DEFINE_STATIC_STRING_EX(InspiratoryLimbToYPiece, AnesthesiaMachineInspiratoryLimbToYPiece);
    DEFINE_STATIC_STRING_EX(YPieceToExpiratoryLimb, AnesthesiaMachineYPieceToExpiratoryLimb);
    DEFINE_STATIC_STRING_EX(ExpiratoryLimbToSelector, AnesthesiaMachineExpiratoryLimbToSelector);
    DEFINE_STATIC_STRING_EX(YPieceToConnection, AnesthesiaMachineYPieceToConnection);
    DEFINE_STATIC_STRING_EX(ConnectionToEnvironment, AnesthesiaMachineConnectionToEnvironment);
  };

  class CombinedAnesthesiaMachinePath
  {
  public:
    DEFINE_STATIC_STRING(ConnectionToAirway);
    DEFINE_STATIC_STRING(GroundConnection);
  };

  //////////////////////////////////
  // Bag Valve Mask Circuit Enums //
  //////////////////////////////////

  class BagValveMaskNode
  {
  public:
    DEFINE_STATIC_STRING_EX(Reservoir, BagValveMaskReservoir);
    DEFINE_STATIC_STRING_EX(Bag, BagValveMaskBag);
    DEFINE_STATIC_STRING_EX(Valve, BagValveMaskValve);
    DEFINE_STATIC_STRING_EX(Filter, BagValveMaskFilter);
    DEFINE_STATIC_STRING_EX(Connection, BagValveMaskConnection);
  };

  class BagValveMaskPath
  {
  public:
    DEFINE_STATIC_STRING_EX(EnvironmentToReservoir, BagValveMaskEnvironmentToReservoir);
    DEFINE_STATIC_STRING_EX(ReservoirToBag, BagValveMaskReservoirToBag);
    DEFINE_STATIC_STRING_EX(BagToValve, BagValveMaskBagToValve);
    DEFINE_STATIC_STRING_EX(ValveToFilter, BagValveMaskValveToFilter);
    DEFINE_STATIC_STRING_EX(FilterToConnection, BagValveMaskFilterToConnection);
    DEFINE_STATIC_STRING_EX(ConnectionToEnvironment, BagValveMaskConnectionToEnvironment);
  };

  class CombinedBagValveMaskPath
  {
  public:
    DEFINE_STATIC_STRING_EX(ConnectionToAirway, BagValveMaskConnectionToAirway);
  };

  ///////////////////////////
  // Inhaler Circuit Enums //
  ///////////////////////////

  class InhalerNode
  {
  public:
    DEFINE_STATIC_STRING_EX(Mouthpiece, InhalerMouthpiece);
  };

  class InhalerPath
  {
  public:
    DEFINE_STATIC_STRING_EX(EnvironmentToMouthpiece, EnvironmentToInhalerMouthpiece);
    DEFINE_STATIC_STRING_EX(MouthpieceToAirway, InhalerMouthpieceToAirway);
  };

  //////////////////////////////////////////
  // Mechanical Ventilation Circuit Enums //
  ////////////////////////////////////////

  class MechanicalVentilationNode
  {
  public:
    DEFINE_STATIC_STRING_EX(Connection, MechanicalVentilationConnection);
  };

  class MechanicalVentilationPath
  {
  public:
    DEFINE_STATIC_STRING_EX(ConnectionToAirway, MechanicalVentilationConnectionToAirway);
    DEFINE_STATIC_STRING_EX(GroundToConnection, MechanicalVentilationGroundToConnection);
  };

  /////////////////////////////////////////
  // Mechanical Ventilator Circuit Enums //
  /////////////////////////////////////////

  class MechanicalVentilatorNode
  {
  public:
    DEFINE_STATIC_STRING_EX(Ventilator, MechanicalVentilator);
    DEFINE_STATIC_STRING_EX(ExpiratoryValve, MechanicalVentilatorExpiratoryValve);
    DEFINE_STATIC_STRING_EX(InspiratoryValve, MechanicalVentilatorInspiratoryValve);
    DEFINE_STATIC_STRING_EX(ExpiratoryLimb, MechanicalVentilatorExpiratoryLimb);
    DEFINE_STATIC_STRING_EX(InspiratoryLimb, MechanicalVentilatorInspiratoryLimb);
    DEFINE_STATIC_STRING_EX(YPiece, MechanicalVentilatorYPiece);
    DEFINE_STATIC_STRING_EX(ReliefValve, MechanicalVentilatorReliefValve);
    DEFINE_STATIC_STRING_EX(Connection, MechanicalVentilatorConnection);
  };

  class MechanicalVentilatorPath
  {
  public:
    DEFINE_STATIC_STRING_EX(EnvironmentToVentilator, EnvironmentToMechanicalVentilator);
    DEFINE_STATIC_STRING_EX(VentilatorToEnvironment, MechanicalVentilatorToEnvironment);
    DEFINE_STATIC_STRING_EX(VentilatorToExpiratoryValve, MechanicalVentilatorToExpiratoryValve);
    DEFINE_STATIC_STRING_EX(VentilatorToInspiratoryValve, MechanicalVentilatorToInspiratoryValve);
    DEFINE_STATIC_STRING_EX(ExpiratoryLimbToExpiratoryValve, MechanicalVentilatorExpiratoryLimbToExpiratoryValve);
    DEFINE_STATIC_STRING_EX(InspiratoryValveToInspiratoryLimb, MechanicalVentilatorInspiratoryValveToInspiratoryLimb);
    DEFINE_STATIC_STRING_EX(ExpiratoryLimbToYPiece, MechanicalVentilatorExpiratoryLimbToYPiece);
    DEFINE_STATIC_STRING_EX(InspiratoryLimbToYPiece, MechanicalVentilatorInspiratoryLimbToYPiece);
    DEFINE_STATIC_STRING_EX(YPieceToConnection, MechanicalVentilatorYPieceToConnection);
    DEFINE_STATIC_STRING_EX(LeakConnectionToEnvironment, MechanicalVentilatorLeakConnectionToEnvironment);
    DEFINE_STATIC_STRING_EX(ConnectionToReliefValve, MechanicalVentilatorConnectionToReliefValve);
    DEFINE_STATIC_STRING_EX(EnvironmentToReliefValve, MechanicalVentilatorEnvironmentToReliefVlave);
    DEFINE_STATIC_STRING_EX(ConnectionToEnvironment, MechanicalVentilatorConnectionToEnvironment);
  };

  class CombinedMechanicalVentilatorPath
  {
  public:
    DEFINE_STATIC_STRING_EX(ConnectionToAirway, MechanicalVentilatorConnectionToAirway);
  };

  /////////////////////////////////
  // Nasal Cannula Circuit Enums //
  /////////////////////////////////

  class NasalCannulaNode
  {
  public:
    DEFINE_STATIC_STRING(NasalCannula);
    DEFINE_STATIC_STRING(NasalCannulaOxygenSource);
  };

  class NasalCannulaPath
  {
  public:
    DEFINE_STATIC_STRING(NasalCannulaPressure);
    DEFINE_STATIC_STRING(NasalCannulaOxygenInlet);
    DEFINE_STATIC_STRING(NasalCannulaSeal);
    DEFINE_STATIC_STRING(NasalCannulaToAirway);
  };

  ///////////////////////////////////
  // NonRebreather Mask Circuit Enums //
  ///////////////////////////////////

  class NonRebreatherMaskNode
  {
  public:
    DEFINE_STATIC_STRING(NonRebreatherMaskOxygenSource);
    DEFINE_STATIC_STRING(NonRebreatherMask);
    DEFINE_STATIC_STRING(NonRebreatherMaskBag);
    DEFINE_STATIC_STRING(NonRebreatherMaskPorts);
  };

  class NonRebreatherMaskPath
  {
  public:
    DEFINE_STATIC_STRING(NonRebreatherMaskPressure);
    DEFINE_STATIC_STRING(NonRebreatherMaskOxygenInlet);
    DEFINE_STATIC_STRING(NonRebreatherMaskBag);
    DEFINE_STATIC_STRING(NonRebreatherMaskReservoirValve);
    DEFINE_STATIC_STRING(NonRebreatherMaskSeal);
    DEFINE_STATIC_STRING(NonRebreatherMaskExhalationValves);
    DEFINE_STATIC_STRING(NonRebreatherMaskExhalation);
    DEFINE_STATIC_STRING(NonRebreatherMaskToAirway);
  };

  ////////////////////////////////
  // Simple Mask Circuit Enums //
  ////////////////////////////////

  class SimpleMaskNode
  {
  public:
    DEFINE_STATIC_STRING(SimpleMask);
    DEFINE_STATIC_STRING(SimpleMaskOxygenSource);
  };

  class SimpleMaskPath
  {
  public:
    DEFINE_STATIC_STRING(SimpleMaskPressure);
    DEFINE_STATIC_STRING(SimpleMaskOxygenInlet);
    DEFINE_STATIC_STRING(SimpleMaskSeal);
    DEFINE_STATIC_STRING(SimpleMaskPorts);
    DEFINE_STATIC_STRING(SimpleMaskToAirway);
  };

  ///////////////////////////////////
  // Environment Gas Circuit Enums //
  ///////////////////////////////////

  class EnvironmentNode
  {
  public:
    DEFINE_STATIC_STRING(Ambient);
  };

  ///////////////////////////////////////////
  // External Temperature Circuit Enums //
  ///////////////////////////////////////////

  class ExternalTemperatureNode
  {
  public:
    DEFINE_STATIC_STRING(Active);
    DEFINE_STATIC_STRING(Clothing);
    DEFINE_STATIC_STRING(Enclosure);
    DEFINE_STATIC_STRING(ExternalCore);
    DEFINE_STATIC_STRING(ExternalSkin);
    DEFINE_STATIC_STRING(ExternalGround);
    DEFINE_STATIC_STRING(Ambient);
  };

  class ExternalTemperaturePath
  {
  public:
    DEFINE_STATIC_STRING(ActiveToClothing);
    DEFINE_STATIC_STRING(ClothingToEnclosure);
    DEFINE_STATIC_STRING(ClothingToEnvironment);
    DEFINE_STATIC_STRING(ExternalCoreToGround);
    DEFINE_STATIC_STRING(GroundToActive);
    DEFINE_STATIC_STRING(GroundToClothing);
    DEFINE_STATIC_STRING(GroundToEnclosure);
    DEFINE_STATIC_STRING(GroundToEnvironment);
    DEFINE_STATIC_STRING(ExternalSkinToGround);
    DEFINE_STATIC_STRING(ExternalSkinToClothing);
  };

  ////////////////////////////////////////
  // Internal Temperature Circuit Enums //
  ////////////////////////////////////////

  class InternalTemperatureNode
  {
  public:
    DEFINE_STATIC_STRING(InternalCore);
    DEFINE_STATIC_STRING(InternalSkin);
    DEFINE_STATIC_STRING(InternalGround);
  };

  class InternalTemperaturePath
  {
  public:
    DEFINE_STATIC_STRING(GroundToInternalCore);
    DEFINE_STATIC_STRING(InternalCoreToInternalSkin);
    DEFINE_STATIC_STRING(InternalCoreToGround);
    DEFINE_STATIC_STRING(InternalSkinToGround);
  };

  ////////////////////////////////////////
  // Combined Temperature Circuit Enums //
  ////////////////////////////////////////

  class CombinedTemperaturePath
  {
  public:
    DEFINE_STATIC_STRING(InternalCoreToExternalCore);
    DEFINE_STATIC_STRING(InternalSkinToExternalSkin);
  };

  class CardiovascularNode
  {
  public:
    DEFINE_STATIC_STRING(RightHeart1);
    DEFINE_STATIC_STRING(RightHeart2);
    DEFINE_STATIC_STRING(RightHeart3);

    DEFINE_STATIC_STRING(MainPulmonaryArteries1);
    DEFINE_STATIC_STRING(LeftIntermediatePulmonaryArteries1);
    DEFINE_STATIC_STRING(LeftPulmonaryArteries1);
    DEFINE_STATIC_STRING(RightIntermediatePulmonaryArteries1);
    DEFINE_STATIC_STRING(RightPulmonaryArteries1);

    DEFINE_STATIC_STRING(LeftPulmonaryCapillaries1);
    DEFINE_STATIC_STRING(RightPulmonaryCapillaries1);

    DEFINE_STATIC_STRING(LeftIntermediatePulmonaryVeins1);
    DEFINE_STATIC_STRING(LeftPulmonaryVeins1);
    DEFINE_STATIC_STRING(RightIntermediatePulmonaryVeins1);
    DEFINE_STATIC_STRING(RightPulmonaryVeins1);

    DEFINE_STATIC_STRING(LeftHeart1);
    DEFINE_STATIC_STRING(LeftHeart2);
    DEFINE_STATIC_STRING(LeftHeart3);

    DEFINE_STATIC_STRING(Aorta1);
    DEFINE_STATIC_STRING(Aorta2);
    DEFINE_STATIC_STRING(Aorta3);
    DEFINE_STATIC_STRING(Aorta4);

    DEFINE_STATIC_STRING(AbdominalCavity1);

    DEFINE_STATIC_STRING(Bone1);
    DEFINE_STATIC_STRING(Bone2);

    DEFINE_STATIC_STRING(Brain1);
    DEFINE_STATIC_STRING(Brain2);

    DEFINE_STATIC_STRING(Fat1);
    DEFINE_STATIC_STRING(Fat2);

    DEFINE_STATIC_STRING(LargeIntestine1);

    DEFINE_STATIC_STRING(Liver1);
    DEFINE_STATIC_STRING(Liver2);
    DEFINE_STATIC_STRING(PortalVein1);

    DEFINE_STATIC_STRING(LeftArm1);
    DEFINE_STATIC_STRING(LeftArm2);

    DEFINE_STATIC_STRING(LeftKidney1);
    DEFINE_STATIC_STRING(LeftKidney2);

    DEFINE_STATIC_STRING(LeftLeg1);
    DEFINE_STATIC_STRING(LeftLeg2);

    DEFINE_STATIC_STRING(Muscle1);
    DEFINE_STATIC_STRING(Muscle2);

    DEFINE_STATIC_STRING(Myocardium1);
    DEFINE_STATIC_STRING(Myocardium2);

    DEFINE_STATIC_STRING(Pericardium1);

    DEFINE_STATIC_STRING(RightArm1);
    DEFINE_STATIC_STRING(RightArm2);

    DEFINE_STATIC_STRING(RightKidney1);
    DEFINE_STATIC_STRING(RightKidney2);

    DEFINE_STATIC_STRING(RightLeg1);
    DEFINE_STATIC_STRING(RightLeg2);

    DEFINE_STATIC_STRING(Skin1);
    DEFINE_STATIC_STRING(Skin2);

    DEFINE_STATIC_STRING(SmallIntestine1);

    DEFINE_STATIC_STRING(Splanchnic1);

    DEFINE_STATIC_STRING(Spleen1);

    DEFINE_STATIC_STRING(VenaCava1);

    DEFINE_STATIC_STRING(Ground);

    DEFINE_STATIC_STRING(RightSuperiorLobeApicalPulmonaryCapillaries);
    DEFINE_STATIC_STRING(RightSuperiorLobePosteriorPulmonaryCapillaries);
    DEFINE_STATIC_STRING(RightSuperiorLobeAnteriorPulmonaryCapillaries);
    DEFINE_STATIC_STRING(RightMiddleLobeLateralPulmonaryCapillaries);
    DEFINE_STATIC_STRING(RightMiddleLobeMedialPulmonaryCapillaries);
    DEFINE_STATIC_STRING(RightInferiorLobeSuperiorPulmonaryCapillaries);
    DEFINE_STATIC_STRING(RightInferiorLobeMedialBasalPulmonaryCapillaries);
    DEFINE_STATIC_STRING(RightInferiorLobeAnteriorBasalPulmonaryCapillaries);
    DEFINE_STATIC_STRING(RightInferiorLobeLateralBasalPulmonaryCapillaries);
    DEFINE_STATIC_STRING(RightInferiorLobePosteriorBasalPulmonaryCapillaries);

    DEFINE_STATIC_STRING(LeftInferiorLobePosteriorBasalPulmonaryCapillaries);
    DEFINE_STATIC_STRING(LeftInferiorLobeLateralBasalPulmonaryCapillaries);
    DEFINE_STATIC_STRING(LeftInferiorLobeAnteromedialBasalPulmonaryCapillaries);
    DEFINE_STATIC_STRING(LeftInferiorLobeSuperiorPulmonaryCapillaries);
    DEFINE_STATIC_STRING(LeftSuperiorLobeInferiorLingulaPulmonaryCapillaries);
    DEFINE_STATIC_STRING(LeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries);
    DEFINE_STATIC_STRING(LeftSuperiorLobeAnteriorPulmonaryCapillaries);
    DEFINE_STATIC_STRING(LeftSuperiorLobeApicoposteriorPulmonaryCapillaries);
  };

  class CardiovascularPath
  {
  public:
    // Heart and Lungs
    DEFINE_STATIC_STRING(VenaCava1ToRightHeart2);
    DEFINE_STATIC_STRING(RightHeart2ToRightHeart1);
    DEFINE_STATIC_STRING(RightHeart1ToRightHeart3);
    DEFINE_STATIC_STRING(RightHeart3ToGround);
    DEFINE_STATIC_STRING(RightHeart1ToMainPulmonaryArteries1);
    DEFINE_STATIC_STRING(MainPulmonaryArteries1ToRightIntermediatePulmonaryArteries1);
    DEFINE_STATIC_STRING(RightIntermediatePulmonaryArteries1ToRightPulmonaryArteries1);
    DEFINE_STATIC_STRING(RightPulmonaryArteries1ToRightPulmonaryVeins1);
    DEFINE_STATIC_STRING(RightPulmonaryArteries1ToRightPulmonaryCapillaries1);
    DEFINE_STATIC_STRING(RightPulmonaryArteries1ToGround);
    DEFINE_STATIC_STRING(RightPulmonaryCapillaries1ToRightPulmonaryVeins1);
    DEFINE_STATIC_STRING(RightPulmonaryCapillaries1ToGround);
    DEFINE_STATIC_STRING(RightPulmonaryVeins1ToRightIntermediatePulmonaryVeins1);
    DEFINE_STATIC_STRING(RightPulmonaryVeins1ToGround);
    DEFINE_STATIC_STRING(RightPulmonaryVeinsLeak1ToGround);
    DEFINE_STATIC_STRING(RightIntermediatePulmonaryVeins1ToLeftHeart2);
    DEFINE_STATIC_STRING(MainPulmonaryArteries1ToLeftIntermediatePulmonaryArteries1);
    DEFINE_STATIC_STRING(LeftIntermediatePulmonaryArteries1ToLeftPulmonaryArteries1);
    DEFINE_STATIC_STRING(LeftPulmonaryArteries1ToLeftPulmonaryVeins1);
    DEFINE_STATIC_STRING(LeftPulmonaryArteries1ToLeftPulmonaryCapillaries1);
    DEFINE_STATIC_STRING(LeftPulmonaryArteries1ToGround);
    DEFINE_STATIC_STRING(LeftPulmonaryCapillaries1ToGround);
    DEFINE_STATIC_STRING(LeftPulmonaryCapillaries1ToLeftPulmonaryVeins1);
    DEFINE_STATIC_STRING(LeftPulmonaryVeins1ToLeftIntermediatePulmonaryVeins1);
    DEFINE_STATIC_STRING(LeftPulmonaryVeins1ToGround);
    DEFINE_STATIC_STRING(LeftPulmonaryVeinsLeak1ToGround);
    DEFINE_STATIC_STRING(LeftIntermediatePulmonaryVeins1ToLeftHeart2)


    DEFINE_STATIC_STRING(LeftHeart2ToLeftHeart1);
    DEFINE_STATIC_STRING(LeftHeart1ToLeftHeart3);
    DEFINE_STATIC_STRING(LeftHeart3ToGround);
    DEFINE_STATIC_STRING(LeftHeart1ToAorta2);
    DEFINE_STATIC_STRING(Aorta2ToAorta3);
    DEFINE_STATIC_STRING(Aorta3ToAorta1);
    DEFINE_STATIC_STRING(Aorta1ToAorta4);

    // Abdominal Cavity
    DEFINE_STATIC_STRING(GroundToAorta4);
    DEFINE_STATIC_STRING(AbdominalCavity1ToGround);
    DEFINE_STATIC_STRING(GroundToAbdominalCavity1);

    // Brain
    DEFINE_STATIC_STRING(Aorta1ToBrain1);
    DEFINE_STATIC_STRING(Brain1ToGround);
    DEFINE_STATIC_STRING(Brain1ToBrain2);
    DEFINE_STATIC_STRING(Brain2ToVenaCava1);
    // Bone
    DEFINE_STATIC_STRING(Aorta1ToBone1);
    DEFINE_STATIC_STRING(Bone1ToGround);
    DEFINE_STATIC_STRING(Bone1ToBone2);
    DEFINE_STATIC_STRING(Bone2ToVenaCava1);
    // Fat
    DEFINE_STATIC_STRING(Aorta1ToFat1);
    DEFINE_STATIC_STRING(Fat1ToGround);
    DEFINE_STATIC_STRING(Fat1ToFat2);
    DEFINE_STATIC_STRING(Fat2ToVenaCava1);
    // Large Intestine
    DEFINE_STATIC_STRING(Aorta1ToLargeIntestine1);
    DEFINE_STATIC_STRING(LargeIntestine1ToGround);
    DEFINE_STATIC_STRING(LargeIntestine1ToPortalVein1);
    // Left Arm
    DEFINE_STATIC_STRING(Aorta1ToLeftArm1);
    DEFINE_STATIC_STRING(LeftArm1ToGround);
    DEFINE_STATIC_STRING(LeftArm1ToLeftArm2);
    DEFINE_STATIC_STRING(LeftArm2ToVenaCava1);
    // Left Kidney
    DEFINE_STATIC_STRING(Aorta1ToLeftKidney1);
    DEFINE_STATIC_STRING(LeftKidney1ToGround);
    DEFINE_STATIC_STRING(LeftKidney1ToLeftKidney2);
    DEFINE_STATIC_STRING(LeftKidney2ToVenaCava1);
    // Left Leg
    DEFINE_STATIC_STRING(Aorta1ToLeftLeg1);
    DEFINE_STATIC_STRING(LeftLeg1ToGround);
    DEFINE_STATIC_STRING(LeftLeg1ToLeftLeg2);
    DEFINE_STATIC_STRING(LeftLeg2ToVenaCava1);
    // Liver
    DEFINE_STATIC_STRING(Aorta1ToLiver1);
    DEFINE_STATIC_STRING(Liver1ToGround);
    DEFINE_STATIC_STRING(PortalVein1ToLiver1);
    DEFINE_STATIC_STRING(Liver1ToLiver2);
    DEFINE_STATIC_STRING(Liver2ToVenaCava1);
    // Muscle
    DEFINE_STATIC_STRING(Aorta1ToMuscle1);
    DEFINE_STATIC_STRING(Muscle1ToGround);
    DEFINE_STATIC_STRING(Muscle1ToMuscle2);
    DEFINE_STATIC_STRING(Muscle2ToVenaCava1);
    // Myocardium
    DEFINE_STATIC_STRING(Aorta1ToMyocardium1);
    DEFINE_STATIC_STRING(Myocardium1ToGround);
    DEFINE_STATIC_STRING(Myocardium1ToMyocardium2);
    DEFINE_STATIC_STRING(Myocardium2ToVenaCava1);
    // Pericardium
    DEFINE_STATIC_STRING(Pericardium1ToGround);
    DEFINE_STATIC_STRING(GroundToPericardium1);
    // Right Arm
    DEFINE_STATIC_STRING(Aorta1ToRightArm1);
    DEFINE_STATIC_STRING(RightArm1ToGround);
    DEFINE_STATIC_STRING(RightArm1ToRightArm2);
    DEFINE_STATIC_STRING(RightArm2ToVenaCava1);
    // Right Kidney
    DEFINE_STATIC_STRING(Aorta1ToRightKidney1);
    DEFINE_STATIC_STRING(RightKidney1ToGround);
    DEFINE_STATIC_STRING(RightKidney1ToRightKidney2);
    DEFINE_STATIC_STRING(RightKidney2ToVenaCava1);
    // Right Leg
    DEFINE_STATIC_STRING(Aorta1ToRightLeg1);
    DEFINE_STATIC_STRING(RightLeg1ToGround);
    DEFINE_STATIC_STRING(RightLeg1ToRightLeg2);
    DEFINE_STATIC_STRING(RightLeg2ToVenaCava1);
    // Skin
    DEFINE_STATIC_STRING(Aorta1ToSkin1);
    DEFINE_STATIC_STRING(Skin1ToGround);
    DEFINE_STATIC_STRING(Skin1ToSkin2);
    DEFINE_STATIC_STRING(Skin2ToVenaCava1);
    // Small Intestine
    DEFINE_STATIC_STRING(Aorta1ToSmallIntestine1);
    DEFINE_STATIC_STRING(SmallIntestine1ToGround);
    DEFINE_STATIC_STRING(SmallIntestine1ToPortalVein1);
    // Splanchnic
    DEFINE_STATIC_STRING(Aorta1ToSplanchnic1);
    DEFINE_STATIC_STRING(Splanchnic1ToGround);
    DEFINE_STATIC_STRING(Splanchnic1ToPortalVein1);
    // Spleen
    DEFINE_STATIC_STRING(Aorta1ToSpleen1);
    DEFINE_STATIC_STRING(Spleen1ToGround);
    DEFINE_STATIC_STRING(Spleen1ToPortalVein1);
    // Vena Cava
    DEFINE_STATIC_STRING(VenaCava1ToGround);
    DEFINE_STATIC_STRING(IVToVenaCava1);

    DEFINE_STATIC_STRING(RightPulmonaryArteries1ToRightSuperiorLobeApicalPulmonaryCapillaries);
    DEFINE_STATIC_STRING(RightPulmonaryArteries1ToRightSuperiorLobePosteriorPulmonaryCapillaries);
    DEFINE_STATIC_STRING(RightPulmonaryArteries1ToRightSuperiorLobeAnteriorPulmonaryCapillaries);
    DEFINE_STATIC_STRING(RightPulmonaryArteries1ToRightMiddleLobeLateralPulmonaryCapillaries);
    DEFINE_STATIC_STRING(RightPulmonaryArteries1ToRightMiddleLobeMedialPulmonaryCapillaries);
    DEFINE_STATIC_STRING(RightPulmonaryArteries1ToRightInferiorLobeSuperiorPulmonaryCapillaries);
    DEFINE_STATIC_STRING(RightPulmonaryArteries1ToRightInferiorLobeMedialBasalPulmonaryCapillaries);
    DEFINE_STATIC_STRING(RightPulmonaryArteries1ToRightInferiorLobeAnteriorBasalPulmonaryCapillaries);
    DEFINE_STATIC_STRING(RightPulmonaryArteries1ToRightInferiorLobeLateralBasalPulmonaryCapillaries);
    DEFINE_STATIC_STRING(RightPulmonaryArteries1ToRightInferiorLobePosteriorBasalPulmonaryCapillaries);

    DEFINE_STATIC_STRING(LeftPulmonaryArteries1ToLeftInferiorLobePosteriorBasalPulmonaryCapillaries);
    DEFINE_STATIC_STRING(LeftPulmonaryArteries1ToLeftInferiorLobeLateralBasalPulmonaryCapillaries);
    DEFINE_STATIC_STRING(LeftPulmonaryArteries1ToLeftInferiorLobeAnteromedialBasalPulmonaryCapillaries);
    DEFINE_STATIC_STRING(LeftPulmonaryArteries1ToLeftInferiorLobeSuperiorPulmonaryCapillaries);
    DEFINE_STATIC_STRING(LeftPulmonaryArteries1ToLeftSuperiorLobeInferiorLingulaPulmonaryCapillaries);
    DEFINE_STATIC_STRING(LeftPulmonaryArteries1ToLeftSuperiorLobeSuperiorLingulaPulmonaryCapillaries);
    DEFINE_STATIC_STRING(LeftPulmonaryArteries1ToLeftSuperiorLobeAnteriorPulmonaryCapillaries);
    DEFINE_STATIC_STRING(LeftPulmonaryArteries1ToLeftSuperiorLobeApicoposteriorPulmonaryCapillaries);

    DEFINE_STATIC_STRING(RightSuperiorLobeApicalPulmonaryCapillariesToRightPulmonaryVeins1);
    DEFINE_STATIC_STRING(RightSuperiorLobePosteriorPulmonaryCapillariesToRightPulmonaryVeins1);
    DEFINE_STATIC_STRING(RightSuperiorLobeAnteriorPulmonaryCapillariesToRightPulmonaryVeins1);
    DEFINE_STATIC_STRING(RightMiddleLobeLateralPulmonaryCapillariesToRightPulmonaryVeins1);
    DEFINE_STATIC_STRING(RightMiddleLobeMedialPulmonaryCapillariesToRightPulmonaryVeins1);
    DEFINE_STATIC_STRING(RightInferiorLobeSuperiorPulmonaryCapillariesToRightPulmonaryVeins1);
    DEFINE_STATIC_STRING(RightInferiorLobeMedialBasalPulmonaryCapillariesToRightPulmonaryVeins1);
    DEFINE_STATIC_STRING(RightInferiorLobeAnteriorBasalPulmonaryCapillariesToRightPulmonaryVeins1);
    DEFINE_STATIC_STRING(RightInferiorLobeLateralBasalPulmonaryCapillariesToRightPulmonaryVeins1);
    DEFINE_STATIC_STRING(RightInferiorLobePosteriorBasalPulmonaryCapillariesToRightPulmonaryVeins1);

    DEFINE_STATIC_STRING(LeftInferiorLobePosteriorBasalPulmonaryCapillariesToLeftPulmonaryVeins1);
    DEFINE_STATIC_STRING(LeftInferiorLobeLateralBasalPulmonaryCapillariesToLeftPulmonaryVeins1);
    DEFINE_STATIC_STRING(LeftInferiorLobeAnteromedialBasalPulmonaryCapillariesToLeftPulmonaryVeins1);
    DEFINE_STATIC_STRING(LeftInferiorLobeSuperiorPulmonaryCapillariesToLeftPulmonaryVeins1);
    DEFINE_STATIC_STRING(LeftSuperiorLobeInferiorLingulaPulmonaryCapillariesToLeftPulmonaryVeins1);
    DEFINE_STATIC_STRING(LeftSuperiorLobeSuperiorLingulaPulmonaryCapillariesToLeftPulmonaryVeins1);
    DEFINE_STATIC_STRING(LeftSuperiorLobeAnteriorPulmonaryCapillariesToLeftPulmonaryVeins1);
    DEFINE_STATIC_STRING(LeftSuperiorLobeApicoposteriorPulmonaryCapillariesToLeftPulmonaryVeins1);

    DEFINE_STATIC_STRING(RightSuperiorLobeApicalPulmonaryCapillariesToGround);
    DEFINE_STATIC_STRING(RightSuperiorLobePosteriorPulmonaryCapillariesToGround);
    DEFINE_STATIC_STRING(RightSuperiorLobeAnteriorPulmonaryCapillariesToGround);
    DEFINE_STATIC_STRING(RightMiddleLobeLateralPulmonaryCapillariesToGround);
    DEFINE_STATIC_STRING(RightMiddleLobeMedialPulmonaryCapillariesToGround);
    DEFINE_STATIC_STRING(RightInferiorLobeSuperiorPulmonaryCapillariesToGround);
    DEFINE_STATIC_STRING(RightInferiorLobeMedialBasalPulmonaryCapillariesToGround);
    DEFINE_STATIC_STRING(RightInferiorLobeAnteriorBasalPulmonaryCapillariesToGround);
    DEFINE_STATIC_STRING(RightInferiorLobeLateralBasalPulmonaryCapillariesToGround);
    DEFINE_STATIC_STRING(RightInferiorLobePosteriorBasalPulmonaryCapillariesToGround);

    DEFINE_STATIC_STRING(LeftInferiorLobePosteriorBasalPulmonaryCapillariesToGround);
    DEFINE_STATIC_STRING(LeftInferiorLobeLateralBasalPulmonaryCapillariesToGround);
    DEFINE_STATIC_STRING(LeftInferiorLobeAnteromedialBasalPulmonaryCapillariesToGround);
    DEFINE_STATIC_STRING(LeftInferiorLobeSuperiorPulmonaryCapillariesToGround);
    DEFINE_STATIC_STRING(LeftSuperiorLobeInferiorLingulaPulmonaryCapillariesToGround);
    DEFINE_STATIC_STRING(LeftSuperiorLobeSuperiorLingulaPulmonaryCapillariesToGround);
    DEFINE_STATIC_STRING(LeftSuperiorLobeAnteriorPulmonaryCapillariesToGround);
    DEFINE_STATIC_STRING(LeftSuperiorLobeApicoposteriorPulmonaryCapillariesToGround);

    DEFINE_STATIC_STRING(RightSuperiorLobeApicalShunt);
    DEFINE_STATIC_STRING(RightSuperiorLobePosteriorShunt);
    DEFINE_STATIC_STRING(RightSuperiorLobeAnteriorShunt);
    DEFINE_STATIC_STRING(RightMiddleLobeLateralShunt);
    DEFINE_STATIC_STRING(RightMiddleLobeMedialShunt);
    DEFINE_STATIC_STRING(RightInferiorLobeSuperiorShunt);
    DEFINE_STATIC_STRING(RightInferiorLobeMedialBasalShunt);
    DEFINE_STATIC_STRING(RightInferiorLobeAnteriorBasalShunt);
    DEFINE_STATIC_STRING(RightInferiorLobeLateralBasalShunt);
    DEFINE_STATIC_STRING(RightInferiorLobePosteriorBasalShunt);

    DEFINE_STATIC_STRING(LeftInferiorLobePosteriorBasalShunt);
    DEFINE_STATIC_STRING(LeftInferiorLobeLateralBasalShunt);
    DEFINE_STATIC_STRING(LeftInferiorLobeAnteromedialBasalShunt);
    DEFINE_STATIC_STRING(LeftInferiorLobeSuperiorShunt);
    DEFINE_STATIC_STRING(LeftSuperiorLobeInferiorLingulaShunt);
    DEFINE_STATIC_STRING(LeftSuperiorLobeSuperiorLingulaShunt);
    DEFINE_STATIC_STRING(LeftSuperiorLobeAnteriorShunt);
    DEFINE_STATIC_STRING(LeftSuperiorLobeApicoposteriorShunt);
  };

  class ExpandedCardiovascularNode
  {
  public:
    DEFINE_STATIC_STRING(RightHeart1);
    DEFINE_STATIC_STRING(RightHeart2);
    DEFINE_STATIC_STRING(RightHeart3);

    DEFINE_STATIC_STRING(MainPulmonaryArteries1);
    DEFINE_STATIC_STRING(LeftIntermediatePulmonaryArteries1);
    DEFINE_STATIC_STRING(LeftPulmonaryArteries1);
    DEFINE_STATIC_STRING(RightIntermediatePulmonaryArteries1);
    DEFINE_STATIC_STRING(RightPulmonaryArteries1);

    DEFINE_STATIC_STRING(LeftPulmonaryCapillaries1);
    DEFINE_STATIC_STRING(RightPulmonaryCapillaries1);

    DEFINE_STATIC_STRING(LeftIntermediatePulmonaryVeins1);
    DEFINE_STATIC_STRING(LeftPulmonaryVeins1);
    DEFINE_STATIC_STRING(RightIntermediatePulmonaryVeins1);
    DEFINE_STATIC_STRING(RightPulmonaryVeins1);

    DEFINE_STATIC_STRING(LeftHeart1);
    DEFINE_STATIC_STRING(LeftHeart2);
    DEFINE_STATIC_STRING(LeftHeart3);

    DEFINE_STATIC_STRING(Aorta1);
    DEFINE_STATIC_STRING(Aorta2);
    DEFINE_STATIC_STRING(Aorta3);
    DEFINE_STATIC_STRING(Aorta4);
    DEFINE_STATIC_STRING(Aorta5);

    DEFINE_STATIC_STRING(AbdominalCavity1);

    DEFINE_STATIC_STRING(Extracranial1);
    DEFINE_STATIC_STRING(Extracranial2);
    DEFINE_STATIC_STRING(Extracranial3);
    DEFINE_STATIC_STRING(Intracranial1);
    DEFINE_STATIC_STRING(Intracranial2);
    DEFINE_STATIC_STRING(Intracranial3);

    DEFINE_STATIC_STRING(Gut1);

    DEFINE_STATIC_STRING(LargeIntestine1);

    DEFINE_STATIC_STRING(Liver1);
    DEFINE_STATIC_STRING(Liver2);
    DEFINE_STATIC_STRING(PortalVein1);

    DEFINE_STATIC_STRING(LeftArm1);
    DEFINE_STATIC_STRING(LeftArm2);
    DEFINE_STATIC_STRING(LeftArmBone1);
    DEFINE_STATIC_STRING(LeftArmFat1);
    DEFINE_STATIC_STRING(LeftArmMicroVasculature1);
    DEFINE_STATIC_STRING(LeftArmMuscle1);
    DEFINE_STATIC_STRING(LeftArmSkin1);

    DEFINE_STATIC_STRING(LeftKidney1);
    DEFINE_STATIC_STRING(LeftKidney2);
    DEFINE_STATIC_STRING(LeftKidney3);

    DEFINE_STATIC_STRING(LeftLeg1);
    DEFINE_STATIC_STRING(LeftLeg2);
    DEFINE_STATIC_STRING(LeftLegBone1);
    DEFINE_STATIC_STRING(LeftLegFat1);
    DEFINE_STATIC_STRING(LeftLegMicroVasculature1);
    DEFINE_STATIC_STRING(LeftLegMuscle1);
    DEFINE_STATIC_STRING(LeftLegSkin1);

    DEFINE_STATIC_STRING(Myocardium1);
    DEFINE_STATIC_STRING(Myocardium2);
    DEFINE_STATIC_STRING(Myocardium3);

    DEFINE_STATIC_STRING(Pericardium1);

    DEFINE_STATIC_STRING(RightArm1);
    DEFINE_STATIC_STRING(RightArm2);
    DEFINE_STATIC_STRING(RightArmBone1);
    DEFINE_STATIC_STRING(RightArmFat1);
    DEFINE_STATIC_STRING(RightArmMicroVasculature1);
    DEFINE_STATIC_STRING(RightArmMuscle1);
    DEFINE_STATIC_STRING(RightArmSkin1);

    DEFINE_STATIC_STRING(RightKidney1);
    DEFINE_STATIC_STRING(RightKidney2);
    DEFINE_STATIC_STRING(RightKidney3);

    DEFINE_STATIC_STRING(RightLeg1);
    DEFINE_STATIC_STRING(RightLeg2);
    DEFINE_STATIC_STRING(RightLegBone1);
    DEFINE_STATIC_STRING(RightLegFat1);
    DEFINE_STATIC_STRING(RightLegMicroVasculature1);
    DEFINE_STATIC_STRING(RightLegMuscle1);
    DEFINE_STATIC_STRING(RightLegSkin1);

    DEFINE_STATIC_STRING(SmallIntestine1);

    DEFINE_STATIC_STRING(Splanchnic1);

    DEFINE_STATIC_STRING(Spleen1);

    DEFINE_STATIC_STRING(Torso1)
    DEFINE_STATIC_STRING(Torso2)
    DEFINE_STATIC_STRING(TorsoBone1);
    DEFINE_STATIC_STRING(TorsoFat1);
    DEFINE_STATIC_STRING(TorsoMicrovasculature1)
    DEFINE_STATIC_STRING(TorsoMuscle1);
    DEFINE_STATIC_STRING(TorsoSkin1);

    DEFINE_STATIC_STRING(VenaCava1);
    DEFINE_STATIC_STRING(VenaCava2);

    DEFINE_STATIC_STRING(Ground);
  };

  class ExpandedCardiovascularPath
  {
  public:
    // Heart and Lungs
    DEFINE_STATIC_STRING(VenaCava1ToRightHeart2);
    DEFINE_STATIC_STRING(RightHeart2ToRightHeart1);
    DEFINE_STATIC_STRING(RightHeart1ToRightHeart3);
    DEFINE_STATIC_STRING(RightHeart3ToGround);
    DEFINE_STATIC_STRING(RightHeart1ToMainPulmonaryArteries1);
    DEFINE_STATIC_STRING(MainPulmonaryArteries1ToRightIntermediatePulmonaryArteries1);
    DEFINE_STATIC_STRING(RightIntermediatePulmonaryArteries1ToRightPulmonaryArteries1);
    DEFINE_STATIC_STRING(RightPulmonaryArteries1ToRightPulmonaryVeins1);
    DEFINE_STATIC_STRING(RightPulmonaryArteries1ToRightPulmonaryCapillaries1);
    DEFINE_STATIC_STRING(RightPulmonaryArteries1ToGround);
    DEFINE_STATIC_STRING(RightPulmonaryCapillaries1ToRightPulmonaryVeins1);
    DEFINE_STATIC_STRING(RightPulmonaryCapillaries1ToGround);
    DEFINE_STATIC_STRING(RightPulmonaryVeins1ToRightIntermediatePulmonaryVeins1);
    DEFINE_STATIC_STRING(RightPulmonaryVeins1ToGround);
    DEFINE_STATIC_STRING(RightIntermediatePulmonaryVeins1ToLeftHeart2);
    DEFINE_STATIC_STRING(MainPulmonaryArteries1ToLeftIntermediatePulmonaryArteries1);
    DEFINE_STATIC_STRING(LeftIntermediatePulmonaryArteries1ToLeftPulmonaryArteries1);
    DEFINE_STATIC_STRING(LeftPulmonaryArteries1ToLeftPulmonaryVeins1);
    DEFINE_STATIC_STRING(LeftPulmonaryArteries1ToLeftPulmonaryCapillaries1);
    DEFINE_STATIC_STRING(LeftPulmonaryArteries1ToGround);
    DEFINE_STATIC_STRING(LeftPulmonaryCapillaries1ToGround);
    DEFINE_STATIC_STRING(LeftPulmonaryCapillaries1ToLeftPulmonaryVeins1);
    DEFINE_STATIC_STRING(LeftPulmonaryVeins1ToLeftIntermediatePulmonaryVeins1);
    DEFINE_STATIC_STRING(LeftPulmonaryVeins1ToGround);
    DEFINE_STATIC_STRING(LeftIntermediatePulmonaryVeins1ToLeftHeart2)
    DEFINE_STATIC_STRING(LeftHeart2ToLeftHeart1);
    DEFINE_STATIC_STRING(LeftHeart1ToLeftHeart3);
    DEFINE_STATIC_STRING(LeftHeart3ToGround);
    DEFINE_STATIC_STRING(LeftHeart1ToAorta2);
    DEFINE_STATIC_STRING(Aorta2ToAorta3);
    DEFINE_STATIC_STRING(Aorta3ToAorta1);
    DEFINE_STATIC_STRING(Aorta1ToAorta4);
    DEFINE_STATIC_STRING(Aorta1ToAorta5);

    // Abdominal Cavity
    DEFINE_STATIC_STRING(GroundToAorta4);
    DEFINE_STATIC_STRING(AbdominalCavity1ToGround);
    DEFINE_STATIC_STRING(GroundToAbdominalCavity1);
    // Brain
    DEFINE_STATIC_STRING(Aorta5ToExtracranial1);
    DEFINE_STATIC_STRING(Extracranial1ToGround);
    DEFINE_STATIC_STRING(Extracranial2ToGround);
    DEFINE_STATIC_STRING(Extracranial3ToGround);
    DEFINE_STATIC_STRING(Extracranial1ToExtracranial2);
    DEFINE_STATIC_STRING(Extracranial2ToExtracranial3);
    DEFINE_STATIC_STRING(Extracranial3ToVenaCava2);
    DEFINE_STATIC_STRING(Aorta5ToIntracranial1);
    DEFINE_STATIC_STRING(Intracranial1ToGround);
    DEFINE_STATIC_STRING(Intracranial2ToGround);
    DEFINE_STATIC_STRING(Intracranial3ToGround);
    DEFINE_STATIC_STRING(Intracranial1ToIntracranial2);
    DEFINE_STATIC_STRING(Intracranial2ToIntracranial3);
    DEFINE_STATIC_STRING(Intracranial3ToVenaCava2);
    // Gut
    DEFINE_STATIC_STRING(Aorta5ToGut1);
    DEFINE_STATIC_STRING(Gut1ToGround);
    // Large Intestine
    DEFINE_STATIC_STRING(Gut1ToLargeIntestine1);
    DEFINE_STATIC_STRING(LargeIntestine1ToGround);
    DEFINE_STATIC_STRING(LargeIntestine1ToPortalVein1);
    // Left Arm
    DEFINE_STATIC_STRING(Aorta5ToLeftArm1);
    DEFINE_STATIC_STRING(LeftArm1ToGround);
    DEFINE_STATIC_STRING(LeftArm2ToGround);
    DEFINE_STATIC_STRING(LeftArm1ToLeftArmBone1);
    DEFINE_STATIC_STRING(LeftArmBone1ToGround);
    DEFINE_STATIC_STRING(LeftArmBone1ToLeftArm2);
    DEFINE_STATIC_STRING(LeftArm1ToLeftArmFat1);
    DEFINE_STATIC_STRING(LeftArmFat1ToGround);
    DEFINE_STATIC_STRING(LeftArmFat1ToLeftArm2);
    DEFINE_STATIC_STRING(LeftArm1ToLeftArmMicrovasculature1);
    DEFINE_STATIC_STRING(LeftArmMicrovasculature1ToGround);
    DEFINE_STATIC_STRING(LeftArmMicrovasculature1ToLeftArm2);
    DEFINE_STATIC_STRING(LeftArm1ToLeftArmMuscle1);
    DEFINE_STATIC_STRING(LeftArmMuscle1ToGround);
    DEFINE_STATIC_STRING(LeftArmMuscle1ToLeftArm2);
    DEFINE_STATIC_STRING(LeftArm1ToLeftArmSkin1);
    DEFINE_STATIC_STRING(LeftArmSkin1ToGround);
    DEFINE_STATIC_STRING(LeftArmSkin1ToLeftArm2);
    DEFINE_STATIC_STRING(LeftArm2ToVenaCava2);
    // Left Kidney
    DEFINE_STATIC_STRING(Aorta5ToLeftKidney1);
    DEFINE_STATIC_STRING(LeftKidney1ToLeftKidney2);
    DEFINE_STATIC_STRING(LeftKidney1ToGround);
    DEFINE_STATIC_STRING(LeftKidney2ToGround);
    DEFINE_STATIC_STRING(LeftKidney3ToGround);
    DEFINE_STATIC_STRING(LeftKidney2ToLeftKidney3);
    DEFINE_STATIC_STRING(LeftKidney3ToVenaCava2);
    // Left Leg
    DEFINE_STATIC_STRING(Aorta5ToLeftLeg1);
    DEFINE_STATIC_STRING(LeftLeg1ToGround);
    DEFINE_STATIC_STRING(LeftLeg2ToGround);
    DEFINE_STATIC_STRING(LeftLeg1ToLeftLegBone1);
    DEFINE_STATIC_STRING(LeftLegBone1ToGround);
    DEFINE_STATIC_STRING(LeftLegBone1ToLeftLeg2);
    DEFINE_STATIC_STRING(LeftLeg1ToLeftLegFat1);
    DEFINE_STATIC_STRING(LeftLegFat1ToGround);
    DEFINE_STATIC_STRING(LeftLegFat1ToLeftLeg2);
    DEFINE_STATIC_STRING(LeftLeg1ToLeftLegMicrovasculature1);
    DEFINE_STATIC_STRING(LeftLegMicrovasculature1ToGround);
    DEFINE_STATIC_STRING(LeftLegMicrovasculature1ToLeftLeg2);
    DEFINE_STATIC_STRING(LeftLeg1ToLeftLegMuscle1);
    DEFINE_STATIC_STRING(LeftLegMuscle1ToGround);
    DEFINE_STATIC_STRING(LeftLegMuscle1ToLeftLeg2);
    DEFINE_STATIC_STRING(LeftLeg1ToLeftLegSkin1);
    DEFINE_STATIC_STRING(LeftLegSkin1ToGround);
    DEFINE_STATIC_STRING(LeftLegSkin1ToLeftLeg2);
    DEFINE_STATIC_STRING(LeftLeg2ToVenaCava2);
    // Liver
    DEFINE_STATIC_STRING(Gut1ToLiver1);
    DEFINE_STATIC_STRING(Liver1ToGround);
    DEFINE_STATIC_STRING(Liver2ToGround);
    DEFINE_STATIC_STRING(PortalVein1ToLiver1);
    DEFINE_STATIC_STRING(Liver1ToLiver2);
    DEFINE_STATIC_STRING(Liver2ToVenaCava2);
    // Myocardium
    DEFINE_STATIC_STRING(Aorta5ToMyocardium1);
    DEFINE_STATIC_STRING(Myocardium1ToMyocardium2);
    DEFINE_STATIC_STRING(Myocardium1ToGround);
    DEFINE_STATIC_STRING(Myocardium2ToGround);
    DEFINE_STATIC_STRING(Myocardium3ToGround);
    DEFINE_STATIC_STRING(Myocardium2ToMyocardium3);
    DEFINE_STATIC_STRING(Myocardium3ToVenaCava2);
    // Pericardium
    DEFINE_STATIC_STRING(Pericardium1ToGround);
    DEFINE_STATIC_STRING(GroundToPericardium1);
    // Right Arm
    DEFINE_STATIC_STRING(Aorta5ToRightArm1);
    DEFINE_STATIC_STRING(RightArm1ToGround);
    DEFINE_STATIC_STRING(RightArm2ToGround);
    DEFINE_STATIC_STRING(RightArm1ToRightArmBone1);
    DEFINE_STATIC_STRING(RightArmBone1ToGround);
    DEFINE_STATIC_STRING(RightArmBone1ToRightArm2);
    DEFINE_STATIC_STRING(RightArm1ToRightArmFat1);
    DEFINE_STATIC_STRING(RightArmFat1ToGround);
    DEFINE_STATIC_STRING(RightArmFat1ToRightArm2);
    DEFINE_STATIC_STRING(RightArm1ToRightArmMicrovasculature1);
    DEFINE_STATIC_STRING(RightArmMicrovasculature1ToGround);
    DEFINE_STATIC_STRING(RightArmMicrovasculature1ToRightArm2);
    DEFINE_STATIC_STRING(RightArm1ToRightArmMuscle1);
    DEFINE_STATIC_STRING(RightArmMuscle1ToGround);
    DEFINE_STATIC_STRING(RightArmMuscle1ToRightArm2);
    DEFINE_STATIC_STRING(RightArm1ToRightArmSkin1);
    DEFINE_STATIC_STRING(RightArmSkin1ToGround);
    DEFINE_STATIC_STRING(RightArmSkin1ToRightArm2);
    DEFINE_STATIC_STRING(RightArm2ToVenaCava2);
    // Right Kidney
    DEFINE_STATIC_STRING(Aorta5ToRightKidney1);
    DEFINE_STATIC_STRING(RightKidney1ToRightKidney2);
    DEFINE_STATIC_STRING(RightKidney1ToGround);
    DEFINE_STATIC_STRING(RightKidney2ToGround);
    DEFINE_STATIC_STRING(RightKidney3ToGround);
    DEFINE_STATIC_STRING(RightKidney2ToRightKidney3);
    DEFINE_STATIC_STRING(RightKidney3ToVenaCava2);
    // Right Leg
    DEFINE_STATIC_STRING(Aorta5ToRightLeg1);
    DEFINE_STATIC_STRING(RightLeg1ToGround);
    DEFINE_STATIC_STRING(RightLeg2ToGround);
    DEFINE_STATIC_STRING(RightLeg1ToRightLegBone1);
    DEFINE_STATIC_STRING(RightLegBone1ToGround);
    DEFINE_STATIC_STRING(RightLegBone1ToRightLeg2);
    DEFINE_STATIC_STRING(RightLeg1ToRightLegFat1);
    DEFINE_STATIC_STRING(RightLegFat1ToGround);
    DEFINE_STATIC_STRING(RightLegFat1ToRightLeg2);
    DEFINE_STATIC_STRING(RightLeg1ToRightLegMicrovasculature1);
    DEFINE_STATIC_STRING(RightLegMicrovasculature1ToGround);
    DEFINE_STATIC_STRING(RightLegMicrovasculature1ToRightLeg2);
    DEFINE_STATIC_STRING(RightLeg1ToRightLegMuscle1);
    DEFINE_STATIC_STRING(RightLegMuscle1ToGround);
    DEFINE_STATIC_STRING(RightLegMuscle1ToRightLeg2);
    DEFINE_STATIC_STRING(RightLeg1ToRightLegSkin1);
    DEFINE_STATIC_STRING(RightLegSkin1ToGround);
    DEFINE_STATIC_STRING(RightLegSkin1ToRightLeg2);
    DEFINE_STATIC_STRING(RightLeg2ToVenaCava2);
    // Small Intestine
    DEFINE_STATIC_STRING(Gut1ToSmallIntestine1);
    DEFINE_STATIC_STRING(SmallIntestine1ToGround);
    DEFINE_STATIC_STRING(SmallIntestine1ToPortalVein1);
    // Splanchnic
    DEFINE_STATIC_STRING(Gut1ToSplanchnic1);
    DEFINE_STATIC_STRING(Splanchnic1ToGround);
    DEFINE_STATIC_STRING(Splanchnic1ToPortalVein1);
    // Spleen
    DEFINE_STATIC_STRING(Gut1ToSpleen1);
    DEFINE_STATIC_STRING(Spleen1ToGround);
    DEFINE_STATIC_STRING(Spleen1ToPortalVein1);
    // Torso
    DEFINE_STATIC_STRING(Aorta5ToTorso1);
    DEFINE_STATIC_STRING(Torso1ToGround);
    DEFINE_STATIC_STRING(Torso2ToGround);
    DEFINE_STATIC_STRING(Torso1ToTorsoBone1);
    DEFINE_STATIC_STRING(TorsoBone1ToGround);
    DEFINE_STATIC_STRING(TorsoBone1ToTorso2);
    DEFINE_STATIC_STRING(Torso1ToTorsoFat1);
    DEFINE_STATIC_STRING(TorsoFat1ToGround);
    DEFINE_STATIC_STRING(TorsoFat1ToTorso2);
    DEFINE_STATIC_STRING(Torso1ToTorsoMicrovasculature1);
    DEFINE_STATIC_STRING(TorsoMicrovasculature1ToGround);
    DEFINE_STATIC_STRING(TorsoMicrovasculature1ToTorso2);
    DEFINE_STATIC_STRING(Torso1ToTorsoMuscle1);
    DEFINE_STATIC_STRING(TorsoMuscle1ToGround);
    DEFINE_STATIC_STRING(TorsoMuscle1ToTorso2);
    DEFINE_STATIC_STRING(Torso1ToTorsoSkin1);
    DEFINE_STATIC_STRING(TorsoSkin1ToGround);
    DEFINE_STATIC_STRING(TorsoSkin1ToTorso2);
    DEFINE_STATIC_STRING(Torso2ToVenaCava2);
    // Vena Cava
    DEFINE_STATIC_STRING(VenaCava2ToVenaCava1);
    DEFINE_STATIC_STRING(VenaCava1ToGround);
    //DEFINE_STATIC_STRING(VenaCava2ToGround);
    DEFINE_STATIC_STRING(IVToVenaCava1);
  };


  ///////////////////////////////////////
  // Cerebrospinal Fluid Circuit Enums //
  ///////////////////////////////////////

  class CerebrospinalFluidNode
  {
  public:
    DEFINE_STATIC_STRING(VascularCSFBarrier);
    DEFINE_STATIC_STRING(IntracranialSpace1);
    DEFINE_STATIC_STRING(IntracranialSpace2);
  };

  class CerebrospinalFluidPath
  {
  public:

    DEFINE_STATIC_STRING(IntracranialSpace2ToGround);
    DEFINE_STATIC_STRING(GroundToIntracranialSpace1);
    DEFINE_STATIC_STRING(Brain1ToVascularCSFBarrier);
    DEFINE_STATIC_STRING(VascularCSFBarrierToIntracranialSpace1);
    DEFINE_STATIC_STRING(IntracranialSpace1ToIntracranialSpace2);
  };

  class TissueNode
  {
  public:
    DEFINE_STATIC_STRING(BoneT1);
    DEFINE_STATIC_STRING(BoneT2);
    DEFINE_STATIC_STRING(BoneT3);
    DEFINE_STATIC_STRING(BrainT1);
    DEFINE_STATIC_STRING(BrainT2);
    DEFINE_STATIC_STRING(BrainT3);
    DEFINE_STATIC_STRING(FatT1);
    DEFINE_STATIC_STRING(FatT2);
    DEFINE_STATIC_STRING(FatT3);
    DEFINE_STATIC_STRING(GutT1);
    DEFINE_STATIC_STRING(GutT2);
    DEFINE_STATIC_STRING(GutT3);
    DEFINE_STATIC_STRING(LiverT1);
    DEFINE_STATIC_STRING(LiverT2);
    DEFINE_STATIC_STRING(LiverT3);
    DEFINE_STATIC_STRING(LeftKidneyT1);
    DEFINE_STATIC_STRING(LeftKidneyT2);
    DEFINE_STATIC_STRING(LeftKidneyT3);
    DEFINE_STATIC_STRING(LeftLungT1);
    DEFINE_STATIC_STRING(LeftLungT2);
    DEFINE_STATIC_STRING(LeftLungT3);
    DEFINE_STATIC_STRING(Lymph);
    DEFINE_STATIC_STRING(MuscleT1);
    DEFINE_STATIC_STRING(MuscleT2);
    DEFINE_STATIC_STRING(MuscleT3);
    DEFINE_STATIC_STRING(MyocardiumT1);
    DEFINE_STATIC_STRING(MyocardiumT2);
    DEFINE_STATIC_STRING(MyocardiumT3);
    DEFINE_STATIC_STRING(RightLungT1);
    DEFINE_STATIC_STRING(RightLungT2);
    DEFINE_STATIC_STRING(RightLungT3);
    DEFINE_STATIC_STRING(RightKidneyT1);
    DEFINE_STATIC_STRING(RightKidneyT2);
    DEFINE_STATIC_STRING(RightKidneyT3);
    DEFINE_STATIC_STRING(SkinT1);
    DEFINE_STATIC_STRING(SkinT2);
    DEFINE_STATIC_STRING(SkinT3);
    DEFINE_STATIC_STRING(SpleenT1);
    DEFINE_STATIC_STRING(SpleenT2);
    DEFINE_STATIC_STRING(SpleenT3);
  };

  class TissuePath
  {
  public:
    DEFINE_STATIC_STRING(Bone1ToBoneT2);
    DEFINE_STATIC_STRING(BoneT2ToBoneT1);
    DEFINE_STATIC_STRING(BoneT1ToBoneT3);
    DEFINE_STATIC_STRING(GroundToBoneT3);
    DEFINE_STATIC_STRING(BoneT1ToLymph);

    DEFINE_STATIC_STRING(Brain1ToBrainT2);
    DEFINE_STATIC_STRING(BrainT2ToBrainT1);
    DEFINE_STATIC_STRING(BrainT1ToBrainT3);
    DEFINE_STATIC_STRING(GroundToBrainT3);
    DEFINE_STATIC_STRING(BrainT1ToLymph);

    DEFINE_STATIC_STRING(Fat1ToFatT2);
    DEFINE_STATIC_STRING(FatT2ToFatT1);
    DEFINE_STATIC_STRING(FatT1ToFatT3);
    DEFINE_STATIC_STRING(GroundToFatT3);
    DEFINE_STATIC_STRING(FatT1ToLymph);

    DEFINE_STATIC_STRING(SmallIntestine1ToGutT2);
    DEFINE_STATIC_STRING(LargeIntestine1ToGutT2);
    DEFINE_STATIC_STRING(Splanchnic1ToGutT2);
    DEFINE_STATIC_STRING(GutT2ToGutT1);
    DEFINE_STATIC_STRING(GutT1ToGutT3);
    DEFINE_STATIC_STRING(GroundToGutT3);
    DEFINE_STATIC_STRING(GutT1ToLymph);

    DEFINE_STATIC_STRING(Liver1ToLiverT2);
    DEFINE_STATIC_STRING(LiverT2ToLiverT1);
    DEFINE_STATIC_STRING(LiverT1ToLiverT3);
    DEFINE_STATIC_STRING(GroundToLiverT3);
    DEFINE_STATIC_STRING(LiverT1ToLymph);

    DEFINE_STATIC_STRING(LeftKidney1ToLeftKidneyT2);
    DEFINE_STATIC_STRING(LeftKidneyT2ToLeftKidneyT1);
    DEFINE_STATIC_STRING(LeftKidneyT1ToLeftKidneyT3);
    DEFINE_STATIC_STRING(GroundToLeftKidneyT3);
    DEFINE_STATIC_STRING(LeftKidneyT1ToLymph);

    DEFINE_STATIC_STRING(LeftLung1ToLeftLungT2);
    DEFINE_STATIC_STRING(LeftLungT2ToLeftLungT1);
    DEFINE_STATIC_STRING(LeftLungT1ToLeftLungT3);
    DEFINE_STATIC_STRING(GroundToLeftLungT3);
    DEFINE_STATIC_STRING(LeftLungT1ToLymph);

    DEFINE_STATIC_STRING(LymphToVenaCava1);

    DEFINE_STATIC_STRING(Muscle1ToMuscleT2);
    DEFINE_STATIC_STRING(MuscleT2ToMuscleT1);
    DEFINE_STATIC_STRING(MuscleT1ToMuscleT3);
    DEFINE_STATIC_STRING(GroundToMuscleT3);
    DEFINE_STATIC_STRING(MuscleT1ToLymph);

    DEFINE_STATIC_STRING(Myocardium1ToMyocardiumT2);
    DEFINE_STATIC_STRING(MyocardiumT2ToMyocardiumT1);
    DEFINE_STATIC_STRING(MyocardiumT1ToMyocardiumT3);
    DEFINE_STATIC_STRING(GroundToMyocardiumT3);
    DEFINE_STATIC_STRING(MyocardiumT1ToLymph);

    DEFINE_STATIC_STRING(RightLung1ToRightLungT2);
    DEFINE_STATIC_STRING(RightLungT2ToRightLungT1);
    DEFINE_STATIC_STRING(RightLungT1ToRightLungT3);
    DEFINE_STATIC_STRING(GroundToRightLungT3);
    DEFINE_STATIC_STRING(RightLungT1ToLymph);

    DEFINE_STATIC_STRING(RightKidney1ToRightKidneyT2);
    DEFINE_STATIC_STRING(RightKidneyT2ToRightKidneyT1);
    DEFINE_STATIC_STRING(RightKidneyT1ToRightKidneyT3);
    DEFINE_STATIC_STRING(GroundToRightKidneyT3);
    DEFINE_STATIC_STRING(RightKidneyT1ToLymph);

    DEFINE_STATIC_STRING(Skin1ToSkinT2);
    DEFINE_STATIC_STRING(SkinT2ToSkinT1);
    DEFINE_STATIC_STRING(SkinT1ToSkinT3);
    DEFINE_STATIC_STRING(GroundToSkinT3);
    DEFINE_STATIC_STRING(SkinT1ToLymph);
    DEFINE_STATIC_STRING(SkinSweating);

    DEFINE_STATIC_STRING(Spleen1ToSpleenT2);
    DEFINE_STATIC_STRING(SpleenT2ToSpleenT1);
    DEFINE_STATIC_STRING(SpleenT1ToSpleenT3);
    DEFINE_STATIC_STRING(GroundToSpleenT3);
    DEFINE_STATIC_STRING(SpleenT1ToLymph);
  };

  class ExpandedCardiovascularTissuePath
  {
  public:
    DEFINE_STATIC_STRING(TorsoBone1ToBoneT2);
    DEFINE_STATIC_STRING(LeftArmBone1ToBoneT2);
    DEFINE_STATIC_STRING(LeftLegBone1ToBoneT2);
    DEFINE_STATIC_STRING(RightArmBone1ToBoneT2);
    DEFINE_STATIC_STRING(RightLegBone1ToBoneT2);
    DEFINE_STATIC_STRING(BoneT2ToBoneT1);
    DEFINE_STATIC_STRING(BoneT1ToBoneT3);
    DEFINE_STATIC_STRING(GroundToBoneT3);
    DEFINE_STATIC_STRING(BoneT1ToLymph);

    DEFINE_STATIC_STRING(Intracranial2ToBrainT2);
    DEFINE_STATIC_STRING(BrainT2ToBrainT1);
    DEFINE_STATIC_STRING(BrainT1ToBrainT3);
    DEFINE_STATIC_STRING(GroundToBrainT3);
    DEFINE_STATIC_STRING(BrainT1ToLymph);

    DEFINE_STATIC_STRING(TorsoFat1ToFatT2);
    DEFINE_STATIC_STRING(LeftArmFat1ToFatT2);
    DEFINE_STATIC_STRING(LeftLegFat1ToFatT2);
    DEFINE_STATIC_STRING(RightArmFat1ToFatT2);
    DEFINE_STATIC_STRING(RightLegFat1ToFatT2);
    DEFINE_STATIC_STRING(FatT2ToFatT1);
    DEFINE_STATIC_STRING(FatT1ToFatT3);
    DEFINE_STATIC_STRING(GroundToFatT3);
    DEFINE_STATIC_STRING(FatT1ToLymph);

    DEFINE_STATIC_STRING(SmallIntestine1ToGutT2);
    DEFINE_STATIC_STRING(LargeIntestine1ToGutT2);
    DEFINE_STATIC_STRING(Splanchnic1ToGutT2);
    DEFINE_STATIC_STRING(GutT2ToGutT1);
    DEFINE_STATIC_STRING(GutT1ToGutT3);
    DEFINE_STATIC_STRING(GroundToGutT3);
    DEFINE_STATIC_STRING(GutT1ToLymph);

    DEFINE_STATIC_STRING(Liver1ToLiverT2);
    DEFINE_STATIC_STRING(LiverT2ToLiverT1);
    DEFINE_STATIC_STRING(LiverT1ToLiverT3);
    DEFINE_STATIC_STRING(GroundToLiverT3);
    DEFINE_STATIC_STRING(LiverT1ToLymph);

    DEFINE_STATIC_STRING(LeftKidney2ToLeftKidneyT2);
    DEFINE_STATIC_STRING(LeftKidneyT2ToLeftKidneyT1);
    DEFINE_STATIC_STRING(LeftKidneyT1ToLeftKidneyT3);
    DEFINE_STATIC_STRING(GroundToLeftKidneyT3);
    DEFINE_STATIC_STRING(LeftKidneyT1ToLymph);

    DEFINE_STATIC_STRING(LeftLung1ToLeftLungT2);
    DEFINE_STATIC_STRING(LeftLungT2ToLeftLungT1);
    DEFINE_STATIC_STRING(LeftLungT1ToLeftLungT3);
    DEFINE_STATIC_STRING(GroundToLeftLungT3);
    DEFINE_STATIC_STRING(LeftLungT1ToLymph);

    DEFINE_STATIC_STRING(LymphToVenaCava);

    DEFINE_STATIC_STRING(TorsoMuscle1ToMuscleT2);
    DEFINE_STATIC_STRING(LeftArmMuscle1ToMuscleT2);
    DEFINE_STATIC_STRING(LeftLegMuscle1ToMuscleT2);
    DEFINE_STATIC_STRING(RightArmMuscle1ToMuscleT2);
    DEFINE_STATIC_STRING(RightLegMuscle1ToMuscleT2);
    DEFINE_STATIC_STRING(MuscleT2ToMuscleT1);
    DEFINE_STATIC_STRING(MuscleT1ToMuscleT3);
    DEFINE_STATIC_STRING(GroundToMuscleT3);
    DEFINE_STATIC_STRING(MuscleT1ToLymph);

    DEFINE_STATIC_STRING(Myocardium2ToMyocardiumT2);
    DEFINE_STATIC_STRING(MyocardiumT2ToMyocardiumT1);
    DEFINE_STATIC_STRING(MyocardiumT1ToMyocardiumT3);
    DEFINE_STATIC_STRING(GroundToMyocardiumT3);
    DEFINE_STATIC_STRING(MyocardiumT1ToLymph);

    DEFINE_STATIC_STRING(RightLung1ToRightLungT2);
    DEFINE_STATIC_STRING(RightLungT2ToRightLungT1);
    DEFINE_STATIC_STRING(RightLungT1ToRightLungT3);
    DEFINE_STATIC_STRING(GroundToRightLungT3);
    DEFINE_STATIC_STRING(RightLungT1ToLymph);

    DEFINE_STATIC_STRING(RightKidney2ToRightKidneyT2);
    DEFINE_STATIC_STRING(RightKidneyT2ToRightKidneyT1);
    DEFINE_STATIC_STRING(RightKidneyT1ToRightKidneyT3);
    DEFINE_STATIC_STRING(GroundToRightKidneyT3);
    DEFINE_STATIC_STRING(RightKidneyT1ToLymph);

    DEFINE_STATIC_STRING(TorsoSkin1ToSkinT2);
    DEFINE_STATIC_STRING(LeftArmSkin1ToSkinT2);
    DEFINE_STATIC_STRING(LeftLegSkin1ToSkinT2);
    DEFINE_STATIC_STRING(RightArmSkin1ToSkinT2);
    DEFINE_STATIC_STRING(RightLegSkin1ToSkinT2);
    DEFINE_STATIC_STRING(SkinT2ToSkinT1);
    DEFINE_STATIC_STRING(SkinT1ToSkinT3);
    DEFINE_STATIC_STRING(GroundToSkinT3);
    DEFINE_STATIC_STRING(SkinT1ToLymph);
    DEFINE_STATIC_STRING(SkinSweating);

    DEFINE_STATIC_STRING(Spleen1ToSpleenT2);
    DEFINE_STATIC_STRING(SpleenT2ToSpleenT1);
    DEFINE_STATIC_STRING(SpleenT1ToSpleenT3);
    DEFINE_STATIC_STRING(GroundToSpleenT3);
    DEFINE_STATIC_STRING(SpleenT1ToLymph);
  };

  class RenalNode
  {
  public:
    // Blood
    DEFINE_STATIC_STRING(RightAortaConnection);
    DEFINE_STATIC_STRING(RightRenalArtery);
    DEFINE_STATIC_STRING(RightAfferentArteriole);
    DEFINE_STATIC_STRING(RightGlomerularCapillaries);
    DEFINE_STATIC_STRING(RightNetGlomerularCapillaries);
    DEFINE_STATIC_STRING(RightEfferentArteriole);
    DEFINE_STATIC_STRING(RightPeritubularCapillaries);
    DEFINE_STATIC_STRING(RightNetPeritubularCapillaries);
    DEFINE_STATIC_STRING(RightRenalVein);
    DEFINE_STATIC_STRING(RightVenaCavaConnection);
    // Urine
    DEFINE_STATIC_STRING(RightBowmansCapsules);
    DEFINE_STATIC_STRING(RightNetBowmansCapsules);
    DEFINE_STATIC_STRING(RightTubules);
    DEFINE_STATIC_STRING(RightNetTubules);
    DEFINE_STATIC_STRING(RightUreter);

    // Blood
    DEFINE_STATIC_STRING(LeftAortaConnection);
    DEFINE_STATIC_STRING(LeftRenalArtery);
    DEFINE_STATIC_STRING(LeftAfferentArteriole);
    DEFINE_STATIC_STRING(LeftGlomerularCapillaries);
    DEFINE_STATIC_STRING(LeftNetGlomerularCapillaries);
    DEFINE_STATIC_STRING(LeftEfferentArteriole);
    DEFINE_STATIC_STRING(LeftPeritubularCapillaries);
    DEFINE_STATIC_STRING(LeftNetPeritubularCapillaries);
    DEFINE_STATIC_STRING(LeftRenalVein);
    DEFINE_STATIC_STRING(LeftVenaCavaConnection);
    // Urine
    DEFINE_STATIC_STRING(LeftNetBowmansCapsules);
    DEFINE_STATIC_STRING(LeftBowmansCapsules);
    DEFINE_STATIC_STRING(LeftTubules);
    DEFINE_STATIC_STRING(LeftNetTubules);
    DEFINE_STATIC_STRING(LeftUreter);

    DEFINE_STATIC_STRING(Bladder);

    DEFINE_STATIC_STRING(Ground);
  };

  class RenalPath
  {
  public:
    DEFINE_STATIC_STRING(RightAortaConnectionToRenalArtery);
    DEFINE_STATIC_STRING(RightRenalArteryToAfferentArteriole);
    DEFINE_STATIC_STRING(RightRenalArteryCompliance);
    DEFINE_STATIC_STRING(RightAfferentArterioleToGlomerularCapillaries);
    DEFINE_STATIC_STRING(RightGlomerularCapillariesToEfferentArteriole);
    DEFINE_STATIC_STRING(RightGlomerularCapillariesCompliance);
    DEFINE_STATIC_STRING(RightEfferentArterioleToPeritubularCapillaries);
    DEFINE_STATIC_STRING(RightPeritubularCapillariesToRenalVein);
    DEFINE_STATIC_STRING(RightRenalVeinToVenaCavaConnection);
    DEFINE_STATIC_STRING(RightRenalVeinCompliance);

    DEFINE_STATIC_STRING(RightGlomerularCapillariesToNetGlomerularCapillaries);
    DEFINE_STATIC_STRING(RightNetGlomerularCapillariesToNetBowmansCapsules);
    DEFINE_STATIC_STRING(RightBowmansCapsulesToNetBowmansCapsules);
    DEFINE_STATIC_STRING(RightBowmansCapsulesToTubules);
    DEFINE_STATIC_STRING(RightTubulesToUreter);
    DEFINE_STATIC_STRING(RightTubulesToNetTubules);
    DEFINE_STATIC_STRING(RightNetTubulesToNetPeritubularCapillaries);
    DEFINE_STATIC_STRING(RightPeritubularCapillariesToNetPeritubularCapillaries);
    DEFINE_STATIC_STRING(RightUreterToBladder);

    DEFINE_STATIC_STRING(LeftAortaConnectionToRenalArtery);
    DEFINE_STATIC_STRING(LeftRenalArteryToAfferentArteriole);
    DEFINE_STATIC_STRING(LeftRenalArteryCompliance);
    DEFINE_STATIC_STRING(LeftAfferentArterioleToGlomerularCapillaries);
    DEFINE_STATIC_STRING(LeftGlomerularCapillariesToEfferentArteriole);
    DEFINE_STATIC_STRING(LeftGlomerularCapillariesCompliance);
    DEFINE_STATIC_STRING(LeftEfferentArterioleToPeritubularCapillaries);
    DEFINE_STATIC_STRING(LeftPeritubularCapillariesToRenalVein);
    DEFINE_STATIC_STRING(LeftRenalVeinToVenaCavaConnection);
    DEFINE_STATIC_STRING(LeftRenalVeinCompliance);

    DEFINE_STATIC_STRING(LeftGlomerularCapillariesToNetGlomerularCapillaries);
    DEFINE_STATIC_STRING(LeftNetGlomerularCapillariesToNetBowmansCapsules);
    DEFINE_STATIC_STRING(LeftBowmansCapsulesToNetBowmansCapsules);
    DEFINE_STATIC_STRING(LeftBowmansCapsulesToTubules);
    DEFINE_STATIC_STRING(LeftTubulesToUreter);
    DEFINE_STATIC_STRING(LeftTubulesToNetTubules);
    DEFINE_STATIC_STRING(LeftNetTubulesToNetPeritubularCapillaries);
    DEFINE_STATIC_STRING(LeftPeritubularCapillariesToNetPeritubularCapillaries);
    DEFINE_STATIC_STRING(LeftUreterToBladder);

    DEFINE_STATIC_STRING(BladderToGroundPressure);
    DEFINE_STATIC_STRING(BladderToGroundUrinate);
  };

  class DigestionNode
  {
  public:
    DEFINE_STATIC_STRING(GutChyme);
  };

  class DigestionPath
  {
  public:
    DEFINE_STATIC_STRING(GutChymeToSmallIntestineVascular);
  };

  class ECMONode
  {
  public:
    DEFINE_STATIC_STRING(BloodSamplingPort);
    DEFINE_STATIC_STRING(Oxygenator);
  };

  class ECMOPath
  {
  public:
    DEFINE_STATIC_STRING(VascularToBloodSamplingPort);
    DEFINE_STATIC_STRING(BloodSamplingPortToGround);
    DEFINE_STATIC_STRING(GroundToOxygenator);
    DEFINE_STATIC_STRING(OxygenatorToVasculature);
  };
END_NAMESPACE
