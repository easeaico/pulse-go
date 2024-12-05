# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import sys
import shutil
import logging
import numbers
import tempfile
import numpy as np
from enum import Enum
from pathlib import Path
from pycel import ExcelCompiler
from dataclasses import dataclass
from openpyxl import load_workbook
from collections import namedtuple
from openpyxl.worksheet.worksheet import Worksheet
from openpyxl.utils.cell import get_column_letter
from typing import Dict, List, Optional, Union

from pulse.cdm.engine import SEDataRequest
from pulse.cdm.validation import SETimeSeriesValidationTarget, SEPatientTimeSeriesValidation
from pulse.cdm.patient import SEPatient, eSex
from pulse.cdm.scalars import get_unit, LengthUnit, MassUnit
from pulse.cdm.utils.file_utils import get_data_dir, get_validation_dir
from pulse.cdm.io.engine import serialize_data_request_list_to_file
from pulse.cdm.io.patient import serialize_patient_from_file
from pulse.engine.PulseEngineResults import PulseLog
from pulse.pipelines.dataset.utils import generate_data_request

_pulse_logger = logging.getLogger('pulse')


class EngineConfig(Enum):
    Standard = 1
    ExpandedLungs = 3
    ComputationalLife = 2


def gen_patient_targets(
        log_file: Path
) -> SEPatientTimeSeriesValidation:
    """
    Generate patient validation timeseries validation targets.
    This will contain targets to
     1. Validate that the stabilized patient meets the requested patient baseline parameters
     2. Validate that the stabilized patient meets the expected values cited from literature

    :param log_file: Path to log file. Could alternatively be the relevant patient file.

    :return: validation targets, or None if was not successful.
    """

    patient_validation = SEPatientTimeSeriesValidation()
    p = patient_validation.get_patient()
    p.copy(extract_patient(patient_file=log_file))
    # If no name, set to filename for potential table filenames
    if not p.has_name() or not p.get_name():
        p.set_name(log_file.stem)

    table_name = "Patient"
    if table_name not in patient_validation.get_targets():
        patient_validation.get_targets()[table_name] = list()
    tgts = patient_validation.get_targets()[table_name]

    PatientValidation = namedtuple('PatientValidation', ["header", "unit", "table_precision", "patient_attr"])
    validated_headers = [
        PatientValidation(
            header="TotalMetabolicRate",
            unit="kcal/day",
            table_precision=".1f",
            patient_attr="_basal_metabolic_rate"
        ), PatientValidation(
            header="BloodVolume",
            unit="mL",
            table_precision=".1f",
            patient_attr="_blood_volume_baseline"
        ), PatientValidation(
            header="DiastolicArterialPressure",
            unit="mmHg",
            table_precision=".1f",
            patient_attr="_diastolic_arterial_pressure_baseline"
        ), PatientValidation(
            header="HeartRate",
            unit="1/min",
            table_precision=".0f",
            patient_attr="_heart_rate_baseline"
        ), PatientValidation(
            header="MeanArterialPressure",
            unit="mmHg",
            table_precision=".1f",
            patient_attr="_mean_arterial_pressure_baseline"
        ), PatientValidation(
            header="RespirationRate",
            unit="1/min",
            table_precision=".0f",
            patient_attr="_respiration_rate_baseline"
        ), PatientValidation(
            header="SystolicArterialPressure",
            unit="mmHg",
            table_precision=".1f",
            patient_attr="_systolic_arterial_pressure_baseline"
        ), PatientValidation(
            header="TidalVolume",
            unit="mL",
            table_precision=".1f",
            patient_attr="_tidal_volume_baseline"
        ), PatientValidation(
            header="Patient-ExpiratoryReserveVolume",
            unit="mL",
            table_precision=".1f",
            patient_attr="_expiratory_reserve_volume"
        ), PatientValidation(
            header="Patient-FunctionalResidualCapacity",
            unit="mL",
            table_precision=".1f",
            patient_attr="_functional_residual_capacity"
        ), PatientValidation(
            header="Patient-InspiratoryCapacity",
            unit="mL",
            table_precision=".1f",
            patient_attr="_inspiratory_capacity"
        ), PatientValidation(
            header="Patient-InspiratoryReserveVolume",
            unit="mL",
            table_precision=".1f",
            patient_attr="_inspiratory_reserve_volume"
        ), PatientValidation(
            header="Patient-ResidualVolume",
            unit="mL",
            table_precision=".1f",
            patient_attr="_residual_volume"
        ), PatientValidation(
            header="Patient-TotalLungCapacity",
            unit="mL",
            table_precision=".1f",
            patient_attr="_total_lung_capacity"
        ), PatientValidation(
            header="Patient-VitalCapacity",
            unit="mL",
            table_precision=".1f",
            patient_attr="_vital_capacity"
        )
    ]

    for header, unit, table_precision, patient_attr in validated_headers:
        tgt = SETimeSeriesValidationTarget()
        tgt.set_header(f"{header}({unit})" if unit else header)
        tgt.set_patient_specific_setting(True)
        tgt.set_table_formatting(table_precision)
        algo = SETimeSeriesValidationTarget.eTargetType.Mean

        # Only create target if patient has this attribute
        if getattr(p, f"has{patient_attr}")():
            scalar = getattr(p, f"get{patient_attr}")()
            ref_val = scalar.get_value(units=get_unit(unit)) if unit else scalar.get_value()

            tgt.set_target_value(ref_val, algo)
            tgts.append(tgt)

    # Pull all the system validation targets from our spreadsheet
    xls_file = Path(get_validation_dir() + "/SystemValidationData.xlsx")
    generate_validation_targets(
        xls_file=xls_file,
        config=EngineConfig.Standard,
        patient_validation=patient_validation
    )
    return patient_validation


def extract_patient(patient_file: Path) -> SEPatient:
    """
    Attempts to extract patient from given file. Expected
    to be a log or json patient file.

    :param patient_file: Path to patient/log file containing
        patient information.

    :raises ValueError: Unknown patient file type.

    :return: Extracted patient
    """
    if patient_file.suffix.lower() == ".json":
        p = SEPatient()
        serialize_patient_from_file(str(patient_file), p)
        return p
    elif patient_file.suffix.lower() == ".log":
        log = PulseLog(log_files=[patient_file])
        return log.patient
    else:
        raise ValueError("Unknown patient file type: {patient_file}")


def generate_validation_targets(
    xls_file: Path,
    config: EngineConfig,
    patient_validation: SEPatientTimeSeriesValidation
) -> bool:
    """
    Generates timeseries validation targets.

    :param xls_file: Path to xls file.
    :param patient_validation: Where generated targets will be stored.

    :return: Whether validation target generation was successful.
    """
    if not xls_file.is_file():
        _pulse_logger.error(f"Could not find xls file {xls_file}")
        return False

    p = patient_validation.get_patient()
    if p is None:
        _pulse_logger.error(f"No patient provided in patient targets")
        return False

    # Temp file to preserve original state through patient updates.
    # We're saving a temporary file so that pycel can get openpyxl's
    # changes. It may be possible to use pycel exclusively to avoid
    # the need for a temporary file.
    tmp_xls = tempfile.NamedTemporaryFile(suffix=".xlsx")
    tmp_xls_path = Path('./tmp.xlsx')

    # xlsx sheets to skip when generating targets and requests
    ignore_sheets = ["Patient"]
    if config == EngineConfig.Standard:
        ignore_sheets.append("CardiovascularExpandedLungs")
        ignore_sheets.append("RespiratoryExpandedLungs")
        ignore_sheets.append("CardiovascularComputationalLife")

    try:
        # Update patient sheet so formulas can be re-evaluated with correct parameters
        update_patient_sheet(patient=p, xls_file=xls_file, new_file=tmp_xls_path)

        # Generate targets for every xls sheet, organized by table name
        workbook = load_workbook(filename=tmp_xls_path, data_only=False)
        evaluator = ExcelCompiler(filename=tmp_xls_path)
        for system in workbook.sheetnames:
            if system in ignore_sheets:
                continue
            if not generate_sheet_targets(
                sheet=workbook[system],
                evaluator=evaluator,
                patient_validation=patient_validation
            ):
                _pulse_logger.error(f"Unable to generate targets for {system} sheet")

    except Exception as e:
        raise
    finally:  # Always clean up temp file
        tmp_xls.close()

    return True


def update_patient_sheet(patient: SEPatient, xls_file: Path, new_file: Optional[Path]=None) -> None:
    """
    Updates workbook with given patient information.

    :param patient: Relevant patient.
    :param xls_file: Original xls_file path.
    :param new_file: (Optional) If provided, the original file will remain
        unchanged and edits will be saved to this file.
    """
    workbook = load_workbook(filename=xls_file)
    sheet = workbook["Patient"]
    sheet["C2"] = "Male" if patient.get_sex() == eSex.Male else "Female"
    if patient.has_height():
        sheet["C3"] = patient.get_height().get_value(units=LengthUnit.cm)
    if patient.has_weight():
        sheet["C4"] = patient.get_weight().get_value(units=MassUnit.kg)
    if patient.has_right_lung_ratio():
        sheet["C9"] = patient.get_right_lung_ratio().get_value()
    if patient.has_body_fat_fraction():
        sheet["C12"] = patient.get_body_fat_fraction().get_value()

    if new_file is None:
        new_file = xls_file
    workbook.save(filename=new_file)


def generate_data_requests(xls_file: Path, output_dir: Path) -> None:
    """
    Generates all data requests for given xls file.

    :param xls_file: Path to xls_file containing data request information.
    :param output_dir: Path to directory to save generated data requests.
    """
    try:
        if output_dir.is_dir():
            shutil.rmtree(output_dir)
        output_dir.mkdir(parents=True, exist_ok=True)
    except OSError as e:
        _pulse_logger.error(f"Unable to clean directories")
        return

    # Generate data requests for every xls sheet, organized by table name
    ignore_sheets = ["Patient"]
    workbook = load_workbook(filename=xls_file, data_only=True)
    dr_dict = dict()
    for system in workbook.sheetnames:
        if system in ignore_sheets:
            continue
        if not generate_sheet_requests(
                sheet=workbook[system],
                dr_dict=dr_dict
        ):
            _pulse_logger.error(f"Unable to generate requests for {system} sheet")

    # Serialize data request files (one for each table name)
    for dr_file, drs in dr_dict.items():
        filename = output_dir / f"{dr_file}.json"
        _pulse_logger.info(f"Writing {filename}")
        serialize_data_request_list_to_file(drs, filename)


def generate_sheet_requests(sheet: Worksheet, dr_dict: Dict[str, List[SEDataRequest]]) -> bool:
    """
    Generates data requests from given worksheet.

    :param sheet: Relevant xls worksheet.
    :param dr_dict: Dictionary to store generated data requests. Key is table name.

    :return: Whether generating data requests for this sheet was successful.
    """
    system = sheet.title.replace(" ", "")

    # Get header to dataclass mapping
    ws_headers = [cell.value for cell in sheet[1]]
    headers = []  # We don't want to make duplicate data request
    # For example a min and max row for the same property will create 2 requests
    try:
        DRB_HEADER = ws_headers.index('Output')
        DRB_UNITS = ws_headers.index('Units')
        DRB_TABLE = ws_headers.index('Table')
        DRB_REQUEST_TYPE = ws_headers.index('Request Type')
        DRB_REQUEST_PRECISION = ws_headers.index('Request Precision')
    except ValueError as e:
        _pulse_logger.error(f"Missing required header {str(e)[:str(e).find(' is not in list')]}")
        return False

    @dataclass
    class DataRequestBuilder:
        header: str
        units: str
        dr_file: str
        request_type: str
        precision: Union[str, numbers.Number]

    for row_num, r in enumerate(sheet.iter_rows(min_row=2, values_only=True)):
        drb = DataRequestBuilder(
            header=r[DRB_HEADER],
            units=r[DRB_UNITS] if r[DRB_UNITS] else "unitless",
            dr_file=r[DRB_TABLE] if r[DRB_TABLE] else "Orphaned",
            request_type=r[DRB_REQUEST_TYPE],
            precision=r[DRB_REQUEST_PRECISION]
        )

        if not drb.header:
            continue

        if "*" in drb.header:
            continue

        # Skip header rows
        if drb.dr_file == "Table":
            continue

        if not drb.request_type:
            _pulse_logger.info(f"Not generating request {drb.header} (no DR type)")
            continue

        if "@" in drb.request_type:
            # Currently ignoring assessment types
            continue

        if drb.dr_file not in dr_dict:
            dr_dict[drb.dr_file] = []
        drs = dr_dict[drb.dr_file]

        dr = generate_data_request(
            request_type=drb.request_type,
            property_name=drb.header.replace("*", ""),
            unit_str=drb.units.strip(),
            precision=int(drb.precision) if drb.precision else None
        )
        if drb.header not in headers:
            drs.append(dr)
            headers.append(drb.header)

    return True


def generate_sheet_targets(
    sheet: Worksheet,
    evaluator: ExcelCompiler,
    patient_validation: SEPatientTimeSeriesValidation
) -> bool:
    """
    Generates validation targets from given worksheet.

    :param sheet: Relevant xls worksheet.
    :param evaluator: xls evaluator to retrieve computed cell values.
    :param patient_validation: Where generated validation targets will be stored.

    :return: Whether generating validation targets for this sheet was successful.
    """
    system = sheet.title
    targets = patient_validation.get_targets()

    # Get header to dataclass mapping
    ws_headers = [cell.value for cell in sheet[1]]
    try:
        VTB_HEADER = ws_headers.index('Output')
        VTB_UNITS = ws_headers.index('Units')
        VTB_ALGO = ws_headers.index('Algorithm')
        VTB_REF_CELL = ws_headers.index('Reference Values')
        VTB_REFS = ws_headers.index('References')
        VTB_NOTES = ws_headers.index('Notes')
        VTB_TGT_DEST = ws_headers.index('Table')
        VTB_REQUEST_TYPE = ws_headers.index('Request Type')
        VTB_TABLE_PRECISION = ws_headers.index('Table Precision')
        VTB_PATIENT_SPECIFIC = ws_headers.index('PatientSpecific')
        VTB_VALIDATION_THRESHOLD = ws_headers.index('Thresholds')
    except ValueError as e:
        _pulse_logger.error(f"Missing required header {str(e)[:str(e).find(' is not in list')]}")
        return False

    @dataclass
    class ValidationTargetBuilder():
        header: str
        units: str
        algorithm: str
        ref_cell: Union[str, numbers.Number]
        tgt_dest: str
        request_type: str
        references: str
        notes: str
        table_precision: str
        patient_specific: bool
        thresholds: str

    for row_num, r in enumerate(sheet.iter_rows(min_row=2, values_only=True)):
        vtb = ValidationTargetBuilder(
            header=r[VTB_HEADER],
            units=r[VTB_UNITS] if r[VTB_UNITS] else "unitless",
            algorithm=r[VTB_ALGO],
            ref_cell=r[VTB_REF_CELL],
            tgt_dest=r[VTB_TGT_DEST] if r[VTB_TGT_DEST] else "Orphaned",
            request_type=r[VTB_REQUEST_TYPE],
            references=r[VTB_REFS] if r[VTB_REFS] else "",
            notes=r[VTB_NOTES] if r[VTB_NOTES] else "",
            table_precision=f".{r[VTB_TABLE_PRECISION]}" if r[VTB_TABLE_PRECISION] else "",
            patient_specific=True if r[VTB_PATIENT_SPECIFIC] and r[VTB_PATIENT_SPECIFIC].lower() == "y" else False,
            thresholds=r[VTB_VALIDATION_THRESHOLD] if r[VTB_VALIDATION_THRESHOLD] else ""
        )
        if not vtb.header:
            continue

        # Skip header
        if vtb.tgt_dest == "Table":
            continue

        # Nothing to validate to
        if '*' in vtb.header:
            # _pulse_logger.info(f"Ignoring request {vtb.header} (has asterisks)")
            continue
        if vtb.ref_cell is None:
            _pulse_logger.info(f"Not validating {vtb.header} (no reference value)")
            continue

        # Only generate targets with provided DR type
        if not vtb.request_type:
            _pulse_logger.info(f"Not validating {vtb.header} (no DR type)")
            continue

        if vtb.tgt_dest not in targets:
            targets[vtb.tgt_dest] = list()
        vts = targets[vtb.tgt_dest]

        # Evaluate cells if needed
        try:
            ref_val = vtb.ref_cell
            if isinstance(ref_val, str) and ref_val.startswith("="):
                if ref_val.startswith("="):
                    cell_loc = f"{system}!{get_column_letter(VTB_REF_CELL + 1)}{row_num + 2}"
                    ref_val = evaluator.evaluate(cell_loc)
            unit_str = vtb.units.strip()
            if unit_str.startswith("="):
                cell_loc = f"{system}!{get_column_letter(VTB_UNITS + 1)}{row_num + 2}"
                unit_str = evaluator.evaluate(cell_loc)
            ref_str = vtb.references
            if ref_str.startswith("="):
                cell_loc = f"{system}!{get_column_letter(VTB_REFS + 1)}{row_num + 2}"
                ref_str = evaluator.evaluate(cell_loc)
            algo = vtb.algorithm
            if algo.startswith("="):
                cell_loc = f"{system}!{get_column_letter(VTB_ALGO + 1)}{row_num + 2}"
                algo = evaluator.evaluate(cell_loc)
        except:
            _pulse_logger.fatal(f"Cannot evaluate: {cell_loc}")

        tgt = SETimeSeriesValidationTarget()
        tgt.set_reference(ref_str)
        tgt.set_notes(vtb.notes)
        tgt.set_patient_specific_setting(vtb.patient_specific)
        tgt.set_table_formatting(vtb.table_precision)
        if "@" in vtb.request_type:
            tgt.set_assessment(vtb.request_type)
            vtb.request_type = "Physiology"
        if vtb.thresholds:
            for threshold in vtb.thresholds.split(","):
                if "Good" in threshold:
                    tgt.set_good_percent_error(float(threshold[threshold.find("Good") + 5:]))
                elif "Fair" in threshold:
                    tgt.set_fair_percent_error(float(threshold[threshold.find("Fair") + 5:]))

        dr = generate_data_request(
            request_type=vtb.request_type,
            property_name=vtb.header,
            unit_str=unit_str,
            precision=None
        )
        tgt.set_header(dr.to_string())

        if algo == "Max":
            algo = "Maximum"
        elif algo == "Min":
            algo = "Minimum"
        elif algo.endswith("(kg)"):
            algo = algo.replace("(kg)", "_kg")
        try:
            algo = SETimeSeriesValidationTarget.eTargetType[algo]
        except:
            _pulse_logger.error(f"Unknown validation target type: {algo}. Skipping validation target for {vtb.header} in {system}.")
            continue

        # TODO: Support other comparison types?
        if isinstance(ref_val, str):
            if algo == SETimeSeriesValidationTarget.eTargetType.Enumeration:
                tgt.set_target_enum(ref_val)
            else:
                s_vals = ref_val.strip()
                s_vals = s_vals.replace('[', ' ')
                s_vals = s_vals.replace(']', ' ')
                vals = [float(s) for s in s_vals.split(',')]
                tgt.set_target_range(min(vals), max(vals), algo)
        elif isinstance(ref_val, numbers.Number):
            val = float(ref_val)
            tgt.set_target_value(val, algo)
        else:
            _pulse_logger.warning(f"Unknown reference value type {ref_val}")
            tgt.set_target_range(np.nan, np.nan)

        vts.append(tgt)

    return True


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
    logging.getLogger("pycel").setLevel(logging.WARNING)

    xls_file = None

    if len(sys.argv) < 2:
        _pulse_logger.error("Expected inputs : <xls validation file path>")
        sys.exit(1)

    xls_file = Path(sys.argv[1])
    if not xls_file.is_file():
        xls_file = Path(get_data_dir()+sys.argv[1])
        if not xls_file.is_file():
            _pulse_logger.error("Please provide a valid xls file")
            sys.exit(1)

    output_dir = Path("./validation/")
    xls_basename = "".join(xls_file.name.rsplit("".join(xls_file.suffixes), 1))
    xls_basename_out = xls_basename[:-4] if xls_basename.lower().endswith("data") else xls_basename
    output_dir = output_dir / xls_basename_out
    try:
        if output_dir.is_dir():
            shutil.rmtree(output_dir)
        output_dir.mkdir(parents=True, exist_ok=True)
    except OSError as e:
        _pulse_logger.error(f"Unable to clean directories")
        sys.exit(1)

    # Generate system validation targets for every patient file
    patient_dir = Path("./patients/")
    if patient_dir.is_dir():
        for patient_file in patient_dir.glob("*.json"):
            full_output_path = output_dir / f"{patient_file.stem}_SystemTargets.json"
            full_output_path.parent.mkdir(parents=True, exist_ok=True)

            if not generate_validation_targets(
                xls_file=xls_file,
                patient_file=patient_file,
                output_file=full_output_path
            ):
                _pulse_logger.error(f"Failed to generate validation targets for {patient_file}")
    else:
        _pulse_logger.error("Unable to locate patient directory")
