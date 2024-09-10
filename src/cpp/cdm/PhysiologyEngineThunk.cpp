/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/PhysiologyEngineThunk.h"

#include "cdm/patient/SEPatient.h"
#include "cdm/patient/assessments/SEArterialBloodGasTest.h"
#include "cdm/patient/assessments/SECompleteBloodCount.h"
#include "cdm/patient/assessments/SEComprehensiveMetabolicPanel.h"
#include "cdm/patient/assessments/SEUrinalysis.h"
#include "cdm/engine/SEAction.h"
#include "cdm/engine/SEDataRequest.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEActionManager.h"
#include "cdm/engine/SEConditionManager.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/engine/SEEngineConfiguration.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/substance/SESubstance.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/properties/SEScalarElectricPotential.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarPressure.h"
#include "cdm/properties/SEScalarTemperature.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/utils/DataTrack.h"
#include "cdm/utils/Logger.h"

PhysiologyEngineThunk::PhysiologyEngineThunk(const std::string& dataDir) : SEEventHandler()
{
  m_dataDir = dataDir;
}
PhysiologyEngineThunk::~PhysiologyEngineThunk()
{
  delete[] m_requestedData;
}

void PhysiologyEngineThunk::Clear()
{
  m_engine->Clear();
}

bool PhysiologyEngineThunk::SerializeFromFile(std::string const& filename, std::string const& data_requests, eSerializationFormat data_requests_format)
{
  if (!m_engine->SerializeFromFile(filename))
    return false;
  m_engine->GetEventManager().ForwardEvents(this);

  // Load up the data requests
  if (!data_requests.empty())
  {
    if (!m_engine->GetEngineTracker()->GetDataRequestManager().SerializeFromString(data_requests, data_requests_format))
    {
      m_engine->GetLogger()->Error("Unable to load data requests string");
      return false;
    }
  }
  else
  {
    SetupDefaultDataRequests();
    m_engine->GetLogger()->Info("No data requested, will return default vitals");
  }
  return SetupRequests();
}

bool PhysiologyEngineThunk::SerializeToFile(std::string const& filename)
{
  return m_engine->SerializeToFile(filename);
}


bool PhysiologyEngineThunk::SerializeFromString(std::string const& state, std::string const& data_requests, eSerializationFormat format)
{
  if (!m_engine->SerializeFromString(state, format))
    return false;
  m_engine->GetEventManager().ForwardEvents(this);

  // Load up the data requests
  if (!data_requests.empty())
  {
    if (!m_engine->GetEngineTracker()->GetDataRequestManager().SerializeFromString(data_requests, format))
    {
      m_engine->GetLogger()->Error("Unable to load data requests string");
      return false;
    }
  }
  else
    SetupDefaultDataRequests();
  return SetupRequests();
}


std::string PhysiologyEngineThunk::SerializeToString(eSerializationFormat format)
{
  std::string state;
  if (!m_engine->SerializeToString(state, format))
    state.clear();
  return state;
}

bool PhysiologyEngineThunk::InitializeEngine(std::string const& patient_configuration, std::string const& data_requests, eSerializationFormat format)
{
  const SESubstanceManager* subMgr;
  if (m_engine->GetSubstanceManager().GetSubstances().empty())
  {
    if (m_subMgr == nullptr)
    {
      // We need to use our subMgr here
      m_subMgr = new SESubstanceManager(m_engine->GetLogger());
      m_subMgr->LoadSubstanceDirectory(m_dataDir);
    }
    subMgr = m_subMgr;
  }
  else
    subMgr = &m_engine->GetSubstanceManager();

  SEPatientConfiguration pc(m_engine->GetLogger());
  if (!pc.SerializeFromString(patient_configuration, format, *subMgr))
  {
    m_engine->GetLogger()->Error("Unable to load patient configuration string");
    return false;
  }

  // Ok, crank 'er up!
  if (!m_engine->InitializeEngine(pc))
    return false;

  // Load up the data requests
  if (!data_requests.empty())
  {
    if (!m_engine->GetEngineTracker()->GetDataRequestManager().SerializeFromString(data_requests, format))
    {
      m_engine->GetLogger()->Error("Unable to load data request string");
      return false;
    }
  }
  else
    SetupDefaultDataRequests();
  if (!SetupRequests())
    return false;

  m_engine->GetEventManager().ForwardEvents(this);
  return true;
}

bool PhysiologyEngineThunk::SetupRequests()
{
  m_engine->GetEngineTracker()->SetupRequests();
  if (m_engine->GetEngineTracker()->GetDataTrack().NumTracks() !=
    m_engine->GetEngineTracker()->GetDataRequestManager().GetDataRequests().size())
  {
    m_engine->Error("Number of data requests does not match the number of tracked properties!");
    m_engine->Error("--Check to see if you have duplicates in your data request list");
    m_engine->Error("--Here is the order of the data items I am traking:");
    for (size_t i = 0; i < m_engine->GetEngineTracker()->GetDataTrack().NumTracks(); i++)
      m_engine->Error("--  " + m_engine->GetEngineTracker()->GetDataTrack().GetProbeName(i));
    m_engine->Error("--Here is what you requested:");
    for (SEDataRequest const* dr : m_engine->GetEngineTracker()->GetDataRequestManager().GetDataRequests())
      m_engine->Error("--  " + dr->GetHeaderName());
    m_engine->Error("I don't have the logic to figure out which tracked items are duplicated and where they go in the pulled data array");
    return false;
  }
  return true;
}

std::string PhysiologyEngineThunk::GetInitialPatient(eSerializationFormat format)
{
  std::string stream;
  m_engine->GetInitialPatient().SerializeToString(stream, format);
  return stream;
}

std::string PhysiologyEngineThunk::GetConditions(eSerializationFormat format)
{
  std::string stream;
  m_engine->GetConditionManager().SerializeToString(stream, format);
  return stream;
}

void PhysiologyEngineThunk::LogToConsole(bool b)
{
  m_engine->GetLogger()->LogToConsole(b);
}
void PhysiologyEngineThunk::KeepLogMessages(bool keep)
{
  m_keepLogMsgs = keep;
}
void PhysiologyEngineThunk::SetLogFilename(std::string const& logfile)
{
  m_engine->GetLogger()->SetLogFile(logfile);
}
std::string PhysiologyEngineThunk::PullLogMessages(eSerializationFormat format)
{
  std::string log_msgs;
  if (m_logMsgs.IsEmpty())
    return log_msgs;

  LogMessages::SerializeToString(m_logMsgs, log_msgs, format, m_engine->GetLogger());
  m_logMsgs.Clear();
  return log_msgs;
}

void PhysiologyEngineThunk::KeepEventChanges(bool keep)
{
  m_keepEventChanges = keep;
}
std::string PhysiologyEngineThunk::PullEvents(eSerializationFormat format)
{
  std::string events;
  if (m_events.empty())
    return events;
  SEEventChange::SerializeToString(m_events, events, format, m_engine->GetLogger());
  DELETE_VECTOR(m_events);
  return events;
}
std::string PhysiologyEngineThunk::PullActiveEvents(eSerializationFormat format)
{
  std::string active_events;
  if (!m_engine->GetEventManager().GetActiveEvents(m_activeEvents))
    active_events.clear();
  SEActiveEvent::SerializeToString(m_activeEvents, active_events, format, m_engine->GetLogger());
  DELETE_VECTOR(m_activeEvents);
  return active_events;
}

std::string PhysiologyEngineThunk::GetPatientAssessment(int type, eSerializationFormat format)
{
  std::string stream;
  switch (type)
  {
  case 0: // ABG
  {
    SEArterialBloodGasTest abg(m_engine->GetLogger());
    m_engine->GetPatientAssessment(abg);
    abg.SerializeToString(stream, format);
    break;
  }
  case 1: // CBC
  {
    SECompleteBloodCount cbc(m_engine->GetLogger());
    m_engine->GetPatientAssessment(cbc);
    cbc.SerializeToString(stream, format);
    break;
  }
  case 2: // CMP
  {
    SEComprehensiveMetabolicPanel cmp(m_engine->GetLogger());
    m_engine->GetPatientAssessment(cmp);
    cmp.SerializeToString(stream, format);
    break;
  }
  case 3:// U
  {
    SEUrinalysis u(m_engine->GetLogger());
    m_engine->GetPatientAssessment(u);
    u.SerializeToString(stream, format);
    break;
  }
  default:
    stream = "Unsupported assessment type";
  };

  return stream;
}

bool PhysiologyEngineThunk::ProcessActions(std::string const& actions, eSerializationFormat format)
{
  bool success = true;
  if (actions.empty())
    return success;

  try
  {
    std::vector<SEAction*> vActions;
    if (!SEActionManager::SerializeFromString(actions, vActions, format, m_engine->GetSubstanceManager()))
      return false;

    for (const SEAction* a : vActions)
    {
      if (!m_engine->ProcessAction(*a))
        success = false;
      delete a;
    }
  }
  catch (CommonDataModelException& ex)
  {
    success = false;
    m_engine->GetLogger()->Error(ex.what());
  }
  catch (std::exception& ex)
  {
    success = false;
    m_engine->GetLogger()->Error(ex.what());
  }
  catch (...)
  {
    success = false;
  }

  return success;
}
std::string PhysiologyEngineThunk::PullActiveActions(eSerializationFormat format)
{
  std::string stream;
  m_engine->GetActionManager().SerializeToString(stream, format);
  return stream;
}

double PhysiologyEngineThunk::GetTimeStep(std::string const& unit)
{
  const TimeUnit& time_unit = TimeUnit::GetCompoundUnit(unit);
  return m_engine->GetTimeStep(time_unit);
}
bool PhysiologyEngineThunk::AdvanceTimeStep()
{
  bool success = true;
  try
  {
    success = m_engine->AdvanceModelTime();
    if (m_engine->GetEngineTracker()->GetDataRequestManager().HasResultsFilename())
      m_engine->GetEngineTracker()->TrackData(m_engine->GetSimulationTime(TimeUnit::s));
  }
  catch (CommonDataModelException& ex)
  {
    m_engine->GetLogger()->Error(ex.what());
    success = false;
  }
  catch (std::exception& ex)
  {
    m_engine->GetLogger()->Error(ex.what());
    success = false;
  }
  catch (...)
  {
    m_engine->GetLogger()->Error("Caught Unknown Exception");
    success = false;
  }
  return success;
}

double* PhysiologyEngineThunk::PullDataPtr()
{
  double currentTime_s = m_engine->GetSimulationTime(TimeUnit::s);
  m_engine->GetEngineTracker()->PullData(currentTime_s);
  if (m_requestedData == nullptr)
  {
    // +1 for the sim time
    m_length = m_engine->GetEngineTracker()->GetDataTrack().NumTracks() + 1;
    m_requestedData = new double[m_length];
  }
  // Always put the sim time in index 0 as seconds
  m_requestedData[0] = currentTime_s;
  // Pull all data we requested and pack into our array for return to the caller
  for (size_t i = 1; i<m_length; i++)
    m_requestedData[i] = m_engine->GetEngineTracker()->GetDataTrack().GetProbe(i-1);

  return m_requestedData;
}

size_t PhysiologyEngineThunk::DataLength() const
{
  return m_length;
}
void PhysiologyEngineThunk::PullData(std::vector<double>& d)
{
  std::cout << "Trying to pull data" << std::endl;
  double* v = PullDataPtr();
  std::cout << "Pult " << m_length << " data" << std::endl;
  if (d.size() != m_length)
  {
    std::cout << "Reserving result" << std::endl;
    d.reserve(m_length);
    std::cout << "Ok" << std::endl;
  }
  for (size_t i = 0; i < m_length; i++)
  {

    std::cout << "Beep " << d[i] << std::endl;
    d[i] = v[i];

    std::cout << "Boop" << d[i] << std::endl;
  }

  std::cout << "Added data to the array? " << m_length << " data" << std::endl;
}

void PhysiologyEngineThunk::SetupDefaultDataRequests()
{// Default to vitals data
  //eng->GetLogger()->Info("No data requests provided, setting up default data requests");
  m_engine->GetEngineTracker()->GetDataRequestManager().CreateECGDataRequest("Lead3ElectricPotential", ElectricPotentialUnit::mV);
  m_engine->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("HeartRate", FrequencyUnit::Per_min);
  m_engine->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("ArterialPressure", PressureUnit::mmHg);
  m_engine->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("MeanArterialPressure", PressureUnit::mmHg);
  m_engine->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("SystolicArterialPressure", PressureUnit::mmHg);
  m_engine->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("DiastolicArterialPressure", PressureUnit::mmHg);
  m_engine->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("OxygenSaturation");
  m_engine->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("EndTidalCarbonDioxidePressure", PressureUnit::mmHg);
  m_engine->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("RespirationRate", FrequencyUnit::Per_min);
  m_engine->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("CoreTemperature", TemperatureUnit::C);
  m_engine->GetEngineTracker()->GetDataRequestManager().CreateGasCompartmentDataRequest("Carina", "CarbonDioxide", "PartialPressure", PressureUnit::mmHg);
  m_engine->GetEngineTracker()->GetDataRequestManager().CreatePhysiologyDataRequest("BloodVolume", VolumeUnit::mL);
}

void PhysiologyEngineThunk::ForwardDebug(const std::string& msg)
{
  if (m_ForwardLogs)
    m_ForwardLogs->ForwardDebug(msg);
  if (m_keepLogMsgs)
    m_logMsgs.debug_msgs.push_back(msg);
}

void PhysiologyEngineThunk::ForwardInfo(const std::string& msg)
{
  if (m_ForwardLogs)
    m_ForwardLogs->ForwardInfo(msg);
  if (m_keepLogMsgs)
    m_logMsgs.info_msgs.push_back(msg);
}

void PhysiologyEngineThunk::ForwardWarning(const std::string& msg)
{
  if (m_ForwardLogs)
    m_ForwardLogs->ForwardWarning(msg);
  if (m_keepLogMsgs)
    m_logMsgs.warning_msgs.push_back(msg);
}

void PhysiologyEngineThunk::ForwardError(const std::string& msg)
{
  if (m_ForwardLogs)
    m_ForwardLogs->ForwardError(msg);
  if (m_keepLogMsgs)
    m_logMsgs.error_msgs.push_back(msg);
}

void PhysiologyEngineThunk::ForwardFatal(const std::string& msg)
{
  if (m_ForwardLogs)
    m_ForwardLogs->ForwardFatal(msg);
  if (m_keepLogMsgs)
    m_logMsgs.fatal_msgs.push_back(msg);
}

void PhysiologyEngineThunk::HandleEvent(eEvent type, bool active, const SEScalarTime* time)
{
  if (m_ForwardEvents)
    m_ForwardEvents->HandleEvent(type, active, time);
  if (m_keepEventChanges)
    m_events.push_back(new SEEventChange(type, active, time));
}