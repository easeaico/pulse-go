/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
PUSH_PROTO_WARNINGS
#include "pulse/cdm/bind/EquipmentActions.pb.h"
#include "pulse/cdm/bind/AnesthesiaMachineActions.pb.h"
#include "pulse/cdm/bind/InhalerActions.pb.h"
#include "pulse/cdm/bind/MechanicalVentilatorActions.pb.h"
POP_PROTO_WARNINGS
#include "cdm/io/protobuf/PBEquipmentActions.h"
#include "cdm/io/protobuf/PBActions.h"
#include "cdm/io/protobuf/PBProperties.h"
#include "cdm/io/protobuf/PBAnesthesiaMachine.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineAction.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineConfiguration.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineOxygenWallPortPressureLoss.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineOxygenTankPressureLoss.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineExpiratoryValveLeak.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineExpiratoryValveObstruction.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineInspiratoryValveLeak.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineInspiratoryValveObstruction.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineMaskLeak.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineSodaLimeFailure.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineTubeCuffLeak.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineVaporizerFailure.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineVentilatorPressureLoss.h"
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineYPieceDisconnect.h"
#include "cdm/io/protobuf/PBBagValveMask.h"
#include "cdm/system/equipment/bag_valve_mask/actions/SEBagValveMaskAction.h"
#include "cdm/system/equipment/bag_valve_mask/actions/SEBagValveMaskConfiguration.h"
#include "cdm/system/equipment/bag_valve_mask/actions/SEBagValveMaskAutomated.h"
#include "cdm/system/equipment/bag_valve_mask/actions/SEBagValveMaskSqueeze.h"
#include "cdm/system/equipment/bag_valve_mask/actions/SEBagValveMaskInstantaneous.h"
#include "cdm/io/protobuf/PBECMO.h"
#include "cdm/system/equipment/ecmo/actions/SEECMOAction.h"
#include "cdm/system/equipment/ecmo/actions/SEECMOConfiguration.h"
#include "cdm/io/protobuf/PBInhaler.h"
#include "cdm/system/equipment/inhaler/actions/SEInhalerAction.h"
#include "cdm/system/equipment/inhaler/actions/SEInhalerConfiguration.h"
#include "cdm/io/protobuf/PBMechanicalVentilator.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorAction.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorConfiguration.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorHold.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorLeak.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorContinuousPositiveAirwayPressure.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorPressureControl.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorVolumeControl.h"
#include "cdm/substance/SESubstanceManager.h"

void PBEquipmentAction::Serialize(const CDM_BIND::EquipmentActionData& src, SEEquipmentAction& dst)
{
  PBAction::Serialize(src.action(), dst);
}
void PBEquipmentAction::Serialize(const SEEquipmentAction& src, CDM_BIND::EquipmentActionData& dst)
{
  PBAction::Serialize(src, *dst.mutable_action());
}

void PBEquipmentAction::Serialize(const CDM_BIND::AnesthesiaMachineActionData& src, SEAnesthesiaMachineAction& dst)
{
  PBEquipmentAction::Serialize(src.equipmentaction(), dst);
}
void PBEquipmentAction::Serialize(const SEAnesthesiaMachineAction& src, CDM_BIND::AnesthesiaMachineActionData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_equipmentaction());
}

void PBEquipmentAction::Load(const CDM_BIND::AnesthesiaMachineConfigurationData& src, SEAnesthesiaMachineConfiguration& dst, const SESubstanceManager& subMgr)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst, subMgr);
}
void PBEquipmentAction::Serialize(const CDM_BIND::AnesthesiaMachineConfigurationData& src, SEAnesthesiaMachineConfiguration& dst, const SESubstanceManager& subMgr)
{
  PBEquipmentAction::Serialize(src.anesthesiamachineaction(), dst);
  if (!src.configurationfile().empty())
    dst.SetConfigurationFile(src.configurationfile());
  else if (src.has_configuration())
    PBAnesthesiaMachine::Load(src.configuration(), dst.GetConfiguration(), subMgr);
  dst.SetMergeType((eMergeType)src.mergetype());
}
CDM_BIND::AnesthesiaMachineConfigurationData* PBEquipmentAction::Unload(const SEAnesthesiaMachineConfiguration& src)
{
  CDM_BIND::AnesthesiaMachineConfigurationData* dst = new CDM_BIND::AnesthesiaMachineConfigurationData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEAnesthesiaMachineConfiguration& src, CDM_BIND::AnesthesiaMachineConfigurationData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_anesthesiamachineaction());
  if (src.HasConfigurationFile())
    dst.set_configurationfile(src.m_ConfigurationFile); 
  else if (src.HasConfiguration())
    dst.set_allocated_configuration(PBAnesthesiaMachine::Unload(*src.m_Configuration));
  dst.set_mergetype((CDM_BIND::eMergeType)src.m_MergeType);
}
void PBEquipmentAction::Copy(const SEAnesthesiaMachineConfiguration& src, SEAnesthesiaMachineConfiguration& dst, const SESubstanceManager& subMgr)
{
  dst.Clear();
  CDM_BIND::AnesthesiaMachineConfigurationData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst, subMgr);
}

void PBEquipmentAction::Load(const CDM_BIND::AnesthesiaMachineExpiratoryValveLeakData& src, SEAnesthesiaMachineExpiratoryValveLeak& dst)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst);
}
void PBEquipmentAction::Serialize(const CDM_BIND::AnesthesiaMachineExpiratoryValveLeakData& src, SEAnesthesiaMachineExpiratoryValveLeak& dst)
{
  PBEquipmentAction::Serialize(src.anesthesiamachineaction(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
}
CDM_BIND::AnesthesiaMachineExpiratoryValveLeakData* PBEquipmentAction::Unload(const SEAnesthesiaMachineExpiratoryValveLeak& src)
{
  CDM_BIND::AnesthesiaMachineExpiratoryValveLeakData* dst = new CDM_BIND::AnesthesiaMachineExpiratoryValveLeakData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEAnesthesiaMachineExpiratoryValveLeak& src, CDM_BIND::AnesthesiaMachineExpiratoryValveLeakData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_anesthesiamachineaction());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
}
void PBEquipmentAction::Copy(const SEAnesthesiaMachineExpiratoryValveLeak& src, SEAnesthesiaMachineExpiratoryValveLeak& dst)
{
  dst.Clear();
  CDM_BIND::AnesthesiaMachineExpiratoryValveLeakData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst);
}

void PBEquipmentAction::Load(const CDM_BIND::AnesthesiaMachineExpiratoryValveObstructionData& src, SEAnesthesiaMachineExpiratoryValveObstruction& dst)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst);
}
void PBEquipmentAction::Serialize(const CDM_BIND::AnesthesiaMachineExpiratoryValveObstructionData& src, SEAnesthesiaMachineExpiratoryValveObstruction& dst)
{
  PBEquipmentAction::Serialize(src.anesthesiamachineaction(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
}
CDM_BIND::AnesthesiaMachineExpiratoryValveObstructionData* PBEquipmentAction::Unload(const SEAnesthesiaMachineExpiratoryValveObstruction& src)
{
  CDM_BIND::AnesthesiaMachineExpiratoryValveObstructionData* dst = new CDM_BIND::AnesthesiaMachineExpiratoryValveObstructionData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEAnesthesiaMachineExpiratoryValveObstruction& src, CDM_BIND::AnesthesiaMachineExpiratoryValveObstructionData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_anesthesiamachineaction());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
}
void PBEquipmentAction::Copy(const SEAnesthesiaMachineExpiratoryValveObstruction& src, SEAnesthesiaMachineExpiratoryValveObstruction& dst)
{
  dst.Clear();
  CDM_BIND::AnesthesiaMachineExpiratoryValveObstructionData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst);
}

void PBEquipmentAction::Load(const CDM_BIND::AnesthesiaMachineInspiratoryValveLeakData& src, SEAnesthesiaMachineInspiratoryValveLeak& dst)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst);
}
void PBEquipmentAction::Serialize(const CDM_BIND::AnesthesiaMachineInspiratoryValveLeakData& src, SEAnesthesiaMachineInspiratoryValveLeak& dst)
{
  PBEquipmentAction::Serialize(src.anesthesiamachineaction(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
}
CDM_BIND::AnesthesiaMachineInspiratoryValveLeakData* PBEquipmentAction::Unload(const SEAnesthesiaMachineInspiratoryValveLeak& src)
{
  CDM_BIND::AnesthesiaMachineInspiratoryValveLeakData* dst = new CDM_BIND::AnesthesiaMachineInspiratoryValveLeakData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEAnesthesiaMachineInspiratoryValveLeak& src, CDM_BIND::AnesthesiaMachineInspiratoryValveLeakData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_anesthesiamachineaction());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
}
void PBEquipmentAction::Copy(const SEAnesthesiaMachineInspiratoryValveLeak& src, SEAnesthesiaMachineInspiratoryValveLeak& dst)
{
  dst.Clear();
  CDM_BIND::AnesthesiaMachineInspiratoryValveLeakData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst);
}

void PBEquipmentAction::Load(const CDM_BIND::AnesthesiaMachineInspiratoryValveObstructionData& src, SEAnesthesiaMachineInspiratoryValveObstruction& dst)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst);
}
void PBEquipmentAction::Serialize(const CDM_BIND::AnesthesiaMachineInspiratoryValveObstructionData& src, SEAnesthesiaMachineInspiratoryValveObstruction& dst)
{
  PBEquipmentAction::Serialize(src.anesthesiamachineaction(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
}
CDM_BIND::AnesthesiaMachineInspiratoryValveObstructionData* PBEquipmentAction::Unload(const SEAnesthesiaMachineInspiratoryValveObstruction& src)
{
  CDM_BIND::AnesthesiaMachineInspiratoryValveObstructionData* dst = new CDM_BIND::AnesthesiaMachineInspiratoryValveObstructionData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEAnesthesiaMachineInspiratoryValveObstruction& src, CDM_BIND::AnesthesiaMachineInspiratoryValveObstructionData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_anesthesiamachineaction());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
}
void PBEquipmentAction::Copy(const SEAnesthesiaMachineInspiratoryValveObstruction& src, SEAnesthesiaMachineInspiratoryValveObstruction& dst)
{
  dst.Clear();
  CDM_BIND::AnesthesiaMachineInspiratoryValveObstructionData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst);
}

void PBEquipmentAction::Load(const CDM_BIND::AnesthesiaMachineMaskLeakData& src, SEAnesthesiaMachineMaskLeak& dst)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst);
}
void PBEquipmentAction::Serialize(const CDM_BIND::AnesthesiaMachineMaskLeakData& src, SEAnesthesiaMachineMaskLeak& dst)
{
  PBEquipmentAction::Serialize(src.anesthesiamachineaction(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
}
CDM_BIND::AnesthesiaMachineMaskLeakData* PBEquipmentAction::Unload(const SEAnesthesiaMachineMaskLeak& src)
{
  CDM_BIND::AnesthesiaMachineMaskLeakData* dst = new CDM_BIND::AnesthesiaMachineMaskLeakData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEAnesthesiaMachineMaskLeak& src, CDM_BIND::AnesthesiaMachineMaskLeakData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_anesthesiamachineaction());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
}
void PBEquipmentAction::Copy(const SEAnesthesiaMachineMaskLeak& src, SEAnesthesiaMachineMaskLeak& dst)
{
  dst.Clear();
  CDM_BIND::AnesthesiaMachineMaskLeakData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst);
}

void PBEquipmentAction::Load(const CDM_BIND::AnesthesiaMachineOxygenTankPressureLossData& src, SEAnesthesiaMachineOxygenTankPressureLoss& dst)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst);
}
void PBEquipmentAction::Serialize(const CDM_BIND::AnesthesiaMachineOxygenTankPressureLossData& src, SEAnesthesiaMachineOxygenTankPressureLoss& dst)
{
  PBEquipmentAction::Serialize(src.anesthesiamachineaction(), dst);
  dst.SetState(src.state() == CDM_BIND::eSwitch::On ? eSwitch::On : eSwitch::Off);
}
CDM_BIND::AnesthesiaMachineOxygenTankPressureLossData* PBEquipmentAction::Unload(const SEAnesthesiaMachineOxygenTankPressureLoss& src)
{
  CDM_BIND::AnesthesiaMachineOxygenTankPressureLossData* dst = new CDM_BIND::AnesthesiaMachineOxygenTankPressureLossData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEAnesthesiaMachineOxygenTankPressureLoss& src, CDM_BIND::AnesthesiaMachineOxygenTankPressureLossData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_anesthesiamachineaction());
  dst.set_state(src.GetState()==eSwitch::On ? CDM_BIND::eSwitch::On : CDM_BIND::eSwitch::Off);
}
void PBEquipmentAction::Copy(const SEAnesthesiaMachineOxygenTankPressureLoss& src, SEAnesthesiaMachineOxygenTankPressureLoss& dst)
{
  dst.Clear();
  CDM_BIND::AnesthesiaMachineOxygenTankPressureLossData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst);
}

void PBEquipmentAction::Load(const CDM_BIND::AnesthesiaMachineOxygenWallPortPressureLossData& src, SEAnesthesiaMachineOxygenWallPortPressureLoss& dst)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst);
}
void PBEquipmentAction::Serialize(const CDM_BIND::AnesthesiaMachineOxygenWallPortPressureLossData& src, SEAnesthesiaMachineOxygenWallPortPressureLoss& dst)
{
  PBEquipmentAction::Serialize(src.anesthesiamachineaction(), dst);
  dst.SetState(src.state() == CDM_BIND::eSwitch::On ? eSwitch::On : eSwitch::Off);
}
CDM_BIND::AnesthesiaMachineOxygenWallPortPressureLossData* PBEquipmentAction::Unload(const SEAnesthesiaMachineOxygenWallPortPressureLoss& src)
{
  CDM_BIND::AnesthesiaMachineOxygenWallPortPressureLossData* dst = new CDM_BIND::AnesthesiaMachineOxygenWallPortPressureLossData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEAnesthesiaMachineOxygenWallPortPressureLoss& src, CDM_BIND::AnesthesiaMachineOxygenWallPortPressureLossData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_anesthesiamachineaction());
  dst.set_state(src.GetState()==eSwitch::On ? CDM_BIND::eSwitch::On : CDM_BIND::eSwitch::Off);
}
void PBEquipmentAction::Copy(const SEAnesthesiaMachineOxygenWallPortPressureLoss& src, SEAnesthesiaMachineOxygenWallPortPressureLoss& dst)
{
  dst.Clear();
  CDM_BIND::AnesthesiaMachineOxygenWallPortPressureLossData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst);
}

void PBEquipmentAction::Load(const CDM_BIND::AnesthesiaMachineSodaLimeFailureData& src, SEAnesthesiaMachineSodaLimeFailure& dst)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst);
}
void PBEquipmentAction::Serialize(const CDM_BIND::AnesthesiaMachineSodaLimeFailureData& src, SEAnesthesiaMachineSodaLimeFailure& dst)
{
  PBEquipmentAction::Serialize(src.anesthesiamachineaction(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
}
CDM_BIND::AnesthesiaMachineSodaLimeFailureData* PBEquipmentAction::Unload(const SEAnesthesiaMachineSodaLimeFailure& src)
{
  CDM_BIND::AnesthesiaMachineSodaLimeFailureData* dst = new CDM_BIND::AnesthesiaMachineSodaLimeFailureData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEAnesthesiaMachineSodaLimeFailure& src, CDM_BIND::AnesthesiaMachineSodaLimeFailureData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_anesthesiamachineaction());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
}
void PBEquipmentAction::Copy(const SEAnesthesiaMachineSodaLimeFailure& src, SEAnesthesiaMachineSodaLimeFailure& dst)
{
  dst.Clear();
  CDM_BIND::AnesthesiaMachineSodaLimeFailureData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst);
}

void PBEquipmentAction::Load(const CDM_BIND::AnesthesiaMachineTubeCuffLeakData& src, SEAnesthesiaMachineTubeCuffLeak& dst)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst);
}
void PBEquipmentAction::Serialize(const CDM_BIND::AnesthesiaMachineTubeCuffLeakData& src, SEAnesthesiaMachineTubeCuffLeak& dst)
{
  PBEquipmentAction::Serialize(src.anesthesiamachineaction(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
}
CDM_BIND::AnesthesiaMachineTubeCuffLeakData* PBEquipmentAction::Unload(const SEAnesthesiaMachineTubeCuffLeak& src)
{
  CDM_BIND::AnesthesiaMachineTubeCuffLeakData* dst = new CDM_BIND::AnesthesiaMachineTubeCuffLeakData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEAnesthesiaMachineTubeCuffLeak& src, CDM_BIND::AnesthesiaMachineTubeCuffLeakData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_anesthesiamachineaction());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
}
void PBEquipmentAction::Copy(const SEAnesthesiaMachineTubeCuffLeak& src, SEAnesthesiaMachineTubeCuffLeak& dst)
{
  dst.Clear();
  CDM_BIND::AnesthesiaMachineTubeCuffLeakData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst);
}

void PBEquipmentAction::Load(const CDM_BIND::AnesthesiaMachineVaporizerFailureData& src, SEAnesthesiaMachineVaporizerFailure& dst)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst);
}
void PBEquipmentAction::Serialize(const CDM_BIND::AnesthesiaMachineVaporizerFailureData& src, SEAnesthesiaMachineVaporizerFailure& dst)
{
  PBEquipmentAction::Serialize(src.anesthesiamachineaction(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
}
CDM_BIND::AnesthesiaMachineVaporizerFailureData* PBEquipmentAction::Unload(const SEAnesthesiaMachineVaporizerFailure& src)
{
  CDM_BIND::AnesthesiaMachineVaporizerFailureData* dst = new CDM_BIND::AnesthesiaMachineVaporizerFailureData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEAnesthesiaMachineVaporizerFailure& src, CDM_BIND::AnesthesiaMachineVaporizerFailureData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_anesthesiamachineaction());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
}
void PBEquipmentAction::Copy(const SEAnesthesiaMachineVaporizerFailure& src, SEAnesthesiaMachineVaporizerFailure& dst)
{
  dst.Clear();
  CDM_BIND::AnesthesiaMachineVaporizerFailureData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst);
}

void PBEquipmentAction::Load(const CDM_BIND::AnesthesiaMachineVentilatorPressureLossData& src, SEAnesthesiaMachineVentilatorPressureLoss& dst)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst);
}
void PBEquipmentAction::Serialize(const CDM_BIND::AnesthesiaMachineVentilatorPressureLossData& src, SEAnesthesiaMachineVentilatorPressureLoss& dst)
{
  PBEquipmentAction::Serialize(src.anesthesiamachineaction(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
}
CDM_BIND::AnesthesiaMachineVentilatorPressureLossData* PBEquipmentAction::Unload(const SEAnesthesiaMachineVentilatorPressureLoss& src)
{
  CDM_BIND::AnesthesiaMachineVentilatorPressureLossData* dst = new CDM_BIND::AnesthesiaMachineVentilatorPressureLossData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEAnesthesiaMachineVentilatorPressureLoss& src, CDM_BIND::AnesthesiaMachineVentilatorPressureLossData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_anesthesiamachineaction());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
}
void PBEquipmentAction::Copy(const SEAnesthesiaMachineVentilatorPressureLoss& src, SEAnesthesiaMachineVentilatorPressureLoss& dst)
{
  dst.Clear();
  CDM_BIND::AnesthesiaMachineVentilatorPressureLossData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst);
}

void PBEquipmentAction::Load(const CDM_BIND::AnesthesiaMachineYPieceDisconnectData& src, SEAnesthesiaMachineYPieceDisconnect& dst)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst);
}
void PBEquipmentAction::Serialize(const CDM_BIND::AnesthesiaMachineYPieceDisconnectData& src, SEAnesthesiaMachineYPieceDisconnect& dst)
{
  PBEquipmentAction::Serialize(src.anesthesiamachineaction(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
}
CDM_BIND::AnesthesiaMachineYPieceDisconnectData* PBEquipmentAction::Unload(const SEAnesthesiaMachineYPieceDisconnect& src)
{
  CDM_BIND::AnesthesiaMachineYPieceDisconnectData* dst = new CDM_BIND::AnesthesiaMachineYPieceDisconnectData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEAnesthesiaMachineYPieceDisconnect& src, CDM_BIND::AnesthesiaMachineYPieceDisconnectData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_anesthesiamachineaction());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
}
void PBEquipmentAction::Copy(const SEAnesthesiaMachineYPieceDisconnect& src, SEAnesthesiaMachineYPieceDisconnect& dst)
{
  dst.Clear();
  CDM_BIND::AnesthesiaMachineYPieceDisconnectData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst);
}

////////////////////
// Bag Valve Mask //
////////////////////

void PBEquipmentAction::Serialize(const CDM_BIND::BagValveMaskActionData& src, SEBagValveMaskAction& dst)
{
  PBEquipmentAction::Serialize(src.equipmentaction(), dst);
}
void PBEquipmentAction::Serialize(const SEBagValveMaskAction& src, CDM_BIND::BagValveMaskActionData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_equipmentaction());
}

void PBEquipmentAction::Load(const CDM_BIND::BagValveMaskConfigurationData& src, SEBagValveMaskConfiguration& dst, const SESubstanceManager& subMgr)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst, subMgr);
}
void PBEquipmentAction::Serialize(const CDM_BIND::BagValveMaskConfigurationData& src, SEBagValveMaskConfiguration& dst, const SESubstanceManager& subMgr)
{
  PBEquipmentAction::Serialize(src.bagvalvemaskaction(), dst);
  if (!src.configurationfile().empty())
    dst.SetConfigurationFile(src.configurationfile());
  else if (src.has_configuration())
    PBBagValveMask::Load(src.configuration(), dst.GetConfiguration(), subMgr);
  dst.SetMergeType((eMergeType)src.mergetype());
}
CDM_BIND::BagValveMaskConfigurationData* PBEquipmentAction::Unload(const SEBagValveMaskConfiguration& src)
{
  CDM_BIND::BagValveMaskConfigurationData* dst = new CDM_BIND::BagValveMaskConfigurationData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEBagValveMaskConfiguration& src, CDM_BIND::BagValveMaskConfigurationData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_bagvalvemaskaction());
  if (src.HasConfigurationFile())
    dst.set_configurationfile(src.m_ConfigurationFile);
  else if (src.HasConfiguration())
    dst.set_allocated_configuration(PBBagValveMask::Unload(*src.m_Configuration));
  dst.set_mergetype((CDM_BIND::eMergeType)src.m_MergeType);
}
void PBEquipmentAction::Copy(const SEBagValveMaskConfiguration& src, SEBagValveMaskConfiguration& dst, const SESubstanceManager& subMgr)
{
  dst.Clear();
  CDM_BIND::BagValveMaskConfigurationData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst, subMgr);
}

void PBEquipmentAction::Load(const CDM_BIND::BagValveMaskInstantaneousData& src, SEBagValveMaskInstantaneous& dst)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst);
}
void PBEquipmentAction::Serialize(const CDM_BIND::BagValveMaskInstantaneousData& src, SEBagValveMaskInstantaneous& dst)
{
  PBEquipmentAction::Serialize(src.bagvalvemaskaction(), dst);
  if (src.has_flow())
    PBProperty::Load(src.flow(), dst.GetFlow());
  else if (src.has_pressure())
    PBProperty::Load(src.pressure(), dst.GetPressure());
}
CDM_BIND::BagValveMaskInstantaneousData* PBEquipmentAction::Unload(const SEBagValveMaskInstantaneous& src)
{
  CDM_BIND::BagValveMaskInstantaneousData* dst = new CDM_BIND::BagValveMaskInstantaneousData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEBagValveMaskInstantaneous& src, CDM_BIND::BagValveMaskInstantaneousData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_bagvalvemaskaction());
  if (src.HasFlow())
    dst.set_allocated_flow(PBProperty::Unload(*src.m_Flow));
  else if (src.HasPressure())
    dst.set_allocated_pressure(PBProperty::Unload(*src.m_Pressure));
}
void PBEquipmentAction::Copy(const SEBagValveMaskInstantaneous& src, SEBagValveMaskInstantaneous& dst)
{
  dst.Clear();
  CDM_BIND::BagValveMaskInstantaneousData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst);
}

void PBEquipmentAction::Load(const CDM_BIND::BagValveMaskSqueezeData& src, SEBagValveMaskSqueeze& dst)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst);
}
void PBEquipmentAction::Serialize(const CDM_BIND::BagValveMaskSqueezeData& src, SEBagValveMaskSqueeze& dst)
{
  PBEquipmentAction::Serialize(src.bagvalvemaskaction(), dst);
  if (src.has_squeezepressure())
    PBProperty::Load(src.squeezepressure(), dst.GetSqueezePressure());
  else if (src.has_squeezevolume())
    PBProperty::Load(src.squeezevolume(), dst.GetSqueezeVolume());
  if (src.has_expiratoryperiod())
    PBProperty::Load(src.expiratoryperiod(), dst.GetExpiratoryPeriod());
  if (src.has_inspiratoryperiod())
    PBProperty::Load(src.inspiratoryperiod(), dst.GetInspiratoryPeriod());
}
CDM_BIND::BagValveMaskSqueezeData* PBEquipmentAction::Unload(const SEBagValveMaskSqueeze& src)
{
  CDM_BIND::BagValveMaskSqueezeData* dst = new CDM_BIND::BagValveMaskSqueezeData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEBagValveMaskSqueeze& src, CDM_BIND::BagValveMaskSqueezeData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_bagvalvemaskaction());
  if (src.HasSqueezePressure())
    dst.set_allocated_squeezepressure(PBProperty::Unload(*src.m_SqueezePressure));
  else if (src.HasSqueezeVolume())
    dst.set_allocated_squeezevolume(PBProperty::Unload(*src.m_SqueezeVolume));
  if (src.HasExpiratoryPeriod())
    dst.set_allocated_expiratoryperiod(PBProperty::Unload(*src.m_ExpiratoryPeriod));
  if (src.HasInspiratoryPeriod())
    dst.set_allocated_inspiratoryperiod(PBProperty::Unload(*src.m_InspiratoryPeriod));
}
void PBEquipmentAction::Copy(const SEBagValveMaskSqueeze& src, SEBagValveMaskSqueeze& dst)
{
  dst.Clear();
  CDM_BIND::BagValveMaskSqueezeData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst);
}

void PBEquipmentAction::Load(const CDM_BIND::BagValveMaskAutomatedData& src, SEBagValveMaskAutomated& dst)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst);
}
void PBEquipmentAction::Serialize(const CDM_BIND::BagValveMaskAutomatedData& src, SEBagValveMaskAutomated& dst)
{
  PBEquipmentAction::Serialize(src.bagvalvemaskaction(), dst);
  if (src.has_breathfrequency())
    PBProperty::Load(src.breathfrequency(), dst.GetBreathFrequency());
  if (src.has_inspiratoryexpiratoryratio())
    PBProperty::Load(src.inspiratoryexpiratoryratio(), dst.GetInspiratoryExpiratoryRatio());
  if (src.has_squeezepressure())
    PBProperty::Load(src.squeezepressure(), dst.GetSqueezePressure());
  else if (src.has_squeezevolume())
    PBProperty::Load(src.squeezevolume(), dst.GetSqueezeVolume());
}
CDM_BIND::BagValveMaskAutomatedData* PBEquipmentAction::Unload(const SEBagValveMaskAutomated& src)
{
  CDM_BIND::BagValveMaskAutomatedData* dst = new CDM_BIND::BagValveMaskAutomatedData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEBagValveMaskAutomated& src, CDM_BIND::BagValveMaskAutomatedData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_bagvalvemaskaction());

  if (src.HasBreathFrequency())
    dst.set_allocated_breathfrequency(PBProperty::Unload(*src.m_BreathFrequency));
  if (src.HasInspiratoryExpiratoryRatio())
    dst.set_allocated_inspiratoryexpiratoryratio(PBProperty::Unload(*src.m_InspiratoryExpiratoryRatio));
  if (src.HasSqueezePressure())
    dst.set_allocated_squeezepressure(PBProperty::Unload(*src.m_SqueezePressure));
  else if (src.HasSqueezeVolume())
    dst.set_allocated_squeezevolume(PBProperty::Unload(*src.m_SqueezeVolume));
}
void PBEquipmentAction::Copy(const SEBagValveMaskAutomated& src, SEBagValveMaskAutomated& dst)
{
  dst.Clear();
  CDM_BIND::BagValveMaskAutomatedData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst);
}

//////////
// ECMO //
//////////

void PBEquipmentAction::Serialize(const CDM_BIND::ECMOActionData& src, SEECMOAction& dst)
{
  PBEquipmentAction::Serialize(src.equipmentaction(), dst);
}
void PBEquipmentAction::Serialize(const SEECMOAction& src, CDM_BIND::ECMOActionData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_equipmentaction());
}

void PBEquipmentAction::Load(const CDM_BIND::ECMOConfigurationData& src, SEECMOConfiguration& dst, const SESubstanceManager& subMgr)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst, subMgr);
}
CDM_BIND::ECMOConfigurationData* PBEquipmentAction::Unload(const SEECMOConfiguration& src)
{
  CDM_BIND::ECMOConfigurationData* dst = new CDM_BIND::ECMOConfigurationData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const CDM_BIND::ECMOConfigurationData& src, SEECMOConfiguration& dst, const SESubstanceManager& subMgr)
{
  PBEquipmentAction::Serialize(src.ecmoaction(), dst);
  if (!src.settingsfile().empty())
    dst.SetSettingsFile(src.settingsfile());
  else if (src.has_settings())
    PBECMO::Load(src.settings(), dst.GetSettings(), subMgr);
  dst.SetMergeType((eMergeType)src.mergetype());
}
void PBEquipmentAction::Serialize(const SEECMOConfiguration& src, CDM_BIND::ECMOConfigurationData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_ecmoaction());
  if (src.HasSettingsFile())
    dst.set_settingsfile(src.m_SettingsFile);
  else if (src.HasSettings())
    dst.set_allocated_settings(PBECMO::Unload(*src.m_Settings));
  dst.set_mergetype((CDM_BIND::eMergeType)src.m_MergeType);
}
void PBEquipmentAction::Copy(const SEECMOConfiguration& src, SEECMOConfiguration& dst, const SESubstanceManager& subMgr)
{
  dst.Clear();
  CDM_BIND::ECMOConfigurationData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst, subMgr);
}

/////////////
// Inhaler //
/////////////

void PBEquipmentAction::Serialize(const CDM_BIND::InhalerActionData& src, SEInhalerAction& dst)
{
  PBEquipmentAction::Serialize(src.equipmentaction(), dst);
}
void PBEquipmentAction::Serialize(const SEInhalerAction& src, CDM_BIND::InhalerActionData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_equipmentaction());
}

void PBEquipmentAction::Load(const CDM_BIND::InhalerConfigurationData& src, SEInhalerConfiguration& dst, const SESubstanceManager& subMgr)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst, subMgr);
}
void PBEquipmentAction::Serialize(const CDM_BIND::InhalerConfigurationData& src, SEInhalerConfiguration& dst, const SESubstanceManager& subMgr)
{
  PBEquipmentAction::Serialize(src.inhaleraction(), dst);
  if (!src.configurationfile().empty())
    dst.SetConfigurationFile(src.configurationfile());
  else if (src.has_configuration())
    PBInhaler::Load(src.configuration(), dst.GetConfiguration(), subMgr);
  dst.SetMergeType((eMergeType)src.mergetype());
}
CDM_BIND::InhalerConfigurationData* PBEquipmentAction::Unload(const SEInhalerConfiguration& src)
{
  CDM_BIND::InhalerConfigurationData* dst = new CDM_BIND::InhalerConfigurationData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEInhalerConfiguration& src, CDM_BIND::InhalerConfigurationData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_inhaleraction());
  if (src.HasConfigurationFile())
    dst.set_configurationfile(src.m_ConfigurationFile); 
  else if (src.HasConfiguration())
    dst.set_allocated_configuration(PBInhaler::Unload(*src.m_Configuration));;
  dst.set_mergetype((CDM_BIND::eMergeType)src.m_MergeType);
}
void PBEquipmentAction::Copy(const SEInhalerConfiguration& src, SEInhalerConfiguration& dst, const SESubstanceManager& subMgr)
{
  dst.Clear();
  CDM_BIND::InhalerConfigurationData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst, subMgr);
}

///////////////////////////
// Mechanical Ventilator //
///////////////////////////

void PBEquipmentAction::Serialize(const CDM_BIND::MechanicalVentilatorActionData& src, SEMechanicalVentilatorAction& dst)
{
  PBEquipmentAction::Serialize(src.equipmentaction(), dst);
}
void PBEquipmentAction::Serialize(const SEMechanicalVentilatorAction& src, CDM_BIND::MechanicalVentilatorActionData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_equipmentaction());
}

void PBEquipmentAction::Load(const CDM_BIND::MechanicalVentilatorConfigurationData& src, SEMechanicalVentilatorConfiguration& dst, const SESubstanceManager& subMgr)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst, subMgr);
}
void PBEquipmentAction::Serialize(const CDM_BIND::MechanicalVentilatorConfigurationData& src, SEMechanicalVentilatorConfiguration& dst, const SESubstanceManager& subMgr)
{
  PBEquipmentAction::Serialize(src.mechanicalventilatoraction(), dst);
  if (!src.settingsfile().empty())
    dst.SetSettingsFile(src.settingsfile());
  else if (src.has_settings())
    PBMechanicalVentilator::Load(src.settings(), dst.GetSettings(), subMgr);
  dst.SetMergeType((eMergeType)src.mergetype());
}
CDM_BIND::MechanicalVentilatorConfigurationData* PBEquipmentAction::Unload(const SEMechanicalVentilatorConfiguration& src)
{
  CDM_BIND::MechanicalVentilatorConfigurationData* dst = new CDM_BIND::MechanicalVentilatorConfigurationData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEMechanicalVentilatorConfiguration& src, CDM_BIND::MechanicalVentilatorConfigurationData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_mechanicalventilatoraction());
  if (src.HasSettingsFile())
    dst.set_settingsfile(src.m_SettingsFile);
  else if (src.HasSettings())
    dst.set_allocated_settings(PBMechanicalVentilator::Unload(*src.m_Settings));
  dst.set_mergetype((CDM_BIND::eMergeType)src.m_MergeType);
}
void PBEquipmentAction::Copy(const SEMechanicalVentilatorConfiguration& src, SEMechanicalVentilatorConfiguration& dst, const SESubstanceManager& subMgr)
{
  dst.Clear();
  CDM_BIND::MechanicalVentilatorConfigurationData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst, subMgr);
}

void PBEquipmentAction::Load(const CDM_BIND::MechanicalVentilatorHoldData& src, SEMechanicalVentilatorHold& dst)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst);
}
void PBEquipmentAction::Serialize(const CDM_BIND::MechanicalVentilatorHoldData& src, SEMechanicalVentilatorHold& dst)
{
  PBEquipmentAction::Serialize(src.mechanicalventilatoraction(), dst);
  dst.SetState((eSwitch)src.state());
  dst.SetAppliedRespiratoryCycle((eAppliedRespiratoryCycle)src.appliedrespiratorycycle());
}
CDM_BIND::MechanicalVentilatorHoldData* PBEquipmentAction::Unload(const SEMechanicalVentilatorHold& src)
{
  CDM_BIND::MechanicalVentilatorHoldData* dst = new CDM_BIND::MechanicalVentilatorHoldData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEMechanicalVentilatorHold& src, CDM_BIND::MechanicalVentilatorHoldData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_mechanicalventilatoraction());
  dst.set_state((CDM_BIND::eSwitch)src.m_State);
  dst.set_appliedrespiratorycycle((CDM_BIND::eAppliedRespiratoryCycle)src.m_AppliedRespiratoryCycle);
}
void PBEquipmentAction::Copy(const SEMechanicalVentilatorHold& src, SEMechanicalVentilatorHold& dst)
{
  dst.Clear();
  CDM_BIND::MechanicalVentilatorHoldData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst);
}

void PBEquipmentAction::Load(const CDM_BIND::MechanicalVentilatorLeakData& src, SEMechanicalVentilatorLeak& dst)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst);
}
void PBEquipmentAction::Serialize(const CDM_BIND::MechanicalVentilatorLeakData& src, SEMechanicalVentilatorLeak& dst)
{
  PBEquipmentAction::Serialize(src.mechanicalventilatoraction(), dst);
  if (src.has_severity())
    PBProperty::Load(src.severity(), dst.GetSeverity());
}
CDM_BIND::MechanicalVentilatorLeakData* PBEquipmentAction::Unload(const SEMechanicalVentilatorLeak& src)
{
  CDM_BIND::MechanicalVentilatorLeakData* dst = new CDM_BIND::MechanicalVentilatorLeakData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEMechanicalVentilatorLeak& src, CDM_BIND::MechanicalVentilatorLeakData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_mechanicalventilatoraction());
  if (src.HasSeverity())
    dst.set_allocated_severity(PBProperty::Unload(*src.m_Severity));
}
void PBEquipmentAction::Copy(const SEMechanicalVentilatorLeak& src, SEMechanicalVentilatorLeak& dst)
{
  dst.Clear();
  CDM_BIND::MechanicalVentilatorLeakData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst);
}

void PBEquipmentAction::Load(const CDM_BIND::MechanicalVentilatorModeData& src, SEMechanicalVentilatorMode& dst)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst);
}
void PBEquipmentAction::Serialize(const CDM_BIND::MechanicalVentilatorModeData& src, SEMechanicalVentilatorMode& dst)
{
  PBEquipmentAction::Serialize(src.mechanicalventilatoraction(), dst);
  dst.m_Connection = (eSwitch)src.connection();
}
CDM_BIND::MechanicalVentilatorModeData* PBEquipmentAction::Unload(const SEMechanicalVentilatorMode& src)
{
  CDM_BIND::MechanicalVentilatorModeData* dst = new CDM_BIND::MechanicalVentilatorModeData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEMechanicalVentilatorMode& src, CDM_BIND::MechanicalVentilatorModeData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_mechanicalventilatoraction());
  dst.set_connection((CDM_BIND::eSwitch)src.m_Connection);
}

void PBEquipmentAction::Load(const CDM_BIND::MechanicalVentilatorContinuousPositiveAirwayPressureData& src, SEMechanicalVentilatorContinuousPositiveAirwayPressure& dst)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst);
}
void PBEquipmentAction::Serialize(const CDM_BIND::MechanicalVentilatorContinuousPositiveAirwayPressureData& src, SEMechanicalVentilatorContinuousPositiveAirwayPressure& dst)
{
  PBEquipmentAction::Serialize(src.mechanicalventilatormode(), dst);
  if (src.has_deltapressuresupport())
    PBProperty::Load(src.deltapressuresupport(), dst.GetDeltaPressureSupport());
  if (src.has_expirationcycleflow())
    PBProperty::Load(src.expirationcycleflow(), dst.GetExpirationCycleFlow());
  if (src.has_expirationcyclepressure())
    PBProperty::Load(src.expirationcyclepressure(), dst.GetExpirationCyclePressure());
  dst.SetExpirationWaveform((eDriverWaveform)src.expirationwaveform());
  if (src.has_fractioninspiredoxygen())
    PBProperty::Load(src.fractioninspiredoxygen(), dst.GetFractionInspiredOxygen());
  if (src.has_inspirationpatienttriggerflow())
    PBProperty::Load(src.inspirationpatienttriggerflow(), dst.GetInspirationPatientTriggerFlow());
  if (src.has_inspirationpatienttriggerpressure())
    PBProperty::Load(src.inspirationpatienttriggerpressure(), dst.GetInspirationPatientTriggerPressure());
  dst.SetInspirationWaveform((eDriverWaveform)src.inspirationwaveform());
  if (src.has_positiveendexpiratorypressure())
    PBProperty::Load(src.positiveendexpiratorypressure(), dst.GetPositiveEndExpiratoryPressure());
  if (src.has_slope())
    PBProperty::Load(src.slope(), dst.GetSlope());
}
CDM_BIND::MechanicalVentilatorContinuousPositiveAirwayPressureData* PBEquipmentAction::Unload(const SEMechanicalVentilatorContinuousPositiveAirwayPressure& src)
{
  CDM_BIND::MechanicalVentilatorContinuousPositiveAirwayPressureData* dst = new CDM_BIND::MechanicalVentilatorContinuousPositiveAirwayPressureData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEMechanicalVentilatorContinuousPositiveAirwayPressure& src, CDM_BIND::MechanicalVentilatorContinuousPositiveAirwayPressureData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_mechanicalventilatormode());
  if (src.HasDeltaPressureSupport())
    dst.set_allocated_deltapressuresupport(PBProperty::Unload(*src.m_DeltaPressureSupport));
  if (src.HasExpirationCycleFlow())
    dst.set_allocated_expirationcycleflow(PBProperty::Unload(*src.m_ExpirationCycleFlow));
  if (src.HasExpirationCyclePressure())
    dst.set_allocated_expirationcyclepressure(PBProperty::Unload(*src.m_ExpirationCyclePressure));
  dst.set_expirationwaveform((CDM_BIND::eDriverWaveform)src.m_ExpirationWaveform);
  if (src.HasFractionInspiredOxygen())
    dst.set_allocated_fractioninspiredoxygen(PBProperty::Unload(*src.m_FractionInspiredOxygen));
  if (src.HasInspirationPatientTriggerFlow())
    dst.set_allocated_inspirationpatienttriggerflow(PBProperty::Unload(*src.m_InspirationPatientTriggerFlow));
  if (src.HasInspirationPatientTriggerPressure())
    dst.set_allocated_inspirationpatienttriggerpressure(PBProperty::Unload(*src.m_InspirationPatientTriggerPressure));
  dst.set_inspirationwaveform((CDM_BIND::eDriverWaveform)src.m_InspirationWaveform);
  if (src.HasPositiveEndExpiratoryPressure())
    dst.set_allocated_positiveendexpiratorypressure(PBProperty::Unload(*src.m_PositiveEndExpiratoryPressure));
  if (src.HasSlope())
    dst.set_allocated_slope(PBProperty::Unload(*src.m_Slope));
}
void PBEquipmentAction::Copy(const SEMechanicalVentilatorContinuousPositiveAirwayPressure& src, SEMechanicalVentilatorContinuousPositiveAirwayPressure& dst)
{
  dst.Clear();
  CDM_BIND::MechanicalVentilatorContinuousPositiveAirwayPressureData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst);
}

void PBEquipmentAction::Load(const CDM_BIND::MechanicalVentilatorPressureControlData& src, SEMechanicalVentilatorPressureControl& dst)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst);
}
void PBEquipmentAction::Serialize(const CDM_BIND::MechanicalVentilatorPressureControlData& src, SEMechanicalVentilatorPressureControl& dst)
{
  PBEquipmentAction::Serialize(src.mechanicalventilatormode(), dst);
  dst.m_Mode = (eMechanicalVentilator_PressureControlMode)src.mode();
  if (src.has_fractioninspiredoxygen())
    PBProperty::Load(src.fractioninspiredoxygen(), dst.GetFractionInspiredOxygen());
  if (src.has_inspirationpatienttriggerflow())
    PBProperty::Load(src.inspirationpatienttriggerflow(), dst.GetInspirationPatientTriggerFlow());
  if (src.has_inspirationpatienttriggerpressure())
    PBProperty::Load(src.inspirationpatienttriggerpressure(), dst.GetInspirationPatientTriggerPressure());
  dst.SetInspirationWaveform((eDriverWaveform)src.inspirationwaveform());
  if (src.has_inspiratoryperiod())
    PBProperty::Load(src.inspiratoryperiod(), dst.GetInspiratoryPeriod());
  if (src.has_inspiratorypressure())
    PBProperty::Load(src.inspiratorypressure(), dst.GetInspiratoryPressure());
  if (src.has_positiveendexpiratorypressure())
    PBProperty::Load(src.positiveendexpiratorypressure(), dst.GetPositiveEndExpiratoryPressure());
  if (src.has_respirationrate())
    PBProperty::Load(src.respirationrate(), dst.GetRespirationRate());
  if (src.has_slope())
    PBProperty::Load(src.slope(), dst.GetSlope());
}
CDM_BIND::MechanicalVentilatorPressureControlData* PBEquipmentAction::Unload(const SEMechanicalVentilatorPressureControl& src)
{
  CDM_BIND::MechanicalVentilatorPressureControlData* dst = new CDM_BIND::MechanicalVentilatorPressureControlData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEMechanicalVentilatorPressureControl& src, CDM_BIND::MechanicalVentilatorPressureControlData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_mechanicalventilatormode());
  dst.set_mode((CDM_BIND::MechanicalVentilatorPressureControlData::eMode)src.m_Mode);
  if (src.HasFractionInspiredOxygen())
    dst.set_allocated_fractioninspiredoxygen(PBProperty::Unload(*src.m_FractionInspiredOxygen));
  if (src.HasInspirationPatientTriggerFlow())
    dst.set_allocated_inspirationpatienttriggerflow(PBProperty::Unload(*src.m_InspirationPatientTriggerFlow));
  if (src.HasInspirationPatientTriggerPressure())
    dst.set_allocated_inspirationpatienttriggerpressure(PBProperty::Unload(*src.m_InspirationPatientTriggerPressure));
  dst.set_inspirationwaveform((CDM_BIND::eDriverWaveform)src.m_InspirationWaveform);
  if (src.HasInspiratoryPeriod())
    dst.set_allocated_inspiratoryperiod(PBProperty::Unload(*src.m_InspiratoryPeriod));
  if (src.HasInspiratoryPressure())
    dst.set_allocated_inspiratorypressure(PBProperty::Unload(*src.m_InspiratoryPressure));
  if (src.HasPositiveEndExpiratoryPressure())
    dst.set_allocated_positiveendexpiratorypressure(PBProperty::Unload(*src.m_PositiveEndExpiratoryPressure));
  if (src.HasRespirationRate())
    dst.set_allocated_respirationrate(PBProperty::Unload(*src.m_RespirationRate));
  if (src.HasSlope())
    dst.set_allocated_slope(PBProperty::Unload(*src.m_Slope));
}
void PBEquipmentAction::Copy(const SEMechanicalVentilatorPressureControl& src, SEMechanicalVentilatorPressureControl& dst)
{
  dst.Clear();
  CDM_BIND::MechanicalVentilatorPressureControlData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst);
}

void PBEquipmentAction::Load(const CDM_BIND::MechanicalVentilatorVolumeControlData& src, SEMechanicalVentilatorVolumeControl& dst)
{
  dst.Clear();
  PBEquipmentAction::Serialize(src, dst);
}
void PBEquipmentAction::Serialize(const CDM_BIND::MechanicalVentilatorVolumeControlData& src, SEMechanicalVentilatorVolumeControl& dst)
{
  PBEquipmentAction::Serialize(src.mechanicalventilatormode(), dst);
  dst.m_Mode = (eMechanicalVentilator_VolumeControlMode)src.mode();
  if (src.has_flow())
    PBProperty::Load(src.flow(), dst.GetFlow());
  if (src.has_fractioninspiredoxygen())
    PBProperty::Load(src.fractioninspiredoxygen(), dst.GetFractionInspiredOxygen());
  if (src.has_inspirationpatienttriggerflow())
    PBProperty::Load(src.inspirationpatienttriggerflow(), dst.GetInspirationPatientTriggerFlow());
  if (src.has_inspirationpatienttriggerpressure())
    PBProperty::Load(src.inspirationpatienttriggerpressure(), dst.GetInspirationPatientTriggerPressure());
  dst.SetInspirationWaveform((eDriverWaveform)src.inspirationwaveform());
  if (src.has_inspiratoryperiod())
    PBProperty::Load(src.inspiratoryperiod(), dst.GetInspiratoryPeriod());
  if (src.has_positiveendexpiratorypressure())
    PBProperty::Load(src.positiveendexpiratorypressure(), dst.GetPositiveEndExpiratoryPressure());
  if (src.has_respirationrate())
    PBProperty::Load(src.respirationrate(), dst.GetRespirationRate());
  if (src.has_tidalvolume())
    PBProperty::Load(src.tidalvolume(), dst.GetTidalVolume());
  if (src.has_slope())
    PBProperty::Load(src.slope(), dst.GetSlope());
}
CDM_BIND::MechanicalVentilatorVolumeControlData* PBEquipmentAction::Unload(const SEMechanicalVentilatorVolumeControl& src)
{
  CDM_BIND::MechanicalVentilatorVolumeControlData* dst = new CDM_BIND::MechanicalVentilatorVolumeControlData();
  PBEquipmentAction::Serialize(src, *dst);
  return dst;
}
void PBEquipmentAction::Serialize(const SEMechanicalVentilatorVolumeControl& src, CDM_BIND::MechanicalVentilatorVolumeControlData& dst)
{
  PBEquipmentAction::Serialize(src, *dst.mutable_mechanicalventilatormode());
  dst.set_mode((CDM_BIND::MechanicalVentilatorVolumeControlData::eMode)src.m_Mode);
  if (src.HasFlow())
    dst.set_allocated_flow(PBProperty::Unload(*src.m_Flow));
  if (src.HasFractionInspiredOxygen())
    dst.set_allocated_fractioninspiredoxygen(PBProperty::Unload(*src.m_FractionInspiredOxygen));
  if (src.HasInspirationPatientTriggerFlow())
    dst.set_allocated_inspirationpatienttriggerflow(PBProperty::Unload(*src.m_InspirationPatientTriggerFlow));
  if (src.HasInspirationPatientTriggerPressure())
    dst.set_allocated_inspirationpatienttriggerpressure(PBProperty::Unload(*src.m_InspirationPatientTriggerPressure));
  dst.set_inspirationwaveform((CDM_BIND::eDriverWaveform)src.m_InspirationWaveform);
  if (src.HasInspiratoryPeriod())
    dst.set_allocated_inspiratoryperiod(PBProperty::Unload(*src.m_InspiratoryPeriod));
  if (src.HasPositiveEndExpiratoryPressure())
    dst.set_allocated_positiveendexpiratorypressure(PBProperty::Unload(*src.m_PositiveEndExpiratoryPressure));
  if (src.HasRespirationRate())
    dst.set_allocated_respirationrate(PBProperty::Unload(*src.m_RespirationRate));
  if (src.HasTidalVolume())
    dst.set_allocated_tidalvolume(PBProperty::Unload(*src.m_TidalVolume));
  if (src.HasSlope())
    dst.set_allocated_slope(PBProperty::Unload(*src.m_Slope));
}
void PBEquipmentAction::Copy(const SEMechanicalVentilatorVolumeControl& src, SEMechanicalVentilatorVolumeControl& dst)
{
  dst.Clear();
  CDM_BIND::MechanicalVentilatorVolumeControlData data;
  PBEquipmentAction::Serialize(src, data);
  PBEquipmentAction::Serialize(data, dst);
}

///////////////////////////
// Equipment Load/Unload //
///////////////////////////

SEEquipmentAction* PBEquipmentAction::Load(const CDM_BIND::AnyEquipmentActionData& any, const SESubstanceManager& subMgr)
{
  switch (any.Action_case())
  {
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kAnesthesiaMachineConfiguration:
  {
    SEAnesthesiaMachineConfiguration* a = new SEAnesthesiaMachineConfiguration(subMgr.GetLogger());
    PBEquipmentAction::Load(any.anesthesiamachineconfiguration(), *a, subMgr);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kAnesthesiaMachineOxygenTankPressureLoss:
  {
    SEAnesthesiaMachineOxygenTankPressureLoss* a = new SEAnesthesiaMachineOxygenTankPressureLoss();
    PBEquipmentAction::Load(any.anesthesiamachineoxygentankpressureloss(), *a);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kAnesthesiaMachineOxygenWallPortPressureLoss:
  {
    SEAnesthesiaMachineOxygenWallPortPressureLoss* a = new SEAnesthesiaMachineOxygenWallPortPressureLoss();
    PBEquipmentAction::Load(any.anesthesiamachineoxygenwallportpressureloss(), *a);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kAnesthesiaMachineExpiratoryValveLeak:
  {
    SEAnesthesiaMachineExpiratoryValveLeak* a = new SEAnesthesiaMachineExpiratoryValveLeak();
    PBEquipmentAction::Load(any.anesthesiamachineexpiratoryvalveleak(), *a);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kAnesthesiaMachineExpiratoryValveObstruction:
  {
    SEAnesthesiaMachineExpiratoryValveObstruction* a = new SEAnesthesiaMachineExpiratoryValveObstruction();
    PBEquipmentAction::Load(any.anesthesiamachineexpiratoryvalveobstruction(), *a);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kAnesthesiaMachineInspiratoryValveLeak:
  {
    SEAnesthesiaMachineInspiratoryValveLeak* a = new SEAnesthesiaMachineInspiratoryValveLeak();
    PBEquipmentAction::Load(any.anesthesiamachineinspiratoryvalveleak(), *a);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kAnesthesiaMachineInspiratoryValveObstruction:
  {
    SEAnesthesiaMachineInspiratoryValveObstruction* a = new SEAnesthesiaMachineInspiratoryValveObstruction();
    PBEquipmentAction::Load(any.anesthesiamachineinspiratoryvalveobstruction(), *a);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kAnesthesiaMachineMaskLeak:
  {
    SEAnesthesiaMachineMaskLeak* a = new SEAnesthesiaMachineMaskLeak();
    PBEquipmentAction::Load(any.anesthesiamachinemaskleak(), *a);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kAnesthesiaMachineSodaLimeFailure:
  {
    SEAnesthesiaMachineSodaLimeFailure* a = new SEAnesthesiaMachineSodaLimeFailure();
    PBEquipmentAction::Load(any.anesthesiamachinesodalimefailure(), *a);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kAnesthesiaMachineTubeCuffLeak:
  {
    SEAnesthesiaMachineTubeCuffLeak* a = new SEAnesthesiaMachineTubeCuffLeak();
    PBEquipmentAction::Load(any.anesthesiamachinetubecuffleak(), *a);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kAnesthesiaMachineVaporizerFailure:
  {
    SEAnesthesiaMachineVaporizerFailure* a = new SEAnesthesiaMachineVaporizerFailure();
    PBEquipmentAction::Load(any.anesthesiamachinevaporizerfailure(), *a);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kAnesthesiaMachineVentilatorPressureLoss:
  {
    SEAnesthesiaMachineVentilatorPressureLoss* a = new SEAnesthesiaMachineVentilatorPressureLoss();
    PBEquipmentAction::Load(any.anesthesiamachineventilatorpressureloss(), *a);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kAnesthesiaMachineYPieceDisconnect:
  {
    SEAnesthesiaMachineYPieceDisconnect* a = new SEAnesthesiaMachineYPieceDisconnect();
    PBEquipmentAction::Load(any.anesthesiamachineypiecedisconnect(), *a);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kBagValveMaskConfiguration:
  {
    SEBagValveMaskConfiguration* a = new SEBagValveMaskConfiguration(subMgr.GetLogger());
    PBEquipmentAction::Load(any.bagvalvemaskconfiguration(), *a, subMgr);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kBagValveMaskAutomated:
  {
    SEBagValveMaskAutomated* a = new SEBagValveMaskAutomated(subMgr.GetLogger());
    PBEquipmentAction::Load(any.bagvalvemaskautomated(), *a);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kBagValveMaskInstantaneous:
  {
    SEBagValveMaskInstantaneous* a = new SEBagValveMaskInstantaneous(subMgr.GetLogger());
    PBEquipmentAction::Load(any.bagvalvemaskinstantaneous(), *a);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kBagValveMaskSqueeze:
  {
    SEBagValveMaskSqueeze* a = new SEBagValveMaskSqueeze(subMgr.GetLogger());
    PBEquipmentAction::Load(any.bagvalvemasksqueeze(), *a);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kECMOConfiguration:
  {
    SEECMOConfiguration* a = new SEECMOConfiguration(subMgr.GetLogger());
    PBEquipmentAction::Load(any.ecmoconfiguration(), *a, subMgr);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kInhalerConfiguration:
  {
    SEInhalerConfiguration* a = new SEInhalerConfiguration(subMgr.GetLogger());
    PBEquipmentAction::Load(any.inhalerconfiguration(), *a, subMgr);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kMechanicalVentilatorConfiguration:
  {
    SEMechanicalVentilatorConfiguration* a = new SEMechanicalVentilatorConfiguration(subMgr.GetLogger());
    PBEquipmentAction::Load(any.mechanicalventilatorconfiguration(), *a, subMgr);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kMechanicalVentilatorHold:
  {
    SEMechanicalVentilatorHold* a = new SEMechanicalVentilatorHold(subMgr.GetLogger());
    PBEquipmentAction::Load(any.mechanicalventilatorhold(), *a);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kMechanicalVentilatorLeak:
  {
    SEMechanicalVentilatorLeak* a = new SEMechanicalVentilatorLeak(subMgr.GetLogger());
    PBEquipmentAction::Load(any.mechanicalventilatorleak(), *a);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kMechanicalVentilatorContinuousPositiveAirwayPressure:
  {
    SEMechanicalVentilatorContinuousPositiveAirwayPressure* a = new SEMechanicalVentilatorContinuousPositiveAirwayPressure(subMgr.GetLogger());
    PBEquipmentAction::Load(any.mechanicalventilatorcontinuouspositiveairwaypressure(), *a);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kMechanicalVentilatorPressureControl:
  {
    SEMechanicalVentilatorPressureControl* a = new SEMechanicalVentilatorPressureControl(subMgr.GetLogger());
    PBEquipmentAction::Load(any.mechanicalventilatorpressurecontrol(), *a);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::kMechanicalVentilatorVolumeControl:
  {
    SEMechanicalVentilatorVolumeControl* a = new SEMechanicalVentilatorVolumeControl(subMgr.GetLogger());
    PBEquipmentAction::Load(any.mechanicalventilatorvolumecontrol(), *a);
    return a;
  }
  case CDM_BIND::AnyEquipmentActionData::ActionCase::ACTION_NOT_SET:
  {
    subMgr.Warning("AnyEquipmentActionData Action is empty...was that intended?");
    return nullptr;
  }
  }
  subMgr.Error("Unknown Equipment action enum :" + std::to_string((int)any.Action_case()));
  return nullptr;
}
CDM_BIND::AnyEquipmentActionData* PBEquipmentAction::Unload(const SEEquipmentAction& action)
{
  CDM_BIND::AnyEquipmentActionData* any = new CDM_BIND::AnyEquipmentActionData();

  const SEAnesthesiaMachineConfiguration* amc = dynamic_cast<const SEAnesthesiaMachineConfiguration*>(&action);
  if (amc != nullptr)
  {
    any->set_allocated_anesthesiamachineconfiguration(PBEquipmentAction::Unload(*amc));
    return any;
  }

  const SEBagValveMaskConfiguration* bvmc = dynamic_cast<const SEBagValveMaskConfiguration*>(&action);
  if (bvmc != nullptr)
  {
    any->set_allocated_bagvalvemaskconfiguration(PBEquipmentAction::Unload(*bvmc));
    return any;
  }

  const SEBagValveMaskAutomated* bvma = dynamic_cast<const SEBagValveMaskAutomated*>(&action);
  if (bvma != nullptr)
  {
    any->set_allocated_bagvalvemaskautomated(PBEquipmentAction::Unload(*bvma));
    return any;
  }

  const SEBagValveMaskInstantaneous* bvmi = dynamic_cast<const SEBagValveMaskInstantaneous*>(&action);
  if (bvmi != nullptr)
  {
    any->set_allocated_bagvalvemaskinstantaneous(PBEquipmentAction::Unload(*bvmi));
    return any;
  }

  const SEBagValveMaskSqueeze* bvms = dynamic_cast<const SEBagValveMaskSqueeze*>(&action);
  if (bvms != nullptr)
  {
    any->set_allocated_bagvalvemasksqueeze(PBEquipmentAction::Unload(*bvms));
    return any;
  }

  const SEECMOConfiguration* ec = dynamic_cast<const SEECMOConfiguration*>(&action);
  if (ec != nullptr)
  {
    any->set_allocated_ecmoconfiguration(PBEquipmentAction::Unload(*ec));
    return any;
  }

  const SEInhalerConfiguration* ic = dynamic_cast<const SEInhalerConfiguration*>(&action);
  if (ic != nullptr)
  {
    any->set_allocated_inhalerconfiguration(PBEquipmentAction::Unload(*ic));
    return any;
  }

  const SEMechanicalVentilatorConfiguration* mvc = dynamic_cast<const SEMechanicalVentilatorConfiguration*>(&action);
  if (mvc != nullptr)
  {
    any->set_allocated_mechanicalventilatorconfiguration(PBEquipmentAction::Unload(*mvc));
    return any;
  }

  const SEMechanicalVentilatorHold* mvh = dynamic_cast<const SEMechanicalVentilatorHold*>(&action);
  if (mvh != nullptr)
  {
    any->set_allocated_mechanicalventilatorhold(PBEquipmentAction::Unload(*mvh));
    return any;
  }

  const SEMechanicalVentilatorLeak* mvl = dynamic_cast<const SEMechanicalVentilatorLeak*>(&action);
  if (mvl != nullptr)
  {
    any->set_allocated_mechanicalventilatorleak(PBEquipmentAction::Unload(*mvl));
    return any;
  }

  const SEMechanicalVentilatorContinuousPositiveAirwayPressure* mvcpap = dynamic_cast<const SEMechanicalVentilatorContinuousPositiveAirwayPressure*>(&action);
  if (mvcpap != nullptr)
  {
    any->set_allocated_mechanicalventilatorcontinuouspositiveairwaypressure(PBEquipmentAction::Unload(*mvcpap));
    return any;
  }

  const SEMechanicalVentilatorPressureControl* mvpc = dynamic_cast<const SEMechanicalVentilatorPressureControl*>(&action);
  if (mvpc != nullptr)
  {
    any->set_allocated_mechanicalventilatorpressurecontrol(PBEquipmentAction::Unload(*mvpc));
    return any;
  }

  const SEMechanicalVentilatorVolumeControl* mvvc = dynamic_cast<const SEMechanicalVentilatorVolumeControl*>(&action);
  if (mvvc != nullptr)
  {
    any->set_allocated_mechanicalventilatorvolumecontrol(PBEquipmentAction::Unload(*mvvc));
    return any;
  }

  const SEAnesthesiaMachineOxygenWallPortPressureLoss* owppl = dynamic_cast<const SEAnesthesiaMachineOxygenWallPortPressureLoss*>(&action);
  if (owppl != nullptr)
  {
    any->set_allocated_anesthesiamachineoxygenwallportpressureloss(PBEquipmentAction::Unload(*owppl));
    return any;
  }

  const SEAnesthesiaMachineOxygenTankPressureLoss* otpl = dynamic_cast<const SEAnesthesiaMachineOxygenTankPressureLoss*>(&action);
  if (otpl != nullptr)
  {
    any->set_allocated_anesthesiamachineoxygentankpressureloss(PBEquipmentAction::Unload(*otpl));
    return any;
  }

  const SEAnesthesiaMachineExpiratoryValveLeak* evl = dynamic_cast<const SEAnesthesiaMachineExpiratoryValveLeak*>(&action);
  if (evl != nullptr)
  {
    any->set_allocated_anesthesiamachineexpiratoryvalveleak(PBEquipmentAction::Unload(*evl));
    return any;
  }

  const SEAnesthesiaMachineExpiratoryValveObstruction* evo = dynamic_cast<const SEAnesthesiaMachineExpiratoryValveObstruction*>(&action);
  if (evo != nullptr)
  {
    any->set_allocated_anesthesiamachineexpiratoryvalveobstruction(PBEquipmentAction::Unload(*evo));
    return any;
  }

  const SEAnesthesiaMachineInspiratoryValveLeak* ivl = dynamic_cast<const SEAnesthesiaMachineInspiratoryValveLeak*>(&action);
  if (ivl != nullptr)
  {
    any->set_allocated_anesthesiamachineinspiratoryvalveleak(PBEquipmentAction::Unload(*ivl));
    return any;
  }

  const SEAnesthesiaMachineInspiratoryValveObstruction* ivo = dynamic_cast<const SEAnesthesiaMachineInspiratoryValveObstruction*>(&action);
  if (ivo != nullptr)
  {
    any->set_allocated_anesthesiamachineinspiratoryvalveobstruction(PBEquipmentAction::Unload(*ivo));
    return any;
  }

  const SEAnesthesiaMachineMaskLeak* ml = dynamic_cast<const SEAnesthesiaMachineMaskLeak*>(&action);
  if (ml != nullptr)
  {
    any->set_allocated_anesthesiamachinemaskleak(PBEquipmentAction::Unload(*ml));
    return any;
  }

  const SEAnesthesiaMachineSodaLimeFailure* slf = dynamic_cast<const SEAnesthesiaMachineSodaLimeFailure*>(&action);
  if (slf != nullptr)
  {
    any->set_allocated_anesthesiamachinesodalimefailure(PBEquipmentAction::Unload(*slf));
    return any;
  }

  const SEAnesthesiaMachineTubeCuffLeak* tcl = dynamic_cast<const SEAnesthesiaMachineTubeCuffLeak*>(&action);
  if (tcl != nullptr)
  {
    any->set_allocated_anesthesiamachinetubecuffleak(PBEquipmentAction::Unload(*tcl));
    return any;
  }

  const SEAnesthesiaMachineVaporizerFailure* vf = dynamic_cast<const SEAnesthesiaMachineVaporizerFailure*>(&action);
  if (vf != nullptr)
  {
    any->set_allocated_anesthesiamachinevaporizerfailure(PBEquipmentAction::Unload(*vf));
    return any;
  }

  const SEAnesthesiaMachineVentilatorPressureLoss* vpl = dynamic_cast<const SEAnesthesiaMachineVentilatorPressureLoss*>(&action);
  if (vpl != nullptr)
  {
    any->set_allocated_anesthesiamachineventilatorpressureloss(PBEquipmentAction::Unload(*vpl));
    return any;
  }

  const SEAnesthesiaMachineYPieceDisconnect* ypd = dynamic_cast<const SEAnesthesiaMachineYPieceDisconnect*>(&action);
  if (ypd != nullptr)
  {
    any->set_allocated_anesthesiamachineypiecedisconnect(PBEquipmentAction::Unload(*ypd));
    return any;
  }
  delete any;
  return nullptr;
}