# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.pipelines.segment_validation import *
from pulse.pipelines.dataset.timeseries_dataset_reader import *
from pulse.pipelines.dataset.utils import generate_data_request
from pulse.cdm.io.engine import serialize_data_request_list_to_file

if __name__ == "__main__":

    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
    logging.getLogger("pycel").setLevel(logging.WARNING)

    # Generate Data Requests
    xls_file = Path(get_validation_dir()) / "SystemValidationData.xlsx"
    generate_data_requests(xls_file, Path("./validation/requests"))

    patient_drs = [
        generate_data_request("Patient", "Weight", "kg", 1),
        generate_data_request("Patient", "IdealBodyWeight", "kg", 1),
        generate_data_request("Patient", "BasalMetabolicRate", "kcal/day", 1),
        generate_data_request("Patient", "BloodVolumeBaseline", "mL", 1),
        generate_data_request("Patient", "DiastolicArterialPressureBaseline", "mmHg", 1),
        generate_data_request("Patient", "HeartRateBaseline", "1/min", 0),
        generate_data_request("Patient", "MeanArterialPressureBaseline", "mmHg", 1),
        generate_data_request("Patient", "RespirationRateBaseline", "1/min", 0),
        generate_data_request("Patient", "SystolicArterialPressureBaseline", "mmHg", 1),
        generate_data_request("Patient", "TidalVolumeBaseline", "mL", 1),
        generate_data_request("Patient", "ExpiratoryReserveVolume", "mL", 1),
        generate_data_request("Patient", "FunctionalResidualCapacity", "mL", 1),
        generate_data_request("Patient", "InspiratoryCapacity", "mL", 1),
        generate_data_request("Patient", "InspiratoryReserveVolume", "mL", 1),
        generate_data_request("Patient", "ResidualVolume", "mL", 1),
        generate_data_request("Patient", "TotalLungCapacity", "mL", 1),
        generate_data_request("Patient", "VitalCapacity", "mL", 1),

        generate_data_request("Physiology", "ArterialPressure", "mmHg", 1),
        generate_data_request("Physiology", "RespiratoryMusclePressure", "cmH2O", 4),
    ]
    logging.info(f"Writing validation\\requests\\Patient.json")
    serialize_data_request_list_to_file(patient_drs, Path("./validation/requests/Patient.json"))

    # In the future, we will do all xlsx in the data/validation dir
    # But for now, we are just hard coding the automated xlsx files
    xls_files = ["Hemorrhage.xlsx", "AirwayObstruction.xlsx", "Dehydration.xlsx", "MechanicalVentilator.xlsx", "CSTARS.xlsx"]

    for xls_file in xls_files:
        segment_validation_pipeline(
            xls_file=Path(xls_file),
            exec_opt=eExecOpt.GenerateOnly
        )
