# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import re
import sys
import shutil
import logging
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.style as mplstyle
import matplotlib.ticker as mplticker
from pathlib import Path
from cycler import cycler
from datetime import timedelta
from textwrap import TextWrapper
from typing import List, Optional
from matplotlib import colors as mcolors
from timeit import default_timer as timer

from pulse.cdm.plots import *
from pulse.cdm.io.plots import serialize_plotter_list_from_file
from pulse.cdm.utils.file_utils import get_config_dir
from pulse.cdm.utils.generate_monitors import generate_monitors


_pulse_logger = logging.getLogger('pulse')


def process_plot_file(plots_file: Path, benchmark: bool = False):
    plotters = []
    serialize_plotter_list_from_file(plots_file, plotters)
    create_plots(plotters, benchmark)


def plot_with_test_results(plotters: []):
    def _adjust_filepath(fp: str) -> str:
        if "$VERIFICATION_DIR" in fp:
            fp = fp.replace("$VERIFICATION_DIR", "./test_results")
        return fp

    for p in plotters:
        if isinstance(p, SEMultiHeaderSeriesPlotter):
            if p.has_plot_sources():
                sources = p.get_plot_sources()
                for source in sources:
                    source.set_csv_data(_adjust_filepath(source.get_csv_data()))
        elif isinstance(p, SEMonitorPlotter):
            if p.has_plot_source():
                source = p.get_plot_source()
                source.set_csv_data(_adjust_filepath(source.get_csv_data()))
            if p.has_data_requested_file():
                p.set_data_requested_file(_adjust_filepath(p.get_data_requested_file().as_posix()))
        else:
            _pulse_logger.error(f"Unknown plotter type: {p}")


def create_plots(plotters: [], benchmark: bool = False):
    for p in plotters:
        if isinstance(p, SEMultiHeaderSeriesPlotter):
            multi_header_series_plotter(p, benchmark)
        elif isinstance(p, SEComparePlotter):
            compare_plotter(p, benchmark)
        elif isinstance(p, SEMonitorPlotter):
            generate_monitors(p, benchmark)
        else:
            _pulse_logger.error(f"Unknown plotter type: {p}")


def multi_header_series_plotter(plotter: SEMultiHeaderSeriesPlotter, benchmark: bool = False):
    if benchmark:
        start = timer()

    if not plotter.has_plot_sources():
        _pulse_logger.error("No plot source provided")
        return
    sources = plotter.get_plot_sources()
    validation_source = plotter.get_validation_source()

    # Create output directory if it does not exist
    output_path = Path("./")
    if plotter.get_plot_config().has_output_path_override():
        output_path = plotter.get_plot_config().get_output_path_override()
    if not output_path.is_dir():
        output_path.mkdir(parents=True)

    for series in plotter.get_series():
        if benchmark:
            start_series = timer()

        config = series.get_plot_config()

        # Check if disabled
        if config.get_disabled():
            continue

        # Default x header is time (first column)
        x_header = None
        if series.has_x_header():
            x_header = series.get_x_header()
        elif not sources[0].get_data_frame().empty:
            x_header = sources[0].get_data_frame().columns[0]
        else: # No data frame
            continue
        x2_header = None
        if series.has_x2_header():
            x2_header = series.get_x2_header()
        elif series.has_y2_headers() or validation_source:
            x2_header = x_header

        config.set_output_filename(series.get_output_filename())
        config.set_x_bounds(series.get_x_bounds())
        config.set_y_bounds(series.get_y_bounds())
        config.set_y2_bounds(series.get_y2_bounds())
        if series.has_title():
            config.set_title(series.get_title())
        else:
            config.invalidate_title()
        if series.has_x_label():
            config.set_x_label(series.get_x_label())
        else:
            config.invalidate_x_label()
        if series.has_y_label():
            config.set_y_label(series.get_y_label())
        else:
            config.invalidate_y_label()
        if series.has_y2_label():
            config.set_y2_label(series.get_y2_label())
        else:
            config.invalidate_y2_label()

        y2_headers = series.get_y2_headers()
        if validation_source and not y2_headers:
            y2_headers = series.get_y_headers()

        # Note: A naming collision could occur in cases where a filename is not provided.
        # Generally, plot names/titles are provided in the configuration. When either of
        # these values are not provided, an attempt will be made to infer/generate the
        # missing information.
        #
        # If the title is missing, one will be generated based on the x and y headers.
        # If the filename is missing, the title will be used as the filename (with
        # relevant characters such as spaces replaced).
        #
        # Since titles are not guaranteed to be unique, neither are filenames;
        # potentially resulting in overwritten files. If you experiences this,
        # provide a unique title/filename in the plot configuration or update
        # this logic.
        if not config.has_title() and series.has_y_headers():
            config.set_title(generate_title(x_header, series.get_y_headers()[0]))
        if not config.get_output_filename():
            title = config.get_title()
            if not title: # Empty string title
                if series.has_y_headers():
                    title = generate_title(x_header, series.get_y_headers()[0])
                else:
                    _pulse_logger.error("Plot has no title nor output filename and one cannot be generated (is this just a legend?)")
                    continue
            config.set_output_filename(generate_filename(title))

        output_filename = Path(config.get_output_filename())
        # Add file extension if needed
        if not output_filename.suffix:
            output_filename = output_filename.with_suffix(config.get_image_properties().get_file_format())
        output_filepath = output_path / output_filename

        if create_plot(
            sources,
            config,
            x_header,
            series.get_y_headers(),
            x2_header,
            y2_headers,
            validation_source
        ):
            save_current_plot(output_filepath, config.get_image_properties())
        else:
            _pulse_logger.error(f"Failed to create plot {output_filepath}")
        clear_current_plot()

        if benchmark:
            end_series = timer()
            _pulse_logger.info(f'Series Execution Time: {timedelta(seconds=end_series - start_series)}')

    if benchmark:
        end = timer()
        _pulse_logger.info(f'Plotter Execution Time: {timedelta(seconds=end - start)}')


def csv_plotter(csv: Path, benchmark: bool = False):
    if benchmark:
        start = timer()

    if not csv.is_file():
        _pulse_logger.error(f"CSV does not exist {csv}")

    output_dir = csv.parent / f"{csv.stem}_plots"

    # Remove and recreate directory
    try:
        if output_dir.is_dir():
            shutil.rmtree(output_dir)
    except OSError as e:
        _pulse_logger.warning(f"Could not remove old report directory: {output_dir}")
    output_dir.mkdir(parents=True, exist_ok=True)

    # Create plot source so we can open csv just once
    ps = SEPlotSource(csv_data=str(csv), line_format="-k")
    df = ps.get_data_frame()

    # Need to compare against time
    x_header = None
    if not df.empty:
         for c in df.columns:
            if re.search('^Time', c):
                x_header = c
                break
    if x_header is None:
        _pulse_logger.error("Time is missing from the CSV file")
        return

    config = SEPlotConfig(set_defaults=True)
    config.set_values(
        gridlines=True,
        output_path_override=output_dir,
        plot_actions=True,
        plot_events=False,
    )

    # Action Event Legend
    if config.get_plot_actions() or config.get_plot_events():
        legend_success = True
        if ps.parse_actions_events():
            ae = ps.get_actions_events(
                plot_actions=config.get_plot_actions(),
                plot_events=config.get_plot_events(),
                allow_actions_with=config.get_allow_actions_with(),
                allow_events_with=config.get_allow_events_with(),
                omit_actions_with=config.get_omit_actions_with(),
                omit_events_with=config.get_omit_events_with(),
                count_limit=20
            )
            if ae:
                config.set_legend_mode(eLegendMode.OnlyActionEventLegend)
                output_filename = "ActionEventLegend" + config.get_image_properties().get_file_format()
                output_filepath = output_dir / output_filename
                if create_plot(
                    [ps],
                    config,
                    x_header,
                    [],
                ):
                    im_props = deepcopy(config.get_image_properties())
                    im_props.set_dimension_mode(eDimensionMode.Unbound)
                    save_current_plot(output_filepath, im_props)
                else:
                    legend_success = False
                clear_current_plot()
            else:
                config.set_plot_actions(False)
                config.set_plot_events(False)
        else:
            legend_success = False
        if not legend_success:
            _pulse_logger.error(f"Failed to create legend {output_dir}")
            config.set_plot_actions(False)
            config.set_plot_events(False)


    # Plot every header against time
    def _plot_header(sources: List[SEPlotSource]):
        if benchmark:
            start_series = timer()

        config.set_title(generate_title(x_header, y_header))
        config.set_output_filename(generate_filename(config.get_title()))
        config.set_legend_mode(eLegendMode.HideActionEventLegend)

        output_filename = Path(config.get_output_filename())
        if not output_filename.suffix:
            output_filename = output_filename.with_suffix(config.get_image_properties().get_file_format())
        output_filepath = output_dir / output_filename

        x_label = " ".join(
            [f"{s.get_label() if s.has_label() else y_header} is NaN"
                for s in sources if s.get_data_frame()[y_header].isna().all()]
        )
        if x_label:
            config.set_x_label(x_label)

        # Axis bounds
        if "PlasmaConcentration" in y_header:
            config.set_log_axis(True)

        if create_plot(
            sources,
            config,
            x_header,
            [y_header],
        ):
            save_current_plot(output_filepath, config.get_image_properties())
        else:
            _pulse_logger.error(f"Failed to create plot {output_filepath}")

        # Reset
        clear_current_plot()
        config.invalidate_x_label()
        config.invalidate_y_bounds()
        config.set_log_axis(False)

        if benchmark:
            end_series = timer()
            _pulse_logger.info(f'Series Execution Time: {timedelta(seconds=end_series - start_series)}')
    for y_header in df.columns:
        if y_header == x_header:
            continue
        _plot_header([ps])

    if benchmark:
        end = timer()
        _pulse_logger.info(f'Plotter Execution Time: {timedelta(seconds=end - start)}')



def compare_plotter(plotter: SEComparePlotter, benchmark: bool = False):
    if benchmark:
        start = timer()

    computed_source = plotter.get_computed_source()
    computed_df = computed_source.get_data_frame()
    expected_source = plotter.get_expected_source()
    expected_df = expected_source.get_data_frame()

    if ((plotter.get_plot_type() == ePlotType.FastPlotErrors or \
            plotter.get_plot_type() == ePlotType.FullPlotErrors) and \
            len(plotter.get_failures()) == 0):  # Only plotting failures and no failures to plot
        _pulse_logger.info(f"No plots for {computed_source.get_csv_data()}.")
        return

    # Create output directory if it does not exist
    output_path = Path("./")
    if plotter.get_plot_config().has_output_path_override():
        output_path = plotter.get_plot_config().get_output_path_override()
    if not output_path.is_dir():
        output_path.mkdir(parents=True)

    # Need to compare against time
    x_header = None
    if not expected_df.empty:
         for c in expected_df.columns:
            if re.search('^Time', c):
                x_header = c
                break
    if x_header is None:
        if not computed_df.empty:
            _pulse_logger.error("There is no data in the expected file? I will plot what you computed...")
            for c in computed_df.columns:
                if re.search('^Time', c):
                    x_header = c
                    break
        else:
            _pulse_logger.error("Both expected and computed files seem to be empty?")
            return

    # Action Event Legend
    config = plotter.get_plot_config()
    if config.get_plot_actions() or config.get_plot_events():
        legend_success = True
        if computed_source.parse_actions_events():
            ae = computed_source.get_actions_events(
                plot_actions=config.get_plot_actions(),
                plot_events=config.get_plot_events(),
                allow_actions_with=config.get_allow_actions_with(),
                allow_events_with=config.get_allow_events_with(),
                omit_actions_with=config.get_omit_actions_with(),
                omit_events_with=config.get_omit_events_with(),
                count_limit=20
            )
            if ae:
                expected_source.set_actions_events(ae)

                config.set_legend_mode(eLegendMode.OnlyActionEventLegend)
                output_filename = "ActionEventLegend" + config.get_image_properties().get_file_format()
                output_filepath = output_path / output_filename
                if create_plot(
                    [computed_source],
                    config,
                    x_header,
                    [],
                ):
                    im_props = deepcopy(config.get_image_properties())
                    im_props.set_dimension_mode(eDimensionMode.Unbound)
                    save_current_plot(output_filepath, im_props)
                else:
                    legend_success = False
                clear_current_plot()
            else: # No actions/events to plot
                config.set_plot_actions(False)
                config.set_plot_events(False)
        else:
            legend_success = False
        if not legend_success:
            _pulse_logger.error(f"Failed to create legend: {output_path}")
            config.set_plot_actions(False)
            config.set_plot_events(False)

    class _eColorMode(Enum):
        Default = 0
        Pass = 1
        Fail = 2
    # Helper function to plot every header against x_header
    def _plot_header(sources: List[SEPlotSource], color_mode: _eColorMode=_eColorMode.Default):
        if benchmark:
            start_series = timer()

        config.set_title(generate_title(x_header, y_header))
        config.set_output_filename(generate_filename(config.get_title()).replace("_vs_", "vs"))
        config.set_legend_mode(eLegendMode.HideActionEventLegend)

        output_filename = Path(config.get_output_filename())
        if not output_filename.suffix:
            output_filename = output_filename.with_suffix(config.get_image_properties().get_file_format())
        output_filepath = output_path / output_filename

        x_label = " ".join(
            [f"{s.get_label() if s.has_label() else s.get_csv_data()} is NaN"
                for s in sources if s.get_data_frame()[y_header].isna().all()]
        )
        if x_label:
            config.set_x_label(x_label)

        # Axis bounds
        if "PlasmaConcentration" in y_header:
            config.set_log_axis(True)

        # Update background color based on fail/pass
        dark_bg_params = {
            "legend.facecolor" : "dimgrey",
            "text.color" : "w",
            "ytick.color" : "w",
            "xtick.color" : "w",
            "axes.labelcolor" : "w",
            "axes.edgecolor" : "w"
        }
        if color_mode == _eColorMode.Default:
            color = "w"
        elif color_mode == _eColorMode.Pass:
            color = "lime"
        elif color_mode == _eColorMode.Fail:
            plt.rcParams.update(dark_bg_params)
            color = "red"

        if create_plot(
            sources,
            config,
            x_header,
            [y_header],
        ):
            save_current_plot(output_filepath, config.get_image_properties(), facecolor=color)
        else:
            _pulse_logger.error(f"Failed to create plot {output_filepath}")

        # Reset
        clear_current_plot()
        config.invalidate_x_label()
        config.invalidate_y_bounds()
        config.set_log_axis(False)
        plt.rcParams.update(plt.rcParamsDefault)

        if benchmark:
            end_series = timer()
            _pulse_logger.info(f'Series Execution Time: {timedelta(seconds=end_series - start_series)}')

    # Plot all expected columns
    rms_values = plotter.get_rms_values()
    for y_header in expected_df.columns:
        if y_header == x_header:
            continue
        if y_header not in computed_df.columns:
            continue

        if y_header in plotter.get_failures():
            mode = _eColorMode.Fail
        else:
            if plotter.get_plot_type() == ePlotType.FastPlotErrors or plotter.get_plot_type() == ePlotType.FullPlotErrors:
                continue  # Only plot errors
            mode = _eColorMode.Pass

        computed_label = computed_source.get_label()
        if y_header in rms_values and not np.isnan(rms_values[y_header]):
            computed_source.set_label(f"{computed_label} (RMS = {rms_values[y_header]:.4f})")

        _plot_header([expected_source, computed_source], color_mode=mode)

        # Restore label to original value
        computed_source.set_label(computed_label)

    # Plot anything not in expected data
    for y_header in computed_df.columns:
        if y_header == x_header:
            continue
        if y_header in expected_df.columns:
            continue

        _pulse_logger.warning(f"{y_header} not found in expected results. Plotting computed data by itself.")
        _plot_header([computed_source], color_mode=_eColorMode.Default)

    if benchmark:
        end = timer()
        _pulse_logger.info(f'Plotter Execution Time: {timedelta(seconds=end - start)}')


def generate_title(x_header: str, y_header: str):
    if not y_header or not x_header:
        return ""
    return y_header + " vs " + x_header


def generate_filename(title: str):
    return (re.sub("[\(\[].*?[\)\]]", "", title)).replace(" ", "_").replace("/", "_Per_")


def percentage_of_baseline(baseline_mode: ePercentageOfBaselineMode,
                           df: pd.DataFrame,
                           x_header: str,
                           y_headers: List[str],
                           x2_header: Optional[str],
                           y2_headers: Optional[List[str]]):
    xcols = []
    ycols = []
    if baseline_mode == ePercentageOfBaselineMode.All:
        xcols = [x_header]
        if x2_header:
            xcols.append(x2_header)
        ycols = list(y_headers)
        if y2_headers:
            ycols.extend(y2_headers)
    elif baseline_mode == ePercentageOfBaselineMode.OnlyX:
        xcols = [x_header]
        if x2_header:
            xcols.append(x2_header)
    elif baseline_mode == ePercentageOfBaselineMode.OnlyY:
        ycols = list(y_headers)
        if y2_headers:
            ycols.extend(y2_headers)
    elif baseline_mode != ePercentageOfBaselineMode.Off:
        _pulse_logger.warning(f"Unknown percentage of baseline mode: {baseline_mode}")
        return False
    df[xcols] = df[xcols].apply(lambda x: (1-(x / x[0])) * 100.0)
    df[ycols] = df[ycols].apply(lambda x: x * 100.0 / x[0])
    return True


# Respects existing line breaks
class DocumentWrapper(TextWrapper):
    def wrap(self, text: str):
        split_text = text.split('\n')
        lines = [line for para in split_text for line in TextWrapper.wrap(self, para)]
        return lines


def create_plot(plot_sources: [SEPlotSource],
                plot_config : SEPlotConfig,
                x_header: str,
                y_headers: List[str],
                x2_header: Optional[str] = None,
                y2_headers: Optional[List[str]] = None,
                validation_source: Optional[SEPlotSource] = None):
    if y2_headers is None:
        y2_headers = []

    # Slightly faster plotting, but may potentially result in a quality loss
    mplstyle.use('fast')

    # To support line formats across axes
    setup_fmt_cycler = cycler(linestyle=['-', '--', '-.', ':']) * cycler(color=['r', 'b', 'g', 'y', 'm', 'c'])
    if plot_config.get_plot_actions() or plot_config.get_plot_events(): # Don't overlap colors between cyclers
        setup_fmt_cycler = cycler(linestyle=['-', '--', '-.', ':']) * cycler(color=['b', 'g', 'c'])
    setup_action_event_fmt_cycler = cycler(color=["magenta", "lime", "yellow", "slateblue", "sandybrown", "dodgerblue"])
    fmt_cycler = setup_fmt_cycler()
    action_event_fmt_cycler = setup_action_event_fmt_cycler()

    # To support legend across axes
    lns = []

    # Labels and title
    fig, ax1 = plt.subplots()
    if plot_config.get_image_properties().get_width_inch() is not None:
        fig.set_size_inches(plot_config.get_image_properties().get_width_inch(), plot_config.get_image_properties().get_height_inch())
    x_label = ""
    if plot_config.has_x_label():
        x_label = plot_config.get_x_label()
    elif x_header:
        x_label = x_header
    ax1.set_xlabel(x_label, fontsize=plot_config.get_font_size())
    y_label = ""
    if plot_config.has_y_label():
        y_label = plot_config.get_y_label()
    elif y_headers:
        y_label = y_headers[0]
    ax1.set_ylabel(y_label, fontsize=plot_config.get_font_size())
    title = ""
    if plot_config.has_title():
        title = plot_config.get_title()
    elif y_headers:
        title = generate_title(x_header, y_headers[0])
    ax1.set_title(title, fontsize=plot_config.get_font_size())
    ax1.ticklabel_format(axis="y", style=plot_config.get_tick_style().name, scilimits=plot_config.get_sci_limits())

    # Main axes
    if plot_config.get_log_axis():
        ax1.set_yscale("log")
        if y_label:
            ax1.set_ylabel(f'Log({y_label})', fontsize=plot_config.get_font_size())
    x_bounds = plot_config.get_x_bounds() if plot_config.has_x_bounds() else None
    if x_bounds is not None and x_bounds.has_lower_bound():
        ax1.set_xlim(left=x_bounds.get_lower_bound())
    if x_bounds is not None and x_bounds.has_upper_bound():
        ax1.set_xlim(right=x_bounds.get_upper_bound())
    determine_y_bounds = False
    y_bounds = plot_config.get_y_bounds() if plot_config.has_y_bounds() else None
    if y_bounds is not None and (y_bounds.has_lower_bound() or y_bounds.has_upper_bound()):
        ax1.set_ylim(bottom=y_bounds.get_lower_bound(), top=y_bounds.get_upper_bound())
    elif plot_config.get_y_bounds_mode() == eYBoundsMode.ZeroMax:
        determine_y_bounds = not plot_config.get_log_axis()
    min_y = None
    max_y = None

    # Secondary y axis
    ax2 = None
    y2_label = ""
    min_y2 = None
    max_y2 = None
    determine_y2_bounds = False
    y2_bounds = plot_config.get_y2_bounds() if plot_config.has_y2_bounds() else None
    if y2_headers:
        ax2 = ax1.twinx()
        if plot_config.has_y2_label():
            y2_label = plot_config.get_y2_label()
        else:
            y2_label = y2_headers[0]
        ax2.set_ylabel(y2_label, fontsize=plot_config.get_font_size())
        ax2.ticklabel_format(axis="y", style=plot_config.get_tick_style().name, scilimits=plot_config.get_sci_limits())
        if plot_config.get_log_axis():
            ax2.set_yscale("log")
            if y2_label:
                ax2.set_ylabel(f'Log({y2_label})', fontsize=plot_config.get_font_size())
        if y2_bounds is not None and y2_bounds.has_lower_bound() or y2_bounds.has_upper_bound():
            ax2.set_ylim(bottom=y2_bounds.get_lower_bound(), top=y2_bounds.get_upper_bound())
        elif plot_config.get_y_bounds_mode() == eYBoundsMode.ZeroMax:
            determine_y2_bounds = not plot_config.get_log_axis()

    # Action/Events axis
    ax3 = None
    if plot_config.get_plot_events() or plot_config.get_plot_actions():
        ax3 = ax1.twinx()
        ax3.axis('off')

    # Helper function to plot each header for specified source and axis
    def _plot_headers(ax, ps, df, x_header, y_headers, y_label):
        color = ""
        for y_header in y_headers:
            if y_header not in df.columns:
                _pulse_logger.error(f"{y_header} not found in {ps.get_csv_data()}")
                return None
            line_lbl = ""
            if ps.has_label():
                line_lbl = ps.get_label()
            elif len(y_headers) > 1:
                line_lbl = y_header
            else:
                line_lbl = y_label

            if ps.has_line_format():
                lns.extend(ax.plot(
                    x_header,
                    y_header,
                    ps.get_line_format(),
                    data=df,
                    label=line_lbl,
                    linewidth=ps.get_line_width(),
                ))
                color = ps.get_line_format()[-1]
            else:
                c = next(fmt_cycler)
                color = c['color']
                lns.extend(ax.plot(
                    x_header,
                    y_header,
                    **c,
                    data=df,
                    label=line_lbl,
                    #linewidth=ps.get_line_width(),
                ))

            if plot_config.get_fill_area():
                ax.fill_between(x_header, y_header, data=df, facecolor=color)

        return color

    # Plot data
    for ps in plot_sources:
        baseline_mode = plot_config.get_percent_of_baseline_mode()
        df = ps.get_data_frame()
        if df.empty:
            _pulse_logger.error(f"Data frame is empty: {ps.get_csv_data()}")
            continue
        if not percentage_of_baseline(baseline_mode,
                                      df,
                                      x_header,
                                      y_headers,
                                      x2_header if not validation_source else None,
                                      y2_headers if not validation_source else None):
            return False

        color = _plot_headers(ax1, ps, df, x_header, y_headers, y_label)
        if color is None:
            return False

        # Identify min/max y values across all headers and sources
        if determine_y_bounds:
            min_ys = [
                m
                for m in (df[y_header].min() for y_header in y_headers if y_header in df.columns)
                if not np.isnan(m)
            ]
            max_ys = [
                m
                for m in (df[y_header].max() for y_header in y_headers if y_header in df.columns)
                if not np.isnan(m)
            ]
            if min_ys:
                min_y_candidate = min(min_ys)
                if min_y is None or min_y_candidate < min_y:
                    min_y = min_y_candidate
            if max_ys:
                max_y_candidate = max(max_ys)
                if max_y is None or max_y_candidate > max_y:
                    max_y = max_y_candidate

        if y2_headers or validation_source:
            ax1.yaxis.label.set_color(color)
            ax1.tick_params(axis='y', colors=color)

        # Secondary axis, not from validation data
        if not validation_source and y2_headers:
            color = _plot_headers(ax2, ps, df, x2_header, y2_headers, y2_label)

            # Identify min/max y values across all headers and sources
            if determine_y2_bounds:
                min_y2s = [
                    m
                    for m in (df[y_header].min() for y_header in y2_headers if y_header in df.columns)
                    if not np.isnan(m)
                ]
                max_y2s = [
                    m
                    for m in (df[y_header].max() for y_header in y2_headers if y_header in df.columns)
                    if not np.isnan(m)
                ]
                if min_y2s:
                    min_y2_candidate = min(min_y2s)
                    if min_y2 is None or min_y2_candidate < min_y2:
                        min_y2 = min_y2_candidate
                if max_y2s:
                    max_y2_candidate = max(max_y2s)
                    if max_y2 is None or max_y2_candidate > max_y2:
                        max_y2 = max_y2_candidate

            ax2.yaxis.label.set_color(color)
            ax2.tick_params(axis='y', colors=color)

    # Plot any actions or events if requested
    ps = plot_sources[0]
    if plot_config.get_plot_events() or plot_config.get_plot_actions():
        # Parse actions/events if needed
        if not ps.has_actions_events():
            if not ps.parse_actions_events():
                return False

        # Plot all actions and events
        for ae in ps.get_actions_events(
            plot_actions=plot_config.get_plot_actions(),
            plot_events=plot_config.get_plot_events(),
            allow_actions_with=plot_config.get_allow_actions_with(),
            allow_events_with=plot_config.get_allow_events_with(),
            omit_actions_with=plot_config.get_omit_actions_with(),
            omit_events_with=plot_config.get_omit_events_with(),
            count_limit=20
        ):
            color = next(action_event_fmt_cycler)['color']
            ax3.axvline(x=ae.time, color = color, label = f"{ae.category.name}:{ae.text}\nt={ae.time}")

    # Plot validation data if needed
    if validation_source:
        df = validation_source.get_data_frame()
        if df.empty:
            _pulse_logger.error(f"Data frame is empty: {validation_source.get_csv_data()}")
        elif y2_headers:
            color = _plot_headers(ax2, validation_source, df, x2_header, y2_headers, y2_label)

            ax2.yaxis.label.set_color(color)
            ax2.tick_params(axis='y', colors=color)
            ax2.set_ylim(ax1.get_ylim())
            min_y2 = min_y
            max_y2 = max_y

    # Scale y axes
    if not plot_config.get_log_axis():
        for det, ax, min_y, max_y in zip([determine_y_bounds, determine_y2_bounds], [ax1, ax2], [min_y, min_y2], [max_y, max_y2]):
            if det:
                if min_y is not None and np.isclose(min_y, 0):
                    min_y = -0.001
                if max_y is not None and np.isclose(max_y, 0):
                    max_y = 0.001
                if min_y is not None and min_y >= 0:
                    min_y = -0.01
                if min_y is not None and not np.isnan(min_y):
                    ax.set_ylim(min_y-0.05*abs(min_y), max_y+0.15*abs(max_y))

    # Ensure negative times aren't shown
    if x_header.lower().startswith("time"):
        l, u = ax1.get_xlim()
        if l < 0:
            l = 0
        ax1.set_xlim(l, u)


    # Dual axis for action/event plots
    if (plot_config.get_plot_actions() or plot_config.get_plot_events()) and ax2 is None:
        ax2 = ax1.twinx()
        ax2.set_ylim(ax1.get_ylim())
        if plot_config.get_log_axis():
            ax2.set_yscale("log")

    # Note that, for both data and action/event legends, the maximum number of columns
    # is estimated via output image width and rough approximations of label lengths.
    # Actual labels are not currently being taken into account and are instead
    # estimated from maximum character count per line based on the DocumentWrapper.
    # Due to this, the full width of the image may not be as highly utilized as
    # possible. For better column estimation, further processing of labels would
    # be required as well as some font size to pixel calculations (considering DPI).
    #
    # The text wrapper attempts to wrap text at logical points but may be forced
    # to break in the middle of a large group of characters without spaces. This
    # particular text wrapper is set up to respect existing line breaks which
    # is useful for action/event legends. If the width given to a wrapper
    # is modified, the estimated widths for the respective legend column
    # approximation will also likely need to be updated.

    # Legend and gridline settings
    if plot_config.get_image_properties().get_width_inch() is not None:
        ax1.xaxis.set_major_locator(mplticker.MaxNLocator(nbins=(plot_config.get_image_properties().get_width_inch() / .55)))
    if plot_config.get_gridlines():
        ax1.grid(linestyle='dotted')
    if plot_config.get_legend_mode() != eLegendMode.NoLegends and len(lns) > 0:
        text_wrapper = DocumentWrapper(width=60)

        box = ax1.get_position()
        ax1.set_position([box.x0, box.y0 + box.height * 0.1,
                        box.width, box.height * 0.9])
        if ax2 is not None:
            ax2.set_position([box.x0, box.y0 + box.height * 0.1,
                        box.width, box.height * 0.9])

        max_ncols = int(plot_config.get_image_properties().get_width_inch() / 2)
        lbls = [text_wrapper.fill(l.get_label()) for l in lns]
        ax1.legend(lns, lbls, fontsize=plot_config.get_legend_font_size(), loc='upper center', bbox_to_anchor=(0.5, -0.1), ncol = min(max_ncols, len(lbls)))

    # Action and event legend
    if plot_config.get_legend_mode() != eLegendMode.NoLegends and \
       plot_config.get_legend_mode() != eLegendMode.HideActionEventLegend and ax3 is not None and ax3.lines:
        text_wrapper = DocumentWrapper(width=45)
        if plot_config.get_image_properties().get_width_inch() is not None:
            max_ncols = int(plot_config.get_image_properties().get_width_inch() / 5.35) # Approximate width of each column for large labels
        else:
            max_ncols = 3

        box = ax1.get_position()
        ax3.set_position([box.x0, box.y0 + box.height * 0.1,
                    box.width, box.height * 0.9])
        lbls = [text_wrapper.fill(l.get_label()) for l in ax3.lines]
        ax3.legend(ax3.lines, lbls, loc='upper center', bbox_to_anchor=(0.5, -0.2), ncol = min(max_ncols, len(lbls)), fontsize=plot_config.get_legend_font_size())

        if plot_config.get_legend_mode() == eLegendMode.OnlyActionEventLegend:
            legend_fig, legend_ax = plt.subplots()
            if plot_config.get_image_properties().get_width_inch() is not None:
                legend_fig.set_size_inches(plot_config.get_image_properties().get_width_inch(), plot_config.get_image_properties().get_height_inch())
            legend_ax.axis(False)
            legend_ax.legend(ax3.lines, lbls, loc='center', ncol = min(max_ncols, len(lbls)), fontsize=int(plot_config.get_legend_font_size()/2))

    return True


def save_current_plot(filename: Path, image_props: SEImageProperties, facecolor: Optional[str] = None):
    _pulse_logger.info(f"Saving plot {filename}")
    figure = plt.gcf()
    if image_props.get_width_inch() is not None:
        # Doing tight layout twice helps prevent legends getting cut off
        plt.tight_layout()
        figure.set_size_inches(image_props.get_width_inch(), image_props.get_height_inch())
        plt.tight_layout()
        figure.savefig(filename, dpi=image_props.get_dpi(), facecolor=facecolor)
    else:
        figure.savefig(filename, dpi=image_props.get_dpi(), facecolor=facecolor, bbox_inches='tight')


def clear_current_plot():
    plt.close('all')
    plt.clf()


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
    logging.getLogger('PIL').setLevel(logging.INFO)
    plt.set_loglevel("info")

    benchmark = False
    plot_file = None

    if len(sys.argv) > 1:
        plot_file = Path(sys.argv[1])
        if not plot_file.is_file():
            plot_file = Path(get_config_dir()+sys.argv[1])
            if not plot_file.is_file():
                _pulse_logger.error("Please provide a valid plot file (JSON or CSV)")
                sys.exit(1)
    if len(sys.argv) > 2 and (sys.argv[2] == '--benchmark' or sys.argv[2] == '-b'):
        benchmark = True

    if benchmark:
        start = timer()

    if plot_file.suffix.lower().endswith(".json"):
        process_plot_file(plot_file, benchmark)
    elif plot_file.suffix.lower().endswith(".csv"):
        csv_plotter(plot_file, benchmark)
    else:
        _pulse_logger.error("Unknown plot file type. Please provide a JSON plot configuration or a CSV data file.")
        sys.exit(1)

    if benchmark:
        end = timer()
        _pulse_logger.info(f'Total Execution Time: {timedelta(seconds=end - start)}')
