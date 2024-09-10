/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

namespace Pulse.CDM
{
  public class SERespiratoryMechanicsModification : SEPatientAction
  {
    protected SERespiratoryMechanicsModifiers modifiers = null;
    protected string modifiersFile = "";
    protected bool incremental = false;

    public SERespiratoryMechanicsModification()
    {

    }

    public SERespiratoryMechanicsModification(SERespiratoryMechanicsModification other)
    {
      Copy(other);
    }

    public void Copy(SERespiratoryMechanicsModification other)
    {
      base.Copy(other);
      if (other.modifiers != null)
        this.GetModifiers().Copy(other.modifiers);
      this.modifiersFile = other.modifiersFile;
      this.incremental = other.incremental;
    }

    public bool GetIncremental() { return incremental; }
    public void SetIncremental(bool b) { incremental = b; }

    public override void Clear()
    {
      base.Clear();
      if (this.modifiers != null)
        this.modifiers.Clear();
      this.modifiersFile = "";
      incremental = false;
    }

    public override bool IsValid()
    {
      return HasModifiers() || HasModifiersFile();
    }

    public bool HasModifiers()
    {
      return this.modifiers != null;
    }
    public SERespiratoryMechanicsModifiers GetModifiers()
    {
      if (this.modifiers == null)
        this.modifiers = new SERespiratoryMechanicsModifiers();
      return this.modifiers;
    }

    public bool HasModifiersFile()
    {
      return !string.IsNullOrEmpty(this.modifiersFile);
    }
    public string GetModifiersFile()
    {
      return this.modifiersFile;
    }
    public void SetModificationFile(string s)
    {
      this.modifiersFile = s;
    }

    public override string ToString()
    {
      string str = "Respiratory Mechanics Modification";
      if (this.HasModifiersFile())
        str += "\n\tModifiers File: " + this.modifiersFile;
      else if (HasModifiers())
      {
        str += modifiers.ToString();
      }
      str += "\n\tIncremental: " + this.incremental;
      return str;
    }
  }
}
