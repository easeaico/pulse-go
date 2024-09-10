/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
package com.kitware.pulse.cdm.validation;

import com.kitware.pulse.cdm.bind.Validation.ValidationTargetData;

public class SEValidationTarget
{
  protected String          m_Header;
  protected String          m_Reference;
  protected String          m_Notes;
  protected double          m_Target;
  protected double          m_TargetMaximum;
  protected double          m_TargetMinimum;
  
  public SEValidationTarget()
  {
    clear();
  }
  
  public void clear()
  {
    m_Header = "";
    m_Reference = "";
    m_Notes = "";
    m_Target         = Double.NaN;
    m_TargetMaximum  = Double.NaN;
    m_TargetMinimum  = Double.NaN;
  }
  
  public static void load(ValidationTargetData src, SEValidationTarget dst)
  {
    dst.m_Header = src.getHeader();
    dst.m_Reference = src.getReference();
    dst.m_Notes = src.getNotes();
  }
  public static ValidationTargetData unload(SEValidationTarget src)
  {
    ValidationTargetData.Builder dst = ValidationTargetData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(SEValidationTarget src, ValidationTargetData.Builder dst)
  {
    dst.setHeader(src.m_Header);
    dst.setReference(src.m_Reference);
    dst.setNotes(src.m_Notes);
  }
  
  public String getHeader() { return m_Header; }
  public void setHeader(String h) { m_Header = h; }

  public String getReference() { return m_Reference; }
  public void setReference(String c) { m_Reference = c; }

  public String getNotes() { return m_Notes; }
  public void setNotes(String n) { m_Notes = n; }

  public double getTargetMaximum() { return m_TargetMaximum; }
  public double getTargetMinimum() { return m_TargetMinimum; }
  public double getTarget() { return m_Target; }
}
