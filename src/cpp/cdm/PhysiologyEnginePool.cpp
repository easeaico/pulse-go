/* Distributed under the Apache License, Version 2.0.
See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/PhysiologyEnginePool.h"
#include "cdm/engine/SEActionManager.h"
#include "cdm/engine/SEAdvanceTime.h"
#include "cdm/engine/SEDataRequested.h"
#include "cdm/engine/SEDataRequestManager.h"
#include "cdm/engine/SEEngineTracker.h"
#include "cdm/engine/SEPatientConfiguration.h"

namespace
{
  bool GatherResults(std::vector<std::future<bool>>& futures)
  {
    bool result = true;
    for (auto& f : futures)
        result &= f.get();
    return result;
  }
}

SEPhysiologyEnginePoolEngine::SEPhysiologyEnginePoolEngine(Logger* logger) : Loggable(logger)
{

};
SEPhysiologyEnginePoolEngine::~SEPhysiologyEnginePoolEngine()
{

}

SEPhysiologyEnginePool::SEPhysiologyEnginePool(size_t poolSize, const std::string& dataDir, Logger* logger) : Loggable(logger),
                                                                                            m_SubMgr(logger), m_Pool(poolSize)
{
  m_IsActive = false;
  m_NextID = 0;
  m_SubMgr.LoadSubstanceDirectory(dataDir);
}
SEPhysiologyEnginePool::~SEPhysiologyEnginePool()
{
  DELETE_MAP_SECOND(m_Engines);
}

bool SEPhysiologyEnginePool::RemoveEngine(int id)
{
  if (m_Engines.find(id) == m_Engines.end())
    return false;
  m_Engines.erase(id);
  return true;
}

SEPhysiologyEnginePoolEngine* SEPhysiologyEnginePool::CreateEngine(SEEngineInitialization& init, int id)
{
  SEPhysiologyEnginePoolEngine* pe = new SEPhysiologyEnginePoolEngine(m_Logger);
  AllocateEngine(*pe);
  pe->DataRequested.SetEngine(*pe->Engine);
  //pe->EngineInitialization.Copy(init, m_SubMgr);
  pe->Engine->GetLogger()->LogToConsole(false);
  m_Engines.insert({ id, pe });

  return pe;
}

SEEngineInitializationStatus SEPhysiologyEnginePool::InitEngine(SEPhysiologyEnginePoolEngine* pe, int id)
{
  pe->IsActive = false;
  PhysiologyEngine* engine = pe->Engine.get();
  if (!init)
  {
    pe->EngineInitializationStatus.Created(pe->IsActive);
    engine->GetLogger()->AddForward(&pe->EngineInitializationStatus);
    engine->GetLogger()->Warning("Recieved nullptr engine initialization object");
    return;
  }

  if(init->HasLogFilename())
    engine->GetLogger()->SetLogFile(init->GetLogFilename());
  // Patient/State
  if (init->HasPatientConfiguration())
    pe->IsActive = engine->InitializeEngine(init->GetPatientConfiguration());
  else if (init->HasStateFilename())
    pe->IsActive = engine->SerializeFromFile(init->GetStateFilename());
  else if (init->HasState())
    pe->IsActive = engine->SerializeFromString(init->GetState(), init->GetStateFormat());

  // Data Requests
  if (init->HasDataRequestManager())
    engine->GetEngineTracker()->GetDataRequestManager().Copy(init->GetDataRequestManager());

  // Events
  pe->DataRequested.KeepEventChanges(init->KeepEventChanges());
  engine->GetEventManager().ForwardEvents(&pe->DataRequested);

  // Logging
  pe->DataRequested.KeepLogMessages(init->KeepLogMessages());
  pe->EngineInitializationStatus.KeepLogMessages(init->KeepLogMessages());
  engine->GetLogger()->AddForward(&pe->DataRequested);
  engine->GetLogger()->AddForward(&pe->EngineInitializationStatus);

  // Set Results Active
  pe->DataRequested.SetIsActive(pe->IsActive);
  if (!pe->IsActive)
    pe->Warning("Failed to initialize engine "+std::to_string(pe->EngineInitializationStatus.GetID()));
  SEEngineInitializationStatus status;
  status.SetID(id);
  status.Created(pe->IsActive);

  return status;
}

SEEngineInitializationStatus SEPhysiologyEnginePool::InitializeEngine(SEEngineInitialization& init)
{
  int id = m_NextID++;
  SEPhysiologyEnginePoolEngine* pe = CreateEngine(init, id);
  return InitEngine(pe, id);
}

std::vector<SEEngineInitializationStatus> SEPhysiologyEnginePool::InitializeEngines(std::vector<SEEngineInitialization>& inits)
{
  std::vector<std::future<void>> futures;
  std::vector<SEEngineInitializationStatus*> statuses;
  for (const auto ei : inits)
  {
    int id = m_NextID++;
    SEPhysiologyEnginePoolEngine* pe = CreateEngine(id);
    statuses.push_back(&pe->EngineInitializationStatus);
    futures.push_back(m_Pool.enqueue(InitEngine, pe, ei));
  }

  for (unsigned int i = 0; i < futures.size(); ++i)
    futures[i].wait();

  return statuses;
}

bool SEPhysiologyEnginePool::AdvanceModelTime(double time, const TimeUnit& unit)
{
  SEAdvanceTime adv;
  adv.GetTime().SetValue(time, unit);
  return ProcessAction(adv);
}
bool SEPhysiologyEnginePool::ProcessAction(const SEAction& action)
{
  for (auto& itr : m_Engines)
    itr.second->Actions.push_back(&action);
  return ProcessActions();
}
bool SEPhysiologyEnginePool::ProcessActions()
{
  if (!m_IsActive)
  {
    Error("Engine pool is not active");
    return false;
  }

  std::vector<std::future<bool>> futures;
  for (auto itr : m_Engines)
  {
    SEPhysiologyEnginePoolEngine* pe = itr.second;
    if (!pe->IsActive)
    {
      pe->Actions.clear();
      continue;
    }

    futures.push_back(m_Pool.enqueue([pe]()
    {
      bool f = true;
      for (const SEAction* a : pe->Actions)
        f &= pe->Engine->ProcessAction(*a);
      pe->Actions.clear();
      return f;
    }));
  }
  return  GatherResults(futures);
}

void SEPhysiologyEnginePool::ClearDataRequested()
{
  for (auto engine : m_Engines)
  {
    engine.second->DataRequested.ClearDataRequested();
  }
}
void SEPhysiologyEnginePool::PullDataRequested(std::vector<SEDataRequested*>& dataRequested)
{
  // TODO the enine now has the data requested tracking in it... how do I get to it?
  //dataRequested.clear();
  //for (auto engine : m_Engines)
  //{
  //  if (engine.second->IsActive)
  //    engine.second->DataRequested.PullDataRequested();
  //   dataRequested.push_back(&engine.second->DataRequested);
  //   engine.second->IsActive = engine.second->DataRequested.IsActive();
  //}
}

///////////////////////////////
// PhysiologyEnginePoolThunk //
///////////////////////////////

class PhysiologyEnginePoolThunk::pimpl
{
public:
  pimpl()
  {

  }
  ~pimpl()
  {
    
  }

  SEPhysiologyEnginePool* pool;
  std::map<int, std::vector<const SEAction*>> actionMap;
  std::vector<SEDataRequested*> dataRequested;
};
PhysiologyEnginePoolThunk::PhysiologyEnginePoolThunk(size_t poolSize, const std::string& dataDir)
{
  data = new PhysiologyEnginePoolThunk::pimpl();
  std::cout << "Creating a pool with " << data->pool->GetWorkerCount() << " threads. \n";
}
PhysiologyEnginePoolThunk::~PhysiologyEnginePoolThunk()
{

}

bool PhysiologyEnginePoolThunk::InitializeEngines(std::string const& engineInitializationList, eSerializationFormat format)
{
  std::vector<SEEngineInitialization*> engines;
  if (!SEEngineInitialization::SerializeFromString(engineInitializationList, engines, format, data->pool->m_SubMgr))
  {
    data->pool->Error("Unable to serialize engine_initializations string");
    return false;
  }

  // TODO: Fix this mess
  std::vector<SEEngineInitialization> eis;
  for (auto e: engines)
    eis.push_back(*e);
    
  data->pool->InitializeEngines(eis);

  // TODO: Fix return
  return true;
}

bool PhysiologyEnginePoolThunk::RemoveEngine(int id)
{
  return data->pool->RemoveEngine(id);
}

bool PhysiologyEnginePoolThunk::ProcessActions(std::string const& actions, eSerializationFormat format)
{
  SEActionManager::SerializeFromString(actions, data->actionMap, format, data->pool->m_SubMgr);
  // TODO: Actions
  /*for (auto itr : data->actionMap)
  {
    for (const SEAction* a : itr.second)
    {
      data->pool->GetEngine(itr.first)->Actions.push_back(a);
    }
  }*/
  return data->pool->ProcessActions();
}

std::string PhysiologyEnginePoolThunk::PullRequestedData(eSerializationFormat format)
{
  std::string dst;
  data->pool->PullDataRequested(data->dataRequested);
  SEDataRequested::SerializeToString(data->dataRequested, dst, format);
  data->pool->ClearDataRequested();
  return dst;
}


/////////////////////////////////////////////////////////////////////////////////
// Simulation Implementation
///

/**
PulseEngineSimPool::PulseEngineSimPool(size_t poolSize) :
    m_pool(poolSize)
{
}

std::shared_ptr<EngineRunner> PulseEngineSimPool::add(std::string patientFile)
{
    auto engine = std::make_shared<PulseEngine>();
    if (engine->SerializeFromFile(patientFile)) {
        return std::make_shared<EngineRunner>(engine);
    }
    return nullptr;
}

EngineRunner::EngineRunner(std::shared_ptr<PulseEngine> engine) :
    m_engine(engine),
    m_advanceTime(1)
{

}

void EngineRunner::run()
{
    while (m_stopped)
    {
        auto start = std::chrono::high_resolution_clock::now();
        m_engine->AdvanceModelTime(static_cast<double>(m_advanceTime.count()), TimeUnit::s);
        processCallbacks();
        auto elapsed = std::chrono::high_resolution_clock::now() - start;

        if ((m_advanceTime - elapsed).count() > 0)
        {
            std::this_thread::sleep_for(m_advanceTime - elapsed);
        }
        else {
            // Warn processing did take too long
        }
    }
}

void EngineRunner::doNext(std::function<bool(PulseEngine*)> f)
{
    std::lock_guard<std::mutex> lock(m_callbackMutex);
    m_callbacks.push_back(f);
}
*/
