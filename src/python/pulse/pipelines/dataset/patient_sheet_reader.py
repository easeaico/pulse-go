# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import re
import logging
import inspect

from pathlib import Path
from openpyxl.worksheet.worksheet import Worksheet

from pulse.cdm.patient import SEPatient, eSex
from pulse.cdm.io.patient import serialize_patient_to_file

_pulse_logger = logging.getLogger('pulse')


def process_patient_sheet(sheet: Worksheet, output_dir: Path) -> bool:
    output_dir.mkdir(exist_ok=True)
    camel_to_kebab = re.compile(r'(?<!^)(?=[A-Z])')

    # The first column has our labels
    fields = []
    for cell in sheet['A']:
        fields.append(cell.value)

    for column in sheet.iter_cols(min_col=2):
        patient = SEPatient()
        patient.set_sex(eSex.Male)
        for i, cell in enumerate(column):
            field = fields[i]
            if field is None or "Required" in field or " " in field:
                continue
            if cell.value is None:
                continue

            if field == "Name":
                patient.set_name(cell.value)
                continue

            if field == "Sex":
                if cell.value.lower == "female":
                    patient.set_sex(eSex.Female)
                continue

            getter = "get_" + camel_to_kebab.sub('_', field).lower()
            get_scalar = getattr(patient, getter)
            scalar = get_scalar()
            set_value = getattr(scalar, "set_value")
            signature = inspect.signature(set_value)
            if "units" in signature.parameters:
                value = cell.value.split(' ')
                units = signature.parameters["units"]
                from_string = getattr(units.annotation, "from_string")
                set_value(float(value[0]), from_string(value[1]))
            else:
                set_value(float(cell.value))

        # Save out the patient
        patient_filename = output_dir / (patient.get_name()+".json")
        _pulse_logger.info(f"Writing patient file {patient_filename}")
        serialize_patient_to_file(patient, patient_filename)

    return True
