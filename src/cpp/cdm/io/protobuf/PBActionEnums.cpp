/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
PUSH_PROTO_WARNINGS
#include "pulse/cdm/bind/Actions.pb.h"
POP_PROTO_WARNINGS
#include "cdm/engine/SESerializeState.h"

const std::string& eSerialization_Mode_Name(eSerialization_Mode m)
{
  return CDM_BIND::SerializeStateData::eMode_Name((CDM_BIND::SerializeStateData::eMode)m);
}
