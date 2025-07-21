# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.engine.PulseEnginePool import SEEnginePoolEngine

from pulse.cdm.enums import eSerializationFormat
from pulse.cdm.io.engine import serialize_engine_initialization_to_bind, \
                                serialize_actions_to_bind
from pulse.cdm.bind.Engine_pb2 import EngineInitializationData, EngineInitializationListData, \
                                      ActionMapData, DataRequestedListData
from google.protobuf import json_format

def serialize_pool_engine_initializations_to_string(src: [SEEnginePoolEngine], fmt: eSerializationFormat):
    dst = EngineInitializationListData()
    for e in src:
        bind = EngineInitializationData()
        serialize_engine_initialization_to_bind(e.engine_initialization, bind)
        dst.EngineInitialization.append(bind)
    return json_format.MessageToJson(dst, True, True)

def serialize_pool_actions_to_string(src: [SEEnginePoolEngine], fmt: eSerializationFormat):
    dst = ActionMapData()
    for e in src:
        if e.is_active:
            serialize_actions_to_bind(e.actions, dst.ActionMap[e.get_id()])
    return json_format.MessageToJson(dst, True, True)

def serialize_pool_data_requested_from_string(string: str, dst: {int, SEEnginePoolEngine}, fmt: eSerializationFormat):
    src = DataRequestedListData()
    json_format.Parse(string, src)
    for dr in src.DataRequested:
        if dr.ID not in dst.keys():
            raise Exception("Engine pool has no engine by data requested id "+str(dr.ID))
        engine = dst[dr.ID]
        engine.is_active = dr.IsActive
        engine.data_requested.is_active = dr.IsActive
        if engine.is_active:
            for i, key in enumerate(engine.data_requested.values.keys()):
                engine.data_requested.values[key] = dr.Value[i]
