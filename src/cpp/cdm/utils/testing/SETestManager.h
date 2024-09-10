/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

#include "cdm/CommonDefs.h"
#include "cdm/utils/Logger.h"

class CDM_DECL SETestManager : public Loggable
{
public:
  SETestManager(Logger* logger = nullptr);

  virtual ~SETestManager() = default;

  virtual bool RunTest(const std::string& testName, const std::string& sOutputDirectory) = 0;

protected:
  virtual void FillFunctionMap()=0;
  std::stringstream m_ss;
};
