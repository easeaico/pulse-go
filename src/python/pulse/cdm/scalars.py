# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import PyPulse


class ScalarException(Exception):
    pass


class SEScalar:
    __slots__ = ["_value"]

    def __init__(self, value: float = None):
        if value is None:
            self._value = None
        else:
            self._value = value

    def set(self, scalar):
        if isinstance(scalar, SEScalar):
            self._value = scalar._value
        else:
            raise ScalarException("Provided argument must be a SEScalar")

    def is_valid(self):
        return self._value is not None

    def is_positive(self):
        return self._value is not None and self._value > 0

    def is_negative(self):
        return self._value is not None and self._value < 0

    def get_value(self):
        return self._value

    def set_value(self, value: float):
        self._value = value

    def invalidate(self):
        self._value = None

    def __repr__(self):
        return " {} """.format(self._value)


class SEScalar0To1(SEScalar):
    def set_value(self, value: float):
        if value > 1.0 or value < 0.0:
            raise ScalarException("Scalar0To1 must be [0,1]")
        self._value = value


class SEScalarNegative1To1(SEScalar):
    def set_value(self, value: float):
        if value > 1 or value < -1:
            raise ScalarException("ScalarNegative1To1 must be [-1,1]")
        self._value = value


class SEScalarQuantity(SEScalar):
    __slots__ = ["_units"]

    def __init__(self, value: float = None, units: any = None):
        super().__init__(value)
        if units is None:
            self.invalidate()
        else:
            self._units = units

    def is_valid(self):
        return self._value is not None and self._units is not None

    def get_unit(self):
        return self._units

    def invalidate(self):
        self._value = None
        self._units = None

    def __repr__(self):
        return "{}{}".format(self._value, self._units)


class SEScalarUnit:
    __slots__ = ["_string"]

    def __init__(self, string: str):
        self._string = string

    def get_string(self):
        return self._string

    def __repr__(self):
        return "({})".format(self._string)

    def __str__(self):
        return self._string


class AmountPerTimeUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == AmountPerTimeUnit.umol_Per_s.get_string():
            return AmountPerTimeUnit.umol_Per_s
        if string == AmountPerTimeUnit.mmol_Per_min.get_string():
            return AmountPerTimeUnit.mmol_Per_min
        if string == AmountPerTimeUnit.pmol_Per_min.get_string():
            return AmountPerTimeUnit.pmol_Per_min
        if string == AmountPerTimeUnit.umol_Per_min.get_string():
            return AmountPerTimeUnit.umol_Per_min
        if string == AmountPerTimeUnit.mol_Per_day.get_string():
            return AmountPerTimeUnit.mol_Per_day
        raise ScalarException(f"No AmountPerTimeUnit defined for {string}")

    __slots__ = ["umol_Per_s", "mmol_Per_min", "pmol_Per_min", "umol_Per_min", "mol_Per_day"]


AmountPerTimeUnit.umol_Per_s = AmountPerTimeUnit("umol/s")
AmountPerTimeUnit.mmol_Per_min = AmountPerTimeUnit("mmol/min")
AmountPerTimeUnit.pmol_Per_min = AmountPerTimeUnit("pmol/min")
AmountPerTimeUnit.umol_Per_min = AmountPerTimeUnit("umol/min")
AmountPerTimeUnit.mol_Per_day = AmountPerTimeUnit("mol/day")


class SEScalarAmountPerTime(SEScalarQuantity):

    def __init__(self, value: float = None, units: AmountPerTimeUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarAmountPerTime):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarAmountPerTime")

    def get_value(self, units: AmountPerTimeUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: AmountPerTimeUnit):
        if isinstance(units, AmountPerTimeUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a AmountPerTimeUnit")


class AmountPerVolumeUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == AmountPerVolumeUnit.mEq_Per_L.get_string():
            return AmountPerVolumeUnit.mEq_Per_L
        if string == AmountPerVolumeUnit.mol_Per_L.get_string():
            return AmountPerVolumeUnit.mol_Per_L
        if string == AmountPerVolumeUnit.mol_Per_mL.get_string():
            return AmountPerVolumeUnit.mol_Per_mL
        if string == AmountPerVolumeUnit.mmol_Per_L.get_string():
            return AmountPerVolumeUnit.mmol_Per_L
        if string == AmountPerVolumeUnit.mmol_Per_mL.get_string():
            return AmountPerVolumeUnit.mmol_Per_mL
        if string == AmountPerVolumeUnit.pmol_Per_L.get_string():
            return AmountPerVolumeUnit.pmol_Per_L
        if string == AmountPerVolumeUnit.ct_Per_L.get_string():
            return AmountPerVolumeUnit.ct_Per_L
        if string == AmountPerVolumeUnit.ct_Per_uL.get_string():
            return AmountPerVolumeUnit.ct_Per_uL
        raise ScalarException(f"No AmountPerVolumeUnit defined for {string}")

    __slots__ = ["mEq_Per_L", "mol_Per_L", "mol_Per_mL", "mmol_Per_L",
                 "mmol_Per_mL", "pmol_Per_L", "ct_Per_L", "ct_Per_uL"]

 
AmountPerVolumeUnit.mEq_Per_L = AmountPerVolumeUnit("mEq/L")
AmountPerVolumeUnit.mol_Per_L = AmountPerVolumeUnit("mol/L")
AmountPerVolumeUnit.mol_Per_mL = AmountPerVolumeUnit("mol/mL")
AmountPerVolumeUnit.mmol_Per_L = AmountPerVolumeUnit("mmol/L")
AmountPerVolumeUnit.mmol_Per_mL = AmountPerVolumeUnit("mmol/mL")
AmountPerVolumeUnit.pmol_Per_L = AmountPerVolumeUnit("pmol/L")
AmountPerVolumeUnit.ct_Per_L = AmountPerVolumeUnit("ct/L")
AmountPerVolumeUnit.ct_Per_uL = AmountPerVolumeUnit("ct/uL")


class SEScalarAmountPerVolume(SEScalarQuantity):

    def __init__(self, value: float = None, units: AmountPerVolumeUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarAmountPerVolume):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarAmountPerVolume")

    def get_value(self, units: AmountPerVolumeUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: AmountPerVolumeUnit):
        if isinstance(units, AmountPerVolumeUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a AmountPerVolumeUnit")


class AreaUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == AreaUnit.cm2.get_string():
            return AreaUnit.cm2
        if string == AreaUnit.m2.get_string():
            return AreaUnit.m2
        raise ScalarException("No AreaUnit defined for " + string)

    __slots__ = ["cm2", "m2"]
    
    
AreaUnit.cm2 = AreaUnit("cm^2")
AreaUnit.m2 = AreaUnit("m^2")


class SEScalarArea(SEScalarQuantity):

    def __init__(self, value: float = None, units: AreaUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarArea):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarArea")

    def get_value(self, units: AreaUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: AreaUnit):
        if isinstance(units, AreaUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a AreaUnit")


class ElectricPotentialUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == ElectricPotentialUnit.V.get_string():
            return ElectricPotentialUnit.V
        if string == ElectricPotentialUnit.mV.get_string():
            return ElectricPotentialUnit.mV
        raise ScalarException("No ElectricPotentialUnit defined for " + string)

    __slots__ = ["V", "mV"]
    
    
ElectricPotentialUnit.V = ElectricPotentialUnit("V")
ElectricPotentialUnit.mV = ElectricPotentialUnit("mV")


class SEScalarElectricPotential(SEScalarQuantity):

    def __init__(self, value: float = None, units: ElectricPotentialUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarElectricPotential):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarElectricPotential")

    def get_value(self, units: ElectricPotentialUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: ElectricPotentialUnit):
        if isinstance(units, ElectricPotentialUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a ElectricPotentialUnit")


class EnergyUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == EnergyUnit.J.get_string():
            return EnergyUnit.J
        if string == EnergyUnit.mJ.get_string():
            return EnergyUnit.mJ
        if string == EnergyUnit.kJ.get_string():
            return EnergyUnit.kJ
        if string == EnergyUnit.kcal.get_string():
            return EnergyUnit.kcal
        raise ScalarException(f"No EnergyUnit defined for {string}")

    __slots__ = ["J", "mJ", "kJ", "kcal"]
    
    
EnergyUnit.J = EnergyUnit("J")
EnergyUnit.mJ = EnergyUnit("mJ")
EnergyUnit.kJ = EnergyUnit("kJ")
EnergyUnit.kcal = EnergyUnit("kcal")


class SEScalarEnergy(SEScalarQuantity):

    def __init__(self, value: float = None, units: EnergyUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarEnergy):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarEnergy")

    def get_value(self, units: EnergyUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: EnergyUnit):
        if isinstance(units, EnergyUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a EnergyUnit")


class EquivalentWeightPerVolumeUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == EquivalentWeightPerVolumeUnit.Eq_Per_L.get_string():
            return EquivalentWeightPerVolumeUnit.Eq_Per_L
        if string == EquivalentWeightPerVolumeUnit.Eq_Per_mL.get_string():
            return EquivalentWeightPerVolumeUnit.Eq_Per_mL
        if string == EquivalentWeightPerVolumeUnit.mEq_Per_L.get_string():
            return EquivalentWeightPerVolumeUnit.mEq_Per_L
        if string == EquivalentWeightPerVolumeUnit.mEq_Per_mL.get_string():
            return EquivalentWeightPerVolumeUnit.mEq_Per_mL
        raise ScalarException("No EquivalentWeightPerVolumeUnit defined for " + string)

    __slots__ = ["Eq_Per_L", "Eq_Per_mL", "mEq_Per_L", "mEq_Per_mL"]
    
    
EquivalentWeightPerVolumeUnit.Eq_Per_L = EquivalentWeightPerVolumeUnit("Eq/L")
EquivalentWeightPerVolumeUnit.Eq_Per_mL = EquivalentWeightPerVolumeUnit("Eq/mL")
EquivalentWeightPerVolumeUnit.mEq_Per_L = EquivalentWeightPerVolumeUnit("mEq/L")
EquivalentWeightPerVolumeUnit.mEq_Per_mL = EquivalentWeightPerVolumeUnit("mEq/mL")


class SEScalarEquivalentWeightPerVolume(SEScalarQuantity):

    def __init__(self, value: float = None, units: EquivalentWeightPerVolumeUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarEquivalentWeightPerVolume):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarEquivalentWeightPerVolume")

    def get_value(self, units: EquivalentWeightPerVolumeUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: EquivalentWeightPerVolumeUnit):
        if isinstance(units, EquivalentWeightPerVolumeUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a EquivalentWeightPerVolumeUnit")


class ForceUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == ForceUnit.N.get_string():
            return ForceUnit.N
        if string == ForceUnit.lbf.get_string():
            return ForceUnit.lbf
        if string == ForceUnit.dyn.get_string():
            return ForceUnit.dyn
        raise ScalarException("No ForceUnit defined for " + string)

    __slots__ = ["N", "lbf", "dyn"]
    
    
ForceUnit.N = ForceUnit("N")
ForceUnit.lbf = ForceUnit("lbf")
ForceUnit.dyn = ForceUnit("dyn")


class SEScalarForce(SEScalarQuantity):

    def __init__(self, value: float = None, units: ForceUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarForce):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarForce")

    def get_value(self, units: ForceUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: ForceUnit):
        if isinstance(units, ForceUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a ForceUnit")


class FrequencyUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == FrequencyUnit.Per_min.get_string():
            return FrequencyUnit.Per_min
        if string == FrequencyUnit.Per_s.get_string():
            return FrequencyUnit.Per_s
        if string == FrequencyUnit.Hz.get_string():
            return FrequencyUnit.Hz
        raise ScalarException("No FrequencyUnit defined for " + string)

    __slots__ = ["Per_min", "Per_s", "Hz"]


FrequencyUnit.Per_min = FrequencyUnit("1/min")
FrequencyUnit.Per_s = FrequencyUnit("1/s")
FrequencyUnit.Hz = FrequencyUnit("Hz")


class SEScalarFrequency(SEScalarQuantity):

    def __init__(self, value: float = None, units: FrequencyUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarFrequency):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarFrequency")

    def get_value(self, units: FrequencyUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: FrequencyUnit):
        if isinstance(units, FrequencyUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a FrequencyUnit")


class HeatCapacitancePerMassUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == HeatCapacitancePerMassUnit.J_Per_K_kg.get_string():
            return HeatCapacitancePerMassUnit.J_Per_K_kg
        if string == HeatCapacitancePerMassUnit.kJ_Per_K_kg.get_string():
            return HeatCapacitancePerMassUnit.kJ_Per_K_kg
        if string == HeatCapacitancePerMassUnit.kcal_Per_K_kg.get_string():
            return HeatCapacitancePerMassUnit.kcal_Per_K_kg
        if string == HeatCapacitancePerMassUnit.kcal_Per_C_kg.get_string():
            return HeatCapacitancePerMassUnit.kcal_Per_C_kg
        raise ScalarException(f"No HeatCapacitancePerMassUnit defined for {string}")

    __slots__ = ["J_Per_K_kg", "kJ_Per_K_kg", "kcal_Per_K_kg", "kcal_Per_C_kg"]


HeatCapacitancePerMassUnit.J_Per_K_kg = HeatCapacitancePerMassUnit("J/K kg")
HeatCapacitancePerMassUnit.kJ_Per_K_kg = HeatCapacitancePerMassUnit("kJ/K kg")
HeatCapacitancePerMassUnit.kcal_Per_K_kg = HeatCapacitancePerMassUnit("kcal/K kg")
HeatCapacitancePerMassUnit.kcal_Per_C_kg = HeatCapacitancePerMassUnit("kcal/degC kg")


class SEScalarHeatCapacitancePerMass(SEScalarQuantity):

    def __init__(self, value: float = None, units: HeatCapacitancePerMassUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarHeatCapacitancePerMass):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarHeatCapacitancePerMass")

    def get_value(self, units: HeatCapacitancePerMassUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: HeatCapacitancePerMassUnit):
        if isinstance(units, HeatCapacitancePerMassUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a HeatCapacitancePerMassUnit")


class HeatConductanceUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == HeatConductanceUnit.W_Per_K.get_string():
            return HeatConductanceUnit.W_Per_K
        if string == HeatConductanceUnit.W_Per_C.get_string():
            return HeatConductanceUnit.W_Per_C
        if string == HeatConductanceUnit.kcal_Per_K_s.get_string():
            return HeatConductanceUnit.kcal_Per_K_s
        if string == HeatConductanceUnit.kcal_Per_C_s.get_string():
            return HeatConductanceUnit.kcal_Per_C_s
        raise ScalarException(f"No HeatConductanceUnit defined for {string}")

    __slots__ = ["W_Per_K", "W_Per_C", "kcal_Per_K_s", "kcal_Per_C_s"]


HeatConductanceUnit.W_Per_K = HeatConductanceUnit("W/K")
HeatConductanceUnit.W_Per_C = HeatConductanceUnit("W/degC")
HeatConductanceUnit.kcal_Per_K_s = HeatConductanceUnit("kcal/K s")
HeatConductanceUnit.kcal_Per_C_s = HeatConductanceUnit("kcal/degC s")


class SEScalarHeatConductance(SEScalarQuantity):

    def __init__(self, value: float = None, units: HeatConductanceUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarHeatConductance):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarHeatConductance")

    def get_value(self, units: HeatConductanceUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: HeatConductanceUnit):
        if isinstance(units, HeatConductanceUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a HeatConductanceUnit")


class HeatConductancePerAreaUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == HeatConductancePerAreaUnit.W_Per_m2_K.get_string():
            return HeatConductancePerAreaUnit.W_Per_m2_K
        if string == HeatConductancePerAreaUnit.W_Per_m2_C.get_string():
            return HeatConductancePerAreaUnit.W_Per_m2_C
        if string == HeatConductancePerAreaUnit.BTU_Per_hr_ft2_C.get_string():
            return HeatConductancePerAreaUnit.BTU_Per_hr_ft2_C
        raise ScalarException(f"No HeatConductancePerAreaUnit defined for {string}")

    __slots__ = ["W_Per_m2_K", "W_Per_m2_C", "BTU_Per_hr_ft2_C"]


HeatConductancePerAreaUnit.W_Per_m2_K = HeatConductancePerAreaUnit("W/m^2 K")
HeatConductancePerAreaUnit.W_Per_m2_C = HeatConductancePerAreaUnit("W/m^2 degC")
HeatConductancePerAreaUnit.BTU_Per_hr_ft2_C = HeatConductancePerAreaUnit("BTU/hr ft^2 degC")


class SEScalarHeatConductancePerArea(SEScalarQuantity):

    def __init__(self, value: float = None, units: HeatConductancePerAreaUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarHeatConductancePerArea):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarHeatConductancePerArea")

    def get_value(self, units: HeatConductancePerAreaUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: HeatConductancePerAreaUnit):
        if isinstance(units, HeatConductancePerAreaUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a HeatConductancePerAreaUnit")


class HeatResistanceAreaUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == HeatResistanceAreaUnit.rsi.get_string():
            return HeatResistanceAreaUnit.rsi
        if string == HeatResistanceAreaUnit.clo.get_string():
            return HeatResistanceAreaUnit.clo
        if string == HeatResistanceAreaUnit.rValue.get_string():
            return HeatResistanceAreaUnit.rValue
        if string == HeatResistanceAreaUnit.tog.get_string():
            return HeatResistanceAreaUnit.tog
        raise ScalarException("No HeatResistanceAreaUnit defined for " + string)

    __slots__ = ["rsi", "clo", "rValue", "tog"]


HeatResistanceAreaUnit.rsi = HeatResistanceAreaUnit("rsi")
HeatResistanceAreaUnit.clo = HeatResistanceAreaUnit("clo")
HeatResistanceAreaUnit.rValue = HeatResistanceAreaUnit("rValue")
HeatResistanceAreaUnit.tog = HeatResistanceAreaUnit("tog")


class SEScalarHeatResistanceArea(SEScalarQuantity):

    def __init__(self, value: float = None, units: HeatResistanceAreaUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarHeatResistanceArea):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarHeatResistanceArea")

    def get_value(self, units: HeatResistanceAreaUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: HeatResistanceAreaUnit):
        if isinstance(units, HeatResistanceAreaUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a HeatResistanceAreaUnit")


class InversePressureUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == InversePressureUnit.Inverse_Pa.get_string():
            return InversePressureUnit.Inverse_Pa
        if string == InversePressureUnit.Inverse_mmHg.get_string():
            return InversePressureUnit.Inverse_mmHg
        if string == InversePressureUnit.Inverse_cmH2O.get_string():
            return InversePressureUnit.Inverse_cmH2O
        if string == InversePressureUnit.Inverse_atm.get_string():
            return InversePressureUnit.Inverse_atm
        raise ScalarException("No InversePressureUnit defined for " + string)

    __slots__ = ["Inverse_Pa", "Inverse_mmHg", "Inverse_cmH2O", "Inverse_atm"]


InversePressureUnit.Inverse_Pa = InversePressureUnit("1/Pa")
InversePressureUnit.Inverse_mmHg = InversePressureUnit("1/mmHg")
InversePressureUnit.Inverse_cmH2O = InversePressureUnit("1/cmH2O")
InversePressureUnit.Inverse_atm = InversePressureUnit("1/atm")


class SEScalarInversePressure(SEScalarQuantity):

    def __init__(self, value: float = None, units: InversePressureUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarInversePressure):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarInversePressure")

    def get_value(self, units: InversePressureUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: InversePressureUnit):
        if isinstance(units, InversePressureUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a InversePressureUnit")


class LengthUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == LengthUnit.m.get_string():
            return LengthUnit.m
        if string == LengthUnit.cm.get_string():
            return LengthUnit.cm
        if string == LengthUnit.mm.get_string():
            return LengthUnit.mm
        if string == LengthUnit.um.get_string():
            return LengthUnit.um
        if string == LengthUnit.inch.get_string():
            return LengthUnit.inch
        if string == LengthUnit.ft.get_string():
            return LengthUnit.ft
        raise ScalarException("No LengthUnit defined for " + string)

    __slots__ = ["m", "cm", "mm", "um", "inch", "ft"]


LengthUnit.m = LengthUnit("m")
LengthUnit.cm = LengthUnit("cm")
LengthUnit.mm = LengthUnit("mm")
LengthUnit.um = LengthUnit("um")
LengthUnit.inch = LengthUnit("in")
LengthUnit.ft = LengthUnit("ft")


class SEScalarLength(SEScalarQuantity):

    def __init__(self, value: float = None, units: LengthUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarLength):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarLength")

    def get_value(self, units: LengthUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: LengthUnit):
        if isinstance(units, LengthUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a LengthUnit")


class LengthPerTimeUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == LengthPerTimeUnit.m_Per_s.get_string():
            return LengthPerTimeUnit.m_Per_s
        if string == LengthPerTimeUnit.cm_Per_s.get_string():
            return LengthPerTimeUnit.cm_Per_s
        if string == LengthPerTimeUnit.m_Per_min.get_string():
            return LengthPerTimeUnit.m_Per_min
        if string == LengthPerTimeUnit.cm_Per_min.get_string():
            return LengthPerTimeUnit.cm_Per_min
        if string == LengthPerTimeUnit.ft_Per_s.get_string():
            return LengthPerTimeUnit.ft_Per_s
        if string == LengthPerTimeUnit.ft_Per_min.get_string():
            return LengthPerTimeUnit.ft_Per_min
        if string == LengthPerTimeUnit.km_Per_hr.get_string():
            return LengthPerTimeUnit.km_Per_hr
        raise ScalarException("No LengthPerTimeUnit defined for " + string)

    __slots__ = ["m_Per_s", "cm_Per_s", "m_Per_min", "cm_Per_min", "ft_Per_s", "ft_Per_min", "km_Per_hr"]


LengthPerTimeUnit.m_Per_s = LengthPerTimeUnit("m/s")
LengthPerTimeUnit.cm_Per_s = LengthPerTimeUnit("cm/s")
LengthPerTimeUnit.m_Per_min = LengthPerTimeUnit("m/min")
LengthPerTimeUnit.cm_Per_min = LengthPerTimeUnit("cm/min")
LengthPerTimeUnit.ft_Per_s = LengthPerTimeUnit("ft/s")
LengthPerTimeUnit.ft_Per_min = LengthPerTimeUnit("ft/min")
LengthPerTimeUnit.km_Per_hr = LengthPerTimeUnit("km/hr")


class SEScalarLengthPerTime(SEScalarQuantity):

    def __init__(self, value: float = None, units: LengthPerTimeUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarLengthPerTime):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarLengthPerTime")

    def get_value(self, units: LengthPerTimeUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: LengthPerTimeUnit):
        if isinstance(units, LengthPerTimeUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a LengthUnit")


class MassUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == MassUnit.g.get_string():
            return MassUnit.g
        if string == MassUnit.ug.get_string():
            return MassUnit.ug
        if string == MassUnit.mg.get_string():
            return MassUnit.mg
        if string == MassUnit.kg.get_string():
            return MassUnit.kg
        if string == MassUnit.lb.get_string():
            return MassUnit.lb
        raise ScalarException("No MassUnit defined for " + string)

    __slots__ = ["g", "ug", "mg", "kg", "lb"]


MassUnit.g = MassUnit("g")
MassUnit.ug = MassUnit("ug")
MassUnit.mg = MassUnit("mg")
MassUnit.kg = MassUnit("kg")
MassUnit.lb = MassUnit("lb")


class SEScalarMass(SEScalarQuantity):

    def __init__(self, value: float = None, units: MassUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarMass):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarMass")

    def get_value(self, units: MassUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: MassUnit):
        if isinstance(units, MassUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a MassUnit")


class MassPerAmountUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == MassPerAmountUnit.g_Per_ct.get_string():
            return MassPerAmountUnit.g_Per_ct
        if string == MassPerAmountUnit.g_Per_mol.get_string():
            return MassPerAmountUnit.g_Per_mol
        if string == MassPerAmountUnit.g_Per_mmol.get_string():
            return MassPerAmountUnit.g_Per_mmol
        if string == MassPerAmountUnit.g_Per_umol.get_string():
            return MassPerAmountUnit.g_Per_umol
        if string == MassPerAmountUnit.mg_Per_mol.get_string():
            return MassPerAmountUnit.mg_Per_mol
        if string == MassPerAmountUnit.mg_Per_mmol.get_string():
            return MassPerAmountUnit.mg_Per_mmol
        if string == MassPerAmountUnit.kg_Per_mol.get_string():
            return MassPerAmountUnit.kg_Per_mol
        if string == MassPerAmountUnit.ug_Per_mmol.get_string():
            return MassPerAmountUnit.ug_Per_mmol
        if string == MassPerAmountUnit.ug_Per_mol.get_string():
            return MassPerAmountUnit.ug_Per_mol
        if string == MassPerAmountUnit.ug_Per_ct.get_string():
            return MassPerAmountUnit.ug_Per_ct
        if string == MassPerAmountUnit.pg_Per_ct.get_string():
            return MassPerAmountUnit.pg_Per_ct
        raise ScalarException("No MassPerAmountUnit defined for " + string)

    __slots__ = ["g_Per_ct", "g_Per_mol", "g_Per_mmol", "g_Per_umol",
                 "mg_Per_mol", "mg_Per_mmol", "kg_Per_mol", "ug_Per_mmol", "ug_Per_mol", "ug_Per_ct", "pg_Per_ct"]


MassPerAmountUnit.g_Per_ct = MassPerAmountUnit("g/ct")
MassPerAmountUnit.g_Per_mol = MassPerAmountUnit("g/mol")
MassPerAmountUnit.g_Per_mmol = MassPerAmountUnit("g/mmol")
MassPerAmountUnit.g_Per_umol = MassPerAmountUnit("g/umol")
MassPerAmountUnit.mg_Per_mol = MassPerAmountUnit("mg/mol")
MassPerAmountUnit.mg_Per_mmol = MassPerAmountUnit("mg/mmol")
MassPerAmountUnit.kg_Per_mol = MassPerAmountUnit("kg/mol")
MassPerAmountUnit.ug_Per_mmol = MassPerAmountUnit("ug/mmol")
MassPerAmountUnit.ug_Per_mol = MassPerAmountUnit("ug/mol")
MassPerAmountUnit.ug_Per_ct = MassPerAmountUnit("ug/ct")
MassPerAmountUnit.pg_Per_ct = MassPerAmountUnit("pg/ct")


class SEScalarMassPerAmount(SEScalarQuantity):

    def __init__(self, value: float = None, units: MassPerAmountUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarMassPerAmount):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarMassPerAmount")

    def get_value(self, units: MassPerAmountUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: MassPerAmountUnit):
        if isinstance(units, MassPerAmountUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a MassPerAmountUnit")


class MassPerAreaTimeUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == MassPerAreaTimeUnit.g_Per_cm2_s.get_string():
            return MassPerAreaTimeUnit.g_Per_cm2_s
        raise ScalarException("No MassPerAreaTimeUnit defined for " + string)

    __slots__ = ["g_Per_cm2_s"]


MassPerAreaTimeUnit.g_Per_cm2_s = MassPerAreaTimeUnit("g/cm^2 s")


class SEScalarMassPerAreaTime(SEScalarQuantity):

    def __init__(self, value: float = None, units: MassPerAreaTimeUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarMassPerAreaTime):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarMassPerAreaTime")

    def get_value(self, units: MassPerAreaTimeUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: MassPerAreaTimeUnit):
        if isinstance(units, MassPerAreaTimeUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a MassPerAreaTimeUnit")


class MassPerTimeUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == MassPerTimeUnit.g_Per_s.get_string():
            return MassPerTimeUnit.g_Per_s
        if string == MassPerTimeUnit.g_Per_min.get_string():
            return MassPerTimeUnit.g_Per_min
        if string == MassPerTimeUnit.g_Per_day.get_string():
            return MassPerTimeUnit.g_Per_day
        if string == MassPerTimeUnit.mg_Per_s.get_string():
            return MassPerTimeUnit.mg_Per_s
        if string == MassPerTimeUnit.mg_Per_min.get_string():
            return MassPerTimeUnit.mg_Per_min
        if string == MassPerTimeUnit.ug_Per_s.get_string():
            return MassPerTimeUnit.ug_Per_s
        if string == MassPerTimeUnit.kg_Per_s.get_string():
            return MassPerTimeUnit.kg_Per_s
        if string == MassPerTimeUnit.ug_Per_min.get_string():
            return MassPerTimeUnit.ug_Per_min
        raise ScalarException("No MassPerTimeUnit defined for " + string)

    __slots__ = ["g_Per_s", "g_Per_min", "g_Per_day", "mg_Per_s", "mg_Per_min", "ug_Per_s", "kg_Per_s", "ug_Per_min"]


MassPerTimeUnit.g_Per_s = MassPerTimeUnit("g/s")
MassPerTimeUnit.g_Per_min = MassPerTimeUnit("g/min")
MassPerTimeUnit.g_Per_day = MassPerTimeUnit("g/day")
MassPerTimeUnit.mg_Per_s = MassPerTimeUnit("mg/s")
MassPerTimeUnit.mg_Per_min = MassPerTimeUnit("mg/min")
MassPerTimeUnit.ug_Per_s = MassPerTimeUnit("ug/s")
MassPerTimeUnit.kg_Per_s = MassPerTimeUnit("kg/s")
MassPerTimeUnit.ug_Per_min = MassPerTimeUnit("ug/min")


class SEScalarMassPerTime(SEScalarQuantity):

    def __init__(self, value: float = None, units: MassPerTimeUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarMassPerTime):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarMassPerTime")

    def get_value(self, units: MassPerTimeUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: MassPerTimeUnit):
        if isinstance(units, MassPerTimeUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a MassPerTimeUnit")


class MassPerVolumeUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == MassPerVolumeUnit.g_Per_dL.get_string():
            return MassPerVolumeUnit.g_Per_dL
        if string == MassPerVolumeUnit.g_Per_cm3.get_string():
            return MassPerVolumeUnit.g_Per_cm3
        if string == MassPerVolumeUnit.g_Per_m3.get_string():
            return MassPerVolumeUnit.g_Per_m3
        if string == MassPerVolumeUnit.ug_Per_mL.get_string():
            return MassPerVolumeUnit.ug_Per_mL
        if string == MassPerVolumeUnit.mg_Per_m3.get_string():
            return MassPerVolumeUnit.mg_Per_m3
        if string == MassPerVolumeUnit.kg_Per_m3.get_string():
            return MassPerVolumeUnit.kg_Per_m3
        if string == MassPerVolumeUnit.ug_Per_L.get_string():
            return MassPerVolumeUnit.ug_Per_L
        if string == MassPerVolumeUnit.g_Per_L.get_string():
            return MassPerVolumeUnit.g_Per_L
        if string == MassPerVolumeUnit.g_Per_mL.get_string():
            return MassPerVolumeUnit.g_Per_mL
        if string == MassPerVolumeUnit.mg_Per_mL.get_string():
            return MassPerVolumeUnit.mg_Per_mL
        if string == MassPerVolumeUnit.mg_Per_L.get_string():
            return MassPerVolumeUnit.mg_Per_L
        if string == MassPerVolumeUnit.mg_Per_dL.get_string():
            return MassPerVolumeUnit.mg_Per_dL
        if string == MassPerVolumeUnit.kg_Per_mL.get_string():
            return MassPerVolumeUnit.kg_Per_mL
        if string == MassPerVolumeUnit.kg_Per_L.get_string():
            return MassPerVolumeUnit.kg_Per_L
        raise ScalarException("No MassPerVolumeUnit defined for " + string)

    __slots__ = ["g_Per_dL", "g_Per_cm3", "g_Per_m3", "ug_Per_mL", "mg_Per_m3", "kg_Per_m3",
                 "ug_Per_L", "g_Per_L", "g_Per_mL", "mg_Per_mL", "mg_Per_L", "mg_Per_dL",
                 "kg_Per_mL", "kg_Per_L"]


MassPerVolumeUnit.g_Per_dL = MassPerVolumeUnit("g/dL")
MassPerVolumeUnit.g_Per_cm3 = MassPerVolumeUnit("g/cm^3")
MassPerVolumeUnit.g_Per_m3 = MassPerVolumeUnit("g/m^3")
MassPerVolumeUnit.ug_Per_mL = MassPerVolumeUnit("ug/mL")
MassPerVolumeUnit.mg_Per_m3 = MassPerVolumeUnit("mg/m^3")
MassPerVolumeUnit.kg_Per_m3 = MassPerVolumeUnit("kg/m^3")
MassPerVolumeUnit.ug_Per_L = MassPerVolumeUnit("ug/L")
MassPerVolumeUnit.g_Per_L = MassPerVolumeUnit("g/L")
MassPerVolumeUnit.g_Per_mL = MassPerVolumeUnit("g/mL")
MassPerVolumeUnit.mg_Per_mL = MassPerVolumeUnit("mg/mL")
MassPerVolumeUnit.mg_Per_L = MassPerVolumeUnit("mg/L")
MassPerVolumeUnit.mg_Per_dL = MassPerVolumeUnit("mg/dL")
MassPerVolumeUnit.kg_Per_mL = MassPerVolumeUnit("kg/mL")
MassPerVolumeUnit.kg_Per_L = MassPerVolumeUnit("kg/L")


class SEScalarMassPerVolume(SEScalarQuantity):

    def __init__(self, value: float = None, units: MassPerVolumeUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarMassPerVolume):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarMassPerVolume")

    def get_value(self, units: MassPerVolumeUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: MassPerVolumeUnit):
        if isinstance(units, MassPerVolumeUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a MassPerVolumeUnit")


class OsmolalityUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == OsmolalityUnit.Osm_Per_kg.get_string():
            return OsmolalityUnit.Osm_Per_kg
        if string == OsmolalityUnit.mOsm_Per_kg.get_string():
            return OsmolalityUnit.mOsm_Per_kg
        raise ScalarException(f"No OsmolalityUnit defined for {string}")

    __slots__ = ["Osm_Per_kg", "mOsm_Per_kg"]


OsmolalityUnit.Osm_Per_kg = OsmolalityUnit("Osm/kg")
OsmolalityUnit.mOsm_Per_kg = OsmolalityUnit("mOsm/kg")


class SEScalarOsmolality(SEScalarQuantity):

    def __init__(self, value: float = None, units: OsmolalityUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarOsmolality):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarOsmolality")

    def get_value(self, units: OsmolalityUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: OsmolalityUnit):
        if isinstance(units, OsmolalityUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a OsmolalityUnit")


class OsmolarityUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == OsmolarityUnit.Osm_Per_L.get_string():
            return OsmolarityUnit.Osm_Per_L
        if string == OsmolarityUnit.mOsm_Per_L.get_string():
            return OsmolarityUnit.mOsm_Per_L
        raise ScalarException(f"No OsmolarityUnit defined for {string}")

    __slots__ = ["Osm_Per_L", "mOsm_Per_L"]


OsmolarityUnit.Osm_Per_L = OsmolarityUnit("Osm/L")
OsmolarityUnit.mOsm_Per_L = OsmolarityUnit("mOsm/L")


class SEScalarOsmolarity(SEScalarQuantity):

    def __init__(self, value: float = None, units: OsmolarityUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarOsmolarity):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarOsmolarity")

    def get_value(self, units: OsmolarityUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: OsmolarityUnit):
        if isinstance(units, OsmolarityUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a OsmolarityUnit")


class PowerUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == PowerUnit.W.get_string():
            return PowerUnit.W
        if string == PowerUnit.kcal_Per_s.get_string():
            return PowerUnit.kcal_Per_s
        if string == PowerUnit.kcal_Per_hr.get_string():
            return PowerUnit.kcal_Per_hr
        if string == PowerUnit.kcal_Per_day.get_string():
            return PowerUnit.kcal_Per_day
        if string == PowerUnit.J_Per_s.get_string():
            return PowerUnit.J_Per_s
        if string == PowerUnit.BTU_Per_hr.get_string():
            return PowerUnit.BTU_Per_hr
        raise ScalarException("No PowerUnit defined for " + string)

    __slots__ = ["W", "kcal_Per_s", "kcal_Per_hr", "kcal_Per_day", "J_Per_s", "BTU_Per_hr"]


PowerUnit.W = PowerUnit("W")
PowerUnit.kcal_Per_s = PowerUnit("kcal/s")
PowerUnit.kcal_Per_hr = PowerUnit("kcal/hr")
PowerUnit.kcal_Per_day = PowerUnit("kcal/day")
PowerUnit.J_Per_s = PowerUnit("J/s")
PowerUnit.BTU_Per_hr = PowerUnit("BTU/hr")


class SEScalarPower(SEScalarQuantity):

    def __init__(self, value: float = None, units: PowerUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarPower):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarPower")

    def get_value(self, units: PowerUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: PowerUnit):
        if isinstance(units, PowerUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a PowerUnit")


class PressureUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == PressureUnit.Pa.get_string():
            return PressureUnit.Pa
        if string == PressureUnit.mmHg.get_string():
            return PressureUnit.mmHg
        if string == PressureUnit.cmH2O.get_string():
            return PressureUnit.cmH2O
        if string == PressureUnit.psi.get_string():
            return PressureUnit.psi
        if string == PressureUnit.atm.get_string():
            return PressureUnit.atm
        raise ScalarException("No PressureUnit defined for " + string)

    __slots__ = ["Pa", "mmHg", "cmH2O", "psi", "atm"]


PressureUnit.Pa = PressureUnit("Pa")
PressureUnit.mmHg = PressureUnit("mmHg")
PressureUnit.cmH2O = PressureUnit("cmH2O")
PressureUnit.psi = PressureUnit("psi")
PressureUnit.atm = PressureUnit("atm")


class SEScalarPressure(SEScalarQuantity):

    def __init__(self, value: float = None, units: PressureUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarPressure):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarPressure")

    def get_value(self, units: PressureUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: PressureUnit):
        if isinstance(units, PressureUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a PressureUnit")


class PressurePerVolumeUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == PressurePerVolumeUnit.mmHg_Per_mL.get_string():
            return PressurePerVolumeUnit.mmHg_Per_mL
        if string == PressurePerVolumeUnit.cmH2O_Per_mL.get_string():
            return PressurePerVolumeUnit.cmH2O_Per_mL
        if string == PressurePerVolumeUnit.cmH2O_Per_L.get_string():
            return PressurePerVolumeUnit.cmH2O_Per_L
        else:
            raise ScalarException("No PressurePerVolumeUnit defined for " + string)

    __slots__ = ["mmHg_Per_mL", "cmH2O_Per_mL", "cmH2O_Per_L"]


PressurePerVolumeUnit.mmHg_Per_mL = PressurePerVolumeUnit("mmHg/mL")
PressurePerVolumeUnit.cmH2O_Per_mL = PressurePerVolumeUnit("cmH2O/mL")
PressurePerVolumeUnit.cmH2O_Per_L = PressurePerVolumeUnit("cmH2O/L")


class SEScalarPressurePerVolume(SEScalarQuantity):

    def __init__(self, value: float = None, units: PressurePerVolumeUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarPressurePerVolume):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarPressurePerVolume")

    def get_value(self, units: PressurePerVolumeUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: PressurePerVolumeUnit):
        if isinstance(units, PressurePerVolumeUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a PressurePerVolumeUnit")


class PressureTimePerVolumeUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == PressureTimePerVolumeUnit.cmH2O_s_Per_L.get_string():
            return PressureTimePerVolumeUnit.cmH2O_s_Per_L
        if string == PressureTimePerVolumeUnit.cmH2O_s_Per_mL.get_string():
            return PressureTimePerVolumeUnit.cmH2O_s_Per_mL
        if string == PressureTimePerVolumeUnit.mmHg_s_Per_mL.get_string():
            return PressureTimePerVolumeUnit.mmHg_s_Per_mL
        if string == PressureTimePerVolumeUnit.mmHg_min_Per_mL.get_string():
            return PressureTimePerVolumeUnit.mmHg_min_Per_mL
        if string == PressureTimePerVolumeUnit.mmHg_min_Per_L.get_string():
            return PressureTimePerVolumeUnit.mmHg_min_Per_L
        if string == PressureTimePerVolumeUnit.Pa_s_Per_m3.get_string():
            return PressureTimePerVolumeUnit.Pa_s_Per_m3
        raise ScalarException("No PressureTimePerVolumeUnit defined for " + string)

    __slots__ = ["cmH2O_s_Per_L", "cmH2O_s_Per_mL", "mmHg_s_Per_mL", "mmHg_min_Per_mL", "mmHg_min_Per_L", "Pa_s_Per_m3"]


PressureTimePerVolumeUnit.cmH2O_s_Per_L = PressureTimePerVolumeUnit("cmH2O s/L")
PressureTimePerVolumeUnit.cmH2O_s_Per_mL = PressureTimePerVolumeUnit("cmH2O s/mL")
PressureTimePerVolumeUnit.mmHg_s_Per_mL = PressureTimePerVolumeUnit("mmHg s/mL")
PressureTimePerVolumeUnit.mmHg_min_Per_mL = PressureTimePerVolumeUnit("mmHg min/mL")
PressureTimePerVolumeUnit.mmHg_min_Per_L = PressureTimePerVolumeUnit("mmHg min/L")
PressureTimePerVolumeUnit.Pa_s_Per_m3 = PressureTimePerVolumeUnit("Pa s/m^3")


class SEScalarPressureTimePerVolume(SEScalarQuantity):

    def __init__(self, value: float = None, units: PressureTimePerVolumeUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarPressureTimePerVolume):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarPressureTimePerVolume")

    def get_value(self, units: PressureTimePerVolumeUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: PressureTimePerVolumeUnit):
        if isinstance(units, PressureTimePerVolumeUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a PressureTimePerVolumeUnit")


class PressureTimePerVolumeAreaUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == PressureTimePerVolumeAreaUnit.mmHg_min_Per_mL_m2.get_string():
            return PressureTimePerVolumeAreaUnit.mmHg_min_Per_mL_m2
        if string == PressureTimePerVolumeAreaUnit.mmHg_s_Per_mL_m2.get_string():
            return PressureTimePerVolumeAreaUnit.mmHg_s_Per_mL_m2
        if string == PressureTimePerVolumeAreaUnit.dyn_s_Per_cm5_m2.get_string():
            return PressureTimePerVolumeAreaUnit.dyn_s_Per_cm5_m2
        raise ScalarException(f"No PressureTimePerVolumeAreaUnit defined for {string}")

    __slots__ = ["mmHg_min_Per_mL_m2", "mmHg_s_Per_mL_m2", "dyn_s_Per_cm5_m2"]


PressureTimePerVolumeAreaUnit.mmHg_min_Per_mL_m2 = PressureTimePerVolumeAreaUnit("mmHg min/mL m^2")
PressureTimePerVolumeAreaUnit.mmHg_s_Per_mL_m2 = PressureTimePerVolumeAreaUnit("mmHg s/mL m^2")
PressureTimePerVolumeAreaUnit.dyn_s_Per_cm5_m2 = PressureTimePerVolumeAreaUnit("dyn s/cm^5 m^2")


class SEScalarPressureTimePerVolumeArea(SEScalarQuantity):

    def __init__(self, value: float = None, units: PressureTimePerVolumeAreaUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarPressureTimePerVolumeArea):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarPressureTimePerVolumeArea")

    def get_value(self, units: PressureTimePerVolumeAreaUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: PressureTimePerVolumeAreaUnit):
        if isinstance(units, PressureTimePerVolumeAreaUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a PressureTimePerVolumeAreaUnit")


class TemperatureUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == TemperatureUnit.F.get_string():
            return TemperatureUnit.F
        if string == TemperatureUnit.C.get_string():
            return TemperatureUnit.C
        if string == TemperatureUnit.K.get_string():
            return TemperatureUnit.K
        if string == TemperatureUnit.R.get_string():
            return TemperatureUnit.R
        raise ScalarException("No TemperatureUnit defined for " + string)

    __slots__ = ["F", "C", "K", "R"]


TemperatureUnit.F = TemperatureUnit("degF")
TemperatureUnit.C = TemperatureUnit("degC")
TemperatureUnit.K = TemperatureUnit("K")
TemperatureUnit.R = TemperatureUnit("degR")


class SEScalarTemperature(SEScalarQuantity):

    def __init__(self, value: float = None, units: TemperatureUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarTemperature):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarTemperature")

    def get_value(self, units: TemperatureUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: TemperatureUnit):
        if isinstance(units, TemperatureUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a TemperatureUnit")


class SEScalarUnsigned(SEScalar):
    def set_value(self, value: float):
        if value < 0.0:
            raise ScalarException("ScalarUnsigned must be greater than or equal to 0")
        self._value = value


class TimeUnit(SEScalarUnit):

    @staticmethod
    def from_string(string: str):
        if string == TimeUnit.s.get_string():
            return TimeUnit.s
        if string == TimeUnit.min.get_string():
            return TimeUnit.min
        if string == TimeUnit.hr.get_string():
            return TimeUnit.hr
        if string == TimeUnit.day.get_string():
            return TimeUnit.day
        if string == TimeUnit.yr.get_string():
            return TimeUnit.yr
        raise ScalarException("No TimeUnit defined for " + string)

    __slots__ = ["s", "min", "hr", "day", "yr"]


TimeUnit.s = TimeUnit("s")
TimeUnit.min = TimeUnit("min")
TimeUnit.hr = TimeUnit("hr")
TimeUnit.day = TimeUnit("day")
TimeUnit.yr = TimeUnit("yr")


class SEScalarTime(SEScalarQuantity):

    def __init__(self, value: float = None, units: TimeUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarTime):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarTime")

    def get_value(self, units: TimeUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: TimeUnit):
        if isinstance(units, TimeUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a TimeUnit")


class VolumeUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == VolumeUnit.L.get_string():
            return VolumeUnit.L
        if string == VolumeUnit.mL.get_string():
            return VolumeUnit.mL
        if string == VolumeUnit.dL.get_string():
            return VolumeUnit.dL
        if string == VolumeUnit.m3.get_string():
            return VolumeUnit.m3
        if string == VolumeUnit.uL.get_string():
            return VolumeUnit.uL
        raise ScalarException("No VolumeUnit defined for " + string)

    __slots__ = ["L", "mL", "dL", "uL", "m3"]


VolumeUnit.L = VolumeUnit("L")
VolumeUnit.mL = VolumeUnit("mL")
VolumeUnit.dL = VolumeUnit("dL")
VolumeUnit.uL = VolumeUnit("uL")
VolumeUnit.m3 = VolumeUnit("m^3")


class SEScalarVolume(SEScalarQuantity):

    def __init__(self, value: float = None, units: VolumeUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarVolume):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarVolume")

    def get_value(self, units: VolumeUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: VolumeUnit):
        if isinstance(units, VolumeUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a VolumeUnit")


class VolumePerPressureUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == VolumePerPressureUnit.L_Per_Pa.get_string():
            return VolumePerPressureUnit.L_Per_Pa
        if string == VolumePerPressureUnit.L_Per_cmH2O.get_string():
            return VolumePerPressureUnit.L_Per_cmH2O
        if string == VolumePerPressureUnit.mL_Per_cmH2O.get_string():
            return VolumePerPressureUnit.mL_Per_cmH2O
        if string == VolumePerPressureUnit.m3_Per_Pa.get_string():
            return VolumePerPressureUnit.m3_Per_Pa
        if string == VolumePerPressureUnit.mL_Per_mmHg.get_string():
            return VolumePerPressureUnit.mL_Per_mmHg
        raise ScalarException("No VolumePerPressureUnit defined for " + string)

    __slots__ = ["L_Per_Pa", "L_Per_cmH2O", "mL_Per_cmH2O", "m3_Per_Pa", "mL_Per_mmHg"]


VolumePerPressureUnit.L_Per_Pa = VolumePerPressureUnit("L/Pa")
VolumePerPressureUnit.L_Per_cmH2O = VolumePerPressureUnit("L/cmH2O")
VolumePerPressureUnit.mL_Per_cmH2O = VolumePerPressureUnit("mL/cmH2O")
VolumePerPressureUnit.m3_Per_Pa = VolumePerPressureUnit("m^3/Pa")
VolumePerPressureUnit.mL_Per_mmHg = VolumePerPressureUnit("mL/mmHg")


class SEScalarVolumePerPressure(SEScalarQuantity):

    def __init__(self, value: float = None, units: VolumePerPressureUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarVolumePerPressure):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarVolumePerPressure")

    def get_value(self, units: VolumePerPressureUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: VolumePerPressureUnit):
        if isinstance(units, VolumePerPressureUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a VolumePerPressureUnit")


class VolumePerTimeUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == VolumePerTimeUnit.L_Per_s.get_string():
            return VolumePerTimeUnit.L_Per_s
        if string == VolumePerTimeUnit.L_Per_day.get_string():
            return VolumePerTimeUnit.L_Per_day
        if string == VolumePerTimeUnit.mL_Per_s.get_string():
            return VolumePerTimeUnit.mL_Per_s
        if string == VolumePerTimeUnit.mL_Per_day.get_string():
            return VolumePerTimeUnit.mL_Per_day
        if string == VolumePerTimeUnit.L_Per_min.get_string():
            return VolumePerTimeUnit.L_Per_min
        if string == VolumePerTimeUnit.m3_Per_s.get_string():
            return VolumePerTimeUnit.m3_Per_s
        if string == VolumePerTimeUnit.mL_Per_min.get_string():
            return VolumePerTimeUnit.mL_Per_min
        if string == VolumePerTimeUnit.mL_Per_hr.get_string():
            return VolumePerTimeUnit.mL_Per_hr
        raise ScalarException("No VolumePerTimeUnit defined for "+string)

    __slots__ = ["L_Per_s", "L_Per_day", "mL_Per_s", "mL_Per_day", "L_Per_min", "m3_Per_s", "mL_Per_min", "mL_Per_hr"]


VolumePerTimeUnit.L_Per_s = VolumePerTimeUnit("L/s")
VolumePerTimeUnit.L_Per_day = VolumePerTimeUnit("L/day")
VolumePerTimeUnit.mL_Per_s = VolumePerTimeUnit("mL/s")
VolumePerTimeUnit.mL_Per_day = VolumePerTimeUnit("mL/day")
VolumePerTimeUnit.L_Per_min = VolumePerTimeUnit("L/min")
VolumePerTimeUnit.m3_Per_s = VolumePerTimeUnit("m^3/s")
VolumePerTimeUnit.mL_Per_min = VolumePerTimeUnit("mL/min")
VolumePerTimeUnit.mL_Per_hr = VolumePerTimeUnit("mL/hr")


class SEScalarVolumePerTime(SEScalarQuantity):

    def __init__(self, value: float = None, units: VolumePerTimeUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarVolumePerTime):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarVolumePerTime")

    def get_value(self, units: VolumePerTimeUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: VolumePerTimeUnit):
        if isinstance(units, VolumePerTimeUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a VolumePerTimeUnit")


class VolumePerTimeAreaUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == VolumePerTimeAreaUnit.mL_Per_min_m2.get_string():
            return VolumePerTimeAreaUnit.mL_Per_min_m2
        if string == VolumePerTimeAreaUnit.mL_Per_s_m2.get_string():
            return VolumePerTimeAreaUnit.mL_Per_s_m2
        if string == VolumePerTimeAreaUnit.L_Per_min_m2.get_string():
            return VolumePerTimeAreaUnit.L_Per_min_m2
        raise ScalarException(f"No VolumePerTimeAreaUnit defined for {string}")

    __slots__ = ["mL_Per_min_m2", "mL_Per_s_m2", "L_Per_min_m2"]


VolumePerTimeAreaUnit.mL_Per_min_m2 = VolumePerTimeAreaUnit("mL/min m^2")
VolumePerTimeAreaUnit.mL_Per_s_m2 = VolumePerTimeAreaUnit("mL/s m^2")
VolumePerTimeAreaUnit.L_Per_min_m2 = VolumePerTimeAreaUnit("L/min m^2")


class SEScalarVolumePerTimeArea(SEScalarQuantity):

    def __init__(self, value: float = None, units: VolumePerTimeAreaUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarVolumePerTimeArea):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarVolumePerTimeArea")

    def get_value(self, units: VolumePerTimeAreaUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: VolumePerTimeAreaUnit):
        if isinstance(units, VolumePerTimeAreaUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a VolumePerTimeAreaUnit")


class VolumePerTimeMassUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == VolumePerTimeMassUnit.L_Per_s_g.get_string():
            return VolumePerTimeMassUnit.L_Per_s_g
        if string == VolumePerTimeMassUnit.mL_Per_s_g.get_string():
            return VolumePerTimeMassUnit.mL_Per_s_g
        if string == VolumePerTimeMassUnit.mL_Per_min_kg.get_string():
            return VolumePerTimeMassUnit.mL_Per_min_kg
        if string == VolumePerTimeMassUnit.mL_Per_s_kg.get_string():
            return VolumePerTimeMassUnit.mL_Per_s_kg
        if string == VolumePerTimeMassUnit.uL_Per_min_kg.get_string():
            return VolumePerTimeMassUnit.uL_Per_min_kg
        raise ScalarException("No VolumePerTimeMassUnit defined for " + string)

    __slots__ = ["L_Per_s_g", "mL_Per_s_g", "mL_Per_min_kg", "mL_Per_s_kg", "uL_Per_min_kg"]


VolumePerTimeMassUnit.L_Per_s_g = VolumePerTimeMassUnit("L/s g")
VolumePerTimeMassUnit.mL_Per_s_g = VolumePerTimeMassUnit("mL/s g")
VolumePerTimeMassUnit.mL_Per_min_kg = VolumePerTimeMassUnit("mL/min kg")
VolumePerTimeMassUnit.mL_Per_s_kg = VolumePerTimeMassUnit("mL/s kg")
VolumePerTimeMassUnit.uL_Per_min_kg = VolumePerTimeMassUnit("uL/min kg")


class SEScalarVolumePerTimeMass(SEScalarQuantity):

    def __init__(self, value: float = None, units: VolumePerTimeMassUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarVolumePerTimeMass):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarVolumePerTimeMass")

    def get_value(self, units: VolumePerTimeMassUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: VolumePerTimeMassUnit):
        if isinstance(units, VolumePerTimeMassUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a VolumePerTimeMassUnit")


class VolumePerTimePressureUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == VolumePerTimePressureUnit.L_Per_s_mmHg.get_string():
            return VolumePerTimePressureUnit.L_Per_s_mmHg
        if string == VolumePerTimePressureUnit.mL_Per_s_mmHg.get_string():
            return VolumePerTimePressureUnit.mL_Per_s_mmHg
        if string == VolumePerTimePressureUnit.L_Per_min_mmHg.get_string():
            return VolumePerTimePressureUnit.L_Per_min_mmHg
        if string == VolumePerTimePressureUnit.mL_Per_min_mmHg.get_string():
            return VolumePerTimePressureUnit.mL_Per_min_mmHg
        raise ScalarException("No VolumePerTimePressureUnit defined for " + string)

    __slots__ = ["L_Per_s_mmHg", "mL_Per_s_mmHg", "L_Per_min_mmHg", "mL_Per_min_mmHg"]


VolumePerTimePressureUnit.L_Per_s_mmHg = VolumePerTimePressureUnit("L/s mmHg")
VolumePerTimePressureUnit.mL_Per_s_mmHg = VolumePerTimePressureUnit("mL/s mmHg")
VolumePerTimePressureUnit.L_Per_min_mmHg = VolumePerTimePressureUnit("L/min mmHg")
VolumePerTimePressureUnit.mL_Per_min_mmHg = VolumePerTimePressureUnit("mL/min mmHg")


class SEScalarVolumePerTimePressure(SEScalarQuantity):

    def __init__(self, value: float = None, units: VolumePerTimePressureUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarVolumePerTimePressure):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarVolumePerTimePressure")

    def get_value(self, units: VolumePerTimePressureUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: VolumePerTimePressureUnit):
        if isinstance(units, VolumePerTimePressureUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a VolumePerTimePressureUnit")


class VolumePerTimePressureAreaUnit(SEScalarUnit):
    @staticmethod
    def from_string(string: str):
        if string == VolumePerTimePressureAreaUnit.mL_Per_min_mmHg_m2.get_string():
            return VolumePerTimePressureAreaUnit.mL_Per_min_mmHg_m2
        if string == VolumePerTimePressureAreaUnit.mL_Per_s_mmHg_m2.get_string():
            return VolumePerTimePressureAreaUnit.mL_Per_s_mmHg_m2
        raise ScalarException("No VolumePerTimePressureAreaUnit defined for " + string)

    __slots__ = ["mL_Per_min_mmHg_m2", "mL_Per_s_mmHg_m2"]


VolumePerTimePressureAreaUnit.mL_Per_min_mmHg_m2 = VolumePerTimePressureAreaUnit("mL/min mmHg m^2")
VolumePerTimePressureAreaUnit.mL_Per_s_mmHg_m2 = VolumePerTimePressureAreaUnit("mL/s mmHg m^2")


class SEScalarVolumePerTimePressureArea(SEScalarQuantity):

    def __init__(self, value: float = None, units: VolumePerTimePressureAreaUnit = None):
        super().__init__(value, units)

    def set(self, scalar):
        if isinstance(scalar, SEScalarVolumePerTimePressureArea):
            self._value = scalar._value
            self._units = scalar._units
        else:
            raise ScalarException("Provided argument must be a SEScalarVolumePerTimePressureArea")

    def get_value(self, units: VolumePerTimePressureAreaUnit = None):
        if self.is_valid() is False:
            return None
        if units is None:
            return self._value
        if self._units is units:
            return self._value
        else:
            return PyPulse.convert(self._value, str(self._units), str(units))

    def set_value(self, value: float, units: VolumePerTimePressureAreaUnit):
        if isinstance(units, VolumePerTimePressureAreaUnit):
            self._value = value
            self._units = units
        else:
            raise ScalarException("Provided argument must be a VolumePerTimePressureAreaUnit")


def get_unit(u: str):
    if not u or u == "unitless":
        return None

    fns = [
        AmountPerTimeUnit.from_string,
        AmountPerVolumeUnit.from_string,
        AreaUnit.from_string,
        EnergyUnit.from_string,
        ElectricPotentialUnit.from_string,
        EquivalentWeightPerVolumeUnit.from_string,
        ForceUnit.from_string,
        FrequencyUnit.from_string,
        HeatCapacitancePerMassUnit.from_string,
        HeatConductanceUnit.from_string,
        HeatConductancePerAreaUnit.from_string,
        HeatResistanceAreaUnit.from_string,
        InversePressureUnit.from_string,
        LengthUnit.from_string,
        LengthPerTimeUnit.from_string,
        MassUnit.from_string,
        MassPerAmountUnit.from_string,
        MassPerAreaTimeUnit.from_string,
        MassPerTimeUnit.from_string,
        MassPerVolumeUnit.from_string,
        OsmolalityUnit.from_string,
        OsmolarityUnit.from_string,
        PowerUnit.from_string,
        PressureUnit.from_string,
        PressurePerVolumeUnit.from_string,
        PressureTimePerVolumeUnit.from_string,
        PressureTimePerVolumeAreaUnit.from_string,
        TemperatureUnit.from_string,
        TimeUnit.from_string,
        VolumeUnit.from_string,
        VolumePerPressureUnit.from_string,
        VolumePerTimeUnit.from_string,
        VolumePerTimeAreaUnit.from_string,
        VolumePerTimeMassUnit.from_string,
        VolumePerTimePressureUnit.from_string,
        VolumePerTimePressureAreaUnit.from_string,
    ]
    for f in fns:
        try:
            return f(u)
        except ScalarException:
            pass

    raise ScalarException(f"Provided unit string could not be identified: {u}")


def main():
    area = SEScalarArea()
    area.set_value(5, AreaUnit.m2)
    cm2 = area.get_value(AreaUnit.cm2)
    print(cm2)


if __name__ == "__main__":
    main()
