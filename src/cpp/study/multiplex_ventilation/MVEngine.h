/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include <thread>
#include "PulseEngine.h"

#include "engine/common/Engine.h"
#include "engine/common/controller/Controller.h"
#include "engine/common/controller/CircuitManager.h"

#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/patient/SEPatient.h"
#include "cdm/patient/actions/SEDyspnea.h"
#include "cdm/patient/actions/SEIntubation.h"
#include "cdm/patient/actions/SEImpairedAlveolarExchangeExacerbation.h"
#include "cdm/patient/actions/SEPulmonaryShuntExacerbation.h"
#include "cdm/system/equipment/mechanical_ventilator/SEMechanicalVentilator.h"
#include "cdm/system/equipment/mechanical_ventilator/SEMechanicalVentilatorSettings.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorConfiguration.h"
#include "cdm/system/physiology/SEBloodChemistrySystem.h"
#include "cdm/system/physiology/SERespiratorySystem.h"
#include "cdm/engine/SEOverrides.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEActionManager.h"
#include "cdm/engine/SEPatientActionCollection.h"
#include "cdm/substance/SESubstanceFraction.h"
#include "cdm/substance/SESubstanceTransport.h"
#include "cdm/circuit/fluid/SEFluidCircuitCalculator.h"
#include "cdm/circuit/fluid/SEFluidCircuit.h"
#include "cdm/circuit/fluid/SEFluidCircuitNode.h"
#include "cdm/circuit/fluid/SEFluidCircuitPath.h"
#include "cdm/compartment/fluid/SEGasCompartmentGraph.h"
#include "cdm/compartment/fluid/SELiquidCompartmentGraph.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerPressure.h"
#include "cdm/utils/DataTrack.h"
#include "cdm/utils/FileUtils.h"
#include "cdm/utils/GeneralMath.h"
#include "cdm/utils/TimingProfile.h"

PUSH_PROTO_WARNINGS
#include "pulse/study/bind/MultiplexVentilation.pb.h"
#include <google/protobuf/text_format.h>
#include <google/protobuf/util/json_util.h>
POP_PROTO_WARNINGS
#include "io/protobuf/PBUtils.h"

namespace pulse::study::multiplex_ventilation
{
  bool ExtractInitialConditions(const std::string& filePath, double& comp, double& imp);
  bool ExtractVentilatorSettings(const std::string& filePath, std::string& fileName, double& pip_cmH2O, double& peep_cmH2O, double& FiO2);

  struct Dir
  {
    static const std::string Base;
    static const std::string Solo;
    static const std::string Results;
  };

  class PULSE_DECL MVEngine : public Loggable, public SEEventHandler
  {
    friend class MVRunner;
    friend class MVGenerator;
  public:
    MVEngine(std::string const& logfile = "", bool cout_enabled = true, std::string const& data_dir = "./");
    ~MVEngine();

    bool CreateEngine(const std::string& simulationDataStr /*eSerializationFormat fmt*/);
    bool CreateEngine(pulse::study::bind::multiplex_ventilation::SimulationData& sim);

    bool AdvanceTime(double time_s);

    bool ProcessAction(const SEAction& action);
    bool ProcessActions(std::string const& actions, eSerializationFormat format);

    std::string GetSimulationState(/*eSerializationFormat fmt*/);
    bool GetSimulationState(pulse::study::bind::multiplex_ventilation::SimulationData& sim);

    void DestroyEngines();

    static void TrackData(SEDataRequestManager& drMgr);
    static bool RunSoloState(const std::string& stateFile, const std::string& outDir, double duration_s, Logger& logger);

    static bool SerializeToString(pulse::study::bind::multiplex_ventilation::SimulationData& src, std::string& dst /*eSerializationFormat fmt*/);
    static bool SerializeFromString(const std::string& src, pulse::study::bind::multiplex_ventilation::SimulationData& dst /*eSerializationFormat fmt*/);

  protected:
    void SetFiO2(double fio2);
    double GetMinSpO2();
    double GetMinPAO2_mmHg();
    void HandleEvent(eEvent e, bool active, const SEScalarTime* simTime = nullptr) override;


    std::string m_DataDir;
    std::vector<pulse::Engine*> m_Engines;
    std::vector<pulse::Controller*> m_Controllers;
    std::vector<SELiquidSubstanceQuantity*> m_AortaO2s;
    std::vector<SELiquidSubstanceQuantity*> m_AortaCO2s;
    SEMechanicalVentilatorConfiguration* m_MVC;
    SESubstanceFraction* m_FiO2;


    double                    m_TimeStep_s;
    double                    m_CurrentTime_s;
    double                    m_SpareAdvanceTime_s;
    // Substances
    SESubstanceManager* m_SubMgr;
    SESubstance* m_Oxygen;
    // Circuits
    SECircuitManager* m_CircuitMgr;
    SEFluidCircuit* m_MultiplexVentilationCircuit;
    SEFluidCircuitCalculator* m_Calculator;
    // Compartments
    SECompartmentManager* m_CmptMgr;
    SEGasCompartmentGraph* m_MultiplexVentilationGraph;
    SEGasTransporter* m_Transporter;

    pulse::study::bind::multiplex_ventilation::SimulationData* m_SimulationData;
  };
}
