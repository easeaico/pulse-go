/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/PulseConfiguration.h"
#include "engine/common/controller/CircuitManager.h"
#include "engine/common/controller/CompartmentManager.h"
#include "engine/common/controller/SubstanceManager.h"
#include "engine/common/system/physiology/EnergyModel.h"
// Conditions
#include "cdm/engine/SEConditionManager.h"
#include "cdm/patient/conditions/SEConsumeMeal.h"
#include "cdm/patient/conditions/SEDehydration.h"
// Actions
#include "cdm/engine/SEActionManager.h"
#include "cdm/engine/SEPatientActionCollection.h"
#include "cdm/patient/actions/SEExercise.h"
// Dependent Systems
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
// CDM
#include "cdm/patient/SEPatient.h"
#include "cdm/patient/SEMeal.h"
#include "cdm/patient/SENutrition.h"
#include "cdm/engine/SEEventManager.h"
#include "cdm/circuit/fluid/SEFluidCircuit.h"
#include "cdm/circuit/thermal/SEThermalCircuit.h"
#include "cdm/circuit/thermal/SEThermalCircuitNode.h"
#include "cdm/circuit/thermal/SEThermalCircuitPath.h"
#include "cdm/circuit/thermal/SEThermalCircuitCalculator.h"
#include "cdm/compartment/fluid/SELiquidCompartment.h"
#include "cdm/compartment/substances/SELiquidSubstanceQuantity.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarLength.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerTime.h"
#include "cdm/properties/SEScalarPower.h"
#include "cdm/properties/SEScalarTemperature.h"
#include "cdm/properties/SEScalarArea.h"
#include "cdm/properties/SEScalarHeatCapacitancePerMass.h"
#include "cdm/properties/SEScalarHeatCapacitance.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarHeatResistance.h"
#include "cdm/properties/SEScalarHeatConductance.h"
#include "cdm/properties/SEScalarEnergyPerMass.h"
#include "cdm/properties/SEScalarEnergy.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarEnergyPerAmount.h"
#include "cdm/properties/SEScalarAmountPerVolume.h"
#include "cdm/properties/SEScalarAmountPerTime.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarAmountPerTime.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolumePerTimeMass.h"
#include "cdm/properties/SERunningAverage.h"
#include "substance/SESubstanceCompound.h"

namespace pulse
{
  EnergyModel::EnergyModel(Data& data) : EnergySystem(data.GetLogger()), Model(data)
  {
    m_BloodpH = new SERunningAverage();
    m_BicarbonateMolarity_mmol_Per_L = new SERunningAverage();
    m_circuitCalculator = new SEThermalCircuitCalculator(DefaultThermalCircuitCalculatorUnits, GetLogger());
    Clear();
  }

  EnergyModel::~EnergyModel()
  {
    Clear();
    delete m_BloodpH;
    delete m_BicarbonateMolarity_mmol_Per_L;
  }

  void EnergyModel::Clear()
  {
    EnergySystem::Clear();
    m_AortaHCO3 = nullptr;
    m_coreNode = nullptr;
    m_skinNode = nullptr;
    m_groundToCorePath = nullptr;
    m_coreToSkinPath = nullptr;
    m_skinExtravascularToSweatingGroundPath = nullptr;
    m_InternalTemperatureCircuit = nullptr;
    m_TemperatureCircuit = nullptr;

    m_BloodpH->Invalidate();
    m_BicarbonateMolarity_mmol_Per_L->Invalidate();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Initializes system properties to valid homeostatic values.
  ///
  /// \details
  /// For stabilization only!
  /// Called AFTER Setup when stabilizing a new patient
  //--------------------------------------------------------------------------------------------------
  void EnergyModel::Initialize()
  {
    Model::Initialize();

    GetTotalMetabolicRate().Set(m_data.GetCurrentPatient().GetBasalMetabolicRate());
    //Initialization of other system variables
    /// \cite herman2008physics
    GetCoreTemperature().SetValue(37.0, TemperatureUnit::C);
    GetSkinTemperature().SetValue(33.0, TemperatureUnit::C);
    /// \cite phypers2006lactate
    GetLactateProductionRate().SetValue(1.3, AmountPerTimeUnit::mol_Per_day);
    /// \cite garber1974ketone
    GetKetoneProductionRate().SetValue(300.0, AmountPerTimeUnit::umol_Per_min);
    /// \cite guyton2006medical
    GetExerciseMeanArterialPressureDelta().SetValue(0.0, PressureUnit::mmHg);
    GetTotalWorkRateLevel().SetValue(0.0);
    GetFatigueLevel().SetValue(0.0);

    //Running average quantities used to trigger events
    m_BloodpH->Sample(7.4); //Initialize
    m_BicarbonateMolarity_mmol_Per_L->Sample(24.0); //Initialize

    // Energy buckets for fatigue
    m_UsableEnergyStore_J = 2600.0;
    m_PeakPowerEnergyStore_J = 4200.0;
    m_MediumPowerEnergyStore_J = 35000.0;
    m_EnduranceEnergyStore_J = 400000.0;
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Initializes the energy specific quantities
  ///
  /// \details
  /// Called during both State loading and Patient Stabilization
  /// Pull and setup up our data (can be from other systems)
  /// Initialize will be called after this and can overwrite any of this data (only if stabilizing)
  ///
  /// The energy local member variables, system level quantities, and metabolic-specific substance
  /// quantities are initialized here. These variables are used in the numerous metabolic and thermal regulation
  /// functions contained in the energy system. Encompassing them in this function allows for easy initialization
  /// either at the beginning of a simulation or after a system crash
  //--------------------------------------------------------------------------------------------------
  void EnergyModel::SetUp()
  {
    m_SkinExtracellular = m_data.GetCompartments().GetLiquidCompartment(pulse::ExtravascularCompartment::SkinExtracellular);
    m_AortaHCO3 = m_data.GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Aorta)->GetSubstanceQuantity(m_data.GetSubstances().GetHCO3());

    //Circuit elements
    //Circuits
    m_TemperatureCircuit = &m_data.GetCircuits().GetTemperatureCircuit();
    m_InternalTemperatureCircuit = &m_data.GetCircuits().GetInternalTemperatureCircuit();
    //Nodes
    m_coreNode = m_InternalTemperatureCircuit->GetNode(pulse::InternalTemperatureNode::InternalCore);
    m_skinNode = m_InternalTemperatureCircuit->GetNode(pulse::InternalTemperatureNode::InternalSkin);
    //Paths
    m_groundToCorePath = m_InternalTemperatureCircuit->GetPath(pulse::InternalTemperaturePath::GroundToInternalCore); //Heat source
    m_coreToGroundPath = m_InternalTemperatureCircuit->GetPath(pulse::InternalTemperaturePath::InternalCoreToGround); //Capacitance
    m_coreToSkinPath = m_InternalTemperatureCircuit->GetPath(pulse::InternalTemperaturePath::InternalCoreToInternalSkin);
    m_skinExtravascularToSweatingGroundPath = m_data.GetCircuits().GetActiveCardiovascularCircuit().GetPath(pulse::TissuePath::SkinSweating);
    //Substances
    m_Sweat = m_data.GetSubstances().GetCompound("Sweat");
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Applies an imposed dehydration time to the current fluid stores
  ///
  /// \details
  /// Starvation uses a coarse time-step approach with an assumed time-step of one day. This coarse time-step
  /// is used to to manually decrement all of the fluid stores which represents the loss due to urination, sweating, and other mechanisms.
  /// For time-steps longer than one day there is a reduced fluid loss.
  //--------------------------------------------------------------------------------------------------
  //void EnergyModel::Dehydration(double time)
  //{
  //  Warning("Consume Dehydration is currently disabled.");
  //  return;
  //  //Fluid loss is approximately 2600 mL/day. Shirreffs SM. Markers of hydration status. J Sports Med Phys Fitness 2000;40:80-4. 
  //  //After the first day without fluid intake, the renal loss decreases by 1000 mL leading to a total fluid loss of 1600 mL/day after day one.
  //  double fluidLossRate_mL_Per_day = 2600;
  //  double fluidLossRate_mL_Per_day2 = 1600;
  //  double coarseTimeStep_days = time;
  //  double fluidLoss_mL = 0.0;
  //
  //  if (coarseTimeStep_days<0.0)
  //    Error("Cannot specify a negative time since last meal. Time since last meal is now set to zero.");
  //
  //  if (coarseTimeStep_days > 1.0)
  //    fluidLoss_mL = fluidLossRate_mL_Per_day + fluidLossRate_mL_Per_day2*(coarseTimeStep_days - 1.0);
  //  else
  //    fluidLoss_mL = fluidLossRate_mL_Per_day*coarseTimeStep_days;
  //
  //  SEScalarVolume fluidDehydrated;
  //  fluidDehydrated.SetValue(-fluidLoss_mL, VolumeUnit::mL);
  //
  //  // How do we take fluid from the body m_data.GetCircuits().DistributeVolume(fluidDehydrated);
  //
  //  //Patient weight decrease due to fluid mass lost
  //  double patientMassLost_kg = m_data.GetConfiguration().GetWaterDensity(MassPerVolumeUnit::kg_Per_mL)*fluidLoss_mL;
  //  m_Patient->GetWeight().Increment(-patientMassLost_kg, MassUnit::kg);
  //}

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Applies an imposed starvation time to the current nutrient stores
  ///
  /// \details
  /// Starvation uses a coarse time-step approach with an assumed time-step of one day. This coarse time-step
  /// is used to to manually decrement all of the nutrient stores which represents the loss due to metabolic consumption.
  /// For time durations greater than one day, there is a decrease in the basal metabolic rate due to mass loss from increased
  /// starvation times.
  //--------------------------------------------------------------------------------------------------
  // Starvation currently disabled
  //void EnergyModel::Starvation(double time)
  //{
  //  if (time < 1.0) //Less than one day without food. Decrement according to consumption/production methodology
  //  {
  //    //CalculateNutrientConsumptionAndProduction(Convert(time, TimeUnit::day, TimeUnit::s)); //TODO:: Add in clearances if they are needed
  //    //BalanceSubstances();
  //  }
  //  else
  //  {
  //    unsigned int numTimeSteps = (int)time;
  //    double deltaT_days = 1.0;
  //    for (unsigned int iStep = 0; iStep < numTimeSteps; iStep++)
  //    {
  //      CalculateNutrientConsumptionAndProduction(Convert(deltaT_days, TimeUnit::day, TimeUnit::s));
  //      CalculateBasalMetabolicRate();
  //      GetTotalMetabolicRate().SetValue(m_PatientBMR_kcal_Per_day, PowerUnit::kcal_Per_day); //Metabolic rate decrease due to reduction in body mass
  //      //BalanceSubstances();
  //    }
  //  }
  //}


  void EnergyModel::AtSteadyState()
  {
    if (m_data.GetState() == EngineState::AtInitialStableState)
    {
      //TODO: Get consume meal files
      // Consume meal condition currently disabled
      if (m_data.GetConditions().HasConsumeMeal())
      {
        Warning("Consume Meal Condition is currently disabled.");
        //  double elapsedTime = m_data.GetConditions().GetConsumeMeal()->GetMeal().GetElapsedTime(TimeUnit::day);
        //  Dehydration(elapsedTime);
        //  m_AdjustCO2 = false;// Don't mess with CO2 (in the blood) during this Starvation call
        //  Starvation(elapsedTime);
        //  m_AdjustCO2 = true;
        //  //Systemic clearance of calcium occurs over the elapsed time period
        //  double patientWeight_kg = SEScalar::Truncate(m_Patient->GetWeight(MassUnit::kg), 2)
        //  double renalVolumeCleared = m_calcium->GetClearance().GetRenalClearance(VolumePerTimeMassUnit::mL_Per_s_kg)*patientWeight_kg*elapsedTime;
        //  double systemicVolumeCleared = m_calcium->GetClearance().GetSystemicClearance(VolumePerTimeMassUnit::mL_Per_s_kg)*patientWeight_kg*elapsedTime - renalVolumeCleared;
        //  SEScalarVolume* integratedVolume = new SEScalarVolume();
        //  integratedVolume->SetValue(systemicVolumeCleared, VolumeUnit::mL);
        //  m_data.GetCircuits().BalanceBloodMassByClearedVolume(*m_calcium, *integratedVolume);
      }

      Dehydration();
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Prepares the energy system for the circuit solver
  ///
  /// \details
  /// The PreProcess function performs function in preparation to update the thermal circuit.
  /// It also processes energy-related actions (exercise) and computes a sweat rate.
  //--------------------------------------------------------------------------------------------------
  void EnergyModel::PreProcess()
  {
    CalculateMetabolicHeatGeneration();
    Perspiration();
    UpdateHeatResistance();
    Exercise();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// The exercise function updates the patient's metabolic rate if the exercise action is present and
  /// computes energy-store recovery post exercise.
  ///
  /// \details
  /// The exercise function adds to the body's basal metabolic rate a value that is specified by 
  /// the exercise action. The metabolic rate set-point is specified by the action but limited by 
  /// the amount of energy available. The energy limit is computed, and the actual metabolic rate is
  /// ramped to the limited set-point. The body's actual work rate as a fraction of the total possible
  /// work rate and the body's actual work rate as a fraction of the action-requested work rate are set
  /// in this method.
  //--------------------------------------------------------------------------------------------------
  void EnergyModel::Exercise()
  {
    // If there is no exercise action and no fatigue, then return.
    // Any exercise action will reduce energy stores and induce some fatigue, setting the fatigue event.
    // While the fatigue event is active, the exercise method will execute even if there is no exercise action.
    // This allows the energy stores to refill post-exercise.
    if (!m_data.GetActions().GetPatientActions().HasExercise() && !m_data.GetEvents().IsEventActive(eEvent::Fatigue))
      return;

    double exerciseIntensity = 0.0;
    double currentMetabolicRate_kcal_Per_day = GetTotalMetabolicRate().GetValue(PowerUnit::kcal_Per_day);
    double basalMetabolicRate_kcal_Per_day = m_data.GetCurrentPatient().GetBasalMetabolicRate().GetValue(PowerUnit::kcal_Per_day);

    // Only try to get intensity if the exercise action is active. Otherwise, we are just refilling energy buckets post-exercise.
    if (m_data.GetActions().GetPatientActions().HasExercise())
    {
      SEExercise& ex = m_data.GetActions().GetPatientActions().GetExercise();
      if (ex.HasIntensity())
      {
        exerciseIntensity = ex.GetIntensity().GetValue();
      }
      else
      {
        Warning("Exercise call with no severity. Action ignored.");
      }
    }

    if (exerciseIntensity > 0.5)
    {
      // Need to limit the exercise intensity due to current (as of 2/03/2016) engine limitations (CO2 solution error).
      Warning("Exercise intensity currently limited to 0.5. Setting intensity to 0.5.");
    }


    // Fatigue calculations
    double energyIncrement_J = 0.0;

    // There are four individual energy stores that comprise the fatigue system.
    // The usable and peak power stores together can be thought of as similar to the phosphagen energy system.
    // The medium power store can be thought of as similar to the glycogen-lactic acid energy system.
    // The endurance energy store can be thought of as similar to the aerobic energy system.
    // The energy store full levels and the maximum work rate are currently hard-coded, but will be a function of physical conditioning,
    // body composition, and starvation levels. See Future Work section in Pulse Energy documentation.
    double usableEnergyStoreFull_J = 2600.0;
    double peakPowerEnergyStoreFull_J = 4200.0;
    double mediumPowerEnergyStoreFull_J = 35000.0;
    double enduranceEnergyStoreFull_J = 400000.0;
    double maxWorkRate_W = 1200.0;
    // The maximum depletion rates will also scale with the body composition, physical conditioning, and
    // fed/starvation state of the Pulse body.
    double maxEnduranceOutRate_W = 240.0;
    double maxPeakOutRate_W = 621.0;
    double maxMediumOutRate_W = 420.0;

    // The following rates dictate the fill rate of the endurance energy store.
    // These rates are currently hard-coded, but will be tied to the concentration
    // of nutrient substances in the blood in the future. See Future Work section in Pulse Energy documentation.
    double glucoseConstant_W = 10.0;
    double lactateConstant_W = 10.0;
    double tristearinConstant_W = 10.0;

    double workRateDesired_W = exerciseIntensity * maxWorkRate_W;
    double workRate_W = 0.0;
    double normalizedUsableEnergyDeficit = (usableEnergyStoreFull_J - m_UsableEnergyStore_J) / usableEnergyStoreFull_J;
    double normalizedPeakEnergyDeficit = (peakPowerEnergyStoreFull_J - m_PeakPowerEnergyStore_J) / peakPowerEnergyStoreFull_J;
    double normalizedMediumEnergyDeficit = (mediumPowerEnergyStoreFull_J - m_MediumPowerEnergyStore_J) / mediumPowerEnergyStoreFull_J;
    double normalizedEnduranceEnergyDeficit = (enduranceEnergyStoreFull_J - m_EnduranceEnergyStore_J) / enduranceEnergyStoreFull_J;

    // Modify the rates based on the usable energy deficit.
    // This effectively creates piecewise rate functions so that all of the storage
    // outflow rates are zero until the immediate energy storage tank is almost depleted.
    double energyRateModifier = 6.0 * normalizedUsableEnergyDeficit - 5.0;
    LLIM(energyRateModifier, 0.0);

    double glucoseRate_W = glucoseConstant_W * (normalizedUsableEnergyDeficit + normalizedPeakEnergyDeficit + normalizedMediumEnergyDeficit + normalizedEnduranceEnergyDeficit) / 4.0;
    double lactateRate_W = lactateConstant_W * (normalizedUsableEnergyDeficit + normalizedPeakEnergyDeficit + normalizedMediumEnergyDeficit + normalizedEnduranceEnergyDeficit) / 4.0;
    double tristearinRate_W = tristearinConstant_W * (normalizedUsableEnergyDeficit + normalizedPeakEnergyDeficit + normalizedMediumEnergyDeficit + normalizedEnduranceEnergyDeficit) / 4.0;

    energyIncrement_J = (glucoseRate_W + lactateRate_W + tristearinRate_W) * m_data.GetTimeStep_s();

    if (m_EnduranceEnergyStore_J + energyIncrement_J > enduranceEnergyStoreFull_J) {
      glucoseRate_W = lactateRate_W = tristearinRate_W = 0.0;
      energyIncrement_J = 0.0;
    }

    double enduranceOutRate_W = (normalizedUsableEnergyDeficit + normalizedPeakEnergyDeficit + normalizedMediumEnergyDeficit) * maxEnduranceOutRate_W;
    BLIM(enduranceOutRate_W, 0.0, maxEnduranceOutRate_W);
    if (m_EnduranceEnergyStore_J + energyIncrement_J < enduranceOutRate_W * m_data.GetTimeStep_s()) {
      enduranceOutRate_W = (m_EnduranceEnergyStore_J + energyIncrement_J) / m_data.GetTimeStep_s();
    }
    m_EnduranceEnergyStore_J += energyIncrement_J - enduranceOutRate_W * m_data.GetTimeStep_s();

    // The aerobic store has multiple outflow paths. One path represents purely aerobic energy
    // usage, and the other paths represent the aerobic replenishment of the anaerobic stores. The following
    // algorithm computes the percent energy flow at the path junctions. There are two junctions.
    // First junction:
    double splitFraction = 1.0;
    if (normalizedUsableEnergyDeficit + normalizedPeakEnergyDeficit + normalizedMediumEnergyDeficit > 0.00000001) {
      splitFraction = normalizedUsableEnergyDeficit / (normalizedUsableEnergyDeficit + (normalizedMediumEnergyDeficit + normalizedPeakEnergyDeficit) / 2.0);
    }
    double enduranceToUsableRate_W = splitFraction * enduranceOutRate_W;
    double enduranceToFillRate_W = (1.0 - splitFraction) * enduranceOutRate_W;
    // Second junction:
    splitFraction = 1.0;
    if (normalizedUsableEnergyDeficit + normalizedPeakEnergyDeficit + normalizedMediumEnergyDeficit > 0.00000001) {
      if (normalizedPeakEnergyDeficit > 0.00000001)
        splitFraction = normalizedPeakEnergyDeficit / (normalizedPeakEnergyDeficit + normalizedMediumEnergyDeficit);
    }
    double peakFillRate_W = splitFraction * enduranceToFillRate_W;
    double mediumFillRate_W = (1.0 - splitFraction) * enduranceToFillRate_W;

    // Advance peak store state
    energyIncrement_J = peakFillRate_W * m_data.GetTimeStep_s();
    double peakOutRate_W = normalizedUsableEnergyDeficit * maxPeakOutRate_W * energyRateModifier;
    BLIM(peakOutRate_W, 0.0, maxPeakOutRate_W);
    if (m_PeakPowerEnergyStore_J + energyIncrement_J < peakOutRate_W * m_data.GetTimeStep_s()) {
      peakOutRate_W = (m_PeakPowerEnergyStore_J + energyIncrement_J) / m_data.GetTimeStep_s();
    }
    m_PeakPowerEnergyStore_J += energyIncrement_J - peakOutRate_W * m_data.GetTimeStep_s();
    // Advance medium store state
    energyIncrement_J = mediumFillRate_W * m_data.GetTimeStep_s();
    double mediumOutRate_W = normalizedUsableEnergyDeficit * maxMediumOutRate_W * energyRateModifier;
    BLIM(mediumOutRate_W, 0.0, maxMediumOutRate_W);
    if (m_MediumPowerEnergyStore_J + energyIncrement_J < mediumOutRate_W * m_data.GetTimeStep_s()) {
      mediumOutRate_W = (m_MediumPowerEnergyStore_J + energyIncrement_J) / m_data.GetTimeStep_s();
    }
    m_MediumPowerEnergyStore_J += energyIncrement_J - mediumOutRate_W * m_data.GetTimeStep_s();

    // Advance usable bucket state
    energyIncrement_J = (enduranceToUsableRate_W + mediumOutRate_W + peakOutRate_W) * m_data.GetTimeStep_s();
    if (m_UsableEnergyStore_J + energyIncrement_J < workRateDesired_W * m_data.GetTimeStep_s())
    {
      workRate_W = (m_UsableEnergyStore_J + energyIncrement_J) / m_data.GetTimeStep_s();
    }
    else
    {
      workRate_W = workRateDesired_W;
    }

    m_UsableEnergyStore_J += energyIncrement_J - workRate_W * m_data.GetTimeStep_s();

    GetTotalWorkRateLevel().SetValue(workRate_W / maxWorkRate_W);
    double fatigue = (normalizedEnduranceEnergyDeficit + normalizedMediumEnergyDeficit + normalizedPeakEnergyDeficit + normalizedUsableEnergyDeficit) / 4.0;
    if (fatigue > 0.0) {
      m_data.GetEvents().SetEvent(eEvent::Fatigue, true, m_data.GetSimulationTime());
    }
    else {
      m_data.GetEvents().SetEvent(eEvent::Fatigue, false, m_data.GetSimulationTime());
    }
    GetFatigueLevel().SetValue(fatigue);

    double kcal_Per_day_Per_Watt = 20.6362855;

    if (exerciseIntensity > 0.0)
    {
      double  achievedExerciseFraction = GetTotalWorkRateLevel().GetValue() / exerciseIntensity;
      //Handle cases of small errors from calculation (e.g., 1.000000001)
      achievedExerciseFraction = LIMIT(achievedExerciseFraction, 0.0, 1.0);

      /// \todo Log an event or info message that available energy is insufficient to meet demand.
      //if (achievedExerciseFraction < 1.0)
      //  Info("Available energy insufficient to meet requested work rate.");

      GetAchievedExerciseLevel().SetValue(achievedExerciseFraction);
    }
    else
      GetAchievedExerciseLevel().Invalidate();
    // End fatigue calculations

    double TotalMetabolicRateSetPoint_kcal_Per_day = basalMetabolicRate_kcal_Per_day + workRate_W * kcal_Per_day_Per_Watt;

    // The MetabolicRateGain is used to ramp the metabolic rate to the value specified by the user's exercise intensity.
    double MetabolicRateGain = 1.0;
    // We only let exercise control the metabolic rate if it is active, otherwise the heat generation method is in charge of the metabolic rate.
    if (m_data.GetActions().GetPatientActions().HasExercise())
    {
      double TotalMetabolicRateProduced_kcal_Per_day = currentMetabolicRate_kcal_Per_day + MetabolicRateGain * (TotalMetabolicRateSetPoint_kcal_Per_day - currentMetabolicRate_kcal_Per_day) * m_data.GetTimeStep_s();
      GetTotalMetabolicRate().SetValue(TotalMetabolicRateProduced_kcal_Per_day, PowerUnit::kcal_Per_day);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Processes the temperature circuit and calculates extravascular transport
  ///
  /// \details
  /// The Process function solves the temperature circuit for all path heat transfer rates and nodal temperatures.
  /// Additionally, the process function also initiates the flow transport of the metabolic substance quantities
  /// from the vascular to extravascular space. After this is complete, the vital signs of the patient are updated
  /// with regards to the new state.
  //--------------------------------------------------------------------------------------------------
  void EnergyModel::Process(bool solve_and_transport)
  {
    if (solve_and_transport)
      m_circuitCalculator->Process(*m_TemperatureCircuit, m_data.GetTimeStep_s());
    CalculateVitalSigns();
    ComputeExposedModelParameters();
  }
  void EnergyModel::ComputeExposedModelParameters()
  {

  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Updates the parameters for the temperature circuit
  ///
  /// \details
  /// Updates the resulting heat transfer rates, temperatures and nodal heat values; effectively moving
  /// the next values to the current ones before time can be advanced.
  //--------------------------------------------------------------------------------------------------
  void EnergyModel::PostProcess(bool solve_and_transport)
  {
    if (solve_and_transport)
    {
      m_circuitCalculator->PostProcess(*m_TemperatureCircuit);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Checks the patient's thermal and metabolic state
  ///
  /// \details
  /// The core and skin temperatures are recorded in this function. In addition, the current metabolic
  /// state of the patient may trigger the following events: hypothermia, hyperthermia, dehydration, fasciculation
  /// or fatigue. These events are only triggered if the current state falls within the criteria of the specific event
  //--------------------------------------------------------------------------------------------------
  void EnergyModel::CalculateVitalSigns()
  {
    double coreTemperature_degC = m_coreNode->GetTemperature(TemperatureUnit::C);
    double skinTemperature_degC = m_skinNode->GetTemperature(TemperatureUnit::C);
    GetCoreTemperature().SetValue(coreTemperature_degC, TemperatureUnit::C);
    GetSkinTemperature().SetValue(skinTemperature_degC, TemperatureUnit::C);
    std::stringstream ss;

    //Hypothermia check
    // \cite Stocks2004HumanPhysiologicalResponseCold coreTempIrreversible_degC = 20.0
    if (coreTemperature_degC < 35.0) /// \cite mallet2001hypothermia
    {
      m_data.GetEvents().SetEvent(eEvent::Hypothermia, true, m_data.GetSimulationTime());

    }
    else if (m_data.GetEvents().IsEventActive(eEvent::Hypothermia) && coreTemperature_degC > 35.2)
    {
      m_data.GetEvents().SetEvent(eEvent::Hypothermia, false, m_data.GetSimulationTime());
    }
    //Hyperthermia check
    if (coreTemperature_degC > 38.8) /// \cite mallet2001hypothermia
    {
      m_data.GetEvents().SetEvent(eEvent::Hyperthermia, true, m_data.GetSimulationTime());
    }
    else if (m_data.GetEvents().IsEventActive(eEvent::Hyperthermia) && coreTemperature_degC < 38.0)
    {
      m_data.GetEvents().SetEvent(eEvent::Hyperthermia, false, m_data.GetSimulationTime());
    }

    /// \todo Move to blood chemistry
    // Although it is called metabolic acidosis/alkalosis, the detection and event setting is actually a part of the @ref BloodChemistrySystem
    // The terms "metabolic" and "respiratory" refer to the origin of the acid-base disturbance
    // The hydrogen ion concentration is a property of the blood
    // The events related to blood concentrations should be detected and set in blood chemistry.
    //double highPh = 8.5;
    //double lowPh = 6.5;   //\cite Edge2006AcidosisConscious
    m_BloodpH->Sample(m_data.GetBloodChemistry().GetBloodPH().GetValue());
    m_BicarbonateMolarity_mmol_Per_L->Sample(m_AortaHCO3->GetMolarity(AmountPerVolumeUnit::mmol_Per_L));
    //Only check these at the end of a cardiac cycle and reset at start of cardiac cycle 
    if (m_data.GetEvents().IsEventActive(eEvent::StartOfCardiacCycle))
    {
      double bloodPH = m_BloodpH->Value();
      double bloodBicarbonate_mmol_Per_L = m_BicarbonateMolarity_mmol_Per_L->Value();

      if (m_data.GetState() > EngineState::InitialStabilization)
      {// Don't throw events if we are initializing
        if (bloodPH < 7.35 && bloodBicarbonate_mmol_Per_L < 22.0)
          m_data.GetEvents().SetEvent(eEvent::MetabolicAcidosis, true, m_data.GetSimulationTime());

        if (bloodPH > 7.38 && bloodBicarbonate_mmol_Per_L > 23.0)
          m_data.GetEvents().SetEvent(eEvent::MetabolicAcidosis, false, m_data.GetSimulationTime());

        if (bloodPH > 7.45 && bloodBicarbonate_mmol_Per_L > 26.0)
          m_data.GetEvents().SetEvent(eEvent::MetabolicAlkalosis, true, m_data.GetSimulationTime());

        else if (bloodPH < 7.42 && bloodBicarbonate_mmol_Per_L < 25.0)
          m_data.GetEvents().SetEvent(eEvent::MetabolicAlkalosis, false, m_data.GetSimulationTime());
      }
      // Reset the running averages. Why do we need running averages here? Does the aorta pH fluctuate that much? 
      m_BloodpH->Invalidate();
      m_BicarbonateMolarity_mmol_Per_L->Invalidate();
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates the metabolic rate, depending on the current state of the patient
  ///
  /// \details
  /// The metabolic rate is dictated by the current state of the core temperature. This function includes
  /// states of increased metabolic rate due to shivering or severe hyperthermia. Additionally, an extreme
  /// drop in core temperature leads to decreasing metabolic rate. If the core temperature does not meet any
  /// of the criteria for increases/decreases, the metabolic rate will be calculated as the basal metabolic rate.
  //--------------------------------------------------------------------------------------------------
  void EnergyModel::CalculateMetabolicHeatGeneration()
  {
    const PulseConfiguration& config = m_data.GetConfiguration();
    double coreTemperature_degC = m_coreNode->GetTemperature(TemperatureUnit::C);
    double coreTemperatureLow_degC = config.GetCoreTemperatureLow(TemperatureUnit::C);
    double coreTemperatureLowDelta_degC = config.GetDeltaCoreTemperatureLow(TemperatureUnit::C);
    double coreTemperatureHigh_degC = 40.0; //This is for a different equation than the config parameter
    double totalMetabolicRateNew_Kcal_Per_day = 0.0;
    double totalMetabolicRateNew_W = 0.0;
    //The summit metabolism is the maximum amount of power the human body can generate due to shivering/response to the cold.
    double summitMetabolism_W = 21.0 * pow(SEScalar::Truncate(m_data.GetCurrentPatient().GetWeight(MassUnit::kg), 4), 0.75); /// \cite herman2008physics
    double currentMetabolicRate_kcal_Per_day = GetTotalMetabolicRate(PowerUnit::kcal_Per_day);
    double basalMetabolicRate_kcal_Per_day = m_data.GetCurrentPatient().GetBasalMetabolicRate(PowerUnit::kcal_Per_day);

    if (coreTemperature_degC < 34.0) //Hypothermic state inducing metabolic depression (decline of metabolic heat generation)
    {
      totalMetabolicRateNew_W = summitMetabolism_W * pow(0.94, 34.0 - coreTemperature_degC); //The metabolic heat generated will drop by 6% for every degree below 34 C
      GetTotalMetabolicRate().SetValue(totalMetabolicRateNew_W, PowerUnit::W);     /// \cite mallet2002hypothermia
    }
    else if (coreTemperature_degC >= 34.0 && coreTemperature_degC < 36.8) //Patient is increasing heat generation via shivering. This caps out at the summit metabolism
    {
      //Add an event for shivering
      double basalMetabolicRate_W = m_data.GetCurrentPatient().GetBasalMetabolicRate(PowerUnit::W);
      totalMetabolicRateNew_W = basalMetabolicRate_W + (summitMetabolism_W - basalMetabolicRate_W) * (coreTemperatureLow_degC - coreTemperature_degC) / coreTemperatureLowDelta_degC;
      totalMetabolicRateNew_W = MIN(totalMetabolicRateNew_W, summitMetabolism_W); //Bounded at the summit metabolism so further heat generation doesn't continue for continue drops below 34 C.
      GetTotalMetabolicRate().SetValue(totalMetabolicRateNew_W, PowerUnit::W);
    }
    else if (coreTemperature_degC >= 36.8 && coreTemperature_degC < 42.5 && !m_data.GetActions().GetPatientActions().HasExercise()) //Basic Metabolic rate
    {
      double TotalMetabolicRateSetPoint_kcal_Per_day = basalMetabolicRate_kcal_Per_day;
      double MetabolicRateGain = 0.0001;  //Used to ramp the metabolic rate from its current value to the basal value if the patient meet's the basal criteria
      double TotalMetabolicRateProduced_kcal_Per_day = currentMetabolicRate_kcal_Per_day + MetabolicRateGain * (TotalMetabolicRateSetPoint_kcal_Per_day - currentMetabolicRate_kcal_Per_day);
      GetTotalMetabolicRate().SetValue(TotalMetabolicRateProduced_kcal_Per_day, PowerUnit::kcal_Per_day);
    }
    else if (coreTemperature_degC > 40.0 && !m_data.GetActions().GetPatientActions().HasExercise()) //Core temperature greater than 40.0. If not exercising, then the hyperthermia leads to increased metabolism
    {
      totalMetabolicRateNew_Kcal_Per_day = basalMetabolicRate_kcal_Per_day * pow(1.11, coreTemperature_degC - coreTemperatureHigh_degC);  //The metabolic heat generated will increase by 11% for every degree above 40.0 C
      GetTotalMetabolicRate().SetValue(totalMetabolicRateNew_Kcal_Per_day, PowerUnit::kcal_Per_day);                  /// \cite pate2001thermal                  
    }

    if (m_groundToCorePath->GetNextHeatSource(PowerUnit::W) == m_groundToCorePath->GetHeatSourceBaseline(PowerUnit::W))
    {
      //Don't override if someone is trying to set it, like with dehydration
      m_groundToCorePath->GetNextHeatSource().SetValue(GetTotalMetabolicRate(PowerUnit::W), PowerUnit::W);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates perspiration if the core temperature is too high
  ///
  /// \details
  /// The sweat rate is calculated from a core temperature control function. The mass lost due to sweating is accounted for
  /// and a flow source from the skin extravascular to ground path is updated to ensure fluid loss. Substances
  /// are removed based on accepted sweat compositions.
  //--------------------------------------------------------------------------------------------------
  void EnergyModel::Perspiration()
  {
    const PulseConfiguration& config = m_data.GetConfiguration();
    double coreTemperature_degC = m_coreNode->GetTemperature(TemperatureUnit::C);
    double coreTemperatureHigh_degC = config.GetCoreTemperatureHigh(TemperatureUnit::C);
    double sweatHeatTranferCoefficient_W_Per_K = config.GetSweatHeatTransfer(HeatConductanceUnit::W_Per_K);
    double vaporizationEnergy_J_Per_kg = config.GetVaporizationEnergy(EnergyPerMassUnit::J_Per_kg);

    //Calculate sweat rate (in kg/s) from core temperature feedback.
    //The sweat rate heat transfer is determined from a control equation that attempts to keep the core temperature in line
    /// \cite herman2008physics

    //Cutaneous evaporation at rest = 500mL/day = 347 mg/min
    /// \cite marriott1993water
    double baselineSweatRate_mg_Per_min = 347.0;

    double sweatRate_kg_Per_s = (0.25 * sweatHeatTranferCoefficient_W_Per_K / vaporizationEnergy_J_Per_kg) * (coreTemperature_degC - coreTemperatureHigh_degC);
    double sweatRate_mg_Per_min = Convert(sweatRate_kg_Per_s, MassPerTimeUnit::kg_Per_s, MassPerTimeUnit::mg_Per_min);
    sweatRate_mg_Per_min = baselineSweatRate_mg_Per_min + MAX(sweatRate_mg_Per_min, 0.0);

    //Thermoregulatory consequences due to decreased blood volume:
    //Reduced sweating and evaporative heat loss mechanisms impaired (increased core temperature)
    double initialBloodVolume_L = m_data.GetInitialPatient().GetBloodVolumeBaseline(VolumeUnit::L);
    double currentBloodVolume_L = m_data.GetCardiovascular().GetBloodVolume(VolumeUnit::L);
    double bloodVolumeFraction = currentBloodVolume_L / initialBloodVolume_L;
    sweatRate_mg_Per_min *= GeneralMath::ExponentialGrowthFunction(10.0, 0.0001, 1.0, LIMIT(bloodVolumeFraction, 0.0, 1.0));

    GetSweatRate().SetValue(sweatRate_mg_Per_min, MassPerTimeUnit::mg_Per_min);
    double sweatDensity_mg_Per_m3 = config.GetWaterDensity(MassPerVolumeUnit::mg_Per_m3); /// \todo Convert to sweat density once specific gravity calculation is in
    double sweatRate_mL_Per_s = sweatRate_mg_Per_min / sweatDensity_mg_Per_m3 * 16666.7; //Conversion
    m_skinExtravascularToSweatingGroundPath->GetNextFlowSource().SetValue(sweatRate_mL_Per_s, VolumePerTimeUnit::mL_Per_s);

    //Note body mass lost is handled in the Tissue system by body liquid accounting

    //Remove substances that are lost through perspiration
    double perspiredVolume_mL = sweatRate_mL_Per_s * m_data.GetTimeStep_s();
    for (const SESubstanceConcentration* component : m_Sweat->GetComponents())
    {
      double concentration = component->GetConcentration(MassPerVolumeUnit::g_Per_L);
      double massLost_g = concentration * perspiredVolume_mL / 1000.0;
      SELiquidSubstanceQuantity* quantity = m_SkinExtracellular->GetSubstanceQuantity(component->GetSubstance());
      massLost_g = MIN(quantity->GetMass(MassUnit::g), massLost_g);
      quantity->GetMass().Increment(-massLost_g, MassUnit::g);
      quantity->Balance(BalanceLiquidBy::Mass);
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Updates the variable core to skin heat transfer resistance
  ///
  /// \details
  /// The variable core to skin heat transfer resistance is updated here according to the inverse 
  /// of the skin blood flow.
  //--------------------------------------------------------------------------------------------------
  void EnergyModel::UpdateHeatResistance()
  {
    double skinBloodFlow_m3_Per_s = m_data.GetCardiovascular().GetMeanSkinFlow().GetValue(VolumePerTimeUnit::m3_Per_s);
    double bloodDensity_kg_Per_m3 = m_data.GetBloodChemistry().GetBloodDensity().GetValue(MassPerVolumeUnit::kg_Per_m3);
    double bloodSpecificHeat_J_Per_K_kg = m_data.GetBloodChemistry().GetBloodSpecificHeat().GetValue(HeatCapacitancePerMassUnit::J_Per_K_kg);

    double alphaScale = 0.5; //Scaling factor for convective heat transfer from core to skin

    //The heat transfer resistance from the core to the skin is inversely proportional to the skin blood flow.
    //When skin blood flow increases, then heat transfer resistance decreases leading to more heat transfer from core to skin. 
    //The opposite occurs for skin blood flow decrease.
    double coreToSkinResistance_K_Per_W = 1.0 / (alphaScale * bloodDensity_kg_Per_m3 * bloodSpecificHeat_J_Per_K_kg * skinBloodFlow_m3_Per_s);

    //Thermoregulatory consequences due to decreased blood volume:
    //Reduced peripheral circulation due to vasoconstricted skin from compensatory concentrated perfusion in core organs (increased core temperature)
    double initialBloodVolume_L = m_data.GetInitialPatient().GetBloodVolumeBaseline(VolumeUnit::L);
    double currentBloodVolume_L = m_data.GetCardiovascular().GetBloodVolume(VolumeUnit::L);
    double bloodVolumeFraction = currentBloodVolume_L / initialBloodVolume_L;
    coreToSkinResistance_K_Per_W *= GeneralMath::LinearInterpolator(0.0, 1.0, 2.0, 1.0, LIMIT(bloodVolumeFraction, 0.0, 1.0));

    BLIM(coreToSkinResistance_K_Per_W, 0.0001, 20.0);
    m_coreToSkinPath->GetNextResistance().SetValue(coreToSkinResistance_K_Per_W, HeatResistanceUnit::K_Per_W);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates the basal metabolic rate from the Harris-Benedict formula
  ///
  ///
  /// \details
  /// The Harris-Benedict formula uses the patient height, weight age and sex to determine
  /// the basal metabolic requirements.
  //--------------------------------------------------------------------------------------------------
  void EnergyModel::CalculateBasalMetabolicRate()
  {
    double PatientMass_kg = SEScalar::Truncate(m_data.GetCurrentPatient().GetWeight(MassUnit::kg), 4);
    double PatientAge_yr = m_data.GetCurrentPatient().GetAge(TimeUnit::yr);
    double PatientHeight_cm = m_data.GetCurrentPatient().GetHeight(LengthUnit::cm);

    //The basal metabolic rate is determined from the Harris-Benedict formula, with differences dependent on sex, age, height and mass
    /// \cite roza1984metabolic
    double patientBMR_kcal_Per_day = 0.0;
    if (m_data.GetCurrentPatient().GetSex() == ePatient_Sex::Male)
      patientBMR_kcal_Per_day = 88.632 + 13.397 * PatientMass_kg + 4.799 * PatientHeight_cm - 5.677 * PatientAge_yr;
    else
      patientBMR_kcal_Per_day = 447.593 + 9.247 * PatientMass_kg + 3.098 * PatientHeight_cm - 4.330 * PatientAge_yr;
    // Systems do their math with MetabolicRate in Watts, so let's make these consistent
    m_data.GetCurrentPatient().GetBasalMetabolicRate().SetValue(patientBMR_kcal_Per_day, PowerUnit::kcal_Per_day);

    std::stringstream ss;
    ss << "Conditions applied homeostasis: " << "Patient basal metabolic rate = " << patientBMR_kcal_Per_day << " kcal/day";
    Info(ss);
  }

  //--------------------------------------------------------------------------------------------------
/// \brief
/// Apply dehydration condition
///
/// \details
/// Apply thermoregulatory consequences as if they progressed slowly and came to a new homeostasis.
//--------------------------------------------------------------------------------------------------
  void EnergyModel::Dehydration()
  {
    if (!m_data.GetConditions().HasDehydration())
    {
      return;
    }

    double severity = m_data.GetConditions().GetDehydration().GetSeverity().GetValue();

    //Set temperature changes and update compliance heat accordingly
    double coreTemperature_C = GeneralMath::LinearInterpolator(0.0, 1.0, GetCoreTemperature(TemperatureUnit::C), 40.0, severity);
    double coreTemperatureChange_K = coreTemperature_C - m_coreNode->GetNextTemperature(TemperatureUnit::C); //Delta K is same as Delta C

    double capacitance_J_Per_K = m_coreToGroundPath->GetNextCapacitance(HeatCapacitanceUnit::J_Per_K);
    double heatFlow_W = capacitance_J_Per_K * coreTemperatureChange_K / m_data.GetTimeStep_s();

    m_groundToCorePath->GetNextHeatSource().SetValue(heatFlow_W, PowerUnit::W);
  }
}
