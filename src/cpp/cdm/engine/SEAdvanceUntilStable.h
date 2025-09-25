/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include "cdm/engine/SEAction.h"

class CDM_DECL SEAdvanceUntilStable : public SEAction
{
  friend class PBAction;//friend the serialization class
public:

  SEAdvanceUntilStable(Logger* logger=nullptr);
  virtual ~SEAdvanceUntilStable();

  static constexpr char const* Name = "Advance Until Stable";
  std::string GetName() const override { return Name; }

  void Clear() override; //clear memory

  bool IsValid() const override;

  virtual bool HasCriteria() const;
  virtual std::string GetCriteria() const;
  virtual void SetCriteria(const std::string& c);
  virtual void InvalidateCriteria();

  const SEScalar* GetScalar(const std::string& name) override;

protected:
  std::string m_Criteria;
};
