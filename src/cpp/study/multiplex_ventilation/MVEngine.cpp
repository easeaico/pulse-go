/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "MVEngine.h"

#include "engine/common/system/Model.h"
#include "cdm/system/environment/SEEnvironment.h"
#include "cdm/system/equipment/anesthesia_machine/SEAnesthesiaMachine.h"
#include "cdm/system/equipment/bag_valve_mask/SEBagValveMask.h"
#include "cdm/system/equipment/electrocardiogram/SEElectroCardioGram.h"
#include "cdm/system/equipment/inhaler/SEInhaler.h"
#include "cdm/system/equipment/mechanical_ventilator/SEMechanicalVentilator.h"
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/system/physiology/SEDrugSystem.h"
#include "cdm/system/physiology/SEEndocrineSystem.h"
#include "cdm/system/physiology/SEEnergySystem.h"
#include "cdm/system/physiology/SEGastrointestinalSystem.h"
#include "cdm/system/physiology/SEHepaticSystem.h"
#include "cdm/system/physiology/SENervousSystem.h"
#include "cdm/system/physiology/SERenalSystem.h"
#include "cdm/system/physiology/SERespiratorySystem.h"
#include "cdm/system/physiology/SETissueSystem.h"

using namespace pulse;

namespace pulse::study::multiplex_ventilation
{
  const std::string Dir::Base = "./test_results/multiplex_ventilation/";
  const std::string Dir::Solo = Dir::Base + "solo_states/";
  const std::string Dir::Results = Dir::Base + "simulations/";

  MVEngine::MVEngine(std::string const& logfile, bool cout_enabled, std::string const& data_dir) : Loggable(new Logger(logfile))
  {
    m_TimeStep_s = 0.02;
    m_CurrentTime_s = 0;
    m_SpareAdvanceTime_s = 0;

    m_SubMgr = nullptr;
    m_Oxygen = nullptr;

    m_CircuitMgr = nullptr;
    m_MultiplexVentilationCircuit = nullptr;
    m_Calculator = nullptr;

    m_CmptMgr = nullptr;
    m_MultiplexVentilationGraph = nullptr;
    m_Transporter = nullptr;

    m_DataDir = data_dir;
    m_SimulationData = nullptr;

    m_MVC = nullptr;
    m_FiO2 = nullptr;

    myLogger = true;
    GetLogger()->LogToConsole(cout_enabled);
    Info("Logging to console : " + std::string(cout_enabled ? "True" : "False"));
  }
  MVEngine::~MVEngine()
  {
    DestroyEngines();
    SAFE_DELETE(m_SimulationData);
  }

  void MVEngine::DestroyEngines()
  {
    m_CurrentTime_s = 0;
    SAFE_DELETE(m_SubMgr);
    m_Oxygen = nullptr;

    SAFE_DELETE(m_CircuitMgr);
    m_MultiplexVentilationCircuit = nullptr;
    SAFE_DELETE(m_Calculator);

    SAFE_DELETE(m_CmptMgr);
    m_MultiplexVentilationGraph = nullptr;
    SAFE_DELETE(m_Transporter);

    DELETE_VECTOR(m_Engines);
    m_Controllers.clear();
    m_AortaO2s.clear();
    m_AortaCO2s.clear();
    SAFE_DELETE(m_MVC);
    m_FiO2 = nullptr;
  }

  void MVEngine::HandleEvent(eEvent /*e*/, bool /*active*/, const SEScalarTime* /*simTime*/)
  {

  }

  bool MVEngine::CreateEngine(const std::string& simulationDataStr)
  {
    SAFE_DELETE(m_SimulationData);
    m_SimulationData = new pulse::study::bind::multiplex_ventilation::SimulationData();
    SerializeFromString(simulationDataStr, *m_SimulationData);
    return CreateEngine(*m_SimulationData);
  }

  bool MVEngine::CreateEngine(pulse::study::bind::multiplex_ventilation::SimulationData& sim)
  {
    try
    {
      if (!m_Controllers.empty())
      {
        Error("The controller currently has engine allocated, please reset the controller to start new set of engines");
        return false;
      }
      // Figure out where to put results
      std::string outDir = sim.outputbasefilename();
      if (outDir.empty())
        outDir = "";

      m_SubMgr = new SESubstanceManager(GetLogger());
      m_SubMgr->LoadSubstanceDirectory(m_DataDir);
      m_Oxygen = m_SubMgr->GetSubstance("Oxygen");
      m_CmptMgr = new SECompartmentManager(*m_SubMgr);
      m_CircuitMgr = new SECircuitManager(GetLogger());

      m_MultiplexVentilationCircuit = &m_CircuitMgr->CreateFluidCircuit("MultiplexVentilation");
      m_MultiplexVentilationGraph = &m_CmptMgr->CreateGasGraph("MultiplexVentilation");

      m_Calculator = new SEFluidCircuitCalculator(VolumePerPressureUnit::L_Per_cmH2O, VolumePerTimeUnit::L_Per_s,
        PressureTimeSquaredPerVolumeUnit::cmH2O_s2_Per_L, PressureUnit::cmH2O,
        VolumeUnit::L, PressureTimePerVolumeUnit::cmH2O_s_Per_L, GetLogger());
      m_Transporter = new SEGasTransporter(VolumePerTimeUnit::L_Per_s, VolumeUnit::L, VolumeUnit::L, NoUnit::unitless, GetLogger());

      SEFluidCircuitNode* inspiratoryConnectionNode = nullptr;
      SEFluidCircuitNode* expiratoryConnectionNode = nullptr;

      SEGasCompartment* inspiratoryConnectionCompartment = nullptr;
      SEGasCompartment* expiratoryConnectionCompartment = nullptr;

      Info("Creating " + std::to_string(sim.patientcomparisons_size()) + " patients");
      for (int p = 0; p < sim.patientcomparisons_size(); p++)
      {
        Engine* pe = (Engine*)CreatePulseEngine().release();
        Controller* pc = &pe->GetController();
        m_Controllers.push_back(pc);
        m_Engines.push_back(pe);

        auto& comparison = (*sim.mutable_patientcomparisons())[p];

        if (comparison.has_soloventilation())
        {
          auto* soloVentilation = comparison.mutable_soloventilation();
          std::string state = soloVentilation->statefile();

          pc->GetLogger()->SetLogFile(outDir + "multiplex_patient_" + std::to_string(p) + ".log");
          if (!pc->SerializeFromFile(state))
          {
            Error("Unable to load file : " + state);
            return false;
          }
          pc->GetLogger()->LogToConsole(GetLogger()->IsLoggingToConsole());

          // Fill out our initial solo ventilation data
          soloVentilation->set_oxygensaturation(pc->GetBloodChemistry().GetOxygenSaturation().GetValue());
          soloVentilation->set_tidalvolume_ml(pc->GetRespiratory().GetTidalVolume(VolumeUnit::mL));
          soloVentilation->set_endtidalcarbondioxidepressure_mmhg(pc->GetRespiratory().GetEndTidalCarbonDioxidePressure(PressureUnit::mmHg));
          auto AortaO2 = pc->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Aorta)->GetSubstanceQuantity(pc->GetSubstances().GetO2());
          soloVentilation->set_arterialoxygenpartialpressure_mmhg(AortaO2->GetPartialPressure(PressureUnit::mmHg));
          soloVentilation->set_arterialcarbondioxidepartialpressure_mmhg(AortaO2->GetPartialPressure(PressureUnit::mmHg));
          soloVentilation->set_horowitzindex_mmhg(pc->GetRespiratory().GetHorowitzIndex(PressureUnit::mmHg));
        }
        else if (comparison.has_multiplexventilation())
        {
          auto* multiVentilation = comparison.mutable_multiplexventilation();

          pc->GetLogger()->SetLogFile(outDir + "multiplex_patient_" + std::to_string(p) + ".log");
          if (!pc->SerializeFromFile(m_DataDir + "/states/StandardMale@0s.json"))
          {
            Error("Unable to load file : StandardMale@0s.json");
            return false;
          }
          pc->GetLogger()->LogToConsole(GetLogger()->IsLoggingToConsole());

          SEDyspnea dyspnea;
          dyspnea.GetTidalVolumeSeverity().SetValue(1.0);
          pc->ProcessAction(dyspnea);

          SEIntubation intubation;
          intubation.SetType(eIntubation_Type::Tracheal);
          pc->ProcessAction(intubation);

          SEOverrides overrides;
          overrides.GetScalarProperties()["RespiratoryCompliance"] = SEScalarPair(multiVentilation->compliance_ml_per_cmh2o(), VolumePerPressureUnit::mL_Per_cmH2O);
          overrides.GetScalarProperties()["RespiratoryResistance"] = SEScalarPair(multiVentilation->resistance_cmh2o_s_per_l(), PressureTimePerVolumeUnit::cmH2O_s_Per_L);
          pc->ProcessAction(overrides);

          SEImpairedAlveolarExchangeExacerbation impairedAlveolarExchange;
          impairedAlveolarExchange.GetSeverity().SetValue(multiVentilation->impairmentfraction());
          pc->ProcessAction(impairedAlveolarExchange);

          SEPulmonaryShuntExacerbation pulmonaryShunt;
          pulmonaryShunt.GetSeverity().SetValue(multiVentilation->impairmentfraction());
          pc->ProcessAction(pulmonaryShunt);
        }
        else
        {
          Error("Simulation does not have valid comparison object");
          return false;
        }

        // Build our multiplex circuit
        if (p == 0)
        {
          // Let's add the first mechanical ventilator circuit to our circuit
          // This will add the ventialtor and the respiratory system
          for (SEFluidCircuitNode* node : pc->GetCircuits().GetRespiratoryAndMechanicalVentilatorCircuit().GetNodes())
            m_MultiplexVentilationCircuit->ForceAddNode(*node);
          for (SEFluidCircuitPath* path : pc->GetCircuits().GetRespiratoryAndMechanicalVentilatorCircuit().GetPaths())
            m_MultiplexVentilationCircuit->ForceAddPath(*path);
          for (SEGasCompartment* cmpt : pc->GetCompartments().GetRespiratoryAndMechanicalVentilatorGraph().GetCompartments())
            m_MultiplexVentilationGraph->ForceAddCompartment(*cmpt);
          for (SEGasCompartmentLink* link : pc->GetCompartments().GetRespiratoryAndMechanicalVentilatorGraph().GetLinks())
            m_MultiplexVentilationGraph->ForceAddLink(*link);

          inspiratoryConnectionNode = pc->GetCircuits().GetRespiratoryAndMechanicalVentilatorCircuit().GetNode(pulse::MechanicalVentilatorNode::InspiratoryValve);
          expiratoryConnectionNode = pc->GetCircuits().GetRespiratoryAndMechanicalVentilatorCircuit().GetNode(pulse::MechanicalVentilatorNode::ExpiratoryValve);

          inspiratoryConnectionCompartment = pc->GetCompartments().GetRespiratoryAndMechanicalVentilatorGraph().GetCompartment(pulse::MechanicalVentilatorCompartment::InspiratoryValve);
          expiratoryConnectionCompartment = pc->GetCompartments().GetRespiratoryAndMechanicalVentilatorGraph().GetCompartment(pulse::MechanicalVentilatorCompartment::ExpiratoryValve);
        }
        else
        {
          SEFluidCircuitNode* expiratoryValveNode = nullptr;
          SEFluidCircuitNode* inspiratoryValveNode = nullptr;
          // Add all the nodes/paths/compartments/links to our circuit/graph
          for (SEFluidCircuitNode* node : pc->GetCircuits().GetRespiratoryAndMechanicalVentilatorCircuit().GetNodes())
          {
            if (node->GetName() != pulse::MechanicalVentilatorNode::Ventilator) // Don't add the ventilator node
              m_MultiplexVentilationCircuit->ForceAddNode(*node);
            if (node->GetName() == pulse::MechanicalVentilatorNode::ExpiratoryValve) // Need this later
              expiratoryValveNode = node;
            if (node->GetName() == pulse::MechanicalVentilatorNode::InspiratoryValve) // Need this later
              inspiratoryValveNode = node;
          }
          for (SEFluidCircuitPath* path : pc->GetCircuits().GetRespiratoryAndMechanicalVentilatorCircuit().GetPaths())
          {
            if (path->GetName() != pulse::MechanicalVentilatorPath::EnvironmentToVentilator &&     // Don't
                path->GetName() != pulse::MechanicalVentilatorPath::VentilatorToEnvironment &&     // Add
                path->GetName() != pulse::MechanicalVentilatorPath::VentilatorToExpiratoryValve && // These
                path->GetName() != pulse::MechanicalVentilatorPath::VentilatorToInspiratoryValve)  // Paths
              m_MultiplexVentilationCircuit->ForceAddPath(*path);
          }
          SEGasCompartment* expiratoryValveCmpt = nullptr;
          SEGasCompartment* inspiratoryValveCmpt = nullptr;
          for (SEGasCompartment* cmpt : pc->GetCompartments().GetRespiratoryAndMechanicalVentilatorGraph().GetCompartments())
          {
            if (cmpt->GetName() != pulse::MechanicalVentilatorCompartment::MechanicalVentilator)// Don't add the ventilator compartment
              m_MultiplexVentilationGraph->ForceAddCompartment(*cmpt);
            if (cmpt->GetName() == pulse::MechanicalVentilatorCompartment::ExpiratoryValve) // Need this later
              expiratoryValveCmpt = cmpt;
            if (cmpt->GetName() == pulse::MechanicalVentilatorCompartment::InspiratoryValve) // Need this later
              inspiratoryValveCmpt = cmpt;
          }
          for (SEGasCompartmentLink* link : pc->GetCompartments().GetRespiratoryAndMechanicalVentilatorGraph().GetLinks())
          {
            if (link->GetName() != pulse::MechanicalVentilatorLink::MechanicalVentilatorToExpiratoryValve && // Don't
              link->GetName() != pulse::MechanicalVentilatorLink::MechanicalVentilatorToInspiratoryValve)  // Add
              m_MultiplexVentilationGraph->ForceAddLink(*link);
          }

          // Connect the tubes to the shared ventilator circuit/graph
          auto& expiratoryConnectionPath = m_MultiplexVentilationCircuit->CreatePath(*expiratoryConnectionNode, *expiratoryValveNode, "expiratoryConnection_" + std::to_string(p));
          auto& expiratoryConnectionLink = m_CmptMgr->CreateGasLink(*expiratoryConnectionCompartment, *expiratoryValveCmpt, "expiratoryConnection_" + std::to_string(p));
          expiratoryConnectionLink.MapPath(expiratoryConnectionPath);
          m_MultiplexVentilationGraph->AddLink(expiratoryConnectionLink);

          auto& inspiratoryConnectionPath = m_MultiplexVentilationCircuit->CreatePath(*inspiratoryConnectionNode, *inspiratoryValveNode, "inspiratoryConnection_p" + std::to_string(p));
          auto& inspiratoryConnectionLink = m_CmptMgr->CreateGasLink(*inspiratoryConnectionCompartment, *inspiratoryValveCmpt, "inspiratoryConnection_p" + std::to_string(p));
          inspiratoryConnectionLink.MapPath(inspiratoryConnectionPath);
          m_MultiplexVentilationGraph->AddLink(inspiratoryConnectionLink);
        }
        TrackData(pc->GetEngineTracker(), outDir + "multiplex_patient_" + std::to_string(p) + "_results.csv");
        auto AortaO2 = pc->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Aorta)->GetSubstanceQuantity(pc->GetSubstances().GetO2());
        auto AortaCO2 = pc->GetCompartments().GetLiquidCompartment(pulse::VascularCompartment::Aorta)->GetSubstanceQuantity(pc->GetSubstances().GetCO2());
        m_AortaO2s.push_back(AortaO2);
        m_AortaCO2s.push_back(AortaCO2);
      }
      m_MultiplexVentilationCircuit->StateChange();
      m_MultiplexVentilationGraph->StateChange();

      Info("Configuring Mechanical Ventilator");
      m_MVC = new SEMechanicalVentilatorConfiguration(GetLogger());
      auto& mv = m_MVC->GetSettings();
      mv.SetConnection(eSwitch::On);
      mv.SetInspirationWaveform(eDriverWaveform::Square);
      mv.SetExpirationWaveform(eDriverWaveform::Square);
      mv.GetPeakInspiratoryPressure().SetValue(sim.pip_cmh2o(), PressureUnit::cmH2O);
      mv.GetPositiveEndExpiratoryPressure().SetValue(sim.peep_cmh2o(), PressureUnit::cmH2O);
      double respirationRate_per_min = sim.respirationrate_per_min();
      double IERatio = sim.ieratio();

      // Translate ventilator settings
      double totalPeriod_s = 60.0 / respirationRate_per_min;
      double inspiratoryPeriod_s = IERatio * totalPeriod_s / (1 + IERatio);
      double expiratoryPeriod_s = totalPeriod_s - inspiratoryPeriod_s;
      mv.GetInspirationMachineTriggerTime().SetValue(expiratoryPeriod_s, TimeUnit::s);
      mv.GetExpirationCycleTime().SetValue(inspiratoryPeriod_s, TimeUnit::s);

      m_FiO2 = &mv.GetFractionInspiredGas(*m_Oxygen);
      m_FiO2->GetFractionAmount().SetValue(sim.fio2());
      Info("Processing Action");
      ProcessAction(*m_MVC);
      m_CurrentTime_s = 0;
      m_SpareAdvanceTime_s = 0;
    }
    catch (CommonDataModelException& cdm_ex)
    {
      GetLogger()->Fatal("Exception caught runnning simulation " + sim.outputbasefilename());
      GetLogger()->Fatal(cdm_ex.what());
      std::cerr << cdm_ex.what() << std::endl;
      return false;
    }
    catch (std::exception& ex)
    {
      GetLogger()->Fatal("Exception caught runnning simulation " + sim.outputbasefilename());
      GetLogger()->Fatal(ex.what());
      std::cerr << ex.what() << std::endl;
      return false;
    }
    catch (...)
    {
      std::cerr << "Unable to run simulation " << sim.outputbasefilename() << std::endl;
      return false;
    }
    Info("Engine is ready");
    return true;
  }

  bool MVEngine::AdvanceTime(double time_s)
  {
    if (m_Controllers.empty())
    {
      Error("No engines have been allocated yet...");
      return false;
    }

    time_s += m_SpareAdvanceTime_s;
    int count = (int)(time_s / m_TimeStep_s);
    m_SpareAdvanceTime_s = time_s - (count * m_TimeStep_s);

    for (int i = 0; i < count; i++)
    {
      for (Controller* pc : m_Controllers)
      {
        pc->CheckIntubation();
        if (pc->GetEvents().IsEventActive(eEvent::IrreversibleState))
          return false;
      }
      // PreProcess
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetEnvironment()).PreProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetCardiovascular()).PreProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetInhaler()).PreProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetRespiratory()).PreProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetAnesthesiaMachine()).PreProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetMechanicalVentilator()).PreProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetGastrointestinal()).PreProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetHepatic()).PreProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetRenal()).PreProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetNervous()).PreProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetEnergy()).PreProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetEndocrine()).PreProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetDrugs()).PreProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetTissue()).PreProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetBloodChemistry()).PreProcess();
      // Since this is the last preprocess,
      // Check if we are in mechanical ventilator mode
      size_t vent_count = 0;
      bool enableMultiplexVentilation = false;
      for (Controller* pc : m_Controllers)
      {
        dynamic_cast<Model&>(pc->GetECG()).PreProcess();
        if (pc->GetAirwayMode() == eAirwayMode::MechanicalVentilator)
          vent_count++;
      }
      if (vent_count > 0)
      {
        if (vent_count == m_Controllers.size())
          enableMultiplexVentilation = true;
        else
        {
          Fatal("Engines are out of sync");
          return false;
        }
      }

      // Process
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetEnvironment()).Process();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetCardiovascular()).Process();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetInhaler()).Process();
      if (enableMultiplexVentilation)
      {
        // Solve the multiplex circuit
        m_Calculator->Process(*m_MultiplexVentilationCircuit, m_TimeStep_s);
        // Transport the multiplex graph
        m_Transporter->Transport(*m_MultiplexVentilationGraph, m_TimeStep_s);
        // TODO Consider adding aerosol support

        // TODO Copy mv node/path/cmpt/link values to engines>1
        // We are using engine 1's MV circuit/graph, so its up to date
        // The other engines MV circuit/graph needs to be manually updated
        // BUT, since the MV methodology does not use any circuit/graph data in its implementation
        // we DO NOT need to keep it up to date.
        // This is an intentional application specific optimization
      }
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetRespiratory()).Process(!enableMultiplexVentilation);
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetAnesthesiaMachine()).Process();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetMechanicalVentilator()).Process();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetGastrointestinal()).Process();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetHepatic()).Process();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetRenal()).Process();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetNervous()).Process();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetEnergy()).Process();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetEndocrine()).Process();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetDrugs()).Process();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetTissue()).Process();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetBloodChemistry()).Process();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetECG()).Process();

      // PostProcess
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetEnvironment()).PostProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetCardiovascular()).PostProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetInhaler()).PostProcess();
      if (enableMultiplexVentilation)
      {
        m_Calculator->PostProcess(*m_MultiplexVentilationCircuit);
      }
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetRespiratory()).PostProcess(!enableMultiplexVentilation);
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetAnesthesiaMachine()).PostProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetMechanicalVentilator()).PostProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetGastrointestinal()).PostProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetHepatic()).PostProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetRenal()).PostProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetNervous()).PostProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetEnergy()).PostProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetEndocrine()).PostProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetDrugs()).PostProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetTissue()).PostProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetBloodChemistry()).PostProcess();
      for (Controller* pc : m_Controllers)
        dynamic_cast<Model&>(pc->GetECG()).PostProcess();

      // Increment Times and track data
      for (Controller* pc : m_Controllers)
      {
        pc->GetEvents().UpdateEvents(pc->GetTimeStep());
        const_cast<SEScalarTime&>(pc->GetEngineTime()).Increment(pc->GetTimeStep());
        const_cast<SEScalarTime&>(pc->GetSimulationTime()).Increment(pc->GetTimeStep());
        pc->GetEngineTracker().TrackData(m_CurrentTime_s);
      }
      m_CurrentTime_s += m_TimeStep_s;
    }
    return true;
  }

  void MVEngine::SetFiO2(double FiO2)
  {
    m_FiO2->GetFractionAmount().SetValue(FiO2);
    ProcessAction(*m_MVC);
  }

  bool MVEngine::ProcessAction(const SEAction& a)
  {
    if (m_Controllers.empty())
    {
      Error("No engines have been allocated yet...");
      return false;
    }
    bool b = true;
    for (Controller* pc : m_Controllers)
      if (!pc->ProcessAction(a))
        b = false;
    return b;
  }
  bool MVEngine::ProcessActions(std::string const& actions, eSerializationFormat format)
  {
    if (m_Controllers.empty())
    {
      Error("No engines have been allocated yet...");
      return false;
    }

    bool success = true;
    if (actions.empty())
      return success;

    try
    {
      std::vector<SEAction*> vActions;
      if (!SEActionManager::SerializeFromString(actions, vActions, format, *m_SubMgr))
        return false;

      for (const SEAction* a : vActions)
      {
        if (!ProcessAction(*a))
          success = false;
        delete a;
      }
    }
    catch (CommonDataModelException& ex)
    {
      success = false;
      GetLogger()->Error(ex.what());
    }
    catch (std::exception& ex)
    {
      success = false;
      GetLogger()->Error(ex.what());
    }
    catch (...)
    {
      success = false;
    }
    return success;
  }

  std::string MVEngine::GetSimulationState()
  {
    if (!GetSimulationState(*m_SimulationData))
      return "";

    std::string content;
    SerializeToString(*m_SimulationData, content);
    return content;
  }
  bool MVEngine::GetSimulationState(pulse::study::bind::multiplex_ventilation::SimulationData& sim)
  {
    if (m_Controllers.empty())
    {
      Error("No engines have been allocated yet...");
      return false;
    }

    for (int p = 0; p < sim.patientcomparisons_size(); p++)
    {
      Controller* pc = m_Controllers[p];
      auto* multiVentilation = (*sim.mutable_patientcomparisons())[p].mutable_multiplexventilation();
      // For Completeness, Write out the ventilator settings

      // Translate ventilator settings
      double expiratoryPeriod_s = pc->GetMechanicalVentilator().GetSettings().GetInspirationMachineTriggerTime(TimeUnit::s);
      double inspiratoryPeriod_s = pc->GetMechanicalVentilator().GetSettings().GetExpirationCycleTime(TimeUnit::s);
      double respirationRate_per_min = 60.0 / (inspiratoryPeriod_s + inspiratoryPeriod_s);
      double IERatio = inspiratoryPeriod_s / expiratoryPeriod_s;

      multiVentilation->set_respirationrate_per_min(respirationRate_per_min);
      multiVentilation->set_ieratio(IERatio);
      multiVentilation->set_peep_cmh2o(pc->GetMechanicalVentilator().GetSettings().GetPositiveEndExpiratoryPressure(PressureUnit::cmH2O));
      multiVentilation->set_pip_cmh2o(pc->GetMechanicalVentilator().GetSettings().GetPeakInspiratoryPressure(PressureUnit::cmH2O));
      multiVentilation->set_fio2(pc->GetMechanicalVentilator().GetSettings().GetFractionInspiredGas(pc->GetSubstances().GetO2()).GetFractionAmount().GetValue());
      // Write out all the vitals
      multiVentilation->set_airwayflow_l_per_min(pc->GetRespiratory().GetInspiratoryFlow(VolumePerTimeUnit::L_Per_min));
      multiVentilation->set_airwaypressure_cmh2o(pc->GetCompartments().GetGasCompartment(pulse::PulmonaryCompartment::Airway)->GetPressure(PressureUnit::cmH2O));
      multiVentilation->set_alveolararterialgradient_mmhg(pc->GetRespiratory().GetAlveolarArterialGradient(PressureUnit::mmHg));
      multiVentilation->set_arterialcarbondioxidepartialpressure_mmhg(m_AortaCO2s[p]->GetPartialPressure(PressureUnit::mmHg));
      multiVentilation->set_arterialoxygenpartialpressure_mmhg(m_AortaO2s[p]->GetPartialPressure(PressureUnit::mmHg));
      multiVentilation->set_horowitzindex_mmhg(pc->GetRespiratory().GetHorowitzIndex(PressureUnit::mmHg));
      multiVentilation->set_endtidalcarbondioxidepressure_mmhg(pc->GetRespiratory().GetEndTidalCarbonDioxidePressure(PressureUnit::mmHg));
      multiVentilation->set_idealbodyweight_kg(pc->GetCurrentPatient().GetIdealBodyWeight(MassUnit::kg));
      multiVentilation->set_meanairwaypressure_cmh2o(pc->GetRespiratory().GetMeanAirwayPressure(PressureUnit::cmH2O));
      multiVentilation->set_oxygenationindex(pc->GetRespiratory().GetOxygenationIndex().GetValue());
      multiVentilation->set_oxygensaturation(pc->GetBloodChemistry().GetOxygenSaturation().GetValue());
      multiVentilation->set_oxygensaturationindex_mmhg(pc->GetRespiratory().GetOxygenSaturationIndex().GetValue(PressureUnit::mmHg));
      multiVentilation->set_sfratio(pc->GetRespiratory().GetSaturationAndFractionOfInspiredOxygenRatio().GetValue());
      multiVentilation->set_shuntfraction(pc->GetBloodChemistry().GetShuntFraction().GetValue());
      multiVentilation->set_tidalvolume_ml(pc->GetRespiratory().GetTidalVolume(VolumeUnit::mL));
      multiVentilation->set_totallungvolume_ml(pc->GetRespiratory().GetTotalLungVolume(VolumeUnit::mL));
      // Update the sim
      if (p == 0)
      {
        sim.set_respirationrate_per_min(multiVentilation->respirationrate_per_min());
        sim.set_ieratio(multiVentilation->ieratio());
        sim.set_peep_cmh2o(multiVentilation->peep_cmh2o());
        sim.set_pip_cmh2o(multiVentilation->pip_cmh2o());
        sim.set_fio2(multiVentilation->fio2());
      }
    }
    return true;
  }

  double MVEngine::GetMinSpO2()
  {
    double SpO2;
    double minSpO2 = 1.0;
    for (Controller* pc : m_Controllers)
    {
      SpO2 = pc->GetBloodChemistry().GetOxygenSaturation().GetValue();
      if (SpO2 < minSpO2)
        minSpO2 = SpO2;
    }
    return minSpO2;
  }

  double MVEngine::GetMinPAO2_mmHg()
  {
    double PAO2_mmHg;
    double minPAO2_mmHg = 1000;
    for (SELiquidSubstanceQuantity* aortaO2 : m_AortaO2s)
    {
      PAO2_mmHg = aortaO2->GetPartialPressure(PressureUnit::mmHg);
      if (PAO2_mmHg < minPAO2_mmHg)
        minPAO2_mmHg = PAO2_mmHg;
    }
    return minPAO2_mmHg;
  }

  void MVEngine::TrackData(SEEngineTracker& trkr, const std::string& csv_filename)
  {
    trkr.GetDataRequestManager().SetResultsFilename(csv_filename);
    trkr.GetDataRequestManager().CreatePhysiologyDataRequest("RespiratoryCompliance", VolumePerPressureUnit::L_Per_cmH2O);
    trkr.GetDataRequestManager().CreatePhysiologyDataRequest("ExpiratoryRespiratoryResistance", PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    trkr.GetDataRequestManager().CreatePhysiologyDataRequest("InspiratoryRespiratoryResistance", PressureTimePerVolumeUnit::cmH2O_s_Per_L);
    trkr.GetDataRequestManager().CreateMechanicalVentilatorDataRequest("PeakInspiratoryPressure", PressureUnit::cmH2O);
    trkr.GetDataRequestManager().CreateMechanicalVentilatorDataRequest("TotalPositiveEndExpiratoryPressure", PressureUnit::cmH2O);
    trkr.GetDataRequestManager().CreatePhysiologyDataRequest("TotalRespiratoryModelCompliance", VolumePerPressureUnit::L_Per_cmH2O);
    trkr.GetDataRequestManager().CreatePhysiologyDataRequest("TotalLungVolume", VolumeUnit::mL);
    trkr.GetDataRequestManager().CreatePhysiologyDataRequest("TidalVolume", VolumeUnit::mL);
    trkr.GetDataRequestManager().CreatePhysiologyDataRequest("EndTidalCarbonDioxidePressure", PressureUnit::mmHg);
    trkr.GetDataRequestManager().CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
    trkr.GetDataRequestManager().CreatePhysiologyDataRequest("OxygenSaturation");
    trkr.GetDataRequestManager().CreatePhysiologyDataRequest("HorowitzIndex", PressureUnit::mmHg);
    trkr.GetDataRequestManager().CreatePhysiologyDataRequest("InspiratoryExpiratoryRatio");
    trkr.GetDataRequestManager().CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
    trkr.GetDataRequestManager().CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
    trkr.GetDataRequestManager().CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
    trkr.GetDataRequestManager().CreatePhysiologyDataRequest("CardiacOutput", VolumePerTimeUnit::L_Per_min);
    trkr.GetDataRequestManager().CreatePhysiologyDataRequest("ShuntFraction");
    trkr.GetDataRequestManager().CreatePhysiologyDataRequest("AlveolarArterialGradient", PressureUnit::mmHg);
    trkr.GetDataRequestManager().CreatePhysiologyDataRequest("InspiratoryFlow", VolumePerTimeUnit::L_Per_min);
    trkr.GetDataRequestManager().CreatePhysiologyDataRequest("TransrespiratoryPressure", PressureUnit::cmH2O);


    trkr.GetDataRequestManager().CreateLiquidCompartmentDataRequest(pulse::PulmonaryCompartment::Airway, "Pressure", PressureUnit::cmH2O);

    trkr.GetDataRequestManager().CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::Aorta, "Oxygen", "PartialPressure", PressureUnit::mmHg);
    trkr.GetDataRequestManager().CreateLiquidCompartmentDataRequest(pulse::VascularCompartment::Aorta, "CarbonDioxide", "PartialPressure", PressureUnit::mmHg);
    trkr.GetDataRequestManager().CreateGasCompartmentDataRequest(pulse::PulmonaryCompartment::Airway, "Oxygen", "PartialPressure", PressureUnit::mmHg);
    trkr.GetDataRequestManager().CreateGasCompartmentDataRequest(pulse::PulmonaryCompartment::Airway, "CarbonDioxide", "PartialPressure", PressureUnit::mmHg);
    trkr.GetDataRequestManager().CreateGasCompartmentDataRequest(pulse::PulmonaryCompartment::LeftAlveoli, "Oxygen", "PartialPressure", PressureUnit::mmHg);
    trkr.GetDataRequestManager().CreateGasCompartmentDataRequest(pulse::PulmonaryCompartment::LeftAlveoli, "CarbonDioxide", "PartialPressure", PressureUnit::mmHg);
    trkr.GetDataRequestManager().CreateGasCompartmentDataRequest(pulse::PulmonaryCompartment::RightAlveoli, "Oxygen", "PartialPressure", PressureUnit::mmHg);
    trkr.GetDataRequestManager().CreateGasCompartmentDataRequest(pulse::PulmonaryCompartment::RightAlveoli, "CarbonDioxide", "PartialPressure", PressureUnit::mmHg);

    trkr.SetupRequests();
  }

  bool MVEngine::SerializeToString(pulse::study::bind::multiplex_ventilation::SimulationData& src, std::string& dst)
  {
    google::protobuf::util::JsonPrintOptions printOpts;
    printOpts.add_whitespace = true;
    printOpts.preserve_proto_field_names = true;
    printOpts.always_print_primitive_fields = true;
    return google::protobuf::util::MessageToJsonString(src, &dst, printOpts).ok();
  }
  bool MVEngine::SerializeFromString(const std::string& src, pulse::study::bind::multiplex_ventilation::SimulationData& dst)
  {
    google::protobuf::util::JsonParseOptions parseOpts;
    auto status = google::protobuf::util::JsonStringToMessage(src, &dst, parseOpts);
    if (!status.ok())
    {
      PBUtils::LogError("MVEngine::SerializeFromString", status.ToString(), nullptr);
      return false;
    }
    return true;
  }


  bool MVEngine::RunSoloState(const std::string& stateFile, const std::string& resultBasePath, double duration_s, Logger& logger)
  {
    std::string logFile = resultBasePath + ".log";
    std::string dataFile = resultBasePath + "Results.csv";

    TimingProfile profiler;
    profiler.Start("Total");
    profiler.Start("Status");
    double statusTime_s = 0;// Current time of this status cycle
    double statusStep_s = 60;//How long did it take to simulate this much time

    double timeStep_s = 0.02;
    double currentTime_s = 0;

    auto pe = CreatePulseEngine();
    pe->GetLogger()->SetLogFile(logFile);
    pe->SerializeFromFile(stateFile);
    MVEngine::TrackData(*pe->GetEngineTracker(), dataFile);
    int count = (int)(duration_s / timeStep_s);
    for (int i = 0; i < count; i++)
    {
      if (pe->GetEventManager().IsEventActive(eEvent::IrreversibleState))
        return false;
      pe->AdvanceModelTime();
      pe->GetEngineTracker()->TrackData(currentTime_s);
      currentTime_s += timeStep_s;
      statusTime_s += timeStep_s;
      // How are we running?
      if (statusTime_s > statusStep_s)
      {
        statusTime_s = 0;
        logger.Info("Current Time is " + cdm::to_string(currentTime_s) + "s, it took " + cdm::to_string(profiler.GetElapsedTime_s("Status")) + "s to simulate the past " + cdm::to_string(statusStep_s) + "s");
        profiler.Reset("Status");
      }
    }
    logger.Info("It took " + cdm::to_string(profiler.GetElapsedTime_s("Total")) + "s to run this simulation");
    profiler.Clear();
    return true;
  }

  bool ExtractInitialConditions(const std::string& filePath, double& comp, double& imp)
  {
    // Parse the file name to get our ventilator settings
    // ex. comp=0.01_peep=10_pip=55_imp=0.3_FiO2=0.21
    size_t compIdx = filePath.find("comp=") + 5;
    size_t _compIdx = filePath.find("_", compIdx);
    comp = std::atof(filePath.substr(compIdx, _compIdx).c_str());
    size_t impIdx = filePath.find("imp=") + 4;
    size_t _impIdx = filePath.find("_", impIdx);
    imp = std::atof(filePath.substr(impIdx, _impIdx).c_str());

    return true;
  }

  bool ExtractVentilatorSettings(const std::string& filePath, std::string& fileName, double& pip_cmH2O, double& peep_cmH2O, double& FiO2)
  {
    // Parse the file name to get our ventilator settings
    // ex. comp=0.01_peep=10_pip=55_imp=0.3_FiO2=0.21
    size_t peepIdx = filePath.find("peep=") + 5;
    size_t _peepIdx = filePath.find("_", peepIdx);
    peep_cmH2O = std::atof(filePath.substr(peepIdx, _peepIdx).c_str());
    size_t pipIdx = filePath.find("pip=") + 4;
    size_t _pipIdx = filePath.find("_", pipIdx);
    pip_cmH2O = std::atof(filePath.substr(pipIdx, _pipIdx).c_str());
    size_t FiO2Idx = filePath.find("FiO2=") + 5;
    size_t _FiO2Idx = filePath.find(".", FiO2Idx);
    FiO2 = std::atof(filePath.substr(FiO2Idx, _FiO2Idx).c_str());

    fileName = filePath.substr(filePath.find_last_of("/") + 1);
    fileName = fileName.substr(0, fileName.length() - 5);
    return true;
  }
}
