/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.actions;

import com.kitware.pulse.cdm.bind.Actions.SerializeStateData;

public class SESerializeState extends SEAction 
{
  private static final long serialVersionUID = 3176123488867413596L;
  
  protected String                   filename;
  protected SerializeStateData.eMode mode;
  
  public SESerializeState() 
  {
    filename = "";
    mode = null;
  }
  
  public void copy(SESerializeState other)
  {
    if(this==other)
      return;
    super.copy(other);
    this.filename = other.filename;
    this.mode = other.mode;
  }
  
  @Override
  public void clear() 
  {
    super.clear();
    filename = "";
    mode = null;
  }
  
  @Override
  public boolean isValid()
  {
    return hasMode();
  }

  public static void load(SerializeStateData src, SESerializeState dst) 
  {
    SEAction.load(src.getAction(), dst);
    if(src.getMode()!=SerializeStateData.eMode.UNRECOGNIZED)
      dst.mode = src.getMode();
    if(src.getFilename()!=null)
      dst.setFilename(src.getFilename());
  }
  public static SerializeStateData unload(SESerializeState src)
  {
    SerializeStateData.Builder dst = SerializeStateData.newBuilder();
    unload(src,dst);
    return dst.build();
  }  
  protected static void unload(SESerializeState src, SerializeStateData.Builder dst)
  {
    SEAction.unload(src, dst.getActionBuilder());
    if(src.hasFilename())
      dst.setFilename(src.filename);
    if(src.hasMode())
      dst.setMode(src.mode);
  }
  
  public boolean hasFilename()
  {
    return !(filename==null || filename.isEmpty());
  }
  public String getFilename() 
  {
    return filename;
  }
  public void setFilename(String filename)
  {
    this.filename = filename;
  }
  public void invalidateFilename()
  {
    this.filename = "";
  }
  
  public boolean hasMode()
  {
    return mode!=null;
  }
  public SerializeStateData.eMode getMode() 
  {
    return mode;
  }
  public void setMode(SerializeStateData.eMode t)
  {
    this.mode = t;
  }
  public void invalidateType()
  {
    this.mode = null;
  }
  
  @Override
  public String toString() 
  {
    return "Serialize State" 
        + "\n\tMode: " + getMode()
        + "\n\tFilename: " + getFilename();
  }
}
