/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#include "cdm/CommonDefs.h"
#include "cdm/system/equipment/SEEquipmentAction.h"

SEEquipmentAction::SEEquipmentAction(Logger* logger) : SEAction(logger)
{

}

SEEquipmentAction::~SEEquipmentAction()
{
  Clear();
}
