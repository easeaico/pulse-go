# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import logging

from typing import Optional

from pulse.cdm.engine import SEDataRequest, eDataRequest_category
from pulse.cdm.scalars import get_unit

_pulse_logger = logging.getLogger('pulse')

def generate_data_request(request_type: str, property_name: str, unit_str: str, precision: Optional[int]) -> SEDataRequest:
    request_type = request_type.strip()
    if request_type == "ActionCmpt" or request_type == "ActionSub":
        category = eDataRequest_category.Action
    else:
        category = eDataRequest_category[request_type.strip()]
    action = None
    compartment = None
    substance = None
    unit = None
    if unit_str.strip().lower() != "unitless":
        try:
            unit = get_unit(unit_str.strip())
        except:
            _pulse_logger.warning(f"Could not identify unit: {unit_str}")
            unit = None

    props = property_name.strip().split("-")

    if category == eDataRequest_category.Patient:
        property = property_name
    elif category == eDataRequest_category.Physiology:
        property = property_name
    elif category == eDataRequest_category.Environment:
        property = property_name
    elif category == eDataRequest_category.Action:
        if request_type == "ActionCmpt":
            action = props[0]
            compartment = props[1]
            property = props[2]
        elif request_type == "ActionSub":
            action = props[0]
            substance = props[1]
            property = props[2]
        else:
            action = props[0]
            property = props[1]
    elif category == eDataRequest_category.GasCompartment:
        compartment = props[0]
        if len(props) == 2:
            property = props[1]
        elif len(props) == 3:
            substance = props[1]
            property = props[2]
        else:
            raise ValueError(f"Invalid property name for GasCompartment Data Request: {property_name}")
    elif category == eDataRequest_category.LiquidCompartment:
        compartment = props[0]
        if len(props) == 2:
            property = props[1]
        elif len(props) == 3:
            substance = props[1]
            property = props[2]
        else:
            raise ValueError(f"Invalid property name for LiquidCompartment Data Request: {property_name}")
    elif category == eDataRequest_category.ThermalCompartment:
        compartment = props[0]
        property = props[1]
    elif category == eDataRequest_category.TissueCompartment:
        compartment = props[0]
        if len(props) == 2:
            property = props[1]
        elif len(props) == 3:
            substance = props[1]
            property = props[2]
        else:
            raise ValueError(f"Invalid property name for TissueCompartment Data Request: {property_name}")
    elif category == eDataRequest_category.Substance:
        substance = props[0]
        property = "-".join(props[1:])
    elif category == eDataRequest_category.AnesthesiaMachine:
        property = property_name
    elif category == eDataRequest_category.BagValveMask:
        property = property_name
    elif category == eDataRequest_category.ECG:
        property = property_name
    elif category == eDataRequest_category.ECMO:
        property = property_name
    elif category == eDataRequest_category.Inhaler:
        property = property_name
    elif category == eDataRequest_category.MechanicalVentilator:
        property = property_name
    else:
        raise ValueError(f"Unknown data request category: {request_type}")

    return SEDataRequest(
        category=category,
        action=action,
        compartment=compartment,
        substance=substance,
        property=property,
        unit=unit,
        precision=precision
    )
