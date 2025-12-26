#ifndef PULSE_C_BRIDGE_H
#define PULSE_C_BRIDGE_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration of the opaque pointer type
typedef struct PhysiologyEngineThunk PhysiologyEngineThunk;

// Declare the C functions exported by PulseEngineC.cpp
// These must match the signatures in PulseEngineC.cpp (exported as extern "C")

void PulseInitialize();
void PulseVersion(char **version_str);
void PulseHash(char **hash_str);
void PulseDeinitialize();

PhysiologyEngineThunk *Allocate(int engine_model, const char *dataDir);
void Deallocate(PhysiologyEngineThunk *thunk);

int ExecuteScenario(const char *sceOpts, int format);
void Clear(PhysiologyEngineThunk *thunk);

int SerializeFromFile(PhysiologyEngineThunk *thunk, const char *filename,
                      const char *data_requests, int data_requests_format);
int SerializeToFile(PhysiologyEngineThunk *thunk, const char *filename);
int SerializeFromString(PhysiologyEngineThunk *thunk, const char *state,
                        const char *data_requests, int format);
int SerializeToString(PhysiologyEngineThunk *thunk, int format,
                      char **state_str);

int InitializeEngine(PhysiologyEngineThunk *thunk,
                     const char *patient_configuration,
                     const char *data_requests, int format);
int GetInitialPatient(PhysiologyEngineThunk *thunk, int format,
                      char **str_addr);
int GetConditions(PhysiologyEngineThunk *thunk, int format, char **conditions);

void LogToConsole(PhysiologyEngineThunk *thunk, int b);
void KeepLogMessages(PhysiologyEngineThunk *thunk, int keep);
void SetLogFilename(PhysiologyEngineThunk *thunk, const char *filename);
int PullLogMessages(PhysiologyEngineThunk *thunk, int format, char **str_addr);

void KeepEventChanges(PhysiologyEngineThunk *thunk, int keep);
int PullEvents(PhysiologyEngineThunk *thunk, int format, char **str_addr);
int PullActiveEvents(PhysiologyEngineThunk *thunk, int format, char **active);

int GetPatientAssessment(PhysiologyEngineThunk *thunk, int type, int format,
                         char **assessment);

int ProcessActions(PhysiologyEngineThunk *thunk, const char *actions,
                   int format);
int PullActiveActions(PhysiologyEngineThunk *thunk, int format, char **actions);

int AdvanceTimeStep(PhysiologyEngineThunk *thunk);
double GetTimeStep(PhysiologyEngineThunk *thunk, const char *unit);
double *PullData(PhysiologyEngineThunk *thunk);

int AreCompatibleUnits(const char *fromUnit, const char *toUnit);
double ConvertValue(double value, const char *fromUnit, const char *toUnit);

#ifdef __cplusplus
}
#endif

#endif // PULSE_C_BRIDGE_H
