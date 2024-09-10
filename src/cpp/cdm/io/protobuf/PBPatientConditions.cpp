/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
PUSH_PROTO_WARNINGS
#include "pulse/cdm/bind/PatientConditions.pb.h"
POP_PROTO_WARNINGS
#include "cdm/io/protobuf/PBPatientConditions.h"
#include "cdm/io/protobuf/PBPatientNutrition.h"
#include "cdm/io/protobuf/PBConditions.h"
#include "cdm/io/protobuf/PBProperties.h"

#include "cdm/substance/SESubstanceManager.h"
#include "cdm/patient/conditions/SEAcuteRespiratoryDistressSyndrome.h"
#include "cdm/patient/conditions/SEChronicAnemia.h"
#include "cdm/patient/conditions/SEChronicObstructivePulmonaryDisease.h"
#include "cdm/patient/conditions/SEChronicPericardialEffusion.h"
#include "cdm/patient/conditions/SEChronicRenalStenosis.h"
#include "cdm/patient/conditions/SEChronicVentricularSystolicDysfunction.h"
#include "cdm/patient/conditions/SEConsumeMeal.h"
#include "cdm/patient/conditions/SEDehydration.h"
#include "cdm/patient/conditions/SEImpairedAlveolarExchange.h"
#include "cdm/patient/conditions/SEPneumonia.h"
#include "cdm/patient/conditions/SEPulmonaryFibrosis.h"
#include "cdm/patient/conditions/SEPulmonaryShunt.h"
#include "cdm/patient/conditions/SESepsis.h"

void PBPatientCondition::Serialize(const CDM_BIND::PatientConditionData& src, SEPatientCondition& dst)
{
  PBCondition::Serialize(src.condition(), dst);
}
void PBPatientCondition::Serialize(const SEPatientCondition& src, CDM_BIND::PatientConditionData& dst)
{
  PBCondition::Serialize(src, *dst.mutable_condition());
}

void PBPatientCondition::Load(const CDM_BIND::AcuteRespiratoryDistressSyndromeData& src, SEAcuteRespiratoryDistressSyndrome& dst)
{
  dst.Clear();
  PBPatientCondition::Serialize(src, dst);
}
void PBPatientCondition::Serialize(const CDM_BIND::AcuteRespiratoryDistressSyndromeData& src, SEAcuteRespiratoryDistressSyndrome& dst)
{
  PBPatientCondition::Serialize(src.patientcondition(), dst);
  for (int i = 0; i<src.severity_size(); i++)
  {
    auto& impairment = src.severity()[i];
    PBProperty::Load(impairment.severity(), dst.GetSeverity((eLungCompartment)impairment.compartment()));
  }
}
CDM_BIND::AcuteRespiratoryDistressSyndromeData* PBPatientCondition::Unload(const SEAcuteRespiratoryDistressSyndrome& src)
{
  CDM_BIND::AcuteRespiratoryDistressSyndromeData* dst = new CDM_BIND::AcuteRespiratoryDistressSyndromeData();
  PBPatientCondition::Serialize(src, *dst);
  return dst;
}
void PBPatientCondition::Serialize(const SEAcuteRespiratoryDistressSyndrome& src, CDM_BIND::AcuteRespiratoryDistressSyndromeData& dst)
{
  PBPatientCondition::Serialize(src, *dst.mutable_patientcondition());
  for (auto itr : src.m_Severities)
  {
    auto impairment = dst.mutable_severity()->Add();
    impairment->set_compartment((CDM_BIND::eLungCompartment)itr.first);
    impairment->set_allocated_severity(PBProperty::Unload(*itr.second));
  }
}
void PBPatientCondition::Copy(const SEAcuteRespiratoryDistressSyndrome& src, SEAcuteRespiratoryDistressSyndrome& dst)
{
  dst.Clear();
  CDM_BIND::AcuteRespiratoryDistressSyndromeData data;
  PBPatientCondition::Serialize(src, data);
  PBPatientCondition::Serialize(data, dst);
}

void PBPatientCondition::Load(const CDM_BIND::ChronicAnemiaData& src, SEChronicAnemia& dst)
{
  dst.Clear();
  PBPatientCondition::Serialize(src, dst);
}
void PBPatientCondition::Serialize(const CDM_BIND::ChronicAnemiaData& src, SEChronicAnemia& dst)
{
  PBPatientCondition::Serialize(src.patientcondition(), dst);
  if (src.has_reductionfactor())
    PBProperty::Load(src.reductionfactor(), dst.GetReductionFactor());
}
CDM_BIND::ChronicAnemiaData* PBPatientCondition::Unload(const SEChronicAnemia& src)
{
  CDM_BIND::ChronicAnemiaData* dst = new CDM_BIND::ChronicAnemiaData();
  PBPatientCondition::Serialize(src, *dst);
  return dst;
}
void PBPatientCondition::Serialize(const SEChronicAnemia& src, CDM_BIND::ChronicAnemiaData& dst)
{
  PBPatientCondition::Serialize(src, *dst.mutable_patientcondition());
  if (src.HasReductionFactor())
    dst.set_allocated_reductionfactor(PBProperty::Unload(*src.m_ReductionFactor));
}
void PBPatientCondition::Copy(const SEChronicAnemia& src, SEChronicAnemia& dst)
{
  dst.Clear();
  CDM_BIND::ChronicAnemiaData data;
  PBPatientCondition::Serialize(src, data);
  PBPatientCondition::Serialize(data, dst);
}

void PBPatientCondition::Load(const CDM_BIND::ChronicObstructivePulmonaryDiseaseData& src, SEChronicObstructivePulmonaryDisease& dst)
{
  dst.Clear();
  PBPatientCondition::Serialize(src, dst);
}
void PBPatientCondition::Serialize(const CDM_BIND::ChronicObstructivePulmonaryDiseaseData& src, SEChronicObstructivePulmonaryDisease& dst)
{
  PBPatientCondition::Serialize(src.patientcondition(), dst);
  if (src.has_bronchitisseverity())
    PBProperty::Load(src.bronchitisseverity(), dst.GetBronchitisSeverity());
  for (int i = 0; i<src.emphysemaseverity_size(); i++)
  {
    auto& impairment = src.emphysemaseverity()[i];
    PBProperty::Load(impairment.severity(), dst.GetEmphysemaSeverity((eLungCompartment)impairment.compartment()));
  }
}
CDM_BIND::ChronicObstructivePulmonaryDiseaseData* PBPatientCondition::Unload(const SEChronicObstructivePulmonaryDisease& src)
{
  CDM_BIND::ChronicObstructivePulmonaryDiseaseData* dst = new CDM_BIND::ChronicObstructivePulmonaryDiseaseData();
  PBPatientCondition::Serialize(src, *dst);
  return dst;
}
void PBPatientCondition::Serialize(const SEChronicObstructivePulmonaryDisease& src, CDM_BIND::ChronicObstructivePulmonaryDiseaseData& dst)
{
  PBPatientCondition::Serialize(src, *dst.mutable_patientcondition());
  if (src.HasBronchitisSeverity())
    dst.set_allocated_bronchitisseverity(PBProperty::Unload(*src.m_BronchitisSeverity));
  for (auto itr : src.m_EmphysemaSeverities)
  {
    auto impairment = dst.mutable_emphysemaseverity()->Add();
    impairment->set_compartment((CDM_BIND::eLungCompartment)itr.first);
    impairment->set_allocated_severity(PBProperty::Unload(*itr.second));
  }
}
void PBPatientCondition::Copy(const SEChronicObstructivePulmonaryDisease& src, SEChronicObstructivePulmonaryDisease& dst)
{
  dst.Clear();
  CDM_BIND::ChronicObstructivePulmonaryDiseaseData data;
  PBPatientCondition::Serialize(src, data);
  PBPatientCondition::Serialize(data, dst);
}

void PBPatientCondition::Load(const CDM_BIND::ChronicPericardialEffusionData& src, SEChronicPericardialEffusion& dst)
{
  dst.Clear();
  PBPatientCondition::Serialize(src, dst);
}
void PBPatientCondition::Serialize(const CDM_BIND::ChronicPericardialEffusionData& src, SEChronicPericardialEffusion& dst)
{
  PBPatientCondition::Serialize(src.patientcondition(), dst);
  if (src.has_accumulatedvolume())
    PBProperty::Load(src.accumulatedvolume(), dst.GetAccumulatedVolume());
}
CDM_BIND::ChronicPericardialEffusionData* PBPatientCondition::Unload(const SEChronicPericardialEffusion& src)
{
  CDM_BIND::ChronicPericardialEffusionData* dst = new CDM_BIND::ChronicPericardialEffusionData();
  PBPatientCondition::Serialize(src, *dst);
  return dst;
}
void PBPatientCondition::Serialize(const SEChronicPericardialEffusion& src, CDM_BIND::ChronicPericardialEffusionData& dst)
{
  PBPatientCondition::Serialize(src, *dst.mutable_patientcondition());
  if (src.HasAccumulatedVolume())
    dst.set_allocated_accumulatedvolume(PBProperty::Unload(*src.m_AccumulatedVolume));
}
void PBPatientCondition::Copy(const SEChronicPericardialEffusion& src, SEChronicPericardialEffusion& dst)
{
  dst.Clear();
  CDM_BIND::ChronicPericardialEffusionData data;
  PBPatientCondition::Serialize(src, data);
  PBPatientCondition::Serialize(data, dst);
}

void PBPatientCondition::Load(const CDM_BIND::ChronicRenalStenosisData& src, SEChronicRenalStenosis& dst)
{
  dst.Clear();
  PBPatientCondition::Serialize(src, dst);
}
void PBPatientCondition::Serialize(const CDM_BIND::ChronicRenalStenosisData& src, SEChronicRenalStenosis& dst)
{
  PBPatientCondition::Serialize(src.patientcondition(), dst);
  if (src.has_leftkidneyseverity())
    PBProperty::Load(src.leftkidneyseverity(), dst.GetLeftKidneySeverity());
  if (src.has_rightkidneyseverity())
    PBProperty::Load(src.rightkidneyseverity(), dst.GetRightKidneySeverity());
}
CDM_BIND::ChronicRenalStenosisData* PBPatientCondition::Unload(const SEChronicRenalStenosis& src)
{
  CDM_BIND::ChronicRenalStenosisData* dst = new CDM_BIND::ChronicRenalStenosisData();
  PBPatientCondition::Serialize(src, *dst);
  return dst;
}
void PBPatientCondition::Serialize(const SEChronicRenalStenosis& src, CDM_BIND::ChronicRenalStenosisData& dst)
{
  PBPatientCondition::Serialize(src, *dst.mutable_patientcondition());
  if (src.HasLeftKidneySeverity())
    dst.set_allocated_leftkidneyseverity(PBProperty::Unload(*src.m_LeftKidneySeverity));
  if (src.HasRightKidneySeverity())
    dst.set_allocated_rightkidneyseverity(PBProperty::Unload(*src.m_RightKidneySeverity));
}
void PBPatientCondition::Copy(const SEChronicRenalStenosis& src, SEChronicRenalStenosis& dst)
{
  dst.Clear();
  CDM_BIND::ChronicRenalStenosisData data;
  PBPatientCondition::Serialize(src, data);
  PBPatientCondition::Serialize(data, dst);
}

void PBPatientCondition::Load(const CDM_BIND::ChronicVentricularSystolicDysfunctionData& src, SEChronicVentricularSystolicDysfunction& dst)
{
  dst.Clear();
  PBPatientCondition::Serialize(src, dst);
}
void PBPatientCondition::Serialize(const CDM_BIND::ChronicVentricularSystolicDysfunctionData& src, SEChronicVentricularSystolicDysfunction& dst)
{
  PBPatientCondition::Serialize(src.patientcondition(), dst);
}
CDM_BIND::ChronicVentricularSystolicDysfunctionData* PBPatientCondition::Unload(const SEChronicVentricularSystolicDysfunction& src)
{
  CDM_BIND::ChronicVentricularSystolicDysfunctionData* dst = new CDM_BIND::ChronicVentricularSystolicDysfunctionData();
  PBPatientCondition::Serialize(src, *dst);
  return dst;
}
void PBPatientCondition::Serialize(const SEChronicVentricularSystolicDysfunction& src, CDM_BIND::ChronicVentricularSystolicDysfunctionData& dst)
{
  PBPatientCondition::Serialize(src, *dst.mutable_patientcondition());
}
void PBPatientCondition::Copy(const SEChronicVentricularSystolicDysfunction& src, SEChronicVentricularSystolicDysfunction& dst)
{
  dst.Clear();
  CDM_BIND::ChronicVentricularSystolicDysfunctionData data;
  PBPatientCondition::Serialize(src, data);
  PBPatientCondition::Serialize(data, dst);
}

void PBPatientCondition::Load(const CDM_BIND::ConsumeMealData& src, SEConsumeMeal& dst)
{
  dst.Clear();
  PBPatientCondition::Serialize(src, dst);
}
void PBPatientCondition::Serialize(const CDM_BIND::ConsumeMealData& src, SEConsumeMeal& dst)
{
  PBPatientCondition::Serialize(src.patientcondition(), dst);
  if(!src.mealfile().empty())
    dst.SetMealFile(src.mealfile());
  else if (src.has_meal())
    PBPatientNutrition::Load(src.meal(), dst.GetMeal());
}
CDM_BIND::ConsumeMealData* PBPatientCondition::Unload(const SEConsumeMeal& src)
{
  CDM_BIND::ConsumeMealData* dst = new CDM_BIND::ConsumeMealData();
  PBPatientCondition::Serialize(src, *dst);
  return dst;
}
void PBPatientCondition::Serialize(const SEConsumeMeal& src, CDM_BIND::ConsumeMealData& dst)
{
  PBPatientCondition::Serialize(src, *dst.mutable_patientcondition());
  if (src.HasMealFile())
    dst.set_mealfile(src.m_MealFile); 
  else if (src.HasMeal())
    dst.set_allocated_meal(PBPatientNutrition::Unload(*src.m_Meal));
}
void PBPatientCondition::Copy(const SEConsumeMeal& src, SEConsumeMeal& dst)
{
  dst.Clear();
  CDM_BIND::ConsumeMealData data;
  PBPatientCondition::Serialize(src, data);
  PBPatientCondition::Serialize(data, dst);
}

void PBPatientCondition::Load(const CDM_BIND::DehydrationData& src, SEDehydration& dst)
{
  dst.Clear();
  PBPatientCondition::Serialize(src, dst);
}
void PBPatientCondition::Serialize(const CDM_BIND::DehydrationData& src, SEDehydration& dst)
{
  PBPatientCondition::Serialize(src.patientcondition(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
}
CDM_BIND::DehydrationData* PBPatientCondition::Unload(const SEDehydration& src)
{
  CDM_BIND::DehydrationData* dst = new CDM_BIND::DehydrationData();
  PBPatientCondition::Serialize(src, *dst);
  return dst;
}
void PBPatientCondition::Serialize(const SEDehydration& src, CDM_BIND::DehydrationData& dst)
{
  PBPatientCondition::Serialize(src, *dst.mutable_patientcondition());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
}
void PBPatientCondition::Copy(const SEDehydration& src, SEDehydration& dst)
{
  dst.Clear();
  CDM_BIND::DehydrationData data;
  PBPatientCondition::Serialize(src, data);
  PBPatientCondition::Serialize(data, dst);
}

void PBPatientCondition::Load(const CDM_BIND::ImpairedAlveolarExchangeData& src, SEImpairedAlveolarExchange& dst)
{
  dst.Clear();
  PBPatientCondition::Serialize(src, dst);
}
void PBPatientCondition::Serialize(const CDM_BIND::ImpairedAlveolarExchangeData& src, SEImpairedAlveolarExchange& dst)
{
  PBPatientCondition::Serialize(src.patientcondition(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
  else if (src.has_impairedfraction())
    PBProperty::Load(src.impairedfraction(), dst.GetImpairedFraction());
  else if (src.has_impairedsurfacearea())
    PBProperty::Load(src.impairedsurfacearea(), dst.GetImpairedSurfaceArea());
}
CDM_BIND::ImpairedAlveolarExchangeData* PBPatientCondition::Unload(const SEImpairedAlveolarExchange& src)
{
  CDM_BIND::ImpairedAlveolarExchangeData* dst = new CDM_BIND::ImpairedAlveolarExchangeData();
  PBPatientCondition::Serialize(src, *dst);
  return dst;
}
void PBPatientCondition::Serialize(const SEImpairedAlveolarExchange& src, CDM_BIND::ImpairedAlveolarExchangeData& dst)
{
  PBPatientCondition::Serialize(src, *dst.mutable_patientcondition());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
  else if (src.HasImpairedFraction())
    dst.set_allocated_impairedfraction(PBProperty::Unload(*src.m_ImpairedFraction));
  else if (src.HasImpairedSurfaceArea())
    dst.set_allocated_impairedsurfacearea(PBProperty::Unload(*src.m_ImpairedSurfaceArea));
}
void PBPatientCondition::Copy(const SEImpairedAlveolarExchange& src, SEImpairedAlveolarExchange& dst)
{
  dst.Clear();
  CDM_BIND::ImpairedAlveolarExchangeData data;
  PBPatientCondition::Serialize(src, data);
  PBPatientCondition::Serialize(data, dst);
}

void PBPatientCondition::Load(const CDM_BIND::PneumoniaData& src, SEPneumonia& dst)
{
  dst.Clear();
  PBPatientCondition::Serialize(src, dst);
}
void PBPatientCondition::Serialize(const CDM_BIND::PneumoniaData& src, SEPneumonia& dst)
{
  PBPatientCondition::Serialize(src.patientcondition(), dst);
  for (int i = 0; i<src.severity_size(); i++)
  {
    auto& impairment = src.severity()[i];
    PBProperty::Load(impairment.severity(), dst.GetSeverity((eLungCompartment)impairment.compartment()));
  }
}
CDM_BIND::PneumoniaData* PBPatientCondition::Unload(const SEPneumonia& src)
{
  CDM_BIND::PneumoniaData* dst = new CDM_BIND::PneumoniaData();
  PBPatientCondition::Serialize(src, *dst);
  return dst;
}
void PBPatientCondition::Serialize(const SEPneumonia& src, CDM_BIND::PneumoniaData& dst)
{
  PBPatientCondition::Serialize(src, *dst.mutable_patientcondition());
  for (auto itr : src.m_Severities)
  {
    auto impairment = dst.mutable_severity()->Add();
    impairment->set_compartment((CDM_BIND::eLungCompartment)itr.first);
    impairment->set_allocated_severity(PBProperty::Unload(*itr.second));
  }
}
void PBPatientCondition::Copy(const SEPneumonia& src, SEPneumonia& dst)
{
  dst.Clear();
  CDM_BIND::PneumoniaData data;
  PBPatientCondition::Serialize(src, data);
  PBPatientCondition::Serialize(data, dst);
}

void PBPatientCondition::Load(const CDM_BIND::PulmonaryFibrosisData& src, SEPulmonaryFibrosis& dst)
{
  dst.Clear();
  PBPatientCondition::Serialize(src, dst);
}
void PBPatientCondition::Serialize(const CDM_BIND::PulmonaryFibrosisData& src, SEPulmonaryFibrosis& dst)
{
  PBPatientCondition::Serialize(src.patientcondition(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
}
CDM_BIND::PulmonaryFibrosisData* PBPatientCondition::Unload(const SEPulmonaryFibrosis& src)
{
  CDM_BIND::PulmonaryFibrosisData* dst = new CDM_BIND::PulmonaryFibrosisData();
  PBPatientCondition::Serialize(src, *dst);
  return dst;
}
void PBPatientCondition::Serialize(const SEPulmonaryFibrosis& src, CDM_BIND::PulmonaryFibrosisData& dst)
{
  PBPatientCondition::Serialize(src, *dst.mutable_patientcondition());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
}
void PBPatientCondition::Copy(const SEPulmonaryFibrosis& src, SEPulmonaryFibrosis& dst)
{
  dst.Clear();
  CDM_BIND::PulmonaryFibrosisData data;
  PBPatientCondition::Serialize(src, data);
  PBPatientCondition::Serialize(data, dst);
}

void PBPatientCondition::Load(const CDM_BIND::PulmonaryShuntData& src, SEPulmonaryShunt& dst)
{
  dst.Clear();
  PBPatientCondition::Serialize(src, dst);
}
void PBPatientCondition::Serialize(const CDM_BIND::PulmonaryShuntData& src, SEPulmonaryShunt& dst)
{
  PBPatientCondition::Serialize(src.patientcondition(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
}
CDM_BIND::PulmonaryShuntData* PBPatientCondition::Unload(const SEPulmonaryShunt& src)
{
  CDM_BIND::PulmonaryShuntData* dst = new CDM_BIND::PulmonaryShuntData();
  PBPatientCondition::Serialize(src, *dst);
  return dst;
}
void PBPatientCondition::Serialize(const SEPulmonaryShunt& src, CDM_BIND::PulmonaryShuntData& dst)
{
  PBPatientCondition::Serialize(src, *dst.mutable_patientcondition());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
}
void PBPatientCondition::Copy(const SEPulmonaryShunt& src, SEPulmonaryShunt& dst)
{
  dst.Clear();
  CDM_BIND::PulmonaryShuntData data;
  PBPatientCondition::Serialize(src, data);
  PBPatientCondition::Serialize(data, dst);
}

void PBPatientCondition::Load(const CDM_BIND::SepsisData& src, SESepsis& dst)
{
  dst.Clear();
  PBPatientCondition::Serialize(src, dst);
}
void PBPatientCondition::Serialize(const CDM_BIND::SepsisData& src, SESepsis& dst)
{
  PBPatientCondition::Serialize(src.patientcondition(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
}
CDM_BIND::SepsisData* PBPatientCondition::Unload(const SESepsis& src)
{
  CDM_BIND::SepsisData* dst = new CDM_BIND::SepsisData();
  PBPatientCondition::Serialize(src, *dst);
  return dst;
}
void PBPatientCondition::Serialize(const SESepsis& src, CDM_BIND::SepsisData& dst)
{
  PBPatientCondition::Serialize(src, *dst.mutable_patientcondition());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
}
void PBPatientCondition::Copy(const SESepsis& src, SESepsis& dst)
{
  dst.Clear();
  CDM_BIND::SepsisData data;
  PBPatientCondition::Serialize(src, data);
  PBPatientCondition::Serialize(data, dst);
}

SEPatientCondition* PBPatientCondition::Load(const CDM_BIND::AnyPatientConditionData& any, const SESubstanceManager& subMgr)
{
  switch (any.Condition_case())
  {
  case CDM_BIND::AnyPatientConditionData::ConditionCase::kAcuteRespiratoryDistressSyndrome:
  {
    SEAcuteRespiratoryDistressSyndrome* a = new SEAcuteRespiratoryDistressSyndrome();
    PBPatientCondition::Load(any.acuterespiratorydistresssyndrome(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientConditionData::ConditionCase::kChronicAnemia:
  {
    SEChronicAnemia* a = new SEChronicAnemia();
    PBPatientCondition::Load(any.chronicanemia(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientConditionData::ConditionCase::kChronicObstructivePulmonaryDisease:
  {
    SEChronicObstructivePulmonaryDisease* a = new SEChronicObstructivePulmonaryDisease();
    PBPatientCondition::Load(any.chronicobstructivepulmonarydisease(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientConditionData::ConditionCase::kChronicPericardialEffusion:
  {
    SEChronicPericardialEffusion* a = new SEChronicPericardialEffusion();
    PBPatientCondition::Load(any.chronicpericardialeffusion(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientConditionData::ConditionCase::kChronicRenalStenosis:
  {
    SEChronicRenalStenosis* a = new SEChronicRenalStenosis();
    PBPatientCondition::Load(any.chronicrenalstenosis(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientConditionData::ConditionCase::kChronicVentricularSystolicDysfunction:
  {
    SEChronicVentricularSystolicDysfunction* a = new SEChronicVentricularSystolicDysfunction();
    PBPatientCondition::Load(any.chronicventricularsystolicdysfunction(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientConditionData::ConditionCase::kConsumeMeal:
  {
    SEConsumeMeal* a = new SEConsumeMeal();
    PBPatientCondition::Load(any.consumemeal(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientConditionData::ConditionCase::kDehydration:
  {
    SEDehydration* a = new SEDehydration();
    PBPatientCondition::Load(any.dehydration(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientConditionData::ConditionCase::kImpairedAlveolarExchange:
  {
    SEImpairedAlveolarExchange* a = new SEImpairedAlveolarExchange();
    PBPatientCondition::Load(any.impairedalveolarexchange(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientConditionData::ConditionCase::kPneumonia:
  {
    SEPneumonia* a = new SEPneumonia();
    PBPatientCondition::Load(any.pneumonia(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientConditionData::ConditionCase::kPulmonaryFibrosis:
  {
    SEPulmonaryFibrosis* a = new SEPulmonaryFibrosis();
    PBPatientCondition::Load(any.pulmonaryfibrosis(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientConditionData::ConditionCase::kPulmonaryShunt:
  {
    SEPulmonaryShunt* a = new SEPulmonaryShunt();
    PBPatientCondition::Load(any.pulmonaryshunt(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientConditionData::ConditionCase::kSepsis:
  {
    SESepsis* a = new SESepsis();
    PBPatientCondition::Load(any.sepsis(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientConditionData::ConditionCase::CONDITION_NOT_SET:
  {
    subMgr.Warning("AnyPatientConditionData Condition is empty...was that intended?");
    return nullptr;
  }
  }
  subMgr.Error("Unknown condition type : " + std::to_string(any.Condition_case()));
  return nullptr;
}
CDM_BIND::AnyPatientConditionData* PBPatientCondition::Unload(const SEPatientCondition& condition)
{
  CDM_BIND::AnyPatientConditionData* any = new CDM_BIND::AnyPatientConditionData();
  const SEAcuteRespiratoryDistressSyndrome* ards = dynamic_cast<const SEAcuteRespiratoryDistressSyndrome*>(&condition);
  if (ards != nullptr)
  {
    any->set_allocated_acuterespiratorydistresssyndrome(PBPatientCondition::Unload(*ards));
    return any;
  }
  const SEChronicAnemia* ca = dynamic_cast<const SEChronicAnemia*>(&condition);
  if (ca != nullptr)
  {
    any->set_allocated_chronicanemia(PBPatientCondition::Unload(*ca));
    return any;
  }
  const SEChronicObstructivePulmonaryDisease* copd = dynamic_cast<const SEChronicObstructivePulmonaryDisease*>(&condition);
  if (copd != nullptr)
  {
    any->set_allocated_chronicobstructivepulmonarydisease(PBPatientCondition::Unload(*copd));
    return any;
  }
  const SEChronicPericardialEffusion* cpe = dynamic_cast<const SEChronicPericardialEffusion*>(&condition);
  if (cpe != nullptr)
  {
    any->set_allocated_chronicpericardialeffusion(PBPatientCondition::Unload(*cpe));
    return any;
  }
  const SEChronicRenalStenosis* crs = dynamic_cast<const SEChronicRenalStenosis*>(&condition);
  if (crs != nullptr)
  {
    any->set_allocated_chronicrenalstenosis(PBPatientCondition::Unload(*crs));
    return any;
  }
  const SEChronicVentricularSystolicDysfunction* cvsd = dynamic_cast<const SEChronicVentricularSystolicDysfunction*>(&condition);
  if (cvsd != nullptr)
  {
    any->set_allocated_chronicventricularsystolicdysfunction(PBPatientCondition::Unload(*cvsd));
    return any;
  }
  const SEConsumeMeal* cm = dynamic_cast<const SEConsumeMeal*>(&condition);
  if (cm != nullptr)
  {
    any->set_allocated_consumemeal(PBPatientCondition::Unload(*cm));
    return any;
  }
  const SEDehydration* d = dynamic_cast<const SEDehydration*>(&condition);
  if (d != nullptr)
  {
    any->set_allocated_dehydration(PBPatientCondition::Unload(*d));
    return any;
  }
  const SEImpairedAlveolarExchange* iae = dynamic_cast<const SEImpairedAlveolarExchange*>(&condition);
  if (iae != nullptr)
  {
    any->set_allocated_impairedalveolarexchange(PBPatientCondition::Unload(*iae));
    return any;
  }
  const SEPneumonia* lp = dynamic_cast<const SEPneumonia*>(&condition);
  if (lp != nullptr)
  {
    any->set_allocated_pneumonia(PBPatientCondition::Unload(*lp));
    return any;
  }
  const SEPulmonaryFibrosis* pf = dynamic_cast<const SEPulmonaryFibrosis*>(&condition);
  if (pf != nullptr)
  {
    any->set_allocated_pulmonaryfibrosis(PBPatientCondition::Unload(*pf));
    return any;
  }
  const SEPulmonaryShunt* ps = dynamic_cast<const SEPulmonaryShunt*>(&condition);
  if (ps != nullptr)
  {
    any->set_allocated_pulmonaryshunt(PBPatientCondition::Unload(*ps));
    return any;
  }
  const SESepsis* s = dynamic_cast<const SESepsis*>(&condition);
  if (s != nullptr)
  {
    any->set_allocated_sepsis(PBPatientCondition::Unload(*s));
    return any;
  }
  delete any;
  return nullptr;
}
