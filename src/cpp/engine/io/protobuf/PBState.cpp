/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "PulseConfiguration.h"
#include "PulseEngine.h"
#include "engine/common/controller/SubstanceManager.h"
#include "engine/common/controller/CircuitManager.h"
#include "engine/common/controller/CompartmentManager.h"
#include "engine/common/system/physiology/Saturation.h"
#include "engine/io/protobuf/PBConfiguration.h"
#include "engine/io/protobuf/PBPhysiology.h"
#include "engine/io/protobuf/PBEnvironment.h"
#include "engine/io/protobuf/PBEquipment.h"
#include "engine/io/protobuf/PBState.h"
PUSH_PROTO_WARNINGS
#include "pulse/engine/bind/State.pb.h"
POP_PROTO_WARNINGS

#include "cdm/io/protobuf/PBCompartment.h"
#include "cdm/io/protobuf/PBSubstance.h"
#include "cdm/io/protobuf/PBPatient.h"
#include "cdm/io/protobuf/PBConditions.h"
#include "cdm/io/protobuf/PBActions.h"
#include "cdm/io/protobuf/PBEngine.h"
#include "cdm/io/protobuf/PBCircuit.h"
#include "cdm/io/protobuf/PBCompartment.h"
#include "cdm/io/protobuf/PBProperties.h"
#include "cdm/io/protobuf/PBUtils.h"
#include "cdm/patient/SEPatient.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceCompound.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/engine/SECondition.h"
#include "cdm/engine/SEConditionManager.h"
#include "cdm/engine/SEAction.h"
#include "cdm/engine/SEActionManager.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEEventManager.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/utils/FileUtils.h"


namespace pulse
{
  void PBState::Load(const PULSE_BIND::StateData& src, Controller& dst, const SEEngineConfiguration* config)
  {
    PBState::Serialize(src, dst, config);
  }
  bool PBState::Serialize(const PULSE_BIND::StateData& src, Controller& dst, const SEEngineConfiguration* config)
  {
    dst.Info("Loading state generated from Pulse-"+src.version());
    std::stringstream ss;
    dst.m_State = EngineState::NotReady;
    // First Get the substances reset and ready
    dst.m_Substances->Clear();
    // Load up our Substances //
    for (int i = 0; i < src.substance_size(); i++)
    {
      const CDM_BIND::SubstanceData& subData = src.substance()[i];
      SESubstance* sub = dst.m_Substances->GetSubstance(subData.name());
      PBSubstance::Load(subData, *sub);
    }
    for (int i = 0; i < src.activesubstance_size(); i++)
    {
      SESubstance* sub = dst.m_Substances->GetSubstance(src.activesubstance()[i]);
      if (sub == nullptr)
      {
        dst.Error("Active substance not found : " + src.activesubstance()[i]);
        return false;
      }
      dst.m_Substances->AddActiveSubstance(*sub);
    }
    // Load up our Compounds //
    for (int i = 0; i < src.compound_size(); i++)
    {
      const CDM_BIND::SubstanceCompoundData& cmpdData = src.compound()[i];
      SESubstanceCompound* cmpd = dst.m_Substances->GetCompound(cmpdData.name());
      PBSubstance::Load(cmpdData, *cmpd, *dst.m_Substances);
    }
    for (int i = 0; i < src.activecompound_size(); i++)
    {
      SESubstanceCompound* cmpd = dst.m_Substances->GetCompound(src.activecompound()[i]);
      if (cmpd == nullptr)
      {
        dst.Error("Active substance compound not found : " + src.activecompound()[i]);
        return false;
      }
      dst.m_Substances->AddActiveCompound(*cmpd);
    }
    if (!dst.m_Substances->Setup())
      return false;
    if (!dst.m_SaturationCalculator->Setup())
      return false;

    // Configuration //
    if (!src.has_configuration())
      ss << "PulseState must have a configuration" << std::endl;
    else
      PBConfiguration::Load(src.configuration(), *dst.m_Config, *dst.m_Substances);

    // We could preserve the tracker, but I think I want to force the user to set it up
    // again, they should have the data tracks (or easily get them), and they should
    // Set it back up, and set or reset the results file they are using
    if (src.has_datarequestmanager())
    {
      dst.m_EngineTrack->GetDataRequestManager().Clear();
      PBEngine::Load(src.datarequestmanager(), dst.m_EngineTrack->GetDataRequestManager());
      dst.m_EngineTrack->ForceConnection();// I don't want to rest the file because I would loose all my data
    }

    if (src.has_simulationtime())
    {
      PBProperty::Load(src.simulationtime(), dst.m_CurrentTime);
      PBProperty::Load(src.simulationtime(), dst.m_SimulationTime);
    }
    else
    {
      dst.m_CurrentTime.SetValue(0, TimeUnit::s);
      dst.m_SimulationTime.SetValue(0, TimeUnit::s);
    }
    dst.Info("[SimTime(s)] " + dst.m_SimulationTime.ToString());

    dst.m_AirwayMode = (eAirwayMode)src.airwaymode();
    if (src.intubation() == (CDM_BIND::eSwitch)eSwitch::NullSwitch)
      ss << "Pulse State must have none null intubation state";
    dst.m_Intubation = (eSwitch)src.intubation();

    /// Patient //
    if (!src.has_currentpatient() || !src.has_initialpatient())
      ss << "PulseState must have a patient" << std::endl;
    else
    {
      PBPatient::Load(src.currentpatient(), *dst.m_CurrentPatient);
      PBPatient::Load(src.initialpatient(), *dst.m_InitialPatient);

      std::string str;
      dst.m_CurrentPatient->SerializeToString(str, eSerializationFormat::JSON);
      dst.Info("[Patient]\n" + str);
    }
    // Conditions //
    dst.m_Conditions->Clear();
    if (src.has_conditions())
    {
      for (int i = 0; i < src.conditions().anycondition_size(); i++)
      {
        SECondition* c = PBCondition::Load(src.conditions().anycondition()[i], *dst.m_Substances);
        dst.m_Conditions->Copy(*c, *dst.m_Substances);
        delete c;
      }
    }
    // Actions //
    dst.m_Actions->Clear();
    if (src.has_activeactions())
    {
      for (int i = 0; i < src.activeactions().anyaction_size(); i++)
      {
        SEAction* a = PBAction::Load(src.activeactions().anyaction()[i], *dst.m_Substances);
        dst.m_Actions->ProcessAction(*a);
        delete a;
      }
    }
    // Active Events //
    dst.m_EventManager->Clear();
    if (src.has_activeevents())
    {
      SEScalarTime time;
      for (int i = 0; i < src.activeevents().activeevent_size(); i++)
      {
        const CDM_BIND::ActiveEventData& e = src.activeevents().activeevent()[i];
        if (e.has_duration())
          PBProperty::Load(e.duration(), time);
        else
        {
          dst.m_ss << "Active event " << CDM_BIND::eEvent_Name(e.event()) << " does not have time associated with it";
          dst.Debug(dst.m_ss);
          time.SetValue(0, TimeUnit::s);
        }
        dst.m_EventManager->OverrideActiveState((eEvent)e.event(), time);
      }
    }

    // Circuit Manager //
    if (!src.has_circuitmanager())
      ss << "PulseState must have a circuit manager" << std::endl;
    else
      PBCircuit::Load(src.circuitmanager(), *dst.m_Circuits);
    // Compartment Manager //
    if (!src.has_compartmentmanager())
      ss << "PulseState must have a compartment manager" << std::endl;
    else
      PBCompartment::Load(src.compartmentmanager(), *dst.m_Compartments, dst.m_Circuits);
    if (config != nullptr)
    {// Merge in any provided configuration parameters, I hope you know what you are doing....
      const PulseConfiguration* peConfig = dynamic_cast<const PulseConfiguration*>(config);
      if (peConfig == nullptr)
      {
        dst.Error("Configuration is not a Pulse configuration object, ignoring it");
      }
      else
      {
        dst.m_Config->Merge(*peConfig, *dst.m_Substances);
      }
    }
    /////////////
    // Models //
    /////////////
    if (!src.has_bloodchemistry())
      ss << "Missing Blood Chemistry State" << std::endl;
    else
      PBPhysiology::Load(src.bloodchemistry(), *dst.m_BloodChemistryModel);

    if (!src.has_cardiovascular())
      ss << "Missing Cardiovascular State" << std::endl;
    else
      PBPhysiology::Load(src.cardiovascular(), *dst.m_CardiovascularModel);

    if (!src.has_drug())
      ss << "Missing Drug State" << std::endl;
    else
      PBPhysiology::Load(src.drug(), *dst.m_DrugModel);

    if (!src.has_endocrine())
      ss << "Missing Endocrine State" << std::endl;
    else
      PBPhysiology::Load(src.endocrine(), *dst.m_EndocrineModel);

    if (!src.has_energy())
      ss << "Missing Energy State" << std::endl;
    else
      PBPhysiology::Load(src.energy(), *dst.m_EnergyModel);

    if (!src.has_gastrointestinal())
      ss << "Missing Gastrointestinal State" << std::endl;
    else
      PBPhysiology::Load(src.gastrointestinal(), *dst.m_GastrointestinalModel);

    if (!src.has_hepatic())
      ss << "Missing Hepatic State" << std::endl;
    else
      PBPhysiology::Load(src.hepatic(), *dst.m_HepaticModel);

    if (!src.has_nervous())
      ss << "Missing Nervous State" << std::endl;
    else
      PBPhysiology::Load(src.nervous(), *dst.m_NervousModel);

    if (!src.has_renal())
      ss << "Missing Renal State" << std::endl;
    else
      PBPhysiology::Load(src.renal(), *dst.m_RenalModel);

    if (!src.has_respiratory())
      ss << "Missing Respiratory State" << std::endl;
    else
      PBPhysiology::Load(src.respiratory(), *dst.m_RespiratoryModel);

    if (!src.has_tissue())
      ss << "Missing Tissue State" << std::endl;
    else
      PBPhysiology::Load(src.tissue(), *dst.m_TissueModel);

    if (!src.has_environment())
      ss << "Missing Environment State" << std::endl;
    else
      PBEnvironment::Load(src.environment(), *dst.m_EnvironmentModel);

    if (!src.has_anesthesiamachine())
      ss << "Missing Anesthesia Machine State" << std::endl;
    else
      PBEquipment::Load(src.anesthesiamachine(), *dst.m_AnesthesiaMachineModel);

    if (!src.has_bagvalvemask())
      ss << "Missing BagValveMask State" << std::endl;
    else
      PBEquipment::Load(src.bagvalvemask(), *dst.m_BagValveMaskModel);

    if (!src.has_ecmo())
      ss << "Missing ECMO State" << std::endl;
    else
      PBEquipment::Load(src.ecmo(), *dst.m_ECMOModel);

    if (!src.has_electrocardiogram())
      ss << "Missing ECG State" << std::endl;
    else
      PBEquipment::Load(src.electrocardiogram(), *dst.m_ElectroCardioGramModel);

    if (!src.has_inhaler())
      ss << "Missing Inhaler State" << std::endl;
    else
      PBEquipment::Load(src.inhaler(), *dst.m_InhalerModel);

    if (!src.has_mechanicalventilator())
      ss << "Missing Mechanical Ventilator State" << std::endl;
    else
      PBEquipment::Load(src.mechanicalventilator(), *dst.m_MechanicalVentilatorModel);

    if (!ss.str().empty())
    {
      dst.Error(ss);
      return false;
    }
    // Make sure links are correct with our modes
    dst.m_Compartments->UpdateAirwayGraph();
    dst.m_Compartments->GetActiveRespiratoryGraph();
    dst.m_Compartments->GetActiveAerosolGraph();

    // If we had any handlers, reinform the listening classes
    if (dst.m_AdvanceHandler != nullptr)
      dst.SetAdvanceHandler(dst.m_AdvanceHandler);


    // It helps to unload what you just loaded and do a compare if you have issues
    //SerializeToFile(dst, "WhatIJustLoaded.json");

    // Good to go, save it off and carry on!
    dst.m_State = EngineState::Active;
    // TODO CheckDataRequirements/IsValid() or something
    return true;
  }

  PULSE_BIND::StateData* PBState::Unload(const Controller& src)
  {
    PULSE_BIND::StateData* dst = new PULSE_BIND::StateData();
    PBState::Serialize(src, *dst);
    return dst;
  }
  bool PBState::Serialize(const Controller& src, PULSE_BIND::StateData& dst)
  {
    dst.set_version(PulseBuildInformation::Version() + "-" + PulseBuildInformation::Hash());
    dst.set_airwaymode((PULSE_BIND::eAirwayMode)src.m_AirwayMode);
    dst.set_intubation((CDM_BIND::eSwitch)src.m_Intubation);
    dst.set_allocated_simulationtime(PBProperty::Unload(src.m_SimulationTime));
    if (src.m_EngineTrack->GetDataRequestManager().HasDataRequests())
      dst.set_allocated_datarequestmanager(PBEngine::Unload(src.m_EngineTrack->GetDataRequestManager()));
    // Patient
    dst.set_allocated_currentpatient(PBPatient::Unload(*src.m_CurrentPatient));
    dst.set_allocated_initialpatient(PBPatient::Unload(*src.m_InitialPatient));
    // Conditions
    dst.set_allocated_conditions(PBEngine::Unload(*src.m_Conditions));
    // Actions
    dst.set_allocated_activeactions(PBEngine::Unload(*src.m_Actions));
    // Active Events
    SEScalarTime time;
    for (auto itr : src.m_EventManager->GetEventStates())
    {
      if (!itr.second)
        continue;

      CDM_BIND::ActiveEventData* eData = dst.mutable_activeevents()->add_activeevent();
      eData->set_event((CDM_BIND::eEvent)itr.first);
      time.SetValue(src.m_EventManager->GetEventDuration(itr.first, TimeUnit::s), TimeUnit::s);
      eData->set_allocated_duration(PBProperty::Unload(time));
    }
    // Add all Substances/Compunds
    for (SESubstance* s : src.m_Substances->GetSubstances())
      dst.mutable_substance()->AddAllocated(PBSubstance::Unload(*s));
    for (SESubstanceCompound* c : src.m_Substances->GetCompounds())
      dst.mutable_compound()->AddAllocated(PBSubstance::Unload(*c));
    // Track the active Substances/Compounds
    for (SESubstance* s : src.m_Substances->GetActiveSubstances())
      dst.mutable_activesubstance()->Add(s->GetName());
    for (SESubstanceCompound* c : src.m_Substances->GetActiveCompounds())
      dst.mutable_activecompound()->Add(c->GetName());
    // Models
    dst.set_allocated_bloodchemistry(PBPhysiology::Unload(*src.m_BloodChemistryModel));
    dst.set_allocated_cardiovascular(PBPhysiology::Unload(*src.m_CardiovascularModel));
    dst.set_allocated_drug(PBPhysiology::Unload(*src.m_DrugModel));
    dst.set_allocated_endocrine(PBPhysiology::Unload(*src.m_EndocrineModel));
    dst.set_allocated_energy(PBPhysiology::Unload(*src.m_EnergyModel));
    dst.set_allocated_gastrointestinal(PBPhysiology::Unload(*src.m_GastrointestinalModel));
    dst.set_allocated_hepatic(PBPhysiology::Unload(*src.m_HepaticModel));
    dst.set_allocated_nervous(PBPhysiology::Unload(*src.m_NervousModel));
    dst.set_allocated_renal(PBPhysiology::Unload(*src.m_RenalModel));
    dst.set_allocated_respiratory(PBPhysiology::Unload(*src.m_RespiratoryModel));
    dst.set_allocated_tissue(PBPhysiology::Unload(*src.m_TissueModel));
    dst.set_allocated_environment(PBEnvironment::Unload(*src.m_EnvironmentModel));
    dst.set_allocated_anesthesiamachine(PBEquipment::Unload(*src.m_AnesthesiaMachineModel));
    dst.set_allocated_bagvalvemask(PBEquipment::Unload(*src.m_BagValveMaskModel));
    dst.set_allocated_ecmo(PBEquipment::Unload(*src.m_ECMOModel));
    dst.set_allocated_electrocardiogram(PBEquipment::Unload(*src.m_ElectroCardioGramModel));
    dst.set_allocated_inhaler(PBEquipment::Unload(*src.m_InhalerModel));
    dst.set_allocated_mechanicalventilator(PBEquipment::Unload(*src.m_MechanicalVentilatorModel));
    // Compartments
    dst.set_allocated_compartmentmanager(PBCompartment::Unload(*src.m_Compartments));
    // Configuration
    dst.set_allocated_configuration(PBConfiguration::Unload(*src.m_Config));
    // Circuits
    dst.set_allocated_circuitmanager(PBCircuit::Unload(*src.m_Circuits));
    return true;
  }

  bool PBState::SerializeFromFile(const std::string& filename, Controller& dst, const SEEngineConfiguration* config)
  {
    PULSE_BIND::StateData data;
    if (!PBUtils::SerializeFromFile(filename, data, dst.GetLogger()))
      return false;
    PBState::Load(data, dst, config);
    return true;
  }

  bool PBState::SerializeToFile(const Controller& src, const std::string& filename)
  {
    PULSE_BIND::StateData data;
    PBState::Serialize(src, data);
    return PBUtils::SerializeToFile(data, filename, src.GetLogger());
  }

  bool PBState::SerializeToString(const Controller& src, std::string& output, eSerializationFormat m)
  {
    PULSE_BIND::StateData data;
    PBState::Serialize(src, data);
    return PBUtils::SerializeToString(data, output, m, src.GetLogger());
  }
  bool PBState::SerializeFromString(const std::string& src, Controller& dst, eSerializationFormat m, const SEEngineConfiguration* config)
  {
    PULSE_BIND::StateData data;
    if (!PBUtils::SerializeFromString(src, data, m, dst.GetLogger()))
      return false;
    PBState::Load(data, dst, config);
    return true;
  }
}