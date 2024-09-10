# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.
from pathlib import Path


def get_root_dir():
    return get_dir_from_run_config("root_dir")


def get_data_dir():
    return get_dir_from_run_config("data_dir")


def get_scenario_dir():
    return get_dir_from_run_config("scenario_dir")


def get_config_dir():
    return get_dir_from_run_config("test_config_dir")


def get_validation_dir():
    return get_dir_from_run_config("validation_dir")


def get_verification_dir():
    return get_dir_from_run_config("verification_dir")


def get_dir_from_run_config(key: str):
    key = key.lower()
    with open("run.config") as file:
        for line in file:
            if line.startswith(key):
                line = line.replace('\n', '')
                directory = line.split("=")
                return directory[1] + "/"
    raise ValueError(f'Could not find {key} directory.')


def adjust_filepath(fp: Path) -> Path:
    if "$VERIFICATION_DIR" in str(fp):
        fp = Path(str(fp).replace("$VERIFICATION_DIR", get_verification_dir()))
    return fp
