# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from enum import Enum
from pulse.cdm.enums import eSwitch
from pulse.cdm.equipment import SEEquipment
from pulse.cdm.substance import SESubstanceFraction, \
                                SESubstanceConcentration
from pulse.cdm.scalars import SEScalarPressureTimePerVolume, SEScalarVolumePerTime, \
                              SEScalarPressure, SEScalarTime, SEScalarVolume, \
                              MassPerVolumeUnit

class SEBagValveMask(SEEquipment):
    __slots__ = ["_connection",
                 "_bag_resistance",
                 "_connection_volume",
                 "_filter_resistance",
                 "_filter_volume",
                 "_seal_resistance",
                 "_valve_positive_end_expired_pressure",
                 "_valve_resistance",
                 "_valve_volume",

                 "_fraction_inspired_gasses",
                 "_concentration_inspired_aerosol"]

    def __init__(self):
        super().__init__()
        self._connection = eSwitch.NullSwitch
        self._bag_resistance = None
        self._connection_volume = None
        self._filter_resistance = None
        self._filter_volume = None
        self._seal_resistance = None
        self._valve_positive_end_expired_pressure = None
        self._valve_resistance = None
        self._valve_volume = None

        self._fraction_inspired_gasses = []
        self._concentration_inspired_aerosol = []

    def clear(self):
        self._connection = eSwitch.NullSwitch
        if self._bag_resistance is not None: self._bag_resistance.invalidate()
        if self._connection_volume is not None: self._connection_volume.invalidate()
        if self._filter_resistance is not None: self._filter_resistance.invalidate()
        if self._filter_volume is not None: self._filter_volume.invalidate()
        if self._seal_resistance is not None: self._bag_resistance.invalidate()
        if self._valve_positive_end_expired_pressure is not None: self._valve_positive_end_expired_pressure.invalidate()
        if self._valve_resistance is not None: self._valve_resistance.invalidate()
        if self._valve_volume is not None: self._valve_volume.invalidate()

        self._fraction_inspired_gasses = []
        self._concentration_inspired_aerosol = []

    def copy(self, src):
        if not isinstance(SEBagValveMask, src):
            raise Exception("Provided argument must be a SEBagValveMask")
        self.clear()
        self._connection = src._connection
        if src.has_bag_resistance(): self.get_bag_resistance().set(src._bag_resistance)
        if src.has_connection_volume(): self.get_connection_volume().set(src._connection_volume)
        if src.has_filter_resistance(): self.get_filter_resistance().set(src._filter_resistance)
        if src.has_filter_volume(): self.get_filter_volume().set(src._filter_volume)
        if src.has_seal_resistance(): self.get_seal_resistance().set(src._seal_resistance)
        if src.has_valve_positive_end_expired_pressure(): self.get_valve_positive_end_expired_pressure().set(src._valve_positive_end_expired_pressure)
        if src.has_valve_resistance(): self.get_valve_resistance().set(src._valve_resistance)
        if src.has_valve_volume(): self.get_valve_volume().set(src._valve_volume)

        if src.has_fraction_inspired_gasses:
            self._fraction_inspired_gasses.append(src._fraction_inspired_gasses[:])
        if src.has_concentration_inspired_aerosol:
            self._concentration_inspired_aerosol.append(src._concentration_inspired_aerosol[:])

    def get_connection(self):
        return self._connection
    def set_connection(self, t: eSwitch):
        self._connection = t

    def has_bag_resistance(self):
        return False if self._bag_resistance is None else self._bag_resistance.is_valid()
    def get_bag_resistance(self):
        if self._bag_resistance is None:
            self._bag_resistance = SEScalarPressureTimePerVolume()
        return self._bag_resistance

    def has_connection_volume(self):
        return False if self._connection_volume is None else self._connection_volume.is_valid()
    def get_connection_volume(self):
        if self._connection_volume is None:
            self._connection_volume = SEScalarVolume()
        return self._connection_volume

    def has_filter_resistance(self):
        return False if self._filter_resistance is None else self._filter_resistance.is_valid()
    def get_filter_resistance(self):
        if self._filter_resistance is None:
            self._filter_resistance = SEScalarPressureTimePerVolume()
        return self._filter_resistance
    
    def has_filter_volume(self):
        return False if self._filter_volume is None else self._filter_volume.is_valid()
    def get_filter_volume(self):
        if self._filter_volume is None:
            self._filter_volume = SEScalarVolume()
        return self._filter_volume

    def has_seal_resistance(self):
        return False if self._seal_resistance is None else self._seal_resistance.is_valid()
    def get_seal_resistance(self):
        if self._seal_resistance is None:
            self._seal_resistance = SEScalarPressureTimePerVolume()
        return self._seal_resistance
    
    def has_valve_positive_end_expired_pressure(self):
        return False if self._valve_positive_end_expired_pressure is None else self._valve_positive_end_expired_pressure.is_valid()
    def get_valve_positive_end_expired_pressure(self):
        if self._valve_positive_end_expired_pressure is None:
            self._valve_positive_end_expired_pressure = SEScalarPressure()
        return self._valve_positive_end_expired_pressure
    
    def has_valve_resistance(self):
        return False if self._valve_resistance is None else self._valve_resistance.is_valid()
    def get_valve_resistance(self):
        if self._valve_resistance is None:
            self._valve_resistance = SEScalarPressureTimePerVolume()
        return self._valve_resistance

    def has_valve_volume(self):
        return False if self._valve_volume is None else self._valve_volume.is_valid()
    def get_valve_volume(self):
        if self._valve_volume is None:
            self._valve_volume = SEScalarVolume()
        return self._valve_volume
        
    def has_fraction_inspired_gas(self, substance_name: str = None):
        if substance_name is None:
            return len(self._fraction_inspired_gasses) > 0
        else:
            for sf in self._fraction_inspired_gasses:
                if sf.get_substance() is substance_name:
                    return True
        return False
    def get_fraction_inspired_gas(self, substance_name: str):
        for sf in self._fraction_inspired_gasses:
            if sf.get_substance() is substance_name:
                return sf
        sf = SESubstanceFraction()
        sf.set_substance(substance_name)
        sf.get_fraction_amount().set_value(0)
        self._fraction_inspired_gasses.append(sf)
        return sf
    def get_fraction_inspired_gasses(self):
        return self._fraction_inspired_gasses
    def remove_fraction_inspired_gas(self, substance_name: str):
        for sf in self._fraction_inspired_gasses:
            if sf.get_substance() is substance_name:
                self._fraction_inspired_gasses.remove(sf)
    def remove_fraction_inspired_gasses(self):
        self._fraction_inspired_gasses = []

    def has_concentration_inspired_aerosol(self, substance_name: str=None):
        if substance_name is None:
            return len(self._concentration_inspired_aerosol) > 0
        else:
            for sf in self._concentration_inspired_aerosol:
                if sf.get_substance() is substance_name:
                    return True
        return False
    def get_concentration_inspired_aerosol(self, substance_name: str):
        for sf in self._concentration_inspired_aerosol:
            if sf.get_substance() is substance_name:
                return sf
        sf = SESubstanceConcentration()
        sf.set_substance(substance_name)
        sf.get_concentration().set_value(0, MassPerVolumeUnit.kg_Per_m3)
        self._concentration_inspired_aerosol.append(sf)
        return sf
    def get_concentration_inspired_aerosols(self):
        return self._concentration_inspired_aerosol
    def remove_concentration_inspired_aerosol(self, substance_name: str):
        for sf in self._concentration_inspired_aerosol:
            if sf.get_substance() is substance_name:
                self._concentration_inspired_aerosol.remove(sf)
    def remove_concentration_inspired_aerosol(self):
        self._concentration_inspired_aerosol = []
