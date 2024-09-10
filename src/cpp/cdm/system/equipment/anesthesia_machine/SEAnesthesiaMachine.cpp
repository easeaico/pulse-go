/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/system/equipment/anesthesia_machine/SEAnesthesiaMachine.h"
#include "cdm/system/equipment/anesthesia_machine/SEAnesthesiaMachineChamber.h"
#include "cdm/system/equipment/anesthesia_machine/SEAnesthesiaMachineOxygenBottle.h"
#include "cdm/substance/SESubstanceManager.h"

// State Actions
#include "cdm/system/equipment/anesthesia_machine/actions/SEAnesthesiaMachineConfiguration.h"

#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarVolumePerTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/io/protobuf/PBAnesthesiaMachine.h"

SEAnesthesiaMachine::SEAnesthesiaMachine(Logger* logger) : SEEquipment(logger)
{
  m_Connection = eSwitch::Off;
  m_InletFlow = nullptr;
  m_InspiratoryExpiratoryRatio = nullptr;
  m_OxygenFraction = nullptr;
  m_OxygenSource = eAnesthesiaMachine_OxygenSource::NoSource;
  m_PeakInspiratoryPressure = nullptr;
  m_PositiveEndExpiratoryPressure = nullptr;
  m_PrimaryGas = eAnesthesiaMachine_PrimaryGas::NoGas;
  m_RespiratoryRate = nullptr;
  m_ReliefValvePressure = nullptr;
  m_LeftChamber = nullptr;
  m_RightChamber = nullptr;
  m_OxygenBottleOne = nullptr;
  m_OxygenBottleTwo = nullptr;
}

SEAnesthesiaMachine::~SEAnesthesiaMachine()
{
  m_Connection = eSwitch::Off;
  SAFE_DELETE(m_InletFlow);
  SAFE_DELETE(m_InspiratoryExpiratoryRatio);
  SAFE_DELETE(m_OxygenFraction);
  m_OxygenSource = eAnesthesiaMachine_OxygenSource::NoSource;
  SAFE_DELETE(m_PositiveEndExpiratoryPressure);
  m_PrimaryGas = eAnesthesiaMachine_PrimaryGas::NoGas;
  SAFE_DELETE(m_RespiratoryRate);
  SAFE_DELETE(m_ReliefValvePressure);
  SAFE_DELETE(m_PeakInspiratoryPressure);
  SAFE_DELETE(m_LeftChamber);
  SAFE_DELETE(m_RightChamber);
  SAFE_DELETE(m_OxygenBottleOne);
  SAFE_DELETE(m_OxygenBottleTwo);
}

void SEAnesthesiaMachine::Clear()
{
  SEEquipment::Clear();

  m_Connection = eSwitch::Off;
  INVALIDATE_PROPERTY(m_InletFlow);
  INVALIDATE_PROPERTY(m_InspiratoryExpiratoryRatio);
  INVALIDATE_PROPERTY(m_OxygenFraction);
  m_OxygenSource = eAnesthesiaMachine_OxygenSource::NoSource;
  INVALIDATE_PROPERTY(m_PositiveEndExpiratoryPressure);
  m_PrimaryGas = eAnesthesiaMachine_PrimaryGas::NoGas;
  INVALIDATE_PROPERTY(m_RespiratoryRate);
  INVALIDATE_PROPERTY(m_ReliefValvePressure);
  INVALIDATE_PROPERTY(m_PeakInspiratoryPressure);
  if (m_LeftChamber)
    m_LeftChamber->Clear();
  if(m_RightChamber)
    m_RightChamber->Clear();
  if(m_OxygenBottleOne)
    m_OxygenBottleOne->Clear();
  if(m_OxygenBottleTwo)
    m_OxygenBottleTwo->Clear();
}

void SEAnesthesiaMachine::ProcessConfiguration(SEAnesthesiaMachineConfiguration& config, SESubstanceManager& subMgr)
{
  if (config.GetMergeType() == eMergeType::Replace)
    Clear();
  if (config.HasConfiguration())
    Merge(config.GetConfiguration(), subMgr);
  else if (config.HasConfigurationFile())
  {
    // Update the action with the file contents
    std::string cfg_file = config.GetConfigurationFile();
    if (!config.GetConfiguration().SerializeFromFile(cfg_file, subMgr))
      Error("Unable to load configuration file");
    Merge(config.GetConfiguration(), subMgr);
  }
}
void SEAnesthesiaMachine::Merge(const SEAnesthesiaMachine& from, SESubstanceManager& subMgr)
{
  if(from.m_Connection!=eSwitch::NullSwitch)
    SetConnection(from.m_Connection);
  COPY_PROPERTY(InletFlow);
  COPY_PROPERTY(InspiratoryExpiratoryRatio);
  COPY_PROPERTY(OxygenFraction);
  if (from.m_OxygenSource != eAnesthesiaMachine_OxygenSource::NullSource)
    SetOxygenSource(from.m_OxygenSource);
  COPY_PROPERTY(PeakInspiratoryPressure);
  COPY_PROPERTY(PositiveEndExpiratoryPressure);
  if (from.m_PrimaryGas != eAnesthesiaMachine_PrimaryGas::NullGas)
    SetPrimaryGas(from.m_PrimaryGas);
  COPY_PROPERTY(RespiratoryRate);
  COPY_PROPERTY(ReliefValvePressure);

  MERGE_CHILD_SUBS(LeftChamber, subMgr);
  MERGE_CHILD_SUBS(RightChamber, subMgr);
 
  MERGE_CHILD(OxygenBottleOne);
  MERGE_CHILD(OxygenBottleTwo);
}

bool SEAnesthesiaMachine::SerializeToString(std::string& output, eSerializationFormat m) const
{
  return PBAnesthesiaMachine::SerializeToString(*this, output, m);
}
bool SEAnesthesiaMachine::SerializeToFile(const std::string& filename) const
{
  return PBAnesthesiaMachine::SerializeToFile(*this, filename);
}
bool SEAnesthesiaMachine::SerializeFromString(const std::string& src, eSerializationFormat m, const SESubstanceManager& subMgr)
{
  return PBAnesthesiaMachine::SerializeFromString(src, *this, m, subMgr);
}
bool SEAnesthesiaMachine::SerializeFromFile(const std::string& filename, const SESubstanceManager& subMgr)
{
  return PBAnesthesiaMachine::SerializeFromFile(filename, *this, subMgr);
}


const SEScalar* SEAnesthesiaMachine::GetScalar(const std::string& name)
{
  if (name == "InletFlow")
    return &GetInletFlow();
  if (name == "InspiratoryExpiratoryRatio")
    return &GetInspiratoryExpiratoryRatio();
  if (name == "OxygenFraction")
    return &GetOxygenFraction();
  if (name == "PeakInspiratoryPressure")
    return &GetPeakInspiratoryPressure();
  if (name == "PositiveEndExpiratoryPressure")
    return &GetPositiveEndExpiratoryPressure();
  if (name == "ReliefValvePressure")
    return &GetReliefValvePressure();
  if (name == "RespiratoryRate")
    return &GetRespiratoryRate();

  size_t split = name.find('-');
  if (split != name.npos)
  {
    std::string child = name.substr(0, split);
    std::string prop = name.substr(split + 1, name.npos);
    if (child == "LeftChamber")
      return GetLeftChamber().GetScalar(prop);
    if (child == "RightChamber")
      return GetRightChamber().GetScalar(prop);
    if (child == "OxygenBottleOne")
      return GetOxygenBottleOne().GetScalar(prop);
    if (child == "OxygenBottleTwo")
      return GetOxygenBottleTwo().GetScalar(prop);
  }

  return nullptr;
}

void SEAnesthesiaMachine::SetConnection(eSwitch c)
{
  m_Connection = c;
}
eSwitch SEAnesthesiaMachine::GetConnection() const
{
  return m_Connection;
}

bool SEAnesthesiaMachine::HasInletFlow() const
{
  return m_InletFlow == nullptr ? false : m_InletFlow->IsValid();
}
SEScalarVolumePerTime& SEAnesthesiaMachine::GetInletFlow()
{
  if (m_InletFlow == nullptr)
    m_InletFlow = new SEScalarVolumePerTime();
  return *m_InletFlow;
}
double SEAnesthesiaMachine::GetInletFlow(const VolumePerTimeUnit& unit) const
{
  if (m_InletFlow == nullptr)
    return SEScalar::dNaN();
  return m_InletFlow->GetValue(unit);
}

bool SEAnesthesiaMachine::HasInspiratoryExpiratoryRatio() const
{
  return m_InspiratoryExpiratoryRatio == nullptr ? false : m_InspiratoryExpiratoryRatio->IsValid();
}
SEScalar& SEAnesthesiaMachine::GetInspiratoryExpiratoryRatio()
{
  if (m_InspiratoryExpiratoryRatio == nullptr)
    m_InspiratoryExpiratoryRatio = new SEScalar();
  return *m_InspiratoryExpiratoryRatio;
}
double SEAnesthesiaMachine::GetInspiratoryExpiratoryRatio() const
{
  if (m_InspiratoryExpiratoryRatio == nullptr)
    return SEScalar::dNaN();
  return m_InspiratoryExpiratoryRatio->GetValue();
}

bool SEAnesthesiaMachine::HasOxygenFraction() const
{
  return m_OxygenFraction == nullptr ? false : m_OxygenFraction->IsValid();
}
SEScalar0To1& SEAnesthesiaMachine::GetOxygenFraction()
{
  if (m_OxygenFraction == nullptr)
    m_OxygenFraction = new SEScalar0To1();
  return *m_OxygenFraction;
}
double SEAnesthesiaMachine::GetOxygenFraction() const
{
  if (m_OxygenFraction == nullptr)
    return SEScalar::dNaN();
  return m_OxygenFraction->GetValue();
}

eAnesthesiaMachine_OxygenSource SEAnesthesiaMachine::GetOxygenSource() const
{
  return m_OxygenSource;
}
void SEAnesthesiaMachine::SetOxygenSource(eAnesthesiaMachine_OxygenSource src)
{
  m_OxygenSource = src;
}

bool SEAnesthesiaMachine::HasPeakInspiratoryPressure() const
{
  return m_PeakInspiratoryPressure == nullptr ? false : m_PeakInspiratoryPressure->IsValid();
}
SEScalarPressure& SEAnesthesiaMachine::GetPeakInspiratoryPressure()
{
  if (m_PeakInspiratoryPressure == nullptr)
    m_PeakInspiratoryPressure = new SEScalarPressure();
  return *m_PeakInspiratoryPressure;
}
double SEAnesthesiaMachine::GetPeakInspiratoryPressure(const PressureUnit& unit) const
{
  if (m_PeakInspiratoryPressure == nullptr)
    return SEScalar::dNaN();
  return m_PeakInspiratoryPressure->GetValue(unit);
}

bool SEAnesthesiaMachine::HasPositiveEndExpiratoryPressure() const
{
  return m_PositiveEndExpiratoryPressure == nullptr ? false : m_PositiveEndExpiratoryPressure->IsValid();
}
SEScalarPressure& SEAnesthesiaMachine::GetPositiveEndExpiratoryPressure()
{
  if (m_PositiveEndExpiratoryPressure == nullptr)
    m_PositiveEndExpiratoryPressure = new SEScalarPressure();
  return *m_PositiveEndExpiratoryPressure;
}
double SEAnesthesiaMachine::GetPositiveEndExpiratoryPressure(const PressureUnit& unit) const
{
  if (m_PositiveEndExpiratoryPressure == nullptr)
    return SEScalar::dNaN();
  return m_PositiveEndExpiratoryPressure->GetValue(unit);
}

eAnesthesiaMachine_PrimaryGas SEAnesthesiaMachine::GetPrimaryGas() const
{
  return m_PrimaryGas;
}
void SEAnesthesiaMachine::SetPrimaryGas(eAnesthesiaMachine_PrimaryGas gas)
{
  m_PrimaryGas = gas;
}

bool SEAnesthesiaMachine::HasRespiratoryRate() const
{
  return m_RespiratoryRate == nullptr ? false : m_RespiratoryRate->IsValid();
}
SEScalarFrequency& SEAnesthesiaMachine::GetRespiratoryRate()
{
  if (m_RespiratoryRate == nullptr)
    m_RespiratoryRate = new SEScalarFrequency();
  return *m_RespiratoryRate;
}
double SEAnesthesiaMachine::GetRespiratoryRate(const FrequencyUnit& unit) const
{
  if (m_RespiratoryRate == nullptr)
    return SEScalar::dNaN();
  return m_RespiratoryRate->GetValue(unit);
}

bool SEAnesthesiaMachine::HasReliefValvePressure() const
{
  return m_ReliefValvePressure == nullptr ? false : m_ReliefValvePressure->IsValid();
}
SEScalarPressure& SEAnesthesiaMachine::GetReliefValvePressure()
{
  if (m_ReliefValvePressure == nullptr)
    m_ReliefValvePressure = new SEScalarPressure();
  return *m_ReliefValvePressure;
}
double SEAnesthesiaMachine::GetReliefValvePressure(const PressureUnit& unit) const
{
  if (m_ReliefValvePressure == nullptr)
    return SEScalar::dNaN();
  return m_ReliefValvePressure->GetValue(unit);
}

bool SEAnesthesiaMachine::HasLeftChamber() const
{
  return m_LeftChamber == nullptr ? false : true;
}
SEAnesthesiaMachineChamber& SEAnesthesiaMachine::GetLeftChamber()
{
  if (m_LeftChamber == nullptr)
    m_LeftChamber = new SEAnesthesiaMachineChamber(GetLogger());
  return *m_LeftChamber;
}
const SEAnesthesiaMachineChamber* SEAnesthesiaMachine::GetLeftChamber() const
{
  return m_LeftChamber;
}
void SEAnesthesiaMachine::RemoveLeftChamber()
{
  SAFE_DELETE(m_LeftChamber);
}

bool SEAnesthesiaMachine::HasRightChamber() const
{
  return m_RightChamber == nullptr ? false : true;
}
SEAnesthesiaMachineChamber& SEAnesthesiaMachine::GetRightChamber()
{
  if (m_RightChamber == nullptr)
    m_RightChamber = new SEAnesthesiaMachineChamber(GetLogger());
  return *m_RightChamber;
}
const SEAnesthesiaMachineChamber* SEAnesthesiaMachine::GetRightChamber() const
{
  return m_RightChamber;
}
void SEAnesthesiaMachine::RemoveRightChamber()
{
  SAFE_DELETE(m_RightChamber);
}

bool SEAnesthesiaMachine::HasOxygenBottleOne() const
{
  return m_OxygenBottleOne == nullptr ? false : true;
}
SEAnesthesiaMachineOxygenBottle& SEAnesthesiaMachine::GetOxygenBottleOne()
{
  if (m_OxygenBottleOne == nullptr)
    m_OxygenBottleOne = new SEAnesthesiaMachineOxygenBottle(GetLogger());
  return *m_OxygenBottleOne;
}
const SEAnesthesiaMachineOxygenBottle* SEAnesthesiaMachine::GetOxygenBottleOne() const
{
  return m_OxygenBottleOne;
}
void SEAnesthesiaMachine::RemoveOxygenBottleOne()
{
  SAFE_DELETE(m_OxygenBottleOne);
}

bool SEAnesthesiaMachine::HasOxygenBottleTwo() const
{
  return m_OxygenBottleTwo == nullptr ? false : true;
}
SEAnesthesiaMachineOxygenBottle& SEAnesthesiaMachine::GetOxygenBottleTwo()
{
  if (m_OxygenBottleTwo == nullptr)
    m_OxygenBottleTwo = new SEAnesthesiaMachineOxygenBottle(GetLogger());
  return *m_OxygenBottleTwo;
}
const SEAnesthesiaMachineOxygenBottle* SEAnesthesiaMachine::GetOxygenBottleTwo() const
{
  return m_OxygenBottleTwo;
}
void SEAnesthesiaMachine::RemoveOxygenBottleTwo()
{
  SAFE_DELETE(m_OxygenBottleTwo);
}
