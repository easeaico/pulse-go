/* Distributed under the Apache License, Version 2.0.
See accompanying NOTICE file for details.*/

#pragma once

#include "cdm/PhysiologyEngineThunk.h"
#include "cdm/utils/ThreadPool.h"
#include <chrono>

#include "cdm/engine/SEDataRequested.h"
#include "cdm/engine/SEEngineInitialization.h"
#include "cdm/substance/SESubstanceManager.h"

class SEPhysiologyEnginePoolEngine : public Loggable
{
  friend class SEPhysiologyEnginePool;
  friend class PhysiologyEnginePoolThunk;
protected:
  SEPhysiologyEnginePoolEngine(Logger* logger = nullptr);

public:
  ~SEPhysiologyEnginePoolEngine();

  bool                              IsActive=false;
  SEEngineInitialization            EngineInitialization;
  std::vector<const SEAction*>      Actions;
  SEDataRequested                   DataRequested;
  std::unique_ptr<PhysiologyEngine> Engine;
};

class CDM_DECL SEPhysiologyEnginePool : public Loggable
{
  friend class PhysiologyEnginePoolThunk;
public:
  SEPhysiologyEnginePool(size_t poolSize = 0, const std::string& dataDir="./", Logger* logger = nullptr);
  ~SEPhysiologyEnginePool();

  const size_t GetWorkerCount() const { return m_Pool.workerCount(); }
  const size_t GetEngineCount() const { return m_Engines.size(); }

  bool RemoveEngine(int id);

  SEEngineInitializationStatus InitializeEngine(SEEngineInitialization& init);
  std::vector<SEEngineInitializationStatus> InitializeEngines(std::vector<SEEngineInitialization>& inits);

  // Advance all engines the same amount of time
  // If you want to advance individual engines/different times
  // Add an SEAdvanceTime action to the SEPhysiologyEnginePoolEngine Actions vector
  bool AdvanceModelTime(double time, const TimeUnit& unit);

  // Process the same action on all engines
  bool ProcessAction(const SEAction& action);

  // Process all the actions on each engine pool engine
  bool ProcessActions();

  void ClearDataRequested();// Call to clear out log/events after you PullData and examine it
                            // If not, the vector of events/logs can get large...
  void PullDataRequested(std::vector<SEDataRequested*>&);

protected:
  virtual void AllocateEngine(SEPhysiologyEnginePoolEngine& pe) = 0;

  SEPhysiologyEnginePoolEngine* CreateEngine(SEEngineInitialization& init, int id);
  static SEEngineInitializationStatus InitEngine(SEPhysiologyEnginePoolEngine* pe, int id);

  bool m_IsActive;
  SESubstanceManager m_SubMgr;
  std::map<int, SEPhysiologyEnginePoolEngine*> m_Engines;
  ThreadPool m_Pool;
  int m_NextID;
};

/**
 * An instance of Pulse where the interface is define in stl and base data types.
 * This interface is a thunk layer using serialized cdm objects to drive a Pulse engine.
 */
class CDM_DECL PhysiologyEnginePoolThunk
{
public:
  PhysiologyEnginePoolThunk(size_t poolSize = 0, const std::string& dataDir="./");
  virtual ~PhysiologyEnginePoolThunk();

  bool InitializeEngines(std::string const& engineInitializationList, eSerializationFormat format);

  bool RemoveEngine(int id);

  bool ProcessActions(std::string const& actions, eSerializationFormat format);

  std::string PullRequestedData(eSerializationFormat format);

private:
  class pimpl;
  pimpl* data;
};


/**
class EngineRunner
{
public:
    EngineRunner(std::shared_ptr<PulseEngine> engine);

    // So here is a tricky decision for each new live engine we can either 
    // keep running the engine on its own thread, as it should be running faster than realtime
    // there should be a good amount of sleeping going on this means that we should be able to 
    // handle a good number of running engines at the same time, 
    // ALTERNATIVELY, we could keep putting new tasks on the threadpool for each AdvanceModelTime
    // but we'd still have sleep the correct amount of time, not sure if that is an improvement

    void setAdvanceTime();
    void setRealTimeRate(); // Determine how many seconds of sim time per second of realtime should be processed

    // Starts calling AdvanceModelTime with the value AdvanceTime repeatedly so that 
    // engine sim time proceeds at the rate determined by setRealtimeRate 
    void run();

    void stop();    // Stops and joins can't call run afterwards

    void pause(bool doPause);

    /// Executes the function f the next time the engine exits from advanceModelTime 
    void doNext(std::function<bool(PulseEngine*)> f);

    /// Returns the amount of simulated time passed 
    std::chrono::duration<std::chrono::seconds> getSimulatedTime();

private:

    void processCallbacks()
    {
        std::lock_guard<std::mutex> lock(m_callbackMutex);
        std::for_each(m_callbacks.begin(), m_callbacks.end(), [this](auto i) {i(m_engine.get()); });
    }


    std::mutex m_callbackMutex;
    std::vector <std::function<bool(PulseEngine*)>> m_callbacks;

    std::chrono::seconds m_advanceTime;
    std::chrono::milliseconds m_elapsed;
    std::atomic<bool> m_stopped = false;

    std::shared_ptr<PulseEngine> m_engine;
};
*/
