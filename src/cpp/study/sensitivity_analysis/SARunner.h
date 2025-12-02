/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

#include <thread>
#include "PulseEngine.h"

#include "cdm/patient/SEPatient.h"
#include "cdm/patient/actions/SEDyspnea.h"
#include "cdm/patient/actions/SEIntubation.h"
#include "cdm/patient/actions/SEImpairedAlveolarExchangeExacerbation.h"
#include "cdm/patient/actions/SEPulmonaryShuntExacerbation.h"
#include "cdm/system/equipment/mechanical_ventilator/SEMechanicalVentilator.h"
#include "cdm/system/equipment/mechanical_ventilator/actions/SEMechanicalVentilatorConfiguration.h"
#include "cdm/engine/SEEventManager.h"
#include "cdm/engine/SEOverrides.h"
#include "cdm/engine/SEDataRequestTracker.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/substance/SESubstanceManager.h"
#include "cdm/substance/SESubstanceFraction.h"
#include "cdm/substance/SESubstanceTransport.h"
#include "cdm/circuit/fluid/SEFluidCircuitCalculator.h"
#include "cdm/circuit/fluid/SEFluidCircuit.h"
#include "cdm/circuit/fluid/SEFluidCircuitNode.h"
#include "cdm/circuit/fluid/SEFluidCircuitPath.h"
#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/compartment/fluid/SEGasCompartmentGraph.h"
#include "cdm/compartment/fluid/SEGasCompartmentGraph.h"
#include "cdm/compartment/fluid/SELiquidCompartmentGraph.h"
#include "cdm/properties/SERunningAverage.h"
#include "cdm/properties/SEScalar0To1.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMass.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarMassPerVolume.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/properties/SEScalarVolume.h"
#include "cdm/properties/SEScalarVolumePerPressure.h"
#include "cdm/utils/DataTrack.h"
#include "cdm/utils/FileUtils.h"
#include "cdm/utils/GeneralMath.h"
#include "cdm/utils/TimingProfile.h"

#include "io/protobuf/PBProperties.h"
PUSH_PROTO_WARNINGS
#include "pulse/study/bind/SensitivityAnalysis.pb.h"
#include <google/protobuf/text_format.h>
#include <google/protobuf/util/json_util.h>
POP_PROTO_WARNINGS

namespace pulse::study::sensitivity_analysis
{

  class SARunner : public Loggable
  {
  public:
    SARunner(const std::string& logfileName, const std::string& dataDir = ".");
    virtual ~SARunner();

    bool Run(const std::string& filename, eSerializationFormat f);
    bool Run(pulse::study::bind::sensitivity_analysis::SimulationListData& simList);

    bool RunSimulationUntilStable(std::string const& outDir, pulse::study::bind::sensitivity_analysis::SimulationData& sim, const std::string& dataDir = "./");
    bool RunSimulation(std::string const& outDir, pulse::study::bind::sensitivity_analysis::SimulationData& sim, const std::string& dataDir = "./");

  protected:
    bool Run();
    bool SerializeToString(pulse::study::bind::sensitivity_analysis::SimulationListData& src, std::string& dst, eSerializationFormat f) const;
    bool SerializeToFile(pulse::study::bind::sensitivity_analysis::SimulationListData& src, const std::string& filename, eSerializationFormat f) const;
    bool SerializeFromString(const std::string& src, pulse::study::bind::sensitivity_analysis::SimulationListData& dst, eSerializationFormat f);
    bool SerializeFromFile(const std::string& filename, pulse::study::bind::sensitivity_analysis::SimulationListData& dst, eSerializationFormat f);

    void ControllerLoop();
    void FinalizeSimulation(pulse::study::bind::sensitivity_analysis::SimulationData& sim);
    pulse::study::bind::sensitivity_analysis::SimulationData* GetNextSimulation();

    std::mutex  m_mutex;
    bool m_Running;

    std::string m_OutDir;
    std::string m_DataDir;
    std::string m_SimulationResultsListFile;
    std::set<unsigned int> m_SimulationsToRun;
    std::vector<std::thread>   m_Threads;
    pulse::study::bind::sensitivity_analysis::SimulationListData* m_SimulationList;
    pulse::study::bind::sensitivity_analysis::SimulationListData* m_SimulationResultsList;

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
            return runningAverage.Sample(cmpt->GetInFlow(*fUnit));
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
