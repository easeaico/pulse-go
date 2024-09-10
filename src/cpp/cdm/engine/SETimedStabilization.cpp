/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/engine/SETimedStabilization.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/engine/SECondition.h"
#include "cdm/engine/SEConditionManager.h"
#include "cdm/utils/TimingProfile.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/io/protobuf/PBEngine.h"

bool SETimedStabilization::Stabilize(Controller& engine, const std::string& criteria)
{
  if (!HasConvergenceCriteria(criteria))
    return false;//No stabilization time for requested
  return Stabilize(engine, GetConvergenceCriteria(criteria));
}
bool SETimedStabilization::StabilizeConditions(Controller& engine, const SEConditionManager& conditions)
{
  if (conditions.IsEmpty())
    return true;
  conditions.GetAllConditions(m_Conditions);
  double cTime_s;
  double maxTime_s = 0;
  for (const SECondition* c : m_Conditions)
  {
    if(!HasConvergenceCriteria(c->GetName()))
    {
      Error("Engine does not support Condition "+c->GetName());
      return false;
    }
    const SEScalarTime& time = GetConvergenceCriteria(c->GetName());
    cTime_s= time.GetValue(TimeUnit::s);
    if (cTime_s > maxTime_s)
      maxTime_s = cTime_s;
  }
  SEScalarTime time;
  time.SetValue(maxTime_s, TimeUnit::s);
  return Stabilize(engine, time);
}
bool SETimedStabilization::Stabilize(Controller& engine, const SEScalarTime& time)
{  
  double sTime_s = time.GetValue(TimeUnit::s);
  if (sTime_s == 0)
    return true; //No stabilization time requested

  m_Cancelled = false;
  std::stringstream ss;
  TimingProfile profiler;
  Info("Initializing Engine : 0%");
  if (m_LogProgress)
  {
    profiler.Start("Total");
    profiler.Start("Status");
  }
  // Execute System initialization time
  SEEngineTracker* tracker = engine.GetEngineTracker();
  eSwitch track = m_TrackingStabilization;
  if (track==eSwitch::On && tracker == nullptr)
  {
    track = eSwitch::Off;
    Warning("PhysiologyEngineTrack not provided by engine, not tracking data to file");
  }

  ss.precision(3);
  double statusTime_s = 0;// Current time of this status cycle
  double statusStep_s = 50;//How long did it take to simulate this much time
  double dT_s = engine.GetTimeStep(TimeUnit::s);
  double currentTime_s=0;
  int count = (int)(sTime_s / dT_s);
  int ProgressStep = (int)(count*.1);
  int Progress = ProgressStep;
  if (track == eSwitch::On)
    tracker->SetupRequests();
  for (int i = 0; i <= count; i++)
  {
    if (m_Cancelled)
      break;
    // Instead of calling AdvanceModelTime
    // We should have a method called AdvanceToRestingState
    // and it will advance time, AND check to see if it is at a Resting state or not
    // if it is we can break our loop. This will allow us to record our stabilization data
    engine.AdvanceTime();
    currentTime_s = engine.GetSimulationTime(TimeUnit::s);

    if (track == eSwitch::On)
      tracker->TrackData(currentTime_s);
    if (m_LogProgress)
    {
      statusTime_s += dT_s;
      if (statusTime_s>statusStep_s)
      {
        statusTime_s = 0;
        ss << "Current Time is " << currentTime_s << "s, it took "
          << profiler.GetElapsedTime_s("Status") << "s to simulate the past "
          << statusStep_s << "s" << std::flush;
        profiler.Reset("Status");
        Info(ss);
      }
      if (i>Progress)
      {
        Progress += ProgressStep;
        ss << "Initializing Engine : " << (double)i / count * 100 << "% Complete";
        Info(ss);
      }
    }
  }
  if (m_LogProgress)
  {
    ss << "It took " << profiler.GetElapsedTime_s("Total") << "s to get engine to stable state";
    Info(ss);
  }
  // Save off how long it took us to stabilize
  GetStabilizationDuration().SetValue(currentTime_s, TimeUnit::s);
  return true;
}

SETimedStabilization::SETimedStabilization(Logger *logger) : SEEngineStabilization(logger)
{
  GetStabilizationDuration().SetValue(0, TimeUnit::s);
}

SETimedStabilization::~SETimedStabilization()
{
  Clear();
}

void SETimedStabilization::Clear()
{
  SEEngineStabilization::Clear();
  DELETE_MAP_SECOND(m_ConvergenceCriteria);
}

bool SETimedStabilization::SerializeToString(std::string& output, eSerializationFormat m) const
{
  return PBEngine::SerializeToString(*this, output, m);
}
bool SETimedStabilization::SerializeToFile(const std::string& filename) const
{
  return PBEngine::SerializeToFile(*this, filename);
}
bool SETimedStabilization::SerializeFromString(const std::string& src, eSerializationFormat m)
{
  return PBEngine::SerializeFromString(src, *this, m);
}
bool SETimedStabilization::SerializeFromFile(const std::string& filename)
{
  return PBEngine::SerializeFromFile(filename, *this);
}

bool SETimedStabilization::HasConvergenceCriteria(const std::string& name) const
{
  return m_ConvergenceCriteria.find(name) != m_ConvergenceCriteria.end();
}
void SETimedStabilization::RemoveConvergenceCriteria(const std::string& name)
{
  for (auto itr : m_ConvergenceCriteria)
  {
    if (itr.first == name)
    {
      SAFE_DELETE(itr.second);
      m_ConvergenceCriteria.erase(name);
      return;
    }
  }
}
SEScalarTime& SETimedStabilization::GetConvergenceCriteria(const std::string& name)
{
  for (auto itr : m_ConvergenceCriteria)
  {
    if (itr.first == name)
      return (*itr.second);
  }
  SEScalarTime* time = new SEScalarTime();
  m_ConvergenceCriteria[name]=time;
  return *time;
}
const SEScalarTime* SETimedStabilization::GetConvergenceCriteria(const std::string& name) const
{
  for (auto itr : m_ConvergenceCriteria)
  {
    if (itr.first == name)
      return itr.second;
  }
  return nullptr;
}

const std::map<std::string,SEScalarTime*>& SETimedStabilization::GetConvergenceCriterias() const
{
  return m_ConvergenceCriteria;
}
