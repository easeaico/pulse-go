/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

#include "cdm/CommonDefs.h"
#include "cdm/engine/SEDataRequest.h"
#include "cdm/engine/SEEventManager.h"

class CDM_DECL SEValidationTarget : public Loggable
{
  friend class PBValidation;//friend the serialization class
protected:
  SEValidationTarget() { Clear(); }
public:
  virtual ~SEValidationTarget() = default;

  virtual void Clear();
  virtual bool IsValid();

  std::string GetHeader() const { return m_Header; }
  void SetHeader(const std::string& h) { m_Header = h; }

  std::string GetReference() const { return m_Reference; }
  void SetReference(const std::string& c) { m_Reference = c; }

  std::string GetNotes() const { return m_Notes; }
  void SetNotes(const std::string& n) { m_Notes = n; }

  double GetTargetMaximum() const { return m_TargetMaximum; }
  double GetTargetMinimum() const { return m_TargetMinimum; }
  double GetTarget() const { return m_Target; }

protected:
  std::string                                m_Header;
  std::string                                m_Reference;
  std::string                                m_Notes;

  double                                     m_Target;
  double                                     m_TargetMaximum;
  double                                     m_TargetMinimum;
};

class CDM_DECL SESegmentValidationTarget : public SEValidationTarget
{
  friend class PBValidation;//friend the serialization class
public:
  enum class eComparisonType { None=0, EqualToValue, EqualToSegment, GreaterThanValue, GreaterThanSegment,
                               LessThanValue, LessThanSegment, TrendsToValue, TrendsToSegment, Range };
  SESegmentValidationTarget();
  virtual ~SESegmentValidationTarget() = default;

  static bool SerializeToString(const std::vector<const SESegmentValidationTarget*>& src, std::string& output, eSerializationFormat m, Logger* logger);
  static bool SerializeToFile(const std::vector<const SESegmentValidationTarget*>& src, const std::string& filename, Logger* logger);
  static bool SerializeFromString(const std::string& src, std::vector<SESegmentValidationTarget*>& dst, eSerializationFormat m, Logger* logger);
  static bool SerializeFromFile(const std::string& filename, std::vector<SESegmentValidationTarget*>& dst, Logger* logger);

  void Clear() override;
  eComparisonType GetComparisonType() const { return m_ComparisonType; }

  int    GetTargetSegment() const { return m_TargetSegment; }

  void   SetEqualToSegment(int segment);
  void   SetEqualToValue(double d);
  void   SetGreaterThanSegment(int segment);
  void   SetGreaterThanValue(double d);
  void   SetLessThanSegment(int segment);
  void   SetLessThanValue(double d);
  void   SetTrendsToSegment(int segment);
  void   SetTrendsToValue(double d);
  void   SetRange(double min, double max);

protected:
  eComparisonType                            m_ComparisonType;
  int                                        m_TargetSegment;
};

class CDM_DECL SETimeSeriesValidationTarget : public SEValidationTarget
{
  friend class PBValidation;//friend the serialization class
public:
  enum class eComparisonType { None=0, EqualToValue, Range };
  enum class eTargetType { Mean=0, Minimum, Maximum,
    MeanPerIdealWeight_kg, MaxPerIdealWeight_kg, MinPerIdealWeight_kg };
  SETimeSeriesValidationTarget();
  virtual ~SETimeSeriesValidationTarget() = default;

  void Clear() override;

  static bool SerializeToString(const std::vector<const SETimeSeriesValidationTarget*>&src, std::string& output, eSerializationFormat m, Logger* logger);
  static bool SerializeToFile(const std::vector<const SETimeSeriesValidationTarget*>& src, const std::string& filename, Logger* logger);
  static bool SerializeFromString(const std::string& src, std::vector<SETimeSeriesValidationTarget*>& dst, eSerializationFormat m, Logger* logger);
  static bool SerializeFromFile(const std::string& filename, std::vector<SETimeSeriesValidationTarget*>& dst, Logger* logger);

  eTargetType GetTargetType() const { return m_TargetType; }
  eComparisonType GetComparisonType() const { return m_ComparisonType; }

  void   SetEqualTo(double d, eTargetType t);
  void   SetRange(double min, double max, eTargetType t);

  // Maybe we want a different class to do the comparision
  // For now, I just put it in line here for the CircuitOptimizer
  bool ComputeError();
  double GetError() const { return m_Error; }
  double GetDataValue() const { return m_ComparisonValue; }
  std::vector<double>& GetData() { return m_Data; }

protected:
  eTargetType                                m_TargetType;
  eComparisonType                            m_ComparisonType;

  // Not serializing
  double                                     m_Error;
  std::vector<double>                        m_Data;
  double                                     m_ComparisonValue;
};
