/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

#include <thread>
#include "PulseEngine.h"

#include "cdm/engine/SEOverrides.h"
#include "cdm/engine/SEDataRequestTracker.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEPatientConfiguration.h"

#include "cdm/substance/SESubstanceManager.h"
#include "cdm/circuit/fluid/SEFluidCircuit.h"
#include "cdm/circuit/fluid/SEFluidCircuitNode.h"
#include "cdm/circuit/fluid/SEFluidCircuitPath.h"
#include "cdm/compartment/fluid/SELiquidCompartmentGraph.h"

#include "cdm/utils/DataTrack.h"
#include "cdm/properties/SERunningAverage.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerPressure.h"
#include "cdm/utils/FileUtils.h"
#include "cdm/utils/GeneralMath.h"
#include "cdm/utils/TimingProfile.h"

#include "cdm/io/protobuf/PBActions.h"
PUSH_PROTO_WARNINGS
#include "pulse/study/bind/Hydrocephalus.pb.h"
#include <google/protobuf/text_format.h>
#include <google/protobuf/util/json_util.h>
POP_PROTO_WARNINGS

namespace pulse::study::hydrocephalus
{
  class HRunner : public Loggable
  {
  public:
    HRunner(const std::string& logfileName, const std::string& dataDir = ".");
    virtual ~HRunner();

    bool Run(const std::string& filename, eSerializationFormat f);
    bool Run(pulse::study::bind::hydrocephalus::SimulationListData& simList);

    bool RunSimulationUntilStable(std::string const& outDir, pulse::study::bind::hydrocephalus::SimulationData& sim, const std::string& dataDir = "./");
  protected:
    bool Run();
    bool SerializeToString(pulse::study::bind::hydrocephalus::SimulationListData& src, std::string& dst, eSerializationFormat f) const;
    bool SerializeToFile(pulse::study::bind::hydrocephalus::SimulationListData& src, const std::string& filename, eSerializationFormat f) const;
    bool SerializeFromString(const std::string& src, pulse::study::bind::hydrocephalus::SimulationListData& dst, eSerializationFormat f);
    bool SerializeFromFile(const std::string& filename, pulse::study::bind::hydrocephalus::SimulationListData& dst, eSerializationFormat f);

    void ControllerLoop();
    void FinalizeSimulation(pulse::study::bind::hydrocephalus::SimulationData& sim);
    pulse::study::bind::hydrocephalus::SimulationData* GetNextSimulation();

    std::mutex  m_mutex;
    bool m_Running;

    std::string m_OutDir;
    std::string m_DataDir;
    std::string m_SimulationResultsListFile;
    std::set<unsigned int> m_SimulationsToRun;
    std::vector<std::thread>   m_Threads;
    pulse::study::bind::hydrocephalus::SimulationListData* m_SimulationList;
    pulse::study::bind::hydrocephalus::SimulationListData* m_SimulationResultsList;

  private:
    // struct to hold new running average data
    struct RunningAverages
    {
      RunningAverages(const SELiquidCompartment* c, const PressureUnit& u)
      {
        cmpt = c;
        pUnit = &u;
      }
      RunningAverages(const SELiquidCompartment* c, const VolumePerTimeUnit& u)
      {
        cmpt = c;
        fUnit = &u;
      }
      RunningAverages(const SELiquidSubstanceQuantity* sq, const PressureUnit& u)
      {
        subQ = sq;
        pUnit = &u;
      }
      double instantaneousAverage = 0.;
      const SELiquidCompartment* cmpt = nullptr;
      const SELiquidSubstanceQuantity* subQ = nullptr;
      const PressureUnit* pUnit = nullptr;
      const VolumePerTimeUnit* fUnit = nullptr;
      SERunningAverage runningAverage;

      double Sample()
      {
        if (cmpt != nullptr)
        {
          if (fUnit != nullptr)
            return runningAverage.Sample(cmpt->GetInflow(*fUnit));
          else if (pUnit != nullptr)
            return runningAverage.Sample(cmpt->GetPressure(*pUnit));
        }
        else if (subQ != nullptr)
          return runningAverage.Sample(subQ->GetPartialPressure(*pUnit));
        return 0;// We should not get here
      }
    };
  };
}
