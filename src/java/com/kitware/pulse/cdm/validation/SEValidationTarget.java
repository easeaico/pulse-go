/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
package com.kitware.pulse.cdm.validation;

import com.kitware.pulse.cdm.bind.Validation.ValidationTargetData;

public class SEValidationTarget
{
  protected String header;
  protected String reference;
  protected String notes;
  protected double target;
  protected double targetMaximum;
  protected double targetMinimum;

  protected double goodPercentError;
  protected double fairPercentError;
  
  public SEValidationTarget()
  {
    clear();
  }
  
  public void clear()
  {
    header = "";
    reference = "";
    notes = "";
    target         = Double.NaN;
    targetMaximum  = Double.NaN;
    targetMinimum  = Double.NaN;
  }
  
  public static void load(ValidationTargetData src, SEValidationTarget dst)
  {
    dst.header = src.getHeader();
    dst.reference = src.getReference();
    dst.notes = src.getNotes();

    dst.goodPercentError = src.getGoodPercentError();
    dst.fairPercentError = src.getFairPercentError();
  }
  public static ValidationTargetData unload(SEValidationTarget src)
  {
    ValidationTargetData.Builder dst = ValidationTargetData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(SEValidationTarget src, ValidationTargetData.Builder dst)
  {
    dst.setHeader(src.header);
    dst.setReference(src.reference);
    dst.setNotes(src.notes);

    dst.setGoodPercentError(src.goodPercentError);
    dst.setFairPercentError(src.fairPercentError);
  }
  
  public String getHeader() { return header; }
  public void setHeader(String h) { header = h; }

  public String getReference() { return reference; }
  public void setReference(String c) { reference = c; }

  public String getNotes() { return notes; }
  public void setNotes(String n) { notes = n; }

  public double getTargetMaximum() { return targetMaximum; }
  public double getTargetMinimum() { return targetMinimum; }
  public double getTarget() { return target; }
  
  public double getGoodPercentError() { return goodPercentError; }
  public void setGoodPercentError(double d) { goodPercentError = d; }
  
  public double getFairPercentError() { return fairPercentError; }
  public void setFairPercentError(double d) { fairPercentError = d; }
}
