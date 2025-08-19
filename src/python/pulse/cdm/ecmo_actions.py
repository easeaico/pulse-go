# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.equipment_actions import SEEquipmentAction
from pulse.cdm.ecmo import SEECMOSettings, eSwitch
from pulse.cdm.scalars import SEScalarPressure, SEScalar0To1, SEScalarTime, \
                              SEScalarFrequency, SEScalarVolumePerTime, SEScalarVolume
from enum import Enum


class SEECMOAction(SEEquipmentAction):
    def __init__(self):
        super().__init__()


class SEECMOConfiguration(SEECMOAction):
    __slots__ = ["_settings_file",
                 "_settings"]

    def __init__(self):
        super().__init__()
        self._settings_file = None
        self._settings = None

    def clear(self):
        self._settings_file = None
        self._settings = None

    def copy(self, src):
        if not isinstance(SEECMOConfiguration, src):
            raise Exception("Provided argument must be a SEECMOConfiguration")
        self.clear()
        self._settings_file = src._settings_file
        self._settings.copy(src._settings)

    def is_valid(self):
        return self.has_settings() or self.has_settings_file()

    def is_active(self):
        return True

    def has_settings_file(self):
        return self._settings_file is not None
    def get_settings_file(self):
        return self._settings_file
    def set_settings_file(self, filename: str):
        self._settings_file = filename

    def has_settings(self):
        return self._settings is not None
    def get_settings(self):
        if self._settings is None:
            self._settings = SEECMOSettings()
        return self._settings
