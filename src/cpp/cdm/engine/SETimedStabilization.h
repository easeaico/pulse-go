/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include "cdm/engine/SEEngineStabilization.h"

class CDM_DECL SETimedStabilization : public SEEngineStabilization
{
  friend class PBEngine;//friend the serialization class
public:
  SETimedStabilization(Logger* logger);
  virtual ~SETimedStabilization();

  void Clear() override;

  bool SerializeToString(std::string& output, eSerializationFormat m) const;
  bool SerializeToFile(const std::string& filename) const;
  bool SerializeFromString(const std::string& src, eSerializationFormat m);
  bool SerializeFromFile(const std::string& filename);

  bool Stabilize(Controller& engine, const std::string& criteria) override;
  bool StabilizeConditions(Controller& engine, const SEConditionManager& conditions) override;

  bool HasConvergenceCriteria(const std::string& name) const override;
  void RemoveConvergenceCriteria(const std::string& name) override;
  virtual const std::map<std::string,SEScalarTime*>& GetConvergenceCriterias() const;
  virtual SEScalarTime& GetConvergenceCriteria(const std::string& name);
  virtual const SEScalarTime* GetConvergenceCriteria(const std::string& name) const;

protected:

  virtual bool Stabilize(Controller& engine, const SEScalarTime& time);

  std::map<std::string,SEScalarTime*> m_ConvergenceCriteria;
};
