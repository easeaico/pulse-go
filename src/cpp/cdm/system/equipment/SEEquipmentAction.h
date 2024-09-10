/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#pragma once
#include "cdm/engine/SEAction.h"

class CDM_DECL SEEquipmentAction : public SEAction
{
  friend class PBEquipmentAction;//friend the serialization class
public:

  SEEquipmentAction(Logger* logger);
  virtual ~SEEquipmentAction();

  void Clear() override { SEAction::Clear(); }
  bool IsValid() const override { return SEAction::IsValid(); }

  void Activate() override { SEAction::Activate(); }
  bool IsActive() const override { return SEAction::IsActive(); }
  void Deactivate() override { SEAction::Deactivate(); }
};  