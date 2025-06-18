/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include "cdm/patient/actions/SEPatientAction.h"

// Keep enums in sync with appropriate schema/cdm/PatientActionEnums.proto file !!
enum class eAirwayObstruction_ResistanceType
{
  Constant = 0,
  Oscillating = 1
};
extern CDM_DECL const std::string& eAirwayObstruction_ResistanceType_Name(eAirwayObstruction_ResistanceType c);

class CDM_DECL SEAirwayObstruction : public SEPatientAction
{
  friend class PBPatientAction;//friend the serialization class
public:

  SEAirwayObstruction(Logger* logger=nullptr);
  virtual ~SEAirwayObstruction();

  static constexpr char const* Name = "Airway Obstruction";
  std::string GetName() const override { return Name; }

  void Clear() override;
  void Copy(const SEAirwayObstruction& src, bool /*preserveState*/=false);

  const SEScalar* GetScalar(const std::string& name) override;

  bool IsValid() const override;
  bool IsActive() const override;
  void Deactivate() override;

  virtual bool HasSeverity() const;
  virtual SEScalar0To1& GetSeverity();
  virtual double GetSeverity() const;

  virtual eAirwayObstruction_ResistanceType GetResistanceType() const;
  virtual void SetResistanceType(eAirwayObstruction_ResistanceType rt);

protected:
  SEScalar0To1*                     m_Severity;
  eAirwayObstruction_ResistanceType m_ResistanceType;
};
