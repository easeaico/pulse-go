/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/system/physiology/SERespiratorySystem.h"
#include "cdm/system/physiology/SERespiratoryMechanics.h"
#include "cdm/properties/SEScalarArea.h"
#include "cdm/properties/SEScalarEnergy.h"
#include "cdm/properties/SEScalarVolumePerPressure.h"
#include "cdm/properties/SEScalarPressureTimePerVolume.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarPower.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarPressurePerVolume.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerPressure.h"
#include "cdm/properties/SEScalarVolumePerTime.h"

const std::string& eLungCompartment_Name(eLungCompartment cmpt)
{
  switch (cmpt)
  {
  case eLungCompartment::LeftLung:
    return "LeftLung";
  case eLungCompartment::RightLung:
    return "RightLung";
  case eLungCompartment::LeftSuperiorLobe:
    return "LeftSuperiorLobe";
  case eLungCompartment::LeftInferiorLobe:
    return "LeftInferiorLobe";
  case eLungCompartment::RightSuperiorLobe:
    return "RightSuperiorLobe";
  case eLungCompartment::RightMiddleLobe:
    return "RightMiddleLobe";
  case eLungCompartment::RightInferiorLobe:
    return "RightInferiorLobe";
  }
  return "Unknown Lung Compartment";
}

SERespiratorySystem::SERespiratorySystem(Logger* logger) : SESystem(logger)
{
  m_AirwayPressure = nullptr;
  m_AlveolarArterialGradient = nullptr;
  m_AlveolarDeadSpace = nullptr;
  m_AnatomicDeadSpace = nullptr;
  m_ClinicalPhysiologicDeadSpaceTidalVolumeRatio = nullptr;
  m_HorowitzIndex = nullptr;
  m_ChestWallCompliance = nullptr;
  m_ElasticWorkOfBreathing = nullptr;
  m_EndTidalCarbonDioxideFraction = nullptr;
  m_EndTidalCarbonDioxidePressure = nullptr;
  m_EndTidalOxygenFraction = nullptr;
  m_EndTidalOxygenPressure = nullptr;
  m_ExpiratoryFlow = nullptr;
  m_ExpiratoryRespiratoryResistance = nullptr;
  m_ExpiratoryTidalVolume = nullptr;
  m_ExtrinsicPositiveEndExpiratoryPressure = nullptr;
  m_FractionOfInspiredOxygen = nullptr;
  m_ImposedPowerOfBreathing = nullptr;
  m_ImposedWorkOfBreathing = nullptr;
  m_InspiratoryExpiratoryRatio = nullptr;
  m_InspiratoryFlow = nullptr;
  m_InspiratoryRespiratoryResistance = nullptr;
  m_InspiratoryTidalVolume = nullptr;
  m_IntrapleuralPressure = nullptr;
  m_IntrapulmonaryPressure = nullptr;
  m_IntrinsicPositiveEndExpiratoryPressure = nullptr;
  m_LungCompliance = nullptr;
  m_MaximalInspiratoryPressure = nullptr;
  m_MeanAirwayPressure = nullptr;
  m_OxygenationIndex = nullptr;
  m_OxygenSaturationIndex = nullptr;
  m_PatientPowerOfBreathing = nullptr;
  m_PatientWorkOfBreathing = nullptr;
  m_PeakInspiratoryPressure = nullptr;
  m_PhysiologicDeadSpace = nullptr;
  m_PhysiologicDeadSpaceTidalVolumeRatio = nullptr;
  m_RespiratoryCompliance = nullptr;
  m_RespiratoryElastance = nullptr;
  m_RelativeTotalLungVolume = nullptr;
  m_ResistiveExpiratoryWorkOfBreathing = nullptr;
  m_ResistiveInspiratoryWorkOfBreathing = nullptr;
  m_RespirationRate = nullptr;
  m_RespiratoryMuscleFatigue = nullptr;
  m_RespiratoryMusclePressure = nullptr;
  m_SaturationAndFractionOfInspiredOxygenRatio = nullptr;
  m_SpecificVentilation = nullptr;
  m_TidalVolume = nullptr;
  m_TotalAlveolarVentilation = nullptr;
  m_TotalDeadSpaceVentilation = nullptr;
  m_TotalLungVolume = nullptr;
  m_TotalPositiveEndExpiratoryPressure = nullptr;
  m_TotalPowerOfBreathing = nullptr;
  m_TotalPulmonaryVentilation = nullptr;
  m_TotalWorkOfBreathing = nullptr;
  m_TransairwayPressure = nullptr;
  m_TransalveolarPressure = nullptr;
  m_TransChestWallPressure = nullptr;
  m_TransMusclePressure = nullptr;
  m_TranspulmonaryPressure = nullptr;
  m_TransrespiratoryPressure = nullptr;
  m_TransthoracicPressure = nullptr;
  m_VentilationPerfusionRatio = nullptr;

  m_Mechanics = nullptr;
}

SERespiratorySystem::~SERespiratorySystem()
{
  SAFE_DELETE(m_AirwayPressure);
  SAFE_DELETE(m_AlveolarArterialGradient);
  SAFE_DELETE(m_AlveolarDeadSpace);
  SAFE_DELETE(m_AnatomicDeadSpace);
  SAFE_DELETE(m_ClinicalPhysiologicDeadSpaceTidalVolumeRatio);
  SAFE_DELETE(m_HorowitzIndex);
  SAFE_DELETE(m_ChestWallCompliance);
  SAFE_DELETE(m_ElasticWorkOfBreathing);
  SAFE_DELETE(m_EndTidalCarbonDioxideFraction);
  SAFE_DELETE(m_EndTidalCarbonDioxidePressure);
  SAFE_DELETE(m_EndTidalOxygenFraction);
  SAFE_DELETE(m_EndTidalOxygenPressure);
  SAFE_DELETE(m_ExpiratoryFlow);
  SAFE_DELETE(m_ExpiratoryTidalVolume);
  SAFE_DELETE(m_ExpiratoryRespiratoryResistance);
  SAFE_DELETE(m_ExtrinsicPositiveEndExpiratoryPressure);
  SAFE_DELETE(m_FractionOfInspiredOxygen);
  SAFE_DELETE(m_ImposedPowerOfBreathing);
  SAFE_DELETE(m_ImposedWorkOfBreathing);
  SAFE_DELETE(m_InspiratoryExpiratoryRatio);
  SAFE_DELETE(m_InspiratoryFlow);
  SAFE_DELETE(m_InspiratoryRespiratoryResistance);
  SAFE_DELETE(m_InspiratoryTidalVolume);
  SAFE_DELETE(m_IntrapleuralPressure);
  SAFE_DELETE(m_IntrapulmonaryPressure);
  SAFE_DELETE(m_IntrinsicPositiveEndExpiratoryPressure);
  SAFE_DELETE(m_LungCompliance);
  SAFE_DELETE(m_MaximalInspiratoryPressure);
  SAFE_DELETE(m_MeanAirwayPressure);
  SAFE_DELETE(m_OxygenationIndex);
  SAFE_DELETE(m_OxygenSaturationIndex);
  SAFE_DELETE(m_PatientPowerOfBreathing);
  SAFE_DELETE(m_PatientWorkOfBreathing);
  SAFE_DELETE(m_PeakInspiratoryPressure);
  SAFE_DELETE(m_PhysiologicDeadSpace);
  SAFE_DELETE(m_PhysiologicDeadSpaceTidalVolumeRatio);
  SAFE_DELETE(m_RespiratoryCompliance);
  SAFE_DELETE(m_RespiratoryElastance);
  SAFE_DELETE(m_RelativeTotalLungVolume);
  SAFE_DELETE(m_ResistiveExpiratoryWorkOfBreathing);
  SAFE_DELETE(m_ResistiveInspiratoryWorkOfBreathing);
  SAFE_DELETE(m_RespirationRate);
  SAFE_DELETE(m_RespiratoryMuscleFatigue);
  SAFE_DELETE(m_RespiratoryMusclePressure);
  SAFE_DELETE(m_SaturationAndFractionOfInspiredOxygenRatio);
  SAFE_DELETE(m_SpecificVentilation);
  SAFE_DELETE(m_TidalVolume);
  SAFE_DELETE(m_TotalAlveolarVentilation);
  SAFE_DELETE(m_TotalDeadSpaceVentilation);
  SAFE_DELETE(m_TotalLungVolume);
  SAFE_DELETE(m_TotalPositiveEndExpiratoryPressure);
  SAFE_DELETE(m_TotalPowerOfBreathing);
  SAFE_DELETE(m_TotalPulmonaryVentilation);
  SAFE_DELETE(m_TotalWorkOfBreathing);
  SAFE_DELETE(m_TransairwayPressure);
  SAFE_DELETE(m_TransalveolarPressure);
  SAFE_DELETE(m_TransChestWallPressure);
  SAFE_DELETE(m_TransMusclePressure);
  SAFE_DELETE(m_TranspulmonaryPressure);
  SAFE_DELETE(m_TransrespiratoryPressure);
  SAFE_DELETE(m_TransthoracicPressure);
  SAFE_DELETE(m_VentilationPerfusionRatio);

  SAFE_DELETE(m_Mechanics);
}

void SERespiratorySystem::Clear()
{
  SESystem::Clear();

  INVALIDATE_PROPERTY(m_AirwayPressure);
  INVALIDATE_PROPERTY(m_AlveolarArterialGradient);
  INVALIDATE_PROPERTY(m_AlveolarDeadSpace);
  INVALIDATE_PROPERTY(m_AnatomicDeadSpace);
  INVALIDATE_PROPERTY(m_ClinicalPhysiologicDeadSpaceTidalVolumeRatio);
  INVALIDATE_PROPERTY(m_HorowitzIndex);
  INVALIDATE_PROPERTY(m_ChestWallCompliance);
  INVALIDATE_PROPERTY(m_ElasticWorkOfBreathing);
  INVALIDATE_PROPERTY(m_EndTidalCarbonDioxideFraction);
  INVALIDATE_PROPERTY(m_EndTidalCarbonDioxidePressure);
  INVALIDATE_PROPERTY(m_EndTidalOxygenFraction);
  INVALIDATE_PROPERTY(m_EndTidalOxygenPressure);
  INVALIDATE_PROPERTY(m_ExpiratoryFlow);
  INVALIDATE_PROPERTY(m_ExpiratoryRespiratoryResistance);
  INVALIDATE_PROPERTY(m_ExpiratoryTidalVolume);
  INVALIDATE_PROPERTY(m_ExtrinsicPositiveEndExpiratoryPressure);
  INVALIDATE_PROPERTY(m_FractionOfInspiredOxygen);
  INVALIDATE_PROPERTY(m_ImposedPowerOfBreathing);
  INVALIDATE_PROPERTY(m_ImposedWorkOfBreathing);
  INVALIDATE_PROPERTY(m_InspiratoryExpiratoryRatio);
  INVALIDATE_PROPERTY(m_InspiratoryFlow);
  INVALIDATE_PROPERTY(m_InspiratoryRespiratoryResistance);
  INVALIDATE_PROPERTY(m_InspiratoryTidalVolume);
  INVALIDATE_PROPERTY(m_IntrapleuralPressure);
  INVALIDATE_PROPERTY(m_IntrapulmonaryPressure);
  INVALIDATE_PROPERTY(m_IntrinsicPositiveEndExpiratoryPressure);
  INVALIDATE_PROPERTY(m_LungCompliance);
  INVALIDATE_PROPERTY(m_MaximalInspiratoryPressure);
  INVALIDATE_PROPERTY(m_MeanAirwayPressure);
  INVALIDATE_PROPERTY(m_OxygenationIndex);
  INVALIDATE_PROPERTY(m_OxygenSaturationIndex);
  INVALIDATE_PROPERTY(m_PatientPowerOfBreathing);
  INVALIDATE_PROPERTY(m_PatientWorkOfBreathing);
  INVALIDATE_PROPERTY(m_PeakInspiratoryPressure);
  INVALIDATE_PROPERTY(m_PhysiologicDeadSpace);
  INVALIDATE_PROPERTY(m_PhysiologicDeadSpaceTidalVolumeRatio);
  INVALIDATE_PROPERTY(m_RespiratoryCompliance);
  INVALIDATE_PROPERTY(m_RespiratoryElastance);
  INVALIDATE_PROPERTY(m_RelativeTotalLungVolume);
  INVALIDATE_PROPERTY(m_ResistiveExpiratoryWorkOfBreathing);
  INVALIDATE_PROPERTY(m_ResistiveInspiratoryWorkOfBreathing);
  INVALIDATE_PROPERTY(m_RespirationRate);
  INVALIDATE_PROPERTY(m_RespiratoryMuscleFatigue);
  INVALIDATE_PROPERTY(m_RespiratoryMusclePressure);
  INVALIDATE_PROPERTY(m_SaturationAndFractionOfInspiredOxygenRatio);
  INVALIDATE_PROPERTY(m_SpecificVentilation);
  INVALIDATE_PROPERTY(m_TidalVolume);
  INVALIDATE_PROPERTY(m_TotalAlveolarVentilation);
  INVALIDATE_PROPERTY(m_TotalDeadSpaceVentilation);
  INVALIDATE_PROPERTY(m_TotalLungVolume);
  INVALIDATE_PROPERTY(m_TotalPositiveEndExpiratoryPressure);
  INVALIDATE_PROPERTY(m_TotalPowerOfBreathing);
  INVALIDATE_PROPERTY(m_TotalPulmonaryVentilation);
  INVALIDATE_PROPERTY(m_TotalWorkOfBreathing);
  INVALIDATE_PROPERTY(m_TransairwayPressure);
  INVALIDATE_PROPERTY(m_TransalveolarPressure);
  INVALIDATE_PROPERTY(m_TransChestWallPressure);
  INVALIDATE_PROPERTY(m_TransMusclePressure);
  INVALIDATE_PROPERTY(m_TranspulmonaryPressure);
  INVALIDATE_PROPERTY(m_TransrespiratoryPressure);
  INVALIDATE_PROPERTY(m_TransthoracicPressure);
  INVALIDATE_PROPERTY(m_VentilationPerfusionRatio);

  if (m_Mechanics != nullptr)
    m_Mechanics->Clear();
}

const SEScalar* SERespiratorySystem::GetScalar(const std::string& name)
{
  if (name.compare("AirwayPressure") == 0)
    return &GetAirwayPressure();
  if (name.compare("AlveolarArterialGradient") == 0)
    return &GetAlveolarArterialGradient();
  if (name.compare("AlveolarDeadSpace") == 0)
    return &GetAlveolarDeadSpace();
  if (name.compare("AnatomicDeadSpace") == 0)
    return &GetAnatomicDeadSpace();
  if (name.compare("ClinicalPhysiologicDeadSpaceTidalVolumeRatio") == 0)
    return &GetClinicalPhysiologicDeadSpaceTidalVolumeRatio();
  if (name.compare("HorowitzIndex") == 0)
    return &GetHorowitzIndex();
  if (name.compare("ChestWallCompliance") == 0)
    return &GetChestWallCompliance();
  if (name.compare("ElasticWorkOfBreathing") == 0)
    return &GetElasticWorkOfBreathing();
  if (name.compare("EndTidalCarbonDioxideFraction") == 0)
    return &GetEndTidalCarbonDioxideFraction();
  if (name.compare("EndTidalCarbonDioxidePressure") == 0)
    return &GetEndTidalCarbonDioxidePressure();
  if (name.compare("EndTidalOxygenFraction") == 0)
    return &GetEndTidalOxygenFraction();
  if (name.compare("EndTidalOxygenPressure") == 0)
    return &GetEndTidalOxygenPressure();
  if (name.compare("ExpiratoryFlow") == 0)
    return &GetExpiratoryFlow();
  if (name.compare("ExpiratoryRespiratoryResistance") == 0)
    return &GetExpiratoryRespiratoryResistance();
  if (name.compare("ExpiratoryTidalVolume") == 0)
    return &GetExpiratoryTidalVolume();
  if (name.compare("ExtrinsicPositiveEndExpiratoryPressure") == 0)
    return &GetExtrinsicPositiveEndExpiratoryPressure();
  if (name.compare("FractionOfInspiredOxygen") == 0)
    return &GetFractionOfInspiredOxygen();
  if (name.compare("ImposedPowerOfBreathing") == 0)
    return &GetImposedPowerOfBreathing();
  if (name.compare("ImposedWorkOfBreathing") == 0)
    return &GetImposedWorkOfBreathing();
  if (name.compare("InspiratoryExpiratoryRatio") == 0)
    return &GetInspiratoryExpiratoryRatio();
  if (name.compare("InspiratoryFlow") == 0)
    return &GetInspiratoryFlow();
  if (name.compare("InspiratoryRespiratoryResistance") == 0)
    return &GetInspiratoryRespiratoryResistance();
  if (name.compare("InspiratoryTidalVolume") == 0)
    return &GetInspiratoryTidalVolume();
  if (name.compare("IntrapleuralPressure") == 0)
    return &GetIntrapleuralPressure();
  if (name.compare("IntrapulmonaryPressure") == 0)
    return &GetIntrapulmonaryPressure();
  if (name.compare("IntrinsicPositiveEndExpiratoryPressure") == 0)
    return &GetIntrinsicPositiveEndExpiratoryPressure();
  if (name.compare("LungCompliance") == 0)
    return &GetLungCompliance();
  if (name.compare("MaximalInspiratoryPressure") == 0)
    return &GetMaximalInspiratoryPressure();
  if (name.compare("MeanAirwayPressure") == 0)
    return &GetMeanAirwayPressure();
  if (name.compare("OxygenationIndex") == 0)
    return &GetOxygenationIndex();
  if (name.compare("OxygenSaturationIndex") == 0)
    return &GetOxygenSaturationIndex();
  if (name.compare("PatientPowerOfBreathing") == 0)
    return &GetPatientPowerOfBreathing();
  if (name.compare("PatientWorkOfBreathing") == 0)
    return &GetPatientWorkOfBreathing();
  if (name.compare("PeakInspiratoryPressure") == 0)
    return &GetPeakInspiratoryPressure();
  if (name.compare("PhysiologicDeadSpace") == 0)
    return &GetPhysiologicDeadSpace();
  if (name.compare("PhysiologicDeadSpaceTidalVolumeRatio") == 0)
    return &GetPhysiologicDeadSpaceTidalVolumeRatio();
  if (name.compare("RespiratoryCompliance") == 0)
    return &GetRespiratoryCompliance();
  if (name.compare("RespiratoryElastance") == 0)
    return &GetRespiratoryElastance();
  if (name.compare("RelativeTotalLungVolume") == 0)
    return &GetRelativeTotalLungVolume();
  if (name.compare("ResistiveExpiratoryWorkOfBreathing") == 0)
    return &GetResistiveExpiratoryWorkOfBreathing();
  if (name.compare("ResistiveInspiratoryWorkOfBreathing") == 0)
    return &GetResistiveInspiratoryWorkOfBreathing();
  if (name.compare("RespirationRate") == 0)
    return &GetRespirationRate();
  if (name.compare("RespiratoryMuscleFatigue") == 0)
    return &GetRespiratoryMuscleFatigue();
  if (name.compare("RespiratoryMusclePressure") == 0)
    return &GetRespiratoryMusclePressure();
  if (name.compare("SaturationAndFractionOfInspiredOxygenRatio") == 0)
    return &GetSaturationAndFractionOfInspiredOxygenRatio();
  if (name.compare("SpecificVentilation") == 0)
    return &GetSpecificVentilation();
  if (name.compare("TidalVolume") == 0)
    return &GetTidalVolume();
  if (name.compare("TotalAlveolarVentilation") == 0)
    return &GetTotalAlveolarVentilation();
  if (name.compare("TotalDeadSpaceVentilation") == 0)
    return &GetTotalDeadSpaceVentilation();
  if (name.compare("TotalLungVolume") == 0)
    return &GetTotalLungVolume();
  if (name.compare("TotalPositiveEndExpiratoryPressure") == 0)
    return &GetTotalPositiveEndExpiratoryPressure();
  if (name.compare("TotalPowerOfBreathing") == 0)
    return &GetTotalPowerOfBreathing();
  if (name.compare("TotalPulmonaryVentilation") == 0)
    return &GetTotalPulmonaryVentilation();
  if (name.compare("TotalWorkOfBreathing") == 0)
    return &GetTotalWorkOfBreathing();
  if (name.compare("TransairwayPressure") == 0)
    return &GetTransairwayPressure();
  if (name.compare("TransalveolarPressure") == 0)
    return &GetTransalveolarPressure();
  if (name.compare("TransChestWallPressure") == 0)
    return &GetTransChestWallPressure();
  if (name.compare("TransMusclePressure") == 0)
    return &GetTransMusclePressure();
  if (name.compare("TranspulmonaryPressure") == 0)
    return &GetTranspulmonaryPressure();
  if (name.compare("TransrespiratoryPressure") == 0)
    return &GetTransrespiratoryPressure();
  if (name.compare("TransthoracicPressure") == 0)
    return &GetTransthoracicPressure();
  if (name.compare("VentilationPerfusionRatio") == 0)
    return &GetVentilationPerfusionRatio();

  if (m_Mechanics != nullptr)
    return m_Mechanics->GetScalar(name);

  return nullptr;
}

bool SERespiratorySystem::HasAirwayPressure() const
{
  return m_AirwayPressure == nullptr ? false : m_AirwayPressure->IsValid();
}
SEScalarPressure& SERespiratorySystem::GetAirwayPressure()
{
  if (m_AirwayPressure == nullptr)
    m_AirwayPressure = new SEScalarPressure();
  return *m_AirwayPressure;
}
double SERespiratorySystem::GetAirwayPressure(const PressureUnit& unit) const
{
  if (m_AirwayPressure == nullptr)
    return SEScalar::dNaN();
  return m_AirwayPressure->GetValue(unit);
}

bool SERespiratorySystem::HasAlveolarArterialGradient() const
{
  return m_AlveolarArterialGradient == nullptr ? false : m_AlveolarArterialGradient->IsValid();
}
SEScalarPressure& SERespiratorySystem::GetAlveolarArterialGradient()
{
  if (m_AlveolarArterialGradient == nullptr)
    m_AlveolarArterialGradient = new SEScalarPressure();
  return *m_AlveolarArterialGradient;
}
double SERespiratorySystem::GetAlveolarArterialGradient(const PressureUnit& unit) const
{
  if (m_AlveolarArterialGradient == nullptr)
    return SEScalar::dNaN();
  return m_AlveolarArterialGradient->GetValue(unit);
}

bool SERespiratorySystem::HasAlveolarDeadSpace() const
{
  return m_AlveolarDeadSpace == nullptr ? false : m_AlveolarDeadSpace->IsValid();
}
SEScalarVolume& SERespiratorySystem::GetAlveolarDeadSpace()
{
  if (m_AlveolarDeadSpace == nullptr)
    m_AlveolarDeadSpace = new SEScalarVolume();
  return *m_AlveolarDeadSpace;
}
double SERespiratorySystem::GetAlveolarDeadSpace(const VolumeUnit& unit) const
{
  if (m_AlveolarDeadSpace == nullptr)
    return SEScalar::dNaN();
  return m_AlveolarDeadSpace->GetValue(unit);
}

bool SERespiratorySystem::HasAnatomicDeadSpace() const
{
  return m_AnatomicDeadSpace == nullptr ? false : m_AnatomicDeadSpace->IsValid();
}
SEScalarVolume& SERespiratorySystem::GetAnatomicDeadSpace()
{
  if (m_AnatomicDeadSpace == nullptr)
    m_AnatomicDeadSpace = new SEScalarVolume();
  return *m_AnatomicDeadSpace;
}
double SERespiratorySystem::GetAnatomicDeadSpace(const VolumeUnit& unit) const
{
  if (m_AnatomicDeadSpace == nullptr)
    return SEScalar::dNaN();
  return m_AnatomicDeadSpace->GetValue(unit);
}

bool SERespiratorySystem::HasClinicalPhysiologicDeadSpaceTidalVolumeRatio() const
{
  return m_ClinicalPhysiologicDeadSpaceTidalVolumeRatio == nullptr ? false : m_ClinicalPhysiologicDeadSpaceTidalVolumeRatio->IsValid();
}
SEScalar& SERespiratorySystem::GetClinicalPhysiologicDeadSpaceTidalVolumeRatio()
{
  if (m_ClinicalPhysiologicDeadSpaceTidalVolumeRatio == nullptr)
    m_ClinicalPhysiologicDeadSpaceTidalVolumeRatio = new SEScalar();
  return *m_ClinicalPhysiologicDeadSpaceTidalVolumeRatio;
}
double SERespiratorySystem::GetClinicalPhysiologicDeadSpaceTidalVolumeRatio() const
{
  if (m_ClinicalPhysiologicDeadSpaceTidalVolumeRatio == nullptr)
    return SEScalar::dNaN();
  return m_ClinicalPhysiologicDeadSpaceTidalVolumeRatio->GetValue();
}

bool SERespiratorySystem::HasHorowitzIndex() const
{
  return m_HorowitzIndex == nullptr ? false : m_HorowitzIndex->IsValid();
}
SEScalarPressure& SERespiratorySystem::GetHorowitzIndex()
{
  if (m_HorowitzIndex == nullptr)
    m_HorowitzIndex = new SEScalarPressure();
  return *m_HorowitzIndex;
}
double SERespiratorySystem::GetHorowitzIndex(const PressureUnit& unit) const
{
  if (m_HorowitzIndex == nullptr)
    return SEScalar::dNaN();
  return m_HorowitzIndex->GetValue(unit);
}

bool SERespiratorySystem::HasChestWallCompliance() const
{
  return m_ChestWallCompliance == nullptr ? false : m_ChestWallCompliance->IsValid();
}
SEScalarVolumePerPressure& SERespiratorySystem::GetChestWallCompliance()
{
  if (m_ChestWallCompliance == nullptr)
    m_ChestWallCompliance = new SEScalarVolumePerPressure();
  return *m_ChestWallCompliance;
}
double SERespiratorySystem::GetChestWallCompliance(const VolumePerPressureUnit& unit) const
{
  if (m_ChestWallCompliance == nullptr)
    return SEScalar::dNaN();
  return m_ChestWallCompliance->GetValue(unit);
}

bool SERespiratorySystem::HasElasticWorkOfBreathing() const
{
  return m_ElasticWorkOfBreathing == nullptr ? false : m_ElasticWorkOfBreathing->IsValid();
}
SEScalarEnergy& SERespiratorySystem::GetElasticWorkOfBreathing()
{
  if (m_ElasticWorkOfBreathing == nullptr)
    m_ElasticWorkOfBreathing = new SEScalarEnergy();
  return *m_ElasticWorkOfBreathing;
}
double SERespiratorySystem::GetElasticWorkOfBreathing(const EnergyUnit& unit) const
{
  if (m_ElasticWorkOfBreathing == nullptr)
    return SEScalar::dNaN();
  return m_ElasticWorkOfBreathing->GetValue(unit);
}

bool SERespiratorySystem::HasEndTidalCarbonDioxideFraction() const
{
  return m_EndTidalCarbonDioxideFraction == nullptr ? false : m_EndTidalCarbonDioxideFraction->IsValid();
}
SEScalar0To1& SERespiratorySystem::GetEndTidalCarbonDioxideFraction()
{
  if (m_EndTidalCarbonDioxideFraction == nullptr)
    m_EndTidalCarbonDioxideFraction = new SEScalar0To1();
  return *m_EndTidalCarbonDioxideFraction;
}
double SERespiratorySystem::GetEndTidalCarbonDioxideFraction() const
{
  if (m_EndTidalCarbonDioxideFraction == nullptr)
    return SEScalar::dNaN();
  return m_EndTidalCarbonDioxideFraction->GetValue();
}

bool SERespiratorySystem::HasEndTidalCarbonDioxidePressure() const
{
  return m_EndTidalCarbonDioxidePressure==nullptr?false:m_EndTidalCarbonDioxidePressure->IsValid();
}
SEScalarPressure& SERespiratorySystem::GetEndTidalCarbonDioxidePressure()
{
  if(m_EndTidalCarbonDioxidePressure==nullptr)
    m_EndTidalCarbonDioxidePressure=new SEScalarPressure();
  return *m_EndTidalCarbonDioxidePressure;
}
double SERespiratorySystem::GetEndTidalCarbonDioxidePressure(const PressureUnit& unit) const
{
  if (m_EndTidalCarbonDioxidePressure == nullptr)
    return SEScalar::dNaN();
  return m_EndTidalCarbonDioxidePressure->GetValue(unit);
}

bool SERespiratorySystem::HasEndTidalOxygenFraction() const
{
  return m_EndTidalOxygenFraction == nullptr ? false : m_EndTidalOxygenFraction->IsValid();
}
SEScalar0To1& SERespiratorySystem::GetEndTidalOxygenFraction()
{
  if (m_EndTidalOxygenFraction == nullptr)
    m_EndTidalOxygenFraction = new SEScalar0To1();
  return *m_EndTidalOxygenFraction;
}
double SERespiratorySystem::GetEndTidalOxygenFraction() const
{
  if (m_EndTidalOxygenFraction == nullptr)
    return SEScalar::dNaN();
  return m_EndTidalOxygenFraction->GetValue();
}

bool SERespiratorySystem::HasEndTidalOxygenPressure() const
{
  return m_EndTidalOxygenPressure == nullptr ? false : m_EndTidalOxygenPressure->IsValid();
}
SEScalarPressure& SERespiratorySystem::GetEndTidalOxygenPressure()
{
  if (m_EndTidalOxygenPressure == nullptr)
    m_EndTidalOxygenPressure = new SEScalarPressure();
  return *m_EndTidalOxygenPressure;
}
double SERespiratorySystem::GetEndTidalOxygenPressure(const PressureUnit& unit) const
{
  if (m_EndTidalOxygenPressure == nullptr)
    return SEScalar::dNaN();
  return m_EndTidalOxygenPressure->GetValue(unit);
}

bool SERespiratorySystem::HasExpiratoryFlow() const
{
  return m_ExpiratoryFlow == nullptr ? false : m_ExpiratoryFlow->IsValid();
}
SEScalarVolumePerTime& SERespiratorySystem::GetExpiratoryFlow()
{
  if (m_ExpiratoryFlow == nullptr)
    m_ExpiratoryFlow = new SEScalarVolumePerTime();
  return *m_ExpiratoryFlow;
}
double SERespiratorySystem::GetExpiratoryFlow(const VolumePerTimeUnit& unit) const
{
  if (m_ExpiratoryFlow == nullptr)
    return SEScalar::dNaN();
  return m_ExpiratoryFlow->GetValue(unit);
}

bool SERespiratorySystem::HasExpiratoryRespiratoryResistance() const
{
  return m_ExpiratoryRespiratoryResistance == nullptr ? false : m_ExpiratoryRespiratoryResistance->IsValid();
}
SEScalarPressureTimePerVolume& SERespiratorySystem::GetExpiratoryRespiratoryResistance()
{
  if (m_ExpiratoryRespiratoryResistance == nullptr)
    m_ExpiratoryRespiratoryResistance = new SEScalarPressureTimePerVolume();
  return *m_ExpiratoryRespiratoryResistance;
}
double SERespiratorySystem::GetExpiratoryRespiratoryResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_ExpiratoryRespiratoryResistance == nullptr)
    return SEScalar::dNaN();
  return m_ExpiratoryRespiratoryResistance->GetValue(unit);
}

bool SERespiratorySystem::HasExpiratoryTidalVolume() const
{
  return m_ExpiratoryTidalVolume == nullptr ? false : m_ExpiratoryTidalVolume->IsValid();
}
SEScalarVolume& SERespiratorySystem::GetExpiratoryTidalVolume()
{
  if (m_ExpiratoryTidalVolume == nullptr)
    m_ExpiratoryTidalVolume = new SEScalarVolume();
  return *m_ExpiratoryTidalVolume;
}
double SERespiratorySystem::GetExpiratoryTidalVolume(const VolumeUnit& unit) const
{
  if (m_ExpiratoryTidalVolume == nullptr)
    return SEScalar::dNaN();
  return m_ExpiratoryTidalVolume->GetValue(unit);
}

bool SERespiratorySystem::HasExtrinsicPositiveEndExpiratoryPressure() const
{
  return m_ExtrinsicPositiveEndExpiratoryPressure == nullptr ? false : m_ExtrinsicPositiveEndExpiratoryPressure->IsValid();
}
SEScalarPressure& SERespiratorySystem::GetExtrinsicPositiveEndExpiratoryPressure()
{
  if (m_ExtrinsicPositiveEndExpiratoryPressure == nullptr)
    m_ExtrinsicPositiveEndExpiratoryPressure = new SEScalarPressure();
  return *m_ExtrinsicPositiveEndExpiratoryPressure;
}
double SERespiratorySystem::GetExtrinsicPositiveEndExpiratoryPressure(const PressureUnit& unit) const
{
  if (m_ExtrinsicPositiveEndExpiratoryPressure == nullptr)
    return SEScalar::dNaN();
  return m_ExtrinsicPositiveEndExpiratoryPressure->GetValue(unit);
}

bool SERespiratorySystem::HasFractionOfInspiredOxygen() const
{
  return m_FractionOfInspiredOxygen == nullptr ? false : m_FractionOfInspiredOxygen->IsValid();
}
SEScalar0To1& SERespiratorySystem::GetFractionOfInspiredOxygen()
{
  if (m_FractionOfInspiredOxygen == nullptr)
    m_FractionOfInspiredOxygen = new SEScalar0To1();
  return *m_FractionOfInspiredOxygen;
}
double SERespiratorySystem::GetFractionOfInspiredOxygen() const
{
  if (m_FractionOfInspiredOxygen == nullptr)
    return SEScalar::dNaN();
  return m_FractionOfInspiredOxygen->GetValue();
}

bool SERespiratorySystem::HasImposedPowerOfBreathing() const
{
  return m_ImposedPowerOfBreathing == nullptr ? false : m_ImposedPowerOfBreathing->IsValid();
}
SEScalarPower& SERespiratorySystem::GetImposedPowerOfBreathing()
{
  if (m_ImposedPowerOfBreathing == nullptr)
    m_ImposedPowerOfBreathing = new SEScalarPower();
  return *m_ImposedPowerOfBreathing;
}
double SERespiratorySystem::GetImposedPowerOfBreathing(const PowerUnit& unit) const
{
  if (m_ImposedPowerOfBreathing == nullptr)
    return SEScalar::dNaN();
  return m_ImposedPowerOfBreathing->GetValue(unit);
}

bool SERespiratorySystem::HasImposedWorkOfBreathing() const
{
  return m_ImposedWorkOfBreathing == nullptr ? false : m_ImposedWorkOfBreathing->IsValid();
}
SEScalarEnergy& SERespiratorySystem::GetImposedWorkOfBreathing()
{
  if (m_ImposedWorkOfBreathing == nullptr)
    m_ImposedWorkOfBreathing = new SEScalarEnergy();
  return *m_ImposedWorkOfBreathing;
}
double SERespiratorySystem::GetImposedWorkOfBreathing(const EnergyUnit& unit) const
{
  if (m_ImposedWorkOfBreathing == nullptr)
    return SEScalar::dNaN();
  return m_ImposedWorkOfBreathing->GetValue(unit);
}

bool SERespiratorySystem::HasInspiratoryExpiratoryRatio() const
{
  return m_InspiratoryExpiratoryRatio == nullptr ? false : m_InspiratoryExpiratoryRatio->IsValid();
}
SEScalar& SERespiratorySystem::GetInspiratoryExpiratoryRatio()
{
  if (m_InspiratoryExpiratoryRatio == nullptr)
    m_InspiratoryExpiratoryRatio = new SEScalar();
  return *m_InspiratoryExpiratoryRatio;
}
double SERespiratorySystem::GetInspiratoryExpiratoryRatio() const
{
  if (m_InspiratoryExpiratoryRatio == nullptr)
    return SEScalar::dNaN();
  return m_InspiratoryExpiratoryRatio->GetValue();
}

bool SERespiratorySystem::HasInspiratoryFlow() const
{
  return m_InspiratoryFlow == nullptr ? false : m_InspiratoryFlow->IsValid();
}
SEScalarVolumePerTime& SERespiratorySystem::GetInspiratoryFlow()
{
  if (m_InspiratoryFlow == nullptr)
    m_InspiratoryFlow = new SEScalarVolumePerTime();
  return *m_InspiratoryFlow;
}
double SERespiratorySystem::GetInspiratoryFlow(const VolumePerTimeUnit& unit) const
{
  if (m_InspiratoryFlow == nullptr)
    return SEScalar::dNaN();
  return m_InspiratoryFlow->GetValue(unit);
}

bool SERespiratorySystem::HasInspiratoryRespiratoryResistance() const
{
  return m_InspiratoryRespiratoryResistance == nullptr ? false : m_InspiratoryRespiratoryResistance->IsValid();
}
SEScalarPressureTimePerVolume& SERespiratorySystem::GetInspiratoryRespiratoryResistance()
{
  if (m_InspiratoryRespiratoryResistance == nullptr)
    m_InspiratoryRespiratoryResistance = new SEScalarPressureTimePerVolume();
  return *m_InspiratoryRespiratoryResistance;
}
double SERespiratorySystem::GetInspiratoryRespiratoryResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_InspiratoryRespiratoryResistance == nullptr)
    return SEScalar::dNaN();
  return m_InspiratoryRespiratoryResistance->GetValue(unit);
}

bool SERespiratorySystem::HasInspiratoryTidalVolume() const
{
  return m_InspiratoryTidalVolume == nullptr ? false : m_InspiratoryTidalVolume->IsValid();
}
SEScalarVolume& SERespiratorySystem::GetInspiratoryTidalVolume()
{
  if (m_InspiratoryTidalVolume == nullptr)
    m_InspiratoryTidalVolume = new SEScalarVolume();
  return *m_InspiratoryTidalVolume;
}
double SERespiratorySystem::GetInspiratoryTidalVolume(const VolumeUnit& unit) const
{
  if (m_InspiratoryTidalVolume == nullptr)
    return SEScalar::dNaN();
  return m_InspiratoryTidalVolume->GetValue(unit);
}

bool SERespiratorySystem::HasIntrapleuralPressure() const
{
  return m_IntrapleuralPressure == nullptr ? false : m_IntrapleuralPressure->IsValid();
}
SEScalarPressure& SERespiratorySystem::GetIntrapleuralPressure()
{
  if (m_IntrapleuralPressure == nullptr)
    m_IntrapleuralPressure = new SEScalarPressure();
  return *m_IntrapleuralPressure;
}
double SERespiratorySystem::GetIntrapleuralPressure(const PressureUnit& unit) const
{
  if (m_IntrapleuralPressure == nullptr)
    return SEScalar::dNaN();
  return m_IntrapleuralPressure->GetValue(unit);
}

bool SERespiratorySystem::HasIntrapulmonaryPressure() const
{
  return m_IntrapulmonaryPressure == nullptr ? false : m_IntrapulmonaryPressure->IsValid();
}
SEScalarPressure& SERespiratorySystem::GetIntrapulmonaryPressure()
{
  if (m_IntrapulmonaryPressure == nullptr)
    m_IntrapulmonaryPressure = new SEScalarPressure();
  return *m_IntrapulmonaryPressure;
}
double SERespiratorySystem::GetIntrapulmonaryPressure(const PressureUnit& unit) const
{
  if (m_IntrapulmonaryPressure == nullptr)
    return SEScalar::dNaN();
  return m_IntrapulmonaryPressure->GetValue(unit);
}

bool SERespiratorySystem::HasIntrinsicPositiveEndExpiratoryPressure() const
{
  return m_IntrinsicPositiveEndExpiratoryPressure == nullptr ? false : m_IntrinsicPositiveEndExpiratoryPressure->IsValid();
}
SEScalarPressure& SERespiratorySystem::GetIntrinsicPositiveEndExpiratoryPressure()
{
  if (m_IntrinsicPositiveEndExpiratoryPressure == nullptr)
    m_IntrinsicPositiveEndExpiratoryPressure = new SEScalarPressure();
  return *m_IntrinsicPositiveEndExpiratoryPressure;
}
double SERespiratorySystem::GetIntrinsicPositiveEndExpiratoryPressure(const PressureUnit& unit) const
{
  if (m_IntrinsicPositiveEndExpiratoryPressure == nullptr)
    return SEScalar::dNaN();
  return m_IntrinsicPositiveEndExpiratoryPressure->GetValue(unit);
}

bool SERespiratorySystem::HasLungCompliance() const
{
  return m_LungCompliance == nullptr ? false : m_LungCompliance->IsValid();
}
SEScalarVolumePerPressure& SERespiratorySystem::GetLungCompliance()
{
  if (m_LungCompliance == nullptr)
    m_LungCompliance = new SEScalarVolumePerPressure();
  return *m_LungCompliance;
}
double SERespiratorySystem::GetLungCompliance(const VolumePerPressureUnit& unit) const
{
  if (m_LungCompliance == nullptr)
    return SEScalar::dNaN();
  return m_LungCompliance->GetValue(unit);
}

bool SERespiratorySystem::HasMaximalInspiratoryPressure() const
{
  return m_MaximalInspiratoryPressure == nullptr ? false : m_MaximalInspiratoryPressure->IsValid();
}
SEScalarPressure& SERespiratorySystem::GetMaximalInspiratoryPressure()
{
  if (m_MaximalInspiratoryPressure == nullptr)
    m_MaximalInspiratoryPressure = new SEScalarPressure();
  return *m_MaximalInspiratoryPressure;
}
double SERespiratorySystem::GetMaximalInspiratoryPressure(const PressureUnit& unit) const
{
  if (m_MaximalInspiratoryPressure == nullptr)
    return SEScalar::dNaN();
  return m_MaximalInspiratoryPressure->GetValue(unit);
}

bool SERespiratorySystem::HasMeanAirwayPressure() const
{
  return m_MeanAirwayPressure == nullptr ? false : m_MeanAirwayPressure->IsValid();
}
SEScalarPressure& SERespiratorySystem::GetMeanAirwayPressure()
{
  if (m_MeanAirwayPressure == nullptr)
    m_MeanAirwayPressure = new SEScalarPressure();
  return *m_MeanAirwayPressure;
}
double SERespiratorySystem::GetMeanAirwayPressure(const PressureUnit& unit) const
{
  if (m_MeanAirwayPressure == nullptr)
    return SEScalar::dNaN();
  return m_MeanAirwayPressure->GetValue(unit);
}

bool SERespiratorySystem::HasOxygenationIndex() const
{
  return m_OxygenationIndex == nullptr ? false : m_OxygenationIndex->IsValid();
}
SEScalar& SERespiratorySystem::GetOxygenationIndex()
{
  if (m_OxygenationIndex == nullptr)
    m_OxygenationIndex = new SEScalar();
  return *m_OxygenationIndex;
}
double SERespiratorySystem::GetOxygenationIndex() const
{
  if (m_OxygenationIndex == nullptr)
    return SEScalar::dNaN();
  return m_OxygenationIndex->GetValue();
}

bool SERespiratorySystem::HasOxygenSaturationIndex() const
{
  return m_OxygenSaturationIndex == nullptr ? false : m_OxygenSaturationIndex->IsValid();
}
SEScalarPressure& SERespiratorySystem::GetOxygenSaturationIndex()
{
  if (m_OxygenSaturationIndex == nullptr)
    m_OxygenSaturationIndex = new SEScalarPressure();
  return *m_OxygenSaturationIndex;
}
double SERespiratorySystem::GetOxygenSaturationIndex(const PressureUnit& unit) const
{
  if (m_OxygenSaturationIndex == nullptr)
    return SEScalar::dNaN();
  return m_OxygenSaturationIndex->GetValue(unit);
}
bool SERespiratorySystem::HasPatientPowerOfBreathing() const
{
  return m_PatientPowerOfBreathing == nullptr ? false : m_PatientPowerOfBreathing->IsValid();
}
SEScalarPower& SERespiratorySystem::GetPatientPowerOfBreathing()
{
  if (m_PatientPowerOfBreathing == nullptr)
    m_PatientPowerOfBreathing = new SEScalarPower();
  return *m_PatientPowerOfBreathing;
}
double SERespiratorySystem::GetPatientPowerOfBreathing(const PowerUnit& unit) const
{
  if (m_PatientPowerOfBreathing == nullptr)
    return SEScalar::dNaN();
  return m_PatientPowerOfBreathing->GetValue(unit);
}

bool SERespiratorySystem::HasPatientWorkOfBreathing() const
{
  return m_PatientWorkOfBreathing == nullptr ? false : m_PatientWorkOfBreathing->IsValid();
}
SEScalarEnergy& SERespiratorySystem::GetPatientWorkOfBreathing()
{
  if (m_PatientWorkOfBreathing == nullptr)
    m_PatientWorkOfBreathing = new SEScalarEnergy();
  return *m_PatientWorkOfBreathing;
}
double SERespiratorySystem::GetPatientWorkOfBreathing(const EnergyUnit& unit) const
{
  if (m_PatientWorkOfBreathing == nullptr)
    return SEScalar::dNaN();
  return m_PatientWorkOfBreathing->GetValue(unit);
}

bool SERespiratorySystem::HasPeakInspiratoryPressure() const
{
  return m_PeakInspiratoryPressure == nullptr ? false : m_PeakInspiratoryPressure->IsValid();
}
SEScalarPressure& SERespiratorySystem::GetPeakInspiratoryPressure()
{
  if (m_PeakInspiratoryPressure == nullptr)
    m_PeakInspiratoryPressure = new SEScalarPressure();
  return *m_PeakInspiratoryPressure;
}
double SERespiratorySystem::GetPeakInspiratoryPressure(const PressureUnit& unit) const
{
  if (m_PeakInspiratoryPressure == nullptr)
    return SEScalar::dNaN();
  return m_PeakInspiratoryPressure->GetValue(unit);
}

bool SERespiratorySystem::HasPhysiologicDeadSpace() const
{
  return m_PhysiologicDeadSpace == nullptr ? false : m_PhysiologicDeadSpace->IsValid();
}
SEScalarVolume& SERespiratorySystem::GetPhysiologicDeadSpace()
{
  if (m_PhysiologicDeadSpace == nullptr)
    m_PhysiologicDeadSpace = new SEScalarVolume();
  return *m_PhysiologicDeadSpace;
}
double SERespiratorySystem::GetPhysiologicDeadSpace(const VolumeUnit& unit) const
{
  if (m_PhysiologicDeadSpace == nullptr)
    return SEScalar::dNaN();
  return m_PhysiologicDeadSpace->GetValue(unit);
}

bool SERespiratorySystem::HasPhysiologicDeadSpaceTidalVolumeRatio() const
{
  return m_PhysiologicDeadSpaceTidalVolumeRatio == nullptr ? false : m_PhysiologicDeadSpaceTidalVolumeRatio->IsValid();
}
SEScalar& SERespiratorySystem::GetPhysiologicDeadSpaceTidalVolumeRatio()
{
  if (m_PhysiologicDeadSpaceTidalVolumeRatio == nullptr)
    m_PhysiologicDeadSpaceTidalVolumeRatio = new SEScalar();
  return *m_PhysiologicDeadSpaceTidalVolumeRatio;
}
double SERespiratorySystem::GetPhysiologicDeadSpaceTidalVolumeRatio() const
{
  if (m_PhysiologicDeadSpaceTidalVolumeRatio == nullptr)
    return SEScalar::dNaN();
  return m_PhysiologicDeadSpaceTidalVolumeRatio->GetValue();
}

bool SERespiratorySystem::HasRespiratoryCompliance() const
{
  return m_RespiratoryCompliance == nullptr ? false : m_RespiratoryCompliance->IsValid();
}
SEScalarVolumePerPressure& SERespiratorySystem::GetRespiratoryCompliance()
{
  if (m_RespiratoryCompliance== nullptr)
    m_RespiratoryCompliance = new SEScalarVolumePerPressure();
  return *m_RespiratoryCompliance;
}
double SERespiratorySystem::GetRespiratoryCompliance(const VolumePerPressureUnit& unit) const
{
  if (m_RespiratoryCompliance == nullptr)
    return SEScalar::dNaN();
  return m_RespiratoryCompliance->GetValue(unit);
}

bool SERespiratorySystem::HasRespiratoryElastance() const
{
  return m_RespiratoryElastance == nullptr ? false : m_RespiratoryElastance->IsValid();
}
SEScalarPressurePerVolume& SERespiratorySystem::GetRespiratoryElastance()
{
  if (m_RespiratoryElastance == nullptr)
    m_RespiratoryElastance = new SEScalarPressurePerVolume();
  return *m_RespiratoryElastance;
}
double SERespiratorySystem::GetRespiratoryElastance(const PressurePerVolumeUnit& unit) const
{
  if (m_RespiratoryElastance == nullptr)
    return SEScalar::dNaN();
  return m_RespiratoryElastance->GetValue(unit);
}

bool SERespiratorySystem::HasRelativeTotalLungVolume() const
{
  return m_RelativeTotalLungVolume == nullptr ? false : m_RelativeTotalLungVolume->IsValid();
}
SEScalarVolume& SERespiratorySystem::GetRelativeTotalLungVolume()
{
  if (m_RelativeTotalLungVolume == nullptr)
    m_RelativeTotalLungVolume = new SEScalarVolume();
  return *m_RelativeTotalLungVolume;
}
double SERespiratorySystem::GetRelativeTotalLungVolume(const VolumeUnit& unit) const
{
  if (m_RelativeTotalLungVolume == nullptr)
    return SEScalar::dNaN();
  return m_RelativeTotalLungVolume->GetValue(unit);
}

bool SERespiratorySystem::HasResistiveExpiratoryWorkOfBreathing() const
{
  return m_ResistiveExpiratoryWorkOfBreathing == nullptr ? false : m_ResistiveExpiratoryWorkOfBreathing->IsValid();
}
SEScalarEnergy& SERespiratorySystem::GetResistiveExpiratoryWorkOfBreathing()
{
  if (m_ResistiveExpiratoryWorkOfBreathing == nullptr)
    m_ResistiveExpiratoryWorkOfBreathing = new SEScalarEnergy();
  return *m_ResistiveExpiratoryWorkOfBreathing;
}
double SERespiratorySystem::GetResistiveExpiratoryWorkOfBreathing(const EnergyUnit& unit) const
{
  if (m_ResistiveExpiratoryWorkOfBreathing == nullptr)
    return SEScalar::dNaN();
  return m_ResistiveExpiratoryWorkOfBreathing->GetValue(unit);
}

bool SERespiratorySystem::HasResistiveInspiratoryWorkOfBreathing() const
{
  return m_ResistiveInspiratoryWorkOfBreathing == nullptr ? false : m_ResistiveInspiratoryWorkOfBreathing->IsValid();
}
SEScalarEnergy& SERespiratorySystem::GetResistiveInspiratoryWorkOfBreathing()
{
  if (m_ResistiveInspiratoryWorkOfBreathing == nullptr)
    m_ResistiveInspiratoryWorkOfBreathing = new SEScalarEnergy();
  return *m_ResistiveInspiratoryWorkOfBreathing;
}
double SERespiratorySystem::GetResistiveInspiratoryWorkOfBreathing(const EnergyUnit& unit) const
{
  if (m_ResistiveInspiratoryWorkOfBreathing == nullptr)
    return SEScalar::dNaN();
  return m_ResistiveInspiratoryWorkOfBreathing->GetValue(unit);
}

bool SERespiratorySystem::HasRespirationRate() const
{
  return m_RespirationRate == nullptr ? false : m_RespirationRate->IsValid();
}
SEScalarFrequency& SERespiratorySystem::GetRespirationRate()
{
  if (m_RespirationRate == nullptr)
    m_RespirationRate = new SEScalarFrequency();
  return *m_RespirationRate;
}
double SERespiratorySystem::GetRespirationRate(const FrequencyUnit& unit) const
{
  if (m_RespirationRate == nullptr)
    return SEScalar::dNaN();
  return m_RespirationRate->GetValue(unit);
}

bool SERespiratorySystem::HasRespiratoryMuscleFatigue() const
{
  return m_RespiratoryMuscleFatigue == nullptr ? false : m_RespiratoryMuscleFatigue->IsValid();
}
SEScalar0To1& SERespiratorySystem::GetRespiratoryMuscleFatigue()
{
  if (m_RespiratoryMuscleFatigue == nullptr)
    m_RespiratoryMuscleFatigue = new SEScalar0To1();
  return *m_RespiratoryMuscleFatigue;
}
double SERespiratorySystem::GetRespiratoryMuscleFatigue() const
{
  if (m_RespiratoryMuscleFatigue == nullptr)
    return SEScalar::dNaN();
  return m_RespiratoryMuscleFatigue->GetValue();
}

bool SERespiratorySystem::HasRespiratoryMusclePressure() const
{
  return m_RespiratoryMusclePressure == nullptr ? false : m_RespiratoryMusclePressure->IsValid();
}
SEScalarPressure& SERespiratorySystem::GetRespiratoryMusclePressure()
{
  if (m_RespiratoryMusclePressure == nullptr)
    m_RespiratoryMusclePressure = new SEScalarPressure();
  return *m_RespiratoryMusclePressure;
}
double SERespiratorySystem::GetRespiratoryMusclePressure(const PressureUnit& unit) const
{
  if (m_RespiratoryMusclePressure == nullptr)
    return SEScalar::dNaN();
  return m_RespiratoryMusclePressure->GetValue(unit);
}

bool SERespiratorySystem::HasSaturationAndFractionOfInspiredOxygenRatio() const
{
  return m_SaturationAndFractionOfInspiredOxygenRatio == nullptr ? false : m_SaturationAndFractionOfInspiredOxygenRatio->IsValid();
}
SEScalar& SERespiratorySystem::GetSaturationAndFractionOfInspiredOxygenRatio()
{
  if (m_SaturationAndFractionOfInspiredOxygenRatio == nullptr)
    m_SaturationAndFractionOfInspiredOxygenRatio = new SEScalar();
  return *m_SaturationAndFractionOfInspiredOxygenRatio;
}
double SERespiratorySystem::GetSaturationAndFractionOfInspiredOxygenRatio() const
{
  if (m_SaturationAndFractionOfInspiredOxygenRatio == nullptr)
    return SEScalar::dNaN();
  return m_SaturationAndFractionOfInspiredOxygenRatio->GetValue();
}

bool SERespiratorySystem::HasSpecificVentilation() const
{
  return m_SpecificVentilation == nullptr ? false : m_SpecificVentilation->IsValid();
}
SEScalar& SERespiratorySystem::GetSpecificVentilation()
{
  if (m_SpecificVentilation == nullptr)
    m_SpecificVentilation = new SEScalar();
  return *m_SpecificVentilation;
}
double SERespiratorySystem::GetSpecificVentilation() const
{
  if (m_SpecificVentilation == nullptr)
    return SEScalar::dNaN();
  return m_SpecificVentilation->GetValue();
}

bool SERespiratorySystem::HasTidalVolume() const
{
  return m_TidalVolume==nullptr?false:m_TidalVolume->IsValid();
}
SEScalarVolume& SERespiratorySystem::GetTidalVolume()
{
  if(m_TidalVolume==nullptr)
    m_TidalVolume=new SEScalarVolume();
  return *m_TidalVolume;
}
double SERespiratorySystem::GetTidalVolume(const VolumeUnit& unit) const
{
  if (m_TidalVolume == nullptr)
    return SEScalar::dNaN();
  return m_TidalVolume->GetValue(unit);
}

bool SERespiratorySystem::HasTotalAlveolarVentilation() const
{
  return m_TotalAlveolarVentilation==nullptr?false:m_TotalAlveolarVentilation->IsValid();
}
SEScalarVolumePerTime& SERespiratorySystem::GetTotalAlveolarVentilation()
{
  if(m_TotalAlveolarVentilation==nullptr)
    m_TotalAlveolarVentilation=new SEScalarVolumePerTime();
  return *m_TotalAlveolarVentilation;
}
double SERespiratorySystem::GetTotalAlveolarVentilation(const VolumePerTimeUnit& unit) const
{
  if (m_TotalAlveolarVentilation == nullptr)
    return SEScalar::dNaN();
  return m_TotalAlveolarVentilation->GetValue(unit);
}

bool SERespiratorySystem::HasTotalDeadSpaceVentilation() const
{
  return m_TotalDeadSpaceVentilation == nullptr ? false : m_TotalDeadSpaceVentilation->IsValid();
}
SEScalarVolumePerTime& SERespiratorySystem::GetTotalDeadSpaceVentilation()
{
  if (m_TotalDeadSpaceVentilation == nullptr)
    m_TotalDeadSpaceVentilation = new SEScalarVolumePerTime();
  return *m_TotalDeadSpaceVentilation;
}
double SERespiratorySystem::GetTotalDeadSpaceVentilation(const VolumePerTimeUnit& unit) const
{
  if (m_TotalDeadSpaceVentilation == nullptr)
    return SEScalar::dNaN();
  return m_TotalDeadSpaceVentilation->GetValue(unit);
}

bool SERespiratorySystem::HasTotalLungVolume() const
{
  return m_TotalLungVolume == nullptr ? false : m_TotalLungVolume->IsValid();
}
SEScalarVolume& SERespiratorySystem::GetTotalLungVolume()
{
  if (m_TotalLungVolume == nullptr)
    m_TotalLungVolume = new SEScalarVolume();
  return *m_TotalLungVolume;
}
double SERespiratorySystem::GetTotalLungVolume(const VolumeUnit& unit) const
{
  if (m_TotalLungVolume == nullptr)
    return SEScalar::dNaN();
  return m_TotalLungVolume->GetValue(unit);
}

bool SERespiratorySystem::HasTotalPositiveEndExpiratoryPressure() const
{
  return m_TotalPositiveEndExpiratoryPressure == nullptr ? false : m_TotalPositiveEndExpiratoryPressure->IsValid();
}
SEScalarPressure& SERespiratorySystem::GetTotalPositiveEndExpiratoryPressure()
{
  if (m_TotalPositiveEndExpiratoryPressure == nullptr)
    m_TotalPositiveEndExpiratoryPressure = new SEScalarPressure();
  return *m_TotalPositiveEndExpiratoryPressure;
}
double SERespiratorySystem::GetTotalPositiveEndExpiratoryPressure(const PressureUnit& unit) const
{
  if (m_TotalPositiveEndExpiratoryPressure == nullptr)
    return SEScalar::dNaN();
  return m_TotalPositiveEndExpiratoryPressure->GetValue(unit);
}

bool SERespiratorySystem::HasTotalPowerOfBreathing() const
{
  return m_TotalPowerOfBreathing == nullptr ? false : m_TotalPowerOfBreathing->IsValid();
}
SEScalarPower& SERespiratorySystem::GetTotalPowerOfBreathing()
{
  if (m_TotalPowerOfBreathing == nullptr)
    m_TotalPowerOfBreathing = new SEScalarPower();
  return *m_TotalPowerOfBreathing;
}
double SERespiratorySystem::GetTotalPowerOfBreathing(const PowerUnit& unit) const
{
  if (m_TotalPowerOfBreathing == nullptr)
    return SEScalar::dNaN();
  return m_TotalPowerOfBreathing->GetValue(unit);
}

bool SERespiratorySystem::HasTotalPulmonaryVentilation() const
{
  return m_TotalPulmonaryVentilation==nullptr?false:m_TotalPulmonaryVentilation->IsValid();
}
SEScalarVolumePerTime& SERespiratorySystem::GetTotalPulmonaryVentilation()
{
  if (m_TotalPulmonaryVentilation == nullptr)
    m_TotalPulmonaryVentilation = new SEScalarVolumePerTime();
  return *m_TotalPulmonaryVentilation;
}
double SERespiratorySystem::GetTotalPulmonaryVentilation(const VolumePerTimeUnit& unit) const
{
  if (m_TotalPulmonaryVentilation == nullptr)
    return SEScalar::dNaN();
  return m_TotalPulmonaryVentilation->GetValue(unit);
}

bool SERespiratorySystem::HasTotalWorkOfBreathing() const
{
  return m_TotalWorkOfBreathing == nullptr ? false : m_TotalWorkOfBreathing->IsValid();
}
SEScalarEnergy& SERespiratorySystem::GetTotalWorkOfBreathing()
{
  if (m_TotalWorkOfBreathing == nullptr)
    m_TotalWorkOfBreathing = new SEScalarEnergy();
  return *m_TotalWorkOfBreathing;
}
double SERespiratorySystem::GetTotalWorkOfBreathing(const EnergyUnit& unit) const
{
  if (m_TotalWorkOfBreathing == nullptr)
    return SEScalar::dNaN();
  return m_TotalWorkOfBreathing->GetValue(unit);
}

bool SERespiratorySystem::HasTransairwayPressure() const
{
  return m_TransairwayPressure == nullptr ? false : m_TransairwayPressure->IsValid();
}
SEScalarPressure& SERespiratorySystem::GetTransairwayPressure()
{
  if (m_TransairwayPressure == nullptr)
    m_TransairwayPressure = new SEScalarPressure();
  return *m_TransairwayPressure;
}
double SERespiratorySystem::GetTransairwayPressure(const PressureUnit& unit) const
{
  if (m_TransairwayPressure == nullptr)
    return SEScalar::dNaN();
  return m_TransairwayPressure->GetValue(unit);
}

bool SERespiratorySystem::HasTransalveolarPressure() const
{
  return m_TransalveolarPressure == nullptr ? false : m_TransalveolarPressure->IsValid();
}
SEScalarPressure& SERespiratorySystem::GetTransalveolarPressure()
{
  if (m_TransalveolarPressure == nullptr)
    m_TransalveolarPressure = new SEScalarPressure();
  return *m_TransalveolarPressure;
}
double SERespiratorySystem::GetTransalveolarPressure(const PressureUnit& unit) const
{
  if (m_TransalveolarPressure == nullptr)
    return SEScalar::dNaN();
  return m_TransalveolarPressure->GetValue(unit);
}

bool SERespiratorySystem::HasTransChestWallPressure() const
{
  return m_TransChestWallPressure == nullptr ? false : m_TransChestWallPressure->IsValid();
}
SEScalarPressure& SERespiratorySystem::GetTransChestWallPressure()
{
  if (m_TransChestWallPressure == nullptr)
    m_TransChestWallPressure = new SEScalarPressure();
  return *m_TransChestWallPressure;
}
double SERespiratorySystem::GetTransChestWallPressure(const PressureUnit& unit) const
{
  if (m_TransChestWallPressure == nullptr)
    return SEScalar::dNaN();
  return m_TransChestWallPressure->GetValue(unit);
}

bool SERespiratorySystem::HasTransMusclePressure() const
{
  return m_TransMusclePressure == nullptr ? false : m_TransMusclePressure->IsValid();
}
SEScalarPressure& SERespiratorySystem::GetTransMusclePressure()
{
  if (m_TransMusclePressure == nullptr)
    m_TransMusclePressure = new SEScalarPressure();
  return *m_TransMusclePressure;
}
double SERespiratorySystem::GetTransMusclePressure(const PressureUnit& unit) const
{
  if (m_TransMusclePressure == nullptr)
    return SEScalar::dNaN();
  return m_TransMusclePressure->GetValue(unit);
}

bool SERespiratorySystem::HasTranspulmonaryPressure() const
{
  return m_TranspulmonaryPressure == nullptr ? false : m_TranspulmonaryPressure->IsValid();
}
SEScalarPressure& SERespiratorySystem::GetTranspulmonaryPressure()
{
  if (m_TranspulmonaryPressure == nullptr)
    m_TranspulmonaryPressure = new SEScalarPressure();
  return *m_TranspulmonaryPressure;
}
double SERespiratorySystem::GetTranspulmonaryPressure(const PressureUnit& unit) const
{
  if (m_TranspulmonaryPressure == nullptr)
    return SEScalar::dNaN();
  return m_TranspulmonaryPressure->GetValue(unit);
}

bool SERespiratorySystem::HasTransrespiratoryPressure() const
{
  return m_TransrespiratoryPressure == nullptr ? false : m_TransrespiratoryPressure->IsValid();
}
SEScalarPressure& SERespiratorySystem::GetTransrespiratoryPressure()
{
  if (m_TransrespiratoryPressure == nullptr)
    m_TransrespiratoryPressure = new SEScalarPressure();
  return *m_TransrespiratoryPressure;
}
double SERespiratorySystem::GetTransrespiratoryPressure(const PressureUnit& unit) const
{
  if (m_TransrespiratoryPressure == nullptr)
    return SEScalar::dNaN();
  return m_TransrespiratoryPressure->GetValue(unit);
}

bool SERespiratorySystem::HasTransthoracicPressure() const
{
  return m_TransthoracicPressure == nullptr ? false : m_TransthoracicPressure->IsValid();
}
SEScalarPressure& SERespiratorySystem::GetTransthoracicPressure()
{
  if (m_TransthoracicPressure == nullptr)
    m_TransthoracicPressure = new SEScalarPressure();
  return *m_TransthoracicPressure;
}
double SERespiratorySystem::GetTransthoracicPressure(const PressureUnit& unit) const
{
  if (m_TransthoracicPressure == nullptr)
    return SEScalar::dNaN();
  return m_TransthoracicPressure->GetValue(unit);
}

bool SERespiratorySystem::HasVentilationPerfusionRatio() const
{
  return m_VentilationPerfusionRatio == nullptr ? false : m_VentilationPerfusionRatio->IsValid();
}
SEScalar& SERespiratorySystem::GetVentilationPerfusionRatio()
{
  if (m_VentilationPerfusionRatio == nullptr)
    m_VentilationPerfusionRatio = new SEScalar();
  return *m_VentilationPerfusionRatio;
}
double SERespiratorySystem::GetVentilationPerfusionRatio() const
{
  if (m_VentilationPerfusionRatio == nullptr)
    return SEScalar::dNaN();
  return m_VentilationPerfusionRatio->GetValue();
}

bool SERespiratorySystem::HasActiveMechanics() const
{
  return m_Mechanics != nullptr && m_Mechanics->GetActive() == eSwitch::On;
}
bool SERespiratorySystem::HasMechanics() const
{
  return m_Mechanics != nullptr;
}
SERespiratoryMechanics& SERespiratorySystem::GetMechanics()
{
  if (m_Mechanics == nullptr)
    m_Mechanics = new SERespiratoryMechanics(GetLogger());
  return *m_Mechanics;
}
const SERespiratoryMechanics* SERespiratorySystem::GetMechanics() const
{
  return m_Mechanics;
}

SEScalar0To1* GetSeverity(LungImpairmentMap& map, eLungCompartment c)
{
  SEScalar0To1* s = map[c];
  if (s == nullptr)
  {
    s = new SEScalar0To1();
    map[c] = s;
  }
  return s;
}
void SERespiratorySystem::FillLungImpairmentMap(LungImpairmentMap& map)
{
  SEScalar0To1* lung;
  SEScalar0To1* lobe;
  lung = GetSeverity(map, eLungCompartment::LeftLung);
  if (!lung->IsValid())
    lung->SetValue(0);
  // Child Lobes
  lobe = GetSeverity(map, eLungCompartment::LeftInferiorLobe);
  if (!lobe->IsValid())
    lobe->Set(*lung);
  lobe = GetSeverity(map, eLungCompartment::LeftSuperiorLobe);
  if (!lobe->IsValid())
    lobe->Set(*lung);

  lung = GetSeverity(map, eLungCompartment::RightLung);
  if (!lung->IsValid())
    lung->SetValue(0);
  // Child Lobes
  lobe = GetSeverity(map, eLungCompartment::RightInferiorLobe);
  if (!lobe->IsValid())
    lobe->Set(*lung);
  lobe = GetSeverity(map, eLungCompartment::RightMiddleLobe);
  if (!lobe->IsValid())
    lobe->Set(*lung);
  lobe = GetSeverity(map, eLungCompartment::RightSuperiorLobe);
  if (!lobe->IsValid())
    lobe->Set(*lung);
}
