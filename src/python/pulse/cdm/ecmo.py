# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from enum import Enum
from pulse.cdm.enums import eSwitch
from pulse.cdm.equipment import SEEquipment
from pulse.cdm.substance import SESubstanceFraction, \
                                SESubstanceConcentration
from pulse.cdm.scalars import SEScalarFrequency, SEScalarPressureTimePerVolume, SEScalarVolumePerTime, \
                              SEScalarPressure, SEScalarTime, SEScalarVolume, SEScalarVolumePerPressure, \
                              MassPerVolumeUnit

class eECMO_CannulationLocation(Enum):
    NullCannulationLocation = 0
    InternalJugular = 1
    RightFemoralVein = 2
    LeftFemoralVein = 3
    RightSubclavianVein = 4
    LeftSubclavianVein = 5

class SEECMOSettings(SEEquipment):
    __slots__ = ["_inflow_location",
                 "_outflow_location",
                 "_oxygenator_volume",
                 "_transfusion_flow",
                 "_substance_compound",
                 "_substance_concentrations"]

    def __init__(self):
        super().__init__()
        self._inflow_location = eECMO_CannulationLocation.NullCannulationLocation
        self._outflow_location = eECMO_CannulationLocation.NullCannulationLocation
        self._oxygenator_volume = None
        self._transfusion_flow = None
        self._substance_compound = None
        self._substance_concentrations = []

    def clear(self):
        self._inflow_location = eECMO_CannulationLocation.NullCannulationLocation
        self._outflow_location = eECMO_CannulationLocation.NullCannulationLocation
        if self._oxygenator_volume is not None: self._oxygenator_volume.invalidate()
        if self._transfusion_flow is not None: self._transfusion_flow.invalidate()
        self._substance_compound = None
        self._substance_concentrations = []

    def copy(self, src):
        if not isinstance(SEECMOSettings, src):
            raise Exception("Provided argument must be a SEECMOSettings")
        self.clear()
        self._inflow_location = src._inflow_location
        self._outflow_location = src._outflow_location
        if src.has_oxygenator_volume(): self.get_oxygenator_volume().set(src._oxygenator_volume)
        if src.has_transfusion_flow(): self.get_transfusion_flow().set(src._transfusion_flow)
        self._substance_compound = src._substance_compound
        if src.has_substance_concentrations:
            self._substance_concentrations.append(src._substance_concentrations[:])

    def get_inflow_location(self):
        return self._inflow_location
    def set_inflow_location(self, t: eECMO_CannulationLocation):
        self._inflow_location = t
    def has_connection(self):
        return self._inflow_location != eECMO_CannulationLocation.NullCannulationLocation

    def get_outflow_location(self):
        return self._outflow_location
    def set_outflow_location(self, t: eECMO_CannulationLocation):
        self._outflow_location = t
    def has_outflow_location(self):
        return self._outflow_location != eECMO_CannulationLocation.NullCannulationLocation

    def has_oxygenator_volume(self):
        return False if self._oxygenator_volume is None else self._oxygenator_volume.is_valid()
    def get_oxygenator_volume(self):
        if self._oxygenator_volume is None:
            self._oxygenator_volume = SEScalarVolume()
        return self._oxygenator_volume

    def has_transfusion_flow(self):
        return False if self._transfusion_flow is None else self._transfusion_flow.is_valid()
    def get_transfusion_flow(self):
        if self._transfusion_flow is None:
            self._transfusion_flow = SEScalarVolumePerTime()
        return self._transfusion_flow

    def get_substance_compound(self):
        return self._substance_compound
    def set_substance_compound(self, sc: str):
        self._substance_compound = sc
    def has_substance_compound(self):
        return self._substance_compound != None

    def has_concentration_inspired_aerosol(self, substance_name: str=None):
        if substance_name is None:
            return len(self._concentration_inspired_aerosol) > 0
        else:
            for sf in self._concentration_inspired_aerosol:
                if sf.get_substance() is substance_name:
                    return True
        return False
    def get_substance_concentration(self, substance_name: str):
        for sf in self._substance_concentrations:
            if sf.get_substance() is substance_name:
                return sf
        sf = SESubstanceConcentration()
        sf.set_substance(substance_name)
        sf.get_concentration().set_value(0, MassPerVolumeUnit.kg_Per_m3)
        self._substance_concentrations.append(sf)
        return sf
    def get_substance_concentrations(self):
        return self._substance_concentrations
    def remove_substance_concentration(self, substance_name: str):
        for sf in self._substance_concentrations:
            if sf.get_substance() is substance_name:
                self._substance_concentrations.remove(sf)
    def remove_substance_concentrations(self):
        self._substance_concentrations = []