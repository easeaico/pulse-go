/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
class SESubstanceManager;
CDM_BIND_DECL2(MechanicalVentilator)
CDM_BIND_DECL2(MechanicalVentilatorSettings)

class CDM_DECL PBMechanicalVentilator
{
public:

  static void Load(const CDM_BIND::MechanicalVentilatorData& src, SEMechanicalVentilator& dst, const SESubstanceManager& subMgr);
  static CDM_BIND::MechanicalVentilatorData* Unload(const SEMechanicalVentilator& src);
  static void Serialize(const CDM_BIND::MechanicalVentilatorData& src, SEMechanicalVentilator& dst, const SESubstanceManager& subMgr);
  static void Serialize(const SEMechanicalVentilator& src, CDM_BIND::MechanicalVentilatorData& dst);

  static void Load(const CDM_BIND::MechanicalVentilatorSettingsData& src, SEMechanicalVentilatorSettings& dst, const SESubstanceManager& subMgr);
  static CDM_BIND::MechanicalVentilatorSettingsData* Unload(const SEMechanicalVentilatorSettings& src);
  static void Serialize(const CDM_BIND::MechanicalVentilatorSettingsData& src, SEMechanicalVentilatorSettings& dst, const SESubstanceManager& subMgr);
  static void Serialize(const SEMechanicalVentilatorSettings& src, CDM_BIND::MechanicalVentilatorSettingsData& dst);
  static void Copy(const SEMechanicalVentilatorSettings& src, SEMechanicalVentilatorSettings& dst, const SESubstanceManager& subMgr);

  static bool SerializeToString(const SEMechanicalVentilatorSettings& src, std::string& output, eSerializationFormat m);
  static bool SerializeToFile(const SEMechanicalVentilatorSettings& src, const std::string& filename);
  static bool SerializeFromString(const std::string& src, SEMechanicalVentilatorSettings& dst, eSerializationFormat m, const SESubstanceManager& subMgr);
  static bool SerializeFromFile(const std::string& filename, SEMechanicalVentilatorSettings& dst, const SESubstanceManager& subMgr);

};