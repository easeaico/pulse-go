/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/PulseConfiguration.h"
#include "engine/common/controller/CircuitManager.h"
#include "engine/common/controller/CompartmentManager.h"
#include "engine/common/controller/SubstanceManager.h"
#include "engine/common/system/physiology/NervousModel.h"
// Conditions
#include "cdm/engine/SEConditionManager.h"
#include "cdm/patient/conditions/SEDehydration.h"
// Actions
#include "cdm/engine/SEActionManager.h"
#include "cdm/engine/SEPatientActionCollection.h"
#include "cdm/patient/SEPatient.h"
#include "cdm/patient/actions/SEBrainInjury.h"
// Dependent Systems
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/system/physiology/SEEnergySystem.h"
#include "cdm/system/physiology/SEPupillaryResponse.h"
#include "cdm/system/physiology/SEDrugSystem.h"
// CDM
#include "cdm/engine/SEEventManager.h"
#include "cdm/circuit/fluid/SEFluidCircuit.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarPressurePerVolume.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarLength.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarNegative1To1.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/utils/GeneralMath.h"
#include "cdm/utils/DataTrack.h"

// #define PROBE

namespace pulse
{
  NervousModel::NervousModel(Data& data) : NervousSystem(data.GetLogger()), Model(data)
  {
    Clear();
  }

  NervousModel::~NervousModel()
  {
    Clear();
  }

  void NervousModel::Clear()
  {
    NervousSystem::Clear();
    m_IntracranialSpace = nullptr;
    m_CSFProductAbsorptionPath = nullptr;
    m_BrainVasculatureCompliancePath = nullptr;
    m_BrainVasculatureResistancePath = nullptr;
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Initializes system properties to valid homeostatic values.
  ///
  /// \details
  /// For stabilization only!
  /// Called AFTER Setup when stabilizing a new patient
  //--------------------------------------------------------------------------------------------------
  void NervousModel::Initialize()
  {
    Model::Initialize();
    SetBaroreceptorFeedback(eSwitch::Off);
    SetChemoreceptorFeedback(eSwitch::Off);
    GetBaroreceptorHeartRateScale().SetValue(1.0);
    GetBaroreceptorHeartElastanceScale().SetValue(1.0);
    GetBaroreceptorResistanceScale().SetValue(1.0);
    GetBaroreceptorComplianceScale().SetValue(1.0);
    GetChemoreceptorHeartRateScale().SetValue(0.0);
    GetChemoreceptorHeartElastanceScale().SetValue(0.0);
    GetLeftEyePupillaryResponse().GetSizeModifier().SetValue(0);
    GetLeftEyePupillaryResponse().GetShapeModifier().SetValue(0);
    GetLeftEyePupillaryResponse().GetReactivityModifier().SetValue(0);
    GetRightEyePupillaryResponse().GetSizeModifier().SetValue(0);
    GetRightEyePupillaryResponse().GetShapeModifier().SetValue(0);
    GetRightEyePupillaryResponse().GetReactivityModifier().SetValue(0);

    // Set when feedback is turned on
    m_BaroreceptorFeedbackStatus = false;
    m_BaroreceptorSaturationStatus = false;
    m_ArterialOxygenBaseline_mmHg = 0;
    m_ArterialCarbonDioxideBaseline_mmHg = 0;
    m_BaroreceptorActiveTime_s = 0.0;
    m_BaroreceptorEffectivenessParameter = 1.0;
    m_BaroreceptorSaturationTime_s = 0.0;
    m_LastMeanArterialPressure_mmHg = 0.0;
    m_PreviousBloodVolume_mL = 0.0;

    m_CSFAbsorptionRate_mLPermin = 0;
    m_CSFProductionRate_mlPermin = 0;

    m_PreviousHeartRhythm = m_data.GetCardiovascular().GetHeartRhythm();

    //m_BaroreceptorPauseTimer = 0;
    //m_BaroreceptorPauseTimerStatus = false;

  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Initializes the nervous specific quantities
  ///
  /// \details
  /// Called during both State loading and Patient Stabilization
  /// Pull and setup up our data (can be from other systems)
  /// Initialize will be called after this and can overwrite any of this data (only if stabilizing)
  //--------------------------------------------------------------------------------------------------
  void NervousModel::SetUp()
  {
    SetBaroreceptorFeedback(m_data.GetConfiguration().GetBaroreceptorFeedback());
    SetChemoreceptorFeedback(m_data.GetConfiguration().GetChemoreceptorFeedback());
    m_NormalizedGammaHeartRate = m_data.GetConfiguration().GetNormalizedHeartRateIntercept();
    m_NormalizedGammaElastance = m_data.GetConfiguration().GetNormalizedHeartElastanceIntercept();
    m_NormalizedGammaCompliance = m_data.GetConfiguration().GetNormalizedComplianceIntercept();
    m_NormalizedGammaResistance = m_data.GetConfiguration().GetNormalizedResistanceIntercept();
    m_NormalizedAlphaHeartRate = m_data.GetConfiguration().GetNormalizedHeartRateSympatheticSlope();
    m_NormalizedAlphaElastance = m_data.GetConfiguration().GetNormalizedHeartElastanceSympatheticSlope();
    m_NormalizedAlphaCompliance = m_data.GetConfiguration().GetNormalizedComplianceParasympatheticSlope();
    m_NormalizedAlphaResistance = m_data.GetConfiguration().GetNormalizedResistanceSympatheticSlope();
    m_NormalizedBetaHeartRate = m_data.GetConfiguration().GetNormalizedHeartRateParasympatheticSlope();

    if (m_data.GetConfiguration().IsCerebrospinalFluidEnabled())
      m_CSFProductAbsorptionPath = m_data.GetCircuits().GetActiveCardiovascularCircuit().GetPath(pulse::CerebrospinalFluidPath::GroundToIntracranialSpace1);
  }

  void NervousModel::AtSteadyState()
  {
    if (m_data.GetState() == EngineState::AtInitialStableState)
    {
      Info("Activating Chemoreceptors");
      m_ChemoreceptorFeedback = m_data.GetConfiguration().GetChemoreceptorFeedback(); // The set-points (Baselines) get reset at the end of each stabilization period.
    }
    else if (m_data.GetState() == EngineState::AtSecondaryStableState)
    {
      Info("Activating Baroreceptors");
      m_BaroreceptorFeedback = m_data.GetConfiguration().GetBaroreceptorFeedback();
    }
    // Set-baselines at the end of each stabilization period.
    SetBaselines();
  }

  void NervousModel::SetBaselines()
  {
    m_ArterialOxygenBaseline_mmHg = m_data.GetBloodChemistry().GetArterialOxygenPressure(PressureUnit::mmHg);
    m_ArterialCarbonDioxideBaseline_mmHg = m_data.GetBloodChemistry().GetArterialCarbonDioxidePressure(PressureUnit::mmHg);
    m_LastMeanArterialPressure_mmHg = m_data.GetCardiovascular().GetMeanArterialPressure(PressureUnit::mmHg);
    m_PreviousBloodVolume_mL = m_data.GetCardiovascular().GetBloodVolume(VolumeUnit::mL);
  }

  void NervousModel::SetBaroreceptorFeedback(eSwitch s)
  {
    if (s == eSwitch::On && m_BaroreceptorFeedback == eSwitch::Off)
      SetBaselines();
    SENervousSystem::SetBaroreceptorFeedback(s);
  }
  void NervousModel::SetChemoreceptorFeedback(eSwitch s)
  {
    if (s == eSwitch::On && m_ChemoreceptorFeedback == eSwitch::Off)
      SetBaselines();
    SENervousSystem::SetChemoreceptorFeedback(s);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Preprocess methods for the nervous system
  ///
  /// \details
  /// Computes nervous system regulation of the body.
  /// Baroreceptor and chemoreceptor feedback is computed and modifiers set in preparation for systems processing.
  //--------------------------------------------------------------------------------------------------
  void NervousModel::PreProcess()
  {
    // Look for any known overrides
    for (auto const& [name, modifier] : m_data.GetOverrides())
    {
      if (name == "CerebrospinalFluidAbsorptionRate")
      {
        m_CSFAbsorptionRate_mLPermin = Convert(modifier.value, VolumePerTimeUnit::GetCompoundUnit(modifier.unit), VolumePerTimeUnit::mL_Per_min);
        Info("CerebrospinalFluidAbsorptionRate override is " + cdm::to_string(m_CSFAbsorptionRate_mLPermin));
        continue;
      }
      if (name == "CerebrospinalFluidProductionRate")
      {
        m_CSFProductionRate_mlPermin = Convert(modifier.value, VolumePerTimeUnit::GetCompoundUnit(modifier.unit), VolumePerTimeUnit::mL_Per_min);
        Info("CerebrospinalFluidProductionRate override is " + cdm::to_string(m_CSFProductionRate_mlPermin));
        continue;
      }
      if (name == pulse::CerebrospinalFluidPath::IntracranialSpace1ToIntracranialSpace2)
      {
        Info("Compliance override is " + cdm::to_string(Convert(modifier.value, VolumePerPressureUnit::GetCompoundUnit(modifier.unit), VolumePerPressureUnit::mL_Per_mmHg)));
        // Print this out and manually make sure its what we want it to be.
        double c = m_data.GetCircuits().GetFluidPath(pulse::CerebrospinalFluidPath::IntracranialSpace1ToIntracranialSpace2)->GetCompliance(VolumePerPressureUnit::mL_Per_mmHg);
        Info("Compliance set to " + cdm::to_string(c));
        continue;
      }
    }

    if (m_data.GetState() == pulse::EngineState::Active)
    {
        /*if (m_data.GetCardiovascular().GetHeartRhythm() != m_previousHeartRhythm && m_BaroreceptorFeedback == eSwitch::Off && !m_data.GetEvents().IsEventActive(eEvent::CardiacArrest))
        {
            m_BaroreceptorPauseTimerStatus = true;
            m_BaroreceptorPauseTimer = 0.0;
        }
        if (m_BaroreceptorPauseTimerStatus && m_BaroreceptorPauseTimer > 5.0)
        {
            m_BaroreceptorFeedback = eSwitch::On;
            m_ChemoreceptorFeedback = eSwitch::On;
            m_BaroreceptorPauseTimerStatus = false;
            m_BaroreceptorPauseTimer = 0.0;
        }
        else
            m_BaroreceptorPauseTimer += m_data.GetTimeStep_s();*/

      if (m_BaroreceptorFeedback == eSwitch::On)
        BaroreceptorFeedback();
      else
      {
        m_BaroreceptorActiveTime_s = 0;
        GetBaroreceptorHeartRateScale().SetValue(1.0);
        GetBaroreceptorHeartElastanceScale().SetValue(1.0);
        GetBaroreceptorResistanceScale().SetValue(1.0);
        GetBaroreceptorComplianceScale().SetValue(1.0);
      }
    }
    else if (m_data.GetConditions().HasDehydration())
    {
      SetBaroreceptorFeedback(eSwitch::On);
      InitializeBaroreceptors();
    }

#ifdef PROBE
    else
    {
      m_data.GetDataTrack().Probe("SympatheticFraction", 0);
      m_data.GetDataTrack().Probe("ParaSympatheticFraction", 0);
      m_data.GetDataTrack().Probe("CumulativeMAP", 0);
      m_data.GetDataTrack().Probe("baroreceptorPressure", 0);
      m_data.GetDataTrack().Probe("deltaPressure", 0);
      m_data.GetDataTrack().Probe("effectivenessParameter", 0);
      m_data.GetDataTrack().Probe("normalizedMAP", 0);
    }
#endif
    if (m_ChemoreceptorFeedback == eSwitch::On)
      ChemoreceptorFeedback();
    else
      GetChemoreceptorHeartRateScale().SetValue(0.0);
    CerebralSpinalFluidUpdates();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Nervous Process Step
  ///
  /// \details
  /// The only current Process-specific function checks the brain status to set events.
  //--------------------------------------------------------------------------------------------------
  void NervousModel::Process(bool /*solve_and_transport*/)
  {
    CheckBrainStatus();
    SetPupilEffects();
    ComputeExposedModelParameters();
    m_PreviousHeartRhythm = m_data.GetCardiovascular().GetHeartRhythm();
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Nervous PostProcess Step
  ///
  /// \details
  /// Currently no nervous postprocess methods.
  //--------------------------------------------------------------------------------------------------
  void NervousModel::PostProcess(bool /*solve_and_transport*/)
  {

  }

  void NervousModel::ComputeExposedModelParameters()
  {

  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// The CSF pressure application function calculates the pressure applied to the brain due to CSF pressure.
  ///
  /// \details
  /// The pressure applied to the brain is dictated by the pressure in the intranial space.
  //--------------------------------------------------------------------------------------------------
  void NervousModel::CerebralSpinalFluidUpdates()
  {
    if (!m_data.GetConfiguration().IsCerebrospinalFluidEnabled())
      return;
    //Update CSF Production and Absorption Rates
    m_CSFProductAbsorptionPath->GetNextFlowSource().SetValue(m_CSFProductionRate_mlPermin - m_CSFAbsorptionRate_mLPermin, VolumePerTimeUnit::mL_Per_min);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates the baroreceptor feedback and sets the scaling parameters in the CDM
  ///
  /// \details
  /// The baroreceptor feedback function uses the current mean arterial pressure relative to the mean arterial
  /// pressure set-point in order to calculate the sympathetic and parasympathetic response fractions.
  /// These fractions are used to update the scaling parameters of heart rate, heart elastance, resistance and compliance
  /// for each time step.
  //--------------------------------------------------------------------------------------------------
  void NervousModel::BaroreceptorFeedback()
  {
    //First calculate the sympathetic and parasympathetic firing rates:
    double nu = 4 * m_data.GetConfiguration().GetResponseSlope();
    double meanArterialPressure_mmHg = m_data.GetCardiovascular().GetMeanArterialPressure(PressureUnit::mmHg);

    //Adjusting the mean arterial pressure set-point to account for cardiovascular drug effects
    double meanArterialPressureBaseline_mmHg = m_data.GetCurrentPatient().GetMeanArterialPressureBaseline(PressureUnit::mmHg);
    double meanArterialPressureCombinedBaseline_mmHg = meanArterialPressureBaseline_mmHg
      + m_data.GetDrugs().GetMeanBloodPressureChange(PressureUnit::mmHg)
      + m_data.GetEnergy().GetExerciseMeanArterialPressureDelta(PressureUnit::mmHg);

    //Determine if barorectors are active and for how long
    double pressureDeviation_mmHg = meanArterialPressure_mmHg - meanArterialPressureCombinedBaseline_mmHg;
    double pressureDeviationFraction = std::abs(pressureDeviation_mmHg) / meanArterialPressureCombinedBaseline_mmHg;
    if (pressureDeviationFraction > 0.05)
    {
      if (!m_BaroreceptorFeedbackStatus)
      {
        m_BaroreceptorActiveTime_s = 0.0;
        m_BaroreceptorFeedbackStatus = true;
        Info("Starting Baroreceptor timer ");
      }
      else
      {
        m_BaroreceptorActiveTime_s += m_data.GetTimeStep_s();
        //Need to change the threshold every 7 minutes based on over activation
        if (m_BaroreceptorActiveTime_s > 420.0/*s*/)
        {
          //Reset the time
          m_BaroreceptorActiveTime_s = 0.0;
          //Adjust the threshold by up to 30%
          meanArterialPressureBaseline_mmHg += 0.35 * pressureDeviation_mmHg;
          m_ss << "Baroreceptor MAP Baseline updated to " << meanArterialPressureBaseline_mmHg << " mmHg";
          m_data.GetCurrentPatient().GetMeanArterialPressureBaseline().SetValue(meanArterialPressureBaseline_mmHg, PressureUnit::mmHg);
          Info(m_ss);
        }
      }
    }
    else
    {
      if (m_BaroreceptorFeedbackStatus)
        Info("Stopping Baroreceptor timer ");
      m_BaroreceptorFeedbackStatus = false;
    }

    // \todo Dampen Baroreceptor response due to sedation
    //double sedationDampeningEffect = m_data.GetDrugs().GetSedationLevel().GetValue() * 0.35; //maximum slope reduction of 35% - making that up right now
    //update nu - the slope response of the firing rate
    //nu *= (1 - sedationDampeningEffect);  TODO: Add this back when I have data for validation
    //Backout the pressure associated with the total sympathetic fraction from the last time step 
    // double cumulativeMeanArterialPressure_mmHg = pow(((1.0 / m_TotalSympatheticFraction) - 1.0), (1.0 / nu)) * meanArterialPressureCombinedBaseline_mmHg;
    // double deltaPressure_mmHg = meanArterialPressure_mmHg - m_LastMeanArterialPressure_mmHg;

    double totalSympatheticFraction = 1.0 / (1.0 + pow(meanArterialPressure_mmHg / meanArterialPressureCombinedBaseline_mmHg, nu));
    double parasympatheticFraction = 1.0 - totalSympatheticFraction;
    double normalizedMAP = meanArterialPressure_mmHg / meanArterialPressureCombinedBaseline_mmHg;

#ifdef PROBE
    m_data.GetDataTrack().Probe("totalSympatheticFraction", totalSympatheticFraction);
    m_data.GetDataTrack().Probe("ParaSympatheticFraction", parasympatheticFraction);
    m_data.GetDataTrack().Probe("pressureDeviation_mmHg", pressureDeviation_mmHg);
    m_data.GetDataTrack().Probe("pressureDeviationFraction", pressureDeviationFraction);
    m_data.GetDataTrack().Probe("meanArterialPressureBaseline_mmHg", meanArterialPressureBaseline_mmHg);
    m_data.GetDataTrack().Probe("normalizedMAP", normalizedMAP);
#endif

    if (totalSympatheticFraction < 0.78)
    {
      if (m_BaroreceptorSaturationStatus)
      {
        m_BaroreceptorSaturationStatus = false;
      }

      //Calculate the normalized change in heart rate
      double normalizedHeartRate = GetBaroreceptorHeartRateScale().GetValue();
      double tauHeartRate_s = m_data.GetConfiguration().GetHeartRateDistributedTimeDelay(TimeUnit::s);
      double deltaNormalizedHeartRate = (1.0 / tauHeartRate_s) * (-normalizedHeartRate + m_NormalizedAlphaHeartRate * totalSympatheticFraction - m_NormalizedBetaHeartRate * parasympatheticFraction + m_NormalizedGammaHeartRate) * m_data.GetTimeStep_s();
      normalizedHeartRate += deltaNormalizedHeartRate;
      GetBaroreceptorHeartRateScale().SetValue(normalizedHeartRate);

      //Calculate the normalized change in heart elastance
      double normalizedHeartElastance = GetBaroreceptorHeartElastanceScale().GetValue();
      double tauElastance_s = m_data.GetConfiguration().GetHeartElastanceDistributedTimeDelay(TimeUnit::s);
      double deltaNormalizedHeartElastance = (1.0 / tauElastance_s) * (-normalizedHeartElastance + m_NormalizedAlphaElastance * totalSympatheticFraction + m_NormalizedGammaElastance) * m_data.GetTimeStep_s();
      normalizedHeartElastance += deltaNormalizedHeartElastance;
      GetBaroreceptorHeartElastanceScale().SetValue(normalizedHeartElastance);

      //Calculate the normalized change in flow resistance for any cardiovascular resistor
      double normalizedResistance = GetBaroreceptorResistanceScale().GetValue();
      double tauResistance_s = m_data.GetConfiguration().GetSystemicResistanceDistributedTimeDelay(TimeUnit::s);
      double deltaNormalizedResistance = (1.0 / tauResistance_s) * (-normalizedResistance + m_NormalizedAlphaResistance * totalSympatheticFraction + m_NormalizedGammaResistance) * m_data.GetTimeStep_s();
      normalizedResistance += deltaNormalizedResistance;
      GetBaroreceptorResistanceScale().SetValue(normalizedResistance);

      //Calculate the normalized change in flow compliance for any cardiovascular compliance
      double normalizedCompliance = GetBaroreceptorComplianceScale().GetValue();
      double tauCompliance_s = m_data.GetConfiguration().GetVenousComplianceDistributedTimeDelay(TimeUnit::s);
      double deltaNormalizedCompliance = (1.0 / tauCompliance_s) * (-normalizedCompliance + m_NormalizedAlphaCompliance * parasympatheticFraction + m_NormalizedGammaCompliance) * m_data.GetTimeStep_s();
      normalizedCompliance += deltaNormalizedCompliance;
      GetBaroreceptorComplianceScale().SetValue(normalizedCompliance);
    }
    else //saturation achieved
    {
      if (!m_BaroreceptorSaturationStatus)
      {
        m_BaroreceptorSaturationStatus = true;
        Info("Baroreceptors Saturated ");
      }
      if (normalizedMAP <= 0.985 && meanArterialPressure_mmHg >= 47)
      {
        //new baroreceptor effect curves
        if (m_PreviousBloodVolume_mL - m_data.GetCardiovascular().GetBloodVolume(VolumeUnit::mL) > 0)
        {
          m_BaroreceptorEffectivenessParameter = -normalizedMAP * 0.06;
        }
        else
        {
          m_BaroreceptorEffectivenessParameter = normalizedMAP * 0.005;
        }
      }
      else if (meanArterialPressure_mmHg > 43 && meanArterialPressure_mmHg < 47)
      {
        if (m_PreviousBloodVolume_mL - m_data.GetCardiovascular().GetBloodVolume(VolumeUnit::mL) > 0)
        {
          m_BaroreceptorEffectivenessParameter -= 0.0001;
          if (m_BaroreceptorEffectivenessParameter < -0.25)
          {
            m_BaroreceptorEffectivenessParameter = -0.25;
          }
        }
        // keep previous parameter if blood volume is not decreasing.
      }
      else if (meanArterialPressure_mmHg <= 43 && meanArterialPressure_mmHg >= 41)
      {
        //last ditch effort
        //m_BaroreceptorEffectivenessParameter += 0.0001;
        m_BaroreceptorEffectivenessParameter = 0.0007;
      }
      else if (meanArterialPressure_mmHg < 41)
      {
        m_BaroreceptorEffectivenessParameter -= 0.00004;
      }
      else
      {
        m_BaroreceptorEffectivenessParameter -= 0.0001;

        if (m_BaroreceptorEffectivenessParameter < -0.25)
        {
          m_BaroreceptorEffectivenessParameter = -0.25;
        }
      }

      //Calculate the normalized change in heart rate
      double normalizedHeartRate = GetBaroreceptorHeartRateScale().GetValue();
      double tauHeartRate_s = m_data.GetConfiguration().GetHeartRateDistributedTimeDelay(TimeUnit::s);
      double deltaNormalizedHeartRate = -1.3 * m_BaroreceptorEffectivenessParameter * (1.0 / tauHeartRate_s) * (-normalizedHeartRate + m_NormalizedAlphaHeartRate * totalSympatheticFraction - m_NormalizedBetaHeartRate * parasympatheticFraction + m_NormalizedGammaHeartRate) * m_data.GetTimeStep_s();
      normalizedHeartRate += deltaNormalizedHeartRate;
      GetBaroreceptorHeartRateScale().SetValue(normalizedHeartRate);

      //Calculate the normalized change in heart elastance
      double normalizedHeartElastance = GetBaroreceptorHeartElastanceScale().GetValue();
      double tauElastance_s = m_data.GetConfiguration().GetHeartElastanceDistributedTimeDelay(TimeUnit::s);
      double deltaNormalizedHeartElastance = 7.5 * m_BaroreceptorEffectivenessParameter * (1.0 / tauElastance_s) * (-normalizedHeartElastance + m_NormalizedAlphaElastance * totalSympatheticFraction + m_NormalizedGammaElastance) * m_data.GetTimeStep_s();
      normalizedHeartElastance += deltaNormalizedHeartElastance;
      GetBaroreceptorHeartElastanceScale().SetValue(normalizedHeartElastance);

      //Calculate the normalized change in flow resistance for any cardiovascular resistor
      double normalizedResistance = GetBaroreceptorResistanceScale().GetValue();
      double tauResistance_s = m_data.GetConfiguration().GetSystemicResistanceDistributedTimeDelay(TimeUnit::s);
      double deltaNormalizedResistance = m_BaroreceptorEffectivenessParameter * (1.0 / tauResistance_s) * (-normalizedResistance + m_NormalizedAlphaResistance * totalSympatheticFraction + m_NormalizedGammaResistance) * m_data.GetTimeStep_s();
      normalizedResistance += deltaNormalizedResistance;
      GetBaroreceptorResistanceScale().SetValue(normalizedResistance);

      //Calculate the normalized change in flow compliance for any cardiovascular compliance
      double normalizedCompliance = GetBaroreceptorComplianceScale().GetValue();
      double tauCompliance_s = m_data.GetConfiguration().GetVenousComplianceDistributedTimeDelay(TimeUnit::s);
      double deltaNormalizedCompliance = m_BaroreceptorEffectivenessParameter * (1.0 / tauCompliance_s) * (-normalizedCompliance + m_NormalizedAlphaCompliance * parasympatheticFraction + m_NormalizedGammaCompliance) * m_data.GetTimeStep_s();
      normalizedCompliance += deltaNormalizedCompliance;
      GetBaroreceptorComplianceScale().SetValue(normalizedCompliance);
    }
    m_PreviousBloodVolume_mL = m_data.GetCardiovascular().GetBloodVolume(VolumeUnit::mL);

#ifdef PROBE
    m_data.GetDataTrack().Probe("effectivenessParameter", m_BaroreceptorEffectivenessParameter);
#endif
  }

//--------------------------------------------------------------------------------------------------
/// \brief 
/// Set baroreceptors during stabilization
///
/// \details
/// This uses the same logic as the main baroreceptor feedback model, but allows it to act as if
/// It as been runnning for a long time due to a chronic condition.
/// 
//--------------------------------------------------------------------------------------------------
  void NervousModel::InitializeBaroreceptors()
  {
    //TODO: Make this part of the BaroreceptorFeedback method and clean them both up

    double nu = 4 * m_data.GetConfiguration().GetResponseSlope();

    double meanArterialPressure_mmHg = m_data.GetCardiovascular().GetMeanArterialPressure(PressureUnit::mmHg);
    double meanArterialPressureBaseline_mmHg = m_data.GetCurrentPatient().GetMeanArterialPressureBaseline(PressureUnit::mmHg);
    double meanArterialPressureCombinedBaseline_mmHg = meanArterialPressureBaseline_mmHg
      + m_data.GetDrugs().GetMeanBloodPressureChange(PressureUnit::mmHg)
      + m_data.GetEnergy().GetExerciseMeanArterialPressureDelta(PressureUnit::mmHg);

    double totalSympatheticFraction = 1.0 / (1.0 + pow(meanArterialPressure_mmHg / meanArterialPressureCombinedBaseline_mmHg, nu));
    double parasympatheticFraction = 1.0 - totalSympatheticFraction;

    //Calculate the normalized change in heart rate
    double normalizedHeartRate = GetBaroreceptorHeartRateScale().GetValue();
    double tauHeartRate_s = m_data.GetConfiguration().GetHeartRateDistributedTimeDelay(TimeUnit::s);
    double deltaNormalizedHeartRate = (1.0 / tauHeartRate_s) * (-normalizedHeartRate + m_NormalizedAlphaHeartRate * totalSympatheticFraction - m_NormalizedBetaHeartRate * parasympatheticFraction + m_NormalizedGammaHeartRate) * m_data.GetTimeStep_s();
    normalizedHeartRate += deltaNormalizedHeartRate;
    GetBaroreceptorHeartRateScale().SetValue(normalizedHeartRate);

    //Calculate the normalized change in heart elastance
    double normalizedHeartElastance = GetBaroreceptorHeartElastanceScale().GetValue();
    double tauElastance_s = m_data.GetConfiguration().GetHeartElastanceDistributedTimeDelay(TimeUnit::s);
    double deltaNormalizedHeartElastance = (1.0 / tauElastance_s) * (-normalizedHeartElastance + m_NormalizedAlphaElastance * totalSympatheticFraction + m_NormalizedGammaElastance) * m_data.GetTimeStep_s();
    normalizedHeartElastance += deltaNormalizedHeartElastance;
    GetBaroreceptorHeartElastanceScale().SetValue(normalizedHeartElastance);

    //Calculate the normalized change in flow resistance for any cardiovascular resistor
    double normalizedResistance = GetBaroreceptorResistanceScale().GetValue();
    double tauResistance_s = m_data.GetConfiguration().GetSystemicResistanceDistributedTimeDelay(TimeUnit::s);
    double deltaNormalizedResistance = (1.0 / tauResistance_s) * (-normalizedResistance + m_NormalizedAlphaResistance * totalSympatheticFraction + m_NormalizedGammaResistance) * m_data.GetTimeStep_s();
    normalizedResistance += deltaNormalizedResistance;
    GetBaroreceptorResistanceScale().SetValue(normalizedResistance);

    //Calculate the normalized change in flow compliance for any cardiovascular compliance
    double normalizedCompliance = GetBaroreceptorComplianceScale().GetValue();
    double tauCompliance_s = m_data.GetConfiguration().GetVenousComplianceDistributedTimeDelay(TimeUnit::s);
    double deltaNormalizedCompliance = (1.0 / tauCompliance_s) * (-normalizedCompliance + m_NormalizedAlphaCompliance * parasympatheticFraction + m_NormalizedGammaCompliance) * m_data.GetTimeStep_s();
    normalizedCompliance += deltaNormalizedCompliance;
    GetBaroreceptorComplianceScale().SetValue(normalizedCompliance);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Checks metrics in the brain to determine events to be thrown
  ///
  /// \details
  /// Intracranial pressure is checked to determine if the patient has Intracranial Hyper/hypotension
  //--------------------------------------------------------------------------------------------------
  void NervousModel::CheckBrainStatus()
  {
    double icp_mmHg = m_data.GetCardiovascular().GetIntracranialPressure().GetValue(PressureUnit::mmHg);

    //Intracranial Hypertension
    if (icp_mmHg > 25.0) // \cite steiner2006monitoring
    {
      m_data.GetEvents().SetEvent(eEvent::IntracranialHypertension, true, m_data.GetSimulationTime());
    }
    else if (m_data.GetEvents().IsEventActive(eEvent::IntracranialHypertension) && icp_mmHg < 24.0)
    {
      m_data.GetEvents().SetEvent(eEvent::IntracranialHypertension, false, m_data.GetSimulationTime());
    }

    //Intracranial Hypotension
    if (icp_mmHg < 7.0) // \cite steiner2006monitoring
    {
      m_data.GetEvents().SetEvent(eEvent::IntracranialHypotension, true, m_data.GetSimulationTime());
    }
    else if (m_data.GetEvents().IsEventActive(eEvent::IntracranialHypotension) && icp_mmHg > 7.5)
    {
      m_data.GetEvents().SetEvent(eEvent::IntracranialHypertension, false, m_data.GetSimulationTime());
    }
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Calculates the chemoreceptor feedback and sets the scaling parameters in the CDM
  ///
  /// \details
  /// The chemoreceptor feedback function uses the current arterial partial pressure of oxygen and carbon dioxide
  /// relative to the partial pressure set-points in order to calculate response signal.
  /// The affected systems identify the signal and adjust accordingly. Note that chemoreception
  /// is currently built into the respiratory driver; therefore, the chemoreceptor feedback only sets CV modifiers.
  //--------------------------------------------------------------------------------------------------
  void NervousModel::ChemoreceptorFeedback()
  {
    double normalized_pO2 = m_data.GetBloodChemistry().GetArterialOxygenPressure(PressureUnit::mmHg) / m_ArterialOxygenBaseline_mmHg;
    double normalized_pCO2 = m_data.GetBloodChemistry().GetArterialCarbonDioxidePressure(PressureUnit::mmHg) / m_ArterialCarbonDioxideBaseline_mmHg;

    // The chemoreceptor heart rate modification function shape parameters.
    // See NervousMethodology documentation for details.
    double amax = -0.1;
    double a50 = 0.5;
    double aeta = -12.;
    double bmax = 1.;
    double b50 = 1.7;
    double beta = 18;
    double cmax = 1.;
    double c50 = 0.65;
    double ceta = -20;
    double dmax = -0.1;
    double d50 = b50;
    double deta = -aeta;

    //Calculate the normalized change in heart rate
    // Maximum HR delta is 1.23 times baseline. The derivation of this maximum is described in the NervousMethodology documentation
    double maxHeartRateDelta = 1.23 * m_data.GetCurrentPatient().GetHeartRateBaseline(FrequencyUnit::Per_min);
    double modifier = GeneralMath::LogisticFunction(amax, a50, aeta, normalized_pCO2);
    modifier += GeneralMath::LogisticFunction(bmax, b50, beta, normalized_pCO2);
    modifier += GeneralMath::LogisticFunction(cmax, c50, ceta, normalized_pO2);
    modifier += GeneralMath::LogisticFunction(dmax, d50, deta, normalized_pO2);

    GetChemoreceptorHeartRateScale().SetValue(maxHeartRateDelta * modifier);

    // Calculate the normalized change in heart elastance
    double normalizedHeartElastance = 1.0;
    /// \todo Compute and apply chemoreceptor-mediated contractility changes
    GetChemoreceptorHeartElastanceScale().SetValue(normalizedHeartElastance);
  }

  //--------------------------------------------------------------------------------------------------
  /// \brief
  /// Sets pupil size and reactivity modifiers based on drug and TBI effects
  ///
  /// \details
  /// Modifiers are on a scale between -1 (for reduction in size/reactivity) and 1 (for increase)
  /// TBI effects are applied to the eye on the same side of the injury if localized or both if diffuse
  /// Drug and TBI pupil effects are simply summed together
  //--------------------------------------------------------------------------------------------------
  void NervousModel::SetPupilEffects()
  {
    // Get modifiers from Drugs
    double leftPupilSizeResponseLevel = m_data.GetDrugs().GetPupillaryResponse().GetSizeModifier().GetValue();
    double leftPupilReactivityResponseLevel = m_data.GetDrugs().GetPupillaryResponse().GetReactivityModifier().GetValue();
    double rightPupilSizeResponseLevel = leftPupilSizeResponseLevel;
    double rightPupilReactivityResponseLevel = leftPupilReactivityResponseLevel;

    // Calculate the TBI response
    if (m_data.GetActions().GetPatientActions().HasBrainInjury())
    {
      SEBrainInjury& b = m_data.GetActions().GetPatientActions().GetBrainInjury();

      if (b.GetSeverity().GetValue() > 0)
      {
        double icp_mmHg = m_data.GetCardiovascular().GetIntracranialPressure().GetValue(PressureUnit::mmHg);

        if (b.GetType() == eBrainInjury_Type::Diffuse)
        {
          leftPupilSizeResponseLevel += (1 / (1 + exp(-2.3 * (icp_mmHg - 22.5))));
          leftPupilReactivityResponseLevel += -.001 * pow(10, .3 * (icp_mmHg - 15));
          rightPupilSizeResponseLevel = leftPupilSizeResponseLevel;
          rightPupilReactivityResponseLevel = leftPupilReactivityResponseLevel;
        }
        else if (b.GetType() == eBrainInjury_Type::LeftFocal)
        {
          leftPupilSizeResponseLevel += (1 / (1 + exp(-2.3 * (icp_mmHg - 22.5))));
          leftPupilReactivityResponseLevel += -.001 * pow(10, .3 * (icp_mmHg - 15));
        }
        else if (b.GetType() == eBrainInjury_Type::RightFocal)
        {
          rightPupilSizeResponseLevel += (1 / (1 + exp(-2.3 * (icp_mmHg - 22.5))));
          rightPupilReactivityResponseLevel += -.001 * pow(10, .3 * (icp_mmHg - 15));
        }
      }
    }

    BLIM(leftPupilSizeResponseLevel, -1, 1);
    BLIM(leftPupilReactivityResponseLevel, -1, 1);
    BLIM(rightPupilSizeResponseLevel, -1, 1);
    BLIM(rightPupilReactivityResponseLevel, -1, 1);
    GetLeftEyePupillaryResponse().GetSizeModifier().SetValue(leftPupilSizeResponseLevel);
    GetLeftEyePupillaryResponse().GetReactivityModifier().SetValue(leftPupilReactivityResponseLevel);
    GetRightEyePupillaryResponse().GetSizeModifier().SetValue(rightPupilSizeResponseLevel);
    GetRightEyePupillaryResponse().GetReactivityModifier().SetValue(rightPupilReactivityResponseLevel);
  }
END_NAMESPACE