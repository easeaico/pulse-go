/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.actions;

import com.kitware.pulse.cdm.bind.Actions.AdvanceUntilStableData;

public class SEAdvanceUntilStable extends SEAction 
{
  private static final long serialVersionUID = -8603576930859749559L;
  
  protected String criteria;
  
  public SEAdvanceUntilStable() 
  {
    
  }
  
  public void copy(SEAdvanceUntilStable other)
  {
    if(this==other)
      return;
    super.copy(other);
    criteria = other.criteria;
  }
  
  @Override
  public void clear() 
  {
    super.clear();
    criteria = "AdvanceUntilStable";
  }
  
  @Override
  public boolean isValid()
  {
    return hasCriteria();
  }

  public static void load(AdvanceUntilStableData src, SEAdvanceUntilStable dst) 
  {
    SEAction.load(src.getAction(), dst);
    dst.setCriteria(src.getCriteria());
  }
  public static AdvanceUntilStableData unload(SEAdvanceUntilStable src)
  {
    AdvanceUntilStableData.Builder dst = AdvanceUntilStableData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(SEAdvanceUntilStable src, AdvanceUntilStableData.Builder dst)
  {
    SEAction.unload(src, dst.getActionBuilder());
    dst.setCriteria(src.getCriteria());
  }
  
  public boolean hasCriteria()
  {
    return criteria != null && !criteria.isEmpty();
  }
  public String getCriteria() 
  {
    return criteria;
  }
  public void setCriteria(String c) 
  {
    criteria = c;
  }
  
  @Override
  public String toString() 
  {
    return "Advance Until Stable" 
        + "\n\tCriteria: " + getCriteria();
  }
}
