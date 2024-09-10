/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

#include "ActionIteration.h"

#include "cdm/engine/SEAdvanceTime.h"
#include "cdm/engine/SESerializeState.h"
#include "cdm/patient/actions/SEAcuteStress.h"
#include "cdm/patient/actions/SEAirwayObstruction.h"
#include "cdm/patient/actions/SEBrainInjury.h"
#include "cdm/patient/actions/SEChestOcclusiveDressing.h"
#include "cdm/patient/actions/SEHemorrhage.h"
#include "cdm/patient/actions/SEHemothorax.h"
#include "cdm/patient/actions/SENeedleDecompression.h"
#include "cdm/patient/actions/SESubstanceCompoundInfusion.h"
#include "cdm/patient/actions/SETensionPneumothorax.h"

namespace pulse::study::patient_variability
{
  enum class eHemorrhageWound
  {
    LeftLegLaceration = 0,
    RightArmLaceration,
    RightArmRightLegLaceration,
    RightLegLeftLegLaceration,
    InternalLiver,
    ExternalLiver,
    _COUNT
  };

  enum class eTensionPneumothoraxWound
  {
    Closed = 0,
    Open,
    _COUNT
  };

  class TCCCIteration : public ActionIteration
  {
  public:
    TCCCIteration(Logger& logger);
    virtual ~TCCCIteration();

    void Clear() override;

    bool PerformInterventions() const { return m_PerformInterventions; }
    void PerformInterventions(bool p) { m_PerformInterventions = p; }

    ParameterIteration<double>& GetAirwayObstructionSeverity() { return m_AirwayObstructionSeverity; }
    const ParameterIteration<double>& GetAirwayObstructionSeverity() const { return m_AirwayObstructionSeverity; }

    ParameterIteration<double>& GetHemorrhageSeverity() { return m_HemorrhageSeverity; }
    const ParameterIteration<double>& GetHemorrhageSeverity() const { return m_HemorrhageSeverity; }

    ParameterIteration<size_t>& GetHemorrhageWound() { return m_HemorrhageWound; }
    const ParameterIteration<size_t>& GetHemorrhageWound() const { return m_HemorrhageWound; }

    ParameterIteration<double>& GetLeftHemothoraxSeverity() { return m_LeftHemothoraxSeverity; }
    const ParameterIteration<double>& GetLeftHemothoraxSeverity() const { return m_LeftHemothoraxSeverity; }

    ParameterIteration<double>& GetRightHemothoraxSeverity() { return m_RightHemothoraxSeverity; }
    const ParameterIteration<double>& GetRightHemothoraxSeverity() const { return m_RightHemothoraxSeverity; }

    ParameterIteration<double>& GetStressSeverity() { return m_StressSeverity; }
    const ParameterIteration<double>& GetStressSeverity() const { return m_StressSeverity; }

    ParameterIteration<double>& GetTBISeverity() { return m_TBISeverity; }
    const ParameterIteration<double>& GetTBISeverity() const { return m_TBISeverity; }

    ParameterIteration<double>& GetLeftTensionPneumothoraxSeverity() { return m_LeftTensionPneumothoraxSeverity; }
    const ParameterIteration<double>& GetLeftTensionPneumothoraxSeverity() const { return m_LeftTensionPneumothoraxSeverity; }

    ParameterIteration<size_t>& GetLeftTensionPneumothoraxWound() { return m_LeftTensionPneumothoraxWound; }
    const ParameterIteration<size_t>& GetLeftTensionPneumothoraxWound() const { return m_LeftTensionPneumothoraxWound; }

    ParameterIteration<double>& GetRightTensionPneumothoraxSeverity() { return m_RightTensionPneumothoraxSeverity; }
    const ParameterIteration<double>& GetRightTensionPneumothoraxSeverity() const { return m_RightTensionPneumothoraxSeverity; }

    ParameterIteration<size_t>& GetRightTensionPneumothoraxWound() { return m_RightTensionPneumothoraxWound; }
    const ParameterIteration<size_t>& GetRightTensionPneumothoraxWound() const { return m_RightTensionPneumothoraxWound; }

    ParameterIteration<double>& GetInsultDuration_s() { return m_InsultDuration_s; }
    const ParameterIteration<double>& GetInsultDuration_s() const { return m_InsultDuration_s; }

    ParameterIteration<double>& GetSalineAvailable() { return m_SalineAvailable; }
    const ParameterIteration<double>& GetSalineAvailable() const { return m_SalineAvailable; }

    ParameterIteration<double>& GetNeedleAvailable() { return m_NeedleAvailable; }
    const ParameterIteration<double>& GetNeedleAvailable() const { return m_NeedleAvailable; }

    ParameterIteration<double>& GetChestWrapAvailable() { return m_ChestWrapAvailable; }
    const ParameterIteration<double>& GetChestWrapAvailable() const { return m_ChestWrapAvailable; }

  protected:
    void FixUp() override;
    void GenerateSlicedActionSets(std::pair<std::string, std::string>) override;
    void GenerateCombinationActionSets(std::pair<std::string, std::string>) override;
    void GenerateScenario(double AirwayObstructionSeverity,
                          double HemorrhageSeverity,
                          size_t HemorrhageWound,
                          double LeftHemothoraxSeverity,
                          double RightHemothoraxSeverity,
                          double StressSeverity,
                          double TBISeverity,
                          double LeftTensionPneumothoraxSeverity,
                          size_t LeftTensionPneumothoraxWound,
                          double RightTensionPneumothoraxSeverity,
                          size_t RightTensionPneumothoraxWound,
                          double InsultDuration_s,
                          const std::string& PatientName);

    // Stateful
    // Injuries
    ParameterIteration<double>    m_AirwayObstructionSeverity;
    ParameterIteration<double>    m_HemorrhageSeverity;
    ParameterIteration<size_t>    m_HemorrhageWound;
    ParameterIteration<double>    m_LeftHemothoraxSeverity;
    ParameterIteration<double>    m_RightHemothoraxSeverity;
    ParameterIteration<double>    m_StressSeverity;
    ParameterIteration<double>    m_TBISeverity;
    ParameterIteration<double>    m_LeftTensionPneumothoraxSeverity;
    ParameterIteration<size_t>    m_LeftTensionPneumothoraxWound;
    ParameterIteration<double>    m_RightTensionPneumothoraxSeverity;
    ParameterIteration<size_t>    m_RightTensionPneumothoraxWound;
    ParameterIteration<double>    m_InsultDuration_s;
    // Interventions
    bool                          m_PerformInterventions;
    ParameterIteration<double>    m_SalineAvailable;
    ParameterIteration<double>    m_NeedleAvailable;
    ParameterIteration<double>    m_ChestWrapAvailable;

    // Stateless
    SEAdvanceTime         m_Adv2Insult;
    SEAirwayObstruction   m_AirwayObstruction;
    SEBrainInjury         m_BrainInjury;
    SEHemorrhage          m_Hemorrhage1;
    SEHemorrhage          m_Hemorrhage2;
    SEHemothorax          m_LeftHemothorax;
    SEHemothorax          m_RightHemothorax;
    SEAcuteStress         m_Stress;
    SETensionPneumothorax m_LeftTensionPneumothorax;
    SETensionPneumothorax m_RightTensionPneumothorax;
    SEAdvanceTime         m_Adv2Intervention;
    SEAdvanceTime         m_Adv2End;
  };
}
