# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from enum import Enum
from pulse.cdm.enums import eSwitch
from pulse.cdm.scalars import SEScalar0To1, SEScalarArea, \
                              SEScalarHeatResistanceArea, SEScalarLengthPerTime, \
                              SEScalarMassPerVolume, MassPerVolumeUnit, SEScalarPower, \
                              SEScalarPressure, SEScalarTemperature, SEScalarVolume
from pulse.cdm.substance import SESubstanceConcentration, \
                                SESubstanceFraction

class SEAppliedTemperature():
    __slots__ = ["_temperature", "_state",
                 "_surface_area", "_surface_area_fraction"]

    def __init__(self):
        self._state = eSwitch.Off
        self._temperature = None
        self._surface_area = None
        self._surface_area_fraction = None

    def clear(self):
        self._state = eSwitch.Off
        if self._temperature is not None: self._temperature.invalidate()
        if self._surface_area is not None: self._surface_area.invalidate()
        if self._surface_area_fraction is not None: self._surface_area_fraction.invalidate()

    def copy(self, src):
        if not isinstance(SEAppliedTemperature, src):
            raise Exception("Provided argument must be a SEAppliedTemperature")
        self.clear()
        self._state = src._state
        if src.has_temperature(): self.get_temperature().set(src._temperature)
        if src.has_surface_area(): self.get_surface_area().set(src._surface_area)
        if src.has_surface_area_fraction(): self.get_surface_area_fraction().set(src._surface_area_fraction)

    def get_state(self):
        return self._state
    def set_state(self, t: eSwitch):
        self._state = t

    def has_temperature(self):
        return False if self._temperature is None else self._temperature.is_valid()
    def get_temperature(self):
        if self._temperature is None:
            self._temperature = SEScalarTemperature()
        return self._temperature

    def has_surface_area(self):
        return False if self._surface_area is None else self._surface_area.is_valid()
    def get_surface_area(self):
        if self._surface_area is None:
            self._surface_area = SEScalarArea()
        return self._surface_area

    def has_surface_area_fraction(self):
        return False if self._surface_area_fraction is None else self._surface_area_fraction.is_valid()
    def get_surface_area_fraction(self):
        if self._surface_area_fraction is None:
            self._surface_area_fraction = SEScalar0To1()
        return self._surface_area_fraction

class SEActiveConditioning():
    __slots__ = ["_power",
                 "_surface_area",
                 "_surface_area_fraction"]

    def __init__(self):
        self._power = None
        self._surface_area = None
        self._surface_area_fraction = None

    def clear(self):
        if self._power is not None: self._power.invalidate()
        if self._surface_area is not None: self._surface_area.invalidate()
        if self._surface_area_fraction is not None: self._surface_area_fraction.invalidate()

    def copy(self, src):
        if not isinstance(SEActiveConditioning, src):
            raise Exception("Provided argument must be a SEActiveConditioning")
        self.clear()
        if src.has_power(): self.get_power().set(src._power)
        if src.has_surface_area(): self.get_surface_area().set(src._surface_area)
        if src.has_surface_area_fraction(): self.get_surface_area_fraction().set(src._surface_area_fraction)

    def has_power(self):
        return False if self._power is None else self._power.is_valid()
    def get_power(self):
        if self._power is None:
            self._power = SEScalarPower()
        return self._power

    def has_surface_area(self):
        return False if self._surface_area is None else self._surface_area.is_valid()
    def get_surface_area(self):
        if self._surface_area is None:
            self._surface_area = SEScalarArea()
        return self._surface_area

    def has_surface_area_fraction(self):
        return False if self._surface_area_fraction is None else self._surface_area_fraction.is_valid()
    def get_surface_area_fraction(self):
        if self._surface_area_fraction is None:
            self._surface_area_fraction = SEScalar0To1()
        return self._surface_area_fraction

class eSurroundingType(Enum):
    NullSurrounding = 0
    Air = 1
    Water = 2
class SEEnvironmentalConditions():
    __slots__ = ["_surrounding_type", "_air_density", "_air_velocity",
                 "_ambient_temperature", "_atmospheric_pressure", "_clothing_resistance",
                 "_emissivity", "_mean_radiant_temperature", "_mechanical_dead_space", "_relative_humidity",
                 "_respiration_ambient_temperature", "_ambient_gasses", "_ambient_aerosols"]

    def __init__(self):
        self._surrounding_type = eSurroundingType.NullSurrounding
        self._air_density = None
        self._air_velocity = None
        self._ambient_temperature = None
        self._atmospheric_pressure = None
        self._clothing_resistance = None
        self._emissivity = None
        self._mean_radiant_temperature = None
        self._mechanical_dead_space = None
        self._relative_humidity = None
        self._respiration_ambient_temperature = None
        self._ambient_gasses = []
        self._ambient_aerosols = []

    def clear(self):
        self._sex = eSurroundingType.NullSurrounding
        if self._air_density is not None: self._air_density.invalidate()
        if self._air_velocity is not None: self._air_velocity.invalidate()
        if self._ambient_temperature is not None: self._ambient_temperature.invalidate()
        if self._atmospheric_pressure is not None: self._atmospheric_pressure.invalidate()
        if self._clothing_resistance is not None: self._clothing_resistance.invalidate()
        if self._emissivity is not None: self._emissivity.invalidate()
        if self._mean_radiant_temperature is not None: self._mean_radiant_temperature.invalidate()
        if self._mechanical_dead_space is not None: self._mechanical_dead_space.invalidate()
        if self._relative_humidity is not None: self._relative_humidity.invalidate()
        if self._respiration_ambient_temperature is not None: self._respiration_ambient_temperature.invalidate()
        self._ambient_gasses = []
        self._ambient_aerosols = []

    def copy(self, src):
        if not isinstance(SEEnvironmentalConditions, src):
            raise Exception("Provided argument must be a SEEnvironmentalConditions")
        self.clear()
        self._surrounding_type = src._surrounding_type
        if src.has_air_density(): self.get_air_density().set(src._air_density)
        if src.has_air_velocity(): self.get_air_velocity().set(src._air_velocity)
        if src.has_ambient_temperature(): self.get_ambient_temperature().set(src._ambient_temperature)
        if src.has_atmospheric_pressure(): self.get_atmospheric_pressure().set(src._atmospheric_pressure)
        if src.has_clothing_resistance(): self.get_clothing_resistance().set(src._clothing_resistance)
        if src.has_emissivity(): self.get_emissivity().set(src._emissivity)
        if src.has_mean_radiant_temperature(): self.get_mean_radiant_temperature().set(src._mean_radiant_temperature)
        if src.has_mechanical_dead_space(): self.get_mechanical_dead_space().set(src._mechanical_dead_space)
        if src.has_relative_humidity(): self.get_relative_humidity().set(src._relative_humidity)
        if src.has_respiration_ambient_temperature(): self.get_respiration_ambient_temperature().set(src._respiration_ambient_temperature)

        if src.has_ambient_gasses:
            self._ambient_gasses.append(src._ambient_gasses[:])
        if src.has__ambient_aerosols:
            self._ambient_aerosols.append(src._ambient_aerosols[:])


    def get_surrounding_type(self):
        return self._surrounding_type
    def set_surrounding_type(self, t: eSurroundingType):
        self._surrounding_type = t

    def has_air_density(self):
        return False if self._air_density is None else self._air_density.is_valid()
    def get_air_density(self):
        if self._air_density is None:
            self._air_density = SEScalarMassPerVolume()
        return self._air_density

    def has_air_velocity(self):
        return False if self._air_velocity is None else self._air_velocity.is_valid()
    def get_air_velocity(self):
        if self._air_velocity is None:
            self._air_velocity = SEScalarLengthPerTime()
        return self._air_velocity

    def has_ambient_temperature(self):
        return False if self._ambient_temperature is None else self._ambient_temperature.is_valid()
    def get_ambient_temperature(self):
        if self._ambient_temperature is None:
            self._ambient_temperature = SEScalarTemperature()
        return self._ambient_temperature

    def has_atmospheric_pressure(self):
        return False if self._atmospheric_pressure is None else self._atmospheric_pressure.is_valid()
    def get_atmospheric_pressure(self):
        if self._atmospheric_pressure is None:
            self._atmospheric_pressure = SEScalarPressure()
        return self._atmospheric_pressure

    def has_clothing_resistance(self):
        return False if self._clothing_resistance is None else self._clothing_resistance.is_valid()
    def get_clothing_resistance(self):
        if self._clothing_resistance is None:
            self._clothing_resistance = SEScalarHeatResistanceArea()
        return self._clothing_resistance

    def has_emissivity(self):
        return False if self._emissivity is None else self._emissivity.is_valid()
    def get_emissivity(self):
        if self._emissivity is None:
            self._emissivity = SEScalar0To1()
        return self._emissivity

    def has_mean_radiant_temperature(self):
        return False if self._mean_radiant_temperature is None else self._mean_radiant_temperature.is_valid()
    def get_mean_radiant_temperature(self):
        if self._mean_radiant_temperature is None:
            self._mean_radiant_temperature = SEScalarTemperature()
        return self._mean_radiant_temperature

    def has_mechanical_dead_space(self):
        return False if self._mechanical_dead_space is None else self._mechanical_dead_space.is_valid()
    def get_mechanical_dead_space(self):
        if self._mechanical_dead_space is None:
            self._mechanical_dead_space = SEScalarVolume()
        return self._mechanical_dead_space

    def has_relative_humidity(self):
        return False if self._relative_humidity is None else self._relative_humidity.is_valid()
    def get_relative_humidity(self):
        if self._relative_humidity is None:
            self._relative_humidity = SEScalar0To1()
        return self._relative_humidity

    def has_respiration_ambient_temperature(self):
        return False if self._respiration_ambient_temperature is None else self._respiration_ambient_temperature.is_valid()
    def get_respiration_ambient_temperature(self):
        if self._respiration_ambient_temperature is None:
            self._respiration_ambient_temperature = SEScalarTemperature()
        return self._respiration_ambient_temperature

    def has_ambient_gas(self, substance_name: str=None):
        if substance_name is None:
            return len(self._ambient_gasses) > 0
        else:
            for sf in self._ambient_gasses:
                if sf.get_substance() is substance_name:
                    return True
        return False
    def get_ambient_gas(self, substance_name: str):
        for sf in self._ambient_gasses:
            if sf.get_substance() is substance_name:
                return sf
        sf = SESubstanceFraction()
        sf.set_substance(substance_name)
        sf.get_fraction_amount().set_value(0)
        self._ambient_gasses.append(sf)
        return sf
    def get_ambient_gasses(self):
        return self._ambient_gasses
    def remove_ambient_gas(self, substance_name: str):
        for sf in self._ambient_gasses:
            if sf.get_substance() is substance_name:
                self._ambient_gasses.remove(sf)
    def remove_ambient_gasses(self):
        self._ambient_gasses = []

    def has_ambient_aerosol(self, substance_name: str=None):
        if substance_name is None:
            return len(self._ambient_aerosols) > 0
        else:
            for sf in self._ambient_aerosols:
                if sf.get_substance() is substance_name:
                    return True
        return False
    def get_ambient_aerosol(self, substance_name: str):
        for sf in self._ambient_aerosols:
            if sf.get_substance() is substance_name:
                return sf
        sf = SESubstanceConcentration()
        sf.set_substance(substance_name)
        sf.get_concentration().set_value(0, MassPerVolumeUnit.kg_Per_m3)
        self._ambient_aerosols.append(sf)
        return sf
    def get_ambient_aerosols(self):
        return self._ambient_aerosols
    def remove_ambient_aerosol(self, substance_name: str):
        for sf in self._ambient_aerosols:
            if sf.get_substance() is substance_name:
                self._ambient_aerosols.remove(sf)
    def remove_ambient_aerosols(self):
        self._ambient_aerosols = []
