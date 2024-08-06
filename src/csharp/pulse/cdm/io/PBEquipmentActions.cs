/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

namespace Pulse.CDM
{
  public class PBEquipmentAction
  {
    #region Any Equipment Action

    public static SEEquipmentAction Load(pulse.cdm.bind.AnyEquipmentActionData any)
    {
      if (any.AnesthesiaMachineConfiguration != null)
      {
        SEAnesthesiaMachineConfiguration dst = new SEAnesthesiaMachineConfiguration();
        PBEquipmentAction.Load(any.AnesthesiaMachineConfiguration, dst/*, subMgr*/);
        return dst;
      }
      if (any.AnesthesiaMachineExpiratoryValveLeak != null)
      {
        SEAnesthesiaMachineExpiratoryValveLeak dst = new SEAnesthesiaMachineExpiratoryValveLeak();
        PBEquipmentAction.Load(any.AnesthesiaMachineExpiratoryValveLeak, dst);
        return dst;
      }
      if (any.AnesthesiaMachineExpiratoryValveObstruction != null)
      {
        SEAnesthesiaMachineExpiratoryValveObstruction dst = new SEAnesthesiaMachineExpiratoryValveObstruction();
        PBEquipmentAction.Load(any.AnesthesiaMachineExpiratoryValveObstruction, dst);
        return dst;
      }
      if (any.AnesthesiaMachineInspiratoryValveLeak != null)
      {
        SEAnesthesiaMachineInspiratoryValveLeak dst = new SEAnesthesiaMachineInspiratoryValveLeak();
        PBEquipmentAction.Load(any.AnesthesiaMachineInspiratoryValveLeak, dst);
        return dst;
      }
      if (any.AnesthesiaMachineInspiratoryValveObstruction != null)
      {
        SEAnesthesiaMachineInspiratoryValveObstruction dst = new SEAnesthesiaMachineInspiratoryValveObstruction();
        PBEquipmentAction.Load(any.AnesthesiaMachineInspiratoryValveObstruction, dst);
        return dst;
      }
      if (any.AnesthesiaMachineMaskLeak != null)
      {
        SEAnesthesiaMachineMaskLeak dst = new SEAnesthesiaMachineMaskLeak();
        PBEquipmentAction.Load(any.AnesthesiaMachineMaskLeak, dst);
        return dst;
      }
      if (any.AnesthesiaMachineSodaLimeFailure != null)
      {
        SEAnesthesiaMachineSodaLimeFailure dst = new SEAnesthesiaMachineSodaLimeFailure();
        PBEquipmentAction.Load(any.AnesthesiaMachineSodaLimeFailure, dst);
        return dst;
      }
      if (any.AnesthesiaMachineTubeCuffLeak != null)
      {
        SEAnesthesiaMachineTubeCuffLeak dst = new SEAnesthesiaMachineTubeCuffLeak();
        PBEquipmentAction.Load(any.AnesthesiaMachineTubeCuffLeak, dst);
        return dst;
      }
      if (any.AnesthesiaMachineVaporizerFailure != null)
      {
        SEAnesthesiaMachineVaporizerFailure dst = new SEAnesthesiaMachineVaporizerFailure();
        PBEquipmentAction.Load(any.AnesthesiaMachineVaporizerFailure, dst);
        return dst;
      }
      if (any.AnesthesiaMachineVentilatorPressureLoss != null)
      {
        SEAnesthesiaMachineVentilatorPressureLoss dst = new SEAnesthesiaMachineVentilatorPressureLoss();
        PBEquipmentAction.Load(any.AnesthesiaMachineVentilatorPressureLoss, dst);
        return dst;
      }
      if (any.AnesthesiaMachineYPieceDisconnect != null)
      {
        SEAnesthesiaMachineYPieceDisconnect dst = new SEAnesthesiaMachineYPieceDisconnect();
        PBEquipmentAction.Load(any.AnesthesiaMachineYPieceDisconnect, dst);
        return dst;
      }
      if (any.AnesthesiaMachineOxygenTankPressureLoss != null)
      {
        SEAnesthesiaMachineOxygenTankPressureLoss dst = new SEAnesthesiaMachineOxygenTankPressureLoss();
        PBEquipmentAction.Load(any.AnesthesiaMachineOxygenTankPressureLoss, dst);
        return dst;
      }
      if (any.AnesthesiaMachineOxygenWallPortPressureLoss != null)
      {
        SEAnesthesiaMachineOxygenWallPortPressureLoss dst = new SEAnesthesiaMachineOxygenWallPortPressureLoss();
        PBEquipmentAction.Load(any.AnesthesiaMachineOxygenWallPortPressureLoss, dst);
        return dst;
      }

      if (any.BagValveMaskConfiguration != null)
      {
        SEBagValveMaskConfiguration dst = new SEBagValveMaskConfiguration();
        PBEquipmentAction.Load(any.BagValveMaskConfiguration, dst/*, subMgr*/);
        return dst;
      }
      if (any.BagValveMaskAutomated != null)
      {
        SEBagValveMaskAutomated dst = new SEBagValveMaskAutomated();
        PBEquipmentAction.Load(any.BagValveMaskAutomated, dst/*, subMgr*/);
        return dst;
      }
      if (any.BagValveMaskInstantaneous != null)
      {
        SEBagValveMaskInstantaneous dst = new SEBagValveMaskInstantaneous();
        PBEquipmentAction.Load(any.BagValveMaskInstantaneous, dst/*, subMgr*/);
        return dst;
      }
      if (any.BagValveMaskSqueeze != null)
      {
        SEBagValveMaskSqueeze dst = new SEBagValveMaskSqueeze();
        PBEquipmentAction.Load(any.BagValveMaskSqueeze, dst/*, subMgr*/);
        return dst;
      }

      if (any.ECMOConfiguration != null)
      {
        SEECMOConfiguration dst = new SEECMOConfiguration();
        PBEquipmentAction.Load(any.ECMOConfiguration, dst/*, subMgr*/);
        return dst;
      }

      if (any.MechanicalVentilatorConfiguration != null)
      {
        SEMechanicalVentilatorConfiguration dst = new SEMechanicalVentilatorConfiguration();
        PBEquipmentAction.Load(any.MechanicalVentilatorConfiguration, dst/*, subMgr*/);
        return dst;
      }
      if (any.MechanicalVentilatorHold != null)
      {
        SEMechanicalVentilatorHold dst = new SEMechanicalVentilatorHold();
        PBEquipmentAction.Load(any.MechanicalVentilatorHold, dst/*, subMgr*/);
        return dst;
      }
      if (any.MechanicalVentilatorLeak != null)
      {
        SEMechanicalVentilatorLeak dst = new SEMechanicalVentilatorLeak();
        PBEquipmentAction.Load(any.MechanicalVentilatorLeak, dst/*, subMgr*/);
        return dst;
      }
      if (any.MechanicalVentilatorContinuousPositiveAirwayPressure != null)
      {
        SEMechanicalVentilatorContinuousPositiveAirwayPressure dst = new SEMechanicalVentilatorContinuousPositiveAirwayPressure();
        PBEquipmentAction.Load(any.MechanicalVentilatorContinuousPositiveAirwayPressure, dst/*, subMgr*/);
        return dst;
      }
      if (any.MechanicalVentilatorPressureControl != null)
      {
        SEMechanicalVentilatorPressureControl dst = new SEMechanicalVentilatorPressureControl();
        PBEquipmentAction.Load(any.MechanicalVentilatorPressureControl, dst/*, subMgr*/);
        return dst;
      }
      if (any.MechanicalVentilatorVolumeControl != null)
      {
        SEMechanicalVentilatorVolumeControl dst = new SEMechanicalVentilatorVolumeControl();
        PBEquipmentAction.Load(any.MechanicalVentilatorVolumeControl, dst/*, subMgr*/);
        return dst;
      }

      if (any.InhalerConfiguration != null)
      {
        SEInhalerConfiguration dst = new SEInhalerConfiguration();
        PBEquipmentAction.Load(any.InhalerConfiguration, dst/*, subMgr*/);
        return dst;
      }

      //Log.error("Unknown AnesthesiaMachine Action");
      return null;
    }

    public static pulse.cdm.bind.AnyEquipmentActionData Unload(SEEquipmentAction action)
    {
      pulse.cdm.bind.AnyEquipmentActionData any = new pulse.cdm.bind.AnyEquipmentActionData();

      if (action is SEAnesthesiaMachineAction)
      {
        if (action.GetType().IsAssignableFrom(typeof(SEAnesthesiaMachineConfiguration)))
        {
          any.AnesthesiaMachineConfiguration = Unload((SEAnesthesiaMachineConfiguration)action);
          return any;
        }
        if (action.GetType().IsAssignableFrom(typeof(SEAnesthesiaMachineExpiratoryValveLeak)))
        {
          any.AnesthesiaMachineExpiratoryValveLeak = Unload((SEAnesthesiaMachineExpiratoryValveLeak)action);
          return any;
        }
        if (action.GetType().IsAssignableFrom(typeof(SEAnesthesiaMachineExpiratoryValveObstruction)))
        {
          any.AnesthesiaMachineExpiratoryValveObstruction = Unload((SEAnesthesiaMachineExpiratoryValveObstruction)action);
          return any;
        }
        if (action.GetType().IsAssignableFrom(typeof(SEAnesthesiaMachineInspiratoryValveLeak)))
        {
          any.AnesthesiaMachineInspiratoryValveLeak = Unload((SEAnesthesiaMachineInspiratoryValveLeak)action);
          return any;
        }
        if (action.GetType().IsAssignableFrom(typeof(SEAnesthesiaMachineInspiratoryValveObstruction)))
        {
          any.AnesthesiaMachineInspiratoryValveObstruction = Unload((SEAnesthesiaMachineInspiratoryValveObstruction)action);
          return any;
        }
        if (action.GetType().IsAssignableFrom(typeof(SEAnesthesiaMachineMaskLeak)))
        {
          any.AnesthesiaMachineMaskLeak = Unload((SEAnesthesiaMachineMaskLeak)action);
          return any;
        }
        if (action.GetType().IsAssignableFrom(typeof(SEAnesthesiaMachineOxygenTankPressureLoss)))
        {
          any.AnesthesiaMachineOxygenTankPressureLoss = Unload((SEAnesthesiaMachineOxygenTankPressureLoss)action);
          return any;
        }
        if (action.GetType().IsAssignableFrom(typeof(SEAnesthesiaMachineOxygenWallPortPressureLoss)))
        {
          any.AnesthesiaMachineOxygenWallPortPressureLoss = Unload((SEAnesthesiaMachineOxygenWallPortPressureLoss)action);
          return any;
        }
        if (action.GetType().IsAssignableFrom(typeof(SEAnesthesiaMachineSodaLimeFailure)))
        {
          any.AnesthesiaMachineSodaLimeFailure = Unload((SEAnesthesiaMachineSodaLimeFailure)action);
          return any;
        }
        if (action.GetType().IsAssignableFrom(typeof(SEAnesthesiaMachineTubeCuffLeak)))
        {
          any.AnesthesiaMachineTubeCuffLeak = Unload((SEAnesthesiaMachineTubeCuffLeak)action);
          return any;
        }
        if (action.GetType().IsAssignableFrom(typeof(SEAnesthesiaMachineVaporizerFailure)))
        {
          any.AnesthesiaMachineVaporizerFailure = Unload((SEAnesthesiaMachineVaporizerFailure)action);
          return any;
        }
        if (action.GetType().IsAssignableFrom(typeof(SEAnesthesiaMachineVentilatorPressureLoss)))
        {
          any.AnesthesiaMachineVentilatorPressureLoss = Unload((SEAnesthesiaMachineVentilatorPressureLoss)action);
          return any;
        }
        if (action.GetType().IsAssignableFrom(typeof(SEAnesthesiaMachineYPieceDisconnect)))
        {
          any.AnesthesiaMachineYPieceDisconnect = Unload((SEAnesthesiaMachineYPieceDisconnect)action);
          return any;
        }
      }

      if (action is SEBagValveMaskAction)
      {
        if (action.GetType().IsAssignableFrom(typeof(SEBagValveMaskConfiguration)))
        {
          any.BagValveMaskConfiguration = Unload((SEBagValveMaskConfiguration)action);
          return any;
        }
        if (action.GetType().IsAssignableFrom(typeof(SEBagValveMaskAutomated)))
        {
          any.BagValveMaskAutomated = Unload((SEBagValveMaskAutomated)action);
          return any;
        }
        if (action.GetType().IsAssignableFrom(typeof(SEBagValveMaskInstantaneous)))
        {
          any.BagValveMaskInstantaneous = Unload((SEBagValveMaskInstantaneous)action);
          return any;
        }
        if (action.GetType().IsAssignableFrom(typeof(SEBagValveMaskSqueeze)))
        {
          any.BagValveMaskSqueeze = Unload((SEBagValveMaskSqueeze)action);
          return any;
        }
      }

      if (action is SEECMOAction)
      {
        if (action.GetType().IsAssignableFrom(typeof(SEECMOConfiguration)))
        {
          any.ECMOConfiguration = Unload((SEECMOConfiguration)action);
          return any;
        }
      }

      if (action is SEInhalerAction)
      {
        if (action.GetType().IsAssignableFrom(typeof(SEInhalerConfiguration)))
        {
          any.InhalerConfiguration = Unload((SEInhalerConfiguration)action);
          return any;
        }
      }

      if (action is SEMechanicalVentilatorAction)
      {
        if (action.GetType().IsAssignableFrom(typeof(SEMechanicalVentilatorConfiguration)))
        {
          any.MechanicalVentilatorConfiguration = Unload((SEMechanicalVentilatorConfiguration)action);
          return any;
        }
        if (action.GetType().IsAssignableFrom(typeof(SEMechanicalVentilatorHold)))
        {
          any.MechanicalVentilatorHold = Unload((SEMechanicalVentilatorHold)action);
          return any;
        }
        if (action.GetType().IsAssignableFrom(typeof(SEMechanicalVentilatorLeak)))
        {
          any.MechanicalVentilatorLeak = Unload((SEMechanicalVentilatorLeak)action);
          return any;
        }
        // Modes
        if (action.GetType().IsAssignableFrom(typeof(SEMechanicalVentilatorContinuousPositiveAirwayPressure)))
        {
          any.MechanicalVentilatorContinuousPositiveAirwayPressure = Unload((SEMechanicalVentilatorContinuousPositiveAirwayPressure)action);
          return any;
        }
        if (action.GetType().IsAssignableFrom(typeof(SEMechanicalVentilatorPressureControl)))
        {
          any.MechanicalVentilatorPressureControl = Unload((SEMechanicalVentilatorPressureControl)action);
          return any;
        }
        if (action.GetType().IsAssignableFrom(typeof(SEMechanicalVentilatorVolumeControl)))
        {
          any.MechanicalVentilatorVolumeControl = Unload((SEMechanicalVentilatorVolumeControl)action);
          return any;
        }
      }

      //Log.error("Unsupported Equipment Action type " + c);
      return null;
    }

    #endregion

    #region SEEquipmentAction
    public static void Serialize(pulse.cdm.bind.EquipmentActionData src, SEEquipmentAction dst)
    {
      PBAction.Serialize(src.Action, dst);
    }
    protected static void Serialize(SEEquipmentAction src, pulse.cdm.bind.EquipmentActionData dst)
    {
      dst.Action = new pulse.cdm.bind.ActionData();
      PBAction.Serialize(src, dst.Action);
    }
    #endregion

    #region SEAnesthesiaMachineAction
    public static void Serialize(pulse.cdm.bind.AnesthesiaMachineActionData src, SEAnesthesiaMachineAction dst)
    {
      if(src.EquipmentAction != null)
        PBEquipmentAction.Serialize(src.EquipmentAction, dst);
    }
    protected static void Serialize(SEAnesthesiaMachineAction src, pulse.cdm.bind.AnesthesiaMachineActionData dst)
    {
      dst.EquipmentAction = new pulse.cdm.bind.EquipmentActionData();
      PBEquipmentAction.Serialize(src, dst.EquipmentAction);
    }
    #endregion

    #region SEAnesthesiaMachineConfiguration
    public static void Load(pulse.cdm.bind.AnesthesiaMachineConfigurationData src, SEAnesthesiaMachineConfiguration dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.AnesthesiaMachineConfigurationData src, SEAnesthesiaMachineConfiguration dst)
    {
      if (src.AnesthesiaMachineAction != null)
        Serialize(src.AnesthesiaMachineAction, dst);
      if (!string.IsNullOrEmpty(src.ConfigurationFile))
        dst.SetConfigurationFile(src.ConfigurationFile);
      else if (src.Configuration != null)
        PBAnesthesiaMachine.Load(src.Configuration, dst.GetConfiguration());
    }
    public static pulse.cdm.bind.AnesthesiaMachineConfigurationData Unload(SEAnesthesiaMachineConfiguration src)
    {
      pulse.cdm.bind.AnesthesiaMachineConfigurationData dst = new pulse.cdm.bind.AnesthesiaMachineConfigurationData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEAnesthesiaMachineConfiguration src, pulse.cdm.bind.AnesthesiaMachineConfigurationData dst)
    {
      dst.AnesthesiaMachineAction = new pulse.cdm.bind.AnesthesiaMachineActionData();
      Serialize(src, dst.AnesthesiaMachineAction);
      if (src.HasConfiguration())
        dst.Configuration = PBAnesthesiaMachine.Unload(src.GetConfiguration());
      else if (src.HasConfigurationFile())
        dst.ConfigurationFile = src.GetConfigurationFile();
    }
    #endregion

    #region SEAnesthesiaMachineExpiratoryValveLeak
    public static void Load(pulse.cdm.bind.AnesthesiaMachineExpiratoryValveLeakData src, SEAnesthesiaMachineExpiratoryValveLeak dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.AnesthesiaMachineExpiratoryValveLeakData src, SEAnesthesiaMachineExpiratoryValveLeak dst)
    {
      if (src.AnesthesiaMachineAction != null)
        Serialize(src.AnesthesiaMachineAction, dst);
      if (src.Severity != null)
        PBProperty.Load(src.Severity, dst.GetSeverity());
    }

    public static pulse.cdm.bind.AnesthesiaMachineExpiratoryValveLeakData Unload(SEAnesthesiaMachineExpiratoryValveLeak src)
    {
      pulse.cdm.bind.AnesthesiaMachineExpiratoryValveLeakData dst = new pulse.cdm.bind.AnesthesiaMachineExpiratoryValveLeakData();
      Serialize(src, dst);
      return dst;
    }
    protected static void Serialize(SEAnesthesiaMachineExpiratoryValveLeak src, pulse.cdm.bind.AnesthesiaMachineExpiratoryValveLeakData dst)
    {
      dst.AnesthesiaMachineAction = new pulse.cdm.bind.AnesthesiaMachineActionData();
      Serialize(src, dst.AnesthesiaMachineAction);
      if (src.HasSeverity())
        dst.Severity = PBProperty.Unload(src.GetSeverity());
    }
    #endregion

    #region SEAnesthesiaMachineExpiratoryValveObstruction
    public static void Load(pulse.cdm.bind.AnesthesiaMachineExpiratoryValveObstructionData src, SEAnesthesiaMachineExpiratoryValveObstruction dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.AnesthesiaMachineExpiratoryValveObstructionData src, SEAnesthesiaMachineExpiratoryValveObstruction dst)
    {
      if (src.AnesthesiaMachineAction != null)
        Serialize(src.AnesthesiaMachineAction, dst);
      if (src.Severity != null)
        PBProperty.Load(src.Severity, dst.GetSeverity());
    }

    public static pulse.cdm.bind.AnesthesiaMachineExpiratoryValveObstructionData Unload(SEAnesthesiaMachineExpiratoryValveObstruction src)
    {
      pulse.cdm.bind.AnesthesiaMachineExpiratoryValveObstructionData dst = new pulse.cdm.bind.AnesthesiaMachineExpiratoryValveObstructionData();
      Serialize(src, dst);
      return dst;
    }
    protected static void Serialize(SEAnesthesiaMachineExpiratoryValveObstruction src, pulse.cdm.bind.AnesthesiaMachineExpiratoryValveObstructionData dst)
    {
      dst.AnesthesiaMachineAction = new pulse.cdm.bind.AnesthesiaMachineActionData();
      Serialize(src, dst.AnesthesiaMachineAction);
      if (src.HasSeverity())
        dst.Severity = PBProperty.Unload(src.GetSeverity());
    }
    #endregion

    #region SEAnesthesiaMachineInspiratoryValveLeak
    public static void Load(pulse.cdm.bind.AnesthesiaMachineInspiratoryValveLeakData src, SEAnesthesiaMachineInspiratoryValveLeak dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.AnesthesiaMachineInspiratoryValveLeakData src, SEAnesthesiaMachineInspiratoryValveLeak dst)
    {
      if (src.AnesthesiaMachineAction != null)
        Serialize(src.AnesthesiaMachineAction, dst);
      if (src.Severity != null)
        PBProperty.Load(src.Severity, dst.GetSeverity());
    }

    public static pulse.cdm.bind.AnesthesiaMachineInspiratoryValveLeakData Unload(SEAnesthesiaMachineInspiratoryValveLeak src)
    {
      pulse.cdm.bind.AnesthesiaMachineInspiratoryValveLeakData dst = new pulse.cdm.bind.AnesthesiaMachineInspiratoryValveLeakData();
      Serialize(src, dst);
      return dst;
    }
    protected static void Serialize(SEAnesthesiaMachineInspiratoryValveLeak src, pulse.cdm.bind.AnesthesiaMachineInspiratoryValveLeakData dst)
    {
      dst.AnesthesiaMachineAction = new pulse.cdm.bind.AnesthesiaMachineActionData();
      Serialize(src, dst.AnesthesiaMachineAction);
      if (src.HasSeverity())
        dst.Severity = PBProperty.Unload(src.GetSeverity());
    }
    #endregion

    #region SEAnesthesiaMachineInspiratoryValveObstruction
    public static void Load(pulse.cdm.bind.AnesthesiaMachineInspiratoryValveObstructionData src, SEAnesthesiaMachineInspiratoryValveObstruction dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.AnesthesiaMachineInspiratoryValveObstructionData src, SEAnesthesiaMachineInspiratoryValveObstruction dst)
    {
      if (src.AnesthesiaMachineAction != null)
        Serialize(src.AnesthesiaMachineAction, dst);
      if (src.Severity != null)
        PBProperty.Load(src.Severity, dst.GetSeverity());
    }

    public static pulse.cdm.bind.AnesthesiaMachineInspiratoryValveObstructionData Unload(SEAnesthesiaMachineInspiratoryValveObstruction src)
    {
      pulse.cdm.bind.AnesthesiaMachineInspiratoryValveObstructionData dst = new pulse.cdm.bind.AnesthesiaMachineInspiratoryValveObstructionData();
      Serialize(src, dst);
      return dst;
    }
    protected static void Serialize(SEAnesthesiaMachineInspiratoryValveObstruction src, pulse.cdm.bind.AnesthesiaMachineInspiratoryValveObstructionData dst)
    {
      dst.AnesthesiaMachineAction = new pulse.cdm.bind.AnesthesiaMachineActionData();
      Serialize(src, dst.AnesthesiaMachineAction);
      if (src.HasSeverity())
        dst.Severity = PBProperty.Unload(src.GetSeverity());
    }
    #endregion

    #region SEAnesthesiaMachineMaskLeak
    public static void Load(pulse.cdm.bind.AnesthesiaMachineMaskLeakData src, SEAnesthesiaMachineMaskLeak dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.AnesthesiaMachineMaskLeakData src, SEAnesthesiaMachineMaskLeak dst)
    {
      if (src.AnesthesiaMachineAction != null)
        Serialize(src.AnesthesiaMachineAction, dst);
      if (src.Severity != null)
        PBProperty.Load(src.Severity, dst.GetSeverity());
    }

    public static pulse.cdm.bind.AnesthesiaMachineMaskLeakData Unload(SEAnesthesiaMachineMaskLeak src)
    {
      pulse.cdm.bind.AnesthesiaMachineMaskLeakData dst = new pulse.cdm.bind.AnesthesiaMachineMaskLeakData();
      Serialize(src, dst);
      return dst;
    }
    protected static void Serialize(SEAnesthesiaMachineMaskLeak src, pulse.cdm.bind.AnesthesiaMachineMaskLeakData dst)
    {
      dst.AnesthesiaMachineAction = new pulse.cdm.bind.AnesthesiaMachineActionData();
      Serialize(src, dst.AnesthesiaMachineAction);
      if (src.HasSeverity())
        dst.Severity = PBProperty.Unload(src.GetSeverity());
    }
    #endregion

    #region SEAnesthesiaMachineOxygenTankPressureLoss
    public static void Load(pulse.cdm.bind.AnesthesiaMachineOxygenTankPressureLossData src, SEAnesthesiaMachineOxygenTankPressureLoss dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.AnesthesiaMachineOxygenTankPressureLossData src, SEAnesthesiaMachineOxygenTankPressureLoss dst)
    {
      if (src.AnesthesiaMachineAction != null)
        Serialize(src.AnesthesiaMachineAction, dst);
      dst.SetState((eSwitch)(int)src.State);
    }

    public static pulse.cdm.bind.AnesthesiaMachineOxygenTankPressureLossData Unload(SEAnesthesiaMachineOxygenTankPressureLoss src)
    {
      pulse.cdm.bind.AnesthesiaMachineOxygenTankPressureLossData dst = new pulse.cdm.bind.AnesthesiaMachineOxygenTankPressureLossData();
      Serialize(src, dst);
      return dst;
    }
    protected static void Serialize(SEAnesthesiaMachineOxygenTankPressureLoss src, pulse.cdm.bind.AnesthesiaMachineOxygenTankPressureLossData dst)
    {
      dst.AnesthesiaMachineAction = new pulse.cdm.bind.AnesthesiaMachineActionData();
      Serialize(src, dst.AnesthesiaMachineAction);
      dst.State = (pulse.cdm.bind.eSwitch)(int)src.GetState();
    }
    #endregion

    #region SEAnesthesiaMachineOxygenWallPortPressureLoss
    public static void Load(pulse.cdm.bind.AnesthesiaMachineOxygenWallPortPressureLossData src, SEAnesthesiaMachineOxygenWallPortPressureLoss dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.AnesthesiaMachineOxygenWallPortPressureLossData src, SEAnesthesiaMachineOxygenWallPortPressureLoss dst)
    {
      if (src.AnesthesiaMachineAction != null)
        Serialize(src.AnesthesiaMachineAction, dst);
      dst.SetState((eSwitch)(int)src.State);
    }

    public static pulse.cdm.bind.AnesthesiaMachineOxygenWallPortPressureLossData Unload(SEAnesthesiaMachineOxygenWallPortPressureLoss src)
    {
      pulse.cdm.bind.AnesthesiaMachineOxygenWallPortPressureLossData dst = new pulse.cdm.bind.AnesthesiaMachineOxygenWallPortPressureLossData();
      Serialize(src, dst);
      return dst;
    }
    protected static void Serialize(SEAnesthesiaMachineOxygenWallPortPressureLoss src, pulse.cdm.bind.AnesthesiaMachineOxygenWallPortPressureLossData dst)
    {
      dst.AnesthesiaMachineAction = new pulse.cdm.bind.AnesthesiaMachineActionData();
      Serialize(src, dst.AnesthesiaMachineAction);
      dst.State = (pulse.cdm.bind.eSwitch)(int)src.GetState();
    }
    #endregion

    #region SEAnesthesiaMachineSodaLimeFailure
    public static void Load(pulse.cdm.bind.AnesthesiaMachineSodaLimeFailureData src, SEAnesthesiaMachineSodaLimeFailure dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.AnesthesiaMachineSodaLimeFailureData src, SEAnesthesiaMachineSodaLimeFailure dst)
    {
      if (src.AnesthesiaMachineAction != null)
        Serialize(src.AnesthesiaMachineAction, dst);
      if (src.Severity != null)
        PBProperty.Load(src.Severity, dst.GetSeverity());
    }

    public static pulse.cdm.bind.AnesthesiaMachineSodaLimeFailureData Unload(SEAnesthesiaMachineSodaLimeFailure src)
    {
      pulse.cdm.bind.AnesthesiaMachineSodaLimeFailureData dst = new pulse.cdm.bind.AnesthesiaMachineSodaLimeFailureData();
      Serialize(src, dst);
      return dst;
    }
    protected static void Serialize(SEAnesthesiaMachineSodaLimeFailure src, pulse.cdm.bind.AnesthesiaMachineSodaLimeFailureData dst)
    {
      dst.AnesthesiaMachineAction = new pulse.cdm.bind.AnesthesiaMachineActionData();
      Serialize(src, dst.AnesthesiaMachineAction);
      if (src.HasSeverity())
        dst.Severity = PBProperty.Unload(src.GetSeverity());
    }
    #endregion

    #region SEAnesthesiaMachineTubeCuffLeak
    public static void Load(pulse.cdm.bind.AnesthesiaMachineTubeCuffLeakData src, SEAnesthesiaMachineTubeCuffLeak dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.AnesthesiaMachineTubeCuffLeakData src, SEAnesthesiaMachineTubeCuffLeak dst)
    {
      if (src.AnesthesiaMachineAction != null)
        Serialize(src.AnesthesiaMachineAction, dst);
      if (src.Severity != null)
        PBProperty.Load(src.Severity, dst.GetSeverity());
    }

    public static pulse.cdm.bind.AnesthesiaMachineTubeCuffLeakData Unload(SEAnesthesiaMachineTubeCuffLeak src)
    {
      pulse.cdm.bind.AnesthesiaMachineTubeCuffLeakData dst = new pulse.cdm.bind.AnesthesiaMachineTubeCuffLeakData();
      Serialize(src, dst);
      return dst;
    }
    protected static void Serialize(SEAnesthesiaMachineTubeCuffLeak src, pulse.cdm.bind.AnesthesiaMachineTubeCuffLeakData dst)
    {
      dst.AnesthesiaMachineAction = new pulse.cdm.bind.AnesthesiaMachineActionData();
      Serialize(src, dst.AnesthesiaMachineAction);
      if (src.HasSeverity())
        dst.Severity = PBProperty.Unload(src.GetSeverity());
    }
    #endregion

    #region SEAnesthesiaMachineVaporizerFailure
    public static void Load(pulse.cdm.bind.AnesthesiaMachineVaporizerFailureData src, SEAnesthesiaMachineVaporizerFailure dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.AnesthesiaMachineVaporizerFailureData src, SEAnesthesiaMachineVaporizerFailure dst)
    {
      if (src.AnesthesiaMachineAction != null)
        Serialize(src.AnesthesiaMachineAction, dst);
      if (src.Severity != null)
        PBProperty.Load(src.Severity, dst.GetSeverity());
    }

    public static pulse.cdm.bind.AnesthesiaMachineVaporizerFailureData Unload(SEAnesthesiaMachineVaporizerFailure src)
    {
      pulse.cdm.bind.AnesthesiaMachineVaporizerFailureData dst = new pulse.cdm.bind.AnesthesiaMachineVaporizerFailureData();
      Serialize(src, dst);
      return dst;
    }
    protected static void Serialize(SEAnesthesiaMachineVaporizerFailure src, pulse.cdm.bind.AnesthesiaMachineVaporizerFailureData dst)
    {
      dst.AnesthesiaMachineAction = new pulse.cdm.bind.AnesthesiaMachineActionData();
      Serialize(src, dst.AnesthesiaMachineAction);
      if (src.HasSeverity())
        dst.Severity = PBProperty.Unload(src.GetSeverity());
    }
    #endregion

    #region SEAnesthesiaMachineVentilatorPressureLoss
    public static void Load(pulse.cdm.bind.AnesthesiaMachineVentilatorPressureLossData src, SEAnesthesiaMachineVentilatorPressureLoss dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.AnesthesiaMachineVentilatorPressureLossData src, SEAnesthesiaMachineVentilatorPressureLoss dst)
    {
      if (src.AnesthesiaMachineAction != null)
        Serialize(src.AnesthesiaMachineAction, dst);
      if (src.Severity != null)
        PBProperty.Load(src.Severity, dst.GetSeverity());
    }

    public static pulse.cdm.bind.AnesthesiaMachineVentilatorPressureLossData Unload(SEAnesthesiaMachineVentilatorPressureLoss src)
    {
      pulse.cdm.bind.AnesthesiaMachineVentilatorPressureLossData dst = new pulse.cdm.bind.AnesthesiaMachineVentilatorPressureLossData();
      Serialize(src, dst);
      return dst;
    }
    protected static void Serialize(SEAnesthesiaMachineVentilatorPressureLoss src, pulse.cdm.bind.AnesthesiaMachineVentilatorPressureLossData dst)
    {
      dst.AnesthesiaMachineAction = new pulse.cdm.bind.AnesthesiaMachineActionData();
      Serialize(src, dst.AnesthesiaMachineAction);
      if (src.HasSeverity())
        dst.Severity = PBProperty.Unload(src.GetSeverity());
    }
    #endregion

    #region SEAnesthesiaMachineYPieceDisconnect
    public static void Load(pulse.cdm.bind.AnesthesiaMachineYPieceDisconnectData src, SEAnesthesiaMachineYPieceDisconnect dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.AnesthesiaMachineYPieceDisconnectData src, SEAnesthesiaMachineYPieceDisconnect dst)
    {
      if (src.AnesthesiaMachineAction != null)
        Serialize(src.AnesthesiaMachineAction, dst);
      if (src.Severity != null)
        PBProperty.Load(src.Severity, dst.GetSeverity());
    }

    public static pulse.cdm.bind.AnesthesiaMachineYPieceDisconnectData Unload(SEAnesthesiaMachineYPieceDisconnect src)
    {
      pulse.cdm.bind.AnesthesiaMachineYPieceDisconnectData dst = new pulse.cdm.bind.AnesthesiaMachineYPieceDisconnectData();
      Serialize(src, dst);
      return dst;
    }
    protected static void Serialize(SEAnesthesiaMachineYPieceDisconnect src, pulse.cdm.bind.AnesthesiaMachineYPieceDisconnectData dst)
    {
      dst.AnesthesiaMachineAction = new pulse.cdm.bind.AnesthesiaMachineActionData();
      Serialize(src, dst.AnesthesiaMachineAction);
      if (src.HasSeverity())
        dst.Severity = PBProperty.Unload(src.GetSeverity());
    }
    #endregion

    #region SEBagValveMaskAction
    public static void Serialize(pulse.cdm.bind.BagValveMaskActionData src, SEBagValveMaskAction dst)
    {
      if (src.EquipmentAction != null)
        PBEquipmentAction.Serialize(src.EquipmentAction, dst);
    }
    protected static void Serialize(SEBagValveMaskAction src, pulse.cdm.bind.BagValveMaskActionData dst)
    {
      dst.EquipmentAction = new pulse.cdm.bind.EquipmentActionData();
      PBEquipmentAction.Serialize(src, dst.EquipmentAction);
    }
    #endregion

    #region SEBagValveMaskConfiguration
    public static void Load(pulse.cdm.bind.BagValveMaskConfigurationData src, SEBagValveMaskConfiguration dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.BagValveMaskConfigurationData src, SEBagValveMaskConfiguration dst)
    {
      Serialize(src.BagValveMaskAction, dst);
      if (!string.IsNullOrEmpty(src.ConfigurationFile))
        dst.SetConfigurationFile(src.ConfigurationFile);
      else if (src.Configuration != null)
        PBBagValveMask.Load(src.Configuration, dst.GetConfiguration());
    }
    public static pulse.cdm.bind.BagValveMaskConfigurationData Unload(SEBagValveMaskConfiguration src)
    {
      pulse.cdm.bind.BagValveMaskConfigurationData dst = new pulse.cdm.bind.BagValveMaskConfigurationData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEBagValveMaskConfiguration src, pulse.cdm.bind.BagValveMaskConfigurationData dst)
    {
      dst.BagValveMaskAction = new pulse.cdm.bind.BagValveMaskActionData();
      Serialize(src, dst.BagValveMaskAction);
      if (src.HasConfigurationFile())
        dst.ConfigurationFile = src.GetConfigurationFile();
      else if (src.HasConfiguration())
        dst.Configuration = PBBagValveMask.Unload(src.GetConfiguration());
    }
    #endregion

    #region SEBagValveMaskAutomated
    public static void Load(pulse.cdm.bind.BagValveMaskAutomatedData src, SEBagValveMaskAutomated dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.BagValveMaskAutomatedData src, SEBagValveMaskAutomated dst)
    {
      Serialize(src.BagValveMaskAction, dst);
      if (src.BreathFrequency != null)
        PBProperty.Load(src.BreathFrequency, dst.GetBreathFrequency());
      if (src.InspiratoryExpiratoryRatio != null)
        PBProperty.Load(src.InspiratoryExpiratoryRatio, dst.GetInspiratoryExpiratoryRatio());
      if (src.SqueezePressure != null)
        PBProperty.Load(src.SqueezePressure, dst.GetSqueezePressure());
      if (src.SqueezeVolume != null)
        PBProperty.Load(src.SqueezeVolume, dst.GetSqueezeVolume());
    }

    public static pulse.cdm.bind.BagValveMaskAutomatedData Unload(SEBagValveMaskAutomated src)
    {
      pulse.cdm.bind.BagValveMaskAutomatedData dst = new pulse.cdm.bind.BagValveMaskAutomatedData();
      Serialize(src, dst);
      return dst;
    }
    protected static void Serialize(SEBagValveMaskAutomated src, pulse.cdm.bind.BagValveMaskAutomatedData dst)
    {
      dst.BagValveMaskAction = new pulse.cdm.bind.BagValveMaskActionData();
      Serialize(src, dst.BagValveMaskAction);
      if (src.HasBreathFrequency())
        dst.BreathFrequency = PBProperty.Unload(src.GetBreathFrequency());
      if (src.HasInspiratoryExpiratoryRatio())
        dst.InspiratoryExpiratoryRatio = PBProperty.Unload(src.GetInspiratoryExpiratoryRatio());
      if (src.HasSqueezePressure())
        dst.SqueezePressure = PBProperty.Unload(src.GetSqueezePressure());
      if (src.HasSqueezeVolume())
        dst.SqueezeVolume = PBProperty.Unload(src.GetSqueezeVolume());
    }
    #endregion

    #region SEBagValveMaskInstantaneous
    public static void Load(pulse.cdm.bind.BagValveMaskInstantaneousData src, SEBagValveMaskInstantaneous dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.BagValveMaskInstantaneousData src, SEBagValveMaskInstantaneous dst)
    {
      Serialize(src.BagValveMaskAction, dst);
      if (src.Flow != null)
        PBProperty.Load(src.Flow, dst.GetFlow());
      if (src.Pressure != null)
        PBProperty.Load(src.Pressure, dst.GetPressure());
    }

    public static pulse.cdm.bind.BagValveMaskInstantaneousData Unload(SEBagValveMaskInstantaneous src)
    {
      pulse.cdm.bind.BagValveMaskInstantaneousData dst = new pulse.cdm.bind.BagValveMaskInstantaneousData();
      Serialize(src, dst);
      return dst;
    }
    protected static void Serialize(SEBagValveMaskInstantaneous src, pulse.cdm.bind.BagValveMaskInstantaneousData dst)
    {
      dst.BagValveMaskAction = new pulse.cdm.bind.BagValveMaskActionData();
      Serialize(src, dst.BagValveMaskAction);
      if (src.HasFlow())
        dst.Flow = PBProperty.Unload(src.GetFlow());
      if (src.HasPressure())
        dst.Pressure = PBProperty.Unload(src.GetPressure());
    }
    #endregion

    #region SEBagValveMaskSqueeze
    public static void Load(pulse.cdm.bind.BagValveMaskSqueezeData src, SEBagValveMaskSqueeze dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.BagValveMaskSqueezeData src, SEBagValveMaskSqueeze dst)
    {
      Serialize(src.BagValveMaskAction, dst);
      if (src.SqueezePressure != null)
        PBProperty.Load(src.SqueezePressure, dst.GetSqueezePressure());
      if (src.SqueezeVolume != null)
        PBProperty.Load(src.SqueezeVolume, dst.GetSqueezeVolume());
      if (src.ExpiratoryPeriod != null)
        PBProperty.Load(src.ExpiratoryPeriod, dst.GetExpiratoryPeriod());
      if (src.InspiratoryPeriod != null)
        PBProperty.Load(src.InspiratoryPeriod, dst.GetInspiratoryPeriod());
    }

    public static pulse.cdm.bind.BagValveMaskSqueezeData Unload(SEBagValveMaskSqueeze src)
    {
      pulse.cdm.bind.BagValveMaskSqueezeData dst = new pulse.cdm.bind.BagValveMaskSqueezeData();
      Serialize(src, dst);
      return dst;
    }
    protected static void Serialize(SEBagValveMaskSqueeze src, pulse.cdm.bind.BagValveMaskSqueezeData dst)
    {
      dst.BagValveMaskAction = new pulse.cdm.bind.BagValveMaskActionData();
      Serialize(src, dst.BagValveMaskAction);
      if (src.HasSqueezePressure())
        dst.SqueezePressure = PBProperty.Unload(src.GetSqueezePressure());
      if (src.HasSqueezeVolume())
        dst.SqueezeVolume = PBProperty.Unload(src.GetSqueezeVolume());
      if (src.HasExpiratoryPeriod())
        dst.ExpiratoryPeriod = PBProperty.Unload(src.GetExpiratoryPeriod());
      if (src.HasInspiratoryPeriod())
        dst.InspiratoryPeriod = PBProperty.Unload(src.GetInspiratoryPeriod());
    }
    #endregion

    #region SEECMOAction
    public static void Serialize(pulse.cdm.bind.ECMOActionData src, SEECMOAction dst)
    {
      if (src.EquipmentAction != null)
        PBEquipmentAction.Serialize(src.EquipmentAction, dst);
    }
    protected static void Serialize(SEECMOAction src, pulse.cdm.bind.ECMOActionData dst)
    {
      dst.EquipmentAction = new pulse.cdm.bind.EquipmentActionData();
      PBEquipmentAction.Serialize(src, dst.EquipmentAction);
    }
    #endregion

    #region SEECMOConfiguration
    public static void Load(pulse.cdm.bind.ECMOConfigurationData src, SEECMOConfiguration dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.ECMOConfigurationData src, SEECMOConfiguration dst)
    {
      if (src.ECMOAction != null)
        Serialize(src.ECMOAction, dst);
      if (!string.IsNullOrEmpty(src.SettingsFile))
        dst.SetSettingsFile(src.SettingsFile);
      else if (src.Settings != null)
        PBECMO.Load(src.Settings, dst.GetSettings());
    }
    public static pulse.cdm.bind.ECMOConfigurationData Unload(SEECMOConfiguration src)
    {
      pulse.cdm.bind.ECMOConfigurationData dst = new pulse.cdm.bind.ECMOConfigurationData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEECMOConfiguration src, pulse.cdm.bind.ECMOConfigurationData dst)
    {
      dst.ECMOAction = new pulse.cdm.bind.ECMOActionData();
      Serialize(src, dst.ECMOAction);
      if (src.HasSettingsFile())
        dst.SettingsFile = src.GetSettingsFile();
      else if (src.HasSettings())
        dst.Settings = PBECMO.Unload(src.GetSettings());
    }
    #endregion

    #region SEInhalerAction
    public static void Serialize(pulse.cdm.bind.InhalerActionData src, SEInhalerAction dst)
    {
      if (src.EquipmentAction != null)
        PBEquipmentAction.Serialize(src.EquipmentAction, dst);
    }
    protected static void Serialize(SEInhalerAction src, pulse.cdm.bind.InhalerActionData dst)
    {
      dst.EquipmentAction = new pulse.cdm.bind.EquipmentActionData();
      PBEquipmentAction.Serialize(src, dst.EquipmentAction);
    }
    #endregion

    #region SEInhalerConfiguration
    public static void Load(pulse.cdm.bind.InhalerConfigurationData src, SEInhalerConfiguration dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.InhalerConfigurationData src, SEInhalerConfiguration dst)
    {
      if (src.InhalerAction != null)
        Serialize(src.InhalerAction, dst);
      if (!string.IsNullOrEmpty(src.ConfigurationFile))
        dst.SetConfigurationFile(src.ConfigurationFile);
      else if (src.Configuration != null)
        PBInhaler.Load(src.Configuration, dst.GetConfiguration());
    }
    public static pulse.cdm.bind.InhalerConfigurationData Unload(SEInhalerConfiguration src)
    {
      pulse.cdm.bind.InhalerConfigurationData dst = new pulse.cdm.bind.InhalerConfigurationData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEInhalerConfiguration src, pulse.cdm.bind.InhalerConfigurationData dst)
    {
      dst.InhalerAction = new pulse.cdm.bind.InhalerActionData();
      Serialize(src, dst.InhalerAction);
      if (src.HasConfigurationFile())
        dst.ConfigurationFile = src.GetConfigurationFile();
      else if (src.HasConfiguration())
        dst.Configuration = PBInhaler.Unload(src.GetConfiguration());
    }
    #endregion

    #region SEMechanicalVentilatorAction
    public static void Serialize(pulse.cdm.bind.MechanicalVentilatorActionData src, SEMechanicalVentilatorAction dst)
    {
      if (src.EquipmentAction != null)
        PBEquipmentAction.Serialize(src.EquipmentAction, dst);
    }
    protected static void Serialize(SEMechanicalVentilatorAction src, pulse.cdm.bind.MechanicalVentilatorActionData dst)
    {
      dst.EquipmentAction = new pulse.cdm.bind.EquipmentActionData();
      PBEquipmentAction.Serialize(src, dst.EquipmentAction);
    }
    #endregion

    #region SEMechanicalVentilatorConfiguration
    public static void Load(pulse.cdm.bind.MechanicalVentilatorConfigurationData src, SEMechanicalVentilatorConfiguration dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.MechanicalVentilatorConfigurationData src, SEMechanicalVentilatorConfiguration dst)
    {
      if (src.MechanicalVentilatorAction != null)
        Serialize(src.MechanicalVentilatorAction, dst);
      if (!string.IsNullOrEmpty(src.SettingsFile))
        dst.SetSettingsFile(src.SettingsFile);
      else if (src.Settings != null)
        PBMechanicalVentilator.Load(src.Settings, dst.GetSettings());
      dst.SetMergeType((eMergeType)src.MergeType);
    }
    public static pulse.cdm.bind.MechanicalVentilatorConfigurationData Unload(SEMechanicalVentilatorConfiguration src)
    {
      pulse.cdm.bind.MechanicalVentilatorConfigurationData dst = new pulse.cdm.bind.MechanicalVentilatorConfigurationData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEMechanicalVentilatorConfiguration src, pulse.cdm.bind.MechanicalVentilatorConfigurationData dst)
    {
      dst.MechanicalVentilatorAction = new pulse.cdm.bind.MechanicalVentilatorActionData();
      Serialize(src, dst.MechanicalVentilatorAction);
      if (src.HasSettingsFile())
        dst.SettingsFile = src.GetSettingsFile();
      else if (src.HasSettings())
        dst.Settings = PBMechanicalVentilator.Unload(src.GetSettings());
      dst.MergeType = (pulse.cdm.bind.eMergeType)(int)src.GetMergeType();
    }
    #endregion

    #region SEMechanicalVentilatorHold
    public static void Load(pulse.cdm.bind.MechanicalVentilatorHoldData src, SEMechanicalVentilatorHold dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.MechanicalVentilatorHoldData src, SEMechanicalVentilatorHold dst)
    {
      if (src.MechanicalVentilatorAction != null)
        Serialize(src.MechanicalVentilatorAction, dst);
      dst.SetState((eSwitch)src.State);
      dst.SetAppliedRespiratoryCycle((eAppliedRespiratoryCycle)src.AppliedRespiratoryCycle);
    }
    public static pulse.cdm.bind.MechanicalVentilatorHoldData Unload(SEMechanicalVentilatorHold src)
    {
      pulse.cdm.bind.MechanicalVentilatorHoldData dst = new pulse.cdm.bind.MechanicalVentilatorHoldData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEMechanicalVentilatorHold src, pulse.cdm.bind.MechanicalVentilatorHoldData dst)
    {
      dst.MechanicalVentilatorAction = new pulse.cdm.bind.MechanicalVentilatorActionData();
      Serialize(src, dst.MechanicalVentilatorAction);
      dst.State = (pulse.cdm.bind.eSwitch)(int)src.GetState();
      dst.AppliedRespiratoryCycle = (pulse.cdm.bind.eAppliedRespiratoryCycle)(int)src.GetAppliedRespiratoryCycle();
    }
    #endregion

    #region SEMechanicalVentilatorLeak
    public static void Load(pulse.cdm.bind.MechanicalVentilatorLeakData src, SEMechanicalVentilatorLeak dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.MechanicalVentilatorLeakData src, SEMechanicalVentilatorLeak dst)
    {
      if (src.MechanicalVentilatorAction != null)
        Serialize(src.MechanicalVentilatorAction, dst);
      if (src.Severity != null)
        PBProperty.Load(src.Severity, dst.GetSeverity());
    }
    public static pulse.cdm.bind.MechanicalVentilatorLeakData Unload(SEMechanicalVentilatorLeak src)
    {
      pulse.cdm.bind.MechanicalVentilatorLeakData dst = new pulse.cdm.bind.MechanicalVentilatorLeakData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEMechanicalVentilatorLeak src, pulse.cdm.bind.MechanicalVentilatorLeakData dst)
    {
      dst.MechanicalVentilatorAction = new pulse.cdm.bind.MechanicalVentilatorActionData();
      Serialize(src, dst.MechanicalVentilatorAction);
      if (src.HasSeverity())
        dst.Severity = PBProperty.Unload(src.GetSeverity());
    }
    #endregion

    #region SEMechanicalVentilatorMode
    public static void Load(pulse.cdm.bind.MechanicalVentilatorModeData src, SEMechanicalVentilatorMode dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.MechanicalVentilatorModeData src, SEMechanicalVentilatorMode dst)
    {
      if (src.MechanicalVentilatorAction != null)
        Serialize(src.MechanicalVentilatorAction, dst);
      dst.SetConnection((eSwitch)src.Connection);
    }
    public static pulse.cdm.bind.MechanicalVentilatorModeData Unload(SEMechanicalVentilatorMode src)
    {
      pulse.cdm.bind.MechanicalVentilatorModeData dst = new pulse.cdm.bind.MechanicalVentilatorModeData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEMechanicalVentilatorMode src, pulse.cdm.bind.MechanicalVentilatorModeData dst)
    {
      dst.MechanicalVentilatorAction = new pulse.cdm.bind.MechanicalVentilatorActionData();
      Serialize(src, dst.MechanicalVentilatorAction);
      dst.Connection = (pulse.cdm.bind.eSwitch)(int)src.GetConnection();
    }
    #endregion

    #region SEMechanicalVentilatorContinuousPositiveAirwayPressure
    public static void Load(pulse.cdm.bind.MechanicalVentilatorContinuousPositiveAirwayPressureData src, SEMechanicalVentilatorContinuousPositiveAirwayPressure dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.MechanicalVentilatorContinuousPositiveAirwayPressureData src, SEMechanicalVentilatorContinuousPositiveAirwayPressure dst)
    {
      if (src.MechanicalVentilatorMode != null)
        Serialize(src.MechanicalVentilatorMode, dst);
      if (src.DeltaPressureSupport != null)
        PBProperty.Load(src.DeltaPressureSupport, dst.GetDeltaPressureSupport());
      if (src.ExpirationCycleFlow != null)
        PBProperty.Load(src.ExpirationCycleFlow, dst.GetExpirationCycleFlow());
      if (src.ExpirationCyclePressure != null)
        PBProperty.Load(src.ExpirationCyclePressure, dst.GetExpirationCyclePressure());
      dst.SetExpirationWaveform((eDriverWaveform)src.ExpirationWaveform);
      if (src.FractionInspiredOxygen != null)
        PBProperty.Load(src.FractionInspiredOxygen, dst.GetFractionInspiredOxygen());
      if (src.InspirationPatientTriggerFlow != null)
        PBProperty.Load(src.InspirationPatientTriggerFlow, dst.GetInspirationPatientTriggerFlow());
      if (src.InspirationPatientTriggerPressure != null)
        PBProperty.Load(src.InspirationPatientTriggerPressure, dst.GetInspirationPatientTriggerPressure());
      dst.SetInspirationWaveform((eDriverWaveform)src.InspirationWaveform);
      if (src.PositiveEndExpiratoryPressure != null)
        PBProperty.Load(src.PositiveEndExpiratoryPressure, dst.GetPositiveEndExpiratoryPressure());
      if (src.Slope != null)
        PBProperty.Load(src.Slope, dst.GetSlope());
    }
    public static pulse.cdm.bind.MechanicalVentilatorContinuousPositiveAirwayPressureData Unload(SEMechanicalVentilatorContinuousPositiveAirwayPressure src)
    {
      pulse.cdm.bind.MechanicalVentilatorContinuousPositiveAirwayPressureData dst = new pulse.cdm.bind.MechanicalVentilatorContinuousPositiveAirwayPressureData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEMechanicalVentilatorContinuousPositiveAirwayPressure src, pulse.cdm.bind.MechanicalVentilatorContinuousPositiveAirwayPressureData dst)
    {
      dst.MechanicalVentilatorMode = new pulse.cdm.bind.MechanicalVentilatorModeData();
      Serialize(src, dst.MechanicalVentilatorMode);
      if (src.HasDeltaPressureSupport())
        dst.DeltaPressureSupport = PBProperty.Unload(src.GetDeltaPressureSupport());
      if (src.HasExpirationCycleFlow())
        dst.ExpirationCycleFlow = PBProperty.Unload(src.GetExpirationCycleFlow());
      if (src.HasExpirationCyclePressure())
        dst.ExpirationCyclePressure = PBProperty.Unload(src.GetExpirationCyclePressure());
      dst.ExpirationWaveform = (pulse.cdm.bind.eDriverWaveform)(int)src.GetExpirationWaveform();
      if (src.HasFractionInspiredOxygen())
        dst.FractionInspiredOxygen = PBProperty.Unload(src.GetFractionInspiredOxygen());
      if (src.HasInspirationPatientTriggerFlow())
        dst.InspirationPatientTriggerFlow = PBProperty.Unload(src.GetInspirationPatientTriggerFlow());
      if (src.HasInspirationPatientTriggerPressure())
        dst.InspirationPatientTriggerPressure = PBProperty.Unload(src.GetInspirationPatientTriggerPressure());
      dst.InspirationWaveform = (pulse.cdm.bind.eDriverWaveform)(int)src.GetInspirationWaveform();
      if (src.HasPositiveEndExpiratoryPressure())
        dst.PositiveEndExpiratoryPressure = PBProperty.Unload(src.GetPositiveEndExpiratoryPressure());
      if (src.HasSlope())
        dst.Slope = PBProperty.Unload(src.GetSlope());
    }
    #endregion

    #region SEMechanicalVentilatorPressureControl
    public static void Load(pulse.cdm.bind.MechanicalVentilatorPressureControlData src, SEMechanicalVentilatorPressureControl dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.MechanicalVentilatorPressureControlData src, SEMechanicalVentilatorPressureControl dst)
    {
      if (src.MechanicalVentilatorMode != null)
        Serialize(src.MechanicalVentilatorMode, dst);
      if (src.FractionInspiredOxygen != null)
        PBProperty.Load(src.FractionInspiredOxygen, dst.GetFractionInspiredOxygen());
      if (src.InspirationPatientTriggerFlow != null)
        PBProperty.Load(src.InspirationPatientTriggerFlow, dst.GetInspirationPatientTriggerFlow());
      if (src.InspirationPatientTriggerPressure != null)
        PBProperty.Load(src.InspirationPatientTriggerPressure, dst.GetInspirationPatientTriggerPressure());
      dst.SetInspirationWaveform((eDriverWaveform)src.InspirationWaveform);
      if (src.InspiratoryPeriod != null)
        PBProperty.Load(src.InspiratoryPeriod, dst.GetInspiratoryPeriod());
      if (src.InspiratoryPressure != null)
        PBProperty.Load(src.InspiratoryPressure, dst.GetInspiratoryPressure());
      if (src.PositiveEndExpiratoryPressure != null)
        PBProperty.Load(src.PositiveEndExpiratoryPressure, dst.GetPositiveEndExpiratoryPressure());
      if (src.RespirationRate != null)
        PBProperty.Load(src.RespirationRate, dst.GetRespirationRate());
      if (src.Slope != null)
        PBProperty.Load(src.Slope, dst.GetSlope());
    }
    public static pulse.cdm.bind.MechanicalVentilatorPressureControlData Unload(SEMechanicalVentilatorPressureControl src)
    {
      pulse.cdm.bind.MechanicalVentilatorPressureControlData dst = new pulse.cdm.bind.MechanicalVentilatorPressureControlData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEMechanicalVentilatorPressureControl src, pulse.cdm.bind.MechanicalVentilatorPressureControlData dst)
    {
      dst.MechanicalVentilatorMode = new pulse.cdm.bind.MechanicalVentilatorModeData();
      Serialize(src, dst.MechanicalVentilatorMode);
      if (src.HasFractionInspiredOxygen())
        dst.FractionInspiredOxygen = PBProperty.Unload(src.GetFractionInspiredOxygen());
      if (src.HasInspirationPatientTriggerFlow())
        dst.InspirationPatientTriggerFlow = PBProperty.Unload(src.GetInspirationPatientTriggerFlow());
      if (src.HasInspirationPatientTriggerPressure())
        dst.InspirationPatientTriggerPressure = PBProperty.Unload(src.GetInspirationPatientTriggerPressure());
      dst.InspirationWaveform = (pulse.cdm.bind.eDriverWaveform)(int)src.GetInspirationWaveform();
      if (src.HasInspiratoryPeriod())
        dst.InspiratoryPeriod = PBProperty.Unload(src.GetInspiratoryPeriod());
      if (src.HasInspiratoryPressure())
        dst.InspiratoryPressure = PBProperty.Unload(src.GetInspiratoryPressure());
      if (src.HasPositiveEndExpiratoryPressure())
        dst.PositiveEndExpiratoryPressure = PBProperty.Unload(src.GetPositiveEndExpiratoryPressure());
      if (src.HasRespirationRate())
        dst.RespirationRate = PBProperty.Unload(src.GetRespirationRate());
      if (src.HasSlope())
        dst.Slope = PBProperty.Unload(src.GetSlope());
    }
    #endregion

    #region SEMechanicalVentilatorVolumeControl
    public static void Load(pulse.cdm.bind.MechanicalVentilatorVolumeControlData src, SEMechanicalVentilatorVolumeControl dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.MechanicalVentilatorVolumeControlData src, SEMechanicalVentilatorVolumeControl dst)
    {
      if (src.MechanicalVentilatorMode != null)
        Serialize(src.MechanicalVentilatorMode, dst);
      if (src.Flow != null)
        PBProperty.Load(src.Flow, dst.GetFlow());
      if (src.FractionInspiredOxygen != null)
        PBProperty.Load(src.FractionInspiredOxygen, dst.GetFractionInspiredOxygen());
      if (src.InspirationPatientTriggerFlow != null)
        PBProperty.Load(src.InspirationPatientTriggerFlow, dst.GetInspirationPatientTriggerFlow());
      if (src.InspirationPatientTriggerPressure != null)
        PBProperty.Load(src.InspirationPatientTriggerPressure, dst.GetInspirationPatientTriggerPressure());
      dst.SetInspirationWaveform((eDriverWaveform)src.InspirationWaveform);
      if (src.InspiratoryPeriod != null)
        PBProperty.Load(src.InspiratoryPeriod, dst.GetInspiratoryPeriod());
      if (src.PositiveEndExpiratoryPressure != null)
        PBProperty.Load(src.PositiveEndExpiratoryPressure, dst.GetPositiveEndExpiratoryPressure());
      if (src.RespirationRate != null)
        PBProperty.Load(src.RespirationRate, dst.GetRespirationRate());
      if (src.Slope != null)
        PBProperty.Load(src.Slope, dst.GetSlope());
      if (src.TidalVolume != null)
        PBProperty.Load(src.TidalVolume, dst.GetTidalVolume());
    }
    public static pulse.cdm.bind.MechanicalVentilatorVolumeControlData Unload(SEMechanicalVentilatorVolumeControl src)
    {
      pulse.cdm.bind.MechanicalVentilatorVolumeControlData dst = new pulse.cdm.bind.MechanicalVentilatorVolumeControlData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEMechanicalVentilatorVolumeControl src, pulse.cdm.bind.MechanicalVentilatorVolumeControlData dst)
    {
      dst.MechanicalVentilatorMode = new pulse.cdm.bind.MechanicalVentilatorModeData();
      Serialize(src, dst.MechanicalVentilatorMode);
      if (src.HasFlow())
        dst.Flow = PBProperty.Unload(src.GetFlow());
      if (src.HasFractionInspiredOxygen())
        dst.FractionInspiredOxygen = PBProperty.Unload(src.GetFractionInspiredOxygen());
      if (src.HasInspirationPatientTriggerFlow())
        dst.InspirationPatientTriggerFlow = PBProperty.Unload(src.GetInspirationPatientTriggerFlow());
      if (src.HasInspirationPatientTriggerPressure())
        dst.InspirationPatientTriggerPressure = PBProperty.Unload(src.GetInspirationPatientTriggerPressure());
      dst.InspirationWaveform = (pulse.cdm.bind.eDriverWaveform)(int)src.GetInspirationWaveform();
      if (src.HasInspiratoryPeriod())
        dst.InspiratoryPeriod = PBProperty.Unload(src.GetInspiratoryPeriod());
      if (src.HasPositiveEndExpiratoryPressure())
        dst.PositiveEndExpiratoryPressure = PBProperty.Unload(src.GetPositiveEndExpiratoryPressure());
      if (src.HasRespirationRate())
        dst.RespirationRate = PBProperty.Unload(src.GetRespirationRate());
      if (src.HasSlope())
        dst.Slope = PBProperty.Unload(src.GetSlope());
      if (src.HasTidalVolume())
        dst.TidalVolume = PBProperty.Unload(src.GetTidalVolume());
    }
    #endregion
  }
}
