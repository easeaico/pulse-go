/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include "cdm/properties/SEScalarUnsigned.h"
class SECardiovascularMechanicsModification;

class CDM_DECL SECardiovascularMechanicsModifiers : public Loggable
{
  friend class PBPhysiology;
public:

  SECardiovascularMechanicsModifiers(Logger* logger);
  virtual ~SECardiovascularMechanicsModifiers();

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

  virtual bool HasArterialComplianceMultiplier() const;
  virtual SEScalarUnsigned& GetArterialComplianceMultiplier();
  virtual double GetArterialComplianceMultiplier() const;

  virtual bool HasArterialResistanceMultiplier() const;
  virtual SEScalarUnsigned& GetArterialResistanceMultiplier();
  virtual double GetArterialResistanceMultiplier() const;

  virtual bool HasPulmonaryComplianceMultiplier() const;
  virtual SEScalarUnsigned& GetPulmonaryComplianceMultiplier();
  virtual double GetPulmonaryComplianceMultiplier() const;;

  virtual bool HasPulmonaryResistanceMultiplier() const;
  virtual SEScalarUnsigned& GetPulmonaryResistanceMultiplier();
  virtual double GetPulmonaryResistanceMultiplier() const;

  virtual bool HasSystemicResistanceMultiplier() const;
  virtual SEScalarUnsigned& GetSystemicResistanceMultiplier();
  virtual double GetSystemicResistanceMultiplier() const;

  virtual bool HasSystemicComplianceMultiplier() const;
  virtual SEScalarUnsigned& GetSystemicComplianceMultiplier();
  virtual double GetSystemicComplianceMultiplier() const;

  virtual bool HasVenousComplianceMultiplier() const;
  virtual SEScalarUnsigned& GetVenousComplianceMultiplier();
  virtual double GetVenousComplianceMultiplier() const;

  virtual bool HasVenousResistanceMultiplier() const;
  virtual SEScalarUnsigned& GetVenousResistanceMultiplier();
  virtual double GetVenousResistanceMultiplier() const;

  virtual bool HasHeartRateMultiplier() const;
  virtual SEScalarUnsigned& GetHeartRateMultiplier();
  virtual double GetHeartRateMultiplier() const;

  virtual bool HasStrokeVolumeMultiplier() const;
  virtual SEScalarUnsigned& GetStrokeVolumeMultiplier();
  virtual double GetStrokeVolumeMultiplier() const;

  virtual std::string ToString() const;
  virtual void ToString(std::ostream& str) const;

protected:
  SEScalarUnsigned* m_ArterialComplianceMultiplier;
  SEScalarUnsigned* m_ArterialResistanceMultiplier;
  SEScalarUnsigned* m_PulmonaryComplianceMultiplier;
  SEScalarUnsigned* m_PulmonaryResistanceMultiplier;
  SEScalarUnsigned* m_SystemicResistanceMultiplier;
  SEScalarUnsigned* m_SystemicComplianceMultiplier;
  SEScalarUnsigned* m_VenousComplianceMultiplier;
  SEScalarUnsigned* m_VenousResistanceMultiplier;

  SEScalarUnsigned* m_HeartRateMultiplier;
  SEScalarUnsigned* m_StrokeVolumeMultiplier;
};

inline std::ostream& operator<< (std::ostream& out, const SECardiovascularMechanicsModifiers* s)
{
  if (s == nullptr)
    out << "Null Cardiovascular Mechanics Modifiers" << std::flush;
  else
    (*s).ToString(out);
  return out;
}
inline std::ostream& operator<< (std::ostream& out, const SECardiovascularMechanicsModifiers& s)
{
  s.ToString(out);
  return out;
}
