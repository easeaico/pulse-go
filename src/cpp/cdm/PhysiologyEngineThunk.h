/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

#include "cdm/CommonDefs.h"
// CDM Features in use
#include "cdm/PhysiologyEngine.h"
#include "cdm/engine/SEEventManager.h"
#include "cdm/utils/ScopedMutex.h"

/**
 * An instance of an engine where the interface is define in stl and base data types.
 * This interface is a thunk layer using serialized cdm objects to drive a Pulse engine.
 */
class CDM_DECL PhysiologyEngineThunk : public LoggerForward, public SEEventHandler
{
public:
  PhysiologyEngineThunk(const std::string& dataDir = "./");
  virtual ~PhysiologyEngineThunk();

  virtual void Clear();

  virtual bool IsValidPatient(std::string const& patient, eSerializationFormat format) = 0;

  //virtual void SetConfigurationOverride(std::string const& config)=0; // Not Implemented

  bool SerializeFromFile(std::string const& filename, std::string const& data_requests, eSerializationFormat data_requests_format);
  bool SerializeToFile(std::string const& filename);

  bool SerializeFromString(std::string const& state, std::string const& data_requests, eSerializationFormat format);
  std::string SerializeToString(eSerializationFormat format);

  bool InitializeEngine(std::string const& patient_configuration, std::string const& data_requests, eSerializationFormat format);

  std::string GetInitialPatient(eSerializationFormat format);
  std::string GetConditions(eSerializationFormat format);

  void LogToConsole(bool b);
  void KeepLogMessages(bool keep);// Set this to true if you are going to pull messages from the engine
  void SetLogFilename(std::string const& logfile);// Set to empty if no log file is wanted
  std::string PullLogMessages(eSerializationFormat format);
  void SetLoggerForward(LoggerForward* lf) { m_ForwardLogs = lf; }

  void KeepEventChanges(bool keep);
  std::string PullEvents(eSerializationFormat format);
  std::string PullActiveEvents(eSerializationFormat format);
  std::string GetPatientAssessment(int type, eSerializationFormat format);
  void SetEventHandler(SEEventHandler* eh) { m_ForwardEvents = eh; }

  bool ProcessActions(std::string const& actions, eSerializationFormat format);
  std::string PullActiveActions(eSerializationFormat format);

  bool AdvanceTimeStep();
  double GetTimeStep(std::string const& unit);

  size_t DataLength() const;
  double* PullDataPtr();
  void PullData(std::vector<double>& data);

  void ForwardDebug(const std::string& msg) override;
  void ForwardInfo(const std::string& msg) override;
  void ForwardWarning(const std::string& msg) override;
  void ForwardError(const std::string& msg) override;
  void ForwardFatal(const std::string& msg) override;

  void HandleEvent(eEvent type, bool active, const SEScalarTime* time = nullptr) override;

protected:
  virtual void AllocateEngine() = 0;
  virtual void SetupDefaultDataRequests(SEDataRequestManager& drMgr);

  std::unique_ptr<PhysiologyEngine> m_engine;
  SESubstanceManager* m_subMgr = nullptr;

  bool m_keepLogMsgs = false;
  bool m_keepEventChanges = false;
  LoggerForward* m_ForwardLogs=nullptr;
  SEEventHandler* m_ForwardEvents=nullptr;

  LogMessages m_logMsgs;
  std::string m_dataDir="./";
  std::vector<const SEEventChange*> m_events;
  std::vector<const SEActiveEvent*> m_activeEvents;
  size_t m_length=0;
  double* m_requestedData = nullptr;
  std::vector<double> m_requestedValues;
};
