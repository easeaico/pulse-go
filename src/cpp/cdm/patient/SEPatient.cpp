/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"

#include "cdm/patient/SEPatient.h"
#include "cdm/patient/SENutrition.h"

#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarArea.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarLength.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPower.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarPressurePerVolume.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/io/protobuf/PBPatient.h"


SEPatient::SEPatient(Logger* logger) : Loggable(logger)
{
  m_Name="";
  m_Sex = ePatient_Sex::Male;
  m_Age=nullptr;
  m_Weight=nullptr;
  m_Height=nullptr;

  m_AlveoliSurfaceArea = nullptr;
  m_BasalMetabolicRate = nullptr;
  m_BloodVolumeBaseline = nullptr;
  m_BodyDensity = nullptr;
  m_BodyFatFraction = nullptr;
  m_BodyMassIndex = nullptr;
  m_DiastolicArterialPressureBaseline = nullptr;
  m_ExpiratoryReserveVolume = nullptr;
  m_FunctionalResidualCapacity = nullptr;
  m_HeartRateBaseline = nullptr;
  m_HeartRateMaximum = nullptr;
  m_HeartRateMinimum = nullptr;
  m_IdealBodyWeight = nullptr;
  m_InspiratoryCapacity = nullptr;
  m_InspiratoryReserveVolume = nullptr;
  m_LeanBodyMass = nullptr;
  m_MeanArterialPressureBaseline = nullptr;
  m_PulsePressureBaseline = nullptr;
  m_ResidualVolume = nullptr;
  m_RespirationRateBaseline = nullptr;
  m_RightLungRatio = nullptr;
  m_SkinSurfaceArea = nullptr;
  m_SystolicArterialPressureBaseline = nullptr;
  m_TidalVolumeBaseline = nullptr;
  m_TotalLungCapacity = nullptr;
  m_VitalCapacity = nullptr;
}

SEPatient::~SEPatient()
{
  m_Name = "";
  m_Sex = ePatient_Sex::Male;
  SAFE_DELETE(m_Age);
  SAFE_DELETE(m_Weight);
  SAFE_DELETE(m_Height);

  SAFE_DELETE(m_AlveoliSurfaceArea);
  SAFE_DELETE(m_BasalMetabolicRate);
  SAFE_DELETE(m_BloodVolumeBaseline);
  SAFE_DELETE(m_BodyDensity);
  SAFE_DELETE(m_BodyFatFraction);
  SAFE_DELETE(m_BodyMassIndex);
  SAFE_DELETE(m_DiastolicArterialPressureBaseline);
  SAFE_DELETE(m_ExpiratoryReserveVolume);
  SAFE_DELETE(m_FunctionalResidualCapacity);
  SAFE_DELETE(m_HeartRateBaseline);
  SAFE_DELETE(m_HeartRateMaximum);
  SAFE_DELETE(m_HeartRateMinimum);
  SAFE_DELETE(m_IdealBodyWeight);
  SAFE_DELETE(m_InspiratoryCapacity);
  SAFE_DELETE(m_InspiratoryReserveVolume);
  SAFE_DELETE(m_LeanBodyMass);
  SAFE_DELETE(m_MeanArterialPressureBaseline);
  SAFE_DELETE(m_PulsePressureBaseline);
  SAFE_DELETE(m_ResidualVolume);
  SAFE_DELETE(m_RespirationRateBaseline);
  SAFE_DELETE(m_RightLungRatio);
  SAFE_DELETE(m_SkinSurfaceArea);
  SAFE_DELETE(m_SystolicArterialPressureBaseline);
  SAFE_DELETE(m_TidalVolumeBaseline);
  SAFE_DELETE(m_TotalLungCapacity);
  SAFE_DELETE(m_VitalCapacity);
}

void SEPatient::Clear()
{
  m_Name="";
  m_Sex = ePatient_Sex::Male;
  INVALIDATE_PROPERTY(m_Age);
  INVALIDATE_PROPERTY(m_Weight);
  INVALIDATE_PROPERTY(m_Height);

  INVALIDATE_PROPERTY(m_AlveoliSurfaceArea);
  INVALIDATE_PROPERTY(m_BasalMetabolicRate);
  INVALIDATE_PROPERTY(m_BloodVolumeBaseline);
  INVALIDATE_PROPERTY(m_BodyDensity);
  INVALIDATE_PROPERTY(m_BodyFatFraction);
  INVALIDATE_PROPERTY(m_BodyMassIndex);
  INVALIDATE_PROPERTY(m_DiastolicArterialPressureBaseline);
  INVALIDATE_PROPERTY(m_ExpiratoryReserveVolume);
  INVALIDATE_PROPERTY(m_FunctionalResidualCapacity);
  INVALIDATE_PROPERTY(m_HeartRateBaseline);
  INVALIDATE_PROPERTY(m_HeartRateMaximum);
  INVALIDATE_PROPERTY(m_HeartRateMinimum);
  INVALIDATE_PROPERTY(m_IdealBodyWeight);
  INVALIDATE_PROPERTY(m_InspiratoryCapacity);
  INVALIDATE_PROPERTY(m_InspiratoryReserveVolume);
  INVALIDATE_PROPERTY(m_LeanBodyMass);
  INVALIDATE_PROPERTY(m_MeanArterialPressureBaseline);
  INVALIDATE_PROPERTY(m_PulsePressureBaseline);
  INVALIDATE_PROPERTY(m_ResidualVolume);
  INVALIDATE_PROPERTY(m_RespirationRateBaseline);
  INVALIDATE_PROPERTY(m_RightLungRatio);
  INVALIDATE_PROPERTY(m_SkinSurfaceArea);
  INVALIDATE_PROPERTY(m_SystolicArterialPressureBaseline);
  INVALIDATE_PROPERTY(m_TidalVolumeBaseline);
  INVALIDATE_PROPERTY(m_TotalLungCapacity);
  INVALIDATE_PROPERTY(m_VitalCapacity);
}

void SEPatient::Copy(const SEPatient& src)
{
  PBPatient::Copy(src, *this);
}

bool SEPatient::SerializeToString(std::string& output, eSerializationFormat m) const
{
  return PBPatient::SerializeToString(*this, output, m);
}
bool SEPatient::SerializeToFile(const std::string& filename) const
{
  return PBPatient::SerializeToFile(*this, filename);
}
bool SEPatient::SerializeFromString(const std::string& src, eSerializationFormat m)
{
  return PBPatient::SerializeFromString(src, *this, m);
}
bool SEPatient::SerializeFromFile(const std::string& filename)
{
  return PBPatient::SerializeFromFile(filename, *this);
}

const SEScalar* SEPatient::GetScalar(const std::string& name)
{
  if (name.compare("Age") == 0)
    return &GetAge();
  if (name.compare("Weight") == 0)
    return &GetWeight();
  if (name.compare("Height") == 0)
    return &GetHeight();

  if (name.compare("AlveoliSurfaceArea") == 0)
    return &GetAlveoliSurfaceArea();
  if (name.compare("BasalMetabolicRate") == 0)
    return &GetBasalMetabolicRate();
  if (name.compare("BloodVolumeBaseline") == 0)
    return &GetBloodVolumeBaseline();
  if (name.compare("BodyDensity") == 0)
    return &GetBodyDensity();
  if (name.compare("BodyFatFraction") == 0)
    return &GetBodyFatFraction();
  if (name.compare("BodyMassIndex") == 0)
    return &GetBodyMassIndex();
  if (name.compare("DiastolicArterialPressureBaseline") == 0)
    return &GetDiastolicArterialPressureBaseline();
  if (name.compare("ExpiratoryReserveVolume") == 0)
    return &GetExpiratoryReserveVolume();
  if (name.compare("FunctionalResidualCapacity") == 0)
    return &GetFunctionalResidualCapacity();
  if (name.compare("HeartRateBaseline") == 0)
    return &GetHeartRateBaseline();
  if (name.compare("HeartRateMaximum") == 0)
    return &GetHeartRateMaximum();
  if (name.compare("HeartRateMinimum") == 0)
    return &GetHeartRateMinimum();
  if (name.compare("IdealBodyWeight") == 0)
    return &GetIdealBodyWeight();
  if (name.compare("InspiratoryCapacity") == 0)
    return &GetInspiratoryCapacity();
  if (name.compare("InspiratoryReserveVolume") == 0)
    return &GetInspiratoryReserveVolume();
  if (name.compare("LeanBodyMass") == 0)
    return &GetLeanBodyMass();
  if (name.compare("MeanArterialPressureBaseline") == 0)
    return &GetMeanArterialPressureBaseline();
  if (name.compare("PulsePressureBaseline") == 0)
    return &GetPulsePressureBaseline();
  if (name.compare("ResidualVolume") == 0)
    return &GetResidualVolume();
  if (name.compare("RespirationRateBaseline") == 0)
    return &GetRespirationRateBaseline();
  if (name.compare("RightLungRatio") == 0)
    return &GetRightLungRatio();
  if (name.compare("SkinSurfaceArea") == 0)
    return &GetSkinSurfaceArea();
  if (name.compare("SystolicArterialPressureBaseline") == 0)
    return &GetSystolicArterialPressureBaseline();
  if (name.compare("TidalVolumeBaseline") == 0)
    return &GetTidalVolumeBaseline();
  if (name.compare("TotalLungCapacity") == 0)
    return &GetTotalLungCapacity();
  if (name.compare("VitalCapacity") == 0)
    return &GetVitalCapacity();

  return nullptr;
}

std::string SEPatient::GetName() const
{
  return m_Name;
}
void SEPatient::SetName(const std::string& name)
{
  m_Name = name;
}
bool SEPatient::HasName() const
{
  return m_Name.empty()?false:true;
}
void SEPatient::InvalidateName()
{
  m_Name = "";
}

ePatient_Sex SEPatient::GetSex() const
{
  return m_Sex;
}
void SEPatient::SetSex(ePatient_Sex sex)
{
  m_Sex = sex;
}

bool SEPatient::HasAge() const
{
  return m_Age==nullptr?false:m_Age->IsValid();
}
SEScalarTime& SEPatient::GetAge()
{
  if(m_Age==nullptr)
    m_Age=new SEScalarTime();
  return *m_Age;
}
double SEPatient::GetAge(const TimeUnit& unit) const
{
  if (m_Age == nullptr)
    return SEScalar::dNaN();
  return m_Age->GetValue(unit);
}

bool SEPatient::HasWeight() const
{
  return m_Weight==nullptr?false:m_Weight->IsValid();
}
SEScalarMass& SEPatient::GetWeight()
{
  if(m_Weight==nullptr)
    m_Weight=new SEScalarMass();
  return *m_Weight;
}
double SEPatient::GetWeight(const MassUnit& unit) const
{
  if (m_Weight == nullptr)
    return SEScalar::dNaN();
  return m_Weight->GetValue(unit);
}

bool SEPatient::HasHeight() const
{
  return m_Height==nullptr?false:m_Height->IsValid();
}
SEScalarLength& SEPatient::GetHeight()
{
  if(m_Height==nullptr)
    m_Height=new SEScalarLength();
  return *m_Height;
}
double SEPatient::GetHeight(const LengthUnit& unit) const
{
  if (m_Height == nullptr)
    return SEScalar::dNaN();
  return m_Height->GetValue(unit);
}

bool SEPatient::HasAlveoliSurfaceArea() const
{
  return m_AlveoliSurfaceArea == nullptr ? false : m_AlveoliSurfaceArea->IsValid();
}
SEScalarArea& SEPatient::GetAlveoliSurfaceArea()
{
  if (m_AlveoliSurfaceArea == nullptr)
    m_AlveoliSurfaceArea = new SEScalarArea();
  return *m_AlveoliSurfaceArea;
}
double SEPatient::GetAlveoliSurfaceArea(const AreaUnit& unit) const
{
  if (m_AlveoliSurfaceArea == nullptr)
    return SEScalar::dNaN();
  return m_AlveoliSurfaceArea->GetValue(unit);
}

bool SEPatient::HasBasalMetabolicRate() const
{
  return m_BasalMetabolicRate == nullptr ? false : m_BasalMetabolicRate->IsValid();
}
SEScalarPower& SEPatient::GetBasalMetabolicRate()
{
  if (m_BasalMetabolicRate == nullptr)
    m_BasalMetabolicRate = new SEScalarPower();
  return *m_BasalMetabolicRate;
}
double SEPatient::GetBasalMetabolicRate(const PowerUnit& unit) const
{
  if (m_BasalMetabolicRate == nullptr)
    return SEScalar::dNaN();
  return m_BasalMetabolicRate->GetValue(unit);
}

bool SEPatient::HasBloodVolumeBaseline() const
{
  return m_BloodVolumeBaseline == nullptr ? false : m_BloodVolumeBaseline->IsValid();
}
SEScalarVolume& SEPatient::GetBloodVolumeBaseline()
{
  if (m_BloodVolumeBaseline == nullptr)
    m_BloodVolumeBaseline = new SEScalarVolume();
  return *m_BloodVolumeBaseline;
}
double SEPatient::GetBloodVolumeBaseline(const VolumeUnit& unit) const
{
  if (m_BloodVolumeBaseline == nullptr)
    return SEScalar::dNaN();
  return m_BloodVolumeBaseline->GetValue(unit);
}

bool SEPatient::HasBodyDensity() const
{
  return m_BodyDensity == nullptr ? false : m_BodyDensity->IsValid();
}
SEScalarMassPerVolume& SEPatient::GetBodyDensity()
{
  if (m_BodyDensity == nullptr)
    m_BodyDensity = new SEScalarMassPerVolume();
  return *m_BodyDensity;
}
double SEPatient::GetBodyDensity(const MassPerVolumeUnit& unit) const
{
  if (m_BodyDensity == nullptr)
    return SEScalar::dNaN();
  return m_BodyDensity->GetValue(unit);
}

bool SEPatient::HasBodyFatFraction() const
{
  return m_BodyFatFraction == nullptr ? false : m_BodyFatFraction->IsValid();
}
SEScalar0To1& SEPatient::GetBodyFatFraction()
{
  if (m_BodyFatFraction == nullptr)
    m_BodyFatFraction = new SEScalar0To1();
  return *m_BodyFatFraction;
}
double SEPatient::GetBodyFatFraction() const
{
  if (m_BodyFatFraction == nullptr)
    return SEScalar::dNaN();
  return m_BodyFatFraction->GetValue();
}

bool SEPatient::HasBodyMassIndex() const
{
  return m_BodyMassIndex == nullptr ? false : m_BodyMassIndex->IsValid();
}
SEScalar& SEPatient::GetBodyMassIndex()
{
  if (m_BodyMassIndex == nullptr)
    m_BodyMassIndex = new SEScalar();
  return *m_BodyMassIndex;
}
double SEPatient::GetBodyMassIndex() const
{
  if (m_BodyMassIndex == nullptr)
    return SEScalar::dNaN();
  return m_BodyMassIndex->GetValue();
}

bool SEPatient::HasDiastolicArterialPressureBaseline() const
{
  return m_DiastolicArterialPressureBaseline == nullptr ? false : m_DiastolicArterialPressureBaseline->IsValid();
}
SEScalarPressure& SEPatient::GetDiastolicArterialPressureBaseline()
{
  if (m_DiastolicArterialPressureBaseline == nullptr)
    m_DiastolicArterialPressureBaseline = new SEScalarPressure();
  return *m_DiastolicArterialPressureBaseline;
}
double SEPatient::GetDiastolicArterialPressureBaseline(const PressureUnit& unit) const
{
  if (m_DiastolicArterialPressureBaseline == nullptr)
    return SEScalar::dNaN();
  return m_DiastolicArterialPressureBaseline->GetValue(unit);
}

bool SEPatient::HasExpiratoryReserveVolume() const
{
  return m_ExpiratoryReserveVolume==nullptr?false:m_ExpiratoryReserveVolume->IsValid();
}
SEScalarVolume& SEPatient::GetExpiratoryReserveVolume()
{
  if(m_ExpiratoryReserveVolume==nullptr)
    m_ExpiratoryReserveVolume=new SEScalarVolume();
  return *m_ExpiratoryReserveVolume;
}
double SEPatient::GetExpiratoryReserveVolume(const VolumeUnit& unit) const
{
  if (m_ExpiratoryReserveVolume == nullptr)
    return SEScalar::dNaN();
  return m_ExpiratoryReserveVolume->GetValue(unit);
}

bool SEPatient::HasFunctionalResidualCapacity() const
{
  return m_FunctionalResidualCapacity==nullptr?false:m_FunctionalResidualCapacity->IsValid();
}
SEScalarVolume& SEPatient::GetFunctionalResidualCapacity()
{
  if(m_FunctionalResidualCapacity==nullptr)
    m_FunctionalResidualCapacity=new SEScalarVolume();
  return *m_FunctionalResidualCapacity;
}
double SEPatient::GetFunctionalResidualCapacity(const VolumeUnit& unit) const
{
  if (m_FunctionalResidualCapacity == nullptr)
    return SEScalar::dNaN();
  return m_FunctionalResidualCapacity->GetValue(unit);
}

bool SEPatient::HasHeartRateBaseline() const
{
  return m_HeartRateBaseline==nullptr?false:m_HeartRateBaseline->IsValid();
}
SEScalarFrequency& SEPatient::GetHeartRateBaseline()
{
  if(m_HeartRateBaseline==nullptr)
    m_HeartRateBaseline=new SEScalarFrequency();
  return *m_HeartRateBaseline;
}
double SEPatient::GetHeartRateBaseline(const FrequencyUnit& unit) const
{
  if (m_HeartRateBaseline == nullptr)
    return SEScalar::dNaN();
  return m_HeartRateBaseline->GetValue(unit);
}

bool SEPatient::HasHeartRateMaximum() const
{
  return m_HeartRateMaximum==nullptr?false:m_HeartRateMaximum->IsValid();
}
SEScalarFrequency& SEPatient::GetHeartRateMaximum()
{
  if(m_HeartRateMaximum==nullptr)
    m_HeartRateMaximum=new SEScalarFrequency();
  return *m_HeartRateMaximum;
}
double SEPatient::GetHeartRateMaximum(const FrequencyUnit& unit) const
{
  if (m_HeartRateMaximum == nullptr)
    return SEScalar::dNaN();
  return m_HeartRateMaximum->GetValue(unit);
}

bool SEPatient::HasHeartRateMinimum() const
{
  return m_HeartRateMinimum==nullptr?false:m_HeartRateMinimum->IsValid();
}
SEScalarFrequency& SEPatient::GetHeartRateMinimum()
{
  if(m_HeartRateMinimum==nullptr)
    m_HeartRateMinimum=new SEScalarFrequency();
  return *m_HeartRateMinimum;
}
double SEPatient::GetHeartRateMinimum(const FrequencyUnit& unit) const
{
  if (m_HeartRateMinimum == nullptr)
    return SEScalar::dNaN();
  return m_HeartRateMinimum->GetValue(unit);
}

bool SEPatient::HasIdealBodyWeight() const
{
  return m_IdealBodyWeight == nullptr ? false : m_IdealBodyWeight->IsValid();
}
SEScalarMass& SEPatient::GetIdealBodyWeight()
{
  if (m_IdealBodyWeight == nullptr)
    m_IdealBodyWeight = new SEScalarMass();
  return *m_IdealBodyWeight;
}
double SEPatient::GetIdealBodyWeight(const MassUnit& unit) const
{
  if (m_IdealBodyWeight == nullptr)
    return SEScalar::dNaN();
  return m_IdealBodyWeight->GetValue(unit);
}

bool SEPatient::HasInspiratoryCapacity() const
{
  return m_InspiratoryCapacity == nullptr ? false : m_InspiratoryCapacity->IsValid();
}
SEScalarVolume& SEPatient::GetInspiratoryCapacity()
{
  if (m_InspiratoryCapacity == nullptr)
    m_InspiratoryCapacity = new SEScalarVolume();
  return *m_InspiratoryCapacity;
}
double SEPatient::GetInspiratoryCapacity(const VolumeUnit& unit) const
{
  if (m_InspiratoryCapacity == nullptr)
    return SEScalar::dNaN();
  return m_InspiratoryCapacity->GetValue(unit);
}

bool SEPatient::HasInspiratoryReserveVolume() const
{
  return m_InspiratoryReserveVolume == nullptr ? false : m_InspiratoryReserveVolume->IsValid();
}
SEScalarVolume& SEPatient::GetInspiratoryReserveVolume()
{
  if (m_InspiratoryReserveVolume == nullptr)
    m_InspiratoryReserveVolume = new SEScalarVolume();
  return *m_InspiratoryReserveVolume;
}
double SEPatient::GetInspiratoryReserveVolume(const VolumeUnit& unit) const
{
  if (m_InspiratoryReserveVolume == nullptr)
    return SEScalar::dNaN();
  return m_InspiratoryReserveVolume->GetValue(unit);
}

bool SEPatient::HasLeanBodyMass() const
{
  return m_LeanBodyMass == nullptr ? false : m_LeanBodyMass->IsValid();
}
SEScalarMass& SEPatient::GetLeanBodyMass()
{
  if (m_LeanBodyMass == nullptr)
    m_LeanBodyMass = new SEScalarMass();
  return *m_LeanBodyMass;
}
double SEPatient::GetLeanBodyMass(const MassUnit& unit) const
{
  if (m_LeanBodyMass == nullptr)
    return SEScalar::dNaN();
  return m_LeanBodyMass->GetValue(unit);
}

bool SEPatient::HasMeanArterialPressureBaseline() const
{
  return m_MeanArterialPressureBaseline==nullptr?false:m_MeanArterialPressureBaseline->IsValid();
}
SEScalarPressure& SEPatient::GetMeanArterialPressureBaseline()
{
  if(m_MeanArterialPressureBaseline==nullptr)
    m_MeanArterialPressureBaseline=new SEScalarPressure();
  return *m_MeanArterialPressureBaseline;
}
double SEPatient::GetMeanArterialPressureBaseline(const PressureUnit& unit) const
{
  if (m_MeanArterialPressureBaseline == nullptr)
    return SEScalar::dNaN();
  return m_MeanArterialPressureBaseline->GetValue(unit);
}

bool SEPatient::HasPulsePressureBaseline() const
{
  return m_PulsePressureBaseline==nullptr?false:m_PulsePressureBaseline->IsValid();
}
SEScalarPressure& SEPatient::GetPulsePressureBaseline()
{
  if(m_PulsePressureBaseline==nullptr)
    m_PulsePressureBaseline=new SEScalarPressure();
  return *m_PulsePressureBaseline;
}
double SEPatient::GetPulsePressureBaseline(const PressureUnit& unit) const
{
  if (m_PulsePressureBaseline == nullptr)
    return SEScalar::dNaN();
  return m_PulsePressureBaseline->GetValue(unit);
}

bool SEPatient::HasResidualVolume() const
{
  return m_ResidualVolume == nullptr ? false : m_ResidualVolume->IsValid();
}
SEScalarVolume& SEPatient::GetResidualVolume()
{
  if (m_ResidualVolume == nullptr)
    m_ResidualVolume = new SEScalarVolume();
  return *m_ResidualVolume;
}
double SEPatient::GetResidualVolume(const VolumeUnit& unit) const
{
  if (m_ResidualVolume == nullptr)
    return SEScalar::dNaN();
  return m_ResidualVolume->GetValue(unit);
}

bool SEPatient::HasRespirationRateBaseline() const
{
  return m_RespirationRateBaseline==nullptr?false:m_RespirationRateBaseline->IsValid();
}
SEScalarFrequency& SEPatient::GetRespirationRateBaseline()
{
  if(m_RespirationRateBaseline==nullptr)
    m_RespirationRateBaseline=new SEScalarFrequency();
  return *m_RespirationRateBaseline;
}
double SEPatient::GetRespirationRateBaseline(const FrequencyUnit& unit) const
{
  if (m_RespirationRateBaseline == nullptr)
    return SEScalar::dNaN();
  return m_RespirationRateBaseline->GetValue(unit);
}

bool SEPatient::HasRightLungRatio() const
{
  return m_RightLungRatio==nullptr?false:m_RightLungRatio->IsValid();
}
SEScalar0To1& SEPatient::GetRightLungRatio()
{
  if(m_RightLungRatio==nullptr)
    m_RightLungRatio=new SEScalar0To1();
  return *m_RightLungRatio;
}
double SEPatient::GetRightLungRatio() const
{
  if (m_RightLungRatio == nullptr)
    return SEScalar::dNaN();
  return m_RightLungRatio->GetValue();
}

bool SEPatient::HasSkinSurfaceArea() const
{
  return m_SkinSurfaceArea == nullptr ? false : m_SkinSurfaceArea->IsValid();
}
SEScalarArea& SEPatient::GetSkinSurfaceArea()
{
  if (m_SkinSurfaceArea == nullptr)
    m_SkinSurfaceArea = new SEScalarArea();
  return *m_SkinSurfaceArea;
}
double SEPatient::GetSkinSurfaceArea(const AreaUnit& unit) const
{
  if (m_SkinSurfaceArea == nullptr)
    return SEScalar::dNaN();
  return m_SkinSurfaceArea->GetValue(unit);
}

bool SEPatient::HasSystolicArterialPressureBaseline() const
{
  return m_SystolicArterialPressureBaseline == nullptr ? false : m_SystolicArterialPressureBaseline->IsValid();
}
SEScalarPressure& SEPatient::GetSystolicArterialPressureBaseline()
{
  if (m_SystolicArterialPressureBaseline == nullptr)
    m_SystolicArterialPressureBaseline = new SEScalarPressure();
  return *m_SystolicArterialPressureBaseline;
}
double SEPatient::GetSystolicArterialPressureBaseline(const PressureUnit& unit) const
{
  if (m_SystolicArterialPressureBaseline == nullptr)
    return SEScalar::dNaN();
  return m_SystolicArterialPressureBaseline->GetValue(unit);
}

bool SEPatient::HasTidalVolumeBaseline() const
{
  return m_TidalVolumeBaseline == nullptr ? false : m_TidalVolumeBaseline->IsValid();
}
SEScalarVolume& SEPatient::GetTidalVolumeBaseline()
{
  if (m_TidalVolumeBaseline == nullptr)
    m_TidalVolumeBaseline = new SEScalarVolume();
  return *m_TidalVolumeBaseline;
}
double SEPatient::GetTidalVolumeBaseline(const VolumeUnit& unit) const
{
  if (m_TidalVolumeBaseline == nullptr)
    return SEScalar::dNaN();
  return m_TidalVolumeBaseline->GetValue(unit);
}

bool SEPatient::HasTotalLungCapacity() const
{
  return m_TotalLungCapacity==nullptr?false:m_TotalLungCapacity->IsValid();
}
SEScalarVolume& SEPatient::GetTotalLungCapacity()
{
  if(m_TotalLungCapacity==nullptr)
    m_TotalLungCapacity=new SEScalarVolume();
  return *m_TotalLungCapacity;
}
double SEPatient::GetTotalLungCapacity(const VolumeUnit& unit) const
{
  if (m_TotalLungCapacity == nullptr)
    return SEScalar::dNaN();
  return m_TotalLungCapacity->GetValue(unit);
}

bool SEPatient::HasVitalCapacity() const
{
  return m_VitalCapacity == nullptr ? false : m_VitalCapacity->IsValid();
}
SEScalarVolume& SEPatient::GetVitalCapacity()
{
  if (m_VitalCapacity == nullptr)
    m_VitalCapacity = new SEScalarVolume();
  return *m_VitalCapacity;
}
double SEPatient::GetVitalCapacity(const VolumeUnit& unit) const
{
  if (m_VitalCapacity == nullptr)
    return SEScalar::dNaN();
  return m_VitalCapacity->GetValue(unit);
}
