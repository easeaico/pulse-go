/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
CDM_BIND_DECL2(PatientCondition)
CDM_BIND_DECL2(AcuteRespiratoryDistressSyndrome)
CDM_BIND_DECL2(ChronicAnemia)
CDM_BIND_DECL2(ChronicObstructivePulmonaryDisease)
CDM_BIND_DECL2(ChronicPericardialEffusion)
CDM_BIND_DECL2(ChronicRenalStenosis)
CDM_BIND_DECL2(ChronicVentricularSystolicDysfunction)
CDM_BIND_DECL2(ConsumeMeal)
CDM_BIND_DECL2(Dehydration)
CDM_BIND_DECL2(ImpairedAlveolarExchange)
CDM_BIND_DECL2(Pneumonia)
CDM_BIND_DECL2(PulmonaryFibrosis)
CDM_BIND_DECL2(PulmonaryShunt)
CDM_BIND_DECL2(Sepsis)
CDM_BIND_DECL(AnyPatientConditionData)
class SESubstanceManager;


class CDM_DECL PBPatientCondition
{
public:

  /** Create a new condition based on the binding object, load that data into the new condition, and return said condition */
  static SEPatientCondition* Load(const CDM_BIND::AnyPatientConditionData& any, const SESubstanceManager& subMgr);
  /** Create a new bind object, unload the action, put that in the bind object, and return said bind object */
  static CDM_BIND::AnyPatientConditionData* Unload(const SEPatientCondition& condition);
  static void Serialize(const CDM_BIND::PatientConditionData& src, SEPatientCondition& dst);
  static void Serialize(const SEPatientCondition& src, CDM_BIND::PatientConditionData& dst);

  static void Load(const CDM_BIND::AcuteRespiratoryDistressSyndromeData& src, SEAcuteRespiratoryDistressSyndrome& dst);
  static CDM_BIND::AcuteRespiratoryDistressSyndromeData* Unload(const SEAcuteRespiratoryDistressSyndrome& src);
  static void Serialize(const CDM_BIND::AcuteRespiratoryDistressSyndromeData& src, SEAcuteRespiratoryDistressSyndrome& dst);
  static void Serialize(const SEAcuteRespiratoryDistressSyndrome& src, CDM_BIND::AcuteRespiratoryDistressSyndromeData& dst);
  static void Copy(const SEAcuteRespiratoryDistressSyndrome& src, SEAcuteRespiratoryDistressSyndrome& dst);

  static void Load(const CDM_BIND::ChronicAnemiaData& src, SEChronicAnemia& dst);
  static CDM_BIND::ChronicAnemiaData* Unload(const SEChronicAnemia& src);
  static void Serialize(const CDM_BIND::ChronicAnemiaData& src, SEChronicAnemia& dst);
  static void Serialize(const SEChronicAnemia& src, CDM_BIND::ChronicAnemiaData& dst);
  static void Copy(const SEChronicAnemia& src, SEChronicAnemia& dst);

  static void Load(const CDM_BIND::ChronicObstructivePulmonaryDiseaseData& src, SEChronicObstructivePulmonaryDisease& dst);
  static CDM_BIND::ChronicObstructivePulmonaryDiseaseData* Unload(const SEChronicObstructivePulmonaryDisease& src);
  static void Serialize(const CDM_BIND::ChronicObstructivePulmonaryDiseaseData& src, SEChronicObstructivePulmonaryDisease& dst);
  static void Serialize(const SEChronicObstructivePulmonaryDisease& src, CDM_BIND::ChronicObstructivePulmonaryDiseaseData& dst);
  static void Copy(const SEChronicObstructivePulmonaryDisease& src, SEChronicObstructivePulmonaryDisease& dst);

  static void Load(const CDM_BIND::ChronicPericardialEffusionData& src, SEChronicPericardialEffusion& dst);
  static CDM_BIND::ChronicPericardialEffusionData* Unload(const SEChronicPericardialEffusion& src);
  static void Serialize(const CDM_BIND::ChronicPericardialEffusionData& src, SEChronicPericardialEffusion& dst);
  static void Serialize(const SEChronicPericardialEffusion& src, CDM_BIND::ChronicPericardialEffusionData& dst);
  static void Copy(const SEChronicPericardialEffusion& src, SEChronicPericardialEffusion& dst);

  static void Load(const CDM_BIND::ChronicRenalStenosisData& src, SEChronicRenalStenosis& dst);
  static CDM_BIND::ChronicRenalStenosisData* Unload(const SEChronicRenalStenosis& src);
  static void Serialize(const CDM_BIND::ChronicRenalStenosisData& src, SEChronicRenalStenosis& dst);
  static void Serialize(const SEChronicRenalStenosis& src, CDM_BIND::ChronicRenalStenosisData& dst);
  static void Copy(const SEChronicRenalStenosis& src, SEChronicRenalStenosis& dst);

  static void Load(const CDM_BIND::ChronicVentricularSystolicDysfunctionData& src, SEChronicVentricularSystolicDysfunction& dst);
  static CDM_BIND::ChronicVentricularSystolicDysfunctionData* Unload(const SEChronicVentricularSystolicDysfunction& src);
  static void Serialize(const CDM_BIND::ChronicVentricularSystolicDysfunctionData& src, SEChronicVentricularSystolicDysfunction& dst);
  static void Serialize(const SEChronicVentricularSystolicDysfunction& src, CDM_BIND::ChronicVentricularSystolicDysfunctionData& dst);
  static void Copy(const SEChronicVentricularSystolicDysfunction& src, SEChronicVentricularSystolicDysfunction& dst);

  static void Load(const CDM_BIND::ConsumeMealData& src, SEConsumeMeal& dst);
  static CDM_BIND::ConsumeMealData* Unload(const SEConsumeMeal& src);
  static void Serialize(const CDM_BIND::ConsumeMealData& src, SEConsumeMeal& dst);
  static void Serialize(const SEConsumeMeal& src, CDM_BIND::ConsumeMealData& dst);
  static void Copy(const SEConsumeMeal& src, SEConsumeMeal& dst);

  static void Load(const CDM_BIND::DehydrationData& src, SEDehydration& dst);
  static CDM_BIND::DehydrationData* Unload(const SEDehydration& src);
  static void Serialize(const CDM_BIND::DehydrationData& src, SEDehydration& dst);
  static void Serialize(const SEDehydration& src, CDM_BIND::DehydrationData& dst);
  static void Copy(const SEDehydration& src, SEDehydration& dst);

  static void Load(const CDM_BIND::ImpairedAlveolarExchangeData& src, SEImpairedAlveolarExchange& dst);
  static CDM_BIND::ImpairedAlveolarExchangeData* Unload(const SEImpairedAlveolarExchange& src);
  static void Serialize(const CDM_BIND::ImpairedAlveolarExchangeData& src, SEImpairedAlveolarExchange& dst);
  static void Serialize(const SEImpairedAlveolarExchange& src, CDM_BIND::ImpairedAlveolarExchangeData& dst);
  static void Copy(const SEImpairedAlveolarExchange& src, SEImpairedAlveolarExchange& dst);

  static void Load(const CDM_BIND::PneumoniaData& src, SEPneumonia& dst);
  static CDM_BIND::PneumoniaData* Unload(const SEPneumonia& src);
  static void Serialize(const CDM_BIND::PneumoniaData& src, SEPneumonia& dst);
  static void Serialize(const SEPneumonia& src, CDM_BIND::PneumoniaData& dst);
  static void Copy(const SEPneumonia& src, SEPneumonia& dst);

  static void Load(const CDM_BIND::PulmonaryFibrosisData& src, SEPulmonaryFibrosis& dst);
  static CDM_BIND::PulmonaryFibrosisData* Unload(const SEPulmonaryFibrosis& src);
  static void Serialize(const CDM_BIND::PulmonaryFibrosisData& src, SEPulmonaryFibrosis& dst);
  static void Serialize(const SEPulmonaryFibrosis& src, CDM_BIND::PulmonaryFibrosisData& dst);
  static void Copy(const SEPulmonaryFibrosis& src, SEPulmonaryFibrosis& dst);

  static void Load(const CDM_BIND::PulmonaryShuntData& src, SEPulmonaryShunt& dst);
  static CDM_BIND::PulmonaryShuntData* Unload(const SEPulmonaryShunt& src);
  static void Serialize(const CDM_BIND::PulmonaryShuntData& src, SEPulmonaryShunt& dst);
  static void Serialize(const SEPulmonaryShunt& src, CDM_BIND::PulmonaryShuntData& dst);
  static void Copy(const SEPulmonaryShunt& src, SEPulmonaryShunt& dst);

  static void Load(const CDM_BIND::SepsisData& src, SESepsis& dst);
  static CDM_BIND::SepsisData* Unload(const SESepsis& src);
  static void Serialize(const CDM_BIND::SepsisData& src, SESepsis& dst);
  static void Serialize(const SESepsis& src, CDM_BIND::SepsisData& dst);
  static void Copy(const SESepsis& src, SESepsis& dst);

};