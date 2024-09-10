# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from enum import Enum
from pathlib import Path
from copy import deepcopy
from operator import attrgetter
from typing import Dict, List, Optional, Set, Tuple, Union
import logging
import pandas as pd

from pulse.cdm.scalars import SEScalar, SEScalarLength
from pulse.cdm.utils.file_utils import get_dir_from_run_config
from pulse.cdm.utils.csv_utils import read_csv_into_df
from pulse.cdm.utils.logger import eActionEventCategory, LogActionEvent, parse_actions, parse_events


_pulse_logger = logging.getLogger('pulse')


class eDimensionMode(Enum):
    Default = 0
    Square = 1
    Legend = 2
    Unbound = 3
class SEImageProperties():
    __slots__ = ["_dimension_mode", "_dim_dict", "_file_format", "_height_inch", "_width_inch", "_dpi"]

    def __init__(self, dimension_mode: eDimensionMode=eDimensionMode.Default, file_format: str=".jpg",
        height_inch: Optional[float]=None, width_inch: Optional[float]=None, dpi: int=100
    ):
        self._dimension_mode = dimension_mode
        self._file_format = file_format
        self._height_inch = height_inch
        self._width_inch = width_inch
        self._dpi = dpi

        self._dim_dict = {
            eDimensionMode.Default:  { "h": 8, "w": 16 },
            eDimensionMode.Square:   { "h": 8, "w": 8  },
            eDimensionMode.Legend:   { "h": 2, "w": 32 },
            eDimensionMode.Unbound:  { "h": None, "w": None }
        }

    def get_dimension_mode(self) -> eDimensionMode:
        return self._dimension_mode
    def set_dimension_mode(self, mode: eDimensionMode) -> None:
        self._dimension_mode = mode
        if mode == eDimensionMode.Unbound:
            self._height_inch = None
            self._width_inch = None

    def get_file_format(self) -> str:
        return self._file_format
    def set_file_format(self, fmt: str) -> None:
        self._file_format = fmt

    def get_height_inch(self) -> Union[float, None]:
        if self._height_inch is not None:
            return self._height_inch
        elif self._dimension_mode in self._dim_dict:
            return self._dim_dict[self._dimension_mode]["h"]
        else:
            raise ValueError(f"Unknown dimension mode {self._dimension_mode}")
    def set_height_inch(self, h: float) -> None:
        self._height_inch = h

    def get_width_inch(self) -> Union[float, None]:
        if self._width_inch is not None:
            return self._width_inch
        elif self._dimension_mode in self._dim_dict:
            return self._dim_dict[self._dimension_mode]["w"]
        else:
            raise ValueError(f"Unknown dimension mode {self._dimension_mode}")
    def set_width_inch(self, w: float) -> None:
        self._width_inch = w

    def get_dpi(self) -> float:
        return self._dpi
    def set_dpi(self, dpi: float) -> None:
        self._dpi = dpi


class SEBounds():
    __slots__ = ["_lower_bound", "_upper_bound"]

    def __init__(self, lower_bound: Optional[float]=None, upper_bound: Optional[float]=None):
        self._lower_bound = lower_bound
        self._upper_bound = upper_bound

    def get_lower_bound(self) -> Union[float, None]:
        return self._lower_bound
    def set_lower_bound(self, bound: float) -> None:
        self._lower_bound = bound
    def has_lower_bound(self) -> bool:
        return self._lower_bound is not None
    def invalidate_lower_bound(self) -> None:
        self._lower_bound = None

    def get_upper_bound(self) -> Union[float, None]:
        return self._upper_bound
    def set_upper_bound(self, bound: float) -> None:
        self._upper_bound = bound
    def has_upper_bound(self) -> bool:
        return self._upper_bound is not None
    def invalidate_upper_bound(self) -> None:
        self._upper_bound = None

class eLegendMode(Enum):
    AllLegends = 0
    NoLegends = 1
    HideActionEventLegend = 2
    OnlyActionEventLegend = 3
class ePercentageOfBaselineMode(Enum):
    Off = 0
    All = 1
    OnlyX = 2
    OnlyY = 3
class eTickStyle(Enum):
    Scientific = 0
    Plain = 1
class eYBoundsMode(Enum):
    ZeroMax = 0
    MinMax = 1
class SEPlotConfig():
    __slots__ = [ "_disabled", "_fill_area", "_font_size", "_gridlines", "_image_properties",
                  "_legend_font_size", "_legend_mode", "_log_axis", "_allow_actions_with", "_allow_events_with",
                  "_omit_actions_with", "_omit_events_with", "_output_filename", "_output_path_override",
                  "_percent_of_baseline_mode", "_plot_actions", "_plot_events",
                  "_sci_limits", "_tick_style", "_title", "_x_label", "_x_bounds", "_y_label",
                  "_y_bounds", "_y2_label", "_y2_bounds", "_y_bounds_mode"]

    def __init__(self, set_defaults: bool=False):
        self.clear()
        if set_defaults:
            self.set_defaults()

    def __repr__(self) -> str:
        return f'SEPlotConfig({self._disabled}, {self._fill_area}, {self._font_size}, {self._gridlines}, ' \
            f'{self._image_properties}, {self._legend_font_size}, {self._legend_mode}, {self._log_axis}, ' \
            f'{self._allow_actions_with}, {self._allow_events_with}, {self._omit_actions_with}, ' \
            f'{self._omit_events_with}, {self._output_path_override}, {self._percent_of_baseline_mode}, ' \
            f'{self._plot_actions}, {self._plot_events}, {self._sci_limits}, {self._tick_style}, ' \
            f'{self._y_bounds_mode})'

    def __str__(self) -> str:
        return f'SEPlotConfig:\n\tDisabled: {self._disabled}\n\tFill Area: {self._fill_area}\n\tFont Size: ' \
            f'{self._font_size}\n\tGridlines: {self._gridlines}\n\tImage Properties: {self._image_properties}\n\t' \
            f'Legend Font Size: {self._legend_font_size}\n\tLegend Mode: {self._legend_mode}\n\tLog Axis: '\
            f'{self._log_axis}\n\tAllow Actions With: {self._allow_actions_with}\n\tAllow Events With: ' \
            f'{self._allow_events_with}\n\tOmit Actions With: {self._omit_actions_with}\n\tOmit Events With: ' \
            f'{self._omit_events_with}\n\tOutput Path Override: {self._output_path_override}\n\t' \
            f'Percent of Baseline Mode: {self._percent_of_baseline_mode}\n\tPlot Actions: {self._plot_actions}\n\t' \
            f'Plot Events: {self._plot_events}\n\tSci Limits: {self._sci_limits}\n\tTick Style: {self._tick_style}\n\t' \
            f'Y Bounds Mode: {self._y_bounds_mode}'

    def clear(self) -> None:
        self._disabled = None
        self._fill_area = None
        self._font_size = None
        self._gridlines = None
        self._image_properties = None
        self._legend_font_size = None
        self._legend_mode = None
        self._log_axis = None
        self._allow_actions_with = None
        self._allow_events_with = None
        self._omit_actions_with = None
        self._omit_events_with = None
        self._output_path_override = None
        self._percent_of_baseline_mode = None
        self._plot_actions = None
        self._plot_events = None
        self._sci_limits = None
        self._tick_style = None
        self._y_bounds_mode = None

        # Internal
        self._output_filename = None
        self._title = None
        self._x_label = None
        self._x_bounds = None
        self._y_label = None
        self._y_bounds = None
        self._y2_label = None
        self._y2_bounds = None

    def set_defaults(self) -> None:
        if self._disabled is None:
            self._disabled = False
        if self._fill_area is None:
            self._fill_area = False
        if self._font_size is None:
            self._font_size = 16
        if self._gridlines is None:
            self._gridlines = False
        if self._image_properties is None:
            self._image_properties = SEImageProperties()
        if self._legend_font_size is None:
            self._legend_font_size = 14
        if self._legend_mode is None:
            self._legend_mode = eLegendMode.AllLegends
        if self._log_axis is None:
            self._log_axis = False
        if self._allow_actions_with is None:
            self._allow_actions_with = list()
        if self._allow_events_with is None:
            self._allow_events_with = list()
        if self._omit_actions_with is None:
            self._omit_actions_with = list()
        if self._omit_events_with is None:
            self._omit_events_with = list()
        if self._output_path_override is None:
            self._output_path_override = Path("./verification/Plots/")
        if self._percent_of_baseline_mode is None:
            self._percent_of_baseline_mode = ePercentageOfBaselineMode.Off
        if self._plot_actions is None:
            self._plot_actions = False
        if self._plot_events is None:
            self._plot_events = False
        if self._tick_style is None:
            self._tick_style = eTickStyle.Scientific
        if self._y_bounds_mode is None:
            self._y_bounds_mode = eYBoundsMode.ZeroMax

        # Internal
        if self._x_bounds is None:
            self._x_bounds = SEBounds()
        if self._y_bounds is None:
            self._y_bounds = SEBounds()
        if self._y2_bounds is None:
            self._y2_bounds = SEBounds()

    def set_values(self, disabled: Optional[bool]=None, fill_area: Optional[bool]=None, font_size: Optional[int]=None,
        gridlines: Optional[bool]=None, image_properties: Optional[SEImageProperties]=None, legend_font_size: Optional[int]=None,
        legend_mode: Optional[eLegendMode]=None, log_axis: Optional[bool]=None, allow_actions_with: Optional[List[str]]=None,
        allow_events_with: Optional[List[str]]=None, omit_actions_with: Optional[List[str]]=None,
        omit_events_with: Optional[List[str]]=None, output_path_override: Optional[Union[str, Path]]=None,
        percent_of_baseline_mode: Optional[ePercentageOfBaselineMode]=None, plot_actions: Optional[bool]=None,
        plot_events: Optional[bool]=None, sci_limits: Optional[Tuple[int, int]]=None, tick_style: Optional[eTickStyle]=None,
        y_bounds_mode: Optional[eYBoundsMode]=None
    )  -> None:
        if disabled is not None:
            self._disabled = disabled
        if fill_area is not None:
            self._fill_area = fill_area
        if font_size is not None:
            self._font_size = font_size
        if gridlines is not None:
            self._gridlines = gridlines
        if image_properties is not None:
            self.set_image_properties(image_properties)
        if legend_font_size is not None:
            self._legend_font_size = legend_font_size
        if legend_mode is not None:
            self._legend_mode = legend_mode
        if log_axis is not None:
            self._log_axis = log_axis
        if allow_actions_with is not None:
            self._allow_actions_with = list(allow_actions_with)
        if allow_events_with is not None:
            self._allow_events_with = list(allow_events_with)
        if omit_actions_with is not None:
            self._omit_actions_with = list(omit_actions_with)
        if omit_events_with is not None:
            self._omit_events_with = list(omit_events_with)
        if output_path_override is not None:
            self._output_path_override = Path(output_path_override)
        if percent_of_baseline_mode is not None:
            self._percent_of_baseline_mode = percent_of_baseline_mode
        if plot_actions is not None:
            self._plot_actions = plot_actions
        if plot_events is not None:
            self._plot_events = plot_events
        if sci_limits is not None:
            self._sci_limits = sci_limits
        if tick_style is not None:
            self._tick_style = tick_style
        if y_bounds_mode is not None:
            self._y_bounds_mode = y_bounds_mode

    def merge_configs(self, src):
        src = deepcopy(src)
        if src._disabled is not None:
            self._disabled = src._disabled
        if src._fill_area is not None:
            self._fill_area = src._fill_area
        if src._font_size is not None:
            self._font_size = src._font_size
        if src._gridlines is not None:
            self._gridlines = src._gridlines
        if src._image_properties is not None:
            self._image_properties = src._image_properties
        if src._legend_font_size is not None:
            self._legend_font_size = src._legend_font_size
        if src._legend_mode is not None:
            self._legend_mode = src._legend_mode
        if src._log_axis is not None:
            self._log_axis = src._log_axis
        if src._allow_actions_with is not None:
            self._allow_actions_with = src._allow_actions_with
        if src._allow_events_with is not None:
            self._allow_events_with = src._allow_events_with
        if src._omit_actions_with is not None:
            self._omit_actions_with = src._omit_actions_with
        if src._omit_events_with is not None:
            self._omit_events_with = src._omit_events_with
        if src._output_path_override is not None:
            self._output_path_override = src._output_path_override
        if src._percent_of_baseline_mode is not None:
            self._percent_of_baseline_mode = src._percent_of_baseline_mode
        if src._plot_actions is not None:
            self._plot_actions = src._plot_actions
        if src._plot_events is not None:
            self._plot_events = src._plot_events
        if src._sci_limits is not None:
            self._sci_limits = src._sci_limits
        if src._tick_style is not None:
            self._tick_style = src._tick_style
        if src._y_bounds_mode is not None:
            self._y_bounds_mode = src._y_bounds_mode

        # Internal
        if src._output_filename is not None:
            self._output_filename = src._output_filename
        if src._title is not None:
            self._title = src._title
        if src._x_label is not None:
            self._x_label = src._x_label
        if src._x_bounds is not None:
            self._x_bounds = src._x_bounds
        if src._y_label is not None:
            self._y_label = src._y_label
        if src._y_bounds is not None:
            self._y_bounds = src._y_bounds
        if src._y2_label is not None:
            self._y2_label = src._y2_label
        if src._y2_bounds is not None:
            self._y2_bounds = src._y2_bounds

    def get_disabled(self) -> Union[bool, None]:
        return self._disabled
    def set_disabled(self, disabled: bool) -> None:
        self._disabled = disabled
    def has_disabled_setting(self) -> bool:
        return self._disabled is not None
    def invalidate_disabled_setting(self) -> None:
        self._disabled = None

    def get_fill_area(self) -> Union[bool, None]:
        return self._fill_area
    def set_fill_area(self, fill: bool) -> None:
        self._fill_area = fill
    def has_fill_area_setting(self) -> bool:
        return self._fill_area is not None
    def invalidate_fill_area_setting(self) -> None:
        self._fill_area = None

    def get_font_size(self) -> Union[float, None]:
        return self._font_size
    def set_font_size(self, font_size: float) -> None:
        self._font_size = font_size
    def has_font_size(self) -> bool:
        return self._font_size is not None
    def invalidate_font_size(self) -> None:
        self._font_size = None

    def get_gridlines(self) -> Union[bool, None]:
        return self._gridlines
    def set_gridlines(self, gridlines: bool) -> None:
        self._gridlines = gridlines
    def has_gridlines_setting(self) -> bool:
        return self._gridlines is not None
    def invalidate_gridlines_setting(self) -> None:
        self._gridlines = None

    def get_image_properties(self) -> Union[SEImageProperties, None]:
        return self._image_properties
    def set_image_properties(self, image_props: SEImageProperties) -> None:
        self._image_properties = deepcopy(image_props)
    def has_image_properties(self) -> bool:
        return self._image_properties is not None
    def invalidate_image_properties(self) -> None:
        self._image_properties = None

    def get_legend_font_size(self) -> Union[float, None]:
        return self._legend_font_size
    def set_legend_font_size(self, font_size: float) -> None:
        self._legend_font_size = font_size
    def has_legend_font_size(self) -> bool:
        return self._legend_font_size is not None
    def invalidate_legend_font_size(self) -> None:
        self._legend_font_size = None

    def get_legend_mode(self) -> Union[eLegendMode, None]:
        return self._legend_mode
    def set_legend_mode(self, mode: eLegendMode) -> None:
        self._legend_mode = mode
    def has_legend_mode(self) -> bool:
        return self._legend_mode is not None
    def invalidate_legend_mode(self) -> None:
        self._legend_mode = None

    def get_log_axis(self) -> Union[bool, None]:
        return self._log_axis
    def set_log_axis(self, log_axis: bool) -> None:
        self._log_axis = log_axis
    def has_log_axis_setting(self) -> bool:
        return self._log_axis is not None
    def invalidate_log_axis_setting(self) -> None:
        self._log_axis = None

    def get_allow_actions_with(self) -> List[str]:
        return self._allow_actions_with
    def add_allow_actions_with(self, string: str) -> None:
        if self._allow_actions_with is None:
            self._allow_actions_with = list()
        self._allow_actions_with.append(string)
    def has_allow_actions_with(self) -> bool:
        return self._allow_actions_with is not None and len(self._allow_actions_with) > 0
    def invalidate_allow_actions_with(self) -> None:
        self._allow_actions_with = list()

    def get_allow_events_with(self) -> List[str]:
        return self._allow_events_with
    def add_allow_events_with(self, string: str) -> None:
        if self._allow_events_with is None:
            self._allow_events_with = list()
        self._allow_events_with.append(string)
    def has_allow_events_with(self) -> bool:
        return self._allow_events_with is not None and len(self._allow_events_with) > 0
    def invalidate_allow_events_with(self) -> None:
        self._allow_events_with = list()

    def get_omit_actions_with(self) -> List[str]:
        return self._omit_actions_with
    def add_omit_actions_with(self, string: str) -> None:
        if self._omit_actions_with is None:
            self._omit_actions_with = list()
        self._omit_actions_with.append(string)
    def has_omit_actions_with(self) -> bool:
        return self._omit_actions_with is not None and len(self._omit_actions_with) > 0
    def invalidate_omit_actions_with(self) -> None:
        self._omit_actions_with = list()

    def get_omit_events_with(self) -> List[str]:
        return self._omit_events_with
    def add_omit_events_with(self, string: str) -> None:
        if self._omit_events_with is None:
            self._omit_events_with = []
        self._omit_events_with.append(string)
    def has_omit_events_with(self) -> bool:
        return self._omit_events_with is not None and len(self._omit_events_with) > 0
    def invalidate_omit_events_with(self) -> None:
        self._omit_events_with = list()

    def get_output_filename(self) -> Union[str, None]:
        return self._output_filename
    def set_output_filename(self, output_filename: str) -> None:
        self._output_filename = output_filename
    def has_output_filename(self) -> bool:
        return self._output_filename is not None and len(self._output_filename) > 0
    def invalidate_output_filename(self) -> None:
        self._output_filename = None

    def get_output_path_override(self) -> Union[Path, None]:
        return self._output_path_override
    def set_output_path_override(self, output_path: Union[str, Path]) -> None:
        self._output_path_override = Path(output_path)
    def has_output_path_override(self) -> bool:
        return self._output_path_override is not None
    def invalidate_output_path_override(self) -> None:
        self._output_path_override = None

    def get_percent_of_baseline_mode(self) -> Union[ePercentageOfBaselineMode, None]:
        return self._percent_of_baseline_mode
    def set_percent_of_baseline_mode(self, mode: ePercentageOfBaselineMode) -> None:
        self._percent_of_baseline_mode = mode
    def has_percent_of_baseline_mode(self) -> bool:
        return self._percent_of_baseline_mode is not None
    def invalidate_percent_of_baseline_mode(self) -> None:
        self._percent_of_baseline_mode = None

    def get_plot_actions(self) -> Union[bool, None]:
        return self._plot_actions
    def set_plot_actions(self, plot_actions: bool) -> None:
        self._plot_actions = plot_actions
    def has_plot_actions_setting(self) -> bool:
        return self._plot_actions is not None
    def invalidate_plot_actions_setting(self) -> None:
        self._plot_actions = None

    def get_plot_events(self) -> Union[bool, None]:
        return self._plot_events
    def set_plot_events(self, plot_events: bool) -> None:
        self._plot_events = plot_events
    def has_plot_events_setting(self) -> bool:
        return self._plot_events is not None
    def invalidate_plot_events_setting(self) -> None:
        self._plot_events = None

    def get_sci_limits(self) -> Union[Tuple[int, int], None]:
        return self._sci_limits
    def set_sci_limits(self, limits: Tuple[int, int]) -> None:
        self._sci_limits = limits
    def has_sci_limits(self) -> bool:
        return self._sci_limits is not None
    def invalidate_sci_limits(self) -> None:
        self._sci_limits = None

    def get_tick_style(self) -> Union[eTickStyle, None]:
        return self._tick_style
    def set_tick_style(self, style: eTickStyle) -> None:
        self._tick_style = style
    def has_tick_style(self) -> bool:
        return self._tick_style is not None
    def invalidate_tick_style(self) -> None:
        self._tick_style = None

    def get_y_bounds_mode(self) -> Union[eYBoundsMode, None]:
        return self._y_bounds_mode
    def set_y_bounds_mode(self, y_bounds_mode: eYBoundsMode) -> None:
        self._y_bounds_mode = y_bounds_mode
    def has_y_bounds_mode(self) -> bool:
        return self._y_bounds_mode is not None
    def invalidate_y_bounds_mode(self) -> None:
        self._y_bounds_mode = None

    def get_title(self) -> Union[str, None]:
        return self._title
    def set_title(self, title: str) -> None:
        self._title = title
    def has_title(self) -> bool:
        return self._title is not None
    def invalidate_title(self) -> None:
        self._title = None

    def get_x_label(self) -> Union[str, None]:
        return self._x_label
    def set_x_label(self, label: str) -> None:
        self._x_label = label
    def has_x_label(self) -> bool:
        return self._x_label is not None
    def invalidate_x_label(self) -> None:
        self._x_label = None

    def get_x_bounds(self) -> Union[SEBounds, None]:
        return self._x_bounds
    def set_x_bounds(self, bounds: SEBounds) -> None:
        self._x_bounds = deepcopy(bounds)
    def has_x_bounds(self) -> bool:
        return self._x_bounds is not None
    def invalidate_x_bounds(self) -> None:
        self._x_bounds = None

    def get_y_label(self) -> Union[str, None]:
        return self._y_label
    def set_y_label(self, label: str) -> None:
        self._y_label = label
    def has_y_label(self) -> bool:
        return self._y_label is not None
    def invalidate_y_label(self) -> None:
        self._y_label = None

    def get_y_bounds(self) -> Union[SEBounds, None]:
        return self._y_bounds
    def set_y_bounds(self, bounds: SEBounds) -> None:
        self._y_bounds = deepcopy(bounds)
    def has_y_bounds(self) -> bool:
        return self._y_bounds is not None
    def invalidate_y_bounds(self) -> None:
        self._y_bounds = None

    def get_y2_label(self) -> Union[str, None]:
        return self._y2_label
    def set_y2_label(self, label: str) -> None:
        self._y2_label = label
    def has_y2_label(self) -> bool:
        return self._y2_label is not None
    def invalidate_y2_label(self) -> None:
        self._y2_label = None

    def get_y2_bounds(self) -> Union[SEBounds, None]:
        return self._y2_bounds
    def set_y2_bounds(self, bounds: SEBounds) -> None:
        self._y2_bounds = deepcopy(bounds)
    def has_y2_bounds(self) -> bool:
        return self._y2_bounds is not None
    def invalidate_y2_bounds(self) -> None:
        self._y2_bounds = None


class SEPlotSource():
    __slots__ = ["_csv_data", "_log_file", "_df", "_label", "_line_format",
                 "_line_width", "_start_row", "_end_row", "_row_skip", "_actions_events"]

    def __init__(self, csv_data: Optional[str]=None, log_file: Optional[str]=None, label: Optional[str]=None,
        line_format: Optional[str]=None, line_width: Optional[float]=None, start_row: Optional[int]=None,
        end_row: Optional[int]=None, row_skip: Optional[int]=None
    ):
        self._csv_data = csv_data
        self._log_file = Path(log_file) if log_file is not None else log_file
        self._label = label
        self._line_format = line_format if line_format is not None else ""
        self._line_width = line_width
        self._start_row = start_row
        self._end_row = end_row
        self._row_skip = row_skip

        self._actions_events = None
        self._df = pd.DataFrame()

    def get_csv_data(self) -> Union[str, None]:
        return self._csv_data
    def set_csv_data(self, csv_data: str) -> None:
        self._csv_data = csv_data
    def has_csv_data(self) -> bool:
        return self._csv_data is not None
    def invalidate_csv_data(self) -> None:
        self._csv_data = None
        self._df = None

    def get_log_file(self) -> Union[Path, None]:
        return self._log_file
    def set_log_file(self, log_file: str) -> None:
        self._log_file = Path(self._filepath_replacement(log_file))
    def has_log_file(self) -> bool:
        return self._log_file is not None
    def invalidate_log_file(self) -> None:
        self._log_file = None

    def parse_actions_events(self) -> bool:
        # Attempt to find log file if not given
        if not self._log_file:
            if self._csv_data:
                idx = self._csv_data.rfind("Results.csv")
                if idx == -1:
                    idx = self._csv_data.rfind(".csv")
                self._log_file = Path(self._csv_data[:idx] + ".log")

        # Ensure log file exists
        if not self._log_file.is_file():
            # Results files ending in Results don't usually match logs but double check
            idx = self._csv_data.rfind("Results.csv")
            if idx != -1:
                self._log_file = Path(self._csv_data[:idx] + "Results.log")

            if not self._log_file.is_file():
                _pulse_logger.error(f"Could not find corresponding log file: {self._csv_data}")
                return False
        self._actions_events = parse_actions(self._log_file)
        self._actions_events.extend(parse_events(self._log_file))

        self._actions_events = sorted(self._actions_events, key=attrgetter('time'))

        return True
    def get_actions_events(self, plot_actions: bool=True, plot_events: bool=True,
            allow_actions_with: Optional[List[str]]=None, allow_events_with: Optional[List[str]]=None,
            omit_actions_with: Optional[List[str]]=None, omit_events_with: Optional[List[str]]=None,
            count_limit: int=None
    ) -> List[LogActionEvent]:
        if allow_actions_with is None:
            allow_actions_with = list()
        if allow_events_with is None:
            allow_events_with = list()
        if omit_actions_with is None:
            omit_actions_with = list()
        if omit_events_with is None:
            omit_events_with = list()

        filtered = []
        ae_counts = {eActionEventCategory.ACTION: {}, eActionEventCategory.EVENT: {}}
        for ae in self._actions_events:
            if plot_actions and ae.category == eActionEventCategory.ACTION:
                if allow_actions_with:
                    keep = False
                    for o in allow_actions_with:
                        if o in ae.text:
                            keep = True
                            break
                else:
                    keep = True

                for o in omit_actions_with:
                    if o in ae.text:
                        keep = False
                        break

                if keep:
                    filtered.append(ae)
                    if ae.name not in ae_counts[ae.category]:
                        ae_counts[ae.category][ae.name] = 0
                    ae_counts[ae.category][ae.name] += 1
            elif plot_events and ae.category == eActionEventCategory.EVENT:
                if allow_events_with:
                    keep = False
                    for o in allow_events_with:
                        if o in ae.text:
                            keep = True
                            break
                else:
                    keep = True

                for o in omit_events_with:
                    if o in ae.text:
                        keep = False
                        break
                if keep:
                    filtered.append(ae)
                    if ae.name not in ae_counts[ae.category]:
                        ae_counts[ae.category][ae.name] = 0
                    ae_counts[ae.category][ae.name] += 1

        if count_limit is not None:
            filtered = [ae for ae in filtered if not ae_counts[ae.category][ae.name] > count_limit]

        return filtered
    def set_actions_events(self, actions_events: List[LogActionEvent]) -> None:
        self._actions_events = sorted(actions_events, key=attrgetter('time'))
    def has_actions_events(self) -> bool:
        return self._actions_events is not None
    def invalidate_log_file(self) -> None:
        self._actions_events = None

    def get_data_frame(self) -> pd.DataFrame:
        if self._df.empty:
            self._read_csv_into_df()
        return self._df.loc[self._start_row:self._end_row] if not self._df.empty else self._df
    def invalidate_data_frame(self) -> None:
        self._df = pd.DataFrame()

    def get_label(self) -> Union[str, None]:
        return self._label
    def set_label(self, label: str) -> None:
        self._label = label
    def has_label(self) -> bool:
        return self._label is not None
    def invalidate_label(self) -> None:
        self._label = None

    def get_line_format(self) -> str:
        return self._line_format
    def set_line_format(self, fmt: str) -> None:
        self._line_format = fmt
    def has_line_format(self) -> bool:
        return len(self._line_format) > 0
    def invalidate_line_format(self) -> None:
        self._line_format = ""

    def get_line_width(self) -> Union[float, None]:
        return self._line_width
    def set_line_width(self, width: float) -> None:
        self._line_width = width
    def has_line_width(self) -> bool:
        return self._line_width is not None
    def invalidate_line_width(self) -> None:
        self._line_width = None

    def get_start_row(self) -> Union[int, None]:
        return self._start_row
    def set_start_row(self, row: int) -> None:
        self._start_row = row
    def has_start_row(self) -> bool:
        return self._start_row is not None
    def invalidate_start_row(self) -> None:
        self._start_row = None

    def get_end_row(self) -> Union[int, None]:
        return self._end_row
    def set_end_row(self, row: int) -> None:
        self._end_row = row
    def has_end_row(self) -> bool:
        return self._end_row is not None
    def invalidate_end_row(self) -> None:
        self._end_row = None

    def get_row_skip(self) -> Union[int, None]:
        return self._row_skip
    def set_row_skip(self, rows: int) -> None:
        self._row_skip = rows
    def has_row_skip(self) -> None:
        return self._row_skip is not None
    def invalidate_row_skip(self) -> None:
        self._row_skip = None

    def _filepath_replacement(self, filepath: str) -> str:
        keys = ['$ROOT_DIR', '$DATA_DIR', '$SCENARIO_DIR','$VALIDATION_DIR', '$VERIFICATION_DIR']
        for key in keys:
            if key in filepath:
               filepath = filepath.replace(key, get_dir_from_run_config(key[1:]))
        return filepath
    def _read_csv_into_df(self) -> None:
        if not self.has_csv_data():
            _pulse_logger.error("No CSV data provided")
            return

        # Perform replacement if needed
        self._csv_data = self._filepath_replacement(self._csv_data)

        csv_file = Path(self._csv_data)
        if csv_file.is_file():
            kwargs = {}
            if self._row_skip:
                # Keep header and every _row_skip-th row starting with first data row
                kwargs = {"skiprows": lambda i: i != 0 and ((i-1) % self._row_skip)}

            self._df = read_csv_into_df(csv_file, replace_slashes=False, **kwargs)
        else:
            _pulse_logger.error(f"File not found: {csv_file}")

class SESeries():
    __slots__ = ["_plot_config", "_output_filename", "_title",
                 "_x_header", "_x_label", "_x_bounds", "_x2_header",
                 "_y_headers", "_y_label", "_y_bounds", "_y2_headers",
                 "_y2_label", "_y2_bounds"]

    def __init__(self, config: Optional[SEPlotConfig]=None, output_filename: str="", title: Optional[str]=None,
        x_header: Optional[str]=None, x_label: Optional[str]=None, x_bounds: Optional[SEBounds]=None,
        x2_header: Optional[str]=None, y_headers: Optional[List[str]]=None, y_label: Optional[str]=None,
        y_bounds: Optional[SEBounds]=None, y2_headers: Optional[List[str]]=None, y2_label: Optional[str]=None,
        y2_bounds: Optional[SEBounds]=None
    ):
        self._plot_config = config
        self._output_filename = output_filename
        self._title = title
        self._x_header = x_header
        self._x_label = x_label
        self._x_bounds = deepcopy(x_bounds) if x_bounds is not None else SEBounds()
        self._x2_header = x2_header
        self._y_headers = list(y_headers) if y_headers is not None else []
        self._y_label = y_label
        self._y_bounds = deepcopy(y_bounds) if y_bounds is not None else SEBounds()
        self._y2_headers = list(y2_headers) if y2_headers is not None else []
        self._y2_label = y2_label
        self._y2_bounds = deepcopy(y2_bounds) if y2_bounds is not None else SEBounds()

    def get_plot_config(self) -> SEPlotConfig:
        if self._plot_config is None:
            self._plot_config = SEPlotConfig()
        return self._plot_config
    def set_plot_config(self, config: SEPlotConfig) -> None:
        self._plot_config = deepcopy(config)
    def has_plot_config(self) -> bool:
        return self._plot_config is not None

    def get_output_filename(self) -> str:
        return self._output_filename
    def set_output_filename(self, output_filename: str) -> None:
        self._output_filename = output_filename
    def has_output_filename(self) -> bool:
        return self._output_filename is not None and len(self._output_filename) > 0
    def invalidate_output_filename(self) -> None:
        self._output_filename = ""

    def get_title(self) -> Union[str, None]:
        return self._title
    def set_title(self, title: str) -> None:
        self._title = title
    def has_title(self) -> bool:
        return self._title is not None
    def invalidate_title(self) -> None:
        self._title = None

    def get_x_header(self) -> Union[str, None]:
        return self._x_header
    def set_x_header(self, header: str) -> None:
        self._x_header = header
    def has_x_header(self) -> bool:
        return self._x_header is not None
    def invalidate_x_header(self) -> None:
        self._x_header = None

    def get_x_label(self) -> Union[str, None]:
        return self._x_label
    def set_x_label(self, label: str) -> None:
        self._x_label = label
    def has_x_label(self) -> bool:
        return self._x_label is not None
    def invalidate_x_label(self) -> None:
        self._x_label = None

    def get_x_bounds(self) -> SEBounds:
        return self._x_bounds
    def set_x_bounds(self, bounds: SEBounds) -> None:
        self._x_bounds = deepcopy(bounds)

    def get_x2_header(self) -> Union[str, None]:
        return self._x2_header
    def set_x2_header(self, header: str) -> None:
        self._x2_header = header
    def has_x2_header(self) -> bool:
        return self._x2_header is not None
    def invalidate_x2_header(self) -> None:
        self._x2_header = None

    def get_y_headers(self) -> List[str]:
        return self._y_headers
    def add_y_header(self, header: str) -> None:
        if self._y_headers is None:
            self._y_headers = []
        self._y_headers.append(header)
    def has_y_headers(self) -> bool:
        return len(self._y_headers) > 0
    def invalidate_y_headers(self) -> None:
        self._y_headers = []

    def get_y_label(self) -> Union[str, None]:
        return self._y_label
    def set_y_label(self, label: str) -> None:
        self._y_label = label
    def has_y_label(self) -> bool:
        return self._y_label is not None
    def invalidate_y_label(self) -> None:
        self._y_label = None

    def get_y_bounds(self) -> SEBounds:
        return self._y_bounds
    def set_y_bounds(self, bounds: SEBounds) -> None:
        self._y_bounds = deepcopy(bounds)

    def get_y2_headers(self) -> List[str]:
        return self._y2_headers
    def add_y2_header(self, header: str) -> None:
        if self._y2_headers is None:
            self._y2_headers = []
        self._y2_headers.append(header)
    def has_y2_headers(self) -> bool:
        return len(self._y2_headers) > 0
    def invalidate_y2_headers(self) -> None:
        self._y2_headers = []

    def get_y2_label(self) -> Union[str, None]:
        return self._y2_label
    def set_y2_label(self, label: str) -> None:
        self._y2_label = label
    def has_y2_label(self) -> bool:
        return self._y2_label is not None
    def invalidate_y2_label(self) -> None:
        self._y2_label = None

    def get_y2_bounds(self) -> SEBounds:
        return self._y2_bounds
    def set_y2_bounds(self, bounds: SEBounds) -> None:
        self._y2_bounds = deepcopy(bounds)

class SEPlotter():
    __slots__ = ["_plot_config"]

    def __init__(self, config: Optional[SEPlotConfig]=None):
        self._plot_config = config

    def get_plot_config(self) -> SEPlotConfig:
        if self._plot_config is None:
            self._plot_config = SEPlotConfig()
        return self._plot_config
    def set_plot_config(self, config: SEPlotConfig) -> None:
        self._plot_config = deepcopy(config)
    def has_plot_config(self) -> bool:
        return self._plot_config is not None

class SEMultiHeaderSeriesPlotter(SEPlotter):
    __slots__ = ["_plot_sources", "_series", "_validation_source"]

    def __init__(self, config: Optional[SEPlotConfig]=None, plot_sources: Optional[List[SEPlotSource]]=None,
        series: Optional[List[SESeries]]=None, validation_source: Optional[SEPlotSource]=None
    ):
        super().__init__(config)

        self._plot_sources = list(plot_sources) if plot_sources else []
        self._series = list(series) if series else []
        self._validation_source = validation_source

    def get_plot_sources(self) -> List[SEPlotSource]:
        return self._plot_sources
    def add_plot_source(self, plot_source: SEPlotSource) -> None:
        if self._plot_sources is None:
            self._plot_sources = []
        self._plot_sources.append(deepcopy(plot_source))
    def has_plot_sources(self) -> bool:
        return len(self._plot_sources) > 0
    def invalidate_plot_sources(self) -> None:
        self._plot_sources = []

    def get_series(self) -> List[SESeries]:
        return self._series
    def add_series(self, series: SESeries) -> None:
        if self._series is None:
            self._series = []
        self._series.append(series)
    def has_series(self) -> bool:
        return len(self._series) > 0
    def invalidate_series(self) -> None:
        self._series = []

    def get_validation_source(self) -> Union[SEPlotSource, None]:
        return self._validation_source
    def set_validation_source(self, validation_source: SEPlotSource) -> None:
        self._validation_source = deepcopy(validation_source)
    def has_valiation_source(self) -> bool:
        return self._validation_source is not None
    def invalidate_validation_source(self) -> None:
        self._validation_source = None

class ePlotType(Enum):
    NoPlot = 0
    FullPlot = 1
    FullPlotErrors = 2
    FastPlot = 3
    FastPlotErrors = 4
    MemoryFastPlot = 5
class SEComparePlotter(SEPlotter):
    __slots__ = ["_computed_source", "_expected_source", "_failures", "_rms", "_plot_type"]

    def __init__(self, config: Optional[SEPlotConfig]=None, computed_source: Optional[SEPlotSource]=None,
        expected_source: Optional[SEPlotSource]=None, failures: Optional[Set[str]]=None,
        rms: Optional[Dict[str, float]]=None, plot_type: ePlotType=ePlotType.FastPlot
    ):
        super().__init__(config)

        self._computed_source = computed_source
        self._expected_source = expected_source
        self._failures = set(failures) if failures is not None else set()
        self._rms = dict(rms) if rms is not None else dict()
        self._plot_type = plot_type

    def get_plot_type(self) -> ePlotType:
        return self._plot_type
    def set_plot_type(self, ptype: ePlotType) -> None:
        self._plot_type = ptype

    def get_computed_source(self) -> Union[SEPlotSource, None]:
        return self._computed_source
    def set_computed_source(self, computed_source: SEPlotSource) -> None:
        self._computed_source = deepcopy(computed_source)
    def has_computed_source(self) -> bool:
        return self._computed_source is not None
    def invalidate_computed_source(self) -> None:
        self._computed_source = None

    def get_expected_source(self) -> Union[SEPlotSource, None]:
        return self._expected_source
    def set_expected_source(self, expected_source: SEPlotSource) -> None:
        self._expected_source = deepcopy(expected_source)
    def has_expected_source(self) -> bool:
        return self._expected_source is not None
    def invalidate_expected_source(self) -> None:
        self._expected_source = None

    def get_failures(self) -> Set[str]:
        return self._failures
    def add_failure(self, header: str) -> None:
        self._failures.add(header)
    def has_failures(self) -> bool:
        return len(self._failures) > 0
    def invalidate_failures(self) -> None:
        self._failures = set()

    def get_rms_values(self) -> Dict[str, float]:
        return self._rms
    def add_rms_value(self, header: str, value: float) -> None:
        self._rms[header] = value
    def has_rms_values(self) -> bool:
        return len(self._rms) > 0
    def invalidate_rms_values(self) -> None:
        self._rms = dict()


class SEMonitorPlotter(SEPlotter):
    __slots__ = ["_plot_source", "_plot_vitals", "_plot_ventilator", "_times_s", "_data_requested_file",
                 "_output_prefix"]

    def __init__(self, config: Optional[SEPlotConfig]=None, plot_source: Optional[SEPlotSource]=None,
        plot_vitals: Optional[bool]=False, plot_ventilator: Optional[bool]=False,
        times_s: Optional[List[float]]=None, data_requested_file: Optional[Union[str, Path]] = None,
        output_prefix: Optional[str]=""
    ):
        super().__init__(config)

        self._plot_source = plot_source
        self._plot_vitals = plot_vitals
        self._plot_ventilator = plot_ventilator
        self._times_s = list(times_s) if times_s is not None else None
        self._data_requested_file = Path(data_requested_file) if data_requested_file is not None else None
        self._output_prefix = output_prefix

    def get_plot_source(self) -> Union[SEPlotSource, None]:
        return self._plot_source
    def set_plot_source(self, plot_source: SEPlotSource) -> None:
        self._plot_source = deepcopy(plot_source)
    def has_plot_source(self) -> bool:
        return self._plot_source is not None
    def invalidate_plot_source(self) -> None:
        self._plot_source = None

    def get_plot_vitals(self) -> bool:
        return self._plot_vitals
    def set_plot_vitals(self, p: bool) -> None:
        self._plot_vitals = p

    def get_plot_ventilator(self) -> bool:
        return self._plot_ventilator
    def set_plot_ventilator(self, p: bool) -> None:
        self._plot_ventilator = p

    def get_times_s(self) -> Union[List[float], None]:
        return self._times_s
    def set_times_s(self, times_s: List[float]) -> None:
        self._times_s = times_s
    def has_times_s(self) -> bool:
        return self._times_s is not None
    def invalidate_times_s(self) -> None:
        self._times_s = None

    def get_data_requested_file(self) -> Union[Path, None]:
        return self._data_requested_file
    def set_data_requested_file(self, f: Union[str, Path]) -> None:
        self._data_requested_file = Path(f)
    def has_data_requested_file(self) -> bool:
        return self._data_requested_file is not None
    def invalidate_data_requested_file(self) -> None:
        self._data_requested_file = None

    def get_output_prefix(self) -> str:
        return self._output_prefix
    def set_output_prefix(self, prefix: str) -> None:
        self._output_prefix = prefix
    def has_output_prefix(self) -> bool:
        return len(self._output_prefix) > 0
    def invalidate_output_prefix(self) -> None:
        self._output_prefix = ""
