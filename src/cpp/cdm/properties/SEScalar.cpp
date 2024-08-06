/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/properties/SEScalar.h"
#include "cdm/utils/GeneralMath.h"

// Declare template classes all Scalar types at the bottom

double SEScalar::dNaN()
{
  return std::nan("");
}

// Opposite of isnan which can be slow
bool SEScalar::IsNumber(double d)
{
  if (d >= -std::numeric_limits<double>::max()&&
      d <=  std::numeric_limits<double>::max())
    return true;
  return false;
}

const NoUnit NoUnit::unitless;

SEScalarPair::SEScalarPair() { value = SEScalar::dNaN(); unit = ""; }
SEScalarPair::SEScalarPair(double v) { value = v; unit = ""; }
SEScalarPair::SEScalarPair(double v, const std::string& u) { value = v; unit = u; }
SEScalarPair::SEScalarPair(double v, const CCompoundUnit& cc) { value = v; unit = cc.GetString(); }

SEScalar::SEScalar() : SEProperty()
{

}

SEScalar::~SEScalar()
{

}

bool SEScalar::Set(const SEScalar& s)
{
  if (dynamic_cast<const SEUnitScalar*>(&s) != nullptr)
    throw CommonDataModelException("SEScalar::Set: Scalar is not of the proper type");
  if(!s.IsValid())
    return false;
  if (m_readOnly)
    throw CommonDataModelException("SEScalar::Set: Scalar is marked read-only");
  m_value = s.m_value;
  m_isnan = false;
  m_isinf = false;
  if (!IsNumber(m_value))
  {
    m_isnan = std::isnan(m_value);
    m_isinf = std::isinf(m_value);
  }
  return true;
}
bool SEScalar::Force(const SEScalar& s)
{
  if (dynamic_cast<const SEUnitScalar*>(&s) != nullptr)
    throw CommonDataModelException("SEScalar::Force: Scalar is not of the proper type");
  if (!s.IsValid())
    return false;
  m_value = s.m_value;
  m_isnan = false;
  m_isinf = false;
  if (!IsNumber(m_value))
  {
    m_isnan = std::isnan(m_value);
    m_isinf = std::isinf(m_value);
  }
  return true;
}

void SEScalar::Copy(const SEScalar& s)
{
  if (m_readOnly)
    throw CommonDataModelException("SEScalar::Copy: Scalar is marked read-only");
  m_value = s.m_value;
  m_isnan = s.m_isnan;
  m_isinf = s.m_isinf;
}

void SEScalar::Invalidate()
{
  if (m_readOnly)
    throw CommonDataModelException("SEScalar::Invalidate: Scalar is marked read-only");
  m_isnan = true;
  m_isinf = false;
  m_value = SEScalar::dNaN();
}

void SEScalar::ForceInvalidate()
{
  m_isnan = true;
  m_isinf = false;
  m_value = SEScalar::dNaN();
}

bool SEScalar::IsValid() const 
{
  if(m_isnan)
    return false;
  return true;
}

bool SEScalar::IsZero(double limit) const
{
  if (!IsValid())
    return false;
  return SEScalar::IsZero(m_value,limit);
}

bool SEScalar::IsPositive() const
{
  if (!IsValid())
    return false;
  return m_value>0;
}

bool SEScalar::IsNegative() const
{
  if (!IsValid())
    return false;
  return m_value<0;
}

void SEScalar::SetReadOnly(bool b)
{
  m_readOnly = b;
}
bool SEScalar::IsReadOnly() const
{
  return m_readOnly;
}

double SEScalar::GetValue() const
{
  if (!m_isnan)
    return m_value;
  throw CommonDataModelException("SEScalar::GetValue is NaN");
}

double SEScalar::GetValue(int precision) const
{
  if (!m_isnan)
    return SEScalar::Truncate(m_value, precision);
  throw CommonDataModelException("SEScalar::GetValue is NaN");
}

void SEScalar::SetValue(double d)
{
  if (m_readOnly)
    throw CommonDataModelException("SEScalar::SetValue: Scalar is marked read-only");
  m_value = d;
  m_isnan = false;
  m_isinf = false;
  if (!IsNumber(m_value))
  {
    m_isnan = std::isnan(m_value);
    m_isinf = std::isinf(m_value);
  }
}
void SEScalar::ForceValue(double d)
{
  m_value = d;
  m_isnan = false;
  m_isinf = false;
  if (!IsNumber(m_value))
  {
    m_isnan = std::isnan(m_value);
    m_isinf = std::isinf(m_value);
  }
}

double SEScalar::Increment(const SEScalar& s)
{
  if (!s.IsValid())
    Invalidate();
  else
    Increment(s.GetValue());
  return m_value;
}

double SEScalar::Increment(double d)
{
  if (!IsValid())
  {
    SetValue(d);
    return d;
  }
  SetValue(m_value+d);
  return m_value;
}

double SEScalar::ForceIncrement(double d)
{
  if (!IsValid())
  {
    ForceValue(d);
    return d;
  }
  ForceValue(m_value + d);
  return m_value;
}

double SEScalar::Multiply(double d)
{
  if (!IsValid())
  {
    SetValue(d);
    return d;
  }
  SetValue(m_value * d);
  return m_value;
}

void SEScalar::Average(int cnt)
{
  if (!IsValid() || cnt == 0)
    return;
  m_value /= cnt;
}

bool SEScalar::Equals(const SEScalar& to) const
{
  if (m_isnan && to.m_isnan)
    return true;
  if (m_isnan || to.m_isnan)
    return false;
  if (m_isinf&&to.m_isinf)
    return true;
  if (m_isinf || to.m_isinf)
    return false;
  return std::abs(GeneralMath::PercentDifference(m_value, to.m_value)) <= 1e-15;
}

std::string SEScalar::ToString() const
{
  return pulse::cdm::to_string(m_value);
}
void SEScalar::ToString(std::ostream &str) const
{
  str<<m_value<<std::flush;
}

bool SEScalar::IsValue(double target, double value)
{
  if (value<(target + 1e-10) && value>(target - 1e-10))
    return true;
  return false;
}

double SEScalar::Truncate(double value, int decimal_places)
{
  switch (decimal_places)
  {
  case 1:
    return std::ceil(value * 10) * 0.1;
  case 2:
    return std::ceil(value * 100) * 0.01;
  case 3:
    return std::ceil(value * 1000) * 0.001;
  case 4:
    return std::ceil(value * 10000) * 0.0001;
  case 5:
    return std::ceil(value * 100000) * 0.00001;
  case 6:
    return std::ceil(value * 1000000) * 0.000001;
  default:
    const double multiplier = std::pow(10.0, decimal_places);
    return std::ceil(value * multiplier) / multiplier;
  }
  return value;
}

bool SEScalar::IsZero(double d, double limit)
{
  if (d<limit&&d>-limit)
    return true;
  return false;
}

//////////////////////
// SEScalarQuantity //
//////////////////////


SEGenericScalar::SEGenericScalar(Logger* logger) : Loggable(logger)
{
  m_Scalar = nullptr;
  m_UnitScalar = nullptr;
}

bool SEGenericScalar::HasScalar() const
{
  return m_Scalar != nullptr;
}
void SEGenericScalar::SetScalar(const SEScalar& s)
{
  m_Scalar = &s;
  m_UnitScalar = dynamic_cast<const SEUnitScalar*>(m_Scalar);
}

bool SEGenericScalar::IsValid() const
{
  return m_UnitScalar == nullptr ? m_Scalar->IsValid() : m_UnitScalar->IsValid();
}

bool SEGenericScalar::IsInfinity() const
{
  return m_Scalar->IsInfinity();
}

bool SEGenericScalar::HasUnit() const
{
  return m_UnitScalar != nullptr && m_UnitScalar->HasUnit();
}
const CCompoundUnit* SEGenericScalar::GetUnit() const
{
  if (m_UnitScalar == nullptr)
    return nullptr;
  return m_UnitScalar->GetUnit();
}
const CCompoundUnit* SEGenericScalar::GetCompoundUnit(const std::string& unit) const
{
  if (m_UnitScalar == nullptr)
    return nullptr;
  return m_UnitScalar->GetCompoundUnit(unit);
}
bool SEGenericScalar::IsValidUnit(const CCompoundUnit& unit) const
{
  if (m_UnitScalar == nullptr)
    return false;
  if (*m_UnitScalar->GetUnit()->GetDimension() == *unit.GetDimension())
    return true;
  // See if the quantity types (Dimensions) are convertable
  double fromExp;
  CCompoundUnit mappingUnit;
  CUnitConversionEngine& uce = CUnitConversionEngine::GetEngine();
  if (uce.GetQuantityConversionParams(m_UnitScalar->GetUnit()->GetDimension(), unit.GetDimension(), fromExp, mappingUnit))
    return true;
  return false;
}

double SEGenericScalar::GetValue() const
{
  return m_Scalar->GetValue();
}
double SEGenericScalar::GetValue(int decimal_places) const
{
  return SEScalar::Truncate(m_Scalar->GetValue(), decimal_places);
}
double SEGenericScalar::GetValue(const CCompoundUnit& unit) const
{
  if (m_UnitScalar == nullptr)
    return SEScalar::dNaN();
  return m_UnitScalar->GetValue(unit);
}
double SEGenericScalar::GetValue(const CCompoundUnit& unit, int decimal_places) const
{
  return SEScalar::Truncate(GetValue(unit), decimal_places);
}

std::string SEGenericScalar::GetString() const
{
  if (!IsValid())
    return "NaN";
  std::string str = pulse::cdm::to_string(GetValue());
  const CCompoundUnit* unit = GetUnit();
  if (unit != nullptr)
    str += " " + unit->GetString();
  return str;
}
