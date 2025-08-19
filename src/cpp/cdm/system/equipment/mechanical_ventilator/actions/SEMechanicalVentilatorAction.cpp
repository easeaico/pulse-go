/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#include "cdm/CommonDefs.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorAction.h"
#include "cdm/substance/SESubstanceManager.h"

SEMechanicalVentilatorAction::SEMechanicalVentilatorAction(Logger* logger) : SEEquipmentAction(logger)
{

}

SEMechanicalVentilatorAction::~SEMechanicalVentilatorAction()
{

}

void SEMechanicalVentilatorAction::Clear()
{
  SEEquipmentAction::Clear();
}

bool SEMechanicalVentilatorAction::IsActive() const
{
  return SEEquipmentAction::IsActive();
}

bool SEMechanicalVentilatorAction::IsValid() const
{
  return SEEquipmentAction::IsValid();
}
