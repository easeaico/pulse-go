/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/CommonDefs.h"
#include "engine/io/protobuf/PBEquipment.h"
PUSH_PROTO_WARNINGS
#include "pulse/engine/bind/Equipment.pb.h"
POP_PROTO_WARNINGS

#include "cdm/io/protobuf/PBAnesthesiaMachine.h"
#include "cdm/io/protobuf/PBBagValveMask.h"
#include "cdm/io/protobuf/PBECMO.h"
#include "cdm/io/protobuf/PBElectroCardioGram.h"
#include "cdm/io/protobuf/PBInhaler.h"
#include "cdm/io/protobuf/PBMechanicalVentilator.h"
#include "cdm/io/protobuf/PBProperties.h"
#include "cdm/system/equipment/anesthesia_machine/SEAnesthesiaMachine.h"
#include "cdm/system/equipment/electrocardiogram/SEElectroCardioGram.h"
#include "cdm/system/equipment/inhaler/SEInhaler.h"

namespace pulse
{
  void PBEquipment::Load(const PULSE_BIND::AnesthesiaMachineData& src, AnesthesiaMachineModel& dst)
  {
    dst.Clear();
    dst.SetUp();
    PBEquipment::Serialize(src, dst);
    dst.StateChange();
  }
  void PBEquipment::Serialize(const PULSE_BIND::AnesthesiaMachineData& src, AnesthesiaMachineModel& dst)
  {
    PBAnesthesiaMachine::Serialize(src.common(), dst, (SESubstanceManager&)dst.m_data.GetSubstances());
    dst.m_inhaling = src.inhaling();
    dst.m_currentbreathingCycleTime_s = src.currentbreathingcycletime_s();
    dst.m_inspirationTime_s = src.inspirationtime_s();
    dst.m_O2InletVolumeFraction = src.oxygeninletvolumefraction();
    dst.m_totalBreathingCycleTime_s = src.totalbreathingcycletime_s();
  }
  PULSE_BIND::AnesthesiaMachineData* PBEquipment::Unload(const AnesthesiaMachineModel& src)
  {
    PULSE_BIND::AnesthesiaMachineData* dst = new PULSE_BIND::AnesthesiaMachineData();
    PBEquipment::Serialize(src, *dst);
    return dst;
  }
  void PBEquipment::Serialize(const AnesthesiaMachineModel& src, PULSE_BIND::AnesthesiaMachineData& dst)
  {
    PBAnesthesiaMachine::Serialize(src, *dst.mutable_common());
    dst.set_inhaling(src.m_inhaling);
    dst.set_currentbreathingcycletime_s(src.m_currentbreathingCycleTime_s);
    dst.set_inspirationtime_s(src.m_inspirationTime_s);
    dst.set_oxygeninletvolumefraction(src.m_O2InletVolumeFraction);
    dst.set_totalbreathingcycletime_s(src.m_totalBreathingCycleTime_s);
  }

  void PBEquipment::Load(const PULSE_BIND::BagValveMaskData& src, BagValveMaskModel& dst)
  {
    dst.Clear();
    dst.SetUp();
    PBEquipment::Serialize(src, dst);
    dst.StateChange();
  }
  void PBEquipment::Serialize(const PULSE_BIND::BagValveMaskData& src, BagValveMaskModel& dst)
  {
    PBBagValveMask::Serialize(src.common(), dst, (SESubstanceManager&)dst.m_data.GetSubstances());
    dst.m_CurrentPeriodTime_s = src.currentperiodtime_s();
    dst.m_SqueezeFlow_L_Per_s = src.squeezeflow_l_per_s();
    dst.m_SqueezePressure_cmH2O = src.squeezepressure_cmh2o();
  }
  PULSE_BIND::BagValveMaskData* PBEquipment::Unload(const BagValveMaskModel& src)
  {
    PULSE_BIND::BagValveMaskData* dst = new PULSE_BIND::BagValveMaskData();
    PBEquipment::Serialize(src, *dst);
    return dst;
  }
  void PBEquipment::Serialize(const BagValveMaskModel& src, PULSE_BIND::BagValveMaskData& dst)
  {
    PBBagValveMask::Serialize(src, *dst.mutable_common());
    dst.set_currentperiodtime_s(src.m_CurrentPeriodTime_s);
    dst.set_squeezeflow_l_per_s(src.m_SqueezeFlow_L_Per_s);
    dst.set_squeezepressure_cmh2o(src.m_SqueezePressure_cmH2O);
  }

  void PBEquipment::Load(const PULSE_BIND::ECMOData& src, ECMOModel& dst)
  {
    dst.Clear();
    dst.SetUp();
    PBEquipment::Serialize(src, dst);
    dst.StateChange();
  }
  void PBEquipment::Serialize(const PULSE_BIND::ECMOData& src, ECMOModel& dst)
  {
    PBECMO::Serialize(src.common(), dst, (SESubstanceManager&)dst.m_data.GetSubstances());
    dst.m_CurrentInflowLocation = dst.GetSettings().GetInflowLocation();
    dst.m_CurrentOutflowLocation = dst.GetSettings().GetOutflowLocation();
  }
  PULSE_BIND::ECMOData* PBEquipment::Unload(const ECMOModel& src)
  {
    PULSE_BIND::ECMOData* dst = new PULSE_BIND::ECMOData();
    PBEquipment::Serialize(src, *dst);
    return dst;
  }
  void PBEquipment::Serialize(const ECMOModel& src, PULSE_BIND::ECMOData& dst)
  {
    PBECMO::Serialize(src, *dst.mutable_common());
  }

  void PBEquipment::Load(const PULSE_BIND::ElectroCardioGramData& src, ElectroCardioGramModel& dst)
  {
    dst.Clear();
    dst.SetUp();
    PBEquipment::Serialize(src, dst);
  }
  void PBEquipment::Serialize(const PULSE_BIND::ElectroCardioGramData& src, ElectroCardioGramModel& dst)
  {
    PBElectroCardioGram::Serialize(src.common(), dst);
  }
  PULSE_BIND::ElectroCardioGramData* PBEquipment::Unload(const ElectroCardioGramModel& src)
  {
    PULSE_BIND::ElectroCardioGramData* dst = new PULSE_BIND::ElectroCardioGramData();
    PBEquipment::Serialize(src, *dst);
    return dst;
  }
  void PBEquipment::Serialize(const ElectroCardioGramModel& src, PULSE_BIND::ElectroCardioGramData& dst)
  {
    PBElectroCardioGram::Serialize(src, *dst.mutable_common());
  }

  void PBEquipment::Load(const PULSE_BIND::InhalerData& src, InhalerModel& dst)
  {
    dst.Clear();
    dst.SetUp();
    PBEquipment::Serialize(src, dst);
    dst.StateChange();
  }
  void PBEquipment::Serialize(const PULSE_BIND::InhalerData& src, InhalerModel& dst)
  {
    PBInhaler::Serialize(src.common(), dst, (SESubstanceManager&)dst.m_data.GetSubstances());
  }
  PULSE_BIND::InhalerData* PBEquipment::Unload(const InhalerModel& src)
  {
    PULSE_BIND::InhalerData* dst = new PULSE_BIND::InhalerData();
    PBEquipment::Serialize(src, *dst);
    return dst;
  }
  void PBEquipment::Serialize(const InhalerModel& src, PULSE_BIND::InhalerData& dst)
  {
    PBInhaler::Serialize(src, *dst.mutable_common());
  }

  void PBEquipment::Load(const PULSE_BIND::MechanicalVentilatorData& src, MechanicalVentilatorModel& dst)
  {
    dst.Clear();
    dst.SetUp();
    PBEquipment::Serialize(src, dst);
    dst.StateChange();
  }
  void PBEquipment::Serialize(const PULSE_BIND::MechanicalVentilatorData& src, MechanicalVentilatorModel& dst)
  {
    PBMechanicalVentilator::Serialize(src.common(), dst, (SESubstanceManager&)dst.m_data.GetSubstances());
    dst.m_CurrentPeriodTime_s = src.currentperiodtime_s();
    dst.m_DriverPressure_cmH2O = src.driverpressure_cmh2o();
    dst.m_PreviousDriverFlow_L_Per_s = src.previousdriverflow_l_per_s();
    dst.m_PreviousDriverPressure_cmH2O = src.previousdriverpressure_cmh2o();
    dst.m_DriverFlow_L_Per_s = src.driverflow_l_per_s();
    dst.m_CurrentVentilatorVolume_L = src.currentventilatorvolume_l();
    dst.m_CurrentRespiratoryVolume_L = src.currentrespiratoryvolume_l();
    dst.m_InspirationTime_s = src.inspirationtime_s();
    dst.m_InspiratoryFlow_L_Per_s = src.inspiratoryflow_l_per_s();
    dst.m_PreviousYPieceToConnectionFlow_L_Per_s = src.previousypiecetoconnectionflow_l_per_s();
    dst.m_PreviousConnectionPressure_cmH2O = src.previousconnectionpressure_cmh2o();
    dst.m_PauseOccurred = src.pauseoccurred();
    dst.m_Initializing = src.initializing();
    dst.m_PositiveEndExpiratoryPressure_cmH2O = src.positiveendexpiratorypressure_cmh2o();
    dst.m_PeakExpiratoryFlow_L_Per_s = src.peakexpiratoryflow_l_per_s();
    dst.m_PeakInspiratoryFlow_L_Per_s = src.peakinspiratoryflow_l_per_s();
    dst.m_EndTidalCarbonDioxideFraction = src.endtidalcarbondioxidefraction();
    dst.m_EndTidalCarbonDioxidePressure_cmH2O = src.endtidalcarbondioxidepressure_cmh2o();
    dst.m_EndTidalOxygenFraction = src.endtidaloxygenfraction();
    dst.m_EndTidalOxygenPressure_cmH2O = src.endtidaloxygenpressure_cmh2o();
    PBProperty::Load(src.meanairwaypressure_cmh2o(), *dst.m_MeanAirwayPressure_cmH2O);
  }
  PULSE_BIND::MechanicalVentilatorData* PBEquipment::Unload(const MechanicalVentilatorModel& src)
  {
    PULSE_BIND::MechanicalVentilatorData* dst = new PULSE_BIND::MechanicalVentilatorData();
    PBEquipment::Serialize(src, *dst);
    return dst;
  }
  void PBEquipment::Serialize(const MechanicalVentilatorModel& src, PULSE_BIND::MechanicalVentilatorData& dst)
  {
    PBMechanicalVentilator::Serialize(src, *dst.mutable_common());
    dst.set_currentperiodtime_s(src.m_CurrentPeriodTime_s);
    dst.set_driverpressure_cmh2o(src.m_DriverPressure_cmH2O);
    dst.set_previousdriverflow_l_per_s(src.m_PreviousDriverFlow_L_Per_s);
    dst.set_previousdriverpressure_cmh2o(src.m_PreviousDriverPressure_cmH2O);
    dst.set_driverflow_l_per_s(src.m_DriverFlow_L_Per_s);
    dst.set_currentventilatorvolume_l(src.m_CurrentVentilatorVolume_L);
    dst.set_currentrespiratoryvolume_l(src.m_CurrentRespiratoryVolume_L);
    dst.set_inspirationtime_s(src.m_InspirationTime_s);
    dst.set_inspiratoryflow_l_per_s(src.m_InspiratoryFlow_L_Per_s);
    dst.set_previousypiecetoconnectionflow_l_per_s(src.m_PreviousYPieceToConnectionFlow_L_Per_s);
    dst.set_previousconnectionpressure_cmh2o(src.m_PreviousConnectionPressure_cmH2O);
    dst.set_pauseoccurred(src.m_PauseOccurred);
    dst.set_initializing(src.m_Initializing);
    dst.set_positiveendexpiratorypressure_cmh2o(src.m_PositiveEndExpiratoryPressure_cmH2O);
    dst.set_peakexpiratoryflow_l_per_s(src.m_PeakExpiratoryFlow_L_Per_s);
    dst.set_peakinspiratoryflow_l_per_s(src.m_PeakInspiratoryFlow_L_Per_s);
    dst.set_endtidalcarbondioxidefraction(src.m_EndTidalCarbonDioxideFraction);
    dst.set_endtidalcarbondioxidepressure_cmh2o(src.m_EndTidalCarbonDioxidePressure_cmH2O);
    dst.set_endtidaloxygenfraction(src.m_EndTidalOxygenFraction);
    dst.set_endtidaloxygenpressure_cmh2o(src.m_EndTidalOxygenPressure_cmH2O);
    dst.set_allocated_meanairwaypressure_cmh2o(PBProperty::Unload(*src.m_MeanAirwayPressure_cmH2O));
  }
}
