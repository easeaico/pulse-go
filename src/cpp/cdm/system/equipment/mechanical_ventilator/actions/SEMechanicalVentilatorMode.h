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

  virtual bool ToSettings(SEMechanicalVentilatorSettings& s, const SESubstanceManager& subMgr);

  virtual eSwitch GetConnection() const;
  virtual void SetConnection(eSwitch c);

  bool HasSupplementalSettings() const;
  SEMechanicalVentilatorSettings& GetSupplementalSettings();
  const SEMechanicalVentilatorSettings* GetSupplementalSettings() const;

protected:

  eSwitch             m_Connection;
  SEMechanicalVentilatorSettings* m_SupplementalSettings;
};
