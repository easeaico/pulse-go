# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.testing import *
from pulse.cdm.bind.TestReport_pb2 import *
from pulse.cdm.engine import eSerializationFormat
from pulse.cdm.io.scalars import serialize_scalar_time_from_bind, serialize_scalar_time_to_bind
from pulse.cdm.io.properties import serialize_function_from_bind, serialize_function_to_bind
from google.protobuf import json_format

def serialize_test_report_from_file(filename: str, dst: SETestReport):
    with open(filename) as f:
        string = f.read()
    serialize_test_report_from_string(string, dst, eSerializationFormat.JSON)
def serialize_test_report_from_string(string: str, dst: SETestReport, fmt: eSerializationFormat):
    src = TestReportData()
    json_format.Parse(string, src)
    serialize_test_report_from_bind(src, dst)
def serialize_test_report_from_bind(src: TestReportData, dst: SETestReport):
    dst.clear()
    for ts_data in src.TestSuite:
        ts = dst.create_test_suite()
        serialize_test_suite_from_bind(ts_data, ts)
def serialize_test_report_to_bind(src: SETestReport, dst: TestReportData):
    for ts in src.get_test_suites():
        ts_data = dst.TestSuite.add()
        serialize_test_suite_to_bind(ts, ts_data)
def serialize_test_report_to_string(src: SETestReport, fmt: eSerializationFormat):
    dst = TestReportData()
    serialize_test_report_to_bind(src, dst)
    return json_format.MessageToJson(dst, True, True)
def serialize_test_report_to_file(src: SETestReport, filename: str):
    string = serialize_test_report_to_string(src, eSerializationFormat.JSON)
    file = open(filename, "w")
    n = file.write(string)
    file.close()

def serialize_test_suite_from_bind(src: TestSuiteData, dst: SETestSuite):
    dst.clear()
    dst.set_name(src.Name)
    dst.set_performed(src.Performed)

    for r in src.Requirement:
        dst.add_requirement(r)
    '''
    for es_data in src.ErrorStats:
        es = dst.create__error_statistic()
        serialize_test_error_statistics_from_bind(es_data, es)
    '''
    for tc_data in src.TestCase:
        tc = dst.create_test_case()
        serialize_test_case_from_bind(tc_data, tc)
def serialize_test_suite_to_bind(src: SETestSuite, dst: TestSuiteData):
    if src.has_name():
        dst.Name = src.get_name()
    dst.Performed = src.get_performed()
    dst.Errors = src.get_num_errors()
    dst.Warnings = src.get_num_warnings()
    dst.Tests = len(src.get_test_cases())

    for r in src.get_requirements():
        dst.Requirement.add(r)
    '''
    for es in src.get_error_statistics():
        es_data = dst.ErrorStats.add()
        serialize_test_error_statistics_to_bind(es, es_data)
    '''
    for tc in src.get_test_cases():
        tc_data = dst.TestCase.add()
        serialize_test_case_to_bind(tc, tc_data)

def serialize_test_case_from_bind(src: TestCaseData, dst: SETestCase):
    dst.clear()
    dst.set_name(src.Name)
    #serialize_scalar_time_from_bind(src.Duration, dst.get_duration())
    for f in src.Failure:
        dst.add_failure(f)
    for w in src.Warning:
        dst.add_warning(w)
    '''
    for es_data in src.ErrorStats:
        es = dst.create_test_error_statistics()
        serialize_test_error_statistics_from_bind(es_data, es)
    '''
def serialize_test_case_to_bind(src: SETestCase, dst: TestCaseData):
    if src.has_name():
        dst.Name = src.get_name()
    if src.has_duration():
        serialize_scalar_time_to_bind(src.get_duration(), dst.Duration)
    for f in src.get_failures():
        dst.Failure.append(f)
    for w in src.get_warnings():
        dst.Warning.append(w)
    '''
    for es in src.get_error_statistics():
        es_data = dst.ErrorStats.add()
        serialize_test_error_statistics_to_bind(es, es_data)
    '''

'''
def serialize_test_error_statistics_from_bind(src: TestErrorStatisticsData, dst: SETestErrorStatistics):
    dst.clear()
    dst.set_property_name(src.PropertyName)
    dst.set_percent_tolerance(src.PercentTolerance)
    dst.set_num_errors(src.NumberOfErrors)
    dst.set_computed_property_id(src.ComputedPropertyID)
    dst.set_expected_property_id(src.ExpectedPropertyID)

    dst.set_minimum_error(src.MinimumError)
    dst.set_maximum_error(src.MaximumError)
    dst.set_average_error(src.AverageError)
    dst.set_standard_deviation(src.StandardDeviation)
    serialize_function_from_bind(src.PercentToleranceVsNumErrors, dst.get_percent_tolerance_vs_num_errors())
    for d in src.Differences:
        dst.add_difference(d)
def serialize_test_error_statistics_to_bind(src: SETestErrorStatistics, dst: TestErrorStatisticsData):
    dst.PropertyName = src.get_property_name()
    dst.PercentTolerance = src.get_percent_tolerance()
    dst.NumberOfErrors = src.get_num_errors()
    dst.ComputedPropertyID = src.get_computed_property_id()
    dst.ExpectedPropertyID = src.get_expected_property_id()
    dst.MinimumError = src.get_minimum_error()
    dst.MaximumError = src.get_maximum_error()
    dst.AverageError = src.get_average_error()
    dst.StandardDeviation = src.get_standard_deviation()
    serialize_function_to_bind(src.get_percent_tolerance_vs_num_errors(), dst.PercentToleranceVsNumErrors)
    for d in src.get_differences():
        dst.Differences.add(d)
'''
