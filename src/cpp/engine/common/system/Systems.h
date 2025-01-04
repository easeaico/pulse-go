/* Distributed under the Apache License, Version 2.0.
See accompanying NOTICE file for details.*/

#pragma once

#include "engine/CommonDefs.h"
#include "engine/common/controller/Controller.h"
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
#include "cdm/system/environment/SEEnvironment.h"
#include "cdm/system/equipment/anesthesia_machine/SEAnesthesiaMachine.h"
#include "cdm/system/equipment/bag_valve_mask/SEBagValveMask.h"
#include "cdm/system/equipment/ecmo/SEECMO.h"
#include "cdm/system/equipment/electrocardiogram/SEElectroCardioGram.h"
#include "cdm/system/equipment/inhaler/SEInhaler.h"
#include "cdm/system/equipment/mechanical_ventilator/SEMechanicalVentilator.h"
#include "cdm/properties/SEScalarVolumePerPressure.h"
#include "cdm/properties/SEScalarPressureTimePerVolume.h"

// These classes are used to expose any methodology specific data to end users
// This data would not belong in the cdm, and it's not a clinical assessment, nor on a compartment
// It is data specific to the system implementation
// Users only get this data if they are using the pulse engine
// They should only contain const pure vitrual methods


namespace pulse
{
  class PULSE_DECL BloodChemistrySystem : public SEBloodChemistrySystem
  {
  public:
    BloodChemistrySystem(Logger* logger=nullptr) : SEBloodChemistrySystem(logger) {}
    virtual ~BloodChemistrySystem() = default;

    void Clear() override
    {
      SEBloodChemistrySystem::Clear();
    }

    virtual const SEScalar* GetScalar(const std::string& name) override
    {
      const SEScalar* s = SEBloodChemistrySystem::GetScalar(name);
      if (s != nullptr)
        return s;
      // Check to see if this a model specific request
      //if (name.compare("ModelParameter") == 0)
      //  return m_ModelParameter;
      return nullptr;
    }
    virtual void ComputeExposedModelParameters()=0;
  protected:
    //SEScalar m_ModelParameter;
  };

  class PULSE_DECL CardiovascularSystem : public SECardiovascularSystem
  {
  public:
    CardiovascularSystem(Logger* logger=nullptr) : SECardiovascularSystem(logger) {}
    virtual ~CardiovascularSystem() = default;

    void Clear() override
    {
      SECardiovascularSystem::Clear();
    }

    virtual const SEScalar* GetScalar(const std::string & name) override
    {
      const SEScalar* s = SECardiovascularSystem::GetScalar(name);
      if (s != nullptr)
        return s;
      // Check to see if this a model specific request
      //if (name.compare("ModelParameter") == 0)
      //  return m_ModelParameter;
      return nullptr;
    }
    virtual void ComputeExposedModelParameters() = 0;
  protected:
    //SEScalar m_ModelParameter;
  };

  class PULSE_DECL DrugSystem : public SEDrugSystem
  {
  public:
    DrugSystem(Logger* logger=nullptr) : SEDrugSystem(logger) {}
    virtual ~DrugSystem() = default;

    void Clear() override
    {
      SEDrugSystem::Clear();
    }

    virtual const SEScalar* GetScalar(const std::string & name) override
    {
      const SEScalar* s = SEDrugSystem::GetScalar(name);
      if (s != nullptr)
        return s;
      // Check to see if this a model specific request
      //if (name.compare("ModelParameter") == 0)
      //  return m_ModelParameter;
      return nullptr;
    }
    virtual void ComputeExposedModelParameters() = 0;
  protected:
    //SEScalar m_ModelParameter;
  };

  class PULSE_DECL EndocrineSystem : public SEEndocrineSystem
  {
  public:
    EndocrineSystem(Logger* logger=nullptr) : SEEndocrineSystem(logger) {}
    virtual ~EndocrineSystem() = default;

    void Clear() override
    {
      SEEndocrineSystem::Clear();
    }

    virtual const SEScalar* GetScalar(const std::string & name) override
    {
      const SEScalar* s = SEEndocrineSystem::GetScalar(name);
      if (s != nullptr)
        return s;
      // Check to see if this a model specific request
      //if (name.compare("ModelParameter") == 0)
      //  return m_ModelParameter;
      return nullptr;
    }
    virtual void ComputeExposedModelParameters() = 0;
  protected:
    //SEScalar m_ModelParameter;
  };

  class PULSE_DECL EnergySystem : public SEEnergySystem
  {
  public:
    EnergySystem(Logger* logger=nullptr) : SEEnergySystem(logger) {}
    virtual ~EnergySystem() = default;

    void Clear() override
    {
      SEEnergySystem::Clear();
    }

    virtual const SEScalar* GetScalar(const std::string & name) override
    {
      const SEScalar* s = SEEnergySystem::GetScalar(name);
      if (s != nullptr)
        return s;
      // Check to see if this a model specific request
      //if (name.compare("ModelParameter") == 0)
      //  return m_ModelParameter;
      return nullptr;
    }
    virtual void ComputeExposedModelParameters() = 0;
  protected:
    //SEScalar m_ModelParameter;
  };

  class PULSE_DECL GastrointestinalSystem : public SEGastrointestinalSystem
  {
  public:
    GastrointestinalSystem(Logger* logger=nullptr) : SEGastrointestinalSystem(logger) {}
    virtual ~GastrointestinalSystem() = default;

    void Clear() override
    {
      SEGastrointestinalSystem::Clear();
    }

    virtual const SEScalar* GetScalar(const std::string & name) override
    {
      const SEScalar* s = SEGastrointestinalSystem::GetScalar(name);
      if (s != nullptr)
        return s;
      // Check to see if this a model specific request
      //if (name.compare("ModelParameter") == 0)
      //  return m_ModelParameter;
      return nullptr;
    }
    virtual void ComputeExposedModelParameters() = 0;
  protected:
    //SEScalar m_ModelParameter;
  };

  class PULSE_DECL HepaticSystem : public SEHepaticSystem
  {
  public:
    HepaticSystem(Logger* logger=nullptr) : SEHepaticSystem(logger) {}
    virtual ~HepaticSystem() = default;

    void Clear() override
    {
      SEHepaticSystem::Clear();
    }

    virtual const SEScalar* GetScalar(const std::string & name) override
    {
      const SEScalar* s = SEHepaticSystem::GetScalar(name);
      if (s != nullptr)
        return s;
      // Check to see if this a model specific request
      //if (name.compare("ModelParameter") == 0)
      //  return m_ModelParameter;
      return nullptr;
    }
    virtual void ComputeExposedModelParameters() = 0;
  protected:
    //SEScalar m_ModelParameter;
  };

  class PULSE_DECL NervousSystem : public SENervousSystem
  {
  public:
    NervousSystem(Logger* logger=nullptr) : SENervousSystem(logger) {}
    virtual ~NervousSystem() = default;

    void Clear() override
    {
      SENervousSystem::Clear();
    }

    virtual const SEScalar* GetScalar(const std::string & name) override
    {
      const SEScalar* s = SENervousSystem::GetScalar(name);
      if (s != nullptr)
        return s;
      // Check to see if this a model specific request
      //if (name.compare("ModelParameter") == 0)
      //  return m_ModelParameter;
      return nullptr;
    }
    virtual void ComputeExposedModelParameters() = 0;
  protected:
    //SEScalar m_ModelParameter;
  };

  class PULSE_DECL RenalSystem : public SERenalSystem
  {
  public:
    RenalSystem(Logger* logger=nullptr) : SERenalSystem(logger) {}
    virtual ~RenalSystem() = default;

    void Clear() override
    {
      SERenalSystem::Clear();
    }

    virtual const SEScalar* GetScalar(const std::string & name) override
    {
      const SEScalar* s = SERenalSystem::GetScalar(name);
      if (s != nullptr)
        return s;
      // Check to see if this a model specific request
      //if (name.compare("ModelParameter") == 0)
      //  return m_ModelParameter;
      return nullptr;
    }
    virtual void ComputeExposedModelParameters() = 0;
  protected:
    //SEScalar m_ModelParameter;
  };

  class PULSE_DECL RespiratorySystem : public SERespiratorySystem
  {
  public:
    RespiratorySystem(Logger* logger=nullptr) : SERespiratorySystem(logger) {}
    virtual ~RespiratorySystem() = default;

    void Clear() override
    {
      SERespiratorySystem::Clear();
      m_TotalRespiratoryModelCompliance.Invalidate();
      m_TotalRespiratoryModelInspiratoryResistance.Invalidate();
      m_TotalRespiratoryModelExpiratoryResistance.Invalidate();
    }

    virtual const SEScalar* GetScalar(const std::string & name) override
    {
      const SEScalar* s = SERespiratorySystem::GetScalar(name);
      if (s != nullptr)
        return s;
      // Check to see if this a model specific request
      if (name.compare("TotalRespiratoryModelCompliance") == 0)
        return &m_TotalRespiratoryModelCompliance;
      if (name.compare("TotalRespiratoryModelInspiratoryResistance") == 0)
        return &m_TotalRespiratoryModelInspiratoryResistance;
      if (name.compare("TotalRespiratoryModelExpiratoryResistance") == 0)
        return &m_TotalRespiratoryModelExpiratoryResistance;
      return nullptr;
    }
    virtual void ComputeExposedModelParameters() = 0;

    DEFINE_UNIT_SCALAR(TotalRespiratoryModelCompliance, VolumePerPressure);
    DEFINE_UNIT_SCALAR(TotalRespiratoryModelInspiratoryResistance, PressureTimePerVolume);
    DEFINE_UNIT_SCALAR(TotalRespiratoryModelExpiratoryResistance, PressureTimePerVolume);
  };

  class PULSE_DECL TissueSystem : public SETissueSystem
  {
  public:
    TissueSystem(Logger* logger=nullptr) : SETissueSystem(logger) {}
    virtual ~TissueSystem() = default;

    void Clear() override
    {
      SETissueSystem::Clear();
    }

    virtual const SEScalar* GetScalar(const std::string & name) override
    {
      const SEScalar* s = SETissueSystem::GetScalar(name);
      if (s != nullptr)
        return s;
      // Check to see if this a model specific request
      //if (name.compare("ModelParameter") == 0)
      //  return m_ModelParameter;
      return nullptr;
    }
    virtual void ComputeExposedModelParameters() = 0;
  protected:
    //SEScalar m_ModelParameter;
  };

  class PULSE_DECL EnvironmentSystem : public SEEnvironment
  {
  public:
    EnvironmentSystem(Logger* logger=nullptr) : SEEnvironment(logger) {}
    virtual ~EnvironmentSystem() = default;

    void Clear() override
    {
      SEEnvironment::Clear();
    }

    virtual const SEScalar* GetScalar(const std::string & name) override
    {
      const SEScalar* s = SEEnvironment::GetScalar(name);
      if (s != nullptr)
        return s;
      // Check to see if this a model specific request
      //if (name.compare("ModelParameter") == 0)
      //  return m_ModelParameter;
      return nullptr;
    }
    virtual void ComputeExposedModelParameters() = 0;
  protected:
    //SEScalar m_ModelParameter;
  };

  class PULSE_DECL AnesthesiaMachineSystem : public SEAnesthesiaMachine
  {
  public:
    AnesthesiaMachineSystem(Logger* logger=nullptr) : SEAnesthesiaMachine(logger) {}
    virtual ~AnesthesiaMachineSystem() = default;

    void Clear() override
    {
      SEAnesthesiaMachine::Clear();
    }

    virtual const SEScalar* GetScalar(const std::string & name) override
    {
      const SEScalar* s = SEAnesthesiaMachine::GetScalar(name);
      if (s != nullptr)
        return s;
      // Check to see if this a model specific request
      //if (name.compare("ModelParameter") == 0)
      //  return m_ModelParameter;
      return nullptr;
    }
    virtual void ComputeExposedModelParameters() = 0;
  protected:
    //SEScalar m_ModelParameter;
  };

  class PULSE_DECL BagValveMaskSystem : public SEBagValveMask
  {
  public:
    BagValveMaskSystem(Logger* logger = nullptr) : SEBagValveMask(logger) {}
    virtual ~BagValveMaskSystem() = default;

    void Clear() override
    {
      SEBagValveMask::Clear();
    }

    virtual const SEScalar* GetScalar(const std::string& name) override
    {
      const SEScalar* s = SEBagValveMask::GetScalar(name);
      if (s != nullptr)
        return s;
      // Check to see if this a model specific request
      //if (name.compare("ModelParameter") == 0)
      //  return m_ModelParameter;
      return nullptr;
    }
    virtual void ComputeExposedModelParameters() = 0;
  protected:
    //SEScalar m_ModelParameter;
  };

  class PULSE_DECL ECMOSystem : public SEECMO
  {
  public:
    ECMOSystem(Logger* logger=nullptr) : SEECMO(logger) {}
    virtual ~ECMOSystem() = default;

    void Clear() override
    {
      SEECMO::Clear();
    }

    virtual const SEScalar* GetScalar(const std::string & name) override
    {
      const SEScalar* s = SEECMO::GetScalar(name);
      if (s != nullptr)
        return s;
      // Check to see if this a model specific request
      //if (name.compare("ModelParameter") == 0)
      //  return m_ModelParameter;
      return nullptr;
    }
    virtual void ComputeExposedModelParameters() = 0;
  protected:
    //SEScalar m_ModelParameter;
  };

  class PULSE_DECL ElectroCardioGramSystem : public SEElectroCardioGram
  {
  public:
    ElectroCardioGramSystem(Logger* logger=nullptr) : SEElectroCardioGram(logger) {}
    virtual ~ElectroCardioGramSystem() = default;

    void Clear() override
    {
      SEElectroCardioGram::Clear();
    }

    virtual const SEScalar* GetScalar(const std::string & name) override
    {
      const SEScalar* s = SEElectroCardioGram::GetScalar(name);
      if (s != nullptr)
        return s;
      // Check to see if this a model specific request
      //if (name.compare("ModelParameter") == 0)
      //  return m_ModelParameter;
      return nullptr;
    }
    virtual void ComputeExposedModelParameters() = 0;
  protected:
    //SEScalar m_ModelParameter;
  };

  class PULSE_DECL InhalerSystem : public SEInhaler
  {
  public:
    InhalerSystem(Logger* logger=nullptr) : SEInhaler(logger) {}
    virtual ~InhalerSystem() = default;

    void Clear() override
    {
      SEInhaler::Clear();
    }

    virtual const SEScalar* GetScalar(const std::string & name) override
    {
      const SEScalar* s = SEInhaler::GetScalar(name);
      if (s != nullptr)
        return s;
      // Check to see if this a model specific request
      //if (name.compare("ModelParameter") == 0)
      //  return m_ModelParameter;
      return nullptr;
    }
    virtual void ComputeExposedModelParameters() = 0;
  protected:
    //SEScalar m_ModelParameter;
  };

  class PULSE_DECL MechanicalVentilatorSystem : public SEMechanicalVentilator
  {
  public:
    MechanicalVentilatorSystem(Logger* logger=nullptr) : SEMechanicalVentilator(logger) {}
    virtual ~MechanicalVentilatorSystem() = default;

    void Clear() override
    {
      SEMechanicalVentilator::Clear();
    }

    virtual const SEScalar* GetScalar(const std::string & name) override
    {
      const SEScalar* s = SEMechanicalVentilator::GetScalar(name);
      if (s != nullptr)
        return s;
      // Check to see if this a model specific request
      //if (name.compare("ModelParameter") == 0)
      //  return m_ModelParameter;
      return nullptr;
    }
    virtual void ComputeExposedModelParameters() = 0;
  protected:
    //SEScalar m_ModelParameter;
  };
END_NAMESPACE