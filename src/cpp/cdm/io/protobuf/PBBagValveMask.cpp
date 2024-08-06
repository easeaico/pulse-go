/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
PUSH_PROTO_WARNINGS
#include "pulse/cdm/bind/BagValveMask.pb.h"
POP_PROTO_WARNINGS
#include "cdm/io/protobuf/PBBagValveMask.h"
#include "cdm/io/protobuf/PBSubstance.h"
#include "cdm/io/protobuf/PBProperties.h"
#include "cdm/io/protobuf/PBUtils.h"
#include "cdm/system/equipment/bag_valve_mask/SEBagValveMask.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/utils/FileUtils.h"

void PBBagValveMask::Load(const CDM_BIND::BagValveMaskData& src, SEBagValveMask& dst, const SESubstanceManager& subMgr)
{
  dst.Clear();
  PBBagValveMask::Serialize(src, dst, subMgr);
  dst.StateChange();
}
void PBBagValveMask::Serialize(const CDM_BIND::BagValveMaskData& src, SEBagValveMask& dst, const SESubstanceManager& subMgr)
{
  dst.m_BreathState = (eBreathState)src.breathstate();
  dst.m_Connection = (eSwitch)src.connection();

  if (src.has_bagresistance())
    PBProperty::Load(src.bagresistance(), dst.GetBagResistance());
  if (src.has_filterresistance())
    PBProperty::Load(src.filterresistance(), dst.GetFilterResistance());
  if (src.has_sealresistance())
    PBProperty::Load(src.sealresistance(), dst.GetSealResistance());
  if (src.has_filtervolume())
    PBProperty::Load(src.filtervolume(), dst.GetFilterVolume());
  if (src.has_connectionvolume())
    PBProperty::Load(src.connectionvolume(), dst.GetConnectionVolume());
  if (src.has_valvevolume())
    PBProperty::Load(src.valvevolume(), dst.GetValveVolume());
  if (src.has_valvepositiveendexpiratorypressure())
    PBProperty::Load(src.valvepositiveendexpiratorypressure(), dst.GetValvePositiveEndExpiratoryPressure());
  if (src.has_valveresistance())
    PBProperty::Load(src.valveresistance(), dst.GetValveResistance());

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

CDM_BIND::BagValveMaskData* PBBagValveMask::Unload(const SEBagValveMask& src)
{
  CDM_BIND::BagValveMaskData* dst = new CDM_BIND::BagValveMaskData();
  PBBagValveMask::Serialize(src, *dst);
  return dst;
}
void PBBagValveMask::Serialize(const SEBagValveMask& src, CDM_BIND::BagValveMaskData& dst)
{
  dst.set_breathstate((CDM_BIND::eBreathState)src.m_BreathState.GetEnum());
  dst.set_connection((CDM_BIND::eSwitch)src.m_Connection.GetEnum());

  if (src.HasBagResistance())
    dst.set_allocated_bagresistance(PBProperty::Unload(*src.m_BagResistance));
  if (src.HasFilterResistance())
    dst.set_allocated_filterresistance(PBProperty::Unload(*src.m_FilterResistance));
  if (src.HasSealResistance())
    dst.set_allocated_sealresistance(PBProperty::Unload(*src.m_SealResistance));
  if (src.HasFilterVolume())
    dst.set_allocated_filtervolume(PBProperty::Unload(*src.m_FilterVolume));
  if (src.HasConnectionVolume())
    dst.set_allocated_connectionvolume(PBProperty::Unload(*src.m_ConnectionVolume));
  if (src.HasValveVolume())
    dst.set_allocated_valvevolume(PBProperty::Unload(*src.m_ValveVolume));
  if (src.HasValvePositiveEndExpiratoryPressure())
    dst.set_allocated_valvepositiveendexpiratorypressure(PBProperty::Unload(*src.m_ValvePositiveEndExpiratoryPressure));
  if (src.HasValveResistance())
    dst.set_allocated_valveresistance(PBProperty::Unload(*src.m_ValveResistance));

  for (SESubstanceFraction* sf : src.m_FractionInspiredGases)
    dst.mutable_fractioninspiredgas()->AddAllocated(PBSubstance::Unload(*sf));

  for (SESubstanceConcentration* sc : src.m_ConcentrationInspiredAerosols)
    dst.mutable_concentrationinspiredaerosol()->AddAllocated(PBSubstance::Unload(*sc));
}

bool PBBagValveMask::SerializeToString(const SEBagValveMask& src, std::string& output, eSerializationFormat m)
{
  CDM_BIND::BagValveMaskData data;
  PBBagValveMask::Serialize(src, data);
  return PBUtils::SerializeToString(data, output, m, src.GetLogger());
}
bool PBBagValveMask::SerializeToFile(const SEBagValveMask& src, const std::string& filename)
{
  CDM_BIND::BagValveMaskData data;
  PBBagValveMask::Serialize(src, data);
  return PBUtils::SerializeToFile(data, filename, src.GetLogger());
}

bool PBBagValveMask::SerializeFromString(const std::string& src, SEBagValveMask& dst, eSerializationFormat m, const SESubstanceManager& subMgr)
{
  CDM_BIND::BagValveMaskData data;
  if (!PBUtils::SerializeFromString(src, data, m, dst.GetLogger()))
    return false;
  PBBagValveMask::Load(data, dst, subMgr);
  return true;
}
bool PBBagValveMask::SerializeFromFile(const std::string& filename, SEBagValveMask& dst, const SESubstanceManager& subMgr)
{
  CDM_BIND::BagValveMaskData data;
  if (!PBUtils::SerializeFromFile(filename, data, dst.GetLogger()))
    return false;
  PBBagValveMask::Load(data, dst, subMgr);
  return true;
}
