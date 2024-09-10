/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/utils/Logger.h"
#include "cdm/utils/FileUtils.h"
#include "cdm/properties/SEScalarTime.h"

class log_lib
{
public:
  log_lib()
  {

  }
  virtual ~log_lib()
  {
    if (_file.is_open())
      _file.close();
  }

  bool log(Logger::Level requested_level) const
  {
    return requested_level >= _log_level;
  }
  void log(Logger::Level requested_level, const std::string& fmtMsg)
  {
    std::scoped_lock lock(_mutex);
    if (!_log_to_console && !_log_to_file)
      return;
    if (_last_requested_level != requested_level)
    {
      switch (requested_level)
      {
      case Logger::Level::Debug:
        _str_requested_level = "[DEBUG]";
        break;
      case Logger::Level::Info:
        _str_requested_level = "[INFO]";
        break;
      case Logger::Level::Warn:
        _str_requested_level = "[WARN]";
        break;
      case Logger::Level::Error:
        _str_requested_level = "[ERROR]";
        break;
      case Logger::Level::Fatal:
        _str_requested_level = "[FATAL]";
        break;
      }
      _last_requested_level = requested_level;
      
    }

    if (_log_to_console)
      std::cout << _console_prefix << _str_requested_level << " " << fmtMsg << std::endl;
    if (_log_to_file)
      _file << _str_requested_level << " " << fmtMsg << std::endl;
  }


  std::string _console_prefix = "";
  bool _log_to_console = false;
  bool _log_to_file = false;
  Logger::Level _log_level = Logger::Level::Info;
  std::string _str_requested_level = "[INFO]";
  Logger::Level _last_requested_level = Logger::Level::Info;

  std::ofstream _file;
  std::mutex    _mutex;
};

//logger constructor
Logger::Logger(const std::string& logFilename)
{
  m_time = nullptr;
  _log_lib = new log_lib();
  SetLogFile(logFilename);
}

void Logger::LogToConsole(bool b)
{
  _log_lib->_log_to_console = b;
}

bool Logger::IsLoggingToConsole()
{
  return _log_lib->_log_to_console;
}

void Logger::SetLogFile(const std::string& logFilename)
{
  if (logFilename.empty())
  {
    _log_lib->_log_to_file = false;
    return;
  }
  _log_lib->_log_to_file = true;

  CreateFilePath(logFilename);
  _log_lib->_file.close();
  // delete previous log contents if it exists
  _log_lib->_file.open(logFilename.c_str(), std::ofstream::out | std::ofstream::trunc);
}

Logger::~Logger()
{
  delete _log_lib;
}

//This function will change the priority of the logger
void Logger::SetLogLevel(Logger::Level l)
{
  _log_lib->_log_level = l;
}

//This function will return the priority of the logger
Logger::Level Logger::GetLogLevel() const
{
  return _log_lib->_log_level;
}

void Logger::AddConsolePrefix(const std::string& p)
{
  _log_lib->_console_prefix = p;
}

void Logger::SetLogTime(const SEScalarTime* time)
{
  m_time = time;
}


bool Logger::HasForward() const
{
  return !m_Forwards.empty();
}
void Logger::AddForward(LoggerForward* forward)
{
  if (forward != nullptr && std::find(m_Forwards.begin(), m_Forwards.end(), forward) == m_Forwards.end())
    m_Forwards.push_back(forward);
}

void Logger::RemoveForward(LoggerForward* forward)
{
  auto idx = std::find(m_Forwards.begin(), m_Forwards.end(), forward);
  if (idx != m_Forwards.end())
    m_Forwards.erase(idx);
}

void Logger::RemoveForwards()
{
  m_Forwards.clear();
}

std::string Logger::FormatLogMessage(const std::string& msg)
{
  std::string out = "";
  if (m_time != nullptr && m_time->IsValid())
    out += "[" + m_time->ToString() + "] ";

  if (!msg.empty())
    out += msg;

  return out;
}

void Logger::Debug(std::string const& msg)
{
  if (_log_lib->log(Level::Debug))
  {
    std::string fmtMsg = FormatLogMessage(msg);
    _log_lib->log(Level::Debug, fmtMsg);
    for (auto fwd : m_Forwards)
      fwd->ForwardDebug(fmtMsg);
  }
}

void Logger::Debug(std::stringstream& msg)
{
  Debug(msg.str());
  msg.str("");
  msg.clear();
}
void Logger::Debug(std::ostream& msg)
{
  std::stringstream ss;
  ss << msg.rdbuf();
  Debug(ss.str());
}

void Logger::Info(const std::string& msg)
{
  if (_log_lib->log(Level::Info))
  {
    std::string fmtMsg = FormatLogMessage(msg);
    _log_lib->log(Level::Info, fmtMsg);
    for (auto fwd : m_Forwards)
      fwd->ForwardInfo(fmtMsg);
  }
}

void Logger::Info(std::stringstream& msg)
{
  Info(msg.str());
  msg.str("");
  msg.clear();
}

void Logger::Info(const std::stringstream& msg)
{
  Info(msg.str());
}

void Logger::Info(std::ostream& msg)
{
  std::stringstream ss;
  ss << msg.rdbuf();
  Info(ss.str());
}

void Logger::Warning(const std::string& msg)
{
  if (_log_lib->log(Level::Warn))
  {
    std::string fmtMsg = FormatLogMessage(msg);
    _log_lib->log(Level::Warn, fmtMsg);
    for (auto fwd : m_Forwards)
      fwd->ForwardWarning(fmtMsg);
  }
}
void Logger::Warning(std::stringstream& msg)
{
  Warning(msg.str());
  msg.str("");
  msg.clear();
}
void Logger::Warning(std::ostream& msg)
{
  std::stringstream ss;
  ss << msg.rdbuf();
  Warning(ss.str());
}

void Logger::Error(const std::string& msg)
{
  if (_log_lib->log(Level::Error))
  {
    std::string fmtMsg = FormatLogMessage(msg);
    _log_lib->log(Level::Error, fmtMsg);
    for (auto fwd : m_Forwards)
      fwd->ForwardError(fmtMsg);
  }
}
void Logger::Error(std::stringstream& msg)
{
  Error(msg.str());
  msg.str("");
  msg.clear();
}
void Logger::Error(std::ostream& msg)
{
  std::stringstream ss;
  ss << msg.rdbuf();
  Error(ss.str());
}

void Logger::Fatal(const std::string& msg)
{
  if (_log_lib->log(Level::Fatal))
  {
    std::string fmtMsg = FormatLogMessage(msg);
    _log_lib->log(Level::Fatal, fmtMsg);
    // Going through forwards in reverse, as our controller handler will be first
    // And that will kick out an exception and halt the engine, so let's let
    // the other forwards process this fatal first, before we halt the engine
    for (auto fwd = m_Forwards.rbegin(); fwd != m_Forwards.rend(); ++fwd)
      (*fwd)->ForwardFatal(fmtMsg);
  }
}
void Logger::Fatal(std::stringstream& msg)
{
  Fatal(msg.str());
  msg.str("");
  msg.clear();
}
void Logger::Fatal(std::ostream& msg)
{
  std::stringstream ss;
  ss << msg.rdbuf();
  Fatal(ss.str());
}

Loggable::Loggable(Logger* logger)
{
  myLogger = false;
  m_Logger = logger;
  if (m_Logger == nullptr)
  {
    myLogger = true;
    m_Logger = new Logger();
  }
}

Loggable::Loggable(std::string const& logfile)
{
  myLogger = true;
  m_Logger = new Logger(logfile);
}

Loggable::~Loggable()
{
  if (myLogger)
    SAFE_DELETE(m_Logger);
}

Logger* Loggable::GetLogger() const
{
  return m_Logger;
}
void Loggable::SetLogger(Logger& logger)
{
  if (myLogger)
    delete m_Logger;
  myLogger = false;
  m_Logger = &logger;
}

void Loggable::Debug(const std::string& msg) const
{
  if (m_Logger)
    m_Logger->Debug(msg);
}
void Loggable::Debug(std::stringstream& msg) const
{
  Debug(msg.str());
  msg.str("");
  msg.clear();
}
void Loggable::Debug(std::ostream& msg) const
{
  std::stringstream ss;
  ss << msg.rdbuf();
  Debug(ss.str());
}

void Loggable::Info(const std::stringstream& msg) const
{
  Info(msg.str());
}

void Loggable::Info(std::ostream& msg) const
{
  std::stringstream ss;
  ss << msg.rdbuf();
  Info(ss.str());
}

void Loggable::Warning(const std::string& msg) const
{
  if (m_Logger)
    m_Logger->Warning(msg);
}
void Loggable::Warning(std::stringstream& msg) const
{
  Warning(msg.str());
  msg.str("");
  msg.clear();
}
void Loggable::Warning(std::ostream& msg) const
{
  std::stringstream ss;
  ss << msg.rdbuf();
  Warning(ss.str());
}

void Loggable::Error(const std::string& msg) const
{
  if (m_Logger)
    m_Logger->Error(msg);
}
void Loggable::Error(std::stringstream& msg) const
{
  Error(msg.str());
  msg.str("");
  msg.clear();
}
void Loggable::Error(std::ostream& msg) const
{
  std::stringstream ss;
  ss << msg.rdbuf();
  Error(ss.str());
}

void Loggable::Info(const std::string& msg) const
{
  if (m_Logger)
    m_Logger->Info(msg);
}
void Loggable::Info(std::stringstream& msg) const
{
  Info(msg.str());
  msg.str("");
  msg.clear();
}

void Loggable::Fatal(const std::string& msg) const
{
  std::cerr << "FATAL:" << msg << std::endl;
  if (m_Logger)
    m_Logger->Fatal(msg);
}
void Loggable::Fatal(std::stringstream& msg) const
{
  Fatal(msg.str());
  msg.str("");
  msg.clear();
}
void Loggable::Fatal(std::ostream& msg) const
{
  std::stringstream ss;
  ss << msg.rdbuf();
  Fatal(ss.str());
}
