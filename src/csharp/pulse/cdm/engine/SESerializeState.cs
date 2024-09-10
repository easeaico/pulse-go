/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

namespace Pulse.CDM
{
  public enum eSerialization_Mode : int
  {
    Save = 0,
    Load
  }
  public class SESerializeState : SEAction
  {
    protected string filename;
    protected eSerialization_Mode mode;

    public SESerializeState()
    {
      Clear();
    }

    public override void Clear()
    {
      base.Clear();
      filename = null;
      mode = eSerialization_Mode.Save;
    }

    public override bool IsValid()
    {
      return HasFilename();
    }

    public eSerialization_Mode GetMode() { return mode; }
    public void SetMode(eSerialization_Mode t) { mode = t; }

    public bool HasFilename()
    {
      return !string.IsNullOrEmpty(filename);
    }
    public string GetFilename()
    {
      return filename;
    }
    public void SetFilename(string f)
    {
      filename = f;
    }
  }
}
