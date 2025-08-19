# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import re
import logging
from enum import Enum

_pulse_logger = logging.getLogger('pulse')


def get_severity_str(severity: float) -> str:
    """
    :param severity: The severity value.

    :return: Severity as a qualitative string.
    """
    if severity == 0:
        raise ValueError("Severity string undefined for 0 severity")
    elif severity <= 0.3:
        severity_str = "Mild"
    elif severity <= 0.6:
        severity_str = "Moderate"
    else:
        severity_str = "Severe"

    return severity_str


def break_camel_case(string: str):
    # https://stackoverflow.com/a/9283563
    camel_case_regex = r"""
        (            # start the group
            # alternative 1
        (?<=[a-z])  # current position is preceded by a lower char
                    # (positive lookbehind: does not consume any char)
        [A-Z]       # an upper char
                    #
        |   # or
            # alternative 2
        (?<!\A)     # current position is not at the beginning of the string
                    # (negative lookbehind: does not consume any char)
        [A-Z]       # an upper char
        (?=[a-z])   # matches if next char is a lower char
                    # lookahead assertion: does not consume any char
        )           # end the group"""

    return re.sub(camel_case_regex, r' \1', string, flags=re.VERBOSE)


class ePrettyPrintType(Enum):
    Action = 0
    Condition = 1


def pretty_print(string: str, print_type: ePrettyPrintType, preserve_camel_case: bool = False):
    ret = ""
    string = string.replace('"', '')
    string = string.replace('{', '')
    string = string.replace('}', '')
    string = string.replace(',', '')
    string = string.replace('[', '')
    string = string.replace(']', '')

    lines = [s.rstrip() for s in string.splitlines() if s]

    idx = 0
    while idx < len(lines):
        line = lines[idx].rstrip()

        if len(line) == 0:
            idx += 1
            continue
        if print_type.name in line:
            idx += 1
            continue
        if "ReadOnly" in line:
            idx += 1
            continue
        if line.endswith("Comment:"):
            idx += 1
            continue

        found_unit = False
        if idx+1 < len(lines):
            peek = lines[idx+1]
            if "Scalar" in peek:
                peek = lines[idx+2]
                line = ''.join([line, peek[(peek.find("Value")+5):].rstrip()])
                if (idx+3) < len(lines) and "Unit:" in lines[idx+3]:
                    idx += 3
                    peek = lines[idx]
                    line = ''.join([line, peek[(peek.find("Unit:")+5):].rstrip()])
                    found_unit = True
                else:
                    idx += 2

        if not preserve_camel_case and "Comment:" not in line:
            # Don't change files or units
            if found_unit or "Unit:" in line or "File" in line:
                colon = line.rfind(":")
                if colon != -1:
                    line = break_camel_case(line[:colon]) + line[colon:]
            else:
                line = break_camel_case(line)

        ret = ''.join([ret, line, "\n"])
        idx += 1

    ret = ret.replace('::', ':')

    return ret
