/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/human_adult/ventilation_mechanics/controller/Controller.h"
#include "engine/human_adult/ventilation_mechanics/controller/CompartmentManager.h"
#include "engine/human_adult/whole_body/controller/CircuitManager.h"
#include "engine/human_adult/whole_body/controller/SubstanceManager.h"
#include "engine/human_adult/whole_body/system/environment/EnvironmentModel.h"
#include "engine/human_adult/whole_body/system/equipment/MechanicalVentilatorModel.h"
#include "engine/human_adult/whole_body/system/physiology/RespiratoryModel.h"

namespace pulse { namespace human_adult_ventilation_mechanics
{
  Controller::Controller(Logger* logger) : pulse::human_adult_whole_body::Controller(logger)
  {

  }
 
  // Serialization is not supported
  bool Controller::SerializeFromFile(const std::string& /*filename*/, const SEDataRequestManager* /*drMgr*/)
  {
    Error("Serialization is not supported in this engine");
    return false;
  }
  bool Controller::SerializeToFile(const std::string& /*filename*/) const
  {
    Error("Serialization is not supported in this engine");
    return false;
  }

  bool Controller::SerializeFromString(const std::string& /*src*/, eSerializationFormat /*m*/, const SEDataRequestManager* /*drMgr*/)
  {
    Error("Serialization is not supported in this engine");
    return false;
  }
  bool Controller::SerializeToString(std::string& /*output*/, eSerializationFormat /*m*/) const
  {
    Error("Serialization is not supported in this engine");
    return false;
  }

  bool Controller::GetPatientAssessment(SEPatientAssessment& /*assessment*/) const
  {
    Error("No assesments are supported in this engine");
    return false;
  }

  bool Controller::IsAirwayModeSupported(pulse::eAirwayMode mode)
  {
    // Only support two modes
    return (mode == pulse::eAirwayMode::Free || mode == pulse::eAirwayMode::MechanicalVentilator);
  }

  void Controller::Allocate()
  {
    pulse::Controller::Allocate();

    // Create our derived objects
    m_Compartments = new CompartmentManager(*this);
    m_Circuits = new pulse::CircuitManager(*this);

    m_EnvironmentModel = new pulse::EnvironmentModel(*this);

    m_RespiratoryModel = new pulse::RespiratoryModel(*this);

    m_MechanicalVentilatorModel = new pulse::MechanicalVentilatorModel(*this);

    // Add in the order to init/pre/post/process
    m_Models.push_back(m_MechanicalVentilatorModel);
    m_Models.push_back(m_RespiratoryModel);

    // Call this after models are setup
    SetupTracker();
  }

  bool Controller::Stabilize(const SEPatientConfiguration& patient_configuration)
  {
    if (patient_configuration.HasConditions() && !patient_configuration.GetConditions()->IsEmpty())
      Warning("Provided conditions will be ignored, this engine does not need to stabilize");

    return true;// Nothing to do here, we are good to go!
  }
END_NAMESPACE_EX
