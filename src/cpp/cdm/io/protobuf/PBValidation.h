/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
CDM_BIND_DECL2(ValidationTarget)
CDM_BIND_DECL2(SegmentValidationTarget)
CDM_BIND_DECL2(SegmentValidationSegment)
CDM_BIND_DECL2(TimeSeriesValidationTarget)
CDM_BIND_DECL2(TimeSeriesValidationTargetList)

class CDM_DECL PBValidation
{
public:
  static void Serialize(const CDM_BIND::ValidationTargetData& src, SEValidationTarget& dst);
  static void Serialize(const SEValidationTarget& src, CDM_BIND::ValidationTargetData& dst);

  static void Load(const CDM_BIND::SegmentValidationTargetData& src, SESegmentValidationTarget& dst);
  static CDM_BIND::SegmentValidationTargetData* Unload(const SESegmentValidationTarget& src);
  static void Serialize(const CDM_BIND::SegmentValidationTargetData& src, SESegmentValidationTarget& dst);
  static void Serialize(const SESegmentValidationTarget& src, CDM_BIND::SegmentValidationTargetData& dst);
  static void Load(const CDM_BIND::SegmentValidationSegmentData& src, std::vector<SESegmentValidationTarget*>& dst);
  static void Serialize(const CDM_BIND::SegmentValidationSegmentData& src, std::vector<SESegmentValidationTarget*>& dst);
  static bool SerializeFromString(const std::string& src, std::vector<SESegmentValidationTarget*>& dst, eSerializationFormat m, Logger* logger);
  static bool SerializeFromFile(const std::string& filename, std::vector<SESegmentValidationTarget*>& dst, Logger* logger);
  static CDM_BIND::SegmentValidationSegmentData* Unload(const std::vector<const SESegmentValidationTarget*>& src);
  static void Serialize(const std::vector<const SESegmentValidationTarget*>& src, CDM_BIND::SegmentValidationSegmentData& dst);
  static bool SerializeToString(const std::vector<const SESegmentValidationTarget*>& src, std::string& output, eSerializationFormat m, Logger* logger);
  static bool SerializeToFile(const std::vector<const SESegmentValidationTarget*>& src, const std::string& filename, Logger* logger);

  static void Load(const CDM_BIND::TimeSeriesValidationTargetData& src, SETimeSeriesValidationTarget& dst);
  static CDM_BIND::TimeSeriesValidationTargetData* Unload(const SETimeSeriesValidationTarget& src);
  static void Serialize(const CDM_BIND::TimeSeriesValidationTargetData& src, SETimeSeriesValidationTarget& dst);
  static void Serialize(const SETimeSeriesValidationTarget& src, CDM_BIND::TimeSeriesValidationTargetData& dst);
  static void Load(const CDM_BIND::TimeSeriesValidationTargetListData& src, std::vector<SETimeSeriesValidationTarget*>& dst);
  static void Serialize(const CDM_BIND::TimeSeriesValidationTargetListData& src, std::vector<SETimeSeriesValidationTarget*>& dst);
  static bool SerializeFromString(const std::string& src, std::vector<SETimeSeriesValidationTarget*>& dst, eSerializationFormat m, Logger* logger);
  static bool SerializeFromFile(const std::string& filename, std::vector<SETimeSeriesValidationTarget*>& dst, Logger* logger);
  static CDM_BIND::TimeSeriesValidationTargetListData* Unload(const std::vector<const SETimeSeriesValidationTarget*>& src);
  static void Serialize(const std::vector<const SETimeSeriesValidationTarget*>& src, CDM_BIND::TimeSeriesValidationTargetListData& dst);
  static bool SerializeToString(const std::vector<const SETimeSeriesValidationTarget*>& src, std::string& output, eSerializationFormat m, Logger* logger);
  static bool SerializeToFile(const std::vector<const SETimeSeriesValidationTarget*>& src, const std::string& filename, Logger* logger);
};
