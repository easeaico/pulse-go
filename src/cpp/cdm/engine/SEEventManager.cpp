/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/engine/SEEventManager.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/io/protobuf/PBEvents.h"

SEEventManager::SEEventManager(Logger* logger) : Loggable(logger)
{
  Clear();
}

SEEventManager::~SEEventManager()
{

}

void SEEventManager::Clear()
{
  m_EventHandler = nullptr;
  m_EventState.clear();
  m_EventDuration_s.clear();
}

void SEEventManager::SetEvent(eEvent type, bool active, const SEScalarTime& time)
{
  bool b = false;// Default is off
  if (m_EventState.find(type) != m_EventState.end())
    b = m_EventState[type];
  if (b == active)
    return;//No Change
  if (active != b)
  {
    m_ss.str("");
    m_ss << "[Event " << eEvent_Name(type) <<" " << active << "] ";
    if (active)
    {
      switch (type)
      {
      case eEvent::Antidiuresis:
        m_ss << " Patient has Antidiuresis";
        break;
      case eEvent::Bradycardia:
        m_ss << " Patient has Bradycardia";
        break;
      case eEvent::Bradypnea:
        m_ss << " Patient has Bradypnea";
        break;
      case eEvent::BrainOxygenDeficit:
        m_ss << " Oxygen tension in the brain is dangerously low";
        break;
      case eEvent::CardiacArrest:
        m_ss << " Patient has Cardiac Arrest";
        break;
      case eEvent::CardiogenicShock:
        m_ss << " Patient has Cardiogenic Shock";
        break;
      case eEvent::CardiovascularCollapse:
        m_ss << " Patient has low blood pressure and the vasculature has collapsed";
        break;
      case eEvent::CriticalBrainOxygenDeficit:
        m_ss << " Oxygen tension in the brain is critically low";
        break;
      case eEvent::Diuresis:
        m_ss << " Patient has entered Diuresis";
        break;
      case eEvent::Fasciculation:
        m_ss << "Patient has Fasciculation";
        break;
      case eEvent::Fatigue:
        m_ss << "Patient has Fatigue";
        break;
      case eEvent::FunctionalIncontinence:
        m_ss << " Patient has involuntarily emptied their bladder";
        break;
      case eEvent::Hypercapnia:
        m_ss << " Patient has Hypercapnia";
        break;
      case eEvent::Hyperglycemia:
        m_ss << " Patient has Hyperglycemia";
        break;
      case eEvent::Hypernatremia:
        m_ss << " Patient has Hypernatremia";
        break;
      case eEvent::Hyperthermia:
        m_ss << " Patient is Hyperthermic";
        break;
      case eEvent::Hypoglycemia:
        m_ss << " Patient has Hypoglycemia";
        break;
      case eEvent::Hyponatremia:
        m_ss << " Patient has Hyponatremia";
        break;
      case eEvent::Hypothermia:
        m_ss << " Patient is Hypothermic";
        break;
      case eEvent::Hypoxia:
        m_ss << " Patient has Hypoxia";
        break;
      case eEvent::HypovolemicShock:
        m_ss << " Patient is in Hypovolemic Shock";
        break;
      case eEvent::IntracranialHypertension:
        m_ss << " Patient has Intracranial Hypertension";
        break;
      case eEvent::IntracranialHypotension:
        m_ss << " Patient has Intracranial Hypotension";
        break;
      case eEvent::IrreversibleState:
        m_ss << " Patient has entered irreversible state";
        break;
      case eEvent::Ketoacidosis:
        m_ss << " Patient has Ketoacidosis";
        break;
      case eEvent::LacticAcidosis:
        m_ss << " Patient has LacticAcidosis";
        break;
      case eEvent::MassiveHemothorax:
        m_ss << " Patient has Massive Hemothorax";
        break;
      case eEvent::MaximumPulmonaryVentilationRate:
        m_ss << " Patient's Respiratory Driver has exceeded the maximum target pulmonary ventilation rate, setting value to the maximum allowable rate";
        break;
      case eEvent::MildDehydration:
        m_ss << " The patient is in a state of mild dehydration";
        break;
      case eEvent::MediumHemothorax:
        m_ss << " Patient has Medium Hemothorax";
        break;
      case eEvent::MetabolicAcidosis:
        m_ss << " The patient is in a state of metabolic acidosis";
        break;
      case eEvent::MetabolicAlkalosis:
        m_ss << " The patient is in a state of metabolic alkalosis";
        break;
      case eEvent::MinimalHemothorax:
        m_ss << " Patient has Minimal Hemothorax";
        break;
      case eEvent::ModerateDehydration:
        m_ss << " The patient is in a state of moderate dehydration";
        break;
      case eEvent::ModerateHyperoxemia:
        m_ss << " The patient is in a state of moderate hyperoxemia";
        break;
      case eEvent::ModerateHypocapnia:
        m_ss << " The patient is in a state of moderate hypocapnia";
        break;
      case eEvent::MyocardiumOxygenDeficit:
        m_ss << " The patient's heart is not receiving enough oxygen";
        break;
      case eEvent::Natriuresis:
        m_ss << " Patient has Natriuresis";
        break;
      case eEvent::NutritionDepleted:
        m_ss << " Patient has depleted all nutrition in body";
        break;
      case eEvent::RenalHypoperfusion:
        m_ss << " Patient has Renal Hypoperfusion";
        break;
      case eEvent::RespiratoryAcidosis:
        m_ss << " Patient has Respiratory Acidosis";
        break;
      case eEvent::RespiratoryAlkalosis:
        m_ss << " Patient has Respiratory Alkalosis";
        break;
      case eEvent::SevereDehydration:
        m_ss << " The patient is in a state of severe dehydration";
        break;
      case eEvent::SevereHyperoxemia:
        m_ss << " The patient is in a state of moderate hyperoxemia";
        break;
      case eEvent::SevereHypocapnia:
        m_ss << " The patient is in a state of severe hypocapnia";
        break;
      case eEvent::Stabilizing:
        m_ss << " Engine is stabilizing";
        break;
      case eEvent::Tachycardia:
        m_ss << " Patient has Tachycardia";
        break;
      case eEvent::Tachypnea:
        m_ss << " Patient has Tachypnea";
        break;
      case eEvent::StartOfCardiacCycle:
      case eEvent::StartOfExhale:
      case eEvent::StartOfInhale:
        m_ss.str("");// make m_ss empty and nothing will be logged, this event does not need to get logged each activation
        break;
      // Equipment
      case eEvent::AnesthesiaMachineOxygenBottleOneExhausted:
        m_ss << "Anesthesia Machine Oxygen Bottle 1 has been exhausted";
        break;
      case eEvent::AnesthesiaMachineOxygenBottleTwoExhausted:
        m_ss << "Anesthesia Machine Oxygen Bottle 2 has been exhausted";
        break;
      case eEvent::AnesthesiaMachineReliefValveActive:
        m_ss << "Anesthesia Machine Relief valve active - pressure exceeded";
        break;
      case eEvent::MechanicalVentilatorReliefValveActive:
        m_ss << "Mechanical Ventilator Relief valve active - pressure exceeded";
        break;
      case eEvent::SupplementalOxygenBottleExhausted:
        m_ss << "Supplemental oxygen bottle is exhausted";
        break;
      case eEvent::NonRebreatherMaskOxygenBagEmpty:
        m_ss << "Non-rebreather mask oxygen bag is empty";
        break;
      default:
        m_ss << " Engine has entered state : " << eEvent_Name(type);
      }
    }
    else
    {
      switch (type)
      {
      case eEvent::Antidiuresis:
        m_ss << " Patient no longer is in Antidiuresis";
        break;
      case eEvent::Bradycardia:
        m_ss << " Patient no longer has Bradycardia";
        break;
      case eEvent::Bradypnea:
        m_ss << " Patient no longer has Bradypnea";
        break;
      case eEvent::BrainOxygenDeficit:
        m_ss << " Oxygen tension in the brain has increased above the danger threshold";
        break;
      case eEvent::CardiacArrest:
        m_ss << " Patient no longer has Cardiac Arrest";
        break;
      case eEvent::CardiogenicShock:
        m_ss << " Patient no longer has Cardiogenic Shock";
        break;
      case eEvent::CardiovascularCollapse:
        m_ss << " Patient is no longer experiencing cardiovasculature collapse";
        break;
      case eEvent::CriticalBrainOxygenDeficit:
        m_ss << " Oxygen tension in the brain has increased above the critical threshold";
        break;
      case eEvent::Diuresis:
        m_ss << " Patient no longer has Diuresis";
        break;
      case eEvent::Fasciculation:
        m_ss << "Patient no longer has fasciculations";
        break;
      case eEvent::Fatigue:
        m_ss << "Patient is no longer fatigued";
        break;
      case eEvent::FunctionalIncontinence:
        m_ss << " Patient has an empty bladder";
        break;
      case eEvent::Hypercapnia:
        m_ss << " Patient no longer has Hypercapnia";
        break;
      case eEvent::Hyperglycemia:
        m_ss << " Patient no longer has Hyperglycemia";
        break;
      case eEvent::Hypernatremia:
        m_ss << " Patient no longer has Hypernatremia";
        break;
      case eEvent::Hyperthermia:
        m_ss << " Patient is no longer has Hyperthermic";
        break;
      case eEvent::Hypoglycemia:
        m_ss << " Patient no longer has Hypoglycemia";
        break;
      case eEvent::Hyponatremia:
        m_ss << " Patient no longer has Hyponatremia";
        break;
      case eEvent::Hypothermia:
        m_ss << " Patient is no longer has Hypothermic";
        break;
      case eEvent::Hypoxia:
        m_ss << " Patient no longer has Hypoxia";
        break;
      case eEvent::HypovolemicShock:
        m_ss << " Patient is no longer in Hypovolemic Shock";
        break;
      case eEvent::IntracranialHypertension:
        m_ss << " Patient no longer has Intracranial Hypertension";
        break;
      case eEvent::IntracranialHypotension:
        m_ss << " Patient no longer has Intracranial Hypotension";
        break;
      case eEvent::IrreversibleState:
        m_ss << " Patient no longer is in irreversible state?!";
        break;
      case eEvent::Ketoacidosis:
        m_ss << " Patient no longer has Ketoacidosis";
        break;
      case eEvent::LacticAcidosis:
        m_ss << " Patient no longer has LacticAcidosis";
        break;
      case eEvent::MassiveHemothorax:
        m_ss << " Patient no longer has Massive Hemothorax";
        break;
      case eEvent::MaximumPulmonaryVentilationRate:
        m_ss << " Patient's Respiratory Driver is no longer exceeding the maximum target pulmonary ventilation rate";
        break;
      case eEvent::MediumHemothorax:
        m_ss << " Patient no longer has Medium Hemothorax";
        break;
      case eEvent::MetabolicAcidosis:
        m_ss << " The patient is no longer in a state of metabolic acidosis";
        break;
      case eEvent::MetabolicAlkalosis:
        m_ss << " The patient is no longer in a state of metabolic alkalosis";
        break;
      case eEvent::MildDehydration:
        m_ss << " The patient no longer has mild dehydration";
        break;
      case eEvent::MinimalHemothorax:
        m_ss << " Patient no longer has Minimal Hemothorax";
        break;
      case eEvent::ModerateDehydration:
        m_ss << " The patient no longer has moderate dehydration";
        break;
      case eEvent::ModerateHyperoxemia:
        m_ss << " The patient is no longer in a state of moderate hyperoxemia";
        break;
      case eEvent::ModerateHypocapnia:
        m_ss << " The patient is no longer in a state of moderate hypocapnia";
        break;
      case eEvent::MyocardiumOxygenDeficit:
        m_ss << " Patient no longer has a Myocardium Oxygen Deficit";
        break;
      case eEvent::Natriuresis:
        m_ss << " Patient no longer has Natriuresis";
        break;
      case eEvent::NutritionDepleted:
        m_ss << " Patient has nutrition in body";
        break;
      case eEvent::RenalHypoperfusion:
        m_ss << " Patient no longer has Renal Hypoperfusion";
        break;
      case eEvent::RespiratoryAcidosis:
        m_ss << " Patient no longer has Respiratory Acidosis";
        break;
      case eEvent::RespiratoryAlkalosis:
        m_ss << " Patient no longer has Respiratory Alkalosis";
        break;
      case eEvent::SevereDehydration:
        m_ss << " The patient no longer has severe dehydration";
        break;
      case eEvent::SevereHyperoxemia:
        m_ss << " The patient is no longer in a state of severe hyperoxemia";
        break;
      case eEvent::SevereHypocapnia:
        m_ss << " The patient is no longer in a state of severe hypocapnia";
        break;
      case eEvent::Stabilizing:
        m_ss << " Engine completed stabilizing";
        break;
      case eEvent::Tachycardia:
        m_ss << " Patient no longer has Tachycardia";
        break;
      case eEvent::Tachypnea:
        m_ss << " Patient no longer has Tachypnea";
        break;
      case eEvent::StartOfCardiacCycle:
      case eEvent::StartOfExhale:
      case eEvent::StartOfInhale:
        m_ss.str("");// make m_ss empty and nothing will be logged, this event does not need to get logged each activation
        break;
      // Equipment
      case eEvent::AnesthesiaMachineOxygenBottleOneExhausted:
        m_ss << "Anesthesia Machine Oxygen Bottle 1 has been replenished";
        break;
      case eEvent::AnesthesiaMachineOxygenBottleTwoExhausted:
        m_ss << "Anesthesia Machine Oxygen Bottle 2 has been replenished";
        break;
      case eEvent::AnesthesiaMachineReliefValveActive:
        m_ss << "Anesthesia Machine Relief valve inactive - pressure below setting";
        break;
      case eEvent::MechanicalVentilatorReliefValveActive:
        m_ss << "Mechanical Ventilator Relief valve inactive - pressure below setting";
        break;
      case eEvent::SupplementalOxygenBottleExhausted:
        m_ss << "Supplemental oxygen bottle is no longer exhausted";
        break;
      case eEvent::NonRebreatherMaskOxygenBagEmpty:
        m_ss << "Non-rebreather mask oxygen bag is now longer empty";
        break;
      default:
        m_ss << " Engine has exited state : " << eEvent_Name(type);
      }
    }
    if (!m_ss.str().empty())
      Info(m_ss);
  }
  m_EventState[type] = active;
  m_EventDuration_s[type] = 0;
  if (m_EventHandler != nullptr)
    m_EventHandler->HandleEvent(type, active, &time);
}

bool SEEventManager::IsEventActive(eEvent e) const
{
  auto i = m_EventState.find(e);
  if (i == m_EventState.end())
    return false;
  return i->second;
}

double SEEventManager::GetEventDuration(eEvent e, const TimeUnit& unit) const
{
  auto i = m_EventDuration_s.find(e);
  if (i == m_EventDuration_s.end())
    return 0;
  return Convert(i->second, TimeUnit::s, unit);
}

void SEEventManager::OverrideActiveState(eEvent e, const SEScalarTime& duration)
{
  m_EventState[e] = true;
  m_EventDuration_s[e] = duration.GetValue(TimeUnit::s);
}

void SEEventManager::UpdateEvents(const SEScalarTime& timeStep)
{
  for (auto& itr : m_EventDuration_s)
    itr.second += timeStep.GetValue(TimeUnit::s);
}

bool SEEventManager::GetActiveEvents(std::vector<const SEActiveEvent*>& active) const
{
  active.clear();
  for (auto itr : m_EventState)
  {
    if (!itr.second)
      continue;
    SEActiveEvent* ae = new SEActiveEvent(itr.first, m_EventDuration_s.at(itr.first), TimeUnit::s);
    active.push_back(ae);
  }
  return !active.empty();
}

void SEEventManager::ForwardEvents(SEEventHandler* handler) const
{
  m_EventHandler = handler;
}

SEEventChange::SEEventChange(eEvent e, bool active, const SEScalarTime* simTime)
{
  m_Event = e;
  m_Active = active;
  if (simTime != nullptr)
    m_SimTime.Set(*simTime);
  else
    m_SimTime.Invalidate();
}
bool SEEventChange::SerializeToString(std::vector<const SEEventChange*>& changes, std::string& output, eSerializationFormat m, Logger* logger)
{
  return PBEvents::SerializeToString(changes, output, m, logger);
}
bool SEEventChange::SerializeFromString(const std::string& src, std::vector<const SEEventChange*>& changes, eSerializationFormat m, Logger* logger)
{
  return PBEvents::SerializeFromString(src, changes, m, logger);
}

SEActiveEvent::SEActiveEvent(eEvent e, const SEScalarTime& duration)
{
  m_Event = e;
  m_Duration.Set(duration);
}
SEActiveEvent::SEActiveEvent(eEvent e, double duration, const TimeUnit& unit)
{
  m_Event = e;
  m_Duration.SetValue(duration,unit);
}
bool SEActiveEvent::SerializeToString(std::vector<const SEActiveEvent*>& active, std::string& output, eSerializationFormat m, Logger* logger)
{
  return PBEvents::SerializeToString(active, output, m, logger);
}
bool SEActiveEvent::SerializeFromString(const std::string& src, std::vector<const SEActiveEvent*>& active, eSerializationFormat m, Logger* logger)
{
  return PBEvents::SerializeFromString(src, active, m, logger);
}
