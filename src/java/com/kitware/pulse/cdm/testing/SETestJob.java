package com.kitware.pulse.cdm.testing;

import java.util.ArrayList;
import java.util.List;

import com.kitware.pulse.engine.PulseScenarioExec;
import com.kitware.pulse.utilities.Log;
import com.kitware.pulse.utilities.LogListener;
import com.kitware.pulse.utilities.PythonUtils;
import com.kitware.pulse.utilities.csv.plots.CSVComparePlotter.PlotType;

public class SETestJob extends LogListener
{
  public enum State {Provisioned, Executing, Executed, Comparing, Complete}
  
  public SETestJob(){ listen(false); }
  public boolean               useState=false;
  public PulseScenarioExec     execOpts=new PulseScenarioExec();
  public String                patientFile = null;
  public State                 state=State.Provisioned;
  public String                name;
  public boolean               skipExecution = false;
  public boolean               skipPlot = false;
  public PythonUtils           python = null;
  public boolean               knownFailure = false;
  public SETestDriver.Executor executor = null;
  public boolean               isValidation = false;
  public boolean               plottableResults = false;
  public PlotType              plotType = PlotType.FastPlot;//Only plot every nth data point
  public double                percentDifference;
  public List<String>          scenarioFiles = new ArrayList<>();
  public String                scenarioDirectory = null;
  public List<String>          baselineFiles = new ArrayList<>();
  public String                baselineDirectory = null;
  public List<String>          computedFiles = new ArrayList<>();
  public String                computedDirectory = null;
  public List<String>          reportFiles = new ArrayList<>();
  public String                resultsFiles = null;

  @Override
  public SETestJob clone()
  {
    SETestJob copy = new SETestJob();
    copy.isValidation = this.isValidation;
    copy.useState = this.useState;
    copy.patientFile = this.patientFile;
    copy.state = this.state;
    copy.name = this.name;
    copy.skipExecution = this.skipExecution;
    copy.skipPlot = this.skipPlot;
    copy.knownFailure = this.knownFailure;
    copy.executor = this.executor;
    copy.python = this.python;
    copy.plottableResults = this.plottableResults;
    copy.plotType = this.plotType;
    copy.percentDifference = this.percentDifference;
    copy.scenarioFiles.addAll(this.scenarioFiles);
    copy.scenarioDirectory = this.scenarioDirectory;
    copy.baselineFiles.addAll(this.baselineFiles);
    copy.baselineDirectory = this.baselineDirectory;
    copy.computedFiles.addAll(this.computedFiles);
    copy.computedDirectory = this.computedDirectory;
    copy.reportFiles.addAll(this.reportFiles);
    copy.resultsFiles = this.resultsFiles;
    return copy;
  }

  @Override
  public void handleDebug(String msg) { Log.debug("["+name+"] "+msg); }
  @Override
  public void handleInfo(String msg)  { Log.info("["+name+"] "+msg); }
  @Override
  public void handleWarn(String msg)  { Log.warn("["+name+"] "+msg); }
  @Override
  public void handleError(String msg) { Log.error("["+name+"] "+msg); }
  @Override
  public void handleFatal(String msg) { Log.fatal("["+name+"] "+msg); }
}
