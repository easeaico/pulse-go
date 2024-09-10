/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.patient.actions;

import com.kitware.pulse.cdm.bind.PatientActions.RespiratoryMechanicsModificationData;
import com.kitware.pulse.cdm.system.physiology.SERespiratoryMechanicsModifiers;

public class SERespiratoryMechanicsModification extends SEPatientAction
  {
    protected SERespiratoryMechanicsModifiers modifiers = null;
    protected String modifiersFile = "";
    protected boolean incremental = false;

    public SERespiratoryMechanicsModification()
    {
      super();
    }

    public SERespiratoryMechanicsModification(SERespiratoryMechanicsModification other)
    {
      copy(other);
    }

    public void copy(SERespiratoryMechanicsModification other)
    {
      super.copy(other);
      if (other.modifiers != null)
        this.getModifiers().copy(other.modifiers);
      this.modifiersFile = other.modifiersFile;
      this.incremental = other.incremental;
    }

    public void clear()
    {
      super.clear();
      if (this.modifiers != null)
        this.modifiers.clear();
      this.modifiersFile = "";
      this.incremental = false;
    }
    
    public static void load(RespiratoryMechanicsModificationData src, SERespiratoryMechanicsModification dst)
    {
      if (src.hasModifiers())
        SERespiratoryMechanicsModifiers.load(src.getModifiers(), dst.getModifiers());
      else if (src.getModifiersFile().isEmpty())
        dst.setModifiersFile(src.getModifiersFile());
      dst.incremental = src.getIncremental();
    }
    public static RespiratoryMechanicsModificationData unload(SERespiratoryMechanicsModification src)
    {
      RespiratoryMechanicsModificationData.Builder dst = RespiratoryMechanicsModificationData.newBuilder();
      unload(src,dst);
      return dst.build();
    }
    protected static void unload(SERespiratoryMechanicsModification src, RespiratoryMechanicsModificationData.Builder dst)
    {
      if(src.hasModifiers())
        dst.setModifiers(SERespiratoryMechanicsModifiers.unload(src.getModifiers()));
      else if (src.hasModifiersFile())
        dst.setModifiersFile(src.getModifiersFile());
      dst.setIncremental(src.incremental);
    }

    public boolean isValid()
    {
      return hasModifiers() || hasModifiersFile();
    }

    public boolean getIncremental() { return this.incremental; }
    void setIncremental(boolean b) { this.incremental = b; }

    public boolean hasModifiers()
    {
      return this.modifiers != null;
    }
    public SERespiratoryMechanicsModifiers getModifiers()
    {
      if (this.modifiers == null)
        this.modifiers = new SERespiratoryMechanicsModifiers();
      return this.modifiers;
    }

    public boolean hasModifiersFile()
    {
      return this.modifiersFile != null && !this.modifiersFile.isEmpty();
    }
    public String getModifiersFile()
    {
      return this.modifiersFile;
    }
    public void setModifiersFile(String s)
    {
      this.modifiersFile = s;
    }

    public String toString()
    {
      String str = "Respiratory Mechanics Modification";
      if (this.hasModifiersFile())
        str += "\n\tModifiers File: " + this.modifiersFile;
      else if (hasModifiers())
      {
        str += modifiers.toString();
      }
      str += "\n\tIncremental: " + this.incremental;
      return str;
    }
  }
