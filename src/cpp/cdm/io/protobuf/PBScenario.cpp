/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
PUSH_PROTO_WARNINGS
#include "pulse/cdm/bind/Scenario.pb.h"
POP_PROTO_WARNINGS
#include "cdm/io/protobuf/PBScenario.h"
#include "cdm/io/protobuf/PBActions.h"
#include "cdm/io/protobuf/PBEngine.h"
#include "cdm/io/protobuf/PBUtils.h"
#include "cdm/scenario/SEScenario.h"
#include "cdm/scenario/SEScenarioExec.h"
#include "cdm/utils/FileUtils.h"


const std::string& eRelativeSerialization_Name(eRelativeSerialization to)
{
  return CDM_BIND::ScenarioExecData::eRelativeSerialization_Name((CDM_BIND::ScenarioExecData::eRelativeSerialization)to);
}

void PBScenario::Load(const CDM_BIND::ScenarioData& src, SEScenario& dst)
{
  dst.Clear();
  PBScenario::Serialize(src, dst);
}
void PBScenario::Serialize(const CDM_BIND::ScenarioData& src, SEScenario& dst)
{
  dst.SetName(src.name());
  dst.SetDescription(src.description());

  if(!src.enginestatefile().empty())
    dst.SetEngineStateFile(src.enginestatefile());
  if (src.has_patientconfiguration())
    PBEngine::Load(src.patientconfiguration(), dst.GetPatientConfiguration(), *dst.m_SubMgr);

  if (src.has_datarequestmanager())
    PBEngine::Load(src.datarequestmanager(), dst.GetDataRequestManager());

  for (int i = 0; i < src.datarequestfile_size(); i++)
    dst.m_DataRequestFiles.push_back(src.datarequestfile()[i]);

  for (int i = 0; i < src.anyaction_size(); i++)
  {
    SEAction* a = PBAction::Load(src.anyaction()[i], *dst.m_SubMgr);
    if(a != nullptr)
      dst.m_Actions.push_back(a);
  }
}

CDM_BIND::ScenarioData* PBScenario::Unload(const SEScenario& src)
{
  CDM_BIND::ScenarioData* dst = new CDM_BIND::ScenarioData();
  PBScenario::Serialize(src, *dst);
  return dst;
}
void PBScenario::Serialize(const SEScenario& src, CDM_BIND::ScenarioData& dst)
{
  dst.set_name(src.m_Name);
  dst.set_description(src.m_Description);

  if (src.HasEngineStateFile())
    dst.set_enginestatefile(src.m_EngineStateFile);
  else if (src.HasPatientConfiguration())
    dst.set_allocated_patientconfiguration(PBEngine::Unload(*src.m_PatientConfiguration));

  dst.set_allocated_datarequestmanager(PBEngine::Unload(*src.m_DataRequestMgr));

  dst.mutable_datarequestfile()->Add(src.m_DataRequestFiles.begin(), src.m_DataRequestFiles.end());

  for (const SEAction* a : src.m_Actions)
    dst.mutable_anyaction()->AddAllocated(PBAction::Unload(*a));
}
void PBScenario::Copy(const SEScenario& src, SEScenario& dst)
{
  dst.Clear();
  CDM_BIND::ScenarioData data;
  PBScenario::Serialize(src, data);
  PBScenario::Serialize(data, dst);
}

bool PBScenario::SerializeToString(const SEScenario& src, std::string& output, eSerializationFormat m)
{
  CDM_BIND::ScenarioData data;
  PBScenario::Serialize(src, data);
  return PBUtils::SerializeToString(data, output, m, src.GetLogger());
}
bool PBScenario::SerializeToFile(const SEScenario& src, const std::string& filename)
{
  CDM_BIND::ScenarioData data;
  PBScenario::Serialize(src, data);
  return PBUtils::SerializeToFile(data, filename, src.GetLogger());
}
bool PBScenario::SerializeFromString(const std::string& src, SEScenario& dst, eSerializationFormat m)
{
  CDM_BIND::ScenarioData data;
  if (!PBUtils::SerializeFromString(src, data, m, dst.GetLogger()))
    return false;
  PBScenario::Load(data, dst);
  return true;
}
bool PBScenario::SerializeFromFile(const std::string& filename, SEScenario& dst)
{
  CDM_BIND::ScenarioData data;
  if (!PBUtils::SerializeFromFile(filename, data, dst.GetLogger()))
    return false;
  PBScenario::Load(data, dst);
  return true;
}

void PBScenario::Copy(const SEScenarioExec& src, SEScenarioExec& dst)
{
  dst.Clear();
  CDM_BIND::ScenarioExecData data;
  PBScenario::Serialize(src, data);
  PBScenario::Serialize(data, dst);
}
void PBScenario::Load(const CDM_BIND::ScenarioExecData& src, SEScenarioExec& dst)
{
  dst.Clear();
  PBScenario::Serialize(src, dst);
}
void PBScenario::Serialize(const CDM_BIND::ScenarioExecData& src, SEScenarioExec& dst)
{
  dst.LogToConsole((eSwitch)src.logtoconsole());
  dst.SetRelativeSerialization((eRelativeSerialization)src.relativeserialization());
  dst.SetDataRootDirectory(src.datarootdirectory());
  dst.SetOutputRootDirectory(src.outputrootdirectory());
  dst.OrganizeOutputDirectory((eSwitch)src.organizeoutputdirectory());

  dst.AutoSerializeAfterActions((eSwitch)src.autoserializeafteractions());
  dst.SetAutoSerializePeriod_s(src.autoserializeperiod_s());
  dst.TimeStampSerializedStates((eSwitch)src.timestampserializedstates());

  switch (src.EngineConfiguration_case())
  {
  case CDM_BIND::ScenarioExecData::EngineConfigurationCase::kEngineConfigurationContent:
  {
    dst.SetEngineConfigurationContent(src.engineconfigurationcontent());
    break;
  }
  case CDM_BIND::ScenarioExecData::EngineConfigurationCase::kEngineConfigurationFilename:
  {
    dst.SetEngineConfigurationFilename(src.engineconfigurationfilename());
    break;
  }
  case CDM_BIND::ScenarioExecData::EngineConfigurationCase::ENGINECONFIGURATION_NOT_SET:
  {
    break;
  }
  }

  switch (src.Scenario_case())
  {
  case CDM_BIND::ScenarioExecData::ScenarioCase::kScenarioContent:
  {
    dst.SetScenarioContent(src.scenariocontent());
    break;
  }
  case CDM_BIND::ScenarioExecData::ScenarioCase::kScenarioFilename:
  {
    dst.SetScenarioFilename(src.scenariofilename());
    break;
  }
  case CDM_BIND::ScenarioExecData::ScenarioCase::kScenarioDirectory:
  {
    dst.SetScenarioDirectory(src.scenariodirectory());
    break;
  }
  case CDM_BIND::ScenarioExecData::ScenarioCase::kScenarioExecListFilename:
  {
    dst.SetScenarioExecListFilename(src.scenarioexeclistfilename());
    break;
  }
  case CDM_BIND::ScenarioExecData::ScenarioCase::kScenarioLogFilename:
  {
    dst.SetScenarioLogFilename(src.scenariologfilename());
    break;
  }
  case CDM_BIND::ScenarioExecData::ScenarioCase::kScenarioLogDirectory:
  {
    dst.SetScenarioLogDirectory(src.scenariologdirectory());
    break;
  }
  case CDM_BIND::ScenarioExecData::ScenarioCase::SCENARIO_NOT_SET:
  {
    break;
  }
  }

  dst.SetContentFormat((eSerializationFormat)src.contentformat());
  dst.SetThreadCount(src.threadcount());

  for (int i = 0; i < src.datarequestfilessearch_size(); i++)
    dst.GetDataRequestFilesSearch().insert(src.datarequestfilessearch(i));
}

CDM_BIND::ScenarioExecData* PBScenario::Unload(const SEScenarioExec& src)
{
  CDM_BIND::ScenarioExecData* dst = new CDM_BIND::ScenarioExecData();
  PBScenario::Serialize(src, *dst);
  return dst;
}
void PBScenario::Serialize(const SEScenarioExec& src, CDM_BIND::ScenarioExecData& dst)
{
  dst.set_logtoconsole((CDM_BIND::eSwitch)src.LogToConsole());
  dst.set_relativeserialization((CDM_BIND::ScenarioExecData::eRelativeSerialization)src.GetRelativeSerialization());
  dst.set_datarootdirectory(src.GetDataRootDirectory());
  dst.set_outputrootdirectory(src.GetOutputRootDirectory());
  dst.set_organizeoutputdirectory((CDM_BIND::eSwitch)src.OrganizeOutputDirectory());

  dst.set_autoserializeafteractions((CDM_BIND::eSwitch)src.AutoSerializeAfterActions());
  dst.set_autoserializeperiod_s(src.GetAutoSerializePeriod_s());
  dst.set_timestampserializedstates((CDM_BIND::eSwitch)src.TimeStampSerializedStates());

  if (!src.GetEngineConfigurationContent().empty())
    dst.set_engineconfigurationcontent(src.GetEngineConfigurationContent());
  else if (!src.GetEngineConfigurationFilename().empty())
    dst.set_engineconfigurationfilename(src.GetEngineConfigurationFilename());

  if (!src.GetScenarioContent().empty())
    dst.set_scenariocontent(src.GetScenarioContent());
  else if (!src.GetScenarioFilename().empty())
    dst.set_scenariofilename(src.GetScenarioFilename());
  else if (!src.GetScenarioDirectory().empty())
    dst.set_scenariodirectory(src.GetScenarioDirectory());
  else if (!src.GetScenarioLogFilename().empty())
    dst.set_scenariologfilename(src.GetScenarioLogFilename());
  else if (!src.GetScenarioLogDirectory().empty())
    dst.set_scenariologdirectory(src.GetScenarioLogDirectory());

  dst.set_contentformat((CDM_BIND::eSerializationFormat)src.GetContentFormat());
  dst.set_threadcount(src.GetThreadCount());

  for (std::string f : src.GetDataRequestFilesSearch())
    dst.mutable_datarequestfilessearch()->AddAllocated(&f);
}

bool PBScenario::SerializeToString(const SEScenarioExec& src, std::string& output, eSerializationFormat m, Logger* logger)
{
  CDM_BIND::ScenarioExecData data;
  PBScenario::Serialize(src, data);
  return PBUtils::SerializeToString(data, output, m, logger);
}
bool PBScenario::SerializeFromString(const std::string& src, SEScenarioExec& dst, eSerializationFormat m, Logger* logger)
{
  CDM_BIND::ScenarioExecData data;
  if (!PBUtils::SerializeFromString(src, data, m, logger))
    return false;
  PBScenario::Load(data, dst);
  return true;
}


void PBScenario::Load(const CDM_BIND::ScenarioExecStatusData& src, SEScenarioExecStatus& dst)
{
  dst.Clear();
  PBScenario::Serialize(src, dst);
}
void PBScenario::Serialize(const CDM_BIND::ScenarioExecStatusData& src, SEScenarioExecStatus& dst)
{
  PBEngine::Serialize(src.initializationstatus(), (SEEngineInitializationStatus&)dst);
  if (!src.scenariofilename().empty())
    dst.SetScenarioFilename(src.scenariofilename());
  dst.SetScenarioExecutionState((eScenarioExecutionState)src.scenarioexecutionstate());
  dst.SetRuntimeError(src.runtimeerror());
  dst.SetFatalRuntimeError(src.fatalruntimeerror());
  dst.SetFinalSimulationTime_s(src.finalsimulationtime_s());
}
CDM_BIND::ScenarioExecStatusData* PBScenario::Unload(const SEScenarioExecStatus& src)
{
  CDM_BIND::ScenarioExecStatusData* dst = new CDM_BIND::ScenarioExecStatusData();
  PBScenario::Serialize(src, *dst);
  return dst;
}
void PBScenario::Serialize(const SEScenarioExecStatus& src, CDM_BIND::ScenarioExecStatusData& dst)
{
  PBEngine::Serialize((SEEngineInitializationStatus&)src, *dst.mutable_initializationstatus());
  if (src.HasScenarioFilename())
    dst.set_scenariofilename(src.m_ScenarioFilename);
  dst.set_scenarioexecutionstate((CDM_BIND::eScenarioExecutionState)src.m_ScenarioExecutionState);
  dst.set_runtimeerror(src.HasRuntimeError());
  dst.set_fatalruntimeerror(src.HasFatalRuntimeError());
  dst.set_finalsimulationtime_s(src.m_FinalSimulationTime_s);
}

const std::string& eScenarioExecutionState_Name(eScenarioExecutionState s)
{
  return CDM_BIND::eScenarioExecutionState_Name((CDM_BIND::eScenarioExecutionState)s);
}

bool PBScenario::SerializeToString(const SEScenarioExecStatus& src, std::string& output, eSerializationFormat m, Logger* logger)
{
  CDM_BIND::ScenarioExecStatusData data;
  PBScenario::Serialize(src, data);
  return PBUtils::SerializeToString(data, output, m, logger);
}
bool PBScenario::SerializeToString(const std::vector<SEScenarioExecStatus>& src, std::string& output, eSerializationFormat m, Logger* logger)
{
  CDM_BIND::ScenarioExecStatusListData data;
  for (auto& status : src)
  {
    PBScenario::Serialize(status, *data.add_scenarioexecstatus());
  }
  return PBUtils::SerializeToString(data, output, m, logger);
}
bool PBScenario::SerializeFromString(const std::string& src, SEScenarioExecStatus& dst, eSerializationFormat m, Logger* logger)
{
  CDM_BIND::ScenarioExecStatusData data;
  if (!PBUtils::SerializeFromString(src, data, m, logger))
    return false;
  PBScenario::Load(data, dst);
  return true;
}
bool PBScenario::SerializeFromString(const std::string& src, std::vector<SEScenarioExecStatus>& dst, eSerializationFormat m, Logger* logger)
{
  CDM_BIND::ScenarioExecStatusListData data;
  if (!PBUtils::SerializeFromString(src, data, m, logger))
    return false;

  SEScenarioExecStatus status;
  for (int i = 0; i < data.scenarioexecstatus_size(); i++)
  {
    PBScenario::Load(data.scenarioexecstatus()[i], status);
    dst.push_back(status);
  }
  return true;
}

bool PBScenario::SerializeToFile(const std::vector<SEScenarioExecStatus>& src, const std::string& filename, Logger* logger)
{
  CDM_BIND::ScenarioExecStatusListData data;
  for (auto& s : src)
  {
    PBScenario::Serialize(s,*data.add_scenarioexecstatus());
  }
  return PBUtils::SerializeToFile(data, filename, logger);
}
bool PBScenario::SerializeFromFile(const std::string& filename, std::vector<SEScenarioExecStatus>& dst, Logger* logger)
{
  dst.clear();
  CDM_BIND::ScenarioExecStatusListData data;
  if (!PBUtils::SerializeFromFile(filename, data, logger))
    return false;

  SEScenarioExecStatus status;
  for (int i = 0; i < data.scenarioexecstatus_size(); i++)
  {
    PBScenario::Load(data.scenarioexecstatus()[i], status);
    dst.push_back(status);
  }
  return true;
}

void PBScenario::Copy(const SEScenarioExecStatus& src, SEScenarioExecStatus& dst)
{
  dst.Clear();
  CDM_BIND::ScenarioExecStatusData data;
  PBScenario::Serialize(src, data);
  PBScenario::Serialize(data, dst);
}
