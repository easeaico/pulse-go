/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
class SEEventHandler;
class SENutrition;

// Keep enums in sync with appropriate schema/cdm/PatientEnums.proto file !!
enum class ePatient_Sex { Male = 0, Female };
extern CDM_DECL const std::string& ePatient_Sex_Name(ePatient_Sex m);

class CDM_DECL SEPatient : public Loggable
{
  friend class PBPatient;//friend the serialization class
public:

  SEPatient(Logger* logger);
  virtual ~SEPatient();

  virtual void Clear();
  void Copy(const SEPatient& src);

  bool SerializeToString(std::string& output, eSerializationFormat m) const;
  bool SerializeToFile(const std::string& filename) const;
  bool SerializeFromString(const std::string& src, eSerializationFormat m);
  bool SerializeFromFile(const std::string& filename);

  /** @name GetScalar
  *   @brief - A reflextion type call that will return the Scalar associated
  *            with the string. ex. GetScalar("Hematocrit") will return the
  *            SEScalarPercent object associated with Hematocrit
  *   @details - Note this is an expensive call as it will string compare many times
  *              This is best used, and intended for, you to dynamically prepopulate
  *              a mapping data structure that will help access what you need
  */
  virtual const SEScalar* GetScalar(const std::string& name);
  
  virtual std::string GetName() const;
  virtual void SetName(const std::string& name);
  virtual bool HasName() const;
  virtual void InvalidateName();

  virtual ePatient_Sex GetSex() const;
  virtual void SetSex(ePatient_Sex sex);
  
  virtual bool HasAge() const;
  virtual SEScalarTime& GetAge();
  virtual double GetAge(const TimeUnit& unit) const;
  
  virtual bool HasWeight() const;
  virtual SEScalarMass& GetWeight();
  virtual double GetWeight(const MassUnit& unit) const;
  
  virtual bool HasHeight() const;
  virtual SEScalarLength& GetHeight();
  virtual double GetHeight(const LengthUnit& unit) const;

  virtual bool HasAlveoliSurfaceArea() const;
  virtual SEScalarArea& GetAlveoliSurfaceArea();
  virtual double GetAlveoliSurfaceArea(const AreaUnit& unit) const;
  
  virtual bool HasBasalMetabolicRate() const;
  virtual SEScalarPower& GetBasalMetabolicRate();
  virtual double GetBasalMetabolicRate(const PowerUnit& unit) const;

  virtual bool HasBloodVolumeBaseline() const;
  virtual SEScalarVolume& GetBloodVolumeBaseline();
  virtual double GetBloodVolumeBaseline(const VolumeUnit& unit) const;

  virtual bool HasBodyDensity() const;
  virtual SEScalarMassPerVolume& GetBodyDensity();
  virtual double GetBodyDensity(const MassPerVolumeUnit& unit) const;

  virtual bool HasBodyFatFraction() const;
  virtual SEScalar0To1& GetBodyFatFraction();
  virtual double GetBodyFatFraction() const;

  virtual bool HasBodyMassIndex() const;
  virtual SEScalar& GetBodyMassIndex();
  virtual double GetBodyMassIndex() const;

  virtual bool HasDiastolicArterialPressureBaseline() const;
  virtual SEScalarPressure& GetDiastolicArterialPressureBaseline();
  virtual double GetDiastolicArterialPressureBaseline(const PressureUnit& unit) const;

  virtual bool HasExpiratoryReserveVolume() const;
  virtual SEScalarVolume& GetExpiratoryReserveVolume();
  virtual double GetExpiratoryReserveVolume(const VolumeUnit& unit) const;
    
  virtual bool HasFunctionalResidualCapacity() const;
  virtual SEScalarVolume& GetFunctionalResidualCapacity();
  virtual double GetFunctionalResidualCapacity(const VolumeUnit& unit) const;

  virtual bool HasHeartRateBaseline() const;
  virtual SEScalarFrequency& GetHeartRateBaseline();
  virtual double GetHeartRateBaseline(const FrequencyUnit& unit) const;

  virtual bool HasHeartRateMaximum() const;
  virtual SEScalarFrequency& GetHeartRateMaximum();
  virtual double GetHeartRateMaximum(const FrequencyUnit& unit) const;

  virtual bool HasHeartRateMinimum() const;
  virtual SEScalarFrequency& GetHeartRateMinimum();
  virtual double GetHeartRateMinimum(const FrequencyUnit& unit) const;

  virtual bool HasIdealBodyWeight() const;
  virtual SEScalarMass& GetIdealBodyWeight();
  virtual double GetIdealBodyWeight(const MassUnit& unit) const;
  
  virtual bool HasInspiratoryCapacity() const;
  virtual SEScalarVolume& GetInspiratoryCapacity();
  virtual double GetInspiratoryCapacity(const VolumeUnit& unit) const;

  virtual bool HasInspiratoryReserveVolume() const;
  virtual SEScalarVolume& GetInspiratoryReserveVolume();
  virtual double GetInspiratoryReserveVolume(const VolumeUnit& unit) const;

  virtual bool HasLeanBodyMass() const;
  virtual SEScalarMass& GetLeanBodyMass();
  virtual double GetLeanBodyMass(const MassUnit& unit) const;

  virtual bool HasMeanArterialPressureBaseline() const;
  virtual SEScalarPressure& GetMeanArterialPressureBaseline();
  virtual double GetMeanArterialPressureBaseline(const PressureUnit& unit) const;

  virtual bool HasPulsePressureBaseline() const;
  virtual SEScalarPressure& GetPulsePressureBaseline();
  virtual double GetPulsePressureBaseline(const PressureUnit& unit) const;  

  virtual bool HasResidualVolume() const;
  virtual SEScalarVolume& GetResidualVolume();
  virtual double GetResidualVolume(const VolumeUnit& unit) const;
  
  virtual bool HasRespirationRateBaseline() const;
  virtual SEScalarFrequency& GetRespirationRateBaseline();
  virtual double GetRespirationRateBaseline(const FrequencyUnit& unit) const;
  
  virtual bool HasRightLungRatio() const;
  virtual SEScalar0To1& GetRightLungRatio();
  virtual double GetRightLungRatio() const;

  virtual bool HasSkinSurfaceArea() const;
  virtual SEScalarArea& GetSkinSurfaceArea();
  virtual double GetSkinSurfaceArea(const AreaUnit& unit) const;
  
  virtual bool HasSystolicArterialPressureBaseline() const;
  virtual SEScalarPressure& GetSystolicArterialPressureBaseline();
  virtual double GetSystolicArterialPressureBaseline(const PressureUnit& unit) const;

  virtual bool HasTidalVolumeBaseline() const;
  virtual SEScalarVolume& GetTidalVolumeBaseline();
  virtual double GetTidalVolumeBaseline(const VolumeUnit& unit) const;
  
  virtual bool HasTotalLungCapacity() const;
  virtual SEScalarVolume& GetTotalLungCapacity();
  virtual double GetTotalLungCapacity(const VolumeUnit& unit) const;

  virtual bool HasVitalCapacity() const;
  virtual SEScalarVolume& GetVitalCapacity();
  virtual double GetVitalCapacity(const VolumeUnit& unit) const;

protected:

  std::string                m_Name;
  ePatient_Sex               m_Sex;
  SEScalarTime*              m_Age;
  SEScalarMass*              m_Weight;
  SEScalarLength*            m_Height;
  SEScalarMassPerVolume*     m_BodyDensity;
  SEScalar0To1*              m_BodyFatFraction;
  SEScalar*                  m_BodyMassIndex;
  SEScalarMass*              m_LeanBodyMass;
  SEScalarMass*              m_IdealBodyWeight;

  SEScalarArea*              m_AlveoliSurfaceArea;
  SEScalar0To1*              m_RightLungRatio;
  SEScalarArea*              m_SkinSurfaceArea;

  SEScalarPower*             m_BasalMetabolicRate;
  SEScalarVolume*            m_BloodVolumeBaseline;
  SEScalarPressure*          m_DiastolicArterialPressureBaseline;
  SEScalarFrequency*         m_HeartRateBaseline;
  SEScalarPressure*          m_MeanArterialPressureBaseline;
  SEScalarPressure*          m_PulsePressureBaseline;
  SEScalarFrequency*         m_RespirationRateBaseline;
  SEScalarPressure*          m_SystolicArterialPressureBaseline;
  SEScalarVolume*            m_TidalVolumeBaseline;

  SEScalarFrequency*         m_HeartRateMaximum;
  SEScalarFrequency*         m_HeartRateMinimum;
  SEScalarVolume*            m_ExpiratoryReserveVolume;
  SEScalarVolume*            m_FunctionalResidualCapacity;
  SEScalarVolume*            m_InspiratoryCapacity;
  SEScalarVolume*            m_InspiratoryReserveVolume;
  SEScalarVolume*            m_ResidualVolume;
  SEScalarVolume*            m_TotalLungCapacity;
  SEScalarVolume*            m_VitalCapacity;
};

inline std::ostream& operator<< (std::ostream& out, const SEPatient& p)
{
  std::string s;
  if (!p.SerializeToString(s, eSerializationFormat::TEXT))
    p.Error("Unable to serialize patient");
  out << s;
  return out;
}
