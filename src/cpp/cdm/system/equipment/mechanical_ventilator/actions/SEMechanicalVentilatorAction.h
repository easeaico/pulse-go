/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#pragma once
#include "cdm/system/equipment/SEEquipmentAction.h"

class CDM_DECL SEMechanicalVentilatorAction : public SEEquipmentAction
{
  friend class PBEquipmentAction;//friend the serialization class
public:

  SEMechanicalVentilatorAction(Logger* logger);
  virtual ~SEMechanicalVentilatorAction();

  static constexpr char const* ActionType = "Mechanical Ventilator";
  std::string GetActionType() const override { return ActionType; }

  void Clear() override;

  bool IsActive() const override;
  bool IsValid() const override;
};
