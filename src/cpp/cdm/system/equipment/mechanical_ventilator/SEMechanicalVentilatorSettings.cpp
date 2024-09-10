/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/system/equipment/mechanical_ventilator/SEMechanicalVentilatorSettings.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorConfiguration.h"

#include "cdm/substance/SESubstance.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarPressureTimePerVolume.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerPressure.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/io/protobuf/PBMechanicalVentilator.h"

SEMechanicalVentilatorSettings::SEMechanicalVentilatorSettings(Logger* logger) : Loggable(logger)
{
  m_Connection = eSwitch::NullSwitch;
  m_ConnectionVolume = nullptr;
  m_Compliance = nullptr;
  m_DriverDampingParameter = nullptr;

  m_PositiveEndExpiratoryPressure = nullptr;
  m_FunctionalResidualCapacity = nullptr;

  m_ExpirationCycleFlow = nullptr;
  m_ExpirationCyclePressure = nullptr;
  m_ExpirationCycleVolume = nullptr;
  m_ExpirationCycleTime = nullptr;
  m_ExpirationCycleRespiratoryModel = eSwitch::NullSwitch;

  m_ExpirationTubeResistance = nullptr;
  m_ExpirationValveResistance = nullptr;
  m_ExpirationWaveform = eDriverWaveform::NullDriverWaveform;
  m_ExpirationWaveformPeriod = nullptr;

  m_InspirationLimitFlow = nullptr;
  m_InspirationLimitPressure = nullptr;
  m_InspirationLimitVolume = nullptr;

  m_InspirationPauseTime = nullptr;

  m_PeakInspiratoryPressure = nullptr;
  m_InspirationTargetFlow = nullptr;

  m_InspirationMachineTriggerTime = nullptr;

  m_InspirationPatientTriggerFlow = nullptr;
  m_InspirationPatientTriggerPressure = nullptr;
  m_InspirationPatientTriggerRespiratoryModel = eSwitch::NullSwitch;

  m_InspirationTubeResistance = nullptr;
  m_InspirationValveResistance = nullptr;
  m_InspirationWaveform = eDriverWaveform::NullDriverWaveform;
  m_InspirationWaveformPeriod = nullptr;

  m_ExpirationLimbVolume = nullptr;
  m_ExpirationValveVolume = nullptr;
  m_InspirationLimbVolume = nullptr;
  m_InspirationValveVolume = nullptr;

  m_ReliefValveThreshold = nullptr;
  m_YPieceVolume = nullptr;
}

SEMechanicalVentilatorSettings::~SEMechanicalVentilatorSettings()
{
  m_Connection = eSwitch::NullSwitch;
  SAFE_DELETE(m_ConnectionVolume);
  SAFE_DELETE(m_Compliance);
  SAFE_DELETE(m_DriverDampingParameter);

  SAFE_DELETE(m_PositiveEndExpiratoryPressure);
  SAFE_DELETE(m_FunctionalResidualCapacity);

  SAFE_DELETE(m_ExpirationCycleFlow);
  SAFE_DELETE(m_ExpirationCyclePressure);
  SAFE_DELETE(m_ExpirationCycleVolume);
  SAFE_DELETE(m_ExpirationCycleTime);
  m_ExpirationCycleRespiratoryModel = eSwitch::NullSwitch;

  SAFE_DELETE(m_ExpirationTubeResistance);
  SAFE_DELETE(m_ExpirationValveResistance);
  m_ExpirationWaveform = eDriverWaveform::NullDriverWaveform;
  SAFE_DELETE(m_ExpirationWaveformPeriod);

  SAFE_DELETE(m_InspirationLimitFlow);
  SAFE_DELETE(m_InspirationLimitPressure);
  SAFE_DELETE(m_InspirationLimitVolume);

  SAFE_DELETE(m_InspirationPauseTime);

  SAFE_DELETE(m_PeakInspiratoryPressure);
  SAFE_DELETE(m_InspirationTargetFlow);

  SAFE_DELETE(m_InspirationMachineTriggerTime);

  SAFE_DELETE(m_InspirationPatientTriggerFlow);
  SAFE_DELETE(m_InspirationPatientTriggerPressure);
  m_InspirationPatientTriggerRespiratoryModel = eSwitch::NullSwitch;

  SAFE_DELETE(m_InspirationTubeResistance);
  SAFE_DELETE(m_InspirationValveResistance);
  m_InspirationWaveform = eDriverWaveform::NullDriverWaveform;
  SAFE_DELETE(m_InspirationWaveformPeriod);

  SAFE_DELETE(m_ExpirationLimbVolume);
  SAFE_DELETE(m_ExpirationValveVolume);
  SAFE_DELETE(m_InspirationLimbVolume);
  SAFE_DELETE(m_InspirationValveVolume);

  SAFE_DELETE(m_ReliefValveThreshold);
  SAFE_DELETE(m_YPieceVolume);

  DELETE_VECTOR(m_FractionInspiredGases);
  m_cFractionInspiredGases.clear();
  m_FractionInspiredGases.clear();

  DELETE_VECTOR(m_ConcentrationInspiredAerosols);
  m_cConcentrationInspiredAerosols.clear();
  m_ConcentrationInspiredAerosols.clear();
}

void SEMechanicalVentilatorSettings::Clear()
{
  m_Connection = eSwitch::NullSwitch;
  INVALIDATE_PROPERTY(m_ConnectionVolume);
  INVALIDATE_PROPERTY(m_Compliance);
  INVALIDATE_PROPERTY(m_DriverDampingParameter);

  INVALIDATE_PROPERTY(m_PositiveEndExpiratoryPressure);
  INVALIDATE_PROPERTY(m_FunctionalResidualCapacity);

  INVALIDATE_PROPERTY(m_ExpirationCycleFlow);
  INVALIDATE_PROPERTY(m_ExpirationCyclePressure);
  INVALIDATE_PROPERTY(m_ExpirationCycleVolume);
  INVALIDATE_PROPERTY(m_ExpirationCycleTime);
  m_ExpirationCycleRespiratoryModel = eSwitch::NullSwitch;

  INVALIDATE_PROPERTY(m_ExpirationTubeResistance);
  INVALIDATE_PROPERTY(m_ExpirationValveResistance);
  m_ExpirationWaveform = eDriverWaveform::NullDriverWaveform;
  INVALIDATE_PROPERTY(m_ExpirationWaveformPeriod);

  INVALIDATE_PROPERTY(m_InspirationLimitFlow);
  INVALIDATE_PROPERTY(m_InspirationLimitPressure);
  INVALIDATE_PROPERTY(m_InspirationLimitVolume);

  INVALIDATE_PROPERTY(m_InspirationPauseTime);

  INVALIDATE_PROPERTY(m_PeakInspiratoryPressure);
  INVALIDATE_PROPERTY(m_InspirationTargetFlow);

  INVALIDATE_PROPERTY(m_InspirationMachineTriggerTime);

  INVALIDATE_PROPERTY(m_InspirationPatientTriggerFlow);
  INVALIDATE_PROPERTY(m_InspirationPatientTriggerPressure);
  m_InspirationPatientTriggerRespiratoryModel = eSwitch::NullSwitch;

  INVALIDATE_PROPERTY(m_InspirationTubeResistance);
  INVALIDATE_PROPERTY(m_InspirationValveResistance);
  m_InspirationWaveform = eDriverWaveform::NullDriverWaveform;
  INVALIDATE_PROPERTY(m_InspirationWaveformPeriod);

  INVALIDATE_PROPERTY(m_ExpirationLimbVolume);
  INVALIDATE_PROPERTY(m_ExpirationValveVolume);
  INVALIDATE_PROPERTY(m_InspirationLimbVolume);
  INVALIDATE_PROPERTY(m_InspirationValveVolume);

  INVALIDATE_PROPERTY(m_ReliefValveThreshold);
  INVALIDATE_PROPERTY(m_YPieceVolume);

  RemoveFractionInspiredGases();
  RemoveConcentrationInspiredAerosols();
}

void SEMechanicalVentilatorSettings::ProcessConfiguration(SEMechanicalVentilatorConfiguration& config, SESubstanceManager& subMgr)
{
  if (config.GetMergeType() == eMergeType::Replace)
    Clear();
  if (config.HasSettings())
    Merge(config.GetSettings(), subMgr);
  else if (config.HasSettingsFile())
  {
    // Update the action with the file contents
    std::string cfg_file = config.GetSettingsFile();
    if (!config.GetSettings().SerializeFromFile(cfg_file, subMgr))
      Error("Unable to load settings file");
    Merge(config.GetSettings(), subMgr);
  }
}
void SEMechanicalVentilatorSettings::Merge(const SEMechanicalVentilatorSettings& from, SESubstanceManager& subMgr)
{
  if(from.HasConnection())
    SetConnection(from.m_Connection);
  COPY_PROPERTY(ConnectionVolume);
  COPY_PROPERTY(Compliance);
  COPY_PROPERTY(DriverDampingParameter);

  // Oneof
  COPY_PROPERTY(PositiveEndExpiratoryPressure);
  COPY_PROPERTY(FunctionalResidualCapacity);

  // Oneof
  COPY_PROPERTY(ExpirationCycleFlow);
  COPY_PROPERTY(ExpirationCyclePressure);
  COPY_PROPERTY(ExpirationCycleVolume);
  COPY_PROPERTY(ExpirationCycleTime);
  if (from.HasExpirationCycleRespiratoryModel())
    SetExpirationCycleRespiratoryModel(from.m_ExpirationCycleRespiratoryModel);

  COPY_PROPERTY(ExpirationTubeResistance);
  COPY_PROPERTY(ExpirationValveResistance);
  if (from.HasExpirationWaveform())
    SetExpirationWaveform(from.m_ExpirationWaveform);
  COPY_PROPERTY(ExpirationWaveformPeriod);

  // Oneof
  COPY_PROPERTY(InspirationLimitFlow);
  COPY_PROPERTY(InspirationLimitPressure);
  COPY_PROPERTY(InspirationLimitVolume);

  COPY_PROPERTY(InspirationPauseTime);

  // Oneof
  COPY_PROPERTY(PeakInspiratoryPressure);
  COPY_PROPERTY(InspirationTargetFlow);

  // Oneof
  COPY_PROPERTY(InspirationMachineTriggerTime);

  // Oneof
  COPY_PROPERTY(InspirationPatientTriggerFlow);
  COPY_PROPERTY(InspirationPatientTriggerPressure);
  if (from.HasInspirationPatientTriggerRespiratoryModel())
    SetInspirationPatientTriggerRespiratoryModel(from.m_InspirationPatientTriggerRespiratoryModel);

  COPY_PROPERTY(InspirationTubeResistance);
  COPY_PROPERTY(InspirationValveResistance);
  if (from.HasInspirationWaveform())
    SetInspirationWaveform(from.m_InspirationWaveform);
  COPY_PROPERTY(InspirationWaveformPeriod);

  COPY_PROPERTY(ExpirationLimbVolume);
  COPY_PROPERTY(ExpirationValveVolume);
  COPY_PROPERTY(InspirationLimbVolume);
  COPY_PROPERTY(InspirationValveVolume);
  COPY_PROPERTY(ReliefValveThreshold);
  COPY_PROPERTY(YPieceVolume);

  // Always need to provide a full (fractions sum to 1) substance list that replaces current
  if (from.HasFractionInspiredGas())
  {
    size_t cnt = 0;
    double amt;
    double total = 0;
    const SESubstance* sub;
    // Since we are allowing only O2 to be specified
    // Remove everything so we know what is intentionally not provided
    // And what is intentially set to 0 (don't just set to 0)
    RemoveFractionInspiredGases();
    for (SESubstanceFraction* osf : from.m_FractionInspiredGases)
    {
      sub = subMgr.GetSubstance(osf->GetSubstance().GetName());
      if (sub == nullptr)
      {
        Error("Do not have substance : " + osf->GetSubstance().GetName());
        continue;
      }
      SESubstanceFraction& sf = GetFractionInspiredGas(*sub);
      amt = osf->GetFractionAmount().GetValue();
      sf.GetFractionAmount().SetValue(amt);
      subMgr.AddActiveSubstance(*sub);
      total += amt;
      if (amt > 0)
        cnt++; // Count of non-zero fractions
    }

    // It's Ok if you ONLY set Oxygen, i.e. FiO2
    // Ventilator models should understand that common setting
    if (!SEScalar::IsValue(1, total))
    {
      bool err = false;
      if (cnt != 1)
        err = true;
      else
      {
        const SESubstance* o2 = subMgr.GetSubstance("Oxygen");
        if (!GetFractionInspiredGas(*o2).GetFractionAmount().IsPositive())
          err = true;
      }
      if(err)
        Error("Mechanical Ventilator substance fractions do not sum to 1");
    }
  }

  if (from.HasConcentrationInspiredAerosol())
  {
    for (SESubstanceConcentration* sc : from.m_ConcentrationInspiredAerosols)
    {
      SESubstanceConcentration& mine = GetConcentrationInspiredAerosol(sc->GetSubstance());
      mine.GetConcentration().Set(sc->GetConcentration());
    }
  }

  //std::string out;
  //this->SerializeToString(out, eSerializationFormat::JSON);
  //std::cout << out << std::endl;
}

bool SEMechanicalVentilatorSettings::SerializeToString(std::string& output, eSerializationFormat m) const
{
  return PBMechanicalVentilator::SerializeToString(*this, output, m);
}
bool SEMechanicalVentilatorSettings::SerializeToFile(const std::string& filename) const
{
  return PBMechanicalVentilator::SerializeToFile(*this, filename);
}
bool SEMechanicalVentilatorSettings::SerializeFromString(const std::string& src, eSerializationFormat m, const SESubstanceManager& subMgr)
{
  return PBMechanicalVentilator::SerializeFromString(src, *this, m, subMgr);
}
bool SEMechanicalVentilatorSettings::SerializeFromFile(const std::string& filename, const SESubstanceManager& subMgr)
{
  return PBMechanicalVentilator::SerializeFromFile(filename, *this, subMgr);
}

const SEScalar* SEMechanicalVentilatorSettings::GetScalar(const std::string& name)
{

  if (name == "ConnectionVolume")
    return &GetConnectionVolume();
  if (name == "Compliance")
    return &GetCompliance();
  if (name == "DriverDampingParameter")
    return &GetDriverDampingParameter();

  if (name == "PositiveEndExpiratoryPressure")
    return &GetPositiveEndExpiratoryPressure();
  if (name == "FunctionalResidualCapacity")
    return &GetFunctionalResidualCapacity();

  if (name == "ExpirationCycleFlow")
    return &GetExpirationCycleFlow();
  if (name == "ExpirationCyclePressure")
    return &GetExpirationCyclePressure();
  if (name == "ExpirationCycleVolume")
    return &GetExpirationCycleVolume();
  if (name == "ExpirationCycleTime")
    return &GetExpirationCycleTime();
  if (name == "ExpirationWaveformPeriod")
    return &GetExpirationWaveformPeriod();

  if (name == "ExpirationTubeResistance")
    return &GetExpirationTubeResistance();
  if (name == "ExpirationValveResistance")
    return &GetExpirationValveResistance();

  if (name == "InspirationLimitFlow")
    return &GetInspirationLimitFlow();
  if (name == "InspirationLimitPressure")
    return &GetInspirationLimitPressure();
  if (name == "InspirationLimitVolume")
    return &GetInspirationLimitVolume();

  if (name == "InspirationPauseTime")
    return &GetInspirationPauseTime();

  if (name == "PeakInspiratoryPressure")
    return &GetPeakInspiratoryPressure();
  if (name == "InspirationTargetFlow")
    return &GetInspirationTargetFlow();

  if (name == "InspirationMachineTriggerTime")
    return &GetInspirationMachineTriggerTime();

  if (name == "InspirationPatientTriggerFlow")
    return &GetInspirationPatientTriggerFlow();
  if (name == "InspirationPatientTriggerPressure")
    return &GetInspirationPatientTriggerPressure();

  if (name == "InspirationWaveformPeriod")
    return &GetInspirationWaveformPeriod();

  if (name == "InspirationTubeResistance")
    return &GetInspirationTubeResistance();
  if (name == "InspirationValveResistance")
    return &GetInspirationValveResistance();

  if (name == "ExpirationLimbVolume")
    return &GetExpirationLimbVolume();
  if (name == "ExpirationValveVolume")
    return &GetExpirationValveVolume();
  if (name == "InspirationLimbVolume")
    return &GetInspirationLimbVolume();
  if (name == "InspirationValveVolume")
    return &GetInspirationValveVolume();

  if (name == "ReliefValveThreshold")
    return &GetReliefValveThreshold();
  if (name == "YPieceVolume")
    return &GetYPieceVolume();

  // I did not support for getting a specific gas/aerosol scalars due to lack of coffee
  return nullptr;
}

bool SEMechanicalVentilatorSettings::HasConnection() const
{
  return m_Connection!=eSwitch::NullSwitch;
}
eSwitch SEMechanicalVentilatorSettings::GetConnection() const
{
  return m_Connection;
}
void SEMechanicalVentilatorSettings::SetConnection(eSwitch c)
{
  m_Connection = c;
}

bool SEMechanicalVentilatorSettings::HasPositiveEndExpiratoryPressure() const
{
  return m_PositiveEndExpiratoryPressure == nullptr ? false : m_PositiveEndExpiratoryPressure->IsValid();
}
SEScalarPressure& SEMechanicalVentilatorSettings::GetPositiveEndExpiratoryPressure()
{
  if (m_PositiveEndExpiratoryPressure == nullptr)
    m_PositiveEndExpiratoryPressure = new SEScalarPressure();
  return *m_PositiveEndExpiratoryPressure;
}
double SEMechanicalVentilatorSettings::GetPositiveEndExpiratoryPressure(const PressureUnit& unit) const
{
  if (m_PositiveEndExpiratoryPressure == nullptr)
    return SEScalar::dNaN();
  return m_PositiveEndExpiratoryPressure->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasFunctionalResidualCapacity() const
{
  return m_FunctionalResidualCapacity == nullptr ? false : m_FunctionalResidualCapacity->IsValid();
}
SEScalarPressure& SEMechanicalVentilatorSettings::GetFunctionalResidualCapacity()
{
  if (m_FunctionalResidualCapacity == nullptr)
    m_FunctionalResidualCapacity = new SEScalarPressure();
  return *m_FunctionalResidualCapacity;
}
double SEMechanicalVentilatorSettings::GetFunctionalResidualCapacity(const PressureUnit& unit) const
{
  if (m_FunctionalResidualCapacity == nullptr)
    return SEScalar::dNaN();
  return m_FunctionalResidualCapacity->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasExpirationCycleFlow() const
{
  return m_ExpirationCycleFlow == nullptr ? false : m_ExpirationCycleFlow->IsValid();
}
SEScalarVolumePerTime& SEMechanicalVentilatorSettings::GetExpirationCycleFlow()
{
  if (m_ExpirationCycleFlow == nullptr)
    m_ExpirationCycleFlow = new SEScalarVolumePerTime();
  return *m_ExpirationCycleFlow;
}
double SEMechanicalVentilatorSettings::GetExpirationCycleFlow(const VolumePerTimeUnit& unit) const
{
  if (m_ExpirationCycleFlow == nullptr)
    return SEScalar::dNaN();
  return m_ExpirationCycleFlow->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasExpirationCyclePressure() const
{
  return m_ExpirationCyclePressure == nullptr ? false : m_ExpirationCyclePressure->IsValid();
}
SEScalarPressure& SEMechanicalVentilatorSettings::GetExpirationCyclePressure()
{
  if (m_ExpirationCyclePressure == nullptr)
    m_ExpirationCyclePressure = new SEScalarPressure();
  return *m_ExpirationCyclePressure;
}
double SEMechanicalVentilatorSettings::GetExpirationCyclePressure(const PressureUnit& unit) const
{
  if (m_ExpirationCyclePressure == nullptr)
    return SEScalar::dNaN();
  return m_ExpirationCyclePressure->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasExpirationCycleVolume() const
{
  return m_ExpirationCycleVolume == nullptr ? false : m_ExpirationCycleVolume->IsValid();
}
SEScalarVolume& SEMechanicalVentilatorSettings::GetExpirationCycleVolume()
{
  if (m_ExpirationCycleVolume == nullptr)
    m_ExpirationCycleVolume = new SEScalarVolume();
  return *m_ExpirationCycleVolume;
}
double SEMechanicalVentilatorSettings::GetExpirationCycleVolume(const VolumeUnit& unit) const
{
  if (m_ExpirationCycleVolume == nullptr)
    return SEScalar::dNaN();
  return m_ExpirationCycleVolume->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasExpirationCycleTime() const
{
  return m_ExpirationCycleTime == nullptr ? false : m_ExpirationCycleTime->IsValid();
}
SEScalarTime& SEMechanicalVentilatorSettings::GetExpirationCycleTime()
{
  if (m_ExpirationCycleTime == nullptr)
    m_ExpirationCycleTime = new SEScalarTime();
  return *m_ExpirationCycleTime;
}
double SEMechanicalVentilatorSettings::GetExpirationCycleTime(const TimeUnit& unit) const
{
  if (m_ExpirationCycleTime == nullptr)
    return SEScalar::dNaN();
  return m_ExpirationCycleTime->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasExpirationCycleRespiratoryModel() const
{
  return m_ExpirationCycleRespiratoryModel!=eSwitch::NullSwitch;
}
eSwitch SEMechanicalVentilatorSettings::GetExpirationCycleRespiratoryModel() const
{
  return m_ExpirationCycleRespiratoryModel;
}
void SEMechanicalVentilatorSettings::SetExpirationCycleRespiratoryModel(eSwitch c)
{
  m_ExpirationCycleRespiratoryModel = c;
}

bool SEMechanicalVentilatorSettings::HasExpirationTubeResistance() const
{
  return m_ExpirationTubeResistance == nullptr ? false : m_ExpirationTubeResistance->IsValid();
}
SEScalarPressureTimePerVolume& SEMechanicalVentilatorSettings::GetExpirationTubeResistance()
{
  if (m_ExpirationTubeResistance == nullptr)
    m_ExpirationTubeResistance = new SEScalarPressureTimePerVolume();
  return *m_ExpirationTubeResistance;
}
double SEMechanicalVentilatorSettings::GetExpirationTubeResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_ExpirationTubeResistance == nullptr)
    return SEScalar::dNaN();
  return m_ExpirationTubeResistance->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasExpirationValveResistance() const
{
  return m_ExpirationValveResistance == nullptr ? false : m_ExpirationValveResistance->IsValid();
}
SEScalarPressureTimePerVolume& SEMechanicalVentilatorSettings::GetExpirationValveResistance()
{
  if (m_ExpirationValveResistance == nullptr)
    m_ExpirationValveResistance = new SEScalarPressureTimePerVolume();
  return *m_ExpirationValveResistance;
}
double SEMechanicalVentilatorSettings::GetExpirationValveResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_ExpirationValveResistance == nullptr)
    return SEScalar::dNaN();
  return m_ExpirationValveResistance->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasExpirationWaveform() const
{
  return m_ExpirationWaveform!=eDriverWaveform::NullDriverWaveform;
}
eDriverWaveform SEMechanicalVentilatorSettings::GetExpirationWaveform() const
{
  return m_ExpirationWaveform;
}
void SEMechanicalVentilatorSettings::SetExpirationWaveform(eDriverWaveform w)
{
  m_ExpirationWaveform = w;
}

bool SEMechanicalVentilatorSettings::HasExpirationWaveformPeriod() const
{
  return m_ExpirationWaveformPeriod == nullptr ? false : m_ExpirationWaveformPeriod->IsValid();
}
SEScalarTime& SEMechanicalVentilatorSettings::GetExpirationWaveformPeriod()
{
  if (m_ExpirationWaveformPeriod == nullptr)
    m_ExpirationWaveformPeriod = new SEScalarTime();
  return *m_ExpirationWaveformPeriod;
}
double SEMechanicalVentilatorSettings::GetExpirationWaveformPeriod(const TimeUnit& unit) const
{
  if (m_ExpirationWaveformPeriod == nullptr)
    return SEScalar::dNaN();
  return m_ExpirationWaveformPeriod->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasInspirationLimitFlow() const
{
  return m_InspirationLimitFlow == nullptr ? false : m_InspirationLimitFlow->IsValid();
}
SEScalarVolumePerTime& SEMechanicalVentilatorSettings::GetInspirationLimitFlow()
{
  if (m_InspirationLimitFlow == nullptr)
    m_InspirationLimitFlow = new SEScalarVolumePerTime();
  return *m_InspirationLimitFlow;
}
double SEMechanicalVentilatorSettings::GetInspirationLimitFlow(const VolumePerTimeUnit& unit) const
{
  if (m_InspirationLimitFlow == nullptr)
    return SEScalar::dNaN();
  return m_InspirationLimitFlow->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasInspirationLimitPressure() const
{
  return m_InspirationLimitPressure == nullptr ? false : m_InspirationLimitPressure->IsValid();
}
SEScalarPressure& SEMechanicalVentilatorSettings::GetInspirationLimitPressure()
{
  if (m_InspirationLimitPressure == nullptr)
    m_InspirationLimitPressure = new SEScalarPressure();
  return *m_InspirationLimitPressure;
}
double SEMechanicalVentilatorSettings::GetInspirationLimitPressure(const PressureUnit& unit) const
{
  if (m_InspirationLimitPressure == nullptr)
    return SEScalar::dNaN();
  return m_InspirationLimitPressure->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasInspirationLimitVolume() const
{
  return m_InspirationLimitVolume == nullptr ? false : m_InspirationLimitVolume->IsValid();
}
SEScalarVolume& SEMechanicalVentilatorSettings::GetInspirationLimitVolume()
{
  if (m_InspirationLimitVolume == nullptr)
    m_InspirationLimitVolume = new SEScalarVolume();
  return *m_InspirationLimitVolume;
}
double SEMechanicalVentilatorSettings::GetInspirationLimitVolume(const VolumeUnit& unit) const
{
  if (m_InspirationLimitVolume == nullptr)
    return SEScalar::dNaN();
  return m_InspirationLimitVolume->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasInspirationPauseTime() const
{
  return m_InspirationPauseTime == nullptr ? false : m_InspirationPauseTime->IsValid();
}
SEScalarTime& SEMechanicalVentilatorSettings::GetInspirationPauseTime()
{
  if (m_InspirationPauseTime == nullptr)
    m_InspirationPauseTime = new SEScalarTime();
  return *m_InspirationPauseTime;
}
double SEMechanicalVentilatorSettings::GetInspirationPauseTime(const TimeUnit& unit) const
{
  if (m_InspirationPauseTime == nullptr)
    return SEScalar::dNaN();
  return m_InspirationPauseTime->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasPeakInspiratoryPressure() const
{
  return m_PeakInspiratoryPressure == nullptr ? false : m_PeakInspiratoryPressure->IsValid();
}
SEScalarPressure& SEMechanicalVentilatorSettings::GetPeakInspiratoryPressure()
{
  if (m_PeakInspiratoryPressure == nullptr)
    m_PeakInspiratoryPressure = new SEScalarPressure();
  return *m_PeakInspiratoryPressure;
}
double SEMechanicalVentilatorSettings::GetPeakInspiratoryPressure(const PressureUnit& unit) const
{
  if (m_PeakInspiratoryPressure == nullptr)
    return SEScalar::dNaN();
  return m_PeakInspiratoryPressure->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasInspirationTargetFlow() const
{
  return m_InspirationTargetFlow == nullptr ? false : m_InspirationTargetFlow->IsValid();
}
SEScalarVolumePerTime& SEMechanicalVentilatorSettings::GetInspirationTargetFlow()
{
  if (m_InspirationTargetFlow == nullptr)
    m_InspirationTargetFlow = new SEScalarVolumePerTime();
  return *m_InspirationTargetFlow;
}
double SEMechanicalVentilatorSettings::GetInspirationTargetFlow(const VolumePerTimeUnit& unit) const
{
  if (m_InspirationTargetFlow == nullptr)
    return SEScalar::dNaN();
  return m_InspirationTargetFlow->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasInspirationMachineTriggerTime() const
{
  return m_InspirationMachineTriggerTime == nullptr ? false : m_InspirationMachineTriggerTime->IsValid();
}
SEScalarTime& SEMechanicalVentilatorSettings::GetInspirationMachineTriggerTime()
{
  if (m_InspirationMachineTriggerTime == nullptr)
    m_InspirationMachineTriggerTime = new SEScalarTime();
  return *m_InspirationMachineTriggerTime;
}
double SEMechanicalVentilatorSettings::GetInspirationMachineTriggerTime(const TimeUnit& unit) const
{
  if (m_InspirationMachineTriggerTime == nullptr)
    return SEScalar::dNaN();
  return m_InspirationMachineTriggerTime->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasInspirationPatientTriggerFlow() const
{
  return m_InspirationPatientTriggerFlow == nullptr ? false : m_InspirationPatientTriggerFlow->IsValid();
}
SEScalarVolumePerTime& SEMechanicalVentilatorSettings::GetInspirationPatientTriggerFlow()
{
  if (m_InspirationPatientTriggerFlow == nullptr)
    m_InspirationPatientTriggerFlow = new SEScalarVolumePerTime();
  return *m_InspirationPatientTriggerFlow;
}
double SEMechanicalVentilatorSettings::GetInspirationPatientTriggerFlow(const VolumePerTimeUnit& unit) const
{
  if (m_InspirationPatientTriggerFlow == nullptr)
    return SEScalar::dNaN();
  return m_InspirationPatientTriggerFlow->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasInspirationPatientTriggerPressure() const
{
  return m_InspirationPatientTriggerPressure == nullptr ? false : m_InspirationPatientTriggerPressure->IsValid();
}
SEScalarPressure& SEMechanicalVentilatorSettings::GetInspirationPatientTriggerPressure()
{
  if (m_InspirationPatientTriggerPressure == nullptr)
    m_InspirationPatientTriggerPressure = new SEScalarPressure();
  return *m_InspirationPatientTriggerPressure;
}
double SEMechanicalVentilatorSettings::GetInspirationPatientTriggerPressure(const PressureUnit& unit) const
{
  if (m_InspirationPatientTriggerPressure == nullptr)
    return SEScalar::dNaN();
  return m_InspirationPatientTriggerPressure->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasInspirationPatientTriggerRespiratoryModel() const
{
  return m_InspirationPatientTriggerRespiratoryModel!=eSwitch::NullSwitch;
}
eSwitch SEMechanicalVentilatorSettings::GetInspirationPatientTriggerRespiratoryModel() const
{
  return m_InspirationPatientTriggerRespiratoryModel;
}
void SEMechanicalVentilatorSettings::SetInspirationPatientTriggerRespiratoryModel(eSwitch c)
{
  m_InspirationPatientTriggerRespiratoryModel = c;
}

bool SEMechanicalVentilatorSettings::HasInspirationTubeResistance() const
{
  return m_InspirationTubeResistance == nullptr ? false : m_InspirationTubeResistance->IsValid();
}
SEScalarPressureTimePerVolume& SEMechanicalVentilatorSettings::GetInspirationTubeResistance()
{
  if (m_InspirationTubeResistance == nullptr)
    m_InspirationTubeResistance = new SEScalarPressureTimePerVolume();
  return *m_InspirationTubeResistance;
}
double SEMechanicalVentilatorSettings::GetInspirationTubeResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_InspirationTubeResistance == nullptr)
    return SEScalar::dNaN();
  return m_InspirationTubeResistance->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasInspirationValveResistance() const
{
  return m_InspirationValveResistance == nullptr ? false : m_InspirationValveResistance->IsValid();
}
SEScalarPressureTimePerVolume& SEMechanicalVentilatorSettings::GetInspirationValveResistance()
{
  if (m_InspirationValveResistance == nullptr)
    m_InspirationValveResistance = new SEScalarPressureTimePerVolume();
  return *m_InspirationValveResistance;
}
double SEMechanicalVentilatorSettings::GetInspirationValveResistance(const PressureTimePerVolumeUnit& unit) const
{
  if (m_InspirationValveResistance == nullptr)
    return SEScalar::dNaN();
  return m_InspirationValveResistance->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasExpirationLimbVolume() const
{
  return m_ExpirationLimbVolume == nullptr ? false : m_ExpirationLimbVolume->IsValid();
}
SEScalarVolume& SEMechanicalVentilatorSettings::GetExpirationLimbVolume()
{
  if (m_ExpirationLimbVolume == nullptr)
    m_ExpirationLimbVolume = new SEScalarVolume();
  return *m_ExpirationLimbVolume;
}
double SEMechanicalVentilatorSettings::GetExpirationLimbVolume(const VolumeUnit& unit) const
{
  if (m_ExpirationLimbVolume == nullptr)
    return SEScalar::dNaN();
  return m_ExpirationLimbVolume->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasExpirationValveVolume() const
{
  return m_ExpirationValveVolume == nullptr ? false : m_ExpirationValveVolume->IsValid();
}
SEScalarVolume& SEMechanicalVentilatorSettings::GetExpirationValveVolume()
{
  if (m_ExpirationValveVolume == nullptr)
    m_ExpirationValveVolume = new SEScalarVolume();
  return *m_ExpirationValveVolume;
}
double SEMechanicalVentilatorSettings::GetExpirationValveVolume(const VolumeUnit& unit) const
{
  if (m_ExpirationValveVolume == nullptr)
    return SEScalar::dNaN();
  return m_ExpirationValveVolume->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasInspirationLimbVolume() const
{
  return m_InspirationLimbVolume == nullptr ? false : m_InspirationLimbVolume->IsValid();
}
SEScalarVolume& SEMechanicalVentilatorSettings::GetInspirationLimbVolume()
{
  if (m_InspirationLimbVolume == nullptr)
    m_InspirationLimbVolume = new SEScalarVolume();
  return *m_InspirationLimbVolume;
}
double SEMechanicalVentilatorSettings::GetInspirationLimbVolume(const VolumeUnit& unit) const
{
  if (m_InspirationLimbVolume == nullptr)
    return SEScalar::dNaN();
  return m_InspirationLimbVolume->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasInspirationValveVolume() const
{
  return m_InspirationValveVolume == nullptr ? false : m_InspirationValveVolume->IsValid();
}
SEScalarVolume& SEMechanicalVentilatorSettings::GetInspirationValveVolume()
{
  if (m_InspirationValveVolume == nullptr)
    m_InspirationValveVolume = new SEScalarVolume();
  return *m_InspirationValveVolume;
}
double SEMechanicalVentilatorSettings::GetInspirationValveVolume(const VolumeUnit& unit) const
{
  if (m_InspirationValveVolume == nullptr)
    return SEScalar::dNaN();
  return m_InspirationValveVolume->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasReliefValveThreshold() const
{
  return m_ReliefValveThreshold == nullptr ? false : m_ReliefValveThreshold->IsValid();
}
SEScalarPressure& SEMechanicalVentilatorSettings::GetReliefValveThreshold()
{
  if (m_ReliefValveThreshold == nullptr)
    m_ReliefValveThreshold = new SEScalarPressure();
  return *m_ReliefValveThreshold;
}
double SEMechanicalVentilatorSettings::GetReliefValveThreshold(const PressureUnit& unit) const
{
  if (m_ReliefValveThreshold == nullptr)
    return SEScalar::dNaN();
  return m_ReliefValveThreshold->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasYPieceVolume() const
{
  return m_YPieceVolume == nullptr ? false : m_YPieceVolume->IsValid();
}
SEScalarVolume& SEMechanicalVentilatorSettings::GetYPieceVolume()
{
  if (m_YPieceVolume == nullptr)
    m_YPieceVolume = new SEScalarVolume();
  return *m_YPieceVolume;
}
double SEMechanicalVentilatorSettings::GetYPieceVolume(const VolumeUnit& unit) const
{
  if (m_YPieceVolume == nullptr)
    return SEScalar::dNaN();
  return m_YPieceVolume->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasConnectionVolume() const
{
  return m_ConnectionVolume == nullptr ? false : m_ConnectionVolume->IsValid();
}
SEScalarVolume& SEMechanicalVentilatorSettings::GetConnectionVolume()
{
  if (m_ConnectionVolume == nullptr)
    m_ConnectionVolume = new SEScalarVolume();
  return *m_ConnectionVolume;
}
double SEMechanicalVentilatorSettings::GetConnectionVolume(const VolumeUnit& unit) const
{
  if (m_ConnectionVolume == nullptr)
    return SEScalar::dNaN();
  return m_ConnectionVolume->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasCompliance() const
{
  return m_Compliance == nullptr ? false : m_Compliance->IsValid();
}
SEScalarVolumePerPressure& SEMechanicalVentilatorSettings::GetCompliance()
{
  if (m_Compliance == nullptr)
    m_Compliance = new SEScalarVolumePerPressure();
  return *m_Compliance;
}
double SEMechanicalVentilatorSettings::GetCompliance(const VolumePerPressureUnit& unit) const
{
  if (m_Compliance == nullptr)
    return SEScalar::dNaN();
  return m_Compliance->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasDriverDampingParameter() const
{
  return m_DriverDampingParameter == nullptr ? false : m_DriverDampingParameter->IsValid();
}
SEScalarFrequency& SEMechanicalVentilatorSettings::GetDriverDampingParameter()
{
  if (m_DriverDampingParameter == nullptr)
    m_DriverDampingParameter = new SEScalarFrequency();
  return *m_DriverDampingParameter;
}
double SEMechanicalVentilatorSettings::GetDriverDampingParameter(const FrequencyUnit& unit) const
{
  if (m_DriverDampingParameter == nullptr)
    return SEScalar::dNaN();
  return m_DriverDampingParameter->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasInspirationWaveform() const
{
  return m_InspirationWaveform!=eDriverWaveform::NullDriverWaveform;
}
eDriverWaveform SEMechanicalVentilatorSettings::GetInspirationWaveform() const
{
  return m_InspirationWaveform;
}
void SEMechanicalVentilatorSettings::SetInspirationWaveform(eDriverWaveform w)
{
  m_InspirationWaveform = w;
}

bool SEMechanicalVentilatorSettings::HasInspirationWaveformPeriod() const
{
  return m_InspirationWaveformPeriod == nullptr ? false : m_InspirationWaveformPeriod->IsValid();
}
SEScalarTime& SEMechanicalVentilatorSettings::GetInspirationWaveformPeriod()
{
  if (m_InspirationWaveformPeriod == nullptr)
    m_InspirationWaveformPeriod = new SEScalarTime();
  return *m_InspirationWaveformPeriod;
}
double SEMechanicalVentilatorSettings::GetInspirationWaveformPeriod(const TimeUnit& unit) const
{
  if (m_InspirationWaveformPeriod == nullptr)
    return SEScalar::dNaN();
  return m_InspirationWaveformPeriod->GetValue(unit);
}

bool SEMechanicalVentilatorSettings::HasFractionInspiredGas() const
{
  if (m_FractionInspiredGases.empty())
    return false;
  for (auto a : m_FractionInspiredGases)
    if (a->HasFractionAmount() && a->GetFractionAmount().IsPositive())
      return true;
  return false;
}
bool SEMechanicalVentilatorSettings::HasFractionInspiredGas(const SESubstance& s) const
{
  for (const SESubstanceFraction* sf : m_FractionInspiredGases)
  {
    if (&s == &sf->GetSubstance())
      return sf->GetFractionAmount() > 0;
  }
  return false;
}
const std::vector<SESubstanceFraction*>& SEMechanicalVentilatorSettings::GetFractionInspiredGases()
{
  return m_FractionInspiredGases;
}
const std::vector<const SESubstanceFraction*>& SEMechanicalVentilatorSettings::GetFractionInspiredGases() const
{
  return m_cFractionInspiredGases;
}
SESubstanceFraction& SEMechanicalVentilatorSettings::GetFractionInspiredGas(const SESubstance& s)
{
  for (SESubstanceFraction* sf : m_FractionInspiredGases)
  {
    if (&s == &sf->GetSubstance())
      return *sf;
  }
  SESubstanceFraction* sf = new SESubstanceFraction(s);
  sf->GetFractionAmount().SetValue(0);
  m_FractionInspiredGases.push_back(sf);
  m_cFractionInspiredGases.push_back(sf);
  return *sf;
}
const SESubstanceFraction* SEMechanicalVentilatorSettings::GetFractionInspiredGas(const SESubstance& s) const
{
  const SESubstanceFraction* sf = nullptr;
  for (unsigned int i = 0; i < m_FractionInspiredGases.size(); i++)
  {
    sf = m_FractionInspiredGases[i];
    if (&s == &sf->GetSubstance())
      return sf;
  }
  return sf;
}
void SEMechanicalVentilatorSettings::RemoveFractionInspiredGas(const SESubstance& s)
{
  SESubstanceFraction& sf = GetFractionInspiredGas(s);
  sf.GetFractionAmount().SetValue(0);
}
void SEMechanicalVentilatorSettings::RemoveFractionInspiredGases()
{
  for (SESubstanceFraction* sf : m_FractionInspiredGases)
    sf->GetFractionAmount().SetValue(0);
}

bool SEMechanicalVentilatorSettings::HasConcentrationInspiredAerosol() const
{
  if (m_ConcentrationInspiredAerosols.empty())
    return false;
  for (auto a : m_ConcentrationInspiredAerosols)
    if (a->HasConcentration() && a->GetConcentration().IsPositive())
      return true;
  return false;
}
bool SEMechanicalVentilatorSettings::HasConcentrationInspiredAerosol(const SESubstance& substance) const
{
  for (SESubstanceConcentration* sc : m_ConcentrationInspiredAerosols)
  {
    if (&substance == &sc->GetSubstance())
      return sc->GetConcentration().IsPositive();
  }
  return false;
}
const std::vector<SESubstanceConcentration*>& SEMechanicalVentilatorSettings::GetConcentrationInspiredAerosols()
{
  return m_ConcentrationInspiredAerosols;
}
const std::vector<const SESubstanceConcentration*>& SEMechanicalVentilatorSettings::GetConcentrationInspiredAerosols() const
{
  return m_cConcentrationInspiredAerosols;
}
SESubstanceConcentration& SEMechanicalVentilatorSettings::GetConcentrationInspiredAerosol(const SESubstance& substance)
{
  for (SESubstanceConcentration* sc : m_ConcentrationInspiredAerosols)
  {
    if (&substance == &sc->GetSubstance())
      return *sc;
  }
  SESubstanceConcentration* sc = new SESubstanceConcentration(substance);
  sc->GetConcentration().SetValue(0, MassPerVolumeUnit::ug_Per_L);
  m_ConcentrationInspiredAerosols.push_back(sc);
  m_cConcentrationInspiredAerosols.push_back(sc);
  return *sc;
}
const SESubstanceConcentration* SEMechanicalVentilatorSettings::GetConcentrationInspiredAerosol(const SESubstance& substance) const
{
  const SESubstanceConcentration* sc = nullptr;
  for (unsigned int i = 0; i < m_ConcentrationInspiredAerosols.size(); i++)
  {
    sc = m_ConcentrationInspiredAerosols[i];
    if (&substance == &sc->GetSubstance())
      return sc;
  }
  return sc;
}
void SEMechanicalVentilatorSettings::RemoveConcentrationInspiredAerosol(const SESubstance& substance)
{
  SESubstanceConcentration& sc = GetConcentrationInspiredAerosol(substance);
  auto& unit = *sc.GetConcentration().GetUnit();
  sc.GetConcentration().SetValue(0, unit);
}
void SEMechanicalVentilatorSettings::RemoveConcentrationInspiredAerosols()
{
  for (SESubstanceConcentration* sc : m_ConcentrationInspiredAerosols)
  {
    auto& unit = *sc->GetConcentration().GetUnit();
    sc->GetConcentration().SetValue(0, unit);
  }
}
