# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.plots import *
from pulse.cdm.bind.Plots_pb2 import *
from pulse.cdm.engine import eSerializationFormat
from google.protobuf import json_format

def serialize_plotter_list_from_file(filename: str, dst: [SEPlotter]):
    with open(filename) as f:
        string = f.read()
    serialize_plotter_list_from_string(string, dst, eSerializationFormat.JSON)
def serialize_plotter_list_from_string(string: str, dst: [SEPlotter], fmt: eSerializationFormat):
    src = PlotterListData()
    json_format.Parse(string, src)
    serialize_plotter_list_from_bind(src, dst)
def serialize_plotter_list_from_bind(src: PlotterListData, dst: [SEPlotter]):
    dst.clear()
    base_config = SEPlotConfig()
    if src.HasField("PlotConfig"):
        serialize_plot_config_from_bind(src.PlotConfig, base_config)
    else:
        base_config.set_defaults()

    for any_plotter_data in src.Plotter:
        if any_plotter_data.HasField("MultiHeaderSeriesPlotter"):
            plot = SEMultiHeaderSeriesPlotter()
            serialize_multi_header_series_plotter_from_bind(
                any_plotter_data.MultiHeaderSeriesPlotter,
                plot,
                base_config
            )
            dst.append(plot)
        elif any_plotter_data.HasField("ComparePlotter"):
            plot = SEComparePlotter()
            serialize_compare_plotter_from_bind(
                any_plotter_data.ComparePlotter,
                plot,
                base_config
            )
            dst.append(plot)
        elif any_plotter_data.HasField("MonitorPlotter"):
            plot = SEMonitorPlotter()
            serialize_monitor_plotter_from_bind(
                any_plotter_data.MonitorPlotter,
                plot,
                base_config
            )
            dst.append(plot)
        else:
            raise Exception("Invalid plotter in serialize_plot_list_from_bind")

def serialize_image_properties_to_bind(src: SEImageProperties, dst: ImagePropertyData):
    dst.DimensionMode = src.get_dimension_mode().value
    dst.FileFormat = src.get_file_format()
    dst.Height_inch = src.get_height_inch()
    dst.Width_inch = src.get_width_inch()
    dst.DPI = src.get_dpi()
def serialize_image_properties_from_bind(src: ImagePropertyData, dst: SEImageProperties):
    if src.HasField("FileFormat"):
        dst.set_file_format(src.FileFormat)
    if src.HasField("Height_inch"):
        dst.set_height_inch(src.Height_inch)
    if src.HasField("Width_inch"):
        dst.set_width_inch(src.Width_inch)
    if src.HasField("DPI"):
        dst.set_dpi(src.DPI)
    # Set dimension mode after height and width so unbound can dominate if needed
    if src.HasField("DimensionMode"):
        dst.set_dimension_mode(eDimensionMode(src.DimensionMode))

def serialize_bounds_to_bind(src: SEBounds, dst: BoundsData):
    if src.has_lower_bound():
        dst.LowerBound = src.get_lower_bound()
    if src.has_upper_bound():
        dst.UpperBound = src.get_upper_bound()
def serialize_bounds_from_bind(src: ImagePropertyData, dst: SEBounds):
    if src.HasField("LowerBound"):
        dst.set_lower_bound(src.LowerBound)
    if src.HasField("UpperBound"):
        dst.set_upper_bound(src.UpperBound)

def serialize_plot_config_to_bind(src: SEPlotConfig, dst: PlotConfigData):
    if src.has_disabled_setting():
        dst.Disabled = src.get_disabled()
    if src.has_fill_area_setting():
        dst.FillArea = src.get_fill_area()
    if src.has_font_size():
        dst.FontSize = src.get_font_size()
    if src.has_gridlines_setting():
        dst.Gridlines = src.get_gridlines()
    if src.has_image_properties():
        dst.ImageProperties = src.get_image_properties()
    if src.has_legend_font_size():
        dst.LegendFontSize = src.get_legend_font_size()
    if src.has_legend_mode():
        dst.LegendMode = src.get_legend_mode().value
    if src.has_log_axis_setting():
        dst.LogAxis = src.get_log_axis()
    if src.has_allow_actions_with():
        for a in src.get_allow_actions_with():
            allowData = dst.AllowActionsWith.add()
            allowData = a
    if src.has_allow_events_with():
        for a in src.get_allow_events_with():
            allowData = dst.AllowEventsWith.add()
            allowData = a
    if src.has_omit_actions_with():
        for o in src.get_omit_actions_with():
            omitData = dst.OmitActionsWith.add()
            omitData = o
    if src.has_omit_events_with():
        for o in src.get_omit_events_with():
            omitData = dst.OmitEventsWith.add()
            omitData = o
    if src.has_output_path_override():
        dst.OutputPathOverride = src.get_output_path_override()
    if src.has_percent_of_baseline_mode():
        dst.PercentOfBaselineMode = src.get_percent_of_baseline_mode().value
    if src.has_plot_actions_setting():
        dst.PlotActions = src.get_plot_actions()
    if src.has_plot_events_setting():
        dst.PlotEvents = src.get_plot_events()
    if src.has_sci_limits():
        limits = src.get_sci_limits()
        dst.SciLimits.M = limits[0]
        dst.SciLimits.N = limits[1]
    if src.has_tick_style():
        dst.TickStyle = src.get_tick_style().value
    if src.has_y_bounds_mode():
        dst.YBoundsMode = src.get_y_bounds_mode().value
def serialize_plot_config_from_bind(
    src: PlotConfigData,
    dst: SEPlotConfig,
    base_config: SEPlotConfig = None
):
    if base_config:
        dst.merge_configs(base_config)
    else:
        dst.set_defaults()

    if src.HasField("Disabled"):
        dst.set_disabled(src.Disabled)
    if src.HasField("FillArea"):
        dst.set_fill_area(src.FillArea)
    if src.HasField("FontSize"):
        dst.set_font_size(src.FontSize)
    if src.HasField("Gridlines"):
        dst.set_gridlines(src.Gridlines)
    if src.HasField("ImageProperties"):
        serialize_image_properties_from_bind(src.ImageProperties, dst.get_image_properties())
    if src.HasField("LegendFontSize"):
        dst.set_legend_font_size(src.LegendFontSize)
    if src.HasField("LegendMode"):
        dst.set_legend_mode(eLegendMode(src.LegendMode))
    if src.HasField("LogAxis"):
        dst.set_log_axis(src.LogAxis)
    for allowData in src.AllowActionsWith:
        dst.add_allow_actions_with(allowData)
    for allowData in src.AllowEventsWith:
        dst.add_allow_events_with(allowData)
    for omitData in src.OmitActionsWith:
        dst.add_omit_actions_with(omitData)
    for omitData in src.OmitEventsWith:
        dst.add_omit_events_with(omitData)
    if src.HasField("OutputPathOverride"):
        dst.set_output_path_override(src.OutputPathOverride)
    if src.HasField("PercentOfBaselineMode"):
        dst.set_percent_of_baseline_mode(ePercentageOfBaselineMode(src.PercentOfBaselineMode))
    if src.HasField("PlotActions"):
        dst.set_plot_actions(src.PlotActions)
    if src.HasField("PlotEvents"):
        dst.set_plot_events(src.PlotEvents)
    if src.HasField("SciLimits"):
        dst.set_sci_limits((src.SciLimits.M, src.SciLimits.N))
    if src.HasField("TickStyle"):
        dst.set_tick_style(eTickStyle(src.TickStyle))
    if src.HasField("YBoundsMode"):
        dst.set_y_bounds_mode(eYBoundsMode(src.YBoundsMode))


def serialize_plot_source_to_bind(src: SEPlotSource, dst: PlotSourceData):
    if src.has_csv_data():
        dst.CSVData = src.get_csv_data()
    if src.has_line_format():
        dst.LineFormat = src.get_line_format()
    if src.has_line_width():
        dst.LineWidth = src.get_line_width()
    if src.has_label():
        dst.Label = src.get_label()
    if src.has_start_row():
        dst.StartRow = src.get_start_row()
    if src.has_end_row():
        dst.EndRow = src.get_end_row()
    if src.has_row_skip():
        dst.RowSkip = src.get_row_skip()
    if src.has_log_file():
        dst.LogFile = src.get_log_file()
def serialize_plot_source_from_bind(src: PlotSourceData, dst: SEPlotSource):
    dst.set_csv_data(src.CSVData)
    if src.HasField("LineFormat"):
        dst.set_line_format(src.LineFormat)
    if src.HasField("LineWidth"):
        dst.set_line_width(src.LineWidth)
    if src.HasField("Label"):
        dst.set_label(src.Label)
    if src.HasField("StartRow"):
        dst.set_start_row(src.StartRow)
    if src.HasField("EndRow"):
        dst.set_end_row(src.EndRow)
    if src.HasField("RowSkip"):
        dst.set_row_skip(src.RowSkip)
    if src.HasField("LogFile"):
        dst.set_log_file(src.LogFile)


def serialize_series_to_bind(src: SESeries, dst: MultiHeaderSeriesPlotterData.SeriesData):
    if src.has_plot_config():
        serialize_plot_config_to_bind(src.get_plot_config(), dst.PlotConfig)
    if src.has_output_filename():
        dst.OutputFilename = src.get_output_filename()
    if src.has_title():
        dst.Title = src.get_title()
    if src.has_x_header():
        dst.XHeader = src.get_x_header()
    if src.has_x_label():
        dst.XLabel = src.get_x_label()
    if src.has_x_bounds():
        serialize_bounds_to_bind(src.get_x_bounds(), dst.XBounds)
    if src.has_x2_header():
        dst.X2Header = src.get_x2_header()
    for y in src.get_y_headers():
        headerData = dst.YHeader.add()
        headerData = y
    if src.has_y_label():
        dst.YLabel = src.get_y_label()
    if src.has_y_bounds():
        serialize_bounds_to_bind(src.get_y_bounds(), dst.YBounds)
    for y2 in src.get_y2_headers():
        headerData = dst.Y2Header.add()
        headerData = y2
    if src.has_y2_label():
        dst.Y2Label = src.get_y2_label()
    if src.has_y2_bounds():
        serialize_bounds_to_bind(src.get_y2_bounds(), dst.Y2Bounds)
def serialize_series_from_bind(
    src: MultiHeaderSeriesPlotterData.SeriesData,
    dst: SESeries,
    base_config: SEPlotConfig
):
    if src.HasField("PlotConfig"):
        serialize_plot_config_from_bind(src.PlotConfig, dst.get_plot_config(), base_config)
    else:
        dst.set_plot_config(base_config)
    if src.HasField("OutputFilename"):
        dst.set_output_filename(src.OutputFilename)
    if src.HasField("Title"):
        dst.set_title(src.Title)
    if src.HasField("XHeader"):
        dst.set_x_header(src.XHeader)
    if src.HasField("XLabel"):
        dst.set_x_label(src.XLabel)
    if src.HasField("XBounds"):
        bounds = SEBounds()
        serialize_bounds_from_bind(src.XBounds, bounds)
        dst.set_x_bounds(bounds)
    if src.HasField("X2Header"):
        dst.set_x2_header(src.X2Header)
    for headerData in src.YHeader:
        dst.add_y_header(headerData)
    if src.HasField("YLabel"):
        dst.set_y_label(src.YLabel)
    if src.HasField("YBounds"):
        bounds = SEBounds()
        serialize_bounds_from_bind(src.YBounds, bounds)
        dst.set_y_bounds(bounds)
    for headerData in src.Y2Header:
        dst.add_y2_header(headerData)
    if src.HasField("Y2Label"):
        dst.set_y2_label(src.Y2Label)
    if src.HasField("Y2Bounds"):
        bounds = SEBounds()
        serialize_bounds_from_bind(src.Y2Bounds, bounds)
        dst.set_y2_bounds(bounds)


def serialize_multi_header_series_plotter_to_bind(src: SEMultiHeaderSeriesPlotter, dst: MultiHeaderSeriesPlotterData):
    if src.has_plot_config():
        serialize_plot_config_to_bind(src.get_plot_config(), dst.PlotConfig)
    for s in src.get_plot_sources():
        sourceData = dst.PlotSource.add()
        serialize_plot_source_to_bind(s, sourceData)
    for s in src.get_series():
        seriesData = dst.Series.add()
        serialize_series_to_bind(s, seriesData)
    if src.has_valiation_source():
        serialize_plot_source_to_bind(src.get_validation_source(), dst.ValidationSource)
def serialize_multi_header_series_plotter_from_bind(
    src: MultiHeaderSeriesPlotterData,
    dst: SEMultiHeaderSeriesPlotter,
    base_config: SEPlotConfig
):
    if src.HasField("PlotConfig"):
        serialize_plot_config_from_bind(src.PlotConfig, dst.get_plot_config(), base_config)
    else:
        dst.set_plot_config(base_config)

    for sourceData in src.PlotSource:
        s = SEPlotSource()
        serialize_plot_source_from_bind(sourceData, s)
        dst.add_plot_source(s)
    for seriesData in src.Series:
        s = SESeries()
        serialize_series_from_bind(seriesData, s, dst.get_plot_config())
        dst.add_series(s)
    if src.HasField("ValidationSource"):
        val_source = SEPlotSource()
        serialize_plot_source_from_bind(src.ValidationSource, val_source)
        dst.set_validation_source(val_source)


def serialize_compare_plotter_to_bind(src: SEComparePlotter, dst: ComparePlotterData):
    if src.has_plot_config():
        serialize_plot_config_to_bind(src.get_plot_config(), dst.PlotConfig)
    if src.has_computed_source():
        serialize_plot_source_to_bind(src.get_computed_source(), dst.ComputedSource)
    if src.has_expected_source():
        serialize_plot_source_to_bind(src.get_expected_source(), dst.ExpectedSource)
    for f in src.get_failures():
        failureData = dst.Failures.append(f)
    for h, r in src.get_rms_values().items():
        dst.RMSValues[h] = r
    dst.PlotType = src.get_plot_type().value
def serialize_compare_plotter_from_bind(
    src: ComparePlotterData,
    dst: SEComparePlotter,
    base_config: SEPlotConfig
):
    if src.HasField("PlotConfig"):
        serialize_plot_config_from_bind(src.PlotConfig, dst.get_plot_config(), base_config)
    else:
        dst.set_plot_config(base_config)

    computed_source = SEPlotSource()
    serialize_plot_source_from_bind(src.ComputedSource, computed_source)
    dst.set_computed_source(computed_source)

    expected_source = SEPlotSource()
    serialize_plot_source_from_bind(src.ExpectedSource, expected_source)
    dst.set_expected_source(expected_source)

    for failureData in src.Failures:
        dst.add_failure(failureData)

    for header in src.RMSValues:
        dst.add_rms_value(header, src.RMSValues[header])

    dst.set_plot_type(ePlotType(src.PlotType))


def serialize_monitor_plotter_to_bind(src: SEMonitorPlotter, dst: MonitorPlotterData):
    if src.has_plot_config():
        serialize_plot_config_to_bind(src.get_plot_config(), dst.PlotConfig)

    if src.has_plot_source():
        serialize_plot_source_to_bind(src.get_plot_source(), dst.PlotSource)

    dst.Vitals = src.get_plot_vitals()
    dst.Ventilator = src.get_plot_ventilator()

    if src.has_times_s():
        dst.Times_s.extend(src.get_times_s())
    elif src.has_data_requested_file():
        dst.DataRequestedFile = src.get_data_requested_file()

    if src.has_output_prefix():
        dst.OutputPrefix = src.get_output_prefix()

def serialize_monitor_plotter_from_bind(
    src: MonitorPlotterData,
    dst: SEMonitorPlotter,
    base_config: SEPlotConfig
):
    if src.HasField("PlotConfig"):
        serialize_plot_config_from_bind(src.PlotConfig, dst.get_plot_config(), base_config)
    else:
        dst.set_plot_config(base_config)

    plot_source = SEPlotSource()
    serialize_plot_source_from_bind(src.PlotSource, plot_source)
    dst.set_plot_source(plot_source)

    dst.set_plot_vitals(src.Vitals)
    dst.set_plot_ventilator(src.Ventilator)

    if src.HasField("Times"):
       dst.set_times_s(src.Times.Times_s)
    elif src.HasField("DataRequestedFile"):
        dst.set_data_requested_file(src.DataRequestedFile)

    if src.HasField("OutputPrefix"):
        dst.set_output_prefix(src.OutputPrefix)
