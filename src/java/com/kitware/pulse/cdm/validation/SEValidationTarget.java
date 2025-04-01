/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
package com.kitware.pulse.cdm.validation;

import com.kitware.pulse.cdm.bind.Validation.ValidationTargetData;

public class SEValidationTarget
{
  protected String header;
  protected String reference;
  protected String notes;
  protected String tableFormatting;
  
  protected String computedEnum;
  protected double computedValue;

  protected double error;
  protected double goodPercentError;
  protected double fairPercentError;
  
  public SEValidationTarget()
  {
    clear();
  }
  
  public void clear()
  {
    header           = "";
    reference        = "";
    notes            = "";
    tableFormatting  = "";
    
    computedEnum     = "";
    computedValue    = Double.NaN;
    
    error            = Double.NaN;
    goodPercentError = Double.NaN;
    fairPercentError = Double.NaN;
  }
  
  public static void load(ValidationTargetData src, SEValidationTarget dst)
  {
    dst.header = src.getHeader();
    dst.reference = src.getReference();
    dst.notes = src.getNotes();
    dst.tableFormatting = src.getTableFormatting();
    
    dst.error = src.getError();
    dst.computedEnum = src.getComputedEnum();
    dst.computedValue = src.getComputedValue();
    
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
    dst.setTableFormatting(src.tableFormatting);
    
    dst.setComputedEnum(src.computedEnum);
    dst.setComputedValue(src.computedValue);

    dst.setError(src.error);
    dst.setGoodPercentError(src.goodPercentError);
    dst.setFairPercentError(src.fairPercentError);
  }
  
  public String getHeader() { return header; }
  public void setHeader(String h) { header = h; }

  public String getReference() { return reference; }
  public void setReference(String c) { reference = c; }

  public String getNotes() { return notes; }
  public void setNotes(String n) { notes = n; }
  
  public String getTableFormatting() { return tableFormatting; }
  public void setTableFormatting(String n) { tableFormatting = n; }
  
  public String getComputedEnum() { return computedEnum; }
  public void setComputedEnum(String s) { computedEnum = s; }

  public double getComputedValue() { return computedValue; }
  public void setComputedValue(double v) { computedValue = v; }
  
  public double getError() { return error; }
  public void setError(double v) { error = v; }
  
  public double getGoodPercentError() { return goodPercentError; }
  public void setGoodPercentError(double d) { goodPercentError = d; }
  
  public double getFairPercentError() { return fairPercentError; }
  public void setFairPercentError(double d) { fairPercentError = d; }
}
