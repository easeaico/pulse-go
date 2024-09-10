/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
PUSH_PROTO_WARNINGS
#include "pulse/cdm/bind/PatientActions.pb.h"
POP_PROTO_WARNINGS
#include "cdm/io/protobuf/PBPatientActions.h"
#include "cdm/io/protobuf/PBPatientNutrition.h"
#include "cdm/io/protobuf/PBActions.h"
#include "cdm/io/protobuf/PBSubstance.h"
#include "cdm/io/protobuf/PBPhysiology.h"
#include "cdm/io/protobuf/PBProperties.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/patient/actions/SEPatientAssessmentRequest.h"
#include "cdm/patient/actions/SEAcuteRespiratoryDistressSyndromeExacerbation.h"
#include "cdm/patient/actions/SEAcuteStress.h"
#include "cdm/patient/actions/SEAirwayObstruction.h"
#include "cdm/patient/actions/SEArrhythmia.h"
#include "cdm/patient/actions/SEAsthmaAttack.h"
#include "cdm/patient/actions/SEBrainInjury.h"
#include "cdm/patient/actions/SEBronchoconstriction.h"
#include "cdm/patient/actions/SECardiovascularMechanicsModification.h"
#include "cdm/patient/actions/SEChestCompression.h"
#include "cdm/patient/actions/SEChestCompressionAutomated.h"
#include "cdm/patient/actions/SEChestCompressionInstantaneous.h"
#include "cdm/patient/actions/SEChestOcclusiveDressing.h"
#include "cdm/patient/actions/SEChronicObstructivePulmonaryDiseaseExacerbation.h"
#include "cdm/patient/actions/SEConsciousRespiration.h"
/**/#include "cdm/patient/actions/SEForcedPause.h"
/**/#include "cdm/patient/actions/SEForcedExhale.h"
/**/#include "cdm/patient/actions/SEForcedInhale.h"
/**/#include "cdm/patient/actions/SEUseInhaler.h"
#include "cdm/patient/actions/SEConsumeNutrients.h"
#include "cdm/patient/actions/SEDyspnea.h"
#include "cdm/patient/actions/SEExercise.h"
#include "cdm/patient/actions/SEHemorrhage.h"
#include "cdm/patient/actions/SEHemothorax.h"
#include "cdm/patient/actions/SEImpairedAlveolarExchangeExacerbation.h"
#include "cdm/patient/actions/SEIntubation.h"
#include "cdm/patient/actions/SEMechanicalVentilation.h"
#include "cdm/patient/actions/SENeedleDecompression.h"
#include "cdm/patient/actions/SEPericardialEffusion.h"
#include "cdm/patient/actions/SEPneumoniaExacerbation.h"
#include "cdm/patient/actions/SEPulmonaryShuntExacerbation.h"
#include "cdm/patient/actions/SERespiratoryFatigue.h"
#include "cdm/patient/actions/SERespiratoryMechanicsConfiguration.h"
#include "cdm/patient/actions/SERespiratoryMechanicsModification.h"
#include "cdm/patient/actions/SESubstanceBolus.h"
#include "cdm/patient/actions/SESubstanceInfusion.h"
#include "cdm/patient/actions/SESubstanceCompoundInfusion.h"
#include "cdm/patient/actions/SESupplementalOxygen.h"
#include "cdm/patient/actions/SETensionPneumothorax.h"
#include "cdm/patient/actions/SETubeThoracostomy.h"
#include "cdm/patient/actions/SEUrinate.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceCompound.h"
#include "cdm/substance/SESubstanceManager.h"

void PBPatientAction::Serialize(const CDM_BIND::PatientActionData& src, SEPatientAction& dst)
{
  PBAction::Serialize(src.action(), dst);
}
void PBPatientAction::Serialize(const SEPatientAction& src, CDM_BIND::PatientActionData& dst)
{
  PBAction::Serialize(src, *dst.mutable_action());
}

void PBPatientAction::Load(const CDM_BIND::AcuteRespiratoryDistressSyndromeExacerbationData& src, SEAcuteRespiratoryDistressSyndromeExacerbation& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::AcuteRespiratoryDistressSyndromeExacerbationData& src, SEAcuteRespiratoryDistressSyndromeExacerbation& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  for (int i = 0; i<src.severity_size(); i++)
  {
    auto& impairment = src.severity()[i];
    PBProperty::Load(impairment.severity(), dst.GetSeverity((eLungCompartment)impairment.compartment()));
  }
}
CDM_BIND::AcuteRespiratoryDistressSyndromeExacerbationData* PBPatientAction::Unload(const SEAcuteRespiratoryDistressSyndromeExacerbation& src)
{
  CDM_BIND::AcuteRespiratoryDistressSyndromeExacerbationData* dst = new CDM_BIND::AcuteRespiratoryDistressSyndromeExacerbationData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEAcuteRespiratoryDistressSyndromeExacerbation& src, CDM_BIND::AcuteRespiratoryDistressSyndromeExacerbationData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  for (auto itr : src.m_Severities)
  {
    auto impairment = dst.mutable_severity()->Add();
    impairment->set_compartment((CDM_BIND::eLungCompartment)itr.first);
    impairment->set_allocated_severity(PBProperty::Unload(*itr.second));
  }
}
void PBPatientAction::Copy(const SEAcuteRespiratoryDistressSyndromeExacerbation& src, SEAcuteRespiratoryDistressSyndromeExacerbation& dst)
{
  dst.Clear();
  CDM_BIND::AcuteRespiratoryDistressSyndromeExacerbationData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::AcuteStressData& src, SEAcuteStress& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::AcuteStressData& src, SEAcuteStress& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
}
CDM_BIND::AcuteStressData* PBPatientAction::Unload(const SEAcuteStress& src)
{
  CDM_BIND::AcuteStressData* dst = new CDM_BIND::AcuteStressData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEAcuteStress& src, CDM_BIND::AcuteStressData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
}
void PBPatientAction::Copy(const SEAcuteStress& src, SEAcuteStress& dst)
{
  dst.Clear();
  CDM_BIND::AcuteStressData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::AirwayObstructionData& src, SEAirwayObstruction& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::AirwayObstructionData& src, SEAirwayObstruction& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
}
CDM_BIND::AirwayObstructionData* PBPatientAction::Unload(const SEAirwayObstruction& src)
{
  CDM_BIND::AirwayObstructionData* dst = new CDM_BIND::AirwayObstructionData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEAirwayObstruction& src, CDM_BIND::AirwayObstructionData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
}
void PBPatientAction::Copy(const SEAirwayObstruction& src, SEAirwayObstruction& dst)
{
  dst.Clear();
  CDM_BIND::AirwayObstructionData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::ArrhythmiaData& src, SEArrhythmia& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::ArrhythmiaData& src, SEArrhythmia& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  dst.SetRhythm((eHeartRhythm)src.rhythm());
}
CDM_BIND::ArrhythmiaData* PBPatientAction::Unload(const SEArrhythmia& src)
{
  CDM_BIND::ArrhythmiaData* dst = new CDM_BIND::ArrhythmiaData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEArrhythmia& src, CDM_BIND::ArrhythmiaData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  dst.set_rhythm((CDM_BIND::eHeartRhythm)src.m_Rhythm);
}
void PBPatientAction::Copy(const SEArrhythmia& src, SEArrhythmia& dst)
{
  dst.Clear();
  CDM_BIND::ArrhythmiaData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::AsthmaAttackData& src, SEAsthmaAttack& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::AsthmaAttackData& src, SEAsthmaAttack& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
}
CDM_BIND::AsthmaAttackData* PBPatientAction::Unload(const SEAsthmaAttack& src)
{
  CDM_BIND::AsthmaAttackData* dst = new CDM_BIND::AsthmaAttackData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEAsthmaAttack& src, CDM_BIND::AsthmaAttackData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
}
void PBPatientAction::Copy(const SEAsthmaAttack& src, SEAsthmaAttack& dst)
{
  dst.Clear();
  CDM_BIND::AsthmaAttackData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::BrainInjuryData& src, SEBrainInjury& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::BrainInjuryData& src, SEBrainInjury& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
  dst.SetType((eBrainInjury_Type)src.type());
}
CDM_BIND::BrainInjuryData* PBPatientAction::Unload(const SEBrainInjury& src)
{
  CDM_BIND::BrainInjuryData* dst = new CDM_BIND::BrainInjuryData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEBrainInjury& src, CDM_BIND::BrainInjuryData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
  dst.set_type((CDM_BIND::BrainInjuryData::eType)src.m_Type);
}
void PBPatientAction::Copy(const SEBrainInjury& src, SEBrainInjury& dst)
{
  dst.Clear();
  CDM_BIND::BrainInjuryData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::BronchoconstrictionData& src, SEBronchoconstriction& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::BronchoconstrictionData& src, SEBronchoconstriction& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
}
CDM_BIND::BronchoconstrictionData* PBPatientAction::Unload(const SEBronchoconstriction& src)
{
  CDM_BIND::BronchoconstrictionData* dst = new CDM_BIND::BronchoconstrictionData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEBronchoconstriction& src, CDM_BIND::BronchoconstrictionData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
}
void PBPatientAction::Copy(const SEBronchoconstriction& src, SEBronchoconstriction& dst)
{
  dst.Clear();
  CDM_BIND::BronchoconstrictionData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::CardiovascularMechanicsModificationData& src, SECardiovascularMechanicsModification& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::CardiovascularMechanicsModificationData& src, SECardiovascularMechanicsModification& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  if (!src.modifiersfile().empty())
    dst.SetModifiersFile(src.modifiersfile());
  else if (src.has_modifiers())
    PBPhysiology::Load(src.modifiers(), dst.GetModifiers());
  dst.SetIncremental(src.incremental());
}
CDM_BIND::CardiovascularMechanicsModificationData* PBPatientAction::Unload(const SECardiovascularMechanicsModification& src)
{
  CDM_BIND::CardiovascularMechanicsModificationData* dst = new CDM_BIND::CardiovascularMechanicsModificationData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SECardiovascularMechanicsModification& src, CDM_BIND::CardiovascularMechanicsModificationData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  if (src.HasModifiersFile())
    dst.set_modifiersfile(src.m_ModifiersFile);
  else if (src.HasModifiers())
    dst.set_allocated_modifiers(PBPhysiology::Unload(*src.m_Modifiers));
  dst.set_incremental(src.GetIncremental());
}
void PBPatientAction::Copy(const SECardiovascularMechanicsModification& src, SECardiovascularMechanicsModification& dst)
{
  dst.Clear();
  CDM_BIND::CardiovascularMechanicsModificationData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::ChestCompressionData& src, SEChestCompression& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::ChestCompressionData& src, SEChestCompression& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  if (src.has_force())
    PBProperty::Load(src.force(), dst.GetForce());
  else if (src.has_depth())
    PBProperty::Load(src.depth(), dst.GetDepth());

  if (src.has_compressionperiod())
    PBProperty::Load(src.compressionperiod(), dst.GetCompressionPeriod());
}
CDM_BIND::ChestCompressionData* PBPatientAction::Unload(const SEChestCompression& src)
{
  CDM_BIND::ChestCompressionData* dst = new CDM_BIND::ChestCompressionData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEChestCompression& src, CDM_BIND::ChestCompressionData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  if (src.HasForce())
    dst.set_allocated_force(PBProperty::Unload(*src.m_Force));
  else if (src.HasDepth())
    dst.set_allocated_depth(PBProperty::Unload(*src.m_Depth));

  if (src.HasCompressionPeriod())
    dst.set_allocated_compressionperiod(PBProperty::Unload(*src.m_CompressionPeriod));
}
void PBPatientAction::Copy(const SEChestCompression& src, SEChestCompression& dst)
{
  dst.Clear();
  CDM_BIND::ChestCompressionData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::ChestCompressionAutomatedData& src, SEChestCompressionAutomated& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::ChestCompressionAutomatedData& src, SEChestCompressionAutomated& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  if (src.has_force())
    PBProperty::Load(src.force(), dst.GetForce());
  else if (src.has_depth())
    PBProperty::Load(src.depth(), dst.GetDepth());

  if (src.has_appliedforcefraction())
    PBProperty::Load(src.appliedforcefraction(), dst.GetAppliedForceFraction());
  if (src.has_compressionfrequency())
    PBProperty::Load(src.compressionfrequency(), dst.GetCompressionFrequency());
}
CDM_BIND::ChestCompressionAutomatedData* PBPatientAction::Unload(const SEChestCompressionAutomated& src)
{
  CDM_BIND::ChestCompressionAutomatedData* dst = new CDM_BIND::ChestCompressionAutomatedData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEChestCompressionAutomated& src, CDM_BIND::ChestCompressionAutomatedData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  if (src.HasForce())
    dst.set_allocated_force(PBProperty::Unload(*src.m_Force));
  else if (src.HasDepth())
    dst.set_allocated_depth(PBProperty::Unload(*src.m_Depth));

  if (src.HasAppliedForceFraction())
    dst.set_allocated_appliedforcefraction(PBProperty::Unload(*src.m_AppliedForceFraction));
  if (src.HasCompressionFrequency())
    dst.set_allocated_compressionfrequency(PBProperty::Unload(*src.m_CompressionFrequency));
}
void PBPatientAction::Copy(const SEChestCompressionAutomated& src, SEChestCompressionAutomated& dst)
{
  dst.Clear();
  CDM_BIND::ChestCompressionAutomatedData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::ChestCompressionInstantaneousData& src, SEChestCompressionInstantaneous& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::ChestCompressionInstantaneousData& src, SEChestCompressionInstantaneous& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  if (src.has_force())
    PBProperty::Load(src.force(), dst.GetForce());
  else if (src.has_depth())
    PBProperty::Load(src.depth(), dst.GetDepth());
}
CDM_BIND::ChestCompressionInstantaneousData* PBPatientAction::Unload(const SEChestCompressionInstantaneous& src)
{
  CDM_BIND::ChestCompressionInstantaneousData* dst = new CDM_BIND::ChestCompressionInstantaneousData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEChestCompressionInstantaneous& src, CDM_BIND::ChestCompressionInstantaneousData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  if (src.HasForce())
    dst.set_allocated_force(PBProperty::Unload(*src.m_Force));
  else if (src.HasDepth())
    dst.set_allocated_depth(PBProperty::Unload(*src.m_Depth));
}
void PBPatientAction::Copy(const SEChestCompressionInstantaneous& src, SEChestCompressionInstantaneous& dst)
{
  dst.Clear();
  CDM_BIND::ChestCompressionInstantaneousData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::ChestOcclusiveDressingData& src, SEChestOcclusiveDressing& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::ChestOcclusiveDressingData& src, SEChestOcclusiveDressing& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  dst.SetSide((eSide)src.side());
  if (src.state() != CDM_BIND::eSwitch::NullSwitch)
    dst.SetState((eSwitch)src.state());
}
CDM_BIND::ChestOcclusiveDressingData* PBPatientAction::Unload(const SEChestOcclusiveDressing& src)
{
  CDM_BIND::ChestOcclusiveDressingData* dst = new CDM_BIND::ChestOcclusiveDressingData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEChestOcclusiveDressing& src, CDM_BIND::ChestOcclusiveDressingData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  dst.set_state((CDM_BIND::eSwitch)src.m_State);
  if (src.HasSide())
    dst.set_side((CDM_BIND::eSide)src.m_Side);
}
void PBPatientAction::Copy(const SEChestOcclusiveDressing& src, SEChestOcclusiveDressing& dst)
{
  dst.Clear();
  CDM_BIND::ChestOcclusiveDressingData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::ChronicObstructivePulmonaryDiseaseExacerbationData& src, SEChronicObstructivePulmonaryDiseaseExacerbation& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::ChronicObstructivePulmonaryDiseaseExacerbationData& src, SEChronicObstructivePulmonaryDiseaseExacerbation& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  if (src.has_bronchitisseverity())
    PBProperty::Load(src.bronchitisseverity(), dst.GetBronchitisSeverity());
  for (int i = 0; i<src.emphysemaseverity_size(); i++)
  {
    auto& impairment = src.emphysemaseverity()[i];
    PBProperty::Load(impairment.severity(), dst.GetEmphysemaSeverity((eLungCompartment)impairment.compartment()));
  }
}
CDM_BIND::ChronicObstructivePulmonaryDiseaseExacerbationData* PBPatientAction::Unload(const SEChronicObstructivePulmonaryDiseaseExacerbation& src)
{
  CDM_BIND::ChronicObstructivePulmonaryDiseaseExacerbationData* dst = new CDM_BIND::ChronicObstructivePulmonaryDiseaseExacerbationData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEChronicObstructivePulmonaryDiseaseExacerbation& src, CDM_BIND::ChronicObstructivePulmonaryDiseaseExacerbationData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  if (src.HasBronchitisSeverity())
    dst.set_allocated_bronchitisseverity(PBProperty::Unload(*src.m_BronchitisSeverity));
  for (auto itr : src.m_EmphysemaSeverities)
  {
    auto impairment = dst.mutable_emphysemaseverity()->Add();
    impairment->set_compartment((CDM_BIND::eLungCompartment)itr.first);
    impairment->set_allocated_severity(PBProperty::Unload(*itr.second));
  }
}
void PBPatientAction::Copy(const SEChronicObstructivePulmonaryDiseaseExacerbation& src, SEChronicObstructivePulmonaryDiseaseExacerbation& dst)
{
  dst.Clear();
  CDM_BIND::ChronicObstructivePulmonaryDiseaseExacerbationData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::ConsciousRespirationData& src, SEConsciousRespiration& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::ConsciousRespirationData& src, SEConsciousRespiration& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  dst.SetStartImmediately(src.startimmediately());
  for (int i = 0; i < src.command().size(); i++)
  {
    const CDM_BIND::AnyConsciousRespirationCommandData& command = src.command()[i];

    switch (command.Command_case())
    {
    case CDM_BIND::AnyConsciousRespirationCommandData::CommandCase::kForcedPause:
      PBPatientAction::Load(command.forcedpause(), dst.AddForcedPause());
      break;
    case CDM_BIND::AnyConsciousRespirationCommandData::CommandCase::kForcedExhale:
      PBPatientAction::Load(command.forcedexhale(), dst.AddForcedExhale());
      break;
    case CDM_BIND::AnyConsciousRespirationCommandData::CommandCase::kForcedInhale:
      PBPatientAction::Load(command.forcedinhale(), dst.AddForcedInhale());
      break;
    case CDM_BIND::AnyConsciousRespirationCommandData::CommandCase::kUseInhaler:
      PBPatientAction::Load(command.useinhaler(), dst.AddUseInhaler());
      break;
    default:
      dst.Warning("Ignoring unknown Conscious Respiration Command : " + std::to_string(command.Command_case()));
      continue;
    }
    dst.m_Commands.back()->SetComment(command.comment());
  }
}
CDM_BIND::ConsciousRespirationData* PBPatientAction::Unload(const SEConsciousRespiration& src)
{
  CDM_BIND::ConsciousRespirationData* dst = new CDM_BIND::ConsciousRespirationData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEConsciousRespiration& src, CDM_BIND::ConsciousRespirationData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  dst.set_startimmediately(src.StartImmediately());
  for (SEConsciousRespirationCommand* cmd : src.m_Commands)
  {
    CDM_BIND::AnyConsciousRespirationCommandData* cmdData = dst.add_command();
    cmdData->set_comment(cmd->GetComment());
    SEForcedPause* bh = dynamic_cast<SEForcedPause*>(cmd);
    if (bh != nullptr)
    {
      cmdData->set_allocated_forcedpause(PBPatientAction::Unload(*bh));
      continue;
    }
    SEForcedExhale* fe = dynamic_cast<SEForcedExhale*>(cmd);
    if (fe != nullptr)
    {
      cmdData->set_allocated_forcedexhale(PBPatientAction::Unload(*fe));
      continue;
    }
    SEForcedInhale* fi = dynamic_cast<SEForcedInhale*>(cmd);
    if (fi != nullptr)
    {
      cmdData->set_allocated_forcedinhale(PBPatientAction::Unload(*fi));
      continue;
    }
    SEUseInhaler* ui = dynamic_cast<SEUseInhaler*>(cmd);
    if (ui != nullptr)
    {
      cmdData->set_allocated_useinhaler(PBPatientAction::Unload(*ui));
      continue;
    }
    src.Warning("The unmapped respiration command in SEConsciousRespiration::Serialize");
  }
}
void PBPatientAction::Copy(const SEConsciousRespiration& src, SEConsciousRespiration& dst)
{
  dst.Clear();
  CDM_BIND::ConsciousRespirationData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::ConsumeNutrientsData& src, SEConsumeNutrients& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::ConsumeNutrientsData& src, SEConsumeNutrients& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  if(!src.nutritionfile().empty())
    dst.SetNutritionFile(src.nutritionfile());
  else if (src.has_nutrition())
    PBPatientNutrition::Load(src.nutrition(), dst.GetNutrition());
}
CDM_BIND::ConsumeNutrientsData* PBPatientAction::Unload(const SEConsumeNutrients& src)
{
  CDM_BIND::ConsumeNutrientsData* dst = new CDM_BIND::ConsumeNutrientsData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEConsumeNutrients& src, CDM_BIND::ConsumeNutrientsData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  if (src.HasNutritionFile())
    dst.set_nutritionfile(src.m_NutritionFile);
  else if (src.HasNutrition())
    dst.set_allocated_nutrition(PBPatientNutrition::Unload(*src.m_Nutrition));
}
void PBPatientAction::Copy(const SEConsumeNutrients& src, SEConsumeNutrients& dst)
{
  dst.Clear();
  CDM_BIND::ConsumeNutrientsData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::DyspneaData& src, SEDyspnea& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::DyspneaData& src, SEDyspnea& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  if (src.has_respirationrateseverity())
    PBProperty::Load(src.respirationrateseverity(), dst.GetRespirationRateSeverity());
  if (src.has_tidalvolumeseverity())
    PBProperty::Load(src.tidalvolumeseverity(), dst.GetTidalVolumeSeverity());
}
CDM_BIND::DyspneaData* PBPatientAction::Unload(const SEDyspnea& src)
{
  CDM_BIND::DyspneaData* dst = new CDM_BIND::DyspneaData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEDyspnea& src, CDM_BIND::DyspneaData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  if (src.HasRespirationRateSeverity())
    dst.set_allocated_respirationrateseverity(PBProperty::Unload(*src.m_RespirationRateSeverity));
  if (src.HasTidalVolumeSeverity())
    dst.set_allocated_tidalvolumeseverity(PBProperty::Unload(*src.m_TidalVolumeSeverity));
}
void PBPatientAction::Copy(const SEDyspnea& src, SEDyspnea& dst)
{
  dst.Clear();
  CDM_BIND::DyspneaData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::ExerciseData& src, SEExercise& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::ExerciseData& src, SEExercise& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  if (src.has_intensity())
    PBProperty::Load(src.intensity(), dst.GetIntensity());
}
CDM_BIND::ExerciseData* PBPatientAction::Unload(const SEExercise& src)
{
  CDM_BIND::ExerciseData* dst = new CDM_BIND::ExerciseData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEExercise& src, CDM_BIND::ExerciseData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  if (src.HasIntensity())
    dst.set_allocated_intensity(PBProperty::Unload(*src.m_Intensity));
}
void PBPatientAction::Copy(const SEExercise& src, SEExercise& dst)
{
  dst.Clear();
  CDM_BIND::ExerciseData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::ForcedExhaleData& src, SEForcedExhale& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::ForcedExhaleData& src, SEForcedExhale& dst)
{
  if (src.has_expiratoryreservevolumefraction())
    PBProperty::Load(src.expiratoryreservevolumefraction(), dst.GetExpiratoryReserveVolumeFraction());
  if (src.has_exhaleperiod())
    PBProperty::Load(src.exhaleperiod(), dst.GetExhalePeriod());
  if (src.has_holdperiod())
    PBProperty::Load(src.holdperiod(), dst.GetHoldPeriod());
  if (src.has_releaseperiod())
    PBProperty::Load(src.releaseperiod(), dst.GetReleasePeriod());
}
CDM_BIND::ForcedExhaleData* PBPatientAction::Unload(const SEForcedExhale& src)
{
  CDM_BIND::ForcedExhaleData* dst = new CDM_BIND::ForcedExhaleData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEForcedExhale& src, CDM_BIND::ForcedExhaleData& dst)
{
  if (src.HasExpiratoryReserveVolumeFraction())
    dst.set_allocated_expiratoryreservevolumefraction(PBProperty::Unload(*src.m_ExpiratoryReserveVolumeFraction));
  if (src.HasExhalePeriod())
    dst.set_allocated_exhaleperiod(PBProperty::Unload(*src.m_ExhalePeriod));
  if (src.HasHoldPeriod())
    dst.set_allocated_holdperiod(PBProperty::Unload(*src.m_HoldPeriod));
  if (src.HasReleasePeriod())
    dst.set_allocated_releaseperiod(PBProperty::Unload(*src.m_ReleasePeriod));
}
void PBPatientAction::Copy(const SEForcedExhale& src, SEForcedExhale& dst)
{
  dst.Clear();
  CDM_BIND::ForcedExhaleData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::ForcedInhaleData& src, SEForcedInhale& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::ForcedInhaleData& src, SEForcedInhale& dst)
{
  if (src.has_inspiratorycapacityfraction())
    PBProperty::Load(src.inspiratorycapacityfraction(), dst.GetInspiratoryCapacityFraction());
  if (src.has_inhaleperiod())
    PBProperty::Load(src.inhaleperiod(), dst.GetInhalePeriod());
  if (src.has_holdperiod())
    PBProperty::Load(src.holdperiod(), dst.GetHoldPeriod());
  if (src.has_releaseperiod())
    PBProperty::Load(src.releaseperiod(), dst.GetReleasePeriod());
}
CDM_BIND::ForcedInhaleData* PBPatientAction::Unload(const SEForcedInhale& src)
{
  CDM_BIND::ForcedInhaleData* dst = new CDM_BIND::ForcedInhaleData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEForcedInhale& src, CDM_BIND::ForcedInhaleData& dst)
{
  if (src.HasInspiratoryCapacityFraction())
    dst.set_allocated_inspiratorycapacityfraction(PBProperty::Unload(*src.m_InspiratoryCapacityFraction));
  if (src.HasInhalePeriod())
    dst.set_allocated_inhaleperiod(PBProperty::Unload(*src.m_InhalePeriod));
  if (src.HasHoldPeriod())
    dst.set_allocated_holdperiod(PBProperty::Unload(*src.m_HoldPeriod));
  if (src.HasReleasePeriod())
    dst.set_allocated_releaseperiod(PBProperty::Unload(*src.m_ReleasePeriod));
}
void PBPatientAction::Copy(const SEForcedInhale& src, SEForcedInhale& dst)
{
  dst.Clear();
  CDM_BIND::ForcedInhaleData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::ForcedPauseData& src, SEForcedPause& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::ForcedPauseData& src, SEForcedPause& dst)
{
  if (src.has_period())
    PBProperty::Load(src.period(), dst.GetPeriod());
}
CDM_BIND::ForcedPauseData* PBPatientAction::Unload(const SEForcedPause& src)
{
  CDM_BIND::ForcedPauseData* dst = new CDM_BIND::ForcedPauseData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEForcedPause& src, CDM_BIND::ForcedPauseData& dst)
{
  if (src.HasPeriod())
    dst.set_allocated_period(PBProperty::Unload(*src.m_Period));
}
void PBPatientAction::Copy(const SEForcedPause& src, SEForcedPause& dst)
{
  dst.Clear();
  CDM_BIND::ForcedPauseData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::HemorrhageData& src, SEHemorrhage& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::HemorrhageData& src, SEHemorrhage& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  dst.SetCompartment((eHemorrhage_Compartment)src.compartment());
  dst.m_Type = (eHemorrhage_Type)src.type();
  if (src.has_flowrate())
    PBProperty::Load(src.flowrate(), dst.GetFlowRate());
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
  if (src.has_totalbloodlost())
    PBProperty::Load(src.totalbloodlost(), dst.GetTotalBloodLost());
}
CDM_BIND::HemorrhageData* PBPatientAction::Unload(const SEHemorrhage& src)
{
  CDM_BIND::HemorrhageData* dst = new CDM_BIND::HemorrhageData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEHemorrhage& src, CDM_BIND::HemorrhageData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  dst.set_compartment((CDM_BIND::HemorrhageData::eCompartment)src.m_Compartment);
  dst.set_type((CDM_BIND::HemorrhageData::eType)src.m_Type);
  if (src.HasFlowRate())
    dst.set_allocated_flowrate(PBProperty::Unload(*src.m_FlowRate));
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
  if (src.HasTotalBloodLost())
    dst.set_allocated_totalbloodlost(PBProperty::Unload(*src.m_TotalBloodLost));
}
void PBPatientAction::Copy(const SEHemorrhage& src, SEHemorrhage& dst)
{
  dst.Clear();
  CDM_BIND::HemorrhageData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::HemothoraxData& src, SEHemothorax& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::HemothoraxData& src, SEHemothorax& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  dst.SetSide((eSide)src.side());
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
  if (src.has_flowrate())
    PBProperty::Load(src.flowrate(), dst.GetFlowRate());
  if (src.has_totalbloodvolume())
    PBProperty::Load(src.totalbloodvolume(), dst.GetTotalBloodVolume());
}
CDM_BIND::HemothoraxData* PBPatientAction::Unload(const SEHemothorax& src)
{
  CDM_BIND::HemothoraxData* dst = new CDM_BIND::HemothoraxData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEHemothorax& src, CDM_BIND::HemothoraxData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  if (src.HasSide())
    dst.set_side((CDM_BIND::eSide)src.m_Side);
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
  if (src.HasFlowRate())
    dst.set_allocated_flowrate(PBProperty::Unload(*src.m_FlowRate));
  if (src.HasTotalBloodVolume())
    dst.set_allocated_totalbloodvolume(PBProperty::Unload(*src.m_TotalBloodVolume));
}
void PBPatientAction::Copy(const SEHemothorax& src, SEHemothorax& dst)
{
  dst.Clear();
  CDM_BIND::HemothoraxData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::ImpairedAlveolarExchangeExacerbationData& src, SEImpairedAlveolarExchangeExacerbation& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::ImpairedAlveolarExchangeExacerbationData& src, SEImpairedAlveolarExchangeExacerbation& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
  else if (src.has_impairedfraction())
    PBProperty::Load(src.impairedfraction(), dst.GetImpairedFraction());
  else if (src.has_impairedsurfacearea())
    PBProperty::Load(src.impairedsurfacearea(), dst.GetImpairedSurfaceArea());

}
CDM_BIND::ImpairedAlveolarExchangeExacerbationData* PBPatientAction::Unload(const SEImpairedAlveolarExchangeExacerbation& src)
{
  CDM_BIND::ImpairedAlveolarExchangeExacerbationData* dst = new CDM_BIND::ImpairedAlveolarExchangeExacerbationData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEImpairedAlveolarExchangeExacerbation& src, CDM_BIND::ImpairedAlveolarExchangeExacerbationData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
  else if (src.HasImpairedFraction())
    dst.set_allocated_impairedfraction(PBProperty::Unload(*src.m_ImpairedFraction));
  else if (src.HasImpairedSurfaceArea())
    dst.set_allocated_impairedsurfacearea(PBProperty::Unload(*src.m_ImpairedSurfaceArea));
}
void PBPatientAction::Copy(const SEImpairedAlveolarExchangeExacerbation& src, SEImpairedAlveolarExchangeExacerbation& dst)
{
  dst.Clear();
  CDM_BIND::ImpairedAlveolarExchangeExacerbationData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::IntubationData& src, SEIntubation& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::IntubationData& src, SEIntubation& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  dst.SetType((eIntubation_Type)src.type());
  if (src.has_airwayresistance())
    PBProperty::Load(src.airwayresistance(), dst.GetAirwayResistance());
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
}
CDM_BIND::IntubationData* PBPatientAction::Unload(const SEIntubation& src)
{
  CDM_BIND::IntubationData* dst = new CDM_BIND::IntubationData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEIntubation& src, CDM_BIND::IntubationData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  dst.set_type((CDM_BIND::IntubationData::eType)src.m_Type);
  if (src.HasAirwayResistance())
    dst.set_allocated_airwayresistance(PBProperty::Unload(*src.m_AirwayResistance));
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
}
void PBPatientAction::Copy(const SEIntubation& src, SEIntubation& dst)
{
  dst.Clear();
  CDM_BIND::IntubationData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::MechanicalVentilationData& src, SEMechanicalVentilation& dst, const SESubstanceManager& subMgr)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst, subMgr);
}
void PBPatientAction::Serialize(const CDM_BIND::MechanicalVentilationData& src, SEMechanicalVentilation& dst, const SESubstanceManager& subMgr)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  if (src.state() != CDM_BIND::eSwitch::NullSwitch)
    dst.SetState((eSwitch)src.state());
  if (src.has_flow())
    PBProperty::Load(src.flow(), dst.GetFlow());
  if (src.has_pressure())
    PBProperty::Load(src.pressure(), dst.GetPressure());

  dst.m_GasFractions.clear();
  dst.m_cGasFractions.clear();
  const SESubstance* sub;
  for (int i = 0; i < src.gasfraction_size(); i++)
  {
    const CDM_BIND::SubstanceFractionData& sfData = src.gasfraction()[i];
    sub = subMgr.GetSubstance(sfData.name());
    if (sub == nullptr)
    {
      dst.Error("MechanicalVentilation substance not found : " + sfData.name());
      continue;
    }
    if (sub->GetState() != eSubstance_State::Gas)
    {
      dst.Error("MechanicalVentilation substance not gas : " + sfData.name());
      continue;
    }
    PBSubstance::Load(sfData, dst.GetGasFraction(*sub));
  }

  for (int i = 0; i < src.aerosol_size(); i++)
  {
    const CDM_BIND::SubstanceConcentrationData& scData = src.aerosol()[i];
    sub = subMgr.GetSubstance(scData.name());
    if (sub == nullptr)
    {
      dst.Error("Ignoring an environmental conditions aerosol that was not found : " + scData.name());
      continue;
    }
    if (sub->GetState() != eSubstance_State::Liquid && sub->GetState() != eSubstance_State::Solid)
    {
      dst.Error("Ignoring an environmental conditions aerosol that is not a liquid or solid : " + scData.name());
      continue;
    }
    PBSubstance::Load(scData, dst.GetAerosol(*sub));
  }
}
CDM_BIND::MechanicalVentilationData* PBPatientAction::Unload(const SEMechanicalVentilation& src)
{
  CDM_BIND::MechanicalVentilationData* dst = new CDM_BIND::MechanicalVentilationData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEMechanicalVentilation& src, CDM_BIND::MechanicalVentilationData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  dst.set_state((CDM_BIND::eSwitch)src.m_State);
  if (src.HasFlow())
    dst.set_allocated_flow(PBProperty::Unload(*src.m_Flow));
  if (src.HasPressure())
    dst.set_allocated_pressure(PBProperty::Unload(*src.m_Pressure));
  for (SESubstanceFraction *sf : src.m_GasFractions)
    dst.mutable_gasfraction()->AddAllocated(PBSubstance::Unload(*sf));
  for (SESubstanceConcentration *sc : src.m_Aerosols)
    dst.mutable_aerosol()->AddAllocated(PBSubstance::Unload(*sc));
}
void PBPatientAction::Copy(const SEMechanicalVentilation& src, SEMechanicalVentilation& dst, const SESubstanceManager& subMgr)
{
  dst.Clear();
  CDM_BIND::MechanicalVentilationData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst, subMgr);
}

void PBPatientAction::Load(const CDM_BIND::NeedleDecompressionData& src, SENeedleDecompression& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::NeedleDecompressionData& src, SENeedleDecompression& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  dst.SetSide((eSide)src.side());
  if (src.state() != CDM_BIND::eSwitch::NullSwitch)
    dst.SetState((eSwitch)src.state());
}
CDM_BIND::NeedleDecompressionData* PBPatientAction::Unload(const SENeedleDecompression& src)
{
  CDM_BIND::NeedleDecompressionData* dst = new CDM_BIND::NeedleDecompressionData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SENeedleDecompression& src, CDM_BIND::NeedleDecompressionData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  if (src.HasSide())
    dst.set_side((CDM_BIND::eSide)src.m_Side);
  dst.set_state((CDM_BIND::eSwitch)src.m_State);
}
void PBPatientAction::Copy(const SENeedleDecompression& src, SENeedleDecompression& dst)
{
  dst.Clear();
  CDM_BIND::NeedleDecompressionData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::PatientAssessmentRequestData& src, SEPatientAssessmentRequest& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::PatientAssessmentRequestData& src, SEPatientAssessmentRequest& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  dst.SetType((ePatientAssessment_Type)src.type());
}
CDM_BIND::PatientAssessmentRequestData* PBPatientAction::Unload(const SEPatientAssessmentRequest& src)
{
  CDM_BIND::PatientAssessmentRequestData* dst = new CDM_BIND::PatientAssessmentRequestData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEPatientAssessmentRequest& src, CDM_BIND::PatientAssessmentRequestData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  dst.set_type((CDM_BIND::ePatientAssessmentType)src.m_Type);
}
void PBPatientAction::Copy(const SEPatientAssessmentRequest& src, SEPatientAssessmentRequest& dst)
{
  dst.Clear();
  CDM_BIND::PatientAssessmentRequestData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::PericardialEffusionData& src, SEPericardialEffusion& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::PericardialEffusionData& src, SEPericardialEffusion& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  if (src.has_effusionrate())
    PBProperty::Load(src.effusionrate(), dst.GetEffusionRate());
}
CDM_BIND::PericardialEffusionData* PBPatientAction::Unload(const SEPericardialEffusion& src)
{
  CDM_BIND::PericardialEffusionData* dst = new CDM_BIND::PericardialEffusionData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEPericardialEffusion& src, CDM_BIND::PericardialEffusionData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  if (src.HasEffusionRate())
    dst.set_allocated_effusionrate(PBProperty::Unload(*src.m_EffusionRate));
}
void PBPatientAction::Copy(const SEPericardialEffusion& src, SEPericardialEffusion& dst)
{
  dst.Clear();
  CDM_BIND::PericardialEffusionData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::PneumoniaExacerbationData& src, SEPneumoniaExacerbation& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::PneumoniaExacerbationData& src, SEPneumoniaExacerbation& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  for (int i = 0; i<src.severity_size(); i++)
  {
    auto& impairment = src.severity()[i];
    PBProperty::Load(impairment.severity(), dst.GetSeverity((eLungCompartment)impairment.compartment()));
  }
}
CDM_BIND::PneumoniaExacerbationData* PBPatientAction::Unload(const SEPneumoniaExacerbation& src)
{
  CDM_BIND::PneumoniaExacerbationData* dst = new CDM_BIND::PneumoniaExacerbationData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEPneumoniaExacerbation& src, CDM_BIND::PneumoniaExacerbationData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  for (auto itr : src.m_Severities)
  {
    auto impairment = dst.mutable_severity()->Add();
    impairment->set_compartment((CDM_BIND::eLungCompartment)itr.first);
    impairment->set_allocated_severity(PBProperty::Unload(*itr.second));
  }
}
void PBPatientAction::Copy(const SEPneumoniaExacerbation& src, SEPneumoniaExacerbation& dst)
{
  dst.Clear();
  CDM_BIND::PneumoniaExacerbationData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::PulmonaryShuntExacerbationData& src, SEPulmonaryShuntExacerbation& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::PulmonaryShuntExacerbationData& src, SEPulmonaryShuntExacerbation& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
}
CDM_BIND::PulmonaryShuntExacerbationData* PBPatientAction::Unload(const SEPulmonaryShuntExacerbation& src)
{
  CDM_BIND::PulmonaryShuntExacerbationData* dst = new CDM_BIND::PulmonaryShuntExacerbationData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEPulmonaryShuntExacerbation& src, CDM_BIND::PulmonaryShuntExacerbationData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
}
void PBPatientAction::Copy(const SEPulmonaryShuntExacerbation& src, SEPulmonaryShuntExacerbation& dst)
{
  dst.Clear();
  CDM_BIND::PulmonaryShuntExacerbationData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::RespiratoryFatigueData& src, SERespiratoryFatigue& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::RespiratoryFatigueData& src, SERespiratoryFatigue& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
}
CDM_BIND::RespiratoryFatigueData* PBPatientAction::Unload(const SERespiratoryFatigue& src)
{
  CDM_BIND::RespiratoryFatigueData* dst = new CDM_BIND::RespiratoryFatigueData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SERespiratoryFatigue& src, CDM_BIND::RespiratoryFatigueData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
}
void PBPatientAction::Copy(const SERespiratoryFatigue& src, SERespiratoryFatigue& dst)
{
  dst.Clear();
  CDM_BIND::RespiratoryFatigueData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::RespiratoryMechanicsConfigurationData& src, SERespiratoryMechanicsConfiguration& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::RespiratoryMechanicsConfigurationData& src, SERespiratoryMechanicsConfiguration& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  if (!src.settingsfile().empty())
    dst.SetSettingsFile(src.settingsfile());
  else if (src.has_settings())
    PBPhysiology::Load(src.settings(), dst.GetSettings());
  dst.SetAppliedRespiratoryCycle((eAppliedRespiratoryCycle)src.appliedcycle());
  dst.SetMergeType((eMergeType)src.mergetype());
}
CDM_BIND::RespiratoryMechanicsConfigurationData* PBPatientAction::Unload(const SERespiratoryMechanicsConfiguration& src)
{
  CDM_BIND::RespiratoryMechanicsConfigurationData* dst = new CDM_BIND::RespiratoryMechanicsConfigurationData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SERespiratoryMechanicsConfiguration& src, CDM_BIND::RespiratoryMechanicsConfigurationData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  if (src.HasSettingsFile())
    dst.set_settingsfile(src.m_SettingsFile);
  else if (src.HasSettings())
    dst.set_allocated_settings(PBPhysiology::Unload(*src.m_Settings));
  dst.set_appliedcycle((CDM_BIND::eAppliedRespiratoryCycle)src.m_AppliedRespiratoryCycle);
  dst.set_mergetype((CDM_BIND::eMergeType)src.m_MergeType);
}
void PBPatientAction::Copy(const SERespiratoryMechanicsConfiguration& src, SERespiratoryMechanicsConfiguration& dst)
{
  dst.Clear();
  CDM_BIND::RespiratoryMechanicsConfigurationData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::RespiratoryMechanicsModificationData& src, SERespiratoryMechanicsModification& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::RespiratoryMechanicsModificationData& src, SERespiratoryMechanicsModification& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  if (!src.modifiersfile().empty())
    dst.SetModifiersFile(src.modifiersfile());
  else if (src.has_modifiers())
    PBPhysiology::Load(src.modifiers(), dst.GetModifiers());
  dst.SetIncremental(src.incremental());
}
CDM_BIND::RespiratoryMechanicsModificationData* PBPatientAction::Unload(const SERespiratoryMechanicsModification& src)
{
  CDM_BIND::RespiratoryMechanicsModificationData* dst = new CDM_BIND::RespiratoryMechanicsModificationData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SERespiratoryMechanicsModification& src, CDM_BIND::RespiratoryMechanicsModificationData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  if (src.HasModifiersFile())
    dst.set_modifiersfile(src.m_ModifiersFile);
  else if (src.HasModifiers())
    dst.set_allocated_modifiers(PBPhysiology::Unload(*src.m_Modifiers));
  dst.set_incremental(src.GetIncremental());
}
void PBPatientAction::Copy(const SERespiratoryMechanicsModification& src, SERespiratoryMechanicsModification& dst)
{
  dst.Clear();
  CDM_BIND::RespiratoryMechanicsModificationData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::SubstanceBolusData& src, SESubstanceBolus& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::SubstanceBolusData& src, SESubstanceBolus& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  dst.SetAdminRoute((eSubstanceAdministration_Route)src.administrationroute());
  if (src.has_administrationduration())
    PBProperty::Load(src.administrationduration(), dst.GetAdminDuration());
  if (src.has_dose())
    PBProperty::Load(src.dose(), dst.GetDose());
  if (src.has_concentration())
    PBProperty::Load(src.concentration(), dst.GetConcentration());
  if (src.has_totalinfuseddose())
    PBProperty::Load(src.totalinfuseddose(), dst.GetTotalInfusedDose());
}
CDM_BIND::SubstanceBolusData* PBPatientAction::Unload(const SESubstanceBolus& src)
{
  CDM_BIND::SubstanceBolusData* dst = new CDM_BIND::SubstanceBolusData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SESubstanceBolus& src, CDM_BIND::SubstanceBolusData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  dst.set_substance(src.m_Substance.GetName());
  dst.set_administrationroute((CDM_BIND::SubstanceBolusData::eRoute)src.m_AdminRoute);
  if (src.HasAdminDuration())
    dst.set_allocated_administrationduration(PBProperty::Unload(*src.m_AdminDuration));
  if (src.HasDose())
    dst.set_allocated_dose(PBProperty::Unload(*src.m_Dose));
  if (src.HasConcentration())
    dst.set_allocated_concentration(PBProperty::Unload(*src.m_Concentration));
  if(src.HasTotalInfusedDose())
    dst.set_allocated_totalinfuseddose(PBProperty::Unload(*src.m_TotalInfusedDose));
}
void PBPatientAction::Copy(const SESubstanceBolus& src, SESubstanceBolus& dst)
{
  dst.Clear();
  CDM_BIND::SubstanceBolusData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::SubstanceCompoundInfusionData& src, SESubstanceCompoundInfusion& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::SubstanceCompoundInfusionData& src, SESubstanceCompoundInfusion& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  if (src.has_rate())
    PBProperty::Load(src.rate(), dst.GetRate());
  if (src.has_bagvolume())
    PBProperty::Load(src.bagvolume(), dst.GetBagVolume());
}
CDM_BIND::SubstanceCompoundInfusionData* PBPatientAction::Unload(const SESubstanceCompoundInfusion& src)
{
  CDM_BIND::SubstanceCompoundInfusionData* dst = new CDM_BIND::SubstanceCompoundInfusionData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SESubstanceCompoundInfusion& src, CDM_BIND::SubstanceCompoundInfusionData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  dst.set_substancecompound(src.m_Compound.GetName());
  if (src.HasRate())
    dst.set_allocated_rate(PBProperty::Unload(*src.m_Rate));
  if (src.HasBagVolume())
    dst.set_allocated_bagvolume(PBProperty::Unload(*src.m_BagVolume));
}
void PBPatientAction::Copy(const SESubstanceCompoundInfusion& src, SESubstanceCompoundInfusion& dst)
{
  dst.Clear();
  CDM_BIND::SubstanceCompoundInfusionData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::SubstanceInfusionData& src, SESubstanceInfusion& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::SubstanceInfusionData& src, SESubstanceInfusion& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  if (src.has_rate())
    PBProperty::Load(src.rate(), dst.GetRate());
  if (src.has_concentration())
    PBProperty::Load(src.concentration(), dst.GetConcentration());
  if (src.has_volume())
    PBProperty::Load(src.volume(), dst.GetVolume());
}
CDM_BIND::SubstanceInfusionData* PBPatientAction::Unload(const SESubstanceInfusion& src)
{
  CDM_BIND::SubstanceInfusionData* dst = new CDM_BIND::SubstanceInfusionData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SESubstanceInfusion& src, CDM_BIND::SubstanceInfusionData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  dst.set_substance(src.m_Substance.GetName());
  if (src.HasRate())
    dst.set_allocated_rate(PBProperty::Unload(*src.m_Rate));
  if (src.HasConcentration())
    dst.set_allocated_concentration(PBProperty::Unload(*src.m_Concentration));
  if (src.HasVolume())
    dst.set_allocated_volume(PBProperty::Unload(*src.m_Volume));
}
void PBPatientAction::Copy(const SESubstanceInfusion& src, SESubstanceInfusion& dst)
{
  dst.Clear();
  CDM_BIND::SubstanceInfusionData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::SupplementalOxygenData& src, SESupplementalOxygen& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::SupplementalOxygenData& src, SESupplementalOxygen& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  dst.SetDevice((eSupplementalOxygen_Device)src.device());
  if (src.has_flow())
    PBProperty::Load(src.flow(), dst.GetFlow());
  if (src.has_volume())
    PBProperty::Load(src.volume(), dst.GetVolume());
}
CDM_BIND::SupplementalOxygenData* PBPatientAction::Unload(const SESupplementalOxygen& src)
{
  CDM_BIND::SupplementalOxygenData* dst = new CDM_BIND::SupplementalOxygenData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SESupplementalOxygen& src, CDM_BIND::SupplementalOxygenData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  dst.set_device((CDM_BIND::SupplementalOxygenData::eDevice)src.m_Device);
  if (src.HasFlow())
    dst.set_allocated_flow(PBProperty::Unload(*src.m_Flow));
  if (src.HasVolume())
    dst.set_allocated_volume(PBProperty::Unload(*src.m_Volume));
}
void PBPatientAction::Copy(const SESupplementalOxygen& src, SESupplementalOxygen& dst)
{
  dst.Clear();
  CDM_BIND::SupplementalOxygenData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::TensionPneumothoraxData& src, SETensionPneumothorax& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::TensionPneumothoraxData& src, SETensionPneumothorax& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  dst.SetType((eGate)src.type());
  dst.SetSide((eSide)src.side());
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
}
CDM_BIND::TensionPneumothoraxData* PBPatientAction::Unload(const SETensionPneumothorax& src)
{
  CDM_BIND::TensionPneumothoraxData* dst = new CDM_BIND::TensionPneumothoraxData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SETensionPneumothorax& src, CDM_BIND::TensionPneumothoraxData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  if (src.HasType())
    dst.set_type((CDM_BIND::eGate)src.m_Type);
  if (src.HasSide())
    dst.set_side((CDM_BIND::eSide)src.m_Side);
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
}
void PBPatientAction::Copy(const SETensionPneumothorax& src, SETensionPneumothorax& dst)
{
  dst.Clear();
  CDM_BIND::TensionPneumothoraxData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::TubeThoracostomyData& src, SETubeThoracostomy& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::TubeThoracostomyData& src, SETubeThoracostomy& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
  dst.SetSide((eSide)src.side());
  if (src.has_flowrate())
    PBProperty::Load(src.flowrate(), dst.GetFlowRate());
}
CDM_BIND::TubeThoracostomyData* PBPatientAction::Unload(const SETubeThoracostomy& src)
{
  CDM_BIND::TubeThoracostomyData* dst = new CDM_BIND::TubeThoracostomyData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SETubeThoracostomy& src, CDM_BIND::TubeThoracostomyData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
  if (src.HasSide())
    dst.set_side((CDM_BIND::eSide)src.m_Side);
  if (src.HasFlowRate())
    dst.set_allocated_flowrate(PBProperty::Unload(*src.m_FlowRate));
}
void PBPatientAction::Copy(const SETubeThoracostomy& src, SETubeThoracostomy& dst)
{
  dst.Clear();
  CDM_BIND::TubeThoracostomyData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::UrinateData& src, SEUrinate& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::UrinateData& src, SEUrinate& dst)
{
  PBPatientAction::Serialize(src.patientaction(), dst);
}
CDM_BIND::UrinateData* PBPatientAction::Unload(const SEUrinate& src)
{
  CDM_BIND::UrinateData* dst = new CDM_BIND::UrinateData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEUrinate& src, CDM_BIND::UrinateData& dst)
{
  PBPatientAction::Serialize(src, *dst.mutable_patientaction());
}
void PBPatientAction::Copy(const SEUrinate& src, SEUrinate& dst)
{
  dst.Clear();
  CDM_BIND::UrinateData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}

void PBPatientAction::Load(const CDM_BIND::UseInhalerData& src, SEUseInhaler& dst)
{
  dst.Clear();
  PBPatientAction::Serialize(src, dst);
}
void PBPatientAction::Serialize(const CDM_BIND::UseInhalerData& /*src*/, SEUseInhaler& /*dst*/)
{

}
CDM_BIND::UseInhalerData* PBPatientAction::Unload(const SEUseInhaler& src)
{
  CDM_BIND::UseInhalerData* dst = new CDM_BIND::UseInhalerData();
  PBPatientAction::Serialize(src, *dst);
  return dst;
}
void PBPatientAction::Serialize(const SEUseInhaler& /*src*/, CDM_BIND::UseInhalerData& /*dst*/)
{

}
void PBPatientAction::Copy(const SEUseInhaler& src, SEUseInhaler& dst)
{
  dst.Clear();
  CDM_BIND::UseInhalerData data;
  PBPatientAction::Serialize(src, data);
  PBPatientAction::Serialize(data, dst);
}


SEPatientAction* PBPatientAction::Load(const CDM_BIND::AnyPatientActionData& any, const SESubstanceManager& subMgr)
{
  switch (any.Action_case())
  {
  case CDM_BIND::AnyPatientActionData::ActionCase::kAssessment:
  {
    SEPatientAssessmentRequest* a = new SEPatientAssessmentRequest();
    PBPatientAction::Load(any.assessment(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kAcuteRespiratoryDistressSyndromeExacerbation:
  {
    SEAcuteRespiratoryDistressSyndromeExacerbation* a = new SEAcuteRespiratoryDistressSyndromeExacerbation();
    PBPatientAction::Load(any.acuterespiratorydistresssyndromeexacerbation(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kAcuteStress:
  {
    SEAcuteStress* a = new SEAcuteStress();
    PBPatientAction::Load(any.acutestress(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kAirwayObstruction:
  {
    SEAirwayObstruction* a = new SEAirwayObstruction();
    PBPatientAction::Load(any.airwayobstruction(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kArrhythmia:
  {
    SEArrhythmia* a = new SEArrhythmia();
    PBPatientAction::Load(any.arrhythmia(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kAsthmaAttack:
  {
    SEAsthmaAttack* a = new SEAsthmaAttack();
    PBPatientAction::Load(any.asthmaattack(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kBrainInjury:
  {
    SEBrainInjury* a = new SEBrainInjury();
    PBPatientAction::Load(any.braininjury(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kBronchoconstriction:
  {
    SEBronchoconstriction* a = new SEBronchoconstriction();
    PBPatientAction::Load(any.bronchoconstriction(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kCardiovascularMechanicsModification:
  {
    SECardiovascularMechanicsModification* a = new SECardiovascularMechanicsModification();
    PBPatientAction::Load(any.cardiovascularmechanicsmodification(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kChestCompression:
  {
    SEChestCompression* a = new SEChestCompression();
    PBPatientAction::Load(any.chestcompression(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kChestCompressionInstantaneous:
  {
    SEChestCompressionInstantaneous* a = new SEChestCompressionInstantaneous();
    PBPatientAction::Load(any.chestcompressioninstantaneous(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kChestCompressionAutomated:
  {
    SEChestCompressionAutomated* a = new SEChestCompressionAutomated();
    PBPatientAction::Load(any.chestcompressionautomated(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kChestOcclusiveDressing:
  {
    SEChestOcclusiveDressing* a = new SEChestOcclusiveDressing();
    PBPatientAction::Load(any.chestocclusivedressing(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kChronicObstructivePulmonaryDiseaseExacerbation:
  {
    SEChronicObstructivePulmonaryDiseaseExacerbation* a = new SEChronicObstructivePulmonaryDiseaseExacerbation();
    PBPatientAction::Load(any.chronicobstructivepulmonarydiseaseexacerbation(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kConsciousRespiration:
  {
    SEConsciousRespiration* a = new SEConsciousRespiration();
    PBPatientAction::Load(any.consciousrespiration(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kConsumeNutrients:
  {
    SEConsumeNutrients* a = new SEConsumeNutrients();
    PBPatientAction::Load(any.consumenutrients(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kDyspnea:
  {
    SEDyspnea* a = new SEDyspnea();
    PBPatientAction::Load(any.dyspnea(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kExercise:
  {
    SEExercise* a = new SEExercise();
    PBPatientAction::Load(any.exercise(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kHemorrhage:
  {
    SEHemorrhage* a = new SEHemorrhage();
    PBPatientAction::Load(any.hemorrhage(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kHemothorax:
  {
    SEHemothorax* a = new SEHemothorax();
    PBPatientAction::Load(any.hemothorax(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kImpairedAlveolarExchangeExacerbation:
  {
    SEImpairedAlveolarExchangeExacerbation* a = new SEImpairedAlveolarExchangeExacerbation();
    PBPatientAction::Load(any.impairedalveolarexchangeexacerbation(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kIntubation:
  {
    SEIntubation* a = new SEIntubation();
    PBPatientAction::Load(any.intubation(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kMechanicalVentilation:
  {
    SEMechanicalVentilation* a = new SEMechanicalVentilation();
    PBPatientAction::Load(any.mechanicalventilation(), *a, subMgr);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kNeedleDecompression:
  {
    SENeedleDecompression* a = new SENeedleDecompression();
    PBPatientAction::Load(any.needledecompression(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kPericardialEffusion:
  {
    SEPericardialEffusion* a = new SEPericardialEffusion();
    PBPatientAction::Load(any.pericardialeffusion(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kPneumoniaExacerbation:
  {
    SEPneumoniaExacerbation* a = new SEPneumoniaExacerbation();
    PBPatientAction::Load(any.pneumoniaexacerbation(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kPulmonaryShuntExacerbation:
  {
    SEPulmonaryShuntExacerbation* a = new SEPulmonaryShuntExacerbation();
    PBPatientAction::Load(any.pulmonaryshuntexacerbation(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kRespiratoryFatigue:
  {
    SERespiratoryFatigue* a = new SERespiratoryFatigue();
    PBPatientAction::Load(any.respiratoryfatigue(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kRespiratoryMechanicsConfiguration:
  {
    SERespiratoryMechanicsConfiguration* a = new SERespiratoryMechanicsConfiguration();
    PBPatientAction::Load(any.respiratorymechanicsconfiguration(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kRespiratoryMechanicsModification:
  {
    SERespiratoryMechanicsModification* a = new SERespiratoryMechanicsModification();
    PBPatientAction::Load(any.respiratorymechanicsmodification(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kSubstanceBolus:
  {
    const SESubstance* sub = subMgr.GetSubstance(any.substancebolus().substance());
    if (sub == nullptr)
    {
      subMgr.Error("Unknown substance " + any.substancebolus().substance());
      return nullptr;
    }
    SESubstanceBolus* a = new SESubstanceBolus(*sub);
    PBPatientAction::Load(any.substancebolus(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kSubstanceInfusion:
  {
    const SESubstance* sub = subMgr.GetSubstance(any.substanceinfusion().substance());
    if (sub == nullptr)
    {
      subMgr.Error("Unknown substance " + any.substanceinfusion().substance());
      return nullptr;
    }
    SESubstanceInfusion* a = new SESubstanceInfusion(*sub);
    PBPatientAction::Load(any.substanceinfusion(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kSubstanceCompoundInfusion:
  {
    const SESubstanceCompound* subC = subMgr.GetCompound(any.substancecompoundinfusion().substancecompound());
    if (subC == nullptr)
    {
      subMgr.Error("Unknown substance compound " + any.substancecompoundinfusion().substancecompound());
      return nullptr;
    }
    SESubstanceCompoundInfusion* a = new SESubstanceCompoundInfusion(*subC);
    PBPatientAction::Load(any.substancecompoundinfusion(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kSupplementalOxygen:
  {
    SESupplementalOxygen* a = new SESupplementalOxygen();
    PBPatientAction::Load(any.supplementaloxygen(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kTensionPneumothorax:
  {
    SETensionPneumothorax* a = new SETensionPneumothorax();
    PBPatientAction::Load(any.tensionpneumothorax(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kTubeThoracostomy:
  {
    SETubeThoracostomy* a = new SETubeThoracostomy();
    PBPatientAction::Load(any.tubethoracostomy(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::kUrinate:
  {
    SEUrinate* a = new SEUrinate();
    PBPatientAction::Load(any.urinate(), *a);
    return a;
  }
  case CDM_BIND::AnyPatientActionData::ActionCase::ACTION_NOT_SET:
  {
    subMgr.Warning("AnyPatienActionData Action is empty...was that intended?");
    return nullptr;
  }
  }
  subMgr.Error("Unknown action type : " + std::to_string(any.Action_case()));
  return nullptr;
}
CDM_BIND::AnyPatientActionData* PBPatientAction::Unload(const SEPatientAction& action)
{
  CDM_BIND::AnyPatientActionData* any = new CDM_BIND::AnyPatientActionData();
  const SEPatientAssessmentRequest* ar = dynamic_cast<const SEPatientAssessmentRequest*>(&action);
  if (ar != nullptr)
  {
    any->set_allocated_assessment(PBPatientAction::Unload(*ar));
    return any;
  }
  const SEAcuteRespiratoryDistressSyndromeExacerbation* ards = dynamic_cast<const SEAcuteRespiratoryDistressSyndromeExacerbation*>(&action);
  if (ards != nullptr)
  {
    any->set_allocated_acuterespiratorydistresssyndromeexacerbation(PBPatientAction::Unload(*ards));
    return any;
  }
  const SEAcuteStress* as = dynamic_cast<const SEAcuteStress*>(&action);
  if (as != nullptr)
  {
    any->set_allocated_acutestress(PBPatientAction::Unload(*as));
    return any;
  }
  const SEAirwayObstruction* ao = dynamic_cast<const SEAirwayObstruction*>(&action);
  if (ao != nullptr)
  {
    any->set_allocated_airwayobstruction(PBPatientAction::Unload(*ao));
    return any;
  }
  const SEArrhythmia* a = dynamic_cast<const SEArrhythmia*>(&action);
  if (a != nullptr)
  {
    any->set_allocated_arrhythmia(PBPatientAction::Unload(*a));
    return any;
  }
  const SEAsthmaAttack* aa = dynamic_cast<const SEAsthmaAttack*>(&action);
  if (aa != nullptr)
  {
    any->set_allocated_asthmaattack(PBPatientAction::Unload(*aa));
    return any;
  }
  const SEBrainInjury* bi = dynamic_cast<const SEBrainInjury*>(&action);
  if (bi != nullptr)
  {
    any->set_allocated_braininjury(PBPatientAction::Unload(*bi));
    return any;
  }
  const SEBronchoconstriction* b = dynamic_cast<const SEBronchoconstriction*>(&action);
  if (b != nullptr)
  {
    any->set_allocated_bronchoconstriction(PBPatientAction::Unload(*b));
    return any;
  }
  const SECardiovascularMechanicsModification* cmm = dynamic_cast<const SECardiovascularMechanicsModification*>(&action);
  if (cmm != nullptr)
  {
    any->set_allocated_cardiovascularmechanicsmodification(PBPatientAction::Unload(*cmm));
    return any;
  }
  const SEChestCompression* cc = dynamic_cast<const SEChestCompression*>(&action);
  if (cc != nullptr)
  {
    any->set_allocated_chestcompression(PBPatientAction::Unload(*cc));
    return any;
  }
  const SEChestCompressionAutomated* cca = dynamic_cast<const SEChestCompressionAutomated*>(&action);
  if (cca != nullptr)
  {
    any->set_allocated_chestcompressionautomated(PBPatientAction::Unload(*cca));
    return any;
  }
  const SEChestCompressionInstantaneous* cci = dynamic_cast<const SEChestCompressionInstantaneous*>(&action);
  if (cci != nullptr)
  {
    any->set_allocated_chestcompressioninstantaneous(PBPatientAction::Unload(*cci));
    return any;
  }
  const SEChestOcclusiveDressing* chd = dynamic_cast<const SEChestOcclusiveDressing*>(&action);
  if (chd != nullptr)
  {
    any->set_allocated_chestocclusivedressing(PBPatientAction::Unload(*chd));
    return any;
  }
  const SEChronicObstructivePulmonaryDiseaseExacerbation* copd = dynamic_cast<const SEChronicObstructivePulmonaryDiseaseExacerbation*>(&action);
  if (copd != nullptr)
  {
    any->set_allocated_chronicobstructivepulmonarydiseaseexacerbation(PBPatientAction::Unload(*copd));
    return any;
  }
  const SEConsciousRespiration* cr = dynamic_cast<const SEConsciousRespiration*>(&action);
  if (cr != nullptr)
  {
    any->set_allocated_consciousrespiration(PBPatientAction::Unload(*cr));
    return any;
  }
  const SEConsumeNutrients* cn = dynamic_cast<const SEConsumeNutrients*>(&action);
  if (cn != nullptr)
  {
    any->set_allocated_consumenutrients(PBPatientAction::Unload(*cn));
    return any;
  }
  const SEDyspnea* d = dynamic_cast<const SEDyspnea*>(&action);
  if (d != nullptr)
  {
    any->set_allocated_dyspnea(PBPatientAction::Unload(*d));
    return any;
  }
  const SEExercise* e = dynamic_cast<const SEExercise*>(&action);
  if (e != nullptr)
  {
    any->set_allocated_exercise(PBPatientAction::Unload(*e));
    return any;
  }
  const SEHemorrhage* h = dynamic_cast<const SEHemorrhage*>(&action);
  if (h != nullptr)
  {
    any->set_allocated_hemorrhage(PBPatientAction::Unload(*h));
    return any;
  }
  const SEHemothorax* hemo = dynamic_cast<const SEHemothorax*>(&action);
  if (hemo != nullptr)
  {
    any->set_allocated_hemothorax(PBPatientAction::Unload(*hemo));
    return any;
  }
  const SEImpairedAlveolarExchangeExacerbation* imaee = dynamic_cast<const SEImpairedAlveolarExchangeExacerbation*>(&action);
  if (imaee != nullptr)
  {
    any->set_allocated_impairedalveolarexchangeexacerbation(PBPatientAction::Unload(*imaee));
    return any;
  }
  const SEIntubation* i = dynamic_cast<const SEIntubation*>(&action);
  if (i != nullptr)
  {
    any->set_allocated_intubation(PBPatientAction::Unload(*i));
    return any;
  }
  const SEMechanicalVentilation* mv = dynamic_cast<const SEMechanicalVentilation*>(&action);
  if (mv != nullptr)
  {
    any->set_allocated_mechanicalventilation(PBPatientAction::Unload(*mv));
    return any;
  }
  const SENeedleDecompression* nd = dynamic_cast<const SENeedleDecompression*>(&action);
  if (nd != nullptr)
  {
    any->set_allocated_needledecompression(PBPatientAction::Unload(*nd));
    return any;
  }
  const SEPericardialEffusion* pe = dynamic_cast<const SEPericardialEffusion*>(&action);
  if (pe != nullptr)
  {
    any->set_allocated_pericardialeffusion(PBPatientAction::Unload(*pe));
    return any;
  }
  const SEPneumoniaExacerbation* lp = dynamic_cast<const SEPneumoniaExacerbation*>(&action);
  if (lp != nullptr)
  {
    any->set_allocated_pneumoniaexacerbation(PBPatientAction::Unload(*lp));
    return any;
  }
  const SEPulmonaryShuntExacerbation* pse = dynamic_cast<const SEPulmonaryShuntExacerbation*>(&action);
  if (pse != nullptr)
  {
    any->set_allocated_pulmonaryshuntexacerbation(PBPatientAction::Unload(*pse));
    return any;
  }
  const SERespiratoryFatigue* rf = dynamic_cast<const SERespiratoryFatigue*>(&action);
  if (rf != nullptr)
  {
    any->set_allocated_respiratoryfatigue(PBPatientAction::Unload(*rf));
    return any;
  }
  const SERespiratoryMechanicsConfiguration* rmc = dynamic_cast<const SERespiratoryMechanicsConfiguration*>(&action);
  if (rmc != nullptr)
  {
    any->set_allocated_respiratorymechanicsconfiguration(PBPatientAction::Unload(*rmc));
    return any;
  }
  const SERespiratoryMechanicsModification* rmm = dynamic_cast<const SERespiratoryMechanicsModification*>(&action);
  if (rmm != nullptr)
  {
    any->set_allocated_respiratorymechanicsmodification(PBPatientAction::Unload(*rmm));
    return any;
  }
  const SESupplementalOxygen* sO2 = dynamic_cast<const SESupplementalOxygen*>(&action);
  if (sO2 != nullptr)
  {
    any->set_allocated_supplementaloxygen(PBPatientAction::Unload(*sO2));
    return any;
  }
  const SESubstanceBolus* sb = dynamic_cast<const SESubstanceBolus*>(&action);
  if (sb != nullptr)
  {
    any->set_allocated_substancebolus(PBPatientAction::Unload(*sb));
    return any;
  }
  const SESubstanceInfusion* si = dynamic_cast<const SESubstanceInfusion*>(&action);
  if (si != nullptr)
  {
    any->set_allocated_substanceinfusion(PBPatientAction::Unload(*si));
    return any;
  }
  const SESubstanceCompoundInfusion* sci = dynamic_cast<const SESubstanceCompoundInfusion*>(&action);
  if (sci != nullptr)
  {
    any->set_allocated_substancecompoundinfusion(PBPatientAction::Unload(*sci));
    return any;
  }
  const SESupplementalOxygen* so2 = dynamic_cast<const SESupplementalOxygen*>(&action);
  if (so2 != nullptr)
  {
    any->set_allocated_supplementaloxygen(PBPatientAction::Unload(*so2));
    return any;
  }
  const SETensionPneumothorax* tp = dynamic_cast<const SETensionPneumothorax*>(&action);
  if (tp != nullptr)
  {
    any->set_allocated_tensionpneumothorax(PBPatientAction::Unload(*tp));
    return any;
  }
  const SETubeThoracostomy* tt = dynamic_cast<const SETubeThoracostomy*>(&action);
  if (tt != nullptr)
  {
    any->set_allocated_tubethoracostomy(PBPatientAction::Unload(*tt));
    return any;
  }
  const SEUrinate* u = dynamic_cast<const SEUrinate*>(&action);
  if (u != nullptr)
  {
    any->set_allocated_urinate(PBPatientAction::Unload(*u));
    return any;
  }
  delete any;
  return nullptr;
}
