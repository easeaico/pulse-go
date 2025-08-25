/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#pragma once
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorAction.h"
#include "cdm/system/equipment/mechanical_ventilator/SEMechanicalVentilatorSettings.h"

class CDM_DECL SEMechanicalVentilatorMode : public SEMechanicalVentilatorAction
{
  friend class PBEquipmentAction;//friend the serialization class
public:

  SEMechanicalVentilatorMode(Logger* logger);
  virtual ~SEMechanicalVentilatorMode();

  void Clear() override;

  bool IsValid() const override;
  bool IsActive() const override;

  virtual bool ToSettings(SEMechanicalVentilatorSettings& s,
                          SESubstanceManager& subMgr,
                          eMergeType mt);

  virtual eSwitch GetConnection() const;
  virtual void SetConnection(eSwitch c);

  bool HasSupplementalSettings() const;
  SEMechanicalVentilatorSettings& GetSupplementalSettings();
  const SEMechanicalVentilatorSettings* GetSupplementalSettings() const;

  virtual std::string GetSupplementalSettingsFile() const;
  virtual void SetSupplementalSettingsFile(const std::string& fileName);
  virtual bool HasSupplementalSettingsFile() const;

  virtual eMergeType GetMergeType() const;
  virtual void SetMergeType(eMergeType m);

protected:

  SEMechanicalVentilatorSettings* m_SupplementalSettings;
  std::string                     m_SupplementalSettingsFile;
  eMergeType                      m_MergeType;
};
