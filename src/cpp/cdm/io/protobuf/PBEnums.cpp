/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/PhysiologyEngine.h"
PUSH_PROTO_WARNINGS
#include "pulse/cdm/bind/Enums.pb.h"
POP_PROTO_WARNINGS

const std::string& eEngineInitializationState_Name(eEngineInitializationState s)
{
  return CDM_BIND::eEngineInitializationState_Name((CDM_BIND::eEngineInitializationState)s);
}

const std::string& eSide_Name(eSide m)
{
  return CDM_BIND::eSide_Name((CDM_BIND::eSide)m);
}

const std::string& eGate_Name(eGate m)
{
  return CDM_BIND::eGate_Name((CDM_BIND::eGate)m);
}

const std::string& eSwitch_Name(eSwitch m)
{
  return CDM_BIND::eSwitch_Name((CDM_BIND::eSwitch)m);
}

const std::string& eCharge_Name(eCharge m)
{
  return CDM_BIND::eCharge_Name((CDM_BIND::eCharge)m);
}