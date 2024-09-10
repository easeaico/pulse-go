# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.validation import SEValidationTarget, \
                             SESegmentValidationPipelineConfig, SESegmentValidationSegmentTable, \
                             SESegmentValidationTarget, SESegmentValidationSegment, \
                             SETimeSeriesValidationTarget, SEPatientTimeSeriesValidation
from pulse.cdm.io.patient import serialize_patient_from_bind, serialize_patient_to_bind
from pulse.cdm.bind.Validation_pb2 import ValidationTargetData, \
                                      SegmentValidationTargetData, SegmentValidationSegmentData, \
                                      SegmentValidationSegmentTableData, SegmentValidationSegmentTableListData, \
                                      SegmentValidationSegmentListData, SegmentValidationPipelineConfigurationData, \
                                      TimeSeriesValidationTargetData, TimeSeriesValidationTargetListData, \
                                      PatientTimeSeriesValidationData, PatientTimeSeriesValidationListData
from pulse.cdm.io.plots import *


def serialize_validation_target_to_bind(src: SEValidationTarget, dst: ValidationTargetData):
    dst.Header = src.get_header()
    dst.Reference = src.get_reference()
    dst.Notes = src.get_notes()
    if src.has_table_formatting():
        dst.TableFormatting = src.get_table_formatting()
def serialize_validation_target_from_bind(src: ValidationTargetData, dst: SEValidationTarget):
    dst.clear()
    dst.set_header(src.Header)
    dst.set_reference(src.Reference)
    dst.set_notes(src.Notes)

def serialize_segment_validation_target_to_bind(src: SESegmentValidationTarget, dst: SegmentValidationTargetData):
    serialize_validation_target_to_bind(src, dst.ValidationTarget)
    if src.get_comparison_type() == SESegmentValidationTarget.eComparisonType.EqualToSegment:
        dst.EqualToSegment = src.get_target_segment()
    elif src.get_comparison_type() == SESegmentValidationTarget.eComparisonType.EqualToValue:
        dst.EqualToValue = src.get_target()
    elif src.get_comparison_type() == SESegmentValidationTarget.eComparisonType.GreaterThanSegment:
        dst.GreaterThanSegment = src.get_target_segment()
    elif src.get_comparison_type() == SESegmentValidationTarget.eComparisonType.GreaterThanValue:
        dst.GreaterThanValue = src.get_target()
    elif src.get_comparison_type() == SESegmentValidationTarget.eComparisonType.LessThanSegment:
        dst.LessThanSegment = src.get_target_segment()
    elif src.get_comparison_type() == SESegmentValidationTarget.eComparisonType.LessThanValue:
        dst.LessThanValue = src.get_target()
    elif src.get_comparison_type() == SESegmentValidationTarget.eComparisonType.TrendsToSegment:
        dst.TrendsToSegment = src.get_target_segment()
    elif src.get_comparison_type() == SESegmentValidationTarget.eComparisonType.TrendsToValue:
        dst.TrendsToValue = src.get_target()
    elif src.get_comparison_type() == SESegmentValidationTarget.eComparisonType.Range:
        dst.Range.Minimum = src.get_target_minimum()
        dst.Range.Maximum = src.get_target_maximum()
    elif src.get_comparison_type() == SESegmentValidationTarget.eComparisonType.NotValidating:
        pass
    else:
        raise ValueError(f"Unknown comparison type: {src.get_comparison_type()}")
def serialize_segment_validation_target_from_bind(src: SegmentValidationTargetData, dst: SESegmentValidationTarget):
    dst.clear()
    serialize_validation_target_from_bind(src.ValidationTarget, dst)
    if src.HasField("EqualToSegment"):
        dst.set_equal_to_segment(src.EqualToSegment)
    elif src.HasField("EqualToValue"):
        dst.set_equal_to_value(src.EqualToValue)
    elif src.HasField("GreaterThanSegment"):
        dst.set_greater_than_segment(src.GreaterThanSegment)
    elif src.HasField("GreaterThanValue"):
        dst.set_greater_than_value(src.GreaterThanValue)
    elif src.HasField("LessThanSegment"):
        dst.set_less_than_segment(src.LessThanSegment)
    elif src.HasField("LessThanValue"):
        dst.set_less_than_value(src.LessThanValue)
    elif src.HasField("TrendsToSegment"):
        dst.set_trends_to_segment(src.TrendsToSegment)
    elif src.HasField("TrendsToValue"):
        dst.set_trends_to_value(src.TrendsTo)
    elif src.HasField("Range"):
        dst.set_range(src.Range.Minimum, src.Range.Maximum)
    elif src.WhichOneof('Expected') is None:  # Not validating
        pass
    else:
        raise ValueError(f"Unknown expected field: {src.WhichOneOf('Expected')}")
def serialize_segment_validation_segment_to_bind(src: SESegmentValidationSegment, dst: SegmentValidationSegmentData):
    dst.Segment = src.get_segment_id()
    dst.Notes = src.get_notes()
    for tgt in src.get_validation_targets():
        serialize_segment_validation_target_to_bind(tgt, dst.SegmentValidationTarget.add())
def serialize_segment_validation_segment_from_bind(src: SegmentValidationSegmentData):
    dst = SESegmentValidationSegment()

    dst.set_notes(src.Notes)
    dst.set_segment_id(src.Segment)

    tgts = []
    for tgtData in src.SegmentValidationTarget:
        tgt = SESegmentValidationTarget()
        serialize_segment_validation_target_from_bind(tgtData, tgt)
        tgts.append(tgt)
    dst.set_validation_targets(tgts)

    return dst

def serialize_segment_validation_segment_table_to_bind(
    src: SESegmentValidationSegmentTable,
    dst: SegmentValidationSegmentTableData
) -> None:
    if src.has_table_name():
        dst.TableName = src.get_table_name()
    if src.has_scenario_name():
        dst.ScenarioName = src.get_scenario_name()
    if src.has_segment():
        dst.Segment = src.get_segment()
    dst.Header.extend(src.get_headers())
    dst.DataRequestFile.extend(src.get_data_request_files())
def serialize_segment_validation_segment_table_from_bind(
    src: SegmentValidationSegmentTableData,
    dst: SESegmentValidationSegmentTable
) -> None:
    dst.set_table_name(src.TableName)
    dst.set_scenario_name(src.ScenarioName)
    dst.set_segment(src.Segment)
    headers = dst.get_headers()
    for h in src.Header:
        headers.append(h)
    dr_files = dst.get_data_request_files()
    for f in src.DataRequestFile:
        dr_files.append(f)
def serialize_segment_validation_segment_table_to_string(
    src: SESegmentValidationSegmentTable,
    fmt: eSerializationFormat
) -> str:
    dst = SegmentValidationSegmentTableData()
    serialize_segment_validation_segment_table_to_bind(src, dst)
    return json_format.MessageToJson(dst, True, True)
def serialize_segment_validation_segment_table_from_string(
    string: str,
    dst: SESegmentValidationSegmentTable,
    fmt: eSerializationFormat
) -> None:
    src = SegmentValidationSegmentTableData()
    json_format.Parse(string, src)
    serialize_segment_validation_segment_table_from_bind(src, dst)
def serialize_segment_validation_segment_table_to_file(src: SESegmentValidationSegmentTable, filename: str) -> None:
    string = serialize_segment_validation_segment_table_to_string(src, eSerializationFormat.JSON)
    with open(filename, "w") as file:
        file.write(string)
def serialize_segment_validation_segment_table_from_file(
    filename: str,
    dst: SESegmentValidationSegmentTable
) -> None:
    with open(filename) as f:
        string = f.read()
    serialize_segment_validation_segment_table_from_string(string, dst, eSerializationFormat.JSON)

def serialize_segment_validation_segment_list_to_bind(src: List[SESegmentValidationSegment], dst: SegmentValidationSegmentListData):
    for segment in src:
        serialize_segment_validation_segment_to_bind(segment, dst.SegmentValidationSegment.add())
def serialize_segment_validation_segment_list_from_bind(src: SegmentValidationSegmentListData):
    dst = []
    for segmentData in src.SegmentValidationSegment:
        dst.append(serialize_segment_validation_segment_from_bind(segmentData))

    return dst
def serialize_segment_validation_segment_list_to_string(src: List[SESegmentValidationSegment], fmt: eSerializationFormat):
    dst = SegmentValidationSegmentListData()
    serialize_segment_validation_segment_list_to_bind(src, dst)
    return json_format.MessageToJson(dst, True, True)
def serialize_segment_validation_segment_list_to_file(src: List[SESegmentValidationSegment], filename: str):
    string = serialize_segment_validation_segment_list_to_string(src, eSerializationFormat.JSON)
    file = open(filename, "w")
    n = file.write(string)
    file.close()
def serialize_segment_validation_segment_list_from_string(string: str, fmt: eSerializationFormat):
    src = SegmentValidationSegmentListData()
    json_format.Parse(string, src)
    return serialize_segment_validation_segment_list_from_bind(src)
def serialize_segment_validation_segment_list_from_file(filename: str):
    with open(filename) as f:
        string = f.read()
    return serialize_segment_validation_segment_list_from_string(string, eSerializationFormat.JSON)

def serialize_segment_validation_segment_table_list_to_bind(
    src: List[SESegmentValidationSegmentTable],
    dst: SegmentValidationSegmentTableListData
) -> None:
    for table in src:
        serialize_segment_validation_segment_table_to_bind(table, dst.Table.add())
def serialize_segment_validation_segment_table_list_from_bind(
    src: SegmentValidationSegmentTableData,
    dst: List[SESegmentValidationSegmentTable]
) -> None:
    for tableData in src.Table:
        table = SESegmentValidationSegmentTable()
        serialize_segment_validation_segment_table_from_bind(tableData, table)
        dst.append(table)
def serialize_segment_validation_segment_table_list_to_string(
    src: List[SESegmentValidationSegmentTable],
    fmt: eSerializationFormat
) -> str:
    dst = SegmentValidationSegmentTableListData()
    serialize_segment_validation_segment_table_list_to_bind(src, dst)
    return json_format.MessageToJson(dst, True, True)
def serialize_segment_validation_segment_table_list_to_file(src: List[SESegmentValidationSegmentTable], filename: str) -> None:
    string = serialize_segment_validation_segment_table_list_to_string(src, eSerializationFormat.JSON)
    with open(filename, "w") as file:
        file.write(string)
def serialize_segment_validation_segment_table_list_from_string(
    string: str,
    dst: List[SESegmentValidationSegmentTable],
    fmt: eSerializationFormat
) -> None:
    src = SegmentValidationSegmentTableListData()
    json_format.Parse(string, src)
    serialize_segment_validation_segment_table_list_from_bind(src, dst)
def serialize_segment_validation_segment_table_list_from_file(
    filename: str,
    dst: List[SESegmentValidationSegmentTable]
) -> None:
    with open(filename) as f:
        string = f.read()
    serialize_segment_validation_segment_table_list_from_string(string, dst, eSerializationFormat.JSON)

def serialize_segment_validation_pipeline_config_from_file(filename: str, dst: SESegmentValidationPipelineConfig):
    with open(filename) as f:
        string = f.read()
    serialize_segment_validation_pipeline_config_from_string(string, dst, eSerializationFormat.JSON)
def serialize_segment_validation_pipeline_config_from_string(string: str, dst: SESegmentValidationPipelineConfig, fmt: eSerializationFormat):
    src = SegmentValidationPipelineConfigurationData()
    json_format.Parse(string, src)
    serialize_segment_validation_pipeline_config_from_bind(src, dst)
def serialize_segment_validation_pipeline_config_from_bind(src: SegmentValidationPipelineConfigurationData, dst: SESegmentValidationPipelineConfig):
    dst.clear()

    serialize_plotter_list_from_bind(src.Plots, dst.get_plotters())
    serialize_segment_validation_segment_table_list_from_bind(src.Tables, dst.get_tables())

def serialize_time_series_validation_target_to_bind(
    src: SETimeSeriesValidationTarget,
    dst: TimeSeriesValidationTargetData
) -> None:
    serialize_validation_target_to_bind(src, dst.ValidationTarget)
    dst.Type = src.get_target_type().value
    if src.has_patient_specific_setting():
        dst.PatientSpecific = src.is_patient_specific()

    if src.get_comparison_type() == SETimeSeriesValidationTarget.eComparisonType.EqualToValue:
        dst.EqualToValue = src.get_target()
    elif src.get_comparison_type() == SETimeSeriesValidationTarget.eComparisonType.Range:
        dst.Range.Minimum = src.get_target_minimum()
        dst.Range.Maximum = src.get_target_maximum()
    elif src.get_comparison_type() == SETimeSeriesValidationTarget.eComparisonType.NotValidating:
        pass
    else:
        raise ValueError(f"Unknown comparison type: {src.get_comparison_type()}")

    if src.has_computed_value():
        dst.ComputedValue = src.get_computed_value()
    if src.has_error_value():
        dst.Error = src.get_error_value()

def serialize_time_series_validation_target_from_bind(
    src: TimeSeriesValidationTargetData,
    dst: SETimeSeriesValidationTarget
) -> None:
    dst.clear()
    serialize_validation_target_from_bind(src.ValidationTarget, dst)
    dst.set_patient_specific_setting(src.PatientSpecific)

    if src.HasField("EqualToValue"):
        dst.set_equal_to(src.EqualToValue, SETimeSeriesValidationTarget.eTargetType(src.Type))
    elif src.HasField("Range"):
        dst.set_range(src.Range.Minimum, src.Range.Maximum, SETimeSeriesValidationTarget.eTargetType(src.Type))
    elif src.WhichOneof('Expected') is None:  # Not validating
        pass
    else:
        raise ValueError(f"Unknown expected field: {src.WhichOneOf('Expected')}")

    if src.HasField("ComputedValue"):
        dst.set_computed_value(src.ComputedValue)
    if src.HasField("Error"):
        dst.set_error_value(src.Error)

def serialize_time_series_validation_target_list_to_bind(
    src: List[SETimeSeriesValidationTarget],
    dst: TimeSeriesValidationTargetListData
) -> None:
    for tgt in src:
        serialize_time_series_validation_target_to_bind(tgt, dst.TimeSeriesValidationTarget.add())
def serialize_time_series_validation_target_list_from_bind(
    src: TimeSeriesValidationTargetListData,
    dst: List[SETimeSeriesValidationTarget]
) -> None:
    for tgtData in src.TimeSeriesValidationTarget:
        tgt = SETimeSeriesValidationTarget()
        serialize_time_series_validation_target_from_bind(tgtData, tgt)
        dst.append(tgt)
def serialize_time_series_validation_target_list_to_string(
    src: List[SETimeSeriesValidationTarget],
    fmt: eSerializationFormat
) -> str:
    dst = TimeSeriesValidationTargetListData()
    serialize_time_series_validation_target_list_to_bind(src, dst)
    return json_format.MessageToJson(dst, True, True)
def serialize_time_series_validation_target_list_to_file(
    src: List[SETimeSeriesValidationTarget],
    filename: str
) -> None:
    string = serialize_time_series_validation_target_list_to_string(src, eSerializationFormat.JSON)
    with open(filename, "w") as f:
        f.write(string)
def serialize_time_series_validation_target_list_from_string(
    string: str,
    dst: List[SETimeSeriesValidationTarget],
    fmt: eSerializationFormat
) -> None:
    src = TimeSeriesValidationTargetListData()
    json_format.Parse(string, src)
    serialize_time_series_validation_target_list_from_bind(src, dst)
def serialize_time_series_validation_target_list_from_file(filename: str, dst: List[SETimeSeriesValidationTarget]) -> None:
    with open(filename) as f:
        string = f.read()
    serialize_time_series_validation_target_list_from_string(string, dst, eSerializationFormat.JSON)

def serialize_patient_time_series_validation_to_bind(
    src: SEPatientTimeSeriesValidation,
    dst: PatientTimeSeriesValidationData
) -> None:
    if src.has_patient():
        serialize_patient_to_bind(src.get_patient(), dst.Patient)
    for tgt_dest, tgts in src.get_targets().items():
        serialize_time_series_validation_target_list_to_bind(tgts, dst.TimeSeriesValidationTargetMap[tgt_dest])
def serialize_patient_time_series_validation_to_string(
    src: SEPatientTimeSeriesValidation,
    fmt: eSerializationFormat
) -> str:
    dst = PatientTimeSeriesValidationData()
    serialize_patient_time_series_validation_to_bind(src, dst)
    return json_format.MessageToJson(dst, True, True)
def serialize_patient_time_series_validation_to_file(
    src: SEPatientTimeSeriesValidation,
    filename: str
) -> None:
    string = serialize_patient_time_series_validation_to_string(src, eSerializationFormat.JSON)
    with open(filename, "w") as f:
        f.write(string)
def serialize_patient_time_series_validation_from_bind(
    src: PatientTimeSeriesValidationData,
    dst: SEPatientTimeSeriesValidation
) -> None:
    dst.clear()
    serialize_patient_from_bind(src.Patient, dst.get_patient())
    tgt_map = dst.get_targets()
    for tgt_dest, tgts in src.TimeSeriesValidationTargetMap.items():
        tgt_map[tgt_dest] = list()
        serialize_time_series_validation_target_list_from_bind(tgts, tgt_map[tgt_dest])
def serialize_patient_time_series_validation_from_string(
    string: str,
    dst: SEPatientTimeSeriesValidation,
    fmt: eSerializationFormat
) -> None:
    src = PatientTimeSeriesValidationData()
    json_format.Parse(string, src)
    serialize_patient_time_series_validation_from_bind(src, dst)
def serialize_patient_time_series_validation_from_file(filename: str, dst: SEPatientTimeSeriesValidation) -> None:
    with open(filename) as f:
        string = f.read()
    serialize_patient_time_series_validation_from_string(string, dst, eSerializationFormat.JSON)

def serialize_patient_time_series_validation_list_to_bind(
    src: List[SEPatientTimeSeriesValidation],
    dst: PatientTimeSeriesValidationListData
) -> None:
    for tgt_map in src:
        serialize_patient_time_series_validation_to_bind(tgt_map, dst.PatientTimeSeriesValidation.add())
def serialize_patient_time_series_validation_list_from_bind(
    src: PatientTimeSeriesValidationListData,
    dst: List[SEPatientTimeSeriesValidation]
) -> None:
    for tgtMapData in src.PatientTimeSeriesValidation:
        tgtMap = SEPatientTimeSeriesValidation()
        serialize_patient_time_series_validation_from_bind(tgtMapData, tgtMap)
        dst.append(tgtMap)

def serialize_patient_time_series_validation_list_to_string(
    src: List[SEPatientTimeSeriesValidation],
    fmt: eSerializationFormat
) -> str:
    dst = PatientTimeSeriesValidationListData()
    serialize_patient_time_series_validation_list_to_bind(src, dst)
    return json_format.MessageToJson(dst, True, True)
def serialize_patient_time_series_validation_list_to_file(
    src: List[SEPatientTimeSeriesValidation],
    filename: str
) -> None:
    string = serialize_patient_time_series_validation_list_to_string(src, eSerializationFormat.JSON)
    with open(filename, "w") as f:
        f.write(string)
def serialize_patient_time_series_validation_list_from_string(
    string: str,
    dst: List[SEPatientTimeSeriesValidation],
    fmt: eSerializationFormat
) -> None:
    src = PatientTimeSeriesValidationListData()
    json_format.Parse(string, src)
    serialize_patient_time_series_validation_list_from_bind(src, dst)
def serialize_patient_time_series_validation_list_from_file(
    filename: str,
    dst: List[SEPatientTimeSeriesValidation]
) -> None:
    with open(filename) as f:
        string = f.read()
    serialize_patient_time_series_validation_list_from_string(string, dst, eSerializationFormat.JSON)
