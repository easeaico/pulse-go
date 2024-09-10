/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "PulseScenarioDriver.h"
#include "PulseScenarioExec.h"
#include "cdm/scenario/SEScenarioLog.h"
#include "cdm/utils/FileUtils.h"

#define test_serialization false
#define generate_clean_results false
// If we are running with serialization
// and we don't have or want to use a baseline csv
// set this to true to generate a clean csv file
// that did not do any serialization

int main(int argc, char* argv[])
{
  if (argc <= 1)
  {
    std::cerr << "Must provide a scenario filename or a directory\n";
    return 1;
  }
  std::string input = argv[1];
  if (!FileExists(input))
  {
    std::cerr << "File does not exist: "+input+"\n";
    return 1;
  }
  eModelType t = (eModelType)-1;
  bool convertLogs = false;
  if (argc >= 3)
  {
    if (std::string(argv[2]).compare("-c") == 0)
      convertLogs =  true;
    else
      eModelType_ValueOf(argv[2], t);
  }

  std::string outputDir = "./test_results/converted_scenarios";
  if (argc >= 4 && convertLogs)
  {
    outputDir = argv[3];
  }

  if (t == (eModelType)-1)
    t = eModelType::HumanAdultWholeBody;

  Logger logger("PulseScenarioDriver.log");
  logger.LogToConsole(true);
  PulseScenarioExec opts(&logger);
  opts.SetModelType(t);

  if (IsDirectory(input))
  {
    opts.LogToConsole(eSwitch::Off);

    if (convertLogs)
    {
      opts.OrganizeOutputDirectory(eSwitch::On);
      opts.SetOutputRootDirectory(outputDir);
      opts.SetScenarioLogDirectory(input);
    }
    else
      opts.SetScenarioDirectory(input);
  }
  else
  {
    opts.LogToConsole(eSwitch::On);

    std::string filename, ext;
    SplitFilenameExt(input, filename, ext);
    if (ext.compare(".log") == 0)
      opts.SetScenarioLogFilename(input);
    else
    {
      // Peek to see what kind of file this is
      std::string line;
      std::fstream fs(input);
      // Read 2 lines
      getline(fs, line);
      getline(fs, line);
      if(line.find("ScenarioExecStatus")==std::string::npos)
        opts.SetScenarioFilename(input);
      else
        opts.SetScenarioExecListFilename(input);
    }
  }

  if (test_serialization)
  {
    if (generate_clean_results)
    {
      if (!opts.Execute())
        return 1;
    }
    opts.AutoSerializeAfterActions(eSwitch::On);
    opts.TimeStampSerializedStates(eSwitch::On);
    opts.SetAutoSerializePeriod_s(0);

    // In general, serailization testing should always be reloading, but...
    // I don't have it exposed, but if you want to serialize,
    // but not read the results back in, you will need to manually set
    // m_ReloadSerializedState in the SEScenarioExec class to Off
    // This is usually for some really hard core debugging if you need it
    // I would assume you are building the code stepping state loading/unloading
    // So programatically flipping this switch is no big deal
  }

  return !opts.Execute();
}
