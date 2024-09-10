# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.engine import eSerializationFormat
from google.protobuf import json_format

from typing import List
from string import Template

from pulse.study.in_the_moment.unstructured_text import SEUnstructuredTextProperty, SEUnstructuredTextGroup
from pulse.study.bind.InTheMoment_pb2 import UnstructuredTextPhraseData, ExpandedRangeData, \
                                             UnstructuredTextPropertyData, UnstructuredTextGroupData, \
                                             UnstructuredTextCorpusData


def serialize_unstructured_text_phrase_to_bind(src: List[Template], dst: UnstructuredTextPhraseData) -> None:
    for phrase in src:
        dst.Phrase.append(phrase.safe_substitute({}))
def serialize_unstructured_text_phrase_from_bind(src: UnstructuredTextPhraseData, dst: List[Template]) -> None:
    for phrase in src.Phrase:
        dst.append(Template(phrase))

def serialize_unstructured_text_property_to_bind(
    src: SEUnstructuredTextProperty,
    dst: UnstructuredTextPropertyData
) -> None:
    if src.has_property():
        dst.Property = src.get_property()

    dst.PropertyType = src.get_property_type().value

    if src.get_comparison_type() == SEUnstructuredTextProperty.eComparisonType.AnyValue:
        dst.AnyValue = True
    elif src.get_comparison_type() == SEUnstructuredTextProperty.eComparisonType.GreaterThan:
        dst.GT = src._target_min
    elif src.get_comparison_type() == SEUnstructuredTextProperty.eComparisonType.GreaterThanEqualTo:
        dst.GE = src._target_min
    elif src.get_comparison_type() == SEUnstructuredTextProperty.eComparisonType.LessThan:
        dst.LT = src._target_max
    elif src.get_comparison_type() == SEUnstructuredTextProperty.eComparisonType.LessThanEqualTo:
        dst.LE = src._target_max
    elif src.get_comparison_type() == SEUnstructuredTextProperty.eComparisonType.EqualTo:
        dst.EQ = src._target_min
    elif src.get_comparison_type() == SEUnstructuredTextProperty.eComparisonType.Range:
        dst.Range.Range.Minimum = src._target_min
        dst.Range.Range.Maximum = src._target_max
        dst.Range.MinInclusive = src._target_min_inclusive
        dst.Range.MaxInclusive = src._target_max_inclusive
    elif src.get_comparison_type() == SEUnstructuredTextProperty.eComparisonType.Active:
        dst.Active = True
    elif src.get_comparison_type() == SEUnstructuredTextProperty.eComparisonType.Inactive:
        dst.Active = False
    else:
        raise ValueError(f"Unknown comparison type: {src.get_comparison_type()}")
def serialize_unstructured_text_property_from_bind(
    src: UnstructuredTextPropertyData,
    dst: SEUnstructuredTextProperty
) -> None:
    dst.set_property(src.Property)
    dst.set_property_type(SEUnstructuredTextProperty.ePropertyType(src.PropertyType))

    if src.HasField("AnyValue"):
        dst.set_any_value()
    elif src.HasField("GT"):
        dst.set_gt(src.GT)
    elif src.HasField("GE"):
        dst.set_ge(src.GE)
    elif src.HasField("LT"):
        dst.set_lt(src.LT)
    elif src.HasField("LE"):
        dst.set_le(src.LE)
    elif src.HasField("EQ"):
        dst.set_eq(src.eq)
    elif src.HasField("Range"):
        dst.set_range(
            min=src.Range.Range.Minimum,
            max=src.Range.Range.Maximum,
            min_inclusive=src.Range.MinInclusive,
            max_inclusive=src.Range.MaxInclusive
        )
    elif src.HasField("Active"):
        dst.set_active(src.Active)
    else:
        raise ValueError(f"Unknown comparison type: {src.WhichOneOf('Values')}")

def serialize_unstructured_text_group_to_bind(src: SEUnstructuredTextGroup, dst: UnstructuredTextGroupData) -> None:
    for prop in src.get_properties():
        prop_data = dst.Properties.add()
        serialize_unstructured_text_property_to_bind(prop, prop_data)

    for phrases in src.get_phrases():
        phrase_data = dst.Phrases.add()
        serialize_unstructured_text_phrase_to_bind(phrases, phrase_data)
def serialize_unstructured_text_group_from_bind(src: UnstructuredTextGroupData, dst: SEUnstructuredTextGroup) -> None:
    prop_list = dst.get_properties()
    for prop_data in src.Properties:
        prop_list.append(SEUnstructuredTextProperty())
        serialize_unstructured_text_property_from_bind(prop_data, prop_list[-1])

    phrase_list = dst.get_phrases()
    for phrase_data in src.Phrases:
        phrase_list.append(list())
        serialize_unstructured_text_phrase_from_bind(phrase_data, phrase_list[-1])
def serialize_unstructured_text_group_to_string(src: SEUnstructuredTextGroup, fmt: eSerializationFormat) -> str:
    dst = UnstructuredTextGroupData()
    serialize_unstructured_text_group_to_bind(src, dst)
    return json_format.MessageToJson(dst, True, True)
def serialize_unstructured_text_group_from_string(string: str, dst: SEUnstructuredTextGroup, fmt: eSerializationFormat) -> None:
    src = UnstructuredTextGroupData()
    json_format.Parse(string, src)
    serialize_unstructured_text_group_from_bind(src, dst)
def serialize_unstructured_text_group_to_file(src: SEUnstructuredTextGroup, filename: str) -> None:
    string = serialize_unstructured_text_group_to_string(src, eSerializationFormat.JSON)
    with open(filename, "w") as file:
        file.write(string)
def serialize_unstructured_text_group_from_file(filename: str, dst: SEUnstructuredTextGroup) -> None:
    with open(filename) as file:
        string = file.read()
    serialize_unstructured_text_group_from_string(string, dst, eSerializationFormat.JSON)

def serialize_unstructured_text_corpus_to_bind(
    src: List[SEUnstructuredTextGroup],
    dst: UnstructuredTextCorpusData
) -> None:
    for group in src:
        group_data = dst.Group.add()
        serialize_unstructured_text_group_to_bind(group, group_data)
def serialize_unstructured_text_corpus_from_bind(
    src: UnstructuredTextCorpusData,
    dst: List[SEUnstructuredTextGroup]
) -> None:
    for group_data in src.Group:
        dst.append(SEUnstructuredTextGroup())
        serialize_unstructured_text_group_from_bind(group_data, dst[-1])
def serialize_unstructured_text_corpus_to_string(src: List[SEUnstructuredTextGroup], fmt: eSerializationFormat) -> str:
    dst = UnstructuredTextCorpusData()
    serialize_unstructured_text_corpus_to_bind(src, dst)
    return json_format.MessageToJson(dst, True, True)
def serialize_unstructured_text_corpus_from_string(
    string: str,
    dst: List[SEUnstructuredTextGroup],
    fmt: eSerializationFormat
) -> None:
    src = UnstructuredTextCorpusData()
    json_format.Parse(string, src)
    serialize_unstructured_text_corpus_from_bind(src, dst)
def serialize_unstructured_text_corpus_to_file(src: List[SEUnstructuredTextGroup], filename: str) -> None:
    string = serialize_unstructured_text_corpus_to_string(src, eSerializationFormat.JSON)
    with open(filename, "w") as file:
        file.write(string)
def serialize_unstructured_text_corpus_from_file(filename: str, dst: SEUnstructuredTextGroup) -> None:
    with open(filename) as file:
        string = file.read()
    serialize_unstructured_text_corpus_from_string(string, dst, eSerializationFormat.JSON)
