# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.engine import SECondition
from pulse.cdm.physiology import eLungCompartment
from pulse.cdm.scalars import SEScalar0To1, SEScalarArea, SEScalarVolume

class SEPatientCondition(SECondition):
    __slots__ = ["_active"]
    def __init__(self):
        super().__init__()
        self._active = True
    def clear(self):
        super().clear()
    def is_active(self):
        return self._active
    def set_active(self, active:bool):
        self._active = active
    def get_name(self):
        return self.__repr__()

class SEAcuteRespiratoryDistressSyndrome(SEPatientCondition):
    __slots__ = ["_severities"]

    def __init__(self):
        super().__init__()
        self._severities = {}

    def clear(self):
        for s in self._severities.values():
            if s is not None:
                s.invalidate()

    def is_valid(self):
        return self.has_severity()

    def has_severity(self, cmpt: eLungCompartment = None):
        if cmpt is None:
            for s in self._severities.values():
                if s is not None and s.is_valid():
                    return True
            return False
        if cmpt not in self._severities:
            return False
        return self._severities.get(cmpt).is_valid()
    def get_severity(self, cmpt: eLungCompartment):
        s = self._severities.get(cmpt)
        if s is None:
            s = SEScalar0To1()
            self._severities[cmpt] = s
        return s

    def __repr__(self):
        out = "Acute Respiratory Distress Syndrome\n";
        for c, s in self._severities.items():
            out += ("\t{}  Severity: {}\n").format(c, s)
        return out

class SEChronicAnemia(SEPatientCondition):
    __slots__ = ['_reduction_factor']

    def __init__(self):
        super().__init__()
        self._reduction_factor=None

    def clear(self):
        super().clear()
        if self._reduction_factor is not None:
            self._reduction_factor.invalidate()

    def is_valid(self):
        return self.has_reduction_factor()

    def has_reduction_factor(self):
        return self._reduction_factor is not None
    def get_reduction_factor(self):
        if self._reduction_factor is None:
            self._reduction_factor = SEScalar0To1()
        return self._reduction_factor
    def __repr__(self):
        return ("Chronic Anemia\n"
                "  Reduction Factor: {}").format(self._reduction_factor)

class SEChronicHeartFailure(SEPatientCondition):
    __slots__ = []

    def __init__(self):
        super().__init__()
        print("No Functionality in this condition")
        pass

class SEChronicObstructivePulmonaryDisease(SEPatientCondition):
    __slots__ = ["_bronchitis_severity", "_emphysema_severities"]

    def __init__(self):
        super().__init__()
        self._bronchitis_severity = None
        self._emphysema_severities = {}
    def clear(self):
        super().clear()
        if self._bronchitis_severity is not None:
            self._bronchitis_severity.invalidate()
        for s in self._emphysema_severities.values():
            if s is not None:
                s.invalidate()
    def is_valid(self):
        return self.has_emphysema_severity() and self.has_bronchitis_severity()

    def has_bronchitis_severity(self):
        return self._bronchitis_severity is not None
    def get_bronchitis_severity(self):
        if self._bronchitis_severity is None:
            self._bronchitis_severity = SEScalar0To1()
        return self._bronchitis_severity

    def has_emphysema_severity(self, cmpt: eLungCompartment = None):
        if cmpt is None:
            for s in self._emphysema_severities.values():
                if s is not None and s.is_valid():
                    return True
            return False
        if cmpt not in self._emphysema_severities:
            return False
        return self._emphysema_severities.get(cmpt).is_valid()
    def get_emphysema_severity(self, cmpt: eLungCompartment):
        s = self._emphysema_severities.get(cmpt)
        if s is None:
            s = SEScalar0To1()
            self._emphysema_severities[cmpt] = s
        return s

    def __repr__(self):
        out = "COPD\n";
        out += ("  Bronchitis Severity: {}]\n").format(self._bronchitis_severity)
        for c, s in self._severities.items():
            out += ("\t{} Emphysema Severity: {}\n").format(c, s)

class SEChronicPericardialEffusion(SEPatientCondition):
    __slots__ = ["_accumulated_volume"]

    def __init__(self):
        super().__init__()
        self._accumulated_volume = None
    def clear(self):
        super().clear()
        if self._accumulated_volume is not None:
            self._accumulated_volume.invalidate()
    def is_valid(self):
        return self.has_accumulated_volume()
    def has_accumulated_volume(self):
        return self._accumulated_volume is not None
    def get_accumulated_volume(self):
        if self._accumulated_volume is None:
            self._accumulated_volume = SEScalarVolume()
        return self._accumulated_volume
    def __repr__(self):
        return ("Chronic Pericardial Effusion\n"
                "  Accumulated Volume: {}").format(self._accumulated_volume)

class SEChronicRenalStenosis(SEPatientCondition):
    __slots__ = ["_right_kidney_severity", "_left_kidney_severity"]

    def __init__(self):
        super().__init__()
        self._left_kidney_severity = None
        self._right_kidney_severity = None
    def clear(self):
        super().clear()
        if self._left_kidney_severity is not None:
            self._left_kidney_severity.invalidate()
        if self._right_kidney_severity is not None:
            self._right_kidney_severity.invalidate()
    def is_valid(self):
        return self.has_right_kidney_severity() and self.has_left_kidney_severity()
    def has_right_kidney_severity(self):
        return self._right_kidney_severity is not None
    def get_right_kidney_severity(self):
        if self._right_kidney_severity is None:
            self._right_kidney_severity = SEScalar0To1()
        return self._right_kidney_severity
    def has_left_kidney_severity(self):
        return self._left_kidney_severity is not None
    def get_left_kidney_severity(self):
        if self._left_kidney_severity is None:
            self._left_kidney_severity = SEScalar0To1()
        return self._left_kidney_severity
    def __repr__(self):
        return ("Chronic Renal Stenosis\n"
                "  Right Kidney Severity: {}\n"
                "  Left Kidney Severity: {}").format(self._right_kidney_severity, self._left_kidney_severity)

class SEChronicVentricularSystolicDysfunction(SEPatientCondition):
    __slots__ = []

    def __init__(self):
        super().__init__()
    def __repr__(self):
        return ("Chronic Ventricular Systolic Dysfunction\n")
    def is_valid(self):
        return True


class SEDehydration(SEPatientCondition):
    __slots__ = ["_severity"]

    def __init__(self):
        super().__init__()
        self._severity = None
    def clear(self):
        if self._severity is not None:
            self._severity.invalidate()
    def is_valid(self):
        return self.has_severity()
    def has_severity(self):
        return self._severity is not None
    def get_severity(self):
        if self._severity is None:
            self._severity = SEScalar0To1()
        return self._severity
    def __repr__(self):
        return ("Pulmonary Shunt\n"
                "  Severity: {}\n").format(self._severity)


class SEImpairedAlveolarExchange(SEPatientCondition):
    __slots__ = ["_impaired_surface_area", "_impaired_fraction", "_severity"]

    def __init__(self):
        super().__init__()
        self._impaired_fraction = None
        self._impaired_surface_area = None
        self._severity = None
    def clear(self):
        super().clear()
        if self._impaired_fraction is not None:
            self._impaired_fraction.invalidate()
        if self._impaired_surface_area is not None:
            self._impaired_surface_area.invalidate()
        if self._severity is not None:
            self._severity.invalidate()
    def is_valid(self):
        return self.has_impaired_surface_area() or \
               self.has_impaired_fraction() or \
               self.has_severity()
    def has_impaired_surface_area(self):
        return self._impaired_surface_area is not None
    def get_impaired_surface_area(self):
        if self._impaired_surface_area is None:
            self._impaired_surface_area = SEScalarArea()
        return self._impaired_surface_area
    def has_impaired_fraction(self):
        return self._impaired_fraction is not None
    def get_impaired_fraction(self):
        if self._impaired_fraction is None:
            self._impaired_fraction = SEScalar0To1()
        return self._impaired_fraction
    def has_severity(self):
        return self._severity is not None
    def get_severity(self):
        if self._severity is None:
            self._severity = SEScalar0To1()
        return self._severity
    def __repr__(self):
        return ("Impaired Alveolar Exchange\n"
                "  Impaired Fraction: {}\n"
                "  Impaired Surface Area: {}\n"
                "  Severity: {}").format(self._impaired_fraction, self._impaired_surface_area, self._severity)

class SEPneumonia(SEPatientCondition):
    __slots__ = ["_severities"]

    def __init__(self):
        super().__init__()
        self._severities = {}
    def clear(self):
        for s in self._severities.values():
            if s is not None:
                s.invalidate()
    def is_valid(self):
        return self.has_severity()

    def has_severity(self, cmpt: eLungCompartment = None):
        if cmpt is None:
            for s in self._severities.values():
                if s is not None and s.is_valid():
                    return True
            return False
        if cmpt not in self._severities:
            return False
        return self._severities.get(cmpt).is_valid()
    def get_severity(self, cmpt: eLungCompartment):
        s = self._severities.get(cmpt)
        if s is None:
            s = SEScalar0To1()
            self._severities[cmpt] = s
        return s
    def __repr__(self):
        out = "Pneumonia\n";
        for c, s in self._severities.items():
            out += ("\t{}  Severity: {}\n").format(c, s)
        return out

class SEPulmonaryFibrosis(SEPatientCondition):
    __slots__ = ["_severity"]

    def __init__(self):
        super().__init__()
        self._severity = None
    def clear(self):
        if self._severity is not None:
            self._severity.invalidate()
    def is_valid(self):
        return self.has_severity()
    def has_severity(self):
        return self._severity is not None
    def get_severity(self):
        if self._severity is None:
            self._severity = SEScalar0To1()
        return self._severity
    def __repr__(self):
        return ("Pulmonary Fibrosis\n"
                "  Severity: {}\n").format(self._severity)


class SEPulmonaryShunt(SEPatientCondition):
    __slots__ = ["_severity"]

    def __init__(self):
        super().__init__()
        self._severity = None
    def clear(self):
        if self._severity is not None:
            self._severity.invalidate()
    def is_valid(self):
        return self.has_severity()
    def has_severity(self):
        return self._severity is not None
    def get_severity(self):
        if self._severity is None:
            self._severity = SEScalar0To1()
        return self._severity
    def __repr__(self):
        return ("Pulmonary Shunt\n"
                "  Severity: {}\n").format(self._severity)

class SESepsis(SEPatientCondition):
    __slots__ = ["_severity"]

    def __init__(self):
        super().__init__()
        self._severity = None
    def clear(self):
        if self._severity is not None:
            self._severity.invalidate()
    def is_valid(self):
        return self.has_severity()
    def has_severity(self):
        return self._severity is not None
    def get_severity(self):
        if self._severity is None:
            self._severity = SEScalar0To1()
        return self._severity
    def __repr__(self):
        return ("Sepsis\n"
                "  Severity: {}\n").format(self._severity)
