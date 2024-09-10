/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

/**
 *
 */
package com.kitware.pulse.cdm.testing;

import com.kitware.pulse.utilities.LogListener;

/**
 * @author abray
 * Simple class to listen to any status messages
 * from the calculation and add them to the report
 */
public class SETestCaseListener extends LogListener
{
  private SETestCase testCase=null;

  public SETestCaseListener()
  {

  }

  public void clear()
  {
    this.testCase=null;
  }

  public void setTestCase(SETestCase tc)
  {
    this.testCase=tc;
  }

  @Override
  public void handleError(String msg)
  {
    if (testCase != null)
    {
    	testCase.AddFailure(msg);
    }
  }

  @Override
  public void handleFatal(String msg)
  {
  	if (testCase != null)
    {
    	testCase.AddFailure(msg);
    }
  }

  @Override
  protected void handleDebug(String msg) {}

  @Override
  protected void handleInfo(String msg) {}

  @Override
  protected void handleWarn(String msg) {
    if (testCase != null)
    {
      testCase.AddWarning(msg);
    }
  }
}
