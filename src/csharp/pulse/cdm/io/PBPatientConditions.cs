/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using pulse.cdm.bind;

namespace Pulse.CDM
{
  public class PBPatientCondition
  {
    #region Any Patient Condition
    /** Create a new Condition based on the binding object, load that data into the new Condition, and return said Condition */
    public static SEPatientCondition Load(pulse.cdm.bind.AnyPatientConditionData any/*, SESubstanceManager subMgr*/)
    {
      if (any.AcuteRespiratoryDistressSyndrome != null)
      {
        SEAcuteRespiratoryDistressSyndrome ards = new SEAcuteRespiratoryDistressSyndrome();
        Serialize(any.AcuteRespiratoryDistressSyndrome, ards);
        return ards;
      }
      if (any.ChronicAnemia != null)
      {
        SEChronicAnemia a = new SEChronicAnemia();
        Serialize(any.ChronicAnemia, a);
        return a;
      }
      if (any.ChronicObstructivePulmonaryDisease != null)
      {
        SEChronicObstructivePulmonaryDisease copd = new SEChronicObstructivePulmonaryDisease();
        Serialize(any.ChronicObstructivePulmonaryDisease, copd);
        return copd;
      }
      if (any.ChronicPericardialEffusion != null)
      {
        SEChronicPericardialEffusion pe = new SEChronicPericardialEffusion();
        Serialize(any.ChronicPericardialEffusion, pe);
        return pe;
      }
      if (any.ChronicRenalStenosis != null)
      {
        SEChronicRenalStenosis rs = new SEChronicRenalStenosis();
        Serialize(any.ChronicRenalStenosis, rs);
        return rs;
      }
      if (any.ChronicVentricularSystolicDysfunction != null)
      {
        SEChronicVentricularSystolicDysfunction cvsd = new SEChronicVentricularSystolicDysfunction();
        Serialize(any.ChronicVentricularSystolicDysfunction, cvsd);
        return cvsd;
      }
      if (any.Dehydration != null)
      {
        SEDehydration d = new SEDehydration();
        Serialize(any.Dehydration, d);
        return d;
      }
      if (any.ImpairedAlveolarExchange != null)
      {
        SEImpairedAlveolarExchange iae = new SEImpairedAlveolarExchange();
        Serialize(any.ImpairedAlveolarExchange, iae);
        return iae;
      }
      if (any.Pneumonia != null)
      {
        SEPneumonia lp = new SEPneumonia();
        Serialize(any.Pneumonia, lp);
        return lp;
      }
      if (any.PulmonaryFibrosis != null)
      {
        SEPulmonaryFibrosis pf = new SEPulmonaryFibrosis();
        Serialize(any.PulmonaryFibrosis, pf);
        return pf;
      }
      if (any.PulmonaryShunt != null)
      {
        SEPulmonaryShunt ps = new SEPulmonaryShunt();
        Serialize(any.PulmonaryShunt, ps);
        return ps;
      }
      if (any.Sepsis != null)
      {
        SESepsis s = new SESepsis();
        Serialize(any.Sepsis, s);
        return s;
      }
      return null;
    }
    /** Create a new bind object, unload the Condition, put that in the bind object, and return said bind object */
    public static pulse.cdm.bind.AnyPatientConditionData Unload(SEPatientCondition Condition)
    {
      pulse.cdm.bind.AnyPatientConditionData any = new pulse.cdm.bind.AnyPatientConditionData();

      if (Condition.GetType().IsAssignableFrom(typeof(SEAcuteRespiratoryDistressSyndrome)))
      {
        any.AcuteRespiratoryDistressSyndrome = Unload((SEAcuteRespiratoryDistressSyndrome)Condition);
        return any;
      }
      if (Condition.GetType().IsAssignableFrom(typeof(SEChronicAnemia)))
      {
        any.ChronicAnemia = Unload((SEChronicAnemia)Condition);
        return any;
      }
      if (Condition.GetType().IsAssignableFrom(typeof(SEChronicObstructivePulmonaryDisease)))
      {
        any.ChronicObstructivePulmonaryDisease = Unload((SEChronicObstructivePulmonaryDisease)Condition);
        return any;
      }
      if (Condition.GetType().IsAssignableFrom(typeof(SEChronicPericardialEffusion)))
      {
        any.ChronicPericardialEffusion = Unload((SEChronicPericardialEffusion)Condition);
        return any;
      }
      if (Condition.GetType().IsAssignableFrom(typeof(SEChronicRenalStenosis)))
      {
        any.ChronicRenalStenosis = Unload((SEChronicRenalStenosis)Condition);
        return any;
      }
      if (Condition.GetType().IsAssignableFrom(typeof(SEChronicVentricularSystolicDysfunction)))
      {
        any.ChronicVentricularSystolicDysfunction = Unload((SEChronicVentricularSystolicDysfunction)Condition);
        return any;
      }
      if (Condition.GetType().IsAssignableFrom(typeof(SEDehydration)))
      {
        any.Dehydration = Unload((SEDehydration)Condition);
        return any;
      }
      if (Condition.GetType().IsAssignableFrom(typeof(SEImpairedAlveolarExchange)))
      {
        any.ImpairedAlveolarExchange = Unload((SEImpairedAlveolarExchange)Condition);
        return any;
      }
      if (Condition.GetType().IsAssignableFrom(typeof(SEPneumonia)))
      {
        any.Pneumonia = Unload((SEPneumonia)Condition);
        return any;
      }
      if (Condition.GetType().IsAssignableFrom(typeof(SEPulmonaryFibrosis)))
      {
        any.PulmonaryFibrosis = Unload((SEPulmonaryFibrosis)Condition);
        return any;
      }
      if (Condition.GetType().IsAssignableFrom(typeof(SEPulmonaryShunt)))
      {
        any.PulmonaryShunt = Unload((SEPulmonaryShunt)Condition);
        return any;
      }
      if (Condition.GetType().IsAssignableFrom(typeof(SESepsis)))
      {
        any.Sepsis = Unload((SESepsis)Condition);
        return any;
      }

      return any;
    }
    #endregion

    #region SEPatientCondition
    public static void Serialize(pulse.cdm.bind.PatientConditionData src, SEPatientCondition dst)
    {
      if (src.Condition != null)
        PBCondition.Serialize(src.Condition, dst);
    }
    static void Serialize(SEPatientCondition src, pulse.cdm.bind.PatientConditionData dst)
    {
      dst.Condition = new pulse.cdm.bind.ConditionData();
      PBCondition.Serialize(src, dst.Condition);
    }
    #endregion

    #region SEAcuteRespiratoryDistressSyndrome
    public static void Load(pulse.cdm.bind.AcuteRespiratoryDistressSyndromeData src, SEAcuteRespiratoryDistressSyndrome dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.AcuteRespiratoryDistressSyndromeData src, SEAcuteRespiratoryDistressSyndrome dst)
    {
      if (src.PatientCondition != null)
        Serialize(src.PatientCondition, dst);
      foreach (var s in src.Severity)
        PBProperty.Load(s.Severity, dst.GetSeverity((eLungCompartment)s.Compartment));
    }
    public static pulse.cdm.bind.AcuteRespiratoryDistressSyndromeData Unload(SEAcuteRespiratoryDistressSyndrome src)
    {
      pulse.cdm.bind.AcuteRespiratoryDistressSyndromeData dst = new pulse.cdm.bind.AcuteRespiratoryDistressSyndromeData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEAcuteRespiratoryDistressSyndrome src, pulse.cdm.bind.AcuteRespiratoryDistressSyndromeData dst)
    {
      dst.PatientCondition = new pulse.cdm.bind.PatientConditionData();
      Serialize(src, dst.PatientCondition);
      foreach (var s in src.GetSeverities())
      {
        LungImpairmentData d = new LungImpairmentData();
        d.Compartment = (pulse.cdm.bind.eLungCompartment)s.Key;
        d.Severity = PBProperty.Unload(s.Value);
        dst.Severity.Add(d);
      }
    }
      #endregion

    #region SEChronicAnemia
      public static void Load(pulse.cdm.bind.ChronicAnemiaData src, SEChronicAnemia dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.ChronicAnemiaData src, SEChronicAnemia dst)
    {
      if (src.PatientCondition != null)
        Serialize(src.PatientCondition, dst);
      if (src.ReductionFactor != null)
        PBProperty.Load(src.ReductionFactor, dst.GetReductionFactor());
    }
    public static pulse.cdm.bind.ChronicAnemiaData Unload(SEChronicAnemia src)
    {
      pulse.cdm.bind.ChronicAnemiaData dst = new pulse.cdm.bind.ChronicAnemiaData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEChronicAnemia src, pulse.cdm.bind.ChronicAnemiaData dst)
    {
      dst.PatientCondition = new pulse.cdm.bind.PatientConditionData();
      Serialize(src, dst.PatientCondition);
      if (src.HasReductionFactor())
        dst.ReductionFactor = PBProperty.Unload(src.GetReductionFactor());
    }
    #endregion

    #region SEChronicObstructivePulmonaryDisease
    public static void Load(pulse.cdm.bind.ChronicObstructivePulmonaryDiseaseData src, SEChronicObstructivePulmonaryDisease dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.ChronicObstructivePulmonaryDiseaseData src, SEChronicObstructivePulmonaryDisease dst)
    {
      if (src.PatientCondition != null)
        Serialize(src.PatientCondition, dst);
      if (src.BronchitisSeverity != null)
        PBProperty.Load(src.BronchitisSeverity, dst.GetBronchitisSeverity());
      foreach (var s in src.EmphysemaSeverity)
        PBProperty.Load(s.Severity, dst.GetEmphysemaSeverity((eLungCompartment)s.Compartment));
    }
    public static pulse.cdm.bind.ChronicObstructivePulmonaryDiseaseData Unload(SEChronicObstructivePulmonaryDisease src)
    {
      pulse.cdm.bind.ChronicObstructivePulmonaryDiseaseData dst = new pulse.cdm.bind.ChronicObstructivePulmonaryDiseaseData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEChronicObstructivePulmonaryDisease src, pulse.cdm.bind.ChronicObstructivePulmonaryDiseaseData dst)
    {
      dst.PatientCondition = new pulse.cdm.bind.PatientConditionData();
      Serialize(src, dst.PatientCondition);
      if (src.HasBronchitisSeverity())
        dst.BronchitisSeverity = PBProperty.Unload(src.GetBronchitisSeverity());
      foreach (var s in src.GetEmphysemaSeverities())
      {
        LungImpairmentData d = new LungImpairmentData();
        d.Compartment = (pulse.cdm.bind.eLungCompartment)s.Key;
        d.Severity = PBProperty.Unload(s.Value);
        dst.EmphysemaSeverity.Add(d);
      }
    }
    #endregion

    #region SEChronicPericardialEffusion
    public static void Load(pulse.cdm.bind.ChronicPericardialEffusionData src, SEChronicPericardialEffusion dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.ChronicPericardialEffusionData src, SEChronicPericardialEffusion dst)
    {
      if (src.PatientCondition != null)
        Serialize(src.PatientCondition, dst);
      if (src.AccumulatedVolume != null)
        PBProperty.Load(src.AccumulatedVolume, dst.GetAccumulatedVolume());
    }
    public static pulse.cdm.bind.ChronicPericardialEffusionData Unload(SEChronicPericardialEffusion src)
    {
      pulse.cdm.bind.ChronicPericardialEffusionData dst = new pulse.cdm.bind.ChronicPericardialEffusionData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEChronicPericardialEffusion src, pulse.cdm.bind.ChronicPericardialEffusionData dst)
    {
      dst.PatientCondition = new pulse.cdm.bind.PatientConditionData();
      Serialize(src, dst.PatientCondition);
      if (src.HasAccumulatedVolume())
        dst.AccumulatedVolume = PBProperty.Unload(src.GetAccumulatedVolume());
    }
    #endregion

    #region SEChronicRenalStenosis
    public static void Load(pulse.cdm.bind.ChronicRenalStenosisData src, SEChronicRenalStenosis dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.ChronicRenalStenosisData src, SEChronicRenalStenosis dst)
    {
      if (src.PatientCondition != null)
        Serialize(src.PatientCondition, dst);
      if (src.LeftKidneySeverity != null)
        PBProperty.Load(src.LeftKidneySeverity, dst.GetLeftKidneySeverity());
      if (src.RightKidneySeverity != null)
        PBProperty.Load(src.RightKidneySeverity, dst.GetRightKidneySeverity());
    }
    public static pulse.cdm.bind.ChronicRenalStenosisData Unload(SEChronicRenalStenosis src)
    {
      pulse.cdm.bind.ChronicRenalStenosisData dst = new pulse.cdm.bind.ChronicRenalStenosisData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEChronicRenalStenosis src, pulse.cdm.bind.ChronicRenalStenosisData dst)
    {
      dst.PatientCondition = new pulse.cdm.bind.PatientConditionData();
      Serialize(src, dst.PatientCondition);
      if (src.HasLeftKidneySeverity())
        dst.LeftKidneySeverity = PBProperty.Unload(src.GetLeftKidneySeverity());
      if (src.HasRightKidneySeverity())
        dst.RightKidneySeverity = PBProperty.Unload(src.GetRightKidneySeverity());
    }
    #endregion

    #region SEChronicVentricularSystolicDysfunction
    public static void Load(pulse.cdm.bind.ChronicVentricularSystolicDysfunctionData src, SEChronicVentricularSystolicDysfunction dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.ChronicVentricularSystolicDysfunctionData src, SEChronicVentricularSystolicDysfunction dst)
    {
      if (src.PatientCondition != null)
        Serialize(src.PatientCondition, dst);
    }
    public static pulse.cdm.bind.ChronicVentricularSystolicDysfunctionData Unload(SEChronicVentricularSystolicDysfunction src)
    {
      pulse.cdm.bind.ChronicVentricularSystolicDysfunctionData dst = new pulse.cdm.bind.ChronicVentricularSystolicDysfunctionData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEChronicVentricularSystolicDysfunction src, pulse.cdm.bind.ChronicVentricularSystolicDysfunctionData dst)
    {
      dst.PatientCondition = new pulse.cdm.bind.PatientConditionData();
      Serialize(src, dst.PatientCondition);
    }
    #endregion

    #region SEDehydration
    public static void Load(pulse.cdm.bind.DehydrationData src, SEDehydration dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.DehydrationData src, SEDehydration dst)
    {
      if (src.PatientCondition != null)
        Serialize(src.PatientCondition, dst);
      if (src.Severity != null)
        PBProperty.Load(src.Severity, dst.GetSeverity());
    }
    public static pulse.cdm.bind.DehydrationData Unload(SEDehydration src)
    {
      pulse.cdm.bind.DehydrationData dst = new pulse.cdm.bind.DehydrationData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEDehydration src, pulse.cdm.bind.DehydrationData dst)
    {
      dst.PatientCondition = new pulse.cdm.bind.PatientConditionData();
      Serialize(src, dst.PatientCondition);
      if (src.HasSeverity())
        dst.Severity = PBProperty.Unload(src.GetSeverity());
    }
    #endregion

    #region SEImpairedAlveolarExchange
    public static void Load(pulse.cdm.bind.ImpairedAlveolarExchangeData src, SEImpairedAlveolarExchange dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.ImpairedAlveolarExchangeData src, SEImpairedAlveolarExchange dst)
    {
      if (src.PatientCondition != null)
        Serialize(src.PatientCondition, dst);
      if (src.ImpairedSurfaceArea != null)
        PBProperty.Load(src.ImpairedSurfaceArea, dst.GetImpairedSurfaceArea());
      else if (src.ImpairedFraction != null)
        PBProperty.Load(src.ImpairedFraction, dst.GetImpairedFraction());
      else if (src.Severity != null)
        PBProperty.Load(src.Severity, dst.GetSeverity());
    }
    public static pulse.cdm.bind.ImpairedAlveolarExchangeData Unload(SEImpairedAlveolarExchange src)
    {
      pulse.cdm.bind.ImpairedAlveolarExchangeData dst = new pulse.cdm.bind.ImpairedAlveolarExchangeData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEImpairedAlveolarExchange src, pulse.cdm.bind.ImpairedAlveolarExchangeData dst)
    {
      dst.PatientCondition = new pulse.cdm.bind.PatientConditionData();
      Serialize(src, dst.PatientCondition);
      if (src.HasImpairedSurfaceArea())
        dst.ImpairedSurfaceArea = PBProperty.Unload(src.GetImpairedSurfaceArea());
      else if (src.HasImpairedFraction())
        dst.ImpairedFraction = PBProperty.Unload(src.GetImpairedFraction());
      else if (src.HasSeverity())
        dst.Severity = PBProperty.Unload(src.GetSeverity());
    }
    #endregion

    #region SEPneumonia
    public static void Load(pulse.cdm.bind.PneumoniaData src, SEPneumonia dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.PneumoniaData src, SEPneumonia dst)
    {
      if (src.PatientCondition != null)
        Serialize(src.PatientCondition, dst);
      foreach (var s in src.Severity)
        PBProperty.Load(s.Severity, dst.GetSeverity((eLungCompartment)s.Compartment));
    }
    public static pulse.cdm.bind.PneumoniaData Unload(SEPneumonia src)
    {
      pulse.cdm.bind.PneumoniaData dst = new pulse.cdm.bind.PneumoniaData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEPneumonia src, pulse.cdm.bind.PneumoniaData dst)
    {
      dst.PatientCondition = new pulse.cdm.bind.PatientConditionData();
      Serialize(src, dst.PatientCondition);
      foreach (var s in src.GetSeverities())
      {
        LungImpairmentData d = new LungImpairmentData();
        d.Compartment = (pulse.cdm.bind.eLungCompartment)s.Key;
        d.Severity = PBProperty.Unload(s.Value);
        dst.Severity.Add(d);
      }
    }
    #endregion

    #region SEPulmonaryFibrosis
    public static void Load(pulse.cdm.bind.PulmonaryFibrosisData src, SEPulmonaryFibrosis dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.PulmonaryFibrosisData src, SEPulmonaryFibrosis dst)
    {
      if (src.PatientCondition != null)
        Serialize(src.PatientCondition, dst);
      if (src.Severity != null)
        PBProperty.Load(src.Severity, dst.GetSeverity());
    }
    public static pulse.cdm.bind.PulmonaryFibrosisData Unload(SEPulmonaryFibrosis src)
    {
      pulse.cdm.bind.PulmonaryFibrosisData dst = new pulse.cdm.bind.PulmonaryFibrosisData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEPulmonaryFibrosis src, pulse.cdm.bind.PulmonaryFibrosisData dst)
    {
      dst.PatientCondition = new pulse.cdm.bind.PatientConditionData();
      Serialize(src, dst.PatientCondition);
      if (src.HasSeverity())
        dst.Severity = PBProperty.Unload(src.GetSeverity());
    }
    #endregion

    #region SEPulmonaryShunt
    public static void Load(pulse.cdm.bind.PulmonaryShuntData src, SEPulmonaryShunt dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.PulmonaryShuntData src, SEPulmonaryShunt dst)
    {
      if (src.PatientCondition != null)
        Serialize(src.PatientCondition, dst);
      if (src.Severity != null)
        PBProperty.Load(src.Severity, dst.GetSeverity());
    }
    public static pulse.cdm.bind.PulmonaryShuntData Unload(SEPulmonaryShunt src)
    {
      pulse.cdm.bind.PulmonaryShuntData dst = new pulse.cdm.bind.PulmonaryShuntData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SEPulmonaryShunt src, pulse.cdm.bind.PulmonaryShuntData dst)
    {
      dst.PatientCondition = new pulse.cdm.bind.PatientConditionData();
      Serialize(src, dst.PatientCondition);
      if (src.HasSeverity())
        dst.Severity = PBProperty.Unload(src.GetSeverity());
    }
    #endregion

    #region SESepsis
    public static void Load(pulse.cdm.bind.SepsisData src, SESepsis dst)
    {
      Serialize(src, dst);
    }
    public static void Serialize(pulse.cdm.bind.SepsisData src, SESepsis dst)
    {
      if (src.PatientCondition != null)
        Serialize(src.PatientCondition, dst);
      if (src.Severity != null)
        PBProperty.Load(src.Severity, dst.GetSeverity());
    }
    public static pulse.cdm.bind.SepsisData Unload(SESepsis src)
    {
      pulse.cdm.bind.SepsisData dst = new pulse.cdm.bind.SepsisData();
      Serialize(src, dst);
      return dst;
    }
    public static void Serialize(SESepsis src, pulse.cdm.bind.SepsisData dst)
    {
      dst.PatientCondition = new pulse.cdm.bind.PatientConditionData();
      Serialize(src, dst.PatientCondition);
      if (src.HasSeverity())
        dst.Severity = PBProperty.Unload(src.GetSeverity());
    }
    #endregion

  }
}
