/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "pulsec_export.h"
#include <cstring>
#include <memory>
#include <iostream>
#include "PulseEngine.h"
#include "PulseEngineThunk.h"
#include "cdm/utils/unitconversion/UnitConversionEngine.h"

// Implement a C Style strdup
// So we can still use std-c11
char* c_strdup(const char* s, size_t slen)
{
  char* result = (char*)malloc(slen + 1);
  if (result == NULL)
  {
    return NULL;
  }
  memcpy(result, s, slen + 1);
  return result;
}

#if defined (__clang__)
#define C_CALL
#elif defined(__gnu_linux__)
#define C_CALL __attribute__((stdcall))
#else
#define C_CALL __stdcall
#endif

extern "C"
PULSEC_EXPORT void C_CALL PulseInitialize()
{
  
}

extern "C"
PULSEC_EXPORT void C_CALL PulseVersion(char** version_str)
{
  std::string version = PulseBuildInformation::Version();
  *version_str = c_strdup(version.c_str(), version.length());
}

extern "C"
PULSEC_EXPORT void C_CALL PulseHash(char** hash_str)
{
  std::string hash = PulseBuildInformation::Hash();
  *hash_str = c_strdup(hash.c_str(), hash.length());
}

extern "C"
PULSEC_EXPORT void C_CALL PulseDeinitialize()
{
  CUnitConversionEngine::DestroyEngine();
}

extern "C"
PULSEC_EXPORT PhysiologyEngineThunk* C_CALL Allocate(int engine_model, const char* dataDir)
{
  return new PulseEngineThunk((eModelType)engine_model, dataDir);
}

extern "C"
PULSEC_EXPORT void C_CALL Deallocate(PhysiologyEngineThunk* thunk)
{
  SAFE_DELETE(thunk);
}

extern "C"
PULSEC_EXPORT bool C_CALL ExecuteScenario(const char* sceOpts, int format)
{
  return PulseEngineThunk::ExecuteScenario(sceOpts, (eSerializationFormat)format);
}

extern "C"
PULSEC_EXPORT void C_CALL Clear(PhysiologyEngineThunk * thunk)
{
  thunk->Clear();
}

extern "C"
PULSEC_EXPORT bool C_CALL IsValidPatient(PhysiologyEngineThunk* thunk, const char* patient, int format)
{
  return thunk->IsValidPatient(patient, (eSerializationFormat)format);
}

extern "C"
PULSEC_EXPORT bool C_CALL SerializeFromFile(PhysiologyEngineThunk * thunk, const char* filename, const char* data_requests, int data_requests_format)
{
  return thunk->SerializeFromFile(filename==nullptr?"":filename, data_requests==nullptr?"":data_requests, (eSerializationFormat)data_requests_format);
}
extern "C"
PULSEC_EXPORT bool C_CALL SerializeToFile(PhysiologyEngineThunk* thunk, const char* filename)
{
  return thunk->SerializeToFile(filename==nullptr?"":filename);
}

extern "C"
PULSEC_EXPORT bool C_CALL SerializeFromString(PhysiologyEngineThunk* thunk, const char* state, const char* data_requests, int format)
{
  return thunk->SerializeFromString(state==nullptr?"":state, data_requests==nullptr?"":data_requests, (eSerializationFormat)format);
}
extern "C"
PULSEC_EXPORT bool C_CALL SerializeToString(PhysiologyEngineThunk* thunk, int format, char** state_str)
{
  std::string state = thunk->SerializeToString((eSerializationFormat)format);
  *state_str = c_strdup(state.c_str(), state.length());
  return true;
}

extern "C"
PULSEC_EXPORT bool C_CALL InitializeEngine(PhysiologyEngineThunk* thunk, const char* patient_configuration, const char* data_requests, int format)
{
  return thunk->InitializeEngine(patient_configuration==nullptr?"":patient_configuration, data_requests==nullptr?"":data_requests, (eSerializationFormat)format);
}

extern "C"
PULSEC_EXPORT bool C_CALL GetInitialPatient(PhysiologyEngineThunk* thunk, int format, char** str_addr)
{
  std::string initial_patient = thunk->GetInitialPatient((eSerializationFormat)format);
  if (initial_patient.empty())
    return false;
  *str_addr = c_strdup(initial_patient.c_str(), initial_patient.length());
  return true;
}

extern "C"
PULSEC_EXPORT bool C_CALL GetConditions(PhysiologyEngineThunk* thunk, int format, char** conditions)
{
  std::string c = thunk->GetConditions((eSerializationFormat)format);
  if (c.empty())
    return false;
  *conditions = c_strdup(c.c_str(), c.length());
  return true;
}

extern "C"
PULSEC_EXPORT void C_CALL LogToConsole(PhysiologyEngineThunk* thunk, bool b)
{
  thunk->LogToConsole(b);
}
extern "C"
PULSEC_EXPORT void C_CALL KeepLogMessages(PhysiologyEngineThunk* thunk, bool keep)
{
  thunk->KeepLogMessages(keep);
}
extern "C"
PULSEC_EXPORT void C_CALL SetLogFilename(PhysiologyEngineThunk* thunk, const char* filename)
{
  thunk->SetLogFilename(filename==nullptr?"":filename);
}
extern "C"
PULSEC_EXPORT bool C_CALL PullLogMessages(PhysiologyEngineThunk* thunk, int format, char** str_addr)
{
  std::string log_msgs = thunk->PullLogMessages((eSerializationFormat)format);
  if (log_msgs.empty())
    return false;
  *str_addr = c_strdup(log_msgs.c_str(), log_msgs.length());
  return true;
}

extern "C"
PULSEC_EXPORT void C_CALL KeepEventChanges(PhysiologyEngineThunk* thunk, bool keep)
{
  thunk->KeepEventChanges(keep);
}
extern "C"
PULSEC_EXPORT bool C_CALL PullEvents(PhysiologyEngineThunk* thunk, int format, char** str_addr)
{
  std::string event_changes = thunk->PullEvents((eSerializationFormat)format);
  if (event_changes.empty())
    return false;
  *str_addr = c_strdup(event_changes.c_str(), event_changes.length());
  return true;
}
extern "C"
PULSEC_EXPORT bool C_CALL PullActiveEvents(PhysiologyEngineThunk* thunk, int format, char** active)
{
  std::string active_events = thunk->PullActiveEvents((eSerializationFormat)format);
  if (active_events.empty())
    return false;
  *active = c_strdup(active_events.c_str(), active_events.length());
  return true;
}

extern "C"
PULSEC_EXPORT bool C_CALL GetPatientAssessment(PhysiologyEngineThunk* thunk, int type, int format, char** assessment)
{
  std::string a = thunk->GetPatientAssessment(type, (eSerializationFormat)format);
  if (a.empty())
    return false;
  *assessment = c_strdup(a.c_str(), a.length());
  return true;
}

extern "C"
PULSEC_EXPORT bool C_CALL ProcessActions(PhysiologyEngineThunk* thunk, const char* actions, int format)
{
  if (actions == nullptr)
    return true;// Nothing to do...
  return thunk->ProcessActions(actions, (eSerializationFormat)format);
}

extern "C"
PULSEC_EXPORT bool C_CALL PullActiveActions(PhysiologyEngineThunk* thunk, int format, char** actions)
{
  std::string a = thunk->PullActiveActions((eSerializationFormat)format);
  if (a.empty())
    return false;
  *actions = c_strdup(a.c_str(), a.length());
  return true;
}

extern "C"
PULSEC_EXPORT bool C_CALL AdvanceTimeStep(PhysiologyEngineThunk* thunk)
{
  return thunk->AdvanceTimeStep();
}
extern "C"
PULSEC_EXPORT double C_CALL GetTimeStep(PhysiologyEngineThunk* thunk, const char* unit)
{
  return thunk->GetTimeStep(unit==nullptr?"":unit);
}

extern "C"
PULSEC_EXPORT double* C_CALL PullData(PhysiologyEngineThunk* thunk)
{
  return thunk->PullDataPtr();
}

extern "C"
PULSEC_EXPORT bool C_CALL AreCompatibleUnits(const char* fromUnit, const char* toUnit)
{
  return CompatibleUnits(CCompoundUnit(fromUnit), CCompoundUnit(toUnit));
}
extern "C"
PULSEC_EXPORT double C_CALL ConvertValue(double value, const char* fromUnit, const char* toUnit)
{
  return Convert(value, CCompoundUnit(fromUnit), CCompoundUnit(toUnit));
}
