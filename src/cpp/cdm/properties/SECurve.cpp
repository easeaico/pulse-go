/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/properties/SECurve.h"
#include "cdm/io/protobuf/PBProperties.h"
#include "cdm/properties/SEScalarVolume.h"

SECurve::SECurve(Logger* logger) : Loggable(logger)
{
  
}

SECurve::~SECurve()
{
  Invalidate();
}

void SECurve::Invalidate()
{
  DELETE_VECTOR(m_Segments);
}

bool SECurve::IsValid() const
{
  if (!HasSegment())
    return false;
  for (SESegment* s : m_Segments)
    if (!s->IsValid())
      return false;

  // Check for overlapping segments
  for (size_t i = 1; i < m_Segments.size(); ++i)
  {
    if (m_Segments[i - 1]->GetEndVolume(VolumeUnit::L) > m_Segments[i]->GetBeginVolume(VolumeUnit::L))
    {
      Error("There are overlapping volume ranges in SECurve.");
      return false;
    }
  }

  ///\TODO Make sure no segment gaps and anything else
  return true;
}

void SECurve::SortSegments()
{
  // Check if segments are sorted by BeginVolume
  bool sorted = true;
  for (size_t i = 1; i < m_Segments.size(); ++i)
  {
    if (m_Segments[i - 1]->GetBeginVolume(VolumeUnit::L) > m_Segments[i]->GetBeginVolume(VolumeUnit::L))
    {
      sorted = false;
      break;
    }
  }

  if (!sorted)
  {
    Warning("Respiratory compliance segments are not sorted. Sorting them now.");
    std::sort(m_Segments.begin(), m_Segments.end(), [](SESegment* a, SESegment* b) {
      return a->GetBeginVolume(VolumeUnit::L) < b->GetBeginVolume(VolumeUnit::L);
      });
  }
}

void SECurve::Copy(const SECurve& src)
{
  PBProperty::Copy(src, *this);
  SortSegments();
}

bool SECurve::HasSegment() const
{
  return !m_Segments.empty();
}
SESegmentConstant& SECurve::AddConstantSegment()
{
  SESegmentConstant* s = new SESegmentConstant();
  m_Segments.push_back(s);
  return *s;
}
SESegmentLinear& SECurve::AddLinearSegment()
{
  SESegmentLinear* s = new SESegmentLinear();
  m_Segments.push_back(s);
  return *s;
}
SESegmentParabolic& SECurve::AddParabolicSegment()
{
  SESegmentParabolic* s = new SESegmentParabolic();
  m_Segments.push_back(s);
  return *s;
}
SESegmentSigmoidal& SECurve::AddSigmoidalSegment()
{
  SESegmentSigmoidal* s = new SESegmentSigmoidal();
  m_Segments.push_back(s);
  return *s;
}
const std::vector<SESegment*>& SECurve::GetSegments()
{
  return m_Segments;
}
const std::vector<const SESegment*> SECurve::GetSegments() const
{
  return std::vector<const SESegment*>(m_Segments.begin(), m_Segments.end());
}

std::string SECurve::ToString() const
{
  std::string str = "Curve";
  for(SESegment* s : m_Segments)
    str += s->ToString();
  return str;
}
void SECurve::ToString(std::ostream& str) const
{
  str << ToString();
}
