/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include "PhysiologyEngine.h"
class SEDataRequestManager;

class CDM_DECL SEEngineInitialization : public Loggable
{
  friend class PBEngine;//friend the serialization class
public:
  explicit SEEngineInitialization(Logger* logger=nullptr);
  virtual ~SEEngineInitialization();

  SEEngineInitialization(const SEEngineInitialization&) = delete;
  SEEngineInitialization operator=(const SEEngineInitialization&) = delete;

  virtual void Copy(const SEEngineInitialization& from, const SESubstanceManager& subMgr);
  virtual void Clear(); //clear memory

  bool SerializeToString(std::string& output, eSerializationFormat m) const;
  bool SerializeFromString(const std::string& src, eSerializationFormat m, const SESubstanceManager& subMgr);
  static bool SerializeFromString(const std::string& src, std::vector<SEEngineInitialization*>& dst, eSerializationFormat m, const SESubstanceManager& subMgr);

  virtual bool IsValid()const;

  virtual int GetID() const;
  virtual void SetID(int id);

  virtual bool HasPatientConfiguration() const;
  virtual SEPatientConfiguration& GetPatientConfiguration();
  virtual const SEPatientConfiguration* GetPatientConfiguration() const;

  virtual bool HasStateFilename() const;
  virtual std::string GetStateFilename() const;
  virtual void SetStateFilename(const std::string& filename);

  virtual bool HasState() const;
  virtual std::string GetState() const;
  virtual void SetState(const std::string& s, eSerializationFormat fmt);
  virtual eSerializationFormat GetStateFormat() const;

  virtual bool HasDataRequestManager() const;
  virtual SEDataRequestManager& GetDataRequestManager();
  virtual const SEDataRequestManager* GetDataRequestManager() const;

  virtual bool HasLogFilename() const;
  virtual std::string GetLogFilename() const;
  virtual void SetLogFilename(const std::string& filename);

  virtual bool KeepLogMessages() const;
  virtual void KeepLogMessages(bool b);

  virtual bool KeepEventChanges() const;
  virtual void KeepEventChanges(bool b);

protected:
  int                             m_ID;
  SEPatientConfiguration*         m_PatientConfiguration;
  std::string                     m_StateFilename;
  std::string                     m_State;
  eSerializationFormat            m_StateFormat;
  SEDataRequestManager*           m_DataRequestManager;
  std::string                     m_LogFilename;
  bool                            m_KeepLogMessages;
  bool                            m_KeepEventChanges;
};

class CDM_DECL SEEngineInitializationStatus : public LoggerForward
{
  friend class PBEngine;//friend the serialization class
public:
  SEEngineInitializationStatus();
  virtual ~SEEngineInitializationStatus();

  virtual void Clear();
  void Copy(const SEEngineInitializationStatus& src);

  virtual bool SerializeToString(std::string& output, eSerializationFormat m, Logger*) const;
  virtual bool SerializeFromString(const std::string& src, eSerializationFormat m, Logger*);

  eEngineInitializationState GetEngineInitializationState() const { return m_EngineInitializationState; }
  void SetEngineInitializationState(eEngineInitializationState s) { m_EngineInitializationState = s; }

  bool HasCSVFilename() const { return !m_CSVFilename.empty(); }
  std::string GetCSVFilename() const { return m_CSVFilename; }
  void SetCSVFilename(const std::string& fn) { m_CSVFilename = fn; }

  bool HasLogFilename() const { return !m_LogFilename.empty(); }
  std::string GetLogFilename() const { return m_LogFilename; }
  void SetLogFilename(const std::string& fn) { m_LogFilename = fn; }

  double GetStabilizationTime_s() const { return m_StabilizationTime_s; }
  void SetStabilizationTime_s(double t) { m_StabilizationTime_s = t; }

protected:
  eEngineInitializationState   m_EngineInitializationState;
  std::string                  m_CSVFilename;
  std::string                  m_LogFilename;
  double                       m_StabilizationTime_s;
};
