/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalarPressureTimePerVolume.h"
#include "cdm/properties/SEScalarPressurePerVolume.h"
#include "cdm/properties/SEScalarPressureTimePerArea.h"
#include "cdm/properties/SEScalarPressureTimePerVolumeArea.h"
#include "cdm/properties/SEScalarVolumePerTimeArea.h"

SECardiovascularSystem::SECardiovascularSystem(Logger* logger) : SESystem(logger)
{
  m_ArterialPressure = nullptr;
  m_BloodVolume = nullptr;
  m_CardiacIndex = nullptr;
  m_CardiacOutput = nullptr;
  m_CentralVenousPressure = nullptr;
  m_CerebralBloodFlow = nullptr;
  m_CerebralPerfusionPressure = nullptr;
  m_CoronaryPerfusionPressure = nullptr;
  m_DiastolicArterialPressure = nullptr;
  m_DiastolicLeftHeartPressure = nullptr;
  m_DiastolicRightHeartPressure = nullptr;
  m_HeartEjectionFraction = nullptr;
  m_HeartRate = nullptr;
  m_HeartRhythm = eHeartRhythm::NormalSinus;
  m_HeartStrokeVolume = nullptr; 
  m_IntracranialPressure = nullptr;
  m_MeanArterialPressure = nullptr;
  m_MeanArterialCarbonDioxidePartialPressure = nullptr;
  m_MeanArterialCarbonDioxidePartialPressureDelta = nullptr;
  m_MeanCentralVenousPressure = nullptr;
  m_MeanSkinFlow = nullptr;
  m_PeripheralPerfusionIndex = nullptr;
  m_PulmonaryArterialPressure = nullptr;
  m_PulmonaryCapillariesCoverageFraction = nullptr;
  m_PulmonaryCapillariesWedgePressure = nullptr;
  m_PulmonaryDiastolicArterialPressure = nullptr;
  m_PulmonaryMeanArterialPressure = nullptr;
  m_PulmonaryMeanCapillaryFlow = nullptr;
  m_PulmonaryMeanShuntFlow = nullptr;
  m_PulmonarySystolicArterialPressure = nullptr;
  m_PulmonaryVascularResistance = nullptr;
  m_PulmonaryVascularResistanceIndex = nullptr;
  m_PulsePressure = nullptr;
  m_SystemicVascularResistance = nullptr;
  m_SystolicArterialPressure = nullptr;
  m_SystolicLeftHeartPressure = nullptr;
  m_SystolicRightHeartPressure = nullptr;
  m_TotalHemorrhageRate = nullptr;
  m_TotalHemorrhagedVolume = nullptr;
  m_TotalPulmonaryPerfusion = nullptr;
}

SECardiovascularSystem::~SECardiovascularSystem()
{
  Clear();

  SAFE_DELETE(m_ArterialPressure);
  SAFE_DELETE(m_BloodVolume);
  SAFE_DELETE(m_CardiacIndex);
  SAFE_DELETE(m_CardiacOutput);
  SAFE_DELETE(m_CentralVenousPressure);
  SAFE_DELETE(m_CerebralBloodFlow);
  SAFE_DELETE(m_CerebralPerfusionPressure);
  SAFE_DELETE(m_CoronaryPerfusionPressure);
  SAFE_DELETE(m_DiastolicArterialPressure);
  SAFE_DELETE(m_DiastolicLeftHeartPressure);
  SAFE_DELETE(m_DiastolicRightHeartPressure);
  SAFE_DELETE(m_HeartEjectionFraction);
  SAFE_DELETE(m_HeartRate);
  m_HeartRhythm = eHeartRhythm::NormalSinus;
  SAFE_DELETE(m_HeartStrokeVolume);
  SAFE_DELETE(m_IntracranialPressure);
  SAFE_DELETE(m_MeanArterialPressure);
  SAFE_DELETE(m_MeanArterialCarbonDioxidePartialPressure);
  SAFE_DELETE(m_MeanArterialCarbonDioxidePartialPressureDelta);
  SAFE_DELETE(m_MeanCentralVenousPressure);
  SAFE_DELETE(m_MeanSkinFlow);
  SAFE_DELETE(m_PeripheralPerfusionIndex);
  SAFE_DELETE(m_PulmonaryArterialPressure);
  SAFE_DELETE(m_PulmonaryCapillariesCoverageFraction);
  SAFE_DELETE(m_PulmonaryCapillariesWedgePressure);
  SAFE_DELETE(m_PulmonaryDiastolicArterialPressure);
  SAFE_DELETE(m_PulmonaryMeanArterialPressure);
  SAFE_DELETE(m_PulmonaryMeanCapillaryFlow);
  SAFE_DELETE(m_PulmonaryMeanShuntFlow);
  SAFE_DELETE(m_PulmonarySystolicArterialPressure);
  SAFE_DELETE(m_PulmonaryVascularResistance);
  SAFE_DELETE(m_PulmonaryVascularResistanceIndex);
  SAFE_DELETE(m_PulsePressure);
  SAFE_DELETE(m_SystemicVascularResistance);
  SAFE_DELETE(m_SystolicArterialPressure);
  SAFE_DELETE(m_SystolicLeftHeartPressure);
  SAFE_DELETE(m_SystolicRightHeartPressure);
  SAFE_DELETE(m_TotalHemorrhageRate);
  SAFE_DELETE(m_TotalHemorrhagedVolume);
  SAFE_DELETE(m_TotalPulmonaryPerfusion);

}

void SECardiovascularSystem::Clear()
{
  INVALIDATE_PROPERTY(m_ArterialPressure);
  INVALIDATE_PROPERTY(m_BloodVolume);
  INVALIDATE_PROPERTY(m_CardiacIndex);
  INVALIDATE_PROPERTY(m_CardiacOutput);
  INVALIDATE_PROPERTY(m_CentralVenousPressure);
  INVALIDATE_PROPERTY(m_CerebralBloodFlow);
  INVALIDATE_PROPERTY(m_CerebralPerfusionPressure);
  INVALIDATE_PROPERTY(m_CoronaryPerfusionPressure);
  INVALIDATE_PROPERTY(m_DiastolicArterialPressure);
  INVALIDATE_PROPERTY(m_DiastolicLeftHeartPressure);
  INVALIDATE_PROPERTY(m_DiastolicRightHeartPressure);
  INVALIDATE_PROPERTY(m_HeartEjectionFraction);
  INVALIDATE_PROPERTY(m_HeartRate);
  m_HeartRhythm = eHeartRhythm::NormalSinus;
  INVALIDATE_PROPERTY(m_HeartStrokeVolume);
  INVALIDATE_PROPERTY(m_IntracranialPressure);
  INVALIDATE_PROPERTY(m_MeanArterialPressure);
  INVALIDATE_PROPERTY(m_MeanArterialCarbonDioxidePartialPressure);
  INVALIDATE_PROPERTY(m_MeanArterialCarbonDioxidePartialPressureDelta);
  INVALIDATE_PROPERTY(m_MeanCentralVenousPressure);
  INVALIDATE_PROPERTY(m_MeanSkinFlow);
  INVALIDATE_PROPERTY(m_PeripheralPerfusionIndex);
  INVALIDATE_PROPERTY(m_PulmonaryArterialPressure);
  INVALIDATE_PROPERTY(m_PulmonaryCapillariesCoverageFraction);
  INVALIDATE_PROPERTY(m_PulmonaryCapillariesWedgePressure);
  INVALIDATE_PROPERTY(m_PulmonaryDiastolicArterialPressure);
  INVALIDATE_PROPERTY(m_PulmonaryMeanArterialPressure);
  INVALIDATE_PROPERTY(m_PulmonaryMeanCapillaryFlow);
  INVALIDATE_PROPERTY(m_PulmonaryMeanShuntFlow);
  INVALIDATE_PROPERTY(m_PulmonarySystolicArterialPressure);
  INVALIDATE_PROPERTY(m_PulmonaryVascularResistance);
  INVALIDATE_PROPERTY(m_PulmonaryVascularResistanceIndex);
  INVALIDATE_PROPERTY(m_PulsePressure);
  INVALIDATE_PROPERTY(m_SystemicVascularResistance);
  INVALIDATE_PROPERTY(m_SystolicArterialPressure);
  INVALIDATE_PROPERTY(m_SystolicLeftHeartPressure);
  INVALIDATE_PROPERTY(m_SystolicRightHeartPressure);
  INVALIDATE_PROPERTY(m_TotalHemorrhageRate);
  INVALIDATE_PROPERTY(m_TotalHemorrhagedVolume);
  INVALIDATE_PROPERTY(m_TotalPulmonaryPerfusion);
}

const SEScalar* SECardiovascularSystem::GetScalar(const std::string& name)
{
  if (name.compare("ArterialPressure") == 0)
    return &GetArterialPressure();
  if (name.compare("BloodVolume") == 0)
    return &GetBloodVolume();
  if (name.compare("CardiacIndex") == 0)
    return &GetCardiacIndex();
  if (name.compare("CardiacOutput") == 0)
    return &GetCardiacOutput();
  if (name.compare("CentralVenousPressure") == 0)
    return &GetCentralVenousPressure();
  if (name.compare("CerebralBloodFlow") == 0)
    return &GetCerebralBloodFlow();
  if (name.compare("CerebralPerfusionPressure") == 0)
    return &GetCerebralPerfusionPressure();
  if (name.compare("CoronaryPerfusionPressure") == 0)
    return &GetCoronaryPerfusionPressure();
  if (name.compare("DiastolicArterialPressure") == 0)
    return &GetDiastolicArterialPressure();
  if (name.compare("DiastolicLeftHeartPressure") == 0)
    return &GetDiastolicLeftHeartPressure();
  if (name.compare("DiastolicRightHeartPressure") == 0)
    return &GetDiastolicRightHeartPressure();
  if (name.compare("HeartEjectionFraction") == 0)
    return &GetHeartEjectionFraction();
  if (name.compare("HeartRate") == 0)
    return &GetHeartRate();
  if (name.compare("HeartStrokeVolume") == 0)
    return &GetHeartStrokeVolume();
  if (name.compare("IntracranialPressure") == 0)
    return &GetIntracranialPressure();
  if (name.compare("MeanArterialPressure") == 0)
    return &GetMeanArterialPressure();
  if (name.compare("MeanArterialCarbonDioxidePartialPressure") == 0)
    return &GetMeanArterialCarbonDioxidePartialPressure();
  if (name.compare("MeanArterialCarbonDioxidePartialPressureDelta") == 0)
    return &GetMeanArterialCarbonDioxidePartialPressureDelta();
  if (name.compare("MeanCentralVenousPressure") == 0)
    return &GetMeanCentralVenousPressure();
  if (name.compare("MeanSkinFlow") == 0)
    return &GetMeanSkinFlow();
  if (name.compare("PeripheralPerfusionIndex") == 0)
    return &GetPeripheralPerfusionIndex();
  if (name.compare("PulmonaryArterialPressure") == 0)
    return &GetPulmonaryArterialPressure();
  if (name.compare("PulmonaryCapillariesCoverageFraction") == 0)
    return &GetPulmonaryCapillariesCoverageFraction();
  if (name.compare("PulmonaryCapillariesWedgePressure") == 0)
    return &GetPulmonaryCapillariesWedgePressure();
  if (name.compare("PulmonaryDiastolicArterialPressure") == 0)
    return &GetPulmonaryDiastolicArterialPressure();
  if (name.compare("PulmonaryMeanArterialPressure") == 0)
    return &GetPulmonaryMeanArterialPressure();
  if (name.compare("PulmonaryMeanCapillaryFlow") == 0)
    return &GetPulmonaryMeanCapillaryFlow();
  if (name.compare("PulmonaryMeanShuntFlow") == 0)
    return &GetPulmonaryMeanShuntFlow();
  if (name.compare("PulmonarySystolicArterialPressure") == 0)
    return &GetPulmonarySystolicArterialPressure();
  if (name.compare("PulmonaryVascularResistance") == 0)
    return &GetPulmonaryVascularResistance();
  if (name.compare("PulmonaryVascularResistanceIndex") == 0)
    return &GetPulmonaryVascularResistanceIndex();
  if (name.compare("PulsePressure") == 0)
    return &GetPulsePressure();
  if (name.compare("SystemicVascularResistance") == 0)
    return &GetSystemicVascularResistance();
  if (name.compare("SystolicArterialPressure") == 0)
    return &GetSystolicArterialPressure();
  if (name.compare("SystolicLeftHeartPressure") == 0)
    return &GetSystolicLeftHeartPressure();
  if (name.compare("SystolicRightHeartPressure") == 0)
    return &GetSystolicRightHeartPressure();
  if (name.compare("TotalHemorrhageRate") == 0)
    return &GetTotalHemorrhageRate();
  if (name.compare("TotalHemorrhagedVolume") == 0)
    return &GetTotalHemorrhagedVolume();
  if (name.compare("TotalPulmonaryPerfusion") == 0)
    return &GetTotalPulmonaryPerfusion();

  return nullptr;
}

bool SECardiovascularSystem::HasArterialPressure() const
{
  return m_ArterialPressure==nullptr?false:m_ArterialPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetArterialPressure()
{
  if(m_ArterialPressure==nullptr)
    m_ArterialPressure=new SEScalarPressure();
  return *m_ArterialPressure;
}
double SECardiovascularSystem::GetArterialPressure(const PressureUnit& unit) const
{
  if (m_ArterialPressure == nullptr)
    return SEScalar::dNaN();
  return m_ArterialPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasBloodVolume() const
{
  return m_BloodVolume==nullptr?false:m_BloodVolume->IsValid();
}
SEScalarVolume& SECardiovascularSystem::GetBloodVolume()
{
  if(m_BloodVolume==nullptr)
    m_BloodVolume=new SEScalarVolume();
  return *m_BloodVolume;
}
double SECardiovascularSystem::GetBloodVolume(const VolumeUnit& unit) const
{
  if (m_BloodVolume == nullptr)
    return SEScalar::dNaN();
  return m_BloodVolume->GetValue(unit);
}

bool SECardiovascularSystem::HasCardiacOutput() const
{
  return m_CardiacOutput==nullptr?false:m_CardiacOutput->IsValid();
}
SEScalarVolumePerTime& SECardiovascularSystem::GetCardiacOutput()
{
  if(m_CardiacOutput==nullptr)
    m_CardiacOutput=new SEScalarVolumePerTime();
  return *m_CardiacOutput;
}
double SECardiovascularSystem::GetCardiacOutput(const VolumePerTimeUnit& unit) const
{
  if (m_CardiacOutput == nullptr)
    return SEScalar::dNaN();
  return m_CardiacOutput->GetValue(unit);
}

bool SECardiovascularSystem::HasCardiacIndex() const
{
  return m_CardiacIndex == nullptr ? false : m_CardiacIndex->IsValid();
}
SEScalarVolumePerTimeArea& SECardiovascularSystem::GetCardiacIndex()
{
  if (m_CardiacIndex == nullptr)
    m_CardiacIndex = new SEScalarVolumePerTimeArea();
  return *m_CardiacIndex;
}
double SECardiovascularSystem::GetCardiacIndex(const VolumePerTimeAreaUnit& unit) const
{
  if (m_CardiacIndex == nullptr)
    return SEScalar::dNaN();
  return m_CardiacIndex->GetValue(unit);
}

bool SECardiovascularSystem::HasCentralVenousPressure() const
{
  return m_CentralVenousPressure==nullptr?false:m_CentralVenousPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetCentralVenousPressure()
{
  if(m_CentralVenousPressure==nullptr)
    m_CentralVenousPressure=new SEScalarPressure();
  return *m_CentralVenousPressure;
}
double SECardiovascularSystem::GetCentralVenousPressure(const PressureUnit& unit) const
{
  if (m_CentralVenousPressure == nullptr)
    return SEScalar::dNaN();
  return m_CentralVenousPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasCerebralBloodFlow() const
{
  return m_CerebralBloodFlow == nullptr ? false : m_CerebralBloodFlow->IsValid();
}
SEScalarVolumePerTime& SECardiovascularSystem::GetCerebralBloodFlow()
{
  if (m_CerebralBloodFlow == nullptr)
    m_CerebralBloodFlow = new SEScalarVolumePerTime();
  return *m_CerebralBloodFlow;
}
double SECardiovascularSystem::GetCerebralBloodFlow(const VolumePerTimeUnit& unit) const
{
  if (m_CerebralBloodFlow == nullptr)
    return SEScalar::dNaN();
  return m_CerebralBloodFlow->GetValue(unit);
}

bool SECardiovascularSystem::HasCerebralPerfusionPressure() const
{
  return m_CerebralPerfusionPressure == nullptr ? false : m_CerebralPerfusionPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetCerebralPerfusionPressure()
{
  if (m_CerebralPerfusionPressure == nullptr)
    m_CerebralPerfusionPressure = new SEScalarPressure();
  return *m_CerebralPerfusionPressure;
}
double SECardiovascularSystem::GetCerebralPerfusionPressure(const PressureUnit& unit) const
{
  if (m_CerebralPerfusionPressure == nullptr)
    return SEScalar::dNaN();
  return m_CerebralPerfusionPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasCoronaryPerfusionPressure() const
{
  return m_CoronaryPerfusionPressure == nullptr ? false : m_CoronaryPerfusionPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetCoronaryPerfusionPressure()
{
  if (m_CoronaryPerfusionPressure == nullptr)
    m_CoronaryPerfusionPressure = new SEScalarPressure();
  return *m_CoronaryPerfusionPressure;
}
double SECardiovascularSystem::GetCoronaryPerfusionPressure(const PressureUnit& unit) const
{
  if (m_CoronaryPerfusionPressure == nullptr)
    return SEScalar::dNaN();
  return m_CoronaryPerfusionPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasDiastolicArterialPressure() const
{
  return m_DiastolicArterialPressure==nullptr?false:m_DiastolicArterialPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetDiastolicArterialPressure()
{
  if(m_DiastolicArterialPressure==nullptr)
    m_DiastolicArterialPressure=new SEScalarPressure();
  return *m_DiastolicArterialPressure;
}
double SECardiovascularSystem::GetDiastolicArterialPressure(const PressureUnit& unit) const
{
  if (m_DiastolicArterialPressure == nullptr)
    return SEScalar::dNaN();
  return m_DiastolicArterialPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasDiastolicLeftHeartPressure() const
{
  return m_DiastolicLeftHeartPressure == nullptr ? false : m_DiastolicLeftHeartPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetDiastolicLeftHeartPressure()
{
  if (m_DiastolicLeftHeartPressure == nullptr)
    m_DiastolicLeftHeartPressure = new SEScalarPressure();
  return *m_DiastolicLeftHeartPressure;
}
double SECardiovascularSystem::GetDiastolicLeftHeartPressure(const PressureUnit& unit) const
{
  if (m_DiastolicLeftHeartPressure == nullptr)
    return SEScalar::dNaN();
  return m_DiastolicLeftHeartPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasDiastolicRightHeartPressure() const
{
  return m_DiastolicRightHeartPressure == nullptr ? false : m_DiastolicRightHeartPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetDiastolicRightHeartPressure()
{
  if (m_DiastolicRightHeartPressure == nullptr)
    m_DiastolicRightHeartPressure = new SEScalarPressure();
  return *m_DiastolicRightHeartPressure;
}
double SECardiovascularSystem::GetDiastolicRightHeartPressure(const PressureUnit& unit) const
{
  if (m_DiastolicRightHeartPressure == nullptr)
    return SEScalar::dNaN();
  return m_DiastolicRightHeartPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasHeartEjectionFraction() const
{
  return m_HeartEjectionFraction==nullptr?false:m_HeartEjectionFraction->IsValid();
}
SEScalar0To1& SECardiovascularSystem::GetHeartEjectionFraction()
{
  if(m_HeartEjectionFraction==nullptr)
    m_HeartEjectionFraction=new SEScalar0To1();
  return *m_HeartEjectionFraction;
}
double SECardiovascularSystem::GetHeartEjectionFraction() const
{
  if (m_HeartEjectionFraction == nullptr)
    return SEScalar::dNaN();
  return m_HeartEjectionFraction->GetValue();
}

bool SECardiovascularSystem::HasHeartRate() const
{
  return m_HeartRate==nullptr?false:m_HeartRate->IsValid();
}
SEScalarFrequency& SECardiovascularSystem::GetHeartRate()
{
  if(m_HeartRate==nullptr)
    m_HeartRate=new SEScalarFrequency();
  return *m_HeartRate;
}
double SECardiovascularSystem::GetHeartRate(const FrequencyUnit& unit) const
{
  if (m_HeartRate == nullptr)
    return SEScalar::dNaN();
  return m_HeartRate->GetValue(unit);
}

eHeartRhythm SECardiovascularSystem::GetHeartRhythm() const
{
  return m_HeartRhythm;
}
void SECardiovascularSystem::SetHeartRhythm(eHeartRhythm rhythm)
{
  m_HeartRhythm = rhythm;
}

bool SECardiovascularSystem::HasHeartStrokeVolume() const
{
  return m_HeartStrokeVolume==nullptr?false:m_HeartStrokeVolume->IsValid();
}
SEScalarVolume& SECardiovascularSystem::GetHeartStrokeVolume()
{
  if(m_HeartStrokeVolume==nullptr)
    m_HeartStrokeVolume=new SEScalarVolume();
  return *m_HeartStrokeVolume;
}
double SECardiovascularSystem::GetHeartStrokeVolume(const VolumeUnit& unit) const
{
  if (m_HeartStrokeVolume == nullptr)
    return SEScalar::dNaN();
  return m_HeartStrokeVolume->GetValue(unit);
}

bool SECardiovascularSystem::HasIntracranialPressure() const
{
  return m_IntracranialPressure == nullptr ? false : m_IntracranialPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetIntracranialPressure()
{
  if (m_IntracranialPressure == nullptr)
    m_IntracranialPressure = new SEScalarPressure();
  return *m_IntracranialPressure;
}
double SECardiovascularSystem::GetIntracranialPressure(const PressureUnit& unit) const
{
  if (m_IntracranialPressure == nullptr)
    return SEScalar::dNaN();
  return m_IntracranialPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasMeanArterialPressure() const
{
  return m_MeanArterialPressure == nullptr ? false : m_MeanArterialPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetMeanArterialPressure()
{
  if (m_MeanArterialPressure == nullptr)
    m_MeanArterialPressure = new SEScalarPressure();
  return *m_MeanArterialPressure;
}
double SECardiovascularSystem::GetMeanArterialPressure(const PressureUnit& unit) const
{
  if (m_MeanArterialPressure == nullptr)
    return SEScalar::dNaN();
  return m_MeanArterialPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasMeanArterialCarbonDioxidePartialPressure() const
{
  return m_MeanArterialCarbonDioxidePartialPressure == nullptr ? false : m_MeanArterialCarbonDioxidePartialPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetMeanArterialCarbonDioxidePartialPressure()
{
  if (m_MeanArterialCarbonDioxidePartialPressure == nullptr)
    m_MeanArterialCarbonDioxidePartialPressure = new SEScalarPressure();
  return *m_MeanArterialCarbonDioxidePartialPressure;
}
double SECardiovascularSystem::GetMeanArterialCarbonDioxidePartialPressure(const PressureUnit& unit) const
{
  if (m_MeanArterialCarbonDioxidePartialPressure == nullptr)
    return SEScalar::dNaN();
  return m_MeanArterialCarbonDioxidePartialPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasMeanArterialCarbonDioxidePartialPressureDelta() const
{
  return m_MeanArterialCarbonDioxidePartialPressureDelta == nullptr ? false : m_MeanArterialCarbonDioxidePartialPressureDelta->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetMeanArterialCarbonDioxidePartialPressureDelta()
{
  if (m_MeanArterialCarbonDioxidePartialPressureDelta == nullptr)
    m_MeanArterialCarbonDioxidePartialPressureDelta = new SEScalarPressure();
  return *m_MeanArterialCarbonDioxidePartialPressureDelta;
}
double SECardiovascularSystem::GetMeanArterialCarbonDioxidePartialPressureDelta(const PressureUnit& unit) const
{
  if (m_MeanArterialCarbonDioxidePartialPressureDelta == nullptr)
    return SEScalar::dNaN();
  return m_MeanArterialCarbonDioxidePartialPressureDelta->GetValue(unit);
}

bool SECardiovascularSystem::HasMeanCentralVenousPressure() const
{
  return m_MeanCentralVenousPressure == nullptr ? false : m_MeanCentralVenousPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetMeanCentralVenousPressure()
{
  if (m_MeanCentralVenousPressure == nullptr)
    m_MeanCentralVenousPressure = new SEScalarPressure();
  return *m_MeanCentralVenousPressure;
}
double SECardiovascularSystem::GetMeanCentralVenousPressure(const PressureUnit& unit) const
{
  if (m_MeanCentralVenousPressure == nullptr)
    return SEScalar::dNaN();
  return m_MeanCentralVenousPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasMeanSkinFlow() const
{
  return m_MeanSkinFlow == nullptr ? false : m_MeanSkinFlow->IsValid();
}
SEScalarVolumePerTime& SECardiovascularSystem::GetMeanSkinFlow()
{
  if (m_MeanSkinFlow == nullptr)
    m_MeanSkinFlow = new SEScalarVolumePerTime();
  return *m_MeanSkinFlow;
}
double SECardiovascularSystem::GetMeanSkinFlow(const VolumePerTimeUnit& unit) const
{
  if (m_MeanSkinFlow == nullptr)
    return SEScalar::dNaN();
  return m_MeanSkinFlow->GetValue(unit);
}

bool SECardiovascularSystem::HasPeripheralPerfusionIndex() const
{
  return m_PeripheralPerfusionIndex == nullptr ? false : m_PeripheralPerfusionIndex->IsValid();
}
SEScalar0To1& SECardiovascularSystem::GetPeripheralPerfusionIndex()
{
  if (m_PeripheralPerfusionIndex == nullptr)
    m_PeripheralPerfusionIndex = new SEScalar0To1();
  return *m_PeripheralPerfusionIndex;
}
double SECardiovascularSystem::GetPeripheralPerfusionIndex() const
{
  if (m_PeripheralPerfusionIndex == nullptr)
    return SEScalar::dNaN();
  return m_PeripheralPerfusionIndex->GetValue();
}

bool SECardiovascularSystem::HasPulmonaryArterialPressure() const
{
  return m_PulmonaryArterialPressure == nullptr ? false : m_PulmonaryArterialPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetPulmonaryArterialPressure()
{
  if (m_PulmonaryArterialPressure == nullptr)
    m_PulmonaryArterialPressure = new SEScalarPressure();
  return *m_PulmonaryArterialPressure;
}
double SECardiovascularSystem::GetPulmonaryArterialPressure(const PressureUnit& unit) const
{
  if (m_PulmonaryArterialPressure == nullptr)
    return SEScalar::dNaN();
  return m_PulmonaryArterialPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasPulmonaryCapillariesCoverageFraction() const
{
  return m_PulmonaryCapillariesCoverageFraction == nullptr ? false : m_PulmonaryCapillariesCoverageFraction->IsValid();
}
SEScalar0To1& SECardiovascularSystem::GetPulmonaryCapillariesCoverageFraction()
{
  if (m_PulmonaryCapillariesCoverageFraction == nullptr)
    m_PulmonaryCapillariesCoverageFraction = new SEScalar0To1();
  return *m_PulmonaryCapillariesCoverageFraction;
}
double SECardiovascularSystem::GetPulmonaryCapillariesCoverageFraction() const
{
  if (m_PulmonaryCapillariesCoverageFraction == nullptr)
    return SEScalar::dNaN();
  return m_PulmonaryCapillariesCoverageFraction->GetValue();
}

bool SECardiovascularSystem::HasPulmonaryCapillariesWedgePressure() const
{
  return m_PulmonaryCapillariesWedgePressure==nullptr?false:m_PulmonaryCapillariesWedgePressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetPulmonaryCapillariesWedgePressure()
{
  if(m_PulmonaryCapillariesWedgePressure==nullptr)
    m_PulmonaryCapillariesWedgePressure=new SEScalarPressure();
  return *m_PulmonaryCapillariesWedgePressure;
}
double SECardiovascularSystem::GetPulmonaryCapillariesWedgePressure(const PressureUnit& unit) const
{
  if (m_PulmonaryCapillariesWedgePressure == nullptr)
    return SEScalar::dNaN();
  return m_PulmonaryCapillariesWedgePressure->GetValue(unit);
}

bool SECardiovascularSystem::HasPulmonaryDiastolicArterialPressure() const
{
  return m_PulmonaryDiastolicArterialPressure==nullptr?false:m_PulmonaryDiastolicArterialPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetPulmonaryDiastolicArterialPressure()
{
  if(m_PulmonaryDiastolicArterialPressure==nullptr)
    m_PulmonaryDiastolicArterialPressure=new SEScalarPressure();
  return *m_PulmonaryDiastolicArterialPressure;
}
double SECardiovascularSystem::GetPulmonaryDiastolicArterialPressure(const PressureUnit& unit) const
{
  if (m_PulmonaryDiastolicArterialPressure == nullptr)
    return SEScalar::dNaN();
  return m_PulmonaryDiastolicArterialPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasPulmonaryMeanArterialPressure() const
{
  return m_PulmonaryMeanArterialPressure==nullptr?false:m_PulmonaryMeanArterialPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetPulmonaryMeanArterialPressure()
{
  if(m_PulmonaryMeanArterialPressure==nullptr)
    m_PulmonaryMeanArterialPressure=new SEScalarPressure();
  return *m_PulmonaryMeanArterialPressure;
}
double SECardiovascularSystem::GetPulmonaryMeanArterialPressure(const PressureUnit& unit) const
{
  if (m_PulmonaryMeanArterialPressure == nullptr)
    return SEScalar::dNaN();
  return m_PulmonaryMeanArterialPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasPulmonaryMeanCapillaryFlow() const
{
  return m_PulmonaryMeanCapillaryFlow==nullptr?false:m_PulmonaryMeanCapillaryFlow->IsValid();
}
SEScalarVolumePerTime& SECardiovascularSystem::GetPulmonaryMeanCapillaryFlow()
{
  if(m_PulmonaryMeanCapillaryFlow==nullptr)
    m_PulmonaryMeanCapillaryFlow=new SEScalarVolumePerTime();
  return *m_PulmonaryMeanCapillaryFlow;
}
double SECardiovascularSystem::GetPulmonaryMeanCapillaryFlow(const VolumePerTimeUnit& unit) const
{
  if (m_PulmonaryMeanCapillaryFlow == nullptr)
    return SEScalar::dNaN();
  return m_PulmonaryMeanCapillaryFlow->GetValue(unit);
}

bool SECardiovascularSystem::HasPulmonaryMeanShuntFlow() const
{
  return m_PulmonaryMeanShuntFlow==nullptr?false:m_PulmonaryMeanShuntFlow->IsValid();
}
SEScalarVolumePerTime& SECardiovascularSystem::GetPulmonaryMeanShuntFlow()
{
  if(m_PulmonaryMeanShuntFlow==nullptr)
    m_PulmonaryMeanShuntFlow=new SEScalarVolumePerTime();
  return *m_PulmonaryMeanShuntFlow;
}
double SECardiovascularSystem::GetPulmonaryMeanShuntFlow(const VolumePerTimeUnit& unit) const
{
  if (m_PulmonaryMeanShuntFlow == nullptr)
    return SEScalar::dNaN();
  return m_PulmonaryMeanShuntFlow->GetValue(unit);
}

bool SECardiovascularSystem::HasSystemicVascularResistance() const
{
  return m_SystemicVascularResistance == nullptr ? false : m_SystemicVascularResistance->IsValid();
}
SEScalarPressureTimePerVolume& SECardiovascularSystem::GetSystemicVascularResistance()
{
  if (m_SystemicVascularResistance == nullptr)
    m_SystemicVascularResistance = new SEScalarPressureTimePerVolume();
  return *m_SystemicVascularResistance;
}
double SECardiovascularSystem::GetSystemicVascularResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_SystemicVascularResistance == nullptr)
    return SEScalar::dNaN();
  return m_SystemicVascularResistance->GetValue(unit);
}

bool SECardiovascularSystem::HasPulmonarySystolicArterialPressure() const
{
  return m_PulmonarySystolicArterialPressure==nullptr?false:m_PulmonarySystolicArterialPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetPulmonarySystolicArterialPressure()
{
  if(m_PulmonarySystolicArterialPressure==nullptr)
    m_PulmonarySystolicArterialPressure=new SEScalarPressure();
  return *m_PulmonarySystolicArterialPressure;
}
double SECardiovascularSystem::GetPulmonarySystolicArterialPressure(const PressureUnit& unit) const
{
  if (m_PulmonarySystolicArterialPressure == nullptr)
    return SEScalar::dNaN();
  return m_PulmonarySystolicArterialPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasPulmonaryVascularResistance() const
{
  return m_PulmonaryVascularResistance == nullptr ? false : m_PulmonaryVascularResistance->IsValid();
}
SEScalarPressureTimePerVolume& SECardiovascularSystem::GetPulmonaryVascularResistance()
{
  if (m_PulmonaryVascularResistance == nullptr)
    m_PulmonaryVascularResistance = new SEScalarPressureTimePerVolume();
  return *m_PulmonaryVascularResistance;
}
double SECardiovascularSystem::GetPulmonaryVascularResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_PulmonaryVascularResistance == nullptr)
    return SEScalar::dNaN();
  return m_PulmonaryVascularResistance->GetValue(unit);
}

bool SECardiovascularSystem::HasPulmonaryVascularResistanceIndex() const
{
  return m_PulmonaryVascularResistanceIndex == nullptr ? false : m_PulmonaryVascularResistanceIndex->IsValid();
}
SEScalarPressureTimePerVolumeArea& SECardiovascularSystem::GetPulmonaryVascularResistanceIndex()
{
  if (m_PulmonaryVascularResistanceIndex == nullptr)
    m_PulmonaryVascularResistanceIndex = new SEScalarPressureTimePerVolumeArea();
  return *m_PulmonaryVascularResistanceIndex;
}
double SECardiovascularSystem::GetPulmonaryVascularResistanceIndex(const PressureTimePerVolumeAreaUnit& unit) const
{
  if (m_PulmonaryVascularResistanceIndex == nullptr)
    return SEScalar::dNaN();
  return m_PulmonaryVascularResistanceIndex->GetValue(unit);
}

bool SECardiovascularSystem::HasPulsePressure() const
{
  return m_PulsePressure==nullptr?false:m_PulsePressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetPulsePressure()
{
  if(m_PulsePressure==nullptr)
    m_PulsePressure=new SEScalarPressure();
  return *m_PulsePressure;
}
double SECardiovascularSystem::GetPulsePressure(const PressureUnit& unit) const
{
  if (m_PulsePressure == nullptr)
    return SEScalar::dNaN();
  return m_PulsePressure->GetValue(unit);
}

bool SECardiovascularSystem::HasSystolicArterialPressure() const
{
  return m_SystolicArterialPressure==nullptr?false:m_SystolicArterialPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetSystolicArterialPressure()
{
  if(m_SystolicArterialPressure==nullptr)
    m_SystolicArterialPressure=new SEScalarPressure();
  return *m_SystolicArterialPressure;
}
double SECardiovascularSystem::GetSystolicArterialPressure(const PressureUnit& unit) const
{
  if (m_SystolicArterialPressure == nullptr)
    return SEScalar::dNaN();
  return m_SystolicArterialPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasSystolicLeftHeartPressure() const
{
  return m_SystolicLeftHeartPressure == nullptr ? false : m_SystolicLeftHeartPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetSystolicLeftHeartPressure()
{
  if (m_SystolicLeftHeartPressure == nullptr)
    m_SystolicLeftHeartPressure = new SEScalarPressure();
  return *m_SystolicLeftHeartPressure;
}
double SECardiovascularSystem::GetSystolicLeftHeartPressure(const PressureUnit& unit) const
{
  if (m_SystolicLeftHeartPressure == nullptr)
    return SEScalar::dNaN();
  return m_SystolicLeftHeartPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasSystolicRightHeartPressure() const
{
  return m_SystolicRightHeartPressure == nullptr ? false : m_SystolicRightHeartPressure->IsValid();
}
SEScalarPressure& SECardiovascularSystem::GetSystolicRightHeartPressure()
{
  if (m_SystolicRightHeartPressure == nullptr)
    m_SystolicRightHeartPressure = new SEScalarPressure();
  return *m_SystolicRightHeartPressure;
}
double SECardiovascularSystem::GetSystolicRightHeartPressure(const PressureUnit& unit) const
{
  if (m_SystolicRightHeartPressure == nullptr)
    return SEScalar::dNaN();
  return m_SystolicRightHeartPressure->GetValue(unit);
}

bool SECardiovascularSystem::HasTotalHemorrhageRate() const
{
  return m_TotalHemorrhageRate == nullptr ? false : m_TotalHemorrhageRate->IsValid();
}
SEScalarVolumePerTime& SECardiovascularSystem::GetTotalHemorrhageRate()
{
  if (m_TotalHemorrhageRate == nullptr)
    m_TotalHemorrhageRate = new SEScalarVolumePerTime();
  return *m_TotalHemorrhageRate;
}
double SECardiovascularSystem::GetTotalHemorrhageRate(const VolumePerTimeUnit& unit) const
{
  if (m_TotalHemorrhageRate == nullptr)
    return SEScalar::dNaN();
  return m_TotalHemorrhageRate->GetValue(unit);
}

bool SECardiovascularSystem::HasTotalHemorrhagedVolume() const
{
  return m_TotalHemorrhagedVolume == nullptr ? false : m_TotalHemorrhagedVolume->IsValid();
}
SEScalarVolume& SECardiovascularSystem::GetTotalHemorrhagedVolume()
{
  if (m_TotalHemorrhagedVolume == nullptr)
    m_TotalHemorrhagedVolume = new SEScalarVolume();
  return *m_TotalHemorrhagedVolume;
}
double SECardiovascularSystem::GetTotalHemorrhagedVolume(const VolumeUnit& unit) const
{
  if (m_TotalHemorrhagedVolume == nullptr)
    return SEScalar::dNaN();
  return m_TotalHemorrhagedVolume->GetValue(unit);
}

bool SECardiovascularSystem::HasTotalPulmonaryPerfusion() const
{
  return m_TotalPulmonaryPerfusion == nullptr ? false : m_TotalPulmonaryPerfusion->IsValid();
}
SEScalarVolumePerTime& SECardiovascularSystem::GetTotalPulmonaryPerfusion()
{
  if (m_TotalPulmonaryPerfusion == nullptr)
    m_TotalPulmonaryPerfusion = new SEScalarVolumePerTime();
  return *m_TotalPulmonaryPerfusion;
}
double SECardiovascularSystem::GetTotalPulmonaryPerfusion(const VolumePerTimeUnit& unit) const
{
  if (m_TotalPulmonaryPerfusion == nullptr)
    return SEScalar::dNaN();
  return m_TotalPulmonaryPerfusion->GetValue(unit);
}
