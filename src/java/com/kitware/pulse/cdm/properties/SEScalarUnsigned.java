/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package com.kitware.pulse.cdm.properties;

import com.kitware.pulse.cdm.bind.Properties.ScalarUnsignedData;
import com.kitware.pulse.utilities.Log;

/**
 * @author abray
 * Unsigned Scalar Class, will enforce that value is >= 0
 */

public class SEScalarUnsigned extends SEScalar
{
  public SEScalarUnsigned()
  {
    super();
  }
  
  /**
   * @param value
   * @param unit - specific any valid standard unit abbreviation
   * http://www.bipm.org/en/si/ for this quantity type
   */
  public SEScalarUnsigned(double value)
  {
    this();
    this.setValue(value, "");
  }
  
  public static void load(ScalarUnsignedData src, SEScalarUnsigned dst)
  {
    SEScalar.load(src.getScalarUnsigned(),dst);
  }
  public static ScalarUnsignedData unload(SEScalarUnsigned src)
  {
    if(!src.isValid())
      return null;
    ScalarUnsignedData.Builder dst = ScalarUnsignedData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(SEScalarUnsigned src, ScalarUnsignedData.Builder dst)
  {
    SEScalar.unload(src,dst.getScalarUnsignedBuilder());
  }

  @Override
  public boolean validUnit(String unit)
  {
    if(unit==null||unit.isEmpty()||unit.compareTo("unitless") == 0)
      return true;
    return false;
  }
  
  @Override
  public void setValue(double d)
  {
    if(d<0)
    {
      d=0;
      Log.warn("Attempting to set 0-1 Scalar to less than 0, bounding to 0");
    }
    this.value = d;
  }
}
