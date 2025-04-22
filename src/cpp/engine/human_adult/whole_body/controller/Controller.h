/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

#include "engine/common/controller/Controller.h"
#include "engine/human_adult/whole_body/Engine.h"

namespace pulse { namespace human_adult_whole_body
{
  class EngineTest;
  /**
  * @brief Manages and controls execution of all data/systems in an engine
  */
  class PULSE_DECL Controller : public pulse::Controller
  {
    friend Engine;
    friend EngineTest;
  protected:// Create via Engine
    Controller(Logger* logger = nullptr);
  public:
    virtual ~Controller();

    bool SerializeFromFile(const std::string& file) override;
    bool SerializeToFile(const std::string& file) const override;

    bool SerializeFromString(const std::string& state, eSerializationFormat m) override;
    bool SerializeToString(std::string& state, eSerializationFormat m) const override;

    bool GetPatientAssessment(SEPatientAssessment& assessment) const override;

  protected:
    std::string GetTypeName() const override { return "Human Adult Whole Body"; }
    void Allocate() override;
    bool SetupPatient(const SEPatient& patient) override;

    void InitializeModels() override;
  };
END_NAMESPACE_EX
