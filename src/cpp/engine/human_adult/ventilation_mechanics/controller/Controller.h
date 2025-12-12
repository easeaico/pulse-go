/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

#include "engine/human_adult/whole_body/controller/Controller.h"
#include "engine/human_adult/ventilation_mechanics/Engine.h"

namespace pulse { namespace human_adult_ventilation_mechanics
{
  /**
  * @brief Manages and controls execution of all data/systems in an engine
  */
  class PULSE_DECL Controller : public pulse::human_adult_whole_body::Controller
  {
    friend Engine;
  protected:// Create via PulseEngine
    Controller(Logger* logger = nullptr);
  public:
    virtual ~Controller() = default;

    bool SerializeFromFile(const std::string& file, const SEDataRequestManager* drMgr=nullptr) override;
    bool SerializeToFile(const std::string& file) const override;

    bool SerializeFromString(const std::string& state, eSerializationFormat m, const SEDataRequestManager* drMgr=nullptr) override;
    bool SerializeToString(std::string& state, eSerializationFormat m) const override;

    bool GetPatientAssessment(SEPatientAssessment& assessment) const override;

    bool IsAirwayModeSupported(pulse::eAirwayMode mode) override;

    bool CreateCircuitsAndCompartments() override;

  protected:
    std::string GetTypeName() const override { return "Human Adult Ventilation Mechanics"; }
    void Allocate() override;
    bool Stabilize(const SEPatientConfiguration& patient_configuration) override;

    void InitializeModels() override { pulse::Controller::InitializeModels(); }
  };
END_NAMESPACE_EX
