/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Pulse.CDM;

namespace Pulse
{
  public enum eModelType : int
  {
    HumanAdultWholeBody = 0,
    HumanAdultVentilationMechanics,
    HumanAdultHemodynamics
  }

  // C# class that wraps the PulseC API
  // PulseEngineBase class has all the logic using the C# API
  // PulseEngine is a simple extension that connects those methods to the C API methods
  // This allows us various native layers to reuse the same C# base class
  // Unity iOS requires C# to connect to native methods via their own attribute compiler

  public abstract class PulseEngineBase
  {
    protected bool alive = false;
    protected IntPtr pulse_cptr;                  // Pointer to the pulse engine in C
    protected double[] data_values;               // Data coming back from the engine
    protected double time_step_s = 0;             // Time step (Updates in ctor)
    protected double extra_time_s = 0;            // Time remainder from spliting requested time into time steps
    protected static string version = "";         // Pulse Version
    protected static string hash = "";            // Git Hash of the pulse repository of this build
    protected ILogListener log_listener = null;   // Forward log messages to this object
    protected SELogMessages log_messages = new SELogMessages();
    protected IEventHandler event_handler = null; // Forware events to this object
    protected List<SEEventChange> event_changes = new List<SEEventChange>();
    protected IntPtr str_addr;                    // Used to hold data between C# and C
    protected eSerializationFormat thunk_as = eSerializationFormat.JSON;
    // TODO when we allow binary thunking, add many if/switchs below!

    public PulseEngineBase(eModelType m = eModelType.HumanAdultWholeBody, string data_dir = "./")
    {
      pulse_cptr = BaseAllocate((int)m, data_dir);
      time_step_s = BaseGetTimeStep("s");
    }
    ~PulseEngineBase()
    {
      BaseDeallocate();
    }

    protected abstract IntPtr BaseAllocate(int model, string data_dir);

    protected abstract void BaseDeallocate();

    protected abstract void BaseClear();
    public void Clear()
    {
      BaseClear();
    }

    protected abstract double BaseGetTimeStep(string unit);
    public double GetTimeStep_s()
    {
      return time_step_s;
    }

    protected abstract bool BaseSerializeFromFile(string filename, string data_mgr, int data_mgr_format);
    public bool SerializeFromFile(string filename, SEDataRequestManager data_mgr)
    {
      data_values = new double[data_mgr.GetDataRequests().Count + 1];
      string data_mgr_str = PBDataRequest.SerializeToString(data_mgr, eSerializationFormat.JSON);
      alive = BaseSerializeFromFile(filename, data_mgr_str, (int)eSerializationFormat.JSON);
      if (!alive) PullLogMessages();
      return alive;
    }

    protected abstract bool BaseSerializeToFile(string filename);
    public bool SerializeToFile(string filename)
    {
      if (!alive)
        return false;
      return BaseSerializeToFile(filename);
    }

    protected abstract bool BaseSerializeFromString(string state, string data_mgr, int format);
    public bool SerializeFromString(string state, SEDataRequestManager data_mgr, eSerializationFormat format)
    {
      data_values = new double[data_mgr.GetDataRequests().Count + 1];
      string data_mgr_str = PBDataRequest.SerializeToString(data_mgr, format);
      alive = BaseSerializeFromString(state, data_mgr_str, (int)format);
      if (!alive) PullLogMessages();
      return alive;
    }

    protected abstract bool BaseSerializeToString(int format, out IntPtr state_str);
    public string SerializeToString(eSerializationFormat format)
    {
      if (!alive)
        return null;
      if (!BaseSerializeToString((int)format, out str_addr))
        return null;
      string state_str = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(str_addr);
      return state_str;
    }

    protected abstract bool BaseInitializeEngine(string patient_configuration, string data_mgr, int thunk_format);
    public bool InitializeEngine(SEPatientConfiguration patient_configuration, SEDataRequestManager data_mgr=null)
    {
      string data_mgr_str = "";
      string patient_configuration_str = PBPatientConfiguration.SerializeToString(patient_configuration);

      if (data_mgr != null)
      {
        data_values = new double[data_mgr.GetDataRequests().Count + 1];
        data_mgr_str = PBDataRequest.SerializeToString(data_mgr, thunk_as);
      }
      alive = BaseInitializeEngine(patient_configuration_str, data_mgr_str, (int)thunk_as);
      if (!alive) PullLogMessages();
      return alive;
    }

    protected abstract bool BaseGetInitialPatient(int format, out IntPtr initial_patient);
    public bool GetInitialPatient(SEPatient p)
    {
      if (!alive)
        return false;
      if (!BaseGetInitialPatient((int)thunk_as, out str_addr))
        return false;
      string patient = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(str_addr);
      if (patient == null)
        return false;
      PBPatient.SerializeFromString(patient, p);
      return true;
    }

    protected abstract bool BaseGetConditions(int format, out IntPtr conditions);
    public bool GetConditions(List<SECondition> conditions)
    {
      if (!alive)
        return false;
      if (!BaseGetConditions((int)thunk_as, out str_addr))
        return false;
      string cList = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(str_addr);
      if (cList == null)
        return false;
      PBCondition.SerializeFromString(cList, conditions);
      return true;
    }

    protected abstract bool BaseGetPatientAssessment(int type, int format, out IntPtr assessment);
    public bool GetPatientAssessment(SEPatientAssessment assessment)
    {
      if (!alive)
        return false;
      int type;
      if (assessment is SEArterialBloodGasTest)
        type = 0;
      else if (assessment is SECompleteBloodCount)
        type = 1;
      else if (assessment is SEComprehensiveMetabolicPanel)
        type = 2;
      else if (assessment is SEUrinalysis)
        type = 3;
      else
        return false;

      if (!BaseGetPatientAssessment(type, (int)thunk_as, out str_addr))
        return false;
      string a = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(str_addr);
      if (a == null)
        return false;
      PBPatientAssessmets.SerializeFromString(a, assessment);
      return true;
    }

    protected abstract void BaseLogToConsole(bool b);
    public void LogToConsole(bool b)
    {
      BaseLogToConsole(b);
    }

    protected abstract void BaseSetLogFilename(string filename);
    public void SetLogFilename(string filename)
    {
      BaseSetLogFilename(filename);
    }

    public void SetLogListener(ILogListener listener)
    {
      log_listener = listener;
      BaseKeepLogMessages(log_listener != null);
    }
    protected abstract void BaseKeepLogMessages(bool save);

    protected abstract bool BasePullLogMessages(int format, out IntPtr event_changes);
    protected void PullLogMessages()
    {
      // Grab log messages and pass them to handler
      if (log_listener != null)
      {
        log_messages.Clear();
        if (BasePullLogMessages((int)thunk_as, out str_addr))
        {
          string log_messages_str = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(str_addr);
          if (log_messages_str != null)
          {
            PBLogMessages.SerializeFromString(log_messages_str, log_messages);
            log_messages.Forward(log_listener);
          }
        }
      }
    }

    protected abstract void BaseKeepEventChanges(bool keep);

    public void SetEventHandler(IEventHandler handler)
    {
      event_handler = handler;
      BaseKeepEventChanges(event_handler != null);
    }

    protected abstract bool BasePullEvents(int format, out IntPtr event_changes);

    protected abstract bool BasePullActiveEvents(int format, out IntPtr active_events);
    public bool PullActiveEvents(List<SEActiveEvent> active_events)
    {
      if (!alive)
        return false;
      active_events.Clear();
      if (!BasePullActiveEvents((int)thunk_as, out str_addr))
        return false;
      string activities = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(str_addr);
      if (activities == null)
        return false;
      PBEvents.SerializeFromString(activities, active_events);
      return true;
    }

    protected abstract bool BaseProcessActions(string any_action_list, int format);
    public bool ProcessAction(SEAction action)
    {
      if (!alive)
        return false;
      List<SEAction> actions = new List<SEAction>();
      actions.Add(action);
      return ProcessActions(actions);
    }
    public bool ProcessActions(List<SEAction> actions)
    {
      if (!alive)
        return false;
      string any_action_list_str = PBAction.SerializeToString(actions);
      //System.Console.Out.WriteLine(any_action_list_str);
      if (!BaseProcessActions(any_action_list_str, (int)thunk_as))
      {
        PullLogMessages();
        return false;
      }
      return true;
    }

    protected abstract bool BasePullActiveActions(int format, out IntPtr actions);
    public bool PullActiveActions(List<SEAction> actions)
    {
      if (!alive)
        return false;
      if (!BasePullActiveActions((int)thunk_as, out str_addr))
        return false;
      string aList = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(str_addr);
      if (aList == null)
        return false;
      PBAction.SerializeFromString(aList, actions);
      return true;
    }

    protected abstract bool BaseAdvanceTimeStep();
    public bool AdvanceTimeStep()
    {
      if (!alive)
        return false;
      bool b = BaseAdvanceTimeStep();
      // Grab events and pass them to handler
      if (event_handler != null)
      {
        event_changes.Clear();
        if (BasePullEvents((int)thunk_as, out str_addr))
        {
          string event_changes_str = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(str_addr);
          if (event_changes_str != null)
          {
            PBEvents.SerializeFromString(event_changes_str, event_changes);
            foreach (var change in event_changes)
            {
              event_handler.HandleEvent(change);
            }
          }
        }
      }
      PullLogMessages();

      return b;
    }
    public bool AdvanceTime_s(double duration)
    {
      if (!alive)
        return false;
      duration += extra_time_s;
      int steps = (int)Math.Floor(duration / time_step_s);
      extra_time_s = duration - (steps * time_step_s);
      for (int i = 0; i < steps; i++)
      {
        if (!AdvanceTimeStep())
          return false;
      }
      return true;
    }

    protected abstract IntPtr BasePullData();
    public double[] PullData()
    {
      if (!alive)
        return null;
      IntPtr data = BasePullData();
      Marshal.Copy(data, data_values, 0, data_values.Length);
      return data_values;
    }

    public static bool AreCompatibleUnits(string fromUnit, string toUnit) { throw new NotImplementedException(); }

    public static double ConvertValue(double value, string fromUnit, string toUnit) { throw new NotImplementedException(); }
  }

  // This is pretty boiler plate and should not change
  // Just copy this entire class and change the DllImport Attribute as needed
  public class PulseEngine : PulseEngineBase
  {
    // You can change the name of our Pulse library (PulseC or PulseCd)
    private const string Attribute = "PulseC";

    public PulseEngine(eModelType m = eModelType.HumanAdultWholeBody, string data_dir = "./") : base(m,data_dir){ }

    [DllImport(Attribute)]
    private static extern void PulseVersion(out IntPtr version_str);
    public static string Version()
    {
      if (version.Length == 0)
      {
        IntPtr str_addr;
        PulseVersion(out str_addr);
        version = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(str_addr);
      }
      return version;
    }

    [DllImport(Attribute)]
    private static extern void PulseHash(out IntPtr version_str);
    public static string Hash()
    {
      if (hash.Length == 0)
      {
        IntPtr str_addr;
        PulseHash(out str_addr);
        hash = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(str_addr);
      }
      return hash;
    }

    [DllImport(Attribute)]
    private static extern IntPtr Allocate(int model, string data_dir);
    protected override IntPtr BaseAllocate(int model, string data_dir)
    {
      return Allocate(model, data_dir);
  }

    [DllImport(Attribute)]
    private static extern void Deallocate(IntPtr pulse);
    protected override void BaseDeallocate()
    {
      Deallocate(pulse_cptr);
    }

    [DllImport(Attribute)]
    private static extern void Clear(IntPtr pulse);
    protected override void BaseClear()
    {
      Clear(pulse_cptr);
    }

    [DllImport(Attribute)]
    private static extern double GetTimeStep(IntPtr pulse,string unit);
    protected override double BaseGetTimeStep(string unit)
    {
      return GetTimeStep(pulse_cptr, unit);
    }

    [DllImport(Attribute)]
    private static extern bool SerializeFromFile(IntPtr pulse, string filename, string data_mgr, int data_mgr_format);
    protected override bool BaseSerializeFromFile(string filename, string data_mgr, int data_mgr_format)
    {
      return SerializeFromFile(pulse_cptr, filename, data_mgr, data_mgr_format);
    }

    [DllImport(Attribute)]
    private static extern bool SerializeToFile(IntPtr pulse, string filename);
    protected override bool BaseSerializeToFile(string filename)
    {
      return SerializeToFile(pulse_cptr, filename);
    }

    [DllImport(Attribute)]
    private static extern bool SerializeFromString(IntPtr pulse, string state, string data_mgr, int format);
    protected override bool BaseSerializeFromString(string state, string data_mgr, int format)
    {
      return SerializeFromString(pulse_cptr, state, data_mgr, format);
    }

    [DllImport(Attribute)]
    private static extern bool SerializeToString(IntPtr pulse, int format, out IntPtr state_str);
    protected override bool BaseSerializeToString(int format, out IntPtr state_str)
    {
      return SerializeToString(pulse_cptr, format, out state_str);
    }

    [DllImport(Attribute)]
    private static extern bool InitializeEngine(IntPtr pulse, string patient_configuration, string data_mgr, int thunk_format);
    protected override bool BaseInitializeEngine(string patient_configuration, string data_mgr, int thunk_format)
    {
      return InitializeEngine(pulse_cptr, patient_configuration, data_mgr, thunk_format);
    }

    [DllImport(Attribute)]
    private static extern bool GetInitialPatient(IntPtr pulse, int format, out IntPtr initial_patient);
    protected override bool BaseGetInitialPatient(int format, out IntPtr initial_patient)
    {
      return GetInitialPatient(pulse_cptr, format, out initial_patient);
    }

    [DllImport(Attribute)]
    private static extern bool GetConditions(IntPtr pulse, int format, out IntPtr conditions);
    protected override bool BaseGetConditions(int format, out IntPtr conditions)
    {
      return GetConditions(pulse_cptr, format, out conditions);
    }

    [DllImport(Attribute)]
    private static extern bool GetPatientAssessment(IntPtr pulse, int type, int format, out IntPtr assessment);
    protected override bool BaseGetPatientAssessment(int type, int format, out IntPtr assessment)
    {
      return GetPatientAssessment(pulse_cptr, type, format, out assessment);
    }

    [DllImport(Attribute)]
    private static extern void LogToConsole(IntPtr pulse, bool b);
    protected override void BaseLogToConsole(bool b)
    {
      LogToConsole(pulse_cptr, b);
    }

    [DllImport(Attribute)]
    private static extern void SetLogFilename(IntPtr pulse, string filename);
    protected override void BaseSetLogFilename(string filename)
    {
      SetLogFilename(pulse_cptr, filename);
    }

    [DllImport(Attribute)]
    private static extern void KeepLogMessages(IntPtr pulse, bool save);// Let the engine know to save log msgs or not
    protected override void BaseKeepLogMessages(bool save)
    {
      KeepLogMessages(pulse_cptr, save);
    }

    [DllImport(Attribute)]
    private static extern bool PullLogMessages(IntPtr pulse, int format, out IntPtr event_changes);
    protected override bool BasePullLogMessages(int format, out IntPtr event_changes)
    {
      return PullLogMessages(pulse_cptr, format, out event_changes);
    }

    [DllImport(Attribute)]
    private static extern void KeepEventChanges(IntPtr pulse, bool keep);// Let the engine know to save events or not
    protected override void BaseKeepEventChanges(bool keep)
    {
      KeepEventChanges(pulse_cptr, keep);
    }

    [DllImport(Attribute)]
    private static extern bool PullEvents(IntPtr pulse, int format, out IntPtr event_changes);
    protected override bool BasePullEvents(int format, out IntPtr event_changes)
    {
      return PullEvents(pulse_cptr, format, out event_changes);
    }

    [DllImport(Attribute)]
    private static extern bool PullActiveEvents(IntPtr pulse, int format, out IntPtr active_events);
    protected override bool BasePullActiveEvents(int format, out IntPtr active_events)
    {
      return PullActiveEvents(pulse_cptr, format, out active_events);
    }

    [DllImport(Attribute)]
    private static extern bool ProcessActions(IntPtr pulse, string any_action_list, int format);
    protected override bool BaseProcessActions(string any_action_list, int format)
    {
      return ProcessActions(pulse_cptr, any_action_list, format);
    }

    [DllImport(Attribute)]
    private static extern bool PullActiveActions(IntPtr pulse, int format, out IntPtr actions);
    protected override bool BasePullActiveActions(int format, out IntPtr actions)
    {
      return PullActiveActions(pulse_cptr, format, out actions);
    }

    [DllImport(Attribute)]
    private static extern bool AdvanceTimeStep(IntPtr pulse);
    protected override bool BaseAdvanceTimeStep()
    {
      return AdvanceTimeStep(pulse_cptr);
    }

    [DllImport(Attribute)]
    private static extern IntPtr PullData(IntPtr pulse);
    protected override IntPtr BasePullData()
    {
      return PullData(pulse_cptr);
    }

    [DllImport(Attribute)]
    public static new extern bool AreCompatibleUnits(string fromUnit, string toUnit);

    [DllImport(Attribute)]
    public static new extern double ConvertValue(double value, string fromUnit, string toUnit);
  }
}
