# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.bag_valve_mask import SEBagValveMask
from pulse.cdm.bind.BagValveMask_pb2 import BagValveMaskData
from pulse.cdm.bind.Substance_pb2 import SubstanceFractionData, SubstanceConcentrationData

from pulse.cdm.io.scalars import *

def serialize_bag_valve_mask_to_bind(src: SEBagValveMask, dst: BagValveMaskData):
    dst.Connection = src.get_connection().value
    
    if src.has_bag_resistance():
        serialize_scalar_pressure_time_per_volume_to_bind(src.get_bag_resistance(), dst.BagResistance)
    if src.has_connection_volume():
        serialize_scalar_volume_to_bind(src.get_connection_volume()(), dst.ConnectionVolume)
    if src.has_filter_resistance():
        serialize_scalar_pressure_time_per_volume_to_bind(src.get_filter_resistance(), dst.FilterResistance)
    if src.has_filter_volume():
        serialize_scalar_volume_to_bind(src.get_filter_volume(), dst.FilterVolume)
    if src.has_seal_resistance():
        serialize_scalar_pressure_time_per_volume_to_bind(src.get_seal_resistance(), dst.SealResistance)
    if src.has_valve_positive_end_expired_pressure():
        serialize_scalar_pressure_to_bind(src.get_valve_positive_end_expired_pressure(), dst.ValvePositiveEndExpiratoryPressure)
    if src.has_valve_resistance():
        serialize_scalar_pressure_time_per_volume_to_bind(src.get_valve_resistance(), dst.ValveResistance)
    if src.has_valve_volume():
        serialize_scalar_volume_to_bind(src.get_valve_volume(), dst.ValveVolume)

    for aGas in src.get_fraction_inspired_gasses():
        sf = SubstanceFractionData()
        sf.Name = aGas.get_substance()
        serialize_scalar_0to1_to_bind(aGas.get_fraction_amount(), sf.Amount)
        dst.FractionInspiredGas.append(sf)

    for aAerosol in src.get_concentration_inspired_aerosols():
        sc = SubstanceConcentrationData()
        sc.Name = aAerosol.get_substance()
        serialize_scalar_mass_per_volume_to_bind(aAerosol.get_concentration(), sc.Concentration)
        dst.ConcentrationInspiredAerosol.append(sc)

def serialize_bag_valve_mask_from_bind(src: BagValveMaskData, dst: SEBagValveMask):
    raise Exception("serialize_bag_valve_mask_from_bind not implemented")