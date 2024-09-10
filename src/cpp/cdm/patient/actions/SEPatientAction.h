/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include "cdm/engine/SEAction.h"

class CDM_DECL SEPatientAction : public SEAction
{
  friend class PBPatientAction;//friend the serialization class
public:

  SEPatientAction(Logger* logger=nullptr);
  virtual ~SEPatientAction();

  static constexpr char const* ActionType = "Patient";
  std::string GetActionType() const override { return ActionType; }

  void Clear() override { SEAction::Clear(); }
  bool IsValid() const override { return SEAction::IsValid(); }

  void Activate() override { SEAction::Activate(); }
  bool IsActive() const override { return SEAction::IsActive(); }
  void Deactivate() override { SEAction::Deactivate(); }
};
