/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#include "EngineHowTo.h"
#include "PulseEngine.h"

#include "cdm/system/physiology/SECardiovascularSystem.h"
#include "cdm/system/physiology/SERespiratorySystem.h"
#include "cdm/properties/SEScalarFrequency.h"
#include "cdm/properties/SEScalarTime.h"
#include "cdm/utils/TimingProfile.h"
#include "cdm/Macros.h"

#include <thread>
#include <atomic>
#include <chrono>


class SystemLoadTest
{
public:
  SystemLoadTest(size_t tId) : thread_id(tId) {}
  virtual ~SystemLoadTest() { DELETE_VECTOR(engines); }

  size_t thread_id;
  size_t engine_count = 4;
  std::thread thread;
  std::vector<PhysiologyEngine*> engines;
  std::atomic<bool> data_ready{ false };
  std::atomic<bool> wait{ false };
  std::atomic<bool> active{ true };

  void allocate_engines()
  {
    do
    {
      // Make the engines to test
      std::unique_ptr<PhysiologyEngine> pe = CreatePulseEngine();
      pe->GetLogger()->SetLogFile("./test_results/Pulse_t"+std::to_string(thread_id)+"_of_"+std::to_string(engines.size())+".log");
      if (!pe->SerializeFromFile("./states/StandardMale@0s.json"))
      {
        pe->GetLogger()->Error("Could not load state, check the error");
        return;
      }
      engines.push_back(pe.get());
    } while (engines.size() < engine_count);
  }

  void time_engines()
  {
    while (active)
    {
      wait = true;// Sleep until something flips wait to false
      do { std::this_thread::sleep_for(std::chrono::microseconds(20)); } while (wait);
      if (!active) break;
      wait = true;
     
      std::cout << "Running\n";
      
      for (auto pe : engines)
       pe->AdvanceModelTime(20, TimeUnit::s);
      // Store data or something
      data_ready = true;
    }
  }

};



//--------------------------------------------------------------------------------------------------
/// \brief
/// A place to do experiment with Pulse
///
/// \details
/// This is a good place to code up something that demonstrates an issue for reproduction and debugging
//--------------------------------------------------------------------------------------------------
void HowToTestSystemCapability()
{
  bool ready;
  size_t total_engines;
  TimingProfile profiler;
  size_t sim_time_s = 20;
  

  size_t num_threads = 6;
  std::vector<SystemLoadTest*> slts;
  for (size_t t = 0; t < num_threads; t++)
  {
    slts.push_back(new SystemLoadTest(t));
    slts[t]->thread=std::thread(&SystemLoadTest::time_engines, slts[t]);
  }

  do
  {
    // Allocate engines and wait for the signal
    // Increase the number of engines
    for (auto slt : slts)
    {
      slt->engine_count++;
      slt->allocate_engines();
    }
    total_engines = num_threads * slts[0]->engine_count;

    profiler.Start("s");
    do
    {// Wait for all threads to be ready
      ready = true;
      for(auto slt : slts)
        ready &= slt->wait;
      if (ready)
        break;
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    } while (true);
    // Start all slts to advancing time
    for (auto slt : slts)
      slt->wait = false;
    // Wait for each slt to finish
    do
    {
      ready = true;
      for (auto slt : slts)
        ready &= slt->data_ready;
      if (ready)
        break;
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    while (true);
    for (auto slt : slts)
    {
      // PULL DATA
      slt->data_ready = false;
    }
    double elapsed_time_s = profiler.GetElapsedTime_s("s");

    // Are we still running real time?
    std::cout << total_engines << " (" << num_threads << " threads with " << slts[0]->engine_count << " engines each), needed " << elapsed_time_s << "s to simulate " << sim_time_s << "s\n";
    if (elapsed_time_s > sim_time_s)
    {
      std::cout << "Pulse failed to meet real-time execution\n";
      for (auto slt : slts)
        slt->active = false;
      break;
    }

  } while (true);
  
  std::cout << "Shutting down...\n";
  for (auto slt : slts)
  {
    slt->active = false;
    slt->wait = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    slt->thread.join();
    delete slt;
  }
  
}