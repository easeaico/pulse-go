/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

namespace Pulse.CDM
{
  public class SEAdvanceUntilStable : SEAction
  {
    protected string criteria;

    public SEAdvanceUntilStable()
    {
      
    }

    public override void Clear()
    {
      base.Clear();
      criteria = "AdvanceUntilStable";
    }

    public override bool IsValid()
    {
      return HasCriteria();
    }

    public bool HasCriteria()
    {
      return criteria != null && criteria.Length > 0;
    }
    public string GetCriteria()
    {
      return criteria;
    }
    public void SetCriteria(string c)
    {
      criteria = c;
    }
  }
}
