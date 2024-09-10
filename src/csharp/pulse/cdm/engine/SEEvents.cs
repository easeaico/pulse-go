/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

namespace Pulse.CDM
{
  public enum eEvent : int
  {
    Antidiuresis                    = 0,
    Bradycardia                     = 1,
    Bradypnea                       = 2,
    BrainOxygenDeficit              = 3,
    CardiacArrest                   = 4,
    CardiogenicShock                = 5,
    CardiovascularCollapse          = 6,
    CriticalBrainOxygenDeficit      = 7,
    Diuresis                        = 8,
    Fasciculation                   = 9,
    Fatigue                         = 10,
    FunctionalIncontinence          = 11,
    Hypercapnia                     = 12,
    Hyperglycemia                   = 13,
    Hypernatremia                   = 14,
    Hyperthermia                    = 15,
    Hypoglycemia                    = 16,
    Hyponatremia                    = 17,
    Hypothermia                     = 18,
    Hypoxia                         = 19,
    HypovolemicShock                = 20,
    IntracranialHypertension        = 21,
    IntracranialHypotension         = 22,
    IrreversibleState               = 23,
    Ketoacidosis                    = 24,
    LacticAcidosis                  = 25,
    MassiveHemothorax               = 26,
    MaximumPulmonaryVentilationRate = 27,
    MediumHemothorax                = 28,
    MetabolicAcidosis               = 29,
    MetabolicAlkalosis              = 30,
    MildDehydration                 = 31,
    MinimalHemothorax               = 32,
    ModerateDehydration             = 33,
    ModerateHyperoxemia             = 34,
    ModerateHypocapnia              = 35,
    MyocardiumOxygenDeficit         = 36,
    Natriuresis                     = 37,
    NutritionDepleted               = 38,
    RenalHypoperfusion              = 39,
    RespiratoryAcidosis             = 40,
    RespiratoryAlkalosis            = 41,
    SevereDehydration               = 42,
    SevereHyperoxemia               = 43,
    SevereHypocapnia                = 44,
    Stabilizing                     = 45,
    StartOfCardiacCycle             = 46,
    StartOfExhale                   = 47,
    StartOfInhale                   = 48,
    Tachycardia                     = 49,
    Tachypnea                       = 50,

    // Equipment
    AnesthesiaMachineOxygenBottleOneExhausted = 1000,
    AnesthesiaMachineOxygenBottleTwoExhausted = 1001,
    AnesthesiaMachineReliefValveActive = 1002,
    MechanicalVentilatorReliefValveActive = 1003,
    SupplementalOxygenBottleExhausted = 1004,
    NonRebreatherMaskOxygenBagEmpty = 1005
  }

  public interface IEventHandler
  {
    void HandleEvent(SEEventChange change);
  }

  public class SEActiveEvent
  {
    public eEvent Event { get; }
    public SEScalarTime Duration { get; }

    public SEActiveEvent(eEvent e, SEScalarTime duration)
    {
      Event = e;
      Duration = new SEScalarTime(duration);
    }

    public new string ToString()
    {
      return Event + " has been active for " + Duration.ToString();
    }
  }

  public class SEEventChange
  {
    public eEvent Event { get; }
    public bool Active { get; }
    public SEScalarTime SimTime { get; }

    public SEEventChange(eEvent e, bool active, SEScalarTime simTime)
    {
      Event = e;
      Active = active;
      SimTime = new SEScalarTime(simTime);
    }

    public new string ToString()
    {
      string str;
      str = Event + " was " + Active;
      if (SimTime.IsValid())
        str += " @ " + SimTime.ToString();
      return str;
    }
  }
}
