/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "CircuitOptimizer.h"

#include "engine/human_adult/hemodynamics/Engine.h"
#include "engine/common/controller/CircuitManager.h"

#include "cdm/compartment/SECompartmentManager.h"
#include "cdm/compartment/fluid/SELiquidCompartment.h"
#include "cdm/engine/SEEventManager.h"
#include "cdm/engine/SEDataRequestTracker.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEPatientConfiguration.h"
#include "cdm/utils/DataTrack.h"

PUSH_EIGEN_WARNINGS
#include "Eigen/Dense"
POP_EIGEN_WARNINGS

namespace pulse::study::circuit_optimization
{

  class MyEventHandler : public SEEventHandler
  {
  public:
    MyEventHandler() : SEEventHandler() {}
   
    
  };

  CircuitOptimizer::CircuitOptimizer(Logger* logger) : Loggable(logger), m_SubMgr(logger)
  {
 
  }
  CircuitOptimizer::~CircuitOptimizer()
  {
  
  }

  void CircuitOptimizer::HandleEvent(eEvent type, bool active, const SEScalarTime*)
  {
    if (type == eEvent::StartOfCardiacCycle && active)
    {
      m_CardiacCycle++;
      if (m_CardiacCycle != m_StopAtCycle)
      {
        m_StartOfCardiacCycle = true;
      }
    }
  }

  bool CircuitOptimizer::ConvergeToHemodynamicsTargets(size_t maxLoops,
                                                       double stepRatio,
                                                       std::string& startModifierSet,
                                                       const std::string& dataRequestFile,
                                                       std::vector<SETimeSeriesValidationTarget*>& targets,
                                                       std::vector<std::string>& modifiers)
  {
    // Start with the default modifiers
    PulseConfiguration cfg(GetLogger());
    // Which modifier set do we want to start with?
    if (startModifierSet.empty())
      cfg.Initialize(); // Use the current defaults, or
    else
      cfg.SerializeFromFile(startModifierSet, m_SubMgr);// Use a file we previously generated

    int nTarget = (int)targets.size();
    Info("Number of modifiers: " + std::to_string(cfg.GetModifiers().size()));
    Info("Number of targets: " + std::to_string(nTarget));
    for (auto& [name, modifier] : cfg.GetModifiers())
      Info("Default modifier " + name + ": " + std::to_string(modifier.value));

    bool converged;
    std::string check;
    double previousErrorNorm  = 0;
    for (unsigned int i = 0; i < maxLoops; ++i)// Maximum loops
    {
      // 1. Generate data with current modifiers
      if (!GenerateHemodynamicsData(cfg, dataRequestFile, targets))
      {
        Fatal("Error generating data");
        return false;
      }
      // 2. Look at the error of each target
      //    (I am just making up convergence criteria)
      Info("Checking simulation results...[Expected] [Value] [Error]");
      converged = true;
      int nFail = 0;
      int nPass = nTarget;
      double errorNorm  = 0;
      for (SETimeSeriesValidationTarget* vt : targets)
      {
        check = "PASS ";
        if (vt->GetError() > 10.0)// Just a guess here...
        {
          check = "FAIL ";
          nFail++;
          nPass--;
          converged = false;
          Info(check + vt->GetHeader() +
               " [" + pulse::cdm::to_string(vt->GetTargetMinimum()) + ", " + pulse::cdm::to_string(vt->GetTargetMaximum()) + "] " +
               pulse::cdm::to_string(vt->GetComputedValue()) + " " + pulse::cdm::to_string(vt->GetError()) + "%");
        }
        errorNorm += vt->GetError() * vt->GetError();
      }
      Info("Total error norm (l2): " + std::to_string(errorNorm) +
           " Passed: " + std::to_string(nPass) +
           " Failed: " + std::to_string(nFail));
      
      // 3. Test convergence criteria and whether gradient vanishes
      if (converged || std::abs(errorNorm - previousErrorNorm) < 0.0001)
      {
        std::string modifierSetFilename = "./test_results/modifier_set_" +
                                          std::to_string((int)round(errorNorm)) + "_" +
                                          std::to_string(nPass) + "_" +
                                          std::to_string(nFail) + ".json";
        Info("Modifier set is saved to: " + modifierSetFilename);
        WriteModifiers(cfg, modifierSetFilename);
        if (converged)
        {
          Info("We have successfully tuned the circuit!");
          return true;
        }
        else
        {
          Info("Gradient vanishes, stop early.");
          return false;
        }
      }

      // 4. We did not converge, so compute a new modifier set to test
      previousErrorNorm = errorNorm;
      if (!ComputeNewModifiers(stepRatio, cfg, dataRequestFile, targets, modifiers))
        return false;

      Info("Finished Modifier Loop " + std::to_string(i+1));
    }

    // Check final results
    if (!GenerateHemodynamicsData(cfg, dataRequestFile, targets))
    {
      Fatal("Error generating data");
      return false;
    }
    Info("Checking simulation results...[Expected] [Value] [Error]");
    int nFail = 0;
    int nPass = nTarget;
    double errorNorm  = 0;
    for (SETimeSeriesValidationTarget* vt : targets)
    {
      check = "PASS ";
      if (vt->GetError() > 10.0)// Just a guess here...
      {
        check = "FAIL ";
        nFail++;
        nPass--;
      }
      Info(check + vt->GetHeader() +
           " [" + pulse::cdm::to_string(vt->GetTargetMinimum()) + ", " + pulse::cdm::to_string(vt->GetTargetMaximum()) + "] " +
           pulse::cdm::to_string(vt->GetComputedValue()) + " " + pulse::cdm::to_string(vt->GetError()) + "%");
      errorNorm += vt->GetError() * vt->GetError();
    }
    Info("Final total error norm (l2): " + std::to_string(errorNorm) +
         " Passed: " + std::to_string(nPass) +
         " Failed: " + std::to_string(nFail));
    std::string modifierSetFilename = "./test_results/modifier_set_" +
                                      std::to_string((int)round(errorNorm)) + "_" +
                                      std::to_string(nPass) + "_" +
                                      std::to_string(nFail) + ".json";
    Info("Modifier set is saved to: " + modifierSetFilename);
    WriteModifiers(cfg, modifierSetFilename);
    return false;
  }

  bool CircuitOptimizer::GenerateHemodynamicsData(PulseConfiguration& cfg,
    const std::string& dataRequestFile, std::vector<SETimeSeriesValidationTarget*>& targets)
  {
    pulse::human_adult_hemodynamics::Engine engine(GetLogger());

    SEPatientConfiguration pCfg(engine.GetLogger());
    pCfg.SetPatientFile("./patients/StandardMale.json");

    if (!engine.SetConfigurationOverride(&cfg))
    {
      Error("Unable to override configuration");
      return false;
    }

    SEDataRequestManager drMgr(engine.GetLogger());
    drMgr.SerializeDataRequestsFromFile(dataRequestFile);

    if (!engine.InitializeEngine(pCfg, &drMgr))
    {
      Error("Unable to initialize engine");
      return false;
    }

    // Listen for Cardiac Cycles
    engine.GetEventManager().ForwardEvents(this);

    // Setup Data Requests
    SEDataRequest* dr;
    std::map<SETimeSeriesValidationTarget*, SEDataRequest*> vTgt2dr;
    //engine.GetEngineTracker().SetTrackMode(TrackMode::Dynamic); // No file needed
    for (SETimeSeriesValidationTarget* vt : targets)
    {
      // Reset the computed data in our targets
      vt->GetData().clear();
      // Cache off the mapping of our target to this engine
      dr = drMgr.FindDataRequest(vt->GetHeader());
      if (dr == nullptr)
      {
        Error("Unable to find data request for "+vt->GetHeader());
        return false;
      }
      vTgt2dr[vt] = dr;
    }

    // Advance for 5 Cardiac Cycles
    m_CardiacCycle = 0;
    m_StartOfCardiacCycle = false;
    while (m_CardiacCycle < m_StopAtCycle)
    {
      if (!engine.AdvanceModelTime())
      {
        Error("Unable to advance time");
        return false;
      }
      for (SETimeSeriesValidationTarget* vt : targets)
      {
        if (m_StartOfCardiacCycle)
          vt->GetData().clear();
        vt->GetData().push_back(engine.GetDataRequestTracker().GetValue(*vTgt2dr[vt]));
      }
      m_StartOfCardiacCycle = false;
    }
    for (SETimeSeriesValidationTarget* vt : targets)
    {
      if (!vt->ComputeError())
      {
        Error("Check target dataset");
        return false;
      }
    }
    Info("Finished simulation " +std::to_string(++m_SimulationNum));

    return true;
  }
  bool CircuitOptimizer::ComputeNewModifiers(double stepRatio,
                                             PulseConfiguration& cfg,
                                             const std::string& dataRequestFile,
                                             std::vector<SETimeSeriesValidationTarget*>& targets,
                                             std::vector<std::string>& modifiers)
  {
    // TODO: No bounding constraint has been imposed yet.
    auto nX = (int)modifiers.size();
    auto nY = (int)targets.size();
    double finiteDiferenceStepSize = 0.0001; // step size for finite differencing

    // Diff/Error/Y matrix for output targets
    Eigen::MatrixXd diffY(nY, 1);
    for (int i = 0; i < (int)targets.size(); i++)
      diffY.coeffRef(i, 0) = targets[i]->GetError();

    // Jacobian matrix
    Eigen::MatrixXd Jacobian(nY, nX);

    // Compute entries in Jacobian matrix
    // Individually step each modifier and compute
    int i = 0;
    for (auto& name : modifiers)
    {
      // Step one modifier and compute
      double val = cfg.GetModifiers()[name].value; // cache it
      cfg.GetModifiers()[name].value = val + finiteDiferenceStepSize;
      if (!GenerateHemodynamicsData(cfg, dataRequestFile, targets)) // TODO this should be a function pointer
      {
        Fatal("Error generating data");
        return false;
      }

      // Put all target errors for this one modifier step in our matrix
      int j = 0;
      for (SETimeSeriesValidationTarget* vt : targets)
      {
        Jacobian.coeffRef(j, i)  = (vt->GetError() - diffY.coeffRef(j, 0)) / finiteDiferenceStepSize;
        j++;
      }

      // Reset the modifier from the cache
      cfg.GetModifiers()[name].value = val;
      i++;
    }

    // Least squares fitting using uniform weights.
    // Assume number of targets is equal or greater than the number of input variables/modifiers,
    // otherwise J^T * J is non-invertible, and we will need to use other solution method (e.g. null-space method)
    Eigen::MatrixXd deltaX = (Jacobian.transpose() * Jacobian).inverse() * (Jacobian.transpose() * diffY); //

    // Put the new modifier set into the config
    i = 0;
    for (auto& name : modifiers)
    {
      double val = cfg.GetModifiers()[name].value;
      cfg.GetModifiers()[name].value = val - (stepRatio * deltaX.coeff(i, 0));
      i++;
    }
    return true;
  }

  void CircuitOptimizer::WriteModifiers(const PulseConfiguration& cfg, const std::string& filename)
  {
    PulseConfiguration out;
    // We only want the modifiers from the given cfg
    out.GetModifiers() = cfg.GetModifiers();
    out.SerializeToFile(filename);
  }
}
