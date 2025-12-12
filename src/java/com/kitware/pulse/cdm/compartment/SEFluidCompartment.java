/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.compartment;

import com.kitware.pulse.cdm.bind.Compartment.FluidCompartmentData;
import com.kitware.pulse.cdm.properties.SEScalarPressure;
import com.kitware.pulse.cdm.properties.SEScalarVolume;
import com.kitware.pulse.cdm.properties.SEScalarVolumePerTime;

public abstract class SEFluidCompartment extends SECompartment
{
  protected SEScalarVolumePerTime inflow;
  protected SEScalarVolumePerTime outflow;
  protected SEScalarPressure      pressure;  
  protected SEScalarVolume        volume;
  
  public SEFluidCompartment()
  {
    super();
    
  }
  
  @Override
  public void clear()
  {
    super.clear();
    if (pressure != null)
      pressure.invalidate();
    if (inflow != null)
      inflow.invalidate();
    if (outflow != null)
      outflow.invalidate();
    if (volume != null)
      volume.invalidate();
  }
  
  public static void load(FluidCompartmentData src, SEFluidCompartment dst)
  {
    SECompartment.load(src.getCompartment(),dst);
    if (src.hasPressure()) 
       SEScalarPressure.load(src.getPressure(),dst.getPressure()); 
    if (src.hasInflow()) 
      SEScalarVolumePerTime.load(src.getInflow(),dst.getInflow()); 
    if (src.hasOutflow()) 
      SEScalarVolumePerTime.load(src.getOutflow(),dst.getOutflow()); 
    if (src.hasVolume()) 
      SEScalarVolume.load(src.getVolume(),dst.getVolume()); 
  }
  protected static void unload(SEFluidCompartment src, FluidCompartmentData.Builder dst)
  {
    SECompartment.unload(src,dst.getCompartment());
    if (src.hasPressure())
      dst.setPressure(SEScalarPressure.unload(src.pressure));
    if (src.hasInflow())
      dst.setInflow(SEScalarVolumePerTime.unload(src.inflow));
    if (src.hasOutflow())
      dst.setOutflow(SEScalarVolumePerTime.unload(src.outflow));
    if (src.hasVolume())
      dst.setVolume(SEScalarVolume.unload(src.volume));
  }
  
  public SEScalarVolumePerTime getInflow() 
  {
    if (inflow == null)
      inflow = new SEScalarVolumePerTime();
    return inflow;
  }
  public boolean hasInflow()
  {
    return inflow == null ? false : inflow.isValid();
  }
  
  public SEScalarVolumePerTime getOutflow() 
  {
    if (outflow == null)
      outflow = new SEScalarVolumePerTime();
    return outflow;
  }
  public boolean hasOutflow()
  {
    return outflow == null ? false : outflow.isValid();
  }
  
  public SEScalarPressure getPressure() 
  {
    if (pressure == null)
      pressure = new SEScalarPressure();
    return pressure;
  }
  public boolean hasPressure()
  {
    return pressure == null ? false : pressure.isValid();
  }
  
  public SEScalarVolume getVolume() 
  {
    if (volume == null)
      volume = new SEScalarVolume();
    return volume;
  }
  public boolean hasVolume()
  {
    return volume == null ? false : volume.isValid();
  }
}
