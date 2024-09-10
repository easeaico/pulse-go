/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/system/equipment/bag_valve_mask/SEBagValveMask.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/substance/SESubstanceConcentration.h"
#include "cdm/substance/SESubstanceFraction.h"

// State Actions
#include "cdm/system/equipment/bag_valve_mask/actions/SEBagValveMaskConfiguration.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarPressureTimePerVolume.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/io/protobuf/PBBagValveMask.h"

SEBagValveMask::SEBagValveMask(Logger* logger) : SEEquipment(logger)
{
  m_BreathState = eBreathState::NoBreath;
  m_Connection = eSwitch::Off;

  m_BagResistance = nullptr;
  m_FilterResistance = nullptr;
  m_SealResistance = nullptr;
  m_FilterVolume = nullptr;
  m_ConnectionVolume = nullptr;
  m_ValveVolume = nullptr;
  m_ValvePositiveEndExpiratoryPressure = nullptr;
  m_ValveResistance = nullptr;
}

SEBagValveMask::~SEBagValveMask()
{
  m_BreathState = eBreathState::NoBreath;
  m_Connection = eSwitch::Off;

  SAFE_DELETE(m_BagResistance);
  SAFE_DELETE(m_FilterResistance);
  SAFE_DELETE(m_SealResistance);
  SAFE_DELETE(m_FilterVolume);
  SAFE_DELETE(m_ConnectionVolume);
  SAFE_DELETE(m_ValveVolume);
  SAFE_DELETE(m_ValvePositiveEndExpiratoryPressure);
  SAFE_DELETE(m_ValveResistance);

  DELETE_VECTOR(m_FractionInspiredGases);
  m_cFractionInspiredGases.clear();
  m_FractionInspiredGases.clear();

  DELETE_VECTOR(m_ConcentrationInspiredAerosols);
  m_cConcentrationInspiredAerosols.clear();
  m_ConcentrationInspiredAerosols.clear();
}

void SEBagValveMask::Clear()
{
  SEEquipment::Clear();

  m_BreathState = eBreathState::NoBreath;
  m_Connection = eSwitch::Off;

  INVALIDATE_PROPERTY(m_ValvePositiveEndExpiratoryPressure);

  INVALIDATE_PROPERTY(m_BagResistance);
  INVALIDATE_PROPERTY(m_FilterResistance);
  INVALIDATE_PROPERTY(m_SealResistance);
  INVALIDATE_PROPERTY(m_FilterVolume);
  INVALIDATE_PROPERTY(m_ConnectionVolume);
  INVALIDATE_PROPERTY(m_ValveVolume);
  INVALIDATE_PROPERTY(m_ValveResistance);

  RemoveFractionInspiredGases();
  RemoveConcentrationInspiredAerosols();
}


void SEBagValveMask::ProcessConfiguration(SEBagValveMaskConfiguration& config, SESubstanceManager& subMgr)
{
  if (config.GetMergeType() == eMergeType::Replace)
    Clear();
  if (config.HasConfiguration())
    Merge(config.GetConfiguration(), subMgr);
  else if (config.HasConfigurationFile())
  {
    // Update the action with the file contents
    std::string cfg_file = config.GetConfigurationFile();
    if (!config.GetConfiguration().SerializeFromFile(cfg_file, subMgr))
      Error("Unable to load configuration file");
    Merge(config.GetConfiguration(), subMgr);
  }
}
void SEBagValveMask::Merge(const SEBagValveMask& from, SESubstanceManager& subMgr)
{
  SetBreathState(from.m_BreathState.GetEnum());
  if(from.m_Connection.GetEnum()!=eSwitch::NullSwitch)
    SetConnection(from.m_Connection.GetEnum());
  COPY_PROPERTY(BagResistance);
  COPY_PROPERTY(FilterResistance);
  COPY_PROPERTY(SealResistance);
  COPY_PROPERTY(FilterVolume);
  COPY_PROPERTY(ConnectionVolume);
  COPY_PROPERTY(ValveVolume);
  COPY_PROPERTY(ValveResistance);

  COPY_PROPERTY(ValvePositiveEndExpiratoryPressure);

  // Always need to provide a full (fractions sum to 1) substance list that replaces current
  if (from.HasFractionInspiredGas())
  {
    size_t cnt = 0;
    double amt;
    double total = 0;
    const SESubstance* sub;
    // Since we are allowing only O2 to be specified
    // Remove everything so we know what is intentionally not provided
    // And what is intentially set to 0 (don't just set to 0)
    RemoveFractionInspiredGases();
    for (SESubstanceFraction* osf : from.m_FractionInspiredGases)
    {
      sub = subMgr.GetSubstance(osf->GetSubstance().GetName());
      if (sub == nullptr)
      {
        Error("Do not have substance : " + osf->GetSubstance().GetName());
        continue;
      }
      SESubstanceFraction& sf = GetFractionInspiredGas(*sub);
      amt = osf->GetFractionAmount().GetValue();
      sf.GetFractionAmount().SetValue(amt);
      subMgr.AddActiveSubstance(*sub);
      total += amt;
      if (amt > 0)
        cnt++; // Count of non-zero fractions
    }

    // It's Ok if you ONLY set Oxygen, i.e. FiO2
    // Ventilator models should understand that common setting
    if (!SEScalar::IsValue(1, total))
    {
      bool err = false;
      if (cnt != 1)
        err = true;
      else
      {
        const SESubstance* o2 = subMgr.GetSubstance("Oxygen");
        if (!GetFractionInspiredGas(*o2).GetFractionAmount().IsPositive())
          err = true;
      }
      if(err)
        Error("Bag valve mask substance fractions do not sum to 1");
    }
  }

  if (from.HasConcentrationInspiredAerosol())
  {
    for (SESubstanceConcentration* sc : from.m_ConcentrationInspiredAerosols)
    {
      SESubstanceConcentration& mine = GetConcentrationInspiredAerosol(sc->GetSubstance());
      mine.GetConcentration().Set(sc->GetConcentration());
    }
  }
}

bool SEBagValveMask::SerializeToString(std::string& output, eSerializationFormat m) const
{
  return PBBagValveMask::SerializeToString(*this, output, m);
}
bool SEBagValveMask::SerializeToFile(const std::string& filename) const
{
  return PBBagValveMask::SerializeToFile(*this, filename);
}
bool SEBagValveMask::SerializeFromString(const std::string& src, eSerializationFormat m, const SESubstanceManager& subMgr)
{
  return PBBagValveMask::SerializeFromString(src, *this, m, subMgr);
}
bool SEBagValveMask::SerializeFromFile(const std::string& filename, const SESubstanceManager& subMgr)
{
  return PBBagValveMask::SerializeFromFile(filename, *this, subMgr);
}

const SEScalar* SEBagValveMask::GetScalar(const std::string& name)
{
  if (name.compare("BreathState") == 0)
    return &m_BreathState;
  if (name.compare("Connection") == 0)
    return &m_Connection;

  if (name == "BagResistance")
    return &GetBagResistance();
  if (name == "FilterResistance")
    return &GetFilterResistance();
  if (name == "SealResistance")
    return &GetSealResistance();
  if (name == "FilterVolume")
    return &GetFilterVolume();
  if (name == "ConnectionVolume")
    return &GetConnectionVolume();
  if (name == "ValveVolume")
    return &GetValveVolume();
  if (name == "ValvePositiveEndExpiratoryPressure")
    return &GetValvePositiveEndExpiratoryPressure();
  if (name == "ValveResistance")
    return &GetValveResistance();

  // I did not support for getting a specific gas/aerosol scalars due to lack of coffee
  return nullptr;
}

void SEBagValveMask::SetBreathState(eBreathState b)
{
  m_BreathState = b;
}
eBreathState SEBagValveMask::GetBreathState() const
{
  return m_BreathState.GetEnum();
}

void SEBagValveMask::SetConnection(eSwitch c)
{
  m_Connection = c;
}
eSwitch SEBagValveMask::GetConnection() const
{
  return m_Connection.GetEnum();
}

bool SEBagValveMask::HasValvePositiveEndExpiratoryPressure() const
{
  return m_ValvePositiveEndExpiratoryPressure == nullptr ? false : m_ValvePositiveEndExpiratoryPressure->IsValid();
}
SEScalarPressure& SEBagValveMask::GetValvePositiveEndExpiratoryPressure()
{
  if (m_ValvePositiveEndExpiratoryPressure == nullptr)
    m_ValvePositiveEndExpiratoryPressure = new SEScalarPressure();
  return *m_ValvePositiveEndExpiratoryPressure;
}
double SEBagValveMask::GetValvePositiveEndExpiratoryPressure(const PressureUnit& unit) const
{
  if (m_ValvePositiveEndExpiratoryPressure == nullptr)
    return SEScalar::dNaN();
  return m_ValvePositiveEndExpiratoryPressure->GetValue(unit);
}

bool SEBagValveMask::HasBagResistance() const
{
  return m_BagResistance == nullptr ? false : m_BagResistance->IsValid();
}
SEScalarPressureTimePerVolume& SEBagValveMask::GetBagResistance()
{
  if (m_BagResistance == nullptr)
    m_BagResistance = new SEScalarPressureTimePerVolume();
  return *m_BagResistance;
}
double SEBagValveMask::GetBagResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_BagResistance == nullptr)
    return SEScalar::dNaN();
  return m_BagResistance->GetValue(unit);
}

bool SEBagValveMask::HasFilterResistance() const
{
  return m_FilterResistance == nullptr ? false : m_FilterResistance->IsValid();
}
SEScalarPressureTimePerVolume& SEBagValveMask::GetFilterResistance()
{
  if (m_FilterResistance == nullptr)
    m_FilterResistance = new SEScalarPressureTimePerVolume();
  return *m_FilterResistance;
}
double SEBagValveMask::GetFilterResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_FilterResistance == nullptr)
    return SEScalar::dNaN();
  return m_FilterResistance->GetValue(unit);
}

bool SEBagValveMask::HasSealResistance() const
{
  return m_SealResistance == nullptr ? false : m_SealResistance->IsValid();
}
SEScalarPressureTimePerVolume& SEBagValveMask::GetSealResistance()
{
  if (m_SealResistance == nullptr)
    m_SealResistance = new SEScalarPressureTimePerVolume();
  return *m_SealResistance;
}
double SEBagValveMask::GetSealResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_SealResistance == nullptr)
    return SEScalar::dNaN();
  return m_SealResistance->GetValue(unit);
}

bool SEBagValveMask::HasFilterVolume() const
{
  return m_FilterVolume == nullptr ? false : m_FilterVolume->IsValid();
}
SEScalarVolume& SEBagValveMask::GetFilterVolume()
{
  if (m_FilterVolume == nullptr)
    m_FilterVolume = new SEScalarVolume();
  return *m_FilterVolume;
}
double SEBagValveMask::GetFilterVolume(const VolumeUnit& unit) const
{
  if (m_FilterVolume == nullptr)
    return SEScalar::dNaN();
  return m_FilterVolume->GetValue(unit);
}

bool SEBagValveMask::HasConnectionVolume() const
{
  return m_ConnectionVolume == nullptr ? false : m_ConnectionVolume->IsValid();
}
SEScalarVolume& SEBagValveMask::GetConnectionVolume()
{
  if (m_ConnectionVolume == nullptr)
    m_ConnectionVolume = new SEScalarVolume();
  return *m_ConnectionVolume;
}
double SEBagValveMask::GetConnectionVolume(const VolumeUnit& unit) const
{
  if (m_ConnectionVolume == nullptr)
    return SEScalar::dNaN();
  return m_ConnectionVolume->GetValue(unit);
}

bool SEBagValveMask::HasValveVolume() const
{
  return m_ValveVolume == nullptr ? false : m_ValveVolume->IsValid();
}
SEScalarVolume& SEBagValveMask::GetValveVolume()
{
  if (m_ValveVolume == nullptr)
    m_ValveVolume = new SEScalarVolume();
  return *m_ValveVolume;
}
double SEBagValveMask::GetValveVolume(const VolumeUnit& unit) const
{
  if (m_ValveVolume == nullptr)
    return SEScalar::dNaN();
  return m_ValveVolume->GetValue(unit);
}

bool SEBagValveMask::HasValveResistance() const
{
  return m_ValveResistance == nullptr ? false : m_ValveResistance->IsValid();
}
SEScalarPressureTimePerVolume& SEBagValveMask::GetValveResistance()
{
  if (m_ValveResistance == nullptr)
    m_ValveResistance = new SEScalarPressureTimePerVolume();
  return *m_ValveResistance;
}
double SEBagValveMask::GetValveResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_ValveResistance == nullptr)
    return SEScalar::dNaN();
  return m_ValveResistance->GetValue(unit);
}

bool SEBagValveMask::HasFractionInspiredGas() const
{
  if (m_FractionInspiredGases.empty())
    return false;
  for (auto a : m_FractionInspiredGases)
    if (a->HasFractionAmount() && a->GetFractionAmount().IsPositive())
      return true;
  return false;
}
bool SEBagValveMask::HasFractionInspiredGas(const SESubstance& s) const
{
  for (const SESubstanceFraction* sf : m_FractionInspiredGases)
  {
    if (&s == &sf->GetSubstance())
      return sf->GetFractionAmount() > 0;
  }
  return false;
}
const std::vector<SESubstanceFraction*>& SEBagValveMask::GetFractionInspiredGases()
{
  return m_FractionInspiredGases;
}
const std::vector<const SESubstanceFraction*>& SEBagValveMask::GetFractionInspiredGases() const
{
  return m_cFractionInspiredGases;
}
SESubstanceFraction& SEBagValveMask::GetFractionInspiredGas(const SESubstance& s)
{
  for (SESubstanceFraction* sf : m_FractionInspiredGases)
  {
    if (&s == &sf->GetSubstance())
      return *sf;
  }
  SESubstanceFraction* sf = new SESubstanceFraction(s);
  sf->GetFractionAmount().SetValue(0);
  m_FractionInspiredGases.push_back(sf);
  m_cFractionInspiredGases.push_back(sf);
  return *sf;
}
const SESubstanceFraction* SEBagValveMask::GetFractionInspiredGas(const SESubstance& s) const
{
  const SESubstanceFraction* sf = nullptr;
  for (unsigned int i = 0; i < m_FractionInspiredGases.size(); i++)
  {
    sf = m_FractionInspiredGases[i];
    if (&s == &sf->GetSubstance())
      return sf;
  }
  return sf;
}
void SEBagValveMask::RemoveFractionInspiredGas(const SESubstance& s)
{
  SESubstanceFraction& sf = GetFractionInspiredGas(s);
  sf.GetFractionAmount().SetValue(0);
}
void SEBagValveMask::RemoveFractionInspiredGases()
{
  for (SESubstanceFraction* sf : m_FractionInspiredGases)
    sf->GetFractionAmount().SetValue(0);
}

bool SEBagValveMask::HasConcentrationInspiredAerosol() const
{
  if (m_ConcentrationInspiredAerosols.empty())
    return false;
  for (auto a : m_ConcentrationInspiredAerosols)
    if (a->HasConcentration() && a->GetConcentration().IsPositive())
      return true;
  return false;
}
bool SEBagValveMask::HasConcentrationInspiredAerosol(const SESubstance& substance) const
{
  for (SESubstanceConcentration* sc : m_ConcentrationInspiredAerosols)
  {
    if (&substance == &sc->GetSubstance())
      return sc->GetConcentration().IsPositive();
  }
  return false;
}
const std::vector<SESubstanceConcentration*>& SEBagValveMask::GetConcentrationInspiredAerosols()
{
  return m_ConcentrationInspiredAerosols;
}
const std::vector<const SESubstanceConcentration*>& SEBagValveMask::GetConcentrationInspiredAerosols() const
{
  return m_cConcentrationInspiredAerosols;
}
SESubstanceConcentration& SEBagValveMask::GetConcentrationInspiredAerosol(const SESubstance& substance)
{
  for (SESubstanceConcentration* sc : m_ConcentrationInspiredAerosols)
  {
    if (&substance == &sc->GetSubstance())
      return *sc;
  }
  SESubstanceConcentration* sc = new SESubstanceConcentration(substance);
  sc->GetConcentration().SetValue(0, MassPerVolumeUnit::ug_Per_L);
  m_ConcentrationInspiredAerosols.push_back(sc);
  m_cConcentrationInspiredAerosols.push_back(sc);
  return *sc;
}
const SESubstanceConcentration* SEBagValveMask::GetConcentrationInspiredAerosol(const SESubstance& substance) const
{
  const SESubstanceConcentration* sc = nullptr;
  for (unsigned int i = 0; i < m_ConcentrationInspiredAerosols.size(); i++)
  {
    sc = m_ConcentrationInspiredAerosols[i];
    if (&substance == &sc->GetSubstance())
      return sc;
  }
  return sc;
}
void SEBagValveMask::RemoveConcentrationInspiredAerosol(const SESubstance& substance)
{
  SESubstanceConcentration& sc = GetConcentrationInspiredAerosol(substance);
  auto& unit = *sc.GetConcentration().GetUnit();
  sc.GetConcentration().SetValue(0, unit);
}
void SEBagValveMask::RemoveConcentrationInspiredAerosols()
{
  for (SESubstanceConcentration* sc : m_ConcentrationInspiredAerosols)
  {
    auto& unit = *sc->GetConcentration().GetUnit();
    sc->GetConcentration().SetValue(0, unit);
  }
}
