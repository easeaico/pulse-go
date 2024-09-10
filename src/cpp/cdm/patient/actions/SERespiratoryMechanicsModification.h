/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include "cdm/patient/actions/SEPatientAction.h"
class SERespiratoryMechanicsModifiers;

class CDM_DECL SERespiratoryMechanicsModification : public SEPatientAction
{
  friend class PBPatientAction;//friend the serialization class
public:

  SERespiratoryMechanicsModification(Logger* logger=nullptr);
  virtual ~SERespiratoryMechanicsModification();

  static constexpr char const* Name = "Respiratory Mechanics Modification";
  std::string GetName() const override { return Name; }

  void Clear() override;
  void Copy(const SERespiratoryMechanicsModification& src, bool /*preserveState*/=false);

  const SEScalar* GetScalar(const std::string& name) override;

  bool IsValid() const override;
  bool IsActive() const override;
  void Deactivate() override;

  bool GetIncremental() const;
  void SetIncremental(bool b);

  bool HasModifiers() const;
  SERespiratoryMechanicsModifiers& GetModifiers();
  const SERespiratoryMechanicsModifiers* GetModifiers() const;

  virtual std::string GetModifiersFile() const;
  virtual void SetModifiersFile(const std::string& fileName);
  virtual bool HasModifiersFile() const;

protected:
  std::string                       m_ModifiersFile;
  SERespiratoryMechanicsModifiers*  m_Modifiers;
  bool                              m_Incremental;
};
