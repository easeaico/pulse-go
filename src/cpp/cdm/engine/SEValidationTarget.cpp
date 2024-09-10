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

  m_Target = SEScalar::dNaN();
  m_TargetMaximum = SEScalar::dNaN();
  m_TargetMinimum = SEScalar::dNaN();
}
bool SEValidationTarget::IsValid()
{
  if (std::isnan(m_Target) && std::isnan(m_TargetMaximum))
    return false;
  return true;
}

SESegmentValidationTarget::SESegmentValidationTarget() : SEValidationTarget()
{
  Clear();
}
void SESegmentValidationTarget::Clear()
{
  SEValidationTarget::Clear();
  m_ComparisonType = eComparisonType::None;
  m_TargetSegment = 0;
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
void SESegmentValidationTarget::SetEqualToSegment(int s)
{
  m_ComparisonType = eComparisonType::EqualToSegment;
  m_Target = SEScalar::dNaN();
  m_TargetMaximum = SEScalar::dNaN();
  m_TargetMinimum = SEScalar::dNaN();
  m_TargetSegment = s;
}
void SESegmentValidationTarget::SetEqualToValue(double d)
{
  m_ComparisonType = eComparisonType::EqualToValue;
  m_Target = d;
  m_TargetMaximum = d;
  m_TargetMinimum = d;
  m_TargetSegment = 0;
}
void SESegmentValidationTarget::SetGreaterThanSegment(int s)
{
  m_ComparisonType = eComparisonType::GreaterThanSegment;
  m_Target = SEScalar::dNaN();
  m_TargetMaximum = SEScalar::dNaN();
  m_TargetMinimum = SEScalar::dNaN();
  m_TargetSegment = s;
}
void SESegmentValidationTarget::SetGreaterThanValue(double d)
{
  m_ComparisonType = eComparisonType::GreaterThanValue;
  m_Target = d;
  m_TargetMaximum = d;
  m_TargetMinimum = d;
  m_TargetSegment = 0;
}
void SESegmentValidationTarget::SetLessThanSegment(int s)
{
  m_ComparisonType = eComparisonType::LessThanSegment;
  m_Target = SEScalar::dNaN();
  m_TargetMaximum = SEScalar::dNaN();
  m_TargetMinimum = SEScalar::dNaN();
  m_TargetSegment = s;
}
void SESegmentValidationTarget::SetLessThanValue(double d)
{
  m_ComparisonType = eComparisonType::LessThanValue;
  m_Target = d;
  m_TargetMaximum = d;
  m_TargetMinimum = d;
  m_TargetSegment = 0;
}
void SESegmentValidationTarget::SetTrendsToSegment(int s)
{
  m_ComparisonType = eComparisonType::TrendsToSegment;
  m_Target = SEScalar::dNaN();
  m_TargetMaximum = SEScalar::dNaN();
  m_TargetMinimum = SEScalar::dNaN();
  m_TargetSegment = s;
}
void SESegmentValidationTarget::SetTrendsToValue(double d)
{
  m_ComparisonType = eComparisonType::TrendsToValue;
  m_Target = d;
  m_TargetMaximum = d;
  m_TargetMinimum = d;
  m_TargetSegment = 0;
}
void SESegmentValidationTarget::SetRange(double min, double max)
{
  m_ComparisonType = eComparisonType::Range;
  m_Target = SEScalar::dNaN();
  m_TargetMaximum = max;
  m_TargetMinimum = min;
  m_TargetSegment = 0;
}

SETimeSeriesValidationTarget::SETimeSeriesValidationTarget() : SEValidationTarget()
{
  m_TargetType = eTargetType::Mean;
  m_Error = 100;
  m_ComparisonValue = 0;
}
void SETimeSeriesValidationTarget::Clear()
{
  SEValidationTarget::Clear();
  m_ComparisonType = eComparisonType::None;
  m_TargetType = eTargetType::Mean;
  m_Target = SEScalar::dNaN();
  m_TargetMaximum = SEScalar::dNaN();
  m_TargetMinimum = SEScalar::dNaN();

  m_Error = SEScalar::dNaN();
  m_Data.clear();
  m_ComparisonValue = SEScalar::dNaN();
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
void SETimeSeriesValidationTarget::SetEqualTo(double d, eTargetType t)
{
  m_ComparisonType = eComparisonType::EqualToValue;
  m_TargetType = t;
  m_Target = d;
  m_TargetMaximum = d;
  m_TargetMinimum = d;
}
void SETimeSeriesValidationTarget::SetRange(double min, double max, eTargetType t)
{
  m_ComparisonType = eComparisonType::Range;
  m_TargetType = t;
  m_Target = SEScalar::dNaN();
  m_TargetMaximum = max;
  m_TargetMinimum = min;
}
bool SETimeSeriesValidationTarget::ComputeError()
{
  switch (m_TargetType)
  {
  case eTargetType::Minimum:
  {
    m_ComparisonValue = *std::min_element(m_Data.begin(), m_Data.end());
    break;
  }
  case eTargetType::Maximum:
  {
    m_ComparisonValue = *std::max_element(m_Data.begin(), m_Data.end());
    break;
  }
  case eTargetType::Mean:
  {
    m_ComparisonValue = (double)(std::accumulate(m_Data.begin(), m_Data.end(),0) / m_Data.size());
    break;
  }
  default:
  {
    return false;
  }
  }
  // NOTE: We could use PercentTolerance too
  double minError = GeneralMath::PercentTolerance(m_TargetMinimum, m_ComparisonValue, 1e-9);
  double maxError = GeneralMath::PercentTolerance(m_TargetMaximum, m_ComparisonValue, 1e-9);
  // No error if we are in range
  if (m_ComparisonValue >= m_TargetMinimum && m_ComparisonValue <= m_TargetMaximum)
  {
    m_Error = 0;
    return true;
  }
  else if (m_ComparisonValue > m_TargetMaximum)
    m_Error = maxError;
  else if (m_ComparisonValue < m_TargetMinimum)
    m_Error = minError;

  if (std::abs(m_Error) < 1e-15)
    m_Error = 0; // Close enough
  return true;
}
