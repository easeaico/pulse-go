# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import logging
import numpy as np


_pulse_logger = logging.getLogger('pulse')


def format_float(f: float, fmt: str = "") -> str:
    abs_p = abs(f)
    #  https://docs.python.org/2/library/string.html#formatstrings
    #  https://stackoverflow.com/questions/15238120/keep-trailing-zeroes-in-python

    if abs_p == 0:
        s = "0.00"
    elif len(fmt) == 0:
        s = f"{f:#.3g}"
    else:
        s = f"{f:{fmt}}"

    if s[-1] == '.':
        s = s[0:-1]
    return s


def percent_tolerance(expected: float, calculated: float, epsilon: float, verbose: bool = True):
    # Check for 'invalid' numbers
    if np.isnan(expected) or np.isnan(calculated) or np.isinf(expected) or np.isinf(calculated):
        if verbose:
            _pulse_logger.warning(f"While finding percent tolerance from values "
                                  f"'expected' = {expected} and 'calculated' = {calculated}, "
                                  f"invalid values (NaN or Infinity) were found. Unexpected results may occur.")
        if (np.isnan(expected) and np.isnan(calculated)) or (np.isinf(expected) and np.isinf(calculated)):
            return 0.0
        return np.nan

    # Special cases
    if expected == 0.0 and calculated == 0.0:
        return 0.0
    elif expected == 0.0 or calculated == 0.0:
        if abs(expected + calculated) < epsilon:
            return 0.0
        else:
            if expected == 0.0:
                return float('inf')
            elif expected < 0.0:
                return -100.0
            else:
                return 100
    else:
        return abs(calculated - expected) / expected * 100.0


def percent_difference(expected: float, calculated: float, epsilon: float = 1e-10, verbose: bool = True):
    # Check for 'invalid' numbers
    if np.isnan(expected) or np.isnan(calculated) or np.isinf(expected) or np.isinf(calculated):
        if verbose:
            _pulse_logger.warning(f"While finding percent difference from values "
                                  f"'expected' = {expected} and 'calculated' = {calculated}, "
                                  f"invalid values (NaN or Infinity) were found. Unexpected results may occur.")
        if (np.isnan(expected) and np.isnan(calculated)) or (np.isinf(expected) and np.isinf(calculated)):
            return 0.0
        return np.nan

    # Special cases
    if expected == 0.0 and calculated == 0.0:
        return 0.0
    elif expected == 0.0 or calculated == 0.0:
        if abs(expected + calculated) < epsilon:
            return 0.0
        else:
            return 200.0
    else:
        difference = calculated - expected
        average = (calculated + expected) / 2.0

        if average == 0.0:
            return float('inf')

        return abs(difference / average) * 100.0


def percent_change(expected: float, calculated: float, epsilon: float, verbose: bool = True):
    # Check for 'invalid' numbers
    if np.isnan(expected) or np.isnan(calculated) or np.isinf(expected) or np.isinf(calculated):
        if verbose:
            _pulse_logger.warning(f"While finding percent change from values "
                                  f"'expected' = {expected} and 'calculated' = {calculated}, "
                                  f"invalid values (NaN or Infinity) were found. Unexpected results may occur.")
        if (np.isnan(expected) and np.isnan(calculated)) or (np.isinf(expected) and np.isinf(calculated)):
            return 0.0
        return np.nan

    # Special cases
    if expected == 0.0 and calculated == 0.0:
        return 0.0
    elif expected == 0.0 or calculated == 0.0:
        if abs(expected + calculated) < epsilon:
            return 0.0
        elif expected == 0.0:
            return float('inf')

    return (calculated - expected) / abs(expected) * 100.0
