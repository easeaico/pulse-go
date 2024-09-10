/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

#include "cdm/patient/SEPatient.h"
#include "PulseScenario.h"

#include "ScenarioIteration.h"

namespace pulse::study::patient_variability
{
  // Min Max values of Pulse
  // Bounds from: https://pulse.kitware.com/_patient_methodology.html
  #define minAge_yr 18
  #define maxAge_yr 65
  #define stdAge_yr 44
  #define minMaleHeight_cm 163
  #define maxMaleHeight_cm 190
  #define stdMaleHeight_cm 180.34
  #define minFemaleHeight_cm 151
  #define maxFemaleHeight_cm 175.5
  #define stdFemaleHeight_cm 162.56
  #define minBMI 16
  #define maxBMI 29.9
  #define stdMaleBMI 23.71
  #define stdFemaleBMI 22.31
  #define minMaleBFF 0.02
  #define maxMaleBFF 0.25
  #define stdMaleBFF 0.21
  #define minFemaleBFF 0.1
  #define maxFemaleBFF 0.32
  #define stdFemaleBFF 0.28
  #define minHR_bpm 60
  #define maxHR_bpm 100
  #define stdHR_bpm 72
  #define minMAP_mmHg 70
  #define maxMAP_mmHg 100
  #define stdMAP_mmHg 87
  #define minPulsePressure_mmHg 30
  #define maxPulsePressure_mmHg 60
  #define stdPulsePressure_mmHg 40.5
  #define minRR_bpm 8
  #define maxRR_bpm 20
  #define stdRR_bpm 12

  class PatientIteration : public ScenarioIteration
  {
  public:
    PatientIteration(Logger& logger);
    virtual ~PatientIteration();

    void Clear() override;

    ePatient_Sex GetSex() const { return m_Sex; }
    void SetSex(ePatient_Sex s) { m_Sex = s; }

    ParameterIteration<double>& GetAge_yr() { return m_Age_yr; }
    const ParameterIteration<double>& GetAge_yr() const { return m_Age_yr; }

    ParameterIteration<double>& GetHeight_cm() { return m_Height_cm; }
    const ParameterIteration<double>& GetHeight_cm() const { return m_Height_cm; }

    ParameterIteration<double>& GetBMI() { return m_BMI; }
    const ParameterIteration<double>& GetBMI() const { return m_BMI; }

    ParameterIteration<double>& GetBFF() { return m_BFF; }
    const ParameterIteration<double>& GetBFF() const { return m_BFF; }

    ParameterIteration<double>& GetHR_bpm() { return m_HR_bpm; }
    const ParameterIteration<double>& GetHR_bpm() const { return m_HR_bpm; }

    ParameterIteration<double>& GetMAP_mmHg() { return m_MAP_mmHg; }
    const ParameterIteration<double>& GetMAP_mmHg() const { return m_MAP_mmHg; }

    ParameterIteration<double>& GetPP_mmHg() { return m_PP_mmHg; }
    const ParameterIteration<double>& GetPP_mmHg() const { return m_PP_mmHg; }

    ParameterIteration<double>& GetRR_bpm() { return m_RR_bpm; }
    const ParameterIteration<double>& GetRR_bpm() const { return m_RR_bpm; }

    void GenerateScenarios();
    const std::map<std::string, std::string>& GetPatientStates() const { return m_PatientStates; }

  protected:
    void FixUp() override;
    void GenerateScenario();
    void GenerateSlicedPatientList();
    void GenerateCombinationPatientList();

    static std::string ToString(SEPatient& patient);

    ePatient_Sex                           m_Sex;
    ParameterIteration<double>             m_Age_yr;
    ParameterIteration<double>             m_Height_cm;
    ParameterIteration<double>             m_BMI;
    ParameterIteration<double>             m_BFF;
    ParameterIteration<double>             m_HR_bpm;
    ParameterIteration<double>             m_MAP_mmHg;
    ParameterIteration<double>             m_PP_mmHg;
    ParameterIteration<double>             m_RR_bpm;

    SEPatient*                             m_Patient;
    std::map<std::string, std::string>     m_PatientStates;
  };
}
