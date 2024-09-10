/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

class Logger;
class SEScalarTime;
class log_lib; // Encapsulates 3rd party logging library
#include <sstream>
#include <mutex>

enum class ePrettyPrintType { Action = 0, Condition };

namespace pulse { namespace cdm
{
  // Not happy with how std does this for floats/doubles
  CDM_DECL std::string to_string(float f);
  CDM_DECL std::string to_string(double d);
  CDM_DECL std::string PrettyPrint(const std::string& str, ePrettyPrintType ppt);
}}

class CDM_DECL Loggable
{
public:

  Loggable(Logger* logger = nullptr);
  Loggable(std::string const& logfile);
  virtual ~Loggable();

  virtual Logger* GetLogger() const;
  virtual void    SetLogger(Logger& logger);

  virtual void Debug(std::string const&  msg) const;
  virtual void Debug(std::stringstream &msg) const;
  virtual void Debug(std::ostream &msg) const;

  virtual void Info(std::string const&  msg) const;
  virtual void Info(std::stringstream &msg) const;
  virtual void Info(const std::stringstream &msg) const;
  virtual void Info(std::ostream &msg) const;

  virtual void Warning(std::string const&  msg) const;
  virtual void Warning(std::stringstream &msg) const;
  virtual void Warning(std::ostream &msg) const;

  virtual void Error(std::string const&  msg) const;
  virtual void Error(std::stringstream &msg) const;
  virtual void Error(std::ostream &msg) const;

  virtual void Fatal(std::string const&  msg) const;
  virtual void Fatal(std::stringstream &msg) const;
  virtual void Fatal(std::ostream &msg) const;

protected:
  bool    myLogger;
  Logger* m_Logger;
};

class CDM_DECL LoggerForward
{
public:
  virtual ~LoggerForward() = default;
  virtual void ForwardDebug(std::string const& /*msg*/) {};
  virtual void ForwardInfo(std::string const& /*msg*/){};
  virtual void ForwardWarning(std::string const& /*msg*/){};
  virtual void ForwardError(std::string const& /*msg*/){};
  virtual void ForwardFatal(std::string const& /*msg*/){};
};

class CDM_DECL Logger
{
  friend Loggable;
public:
  Logger(std::string const&  logFilename = "");
  virtual ~Logger();

  void LogToConsole(bool b);
  bool IsLoggingToConsole();
  // Put some identifier at the beginning of each message going to std::cout
  void AddConsolePrefix(const std::string& p);

  void SetLogFile(std::string const&  logFilename = "");

  enum class Level
  {
    Debug,
    Info,
    Warn,
    Error,
    Fatal
  };
  void SetLogLevel(Level  level);
  Level GetLogLevel() const;


  virtual void SetLogTime(const SEScalarTime* time);

  virtual bool HasForward() const;
  virtual void AddForward(LoggerForward* forward);
  virtual void RemoveForward(LoggerForward* forward);
  virtual void RemoveForwards();

  virtual void Debug(std::string const&  msg);
  virtual void Debug(std::stringstream &msg);
  virtual void Debug(std::ostream &msg);

  virtual void Info(std::string const&  msg);
  virtual void Info(std::stringstream &msg);
  virtual void Info(const std::stringstream &msg);
  virtual void Info(std::ostream &msg);

  virtual void Warning(std::string const&  msg);
  virtual void Warning(std::stringstream &msg);
  virtual void Warning(std::ostream &msg);

  virtual void Error(std::string const&  msg);
  virtual void Error(std::stringstream &msg);
  virtual void Error(std::ostream &msg);

  virtual void Fatal(std::string const&  msg);
  virtual void Fatal(std::stringstream &msg);
  virtual void Fatal(std::ostream &msg);

protected:

  virtual std::string FormatLogMessage(std::string const&  msg);

  std::vector<LoggerForward*>   m_Forwards;
  const SEScalarTime*           m_time;

private:
   log_lib*                     _log_lib;
};

class CDM_DECL LogMessages
{
public:
  LogMessages() {}
  virtual ~LogMessages() {};

  bool static SerializeToString(const LogMessages& msgs, std::string& output, eSerializationFormat m, Logger* logger);
  bool static SerializeFromString(const std::string& src, LogMessages& msgs, eSerializationFormat m, Logger* logger);

  void Clear()
  {
    debug_msgs.clear();
    info_msgs.clear();
    warning_msgs.clear();
    error_msgs.clear();
    fatal_msgs.clear();
  }

  bool IsEmpty()
  {
    if (!debug_msgs.empty())
      return false;
    if (!info_msgs.empty())
      return false;
    if (!warning_msgs.empty())
      return false;
    if (!error_msgs.empty())
      return false;
    if (!fatal_msgs.empty())
      return false;
    return true;
  }

  std::vector<std::string> debug_msgs;
  std::vector<std::string> info_msgs;
  std::vector<std::string> warning_msgs;
  std::vector<std::string> error_msgs;
  std::vector<std::string> fatal_msgs;
};

inline std::ostream& operator<< (std::ostream& out, const LogMessages& a)
{
  if (!a.debug_msgs.empty())
  {
    out << "Debug Messages: \n";
    for (auto& m : a.debug_msgs)
      out << m << "\n";
  }
  if (!a.info_msgs.empty())
  {
    out << "Info Messages: \n";
    for (auto& m : a.info_msgs)
      out << m << "\n";
  }
  if (!a.warning_msgs.empty())
  {
    out << "Warning Messages: \n";
    for (auto& m : a.warning_msgs)
      out << m << "\n";
  }
  if (!a.error_msgs.empty())
  {
    out << "Error Messages: \n";
    for (auto& m : a.error_msgs)
      out << m << "\n";
  }
  if (!a.fatal_msgs.empty())
  {
    out << "Fatal Messages: \n";
    for (auto& m : a.fatal_msgs)
      out << m << "\n";
  }
  return out;
}
