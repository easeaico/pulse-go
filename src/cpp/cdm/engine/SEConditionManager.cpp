/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/engine/SEConditionManager.h"
   //Patient Conditions
#include "cdm/patient/conditions/SEAcuteRespiratoryDistressSyndrome.h"
#include "cdm/patient/conditions/SEChronicAnemia.h"
#include "cdm/patient/conditions/SEChronicObstructivePulmonaryDisease.h"
#include "cdm/patient/conditions/SEChronicPericardialEffusion.h"
#include "cdm/patient/conditions/SEChronicRenalStenosis.h"
#include "cdm/patient/conditions/SEChronicVentricularSystolicDysfunction.h"
#include "cdm/patient/conditions/SEConsumeMeal.h"
#include "cdm/patient/conditions/SEDehydration.h"
#include "cdm/patient/conditions/SEImpairedAlveolarExchange.h"
#include "cdm/patient/conditions/SEPneumonia.h"
#include "cdm/patient/conditions/SEPulmonaryFibrosis.h"
#include "cdm/patient/conditions/SEPulmonaryShunt.h"
#include "cdm/patient/conditions/SESepsis.h"
// Environment Conditions
#include "cdm/system/environment/conditions/SEInitialEnvironmentalConditions.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/io/protobuf/PBEngine.h"

SEConditionManager::SEConditionManager(Logger* logger) : Loggable(logger)
{
  m_ARDS = nullptr;
  m_Anemia = nullptr;
  m_COPD = nullptr;
  m_ChronicVentricularSystolicDysfunction = nullptr;
  m_RenalStenosis = nullptr;
  m_ConsumeMeal = nullptr;
  m_Dehydration = nullptr;
  m_PericardialEffusion = nullptr;
  m_Pneumonia = nullptr;
  m_PulmonaryFibrosis = nullptr;
  m_PulmonaryShunt = nullptr;
  m_ImpairedAlveolarExchange = nullptr;
  m_InitialEnvironmentalConditions = nullptr;
  m_Sepsis = nullptr;
}

SEConditionManager::~SEConditionManager()
{
  SAFE_DELETE(m_ARDS);
  SAFE_DELETE(m_Anemia);
  SAFE_DELETE(m_COPD);
  SAFE_DELETE(m_ChronicVentricularSystolicDysfunction);
  SAFE_DELETE(m_RenalStenosis);
  SAFE_DELETE(m_ConsumeMeal);
  SAFE_DELETE(m_Dehydration);
  SAFE_DELETE(m_Pneumonia);
  SAFE_DELETE(m_PericardialEffusion);
  SAFE_DELETE(m_PulmonaryFibrosis);
  SAFE_DELETE(m_PulmonaryShunt);
  SAFE_DELETE(m_ImpairedAlveolarExchange);
  SAFE_DELETE(m_InitialEnvironmentalConditions);
  SAFE_DELETE(m_Sepsis);
}

void SEConditionManager::Clear()
{
  SAFE_DELETE(m_ARDS);
  SAFE_DELETE(m_Anemia);
  SAFE_DELETE(m_COPD);
  SAFE_DELETE(m_ChronicVentricularSystolicDysfunction);
  SAFE_DELETE(m_RenalStenosis);
  SAFE_DELETE(m_ConsumeMeal);
  SAFE_DELETE(m_Dehydration);
  SAFE_DELETE(m_Pneumonia);
  SAFE_DELETE(m_PericardialEffusion);
  SAFE_DELETE(m_PulmonaryFibrosis);
  SAFE_DELETE(m_PulmonaryShunt);
  SAFE_DELETE(m_ImpairedAlveolarExchange);
  SAFE_DELETE(m_InitialEnvironmentalConditions);
  SAFE_DELETE(m_Sepsis);
}

bool SEConditionManager::SerializeToString(std::string& output, eSerializationFormat m) const
{
  return PBEngine::SerializeToString(*this, output, m);
}
bool SEConditionManager::SerializeToFile(const std::string& filename) const
{
  return PBEngine::SerializeToFile(*this, filename);
}
bool SEConditionManager::SerializeFromString(const std::string& src, eSerializationFormat m, const SESubstanceManager& subMgr)
{
  return PBEngine::SerializeFromString(src, *this, m, subMgr);
}
bool SEConditionManager::SerializeFromFile(const std::string& filename, const SESubstanceManager& subMgr)
{
  return PBEngine::SerializeFromFile(filename, *this, subMgr);
}

void SEConditionManager::Copy(const SEConditionManager& src, const SESubstanceManager& subMgr)
{
  Clear();
  if (src.IsEmpty())
    return;

  std::vector<const SECondition*> conditions;
  src.GetAllConditions(conditions);
  for (const SECondition* c : conditions)
  {
    if (!Copy(*c, subMgr))
      Error("Could not process " + c->GetName());
  }
}

bool SEConditionManager::Copy(const SECondition& condition, const SESubstanceManager& subMgr)
{
  if (!condition.IsValid())
  {
    m_ss << "Ignoring invalid condition : " << condition;
    Error(m_ss);
    return false;
  }

  if (dynamic_cast<const SEPatientCondition*>(&condition) != nullptr)
  {
    const SEAcuteRespiratoryDistressSyndrome* ards = dynamic_cast<const SEAcuteRespiratoryDistressSyndrome*>(&condition);
    if (ards != nullptr)
    {
      GetAcuteRespiratoryDistressSyndrome().Copy(*ards);
      GetAcuteRespiratoryDistressSyndrome().Activate();
      return true;
    }

    const SEChronicAnemia* a = dynamic_cast<const SEChronicAnemia*>(&condition);
    if (a != nullptr)
    {
      GetChronicAnemia().Copy(*a);
      GetChronicAnemia().Activate();
      return true;
    }

    const SEChronicObstructivePulmonaryDisease* copd = dynamic_cast<const SEChronicObstructivePulmonaryDisease*>(&condition);
    if (copd != nullptr)
    {
      GetChronicObstructivePulmonaryDisease().Copy(*copd);
      GetChronicObstructivePulmonaryDisease().Activate();
      return true;
    }

    const SEChronicPericardialEffusion* pe = dynamic_cast<const SEChronicPericardialEffusion*>(&condition);
    if (pe != nullptr)
    {
      GetChronicPericardialEffusion().Copy(*pe);
      GetChronicPericardialEffusion().Activate();
      return true;
    }

    const SEChronicRenalStenosis* r = dynamic_cast<const SEChronicRenalStenosis*>(&condition);
    if (r != nullptr)
    {
      GetChronicRenalStenosis().Copy(*r);
      GetChronicRenalStenosis().Activate();
      return true;
    }

    const SEChronicVentricularSystolicDysfunction* vsd = dynamic_cast<const SEChronicVentricularSystolicDysfunction*>(&condition);
    if (vsd != nullptr)
    {
      GetChronicVentricularSystolicDysfunction().Copy(*vsd);
      GetChronicVentricularSystolicDysfunction().Activate();
      return true;
    }

    const SEConsumeMeal* g = dynamic_cast<const SEConsumeMeal*>(&condition);
    if (g != nullptr)
    {
      GetConsumeMeal().Copy(*g);
      GetConsumeMeal().Activate();
      return true;
    }

    const SEDehydration* d = dynamic_cast<const SEDehydration*>(&condition);
    if (d != nullptr)
    {
      GetDehydration().Copy(*d);
      GetDehydration().Activate();
      return true;
    }

    const SEImpairedAlveolarExchange* iae = dynamic_cast<const SEImpairedAlveolarExchange*>(&condition);
    if (iae != nullptr)
    {
      GetImpairedAlveolarExchange().Copy(*iae);
      GetImpairedAlveolarExchange().Activate();
      return true;
    }

    const SEPneumonia* lp = dynamic_cast<const SEPneumonia*>(&condition);
    if (lp != nullptr)
    {
      GetPneumonia().Copy(*lp);
      GetPneumonia().Activate();
      return true;
    }

    const SEPulmonaryFibrosis* pf = dynamic_cast<const SEPulmonaryFibrosis*>(&condition);
    if (pf != nullptr)
    {
      GetPulmonaryFibrosis().Copy(*pf);
      GetPulmonaryFibrosis().Activate();
      return true;
    }

    const SEPulmonaryShunt* ps = dynamic_cast<const SEPulmonaryShunt*>(&condition);
    if (ps != nullptr)
    {
      GetPulmonaryShunt().Copy(*ps);
      GetPulmonaryShunt().Activate();
      return true;
    }

    const SESepsis* s = dynamic_cast<const SESepsis*>(&condition);
    if (s != nullptr)
    {
      GetSepsis().Copy(*s);
      GetSepsis().Activate();
      return true;
    }
  }

  if (dynamic_cast<const SEEnvironmentCondition*>(&condition) != nullptr)
  {
    const SEInitialEnvironmentalConditions* ie = dynamic_cast<const SEInitialEnvironmentalConditions*>(&condition);
    if (ie != nullptr)
    {
      GetInitialEnvironmentalConditions().Copy(*ie, subMgr);
      GetInitialEnvironmentalConditions().Activate();
      return true;
    }
  }

  Error(condition.GetName()+" is an unsupported Condition");
  return false;
}

bool SEConditionManager::HasAcuteRespiratoryDistressSyndrome() const
{
  return m_ARDS == nullptr ? false : m_ARDS->IsValid();
}
SEAcuteRespiratoryDistressSyndrome& SEConditionManager::GetAcuteRespiratoryDistressSyndrome()
{
  if (m_ARDS == nullptr)
    m_ARDS = new SEAcuteRespiratoryDistressSyndrome(GetLogger());
  return *m_ARDS;
}
const SEAcuteRespiratoryDistressSyndrome* SEConditionManager::GetAcuteRespiratoryDistressSyndrome() const
{
  return m_ARDS;
}

bool SEConditionManager::HasChronicAnemia() const
{
  return m_Anemia == nullptr ? false : m_Anemia->IsValid();
}
SEChronicAnemia& SEConditionManager::GetChronicAnemia()
{
  if (m_Anemia == nullptr)
    m_Anemia = new SEChronicAnemia(GetLogger());
  return *m_Anemia;
}
const SEChronicAnemia* SEConditionManager::GetChronicAnemia() const
{
  return m_Anemia;
}

bool SEConditionManager::HasChronicObstructivePulmonaryDisease() const
{
  return m_COPD == nullptr ? false : m_COPD->IsValid();
}
SEChronicObstructivePulmonaryDisease& SEConditionManager::GetChronicObstructivePulmonaryDisease()
{
  if (m_COPD == nullptr)
    m_COPD = new SEChronicObstructivePulmonaryDisease(GetLogger());
  return *m_COPD;
}
const SEChronicObstructivePulmonaryDisease* SEConditionManager::GetChronicObstructivePulmonaryDisease() const
{
  return m_COPD;
}

bool SEConditionManager::HasChronicHeartFailure() const
{
  return m_ChronicVentricularSystolicDysfunction != nullptr;
}
bool SEConditionManager::HasChronicVentricularSystolicDysfunction() const
{
  return m_ChronicVentricularSystolicDysfunction == nullptr ? false : m_ChronicVentricularSystolicDysfunction->IsValid();
}
SEChronicVentricularSystolicDysfunction& SEConditionManager::GetChronicVentricularSystolicDysfunction()
{
  if (m_ChronicVentricularSystolicDysfunction == nullptr)
    m_ChronicVentricularSystolicDysfunction = new SEChronicVentricularSystolicDysfunction(GetLogger());
  return *m_ChronicVentricularSystolicDysfunction;
}
const SEChronicVentricularSystolicDysfunction* SEConditionManager::GetChronicVentricularSystolicDysfunction() const
{
  return m_ChronicVentricularSystolicDysfunction;
}

bool SEConditionManager::HasChronicPericardialEffusion() const
{
  return m_PericardialEffusion == nullptr ? false : m_PericardialEffusion->IsValid();
}
SEChronicPericardialEffusion& SEConditionManager::GetChronicPericardialEffusion()
{
  if (m_PericardialEffusion == nullptr)
    m_PericardialEffusion = new SEChronicPericardialEffusion(GetLogger());
  return *m_PericardialEffusion;
}
const SEChronicPericardialEffusion* SEConditionManager::GetChronicPericardialEffusion() const
{
  return m_PericardialEffusion;
}

bool SEConditionManager::HasChronicRenalStenosis() const
{
  return m_RenalStenosis == nullptr ? false : m_RenalStenosis->IsValid();
}
SEChronicRenalStenosis& SEConditionManager::GetChronicRenalStenosis()
{
  if (m_RenalStenosis == nullptr)
    m_RenalStenosis = new SEChronicRenalStenosis(GetLogger());
  return *m_RenalStenosis;
}
const SEChronicRenalStenosis* SEConditionManager::GetChronicRenalStenosis() const
{
  return m_RenalStenosis;
}

bool SEConditionManager::HasConsumeMeal() const
{
  return m_ConsumeMeal == nullptr ? false : m_ConsumeMeal->IsValid();
}
SEConsumeMeal& SEConditionManager::GetConsumeMeal()
{
  if (m_ConsumeMeal == nullptr)
    m_ConsumeMeal = new SEConsumeMeal(GetLogger());
  return *m_ConsumeMeal;
}
const SEConsumeMeal* SEConditionManager::GetConsumeMeal() const
{
  return m_ConsumeMeal;
}

bool SEConditionManager::HasDehydration() const
{
  return m_Dehydration == nullptr ? false : m_Dehydration->IsValid();
}
SEDehydration& SEConditionManager::GetDehydration()
{
  if (m_Dehydration == nullptr)
    m_Dehydration = new SEDehydration(GetLogger());
  return *m_Dehydration;
}
const SEDehydration* SEConditionManager::GetDehydration() const
{
  return m_Dehydration;
}

bool SEConditionManager::HasImpairedAlveolarExchange() const
{
  return m_ImpairedAlveolarExchange == nullptr ? false : m_ImpairedAlveolarExchange->IsValid();
}
SEImpairedAlveolarExchange& SEConditionManager::GetImpairedAlveolarExchange()
{
  if (m_ImpairedAlveolarExchange == nullptr)
    m_ImpairedAlveolarExchange = new SEImpairedAlveolarExchange(GetLogger());
  return *m_ImpairedAlveolarExchange;
}
const SEImpairedAlveolarExchange* SEConditionManager::GetImpairedAlveolarExchange() const
{
  return m_ImpairedAlveolarExchange;
}

bool SEConditionManager::HasPneumonia() const
{
  return m_Pneumonia == nullptr ? false : m_Pneumonia->IsValid();
}
SEPneumonia& SEConditionManager::GetPneumonia()
{
  if (m_Pneumonia == nullptr)
    m_Pneumonia = new SEPneumonia(GetLogger());
  return *m_Pneumonia;
}
const SEPneumonia* SEConditionManager::GetPneumonia() const
{
  return m_Pneumonia;
}

bool SEConditionManager::HasPulmonaryFibrosis() const
{
  return m_PulmonaryFibrosis == nullptr ? false : m_PulmonaryFibrosis->IsValid();
}
SEPulmonaryFibrosis& SEConditionManager::GetPulmonaryFibrosis()
{
  if (m_PulmonaryFibrosis == nullptr)
    m_PulmonaryFibrosis = new SEPulmonaryFibrosis(GetLogger());
  return *m_PulmonaryFibrosis;
}
const SEPulmonaryFibrosis* SEConditionManager::GetPulmonaryFibrosis() const
{
  return m_PulmonaryFibrosis;
}

bool SEConditionManager::HasPulmonaryShunt() const
{
  return m_PulmonaryShunt == nullptr ? false : m_PulmonaryShunt->IsValid();
}
SEPulmonaryShunt& SEConditionManager::GetPulmonaryShunt()
{
  if (m_PulmonaryShunt == nullptr)
    m_PulmonaryShunt = new SEPulmonaryShunt(GetLogger());
  return *m_PulmonaryShunt;
}
const SEPulmonaryShunt* SEConditionManager::GetPulmonaryShunt() const
{
  return m_PulmonaryShunt;
}

bool SEConditionManager::HasSepsis() const
{
  return m_Sepsis == nullptr ? false : m_Sepsis->IsValid();
}
SESepsis& SEConditionManager::GetSepsis()
{
  if (m_Sepsis == nullptr)
    m_Sepsis = new SESepsis(GetLogger());
  return *m_Sepsis;
}
const SESepsis* SEConditionManager::GetSepsis() const
{
  return m_Sepsis;
}

bool SEConditionManager::HasInitialEnvironmentalConditions() const
{
  return m_InitialEnvironmentalConditions == nullptr ? false : m_InitialEnvironmentalConditions->IsValid();
}
SEInitialEnvironmentalConditions& SEConditionManager::GetInitialEnvironmentalConditions()
{
  if (m_InitialEnvironmentalConditions == nullptr)
    m_InitialEnvironmentalConditions = new SEInitialEnvironmentalConditions(GetLogger());
  return *m_InitialEnvironmentalConditions;
}
const SEInitialEnvironmentalConditions* SEConditionManager::GetInitialEnvironmentalConditions() const
{
  return m_InitialEnvironmentalConditions;
}

void SEConditionManager::GetAllConditions(std::vector<const SECondition*>& conditions) const
{
  if (HasAcuteRespiratoryDistressSyndrome())
    conditions.push_back(GetAcuteRespiratoryDistressSyndrome());
  if (HasChronicAnemia())
    conditions.push_back(GetChronicAnemia());
  if (HasChronicObstructivePulmonaryDisease())
    conditions.push_back(GetChronicObstructivePulmonaryDisease());
  if (HasChronicVentricularSystolicDysfunction())
    conditions.push_back(GetChronicVentricularSystolicDysfunction());
  if (HasChronicPericardialEffusion())
    conditions.push_back(GetChronicPericardialEffusion());
  if (HasChronicRenalStenosis())
    conditions.push_back(GetChronicRenalStenosis());
  if (HasConsumeMeal())
    conditions.push_back(GetConsumeMeal());
  if (HasDehydration())
    conditions.push_back(GetDehydration());
  if (HasImpairedAlveolarExchange())
    conditions.push_back(GetImpairedAlveolarExchange());
  if (HasPneumonia())
    conditions.push_back(GetPneumonia());
  if (HasPulmonaryFibrosis())
    conditions.push_back(GetPulmonaryFibrosis());
  if (HasPulmonaryShunt())
    conditions.push_back(GetPulmonaryShunt());
  if (HasSepsis())
    conditions.push_back(GetSepsis());

  if (HasInitialEnvironmentalConditions())
    conditions.push_back(GetInitialEnvironmentalConditions());
}

bool SEConditionManager::IsEmpty() const
{
  if (HasAcuteRespiratoryDistressSyndrome())
    return false;
  if (HasChronicAnemia())
    return false;
  if (HasChronicObstructivePulmonaryDisease())
    return false;
  if (HasChronicVentricularSystolicDysfunction())
    return false;
  if (HasChronicPericardialEffusion())
    return false;
  if (HasChronicRenalStenosis())
    return false;
  if (HasConsumeMeal())
    return false;
  if (HasDehydration())
    return false;
  if (HasImpairedAlveolarExchange())
    return false;
  if (HasPneumonia())
    return false;
  if (HasPulmonaryFibrosis())
    return false;
  if (HasPulmonaryShunt())
    return false;
  if (HasSepsis())
    return false;

  if (HasInitialEnvironmentalConditions())
    return false;
  return true;
}