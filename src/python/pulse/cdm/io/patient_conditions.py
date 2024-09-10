# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

from pulse.cdm.io.condition import *

from pulse.cdm.patient_conditions import *
from pulse.cdm.bind.PatientConditions_pb2 import *
from pulse.cdm.io.scalars import *

#################################################################
def serialize_patient_condition_to_bind(src: SEPatientCondition, dst: PatientConditionData):
    serialize_condition_to_bind(src,dst.Condition)

def serialize_patient_condition_from_bind(src: PatientConditionData, dst: SEPatientCondition):
    raise Exception("serialize_patient_condition_from_bind not implemented")

#################################################################
def serialize_acute_respiratory_distress_syndrome_to_bind(src: SEAcuteRespiratoryDistressSyndrome,
                                                          dst: AcuteRespiratoryDistressSyndromeData):
    serialize_patient_condition_to_bind(src, dst.PatientCondition)
    for c,s in src._severities.items():
        i = dst.Severity.add()
        i.Compartment = c.value
        serialize_scalar_0to1_to_bind(s, i.Severity)


def serialize_acute_respiratory_distress_syndrome_from_bind(src: AcuteRespiratoryDistressSyndromeData,
                                                            dst:SEAcuteRespiratoryDistressSyndrome):
    serialize_patient_condition_from_bind(src.PatientCondition, dst)
    raise Exception("serialize_acute_respiratory_distress_syndrome_from_bind not implemented")

#################################################################

def serialize_chronic_anemia_to_bind(src: SEChronicAnemia , dst: ChronicAnemiaData):
    serialize_patient_condition_to_bind(src, dst.PatientCondition)
    if src.has_reduction_factor():
        serialize_scalar_0to1_to_bind(src.get_reduction_factor(), dst.ReductionFactor)

def serialize_chronic_anemia_from_bind(src: ChronicAnemiaData , dst: SEChronicAnemia):
    serialize_patient_condition_from_bind(src.PatientCondition, dst)
    raise Exception("serialize_chronic_anemia_from_bind not implemented")

#################################################################

def serialize_chronic_obstructive_pulmonary_disease_to_bind(src: SEChronicObstructivePulmonaryDisease,
                                                            dst: ChronicObstructivePulmonaryDiseaseData):
    serialize_patient_condition_to_bind(src, dst.PatientCondition)
    if src.has_bronchitis_severity():
        serialize_scalar_0to1_to_bind(src.get_bronchitis_severity(), dst.BronchitisSeverity)
    for c, s in src._emphysema_severities.items():
        i = dst.EmphysemaSeverity.add()
        i.Compartment = c.value
        serialize_scalar_0to1_to_bind(s, i.Severity)

def serialize_chronic_obstructive_pulmonary_disease_from_bind(src: ChronicObstructivePulmonaryDiseaseData,
                                                              dst: SEChronicObstructivePulmonaryDisease):
    serialize_patient_condition_from_bind(src.PatientCondition, dst)
    raise Exception("serialize_chronic_obstructive_pulmonary_disease_from_bind not implemented")

#################################################################

def serialize_chronic_pericardial_effusion_to_bind(src: SEChronicPericardialEffusion,
                                                   dst: ChronicPericardialEffusionData):
    serialize_patient_condition_to_bind(src, dst.PatientCondition)
    if src.has_accumulated_volume():
        serialize_scalar_volume_to_bind(src.get_accumulated_volume(), dst.AccumulatedVolume)

def serialize_chronic_pericardial_effusion_from_bind(src: ChronicPericardialEffusionData,
                                                     dst: SEChronicPericardialEffusion):
    serialize_patient_condition_from_bind(src.PatientCondition, dst)
    raise Exception("serialize_chronic_pericardial_effusion_from_bind not implemented")

#################################################################

def serialize_chronic_renal_stenosis_to_bind(src: SEChronicRenalStenosis, dst: ChronicRenalStenosisData):
    serialize_patient_condition_to_bind(src, dst.PatientCondition)
    if src.has_left_kidney_severity():
        serialize_scalar_0to1_to_bind(src.get_left_kidney_severity(), dst.LeftKidneySeverity)
    if src.has_right_kidney_severity():
        serialize_scalar_0to1_to_bind(src.get_right_kidney_severity(), dst.RightKidneySeverity)

def serialize_chronic_renal_stenosis_from_bind(src: ChronicRenalStenosisData , dst: SEChronicRenalStenosis):
    serialize_patient_condition_from_bind(src.PatientCondition, dst)
    raise Exception("serialize_chronic_renal_stenosis_from_bind not implemented")

#################################################################

def serialize_chronic_ventricular_systolic_dysfunction_to_bind(src: SEChronicVentricularSystolicDysfunction, dst: ChronicVentricularSystolicDysfunctionData):
    serialize_patient_condition_to_bind(src, dst.PatientCondition)
    if len(dst.PatientCondition.Condition.Comment) == 0:
        dst.PatientCondition.Condition.Comment = "ChronicVentricularSystolicDysfunction Active" # Python Protobuf needs to have something modified in the structure to write it
        # Since CVSD has no fields, we need to modify the comment for Protobuf to add it to the message

def serialize_chronic_ventricular_systolic_dysfunction_from_bind(src: ChronicVentricularSystolicDysfunctionData, dst: SEChronicVentricularSystolicDysfunction):
    serialize_patient_condition_from_bind(src.PatientCondition, dst)
    raise Exception("serialize_chronic_ventricular_systolic_dysfunction_from_bind not implemented")

#################################################################

def serialize_dehydration_to_bind(src: SEDehydration , dst: DehydrationData):
    serialize_patient_condition_to_bind(src, dst.PatientCondition)
    if src.has_severity():
        serialize_scalar_0to1_to_bind(src.get_severity(), dst.Severity)

def serialize_dehydration_from_bind(src: DehydrationData, dst: SEDehydration):
    serialize_patient_condition_from_bind(src.PatientCondition, dst)
    raise Exception("serialize_dehydration_from_bind not implemented")

#################################################################

def serialize_impaired_alveolar_exchange_to_bind(src: SEImpairedAlveolarExchange , dst: ImpairedAlveolarExchangeData):
    serialize_patient_condition_to_bind(src, dst.PatientCondition)
    if src.has_impaired_fraction():
        serialize_scalar_0to1_to_bind(src.get_impaired_fraction(), dst.ImpairedFraction)
    if src.has_impaired_surface_area():
        serialize_scalar_area_to_bind(src.get_impaired_surface_area(), dst.ImpairedSurfaceArea)
    if src.has_severity():
        serialize_scalar_area_to_bind(src.get_severity(), dst.Severity)

def serialize_impaired_alveolar_exchange_from_bind(src: ImpairedAlveolarExchangeData, dst: SEImpairedAlveolarExchange):
    serialize_patient_condition_from_bind(src.PatientCondition, dst)
    raise Exception("serialize_impaired_alveolar_exchange_from_bind not implemented")

#################################################################

def serialize_pneumonia_to_bind(src: SEPneumonia, dst: PneumoniaData):
    serialize_patient_condition_to_bind(src, dst.PatientCondition)
    for c,s in src._severities.items():
        i = dst.Severity.add()
        i.Compartment = c.value
        serialize_scalar_0to1_to_bind(s, i.Severity)

def serialize_pneumonia_from_bind(src: PneumoniaData, dst: SEPneumonia):
    serialize_patient_condition_from_bind(src.PatientCondition, dst)
    raise Exception("serialize_pneumonia_from_bind not implemented")

#################################################################

def serialize_pulmonary_fibrosis_to_bind(src: SEPulmonaryFibrosis , dst: PulmonaryFibrosisData):
    serialize_patient_condition_to_bind(src, dst.PatientCondition)
    if src.has_severity():
      serialize_scalar_0to1_to_bind(src.get_severity(), dst.Severity)

def serialize_pulmonary_fibrosis_from_bind(src: PulmonaryFibrosisData, dst: SEPulmonaryFibrosis):
    serialize_patient_condition_from_bind(src.PatientCondition, dst)
    raise Exception("serialize_pulmonary_fibrosis_from_bind not implemented")

#################################################################

def serialize_pulmonary_shunt_to_bind(src: SEPulmonaryShunt , dst: PulmonaryShuntData):
    serialize_patient_condition_to_bind(src, dst.PatientCondition)
    if src.has_severity():
        serialize_scalar_0to1_to_bind(src.get_severity(), dst.Severity)

def serialize_pulmonary_shunt_from_bind(src: PulmonaryShuntData, dst: SEPulmonaryShunt):
    serialize_patient_condition_from_bind(src.PatientCondition, dst)
    raise Exception("serialize_pulmonary_shunt_from_bind not implemented")

#################################################################

def serialize_sepsis_to_bind(src: SESepsis, dst:SepsisData):
    serialize_patient_condition_to_bind(src, dst.PatientCondition)
    if src.has_severity():
        serialize_scalar_0to1_to_bind(src.get_severity(), dst.Severity)

def serialize_sepsis_from_bind(src: SepsisData, dst: SESepsis):
    serialize_patient_condition_from_bind(src.PatientCondition, dst)
    raise Exception("serialize_sepsis_from_bind not implemented")
