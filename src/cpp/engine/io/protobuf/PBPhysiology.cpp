/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/CommonDefs.h"
#include "engine/common/controller/CircuitManager.h"
#include "engine/common/controller/CompartmentManager.h"
PUSH_PROTO_WARNINGS
#include "pulse/engine/bind/Physiology.pb.h"
POP_PROTO_WARNINGS
#include "engine/io/protobuf/PBPhysiology.h"

#include "cdm/circuit/fluid/SEFluidCircuit.h"
#include "cdm/compartment/fluid/SELiquidCompartmentGraph.h"
#include "cdm/compartment/fluid/SELiquidCompartment.h"
#include "cdm/io/protobuf/PBPhysiology.h"
#include "cdm/io/protobuf/PBProperties.h"

namespace pulse
{
  void PBPhysiology::Load(const PULSE_BIND::BloodChemistryData& src, BloodChemistryModel& dst)
  {
    dst.Clear();
    dst.SetUp();
    PBPhysiology::Serialize(src, dst);
  }
  void PBPhysiology::Serialize(const PULSE_BIND::BloodChemistryData& src, BloodChemistryModel& dst)
  {
    ::PBPhysiology::Serialize(src.common(), dst);
    if (!src.has_arterialoxygenaverage_mmhg()) { dst.Fatal("Missing ArterialOxygen_mmHg"); }
    PBProperty::Load(src.arterialoxygenaverage_mmhg(), *dst.m_ArterialOxygen_mmHg);
    if (!src.has_arterialcarbondioxideaverage_mmhg()) { dst.Fatal("Missing ArterialCarbonDioxide_mmHg"); }
    PBProperty::Load(src.arterialcarbondioxideaverage_mmhg(), *dst.m_ArterialCarbonDioxide_mmHg);
  }
  PULSE_BIND::BloodChemistryData* PBPhysiology::Unload(const BloodChemistryModel& src)
  {
    PULSE_BIND::BloodChemistryData* dst = new PULSE_BIND::BloodChemistryData();
    PBPhysiology::Serialize(src, *dst);
    return dst;
  }
  void PBPhysiology::Serialize(const BloodChemistryModel& src, PULSE_BIND::BloodChemistryData& dst)
  {
    ::PBPhysiology::Serialize(src, *dst.mutable_common());
    dst.set_allocated_arterialoxygenaverage_mmhg(PBProperty::Unload(*src.m_ArterialOxygen_mmHg));
    dst.set_allocated_arterialcarbondioxideaverage_mmhg(PBProperty::Unload(*src.m_ArterialCarbonDioxide_mmHg));
  }

  void PBPhysiology::Load(const PULSE_BIND::CardiovascularData& src, CardiovascularModel& dst)
  {
    dst.Clear();
    dst.SetUp();
    PBPhysiology::Serialize(src, dst);
  }
  void PBPhysiology::Serialize(const PULSE_BIND::CardiovascularData& src, CardiovascularModel& dst)
  {
    ::PBPhysiology::Serialize(src.common(), dst);
    dst.m_StartSystole = src.startsystole();
    dst.m_HeartFlowDetected = src.heartflowdetected();
    dst.m_FullyCompressedHeart = src.fullycompressedheart();
    dst.m_StabilizedHeartRateBaseline_Per_min = src.stabilizedheartratebaseline_per_min();
    dst.m_StabilizedMeanArterialPressureBaseline_mmHg = src.stabilizedmeanarterialpressurebaseline_mmhg();

    dst.m_CurrentDriverCycleTime_s = src.currentdrivercycletime_s();
    dst.m_DriverCyclePeriod_s = src.drivercycleperiod_s();
    dst.m_LeftHeartElastanceModifier = src.leftheartelastancemodifier();
    dst.m_LeftHeartElastance_mmHg_Per_mL = src.leftheartelastance_mmhg_per_ml();
    dst.m_LeftHeartElastanceMax_mmHg_Per_mL = src.leftheartelastancemax_mmhg_per_ml();
    dst.m_LeftHeartElastanceMin_mmHg_Per_mL = src.leftheartelastancemin_mmhg_per_ml();
    dst.m_RightHeartElastance_mmHg_Per_mL = src.rightheartelastance_mmhg_per_ml();
    dst.m_RightHeartElastanceMax_mmHg_Per_mL = src.rightheartelastancemax_mmhg_per_ml();
    dst.m_RightHeartElastanceMin_mmHg_Per_mL = src.rightheartelastancemin_mmhg_per_ml();

    dst.m_EnableFeedbackAfterArrhythmiaTrasition = (eSwitch)src.enablefeedbackafterarrhythmiatrasition();
    dst.m_StartCardiacArrest = src.startcardiacarrest();
    dst.m_TransitionArrhythmia = src.transitionarrhythmia();
    dst.m_CardiacArrestVitalsUpdateTimer_s = src.cardiacarrestvitalsupdatetimer_s();

    PBProperty::Load(src.heartratebaseline_per_min(), *dst.m_HeartRateBaseline_Per_min);
    PBProperty::Load(src.heartcompliancemodifier(), *dst.m_HeartComplianceModifier);
    PBProperty::Load(src.aortacompliancemodifier(), *dst.m_AortaComplianceModifier);
    PBProperty::Load(src.venacavacompliancemodifier(), *dst.m_VenaCavaComplianceModifier);
    PBProperty::Load(src.pulmonarycompliancemodifier(), *dst.m_RespiratoryComplianceModifier);
    PBProperty::Load(src.systemicvascularresistancemodifier(), *dst.m_SystemicVascularResistanceModifier);
    PBProperty::Load(src.systemicvascularcompliancemodifier(), *dst.m_SystemicVascularComplianceModifier);

    dst.m_CompressionFrequencyCurrentTime_s = src.compressionfrequencycurrenttime_s();
    dst.m_CompressionFrequencyDuration_s = src.compressionfrequencyduration_s();
    dst.m_CompressionPeriod_s = src.compressionperiod_s();
    dst.m_CompressionPeriodCurrentTime_s = src.compressionperiodcurrenttime_s();

    dst.m_CardiacCycleDiastolicVolume_mL = src.cardiaccyclediastolicvolume_ml();
    dst.m_CardiacCycleAortaPressureLow_mmHg = src.cardiaccycleaortapressurelow_mmhg();
    dst.m_CardiacCycleAortaPressureHigh_mmHg = src.cardiaccycleaortapressurehigh_mmhg();
    dst.m_CardiacCycleLeftHeartPressureLow_mmHg = src.cardiaccycleleftheartpressurelow_mmhg();
    dst.m_CardiacCycleLeftHeartPressureHigh_mmHg = src.cardiaccycleleftheartpressurehigh_mmhg();
    dst.m_CardiacCyclePulmonaryArteryPressureLow_mmHg = src.cardiaccyclepulmonaryarterypressurelow_mmhg();
    dst.m_CardiacCyclePulmonaryArteryPressureHigh_mmHg = src.cardiaccyclepulmonaryarterypressurehigh_mmhg();
    dst.m_CardiacCycleRightHeartPressureLow_mmHg = src.cardiaccyclerightheartpressurelow_mmhg();
    dst.m_CardiacCycleRightHeartPressureHigh_mmHg = src.cardiaccyclerightheartpressurehigh_mmhg();
    dst.m_LastCardiacCycleMeanArterialCO2PartialPressure_mmHg = src.lastcardiaccyclemeanarterialco2partialpressure_mmhg();
    dst.m_CardiacCycleStrokeVolume_mL = src.cardiaccyclestrokevolume_ml();

    for (int i = 0; i < src.leftcardiaccycleperfusionvolumes_ml_size(); i++)
      dst.m_LeftCardiacCyclePerfusionVolumes_mL.push_back(src.leftcardiaccycleperfusionvolumes_ml()[i]);
    for (int i = 0; i < src.rightcardiaccycleperfusionvolumes_ml_size(); i++)
      dst.m_RightCardiacCyclePerfusionVolumes_mL.push_back(src.rightcardiaccycleperfusionvolumes_ml()[i]);

    dst.m_PeripheralVolumeHigh_mL = src.peripheralvolumehigh_ml();
    dst.m_PeripheralVolumeLow_mL = src.peripheralvolumelow_ml();

    PBProperty::Load(src.cardiaccyclearterialpressure_mmhg(), *dst.m_CardiacCycleArterialPressure_mmHg);
    PBProperty::Load(src.cardiaccyclearterialco2partialpressure_mmhg(), *dst.m_CardiacCycleArterialCO2PartialPressure_mmHg);
    PBProperty::Load(src.cardiaccyclepulmonarycapillarieswedgepressure_mmhg(), *dst.m_CardiacCyclePulmonaryCapillariesWedgePressure_mmHg);
    PBProperty::Load(src.cardiaccyclepulmonarycapillariesflow_ml_per_s(), *dst.m_CardiacCyclePulmonaryCapillariesFlow_mL_Per_s);
    PBProperty::Load(src.cardiaccyclepulmonaryshuntflow_ml_per_s(), *dst.m_CardiacCyclePulmonaryShuntFlow_mL_Per_s);
    PBProperty::Load(src.cardiaccyclepulmonaryarterypressure_mmhg(), *dst.m_CardiacCyclePulmonaryArteryPressure_mmHg);
    PBProperty::Load(src.cardiaccyclecentralvenouspressure_mmhg(), *dst.m_CardiacCycleCentralVenousPressure_mmHg);
    PBProperty::Load(src.cardiaccycleskinflow_ml_per_s(), *dst.m_CardiacCycleSkinFlow_mL_Per_s);
  }
  PULSE_BIND::CardiovascularData* PBPhysiology::Unload(const CardiovascularModel& src)
  {
    PULSE_BIND::CardiovascularData* dst = new PULSE_BIND::CardiovascularData();
    PBPhysiology::Serialize(src, *dst);
    return dst;
  }
  void PBPhysiology::Serialize(const CardiovascularModel& src, PULSE_BIND::CardiovascularData& dst)
  {
    ::PBPhysiology::Serialize(src, *dst.mutable_common());
    dst.set_startsystole(src.m_StartSystole);
    dst.set_heartflowdetected(src.m_HeartFlowDetected);
    dst.set_fullycompressedheart(src.m_FullyCompressedHeart);
    dst.set_stabilizedheartratebaseline_per_min(src.m_StabilizedHeartRateBaseline_Per_min);
    dst.set_stabilizedmeanarterialpressurebaseline_mmhg(src.m_StabilizedMeanArterialPressureBaseline_mmHg);
    dst.set_currentdrivercycletime_s(src.m_CurrentDriverCycleTime_s);
    dst.set_drivercycleperiod_s(src.m_DriverCyclePeriod_s);
    dst.set_leftheartelastance_mmhg_per_ml(src.m_LeftHeartElastance_mmHg_Per_mL);
    dst.set_leftheartelastancemodifier(src.m_LeftHeartElastanceModifier);
    dst.set_leftheartelastancemax_mmhg_per_ml(src.m_LeftHeartElastanceMax_mmHg_Per_mL);
    dst.set_leftheartelastancemin_mmhg_per_ml(src.m_LeftHeartElastanceMin_mmHg_Per_mL);
    dst.set_rightheartelastance_mmhg_per_ml(src.m_RightHeartElastance_mmHg_Per_mL);
    dst.set_rightheartelastancemax_mmhg_per_ml(src.m_RightHeartElastanceMax_mmHg_Per_mL);
    dst.set_rightheartelastancemin_mmhg_per_ml(src.m_RightHeartElastanceMin_mmHg_Per_mL);

    dst.set_enablefeedbackafterarrhythmiatrasition((CDM_BIND::eSwitch)src.m_EnableFeedbackAfterArrhythmiaTrasition);
    dst.set_startcardiacarrest(src.m_StartCardiacArrest);
    dst.set_transitionarrhythmia(src.m_TransitionArrhythmia);
    dst.set_cardiacarrestvitalsupdatetimer_s(src.m_CardiacArrestVitalsUpdateTimer_s);

    dst.set_allocated_heartratebaseline_per_min(PBProperty::Unload(*src.m_HeartRateBaseline_Per_min));
    dst.set_allocated_heartcompliancemodifier(PBProperty::Unload(*src.m_HeartComplianceModifier));
    dst.set_allocated_aortacompliancemodifier(PBProperty::Unload(*src.m_AortaComplianceModifier));
    dst.set_allocated_venacavacompliancemodifier(PBProperty::Unload(*src.m_VenaCavaComplianceModifier));
    dst.set_allocated_pulmonarycompliancemodifier(PBProperty::Unload(*src.m_RespiratoryComplianceModifier));
    dst.set_allocated_systemicvascularresistancemodifier(PBProperty::Unload(*src.m_SystemicVascularResistanceModifier));
    dst.set_allocated_systemicvascularcompliancemodifier(PBProperty::Unload(*src.m_SystemicVascularComplianceModifier));

    dst.set_compressionfrequencycurrenttime_s(src.m_CompressionFrequencyCurrentTime_s);
    dst.set_compressionfrequencyduration_s(src.m_CompressionFrequencyDuration_s);
    dst.set_compressionperiod_s(src.m_CompressionPeriod_s);
    dst.set_compressionperiodcurrenttime_s(src.m_CompressionPeriodCurrentTime_s);

    dst.set_cardiaccyclediastolicvolume_ml(src.m_CardiacCycleDiastolicVolume_mL);
    dst.set_cardiaccycleaortapressurelow_mmhg(src.m_CardiacCycleAortaPressureLow_mmHg);
    dst.set_cardiaccycleaortapressurehigh_mmhg(src.m_CardiacCycleAortaPressureHigh_mmHg);
    dst.set_cardiaccycleleftheartpressurelow_mmhg(src.m_CardiacCycleLeftHeartPressureLow_mmHg);
    dst.set_cardiaccycleleftheartpressurehigh_mmhg(src.m_CardiacCycleLeftHeartPressureHigh_mmHg);
    dst.set_cardiaccyclepulmonaryarterypressurelow_mmhg(src.m_CardiacCyclePulmonaryArteryPressureLow_mmHg);
    dst.set_cardiaccyclepulmonaryarterypressurehigh_mmhg(src.m_CardiacCyclePulmonaryArteryPressureHigh_mmHg);
    dst.set_cardiaccyclerightheartpressurelow_mmhg(src.m_CardiacCycleRightHeartPressureLow_mmHg);
    dst.set_cardiaccyclerightheartpressurehigh_mmhg(src.m_CardiacCycleRightHeartPressureHigh_mmHg);
    dst.set_lastcardiaccyclemeanarterialco2partialpressure_mmhg(src.m_LastCardiacCycleMeanArterialCO2PartialPressure_mmHg);
    dst.set_cardiaccyclestrokevolume_ml(src.m_CardiacCycleStrokeVolume_mL);

    for (double d : src.m_LeftCardiacCyclePerfusionVolumes_mL)
      dst.mutable_leftcardiaccycleperfusionvolumes_ml()->Add(d);
    for (double d : src.m_RightCardiacCyclePerfusionVolumes_mL)
      dst.mutable_rightcardiaccycleperfusionvolumes_ml()->Add(d);

    dst.set_peripheralvolumehigh_ml(src.m_PeripheralVolumeHigh_mL);
    dst.set_peripheralvolumelow_ml(src.m_PeripheralVolumeLow_mL);

    dst.set_allocated_cardiaccyclearterialpressure_mmhg(PBProperty::Unload(*src.m_CardiacCycleArterialPressure_mmHg));
    dst.set_allocated_cardiaccyclearterialco2partialpressure_mmhg(PBProperty::Unload(*src.m_CardiacCycleArterialCO2PartialPressure_mmHg));
    dst.set_allocated_cardiaccyclepulmonarycapillarieswedgepressure_mmhg(PBProperty::Unload(*src.m_CardiacCyclePulmonaryCapillariesWedgePressure_mmHg));
    dst.set_allocated_cardiaccyclepulmonarycapillariesflow_ml_per_s(PBProperty::Unload(*src.m_CardiacCyclePulmonaryCapillariesFlow_mL_Per_s));
    dst.set_allocated_cardiaccyclepulmonaryshuntflow_ml_per_s(PBProperty::Unload(*src.m_CardiacCyclePulmonaryShuntFlow_mL_Per_s));
    dst.set_allocated_cardiaccyclepulmonaryarterypressure_mmhg(PBProperty::Unload(*src.m_CardiacCyclePulmonaryArteryPressure_mmHg));
    dst.set_allocated_cardiaccyclecentralvenouspressure_mmhg(PBProperty::Unload(*src.m_CardiacCycleCentralVenousPressure_mmHg));
    dst.set_allocated_cardiaccycleskinflow_ml_per_s(PBProperty::Unload(*src.m_CardiacCycleSkinFlow_mL_Per_s));
  }

  void PBPhysiology::Load(const PULSE_BIND::DrugData& src, DrugModel& dst)
  {
    dst.Clear();
    dst.SetUp();
    PBPhysiology::Serialize(src, dst);
  }
  void PBPhysiology::Serialize(const PULSE_BIND::DrugData& src, DrugModel& dst)
  {
    ::PBPhysiology::Serialize(src.common(), dst);
  }
  PULSE_BIND::DrugData* PBPhysiology::Unload(const DrugModel& src)
  {
    PULSE_BIND::DrugData* dst = new PULSE_BIND::DrugData();
    PBPhysiology::Serialize(src, *dst);
    return dst;
  }
  void PBPhysiology::Serialize(const DrugModel& src, PULSE_BIND::DrugData& dst)
  {
    ::PBPhysiology::Serialize(src, *dst.mutable_common());
  }

  void PBPhysiology::Load(const PULSE_BIND::EndocrineData& src, EndocrineModel& dst)
  {
    dst.Clear();
    dst.SetUp();
    PBPhysiology::Serialize(src, dst);
  }
  void PBPhysiology::Serialize(const PULSE_BIND::EndocrineData& src, EndocrineModel& dst)
  {
    ::PBPhysiology::Serialize(src.common(), dst);
  }
  PULSE_BIND::EndocrineData* PBPhysiology::Unload(const EndocrineModel& src)
  {
    PULSE_BIND::EndocrineData* dst = new PULSE_BIND::EndocrineData();
    PBPhysiology::Serialize(src, *dst);
    return dst;
  }
  void PBPhysiology::Serialize(const EndocrineModel& src, PULSE_BIND::EndocrineData& dst)
  {
    ::PBPhysiology::Serialize(src, *dst.mutable_common());
  }

  void PBPhysiology::Load(const PULSE_BIND::EnergyData& src, EnergyModel& dst)
  {
    dst.Clear();
    dst.SetUp();
    PBPhysiology::Serialize(src, dst);
  }
  void PBPhysiology::Serialize(const PULSE_BIND::EnergyData& src, EnergyModel& dst)
  {
    ::PBPhysiology::Serialize(src.common(), dst);
    dst.m_UsableEnergyStore_J = src.usableenergystore_j();
    dst.m_PeakPowerEnergyStore_J = src.peakpowerenergystore_j();
    dst.m_MediumPowerEnergyStore_J = src.mediumpowerenergystore_j();
    dst.m_EnduranceEnergyStore_J = src.enduranceenergystore_j();

    PBProperty::Load(src.bloodph(), *dst.m_BloodpH);
    PBProperty::Load(src.bicarbonatemolarity_mmol_per_l(), *dst.m_BicarbonateMolarity_mmol_Per_L);
  }
  PULSE_BIND::EnergyData* PBPhysiology::Unload(const EnergyModel& src)
  {
    PULSE_BIND::EnergyData* dst = new PULSE_BIND::EnergyData();
    PBPhysiology::Serialize(src, *dst);
    return dst;
  }
  void PBPhysiology::Serialize(const EnergyModel& src, PULSE_BIND::EnergyData& dst)
  {
    ::PBPhysiology::Serialize(src, *dst.mutable_common());
    dst.set_usableenergystore_j(src.m_UsableEnergyStore_J);
    dst.set_peakpowerenergystore_j(src.m_PeakPowerEnergyStore_J);
    dst.set_mediumpowerenergystore_j(src.m_MediumPowerEnergyStore_J);
    dst.set_enduranceenergystore_j(src.m_EnduranceEnergyStore_J);

    dst.set_allocated_bloodph(PBProperty::Unload(*src.m_BloodpH));
    dst.set_allocated_bicarbonatemolarity_mmol_per_l(PBProperty::Unload(*src.m_BicarbonateMolarity_mmol_Per_L));
  }

  void PBPhysiology::Load(const PULSE_BIND::GastrointestinalData& src, GastrointestinalModel& dst)
  {
    dst.Clear();
    dst.SetUp();
    PBPhysiology::Serialize(src, dst);
  }
  void PBPhysiology::Serialize(const PULSE_BIND::GastrointestinalData& src, GastrointestinalModel& dst)
  {
    ::PBPhysiology::Serialize(src.common(), dst);
  }
  PULSE_BIND::GastrointestinalData* PBPhysiology::Unload(const GastrointestinalModel& src)
  {
    PULSE_BIND::GastrointestinalData* dst = new PULSE_BIND::GastrointestinalData();
    PBPhysiology::Serialize(src, *dst);
    return dst;
  }
  void PBPhysiology::Serialize(const GastrointestinalModel& src, PULSE_BIND::GastrointestinalData& dst)
  {
    ::PBPhysiology::Serialize(src, *dst.mutable_common());
  }

  void PBPhysiology::Load(const PULSE_BIND::HepaticData& src, HepaticModel& dst)
  {
    dst.Clear();
    dst.SetUp();
    PBPhysiology::Serialize(src, dst);
  }
  void PBPhysiology::Serialize(const PULSE_BIND::HepaticData& src, HepaticModel& dst)
  {
    ::PBPhysiology::Serialize(src.common(), dst);
  }
  PULSE_BIND::HepaticData* PBPhysiology::Unload(const HepaticModel& src)
  {
    PULSE_BIND::HepaticData* dst = new PULSE_BIND::HepaticData();
    PBPhysiology::Serialize(src, *dst);
    return dst;
  }
  void PBPhysiology::Serialize(const HepaticModel& src, PULSE_BIND::HepaticData& dst)
  {
    ::PBPhysiology::Serialize(src, *dst.mutable_common());
  }

  void PBPhysiology::Load(const PULSE_BIND::NervousData& src, NervousModel& dst)
  {
    dst.Clear();
    dst.SetUp();
    PBPhysiology::Serialize(src, dst);
  }
  void PBPhysiology::Serialize(const PULSE_BIND::NervousData& src, NervousModel& dst)
  {
    ::PBPhysiology::Serialize(src.common(), dst);
    // We assume state have to be after all stabilization
    dst.m_BaroreceptorFeedbackStatus = src.baroreceptorfeedbackstatus();
    dst.m_BaroreceptorSaturationStatus = src.baroreceptorsaturationstatus();
    dst.m_ArterialOxygenBaseline_mmHg = src.arterialoxygenbaseline_mmhg();
    dst.m_ArterialCarbonDioxideBaseline_mmHg = src.arterialcarbondioxidebaseline_mmhg();
    dst.m_BaroreceptorActiveTime_s = src.baroreceptoractivetime_s();
    dst.m_BaroreceptorEffectivenessParameter = src.baroreceptoreffectivenessparameter();
    dst.m_BaroreceptorSaturationTime_s = src.baroreceptorsaturationtime_s();
    dst.m_LastMeanArterialPressure_mmHg = src.lastmeanarterialpressure_mmhg();
    dst.m_PreviousBloodVolume_mL = src.previousbloodvolume_ml();
    dst.m_PreviousHeartRhythm = (eHeartRhythm)src.previousheartrhythm();
  }
  PULSE_BIND::NervousData* PBPhysiology::Unload(const NervousModel& src)
  {
    PULSE_BIND::NervousData* dst = new PULSE_BIND::NervousData();
    PBPhysiology::Serialize(src, *dst);
    return dst;
  }
  void PBPhysiology::Serialize(const NervousModel& src, PULSE_BIND::NervousData& dst)
  {
    ::PBPhysiology::Serialize(src, *dst.mutable_common());
    dst.set_baroreceptorfeedbackstatus(src.m_BaroreceptorFeedbackStatus);
    dst.set_baroreceptorsaturationstatus(src.m_BaroreceptorSaturationStatus);
    dst.set_arterialoxygenbaseline_mmhg(src.m_ArterialOxygenBaseline_mmHg);
    dst.set_arterialcarbondioxidebaseline_mmhg(src.m_ArterialCarbonDioxideBaseline_mmHg);
    dst.set_baroreceptoractivetime_s(src.m_BaroreceptorActiveTime_s);
    dst.set_baroreceptoreffectivenessparameter(src.m_BaroreceptorEffectivenessParameter);
    dst.set_baroreceptorsaturationtime_s(src.m_BaroreceptorSaturationTime_s);
    dst.set_lastmeanarterialpressure_mmhg(src.m_LastMeanArterialPressure_mmHg);
    dst.set_previousbloodvolume_ml(src.m_PreviousBloodVolume_mL);
    dst.set_previousheartrhythm((CDM_BIND::eHeartRhythm)src.m_PreviousHeartRhythm);
  }

  void PBPhysiology::Load(const PULSE_BIND::RenalData& src, RenalModel& dst)
  {
    dst.Clear();
    dst.SetUp();
    PBPhysiology::Serialize(src, dst);
  }
  void PBPhysiology::Serialize(const PULSE_BIND::RenalData& src, RenalModel& dst)
  {
    ::PBPhysiology::Serialize(src.common(), dst);
    dst.m_Urinating = src.urinating();
    dst.m_leftAfferentResistance_mmHg_s_Per_mL = src.leftafferentresistance_mmhg_s_per_ml();
    dst.m_rightAfferentResistance_mmHg_s_Per_mL = src.rightafferentresistance_mmhg_s_per_ml();
    dst.m_leftSodiumFlowSetPoint_mg_Per_s = src.leftsodiumflowsetpoint_mg_per_s();
    dst.m_rightSodiumFlowSetPoint_mg_Per_s = src.rightsodiumflowsetpoint_mg_per_s();

    PBProperty::Load(src.urineproductionrate_ml_per_min(), *dst.m_urineProductionRate_mL_Per_min_runningAvg);
    PBProperty::Load(src.urineosmolarity_mosm_per_l(), *dst.m_urineOsmolarity_mOsm_Per_L_runningAvg);
    PBProperty::Load(src.sodiumconcentration_mg_per_ml(), *dst.m_sodiumConcentration_mg_Per_mL_runningAvg);
    PBProperty::Load(src.sodiumexcretionrate_mg_per_min(), *dst.m_sodiumExcretionRate_mg_Per_min_runningAvg);
    PBProperty::Load(src.leftsodiumflow_mg_per_s(), *dst.m_leftSodiumFlow_mg_Per_s_runningAvg);
    PBProperty::Load(src.rightsodiumflow_mg_per_s(), *dst.m_rightSodiumFlow_mg_Per_s_runningAvg);
    PBProperty::Load(src.leftrenalarterialpressure_mmhg(), *dst.m_leftRenalArterialPressure_mmHg_runningAvg);
    PBProperty::Load(src.rightrenalarterialpressure_mmhg(), *dst.m_rightRenalArterialPressure_mmHg_runningAvg);
  }
  PULSE_BIND::RenalData* PBPhysiology::Unload(const RenalModel& src)
  {
    PULSE_BIND::RenalData* dst = new PULSE_BIND::RenalData();
    PBPhysiology::Serialize(src, *dst);
    return dst;
  }
  void PBPhysiology::Serialize(const RenalModel& src, PULSE_BIND::RenalData& dst)
  {
    ::PBPhysiology::Serialize(src, *dst.mutable_common());
    dst.set_urinating(src.m_Urinating);
    dst.set_leftafferentresistance_mmhg_s_per_ml(src.m_leftAfferentResistance_mmHg_s_Per_mL);
    dst.set_rightafferentresistance_mmhg_s_per_ml(src.m_rightAfferentResistance_mmHg_s_Per_mL);
    dst.set_leftsodiumflowsetpoint_mg_per_s(src.m_leftSodiumFlowSetPoint_mg_Per_s);
    dst.set_rightsodiumflowsetpoint_mg_per_s(src.m_rightSodiumFlowSetPoint_mg_Per_s);

    dst.set_allocated_urineproductionrate_ml_per_min(PBProperty::Unload(*src.m_urineProductionRate_mL_Per_min_runningAvg));
    dst.set_allocated_urineosmolarity_mosm_per_l(PBProperty::Unload(*src.m_urineOsmolarity_mOsm_Per_L_runningAvg));
    dst.set_allocated_sodiumconcentration_mg_per_ml(PBProperty::Unload(*src.m_sodiumConcentration_mg_Per_mL_runningAvg));
    dst.set_allocated_sodiumexcretionrate_mg_per_min(PBProperty::Unload(*src.m_sodiumExcretionRate_mg_Per_min_runningAvg));
    dst.set_allocated_leftsodiumflow_mg_per_s(PBProperty::Unload(*src.m_leftSodiumFlow_mg_Per_s_runningAvg));
    dst.set_allocated_rightsodiumflow_mg_per_s(PBProperty::Unload(*src.m_rightSodiumFlow_mg_Per_s_runningAvg));
    dst.set_allocated_leftrenalarterialpressure_mmhg(PBProperty::Unload(*src.m_leftRenalArterialPressure_mmHg_runningAvg));
    dst.set_allocated_rightrenalarterialpressure_mmhg(PBProperty::Unload(*src.m_rightRenalArterialPressure_mmHg_runningAvg));
  }

  void PBPhysiology::Load(const PULSE_BIND::RespiratoryData& src, RespiratoryModel& dst)
  {
    dst.Clear();
    dst.SetUp();
    PBPhysiology::Serialize(src, dst);
  }
  void PBPhysiology::Serialize(const PULSE_BIND::RespiratoryData& src, RespiratoryModel& dst)
  {
    ::PBPhysiology::Serialize(src.common(), dst);

    dst.m_BreathingCycle = src.breathingcycle();
    dst.m_NotBreathing = src.notbreathing();

    dst.m_TopBreathTotalVolume_L = src.topbreathtotalvolume_l();
    dst.m_LastCardiacCycleBloodPH = src.lastcardiaccyclebloodph();
    dst.m_TopCarinaO2 = src.topcarinao2();
    dst.m_TopBreathElapsedTime_min = src.topbreathelapsedtime_min();
    dst.m_BottomBreathElapsedTime_min = src.bottombreathelapsedtime_min();
    dst.m_BottomBreathTotalVolume_L = src.bottombreathtotalvolume_l();
    dst.m_BottomBreathAlveoliPressure_cmH2O = src.bottombreathalveolipressure_cmh2o();
    dst.m_BottomBreathAirwayPressure_cmH2O = src.bottombreathairwaypressure_cmh2o();
    dst.m_PeakAlveolarPressure_cmH2O = src.peakalveolarpressure_cmh2o();
    dst.m_MaximalAlveolarPressure_cmH2O = src.maximalalveolarpressure_cmh2o();
    PBProperty::Load(src.bloodphrunningaverage(), *dst.m_BloodPHRunningAverage);
    PBProperty::Load(src.meanairwaypressure_cmh2o(), *dst.m_MeanAirwayPressure_cmH2O);

    for(int i=0; i<src.alveolivolumeincrement_l_size(); i++)
      dst.m_AlveoliVolumeIncrement_L.push_back(src.alveolivolumeincrement_l()[i]);
    for (int i=0; i<src.topbreathacinarzonevolumes_l_size(); i++)
      dst.m_TopBreathAcinarZoneVolumes_L.push_back(src.topbreathacinarzonevolumes_l()[i]);
    for (int i=0; i<src.bottombreathacinarzonevolumes_l_size(); i++)
      dst.m_BottomBreathAcinarZoneVolumes_L.push_back(src.bottombreathacinarzonevolumes_l()[i]);
    dst.m_PreviousPleuralVolume_L = src.previouspleuralvolume_l();

    dst.m_ArterialO2PartialPressure_mmHg = src.arterialo2partialpressure_mmhg();
    dst.m_ArterialCO2PartialPressure_mmHg = src.arterialco2partialpressure_mmhg();
    dst.m_BreathingCycleTime_s = src.breathingcycletime_s();
    dst.m_DriverPressure_cmH2O = src.driverpressure_cmh2o();
    dst.m_ElapsedBreathingCycleTime_min = src.elapsedbreathingcycletime_min();
    dst.m_IERatioScaleFactor = src.ieratioscalefactor();
    dst.m_PeakInspiratoryPressure_cmH2O = src.peakinspiratorypressure_cmh2o();
    dst.m_PeakExpiratoryPressure_cmH2O = src.peakexpiratorypressure_cmh2o();
    dst.m_PreviousTargetAlveolarVentilation_L_Per_min = src.previoustargetalveolarventilation_l_per_min();
    dst.m_VentilationFrequency_Per_min = src.ventilationfrequency_per_min();
    dst.m_VentilationPeriod_s = src.ventilationperiod_s();
    dst.m_VentilationToTidalVolumeSlope = src.ventilationtotidalvolumeslope();
    PBProperty::Load(src.arterialo2runningaverage_mmhg(), *dst.m_ArterialO2RunningAverage_mmHg);
    PBProperty::Load(src.arterialco2runningaverage_mmhg(), *dst.m_ArterialCO2RunningAverage_mmHg);

    dst.m_ExpiratoryHoldFraction = src.expiratoryholdfraction();
    dst.m_ExpiratoryReleaseFraction = src.expiratoryreleasefraction();
    dst.m_ExpiratoryRiseFraction = src.expiratoryrisefraction();
    dst.m_InspiratoryHoldFraction = src.inspiratoryholdfraction();
    dst.m_InspiratoryReleaseFraction = src.inspiratoryreleasefraction();
    dst.m_InspiratoryRiseFraction = src.inspiratoryrisefraction();
    dst.m_InspiratoryToExpiratoryPauseFraction = src.inspiratorytoexpiratorypausefraction();
    dst.m_ResidueFraction = src.residuefraction();
    dst.m_PreviousDyspneaSeverity = src.previousdyspneaseverity();

    dst.m_LeftAlveoliDecrease_L = src.leftalveolidecrease_l();
    dst.m_RightAlveoliDecrease_L = src.rightalveolidecrease_l();

    dst.m_ActiveConsciousRespirationCommand = src.activeconsciousrespirationcommand();

    dst.m_RespiratoryComplianceOverride_L_Per_cmH2O = src.respiratorycomplianceoverride_l_per_cmh2o();
    dst.m_RespiratoryResistanceOverride_cmH2O_s_Per_L = src.respiratoryresistanceoverride_cmh2o_s_per_l();
  }
  PULSE_BIND::RespiratoryData* PBPhysiology::Unload(const RespiratoryModel& src)
  {
    PULSE_BIND::RespiratoryData* dst = new PULSE_BIND::RespiratoryData();
    PBPhysiology::Serialize(src, *dst);
    return dst;
  }
  void PBPhysiology::Serialize(const RespiratoryModel& src, PULSE_BIND::RespiratoryData& dst)
  {
    ::PBPhysiology::Serialize(src, *dst.mutable_common());

    dst.set_breathingcycle(src.m_BreathingCycle);
    dst.set_notbreathing(src.m_NotBreathing);

    dst.set_topbreathtotalvolume_l(src.m_TopBreathTotalVolume_L);
    dst.set_lastcardiaccyclebloodph(src.m_LastCardiacCycleBloodPH);
    dst.set_topcarinao2(src.m_TopCarinaO2);
    dst.set_topbreathelapsedtime_min(src.m_TopBreathElapsedTime_min);
    dst.set_bottombreathelapsedtime_min(src.m_BottomBreathElapsedTime_min);
    dst.set_bottombreathtotalvolume_l(src.m_BottomBreathTotalVolume_L);
    dst.set_bottombreathairwaypressure_cmh2o(src.m_BottomBreathAirwayPressure_cmH2O);
    dst.set_bottombreathalveolipressure_cmh2o(src.m_BottomBreathAlveoliPressure_cmH2O);
    dst.set_peakalveolarpressure_cmh2o(src.m_PeakAlveolarPressure_cmH2O);
    dst.set_maximalalveolarpressure_cmh2o(src.m_MaximalAlveolarPressure_cmH2O);
    dst.set_allocated_bloodphrunningaverage(PBProperty::Unload(*src.m_BloodPHRunningAverage));
    dst.set_allocated_meanairwaypressure_cmh2o(PBProperty::Unload(*src.m_MeanAirwayPressure_cmH2O));

    for(double d : src.m_AlveoliVolumeIncrement_L)
      dst.mutable_alveolivolumeincrement_l()->Add(d);
    for (double d : src.m_TopBreathAcinarZoneVolumes_L)
      dst.mutable_topbreathacinarzonevolumes_l()->Add(d);
    for (double d : src.m_BottomBreathAcinarZoneVolumes_L)
      dst.mutable_bottombreathacinarzonevolumes_l()->Add(d);
    dst.set_previouspleuralvolume_l(src.m_PreviousPleuralVolume_L);

    dst.set_arterialo2partialpressure_mmhg(src.m_ArterialO2PartialPressure_mmHg);
    dst.set_arterialco2partialpressure_mmhg(src.m_ArterialCO2PartialPressure_mmHg);
    dst.set_breathingcycletime_s(src.m_BreathingCycleTime_s);
    dst.set_driverpressure_cmh2o(src.m_DriverPressure_cmH2O);
    dst.set_elapsedbreathingcycletime_min(src.m_ElapsedBreathingCycleTime_min);
    dst.set_ieratioscalefactor(src.m_IERatioScaleFactor);
    dst.set_peakinspiratorypressure_cmh2o(src.m_PeakInspiratoryPressure_cmH2O);
    dst.set_peakexpiratorypressure_cmh2o(src.m_PeakExpiratoryPressure_cmH2O);
    dst.set_previoustargetalveolarventilation_l_per_min(src.m_PreviousTargetAlveolarVentilation_L_Per_min);
    dst.set_ventilationfrequency_per_min(src.m_VentilationFrequency_Per_min);
    dst.set_ventilationperiod_s(src.m_VentilationPeriod_s);
    dst.set_ventilationtotidalvolumeslope(src.m_VentilationToTidalVolumeSlope);
    dst.set_allocated_arterialo2runningaverage_mmhg(PBProperty::Unload(*src.m_ArterialO2RunningAverage_mmHg));
    dst.set_allocated_arterialco2runningaverage_mmhg(PBProperty::Unload(*src.m_ArterialCO2RunningAverage_mmHg));

    dst.set_expiratoryholdfraction(src.m_ExpiratoryHoldFraction);
    dst.set_expiratoryreleasefraction(src.m_ExpiratoryReleaseFraction);
    dst.set_expiratoryrisefraction(src.m_ExpiratoryRiseFraction);
    dst.set_inspiratoryholdfraction(src.m_InspiratoryHoldFraction);
    dst.set_inspiratoryreleasefraction(src.m_InspiratoryReleaseFraction);
    dst.set_inspiratoryrisefraction(src.m_InspiratoryRiseFraction);
    dst.set_inspiratorytoexpiratorypausefraction(src.m_InspiratoryToExpiratoryPauseFraction);
    dst.set_residuefraction(src.m_ResidueFraction);
    dst.set_previousdyspneaseverity(src.m_PreviousDyspneaSeverity);

    dst.set_activeconsciousrespirationcommand(src.m_ActiveConsciousRespirationCommand);

    dst.set_leftalveolidecrease_l(src.m_LeftAlveoliDecrease_L);
    dst.set_rightalveolidecrease_l(src.m_RightAlveoliDecrease_L);

    dst.set_respiratorycomplianceoverride_l_per_cmh2o(src.m_RespiratoryComplianceOverride_L_Per_cmH2O);
    dst.set_respiratoryresistanceoverride_cmh2o_s_per_l(src.m_RespiratoryResistanceOverride_cmH2O_s_Per_L);
  }

  void PBPhysiology::Load(const PULSE_BIND::TissueData& src, TissueModel& dst)
  {
    dst.Clear();
    dst.SetUp();
    PBPhysiology::Serialize(src, dst);
  }
  void PBPhysiology::Serialize(const PULSE_BIND::TissueData& src, TissueModel& dst)
  {
    ::PBPhysiology::Serialize(src.common(), dst);
    //dst.m_CardiacArrestCarbonDioxideProductionFactor = src.cardiacarrestcarbondioxideproductionfactor();
    dst.m_RestingTissueGlucose_g = src.restingtissueglucose_g();
    dst.m_RestingBloodGlucose_mg_Per_mL = src.restingbloodglucose_mg_per_ml();
    dst.m_RestingBloodLipid_mg_Per_mL = src.restingbloodlipid_mg_per_ml();
    dst.m_RestingBloodInsulin_mg_Per_mL = src.restingbloodinsulin_mg_per_ml();
    dst.m_RestingFluidMass_kg = src.restingfluidmass_kg();
    dst.m_PreviousFluidMass_kg = src.previousfluidmass_kg();
    for (auto sitr : src.cardiacarrestvascularflows_ml_per_min())
    {
      bool found = false;
      for (auto ditr : dst.m_CardiacArrestVascularFlows_ml_per_min)
      {
        if (ditr.first->GetName() == sitr.first)
        {
          found = true;
          dst.m_CardiacArrestVascularFlows_ml_per_min[ditr.first] = sitr.second;
          break;
        }
      }
      if(!found)
        dst.Warning("Unknown vasculature cmpt in Tissue::CardiacArrestVascularFlows_ml_per_min : " + sitr.first);
    }
  }
  PULSE_BIND::TissueData* PBPhysiology::Unload(const TissueModel& src)
  {
    PULSE_BIND::TissueData* dst = new PULSE_BIND::TissueData();
    PBPhysiology::Serialize(src, *dst);
    return dst;
  }
  void PBPhysiology::Serialize(const TissueModel& src, PULSE_BIND::TissueData& dst)
  {
    ::PBPhysiology::Serialize(src, *dst.mutable_common());
    dst.set_restingtissueglucose_g(src.m_RestingTissueGlucose_g);
    dst.set_restingbloodglucose_mg_per_ml(src.m_RestingBloodGlucose_mg_Per_mL);
    dst.set_restingbloodlipid_mg_per_ml(src.m_RestingBloodLipid_mg_Per_mL);
    dst.set_restingbloodinsulin_mg_per_ml(src.m_RestingBloodInsulin_mg_Per_mL);
    dst.set_restingfluidmass_kg(src.m_RestingFluidMass_kg);
    dst.set_previousfluidmass_kg(src.m_PreviousFluidMass_kg);
    for (auto ditr : src.m_CardiacArrestVascularFlows_ml_per_min)
      (*dst.mutable_cardiacarrestvascularflows_ml_per_min())[ditr.first->GetName()] = ditr.second;
  }
}