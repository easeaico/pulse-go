/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include <numeric>

#include "SEValidationTarget.h"
#include "cdm/io/protobuf/PBValidation.h"
#include "cdm/utils/GeneralMath.h"

void SEValidationTarget::Clear()
{
  m_Header = "";
  m_Reference = "";
  m_Notes = "";
  m_TableFormatting = "";

  m_ComputedEnum = "";
  m_ComputedValue = SEScalar::dNaN();

  m_Error = SEScalar::dNaN();
  m_GoodPercentError = SEScalar::dNaN();
  m_FairPercentError = SEScalar::dNaN();
}
bool SEValidationTarget::IsValid()
{
  return true;
}

SESegmentValidationTarget::SESegmentValidationTarget() : SEValidationTarget()
{
  Clear();
}
void SESegmentValidationTarget::Clear()
{
  SEValidationTarget::Clear();
  m_ComparisonFormula = "";
  m_GoodPercentError = 0;
  m_FairPercentError = 0;
}
bool SESegmentValidationTarget::SerializeToString(const std::vector<const SESegmentValidationTarget*>& src, std::string& output, eSerializationFormat m, Logger* logger)
{
  return PBValidation::SerializeToString(src, output, m, logger);
}
bool SESegmentValidationTarget::SerializeToFile(const std::vector<const SESegmentValidationTarget*>& src, const std::string& filename, Logger* logger)
{
  return PBValidation::SerializeToFile(src, filename, logger);
}
bool SESegmentValidationTarget::SerializeFromString(const std::string& src, std::vector<SESegmentValidationTarget*>& dst, eSerializationFormat m, Logger* logger)
{
  return PBValidation::SerializeFromString(src, dst, m, logger);
}
bool SESegmentValidationTarget::SerializeFromFile(const std::string& filename, std::vector<SESegmentValidationTarget*>& dst, Logger* logger)
{
  return PBValidation::SerializeFromFile(filename, dst, logger);
}

SETimeSeriesValidationTarget::SETimeSeriesValidationTarget() : SEValidationTarget()
{
  Clear();
}
void SETimeSeriesValidationTarget::Clear()
{
  SEValidationTarget::Clear();
  m_ComparisonType = eComparisonType::None;
  m_TargetType = eTargetType::Mean;

  m_Assessment = "";
  m_PatientSpecific = false;

  m_TargetEnum = "";
  m_TargetValue = SEScalar::dNaN();
  m_TargetMaximum = SEScalar::dNaN();
  m_TargetMinimum = SEScalar::dNaN();

  m_Data.clear();
}
bool SETimeSeriesValidationTarget::SerializeToString(const std::vector<const SETimeSeriesValidationTarget*>& src, std::string& output, eSerializationFormat m, Logger* logger)
{
  return PBValidation::SerializeToString(src, output, m, logger);
}
bool SETimeSeriesValidationTarget::SerializeToFile(const std::vector<const SETimeSeriesValidationTarget*>& src, const std::string& filename, Logger* logger)
{
  return PBValidation::SerializeToFile(src, filename, logger);
}
bool SETimeSeriesValidationTarget::SerializeFromString(const std::string& src, std::vector<SETimeSeriesValidationTarget*>& dst, eSerializationFormat m, Logger* logger)
{
  return PBValidation::SerializeFromString(src, dst, m, logger);
}
bool SETimeSeriesValidationTarget::SerializeFromFile(const std::string& filename, std::vector<SETimeSeriesValidationTarget*>& dst, Logger* logger)
{
  return PBValidation::SerializeFromFile(filename, dst, logger);
}
void SETimeSeriesValidationTarget::SetTargetEnum(const std::string& s)
{
  m_ComparisonType = eComparisonType::TargetValue;
  m_TargetType = eTargetType::Enumeration;
  m_TargetEnum = s;
  m_TargetValue = SEScalar::dNaN();
  m_TargetMaximum = SEScalar::dNaN();
  m_TargetMinimum = SEScalar::dNaN();
}
void SETimeSeriesValidationTarget::SetTargetValue(double d, eTargetType t)
{
  m_ComparisonType = eComparisonType::TargetValue;
  m_TargetType = t;
  m_TargetEnum = "";
  m_TargetValue = d;
  m_TargetMaximum = d;
  m_TargetMinimum = d;
}
void SETimeSeriesValidationTarget::SetTargetRange(double min, double max, eTargetType t)
{
  m_ComparisonType = eComparisonType::TargetRange;
  m_TargetType = t;
  m_TargetEnum = "";
  m_TargetValue = SEScalar::dNaN();
  m_TargetMaximum = max;
  m_TargetMinimum = min;
}
bool SETimeSeriesValidationTarget::ComputeError()
{
  switch (m_TargetType)
  {
  case eTargetType::Minimum:
  {
    m_ComputedValue = *std::min_element(m_Data.begin(), m_Data.end());
    break;
  }
  case eTargetType::Maximum:
  {
    m_ComputedValue = *std::max_element(m_Data.begin(), m_Data.end());
    break;
  }
  case eTargetType::Mean:
  {
    m_ComputedValue = (double)(std::accumulate(m_Data.begin(), m_Data.end(),0) / m_Data.size());
    break;
  }
  default:
  {
    return false;
  }
  }
  // NOTE: We could use PercentTolerance too
  double minError = GeneralMath::PercentTolerance(m_TargetMinimum, m_ComputedValue, 1e-9);
  double maxError = GeneralMath::PercentTolerance(m_TargetMaximum, m_ComputedValue, 1e-9);
  // No error if we are in range
  if (m_ComputedValue >= m_TargetMinimum && m_ComputedValue <= m_TargetMaximum)
  {
    m_Error = 0;
    return true;
  }
  else if (m_ComputedValue > m_TargetMaximum)
    m_Error = maxError;
  else if (m_ComputedValue < m_TargetMinimum)
    m_Error = minError;

  if (std::abs(m_Error) < 1e-15)
    m_Error = 0; // Close enough
  return true;
}
