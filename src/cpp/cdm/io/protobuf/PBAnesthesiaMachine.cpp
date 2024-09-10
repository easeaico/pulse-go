/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
PUSH_PROTO_WARNINGS
#include "pulse/cdm/bind/AnesthesiaMachine.pb.h"
POP_PROTO_WARNINGS
#include "cdm/io/protobuf/PBAnesthesiaMachine.h"
#include "cdm/io/protobuf/PBProperties.h"
#include "cdm/io/protobuf/PBUtils.h"
#include "cdm/system/equipment/anesthesia_machine/SEAnesthesiaMachine.h"
#include "cdm/system/equipment/anesthesia_machine/SEAnesthesiaMachineChamber.h"
#include "cdm/system/equipment/anesthesia_machine/SEAnesthesiaMachineOxygenBottle.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/utils/FileUtils.h"


void PBAnesthesiaMachine::Load(const CDM_BIND::AnesthesiaMachineData& src, SEAnesthesiaMachine& dst, const SESubstanceManager& subMgr)
{
  dst.Clear();
  PBAnesthesiaMachine::Serialize(src, dst, subMgr);
  dst.StateChange();
}
void PBAnesthesiaMachine::Serialize(const CDM_BIND::AnesthesiaMachineData& src, SEAnesthesiaMachine& dst, const SESubstanceManager& subMgr)
{
  dst.m_Connection = (eSwitch)src.connection();

  if (src.has_inletflow())
    PBProperty::Load(src.inletflow(), dst.GetInletFlow());
  if (src.has_inspiratoryexpiratoryratio())
    PBProperty::Load(src.inspiratoryexpiratoryratio(), dst.GetInspiratoryExpiratoryRatio());
  if (src.has_oxygenfraction())
    PBProperty::Load(src.oxygenfraction(), dst.GetOxygenFraction());

  dst.SetOxygenSource((eAnesthesiaMachine_OxygenSource)src.oxygensource());
  if (src.has_peakinspiratorypressure())
    PBProperty::Load(src.peakinspiratorypressure(), dst.GetPeakInspiratoryPressure());
  if (src.has_positiveendexpiratorypressure())
    PBProperty::Load(src.positiveendexpiratorypressure(), dst.GetPositiveEndExpiratoryPressure());
  dst.SetPrimaryGas((eAnesthesiaMachine_PrimaryGas)src.primarygas());

  if (src.has_respiratoryrate())
    PBProperty::Load(src.respiratoryrate(), dst.GetRespiratoryRate());
  if (src.has_reliefvalvepressure())
    PBProperty::Load(src.reliefvalvepressure(), dst.GetReliefValvePressure());
  if (src.has_leftchamber())
    PBAnesthesiaMachine::Load(src.leftchamber(), dst.GetLeftChamber(), subMgr);
  if (src.has_rightchamber())
    PBAnesthesiaMachine::Load(src.rightchamber(), dst.GetRightChamber(), subMgr);
  if (src.has_oxygenbottleone())
    PBAnesthesiaMachine::Load(src.oxygenbottleone(), dst.GetOxygenBottleOne());
  if (src.has_oxygenbottletwo())
    PBAnesthesiaMachine::Load(src.oxygenbottletwo(), dst.GetOxygenBottleTwo());
}

CDM_BIND::AnesthesiaMachineData* PBAnesthesiaMachine::Unload(const SEAnesthesiaMachine& src)
{
  CDM_BIND::AnesthesiaMachineData* dst = new CDM_BIND::AnesthesiaMachineData();
  PBAnesthesiaMachine::Serialize(src, *dst);
  return dst;
}
void PBAnesthesiaMachine::Serialize(const SEAnesthesiaMachine& src, CDM_BIND::AnesthesiaMachineData& dst)
{
  dst.set_connection((CDM_BIND::eSwitch)src.m_Connection);
  if (src.HasInletFlow())
    dst.set_allocated_inletflow(PBProperty::Unload(*src.m_InletFlow));
  if (src.HasInspiratoryExpiratoryRatio())
    dst.set_allocated_inspiratoryexpiratoryratio(PBProperty::Unload(*src.m_InspiratoryExpiratoryRatio));
  if (src.HasOxygenFraction())
    dst.set_allocated_oxygenfraction(PBProperty::Unload(*src.m_OxygenFraction));

  dst.set_oxygensource((CDM_BIND::AnesthesiaMachineData::eOxygenSource)src.m_OxygenSource);
  if (src.HasPeakInspiratoryPressure())
    dst.set_allocated_peakinspiratorypressure(PBProperty::Unload(*src.m_PeakInspiratoryPressure));
  if (src.HasPositiveEndExpiratoryPressure())
    dst.set_allocated_positiveendexpiratorypressure(PBProperty::Unload(*src.m_PositiveEndExpiratoryPressure));
  dst.set_primarygas((CDM_BIND::AnesthesiaMachineData::ePrimaryGas)src.m_PrimaryGas);

  if (src.HasRespiratoryRate())
    dst.set_allocated_respiratoryrate(PBProperty::Unload(*src.m_RespiratoryRate));
  if (src.HasReliefValvePressure())
    dst.set_allocated_reliefvalvepressure(PBProperty::Unload(*src.m_ReliefValvePressure));
  if (src.HasLeftChamber())
    dst.set_allocated_leftchamber(PBAnesthesiaMachine::Unload(*src.m_LeftChamber));
  if (src.HasRightChamber())
    dst.set_allocated_rightchamber(PBAnesthesiaMachine::Unload(*src.m_RightChamber));
  if (src.HasOxygenBottleOne())
    dst.set_allocated_oxygenbottleone(PBAnesthesiaMachine::Unload(*src.m_OxygenBottleOne));
  if (src.HasOxygenBottleTwo())
    dst.set_allocated_oxygenbottletwo(PBAnesthesiaMachine::Unload(*src.m_OxygenBottleTwo));
}

void PBAnesthesiaMachine::Load(const CDM_BIND::AnesthesiaMachineChamberData& src, SEAnesthesiaMachineChamber& dst, const SESubstanceManager& subMgr)
{
  dst.Clear();
  PBAnesthesiaMachine::Serialize(src, dst, subMgr);
}
void PBAnesthesiaMachine::Serialize(const CDM_BIND::AnesthesiaMachineChamberData& src, SEAnesthesiaMachineChamber& dst, const SESubstanceManager& subMgr)
{
  if (src.state() != CDM_BIND::eSwitch::NullSwitch)
    dst.SetState((eSwitch)src.state());
  if (src.has_substancefraction())
    PBProperty::Load(src.substancefraction(), dst.GetSubstanceFraction());

  if (!src.substance().empty())
  {
    dst.m_Substance = subMgr.GetSubstance(src.substance());
    if (dst.m_Substance == nullptr)
    {
      dst.Error("Do not have substance : " + src.substance());
    }
  }
}

CDM_BIND::AnesthesiaMachineChamberData* PBAnesthesiaMachine::Unload(const SEAnesthesiaMachineChamber& src)
{
  CDM_BIND::AnesthesiaMachineChamberData* dst = new CDM_BIND::AnesthesiaMachineChamberData();
  PBAnesthesiaMachine::Serialize(src, *dst);
  return dst;
}
void PBAnesthesiaMachine::Serialize(const SEAnesthesiaMachineChamber& src, CDM_BIND::AnesthesiaMachineChamberData& dst)
{
  dst.set_state((CDM_BIND::eSwitch)src.m_State);
  if (src.HasSubstanceFraction())
    dst.set_allocated_substancefraction(PBProperty::Unload(*src.m_SubstanceFraction));
  if(src.HasSubstance())
    dst.set_substance(src.m_Substance->GetName());
}

void PBAnesthesiaMachine::Load(const CDM_BIND::AnesthesiaMachineOxygenBottleData& src, SEAnesthesiaMachineOxygenBottle& dst)
{
  dst.Clear();
  PBAnesthesiaMachine::Serialize(src, dst);
}
void PBAnesthesiaMachine::Serialize(const CDM_BIND::AnesthesiaMachineOxygenBottleData& src, SEAnesthesiaMachineOxygenBottle& dst)
{
  if (src.has_volume())
    PBProperty::Load(src.volume(), dst.GetVolume());
}

CDM_BIND::AnesthesiaMachineOxygenBottleData* PBAnesthesiaMachine::Unload(const SEAnesthesiaMachineOxygenBottle& src)
{
  CDM_BIND::AnesthesiaMachineOxygenBottleData* dst = new CDM_BIND::AnesthesiaMachineOxygenBottleData();
  PBAnesthesiaMachine::Serialize(src, *dst);
  return dst;
}
void PBAnesthesiaMachine::Serialize(const SEAnesthesiaMachineOxygenBottle& src, CDM_BIND::AnesthesiaMachineOxygenBottleData& dst)
{
  if (src.HasVolume())
    dst.set_allocated_volume(PBProperty::Unload(*src.m_Volume));
}

bool PBAnesthesiaMachine::SerializeToString(const SEAnesthesiaMachine& src, std::string& output, eSerializationFormat m)
{
  CDM_BIND::AnesthesiaMachineData data;
  PBAnesthesiaMachine::Serialize(src, data);
  return PBUtils::SerializeToString(data, output, m, src.GetLogger());
}
bool PBAnesthesiaMachine::SerializeToFile(const SEAnesthesiaMachine& src, const std::string& filename)
{
  CDM_BIND::AnesthesiaMachineData data;
  PBAnesthesiaMachine::Serialize(src, data);
  return PBUtils::SerializeToFile(data, filename, src.GetLogger());
}

bool PBAnesthesiaMachine::SerializeFromString(const std::string& src, SEAnesthesiaMachine& dst, eSerializationFormat m, const SESubstanceManager& subMgr)
{
  CDM_BIND::AnesthesiaMachineData data;
  if (!PBUtils::SerializeFromString(src, data, m, dst.GetLogger()))
    return false;
  PBAnesthesiaMachine::Load(data, dst, subMgr);
  return true;
}
bool PBAnesthesiaMachine::SerializeFromFile(const std::string& filename, SEAnesthesiaMachine& dst, const SESubstanceManager& subMgr)
{
  CDM_BIND::AnesthesiaMachineData data;
  if (!PBUtils::SerializeFromFile(filename, data, dst.GetLogger()))
    return false;
  PBAnesthesiaMachine::Load(data, dst, subMgr);
  return true;
}
