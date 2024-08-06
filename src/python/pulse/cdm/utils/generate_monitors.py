# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import sys
import logging
import argparse
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path
from typing import Sequence
from datetime import timedelta
from timeit import default_timer as timer

from pulse.cdm.plots import SEPlotConfig, SEPlotSource, SEMonitorPlotter
from pulse.cdm.io.engine import serialize_data_requested_result_from_file


_pulse_logger = logging.getLogger('pulse')


def generate_monitors(monitor_plotter: SEMonitorPlotter, benchmark: bool=False):
    if benchmark:
        start = timer()

    csv_file = None
    if monitor_plotter.has_plot_source():
        ps = monitor_plotter.get_plot_source()
        if not ps.has_csv_data():
            _pulse_logger.error("Plot source has no csv file. Unable to plot monitors.")
            return
        csv_file = ps.get_csv_data()
    else:
        _pulse_logger.error("Monitor plotter has no plot source. Unable to plot monitors.")
        return

    times_s = []
    if monitor_plotter.has_times_s():
        times_s = monitor_plotter.get_times_s()
    elif monitor_plotter.has_data_requested_file():
        data_requested_file = monitor_plotter.get_data_requested_file()
        if not data_requested_file.is_file():
            _pulse_logger.error(f"Data requested file does not exist: {data_requested_file}")
            return
        results = serialize_data_requested_result_from_file(monitor_plotter.get_data_requested_file())
        for segment in results.get_segments():
            if segment.id == 0:  # Don't generate monitors for segment 0
                continue
            times_s.append(segment.time_s)
    else:
        _pulse_logger.error("No times provided. "
                            "To generate monitors, times must be provided directly or via a data requested file.")
        return

    dest_dir = Path(".")
    if monitor_plotter.has_plot_config():
        config = monitor_plotter.get_plot_config()
        if config.has_output_path_override():
            dest_dir = config.get_output_path_override()

    filename_prefix = ""
    if monitor_plotter.has_output_prefix():
        filename_prefix = monitor_plotter.get_output_prefix()

    vitals = monitor_plotter.get_plot_vitals()
    ventilator = monitor_plotter.get_plot_ventilator()

    # Make sure destination dir exists
    dest_dir.mkdir(parents=True, exist_ok=True)

    vitals_monitor_timeframe_s = 15.
    ventilator_timeframe_s = 15.
    for idx, end_time_s in enumerate(times_s):
        if vitals:
            start_time_s = end_time_s - vitals_monitor_timeframe_s
            if start_time_s < 0:
                _pulse_logger.warning("Full vitals monitor timeframe not possible, restricting start time to 0")
                start_time_s = 0.
            out_file = dest_dir / f"{filename_prefix}vitals_monitor_{idx+1}.jpg"
            create_vitals_monitor_image(csv_file, start_time_s, end_time_s, out_file)

        if ventilator:
            start_time_s = end_time_s - ventilator_timeframe_s
            if start_time_s < 0:
                _pulse_logger.warning("Full ventilator monitor timeframe not possible, restricting start time to 0")
                start_time_s = 0.
            out_file = dest_dir / f"{filename_prefix}ventilator_monitor_{idx+1}.jpg"
            create_ventilator_monitor_image(csv_file, start_time_s, end_time_s, out_file)

            out_file = dest_dir / f"{filename_prefix}ventilator_loops_{idx+1}.jpg"
            create_ventilator_loops_image(csv_file, start_time_s, end_time_s, out_file)

    if benchmark:
        end = timer()
        _pulse_logger.info(f'Plotter Execution Time: {timedelta(seconds=end - start)}')


def create_vitals_monitor_image(csv_file: Path, start_time_s: float, end_time_s: float, fig_name: str="vitals_monitor.jpg"):
    # Read the CSV file
    data = pd.read_csv(csv_file)

    # Filter rows between start time and end time in the "Time(s)" column
    filtered_data_long = data[(data["Time(s)"] >= start_time_s) & (data["Time(s)"] <= end_time_s)]
    filtered_data_short = data[(data["Time(s)"] >= start_time_s + 10) & (data["Time(s)"] <= end_time_s)]
    if len(filtered_data_long) == 0:
        _pulse_logger.error(f"No data found between times {start_time_s} and {end_time_s}. Cannot create vitals monitor {fig_name}")
        return

    # Extract required data for subplots
    time_long = filtered_data_long["Time(s)"]
    time_short = filtered_data_short["Time(s)"]
    ecg = filtered_data_short["ECG-Lead3ElectricPotential(mV)"] if "ECG-Lead3ElectricPotential(mV)" in filtered_data_short else pd.Series([np.nan] * len(time_short))
    arterial_pressure = filtered_data_short["ArterialPressure(mmHg)"] if "ArterialPressure(mmHg)" in filtered_data_short else pd.Series([np.nan] * len(time_short))
    carbon_dioxide_partial_pressure = filtered_data_long["Carina-CarbonDioxide-PartialPressure(mmHg)"] if "Carina-CarbonDioxide-PartialPressure(mmHg)" in filtered_data_long else pd.Series([np.nan] * len(time_long))

    # Extract required data for number values
    heart_rate = f'{round(filtered_data_long["HeartRate(1/min)"].iloc[-1], 0):.0f}' if "HeartRate(1/min)" in filtered_data_long else "--"
    mean_arterial_pressure = f'{round(filtered_data_long["MeanArterialPressure(mmHg)"].iloc[-1], 0):.0f}' if "MeanArterialPressure(mmHg)" in filtered_data_long else "--"
    systolic_arterial_pressure = f'{round(filtered_data_long["SystolicArterialPressure(mmHg)"].iloc[-1], 0):.0f}' if "SystolicArterialPressure(mmHg)" in filtered_data_long else "--"
    diastolic_arterial_pressure = f'{round(filtered_data_long["DiastolicArterialPressure(mmHg)"].iloc[-1], 0):.0f}' if "DiastolicArterialPressure(mmHg)" in filtered_data_long else "--"
    oxygen_saturation = f'{round(filtered_data_long["OxygenSaturation"].iloc[-1]*100, 0):.0f}' if "OxygenSaturation" in filtered_data_long else "--"
    end_tidal_carbon_dioxide_pressure = f'{round(filtered_data_long["EndTidalCarbonDioxidePressure(mmHg)"].iloc[-1], 0):.0f}' if "EndTidalCarbonDioxidePressure(mmHg)" in filtered_data_long else "--"
    respiration_rate = f'{round(filtered_data_long["RespirationRate(1/min)"].iloc[-1], 0):.0f}' if "RespirationRate(1/min)" in filtered_data_long else "--"
    core_temperature = f'{round(filtered_data_long["CoreTemperature(degC)"].iloc[-1], 1):.1f}' if "CoreTemperature(degC)" in filtered_data_long else "--"

    # Create the figure and subplots
    fig, axs = plt.subplots(3, 1, figsize=(14, 8))

    # Set the facecolor of each subplot
    for ax in axs:
        ax.set_facecolor("black")

    # Configure the subplots
    axs[0].plot(time_short, ecg, color="green")
    axs[0].set_ylabel("ECGIII (mV)", color="green", fontsize=18)
    axs[0].tick_params(colors="white")
    axs[0].spines["bottom"].set_color("white")
    axs[0].spines["top"].set_color("white")
    axs[0].spines["right"].set_color("white")
    axs[0].spines["left"].set_color("white")

    axs[1].plot(time_short, arterial_pressure, color="red")
    axs[1].set_ylabel("ABP (mmHg)", color="red", fontsize=18)
    axs[1].tick_params(colors="white")
    axs[1].spines["bottom"].set_color("white")
    axs[1].spines["top"].set_color("white")
    axs[1].spines["right"].set_color("white")
    axs[1].spines["left"].set_color("white")

    axs[2].plot(time_long, carbon_dioxide_partial_pressure, color="yellow")
    axs[2].set_xlabel("Time (s)", color="white", fontsize=18)
    axs[2].set_ylabel("CO2 (mmHg)", color="yellow", fontsize=18)
    axs[2].tick_params(colors="white")
    axs[2].spines["bottom"].set_color("white")
    axs[2].spines["top"].set_color("white")
    axs[2].spines["right"].set_color("white")
    axs[2].spines["left"].set_color("white")

    # Create the number value boxes
    value_boxes_headings = [
        f"HR (/min)\n",
        f"BP  (mmHg)\n",
        f"SpO2 (%)\n",
        f"etCO2 (mmHg)\n",
        f"RR (/min)\n",
        f"T (oC)\n",
    ]

    value_boxes_values = [
        f"\n{heart_rate}",
        f"\n{systolic_arterial_pressure} / {diastolic_arterial_pressure} ({mean_arterial_pressure})",
        f"\n{oxygen_saturation}",
        f"\n{end_tidal_carbon_dioxide_pressure}",
        f"\n{respiration_rate}",
        f"\n{core_temperature}",
    ]

    colors = [
        "green",
        "red",
        "blue",
        "yellow",
        "purple",
        "orange",
    ]

    value_box_height = 0.15
    value_box_width = 0.3
    value_box_start = 0.65

    for i, value_box_heading in enumerate(value_boxes_headings):
        rect = plt.Rectangle(
            (value_box_start, 1 - (i + 1) * value_box_height),
            value_box_width,
            value_box_height,
            edgecolor="white",
            facecolor="black",
        )
        fig.add_artist(rect)
        fig.text(
            value_box_start + 0.02,
            1 - (i + 1) * value_box_height + 0.08,
            value_box_heading,
            color=colors[i],
            fontsize=18,
            va="center",
            ha="left",
            linespacing=1.5
        )
        fig.text(
            value_box_start + value_box_width / 2,
            1 - (i + 1) * value_box_height + 0.08,
            value_boxes_values[i],
            color=colors[i],
            fontsize=36,
            weight='bold',
            va="center",
            ha="center",
            linespacing=1.5
        )

    # Adjust spacing between subplots
    plt.subplots_adjust(left=0.1, right=value_box_start-0.05, top=0.95, bottom=0.1)

    # Set the background color of the figure
    fig.patch.set_facecolor("black")

    # Save the image
    _pulse_logger.info(f"Saving plot {fig_name}")
    plt.savefig(fig_name, facecolor="black", bbox_inches="tight")
    #plt.show()


def create_ventilator_monitor_image(csv_file: Path, start_time_s: float, end_time_s: float, fig_name: str="ventilator_monitor.jpg"):
    # Read the CSV file
    data = pd.read_csv(csv_file)

    # Filter rows between 1.0 and 20.0 in the "Time(s)" column
    filtered_data = data[(data["Time(s)"] >= start_time_s) & (data["Time(s)"] <= end_time_s)]
    if len(filtered_data) == 0:
        _pulse_logger.error(f"No data found between times {start_time_s} and {end_time_s}. Cannot create ventilator monitor {fig_name}")
        return

    # Extract required data for subplots
    time = filtered_data["Time(s)"]
    airway_pressure = filtered_data["MechanicalVentilator-AirwayPressure(cmH2O)"] if "MechanicalVentilator-AirwayPressure(cmH2O)" in filtered_data else pd.Series([np.nan] * len(time))
    muscle_pressure = filtered_data["TransMusclePressure(cmH2O)"] if "TransMusclePressure(cmH2O)" in filtered_data else pd.Series([np.nan] * len(time))
    inspiratory_flow = filtered_data["MechanicalVentilator-InspiratoryFlow(L/min)"] if "MechanicalVentilator-InspiratoryFlow(L/min)" in filtered_data else pd.Series([np.nan] * len(time))
    total_lung_volume = filtered_data["MechanicalVentilator-TotalLungVolume(mL)"] if "MechanicalVentilator-TotalLungVolume(mL)" in filtered_data else pd.Series([np.nan] * len(time))

    # Extract required data for number values
    peak_inspiratory_pressure = f'{round(filtered_data["MechanicalVentilator-PeakInspiratoryPressure(cmH2O)"].iloc[-1], 0):.0f}' if "MechanicalVentilator-PeakInspiratoryPressure(cmH2O)" in filtered_data else "--"
    mean_airway_pressure = f'{round(filtered_data["MechanicalVentilator-MeanAirwayPressure(cmH2O)"].iloc[-1], 0):.0f}' if "MechanicalVentilator-MeanAirwayPressure(cmH2O)" in filtered_data else "--"
    tidal_volume = f'{round(filtered_data["MechanicalVentilator-TidalVolume(mL)"].iloc[-1], 0):.0f}' if "MechanicalVentilator-TidalVolume(mL)" in filtered_data else "--"
    respiration_rate = f'{round(filtered_data["MechanicalVentilator-RespirationRate(1/min)"].iloc[-1], 0):.0f}' if "MechanicalVentilator-RespirationRate(1/min)" in filtered_data else "--"
    end_tidal_carbon_dioxide_pressure = f'{round(filtered_data["MechanicalVentilator-EndTidalCarbonDioxidePressure(mmHg)"].iloc[-1], 0):.0f}' if "MechanicalVentilator-EndTidalCarbonDioxidePressure(mmHg)" in filtered_data else "--"
    static_pulmonary_compliance = f'{round(filtered_data["MechanicalVentilator-StaticRespiratoryCompliance(mL/cmH2O)"].iloc[-1], 0):.0f}' if "MechanicalVentilator-StaticRespiratoryCompliance(mL/cmH2O)" in filtered_data else "--"
    inspiratory_expiratory_ratio = f'1:{round(1 / filtered_data["MechanicalVentilator-InspiratoryExpiratoryRatio"].iloc[-1], 1):.1f}' if "MechanicalVentilator-InspiratoryExpiratoryRatio" in filtered_data else "--"

    # Create the figure and subplots
    fig, axs = plt.subplots(4, 1, figsize=(14, 14))

    # Set the facecolor of each subplot
    for ax in axs:
        ax.set_facecolor("black")

    # Configure the subplots
    axs[0].plot(time, airway_pressure, color="white")
    axs[0].set_ylabel("Paw (cmH2O)", color="white", fontsize=18)
    axs[0].tick_params(colors="white")
    axs[0].spines["bottom"].set_color("white")
    axs[0].spines["top"].set_color("white")
    axs[0].spines["right"].set_color("white")
    axs[0].spines["left"].set_color("white")
    axs[0].axhline(0, color='grey')

    axs[1].plot(time, muscle_pressure, color="white")
    axs[1].set_ylabel("Pmus (cmH2O)", color="white", fontsize=18)
    axs[1].tick_params(colors="white")
    axs[1].spines["bottom"].set_color("white")
    axs[1].spines["top"].set_color("white")
    axs[1].spines["right"].set_color("white")
    axs[1].spines["left"].set_color("white")
    axs[1].axhline(0, color='grey')

    axs[2].plot(time, inspiratory_flow, color="white")
    axs[2].set_ylabel("Flow (L/min)", color="white", fontsize=18)
    axs[2].tick_params(colors="white")
    axs[2].spines["bottom"].set_color("white")
    axs[2].spines["top"].set_color("white")
    axs[2].spines["right"].set_color("white")
    axs[2].spines["left"].set_color("white")
    axs[2].axhline(0, color='grey')

    axs[3].plot(time, total_lung_volume, color="white")
    axs[3].set_xlabel("Time (s)", color="white", fontsize=18)
    axs[3].set_ylabel("Volume (mL)", color="white", fontsize=18)
    axs[3].tick_params(colors="white")
    axs[3].spines["bottom"].set_color("white")
    axs[3].spines["top"].set_color("white")
    axs[3].spines["right"].set_color("white")
    axs[3].spines["left"].set_color("white")
    axs[3].axhline(0, color='grey')

    # Create the number value boxes
    value_boxes_headings = [
        f"PIP (cmH2O)\n",
        f"Pmean (cmH2O)\n",
        f"VT (mL)\n",
        f"RR (/min)\n",
        f"etCO2 (mmHg)\n",
        f"cStat (mL/cmH2O)\n",
        f"I:E\n",
    ]

    value_boxes_values = [
        f"\n{peak_inspiratory_pressure}",
        f"\n{mean_airway_pressure}",
        f"\n{tidal_volume}",
        f"\n{respiration_rate}",
        f"\n{end_tidal_carbon_dioxide_pressure}",
        f"\n{static_pulmonary_compliance}",
        f"\n{inspiratory_expiratory_ratio}",
    ]

    value_box_height = 0.14
    value_box_width = 0.3
    value_box_start = 0.65

    for i, value_box_heading in enumerate(value_boxes_headings):
        rect = plt.Rectangle(
            (value_box_start, 1 - (i + 1) * value_box_height),
            value_box_width,
            value_box_height,
            edgecolor="white",
            facecolor="black",
        )
        fig.add_artist(rect)
        fig.text(
            value_box_start + 0.02,
            1 - (i + 1) * value_box_height + 0.08,
            value_box_heading,
            color="white",
            fontsize=18,
            va="center",
            ha="left",
            linespacing=1.5
        )
        fig.text(
            value_box_start + value_box_width / 2,
            1 - (i + 1) * value_box_height + 0.08,
            value_boxes_values[i],
            color="white",
            fontsize=36,
            weight='bold',
            va="center",
            ha="center",
            linespacing=1.5
        )

    # Adjust spacing between subplots
    plt.subplots_adjust(left=0.1, right=value_box_start-0.05, top=0.95, bottom=0.1)

    # Set the background color of the figure
    fig.patch.set_facecolor("black")

    # Save the image
    _pulse_logger.info(f"Saving plot {fig_name}")
    plt.savefig(fig_name, facecolor="black", bbox_inches="tight")
    #plt.show()


def create_ventilator_loops_image(csv_file: Path, start_time_s: float, end_time_s: float, fig_name: str="ventilator_loops.jpg"):
    # Read the CSV file
    data = pd.read_csv(csv_file)

    # Filter rows between 1.0 and 20.0 in the "Time(s)" column
    filtered_data = data[(data["Time(s)"] >= start_time_s) & (data["Time(s)"] <= end_time_s)]
    if len(filtered_data) == 0:
        _pulse_logger.error(f"No data found between times {start_time_s} and {end_time_s}. Cannot create ventilator loops {fig_name}")
        return

    # Extract required data for subplots
    airway_pressure = filtered_data["MechanicalVentilator-AirwayPressure(cmH2O)"] if "MechanicalVentilator-AirwayPressure(cmH2O)" in filtered_data else pd.Series([], dtype="float64")
    inspiratory_flow = filtered_data["MechanicalVentilator-InspiratoryFlow(L/min)"] if "MechanicalVentilator-InspiratoryFlow(L/min)" in filtered_data else pd.Series([np.nan] * len(airway_pressure), dtype="float64")
    total_lung_volume = filtered_data["MechanicalVentilator-TotalLungVolume(mL)"] if "MechanicalVentilator-TotalLungVolume(mL)" in filtered_data else pd.Series([np.nan] * len(inspiratory_flow), dtype="float64")
    if airway_pressure.empty and not inspiratory_flow.empty:
        airway_pressure = pd.Series([np.nan] * len(inspiratory_flow))

    # Create the figure and subplots
    fig, axs = plt.subplots(2, 1, figsize=(6, 12))

    # Set the facecolor of each subplot
    for ax in axs:
        ax.set_facecolor("black")

    # Configure the subplots
    axs[0].plot(airway_pressure, total_lung_volume, color="white")
    axs[0].set_xlabel("Pressure (cmH2O)", color="white", fontsize=18)
    axs[0].set_ylabel("Volume (mL)", color="white", fontsize=18)
    axs[0].tick_params(colors="white")
    axs[0].spines["bottom"].set_color("white")
    axs[0].spines["top"].set_color("white")
    axs[0].spines["right"].set_color("white")
    axs[0].spines["left"].set_color("white")
    axs[0].axhline(0, color='grey')

    axs[1].plot(total_lung_volume, inspiratory_flow, color="white")
    axs[1].set_xlabel("Volume (mL)", color="white", fontsize=18)
    axs[1].set_ylabel("Flow (L/min)", color="white", fontsize=18)
    axs[1].tick_params(colors="white")
    axs[1].spines["bottom"].set_color("white")
    axs[1].spines["top"].set_color("white")
    axs[1].spines["right"].set_color("white")
    axs[1].spines["left"].set_color("white")
    axs[1].axhline(0, color='grey')

    # Set the background color of the figure
    fig.patch.set_facecolor("black")

    plt.subplots_adjust(wspace=0.25, hspace=0.25)

    # Save the image
    _pulse_logger.info(f"Saving plot {fig_name}")
    plt.savefig(fig_name, facecolor="black", bbox_inches="tight")
    #plt.show()


if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG, format='%(levelname)s: %(message)s')
    logging.getLogger('PIL').setLevel(logging.INFO)
    plt.set_loglevel("info")

    parser = argparse.ArgumentParser(description="Generate monitor snapshots for the given results file and times")
    parser.add_argument(
        "csv_file",
        type=Path,
        help="results csv filepath"
    )
    parser.add_argument(
        "times_s",
        type=float,
        nargs='+',
        help="times of interest (seconds)"
    )
    parser.add_argument(
        "-d", "--destination",
        type=Path,
        default=Path("."),
        help="where to save generated imaged (default:  %(default)s)"
    )
    parser.add_argument(
        "-m", "--mechanical-ventilator",
        action="store_true",
        help="generate ventilator monitors"
    )
    parser.add_argument(
        "-v", "--vitals",
        action="store_true",
        help="generate vitals monitors"
    )
    opts = parser.parse_args()

    plot_config = SEPlotConfig()
    plot_config.set_values(output_path_override=opts.destination)
    monitor_plotter = SEMonitorPlotter(
        plot_config=plot_config,
        plot_source = SEPlotSource(csv_data=opts.csv_file),
        plot_vitals=opts.vitals,
        plot_ventilator=opts.mechanical_ventilator,
        times_s=opts.times_s,

    )

    generate_monitors(monitor_plotter)
