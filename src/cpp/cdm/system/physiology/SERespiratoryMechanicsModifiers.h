/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
class SERespiratoryMechanicsModification;
#include "cdm/system/physiology/SERespiratorySystem.h"

class CDM_DECL SERespiratoryMechanicsModifiers : public Loggable
{
  friend class PBPhysiology;
public:

  SERespiratoryMechanicsModifiers(Logger* logger);
  virtual ~SERespiratoryMechanicsModifiers();

  virtual void Clear();// Deletes all members
  virtual void Activate();
  virtual bool IsActive() const;

  /** @name GetScalar
  *   @brief - A reflextion type call that will return the Scalar associated
  *            with the string. ex. GetScalar("Hematocrit") will return the
  *            SEScalarPercent object associated with Hematocrit
  *   @details - Note this is an expensive call as it will string compare many times
  *              This is best used, and intended for, you to dynamically prepopulate
  *              a mapping data structure that will help access what you need
  */
  virtual const SEScalar* GetScalar(const std::string& name);

  bool SerializeToString(std::string& output, eSerializationFormat m) const;
  bool SerializeToFile(const std::string& filename) const;
  bool SerializeFromString(const std::string& src, eSerializationFormat m);
  bool SerializeFromFile(const std::string& filename);

  virtual bool HasLeftComplianceMultiplier() const;
  virtual SEScalarUnsigned& GetLeftComplianceMultiplier();
  virtual double GetLeftComplianceMultiplier() const;

  virtual bool HasRightComplianceMultiplier() const;
  virtual SEScalarUnsigned& GetRightComplianceMultiplier();
  virtual double GetRightComplianceMultiplier() const;

  virtual bool HasLeftExpiratoryResistanceMultiplier() const;
  virtual SEScalarUnsigned& GetLeftExpiratoryResistanceMultiplier();
  virtual double GetLeftExpiratoryResistanceMultiplier() const;

  virtual bool HasLeftInspiratoryResistanceMultiplier() const;
  virtual SEScalarUnsigned& GetLeftInspiratoryResistanceMultiplier();
  virtual double GetLeftInspiratoryResistanceMultiplier() const;

  virtual bool HasRightExpiratoryResistanceMultiplier() const;
  virtual SEScalarUnsigned& GetRightExpiratoryResistanceMultiplier();
  virtual double GetRightExpiratoryResistanceMultiplier() const;;

  virtual bool HasRightInspiratoryResistanceMultiplier() const;
  virtual SEScalarUnsigned& GetRightInspiratoryResistanceMultiplier();
  virtual double GetRightInspiratoryResistanceMultiplier() const;

  virtual bool HasUpperExpiratoryResistanceMultiplier() const;
  virtual SEScalarUnsigned& GetUpperExpiratoryResistanceMultiplier();
  virtual double GetUpperExpiratoryResistanceMultiplier() const;

  virtual bool HasUpperInspiratoryResistanceMultiplier() const;
  virtual SEScalarUnsigned& GetUpperInspiratoryResistanceMultiplier();
  virtual double GetUpperInspiratoryResistanceMultiplier() const;

  virtual bool HasRespirationRateMultiplier() const;
  virtual SEScalarUnsigned& GetRespirationRateMultiplier();
  virtual double GetRespirationRateMultiplier() const;

  virtual bool HasTidalVolumeMultiplier() const;
  virtual SEScalarUnsigned& GetTidalVolumeMultiplier();
  virtual double GetTidalVolumeMultiplier() const;

  virtual bool HasLungVolumeIncrement() const;
  virtual LungVolumeIncrementMap& GetLungVolumeIncrements();
  virtual const LungVolumeIncrementMap& GetLungVolumeIncrements() const;
  virtual bool HasLungVolumeIncrement(eLungCompartment cmpt) const;
  virtual SEScalarVolume& GetLungVolumeIncrement(eLungCompartment cmpt);
  virtual double GetLungVolumeIncrement(eLungCompartment cmpt, const VolumeUnit& unit) const;

  virtual std::string ToString() const;
  virtual void ToString(std::ostream& str) const;

protected:
  SEScalarUnsigned*      m_LeftComplianceMultiplier;
  SEScalarUnsigned*      m_RightComplianceMultiplier;

  SEScalarUnsigned*      m_LeftExpiratoryResistanceMultiplier;
  SEScalarUnsigned*      m_LeftInspiratoryResistanceMultiplier;
  SEScalarUnsigned*      m_RightExpiratoryResistanceMultiplier;
  SEScalarUnsigned*      m_RightInspiratoryResistanceMultiplier;
  SEScalarUnsigned*      m_UpperExpiratoryResistanceMultiplier;
  SEScalarUnsigned*      m_UpperInspiratoryResistanceMultiplier;

  SEScalarUnsigned*      m_RespirationRateMultiplier;
  SEScalarUnsigned*      m_TidalVolumeMultiplier;

  LungVolumeIncrementMap m_VolumeIncrements;
};

inline std::ostream& operator<< (std::ostream& out, const SERespiratoryMechanicsModifiers* s)
{
  if (s == nullptr)
    out << "Null Respiratory Mechanics Modifiers" << std::flush;
  else
    (*s).ToString(out);
  return out;
}
inline std::ostream& operator<< (std::ostream& out, const SERespiratoryMechanicsModifiers& s)
{
  s.ToString(out);
  return out;
}
