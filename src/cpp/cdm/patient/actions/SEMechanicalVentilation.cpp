/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/patient/actions/SEMechanicalVentilation.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceConcentration.h"
#include "cdm/substance/SESubstanceFraction.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/io/protobuf/PBPatientActions.h"

SEMechanicalVentilation::SEMechanicalVentilation(Logger* logger) : SEPatientAction(logger)
{
  m_State = eSwitch::Off;
  m_Flow = nullptr;
  m_Pressure = nullptr;
  m_MechanicalDeadSpaceVolume = nullptr;
}

SEMechanicalVentilation::~SEMechanicalVentilation()
{

  m_State = eSwitch::Off;
  SAFE_DELETE(m_Flow);
  SAFE_DELETE(m_Pressure);
  SAFE_DELETE(m_MechanicalDeadSpaceVolume);

  DELETE_VECTOR(m_GasFractions);
  m_cGasFractions.clear();
  DELETE_VECTOR(m_Aerosols);
  m_cAerosols.clear();
}

void SEMechanicalVentilation::Clear()
{
  SEPatientAction::Clear();

  m_State = eSwitch::Off;
  INVALIDATE_PROPERTY(m_Flow);
  INVALIDATE_PROPERTY(m_Pressure);
  INVALIDATE_PROPERTY(m_MechanicalDeadSpaceVolume);

  for (SESubstanceFraction* sf : m_GasFractions)
    sf->Clear();
  for (SESubstanceConcentration* sc : m_Aerosols)
    sc->Clear();
}

void SEMechanicalVentilation::Copy(const SEMechanicalVentilation& src, const SESubstanceManager& subMgr, bool /*preserveState*/)
{
  //if(preserveState) // Cache any state before copy,
  PBPatientAction::Copy(src, *this, subMgr);
  //if(preserveState) // Put back any state
}

bool SEMechanicalVentilation::IsValid() const
{
  if (GetState() == eSwitch::Off)
    return true;
  else
  {
    if (HasGasFraction())
    {
      double total = 0;
      for (const SESubstanceFraction* sf : m_cGasFractions)
      {
        if (sf->GetSubstance().GetState() != eSubstance_State::Gas)
        {
          Error("Mechanical Ventilation Gas fractions substances must be a gas");
          return false;
        }

        total += sf->GetFractionAmount();
      }
      if (!SEScalar::IsValue(1, total))
      {
        Error("Mechanical Ventilation Gas fractions do not sum to 1");
        return false;
      }
    }
    if (HasAerosol())
    {
      for (const SESubstanceConcentration* sc : m_cAerosols)
      {
        if (sc->GetSubstance().GetState() != eSubstance_State::Solid &&
            sc->GetSubstance().GetState() != eSubstance_State::Liquid)
        {
          Error("Mechanical Ventilation aerosol substances must be a solid or a liquid");
          return false;
        }
      }
    }
  }
  if (!HasPressure() && !HasFlow())
  {
    Error("Mechanical Ventilation must have pressure and/or flow");
    return false;
  }
  return true;
}

bool SEMechanicalVentilation::IsActive() const
{
  if (!SEPatientAction::IsActive())
    return false;
  return GetState() == eSwitch::On;
}
void SEMechanicalVentilation::Deactivate()
{
  SEPatientAction::Deactivate();
  Clear();//No stateful properties
}

const SEScalar* SEMechanicalVentilation::GetScalar(const std::string& name)
{
  if (name.compare("Flow") == 0)
    return &GetFlow();
  if (name.compare("Pressure") == 0)
    return &GetPressure();
  return nullptr;
}

eSwitch SEMechanicalVentilation::GetState() const
{
  return m_State;
}
void SEMechanicalVentilation::SetState(eSwitch state)
{
  m_State = (state == eSwitch::NullSwitch) ? eSwitch::Off : state;
}

bool SEMechanicalVentilation::HasFlow() const
{
  return m_Flow == nullptr ? false : m_Flow->IsValid();
}
SEScalarVolumePerTime& SEMechanicalVentilation::GetFlow()
{
  if (m_Flow == nullptr)
    m_Flow = new SEScalarVolumePerTime();
  return *m_Flow;
}
double SEMechanicalVentilation::GetFlow(const VolumePerTimeUnit& unit) const
{
  if (m_Flow == nullptr)
    return SEScalar::dNaN();
  return m_Flow->GetValue(unit);
}

bool SEMechanicalVentilation::HasPressure() const
{
  return m_Pressure == nullptr ? false : m_Pressure->IsValid();
}
SEScalarPressure& SEMechanicalVentilation::GetPressure()
{
  if (m_Pressure == nullptr)
    m_Pressure = new SEScalarPressure();
  return *m_Pressure;
}
double SEMechanicalVentilation::GetPressure(const PressureUnit& unit) const
{
  if (m_Pressure == nullptr)
    return SEScalar::dNaN();
  return m_Pressure->GetValue(unit);
}

bool SEMechanicalVentilation::HasMechanicalDeadSpaceVolume() const
{
  return m_MechanicalDeadSpaceVolume == nullptr ? false : m_MechanicalDeadSpaceVolume->IsValid();
}
SEScalarVolume& SEMechanicalVentilation::GetMechanicalDeadSpaceVolume()
{
  if (m_MechanicalDeadSpaceVolume == nullptr)
    m_MechanicalDeadSpaceVolume = new SEScalarVolume();
  return *m_MechanicalDeadSpaceVolume;
}
double SEMechanicalVentilation::GetMechanicalDeadSpaceVolume(const VolumeUnit& unit) const
{
  if (m_MechanicalDeadSpaceVolume == nullptr)
    return SEScalar::dNaN();
  return m_MechanicalDeadSpaceVolume->GetValue(unit);
}

bool SEMechanicalVentilation::HasGasFraction() const
{
  return m_GasFractions.size() == 0 ? false : true;
}
bool SEMechanicalVentilation::HasGasFraction(const SESubstance& s) const
{
  for (const SESubstanceFraction* sf : m_GasFractions)
  {
    if (&s == &sf->GetSubstance())
      return true;
  }
  return false;
}
const std::vector<SESubstanceFraction*>& SEMechanicalVentilation::GetGasFractions()
{
  return m_GasFractions;
}
const std::vector<const SESubstanceFraction*>& SEMechanicalVentilation::GetGasFractions() const
{
  return m_cGasFractions;
}
SESubstanceFraction& SEMechanicalVentilation::GetGasFraction(const SESubstance& s)
{
  for (SESubstanceFraction* sf : m_GasFractions)
  {
    if (&s == &sf->GetSubstance())
      return *sf;
  }
  SESubstanceFraction* sf = new SESubstanceFraction(s);
  sf->GetFractionAmount().SetValue(0);
  m_GasFractions.push_back(sf);
  m_cGasFractions.push_back(sf);
  return *sf;
}
const SESubstanceFraction* SEMechanicalVentilation::GetGasFraction(const SESubstance& s) const
{
  const SESubstanceFraction* sf = nullptr;
  for (unsigned int i = 0; i < m_GasFractions.size(); i++)
  {
    sf = m_GasFractions[i];
    if (&s == &sf->GetSubstance())
      return sf;
  }
  return sf;
}
void SEMechanicalVentilation::RemoveGasFraction(const SESubstance& s)
{
  const SESubstanceFraction* sf;
  for (unsigned int i = 0; i < m_GasFractions.size(); i++)
  {
    sf = m_GasFractions[i];
    if (&s == &sf->GetSubstance())
    {
      m_GasFractions.erase(m_GasFractions.begin() + i);
      m_cGasFractions.erase(m_cGasFractions.begin() + i);
      delete sf;
    }
  }
}
void SEMechanicalVentilation::RemoveGasFractions()
{
  DELETE_VECTOR(m_GasFractions);
  m_cGasFractions.clear();
}

bool SEMechanicalVentilation::HasAerosol() const
{
  return m_Aerosols.size() == 0 ? false : true;
}
bool SEMechanicalVentilation::HasAerosol(const SESubstance& substance) const
{
  for (const SESubstanceConcentration* sc : m_Aerosols)
  {
    if (&substance == &sc->GetSubstance())
      return true;
  }
  return false;
}
const std::vector<SESubstanceConcentration*>& SEMechanicalVentilation::GetAerosols()
{
  return m_Aerosols;
}
const std::vector<const SESubstanceConcentration*>& SEMechanicalVentilation::GetAerosols() const
{
  return m_cAerosols;
}
SESubstanceConcentration& SEMechanicalVentilation::GetAerosol(const SESubstance& substance)
{
  for (SESubstanceConcentration* sc : m_Aerosols)
  {
    if (&substance == &sc->GetSubstance())
      return *sc;
  }
  SESubstanceConcentration* sc = new SESubstanceConcentration(substance);
  sc->GetConcentration().SetValue(0, MassPerVolumeUnit::ug_Per_L);
  m_Aerosols.push_back(sc);
  m_cAerosols.push_back(sc);
  return *sc;
}
const SESubstanceConcentration* SEMechanicalVentilation::GetAerosol(const SESubstance& substance) const
{
  const SESubstanceConcentration* sc = nullptr;
  for (unsigned int i = 0; i<m_Aerosols.size(); i++)
  {
    sc = m_Aerosols[i];
    if (&substance == &sc->GetSubstance())
      return sc;
  }
  return sc;
}
void SEMechanicalVentilation::RemoveAerosol(const SESubstance& substance)
{
  const SESubstanceConcentration* sc;
  for (unsigned int i = 0; i<m_Aerosols.size(); i++)
  {
    sc = m_Aerosols[i];
    if (&substance == &sc->GetSubstance())
    {
      m_Aerosols.erase(m_Aerosols.begin() + i);
      m_cAerosols.erase(m_cAerosols.begin() + i);
      delete sc;
    }
  }
}
void SEMechanicalVentilation::RemoveAerosols()
{
  DELETE_VECTOR(m_Aerosols);
  m_cAerosols.clear();
}
