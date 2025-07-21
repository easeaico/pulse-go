# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from enum import Enum


class eEngineInitializationState(int, Enum):
    Uninitialized = 0
    FailedState = 1
    FailedPatientSetup = 2
    FailedStabilization = 3
    Initialized = 4


class eSerializationFormat(int, Enum):
    JSON = 0
    BINARY = 1
    VERBOSE_JSON = 2
    TEXT = 3


class eSide(int, Enum):
    NullSide = 0
    Left = 1
    Right = 2


class eGate(int, Enum):
    NullGate = 0
    Open = 1
    Closed = 2


class eSwitch(int, Enum):
    NullSwitch = 0
    Off = 1
    On = 2


class eCharge(int, Enum):
    NullCharge = 0
    Negative = 1
    Neutral = 2
    Positive = 3

