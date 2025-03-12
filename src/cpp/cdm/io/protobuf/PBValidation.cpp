/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
PUSH_PROTO_WARNINGS
#include "pulse/cdm/bind/Validation.pb.h"
POP_PROTO_WARNINGS
#include "cdm/io/protobuf/PBValidation.h"
#include "cdm/io/protobuf/PBUtils.h"
#include "cdm/engine/SEValidationTarget.h"
#include "cdm/utils/FileUtils.h"

void PBValidation::Serialize(const CDM_BIND::ValidationTargetData& src, SEValidationTarget& dst)
{
  dst.m_Header = src.header();
  dst.m_Reference = src.reference();
  dst.m_Notes = src.notes();

  if (src.has_goodpercenterror())
    dst.SetGoodPercentError(src.goodpercenterror());
  if (src.has_fairpercenterror())
    dst.SetFairPercentError(src.fairpercenterror());
}
void PBValidation::Serialize(const SEValidationTarget& src, CDM_BIND::ValidationTargetData& dst)
{
  dst.set_header(src.m_Header);
  dst.set_reference(src.m_Reference);
  dst.set_notes(src.m_Notes);

  dst.set_goodpercenterror(src.GetGoodPercentError());
  dst.set_fairpercenterror(src.GetFairPercentError());
}
void PBValidation::Load(const CDM_BIND::SegmentValidationTargetData& src, SESegmentValidationTarget& dst)
{
  dst.Clear();
  PBValidation::Serialize(src, dst);
}
void PBValidation::Serialize(const CDM_BIND::SegmentValidationTargetData& src, SESegmentValidationTarget& dst)
{
  PBValidation::Serialize(src.validationtarget(), dst);
  dst.SetComparisonFormula(src.comparisonformula());
}
CDM_BIND::SegmentValidationTargetData* PBValidation::Unload(const SESegmentValidationTarget& src)
{
  CDM_BIND::SegmentValidationTargetData* dst = new CDM_BIND::SegmentValidationTargetData();
  PBValidation::Serialize(src, *dst);
  return dst;
}
void PBValidation::Serialize(const SESegmentValidationTarget& src, CDM_BIND::SegmentValidationTargetData& dst)
{
  PBValidation::Serialize(src, *dst.mutable_validationtarget());
  dst.set_comparisonformula(src.GetComparisonFormula());
}
void PBValidation::Load(const CDM_BIND::SegmentValidationSegmentData& src, std::vector<SESegmentValidationTarget*>& dst)
{
  PBValidation::Serialize(src, dst);
}
void PBValidation::Serialize(const CDM_BIND::SegmentValidationSegmentData& src, std::vector<SESegmentValidationTarget*>& dst)
{
  for (int i = 0; i < src.segmentvalidationtarget_size(); i++)
  {
    auto& data = src.segmentvalidationtarget()[i];
    SESegmentValidationTarget* tgt = new SESegmentValidationTarget();
    Load(data, *tgt);
    dst.push_back(tgt);
  }
}
bool PBValidation::SerializeFromString(const std::string& src, std::vector<SESegmentValidationTarget*>& dst, eSerializationFormat m, Logger* logger)
{
  CDM_BIND::SegmentValidationSegmentData data;
  if (!PBUtils::SerializeFromString(src, data, m, logger))
    return false;
  PBValidation::Serialize(data, dst);
  return true;
}
bool PBValidation::SerializeFromFile(const std::string& filename, std::vector<SESegmentValidationTarget*>& dst, Logger* logger)
{
  CDM_BIND::SegmentValidationSegmentData data;
  if (!PBUtils::SerializeFromFile(filename, data, logger))
    return false;
  PBValidation::Load(data, dst);
  return true;
}
CDM_BIND::SegmentValidationSegmentData* PBValidation::Unload(const std::vector<const SESegmentValidationTarget*>& src)
{
  CDM_BIND::SegmentValidationSegmentData* dst = new CDM_BIND::SegmentValidationSegmentData();
  PBValidation::Serialize(src, *dst);
  return dst;
}
void PBValidation::Serialize(const std::vector<const SESegmentValidationTarget*>& src, CDM_BIND::SegmentValidationSegmentData& dst)
{
  for (const SESegmentValidationTarget* vt : src)
  {
    PBValidation::Serialize(*vt, *dst.add_segmentvalidationtarget());
  }
}
bool PBValidation::SerializeToString(const std::vector<const SESegmentValidationTarget*>& src, std::string& output, eSerializationFormat m, Logger* logger)
{
  CDM_BIND::SegmentValidationSegmentData data;
  PBValidation::Serialize(src, data);
  if (!PBUtils::SerializeToString(data, output, m, logger))
    return false;
  return true;
}
bool PBValidation::SerializeToFile(const std::vector<const SESegmentValidationTarget*>& src, const std::string& filename, Logger* logger)
{
  CDM_BIND::SegmentValidationSegmentData data;
  PBValidation::Serialize(src, data);
  if (!PBUtils::SerializeToFile(data, filename, logger))
    return false;
  return true;
}

void PBValidation::Load(const CDM_BIND::TimeSeriesValidationTargetData& src, SETimeSeriesValidationTarget& dst)
{
  dst.Clear();
  PBValidation::Serialize(src, dst);
}
void PBValidation::Serialize(const CDM_BIND::TimeSeriesValidationTargetData& src, SETimeSeriesValidationTarget& dst)
{
  PBValidation::Serialize(src.validationtarget(), dst);
  switch (src.Expected_case())
  {
  case CDM_BIND::TimeSeriesValidationTargetData::kEqualToValue:
    dst.SetEqualTo(src.equaltovalue(), (SETimeSeriesValidationTarget::eTargetType)src.type());
    break;
  case CDM_BIND::TimeSeriesValidationTargetData::kRange:
    dst.SetRange(src.range().minimum(), src.range().maximum(), (SETimeSeriesValidationTarget::eTargetType)src.type());
    break;
  default: break;
  }
}
CDM_BIND::TimeSeriesValidationTargetData* PBValidation::Unload(const SETimeSeriesValidationTarget& src)
{
  CDM_BIND::TimeSeriesValidationTargetData* dst = new CDM_BIND::TimeSeriesValidationTargetData();
  PBValidation::Serialize(src, *dst);
  return dst;
}
void PBValidation::Serialize(const SETimeSeriesValidationTarget& src, CDM_BIND::TimeSeriesValidationTargetData& dst)
{
  PBValidation::Serialize(src, *dst.mutable_validationtarget());
  dst.set_type((CDM_BIND::TimeSeriesValidationTargetData_eType)src.m_TargetType);
  switch (src.m_ComparisonType)
  {
  case SETimeSeriesValidationTarget::eComparisonType::EqualToValue:
    dst.set_equaltovalue(src.m_Target);
    break;
  case SETimeSeriesValidationTarget::eComparisonType::Range:
    dst.mutable_range()->set_minimum(src.m_TargetMinimum);
    dst.mutable_range()->set_maximum(src.m_TargetMaximum);
    break;
  case SETimeSeriesValidationTarget::eComparisonType::None:
    src.Warning("TimeSeriesValidationTarget "+src.GetHeader()+" does not have a comparision type");
    break;
  }
}
void PBValidation::Load(const CDM_BIND::TimeSeriesValidationTargetListData& src, std::vector<SETimeSeriesValidationTarget*>& dst)
{
  PBValidation::Serialize(src, dst);
}
void PBValidation::Serialize(const CDM_BIND::TimeSeriesValidationTargetListData& src, std::vector<SETimeSeriesValidationTarget*>& dst)
{
  for (int i = 0; i < src.timeseriesvalidationtarget_size(); i++)
  {
    auto& data = src.timeseriesvalidationtarget()[i];
    SETimeSeriesValidationTarget* tgt = new SETimeSeriesValidationTarget();
    Load(data, *tgt);
    dst.push_back(tgt);
  }
}
bool PBValidation::SerializeFromString(const std::string& src, std::vector<SETimeSeriesValidationTarget*>& dst, eSerializationFormat m, Logger* logger)
{
  CDM_BIND::TimeSeriesValidationTargetListData data;
  if (!PBUtils::SerializeFromString(src, data, m, logger))
    return false;
  PBValidation::Serialize(data, dst);
  return true;
}
bool PBValidation::SerializeFromFile(const std::string& filename, std::vector<SETimeSeriesValidationTarget*>& dst, Logger* logger)
{
  CDM_BIND::TimeSeriesValidationTargetListData data;
  if (!PBUtils::SerializeFromFile(filename, data, logger))
    return false;
  PBValidation::Load(data, dst);
  return true;
}
CDM_BIND::TimeSeriesValidationTargetListData* PBValidation::Unload(const std::vector<const SETimeSeriesValidationTarget*>& src)
{
  CDM_BIND::TimeSeriesValidationTargetListData* dst = new CDM_BIND::TimeSeriesValidationTargetListData();
  PBValidation::Serialize(src, *dst);
  return dst;
}
void PBValidation::Serialize(const std::vector<const SETimeSeriesValidationTarget*>& src, CDM_BIND::TimeSeriesValidationTargetListData& dst)
{
  for (const SETimeSeriesValidationTarget* vt : src)
  {
    PBValidation::Serialize(*vt, *dst.add_timeseriesvalidationtarget());
  }
}
bool PBValidation::SerializeToString(const std::vector<const SETimeSeriesValidationTarget*>& src, std::string& output, eSerializationFormat m, Logger* logger)
{
  CDM_BIND::TimeSeriesValidationTargetListData data;
  PBValidation::Serialize(src, data);
  if (!PBUtils::SerializeToString(data, output, m, logger))
    return false;
  return true;
}
bool PBValidation::SerializeToFile(const std::vector<const SETimeSeriesValidationTarget*>& src, const std::string& filename, Logger* logger)
{
  CDM_BIND::TimeSeriesValidationTargetListData data;
  PBValidation::Serialize(src, data);
  if (!PBUtils::SerializeToFile(data, filename, logger))
    return false;
  return true;
}
