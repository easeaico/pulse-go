/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "engine/human_adult/hemodynamics/controller/Controller.h"
#include "engine/human_adult/hemodynamics/controller/CompartmentManager.h"
#include "engine/human_adult/whole_body/controller/CircuitManager.h"
#include "engine/human_adult/whole_body/controller/SubstanceManager.h"
#include "engine/human_adult/whole_body/system/physiology/CardiovascularModel.h"
#include "engine/human_adult/whole_body/system/physiology/DrugModel.h"
#include "engine/human_adult/whole_body/system/physiology/EnergyModel.h"
#include "engine/human_adult/whole_body/system/physiology/NervousModel.h"
#include "engine/human_adult/whole_body/system/physiology/Saturation.h"

namespace pulse { namespace human_adult_hemodynamics
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

  bool Controller::IsAirwayModeSupported(pulse::eAirwayMode)
  {
    // No Airway Modes supported
    return false;
  }

  void Controller::Allocate()
  {
    pulse::Controller::Allocate();

    // Create our derived objects
    m_BlackBoxes = new pulse::BlackBoxManager(*this);
    m_Compartments = new CompartmentManager(*this);
    m_Circuits = new pulse::CircuitManager(*this);

    m_CardiovascularModel = new pulse::CardiovascularModel(*this);
    m_Models.push_back(m_CardiovascularModel);

    // Not doing anything with these, but CV looks for data on them
    m_EnergyModel = new pulse::EnergyModel(*this);
    m_DrugModel = new pulse::DrugModel(*this);
    m_NervousModel = new pulse::NervousModel(*this);

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
