/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include "cdm/system/SESystem.h"

/** @copydoc Physiology_BloodChemistrySystemData
  @nosubgrouping */
class CDM_DECL SEBloodChemistrySystem : public SESystem
{
  friend class PBPhysiology;//friend the serialization class
public:

  SEBloodChemistrySystem(Logger* logger);
  virtual ~SEBloodChemistrySystem();

  void Clear() override;                                                                      /**< @copydoc DOXY_CDM_CLEAR */

  const SEScalar* GetScalar(const std::string& name) override;                                /**< @copydoc DOXY_CDM_GET_SCALAR */

  /** @name ArterialOxygenContent
  *  @brief The total oxygen content in arterial blood, including dissolved and hemoglobin-bound oxygen.
  *  @{*/
  /// Test if member has been allocated
  virtual bool HasArterialOxygenContent() const;
  /// Get member class, allocate if nullptr
  virtual SEScalar& GetArterialOxygenContent();
  virtual double GetArterialOxygenContent() const;
  //@}

  /** @name ArteriovenousOxygenDifference
  *  @brief The difference between arterial and mixed venous oxygen content.
  *  @{*/
  /// Test if member has been allocated
  virtual bool HasArteriovenousOxygenDifference() const;
  /// Get member class, allocate if nullptr
  virtual SEScalar& GetArteriovenousOxygenDifference();
  virtual double GetArteriovenousOxygenDifference() const;
  //@}

  /** @name BaseExcess
  *  @brief @copybrief Physiology_BloodChemistrySystemData_BaseExcess
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasBaseExcess() const;
  /// Get member class, allocate if nullptr
  virtual SEScalarAmountPerVolume& GetBaseExcess();
  virtual double GetBaseExcess(const AmountPerVolumeUnit& unit) const;
  //@}

  /**  @name BloodDensity *///@{ @copybrief Physiology_BloodChemistrySystemData_BloodDensity
  virtual bool HasBloodDensity() const;                                                               /**< @copydoc DOXY_CDM_HAS */
  virtual SEScalarMassPerVolume& GetBloodDensity();                                                   /**< @copydoc DOXY_CDM_GET */
  virtual double GetBloodDensity(const MassPerVolumeUnit& unit) const;                                /**< @copydoc DOXY_CDM_GET_VALUE *///@}

  /** @name BloodPH
  *  @brief @copybrief Physiology_BloodChemistrySystemData_BloodPH
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasBloodPH() const;
  /// Get member class, allocate if nullptr
  virtual SEScalar& GetBloodPH();
  virtual double GetBloodPH() const;
  //@}

  /** @name BloodSpecificHeat
  *  @brief @copybrief Physiology_BloodChemistrySystemData_BloodSpecificHeat
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasBloodSpecificHeat() const;
  /// Get member class, allocate if nullptr
  virtual SEScalarHeatCapacitancePerMass& GetBloodSpecificHeat();
  virtual double GetBloodSpecificHeat(const HeatCapacitancePerMassUnit& unit) const;
  //@}

  /** @name BloodUreaNitrogenConcentration
  *  @brief @copybrief Physiology_BloodChemistrySystemData_BloodUreaNitrogenConcentration
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasBloodUreaNitrogenConcentration() const;
  /// Get member class, allocate if nullptr
  virtual SEScalarMassPerVolume& GetBloodUreaNitrogenConcentration();
  virtual double GetBloodUreaNitrogenConcentration(const MassPerVolumeUnit& unit) const;
  //@}

  /** @name CarbonDioxideSaturation
  *  @brief @copybrief Physiology_BloodChemistrySystemData_CarbonDioxideSaturation
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasCarbonDioxideSaturation() const;
  /// Get member class, allocate if nullptr
  virtual SEScalar0To1& GetCarbonDioxideSaturation();
  virtual double GetCarbonDioxideSaturation() const;
  //@}

  /** @name CarbonMonoxideSaturation
  *  @brief @copybrief Physiology_BloodChemistrySystemData_CarbonMonoxideSaturation
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasCarbonMonoxideSaturation() const;
  /// Get member class, allocate if nullptr
  virtual SEScalar0To1& GetCarbonMonoxideSaturation();
  virtual double GetCarbonMonoxideSaturation() const;
  //@}

  /** @name ClinicalShuntFraction
  *  @brief @copybrief Physiology_BloodChemistrySystemData_ClinicalShuntFraction
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasClinicalShuntFraction() const;
  /// Get member class, allocate if nullptr
  virtual SEScalar0To1& GetClinicalShuntFraction();
  virtual double GetClinicalShuntFraction() const;
  //@}

  /** @name Hematocrit
  *  @brief @copybrief Physiology_BloodChemistrySystemData_Hematocrit
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasHematocrit() const;
  /// Get member class, allocate if nullptr
  virtual SEScalar0To1& GetHematocrit();
  virtual double GetHematocrit() const;
  //@}

  /** @name HemoglobinContent
  *  @brief @copybrief Physiology_BloodChemistrySystemData_HemoglobinContent
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasHemoglobinContent() const;
  /// Get member class, allocate if nullptr
  virtual SEScalarMass& GetHemoglobinContent();
  virtual double GetHemoglobinContent(const MassUnit& unit) const;
  //@}

  /** @name MixedVenousOxygenContent
  *  @brief The total oxygen content in mixed venous blood, including dissolved and hemoglobin-bound oxygen.
  *  @{*/
  /// Test if member has been allocated
  virtual bool HasMixedVenousOxygenContent() const;
  /// Get member class, allocate if nullptr
  virtual SEScalar& GetMixedVenousOxygenContent();
  virtual double GetMixedVenousOxygenContent() const;
  //@}

  /** @name ClinicalOxygenConsumption
  *  @brief The rate at which oxygen is consumed by the tissues, calculated as the product of cardiac output and arteriovenous oxygen difference.
  *  @{*/
  /// Test if member has been allocated
  virtual bool HasClinicalOxygenConsumption() const;
  /// Get member class, allocate if nullptr
  virtual SEScalarVolumePerTime& GetClinicalOxygenConsumption();
  virtual double GetClinicalOxygenConsumption(const VolumePerTimeUnit& unit) const;
  //@}

  /** @name OxygenDelivery
  *  @brief The rate at which oxygen is delivered to the tissues, calculated as the product of cardiac output and arterial oxygen content.
  *  @{*/
  /// Test if member has been allocated
  virtual bool HasOxygenDelivery() const;
  /// Get member class, allocate if nullptr
  virtual SEScalarVolumePerTime& GetOxygenDelivery();
  virtual double GetOxygenDelivery(const VolumePerTimeUnit& unit) const;
  //@}

  /** @name OxygenDeliveryToOxygenConsumptionRatio
  *  @brief The ratio of oxygen delivery to oxygen consumption, an indicator of the adequacy of tissue oxygenation.
  *  @{*/
  /// Test if member has been allocated
  virtual bool HasOxygenDeliveryToOxygenConsumptionRatio() const;
  /// Get member class, allocate if nullptr
  virtual SEScalar& GetOxygenDeliveryToOxygenConsumptionRatio();
  virtual double GetOxygenDeliveryToOxygenConsumptionRatio() const;
  //@}

  /** @name OxygenSaturation
   *  @brief @copybrief Physiology_BloodChemistrySystemData_OxygenSaturation
   *  @{*/
   /// %Test if member has been allocated
  virtual bool HasOxygenSaturation() const;
  /// Get member class, allocate if nullptr
  virtual SEScalar0To1& GetOxygenSaturation();
  virtual double GetOxygenSaturation() const;
  //@}

  /** @name Phosphate
  *  @brief @copybrief Physiology_BloodChemistrySystemData_Phosphate
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasPhosphate() const;
  /// Get member class, allocate if nullptr
  virtual SEScalarAmountPerVolume& GetPhosphate();
  virtual double GetPhosphate(const AmountPerVolumeUnit& unit) const;
  //@}

  virtual bool HasPlasmaOsmolality() const;
  virtual SEScalarOsmolality& GetPlasmaOsmolality();
  virtual double GetPlasmaOsmolality(const OsmolalityUnit& unit) const;

  virtual bool HasPlasmaOsmolarity() const;
  virtual SEScalarOsmolarity& GetPlasmaOsmolarity();
  virtual double GetPlasmaOsmolarity(const OsmolarityUnit& unit) const;

  /** @name PlasmaVolume
  *  @brief @copybrief Physiology_BloodChemistrySystemData_PlasmaVolume
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasPlasmaVolume() const;
  /// Get member class, allocate if nullptr
  virtual SEScalarVolume& GetPlasmaVolume();
  virtual double GetPlasmaVolume(const VolumeUnit& unit) const;
  //@}

  /** @name PulseOximetry
  *  @brief @copybrief Physiology_BloodChemistrySystemData_PulseOximetry
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasPulseOximetry() const;
  /// Get member class, allocate if nullptr
  virtual SEScalar0To1& GetPulseOximetry();
  virtual double GetPulseOximetry() const;
  //@}

  /** @name RedBloodCellCount
   *  @brief @copybrief Physiology_BloodChemistrySystemData_RedBloodCellCount
   *  @{*/
   /// %Test if member has been allocated
  virtual bool HasRedBloodCellCount() const;
  /// Get member class, allocate if nullptr
  virtual SEScalarAmountPerVolume& GetRedBloodCellCount();
  virtual double GetRedBloodCellCount(const AmountPerVolumeUnit& unit) const;
  //@}

  /** @name StrongIonDifference
  *  @brief @copybrief Physiology_BloodChemistrySystemData_StrongIonDifference
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasStrongIonDifference() const;
  /// Get member class, allocate if nullptr
  virtual SEScalarAmountPerVolume& GetStrongIonDifference();
  virtual double GetStrongIonDifference(const AmountPerVolumeUnit& unit) const;
  //@}

  /** @name AnatomicShuntFraction
   *  @brief @copybrief Physiology_BloodChemistrySystemData_AnatomicShuntFraction
   *  @{*/
   /// %Test if member has been allocated
  virtual bool HasAnatomicShuntFraction() const;
  /// Get member class, allocate if nullptr
  virtual SEScalar0To1& GetAnatomicShuntFraction();
  virtual double GetAnatomicShuntFraction() const;
  //@}

  /** @name TotalProteinConcentration
  *  @brief @copybrief Physiology_BloodChemistrySystemData_TotalProteinConcentration
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasTotalProteinConcentration() const;
  /// Get member class, allocate if nullptr
  virtual SEScalarMassPerVolume& GetTotalProteinConcentration();
  virtual double GetTotalProteinConcentration(const MassPerVolumeUnit& unit) const;
  //@}

  /** @name VolumeFractionNeutralPhospholipidInPlasma
  *  @brief @copybrief Physiology_BloodChemistrySystemData_VolumeFractionNeutralPhospholipidInPlasma
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasVolumeFractionNeutralPhospholipidInPlasma() const;
  /// Get member class, allocate if nullptr
  virtual SEScalar0To1& GetVolumeFractionNeutralPhospholipidInPlasma();
  virtual double GetVolumeFractionNeutralPhospholipidInPlasma() const;
  //@}

  /** @name VolumeFractionNeutralLipidInPlasma
  *  @brief @copybrief Physiology_BloodChemistrySystemData_VolumeFractionNeutralLipidInPlasma
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasVolumeFractionNeutralLipidInPlasma() const;
  /// Get member class, allocate if nullptr
  virtual SEScalar0To1& GetVolumeFractionNeutralLipidInPlasma();
  virtual double GetVolumeFractionNeutralLipidInPlasma() const;
  //@}

  /** @name WhiteBloodCellCount
  *  @brief @copybrief Physiology_BloodChemistrySystemData_WhiteBloodCellCount
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasWhiteBloodCellCount() const;
  /// Get member class, allocate if nullptr
  virtual SEScalarAmountPerVolume& GetWhiteBloodCellCount();
  virtual double GetWhiteBloodCellCount(const AmountPerVolumeUnit& unit) const;
  //@}

  /** @name ArterialCarbonDioxidePressure
  *  @brief @copybrief Physiology_BloodChemistrySystemData_ArterialCarbonDioxidePressure
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasArterialCarbonDioxidePressure() const;
  /// Get member class, allocate if nullptr
  virtual SEScalarPressure& GetArterialCarbonDioxidePressure();
  virtual double GetArterialCarbonDioxidePressure(const PressureUnit& unit) const;
  //@}

  /** @name ArterialOxygenPressure
  *  @brief @copybrief Physiology_BloodChemistrySystemData_ArterialOxygenPressure
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasArterialOxygenPressure() const;
  /// Get member class, allocate if nullptr
  virtual SEScalarPressure& GetArterialOxygenPressure();
  virtual double GetArterialOxygenPressure(const PressureUnit& unit) const;
  //@}

  /** @name PulmonaryArterialCarbonDioxidePressure
  *  @brief @copybrief Physiology_BloodChemistrySystemData_PulmonaryArterialCarbonDioxidePressure
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasPulmonaryArterialCarbonDioxidePressure() const;
  /// Get member class, allocate if nullptr
  virtual SEScalarPressure& GetPulmonaryArterialCarbonDioxidePressure();
  virtual double GetPulmonaryArterialCarbonDioxidePressure(const PressureUnit& unit) const;
  //@}

  /** @name PulmonaryArterialOxygenPressure
  *  @brief @copybrief Physiology_BloodChemistrySystemData_PulmonaryArterialOxygenPressure
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasPulmonaryArterialOxygenPressure() const;
  /// Get member class, allocate if nullptr
  virtual SEScalarPressure& GetPulmonaryArterialOxygenPressure();
  virtual double GetPulmonaryArterialOxygenPressure(const PressureUnit& unit) const;
  //@}

  /** @name PulmonaryVenousCarbonDioxidePressure
  *  @brief @copybrief Physiology_BloodChemistrySystemData_PulmonaryVenousCarbonDioxidePressure
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasPulmonaryVenousCarbonDioxidePressure() const;
  /// Get member class, allocate if nullptr
  virtual SEScalarPressure& GetPulmonaryVenousCarbonDioxidePressure();
  virtual double GetPulmonaryVenousCarbonDioxidePressure(const PressureUnit& unit) const;
  //@}

  /** @name PulmonaryVenousOxygenPressure
  *  @brief @copybrief Physiology_BloodChemistrySystemData_PulmonaryVenousOxygenPressure
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasPulmonaryVenousOxygenPressure() const;
  /// Get member class, allocate if nullptr
  virtual SEScalarPressure& GetPulmonaryVenousOxygenPressure();
  virtual double GetPulmonaryVenousOxygenPressure(const PressureUnit& unit) const;
  //@}

  /** @name VenousCarbonDioxidePressure
  *  @brief @copybrief Physiology_BloodChemistrySystemData_VenousCarbonDioxidePressure
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasVenousCarbonDioxidePressure() const;
  /// Get member class, allocate if nullptr
  virtual SEScalarPressure& GetVenousCarbonDioxidePressure();
  virtual double GetVenousCarbonDioxidePressure(const PressureUnit& unit) const;
  //@}
  /** @name VenousOxygenPressure
  *  @brief @copybrief Physiology_BloodChemistrySystemData_VenousOxygenPressure
  *  @{*/
  /// %Test if member has been allocated
  virtual bool HasVenousOxygenPressure() const;
  /// Get member class, allocate if nullptr
  virtual SEScalarPressure& GetVenousOxygenPressure();
  virtual double GetVenousOxygenPressure(const PressureUnit& unit) const;
  //@}


protected:
  SEScalar*                       m_ArterialOxygenContent;
  SEScalar*                       m_ArteriovenousOxygenDifference;
  SEScalarAmountPerVolume*        m_BaseExcess;
  SEScalarMassPerVolume*          m_BloodDensity;
  SEScalar*                       m_BloodPH;
  SEScalarHeatCapacitancePerMass* m_BloodSpecificHeat;
  SEScalarMassPerVolume*          m_BloodUreaNitrogenConcentration;
  SEScalar0To1*                   m_CarbonDioxideSaturation;
  SEScalar0To1*                   m_CarbonMonoxideSaturation;
  SEScalar0To1*                   m_ClinicalShuntFraction;
  SEScalar0To1*                   m_Hematocrit;
  SEScalarMass*                   m_HemoglobinContent;
  SEScalar*                       m_MixedVenousOxygenContent;
  SEScalarVolumePerTime*          m_ClinicalOxygenConsumption;
  SEScalarVolumePerTime*          m_OxygenDelivery;
  SEScalar*                       m_OxygenDeliveryToOxygenConsumptionRatio;
  SEScalar0To1*                   m_OxygenSaturation;
  SEScalarAmountPerVolume*        m_Phosphate;
  SEScalarOsmolality*             m_PlasmaOsmolality;
  SEScalarOsmolarity*             m_PlasmaOsmolarity;
  SEScalarVolume*                 m_PlasmaVolume;
  SEScalar0To1*                   m_PulseOximetry;
  SEScalarAmountPerVolume*        m_RedBloodCellCount;
  SEScalar0To1*                   m_AnatomicShuntFraction;
  SEScalarAmountPerVolume*        m_StrongIonDifference;
  SEScalarMassPerVolume*          m_TotalProteinConcentration;
  SEScalar0To1*                   m_VolumeFractionNeutralPhospholipidInPlasma;
  SEScalar0To1*                   m_VolumeFractionNeutralLipidInPlasma;
  SEScalarAmountPerVolume*        m_WhiteBloodCellCount;
  
  SEScalarPressure*               m_ArterialCarbonDioxidePressure;
  SEScalarPressure*               m_ArterialOxygenPressure;
  SEScalarPressure*               m_PulmonaryArterialCarbonDioxidePressure;
  SEScalarPressure*               m_PulmonaryArterialOxygenPressure;
  SEScalarPressure*               m_PulmonaryVenousCarbonDioxidePressure;
  SEScalarPressure*               m_PulmonaryVenousOxygenPressure;
  SEScalarPressure*               m_VenousCarbonDioxidePressure;
  SEScalarPressure*               m_VenousOxygenPressure;
};