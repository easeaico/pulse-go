/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

namespace Pulse.CDM
{
  public class SEConditionManager
  {
    protected SEAcuteRespiratoryDistressSyndrome ards;
    protected SEChronicAnemia anemia;
    //protected SEConsumeMeal consume_meal;
    protected SEChronicObstructivePulmonaryDisease copd;
    protected SEChronicVentricularSystolicDysfunction cvsd;
    protected SEDehydration dehydration;
    protected SEImpairedAlveolarExchange impaired_alveolar_exchange;
    protected SEChronicPericardialEffusion pericardial_effusion;
    protected SEPneumonia pneumonia;
    protected SEPulmonaryFibrosis pulmonary_fibrosis;
    protected SEPulmonaryShunt pulmonary_shunt;
    protected SEChronicRenalStenosis renal_stenosis;
    protected SESepsis sepsis;

    protected SEInitialEnvironmentalConditions initial_environmental_conditions;

    public SEConditionManager()
    {
      Clear();
    }

    public void Clear()
    {
      ards = null;
      anemia = null;
      //consume_meal = null;
      copd = null;
      cvsd = null;
      dehydration = null;
      impaired_alveolar_exchange = null;
      pericardial_effusion = null;
      pneumonia = null;
      pulmonary_fibrosis = null;
      pulmonary_shunt = null;
      renal_stenosis = null;
      sepsis = null;
      initial_environmental_conditions = null;
    }

    public bool IsEmpty()
    {
      if (HasAcuteRespiratoryDistressSyndrome())
        return false;
      if (HasChronicAnemia())
        return false;
      if (HasChronicObstructivePulmonaryDisease())
        return false;
      if (HasChronicVentricularSystolicDysfunction())
        return false;
      if (HasChronicPericardialEffusion())
        return false;
      if (HasChronicRenalStenosis())
        return false;
      //if (HasConsumeMeal())
      //  return false;
      if (HasDehydration())
        return false;
      if (HasImpairedAlveolarExchange())
        return false;
      if (HasPneumonia())
        return false;
      if (HasPulmonaryFibrosis())
        return false;
      if (HasPulmonaryShunt())
        return false;
      if (HasSepsis())
        return false;

      if (HasInitialEnvironmentalConditions())
        return false;
      return true;
    }

    public bool Add(SECondition c)
    {
      if (c.GetType().IsAssignableFrom(typeof(SEAcuteRespiratoryDistressSyndrome)))
      {
        ards = (SEAcuteRespiratoryDistressSyndrome)c;
        return true;
      }
      if (c.GetType().IsAssignableFrom(typeof(SEChronicAnemia)))
      {
        anemia = (SEChronicAnemia)c;
        return true;
      }
      if (c.GetType().IsAssignableFrom(typeof(SEChronicObstructivePulmonaryDisease)))
      {
        copd = (SEChronicObstructivePulmonaryDisease)c;
        return true;
      }
      if (c.GetType().IsAssignableFrom(typeof(SEChronicVentricularSystolicDysfunction)))
      {
        cvsd = (SEChronicVentricularSystolicDysfunction)c;
        return true;
      }
      if (c.GetType().IsAssignableFrom(typeof(SEChronicPericardialEffusion)))
      {
        pericardial_effusion = (SEChronicPericardialEffusion)c;
        return true;
      }
      if (c.GetType().IsAssignableFrom(typeof(SEChronicRenalStenosis)))
      {
        renal_stenosis = (SEChronicRenalStenosis)c;
        return true;
      }
      //if (c.GetType().IsAssignableFrom(typeof(SEConsumeMeal)))
      //{
      //  consume_meal = (SEConsumeMeal)c;
      //  return true;
      //}
      if (c.GetType().IsAssignableFrom(typeof(SEDehydration)))
      {
        dehydration = (SEDehydration)c;
        return true;
      }
      if (c.GetType().IsAssignableFrom(typeof(SEImpairedAlveolarExchange)))
      {
        impaired_alveolar_exchange = (SEImpairedAlveolarExchange)c;
        return true;
      }
      if (c.GetType().IsAssignableFrom(typeof(SEPneumonia)))
      {
        pneumonia = (SEPneumonia)c;
        return true;
      }
      if (c.GetType().IsAssignableFrom(typeof(SEPulmonaryFibrosis)))
      {
        pulmonary_fibrosis = (SEPulmonaryFibrosis)c;
        return true;
      }
      if (c.GetType().IsAssignableFrom(typeof(SEPulmonaryShunt)))
      {
        pulmonary_shunt = (SEPulmonaryShunt)c;
        return true;
      }
      if (c.GetType().IsAssignableFrom(typeof(SESepsis)))
      {
        sepsis = (SESepsis)c;
        return true;
      }

      if (c.GetType().IsAssignableFrom(typeof(SEInitialEnvironmentalConditions)))
      {
        initial_environmental_conditions = (SEInitialEnvironmentalConditions)c;
        return true;
      }
      return false;
    }

    public bool HasAcuteRespiratoryDistressSyndrome()
    {
      return ards == null ? false : ards.IsValid();
    }
    public SEAcuteRespiratoryDistressSyndrome GetAcuteRespiratoryDistressSyndrome()
    {
      if (ards == null)
        ards = new SEAcuteRespiratoryDistressSyndrome();
      return ards;
    }
    public void RemoveAcuteRespiratoryDistressSyndrome()
    {
      ards = null;
    }

    public bool HasChronicAnemia()
    {
      return anemia == null ? false : anemia.IsValid();
    }
    public SEChronicAnemia GetChronicAnemia()
    {
      if (anemia == null)
        anemia = new SEChronicAnemia();
      return anemia;
    }
    public void RemoveChronicAnemia()
    {
      anemia = null;
    }

    public bool HasChronicObstructivePulmonaryDisease()
    {
      return copd == null ? false : copd.IsValid();
    }
    public SEChronicObstructivePulmonaryDisease GetChronicObstructivePulmonaryDisease()
    {
      if (copd == null)
        copd = new SEChronicObstructivePulmonaryDisease();
      return copd;
    }
    public void RemoveChronicObstructivePulmonaryDisease()
    {
      copd = null;
    }

    public bool HasChronicHeartFailure()
    {
      return HasChronicVentricularSystolicDysfunction();
    }
    public bool HasChronicVentricularSystolicDysfunction()
    {
      return cvsd == null ? false : cvsd.IsValid();
    }
    public SEChronicVentricularSystolicDysfunction GetChronicVentricularSystolicDysfunction()
    {
      if (cvsd == null)
        cvsd = new SEChronicVentricularSystolicDysfunction();
      return cvsd;
    }
    public void RemoveChronicVentricularSystolicDysfunction()
    {
      cvsd = null;
    }

    public bool HasChronicPericardialEffusion()
    {
      return pericardial_effusion == null ? false : pericardial_effusion.IsValid();
    }
    public SEChronicPericardialEffusion GetChronicPericardialEffusion()
    {
      if (pericardial_effusion == null)
        pericardial_effusion = new SEChronicPericardialEffusion();
      return pericardial_effusion;
    }
    public void RemoveChronicPericardialEffusion()
    {
      pericardial_effusion = null;
    }

    public bool HasChronicRenalStenosis()
    {
      return renal_stenosis == null ? false : renal_stenosis.IsValid();
    }
    public SEChronicRenalStenosis GetChronicRenalStenosis()
    {
      if (renal_stenosis == null)
        renal_stenosis = new SEChronicRenalStenosis();
      return renal_stenosis;
    }
    public void RemoveChronicRenalStenosis()
    {
      renal_stenosis = null;
    }

    //public bool HasConsumeMeal()
    //public SEConsumeMeal GetConsumeMeal()
    //public void RemoveConsumeMeal()

    public bool HasDehydration()
    {
      return dehydration == null ? false : dehydration.IsValid();
    }
    public SEDehydration GetDehydration()
    {
      if (dehydration == null)
        dehydration = new SEDehydration();
      return dehydration;
    }
    public void RemoveDehydration()
    {
      dehydration = null;
    }

    public bool HasImpairedAlveolarExchange()
    {
      return impaired_alveolar_exchange == null ? false : impaired_alveolar_exchange.IsValid();
    }
    public SEImpairedAlveolarExchange GetImpairedAlveolarExchange()
    {
      if (impaired_alveolar_exchange == null)
        impaired_alveolar_exchange = new SEImpairedAlveolarExchange();
      return impaired_alveolar_exchange;
    }
    public void RemoveImpairedAlveolarExchange()
    {
      impaired_alveolar_exchange = null;
    }

    public bool HasPneumonia()
    {
      return pneumonia == null ? false : pneumonia.IsValid();
    }
    public SEPneumonia GetPneumonia()
    {
      if (pneumonia == null)
        pneumonia = new SEPneumonia();
      return pneumonia;
    }
    public void RemovePneumonia()
    {
      pneumonia = null;
    }

    public bool HasPulmonaryFibrosis()
    {
      return pulmonary_fibrosis == null ? false : pulmonary_fibrosis.IsValid();
    }
    public SEPulmonaryFibrosis GetPulmonaryFibrosis()
    {
      if (pulmonary_fibrosis == null)
        pulmonary_fibrosis = new SEPulmonaryFibrosis();
      return pulmonary_fibrosis;
    }
    public void RemovePulmonaryFibrosis()
    {
      pulmonary_fibrosis = null;
    }

    public bool HasPulmonaryShunt()
    {
      return pulmonary_shunt == null ? false : pulmonary_shunt.IsValid();
    }
    public SEPulmonaryShunt GetPulmonaryShunt()
    {
      if (pulmonary_shunt == null)
        pulmonary_shunt = new SEPulmonaryShunt();
      return pulmonary_shunt;
    }
    public void RemovePulmonaryShunt()
    {
      pulmonary_shunt = null;
    }

    public bool HasSepsis()
    {
      return sepsis == null ? false : sepsis.IsValid();
    }
    public SESepsis GetSepsis()
    {
      if (sepsis == null)
        sepsis = new SESepsis();
      return sepsis;
    }
    public void RemoveSepsis()
    {
      sepsis = null;
    }

    // Environment Conditions
    public bool HasInitialEnvironmentalConditions()
    {
      return initial_environmental_conditions == null ? false : initial_environmental_conditions.IsValid();
    }
    public SEInitialEnvironmentalConditions GetInitialEnvironmentalConditions()
    {
      if (initial_environmental_conditions == null)
        initial_environmental_conditions = new SEInitialEnvironmentalConditions();
      return initial_environmental_conditions;
    }
    public void RemoveInitialEnvironmentalConditions()
    {
      initial_environmental_conditions = null;
    }
  }
}
