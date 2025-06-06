/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#pragma once
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorAction.h"
class SEMechanicalVentilatorSettings;
class SESubstanceManager;

class CDM_DECL SEMechanicalVentilatorConfiguration : public SEMechanicalVentilatorAction
{
  friend class PBEquipmentAction;//friend the serialization class
public:

  SEMechanicalVentilatorConfiguration(Logger* logger=nullptr);
  virtual ~SEMechanicalVentilatorConfiguration();

  static constexpr char const* Name = "Configuration";
  std::string GetName() const override { return Name; }

  void Clear() override;
  virtual void Copy(const SEMechanicalVentilatorConfiguration& src, const SESubstanceManager& subMgr, bool /*preserveState*/ = false);

  bool IsValid() const override;
  bool IsActive() const override;
  void Deactivate() override;

  bool HasSettings() const;
  SEMechanicalVentilatorSettings& GetSettings();
  const SEMechanicalVentilatorSettings* GetSettings() const;

  virtual std::string GetSettingsFile() const;
  virtual void SetSettingsFile(const std::string& fileName);
  virtual bool HasSettingsFile() const;

  virtual eMergeType GetMergeType() const;
  virtual void SetMergeType(eMergeType m);

  const SEScalar* GetScalar(const std::string& name) override;

protected:

  SEMechanicalVentilatorSettings*   m_Settings;
  std::string                       m_SettingsFile;
  eMergeType                        m_MergeType;
};