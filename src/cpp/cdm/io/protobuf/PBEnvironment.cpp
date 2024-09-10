/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
PUSH_PROTO_WARNINGS
#include "pulse/cdm/bind/Environment.pb.h"
POP_PROTO_WARNINGS
#include "cdm/io/protobuf/PBEnvironment.h"
#include "cdm/io/protobuf/PBSubstance.h"
#include "cdm/io/protobuf/PBProperties.h"
#include "cdm/io/protobuf/PBUtils.h"
#include "cdm/system/environment/SEEnvironment.h"
#include "cdm/system/environment/SEActiveConditioning.h"
#include "cdm/system/environment/SEAppliedTemperature.h"
#include "cdm/system/environment/SEEnvironmentalConditions.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/properties/SEScalarPower.h"
#include "cdm/utils/FileUtils.h"

void PBEnvironment::Load(const CDM_BIND::EnvironmentData& src, SEEnvironment& dst, const SESubstanceManager& subMgr)
{
  dst.Clear();
  PBEnvironment::Serialize(src, dst, subMgr);
  dst.StateChange();
}
void PBEnvironment::Serialize(const CDM_BIND::EnvironmentData& src, SEEnvironment& dst, const SESubstanceManager& subMgr)
{
  if (src.has_convectiveheatloss())
    PBProperty::Load(src.convectiveheatloss(), dst.GetConvectiveHeatLoss());
  if (src.has_convectiveheattranfercoefficient())
    PBProperty::Load(src.convectiveheattranfercoefficient(), dst.GetConvectiveHeatTranferCoefficient());
  if (src.has_evaporativeheatloss())
    PBProperty::Load(src.evaporativeheatloss(), dst.GetEvaporativeHeatLoss());
  if (src.has_evaporativeheattranfercoefficient())
    PBProperty::Load(src.evaporativeheattranfercoefficient(), dst.GetEvaporativeHeatTranferCoefficient());
  if (src.has_radiativeheatloss())
    PBProperty::Load(src.radiativeheatloss(), dst.GetRadiativeHeatLoss());
  if (src.has_radiativeheattranfercoefficient())
    PBProperty::Load(src.radiativeheattranfercoefficient(), dst.GetRadiativeHeatTranferCoefficient());
  if (src.has_respirationheatloss())
    PBProperty::Load(src.respirationheatloss(), dst.GetRespirationHeatLoss());
  if (src.has_skinheatloss())
    PBProperty::Load(src.skinheatloss(), dst.GetSkinHeatLoss());

  if (src.has_environmentalconditions())
    PBEnvironment::Load(src.environmentalconditions(), dst.GetEnvironmentalConditions(), subMgr);
  if (src.has_activeheating())
    PBEnvironment::Load(src.activeheating(), dst.GetActiveHeating());
  if (src.has_activecooling())
    PBEnvironment::Load(src.activecooling(), dst.GetActiveCooling());
  if (src.has_appliedtemperature())
    PBEnvironment::Load(src.appliedtemperature(), dst.GetAppliedTemperature());
}
CDM_BIND::EnvironmentData* PBEnvironment::Unload(const SEEnvironment& src)
{
  CDM_BIND::EnvironmentData* dst = new CDM_BIND::EnvironmentData();
  PBEnvironment::Serialize(src, *dst);
  return dst;
}
void PBEnvironment::Serialize(const SEEnvironment& src, CDM_BIND::EnvironmentData& dst)
{
  if (src.HasConvectiveHeatLoss())
    dst.set_allocated_convectiveheatloss(PBProperty::Unload(*src.m_ConvectiveHeatLoss));
  if (src.HasConvectiveHeatTranferCoefficient())
    dst.set_allocated_convectiveheattranfercoefficient(PBProperty::Unload(*src.m_ConvectiveHeatTranferCoefficient));
  if (src.HasEvaporativeHeatLoss())
    dst.set_allocated_evaporativeheatloss(PBProperty::Unload(*src.m_EvaporativeHeatLoss));
  if (src.HasEvaporativeHeatTranferCoefficient())
    dst.set_allocated_evaporativeheattranfercoefficient(PBProperty::Unload(*src.m_EvaporativeHeatTranferCoefficient));
  if (src.HasRadiativeHeatLoss())
    dst.set_allocated_radiativeheatloss(PBProperty::Unload(*src.m_RadiativeHeatLoss));
  if (src.HasRadiativeHeatTranferCoefficient())
    dst.set_allocated_radiativeheattranfercoefficient(PBProperty::Unload(*src.m_RadiativeHeatTranferCoefficient));
  if (src.HasRespirationHeatLoss())
    dst.set_allocated_respirationheatloss(PBProperty::Unload(*src.m_RespirationHeatLoss));
  if (src.HasSkinHeatLoss())
    dst.set_allocated_skinheatloss(PBProperty::Unload(*src.m_SkinHeatLoss));

  if (src.HasActiveHeating() && src.m_ActiveHeating->GetPower().IsPositive())
    dst.set_allocated_activeheating(PBEnvironment::Unload(*src.m_ActiveHeating));
  if (src.HasActiveCooling() && src.m_ActiveCooling->GetPower().IsPositive())
    dst.set_allocated_activecooling(PBEnvironment::Unload(*src.m_ActiveCooling));
  if (src.HasAppliedTemperature())
    dst.set_allocated_appliedtemperature(PBEnvironment::Unload(*src.m_AppliedTemperature));
  if (src.HasEnvironmentalConditions())
    dst.set_allocated_environmentalconditions(PBEnvironment::Unload(*src.m_EnvironmentalConditions));
}
void PBEnvironment::Copy(const SEEnvironment& src, SEEnvironment& dst, const SESubstanceManager& subMgr)
{
  dst.Clear();
  CDM_BIND::EnvironmentData data;
  PBEnvironment::Serialize(src, data);
  PBEnvironment::Serialize(data, dst, subMgr);
}

void PBEnvironment::Load(const CDM_BIND::EnvironmentalConditionsData& src, SEEnvironmentalConditions& dst, const SESubstanceManager& subMgr)
{
  dst.Clear();
  PBEnvironment::Serialize(src, dst, subMgr);
}
void PBEnvironment::Serialize(const CDM_BIND::EnvironmentalConditionsData& src, SEEnvironmentalConditions& dst, const SESubstanceManager& subMgr)
{
  dst.SetSurroundingType((eSurroundingType)src.surroundingtype());
  if (src.has_airdensity())
    PBProperty::Load(src.airdensity(), dst.GetAirDensity());
  if (src.has_airvelocity())
    PBProperty::Load(src.airvelocity(), dst.GetAirVelocity());
  if (src.has_ambienttemperature())
    PBProperty::Load(src.ambienttemperature(), dst.GetAmbientTemperature());
  if (src.has_atmosphericpressure())
    PBProperty::Load(src.atmosphericpressure(), dst.GetAtmosphericPressure());
  if (src.has_clothingresistance())
    PBProperty::Load(src.clothingresistance(), dst.GetClothingResistance());
  if (src.has_emissivity())
    PBProperty::Load(src.emissivity(), dst.GetEmissivity());
  if (src.has_meanradianttemperature())
    PBProperty::Load(src.meanradianttemperature(), dst.GetMeanRadiantTemperature());
  if (src.has_mechanicaldeadspace())
    PBProperty::Load(src.mechanicaldeadspace(), dst.GetMechanicalDeadSpace());
  if (src.has_relativehumidity())
    PBProperty::Load(src.relativehumidity(), dst.GetRelativeHumidity());
  if (src.has_respirationambienttemperature())
    PBProperty::Load(src.respirationambienttemperature(), dst.GetRespirationAmbientTemperature());

  const SESubstance* sub;
  for (int i = 0; i < src.ambientgas_size(); i++)
  {
    const CDM_BIND::SubstanceFractionData& sfData = src.ambientgas()[i];
    sub = subMgr.GetSubstance(sfData.name());
    if (sub == nullptr)
    {
      dst.Error("Ignoring an environmental conditions ambient gas that was not found : " + sfData.name());
      continue;
    }
    if (sub->GetState() != eSubstance_State::Gas)
    {
      dst.Error("Ignoring an environmental conditions ambient gas that is not a gas : " + sfData.name());
      continue;
    }
    PBSubstance::Load(sfData, dst.GetAmbientGas(*sub));
  }

  for (int i = 0; i < src.ambientaerosol_size(); i++)
  {
    const CDM_BIND::SubstanceConcentrationData& scData = src.ambientaerosol()[i];
    sub = subMgr.GetSubstance(scData.name());
    if (sub == nullptr)
    {
      dst.Error("Ignoring an environmental conditions ambient aerosol that was not found : " + scData.name());
      continue;
    }
    if (sub->GetState() != eSubstance_State::Liquid && sub->GetState() != eSubstance_State::Solid)
    {
      dst.Error("Ignoring an environmental conditions ambient aerosol that is not a gas : " + scData.name());
      continue;
    }
    PBSubstance::Load(scData, dst.GetAmbientAerosol(*sub));
  }
}
CDM_BIND::EnvironmentalConditionsData* PBEnvironment::Unload(const SEEnvironmentalConditions& src)
{
  CDM_BIND::EnvironmentalConditionsData* dst = new CDM_BIND::EnvironmentalConditionsData();
  PBEnvironment::Serialize(src, *dst);
  return dst;
}
void PBEnvironment::Serialize(const SEEnvironmentalConditions& src, CDM_BIND::EnvironmentalConditionsData& dst)
{
  dst.set_surroundingtype((CDM_BIND::EnvironmentalConditionsData::eSurroundingType)src.m_SurroundingType);
  if (src.HasAirDensity())
    dst.set_allocated_airdensity(PBProperty::Unload(*src.m_AirDensity));
  if (src.HasAirVelocity())
    dst.set_allocated_airvelocity(PBProperty::Unload(*src.m_AirVelocity));
  if (src.HasAmbientTemperature())
    dst.set_allocated_ambienttemperature(PBProperty::Unload(*src.m_AmbientTemperature));
  if (src.HasAtmosphericPressure())
    dst.set_allocated_atmosphericpressure(PBProperty::Unload(*src.m_AtmosphericPressure));
  if (src.HasClothingResistance())
    dst.set_allocated_clothingresistance(PBProperty::Unload(*src.m_ClothingResistance));
  if (src.HasEmissivity())
    dst.set_allocated_emissivity(PBProperty::Unload(*src.m_Emissivity));
  if (src.HasMeanRadiantTemperature())
    dst.set_allocated_meanradianttemperature(PBProperty::Unload(*src.m_MeanRadiantTemperature));
  if (src.HasMechanicalDeadSpace())
    dst.set_allocated_mechanicaldeadspace(PBProperty::Unload(*src.m_MechanicalDeadSpace));
  if (src.HasRelativeHumidity())
    dst.set_allocated_relativehumidity(PBProperty::Unload(*src.m_RelativeHumidity));
  if (src.HasRespirationAmbientTemperature())
    dst.set_allocated_respirationambienttemperature(PBProperty::Unload(*src.m_RespirationAmbientTemperature));

  for (SESubstanceFraction *sf : src.m_AmbientGases)
    dst.mutable_ambientgas()->AddAllocated(PBSubstance::Unload(*sf));

  for (SESubstanceConcentration *sc : src.m_AmbientAerosols)
    dst.mutable_ambientaerosol()->AddAllocated(PBSubstance::Unload(*sc));
}
void PBEnvironment::Copy(const SEEnvironmentalConditions& src, SEEnvironmentalConditions& dst, const SESubstanceManager& subMgr)
{
  dst.Clear();
  CDM_BIND::EnvironmentalConditionsData data;
  PBEnvironment::Serialize(src, data);
  PBEnvironment::Serialize(data, dst, subMgr);
}

void PBEnvironment::Load(const CDM_BIND::AppliedTemperatureData& src, SEAppliedTemperature& dst)
{
  dst.Clear();
  PBEnvironment::Serialize(src, dst);
}
void PBEnvironment::Serialize(const CDM_BIND::AppliedTemperatureData& src, SEAppliedTemperature& dst)
{
  if (src.state() != CDM_BIND::eSwitch::NullSwitch)
    dst.SetState((eSwitch)src.state());
  if (src.has_temperature())
    PBProperty::Load(src.temperature(), dst.GetTemperature());
  if (src.has_surfacearea())
    PBProperty::Load(src.surfacearea(), dst.GetSurfaceArea());
  if (src.has_surfaceareafraction())
    PBProperty::Load(src.surfaceareafraction(), dst.GetSurfaceAreaFraction());
}
CDM_BIND::AppliedTemperatureData* PBEnvironment::Unload(const SEAppliedTemperature& src)
{
  CDM_BIND::AppliedTemperatureData* dst = new CDM_BIND::AppliedTemperatureData();
  PBEnvironment::Serialize(src, *dst);
  return dst;
}
void PBEnvironment::Serialize(const SEAppliedTemperature& src, CDM_BIND::AppliedTemperatureData& dst)
{
  dst.set_state((CDM_BIND::eSwitch)src.m_State);
  if (src.HasTemperature())
    dst.set_allocated_temperature(PBProperty::Unload(*src.m_Temperature));
  if (src.HasSurfaceArea())
    dst.set_allocated_surfacearea(PBProperty::Unload(*src.m_SurfaceArea));
  if (src.HasSurfaceAreaFraction())
    dst.set_allocated_surfaceareafraction(PBProperty::Unload(*src.m_SurfaceAreaFraction));
}
void PBEnvironment::Copy(const SEAppliedTemperature& src, SEAppliedTemperature& dst)
{
  dst.Clear();
  CDM_BIND::AppliedTemperatureData data;
  PBEnvironment::Serialize(src, data);
  PBEnvironment::Serialize(data, dst);
}

void PBEnvironment::Load(const CDM_BIND::ActiveConditioningData& src, SEActiveConditioning& dst)
{
  dst.Clear();
  PBEnvironment::Serialize(src, dst);
}
void PBEnvironment::Serialize(const CDM_BIND::ActiveConditioningData& src, SEActiveConditioning& dst)
{
  if (src.has_power())
    PBProperty::Load(src.power(), dst.GetPower());
  if (src.has_surfacearea())
    PBProperty::Load(src.surfacearea(), dst.GetSurfaceArea());
  if (src.has_surfaceareafraction())
    PBProperty::Load(src.surfaceareafraction(), dst.GetSurfaceAreaFraction());
}
CDM_BIND::ActiveConditioningData* PBEnvironment::Unload(const SEActiveConditioning& src)
{
  CDM_BIND::ActiveConditioningData* dst = new CDM_BIND::ActiveConditioningData();
  PBEnvironment::Serialize(src, *dst);
  return dst;
}
void PBEnvironment::Serialize(const SEActiveConditioning& src, CDM_BIND::ActiveConditioningData& dst)
{
  if (src.HasPower())
    dst.set_allocated_power(PBProperty::Unload(*src.m_Power));
  if (src.HasSurfaceArea())
    dst.set_allocated_surfacearea(PBProperty::Unload(*src.m_SurfaceArea));
  if (src.HasSurfaceAreaFraction())
    dst.set_allocated_surfaceareafraction(PBProperty::Unload(*src.m_SurfaceAreaFraction));
}
void PBEnvironment::Copy(const SEActiveConditioning& src, SEActiveConditioning& dst)
{
  dst.Clear();
  CDM_BIND::ActiveConditioningData data;
  PBEnvironment::Serialize(src, data);
  PBEnvironment::Serialize(data, dst);
}

bool PBEnvironment::SerializeToString(const SEEnvironmentalConditions& src, std::string& output, eSerializationFormat m)
{
  CDM_BIND::EnvironmentalConditionsData data;
  PBEnvironment::Serialize(src, data);
  return PBUtils::SerializeToString(data, output, m, src.GetLogger());
}
bool PBEnvironment::SerializeToFile(const SEEnvironmentalConditions& src, const std::string& filename)
{
  CDM_BIND::EnvironmentalConditionsData data;
  PBEnvironment::Serialize(src, data);
  return PBUtils::SerializeToFile(data, filename, src.GetLogger());
}

bool PBEnvironment::SerializeFromString(const std::string& src, SEEnvironmentalConditions& dst, eSerializationFormat m, const SESubstanceManager& subMgr)
{
  CDM_BIND::EnvironmentalConditionsData data;
  if (!PBUtils::SerializeFromString(src, data, m, dst.GetLogger()))
    return false;
  PBEnvironment::Load(data, dst, subMgr);
  return true;
}
bool PBEnvironment::SerializeFromFile(const std::string& filename, SEEnvironmentalConditions& dst, const SESubstanceManager& subMgr)
{
  CDM_BIND::EnvironmentalConditionsData data;
  if (!PBUtils::SerializeFromFile(filename, data, dst.GetLogger()))
    return false;
  PBEnvironment::Load(data, dst, subMgr);
  return true;
}