# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import logging
import numpy as np


_pulse_logger = logging.getLogger('pulse')


def percent_tolerance(expected: float, calculated: float, epsilon: float, verbose: bool=True):
    # Check for 'invalid' numbers
    if np.isnan(expected) or np.isnan(calculated) or np.isinf(expected) or np.isinf(calculated):
        if verbose:
            _pulse_logger.warning(f"While finding percent tolerance from values 'expected' = {expected} and " \
                f"'calculated' = {calculated}, invalid values (NaN or Infinity) were found. Unexpected results may occur.")
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


def generate_percent_tolerance_span(expected: float, calculated: float, epsilon: float, precision: int = 1):
    percent = percent_tolerance(expected, calculated, epsilon)

    return generate_percent_span(percent, precision)


def percent_difference(expected: float, calculated: float, epsilon: float, verbose: bool=True):
    # Check for 'invalid' numbers
    if np.isnan(expected) or np.isnan(calculated) or np.isinf(expected) or np.isinf(calculated):
        if verbose:
            _pulse_logger.warning(f"While finding percent difference from values 'expected' = {expected} and " \
                f"'calculated' = {calculated}, invalid values (NaN or Infinity) were found. Unexpected results may occur.")
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


def generate_percent_difference_span(expected: float, calculated: float, epsilon: float, precision: int = 1):
    percent = percent_difference(expected, calculated, epsilon)

    return generate_percentage_span(percent, precision)


def percent_change(expected: float, calculated: float, epsilon: float, verbose: bool=True):
    # Check for 'invalid' numbers
    if np.isnan(expected) or np.isnan(calculated) or np.isinf(expected) or np.isinf(calculated):
        if verbose:
            _pulse_logger.warning(f"While finding percent change from values 'expected' = {expected} and " \
                f"'calculated' = {calculated}, invalid values (NaN or Infinity) were found. Unexpected results may occur.")
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


def generate_percentage_span(percentage, precision, success=10, warning=30):
    if abs(percentage) <= success:
        c = '"success"'
    elif abs(percentage) <= warning:
        c = '"warning"'
    else:
        c = '"danger"'

    return f'<span class={c}>{percentage:.{precision}G}%</span>'
