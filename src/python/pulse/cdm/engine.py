# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from enum import Enum
from dataclasses import dataclass
from abc import ABC, abstractmethod
from typing import List, Optional, Union

from pulse.cdm.scalars import SEScalarTime, SEScalarUnit, TimeUnit


class eEngineInitializationState(Enum):
    Uninitialized = 0
    FailedState = 1
    FailedPatientSetup = 2
    FailedStabilization = 3
    Initialized = 4


class eSerializationFormat(Enum):
    JSON = 0
    BINARY = 1
    VERBOSE_JSON = 2
    TEXT = 3


class eSide(Enum):
    NullSide = 0
    Left = 1
    Right = 2


class eGate(Enum):
    NullGate = 0
    Open = 1
    Closed = 2


class eSwitch(Enum):
    NullSwitch = 0
    Off = 1
    On = 2


class eCharge(Enum):
    NullCharge = 0
    Negative = 1
    Neutral = 2
    Positive = 3


class eEvent(Enum):
    Antidiuresis = 0
    Bradycardia = 1
    Bradypnea = 2
    BrainOxygenDeficit = 3
    CardiacArrest = 4
    CardiogenicShock = 5
    CardiovascularCollapse = 6
    CriticalBrainOxygenDeficit = 7
    Diuresis = 8
    Fasciculation = 9
    Fatigue = 10
    FunctionalIncontinence = 11
    Hypercapnia = 12
    Hyperglycemia = 13
    Hypernatremia = 14
    Hyperthermia = 15
    Hypoglycemia = 16
    Hyponatremia = 17
    Hypothermia = 18
    Hypoxia = 19
    HypovolemicShock = 20
    IntracranialHypertension = 21
    IntracranialHypotension = 22
    IrreversibleState = 23
    Ketoacidosis = 24
    LacticAcidosis = 25
    MassiveHemothorax = 26
    MaximumPulmonaryVentilationRate = 27
    MediumHemothorax = 28
    MetabolicAcidosis = 29
    MetabolicAlkalosis = 30
    MildDehydration = 31
    MinimalHemothorax = 32
    ModerateDehydration = 33
    ModerateHyperoxemia = 34
    ModerateHypocapnia = 35
    MyocardiumOxygenDeficit = 36
    Natriuresis = 37
    NutritionDepleted = 38
    RenalHypoperfusion = 39
    RespiratoryAcidosis = 40
    RespiratoryAlkalosis = 41
    SevereDehydration = 42
    SevereHyperoxemia = 43
    SevereHypocapnia = 44
    Stabilizing = 45
    StartOfCardiacCycle = 46
    StartOfExhale = 47
    StartOfInhale = 48
    Tachycardia = 49
    Tachypnea = 50

    # Equipment
    AnesthesiaMachineOxygenBottleOneExhausted = 1000
    AnesthesiaMachineOxygenBottleTwoExhausted = 1001
    AnesthesiaMachineReliefValveActive = 1002
    MechanicalVentilatorReliefValveActive = 1003
    SupplementalOxygenBottleExhausted = 1004
    NonRebreatherMaskOxygenBagEmpty = 1005


class SEEventChange:
    __slots__ = ["event", "active", "sim_time"]

    def __init__(
        self,
        event: Optional[eEvent]=None,
        active: Optional[bool]=None,
        sim_time_s: Optional[float]=None
    ):
        self.event = event
        self.active = active
        self.sim_time = SEScalarTime(sim_time_s, TimeUnit.s) if sim_time_s is not None else SEScalarTime()

    def __repr__(self) -> str:
        return_text = ("{} is {}").format(self.event, "Active" if self.active else "Inactive")
        if self.sim_time.is_valid():
            return_text += (" @ {}").format(self.sim_time)
        return return_text

class IEventHandler:
    def __init__(self, active_events_only=False):
        self._active = active_events_only
    def handle_event(self, change: SEEventChange):
        pass


class SEAction(ABC):
    __slots__ = ["_comment"]

    def __init__(self):
        self._comment = None

    def copy(self, action):
        self.clear()
        self._comment = action._comment

    def clear(self):
        self.invalidate_comment()

    def has_comment(self):
        return self._comment is not None
    def get_comment(self):
        return self._comment
    def set_comment(self, comment: str):
        self._comment = comment
    def invalidate_comment(self):
        self._comment = None

    @abstractmethod
    def is_valid(self):
        pass


class SEAdvanceTime(SEAction):
    __slots__ = ["_action","_time"]

    def __init__(self):
        super().__init__()
        self._time = None

    def clear(self):
        super().clear()
        if self._time is not None:
            self._time.invalidate()

    def is_valid(self):
        return self.has_time()

    def has_time(self):
        return self._time is not None

    def get_time(self):
        if self._time is None:
            self._time = SEScalarTime()
        return self._time

    def __repr__(self):
        return ("Advance Time\n"
                "  Time: {}").format(self._time)


class SEAdvanceUntilStable(SEAction):
    __slots__ = ["_criteria"]

    def __init__(self) -> None:
        super().__init__()
        self._criteria = ""

    def clear(self) -> None:
        super().clear()
        if self._criteria is not None:
            self.invalidate_criteria()

    def is_valid(self) -> bool:
        return self.has_criteria()

    def has_criteria(self) -> bool:
        return self._criteria != ""
    def get_criteria(self) -> str:
        return self._criteria
    def set_criteria(self, c: str) -> None:
        self._criteria = c.replace('\\', '/')
    def invalidate_criteria(self) -> None:
        self._criteria = ""

    def __repr__(self) -> str:
        return ("Advance Until Stable\n"
                "  Criteria: {}").format(self._criteria)


class SECondition(ABC):
    __slots__ = ["_comment"]

    def __init__(self):
        self._comment = None

    def copy(self, condition):
        self.clear()
        self._comment = condition._comment

    def clear(self):
        self.invalidate_comment()

    @abstractmethod
    def get_name(self):
        pass

    def has_comment(self):
        return self._comment is not None
    def get_comment(self):
        return self._comment
    def set_comment(self, comment: str):
        self._comment = comment
    def invalidate_comment(self):
        self._comment = None

    @abstractmethod
    def is_valid(self):
        pass

    @abstractmethod
    def is_active(self):
        pass

from pulse.cdm.environment_conditions import SEInitialEnvironmentalConditions
from pulse.cdm.patient_conditions import *

class SEConditionManager():
    __slots__ = ["_ards", "_anemia", "_copd", "_cvsd", "_impaired_alveolar_exchange",
                 "_dehydration", "_pericardial_effusion", "_pneumonia",
                 "_pulmonary_fibrosis", "_pulmonary_shunt", "_renal_stenosis", "_sepsis",
                 "_initial_environmental_conditions"]

    def __init__(self):
        self.clear()

    def clear(self):
        self._ards = None
        self._anemia = None
        self._copd = None
        self._cvsd = None
        self._dehydration = None
        self._impaired_alveolar_exchange = None
        self._pericardial_effusion = None
        self._pneumonia = None
        self._pulmonary_fibrosis = None
        self._pulmonary_shunt = None
        self._renal_stenosis = None
        self._sepsis = None
        self._initial_environmental_conditions = None

    def is_empty(self):
        if self.has_acute_respiratory_distress_syndrome():
            return False
        if self.has_chronic_anemia():
            return False
        if self.has_chronic_obstructive_pulmonary_disease():
            return False
        if self.has_chronic_ventricular_systolic_dysfunction():
            return False
        if self.has_chronic_pericardial_effusion():
            return False
        if self.has_chronic_renal_stenosis():
            return False
        if self.has_dehydration():
            return False
        if self.has_impaired_alveolar_exchange():
            return False
        if self.has_pneumonia():
            return False
        if self.has_pulmonary_fibrosis():
            return False
        if self.has_pulmonary_shunt():
            return False
        if self.has_sepsis():
            return False

        if self.has_initial_environmental_conditions():
            return False;
        return True;

    def has_acute_respiratory_distress_syndrome(self):
        return False if self._ards is None else self._ards.is_valid()
    def get_acute_respiratory_distress_syndrome(self):
        if self._ards is None:
            self._ards = SEAcuteRespiratoryDistressSyndrome()
        return self._ards
    def remove_acute_respiratory_distress_syndrome(self):
        self._ards = None

    def has_chronic_anemia(self):
        return False if self._anemia is None else self._anemia.is_valid()
    def get_chronic_anemia(self):
        if self._anemia is None:
            self._anemia = SEChronicAnemia()
        return self._anemia
    def remove_chronic_anemia(self):
        self._anemia = None

    def has_chronic_obstructive_pulmonary_disease(self):
        return False if self._copd is None else self._copd.is_valid()
    def get_chronic_obstructive_pulmonary_disease(self):
        if self._copd is None:
            self._copd = SEChronicObstructivePulmonaryDisease()
        return self._copd
    def remove_chronic_obstructive_pulmonary_disease(self):
        self._copd = None

    def has_chronic_ventricular_systolic_dysfunction(self):
        return False if self._cvsd is None else self._cvsd.is_valid()
    def get_chronic_ventricular_systolic_dysfunction(self):
        if self._cvsd is None:
            self._cvsd = SEChronicVentricularSystolicDysfunction()
        return self._cvsd
    def remove_chronic_ventricular_systolic_dysfunction(self):
        self._cvsd = None

    def has_chronic_pericardial_effusion(self):
        return False if self._pericardial_effusion is None else self._pericardial_effusion.is_valid()
    def get_chronic_pericardial_effusion(self):
        if self._pericardial_effusion is None:
            self._pericardial_effusion = SEChronicPericardialEffusion()
        return self._pericardial_effusion
    def remove_chronic_pericardial_effusion(self):
        self._pericardial_effusion = None

    def has_chronic_renal_stenosis(self):
        return False if self._renal_stenosis is None else self._renal_stenosis.is_valid()
    def get_chronic_renal_stenosis(self):
        if self._renal_stenosis is None:
            self._renal_stenosis = SEChronicRenalStenosis()
        return self._renal_stenosis
    def remove_chronic_renal_stenosis(self):
        self._renal_stenosis = None

    def has_dehydration(self):
        return False if self._dehydration is None else self._dehydration.is_valid()
    def get_dehydration(self):
        if self._dehydration is None:
            self._dehydration = SEDehydration()
        return self._dehydration
    def remove_dehydration(self):
        self._dehydration = None

    def has_impaired_alveolar_exchange(self):
        return False if self._impaired_alveolar_exchange is None else self._impaired_alveolar_exchange.is_valid()
    def get_impaired_alveolar_exchange(self):
        if self._impaired_alveolar_exchange is None:
            self._impaired_alveolar_exchange = SEImpairedAlveolarExchange()
        return self._impaired_alveolar_exchange
    def remove_impaired_alveolar_exchange(self):
        self._impaired_alveolar_exchange = None

    def has_pneumonia(self):
        return False if self._pneumonia is None else self._pneumonia.is_valid()
    def get_pneumonia(self):
        if self._pneumonia is None:
            self._pneumonia = SEPneumonia()
        return self._pneumonia
    def remove_pneumonia(self):
        self._pneumonia = None

    def has_pulmonary_fibrosis(self):
        return False if self._pulmonary_fibrosis is None else self._pulmonary_fibrosis.is_valid()
    def get_pulmonary_fibrosis(self):
        if self._pulmonary_fibrosis is None:
            self._pulmonary_fibrosis = SEPulmonaryFibrosis()
        return self._pulmonary_fibrosis
    def remove_pulmonary_fibrosis(self):
        self._pulmonary_fibrosis = None

    def has_pulmonary_shunt(self):
        return False if self._pulmonary_shunt is None else self._pulmonary_shunt.is_valid()
    def get_pulmonary_shunt(self):
        if self._pulmonary_shunt is None:
            self._pulmonary_shunt = SEPulmonaryShunt()
        return self._pulmonary_shunt
    def remove_pulmonary_shunt(self):
        self._pulmonary_shunt = None

    def has_sepsis(self):
        return False if self._sepsis is None else self._sepsis.is_valid()
    def get_sepsis(self):
        if self._sepsis is None:
            self._sepsis = SESepsis()
        return self._sepsis
    def remove_sepsis(self):
        self._sepsis = None

    def has_initial_environmental_conditions(self):
        return False if self._initial_environmental_conditions is None else self._initial_environmental_conditions.is_valid()
    def get_initial_environmental_conditions(self):
        if self._initial_environmental_conditions is None:
            self._initial_environmental_conditions = SEInitialEnvironmentalConditions()
        return self._initial_environmental_conditions
    def remove_initial_environmental_conditions(self):
        self._initial_environmental_conditions = None


class eDecimalFormat_type(Enum):
    SystemFormatting = 0
    DefaultFloat = 1
    FixedMantissa = 2
    SignificantDigits = 3


class SEDecimalFormat():
    __slots__ = ["_precision", "_notation"]

    def __init__(self, precision: Optional[int]=None, notation: Optional[eDecimalFormat_type]=None) -> None:
        self.clear()
        self._precision = precision
        self._notation = notation

    def clear(self) -> None:
        self._precision = None
        self._notation = None

    def set_precision(self, p: int) -> None:
        self._precision = p
    def get_precision(self) -> Union[int, None]:
        return self._precision
    def has_precision(self) -> bool:
        return self._precision is not None

    def set_notation(self, n: eDecimalFormat_type) -> None:
        self._notation = n
    def get_notation(self) -> Union[eDecimalFormat_type, None]:
        return self._notation
    def has_notation(self) -> bool:
        return self._notation is not None


class eDataRequest_category(Enum):
    Patient = 0
    Physiology = 1
    Environment = 2
    Action = 3
    GasCompartment = 4
    LiquidCompartment = 5
    ThermalCompartment = 6
    TissueCompartment = 7
    Substance = 8
    AnesthesiaMachine = 9
    BagValveMask = 10
    ECG = 11
    ECMO = 12
    Inhaler = 13
    MechanicalVentilator = 14

class SEDataRequest(SEDecimalFormat):
    __slots__ = ['_category', '_action_name', '_compartment_name', '_substance_name', '_property_name', '_unit']

    def __init__(
        self, category: eDataRequest_category, action:Optional[str]=None, compartment:Optional[str]=None,
        substance:Optional[str]=None, property:Optional[str]=None, unit:Optional[SEScalarUnit]=None,
        precision: Optional[int]=None, notation: Optional[eDecimalFormat_type]=None
    ):
        super().__init__(precision, notation)
        if category is None:
            raise Exception("Must provide a Data Request Category")
        if property is None:
            raise Exception("Must provide a Data Request Property Name")
        if (action is None and category is eDataRequest_category.Action):
            raise Exception("Must provide an Action Name for Action Data Requests");
        if (compartment is None and (category is eDataRequest_category.GasCompartment or
                                     category is eDataRequest_category.LiquidCompartment or
                                     category is eDataRequest_category.ThermalCompartment or
                                     category is eDataRequest_category.TissueCompartment)):
            raise Exception("Must provide a Compartment Name for Compartment Data Requests")
        if (substance is None and category is eDataRequest_category.Substance):
            raise Exception("Must provide a Substance Name for Substance Data Requests")
        self._category = category
        self._action_name = action
        self._compartment_name = compartment
        self._substance_name = substance
        self._property_name = property
        if unit is None:
            self._unit = None
        else:
            self._unit = unit.get_string()

    def __repr__(self) -> str:
        out_string = ""
        if self._category == eDataRequest_category.Action:
            out_string = self._action_name+"-"
            if self.has_compartment_name():
                out_string += "{}-".format(self._compartment_name)
            elif self.has_substance_name():
                out_string += "{}-".format(self._substance_name)
        elif self._category == eDataRequest_category.Patient:
            out_string = "Patient-"
        elif self._category == eDataRequest_category.AnesthesiaMachine:
            out_string = "AnesthesiaMachine-"
        elif self._category == eDataRequest_category.BagValveMask:
            out_string = "BagValveMask-"
        elif self._category == eDataRequest_category.ECG:
            out_string = "ECG-"
        elif self._category == eDataRequest_category.ECMO:
            out_string = "ECMO-"
        elif self._category == eDataRequest_category.Inhaler:
            out_string = "Inhaler-"
        elif self._category == eDataRequest_category.MechanicalVentilator:
            out_string = "MechanicalVentilator-"
        elif self._category == eDataRequest_category.GasCompartment or \
             self._category == eDataRequest_category.LiquidCompartment or \
             self._category == eDataRequest_category.ThermalCompartment or \
             self._category == eDataRequest_category.TissueCompartment:
            out_string = self._compartment_name+"-"
            if self.has_substance_name():
                out_string += "{}-".format(self._substance_name)
        elif self._category == eDataRequest_category.Substance:
            out_string = self._substance_name+"-"
            if self.has_compartment_name():
                out_string += "{}-".format(self._compartment_name)
        out_string += self._property_name
        if self.has_unit():
            out_string += "({})".format(self.get_unit())

        return out_string.replace(" ", "_")

    def to_string(self) -> str:
        return self.__repr__()

    @classmethod
    def create_patient_request(cls, property:str, unit:SEScalarUnit=None):
        return cls(eDataRequest_category.Patient, property=property,  unit=unit)
    @classmethod
    def create_physiology_request(cls, property:str, unit:SEScalarUnit=None):
        return cls(eDataRequest_category.Physiology, property=property,  unit=unit)
    @classmethod
    def create_environment_request(cls, property:str, unit:SEScalarUnit=None):
        return cls(eDataRequest_category.Environment, property=property,  unit=unit)

    @classmethod
    def create_action_request(cls, action:str, property:str, unit:SEScalarUnit=None):
        return cls(eDataRequest_category.Action, action=action, property=property, unit=unit)
    @classmethod
    def create_action_compartment_request(cls, action:str, compartment:str, property:str, unit:SEScalarUnit=None):
        return cls(eDataRequest_category.Action, action=action, compartment=compartment, property=property, unit=unit)
    @classmethod
    def create_action_substance_request(cls, action:str, substance:str, property: str, unit: SEScalarUnit = None):
        return cls(eDataRequest_category.Action, action=action, substance=substance, property=property, unit=unit)

    @classmethod
    def create_action_data_request(cls, action:str, property:str, unit:SEScalarUnit=None):
        return cls(eDataRequest_category.Action, action=action, property=property, unit=unit)
    @classmethod
    def create_action_compartment_data_request(cls, action:str, compartment:str, property:str, unit:SEScalarUnit=None):
        return cls(eDataRequest_category.Action, action=action, compartment=compartment, property=property, unit=unit)
    @classmethod
    def create_action_substance_data_request(cls, action:str, substance:str, property:str, unit:SEScalarUnit=None):
        return cls(eDataRequest_category.Action, action=action, substance=substance, property=property, unit=unit)
    @classmethod
    def create_gas_compartment_request(cls, compartment:str, property:str, unit:SEScalarUnit=None):
        return cls(eDataRequest_category.GasCompartment, compartment=compartment, property=property,  unit=unit)
    @classmethod
    def create_gas_compartment_substance_request(cls, compartment:str, substance:str, property:str, unit:SEScalarUnit=None):
        return cls(eDataRequest_category.GasCompartment,
                   compartment=compartment,
                   substance=substance,
                   property=property,
                   unit=unit)
    @classmethod
    def create_liquid_compartment_request(cls, compartment:str, property:str, unit:SEScalarUnit=None):
        return cls(eDataRequest_category.LiquidCompartment, compartment=compartment, property=property,  unit=unit)
    @classmethod
    def create_liquid_compartment_substance_request(cls, compartment:str, substance:str, property:str, unit:SEScalarUnit=None):
        return cls(eDataRequest_category.LiquidCompartment,
                   compartment=compartment,
                   substance=substance,
                   property=property,
                   unit=unit)
    @classmethod
    def create_thermal_compartment_request(cls, compartment:str, property:str, unit:SEScalarUnit=None):
        return cls(eDataRequest_category.ThermalCompartment, compartment=compartment, property=property,  unit=unit)

    @classmethod
    def create_tissue_request(cls, compartment:str, property:str, unit:SEScalarUnit=None):
        return cls(eDataRequest_category.TissueCompartment, compartment=compartment, property=property,  unit=unit)

    @classmethod
    def create_substance_request(cls, substance:str, property:str, unit:SEScalarUnit=None):
        return cls(eDataRequest_category.Substance, substance=substance, property=property,  unit=unit)

    @classmethod
    def create_ecg_request(cls, property:str, unit:SEScalarUnit=None):
        return cls(eDataRequest_category.ECG, property=property,  unit=unit)
    @classmethod
    def create_anesthesia_machine_request(cls, property:str, unit:SEScalarUnit=None):
        return cls(eDataRequest_category.AnesthesiaMachine, property=property,  unit=unit)
    @classmethod
    def create_ecmo_request(cls, property:str, unit:SEScalarUnit=None):
        return cls(eDataRequest_category.ECMO, property=property,  unit=unit)
    @classmethod
    def create_inhaler_request(cls, property:str, unit:SEScalarUnit=None):
        return cls(eDataRequest_category.Inhaler, property=property,  unit=unit)
    @classmethod
    def create_mechanical_ventilator_request(cls, property:str, unit:SEScalarUnit=None):
        return cls(eDataRequest_category.MechanicalVentilator, property=property,  unit=unit)

    def get_category(self) -> eDataRequest_category:
        return self._category

    def has_action_name(self) -> bool:
        return bool(self._action_name)
    def get_action_name(self) -> str:
        return self._action_name
    def has_compartment_name(self) -> bool:
        return bool(self._compartment_name)
    def get_compartment_name(self) -> str:
        return self._compartment_name
    def has_substance_name(self) -> bool:
        return bool(self._substance_name)
    def get_substance_name(self) -> str:
        return self._substance_name
    def has_property_name(self) -> bool:
        return bool(self._property_name)
    def get_property_name(self) -> str:
        return self._property_name
    def has_unit(self) -> bool:
        return bool(self._unit)
    def get_unit(self) -> str:
        return self._unit


class SEDataRequested: # Event and Log support
    __slots__ = ['_id', '_is_active', '_headers', '_header_idxs', '_segments']

    @dataclass
    class Segment:
        id: int
        time_s: float
        values: [float]

    def __init__(self):
        self._id = -1
        self._is_active = False
        self._headers = [str]
        self._header_idxs = {}
        self._segments = []

    def clear(self):
        self._id = -1
        self._is_active = False
        self._headers = [str]
        self._segments = []

    def set_id(self, i: int):
        self._id = i
    def get_id(self) ->int:
        return self._id

    def set_active(self, a: bool):
        self._is_active = a
    def get_active(self) ->bool:
        return self._is_active

    def has_headers(self):
        return len(self._headers) > 0
    def get_headers(self) ->[str]:
        return self._headers
    def set_headers(self, h: [str]):
        self._headers = h.copy()
        idx = 0
        for header in self._headers:
            paren_idx = header.find("(")
            if paren_idx != -1:
                header = header[:paren_idx]
            self._header_idxs[header] = idx
            idx = idx + 1
    def get_header_index(self, header: str) ->int:
        paren_idx = header.find("(")
        if paren_idx != -1:
            header = header[:paren_idx]
        if header in self._header_idxs:
            return self._header_idxs[header]
        return None

    def has_segment(self, id_: int) ->bool:
        for s in self._segments:
            if s.id == id_:
                return True
        return False
    def add_segment(self, id_: int, time_s: float, values: [float]):
        s = self.get_segment(id_)
        if s is not None:
            s.time_s = time_s
            s.values = values
            return
        s = SEDataRequested.Segment(id_, time_s, values)
        self._segments.append(s)
    def get_segment(self, id_: int):
        for s in self._segments:
            if s.id == id_:
                return s
        return None

    def get_segments(self) -> [Segment]:
        return self._segments


class SEDataRequestManager:
    __slots__ = ["_results_filename", "_samples_per_second", "_data_requests"]

    def __init__(self, data_requests: Optional[List[SEDataRequest]]=None):
        self.clear()
        self._data_requests = data_requests if data_requests is not None else list()

    def clear(self):
        self._data_requests = list()
        self._results_filename = ""
        self._samples_per_second = 0

    def has_data_requests(self) -> int: return len(self._data_requests)
    def get_data_requests(self) -> List[SEDataRequest]: return self._data_requests
    def set_data_requests(self, requests: List[SEDataRequest]) -> None: self._data_requests = requests

    def has_results_filename(self) -> bool: return self._results_filename is not None
    def get_results_filename(self) -> str: return self._results_filename
    def set_results_filename(self, filename: str) -> None: self._results_filename = filename

    def get_samples_per_second(self) -> float: return self._samples_per_second
    def set_samples_per_second(self, sample: float) -> None: self._samples_per_second = sample

    def to_console(self, data_values):
        print("SimulationTime(s)={})".format(data_values[0]))
        for i in range(len(data_values)-1):
            print("{}={}".format(self._data_requests[i], data_values[i+1]))


class SEEngineInitialization:
    __slots__ = ["id", "patient_configuration", "state_filename",
                 "state", "data_request_mgr", "keep_event_changes",
                 "log_to_console", "log_filename", "keep_log_messages" ]

    def __init__(self):
        self.id = None
        self.patient_configuration = None
        self.state_filename = None
        self.state = None
        self.data_request_mgr = None
        self.log_filename = ""
        self.log_to_console = False
        self.keep_event_changes = False
        self.keep_log_messages = False


class SEEngineInitializationStatus:
    __slots__ = ("_initialization_state", "_csv_filename",
                 "_log_filename", "_stabilization_time_s")

    def __init__(self):
        self.clear()

    def clear(self) -> None:
        self._initialization_state = eEngineInitializationState.Uninitialized
        self._csv_filename = ""
        self._log_filename = ""
        self._stabilization_time_s = 0.

    def copy(self, other: "SEEngineInitializationStatus") -> None:
        self.clear()
        self._initialization_state = other._initialization_state
        self._csv_filename = other._csv_filename
        self._log_filename = other._log_filename
        self._stabilization_time_s = other._stabilization_time_s

    def get_initialization_state(self) -> eEngineInitializationState:
        return self._initialization_state
    def set_initialization_state(self, s: eEngineInitializationState) -> None:
        self._initialization_state = s

    def has_csv_filename(self) -> bool:
        return bool(self._csv_filename)
    def get_csv_filename(self) -> str:
        return self._csv_filename
    def set_csv_filename(self, fn: str) -> None:
        self._csv_filename = fn

    def has_log_filename(self) -> bool:
        return bool(self._log_filename)
    def get_log_filename(self) -> str:
        return self._log_filename
    def set_log_filename(self, fn: str) -> None:
        self._log_filename = fn

    def get_stabilization_time_s(self) -> float:
        return self._stabilization_time_s
    def set_stabilization_time_s(self, t: float) -> None:
        self._stabilization_time_s = t


class SESerializeRequested(SEAction):
    __slots__ = ["_filename"]

    def __init__(self) -> None:
        super().__init__()
        self._filename = ""

    def clear(self) -> None:
        super().clear()
        self._filename = ""

    def is_valid(self) -> bool:
        return self.has_filename()

    def has_filename(self) -> bool:
        return self._filename != ""
    def get_filename(self) -> str:
        return self._filename
    def set_filename(self, f: str) -> None:
        self._filename = f.replace("\\", "/")
    def invalidate_filename(self) -> None:
        self._filename = ""

    def __repr__(self) -> str:
        return ("Serialize Requested\n"
                "  Filename: {}").format(self._filename)


class eSerializationMode(Enum):
    Save = 0
    Load = 1


class SESerializeState(SEAction):
    __slots__ = ["_filename", "_mode"]

    def __init__(self) -> None:
        super().__init__()
        self._filename = ""
        self._type = eSerializationMode.Save

    def clear(self) -> None:
        super().clear()
        self._filename = ""
        self._mode = eSerializationMode.Save

    def is_valid(self) -> bool:
        return self.has_filename()

    def get_mode(self) -> eSerializationMode:
        return self._mode
    def set_mode(self, t: eSerializationMode):
        self._mode = t

    def has_filename(self) -> bool:
        return self._filename != ""
    def get_filename(self) -> str:
        return self._filename
    def set_filename(self, f: str) -> None:
        self._filename = f.replace("\\", "/")
    def invalidate_filename(self) -> None:
        self._filename = ""

    def __repr__(self) -> str:
        return ("Serialize State\n"
                "  Filename: {}\n"
                "  Mode: {}").format(self._filename, self._mode.name)


class ILoggerForward():
    def __init__(self):
        pass

    def forward_debug(self, msg):
        pass
    def forward_info(self, msg):
        pass
    def forward_warning(self, msg):
        pass
    def forward_error(self, msg):
        pass
    def forward_fatal(self, msg):
        pass
