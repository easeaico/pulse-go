/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

using System;
using System.Linq;

namespace Pulse.CDM
{
  public enum eMergeType : int
  {
    Append = 0,
    Replace
  }
  
  public enum eAppliedRespiratoryCycle : int
  {
    Expiratory = 0,
    Inspiratory,
    Instantaneous
  }
  public abstract class SEAction
  {
    protected string comment;

    public SEAction()
    {
      Clear();
    }

    public void Copy(SEAction other)
    {
      Clear();
      comment = other.comment;
    }

    public virtual void Clear()
    {
      comment = null;
    }

    public bool HasComment()
    {
      return !string.IsNullOrEmpty(comment);
    }
    public string GetComment()
    {
      return this.comment;
    }
    public void SetComment(string comment)
    {
      this.comment = comment;
    }
    public void InvalidateComment()
    {
      this.comment = null;
    }

    public abstract bool IsValid();

    public static string PrettyPrint(string msg)
    {
      return Logger.PrettyPrint(msg, Logger.ePrettyPrintType.Action);
    }
  }
}
