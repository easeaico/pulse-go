# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import sys
import logging
import argparse
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path
from PIL import Image

from datetime import timedelta
from timeit import default_timer as timer

from pulse.cdm.engine import eEvent
from pulse.cdm.engine import eSwitch
from pulse.cdm.io.engine import serialize_data_requested_result_from_file
from pulse.cdm.plots import SEPlotConfig, SEPlotSource, SEMonitorPlotter
from pulse.cdm.utils.file_utils import adjust_filepath
from pulse.cdm.utils.logger import PulseLog

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
        data_requested_file = adjust_filepath(monitor_plotter.get_data_requested_file())
        if not data_requested_file.is_file():
            _pulse_logger.error(f"Data requested file does not exist: {data_requested_file}")
            return
        results = serialize_data_requested_result_from_file(adjust_filepath(monitor_plotter.get_data_requested_file()))
        for segment in results.get_segments():
            if segment.id <= 0:  # Don't generate monitors for segments < 0
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
            
            # Create temporary files for individual components
            temp_monitor_file = dest_dir / f"temp_ventilator_monitor_{idx+1}.jpg"
            temp_alarms_file = dest_dir / f"temp_ventilator_alarms_{idx+1}.jpg"
            temp_loops_file = dest_dir / f"temp_ventilator_loops_{idx+1}.jpg"
            
            # Generate individual images
            create_ventilator_monitor_image(csv_file, start_time_s, end_time_s, temp_monitor_file)
            create_ventilator_alarms_image(csv_file, start_time_s, end_time_s, temp_alarms_file)
            create_ventilator_loops_image(csv_file, start_time_s, end_time_s, temp_loops_file)
            
            # Combine the three images into one
            final_file = dest_dir / f"{filename_prefix}ventilator_monitor_{idx+1}.jpg"
            combine_ventilator_images(temp_monitor_file, temp_alarms_file, temp_loops_file, final_file)
            
            # Clean up temporary files
            temp_monitor_file.unlink(missing_ok=True)
            temp_alarms_file.unlink(missing_ok=True)
            temp_loops_file.unlink(missing_ok=True)

    if benchmark:
        end = timer()
        _pulse_logger.info(f'Plotter Execution Time: {timedelta(seconds=end - start)}')


def combine_ventilator_images(monitor_path, alarms_path, loops_path, output_path):
    try:
        # Load the three images
        monitor_img = Image.open(monitor_path)
        alarms_img = Image.open(alarms_path)
        loops_img = Image.open(loops_path)
        
        # Get dimensions
        monitor_width, monitor_height = monitor_img.size
        alarms_width, alarms_height = alarms_img.size
        loops_width, loops_height = loops_img.size
        
        # Calculate right panel dimensions based on monitor
        right_panel_width = int(monitor_width * 0.33)  # 33% of monitor width (so total is 133% -> 75%/25% ratio)
        alarms_target_height = int(monitor_height * 0.33)  # 33% of monitor height
        loops_target_height = int(monitor_height * 0.67)   # 67% of monitor height
        
        # Calculate total combined width
        total_width = monitor_width + right_panel_width
        total_height = monitor_height
        
        # Resize images maintaining aspect ratio (fit within target dimensions)
        def resize_maintain_aspect(img, target_width, target_height):
            img_width, img_height = img.size
            aspect_ratio = img_width / img_height
            target_aspect = target_width / target_height
            
            if aspect_ratio > target_aspect:
                # Image is wider than target - fit to width
                new_width = target_width
                new_height = int(target_width / aspect_ratio)
            else:
                # Image is taller than target - fit to height
                new_height = target_height
                new_width = int(target_height * aspect_ratio)
            
            return img.resize((new_width, new_height), Image.Resampling.LANCZOS)
        
        # Resize images maintaining their aspect ratios
        alarms_resized = resize_maintain_aspect(alarms_img, right_panel_width, alarms_target_height)
        loops_resized = resize_maintain_aspect(loops_img, right_panel_width, loops_target_height)
        
        # Create the combined image with black background
        combined_img = Image.new('RGB', (total_width, total_height), color='black')
        
        # Paste the monitor image on the left
        combined_img.paste(monitor_img, (0, 0))
        
        # Calculate centering positions for right panel images
        alarms_resized_width, alarms_resized_height = alarms_resized.size
        loops_resized_width, loops_resized_height = loops_resized.size
        
        # Center alarms horizontally in right panel, top-aligned in its section
        alarms_x = monitor_width + (right_panel_width - alarms_resized_width) // 2
        alarms_y = (alarms_target_height - alarms_resized_height) // 2
        
        # Center loops horizontally in right panel, center-aligned in its section
        loops_x = monitor_width + (right_panel_width - loops_resized_width) // 2
        loops_y = alarms_target_height + (loops_target_height - loops_resized_height) // 2
        
        # Paste resized images
        combined_img.paste(alarms_resized, (alarms_x, alarms_y))
        combined_img.paste(loops_resized, (loops_x, loops_y))
        
        # Save the combined image
        combined_img.save(output_path, 'JPEG', quality=95)
        _pulse_logger.info(f"Combined ventilator images saved to {output_path}")
        
    except Exception as e:
        _pulse_logger.error(f"Error combining ventilator images: {e}")


def create_vitals_monitor_image(csv_file: Path, start_time_s: float, end_time_s: float, fig_name: str="vitals_monitor.jpg"):
    # Read the CSV file
    data = pd.read_csv(adjust_filepath(csv_file))

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
    csv_path = adjust_filepath(csv_file)
    data = pd.read_csv(csv_path)

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
    minute_ventilation = f'{round(filtered_data["MechanicalVentilator-TotalPulmonaryVentilation(L/min)"].iloc[-1], 1):.1f}' if "MechanicalVentilator-TotalPulmonaryVentilation(L/min)" in filtered_data else "--"
    respiration_rate = f'{round(filtered_data["MechanicalVentilator-RespirationRate(1/min)"].iloc[-1], 0):.0f}' if "MechanicalVentilator-RespirationRate(1/min)" in filtered_data else "--"
    plateau_pressure = f'{round(filtered_data["MechanicalVentilator-PlateauPressure(cmH2O)"].iloc[-1], 0):.0f}' if "MechanicalVentilator-PlateauPressure(cmH2O)" in filtered_data else "--"
    static_pulmonary_compliance = f'{round(filtered_data["MechanicalVentilator-StaticRespiratoryCompliance(mL/cmH2O)"].iloc[-1], 0):.0f}' if "MechanicalVentilator-StaticRespiratoryCompliance(mL/cmH2O)" in filtered_data else "--"
    tidal_volume = f'{round(filtered_data["MechanicalVentilator-TidalVolume(mL)"].iloc[-1], 0):.0f}' if "MechanicalVentilator-TidalVolume(mL)" in filtered_data else "--"
    mean_airway_pressure = f'{round(filtered_data["MechanicalVentilator-MeanAirwayPressure(cmH2O)"].iloc[-1], 0):.0f}' if "MechanicalVentilator-MeanAirwayPressure(cmH2O)" in filtered_data else "--"
    end_tidal_carbon_dioxide_pressure = f'{round(filtered_data["MechanicalVentilator-EndTidalCarbonDioxidePressure(mmHg)"].iloc[-1], 0):.0f}' if "MechanicalVentilator-EndTidalCarbonDioxidePressure(mmHg)" in filtered_data else "--"
    peak_expiratory_flow = f'{round(filtered_data["MechanicalVentilator-PeakExpiratoryFlow(L/min)"].iloc[-1], 1):.1f}' if "MechanicalVentilator-PeakExpiratoryFlow(L/min)" in filtered_data else "--"
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

    # Create the parameter display boxes in 4x2 grid layout to match the image
    # Row 1: PIP and VT
    # Row 2: MVe and Pmean
    # Row 3: RR and etCO2
    # Row 4: Pplat and PEF
    # Row 5: cStat and I:E (bottom row)
    
    parameter_data = [
        # Row 1
        ("PIP (cmH20)", peak_inspiratory_pressure),
        ("VT (mL)", tidal_volume),
        # Row 2  
        ("MVe (L/min)", minute_ventilation),
        ("Pmean (cmH20)", mean_airway_pressure),
        # Row 3
        ("RR (/min)", respiration_rate),
        ("etCO2 (mmHg)", end_tidal_carbon_dioxide_pressure),
        # Row 4
        ("Pplat (cmH20)", plateau_pressure),
        ("PEF (L/min)", peak_expiratory_flow),
        # Row 5
        ("cStat (mL/cmH20)", static_pulmonary_compliance),
        ("I:E", inspiratory_expiratory_ratio),
    ]

    # Grid layout parameters - centered vertically with plots
    box_width = 0.14
    box_height = 0.16
    start_x = 0.72  # Moved closer to plots to reduce buffer
    # Center the 5-row matrix vertically (total height = 5 * box_height = 0.8)
    # Plot area spans from 0.1 to 0.95 (height = 0.85), so center is at 0.525
    matrix_center = 0.525
    matrix_half_height = (5 * box_height) / 2  # 0.4
    start_y = matrix_center + matrix_half_height  # Center + half height
    x_spacing = box_width  # No gap between boxes horizontally
    y_spacing = box_height  # No gap between boxes vertically

    # Create 4x2 grid of parameter boxes
    for i, (label, value) in enumerate(parameter_data):
        row = i // 2
        col = i % 2
        
        x_pos = start_x + col * x_spacing
        y_pos = start_y - row * y_spacing
        
        # Create rectangle
        rect = plt.Rectangle(
            (x_pos, y_pos - box_height),
            box_width,
            box_height,
            edgecolor="white",
            facecolor="black",
            linewidth=2
        )
        fig.add_artist(rect)
        
        # Add label text (slightly larger)
        fig.text(
            x_pos + box_width/2,
            y_pos - 0.02,
            label,
            color="white",
            fontsize=14,
            weight='bold',
            va="top",
            ha="center"
        )
        
        # Add value text (slightly larger)
        fig.text(
            x_pos + box_width/2,
            y_pos - box_height/2,
            str(value),
            color="white",
            fontsize=22,
            weight='bold',
            va="center",
            ha="center"
        )

    # Adjust spacing between subplots to make room for parameter boxes - reduced buffer
    plt.subplots_adjust(left=0.1, right=0.7, top=0.95, bottom=0.1)

    # Set the background color of the figure
    fig.patch.set_facecolor("black")

    # Save the image
    _pulse_logger.info(f"Saving plot {fig_name}")
    plt.savefig(fig_name, facecolor="black", bbox_inches="tight")
    #plt.show()


def create_ventilator_alarms_image(csv_file: Path, start_time_s: float, end_time_s: float, fig_name: str="ventilator_alarms.jpg"):
    # Read the CSV file
    csv_path = adjust_filepath(csv_file)
    data = pd.read_csv(csv_path)
    # Filter rows between 1.0 and 20.0 in the "Time(s)" column
    filtered_data = data[(data["Time(s)"] >= start_time_s) & (data["Time(s)"] <= end_time_s)]
    if len(filtered_data) == 0:
        _pulse_logger.error(f"No data found between times {start_time_s} and {end_time_s}. Cannot create ventilator alarms {fig_name}")
        return
    # Grab the log file
    log_file = Path(csv_path).with_suffix(".log")
    if not log_file.exists():
        _pulse_logger.error(f"Expected log file {log_file} does not exist")
        return
    # Load and get information out of the log for our plot
    log = PulseLog()
    log.parse(log_file)
    
    # Get alarms
    # Pressure Alarms
    # Pressure Alarms
    has_high_PIP_alarm = log.get_event_status(eEvent.HighPressureAlarmTriggered, end_time_s) == eSwitch.On
    has_low_PIP_alarm = log.get_event_status(eEvent.LowPressureAlarmTriggered, end_time_s) == eSwitch.On
    has_high_PEEP_alarm = log.get_event_status(eEvent.HighPositiveEndExpiratoryPressureAlarmTriggered, end_time_s) == eSwitch.On
    has_low_PEEP_alarm = log.get_event_status(eEvent.LowPositiveEndExpiratoryPressureAlarmTriggered, end_time_s) == eSwitch.On
    has_auto_PEEP = log.get_event_status(eEvent.AutoPositiveEndExpiratoryPressureAlarmTriggered, end_time_s) == eSwitch.On
    # Volume Alarms
    has_high_VT_alarm = log.get_event_status(eEvent.HighTidalVolumeAlarmTriggered, end_time_s) == eSwitch.On
    has_low_VT_alarm = log.get_event_status(eEvent.LowTidalVolumeAlarmTriggered, end_time_s) == eSwitch.On
    # Minute Ventilation Alarms
    has_high_MV_alarm = log.get_event_status(eEvent.HighMinuteVentilationAlarmTriggered, end_time_s) == eSwitch.On
    has_low_MV_alarm = log.get_event_status(eEvent.LowMinuteVentilationAlarmTriggered, end_time_s) == eSwitch.On
    # Oxygenation Alarms
    has_high_SPO2_alarm = log.get_event_status(eEvent.HighOxygenSaturationAlarmTriggered, end_time_s) == eSwitch.On
    has_low_SPO2_alarm = log.get_event_status(eEvent.LowOxygenSaturationAlarmTriggered, end_time_s) == eSwitch.On
    has_oxygen_supply_failure_alarm = log.get_event_status(eEvent.OxygenSupplyFailureAlarmTriggered, end_time_s) == eSwitch.On
    # CO2 Alarms
    has_high_EtCO2_alarm = log.get_event_status(eEvent.HighEndTidalCarbonDioxideAlarmTriggered, end_time_s) == eSwitch.On
    has_low_EtCO2_alarm = log.get_event_status(eEvent.LowEndTidalCarbonDioxideAlarmTriggered, end_time_s) == eSwitch.On
    # Respiratory Rate Alarm
    has_high_RR_alarm = log.get_event_status(eEvent.HighRespiratoryRateAlarmTriggered, end_time_s) == eSwitch.On
    # Leak and Apnea
    has_circuit_leak_alarm = log.get_event_status(eEvent.CircuitLeakAlarmTriggered, end_time_s) == eSwitch.On
    has_apnea_alarm = log.get_event_status(eEvent.ApneaTimeAlarmTriggered, end_time_s) == eSwitch.On

    # Create list of active alarms with their display names
    active_alarms = []
    alarm_checks = [
        (has_high_PIP_alarm, "High PIP Alarm"),
        (has_low_PIP_alarm, "Low PIP Alarm"),
        (has_high_PEEP_alarm, "High PEEP Alarm"),
        (has_low_PEEP_alarm, "Low PEEP Alarm"),
        (has_auto_PEEP, "Auto PEEP Alarm"),
        (has_high_VT_alarm, "High Tidal Volume Alarm"),
        (has_low_VT_alarm, "Low Tidal Volume Alarm"),
        (has_high_MV_alarm, "High Minute Ventilation Alarm"),
        (has_low_MV_alarm, "Low Minute Ventilation Alarm"),
        (has_high_SPO2_alarm, "High SpO2 Alarm"),
        (has_low_SPO2_alarm, "Low SpO2 Alarm"),
        (has_oxygen_supply_failure_alarm, "Oxygen Supply Failure"),
        (has_high_EtCO2_alarm, "High EtCO2 Alarm"),
        (has_low_EtCO2_alarm, "Low EtCO2 Alarm"),
        (has_high_RR_alarm, "High Respiratory Rate Alarm"),
        (has_circuit_leak_alarm, "Circuit Leak Alarm"),
        (has_apnea_alarm, "Apnea Alarm")
    ]
    
    # Collect active alarms
    for alarm_status, alarm_name in alarm_checks:
        if alarm_status:
            active_alarms.append(alarm_name)
    
    # Limit to first 5 alarms if more than 5
    if len(active_alarms) > 5:
        active_alarms = active_alarms[:5]
    
    # Create the figure
    fig, ax = plt.subplots(1, 1, figsize=(8, 6))
    ax.set_xlim(0, 1)
    ax.set_ylim(0, 1)
    ax.axis('off')  # Hide axes
    
    # Set background color
    fig.patch.set_facecolor("black")
    ax.set_facecolor("black")
    
    # Create alarm box
    if len(active_alarms) == 0:
        # No alarms - black box with white text
        box_color = "black"
        text_color = "white"
        edge_color = "white"
        display_text = "No Alarms"
    else:
        # Alarms present - red box with white text
        box_color = "red"
        text_color = "white"
        edge_color = "red"
        display_text = "\n".join(active_alarms)
    
    # Create the alarm box (centered)
    box_width = 0.8
    box_height = 0.6
    box_x = 0.1
    box_y = 0.2
    
    rect = plt.Rectangle(
        (box_x, box_y),
        box_width,
        box_height,
        edgecolor=edge_color,
        facecolor=box_color,
        linewidth=3
    )
    ax.add_patch(rect)
    
    # Add text to the box
    ax.text(
        0.5, 0.5,  # Center of the plot
        display_text,
        color=text_color,
        fontsize=24,
        weight='bold',
        ha='center',
        va='center',
        linespacing=1.5
    )
    
    # Save the image
    _pulse_logger.info(f"Saving alarm plot {fig_name}")
    plt.savefig(fig_name, facecolor="black", bbox_inches="tight")
    #plt.show()


def create_ventilator_loops_image(csv_file: Path, start_time_s: float, end_time_s: float, fig_name: str="ventilator_loops.jpg"):
    # Read the CSV file
    data = pd.read_csv(adjust_filepath(csv_file))

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
