package com.kitware.pulse.cdm.testing;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import com.kitware.pulse.cdm.bind.TestReport.TestCaseData;
import com.kitware.pulse.cdm.bind.TestReport.TestSuiteData;
import com.kitware.pulse.cdm.properties.CommonUnits.TimeUnit;
import com.kitware.pulse.utilities.SEEqualOptions;

public class SETestSuite
{
	protected String             name;
	protected boolean            performed;
	protected List<String>       requirements = new ArrayList<>();
	protected List<SETestCase>   testCases = new ArrayList<>();

	protected SETestCase         activeCase;
	protected SETestCaseListener activeCaseListener = new SETestCaseListener();

	protected SETestSuite()
	{
	  clear();
	}

	public void clear()
	{
		this.name=null;
		this.performed=true;
		this.requirements.clear();
		this.testCases.clear();
		this.activeCase = null;
		this.activeCaseListener.clear();
	}

	public static void load(TestSuiteData src, SETestSuite dst)
  {
    dst.clear();
    dst.setName(src.getName());
    dst.setPerformed(src.getPerformed());
    for(String req : src.getRequirementList())
    	dst.requirements.add(req);
    for(TestCaseData tcd : src.getTestCaseList())
    {
    	SETestCase tc = dst.createTestCase();
    	SETestCase.load(tcd, tc);
    }
  }

	public static TestSuiteData unload(SETestSuite src)
  {
		TestSuiteData.Builder dst = TestSuiteData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(SETestSuite src, TestSuiteData.Builder dst)
  {
    if(src.hasName())
      dst.setName(src.name);
    dst.setPerformed(src.performed);
    dst.setErrors(src.getNumErrors());
    dst.setWarnings(src.getNumWarnings());
    dst.setTests(src.testCases.size());

    for(String req : src.requirements)
    	dst.addRequirement(req);

    for(SETestCase tc : src.testCases)
    	dst.addTestCase(SETestCase.unload(tc));
  }

  public String  getName() { return this.name;}
  public void    setName(String name){this.name=name;}
  public boolean hasName(){return this.name==null?false:true;}

  public boolean getPerformed() { return this.performed;}
  public void    setPerformed(boolean b){this.performed=b;}

  public void AddRequirement(String f){requirements.add(f);}
  public List<String> getRequirements(){ return Collections.unmodifiableList(this.requirements); }

  public SETestCase createTestCase()
  {
  	SETestCase tc = new SETestCase();
  	this.testCases.add(tc);
  	return tc;
  }
  public List<SETestCase> getTestCases()
  {
  	return Collections.unmodifiableList(this.testCases);
  }
  public SEEqualOptions getCaseEqualOptions()
	{
		if(activeCase==null)
			return null;
		return activeCase.getEqualOptions();
	}
  public SETestCase startCase(String name)
  {
  	activeCase = createTestCase();
  	activeCase.setName(name);
  	activeCaseListener.setTestCase(activeCase);
  	return activeCase;
  }
  public SETestCase getActiveCase()
  {
  	return activeCase;
  }
  public void endCase()
  {
  	activeCaseListener.clear();
  }
  public int getNumErrors()
  {
  	int errs=0;
  	for(SETestCase tc : this.testCases)
  		if(!tc.failures.isEmpty())
  			errs++;
  	return errs;
  }
  public int getNumWarnings()
  {
  	int warns=0;
  	for(SETestCase tc : this.testCases)
  		if(!tc.warnings.isEmpty())
  			warns++;
  	return warns;
  }
  public double getDuration(TimeUnit unit)
  {
  	double time=0;
  	for(SETestCase tc : this.testCases)
  		time += tc.getDuration().getValue(unit);
  	return time;
  }
}
