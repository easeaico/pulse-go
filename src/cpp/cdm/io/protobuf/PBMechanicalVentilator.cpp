/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
PUSH_PROTO_WARNINGS
#include "pulse/cdm/bind/MechanicalVentilator.pb.h"
POP_PROTO_WARNINGS
#include "cdm/io/protobuf/PBMechanicalVentilator.h"
#include "cdm/io/protobuf/PBSubstance.h"
#include "cdm/io/protobuf/PBProperties.h"
#include "cdm/io/protobuf/PBUtils.h"
#include "cdm/system/equipment/mechanical_ventilator/SEMechanicalVentilator.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/utils/FileUtils.h"

void PBMechanicalVentilator::Load(const CDM_BIND::MechanicalVentilatorData& src, SEMechanicalVentilator& dst, const SESubstanceManager& subMgr)
{
  dst.Clear();
  PBMechanicalVentilator::Serialize(src, dst, subMgr);
  dst.StateChange();
}
void PBMechanicalVentilator::Serialize(const CDM_BIND::MechanicalVentilatorData& src, SEMechanicalVentilator& dst, const SESubstanceManager& subMgr)
{
  if (src.has_airwaypressure())
    PBProperty::Load(src.airwaypressure(), dst.GetAirwayPressure());
  dst.m_BreathState = (eBreathState)src.breathstate();
  if (src.has_dynamicrespiratorycompliance())
    PBProperty::Load(src.dynamicrespiratorycompliance(), dst.GetDynamicRespiratoryCompliance());
  if (src.has_endtidalcarbondioxidefraction())
    PBProperty::Load(src.endtidalcarbondioxidefraction(), dst.GetEndTidalCarbonDioxideFraction());
  if (src.has_endtidalcarbondioxidepressure())
    PBProperty::Load(src.endtidalcarbondioxidepressure(), dst.GetEndTidalCarbonDioxidePressure());
  if (src.has_endtidaloxygenfraction())
    PBProperty::Load(src.endtidaloxygenfraction(), dst.GetEndTidalOxygenFraction());
  if (src.has_endtidaloxygenpressure())
    PBProperty::Load(src.endtidaloxygenpressure(), dst.GetEndTidalOxygenPressure());
  if (src.has_expiratoryflow())
    PBProperty::Load(src.expiratoryflow(), dst.GetExpiratoryFlow());
  if (src.has_expiratoryresistance())
    PBProperty::Load(src.expiratoryresistance(), dst.GetExpiratoryResistance());
  if (src.has_expiratorytidalvolume())
    PBProperty::Load(src.expiratorytidalvolume(), dst.GetExpiratoryTidalVolume());
  if (src.has_extrinsicpositiveendexpiratorypressure())
    PBProperty::Load(src.extrinsicpositiveendexpiratorypressure(), dst.GetExtrinsicPositiveEndExpiratoryPressure());
  if (src.has_inspiratoryexpiratoryratio())
    PBProperty::Load(src.inspiratoryexpiratoryratio(), dst.GetInspiratoryExpiratoryRatio());
  if (src.has_inspiratoryflow())
    PBProperty::Load(src.inspiratoryflow(), dst.GetInspiratoryFlow());
  if (src.has_inspiratoryresistance())
    PBProperty::Load(src.inspiratoryresistance(), dst.GetInspiratoryResistance());
  if (src.has_inspiratorytidalvolume())
    PBProperty::Load(src.inspiratorytidalvolume(), dst.GetInspiratoryTidalVolume());
  if (src.has_intrinsicpositiveendexpiratorypressure())
    PBProperty::Load(src.intrinsicpositiveendexpiratorypressure(), dst.GetIntrinsicPositiveEndExpiratoryPressure());
  if (src.has_leakfraction())
    PBProperty::Load(src.leakfraction(), dst.GetLeakFraction());
  if (src.has_meanairwaypressure())
    PBProperty::Load(src.meanairwaypressure(), dst.GetMeanAirwayPressure());
  if (src.has_peakexpiratoryflow())
    PBProperty::Load(src.peakexpiratoryflow(), dst.GetPeakExpiratoryFlow());
  if (src.has_peakinspiratoryflow())
    PBProperty::Load(src.peakinspiratoryflow(), dst.GetPeakInspiratoryFlow());
  if (src.has_peakinspiratorypressure())
    PBProperty::Load(src.peakinspiratorypressure(), dst.GetPeakInspiratoryPressure());
  if (src.has_plateaupressure())
    PBProperty::Load(src.plateaupressure(), dst.GetPlateauPressure());
  if (src.has_respirationrate())
    PBProperty::Load(src.respirationrate(), dst.GetRespirationRate());
  if (src.has_staticrespiratorycompliance())
    PBProperty::Load(src.staticrespiratorycompliance(), dst.GetStaticRespiratoryCompliance());
  if (src.has_tidalvolume())
    PBProperty::Load(src.tidalvolume(), dst.GetTidalVolume());
  if (src.has_totallungvolume())
    PBProperty::Load(src.totallungvolume(), dst.GetTotalLungVolume());
  if (src.has_totalpositiveendexpiratorypressure())
    PBProperty::Load(src.totalpositiveendexpiratorypressure(), dst.GetTotalPositiveEndExpiratoryPressure());
  if (src.has_totalpulmonaryventilation())
    PBProperty::Load(src.totalpulmonaryventilation(), dst.GetTotalPulmonaryVentilation());

  if (src.has_settings())
    PBMechanicalVentilator::Load(src.settings(), dst.GetSettings(), subMgr);
}
CDM_BIND::MechanicalVentilatorData* PBMechanicalVentilator::Unload(const SEMechanicalVentilator& src)
{
  CDM_BIND::MechanicalVentilatorData* dst = new CDM_BIND::MechanicalVentilatorData();
  PBMechanicalVentilator::Serialize(src, *dst);
  return dst;
}
void PBMechanicalVentilator::Serialize(const SEMechanicalVentilator& src, CDM_BIND::MechanicalVentilatorData& dst)
{
  if (src.HasAirwayPressure())
    dst.set_allocated_airwaypressure(PBProperty::Unload(*src.m_AirwayPressure));
  dst.set_breathstate((CDM_BIND::eBreathState)src.m_BreathState.GetEnum());
  if (src.HasDynamicRespiratoryCompliance())
    dst.set_allocated_dynamicrespiratorycompliance(PBProperty::Unload(*src.m_DynamicRespiratoryCompliance));
  if (src.HasEndTidalCarbonDioxideFraction())
    dst.set_allocated_endtidalcarbondioxidefraction(PBProperty::Unload(*src.m_EndTidalCarbonDioxideFraction));
  if (src.HasEndTidalCarbonDioxidePressure())
    dst.set_allocated_endtidalcarbondioxidepressure(PBProperty::Unload(*src.m_EndTidalCarbonDioxidePressure));
  if (src.HasEndTidalOxygenFraction())
    dst.set_allocated_endtidaloxygenfraction(PBProperty::Unload(*src.m_EndTidalOxygenFraction));
  if (src.HasEndTidalOxygenPressure())
    dst.set_allocated_endtidaloxygenpressure(PBProperty::Unload(*src.m_EndTidalOxygenPressure));
  if (src.HasExpiratoryFlow())
    dst.set_allocated_expiratoryflow(PBProperty::Unload(*src.m_ExpiratoryFlow));
  if (src.HasExpiratoryResistance())
    dst.set_allocated_expiratoryresistance(PBProperty::Unload(*src.m_ExpiratoryResistance));
  if (src.HasExpiratoryTidalVolume())
    dst.set_allocated_expiratorytidalvolume(PBProperty::Unload(*src.m_ExpiratoryTidalVolume));
  if (src.HasExtrinsicPositiveEndExpiratoryPressure())
    dst.set_allocated_extrinsicpositiveendexpiratorypressure(PBProperty::Unload(*src.m_ExtrinsicPositiveEndExpiratoryPressure));
  if (src.HasInspiratoryExpiratoryRatio())
    dst.set_allocated_inspiratoryexpiratoryratio(PBProperty::Unload(*src.m_InspiratoryExpiratoryRatio));
  if (src.HasInspiratoryFlow())
    dst.set_allocated_inspiratoryflow(PBProperty::Unload(*src.m_InspiratoryFlow));
  if (src.HasInspiratoryResistance())
    dst.set_allocated_inspiratoryresistance(PBProperty::Unload(*src.m_InspiratoryResistance));
  if (src.HasInspiratoryTidalVolume())
    dst.set_allocated_inspiratorytidalvolume(PBProperty::Unload(*src.m_InspiratoryTidalVolume));
  if (src.HasIntrinsicPositiveEndExpiratoryPressure())
    dst.set_allocated_intrinsicpositiveendexpiratorypressure(PBProperty::Unload(*src.m_IntrinsicPositiveEndExpiratoryPressure));
  if (src.HasLeakFraction())
    dst.set_allocated_leakfraction(PBProperty::Unload(*src.m_LeakFraction));
  if (src.HasMeanAirwayPressure())
    dst.set_allocated_meanairwaypressure(PBProperty::Unload(*src.m_MeanAirwayPressure));
  if (src.HasPeakExpiratoryFlow())
    dst.set_allocated_peakexpiratoryflow(PBProperty::Unload(*src.m_PeakExpiratoryFlow));
  if (src.HasPeakInspiratoryFlow())
    dst.set_allocated_peakinspiratoryflow(PBProperty::Unload(*src.m_PeakInspiratoryFlow));
  if (src.HasPeakInspiratoryPressure())
    dst.set_allocated_peakinspiratorypressure(PBProperty::Unload(*src.m_PeakInspiratoryPressure));
  if (src.HasPlateauPressure())
    dst.set_allocated_plateaupressure(PBProperty::Unload(*src.m_PlateauPressure));
  if (src.HasRespirationRate())
    dst.set_allocated_respirationrate(PBProperty::Unload(*src.m_RespirationRate));
  if (src.HasStaticRespiratoryCompliance())
    dst.set_allocated_staticrespiratorycompliance(PBProperty::Unload(*src.m_StaticRespiratoryCompliance));
  if (src.HasTidalVolume())
    dst.set_allocated_tidalvolume(PBProperty::Unload(*src.m_TidalVolume));
  if (src.HasTotalLungVolume())
    dst.set_allocated_totallungvolume(PBProperty::Unload(*src.m_TotalLungVolume));
  if (src.HasTotalPositiveEndExpiratoryPressure())
    dst.set_allocated_totalpositiveendexpiratorypressure(PBProperty::Unload(*src.m_TotalPositiveEndExpiratoryPressure));
  if (src.HasTotalPulmonaryVentilation())
    dst.set_allocated_totalpulmonaryventilation(PBProperty::Unload(*src.m_TotalPulmonaryVentilation));

  if (src.HasSettings())
    dst.set_allocated_settings(PBMechanicalVentilator::Unload(*src.m_Settings));
}

void PBMechanicalVentilator::Load(const CDM_BIND::MechanicalVentilatorSettingsData& src, SEMechanicalVentilatorSettings& dst, const SESubstanceManager& subMgr)
{
  dst.Clear();
  PBMechanicalVentilator::Serialize(src, dst, subMgr);
}
void PBMechanicalVentilator::Serialize(const CDM_BIND::MechanicalVentilatorSettingsData& src, SEMechanicalVentilatorSettings& dst, const SESubstanceManager& subMgr)
{
  dst.m_Connection = (eSwitch)src.connection();
  if (src.has_connectionvolume())
    PBProperty::Load(src.connectionvolume(), dst.GetConnectionVolume());
  if (src.has_compliance())
    PBProperty::Load(src.compliance(), dst.GetCompliance());
  if (src.has_driverdampingparameter())
    PBProperty::Load(src.driverdampingparameter(), dst.GetDriverDampingParameter());

  if (src.has_positiveendexpiratorypressure())
    PBProperty::Load(src.positiveendexpiratorypressure(), dst.GetPositiveEndExpiratoryPressure());
  else if (src.has_functionalresidualcapacity())
    PBProperty::Load(src.functionalresidualcapacity(), dst.GetFunctionalResidualCapacity());

  if (src.has_expirationcycleflow())
    PBProperty::Load(src.expirationcycleflow(), dst.GetExpirationCycleFlow());
  else if (src.has_expirationcyclepressure())
    PBProperty::Load(src.expirationcyclepressure(), dst.GetExpirationCyclePressure());
  else if (src.has_expirationcyclevolume())
    PBProperty::Load(src.expirationcyclevolume(), dst.GetExpirationCycleVolume());
  else if (src.has_expirationcycletime())
    PBProperty::Load(src.expirationcycletime(), dst.GetExpirationCycleTime());
  else
    dst.m_ExpirationCycleRespiratoryModel = (eSwitch)src.expirationcyclerespiratorymodel();

  if (src.has_expirationlimbvolume())
    PBProperty::Load(src.expirationlimbvolume(), dst.GetExpirationLimbVolume());
  if (src.has_expirationtuberesistance())
    PBProperty::Load(src.expirationtuberesistance(), dst.GetExpirationTubeResistance());
  if (src.has_expirationvalveresistance())
    PBProperty::Load(src.expirationvalveresistance(), dst.GetExpirationValveResistance());
  if (src.has_expirationvalvevolume())
    PBProperty::Load(src.expirationvalvevolume(), dst.GetExpirationValveVolume());
  dst.SetExpirationWaveform((eDriverWaveform)src.expirationwaveform());
  if (src.has_expirationwaveformperiod())
    PBProperty::Load(src.expirationwaveformperiod(), dst.GetExpirationWaveformPeriod());

  if (src.has_inspirationlimitflow())
    PBProperty::Load(src.inspirationlimitflow(), dst.GetInspirationLimitFlow());
  else if (src.has_inspirationlimitpressure())
    PBProperty::Load(src.inspirationlimitpressure(), dst.GetInspirationLimitPressure());
  else if (src.has_inspirationlimitvolume())
    PBProperty::Load(src.inspirationlimitvolume(), dst.GetInspirationLimitVolume());

  if (src.has_inspirationpausetime())
    PBProperty::Load(src.inspirationpausetime(), dst.GetInspirationPauseTime());

  if (src.has_peakinspiratorypressure())
    PBProperty::Load(src.peakinspiratorypressure(), dst.GetPeakInspiratoryPressure());
  else if (src.has_inspirationtargetflow())
    PBProperty::Load(src.inspirationtargetflow(), dst.GetInspirationTargetFlow());

  if (src.has_inspirationmachinetriggertime())
    PBProperty::Load(src.inspirationmachinetriggertime(), dst.GetInspirationMachineTriggerTime());

  if (src.has_inspirationpatienttriggerflow())
    PBProperty::Load(src.inspirationpatienttriggerflow(), dst.GetInspirationPatientTriggerFlow());
  else if (src.has_inspirationpatienttriggerpressure())
    PBProperty::Load(src.inspirationpatienttriggerpressure(), dst.GetInspirationPatientTriggerPressure());
  else
    dst.m_InspirationPatientTriggerRespiratoryModel = (eSwitch)src.inspirationpatienttriggerrespiratorymodel();

  if (src.has_inspirationlimbvolume())
    PBProperty::Load(src.inspirationlimbvolume(), dst.GetInspirationLimbVolume());
  if (src.has_inspirationtuberesistance())
    PBProperty::Load(src.inspirationtuberesistance(), dst.GetInspirationTubeResistance());
  if (src.has_inspirationvalveresistance())
    PBProperty::Load(src.inspirationvalveresistance(), dst.GetInspirationValveResistance());
  if (src.has_inspirationvalvevolume())
    PBProperty::Load(src.inspirationvalvevolume(), dst.GetInspirationValveVolume());
  dst.SetInspirationWaveform((eDriverWaveform)src.inspirationwaveform());
  if (src.has_inspirationwaveformperiod())
    PBProperty::Load(src.inspirationwaveformperiod(), dst.GetInspirationWaveformPeriod());

  if (src.has_reliefvalvethreshold())
    PBProperty::Load(src.reliefvalvethreshold(), dst.GetReliefValveThreshold());
  if (src.has_ypiecevolume())
    PBProperty::Load(src.ypiecevolume(), dst.GetYPieceVolume());

  const SESubstance* sub;
  for (int i = 0; i < src.fractioninspiredgas_size(); i++)
  {
    const CDM_BIND::SubstanceFractionData& sfData = src.fractioninspiredgas()[i];
    sub = subMgr.GetSubstance(sfData.name());
    if (sub == nullptr)
    {
      dst.Error("Ignoring a mechanical ventilator fraction inspired gas that was not found : " + sfData.name());
      continue;
    }
    if (sub->GetState() != eSubstance_State::Gas)
    {
      dst.Error("Ignoring a mechanical ventilator fraction gas that is not a gas : " + sfData.name());
      continue;
    }
    PBSubstance::Load(sfData, dst.GetFractionInspiredGas(*sub));
  }

  for (int i = 0; i < src.concentrationinspiredaerosol_size(); i++)
  {
    const CDM_BIND::SubstanceConcentrationData& scData = src.concentrationinspiredaerosol()[i];
    sub = subMgr.GetSubstance(scData.name());
    if (sub == nullptr)
    {
      dst.Error("Ignoring an mechanical ventilator aerosol concentration that was not found : " + scData.name());
      continue;
    }
    if (sub->GetState() != eSubstance_State::Liquid && sub->GetState() != eSubstance_State::Solid)
    {
      dst.Error("Ignoring an mechanical ventilator aerosol concentration that is not a gas : " + scData.name());
      continue;
    }
    PBSubstance::Load(scData, dst.GetConcentrationInspiredAerosol(*sub));
  }
}

CDM_BIND::MechanicalVentilatorSettingsData* PBMechanicalVentilator::Unload(const SEMechanicalVentilatorSettings& src)
{
  CDM_BIND::MechanicalVentilatorSettingsData* dst = new CDM_BIND::MechanicalVentilatorSettingsData();
  PBMechanicalVentilator::Serialize(src, *dst);
  return dst;
}
void PBMechanicalVentilator::Serialize(const SEMechanicalVentilatorSettings& src, CDM_BIND::MechanicalVentilatorSettingsData& dst)
{
  dst.set_connection((CDM_BIND::eSwitch)src.m_Connection);
  if (src.HasConnectionVolume())
    dst.set_allocated_connectionvolume(PBProperty::Unload(*src.m_ConnectionVolume));
  if (src.HasCompliance())
    dst.set_allocated_compliance(PBProperty::Unload(*src.m_Compliance));
  if (src.HasDriverDampingParameter())
    dst.set_allocated_driverdampingparameter(PBProperty::Unload(*src.m_DriverDampingParameter));

  if (src.HasPositiveEndExpiratoryPressure())
    dst.set_allocated_positiveendexpiratorypressure(PBProperty::Unload(*src.m_PositiveEndExpiratoryPressure));
  else if (src.HasFunctionalResidualCapacity())
    dst.set_allocated_functionalresidualcapacity(PBProperty::Unload(*src.m_FunctionalResidualCapacity));

  if (src.HasExpirationCycleFlow())
    dst.set_allocated_expirationcycleflow(PBProperty::Unload(*src.m_ExpirationCycleFlow));
  else if (src.HasExpirationCyclePressure())
    dst.set_allocated_expirationcyclepressure(PBProperty::Unload(*src.m_ExpirationCyclePressure));
  else if (src.HasExpirationCycleVolume())
    dst.set_allocated_expirationcyclevolume(PBProperty::Unload(*src.m_ExpirationCycleVolume));
  else if (src.HasExpirationCycleTime())
    dst.set_allocated_expirationcycletime(PBProperty::Unload(*src.m_ExpirationCycleTime));
  else
    dst.set_expirationcyclerespiratorymodel((CDM_BIND::eSwitch)src.m_ExpirationCycleRespiratoryModel);

  if (src.HasExpirationLimbVolume())
    dst.set_allocated_expirationlimbvolume(PBProperty::Unload(*src.m_ExpirationLimbVolume));
  if (src.HasExpirationTubeResistance())
    dst.set_allocated_expirationtuberesistance(PBProperty::Unload(*src.m_ExpirationTubeResistance));
  if (src.HasExpirationValveVolume())
    dst.set_allocated_expirationvalvevolume(PBProperty::Unload(*src.m_ExpirationValveVolume));
  if (src.HasExpirationValveResistance())
    dst.set_allocated_expirationvalveresistance(PBProperty::Unload(*src.m_ExpirationValveResistance));
  dst.set_expirationwaveform((CDM_BIND::eDriverWaveform)src.m_ExpirationWaveform);
  if (src.HasExpirationWaveformPeriod())
    dst.set_allocated_expirationwaveformperiod(PBProperty::Unload(*src.m_ExpirationWaveformPeriod));

  if (src.HasInspirationLimitFlow())
    dst.set_allocated_inspirationlimitflow(PBProperty::Unload(*src.m_InspirationLimitFlow));
  else if (src.HasInspirationLimitPressure())
    dst.set_allocated_inspirationlimitpressure(PBProperty::Unload(*src.m_InspirationLimitPressure));
  else if (src.HasInspirationLimitVolume())
    dst.set_allocated_inspirationlimitvolume(PBProperty::Unload(*src.m_InspirationLimitVolume));

  if (src.HasInspirationPauseTime())
    dst.set_allocated_inspirationpausetime(PBProperty::Unload(*src.m_InspirationPauseTime));

  if (src.HasPeakInspiratoryPressure())
    dst.set_allocated_peakinspiratorypressure(PBProperty::Unload(*src.m_PeakInspiratoryPressure));
  else if (src.HasInspirationTargetFlow())
    dst.set_allocated_inspirationtargetflow(PBProperty::Unload(*src.m_InspirationTargetFlow));

  if (src.HasInspirationMachineTriggerTime())
    dst.set_allocated_inspirationmachinetriggertime(PBProperty::Unload(*src.m_InspirationMachineTriggerTime));

  if (src.HasInspirationPatientTriggerFlow())
    dst.set_allocated_inspirationpatienttriggerflow(PBProperty::Unload(*src.m_InspirationPatientTriggerFlow));
  else if (src.HasInspirationPatientTriggerPressure())
    dst.set_allocated_inspirationpatienttriggerpressure(PBProperty::Unload(*src.m_InspirationPatientTriggerPressure));
  else
    dst.set_inspirationpatienttriggerrespiratorymodel((CDM_BIND::eSwitch)src.m_InspirationPatientTriggerRespiratoryModel);

  if (src.HasInspirationTubeResistance())
    dst.set_allocated_inspirationtuberesistance(PBProperty::Unload(*src.m_InspirationTubeResistance));
  if (src.HasInspirationValveResistance())
    dst.set_allocated_inspirationvalveresistance(PBProperty::Unload(*src.m_InspirationValveResistance));
  if (src.HasInspirationLimbVolume())
    dst.set_allocated_inspirationlimbvolume(PBProperty::Unload(*src.m_InspirationLimbVolume));
  if (src.HasInspirationValveVolume())
    dst.set_allocated_inspirationvalvevolume(PBProperty::Unload(*src.m_InspirationValveVolume));
  dst.set_inspirationwaveform((CDM_BIND::eDriverWaveform)src.m_InspirationWaveform);
  if (src.HasInspirationWaveformPeriod())
    dst.set_allocated_inspirationwaveformperiod(PBProperty::Unload(*src.m_InspirationWaveformPeriod));

  if (src.HasReliefValveThreshold())
    dst.set_allocated_reliefvalvethreshold(PBProperty::Unload(*src.m_ReliefValveThreshold));
  if (src.HasYPieceVolume())
    dst.set_allocated_ypiecevolume(PBProperty::Unload(*src.m_YPieceVolume));

  for (SESubstanceFraction* sf : src.m_FractionInspiredGases)
    dst.mutable_fractioninspiredgas()->AddAllocated(PBSubstance::Unload(*sf));

  for (SESubstanceConcentration* sc : src.m_ConcentrationInspiredAerosols)
    dst.mutable_concentrationinspiredaerosol()->AddAllocated(PBSubstance::Unload(*sc));
}

bool PBMechanicalVentilator::SerializeToString(const SEMechanicalVentilatorSettings& src, std::string& output, eSerializationFormat m)
{
  CDM_BIND::MechanicalVentilatorSettingsData data;
  PBMechanicalVentilator::Serialize(src, data);
  return PBUtils::SerializeToString(data, output, m, src.GetLogger());
}
bool PBMechanicalVentilator::SerializeToFile(const SEMechanicalVentilatorSettings& src, const std::string& filename)
{
  CDM_BIND::MechanicalVentilatorSettingsData data;
  PBMechanicalVentilator::Serialize(src, data);
  return PBUtils::SerializeToFile(data, filename, src.GetLogger());
}

bool PBMechanicalVentilator::SerializeFromString(const std::string& src, SEMechanicalVentilatorSettings& dst, eSerializationFormat m, const SESubstanceManager& subMgr)
{
  CDM_BIND::MechanicalVentilatorSettingsData data;
  if (!PBUtils::SerializeFromString(src, data, m, dst.GetLogger()))
    return false;
  PBMechanicalVentilator::Load(data, dst, subMgr);
  return true;
}
bool PBMechanicalVentilator::SerializeFromFile(const std::string& filename, SEMechanicalVentilatorSettings& dst, const SESubstanceManager& subMgr)
{
  CDM_BIND::MechanicalVentilatorSettingsData data;
  if (!PBUtils::SerializeFromFile(filename, data, dst.GetLogger()))
    return false;
  PBMechanicalVentilator::Load(data, dst, subMgr);
  return true;
}
