/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "PulseConfiguration.h"
#include "engine/human_adult/whole_body/controller/Controller.h"
#include "engine/human_adult/whole_body/controller/BlackBoxManager.h"
#include "engine/human_adult/whole_body/controller/CircuitManager.h"
#include "engine/human_adult/whole_body/controller/CompartmentManager.h"
#include "engine/human_adult/whole_body/controller/SubstanceManager.h"
#include "engine/human_adult/whole_body/system/environment/EnvironmentModel.h"
#include "engine/human_adult/whole_body/system/equipment/AnesthesiaMachineModel.h"
#include "engine/human_adult/whole_body/system/equipment/BagValveMaskModel.h"
#include "engine/human_adult/whole_body/system/equipment/ECMOModel.h"
#include "engine/human_adult/whole_body/system/equipment/ElectroCardioGramModel.h"
#include "engine/human_adult/whole_body/system/equipment/InhalerModel.h"
#include "engine/human_adult/whole_body/system/equipment/MechanicalVentilatorModel.h"
#include "engine/human_adult/whole_body/system/physiology/BloodChemistryModel.h"
#include "engine/human_adult/whole_body/system/physiology/CardiovascularModel.h"
#include "engine/human_adult/whole_body/system/physiology/DrugModel.h"
#include "engine/human_adult/whole_body/system/physiology/EndocrineModel.h"
#include "engine/human_adult/whole_body/system/physiology/EnergyModel.h"
#include "engine/human_adult/whole_body/system/physiology/GastrointestinalModel.h"
#include "engine/human_adult/whole_body/system/physiology/HepaticModel.h"
#include "engine/human_adult/whole_body/system/physiology/NervousModel.h"
#include "engine/human_adult/whole_body/system/physiology/RenalModel.h"
#include "engine/human_adult/whole_body/system/physiology/RespiratoryModel.h"
#include "engine/human_adult/whole_body/system/physiology/Saturation.h"
#include "engine/human_adult/whole_body/system/physiology/TissueModel.h"

#include "cdm/patient/assessments/SEArterialBloodGasTest.h"
#include "cdm/patient/assessments/SECompleteBloodCount.h"
#include "cdm/patient/assessments/SEComprehensiveMetabolicPanel.h"
#include "cdm/patient/assessments/SEUrinalysis.h"

namespace pulse { namespace human_adult_whole_body
{
  Controller::Controller(Logger* logger) : pulse::Controller(logger)
  {

  }

  Controller::~Controller()
  {
    Info("[Final SimTime(s)] " + m_SimulationTime.ToString());
  }

  // I am pretty sure we will want different states per engine
  bool Controller::SerializeFromFile(const std::string& filename, const SEDataRequestManager* drMgr)
  {
    return pulse::Controller::SerializeFromFile(filename, drMgr);
  }
  bool Controller::SerializeToFile(const std::string& filename) const
  {
    return pulse::Controller::SerializeToFile(filename);
  }

  bool Controller::SerializeFromString(const std::string& src, eSerializationFormat m, const SEDataRequestManager* drMgr)
  {
    return pulse::Controller::SerializeFromString(src, m, drMgr);
  }
  bool Controller::SerializeToString(std::string& output, eSerializationFormat m) const
  {
    return pulse::Controller::SerializeToString(output, m);
  }

  void Controller::Allocate()
  {
    pulse::Controller::Allocate();

    // Create our derived objects
    m_BlackBoxes = new pulse::BlackBoxManager(*this);
    m_Compartments = new CompartmentManager(*this);
    m_Circuits = new pulse::CircuitManager(*this);

    m_Stabilizer = new pulse::StabilizationController(*this);

    m_SaturationCalculator = new pulse::SaturationCalculator(*this);

    m_BloodChemistryModel = new BloodChemistryModel(*this);
    m_CardiovascularModel = new CardiovascularModel(*this);
    m_EndocrineModel = new EndocrineModel(*this);
    m_EnergyModel = new EnergyModel(*this);
    m_GastrointestinalModel = new GastrointestinalModel(*this);
    m_HepaticModel = new HepaticModel(*this);
    m_NervousModel = new NervousModel(*this);
    m_RenalModel = new RenalModel(*this);
    m_RespiratoryModel = new RespiratoryModel(*this);
    m_DrugModel = new DrugModel(*this);
    m_TissueModel = new TissueModel(*this);

    m_EnvironmentModel = new EnvironmentModel(*this);

    m_AnesthesiaMachineModel = new AnesthesiaMachineModel(*this);
    m_BagValveMaskModel = new BagValveMaskModel(*this);
    m_ECMOModel = new ECMOModel(*this);
    m_ElectroCardioGramModel = new ElectroCardioGramModel(*this);
    m_InhalerModel = new InhalerModel(*this);
    m_MechanicalVentilatorModel = new MechanicalVentilatorModel(*this);

    // Add in the order to init/pre/post/process
    m_Models.push_back(m_EnvironmentModel);
    m_Models.push_back(m_CardiovascularModel);
    m_Models.push_back(m_InhalerModel);
    m_Models.push_back(m_RespiratoryModel);
    m_Models.push_back(m_AnesthesiaMachineModel);
    m_Models.push_back(m_BagValveMaskModel);
    m_Models.push_back(m_MechanicalVentilatorModel);
    m_Models.push_back(m_GastrointestinalModel);
    m_Models.push_back(m_HepaticModel);
    m_Models.push_back(m_RenalModel);
    m_Models.push_back(m_NervousModel);
    m_Models.push_back(m_EnergyModel);
    m_Models.push_back(m_EndocrineModel);
    m_Models.push_back(m_DrugModel);
    m_Models.push_back(m_BloodChemistryModel);
    m_Models.push_back(m_TissueModel);
    m_Models.push_back(m_ElectroCardioGramModel);
    m_Models.push_back(m_ECMOModel);

    m_EngineTracker = new SEEngineTracker(m_Logger);
    SetupTracker();
  }

  void Controller::InitializeModels()
  {
    // Environment (idx=0) is cleared and initialized in circuit setup
    for (size_t i = 1; i < m_Models.size(); i++)
      m_Models[i]->Clear();

    Info("Initializing Models");
    for (size_t i = 1; i < m_Models.size(); i++)
      m_Models[i]->Initialize();
  }

  bool Controller::GetPatientAssessment(SEPatientAssessment& assessment) const
  {
    if (!IsReady())
      return false;

    SEArterialBloodGasTest* abg = dynamic_cast<SEArterialBloodGasTest*>(&assessment);
    if (abg != nullptr)
      return m_BloodChemistryModel->CalculateArterialBloodGasTest(*abg);

    SECompleteBloodCount* cbc = dynamic_cast<SECompleteBloodCount*>(&assessment);
    if (cbc != nullptr)
      return m_BloodChemistryModel->CalculateCompleteBloodCount(*cbc);

    SEComprehensiveMetabolicPanel* cmp = dynamic_cast<SEComprehensiveMetabolicPanel*>(&assessment);
    if (cmp != nullptr)
      return m_BloodChemistryModel->CalculateComprehensiveMetabolicPanel(*cmp);

    SEUrinalysis* u = dynamic_cast<SEUrinalysis*>(&assessment);
    if (u != nullptr)
    {
      return m_RenalModel->CalculateUrinalysis(*u);
    }

    Error("Unsupported patient assessment");
    return false;
  }
END_NAMESPACE_EX
