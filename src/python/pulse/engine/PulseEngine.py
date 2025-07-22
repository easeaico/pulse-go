# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from enum import Enum
import PyPulse
from pulse.cdm.patient import SEPatient, SEPatientConfiguration
from pulse.cdm.enums import eSerializationFormat
from pulse.cdm.engine import SEAction, SEDataRequestManager, SEDataRequest
from pulse.cdm.engine import IEventHandler, ILoggerForward
from pulse.cdm.io.engine import serialize_actions_to_string, \
                                serialize_patient_configuration_to_string, \
                                serialize_data_request_manager_to_string, \
                                serialize_event_change_list_from_string, \
                                serialize_active_event_list_from_string, \
                                serialize_log_messages_from_string
from pulse.cdm.io.patient import serialize_patient_from_string, serialize_patient_to_string
from pulse.engine.io.PulseConfiguration import serialize_pulse_configuration_to_string
from pulse.cdm.scalars import ElectricPotentialUnit, FrequencyUnit, \
                              PressureUnit, TemperatureUnit, VolumeUnit, VolumePerTimeUnit
from pulse.engine import PulseConfiguration

from typing import Optional
class eModelType(Enum):
    HumanAdultWholeBody = 0
    HumanAdultVentilationMechanics = 1
    HumanAdultHemodynamics = 2

def version():
    return PyPulse.__version__;

def hash():
    return PyPulse.__hash__;

class PulseEngine:
    __slots__ = ['__pulse', "_is_ready", "_dt_s",
                 "_data_request_mgr","_results",
                 "_event_handler", "_log_forward",
                 "_spare_time_s", "_args"]

    def __init__(self, eModelType=eModelType.HumanAdultWholeBody, data_root_dir="./"):
        self._args = (eModelType, data_root_dir)
        t = PyPulse.model_type.human_adult_whole_body
        if eModelType is eModelType.HumanAdultVentilationMechanics:
            t = PyPulse.model_type.human_adult_ventilation_mechanics
        self.__pulse = PyPulse.Engine(t, data_root_dir)
        self._is_ready = False
        self._log_forward = None
        self._event_handler = None
        self._results = []
        self._data_request_mgr = None
        self._spare_time_s = 0
        self._dt_s = self.__pulse.get_timestep("s")

    def clear(self):
        self.__pulse.clear();

    @staticmethod
    def _default_data_request_mgr():
        data_request_mgr = SEDataRequestManager()
        data_request_mgr.set_data_requests([
            SEDataRequest.create_physiology_request("HeartRate", unit=FrequencyUnit.Per_min),
            SEDataRequest.create_physiology_request("ArterialPressure", unit=PressureUnit.mmHg),
            SEDataRequest.create_physiology_request("MeanArterialPressure", unit=PressureUnit.mmHg),
            SEDataRequest.create_physiology_request("SystolicArterialPressure", unit=PressureUnit.mmHg),
            SEDataRequest.create_physiology_request("DiastolicArterialPressure", unit=PressureUnit.mmHg),
            SEDataRequest.create_physiology_request("OxygenSaturation"),
            SEDataRequest.create_physiology_request("EndTidalCarbonDioxidePressure", unit=PressureUnit.mmHg),
            SEDataRequest.create_physiology_request("RespirationRate", unit=FrequencyUnit.Per_min),
            SEDataRequest.create_physiology_request("SkinTemperature", unit=TemperatureUnit.C),
            SEDataRequest.create_physiology_request("CardiacOutput", unit=VolumePerTimeUnit.L_Per_min),
            SEDataRequest.create_physiology_request("BloodVolume", unit=VolumeUnit.mL),
            SEDataRequest.create_ecg_request("Lead3ElectricPotential", unit=ElectricPotentialUnit.mV),
            SEDataRequest.create_gas_compartment_substance_request("Carina", "CarbonDioxide", "PartialPressure", unit=PressureUnit.mmHg)

        ])
        return data_request_mgr

    def is_valid_patient(self, patient: SEPatient):
        json = serialize_patient_to_string(patient, eSerializationFormat.JSON)
        return self.__pulse.is_valid_patient(json, PyPulse.serialization_format.json)

    def serialize_from_file(self, state_file: str,
                            data_request_mgr: Optional[SEDataRequestManager] = None):
        # Process requests and setup our results structure
        drm = self._process_requests(data_request_mgr, eSerializationFormat.JSON)
        self._is_ready = self.__pulse.serialize_from_file(state_file, drm, PyPulse.serialization_format.json)
        self._dt_s = self.__pulse.get_timestep("s")
        if self._is_ready:
            self._pull()
        return self._is_ready

    def serialize_to_file(self, state_file: str):
        if self._is_ready:
            return self.__pulse.serialize_to_file(state_file)
        return False

    def serialize_from_string(self, state: str,
                              data_request_mgr: Optional[SEDataRequestManager],
                              state_format: eSerializationFormat):
        # Process requests and setup our results structure
        drm = self._process_requests(data_request_mgr, state_format)
        if state_format == eSerializationFormat.BINARY:
            fmt = PyPulse.serialization_format.binary
        else:
            fmt = PyPulse.serialization_format.json
        self._is_ready = self.__pulse.serialize_from_string(state, drm, fmt)
        self._dt_s = self.__pulse.get_timestep("s")
        if self._is_ready:
            self._pull()
        return self._is_ready

    def serialize_to_string(self, format: eSerializationFormat):
        if self._is_ready:
            return self.__pulse.serialize_to_string(format)
        return None

    def __getstate__(self):
        return self._args, self.serialize_to_string(PyPulse.serialization_format.binary)

    def __setstate__(self, args_state):
        args, state = args_state
        self.__init__(*args)
        return self.serialize_from_string(state, self._data_request_mgr, eSerializationFormat.BINARY)

    def set_configuration_override(self, cfg: PulseConfiguration) -> bool:
        json = serialize_pulse_configuration_to_string(cfg, eSerializationFormat.JSON)
        return self.__pulse.set_configuration_override(json, PyPulse.serialization_format.json)

    def initialize_engine(self, patient_configuration: SEPatientConfiguration,
                          data_request_mgr: SEDataRequestManager):
        # Process requests and setup our results structure
        drm = self._process_requests(data_request_mgr, eSerializationFormat.JSON)
        pc = serialize_patient_configuration_to_string(patient_configuration, eSerializationFormat.JSON)
        self._is_ready = self.__pulse.initialize_engine(pc, drm, PyPulse.serialization_format.json)
        self._dt_s = self.__pulse.get_timestep("s")
        if self._is_ready:
            self._pull()
        return self._is_ready

    def get_initial_patient(self, dst: SEPatient):
        stream = self.__pulse.get_initial_patient(PyPulse.serialization_format.json)
        serialize_patient_from_string(stream, dst, eSerializationFormat.JSON)

    def set_event_handler(self, event_handler: IEventHandler):
        self._event_handler = event_handler
        self.__pulse.keep_event_changes( self._event_handler is not None )

    def set_log_listener(self, log_forward: ILoggerForward):
        self._log_forward = log_forward
        self.__pulse.keep_log_messages( self._log_forward is not None )

    def log_to_console(self, b: bool):
        self.__pulse.log_to_console( b )

    def set_log_filename(self, filename: str):
        self.__pulse.set_log_filename( filename )

    def _advance_time(self):
        if self.__pulse.advance_timestep():
            self._process_events()
            self._process_logs()
            return True
        return False

    def _pull(self):
        # self._results = copy.deepcopy(self.__pulse.pull_data())
        self._results = self.__pulse.pull_data()

    def advance_time(self):
        if not self._is_ready:
            return False
        if self._advance_time():
            self._pull()
            return True
        return False

    def advance_time_s(self, duration_s: float):
        if not self._is_ready:
            return False
        total_time = duration_s + self._spare_time_s
        num_steps = int(total_time / self._dt_s)
        for n in range(num_steps):
            if not self._advance_time():
                return False
        self._pull()
        self._spare_time_s = total_time - (num_steps * self._dt_s)
        return True

    def pull_data(self):
        return self._results

    def print_results(self):
        self._data_request_mgr.to_console(self._results)

    def pull_active_events(self):
        events = self.__pulse.pull_active_events(PyPulse.serialization_format.json)
        if events:
            active_events = serialize_active_event_list_from_string(events, eSerializationFormat.JSON)
            return active_events
        return None

    def _process_requests(self, data_request_mgr, fmt: eSerializationFormat):
        if data_request_mgr is None:
            data_request_mgr = self._default_data_request_mgr()
        self._data_request_mgr = data_request_mgr

        # Simulation time is always the first result.
        self._results = []  # Clear all results
        return serialize_data_request_manager_to_string(data_request_mgr, fmt)

    def process_action(self, action: SEAction):
        if not self._is_ready:
            return False
        actions = [action]
        self.process_actions(actions)

    def process_actions(self, actions: []):
        if not self._is_ready:
            return False
        if len(actions) == 0:
            return True
        json = serialize_actions_to_string(actions, eSerializationFormat.JSON)
        # print(json)
        self.__pulse.process_actions(json, PyPulse.serialization_format.json)

    def _process_events(self):
        if self._is_ready and self._event_handler:
            events = self.__pulse.pull_events(PyPulse.serialization_format.json)
            if events:
                event_changes = serialize_event_change_list_from_string(events, eSerializationFormat.JSON)
                for event_change in event_changes:
                    self._event_handler.handle_event(event_change)

    def _process_logs(self):
        if self._is_ready and self._log_forward:
            logs = self.__pulse.pull_log_messages(PyPulse.serialization_format.json)
            if logs:
                log_msgs = serialize_log_messages_from_string(logs,eSerializationFormat.JSON)
                for msg in log_msgs['Debug']:
                    self._log_forward.forward_debug(msg)
                for msg in log_msgs['Info']:
                    self._log_forward.forward_info(msg)
                for msg in log_msgs['Warning']:
                    self._log_forward.forward_warning(msg)
                for msg in log_msgs['Fatal']:
                    self._log_forward.forward_fatal(msg)
                for msg in log_msgs['Error']:
                    self._log_forward.forward_error(msg)
