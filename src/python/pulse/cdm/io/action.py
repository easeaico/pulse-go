# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.engine import (
    SEAction, SEAdvanceTime, SEAdvanceUntilStable, SESerializeRequested, SESerializeState
)
from pulse.cdm.io.scalars import serialize_scalar_time_to_bind
from pulse.cdm.bind.Actions_pb2 import (
    ActionData, AdvanceTimeData, AdvanceUntilStableData, OverridesData,
    SerializeRequestedData, SerializeStateData
)

def serialize_action_to_bind(src: SEAction, dst: ActionData):
    if src.has_comment():
        dst.Comment = src.get_comment()

def serialize_action_from_bind(src: ActionData, dst: SEAction):
    dst.set_comment(src.Comment)

def serialize_advance_time_to_bind(src: SEAdvanceTime, dst: AdvanceTimeData):
    serialize_action_to_bind(src, dst.Action)
    if src.has_time():
        serialize_scalar_time_to_bind(src.get_time(), dst.Time)

def serialize_advance_time_from_bind(src: AdvanceTimeData, dst: SEAdvanceTime):
    serialize_action_from_bind(src.PatientAction, dst)
    raise Exception("serialize_advance_time_from_bind not implemented")

def serialize_advance_until_stable_to_bind(src: SEAdvanceUntilStable, dst: AdvanceUntilStableData):
    serialize_action_to_bind(src, dst.Action)
    if src.has_criteria():
        dst.Criteria = src.get_criteria()

def serialize_advance_until_stable_from_bind(src: AdvanceUntilStableData, dst: SEAdvanceUntilStable):
    serialize_action_from_bind(src.PatientAction, dst)
    raise Exception("serialize_advance_until_stable_from_bind not implemented")

def serialize_serialize_requested_to_bind(src: SESerializeRequested, dst: SerializeRequestedData):
    serialize_action_to_bind(src, dst.Action)
    if src.has_filename():
        dst.Filename = src.get_filename()

def serialize_serialize_requested_from_bind(src: SerializeRequestedData, dst: SESerializeRequested):
    serialize_action_from_bind(src.PatientAction, dst)
    raise Exception("serialize_serialize_requested_from_bind not implemented")

def serialize_serialize_state_to_bind(src: SESerializeState, dst: SerializeStateData):
    serialize_action_to_bind(src, dst.Action)
    dst.Mode = src.get_mode().value
    if src.has_filename():
        dst.Filename = src.get_filename()

def serialize_serialize_state_from_bind(src: SerializeStateData, dst: SESerializeState):
    serialize_action_from_bind(src.PatientAction, dst)
    raise Exception("serialize_serialize_state_from_bind not implemented")
