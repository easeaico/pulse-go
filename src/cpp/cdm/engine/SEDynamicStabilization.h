/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include "cdm/engine/SEEngineStabilization.h"
class SEDynamicStabilizationEngineConvergence;

class CDM_DECL SEDynamicStabilization : public SEEngineStabilization
{
  friend class PBEngine;//friend the serialization class
public:
  SEDynamicStabilization(Logger* logger);
  virtual ~SEDynamicStabilization();

  void Clear() override;

  bool SerializeToString(std::string& output, eSerializationFormat m) const;
  bool SerializeToFile(const std::string& filename) const;
  bool SerializeFromString(const std::string& src, eSerializationFormat m);
  bool SerializeFromFile(const std::string& filename);

  bool Stabilize(Controller& engine, const std::string& criteria) override;
  bool StabilizeConditions(Controller& engine, const SEConditionManager& conditions) override;

  bool HasConvergenceCriteria(const std::string& name) const override;
  void RemoveConvergenceCriteria(const std::string& name) override;
  virtual SEDynamicStabilizationEngineConvergence& GetConvergenceCriteria(const std::string& name);
  virtual const SEDynamicStabilizationEngineConvergence* GetConvergenceCriteria(const std::string& name) const;
  virtual const std::map<std::string, SEDynamicStabilizationEngineConvergence*>& GetConvergenceCriteria() const;

protected:

  virtual bool Stabilize(Controller& engine, const SEDynamicStabilizationEngineConvergence& criteria);

  std::map<std::string, SEDynamicStabilizationEngineConvergence*> m_ConvergenceCriteria;

  bool Merge();
  SEDynamicStabilizationEngineConvergence* m_MergedConditions;
  std::map<std::string, SEDynamicStabilizationEngineConvergence*> m_ActiveConditions;
};

