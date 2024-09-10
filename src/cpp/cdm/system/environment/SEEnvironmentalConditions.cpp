/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/system/environment/SEEnvironmentalConditions.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/substance/SESubstanceFraction.h"
#include "cdm/substance/SESubstanceConcentration.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarHeatConductancePerArea.h"
#include "cdm/properties/SEScalarHeatResistanceArea.h"
#include "cdm/properties/SEScalarLengthPerTime.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPower.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTemperature.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/io/protobuf/PBEnvironment.h"


SEEnvironmentalConditions::SEEnvironmentalConditions(Logger* logger) : Loggable(logger)
{
  m_SurroundingType = eSurroundingType::NullSurrounding;

  m_AirDensity=nullptr;
  m_AirVelocity = nullptr;
  m_AmbientTemperature = nullptr;
  m_AtmosphericPressure = nullptr;
  m_ClothingResistance = nullptr;
  m_Emissivity = nullptr;
  m_MeanRadiantTemperature = nullptr;
  m_MechanicalDeadSpace = nullptr;
  m_RelativeHumidity = nullptr;
  m_RespirationAmbientTemperature = nullptr;
}

SEEnvironmentalConditions::~SEEnvironmentalConditions()
{
  m_SurroundingType = eSurroundingType::NullSurrounding;
  SAFE_DELETE(m_AirDensity);
  SAFE_DELETE(m_AirVelocity);
  SAFE_DELETE(m_AmbientTemperature);
  SAFE_DELETE(m_AtmosphericPressure);
  SAFE_DELETE(m_ClothingResistance);
  SAFE_DELETE(m_Emissivity);
  SAFE_DELETE(m_MeanRadiantTemperature);
  SAFE_DELETE(m_MechanicalDeadSpace);
  SAFE_DELETE(m_RelativeHumidity);
  SAFE_DELETE(m_RespirationAmbientTemperature);

  DELETE_VECTOR(m_AmbientGases);
  m_cAmbientGases.clear();

  DELETE_VECTOR(m_AmbientAerosols);
  m_cAmbientAerosols.clear();
}

void SEEnvironmentalConditions::Clear()
{
  m_SurroundingType = eSurroundingType::NullSurrounding;
  INVALIDATE_PROPERTY(m_AirDensity);
  INVALIDATE_PROPERTY(m_AirVelocity);
  INVALIDATE_PROPERTY(m_AmbientTemperature);
  INVALIDATE_PROPERTY(m_AtmosphericPressure);
  INVALIDATE_PROPERTY(m_ClothingResistance);
  INVALIDATE_PROPERTY(m_Emissivity);
  INVALIDATE_PROPERTY(m_MeanRadiantTemperature);
  INVALIDATE_PROPERTY(m_MechanicalDeadSpace);
  INVALIDATE_PROPERTY(m_RelativeHumidity);
  INVALIDATE_PROPERTY(m_RespirationAmbientTemperature);
  RemoveAmbientGases();
  RemoveAmbientAerosols();
}

void SEEnvironmentalConditions::Copy(const SEEnvironmentalConditions& src, const SESubstanceManager& subMgr)
{
  PBEnvironment::Copy(src, *this, subMgr);
}

bool SEEnvironmentalConditions::SerializeToString(std::string& output, eSerializationFormat m) const
{
  return PBEnvironment::SerializeToString(*this, output, m);
}
bool SEEnvironmentalConditions::SerializeToFile(const std::string& filename) const
{
  return PBEnvironment::SerializeToFile(*this, filename);
}
bool SEEnvironmentalConditions::SerializeFromString(const std::string& src, eSerializationFormat m, const SESubstanceManager& subMgr)
{
  return PBEnvironment::SerializeFromString(src, *this, m, subMgr);
}
bool SEEnvironmentalConditions::SerializeFromFile(const std::string& filename, const SESubstanceManager& subMgr)
{
  return PBEnvironment::SerializeFromFile(filename, *this, subMgr);
}

const SEScalar* SEEnvironmentalConditions::GetScalar(const std::string& name)
{
  if (name.compare("AirDensity") == 0)
    return &GetAirDensity();
  if (name.compare("AirVelocity") == 0)
    return &GetAirVelocity();
  if (name.compare("AmbientTemperature") == 0)
    return &GetAmbientTemperature();
  if (name.compare("AtmosphericPressure") == 0)
    return &GetAtmosphericPressure();
  if (name.compare("ClothingResistance") == 0)
    return &GetClothingResistance();
  if (name.compare("Emissivity") == 0)
    return &GetEmissivity();
  if (name.compare("MeanRadiantTemperature") == 0)
    return &GetMeanRadiantTemperature();
  if (name.compare("MechanicalDeadSpace") == 0)
    return &GetMechanicalDeadSpace();
  if (name.compare("RelativeHumidity") == 0)
    return &GetRelativeHumidity();
  if (name.compare("RespirationAmbientTemperature") == 0)
    return &GetRespirationAmbientTemperature();
  // I did not support for getting a specific gas/aerosol scalars due to lack of coffee
  return nullptr;
}

void SEEnvironmentalConditions::Merge(const SEEnvironmentalConditions& from, SESubstanceManager& subMgr)
{
  if(from.m_SurroundingType != eSurroundingType::NullSurrounding)
    SetSurroundingType(from.m_SurroundingType);
  COPY_PROPERTY(AirDensity);
  COPY_PROPERTY(AirVelocity);
  COPY_PROPERTY(AmbientTemperature);
  COPY_PROPERTY(AtmosphericPressure);
  COPY_PROPERTY(ClothingResistance);
  COPY_PROPERTY(Emissivity);
  COPY_PROPERTY(MeanRadiantTemperature);
  COPY_PROPERTY(MechanicalDeadSpace);
  COPY_PROPERTY(RelativeHumidity);
  COPY_PROPERTY(RespirationAmbientTemperature);

  // Always need to provide a full (fractions sum to 1) substance list that replaces current
  if (from.HasAmbientGas())
  {
    double amt;
    double total = 0;
    RemoveAmbientGases();
    const SESubstance* sub;
    for (SESubstanceFraction* osf : from.m_AmbientGases)
    {
      sub = subMgr.GetSubstance(osf->GetSubstance().GetName());
      if (sub == nullptr)
      {
        Error("Do not have substance : " + osf->GetSubstance().GetName());
        continue;
      }
      SESubstanceFraction& sf = GetAmbientGas(*sub);
      amt = osf->GetFractionAmount().GetValue();
      sf.GetFractionAmount().SetValue(amt);
      subMgr.AddActiveSubstance(*sub);
      total += amt;
    }
    if (!SEScalar::IsValue(1, total))
      subMgr.Error("Environment Ambient Substance fractions do not sum to 1");
  }

  if (from.HasAmbientAerosol())
  {
    for (SESubstanceConcentration* sc : from.m_AmbientAerosols)
    {
      SESubstanceConcentration& mine = GetAmbientAerosol(sc->GetSubstance());
      mine.GetConcentration().Set(sc->GetConcentration());
    }
  }
}

eSurroundingType SEEnvironmentalConditions::GetSurroundingType() const
{
  return m_SurroundingType;
}
void SEEnvironmentalConditions::SetSurroundingType(eSurroundingType state)
{
  m_SurroundingType = state;
}

bool SEEnvironmentalConditions::HasAirDensity() const
{
  return m_AirDensity == nullptr ? false : m_AirDensity->IsValid();
}
SEScalarMassPerVolume& SEEnvironmentalConditions::GetAirDensity()
{
  if (m_AirDensity == nullptr)
    m_AirDensity = new SEScalarMassPerVolume();
  return *m_AirDensity;
}
double SEEnvironmentalConditions::GetAirDensity(const MassPerVolumeUnit& unit) const
{
  if (m_AirDensity == nullptr)
    return SEScalar::dNaN();
  return m_AirDensity->GetValue(unit);
}

bool SEEnvironmentalConditions::HasAirVelocity() const
{
  return m_AirVelocity == nullptr ? false : m_AirVelocity->IsValid();
}
SEScalarLengthPerTime& SEEnvironmentalConditions::GetAirVelocity()
{
  if (m_AirVelocity == nullptr)
    m_AirVelocity = new SEScalarLengthPerTime();
  return *m_AirVelocity;
}
double SEEnvironmentalConditions::GetAirVelocity(const LengthPerTimeUnit& unit) const
{
  if (m_AirVelocity == nullptr)
    return SEScalar::dNaN();
  return m_AirVelocity->GetValue(unit);
}


bool SEEnvironmentalConditions::HasAmbientTemperature() const
{
  return m_AmbientTemperature==nullptr?false:m_AmbientTemperature->IsValid();
}
SEScalarTemperature& SEEnvironmentalConditions::GetAmbientTemperature()
{
  if(m_AmbientTemperature==nullptr)
    m_AmbientTemperature=new SEScalarTemperature();
  return *m_AmbientTemperature;
}
double SEEnvironmentalConditions::GetAmbientTemperature(const TemperatureUnit& unit) const
{
  if (m_AmbientTemperature == nullptr)
    return SEScalar::dNaN();
  return m_AmbientTemperature->GetValue(unit);
}

bool SEEnvironmentalConditions::HasAtmosphericPressure() const
{
  return m_AtmosphericPressure==nullptr?false:m_AtmosphericPressure->IsValid();
}
SEScalarPressure& SEEnvironmentalConditions::GetAtmosphericPressure()
{
  if(m_AtmosphericPressure==nullptr)
    m_AtmosphericPressure=new SEScalarPressure();
  return *m_AtmosphericPressure;
}
double SEEnvironmentalConditions::GetAtmosphericPressure(const PressureUnit& unit) const
{
  if (m_AtmosphericPressure == nullptr)
    return SEScalar::dNaN();
  return m_AtmosphericPressure->GetValue(unit);
}

bool SEEnvironmentalConditions::HasClothingResistance() const
{
  return m_ClothingResistance == nullptr ? false : m_ClothingResistance->IsValid();
}
SEScalarHeatResistanceArea& SEEnvironmentalConditions::GetClothingResistance()
{
  if (m_ClothingResistance == nullptr)
    m_ClothingResistance = new SEScalarHeatResistanceArea();
  return *m_ClothingResistance;
}
double SEEnvironmentalConditions::GetClothingResistance(const HeatResistanceAreaUnit& unit) const
{
  if (m_ClothingResistance == nullptr)
    return SEScalar::dNaN();
  return m_ClothingResistance->GetValue(unit);
}

bool SEEnvironmentalConditions::HasEmissivity() const
{
  return m_Emissivity == nullptr ? false : m_Emissivity->IsValid();
}
SEScalar0To1& SEEnvironmentalConditions::GetEmissivity()
{
  if (m_Emissivity == nullptr)
    m_Emissivity = new SEScalar0To1();
  return *m_Emissivity;
}
double SEEnvironmentalConditions::GetEmissivity() const
{
  if (m_Emissivity == nullptr)
    return SEScalar::dNaN();
  return m_Emissivity->GetValue();
}

bool SEEnvironmentalConditions::HasMeanRadiantTemperature() const
{
  return m_MeanRadiantTemperature == nullptr ? false : m_MeanRadiantTemperature->IsValid();
}
SEScalarTemperature& SEEnvironmentalConditions::GetMeanRadiantTemperature()
{
  if (m_MeanRadiantTemperature == nullptr)
    m_MeanRadiantTemperature = new SEScalarTemperature();
  return *m_MeanRadiantTemperature;
}
double SEEnvironmentalConditions::GetMeanRadiantTemperature(const TemperatureUnit& unit) const
{
  if (m_MeanRadiantTemperature == nullptr)
    return SEScalar::dNaN();
  return m_MeanRadiantTemperature->GetValue(unit);
}

bool SEEnvironmentalConditions::HasMechanicalDeadSpace() const
{
  return m_MechanicalDeadSpace == nullptr ? false : m_MechanicalDeadSpace->IsValid();
}
SEScalarVolume& SEEnvironmentalConditions::GetMechanicalDeadSpace()
{
  if (m_MechanicalDeadSpace == nullptr)
    m_MechanicalDeadSpace = new SEScalarVolume();
  return *m_MechanicalDeadSpace;
}
double SEEnvironmentalConditions::GetMechanicalDeadSpace(const VolumeUnit& unit) const
{
  if (m_MechanicalDeadSpace == nullptr)
    return SEScalar::dNaN();
  return m_MechanicalDeadSpace->GetValue(unit);
}

bool SEEnvironmentalConditions::HasRelativeHumidity() const
{
  return m_RelativeHumidity == nullptr ? false : m_RelativeHumidity->IsValid();
}
SEScalar0To1& SEEnvironmentalConditions::GetRelativeHumidity()
{
  if (m_RelativeHumidity == nullptr)
    m_RelativeHumidity = new SEScalar0To1();
  return *m_RelativeHumidity;
}
double SEEnvironmentalConditions::GetRelativeHumidity() const
{
  if (m_RelativeHumidity == nullptr)
    return SEScalar::dNaN();
  return m_RelativeHumidity->GetValue();
}

bool SEEnvironmentalConditions::HasRespirationAmbientTemperature() const
{
  return m_RespirationAmbientTemperature == nullptr ? false : m_RespirationAmbientTemperature->IsValid();
}
SEScalarTemperature& SEEnvironmentalConditions::GetRespirationAmbientTemperature()
{
  if (m_RespirationAmbientTemperature == nullptr)
    m_RespirationAmbientTemperature = new SEScalarTemperature();
  return *m_RespirationAmbientTemperature;
}
double SEEnvironmentalConditions::GetRespirationAmbientTemperature(const TemperatureUnit& unit) const
{
  if (m_RespirationAmbientTemperature == nullptr)
    return SEScalar::dNaN();
  return m_RespirationAmbientTemperature->GetValue(unit);
}

bool SEEnvironmentalConditions::HasAmbientGas() const
{
  if (m_AmbientGases.empty())
    return false;
  for (auto g : m_AmbientGases)
    if (g->HasFractionAmount() && g->GetFractionAmount().IsPositive())
      return true;
  return false;
}
bool SEEnvironmentalConditions::HasAmbientGas(const SESubstance& s) const
{
  for (const SESubstanceFraction* sf : m_AmbientGases)
  {
    if (&s == &sf->GetSubstance())
      return sf->HasFractionAmount() && sf->GetFractionAmount() > 0;
  }
  return false;
}
const std::vector<SESubstanceFraction*>& SEEnvironmentalConditions::GetAmbientGases()
{
  return m_AmbientGases;
}
const std::vector<const SESubstanceFraction*>& SEEnvironmentalConditions::GetAmbientGases() const
{
  return m_cAmbientGases;
}
SESubstanceFraction& SEEnvironmentalConditions::GetAmbientGas(const SESubstance& s)
{
  for (SESubstanceFraction* sf : m_AmbientGases)
  {
    if (&s == &sf->GetSubstance())
      return *sf;
  }
  SESubstanceFraction* sf = new SESubstanceFraction(s);
  sf->GetFractionAmount().SetValue(0);
  m_AmbientGases.push_back(sf);
  m_cAmbientGases.push_back(sf);
  return *sf;
}
const SESubstanceFraction* SEEnvironmentalConditions::GetAmbientGas(const SESubstance& s) const
{
  const SESubstanceFraction* sf = nullptr;
  for (unsigned int i = 0; i<m_AmbientGases.size(); i++)
  {
    sf = m_AmbientGases[i];
    if (&s == &sf->GetSubstance())
      return sf;
  }
  return sf;
}
void SEEnvironmentalConditions::RemoveAmbientGas(const SESubstance& s)
{
  SESubstanceFraction& sf = GetAmbientGas(s);
  sf.GetFractionAmount().SetValue(0);
}
void SEEnvironmentalConditions::RemoveAmbientGases()
{
  for (SESubstanceFraction* sf : m_AmbientGases)
    sf->GetFractionAmount().SetValue(0);
}

bool SEEnvironmentalConditions::HasAmbientAerosol() const
{
  if (m_AmbientAerosols.empty())
    return false;
  for (auto a : m_AmbientAerosols)
    if (a->HasConcentration() && a->GetConcentration().IsPositive())
      return true;
  return false;
}
bool SEEnvironmentalConditions::HasAmbientAerosol(const SESubstance& substance) const
{
  for (SESubstanceConcentration* sc : m_AmbientAerosols)
  {
    if (&substance == &sc->GetSubstance())
      return sc->HasConcentration() && sc->GetConcentration().IsPositive();
  }
  return false;
}
const std::vector<SESubstanceConcentration*>& SEEnvironmentalConditions::GetAmbientAerosols()
{
  return m_AmbientAerosols;
}
const std::vector<const SESubstanceConcentration*>& SEEnvironmentalConditions::GetAmbientAerosols() const
{
  return m_cAmbientAerosols;
}
SESubstanceConcentration& SEEnvironmentalConditions::GetAmbientAerosol(const SESubstance& substance)
{
  for (SESubstanceConcentration* sc : m_AmbientAerosols)
  {
    if (&substance == &sc->GetSubstance())
      return *sc;
  }
  SESubstanceConcentration* sc = new SESubstanceConcentration(substance);
  sc->GetConcentration().SetValue(0,MassPerVolumeUnit::ug_Per_L);
  m_AmbientAerosols.push_back(sc);
  m_cAmbientAerosols.push_back(sc);
  return *sc;
}
const SESubstanceConcentration* SEEnvironmentalConditions::GetAmbientAerosol(const SESubstance& substance) const
{
  const SESubstanceConcentration* sc = nullptr;
  for (unsigned int i = 0; i<m_AmbientAerosols.size(); i++)
  {
    sc = m_AmbientAerosols[i];
    if (&substance == &sc->GetSubstance())
      return sc;
  }
  return sc;
}
void SEEnvironmentalConditions::RemoveAmbientAerosol(const SESubstance& substance)
{
  SESubstanceConcentration& sc = GetAmbientAerosol(substance);
  auto& unit = *sc.GetConcentration().GetUnit();
  sc.GetConcentration().SetValue(0, unit);
}
void SEEnvironmentalConditions::RemoveAmbientAerosols()
{
  for (SESubstanceConcentration* sc : m_AmbientAerosols)
  {
    auto& unit = *sc->GetConcentration().GetUnit();
    sc->GetConcentration().SetValue(0, unit);
  }
}
