/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
package com.kitware.pulse.cdm.validation;

import com.kitware.pulse.cdm.bind.Validation.SegmentValidationTargetData;

public class SESegmentValidationTarget extends SEValidationTarget
{
  protected String comparisonFormula;
  
  public SESegmentValidationTarget()
  {
    super();
  }

  public void clear()
  {
    super.clear();
    comparisonFormula = null;
  }

  public static void load(SegmentValidationTargetData src, SESegmentValidationTarget dst)
  {
    SEValidationTarget.load(src.getValidationTarget(), dst);
    dst.comparisonFormula = src.getComparisonFormula();
    
  }
  public static SegmentValidationTargetData unload(SESegmentValidationTarget src)
  {
    SegmentValidationTargetData.Builder dst = SegmentValidationTargetData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(SESegmentValidationTarget src, SegmentValidationTargetData.Builder dst)
  {
    SEValidationTarget.unload(src,dst.getValidationTargetBuilder());
    dst.setComparisonFormula(src.comparisonFormula);
  }
  
  public String getComparisonFormula() { return comparisonFormula; }
  public void setComparisonFormula(String f) { comparisonFormula = f; }
}
